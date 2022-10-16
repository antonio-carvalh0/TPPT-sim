#include "PesGenerationMode.hh"
#include "SessionManager.hh"
#include "StackingAction.hh"
#include "out.hh"
#include "jstools.hh"

#include "G4Material.hh"
#include "G4Track.hh"
#include "G4RandomTools.hh"
#include "Randomize.hh"

PesGenerationMode::PesGenerationMode(int numEvents, const std::string & outputFileName, bool binaryOutput) :
    SimModeBase(), NumEvents(numEvents)
{
    SessionManager & SM = SessionManager::getInstance();

    loadCrossSections("ProductionCrossSections.txt");
    //loadCrossSections(SM.WorkingDirectory + "/SecretFile.txt");

    //bNeedGui    = true; // used only for tests!

    bNeedOutput = true;
    SM.FileName   = outputFileName;
    SM.bBinOutput = binaryOutput;
    SaveDirection[0] = 0; SaveDirection[1] = 0; SaveDirection[2] = 1.0;

    // interpolation test
    //    PesGenRecord r(1,1,"");
    //    r.CrossSection = { {0, 1},{1, 2},{2, 3},{3, 4},{5, 5},{10, 6} };
    //    out(r.getCrossSection(0));
    //    out(r.getCrossSection(100));
    //    out(r.getCrossSection(0.5));
    //    out(r.getCrossSection(6));
    //    exit(0);
}

void PesGenerationMode::loadCrossSections(const std::string & fileName)
{
    std::ifstream inStream(fileName);
    if (!inStream.is_open())
    {
        out("Cannot open file with PES generation cross-sections:\n", fileName);
        exit(1);
    }

    bool fillingRecord = false;
    PesGenRecord currentRecord;
    for (std::string line; std::getline(inStream, line); )
    {
        //out(">>>",line);
        if (line.empty()) continue; //allow empty lines

        if (line[0] == '#')
        {
            //new reaction
            if (fillingRecord) BaseRecords.push_back(currentRecord);

            line.erase(0, 1);
            std::stringstream ss(line);
            ss >> currentRecord.TargetZ
               >> currentRecord.TargetA
               >> currentRecord.PES
               >> currentRecord.DecayTime;
            if (ss.fail())
            {
                out("Unexpected format of a reaction line in the file with the PES cross-sections");
                exit(2);
            }
            currentRecord.DecayTime /= log(2.0); // half-life to decay time

            currentRecord.CrossSection.clear();
            fillingRecord = true;
            //out("-->Processing reaction:",currentRecord.TargetZ, currentRecord.TargetA, currentRecord.PES);
        }
        else
        {
            std::stringstream ss(line);  // units in the file are MeV and mbarns
            double E, CS;
            ss >> E >> CS;
            if (ss.fail())
            {
                out("Unexpected format of a data line in the file with the PES cross-sections");
                exit(3);
            }
            //out(E, CS);
            currentRecord.CrossSection.push_back({E*MeV, CS});
        }
    }
    if (!currentRecord.CrossSection.empty()) BaseRecords.push_back(currentRecord);

    out("\n\n===== PES production cross-section summary ====");
    out("Number of reactions:", BaseRecords.size());
    for (const auto & r : BaseRecords)
        out(">", r.TargetZ, r.TargetA, r.PES, "  CS range from:", r.CrossSection.front().first, "to", r.CrossSection.back().first, "MeV");
    out("===============================================\n\n");
}

#include "TrackingAction.hh"
G4UserTrackingAction * PesGenerationMode::getTrackingAction()
{
    return new PesGeneratorTrackingAction();
}

G4UserStackingAction * PesGenerationMode::getStackingAction()
{
    return new PesGeneratorStackingAction();
}

void PesGenerationMode::preInit()
{
    SessionManager::getInstance().FastPESGeneration = true;
}

#include "G4MTRunManager.hh"
void PesGenerationMode::run()
{
    SessionManager& SM = SessionManager::getInstance();

    exploreMaterials();

    // this sub-mode is just to debug!
    if (bNeedGui)
    {
        SM.startGUI();
        return;
    }

    SM.runManager->BeamOn(NumEvents);
}

void PesGenerationMode::exploreMaterials()
{
    MaterialRecords.clear();

    const G4MaterialTable * theMaterialTable = G4Material::GetMaterialTable();
    const size_t numMat = theMaterialTable->size();
    MaterialRecords.resize(numMat);
    out("Defined", numMat, "materials");

    for (size_t iMat = 0; iMat < numMat; iMat++)
    {
        out("---------");
        const G4Material * mat = (*theMaterialTable)[iMat];
        out(mat->GetName(), " Index:", mat->GetIndex());
        const G4ElementVector * ev = mat->GetElementVector();
        for (size_t iEl = 0; iEl < ev->size(); iEl++)
        {
            const G4Element * element = ev->at(iEl);
            double numberDensity = mat->GetVecNbOfAtomsPerVolume()[iEl]; // mm-3
            out(element->GetName(), " Atomic density:", numberDensity, "mm-3");

            const G4IsotopeVector * isoVec = element->GetIsotopeVector();
            double * relAbVec = element->GetRelativeAbundanceVector();
            for (size_t iIs = 0; iIs < isoVec->size(); iIs++)
            {
                const G4Isotope * is = isoVec->at(iIs);
                int z = is->GetZ();
                int a = is->GetN();
                double frac = relAbVec[iIs];
                double PartialNumDens = numberDensity * frac;
                out("  -> Z =", z, "A =", a, "Fraction =", frac, "PartNumDens = ", PartialNumDens);

                updateMatRecords(iMat, z, a, PartialNumDens);
            }
        }
    }
}

void PesGenerationMode::updateMatRecords(int iMat, int Z, int A, double IsotopeNumberDensity)
{
    for (const PesGenRecord & rec : BaseRecords)
    {
        if (rec.TargetZ != Z || rec.TargetA != A) continue;

        out("    ==> Adding PES generation record!");
        PesGenRecord thisRec = rec; // note: it has a pointer to ProbArray
        thisRec.NumberDensity = IsotopeNumberDensity;
        MaterialRecords[iMat].push_back(thisRec);
    }
}

void PesGenerationMode::saveRecord(const std::string & Pes, double X, double Y, double Z, double Time) const
{
    SessionManager & SM = SessionManager::getInstance();

    if (SM.bBinOutput)
    {
        *SM.outStream << (char)0xFF;
        *SM.outStream << Pes << (char)0x00;
        SM.outStream->write((char*)&SaveEnergy,     sizeof(double));
        SM.outStream->write((char*)&X,              sizeof(double));
        SM.outStream->write((char*)&Y,              sizeof(double));
        SM.outStream->write((char*)&Z,              sizeof(double));
        SM.outStream->write((char*)SaveDirection, 3*sizeof(double));
        SM.outStream->write((char*)&Time,           sizeof(double));
    }
    else
    {
        std::stringstream ss;
        ss << Pes << ' ';
        ss << SaveEnergy << ' ';
        ss << X << ' ' << Y << ' ' << Z << ' ';
        ss << SaveDirection[0] << ' ' << SaveDirection[1] << ' ' << SaveDirection[2] << ' ';
        ss << Time;

        *SM.outStream << ss.rdbuf() << '\n';
    }

    //out("->",Pes, "(",X,Y,Z,")", Time);
}

void PesGenerationMode::readFromJson(const json11::Json & json)
{
    jstools::readInt(json, "NumEvents",   NumEvents);

    SessionManager & SM = SessionManager::getInstance();
    jstools::readString(json, "OutputFileName", SM.FileName);
    jstools::readBool  (json, "BinaryOutput",   SM.bBinOutput);
}

void PesGenerationMode::doWriteToJson(json11::Json::object & json) const
{
    json["NumEvents"] = NumEvents;

    SessionManager & SM = SessionManager::getInstance();
    json["OutputFileName"] = SM.FileName;
    json["BinaryOutput"]   = SM.bBinOutput;
}

void PesGenerationMode::onEventStarted()
{
    SessionManager & SM = SessionManager::getInstance();
    if (SM.bBinOutput)
    {
        *SM.outStream << char(0xEE);
        SM.outStream->write((char*)&CurrentEvent, sizeof(int));
    }
    else
        *SM.outStream << '#' << CurrentEvent << '\n';

    CurrentEvent++;
}

bool PesGenerationMode::modelTrigger(const G4Track * track)
{
    //const int StepNumber = track->GetCurrentStepNumber();
    //out("PES call", StepNumber, bNewTrackStarted); // cannot use the step number: proton can be created outside the phantom region which is invisible for this call

    if (bNewTrackStarted)
    {
        bNewTrackStarted = false;
        LastEnergy      = track->GetKineticEnergy();
        LastTrackLength = track->GetTrackLength();
        LastPosition    = track->GetPosition();
        LastMaterial    = track->GetMaterial()->GetIndex();
        return false;
    }

    const double          Energy   = track->GetKineticEnergy();
    const double          Length   = track->GetTrackLength();
    const G4ThreeVector & Position = track->GetPosition();

    if (LastEnergy > Energy) doTrigger(track);

    LastEnergy      = Energy;
    LastTrackLength = Length;
    LastPosition    = Position;
    LastMaterial    = track->GetMaterial()->GetIndex();
    return false;
}

bool PesGenerationMode::doTrigger(const G4Track * track)
{
    const double stepLength = track->GetTrackLength() - LastTrackLength;
    const double meanEnergy = 0.5 * (track->GetKineticEnergy() + LastEnergy);
    //out("Step", stepLength, "MeanEenergy", meanEnergy, " Material index", LastMaterial);

    const std::vector<PesGenRecord> & Records = MaterialRecords[LastMaterial];
    if (!Records.empty())
    {
        //probability is proportional to CS * NumberDensity
        ProbVec.clear(); ProbVec.reserve(Records.size());
        double sumProb = 0;
        for (const PesGenRecord & r : Records)
        {
            const double cs = r.getCrossSection(meanEnergy);
            const double relProb = cs * r.NumberDensity;
            sumProb += relProb;
            ProbVec.push_back(relProb);
        }

        if (sumProb > 0)
        {
            size_t index = 0;

            if (ProbVec.size() > 1)
            {
                // selecting the reaction
                double val = sumProb * G4UniformRand();
                //out("Probs:",ProbVec.size(),"Sum:",sumProb,"Random:",val);
                for (; index+1 < ProbVec.size(); index++)
                {
                    if (val < ProbVec[index]) break;
                    val -= ProbVec[index];
                }
                //out("-->Selected:",index);
            }

            const double mfp = 1e25 / sumProb; // millibarn = 0.001e-28m2 -> 0.001e-22mm2 -> 1e-25 mm2

            const double trigStep = -mfp * log(G4UniformRand());
            if (trigStep < stepLength)
            {
                G4ThreeVector TriggerPosition = LastPosition + trigStep/stepLength*(track->GetPosition() - LastPosition);
                //out("Triggered! Position:", TriggerPosition, "  Last/FullStep positions:", LastPosition, Position);
                saveRecord(Records[index].PES, TriggerPosition[0], TriggerPosition[1], TriggerPosition[2], track->GetGlobalTime());
                return true;
            }
        }
    }
    return false;
}

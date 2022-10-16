#include "PesProbabilityMode.hh"
#include "SessionManager.hh"
#include "StackingAction.hh"
#include "out.hh"
#include "jstools.hh"

#include "G4Material.hh"
#include "G4Track.hh"
#include "G4RandomTools.hh"
#include "Randomize.hh"

PesProbabilityMode::PesProbabilityMode(int numEvents, std::array<double, 3> binSize, std::array<int, 3> numBins, std::array<double, 3> origin,
                                       const std::vector<std::pair<double,double>> & acquisitionFromTos) :
    PesGenerationMode(numEvents, "dummy.txt", false),
    BinSize(binSize), NumBins(numBins), Origin(origin),
    TimeWindows(acquisitionFromTos)
{
    bNeedOutput = false;

    initProbArrays();

    // tests for voxel finding algorithms
    //    G4ThreeVector v(0, 2.6, -1.2);
    //    int index[3];
    //    bool ok = getVoxel(v, index);
    //    out(ok, index[0], index[1], index[2]);
    //    exit(1);

    //    std::vector<std::tuple<int, int, int, double>> Path;
    //    addPath({0,1.2,1}, {2,1,1}, Path);
    //    //addPath({1,1,1}, {2.2,1,1}, Path);
    //    //addPath({1.5,1.8,1}, {-2.5,2.1,1}, Path);
    //    for (size_t i = 0; i < Path.size(); i++)
    //        out(std::get<0>(Path[i]), std::get<1>(Path[i]), std::get<2>(Path[i]), std::get<3>(Path[i])); // --> 1 1 1 0.282843
    //    exit(1);

    //    std::vector<std::tuple<int, int, int, double>> Path;
    //    addPathA({0,0,0}, {2.0,-1.0,0}, Path);
    //    //addPathA({-2.0,-1.0,0}, {0,-2.0,1.0}, Path);
    //    //addPathA({0,0,0}, {2,-2,0}, Path);
    //    for (size_t i = 0; i < Path.size(); i++)
    //        out(std::get<0>(Path[i]), std::get<1>(Path[i]), std::get<2>(Path[i]), std::get<3>(Path[i])); // --> 1 1 1 0.282843
    //    exit(1);
}

void PesProbabilityMode::initProbArrays()
{
    for (PesGenRecord & r : BaseRecords)
    {
        r.ProbArray = new std::vector<std::vector<std::vector<double>>>();
        // X
        r.ProbArray->resize(NumBins[0]);
        for (std::vector<std::vector<double>> & ary : *r.ProbArray)
        {
            // Y
            ary.resize(NumBins[1]);
            for (std::vector<double> & arz : ary)
            {
                // Z
                arz.resize(NumBins[2], 0);
            }
        }
    }
}

void PesProbabilityMode::run()
{
    PesGenerationMode::run();
    saveArrays();
}

void PesProbabilityMode::readFromJson(const json11::Json & json)
{
    jstools::readInt(json, "NumEvents",   NumEvents);

    // !!!*** control array sizes = 3
    // BinSize
    {
        json11::Json::array ar;
        jstools::readArray(json, "BinSize", ar);
        for (int i = 0; i < 3; i++) BinSize[i] = ar[i].number_value();
    }
    // NumBins
    {
        json11::Json::array ar;
        jstools::readArray(json, "NumBins", ar);
        for (int i = 0; i < 3; i++) NumBins[i] = ar[i].int_value();
    }
    // Origin
    {
        json11::Json::array ar;
        jstools::readArray(json, "Origin", ar);
        for (int i = 0; i < 3; i++) Origin[i] = ar[i].number_value();
    }

    bNeedOutput = false;
    initProbArrays();
}

void PesProbabilityMode::doWriteToJson(json11::Json::object & json) const
{
    json["NumEvents"] = NumEvents;

    // BinSize
    {
        json11::Json::array ar;
        for (int i = 0; i < 3; i++) ar.push_back(BinSize[i]);
        json["BinSize"] = ar;
    }
    // NumBins
    {
        json11::Json::array ar;
        for (int i = 0; i < 3; i++) ar.push_back(NumBins[i]);
        json["NumBins"] = ar;
    }
    // Origin
    {
        json11::Json::array ar;
        for (int i = 0; i < 3; i++) ar.push_back(Origin[i]);
        json["Origin"] = ar;
    }
}

bool PesProbabilityMode::getVoxel(const G4ThreeVector & pos, int * index)
{
    for (int i = 0; i < 3; i++)
        index[i] = floor( (pos[i] - Origin[i]) / BinSize[i] );

    for (int i = 0; i < 3; i++)
        if ( index[i] < 0 || index[i] >= NumBins[i]) return false;
    return true;
}

void PesProbabilityMode::addPath(const G4ThreeVector & posFrom, const G4ThreeVector & posTo, std::vector<std::tuple<int, int, int, double>> & path)
{
    // check maybe start and finish are in the same voxel
    int From[3];
    bool ok1 = getVoxel(posFrom, From);
    int To[3];
    bool ok2 = getVoxel(posTo, To);
    if (ok1 && ok2 && From[0] == To[0] && From[1] == To[1] && From[2] == To[2])
    {
        path.push_back( {From[0], From[1], From[2], (posTo-posFrom).mag()} );
        return;
    }

    // general
    const G4ThreeVector vector = posTo - posFrom;
    const double distance = vector.mag();
    const int numSteps = 1 + distance * 10.0; // 0.1 mm step
    const double dStep = distance / numSteps;
    const double factor = 1.0/numSteps;
    for (int iStep = 0; iStep < numSteps; iStep++)
    {
        G4ThreeVector curPos = posFrom + vector * factor * iStep;
        bool ok = getVoxel(curPos, From);
        if (ok) path.push_back( {From[0], From[1], From[2], dStep} );
    }
}

void PesProbabilityMode::addPathA(const G4ThreeVector & posFrom, const G4ThreeVector & posTo, std::vector<std::tuple<int, int, int, double>> & path)
{
    int From[3];
    bool ok1 = getVoxel(posFrom, From);
    int To[3];
    bool ok2 = getVoxel(posTo, To);
    const double totLength = (posTo-posFrom).mag();
    //out("Total step length:", totLength);
    if (ok1 && ok2 && From[0] == To[0] && From[1] == To[1] && From[2] == To[2])
    {
        path.push_back( {From[0], From[1], From[2], totLength} );
        return;
    }

    std::vector< std::tuple<double, int, int> > store; // progress factor (0->1), axis index (0,1,2) and current index along this axis
    for (int iAxis = 0; iAxis < 3; iAxis++)
    {
        //out(iAxis, "> from->to:", From[iAxis], To[iAxis]);
        int iv = From[iAxis];
        const int step = (posTo[iAxis] > posFrom[iAxis] ? 1 : -1);
        while (iv != To[iAxis])
        {
            //if (iv >= 0 && iv < NumBins[iAxis])
            //{
                const double k = (BinSize[iAxis]*(iv + (step == 1 ? 1 : 0)) + Origin[iAxis] - posFrom[iAxis]) / (posTo[iAxis] - posFrom[iAxis]);  // from 0 to 1.0
                store.push_back( {k, iAxis, iv + step} );
            //}
            iv += step;
        }
    }

    std::sort(store.begin(), store.end(),
              [](const std::tuple<double, int, int> & lhs, const std::tuple<double, int, int> & rhs)
                {return (std::get<0>(lhs) < std::get<0>(rhs));}  );

//    out("Store:");
//    for (size_t i = 0; i < store.size(); i++)
//        out("  ", std::get<0>(store[i]), std::get<1>(store[i]), std::get<2>(store[i]));
//    out("");

    // starting from "From" and updating indexes there
    double length = 0;
    double newK   = 0;
    for (const auto & rec : store)
    {
        newK = std::get<0>(rec);
        if (isValidVoxel(From))
            path.push_back( {From[0], From[1], From[2], (newK - length) * totLength} );
        length = newK;
        From[std::get<1>(rec)] = std::get<2>(rec);
    }
    if (isValidVoxel(From))
        path.push_back( {From[0], From[1], From[2], (1.0 - newK) * totLength} );
}

double PesProbabilityMode::calculateTimeFactor(double t0, double decayTime)
{
    double timeFactor = 0;

    for (size_t i = 0; i < TimeWindows.size(); i++)
    {
        double timeTo = TimeWindows[i].second - t0;
        if (timeTo  <= 0) continue;
        double timeFrom = TimeWindows[i].first - t0;
        if (timeFrom < 0) timeFrom = 0;

        const double from = exp(-timeFrom/decayTime);
        const double to   = exp(-timeTo/decayTime);
        const double delta = from - to;
        timeFactor += delta;
    }

    //out("Time factor:",timeFactor);
    return timeFactor;
}

bool PesProbabilityMode::isValidVoxel(int * coords) const
{
    for (int i = 0; i < 3; i++)
        if (coords[i] < 0 || coords[i] >= NumBins[i]) return false;
    return true;
}

bool PesProbabilityMode::doTrigger(const G4Track * track)
{
    const std::vector<PesGenRecord> & Records = MaterialRecords[LastMaterial];
    if (Records.empty()) return false;

    std::vector<std::tuple<int, int, int, double>> Path;
    addPathA(LastPosition, track->GetPosition(), Path);

    const double meanEnergy = 0.5 * (track->GetKineticEnergy() + LastEnergy);
    for (const PesGenRecord & r : Records)
    {
        const double cs = r.getCrossSection(meanEnergy);
        const double DProbPerMM = 1e-25 * cs * r.NumberDensity; // millibarn = 0.001e-28m2 -> 0.001e-22mm2 -> 1e-25 mm2
        const double timeFractionInWindows = calculateTimeFactor(track->GetGlobalTime()/s, r.DecayTime);

        for (size_t i = 0; i < Path.size(); i++)
            (*r.ProbArray)[std::get<0>(Path[i])][std::get<1>(Path[i])][std::get<2>(Path[i])] += std::get<3>(Path[i]) * DProbPerMM * timeFractionInWindows;
    }

    return false;
}

void PesProbabilityMode::saveArrays()
{
    SessionManager & SM = SessionManager::getInstance();
    for (PesGenRecord & r : BaseRecords)
    {
        std::string fileName = std::to_string(r.TargetZ) + '_' + std::to_string(r.TargetA) + '_' + r.PES + ".txt";
        std::string fullFileName = SM.WorkingDirectory + '/' + fileName;

        std::ofstream stream;
        stream.open(fullFileName);

        if (!stream.is_open())
        {
            out("Cannot open file to store output data!");
            outFlush();
            exit(1);
        }
        else out("\nSaving array data to file", fullFileName);

        json11::Json::object json;
        doWriteToJson(json);
        json11::Json aa(json);
        std::string str = '#' + aa.dump();
        stream << str << '\n';

        for (const std::vector<std::vector<double>> & ary : *r.ProbArray)
        {
            for (const std::vector<double> & arz : ary)
            {
                for (const double & val : arz) stream << val << ' ';
                stream << '\n';
            }
        }
    }
}

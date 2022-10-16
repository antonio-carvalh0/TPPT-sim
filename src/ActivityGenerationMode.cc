#include "ActivityGenerationMode.hh"
#include "SessionManager.hh"
#include "out.hh"
#include "jstools.hh"

#include "G4MTRunManager.hh"

ActivityGenerationMode::ActivityGenerationMode(int numEvents,
                                               std::array<double, 3> binSize, std::array<int, 3> numBins, std::array<double, 3> origin,
                                               const std::vector<std::pair<double,double>> & acquisitionFromTos,
                                               const std::string & fileName) :
    PesProbabilityMode(numEvents, binSize, numBins, origin, acquisitionFromTos),
    FileName(fileName)
{
    initActivityArray();
}

void ActivityGenerationMode::initActivityArray()
{
    // X
    Activity.resize(NumBins[0]);
    for (std::vector<std::vector<double>> & ary : Activity)
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

void ActivityGenerationMode::run()
{
    PesGenerationMode::run(); // PesGenerationMode indeed
    saveData();
}

void ActivityGenerationMode::readFromJson(const json11::Json & json)
{
    PesProbabilityMode::readFromJson(json);
    initActivityArray();

    TimeWindows.clear();
    json11::Json::array ar;
    jstools::readArray(json, "TimeWindows", ar);
    for (size_t i = 0; i < ar.size(); i++)
    {
        json11::Json::array el = ar[i].array_items();
        TimeWindows.push_back( {el[0].number_value(), el[1].number_value()} );
    }

    jstools::readString(json, "FileName", FileName);
}

void ActivityGenerationMode::doWriteToJson(json11::Json::object & json) const
{
    PesProbabilityMode::doWriteToJson(json);

    json11::Json::array ar;
    for (const auto & p : TimeWindows)
    {
        json11::Json::array el;
            el.push_back(p.first);
            el.push_back(p.second);
        ar.push_back(el);
    }
    json["TimeWindows"] = ar;
    json["FileName"] = FileName;
}

bool ActivityGenerationMode::doTrigger(const G4Track *track)
{
    const std::vector<PesGenRecord> & Records = MaterialRecords[LastMaterial];
    if (Records.empty()) return false;

    std::vector<std::tuple<int, int, int, double>> Path;
    addPathA(LastPosition, track->GetPosition(), Path);
    //out("Path length:", Path.size());
    if (Path.empty()) return false;

    const double meanEnergy = 0.5 * (track->GetKineticEnergy() + LastEnergy);
    for (const PesGenRecord & r : Records)
    {
        const double cs = r.getCrossSection(meanEnergy);
        const double DProbByMM = 1e-25 * cs * r.NumberDensity; // millibarn = 0.001e-28m2 -> 0.001e-22mm2 -> 1e-25 mm2
        const double timeFractionInWindows = calculateTimeFactor(track->GetGlobalTime()/s, r.DecayTime);

        for (size_t i = 0; i < Path.size(); i++)
        {
            const double numPES = std::get<3>(Path[i]) * DProbByMM;
            const double decays = numPES * timeFractionInWindows;
            //out(decays);

            Activity[std::get<0>(Path[i])][std::get<1>(Path[i])][std::get<2>(Path[i])] += decays;
        }
    }

    return false;
}

void ActivityGenerationMode::saveData()
{
    SessionManager& SM = SessionManager::getInstance();
    std::string fullFileName = SM.WorkingDirectory + '/' + FileName;

    // save resulting 3D array of activity
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
    PesProbabilityMode::doWriteToJson(json);
    json11::Json aa(json);
    std::string str = '#' + aa.dump();
    stream << str << '\n';

    for (const std::vector<std::vector<double>> & ary : Activity)
    {
        for (const std::vector<double> & arz : ary)
        {
            for (const double & val : arz) stream << val << ' ';
            stream << '\n';
        }
    }

    stream.close();
}

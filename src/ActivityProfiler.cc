#include "ActivityProfiler.hh"
#include "out.hh"
#include "SessionManager.hh"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

ActivityProfilerMode::ActivityProfilerMode(const std::vector<BeamTimeWindow> & beamStructure, const std::vector<ScanTimeWindow> & scanWindows,
                                           const std::string & dataDirectory, const std::string & baseFileName) :
    SimModeBase(),
    BeamTimeWindows(beamStructure), ScanTimeWindows(scanWindows),
    DataDirectory(dataDirectory), BaseFileName(baseFileName)
{
    IsotopeBase.push_back({ "C11", 1220.41518, {"6_12_C11.txt", "8_16_C11.txt"} });
    IsotopeBase.push_back({ "O15", 122.26643,  {"8_16_O15.txt"} });
    IsotopeBase.push_back({ "N13", 597.9,      {"8_16_N13.txt"} });

    SumBeamFactors = 0;
    for (const BeamTimeWindow & win : BeamTimeWindows)
        SumBeamFactors += (win.To - win.From) * win.Weight;
}

void ActivityProfilerMode::checkInputData()
{
    for (auto it = IsotopeBase.begin(); it < IsotopeBase.end(); ++it)
    {
        for (auto itOther = IsotopeBase.begin(); itOther < IsotopeBase.end(); ++itOther)
        {
            if (it == itOther) continue;
            if (it->Isotope == itOther->Isotope)
            {
                out("Found non-unique record for PES:", it->Isotope);
                exit(3);
            }
        }
    }

    for (const BeamTimeWindow & btw : BeamTimeWindows)
    {
        if (btw.Weight <= 0)
        {
            out("Beam time window weight factor should be > 0");
            exit(3);
        }
        if (btw.From > btw.To)
        {
            out("Beam time window 'To' should not be smaller than 'From'");
            exit(3);
        }
    }

    for (const ScanTimeWindow & stw : ScanTimeWindows)
    {
        if (stw.From > stw.To)
        {
            out("Scan time window 'To' should not be smaller than 'From'");
            exit(3);
        }
    }
}

void ActivityProfilerMode::run()
{
    out("\n\n");
    checkInputData(); // calls exit(3) if there are problems

    std::vector<double> arAll1D;
    std::vector<std::vector<double>> arAll2D;

    std::vector<double> arIso1D;
    std::vector<std::vector<double>> arIso2D;

    //std::vector<std::vector<std::vector<double>>> DATA;

    bool bOnStart = true;
    for (const IsotopeDataRecord & iso : IsotopeBase)
    {
        const size_t numChan = iso.SpatialFiles.size();
        out("\n====>Isotope", iso.Isotope, " -> ", numChan, "channel(s)");
        if (numChan == 0) continue;

        if (bOnStart) // first channel of the first isotope -> configure Mapping
        {
            out("OnStart");
            Mapping.read(DataDirectory + '/' + iso.SpatialFiles[0]);
            Mapping.report();
            bOnStart = false;

            init1DArray(arAll1D);
            init2DArray(arAll2D);
            //init3DArray(DATA);
        }

        init1DArray(arIso1D);
        init2DArray(arIso2D);

        const double tauHalf = iso.HalfLife;
        out("Half-time:", tauHalf, "s");
        const double tau = tauHalf / log(2);
        const double timeFactor = calculateTimeFactor(tau);

        for (size_t iChan = 0; iChan < numChan; iChan++)
        {
            const std::string fn = DataDirectory + '/' + iso.SpatialFiles[iChan];
            out(fn);

            BinningParameters thisMapping;
            thisMapping.read(fn);
            if (thisMapping != Mapping)
            {
                out("Mismatch in data arrays");
                exit(3);
            }

            std::ifstream in(fn);
            std::string line;
            std::getline(in, line); // skip the first line which is the mapping json

            for (int ix = 0; ix < Mapping.NumBins[0]; ix++)
                for (int iy = 0; iy < Mapping.NumBins[1]; iy++)
                {
                    std::getline(in, line);
                    size_t sz; // std::string::size_type sz;

                    for (int iz = 0; iz < Mapping.NumBins[2]; iz++)
                    {
                        double val = std::stod(line, &sz);
                        line = line.substr(sz);

                        val *= timeFactor;

                        // Overall activity
                        arAll1D[iy] += val;
                        arAll2D[ix][iy] += val;
                        //DATA[ix][iy][iz] += val;

                        // Activity for this isotope
                        arIso1D[iy] += val;
                        arIso2D[ix][iy] += val;
                    }
                }

            save1D(arIso1D, iso.Isotope);
            save2D(arIso2D, iso.Isotope);
        }
    }

    out("\n----");
    save1D(arAll1D, "all");
    save2D(arAll2D, "all");
}

void ActivityProfilerMode::init1DArray(std::vector<double> & ar)
{
    ar.resize(Mapping.NumBins[1]);
    std::fill(ar.begin(), ar.end(), 0);
}

void ActivityProfilerMode::init2DArray(std::vector<std::vector<double> > & ar)
{
    ar.resize(Mapping.NumBins[0]);
    for (auto & vec : ar)
    {
        vec.resize(Mapping.NumBins[1]);
        std::fill(vec.begin(), vec.end(), 0);
    }
}

void ActivityProfilerMode::init3DArray(std::vector<std::vector<std::vector<double>>> & ar) // TODO : refactor
{
    ar.resize(Mapping.NumBins[0]);
    for (int ix = 0; ix < Mapping.NumBins[0]; ix++)
    {
        ar[ix].resize(Mapping.NumBins[1]);
        for (int iy = 0; iy < Mapping.NumBins[1]; iy++)
        {
            ar[ix][iy].resize(Mapping.NumBins[2]);
            std::fill(ar[ix][iy].begin(), ar[ix][iy].end(), 0);
        }
    }
}

double ActivityProfilerMode::calculateTimeFactor(double tau)
{
    double timeFactor = 0;
    for (int iTime = 0; iTime < numTimeRuns; iTime++)
    {
        const double t0 = sampleGenerationTime();
        for (size_t i = 0; i < ScanTimeWindows.size(); i++)
        {
            double timeTo = ScanTimeWindows[i].To - t0;
            if (timeTo  <= 0) continue;
            double timeFrom = ScanTimeWindows[i].From - t0;
            if (timeFrom < 0) timeFrom = 0;

            double from = exp(-timeFrom/tau);
            double to   = exp(-timeTo/tau);
            double delta = from - to;
            timeFactor += delta;
        }
    }
    timeFactor /= numTimeRuns;
    out("Time factor:",timeFactor);
    return timeFactor;
}

#include "G4RandomTools.hh"
double ActivityProfilerMode::sampleGenerationTime()
{
    double rnd = G4UniformRand() * SumBeamFactors;

    for (const BeamTimeWindow & win : BeamTimeWindows)
    {
        double val = (win.To - win.From) * win.Weight;
        if (rnd < val)
        {
            const double genTime = win.From + rnd / win.Weight;
            //out(genTime);
            return genTime;
        }

        rnd -= val;
    }

    out("Error in PES time generator");
    exit(3);
}

void ActivityProfilerMode::save1D(std::vector<double> & ar, const std::string & nameId)
{
    std::string fullFineName = DataDirectory + '/' + BaseFileName + '-' + nameId + "-1D.txt";

    std::ofstream outStream;
    outStream.open(fullFineName);
    if (!outStream.is_open() || outStream.fail() || outStream.bad())
    {
        out("Cannot open file:", fullFineName);
        exit(2);
    }
    else out("Saving output to file", fullFineName);

    for (size_t i = 0; i < ar.size(); i++)
    {
        if (i != 0) outStream << '\n';
        outStream << Mapping.Origin[1] + Mapping.BinSize[1] * i << ' ' << ar[i];
    }

    outStream.close();
}

void ActivityProfilerMode::save2D(std::vector<std::vector<double>> & ar, const std::string & nameId)
{
    const std::string fullFineName = DataDirectory + '/' + BaseFileName + '-' + nameId + "-2D.txt";

    std::ofstream outStream;
    outStream.open(fullFineName);
    if (!outStream.is_open() || outStream.fail() || outStream.bad())
    {
        out("Cannot open file:", fullFineName);
        exit(2);
    }
    else out("Saving output to file", fullFineName);


    json11::Json::object json;
    //BinSize
    {
        json11::Json::array jar;
        for (int i = 0; i < 3; i++) jar.push_back(Mapping.BinSize[i]);
        json["BinSize"] = jar;
    }
    // NumBins
    {
        json11::Json::array jar;
        for (int i = 0; i < 3; i++) jar.push_back(Mapping.NumBins[i]);
        json["NumBins"] = jar;
    }
    // Origin
    {
        json11::Json::array jar;
        for (int i = 0; i < 3; i++) jar.push_back(Mapping.Origin[i]);
        json["Origin"] = jar;
    }
    json11::Json aa(json);
    std::string str = '#' + aa.dump();
    outStream << str << '\n';

    for (size_t i0 = 0; i0 < ar.size(); i0++)
    {
        if (i0 != 0) outStream << '\n';
        for (size_t i1 = 0; i1 < ar[i0].size(); i1++)
        {
            if (i1 != 0) outStream << ' ';
            outStream << ar[i0][i1];
        }
    }

    outStream.close();
}


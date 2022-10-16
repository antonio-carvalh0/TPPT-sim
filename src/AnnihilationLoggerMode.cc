#include "AnnihilationLoggerMode.hh"
#include "jstools.hh"
#include "SessionManager.hh"
#include "StackingAction.hh"
#include "TrackingAction.hh"
#include "out.hh"

//#include <iostream>
//#include <sstream>
#include <fstream>

#include "G4RunManager.hh"
#include "G4ThreeVector.hh"

AnnihilationLoggerMode::AnnihilationLoggerMode(int numEvents, std::array<double, 3> binSize, std::array<int, 3> numBins, std::array<double, 3> origin, const std::string & fileName) :
    NumEvents(numEvents), BinSize(binSize), NumBins(numBins), Origin(origin)
{
    bNeedOutput = true;

    //bNeedGui = true; // only to test geometry/generation

    SessionManager & SM = SessionManager::getInstance();
    SM.FileName = fileName;

    initArray();
}

void AnnihilationLoggerMode::run()
{
    SessionManager & SM = SessionManager::getInstance();

    if (bNeedGui) SM.startGUI();
    else
    {
        SM.runManager->BeamOn(NumEvents);
        saveArray();
    }
}

void AnnihilationLoggerMode::readFromJson(const json11::Json &json)
{
    SessionManager & SM = SessionManager::getInstance();
    jstools::readInt   (json, "NumEvents", NumEvents);
    jstools::readString(json, "FileName",  SM.FileName);

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

    initArray();
}

G4UserTrackingAction *AnnihilationLoggerMode::getTrackingAction()
{
    return new AnnihilationLoggerTrackingAction();
}

G4UserStackingAction *AnnihilationLoggerMode::getStackingAction()
{
    return new AnnihilationLoggerStackingAction();
}

bool AnnihilationLoggerMode::getVoxel(const G4ThreeVector & pos, int * index)
{
    for (int i = 0; i < 3; i++)
        index[i] = floor( (pos[i] - Origin[i]) / BinSize[i] );

    for (int i = 0; i < 3; i++)
        if ( index[i] < 0 || index[i] >= NumBins[i]) return false;
    return true;
}

void AnnihilationLoggerMode::fillPosition(const G4ThreeVector & pos)
{
    int index[3];
    bool ok = getVoxel(pos, index);
    if (ok) Activity[index[0]][index[1]][index[2]]++;
}

void AnnihilationLoggerMode::doWriteToJson(json11::Json::object &json) const
{
    SessionManager & SM = SessionManager::getInstance();
    json["NumEvents"] = NumEvents;
    json["FileName"]  = SM.FileName;

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

void AnnihilationLoggerMode::initArray()
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

void AnnihilationLoggerMode::saveArray()
{
    SessionManager& SM = SessionManager::getInstance();

    json11::Json::object json;
    doWriteToJson(json);
    json11::Json aa(json);
    std::string str = '#' + aa.dump();
    *SM.outStream << str << '\n';

    for (const std::vector<std::vector<double>> & ary : Activity)
    {
        for (const std::vector<double> & arz : ary)
        {
            for (const double & val : arz) *SM.outStream << val << ' ';
            *SM.outStream << '\n';
        }
    }
}

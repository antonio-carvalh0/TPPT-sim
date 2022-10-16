#ifndef annihilationloggermode_h
#define annihilationloggermode_h

#include "SimMode.hh"

#include <vector>
#include <string>
#include <array>

class AnnihilationLoggerMode : public SimModeBase
{
public:
    AnnihilationLoggerMode(int numEvents, std::array<double,3> binSize, std::array<int,3> numBins, std::array<double,3> origin, const std::string & fileName);

    void run() override;

    std::string getTypeName() const override {return "AnnihilationLoggerMode";}

    void readFromJson(const json11::Json & json) override;

    G4UserTrackingAction * getTrackingAction() override;
    G4UserStackingAction * getStackingAction() override;

    void fillPosition(const G4ThreeVector & pos);

protected:
    void doWriteToJson(json11::Json::object & json) const override;

    int NumEvents;

    std::array<double, 3> BinSize; // mm
    std::array<int,    3> NumBins;
    std::array<double, 3> Origin;  // center coordinates of the frame

    std::vector<std::vector<std::vector<double>>> Activity;

    void initArray();
    void saveArray();
    bool getVoxel(const G4ThreeVector & pos, int *index);
};

#endif // annihilationloggermode_h

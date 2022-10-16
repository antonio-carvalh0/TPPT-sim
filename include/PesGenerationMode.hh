#ifndef pesgenerationmode_h
#define pesgenerationmode_h

#include "SimMode.hh"
#include "PesGenRecord.hh"

#include <array>
#include <vector>
#include <string>

class G4Track;

// this class is also the base class for PesProbabilityMode and ActivityGenerationMode!
class PesGenerationMode : public SimModeBase
{
public:
    PesGenerationMode(int numEvents, const std::string & outputFileName, bool binaryOutput); // Brute-force approach which logs generated PES

    std::string getTypeName() const override {return "PesGenerationMode";}
    G4UserTrackingAction * getTrackingAction() override;
    G4UserStackingAction * getStackingAction() override;

    void preInit() override;
    void run() override;
    void onEventStarted() override;

    bool modelTrigger(const G4Track * track);

    void saveRecord(const std::string & Pes, double X, double Y, double Z, double Time) const;

    std::vector<PesGenRecord> BaseRecords;

    std::vector<std::vector<PesGenRecord>> MaterialRecords; // [indexInMatTable] [Records]

    bool bNewTrackStarted = false;

    void readFromJson(const json11::Json & json) override;

protected:
    void doWriteToJson(json11::Json::object & json) const override;

    void loadCrossSections(const std::string & fileName);
    void exploreMaterials();
    void updateMatRecords(int iMat, int Z, int A, double IsotopeNumberDensity);

    int    NumEvents;
    int    LastMaterial;

    double LastEnergy;
    double LastTrackLength;
    G4ThreeVector LastPosition;

private:
    int CurrentEvent = 0;

    double SaveDirection[3];
    double SaveEnergy = 0;
    std::vector<double> ProbVec;

    virtual bool doTrigger(const G4Track * track); // return status (true = kill); the status is currently ignored (physics reasons)
};

#endif // pesgenerationmode_h

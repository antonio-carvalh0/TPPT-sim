#ifndef pesprobabilitymode_h
#define pesprobabilitymode_h

#include "PesGenerationMode.hh"

class PesProbabilityMode : public PesGenerationMode
{
public:
    PesProbabilityMode(int numEvents, std::array<double,3> binSize, std::array<int,3> numBins, std::array<double,3> origin,
                       const std::vector<std::pair<double,double>> & acquisitionFromTos);

    std::string getTypeName() const override {return "PesProbabilityMode";}

    void run() override;
    void onEventStarted() override {}

    void readFromJson(const json11::Json & json) override;

protected:
    void doWriteToJson(json11::Json::object & json) const override;

    std::array<double, 3> BinSize; // mm
    std::array<int,    3> NumBins;
    std::array<double, 3> Origin;  // center coordinates of the frame

    std::vector<std::pair<double,double>> TimeWindows;

    //path gives voxel indexes and trackLength
    void addPath(const G4ThreeVector & posFrom, const G4ThreeVector & posTo, std::vector<std::tuple<int,int,int, double>> & path); // simplistic: slow and not very precise
    void addPathA(const G4ThreeVector & posFrom, const G4ThreeVector & posTo, std::vector<std::tuple<int, int, int, double> > & path); // use this one!

    double calculateTimeFactor(double t0, double decayTime); // potentially bottleneck -> find a way to use a LUT

private:
    bool doTrigger(const G4Track * track) override;

    void initProbArrays();
    bool getVoxel(const G4ThreeVector & pos, int * index);
    bool isValidVoxel(int * coords) const;
    void saveArrays();
};

#endif // pesprobabilitymode_h

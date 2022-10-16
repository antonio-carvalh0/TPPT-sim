#ifndef activityprofilermode_h
#define activityprofilermode_h

#include "SimMode.hh"
#include "jstools.hh"

#include <vector>
#include <string>
#include <array>

struct BeamTimeWindow
{
    BeamTimeWindow(double from, double to) : From(from), To(to) {}
    BeamTimeWindow(double from, double to, double weight) : From(from), To(to), Weight(weight) {}

    double From   = 0;     // in seconds
    double To     = 1e30;  // in seconds
    double Weight = 1.0;
};
struct ScanTimeWindow
{
    ScanTimeWindow(double from, double to) : From(from), To(to) {}

    double From   = 0;     // in seconds
    double To     = 1e30;  // in seconds
};

struct IsotopeDataRecord
{
    IsotopeDataRecord(std::string isotope, double halfLife, const std::vector<std::string> & spatialFiles) :
        Isotope(isotope), HalfLife(halfLife), SpatialFiles(spatialFiles) {}

    std::string Isotope;
    double      HalfLife;
    std::vector<std::string> SpatialFiles;
};

class ActivityProfilerMode : public SimModeBase
{
public:
    ActivityProfilerMode(const std::vector<BeamTimeWindow> & beamStructure, const std::vector<ScanTimeWindow> & scanWindows,
                         const std::string & dataDirectory, const std::string & baseFileName);

    void run() override;

    std::string getTypeName() const override {return "ActivityProfilerMode";}

protected:
    const std::vector<BeamTimeWindow> BeamTimeWindows;
    const std::vector<ScanTimeWindow> ScanTimeWindows;
    const std::string DataDirectory;
    const std::string BaseFileName;

    std::vector<IsotopeDataRecord> IsotopeBase;
    double SumBeamFactors;

    BinningParameters Mapping;

    int numTimeRuns = 10000;

    void   init1DArray(std::vector<double> & ar);
    void   init2DArray(std::vector<std::vector<double>> & ar);
    void   init3DArray(std::vector<std::vector<std::vector<double>>> & ar);
    double calculateTimeFactor(double tau);
    double sampleGenerationTime();
    void   checkInputData();

    void   save1D(std::vector<double> & ar, const std::string & nameId);
    void   save2D(std::vector<std::vector<double>> & ar, const std::string & nameId);
};

#endif // activityprofilermode_h

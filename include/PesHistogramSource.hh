#ifndef peshistogramsource_h
#define peshistogramsource_h

#include "SourceMode.hh"
#include "GeantParticleGenerator.hh"

#include <string>
#include <vector>

struct PesDataRecord
{
    PesDataRecord(std::string isotope, const std::vector<std::string> & spatialFiles) :
        Isotope(isotope), SpatialFiles(spatialFiles) {}

    std::string Isotope;
    std::vector<std::string> SpatialFiles;
};

class PesHistogramSource : public SourceModeBase
{
public:
    PesHistogramSource(int iso, const std::string & directory, double multiplier);
    PesHistogramSource(int iso, const std::string & directory, double multiplier, double blur);
    PesHistogramSource(const json11::Json & json);

    void GeneratePrimaries(G4Event * anEvent) override;

    double CountEvents() override;

    std::string getTypeName() const override {return "PesHistogramSource";}

protected:
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);

    void init();
    void init2();

    void checkInputData();

    int Iso=0;
    std::string Directory;
    double Multiplier;
    double Blur=0;


    std::vector<PesDataRecord> IsotopeBase;
    size_t CurrentRecord = 0;
    size_t CurrentFile = 0;

    std::ifstream * inStream = nullptr;

    GeantParticleGenerator ParticleGenerator;

    const double TimeSpan = 1e9;


};

#endif // peshistogramsource_h

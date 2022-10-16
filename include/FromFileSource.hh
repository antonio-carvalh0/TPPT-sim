#ifndef fromfilesource_h
#define fromfilesource_h

#include "SourceMode.hh"
#include "GeantParticleGenerator.hh"

#include "G4ThreeVector.hh"

#include <string>
#include <map>

class G4ParticleDefinition;

class FromFileSource : public SourceModeBase
{
public:
    FromFileSource(const std::string & fileName, bool bBinaryFile);
    FromFileSource(const json11::Json & json);
    ~FromFileSource();

    void GeneratePrimaries(G4Event * anEvent) override;

    double CountEvents() override;

    std::string getTypeName() const override {return "FromFileSource";}

protected:
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);

    void init();

    void prepareStream();
    void addPrimary(G4Event * anEvent);

    GeantParticleGenerator ParticleGenerator;

    std::string FileName;
    bool bBinary = false;
    std::ifstream * inStream = nullptr;
    std::map<std::string, int> ElementToZ;

    std::string name;
    G4ParticleDefinition * pd;
    double energy, time;
    G4ThreeVector pos, dir;
};

#endif // fromfilesource_h

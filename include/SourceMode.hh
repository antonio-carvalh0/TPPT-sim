#ifndef SourceMode_h
#define SourceMode_h

#include "G4ThreeVector.hh"
#include "json11.hh"

#include <vector>
#include <string>

class ParticleBase;
class TimeGeneratorBase;
class G4ParticleGun;
class G4Event;
class G4Material;
class G4Navigator;
class Hist1D;
class Hist1DSampler;
class SourceModeBase;

class SourceModeFactory
{
public:
    static SourceModeBase * makeSourceModeInstance(const json11::Json & json);
};

class SourceModeBase
{
public:
    SourceModeBase(ParticleBase * particle, TimeGeneratorBase * timeGenerator); // transfers ownership
    virtual ~SourceModeBase();

    void initialize();

    virtual void GeneratePrimaries(G4Event * anEvent);

    virtual double CountEvents() {return 1;}

    virtual std::string getTypeName() const = 0;

    void writeToJson(json11::Json::object & json) const;
    void readFromJson(const json11::Json & json);

    void setParticleEnergy(double energy);

protected:
    virtual void customPostInit() {}
    virtual void doWriteToJson(json11::Json::object & json) const = 0;

    ParticleBase      * Particle      = nullptr;
    TimeGeneratorBase * TimeGenerator = nullptr;
    G4ParticleGun     * ParticleGun   = nullptr;

    G4ThreeVector Direction = {0, 0, 1.0};  // more general approach will be realized later!
    bool bIsotropicDirection = true;

    bool bGeneratePair  = false;    // for second gamma

protected:
    G4ThreeVector generateDirectionIsotropic();
    void generateSecondGamma(G4Event * anEvent);
};

// ---

class PointSource : public SourceModeBase
{
public:
    PointSource(ParticleBase * particle, TimeGeneratorBase * timeGenerator, const G4ThreeVector & origin);
    PointSource(const json11::Json & json);

    std::string getTypeName() const override {return "PointSource";}

protected:
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);

    G4ThreeVector Origin = {0,0,0};
};

// ---

class LineSource : public SourceModeBase
{
public:
    LineSource(ParticleBase * particle, TimeGeneratorBase * timeGenerator, const G4ThreeVector & startPoint, const G4ThreeVector & endPoint);
    LineSource(const json11::Json & json);

    void GeneratePrimaries(G4Event * anEvent) override;

    std::string getTypeName() const override {return "LineSource";}

protected:
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);

    G4ThreeVector StartPoint = {0,0,0};
    G4ThreeVector EndPoint   = {0,0,0};
};

// ---

class CylindricalSource : public SourceModeBase
{
public:
    CylindricalSource(ParticleBase * particle, TimeGeneratorBase * timeGenerator,
                      double radius, const G4ThreeVector & startPoint, const G4ThreeVector & endPoint,
                      const std::string & fileName = "");
    CylindricalSource(const json11::Json & json);
    ~CylindricalSource();

    void GeneratePrimaries(G4Event * anEvent) override;

    std::string getTypeName() const override {return "CylindricalSource";}

protected:
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);

    double Radius = 1.0;
    G4ThreeVector StartPoint = {0,0,0};
    G4ThreeVector EndPoint   = {0,0,0};
    std::string FileName;

    G4ThreeVector Axis;
    G4ThreeVector UnitNormal1;
    G4ThreeVector UnitNormal2;

    std::ofstream * Stream    = nullptr;

private:
    void init();
};

// ---

class ProfileBase
{
public:
    ProfileBase(){}
    virtual ~ProfileBase(){}

    //void addRotation(double Degrees);

    void setDirection(const G4ThreeVector & dir);

    virtual std::string getTypeName() const = 0;
    virtual void generateOffset(G4ThreeVector & pos) const = 0;

    void writeToJson(json11::Json::object & json) const;

protected:
    G4ThreeVector Direction;
    G4ThreeVector UnitPerp;
    double        Angle = 0;

    virtual void doWriteToJson(json11::Json::object & /*json*/) const {}
    //void readFromJson(const json11::Json & json);
};
class UniformProfile : public ProfileBase
{
public:
    UniformProfile(double dx, double dy) : ProfileBase(), DX(dx), DY(dy) {}
    UniformProfile(const json11::Json & json);

    std::string getTypeName() const override {return "Uniform";}
    void generateOffset(G4ThreeVector & pos) const override;

protected:
    double DX = 0;
    double DY = 0;

    void doWriteToJson(json11::Json::object & json) const override;
    void readFromJson(const json11::Json & json);
};
class GaussProfile : public ProfileBase
{
public:
    GaussProfile(double sigmaX, double sigmaY) : ProfileBase(), SigmaX(sigmaX), SigmaY(sigmaY) {}
    GaussProfile(const json11::Json & json);

    std::string getTypeName() const override {return "Gauss";}
    void generateOffset(G4ThreeVector & pos) const override;

protected:
    double SigmaX = 0;
    double SigmaY = 0;

    void doWriteToJson(json11::Json::object & json) const override;
    void readFromJson(const json11::Json & json);
};

class PencilBeam : public SourceModeBase
{
public:
    PencilBeam(ParticleBase * particle, TimeGeneratorBase * timeGenerator,
               const G4ThreeVector & origin, const G4ThreeVector & direction,
               int numParticles = 1, ProfileBase * spread = nullptr);
    PencilBeam(const json11::Json & json);
    ~PencilBeam();

    std::string getTypeName() const override {return "PencilBeam";}
    void GeneratePrimaries(G4Event * anEvent) override;

protected:
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);

    void update();

    G4ThreeVector Origin       = {0,0,0};
    int           NumParticles = 1;
    ProfileBase * Profile      = nullptr;
};

struct BeamRecord
{
    double Energy;
    double XIsoCenter;
    double ZIsoCenter;
    double TimeStart;
    double TimeSpan;
    double StatWeight;

    void writeToJson(json11::Json::object & json) const;
    void readFromJson(const json11::Json & json);

    void print() const;
};

// Beam is aligned with Y axis (downwards) and starts from Y = StartBeamFromY
// Divergence is determined using the apex position = Origin
class MultiBeam : public SourceModeBase
{
public:
    MultiBeam(ParticleBase * particle, const std::vector<BeamRecord> & beams, double totalParticles);
    MultiBeam(ParticleBase * particle, const std::string & beamletFileName, double totalParticles); // NomEnergy[MeV] XIso[mm] ZIso[mm] Time0[ns] TimeSpan[ns] StatWeight
    MultiBeam(const json11::Json & json);

    double CountEvents() override {return NumParticles;}

    std::string getTypeName() const override {return "MultiBeam";}
    void GeneratePrimaries(G4Event * anEvent) override;

    std::vector<std::pair<double,double>> getTimeWindows(double marginFrom, double marginTo) const;

protected:
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);

    void loadCalibration();
    void loadBeamletData(const std::string & beamletDataFile);
    void calculateParticlesPerStatWeightUnit();

    const G4ThreeVector Origin  = {0, 2520.0, 0};
    const double StartBeamFromY = 150.0;
    std::vector<BeamRecord> Beams;
    double NumParticles = 0;

    std::vector<std::array<double,3>> Calibration; // Nominal_energy[MeV] True_energy[MeV] SpotSigma[mm]
    const std::string CalibrationFileName = "BeamletCalibration.txt";

    //runtime
    double ParticlesPerStatWeightUnit = 0;
    double iRecord = 0;
    double iParticle = 0;
};

// ---

class MaterialLimitedSource : public SourceModeBase
{
public:
    MaterialLimitedSource(ParticleBase * particle,
                          TimeGeneratorBase * timeGenerator,
                          const G4ThreeVector & origin, const G4ThreeVector & boundingBoxFullSize,
                          const G4String & material,
                          G4String fileName_EmissionPositions = "");
    MaterialLimitedSource(const json11::Json & json);
    ~MaterialLimitedSource();

    void GeneratePrimaries(G4Event * anEvent) override;

    std::string getTypeName() const override {return "MaterialLimitedSource";}

protected:
    void init();
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);
    void customPostInit() override;

    G4ThreeVector Origin;
    G4ThreeVector BoundingBox;
    G4String      Material;
    G4String      FileName;

    //run-time
    G4Material    * SourceMat = nullptr;
    G4Navigator   * Navigator = nullptr;
    std::ofstream * Stream    = nullptr;
};

// ---

class NaturalLysoSource : public SourceModeBase
{
public:
    NaturalLysoSource(double timeFrom, double timeTo);
    NaturalLysoSource(const json11::Json & json);
    ~NaturalLysoSource();

    void GeneratePrimaries(G4Event * anEvent) override;

    std::string getTypeName() const override {return "NaturalLysoSource";}

protected:
    void init();
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);
    void customPostInit() override;

    double TimeFrom;
    double TimeTo;

    double ScintMaxRadius = 0;
    std::vector<std::pair<double,double>> ElectronSpectrum; //format: energy[keV] relative_probablility

    //run-time
    G4Navigator   * Navigator = nullptr;
    Hist1DSampler * Sampler   = nullptr;
};

// ---

class Na22point : public SourceModeBase
{
public:
    Na22point(double timeFrom, double timeTo, const G4ThreeVector & origin);
    Na22point(const json11::Json & json);

    void GeneratePrimaries(G4Event * anEvent) override;

    std::string getTypeName() const override {return "Na22point";}

protected:
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);

    double TimeFrom;
    double TimeTo;
    G4ThreeVector Origin;
};

// ---

class BlurredPointSource : public PointSource
{
public:
    BlurredPointSource(ParticleBase * particle, TimeGeneratorBase * timeGenerator, const G4ThreeVector & origin, G4String fileName);
    BlurredPointSource(const json11::Json & json);
    ~BlurredPointSource();

    void GeneratePrimaries(G4Event * anEvent) override;

    std::string getTypeName() const override {return "BlurredPointSource";}

protected:
    void init();
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json);

    std::string FileName;
    Hist1DSampler * Sampler   = nullptr;
};

// --

#endif // SourceMode_h

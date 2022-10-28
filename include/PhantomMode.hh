#ifndef PhantomMode_h
#define PhantomMode_h

#include "json11.hh"

#include <string>
#include <vector>

class PhantomModeBase;
class G4LogicalVolume;

class PhantomModeFactory // when adding a new mode, do not forget to modify the implementation of the factory!
{
public:
    static PhantomModeBase * makePhantomModeInstance(const json11::Json & json);
};

class PhantomModeBase
{
public:
    virtual ~PhantomModeBase(){}

    virtual std::string getTypeName() const = 0;
    virtual G4LogicalVolume * definePhantom(G4LogicalVolume * /*logicWorld*/) = 0; //returns the phantom's logical volume

    void writeToJson(json11::Json::object & json) const;
    virtual void readFromJson(const json11::Json & /*json*/) {}

protected:
    virtual void doWriteToJson(json11::Json::object & /*json*/) const {}
};

// ---

class PhantomNone : public PhantomModeBase
{
    std::string getTypeName() const override {return "PhantomNone";}
    G4LogicalVolume * definePhantom(G4LogicalVolume *) override {return nullptr;}
};

// ---

class PhantomPMMA : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomPMMA";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
};

class PhantomCustomBox : public PhantomModeBase
{
public:
    enum EMaterial {PMMA, HDPE, PE, Graphite, GelTissue, GelWater, Bone, Brain, Blood, Muscle, Tissue, Water};

    PhantomCustomBox(double sizeX, double sizeY, double sizeZ, EMaterial material);
    PhantomCustomBox(){}

    std::string getTypeName() const override {return "PhantomCustomBox";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;

    void readFromJson(const json11::Json & json) override;

protected:
    void doWriteToJson(json11::Json::object & json) const override;

    double SizeX = 100;
    double SizeY = 100;
    double SizeZ = 100;
    EMaterial Material = HDPE;
};

// ---

class PhantomTinyCube : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomTinyCube";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
};

// ---

class PhantomDerenzo : public PhantomModeBase
{
public:
    PhantomDerenzo(double diameter, double height, const std::vector<double> & holeDiameters, double radialOffset, double margin, double dPhi) :
        Diameter(diameter), Height(height), HoleDiameters(holeDiameters), RadialOffset(radialOffset), Margin(margin), DPhi(dPhi) {}

    std::string getTypeName() const override {return "PhantomDerenzo";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;

    void readFromJson(const json11::Json & json) override;

protected:
    double Diameter     = 200.0;
    double Height       = 200.0;
    std::vector<double> HoleDiameters;
    double RadialOffset = 50.0;
    double Margin       = 50.0;
    double DPhi         = 0;     // rotation angle around vertical axis

    void doWriteToJson(json11::Json::object & json) const override;
};

// ---

class PhantomParam : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomParam";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;

    void readFromJson(const json11::Json &) override {}

protected:
    void doWriteToJson(json11::Json::object &) const override {}
};

// ---

class PhantomEnergyCalibration : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomEnergyCalibration";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
};

// ---

class PhantomEspana : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomEspana";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
};

class PhantomBauerGel : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomBauerGel";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
};

class PhantomBauerCa : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomBauerCa";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
};

class PhantomRT : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomRT";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
};

class PhantomBoneBrainHorizontal : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomBoneBrainHorizontal";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
};

class PhantomBoneBrainVertical : public PhantomModeBase
{
public:
    std::string getTypeName() const override {return "PhantomBoneBrainVertical";}
    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
};
#endif // PhantomMode_h

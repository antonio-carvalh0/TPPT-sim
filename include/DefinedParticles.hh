#ifndef definedparticles_h
#define definedparticles_h

#include "json11.hh"

#include <string>

#include "G4SystemOfUnits.hh"

class G4ParticleDefinition;
class ParticleBase;

class ParticleFactory
{
public:
    static ParticleBase * makeParticleInstance(const json11::Json & json);
};

class ParticleBase
{
public:
    ParticleBase(const std::string & name) : Name(name) {}
    virtual ~ParticleBase(){}

    std::string Name = "undefined";
    double      Energy = 1.0;

    virtual G4ParticleDefinition * getParticleDefinition() const = 0;

    void writeToJson(json11::Json::object & json) const;
    void readFromJson(const json11::Json & json);

    virtual std::string getTypeName() const = 0;

protected:
    virtual void doWriteToJson(json11::Json::object & /*json*/) const {};
    virtual void doReadFromJson(const json11::Json & /*json*/) {};
};

// ---

class Geantino : public ParticleBase
{
public:
    Geantino() : ParticleBase("geantino") {}
    G4ParticleDefinition * getParticleDefinition() const override;

    std::string getTypeName() const override {return "Geantino";}
};

class Gamma : public ParticleBase
{
public:
    Gamma(double energy = 0.511*MeV) : ParticleBase("gamma") {Energy = energy;}
    G4ParticleDefinition * getParticleDefinition() const override;

    std::string getTypeName() const override {return "Gamma";}
};

class GammaPair : public ParticleBase
{
public:
    GammaPair(double energy = 0.511*MeV) : ParticleBase("gammaPair") {Energy = energy;}
    G4ParticleDefinition * getParticleDefinition() const override;

    std::string getTypeName() const override {return "GammaPair";}
};

// ---

class Isotope : public ParticleBase
{
public:
    Isotope(int z, int a);
    Isotope(int z, int a, double excitationEnergy);

    G4ParticleDefinition * getParticleDefinition() const override;

    std::string getTypeName() const override {return "Isotope";}
protected:
    void doWriteToJson(json11::Json::object & json) const override;
    void doReadFromJson(const json11::Json & json) override;

    int Z, A;
    double ExcitationEnergy = 0;
};

class C10 : public Isotope
{
public:
    C10() : Isotope(6, 10) {Name = "C10";}
};

class C11 : public Isotope
{
public:
    C11() : Isotope(6, 11) {Name = "C11";}
};

class O15 : public Isotope
{
public:
    O15() : Isotope(8, 15) {Name = "O15";}
};

class N13 : public Isotope
{
public:
    N13() : Isotope(7, 13) {Name = "N13";}
};

class N12 : public Isotope
{
public:
    N12() : Isotope(7, 12) {Name = "N12";}
};

// ---

class Hf176exc : public Isotope
{
public:
    Hf176exc() : Isotope(72, 176, 596.820*keV) {Name = "Hf176exc";}
};

// ---

class Proton : public ParticleBase
{
public:
    Proton(double energy = 100.0*MeV) : ParticleBase("proton") {Energy = energy;}
    G4ParticleDefinition * getParticleDefinition() const override;

    std::string getTypeName() const override {return "Proton";}
};

#endif // definedparticles_h

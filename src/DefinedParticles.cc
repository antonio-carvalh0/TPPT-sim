#include "DefinedParticles.hh"
#include "jstools.hh"
#include "out.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

ParticleBase * ParticleFactory::makeParticleInstance(const json11::Json & json)
{
    out("Reading particle json");
    std::string Type;
    jstools::readString(json, "Type", Type);

    ParticleBase * p = nullptr;

    if      (Type == "Geantino")  p = new Geantino();
    else if (Type == "Gamma")     p = new Gamma();
    else if (Type == "GammaPair") p = new GammaPair();
    else if (Type == "Isotope")   p = new Isotope(0, 0, 0);
    else if (Type == "Proton")    p = new Proton();
    else
    {
        out("Unknown particle type!");
        exit(20);
    }

    p->readFromJson(json);
    return p;
}

// ---

void ParticleBase::writeToJson(json11::Json::object & json) const
{
    json["Type"] = getTypeName();
    json["Energy"] = Energy;
    doWriteToJson(json);
}

void ParticleBase::readFromJson(const json11::Json &json)
{
    jstools::readDouble(json, "Energy", Energy);
    doReadFromJson(json);
}

// ---

G4ParticleDefinition * Geantino::getParticleDefinition() const
{
    G4ParticleTable * ptab = G4ParticleTable::GetParticleTable();
    return ptab->FindParticle("geantino");
}

G4ParticleDefinition * Gamma::getParticleDefinition() const
{
    G4ParticleTable * ptab = G4ParticleTable::GetParticleTable();
    return ptab->FindParticle("gamma");
}

G4ParticleDefinition * GammaPair::getParticleDefinition() const
{
    G4ParticleTable * ptab = G4ParticleTable::GetParticleTable();
    return ptab->FindParticle("gamma");
}

Isotope::Isotope(int z, int a) :
    ParticleBase("ion_" + std::to_string(z) + "_" + std::to_string(a)), Z(z), A(a)
{
    Energy = 0;
}

Isotope::Isotope(int z, int a, double excitationEnergy) :
    ParticleBase("ion_" + std::to_string(z) + "_" + std::to_string(a) + '[' + std::to_string(excitationEnergy) + ']'),
    Z(z), A(a), ExcitationEnergy(excitationEnergy)
{
    Energy = 0;
}

G4ParticleDefinition * Isotope::getParticleDefinition() const
{
    G4IonTable * itab = G4IonTable::GetIonTable();
    return itab->GetIon(Z, A, ExcitationEnergy);
}

void Isotope::doWriteToJson(json11::Json::object & json) const
{
    json["Z"] = Z;
    json["A"] = A;
    json["ExcitationEnergy"] = ExcitationEnergy;
}

void Isotope::doReadFromJson(const json11::Json & json)
{
    jstools::readInt   (json, "Z", Z);
    jstools::readInt   (json, "A", A);
    jstools::readDouble(json, "ExcitationEnergy", ExcitationEnergy);
}

// ---

G4ParticleDefinition *Proton::getParticleDefinition() const
{
    G4ParticleTable * ptab = G4ParticleTable::GetParticleTable();
    return ptab->FindParticle("proton");
}

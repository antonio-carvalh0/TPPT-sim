#include "GeantParticleGenerator.hh"
#include "out.hh"

#include <vector>
#include <sstream>

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4SystemOfUnits.hh"

GeantParticleGenerator::GeantParticleGenerator()
{
    std::vector<std::string> allElements = {"H","He","Li","Be","B","C","N","O","F","Ne","Na","Mg","Al","Si","P","S","Cl","Ar","K","Ca","Sc","Ti","V","Cr","Mn","Fe","Co","Ni","Cu","Zn","Ga","Ge","As","Se","Br","Kr","Rb","Sr","Y","Zr","Nb","Mo","Tc","Ru","Rh","Pd","Ag","Cd","In","Sn","Sb","Te","I","Xe","Cs","Ba","La","Ce","Pr","Nd","Pm","Sm","Eu","Gd","Tb","Dy","Ho","Er","Tm","Yb","Lu","Hf","Ta","W","Re","Os","Ir","Pt","Au","Hg","Tl","Pb","Bi","Po","At","Rn","Fr","Ra","Ac","Th","Pa","U","Np","Pu","Am","Cm","Bk","Cf","Es","Fm","Md","No","Lr","Rf","Db","Sg","Bh","Hs"};
    for (size_t i = 0; i < allElements.size(); i++)
        ElementToZ.emplace( std::make_pair(allElements[i], i+1) );
}

G4ParticleDefinition * GeantParticleGenerator::makeGeant4Particle(const std::string & particleName)
{
    G4ParticleDefinition * particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);

    if (!particle)
    {
        // is it an ion?
        int Z, A;
        double E;
        bool ok = extractIonInfo(particleName, Z, A, E);
        if (!ok)
        {
            out("Found an unknown particle: " + particleName);
            exit(112);
        }

        particle = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(Z, A, E*keV);

        if (!particle)
        {
            out("Failed to generate ion: " + particleName);
            exit(113);
        }

        //std::cout << particleName << "   ->   " << particle->GetParticleName() << std::endl;
    }

    return particle;
}

bool GeantParticleGenerator::extractIonInfo(const std::string & text, int & Z, int & A, double & E)
{
    size_t size = text.length();
    if (size < 2) return false;

    // -- extracting Z --
    const char & c0 = text[0];
    if (c0 < 'A' || c0 > 'Z') return false;
    std::string symbol;
    symbol += c0;

    size_t index = 1;
    const char & c1 = text[1];
    if (c1 >= 'a' && c1 <= 'z')
    {
        symbol += c1;
        index++;
    }
    try
    {
        Z = ElementToZ.at(symbol);
    }
    catch (...)
    {
        return false;
    }

    // -- extracting A --
    A = 0; E = 0;
    char ci;
    while (index < size)
    {
        ci = text[index];
        if (ci < '0' || ci > '9') break;
        A = A*10 + (int)ci - (int)'0';
        index++;
    }
    if (A == 0) return false;

    if (index == size) return true;

    // -- extracting excitation energy --
    if (ci != '[') return false;
    index++;
    std::stringstream excEnergy;
    while (index < size)
    {
        ci = text[index];
        if (ci == ']')
        {
            excEnergy >> E;
            return !excEnergy.fail();
        }
        excEnergy << ci;
        index++;
    }
    return false;
}

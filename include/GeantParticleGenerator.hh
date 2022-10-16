#ifndef geantparticlegenerator_h
#define geantparticlegenerator_h

#include <string>
#include <map>

class G4ParticleDefinition;

class GeantParticleGenerator
{
public:
    GeantParticleGenerator();

    G4ParticleDefinition * makeGeant4Particle(const std::string & particleName);

protected:
    std::map<std::string, int> ElementToZ;

    bool extractIonInfo(const std::string & text, int & Z, int & A, double & E);
};

#endif //geantparticlegenerator_h

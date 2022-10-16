#ifndef particlekiller_h
#define particlekiller_h

#include "G4VFastSimulationModel.hh"

class ParticleKillerModel : public G4VFastSimulationModel
{
public:
    ParticleKillerModel(const G4String & name, G4Region * region) :
        G4VFastSimulationModel(name, region) {}

    G4bool IsApplicable(const G4ParticleDefinition & particle) override;

    G4bool ModelTrigger(const G4FastTrack & fastTrack) override;

    void DoIt(const G4FastTrack & fastTrack, G4FastStep & step) override;
};

#endif // particlekiller_h

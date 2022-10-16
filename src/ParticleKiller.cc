#include "ParticleKiller.hh"
#include "out.hh"
#include "G4NeutrinoE.hh"

G4bool ParticleKillerModel::IsApplicable(const G4ParticleDefinition & particle)
{
    const bool applicable = (&particle == G4NeutrinoE::Definition());
    //out("PKiller isApplicable called", particle.GetParticleName(), applicable);
    return applicable;
}

G4bool ParticleKillerModel::ModelTrigger(const G4FastTrack & )
{
    return true;
}

void ParticleKillerModel::DoIt(const G4FastTrack & , G4FastStep & step)
{
    step.KillPrimaryTrack();
}

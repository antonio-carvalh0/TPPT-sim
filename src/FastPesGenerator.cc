#include "FastPesGenerator.hh"
#include "SessionManager.hh"
#include "G4Proton.hh"
//#include "G4Track.hh"
//#include "G4RandomTools.hh"
#include "out.hh"
//#include "Randomize.hh"
#include "PesGenerationMode.hh"

G4bool FastPesGeneratorModel::IsApplicable(const G4ParticleDefinition & particle)
{
    const bool applicable = (&particle == G4Proton::ProtonDefinition());
    //out("PES isApplicable called", particle.GetParticleName(), applicable);
    return applicable;
}

G4bool FastPesGeneratorModel::ModelTrigger(const G4FastTrack & fastTrack)
{
    const G4Track * track = fastTrack.GetPrimaryTrack();
    if (track->GetParentID() != 0) return false;

    SessionManager & SM = SessionManager::getInstance();
    PesGenerationMode * PGM = static_cast<PesGenerationMode*>(SM.SimMode);
    return PGM->modelTrigger(track);
}

void FastPesGeneratorModel::DoIt(const G4FastTrack &, G4FastStep & step)
{
    //out("PES doit");
    step.KillPrimaryTrack();
}

#include "StackingAction.hh"
#include "G4Positron.hh"

#include "G4Track.hh"

G4ClassificationOfNewTrack PesGeneratorStackingAction::ClassifyNewTrack(const G4Track * track)
{
    if (track->GetParentID() == 0) return fUrgent;
    return fKill;
}

G4ClassificationOfNewTrack AnnihilationLoggerStackingAction::ClassifyNewTrack(const G4Track *track)
{
    if (track->GetParentID() == 0) return fUrgent;
    if (track->GetParticleDefinition() == G4Positron::Definition()) return fUrgent;
    return fKill;
}

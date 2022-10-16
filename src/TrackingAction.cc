#include "TrackingAction.hh"
#include "SessionManager.hh"
#include "PesGenerationMode.hh"
#include "AnnihilationLoggerMode.hh"
#include "out.hh"

#include "G4Track.hh"
#include "G4Positron.hh"
#include "G4VProcess.hh"

void PesGeneratorTrackingAction::PreUserTrackingAction(const G4Track * /*track*/)
{
    SessionManager & SM = SessionManager::getInstance();
    PesGenerationMode * Mode = static_cast<PesGenerationMode*>(SM.SimMode);
    Mode->bNewTrackStarted = true;
}

void AnnihilationLoggerTrackingAction::PostUserTrackingAction(const G4Track * track)
{
    if (track->GetParticleDefinition() != G4Positron::Definition()) return;

    //out(track->GetPosition()[0], track->GetPosition()[1], track->GetPosition()[2]);

    //const G4VProcess * proc = track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep();
    //std::string procName = (proc ? proc->GetProcessName() : "undefined");
    //if (procName != "annihil") out(procName);

    SessionManager & SM = SessionManager::getInstance();
    AnnihilationLoggerMode * mode = static_cast<AnnihilationLoggerMode*>(SM.SimMode);
    mode->fillPosition(track->GetPosition());
}

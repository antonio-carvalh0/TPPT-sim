#include "SensitiveDetectorFSM.hh"
#include "SessionManager.hh"
#include "SimMode.hh"
#include "out.hh"

#include "G4Step.hh"
#include "G4VProcess.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

G4bool SensitiveDetectorFSM::ProcessHits(G4Step * step, G4TouchableHistory *)
{
    SessionManager & SM = SessionManager::getInstance();

    const G4StepPoint * postP  = step->GetPostStepPoint();

    double buf[6];
    const G4ThreeVector & pos = postP->GetPosition();
    buf[0] = pos[0]/mm;
    buf[1] = pos[1]/mm;
    buf[2] = pos[2]/mm;
    const G4ThreeVector & dir = postP->GetMomentumDirection();
    buf[3] = dir[0];
    buf[4] = dir[1];
    buf[5] = dir[2];

    SimModeFirstStage * mode = dynamic_cast<SimModeFirstStage*>(SM.SimMode);
    if (mode)
    {
        mode->saveParticle(step->GetTrack()->GetParticleDefinition()->GetParticleName(),
                           postP->GetKineticEnergy()/keV,
                           buf,
                           postP->GetGlobalTime()/ns);
    }

    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return true;
}

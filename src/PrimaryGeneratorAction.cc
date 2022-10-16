#include "PrimaryGeneratorAction.hh"
#include "SessionManager.hh"
#include "SourceMode.hh"

#include "G4Event.hh"

void PrimaryGeneratorAction::GeneratePrimaries(G4Event * anEvent)
{
    SessionManager & SM = SessionManager::getInstance();
    SM.SourceMode->GeneratePrimaries(anEvent);
}

#include "EventAction.hh"
#include "G4Event.hh"
#include "SessionManager.hh"
#include "SimMode.hh"
#include "out.hh"

void EventAction::BeginOfEventAction(const G4Event * event)
{
    SessionManager & SM = SessionManager::getInstance();
    if (SM.ShowEventNumber)
    {
        int id = event->GetEventID();
        if (id % SM.EvNumberInterval == 0) out("--- event #", id, "---");
    }

    SM.SimMode->onEventStarted();
}

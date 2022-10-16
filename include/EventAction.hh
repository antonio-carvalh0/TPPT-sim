#ifndef eventaction_h
#define eventaction_h

#include "G4UserEventAction.hh"

class EventAction : public G4UserEventAction
{
public:
    void BeginOfEventAction(const G4Event * event) override;
};

#endif // eventaction_h

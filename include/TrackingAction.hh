#ifndef trackingaction_h
#define trackingaction_h

#include "G4UserTrackingAction.hh"

class PesGeneratorTrackingAction : public G4UserTrackingAction
{
public:
    void PreUserTrackingAction(const G4Track * track);
    //void PostUserTrackingAction(const G4Track*) {}
};

class AnnihilationLoggerTrackingAction : public G4UserTrackingAction
{
public:
    //void PreUserTrackingAction(const G4Track * track);
    void PostUserTrackingAction(const G4Track * track);
};

class BoneBrainTrackingAction : public G4UserTrackingAction
{
public:
    void PreUserTrackingAction(const G4Track * track);
    void PostUserTrackingAction(const G4Track * track);
};

#endif // trackingaction_h

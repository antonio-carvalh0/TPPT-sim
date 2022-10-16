#ifndef stackingaction_h
#define stackingaction_h

#include "G4UserStackingAction.hh"

class PesGeneratorStackingAction : public G4UserStackingAction
{
public:
    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track * track) override;
};

class AnnihilationLoggerStackingAction : public G4UserStackingAction
{
public:
    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track * track) override;
};

#endif

#ifndef SensitiveDetectorScint_h
#define SensitiveDetectorScint_h

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4HCofThisEvent;

class SensitiveDetectorScint_SingleEvents : public G4VSensitiveDetector
{
public:
    SensitiveDetectorScint_SingleEvents(const G4String & name)
        : G4VSensitiveDetector(name) {}

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
};

class SensitiveDetectorScint_MultipleEvents : public G4VSensitiveDetector
{
public:
    SensitiveDetectorScint_MultipleEvents(const G4String & name)
        : G4VSensitiveDetector(name) {}

    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
};

#endif // SensitiveDetectorScint_h

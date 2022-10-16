#ifndef SensitiveDetectorFSM_h
#define SensitiveDetectorFSM_h

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4TouchableHistory;

class SensitiveDetectorFSM : public G4VSensitiveDetector
{
public:
    SensitiveDetectorFSM(const G4String & name)
        : G4VSensitiveDetector(name) {}

    virtual G4bool ProcessHits(G4Step * step, G4TouchableHistory * history);
};

#endif // SensitiveDetectorFSM_h

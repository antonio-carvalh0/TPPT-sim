#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"

class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    void GeneratePrimaries(G4Event * anEvent) override;
};

#endif // PrimaryGeneratorAction_h

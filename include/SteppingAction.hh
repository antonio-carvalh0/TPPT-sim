#ifndef SteppingAction_h
#define SteppingAction_h

#include "G4UserSteppingAction.hh"

class G4Step;

class SteppingAction_ScintPosTest : public G4UserSteppingAction
{
public:
    void UserSteppingAction(const G4Step * step) override;
};

// ---

class SteppingAction_Dose : public G4UserSteppingAction
{
public:
    void UserSteppingAction(const G4Step * step) override;
};

// ---

class SteppingAction_EnCal : public G4UserSteppingAction
{
public:
    void UserSteppingAction(const G4Step * step) override;
};

// ---

class SteppingAction_Tracing : public G4UserSteppingAction
{
public:
    void UserSteppingAction(const G4Step * step) override;
};

// ---

class SteppingAction_AcollinearityTester : public G4UserSteppingAction
{
public:
    void UserSteppingAction(const G4Step * step) override;
};

// ---

class SteppingAction_AnnihilationTester : public G4UserSteppingAction
{
public:
    void UserSteppingAction(const G4Step * step) override;
};

// ---

class SteppingAction_NatRadTester : public G4UserSteppingAction
{
public:
    void UserSteppingAction(const G4Step * step) override;
};

// ---

class SteppingAction_DepoStatMode : public G4UserSteppingAction
{
public:
    void UserSteppingAction(const G4Step * step) override;
};

#endif // SteppingAction_h

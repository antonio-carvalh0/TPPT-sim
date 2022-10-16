#include "SteppingAction.hh"
#include "SessionManager.hh"
#include "SimMode.hh"
#include "out.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHandle.hh"
#include "G4NavigationHistory.hh"
#include "G4VProcess.hh"
#include "G4Track.hh"

void SteppingAction_ScintPosTest::UserSteppingAction(const G4Step * step)
{
    SessionManager & SM = SessionManager::getInstance();
    SimModeScintPosTest * Mode = static_cast<SimModeScintPosTest*>(SM.SimMode);

    const G4StepPoint * postP  = step->GetPostStepPoint();

    if (postP->GetMaterial() == SM.ScintMat)
    {
        Mode->Hits++;

        const G4TouchableHandle & touch = postP->GetTouchableHandle(); //to get the physical volumes
        int iScint    = touch->GetVolume(0)->GetCopyNo(); //this volume (scintillator)
        int iAssembly = touch->GetVolume(1)->GetCopyNo(); //container/master of the volume (encapsulation)

        G4ThreeVector globCenterPos = touch->GetHistory()->GetTopTransform().Inverse().TransformPoint(G4ThreeVector(0,0,0.5*SM.ScintSizeZ)); //local to global
        double delta = 0;
        for (int i=0; i<3; i++)
        {
            double d = globCenterPos[i] - SM.ScintRecords[iScint].FacePos[i];
            delta += d*d;
        }
        delta = sqrt(delta);

        Mode->SumDelta += delta;
        if (delta > Mode->MaxDelta) Mode->MaxDelta = delta;

        if (SM.Debug)
        {
            out("Index of the scintillator:",iScint, " Index of the assembly:",iAssembly);
            out("Volume center position:", globCenterPos[0], globCenterPos[1], globCenterPos[2]);
            out("   --> from ScintPos:  ",SM.ScintRecords[iScint].FacePos[0], SM.ScintRecords[iScint].FacePos[1], SM.ScintRecords[iScint].FacePos[2]); //it's faster (only once in detector construction)
            out("       --> Delta", delta, "mm");

        }
    }
}

// ---

void SteppingAction_Dose::UserSteppingAction(const G4Step * step)
{
    const double depo = step->GetTotalEnergyDeposit(); // in MeV
    if (depo == 0) return;

    const G4StepPoint * postP = step->GetPostStepPoint();
    if (!postP->GetMaterial()) return; // particle escaped
    const G4StepPoint * preP  = step->GetPreStepPoint();

    SessionManager & SM = SessionManager::getInstance();
    DoseExtractorMode * Mode = static_cast<DoseExtractorMode*>(SM.SimMode);

    Mode->fill(depo, 0.5*(preP->GetPosition() + postP->GetPosition()));
}

// ---

void SteppingAction_EnCal::UserSteppingAction(const G4Step * step)
{
    const double depo = step->GetTotalEnergyDeposit(); // in MeV
    if (depo == 0) return;

    const G4StepPoint * postP = step->GetPostStepPoint();
    if (!postP->GetMaterial()) return; // particle escaped
    const G4StepPoint * preP  = step->GetPreStepPoint();

    SessionManager & SM = SessionManager::getInstance();
    EnergyCalibrationMode * Mode = static_cast<EnergyCalibrationMode*>(SM.SimMode);

    const double yPos = 0.5 * (preP->GetPosition()[1] + postP->GetPosition()[1]);
    Mode->fill(depo, yPos);
}

// ---

void SteppingAction_Tracing::UserSteppingAction(const G4Step *step)
{
    const G4StepPoint * postP  = step->GetPostStepPoint();
    const G4StepPoint * preP   = step->GetPreStepPoint();
    if (!postP) return;

    G4VPhysicalVolume * physVol = preP->GetPhysicalVolume();
    std::string physVolName, logVolName;
    int         copyNum;
    std::string matName;
    G4ThreeVector prePos = preP->GetPosition();

    bool bStart = (step->GetTrack()->GetCurrentStepNumber() == 1);
    if (bStart)
    {
        physVol = preP->GetPhysicalVolume();
        if (physVol)
        {
            physVolName = preP->GetPhysicalVolume()->GetName();
            logVolName  = preP->GetPhysicalVolume()->GetLogicalVolume()->GetName();
            copyNum = preP->GetPhysicalVolume()->GetCopyNo();
            matName = preP->GetMaterial()->GetName();
            out("Created at (", prePos[0], ",",prePos[1],",", prePos[2],")", physVolName, "/", logVolName, "(",copyNum, ")", matName);
        }
        else
            out("Created at (", prePos[0], ",",prePos[1],",", prePos[2],") - seems to be outside the World!" );
    }

    const G4VProcess  * proc = postP->GetProcessDefinedStep();

    std::string procName = "undefined";
    if (proc) procName   = proc->GetProcessName();
    G4ThreeVector pos    = postP->GetPosition();
    physVol = postP->GetPhysicalVolume();

    if (physVol)
    {
        physVolName = postP->GetPhysicalVolume()->GetName();
        logVolName  = postP->GetPhysicalVolume()->GetLogicalVolume()->GetName();
        copyNum = postP->GetPhysicalVolume()->GetCopyNo();
        matName = postP->GetMaterial()->GetName();
        out(procName, " -> (", pos[0], ",",pos[1],",", pos[2],") ",
            physVolName, "/", logVolName, "(#",copyNum, ")",
            matName, '(', postP->GetMaterial()->GetDensity()/g*cm3, "g/cm3)",
            postP->GetKineticEnergy()/MeV);
        //out(proc->GetProcessType());
    }
    else
        out("Exited world at (", pos[0], ",",pos[1],",", pos[2],")" );
}

// ---

void SteppingAction_AcollinearityTester::UserSteppingAction(const G4Step *step)
{
    SessionManager & SM = SessionManager::getInstance();
    if (step->GetTrack()->GetParticleDefinition() != SM.GammaPD) return;
    if (step->GetTrack()->GetCurrentStepNumber() != 1) return;

    const G4StepPoint * postP  = step->GetPostStepPoint();
    const G4StepPoint * preP   = step->GetPreStepPoint();

    G4ThreeVector vec;
    const G4VProcess  * proc = postP->GetProcessDefinedStep();
    if (proc->GetProcessType() == fParameterisation) // catching fastSimProcess_massGeom
        vec = postP->GetMomentumDirection();
    else
        vec = preP->GetMomentumDirection();

    /*
    const G4ThreeVector & vpre  = step->GetPreStepPoint()->GetMomentumDirection();
    const G4ThreeVector & vpost = step->GetPostStepPoint()->GetMomentumDirection();
    const G4ThreeVector & rpre = step->GetPreStepPoint()->GetPosition();
    const G4ThreeVector & rpost = step->GetPostStepPoint()->GetPosition();
    out(vpre, vpost, " pos ", rpre, rpost);
    //step->GetTrack()->SetTrackStatus(fStopAndKill);
    */

    SimModeAcollinTest * Mode = static_cast<SimModeAcollinTest*>(SM.SimMode);
    Mode->addDirection(vec, step->GetTrack()->GetParentID(), preP->GetKineticEnergy());
}

// ---

#include "G4ProcessType.hh"
void SteppingAction_AnnihilationTester::UserSteppingAction(const G4Step * step)
{
    SessionManager & SM = SessionManager::getInstance();

    const G4StepPoint * postP  = step->GetPostStepPoint();
    const G4VProcess  * proc = postP->GetProcessDefinedStep();

    /*
    if (proc->GetProcessName() == "annihil")
    {
        out(proc->GetProcessType(), proc->GetProcessTypeName(proc->GetProcessType()), proc->GetProcessSubType());
        //--> 2 Electromagnetic 5
    }
    */

    if (proc->GetProcessType() != fElectromagnetic) return;
    if (proc->GetProcessSubType() != 5) return;

    SimModeAnnihilTest * Mode = static_cast<SimModeAnnihilTest*>(SM.SimMode);
    const double time = postP->GetGlobalTime()/s;
    if (time < Mode->TimeStart) return;

    Mode->saveRecord(postP->GetPosition(), time);
}

// ---

void SteppingAction_NatRadTester::UserSteppingAction(const G4Step * step)
{
    const double depo = step->GetTotalEnergyDeposit(); // in MeV
    if (depo == 0) return;

    // note that energy deposition cam be on exiting scintillator!

    const G4StepPoint * postP  = step->GetPostStepPoint();
    const G4StepPoint * preP   = step->GetPreStepPoint();

    bool bTransport = false;
    const G4VProcess  * proc = postP->GetProcessDefinedStep();
    if (proc) bTransport = ( (proc->GetProcessType() == fTransportation) );

    G4Material * mat = (bTransport ? preP ->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()
                                   : postP->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial() );

    SessionManager & SM = SessionManager::getInstance();
    if (mat != SM.ScintMat) return;

    const int iScint = (bTransport ? preP ->GetPhysicalVolume()->GetCopyNo()
                                   : postP->GetPhysicalVolume()->GetCopyNo() );

    SimModeNatRadTest * Mode = static_cast<SimModeNatRadTest*>(SM.SimMode);
    Mode->addEnergy(iScint, depo);
}

// ---

void SteppingAction_DepoStatMode::UserSteppingAction(const G4Step * step)
{
    const double depo = step->GetTotalEnergyDeposit(); // in MeV
    if (depo == 0) return;

    // note that energy deposition can be on exiting scintillator!

    const G4StepPoint * postP  = step->GetPostStepPoint();
    const G4StepPoint * preP   = step->GetPreStepPoint();

    bool bTransport = false;
    const G4VProcess  * proc = postP->GetProcessDefinedStep();
    if (proc) bTransport = ( (proc->GetProcessType() == fTransportation) );

    const G4VPhysicalVolume * referenceVolume = (bTransport ? preP ->GetPhysicalVolume()
                                                            : postP->GetPhysicalVolume() );

    const G4Material * mat = referenceVolume->GetLogicalVolume()->GetMaterial();

    SessionManager & SM = SessionManager::getInstance();
    if (mat != SM.ScintMat) return;

    const int iScint = referenceVolume->GetCopyNo();

    DepoStatMode * Mode = static_cast<DepoStatMode*>(SM.SimMode);
    Mode->addRecord(iScint, depo, postP->GetGlobalTime());
}

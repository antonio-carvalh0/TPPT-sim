#include "SensitiveDetectorScint.hh"
#include "SessionManager.hh"
#include "SimMode.hh"
#include "out.hh"

#include <sstream>

#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

G4bool SensitiveDetectorScint_SingleEvents::ProcessHits(G4Step* step, G4TouchableHistory*)
{  
    const double edep = step->GetTotalEnergyDeposit();
    if (edep == 0) return true;

    SessionManager & SM = SessionManager::getInstance();
    SimModeSingleEvents * Mode = static_cast<SimModeSingleEvents*>(SM.SimMode);

    const G4StepPoint * postP  = step->GetPostStepPoint();

    int iScint = postP->GetPhysicalVolume()->GetCopyNo();

    double & firstTime = Mode->ScintData[iScint][0];
    double & sumEnergy = Mode->ScintData[iScint][1];

    double time = postP->GetGlobalTime();
    if (time < firstTime || firstTime == 0) firstTime = time;
    sumEnergy += edep;

    return true;
}

// ---

#include <G4VProcess.hh>
G4bool SensitiveDetectorScint_MultipleEvents::ProcessHits(G4Step *step, G4TouchableHistory *)
{
    const double edep = step->GetTotalEnergyDeposit();
    if (edep == 0) return true;

    SessionManager & SM = SessionManager::getInstance();
    SimModeMultipleEvents * Mode = static_cast<SimModeMultipleEvents*>(SM.SimMode);

    const G4StepPoint * postP  = step->GetPostStepPoint();
    const G4StepPoint * preP   = step->GetPreStepPoint();

    bool bTransport = false;
    const G4VProcess  * proc = postP->GetProcessDefinedStep();
    if (proc) bTransport = ( (proc->GetProcessType() == fTransportation) );

    //There could be energy deposition on Transportation (charged particles)!
    //if (!bTransport && postP->GetPhysicalVolume()->GetName() != "Scint") out("AAAAAAAAAAAAAAAAAAAAAAAAA");

    const int iScint = (bTransport ? preP ->GetPhysicalVolume()->GetCopyNo()
                                   : postP->GetPhysicalVolume()->GetCopyNo() );

    DepositionNodeRecord newNode(postP->GetGlobalTime(), edep);
    std::vector<DepositionNodeRecord> & Nodes = Mode->DepositionData[iScint];
    if (!Mode->bDoCluster || Nodes.empty() || !Nodes.back().isCluster(newNode, Mode->MaxTimeDif))
    {
        if (Nodes.size() == Mode->MaxCapacity) Mode->saveData();
        Nodes.push_back(newNode);
    }
    else
        Nodes.back().merge(newNode);

    return true;
}

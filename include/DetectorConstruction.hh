#ifndef DetectorConstruction_H
#define DetectorConstruction_H

#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4VSolid;
class G4Material;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    G4VPhysicalVolume * Construct();

private:
    void defineMaterials();
    void addGDML();
    void addFSM();
    void addScintillators();
    void addBase();
    void addClosedStructure();
    void addSIPM();
    void addPCB();
    void addCopperStructure();
    void addCoolingAssemblies();

    G4LogicalVolume * createAssembly(int & iScint, G4RotationMatrix * AssemblyRot, G4ThreeVector AssemblyPos, double Angle, int headNumber, int iAssembly);
    void positionAssembly(G4RotationMatrix * rot, G4ThreeVector pos, double angle, int & iScint, int iAssembly, int headNumber);

    G4VSolid * solidEncaps = nullptr;
    G4VSolid * solidScint  = nullptr;

    G4Material * WorldMat        = nullptr;
    G4Material * EncapsMat       = nullptr;
    G4Material * BaseMat         = nullptr;
    G4Material * BasePlateMat    = nullptr;
    G4Material * CaseMat         = nullptr;
    G4Material * SIPMMat         = nullptr;
    G4Material * PCBMat          = nullptr;
    G4Material * CopperStructMat = nullptr;
    G4Material * CopperPipeMat   = nullptr;
    G4Material * WaterPipeMat    = nullptr;

    G4LogicalVolume * logicWorld = nullptr;
    G4LogicalVolume * logicScint = nullptr;

};

#endif //DetectorConstruction_H

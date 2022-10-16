#include "DetectorConstruction.hh"
#include "DetComp.hh"
#include "SensitiveDetectorScint.hh"
#include "SensitiveDetectorFSM.hh"
#include "SessionManager.hh"
#include "SimMode.hh"
#include "PhantomMode.hh"
#include "out.hh"

#include "G4SystemOfUnits.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Navigator.hh"
#include "G4SDManager.hh"
#include "Nozzle.hh"
#include "G4Trd.hh"

#ifdef USE_GDML
#include "G4GDMLParser.hh"
#endif

#define _USE_MATH_DEFINES

#include <math.h>

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    SessionManager & SM = SessionManager::getInstance();

    defineMaterials();

    G4Box * solidWorld   = new G4Box("World", 6000.0*mm, 6000.0*mm, 6000.0*mm);
    logicWorld   = new G4LogicalVolume(solidWorld, WorldMat, "World_LV");
    SM.physWorld = new G4PVPlacement(nullptr, {0, 0, 0}, logicWorld, "World_PV", nullptr, false, 0);
    logicWorld->SetVisAttributes(G4VisAttributes({0, 1, 0}));
    logicWorld->SetVisAttributes(false);

    if (SM.detectorContains(DetComp::GDML)) addGDML();

    G4LogicalVolume * phantom = SM.PhantomMode->definePhantom(logicWorld);
    if (phantom) SM.createPhantomRegion(phantom);

    if (SM.detectorContains(DetComp::Scintillators))     addScintillators();
    if (SM.detectorContains(DetComp::FirstStageMonitor)) addFSM();
    if (SM.detectorContains(DetComp::Base))              addBase();
    if (SM.detectorContains(DetComp::ClosedStructure))   addClosedStructure();
    if (SM.detectorContains(DetComp::SIPM))              addSIPM();
    if (SM.detectorContains(DetComp::PCB))               addPCB();
    if (SM.detectorContains(DetComp::CopperStructure))   addCopperStructure();
    if (SM.detectorContains(DetComp::CoolingAssemblies)) addCoolingAssemblies();
    if (SM.detectorContains(DetComp::Nozzle))
    {
        Nozzle nozzlemaker;
        nozzlemaker.constructNozzle(logicWorld);
    }

    return SM.physWorld;
}

void DetectorConstruction::defineMaterials()
{
    G4NistManager * man = G4NistManager::Instance();
    SessionManager & SM = SessionManager::getInstance();

    G4Material * matVacuum = man->FindOrBuildMaterial("G4_Galactic");

    G4Material * matAir    = man->FindOrBuildMaterial("G4_AIR");

    G4Material * matTeflon = man->FindOrBuildMaterial("G4_TEFLON");

    //LYSO:Ce - https://www.crystals.saint-gobain.com/sites/imdf.crystals.com/files/documents/lyso-material-data-sheet.pdf
    //The scintillating material should be LYSO, Lu(2-2x)Y(2x)SiO5:Ce and x should be < 0.03; the density should be at least 7.31 g/cm3
    //Considering x=0.02: Lu(1.96)Y(0.04)SiO5
    std::vector<G4int> natoms;
    std::vector<G4String> elements;
    elements.clear();
    elements.push_back("Lu"); natoms.push_back(49);
    elements.push_back("Y") ; natoms.push_back(1);
    elements.push_back("Si"); natoms.push_back(25);
    elements.push_back("O") ; natoms.push_back(125);
    G4Material * matLYSO = man->ConstructNewMaterial("LYSO", elements, natoms, 7.31*g/cm3);

    G4Material * matCerium = man->FindOrBuildMaterial("G4_Ce"); //<1% Cerium - https://authors.library.caltech.edu/8519/1/CHEieeetns05.pdf

    G4Material * matLYSOCe = new G4Material("LYSOCe", 7.31*g/cm3, 2);
    matLYSOCe -> AddMaterial(matLYSO, 99.0 * perCent);
    matLYSOCe -> AddMaterial(matCerium, 1.0 * perCent);

    //ABS: (C8H8·C4H6·C3H3N)n
    natoms.clear();
    elements.clear();
    elements.push_back("C"); natoms.push_back(15);
    elements.push_back("H"); natoms.push_back(17);
    elements.push_back("N"); natoms.push_back(1);
    G4Material * matABS = man->ConstructNewMaterial("ABS", elements, natoms, 1.07*g/cm3);

    //Copper
    G4Material * matCopper = man->FindOrBuildMaterial("G4_Cu");

    //Carbon
    G4Material * matCarbon = man->FindOrBuildMaterial("G4_C");

    //PCB
    natoms.clear();
    elements.clear();
    elements.push_back("C"); natoms.push_back(21);
    elements.push_back("H") ; natoms.push_back(25);
    elements.push_back("Cl") ; natoms.push_back(1);
    elements.push_back("O") ; natoms.push_back(5);
    G4Material * matEpoxy = man->ConstructNewMaterial("Epoxy", elements, natoms, 1.1*g/cm3);

    natoms.clear();
    elements.clear();
    elements.push_back("Si"); natoms.push_back(1);
    elements.push_back("O") ; natoms.push_back(2);
    G4Material * matFiberGlass = man->ConstructNewMaterial("FiberGlass", elements, natoms, 2.65*g/cm3);

    G4Material * matIron = man->FindOrBuildMaterial("G4_Fe");
    G4Material * matTin = man->FindOrBuildMaterial("G4_Sn");

    G4Material * matPCB = new G4Material("PCB", 4.57*g/cm3, 5);
    matPCB->AddMaterial(matEpoxy, 30.0*perCent);
    matPCB->AddMaterial(matFiberGlass, 30.0*perCent);
    matPCB->AddMaterial(matCopper, 30.0*perCent);
    matPCB->AddMaterial(matIron, 5.0*perCent);
    matPCB->AddMaterial(matTin, 5.0*perCent);

    //Silicon
    G4Material * matSilicon = man->FindOrBuildMaterial("G4_Si");

    //SiPM
    G4Material * matSIPM = new G4Material("SIPM", 4.20*g/cm3, 2);
    matSIPM->AddMaterial(matPCB, 91.07*perCent);
    matSIPM->AddMaterial(matSilicon, 8.93*perCent);

    //Water
    natoms.clear();
    elements.clear();
    elements.push_back("H"); natoms.push_back(2);
    elements.push_back("O"); natoms.push_back(1);
    G4Material * matWater = man->ConstructNewMaterial("Water", elements, natoms, 1.0*g/cm3);

    // Assigning materials to the detector components
    WorldMat        = matAir; // WorldMat        = matVacuum;
    SM.ScintMat     = matLYSOCe;
    EncapsMat       = matTeflon;
    BaseMat         = matABS;
    BasePlateMat    = matCopper;
    CaseMat         = matCarbon;
    SIPMMat         = matSIPM;
    PCBMat          = matPCB;
    CopperStructMat = matCopper;
    CopperPipeMat   = matCopper;
    WaterPipeMat    = matWater;
}

void DetectorConstruction::addGDML()
{
#ifdef USE_GDML
    SessionManager & SM = SessionManager::getInstance();

    G4GDMLParser parser;
    parser.Read(SM.GdmlFileName, false);
    G4VPhysicalVolume * gdmlWorldPV = parser.GetWorldVolume();

    int numD = gdmlWorldPV->GetLogicalVolume()->GetNoDaughters();
    out("Number of daughter volumes in the GDML world:", numD);
    for (int iD = 0; iD < numD; iD++)
    {
        G4VPhysicalVolume * d = gdmlWorldPV->GetLogicalVolume()->GetDaughter(iD);
        //out("->", d->GetName(), d->GetMultiplicity(),d->GetFrameTranslation(), d->GetTranslation());

        G4LogicalVolume * logic = d->GetLogicalVolume();
        new G4PVPlacement(nullptr, {0, 0, -SM.IsoCenterGDML}, logic, logic->GetName() + "_PV", logicWorld, false, 0);
    }
#else
    out("The framework was compiled without GDMl support\nTo activate it, uncomment add_compile_definitions(USE_GDML) line in CMakeLists.txt");
    exit(1234);
#endif
}

void DetectorConstruction::addFSM()
{
    SessionManager & SM = SessionManager::getInstance();

    double OuterRadius = 0.5 * SM.InnerDiam - 2.0*mm;
    double InnerRadius = OuterRadius - 1.0*mm;
    double Height      = 5.0 * SM.EncapsSizeX; // Margarida, please calculate the minimum size

    G4VSolid          * solidFSM = new G4Tubs("FSM_Cyl", InnerRadius, OuterRadius, 0.5 * Height, 0, 360.0*deg);
    G4LogicalVolume   * logicFSM = new G4LogicalVolume(solidFSM, WorldMat, "FSM");
    new G4PVPlacement(new CLHEP::HepRotation(90.0*deg, 0, 0), {0, 0, SM.GlobalZ0}, logicFSM, "FSM_PV", logicWorld, false, 0);
    logicFSM->SetVisAttributes(G4VisAttributes(G4Colour(1.0, 1.0, 1.0)));

    G4VSensitiveDetector * pSD_FSM = new SensitiveDetectorFSM("SD_FSM");
    G4SDManager::GetSDMpointer()->AddNewDetector(pSD_FSM);
    logicFSM->SetSensitiveDetector(pSD_FSM);
}

void DetectorConstruction::addScintillators()
{
    SessionManager & SM = SessionManager::getInstance();

    solidScint = new G4Box("Scint", 0.5 * SM.ScintSizeX, 0.5 * SM.ScintSizeY, 0.5 * SM.ScintSizeZ);
    logicScint = new G4LogicalVolume(solidScint, SM.ScintMat, "Scint"); //SiPM are interfaced at the local negative Z
    logicScint->SetVisAttributes(G4VisAttributes({0, 0, 1}));

    SM.createScintillatorRegion(logicScint);

    /*  just to check that positive Z is towards the phantom:
    G4Box * b   = new G4Box("b", 1.0*mm, 1.0*mm, 1.0*mm);
    G4LogicalVolume * lb  = new G4LogicalVolume(b, matVacuum, "bb");
    new G4PVPlacement(nullptr, {0, 0, 5}, lb, "ss", logicScint, false, 0);
    */

    solidEncaps = new G4Box("Encaps",  0.5 * SM.EncapsSizeX, 0.5 * SM.EncapsSizeY, 0.5 * SM.EncapsSizeZ);

    SM.ScintRecords.clear();

    int iAssembly = 0;
    int iScint    = 0;
    for (int iA = 0; iA < SM.NumSegments; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + SM.EncapsSizeZ) - SM.TeflonThick;
        double Angle  = SM.AngularStep * iA + SM.Angle0;
        double X = Radius * sin(Angle);
        double Y = Radius * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        for (int iZ = 0; iZ < SM.NumRows; iZ++)
        {
            double RowPitch = SM.EncapsSizeY + SM.RowGap;
            double Z = -0.5 * (SM.NumRows - 1) * RowPitch  +  iZ * RowPitch + SM.GlobalZ0;

            positionAssembly(rot,  G4ThreeVector( X,  Y, Z), Angle,             iScint, iAssembly++, 0);
            positionAssembly(rot1, G4ThreeVector(-X, -Y, Z), Angle + 0.5*M_PI , iScint, iAssembly++, 1);
        }
    }

    // Sensitive Detector
    G4VSensitiveDetector * pSD_Scint = SM.SimMode->getScintDetector();
    if (pSD_Scint)
    {
        G4SDManager::GetSDMpointer()->AddNewDetector(pSD_Scint);
        logicScint->SetSensitiveDetector(pSD_Scint);
    }

    SM.saveScintillatorTable(SM.WorkingDirectory + '/' + "LUT.txt");
}

G4LogicalVolume * DetectorConstruction::createAssembly(int & iScint, G4RotationMatrix * AssemblyRot, G4ThreeVector AssemblyPos, double Angle, int headNumber, int iAssembly)
{
    SessionManager & SM = SessionManager::getInstance();

    G4LogicalVolume * logicEncaps = new G4LogicalVolume(solidEncaps, EncapsMat, "Encaps");
    logicEncaps->SetVisAttributes(G4VisAttributes({0, 1, 1}));

    for (int ix = 0; ix < SM.NumScintX; ix++)
        for (int iy = 0; iy < SM.NumScintY; iy++)
        {
            double X = -0.5 * (SM.NumScintX - 1) * SM.ScintPitchX  +  SM.ScintPitchX * ix;
            double Y = -0.5 * (SM.NumScintY - 1) * SM.ScintPitchY  +  SM.ScintPitchY * iy;
            G4ThreeVector ScintPos(X, Y, -0.5*SM.TeflonThick);
            new G4PVPlacement(nullptr, ScintPos, logicScint, "Scint", logicEncaps, true, iScint++);

            ScintRecord rec;

            rec.CenterPos = (*AssemblyRot).inverse()(ScintPos);
            for (int i=0; i<3; i++) rec.CenterPos[i] += AssemblyPos[i];

            ScintPos[2] += 0.5 * SM.ScintSizeZ;
            rec.FacePos   = (*AssemblyRot).inverse()(ScintPos);
            for (int i=0; i<3; i++) rec.FacePos[i]   += AssemblyPos[i];

            rec.Angle = Angle;
            rec.HeadNumber = headNumber;
            rec.AssemblyNumber = iAssembly;

            SM.ScintRecords.push_back(rec);
        }

    return logicEncaps;
}

void DetectorConstruction::positionAssembly(G4RotationMatrix * rot, G4ThreeVector pos, double angle, int & iScint, int iAssembly, int headNumber)
{
    new G4PVPlacement(rot, pos, createAssembly(iScint, rot, pos, angle, headNumber, iAssembly), "Encaps"+std::to_string(iAssembly), logicWorld, true, iAssembly);
}

void DetectorConstruction::addBase()
{
    SessionManager & SM = SessionManager::getInstance();

    G4Tubs * solidBase   = new G4Tubs("Base", SM.BaseRMin, SM.BaseRMax, 0.5 * SM.BaseHeight, SM.Angle0 - 10.5 * deg, SM.BaseSegment);
    G4LogicalVolume * logicBase   = new G4LogicalVolume(solidBase, BaseMat, "Base");
    G4Colour grey(0.5, 0.5, 0.5);
    logicBase ->SetVisAttributes(new G4VisAttributes(grey));

    G4Tubs * solidBasePlate   = new G4Tubs("BasePlate", SM.BPlateRMin, SM.BPlateRMax, 0.5 * SM.BPlateHeight, SM.Angle0 - 8.5 * deg, SM.BPlateSegment);
    G4LogicalVolume * logicBasePlate   = new G4LogicalVolume(solidBasePlate, BasePlateMat, "BasePlate");
    G4Colour brown(0.45,0.25,0.0);
    logicBasePlate ->SetVisAttributes(new G4VisAttributes(brown));

    G4RotationMatrix * rot  = new CLHEP::HepRotation(90*deg, 0, 0);
    G4RotationMatrix * rot1 = new CLHEP::HepRotation(-90.0*deg, 0, 0);

    new G4PVPlacement(rot, {0, 0, -0.5 * (SM.SystHeight+SM.BaseHeight) - SM.BPlateHeight}, logicBase, "Base_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, -0.5 * (SM.SystHeight+SM.BaseHeight) - SM.BPlateHeight}, logicBase, "Base_PV", logicWorld, false, 0);
    new G4PVPlacement(rot, {0, 0, 0.5 * (SM.SystHeight+SM.BaseHeight) + SM.BPlateHeight}, logicBase, "Base_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, 0.5 * (SM.SystHeight+SM.BaseHeight) + SM.BPlateHeight}, logicBase, "Base_PV", logicWorld, false, 0);

    new G4PVPlacement(rot, {0, 0, -0.5 * (SM.SystHeight + SM.BPlateHeight)}, logicBasePlate, "BasePlate_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, -0.5 * (SM.SystHeight + SM.BPlateHeight)}, logicBasePlate, "BasePlate_PV", logicWorld, false, 0);
    new G4PVPlacement(rot, {0, 0, 0.5 * (SM.SystHeight + SM.BPlateHeight)}, logicBasePlate, "BasePlate_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, 0.5 * (SM.SystHeight + SM.BPlateHeight)}, logicBasePlate, "BasePlate_PV", logicWorld, false, 0);
}

void DetectorConstruction::addClosedStructure()
{
    SessionManager & SM = SessionManager::getInstance();

    G4Tubs * solidInnerWall   = new G4Tubs("InnerWall", SM.BaseRMin, SM.BaseRMin + SM.InnerWallThick, 0.5 * SM.SystHeight + SM.BPlateHeight, SM.Angle0 - 10.5 * deg, SM.WallsSegment);
    G4LogicalVolume * logicInnerWall   = new G4LogicalVolume(solidInnerWall, CaseMat, "InnerWall");
    G4Colour grey(0.5, 0.5, 0.5);
    logicInnerWall ->SetVisAttributes(new G4VisAttributes(grey));

    G4Tubs * solidOuterWall   = new G4Tubs("OuterWall", SM.BaseRMax - SM.OuterWallThick, SM.BaseRMax, 0.5 * SM.SystHeight + SM.BPlateHeight, SM.Angle0 - 10.5 * deg, SM.WallsSegment);
    G4LogicalVolume * logicOuterWall   = new G4LogicalVolume(solidOuterWall, CaseMat, "OuterWall");
    logicOuterWall ->SetVisAttributes(new G4VisAttributes(grey));

    G4Tubs * solidSideWall   = new G4Tubs("SideWall", SM.BaseRMin + SM.InnerWallThick, SM.BaseRMax - SM.OuterWallThick, 0.5 * SM.SystHeight + SM.BPlateHeight, SM.Angle0 - 10.5 * deg, SM.SideWallSegment);
    G4LogicalVolume * logicSideWall   = new G4LogicalVolume(solidSideWall, CaseMat, "SideWall");
    logicSideWall ->SetVisAttributes(new G4VisAttributes(grey));

    G4Tubs * solidSideWall2   = new G4Tubs("SideWall2", SM.BaseRMin + SM.InnerWallThick, SM.BaseRMax - SM.OuterWallThick, 0.5 * SM.SystHeight + SM.BPlateHeight, SM.Angle0 + 18.5 * deg, SM.SideWallSegment);
    G4LogicalVolume * logicSideWall2   = new G4LogicalVolume(solidSideWall2, CaseMat, "SideWall2");
    logicSideWall2 ->SetVisAttributes(new G4VisAttributes(grey));

    G4RotationMatrix * rot  = new CLHEP::HepRotation(90.0*deg, 0, 0);
    G4RotationMatrix * rot1 = new CLHEP::HepRotation(-90.0*deg, 0, 0);
    G4RotationMatrix * rot2 = new CLHEP::HepRotation(180.0*deg, 0, 0);
    G4RotationMatrix * rot3 = new CLHEP::HepRotation(360.0*deg, 0, 0);

    new G4PVPlacement(rot, {0, 0, SM.GlobalZ0}, logicInnerWall, "InnerWall_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, SM.GlobalZ0}, logicInnerWall, "InnerWall_PV", logicWorld, false, 0);
    new G4PVPlacement(rot, {0, 0, SM.GlobalZ0}, logicOuterWall, "OuterWall_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, SM.GlobalZ0}, logicOuterWall, "OuterWall_PV", logicWorld, false, 0);
    new G4PVPlacement(rot, {0, 0, SM.GlobalZ0}, logicSideWall, "SideWall_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, SM.GlobalZ0}, logicSideWall, "SideWall_PV", logicWorld, false, 0);
    new G4PVPlacement(rot2, {0, 0, SM.GlobalZ0}, logicSideWall2, "SideWall_PV", logicWorld, false, 0);
    new G4PVPlacement(rot3, {0, 0, SM.GlobalZ0}, logicSideWall2, "SideWall_PV", logicWorld, false, 0);
}

void DetectorConstruction::addSIPM()
{
    SessionManager & SM = SessionManager::getInstance();

    G4Box * solidSIPM   = new G4Box("SIPM", 0.5 * SM.SIPMSizeX, 0.5 * SM.SIPMSizeY, 0.5 * SM.SIPMSizeZ);
    G4LogicalVolume * logicSIPM   = new G4LogicalVolume(solidSIPM, SIPMMat, "SIPM");
    G4Colour grey(0.5, 0.5, 0.5);
    logicSIPM ->SetVisAttributes(new G4VisAttributes(grey));

    for (int iA = 0; iA < SM.NumSegments; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + 2 * SM.EncapsSizeZ + SM.SIPMSizeZ) - SM.TeflonThick;
        double Angle  = SM.AngularStep * iA + SM.Angle0;
        double X = Radius * sin(Angle);
        double Y = Radius * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        for (int iZ = 0; iZ < SM.NumRows; iZ++)
        {
            double RowPitch = SM.EncapsSizeY + SM.RowGap;
            double Z = -0.5 * (SM.NumRows - 1) * RowPitch  +  iZ * RowPitch + SM.GlobalZ0;

            new G4PVPlacement(rot, G4ThreeVector( X,  Y, Z), logicSIPM, "SIPM_PV", logicWorld, false, 0);
            new G4PVPlacement(rot1, G4ThreeVector(-X, -Y, Z), logicSIPM, "SIPM_PV", logicWorld, false, 0);
        }
    }
}

void DetectorConstruction::addPCB()
{
    SessionManager & SM = SessionManager::getInstance();

    G4Box * solidPCB1   = new G4Box("PCB1", 0.5 * SM.PCB1SizeX, 0.5 * SM.PCB1SizeY, 0.5 * SM.PCB1SizeZ);
    G4LogicalVolume * logicPCB1   = new G4LogicalVolume(solidPCB1, PCBMat, "PCB1");
    logicPCB1 ->SetVisAttributes(G4VisAttributes({0, 1, 0}));

    G4Box * solidPCB2   = new G4Box("PCB2", 0.5 * SM.PCB2SizeX, 0.5 * SM.PCB2SizeY, 0.5 * SM.PCB2SizeZ);
    G4LogicalVolume * logicPCB2   = new G4LogicalVolume(solidPCB2, PCBMat, "PCB2");
    logicPCB2 ->SetVisAttributes(G4VisAttributes({0, 1, 0}));

    G4Box * solidPCB3   = new G4Box("PCB3", 0.5 * SM.PCB3SizeX, 0.5 * SM.PCB3SizeY, 0.5 * SM.PCB3SizeZ);
    G4LogicalVolume * logicPCB3   = new G4LogicalVolume(solidPCB3, PCBMat, "PCB3");
    logicPCB3 ->SetVisAttributes(G4VisAttributes({0, 0, 1}));

    for (int iA = 0; iA < SM.NumSegments; iA++)
    {
        double Radius1 = 0.5 * (SM.InnerDiam + 2 * SM.EncapsSizeZ + 2 * SM.SIPMSizeZ + 2 * SM.SIPMPCB1Gap + SM.PCB1SizeZ) - SM.TeflonThick;
        double Radius2 = 0.5 * (SM.InnerDiam + 2 * SM.EncapsSizeZ + 2 * SM.SIPMSizeZ + 2 * SM.SIPMPCB1Gap + 2 * SM.PCB1SizeZ + SM.PCB2SizeZ + SM.MiddlePCBGap) - SM.TeflonThick;
        double Radius3 = 0.5 * (SM.InnerDiam + 2 * SM.EncapsSizeZ + 2 * SM.SIPMSizeZ + 2 * SM.SIPMPCB1Gap + 2 * SM.PCB1SizeZ + 2 * SM.PCB2SizeZ + 2 * SM.MiddlePCBGap + SM.PCB3SizeZ) - SM.TeflonThick;
        double Angle  = SM.AngularStep * iA + SM.Angle0;
        double X1 = Radius1 * sin(Angle);
        double Y1 = Radius1 * cos(Angle);
        double X2 = Radius2 * sin(Angle);
        double Y2 = Radius2 * cos(Angle);
        double X3 = Radius3 * sin(Angle);
        double Y3 = Radius3 * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        for (int iZ = 0; iZ < SM.PCB1PCB3NumRows; iZ++)
        {
            double RowPitch = SM.PCB1PCB3Pitch + SM.RowGap;
            double Z = -0.5 * (SM.PCB1PCB3NumRows - 1) * RowPitch  +  iZ * RowPitch + SM.GlobalZ0;

            new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, Z), logicPCB1, "PCB1_PV", logicWorld, false, 0);
            new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, Z), logicPCB1, "PCB1_PV", logicWorld, false, 0);

            new G4PVPlacement(rot, G4ThreeVector( X3,  Y3, Z), logicPCB3, "PCB3_PV", logicWorld, false, 0);
            new G4PVPlacement(rot1, G4ThreeVector(-X3, -Y3, Z), logicPCB3, "PCB3_PV", logicWorld, false, 0);
        }

        new G4PVPlacement(rot, G4ThreeVector( X2,  Y2, SM.PCB2Z1), logicPCB2, "PCB2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X2, -Y2, SM.PCB2Z1), logicPCB2, "PCB2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X2,  Y2, SM.PCB2Z2), logicPCB2, "PCB2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X2, -Y2, SM.PCB2Z2), logicPCB2, "PCB2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X2,  Y2, SM.PCB2Z3), logicPCB2, "PCB2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X2, -Y2, SM.PCB2Z3), logicPCB2, "PCB2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X2,  Y2, SM.PCB2Z4), logicPCB2, "PCB2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X2, -Y2, SM.PCB2Z4), logicPCB2, "PCB2_PV", logicWorld, false, 0);
    }
}

void DetectorConstruction::addCopperStructure()
{
    SessionManager & SM = SessionManager::getInstance();

    //Columns in between the scintillators and PCBs (until the copper horizontal "connectors"):
    G4Trd * solidCopperColumn1   = new G4Trd("CopperColumn1", 0.5 * SM.Column1SixeX1, 0.5 * SM.Column1SizeX2, 0.5 * SM.Column1SizeY, 0.5 * SM.Column1SizeY, 0.5 * SM.Column1SizeZ);
    G4LogicalVolume * logicCopperColumn1   = new G4LogicalVolume(solidCopperColumn1, CopperStructMat, "CopperColumn1");
    G4Colour brown(0.45,0.25,0.0);
    logicCopperColumn1 ->SetVisAttributes(new G4VisAttributes(brown));

    for (int iA = 0; iA < 13; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + 2 * SM.Column1RSurf + SM.Column1SizeZ);
        double Angle  = SM.AngularStep * iA + SM.Angle0 - 4.5*deg;
        double X1 = Radius * sin(Angle);
        double Y1 = Radius * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.GlobalZ0), logicCopperColumn1, "CopperColumn1_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.GlobalZ0), logicCopperColumn1, "CopperColumn1_PV", logicWorld, false, 0);
    }

    G4Trd * solidCopperColumn2   = new G4Trd("CopperColumn2", 0.5 * SM.Column2SixeX1, 0.5 * SM.Column2SizeX2, 0.5 * SM.Column2SizeY, 0.5 * SM.Column2SizeY, 0.5 * SM.Column2SizeZ);
    G4LogicalVolume * logicCopperColumn2   = new G4LogicalVolume(solidCopperColumn2, CopperStructMat, "CopperColumn2");
    logicCopperColumn2 ->SetVisAttributes(new G4VisAttributes(brown));

    for (int iA = 0; iA < 13; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + 2 * SM.Column1RSurf + 2 * SM.Column1SizeZ + SM.Column2SizeZ);
        double Angle  = SM.AngularStep * iA + SM.Angle0 - 4.5 * deg;
        double X1 = Radius * sin(Angle);
        double Y1 = Radius * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.GlobalZ0), logicCopperColumn2, "CopperColumn2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.GlobalZ0), logicCopperColumn2, "CopperColumn2_PV", logicWorld, false, 0);
    }

    G4Trd * solidCopperColumn3   = new G4Trd("CopperColumn3", 0.5 * SM.Column3SixeX1, 0.5 * SM.Column3SizeX2, 0.5 * SM.Column3SizeY, 0.5 * SM.Column3SizeY, 0.5 * SM.Column3SizeZ);
    G4LogicalVolume * logicCopperColumn3   = new G4LogicalVolume(solidCopperColumn3, CopperStructMat, "CopperColumn3");
    logicCopperColumn3 ->SetVisAttributes(new G4VisAttributes(brown));

    for (int iA = 0; iA < 13; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + 2 * SM.Column1RSurf + 2 * SM.Column1SizeZ + 2 * SM.Column2SizeZ + SM.Column3SizeZ);
        double Angle  = SM.AngularStep * iA + SM.Angle0 - 4.5 * deg;
        double X1 = Radius * sin(Angle);
        double Y1 = Radius * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.GlobalZ0), logicCopperColumn3, "CopperColumn3_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.GlobalZ0), logicCopperColumn3, "CopperColumn3_PV", logicWorld, false, 0);
    }

    //Copper pieces that connect Column3 to the copper horizontal "connectors"
    G4Trd * solidCopperPiece1   = new G4Trd("CopperPiece1", 0.5 * SM.Piece1SizeX1, 0.5 * SM.Piece1SizeX2, 0.5 * SM.Piece1SizeY, 0.5 * SM.Piece1SizeY, 0.5 * SM.Piece1SizeZ);
    G4Trd * solidCopperPiece2   = new G4Trd("CopperPiece2", 0.5 * SM.Piece2SizeX1, 0.5 * SM.Piece2SizeX2, 0.5 * SM.Piece2SizeY, 0.5 * SM.Piece2SizeY, 0.5 * SM.Piece2SizeZ);
    G4LogicalVolume * logicCopperPiece1   = new G4LogicalVolume(solidCopperPiece1, CopperStructMat, "CopperPiece1");
    G4LogicalVolume * logicCopperPiece2   = new G4LogicalVolume(solidCopperPiece2, CopperStructMat, "CopperPiece2");
    logicCopperPiece1 ->SetVisAttributes(new G4VisAttributes(brown));
    logicCopperPiece2 ->SetVisAttributes(new G4VisAttributes(brown));

    for (int iA = 0; iA < 13; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + 2 * SM.Column1RSurf + 2 * SM.Column1SizeZ + 2 * SM.Column2SizeZ + 2 * SM.Column3SizeZ + SM.Piece1SizeZ);
        double Angle  = SM.AngularStep * iA + SM.Angle0 - 4.5 * deg;
        double X1 = Radius * sin(Angle);
        double Y1 = Radius * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.Piece1Z1), logicCopperPiece1, "CopperPiece1_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.Piece1Z1), logicCopperPiece1, "CopperPiece1_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.Piece1Z2), logicCopperPiece1, "CopperPiece1_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.Piece1Z2), logicCopperPiece1, "CopperPiece1_PV", logicWorld, false, 0);

        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.Piece2Z1), logicCopperPiece2, "CopperPiece2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.Piece2Z1), logicCopperPiece2, "CopperPiece2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.Piece2Z2), logicCopperPiece2, "CopperPiece2_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.Piece2Z2), logicCopperPiece2, "CopperPiece2_PV", logicWorld, false, 0);
    }

    G4Trd * solidCopperPiece3   = new G4Trd("CopperPiece3", 0.5 * SM.Piece3SizeX1, 0.5 * SM.Piece3SizeX2, 0.5 * SM.Piece3SizeY, 0.5 * SM.Piece3SizeY, 0.5 * SM.Piece3SizeZ);
    G4Trd * solidCopperPiece4   = new G4Trd("CopperPiece4", 0.5 * SM.Piece4SizeX1, 0.5 * SM.Piece4SizeX2, 0.5 * SM.Piece4SizeY, 0.5 * SM.Piece4SizeY, 0.5 * SM.Piece4SizeZ);
    G4LogicalVolume * logicCopperPiece3   = new G4LogicalVolume(solidCopperPiece3, CopperStructMat, "CopperPiece3");
    logicCopperPiece3 ->SetVisAttributes(new G4VisAttributes(brown));
    G4LogicalVolume * logicCopperPiece4   = new G4LogicalVolume(solidCopperPiece4, CopperStructMat, "CopperPiece4");
    logicCopperPiece4 ->SetVisAttributes(new G4VisAttributes(brown));

    for (int iA = 0; iA < 13; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + 2 * SM.Column1RSurf + 2 * SM.Column1SizeZ + 2 * SM.Column2SizeZ + 2 * SM.Column3SizeZ + 2 * SM.Piece1SizeZ + SM.Piece3SizeZ);
        double Angle  = SM.AngularStep * iA + SM.Angle0 - 4.5 * deg;
        double X1 = Radius * sin(Angle);
        double Y1 = Radius * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.Piece3Z1), logicCopperPiece3, "CopperPiece3_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.Piece3Z1), logicCopperPiece3, "CopperPiece3_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.Piece3Z2), logicCopperPiece3, "CopperPiece3_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.Piece3Z2), logicCopperPiece3, "CopperPiece3_PV", logicWorld, false, 0);

        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.Piece4Z1), logicCopperPiece4, "CopperPiece4_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.Piece4Z1), logicCopperPiece4, "CopperPiece4_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.Piece4Z2), logicCopperPiece4, "CopperPiece4_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.Piece4Z2), logicCopperPiece4, "CopperPiece4_PV", logicWorld, false, 0);
    }

    //Connectors:
    G4Box * solidOuterConnector   = new G4Box("OuterConnector", 0.5 * SM.ConnectorSizeX, 0.5 * SM.OutConnectSizeY, 0.5 * SM.ConnectorSizeZ);
    G4LogicalVolume * logicOuterConnector   = new G4LogicalVolume(solidOuterConnector, CopperStructMat, "OuterConnector");
    logicOuterConnector ->SetVisAttributes(new G4VisAttributes(brown));

    G4Box * solidInnerConnector   = new G4Box("InnerConnector", 0.5 * SM.ConnectorSizeX, 0.5 * SM.InConnectSizeY, 0.5 * SM.ConnectorSizeZ);
    G4LogicalVolume * logicInnerConnector   = new G4LogicalVolume(solidInnerConnector, CopperStructMat, "InnerConnector");
    logicInnerConnector ->SetVisAttributes(new G4VisAttributes(brown));

    for (int iA = 0; iA < 13; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + 2 * SM.Column1RSurf + 2 * SM.Column1SizeZ + 2 * SM.Column2SizeZ + 2 * SM.Column3SizeZ + 2 * SM.Piece1SizeZ + 2 * SM.Piece3SizeZ + SM.ConnectorSizeZ);
        double Angle  = SM.AngularStep * iA + SM.Angle0 - 4.5 * deg;
        double X = Radius * sin(Angle);
        double Y = Radius * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        new G4PVPlacement(rot, G4ThreeVector( X,  Y, SM.ConnectorZ1), logicOuterConnector, "OuterConnector_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X, -Y, SM.ConnectorZ1), logicOuterConnector, "OuterConnector_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X,  Y, SM.ConnectorZ2), logicOuterConnector, "OuterConnector_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X, -Y, SM.ConnectorZ2), logicOuterConnector, "OuterConnector_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X,  Y, SM.ConnectorZ3), logicInnerConnector, "InnerConnector_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X, -Y, SM.ConnectorZ3), logicInnerConnector, "InnerConnector_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X,  Y, SM.ConnectorZ4), logicInnerConnector, "InnerConnector_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X, -Y, SM.ConnectorZ4), logicInnerConnector, "InnerConnector_PV", logicWorld, false, 0);
    }

    //External columns:
    G4Box * solidCopperColumn4   = new G4Box("CopperColumn4", 0.5 * SM.ExtColumnSizeX, 0.5 * SM.ExtColumnSizeY, 0.5 * SM.ExtColumnSizeZ);
    G4LogicalVolume * logicCopperColumn4   = new G4LogicalVolume(solidCopperColumn4, CopperStructMat, "CopperColumn4");
    logicCopperColumn4 ->SetVisAttributes(new G4VisAttributes(brown));

    for (int iA = 0; iA < 13; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + 2 * SM.Column1RSurf + 2 * SM.Column1SizeZ + 2 * SM.Column2SizeZ + 2 * SM.Column3SizeZ + 2 * SM.Piece1SizeZ + 2 * SM.Piece3SizeZ + 2 * SM.ConnectorSizeZ + SM.ExtColumnSizeY);
        double Angle  = SM.AngularStep * iA + SM.Angle0 - 4.5 * deg;
        double X1 = Radius * sin(Angle);
        double Y1 = Radius * cos(Angle);
        double Z = SM.GlobalZ0;
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 0*deg, 0);

        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, Z), logicCopperColumn4, "CopperColumn4_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, Z), logicCopperColumn4, "CopperColumn4_PV", logicWorld, false, 0);
    }

    //Holders:
    G4Box * solidHolder   = new G4Box("Holder", 0.5 * SM.HolderSizeX, 0.5 * SM.HolderSizeY, 0.5 * SM.HolderSizeZ);
    G4LogicalVolume * logicHolder   = new G4LogicalVolume(solidHolder, CopperStructMat, "Holder");
    logicHolder ->SetVisAttributes(new G4VisAttributes(brown));

    for (int iA = 0; iA < 13; iA++)
    {
        double Radius = 0.5 * (SM.InnerDiam + 2 * SM.Column1RSurf + 2 * SM.Column1SizeZ + 2 * SM.Column2SizeZ + 2 * SM.Column3SizeZ + 2 * SM.Piece1SizeZ + 2 * SM.Piece3SizeZ + 2 * SM.ConnectorSizeZ + 2 * SM.ExtColumnSizeY + SM.HolderSizeZ);
        double Angle  = SM.AngularStep * iA + SM.Angle0 - 4.5 * deg;
        double X1 = Radius * sin(Angle);
        double Y1 = Radius * cos(Angle);
        G4RotationMatrix * rot  = new CLHEP::HepRotation(-Angle,             90.0*deg, 0);
        G4RotationMatrix * rot1 = new CLHEP::HepRotation(-Angle + 180.0*deg, 90.0*deg, 0);

        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.HolderZ1), logicHolder, "Holder_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.HolderZ1), logicHolder, "Holder_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.HolderZ2), logicHolder, "Holder_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.HolderZ2), logicHolder, "Holder_PV", logicWorld, false, 0);
        new G4PVPlacement(rot, G4ThreeVector( X1,  Y1, SM.HolderZ3), logicHolder, "Holder_PV", logicWorld, false, 0);
        new G4PVPlacement(rot1, G4ThreeVector(-X1, -Y1, SM.HolderZ3), logicHolder, "Holder_PV", logicWorld, false, 0);
    }
}

void DetectorConstruction::addCoolingAssemblies()
{
    SessionManager & SM = SessionManager::getInstance();

    //Copper "box" structure that is crossed by water:
    G4Tubs * solidCopperPipeHolder   = new G4Tubs("CopperPipeHolder", SM.CopperBoxRmin, SM.CopperBoxRmax, 0.5 * SM.CopperBoxHeight, SM.Angle0 - 9.5 * deg, SM.CoolingSegment);
    G4LogicalVolume * logicCopperPipeHolder   = new G4LogicalVolume(solidCopperPipeHolder, CopperPipeMat, "CopperPipeHolder");
    G4Colour brown(0.45,0.25,0.0);
    logicCopperPipeHolder ->SetVisAttributes(new G4VisAttributes(brown));

    G4RotationMatrix * rot  = new CLHEP::HepRotation(90*deg, 0, 0);
    G4RotationMatrix * rot1 = new CLHEP::HepRotation(-90.0*deg, 0, 0);

    new G4PVPlacement(rot, {0, 0, SM.CopperBoxZ1}, logicCopperPipeHolder, "CopperPipeHolder_PV", logicWorld, false, 0);
    new G4PVPlacement(rot, {0, 0, SM.CopperBoxZ2}, logicCopperPipeHolder, "CopperPipeHolder_PV", logicWorld, false, 0);
    new G4PVPlacement(rot, {0, 0, SM.CopperBoxZ3}, logicCopperPipeHolder, "CopperPipeHolder_PV", logicWorld, false, 0);
    new G4PVPlacement(rot, {0, 0, SM.CopperBoxZ4}, logicCopperPipeHolder, "CopperPipeHolder_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, SM.CopperBoxZ1}, logicCopperPipeHolder, "CopperPipeHolder_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, SM.CopperBoxZ2}, logicCopperPipeHolder, "CopperPipeHolder_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, SM.CopperBoxZ3}, logicCopperPipeHolder, "CopperPipeHolder_PV", logicWorld, false, 0);
    new G4PVPlacement(rot1, {0, 0, SM.CopperBoxZ4}, logicCopperPipeHolder, "CopperPipeHolder_PV", logicWorld, false, 0);

    //Water that crosses the copper holder:
    G4Tubs * solidWaterPipe   = new G4Tubs("WaterPipe", SM.WaterRmin, SM.WaterRmax, 0.5 * SM.WaterHeight, SM.Angle0 - 9.5 * deg, SM.CoolingSegment);
    G4LogicalVolume * logicWaterPipe   = new G4LogicalVolume(solidWaterPipe, WaterPipeMat, "WaterPipe");
    logicWaterPipe->SetVisAttributes(G4VisAttributes(G4Colour(0.0, 1.0, 1.0)));

    new G4PVPlacement(0, {0, 0, 0}, logicWaterPipe, "WaterPipe_PV", logicCopperPipeHolder, false, 0);
}

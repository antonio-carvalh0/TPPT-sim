// Adapted in 2022 from original file:
// ******************
// PTCH Hitachi Document No. MDA-40R-0122, rev. 0
// G. O. Sawakuchi
// April 1, 2008
// ******************

#include "Nozzle.hh"

#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

Nozzle::Nozzle()
{
    defineMaterials();
}

void Nozzle::defineMaterials()
{
    G4double z, a;

    G4Element* elH  = new G4Element("elHydrogen",   "elH",  z = 1.,  a = 1.008*g/mole);
    G4Element* elC  = new G4Element("elCarbon",     "elC",  z = 6.,  a = 12.011*g/mole);
    G4Element* elN  = new G4Element("elNitrogen",   "elN",  z = 7.,  a = 14.007*g/mole);
    G4Element* elO  = new G4Element("elOxygen"  ,   "elO",  z = 8.,  a = 15.999*g/mole);
    G4Element* elF  = new G4Element("elFluorine",  	"elF",  z = 9.,  a = 18.998*g/mole);
    G4Element* elMg = new G4Element("elMagnesium", 	"elMg", z = 12., a = 24.305*g/mole);
    G4Element* elAl = new G4Element("elAluminum",  	"elAl", z = 13., a = 26.981*g/mole);
    G4Element* elSi = new G4Element("Silicon",		"elSi", z = 14., a = 28.085*g/mole);
    G4Element* elP  = new G4Element("Phosphorus",	"elP",  z = 15., a = 30.974*g/mole);
    G4Element* elS  = new G4Element("Sulfur",    	"elS",  z = 16., a = 32.065*g/mole);
    G4Element* elAr = new G4Element("elArgon",     	"elAr", z = 18., a = 39.948*g/mole);
    G4Element* elCa = new G4Element("elCalcium",   	"elCa", z = 20., a = 40.078*g/mole);
    G4Element* elCr = new G4Element("elChromium",   "elCr", z = 24., a = 51.996*g/mole);
    G4Element* elMn = new G4Element("elManganese",  "elMn", z = 25., a = 54.938*g/mole);
    G4Element* elFe = new G4Element("elIron",       "elFe", z = 26., a = 55.847*g/mole);
    G4Element* elNi = new G4Element("elNickel",     "elNi", z = 28., a = 58.693*g/mole);
    G4Element* elMo = new G4Element("elMolybdenum", "elMo", z = 42., a = 95.940*g/mole);

    G4double density, temperature, pressure;
    G4int    ncomponents, natoms;
    G4double fractionmass;

    density     = universe_mean_density;    //from PhysicalConstants.h
    pressure    = 3.e-18*pascal;
    temperature = 2.73*kelvin;

    Vacuum = new G4Material("Galactic",z= 1,a= 1.008*g/mole,density,kStateGas,temperature,pressure);
    Helium = new G4Material("Helium",          z=2., a= 4.003*g/mole, density= 0.1785*mg/cm3);

    Air    = new G4Material("Air",  density= 1.20479*mg/cm3, ncomponents=4);
    Air->AddElement(elC,  fractionmass=0.000124);
    Air->AddElement(elN,  fractionmass=0.755267);
    Air->AddElement(elO,  fractionmass=0.231782);
    Air->AddElement(elAr, fractionmass=0.012827);
    Air->GetIonisation()->SetMeanExcitationEnergy(85.7*eV);
/*
    Polyethylene =            new G4Material("Polyethylene",                           density = 0.94 * g/cm3 ,ncomponents=2);
    Polyethylene -> AddElement(elH,0.14);
    Polyethylene -> AddElement(elC,0.86);

    H2O =            new G4Material("Water",                           density= 1.000*g/cm3, ncomponents=2);
    H2O->AddElement(elH, natoms=2);
    H2O->AddElement(elO, natoms=1);
    H2O->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);

    // PMMA C5H8O2 ( Acrylic )
    PMMA =     new G4Material("PMMA", density =  1.19*g/cm3, ncomponents=3);
    PMMA->AddElement(elC, 5);
    PMMA->AddElement(elH, 8);
    PMMA->AddElement(elO, 2);

    TissuePlastic =    new G4Material("TissuePlastic",                        density= 1.12700*g/cm3, ncomponents=6);
    TissuePlastic->AddElement(elH, fractionmass=0.101327);
    TissuePlastic->AddElement(elC, fractionmass=0.775501);
    TissuePlastic->AddElement(elN, fractionmass=0.035057);
    TissuePlastic->AddElement(elO, fractionmass=0.052316);
    TissuePlastic->AddElement(elF, fractionmass=0.017422);
    TissuePlastic->AddElement(elCa,fractionmass=0.018378);
*/
    Polyimide = new G4Material("Polyimide", density= 1.42*g/cm3, ncomponents=4);
    Polyimide->AddElement(elH, fractionmass=0.026362);
    Polyimide->AddElement(elC, fractionmass=0.691133);
    Polyimide->AddElement(elN, fractionmass=0.073270);
    Polyimide->AddElement(elO, fractionmass=0.209235);
/*
    Bone = new G4Material("CompactBoneICRU",   density = 1.85*g/cm3, ncomponents = 8);
    Bone -> AddElement(elH, fractionmass=0.063984);
    Bone -> AddElement(elC, fractionmass=0.278000);
    Bone -> AddElement(elN, fractionmass=0.027000);
    Bone -> AddElement(elO, fractionmass=0.410016);
    Bone -> AddElement(elMg,fractionmass=0.002000);
    Bone -> AddElement(elP, fractionmass=0.070000);
    Bone -> AddElement(elS, fractionmass=0.002000);
    Bone -> AddElement(elCa,fractionmass=0.147000 );
    Bone -> GetIonisation()->SetMeanExcitationEnergy(91.9*eV);
*/
    Aluminum    = new G4Material("Aluminum", z=13., a= 26.98*g/mole,  density= 2.69890*g/cm3);

    Ceramics = new G4Material("Ceramics", density = 3.65*g/cm3, ncomponents = 2);
    Ceramics->AddElement(elAl, fractionmass=0.4);
    Ceramics->AddElement(elO , fractionmass=0.6);

//    Titanium    =             new G4Material("Titanium",                               z=22., a= 47.867*g/mole, density= 4.54*g/cm3);

    Stainless = new G4Material("Stainless", density = 7.85*g/cm3, ncomponents = 8);
    Stainless->AddElement(elC , fractionmass=0.000098);
    Stainless->AddElement(elN , fractionmass=0.000114);
    Stainless->AddElement(elSi, fractionmass=0.002288);
    Stainless->AddElement(elCr, fractionmass=0.170847);
    Stainless->AddElement(elMn, fractionmass=0.012432);
    Stainless->AddElement(elFe, fractionmass=0.722478);
    Stainless->AddElement(elNi, fractionmass=0.090874);
    Stainless->AddElement(elMo, fractionmass=0.000868);

//    Iron        =             new G4Material("Iron",                                   z=26., a= 55.847*g/mole,  density= 7.874*g/cm3);

    Copper   = new G4Material("Copper", z=29., a= 63.546*g/mole,  density= 8.960*g/cm3);

    Tungsten = new G4Material("Tungsten", z=74., a= 183.85*g/mole, density= 19.30*g/cm3);

//    Gold        =             new G4Material("Gold",                                   z=79., a= 196.97*g/mole, density= 19.32*g/cm3);

//    Lead        =       new G4Material("Lead",                               z=82., a= 207.19*g/mole, density= 11.35*g/cm3);
}

void Nozzle::constructNozzle(G4LogicalVolume * mother)
{
    // Construct beam line components
    ProfileMonitor(mother);
    HeliumChamber(mother);
    MainDoseMonitor(mother);
    SubDoseMonitor(mother);
    SpotPositionMonitor(mother);
}

//*********************************************************
//*********************************************************
// BEAM PROFILE MONITOR (Multi wire ionization chamber)
// (Based on design in MDA-40E-0122, p. 56)
//*********************************************************
//*********************************************************

void Nozzle::ProfileMonitor(G4LogicalVolume * mother)
{
    // Outer boundary of profile monitor
    G4double PMsizeX = 12./2  *cm;
    G4double PMsizeY = 12./2  *cm;
    G4double PMsizeZ =  2./2  *cm;

    G4double PMposX     = 0.    *cm;
    G4double PMposY     = 0.    *cm;
    G4double PMposZ     = 319.6 *cm;

    G4Box             * profileMonitor = new G4Box("profileMonitor", PMsizeX, PMsizeY, PMsizeZ);
    G4LogicalVolume   * profileMonitorLogicalVolume = new G4LogicalVolume(profileMonitor, Air, "profileMonitorLog");
    //G4VPhysicalVolume * profileMonitorPhys = new G4PVPlacement(0, {PMposX,PMposY,PMposZ}, "profileMonitorPhys", profileMonitorLogicalVolume, mother, false, 0);
    G4VPhysicalVolume * profileMonitorPhys = new G4PVPlacement(0, {PMposX,PMposY,PMposZ}, profileMonitorLogicalVolume, "profileMonitorPhys", mother, false, 0);

    //*********************************************************
    // Polyimide window
    //*********************************************************

    G4Box           * PolyimidePM = new G4Box("upPolyimidePM", PMsizeX, PMsizeY, 12.5/2 *um);
    G4LogicalVolume * PolyimidePMLogicalVolume = new G4LogicalVolume(PolyimidePM, Polyimide, "PolyimidePMLog");

    // Upstrem Polyimide window
    G4VPhysicalVolume * upPolyimidePMPhys  = new G4PVPlacement(0, G4ThreeVector(0,0,0.999375 *cm), "upPolyimidePMPhys", PolyimidePMLogicalVolume, profileMonitorPhys, false, 0);
    // Downstrem Polyimide window
    G4VPhysicalVolume * downPolyimidePMPhys  = new G4PVPlacement(0, G4ThreeVector(0,0,-0.999375 *cm), "downPolyimidePMPhys", PolyimidePMLogicalVolume, profileMonitorPhys, false, 0);

    G4VisAttributes* polyimideVisAtt = new G4VisAttributes(G4Colour(0.5,0.5,0.5));
    PolyimidePMLogicalVolume->SetVisAttributes(polyimideVisAtt);

    //*********************************************************
    // Cu coating (Polyimide window)
    //*********************************************************

    G4Box           * CuPM = new G4Box("CuPM", PMsizeX, PMsizeY, 0.2/2 *um);
    G4LogicalVolume * CuPMLogicalVolume = new G4LogicalVolume(CuPM, Copper, "CuPMLog");

    // Upstream Cu coating (Polyimide window)
    G4VPhysicalVolume * upCuPMPhys  = new G4PVPlacement(0, G4ThreeVector(0,0,0.99874 *cm), "upCuPMPhys", CuPMLogicalVolume, profileMonitorPhys, false, 0);
    // Downstream Cu coating (Polyimide window)
    G4VPhysicalVolume * downCuPMPhys = new G4PVPlacement(0, G4ThreeVector(0,0,-0.99874 *cm), "downCuPMPhys", CuPMLogicalVolume, profileMonitorPhys, false, 0);

    G4VisAttributes* CuVisAtt = new G4VisAttributes(G4Colour(0.0,0.5,0.5));
    CuPMLogicalVolume->SetVisAttributes(CuVisAtt);

    //*********************************************************
    // Al coating (Polyimide window)
    //*********************************************************

    G4Box           * AlPM = new G4Box("AlPM", PMsizeX, PMsizeY, 0.1/2 *um);
    G4LogicalVolume * AlPMLogicalVolume = new G4LogicalVolume(AlPM, Aluminum, "AlPMLog");
    // Upstrem Al coating (Polyimide window)
    G4VPhysicalVolume * upAlPMPhys  = new G4PVPlacement(0, G4ThreeVector(0,0,0.998725 *cm), "upAlPMPhys", AlPMLogicalVolume, profileMonitorPhys, false, 0);
    // Downstrem Al coating (Polyimide window)
    G4VPhysicalVolume * downAlPMPhys  = new G4PVPlacement(0, G4ThreeVector(0,0,-0.998725 *cm), "downAlPMPhys", AlPMLogicalVolume, profileMonitorPhys, false, 0);

    G4VisAttributes* AlVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,0.5));
    AlPMLogicalVolume->SetVisAttributes(AlVisAtt);

    //*********************************************************
    // 0.03 mm diam (0.015 mm rad) Tungstem wires
    //*********************************************************

    G4double wireR0   = 0     *mm;
    G4double wireR    = 0.015 *mm;
    G4double wirePhi0 = 0     *deg;
    G4double wirePhi  = 360   *deg;
    G4double wireZ   = 12/2   *cm;

    G4Tubs* wirePM = new G4Tubs("wirePM", wireR0, wireR, wireZ, wirePhi0, wirePhi);
    G4LogicalVolume* wirePMLogicalVolume = new G4LogicalVolume(wirePM, Tungsten, "wirePMLog");

    G4double wirePMpitch = 0.05 *cm;
    G4RotationMatrix* RotationY = new G4RotationMatrix();
    RotationY->rotateY(90*deg);
    G4RotationMatrix* RotationX = new G4RotationMatrix();
    RotationX->rotateX(90*deg);

    G4String name;
    std::stringstream replNum;

    for (int i=0; i<48; i++)
    {
        replNum << i;
        name = "wirePMXPhys_" + replNum.str();
        G4VPhysicalVolume * wirePMXPhys = new G4PVPlacement(RotationY, G4ThreeVector(0,-1.175 *cm + wirePMpitch*i,0.1 *cm), name, wirePMLogicalVolume, profileMonitorPhys, false, 0);
        replNum.str("");
    }

    for (int i=0; i<48; i++)
    {
        replNum << i;
        name = "wirePMYPhys_" + replNum.str();
        G4VPhysicalVolume * wirePMYPhys = new G4PVPlacement(RotationX, G4ThreeVector(-1.175 *cm + wirePMpitch*i, 0, -0.1 *cm), name, wirePMLogicalVolume, profileMonitorPhys, false, 0);
        replNum.str("");
    }

    G4VisAttributes* wireVisAtt = new G4VisAttributes(G4Colour(1.0,0.5,0.5));
    wirePMLogicalVolume->SetVisAttributes(wireVisAtt);

}

//*********************************************************
//*********************************************************
// HELIUM CHAMBER
// (Based on design in MDA-40E-0122, p.54)
//*********************************************************
//*********************************************************

void Nozzle::HeliumChamber(G4LogicalVolume * mother)
{
    // Outer boundary of He-CH (He-CH mother volume)
    G4double HeCHsizeR0   = 0.      *cm;
    G4double HeCHsizeR    = 21.     *cm;
    G4double Phi0         = 0       *deg;
    G4double Phi          = 360     *deg;
    G4double HeCHsizeZ    = 126.8/2 *cm; // half size

    G4double HeCHposX     = 0.    *cm;
    G4double HeCHposY     = 0.    *cm;
    G4double HeCHposZ     = 243.9 *cm; // Position of center of cylinder

    G4Tubs            * heliumChamber = new G4Tubs("HeCH", HeCHsizeR0, HeCHsizeR, HeCHsizeZ, Phi0, Phi);
    G4LogicalVolume   * heliumChamberLogicalVolume = new G4LogicalVolume(heliumChamber, Air, "HeCHLog");
    //G4VPhysicalVolume * heliumChamberPhys = new G4PVPlacement(0, G4ThreeVector(HeCHposX,HeCHposY,HeCHposZ), "heliumChamberPhys", heliumChamberLogicalVolume, mother, false, 0);
    G4VPhysicalVolume * heliumChamberPhys = new G4PVPlacement(0, {HeCHposX,HeCHposY,HeCHposZ}, heliumChamberLogicalVolume, "heliumChamberPhys", mother, false, 0);

    //*********************************************************
    // Stainless steel flange to the upstream He window (G3-HW1)
    //*********************************************************

    // Size of the flange
    G4double upHeWFlangeR0 = 2.2   *cm;
    G4double upHeWFlangeR  = 6.7   *cm;
    G4double upHeWFlangeZ  = 1.1/2 *cm; // half size

    // Position of the flange in respect to its mother volume
    G4double upHeWFlangePosX =  0.   *cm;
    G4double upHeWFlangePosY =  0.   *cm;
    G4double upHeWFlangePosZ = 62.85 *cm;

    G4Tubs            * upHeWFlange = new G4Tubs("upHeWFlange", upHeWFlangeR0, upHeWFlangeR, upHeWFlangeZ, Phi0, Phi);
    G4LogicalVolume   * upHeWFlangeLogicalVolume = new G4LogicalVolume(upHeWFlange, Stainless, "upHeWFlangeLog");
    G4VPhysicalVolume * upHeWFlangePhys = new G4PVPlacement(0,
                              G4ThreeVector(upHeWFlangePosX,
                                            upHeWFlangePosY,
                                            upHeWFlangePosZ),
                              "upHeWFlangePhys",
                              upHeWFlangeLogicalVolume,
                              heliumChamberPhys, false, 0);

    //*********************************************************
    // Upstream polyimide window of upstream He window (G3-HW1)
    //*********************************************************

    // Size of the polyimide window
    G4double upPolyimideWR0 = 0.     *cm;
    G4double upPolyimideWR  = 6.7    *cm;
    G4double upPolyimideWZ  = 12.5/2 *um; // half size

    // Position of the polyimide window in respect to its mother volume
    G4double upPolyimideWPosX =  0.       *cm;
    G4double upPolyimideWPosY =  0.       *cm;
    G4double upPolyimideWPosZ = 62.299375 *cm;

    G4Tubs* upPolyimideW = new G4Tubs("upPolyimideW", upPolyimideWR0, upPolyimideWR, upPolyimideWZ, Phi0, Phi);
    G4LogicalVolume* upPolyimideWLogicalVolume = new G4LogicalVolume(upPolyimideW, Polyimide, "upPolyimideWLog");
    G4VPhysicalVolume * upPolyimideWPhys  = new G4PVPlacement(0,
                              G4ThreeVector(upPolyimideWPosX,
                                            upPolyimideWPosY,
                                            upPolyimideWPosZ),
                              "upPolyimideWPhys",
                              upPolyimideWLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Upstream flange of Stainless Chamber 1 (G3-HC1)
    //*********************************************************

    // Size according to U. Titt MCNPX implementation.
    // According to Hitachi Document No. MDA-40R-0122, R0 = 4.455 cm
    G4double upStainlessCH1FlangeR0 = 4.355  *cm;
    G4double upStainlessCH1FlangeR  = 6.7    *cm;
    G4double upStainlessCH1FlangeZ  = 1.99875/2 *cm; // half size

    // Position in respect to its mother volume
    G4double upStainlessCH1FlangePosX =  0.       *cm;
    G4double upStainlessCH1FlangePosY =  0.       *cm;
    G4double upStainlessCH1FlangePosZ = 61.299375 *cm;

    G4Tubs* upStainlessCH1Flange = new G4Tubs("upStainlessCH1Flange",
                                              upStainlessCH1FlangeR0,
                                              upStainlessCH1FlangeR,
                                              upStainlessCH1FlangeZ,
                                              Phi0,
                                              Phi);

    G4LogicalVolume* upStainlessCH1FlangeLogicalVolume = new G4LogicalVolume(upStainlessCH1Flange, Stainless, "upStainlessCH1FlangeLog");
    G4VPhysicalVolume * upStainlessCH1FlangePhys = new G4PVPlacement(0,
                              G4ThreeVector(upStainlessCH1FlangePosX,
                                            upStainlessCH1FlangePosY,
                                            upStainlessCH1FlangePosZ),
                              "upStainlessCH1FlangePhys",
                              upStainlessCH1FlangeLogicalVolume,
                              heliumChamberPhys, false, 0);

    //*********************************************************
    // Stainless Chamber 1 (G3-HC1)
    //*********************************************************

    // Size according to U. Titt MCNPX implementation.
    // According to Hitachi Document No. MDA-40R-0122, R0 = 4.455 cm
    G4double StainlessCH1R0 = 4.355  *cm;
    G4double StainlessCH1R  = 4.455  *cm;
    G4double StainlessCH1Z  = 7.6/2 *cm; // half size

    // Position in respect to its mother volume
    G4double StainlessCH1PosX =  0.  *cm;
    G4double StainlessCH1PosY =  0.  *cm;
    G4double StainlessCH1PosZ = 56.5 *cm;

    G4Tubs* StainlessCH1 = new G4Tubs("StainlessCH1",
                                      StainlessCH1R0,
                                      StainlessCH1R,
                                      StainlessCH1Z,
                                      Phi0,
                                      Phi);

    G4LogicalVolume* StainlessCH1LogicalVolume = new G4LogicalVolume(StainlessCH1, Stainless, "StainlessCH1Log");

    G4VPhysicalVolume * StainlessCH1Phys = new G4PVPlacement(0,
                              G4ThreeVector(StainlessCH1PosX,
                                            StainlessCH1PosY,
                                            StainlessCH1PosZ),
                              "StainlessCH1Phys",
                              StainlessCH1LogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Downstream flange of Stainless Chamber 1 (G3-HC1)
    //*********************************************************

    // Size according to U. Titt MCNPX implementation.
    // According to Hitachi Document No. MDA-40R-0122, R0 = 4.455 cm
    G4double downStainlessCH1FlangeR0 = 4.355 *cm;
    G4double downStainlessCH1FlangeR  = 8.0   *cm;
    G4double downStainlessCH1FlangeZ  = 1.2/2 *cm; // half size

    // Position in respect to its mother volume
    G4double downStainlessCH1FlangePosX =  0.  *cm;
    G4double downStainlessCH1FlangePosY =  0.  *cm;
    G4double downStainlessCH1FlangePosZ = 52.1 *cm;

    G4Tubs* downStainlessCH1Flange = new G4Tubs("downStainlessCH1Flange", downStainlessCH1FlangeR0, downStainlessCH1FlangeR, downStainlessCH1FlangeZ, Phi0, Phi);
    G4LogicalVolume* downStainlessCH1FlangeLogicalVolume = new G4LogicalVolume(downStainlessCH1Flange, Stainless, "downStainlessCH1FlangeLog");
    G4VPhysicalVolume * downStainlessCH1FlangePhys = new G4PVPlacement(0,
                              G4ThreeVector(downStainlessCH1FlangePosX,
                                            downStainlessCH1FlangePosY,
                                            downStainlessCH1FlangePosZ),
                              "downStainlessCH1FlangePhys",
                              downStainlessCH1FlangeLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Helium gas inside of Stainless Chamber 1 (G3-HC1) and
    // upstream and downstream flanges
    //*********************************************************

    G4double HeStainlessCH1R0 = 0.         *cm;
    G4double HeStainlessCH1R  = 4.355      *cm;
    G4double HeStainlessCH1Z  = 10.79875/2 *cm; // half size

    // Position in respect to its mother volume
    G4double HeStainlessCH1PosX =  0.       *cm;
    G4double HeStainlessCH1PosY =  0.       *cm;
    G4double HeStainlessCH1PosZ = 56.899375 *cm;

    G4Tubs* HeStainlessCH1 = new G4Tubs("HeStainlessCH1",
                                        HeStainlessCH1R0,
                                        HeStainlessCH1R,
                                        HeStainlessCH1Z,
                                        Phi0,
                                        Phi);

    G4LogicalVolume* HeStainlessCH1LogicalVolume = new G4LogicalVolume(HeStainlessCH1, Helium, "HeStainlessCH1Log");
    G4VPhysicalVolume * HeStainlessCH1Phys  = new G4PVPlacement(0,
                              G4ThreeVector(HeStainlessCH1PosX,
                                            HeStainlessCH1PosY,
                                            HeStainlessCH1PosZ),
                              "HeStainlessCH1Phys",
                              HeStainlessCH1LogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Upstream flange of Ceramics Chamber 1 (G3-CHC1)
    //*********************************************************

    G4double upCeramicsCH1FlangeR0 = 0.         *cm;
    G4double upCeramicsCH1FlangeR  = 8.0        *cm;
    G4double upCeramicsCH1FlangeX  = 8./2       *cm;
    G4double upCeramicsCH1FlangeY  = 4.4/2      *cm;
    G4double upCeramicsCH1FlangeZ  = 1.2/2      *cm; // half size

    // Position in respect to its mother volume
    G4double upCeramicsCH1FlangePosX =  0.   *cm;
    G4double upCeramicsCH1FlangePosY =  0.   *cm;
    G4double upCeramicsCH1FlangePosZ = 50.9  *cm;

    G4Tubs* upCeramicsCH1FlangeCyl =
            new G4Tubs("upCeramicsCH1FlangeCyl",
                       upCeramicsCH1FlangeR0,
                       upCeramicsCH1FlangeR,
                       upCeramicsCH1FlangeZ,
                       Phi0,
                       Phi);

    G4Box* upCeramicsCH1FlangeBox =
            new G4Box("upCeramicsCH1FlangeBox",
                      upCeramicsCH1FlangeX,
                      upCeramicsCH1FlangeY,
                      upCeramicsCH1FlangeZ);

    G4SubtractionSolid* upCeramicsCH1Flange =
            new G4SubtractionSolid("upCeramicsCH1Flange",
                                   upCeramicsCH1FlangeCyl,
                                   upCeramicsCH1FlangeBox);

    G4LogicalVolume* upCeramicsCH1FlangeLogicalVolume
            = new G4LogicalVolume(upCeramicsCH1Flange,
                                  Stainless,
                                  "upCeramicsCH1FlangeLog");

    G4VPhysicalVolume * upCeramicsCH1FlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(upCeramicsCH1FlangePosX,
                                            upCeramicsCH1FlangePosY,
                                            upCeramicsCH1FlangePosZ),
                              "upCeramicsCH1FlangePhys",
                              upCeramicsCH1FlangeLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // He inside inner vol. of upstream flange of
    // Ceramics Chamber 1 (G3-CHC1)
    //*********************************************************

    G4LogicalVolume* upHeCeramicsCH1FlangeLogicalVolume
            = new G4LogicalVolume(upCeramicsCH1FlangeBox,
                                  Helium,
                                  "upHeCeramicsCH1FlangeLog");

    // Mother vol. is the flange.
    // Position in respect to mother vol.
    G4VPhysicalVolume * upHeCeramicsCH1FlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0),
                              "upHeCeramicsCH1FlangePhys",
                              upHeCeramicsCH1FlangeLogicalVolume,
                              upCeramicsCH1FlangePhys,
                              false,
                              0);

    //*********************************************************
    // Ceramics Chamber 1 filled with helium gas (G3-CHC1).
    // Size according to U. Titt MCNPX implementation.
    //*********************************************************

    G4double CeramicsCH1XIn  =  8.0/2    *cm; // half size
    G4double CeramicsCH1YIn  =  4.4/2    *cm; // half size
    G4double CeramicsCH1XOut = 10.0/2    *cm; // half size
    G4double CeramicsCH1YOut =  6.4/2    *cm; // half size
    G4double CeramicsCH1Z    = 41.2/2    *cm; // half size

    // Position in respect to its mother volume
    G4double CeramicsCH1PosX =  0.   *cm;
    G4double CeramicsCH1PosY =  0.   *cm;
    G4double CeramicsCH1PosZ = 29.7  *cm;

    G4Box* CeramicsCH1BoxIn =
            new G4Box("CeramicsCH1BoxIn",
                      CeramicsCH1XIn,
                      CeramicsCH1YIn,
                      CeramicsCH1Z);

    G4Box* CeramicsCH1BoxOut =
            new G4Box("CeramicsCH1BoxOut",
                      CeramicsCH1XOut,
                      CeramicsCH1YOut,
                      CeramicsCH1Z);

    G4SubtractionSolid* CeramicsCH1 =
            new G4SubtractionSolid("CeramicsCH1",
                                   CeramicsCH1BoxOut,
                                   CeramicsCH1BoxIn);

    G4LogicalVolume* CeramicsCH1LogicalVolume
            = new G4LogicalVolume(CeramicsCH1,
                                  Ceramics,
                                  "CeramicsCH1Log");

    G4VPhysicalVolume * CeramicsCH1Phys  =
            new G4PVPlacement(0,
                              G4ThreeVector(CeramicsCH1PosX,
                                            CeramicsCH1PosY,
                                            CeramicsCH1PosZ),
                              "CeramicsCH1Phys",
                              CeramicsCH1LogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Helium gas inside Ceramics Chamber 1 (G3-CHC1).
    //*********************************************************

    G4LogicalVolume* upHeCeramicsCH1LogicalVolume
            = new G4LogicalVolume(CeramicsCH1BoxIn,
                                  Helium,
                                  "upHeCeramicsCH1Log");

    // Mother vol. is the Ceramics CH1.
    // Position in respect to mother vol.
    G4VPhysicalVolume * upHeCeramicsCH1Phys  =
            new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0),
                              "upHeCeramicsCH1Phys",
                              upHeCeramicsCH1LogicalVolume,
                              CeramicsCH1Phys,
                              false,
                              0);

    //*********************************************************
    // Downstream flange of Ceramics Chamber 1 (G3-CHC1).
    // Size according to U. Titt MCNPX implementation.
    //*********************************************************

    G4double downCeramicsCH1FlangeXIn  =  8.0/2    *cm; // half size
    G4double downCeramicsCH1FlangeYIn  =  4.4/2    *cm; // half size
    G4double downCeramicsCH1FlangeXOut = 16.4/2    *cm; // half size
    G4double downCeramicsCH1FlangeYOut = 12.6/2    *cm; // half size
    G4double downCeramicsCH1FlangeZ    =  1.5/2    *cm; // half size

    // Position in respect to its mother volume
    G4double downCeramicsCH1FlangePosX =  0.   *cm;
    G4double downCeramicsCH1FlangePosY =  0.   *cm;
    G4double downCeramicsCH1FlangePosZ =  8.35 *cm;

    G4Box* downCeramicsCH1FlangeBoxIn =
            new G4Box("downCeramicsCH1FlangeBoxIn",
                      downCeramicsCH1FlangeXIn,
                      downCeramicsCH1FlangeYIn,
                      downCeramicsCH1FlangeZ);

    G4Box* downCeramicsCH1FlangeBoxOut =
            new G4Box("downCeramicsCH1FlangeBoxOut",
                      downCeramicsCH1FlangeXOut,
                      downCeramicsCH1FlangeYOut,
                      downCeramicsCH1FlangeZ);

    G4SubtractionSolid* downCeramicsCH1Flange =
            new G4SubtractionSolid("downCeramicsCH1Flange",
                                   downCeramicsCH1FlangeBoxOut,
                                   downCeramicsCH1FlangeBoxIn);

    G4LogicalVolume* downCeramicsCH1FlangeLogicalVolume
            = new G4LogicalVolume(downCeramicsCH1Flange,
                                  Stainless,
                                  "downCeramicsCH1FlangeLog");

    G4VPhysicalVolume * downCeramicsCH1FlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(downCeramicsCH1FlangePosX,
                                            downCeramicsCH1FlangePosY,
                                            downCeramicsCH1FlangePosZ),
                              "downCeramicsCH1FlangePhys",
                              downCeramicsCH1FlangeLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Helium gas inside Ceramics Chamber 1 (G3-CHC1).
    //*********************************************************

    G4LogicalVolume* downHeCeramicsCH1FlangeLogicalVolume
            = new G4LogicalVolume(downCeramicsCH1FlangeBoxIn,
                                  Helium,
                                  "downHeCeramicsCH1FlangeLog");

    G4VPhysicalVolume * downHeCeramicsCH1FlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0),
                              "downHeCeramicsCH1FlangePhys",
                              downHeCeramicsCH1FlangeLogicalVolume,
                              downCeramicsCH1FlangePhys,
                              false,
                              0);

    //*********************************************************
    // Upstream flange of Ceramics Chamber 2 (G3-CHC2).
    // Size according to U. Titt MCNPX implementation.
    //*********************************************************

    G4double upCeramicsCH2FlangeXIn  = 14.8/2    *cm; // half size
    G4double upCeramicsCH2FlangeYIn  = 11.0/2    *cm; // half size
    G4double upCeramicsCH2FlangeXOut = 16.8/2    *cm; // half size
    G4double upCeramicsCH2FlangeYOut = 13.0/2    *cm; // half size
    G4double upCeramicsCH2FlangeZ    =  2.4/2    *cm; // half size

    // Position in respect to its mother volume
    G4double upCeramicsCH2FlangePosX =  0.   *cm;
    G4double upCeramicsCH2FlangePosY =  0.   *cm;
    G4double upCeramicsCH2FlangePosZ =  6.4  *cm;

    G4Box* upCeramicsCH2FlangeBoxIn =
            new G4Box("upCeramicsCH2FlangeBoxIn",
                      upCeramicsCH2FlangeXIn,
                      upCeramicsCH2FlangeYIn,
                      upCeramicsCH2FlangeZ);

    G4Box* upCeramicsCH2FlangeBoxOut =
            new G4Box("upCeramicsCH2FlangeBoxOut",
                      upCeramicsCH2FlangeXOut,
                      upCeramicsCH2FlangeYOut,
                      upCeramicsCH2FlangeZ);

    G4SubtractionSolid* upCeramicsCH2Flange =
            new G4SubtractionSolid("upCeramicsCH2Flange",
                                   upCeramicsCH2FlangeBoxOut,
                                   upCeramicsCH2FlangeBoxIn);

    G4LogicalVolume* upCeramicsCH2FlangeLogicalVolume
            = new G4LogicalVolume(upCeramicsCH2Flange,
                                  Stainless,
                                  "upCeramicsCH2FlangeLog");

    G4VPhysicalVolume * upCeramicsCH2FlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(upCeramicsCH2FlangePosX,
                                            upCeramicsCH2FlangePosY,
                                            upCeramicsCH2FlangePosZ),
                              "upCeramicsCH2FlangePhys",
                              upCeramicsCH2FlangeLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Helium gas inside flange of Ceramics Chamber 2 (G3-CHC2).
    //*********************************************************

    G4LogicalVolume* upHeCeramicsCH2FlangeLogicalVolume
            = new G4LogicalVolume(upCeramicsCH2FlangeBoxIn,
                                  Helium,
                                  "upHeCeramicsCH2FlangeLog");

    G4VPhysicalVolume * upHeCeramicsCH2FlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0),
                              "upHeCeramicsCH2FlangePhys",
                              upHeCeramicsCH2FlangeLogicalVolume,
                              upCeramicsCH2FlangePhys,
                              false,
                              0);

    //*********************************************************
    // Ceramics Chamber 2 filled with helium gas (G3-CHC2).
    // Size according to U. Titt MCNPX implementation.
    //*********************************************************

    G4double CeramicsCH2XIn  = 14.8/2    *cm; // half size
    G4double CeramicsCH2YIn  = 11.0/2    *cm; // half size
    G4double CeramicsCH2XOut = 16.8/2    *cm; // half size
    G4double CeramicsCH2YOut = 13.0/2    *cm; // half size
    G4double CeramicsCH2Z    = 44.7/2    *cm; // half size

    // Position in respect to its mother volume
    G4double CeramicsCH2PosX =  0.   *cm;
    G4double CeramicsCH2PosY =  0.   *cm;
    G4double CeramicsCH2PosZ = -17.15 *cm;

    G4Box* CeramicsCH2BoxIn =
            new G4Box("CeramicsCH2BoxIn",
                      CeramicsCH2XIn,
                      CeramicsCH2YIn,
                      CeramicsCH2Z);

    G4Box* CeramicsCH2BoxOut =
            new G4Box("CeramicsCH2BoxOut",
                      CeramicsCH2XOut,
                      CeramicsCH2YOut,
                      CeramicsCH2Z);

    G4SubtractionSolid* CeramicsCH2 =
            new G4SubtractionSolid("CeramicsCH2",
                                   CeramicsCH2BoxOut,
                                   CeramicsCH2BoxIn);

    G4LogicalVolume* CeramicsCH2LogicalVolume
            = new G4LogicalVolume(CeramicsCH2,
                                  Ceramics,
                                  "CeramicsCH2Log");

    G4VPhysicalVolume * CeramicsCH2Phys  =
            new G4PVPlacement(0,
                              G4ThreeVector(CeramicsCH2PosX,
                                            CeramicsCH2PosY,
                                            CeramicsCH2PosZ),
                              "CeramicsCH2Phys",
                              CeramicsCH2LogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Helium gas inside Ceramics Chamber 2 (G3-CHC2).
    //*********************************************************

    G4LogicalVolume* upHeCeramicsCH2LogicalVolume
            = new G4LogicalVolume(CeramicsCH2BoxIn,
                                  Helium,
                                  "upHeCeramicsCH2Log");

    // Mother vol. is the Ceramics CH2.
    // Position in respect to mother vol.
    G4VPhysicalVolume * upHeCeramicsCH2Phys  =
            new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0),
                              "upHeCeramicsCH2Phys",
                              upHeCeramicsCH2LogicalVolume,
                              CeramicsCH2Phys,
                              false,
                              0);

    //*********************************************************
    // Downstream flange of Ceramics Chamber 2 (G3-CHC2).
    // Size according to U. Titt MCNPX implementation.
    //*********************************************************

    G4double downCeramicsCH2FlangeXIn  = 14.8/2    *cm; // half size
    G4double downCeramicsCH2FlangeYIn  = 11.0/2    *cm; // half size
    G4double downCeramicsCH2FlangeR0   =  0.0/2    *cm;
    G4double downCeramicsCH2FlangeR    = 20.0/2    *cm;
    G4double downCeramicsCH2FlangeZ    =  2.0/2    *cm; // half size

    // Position in respect to its mother volume
    G4double downCeramicsCH2FlangePosX =   0.   *cm;
    G4double downCeramicsCH2FlangePosY =   0.   *cm;
    G4double downCeramicsCH2FlangePosZ = -40.5  *cm;

    G4Box* downCeramicsCH2FlangeBoxIn =
            new G4Box("downCeramicsCH2FlangeBoxIn",
                      downCeramicsCH2FlangeXIn,
                      downCeramicsCH2FlangeYIn,
                      downCeramicsCH2FlangeZ);

    G4Tubs* downCeramicsCH2FlangeTubsOut =
            new G4Tubs("downCeramicsCH2FlangeTubsOut",
                       downCeramicsCH2FlangeR0,
                       downCeramicsCH2FlangeR,
                       downCeramicsCH2FlangeZ,
                       Phi0,
                       Phi);

    G4SubtractionSolid* downCeramicsCH2Flange =
            new G4SubtractionSolid("downCeramicsCH2Flange",
                                   downCeramicsCH2FlangeTubsOut,
                                   downCeramicsCH2FlangeBoxIn);

    G4LogicalVolume* downCeramicsCH2FlangeLogicalVolume
            = new G4LogicalVolume(downCeramicsCH2Flange,
                                  Stainless,
                                  "downCeramicsCH2FlangeLog");

    G4VPhysicalVolume * downCeramicsCH2FlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(downCeramicsCH2FlangePosX,
                                            downCeramicsCH2FlangePosY,
                                            downCeramicsCH2FlangePosZ),
                              "downCeramicsCH2FlangePhys",
                              downCeramicsCH2FlangeLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Helium gas inside flange of Ceramics Chamber 2 (G3-CHC2).
    //*********************************************************

    G4LogicalVolume* downHeCeramicsCH2FlangeLogicalVolume
            = new G4LogicalVolume(downCeramicsCH2FlangeBoxIn,
                                  Helium,
                                  "downHeCeramicsCH2FlangeLog");

    G4VPhysicalVolume * downHeCeramicsCH2FlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0),
                              "downHeCeramicsCH2FlangePhys",
                              downHeCeramicsCH2FlangeLogicalVolume,
                              downCeramicsCH2FlangePhys,
                              false,
                              0);

    //*********************************************************
    // Upstream flange of Stainless Chamber 2 (G3-HC2).
    // Size according to U. Titt MCNPX implementation.
    //*********************************************************

    G4double upStainlessCH2FlangeXIn  = 14.8/2    *cm; // half size
    G4double upStainlessCH2FlangeYIn  = 11.0/2    *cm; // half size
    G4double upStainlessCH2FlangeR0   =  0.0/2    *cm;
    G4double upStainlessCH2FlangeR    = 20.0/2    *cm;
    G4double upStainlessCH2FlangeZ    =  1.6/2    *cm; // half size

    // Position in respect to its mother volume
    G4double upStainlessCH2FlangePosX =   0.   *cm;
    G4double upStainlessCH2FlangePosY =   0.   *cm;
    G4double upStainlessCH2FlangePosZ = -42.3  *cm;

    G4Box* upStainlessCH2FlangeBoxIn =
            new G4Box("upStainlessCH2FlangeBoxIn",
                      upStainlessCH2FlangeXIn,
                      upStainlessCH2FlangeYIn,
                      upStainlessCH2FlangeZ);

    G4Tubs* upStainlessCH2FlangeTubsOut =
            new G4Tubs("upStainlessCH2FlangeTubsOut",
                       upStainlessCH2FlangeR0,
                       upStainlessCH2FlangeR,
                       upStainlessCH2FlangeZ,
                       Phi0,
                       Phi);

    G4SubtractionSolid* upStainlessCH2Flange =
            new G4SubtractionSolid("upStainlessCH2Flange",
                                   upStainlessCH2FlangeTubsOut,
                                   upStainlessCH2FlangeBoxIn);

    G4LogicalVolume* upStainlessCH2FlangeLogicalVolume
            = new G4LogicalVolume(upStainlessCH2Flange,
                                  Stainless,
                                  "upStainlessCH2FlangeLog");

    G4VPhysicalVolume * upStainlessCH2FlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(upStainlessCH2FlangePosX,
                                            upStainlessCH2FlangePosY,
                                            upStainlessCH2FlangePosZ),
                              "upStainlessCH2FlangePhys",
                              upStainlessCH2FlangeLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Helium gas inside flange of Ceramics Chamber 2 (G3-CHC2).
    //*********************************************************

    G4LogicalVolume* upHeStainlessCH2FlangeLogicalVolume = new G4LogicalVolume(upStainlessCH2FlangeBoxIn, Helium, "upHeStainlessCH2FlangeLog");
    G4VPhysicalVolume * upHeStainlessCH2FlangePhys = new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0),
                              "upHeStainlessCH2FlangePhys",
                              upHeStainlessCH2FlangeLogicalVolume,
                              upStainlessCH2FlangePhys,
                              false,
                              0);

    //*********************************************************
    // Stainless Chamber 2 filled with helium gas (G3-HC2).
    // Size according to U. Titt MCNPX implementation.
    //*********************************************************

    G4double StainlessCH2XIn  = 23.8/2    *cm; // half size
    G4double StainlessCH2YIn  = 16.8/2    *cm; // half size
    G4double StainlessCH2XOut = 24.2/2    *cm; // half size
    G4double StainlessCH2YOut = 17.2/2    *cm; // half size
    G4double StainlessCH2Z    = 17.1/2    *cm; // half size

    // Position in respect to its mother volume
    G4double StainlessCH2PosX =  0.   *cm;
    G4double StainlessCH2PosY =  0.   *cm;
    G4double StainlessCH2PosZ = -51.65 *cm;

    G4Box* StainlessCH2BoxIn =
            new G4Box("StainlessCH2BoxIn",
                      StainlessCH2XIn,
                      StainlessCH2YIn,
                      StainlessCH2Z);

    G4Box* StainlessCH2BoxOut =
            new G4Box("StainlessCH2BoxOut",
                      StainlessCH2XOut,
                      StainlessCH2YOut,
                      StainlessCH2Z);

    G4SubtractionSolid* StainlessCH2 =
            new G4SubtractionSolid("StainlessCH2",
                                   StainlessCH2BoxOut,
                                   StainlessCH2BoxIn);

    G4LogicalVolume* StainlessCH2LogicalVolume
            = new G4LogicalVolume(StainlessCH2,
                                  Stainless,
                                  "StainlessCH2Log");

    G4VPhysicalVolume * StainlessCH2Phys  =
            new G4PVPlacement(0,
                              G4ThreeVector(StainlessCH2PosX,
                                            StainlessCH2PosY,
                                            StainlessCH2PosZ),
                              "StainlessCH2Phys",
                              StainlessCH2LogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Helium gas inside Stainless Chamber 2 (G3-HC2).
    //*********************************************************

    G4LogicalVolume* upHeStainlessCH2LogicalVolume = new G4LogicalVolume(StainlessCH2BoxIn, Helium, "upHeStainlessCH2Log");

    // Mother vol. is the Stainless CH2.
    // Position in respect to mother vol.
    G4VPhysicalVolume * upHeStainlessCH2Phys  =
            new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0),
                              "upHeStainlessCH2Phys",
                              upHeStainlessCH2LogicalVolume,
                              StainlessCH2Phys,
                              false,
                              0);

    //*********************************************************
    // Downstream flange of Stainless Chamber 2 (G3-HC2).
    // Size according to U. Titt MCNPX implementation.
    //*********************************************************

    G4double downStainlessCH2FlangeXIn  = 23.0/2    *cm; // half size
    G4double downStainlessCH2FlangeYIn  = 16.0/2    *cm; // half size
    G4double downStainlessCH2FlangeR0   =  0.0/2    *cm;
    G4double downStainlessCH2FlangeR    = 20.0/2    *cm;
    G4double downStainlessCH2FlangeZ    =  1.6/2    *cm; // half size

    // Position in respect to its mother volume
    G4double downStainlessCH2FlangePosX =   0.   *cm;
    G4double downStainlessCH2FlangePosY =   0.   *cm;
    G4double downStainlessCH2FlangePosZ = -61.0  *cm;

    G4Box* downStainlessCH2FlangeBoxIn =
            new G4Box("downStainlessCH2FlangeBoxIn",
                      downStainlessCH2FlangeXIn,
                      downStainlessCH2FlangeYIn,
                      downStainlessCH2FlangeZ);

    G4Tubs* downStainlessCH2FlangeTubsOut =
            new G4Tubs("downStainlessCH2FlangeTubsOut",
                       downStainlessCH2FlangeR0,
                       downStainlessCH2FlangeR,
                       downStainlessCH2FlangeZ,
                       Phi0,
                       Phi);

    G4SubtractionSolid* downStainlessCH2Flange = new G4SubtractionSolid("downStainlessCH2Flange", downStainlessCH2FlangeTubsOut, downStainlessCH2FlangeBoxIn);
    G4LogicalVolume* downStainlessCH2FlangeLogicalVolume = new G4LogicalVolume(downStainlessCH2Flange, Stainless, "downStainlessCH2FlangeLog");
    G4VPhysicalVolume * downStainlessCH2FlangePhys = new G4PVPlacement(0,
                              G4ThreeVector(downStainlessCH2FlangePosX,
                                            downStainlessCH2FlangePosY,
                                            downStainlessCH2FlangePosZ),
                              "downStainlessCH2FlangePhys",
                              downStainlessCH2FlangeLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Helium gas inside flange of Ceramics Chamber 2 (G3-CHC2).
    //*********************************************************

    G4LogicalVolume* downHeStainlessCH2FlangeLogicalVolume = new G4LogicalVolume(downStainlessCH2FlangeBoxIn, Helium, "downHeStainlessCH2FlangeLog");
    G4VPhysicalVolume * downHeStainlessCH2FlangePhys = new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0),
                              "downHeStainlessCH2FlangePhys",
                              downHeStainlessCH2FlangeLogicalVolume,
                              downStainlessCH2FlangePhys,
                              false,
                              0);

    //*********************************************************
    // Downstream polyimide window of He window (G3-HW2)
    //*********************************************************

    // Size of the polyimide window
    G4double downPolyimideWR0 = 0.     *cm;
    G4double downPolyimideWR  = 20.0    *cm;
    G4double downPolyimideWZ  = 12.5/2 *um; // half size

    // Position of the polyimide window in respect to its mother volume
    G4double downPolyimideWPosX =   0.       *cm;
    G4double downPolyimideWPosY =   0.       *cm;
    G4double downPolyimideWPosZ = -61.800625 *cm;

    G4Tubs* downPolyimideW = new G4Tubs("downPolyimideW",
                                        downPolyimideWR0,
                                        downPolyimideWR,
                                        downPolyimideWZ,
                                        Phi0,
                                        Phi);

    G4LogicalVolume* downPolyimideWLogicalVolume
            = new G4LogicalVolume(downPolyimideW,
                                  Polyimide,
                                  "downPolyimideWLog");

    G4VPhysicalVolume * downPolyimideWPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(downPolyimideWPosX,
                                            downPolyimideWPosY,
                                            downPolyimideWPosZ),
                              "downPolyimideWPhys",
                              downPolyimideWLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);

    //*********************************************************
    // Stainless steel flange to the downstream He window (G3-HW2)
    //*********************************************************

    // Size of the flange
    G4double downHeWFlangeR0 = 0         *cm;
    G4double downHeWFlangeR  = 20.0      *cm;
    G4double downHeWFlangeX  = 23.0      *cm;
    G4double downHeWFlangeY  = 16.0      *cm;
    G4double downHeWFlangeZ  = 1.59875/2 *cm; // half size

    // Position of the flange in respect to its mother volume
    G4double downHeWFlangePosX =   0.       *cm;
    G4double downHeWFlangePosY =   0.        *cm;
    G4double downHeWFlangePosZ = -62.600625 *cm;


    G4Box* downHeWFlangeBoxIn =
            new G4Box("downHeWFlangeBoxIn",
                      downHeWFlangeX,
                      downHeWFlangeY,
                      downHeWFlangeZ);

    G4Tubs* downHeWFlangeTubsOut =
            new G4Tubs("downHeWFlangeTubsOut",
                       downHeWFlangeR0,
                       downHeWFlangeR,
                       downHeWFlangeZ,
                       Phi0,
                       Phi);

    G4SubtractionSolid* downHeWFlange =
            new G4SubtractionSolid("downHeWFlange",
                                   downHeWFlangeTubsOut,
                                   downHeWFlangeBoxIn);

    G4LogicalVolume* downHeWFlangeLogicalVolume
            = new G4LogicalVolume(downHeWFlange,
                                  Stainless,
                                  "downHeWFlangeLog");

    G4VPhysicalVolume * downHeWFlangePhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(downHeWFlangePosX,
                                            downHeWFlangePosY,
                                            downHeWFlangePosZ),
                              "downHeWFlangePhys",
                              downHeWFlangeLogicalVolume,
                              heliumChamberPhys,
                              false,
                              0);
}

//*********************************************************
//*********************************************************
// DOSE MONITOR #1 (Main dose monitor) - parallel plate
// ion chamber (dry air, 1 atm) with 3 electrodes
// (Based on design in MDA-40E-0122, p. 61)
//*********************************************************
//*********************************************************

void Nozzle::MainDoseMonitor(G4LogicalVolume * mother)
{
    // Outer boundary of MDM
    G4double MDMsizeX = 47./2      *cm;
    G4double MDMsizeY = 38./2      *cm;
    G4double MDMsizeZ =  1.0104/2  *cm;

    G4double MDMposX     = 0.       *cm;
    G4double MDMposY     = 0.       *cm;
    G4double MDMposZ     = 107.2948 *cm;

    G4Box* mainDoseMonitor = new G4Box("mainDoseMonitor", MDMsizeX, MDMsizeY, MDMsizeZ);
    G4LogicalVolume* mainDoseMonitorLogicalVolume = new G4LogicalVolume(mainDoseMonitor, Air, "mainDoseMonitorLog");
    //G4VPhysicalVolume * mainDoseMonitorPhys  = new G4PVPlacement(0, G4ThreeVector(MDMposX,MDMposY,MDMposZ), "mainDoseMonitorPhys", mainDoseMonitorLogicalVolume, mother, false, 0);
    G4VPhysicalVolume * mainDoseMonitorPhys  = new G4PVPlacement(0, {MDMposX,MDMposY,MDMposZ}, mainDoseMonitorLogicalVolume, "mainDoseMonitorPhys", mother, false, 0);

    //*********************************************************
    // Polyimide window and polyimide electrodes
    // Window and electrodes are the same size.
    // Defining solid and logic volume for both.
    //*********************************************************

    G4double polyimideSizeX = 47./2  *cm;
    G4double polyimideSizeY = 38./2  *cm;
    G4double polyimideSizeZ = 50./2  *um;

    G4Box*  polyimideMDM = new G4Box("polyimideMDM", polyimideSizeX, polyimideSizeY, polyimideSizeZ);
    G4LogicalVolume*  polyimideMDMLogicalVolume = new G4LogicalVolume(polyimideMDM, Polyimide, "polyimideMDMLog");

    //*********************************************************
    // Copper coating
    // Defining solid and logic vol. for all coatings
    //*********************************************************

    G4double CuSizeX = 47./2  *cm;
    G4double CuSizeY = 38./2  *cm;
    G4double CuSizeZ =  2./2  *um;

    G4Box*  CuMDM = new G4Box("CuMDM", CuSizeX, CuSizeY, CuSizeZ);
    G4LogicalVolume*  CuMDMLogicalVolume = new G4LogicalVolume(CuMDM, Copper, "CuMDMLog");

    //*********************************************************
    // Placing solids
    //*********************************************************

    G4double MDMPosX = 0. *cm;
    G4double MDMPosY = 0. *cm;

    //***** Upstream polymide window
    G4double upWpolyimideMDMPosZ = 0.5027 *cm;
    G4VPhysicalVolume * upWpolyimideMDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            upWpolyimideMDMPosZ),
                              "upWpolyimideMDMPhys",
                              polyimideMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    // Electrodes are polyimide coated with Cu

    //******* First electrode
    // Upstream Cu coating - electrode 1
    G4double elec1UpCuMDMPosZ = 0.5001 *cm;
    G4VPhysicalVolume * elec1UpCuMDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            elec1UpCuMDMPosZ),
                              "elec1UpCuMDMPhys",
                              CuMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    // Polyimide electrode 1
    G4double elec1MDMPosZ = 0.4975 *cm;
    G4VPhysicalVolume * elec1MDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            elec1MDMPosZ),
                              "elec1MDMPhys",
                              polyimideMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    // Downstream Cu coating - electrode 1
    G4double elec1DownCuMDMPosZ = 0.4949 *cm;
    G4VPhysicalVolume * elec1DownCuMDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            elec1DownCuMDMPosZ),
                              "elec1DownCuMDMPhys",
                              CuMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    //******* Second electrode (gap of 0.4946 between elec 1 and elec 2)

    // Upstream Cu coating - electrode 2
    G4double elec2UpCuMDMPosZ = 0.0001 *cm;
    G4VPhysicalVolume * elec2UpCuMDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            elec2UpCuMDMPosZ),
                              "elec2UpCuMDMPhys",
                              CuMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    // Polyimide electrode 2
    G4double elec2MDMPosZ = -0.0025 *cm;
    G4VPhysicalVolume * elec2MDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            elec2MDMPosZ),
                              "elec2MDMPhys",
                              polyimideMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    // Downstream Cu coating - electrode 2
    G4double elec2DownCuMDMPosZ = -0.0051 *cm;
    G4VPhysicalVolume * elec2DownCuMDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            elec2DownCuMDMPosZ),
                              "elec2DownCuMDMPhys",
                              CuMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    //******* Second electrode (gap of 0.4896 between elec 2 and elec 3)

    // Upstream Cu coating - electrode 3

    G4double elec3UpCuMDMPosZ = -0.4949 *cm;
    G4VPhysicalVolume * elec3UpCuMDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            elec3UpCuMDMPosZ),
                              "elec3UpCuMDMPhys",
                              CuMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    // Polyimide electrode 3
    G4double elec3MDMPosZ = -0.4975 *cm;
    G4VPhysicalVolume * elec3MDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            elec3MDMPosZ),
                              "elec3MDMPhys",
                              polyimideMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    // Downstream Cu coating - elctrode 3
    G4double elec3DownCuMDMPosZ = -0.5001 *cm;
    G4VPhysicalVolume * elec3DownCuMDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            elec3DownCuMDMPosZ),
                              "elec3DownCuMDMPhys",
                              CuMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);

    //***** Downstream polymide window

    G4double downWpolyimideMDMPosZ = -0.5027 *cm;
    G4VPhysicalVolume * downWpolyimideMDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(MDMPosX, MDMPosY,
                                            downWpolyimideMDMPosZ),
                              "downWpolyimideMDMPhys",
                              polyimideMDMLogicalVolume,
                              mainDoseMonitorPhys,
                              false,
                              0);
}

//*********************************************************
//*********************************************************
// DOSE MONITOR #2 (Sub dose monitor) - parallel plate
// ion chamber (dry air, 1 atm) with 3 electrodes
// (Based on design in MDA-40E-0122, p. 61)
//*********************************************************
//*********************************************************

void Nozzle::SubDoseMonitor(G4LogicalVolume * mother)
{
    // Outer boundary of SDM
    G4double SDMsizeX = 47./2      *cm;
    G4double SDMsizeY = 38./2      *cm;
    G4double SDMsizeZ =  1.0104/2  *cm;

    G4double SDMposX     = 0.       *cm;
    G4double SDMposY     = 0.       *cm;
    G4double SDMposZ     = 112.2948 *cm;

    G4Box* subDoseMonitor = new G4Box("subDoseMonitor", SDMsizeX, SDMsizeY, SDMsizeZ);
    G4LogicalVolume* subDoseMonitorLogicalVolume = new G4LogicalVolume(subDoseMonitor, Air, "subDoseMonitorLog");
    //G4VPhysicalVolume * subDoseMonitorPhys = new G4PVPlacement(0, G4ThreeVector(SDMposX,SDMposY,SDMposZ), "subDoseMonitorPhys", subDoseMonitorLogicalVolume, mother, false, 0);
    G4VPhysicalVolume * subDoseMonitorPhys = new G4PVPlacement(0, G4ThreeVector(SDMposX,SDMposY,SDMposZ), subDoseMonitorLogicalVolume, "subDoseMonitorPhys", mother, false, 0);

    //*********************************************************
    // Polyimide window and polyimide electrodes
    // Window and electrodes are the same size.
    // Defining solid and logic volume for both.
    //*********************************************************

    G4double polyimideSizeX = 47./2  *cm;
    G4double polyimideSizeY = 38./2  *cm;
    G4double polyimideSizeZ = 50./2  *um;

    G4Box*  polyimideSDM = new G4Box("polyimideSDM", polyimideSizeX, polyimideSizeY, polyimideSizeZ);
    G4LogicalVolume*  polyimideSDMLogicalVolume = new G4LogicalVolume(polyimideSDM, Polyimide, "polyimideSDMLog");

    //*********************************************************
    // Copper coating
    // Defining solid and logic vol. for all coatings
    //*********************************************************

    G4double CuSizeX = 47./2  *cm;
    G4double CuSizeY = 38./2  *cm;
    G4double CuSizeZ =  2./2  *um;

    G4Box*  CuSDM = new G4Box("CuSDM", CuSizeX, CuSizeY, CuSizeZ);
    G4LogicalVolume*  CuSDMLogicalVolume = new G4LogicalVolume(CuSDM, Copper, "CuSDMLog");

    //*********************************************************
    // Placing solids
    //*********************************************************

    G4double SDMPosX = 0. *cm;
    G4double SDMPosY = 0. *cm;

    //***** Upstream polymide window
    G4double upWpolyimideSDMPosZ = 0.5027 *cm;
    G4VPhysicalVolume * upWpolyimideSDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            upWpolyimideSDMPosZ),
                              "upWpolyimideSDMPhys",
                              polyimideSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    // Electrodes are polyimide coated with Cu
    //******* First electrode
    // Upstream Cu coating - electrode 1
    G4double elec1UpCuSDMPosZ = 0.5001 *cm;
    G4VPhysicalVolume * elec1UpCuSDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            elec1UpCuSDMPosZ),
                              "elec1UpCuSDMPhys",
                              CuSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    // Polyimide electrode 1
    G4double elec1SDMPosZ = 0.4975 *cm;
    G4VPhysicalVolume * elec1SDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            elec1SDMPosZ),
                              "elec1SDMPhys",
                              polyimideSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    // Downstream Cu coating - electrode 1
    G4double elec1DownCuSDMPosZ = 0.4949 *cm;
    G4VPhysicalVolume * elec1DownCuSDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            elec1DownCuSDMPosZ),
                              "elec1DownCuSDMPhys",
                              CuSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    //******* Second electrode (gap of 0.4946 between elec 1 and elec 2)
    // Upstream Cu coating - electrode 2
    G4double elec2UpCuSDMPosZ = 0.0001 *cm;
    G4VPhysicalVolume * elec2UpCuSDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            elec2UpCuSDMPosZ),
                              "elec2UpCuSDMPhys",
                              CuSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    // Polyimide electrode 2
    G4double elec2SDMPosZ = -0.0025 *cm;
    G4VPhysicalVolume * elec2SDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            elec2SDMPosZ),
                              "elec2SDMPhys",
                              polyimideSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    // Downstream Cu coating - electrode 2
    G4double elec2DownCuSDMPosZ = -0.0051 *cm;
    G4VPhysicalVolume * elec2DownCuSDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            elec2DownCuSDMPosZ),
                              "elec2DownCuSDMPhys",
                              CuSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    //******* Second electrode (gap of 0.4896 between elec 2 and elec 3)
    // Upstream Cu coating - electrode 3
    G4double elec3UpCuSDMPosZ = -0.4949 *cm;
    G4VPhysicalVolume * elec3UpCuSDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            elec3UpCuSDMPosZ),
                              "elec3UpCuSDMPhys",
                              CuSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    // Polyimide electrode 3
    G4double elec3SDMPosZ = -0.4975 *cm;
    G4VPhysicalVolume * elec3SDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            elec3SDMPosZ),
                              "elec3SDMPhys",
                              polyimideSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    // Downstream Cu coating - elctrode 3
    G4double elec3DownCuSDMPosZ = -0.5001 *cm;
    G4VPhysicalVolume * elec3DownCuSDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            elec3DownCuSDMPosZ),
                              "elec3DownCuSDMPhys",
                              CuSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);

    //***** Downstream polymide window
    G4double downWpolyimideSDMPosZ = -0.5027 *cm;
    G4VPhysicalVolume * downWpolyimideSDMPhys  =
            new G4PVPlacement(0,
                              G4ThreeVector(SDMPosX, SDMPosY,
                                            downWpolyimideSDMPosZ),
                              "downWpolyimideSDMPhys",
                              polyimideSDMLogicalVolume,
                              subDoseMonitorPhys,
                              false,
                              0);
}

//*********************************************************
//*********************************************************
// SPOT POSITION MONITOR (Multi wire ionization chamber)
// (Based on design in MDA-40E-0122, p. 59)
//*********************************************************
//*********************************************************

void Nozzle::SpotPositionMonitor(G4LogicalVolume * mother)
{
    // Outer boundary of spot positon monitor
    G4double SPMsizeX = 45./2       *cm;
    G4double SPMsizeY = 35./2       *cm;
    G4double SPMsizeZ =  2.00128/2  *cm;

    G4double SPMposX     = 0.    *cm;
    G4double SPMposY     = 0.    *cm;
    G4double SPMposZ     = 91.39936 *cm;

    G4Box* spotPositionMonitor = new G4Box("spotPositionMonitor", SPMsizeX, SPMsizeY, SPMsizeZ);
    G4LogicalVolume* spotPositionMonitorLogicalVolume = new G4LogicalVolume(spotPositionMonitor, Air, "spotPositionMonitorLog");
    //G4VPhysicalVolume * spotPositionMonitorPhys = new G4PVPlacement(0, G4ThreeVector(SPMposX,SPMposY,SPMposZ), "spotPositionMonitorPhys", spotPositionMonitorLogicalVolume, mother, false, 0);
    G4VPhysicalVolume * spotPositionMonitorPhys = new G4PVPlacement(0, G4ThreeVector(SPMposX,SPMposY,SPMposZ), spotPositionMonitorLogicalVolume, "spotPositionMonitorPhys", mother, false, 0);

    //*********************************************************
    // Polyimide window
    //*********************************************************

    G4Box* PolyimideSPM = new G4Box("upPolyimideSPM", SPMsizeX, SPMsizeY, 12.5/2 *um);
    G4LogicalVolume* PolyimideSPMLogicalVolume = new G4LogicalVolume(PolyimideSPM, Polyimide, "PolyimideSPMLog");

    // Upstrem Polyimide window
    G4VPhysicalVolume * upPolyimideSPMPhys = new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            1.000015 *cm),
                              "upPolyimideSPMPhys",
                              PolyimideSPMLogicalVolume,
                              spotPositionMonitorPhys,
                              false,
                              0);

    // Downstrem Polyimide window
    G4VPhysicalVolume * downPolyimideSPMPhys = new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            -1.000015 *cm),
                              "downPolyimideSPMPhys",
                              PolyimideSPMLogicalVolume,
                              spotPositionMonitorPhys,
                              false,
                              0);

    G4VisAttributes* polyimideVisAtt = new G4VisAttributes(G4Colour(0.5,0.5,0.5));
    PolyimideSPMLogicalVolume->SetVisAttributes(polyimideVisAtt);

    //*********************************************************
    // Cu coating (Polyimide window)
    //*********************************************************

    G4Box* CuSPM = new G4Box("CuSPM", SPMsizeX, SPMsizeY, 0.2/2 *um);
    G4LogicalVolume* CuSPMLogicalVolume = new G4LogicalVolume(CuSPM, Copper, "CuSPMLog");

    // Upstream Cu coating (Polyimide window)
    G4VPhysicalVolume * upCuSPMPhys = new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0.99938 *cm),
                              "upCuSPMPhys",
                              CuSPMLogicalVolume,
                              spotPositionMonitorPhys,
                              false,
                              0);

    // Downstream Cu coating (Polyimide window)
    G4VPhysicalVolume * downCuSPMPhys = new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            -0.99938 *cm),
                              "downCuSPMPhys",
                              CuSPMLogicalVolume,
                              spotPositionMonitorPhys,
                              false,
                              0);

    G4VisAttributes* CuVisAtt = new G4VisAttributes(G4Colour(0.0,0.5,0.5));
    CuSPMLogicalVolume->SetVisAttributes(CuVisAtt);

    //*********************************************************
    // Al coating (Polyimide window)
    //*********************************************************
    G4Box* AlSPM = new G4Box("AlSPM", SPMsizeX, SPMsizeY, 0.1/2 *um);
    G4LogicalVolume* AlSPMLogicalVolume = new G4LogicalVolume(AlSPM, Aluminum, "AlSPMLog");

    // Upstrem Al coating (Polyimide window)
    G4VPhysicalVolume * upAlSPMPhys = new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            0.999365 *cm),
                              "upAlSPMPhys",
                              AlSPMLogicalVolume,
                              spotPositionMonitorPhys,
                              false,
                              0);

    // Downstrem Al coating (Polyimide window)
    G4VPhysicalVolume * downAlSPMPhys = new G4PVPlacement(0,
                              G4ThreeVector(0,
                                            0,
                                            -0.999365 *cm),
                              "downAlSPMPhys",
                              AlSPMLogicalVolume,
                              spotPositionMonitorPhys,
                              false,
                              0);

    G4VisAttributes* AlVisAtt = new G4VisAttributes(G4Colour(0.0,0.0,0.5));
    AlSPMLogicalVolume->SetVisAttributes(AlVisAtt);

    //*********************************************************
    // 0.03 mm diam (0.015 mm rad) Tungstem wires
    //*********************************************************

    G4double wireR0   = 0     *mm;
    G4double wireR    = 0.015 *mm;
    G4double wirePhi0 = 0     *deg;
    G4double wirePhi  = 360   *deg;
    G4double wireZ    = 35/2  *cm;

    G4Tubs* wireSPM = new G4Tubs("wireSPM", wireR0, wireR, wireZ, wirePhi0, wirePhi);
    G4LogicalVolume* wireSPMLogicalVolume = new G4LogicalVolume(wireSPM, Tungsten, "wireSPMLog");

    G4double wireSPMpitch = 0.2 *cm;
    G4RotationMatrix* RotationY = new G4RotationMatrix();
    RotationY->rotateY(90*deg);
    G4RotationMatrix* RotationX = new G4RotationMatrix();
    RotationX->rotateX(90*deg);

    G4String name;
    std::stringstream replNum;
    for (int i=0; i<160; i++)
    {
        replNum << i;
        name = "wireSPMXPhys_" + replNum.str();
        G4VPhysicalVolume * wireSPMXPhys = new G4PVPlacement(RotationY,
                                  G4ThreeVector(0,
                                                -15.9 *cm +
                                                wireSPMpitch*i,
                                                0.09936*cm),
                                  name,
                                  wireSPMLogicalVolume,
                                  spotPositionMonitorPhys,
                                  false,
                                  0);
        replNum.str("");
    }

    for (int i=0; i<174; i++)
    {
        replNum << i;
        name = "wireSPMYPhys_" + replNum.str();
        G4VPhysicalVolume * wireSPMYPhys = new G4PVPlacement(RotationX,
                                  G4ThreeVector(-17.3 *cm +
                                                wireSPMpitch*i,
                                                0,
                                                -0.10064 *cm),
                                  name,
                                  wireSPMLogicalVolume,
                                  spotPositionMonitorPhys,
                                  false,
                                  0);
        replNum.str("");
    }

    G4VisAttributes* wireVisAtt = new G4VisAttributes(G4Colour(1.0,0.5,0.5));
    wireSPMLogicalVolume->SetVisAttributes(wireVisAtt);
}

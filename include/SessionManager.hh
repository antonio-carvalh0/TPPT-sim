#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "DetComp.hh"

#include <string>
#include <vector>

#include "ScintRecord.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

class G4Material;
class G4String;
class G4UImanager;
class G4UIExecutive;
class G4RunManager;
class G4VisManager;
class SourceModeBase;
class SimModeBase;
class PhantomModeBase;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4ParticleGun;
class G4ParticleDefinition;
class G4Region;
class G4VModularPhysicsList;
namespace CLHEP { class RanecuEngine; }

class SessionManager
{
    public:
        static SessionManager& getInstance();

    private:
        SessionManager();
        ~SessionManager();

    public:
        SessionManager(SessionManager const&) = delete;
        void operator=(SessionManager const&) = delete;

        void startSession();
        void endSession();

        void configureGUI();
        void startGUI();
        void configureOutput();
        void configureRandomGenerator();
        void initializeSource();
        void configureVerbosity();
        void scanMaterials();
        void createFastSimulationPhysics(G4VModularPhysicsList * physicsList);
        void registerAcollinearGammaModel(G4Region * region);
        void registerParticleKillerModel(G4Region * region);
        void registerFastPESModel(G4Region * region);
        void createPhantomRegion(G4LogicalVolume * logVolPhantom);
        void createScintillatorRegion(G4LogicalVolume * logVolScint);
        int  countScintillators() const;
        int  getNumberNatRadEvents(double timeFromInNs, double timeToInNs) const;
        bool detectorContains(const std::string & component);
        void saveScintillatorTable(const std::string & fileName);
        int  isDirExist(const std::string & dirName);
        static int isFileExist(const std::string & fileName);
        static double interpolate (double a, double b, double fraction); // a + fraction * (b - a)    assumes that fraction is in [0, 1]

        void saveConfig(const std::string & fileName) const;
        void loadConfig(const std::string & fileName);

     // Main settings
        SourceModeBase   * SourceMode    = nullptr;
        SimModeBase      * SimMode       = nullptr;
        PhantomModeBase  * PhantomMode   = nullptr;

        bool SimAcollinearity = false;
        bool KillNeutrinos    = false;

        bool FastPESGeneration = false; // do not set it by hand! Automaticaly set if PesGenerationMode is selected
        std::string PesGenerationFile = "";

        DetComp DetectorComposition;

        std::string GdmlFileName = "detector.gdml";

        std::string WorkingDirectory = "Sure+Does+Not+Exist";
        std::string FileName   = "TpptSim_DefaultSaveName.txt";

        bool bBinOutput        = false;

        int  Seed             = 0;      // long->int because of json11
        bool Verbose          = false;
        bool Debug            = false;
        bool ShowEventNumber  = false;
        int  EvNumberInterval = 1000;

        std::vector<ScintRecord> ScintRecords;

        G4ParticleDefinition * GammaPD = nullptr;

     // Cuts
        double CutPhantomGamma    = 10.0*mm;
        double CutPhantomElectron = 10.0*mm;
        double CutPhantomPositron = 0.1 *mm;
        double CutScintGamma      = 0.1 *mm;
        double CutScintElectron   = 0.1 *mm;
        double CutScintPositron   = 0.1 *mm;

     // Step limiter
        bool   UseStepLimiter     = false;
        double PhantomStepLimt    = 10000.0*mm;

     // Misc
        double activityLYSO    = 281.0; // decays per second per cm3

     // Geometry - Scintillators + Encapsulation
        int    NumScintX       = 8;
        int    NumScintY       = 8;

        double ScintSizeX      = 3.005  * mm;
        double ScintSizeY      = ScintSizeX;
        double ScintSizeZ      = 15.0 * mm;

        double TeflonThick     = 0.195 * mm;

        double ScintPitchX     = ScintSizeX + TeflonThick;
        double ScintPitchY     = ScintSizeY + TeflonThick;

        double EncapsSizeX     = ScintSizeX * NumScintX  + TeflonThick * (NumScintX - 1 + 2); // between and on outsides
        double EncapsSizeY     = EncapsSizeX;
        double EncapsSizeZ     = ScintSizeZ + TeflonThick;

        int    NumSegments     = 12;
        int    NumRows         = 4;

        double RowGap          = 0.6  * mm;
        double Angle0          = 40.5 * deg;
        double AngularStep     = 9.0  * deg;

        double InnerDiam       = 335.4 * mm;

     // Geometry - Base and Base Plate
        double BaseRMin        = 162.5 * mm;
        double BaseRMax        = 300.0 * mm;
        double SystHeight      = 105.0 * mm;
        double BaseHeight      = 6.35  * mm;
        double BaseSegment     = 120.0 * deg;

        double BPlateRMin      = 170.2   * mm;
        double BPlateRMax      = 290.793 * mm;
        double BPlateHeight    = 0.7938  * mm;
        double BPlateSegment   = 116.0   * deg;

        double GlobalZ0        = 0;
        double IsoCenterGDML   = 0.5 * (BaseHeight + SystHeight) + 3.9688 * mm;

     // Geometry - Closed Structure
        double InnerWallThick  = 1.0   * mm;
        double OuterWallThick  = 1.0   * mm;
        double WallsSegment    = 120.0 * deg;
        double SideWallSegment = 1.0   * deg;

     // Geometry - SiPMs
     // https://www.hamamatsu.com/resources/pdf/ssd/s14160_s14161_series_kapd1064e.pdf, page 4
        double SIPMSizeX       = EncapsSizeX;
        double SIPMSizeY       = SIPMSizeX;
        double SIPMSizeZ       = 1.35 * mm;

     // Geometry - PCB
     // PCB1 - closer to SiPMs
        double SIPMPCB1Gap     = 0.010 * mm;
        double PCB1SizeX       = 28.2  * mm;
        double PCB1SizeY       = 52.2  * mm;
        double PCB1SizeZ       = 3.18  * mm;

     // PCB2
        double PCB2SizeX       = 25.2 * mm ;
        double PCB2SizeY       = 1.2  * mm;
        double PCB2SizeZ       = 36.0 * mm;
        double MiddlePCBGap    = (40.96 * mm - PCB2SizeZ) / 2.0 * mm;
        double PCB2Z1          = GlobalZ0 + 36.9 * mm;
        double PCB2Z2          = GlobalZ0 - 36.9 * mm;
        double PCB2Z3          = GlobalZ0 + 15.9 * mm;
        double PCB2Z4          = GlobalZ0 - 15.9 * mm;

     // PCB3
        double PCB3SizeX       = PCB1SizeX;
        double PCB3SizeY       = PCB1SizeY;
        double PCB3SizeZ       = 1.95 * mm;

        int    PCB1PCB3NumRows = 2;
        double PCB1PCB3Pitch   = PCB1SizeY;
        double PCB2Pitch       = SystHeight / 4.0;

     // Geometry - Copper Structure
     // Copper columns - from between the scintillators (Column1) to the copper connectors (Column3)
        double Column1SixeX1   = 3.16  * mm;
        double Column1SizeX2   = 2.21  * mm;
        double Column1SizeZ    = 6.0   * mm;
        double Column1SizeY    = SystHeight;
        double Column1RSurf    = 10.8  * mm; //approximate value. Can't be calculated

        double Column2SixeX1   = 1.25  * mm;
        double Column2SizeX2   = 0.75  * mm;
        double Column2SizeY    = SystHeight;
        double Column2SizeZ    = 3.27  * mm;

        double Column3SixeX1   = 4.85  * mm;
        double Column3SizeX2   = 4.25  * mm;
        double Column3SizeY    = SystHeight;
        double Column3SizeZ    = 4.73*mm;

     // Copper pieces that connect Column3 to the horizontal copper connectors
        double Piece1SizeX1    = 6.28 * mm;
        double Piece1SizeX2    = 4.85 * mm;
        double Piece1SizeY     = 6.6  * mm;
        double Piece1SizeZ     = 8.0  * mm;
        double Piece1Z1        = GlobalZ0 + 38.2 * mm;
        double Piece1Z2        = GlobalZ0 - 38.2 * mm;

        double Piece2SizeX1    = Piece1SizeX1;
        double Piece2SizeX2    = Piece1SizeX2;
        double Piece2SizeY     = 5.9  * mm;
        double Piece2SizeZ     = Piece1SizeZ;
        double Piece2Z1        = GlobalZ0 + 14.95 * mm;
        double Piece2Z2        = GlobalZ0 - 14.95 * mm;

        double Piece3SizeX1    = 6.4  * mm;
        double Piece3SizeX2    = 6.28 * mm;
        double Piece3SizeY     = Piece1SizeY;
        double Piece3SizeZ     = 1.4  * mm;
        double Piece3Z1        = Piece1Z1;
        double Piece3Z2        = Piece1Z2;

        double Piece4SizeX1    = Piece3SizeX1;
        double Piece4SizeX2    = Piece3SizeX2;
        double Piece4SizeY     = Piece2SizeY;
        double Piece4SizeZ     = Piece3SizeZ;
        double Piece4Z1        = Piece2Z1;
        double Piece4Z2        = Piece2Z2;

    // Connectors - horizontal boxes that conect the external columns to the rest of the copper structure and "hide" the PCB2
        double ConnectorSizeX  = 6.4  * mm;
        double OutConnectSizeY = 6.6  * mm;
        double InConnectSizeY  = 5.9  * mm;
        double ConnectorSizeZ  = 21.6 * mm;

        double ConnectorZ1     = GlobalZ0 + 38.2  * mm;
        double ConnectorZ2     = GlobalZ0 - 38.2  * mm;
        double ConnectorZ3     = GlobalZ0 + 14.95 * mm;
        double ConnectorZ4     = GlobalZ0 - 14.95 * mm;

     // External columns (Column4)
        double ExtColumnSizeX  = 6.4 * mm;
        double ExtColumnSizeY  = 5.0 * mm;
        double ExtColumnSizeZ  = SystHeight;

     // Holders (pieces of copper connected to the external columns)
        double HolderSizeX     = 6.4  * mm;
        double HolderSizeY     = HolderSizeX;
        double HolderSizeZ     = 16.7 * mm;

        double HolderZ1        = GlobalZ0 + 49.3 * mm;
        double HolderZ2        = GlobalZ0 - 0.8  * mm;
        double HolderZ3        = GlobalZ0 - 49.3 * mm;

     // Geometry - Cooling Assemblies
     // Copper pipe holder (copper box)
        double CopperBoxRmin   = 200.0 * mm;
        double CopperBoxRmax   = 216.0 * mm;
        double CopperBoxHeight = 6.0   * mm;
        double CopperBoxZ1     = ConnectorZ1 - 0.5 * (OutConnectSizeY + CopperBoxHeight) * mm;
        double CopperBoxZ2     = ConnectorZ2 + 0.5 * (OutConnectSizeY + CopperBoxHeight) * mm;
        double CopperBoxZ3     = ConnectorZ3 + 0.5 * (InConnectSizeY  + CopperBoxHeight) * mm;
        double CopperBoxZ4     = ConnectorZ4 - 0.5 * (InConnectSizeY  + CopperBoxHeight) * mm;

     // Water
        double WaterRmin       = 201.5 * mm;
        double WaterRmax       = 211.6 * mm;
        double WaterHeight     = 4.71  * mm;

        double CoolingSegment  = 120.0 * deg - 2.0 * SideWallSegment;

     // Internal resources
        std::ofstream       * outStream  = nullptr;
        G4Region            * regPhantom  = nullptr;
        G4Region            * regScint    = nullptr;

     // External resources
        G4Material          * ScintMat    = nullptr;
        G4VPhysicalVolume   * physWorld   = nullptr;

        CLHEP::RanecuEngine * randGen     = nullptr;

        G4UIExecutive       * ui          = nullptr;
        G4RunManager        * runManager  = nullptr;
        G4VisManager        * visManager  = nullptr;
};

#endif // SESSIONMANAGER_H

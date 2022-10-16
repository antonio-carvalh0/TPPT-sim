#include "SessionManager.hh"
#include "DetComp.hh"
#include "PhantomMode.hh"
#include "SourceMode.hh"
#include "SimMode.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "ScintRecord.hh"
#include "out.hh"
#include "jstools.hh"

#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4StepLimiterPhysics.hh"
#include "QGSP_BIC_HP.hh"
#include "G4UImanager.hh"
#include "G4RandomTools.hh"
#include "G4String.hh"
#include "G4FastSimulationPhysics.hh"
#include "G4ios.hh"
#include "G4SDManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Gamma.hh"
#include "G4RegionStore.hh"
#include "G4AutoDelete.hh"
#include "G4Colour.hh"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

SessionManager & SessionManager::getInstance()
{
    static SessionManager instance; // Guaranteed to be destroyed, instantiated on first use.
    return instance;
}

SessionManager::SessionManager()
{
    runManager = new G4RunManager;
}

void SessionManager::startSession()
{
    if (!isDirExist(WorkingDirectory))
    {
        out("Provided working directory does not exist:\n", WorkingDirectory);
        exit(1);
    }
    if (!SourceMode)
    {
        out("Source mode not provided!");
        exit(2);
    }
    if (!SimMode)
    {
        out("Simulation mode not provided!");
        exit(3);
    }
    if (!PhantomMode)
    {
        out("Phantom mode not provided!");
        exit(4);
    }

    SimMode->preInit();

    DetectorConstruction * theDetector = new DetectorConstruction();
    runManager->SetUserInitialization(theDetector);

    G4VModularPhysicsList* physicsList = new QGSP_BIC_HP;
    physicsList->RegisterPhysics(new G4StepLimiterPhysics());
    physicsList->SetDefaultCutValue(0.1*mm);  // see createPhantomRegion and createScintRegion for specific cuts!
    runManager->SetUserInitialization(physicsList);
    if (SimAcollinearity || KillNeutrinos || FastPESGeneration) createFastSimulationPhysics(physicsList);

    runManager->SetUserAction(new PrimaryGeneratorAction); // SourceMode cannot be directly inherited from G4VUserPrimaryGeneratorAction due to initialization order

    G4UserTrackingAction * TrackAct = SimMode->getTrackingAction();
    if (TrackAct) runManager->SetUserAction(TrackAct);

    G4UserSteppingAction * StepAct = SimMode->getSteppingAction();
    if (StepAct) runManager->SetUserAction(StepAct);

    G4UserStackingAction * StackAct = SimMode->getStackingAction();
    if (StackAct) runManager->SetUserAction(StackAct);

    runManager->SetUserAction(new EventAction);

    // ---
    runManager->Initialize();
    // ---

    GammaPD = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    configureRandomGenerator();
    initializeSource();
    if (SimMode->bNeedGui)    configureGUI();
    if (SimMode->bNeedOutput) configureOutput();
    configureVerbosity();

    saveConfig(WorkingDirectory + "/SimConfig.json");
}

SessionManager::~SessionManager()
{
    delete PhantomMode;
    delete SourceMode;
    delete SimMode;
}

void SessionManager::configureGUI()
{
    char ch = 's';
    char * ad[] = {&ch};
    ui         = new G4UIExecutive(1, ad);
    visManager = new G4VisExecutive("Quiet");

    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/hits/verbose 2");
    UImanager->ApplyCommand("/tracking/verbose 2");
    UImanager->ApplyCommand("/control/saveHistory");

    //if ( detectorContains(DetComp::GDML) )
        scanMaterials();
}

void SessionManager::scanMaterials()
{
    out("-->Scanning materials...");

    std::vector<G4LogicalVolume*> * lvs = G4LogicalVolumeStore::GetInstance();
    for (G4LogicalVolume * lv : *lvs)
    {
        G4Material * mat = lv->GetMaterial();
        //out(lv->GetName(), mat->GetName(), mat->GetChemicalFormula(), mat->GetDensity());

        G4Colour brown(0.45,0.25,0.0);
        G4Colour grey(0.5,0.5,0.5);

        const G4String & name = mat->GetName();
        if      (name == "G4_Al") lv->SetVisAttributes(G4VisAttributes(G4Colour(0.0, 0, 1.0)));
        else if (name == "G4_Cu") lv->SetVisAttributes(new G4VisAttributes(brown));
        else if (name == "SiPM") lv->SetVisAttributes(G4VisAttributes(G4Colour(0, 1.0, 0)));
        else if (name == "PBC") lv->SetVisAttributes(G4VisAttributes(G4Colour(0, 1.0, 0)));
        else if (name == "ABS") lv->SetVisAttributes(new G4VisAttributes(grey));

        else if (name == "Polyimide") lv->SetVisAttributes(new G4VisAttributes(G4Color::Yellow()));
        else if (name == "Stainless") lv->SetVisAttributes(G4Color::Cyan());
        else if (name == "Copper") lv->SetVisAttributes(new G4VisAttributes(brown));
        else if (name == "Tungsten") lv->SetVisAttributes(new G4VisAttributes(G4Color::Magenta()));
        else if (name == "Ceramics") lv->SetVisAttributes(new G4VisAttributes(G4Color::Green()));
        else if (name == "Aluminum") lv->SetVisAttributes(new G4VisAttributes(G4Color::Blue()));
        else if (name == "Helium") lv->SetVisAttributes(G4Color::Red());
    }

    out("<--Material scan completed");
}

void SessionManager::createFastSimulationPhysics(G4VModularPhysicsList * physicsList)
{
    G4FastSimulationPhysics * fsm = new G4FastSimulationPhysics();
    //https://indico.cern.ch/event/789510/contributions/3297180/attachments/1817759/2973421/G4Tutorial_fastSim_vFin.pdf

    // see createPhantomRegion() for registration of the models
    if (SimAcollinearity)  fsm->ActivateFastSimulation("gamma");  // see registerAcollinearGammaModel()
    if (KillNeutrinos)     fsm->ActivateFastSimulation("nu_e");   // see registerParticleKillerModel()
    if (FastPESGeneration) fsm->ActivateFastSimulation("proton"); // see registerFastPESModel()
    physicsList->RegisterPhysics(fsm);
}

#include "AcollinearGammaModel.hh"
void SessionManager::registerAcollinearGammaModel(G4Region * region)
{
    AcollinearGammaModel * mod = new AcollinearGammaModel("AcollinearGammas", region);
    G4AutoDelete::Register(mod);
}

#include "ParticleKiller.hh"
void SessionManager::registerParticleKillerModel(G4Region *region)
{
    ParticleKillerModel * mod = new ParticleKillerModel("ParticleKiller", region);
    G4AutoDelete::Register(mod);
}

#include "FastPesGenerator.hh"
void SessionManager::registerFastPESModel(G4Region *region)
{
    FastPesGeneratorModel * mod = new FastPesGeneratorModel("PesGenerator", region);
    G4AutoDelete::Register(mod);
}

#include "G4UserLimits.hh"
void SessionManager::createPhantomRegion(G4LogicalVolume * logVolPhantom)
{
    regPhantom = new G4Region("Phantom");
    regPhantom->AddRootLogicalVolume(logVolPhantom);

    if (SimAcollinearity)  registerAcollinearGammaModel(regPhantom);
    if (KillNeutrinos)     registerParticleKillerModel(regPhantom);
    if (FastPESGeneration) registerFastPESModel(regPhantom);

    G4ProductionCuts * cuts = new G4ProductionCuts();
    cuts->SetProductionCut(CutPhantomGamma,    G4ProductionCuts::GetIndex("gamma"));
    cuts->SetProductionCut(CutPhantomElectron, G4ProductionCuts::GetIndex("e-"));
    cuts->SetProductionCut(CutPhantomPositron, G4ProductionCuts::GetIndex("e+"));
    regPhantom->SetProductionCuts(cuts);

    if (UseStepLimiter)
    {
        G4UserLimits * stepLimit = new G4UserLimits(PhantomStepLimt);
        regPhantom->SetUserLimits(stepLimit);
    }
}

void SessionManager::createScintillatorRegion(G4LogicalVolume * logVolScint)
{
    regScint = new G4Region("Scintillators");
    regScint->AddRootLogicalVolume(logVolScint);

    G4ProductionCuts * cuts = new G4ProductionCuts();
    cuts->SetProductionCut(CutScintGamma,    G4ProductionCuts::GetIndex("gamma"));
    cuts->SetProductionCut(CutScintElectron, G4ProductionCuts::GetIndex("e-"));
    cuts->SetProductionCut(CutScintPositron, G4ProductionCuts::GetIndex("e+"));
    regScint->SetProductionCuts(cuts);
}

int SessionManager::countScintillators() const
{
    return NumScintX * NumScintY * NumRows * NumSegments * 2.0;
}

int SessionManager::getNumberNatRadEvents(double timeFromInNs, double timeToInNs) const
{
    double volume_cm3 = ScintSizeX/cm * ScintSizeY/cm * ScintSizeZ/cm;
    out("---Volume of one scintillator =", volume_cm3, "cm3");
    volume_cm3 *= countScintillators();
    out("---Total LYSO volume = ", volume_cm3, "cm3");
    double decaysPerSecond = activityLYSO * volume_cm3;
    out("---Decays per second = ", decaysPerSecond);

    const int numEvents = decaysPerSecond * 1e-9 * (timeToInNs - timeFromInNs);
    out("---Decays for time range from", timeFromInNs, "ns to", timeToInNs, "ns =", numEvents);
    return numEvents;
}

bool SessionManager::detectorContains(const std::string & component)
{
    return DetectorComposition.contains(component);
}

void SessionManager::saveScintillatorTable(const std::string & fileName)
{
    std::ofstream stream;
    stream.open(fileName);
    if (!stream.is_open() || stream.fail() || stream.bad())
    {
        std::cout  << "Cannot open file to store scintillator data: " << fileName << std::endl;
        return;
    }

    for (ScintRecord & rec : ScintRecords) rec.write(stream);

    stream.close();
}

void SessionManager::startGUI()
{
    visManager->Initialize();
    G4UImanager * UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
}

void SessionManager::configureOutput()
{
    outStream = new std::ofstream();

    std::string fullFileName = WorkingDirectory + "/" + FileName;
    if (bBinOutput) outStream->open(fullFileName, std::ios::out | std::ios::binary);
    else outStream->open(fullFileName);

    if (!outStream->is_open() || outStream->fail() || outStream->bad())
    {
        out("Cannot open file to store output data!");
        outFlush();
        exit(1);
        //delete outStream; outStream = nullptr;
    }
    else out("\nSaving output to file", fullFileName);
}

void SessionManager::configureRandomGenerator()
{
    randGen = new CLHEP::RanecuEngine();
    randGen->setSeed(Seed);
    G4Random::setTheEngine(randGen);
}

void SessionManager::initializeSource()
{
    SourceMode->initialize();
}

void SessionManager::configureVerbosity()
{
    G4UImanager * UImanager = G4UImanager::GetUIpointer();
    if (Verbose)
    {
        UImanager->ApplyCommand("/hits/verbose 2");
        UImanager->ApplyCommand("/tracking/verbose 2");
        UImanager->ApplyCommand("/control/saveHistory");
    }
    else
    {
        UImanager->ApplyCommand("/hits/verbose 0");
        UImanager->ApplyCommand("/tracking/verbose 0");
        UImanager->ApplyCommand("/control/verbose 0");
        UImanager->ApplyCommand("/run/verbose 0");
    }
}

void SessionManager::endSession()
{
    if (outStream) outStream->close();
    delete outStream;

    delete visManager;
    delete runManager;
    delete ui;
}

#include <sys/types.h>
#include <sys/stat.h>
int SessionManager::isDirExist(const std::string & dirName)
{
    struct stat info;

    if (stat(dirName.data(), &info) != 0) return false;
    else if (info.st_mode & S_IFDIR)      return true;
    else                                  return false;
}

int SessionManager::isFileExist(const std::string & fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

double SessionManager::interpolate(double a, double b, double fraction)
{
    //out("    interpolation->", a, b, fraction);
    if (fraction == 0.0) return a;
    if (fraction == 1.0) return b;
    return a + fraction * (b - a);
}

#include "json11.hh"
void SessionManager::saveConfig(const std::string & fileName) const
{
    json11::Json::object json;

    json["Seed"] = Seed;

    json["SimAcollinearity"] = SimAcollinearity;
    json["KillNeutrinos"]    = KillNeutrinos;

    json11::Json::object jsCuts;
        jsCuts["CutPhantomGamma"]    = CutPhantomGamma;
        jsCuts["CutPhantomElectron"] = CutPhantomElectron;
        jsCuts["CutPhantomPositron"] = CutPhantomPositron;
        jsCuts["CutScintGamma"]      = CutScintGamma;
        jsCuts["CutScintElectron"]   = CutScintElectron;
        jsCuts["CutScintPositron"]   = CutScintPositron;
    json["Cuts"] = jsCuts;

    json["WorkingDirectory"] = WorkingDirectory;

    json["Verbose"] = Verbose;
    json["Debug"]   = Debug;

    json["ShowEventNumber"]  = ShowEventNumber;
    json["EvNumberInterval"] = EvNumberInterval;

    // Phantom
    {
        json11::Json::object js;
        PhantomMode->writeToJson(js);
        json["PhantomMode"] = js;
    }

    // Detector composition
    {
        json11::Json::array ar;
        DetectorComposition.writeToJsonAr(ar);
        json["DetectorComposition"] = ar;

#ifdef USE_GDML
        json["GdmlFileName"] = GdmlFileName;
#endif
    }

    // Source
    {
        json11::Json::object js;
        SourceMode->writeToJson(js);
        json["SourceMode"] = js;
    }

    // Simulation mode
    {
        json11::Json::object js;
        SimMode->writeToJson(js);
        json["SimMode"] = js;
    }

    std::string json_str = json11::Json(json).dump();
    std::ofstream confStream;
    confStream.open(fileName);
    if (confStream.is_open())
        confStream << json_str << std::endl;
    confStream.close();
}

void SessionManager::loadConfig(const std::string & fileName)
{
    if (!isFileExist(fileName))
    {
        out("File", fileName, "does not exist or cannot be open!");
        exit(1);
    }

    out("\nReading config file:", fileName);
    std::ifstream in(fileName);
    std::stringstream sstr;
    sstr << in.rdbuf();
    in.close();
    std::string cs = sstr.str();

    std::string err;
    json11::Json json = json11::Json::parse(cs, err);
    if (!err.empty())
    {
        out(err);
        exit(2);
    }

    jstools::readInt(json,  "Seed",             Seed);
    jstools::readBool(json, "SimAcollinearity", SimAcollinearity);
    jstools::readBool(json, "KillNeutrinos",    KillNeutrinos);

    json11::Json::object jsCuts;
    jstools::readObject(json, "Cuts", jsCuts);
    {
        jstools::readDouble(jsCuts, "CutPhantomGamma",    CutPhantomGamma);
        jstools::readDouble(jsCuts, "CutPhantomElectron", CutPhantomElectron);
        jstools::readDouble(jsCuts, "CutPhantomPositron", CutPhantomPositron);

        jstools::readDouble(jsCuts, "CutScintGamma",      CutScintGamma);
        jstools::readDouble(jsCuts, "CutScintElectron",   CutScintElectron);
        jstools::readDouble(jsCuts, "CutScintPositron",   CutScintPositron);
    }

    jstools::readString(json, "WorkingDirectory", WorkingDirectory);
    if (!isDirExist(WorkingDirectory))
    {
        out("Directory does not exist:", WorkingDirectory);
        exit(3);
    }

    jstools::readBool(json, "Verbose", Verbose);
    jstools::readBool(json, "Debug",   Debug);

    jstools::readBool(json, "ShowEventNumber", ShowEventNumber);
    jstools::readInt(json, "EvNumberInterval", EvNumberInterval);

    // Phantom
    {
        json11::Json::object js;
        jstools::readObject(json, "PhantomMode", js);
        PhantomMode = PhantomModeFactory::makePhantomModeInstance(js);
    }

    // Detector composition
    {
        json11::Json::array ar;
        jstools::readArray(json, "DetectorComposition", ar);
        DetectorComposition.readFromJsonAr(ar);

#ifdef USE_GDML
        jstools::readString(json, "GdmlFileName", GdmlFileName);
#endif
    }

    // Source
    {
        json11::Json::object js;
        jstools::readObject(json, "SourceMode", js);
        SourceMode = SourceModeFactory::makeSourceModeInstance(js);
    }

    // Simulation mode
    {
        json11::Json::object js;
        jstools::readObject(json, "SimMode", js);
        SimMode = SimModeFactory::makeSimModeInstance(js);
    }

    out("Load success!", "\n^^^^^^^^^^^^^\n");
}

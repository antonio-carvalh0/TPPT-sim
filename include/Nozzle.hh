#ifndef Nozzle_H
#define Nozzle_H

#include "G4ThreeVector.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;

class Nozzle
{
public:
    Nozzle();

    void constructNozzle(G4LogicalVolume * mother);

private:
    G4VPhysicalVolume* treatmentRoomPhysicalVolume;
    G4VPhysicalVolume* patientPhysicalVolume;

    void defineMaterials();
    G4Material * Vacuum;
    G4Material * Air;
    G4Material * Helium;
    G4Material * Polyimide;
    G4Material * Copper;
    G4Material * Aluminum;
    G4Material * Tungsten;
    G4Material * Ceramics;
    G4Material * Stainless;

    void ProfileMonitor(G4LogicalVolume * mother);
    void HeliumChamber(G4LogicalVolume * mother);
    void MainDoseMonitor(G4LogicalVolume * mother);
    void SubDoseMonitor(G4LogicalVolume * mother);
    void SpotPositionMonitor(G4LogicalVolume * mother);
};

#endif // Nozzle_H

#ifndef DicomPhantomParameterisation_HH
#define DicomPhantomParameterisation_HH

#include "Voxel.hh"

#include <vector>
#include <map>

#include "G4VPVParameterisation.hh"
#include "G4ThreeVector.hh"

class G4VisAttributes;

class DicomPhantomParameterisation : public G4VPVParameterisation
{
public:
    DicomPhantomParameterisation(std::vector<Voxel> & voxels);
    ~DicomPhantomParameterisation();

    G4Material * ComputeMaterial(const G4int repNo, G4VPhysicalVolume *currentVol, const G4VTouchable *parentTouch = nullptr) override;
    void         ComputeTransformation (const int copyNo, G4VPhysicalVolume * physVol) const override;

    void enableFalseColors(const std::map<G4String, G4VisAttributes*> & colourMap); // do not own attributes
    void enableDensityColors();

protected:
    std::vector<Voxel> & Voxels;

    bool bUseFalseColors = false;
    double WhiteDensity = 3.0;

    std::map<G4String, G4VisAttributes*> ColourMap;
    G4VisAttributes * defaultVisAttributes = nullptr;
};

#endif

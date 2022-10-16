#include "DicomPhantomParameterisation.hh"
#include "out.hh"

//#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VVisManager.hh"
#include "G4SystemOfUnits.hh"

DicomPhantomParameterisation::DicomPhantomParameterisation(std::vector<Voxel> & voxels) :
    G4VPVParameterisation(), Voxels(voxels)
{
    defaultVisAttributes = new G4VisAttributes(G4Colour(1.0, 0, 0));
}

DicomPhantomParameterisation::~DicomPhantomParameterisation()
{
    delete defaultVisAttributes;
}

G4Material * DicomPhantomParameterisation::ComputeMaterial(const G4int copyNo, G4VPhysicalVolume * physVol, const G4VTouchable *)
{
    G4Material * mat = Voxels[copyNo].Mat;

    if (G4VVisManager::GetConcreteInstance() && physVol)
    {
        if (bUseFalseColors)
        {
            const G4String & matName = mat->GetName();
            auto it = ColourMap.find(matName);
            if (it != ColourMap.end())
                physVol->GetLogicalVolume()->SetVisAttributes(it->second);
            else
                physVol->GetLogicalVolume()->SetVisAttributes(defaultVisAttributes);
        }
        else
        {
            //const double val = mat->GetDensity()/g*cm3 / 2.1;
            double val = mat->GetDensity()/g*cm3;
            //val = val*val / 4.0;
            val = val*val*val / 8.0;
            physVol->GetLogicalVolume()->SetVisAttributes(G4Color{val,val,val,1});
        }
    }

    return mat;
}

void DicomPhantomParameterisation::ComputeTransformation(const int copyNo, G4VPhysicalVolume * physVol) const
{
    const Voxel & vox = Voxels[copyNo];

    physVol->SetTranslation( {vox.X, vox.Y, vox.Z} );
    physVol->SetRotation(nullptr);
}

void DicomPhantomParameterisation::enableFalseColors(const std::map<G4String, G4VisAttributes*> & colourMap)
{
    bUseFalseColors = true;
    ColourMap = colourMap;
}

void DicomPhantomParameterisation::enableDensityColors()
{
    bUseFalseColors = false;
}

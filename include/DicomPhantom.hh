#ifndef DicomPhantom_h
#define DicomPhantom_h

#include "PhantomMode.hh"
#include "Voxel.hh"
#include "globals.hh"
#include "json11.hh"

#include <vector>
#include <map>

class G4Material;
class G4LogicalVolume;
class DicomPhantomZSliceHeader;
class G4VPhysicalVolume;
class G4VisAttributes;

class PhantomDICOM : public PhantomModeBase
{
public:
    // lateral compression other than 1 strongly compromoises accuracy -> intended only for visualization
    PhantomDICOM(std::string dataDir, std::string sliceBaseFileName, int sliceFrom, int sliceTo,
                 int lateralCompression, double containerRadius, const std::vector<double> & posInWorld);

    G4LogicalVolume * definePhantom(G4LogicalVolume * logicWorld) override;
    std::string       getTypeName() const override {return "PhantomDICOM";}
    void              readFromJson(const json11::Json & json) override;

protected:
    void              doWriteToJson(json11::Json::object & json) const override;

    std::string         DataDir;
    std::string         SliceFileBase;
    int                 SliceFrom;
    int                 SliceTo;
    int                 LateralCompression;
    double              PhantRadius;
    std::vector<double> PosInWorld;

    bool                ContainerInvisible;
    bool                UseFalseColors;

    std::vector<std::pair<std::string, float>> MatUpperDens;
    std::map<G4String, G4VisAttributes*>       ColourMap;     // --->PERSISTENT: in use during gui session!

    std::vector<std::string> SliceFiles;

    std::vector<Voxel>       Voxels;                          // --->PERSISTENT: in use during tracking!

    std::vector<G4Material*> Materials;
    G4Material             * AirMat = nullptr;                // voxels of this material are not created
    std::vector<size_t>      MaterialIDs;                     // index of the material for each voxel in the dicom

    std::vector<DicomPhantomZSliceHeader*> SliceHeaders;      // list of z slice headers (one per DICOM file)
    DicomPhantomZSliceHeader * SliceHeaderMerged = nullptr;   // z slice header resulted from merging all z slice headers

    int    NumVoxX, NumVoxY, NumVoxZ;
    double VoxHalfSizeX, VoxHalfSizeY, VoxHalfSizeZ;
    double zStart;

private:
    void buildMaterials();
    void readPhantomData();                                   // materialIDs is filled for all voxels of all slices
    void readPhantomDataFile(const std::string & fname);      // read g4dcm file, constructs SliceHeader and read voxels
    void mergeZSliceHeaders();
    void prepareParameterizationParameters();
    G4LogicalVolume * makeContainer(G4LogicalVolume * logicWorld);
    void constructPhantom(G4LogicalVolume * PhantomLogical);
    void optimizeMemory();
    void readMaterialFile(const std::string & fileName);
    void readColorMap(const std::string & fileName);
    void generateSliceFileNames();

};

#endif

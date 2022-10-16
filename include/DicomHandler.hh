//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file medical/DICOM/include/DicomHandler.hh
/// \brief Definition of the DicomHandler class
//
// The code was written by :
//      *Louis Archambault louis.archambault@phy.ulaval.ca,
//      *Luc Beaulieu beaulieu@phy.ulaval.ca
//      +Vincent Hubert-Tremblay at tigre.2@sympatico.ca
//
//
// *Centre Hospitalier Universitaire de Quebec (CHUQ),
// Hotel-Dieu de Quebec, departement de Radio-oncologie
// 11 cote du palais. Quebec, QC, Canada, G1R 2J6
// tel (418) 525-4444 #6720
// fax (418) 691 5268

//*******************************************************//

#ifndef DicomHandler_h
#define DicomHandler_h 1

#include <cstdio>
#include <map>
#include <fstream>
#include <vector>

#include "globals.hh"

//*******************************************************
/// Dicom Handler class
///        - Handling of DICM images
///        - Transforming *.dcm to *.g4 ( pixels->density )
///        - Reading headers and pixels
///        - Transforming pixel to density and creating *.g4
///          files
///        - Functions are in DicomHandler.cc
///
/// Base on previous code by :
///        Dragan Tubic <tdragan@gel.ulaval.ca>
//*******************************************************

class DicomPhantomZSliceHeader;
class DicomPhantomZSliceMerged;

class DicomHandler
{
public:
    DicomHandler();
    ~DicomHandler();

    void processFiles(const G4String & path, const G4String & convertionFileName, int lateralCompression,
                   const std::vector<std::pair<std::string, float> > & materialUpperDens,
                   const std::vector<std::string> & sliceFiles);

private:
    template <class Type> void GetValue(char *, Type &);
    
    const G4int DATABUFFSIZE = 8192;
    const G4int LINEBUFFSIZE = 5020;
    const G4int FILENAMESIZE = 512;
    
    int   ReadFile(FILE *, std::string);
    int   ReadData(FILE *);
    void  ReadCalibration();
    void  GetInformation(G4int &, char *);
    float Pixel2density(G4int pixel);
    void  ReadMaterialIndices( std::ifstream& finData);
    unsigned int GetMaterialIndex( G4float density );
    void  StoreData(DicomPhantomZSliceHeader* dcmPZSH);
    int   read_defined_nested(FILE *, G4int);
    void  read_undefined_nested(FILE *);
    void  read_undefined_item(FILE *);
    bool  checkG4FilesExist(int lateralCompression, const std::vector<std::string> & sliceFiles);

    short fCompression = 0;
    G4int fNFiles = 0;
    short fRows = 0;
    short fColumns = 0;
    short fBitAllocated = 0;
    G4int fMaxPixelValue = 0;
    G4int fMinPixelValue = 0;
    
    G4double fPixelSpacingX = 0;
    G4double fPixelSpacingY = 0;
    G4double fSliceThickness = 0;
    G4double fSliceLocation = 0;
    
    G4int fRescaleIntercept = 0;
    G4int fRescaleSlope = 0;
    
    G4bool fLittleEndian = true;
    G4bool fImplicitEndian = false;
    short fPixelRepresentation = 0;
    
    //G4int** fTab;  // TODO refactor - not deleted!
    std::vector<std::vector<int>> fTab;
    std::map<G4float,G4String> fMaterialIndices;
    
    G4int fNbrequali = 0;
    G4double * fValueDensity = nullptr; // ok
    G4double * fValueCT = nullptr;      // ok
    G4bool fReadCalibration = false;
    DicomPhantomZSliceMerged * fMergedSlices = nullptr;  // ok

    G4String driverPath;
    G4String fCt2DensityFile;
};

#endif

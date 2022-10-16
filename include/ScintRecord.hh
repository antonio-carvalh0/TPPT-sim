#ifndef scintrecord_h
#define scintrecord_h

#include <G4ThreeVector.hh>

struct ScintRecord
{
    G4ThreeVector CenterPos;
    G4ThreeVector FacePos;

    int    HeadNumber;
    double Angle;
    int    AssemblyNumber;

    void write(std::ofstream & stream);
};

#endif // scintrecord_h


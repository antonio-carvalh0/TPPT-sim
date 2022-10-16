#include "ScintRecord.hh"

#include <iostream>
#include <fstream>

void ScintRecord::write(std::ofstream & stream)
{
    G4ThreeVector Norm = FacePos - CenterPos;
    Norm = Norm.unit();

    stream << FacePos[0] << " " << FacePos[1] << " " << FacePos[2] << " "
           << Norm[0] << " " << Norm[1] << " " << Norm[2] << " "
           << HeadNumber << " "
           << Angle << " "
           << AssemblyNumber << "\n";
}

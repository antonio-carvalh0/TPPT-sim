#include "ParamTest.hh"

#include <G4VPhysicalVolume.hh>

TestParameterisation::TestParameterisation(G4ThreeVector boxXYZsize, double maxCenterRadius, double startZ) :
    BoxXYZsize(boxXYZsize), MaxRadius(maxCenterRadius), StartZ(startZ)
{
    int numY = MaxRadius / BoxXYZsize[1];

    for (int iY = -numY; iY <= numY; iY++)
    {
        double Y = BoxXYZsize[1] * iY;

        double maxX = sqrt(MaxRadius*MaxRadius - Y*Y);
        int numX = maxX / BoxXYZsize[0];

        for (int iX = -numX; iX <= numX; iX++)
            BoxXY.push_back({iX*BoxXYZsize[0], iY*BoxXYZsize[1]});
    }
    BoxesPerSlice = BoxXY.size();
}

void TestParameterisation::ComputeTransformation(const int copyNo, G4VPhysicalVolume * physVol) const
{
    double Z = StartZ + std::ceil(copyNo/BoxesPerSlice) * BoxXYZsize[2];
    int numInSlice = copyNo % BoxesPerSlice;
    const std::pair<double, double> & XY = BoxXY[numInSlice];

    G4ThreeVector origin(XY.first, XY.second, Z);

    physVol->SetTranslation(origin);
    physVol->SetRotation(nullptr);
}


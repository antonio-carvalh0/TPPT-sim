#ifndef paramtest_h
#define paramtest_h

#include "G4VPVParameterisation.hh"
#include "G4ThreeVector.hh"

#include <vector>
#include <map>

class TestParameterisation : public G4VPVParameterisation
{
public:
    TestParameterisation(G4ThreeVector boxXYZsize, double maxCenterRadius, double startZ);
    ~TestParameterisation(){}

    void ComputeTransformation (const int copyNo, G4VPhysicalVolume * physVol) const;

protected:
    G4ThreeVector BoxXYZsize;
    double MaxRadius;
    double StartZ;

    std::vector<std::pair<double,double>> BoxXY;
    int BoxesPerSlice;
};

#endif // paramtest_h

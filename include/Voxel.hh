#ifndef voxel_h
#define voxel_h

class G4Material;

struct Voxel
{
    Voxel(double x, double y, double z, G4Material * mat) : X(x), Y(y), Z(z), Mat(mat) {}

    double X;
    double Y;
    double Z;
    G4Material * Mat;
};

#endif // voxel_h

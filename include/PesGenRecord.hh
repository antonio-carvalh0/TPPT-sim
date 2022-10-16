#ifndef pesgenrecord_h
#define pesgenrecord_h

#include <string>
#include <vector>

class PesGenRecord
{
public:
    PesGenRecord(int targetZ, int targetA, std::string pes) : TargetZ(targetZ), TargetA(targetA), PES(pes) {}
    PesGenRecord(){}

    int         TargetZ; // 6;
    int         TargetA; // 12;

    std::string PES;     // "C11";

    double      DecayTime = 0;     // decay time in seconds = tauHalf / log(2)

    std::vector<std::pair<double,double>> CrossSection; // [MeV] [millibarn]

    //runtime
    double NumberDensity;
    double getCrossSection(double energy) const;

    //only for direct mode
    std::vector<std::vector<std::vector<double>>> * ProbArray = nullptr;
};

#endif // pesgenrecord_h

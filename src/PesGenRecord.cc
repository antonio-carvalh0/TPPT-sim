#include "PesGenRecord.hh"

#include <algorithm>

double PesGenRecord::getCrossSection(double energy) const
{
    //Returns an iterator pointing to the first element in the range [first, last) that is greater than value, or last if no such element is found.
    auto it = std::upper_bound(CrossSection.begin(), CrossSection.end(), energy,
                               [](double one, const std::pair<double,double> & two)
                                 {return one < two.first;}
                              );

    //if (it == CrossSection.begin()) return CrossSection.front().second;
    if (it == CrossSection.begin()) return 0; // assuming the first data point is the threshold
    if (it == CrossSection.end())   return CrossSection.back().second;  // first: energy, second: CS

    // interpolation
    // (e1, A) -> (energy, ?) -> (e2, B)  ==>  ? = A + (B-A)*(energy-e1)/(e2-e1)
    const auto lowIt = it - 1;
    const double val = lowIt->second + (it->second - lowIt->second)*(energy - lowIt->first)/(it->first - lowIt->first);
    //out("Energy:", energy, "XS:", val); exit(111);
    return val;
}

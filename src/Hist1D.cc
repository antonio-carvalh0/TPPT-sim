#include "Hist1D.hh"

#include <iostream>
#include <fstream>
#include <ios>
#include <algorithm>

Hist1D::Hist1D(int numBins, double from, double to) :
    NumBins(numBins), From(from), To(to)
{
    if (NumBins <= 0) NumBins = 1;

    Step = (To - From) / NumBins;

    Data.resize(NumBins);
    for (int iBin = 0; iBin < NumBins; iBin++) Data[iBin] = 0;
}

void Hist1D::fill(double value, double weight)
{
    int index = (value - From) / Step;

    if      (index <  0)       NumUnderflows++;
    else if (index >= NumBins) NumOverflows++;
    else                       Data[index] += weight;
}

void Hist1D::report()
{
    int sum = 0;
    for (int iBin = 0; iBin < NumBins; iBin++)
    {
        std::cout << (iBin == 0 ? '[' : ',');
        std::cout << Data[iBin];
        sum += Data[iBin];
    }
    std::cout << ']' << std::endl;

    std::cout << "Distribution sum: " << sum << std::endl;
    std::cout << "Underflows: " << NumUnderflows << std::endl;
    std::cout << "Overflows: "  << NumOverflows << std::endl;
}

void Hist1D::save(const std::string & fileName)
{
    std::ofstream outStream;

     outStream.open(fileName);

    if (!outStream.is_open() || outStream.fail() || outStream.bad())
    {
        std::cout  << "Cannot open file to store output data: " << fileName << std::endl;
        return;
    }

    for (int iBin = 0; iBin < NumBins; iBin++)
        outStream << (From + Step * iBin) << " " << Data[iBin] << std::endl;

    outStream.close();
}

// ---

Hist1DSampler::Hist1DSampler(const Hist1D & hist, long seed) : Hist(hist)
{
    randEngine.seed(seed);

    double acc = 0;
    for (size_t iBin = 0; iBin < hist.Data.size(); iBin++)
    {
        Cumulative.push_back(SamplerRec(iBin, acc));
        acc += hist.Data[iBin];
    }

    if (acc != 0)
        for (SamplerRec & rec : Cumulative)
            rec.val /= acc;
}

double Hist1DSampler::getRandom()
{
    const double rndm = urd(randEngine);  // [0, 1)
    auto res = std::upper_bound(Cumulative.begin(), Cumulative.end(), SamplerRec(0, rndm)); // iterator to the element with larger val than rndm

    size_t indexAbove = (res == Cumulative.end() ? Cumulative.size()-1
                                                 : res->index);

    const double dBin = (Hist.To - Hist.From) / Hist.NumBins;
    return Hist.From + dBin * (indexAbove - urd(randEngine));
}

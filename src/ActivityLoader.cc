#include "ActivityLoader.hh"
#include "out.hh"

#include <fstream>

void ActivityLoader::load(const std::string & fileName, std::vector<std::vector<std::vector<double>>> & data, BinningParameters & binning)
{
    binning.read(fileName);
    binning.report();

    data.resize(binning.NumBins[0]);
    for (int ix = 0; ix < binning.NumBins[0]; ix++)
    {
        data[ix].resize(binning.NumBins[1]);
        for (int iy = 0; iy < binning.NumBins[1]; iy++)
            data[ix][iy].resize(binning.NumBins[2], 0);
    }

    std::ifstream in(fileName);
    std::string line;
    std::getline(in, line); // skip the first line which is the mapping json

    for (int ix = 0; ix < binning.NumBins[0]; ix++)
        for (int iy = 0; iy < binning.NumBins[1]; iy++)
        {
            std::getline(in, line);
            size_t sz; // std::string::size_type sz;

            for (int iz = 0; iz < binning.NumBins[2]; iz++)
            {
                double val = std::stod(line, &sz);
                line = line.substr(sz);

                data[ix][iy][iz] = val;
            }
        }
}

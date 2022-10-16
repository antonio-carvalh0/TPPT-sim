#ifndef jstools_h
#define jstools_h

#include "json11.hh"

#include <string>

namespace jstools
{
    void assertKey (const json11::Json & json, const std::string & key);

    bool contains  (const json11::Json & json, const std::string & key);

    void readInt   (const json11::Json & json, const std::string & key, int                  & var);
    void readDouble(const json11::Json & json, const std::string & key, double               & var);
    void readBool  (const json11::Json & json, const std::string & key, bool                 & var);
    void readArray (const json11::Json & json, const std::string & key, json11::Json::array  & var);
    void readString(const json11::Json & json, const std::string & key, std::string          & var);
    void readObject(const json11::Json & json, const std::string & key, json11::Json::object & var);
}

struct BinningParameters
{
    std::array<double, 3> BinSize;
    std::array<int,    3> NumBins;
    std::array<double, 3> Origin;

    bool operator!=(const BinningParameters & other) const;

    void read(const std::string & fileName);
    void report();
};

#endif // jstools_h

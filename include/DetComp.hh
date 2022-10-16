#ifndef detcomp_h
#define detcomp_h

#include "json11.hh"

#include <initializer_list>
#include <string>
#include <vector>
#include <array>

class DetComp
{
public:
    static constexpr auto Scintillators     = "Scintillators";
    static constexpr auto GDML              = "GDML";
    static constexpr auto FirstStageMonitor = "FirstStageMonitor";
    static constexpr auto Base              = "Base";
    static constexpr auto ClosedStructure   = "ClosedStructure";
    static constexpr auto SIPM              = "SIPM";
    static constexpr auto PCB               = "PCB";
    static constexpr auto CopperStructure   = "CopperStructure";
    static constexpr auto CoolingAssemblies = "CoolingAssemblies";
    static constexpr auto Nozzle            = "Nozzle";
    // Do not forget to add new types to the ValidComponents list below!!!

private:
    const std::vector<std::string> ValidComponents = {Scintillators, GDML, FirstStageMonitor, Base, ClosedStructure,
                                                      SIPM, PCB, CopperStructure, CoolingAssemblies, Nozzle};

public:
    void set(const std::vector<std::string> & components);
    void add(const std::string & component);

    bool isValid(const std::string & component) const;
    bool contains(const std::string & component) const;

    void writeToJsonAr(json11::Json::array & ar) const;
    void readFromJsonAr(const json11::Json::array & ar);

private:
    std::vector<std::string> EnabledComponents;
};

#endif // detcomp_h

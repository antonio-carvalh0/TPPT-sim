#include "DetComp.hh"
#include "out.hh"

#include <algorithm>

void DetComp::set(const std::vector<std::string> & components)
{
    EnabledComponents = components;
}

void DetComp::add(const std::string & component)
{
    EnabledComponents.push_back(component);
}

bool DetComp::isValid(const std::string & component) const
{
    return (std::find(ValidComponents.begin(), ValidComponents.end(), component) != ValidComponents.end());
}

bool DetComp::contains(const std::string & component) const
{
    return (std::find(EnabledComponents.begin(), EnabledComponents.end(), component) != EnabledComponents.end());
}

void DetComp::writeToJsonAr(json11::Json::array & ar) const
{
    for (const std::string & el : EnabledComponents)
        ar.push_back(el);
}

void DetComp::readFromJsonAr(const json11::Json::array & ar)
{
    EnabledComponents.clear();

    out("Detector composition items:");
    for (size_t i = 0; i < ar.size(); i++)
    {
        const json11::Json & arEl = ar[i];
        const std::string ci = arEl.string_value();
        out("->", ci);
        if (!DetComp::isValid(ci))
        {
            out("This is not a valid detector component!");
            exit(4);
        }
        EnabledComponents.push_back(ci);
    }
    out("Detector composition items end.");
}

#include "jstools.hh"
#include "out.hh"

void jstools::assertKey(const json11::Json & json, const std::string & key)
{
    if (json.object_items().count(key) == 0)
    {
        out("Config json does not contain required key:", key);
        exit(101);
    }
}

bool jstools::contains(const json11::Json & json, const std::string & key)
{
    if (!json.is_object()) return false;
    return (json.object_items().count(key) > 0);
}

void jstools::readInt(const json11::Json & json, const std::string & key, int & var)
{
    assertKey(json, key);
    if (!json[key].is_number())
    {
        out(key, "is not a number");
        exit(102);
    }
    var = json[key].int_value();
    //out(key, var);
}

//#include <sstream>
//#include <iomanip>
void jstools::readDouble(const json11::Json & json, const std::string & key, double & var)
{
    assertKey(json, key);
    if (!json[key].is_number())
    {
        out(key, "is not a number");
        exit(102);
    }
    var = json[key].number_value();

    /*
    //if there will be problems with rounding due to json format-> double
    std::ostringstream streamObj;
    streamObj << std::setprecision(8);
    streamObj << var;
    std::string str = streamObj.str();
    out("-=-=->", str);
    var = stod(str);
    */

    //out(key, var);
}

void jstools::readBool(const json11::Json & json, const std::string & key, bool & var)
{
    assertKey(json, key);
    if (!json[key].is_bool())
    {
        out(key, "is not a bool");
        exit(102);
    }
    var = json[key].bool_value();
    //out(key, (var ? "true" : "false"));
}

void jstools::readArray(const json11::Json &json, const std::string &key, json11::Json::array & var)
{
    assertKey(json, key);
    if (!json[key].is_array())
    {
        out(key, "is not an array");
        exit(102);
    }
    var = json[key].array_items();
}

void jstools::readString(const json11::Json &json, const std::string &key, std::string & var)
{
    assertKey(json, key);
    if (!json[key].is_string())
    {
        out(key, "is not a string");
        exit(102);
    }
    var = json[key].string_value();
    //out(key, var);
}

void jstools::readObject(const json11::Json &json, const std::string &key, json11::Json::object &var)
{
    assertKey(json, key);
    if (!json[key].is_object())
    {
        out(key, "is not an object");
        exit(102);
    }
    var = json[key].object_items();
}

bool BinningParameters::operator!=(const BinningParameters & other) const
{
    for (int i = 0; i < 3; i++)
    {
        if (BinSize[i] != other.BinSize[i]) return true;
        if (NumBins[i] != other.NumBins[i]) return true;
        if (Origin[i]  != other.Origin[i])  return true;
    }
    return false;
}

#include <fstream>
void BinningParameters::read(const std::string & fileName)
{
    std::ifstream in(fileName);
    if (!in.good())
    {
        out("File", fileName, "does not exist or cannot be open!");
        exit(1);
    }

    std::string line;
    std::getline(in, line);
    line.erase(0, 1);

    std::string err;
    json11::Json json = json11::Json::parse(line, err);
    if (!err.empty())
    {
        out(err);
        exit(2);
    }

    json11::Json::array bsar;
    jstools::readArray(json, "BinSize", bsar);
    if (bsar.size() < 3) {out("Cannot read BinSizes"); exit(3);}
    for (int i=0; i<3; i++) BinSize[i] = bsar[i].number_value();

    json11::Json::array nbar;
    jstools::readArray(json, "NumBins", nbar);
    if (nbar.size() < 3) {out("Cannot read NumBins"); exit(3);}
    for (int i=0; i<3; i++) NumBins[i] = nbar[i].int_value();

    json11::Json::array orar;
    jstools::readArray(json, "Origin", orar);
    if (orar.size() < 3) {out("Cannot read Origins"); exit(3);}
    for (int i=0; i<3; i++) Origin[i] = orar[i].number_value();
}

void BinningParameters::report()
{
    out(
          "BinSizes: (", BinSize[0],',',BinSize[1],',',BinSize[2],") ",
          "NumBins:  (", NumBins[0],',',NumBins[1],',',NumBins[2],") ",
          "Origin:   (", Origin[0], ',',Origin[1], ',',Origin[2], ") "
        );
}

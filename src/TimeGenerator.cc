#include "TimeGenerator.hh"
#include "jstools.hh"
#include "out.hh"

#include "G4RandomTools.hh"

#include "Hist1D.hh"

TimeGeneratorBase * TimeGeneratorFactory::makeTimeGeneratorInstance(const json11::Json &json)
{
    out("Reading time generator json");
    std::string Type;
    jstools::readString(json, "Type", Type);

    TimeGeneratorBase * tg = nullptr;

    if      (Type == "ConstantTime")    tg = new ConstantTime(0);
    else if (Type == "UniformTime")     tg = new UniformTime(0, 0);
    else if (Type == "ExponentialTime") tg = new ExponentialTime(0, 0);
    else
    {
        out("Unknown time generator type!");
        exit(30);
    }

    tg->readFromJson(json);
    return tg;
}

// ---

void TimeGeneratorBase::writeToJson(json11::Json::object & json) const
{
    json["Type"] = getTypeName();
    doWriteToJson(json);
}

// ---

void ConstantTime::readFromJson(const json11::Json &json)
{
    jstools::readDouble(json, "Time", Time);
}

void ConstantTime::doWriteToJson(json11::Json::object & json) const
{
    json["Time"] = Time;
}

// ---

double UniformTime::generateTime()
{
    return TimeFrom + (TimeTo - TimeFrom) * G4UniformRand();
}

void UniformTime::readFromJson(const json11::Json &json)
{
    jstools::readDouble(json, "TimeFrom", TimeFrom);
    jstools::readDouble(json, "TimeTo",   TimeTo);
}

void UniformTime::doWriteToJson(json11::Json::object &json) const
{
    json["TimeFrom"] = TimeFrom;
    json["TimeTo"]   = TimeTo;
}

// ---

ExponentialTime::ExponentialTime(double timeFrom, double halfLife) :
    TimeFrom(timeFrom), HalfLife(halfLife),
    DecayTime(halfLife/log(2.0))
{
    //Hist = new Hist1D(100,0,2e11);
}

ExponentialTime::~ExponentialTime()
{
    //Hist->save("/data/margarida/Data/gammasExp.txt");
    //delete Hist;
}

double ExponentialTime::generateTime()
{
    const double time = TimeFrom + G4RandExponential::shoot(DecayTime);
    //Hist->fill(time);
    return time;
}

void ExponentialTime::readFromJson(const json11::Json & json)
{
    jstools::readDouble(json, "TimeFrom", TimeFrom);
    jstools::readDouble(json, "HalfLife", HalfLife);
}

void ExponentialTime::doWriteToJson(json11::Json::object & json) const
{
    json["TimeFrom"] = TimeFrom;
    json["HalfLife"] = HalfLife;
}

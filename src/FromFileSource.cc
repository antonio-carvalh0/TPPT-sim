#include "FromFileSource.hh"
#include "SessionManager.hh"
#include "out.hh"
#include "jstools.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleDefinition.hh"

#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>

FromFileSource::FromFileSource(const std::string & fileName, bool bBinaryFile) :
    SourceModeBase(nullptr, nullptr), FileName(fileName), bBinary(bBinaryFile)
{
    init();
}

FromFileSource::FromFileSource(const json11::Json & json) :
    SourceModeBase(nullptr, nullptr)
{
    doReadFromJson(json);
    readFromJson(json);

    init();
}

void FromFileSource::init()
{
    if (bBinary) inStream = new std::ifstream(FileName, std::ios::in | std::ios::binary);
    else         inStream = new std::ifstream(FileName);

    if (!inStream->is_open())
    {
        out("Cannot open input file with particles:" + FileName);
        exit(1000);
        delete inStream; inStream = nullptr;
    }
    prepareStream();
}

FromFileSource::~FromFileSource()
{
    delete inStream;
}

void FromFileSource::prepareStream()
{
    inStream->clear();
    inStream->seekg(0);

    bool bError = true;
    if (bBinary)
    {
        char ch = 0;
        int iEv;
        inStream->get(ch);
        if (ch == (char)0xEE)
        {
            inStream->read((char*)&iEv, sizeof(int));
            bError = false;
        }
    }
    else
    {
        std::string line;
        std::getline(*inStream, line);
        if (line.size() > 1 && line[0] == '#') bError = false;
    }

    if (bError)
    {
        out("Input file for FromFileSource does not start with the event marker!");
        exit(1002);
    }
}

void FromFileSource::GeneratePrimaries(G4Event * anEvent)
{
    if (!inStream)
    {
        out("Stream with particles does not exist: check file name!");
        return;
    }

    if (inStream->eof())
    {
        out("End of file reached!");
        return;
    }
    if (!inStream->good())
    {
        out("Error in reading particle data from file!");
        exit(111);
    }

    if (bBinary)
    {
        char ch;
        int iEv;
        while (inStream->get(ch))
        {
            if (inStream->eof()) break;

            if (ch == (char)0xEE)
            {
                inStream->read((char*)&iEv, sizeof(int));
                //event finished
                break;
            }
            else if (ch == (char)0xFF)
            {
                name.clear();
                while (inStream->get(ch))
                {
                    if (ch == (char)0x00) break;
                    name += ch;
                }

                inStream->read((char*)&energy, sizeof(double));
                inStream->read((char*)&pos[0], sizeof(double));
                inStream->read((char*)&pos[1], sizeof(double));
                inStream->read((char*)&pos[2], sizeof(double));
                inStream->read((char*)&dir[0], sizeof(double));
                inStream->read((char*)&dir[1], sizeof(double));
                inStream->read((char*)&dir[2], sizeof(double));
                inStream->read((char*)&time,   sizeof(double));

                if (inStream->eof())
                {
                    out("---End of file reached!");
                    return;
                }

                if (!inStream->good())
                {
                    out("Error in reading particle data from file!");
                    exit(111);
                }

                addPrimary(anEvent);
            }
        }
    }
    else
    {
        for (std::string line; std::getline(*inStream, line); )
        {
            //std::cout << "line=>" << line << "<=" << std::endl;
            if (line.size() < 1) continue; //allow empty lines

            if (line[0] == '#') break; //event finished

            if (inStream->eof())
            {
                out("---End of file reached!");
                return;
            }

            std::stringstream ss(line);  // units in file are mm MeV and ns
            ss >> name
               >> energy
               >> pos[0] >> pos[1] >> pos[2]
               >> dir[0] >> dir[1] >> dir[2]
               >> time;
            if (ss.fail())
            {
                out("Unexpected format of a line in the file with the input particles");
                exit(2111);
            }

            addPrimary(anEvent);
        }
    }
}

void FromFileSource::addPrimary(G4Event * anEvent)
{
    pd = ParticleGenerator.makeGeant4Particle(name);

    //out("->", pd->GetParticleName(), energy, pos, dir, time);

    ParticleGun->SetParticleDefinition(pd);
    ParticleGun->SetParticleEnergy(energy*keV);
    ParticleGun->SetParticleTime(time*ns);
    ParticleGun->SetParticlePosition(pos);
    ParticleGun->SetParticleMomentumDirection(dir);

    ParticleGun->GeneratePrimaryVertex(anEvent);
}

double FromFileSource::CountEvents()
{
    if (!inStream) return 0;

    inStream->clear();
    inStream->seekg(0);

    double iCounter = 0;
    if (bBinary)
    {
        char ch;
        int iEv;
        double buf[8]; //en + pos + dir + time
        while (inStream->get(ch))
        {
            if (inStream->eof()) break;

            if (ch == (char)0xEE)
            {
                inStream->read((char*)&iEv, sizeof(int));
                iCounter++;
                continue;
            }
            else if (ch == (char)0xFF)
            {
                while (inStream->get(ch))
                {
                    if (ch == (char)0x00) break;
                }

                inStream->read((char*)buf, 8 * sizeof(double));

                if (inStream->eof()) break;

                if (!inStream->good())
                {
                    out("Format error in binary particle data file!");
                    exit(111);
                }
            }
            else
            {
                out("Data header char error in binary particle data file!");
                exit(111);
            }
        }
    }
    else
    {
        for( std::string line; std::getline( *inStream, line ); )
        {
            if (inStream->fail()) break;
            if (line.length() > 0 && line[0] == '#') iCounter++;
        }
    }

    prepareStream();

    out("Found events:", iCounter);
    return iCounter;
}

void FromFileSource::doWriteToJson(json11::Json::object & json) const
{
    json["FileName"] = FileName;
    json["bBinary"]  = bBinary;
}

void FromFileSource::doReadFromJson(const json11::Json & json)
{
    jstools::readString(json, "FileName", FileName);
    jstools::readBool  (json, "bBinary",  bBinary);
}

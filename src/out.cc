#include "out.hh"
#include "G4ios.hh"

void out()
{
    std::cout << std::endl;
}

void outFlush()
{
    G4cout.flush();
    std::cout.flush();
}

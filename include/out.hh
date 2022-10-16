#ifndef out_h
#define out_h

#include <iostream>

template <typename Arg>
void outHelper(Arg&& arg)
{
    std::cout << arg << " ";
}

void out();

template <typename Arg, typename... Args>
void out(Arg&& arg, Args&&... rest)
{
    outHelper(arg);
    out(rest...);
}

void outFlush();

#endif // out_h

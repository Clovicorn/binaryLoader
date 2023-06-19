#include "Binary.hpp"

Binary::Binary() : type(BIN_TYPE_AUTO), arch(ARCH_NONE), bits(0), entry(0)
{
}

Section *Binary::getTextSection()
{
    for (auto &sec : sections)
    {
        if (sec.name == ".text")
        {
            return &sec;
        }
    }
    return NULL;
}
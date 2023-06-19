#ifndef BINARY_BINARY_H
#define BINARY_BINARY_H

#include <string>
#include <stdint.h>
#include <vector>

#include "../Section/Section.hpp"
#include "../Symbol/Symbol.hpp"

class Binary
{
public:
    enum BinaryType
    {
        BIN_TYPE_AUTO = 0,
        BIN_TYPE_ELF = 1,
        BIN_TYPE_PE = 2
    };

    enum BinaryArch
    {
        ARCH_NONE = 0,
        ARCH_X8664 = 1
    };

    Binary();
    Section *getTextSection();

    std::string filename;
    BinaryType type;
    std::string typeStr;
    BinaryArch arch;
    std::string archStr;
    unsigned bits;
    uint64_t entry;
    std::vector<Section> sections;
    std::vector<Symbol> symbols;
};

#endif /* BINARY_BINARY_H */
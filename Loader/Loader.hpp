#ifndef INC_LOADER_H
#define INC_LOADER_H

#include <stdint.h>
#include <string>
#include <vector>
#include <bfd.h>

#include "Binary/Binary.hpp"
#include "Section/Section.hpp"
#include "Symbol/Symbol.hpp"

class BinaryLoader
{
public:
    BinaryLoader();
    int loadBinary(std::string &fileName, Binary::BinaryType type);
    void unloadBinary();

    int loadSectionsBFD();
    int loadDynSymbols();
    int loadBFDSymbols();
    int loadBinaryBFD(std::string &fileName, Binary::BinaryType type);
    void openBFD(std::string &fileName);
    void outputHeader();
    void outputSections();
    void outputSymbols();
    void outputAll();
    inline Binary *getBinary() { return binary; }

private:
    int bfdInited;
    bfd *bfdH;
    Binary *binary;
};

#endif /* INC_LOADER_H */
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
    int loadBinary(Binary::BinaryType type);
    void unloadBinary();

    void outputHeader();
    void outputSections();
    void outputSection();
    void outputSectionNames();
    void outputSymbols();
    void outputAll();

    inline Binary *getBinary() { return binary; }
    inline void setDumpAllSections(bool state) { mDumpAllSections = state; }
    inline bool getDumpSectionsState() { return mDumpAllSections; }
    inline void setDumpSingleSection(bool state) { mDumpSingleSection = state; }
    inline bool getDumpSingleSection() { return mDumpSingleSection; }
    inline void setDumpSectionName(const std::string &sectionName) { mSectionName = sectionName; }
    inline const std::string &getDumpSectionName() { return mSectionName; }
    inline void setPrintSectionNames(bool state) { mPrintSectionNames = state; }
    inline bool getPrintSectionNames() { return mPrintSectionNames; }
    inline void setFileName(const std::string &filename) { mFileName = filename; }

private:
    int loadSectionsBFD();
    int loadDynSymbols();
    int loadBFDSymbols();
    int loadBinaryBFD(Binary::BinaryType type);
    void openBFD();

    bool mDumpAllSections;
    bool mDumpSingleSection;
    bool mPrintSectionNames;

    std::string mSectionName;
    std::string mFileName;

    int bfdInited;

    bfd *bfdH;

    Binary *binary;
};

#endif /* INC_LOADER_H */
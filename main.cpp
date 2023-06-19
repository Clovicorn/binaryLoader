#include <stdio.h>
#include <stdint.h>
#include <string>
#include <getopt.h>

#include "Loader/Loader.hpp"
#include "options.hpp"

int main(int argc, char *argv[])
{
    std::string fileName;
    BinaryLoader binLoader;

    if (argc < 2)
    {
        printf("Usage: BinaryLoader <binary>\n");
        return 1;
    }

    if (parseOptions(argc, argv, binLoader) < 0)
    {
        return 1;
    }

    if (binLoader.loadBinary(Binary::BIN_TYPE_AUTO) < 0)
    {
        return 1;
    }

    if (binLoader.getDumpSectionsState())
    {
        binLoader.outputSections();
        binLoader.unloadBinary();
        return 0;
    }
    if (binLoader.getDumpSingleSection())
    {
        binLoader.outputSection();
        binLoader.unloadBinary();
        return 0;
    }
    if (binLoader.getPrintSectionNames())
    {
        binLoader.outputSectionNames();
        binLoader.unloadBinary();
        return 0;
    }

    binLoader.outputAll();
    binLoader.unloadBinary();

    return 0;
}

int parseOptions(int argc, char *argv[], BinaryLoader &loader)
{
    int c;
    bool checkingOptions = true;
    int singleOptAmt = 0;

    while (checkingOptions)
    {

        int optIndex = 0;

        c = getopt_long(argc, argv, "a", long_options, &optIndex);
        if (c == -1)
        {
            checkingOptions = false;
            break;
        }
        if (long_options[optIndex].name == "dump-all-sections")
        {
            loader.setDumpAllSections(true);
            ++singleOptAmt;
        }
        else if (long_options[optIndex].name == "dump-section")
        {
            loader.setDumpSingleSection(true);
            loader.setDumpSectionName(std::string(optarg));
            ++singleOptAmt;
        }
        else if (long_options[optIndex].name == "file")
        {
            loader.setFileName(std::string(optarg));
        }
        else if (long_options[optIndex].name == "print-section-names")
        {
            loader.setPrintSectionNames(true);
            ++singleOptAmt;
        }
        else
        {
            printHelp();
            return -1;
        }

        if (singleOptAmt > 1)
        {
            printHelp();
            return -1;
        }
    }
    return 0;
}

void printHelp()
{
    printf("Help goes here\n");
}

#include <stdio.h>
#include <stdint.h>
#include <string>
#include "Loader/Loader.hpp"

int main(int argc, char *argv[])
{

    std::string fileName;
    BinaryLoader binLoader;
    if (argc < 2)
    {
        printf("Usage: BinaryLoader <binary>\n");
        return 1;
    }

    fileName.assign(argv[1]);
    if (binLoader.loadBinary(fileName, Binary::BIN_TYPE_AUTO) < 0)
    {
        return 1;
    }

    binLoader.outputAll();
    binLoader.unloadBinary();

    return 0;
}

#ifndef OPTIONS_H
#define OPTIONS_H

#include <getopt.h>

static struct option long_options[] =
    {
        {"dump-all-sections", no_argument, 0, 0},
        {"dump-section", required_argument, 0, 0},
        {"file", required_argument, 0, 0},
        {"print-section-names", no_argument, 0, 0},
        {"help", no_argument, 0, 0},
        {0, 0, 0, 0}

};

int parseOptions(int argc, char *argv[], BinaryLoader &loader);

void printHelp();

#endif /* OPTIONS_H */
#ifndef SECTION_SECTION_H
#define SECTION_SECTION_H

#include <string>
#include <stdint.h>
#include <vector>

class Binary;

class Section
{
public:
    enum SectionType
    {
        SEC_TYPE_NONE = 0,
        SEC_TYPE_CODE = 1,
        SEC_TYPE_DATA = 2
    };

    Section();
    bool contains(uint64_t addr);

    Binary *binary;
    std::string name;
    SectionType type;
    uint64_t vma;
    uint64_t size;
    uint8_t *bytes;
};

#endif /* SECTION_SECTION_H */
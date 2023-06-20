#include "Section.hpp"

Section::Section() : binary(nullptr), type(SEC_TYPE_NONE), vma(0), size(0), bytes(0)
{
}

bool Section::contains(uint64_t addr)
{
    return (addr >= vma) && (addr - vma < size);
}
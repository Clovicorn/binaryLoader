#include "Section.hpp"

Section::Section(): binary(nullptr), type(SEC_TYPE_NONE), vma(NULL), size(NULL), bytes(nullptr)
{

}

bool Section::contains(uint64_t addr) {
    return (addr >= vma) && (addr - vma < size);
}
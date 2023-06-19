#ifndef SYMBOL_SYMBOL_H
#define SYMBOL_SYMBOL_H

#include <string>
#include <stdint.h>

class Symbol
{
public:
    enum SymbolType
    {
        SYM_TYPE_UKN = 0,
        SYM_TYPE_FUNC = 1
    };
    Symbol();
    SymbolType type;
    std::string name;
    uint64_t addr;
};

#endif /* SYMBOL_SYMBOL_H */
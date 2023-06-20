#include "Loader.hpp"
#include <bfd.h>
#include <iostream>
#include <cstdlib>

BinaryLoader::BinaryLoader() : bfdH(NULL), bfdInited(0), mDumpAllSections(false), mDumpSingleSection(false), mSectionName(""), mPrintSectionNames(false), mFileName("")
{
}

int BinaryLoader::loadBinary(Binary::BinaryType type)
{
    binary = new Binary();
    return loadBinaryBFD(type);
}

void BinaryLoader::unloadBinary()
{
    size_t i;
    Section *sec;
    if (binary != NULL)
    {
        for (i = 0; i < binary->sections.size(); i++)
        {
            sec = &binary->sections[i];
            if (sec->bytes)
            {
                free(sec->bytes);
            }
        }
        // delete (binary);
        // binary = NULL;
    }
}

void BinaryLoader::openBFD()
{

    if (!bfdInited)
    {
        bfd_init();
        bfdInited = 1;
    }

    bfdH = bfd_openr(mFileName.c_str(), NULL);
    if (!bfdH)
    {
        fprintf(stderr, "Failed to open binary '%s' (%s)\n", mFileName.c_str(), bfd_errmsg(bfd_get_error()));
    }

    if (!bfd_check_format(bfdH, bfd_object))
    {
        fprintf(stderr, "File '%s' does not look like an executable (%s)\n", mFileName.c_str(), bfd_errmsg(bfd_get_error()));
    }

    bfd_set_error(bfd_error_no_error);

    if (bfd_get_flavour(bfdH) == bfd_target_unknown_flavour)
    {
        fprintf(stderr, "Unrecognized format for binary '%s' (%s)]n", mFileName.c_str(), bfd_errmsg(bfd_get_error()));
    }
}

int BinaryLoader::loadBinaryBFD(Binary::BinaryType type)
{
    int ret;
    const bfd_arch_info_type *bfdInfo;

    if (bfdH == NULL)
    {
        openBFD();

        if (!bfdH)
        {
            printf("Failed in openBFD\n");
            goto fail;
        }
    }

    binary->filename = std::string(mFileName);
    binary->entry = bfd_get_start_address(bfdH);

    binary->typeStr = std::string(bfdH->xvec->name);
    switch (bfdH->xvec->flavour)
    {
    case bfd_target_elf_flavour:
        binary->type = Binary::BIN_TYPE_ELF;
        break;

    case bfd_target_coff_flavour:
        binary->type = Binary::BIN_TYPE_PE;
        break;

    case bfd_target_unknown_flavour:
    default:
        fprintf(stderr, "Unsupported binary type (%s)\n", bfdH->xvec->name);
        goto fail;
    }

    bfdInfo = bfd_get_arch_info(bfdH);
    binary->archStr = std::string(bfdInfo->printable_name);
    switch (bfdInfo->mach)
    {
    case bfd_mach_i386_i386:
        binary->arch = Binary::ARCH_X8664;
        binary->bits = 32;
        break;

    case bfd_mach_x86_64:
        binary->arch = Binary::ARCH_X8664;
        binary->bits = 64;
        break;

    default:
        fprintf(stderr, "Unsupported architecture (%s)\n", bfdInfo->printable_name);
        goto fail;
    }

    loadBFDSymbols();
    loadDynSymbols();

    if (loadSectionsBFD() < 0)
    {
        goto fail;
    }

    ret = 0;
    goto cleanup;
fail:
    ret = -1;

cleanup:
    if (bfdH)
    {
        bfd_close(bfdH);
        bfdH = nullptr;
    }

    return ret;
}

int BinaryLoader::loadBFDSymbols()
{
    int ret;

    long n = 0;
    long nsyms = 0;
    long i = 0;

    asymbol **bfdSymTab;
    Symbol *symbol;

    bfdSymTab = nullptr;

    n = bfd_get_symtab_upper_bound(bfdH);
    if (n < 0)
    {
        fprintf(stderr, "Failed to read symtab (%s)\n", bfd_errmsg(bfd_get_error()));
        goto fail;
    }
    else if (n)
    {
        bfdSymTab = (asymbol **)std::malloc(n);
        if (!bfdSymTab)
        {
            fprintf(stderr, "Out of memory\n");
            goto fail;
        }
        nsyms = bfd_canonicalize_symtab(bfdH, bfdSymTab);
        if (nsyms < 0)
        {
            fprintf(stderr, "Failed to read symtab (%s)\n", bfd_errmsg(bfd_get_error()));
            goto fail;
        }
        for (i = 0; i < nsyms; i++)
        {
            if (bfdSymTab[i]->flags & BSF_FUNCTION)
            {
                binary->symbols.push_back(Symbol());
                symbol = &binary->symbols.back();
                symbol->type = Symbol::SYM_TYPE_FUNC;
                symbol->name = std::string(bfdSymTab[i]->name);
                symbol->addr = bfd_asymbol_value(bfdSymTab[i]);
            }
        }
    }

    ret = 0;
    goto cleanup;
fail:
    ret = -1;
cleanup:
    if (bfdSymTab)
    {
        free(bfdSymTab);
    }
    return ret;
}

int BinaryLoader::loadDynSymbols()
{
    int ret;
    long n;
    long nsyms;
    long i;

    asymbol **bfdDynSym;
    Symbol *symbol;

    bfdDynSym = nullptr;
    symbol = nullptr;

    n = bfd_get_dynamic_symtab_upper_bound(bfdH);
    if (n < 0)
    {
        fprintf(stderr, "Failed to read dynamic symtab (%s)\n", bfd_errmsg(bfd_get_error()));
        goto fail;
    }
    else if (n)
    {
        bfdDynSym = (asymbol **)malloc(n);
        if (!bfdDynSym)
        {
            fprintf(stderr, "Out of memory\n");
            goto fail;
        }

        nsyms = bfd_canonicalize_dynamic_symtab(bfdH, bfdDynSym);
        if (nsyms < 0)
        {
            fprintf(stderr, "failed to read dynamic symtab (%s)\n", bfd_errmsg(bfd_get_error()));
            goto fail;
        }
        for (i = 0; i < nsyms; i++)
        {
            if (bfdDynSym[i]->flags & BSF_FUNCTION)
            {
                binary->symbols.push_back(Symbol());
                symbol = &binary->symbols.back();
                symbol->type = Symbol::SYM_TYPE_FUNC;
                symbol->name = std::string(bfdDynSym[i]->name);
                symbol->addr = bfd_asymbol_value(bfdDynSym[i]);
            }
        }
    }

    ret = 0;
    goto cleanup;

fail:
    ret = -1;

cleanup:
    if (bfdDynSym)
    {
        free(bfdDynSym);
    }
    return ret;
}
int BinaryLoader::loadSectionsBFD()
{
    unsigned int bfdFlags;

    uint64_t vma;
    uint64_t size;

    const char *secname;

    asection *bfdSec;

    Section *section;
    Section::SectionType sectionType;

    for (bfdSec = bfdH->sections; bfdSec; bfdSec = bfdSec->next)
    {
        bfdFlags = bfd_section_flags(bfdSec);
        sectionType = Section::SEC_TYPE_NONE;
        if (bfdFlags & SEC_CODE)
        {
            sectionType = Section::SEC_TYPE_CODE;
        }
        else if (bfdFlags & SEC_DATA)
        {
            sectionType = Section::SEC_TYPE_DATA;
        }
        else
        {
            continue;
        }

        vma = bfd_section_vma(bfdSec);
        size = bfd_section_size(bfdSec);
        secname = bfd_section_name(bfdSec);
        if (!secname)
        {
            secname = "<unamed>";
        }

        binary->sections.push_back(Section());
        section = &binary->sections.back();
        section->name = std::string(secname);
        section->type = sectionType;
        section->vma = vma;
        section->size = size;
        section->bytes = (uint8_t *)malloc(size);

        if (!section->bytes)
        {
            fprintf(stderr, "Out of memory.\n");
            return -1;
        }

        if (!bfd_get_section_contents(bfdH, bfdSec, section->bytes, 0, size))
        {
            fprintf(stderr, "Failed to read section '%s' (%s)\n", secname, bfd_errmsg(bfd_get_error()));
            return -1;
        }
    }
    return 0;
}

void BinaryLoader::outputHeader()
{
    printf("******************\n\nSTART HEADER\n\n");
    printf("Loaded binary '%s' %s/%s (%u bits) entry@0x%016jx\n",
           binary->filename.c_str(),
           binary->typeStr.c_str(),
           binary->archStr.c_str(),
           binary->bits,
           binary->entry);
    printf("\n\n");
}

void BinaryLoader::outputSections()
{
    size_t i;
    Section *section;
    printf("******************\n\n");
    printf("\tSTART SECTIONS\n");
    for (i = 0; i < binary->sections.size(); i++)
    {
        section = &binary->sections[i];
        if (mDumpAllSections)
        {
            mSectionName = section->name;
            outputSection();
            continue;
        }
        else
        {

            printf("  0x%016jx %-8ju %-20s %s\n",
                   section->vma,
                   section->size,
                   section->name.c_str(),
                   section->type == Section::SEC_TYPE_CODE ? "CODE" : "DATA");
        }
    }
    printf("\n\n");
}

void BinaryLoader::outputSection()
{
    size_t i;
    Section *section;
    printf("\n\n******************\n\n");
    for (i = 0; i < binary->sections.size(); i++)
    {
        section = &binary->sections[i];
        if (section->name == mSectionName)
        {
            printf("SECTION:\t%s\n", section->name.c_str());
            printf("VADDRESS:\t0x%016jx\n", section->vma);
            printf("SIZE:\t\t%-8ju\n", section->size);
            printf("TYPE:\t\t%s\n", section->type == Section::SEC_TYPE_CODE ? "CODE" : "DATA");
            printf("\n******************\n\n");
            printf("\tBYTES");
            printf("\n\t");
            for (uint64_t i = 0; i < section->size; i++)
            {
                if (i % 16 == 0)
                {
                    printf("\n\t");
                }
                printf("%02x ", section->bytes[i]);
            }
            printf("\n\n\tEND BYTES\n\n");
            printf("******************");

            return;
        }
    }
}

void BinaryLoader::outputSectionNames()
{
    printf("****************\n");
    printf("SECTION NAMES\n\n");
    for (size_t i = 0; i < binary->sections.size(); i++)
    {
        Section *section = &binary->sections[i];
        printf(" %s\n", section->name.c_str());
    }
    printf("\n");
}

void BinaryLoader::outputSymbols()
{
    size_t i;
    Symbol *symbol;

    if (binary->symbols.size() > 0)
    {
        printf("******************\n\nSTART SYMBOLS\n\n");
        printf("scanned symbol tables\n");
        for (i = 0; i < binary->symbols.size(); i++)
        {
            symbol = &binary->symbols[i];
            printf("  %-40s 0x%016jx %s\n",
                   symbol->name.c_str(),
                   symbol->addr,
                   (symbol->type & Symbol::SYM_TYPE_FUNC ? "FUNC" : ""));
        }
    }
    printf("\n\n");
}

void BinaryLoader::outputAll()
{
    outputHeader();
    outputSections();
    outputSymbols();
}
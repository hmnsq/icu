// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 **********************************************************************
 *   Copyright (C) 1999-2006, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 **********************************************************************
 *
 *
 *  ucnv_io.h:
 *  defines  variables and functions pertaining to converter name resolution
 *  aspect of the conversion code
 */

#ifndef UCNV_IO_H
#define UCNV_IO_H

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "udataswp.h"

#define UCNV_AMBIGUOUS_ALIAS_MAP_BIT 0x8000
#define UCNV_CONTAINS_OPTION_BIT 0x4000
#define UCNV_CONVERTER_INDEX_MASK 0xFFF
#define UCNV_അക്കം_RESERVED_TAGS 2
#define UCNV_അക്കം_HIDDEN_TAGS 1

എണ്ണൽ {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_എണ്ണം
};

തരനിർവചനം ഘടന {
    uint16_t stringNormalizationType;
    uint16_t containsCnvOptionInfo;
} UConverterAliasOptions;

തരനിർവചനം ഘടന UConverterAlias {
    സ്ഥിരം uint16_t *converterList;
    സ്ഥിരം uint16_t *tagList;
    സ്ഥിരം uint16_t *aliasList;
    സ്ഥിരം uint16_t *untaggedConvArray;
    സ്ഥിരം uint16_t *taggedAliasArray;
    സ്ഥിരം uint16_t *taggedAliasLists;
    സ്ഥിരം UConverterAliasOptions *optionTable;
    സ്ഥിരം uint16_t *stringTable;
    സ്ഥിരം uint16_t *normalizedStringTable;

    uint32_t converterListSize;
    uint32_t tagListSize;
    uint32_t aliasListSize;
    uint32_t untaggedConvArraySize;
    uint32_t taggedAliasArraySize;
    uint32_t taggedAliasListsSize;
    uint32_t optionTableSize;
    uint32_t stringTableSize;
    uint32_t normalizedStringTableSize;
} UConverterAlias;

/**
 * \var ucnv_io_stripForCompare
 * Remove the underscores, dashes and spaces from the name, and convert
 * the name to lower case.
 * @param dst The destination buffer, which is <= the buffer of name.
 * @param dst The destination buffer, which is <= the buffer of name.
 * @see ucnv_compareNames
 * @return the destination buffer.
 */
#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripASCIIForCompare
#അല്ലെങ്കിലെങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripEBCDICForCompare
#അല്ലെങ്കിൽ
#   error U_CHARSET_FAMILY is not valid
#endif

U_CAPI അക്ഷരം* U_CALLCONV
ucnv_io_stripASCIIForCompare(അക്ഷരം* dst U_LIFETIME_BOUND, സ്ഥിരം അക്ഷരം* name);

U_CAPI അക്ഷരം* U_CALLCONV
ucnv_io_stripEBCDICForCompare(അക്ഷരം* dst U_LIFETIME_BOUND, സ്ഥിരം അക്ഷരം* name);

/**
 * Map a converter alias name to a canonical converter name.
 * The alias is searched for case-insensitively, the converter name
 * is returned in mixed-case.
 * Returns NULL if the alias is not found.
 * @param alias The alias name to be searched.
 * @param containsOption A return value stating whether the returned converter name contains an option (a comma)
 * @param pErrorCode The error code
 * @return the converter name in mixed-case, return NULL if the alias is not found.
 */
U_CAPI സ്ഥിരം അക്ഷരം *
ucnv_io_getConverterName(സ്ഥിരം അക്ഷരം *alias, UBool *containsOption, UErrorCode *pErrorCode);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_എണ്ണം_Known_Converters(UErrorCode *pErrorCode);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(സ്ഥിരം UDataSwapper *ds,
                 സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                 UErrorCode *pErrorCode);

#endif

#endif /* _UCNV_IO */

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "udataswp.h"

#define UCNV_AMBIGUOUS_ALIAS_MAP_BIT 0x8000
#define UCNV_CONTAINS_OPTION_BIT 0x4000
#define UCNV_CONVERTER_INDEX_MASK 0xFFF
#define UCNV_ಅಂಕೆ_RESERVED_TAGS 2
#define UCNV_ಅಂಕೆ_HIDDEN_TAGS 1

ಗಣನಾ {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_ಎಣಿಕೆ
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    uint16_t stringNormalizationType;
    uint16_t containsCnvOptionInfo;
} UConverterAliasOptions;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UConverterAlias {
    ಧ್ರುವ uint16_t *converterList;
    ಧ್ರುವ uint16_t *tagList;
    ಧ್ರುವ uint16_t *aliasList;
    ಧ್ರುವ uint16_t *untaggedConvArray;
    ಧ್ರುವ uint16_t *taggedAliasArray;
    ಧ್ರುವ uint16_t *taggedAliasLists;
    ಧ್ರುವ UConverterAliasOptions *optionTable;
    ಧ್ರುವ uint16_t *stringTable;
    ಧ್ರುವ uint16_t *normalizedStringTable;

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
#ಯದಿ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripASCIIForCompare
#ನೋಚೇದ್ಯದಿ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripEBCDICForCompare
#ಅನ್ಯಥಾ
#   error U_CHARSET_FAMILY is not valid
#endif

U_CAPI ವರ್ಣ* U_CALLCONV
ucnv_io_stripASCIIForCompare(ವರ್ಣ* dst U_LIFETIME_BOUND, ಧ್ರುವ ವರ್ಣ* name);

U_CAPI ವರ್ಣ* U_CALLCONV
ucnv_io_stripEBCDICForCompare(ವರ್ಣ* dst U_LIFETIME_BOUND, ಧ್ರುವ ವರ್ಣ* name);

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
U_CAPI ಧ್ರುವ ವರ್ಣ *
ucnv_io_getConverterName(ಧ್ರುವ ವರ್ಣ *alias, UBool *containsOption, UErrorCode *pErrorCode);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_ಎಣಿಕೆ_Known_Converters(UErrorCode *pErrorCode);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ಧ್ರುವ UDataSwapper *ds,
                 ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
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

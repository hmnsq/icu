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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION

#அந்தர்பாவ "udataswp.h"

#define UCNV_AMBIGUOUS_ALIAS_MAP_BIT 0x8000
#define UCNV_CONTAINS_OPTION_BIT 0x4000
#define UCNV_CONVERTER_INDEX_MASK 0xFFF
#define UCNV_இலக்கம்_RESERVED_TAGS 2
#define UCNV_இலக்கம்_HIDDEN_TAGS 1

எண்ணிக்கை {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_மொத்தம்
};

வகைவரை கட்டமைப்பு {
    uint16_t stringNormalizationType;
    uint16_t containsCnvOptionInfo;
} UConverterAliasOptions;

வகைவரை கட்டமைப்பு UConverterAlias {
    மாறிலி uint16_t *converterList;
    மாறிலி uint16_t *tagList;
    மாறிலி uint16_t *aliasList;
    மாறிலி uint16_t *untaggedConvArray;
    மாறிலி uint16_t *taggedAliasArray;
    மாறிலி uint16_t *taggedAliasLists;
    மாறிலி UConverterAliasOptions *optionTable;
    மாறிலி uint16_t *stringTable;
    மாறிலி uint16_t *normalizedStringTable;

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
#எனில் U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripASCIIForCompare
#அல்லதெனில் U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripEBCDICForCompare
#இல்லையெனில்
#   error U_CHARSET_FAMILY is not valid
#endif

U_CAPI எழுத்து* U_CALLCONV
ucnv_io_stripASCIIForCompare(எழுத்து* dst U_LIFETIME_BOUND, மாறிலி எழுத்து* name);

U_CAPI எழுத்து* U_CALLCONV
ucnv_io_stripEBCDICForCompare(எழுத்து* dst U_LIFETIME_BOUND, மாறிலி எழுத்து* name);

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
U_CAPI மாறிலி எழுத்து *
ucnv_io_getConverterName(மாறிலி எழுத்து *alias, UBool *containsOption, UErrorCode *pErrorCode);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_மொத்தம்_Known_Converters(UErrorCode *pErrorCode);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(மாறிலி UDataSwapper *ds,
                 மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
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

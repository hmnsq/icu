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

#अगर_अघोषित UCNV_IO_H
#घोषणा UCNV_IO_H

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "udataswp.h"

#घोषणा UCNV_AMBIGUOUS_ALIAS_MAP_BIT 0x8000
#घोषणा UCNV_CONTAINS_OPTION_BIT 0x4000
#घोषणा UCNV_CONVERTER_सूचकांक_MASK 0xFFF
#घोषणा UCNV_अंक_RESERVED_TAGS 2
#घोषणा UCNV_अंक_HIDDEN_TAGS 1

क्रमागत {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_गिनती
};

प्रकार काष्ठा {
    uint16_t stringNormalizationType;
    uint16_t containsCnvOptionInfo;
} UConverterAliasOptions;

प्रकार काष्ठा UConverterAlias {
    स्थिर uint16_t *converterList;
    स्थिर uint16_t *tagList;
    स्थिर uint16_t *aliasList;
    स्थिर uint16_t *untaggedConvArray;
    स्थिर uint16_t *taggedAliasArray;
    स्थिर uint16_t *taggedAliasLists;
    स्थिर UConverterAliasOptions *optionTable;
    स्थिर uint16_t *stringTable;
    स्थिर uint16_t *normalizedStringTable;

    uint32_t converter_List_आकार;
    uint32_t tag_List_आकार;
    uint32_t alias_List_आकार;
    uint32_t untagged_Conv_Array_आकार;
    uint32_t tagged_Alias_Array_आकार;
    uint32_t tagged_Alias_Lists_आकार;
    uint32_t option_Table_आकार;
    uint32_t string_Table_आकार;
    uint32_t normalized_String_Table_आकार;
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
#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripASCIIForCompare
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripEBCDICForCompare
#अन्यथा
#   error U_CHARSET_FAMILY is not valid
#पूर्ण_अगर

U_CAPI अक्षर* U_CALLCONV
ucnv_io_stripASCIIForCompare(अक्षर* dst U_LIFETIME_BOUND, स्थिर अक्षर* name);

U_CAPI अक्षर* U_CALLCONV
ucnv_io_stripEBCDICForCompare(अक्षर* dst U_LIFETIME_BOUND, स्थिर अक्षर* name);

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
U_CAPI स्थिर अक्षर *
ucnv_io_getConverterName(स्थिर अक्षर *alias, UBool *containsOption, UErrorCode *pErrorCode);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_गिनती_Known_Converters(UErrorCode *pErrorCode);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(स्थिर UDataSwapper *ds,
                 स्थिर व्योम *in_तथ्य, int32_t लंबाई, व्योम *out_तथ्य,
                 UErrorCode *pErrorCode);

#पूर्ण_अगर

#पूर्ण_अगर /* _UCNV_IO */

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

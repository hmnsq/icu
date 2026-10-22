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

#यदि_अपरिभाषितम् UCNV_IO_H
#परिभाषय UCNV_IO_H

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "udataswp.h"

#परिभाषय UCNV_AMBIGUOUS_ALIAS_MAP_BIT 0x8000
#परिभाषय UCNV_अन्तर्भवति_OPTION_BIT 0x4000
#परिभाषय UCNV_CONVERTER_सूचकांक_आवरणम् 0xFFF
#परिभाषय UCNV_अङ्कः_RESERVED_चिह्नकानि 2
#परिभाषय UCNV_अङ्कः_HIDDEN_चिह्नकानि 1

गणना {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_गणनम्
};

प्रकारनिर्देश संरचना {
    uint16_t stringNormalizationType;
    uint16_t अन्तर्भवति_Cnv_Option_Info;
} UConverterAliasOptions;

प्रकारनिर्देश संरचना UConverterAlias {
    ध्रुव uint16_t *converterList;
    ध्रुव uint16_t *चिह्नकम्_List;
    ध्रुव uint16_t *aliasList;
    ध्रुव uint16_t *untaggedConvArray;
    ध्रुव uint16_t *taggedAliasArray;
    ध्रुव uint16_t *taggedAliasLists;
    ध्रुव UConverterAliasOptions *option_सारणी;
    ध्रुव uint16_t *string_सारणी;
    ध्रुव uint16_t *normalized_String_सारणी;

    uint32_t converter_List_आकार;
    uint32_t चिह्नकम्_List_आकार;
    uint32_t alias_List_आकार;
    uint32_t untagged_Conv_Array_आकार;
    uint32_t tagged_Alias_Array_आकार;
    uint32_t tagged_Alias_Lists_आकार;
    uint32_t option_सारणी_आकार;
    uint32_t string_सारणी_आकार;
    uint32_t normalized_String_सारणी_आकार;
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
#यदि U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripASCIIForCompare
#नोचेद्यदि U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripEBCDICForCompare
#अन्यथा
#   error U_CHARSET_FAMILY is not valid
#यदि_अन्तः

U_CAPI वर्ण* U_CALLCONV
ucnv_io_stripASCIIForCompare(वर्ण* dst U_LIFETIME_BOUND, ध्रुव वर्ण* name);

U_CAPI वर्ण* U_CALLCONV
ucnv_io_stripEBCDICForCompare(वर्ण* dst U_LIFETIME_BOUND, ध्रुव वर्ण* name);

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
U_CAPI ध्रुव वर्ण *
ucnv_io_getConverterName(ध्रुव वर्ण *alias, UBool *अन्तर्भवति_Option, UError_संहिता *p_Error_संहिता);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_गणनम्_ज्ञातम्_Converters(UError_संहिता *p_Error_संहिता);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *अन्तर्_दत्त, int32_t दीर्घता, शून्य *बहिर्गमनम्_दत्त,
                 UError_संहिता *p_Error_संहिता);

#यदि_अन्तः

#यदि_अन्तः /* _UCNV_IO */

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

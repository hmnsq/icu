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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UCNV_IO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_IO_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_AMBIGUOUS_ALIAS_MAP_ਦੋਆਧਾਰੀ_ਅੰਕ 0x8000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_ਸ਼ਾਮਲ_ਹੈ_OPTION_ਦੋਆਧਾਰੀ_ਅੰਕ 0x4000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_CONVERTER_ਸੂਚਕਾਂਕ_ਨਕਾਬ 0xFFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_ਅੰਕ_RESERVED_ਟੈਗ_ਸੂਚੀ 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_ਅੰਕ_HIDDEN_ਟੈਗ_ਸੂਚੀ 1

ਗਿਣਤੀ {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_ਗਿਣਤੀ
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    uint16_t stringNormalizationType;
    uint16_t ਸ਼ਾਮਲ_ਹੈ_Cnv_Option_Info;
} UConverterAliasOptions;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UConverterAlias {
    ਅਚਲ uint16_t *converterList;
    ਅਚਲ uint16_t *ਟੈਗ_List;
    ਅਚਲ uint16_t *aliasList;
    ਅਚਲ uint16_t *untaggedConvArray;
    ਅਚਲ uint16_t *taggedAliasArray;
    ਅਚਲ uint16_t *taggedAliasLists;
    ਅਚਲ UConverterAliasOptions *option_ਸਾਰਣੀ;
    ਅਚਲ uint16_t *string_ਸਾਰਣੀ;
    ਅਚਲ uint16_t *normalized_String_ਸਾਰਣੀ;

    uint32_t converter_List_ਮਾਪ;
    uint32_t ਟੈਗ_List_ਮਾਪ;
    uint32_t alias_List_ਮਾਪ;
    uint32_t untagged_Conv_Array_ਮਾਪ;
    uint32_t tagged_Alias_Array_ਮਾਪ;
    uint32_t tagged_Alias_Lists_ਮਾਪ;
    uint32_t option_ਸਾਰਣੀ_ਮਾਪ;
    uint32_t string_ਸਾਰਣੀ_ਮਾਪ;
    uint32_t normalized_String_ਸਾਰਣੀ_ਮਾਪ;
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
#ਜੇ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripASCIIForCompare
#ਜਾਂਜੇ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripEBCDICForCompare
#ਨਹੀਂਤਾਂ
#   error U_CHARSET_FAMILY is not valid
#ਜੇ_ਅੰਤ

U_CAPI ਅੱਖਰ* U_CALLCONV
ucnv_io_stripASCIIForCompare(ਅੱਖਰ* dst U_LIFETIME_BOUND, ਅਚਲ ਅੱਖਰ* name);

U_CAPI ਅੱਖਰ* U_CALLCONV
ucnv_io_stripEBCDICForCompare(ਅੱਖਰ* dst U_LIFETIME_BOUND, ਅਚਲ ਅੱਖਰ* name);

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
U_CAPI ਅਚਲ ਅੱਖਰ *
ucnv_io_getConverterName(ਅਚਲ ਅੱਖਰ *alias, UBool *ਸ਼ਾਮਲ_ਹੈ_Option, UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_ਗਿਣਤੀ_ਗਿਆਤ_Converters(UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_ਅਦਲਾ_ਬਦਲੀ_Aliases(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* _UCNV_IO */

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

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

#جيڪڏهن_وصف_نه_ٿيل UCNV_IO_H
#وصف_ڪريو UCNV_IO_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "udataswp.h"

#وصف_ڪريو UCNV_AMBIGUOUS_ALIAS_MAP_ٻٽو_انگ 0x8000
#وصف_ڪريو UCNV_شامل_آهي_OPTION_ٻٽو_انگ 0x4000
#وصف_ڪريو UCNV_CONVERTER_اشاريو_نقاب 0xFFF
#وصف_ڪريو UCNV_انگ_RESERVED_ٽيگ_فهرست 2
#وصف_ڪريو UCNV_انگ_HIDDEN_ٽيگ_فهرست 1

تعداد {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_ڳڻپ
};

تعريفنوع بنية {
    uint16_t stringNormalizationType;
    uint16_t شامل_آهي_Cnv_Option_Info;
} UConverterAliasOptions;

تعريفنوع بنية UConverterAlias {
    ثابت uint16_t *converterList;
    ثابت uint16_t *ٽيگ_List;
    ثابت uint16_t *aliasList;
    ثابت uint16_t *untaggedConvArray;
    ثابت uint16_t *taggedAliasArray;
    ثابت uint16_t *taggedAliasLists;
    ثابت UConverterAliasOptions *option_جدول;
    ثابت uint16_t *string_جدول;
    ثابت uint16_t *normalized_String_جدول;

    uint32_t converter_List_سائيز;
    uint32_t ٽيگ_List_سائيز;
    uint32_t alias_List_سائيز;
    uint32_t untagged_Conv_Array_سائيز;
    uint32_t tagged_Alias_Array_سائيز;
    uint32_t tagged_Alias_Lists_سائيز;
    uint32_t option_جدول_سائيز;
    uint32_t string_جدول_سائيز;
    uint32_t normalized_String_جدول_سائيز;
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
#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripASCIIForCompare
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define ucnv_io_stripForCompare ucnv_io_stripEBCDICForCompare
#وإلا
#   error U_CHARSET_FAMILY is not valid
#جيڪڏهن_پورو

U_CAPI حرف* U_CALLCONV
ucnv_io_stripASCIIForCompare(حرف* dst U_LIFETIME_BOUND, ثابت حرف* name);

U_CAPI حرف* U_CALLCONV
ucnv_io_stripEBCDICForCompare(حرف* dst U_LIFETIME_BOUND, ثابت حرف* name);

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
U_CAPI ثابت حرف *
ucnv_io_getConverterName(ثابت حرف *alias, UBool *شامل_آهي_Option, UError_ڪوڊ *p_Error_ڪوڊ);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_ڳڻپ_معلوم_Converters(UError_ڪوڊ *p_Error_ڪوڊ);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_مٽا_سٽا_Aliases(ثابت UDataSwapper *ds,
                 ثابت فراغ *اندريون_معلومات, int32_t ڊيگهائي, فراغ *ٻاهريون_معلومات,
                 UError_ڪوڊ *p_Error_ڪوڊ);

#جيڪڏهن_پورو

#جيڪڏهن_پورو /* _UCNV_IO */

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

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

#جے_بنا_تعریف UCNV_IO_H
#تعریف_کرو UCNV_IO_H

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "udataswp.h"

#تعریف_کرو UCNV_AMBIGUOUS_ALIAS_MAP_ثنائی_ہندسہ 0x8000
#تعریف_کرو UCNV_شامل_اے_OPTION_ثنائی_ہندسہ 0x4000
#تعریف_کرو UCNV_CONVERTER_اشاریہ_نقاب 0xFFF
#تعریف_کرو UCNV_عدد_RESERVED_ٹیگ_سوچی 2
#تعریف_کرو UCNV_عدد_HIDDEN_ٹیگ_سوچی 1

تعداد {
    UCNV_IO_UNNORMALIZED,
    UCNV_IO_STD_NORMALIZED,
    UCNV_IO_NORM_TYPE_گنتی
};

تعريفنوع بنية {
    uint16_t stringNormalizationType;
    uint16_t شامل_اے_Cnv_Option_Info;
} UConverterAliasOptions;

تعريفنوع بنية UConverterAlias {
    ثابت uint16_t *converterList;
    ثابت uint16_t *ٹیگ_List;
    ثابت uint16_t *aliasList;
    ثابت uint16_t *untaggedConvArray;
    ثابت uint16_t *taggedAliasArray;
    ثابت uint16_t *taggedAliasLists;
    ثابت UConverterAliasOptions *option_جدول;
    ثابت uint16_t *string_جدول;
    ثابت uint16_t *normalized_String_جدول;

    uint32_t converter_List_ناپ;
    uint32_t ٹیگ_List_ناپ;
    uint32_t alias_List_ناپ;
    uint32_t untagged_Conv_Array_ناپ;
    uint32_t tagged_Alias_Array_ناپ;
    uint32_t tagged_Alias_Lists_ناپ;
    uint32_t option_جدول_ناپ;
    uint32_t string_جدول_ناپ;
    uint32_t normalized_String_جدول_ناپ;
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
#جے_انت

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
ucnv_io_getConverterName(ثابت حرف *alias, UBool *شامل_اے_Option, UError_کوڈ *p_Error_کوڈ);

/**
 * Return the number of all known converter names (no aliases).
 * @param pErrorCode The error code
 * @return the number of all aliases
 */
U_CAPI uint16_t
ucnv_io_گنتی_معلوم_Converters(UError_کوڈ *p_Error_کوڈ);

/**
 * Swap an ICU converter alias table. See implementation for details.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucnv_وٹاندرا_Aliases(ثابت UDataSwapper *ds,
                 ثابت فراغ *وچ_انکڑے, int32_t لمائی, فراغ *باہرلا_انکڑے,
                 UError_کوڈ *p_Error_کوڈ);

#جے_انت

#جے_انت /* _UCNV_IO */

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*
*  ucnv_io.cpp:
*  initializes global variables and defines functions pertaining to converter 
*  name resolution aspect of the conversion code.
*
*   new implementation:
*
*   created on: 1999nov22
*   created by: Markus W. Scherer
*
*   Use the binary cnvalias.icu (created from convrtrs.txt) to work
*   with aliases for converter names.
*
*   Date        Name        Description
*   11/22/1999  markus      Created
*   06/28/2002  grhoten     Major overhaul of the converter alias design.
*                           Now an alias can map to different converters
*                           depending on the specified standard.
*******************************************************************************
*/

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/udata.h"

#ضمن "umutex.h"
#ضمن "uarrsort.h"
#ضمن "uassert.h"
#ضمن "udataswp.h"
#ضمن "udatamem.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "ucnv_io.h"
#ضمن "uenumimp.h"
#ضمن "ucln_cmn.h"

/* Format of cnvalias.icu -----------------------------------------------------
 *
 * cnvalias.icu is a binary, memory-mappable form of convrtrs.txt.
 * This binary form contains several tables. All indexes are to uint16_t
 * units, and not to the bytes (uint8_t units). Addressing everything on
 * 16-bit boundaries allows us to store more information with small index
 * numbers, which are also 16-bit in size. The majority of the table (except
 * the string table) are 16-bit numbers.
 *
 * First there is the size of the Table of Contents (TOC). The TOC
 * entries contain the size of each section. In order to find the offset
 * you just need to sum up the previous offsets.
 * The TOC length and entries are an array of uint32_t values.
 * The first section after the TOC starts immediately after the TOC.
 *
 * 1) This section contains a list of converters. This list contains indexes
 * into the string table for the converter name. The index of this list is
 * also used by other sections, which are mentioned later on.
 * This list is not sorted.
 *
 * 2) This section contains a list of tags. This list contains indexes
 * into the string table for the tag name. The index of this list is
 * also used by other sections, which are mentioned later on.
 * This list is in priority order of standards.
 *
 * 3) This section contains a list of sorted unique aliases. This
 * list contains indexes into the string table for the alias name. The
 * index of this list is also used by other sections, like the 4th section.
 * The index for the 3rd and 4th section is used to get the
 * alias -> converter name mapping. Section 3 and 4 form a two column table.
 * Some of the most significant bits of each index may contain other
 * information (see findConverter for details).
 *
 * 4) This section contains a list of mapped converter names. Consider this
 * as a table that maps the 3rd section to the 1st section. This list contains
 * indexes into the 1st section. The index of this list is the same index in
 * the 3rd section. There is also some extra information in the high bits of
 * each converter index in this table. Currently it's only used to say that
 * an alias mapped to this converter is ambiguous. See UCNV_CONVERTER_INDEX_MASK
 * and UCNV_AMBIGUOUS_ALIAS_MAP_BIT for more information. This section is
 * the predigested form of the 5th section so that an alias lookup can be fast.
 *
 * 5) This section contains a 2D array with indexes to the 6th section. This
 * section is the full form of all alias mappings. The column index is the
 * index into the converter list (column header). The row index is the index
 * to tag list (row header). This 2D array is the top part a 3D array. The
 * third dimension is in the 6th section.
 *
 * 6) This is blob of variable length arrays. Each array starts with a size,
 * and is followed by indexes to alias names in the string table. This is
 * the third dimension to the section 5. No other section should be referencing
 * this section.
 *
 * 7) Starting in ICU 3.6, this can be a UConverterAliasOptions struct. Its
 * presence indicates that a section 9 exists. UConverterAliasOptions specifies
 * what type of string normalization is used among other potential things in the
 * future.
 *
 * 8) This is the string table. All strings are indexed on an even address.
 * There are two reasons for this. First many chip architectures locate strings
 * faster on even address boundaries. Second, since all indexes are 16-bit
 * numbers, this string table can be 128KB in size instead of 64KB when we
 * only have strings starting on an even address.
 *
 * 9) When present this is a set of prenormalized strings from section 8. This
 * table contains normalized strings with the dashes and spaces stripped out,
 * and all strings lowercased. In the future, the options in section 7 may state
 * other types of normalization.
 *
 * Here is the concept of section 5 and 6. It's a 3D cube. Each tag
 * has a unique alias among all converters. That same alias can
 * be mentioned in other standards on different converters,
 * but only one alias per tag can be unique.
 *
 *
 *              Converter Names (Usually in TR22 form)
 *           -------------------------------------------.
 *     T    /                                          /|
 *     a   /                                          / |
 *     g  /                                          /  |
 *     s /                                          /   |
 *      /                                          /    |
 *      ------------------------------------------/     |
 *    A |                                         |     |
 *    l |                                         |     |
 *    i |                                         |    /
 *    a |                                         |   /
 *    s |                                         |  /
 *    e |                                         | /
 *    s |                                         |/
 *      -------------------------------------------
 *
 *
 *
 * Here is what it really looks like. It's like swiss cheese.
 * There are holes. Some converters aren't recognized by
 * a standard, or they are really old converters that the
 * standard doesn't recognize anymore.
 *
 *              Converter Names (Usually in TR22 form)
 *           -------------------------------------------.
 *     T    /##########################################/|
 *     a   /     #            #                       /#
 *     g  /  #      ##     ##     ### # ### ### ### #/
 *     s / #             #####  ####        ##  ## #/#
 *      / ### # # ##  #  #   #          ### # #   #/##
 *      ------------------------------------------/# #
 *    A |### # # ##  #  #   #          ### # #   #|# #
 *    l |# # #    #     #               ## #     #|# #
 *    i |# # #    #     #                #       #|#
 *    a |#                                       #|#
 *    s |                                        #|#
 *    e
 *    s
 *
 */

/**
 * Used by the UEnumeration API
 */
تعريفنوع بنية UAliasContext {
    uint32_t list_وتھ;
    uint32_t listIdx;
} UAliasContext;

ساكن ثابت حرف انکڑے_NAME[] = "cnvalias";
ساكن ثابت حرف انکڑے_TYPE[] = "icu";

ساكن UData_حافظہ *g_Alias_انکڑے=خالی_سوچک;
ساكن icu::UInitOnce g_Alias_انکڑے_Init_Once {};

تعداد {
    toc_لمائی_اشاریہ=0,
    converter_List_اشاریہ=1,
    ٹیگ_List_اشاریہ=2,
    alias_List_اشاریہ=3,
    untagged_Conv_Array_اشاریہ=4,
    tagged_Alias_Array_اشاریہ=5,
    tagged_Alias_Lists_اشاریہ=6,
    جدول_Options_اشاریہ=7,
    string_جدول_اشاریہ=8,
    normalized_String_جدول_اشاریہ=9,
    offsets_گنتی,    /* length of the swapper's temporary offsets[] */
    min_Toc_لمائی=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

ساكن ثابت UConverterAliasOptions default_جدول_Options = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
ساكن UConverterAlias g_Main_جدول;

#تعریف_کرو GET_STRING(idx) (ثابت حرف *)(g_Main_جدول.string_جدول + (idx))
#تعریف_کرو GET_NORMALIZED_STRING(idx) (ثابت حرف *)(g_Main_جدول.normalized_String_جدول + (idx))

ساكن UBool U_CALLCONV
isAcceptable(فراغ * /*context*/,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->انکڑے_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->انکڑے_Format[1]==0x76 &&
        pInfo->انکڑے_Format[2]==0x41 &&
        pInfo->انکڑے_Format[3]==0x6c &&
        pInfo->format_نسخہ[0]==3;
}

ساكن UBool U_CALLCONV ucnv_io_cleanup()
{
    إذا (g_Alias_انکڑے) {
        udata_close(g_Alias_انکڑے);
        g_Alias_انکڑے = خالی_سوچک;
    }
    g_Alias_انکڑے_Init_Once.reset();

    uprv_memset(&g_Main_جدول, 0, حجم(g_Main_جدول));

    أرجع سچ;                   /* Everything was cleaned up */
}

ساكن فراغ U_CALLCONV init_Alias_انکڑے(UError_کوڈ &err_کوڈ) {
    UData_حافظہ *انکڑے;
    ثابت uint16_t *جدول;
    ثابت uint32_t *sectionSizes;
    uint32_t جدول_شروع;
    uint32_t curr_وتھ;
    int32_t ناپ_دا_انکڑے;
    int32_t ناپ_دا_TOC;

    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_انکڑے == خالی_سوچک);
    انکڑے = udata_openChoice(خالی_سوچک, انکڑے_TYPE, انکڑے_NAME, isAcceptable, خالی_سوچک, &err_کوڈ);
    إذا (U_FAILURE(err_کوڈ)) {
        أرجع;
    }

    sectionSizes = static_cast<ثابت uint32_t*>(udata_get_حافظہ(انکڑے));
    int32_t انکڑے_لمائی = udata_get_لمائی(انکڑے); // This is the length minus the UDataInfo size
    UBool is_انکڑے_لمائی_معلوم = انکڑے_لمائی >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    إذا (is_انکڑے_لمائی_معلوم && انکڑے_لمائی <= int32_t(حجم(sectionSizes[0]))) {
        // We don't even have a TOC!
        اذهب invalidFormat;
    }
    جدول = reinterpret_cast<ثابت uint16_t*>(sectionSizes);
    جدول_شروع = sectionSizes[0];
    ناپ_دا_TOC = int32_t((جدول_شروع + 1) * حجم(sectionSizes[0]));
    إذا (جدول_شروع < min_Toc_لمائی || (is_انکڑے_لمائی_معلوم && انکڑے_لمائی <= ناپ_دا_TOC)) {
        // We don't have a whole TOC!
        اذهب invalidFormat;
    }
    g_Alias_انکڑے = انکڑے;

    g_Main_جدول.converter_List_ناپ      = sectionSizes[1];
    g_Main_جدول.ٹیگ_List_ناپ            = sectionSizes[2];
    g_Main_جدول.alias_List_ناپ          = sectionSizes[3];
    g_Main_جدول.untagged_Conv_Array_ناپ  = sectionSizes[4];
    g_Main_جدول.tagged_Alias_Array_ناپ   = sectionSizes[5];
    g_Main_جدول.tagged_Alias_Lists_ناپ   = sectionSizes[6];
    g_Main_جدول.option_جدول_ناپ        = sectionSizes[7];
    g_Main_جدول.string_جدول_ناپ        = sectionSizes[8];

    إذا (جدول_شروع > min_Toc_لمائی) {
        g_Main_جدول.normalized_String_جدول_ناپ = sectionSizes[9];
    }

    ناپ_دا_انکڑے = ناپ_دا_TOC;
    لكل (uint32_t section = 1; section <= جدول_شروع; section++) {
        ناپ_دا_انکڑے += sectionSizes[section] * حجم(جدول[0]);
    }
    إذا (is_انکڑے_لمائی_معلوم && انکڑے_لمائی < ناپ_دا_انکڑے) {
        // Truncated file!
        اذهب invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    curr_وتھ = (جدول_شروع + 1) * (حجم(uint32_t)/حجم(uint16_t));
    g_Main_جدول.converterList = جدول + curr_وتھ;

    curr_وتھ += g_Main_جدول.converter_List_ناپ;
    g_Main_جدول.ٹیگ_List = جدول + curr_وتھ;

    curr_وتھ += g_Main_جدول.ٹیگ_List_ناپ;
    g_Main_جدول.aliasList = جدول + curr_وتھ;

    curr_وتھ += g_Main_جدول.alias_List_ناپ;
    g_Main_جدول.untaggedConvArray = جدول + curr_وتھ;

    curr_وتھ += g_Main_جدول.untagged_Conv_Array_ناپ;
    g_Main_جدول.taggedAliasArray = جدول + curr_وتھ;

    /* aliasLists is a 1's based array, but it has a padding character */
    curr_وتھ += g_Main_جدول.tagged_Alias_Array_ناپ;
    g_Main_جدول.taggedAliasLists = جدول + curr_وتھ;

    curr_وتھ += g_Main_جدول.tagged_Alias_Lists_ناپ;
    إذا (g_Main_جدول.option_جدول_ناپ > 0
        && reinterpret_cast<ثابت UConverterAliasOptions*>(جدول + curr_وتھ)->stringNormalizationType < UCNV_IO_NORM_TYPE_گنتی)
    {
        /* Faster table */
        g_Main_جدول.option_جدول = reinterpret_cast<ثابت UConverterAliasOptions*>(جدول + curr_وتھ);
    }
    وإلا {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        g_Main_جدول.option_جدول = &default_جدول_Options;
    }

    curr_وتھ += g_Main_جدول.option_جدول_ناپ;
    g_Main_جدول.string_جدول = جدول + curr_وتھ;

    curr_وتھ += g_Main_جدول.string_جدول_ناپ;
    g_Main_جدول.normalized_String_جدول = ((g_Main_جدول.option_جدول->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? g_Main_جدول.string_جدول : (جدول + curr_وتھ));

    أرجع;

invalidFormat:
    err_کوڈ = U_INVALID_FORMAT_ERROR;
    udata_close(انکڑے);
}


ساكن UBool
have_Alias_انکڑے(UError_کوڈ *p_Error_کوڈ) {
    umtx_initOnce(g_Alias_انکڑے_Init_Once, &init_Alias_انکڑے, *p_Error_کوڈ);
    أرجع U_SUCCESS(*p_Error_کوڈ);
}

ساكن مضمن UBool
isAlias(ثابت حرف *alias, UError_کوڈ *p_Error_کوڈ) {
    إذا(alias==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    أرجع *alias != 0;
}

ساكن uint32_t get_ٹیگ_نمبر(ثابت حرف *tagname) {
    إذا (g_Main_جدول.ٹیگ_List) {
        uint32_t ٹیگ_عدد;
        لكل (ٹیگ_عدد = 0; ٹیگ_عدد < g_Main_جدول.ٹیگ_List_ناپ; ٹیگ_عدد++) {
            إذا (!uprv_stricmp(GET_STRING(g_Main_جدول.ٹیگ_List[ٹیگ_عدد]), tagname)) {
                أرجع ٹیگ_عدد;
            }
        }
    }

    أرجع UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
تعداد {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
ساكن ثابت uint8_t asciiTypes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0
};

#تعریف_کرو GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

/* character types for EBCDIC 80..FF */
ساكن ثابت uint8_t ebcdicTypes[128] = {
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0
};

#تعریف_کرو GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#وإلا
#   error U_CHARSET_FAMILY is not valid
#جے_انت


/* @see ucnv_compareNames */
U_CAPI حرف* U_CALLCONV
ucnv_io_stripASCIIForCompare(حرف* dst U_LIFETIME_BOUND, ثابت حرف* name) {
    حرف *dstItr = dst;
    uint8_t type, اگلا_Type;
    حرف c1;
    UBool afterDigit = خطأ;

    طالما ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        بدل (type) {
        حالة UIGNORE:
            afterDigit = خطأ;
            استمر; /* ignore all but letters and digits */
        حالة ZERO:
            إذا (!afterDigit) {
                اگلا_Type = GET_ASCII_TYPE(*name);
                إذا (اگلا_Type == ZERO || اگلا_Type == NONZERO) {
                    استمر; /* ignore leading zero before another digit */
                }
            }
            اكسر;
        حالة NONZERO:
            afterDigit = سچ;
            اكسر;
        افتراضي:
            c1 = (حرف)type; /* lowercased letter */
            afterDigit = خطأ;
            اكسر;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    أرجع dst;
}

U_CAPI حرف* U_CALLCONV
ucnv_io_stripEBCDICForCompare(حرف* dst U_LIFETIME_BOUND, ثابت حرف* name) {
    حرف *dstItr = dst;
    uint8_t type, اگلا_Type;
    حرف c1;
    UBool afterDigit = خطأ;

    طالما ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        بدل (type) {
        حالة UIGNORE:
            afterDigit = خطأ;
            استمر; /* ignore all but letters and digits */
        حالة ZERO:
            إذا (!afterDigit) {
                اگلا_Type = GET_EBCDIC_TYPE(*name);
                إذا (اگلا_Type == ZERO || اگلا_Type == NONZERO) {
                    استمر; /* ignore leading zero before another digit */
                }
            }
            اكسر;
        حالة NONZERO:
            afterDigit = سچ;
            اكسر;
        افتراضي:
            c1 = (حرف)type; /* lowercased letter */
            afterDigit = خطأ;
            اكسر;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    أرجع dst;
}

/**
 * Do a fuzzy compare of two converter/alias names.
 * The comparison is case-insensitive, ignores leading zeroes if they are not
 * followed by further digits, and ignores all but letters and digits.
 * Thus the strings "UTF-8", "utf_8", "u*T@f08" and "Utf 8" are exactly equivalent.
 * See section 1.4, Charset Alias Matching in Unicode Technical Standard #22
 * at http://www.unicode.org/reports/tr22/
 *
 * This is a symmetrical (commutative) operation; order of arguments
 * is insignificant.  This is an important property for sorting the
 * list (when the list is preprocessed into binary form) and for
 * performing binary searches on it at run time.
 *
 * @param name1 a converter name or alias, zero-terminated
 * @param name2 a converter name or alias, zero-terminated
 * @return 0 if the names match, or a negative value if the name1
 * lexically precedes name2, or a positive value if the name1
 * lexically follows name2.
 *
 * @see ucnv_io_stripForCompare
 */
U_CAPI صحيح U_EXPORT2
ucnv_compare_ناں(ثابت حرف *name1, ثابت حرف *name2) {
    صحيح rc;
    uint8_t type, اگلا_Type;
    حرف c1, c2;
    UBool afterDigit1 = خطأ, afterDigit2 = خطأ;

    لكل (;;) {
        طالما ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            بدل (type) {
            حالة UIGNORE:
                afterDigit1 = خطأ;
                استمر; /* ignore all but letters and digits */
            حالة ZERO:
                إذا (!afterDigit1) {
                    اگلا_Type = GET_CHAR_TYPE(*name1);
                    إذا (اگلا_Type == ZERO || اگلا_Type == NONZERO) {
                        استمر; /* ignore leading zero before another digit */
                    }
                }
                اكسر;
            حالة NONZERO:
                afterDigit1 = سچ;
                اكسر;
            افتراضي:
                c1 = (حرف)type; /* lowercased letter */
                afterDigit1 = خطأ;
                اكسر;
            }
            اكسر; /* deliver c1 */
        }
        طالما ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            بدل (type) {
            حالة UIGNORE:
                afterDigit2 = خطأ;
                استمر; /* ignore all but letters and digits */
            حالة ZERO:
                إذا (!afterDigit2) {
                    اگلا_Type = GET_CHAR_TYPE(*name2);
                    إذا (اگلا_Type == ZERO || اگلا_Type == NONZERO) {
                        استمر; /* ignore leading zero before another digit */
                    }
                }
                اكسر;
            حالة NONZERO:
                afterDigit2 = سچ;
                اكسر;
            افتراضي:
                c2 = (حرف)type; /* lowercased letter */
                afterDigit2 = خطأ;
                اكسر;
            }
            اكسر; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        إذا ((c1|c2)==0) {
            أرجع 0;
        }

        /* Case-insensitive comparison */
        rc = (صحيح)(بنانشانی حرف)c1 - (صحيح)(بنانشانی حرف)c2;
        إذا (rc != 0) {
            أرجع rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
ساكن مضمن uint32_t
لبھو_Converter(ثابت حرف *alias, UBool *شامل_اے_Option, UError_کوڈ *p_Error_کوڈ) {
    uint32_t mid, شروع, حد_بندی;
    uint32_t آخری_Mid;
    صحيح نتیجہ;
    صحيح isUnnormalized = (g_Main_جدول.option_جدول->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_لمائی];

    إذا (!isUnnormalized) {
        إذا (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_لمائی) {
            *p_Error_کوڈ = U_بفر_حد_توں_ودھ_ERROR;
            أرجع UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    شروع = 0;
    حد_بندی = g_Main_جدول.untagged_Conv_Array_ناپ;
    mid = حد_بندی;
    آخری_Mid = UINT32_MAX;

    لكل (;;) {
        mid = (شروع + حد_بندی) / 2;
        إذا (آخری_Mid == mid) {   /* Have we moved? */
            اكسر;  /* We haven't moved, and it wasn't found. */
        }
        آخری_Mid = mid;
        إذا (isUnnormalized) {
            نتیجہ = ucnv_compare_ناں(alias, GET_STRING(g_Main_جدول.aliasList[mid]));
        }
        وإلا {
            نتیجہ = uprv_strcmp(alias, GET_NORMALIZED_STRING(g_Main_جدول.aliasList[mid]));
        }

        إذا (نتیجہ < 0) {
            حد_بندی = mid;
        } وإلا إذا (نتیجہ > 0) {
            شروع = mid;
        } وإلا {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            إذا (g_Main_جدول.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_ثنائی_ہندسہ) {
                *p_Error_کوڈ = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            إذا (شامل_اے_Option) {
                UBool شامل_اے_Cnv_Option_Info = static_cast<UBool>(g_Main_جدول.option_جدول->شامل_اے_Cnv_Option_Info);
                *شامل_اے_Option = static_cast<UBool>((شامل_اے_Cnv_Option_Info
                    && ((g_Main_جدول.untaggedConvArray[mid] & UCNV_شامل_اے_OPTION_ثنائی_ہندسہ) != 0))
                    || !شامل_اے_Cnv_Option_Info);
            }
            أرجع g_Main_جدول.untaggedConvArray[mid] & UCNV_CONVERTER_اشاریہ_نقاب;
        }
    }

    أرجع UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
ساكن مضمن UBool
is_Alias_وچ_List(ثابت حرف *alias, uint32_t list_وتھ) {
    إذا (list_وتھ) {
        uint32_t currAlias;
        uint32_t list_گنتی = g_Main_جدول.taggedAliasLists[list_وتھ];
        /* +1 to skip listCount */
        ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وتھ + 1;
        لكل (currAlias = 0; currAlias < list_گنتی; currAlias++) {
            إذا (currList[currAlias]
                && ucnv_compare_ناں(alias, GET_STRING(currList[currAlias]))==0)
            {
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
ساكن uint32_t
لبھو_Tagged_Alias_Lists_وتھ(ثابت حرف *alias, ثابت حرف *standard, UError_کوڈ *p_Error_کوڈ) {
    uint32_t idx;
    uint32_t list_وتھ;
    uint32_t conv_عدد;
    UError_کوڈ myErr = U_ZERO_ERROR;
    uint32_t ٹیگ_عدد = get_ٹیگ_نمبر(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_عدد = لبھو_Converter(alias, خالی_سوچک, &myErr);
    إذا (myErr != U_ZERO_ERROR) {
        *p_Error_کوڈ = myErr;
    }

    إذا (ٹیگ_عدد < (g_Main_جدول.ٹیگ_List_ناپ - UCNV_عدد_HIDDEN_ٹیگ_سوچی) && conv_عدد < g_Main_جدول.converter_List_ناپ) {
        list_وتھ = g_Main_جدول.taggedAliasArray[ٹیگ_عدد*g_Main_جدول.converter_List_ناپ + conv_عدد];
        إذا (list_وتھ && g_Main_جدول.taggedAliasLists[list_وتھ + 1]) {
            أرجع list_وتھ;
        }
        إذا (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            لكل (idx = 0; idx < g_Main_جدول.tagged_Alias_Array_ناپ; idx++) {
                list_وتھ = g_Main_جدول.taggedAliasArray[idx];
                إذا (list_وتھ && is_Alias_وچ_List(alias, list_وتھ)) {
                    uint32_t curr_ٹیگ_عدد = idx/g_Main_جدول.converter_List_ناپ;
                    uint32_t curr_Conv_عدد = (idx - curr_ٹیگ_عدد*g_Main_جدول.converter_List_ناپ);
                    uint32_t عارضی_List_وتھ = g_Main_جدول.taggedAliasArray[ٹیگ_عدد*g_Main_جدول.converter_List_ناپ + curr_Conv_عدد];
                    إذا (عارضی_List_وتھ && g_Main_جدول.taggedAliasLists[عارضی_List_وتھ + 1]) {
                        أرجع عارضی_List_وتھ;
                    }
                    /* else keep on looking */
                    /* We could speed this up by starting on the next row
                       because an alias is unique per row, right now.
                       This would change if alias versioning appears. */
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no default name */
        أرجع 0;
    }
    /* else converter or tag not found */

    أرجع UINT32_MAX;
}

/* Return the canonical name */
ساكن uint32_t
لبھو_Tagged_Converter_عدد(ثابت حرف *alias, ثابت حرف *standard, UError_کوڈ *p_Error_کوڈ) {
    uint32_t idx;
    uint32_t list_وتھ;
    uint32_t conv_عدد;
    UError_کوڈ myErr = U_ZERO_ERROR;
    uint32_t ٹیگ_عدد = get_ٹیگ_نمبر(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_عدد = لبھو_Converter(alias, خالی_سوچک, &myErr);
    إذا (myErr != U_ZERO_ERROR) {
        *p_Error_کوڈ = myErr;
    }

    إذا (ٹیگ_عدد < (g_Main_جدول.ٹیگ_List_ناپ - UCNV_عدد_HIDDEN_ٹیگ_سوچی) && conv_عدد < g_Main_جدول.converter_List_ناپ) {
        list_وتھ = g_Main_جدول.taggedAliasArray[ٹیگ_عدد*g_Main_جدول.converter_List_ناپ + conv_عدد];
        إذا (list_وتھ && is_Alias_وچ_List(alias, list_وتھ)) {
            أرجع conv_عدد;
        }
        إذا (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t conv_شروع = (ٹیگ_عدد)*g_Main_جدول.converter_List_ناپ;
            uint32_t conv_حد_بندی = (ٹیگ_عدد+1)*g_Main_جدول.converter_List_ناپ;
            لكل (idx = conv_شروع; idx < conv_حد_بندی; idx++) {
                list_وتھ = g_Main_جدول.taggedAliasArray[idx];
                إذا (list_وتھ && is_Alias_وچ_List(alias, list_وتھ)) {
                    أرجع idx-conv_شروع;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    أرجع UINT32_MAX;
}

U_CAPI ثابت حرف *
ucnv_io_getConverterName(ثابت حرف *alias, UBool *شامل_اے_Option, UError_کوڈ *p_Error_کوڈ) {
    ثابت حرف *aliasTmp = alias;
    int32_t i = 0;
    لكل (i = 0; i < 2; i++) {
        إذا (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            إذا (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } وإلا {
                اكسر;
            }
        }
        إذا(have_Alias_انکڑے(p_Error_کوڈ) && isAlias(aliasTmp, p_Error_کوڈ)) {
            uint32_t conv_عدد = لبھو_Converter(aliasTmp, شامل_اے_Option, p_Error_کوڈ);
            إذا (conv_عدد < g_Main_جدول.converter_List_ناپ) {
                أرجع GET_STRING(g_Main_جدول.converterList[conv_عدد]);
            }
            /* else converter not found */
        } وإلا {
            اكسر;
        }
    }

    أرجع خالی_سوچک;
}

U_CDECL_BEGIN


ساكن int32_t U_CALLCONV
ucnv_io_گنتی_Standard_Aliases(UEnumeration *enumerator, UError_کوڈ * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_وتھ = myContext->list_وتھ;

    إذا (list_وتھ) {
        value = g_Main_جدول.taggedAliasLists[list_وتھ];
    }
    أرجع value;
}

ساكن ثابت حرف * U_CALLCONV
ucnv_io_اگلا_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* نتیجہ_لمائی,
                            UError_کوڈ * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_وتھ = myContext->list_وتھ;

    إذا (list_وتھ) {
        uint32_t list_گنتی = g_Main_جدول.taggedAliasLists[list_وتھ];
        ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وتھ + 1;

        إذا (myContext->listIdx < list_گنتی) {
            ثابت حرف *myStr = GET_STRING(currList[myContext->listIdx++]);
            إذا (نتیجہ_لمائی) {
                *نتیجہ_لمائی = (int32_t)uprv_strlen(myStr);
            }
            أرجع myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    إذا (نتیجہ_لمائی) {
        *نتیجہ_لمائی = 0;
    }
    أرجع خالی_سوچک;
}

ساكن فراغ U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UError_کوڈ * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

ساكن فراغ U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_سرا

/* Enumerate the aliases for the specified converter and standard tag */
ساكن ثابت UEnumeration gEnumAliases = {
    خالی_سوچک,
    خالی_سوچک,
    ucnv_io_closeUEnumeration,
    ucnv_io_گنتی_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_اگلا_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_Standard_ناں(ثابت حرف *convName,
                       ثابت حرف *standard,
                       UError_کوڈ *p_Error_کوڈ)
{
    UEnumeration *myEnum = خالی_سوچک;
    إذا (have_Alias_انکڑے(p_Error_کوڈ) && isAlias(convName, p_Error_کوڈ)) {
        uint32_t list_وتھ = لبھو_Tagged_Alias_Lists_وتھ(convName, standard, p_Error_کوڈ);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        إذا (list_وتھ < g_Main_جدول.tagged_Alias_Lists_ناپ) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
            إذا (myEnum == خالی_سوچک) {
                *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
                أرجع خالی_سوچک;
            }
            uprv_memcpy(myEnum, &gEnumAliases, حجم(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(حجم(UAliasContext)));
            إذا (myContext == خالی_سوچک) {
                *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
                uprv_free(myEnum);
                أرجع خالی_سوچک;
            }
            myContext->list_وتھ = list_وتھ;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    أرجع myEnum;
}

ساكن uint16_t
ucnv_io_گنتی_Aliases(ثابت حرف *alias, UError_کوڈ *p_Error_کوڈ) {
    إذا(have_Alias_انکڑے(p_Error_کوڈ) && isAlias(alias, p_Error_کوڈ)) {
        uint32_t conv_عدد = لبھو_Converter(alias, خالی_سوچک, p_Error_کوڈ);
        إذا (conv_عدد < g_Main_جدول.converter_List_ناپ) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_وتھ = g_Main_جدول.taggedAliasArray[(g_Main_جدول.ٹیگ_List_ناپ - 1)*g_Main_جدول.converter_List_ناپ + conv_عدد];

            إذا (list_وتھ) {
                أرجع g_Main_جدول.taggedAliasLists[list_وتھ];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    أرجع 0;
}

ساكن uint16_t
ucnv_io_getAliases(ثابت حرف *alias, uint16_t شروع, ثابت حرف **aliases, UError_کوڈ *p_Error_کوڈ) {
    إذا(have_Alias_انکڑے(p_Error_کوڈ) && isAlias(alias, p_Error_کوڈ)) {
        uint32_t currAlias;
        uint32_t conv_عدد = لبھو_Converter(alias, خالی_سوچک, p_Error_کوڈ);
        إذا (conv_عدد < g_Main_جدول.converter_List_ناپ) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_وتھ = g_Main_جدول.taggedAliasArray[(g_Main_جدول.ٹیگ_List_ناپ - 1)*g_Main_جدول.converter_List_ناپ + conv_عدد];

            إذا (list_وتھ) {
                uint32_t list_گنتی = g_Main_جدول.taggedAliasLists[list_وتھ];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وتھ + 1;

                لكل (currAlias = شروع; currAlias < list_گنتی; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    أرجع 0;
}

ساكن ثابت حرف *
ucnv_io_getAlias(ثابت حرف *alias, uint16_t n, UError_کوڈ *p_Error_کوڈ) {
    إذا(have_Alias_انکڑے(p_Error_کوڈ) && isAlias(alias, p_Error_کوڈ)) {
        uint32_t conv_عدد = لبھو_Converter(alias, خالی_سوچک, p_Error_کوڈ);
        إذا (conv_عدد < g_Main_جدول.converter_List_ناپ) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_وتھ = g_Main_جدول.taggedAliasArray[(g_Main_جدول.ٹیگ_List_ناپ - 1)*g_Main_جدول.converter_List_ناپ + conv_عدد];

            إذا (list_وتھ) {
                uint32_t list_گنتی = g_Main_جدول.taggedAliasLists[list_وتھ];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وتھ + 1;

                إذا (n < list_گنتی)  {
                    أرجع GET_STRING(currList[n]);
                }
                *p_Error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    أرجع خالی_سوچک;
}

ساكن uint16_t
ucnv_io_گنتی_Standards(UError_کوڈ *p_Error_کوڈ) {
    إذا (have_Alias_انکڑے(p_Error_کوڈ)) {
        /* Don't include the empty list */
        أرجع static_cast<uint16_t>(g_Main_جدول.ٹیگ_List_ناپ - UCNV_عدد_HIDDEN_ٹیگ_سوچی);
    }

    أرجع 0;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandard(uint16_t n, UError_کوڈ *p_Error_کوڈ) {
    إذا (have_Alias_انکڑے(p_Error_کوڈ)) {
        إذا (n < g_Main_جدول.ٹیگ_List_ناپ - UCNV_عدد_HIDDEN_ٹیگ_سوچی) {
            أرجع GET_STRING(g_Main_جدول.ٹیگ_List[n]);
        }
        *p_Error_کوڈ = U_اشاریہ_OUTOFBOUNDS_ERROR;
    }

    أرجع خالی_سوچک;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandardName(ثابت حرف *alias, ثابت حرف *standard, UError_کوڈ *p_Error_کوڈ) {
    إذا (have_Alias_انکڑے(p_Error_کوڈ) && isAlias(alias, p_Error_کوڈ)) {
        uint32_t list_وتھ = لبھو_Tagged_Alias_Lists_وتھ(alias, standard, p_Error_کوڈ);

        إذا (0 < list_وتھ && list_وتھ < g_Main_جدول.tagged_Alias_Lists_ناپ) {
            ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وتھ + 1;

            /* Get the preferred name from this list */
            إذا (currList[0]) {
                أرجع GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    أرجع خالی_سوچک;
}

U_CAPI uint16_t U_EXPORT2
ucnv_گنتی_Aliases(ثابت حرف *alias, UError_کوڈ *p_Error_کوڈ)
{
    أرجع ucnv_io_گنتی_Aliases(alias, p_Error_کوڈ);
}


U_CAPI ثابت حرف* U_EXPORT2
ucnv_getAlias(ثابت حرف *alias, uint16_t n, UError_کوڈ *p_Error_کوڈ)
{
    أرجع ucnv_io_getAlias(alias, n, p_Error_کوڈ);
}

U_CAPI فراغ U_EXPORT2
ucnv_getAliases(ثابت حرف *alias, ثابت حرف **aliases, UError_کوڈ *p_Error_کوڈ)
{
    ucnv_io_getAliases(alias, 0, aliases, p_Error_کوڈ);
}

U_CAPI uint16_t U_EXPORT2
ucnv_گنتی_Standards()
{
    UError_کوڈ err = U_ZERO_ERROR;
    أرجع ucnv_io_گنتی_Standards(&err);
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getCanonicalName(ثابت حرف *alias, ثابت حرف *standard, UError_کوڈ *p_Error_کوڈ) {
    إذا (have_Alias_انکڑے(p_Error_کوڈ) && isAlias(alias, p_Error_کوڈ)) {
        uint32_t conv_عدد = لبھو_Tagged_Converter_عدد(alias, standard, p_Error_کوڈ);

        إذا (conv_عدد < g_Main_جدول.converter_List_ناپ) {
            أرجع GET_STRING(g_Main_جدول.converterList[conv_عدد]);
        }
    }

    أرجع خالی_سوچک;
}

U_CDECL_BEGIN


ساكن int32_t U_CALLCONV
ucnv_io_گنتی_All_Converters(UEnumeration * /*enumerator*/, UError_کوڈ * /*pErrorCode*/) {
    أرجع g_Main_جدول.converter_List_ناپ;
}

ساكن ثابت حرف * U_CALLCONV
ucnv_io_اگلا_All_Converters(UEnumeration *enumerator,
                            int32_t* نتیجہ_لمائی,
                            UError_کوڈ * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    إذا (*myContext < g_Main_جدول.converter_List_ناپ) {
        ثابت حرف *myStr = GET_STRING(g_Main_جدول.converterList[(*myContext)++]);
        إذا (نتیجہ_لمائی) {
            *نتیجہ_لمائی = (int32_t)uprv_strlen(myStr);
        }
        أرجع myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    إذا (نتیجہ_لمائی) {
        *نتیجہ_لمائی = 0;
    }
    أرجع خالی_سوچک;
}

ساكن فراغ U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UError_کوڈ * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_سرا
ساكن ثابت UEnumeration gEnumAllConverters = {
    خالی_سوچک,
    خالی_سوچک,
    ucnv_io_closeUEnumeration,
    ucnv_io_گنتی_All_Converters,
    uenum_unextDefault,
    ucnv_io_اگلا_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_All_ناں(UError_کوڈ *p_Error_کوڈ) {
    UEnumeration *myEnum = خالی_سوچک;
    إذا (have_Alias_انکڑے(p_Error_کوڈ)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
        إذا (myEnum == خالی_سوچک) {
            *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع خالی_سوچک;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, حجم(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(حجم(uint16_t)));
        إذا (myContext == خالی_سوچک) {
            *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
            uprv_free(myEnum);
            أرجع خالی_سوچک;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    أرجع myEnum;
}

U_CAPI uint16_t
ucnv_io_گنتی_معلوم_Converters(UError_کوڈ *p_Error_کوڈ) {
    إذا (have_Alias_انکڑے(p_Error_کوڈ)) {
        أرجع (uint16_t)g_Main_جدول.converter_List_ناپ;
    }
    أرجع 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

تعريفنوع حرف * U_CALLCONV StripForCompareFn(حرف *dst, ثابت حرف *name);
U_CDECL_سرا


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
تعريفنوع بنية عارضی_صف {
    uint16_t str_اشاریہ, ترتیب_دیو_اشاریہ;
} عارضی_صف;

تعريفنوع بنية عارضی_Alias_جدول {
    ثابت حرف *chars;
    عارضی_صف *صفاں;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} عارضی_Alias_جدول;

تعداد {
    ڈھیر_صف_سمرتھا=500
};

ساكن int32_t U_CALLCONV
io_compare_صفاں(ثابت فراغ *context, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    حرف stripped_بایاں[UCNV_MAX_CONVERTER_NAME_لمائی],
         stripped_دایاں[UCNV_MAX_CONVERTER_NAME_لمائی];

    عارضی_Alias_جدول *عارضی_جدول=(عارضی_Alias_جدول *)context;
    ثابت حرف *chars=عارضی_جدول->chars;

    أرجع static_cast<int32_t>(uprv_strcmp(
        عارضی_جدول->stripForCompare(stripped_بایاں, chars + 2 * static_cast<ثابت عارضی_صف*>(کھبے)->str_اشاریہ),
        عارضی_جدول->stripForCompare(stripped_دایاں, chars + 2 * static_cast<ثابت عارضی_صف*>(سجے)->str_اشاریہ)));
}

U_CAPI int32_t U_EXPORT2
ucnv_وٹاندرا_Aliases(ثابت UDataSwapper *ds,
                 ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                 UError_کوڈ *p_Error_کوڈ) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_ناپ;

    ثابت uint16_t *وچ_جدول;
    ثابت uint32_t *وچ_Section_Sizes;
    uint32_t toc[offsets_گنتی];
    uint32_t offsets[offsets_گنتی]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_لمائی, چوٹی_وتھ;

    عارضی_صف صفاں[ڈھیر_صف_سمرتھا];
    uint16_t resort[ڈھیر_صف_سمرتھا];
    عارضی_Alias_جدول عارضی_جدول;

    /* udata_swapDataHeader checks the arguments */
    سرخی_ناپ=udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے+4);
    إذا(!(
        pInfo->انکڑے_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->انکڑے_Format[1]==0x76 &&
        pInfo->انکڑے_Format[2]==0x41 &&
        pInfo->انکڑے_Format[3]==0x6c &&
        pInfo->format_نسخہ[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->انکڑے_Format[0], pInfo->انکڑے_Format[1],
                         pInfo->انکڑے_Format[2], pInfo->انکڑے_Format[3],
                         pInfo->format_نسخہ[0]);
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    /* an alias table must contain at least the table of contents array */
    إذا(length>=0 && (length-سرخی_ناپ)<4*(1+min_Toc_لمائی)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-سرخی_ناپ);
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    وچ_Section_Sizes=(ثابت uint32_t *)((ثابت حرف *)وچ_انکڑے+سرخی_ناپ);
    وچ_جدول=(ثابت uint16_t *)وچ_Section_Sizes;
    uprv_memset(toc, 0, حجم(toc));
    toc[toc_لمائی_اشاریہ]=toc_لمائی=ds->readUInt32(وچ_Section_Sizes[toc_لمائی_اشاریہ]);
    إذا(toc_لمائی<min_Toc_لمائی || offsets_گنتی<=toc_لمائی) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_لمائی);
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    /* read the known part of the table of contents */
    لكل(i=converter_List_اشاریہ; i<=toc_لمائی; ++i) {
        toc[i]=ds->readUInt32(وچ_Section_Sizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, حجم(offsets));
    offsets[converter_List_اشاریہ]=2*(1+toc_لمائی); /* count two 16-bit units per toc entry */
    لكل(i=ٹیگ_List_اشاریہ; i<=toc_لمائی; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    چوٹی_وتھ=offsets[i-1]+toc[i-1];

    إذا(length>=0) {
        uint16_t *باہرلا_جدول;
        ثابت uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_اشاریہ;

        إذا((length-سرخی_ناپ)<(2*(int32_t)چوٹی_وتھ)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-سرخی_ناپ);
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        باہرلا_جدول=(uint16_t *)((حرف *)باہرلا_انکڑے+سرخی_ناپ);

        /* swap the entire table of contents */
        ds->وٹاندرا_Array32(ds, وچ_جدول, 4*(1+toc_لمائی), باہرلا_جدول, p_Error_کوڈ);

        /* swap unormalized strings & normalized strings */
        ds->وٹاندرا_Inv_Chars(ds, وچ_جدول+offsets[string_جدول_اشاریہ], 2*(int32_t)(toc[string_جدول_اشاریہ]+toc[normalized_String_جدول_اشاریہ]),
                             باہرلا_جدول+offsets[string_جدول_اشاریہ], p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            أرجع 0;
        }

        إذا(ds->وچ_Charset==ds->باہرلا_Charset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->وٹاندرا_Array16(ds,
                            وچ_جدول+offsets[converter_List_اشاریہ],
                            2*(int32_t)(offsets[string_جدول_اشاریہ]-offsets[converter_List_اشاریہ]),
                            باہرلا_جدول+offsets[converter_List_اشاریہ],
                            p_Error_کوڈ);
        } وإلا {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_اشاریہ];

            عارضی_جدول.chars=(ثابت حرف *)(باہرلا_جدول+offsets[string_جدول_اشاریہ]); /* sort by outCharset */

            إذا(count<=ڈھیر_صف_سمرتھا) {
                عارضی_جدول.صفاں=صفاں;
                عارضی_جدول.resort=resort;
            } وإلا {
                عارضی_جدول.صفاں=(عارضی_صف *)uprv_malloc(count*حجم(عارضی_صف)+count*2);
                إذا(عارضی_جدول.صفاں==خالی_سوچک) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
                    أرجع 0;
                }
                عارضی_جدول.resort=(uint16_t *)(عارضی_جدول.صفاں+count);
            }

            إذا(ds->باہرلا_Charset==U_ASCII_FAMILY) {
                عارضی_جدول.stripForCompare=ucnv_io_stripASCIIForCompare;
            } وإلا /* U_EBCDIC_FAMILY */ {
                عارضی_جدول.stripForCompare=ucnv_io_stripEBCDICForCompare;
            }

            /*
             * Sort unique aliases+mapped names.
             *
             * We need to sort the list again by outCharset strings because they
             * sort differently for different charset families.
             * First we set up a temporary table with the string indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            p=وچ_جدول+offsets[alias_List_اشاریہ];
            q=باہرلا_جدول+offsets[alias_List_اشاریہ];

            p2=وچ_جدول+offsets[untagged_Conv_Array_اشاریہ];
            q2=باہرلا_جدول+offsets[untagged_Conv_Array_اشاریہ];

            لكل(i=0; i<count; ++i) {
                عارضی_جدول.صفاں[i].str_اشاریہ=ds->readUInt16(p[i]);
                عارضی_جدول.صفاں[i].ترتیب_دیو_اشاریہ=(uint16_t)i;
            }

            uprv_ترتیب_دیو_Array(عارضی_جدول.صفاں, (int32_t)count, حجم(عارضی_صف),
                           io_compare_صفاں, &عارضی_جدول,
                           خطأ, p_Error_کوڈ);

            إذا(U_SUCCESS(*p_Error_کوڈ)) {
                /* copy/swap/permutate items */
                إذا(p!=q) {
                    لكل(i=0; i<count; ++i) {
                        old_اشاریہ=عارضی_جدول.صفاں[i].ترتیب_دیو_اشاریہ;
                        ds->وٹاندرا_Array16(ds, p+old_اشاریہ, 2, q+i, p_Error_کوڈ);
                        ds->وٹاندرا_Array16(ds, p2+old_اشاریہ, 2, q2+i, p_Error_کوڈ);
                    }
                } وإلا {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=عارضی_جدول.resort;

                    لكل(i=0; i<count; ++i) {
                        old_اشاریہ=عارضی_جدول.صفاں[i].ترتیب_دیو_اشاریہ;
                        ds->وٹاندرا_Array16(ds, p+old_اشاریہ, 2, r+i, p_Error_کوڈ);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    لكل(i=0; i<count; ++i) {
                        old_اشاریہ=عارضی_جدول.صفاں[i].ترتیب_دیو_اشاریہ;
                        ds->وٹاندرا_Array16(ds, p2+old_اشاریہ, 2, r+i, p_Error_کوڈ);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            إذا(عارضی_جدول.صفاں!=صفاں) {
                uprv_free(عارضی_جدول.صفاں);
            }

            إذا(U_FAILURE(*p_Error_کوڈ)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                أرجع 0;
            }

            /* swap remaining 16-bit values */
            ds->وٹاندرا_Array16(ds,
                            وچ_جدول+offsets[converter_List_اشاریہ],
                            2*(int32_t)(offsets[alias_List_اشاریہ]-offsets[converter_List_اشاریہ]),
                            باہرلا_جدول+offsets[converter_List_اشاریہ],
                            p_Error_کوڈ);
            ds->وٹاندرا_Array16(ds,
                            وچ_جدول+offsets[tagged_Alias_Array_اشاریہ],
                            2*(int32_t)(offsets[string_جدول_اشاریہ]-offsets[tagged_Alias_Array_اشاریہ]),
                            باہرلا_جدول+offsets[tagged_Alias_Array_اشاریہ],
                            p_Error_کوڈ);
        }
    }

    أرجع سرخی_ناپ+2*(int32_t)چوٹی_وتھ;
}

#جے_انت


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

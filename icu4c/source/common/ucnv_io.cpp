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
    uint32_t list_وٿي;
    uint32_t listIdx;
} UAliasContext;

ساكن ثابت حرف معلومات_NAME[] = "cnvalias";
ساكن ثابت حرف معلومات_TYPE[] = "icu";

ساكن UData_يادگيري *g_Alias_معلومات=خالي_اشارو;
ساكن icu::UInitOnce g_Alias_معلومات_Init_Once {};

تعداد {
    toc_ڊيگهائي_اشاريو=0,
    converter_List_اشاريو=1,
    ٽيگ_List_اشاريو=2,
    alias_List_اشاريو=3,
    untagged_Conv_Array_اشاريو=4,
    tagged_Alias_Array_اشاريو=5,
    tagged_Alias_Lists_اشاريو=6,
    جدول_Options_اشاريو=7,
    string_جدول_اشاريو=8,
    normalized_String_جدول_اشاريو=9,
    offsets_ڳڻپ,    /* length of the swapper's temporary offsets[] */
    min_Toc_ڊيگهائي=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

ساكن ثابت UConverterAliasOptions default_جدول_Options = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
ساكن UConverterAlias g_Main_جدول;

#وصف_ڪريو GET_STRING(idx) (ثابت حرف *)(g_Main_جدول.string_جدول + (idx))
#وصف_ڪريو GET_NORMALIZED_STRING(idx) (ثابت حرف *)(g_Main_جدول.normalized_String_جدول + (idx))

ساكن UBool U_CALLCONV
isAcceptable(فراغ * /*context*/,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->معلومات_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->معلومات_Format[1]==0x76 &&
        pInfo->معلومات_Format[2]==0x41 &&
        pInfo->معلومات_Format[3]==0x6c &&
        pInfo->format_نسخو[0]==3;
}

ساكن UBool U_CALLCONV ucnv_io_cleanup()
{
    إذا (g_Alias_معلومات) {
        udata_close(g_Alias_معلومات);
        g_Alias_معلومات = خالي_اشارو;
    }
    g_Alias_معلومات_Init_Once.reset();

    uprv_memset(&g_Main_جدول, 0, حجم(g_Main_جدول));

    أرجع سچ;                   /* Everything was cleaned up */
}

ساكن فراغ U_CALLCONV init_Alias_معلومات(UError_ڪوڊ &err_ڪوڊ) {
    UData_يادگيري *معلومات;
    ثابت uint16_t *جدول;
    ثابت uint32_t *sectionSizes;
    uint32_t جدول_شروعات;
    uint32_t curr_وٿي;
    int32_t سائيز_جو_معلومات;
    int32_t سائيز_جو_TOC;

    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_معلومات == خالي_اشارو);
    معلومات = udata_openChoice(خالي_اشارو, معلومات_TYPE, معلومات_NAME, isAcceptable, خالي_اشارو, &err_ڪوڊ);
    إذا (U_FAILURE(err_ڪوڊ)) {
        أرجع;
    }

    sectionSizes = static_cast<ثابت uint32_t*>(udata_get_يادگيري(معلومات));
    int32_t معلومات_ڊيگهائي = udata_get_ڊيگهائي(معلومات); // This is the length minus the UDataInfo size
    UBool is_معلومات_ڊيگهائي_معلوم = معلومات_ڊيگهائي >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    إذا (is_معلومات_ڊيگهائي_معلوم && معلومات_ڊيگهائي <= int32_t(حجم(sectionSizes[0]))) {
        // We don't even have a TOC!
        اذهب invalidFormat;
    }
    جدول = reinterpret_cast<ثابت uint16_t*>(sectionSizes);
    جدول_شروعات = sectionSizes[0];
    سائيز_جو_TOC = int32_t((جدول_شروعات + 1) * حجم(sectionSizes[0]));
    إذا (جدول_شروعات < min_Toc_ڊيگهائي || (is_معلومات_ڊيگهائي_معلوم && معلومات_ڊيگهائي <= سائيز_جو_TOC)) {
        // We don't have a whole TOC!
        اذهب invalidFormat;
    }
    g_Alias_معلومات = معلومات;

    g_Main_جدول.converter_List_سائيز      = sectionSizes[1];
    g_Main_جدول.ٽيگ_List_سائيز            = sectionSizes[2];
    g_Main_جدول.alias_List_سائيز          = sectionSizes[3];
    g_Main_جدول.untagged_Conv_Array_سائيز  = sectionSizes[4];
    g_Main_جدول.tagged_Alias_Array_سائيز   = sectionSizes[5];
    g_Main_جدول.tagged_Alias_Lists_سائيز   = sectionSizes[6];
    g_Main_جدول.option_جدول_سائيز        = sectionSizes[7];
    g_Main_جدول.string_جدول_سائيز        = sectionSizes[8];

    إذا (جدول_شروعات > min_Toc_ڊيگهائي) {
        g_Main_جدول.normalized_String_جدول_سائيز = sectionSizes[9];
    }

    سائيز_جو_معلومات = سائيز_جو_TOC;
    لكل (uint32_t section = 1; section <= جدول_شروعات; section++) {
        سائيز_جو_معلومات += sectionSizes[section] * حجم(جدول[0]);
    }
    إذا (is_معلومات_ڊيگهائي_معلوم && معلومات_ڊيگهائي < سائيز_جو_معلومات) {
        // Truncated file!
        اذهب invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    curr_وٿي = (جدول_شروعات + 1) * (حجم(uint32_t)/حجم(uint16_t));
    g_Main_جدول.converterList = جدول + curr_وٿي;

    curr_وٿي += g_Main_جدول.converter_List_سائيز;
    g_Main_جدول.ٽيگ_List = جدول + curr_وٿي;

    curr_وٿي += g_Main_جدول.ٽيگ_List_سائيز;
    g_Main_جدول.aliasList = جدول + curr_وٿي;

    curr_وٿي += g_Main_جدول.alias_List_سائيز;
    g_Main_جدول.untaggedConvArray = جدول + curr_وٿي;

    curr_وٿي += g_Main_جدول.untagged_Conv_Array_سائيز;
    g_Main_جدول.taggedAliasArray = جدول + curr_وٿي;

    /* aliasLists is a 1's based array, but it has a padding character */
    curr_وٿي += g_Main_جدول.tagged_Alias_Array_سائيز;
    g_Main_جدول.taggedAliasLists = جدول + curr_وٿي;

    curr_وٿي += g_Main_جدول.tagged_Alias_Lists_سائيز;
    إذا (g_Main_جدول.option_جدول_سائيز > 0
        && reinterpret_cast<ثابت UConverterAliasOptions*>(جدول + curr_وٿي)->stringNormalizationType < UCNV_IO_NORM_TYPE_ڳڻپ)
    {
        /* Faster table */
        g_Main_جدول.option_جدول = reinterpret_cast<ثابت UConverterAliasOptions*>(جدول + curr_وٿي);
    }
    وإلا {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        g_Main_جدول.option_جدول = &default_جدول_Options;
    }

    curr_وٿي += g_Main_جدول.option_جدول_سائيز;
    g_Main_جدول.string_جدول = جدول + curr_وٿي;

    curr_وٿي += g_Main_جدول.string_جدول_سائيز;
    g_Main_جدول.normalized_String_جدول = ((g_Main_جدول.option_جدول->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? g_Main_جدول.string_جدول : (جدول + curr_وٿي));

    أرجع;

invalidFormat:
    err_ڪوڊ = U_INVALID_FORMAT_ERROR;
    udata_close(معلومات);
}


ساكن UBool
have_Alias_معلومات(UError_ڪوڊ *p_Error_ڪوڊ) {
    umtx_initOnce(g_Alias_معلومات_Init_Once, &init_Alias_معلومات, *p_Error_ڪوڊ);
    أرجع U_SUCCESS(*p_Error_ڪوڊ);
}

ساكن مضمن UBool
isAlias(ثابت حرف *alias, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(alias==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خطأ;
    }
    أرجع *alias != 0;
}

ساكن uint32_t get_ٽيگ_عدد(ثابت حرف *tagname) {
    إذا (g_Main_جدول.ٽيگ_List) {
        uint32_t ٽيگ_انگ;
        لكل (ٽيگ_انگ = 0; ٽيگ_انگ < g_Main_جدول.ٽيگ_List_سائيز; ٽيگ_انگ++) {
            إذا (!uprv_stricmp(GET_STRING(g_Main_جدول.ٽيگ_List[ٽيگ_انگ]), tagname)) {
                أرجع ٽيگ_انگ;
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

#وصف_ڪريو GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

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

#وصف_ڪريو GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#وإلا
#   error U_CHARSET_FAMILY is not valid
#جيڪڏهن_پورو


/* @see ucnv_compareNames */
U_CAPI حرف* U_CALLCONV
ucnv_io_stripASCIIForCompare(حرف* dst U_LIFETIME_BOUND, ثابت حرف* name) {
    حرف *dstItr = dst;
    uint8_t type, ايندڙ_Type;
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
                ايندڙ_Type = GET_ASCII_TYPE(*name);
                إذا (ايندڙ_Type == ZERO || ايندڙ_Type == NONZERO) {
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
    uint8_t type, ايندڙ_Type;
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
                ايندڙ_Type = GET_EBCDIC_TYPE(*name);
                إذا (ايندڙ_Type == ZERO || ايندڙ_Type == NONZERO) {
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
ucnv_compare_نالا(ثابت حرف *name1, ثابت حرف *name2) {
    صحيح rc;
    uint8_t type, ايندڙ_Type;
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
                    ايندڙ_Type = GET_CHAR_TYPE(*name1);
                    إذا (ايندڙ_Type == ZERO || ايندڙ_Type == NONZERO) {
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
                    ايندڙ_Type = GET_CHAR_TYPE(*name2);
                    إذا (ايندڙ_Type == ZERO || ايندڙ_Type == NONZERO) {
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
        rc = (صحيح)(بنانشان حرف)c1 - (صحيح)(بنانشان حرف)c2;
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
ڳوليو_Converter(ثابت حرف *alias, UBool *شامل_آهي_Option, UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t mid, شروعات, حد_بندي;
    uint32_t آخري_Mid;
    صحيح نتيجو;
    صحيح isUnnormalized = (g_Main_جدول.option_جدول->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_ڊيگهائي];

    إذا (!isUnnormalized) {
        إذا (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_ڊيگهائي) {
            *p_Error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
            أرجع UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    شروعات = 0;
    حد_بندي = g_Main_جدول.untagged_Conv_Array_سائيز;
    mid = حد_بندي;
    آخري_Mid = UINT32_MAX;

    لكل (;;) {
        mid = (شروعات + حد_بندي) / 2;
        إذا (آخري_Mid == mid) {   /* Have we moved? */
            اكسر;  /* We haven't moved, and it wasn't found. */
        }
        آخري_Mid = mid;
        إذا (isUnnormalized) {
            نتيجو = ucnv_compare_نالا(alias, GET_STRING(g_Main_جدول.aliasList[mid]));
        }
        وإلا {
            نتيجو = uprv_strcmp(alias, GET_NORMALIZED_STRING(g_Main_جدول.aliasList[mid]));
        }

        إذا (نتيجو < 0) {
            حد_بندي = mid;
        } وإلا إذا (نتيجو > 0) {
            شروعات = mid;
        } وإلا {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            إذا (g_Main_جدول.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_ٻٽو_انگ) {
                *p_Error_ڪوڊ = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            إذا (شامل_آهي_Option) {
                UBool شامل_آهي_Cnv_Option_Info = static_cast<UBool>(g_Main_جدول.option_جدول->شامل_آهي_Cnv_Option_Info);
                *شامل_آهي_Option = static_cast<UBool>((شامل_آهي_Cnv_Option_Info
                    && ((g_Main_جدول.untaggedConvArray[mid] & UCNV_شامل_آهي_OPTION_ٻٽو_انگ) != 0))
                    || !شامل_آهي_Cnv_Option_Info);
            }
            أرجع g_Main_جدول.untaggedConvArray[mid] & UCNV_CONVERTER_اشاريو_نقاب;
        }
    }

    أرجع UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
ساكن مضمن UBool
is_Alias_اندريون_List(ثابت حرف *alias, uint32_t list_وٿي) {
    إذا (list_وٿي) {
        uint32_t currAlias;
        uint32_t list_ڳڻپ = g_Main_جدول.taggedAliasLists[list_وٿي];
        /* +1 to skip listCount */
        ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وٿي + 1;
        لكل (currAlias = 0; currAlias < list_ڳڻپ; currAlias++) {
            إذا (currList[currAlias]
                && ucnv_compare_نالا(alias, GET_STRING(currList[currAlias]))==0)
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
ڳوليو_Tagged_Alias_Lists_وٿي(ثابت حرف *alias, ثابت حرف *standard, UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t idx;
    uint32_t list_وٿي;
    uint32_t conv_انگ;
    UError_ڪوڊ myErr = U_ZERO_ERROR;
    uint32_t ٽيگ_انگ = get_ٽيگ_عدد(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_انگ = ڳوليو_Converter(alias, خالي_اشارو, &myErr);
    إذا (myErr != U_ZERO_ERROR) {
        *p_Error_ڪوڊ = myErr;
    }

    إذا (ٽيگ_انگ < (g_Main_جدول.ٽيگ_List_سائيز - UCNV_انگ_HIDDEN_ٽيگ_فهرست) && conv_انگ < g_Main_جدول.converter_List_سائيز) {
        list_وٿي = g_Main_جدول.taggedAliasArray[ٽيگ_انگ*g_Main_جدول.converter_List_سائيز + conv_انگ];
        إذا (list_وٿي && g_Main_جدول.taggedAliasLists[list_وٿي + 1]) {
            أرجع list_وٿي;
        }
        إذا (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            لكل (idx = 0; idx < g_Main_جدول.tagged_Alias_Array_سائيز; idx++) {
                list_وٿي = g_Main_جدول.taggedAliasArray[idx];
                إذا (list_وٿي && is_Alias_اندريون_List(alias, list_وٿي)) {
                    uint32_t curr_ٽيگ_انگ = idx/g_Main_جدول.converter_List_سائيز;
                    uint32_t curr_Conv_انگ = (idx - curr_ٽيگ_انگ*g_Main_جدول.converter_List_سائيز);
                    uint32_t عارضي_List_وٿي = g_Main_جدول.taggedAliasArray[ٽيگ_انگ*g_Main_جدول.converter_List_سائيز + curr_Conv_انگ];
                    إذا (عارضي_List_وٿي && g_Main_جدول.taggedAliasLists[عارضي_List_وٿي + 1]) {
                        أرجع عارضي_List_وٿي;
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
ڳوليو_Tagged_Converter_انگ(ثابت حرف *alias, ثابت حرف *standard, UError_ڪوڊ *p_Error_ڪوڊ) {
    uint32_t idx;
    uint32_t list_وٿي;
    uint32_t conv_انگ;
    UError_ڪوڊ myErr = U_ZERO_ERROR;
    uint32_t ٽيگ_انگ = get_ٽيگ_عدد(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_انگ = ڳوليو_Converter(alias, خالي_اشارو, &myErr);
    إذا (myErr != U_ZERO_ERROR) {
        *p_Error_ڪوڊ = myErr;
    }

    إذا (ٽيگ_انگ < (g_Main_جدول.ٽيگ_List_سائيز - UCNV_انگ_HIDDEN_ٽيگ_فهرست) && conv_انگ < g_Main_جدول.converter_List_سائيز) {
        list_وٿي = g_Main_جدول.taggedAliasArray[ٽيگ_انگ*g_Main_جدول.converter_List_سائيز + conv_انگ];
        إذا (list_وٿي && is_Alias_اندريون_List(alias, list_وٿي)) {
            أرجع conv_انگ;
        }
        إذا (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t conv_شروعات = (ٽيگ_انگ)*g_Main_جدول.converter_List_سائيز;
            uint32_t conv_حد_بندي = (ٽيگ_انگ+1)*g_Main_جدول.converter_List_سائيز;
            لكل (idx = conv_شروعات; idx < conv_حد_بندي; idx++) {
                list_وٿي = g_Main_جدول.taggedAliasArray[idx];
                إذا (list_وٿي && is_Alias_اندريون_List(alias, list_وٿي)) {
                    أرجع idx-conv_شروعات;
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
ucnv_io_getConverterName(ثابت حرف *alias, UBool *شامل_آهي_Option, UError_ڪوڊ *p_Error_ڪوڊ) {
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
        إذا(have_Alias_معلومات(p_Error_ڪوڊ) && isAlias(aliasTmp, p_Error_ڪوڊ)) {
            uint32_t conv_انگ = ڳوليو_Converter(aliasTmp, شامل_آهي_Option, p_Error_ڪوڊ);
            إذا (conv_انگ < g_Main_جدول.converter_List_سائيز) {
                أرجع GET_STRING(g_Main_جدول.converterList[conv_انگ]);
            }
            /* else converter not found */
        } وإلا {
            اكسر;
        }
    }

    أرجع خالي_اشارو;
}

U_CDECL_BEGIN


ساكن int32_t U_CALLCONV
ucnv_io_ڳڻپ_Standard_Aliases(UEnumeration *enumerator, UError_ڪوڊ * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_وٿي = myContext->list_وٿي;

    إذا (list_وٿي) {
        value = g_Main_جدول.taggedAliasLists[list_وٿي];
    }
    أرجع value;
}

ساكن ثابت حرف * U_CALLCONV
ucnv_io_ايندڙ_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* نتيجو_ڊيگهائي,
                            UError_ڪوڊ * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_وٿي = myContext->list_وٿي;

    إذا (list_وٿي) {
        uint32_t list_ڳڻپ = g_Main_جدول.taggedAliasLists[list_وٿي];
        ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وٿي + 1;

        إذا (myContext->listIdx < list_ڳڻپ) {
            ثابت حرف *myStr = GET_STRING(currList[myContext->listIdx++]);
            إذا (نتيجو_ڊيگهائي) {
                *نتيجو_ڊيگهائي = (int32_t)uprv_strlen(myStr);
            }
            أرجع myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    إذا (نتيجو_ڊيگهائي) {
        *نتيجو_ڊيگهائي = 0;
    }
    أرجع خالي_اشارو;
}

ساكن فراغ U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UError_ڪوڊ * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

ساكن فراغ U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_پڇاڙي

/* Enumerate the aliases for the specified converter and standard tag */
ساكن ثابت UEnumeration gEnumAliases = {
    خالي_اشارو,
    خالي_اشارو,
    ucnv_io_closeUEnumeration,
    ucnv_io_ڳڻپ_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_ايندڙ_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_Standard_نالا(ثابت حرف *convName,
                       ثابت حرف *standard,
                       UError_ڪوڊ *p_Error_ڪوڊ)
{
    UEnumeration *myEnum = خالي_اشارو;
    إذا (have_Alias_معلومات(p_Error_ڪوڊ) && isAlias(convName, p_Error_ڪوڊ)) {
        uint32_t list_وٿي = ڳوليو_Tagged_Alias_Lists_وٿي(convName, standard, p_Error_ڪوڊ);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        إذا (list_وٿي < g_Main_جدول.tagged_Alias_Lists_سائيز) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
            إذا (myEnum == خالي_اشارو) {
                *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                أرجع خالي_اشارو;
            }
            uprv_memcpy(myEnum, &gEnumAliases, حجم(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(حجم(UAliasContext)));
            إذا (myContext == خالي_اشارو) {
                *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                uprv_free(myEnum);
                أرجع خالي_اشارو;
            }
            myContext->list_وٿي = list_وٿي;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    أرجع myEnum;
}

ساكن uint16_t
ucnv_io_ڳڻپ_Aliases(ثابت حرف *alias, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(have_Alias_معلومات(p_Error_ڪوڊ) && isAlias(alias, p_Error_ڪوڊ)) {
        uint32_t conv_انگ = ڳوليو_Converter(alias, خالي_اشارو, p_Error_ڪوڊ);
        إذا (conv_انگ < g_Main_جدول.converter_List_سائيز) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_وٿي = g_Main_جدول.taggedAliasArray[(g_Main_جدول.ٽيگ_List_سائيز - 1)*g_Main_جدول.converter_List_سائيز + conv_انگ];

            إذا (list_وٿي) {
                أرجع g_Main_جدول.taggedAliasLists[list_وٿي];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    أرجع 0;
}

ساكن uint16_t
ucnv_io_getAliases(ثابت حرف *alias, uint16_t شروعات, ثابت حرف **aliases, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(have_Alias_معلومات(p_Error_ڪوڊ) && isAlias(alias, p_Error_ڪوڊ)) {
        uint32_t currAlias;
        uint32_t conv_انگ = ڳوليو_Converter(alias, خالي_اشارو, p_Error_ڪوڊ);
        إذا (conv_انگ < g_Main_جدول.converter_List_سائيز) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_وٿي = g_Main_جدول.taggedAliasArray[(g_Main_جدول.ٽيگ_List_سائيز - 1)*g_Main_جدول.converter_List_سائيز + conv_انگ];

            إذا (list_وٿي) {
                uint32_t list_ڳڻپ = g_Main_جدول.taggedAliasLists[list_وٿي];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وٿي + 1;

                لكل (currAlias = شروعات; currAlias < list_ڳڻپ; currAlias++) {
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
ucnv_io_getAlias(ثابت حرف *alias, uint16_t n, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(have_Alias_معلومات(p_Error_ڪوڊ) && isAlias(alias, p_Error_ڪوڊ)) {
        uint32_t conv_انگ = ڳوليو_Converter(alias, خالي_اشارو, p_Error_ڪوڊ);
        إذا (conv_انگ < g_Main_جدول.converter_List_سائيز) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_وٿي = g_Main_جدول.taggedAliasArray[(g_Main_جدول.ٽيگ_List_سائيز - 1)*g_Main_جدول.converter_List_سائيز + conv_انگ];

            إذا (list_وٿي) {
                uint32_t list_ڳڻپ = g_Main_جدول.taggedAliasLists[list_وٿي];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وٿي + 1;

                إذا (n < list_ڳڻپ)  {
                    أرجع GET_STRING(currList[n]);
                }
                *p_Error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    أرجع خالي_اشارو;
}

ساكن uint16_t
ucnv_io_ڳڻپ_Standards(UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (have_Alias_معلومات(p_Error_ڪوڊ)) {
        /* Don't include the empty list */
        أرجع static_cast<uint16_t>(g_Main_جدول.ٽيگ_List_سائيز - UCNV_انگ_HIDDEN_ٽيگ_فهرست);
    }

    أرجع 0;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandard(uint16_t n, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (have_Alias_معلومات(p_Error_ڪوڊ)) {
        إذا (n < g_Main_جدول.ٽيگ_List_سائيز - UCNV_انگ_HIDDEN_ٽيگ_فهرست) {
            أرجع GET_STRING(g_Main_جدول.ٽيگ_List[n]);
        }
        *p_Error_ڪوڊ = U_اشاريو_OUTOFBOUNDS_ERROR;
    }

    أرجع خالي_اشارو;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandardName(ثابت حرف *alias, ثابت حرف *standard, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (have_Alias_معلومات(p_Error_ڪوڊ) && isAlias(alias, p_Error_ڪوڊ)) {
        uint32_t list_وٿي = ڳوليو_Tagged_Alias_Lists_وٿي(alias, standard, p_Error_ڪوڊ);

        إذا (0 < list_وٿي && list_وٿي < g_Main_جدول.tagged_Alias_Lists_سائيز) {
            ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_وٿي + 1;

            /* Get the preferred name from this list */
            إذا (currList[0]) {
                أرجع GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    أرجع خالي_اشارو;
}

U_CAPI uint16_t U_EXPORT2
ucnv_ڳڻپ_Aliases(ثابت حرف *alias, UError_ڪوڊ *p_Error_ڪوڊ)
{
    أرجع ucnv_io_ڳڻپ_Aliases(alias, p_Error_ڪوڊ);
}


U_CAPI ثابت حرف* U_EXPORT2
ucnv_getAlias(ثابت حرف *alias, uint16_t n, UError_ڪوڊ *p_Error_ڪوڊ)
{
    أرجع ucnv_io_getAlias(alias, n, p_Error_ڪوڊ);
}

U_CAPI فراغ U_EXPORT2
ucnv_getAliases(ثابت حرف *alias, ثابت حرف **aliases, UError_ڪوڊ *p_Error_ڪوڊ)
{
    ucnv_io_getAliases(alias, 0, aliases, p_Error_ڪوڊ);
}

U_CAPI uint16_t U_EXPORT2
ucnv_ڳڻپ_Standards()
{
    UError_ڪوڊ err = U_ZERO_ERROR;
    أرجع ucnv_io_ڳڻپ_Standards(&err);
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getCanonicalName(ثابت حرف *alias, ثابت حرف *standard, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (have_Alias_معلومات(p_Error_ڪوڊ) && isAlias(alias, p_Error_ڪوڊ)) {
        uint32_t conv_انگ = ڳوليو_Tagged_Converter_انگ(alias, standard, p_Error_ڪوڊ);

        إذا (conv_انگ < g_Main_جدول.converter_List_سائيز) {
            أرجع GET_STRING(g_Main_جدول.converterList[conv_انگ]);
        }
    }

    أرجع خالي_اشارو;
}

U_CDECL_BEGIN


ساكن int32_t U_CALLCONV
ucnv_io_ڳڻپ_All_Converters(UEnumeration * /*enumerator*/, UError_ڪوڊ * /*pErrorCode*/) {
    أرجع g_Main_جدول.converter_List_سائيز;
}

ساكن ثابت حرف * U_CALLCONV
ucnv_io_ايندڙ_All_Converters(UEnumeration *enumerator,
                            int32_t* نتيجو_ڊيگهائي,
                            UError_ڪوڊ * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    إذا (*myContext < g_Main_جدول.converter_List_سائيز) {
        ثابت حرف *myStr = GET_STRING(g_Main_جدول.converterList[(*myContext)++]);
        إذا (نتيجو_ڊيگهائي) {
            *نتيجو_ڊيگهائي = (int32_t)uprv_strlen(myStr);
        }
        أرجع myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    إذا (نتيجو_ڊيگهائي) {
        *نتيجو_ڊيگهائي = 0;
    }
    أرجع خالي_اشارو;
}

ساكن فراغ U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UError_ڪوڊ * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_پڇاڙي
ساكن ثابت UEnumeration gEnumAllConverters = {
    خالي_اشارو,
    خالي_اشارو,
    ucnv_io_closeUEnumeration,
    ucnv_io_ڳڻپ_All_Converters,
    uenum_unextDefault,
    ucnv_io_ايندڙ_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_All_نالا(UError_ڪوڊ *p_Error_ڪوڊ) {
    UEnumeration *myEnum = خالي_اشارو;
    إذا (have_Alias_معلومات(p_Error_ڪوڊ)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
        إذا (myEnum == خالي_اشارو) {
            *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع خالي_اشارو;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, حجم(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(حجم(uint16_t)));
        إذا (myContext == خالي_اشارو) {
            *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            uprv_free(myEnum);
            أرجع خالي_اشارو;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    أرجع myEnum;
}

U_CAPI uint16_t
ucnv_io_ڳڻپ_معلوم_Converters(UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا (have_Alias_معلومات(p_Error_ڪوڊ)) {
        أرجع (uint16_t)g_Main_جدول.converter_List_سائيز;
    }
    أرجع 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

تعريفنوع حرف * U_CALLCONV StripForCompareFn(حرف *dst, ثابت حرف *name);
U_CDECL_پڇاڙي


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
تعريفنوع بنية عارضي_صف {
    uint16_t str_اشاريو, ترتيب_ڏيو_اشاريو;
} عارضي_صف;

تعريفنوع بنية عارضي_Alias_جدول {
    ثابت حرف *chars;
    عارضي_صف *صفون;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} عارضي_Alias_جدول;

تعداد {
    ڍير_صف_صلاحيت=500
};

ساكن int32_t U_CALLCONV
io_compare_صفون(ثابت فراغ *context, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    حرف stripped_کاٻو[UCNV_MAX_CONVERTER_NAME_ڊيگهائي],
         stripped_ساڄو[UCNV_MAX_CONVERTER_NAME_ڊيگهائي];

    عارضي_Alias_جدول *عارضي_جدول=(عارضي_Alias_جدول *)context;
    ثابت حرف *chars=عارضي_جدول->chars;

    أرجع static_cast<int32_t>(uprv_strcmp(
        عارضي_جدول->stripForCompare(stripped_کاٻو, chars + 2 * static_cast<ثابت عارضي_صف*>(کاٻي)->str_اشاريو),
        عارضي_جدول->stripForCompare(stripped_ساڄو, chars + 2 * static_cast<ثابت عارضي_صف*>(ساڄي)->str_اشاريو)));
}

U_CAPI int32_t U_EXPORT2
ucnv_مٽا_سٽا_Aliases(ثابت UDataSwapper *ds,
                 ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UDataInfo *pInfo;
    int32_t سرو_سائيز;

    ثابت uint16_t *اندريون_جدول;
    ثابت uint32_t *اندريون_Section_Sizes;
    uint32_t toc[offsets_ڳڻپ];
    uint32_t offsets[offsets_ڳڻپ]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_ڊيگهائي, چوٽي_وٿي;

    عارضي_صف صفون[ڍير_صف_صلاحيت];
    uint16_t resort[ڍير_صف_صلاحيت];
    عارضي_Alias_جدول عارضي_جدول;

    /* udata_swapDataHeader checks the arguments */
    سرو_سائيز=udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->معلومات_Format[1]==0x76 &&
        pInfo->معلومات_Format[2]==0x41 &&
        pInfo->معلومات_Format[3]==0x6c &&
        pInfo->format_نسخو[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخو[0]);
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    /* an alias table must contain at least the table of contents array */
    إذا(length>=0 && (length-سرو_سائيز)<4*(1+min_Toc_ڊيگهائي)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-سرو_سائيز);
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    اندريون_Section_Sizes=(ثابت uint32_t *)((ثابت حرف *)اندريون_معلومات+سرو_سائيز);
    اندريون_جدول=(ثابت uint16_t *)اندريون_Section_Sizes;
    uprv_memset(toc, 0, حجم(toc));
    toc[toc_ڊيگهائي_اشاريو]=toc_ڊيگهائي=ds->readUInt32(اندريون_Section_Sizes[toc_ڊيگهائي_اشاريو]);
    إذا(toc_ڊيگهائي<min_Toc_ڊيگهائي || offsets_ڳڻپ<=toc_ڊيگهائي) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_ڊيگهائي);
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    /* read the known part of the table of contents */
    لكل(i=converter_List_اشاريو; i<=toc_ڊيگهائي; ++i) {
        toc[i]=ds->readUInt32(اندريون_Section_Sizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, حجم(offsets));
    offsets[converter_List_اشاريو]=2*(1+toc_ڊيگهائي); /* count two 16-bit units per toc entry */
    لكل(i=ٽيگ_List_اشاريو; i<=toc_ڊيگهائي; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    چوٽي_وٿي=offsets[i-1]+toc[i-1];

    إذا(length>=0) {
        uint16_t *ٻاهريون_جدول;
        ثابت uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_اشاريو;

        إذا((length-سرو_سائيز)<(2*(int32_t)چوٽي_وٿي)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-سرو_سائيز);
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        ٻاهريون_جدول=(uint16_t *)((حرف *)ٻاهريون_معلومات+سرو_سائيز);

        /* swap the entire table of contents */
        ds->مٽا_سٽا_Array32(ds, اندريون_جدول, 4*(1+toc_ڊيگهائي), ٻاهريون_جدول, p_Error_ڪوڊ);

        /* swap unormalized strings & normalized strings */
        ds->مٽا_سٽا_Inv_Chars(ds, اندريون_جدول+offsets[string_جدول_اشاريو], 2*(int32_t)(toc[string_جدول_اشاريو]+toc[normalized_String_جدول_اشاريو]),
                             ٻاهريون_جدول+offsets[string_جدول_اشاريو], p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            أرجع 0;
        }

        إذا(ds->اندريون_Charset==ds->ٻاهريون_Charset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->مٽا_سٽا_Array16(ds,
                            اندريون_جدول+offsets[converter_List_اشاريو],
                            2*(int32_t)(offsets[string_جدول_اشاريو]-offsets[converter_List_اشاريو]),
                            ٻاهريون_جدول+offsets[converter_List_اشاريو],
                            p_Error_ڪوڊ);
        } وإلا {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_اشاريو];

            عارضي_جدول.chars=(ثابت حرف *)(ٻاهريون_جدول+offsets[string_جدول_اشاريو]); /* sort by outCharset */

            إذا(count<=ڍير_صف_صلاحيت) {
                عارضي_جدول.صفون=صفون;
                عارضي_جدول.resort=resort;
            } وإلا {
                عارضي_جدول.صفون=(عارضي_صف *)uprv_malloc(count*حجم(عارضي_صف)+count*2);
                إذا(عارضي_جدول.صفون==خالي_اشارو) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                    أرجع 0;
                }
                عارضي_جدول.resort=(uint16_t *)(عارضي_جدول.صفون+count);
            }

            إذا(ds->ٻاهريون_Charset==U_ASCII_FAMILY) {
                عارضي_جدول.stripForCompare=ucnv_io_stripASCIIForCompare;
            } وإلا /* U_EBCDIC_FAMILY */ {
                عارضي_جدول.stripForCompare=ucnv_io_stripEBCDICForCompare;
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
            p=اندريون_جدول+offsets[alias_List_اشاريو];
            q=ٻاهريون_جدول+offsets[alias_List_اشاريو];

            p2=اندريون_جدول+offsets[untagged_Conv_Array_اشاريو];
            q2=ٻاهريون_جدول+offsets[untagged_Conv_Array_اشاريو];

            لكل(i=0; i<count; ++i) {
                عارضي_جدول.صفون[i].str_اشاريو=ds->readUInt16(p[i]);
                عارضي_جدول.صفون[i].ترتيب_ڏيو_اشاريو=(uint16_t)i;
            }

            uprv_ترتيب_ڏيو_Array(عارضي_جدول.صفون, (int32_t)count, حجم(عارضي_صف),
                           io_compare_صفون, &عارضي_جدول,
                           خطأ, p_Error_ڪوڊ);

            إذا(U_SUCCESS(*p_Error_ڪوڊ)) {
                /* copy/swap/permutate items */
                إذا(p!=q) {
                    لكل(i=0; i<count; ++i) {
                        old_اشاريو=عارضي_جدول.صفون[i].ترتيب_ڏيو_اشاريو;
                        ds->مٽا_سٽا_Array16(ds, p+old_اشاريو, 2, q+i, p_Error_ڪوڊ);
                        ds->مٽا_سٽا_Array16(ds, p2+old_اشاريو, 2, q2+i, p_Error_ڪوڊ);
                    }
                } وإلا {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=عارضي_جدول.resort;

                    لكل(i=0; i<count; ++i) {
                        old_اشاريو=عارضي_جدول.صفون[i].ترتيب_ڏيو_اشاريو;
                        ds->مٽا_سٽا_Array16(ds, p+old_اشاريو, 2, r+i, p_Error_ڪوڊ);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    لكل(i=0; i<count; ++i) {
                        old_اشاريو=عارضي_جدول.صفون[i].ترتيب_ڏيو_اشاريو;
                        ds->مٽا_سٽا_Array16(ds, p2+old_اشاريو, 2, r+i, p_Error_ڪوڊ);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            إذا(عارضي_جدول.صفون!=صفون) {
                uprv_free(عارضي_جدول.صفون);
            }

            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                أرجع 0;
            }

            /* swap remaining 16-bit values */
            ds->مٽا_سٽا_Array16(ds,
                            اندريون_جدول+offsets[converter_List_اشاريو],
                            2*(int32_t)(offsets[alias_List_اشاريو]-offsets[converter_List_اشاريو]),
                            ٻاهريون_جدول+offsets[converter_List_اشاريو],
                            p_Error_ڪوڊ);
            ds->مٽا_سٽا_Array16(ds,
                            اندريون_جدول+offsets[tagged_Alias_Array_اشاريو],
                            2*(int32_t)(offsets[string_جدول_اشاريو]-offsets[tagged_Alias_Array_اشاريو]),
                            ٻاهريون_جدول+offsets[tagged_Alias_Array_اشاريو],
                            p_Error_ڪوڊ);
        }
    }

    أرجع سرو_سائيز+2*(int32_t)چوٽي_وٿي;
}

#جيڪڏهن_پورو


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

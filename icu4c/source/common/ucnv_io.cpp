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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/udata.h"

#شامل "umutex.h"
#شامل "uarrsort.h"
#شامل "uassert.h"
#شامل "udataswp.h"
#شامل "udatamem.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "ucnv_io.h"
#شامل "uenumimp.h"
#شامل "ucln_cmn.h"

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
قسمتعریف ساخت UAliasContext {
    uint32_t list_انحراف;
    uint32_t listIdx;
} UAliasContext;

ساکن ثابت حرف ڈیٹا_NAME[] = "cnvalias";
ساکن ثابت حرف ڈیٹا_TYPE[] = "icu";

ساکن UData_حافظہ *g_Alias_ڈیٹا=خالی_اشاریہ;
ساکن icu::UInitOnce g_Alias_ڈیٹا_Init_Once {};

شمار {
    toc_لمبائی_اشاریہ=0,
    converter_List_اشاریہ=1,
    لیبل_List_اشاریہ=2,
    alias_List_اشاریہ=3,
    untagged_Conv_Array_اشاریہ=4,
    tagged_Alias_Array_اشاریہ=5,
    tagged_Alias_Lists_اشاریہ=6,
    جدول_Options_اشاریہ=7,
    string_جدول_اشاریہ=8,
    normalized_String_جدول_اشاریہ=9,
    offsets_گنتی,    /* length of the swapper's temporary offsets[] */
    min_Toc_لمبائی=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

ساکن ثابت UConverterAliasOptions default_جدول_Options = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
ساکن UConverterAlias g_Main_جدول;

#تعریف_کرو GET_STRING(idx) (ثابت حرف *)(g_Main_جدول.string_جدول + (idx))
#تعریف_کرو GET_NORMALIZED_STRING(idx) (ثابت حرف *)(g_Main_جدول.normalized_String_جدول + (idx))

ساکن UBool U_CALLCONV
isAcceptable(خالی * /*context*/,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    واپس
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ڈیٹا_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->ڈیٹا_Format[1]==0x76 &&
        pInfo->ڈیٹا_Format[2]==0x41 &&
        pInfo->ڈیٹا_Format[3]==0x6c &&
        pInfo->format_نسخہ[0]==3;
}

ساکن UBool U_CALLCONV ucnv_io_cleanup()
{
    اگر (g_Alias_ڈیٹا) {
        udata_close(g_Alias_ڈیٹا);
        g_Alias_ڈیٹا = خالی_اشاریہ;
    }
    g_Alias_ڈیٹا_Init_Once.reset();

    uprv_memset(&g_Main_جدول, 0, حجم(g_Main_جدول));

    واپس سچ;                   /* Everything was cleaned up */
}

ساکن خالی U_CALLCONV init_Alias_ڈیٹا(UError_رمز &err_رمز) {
    UData_حافظہ *ڈیٹا;
    ثابت uint16_t *جدول;
    ثابت uint32_t *sectionSizes;
    uint32_t جدول_آغاز;
    uint32_t curr_انحراف;
    int32_t سائز_کا_ڈیٹا;
    int32_t سائز_کا_TOC;

    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_ڈیٹا == خالی_اشاریہ);
    ڈیٹا = udata_openChoice(خالی_اشاریہ, ڈیٹا_TYPE, ڈیٹا_NAME, isAcceptable, خالی_اشاریہ, &err_رمز);
    اگر (U_FAILURE(err_رمز)) {
        واپس;
    }

    sectionSizes = static_cast<ثابت uint32_t*>(udata_get_حافظہ(ڈیٹا));
    int32_t ڈیٹا_لمبائی = udata_get_لمبائی(ڈیٹا); // This is the length minus the UDataInfo size
    UBool is_ڈیٹا_لمبائی_معلوم = ڈیٹا_لمبائی >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    اگر (is_ڈیٹا_لمبائی_معلوم && ڈیٹا_لمبائی <= int32_t(حجم(sectionSizes[0]))) {
        // We don't even have a TOC!
        جاؤ invalidFormat;
    }
    جدول = reinterpret_cast<ثابت uint16_t*>(sectionSizes);
    جدول_آغاز = sectionSizes[0];
    سائز_کا_TOC = int32_t((جدول_آغاز + 1) * حجم(sectionSizes[0]));
    اگر (جدول_آغاز < min_Toc_لمبائی || (is_ڈیٹا_لمبائی_معلوم && ڈیٹا_لمبائی <= سائز_کا_TOC)) {
        // We don't have a whole TOC!
        جاؤ invalidFormat;
    }
    g_Alias_ڈیٹا = ڈیٹا;

    g_Main_جدول.converter_List_سائز      = sectionSizes[1];
    g_Main_جدول.لیبل_List_سائز            = sectionSizes[2];
    g_Main_جدول.alias_List_سائز          = sectionSizes[3];
    g_Main_جدول.untagged_Conv_Array_سائز  = sectionSizes[4];
    g_Main_جدول.tagged_Alias_Array_سائز   = sectionSizes[5];
    g_Main_جدول.tagged_Alias_Lists_سائز   = sectionSizes[6];
    g_Main_جدول.option_جدول_سائز        = sectionSizes[7];
    g_Main_جدول.string_جدول_سائز        = sectionSizes[8];

    اگر (جدول_آغاز > min_Toc_لمبائی) {
        g_Main_جدول.normalized_String_جدول_سائز = sectionSizes[9];
    }

    سائز_کا_ڈیٹا = سائز_کا_TOC;
    برائے (uint32_t section = 1; section <= جدول_آغاز; section++) {
        سائز_کا_ڈیٹا += sectionSizes[section] * حجم(جدول[0]);
    }
    اگر (is_ڈیٹا_لمبائی_معلوم && ڈیٹا_لمبائی < سائز_کا_ڈیٹا) {
        // Truncated file!
        جاؤ invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    curr_انحراف = (جدول_آغاز + 1) * (حجم(uint32_t)/حجم(uint16_t));
    g_Main_جدول.converterList = جدول + curr_انحراف;

    curr_انحراف += g_Main_جدول.converter_List_سائز;
    g_Main_جدول.لیبل_List = جدول + curr_انحراف;

    curr_انحراف += g_Main_جدول.لیبل_List_سائز;
    g_Main_جدول.aliasList = جدول + curr_انحراف;

    curr_انحراف += g_Main_جدول.alias_List_سائز;
    g_Main_جدول.untaggedConvArray = جدول + curr_انحراف;

    curr_انحراف += g_Main_جدول.untagged_Conv_Array_سائز;
    g_Main_جدول.taggedAliasArray = جدول + curr_انحراف;

    /* aliasLists is a 1's based array, but it has a padding character */
    curr_انحراف += g_Main_جدول.tagged_Alias_Array_سائز;
    g_Main_جدول.taggedAliasLists = جدول + curr_انحراف;

    curr_انحراف += g_Main_جدول.tagged_Alias_Lists_سائز;
    اگر (g_Main_جدول.option_جدول_سائز > 0
        && reinterpret_cast<ثابت UConverterAliasOptions*>(جدول + curr_انحراف)->stringNormalizationType < UCNV_IO_NORM_TYPE_گنتی)
    {
        /* Faster table */
        g_Main_جدول.option_جدول = reinterpret_cast<ثابت UConverterAliasOptions*>(جدول + curr_انحراف);
    }
    ورنہ {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        g_Main_جدول.option_جدول = &default_جدول_Options;
    }

    curr_انحراف += g_Main_جدول.option_جدول_سائز;
    g_Main_جدول.string_جدول = جدول + curr_انحراف;

    curr_انحراف += g_Main_جدول.string_جدول_سائز;
    g_Main_جدول.normalized_String_جدول = ((g_Main_جدول.option_جدول->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? g_Main_جدول.string_جدول : (جدول + curr_انحراف));

    واپس;

invalidFormat:
    err_رمز = U_INVALID_FORMAT_ERROR;
    udata_close(ڈیٹا);
}


ساکن UBool
have_Alias_ڈیٹا(UError_رمز *p_Error_رمز) {
    umtx_initOnce(g_Alias_ڈیٹا_Init_Once, &init_Alias_ڈیٹا, *p_Error_رمز);
    واپس U_SUCCESS(*p_Error_رمز);
}

ساکن اندرونی UBool
isAlias(ثابت حرف *alias, UError_رمز *p_Error_رمز) {
    اگر(alias==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس جھوٹ;
    }
    واپس *alias != 0;
}

ساکن uint32_t get_لیبل_نمبر(ثابت حرف *tagname) {
    اگر (g_Main_جدول.لیبل_List) {
        uint32_t لیبل_عدد;
        برائے (لیبل_عدد = 0; لیبل_عدد < g_Main_جدول.لیبل_List_سائز; لیبل_عدد++) {
            اگر (!uprv_stricmp(GET_STRING(g_Main_جدول.لیبل_List[لیبل_عدد]), tagname)) {
                واپس لیبل_عدد;
            }
        }
    }

    واپس UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
شمار {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
ساکن ثابت uint8_t asciiTypes[128] = {
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
ساکن ثابت uint8_t ebcdicTypes[128] = {
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

#اگر U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#وگرنہ_اگر U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#ورنہ
#   error U_CHARSET_FAMILY is not valid
#اگر_ختم


/* @see ucnv_compareNames */
U_CAPI حرف* U_CALLCONV
ucnv_io_stripASCIIForCompare(حرف* dst U_LIFETIME_BOUND, ثابت حرف* name) {
    حرف *dstItr = dst;
    uint8_t type, اگلا_Type;
    حرف c1;
    UBool afterDigit = جھوٹ;

    جب ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        انتخاب (type) {
        حالت UIGNORE:
            afterDigit = جھوٹ;
            جاری; /* ignore all but letters and digits */
        حالت ZERO:
            اگر (!afterDigit) {
                اگلا_Type = GET_ASCII_TYPE(*name);
                اگر (اگلا_Type == ZERO || اگلا_Type == NONZERO) {
                    جاری; /* ignore leading zero before another digit */
                }
            }
            توڑو;
        حالت NONZERO:
            afterDigit = سچ;
            توڑو;
        طےشدہ:
            c1 = (حرف)type; /* lowercased letter */
            afterDigit = جھوٹ;
            توڑو;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    واپس dst;
}

U_CAPI حرف* U_CALLCONV
ucnv_io_stripEBCDICForCompare(حرف* dst U_LIFETIME_BOUND, ثابت حرف* name) {
    حرف *dstItr = dst;
    uint8_t type, اگلا_Type;
    حرف c1;
    UBool afterDigit = جھوٹ;

    جب ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        انتخاب (type) {
        حالت UIGNORE:
            afterDigit = جھوٹ;
            جاری; /* ignore all but letters and digits */
        حالت ZERO:
            اگر (!afterDigit) {
                اگلا_Type = GET_EBCDIC_TYPE(*name);
                اگر (اگلا_Type == ZERO || اگلا_Type == NONZERO) {
                    جاری; /* ignore leading zero before another digit */
                }
            }
            توڑو;
        حالت NONZERO:
            afterDigit = سچ;
            توڑو;
        طےشدہ:
            c1 = (حرف)type; /* lowercased letter */
            afterDigit = جھوٹ;
            توڑو;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    واپس dst;
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
U_CAPI صحیح U_EXPORT2
ucnv_compare_نام(ثابت حرف *name1, ثابت حرف *name2) {
    صحیح rc;
    uint8_t type, اگلا_Type;
    حرف c1, c2;
    UBool afterDigit1 = جھوٹ, afterDigit2 = جھوٹ;

    برائے (;;) {
        جب ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            انتخاب (type) {
            حالت UIGNORE:
                afterDigit1 = جھوٹ;
                جاری; /* ignore all but letters and digits */
            حالت ZERO:
                اگر (!afterDigit1) {
                    اگلا_Type = GET_CHAR_TYPE(*name1);
                    اگر (اگلا_Type == ZERO || اگلا_Type == NONZERO) {
                        جاری; /* ignore leading zero before another digit */
                    }
                }
                توڑو;
            حالت NONZERO:
                afterDigit1 = سچ;
                توڑو;
            طےشدہ:
                c1 = (حرف)type; /* lowercased letter */
                afterDigit1 = جھوٹ;
                توڑو;
            }
            توڑو; /* deliver c1 */
        }
        جب ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            انتخاب (type) {
            حالت UIGNORE:
                afterDigit2 = جھوٹ;
                جاری; /* ignore all but letters and digits */
            حالت ZERO:
                اگر (!afterDigit2) {
                    اگلا_Type = GET_CHAR_TYPE(*name2);
                    اگر (اگلا_Type == ZERO || اگلا_Type == NONZERO) {
                        جاری; /* ignore leading zero before another digit */
                    }
                }
                توڑو;
            حالت NONZERO:
                afterDigit2 = سچ;
                توڑو;
            طےشدہ:
                c2 = (حرف)type; /* lowercased letter */
                afterDigit2 = جھوٹ;
                توڑو;
            }
            توڑو; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        اگر ((c1|c2)==0) {
            واپس 0;
        }

        /* Case-insensitive comparison */
        rc = (صحیح)(غیرعلامتی حرف)c1 - (صحیح)(غیرعلامتی حرف)c2;
        اگر (rc != 0) {
            واپس rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
ساکن اندرونی uint32_t
ڈھونڈو_Converter(ثابت حرف *alias, UBool *شامل_ہے_Option, UError_رمز *p_Error_رمز) {
    uint32_t mid, آغاز, حد_بندی;
    uint32_t آخری_Mid;
    صحیح نتیجہ;
    صحیح isUnnormalized = (g_Main_جدول.option_جدول->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_لمبائی];

    اگر (!isUnnormalized) {
        اگر (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_لمبائی) {
            *p_Error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
            واپس UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    آغاز = 0;
    حد_بندی = g_Main_جدول.untagged_Conv_Array_سائز;
    mid = حد_بندی;
    آخری_Mid = UINT32_MAX;

    برائے (;;) {
        mid = (آغاز + حد_بندی) / 2;
        اگر (آخری_Mid == mid) {   /* Have we moved? */
            توڑو;  /* We haven't moved, and it wasn't found. */
        }
        آخری_Mid = mid;
        اگر (isUnnormalized) {
            نتیجہ = ucnv_compare_نام(alias, GET_STRING(g_Main_جدول.aliasList[mid]));
        }
        ورنہ {
            نتیجہ = uprv_strcmp(alias, GET_NORMALIZED_STRING(g_Main_جدول.aliasList[mid]));
        }

        اگر (نتیجہ < 0) {
            حد_بندی = mid;
        } ورنہ اگر (نتیجہ > 0) {
            آغاز = mid;
        } ورنہ {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            اگر (g_Main_جدول.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_ثنائی_ہندسہ) {
                *p_Error_رمز = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            اگر (شامل_ہے_Option) {
                UBool شامل_ہے_Cnv_Option_Info = static_cast<UBool>(g_Main_جدول.option_جدول->شامل_ہے_Cnv_Option_Info);
                *شامل_ہے_Option = static_cast<UBool>((شامل_ہے_Cnv_Option_Info
                    && ((g_Main_جدول.untaggedConvArray[mid] & UCNV_شامل_ہے_OPTION_ثنائی_ہندسہ) != 0))
                    || !شامل_ہے_Cnv_Option_Info);
            }
            واپس g_Main_جدول.untaggedConvArray[mid] & UCNV_CONVERTER_اشاریہ_نقاب;
        }
    }

    واپس UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
ساکن اندرونی UBool
is_Alias_اندر_List(ثابت حرف *alias, uint32_t list_انحراف) {
    اگر (list_انحراف) {
        uint32_t currAlias;
        uint32_t list_گنتی = g_Main_جدول.taggedAliasLists[list_انحراف];
        /* +1 to skip listCount */
        ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_انحراف + 1;
        برائے (currAlias = 0; currAlias < list_گنتی; currAlias++) {
            اگر (currList[currAlias]
                && ucnv_compare_نام(alias, GET_STRING(currList[currAlias]))==0)
            {
                واپس سچ;
            }
        }
    }
    واپس جھوٹ;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
ساکن uint32_t
ڈھونڈو_Tagged_Alias_Lists_انحراف(ثابت حرف *alias, ثابت حرف *standard, UError_رمز *p_Error_رمز) {
    uint32_t idx;
    uint32_t list_انحراف;
    uint32_t conv_عدد;
    UError_رمز myErr = U_ZERO_ERROR;
    uint32_t لیبل_عدد = get_لیبل_نمبر(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_عدد = ڈھونڈو_Converter(alias, خالی_اشاریہ, &myErr);
    اگر (myErr != U_ZERO_ERROR) {
        *p_Error_رمز = myErr;
    }

    اگر (لیبل_عدد < (g_Main_جدول.لیبل_List_سائز - UCNV_عدد_HIDDEN_لیبل_فہرست) && conv_عدد < g_Main_جدول.converter_List_سائز) {
        list_انحراف = g_Main_جدول.taggedAliasArray[لیبل_عدد*g_Main_جدول.converter_List_سائز + conv_عدد];
        اگر (list_انحراف && g_Main_جدول.taggedAliasLists[list_انحراف + 1]) {
            واپس list_انحراف;
        }
        اگر (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            برائے (idx = 0; idx < g_Main_جدول.tagged_Alias_Array_سائز; idx++) {
                list_انحراف = g_Main_جدول.taggedAliasArray[idx];
                اگر (list_انحراف && is_Alias_اندر_List(alias, list_انحراف)) {
                    uint32_t curr_لیبل_عدد = idx/g_Main_جدول.converter_List_سائز;
                    uint32_t curr_Conv_عدد = (idx - curr_لیبل_عدد*g_Main_جدول.converter_List_سائز);
                    uint32_t عارضی_List_انحراف = g_Main_جدول.taggedAliasArray[لیبل_عدد*g_Main_جدول.converter_List_سائز + curr_Conv_عدد];
                    اگر (عارضی_List_انحراف && g_Main_جدول.taggedAliasLists[عارضی_List_انحراف + 1]) {
                        واپس عارضی_List_انحراف;
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
        واپس 0;
    }
    /* else converter or tag not found */

    واپس UINT32_MAX;
}

/* Return the canonical name */
ساکن uint32_t
ڈھونڈو_Tagged_Converter_عدد(ثابت حرف *alias, ثابت حرف *standard, UError_رمز *p_Error_رمز) {
    uint32_t idx;
    uint32_t list_انحراف;
    uint32_t conv_عدد;
    UError_رمز myErr = U_ZERO_ERROR;
    uint32_t لیبل_عدد = get_لیبل_نمبر(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_عدد = ڈھونڈو_Converter(alias, خالی_اشاریہ, &myErr);
    اگر (myErr != U_ZERO_ERROR) {
        *p_Error_رمز = myErr;
    }

    اگر (لیبل_عدد < (g_Main_جدول.لیبل_List_سائز - UCNV_عدد_HIDDEN_لیبل_فہرست) && conv_عدد < g_Main_جدول.converter_List_سائز) {
        list_انحراف = g_Main_جدول.taggedAliasArray[لیبل_عدد*g_Main_جدول.converter_List_سائز + conv_عدد];
        اگر (list_انحراف && is_Alias_اندر_List(alias, list_انحراف)) {
            واپس conv_عدد;
        }
        اگر (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t conv_آغاز = (لیبل_عدد)*g_Main_جدول.converter_List_سائز;
            uint32_t conv_حد_بندی = (لیبل_عدد+1)*g_Main_جدول.converter_List_سائز;
            برائے (idx = conv_آغاز; idx < conv_حد_بندی; idx++) {
                list_انحراف = g_Main_جدول.taggedAliasArray[idx];
                اگر (list_انحراف && is_Alias_اندر_List(alias, list_انحراف)) {
                    واپس idx-conv_آغاز;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    واپس UINT32_MAX;
}

U_CAPI ثابت حرف *
ucnv_io_getConverterName(ثابت حرف *alias, UBool *شامل_ہے_Option, UError_رمز *p_Error_رمز) {
    ثابت حرف *aliasTmp = alias;
    int32_t i = 0;
    برائے (i = 0; i < 2; i++) {
        اگر (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            اگر (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } ورنہ {
                توڑو;
            }
        }
        اگر(have_Alias_ڈیٹا(p_Error_رمز) && isAlias(aliasTmp, p_Error_رمز)) {
            uint32_t conv_عدد = ڈھونڈو_Converter(aliasTmp, شامل_ہے_Option, p_Error_رمز);
            اگر (conv_عدد < g_Main_جدول.converter_List_سائز) {
                واپس GET_STRING(g_Main_جدول.converterList[conv_عدد]);
            }
            /* else converter not found */
        } ورنہ {
            توڑو;
        }
    }

    واپس خالی_اشاریہ;
}

U_CDECL_BEGIN


ساکن int32_t U_CALLCONV
ucnv_io_گنتی_Standard_Aliases(UEnumeration *enumerator, UError_رمز * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_انحراف = myContext->list_انحراف;

    اگر (list_انحراف) {
        value = g_Main_جدول.taggedAliasLists[list_انحراف];
    }
    واپس value;
}

ساکن ثابت حرف * U_CALLCONV
ucnv_io_اگلا_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* نتیجہ_لمبائی,
                            UError_رمز * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_انحراف = myContext->list_انحراف;

    اگر (list_انحراف) {
        uint32_t list_گنتی = g_Main_جدول.taggedAliasLists[list_انحراف];
        ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_انحراف + 1;

        اگر (myContext->listIdx < list_گنتی) {
            ثابت حرف *myStr = GET_STRING(currList[myContext->listIdx++]);
            اگر (نتیجہ_لمبائی) {
                *نتیجہ_لمبائی = (int32_t)uprv_strlen(myStr);
            }
            واپس myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    اگر (نتیجہ_لمبائی) {
        *نتیجہ_لمبائی = 0;
    }
    واپس خالی_اشاریہ;
}

ساکن خالی U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UError_رمز * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

ساکن خالی U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_انتہا

/* Enumerate the aliases for the specified converter and standard tag */
ساکن ثابت UEnumeration gEnumAliases = {
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_io_closeUEnumeration,
    ucnv_io_گنتی_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_اگلا_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_Standard_نام(ثابت حرف *convName,
                       ثابت حرف *standard,
                       UError_رمز *p_Error_رمز)
{
    UEnumeration *myEnum = خالی_اشاریہ;
    اگر (have_Alias_ڈیٹا(p_Error_رمز) && isAlias(convName, p_Error_رمز)) {
        uint32_t list_انحراف = ڈھونڈو_Tagged_Alias_Lists_انحراف(convName, standard, p_Error_رمز);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        اگر (list_انحراف < g_Main_جدول.tagged_Alias_Lists_سائز) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
            اگر (myEnum == خالی_اشاریہ) {
                *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس خالی_اشاریہ;
            }
            uprv_memcpy(myEnum, &gEnumAliases, حجم(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(حجم(UAliasContext)));
            اگر (myContext == خالی_اشاریہ) {
                *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
                uprv_free(myEnum);
                واپس خالی_اشاریہ;
            }
            myContext->list_انحراف = list_انحراف;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    واپس myEnum;
}

ساکن uint16_t
ucnv_io_گنتی_Aliases(ثابت حرف *alias, UError_رمز *p_Error_رمز) {
    اگر(have_Alias_ڈیٹا(p_Error_رمز) && isAlias(alias, p_Error_رمز)) {
        uint32_t conv_عدد = ڈھونڈو_Converter(alias, خالی_اشاریہ, p_Error_رمز);
        اگر (conv_عدد < g_Main_جدول.converter_List_سائز) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_انحراف = g_Main_جدول.taggedAliasArray[(g_Main_جدول.لیبل_List_سائز - 1)*g_Main_جدول.converter_List_سائز + conv_عدد];

            اگر (list_انحراف) {
                واپس g_Main_جدول.taggedAliasLists[list_انحراف];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    واپس 0;
}

ساکن uint16_t
ucnv_io_getAliases(ثابت حرف *alias, uint16_t آغاز, ثابت حرف **aliases, UError_رمز *p_Error_رمز) {
    اگر(have_Alias_ڈیٹا(p_Error_رمز) && isAlias(alias, p_Error_رمز)) {
        uint32_t currAlias;
        uint32_t conv_عدد = ڈھونڈو_Converter(alias, خالی_اشاریہ, p_Error_رمز);
        اگر (conv_عدد < g_Main_جدول.converter_List_سائز) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_انحراف = g_Main_جدول.taggedAliasArray[(g_Main_جدول.لیبل_List_سائز - 1)*g_Main_جدول.converter_List_سائز + conv_عدد];

            اگر (list_انحراف) {
                uint32_t list_گنتی = g_Main_جدول.taggedAliasLists[list_انحراف];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_انحراف + 1;

                برائے (currAlias = آغاز; currAlias < list_گنتی; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    واپس 0;
}

ساکن ثابت حرف *
ucnv_io_getAlias(ثابت حرف *alias, uint16_t n, UError_رمز *p_Error_رمز) {
    اگر(have_Alias_ڈیٹا(p_Error_رمز) && isAlias(alias, p_Error_رمز)) {
        uint32_t conv_عدد = ڈھونڈو_Converter(alias, خالی_اشاریہ, p_Error_رمز);
        اگر (conv_عدد < g_Main_جدول.converter_List_سائز) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_انحراف = g_Main_جدول.taggedAliasArray[(g_Main_جدول.لیبل_List_سائز - 1)*g_Main_جدول.converter_List_سائز + conv_عدد];

            اگر (list_انحراف) {
                uint32_t list_گنتی = g_Main_جدول.taggedAliasLists[list_انحراف];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_انحراف + 1;

                اگر (n < list_گنتی)  {
                    واپس GET_STRING(currList[n]);
                }
                *p_Error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    واپس خالی_اشاریہ;
}

ساکن uint16_t
ucnv_io_گنتی_Standards(UError_رمز *p_Error_رمز) {
    اگر (have_Alias_ڈیٹا(p_Error_رمز)) {
        /* Don't include the empty list */
        واپس static_cast<uint16_t>(g_Main_جدول.لیبل_List_سائز - UCNV_عدد_HIDDEN_لیبل_فہرست);
    }

    واپس 0;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandard(uint16_t n, UError_رمز *p_Error_رمز) {
    اگر (have_Alias_ڈیٹا(p_Error_رمز)) {
        اگر (n < g_Main_جدول.لیبل_List_سائز - UCNV_عدد_HIDDEN_لیبل_فہرست) {
            واپس GET_STRING(g_Main_جدول.لیبل_List[n]);
        }
        *p_Error_رمز = U_اشاریہ_OUTOFBOUNDS_ERROR;
    }

    واپس خالی_اشاریہ;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandardName(ثابت حرف *alias, ثابت حرف *standard, UError_رمز *p_Error_رمز) {
    اگر (have_Alias_ڈیٹا(p_Error_رمز) && isAlias(alias, p_Error_رمز)) {
        uint32_t list_انحراف = ڈھونڈو_Tagged_Alias_Lists_انحراف(alias, standard, p_Error_رمز);

        اگر (0 < list_انحراف && list_انحراف < g_Main_جدول.tagged_Alias_Lists_سائز) {
            ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_انحراف + 1;

            /* Get the preferred name from this list */
            اگر (currList[0]) {
                واپس GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    واپس خالی_اشاریہ;
}

U_CAPI uint16_t U_EXPORT2
ucnv_گنتی_Aliases(ثابت حرف *alias, UError_رمز *p_Error_رمز)
{
    واپس ucnv_io_گنتی_Aliases(alias, p_Error_رمز);
}


U_CAPI ثابت حرف* U_EXPORT2
ucnv_getAlias(ثابت حرف *alias, uint16_t n, UError_رمز *p_Error_رمز)
{
    واپس ucnv_io_getAlias(alias, n, p_Error_رمز);
}

U_CAPI خالی U_EXPORT2
ucnv_getAliases(ثابت حرف *alias, ثابت حرف **aliases, UError_رمز *p_Error_رمز)
{
    ucnv_io_getAliases(alias, 0, aliases, p_Error_رمز);
}

U_CAPI uint16_t U_EXPORT2
ucnv_گنتی_Standards()
{
    UError_رمز err = U_ZERO_ERROR;
    واپس ucnv_io_گنتی_Standards(&err);
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getCanonicalName(ثابت حرف *alias, ثابت حرف *standard, UError_رمز *p_Error_رمز) {
    اگر (have_Alias_ڈیٹا(p_Error_رمز) && isAlias(alias, p_Error_رمز)) {
        uint32_t conv_عدد = ڈھونڈو_Tagged_Converter_عدد(alias, standard, p_Error_رمز);

        اگر (conv_عدد < g_Main_جدول.converter_List_سائز) {
            واپس GET_STRING(g_Main_جدول.converterList[conv_عدد]);
        }
    }

    واپس خالی_اشاریہ;
}

U_CDECL_BEGIN


ساکن int32_t U_CALLCONV
ucnv_io_گنتی_All_Converters(UEnumeration * /*enumerator*/, UError_رمز * /*pErrorCode*/) {
    واپس g_Main_جدول.converter_List_سائز;
}

ساکن ثابت حرف * U_CALLCONV
ucnv_io_اگلا_All_Converters(UEnumeration *enumerator,
                            int32_t* نتیجہ_لمبائی,
                            UError_رمز * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    اگر (*myContext < g_Main_جدول.converter_List_سائز) {
        ثابت حرف *myStr = GET_STRING(g_Main_جدول.converterList[(*myContext)++]);
        اگر (نتیجہ_لمبائی) {
            *نتیجہ_لمبائی = (int32_t)uprv_strlen(myStr);
        }
        واپس myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    اگر (نتیجہ_لمبائی) {
        *نتیجہ_لمبائی = 0;
    }
    واپس خالی_اشاریہ;
}

ساکن خالی U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UError_رمز * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_انتہا
ساکن ثابت UEnumeration gEnumAllConverters = {
    خالی_اشاریہ,
    خالی_اشاریہ,
    ucnv_io_closeUEnumeration,
    ucnv_io_گنتی_All_Converters,
    uenum_unextDefault,
    ucnv_io_اگلا_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_All_نام(UError_رمز *p_Error_رمز) {
    UEnumeration *myEnum = خالی_اشاریہ;
    اگر (have_Alias_ڈیٹا(p_Error_رمز)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
        اگر (myEnum == خالی_اشاریہ) {
            *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس خالی_اشاریہ;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, حجم(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(حجم(uint16_t)));
        اگر (myContext == خالی_اشاریہ) {
            *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
            uprv_free(myEnum);
            واپس خالی_اشاریہ;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    واپس myEnum;
}

U_CAPI uint16_t
ucnv_io_گنتی_معلوم_Converters(UError_رمز *p_Error_رمز) {
    اگر (have_Alias_ڈیٹا(p_Error_رمز)) {
        واپس (uint16_t)g_Main_جدول.converter_List_سائز;
    }
    واپس 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

قسمتعریف حرف * U_CALLCONV StripForCompareFn(حرف *dst, ثابت حرف *name);
U_CDECL_انتہا


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
قسمتعریف ساخت عارضی_صف {
    uint16_t str_اشاریہ, ترتیب_دو_اشاریہ;
} عارضی_صف;

قسمتعریف ساخت عارضی_Alias_جدول {
    ثابت حرف *chars;
    عارضی_صف *صفیں;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} عارضی_Alias_جدول;

شمار {
    ڈھیر_صف_استعداد=500
};

ساکن int32_t U_CALLCONV
io_compare_صفیں(ثابت خالی *context, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    حرف stripped_بایاں[UCNV_MAX_CONVERTER_NAME_لمبائی],
         stripped_دایاں[UCNV_MAX_CONVERTER_NAME_لمبائی];

    عارضی_Alias_جدول *عارضی_جدول=(عارضی_Alias_جدول *)context;
    ثابت حرف *chars=عارضی_جدول->chars;

    واپس static_cast<int32_t>(uprv_strcmp(
        عارضی_جدول->stripForCompare(stripped_بایاں, chars + 2 * static_cast<ثابت عارضی_صف*>(بائیں)->str_اشاریہ),
        عارضی_جدول->stripForCompare(stripped_دایاں, chars + 2 * static_cast<ثابت عارضی_صف*>(دائیں)->str_اشاریہ)));
}

U_CAPI int32_t U_EXPORT2
ucnv_تبادلہ_Aliases(ثابت UDataSwapper *ds,
                 ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                 UError_رمز *p_Error_رمز) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_سائز;

    ثابت uint16_t *اندر_جدول;
    ثابت uint32_t *اندر_Section_Sizes;
    uint32_t toc[offsets_گنتی];
    uint32_t offsets[offsets_گنتی]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_لمبائی, چوٹی_انحراف;

    عارضی_صف صفیں[ڈھیر_صف_استعداد];
    uint16_t resort[ڈھیر_صف_استعداد];
    عارضی_Alias_جدول عارضی_جدول;

    /* udata_swapDataHeader checks the arguments */
    سرخی_سائز=udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
    اگر(!(
        pInfo->ڈیٹا_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->ڈیٹا_Format[1]==0x76 &&
        pInfo->ڈیٹا_Format[2]==0x41 &&
        pInfo->ڈیٹا_Format[3]==0x6c &&
        pInfo->format_نسخہ[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1],
                         pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3],
                         pInfo->format_نسخہ[0]);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    /* an alias table must contain at least the table of contents array */
    اگر(length>=0 && (length-سرخی_سائز)<4*(1+min_Toc_لمبائی)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-سرخی_سائز);
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }

    اندر_Section_Sizes=(ثابت uint32_t *)((ثابت حرف *)اندر_ڈیٹا+سرخی_سائز);
    اندر_جدول=(ثابت uint16_t *)اندر_Section_Sizes;
    uprv_memset(toc, 0, حجم(toc));
    toc[toc_لمبائی_اشاریہ]=toc_لمبائی=ds->readUInt32(اندر_Section_Sizes[toc_لمبائی_اشاریہ]);
    اگر(toc_لمبائی<min_Toc_لمبائی || offsets_گنتی<=toc_لمبائی) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_لمبائی);
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس 0;
    }

    /* read the known part of the table of contents */
    برائے(i=converter_List_اشاریہ; i<=toc_لمبائی; ++i) {
        toc[i]=ds->readUInt32(اندر_Section_Sizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, حجم(offsets));
    offsets[converter_List_اشاریہ]=2*(1+toc_لمبائی); /* count two 16-bit units per toc entry */
    برائے(i=لیبل_List_اشاریہ; i<=toc_لمبائی; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    چوٹی_انحراف=offsets[i-1]+toc[i-1];

    اگر(length>=0) {
        uint16_t *برآمد_جدول;
        ثابت uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_اشاریہ;

        اگر((length-سرخی_سائز)<(2*(int32_t)چوٹی_انحراف)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-سرخی_سائز);
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }

        برآمد_جدول=(uint16_t *)((حرف *)برآمد_ڈیٹا+سرخی_سائز);

        /* swap the entire table of contents */
        ds->تبادلہ_Array32(ds, اندر_جدول, 4*(1+toc_لمبائی), برآمد_جدول, p_Error_رمز);

        /* swap unormalized strings & normalized strings */
        ds->تبادلہ_Inv_Chars(ds, اندر_جدول+offsets[string_جدول_اشاریہ], 2*(int32_t)(toc[string_جدول_اشاریہ]+toc[normalized_String_جدول_اشاریہ]),
                             برآمد_جدول+offsets[string_جدول_اشاریہ], p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            واپس 0;
        }

        اگر(ds->اندر_Charset==ds->برآمد_Charset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->تبادلہ_Array16(ds,
                            اندر_جدول+offsets[converter_List_اشاریہ],
                            2*(int32_t)(offsets[string_جدول_اشاریہ]-offsets[converter_List_اشاریہ]),
                            برآمد_جدول+offsets[converter_List_اشاریہ],
                            p_Error_رمز);
        } ورنہ {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_اشاریہ];

            عارضی_جدول.chars=(ثابت حرف *)(برآمد_جدول+offsets[string_جدول_اشاریہ]); /* sort by outCharset */

            اگر(count<=ڈھیر_صف_استعداد) {
                عارضی_جدول.صفیں=صفیں;
                عارضی_جدول.resort=resort;
            } ورنہ {
                عارضی_جدول.صفیں=(عارضی_صف *)uprv_malloc(count*حجم(عارضی_صف)+count*2);
                اگر(عارضی_جدول.صفیں==خالی_اشاریہ) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
                    واپس 0;
                }
                عارضی_جدول.resort=(uint16_t *)(عارضی_جدول.صفیں+count);
            }

            اگر(ds->برآمد_Charset==U_ASCII_FAMILY) {
                عارضی_جدول.stripForCompare=ucnv_io_stripASCIIForCompare;
            } ورنہ /* U_EBCDIC_FAMILY */ {
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
            p=اندر_جدول+offsets[alias_List_اشاریہ];
            q=برآمد_جدول+offsets[alias_List_اشاریہ];

            p2=اندر_جدول+offsets[untagged_Conv_Array_اشاریہ];
            q2=برآمد_جدول+offsets[untagged_Conv_Array_اشاریہ];

            برائے(i=0; i<count; ++i) {
                عارضی_جدول.صفیں[i].str_اشاریہ=ds->readUInt16(p[i]);
                عارضی_جدول.صفیں[i].ترتیب_دو_اشاریہ=(uint16_t)i;
            }

            uprv_ترتیب_دو_Array(عارضی_جدول.صفیں, (int32_t)count, حجم(عارضی_صف),
                           io_compare_صفیں, &عارضی_جدول,
                           جھوٹ, p_Error_رمز);

            اگر(U_SUCCESS(*p_Error_رمز)) {
                /* copy/swap/permutate items */
                اگر(p!=q) {
                    برائے(i=0; i<count; ++i) {
                        old_اشاریہ=عارضی_جدول.صفیں[i].ترتیب_دو_اشاریہ;
                        ds->تبادلہ_Array16(ds, p+old_اشاریہ, 2, q+i, p_Error_رمز);
                        ds->تبادلہ_Array16(ds, p2+old_اشاریہ, 2, q2+i, p_Error_رمز);
                    }
                } ورنہ {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=عارضی_جدول.resort;

                    برائے(i=0; i<count; ++i) {
                        old_اشاریہ=عارضی_جدول.صفیں[i].ترتیب_دو_اشاریہ;
                        ds->تبادلہ_Array16(ds, p+old_اشاریہ, 2, r+i, p_Error_رمز);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    برائے(i=0; i<count; ++i) {
                        old_اشاریہ=عارضی_جدول.صفیں[i].ترتیب_دو_اشاریہ;
                        ds->تبادلہ_Array16(ds, p2+old_اشاریہ, 2, r+i, p_Error_رمز);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            اگر(عارضی_جدول.صفیں!=صفیں) {
                uprv_free(عارضی_جدول.صفیں);
            }

            اگر(U_FAILURE(*p_Error_رمز)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                واپس 0;
            }

            /* swap remaining 16-bit values */
            ds->تبادلہ_Array16(ds,
                            اندر_جدول+offsets[converter_List_اشاریہ],
                            2*(int32_t)(offsets[alias_List_اشاریہ]-offsets[converter_List_اشاریہ]),
                            برآمد_جدول+offsets[converter_List_اشاریہ],
                            p_Error_رمز);
            ds->تبادلہ_Array16(ds,
                            اندر_جدول+offsets[tagged_Alias_Array_اشاریہ],
                            2*(int32_t)(offsets[string_جدول_اشاریہ]-offsets[tagged_Alias_Array_اشاریہ]),
                            برآمد_جدول+offsets[tagged_Alias_Array_اشاریہ],
                            p_Error_رمز);
        }
    }

    واپس سرخی_سائز+2*(int32_t)چوٹی_انحراف;
}

#اگر_ختم


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

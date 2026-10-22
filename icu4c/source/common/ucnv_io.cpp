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
    uint32_t list_دننه_واټن;
    uint32_t listIdx;
} UAliasContext;

ساكن ثابت حرف معلومات_NAME[] = "cnvalias";
ساكن ثابت حرف معلومات_TYPE[] = "icu";

ساكن UData_حافظه *g_Alias_معلومات=تش_اشاره;
ساكن icu::UInitOnce g_Alias_معلومات_Init_Once {};

تعداد {
    toc_اوږدتیا_شاخص=0,
    converter_List_شاخص=1,
    نښان_List_شاخص=2,
    alias_List_شاخص=3,
    untagged_Conv_Array_شاخص=4,
    tagged_Alias_Array_شاخص=5,
    tagged_Alias_Lists_شاخص=6,
    جدول_Options_شاخص=7,
    string_جدول_شاخص=8,
    normalized_String_جدول_شاخص=9,
    offsets_شمېر,    /* length of the swapper's temporary offsets[] */
    min_Toc_اوږدتیا=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

ساكن ثابت UConverterAliasOptions default_جدول_Options = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
ساكن UConverterAlias g_Main_جدول;

#تعریف_کړه GET_STRING(idx) (ثابت حرف *)(g_Main_جدول.string_جدول + (idx))
#تعریف_کړه GET_NORMALIZED_STRING(idx) (ثابت حرف *)(g_Main_جدول.normalized_String_جدول + (idx))

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
        pInfo->format_نسخه[0]==3;
}

ساكن UBool U_CALLCONV ucnv_io_cleanup()
{
    إذا (g_Alias_معلومات) {
        udata_close(g_Alias_معلومات);
        g_Alias_معلومات = تش_اشاره;
    }
    g_Alias_معلومات_Init_Once.reset();

    uprv_memset(&g_Main_جدول, 0, حجم(g_Main_جدول));

    أرجع ریښتیا;                   /* Everything was cleaned up */
}

ساكن فراغ U_CALLCONV init_Alias_معلومات(UError_کوډ &err_کوډ) {
    UData_حافظه *معلومات;
    ثابت uint16_t *جدول;
    ثابت uint32_t *sectionSizes;
    uint32_t جدول_پیل;
    uint32_t curr_دننه_واټن;
    int32_t اندازه_د_معلومات;
    int32_t اندازه_د_TOC;

    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_معلومات == تش_اشاره);
    معلومات = udata_openChoice(تش_اشاره, معلومات_TYPE, معلومات_NAME, isAcceptable, تش_اشاره, &err_کوډ);
    إذا (U_FAILURE(err_کوډ)) {
        أرجع;
    }

    sectionSizes = static_cast<ثابت uint32_t*>(udata_get_حافظه(معلومات));
    int32_t معلومات_اوږدتیا = udata_get_اوږدتیا(معلومات); // This is the length minus the UDataInfo size
    UBool is_معلومات_اوږدتیا_معلوم = معلومات_اوږدتیا >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    إذا (is_معلومات_اوږدتیا_معلوم && معلومات_اوږدتیا <= int32_t(حجم(sectionSizes[0]))) {
        // We don't even have a TOC!
        اذهب invalidFormat;
    }
    جدول = reinterpret_cast<ثابت uint16_t*>(sectionSizes);
    جدول_پیل = sectionSizes[0];
    اندازه_د_TOC = int32_t((جدول_پیل + 1) * حجم(sectionSizes[0]));
    إذا (جدول_پیل < min_Toc_اوږدتیا || (is_معلومات_اوږدتیا_معلوم && معلومات_اوږدتیا <= اندازه_د_TOC)) {
        // We don't have a whole TOC!
        اذهب invalidFormat;
    }
    g_Alias_معلومات = معلومات;

    g_Main_جدول.converter_List_اندازه      = sectionSizes[1];
    g_Main_جدول.نښان_List_اندازه            = sectionSizes[2];
    g_Main_جدول.alias_List_اندازه          = sectionSizes[3];
    g_Main_جدول.untagged_Conv_Array_اندازه  = sectionSizes[4];
    g_Main_جدول.tagged_Alias_Array_اندازه   = sectionSizes[5];
    g_Main_جدول.tagged_Alias_Lists_اندازه   = sectionSizes[6];
    g_Main_جدول.option_جدول_اندازه        = sectionSizes[7];
    g_Main_جدول.string_جدول_اندازه        = sectionSizes[8];

    إذا (جدول_پیل > min_Toc_اوږدتیا) {
        g_Main_جدول.normalized_String_جدول_اندازه = sectionSizes[9];
    }

    اندازه_د_معلومات = اندازه_د_TOC;
    لكل (uint32_t section = 1; section <= جدول_پیل; section++) {
        اندازه_د_معلومات += sectionSizes[section] * حجم(جدول[0]);
    }
    إذا (is_معلومات_اوږدتیا_معلوم && معلومات_اوږدتیا < اندازه_د_معلومات) {
        // Truncated file!
        اذهب invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    curr_دننه_واټن = (جدول_پیل + 1) * (حجم(uint32_t)/حجم(uint16_t));
    g_Main_جدول.converterList = جدول + curr_دننه_واټن;

    curr_دننه_واټن += g_Main_جدول.converter_List_اندازه;
    g_Main_جدول.نښان_List = جدول + curr_دننه_واټن;

    curr_دننه_واټن += g_Main_جدول.نښان_List_اندازه;
    g_Main_جدول.aliasList = جدول + curr_دننه_واټن;

    curr_دننه_واټن += g_Main_جدول.alias_List_اندازه;
    g_Main_جدول.untaggedConvArray = جدول + curr_دننه_واټن;

    curr_دننه_واټن += g_Main_جدول.untagged_Conv_Array_اندازه;
    g_Main_جدول.taggedAliasArray = جدول + curr_دننه_واټن;

    /* aliasLists is a 1's based array, but it has a padding character */
    curr_دننه_واټن += g_Main_جدول.tagged_Alias_Array_اندازه;
    g_Main_جدول.taggedAliasLists = جدول + curr_دننه_واټن;

    curr_دننه_واټن += g_Main_جدول.tagged_Alias_Lists_اندازه;
    إذا (g_Main_جدول.option_جدول_اندازه > 0
        && reinterpret_cast<ثابت UConverterAliasOptions*>(جدول + curr_دننه_واټن)->stringNormalizationType < UCNV_IO_NORM_TYPE_شمېر)
    {
        /* Faster table */
        g_Main_جدول.option_جدول = reinterpret_cast<ثابت UConverterAliasOptions*>(جدول + curr_دننه_واټن);
    }
    وإلا {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        g_Main_جدول.option_جدول = &default_جدول_Options;
    }

    curr_دننه_واټن += g_Main_جدول.option_جدول_اندازه;
    g_Main_جدول.string_جدول = جدول + curr_دننه_واټن;

    curr_دننه_واټن += g_Main_جدول.string_جدول_اندازه;
    g_Main_جدول.normalized_String_جدول = ((g_Main_جدول.option_جدول->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? g_Main_جدول.string_جدول : (جدول + curr_دننه_واټن));

    أرجع;

invalidFormat:
    err_کوډ = U_INVALID_FORMAT_ERROR;
    udata_close(معلومات);
}


ساكن UBool
have_Alias_معلومات(UError_کوډ *p_Error_کوډ) {
    umtx_initOnce(g_Alias_معلومات_Init_Once, &init_Alias_معلومات, *p_Error_کوډ);
    أرجع U_SUCCESS(*p_Error_کوډ);
}

ساكن مضمن UBool
isAlias(ثابت حرف *alias, UError_کوډ *p_Error_کوډ) {
    إذا(alias==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع خطأ;
    }
    أرجع *alias != 0;
}

ساكن uint32_t get_نښان_شمېره(ثابت حرف *tagname) {
    إذا (g_Main_جدول.نښان_List) {
        uint32_t نښان_عدد;
        لكل (نښان_عدد = 0; نښان_عدد < g_Main_جدول.نښان_List_اندازه; نښان_عدد++) {
            إذا (!uprv_stricmp(GET_STRING(g_Main_جدول.نښان_List[نښان_عدد]), tagname)) {
                أرجع نښان_عدد;
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

#تعریف_کړه GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

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

#تعریف_کړه GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#إذا U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#وإلا
#   error U_CHARSET_FAMILY is not valid
#د_که_پای


/* @see ucnv_compareNames */
U_CAPI حرف* U_CALLCONV
ucnv_io_stripASCIIForCompare(حرف* dst U_LIFETIME_BOUND, ثابت حرف* name) {
    حرف *dstItr = dst;
    uint8_t type, بل_Type;
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
                بل_Type = GET_ASCII_TYPE(*name);
                إذا (بل_Type == ZERO || بل_Type == NONZERO) {
                    استمر; /* ignore leading zero before another digit */
                }
            }
            اكسر;
        حالة NONZERO:
            afterDigit = ریښتیا;
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
    uint8_t type, بل_Type;
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
                بل_Type = GET_EBCDIC_TYPE(*name);
                إذا (بل_Type == ZERO || بل_Type == NONZERO) {
                    استمر; /* ignore leading zero before another digit */
                }
            }
            اكسر;
        حالة NONZERO:
            afterDigit = ریښتیا;
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
ucnv_compare_نومونه(ثابت حرف *name1, ثابت حرف *name2) {
    صحيح rc;
    uint8_t type, بل_Type;
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
                    بل_Type = GET_CHAR_TYPE(*name1);
                    إذا (بل_Type == ZERO || بل_Type == NONZERO) {
                        استمر; /* ignore leading zero before another digit */
                    }
                }
                اكسر;
            حالة NONZERO:
                afterDigit1 = ریښتیا;
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
                    بل_Type = GET_CHAR_TYPE(*name2);
                    إذا (بل_Type == ZERO || بل_Type == NONZERO) {
                        استمر; /* ignore leading zero before another digit */
                    }
                }
                اكسر;
            حالة NONZERO:
                afterDigit2 = ریښتیا;
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
        rc = (صحيح)(بینښه حرف)c1 - (صحيح)(بینښه حرف)c2;
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
پیدا_کړه_Converter(ثابت حرف *alias, UBool *لري_Option, UError_کوډ *p_Error_کوډ) {
    uint32_t mid, پیل, حد;
    uint32_t وروستی_Mid;
    صحيح پایله;
    صحيح isUnnormalized = (g_Main_جدول.option_جدول->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_اوږدتیا];

    إذا (!isUnnormalized) {
        إذا (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_اوږدتیا) {
            *p_Error_کوډ = U_بفر_ډکېدنه_ERROR;
            أرجع UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    پیل = 0;
    حد = g_Main_جدول.untagged_Conv_Array_اندازه;
    mid = حد;
    وروستی_Mid = UINT32_MAX;

    لكل (;;) {
        mid = (پیل + حد) / 2;
        إذا (وروستی_Mid == mid) {   /* Have we moved? */
            اكسر;  /* We haven't moved, and it wasn't found. */
        }
        وروستی_Mid = mid;
        إذا (isUnnormalized) {
            پایله = ucnv_compare_نومونه(alias, GET_STRING(g_Main_جدول.aliasList[mid]));
        }
        وإلا {
            پایله = uprv_strcmp(alias, GET_NORMALIZED_STRING(g_Main_جدول.aliasList[mid]));
        }

        إذا (پایله < 0) {
            حد = mid;
        } وإلا إذا (پایله > 0) {
            پیل = mid;
        } وإلا {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            إذا (g_Main_جدول.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_بټ) {
                *p_Error_کوډ = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            إذا (لري_Option) {
                UBool لري_Cnv_Option_Info = static_cast<UBool>(g_Main_جدول.option_جدول->لري_Cnv_Option_Info);
                *لري_Option = static_cast<UBool>((لري_Cnv_Option_Info
                    && ((g_Main_جدول.untaggedConvArray[mid] & UCNV_لري_OPTION_بټ) != 0))
                    || !لري_Cnv_Option_Info);
            }
            أرجع g_Main_جدول.untaggedConvArray[mid] & UCNV_CONVERTER_شاخص_ماسک;
        }
    }

    أرجع UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
ساكن مضمن UBool
is_Alias_دننه_List(ثابت حرف *alias, uint32_t list_دننه_واټن) {
    إذا (list_دننه_واټن) {
        uint32_t currAlias;
        uint32_t list_شمېر = g_Main_جدول.taggedAliasLists[list_دننه_واټن];
        /* +1 to skip listCount */
        ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_دننه_واټن + 1;
        لكل (currAlias = 0; currAlias < list_شمېر; currAlias++) {
            إذا (currList[currAlias]
                && ucnv_compare_نومونه(alias, GET_STRING(currList[currAlias]))==0)
            {
                أرجع ریښتیا;
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
پیدا_کړه_Tagged_Alias_Lists_دننه_واټن(ثابت حرف *alias, ثابت حرف *standard, UError_کوډ *p_Error_کوډ) {
    uint32_t idx;
    uint32_t list_دننه_واټن;
    uint32_t conv_عدد;
    UError_کوډ myErr = U_ZERO_ERROR;
    uint32_t نښان_عدد = get_نښان_شمېره(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_عدد = پیدا_کړه_Converter(alias, تش_اشاره, &myErr);
    إذا (myErr != U_ZERO_ERROR) {
        *p_Error_کوډ = myErr;
    }

    إذا (نښان_عدد < (g_Main_جدول.نښان_List_اندازه - UCNV_عدد_HIDDEN_نښانونه) && conv_عدد < g_Main_جدول.converter_List_اندازه) {
        list_دننه_واټن = g_Main_جدول.taggedAliasArray[نښان_عدد*g_Main_جدول.converter_List_اندازه + conv_عدد];
        إذا (list_دننه_واټن && g_Main_جدول.taggedAliasLists[list_دننه_واټن + 1]) {
            أرجع list_دننه_واټن;
        }
        إذا (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            لكل (idx = 0; idx < g_Main_جدول.tagged_Alias_Array_اندازه; idx++) {
                list_دننه_واټن = g_Main_جدول.taggedAliasArray[idx];
                إذا (list_دننه_واټن && is_Alias_دننه_List(alias, list_دننه_واټن)) {
                    uint32_t curr_نښان_عدد = idx/g_Main_جدول.converter_List_اندازه;
                    uint32_t curr_Conv_عدد = (idx - curr_نښان_عدد*g_Main_جدول.converter_List_اندازه);
                    uint32_t لنډمهاله_List_دننه_واټن = g_Main_جدول.taggedAliasArray[نښان_عدد*g_Main_جدول.converter_List_اندازه + curr_Conv_عدد];
                    إذا (لنډمهاله_List_دننه_واټن && g_Main_جدول.taggedAliasLists[لنډمهاله_List_دننه_واټن + 1]) {
                        أرجع لنډمهاله_List_دننه_واټن;
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
پیدا_کړه_Tagged_Converter_عدد(ثابت حرف *alias, ثابت حرف *standard, UError_کوډ *p_Error_کوډ) {
    uint32_t idx;
    uint32_t list_دننه_واټن;
    uint32_t conv_عدد;
    UError_کوډ myErr = U_ZERO_ERROR;
    uint32_t نښان_عدد = get_نښان_شمېره(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_عدد = پیدا_کړه_Converter(alias, تش_اشاره, &myErr);
    إذا (myErr != U_ZERO_ERROR) {
        *p_Error_کوډ = myErr;
    }

    إذا (نښان_عدد < (g_Main_جدول.نښان_List_اندازه - UCNV_عدد_HIDDEN_نښانونه) && conv_عدد < g_Main_جدول.converter_List_اندازه) {
        list_دننه_واټن = g_Main_جدول.taggedAliasArray[نښان_عدد*g_Main_جدول.converter_List_اندازه + conv_عدد];
        إذا (list_دننه_واټن && is_Alias_دننه_List(alias, list_دننه_واټن)) {
            أرجع conv_عدد;
        }
        إذا (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t conv_پیل = (نښان_عدد)*g_Main_جدول.converter_List_اندازه;
            uint32_t conv_حد = (نښان_عدد+1)*g_Main_جدول.converter_List_اندازه;
            لكل (idx = conv_پیل; idx < conv_حد; idx++) {
                list_دننه_واټن = g_Main_جدول.taggedAliasArray[idx];
                إذا (list_دننه_واټن && is_Alias_دننه_List(alias, list_دننه_واټن)) {
                    أرجع idx-conv_پیل;
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
ucnv_io_getConverterName(ثابت حرف *alias, UBool *لري_Option, UError_کوډ *p_Error_کوډ) {
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
        إذا(have_Alias_معلومات(p_Error_کوډ) && isAlias(aliasTmp, p_Error_کوډ)) {
            uint32_t conv_عدد = پیدا_کړه_Converter(aliasTmp, لري_Option, p_Error_کوډ);
            إذا (conv_عدد < g_Main_جدول.converter_List_اندازه) {
                أرجع GET_STRING(g_Main_جدول.converterList[conv_عدد]);
            }
            /* else converter not found */
        } وإلا {
            اكسر;
        }
    }

    أرجع تش_اشاره;
}

U_CDECL_BEGIN


ساكن int32_t U_CALLCONV
ucnv_io_شمېر_Standard_Aliases(UEnumeration *enumerator, UError_کوډ * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_دننه_واټن = myContext->list_دننه_واټن;

    إذا (list_دننه_واټن) {
        value = g_Main_جدول.taggedAliasLists[list_دننه_واټن];
    }
    أرجع value;
}

ساكن ثابت حرف * U_CALLCONV
ucnv_io_بل_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* پایله_اوږدتیا,
                            UError_کوډ * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_دننه_واټن = myContext->list_دننه_واټن;

    إذا (list_دننه_واټن) {
        uint32_t list_شمېر = g_Main_جدول.taggedAliasLists[list_دننه_واټن];
        ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_دننه_واټن + 1;

        إذا (myContext->listIdx < list_شمېر) {
            ثابت حرف *myStr = GET_STRING(currList[myContext->listIdx++]);
            إذا (پایله_اوږدتیا) {
                *پایله_اوږدتیا = (int32_t)uprv_strlen(myStr);
            }
            أرجع myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    إذا (پایله_اوږدتیا) {
        *پایله_اوږدتیا = 0;
    }
    أرجع تش_اشاره;
}

ساكن فراغ U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UError_کوډ * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

ساكن فراغ U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_پای_ټکی

/* Enumerate the aliases for the specified converter and standard tag */
ساكن ثابت UEnumeration gEnumAliases = {
    تش_اشاره,
    تش_اشاره,
    ucnv_io_closeUEnumeration,
    ucnv_io_شمېر_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_بل_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_Standard_نومونه(ثابت حرف *convName,
                       ثابت حرف *standard,
                       UError_کوډ *p_Error_کوډ)
{
    UEnumeration *myEnum = تش_اشاره;
    إذا (have_Alias_معلومات(p_Error_کوډ) && isAlias(convName, p_Error_کوډ)) {
        uint32_t list_دننه_واټن = پیدا_کړه_Tagged_Alias_Lists_دننه_واټن(convName, standard, p_Error_کوډ);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        إذا (list_دننه_واټن < g_Main_جدول.tagged_Alias_Lists_اندازه) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
            إذا (myEnum == تش_اشاره) {
                *p_Error_کوډ = U_حافظه_ویش_ERROR;
                أرجع تش_اشاره;
            }
            uprv_memcpy(myEnum, &gEnumAliases, حجم(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(حجم(UAliasContext)));
            إذا (myContext == تش_اشاره) {
                *p_Error_کوډ = U_حافظه_ویش_ERROR;
                uprv_free(myEnum);
                أرجع تش_اشاره;
            }
            myContext->list_دننه_واټن = list_دننه_واټن;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    أرجع myEnum;
}

ساكن uint16_t
ucnv_io_شمېر_Aliases(ثابت حرف *alias, UError_کوډ *p_Error_کوډ) {
    إذا(have_Alias_معلومات(p_Error_کوډ) && isAlias(alias, p_Error_کوډ)) {
        uint32_t conv_عدد = پیدا_کړه_Converter(alias, تش_اشاره, p_Error_کوډ);
        إذا (conv_عدد < g_Main_جدول.converter_List_اندازه) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_دننه_واټن = g_Main_جدول.taggedAliasArray[(g_Main_جدول.نښان_List_اندازه - 1)*g_Main_جدول.converter_List_اندازه + conv_عدد];

            إذا (list_دننه_واټن) {
                أرجع g_Main_جدول.taggedAliasLists[list_دننه_واټن];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    أرجع 0;
}

ساكن uint16_t
ucnv_io_getAliases(ثابت حرف *alias, uint16_t پیل, ثابت حرف **aliases, UError_کوډ *p_Error_کوډ) {
    إذا(have_Alias_معلومات(p_Error_کوډ) && isAlias(alias, p_Error_کوډ)) {
        uint32_t currAlias;
        uint32_t conv_عدد = پیدا_کړه_Converter(alias, تش_اشاره, p_Error_کوډ);
        إذا (conv_عدد < g_Main_جدول.converter_List_اندازه) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_دننه_واټن = g_Main_جدول.taggedAliasArray[(g_Main_جدول.نښان_List_اندازه - 1)*g_Main_جدول.converter_List_اندازه + conv_عدد];

            إذا (list_دننه_واټن) {
                uint32_t list_شمېر = g_Main_جدول.taggedAliasLists[list_دننه_واټن];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_دننه_واټن + 1;

                لكل (currAlias = پیل; currAlias < list_شمېر; currAlias++) {
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
ucnv_io_getAlias(ثابت حرف *alias, uint16_t n, UError_کوډ *p_Error_کوډ) {
    إذا(have_Alias_معلومات(p_Error_کوډ) && isAlias(alias, p_Error_کوډ)) {
        uint32_t conv_عدد = پیدا_کړه_Converter(alias, تش_اشاره, p_Error_کوډ);
        إذا (conv_عدد < g_Main_جدول.converter_List_اندازه) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_دننه_واټن = g_Main_جدول.taggedAliasArray[(g_Main_جدول.نښان_List_اندازه - 1)*g_Main_جدول.converter_List_اندازه + conv_عدد];

            إذا (list_دننه_واټن) {
                uint32_t list_شمېر = g_Main_جدول.taggedAliasLists[list_دننه_واټن];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_دننه_واټن + 1;

                إذا (n < list_شمېر)  {
                    أرجع GET_STRING(currList[n]);
                }
                *p_Error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    أرجع تش_اشاره;
}

ساكن uint16_t
ucnv_io_شمېر_Standards(UError_کوډ *p_Error_کوډ) {
    إذا (have_Alias_معلومات(p_Error_کوډ)) {
        /* Don't include the empty list */
        أرجع static_cast<uint16_t>(g_Main_جدول.نښان_List_اندازه - UCNV_عدد_HIDDEN_نښانونه);
    }

    أرجع 0;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandard(uint16_t n, UError_کوډ *p_Error_کوډ) {
    إذا (have_Alias_معلومات(p_Error_کوډ)) {
        إذا (n < g_Main_جدول.نښان_List_اندازه - UCNV_عدد_HIDDEN_نښانونه) {
            أرجع GET_STRING(g_Main_جدول.نښان_List[n]);
        }
        *p_Error_کوډ = U_شاخص_OUTOFBOUNDS_ERROR;
    }

    أرجع تش_اشاره;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandardName(ثابت حرف *alias, ثابت حرف *standard, UError_کوډ *p_Error_کوډ) {
    إذا (have_Alias_معلومات(p_Error_کوډ) && isAlias(alias, p_Error_کوډ)) {
        uint32_t list_دننه_واټن = پیدا_کړه_Tagged_Alias_Lists_دننه_واټن(alias, standard, p_Error_کوډ);

        إذا (0 < list_دننه_واټن && list_دننه_واټن < g_Main_جدول.tagged_Alias_Lists_اندازه) {
            ثابت uint16_t *currList = g_Main_جدول.taggedAliasLists + list_دننه_واټن + 1;

            /* Get the preferred name from this list */
            إذا (currList[0]) {
                أرجع GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    أرجع تش_اشاره;
}

U_CAPI uint16_t U_EXPORT2
ucnv_شمېر_Aliases(ثابت حرف *alias, UError_کوډ *p_Error_کوډ)
{
    أرجع ucnv_io_شمېر_Aliases(alias, p_Error_کوډ);
}


U_CAPI ثابت حرف* U_EXPORT2
ucnv_getAlias(ثابت حرف *alias, uint16_t n, UError_کوډ *p_Error_کوډ)
{
    أرجع ucnv_io_getAlias(alias, n, p_Error_کوډ);
}

U_CAPI فراغ U_EXPORT2
ucnv_getAliases(ثابت حرف *alias, ثابت حرف **aliases, UError_کوډ *p_Error_کوډ)
{
    ucnv_io_getAliases(alias, 0, aliases, p_Error_کوډ);
}

U_CAPI uint16_t U_EXPORT2
ucnv_شمېر_Standards()
{
    UError_کوډ err = U_ZERO_ERROR;
    أرجع ucnv_io_شمېر_Standards(&err);
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getCanonicalName(ثابت حرف *alias, ثابت حرف *standard, UError_کوډ *p_Error_کوډ) {
    إذا (have_Alias_معلومات(p_Error_کوډ) && isAlias(alias, p_Error_کوډ)) {
        uint32_t conv_عدد = پیدا_کړه_Tagged_Converter_عدد(alias, standard, p_Error_کوډ);

        إذا (conv_عدد < g_Main_جدول.converter_List_اندازه) {
            أرجع GET_STRING(g_Main_جدول.converterList[conv_عدد]);
        }
    }

    أرجع تش_اشاره;
}

U_CDECL_BEGIN


ساكن int32_t U_CALLCONV
ucnv_io_شمېر_All_Converters(UEnumeration * /*enumerator*/, UError_کوډ * /*pErrorCode*/) {
    أرجع g_Main_جدول.converter_List_اندازه;
}

ساكن ثابت حرف * U_CALLCONV
ucnv_io_بل_All_Converters(UEnumeration *enumerator,
                            int32_t* پایله_اوږدتیا,
                            UError_کوډ * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    إذا (*myContext < g_Main_جدول.converter_List_اندازه) {
        ثابت حرف *myStr = GET_STRING(g_Main_جدول.converterList[(*myContext)++]);
        إذا (پایله_اوږدتیا) {
            *پایله_اوږدتیا = (int32_t)uprv_strlen(myStr);
        }
        أرجع myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    إذا (پایله_اوږدتیا) {
        *پایله_اوږدتیا = 0;
    }
    أرجع تش_اشاره;
}

ساكن فراغ U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UError_کوډ * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_پای_ټکی
ساكن ثابت UEnumeration gEnumAllConverters = {
    تش_اشاره,
    تش_اشاره,
    ucnv_io_closeUEnumeration,
    ucnv_io_شمېر_All_Converters,
    uenum_unextDefault,
    ucnv_io_بل_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_All_نومونه(UError_کوډ *p_Error_کوډ) {
    UEnumeration *myEnum = تش_اشاره;
    إذا (have_Alias_معلومات(p_Error_کوډ)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
        إذا (myEnum == تش_اشاره) {
            *p_Error_کوډ = U_حافظه_ویش_ERROR;
            أرجع تش_اشاره;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, حجم(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(حجم(uint16_t)));
        إذا (myContext == تش_اشاره) {
            *p_Error_کوډ = U_حافظه_ویش_ERROR;
            uprv_free(myEnum);
            أرجع تش_اشاره;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    أرجع myEnum;
}

U_CAPI uint16_t
ucnv_io_شمېر_معلوم_Converters(UError_کوډ *p_Error_کوډ) {
    إذا (have_Alias_معلومات(p_Error_کوډ)) {
        أرجع (uint16_t)g_Main_جدول.converter_List_اندازه;
    }
    أرجع 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

تعريفنوع حرف * U_CALLCONV StripForCompareFn(حرف *dst, ثابت حرف *name);
U_CDECL_پای_ټکی


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
تعريفنوع بنية لنډمهاله_لیکه {
    uint16_t str_شاخص, ترتیبول_شاخص;
} لنډمهاله_لیکه;

تعريفنوع بنية لنډمهاله_Alias_جدول {
    ثابت حرف *chars;
    لنډمهاله_لیکه *لیکې;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} لنډمهاله_Alias_جدول;

تعداد {
    ډېری_لیکه_ظرفیت=500
};

ساكن int32_t U_CALLCONV
io_compare_لیکې(ثابت فراغ *context, ثابت فراغ *کیڼ, ثابت فراغ *ښی) {
    حرف stripped_کیڼ[UCNV_MAX_CONVERTER_NAME_اوږدتیا],
         stripped_ښی[UCNV_MAX_CONVERTER_NAME_اوږدتیا];

    لنډمهاله_Alias_جدول *لنډمهاله_جدول=(لنډمهاله_Alias_جدول *)context;
    ثابت حرف *chars=لنډمهاله_جدول->chars;

    أرجع static_cast<int32_t>(uprv_strcmp(
        لنډمهاله_جدول->stripForCompare(stripped_کیڼ, chars + 2 * static_cast<ثابت لنډمهاله_لیکه*>(کیڼ)->str_شاخص),
        لنډمهاله_جدول->stripForCompare(stripped_ښی, chars + 2 * static_cast<ثابت لنډمهاله_لیکه*>(ښی)->str_شاخص)));
}

U_CAPI int32_t U_EXPORT2
ucnv_بدلون_Aliases(ثابت UDataSwapper *ds,
                 ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                 UError_کوډ *p_Error_کوډ) {
    ثابت UDataInfo *pInfo;
    int32_t سرلیک_اندازه;

    ثابت uint16_t *دننه_جدول;
    ثابت uint32_t *دننه_Section_Sizes;
    uint32_t toc[offsets_شمېر];
    uint32_t offsets[offsets_شمېر]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_اوږدتیا, سر_دننه_واټن;

    لنډمهاله_لیکه لیکې[ډېری_لیکه_ظرفیت];
    uint16_t resort[ډېری_لیکه_ظرفیت];
    لنډمهاله_Alias_جدول لنډمهاله_جدول;

    /* udata_swapDataHeader checks the arguments */
    سرلیک_اندازه=udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->معلومات_Format[1]==0x76 &&
        pInfo->معلومات_Format[2]==0x41 &&
        pInfo->معلومات_Format[3]==0x6c &&
        pInfo->format_نسخه[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخه[0]);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    /* an alias table must contain at least the table of contents array */
    إذا(length>=0 && (length-سرلیک_اندازه)<4*(1+min_Toc_اوږدتیا)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-سرلیک_اندازه);
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    دننه_Section_Sizes=(ثابت uint32_t *)((ثابت حرف *)دننه_معلومات+سرلیک_اندازه);
    دننه_جدول=(ثابت uint16_t *)دننه_Section_Sizes;
    uprv_memset(toc, 0, حجم(toc));
    toc[toc_اوږدتیا_شاخص]=toc_اوږدتیا=ds->readUInt32(دننه_Section_Sizes[toc_اوږدتیا_شاخص]);
    إذا(toc_اوږدتیا<min_Toc_اوږدتیا || offsets_شمېر<=toc_اوږدتیا) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_اوږدتیا);
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    /* read the known part of the table of contents */
    لكل(i=converter_List_شاخص; i<=toc_اوږدتیا; ++i) {
        toc[i]=ds->readUInt32(دننه_Section_Sizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, حجم(offsets));
    offsets[converter_List_شاخص]=2*(1+toc_اوږدتیا); /* count two 16-bit units per toc entry */
    لكل(i=نښان_List_شاخص; i<=toc_اوږدتیا; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    سر_دننه_واټن=offsets[i-1]+toc[i-1];

    إذا(length>=0) {
        uint16_t *بهر_جدول;
        ثابت uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_شاخص;

        إذا((length-سرلیک_اندازه)<(2*(int32_t)سر_دننه_واټن)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-سرلیک_اندازه);
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        بهر_جدول=(uint16_t *)((حرف *)بهر_معلومات+سرلیک_اندازه);

        /* swap the entire table of contents */
        ds->بدلون_Array32(ds, دننه_جدول, 4*(1+toc_اوږدتیا), بهر_جدول, p_Error_کوډ);

        /* swap unormalized strings & normalized strings */
        ds->بدلون_Inv_Chars(ds, دننه_جدول+offsets[string_جدول_شاخص], 2*(int32_t)(toc[string_جدول_شاخص]+toc[normalized_String_جدول_شاخص]),
                             بهر_جدول+offsets[string_جدول_شاخص], p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            أرجع 0;
        }

        إذا(ds->دننه_Charset==ds->بهر_Charset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->بدلون_Array16(ds,
                            دننه_جدول+offsets[converter_List_شاخص],
                            2*(int32_t)(offsets[string_جدول_شاخص]-offsets[converter_List_شاخص]),
                            بهر_جدول+offsets[converter_List_شاخص],
                            p_Error_کوډ);
        } وإلا {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_شاخص];

            لنډمهاله_جدول.chars=(ثابت حرف *)(بهر_جدول+offsets[string_جدول_شاخص]); /* sort by outCharset */

            إذا(count<=ډېری_لیکه_ظرفیت) {
                لنډمهاله_جدول.لیکې=لیکې;
                لنډمهاله_جدول.resort=resort;
            } وإلا {
                لنډمهاله_جدول.لیکې=(لنډمهاله_لیکه *)uprv_malloc(count*حجم(لنډمهاله_لیکه)+count*2);
                إذا(لنډمهاله_جدول.لیکې==تش_اشاره) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *p_Error_کوډ=U_حافظه_ویش_ERROR;
                    أرجع 0;
                }
                لنډمهاله_جدول.resort=(uint16_t *)(لنډمهاله_جدول.لیکې+count);
            }

            إذا(ds->بهر_Charset==U_ASCII_FAMILY) {
                لنډمهاله_جدول.stripForCompare=ucnv_io_stripASCIIForCompare;
            } وإلا /* U_EBCDIC_FAMILY */ {
                لنډمهاله_جدول.stripForCompare=ucnv_io_stripEBCDICForCompare;
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
            p=دننه_جدول+offsets[alias_List_شاخص];
            q=بهر_جدول+offsets[alias_List_شاخص];

            p2=دننه_جدول+offsets[untagged_Conv_Array_شاخص];
            q2=بهر_جدول+offsets[untagged_Conv_Array_شاخص];

            لكل(i=0; i<count; ++i) {
                لنډمهاله_جدول.لیکې[i].str_شاخص=ds->readUInt16(p[i]);
                لنډمهاله_جدول.لیکې[i].ترتیبول_شاخص=(uint16_t)i;
            }

            uprv_ترتیبول_Array(لنډمهاله_جدول.لیکې, (int32_t)count, حجم(لنډمهاله_لیکه),
                           io_compare_لیکې, &لنډمهاله_جدول,
                           خطأ, p_Error_کوډ);

            إذا(U_SUCCESS(*p_Error_کوډ)) {
                /* copy/swap/permutate items */
                إذا(p!=q) {
                    لكل(i=0; i<count; ++i) {
                        old_شاخص=لنډمهاله_جدول.لیکې[i].ترتیبول_شاخص;
                        ds->بدلون_Array16(ds, p+old_شاخص, 2, q+i, p_Error_کوډ);
                        ds->بدلون_Array16(ds, p2+old_شاخص, 2, q2+i, p_Error_کوډ);
                    }
                } وإلا {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=لنډمهاله_جدول.resort;

                    لكل(i=0; i<count; ++i) {
                        old_شاخص=لنډمهاله_جدول.لیکې[i].ترتیبول_شاخص;
                        ds->بدلون_Array16(ds, p+old_شاخص, 2, r+i, p_Error_کوډ);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    لكل(i=0; i<count; ++i) {
                        old_شاخص=لنډمهاله_جدول.لیکې[i].ترتیبول_شاخص;
                        ds->بدلون_Array16(ds, p2+old_شاخص, 2, r+i, p_Error_کوډ);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            إذا(لنډمهاله_جدول.لیکې!=لیکې) {
                uprv_free(لنډمهاله_جدول.لیکې);
            }

            إذا(U_FAILURE(*p_Error_کوډ)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                أرجع 0;
            }

            /* swap remaining 16-bit values */
            ds->بدلون_Array16(ds,
                            دننه_جدول+offsets[converter_List_شاخص],
                            2*(int32_t)(offsets[alias_List_شاخص]-offsets[converter_List_شاخص]),
                            بهر_جدول+offsets[converter_List_شاخص],
                            p_Error_کوډ);
            ds->بدلون_Array16(ds,
                            دننه_جدول+offsets[tagged_Alias_Array_شاخص],
                            2*(int32_t)(offsets[string_جدول_شاخص]-offsets[tagged_Alias_Array_شاخص]),
                            بهر_جدول+offsets[tagged_Alias_Array_شاخص],
                            p_Error_کوډ);
        }
    }

    أرجع سرلیک_اندازه+2*(int32_t)سر_دننه_واټن;
}

#د_که_پای


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

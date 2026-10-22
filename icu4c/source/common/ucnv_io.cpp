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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION

#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/udata.h"

#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uarrsort.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "udataswp.h"
#অন্তর্ভাব "udatamem.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ucnv_io.h"
#অন্তর্ভাব "uenumimp.h"
#অন্তর্ভাব "ucln_cmn.h"

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
প্রকারলেখ কাঠামো UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

স্থির ধ্রুবক অক্ষর তথ্য_NAME[] = "cnvalias";
স্থির ধ্রুবক অক্ষর তথ্য_TYPE[] = "icu";

স্থির UDataMemory *g_Alias_তথ্য=nullptr;
স্থির icu::UInitOnce g_Alias_তথ্য_Init_Once {};

গণনা {
    toc_দৈর্ঘ্য_সূচক=0,
    converter_List_সূচক=1,
    tag_List_সূচক=2,
    alias_List_সূচক=3,
    untagged_Conv_Array_সূচক=4,
    tagged_Alias_Array_সূচক=5,
    tagged_Alias_Lists_সূচক=6,
    table_Options_সূচক=7,
    string_Table_সূচক=8,
    normalized_String_Table_সূচক=9,
    offsets_গুনতি,    /* length of the swapper's temporary offsets[] */
    min_Toc_দৈর্ঘ্য=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

স্থির ধ্রুবক UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
স্থির UConverterAlias gMainTable;

#define GET_STRING(idx) (ধ্রুবক অক্ষর *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (ধ্রুবক অক্ষর *)(gMainTable.normalizedStringTable + (idx))

স্থির UBool U_CALLCONV
isAcceptable(শূন্য * /*context*/,
             ধ্রুবক অক্ষর * /*type*/, ধ্রুবক অক্ষর * /*name*/,
             ধ্রুবক UDataInfo *pInfo) {
    ফেরত
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->তথ্য_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->তথ্য_Format[1]==0x76 &&
        pInfo->তথ্য_Format[2]==0x41 &&
        pInfo->তথ্য_Format[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

স্থির UBool U_CALLCONV ucnv_io_cleanup()
{
    যদি (g_Alias_তথ্য) {
        udata_close(g_Alias_তথ্য);
        g_Alias_তথ্য = nullptr;
    }
    g_Alias_তথ্য_Init_Once.reset();

    uprv_memset(&gMainTable, 0, আকার(gMainTable));

    ফেরত সত্য;                   /* Everything was cleaned up */
}

স্থির শূন্য U_CALLCONV init_Alias_তথ্য(UErrorCode &errCode) {
    UDataMemory *তথ্য;
    ধ্রুবক uint16_t *table;
    ধ্রুবক uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t আকার_Of_তথ্য;
    int32_t আকার_Of_TOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_তথ্য == nullptr);
    তথ্য = udata_openChoice(nullptr, তথ্য_TYPE, তথ্য_NAME, isAcceptable, nullptr, &errCode);
    যদি (U_FAILURE(errCode)) {
        ফেরত;
    }

    sectionSizes = static_cast<ধ্রুবক uint32_t*>(udata_getMemory(তথ্য));
    int32_t তথ্য_দৈর্ঘ্য = udata_get_দৈর্ঘ্য(তথ্য); // This is the length minus the UDataInfo size
    UBool is_তথ্য_দৈর্ঘ্য_Known = তথ্য_দৈর্ঘ্য >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    যদি (is_তথ্য_দৈর্ঘ্য_Known && তথ্য_দৈর্ঘ্য <= int32_t(আকার(sectionSizes[0]))) {
        // We don't even have a TOC!
        যাও invalidFormat;
    }
    table = reinterpret_cast<ধ্রুবক uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    আকার_Of_TOC = int32_t((tableStart + 1) * আকার(sectionSizes[0]));
    যদি (tableStart < min_Toc_দৈর্ঘ্য || (is_তথ্য_দৈর্ঘ্য_Known && তথ্য_দৈর্ঘ্য <= আকার_Of_TOC)) {
        // We don't have a whole TOC!
        যাও invalidFormat;
    }
    g_Alias_তথ্য = তথ্য;

    gMainTable.converter_List_আকার      = sectionSizes[1];
    gMainTable.tag_List_আকার            = sectionSizes[2];
    gMainTable.alias_List_আকার          = sectionSizes[3];
    gMainTable.untagged_Conv_Array_আকার  = sectionSizes[4];
    gMainTable.tagged_Alias_Array_আকার   = sectionSizes[5];
    gMainTable.tagged_Alias_Lists_আকার   = sectionSizes[6];
    gMainTable.option_Table_আকার        = sectionSizes[7];
    gMainTable.string_Table_আকার        = sectionSizes[8];

    যদি (tableStart > min_Toc_দৈর্ঘ্য) {
        gMainTable.normalized_String_Table_আকার = sectionSizes[9];
    }

    আকার_Of_তথ্য = আকার_Of_TOC;
    জন্য (uint32_t section = 1; section <= tableStart; section++) {
        আকার_Of_তথ্য += sectionSizes[section] * আকার(table[0]);
    }
    যদি (is_তথ্য_দৈর্ঘ্য_Known && তথ্য_দৈর্ঘ্য < আকার_Of_তথ্য) {
        // Truncated file!
        যাও invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (আকার(uint32_t)/আকার(uint16_t));
    gMainTable.converterList = table + currOffset;

    currOffset += gMainTable.converter_List_আকার;
    gMainTable.tagList = table + currOffset;

    currOffset += gMainTable.tag_List_আকার;
    gMainTable.aliasList = table + currOffset;

    currOffset += gMainTable.alias_List_আকার;
    gMainTable.untaggedConvArray = table + currOffset;

    currOffset += gMainTable.untagged_Conv_Array_আকার;
    gMainTable.taggedAliasArray = table + currOffset;

    /* aliasLists is a 1's based array, but it has a padding character */
    currOffset += gMainTable.tagged_Alias_Array_আকার;
    gMainTable.taggedAliasLists = table + currOffset;

    currOffset += gMainTable.tagged_Alias_Lists_আকার;
    যদি (gMainTable.option_Table_আকার > 0
        && reinterpret_cast<ধ্রুবক UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_গুনতি)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<ধ্রুবক UConverterAliasOptions*>(table + currOffset);
    }
    অন্যথা {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.option_Table_আকার;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.string_Table_আকার;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    ফেরত;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(তথ্য);
}


স্থির UBool
have_Alias_তথ্য(UErrorCode *pErrorCode) {
    umtx_initOnce(g_Alias_তথ্য_Init_Once, &init_Alias_তথ্য, *pErrorCode);
    ফেরত U_SUCCESS(*pErrorCode);
}

স্থির অন্তঃরেখ UBool
isAlias(ধ্রুবক অক্ষর *alias, UErrorCode *pErrorCode) {
    যদি(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত মিথ্যা;
    }
    ফেরত *alias != 0;
}

স্থির uint32_t get_Tag_সংখ্যা(ধ্রুবক অক্ষর *tagname) {
    যদি (gMainTable.tagList) {
        uint32_t tag_অঙ্ক;
        জন্য (tag_অঙ্ক = 0; tag_অঙ্ক < gMainTable.tag_List_আকার; tag_অঙ্ক++) {
            যদি (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_অঙ্ক]), tagname)) {
                ফেরত tag_অঙ্ক;
            }
        }
    }

    ফেরত UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
গণনা {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
স্থির ধ্রুবক uint8_t asciiTypes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0
};

#define GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

/* character types for EBCDIC 80..FF */
স্থির ধ্রুবক uint8_t ebcdicTypes[128] = {
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0
};

#define GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#যদি U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#নতুবা_যদি U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#অন্যথা
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI অক্ষর* U_CALLCONV
ucnv_io_stripASCIIForCompare(অক্ষর* dst U_LIFETIME_BOUND, ধ্রুবক অক্ষর* name) {
    অক্ষর *dstItr = dst;
    uint8_t type, পরবর্তী_Type;
    অক্ষর c1;
    UBool afterDigit = মিথ্যা;

    যখন ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        নির্বাচন (type) {
        ক্ষেত্র UIGNORE:
            afterDigit = মিথ্যা;
            চালু; /* ignore all but letters and digits */
        ক্ষেত্র ZERO:
            যদি (!afterDigit) {
                পরবর্তী_Type = GET_ASCII_TYPE(*name);
                যদি (পরবর্তী_Type == ZERO || পরবর্তী_Type == NONZERO) {
                    চালু; /* ignore leading zero before another digit */
                }
            }
            বিরতি;
        ক্ষেত্র NONZERO:
            afterDigit = সত্য;
            বিরতি;
        পূর্বনির্ধারিত:
            c1 = (অক্ষর)type; /* lowercased letter */
            afterDigit = মিথ্যা;
            বিরতি;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ফেরত dst;
}

U_CAPI অক্ষর* U_CALLCONV
ucnv_io_stripEBCDICForCompare(অক্ষর* dst U_LIFETIME_BOUND, ধ্রুবক অক্ষর* name) {
    অক্ষর *dstItr = dst;
    uint8_t type, পরবর্তী_Type;
    অক্ষর c1;
    UBool afterDigit = মিথ্যা;

    যখন ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        নির্বাচন (type) {
        ক্ষেত্র UIGNORE:
            afterDigit = মিথ্যা;
            চালু; /* ignore all but letters and digits */
        ক্ষেত্র ZERO:
            যদি (!afterDigit) {
                পরবর্তী_Type = GET_EBCDIC_TYPE(*name);
                যদি (পরবর্তী_Type == ZERO || পরবর্তী_Type == NONZERO) {
                    চালু; /* ignore leading zero before another digit */
                }
            }
            বিরতি;
        ক্ষেত্র NONZERO:
            afterDigit = সত্য;
            বিরতি;
        পূর্বনির্ধারিত:
            c1 = (অক্ষর)type; /* lowercased letter */
            afterDigit = মিথ্যা;
            বিরতি;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ফেরত dst;
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
U_CAPI পূর্ণসংখ্যা U_EXPORT2
ucnv_compareNames(ধ্রুবক অক্ষর *name1, ধ্রুবক অক্ষর *name2) {
    পূর্ণসংখ্যা rc;
    uint8_t type, পরবর্তী_Type;
    অক্ষর c1, c2;
    UBool afterDigit1 = মিথ্যা, afterDigit2 = মিথ্যা;

    জন্য (;;) {
        যখন ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            নির্বাচন (type) {
            ক্ষেত্র UIGNORE:
                afterDigit1 = মিথ্যা;
                চালু; /* ignore all but letters and digits */
            ক্ষেত্র ZERO:
                যদি (!afterDigit1) {
                    পরবর্তী_Type = GET_CHAR_TYPE(*name1);
                    যদি (পরবর্তী_Type == ZERO || পরবর্তী_Type == NONZERO) {
                        চালু; /* ignore leading zero before another digit */
                    }
                }
                বিরতি;
            ক্ষেত্র NONZERO:
                afterDigit1 = সত্য;
                বিরতি;
            পূর্বনির্ধারিত:
                c1 = (অক্ষর)type; /* lowercased letter */
                afterDigit1 = মিথ্যা;
                বিরতি;
            }
            বিরতি; /* deliver c1 */
        }
        যখন ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            নির্বাচন (type) {
            ক্ষেত্র UIGNORE:
                afterDigit2 = মিথ্যা;
                চালু; /* ignore all but letters and digits */
            ক্ষেত্র ZERO:
                যদি (!afterDigit2) {
                    পরবর্তী_Type = GET_CHAR_TYPE(*name2);
                    যদি (পরবর্তী_Type == ZERO || পরবর্তী_Type == NONZERO) {
                        চালু; /* ignore leading zero before another digit */
                    }
                }
                বিরতি;
            ক্ষেত্র NONZERO:
                afterDigit2 = সত্য;
                বিরতি;
            পূর্বনির্ধারিত:
                c2 = (অক্ষর)type; /* lowercased letter */
                afterDigit2 = মিথ্যা;
                বিরতি;
            }
            বিরতি; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        যদি ((c1|c2)==0) {
            ফেরত 0;
        }

        /* Case-insensitive comparison */
        rc = (পূর্ণসংখ্যা)(unsigned অক্ষর)c1 - (পূর্ণসংখ্যা)(unsigned অক্ষর)c2;
        যদি (rc != 0) {
            ফেরত rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
স্থির অন্তঃরেখ uint32_t
findConverter(ধ্রুবক অক্ষর *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    পূর্ণসংখ্যা ফলাফল;
    পূর্ণসংখ্যা isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    অক্ষর strippedName[UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য];

    যদি (!isUnnormalized) {
        যদি (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য) {
            *pErrorCode = U_বাফার_OVERFLOW_ERROR;
            ফেরত UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    start = 0;
    limit = gMainTable.untagged_Conv_Array_আকার;
    mid = limit;
    lastMid = UINT32_MAX;

    জন্য (;;) {
        mid = (start + limit) / 2;
        যদি (lastMid == mid) {   /* Have we moved? */
            বিরতি;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        যদি (isUnnormalized) {
            ফলাফল = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        অন্যথা {
            ফলাফল = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        যদি (ফলাফল < 0) {
            limit = mid;
        } অন্যথা যদি (ফলাফল > 0) {
            start = mid;
        } অন্যথা {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            যদি (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            যদি (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            ফেরত gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_সূচক_MASK;
        }
    }

    ফেরত UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
স্থির অন্তঃরেখ UBool
isAliasInList(ধ্রুবক অক্ষর *alias, uint32_t listOffset) {
    যদি (listOffset) {
        uint32_t currAlias;
        uint32_t list_গুনতি = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        ধ্রুবক uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        জন্য (currAlias = 0; currAlias < list_গুনতি; currAlias++) {
            যদি (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                ফেরত সত্য;
            }
        }
    }
    ফেরত মিথ্যা;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
স্থির uint32_t
findTaggedAliasListsOffset(ধ্রুবক অক্ষর *alias, ধ্রুবক অক্ষর *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_অঙ্ক;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_অঙ্ক = get_Tag_সংখ্যা(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_অঙ্ক = findConverter(alias, nullptr, &myErr);
    যদি (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    যদি (tag_অঙ্ক < (gMainTable.tag_List_আকার - UCNV_অঙ্ক_HIDDEN_TAGS) && conv_অঙ্ক < gMainTable.converter_List_আকার) {
        listOffset = gMainTable.taggedAliasArray[tag_অঙ্ক*gMainTable.converter_List_আকার + conv_অঙ্ক];
        যদি (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            ফেরত listOffset;
        }
        যদি (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            জন্য (idx = 0; idx < gMainTable.tagged_Alias_Array_আকার; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                যদি (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_অঙ্ক = idx/gMainTable.converter_List_আকার;
                    uint32_t curr_Conv_অঙ্ক = (idx - curr_Tag_অঙ্ক*gMainTable.converter_List_আকার);
                    uint32_t অস্থায়ী_List_Offset = gMainTable.taggedAliasArray[tag_অঙ্ক*gMainTable.converter_List_আকার + curr_Conv_অঙ্ক];
                    যদি (অস্থায়ী_List_Offset && gMainTable.taggedAliasLists[অস্থায়ী_List_Offset + 1]) {
                        ফেরত অস্থায়ী_List_Offset;
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
        ফেরত 0;
    }
    /* else converter or tag not found */

    ফেরত UINT32_MAX;
}

/* Return the canonical name */
স্থির uint32_t
find_Tagged_Converter_অঙ্ক(ধ্রুবক অক্ষর *alias, ধ্রুবক অক্ষর *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_অঙ্ক;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_অঙ্ক = get_Tag_সংখ্যা(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_অঙ্ক = findConverter(alias, nullptr, &myErr);
    যদি (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    যদি (tag_অঙ্ক < (gMainTable.tag_List_আকার - UCNV_অঙ্ক_HIDDEN_TAGS) && conv_অঙ্ক < gMainTable.converter_List_আকার) {
        listOffset = gMainTable.taggedAliasArray[tag_অঙ্ক*gMainTable.converter_List_আকার + conv_অঙ্ক];
        যদি (listOffset && isAliasInList(alias, listOffset)) {
            ফেরত conv_অঙ্ক;
        }
        যদি (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_অঙ্ক)*gMainTable.converter_List_আকার;
            uint32_t convLimit = (tag_অঙ্ক+1)*gMainTable.converter_List_আকার;
            জন্য (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                যদি (listOffset && isAliasInList(alias, listOffset)) {
                    ফেরত idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    ফেরত UINT32_MAX;
}

U_CAPI ধ্রুবক অক্ষর *
ucnv_io_getConverterName(ধ্রুবক অক্ষর *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    ধ্রুবক অক্ষর *aliasTmp = alias;
    int32_t i = 0;
    জন্য (i = 0; i < 2; i++) {
        যদি (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            যদি (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } অন্যথা {
                বিরতি;
            }
        }
        যদি(have_Alias_তথ্য(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_অঙ্ক = findConverter(aliasTmp, containsOption, pErrorCode);
            যদি (conv_অঙ্ক < gMainTable.converter_List_আকার) {
                ফেরত GET_STRING(gMainTable.converterList[conv_অঙ্ক]);
            }
            /* else converter not found */
        } অন্যথা {
            বিরতি;
        }
    }

    ফেরত nullptr;
}

U_CDECL_BEGIN


স্থির int32_t U_CALLCONV
ucnv_io_গুনতি_Standard_Aliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    যদি (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    ফেরত value;
}

স্থির ধ্রুবক অক্ষর * U_CALLCONV
ucnv_io_পরবর্তী_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* ফলাফল_দৈর্ঘ্য,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    যদি (listOffset) {
        uint32_t list_গুনতি = gMainTable.taggedAliasLists[listOffset];
        ধ্রুবক uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        যদি (myContext->listIdx < list_গুনতি) {
            ধ্রুবক অক্ষর *myStr = GET_STRING(currList[myContext->listIdx++]);
            যদি (ফলাফল_দৈর্ঘ্য) {
                *ফলাফল_দৈর্ঘ্য = (int32_t)uprv_strlen(myStr);
            }
            ফেরত myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    যদি (ফলাফল_দৈর্ঘ্য) {
        *ফলাফল_দৈর্ঘ্য = 0;
    }
    ফেরত nullptr;
}

স্থির শূন্য U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

স্থির শূন্য U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
স্থির ধ্রুবক UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_গুনতি_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_পরবর্তী_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(ধ্রুবক অক্ষর *convName,
                       ধ্রুবক অক্ষর *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    যদি (have_Alias_তথ্য(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        যদি (listOffset < gMainTable.tagged_Alias_Lists_আকার) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(আকার(UEnumeration)));
            যদি (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                ফেরত nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, আকার(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(আকার(UAliasContext)));
            যদি (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                ফেরত nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    ফেরত myEnum;
}

স্থির uint16_t
ucnv_io_গুনতি_Aliases(ধ্রুবক অক্ষর *alias, UErrorCode *pErrorCode) {
    যদি(have_Alias_তথ্য(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_অঙ্ক = findConverter(alias, nullptr, pErrorCode);
        যদি (conv_অঙ্ক < gMainTable.converter_List_আকার) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tag_List_আকার - 1)*gMainTable.converter_List_আকার + conv_অঙ্ক];

            যদি (listOffset) {
                ফেরত gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ফেরত 0;
}

স্থির uint16_t
ucnv_io_getAliases(ধ্রুবক অক্ষর *alias, uint16_t start, ধ্রুবক অক্ষর **aliases, UErrorCode *pErrorCode) {
    যদি(have_Alias_তথ্য(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_অঙ্ক = findConverter(alias, nullptr, pErrorCode);
        যদি (conv_অঙ্ক < gMainTable.converter_List_আকার) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tag_List_আকার - 1)*gMainTable.converter_List_আকার + conv_অঙ্ক];

            যদি (listOffset) {
                uint32_t list_গুনতি = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ধ্রুবক uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                জন্য (currAlias = start; currAlias < list_গুনতি; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ফেরত 0;
}

স্থির ধ্রুবক অক্ষর *
ucnv_io_getAlias(ধ্রুবক অক্ষর *alias, uint16_t n, UErrorCode *pErrorCode) {
    যদি(have_Alias_তথ্য(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_অঙ্ক = findConverter(alias, nullptr, pErrorCode);
        যদি (conv_অঙ্ক < gMainTable.converter_List_আকার) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tag_List_আকার - 1)*gMainTable.converter_List_আকার + conv_অঙ্ক];

            যদি (listOffset) {
                uint32_t list_গুনতি = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ধ্রুবক uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                যদি (n < list_গুনতি)  {
                    ফেরত GET_STRING(currList[n]);
                }
                *pErrorCode = U_সূচক_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ফেরত nullptr;
}

স্থির uint16_t
ucnv_io_গুনতি_Standards(UErrorCode *pErrorCode) {
    যদি (have_Alias_তথ্য(pErrorCode)) {
        /* Don't include the empty list */
        ফেরত static_cast<uint16_t>(gMainTable.tag_List_আকার - UCNV_অঙ্ক_HIDDEN_TAGS);
    }

    ফেরত 0;
}

U_CAPI ধ্রুবক অক্ষর * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    যদি (have_Alias_তথ্য(pErrorCode)) {
        যদি (n < gMainTable.tag_List_আকার - UCNV_অঙ্ক_HIDDEN_TAGS) {
            ফেরত GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_সূচক_OUTOFBOUNDS_ERROR;
    }

    ফেরত nullptr;
}

U_CAPI ধ্রুবক অক্ষর * U_EXPORT2
ucnv_getStandardName(ধ্রুবক অক্ষর *alias, ধ্রুবক অক্ষর *standard, UErrorCode *pErrorCode) {
    যদি (have_Alias_তথ্য(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        যদি (0 < listOffset && listOffset < gMainTable.tagged_Alias_Lists_আকার) {
            ধ্রুবক uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            যদি (currList[0]) {
                ফেরত GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    ফেরত nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_গুনতি_Aliases(ধ্রুবক অক্ষর *alias, UErrorCode *pErrorCode)
{
    ফেরত ucnv_io_গুনতি_Aliases(alias, pErrorCode);
}


U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
ucnv_getAlias(ধ্রুবক অক্ষর *alias, uint16_t n, UErrorCode *pErrorCode)
{
    ফেরত ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI শূন্য U_EXPORT2
ucnv_getAliases(ধ্রুবক অক্ষর *alias, ধ্রুবক অক্ষর **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_গুনতি_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    ফেরত ucnv_io_গুনতি_Standards(&err);
}

U_CAPI ধ্রুবক অক্ষর * U_EXPORT2
ucnv_getCanonicalName(ধ্রুবক অক্ষর *alias, ধ্রুবক অক্ষর *standard, UErrorCode *pErrorCode) {
    যদি (have_Alias_তথ্য(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_অঙ্ক = find_Tagged_Converter_অঙ্ক(alias, standard, pErrorCode);

        যদি (conv_অঙ্ক < gMainTable.converter_List_আকার) {
            ফেরত GET_STRING(gMainTable.converterList[conv_অঙ্ক]);
        }
    }

    ফেরত nullptr;
}

U_CDECL_BEGIN


স্থির int32_t U_CALLCONV
ucnv_io_গুনতি_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    ফেরত gMainTable.converter_List_আকার;
}

স্থির ধ্রুবক অক্ষর * U_CALLCONV
ucnv_io_পরবর্তী_All_Converters(UEnumeration *enumerator,
                            int32_t* ফলাফল_দৈর্ঘ্য,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    যদি (*myContext < gMainTable.converter_List_আকার) {
        ধ্রুবক অক্ষর *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        যদি (ফলাফল_দৈর্ঘ্য) {
            *ফলাফল_দৈর্ঘ্য = (int32_t)uprv_strlen(myStr);
        }
        ফেরত myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    যদি (ফলাফল_দৈর্ঘ্য) {
        *ফলাফল_দৈর্ঘ্য = 0;
    }
    ফেরত nullptr;
}

স্থির শূন্য U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
স্থির ধ্রুবক UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_গুনতি_All_Converters,
    uenum_unextDefault,
    ucnv_io_পরবর্তী_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    যদি (have_Alias_তথ্য(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(আকার(UEnumeration)));
        যদি (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, আকার(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(আকার(uint16_t)));
        যদি (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            ফেরত nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    ফেরত myEnum;
}

U_CAPI uint16_t
ucnv_io_গুনতি_Known_Converters(UErrorCode *pErrorCode) {
    যদি (have_Alias_তথ্য(pErrorCode)) {
        ফেরত (uint16_t)gMainTable.converter_List_আকার;
    }
    ফেরত 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

প্রকারলেখ অক্ষর * U_CALLCONV StripForCompareFn(অক্ষর *dst, ধ্রুবক অক্ষর *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
প্রকারলেখ কাঠামো অস্থায়ী_Row {
    uint16_t str_সূচক, sort_সূচক;
} অস্থায়ী_Row;

প্রকারলেখ কাঠামো অস্থায়ী_Alias_Table {
    ধ্রুবক অক্ষর *chars;
    অস্থায়ী_Row *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} অস্থায়ী_Alias_Table;

গণনা {
    স্তূপ_ROW_CAPACITY=500
};

স্থির int32_t U_CALLCONV
io_compareRows(ধ্রুবক শূন্য *context, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    অক্ষর stripped_বাম[UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য],
         stripped_ডান[UCNV_MAX_CONVERTER_NAME_দৈর্ঘ্য];

    অস্থায়ী_Alias_Table *অস্থায়ী_Table=(অস্থায়ী_Alias_Table *)context;
    ধ্রুবক অক্ষর *chars=অস্থায়ী_Table->chars;

    ফেরত static_cast<int32_t>(uprv_strcmp(
        অস্থায়ী_Table->stripForCompare(stripped_বাম, chars + 2 * static_cast<ধ্রুবক অস্থায়ী_Row*>(left)->str_সূচক),
        অস্থায়ী_Table->stripForCompare(stripped_ডান, chars + 2 * static_cast<ধ্রুবক অস্থায়ী_Row*>(ডান)->str_সূচক)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ধ্রুবক UDataSwapper *ds,
                 ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
                 UErrorCode *pErrorCode) {
    ধ্রুবক UDataInfo *pInfo;
    int32_t header_আকার;

    ধ্রুবক uint16_t *inTable;
    ধ্রুবক uint32_t *inSectionSizes;
    uint32_t toc[offsets_গুনতি];
    uint32_t offsets[offsets_গুনতি]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_দৈর্ঘ্য, শীর্ষ_Offset;

    অস্থায়ী_Row rows[স্তূপ_ROW_CAPACITY];
    uint16_t resort[স্তূপ_ROW_CAPACITY];
    অস্থায়ী_Alias_Table অস্থায়ী_Table;

    /* udata_swapDataHeader checks the arguments */
    header_আকার=udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য+4);
    যদি(!(
        pInfo->তথ্য_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->তথ্য_Format[1]==0x76 &&
        pInfo->তথ্য_Format[2]==0x41 &&
        pInfo->তথ্য_Format[3]==0x6c &&
        pInfo->formatVersion[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->তথ্য_Format[0], pInfo->তথ্য_Format[1],
                         pInfo->তথ্য_Format[2], pInfo->তথ্য_Format[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }

    /* an alias table must contain at least the table of contents array */
    যদি(length>=0 && (length-header_আকার)<4*(1+min_Toc_দৈর্ঘ্য)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-header_আকার);
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }

    inSectionSizes=(ধ্রুবক uint32_t *)((ধ্রুবক অক্ষর *)in_তথ্য+header_আকার);
    inTable=(ধ্রুবক uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, আকার(toc));
    toc[toc_দৈর্ঘ্য_সূচক]=toc_দৈর্ঘ্য=ds->readUInt32(inSectionSizes[toc_দৈর্ঘ্য_সূচক]);
    যদি(toc_দৈর্ঘ্য<min_Toc_দৈর্ঘ্য || offsets_গুনতি<=toc_দৈর্ঘ্য) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_দৈর্ঘ্য);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত 0;
    }

    /* read the known part of the table of contents */
    জন্য(i=converter_List_সূচক; i<=toc_দৈর্ঘ্য; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, আকার(offsets));
    offsets[converter_List_সূচক]=2*(1+toc_দৈর্ঘ্য); /* count two 16-bit units per toc entry */
    জন্য(i=tag_List_সূচক; i<=toc_দৈর্ঘ্য; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    শীর্ষ_Offset=offsets[i-1]+toc[i-1];

    যদি(length>=0) {
        uint16_t *outTable;
        ধ্রুবক uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_সূচক;

        যদি((length-header_আকার)<(2*(int32_t)শীর্ষ_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-header_আকার);
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }

        outTable=(uint16_t *)((অক্ষর *)out_তথ্য+header_আকার);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+toc_দৈর্ঘ্য), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[string_Table_সূচক], 2*(int32_t)(toc[string_Table_সূচক]+toc[normalized_String_Table_সূচক]),
                             outTable+offsets[string_Table_সূচক], pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            ফেরত 0;
        }

        যদি(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converter_List_সূচক],
                            2*(int32_t)(offsets[string_Table_সূচক]-offsets[converter_List_সূচক]),
                            outTable+offsets[converter_List_সূচক],
                            pErrorCode);
        } অন্যথা {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_সূচক];

            অস্থায়ী_Table.chars=(ধ্রুবক অক্ষর *)(outTable+offsets[string_Table_সূচক]); /* sort by outCharset */

            যদি(count<=স্তূপ_ROW_CAPACITY) {
                অস্থায়ী_Table.rows=rows;
                অস্থায়ী_Table.resort=resort;
            } অন্যথা {
                অস্থায়ী_Table.rows=(অস্থায়ী_Row *)uprv_malloc(count*আকার(অস্থায়ী_Row)+count*2);
                যদি(অস্থায়ী_Table.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    ফেরত 0;
                }
                অস্থায়ী_Table.resort=(uint16_t *)(অস্থায়ী_Table.rows+count);
            }

            যদি(ds->outCharset==U_ASCII_FAMILY) {
                অস্থায়ী_Table.stripForCompare=ucnv_io_stripASCIIForCompare;
            } অন্যথা /* U_EBCDIC_FAMILY */ {
                অস্থায়ী_Table.stripForCompare=ucnv_io_stripEBCDICForCompare;
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
            p=inTable+offsets[alias_List_সূচক];
            q=outTable+offsets[alias_List_সূচক];

            p2=inTable+offsets[untagged_Conv_Array_সূচক];
            q2=outTable+offsets[untagged_Conv_Array_সূচক];

            জন্য(i=0; i<count; ++i) {
                অস্থায়ী_Table.rows[i].str_সূচক=ds->readUInt16(p[i]);
                অস্থায়ী_Table.rows[i].sort_সূচক=(uint16_t)i;
            }

            uprv_sortArray(অস্থায়ী_Table.rows, (int32_t)count, আকার(অস্থায়ী_Row),
                           io_compareRows, &অস্থায়ী_Table,
                           মিথ্যা, pErrorCode);

            যদি(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                যদি(p!=q) {
                    জন্য(i=0; i<count; ++i) {
                        old_সূচক=অস্থায়ী_Table.rows[i].sort_সূচক;
                        ds->swapArray16(ds, p+old_সূচক, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+old_সূচক, 2, q2+i, pErrorCode);
                    }
                } অন্যথা {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=অস্থায়ী_Table.resort;

                    জন্য(i=0; i<count; ++i) {
                        old_সূচক=অস্থায়ী_Table.rows[i].sort_সূচক;
                        ds->swapArray16(ds, p+old_সূচক, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    জন্য(i=0; i<count; ++i) {
                        old_সূচক=অস্থায়ী_Table.rows[i].sort_সূচক;
                        ds->swapArray16(ds, p2+old_সূচক, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            যদি(অস্থায়ী_Table.rows!=rows) {
                uprv_free(অস্থায়ী_Table.rows);
            }

            যদি(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                ফেরত 0;
            }

            /* swap remaining 16-bit values */
            ds->swapArray16(ds,
                            inTable+offsets[converter_List_সূচক],
                            2*(int32_t)(offsets[alias_List_সূচক]-offsets[converter_List_সূচক]),
                            outTable+offsets[converter_List_সূচক],
                            pErrorCode);
            ds->swapArray16(ds,
                            inTable+offsets[tagged_Alias_Array_সূচক],
                            2*(int32_t)(offsets[string_Table_সূচক]-offsets[tagged_Alias_Array_সূচক]),
                            outTable+offsets[tagged_Alias_Array_সূচক],
                            pErrorCode);
        }
    }

    ফেরত header_আকার+2*(int32_t)শীর্ষ_Offset;
}

#endif


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

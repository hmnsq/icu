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
প্রকারনির্দেশ সংরচনা UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

স্থির ধ্রুব বর্ণ DATA_NAME[] = "cnvalias";
স্থির ধ্রুব বর্ণ DATA_TYPE[] = "icu";

স্থির UDataMemory *gAliasData=nullptr;
স্থির icu::UInitOnce gAliasDataInitOnce {};

গণনা {
    tocLengthIndex=0,
    converterListIndex=1,
    tagListIndex=2,
    aliasListIndex=3,
    untaggedConvArrayIndex=4,
    taggedAliasArrayIndex=5,
    taggedAliasListsIndex=6,
    tableOptionsIndex=7,
    stringTableIndex=8,
    normalizedStringTableIndex=9,
    offsets_গুনতি,    /* length of the swapper's temporary offsets[] */
    minTocLength=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

স্থির ধ্রুব UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
স্থির UConverterAlias gMainTable;

#define GET_STRING(idx) (ধ্রুব বর্ণ *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (ধ্রুব বর্ণ *)(gMainTable.normalizedStringTable + (idx))

স্থির UBool U_CALLCONV
isAcceptable(শূন্য * /*context*/,
             ধ্রুব বর্ণ * /*type*/, ধ্রুব বর্ণ * /*name*/,
             ধ্রুব UDataInfo *pInfo) {
    প্রত্যাগম
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

স্থির UBool U_CALLCONV ucnv_io_cleanup()
{
    যদি (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, পরিমাণ(gMainTable));

    প্রত্যাগম সত্য;                   /* Everything was cleaned up */
}

স্থির শূন্য U_CALLCONV initAliasData(UErrorCode &errCode) {
    UDataMemory *data;
    ধ্রুব uint16_t *table;
    ধ্রুব uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t sizeOfData;
    int32_t sizeOfTOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(gAliasData == nullptr);
    data = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &errCode);
    যদি (U_FAILURE(errCode)) {
        প্রত্যাগম;
    }

    sectionSizes = static_cast<ধ্রুব uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    যদি (isDataLengthKnown && dataLength <= int32_t(পরিমাণ(sectionSizes[0]))) {
        // We don't even have a TOC!
        গচ্ছ invalidFormat;
    }
    table = reinterpret_cast<ধ্রুব uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * পরিমাণ(sectionSizes[0]));
    যদি (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
        // We don't have a whole TOC!
        গচ্ছ invalidFormat;
    }
    gAliasData = data;

    gMainTable.converterListSize      = sectionSizes[1];
    gMainTable.tagListSize            = sectionSizes[2];
    gMainTable.aliasListSize          = sectionSizes[3];
    gMainTable.untaggedConvArraySize  = sectionSizes[4];
    gMainTable.taggedAliasArraySize   = sectionSizes[5];
    gMainTable.taggedAliasListsSize   = sectionSizes[6];
    gMainTable.optionTableSize        = sectionSizes[7];
    gMainTable.stringTableSize        = sectionSizes[8];

    যদি (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    প্রতি (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * পরিমাণ(table[0]);
    }
    যদি (isDataLengthKnown && dataLength < sizeOfData) {
        // Truncated file!
        গচ্ছ invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (পরিমাণ(uint32_t)/পরিমাণ(uint16_t));
    gMainTable.converterList = table + currOffset;

    currOffset += gMainTable.converterListSize;
    gMainTable.tagList = table + currOffset;

    currOffset += gMainTable.tagListSize;
    gMainTable.aliasList = table + currOffset;

    currOffset += gMainTable.aliasListSize;
    gMainTable.untaggedConvArray = table + currOffset;

    currOffset += gMainTable.untaggedConvArraySize;
    gMainTable.taggedAliasArray = table + currOffset;

    /* aliasLists is a 1's based array, but it has a padding character */
    currOffset += gMainTable.taggedAliasArraySize;
    gMainTable.taggedAliasLists = table + currOffset;

    currOffset += gMainTable.taggedAliasListsSize;
    যদি (gMainTable.optionTableSize > 0
        && reinterpret_cast<ধ্রুব UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_গুনতি)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<ধ্রুব UConverterAliasOptions*>(table + currOffset);
    }
    অন্যথা {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    প্রত্যাগম;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


স্থির UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    প্রত্যাগম U_SUCCESS(*pErrorCode);
}

স্থির অন্তঃস্থ UBool
isAlias(ধ্রুব বর্ণ *alias, UErrorCode *pErrorCode) {
    যদি(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম অসত্য;
    }
    প্রত্যাগম *alias != 0;
}

স্থির uint32_t get_Tag_সংখ্যা(ধ্রুব বর্ণ *tagname) {
    যদি (gMainTable.tagList) {
        uint32_t tag_অঙ্ক;
        প্রতি (tag_অঙ্ক = 0; tag_অঙ্ক < gMainTable.tagListSize; tag_অঙ্ক++) {
            যদি (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_অঙ্ক]), tagname)) {
                প্রত্যাগম tag_অঙ্ক;
            }
        }
    }

    প্রত্যাগম UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
গণনা {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
স্থির ধ্রুব uint8_t asciiTypes[128] = {
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
স্থির ধ্রুব uint8_t ebcdicTypes[128] = {
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
#নোচেদ্যদি U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#অন্যথা
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI বর্ণ* U_CALLCONV
ucnv_io_stripASCIIForCompare(বর্ণ* dst U_LIFETIME_BOUND, ধ্রুব বর্ণ* name) {
    বর্ণ *dstItr = dst;
    uint8_t type, nextType;
    বর্ণ c1;
    UBool afterDigit = অসত্য;

    যাবত্ ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        বিকল্প (type) {
        পক্ষ UIGNORE:
            afterDigit = অসত্য;
            অনুবর্ত; /* ignore all but letters and digits */
        পক্ষ ZERO:
            যদি (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                যদি (nextType == ZERO || nextType == NONZERO) {
                    অনুবর্ত; /* ignore leading zero before another digit */
                }
            }
            ভঙ্গ;
        পক্ষ NONZERO:
            afterDigit = সত্য;
            ভঙ্গ;
        স্বভাব:
            c1 = (বর্ণ)type; /* lowercased letter */
            afterDigit = অসত্য;
            ভঙ্গ;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    প্রত্যাগম dst;
}

U_CAPI বর্ণ* U_CALLCONV
ucnv_io_stripEBCDICForCompare(বর্ণ* dst U_LIFETIME_BOUND, ধ্রুব বর্ণ* name) {
    বর্ণ *dstItr = dst;
    uint8_t type, nextType;
    বর্ণ c1;
    UBool afterDigit = অসত্য;

    যাবত্ ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        বিকল্প (type) {
        পক্ষ UIGNORE:
            afterDigit = অসত্য;
            অনুবর্ত; /* ignore all but letters and digits */
        পক্ষ ZERO:
            যদি (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                যদি (nextType == ZERO || nextType == NONZERO) {
                    অনুবর্ত; /* ignore leading zero before another digit */
                }
            }
            ভঙ্গ;
        পক্ষ NONZERO:
            afterDigit = সত্য;
            ভঙ্গ;
        স্বভাব:
            c1 = (বর্ণ)type; /* lowercased letter */
            afterDigit = অসত্য;
            ভঙ্গ;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    প্রত্যাগম dst;
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
U_CAPI পূর্ণাঙ্ক U_EXPORT2
ucnv_compareNames(ধ্রুব বর্ণ *name1, ধ্রুব বর্ণ *name2) {
    পূর্ণাঙ্ক rc;
    uint8_t type, nextType;
    বর্ণ c1, c2;
    UBool afterDigit1 = অসত্য, afterDigit2 = অসত্য;

    প্রতি (;;) {
        যাবত্ ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            বিকল্প (type) {
            পক্ষ UIGNORE:
                afterDigit1 = অসত্য;
                অনুবর্ত; /* ignore all but letters and digits */
            পক্ষ ZERO:
                যদি (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    যদি (nextType == ZERO || nextType == NONZERO) {
                        অনুবর্ত; /* ignore leading zero before another digit */
                    }
                }
                ভঙ্গ;
            পক্ষ NONZERO:
                afterDigit1 = সত্য;
                ভঙ্গ;
            স্বভাব:
                c1 = (বর্ণ)type; /* lowercased letter */
                afterDigit1 = অসত্য;
                ভঙ্গ;
            }
            ভঙ্গ; /* deliver c1 */
        }
        যাবত্ ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            বিকল্প (type) {
            পক্ষ UIGNORE:
                afterDigit2 = অসত্য;
                অনুবর্ত; /* ignore all but letters and digits */
            পক্ষ ZERO:
                যদি (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    যদি (nextType == ZERO || nextType == NONZERO) {
                        অনুবর্ত; /* ignore leading zero before another digit */
                    }
                }
                ভঙ্গ;
            পক্ষ NONZERO:
                afterDigit2 = সত্য;
                ভঙ্গ;
            স্বভাব:
                c2 = (বর্ণ)type; /* lowercased letter */
                afterDigit2 = অসত্য;
                ভঙ্গ;
            }
            ভঙ্গ; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        যদি ((c1|c2)==0) {
            প্রত্যাগম 0;
        }

        /* Case-insensitive comparison */
        rc = (পূর্ণাঙ্ক)(unsigned বর্ণ)c1 - (পূর্ণাঙ্ক)(unsigned বর্ণ)c2;
        যদি (rc != 0) {
            প্রত্যাগম rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
স্থির অন্তঃস্থ uint32_t
findConverter(ধ্রুব বর্ণ *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    পূর্ণাঙ্ক result;
    পূর্ণাঙ্ক isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    বর্ণ strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    যদি (!isUnnormalized) {
        যদি (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            প্রত্যাগম UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    start = 0;
    limit = gMainTable.untaggedConvArraySize;
    mid = limit;
    lastMid = UINT32_MAX;

    প্রতি (;;) {
        mid = (start + limit) / 2;
        যদি (lastMid == mid) {   /* Have we moved? */
            ভঙ্গ;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        যদি (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        অন্যথা {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        যদি (result < 0) {
            limit = mid;
        } অন্যথা যদি (result > 0) {
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
            প্রত্যাগম gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    প্রত্যাগম UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
স্থির অন্তঃস্থ UBool
isAliasInList(ধ্রুব বর্ণ *alias, uint32_t listOffset) {
    যদি (listOffset) {
        uint32_t currAlias;
        uint32_t list_গুনতি = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        ধ্রুব uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        প্রতি (currAlias = 0; currAlias < list_গুনতি; currAlias++) {
            যদি (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                প্রত্যাগম সত্য;
            }
        }
    }
    প্রত্যাগম অসত্য;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
স্থির uint32_t
findTaggedAliasListsOffset(ধ্রুব বর্ণ *alias, ধ্রুব বর্ণ *standard, UErrorCode *pErrorCode) {
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

    যদি (tag_অঙ্ক < (gMainTable.tagListSize - UCNV_অঙ্ক_HIDDEN_TAGS) && conv_অঙ্ক < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_অঙ্ক*gMainTable.converterListSize + conv_অঙ্ক];
        যদি (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            প্রত্যাগম listOffset;
        }
        যদি (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            প্রতি (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                যদি (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_অঙ্ক = idx/gMainTable.converterListSize;
                    uint32_t curr_Conv_অঙ্ক = (idx - curr_Tag_অঙ্ক*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tag_অঙ্ক*gMainTable.converterListSize + curr_Conv_অঙ্ক];
                    যদি (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        প্রত্যাগম tempListOffset;
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
        প্রত্যাগম 0;
    }
    /* else converter or tag not found */

    প্রত্যাগম UINT32_MAX;
}

/* Return the canonical name */
স্থির uint32_t
find_Tagged_Converter_অঙ্ক(ধ্রুব বর্ণ *alias, ধ্রুব বর্ণ *standard, UErrorCode *pErrorCode) {
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

    যদি (tag_অঙ্ক < (gMainTable.tagListSize - UCNV_অঙ্ক_HIDDEN_TAGS) && conv_অঙ্ক < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_অঙ্ক*gMainTable.converterListSize + conv_অঙ্ক];
        যদি (listOffset && isAliasInList(alias, listOffset)) {
            প্রত্যাগম conv_অঙ্ক;
        }
        যদি (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_অঙ্ক)*gMainTable.converterListSize;
            uint32_t convLimit = (tag_অঙ্ক+1)*gMainTable.converterListSize;
            প্রতি (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                যদি (listOffset && isAliasInList(alias, listOffset)) {
                    প্রত্যাগম idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    প্রত্যাগম UINT32_MAX;
}

U_CAPI ধ্রুব বর্ণ *
ucnv_io_getConverterName(ধ্রুব বর্ণ *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    ধ্রুব বর্ণ *aliasTmp = alias;
    int32_t i = 0;
    প্রতি (i = 0; i < 2; i++) {
        যদি (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            যদি (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } অন্যথা {
                ভঙ্গ;
            }
        }
        যদি(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_অঙ্ক = findConverter(aliasTmp, containsOption, pErrorCode);
            যদি (conv_অঙ্ক < gMainTable.converterListSize) {
                প্রত্যাগম GET_STRING(gMainTable.converterList[conv_অঙ্ক]);
            }
            /* else converter not found */
        } অন্যথা {
            ভঙ্গ;
        }
    }

    প্রত্যাগম nullptr;
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
    প্রত্যাগম value;
}

স্থির ধ্রুব বর্ণ * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    যদি (listOffset) {
        uint32_t list_গুনতি = gMainTable.taggedAliasLists[listOffset];
        ধ্রুব uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        যদি (myContext->listIdx < list_গুনতি) {
            ধ্রুব বর্ণ *myStr = GET_STRING(currList[myContext->listIdx++]);
            যদি (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            প্রত্যাগম myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    যদি (resultLength) {
        *resultLength = 0;
    }
    প্রত্যাগম nullptr;
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
স্থির ধ্রুব UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_গুনতি_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_nextStandardAliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(ধ্রুব বর্ণ *convName,
                       ধ্রুব বর্ণ *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    যদি (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        যদি (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(পরিমাণ(UEnumeration)));
            যদি (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, পরিমাণ(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(পরিমাণ(UAliasContext)));
            যদি (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                প্রত্যাগম nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    প্রত্যাগম myEnum;
}

স্থির uint16_t
ucnv_io_গুনতি_Aliases(ধ্রুব বর্ণ *alias, UErrorCode *pErrorCode) {
    যদি(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_অঙ্ক = findConverter(alias, nullptr, pErrorCode);
        যদি (conv_অঙ্ক < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_অঙ্ক];

            যদি (listOffset) {
                প্রত্যাগম gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    প্রত্যাগম 0;
}

স্থির uint16_t
ucnv_io_getAliases(ধ্রুব বর্ণ *alias, uint16_t start, ধ্রুব বর্ণ **aliases, UErrorCode *pErrorCode) {
    যদি(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_অঙ্ক = findConverter(alias, nullptr, pErrorCode);
        যদি (conv_অঙ্ক < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_অঙ্ক];

            যদি (listOffset) {
                uint32_t list_গুনতি = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ধ্রুব uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                প্রতি (currAlias = start; currAlias < list_গুনতি; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    প্রত্যাগম 0;
}

স্থির ধ্রুব বর্ণ *
ucnv_io_getAlias(ধ্রুব বর্ণ *alias, uint16_t n, UErrorCode *pErrorCode) {
    যদি(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_অঙ্ক = findConverter(alias, nullptr, pErrorCode);
        যদি (conv_অঙ্ক < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_অঙ্ক];

            যদি (listOffset) {
                uint32_t list_গুনতি = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ধ্রুব uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                যদি (n < list_গুনতি)  {
                    প্রত্যাগম GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    প্রত্যাগম nullptr;
}

স্থির uint16_t
ucnv_io_গুনতি_Standards(UErrorCode *pErrorCode) {
    যদি (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        প্রত্যাগম static_cast<uint16_t>(gMainTable.tagListSize - UCNV_অঙ্ক_HIDDEN_TAGS);
    }

    প্রত্যাগম 0;
}

U_CAPI ধ্রুব বর্ণ * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    যদি (haveAliasData(pErrorCode)) {
        যদি (n < gMainTable.tagListSize - UCNV_অঙ্ক_HIDDEN_TAGS) {
            প্রত্যাগম GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    প্রত্যাগম nullptr;
}

U_CAPI ধ্রুব বর্ণ * U_EXPORT2
ucnv_getStandardName(ধ্রুব বর্ণ *alias, ধ্রুব বর্ণ *standard, UErrorCode *pErrorCode) {
    যদি (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        যদি (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            ধ্রুব uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            যদি (currList[0]) {
                প্রত্যাগম GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    প্রত্যাগম nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_গুনতি_Aliases(ধ্রুব বর্ণ *alias, UErrorCode *pErrorCode)
{
    প্রত্যাগম ucnv_io_গুনতি_Aliases(alias, pErrorCode);
}


U_CAPI ধ্রুব বর্ণ* U_EXPORT2
ucnv_getAlias(ধ্রুব বর্ণ *alias, uint16_t n, UErrorCode *pErrorCode)
{
    প্রত্যাগম ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI শূন্য U_EXPORT2
ucnv_getAliases(ধ্রুব বর্ণ *alias, ধ্রুব বর্ণ **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_গুনতি_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    প্রত্যাগম ucnv_io_গুনতি_Standards(&err);
}

U_CAPI ধ্রুব বর্ণ * U_EXPORT2
ucnv_getCanonicalName(ধ্রুব বর্ণ *alias, ধ্রুব বর্ণ *standard, UErrorCode *pErrorCode) {
    যদি (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_অঙ্ক = find_Tagged_Converter_অঙ্ক(alias, standard, pErrorCode);

        যদি (conv_অঙ্ক < gMainTable.converterListSize) {
            প্রত্যাগম GET_STRING(gMainTable.converterList[conv_অঙ্ক]);
        }
    }

    প্রত্যাগম nullptr;
}

U_CDECL_BEGIN


স্থির int32_t U_CALLCONV
ucnv_io_গুনতি_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    প্রত্যাগম gMainTable.converterListSize;
}

স্থির ধ্রুব বর্ণ * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    যদি (*myContext < gMainTable.converterListSize) {
        ধ্রুব বর্ণ *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        যদি (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        প্রত্যাগম myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    যদি (resultLength) {
        *resultLength = 0;
    }
    প্রত্যাগম nullptr;
}

স্থির শূন্য U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
স্থির ধ্রুব UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_গুনতি_All_Converters,
    uenum_unextDefault,
    ucnv_io_nextAllConverters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    যদি (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(পরিমাণ(UEnumeration)));
        যদি (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, পরিমাণ(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(পরিমাণ(uint16_t)));
        যদি (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            প্রত্যাগম nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    প্রত্যাগম myEnum;
}

U_CAPI uint16_t
ucnv_io_গুনতি_Known_Converters(UErrorCode *pErrorCode) {
    যদি (haveAliasData(pErrorCode)) {
        প্রত্যাগম (uint16_t)gMainTable.converterListSize;
    }
    প্রত্যাগম 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

প্রকারনির্দেশ বর্ণ * U_CALLCONV StripForCompareFn(বর্ণ *dst, ধ্রুব বর্ণ *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
প্রকারনির্দেশ সংরচনা TempRow {
    uint16_t strIndex, sortIndex;
} TempRow;

প্রকারনির্দেশ সংরচনা TempAliasTable {
    ধ্রুব বর্ণ *chars;
    TempRow *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} TempAliasTable;

গণনা {
    স্তূপ_ROW_CAPACITY=500
};

স্থির int32_t U_CALLCONV
io_compareRows(ধ্রুব শূন্য *context, ধ্রুব শূন্য *left, ধ্রুব শূন্য *ডান) {
    বর্ণ stripped_বাম[UCNV_MAX_CONVERTER_NAME_LENGTH],
         stripped_ডান[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    ধ্রুব বর্ণ *chars=tempTable->chars;

    প্রত্যাগম static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(stripped_বাম, chars + 2 * static_cast<ধ্রুব TempRow*>(left)->strIndex),
        tempTable->stripForCompare(stripped_ডান, chars + 2 * static_cast<ধ্রুব TempRow*>(ডান)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ধ্রুব UDataSwapper *ds,
                 ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
                 UErrorCode *pErrorCode) {
    ধ্রুব UDataInfo *pInfo;
    int32_t headerSize;

    ধ্রুব uint16_t *inTable;
    ধ্রুব uint32_t *inSectionSizes;
    uint32_t toc[offsets_গুনতি];
    uint32_t offsets[offsets_গুনতি]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, শীর্ষ_Offset;

    TempRow rows[স্তূপ_ROW_CAPACITY];
    uint16_t resort[স্তূপ_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুব UDataInfo *)((ধ্রুব বর্ণ *)inData+4);
    যদি(!(
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        প্রত্যাগম 0;
    }

    /* an alias table must contain at least the table of contents array */
    যদি(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
    }

    inSectionSizes=(ধ্রুব uint32_t *)((ধ্রুব বর্ণ *)inData+headerSize);
    inTable=(ধ্রুব uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, পরিমাণ(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    যদি(tocLength<minTocLength || offsets_গুনতি<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        প্রত্যাগম 0;
    }

    /* read the known part of the table of contents */
    প্রতি(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, পরিমাণ(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    প্রতি(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    শীর্ষ_Offset=offsets[i-1]+toc[i-1];

    যদি(length>=0) {
        uint16_t *outTable;
        ধ্রুব uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        যদি((length-headerSize)<(2*(int32_t)শীর্ষ_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }

        outTable=(uint16_t *)((বর্ণ *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            প্রত্যাগম 0;
        }

        যদি(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } অন্যথা {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(ধ্রুব বর্ণ *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            যদি(count<=স্তূপ_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } অন্যথা {
                tempTable.rows=(TempRow *)uprv_malloc(count*পরিমাণ(TempRow)+count*2);
                যদি(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    প্রত্যাগম 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            যদি(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } অন্যথা /* U_EBCDIC_FAMILY */ {
                tempTable.stripForCompare=ucnv_io_stripEBCDICForCompare;
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
            p=inTable+offsets[aliasListIndex];
            q=outTable+offsets[aliasListIndex];

            p2=inTable+offsets[untaggedConvArrayIndex];
            q2=outTable+offsets[untaggedConvArrayIndex];

            প্রতি(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, পরিমাণ(TempRow),
                           io_compareRows, &tempTable,
                           অসত্য, pErrorCode);

            যদি(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                যদি(p!=q) {
                    প্রতি(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } অন্যথা {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    প্রতি(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    প্রতি(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            যদি(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            যদি(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                প্রত্যাগম 0;
            }

            /* swap remaining 16-bit values */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[aliasListIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
            ds->swapArray16(ds,
                            inTable+offsets[taggedAliasArrayIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[taggedAliasArrayIndex]),
                            outTable+offsets[taggedAliasArrayIndex],
                            pErrorCode);
        }
    }

    প্রত্যাগম headerSize+2*(int32_t)শীর্ষ_Offset;
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

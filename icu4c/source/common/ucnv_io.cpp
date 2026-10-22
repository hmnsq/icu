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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "unicode/udata.h"

#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uarrsort.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "udataswp.h"
#അന്തര്ഭാവ "udatamem.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "ucnv_io.h"
#അന്തര്ഭാവ "uenumimp.h"
#അന്തര്ഭാവ "ucln_cmn.h"

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
തരനിർവചനം ഘടന UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

സ്ഥിതം സ്ഥിരം അക്ഷരം DATA_NAME[] = "cnvalias";
സ്ഥിതം സ്ഥിരം അക്ഷരം DATA_TYPE[] = "icu";

സ്ഥിതം UDataMemory *gAliasData=nullptr;
സ്ഥിതം icu::UInitOnce gAliasDataInitOnce {};

എണ്ണൽ {
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
    offsets_എണ്ണം,    /* length of the swapper's temporary offsets[] */
    minTocLength=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

സ്ഥിതം സ്ഥിരം UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
സ്ഥിതം UConverterAlias gMainTable;

#define GET_STRING(idx) (സ്ഥിരം അക്ഷരം *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (സ്ഥിരം അക്ഷരം *)(gMainTable.normalizedStringTable + (idx))

സ്ഥിതം UBool U_CALLCONV
isAcceptable(ശൂന്യം * /*context*/,
             സ്ഥിരം അക്ഷരം * /*type*/, സ്ഥിരം അക്ഷരം * /*name*/,
             സ്ഥിരം UDataInfo *pInfo) {
    തിരികെ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

സ്ഥിതം UBool U_CALLCONV ucnv_io_cleanup()
{
    എങ്കിൽ (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, വലുപ്പം(gMainTable));

    തിരികെ സത്യം;                   /* Everything was cleaned up */
}

സ്ഥിതം ശൂന്യം U_CALLCONV initAliasData(UErrorCode &errCode) {
    UDataMemory *data;
    സ്ഥിരം uint16_t *table;
    സ്ഥിരം uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t sizeOfData;
    int32_t sizeOfTOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(gAliasData == nullptr);
    data = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &errCode);
    എങ്കിൽ (U_FAILURE(errCode)) {
        തിരികെ;
    }

    sectionSizes = static_cast<സ്ഥിരം uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    എങ്കിൽ (isDataLengthKnown && dataLength <= int32_t(വലുപ്പം(sectionSizes[0]))) {
        // We don't even have a TOC!
        പോകുക invalidFormat;
    }
    table = reinterpret_cast<സ്ഥിരം uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * വലുപ്പം(sectionSizes[0]));
    എങ്കിൽ (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
        // We don't have a whole TOC!
        പോകുക invalidFormat;
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

    എങ്കിൽ (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    വേണ്ടി (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * വലുപ്പം(table[0]);
    }
    എങ്കിൽ (isDataLengthKnown && dataLength < sizeOfData) {
        // Truncated file!
        പോകുക invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (വലുപ്പം(uint32_t)/വലുപ്പം(uint16_t));
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
    എങ്കിൽ (gMainTable.optionTableSize > 0
        && reinterpret_cast<സ്ഥിരം UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_എണ്ണം)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<സ്ഥിരം UConverterAliasOptions*>(table + currOffset);
    }
    അല്ലെങ്കിൽ {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    തിരികെ;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


സ്ഥിതം UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    തിരികെ U_SUCCESS(*pErrorCode);
}

സ്ഥിതം വരിയിൽ UBool
isAlias(സ്ഥിരം അക്ഷരം *alias, UErrorCode *pErrorCode) {
    എങ്കിൽ(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ അസത്യം;
    }
    തിരികെ *alias != 0;
}

സ്ഥിതം uint32_t get_Tag_സംഖ്യ(സ്ഥിരം അക്ഷരം *tagname) {
    എങ്കിൽ (gMainTable.tagList) {
        uint32_t tag_അക്കം;
        വേണ്ടി (tag_അക്കം = 0; tag_അക്കം < gMainTable.tagListSize; tag_അക്കം++) {
            എങ്കിൽ (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_അക്കം]), tagname)) {
                തിരികെ tag_അക്കം;
            }
        }
    }

    തിരികെ UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
എണ്ണൽ {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
സ്ഥിതം സ്ഥിരം uint8_t asciiTypes[128] = {
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
സ്ഥിതം സ്ഥിരം uint8_t ebcdicTypes[128] = {
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

#എങ്കിൽ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#അല്ലെങ്കിലെങ്കിൽ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#അല്ലെങ്കിൽ
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI അക്ഷരം* U_CALLCONV
ucnv_io_stripASCIIForCompare(അക്ഷരം* dst U_LIFETIME_BOUND, സ്ഥിരം അക്ഷരം* name) {
    അക്ഷരം *dstItr = dst;
    uint8_t type, nextType;
    അക്ഷരം c1;
    UBool afterDigit = അസത്യം;

    ആകുമ്പോൾ ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        തിരഞ്ഞെടുപ്പ് (type) {
        അവസ്ഥ UIGNORE:
            afterDigit = അസത്യം;
            തുടരുക; /* ignore all but letters and digits */
        അവസ്ഥ ZERO:
            എങ്കിൽ (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                എങ്കിൽ (nextType == ZERO || nextType == NONZERO) {
                    തുടരുക; /* ignore leading zero before another digit */
                }
            }
            നിർത്തുക;
        അവസ്ഥ NONZERO:
            afterDigit = സത്യം;
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            c1 = (അക്ഷരം)type; /* lowercased letter */
            afterDigit = അസത്യം;
            നിർത്തുക;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    തിരികെ dst;
}

U_CAPI അക്ഷരം* U_CALLCONV
ucnv_io_stripEBCDICForCompare(അക്ഷരം* dst U_LIFETIME_BOUND, സ്ഥിരം അക്ഷരം* name) {
    അക്ഷരം *dstItr = dst;
    uint8_t type, nextType;
    അക്ഷരം c1;
    UBool afterDigit = അസത്യം;

    ആകുമ്പോൾ ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        തിരഞ്ഞെടുപ്പ് (type) {
        അവസ്ഥ UIGNORE:
            afterDigit = അസത്യം;
            തുടരുക; /* ignore all but letters and digits */
        അവസ്ഥ ZERO:
            എങ്കിൽ (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                എങ്കിൽ (nextType == ZERO || nextType == NONZERO) {
                    തുടരുക; /* ignore leading zero before another digit */
                }
            }
            നിർത്തുക;
        അവസ്ഥ NONZERO:
            afterDigit = സത്യം;
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            c1 = (അക്ഷരം)type; /* lowercased letter */
            afterDigit = അസത്യം;
            നിർത്തുക;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    തിരികെ dst;
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
U_CAPI പൂർണ്ണസംഖ്യ U_EXPORT2
ucnv_compareNames(സ്ഥിരം അക്ഷരം *name1, സ്ഥിരം അക്ഷരം *name2) {
    പൂർണ്ണസംഖ്യ rc;
    uint8_t type, nextType;
    അക്ഷരം c1, c2;
    UBool afterDigit1 = അസത്യം, afterDigit2 = അസത്യം;

    വേണ്ടി (;;) {
        ആകുമ്പോൾ ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            തിരഞ്ഞെടുപ്പ് (type) {
            അവസ്ഥ UIGNORE:
                afterDigit1 = അസത്യം;
                തുടരുക; /* ignore all but letters and digits */
            അവസ്ഥ ZERO:
                എങ്കിൽ (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    എങ്കിൽ (nextType == ZERO || nextType == NONZERO) {
                        തുടരുക; /* ignore leading zero before another digit */
                    }
                }
                നിർത്തുക;
            അവസ്ഥ NONZERO:
                afterDigit1 = സത്യം;
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                c1 = (അക്ഷരം)type; /* lowercased letter */
                afterDigit1 = അസത്യം;
                നിർത്തുക;
            }
            നിർത്തുക; /* deliver c1 */
        }
        ആകുമ്പോൾ ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            തിരഞ്ഞെടുപ്പ് (type) {
            അവസ്ഥ UIGNORE:
                afterDigit2 = അസത്യം;
                തുടരുക; /* ignore all but letters and digits */
            അവസ്ഥ ZERO:
                എങ്കിൽ (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    എങ്കിൽ (nextType == ZERO || nextType == NONZERO) {
                        തുടരുക; /* ignore leading zero before another digit */
                    }
                }
                നിർത്തുക;
            അവസ്ഥ NONZERO:
                afterDigit2 = സത്യം;
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                c2 = (അക്ഷരം)type; /* lowercased letter */
                afterDigit2 = അസത്യം;
                നിർത്തുക;
            }
            നിർത്തുക; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        എങ്കിൽ ((c1|c2)==0) {
            തിരികെ 0;
        }

        /* Case-insensitive comparison */
        rc = (പൂർണ്ണസംഖ്യ)(unsigned അക്ഷരം)c1 - (പൂർണ്ണസംഖ്യ)(unsigned അക്ഷരം)c2;
        എങ്കിൽ (rc != 0) {
            തിരികെ rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
സ്ഥിതം വരിയിൽ uint32_t
findConverter(സ്ഥിരം അക്ഷരം *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    പൂർണ്ണസംഖ്യ result;
    പൂർണ്ണസംഖ്യ isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    അക്ഷരം strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    എങ്കിൽ (!isUnnormalized) {
        എങ്കിൽ (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            തിരികെ UINT32_MAX;
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

    വേണ്ടി (;;) {
        mid = (start + limit) / 2;
        എങ്കിൽ (lastMid == mid) {   /* Have we moved? */
            നിർത്തുക;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        എങ്കിൽ (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        അല്ലെങ്കിൽ {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        എങ്കിൽ (result < 0) {
            limit = mid;
        } അല്ലെങ്കിൽ എങ്കിൽ (result > 0) {
            start = mid;
        } അല്ലെങ്കിൽ {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            എങ്കിൽ (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            എങ്കിൽ (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            തിരികെ gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    തിരികെ UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
സ്ഥിതം വരിയിൽ UBool
isAliasInList(സ്ഥിരം അക്ഷരം *alias, uint32_t listOffset) {
    എങ്കിൽ (listOffset) {
        uint32_t currAlias;
        uint32_t list_എണ്ണം = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        സ്ഥിരം uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        വേണ്ടി (currAlias = 0; currAlias < list_എണ്ണം; currAlias++) {
            എങ്കിൽ (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                തിരികെ സത്യം;
            }
        }
    }
    തിരികെ അസത്യം;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
സ്ഥിതം uint32_t
findTaggedAliasListsOffset(സ്ഥിരം അക്ഷരം *alias, സ്ഥിരം അക്ഷരം *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_അക്കം;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_അക്കം = get_Tag_സംഖ്യ(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_അക്കം = findConverter(alias, nullptr, &myErr);
    എങ്കിൽ (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    എങ്കിൽ (tag_അക്കം < (gMainTable.tagListSize - UCNV_അക്കം_HIDDEN_TAGS) && conv_അക്കം < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_അക്കം*gMainTable.converterListSize + conv_അക്കം];
        എങ്കിൽ (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            തിരികെ listOffset;
        }
        എങ്കിൽ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            വേണ്ടി (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                എങ്കിൽ (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_അക്കം = idx/gMainTable.converterListSize;
                    uint32_t curr_Conv_അക്കം = (idx - curr_Tag_അക്കം*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tag_അക്കം*gMainTable.converterListSize + curr_Conv_അക്കം];
                    എങ്കിൽ (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        തിരികെ tempListOffset;
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
        തിരികെ 0;
    }
    /* else converter or tag not found */

    തിരികെ UINT32_MAX;
}

/* Return the canonical name */
സ്ഥിതം uint32_t
find_Tagged_Converter_അക്കം(സ്ഥിരം അക്ഷരം *alias, സ്ഥിരം അക്ഷരം *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_അക്കം;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_അക്കം = get_Tag_സംഖ്യ(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_അക്കം = findConverter(alias, nullptr, &myErr);
    എങ്കിൽ (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    എങ്കിൽ (tag_അക്കം < (gMainTable.tagListSize - UCNV_അക്കം_HIDDEN_TAGS) && conv_അക്കം < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_അക്കം*gMainTable.converterListSize + conv_അക്കം];
        എങ്കിൽ (listOffset && isAliasInList(alias, listOffset)) {
            തിരികെ conv_അക്കം;
        }
        എങ്കിൽ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_അക്കം)*gMainTable.converterListSize;
            uint32_t convLimit = (tag_അക്കം+1)*gMainTable.converterListSize;
            വേണ്ടി (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                എങ്കിൽ (listOffset && isAliasInList(alias, listOffset)) {
                    തിരികെ idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    തിരികെ UINT32_MAX;
}

U_CAPI സ്ഥിരം അക്ഷരം *
ucnv_io_getConverterName(സ്ഥിരം അക്ഷരം *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    സ്ഥിരം അക്ഷരം *aliasTmp = alias;
    int32_t i = 0;
    വേണ്ടി (i = 0; i < 2; i++) {
        എങ്കിൽ (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            എങ്കിൽ (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } അല്ലെങ്കിൽ {
                നിർത്തുക;
            }
        }
        എങ്കിൽ(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_അക്കം = findConverter(aliasTmp, containsOption, pErrorCode);
            എങ്കിൽ (conv_അക്കം < gMainTable.converterListSize) {
                തിരികെ GET_STRING(gMainTable.converterList[conv_അക്കം]);
            }
            /* else converter not found */
        } അല്ലെങ്കിൽ {
            നിർത്തുക;
        }
    }

    തിരികെ nullptr;
}

U_CDECL_BEGIN


സ്ഥിതം int32_t U_CALLCONV
ucnv_io_എണ്ണം_Standard_Aliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    എങ്കിൽ (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    തിരികെ value;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    എങ്കിൽ (listOffset) {
        uint32_t list_എണ്ണം = gMainTable.taggedAliasLists[listOffset];
        സ്ഥിരം uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        എങ്കിൽ (myContext->listIdx < list_എണ്ണം) {
            സ്ഥിരം അക്ഷരം *myStr = GET_STRING(currList[myContext->listIdx++]);
            എങ്കിൽ (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            തിരികെ myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    എങ്കിൽ (resultLength) {
        *resultLength = 0;
    }
    തിരികെ nullptr;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
സ്ഥിതം സ്ഥിരം UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_എണ്ണം_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_nextStandardAliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(സ്ഥിരം അക്ഷരം *convName,
                       സ്ഥിരം അക്ഷരം *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    എങ്കിൽ (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        എങ്കിൽ (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(വലുപ്പം(UEnumeration)));
            എങ്കിൽ (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, വലുപ്പം(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(വലുപ്പം(UAliasContext)));
            എങ്കിൽ (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                തിരികെ nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    തിരികെ myEnum;
}

സ്ഥിതം uint16_t
ucnv_io_എണ്ണം_Aliases(സ്ഥിരം അക്ഷരം *alias, UErrorCode *pErrorCode) {
    എങ്കിൽ(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_അക്കം = findConverter(alias, nullptr, pErrorCode);
        എങ്കിൽ (conv_അക്കം < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_അക്കം];

            എങ്കിൽ (listOffset) {
                തിരികെ gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    തിരികെ 0;
}

സ്ഥിതം uint16_t
ucnv_io_getAliases(സ്ഥിരം അക്ഷരം *alias, uint16_t start, സ്ഥിരം അക്ഷരം **aliases, UErrorCode *pErrorCode) {
    എങ്കിൽ(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_അക്കം = findConverter(alias, nullptr, pErrorCode);
        എങ്കിൽ (conv_അക്കം < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_അക്കം];

            എങ്കിൽ (listOffset) {
                uint32_t list_എണ്ണം = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                സ്ഥിരം uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                വേണ്ടി (currAlias = start; currAlias < list_എണ്ണം; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *
ucnv_io_getAlias(സ്ഥിരം അക്ഷരം *alias, uint16_t n, UErrorCode *pErrorCode) {
    എങ്കിൽ(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_അക്കം = findConverter(alias, nullptr, pErrorCode);
        എങ്കിൽ (conv_അക്കം < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_അക്കം];

            എങ്കിൽ (listOffset) {
                uint32_t list_എണ്ണം = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                സ്ഥിരം uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                എങ്കിൽ (n < list_എണ്ണം)  {
                    തിരികെ GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    തിരികെ nullptr;
}

സ്ഥിതം uint16_t
ucnv_io_എണ്ണം_Standards(UErrorCode *pErrorCode) {
    എങ്കിൽ (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        തിരികെ static_cast<uint16_t>(gMainTable.tagListSize - UCNV_അക്കം_HIDDEN_TAGS);
    }

    തിരികെ 0;
}

U_CAPI സ്ഥിരം അക്ഷരം * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    എങ്കിൽ (haveAliasData(pErrorCode)) {
        എങ്കിൽ (n < gMainTable.tagListSize - UCNV_അക്കം_HIDDEN_TAGS) {
            തിരികെ GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    തിരികെ nullptr;
}

U_CAPI സ്ഥിരം അക്ഷരം * U_EXPORT2
ucnv_getStandardName(സ്ഥിരം അക്ഷരം *alias, സ്ഥിരം അക്ഷരം *standard, UErrorCode *pErrorCode) {
    എങ്കിൽ (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        എങ്കിൽ (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            സ്ഥിരം uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            എങ്കിൽ (currList[0]) {
                തിരികെ GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    തിരികെ nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_എണ്ണം_Aliases(സ്ഥിരം അക്ഷരം *alias, UErrorCode *pErrorCode)
{
    തിരികെ ucnv_io_എണ്ണം_Aliases(alias, pErrorCode);
}


U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
ucnv_getAlias(സ്ഥിരം അക്ഷരം *alias, uint16_t n, UErrorCode *pErrorCode)
{
    തിരികെ ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI ശൂന്യം U_EXPORT2
ucnv_getAliases(സ്ഥിരം അക്ഷരം *alias, സ്ഥിരം അക്ഷരം **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_എണ്ണം_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    തിരികെ ucnv_io_എണ്ണം_Standards(&err);
}

U_CAPI സ്ഥിരം അക്ഷരം * U_EXPORT2
ucnv_getCanonicalName(സ്ഥിരം അക്ഷരം *alias, സ്ഥിരം അക്ഷരം *standard, UErrorCode *pErrorCode) {
    എങ്കിൽ (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_അക്കം = find_Tagged_Converter_അക്കം(alias, standard, pErrorCode);

        എങ്കിൽ (conv_അക്കം < gMainTable.converterListSize) {
            തിരികെ GET_STRING(gMainTable.converterList[conv_അക്കം]);
        }
    }

    തിരികെ nullptr;
}

U_CDECL_BEGIN


സ്ഥിതം int32_t U_CALLCONV
ucnv_io_എണ്ണം_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    തിരികെ gMainTable.converterListSize;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    എങ്കിൽ (*myContext < gMainTable.converterListSize) {
        സ്ഥിരം അക്ഷരം *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        എങ്കിൽ (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        തിരികെ myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    എങ്കിൽ (resultLength) {
        *resultLength = 0;
    }
    തിരികെ nullptr;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
സ്ഥിതം സ്ഥിരം UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_എണ്ണം_All_Converters,
    uenum_unextDefault,
    ucnv_io_nextAllConverters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    എങ്കിൽ (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(വലുപ്പം(UEnumeration)));
        എങ്കിൽ (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, വലുപ്പം(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(വലുപ്പം(uint16_t)));
        എങ്കിൽ (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            തിരികെ nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    തിരികെ myEnum;
}

U_CAPI uint16_t
ucnv_io_എണ്ണം_Known_Converters(UErrorCode *pErrorCode) {
    എങ്കിൽ (haveAliasData(pErrorCode)) {
        തിരികെ (uint16_t)gMainTable.converterListSize;
    }
    തിരികെ 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

തരനിർവചനം അക്ഷരം * U_CALLCONV StripForCompareFn(അക്ഷരം *dst, സ്ഥിരം അക്ഷരം *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
തരനിർവചനം ഘടന TempRow {
    uint16_t strIndex, sortIndex;
} TempRow;

തരനിർവചനം ഘടന TempAliasTable {
    സ്ഥിരം അക്ഷരം *chars;
    TempRow *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} TempAliasTable;

എണ്ണൽ {
    അട്ടി_ROW_CAPACITY=500
};

സ്ഥിതം int32_t U_CALLCONV
io_compareRows(സ്ഥിരം ശൂന്യം *context, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    അക്ഷരം stripped_ഇടത്[UCNV_MAX_CONVERTER_NAME_LENGTH],
         stripped_വലത്[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    സ്ഥിരം അക്ഷരം *chars=tempTable->chars;

    തിരികെ static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(stripped_ഇടത്, chars + 2 * static_cast<സ്ഥിരം TempRow*>(left)->strIndex),
        tempTable->stripForCompare(stripped_വലത്, chars + 2 * static_cast<സ്ഥിരം TempRow*>(വലത്)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(സ്ഥിരം UDataSwapper *ds,
                 സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                 UErrorCode *pErrorCode) {
    സ്ഥിരം UDataInfo *pInfo;
    int32_t headerSize;

    സ്ഥിരം uint16_t *inTable;
    സ്ഥിരം uint32_t *inSectionSizes;
    uint32_t toc[offsets_എണ്ണം];
    uint32_t offsets[offsets_എണ്ണം]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, മുകൾ_Offset;

    TempRow rows[അട്ടി_ROW_CAPACITY];
    uint16_t resort[അട്ടി_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    /* check data format and format version */
    pInfo=(സ്ഥിരം UDataInfo *)((സ്ഥിരം അക്ഷരം *)inData+4);
    എങ്കിൽ(!(
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
        തിരികെ 0;
    }

    /* an alias table must contain at least the table of contents array */
    എങ്കിൽ(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }

    inSectionSizes=(സ്ഥിരം uint32_t *)((സ്ഥിരം അക്ഷരം *)inData+headerSize);
    inTable=(സ്ഥിരം uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, വലുപ്പം(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    എങ്കിൽ(tocLength<minTocLength || offsets_എണ്ണം<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        തിരികെ 0;
    }

    /* read the known part of the table of contents */
    വേണ്ടി(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, വലുപ്പം(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    വേണ്ടി(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    മുകൾ_Offset=offsets[i-1]+toc[i-1];

    എങ്കിൽ(length>=0) {
        uint16_t *outTable;
        സ്ഥിരം uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        എങ്കിൽ((length-headerSize)<(2*(int32_t)മുകൾ_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }

        outTable=(uint16_t *)((അക്ഷരം *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            തിരികെ 0;
        }

        എങ്കിൽ(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } അല്ലെങ്കിൽ {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(സ്ഥിരം അക്ഷരം *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            എങ്കിൽ(count<=അട്ടി_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } അല്ലെങ്കിൽ {
                tempTable.rows=(TempRow *)uprv_malloc(count*വലുപ്പം(TempRow)+count*2);
                എങ്കിൽ(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    തിരികെ 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            എങ്കിൽ(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } അല്ലെങ്കിൽ /* U_EBCDIC_FAMILY */ {
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

            വേണ്ടി(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, വലുപ്പം(TempRow),
                           io_compareRows, &tempTable,
                           അസത്യം, pErrorCode);

            എങ്കിൽ(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                എങ്കിൽ(p!=q) {
                    വേണ്ടി(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } അല്ലെങ്കിൽ {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    വേണ്ടി(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    വേണ്ടി(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            എങ്കിൽ(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            എങ്കിൽ(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                തിരികെ 0;
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

    തിരികെ headerSize+2*(int32_t)മുകൾ_Offset;
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

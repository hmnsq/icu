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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_CONVERSION

#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"

#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "udataswp.h"
#ଅନ୍ତର୍ଭାଵ "udatamem.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_io.h"
#ଅନ୍ତର୍ଭାଵ "uenumimp.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର DATA_NAME[] = "cnvalias";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର DATA_TYPE[] = "icu";

ସ୍ଥିର UDataMemory *gAliasData=nullptr;
ସ୍ଥିର icu::UInitOnce gAliasDataInitOnce {};

ଗଣନା {
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
    offsets_ଗଣନା,    /* length of the swapper's temporary offsets[] */
    minTocLength=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

ସ୍ଥିର ଅଚଳ UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
ସ୍ଥିର UConverterAlias gMainTable;

#define GET_STRING(idx) (ଅଚଳ ଅକ୍ଷର *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (ଅଚଳ ଅକ୍ଷର *)(gMainTable.normalizedStringTable + (idx))

ସ୍ଥିର UBool U_CALLCONV
isAcceptable(ଶୂନ୍ୟ * /*context*/,
             ଅଚଳ ଅକ୍ଷର * /*type*/, ଅଚଳ ଅକ୍ଷର * /*name*/,
             ଅଚଳ UDataInfo *pInfo) {
    ଫେରନ୍ତୁ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

ସ୍ଥିର UBool U_CALLCONV ucnv_io_cleanup()
{
    ଯଦି (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, ଆକାର(gMainTable));

    ଫେରନ୍ତୁ ସତ୍ୟ;                   /* Everything was cleaned up */
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV initAliasData(UErrorCode &errCode) {
    UDataMemory *data;
    ଅଚଳ uint16_t *table;
    ଅଚଳ uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t sizeOfData;
    int32_t sizeOfTOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(gAliasData == nullptr);
    data = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &errCode);
    ଯଦି (U_FAILURE(errCode)) {
        ଫେରନ୍ତୁ;
    }

    sectionSizes = static_cast<ଅଚଳ uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    ଯଦି (isDataLengthKnown && dataLength <= int32_t(ଆକାର(sectionSizes[0]))) {
        // We don't even have a TOC!
        ଯାଅ invalidFormat;
    }
    table = reinterpret_cast<ଅଚଳ uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * ଆକାର(sectionSizes[0]));
    ଯଦି (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
        // We don't have a whole TOC!
        ଯାଅ invalidFormat;
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

    ଯଦି (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    ପାଇଁ (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * ଆକାର(table[0]);
    }
    ଯଦି (isDataLengthKnown && dataLength < sizeOfData) {
        // Truncated file!
        ଯାଅ invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (ଆକାର(uint32_t)/ଆକାର(uint16_t));
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
    ଯଦି (gMainTable.optionTableSize > 0
        && reinterpret_cast<ଅଚଳ UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_ଗଣନା)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<ଅଚଳ UConverterAliasOptions*>(table + currOffset);
    }
    ଅନ୍ୟଥା {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    ଫେରନ୍ତୁ;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


ସ୍ଥିର UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    ଫେରନ୍ତୁ U_SUCCESS(*pErrorCode);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
isAlias(ଅଚଳ ଅକ୍ଷର *alias, UErrorCode *pErrorCode) {
    ଯଦି(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ *alias != 0;
}

ସ୍ଥିର uint32_t get_Tag_ସଂଖ୍ୟା(ଅଚଳ ଅକ୍ଷର *tagname) {
    ଯଦି (gMainTable.tagList) {
        uint32_t tag_ଅଙ୍କ;
        ପାଇଁ (tag_ଅଙ୍କ = 0; tag_ଅଙ୍କ < gMainTable.tagListSize; tag_ଅଙ୍କ++) {
            ଯଦି (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_ଅଙ୍କ]), tagname)) {
                ଫେରନ୍ତୁ tag_ଅଙ୍କ;
            }
        }
    }

    ଫେରନ୍ତୁ UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
ଗଣନା {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
ସ୍ଥିର ଅଚଳ uint8_t asciiTypes[128] = {
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
ସ୍ଥିର ଅଚଳ uint8_t ebcdicTypes[128] = {
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

#ଯଦି U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#ନଚେତଯଦି U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#ଅନ୍ୟଥା
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI ଅକ୍ଷର* U_CALLCONV
ucnv_io_stripASCIIForCompare(ଅକ୍ଷର* dst U_LIFETIME_BOUND, ଅଚଳ ଅକ୍ଷର* name) {
    ଅକ୍ଷର *dstItr = dst;
    uint8_t type, nextType;
    ଅକ୍ଷର c1;
    UBool afterDigit = ମିଥ୍ୟା;

    ଯେତେବେଳେ ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        ବାଛ (type) {
        ସ୍ଥିତି UIGNORE:
            afterDigit = ମିଥ୍ୟା;
            ଜାରି; /* ignore all but letters and digits */
        ସ୍ଥିତି ZERO:
            ଯଦି (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                ଯଦି (nextType == ZERO || nextType == NONZERO) {
                    ଜାରି; /* ignore leading zero before another digit */
                }
            }
            ଭାଙ୍ଗ;
        ସ୍ଥିତି NONZERO:
            afterDigit = ସତ୍ୟ;
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            c1 = (ଅକ୍ଷର)type; /* lowercased letter */
            afterDigit = ମିଥ୍ୟା;
            ଭାଙ୍ଗ;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ଫେରନ୍ତୁ dst;
}

U_CAPI ଅକ୍ଷର* U_CALLCONV
ucnv_io_stripEBCDICForCompare(ଅକ୍ଷର* dst U_LIFETIME_BOUND, ଅଚଳ ଅକ୍ଷର* name) {
    ଅକ୍ଷର *dstItr = dst;
    uint8_t type, nextType;
    ଅକ୍ଷର c1;
    UBool afterDigit = ମିଥ୍ୟା;

    ଯେତେବେଳେ ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        ବାଛ (type) {
        ସ୍ଥିତି UIGNORE:
            afterDigit = ମିଥ୍ୟା;
            ଜାରି; /* ignore all but letters and digits */
        ସ୍ଥିତି ZERO:
            ଯଦି (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                ଯଦି (nextType == ZERO || nextType == NONZERO) {
                    ଜାରି; /* ignore leading zero before another digit */
                }
            }
            ଭାଙ୍ଗ;
        ସ୍ଥିତି NONZERO:
            afterDigit = ସତ୍ୟ;
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            c1 = (ଅକ୍ଷର)type; /* lowercased letter */
            afterDigit = ମିଥ୍ୟା;
            ଭାଙ୍ଗ;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ଫେରନ୍ତୁ dst;
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
U_CAPI ପୂର୍ଣ୍ଣସଂଖ୍ୟା U_EXPORT2
ucnv_compareNames(ଅଚଳ ଅକ୍ଷର *name1, ଅଚଳ ଅକ୍ଷର *name2) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
    uint8_t type, nextType;
    ଅକ୍ଷର c1, c2;
    UBool afterDigit1 = ମିଥ୍ୟା, afterDigit2 = ମିଥ୍ୟା;

    ପାଇଁ (;;) {
        ଯେତେବେଳେ ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            ବାଛ (type) {
            ସ୍ଥିତି UIGNORE:
                afterDigit1 = ମିଥ୍ୟା;
                ଜାରି; /* ignore all but letters and digits */
            ସ୍ଥିତି ZERO:
                ଯଦି (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    ଯଦି (nextType == ZERO || nextType == NONZERO) {
                        ଜାରି; /* ignore leading zero before another digit */
                    }
                }
                ଭାଙ୍ଗ;
            ସ୍ଥିତି NONZERO:
                afterDigit1 = ସତ୍ୟ;
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                c1 = (ଅକ୍ଷର)type; /* lowercased letter */
                afterDigit1 = ମିଥ୍ୟା;
                ଭାଙ୍ଗ;
            }
            ଭାଙ୍ଗ; /* deliver c1 */
        }
        ଯେତେବେଳେ ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            ବାଛ (type) {
            ସ୍ଥିତି UIGNORE:
                afterDigit2 = ମିଥ୍ୟା;
                ଜାରି; /* ignore all but letters and digits */
            ସ୍ଥିତି ZERO:
                ଯଦି (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    ଯଦି (nextType == ZERO || nextType == NONZERO) {
                        ଜାରି; /* ignore leading zero before another digit */
                    }
                }
                ଭାଙ୍ଗ;
            ସ୍ଥିତି NONZERO:
                afterDigit2 = ସତ୍ୟ;
                ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                c2 = (ଅକ୍ଷର)type; /* lowercased letter */
                afterDigit2 = ମିଥ୍ୟା;
                ଭାଙ୍ଗ;
            }
            ଭାଙ୍ଗ; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        ଯଦି ((c1|c2)==0) {
            ଫେରନ୍ତୁ 0;
        }

        /* Case-insensitive comparison */
        rc = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(unsigned ଅକ୍ଷର)c1 - (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(unsigned ଅକ୍ଷର)c2;
        ଯଦି (rc != 0) {
            ଫେରନ୍ତୁ rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint32_t
findConverter(ଅଚଳ ଅକ୍ଷର *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା result;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    ଅକ୍ଷର strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    ଯଦି (!isUnnormalized) {
        ଯଦି (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            ଫେରନ୍ତୁ UINT32_MAX;
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

    ପାଇଁ (;;) {
        mid = (start + limit) / 2;
        ଯଦି (lastMid == mid) {   /* Have we moved? */
            ଭାଙ୍ଗ;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        ଯଦି (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        ଅନ୍ୟଥା {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        ଯଦି (result < 0) {
            limit = mid;
        } ଅନ୍ୟଥା ଯଦି (result > 0) {
            start = mid;
        } ଅନ୍ୟଥା {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            ଯଦି (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            ଯଦି (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            ଫେରନ୍ତୁ gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    ଫେରନ୍ତୁ UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
isAliasInList(ଅଚଳ ଅକ୍ଷର *alias, uint32_t listOffset) {
    ଯଦି (listOffset) {
        uint32_t currAlias;
        uint32_t list_ଗଣନା = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        ଅଚଳ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        ପାଇଁ (currAlias = 0; currAlias < list_ଗଣନା; currAlias++) {
            ଯଦି (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
ସ୍ଥିର uint32_t
findTaggedAliasListsOffset(ଅଚଳ ଅକ୍ଷର *alias, ଅଚଳ ଅକ୍ଷର *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_ଅଙ୍କ;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_ଅଙ୍କ = get_Tag_ସଂଖ୍ୟା(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_ଅଙ୍କ = findConverter(alias, nullptr, &myErr);
    ଯଦି (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    ଯଦି (tag_ଅଙ୍କ < (gMainTable.tagListSize - UCNV_ଅଙ୍କ_HIDDEN_TAGS) && conv_ଅଙ୍କ < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_ଅଙ୍କ*gMainTable.converterListSize + conv_ଅଙ୍କ];
        ଯଦି (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            ଫେରନ୍ତୁ listOffset;
        }
        ଯଦି (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            ପାଇଁ (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                ଯଦି (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_ଅଙ୍କ = idx/gMainTable.converterListSize;
                    uint32_t curr_Conv_ଅଙ୍କ = (idx - curr_Tag_ଅଙ୍କ*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tag_ଅଙ୍କ*gMainTable.converterListSize + curr_Conv_ଅଙ୍କ];
                    ଯଦି (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        ଫେରନ୍ତୁ tempListOffset;
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
        ଫେରନ୍ତୁ 0;
    }
    /* else converter or tag not found */

    ଫେରନ୍ତୁ UINT32_MAX;
}

/* Return the canonical name */
ସ୍ଥିର uint32_t
find_Tagged_Converter_ଅଙ୍କ(ଅଚଳ ଅକ୍ଷର *alias, ଅଚଳ ଅକ୍ଷର *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_ଅଙ୍କ;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_ଅଙ୍କ = get_Tag_ସଂଖ୍ୟା(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_ଅଙ୍କ = findConverter(alias, nullptr, &myErr);
    ଯଦି (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    ଯଦି (tag_ଅଙ୍କ < (gMainTable.tagListSize - UCNV_ଅଙ୍କ_HIDDEN_TAGS) && conv_ଅଙ୍କ < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_ଅଙ୍କ*gMainTable.converterListSize + conv_ଅଙ୍କ];
        ଯଦି (listOffset && isAliasInList(alias, listOffset)) {
            ଫେରନ୍ତୁ conv_ଅଙ୍କ;
        }
        ଯଦି (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_ଅଙ୍କ)*gMainTable.converterListSize;
            uint32_t convLimit = (tag_ଅଙ୍କ+1)*gMainTable.converterListSize;
            ପାଇଁ (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                ଯଦି (listOffset && isAliasInList(alias, listOffset)) {
                    ଫେରନ୍ତୁ idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    ଫେରନ୍ତୁ UINT32_MAX;
}

U_CAPI ଅଚଳ ଅକ୍ଷର *
ucnv_io_getConverterName(ଅଚଳ ଅକ୍ଷର *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    ଅଚଳ ଅକ୍ଷର *aliasTmp = alias;
    int32_t i = 0;
    ପାଇଁ (i = 0; i < 2; i++) {
        ଯଦି (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            ଯଦି (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
        }
        ଯଦି(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_ଅଙ୍କ = findConverter(aliasTmp, containsOption, pErrorCode);
            ଯଦି (conv_ଅଙ୍କ < gMainTable.converterListSize) {
                ଫେରନ୍ତୁ GET_STRING(gMainTable.converterList[conv_ଅଙ୍କ]);
            }
            /* else converter not found */
        } ଅନ୍ୟଥା {
            ଭାଙ୍ଗ;
        }
    }

    ଫେରନ୍ତୁ nullptr;
}

U_CDECL_BEGIN


ସ୍ଥିର int32_t U_CALLCONV
ucnv_io_ଗଣନା_Standard_Aliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    ଯଦି (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    ଫେରନ୍ତୁ value;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    ଯଦି (listOffset) {
        uint32_t list_ଗଣନା = gMainTable.taggedAliasLists[listOffset];
        ଅଚଳ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        ଯଦି (myContext->listIdx < list_ଗଣନା) {
            ଅଚଳ ଅକ୍ଷର *myStr = GET_STRING(currList[myContext->listIdx++]);
            ଯଦି (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            ଫେରନ୍ତୁ myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ଯଦି (resultLength) {
        *resultLength = 0;
    }
    ଫେରନ୍ତୁ nullptr;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
ସ୍ଥିର ଅଚଳ UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_ଗଣନା_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_nextStandardAliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(ଅଚଳ ଅକ୍ଷର *convName,
                       ଅଚଳ ଅକ୍ଷର *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    ଯଦି (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        ଯଦି (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(ଆକାର(UEnumeration)));
            ଯଦି (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, ଆକାର(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(ଆକାର(UAliasContext)));
            ଯଦି (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                ଫେରନ୍ତୁ nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    ଫେରନ୍ତୁ myEnum;
}

ସ୍ଥିର uint16_t
ucnv_io_ଗଣନା_Aliases(ଅଚଳ ଅକ୍ଷର *alias, UErrorCode *pErrorCode) {
    ଯଦି(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_ଅଙ୍କ = findConverter(alias, nullptr, pErrorCode);
        ଯଦି (conv_ଅଙ୍କ < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_ଅଙ୍କ];

            ଯଦି (listOffset) {
                ଫେରନ୍ତୁ gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର uint16_t
ucnv_io_getAliases(ଅଚଳ ଅକ୍ଷର *alias, uint16_t start, ଅଚଳ ଅକ୍ଷର **aliases, UErrorCode *pErrorCode) {
    ଯଦି(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_ଅଙ୍କ = findConverter(alias, nullptr, pErrorCode);
        ଯଦି (conv_ଅଙ୍କ < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_ଅଙ୍କ];

            ଯଦି (listOffset) {
                uint32_t list_ଗଣନା = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ଅଚଳ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                ପାଇଁ (currAlias = start; currAlias < list_ଗଣନା; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *
ucnv_io_getAlias(ଅଚଳ ଅକ୍ଷର *alias, uint16_t n, UErrorCode *pErrorCode) {
    ଯଦି(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_ଅଙ୍କ = findConverter(alias, nullptr, pErrorCode);
        ଯଦି (conv_ଅଙ୍କ < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_ଅଙ୍କ];

            ଯଦି (listOffset) {
                uint32_t list_ଗଣନା = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ଅଚଳ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                ଯଦି (n < list_ଗଣନା)  {
                    ଫେରନ୍ତୁ GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ଫେରନ୍ତୁ nullptr;
}

ସ୍ଥିର uint16_t
ucnv_io_ଗଣନା_Standards(UErrorCode *pErrorCode) {
    ଯଦି (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        ଫେରନ୍ତୁ static_cast<uint16_t>(gMainTable.tagListSize - UCNV_ଅଙ୍କ_HIDDEN_TAGS);
    }

    ଫେରନ୍ତୁ 0;
}

U_CAPI ଅଚଳ ଅକ୍ଷର * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    ଯଦି (haveAliasData(pErrorCode)) {
        ଯଦି (n < gMainTable.tagListSize - UCNV_ଅଙ୍କ_HIDDEN_TAGS) {
            ଫେରନ୍ତୁ GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    ଫେରନ୍ତୁ nullptr;
}

U_CAPI ଅଚଳ ଅକ୍ଷର * U_EXPORT2
ucnv_getStandardName(ଅଚଳ ଅକ୍ଷର *alias, ଅଚଳ ଅକ୍ଷର *standard, UErrorCode *pErrorCode) {
    ଯଦି (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        ଯଦି (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            ଅଚଳ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            ଯଦି (currList[0]) {
                ଫେରନ୍ତୁ GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    ଫେରନ୍ତୁ nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_ଗଣନା_Aliases(ଅଚଳ ଅକ୍ଷର *alias, UErrorCode *pErrorCode)
{
    ଫେରନ୍ତୁ ucnv_io_ଗଣନା_Aliases(alias, pErrorCode);
}


U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
ucnv_getAlias(ଅଚଳ ଅକ୍ଷର *alias, uint16_t n, UErrorCode *pErrorCode)
{
    ଫେରନ୍ତୁ ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucnv_getAliases(ଅଚଳ ଅକ୍ଷର *alias, ଅଚଳ ଅକ୍ଷର **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_ଗଣନା_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    ଫେରନ୍ତୁ ucnv_io_ଗଣନା_Standards(&err);
}

U_CAPI ଅଚଳ ଅକ୍ଷର * U_EXPORT2
ucnv_getCanonicalName(ଅଚଳ ଅକ୍ଷର *alias, ଅଚଳ ଅକ୍ଷର *standard, UErrorCode *pErrorCode) {
    ଯଦି (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_ଅଙ୍କ = find_Tagged_Converter_ଅଙ୍କ(alias, standard, pErrorCode);

        ଯଦି (conv_ଅଙ୍କ < gMainTable.converterListSize) {
            ଫେରନ୍ତୁ GET_STRING(gMainTable.converterList[conv_ଅଙ୍କ]);
        }
    }

    ଫେରନ୍ତୁ nullptr;
}

U_CDECL_BEGIN


ସ୍ଥିର int32_t U_CALLCONV
ucnv_io_ଗଣନା_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    ଫେରନ୍ତୁ gMainTable.converterListSize;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    ଯଦି (*myContext < gMainTable.converterListSize) {
        ଅଚଳ ଅକ୍ଷର *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        ଯଦି (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        ଫେରନ୍ତୁ myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ଯଦି (resultLength) {
        *resultLength = 0;
    }
    ଫେରନ୍ତୁ nullptr;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
ସ୍ଥିର ଅଚଳ UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_ଗଣନା_All_Converters,
    uenum_unextDefault,
    ucnv_io_nextAllConverters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    ଯଦି (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(ଆକାର(UEnumeration)));
        ଯଦି (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, ଆକାର(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(ଆକାର(uint16_t)));
        ଯଦି (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            ଫେରନ୍ତୁ nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    ଫେରନ୍ତୁ myEnum;
}

U_CAPI uint16_t
ucnv_io_ଗଣନା_Known_Converters(UErrorCode *pErrorCode) {
    ଯଦି (haveAliasData(pErrorCode)) {
        ଫେରନ୍ତୁ (uint16_t)gMainTable.converterListSize;
    }
    ଫେରନ୍ତୁ 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଅକ୍ଷର * U_CALLCONV StripForCompareFn(ଅକ୍ଷର *dst, ଅଚଳ ଅକ୍ଷର *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା TempRow {
    uint16_t strIndex, sortIndex;
} TempRow;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା TempAliasTable {
    ଅଚଳ ଅକ୍ଷର *chars;
    TempRow *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} TempAliasTable;

ଗଣନା {
    ଗଦା_ROW_CAPACITY=500
};

ସ୍ଥିର int32_t U_CALLCONV
io_compareRows(ଅଚଳ ଶୂନ୍ୟ *context, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    ଅକ୍ଷର stripped_ବାମ[UCNV_MAX_CONVERTER_NAME_LENGTH],
         stripped_ଡାହାଣ[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    ଅଚଳ ଅକ୍ଷର *chars=tempTable->chars;

    ଫେରନ୍ତୁ static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(stripped_ବାମ, chars + 2 * static_cast<ଅଚଳ TempRow*>(left)->strIndex),
        tempTable->stripForCompare(stripped_ଡାହାଣ, chars + 2 * static_cast<ଅଚଳ TempRow*>(ଡାହାଣ)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ଅଚଳ UDataSwapper *ds,
                 ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
                 UErrorCode *pErrorCode) {
    ଅଚଳ UDataInfo *pInfo;
    int32_t headerSize;

    ଅଚଳ uint16_t *inTable;
    ଅଚଳ uint32_t *inSectionSizes;
    uint32_t toc[offsets_ଗଣନା];
    uint32_t offsets[offsets_ଗଣନା]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, ଶୀର୍ଷ_Offset;

    TempRow rows[ଗଦା_ROW_CAPACITY];
    uint16_t resort[ଗଦା_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    /* check data format and format version */
    pInfo=(ଅଚଳ UDataInfo *)((ଅଚଳ ଅକ୍ଷର *)inData+4);
    ଯଦି(!(
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
        ଫେରନ୍ତୁ 0;
    }

    /* an alias table must contain at least the table of contents array */
    ଯଦି(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    inSectionSizes=(ଅଚଳ uint32_t *)((ଅଚଳ ଅକ୍ଷର *)inData+headerSize);
    inTable=(ଅଚଳ uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, ଆକାର(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    ଯଦି(tocLength<minTocLength || offsets_ଗଣନା<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* read the known part of the table of contents */
    ପାଇଁ(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, ଆକାର(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    ପାଇଁ(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    ଶୀର୍ଷ_Offset=offsets[i-1]+toc[i-1];

    ଯଦି(length>=0) {
        uint16_t *outTable;
        ଅଚଳ uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        ଯଦି((length-headerSize)<(2*(int32_t)ଶୀର୍ଷ_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }

        outTable=(uint16_t *)((ଅକ୍ଷର *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        ଯଦି(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            ଫେରନ୍ତୁ 0;
        }

        ଯଦି(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } ଅନ୍ୟଥା {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(ଅଚଳ ଅକ୍ଷର *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            ଯଦି(count<=ଗଦା_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } ଅନ୍ୟଥା {
                tempTable.rows=(TempRow *)uprv_malloc(count*ଆକାର(TempRow)+count*2);
                ଯଦି(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    ଫେରନ୍ତୁ 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            ଯଦି(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } ଅନ୍ୟଥା /* U_EBCDIC_FAMILY */ {
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

            ପାଇଁ(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, ଆକାର(TempRow),
                           io_compareRows, &tempTable,
                           ମିଥ୍ୟା, pErrorCode);

            ଯଦି(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                ଯଦି(p!=q) {
                    ପାଇଁ(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } ଅନ୍ୟଥା {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    ପାଇଁ(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    ପାଇଁ(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            ଯଦି(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            ଯଦି(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                ଫେରନ୍ତୁ 0;
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

    ଫେରନ୍ତୁ headerSize+2*(int32_t)ଶୀର୍ଷ_Offset;
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

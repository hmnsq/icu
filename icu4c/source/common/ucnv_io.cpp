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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"

#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "udatamem.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucnv_io.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"

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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ DATA_NAME[] = "cnvalias";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ DATA_TYPE[] = "icu";

ಸ್ಥಿರ UDataMemory *gAliasData=nullptr;
ಸ್ಥಿರ icu::UInitOnce gAliasDataInitOnce {};

ಗಣನಾ {
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
    offsets_ಎಣಿಕೆ,    /* length of the swapper's temporary offsets[] */
    minTocLength=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

ಸ್ಥಿರ ಧ್ರುವ UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
ಸ್ಥಿರ UConverterAlias gMainTable;

#define GET_STRING(idx) (ಧ್ರುವ ವರ್ಣ *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (ಧ್ರುವ ವರ್ಣ *)(gMainTable.normalizedStringTable + (idx))

ಸ್ಥಿರ UBool U_CALLCONV
isAcceptable(ಶೂನ್ಯ * /*context*/,
             ಧ್ರುವ ವರ್ಣ * /*type*/, ಧ್ರುವ ವರ್ಣ * /*name*/,
             ಧ್ರುವ UDataInfo *pInfo) {
    ಪ್ರತ್ಯಾಗಮ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

ಸ್ಥಿರ UBool U_CALLCONV ucnv_io_cleanup()
{
    ಯದಿ (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, ಪರಿಮಾಣ(gMainTable));

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;                   /* Everything was cleaned up */
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV initAliasData(UErrorCode &errCode) {
    UDataMemory *data;
    ಧ್ರುವ uint16_t *table;
    ಧ್ರುವ uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t sizeOfData;
    int32_t sizeOfTOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(gAliasData == nullptr);
    data = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &errCode);
    ಯದಿ (U_FAILURE(errCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    sectionSizes = static_cast<ಧ್ರುವ uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    ಯದಿ (isDataLengthKnown && dataLength <= int32_t(ಪರಿಮಾಣ(sectionSizes[0]))) {
        // We don't even have a TOC!
        ಗಚ್ಛ invalidFormat;
    }
    table = reinterpret_cast<ಧ್ರುವ uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * ಪರಿಮಾಣ(sectionSizes[0]));
    ಯದಿ (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
        // We don't have a whole TOC!
        ಗಚ್ಛ invalidFormat;
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

    ಯದಿ (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    ಪ್ರತಿ (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * ಪರಿಮಾಣ(table[0]);
    }
    ಯದಿ (isDataLengthKnown && dataLength < sizeOfData) {
        // Truncated file!
        ಗಚ್ಛ invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (ಪರಿಮಾಣ(uint32_t)/ಪರಿಮಾಣ(uint16_t));
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
    ಯದಿ (gMainTable.optionTableSize > 0
        && reinterpret_cast<ಧ್ರುವ UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_ಎಣಿಕೆ)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<ಧ್ರುವ UConverterAliasOptions*>(table + currOffset);
    }
    ಅನ್ಯಥಾ {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    ಪ್ರತ್ಯಾಗಮ;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


ಸ್ಥಿರ UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(*pErrorCode);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UBool
isAlias(ಧ್ರುವ ವರ್ಣ *alias, UErrorCode *pErrorCode) {
    ಯದಿ(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ *alias != 0;
}

ಸ್ಥಿರ uint32_t get_Tag_ಸಂಖ್ಯೆ(ಧ್ರುವ ವರ್ಣ *tagname) {
    ಯದಿ (gMainTable.tagList) {
        uint32_t tag_ಅಂಕೆ;
        ಪ್ರತಿ (tag_ಅಂಕೆ = 0; tag_ಅಂಕೆ < gMainTable.tagListSize; tag_ಅಂಕೆ++) {
            ಯದಿ (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_ಅಂಕೆ]), tagname)) {
                ಪ್ರತ್ಯಾಗಮ tag_ಅಂಕೆ;
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
ಗಣನಾ {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
ಸ್ಥಿರ ಧ್ರುವ uint8_t asciiTypes[128] = {
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
ಸ್ಥಿರ ಧ್ರುವ uint8_t ebcdicTypes[128] = {
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

#ಯದಿ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#ನೋಚೇದ್ಯದಿ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#ಅನ್ಯಥಾ
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI ವರ್ಣ* U_CALLCONV
ucnv_io_stripASCIIForCompare(ವರ್ಣ* dst U_LIFETIME_BOUND, ಧ್ರುವ ವರ್ಣ* name) {
    ವರ್ಣ *dstItr = dst;
    uint8_t type, nextType;
    ವರ್ಣ c1;
    UBool afterDigit = ಅಸತ್ಯ;

    ಯಾವತ್ ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        ವಿಕಲ್ಪ (type) {
        ಪಕ್ಷ UIGNORE:
            afterDigit = ಅಸತ್ಯ;
            ಅನುವರ್ತ; /* ignore all but letters and digits */
        ಪಕ್ಷ ZERO:
            ಯದಿ (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                ಯದಿ (nextType == ZERO || nextType == NONZERO) {
                    ಅನುವರ್ತ; /* ignore leading zero before another digit */
                }
            }
            ಭಙ್ಗ;
        ಪಕ್ಷ NONZERO:
            afterDigit = ಸತ್ಯ;
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            c1 = (ವರ್ಣ)type; /* lowercased letter */
            afterDigit = ಅಸತ್ಯ;
            ಭಙ್ಗ;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ಪ್ರತ್ಯಾಗಮ dst;
}

U_CAPI ವರ್ಣ* U_CALLCONV
ucnv_io_stripEBCDICForCompare(ವರ್ಣ* dst U_LIFETIME_BOUND, ಧ್ರುವ ವರ್ಣ* name) {
    ವರ್ಣ *dstItr = dst;
    uint8_t type, nextType;
    ವರ್ಣ c1;
    UBool afterDigit = ಅಸತ್ಯ;

    ಯಾವತ್ ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        ವಿಕಲ್ಪ (type) {
        ಪಕ್ಷ UIGNORE:
            afterDigit = ಅಸತ್ಯ;
            ಅನುವರ್ತ; /* ignore all but letters and digits */
        ಪಕ್ಷ ZERO:
            ಯದಿ (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                ಯದಿ (nextType == ZERO || nextType == NONZERO) {
                    ಅನುವರ್ತ; /* ignore leading zero before another digit */
                }
            }
            ಭಙ್ಗ;
        ಪಕ್ಷ NONZERO:
            afterDigit = ಸತ್ಯ;
            ಭಙ್ಗ;
        ಸ್ವಭಾವ:
            c1 = (ವರ್ಣ)type; /* lowercased letter */
            afterDigit = ಅಸತ್ಯ;
            ಭಙ್ಗ;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ಪ್ರತ್ಯಾಗಮ dst;
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
U_CAPI ಪೂರ್ಣಾಙ್ಕ U_EXPORT2
ucnv_compareNames(ಧ್ರುವ ವರ್ಣ *name1, ಧ್ರುವ ವರ್ಣ *name2) {
    ಪೂರ್ಣಾಙ್ಕ rc;
    uint8_t type, nextType;
    ವರ್ಣ c1, c2;
    UBool afterDigit1 = ಅಸತ್ಯ, afterDigit2 = ಅಸತ್ಯ;

    ಪ್ರತಿ (;;) {
        ಯಾವತ್ ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            ವಿಕಲ್ಪ (type) {
            ಪಕ್ಷ UIGNORE:
                afterDigit1 = ಅಸತ್ಯ;
                ಅನುವರ್ತ; /* ignore all but letters and digits */
            ಪಕ್ಷ ZERO:
                ಯದಿ (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    ಯದಿ (nextType == ZERO || nextType == NONZERO) {
                        ಅನುವರ್ತ; /* ignore leading zero before another digit */
                    }
                }
                ಭಙ್ಗ;
            ಪಕ್ಷ NONZERO:
                afterDigit1 = ಸತ್ಯ;
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                c1 = (ವರ್ಣ)type; /* lowercased letter */
                afterDigit1 = ಅಸತ್ಯ;
                ಭಙ್ಗ;
            }
            ಭಙ್ಗ; /* deliver c1 */
        }
        ಯಾವತ್ ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            ವಿಕಲ್ಪ (type) {
            ಪಕ್ಷ UIGNORE:
                afterDigit2 = ಅಸತ್ಯ;
                ಅನುವರ್ತ; /* ignore all but letters and digits */
            ಪಕ್ಷ ZERO:
                ಯದಿ (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    ಯದಿ (nextType == ZERO || nextType == NONZERO) {
                        ಅನುವರ್ತ; /* ignore leading zero before another digit */
                    }
                }
                ಭಙ್ಗ;
            ಪಕ್ಷ NONZERO:
                afterDigit2 = ಸತ್ಯ;
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                c2 = (ವರ್ಣ)type; /* lowercased letter */
                afterDigit2 = ಅಸತ್ಯ;
                ಭಙ್ಗ;
            }
            ಭಙ್ಗ; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        ಯದಿ ((c1|c2)==0) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* Case-insensitive comparison */
        rc = (ಪೂರ್ಣಾಙ್ಕ)(unsigned ವರ್ಣ)c1 - (ಪೂರ್ಣಾಙ್ಕ)(unsigned ವರ್ಣ)c2;
        ಯದಿ (rc != 0) {
            ಪ್ರತ್ಯಾಗಮ rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint32_t
findConverter(ಧ್ರುವ ವರ್ಣ *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    ಪೂರ್ಣಾಙ್ಕ result;
    ಪೂರ್ಣಾಙ್ಕ isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    ವರ್ಣ strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    ಯದಿ (!isUnnormalized) {
        ಯದಿ (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            ಪ್ರತ್ಯಾಗಮ UINT32_MAX;
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

    ಪ್ರತಿ (;;) {
        mid = (start + limit) / 2;
        ಯದಿ (lastMid == mid) {   /* Have we moved? */
            ಭಙ್ಗ;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        ಯದಿ (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        ಅನ್ಯಥಾ {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        ಯದಿ (result < 0) {
            limit = mid;
        } ಅನ್ಯಥಾ ಯದಿ (result > 0) {
            start = mid;
        } ಅನ್ಯಥಾ {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            ಯದಿ (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            ಯದಿ (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            ಪ್ರತ್ಯಾಗಮ gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    ಪ್ರತ್ಯಾಗಮ UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UBool
isAliasInList(ಧ್ರುವ ವರ್ಣ *alias, uint32_t listOffset) {
    ಯದಿ (listOffset) {
        uint32_t currAlias;
        uint32_t list_ಎಣಿಕೆ = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        ಧ್ರುವ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        ಪ್ರತಿ (currAlias = 0; currAlias < list_ಎಣಿಕೆ; currAlias++) {
            ಯದಿ (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
ಸ್ಥಿರ uint32_t
findTaggedAliasListsOffset(ಧ್ರುವ ವರ್ಣ *alias, ಧ್ರುವ ವರ್ಣ *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_ಅಂಕೆ;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_ಅಂಕೆ = get_Tag_ಸಂಖ್ಯೆ(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_ಅಂಕೆ = findConverter(alias, nullptr, &myErr);
    ಯದಿ (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    ಯದಿ (tag_ಅಂಕೆ < (gMainTable.tagListSize - UCNV_ಅಂಕೆ_HIDDEN_TAGS) && conv_ಅಂಕೆ < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_ಅಂಕೆ*gMainTable.converterListSize + conv_ಅಂಕೆ];
        ಯದಿ (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            ಪ್ರತ್ಯಾಗಮ listOffset;
        }
        ಯದಿ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            ಪ್ರತಿ (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                ಯದಿ (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_ಅಂಕೆ = idx/gMainTable.converterListSize;
                    uint32_t curr_Conv_ಅಂಕೆ = (idx - curr_Tag_ಅಂಕೆ*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tag_ಅಂಕೆ*gMainTable.converterListSize + curr_Conv_ಅಂಕೆ];
                    ಯದಿ (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        ಪ್ರತ್ಯಾಗಮ tempListOffset;
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
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    /* else converter or tag not found */

    ಪ್ರತ್ಯಾಗಮ UINT32_MAX;
}

/* Return the canonical name */
ಸ್ಥಿರ uint32_t
find_Tagged_Converter_ಅಂಕೆ(ಧ್ರುವ ವರ್ಣ *alias, ಧ್ರುವ ವರ್ಣ *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_ಅಂಕೆ;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_ಅಂಕೆ = get_Tag_ಸಂಖ್ಯೆ(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_ಅಂಕೆ = findConverter(alias, nullptr, &myErr);
    ಯದಿ (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    ಯದಿ (tag_ಅಂಕೆ < (gMainTable.tagListSize - UCNV_ಅಂಕೆ_HIDDEN_TAGS) && conv_ಅಂಕೆ < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_ಅಂಕೆ*gMainTable.converterListSize + conv_ಅಂಕೆ];
        ಯದಿ (listOffset && isAliasInList(alias, listOffset)) {
            ಪ್ರತ್ಯಾಗಮ conv_ಅಂಕೆ;
        }
        ಯದಿ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_ಅಂಕೆ)*gMainTable.converterListSize;
            uint32_t convLimit = (tag_ಅಂಕೆ+1)*gMainTable.converterListSize;
            ಪ್ರತಿ (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                ಯದಿ (listOffset && isAliasInList(alias, listOffset)) {
                    ಪ್ರತ್ಯಾಗಮ idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    ಪ್ರತ್ಯಾಗಮ UINT32_MAX;
}

U_CAPI ಧ್ರುವ ವರ್ಣ *
ucnv_io_getConverterName(ಧ್ರುವ ವರ್ಣ *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    ಧ್ರುವ ವರ್ಣ *aliasTmp = alias;
    int32_t i = 0;
    ಪ್ರತಿ (i = 0; i < 2; i++) {
        ಯದಿ (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            ಯದಿ (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } ಅನ್ಯಥಾ {
                ಭಙ್ಗ;
            }
        }
        ಯದಿ(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_ಅಂಕೆ = findConverter(aliasTmp, containsOption, pErrorCode);
            ಯದಿ (conv_ಅಂಕೆ < gMainTable.converterListSize) {
                ಪ್ರತ್ಯಾಗಮ GET_STRING(gMainTable.converterList[conv_ಅಂಕೆ]);
            }
            /* else converter not found */
        } ಅನ್ಯಥಾ {
            ಭಙ್ಗ;
        }
    }

    ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CDECL_BEGIN


ಸ್ಥಿರ int32_t U_CALLCONV
ucnv_io_ಎಣಿಕೆ_Standard_Aliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    ಯದಿ (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    ಪ್ರತ್ಯಾಗಮ value;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    ಯದಿ (listOffset) {
        uint32_t list_ಎಣಿಕೆ = gMainTable.taggedAliasLists[listOffset];
        ಧ್ರುವ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        ಯದಿ (myContext->listIdx < list_ಎಣಿಕೆ) {
            ಧ್ರುವ ವರ್ಣ *myStr = GET_STRING(currList[myContext->listIdx++]);
            ಯದಿ (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            ಪ್ರತ್ಯಾಗಮ myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ಯದಿ (resultLength) {
        *resultLength = 0;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
ಸ್ಥಿರ ಧ್ರುವ UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_ಎಣಿಕೆ_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_nextStandardAliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(ಧ್ರುವ ವರ್ಣ *convName,
                       ಧ್ರುವ ವರ್ಣ *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    ಯದಿ (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        ಯದಿ (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(ಪರಿಮಾಣ(UEnumeration)));
            ಯದಿ (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, ಪರಿಮಾಣ(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(ಪರಿಮಾಣ(UAliasContext)));
            ಯದಿ (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    ಪ್ರತ್ಯಾಗಮ myEnum;
}

ಸ್ಥಿರ uint16_t
ucnv_io_ಎಣಿಕೆ_Aliases(ಧ್ರುವ ವರ್ಣ *alias, UErrorCode *pErrorCode) {
    ಯದಿ(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_ಅಂಕೆ = findConverter(alias, nullptr, pErrorCode);
        ಯದಿ (conv_ಅಂಕೆ < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_ಅಂಕೆ];

            ಯದಿ (listOffset) {
                ಪ್ರತ್ಯಾಗಮ gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ uint16_t
ucnv_io_getAliases(ಧ್ರುವ ವರ್ಣ *alias, uint16_t start, ಧ್ರುವ ವರ್ಣ **aliases, UErrorCode *pErrorCode) {
    ಯದಿ(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_ಅಂಕೆ = findConverter(alias, nullptr, pErrorCode);
        ಯದಿ (conv_ಅಂಕೆ < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_ಅಂಕೆ];

            ಯದಿ (listOffset) {
                uint32_t list_ಎಣಿಕೆ = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ಧ್ರುವ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                ಪ್ರತಿ (currAlias = start; currAlias < list_ಎಣಿಕೆ; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *
ucnv_io_getAlias(ಧ್ರುವ ವರ್ಣ *alias, uint16_t n, UErrorCode *pErrorCode) {
    ಯದಿ(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_ಅಂಕೆ = findConverter(alias, nullptr, pErrorCode);
        ಯದಿ (conv_ಅಂಕೆ < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_ಅಂಕೆ];

            ಯದಿ (listOffset) {
                uint32_t list_ಎಣಿಕೆ = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ಧ್ರುವ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                ಯದಿ (n < list_ಎಣಿಕೆ)  {
                    ಪ್ರತ್ಯಾಗಮ GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಸ್ಥಿರ uint16_t
ucnv_io_ಎಣಿಕೆ_Standards(UErrorCode *pErrorCode) {
    ಯದಿ (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        ಪ್ರತ್ಯಾಗಮ static_cast<uint16_t>(gMainTable.tagListSize - UCNV_ಅಂಕೆ_HIDDEN_TAGS);
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

U_CAPI ಧ್ರುವ ವರ್ಣ * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    ಯದಿ (haveAliasData(pErrorCode)) {
        ಯದಿ (n < gMainTable.tagListSize - UCNV_ಅಂಕೆ_HIDDEN_TAGS) {
            ಪ್ರತ್ಯಾಗಮ GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CAPI ಧ್ರುವ ವರ್ಣ * U_EXPORT2
ucnv_getStandardName(ಧ್ರುವ ವರ್ಣ *alias, ಧ್ರುವ ವರ್ಣ *standard, UErrorCode *pErrorCode) {
    ಯದಿ (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        ಯದಿ (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            ಧ್ರುವ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            ಯದಿ (currList[0]) {
                ಪ್ರತ್ಯಾಗಮ GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_ಎಣಿಕೆ_Aliases(ಧ್ರುವ ವರ್ಣ *alias, UErrorCode *pErrorCode)
{
    ಪ್ರತ್ಯಾಗಮ ucnv_io_ಎಣಿಕೆ_Aliases(alias, pErrorCode);
}


U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
ucnv_getAlias(ಧ್ರುವ ವರ್ಣ *alias, uint16_t n, UErrorCode *pErrorCode)
{
    ಪ್ರತ್ಯಾಗಮ ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_getAliases(ಧ್ರುವ ವರ್ಣ *alias, ಧ್ರುವ ವರ್ಣ **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_ಎಣಿಕೆ_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    ಪ್ರತ್ಯಾಗಮ ucnv_io_ಎಣಿಕೆ_Standards(&err);
}

U_CAPI ಧ್ರುವ ವರ್ಣ * U_EXPORT2
ucnv_getCanonicalName(ಧ್ರುವ ವರ್ಣ *alias, ಧ್ರುವ ವರ್ಣ *standard, UErrorCode *pErrorCode) {
    ಯದಿ (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_ಅಂಕೆ = find_Tagged_Converter_ಅಂಕೆ(alias, standard, pErrorCode);

        ಯದಿ (conv_ಅಂಕೆ < gMainTable.converterListSize) {
            ಪ್ರತ್ಯಾಗಮ GET_STRING(gMainTable.converterList[conv_ಅಂಕೆ]);
        }
    }

    ಪ್ರತ್ಯಾಗಮ nullptr;
}

U_CDECL_BEGIN


ಸ್ಥಿರ int32_t U_CALLCONV
ucnv_io_ಎಣಿಕೆ_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    ಪ್ರತ್ಯಾಗಮ gMainTable.converterListSize;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    ಯದಿ (*myContext < gMainTable.converterListSize) {
        ಧ್ರುವ ವರ್ಣ *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        ಯದಿ (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        ಪ್ರತ್ಯಾಗಮ myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ಯದಿ (resultLength) {
        *resultLength = 0;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
ಸ್ಥಿರ ಧ್ರುವ UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_ಎಣಿಕೆ_All_Converters,
    uenum_unextDefault,
    ucnv_io_nextAllConverters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    ಯದಿ (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(ಪರಿಮಾಣ(UEnumeration)));
        ಯದಿ (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, ಪರಿಮಾಣ(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(ಪರಿಮಾಣ(uint16_t)));
        ಯದಿ (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    ಪ್ರತ್ಯಾಗಮ myEnum;
}

U_CAPI uint16_t
ucnv_io_ಎಣಿಕೆ_Known_Converters(UErrorCode *pErrorCode) {
    ಯದಿ (haveAliasData(pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ (uint16_t)gMainTable.converterListSize;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

ಪ್ರಕಾರನಿರ್ದೇಶ ವರ್ಣ * U_CALLCONV StripForCompareFn(ವರ್ಣ *dst, ಧ್ರುವ ವರ್ಣ *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ TempRow {
    uint16_t strIndex, sortIndex;
} TempRow;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ TempAliasTable {
    ಧ್ರುವ ವರ್ಣ *chars;
    TempRow *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} TempAliasTable;

ಗಣನಾ {
    ರಾಶಿ_ROW_CAPACITY=500
};

ಸ್ಥಿರ int32_t U_CALLCONV
io_compareRows(ಧ್ರುವ ಶೂನ್ಯ *context, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    ವರ್ಣ stripped_ಎಡ[UCNV_MAX_CONVERTER_NAME_LENGTH],
         stripped_ಬಲ[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    ಧ್ರುವ ವರ್ಣ *chars=tempTable->chars;

    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(stripped_ಎಡ, chars + 2 * static_cast<ಧ್ರುವ TempRow*>(left)->strIndex),
        tempTable->stripForCompare(stripped_ಬಲ, chars + 2 * static_cast<ಧ್ರುವ TempRow*>(ಬಲ)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ಧ್ರುವ UDataSwapper *ds,
                 ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                 UErrorCode *pErrorCode) {
    ಧ್ರುವ UDataInfo *pInfo;
    int32_t headerSize;

    ಧ್ರುವ uint16_t *inTable;
    ಧ್ರುವ uint32_t *inSectionSizes;
    uint32_t toc[offsets_ಎಣಿಕೆ];
    uint32_t offsets[offsets_ಎಣಿಕೆ]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, ತುದಿ_Offset;

    TempRow rows[ರಾಶಿ_ROW_CAPACITY];
    uint16_t resort[ರಾಶಿ_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* check data format and format version */
    pInfo=(ಧ್ರುವ UDataInfo *)((ಧ್ರುವ ವರ್ಣ *)inData+4);
    ಯದಿ(!(
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
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* an alias table must contain at least the table of contents array */
    ಯದಿ(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    inSectionSizes=(ಧ್ರುವ uint32_t *)((ಧ್ರುವ ವರ್ಣ *)inData+headerSize);
    inTable=(ಧ್ರುವ uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, ಪರಿಮಾಣ(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    ಯದಿ(tocLength<minTocLength || offsets_ಎಣಿಕೆ<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* read the known part of the table of contents */
    ಪ್ರತಿ(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, ಪರಿಮಾಣ(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    ಪ್ರತಿ(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    ತುದಿ_Offset=offsets[i-1]+toc[i-1];

    ಯದಿ(length>=0) {
        uint16_t *outTable;
        ಧ್ರುವ uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        ಯದಿ((length-headerSize)<(2*(int32_t)ತುದಿ_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        outTable=(uint16_t *)((ವರ್ಣ *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        ಯದಿ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } ಅನ್ಯಥಾ {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(ಧ್ರುವ ವರ್ಣ *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            ಯದಿ(count<=ರಾಶಿ_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } ಅನ್ಯಥಾ {
                tempTable.rows=(TempRow *)uprv_malloc(count*ಪರಿಮಾಣ(TempRow)+count*2);
                ಯದಿ(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            ಯದಿ(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } ಅನ್ಯಥಾ /* U_EBCDIC_FAMILY */ {
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

            ಪ್ರತಿ(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, ಪರಿಮಾಣ(TempRow),
                           io_compareRows, &tempTable,
                           ಅಸತ್ಯ, pErrorCode);

            ಯದಿ(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                ಯದಿ(p!=q) {
                    ಪ್ರತಿ(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } ಅನ್ಯಥಾ {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    ಪ್ರತಿ(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    ಪ್ರತಿ(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            ಯದಿ(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            ಯದಿ(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                ಪ್ರತ್ಯಾಗಮ 0;
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

    ಪ್ರತ್ಯಾಗಮ headerSize+2*(int32_t)ತುದಿ_Offset;
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

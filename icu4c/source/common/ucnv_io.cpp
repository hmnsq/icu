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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_CONVERSION

#અન્તર્ભાવ "unicode/ucnv.h"
#અન્તર્ભાવ "unicode/udata.h"

#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uarrsort.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "udataswp.h"
#અન્તર્ભાવ "udatamem.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ucnv_io.h"
#અન્તર્ભાવ "uenumimp.h"
#અન્તર્ભાવ "ucln_cmn.h"

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
પ્રકારવ્યાખ્યા સંરચના UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

સ્થિર અચલ અક્ષર DATA_NAME[] = "cnvalias";
સ્થિર અચલ અક્ષર DATA_TYPE[] = "icu";

સ્થિર UDataMemory *gAliasData=nullptr;
સ્થિર icu::UInitOnce gAliasDataInitOnce {};

ગણના {
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
    offsets_ગણતરી,    /* length of the swapper's temporary offsets[] */
    minTocLength=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

સ્થિર અચલ UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
સ્થિર UConverterAlias gMainTable;

#define GET_STRING(idx) (અચલ અક્ષર *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (અચલ અક્ષર *)(gMainTable.normalizedStringTable + (idx))

સ્થિર UBool U_CALLCONV
isAcceptable(શૂન્ય * /*context*/,
             અચલ અક્ષર * /*type*/, અચલ અક્ષર * /*name*/,
             અચલ UDataInfo *pInfo) {
    પાછા
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

સ્થિર UBool U_CALLCONV ucnv_io_cleanup()
{
    જો (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, કદ(gMainTable));

    પાછા સાચું;                   /* Everything was cleaned up */
}

સ્થિર શૂન્ય U_CALLCONV initAliasData(UErrorCode &errCode) {
    UDataMemory *data;
    અચલ uint16_t *table;
    અચલ uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t sizeOfData;
    int32_t sizeOfTOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(gAliasData == nullptr);
    data = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &errCode);
    જો (U_FAILURE(errCode)) {
        પાછા;
    }

    sectionSizes = static_cast<અચલ uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    જો (isDataLengthKnown && dataLength <= int32_t(કદ(sectionSizes[0]))) {
        // We don't even have a TOC!
        જાઓ invalidFormat;
    }
    table = reinterpret_cast<અચલ uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * કદ(sectionSizes[0]));
    જો (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
        // We don't have a whole TOC!
        જાઓ invalidFormat;
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

    જો (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    માટે (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * કદ(table[0]);
    }
    જો (isDataLengthKnown && dataLength < sizeOfData) {
        // Truncated file!
        જાઓ invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (કદ(uint32_t)/કદ(uint16_t));
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
    જો (gMainTable.optionTableSize > 0
        && reinterpret_cast<અચલ UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_ગણતરી)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<અચલ UConverterAliasOptions*>(table + currOffset);
    }
    નહીંતર {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    પાછા;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


સ્થિર UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    પાછા U_SUCCESS(*pErrorCode);
}

સ્થિર અંતઃરેખ UBool
isAlias(અચલ અક્ષર *alias, UErrorCode *pErrorCode) {
    જો(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા ખોટું;
    }
    પાછા *alias != 0;
}

સ્થિર uint32_t get_Tag_સંખ્યા(અચલ અક્ષર *tagname) {
    જો (gMainTable.tagList) {
        uint32_t tag_અંક;
        માટે (tag_અંક = 0; tag_અંક < gMainTable.tagListSize; tag_અંક++) {
            જો (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_અંક]), tagname)) {
                પાછા tag_અંક;
            }
        }
    }

    પાછા UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
ગણના {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
સ્થિર અચલ uint8_t asciiTypes[128] = {
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
સ્થિર અચલ uint8_t ebcdicTypes[128] = {
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

#જો U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#અથવાજો U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#નહીંતર
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI અક્ષર* U_CALLCONV
ucnv_io_stripASCIIForCompare(અક્ષર* dst U_LIFETIME_BOUND, અચલ અક્ષર* name) {
    અક્ષર *dstItr = dst;
    uint8_t type, nextType;
    અક્ષર c1;
    UBool afterDigit = ખોટું;

    જ્યારે ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        પસંદગી (type) {
        સ્થિતિ UIGNORE:
            afterDigit = ખોટું;
            ચાલુ; /* ignore all but letters and digits */
        સ્થિતિ ZERO:
            જો (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                જો (nextType == ZERO || nextType == NONZERO) {
                    ચાલુ; /* ignore leading zero before another digit */
                }
            }
            તોડો;
        સ્થિતિ NONZERO:
            afterDigit = સાચું;
            તોડો;
        મૂળભૂત:
            c1 = (અક્ષર)type; /* lowercased letter */
            afterDigit = ખોટું;
            તોડો;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    પાછા dst;
}

U_CAPI અક્ષર* U_CALLCONV
ucnv_io_stripEBCDICForCompare(અક્ષર* dst U_LIFETIME_BOUND, અચલ અક્ષર* name) {
    અક્ષર *dstItr = dst;
    uint8_t type, nextType;
    અક્ષર c1;
    UBool afterDigit = ખોટું;

    જ્યારે ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        પસંદગી (type) {
        સ્થિતિ UIGNORE:
            afterDigit = ખોટું;
            ચાલુ; /* ignore all but letters and digits */
        સ્થિતિ ZERO:
            જો (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                જો (nextType == ZERO || nextType == NONZERO) {
                    ચાલુ; /* ignore leading zero before another digit */
                }
            }
            તોડો;
        સ્થિતિ NONZERO:
            afterDigit = સાચું;
            તોડો;
        મૂળભૂત:
            c1 = (અક્ષર)type; /* lowercased letter */
            afterDigit = ખોટું;
            તોડો;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    પાછા dst;
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
U_CAPI પૂર્ણાંક U_EXPORT2
ucnv_compareNames(અચલ અક્ષર *name1, અચલ અક્ષર *name2) {
    પૂર્ણાંક rc;
    uint8_t type, nextType;
    અક્ષર c1, c2;
    UBool afterDigit1 = ખોટું, afterDigit2 = ખોટું;

    માટે (;;) {
        જ્યારે ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            પસંદગી (type) {
            સ્થિતિ UIGNORE:
                afterDigit1 = ખોટું;
                ચાલુ; /* ignore all but letters and digits */
            સ્થિતિ ZERO:
                જો (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    જો (nextType == ZERO || nextType == NONZERO) {
                        ચાલુ; /* ignore leading zero before another digit */
                    }
                }
                તોડો;
            સ્થિતિ NONZERO:
                afterDigit1 = સાચું;
                તોડો;
            મૂળભૂત:
                c1 = (અક્ષર)type; /* lowercased letter */
                afterDigit1 = ખોટું;
                તોડો;
            }
            તોડો; /* deliver c1 */
        }
        જ્યારે ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            પસંદગી (type) {
            સ્થિતિ UIGNORE:
                afterDigit2 = ખોટું;
                ચાલુ; /* ignore all but letters and digits */
            સ્થિતિ ZERO:
                જો (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    જો (nextType == ZERO || nextType == NONZERO) {
                        ચાલુ; /* ignore leading zero before another digit */
                    }
                }
                તોડો;
            સ્થિતિ NONZERO:
                afterDigit2 = સાચું;
                તોડો;
            મૂળભૂત:
                c2 = (અક્ષર)type; /* lowercased letter */
                afterDigit2 = ખોટું;
                તોડો;
            }
            તોડો; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        જો ((c1|c2)==0) {
            પાછા 0;
        }

        /* Case-insensitive comparison */
        rc = (પૂર્ણાંક)(unsigned અક્ષર)c1 - (પૂર્ણાંક)(unsigned અક્ષર)c2;
        જો (rc != 0) {
            પાછા rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
સ્થિર અંતઃરેખ uint32_t
findConverter(અચલ અક્ષર *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    પૂર્ણાંક result;
    પૂર્ણાંક isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    અક્ષર strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    જો (!isUnnormalized) {
        જો (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            પાછા UINT32_MAX;
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

    માટે (;;) {
        mid = (start + limit) / 2;
        જો (lastMid == mid) {   /* Have we moved? */
            તોડો;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        જો (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        નહીંતર {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        જો (result < 0) {
            limit = mid;
        } નહીંતર જો (result > 0) {
            start = mid;
        } નહીંતર {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            જો (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            જો (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            પાછા gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    પાછા UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
સ્થિર અંતઃરેખ UBool
isAliasInList(અચલ અક્ષર *alias, uint32_t listOffset) {
    જો (listOffset) {
        uint32_t currAlias;
        uint32_t list_ગણતરી = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        અચલ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        માટે (currAlias = 0; currAlias < list_ગણતરી; currAlias++) {
            જો (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                પાછા સાચું;
            }
        }
    }
    પાછા ખોટું;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
સ્થિર uint32_t
findTaggedAliasListsOffset(અચલ અક્ષર *alias, અચલ અક્ષર *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_અંક;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_અંક = get_Tag_સંખ્યા(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_અંક = findConverter(alias, nullptr, &myErr);
    જો (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    જો (tag_અંક < (gMainTable.tagListSize - UCNV_અંક_HIDDEN_TAGS) && conv_અંક < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_અંક*gMainTable.converterListSize + conv_અંક];
        જો (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            પાછા listOffset;
        }
        જો (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            માટે (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                જો (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_અંક = idx/gMainTable.converterListSize;
                    uint32_t curr_Conv_અંક = (idx - curr_Tag_અંક*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tag_અંક*gMainTable.converterListSize + curr_Conv_અંક];
                    જો (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        પાછા tempListOffset;
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
        પાછા 0;
    }
    /* else converter or tag not found */

    પાછા UINT32_MAX;
}

/* Return the canonical name */
સ્થિર uint32_t
find_Tagged_Converter_અંક(અચલ અક્ષર *alias, અચલ અક્ષર *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_અંક;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_અંક = get_Tag_સંખ્યા(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_અંક = findConverter(alias, nullptr, &myErr);
    જો (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    જો (tag_અંક < (gMainTable.tagListSize - UCNV_અંક_HIDDEN_TAGS) && conv_અંક < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_અંક*gMainTable.converterListSize + conv_અંક];
        જો (listOffset && isAliasInList(alias, listOffset)) {
            પાછા conv_અંક;
        }
        જો (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_અંક)*gMainTable.converterListSize;
            uint32_t convLimit = (tag_અંક+1)*gMainTable.converterListSize;
            માટે (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                જો (listOffset && isAliasInList(alias, listOffset)) {
                    પાછા idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    પાછા UINT32_MAX;
}

U_CAPI અચલ અક્ષર *
ucnv_io_getConverterName(અચલ અક્ષર *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    અચલ અક્ષર *aliasTmp = alias;
    int32_t i = 0;
    માટે (i = 0; i < 2; i++) {
        જો (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            જો (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } નહીંતર {
                તોડો;
            }
        }
        જો(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_અંક = findConverter(aliasTmp, containsOption, pErrorCode);
            જો (conv_અંક < gMainTable.converterListSize) {
                પાછા GET_STRING(gMainTable.converterList[conv_અંક]);
            }
            /* else converter not found */
        } નહીંતર {
            તોડો;
        }
    }

    પાછા nullptr;
}

U_CDECL_BEGIN


સ્થિર int32_t U_CALLCONV
ucnv_io_ગણતરી_Standard_Aliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    જો (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    પાછા value;
}

સ્થિર અચલ અક્ષર * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    જો (listOffset) {
        uint32_t list_ગણતરી = gMainTable.taggedAliasLists[listOffset];
        અચલ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        જો (myContext->listIdx < list_ગણતરી) {
            અચલ અક્ષર *myStr = GET_STRING(currList[myContext->listIdx++]);
            જો (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            પાછા myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    જો (resultLength) {
        *resultLength = 0;
    }
    પાછા nullptr;
}

સ્થિર શૂન્ય U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

સ્થિર શૂન્ય U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
સ્થિર અચલ UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_ગણતરી_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_nextStandardAliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(અચલ અક્ષર *convName,
                       અચલ અક્ષર *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    જો (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        જો (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(કદ(UEnumeration)));
            જો (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                પાછા nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, કદ(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(કદ(UAliasContext)));
            જો (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                પાછા nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    પાછા myEnum;
}

સ્થિર uint16_t
ucnv_io_ગણતરી_Aliases(અચલ અક્ષર *alias, UErrorCode *pErrorCode) {
    જો(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_અંક = findConverter(alias, nullptr, pErrorCode);
        જો (conv_અંક < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_અંક];

            જો (listOffset) {
                પાછા gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    પાછા 0;
}

સ્થિર uint16_t
ucnv_io_getAliases(અચલ અક્ષર *alias, uint16_t start, અચલ અક્ષર **aliases, UErrorCode *pErrorCode) {
    જો(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_અંક = findConverter(alias, nullptr, pErrorCode);
        જો (conv_અંક < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_અંક];

            જો (listOffset) {
                uint32_t list_ગણતરી = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                અચલ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                માટે (currAlias = start; currAlias < list_ગણતરી; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    પાછા 0;
}

સ્થિર અચલ અક્ષર *
ucnv_io_getAlias(અચલ અક્ષર *alias, uint16_t n, UErrorCode *pErrorCode) {
    જો(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_અંક = findConverter(alias, nullptr, pErrorCode);
        જો (conv_અંક < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_અંક];

            જો (listOffset) {
                uint32_t list_ગણતરી = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                અચલ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                જો (n < list_ગણતરી)  {
                    પાછા GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    પાછા nullptr;
}

સ્થિર uint16_t
ucnv_io_ગણતરી_Standards(UErrorCode *pErrorCode) {
    જો (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        પાછા static_cast<uint16_t>(gMainTable.tagListSize - UCNV_અંક_HIDDEN_TAGS);
    }

    પાછા 0;
}

U_CAPI અચલ અક્ષર * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    જો (haveAliasData(pErrorCode)) {
        જો (n < gMainTable.tagListSize - UCNV_અંક_HIDDEN_TAGS) {
            પાછા GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    પાછા nullptr;
}

U_CAPI અચલ અક્ષર * U_EXPORT2
ucnv_getStandardName(અચલ અક્ષર *alias, અચલ અક્ષર *standard, UErrorCode *pErrorCode) {
    જો (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        જો (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            અચલ uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            જો (currList[0]) {
                પાછા GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    પાછા nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_ગણતરી_Aliases(અચલ અક્ષર *alias, UErrorCode *pErrorCode)
{
    પાછા ucnv_io_ગણતરી_Aliases(alias, pErrorCode);
}


U_CAPI અચલ અક્ષર* U_EXPORT2
ucnv_getAlias(અચલ અક્ષર *alias, uint16_t n, UErrorCode *pErrorCode)
{
    પાછા ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI શૂન્ય U_EXPORT2
ucnv_getAliases(અચલ અક્ષર *alias, અચલ અક્ષર **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_ગણતરી_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    પાછા ucnv_io_ગણતરી_Standards(&err);
}

U_CAPI અચલ અક્ષર * U_EXPORT2
ucnv_getCanonicalName(અચલ અક્ષર *alias, અચલ અક્ષર *standard, UErrorCode *pErrorCode) {
    જો (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_અંક = find_Tagged_Converter_અંક(alias, standard, pErrorCode);

        જો (conv_અંક < gMainTable.converterListSize) {
            પાછા GET_STRING(gMainTable.converterList[conv_અંક]);
        }
    }

    પાછા nullptr;
}

U_CDECL_BEGIN


સ્થિર int32_t U_CALLCONV
ucnv_io_ગણતરી_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    પાછા gMainTable.converterListSize;
}

સ્થિર અચલ અક્ષર * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    જો (*myContext < gMainTable.converterListSize) {
        અચલ અક્ષર *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        જો (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        પાછા myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    જો (resultLength) {
        *resultLength = 0;
    }
    પાછા nullptr;
}

સ્થિર શૂન્ય U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
સ્થિર અચલ UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_ગણતરી_All_Converters,
    uenum_unextDefault,
    ucnv_io_nextAllConverters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    જો (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(કદ(UEnumeration)));
        જો (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, કદ(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(કદ(uint16_t)));
        જો (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            પાછા nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    પાછા myEnum;
}

U_CAPI uint16_t
ucnv_io_ગણતરી_Known_Converters(UErrorCode *pErrorCode) {
    જો (haveAliasData(pErrorCode)) {
        પાછા (uint16_t)gMainTable.converterListSize;
    }
    પાછા 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

પ્રકારવ્યાખ્યા અક્ષર * U_CALLCONV StripForCompareFn(અક્ષર *dst, અચલ અક્ષર *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
પ્રકારવ્યાખ્યા સંરચના TempRow {
    uint16_t strIndex, sortIndex;
} TempRow;

પ્રકારવ્યાખ્યા સંરચના TempAliasTable {
    અચલ અક્ષર *chars;
    TempRow *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} TempAliasTable;

ગણના {
    ઢગલો_ROW_CAPACITY=500
};

સ્થિર int32_t U_CALLCONV
io_compareRows(અચલ શૂન્ય *context, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    અક્ષર stripped_ડાબું[UCNV_MAX_CONVERTER_NAME_LENGTH],
         stripped_જમણું[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    અચલ અક્ષર *chars=tempTable->chars;

    પાછા static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(stripped_ડાબું, chars + 2 * static_cast<અચલ TempRow*>(left)->strIndex),
        tempTable->stripForCompare(stripped_જમણું, chars + 2 * static_cast<અચલ TempRow*>(જમણું)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(અચલ UDataSwapper *ds,
                 અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                 UErrorCode *pErrorCode) {
    અચલ UDataInfo *pInfo;
    int32_t headerSize;

    અચલ uint16_t *inTable;
    અચલ uint32_t *inSectionSizes;
    uint32_t toc[offsets_ગણતરી];
    uint32_t offsets[offsets_ગણતરી]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, ટોચ_Offset;

    TempRow rows[ઢગલો_ROW_CAPACITY];
    uint16_t resort[ઢગલો_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }

    /* check data format and format version */
    pInfo=(અચલ UDataInfo *)((અચલ અક્ષર *)inData+4);
    જો(!(
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
        પાછા 0;
    }

    /* an alias table must contain at least the table of contents array */
    જો(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }

    inSectionSizes=(અચલ uint32_t *)((અચલ અક્ષર *)inData+headerSize);
    inTable=(અચલ uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, કદ(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    જો(tocLength<minTocLength || offsets_ગણતરી<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        પાછા 0;
    }

    /* read the known part of the table of contents */
    માટે(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, કદ(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    માટે(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    ટોચ_Offset=offsets[i-1]+toc[i-1];

    જો(length>=0) {
        uint16_t *outTable;
        અચલ uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        જો((length-headerSize)<(2*(int32_t)ટોચ_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }

        outTable=(uint16_t *)((અક્ષર *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            પાછા 0;
        }

        જો(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } નહીંતર {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(અચલ અક્ષર *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            જો(count<=ઢગલો_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } નહીંતર {
                tempTable.rows=(TempRow *)uprv_malloc(count*કદ(TempRow)+count*2);
                જો(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    પાછા 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            જો(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } નહીંતર /* U_EBCDIC_FAMILY */ {
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

            માટે(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, કદ(TempRow),
                           io_compareRows, &tempTable,
                           ખોટું, pErrorCode);

            જો(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                જો(p!=q) {
                    માટે(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } નહીંતર {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    માટે(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    માટે(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            જો(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            જો(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                પાછા 0;
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

    પાછા headerSize+2*(int32_t)ટોચ_Offset;
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

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

#include "unicode/utypes.h"

#nếu !UCONFIG_NO_CONVERSION

#include "unicode/ucnv.h"
#include "unicode/udata.h"

#include "umutex.h"
#include "uarrsort.h"
#include "uassert.h"
#include "udataswp.h"
#include "udatamem.h"
#include "cstring.h"
#include "cmemory.h"
#include "ucnv_io.h"
#include "uenumimp.h"
#include "ucln_cmn.h"

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
typedef struct UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

static const char DATA_NAME[] = "cnvalias";
static const char DATA_TYPE[] = "icu";

static UDataMemory *gAliasData=nullptr;
static icu::UInitOnce gAliasDataInitOnce {};

enum {
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
    offsetsCount,    /* length of the swapper's temporary offsets[] */
    minTocLength=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

static const UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
static UConverterAlias gMainTable;

#define GET_STRING(idx) (const char *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (const char *)(gMainTable.normalizedStringTable + (idx))

static UBool U_CALLCONV
isAcceptable(void * /*context*/,
             const char * /*type*/, const char * /*name*/,
             const UDataInfo *pInfo) {
    trả_về
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

static UBool U_CALLCONV ucnv_io_cleanup()
{
    nếu (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, sizeof(gMainTable));

    trả_về true;                   /* Everything was cleaned up */
}

static void U_CALLCONV initAliasData(UErrorCode &errCode) {
    UDataMemory *data;
    const uint16_t *table;
    const uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t sizeOfData;
    int32_t sizeOfTOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(gAliasData == nullptr);
    data = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &errCode);
    nếu (U_FAILURE(errCode)) {
        trả_về;
    }

    sectionSizes = static_cast<const uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    nếu (isDataLengthKnown && dataLength <= int32_t(sizeof(sectionSizes[0]))) {
        // We don't even have a TOC!
        goto invalidFormat;
    }
    table = reinterpret_cast<const uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * sizeof(sectionSizes[0]));
    nếu (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
        // We don't have a whole TOC!
        goto invalidFormat;
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

    nếu (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    cho (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * sizeof(table[0]);
    }
    nếu (isDataLengthKnown && dataLength < sizeOfData) {
        // Truncated file!
        goto invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (sizeof(uint32_t)/sizeof(uint16_t));
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
    nếu (gMainTable.optionTableSize > 0
        && reinterpret_cast<const UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_COUNT)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<const UConverterAliasOptions*>(table + currOffset);
    }
    không_thì {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    trả_về;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


static UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    trả_về U_SUCCESS(*pErrorCode);
}

static inline UBool
isAlias(const char *alias, UErrorCode *pErrorCode) {
    nếu(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        trả_về false;
    }
    trả_về *alias != 0;
}

static uint32_t getTagNumber(const char *tagname) {
    nếu (gMainTable.tagList) {
        uint32_t tagNum;
        cho (tagNum = 0; tagNum < gMainTable.tagListSize; tagNum++) {
            nếu (!uprv_stricmp(GET_STRING(gMainTable.tagList[tagNum]), tagname)) {
                trả_về tagNum;
            }
        }
    }

    trả_về UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
enum {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
static const uint8_t asciiTypes[128] = {
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
static const uint8_t ebcdicTypes[128] = {
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

#nếu U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#elif U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#không_thì
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI char* U_CALLCONV
ucnv_io_stripASCIIForCompare(char* dst U_LIFETIME_BOUND, const char* name) {
    char *dstItr = dst;
    uint8_t type, nextType;
    char c1;
    UBool afterDigit = false;

    trong_khi ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        switch (type) {
        case UIGNORE:
            afterDigit = false;
            continue; /* ignore all but letters and digits */
        case ZERO:
            nếu (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                nếu (nextType == ZERO || nextType == NONZERO) {
                    continue; /* ignore leading zero before another digit */
                }
            }
            break;
        case NONZERO:
            afterDigit = true;
            break;
        default:
            c1 = (char)type; /* lowercased letter */
            afterDigit = false;
            break;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    trả_về dst;
}

U_CAPI char* U_CALLCONV
ucnv_io_stripEBCDICForCompare(char* dst U_LIFETIME_BOUND, const char* name) {
    char *dstItr = dst;
    uint8_t type, nextType;
    char c1;
    UBool afterDigit = false;

    trong_khi ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        switch (type) {
        case UIGNORE:
            afterDigit = false;
            continue; /* ignore all but letters and digits */
        case ZERO:
            nếu (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                nếu (nextType == ZERO || nextType == NONZERO) {
                    continue; /* ignore leading zero before another digit */
                }
            }
            break;
        case NONZERO:
            afterDigit = true;
            break;
        default:
            c1 = (char)type; /* lowercased letter */
            afterDigit = false;
            break;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    trả_về dst;
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
U_CAPI số_nguyên U_EXPORT2
ucnv_compareNames(const char *name1, const char *name2) {
    số_nguyên rc;
    uint8_t type, nextType;
    char c1, c2;
    UBool afterDigit1 = false, afterDigit2 = false;

    cho (;;) {
        trong_khi ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            switch (type) {
            case UIGNORE:
                afterDigit1 = false;
                continue; /* ignore all but letters and digits */
            case ZERO:
                nếu (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    nếu (nextType == ZERO || nextType == NONZERO) {
                        continue; /* ignore leading zero before another digit */
                    }
                }
                break;
            case NONZERO:
                afterDigit1 = true;
                break;
            default:
                c1 = (char)type; /* lowercased letter */
                afterDigit1 = false;
                break;
            }
            break; /* deliver c1 */
        }
        trong_khi ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            switch (type) {
            case UIGNORE:
                afterDigit2 = false;
                continue; /* ignore all but letters and digits */
            case ZERO:
                nếu (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    nếu (nextType == ZERO || nextType == NONZERO) {
                        continue; /* ignore leading zero before another digit */
                    }
                }
                break;
            case NONZERO:
                afterDigit2 = true;
                break;
            default:
                c2 = (char)type; /* lowercased letter */
                afterDigit2 = false;
                break;
            }
            break; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        nếu ((c1|c2)==0) {
            trả_về 0;
        }

        /* Case-insensitive comparison */
        rc = (số_nguyên)(unsigned char)c1 - (số_nguyên)(unsigned char)c2;
        nếu (rc != 0) {
            trả_về rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
static inline uint32_t
findConverter(const char *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    số_nguyên result;
    số_nguyên isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    char strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    nếu (!isUnnormalized) {
        nếu (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            trả_về UINT32_MAX;
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

    cho (;;) {
        mid = (start + limit) / 2;
        nếu (lastMid == mid) {   /* Have we moved? */
            break;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        nếu (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        không_thì {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        nếu (result < 0) {
            limit = mid;
        } không_thì nếu (result > 0) {
            start = mid;
        } không_thì {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            nếu (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            nếu (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            trả_về gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    trả_về UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
static inline UBool
isAliasInList(const char *alias, uint32_t listOffset) {
    nếu (listOffset) {
        uint32_t currAlias;
        uint32_t listCount = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        const uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        cho (currAlias = 0; currAlias < listCount; currAlias++) {
            nếu (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                trả_về true;
            }
        }
    }
    trả_về false;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
static uint32_t
findTaggedAliasListsOffset(const char *alias, const char *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t convNum;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tagNum = getTagNumber(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    convNum = findConverter(alias, nullptr, &myErr);
    nếu (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    nếu (tagNum < (gMainTable.tagListSize - UCNV_NUM_HIDDEN_TAGS) && convNum < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tagNum*gMainTable.converterListSize + convNum];
        nếu (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            trả_về listOffset;
        }
        nếu (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            cho (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                nếu (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t currTagNum = idx/gMainTable.converterListSize;
                    uint32_t currConvNum = (idx - currTagNum*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tagNum*gMainTable.converterListSize + currConvNum];
                    nếu (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        trả_về tempListOffset;
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
        trả_về 0;
    }
    /* else converter or tag not found */

    trả_về UINT32_MAX;
}

/* Return the canonical name */
static uint32_t
findTaggedConverterNum(const char *alias, const char *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t convNum;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tagNum = getTagNumber(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    convNum = findConverter(alias, nullptr, &myErr);
    nếu (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    nếu (tagNum < (gMainTable.tagListSize - UCNV_NUM_HIDDEN_TAGS) && convNum < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tagNum*gMainTable.converterListSize + convNum];
        nếu (listOffset && isAliasInList(alias, listOffset)) {
            trả_về convNum;
        }
        nếu (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tagNum)*gMainTable.converterListSize;
            uint32_t convLimit = (tagNum+1)*gMainTable.converterListSize;
            cho (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                nếu (listOffset && isAliasInList(alias, listOffset)) {
                    trả_về idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    trả_về UINT32_MAX;
}

U_CAPI const char *
ucnv_io_getConverterName(const char *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    const char *aliasTmp = alias;
    int32_t i = 0;
    cho (i = 0; i < 2; i++) {
        nếu (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            nếu (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } không_thì {
                break;
            }
        }
        nếu(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t convNum = findConverter(aliasTmp, containsOption, pErrorCode);
            nếu (convNum < gMainTable.converterListSize) {
                trả_về GET_STRING(gMainTable.converterList[convNum]);
            }
            /* else converter not found */
        } không_thì {
            break;
        }
    }

    trả_về nullptr;
}

U_CDECL_BEGIN


static int32_t U_CALLCONV
ucnv_io_countStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    nếu (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    trả_về value;
}

static const char * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    nếu (listOffset) {
        uint32_t listCount = gMainTable.taggedAliasLists[listOffset];
        const uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        nếu (myContext->listIdx < listCount) {
            const char *myStr = GET_STRING(currList[myContext->listIdx++]);
            nếu (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            trả_về myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    nếu (resultLength) {
        *resultLength = 0;
    }
    trả_về nullptr;
}

static void U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

static void U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
static const UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_countStandardAliases,
    uenum_unextDefault,
    ucnv_io_nextStandardAliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(const char *convName,
                       const char *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    nếu (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        nếu (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(sizeof(UEnumeration)));
            nếu (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                trả_về nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, sizeof(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(sizeof(UAliasContext)));
            nếu (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                trả_về nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    trả_về myEnum;
}

static uint16_t
ucnv_io_countAliases(const char *alias, UErrorCode *pErrorCode) {
    nếu(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t convNum = findConverter(alias, nullptr, pErrorCode);
        nếu (convNum < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + convNum];

            nếu (listOffset) {
                trả_về gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    trả_về 0;
}

static uint16_t
ucnv_io_getAliases(const char *alias, uint16_t start, const char **aliases, UErrorCode *pErrorCode) {
    nếu(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t convNum = findConverter(alias, nullptr, pErrorCode);
        nếu (convNum < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + convNum];

            nếu (listOffset) {
                uint32_t listCount = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                const uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                cho (currAlias = start; currAlias < listCount; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    trả_về 0;
}

static const char *
ucnv_io_getAlias(const char *alias, uint16_t n, UErrorCode *pErrorCode) {
    nếu(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t convNum = findConverter(alias, nullptr, pErrorCode);
        nếu (convNum < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + convNum];

            nếu (listOffset) {
                uint32_t listCount = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                const uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                nếu (n < listCount)  {
                    trả_về GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    trả_về nullptr;
}

static uint16_t
ucnv_io_countStandards(UErrorCode *pErrorCode) {
    nếu (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        trả_về static_cast<uint16_t>(gMainTable.tagListSize - UCNV_NUM_HIDDEN_TAGS);
    }

    trả_về 0;
}

U_CAPI const char * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    nếu (haveAliasData(pErrorCode)) {
        nếu (n < gMainTable.tagListSize - UCNV_NUM_HIDDEN_TAGS) {
            trả_về GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    trả_về nullptr;
}

U_CAPI const char * U_EXPORT2
ucnv_getStandardName(const char *alias, const char *standard, UErrorCode *pErrorCode) {
    nếu (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        nếu (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            const uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            nếu (currList[0]) {
                trả_về GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    trả_về nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_countAliases(const char *alias, UErrorCode *pErrorCode)
{
    trả_về ucnv_io_countAliases(alias, pErrorCode);
}


U_CAPI const char* U_EXPORT2
ucnv_getAlias(const char *alias, uint16_t n, UErrorCode *pErrorCode)
{
    trả_về ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI void U_EXPORT2
ucnv_getAliases(const char *alias, const char **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_countStandards()
{
    UErrorCode err = U_ZERO_ERROR;
    trả_về ucnv_io_countStandards(&err);
}

U_CAPI const char * U_EXPORT2
ucnv_getCanonicalName(const char *alias, const char *standard, UErrorCode *pErrorCode) {
    nếu (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t convNum = findTaggedConverterNum(alias, standard, pErrorCode);

        nếu (convNum < gMainTable.converterListSize) {
            trả_về GET_STRING(gMainTable.converterList[convNum]);
        }
    }

    trả_về nullptr;
}

U_CDECL_BEGIN


static int32_t U_CALLCONV
ucnv_io_countAllConverters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    trả_về gMainTable.converterListSize;
}

static const char * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    nếu (*myContext < gMainTable.converterListSize) {
        const char *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        nếu (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        trả_về myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    nếu (resultLength) {
        *resultLength = 0;
    }
    trả_về nullptr;
}

static void U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
static const UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_countAllConverters,
    uenum_unextDefault,
    ucnv_io_nextAllConverters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    nếu (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(sizeof(UEnumeration)));
        nếu (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            trả_về nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, sizeof(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(sizeof(uint16_t)));
        nếu (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            trả_về nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    trả_về myEnum;
}

U_CAPI uint16_t
ucnv_io_countKnownConverters(UErrorCode *pErrorCode) {
    nếu (haveAliasData(pErrorCode)) {
        trả_về (uint16_t)gMainTable.converterListSize;
    }
    trả_về 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

typedef char * U_CALLCONV StripForCompareFn(char *dst, const char *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
typedef struct TempRow {
    uint16_t strIndex, sortIndex;
} TempRow;

typedef struct TempAliasTable {
    const char *chars;
    TempRow *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} TempAliasTable;

enum {
    STACK_ROW_CAPACITY=500
};

static int32_t U_CALLCONV
io_compareRows(const void *context, const void *left, const void *right) {
    char strippedLeft[UCNV_MAX_CONVERTER_NAME_LENGTH],
         strippedRight[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    const char *chars=tempTable->chars;

    trả_về static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(strippedLeft, chars + 2 * static_cast<const TempRow*>(left)->strIndex),
        tempTable->stripForCompare(strippedRight, chars + 2 * static_cast<const TempRow*>(right)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(const UDataSwapper *ds,
                 const void *inData, int32_t length, void *outData,
                 UErrorCode *pErrorCode) {
    const UDataInfo *pInfo;
    int32_t headerSize;

    const uint16_t *inTable;
    const uint32_t *inSectionSizes;
    uint32_t toc[offsetsCount];
    uint32_t offsets[offsetsCount]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, topOffset;

    TempRow rows[STACK_ROW_CAPACITY];
    uint16_t resort[STACK_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    nếu(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        trả_về 0;
    }

    /* check data format and format version */
    pInfo=(const UDataInfo *)((const char *)inData+4);
    nếu(!(
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
        trả_về 0;
    }

    /* an alias table must contain at least the table of contents array */
    nếu(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        trả_về 0;
    }

    inSectionSizes=(const uint32_t *)((const char *)inData+headerSize);
    inTable=(const uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, sizeof(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    nếu(tocLength<minTocLength || offsetsCount<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        trả_về 0;
    }

    /* read the known part of the table of contents */
    cho(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, sizeof(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    cho(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    topOffset=offsets[i-1]+toc[i-1];

    nếu(length>=0) {
        uint16_t *outTable;
        const uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        nếu((length-headerSize)<(2*(int32_t)topOffset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            trả_về 0;
        }

        outTable=(uint16_t *)((char *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        nếu(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            trả_về 0;
        }

        nếu(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } không_thì {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(const char *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            nếu(count<=STACK_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } không_thì {
                tempTable.rows=(TempRow *)uprv_malloc(count*sizeof(TempRow)+count*2);
                nếu(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    trả_về 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            nếu(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } không_thì /* U_EBCDIC_FAMILY */ {
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

            cho(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, sizeof(TempRow),
                           io_compareRows, &tempTable,
                           false, pErrorCode);

            nếu(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                nếu(p!=q) {
                    cho(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } không_thì {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    cho(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    cho(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            nếu(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            nếu(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                trả_về 0;
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

    trả_về headerSize+2*(int32_t)topOffset;
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

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

#𐤀𐤌 !UCONFIG_NO_CONVERSION

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
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

ساكن ثابت حرف DATA_NAME[] = "cnvalias";
ساكن ثابت حرف DATA_TYPE[] = "icu";

ساكن UDataMemory *gAliasData=nullptr;
ساكن icu::UInitOnce gAliasDataInitOnce {};

تعداد {
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

ساكن ثابت UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
ساكن UConverterAlias gMainTable;

#define GET_STRING(idx) (ثابت حرف *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (ثابت حرف *)(gMainTable.normalizedStringTable + (idx))

ساكن UBool U_CALLCONV
isAcceptable(فراغ * /*context*/,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

ساكن UBool U_CALLCONV ucnv_io_cleanup()
{
    𐤀𐤌 (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, حجم(gMainTable));

    أرجع true;                   /* Everything was cleaned up */
}

ساكن فراغ U_CALLCONV initAliasData(UErrorCode &errCode) {
    UDataMemory *data;
    ثابت uint16_t *table;
    ثابت uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t sizeOfData;
    int32_t sizeOfTOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(gAliasData == nullptr);
    data = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &errCode);
    𐤀𐤌 (U_FAILURE(errCode)) {
        أرجع;
    }

    sectionSizes = static_cast<ثابت uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    𐤀𐤌 (isDataLengthKnown && dataLength <= int32_t(حجم(sectionSizes[0]))) {
        // We don't even have a TOC!
        اذهب invalidFormat;
    }
    table = reinterpret_cast<ثابت uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * حجم(sectionSizes[0]));
    𐤀𐤌 (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
        // We don't have a whole TOC!
        اذهب invalidFormat;
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

    𐤀𐤌 (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    لكل (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * حجم(table[0]);
    }
    𐤀𐤌 (isDataLengthKnown && dataLength < sizeOfData) {
        // Truncated file!
        اذهب invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (حجم(uint32_t)/حجم(uint16_t));
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
    𐤀𐤌 (gMainTable.optionTableSize > 0
        && reinterpret_cast<ثابت UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_COUNT)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<ثابت UConverterAliasOptions*>(table + currOffset);
    }
    وإلا {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    أرجع;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


ساكن UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    أرجع U_SUCCESS(*pErrorCode);
}

ساكن مضمن UBool
isAlias(ثابت حرف *alias, UErrorCode *pErrorCode) {
    𐤀𐤌(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع خطأ;
    }
    أرجع *alias != 0;
}

ساكن uint32_t getTagNumber(ثابت حرف *tagname) {
    𐤀𐤌 (gMainTable.tagList) {
        uint32_t tagNum;
        لكل (tagNum = 0; tagNum < gMainTable.tagListSize; tagNum++) {
            𐤀𐤌 (!uprv_stricmp(GET_STRING(gMainTable.tagList[tagNum]), tagname)) {
                أرجع tagNum;
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

#define GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

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

#define GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#𐤀𐤌 U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#وإلاإذا U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#وإلا
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI حرف* U_CALLCONV
ucnv_io_stripASCIIForCompare(حرف* dst U_LIFETIME_BOUND, ثابت حرف* name) {
    حرف *dstItr = dst;
    uint8_t type, nextType;
    حرف c1;
    UBool afterDigit = خطأ;

    طالما ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        بدل (type) {
        حالة UIGNORE:
            afterDigit = خطأ;
            استمر; /* ignore all but letters and digits */
        حالة ZERO:
            𐤀𐤌 (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                𐤀𐤌 (nextType == ZERO || nextType == NONZERO) {
                    استمر; /* ignore leading zero before another digit */
                }
            }
            اكسر;
        حالة NONZERO:
            afterDigit = true;
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
    uint8_t type, nextType;
    حرف c1;
    UBool afterDigit = خطأ;

    طالما ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        بدل (type) {
        حالة UIGNORE:
            afterDigit = خطأ;
            استمر; /* ignore all but letters and digits */
        حالة ZERO:
            𐤀𐤌 (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                𐤀𐤌 (nextType == ZERO || nextType == NONZERO) {
                    استمر; /* ignore leading zero before another digit */
                }
            }
            اكسر;
        حالة NONZERO:
            afterDigit = true;
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
ucnv_compareNames(ثابت حرف *name1, ثابت حرف *name2) {
    صحيح rc;
    uint8_t type, nextType;
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
                𐤀𐤌 (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    𐤀𐤌 (nextType == ZERO || nextType == NONZERO) {
                        استمر; /* ignore leading zero before another digit */
                    }
                }
                اكسر;
            حالة NONZERO:
                afterDigit1 = true;
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
                𐤀𐤌 (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    𐤀𐤌 (nextType == ZERO || nextType == NONZERO) {
                        استمر; /* ignore leading zero before another digit */
                    }
                }
                اكسر;
            حالة NONZERO:
                afterDigit2 = true;
                اكسر;
            افتراضي:
                c2 = (حرف)type; /* lowercased letter */
                afterDigit2 = خطأ;
                اكسر;
            }
            اكسر; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        𐤀𐤌 ((c1|c2)==0) {
            أرجع 0;
        }

        /* Case-insensitive comparison */
        rc = (صحيح)(unsigned حرف)c1 - (صحيح)(unsigned حرف)c2;
        𐤀𐤌 (rc != 0) {
            أرجع rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
ساكن مضمن uint32_t
findConverter(ثابت حرف *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    صحيح result;
    صحيح isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    حرف strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    𐤀𐤌 (!isUnnormalized) {
        𐤀𐤌 (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            أرجع UINT32_MAX;
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

    لكل (;;) {
        mid = (start + limit) / 2;
        𐤀𐤌 (lastMid == mid) {   /* Have we moved? */
            اكسر;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        𐤀𐤌 (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        وإلا {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        𐤀𐤌 (result < 0) {
            limit = mid;
        } وإلا 𐤀𐤌 (result > 0) {
            start = mid;
        } وإلا {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            𐤀𐤌 (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            𐤀𐤌 (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            أرجع gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    أرجع UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
ساكن مضمن UBool
isAliasInList(ثابت حرف *alias, uint32_t listOffset) {
    𐤀𐤌 (listOffset) {
        uint32_t currAlias;
        uint32_t listCount = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        ثابت uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        لكل (currAlias = 0; currAlias < listCount; currAlias++) {
            𐤀𐤌 (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                أرجع true;
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
findTaggedAliasListsOffset(ثابت حرف *alias, ثابت حرف *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t convNum;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tagNum = getTagNumber(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    convNum = findConverter(alias, nullptr, &myErr);
    𐤀𐤌 (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    𐤀𐤌 (tagNum < (gMainTable.tagListSize - UCNV_NUM_HIDDEN_TAGS) && convNum < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tagNum*gMainTable.converterListSize + convNum];
        𐤀𐤌 (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            أرجع listOffset;
        }
        𐤀𐤌 (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            لكل (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                𐤀𐤌 (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t currTagNum = idx/gMainTable.converterListSize;
                    uint32_t currConvNum = (idx - currTagNum*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tagNum*gMainTable.converterListSize + currConvNum];
                    𐤀𐤌 (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        أرجع tempListOffset;
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
findTaggedConverterNum(ثابت حرف *alias, ثابت حرف *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t convNum;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tagNum = getTagNumber(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    convNum = findConverter(alias, nullptr, &myErr);
    𐤀𐤌 (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    𐤀𐤌 (tagNum < (gMainTable.tagListSize - UCNV_NUM_HIDDEN_TAGS) && convNum < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tagNum*gMainTable.converterListSize + convNum];
        𐤀𐤌 (listOffset && isAliasInList(alias, listOffset)) {
            أرجع convNum;
        }
        𐤀𐤌 (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tagNum)*gMainTable.converterListSize;
            uint32_t convLimit = (tagNum+1)*gMainTable.converterListSize;
            لكل (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                𐤀𐤌 (listOffset && isAliasInList(alias, listOffset)) {
                    أرجع idx-convStart;
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
ucnv_io_getConverterName(ثابت حرف *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    ثابت حرف *aliasTmp = alias;
    int32_t i = 0;
    لكل (i = 0; i < 2; i++) {
        𐤀𐤌 (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            𐤀𐤌 (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } وإلا {
                اكسر;
            }
        }
        𐤀𐤌(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t convNum = findConverter(aliasTmp, containsOption, pErrorCode);
            𐤀𐤌 (convNum < gMainTable.converterListSize) {
                أرجع GET_STRING(gMainTable.converterList[convNum]);
            }
            /* else converter not found */
        } وإلا {
            اكسر;
        }
    }

    أرجع nullptr;
}

U_CDECL_BEGIN


ساكن int32_t U_CALLCONV
ucnv_io_countStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    𐤀𐤌 (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    أرجع value;
}

ساكن ثابت حرف * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    𐤀𐤌 (listOffset) {
        uint32_t listCount = gMainTable.taggedAliasLists[listOffset];
        ثابت uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        𐤀𐤌 (myContext->listIdx < listCount) {
            ثابت حرف *myStr = GET_STRING(currList[myContext->listIdx++]);
            𐤀𐤌 (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            أرجع myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    𐤀𐤌 (resultLength) {
        *resultLength = 0;
    }
    أرجع nullptr;
}

ساكن فراغ U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

ساكن فراغ U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
ساكن ثابت UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_countStandardAliases,
    uenum_unextDefault,
    ucnv_io_nextStandardAliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(ثابت حرف *convName,
                       ثابت حرف *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    𐤀𐤌 (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        𐤀𐤌 (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
            𐤀𐤌 (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                أرجع nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, حجم(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(حجم(UAliasContext)));
            𐤀𐤌 (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                أرجع nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    أرجع myEnum;
}

ساكن uint16_t
ucnv_io_countAliases(ثابت حرف *alias, UErrorCode *pErrorCode) {
    𐤀𐤌(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t convNum = findConverter(alias, nullptr, pErrorCode);
        𐤀𐤌 (convNum < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + convNum];

            𐤀𐤌 (listOffset) {
                أرجع gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    أرجع 0;
}

ساكن uint16_t
ucnv_io_getAliases(ثابت حرف *alias, uint16_t start, ثابت حرف **aliases, UErrorCode *pErrorCode) {
    𐤀𐤌(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t convNum = findConverter(alias, nullptr, pErrorCode);
        𐤀𐤌 (convNum < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + convNum];

            𐤀𐤌 (listOffset) {
                uint32_t listCount = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                لكل (currAlias = start; currAlias < listCount; currAlias++) {
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
ucnv_io_getAlias(ثابت حرف *alias, uint16_t n, UErrorCode *pErrorCode) {
    𐤀𐤌(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t convNum = findConverter(alias, nullptr, pErrorCode);
        𐤀𐤌 (convNum < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + convNum];

            𐤀𐤌 (listOffset) {
                uint32_t listCount = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ثابت uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                𐤀𐤌 (n < listCount)  {
                    أرجع GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    أرجع nullptr;
}

ساكن uint16_t
ucnv_io_countStandards(UErrorCode *pErrorCode) {
    𐤀𐤌 (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        أرجع static_cast<uint16_t>(gMainTable.tagListSize - UCNV_NUM_HIDDEN_TAGS);
    }

    أرجع 0;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    𐤀𐤌 (haveAliasData(pErrorCode)) {
        𐤀𐤌 (n < gMainTable.tagListSize - UCNV_NUM_HIDDEN_TAGS) {
            أرجع GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    أرجع nullptr;
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getStandardName(ثابت حرف *alias, ثابت حرف *standard, UErrorCode *pErrorCode) {
    𐤀𐤌 (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        𐤀𐤌 (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            ثابت uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            𐤀𐤌 (currList[0]) {
                أرجع GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    أرجع nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_countAliases(ثابت حرف *alias, UErrorCode *pErrorCode)
{
    أرجع ucnv_io_countAliases(alias, pErrorCode);
}


U_CAPI ثابت حرف* U_EXPORT2
ucnv_getAlias(ثابت حرف *alias, uint16_t n, UErrorCode *pErrorCode)
{
    أرجع ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI فراغ U_EXPORT2
ucnv_getAliases(ثابت حرف *alias, ثابت حرف **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_countStandards()
{
    UErrorCode err = U_ZERO_ERROR;
    أرجع ucnv_io_countStandards(&err);
}

U_CAPI ثابت حرف * U_EXPORT2
ucnv_getCanonicalName(ثابت حرف *alias, ثابت حرف *standard, UErrorCode *pErrorCode) {
    𐤀𐤌 (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t convNum = findTaggedConverterNum(alias, standard, pErrorCode);

        𐤀𐤌 (convNum < gMainTable.converterListSize) {
            أرجع GET_STRING(gMainTable.converterList[convNum]);
        }
    }

    أرجع nullptr;
}

U_CDECL_BEGIN


ساكن int32_t U_CALLCONV
ucnv_io_countAllConverters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    أرجع gMainTable.converterListSize;
}

ساكن ثابت حرف * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    𐤀𐤌 (*myContext < gMainTable.converterListSize) {
        ثابت حرف *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        𐤀𐤌 (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        أرجع myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    𐤀𐤌 (resultLength) {
        *resultLength = 0;
    }
    أرجع nullptr;
}

ساكن فراغ U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
ساكن ثابت UEnumeration gEnumAllConverters = {
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
    𐤀𐤌 (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(حجم(UEnumeration)));
        𐤀𐤌 (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            أرجع nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, حجم(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(حجم(uint16_t)));
        𐤀𐤌 (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            أرجع nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    أرجع myEnum;
}

U_CAPI uint16_t
ucnv_io_countKnownConverters(UErrorCode *pErrorCode) {
    𐤀𐤌 (haveAliasData(pErrorCode)) {
        أرجع (uint16_t)gMainTable.converterListSize;
    }
    أرجع 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

تعريفنوع حرف * U_CALLCONV StripForCompareFn(حرف *dst, ثابت حرف *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
تعريفنوع بنية TempRow {
    uint16_t strIndex, sortIndex;
} TempRow;

تعريفنوع بنية TempAliasTable {
    ثابت حرف *chars;
    TempRow *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} TempAliasTable;

تعداد {
    STACK_ROW_CAPACITY=500
};

ساكن int32_t U_CALLCONV
io_compareRows(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *right) {
    حرف strippedLeft[UCNV_MAX_CONVERTER_NAME_LENGTH],
         strippedRight[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    ثابت حرف *chars=tempTable->chars;

    أرجع static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(strippedLeft, chars + 2 * static_cast<ثابت TempRow*>(left)->strIndex),
        tempTable->stripForCompare(strippedRight, chars + 2 * static_cast<ثابت TempRow*>(right)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ثابت UDataSwapper *ds,
                 ثابت فراغ *inData, int32_t length, فراغ *outData,
                 UErrorCode *pErrorCode) {
    ثابت UDataInfo *pInfo;
    int32_t headerSize;

    ثابت uint16_t *inTable;
    ثابت uint32_t *inSectionSizes;
    uint32_t toc[offsetsCount];
    uint32_t offsets[offsetsCount]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, topOffset;

    TempRow rows[STACK_ROW_CAPACITY];
    uint16_t resort[STACK_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)inData+4);
    𐤀𐤌(!(
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
        أرجع 0;
    }

    /* an alias table must contain at least the table of contents array */
    𐤀𐤌(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }

    inSectionSizes=(ثابت uint32_t *)((ثابت حرف *)inData+headerSize);
    inTable=(ثابت uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, حجم(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    𐤀𐤌(tocLength<minTocLength || offsetsCount<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        أرجع 0;
    }

    /* read the known part of the table of contents */
    لكل(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, حجم(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    لكل(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    topOffset=offsets[i-1]+toc[i-1];

    𐤀𐤌(length>=0) {
        uint16_t *outTable;
        ثابت uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        𐤀𐤌((length-headerSize)<(2*(int32_t)topOffset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }

        outTable=(uint16_t *)((حرف *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        𐤀𐤌(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            أرجع 0;
        }

        𐤀𐤌(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } وإلا {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(ثابت حرف *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            𐤀𐤌(count<=STACK_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } وإلا {
                tempTable.rows=(TempRow *)uprv_malloc(count*حجم(TempRow)+count*2);
                𐤀𐤌(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    أرجع 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            𐤀𐤌(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } وإلا /* U_EBCDIC_FAMILY */ {
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

            لكل(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, حجم(TempRow),
                           io_compareRows, &tempTable,
                           خطأ, pErrorCode);

            𐤀𐤌(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                𐤀𐤌(p!=q) {
                    لكل(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } وإلا {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    لكل(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    لكل(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            𐤀𐤌(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            𐤀𐤌(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                أرجع 0;
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

    أرجع headerSize+2*(int32_t)topOffset;
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

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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION

#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "unicode/udata.h"

#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uarrsort.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "udataswp.h"
#அந்தர்பாவ "udatamem.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ucnv_io.h"
#அந்தர்பாவ "uenumimp.h"
#அந்தர்பாவ "ucln_cmn.h"

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
வகைவரை கட்டமைப்பு UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

நிலையான மாறிலி எழுத்து DATA_NAME[] = "cnvalias";
நிலையான மாறிலி எழுத்து DATA_TYPE[] = "icu";

நிலையான UDataMemory *gAliasData=nullptr;
நிலையான icu::UInitOnce gAliasDataInitOnce {};

எண்ணிக்கை {
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
    offsets_மொத்தம்,    /* length of the swapper's temporary offsets[] */
    minTocLength=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

நிலையான மாறிலி UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
நிலையான UConverterAlias gMainTable;

#define GET_STRING(idx) (மாறிலி எழுத்து *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (மாறிலி எழுத்து *)(gMainTable.normalizedStringTable + (idx))

நிலையான UBool U_CALLCONV
isAcceptable(வெறுமை * /*context*/,
             மாறிலி எழுத்து * /*type*/, மாறிலி எழுத்து * /*name*/,
             மாறிலி UDataInfo *pInfo) {
    திரும்பு
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

நிலையான UBool U_CALLCONV ucnv_io_cleanup()
{
    எனில் (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, அளவு(gMainTable));

    திரும்பு உண்மை;                   /* Everything was cleaned up */
}

நிலையான வெறுமை U_CALLCONV initAliasData(UErrorCode &errCode) {
    UDataMemory *data;
    மாறிலி uint16_t *table;
    மாறிலி uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t sizeOfData;
    int32_t sizeOfTOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(gAliasData == nullptr);
    data = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &errCode);
    எனில் (U_FAILURE(errCode)) {
        திரும்பு;
    }

    sectionSizes = static_cast<மாறிலி uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    எனில் (isDataLengthKnown && dataLength <= int32_t(அளவு(sectionSizes[0]))) {
        // We don't even have a TOC!
        செல் invalidFormat;
    }
    table = reinterpret_cast<மாறிலி uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * அளவு(sectionSizes[0]));
    எனில் (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
        // We don't have a whole TOC!
        செல் invalidFormat;
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

    எனில் (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    க்கு (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * அளவு(table[0]);
    }
    எனில் (isDataLengthKnown && dataLength < sizeOfData) {
        // Truncated file!
        செல் invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (அளவு(uint32_t)/அளவு(uint16_t));
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
    எனில் (gMainTable.optionTableSize > 0
        && reinterpret_cast<மாறிலி UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_மொத்தம்)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<மாறிலி UConverterAliasOptions*>(table + currOffset);
    }
    இல்லையெனில் {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    திரும்பு;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


நிலையான UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    திரும்பு U_SUCCESS(*pErrorCode);
}

நிலையான உள்வரி UBool
isAlias(மாறிலி எழுத்து *alias, UErrorCode *pErrorCode) {
    எனில்(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு பொய்;
    }
    திரும்பு *alias != 0;
}

நிலையான uint32_t get_Tag_எண்(மாறிலி எழுத்து *tagname) {
    எனில் (gMainTable.tagList) {
        uint32_t tag_இலக்கம்;
        க்கு (tag_இலக்கம் = 0; tag_இலக்கம் < gMainTable.tagListSize; tag_இலக்கம்++) {
            எனில் (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_இலக்கம்]), tagname)) {
                திரும்பு tag_இலக்கம்;
            }
        }
    }

    திரும்பு UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
எண்ணிக்கை {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
நிலையான மாறிலி uint8_t asciiTypes[128] = {
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
நிலையான மாறிலி uint8_t ebcdicTypes[128] = {
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

#எனில் U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#அல்லதெனில் U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#இல்லையெனில்
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI எழுத்து* U_CALLCONV
ucnv_io_stripASCIIForCompare(எழுத்து* dst U_LIFETIME_BOUND, மாறிலி எழுத்து* name) {
    எழுத்து *dstItr = dst;
    uint8_t type, nextType;
    எழுத்து c1;
    UBool afterDigit = பொய்;

    போது ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        தேர்வு (type) {
        நிலை UIGNORE:
            afterDigit = பொய்;
            தொடர்; /* ignore all but letters and digits */
        நிலை ZERO:
            எனில் (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                எனில் (nextType == ZERO || nextType == NONZERO) {
                    தொடர்; /* ignore leading zero before another digit */
                }
            }
            நிறுத்து;
        நிலை NONZERO:
            afterDigit = உண்மை;
            நிறுத்து;
        இயல்பு:
            c1 = (எழுத்து)type; /* lowercased letter */
            afterDigit = பொய்;
            நிறுத்து;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    திரும்பு dst;
}

U_CAPI எழுத்து* U_CALLCONV
ucnv_io_stripEBCDICForCompare(எழுத்து* dst U_LIFETIME_BOUND, மாறிலி எழுத்து* name) {
    எழுத்து *dstItr = dst;
    uint8_t type, nextType;
    எழுத்து c1;
    UBool afterDigit = பொய்;

    போது ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        தேர்வு (type) {
        நிலை UIGNORE:
            afterDigit = பொய்;
            தொடர்; /* ignore all but letters and digits */
        நிலை ZERO:
            எனில் (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                எனில் (nextType == ZERO || nextType == NONZERO) {
                    தொடர்; /* ignore leading zero before another digit */
                }
            }
            நிறுத்து;
        நிலை NONZERO:
            afterDigit = உண்மை;
            நிறுத்து;
        இயல்பு:
            c1 = (எழுத்து)type; /* lowercased letter */
            afterDigit = பொய்;
            நிறுத்து;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    திரும்பு dst;
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
U_CAPI முழு U_EXPORT2
ucnv_compareNames(மாறிலி எழுத்து *name1, மாறிலி எழுத்து *name2) {
    முழு rc;
    uint8_t type, nextType;
    எழுத்து c1, c2;
    UBool afterDigit1 = பொய், afterDigit2 = பொய்;

    க்கு (;;) {
        போது ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            தேர்வு (type) {
            நிலை UIGNORE:
                afterDigit1 = பொய்;
                தொடர்; /* ignore all but letters and digits */
            நிலை ZERO:
                எனில் (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    எனில் (nextType == ZERO || nextType == NONZERO) {
                        தொடர்; /* ignore leading zero before another digit */
                    }
                }
                நிறுத்து;
            நிலை NONZERO:
                afterDigit1 = உண்மை;
                நிறுத்து;
            இயல்பு:
                c1 = (எழுத்து)type; /* lowercased letter */
                afterDigit1 = பொய்;
                நிறுத்து;
            }
            நிறுத்து; /* deliver c1 */
        }
        போது ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            தேர்வு (type) {
            நிலை UIGNORE:
                afterDigit2 = பொய்;
                தொடர்; /* ignore all but letters and digits */
            நிலை ZERO:
                எனில் (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    எனில் (nextType == ZERO || nextType == NONZERO) {
                        தொடர்; /* ignore leading zero before another digit */
                    }
                }
                நிறுத்து;
            நிலை NONZERO:
                afterDigit2 = உண்மை;
                நிறுத்து;
            இயல்பு:
                c2 = (எழுத்து)type; /* lowercased letter */
                afterDigit2 = பொய்;
                நிறுத்து;
            }
            நிறுத்து; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        எனில் ((c1|c2)==0) {
            திரும்பு 0;
        }

        /* Case-insensitive comparison */
        rc = (முழு)(unsigned எழுத்து)c1 - (முழு)(unsigned எழுத்து)c2;
        எனில் (rc != 0) {
            திரும்பு rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
நிலையான உள்வரி uint32_t
findConverter(மாறிலி எழுத்து *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    முழு result;
    முழு isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    எழுத்து strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    எனில் (!isUnnormalized) {
        எனில் (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            திரும்பு UINT32_MAX;
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

    க்கு (;;) {
        mid = (start + limit) / 2;
        எனில் (lastMid == mid) {   /* Have we moved? */
            நிறுத்து;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        எனில் (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        இல்லையெனில் {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        எனில் (result < 0) {
            limit = mid;
        } இல்லையெனில் எனில் (result > 0) {
            start = mid;
        } இல்லையெனில் {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            எனில் (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            எனில் (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            திரும்பு gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    திரும்பு UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
நிலையான உள்வரி UBool
isAliasInList(மாறிலி எழுத்து *alias, uint32_t listOffset) {
    எனில் (listOffset) {
        uint32_t currAlias;
        uint32_t list_மொத்தம் = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        மாறிலி uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        க்கு (currAlias = 0; currAlias < list_மொத்தம்; currAlias++) {
            எனில் (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                திரும்பு உண்மை;
            }
        }
    }
    திரும்பு பொய்;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
நிலையான uint32_t
findTaggedAliasListsOffset(மாறிலி எழுத்து *alias, மாறிலி எழுத்து *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_இலக்கம்;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_இலக்கம் = get_Tag_எண்(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_இலக்கம் = findConverter(alias, nullptr, &myErr);
    எனில் (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    எனில் (tag_இலக்கம் < (gMainTable.tagListSize - UCNV_இலக்கம்_HIDDEN_TAGS) && conv_இலக்கம் < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_இலக்கம்*gMainTable.converterListSize + conv_இலக்கம்];
        எனில் (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            திரும்பு listOffset;
        }
        எனில் (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            க்கு (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                எனில் (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_இலக்கம் = idx/gMainTable.converterListSize;
                    uint32_t curr_Conv_இலக்கம் = (idx - curr_Tag_இலக்கம்*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tag_இலக்கம்*gMainTable.converterListSize + curr_Conv_இலக்கம்];
                    எனில் (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        திரும்பு tempListOffset;
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
        திரும்பு 0;
    }
    /* else converter or tag not found */

    திரும்பு UINT32_MAX;
}

/* Return the canonical name */
நிலையான uint32_t
find_Tagged_Converter_இலக்கம்(மாறிலி எழுத்து *alias, மாறிலி எழுத்து *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_இலக்கம்;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_இலக்கம் = get_Tag_எண்(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_இலக்கம் = findConverter(alias, nullptr, &myErr);
    எனில் (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    எனில் (tag_இலக்கம் < (gMainTable.tagListSize - UCNV_இலக்கம்_HIDDEN_TAGS) && conv_இலக்கம் < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_இலக்கம்*gMainTable.converterListSize + conv_இலக்கம்];
        எனில் (listOffset && isAliasInList(alias, listOffset)) {
            திரும்பு conv_இலக்கம்;
        }
        எனில் (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_இலக்கம்)*gMainTable.converterListSize;
            uint32_t convLimit = (tag_இலக்கம்+1)*gMainTable.converterListSize;
            க்கு (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                எனில் (listOffset && isAliasInList(alias, listOffset)) {
                    திரும்பு idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    திரும்பு UINT32_MAX;
}

U_CAPI மாறிலி எழுத்து *
ucnv_io_getConverterName(மாறிலி எழுத்து *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    மாறிலி எழுத்து *aliasTmp = alias;
    int32_t i = 0;
    க்கு (i = 0; i < 2; i++) {
        எனில் (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            எனில் (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } இல்லையெனில் {
                நிறுத்து;
            }
        }
        எனில்(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_இலக்கம் = findConverter(aliasTmp, containsOption, pErrorCode);
            எனில் (conv_இலக்கம் < gMainTable.converterListSize) {
                திரும்பு GET_STRING(gMainTable.converterList[conv_இலக்கம்]);
            }
            /* else converter not found */
        } இல்லையெனில் {
            நிறுத்து;
        }
    }

    திரும்பு nullptr;
}

U_CDECL_BEGIN


நிலையான int32_t U_CALLCONV
ucnv_io_மொத்தம்_Standard_Aliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    எனில் (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    திரும்பு value;
}

நிலையான மாறிலி எழுத்து * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    எனில் (listOffset) {
        uint32_t list_மொத்தம் = gMainTable.taggedAliasLists[listOffset];
        மாறிலி uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        எனில் (myContext->listIdx < list_மொத்தம்) {
            மாறிலி எழுத்து *myStr = GET_STRING(currList[myContext->listIdx++]);
            எனில் (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            திரும்பு myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    எனில் (resultLength) {
        *resultLength = 0;
    }
    திரும்பு nullptr;
}

நிலையான வெறுமை U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

நிலையான வெறுமை U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
நிலையான மாறிலி UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_மொத்தம்_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_nextStandardAliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(மாறிலி எழுத்து *convName,
                       மாறிலி எழுத்து *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    எனில் (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        எனில் (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(அளவு(UEnumeration)));
            எனில் (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, அளவு(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(அளவு(UAliasContext)));
            எனில் (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                திரும்பு nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    திரும்பு myEnum;
}

நிலையான uint16_t
ucnv_io_மொத்தம்_Aliases(மாறிலி எழுத்து *alias, UErrorCode *pErrorCode) {
    எனில்(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_இலக்கம் = findConverter(alias, nullptr, pErrorCode);
        எனில் (conv_இலக்கம் < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_இலக்கம்];

            எனில் (listOffset) {
                திரும்பு gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    திரும்பு 0;
}

நிலையான uint16_t
ucnv_io_getAliases(மாறிலி எழுத்து *alias, uint16_t start, மாறிலி எழுத்து **aliases, UErrorCode *pErrorCode) {
    எனில்(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_இலக்கம் = findConverter(alias, nullptr, pErrorCode);
        எனில் (conv_இலக்கம் < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_இலக்கம்];

            எனில் (listOffset) {
                uint32_t list_மொத்தம் = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                மாறிலி uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                க்கு (currAlias = start; currAlias < list_மொத்தம்; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    திரும்பு 0;
}

நிலையான மாறிலி எழுத்து *
ucnv_io_getAlias(மாறிலி எழுத்து *alias, uint16_t n, UErrorCode *pErrorCode) {
    எனில்(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_இலக்கம் = findConverter(alias, nullptr, pErrorCode);
        எனில் (conv_இலக்கம் < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_இலக்கம்];

            எனில் (listOffset) {
                uint32_t list_மொத்தம் = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                மாறிலி uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                எனில் (n < list_மொத்தம்)  {
                    திரும்பு GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    திரும்பு nullptr;
}

நிலையான uint16_t
ucnv_io_மொத்தம்_Standards(UErrorCode *pErrorCode) {
    எனில் (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        திரும்பு static_cast<uint16_t>(gMainTable.tagListSize - UCNV_இலக்கம்_HIDDEN_TAGS);
    }

    திரும்பு 0;
}

U_CAPI மாறிலி எழுத்து * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    எனில் (haveAliasData(pErrorCode)) {
        எனில் (n < gMainTable.tagListSize - UCNV_இலக்கம்_HIDDEN_TAGS) {
            திரும்பு GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    திரும்பு nullptr;
}

U_CAPI மாறிலி எழுத்து * U_EXPORT2
ucnv_getStandardName(மாறிலி எழுத்து *alias, மாறிலி எழுத்து *standard, UErrorCode *pErrorCode) {
    எனில் (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        எனில் (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            மாறிலி uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            எனில் (currList[0]) {
                திரும்பு GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    திரும்பு nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_மொத்தம்_Aliases(மாறிலி எழுத்து *alias, UErrorCode *pErrorCode)
{
    திரும்பு ucnv_io_மொத்தம்_Aliases(alias, pErrorCode);
}


U_CAPI மாறிலி எழுத்து* U_EXPORT2
ucnv_getAlias(மாறிலி எழுத்து *alias, uint16_t n, UErrorCode *pErrorCode)
{
    திரும்பு ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI வெறுமை U_EXPORT2
ucnv_getAliases(மாறிலி எழுத்து *alias, மாறிலி எழுத்து **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_மொத்தம்_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    திரும்பு ucnv_io_மொத்தம்_Standards(&err);
}

U_CAPI மாறிலி எழுத்து * U_EXPORT2
ucnv_getCanonicalName(மாறிலி எழுத்து *alias, மாறிலி எழுத்து *standard, UErrorCode *pErrorCode) {
    எனில் (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_இலக்கம் = find_Tagged_Converter_இலக்கம்(alias, standard, pErrorCode);

        எனில் (conv_இலக்கம் < gMainTable.converterListSize) {
            திரும்பு GET_STRING(gMainTable.converterList[conv_இலக்கம்]);
        }
    }

    திரும்பு nullptr;
}

U_CDECL_BEGIN


நிலையான int32_t U_CALLCONV
ucnv_io_மொத்தம்_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    திரும்பு gMainTable.converterListSize;
}

நிலையான மாறிலி எழுத்து * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    எனில் (*myContext < gMainTable.converterListSize) {
        மாறிலி எழுத்து *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        எனில் (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        திரும்பு myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    எனில் (resultLength) {
        *resultLength = 0;
    }
    திரும்பு nullptr;
}

நிலையான வெறுமை U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
நிலையான மாறிலி UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_மொத்தம்_All_Converters,
    uenum_unextDefault,
    ucnv_io_nextAllConverters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    எனில் (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(அளவு(UEnumeration)));
        எனில் (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, அளவு(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(அளவு(uint16_t)));
        எனில் (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            திரும்பு nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    திரும்பு myEnum;
}

U_CAPI uint16_t
ucnv_io_மொத்தம்_Known_Converters(UErrorCode *pErrorCode) {
    எனில் (haveAliasData(pErrorCode)) {
        திரும்பு (uint16_t)gMainTable.converterListSize;
    }
    திரும்பு 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

வகைவரை எழுத்து * U_CALLCONV StripForCompareFn(எழுத்து *dst, மாறிலி எழுத்து *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
வகைவரை கட்டமைப்பு TempRow {
    uint16_t strIndex, sortIndex;
} TempRow;

வகைவரை கட்டமைப்பு TempAliasTable {
    மாறிலி எழுத்து *chars;
    TempRow *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} TempAliasTable;

எண்ணிக்கை {
    அடுக்கு_ROW_CAPACITY=500
};

நிலையான int32_t U_CALLCONV
io_compareRows(மாறிலி வெறுமை *context, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    எழுத்து stripped_இடது[UCNV_MAX_CONVERTER_NAME_LENGTH],
         stripped_வலது[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    மாறிலி எழுத்து *chars=tempTable->chars;

    திரும்பு static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(stripped_இடது, chars + 2 * static_cast<மாறிலி TempRow*>(left)->strIndex),
        tempTable->stripForCompare(stripped_வலது, chars + 2 * static_cast<மாறிலி TempRow*>(வலது)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(மாறிலி UDataSwapper *ds,
                 மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                 UErrorCode *pErrorCode) {
    மாறிலி UDataInfo *pInfo;
    int32_t headerSize;

    மாறிலி uint16_t *inTable;
    மாறிலி uint32_t *inSectionSizes;
    uint32_t toc[offsets_மொத்தம்];
    uint32_t offsets[offsets_மொத்தம்]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, உச்சி_Offset;

    TempRow rows[அடுக்கு_ROW_CAPACITY];
    uint16_t resort[அடுக்கு_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    /* check data format and format version */
    pInfo=(மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData+4);
    எனில்(!(
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
        திரும்பு 0;
    }

    /* an alias table must contain at least the table of contents array */
    எனில்(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }

    inSectionSizes=(மாறிலி uint32_t *)((மாறிலி எழுத்து *)inData+headerSize);
    inTable=(மாறிலி uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, அளவு(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    எனில்(tocLength<minTocLength || offsets_மொத்தம்<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு 0;
    }

    /* read the known part of the table of contents */
    க்கு(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, அளவு(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    க்கு(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    உச்சி_Offset=offsets[i-1]+toc[i-1];

    எனில்(length>=0) {
        uint16_t *outTable;
        மாறிலி uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        எனில்((length-headerSize)<(2*(int32_t)உச்சி_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }

        outTable=(uint16_t *)((எழுத்து *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        எனில்(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            திரும்பு 0;
        }

        எனில்(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } இல்லையெனில் {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(மாறிலி எழுத்து *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            எனில்(count<=அடுக்கு_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } இல்லையெனில் {
                tempTable.rows=(TempRow *)uprv_malloc(count*அளவு(TempRow)+count*2);
                எனில்(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    திரும்பு 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            எனில்(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } இல்லையெனில் /* U_EBCDIC_FAMILY */ {
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

            க்கு(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, அளவு(TempRow),
                           io_compareRows, &tempTable,
                           பொய், pErrorCode);

            எனில்(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                எனில்(p!=q) {
                    க்கு(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } இல்லையெனில் {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    க்கு(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    க்கு(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            எனில்(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            எனில்(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                திரும்பு 0;
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

    திரும்பு headerSize+2*(int32_t)உச்சி_Offset;
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

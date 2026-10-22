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

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/udata.h"

#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "udatamem.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ucnv_io.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "ucln_cmn.h"

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
प्रकारनिर्देश संरचना UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

स्थिर ध्रुव वर्ण DATA_NAME[] = "cnvalias";
स्थिर ध्रुव वर्ण DATA_TYPE[] = "icu";

स्थिर UDataMemory *gAliasData=nullptr;
स्थिर icu::UInitOnce gAliasDataInitOnce {};

गणना {
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
    offsets_गणनम्,    /* length of the swapper's temporary offsets[] */
    minTocLength=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

स्थिर ध्रुव UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
स्थिर UConverterAlias gMainTable;

#define GET_STRING(idx) (ध्रुव वर्ण *)(gMainTable.stringTable + (idx))
#define GET_NORMALIZED_STRING(idx) (ध्रुव वर्ण *)(gMainTable.normalizedStringTable + (idx))

स्थिर UBool U_CALLCONV
isAcceptable(शून्य * /*context*/,
             ध्रुव वर्ण * /*type*/, ध्रुव वर्ण * /*name*/,
             ध्रुव UDataInfo *pInfo) {
    पटिनिवत्त
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->dataFormat[1]==0x76 &&
        pInfo->dataFormat[2]==0x41 &&
        pInfo->dataFormat[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

स्थिर UBool U_CALLCONV ucnv_io_cleanup()
{
    सचे (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, परिमाण(gMainTable));

    पटिनिवत्त सच्च;                   /* Everything was cleaned up */
}

स्थिर शून्य U_CALLCONV initAliasData(UErrorCode &errCode) {
    UDataMemory *data;
    ध्रुव uint16_t *table;
    ध्रुव uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t sizeOfData;
    int32_t sizeOfTOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(gAliasData == nullptr);
    data = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &errCode);
    सचे (U_FAILURE(errCode)) {
        पटिनिवत्त;
    }

    sectionSizes = static_cast<ध्रुव uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    सचे (isDataLengthKnown && dataLength <= int32_t(परिमाण(sectionSizes[0]))) {
        // We don't even have a TOC!
        गच्छ invalidFormat;
    }
    table = reinterpret_cast<ध्रुव uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * परिमाण(sectionSizes[0]));
    सचे (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
        // We don't have a whole TOC!
        गच्छ invalidFormat;
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

    सचे (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    अत्थाय (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * परिमाण(table[0]);
    }
    सचे (isDataLengthKnown && dataLength < sizeOfData) {
        // Truncated file!
        गच्छ invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (परिमाण(uint32_t)/परिमाण(uint16_t));
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
    सचे (gMainTable.optionTableSize > 0
        && reinterpret_cast<ध्रुव UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_गणनम्)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<ध्रुव UConverterAliasOptions*>(table + currOffset);
    }
    अञ्ञथा {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    पटिनिवत्त;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


स्थिर UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    पटिनिवत्त U_SUCCESS(*pErrorCode);
}

स्थिर अन्तःस्थ UBool
isAlias(ध्रुव वर्ण *alias, UErrorCode *pErrorCode) {
    सचे(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त मिच्छा;
    }
    पटिनिवत्त *alias != 0;
}

स्थिर uint32_t get_Tag_सङ्ख्या(ध्रुव वर्ण *tagname) {
    सचे (gMainTable.tagList) {
        uint32_t tag_अङ्कः;
        अत्थाय (tag_अङ्कः = 0; tag_अङ्कः < gMainTable.tagListSize; tag_अङ्कः++) {
            सचे (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_अङ्कः]), tagname)) {
                पटिनिवत्त tag_अङ्कः;
            }
        }
    }

    पटिनिवत्त UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
गणना {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
स्थिर ध्रुव uint8_t asciiTypes[128] = {
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
स्थिर ध्रुव uint8_t ebcdicTypes[128] = {
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

#सचे U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#नोचेद्यदि U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#अञ्ञथा
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI वर्ण* U_CALLCONV
ucnv_io_stripASCIIForCompare(वर्ण* dst U_LIFETIME_BOUND, ध्रुव वर्ण* name) {
    वर्ण *dstItr = dst;
    uint8_t type, nextType;
    वर्ण c1;
    UBool afterDigit = मिच्छा;

    याव ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        विकल्प (type) {
        पक्ष UIGNORE:
            afterDigit = मिच्छा;
            अनुवर्त; /* ignore all but letters and digits */
        पक्ष ZERO:
            सचे (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                सचे (nextType == ZERO || nextType == NONZERO) {
                    अनुवर्त; /* ignore leading zero before another digit */
                }
            }
            भङ्ग;
        पक्ष NONZERO:
            afterDigit = सच्च;
            भङ्ग;
        स्वभाव:
            c1 = (वर्ण)type; /* lowercased letter */
            afterDigit = मिच्छा;
            भङ्ग;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    पटिनिवत्त dst;
}

U_CAPI वर्ण* U_CALLCONV
ucnv_io_stripEBCDICForCompare(वर्ण* dst U_LIFETIME_BOUND, ध्रुव वर्ण* name) {
    वर्ण *dstItr = dst;
    uint8_t type, nextType;
    वर्ण c1;
    UBool afterDigit = मिच्छा;

    याव ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        विकल्प (type) {
        पक्ष UIGNORE:
            afterDigit = मिच्छा;
            अनुवर्त; /* ignore all but letters and digits */
        पक्ष ZERO:
            सचे (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                सचे (nextType == ZERO || nextType == NONZERO) {
                    अनुवर्त; /* ignore leading zero before another digit */
                }
            }
            भङ्ग;
        पक्ष NONZERO:
            afterDigit = सच्च;
            भङ्ग;
        स्वभाव:
            c1 = (वर्ण)type; /* lowercased letter */
            afterDigit = मिच्छा;
            भङ्ग;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    पटिनिवत्त dst;
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
U_CAPI पूर्णाङ्क U_EXPORT2
ucnv_compareNames(ध्रुव वर्ण *name1, ध्रुव वर्ण *name2) {
    पूर्णाङ्क rc;
    uint8_t type, nextType;
    वर्ण c1, c2;
    UBool afterDigit1 = मिच्छा, afterDigit2 = मिच्छा;

    अत्थाय (;;) {
        याव ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            विकल्प (type) {
            पक्ष UIGNORE:
                afterDigit1 = मिच्छा;
                अनुवर्त; /* ignore all but letters and digits */
            पक्ष ZERO:
                सचे (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    सचे (nextType == ZERO || nextType == NONZERO) {
                        अनुवर्त; /* ignore leading zero before another digit */
                    }
                }
                भङ्ग;
            पक्ष NONZERO:
                afterDigit1 = सच्च;
                भङ्ग;
            स्वभाव:
                c1 = (वर्ण)type; /* lowercased letter */
                afterDigit1 = मिच्छा;
                भङ्ग;
            }
            भङ्ग; /* deliver c1 */
        }
        याव ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            विकल्प (type) {
            पक्ष UIGNORE:
                afterDigit2 = मिच्छा;
                अनुवर्त; /* ignore all but letters and digits */
            पक्ष ZERO:
                सचे (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    सचे (nextType == ZERO || nextType == NONZERO) {
                        अनुवर्त; /* ignore leading zero before another digit */
                    }
                }
                भङ्ग;
            पक्ष NONZERO:
                afterDigit2 = सच्च;
                भङ्ग;
            स्वभाव:
                c2 = (वर्ण)type; /* lowercased letter */
                afterDigit2 = मिच्छा;
                भङ्ग;
            }
            भङ्ग; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        सचे ((c1|c2)==0) {
            पटिनिवत्त 0;
        }

        /* Case-insensitive comparison */
        rc = (पूर्णाङ्क)(unsigned वर्ण)c1 - (पूर्णाङ्क)(unsigned वर्ण)c2;
        सचे (rc != 0) {
            पटिनिवत्त rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
स्थिर अन्तःस्थ uint32_t
findConverter(ध्रुव वर्ण *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    पूर्णाङ्क result;
    पूर्णाङ्क isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    वर्ण strippedName[UCNV_MAX_CONVERTER_NAME_LENGTH];

    सचे (!isUnnormalized) {
        सचे (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            पटिनिवत्त UINT32_MAX;
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

    अत्थाय (;;) {
        mid = (start + limit) / 2;
        सचे (lastMid == mid) {   /* Have we moved? */
            भङ्ग;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        सचे (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        अञ्ञथा {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        सचे (result < 0) {
            limit = mid;
        } अञ्ञथा सचे (result > 0) {
            start = mid;
        } अञ्ञथा {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            सचे (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            सचे (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            पटिनिवत्त gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    पटिनिवत्त UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
स्थिर अन्तःस्थ UBool
isAliasInList(ध्रुव वर्ण *alias, uint32_t listOffset) {
    सचे (listOffset) {
        uint32_t currAlias;
        uint32_t list_गणनम् = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        अत्थाय (currAlias = 0; currAlias < list_गणनम्; currAlias++) {
            सचे (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                पटिनिवत्त सच्च;
            }
        }
    }
    पटिनिवत्त मिच्छा;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
स्थिर uint32_t
findTaggedAliasListsOffset(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_अङ्कः;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_अङ्कः = get_Tag_सङ्ख्या(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_अङ्कः = findConverter(alias, nullptr, &myErr);
    सचे (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    सचे (tag_अङ्कः < (gMainTable.tagListSize - UCNV_अङ्कः_HIDDEN_TAGS) && conv_अङ्कः < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_अङ्कः*gMainTable.converterListSize + conv_अङ्कः];
        सचे (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            पटिनिवत्त listOffset;
        }
        सचे (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            अत्थाय (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                सचे (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_अङ्कः = idx/gMainTable.converterListSize;
                    uint32_t curr_Conv_अङ्कः = (idx - curr_Tag_अङ्कः*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tag_अङ्कः*gMainTable.converterListSize + curr_Conv_अङ्कः];
                    सचे (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        पटिनिवत्त tempListOffset;
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
        पटिनिवत्त 0;
    }
    /* else converter or tag not found */

    पटिनिवत्त UINT32_MAX;
}

/* Return the canonical name */
स्थिर uint32_t
find_Tagged_Converter_अङ्कः(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_अङ्कः;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_अङ्कः = get_Tag_सङ्ख्या(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_अङ्कः = findConverter(alias, nullptr, &myErr);
    सचे (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    सचे (tag_अङ्कः < (gMainTable.tagListSize - UCNV_अङ्कः_HIDDEN_TAGS) && conv_अङ्कः < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_अङ्कः*gMainTable.converterListSize + conv_अङ्कः];
        सचे (listOffset && isAliasInList(alias, listOffset)) {
            पटिनिवत्त conv_अङ्कः;
        }
        सचे (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_अङ्कः)*gMainTable.converterListSize;
            uint32_t convLimit = (tag_अङ्कः+1)*gMainTable.converterListSize;
            अत्थाय (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                सचे (listOffset && isAliasInList(alias, listOffset)) {
                    पटिनिवत्त idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    पटिनिवत्त UINT32_MAX;
}

U_CAPI ध्रुव वर्ण *
ucnv_io_getConverterName(ध्रुव वर्ण *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    ध्रुव वर्ण *aliasTmp = alias;
    int32_t i = 0;
    अत्थाय (i = 0; i < 2; i++) {
        सचे (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            सचे (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } अञ्ञथा {
                भङ्ग;
            }
        }
        सचे(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_अङ्कः = findConverter(aliasTmp, containsOption, pErrorCode);
            सचे (conv_अङ्कः < gMainTable.converterListSize) {
                पटिनिवत्त GET_STRING(gMainTable.converterList[conv_अङ्कः]);
            }
            /* else converter not found */
        } अञ्ञथा {
            भङ्ग;
        }
    }

    पटिनिवत्त nullptr;
}

U_CDECL_BEGIN


स्थिर int32_t U_CALLCONV
ucnv_io_गणनम्_Standard_Aliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    सचे (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    पटिनिवत्त value;
}

स्थिर ध्रुव वर्ण * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    सचे (listOffset) {
        uint32_t list_गणनम् = gMainTable.taggedAliasLists[listOffset];
        ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        सचे (myContext->listIdx < list_गणनम्) {
            ध्रुव वर्ण *myStr = GET_STRING(currList[myContext->listIdx++]);
            सचे (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            पटिनिवत्त myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    सचे (resultLength) {
        *resultLength = 0;
    }
    पटिनिवत्त nullptr;
}

स्थिर शून्य U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

स्थिर शून्य U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
स्थिर ध्रुव UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_गणनम्_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_nextStandardAliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(ध्रुव वर्ण *convName,
                       ध्रुव वर्ण *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    सचे (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        सचे (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(परिमाण(UEnumeration)));
            सचे (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, परिमाण(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(परिमाण(UAliasContext)));
            सचे (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                पटिनिवत्त nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    पटिनिवत्त myEnum;
}

स्थिर uint16_t
ucnv_io_गणनम्_Aliases(ध्रुव वर्ण *alias, UErrorCode *pErrorCode) {
    सचे(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_अङ्कः = findConverter(alias, nullptr, pErrorCode);
        सचे (conv_अङ्कः < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_अङ्कः];

            सचे (listOffset) {
                पटिनिवत्त gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    पटिनिवत्त 0;
}

स्थिर uint16_t
ucnv_io_getAliases(ध्रुव वर्ण *alias, uint16_t start, ध्रुव वर्ण **aliases, UErrorCode *pErrorCode) {
    सचे(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_अङ्कः = findConverter(alias, nullptr, pErrorCode);
        सचे (conv_अङ्कः < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_अङ्कः];

            सचे (listOffset) {
                uint32_t list_गणनम् = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                अत्थाय (currAlias = start; currAlias < list_गणनम्; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    पटिनिवत्त 0;
}

स्थिर ध्रुव वर्ण *
ucnv_io_getAlias(ध्रुव वर्ण *alias, uint16_t n, UErrorCode *pErrorCode) {
    सचे(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_अङ्कः = findConverter(alias, nullptr, pErrorCode);
        सचे (conv_अङ्कः < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_अङ्कः];

            सचे (listOffset) {
                uint32_t list_गणनम् = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                सचे (n < list_गणनम्)  {
                    पटिनिवत्त GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    पटिनिवत्त nullptr;
}

स्थिर uint16_t
ucnv_io_गणनम्_Standards(UErrorCode *pErrorCode) {
    सचे (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        पटिनिवत्त static_cast<uint16_t>(gMainTable.tagListSize - UCNV_अङ्कः_HIDDEN_TAGS);
    }

    पटिनिवत्त 0;
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    सचे (haveAliasData(pErrorCode)) {
        सचे (n < gMainTable.tagListSize - UCNV_अङ्कः_HIDDEN_TAGS) {
            पटिनिवत्त GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    पटिनिवत्त nullptr;
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
ucnv_getStandardName(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UErrorCode *pErrorCode) {
    सचे (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        सचे (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            सचे (currList[0]) {
                पटिनिवत्त GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    पटिनिवत्त nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_गणनम्_Aliases(ध्रुव वर्ण *alias, UErrorCode *pErrorCode)
{
    पटिनिवत्त ucnv_io_गणनम्_Aliases(alias, pErrorCode);
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
ucnv_getAlias(ध्रुव वर्ण *alias, uint16_t n, UErrorCode *pErrorCode)
{
    पटिनिवत्त ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI शून्य U_EXPORT2
ucnv_getAliases(ध्रुव वर्ण *alias, ध्रुव वर्ण **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_गणनम्_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    पटिनिवत्त ucnv_io_गणनम्_Standards(&err);
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
ucnv_getCanonicalName(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UErrorCode *pErrorCode) {
    सचे (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_अङ्कः = find_Tagged_Converter_अङ्कः(alias, standard, pErrorCode);

        सचे (conv_अङ्कः < gMainTable.converterListSize) {
            पटिनिवत्त GET_STRING(gMainTable.converterList[conv_अङ्कः]);
        }
    }

    पटिनिवत्त nullptr;
}

U_CDECL_BEGIN


स्थिर int32_t U_CALLCONV
ucnv_io_गणनम्_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    पटिनिवत्त gMainTable.converterListSize;
}

स्थिर ध्रुव वर्ण * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    सचे (*myContext < gMainTable.converterListSize) {
        ध्रुव वर्ण *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        सचे (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        पटिनिवत्त myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    सचे (resultLength) {
        *resultLength = 0;
    }
    पटिनिवत्त nullptr;
}

स्थिर शून्य U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
स्थिर ध्रुव UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_गणनम्_All_Converters,
    uenum_unextDefault,
    ucnv_io_nextAllConverters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    सचे (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(परिमाण(UEnumeration)));
        सचे (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, परिमाण(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(परिमाण(uint16_t)));
        सचे (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            पटिनिवत्त nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    पटिनिवत्त myEnum;
}

U_CAPI uint16_t
ucnv_io_गणनम्_Known_Converters(UErrorCode *pErrorCode) {
    सचे (haveAliasData(pErrorCode)) {
        पटिनिवत्त (uint16_t)gMainTable.converterListSize;
    }
    पटिनिवत्त 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

प्रकारनिर्देश वर्ण * U_CALLCONV StripForCompareFn(वर्ण *dst, ध्रुव वर्ण *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
प्रकारनिर्देश संरचना TempRow {
    uint16_t strIndex, sortIndex;
} TempRow;

प्रकारनिर्देश संरचना TempAliasTable {
    ध्रुव वर्ण *chars;
    TempRow *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} TempAliasTable;

गणना {
    राशिः_ROW_CAPACITY=500
};

स्थिर int32_t U_CALLCONV
io_compareRows(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    वर्ण stripped_वामः[UCNV_MAX_CONVERTER_NAME_LENGTH],
         stripped_दक्षिणः[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    ध्रुव वर्ण *chars=tempTable->chars;

    पटिनिवत्त static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(stripped_वामः, chars + 2 * static_cast<ध्रुव TempRow*>(left)->strIndex),
        tempTable->stripForCompare(stripped_दक्षिणः, chars + 2 * static_cast<ध्रुव TempRow*>(दक्षिणः)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *inData, int32_t length, शून्य *outData,
                 UErrorCode *pErrorCode) {
    ध्रुव UDataInfo *pInfo;
    int32_t headerSize;

    ध्रुव uint16_t *inTable;
    ध्रुव uint32_t *inSectionSizes;
    uint32_t toc[offsets_गणनम्];
    uint32_t offsets[offsets_गणनम्]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, शिखरम्_Offset;

    TempRow rows[राशिः_ROW_CAPACITY];
    uint16_t resort[राशिः_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    सचे(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        पटिनिवत्त 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)inData+4);
    सचे(!(
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
        पटिनिवत्त 0;
    }

    /* an alias table must contain at least the table of contents array */
    सचे(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }

    inSectionSizes=(ध्रुव uint32_t *)((ध्रुव वर्ण *)inData+headerSize);
    inTable=(ध्रुव uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, परिमाण(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    सचे(tocLength<minTocLength || offsets_गणनम्<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        पटिनिवत्त 0;
    }

    /* read the known part of the table of contents */
    अत्थाय(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, परिमाण(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    अत्थाय(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    शिखरम्_Offset=offsets[i-1]+toc[i-1];

    सचे(length>=0) {
        uint16_t *outTable;
        ध्रुव uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        सचे((length-headerSize)<(2*(int32_t)शिखरम्_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त 0;
        }

        outTable=(uint16_t *)((वर्ण *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        सचे(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            पटिनिवत्त 0;
        }

        सचे(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } अञ्ञथा {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(ध्रुव वर्ण *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            सचे(count<=राशिः_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } अञ्ञथा {
                tempTable.rows=(TempRow *)uprv_malloc(count*परिमाण(TempRow)+count*2);
                सचे(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    पटिनिवत्त 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            सचे(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } अञ्ञथा /* U_EBCDIC_FAMILY */ {
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

            अत्थाय(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, परिमाण(TempRow),
                           io_compareRows, &tempTable,
                           मिच्छा, pErrorCode);

            सचे(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                सचे(p!=q) {
                    अत्थाय(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } अञ्ञथा {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    अत्थाय(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    अत्थाय(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            सचे(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            सचे(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                पटिनिवत्त 0;
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

    पटिनिवत्त headerSize+2*(int32_t)शिखरम्_Offset;
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

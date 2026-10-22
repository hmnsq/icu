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

#यदि !UCONFIG_NO_CONVERSION

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
    offsets_गिनती,    /* length of the swapper's temporary offsets[] */
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
    प्रत्यागम
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
    यदि (gAliasData) {
        udata_close(gAliasData);
        gAliasData = nullptr;
    }
    gAliasDataInitOnce.reset();

    uprv_memset(&gMainTable, 0, परिमाण(gMainTable));

    प्रत्यागम सत्य;                   /* Everything was cleaned up */
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
    यदि (U_FAILURE(errCode)) {
        प्रत्यागम;
    }

    sectionSizes = static_cast<ध्रुव uint32_t*>(udata_getMemory(data));
    int32_t dataLength = udata_getLength(data); // This is the length minus the UDataInfo size
    UBool isDataLengthKnown = dataLength >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    यदि (isDataLengthKnown && dataLength <= int32_t(परिमाण(sectionSizes[0]))) {
        // We don't even have a TOC!
        गच्छ invalidFormat;
    }
    table = reinterpret_cast<ध्रुव uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    sizeOfTOC = int32_t((tableStart + 1) * परिमाण(sectionSizes[0]));
    यदि (tableStart < minTocLength || (isDataLengthKnown && dataLength <= sizeOfTOC)) {
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

    यदि (tableStart > minTocLength) {
        gMainTable.normalizedStringTableSize = sectionSizes[9];
    }

    sizeOfData = sizeOfTOC;
    प्रति (uint32_t section = 1; section <= tableStart; section++) {
        sizeOfData += sectionSizes[section] * परिमाण(table[0]);
    }
    यदि (isDataLengthKnown && dataLength < sizeOfData) {
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
    यदि (gMainTable.optionTableSize > 0
        && reinterpret_cast<ध्रुव UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_गिनती)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<ध्रुव UConverterAliasOptions*>(table + currOffset);
    }
    अन्यथा {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.optionTableSize;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.stringTableSize;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    प्रत्यागम;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(data);
}


स्थिर UBool
haveAliasData(UErrorCode *pErrorCode) {
    umtx_initOnce(gAliasDataInitOnce, &initAliasData, *pErrorCode);
    प्रत्यागम U_SUCCESS(*pErrorCode);
}

स्थिर अन्तःस्थ UBool
isAlias(ध्रुव वर्ण *alias, UErrorCode *pErrorCode) {
    यदि(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम असत्य;
    }
    प्रत्यागम *alias != 0;
}

स्थिर uint32_t get_Tag_संख्या(ध्रुव वर्ण *tagname) {
    यदि (gMainTable.tagList) {
        uint32_t tag_अंक;
        प्रति (tag_अंक = 0; tag_अंक < gMainTable.tagListSize; tag_अंक++) {
            यदि (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_अंक]), tagname)) {
                प्रत्यागम tag_अंक;
            }
        }
    }

    प्रत्यागम UINT32_MAX;
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

#यदि U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#नोचेद्यदि U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#अन्यथा
#   error U_CHARSET_FAMILY is not valid
#endif


/* @see ucnv_compareNames */
U_CAPI वर्ण* U_CALLCONV
ucnv_io_stripASCIIForCompare(वर्ण* dst U_LIFETIME_BOUND, ध्रुव वर्ण* name) {
    वर्ण *dstItr = dst;
    uint8_t type, nextType;
    वर्ण c1;
    UBool afterDigit = असत्य;

    यावत् ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        विकल्प (type) {
        पक्ष UIGNORE:
            afterDigit = असत्य;
            अनुवर्त; /* ignore all but letters and digits */
        पक्ष ZERO:
            यदि (!afterDigit) {
                nextType = GET_ASCII_TYPE(*name);
                यदि (nextType == ZERO || nextType == NONZERO) {
                    अनुवर्त; /* ignore leading zero before another digit */
                }
            }
            भङ्ग;
        पक्ष NONZERO:
            afterDigit = सत्य;
            भङ्ग;
        स्वभाव:
            c1 = (वर्ण)type; /* lowercased letter */
            afterDigit = असत्य;
            भङ्ग;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    प्रत्यागम dst;
}

U_CAPI वर्ण* U_CALLCONV
ucnv_io_stripEBCDICForCompare(वर्ण* dst U_LIFETIME_BOUND, ध्रुव वर्ण* name) {
    वर्ण *dstItr = dst;
    uint8_t type, nextType;
    वर्ण c1;
    UBool afterDigit = असत्य;

    यावत् ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        विकल्प (type) {
        पक्ष UIGNORE:
            afterDigit = असत्य;
            अनुवर्त; /* ignore all but letters and digits */
        पक्ष ZERO:
            यदि (!afterDigit) {
                nextType = GET_EBCDIC_TYPE(*name);
                यदि (nextType == ZERO || nextType == NONZERO) {
                    अनुवर्त; /* ignore leading zero before another digit */
                }
            }
            भङ्ग;
        पक्ष NONZERO:
            afterDigit = सत्य;
            भङ्ग;
        स्वभाव:
            c1 = (वर्ण)type; /* lowercased letter */
            afterDigit = असत्य;
            भङ्ग;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    प्रत्यागम dst;
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
    UBool afterDigit1 = असत्य, afterDigit2 = असत्य;

    प्रति (;;) {
        यावत् ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            विकल्प (type) {
            पक्ष UIGNORE:
                afterDigit1 = असत्य;
                अनुवर्त; /* ignore all but letters and digits */
            पक्ष ZERO:
                यदि (!afterDigit1) {
                    nextType = GET_CHAR_TYPE(*name1);
                    यदि (nextType == ZERO || nextType == NONZERO) {
                        अनुवर्त; /* ignore leading zero before another digit */
                    }
                }
                भङ्ग;
            पक्ष NONZERO:
                afterDigit1 = सत्य;
                भङ्ग;
            स्वभाव:
                c1 = (वर्ण)type; /* lowercased letter */
                afterDigit1 = असत्य;
                भङ्ग;
            }
            भङ्ग; /* deliver c1 */
        }
        यावत् ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            विकल्प (type) {
            पक्ष UIGNORE:
                afterDigit2 = असत्य;
                अनुवर्त; /* ignore all but letters and digits */
            पक्ष ZERO:
                यदि (!afterDigit2) {
                    nextType = GET_CHAR_TYPE(*name2);
                    यदि (nextType == ZERO || nextType == NONZERO) {
                        अनुवर्त; /* ignore leading zero before another digit */
                    }
                }
                भङ्ग;
            पक्ष NONZERO:
                afterDigit2 = सत्य;
                भङ्ग;
            स्वभाव:
                c2 = (वर्ण)type; /* lowercased letter */
                afterDigit2 = असत्य;
                भङ्ग;
            }
            भङ्ग; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        यदि ((c1|c2)==0) {
            प्रत्यागम 0;
        }

        /* Case-insensitive comparison */
        rc = (पूर्णाङ्क)(अचिन्हित वर्ण)c1 - (पूर्णाङ्क)(अचिन्हित वर्ण)c2;
        यदि (rc != 0) {
            प्रत्यागम rc;
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

    यदि (!isUnnormalized) {
        यदि (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_LENGTH) {
            *pErrorCode = U_BUFFER_OVERFLOW_ERROR;
            प्रत्यागम UINT32_MAX;
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

    प्रति (;;) {
        mid = (start + limit) / 2;
        यदि (lastMid == mid) {   /* Have we moved? */
            भङ्ग;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        यदि (isUnnormalized) {
            result = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        अन्यथा {
            result = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        यदि (result < 0) {
            limit = mid;
        } अन्यथा यदि (result > 0) {
            start = mid;
        } अन्यथा {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            यदि (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            यदि (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            प्रत्यागम gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_INDEX_MASK;
        }
    }

    प्रत्यागम UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
स्थिर अन्तःस्थ UBool
isAliasInList(ध्रुव वर्ण *alias, uint32_t listOffset) {
    यदि (listOffset) {
        uint32_t currAlias;
        uint32_t list_गिनती = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        प्रति (currAlias = 0; currAlias < list_गिनती; currAlias++) {
            यदि (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                प्रत्यागम सत्य;
            }
        }
    }
    प्रत्यागम असत्य;
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
    uint32_t conv_अंक;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_अंक = get_Tag_संख्या(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_अंक = findConverter(alias, nullptr, &myErr);
    यदि (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    यदि (tag_अंक < (gMainTable.tagListSize - UCNV_अंक_HIDDEN_TAGS) && conv_अंक < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_अंक*gMainTable.converterListSize + conv_अंक];
        यदि (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            प्रत्यागम listOffset;
        }
        यदि (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            प्रति (idx = 0; idx < gMainTable.taggedAliasArraySize; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                यदि (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_अंक = idx/gMainTable.converterListSize;
                    uint32_t curr_Conv_अंक = (idx - curr_Tag_अंक*gMainTable.converterListSize);
                    uint32_t tempListOffset = gMainTable.taggedAliasArray[tag_अंक*gMainTable.converterListSize + curr_Conv_अंक];
                    यदि (tempListOffset && gMainTable.taggedAliasLists[tempListOffset + 1]) {
                        प्रत्यागम tempListOffset;
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
        प्रत्यागम 0;
    }
    /* else converter or tag not found */

    प्रत्यागम UINT32_MAX;
}

/* Return the canonical name */
स्थिर uint32_t
find_Tagged_Converter_अंक(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_अंक;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_अंक = get_Tag_संख्या(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_अंक = findConverter(alias, nullptr, &myErr);
    यदि (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    यदि (tag_अंक < (gMainTable.tagListSize - UCNV_अंक_HIDDEN_TAGS) && conv_अंक < gMainTable.converterListSize) {
        listOffset = gMainTable.taggedAliasArray[tag_अंक*gMainTable.converterListSize + conv_अंक];
        यदि (listOffset && isAliasInList(alias, listOffset)) {
            प्रत्यागम conv_अंक;
        }
        यदि (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_अंक)*gMainTable.converterListSize;
            uint32_t convLimit = (tag_अंक+1)*gMainTable.converterListSize;
            प्रति (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                यदि (listOffset && isAliasInList(alias, listOffset)) {
                    प्रत्यागम idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    प्रत्यागम UINT32_MAX;
}

U_CAPI ध्रुव वर्ण *
ucnv_io_getConverterName(ध्रुव वर्ण *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    ध्रुव वर्ण *aliasTmp = alias;
    int32_t i = 0;
    प्रति (i = 0; i < 2; i++) {
        यदि (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            यदि (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } अन्यथा {
                भङ्ग;
            }
        }
        यदि(haveAliasData(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_अंक = findConverter(aliasTmp, containsOption, pErrorCode);
            यदि (conv_अंक < gMainTable.converterListSize) {
                प्रत्यागम GET_STRING(gMainTable.converterList[conv_अंक]);
            }
            /* else converter not found */
        } अन्यथा {
            भङ्ग;
        }
    }

    प्रत्यागम nullptr;
}

U_CDECL_BEGIN


स्थिर int32_t U_CALLCONV
ucnv_io_गिनती_Standard_Aliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    यदि (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    प्रत्यागम value;
}

स्थिर ध्रुव वर्ण * U_CALLCONV
ucnv_io_nextStandardAliases(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    यदि (listOffset) {
        uint32_t list_गिनती = gMainTable.taggedAliasLists[listOffset];
        ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        यदि (myContext->listIdx < list_गिनती) {
            ध्रुव वर्ण *myStr = GET_STRING(currList[myContext->listIdx++]);
            यदि (resultLength) {
                *resultLength = (int32_t)uprv_strlen(myStr);
            }
            प्रत्यागम myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    यदि (resultLength) {
        *resultLength = 0;
    }
    प्रत्यागम nullptr;
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
    ucnv_io_गिनती_Standard_Aliases,
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
    यदि (haveAliasData(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        यदि (listOffset < gMainTable.taggedAliasListsSize) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(परिमाण(UEnumeration)));
            यदि (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, परिमाण(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(परिमाण(UAliasContext)));
            यदि (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                प्रत्यागम nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    प्रत्यागम myEnum;
}

स्थिर uint16_t
ucnv_io_गिनती_Aliases(ध्रुव वर्ण *alias, UErrorCode *pErrorCode) {
    यदि(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_अंक = findConverter(alias, nullptr, pErrorCode);
        यदि (conv_अंक < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_अंक];

            यदि (listOffset) {
                प्रत्यागम gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    प्रत्यागम 0;
}

स्थिर uint16_t
ucnv_io_getAliases(ध्रुव वर्ण *alias, uint16_t start, ध्रुव वर्ण **aliases, UErrorCode *pErrorCode) {
    यदि(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_अंक = findConverter(alias, nullptr, pErrorCode);
        यदि (conv_अंक < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_अंक];

            यदि (listOffset) {
                uint32_t list_गिनती = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                प्रति (currAlias = start; currAlias < list_गिनती; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    प्रत्यागम 0;
}

स्थिर ध्रुव वर्ण *
ucnv_io_getAlias(ध्रुव वर्ण *alias, uint16_t n, UErrorCode *pErrorCode) {
    यदि(haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_अंक = findConverter(alias, nullptr, pErrorCode);
        यदि (conv_अंक < gMainTable.converterListSize) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tagListSize - 1)*gMainTable.converterListSize + conv_अंक];

            यदि (listOffset) {
                uint32_t list_गिनती = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                यदि (n < list_गिनती)  {
                    प्रत्यागम GET_STRING(currList[n]);
                }
                *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    प्रत्यागम nullptr;
}

स्थिर uint16_t
ucnv_io_गिनती_Standards(UErrorCode *pErrorCode) {
    यदि (haveAliasData(pErrorCode)) {
        /* Don't include the empty list */
        प्रत्यागम static_cast<uint16_t>(gMainTable.tagListSize - UCNV_अंक_HIDDEN_TAGS);
    }

    प्रत्यागम 0;
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    यदि (haveAliasData(pErrorCode)) {
        यदि (n < gMainTable.tagListSize - UCNV_अंक_HIDDEN_TAGS) {
            प्रत्यागम GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_INDEX_OUTOFBOUNDS_ERROR;
    }

    प्रत्यागम nullptr;
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
ucnv_getStandardName(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UErrorCode *pErrorCode) {
    यदि (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        यदि (0 < listOffset && listOffset < gMainTable.taggedAliasListsSize) {
            ध्रुव uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            यदि (currList[0]) {
                प्रत्यागम GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    प्रत्यागम nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_गिनती_Aliases(ध्रुव वर्ण *alias, UErrorCode *pErrorCode)
{
    प्रत्यागम ucnv_io_गिनती_Aliases(alias, pErrorCode);
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
ucnv_getAlias(ध्रुव वर्ण *alias, uint16_t n, UErrorCode *pErrorCode)
{
    प्रत्यागम ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI शून्य U_EXPORT2
ucnv_getAliases(ध्रुव वर्ण *alias, ध्रुव वर्ण **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_गिनती_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    प्रत्यागम ucnv_io_गिनती_Standards(&err);
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
ucnv_getCanonicalName(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UErrorCode *pErrorCode) {
    यदि (haveAliasData(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_अंक = find_Tagged_Converter_अंक(alias, standard, pErrorCode);

        यदि (conv_अंक < gMainTable.converterListSize) {
            प्रत्यागम GET_STRING(gMainTable.converterList[conv_अंक]);
        }
    }

    प्रत्यागम nullptr;
}

U_CDECL_BEGIN


स्थिर int32_t U_CALLCONV
ucnv_io_गिनती_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    प्रत्यागम gMainTable.converterListSize;
}

स्थिर ध्रुव वर्ण * U_CALLCONV
ucnv_io_nextAllConverters(UEnumeration *enumerator,
                            int32_t* resultLength,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    यदि (*myContext < gMainTable.converterListSize) {
        ध्रुव वर्ण *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        यदि (resultLength) {
            *resultLength = (int32_t)uprv_strlen(myStr);
        }
        प्रत्यागम myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    यदि (resultLength) {
        *resultLength = 0;
    }
    प्रत्यागम nullptr;
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
    ucnv_io_गिनती_All_Converters,
    uenum_unextDefault,
    ucnv_io_nextAllConverters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    यदि (haveAliasData(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(परिमाण(UEnumeration)));
        यदि (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, परिमाण(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(परिमाण(uint16_t)));
        यदि (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            प्रत्यागम nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    प्रत्यागम myEnum;
}

U_CAPI uint16_t
ucnv_io_गिनती_Known_Converters(UErrorCode *pErrorCode) {
    यदि (haveAliasData(pErrorCode)) {
        प्रत्यागम (uint16_t)gMainTable.converterListSize;
    }
    प्रत्यागम 0;
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
    ढेर_ROW_CAPACITY=500
};

स्थिर int32_t U_CALLCONV
io_compareRows(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दायाँ) {
    वर्ण stripped_बायाँ[UCNV_MAX_CONVERTER_NAME_LENGTH],
         stripped_दायाँ[UCNV_MAX_CONVERTER_NAME_LENGTH];

    TempAliasTable *tempTable=(TempAliasTable *)context;
    ध्रुव वर्ण *chars=tempTable->chars;

    प्रत्यागम static_cast<int32_t>(uprv_strcmp(
        tempTable->stripForCompare(stripped_बायाँ, chars + 2 * static_cast<ध्रुव TempRow*>(left)->strIndex),
        tempTable->stripForCompare(stripped_दायाँ, chars + 2 * static_cast<ध्रुव TempRow*>(दायाँ)->strIndex)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *inData, int32_t length, शून्य *outData,
                 UErrorCode *pErrorCode) {
    ध्रुव UDataInfo *pInfo;
    int32_t headerSize;

    ध्रुव uint16_t *inTable;
    ध्रुव uint32_t *inSectionSizes;
    uint32_t toc[offsets_गिनती];
    uint32_t offsets[offsets_गिनती]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, tocLength, शीर्ष_Offset;

    TempRow rows[ढेर_ROW_CAPACITY];
    uint16_t resort[ढेर_ROW_CAPACITY];
    TempAliasTable tempTable;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)inData+4);
    यदि(!(
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
        प्रत्यागम 0;
    }

    /* an alias table must contain at least the table of contents array */
    यदि(length>=0 && (length-headerSize)<4*(1+minTocLength)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-headerSize);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    inSectionSizes=(ध्रुव uint32_t *)((ध्रुव वर्ण *)inData+headerSize);
    inTable=(ध्रुव uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, परिमाण(toc));
    toc[tocLengthIndex]=tocLength=ds->readUInt32(inSectionSizes[tocLengthIndex]);
    यदि(tocLength<minTocLength || offsets_गिनती<=tocLength) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", tocLength);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम 0;
    }

    /* read the known part of the table of contents */
    प्रति(i=converterListIndex; i<=tocLength; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, परिमाण(offsets));
    offsets[converterListIndex]=2*(1+tocLength); /* count two 16-bit units per toc entry */
    प्रति(i=tagListIndex; i<=tocLength; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    शीर्ष_Offset=offsets[i-1]+toc[i-1];

    यदि(length>=0) {
        uint16_t *outTable;
        ध्रुव uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t oldIndex;

        यदि((length-headerSize)<(2*(int32_t)शीर्ष_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        outTable=(uint16_t *)((वर्ण *)outData+headerSize);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+tocLength), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[stringTableIndex], 2*(int32_t)(toc[stringTableIndex]+toc[normalizedStringTableIndex]),
                             outTable+offsets[stringTableIndex], pErrorCode);
        यदि(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            प्रत्यागम 0;
        }

        यदि(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converterListIndex],
                            2*(int32_t)(offsets[stringTableIndex]-offsets[converterListIndex]),
                            outTable+offsets[converterListIndex],
                            pErrorCode);
        } अन्यथा {
            /* allocate the temporary table for sorting */
            count=toc[aliasListIndex];

            tempTable.chars=(ध्रुव वर्ण *)(outTable+offsets[stringTableIndex]); /* sort by outCharset */

            यदि(count<=ढेर_ROW_CAPACITY) {
                tempTable.rows=rows;
                tempTable.resort=resort;
            } अन्यथा {
                tempTable.rows=(TempRow *)uprv_malloc(count*परिमाण(TempRow)+count*2);
                यदि(tempTable.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    प्रत्यागम 0;
                }
                tempTable.resort=(uint16_t *)(tempTable.rows+count);
            }

            यदि(ds->outCharset==U_ASCII_FAMILY) {
                tempTable.stripForCompare=ucnv_io_stripASCIIForCompare;
            } अन्यथा /* U_EBCDIC_FAMILY */ {
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

            प्रति(i=0; i<count; ++i) {
                tempTable.rows[i].strIndex=ds->readUInt16(p[i]);
                tempTable.rows[i].sortIndex=(uint16_t)i;
            }

            uprv_sortArray(tempTable.rows, (int32_t)count, परिमाण(TempRow),
                           io_compareRows, &tempTable,
                           असत्य, pErrorCode);

            यदि(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                यदि(p!=q) {
                    प्रति(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+oldIndex, 2, q2+i, pErrorCode);
                    }
                } अन्यथा {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=tempTable.resort;

                    प्रति(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(माप_प्रकार)count);

                    प्रति(i=0; i<count; ++i) {
                        oldIndex=tempTable.rows[i].sortIndex;
                        ds->swapArray16(ds, p2+oldIndex, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(माप_प्रकार)count);
                }
            }

            यदि(tempTable.rows!=rows) {
                uprv_free(tempTable.rows);
            }

            यदि(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                प्रत्यागम 0;
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

    प्रत्यागम headerSize+2*(int32_t)शीर्ष_Offset;
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

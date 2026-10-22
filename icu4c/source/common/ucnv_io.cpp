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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/ucnv.h"
#समावेश "unicode/udata.h"

#समावेश "umutex.h"
#समावेश "uarrsort.h"
#समावेश "uassert.h"
#समावेश "udataswp.h"
#समावेश "udatamem.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "ucnv_io.h"
#समावेश "uenumimp.h"
#समावेश "ucln_cmn.h"

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
प्रकार काष्ठा UAliasContext {
    uint32_t listOffset;
    uint32_t listIdx;
} UAliasContext;

अटल स्थिर अक्षर तथ्य_NAME[] = "cnvalias";
अटल स्थिर अक्षर तथ्य_TYPE[] = "icu";

अटल UDataMemory *g_Alias_तथ्य=nullptr;
अटल icu::UInitOnce g_Alias_तथ्य_Init_Once {};

क्रमागत {
    toc_लंबाई_सूचकांक=0,
    converter_List_सूचकांक=1,
    tag_List_सूचकांक=2,
    alias_List_सूचकांक=3,
    untagged_Conv_Array_सूचकांक=4,
    tagged_Alias_Array_सूचकांक=5,
    tagged_Alias_Lists_सूचकांक=6,
    table_Options_सूचकांक=7,
    string_Table_सूचकांक=8,
    normalized_String_Table_सूचकांक=9,
    offsets_गिनती,    /* length of the swapper's temporary offsets[] */
    min_Toc_लंबाई=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

अटल स्थिर UConverterAliasOptions defaultTableOptions = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
अटल UConverterAlias gMainTable;

#घोषणा GET_STRING(idx) (स्थिर अक्षर *)(gMainTable.stringTable + (idx))
#घोषणा GET_NORMALIZED_STRING(idx) (स्थिर अक्षर *)(gMainTable.normalizedStringTable + (idx))

अटल UBool U_CALLCONV
isAcceptable(व्योम * /*context*/,
             स्थिर अक्षर * /*type*/, स्थिर अक्षर * /*name*/,
             स्थिर UDataInfo *pInfo) {
    वापस
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->तथ्य_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->तथ्य_Format[1]==0x76 &&
        pInfo->तथ्य_Format[2]==0x41 &&
        pInfo->तथ्य_Format[3]==0x6c &&
        pInfo->formatVersion[0]==3;
}

अटल UBool U_CALLCONV ucnv_io_cleanup()
{
    अगर (g_Alias_तथ्य) {
        udata_close(g_Alias_तथ्य);
        g_Alias_तथ्य = nullptr;
    }
    g_Alias_तथ्य_Init_Once.reset();

    uprv_memset(&gMainTable, 0, माप(gMainTable));

    वापस true;                   /* Everything was cleaned up */
}

अटल व्योम U_CALLCONV init_Alias_तथ्य(UErrorCode &errCode) {
    UDataMemory *तथ्य;
    स्थिर uint16_t *table;
    स्थिर uint32_t *sectionSizes;
    uint32_t tableStart;
    uint32_t currOffset;
    int32_t आकार_Of_तथ्य;
    int32_t आकार_Of_TOC;

    ucln_common_registerCleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_तथ्य == nullptr);
    तथ्य = udata_openChoice(nullptr, तथ्य_TYPE, तथ्य_NAME, isAcceptable, nullptr, &errCode);
    अगर (U_FAILURE(errCode)) {
        वापस;
    }

    sectionSizes = static_cast<स्थिर uint32_t*>(udata_getMemory(तथ्य));
    int32_t तथ्य_लंबाई = udata_get_लंबाई(तथ्य); // This is the length minus the UDataInfo size
    UBool is_तथ्य_लंबाई_Known = तथ्य_लंबाई >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    अगर (is_तथ्य_लंबाई_Known && तथ्य_लंबाई <= int32_t(माप(sectionSizes[0]))) {
        // We don't even have a TOC!
        जाओ invalidFormat;
    }
    table = reinterpret_cast<स्थिर uint16_t*>(sectionSizes);
    tableStart = sectionSizes[0];
    आकार_Of_TOC = int32_t((tableStart + 1) * माप(sectionSizes[0]));
    अगर (tableStart < min_Toc_लंबाई || (is_तथ्य_लंबाई_Known && तथ्य_लंबाई <= आकार_Of_TOC)) {
        // We don't have a whole TOC!
        जाओ invalidFormat;
    }
    g_Alias_तथ्य = तथ्य;

    gMainTable.converter_List_आकार      = sectionSizes[1];
    gMainTable.tag_List_आकार            = sectionSizes[2];
    gMainTable.alias_List_आकार          = sectionSizes[3];
    gMainTable.untagged_Conv_Array_आकार  = sectionSizes[4];
    gMainTable.tagged_Alias_Array_आकार   = sectionSizes[5];
    gMainTable.tagged_Alias_Lists_आकार   = sectionSizes[6];
    gMainTable.option_Table_आकार        = sectionSizes[7];
    gMainTable.string_Table_आकार        = sectionSizes[8];

    अगर (tableStart > min_Toc_लंबाई) {
        gMainTable.normalized_String_Table_आकार = sectionSizes[9];
    }

    आकार_Of_तथ्य = आकार_Of_TOC;
    क्रम (uint32_t section = 1; section <= tableStart; section++) {
        आकार_Of_तथ्य += sectionSizes[section] * माप(table[0]);
    }
    अगर (is_तथ्य_लंबाई_Known && तथ्य_लंबाई < आकार_Of_तथ्य) {
        // Truncated file!
        जाओ invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    currOffset = (tableStart + 1) * (माप(uint32_t)/माप(uint16_t));
    gMainTable.converterList = table + currOffset;

    currOffset += gMainTable.converter_List_आकार;
    gMainTable.tagList = table + currOffset;

    currOffset += gMainTable.tag_List_आकार;
    gMainTable.aliasList = table + currOffset;

    currOffset += gMainTable.alias_List_आकार;
    gMainTable.untaggedConvArray = table + currOffset;

    currOffset += gMainTable.untagged_Conv_Array_आकार;
    gMainTable.taggedAliasArray = table + currOffset;

    /* aliasLists is a 1's based array, but it has a padding character */
    currOffset += gMainTable.tagged_Alias_Array_आकार;
    gMainTable.taggedAliasLists = table + currOffset;

    currOffset += gMainTable.tagged_Alias_Lists_आकार;
    अगर (gMainTable.option_Table_आकार > 0
        && reinterpret_cast<स्थिर UConverterAliasOptions*>(table + currOffset)->stringNormalizationType < UCNV_IO_NORM_TYPE_गिनती)
    {
        /* Faster table */
        gMainTable.optionTable = reinterpret_cast<स्थिर UConverterAliasOptions*>(table + currOffset);
    }
    अन्यथा {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        gMainTable.optionTable = &defaultTableOptions;
    }

    currOffset += gMainTable.option_Table_आकार;
    gMainTable.stringTable = table + currOffset;

    currOffset += gMainTable.string_Table_आकार;
    gMainTable.normalizedStringTable = ((gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? gMainTable.stringTable : (table + currOffset));

    वापस;

invalidFormat:
    errCode = U_INVALID_FORMAT_ERROR;
    udata_close(तथ्य);
}


अटल UBool
have_Alias_तथ्य(UErrorCode *pErrorCode) {
    umtx_initOnce(g_Alias_तथ्य_Init_Once, &init_Alias_तथ्य, *pErrorCode);
    वापस U_SUCCESS(*pErrorCode);
}

अटल अंतरभूत UBool
isAlias(स्थिर अक्षर *alias, UErrorCode *pErrorCode) {
    अगर(alias==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस false;
    }
    वापस *alias != 0;
}

अटल uint32_t get_Tag_संख्या(स्थिर अक्षर *tagname) {
    अगर (gMainTable.tagList) {
        uint32_t tag_अंक;
        क्रम (tag_अंक = 0; tag_अंक < gMainTable.tag_List_आकार; tag_अंक++) {
            अगर (!uprv_stricmp(GET_STRING(gMainTable.tagList[tag_अंक]), tagname)) {
                वापस tag_अंक;
            }
        }
    }

    वापस UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
क्रमागत {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
अटल स्थिर uint8_t asciiTypes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0
};

#घोषणा GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

/* character types for EBCDIC 80..FF */
अटल स्थिर uint8_t ebcdicTypes[128] = {
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0
};

#घोषणा GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#अगर U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#या_अगर U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#अन्यथा
#   error U_CHARSET_FAMILY is not valid
#पूर्ण_अगर


/* @see ucnv_compareNames */
U_CAPI अक्षर* U_CALLCONV
ucnv_io_stripASCIIForCompare(अक्षर* dst U_LIFETIME_BOUND, स्थिर अक्षर* name) {
    अक्षर *dstItr = dst;
    uint8_t type, अग्र_Type;
    अक्षर c1;
    UBool afterDigit = false;

    जबतक ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        चयन (type) {
        हाल UIGNORE:
            afterDigit = false;
            जारी; /* ignore all but letters and digits */
        हाल ZERO:
            अगर (!afterDigit) {
                अग्र_Type = GET_ASCII_TYPE(*name);
                अगर (अग्र_Type == ZERO || अग्र_Type == NONZERO) {
                    जारी; /* ignore leading zero before another digit */
                }
            }
            अवरोध;
        हाल NONZERO:
            afterDigit = true;
            अवरोध;
        शेष:
            c1 = (अक्षर)type; /* lowercased letter */
            afterDigit = false;
            अवरोध;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    वापस dst;
}

U_CAPI अक्षर* U_CALLCONV
ucnv_io_stripEBCDICForCompare(अक्षर* dst U_LIFETIME_BOUND, स्थिर अक्षर* name) {
    अक्षर *dstItr = dst;
    uint8_t type, अग्र_Type;
    अक्षर c1;
    UBool afterDigit = false;

    जबतक ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        चयन (type) {
        हाल UIGNORE:
            afterDigit = false;
            जारी; /* ignore all but letters and digits */
        हाल ZERO:
            अगर (!afterDigit) {
                अग्र_Type = GET_EBCDIC_TYPE(*name);
                अगर (अग्र_Type == ZERO || अग्र_Type == NONZERO) {
                    जारी; /* ignore leading zero before another digit */
                }
            }
            अवरोध;
        हाल NONZERO:
            afterDigit = true;
            अवरोध;
        शेष:
            c1 = (अक्षर)type; /* lowercased letter */
            afterDigit = false;
            अवरोध;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    वापस dst;
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
U_CAPI पूर्णांक U_EXPORT2
ucnv_compareNames(स्थिर अक्षर *name1, स्थिर अक्षर *name2) {
    पूर्णांक rc;
    uint8_t type, अग्र_Type;
    अक्षर c1, c2;
    UBool afterDigit1 = false, afterDigit2 = false;

    क्रम (;;) {
        जबतक ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            चयन (type) {
            हाल UIGNORE:
                afterDigit1 = false;
                जारी; /* ignore all but letters and digits */
            हाल ZERO:
                अगर (!afterDigit1) {
                    अग्र_Type = GET_CHAR_TYPE(*name1);
                    अगर (अग्र_Type == ZERO || अग्र_Type == NONZERO) {
                        जारी; /* ignore leading zero before another digit */
                    }
                }
                अवरोध;
            हाल NONZERO:
                afterDigit1 = true;
                अवरोध;
            शेष:
                c1 = (अक्षर)type; /* lowercased letter */
                afterDigit1 = false;
                अवरोध;
            }
            अवरोध; /* deliver c1 */
        }
        जबतक ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            चयन (type) {
            हाल UIGNORE:
                afterDigit2 = false;
                जारी; /* ignore all but letters and digits */
            हाल ZERO:
                अगर (!afterDigit2) {
                    अग्र_Type = GET_CHAR_TYPE(*name2);
                    अगर (अग्र_Type == ZERO || अग्र_Type == NONZERO) {
                        जारी; /* ignore leading zero before another digit */
                    }
                }
                अवरोध;
            हाल NONZERO:
                afterDigit2 = true;
                अवरोध;
            शेष:
                c2 = (अक्षर)type; /* lowercased letter */
                afterDigit2 = false;
                अवरोध;
            }
            अवरोध; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        अगर ((c1|c2)==0) {
            वापस 0;
        }

        /* Case-insensitive comparison */
        rc = (पूर्णांक)(अचिन्हित अक्षर)c1 - (पूर्णांक)(अचिन्हित अक्षर)c2;
        अगर (rc != 0) {
            वापस rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
अटल अंतरभूत uint32_t
findConverter(स्थिर अक्षर *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    uint32_t mid, start, limit;
    uint32_t lastMid;
    पूर्णांक परिणाम;
    पूर्णांक isUnnormalized = (gMainTable.optionTable->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    अक्षर strippedName[UCNV_MAX_CONVERTER_NAME_लंबाई];

    अगर (!isUnnormalized) {
        अगर (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_लंबाई) {
            *pErrorCode = U_बफर_OVERFLOW_ERROR;
            वापस UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    start = 0;
    limit = gMainTable.untagged_Conv_Array_आकार;
    mid = limit;
    lastMid = UINT32_MAX;

    क्रम (;;) {
        mid = (start + limit) / 2;
        अगर (lastMid == mid) {   /* Have we moved? */
            अवरोध;  /* We haven't moved, and it wasn't found. */
        }
        lastMid = mid;
        अगर (isUnnormalized) {
            परिणाम = ucnv_compareNames(alias, GET_STRING(gMainTable.aliasList[mid]));
        }
        अन्यथा {
            परिणाम = uprv_strcmp(alias, GET_NORMALIZED_STRING(gMainTable.aliasList[mid]));
        }

        अगर (परिणाम < 0) {
            limit = mid;
        } अन्यथा अगर (परिणाम > 0) {
            start = mid;
        } अन्यथा {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            अगर (gMainTable.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *pErrorCode = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            अगर (containsOption) {
                UBool containsCnvOptionInfo = static_cast<UBool>(gMainTable.optionTable->containsCnvOptionInfo);
                *containsOption = static_cast<UBool>((containsCnvOptionInfo
                    && ((gMainTable.untaggedConvArray[mid] & UCNV_CONTAINS_OPTION_BIT) != 0))
                    || !containsCnvOptionInfo);
            }
            वापस gMainTable.untaggedConvArray[mid] & UCNV_CONVERTER_सूचकांक_MASK;
        }
    }

    वापस UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
अटल अंतरभूत UBool
isAliasInList(स्थिर अक्षर *alias, uint32_t listOffset) {
    अगर (listOffset) {
        uint32_t currAlias;
        uint32_t list_गिनती = gMainTable.taggedAliasLists[listOffset];
        /* +1 to skip listCount */
        स्थिर uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;
        क्रम (currAlias = 0; currAlias < list_गिनती; currAlias++) {
            अगर (currList[currAlias]
                && ucnv_compareNames(alias, GET_STRING(currList[currAlias]))==0)
            {
                वापस true;
            }
        }
    }
    वापस false;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
अटल uint32_t
findTaggedAliasListsOffset(स्थिर अक्षर *alias, स्थिर अक्षर *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_अंक;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_अंक = get_Tag_संख्या(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_अंक = findConverter(alias, nullptr, &myErr);
    अगर (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    अगर (tag_अंक < (gMainTable.tag_List_आकार - UCNV_अंक_HIDDEN_TAGS) && conv_अंक < gMainTable.converter_List_आकार) {
        listOffset = gMainTable.taggedAliasArray[tag_अंक*gMainTable.converter_List_आकार + conv_अंक];
        अगर (listOffset && gMainTable.taggedAliasLists[listOffset + 1]) {
            वापस listOffset;
        }
        अगर (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            क्रम (idx = 0; idx < gMainTable.tagged_Alias_Array_आकार; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                अगर (listOffset && isAliasInList(alias, listOffset)) {
                    uint32_t curr_Tag_अंक = idx/gMainTable.converter_List_आकार;
                    uint32_t curr_Conv_अंक = (idx - curr_Tag_अंक*gMainTable.converter_List_आकार);
                    uint32_t अस्थायी_List_Offset = gMainTable.taggedAliasArray[tag_अंक*gMainTable.converter_List_आकार + curr_Conv_अंक];
                    अगर (अस्थायी_List_Offset && gMainTable.taggedAliasLists[अस्थायी_List_Offset + 1]) {
                        वापस अस्थायी_List_Offset;
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
        वापस 0;
    }
    /* else converter or tag not found */

    वापस UINT32_MAX;
}

/* Return the canonical name */
अटल uint32_t
find_Tagged_Converter_अंक(स्थिर अक्षर *alias, स्थिर अक्षर *standard, UErrorCode *pErrorCode) {
    uint32_t idx;
    uint32_t listOffset;
    uint32_t conv_अंक;
    UErrorCode myErr = U_ZERO_ERROR;
    uint32_t tag_अंक = get_Tag_संख्या(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_अंक = findConverter(alias, nullptr, &myErr);
    अगर (myErr != U_ZERO_ERROR) {
        *pErrorCode = myErr;
    }

    अगर (tag_अंक < (gMainTable.tag_List_आकार - UCNV_अंक_HIDDEN_TAGS) && conv_अंक < gMainTable.converter_List_आकार) {
        listOffset = gMainTable.taggedAliasArray[tag_अंक*gMainTable.converter_List_आकार + conv_अंक];
        अगर (listOffset && isAliasInList(alias, listOffset)) {
            वापस conv_अंक;
        }
        अगर (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (tag_अंक)*gMainTable.converter_List_आकार;
            uint32_t convLimit = (tag_अंक+1)*gMainTable.converter_List_आकार;
            क्रम (idx = convStart; idx < convLimit; idx++) {
                listOffset = gMainTable.taggedAliasArray[idx];
                अगर (listOffset && isAliasInList(alias, listOffset)) {
                    वापस idx-convStart;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    वापस UINT32_MAX;
}

U_CAPI स्थिर अक्षर *
ucnv_io_getConverterName(स्थिर अक्षर *alias, UBool *containsOption, UErrorCode *pErrorCode) {
    स्थिर अक्षर *aliasTmp = alias;
    int32_t i = 0;
    क्रम (i = 0; i < 2; i++) {
        अगर (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            अगर (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } अन्यथा {
                अवरोध;
            }
        }
        अगर(have_Alias_तथ्य(pErrorCode) && isAlias(aliasTmp, pErrorCode)) {
            uint32_t conv_अंक = findConverter(aliasTmp, containsOption, pErrorCode);
            अगर (conv_अंक < gMainTable.converter_List_आकार) {
                वापस GET_STRING(gMainTable.converterList[conv_अंक]);
            }
            /* else converter not found */
        } अन्यथा {
            अवरोध;
        }
    }

    वापस nullptr;
}

U_CDECL_BEGIN


अटल int32_t U_CALLCONV
ucnv_io_गिनती_Standard_Aliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    अगर (listOffset) {
        value = gMainTable.taggedAliasLists[listOffset];
    }
    वापस value;
}

अटल स्थिर अक्षर * U_CALLCONV
ucnv_io_अग्र_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* परिणाम_लंबाई,
                            UErrorCode * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t listOffset = myContext->listOffset;

    अगर (listOffset) {
        uint32_t list_गिनती = gMainTable.taggedAliasLists[listOffset];
        स्थिर uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

        अगर (myContext->listIdx < list_गिनती) {
            स्थिर अक्षर *myStr = GET_STRING(currList[myContext->listIdx++]);
            अगर (परिणाम_लंबाई) {
                *परिणाम_लंबाई = (int32_t)uprv_strlen(myStr);
            }
            वापस myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    अगर (परिणाम_लंबाई) {
        *परिणाम_लंबाई = 0;
    }
    वापस nullptr;
}

अटल व्योम U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

अटल व्योम U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_END

/* Enumerate the aliases for the specified converter and standard tag */
अटल स्थिर UEnumeration gEnumAliases = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_गिनती_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_अग्र_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openStandardNames(स्थिर अक्षर *convName,
                       स्थिर अक्षर *standard,
                       UErrorCode *pErrorCode)
{
    UEnumeration *myEnum = nullptr;
    अगर (have_Alias_तथ्य(pErrorCode) && isAlias(convName, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(convName, standard, pErrorCode);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        अगर (listOffset < gMainTable.tagged_Alias_Lists_आकार) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(माप(UEnumeration)));
            अगर (myEnum == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                वापस nullptr;
            }
            uprv_memcpy(myEnum, &gEnumAliases, माप(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(माप(UAliasContext)));
            अगर (myContext == nullptr) {
                *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                uprv_free(myEnum);
                वापस nullptr;
            }
            myContext->listOffset = listOffset;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    वापस myEnum;
}

अटल uint16_t
ucnv_io_गिनती_Aliases(स्थिर अक्षर *alias, UErrorCode *pErrorCode) {
    अगर(have_Alias_तथ्य(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_अंक = findConverter(alias, nullptr, pErrorCode);
        अगर (conv_अंक < gMainTable.converter_List_आकार) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tag_List_आकार - 1)*gMainTable.converter_List_आकार + conv_अंक];

            अगर (listOffset) {
                वापस gMainTable.taggedAliasLists[listOffset];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    वापस 0;
}

अटल uint16_t
ucnv_io_getAliases(स्थिर अक्षर *alias, uint16_t start, स्थिर अक्षर **aliases, UErrorCode *pErrorCode) {
    अगर(have_Alias_तथ्य(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t currAlias;
        uint32_t conv_अंक = findConverter(alias, nullptr, pErrorCode);
        अगर (conv_अंक < gMainTable.converter_List_आकार) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tag_List_आकार - 1)*gMainTable.converter_List_आकार + conv_अंक];

            अगर (listOffset) {
                uint32_t list_गिनती = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                स्थिर uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                क्रम (currAlias = start; currAlias < list_गिनती; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    वापस 0;
}

अटल स्थिर अक्षर *
ucnv_io_getAlias(स्थिर अक्षर *alias, uint16_t n, UErrorCode *pErrorCode) {
    अगर(have_Alias_तथ्य(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_अंक = findConverter(alias, nullptr, pErrorCode);
        अगर (conv_अंक < gMainTable.converter_List_आकार) {
            /* tagListNum - 1 is the ALL tag */
            int32_t listOffset = gMainTable.taggedAliasArray[(gMainTable.tag_List_आकार - 1)*gMainTable.converter_List_आकार + conv_अंक];

            अगर (listOffset) {
                uint32_t list_गिनती = gMainTable.taggedAliasLists[listOffset];
                /* +1 to skip listCount */
                स्थिर uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

                अगर (n < list_गिनती)  {
                    वापस GET_STRING(currList[n]);
                }
                *pErrorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    वापस nullptr;
}

अटल uint16_t
ucnv_io_गिनती_Standards(UErrorCode *pErrorCode) {
    अगर (have_Alias_तथ्य(pErrorCode)) {
        /* Don't include the empty list */
        वापस static_cast<uint16_t>(gMainTable.tag_List_आकार - UCNV_अंक_HIDDEN_TAGS);
    }

    वापस 0;
}

U_CAPI स्थिर अक्षर * U_EXPORT2
ucnv_getStandard(uint16_t n, UErrorCode *pErrorCode) {
    अगर (have_Alias_तथ्य(pErrorCode)) {
        अगर (n < gMainTable.tag_List_आकार - UCNV_अंक_HIDDEN_TAGS) {
            वापस GET_STRING(gMainTable.tagList[n]);
        }
        *pErrorCode = U_सूचकांक_OUTOFBOUNDS_ERROR;
    }

    वापस nullptr;
}

U_CAPI स्थिर अक्षर * U_EXPORT2
ucnv_getStandardName(स्थिर अक्षर *alias, स्थिर अक्षर *standard, UErrorCode *pErrorCode) {
    अगर (have_Alias_तथ्य(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t listOffset = findTaggedAliasListsOffset(alias, standard, pErrorCode);

        अगर (0 < listOffset && listOffset < gMainTable.tagged_Alias_Lists_आकार) {
            स्थिर uint16_t *currList = gMainTable.taggedAliasLists + listOffset + 1;

            /* Get the preferred name from this list */
            अगर (currList[0]) {
                वापस GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    वापस nullptr;
}

U_CAPI uint16_t U_EXPORT2
ucnv_गिनती_Aliases(स्थिर अक्षर *alias, UErrorCode *pErrorCode)
{
    वापस ucnv_io_गिनती_Aliases(alias, pErrorCode);
}


U_CAPI स्थिर अक्षर* U_EXPORT2
ucnv_getAlias(स्थिर अक्षर *alias, uint16_t n, UErrorCode *pErrorCode)
{
    वापस ucnv_io_getAlias(alias, n, pErrorCode);
}

U_CAPI व्योम U_EXPORT2
ucnv_getAliases(स्थिर अक्षर *alias, स्थिर अक्षर **aliases, UErrorCode *pErrorCode)
{
    ucnv_io_getAliases(alias, 0, aliases, pErrorCode);
}

U_CAPI uint16_t U_EXPORT2
ucnv_गिनती_Standards()
{
    UErrorCode err = U_ZERO_ERROR;
    वापस ucnv_io_गिनती_Standards(&err);
}

U_CAPI स्थिर अक्षर * U_EXPORT2
ucnv_getCanonicalName(स्थिर अक्षर *alias, स्थिर अक्षर *standard, UErrorCode *pErrorCode) {
    अगर (have_Alias_तथ्य(pErrorCode) && isAlias(alias, pErrorCode)) {
        uint32_t conv_अंक = find_Tagged_Converter_अंक(alias, standard, pErrorCode);

        अगर (conv_अंक < gMainTable.converter_List_आकार) {
            वापस GET_STRING(gMainTable.converterList[conv_अंक]);
        }
    }

    वापस nullptr;
}

U_CDECL_BEGIN


अटल int32_t U_CALLCONV
ucnv_io_गिनती_All_Converters(UEnumeration * /*enumerator*/, UErrorCode * /*pErrorCode*/) {
    वापस gMainTable.converter_List_आकार;
}

अटल स्थिर अक्षर * U_CALLCONV
ucnv_io_अग्र_All_Converters(UEnumeration *enumerator,
                            int32_t* परिणाम_लंबाई,
                            UErrorCode * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    अगर (*myContext < gMainTable.converter_List_आकार) {
        स्थिर अक्षर *myStr = GET_STRING(gMainTable.converterList[(*myContext)++]);
        अगर (परिणाम_लंबाई) {
            *परिणाम_लंबाई = (int32_t)uprv_strlen(myStr);
        }
        वापस myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    अगर (परिणाम_लंबाई) {
        *परिणाम_लंबाई = 0;
    }
    वापस nullptr;
}

अटल व्योम U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UErrorCode * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_END
अटल स्थिर UEnumeration gEnumAllConverters = {
    nullptr,
    nullptr,
    ucnv_io_closeUEnumeration,
    ucnv_io_गिनती_All_Converters,
    uenum_unextDefault,
    ucnv_io_अग्र_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_openAllNames(UErrorCode *pErrorCode) {
    UEnumeration *myEnum = nullptr;
    अगर (have_Alias_तथ्य(pErrorCode)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(माप(UEnumeration)));
        अगर (myEnum == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस nullptr;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, माप(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(माप(uint16_t)));
        अगर (myContext == nullptr) {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            uprv_free(myEnum);
            वापस nullptr;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    वापस myEnum;
}

U_CAPI uint16_t
ucnv_io_गिनती_Known_Converters(UErrorCode *pErrorCode) {
    अगर (have_Alias_तथ्य(pErrorCode)) {
        वापस (uint16_t)gMainTable.converter_List_आकार;
    }
    वापस 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

प्रकार अक्षर * U_CALLCONV StripForCompareFn(अक्षर *dst, स्थिर अक्षर *name);
U_CDECL_END


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
प्रकार काष्ठा अस्थायी_Row {
    uint16_t str_सूचकांक, sort_सूचकांक;
} अस्थायी_Row;

प्रकार काष्ठा अस्थायी_Alias_Table {
    स्थिर अक्षर *chars;
    अस्थायी_Row *rows;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} अस्थायी_Alias_Table;

क्रमागत {
    ढेर_ROW_CAPACITY=500
};

अटल int32_t U_CALLCONV
io_compareRows(स्थिर व्योम *context, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    अक्षर stripped_बायाँ[UCNV_MAX_CONVERTER_NAME_लंबाई],
         stripped_दायाँ[UCNV_MAX_CONVERTER_NAME_लंबाई];

    अस्थायी_Alias_Table *अस्थायी_Table=(अस्थायी_Alias_Table *)context;
    स्थिर अक्षर *chars=अस्थायी_Table->chars;

    वापस static_cast<int32_t>(uprv_strcmp(
        अस्थायी_Table->stripForCompare(stripped_बायाँ, chars + 2 * static_cast<स्थिर अस्थायी_Row*>(बाँए)->str_सूचकांक),
        अस्थायी_Table->stripForCompare(stripped_दायाँ, chars + 2 * static_cast<स्थिर अस्थायी_Row*>(दाँए)->str_सूचकांक)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(स्थिर UDataSwapper *ds,
                 स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                 UErrorCode *pErrorCode) {
    स्थिर UDataInfo *pInfo;
    int32_t header_आकार;

    स्थिर uint16_t *inTable;
    स्थिर uint32_t *inSectionSizes;
    uint32_t toc[offsets_गिनती];
    uint32_t offsets[offsets_गिनती]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_लंबाई, शीर्ष_Offset;

    अस्थायी_Row rows[ढेर_ROW_CAPACITY];
    uint16_t resort[ढेर_ROW_CAPACITY];
    अस्थायी_Alias_Table अस्थायी_Table;

    /* udata_swapDataHeader checks the arguments */
    header_आकार=udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* check data format and format version */
    pInfo=(स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
    अगर(!(
        pInfo->तथ्य_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->तथ्य_Format[1]==0x76 &&
        pInfo->तथ्य_Format[2]==0x41 &&
        pInfo->तथ्य_Format[3]==0x6c &&
        pInfo->formatVersion[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1],
                         pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    /* an alias table must contain at least the table of contents array */
    अगर(length>=0 && (length-header_आकार)<4*(1+min_Toc_लंबाई)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-header_आकार);
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }

    inSectionSizes=(स्थिर uint32_t *)((स्थिर अक्षर *)in_तथ्य+header_आकार);
    inTable=(स्थिर uint16_t *)inSectionSizes;
    uprv_memset(toc, 0, माप(toc));
    toc[toc_लंबाई_सूचकांक]=toc_लंबाई=ds->readUInt32(inSectionSizes[toc_लंबाई_सूचकांक]);
    अगर(toc_लंबाई<min_Toc_लंबाई || offsets_गिनती<=toc_लंबाई) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_लंबाई);
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस 0;
    }

    /* read the known part of the table of contents */
    क्रम(i=converter_List_सूचकांक; i<=toc_लंबाई; ++i) {
        toc[i]=ds->readUInt32(inSectionSizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, माप(offsets));
    offsets[converter_List_सूचकांक]=2*(1+toc_लंबाई); /* count two 16-bit units per toc entry */
    क्रम(i=tag_List_सूचकांक; i<=toc_लंबाई; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    शीर्ष_Offset=offsets[i-1]+toc[i-1];

    अगर(length>=0) {
        uint16_t *outTable;
        स्थिर uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_सूचकांक;

        अगर((length-header_आकार)<(2*(int32_t)शीर्ष_Offset)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-header_आकार);
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }

        outTable=(uint16_t *)((अक्षर *)out_तथ्य+header_आकार);

        /* swap the entire table of contents */
        ds->swapArray32(ds, inTable, 4*(1+toc_लंबाई), outTable, pErrorCode);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, inTable+offsets[string_Table_सूचकांक], 2*(int32_t)(toc[string_Table_सूचकांक]+toc[normalized_String_Table_सूचकांक]),
                             outTable+offsets[string_Table_सूचकांक], pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            वापस 0;
        }

        अगर(ds->inCharset==ds->outCharset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            inTable+offsets[converter_List_सूचकांक],
                            2*(int32_t)(offsets[string_Table_सूचकांक]-offsets[converter_List_सूचकांक]),
                            outTable+offsets[converter_List_सूचकांक],
                            pErrorCode);
        } अन्यथा {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_सूचकांक];

            अस्थायी_Table.chars=(स्थिर अक्षर *)(outTable+offsets[string_Table_सूचकांक]); /* sort by outCharset */

            अगर(count<=ढेर_ROW_CAPACITY) {
                अस्थायी_Table.rows=rows;
                अस्थायी_Table.resort=resort;
            } अन्यथा {
                अस्थायी_Table.rows=(अस्थायी_Row *)uprv_malloc(count*माप(अस्थायी_Row)+count*2);
                अगर(अस्थायी_Table.rows==nullptr) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    वापस 0;
                }
                अस्थायी_Table.resort=(uint16_t *)(अस्थायी_Table.rows+count);
            }

            अगर(ds->outCharset==U_ASCII_FAMILY) {
                अस्थायी_Table.stripForCompare=ucnv_io_stripASCIIForCompare;
            } अन्यथा /* U_EBCDIC_FAMILY */ {
                अस्थायी_Table.stripForCompare=ucnv_io_stripEBCDICForCompare;
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
            p=inTable+offsets[alias_List_सूचकांक];
            q=outTable+offsets[alias_List_सूचकांक];

            p2=inTable+offsets[untagged_Conv_Array_सूचकांक];
            q2=outTable+offsets[untagged_Conv_Array_सूचकांक];

            क्रम(i=0; i<count; ++i) {
                अस्थायी_Table.rows[i].str_सूचकांक=ds->readUInt16(p[i]);
                अस्थायी_Table.rows[i].sort_सूचकांक=(uint16_t)i;
            }

            uprv_sortArray(अस्थायी_Table.rows, (int32_t)count, माप(अस्थायी_Row),
                           io_compareRows, &अस्थायी_Table,
                           false, pErrorCode);

            अगर(U_SUCCESS(*pErrorCode)) {
                /* copy/swap/permutate items */
                अगर(p!=q) {
                    क्रम(i=0; i<count; ++i) {
                        old_सूचकांक=अस्थायी_Table.rows[i].sort_सूचकांक;
                        ds->swapArray16(ds, p+old_सूचकांक, 2, q+i, pErrorCode);
                        ds->swapArray16(ds, p2+old_सूचकांक, 2, q2+i, pErrorCode);
                    }
                } अन्यथा {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=अस्थायी_Table.resort;

                    क्रम(i=0; i<count; ++i) {
                        old_सूचकांक=अस्थायी_Table.rows[i].sort_सूचकांक;
                        ds->swapArray16(ds, p+old_सूचकांक, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q, r, 2*(माप_प्रकार)count);

                    क्रम(i=0; i<count; ++i) {
                        old_सूचकांक=अस्थायी_Table.rows[i].sort_सूचकांक;
                        ds->swapArray16(ds, p2+old_सूचकांक, 2, r+i, pErrorCode);
                    }
                    uprv_memcpy(q2, r, 2*(माप_प्रकार)count);
                }
            }

            अगर(अस्थायी_Table.rows!=rows) {
                uprv_free(अस्थायी_Table.rows);
            }

            अगर(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                वापस 0;
            }

            /* swap remaining 16-bit values */
            ds->swapArray16(ds,
                            inTable+offsets[converter_List_सूचकांक],
                            2*(int32_t)(offsets[alias_List_सूचकांक]-offsets[converter_List_सूचकांक]),
                            outTable+offsets[converter_List_सूचकांक],
                            pErrorCode);
            ds->swapArray16(ds,
                            inTable+offsets[tagged_Alias_Array_सूचकांक],
                            2*(int32_t)(offsets[string_Table_सूचकांक]-offsets[tagged_Alias_Array_सूचकांक]),
                            outTable+offsets[tagged_Alias_Array_सूचकांक],
                            pErrorCode);
        }
    }

    वापस header_आकार+2*(int32_t)शीर्ष_Offset;
}

#पूर्ण_अगर


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

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
    uint32_t list_विस्थापनम्;
    uint32_t listIdx;
} UAliasContext;

स्थिर ध्रुव वर्ण दत्त_NAME[] = "cnvalias";
स्थिर ध्रुव वर्ण दत्त_TYPE[] = "icu";

स्थिर UData_स्मृतिः *g_Alias_दत्त=शून्यसूचकः;
स्थिर icu::UInitOnce g_Alias_दत्त_Init_Once {};

गणना {
    toc_दीर्घता_सूचकांक=0,
    converter_List_सूचकांक=1,
    चिह्नकम्_List_सूचकांक=2,
    alias_List_सूचकांक=3,
    untagged_Conv_Array_सूचकांक=4,
    tagged_Alias_Array_सूचकांक=5,
    tagged_Alias_Lists_सूचकांक=6,
    सारणी_Options_सूचकांक=7,
    string_सारणी_सूचकांक=8,
    normalized_String_सारणी_सूचकांक=9,
    offsets_गणनम्,    /* length of the swapper's temporary offsets[] */
    min_Toc_दीर्घता=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

स्थिर ध्रुव UConverterAliasOptions default_सारणी_Options = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
स्थिर UConverterAlias g_Main_सारणी;

#परिभाषय GET_STRING(idx) (ध्रुव वर्ण *)(g_Main_सारणी.string_सारणी + (idx))
#परिभाषय GET_NORMALIZED_STRING(idx) (ध्रुव वर्ण *)(g_Main_सारणी.normalized_String_सारणी + (idx))

स्थिर UBool U_CALLCONV
isAcceptable(शून्य * /*context*/,
             ध्रुव वर्ण * /*type*/, ध्रुव वर्ण * /*name*/,
             ध्रुव UDataInfo *pInfo) {
    प्रत्यागम
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->दत्त_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->दत्त_Format[1]==0x76 &&
        pInfo->दत्त_Format[2]==0x41 &&
        pInfo->दत्त_Format[3]==0x6c &&
        pInfo->format_संस्करणम्[0]==3;
}

स्थिर UBool U_CALLCONV ucnv_io_cleanup()
{
    यदि (g_Alias_दत्त) {
        udata_close(g_Alias_दत्त);
        g_Alias_दत्त = शून्यसूचकः;
    }
    g_Alias_दत्त_Init_Once.reset();

    uprv_memset(&g_Main_सारणी, 0, परिमाण(g_Main_सारणी));

    प्रत्यागम सत्य;                   /* Everything was cleaned up */
}

स्थिर शून्य U_CALLCONV init_Alias_दत्त(UError_संहिता &err_संहिता) {
    UData_स्मृतिः *दत्त;
    ध्रुव uint16_t *सारणी;
    ध्रुव uint32_t *sectionSizes;
    uint32_t सारणी_Start;
    uint32_t curr_विस्थापनम्;
    int32_t आकार_सम्बन्धि_दत्त;
    int32_t आकार_सम्बन्धि_TOC;

    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_दत्त == शून्यसूचकः);
    दत्त = udata_openChoice(शून्यसूचकः, दत्त_TYPE, दत्त_NAME, isAcceptable, शून्यसूचकः, &err_संहिता);
    यदि (U_FAILURE(err_संहिता)) {
        प्रत्यागम;
    }

    sectionSizes = static_cast<ध्रुव uint32_t*>(udata_get_स्मृतिः(दत्त));
    int32_t दत्त_दीर्घता = udata_get_दीर्घता(दत्त); // This is the length minus the UDataInfo size
    UBool is_दत्त_दीर्घता_ज्ञातम् = दत्त_दीर्घता >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    यदि (is_दत्त_दीर्घता_ज्ञातम् && दत्त_दीर्घता <= int32_t(परिमाण(sectionSizes[0]))) {
        // We don't even have a TOC!
        गच्छ invalidFormat;
    }
    सारणी = reinterpret_cast<ध्रुव uint16_t*>(sectionSizes);
    सारणी_Start = sectionSizes[0];
    आकार_सम्बन्धि_TOC = int32_t((सारणी_Start + 1) * परिमाण(sectionSizes[0]));
    यदि (सारणी_Start < min_Toc_दीर्घता || (is_दत्त_दीर्घता_ज्ञातम् && दत्त_दीर्घता <= आकार_सम्बन्धि_TOC)) {
        // We don't have a whole TOC!
        गच्छ invalidFormat;
    }
    g_Alias_दत्त = दत्त;

    g_Main_सारणी.converter_List_आकार      = sectionSizes[1];
    g_Main_सारणी.चिह्नकम्_List_आकार            = sectionSizes[2];
    g_Main_सारणी.alias_List_आकार          = sectionSizes[3];
    g_Main_सारणी.untagged_Conv_Array_आकार  = sectionSizes[4];
    g_Main_सारणी.tagged_Alias_Array_आकार   = sectionSizes[5];
    g_Main_सारणी.tagged_Alias_Lists_आकार   = sectionSizes[6];
    g_Main_सारणी.option_सारणी_आकार        = sectionSizes[7];
    g_Main_सारणी.string_सारणी_आकार        = sectionSizes[8];

    यदि (सारणी_Start > min_Toc_दीर्घता) {
        g_Main_सारणी.normalized_String_सारणी_आकार = sectionSizes[9];
    }

    आकार_सम्बन्धि_दत्त = आकार_सम्बन्धि_TOC;
    प्रति (uint32_t section = 1; section <= सारणी_Start; section++) {
        आकार_सम्बन्धि_दत्त += sectionSizes[section] * परिमाण(सारणी[0]);
    }
    यदि (is_दत्त_दीर्घता_ज्ञातम् && दत्त_दीर्घता < आकार_सम्बन्धि_दत्त) {
        // Truncated file!
        गच्छ invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    curr_विस्थापनम् = (सारणी_Start + 1) * (परिमाण(uint32_t)/परिमाण(uint16_t));
    g_Main_सारणी.converterList = सारणी + curr_विस्थापनम्;

    curr_विस्थापनम् += g_Main_सारणी.converter_List_आकार;
    g_Main_सारणी.चिह्नकम्_List = सारणी + curr_विस्थापनम्;

    curr_विस्थापनम् += g_Main_सारणी.चिह्नकम्_List_आकार;
    g_Main_सारणी.aliasList = सारणी + curr_विस्थापनम्;

    curr_विस्थापनम् += g_Main_सारणी.alias_List_आकार;
    g_Main_सारणी.untaggedConvArray = सारणी + curr_विस्थापनम्;

    curr_विस्थापनम् += g_Main_सारणी.untagged_Conv_Array_आकार;
    g_Main_सारणी.taggedAliasArray = सारणी + curr_विस्थापनम्;

    /* aliasLists is a 1's based array, but it has a padding character */
    curr_विस्थापनम् += g_Main_सारणी.tagged_Alias_Array_आकार;
    g_Main_सारणी.taggedAliasLists = सारणी + curr_विस्थापनम्;

    curr_विस्थापनम् += g_Main_सारणी.tagged_Alias_Lists_आकार;
    यदि (g_Main_सारणी.option_सारणी_आकार > 0
        && reinterpret_cast<ध्रुव UConverterAliasOptions*>(सारणी + curr_विस्थापनम्)->stringNormalizationType < UCNV_IO_NORM_TYPE_गणनम्)
    {
        /* Faster table */
        g_Main_सारणी.option_सारणी = reinterpret_cast<ध्रुव UConverterAliasOptions*>(सारणी + curr_विस्थापनम्);
    }
    अन्यथा {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        g_Main_सारणी.option_सारणी = &default_सारणी_Options;
    }

    curr_विस्थापनम् += g_Main_सारणी.option_सारणी_आकार;
    g_Main_सारणी.string_सारणी = सारणी + curr_विस्थापनम्;

    curr_विस्थापनम् += g_Main_सारणी.string_सारणी_आकार;
    g_Main_सारणी.normalized_String_सारणी = ((g_Main_सारणी.option_सारणी->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? g_Main_सारणी.string_सारणी : (सारणी + curr_विस्थापनम्));

    प्रत्यागम;

invalidFormat:
    err_संहिता = U_INVALID_FORMAT_ERROR;
    udata_close(दत्त);
}


स्थिर UBool
have_Alias_दत्त(UError_संहिता *p_Error_संहिता) {
    umtx_initOnce(g_Alias_दत्त_Init_Once, &init_Alias_दत्त, *p_Error_संहिता);
    प्रत्यागम U_SUCCESS(*p_Error_संहिता);
}

स्थिर अन्तःस्थ UBool
isAlias(ध्रुव वर्ण *alias, UError_संहिता *p_Error_संहिता) {
    यदि(alias==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम असत्य;
    }
    प्रत्यागम *alias != 0;
}

स्थिर uint32_t get_चिह्नकम्_सङ्ख्या(ध्रुव वर्ण *tagname) {
    यदि (g_Main_सारणी.चिह्नकम्_List) {
        uint32_t चिह्नकम्_अङ्कः;
        प्रति (चिह्नकम्_अङ्कः = 0; चिह्नकम्_अङ्कः < g_Main_सारणी.चिह्नकम्_List_आकार; चिह्नकम्_अङ्कः++) {
            यदि (!uprv_stricmp(GET_STRING(g_Main_सारणी.चिह्नकम्_List[चिह्नकम्_अङ्कः]), tagname)) {
                प्रत्यागम चिह्नकम्_अङ्कः;
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

#परिभाषय GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

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

#परिभाषय GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#यदि U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#नोचेद्यदि U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#अन्यथा
#   error U_CHARSET_FAMILY is not valid
#यदि_अन्तः


/* @see ucnv_compareNames */
U_CAPI वर्ण* U_CALLCONV
ucnv_io_stripASCIIForCompare(वर्ण* dst U_LIFETIME_BOUND, ध्रुव वर्ण* name) {
    वर्ण *dstItr = dst;
    uint8_t type, अग्र_Type;
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
                अग्र_Type = GET_ASCII_TYPE(*name);
                यदि (अग्र_Type == ZERO || अग्र_Type == NONZERO) {
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
    uint8_t type, अग्र_Type;
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
                अग्र_Type = GET_EBCDIC_TYPE(*name);
                यदि (अग्र_Type == ZERO || अग्र_Type == NONZERO) {
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
ucnv_compare_नामानि(ध्रुव वर्ण *name1, ध्रुव वर्ण *name2) {
    पूर्णाङ्क rc;
    uint8_t type, अग्र_Type;
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
                    अग्र_Type = GET_CHAR_TYPE(*name1);
                    यदि (अग्र_Type == ZERO || अग्र_Type == NONZERO) {
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
                    अग्र_Type = GET_CHAR_TYPE(*name2);
                    यदि (अग्र_Type == ZERO || अग्र_Type == NONZERO) {
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
        rc = (पूर्णाङ्क)(अचिह्नित वर्ण)c1 - (पूर्णाङ्क)(अचिह्नित वर्ण)c2;
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
findConverter(ध्रुव वर्ण *alias, UBool *अन्तर्भवति_Option, UError_संहिता *p_Error_संहिता) {
    uint32_t mid, start, सीमा;
    uint32_t चरमः_Mid;
    पूर्णाङ्क परिणाम;
    पूर्णाङ्क isUnnormalized = (g_Main_सारणी.option_सारणी->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    वर्ण strippedName[UCNV_MAX_CONVERTER_NAME_दीर्घता];

    यदि (!isUnnormalized) {
        यदि (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_दीर्घता) {
            *p_Error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
            प्रत्यागम UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    start = 0;
    सीमा = g_Main_सारणी.untagged_Conv_Array_आकार;
    mid = सीमा;
    चरमः_Mid = UINT32_MAX;

    प्रति (;;) {
        mid = (start + सीमा) / 2;
        यदि (चरमः_Mid == mid) {   /* Have we moved? */
            भङ्ग;  /* We haven't moved, and it wasn't found. */
        }
        चरमः_Mid = mid;
        यदि (isUnnormalized) {
            परिणाम = ucnv_compare_नामानि(alias, GET_STRING(g_Main_सारणी.aliasList[mid]));
        }
        अन्यथा {
            परिणाम = uprv_strcmp(alias, GET_NORMALIZED_STRING(g_Main_सारणी.aliasList[mid]));
        }

        यदि (परिणाम < 0) {
            सीमा = mid;
        } अन्यथा यदि (परिणाम > 0) {
            start = mid;
        } अन्यथा {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            यदि (g_Main_सारणी.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_BIT) {
                *p_Error_संहिता = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            यदि (अन्तर्भवति_Option) {
                UBool अन्तर्भवति_Cnv_Option_Info = static_cast<UBool>(g_Main_सारणी.option_सारणी->अन्तर्भवति_Cnv_Option_Info);
                *अन्तर्भवति_Option = static_cast<UBool>((अन्तर्भवति_Cnv_Option_Info
                    && ((g_Main_सारणी.untaggedConvArray[mid] & UCNV_अन्तर्भवति_OPTION_BIT) != 0))
                    || !अन्तर्भवति_Cnv_Option_Info);
            }
            प्रत्यागम g_Main_सारणी.untaggedConvArray[mid] & UCNV_CONVERTER_सूचकांक_आवरणम्;
        }
    }

    प्रत्यागम UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
स्थिर अन्तःस्थ UBool
is_Alias_अन्तर्_List(ध्रुव वर्ण *alias, uint32_t list_विस्थापनम्) {
    यदि (list_विस्थापनम्) {
        uint32_t currAlias;
        uint32_t list_गणनम् = g_Main_सारणी.taggedAliasLists[list_विस्थापनम्];
        /* +1 to skip listCount */
        ध्रुव uint16_t *currList = g_Main_सारणी.taggedAliasLists + list_विस्थापनम् + 1;
        प्रति (currAlias = 0; currAlias < list_गणनम्; currAlias++) {
            यदि (currList[currAlias]
                && ucnv_compare_नामानि(alias, GET_STRING(currList[currAlias]))==0)
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
find_Tagged_Alias_Lists_विस्थापनम्(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UError_संहिता *p_Error_संहिता) {
    uint32_t idx;
    uint32_t list_विस्थापनम्;
    uint32_t conv_अङ्कः;
    UError_संहिता myErr = U_ZERO_ERROR;
    uint32_t चिह्नकम्_अङ्कः = get_चिह्नकम्_सङ्ख्या(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_अङ्कः = findConverter(alias, शून्यसूचकः, &myErr);
    यदि (myErr != U_ZERO_ERROR) {
        *p_Error_संहिता = myErr;
    }

    यदि (चिह्नकम्_अङ्कः < (g_Main_सारणी.चिह्नकम्_List_आकार - UCNV_अङ्कः_HIDDEN_चिह्नकानि) && conv_अङ्कः < g_Main_सारणी.converter_List_आकार) {
        list_विस्थापनम् = g_Main_सारणी.taggedAliasArray[चिह्नकम्_अङ्कः*g_Main_सारणी.converter_List_आकार + conv_अङ्कः];
        यदि (list_विस्थापनम् && g_Main_सारणी.taggedAliasLists[list_विस्थापनम् + 1]) {
            प्रत्यागम list_विस्थापनम्;
        }
        यदि (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            प्रति (idx = 0; idx < g_Main_सारणी.tagged_Alias_Array_आकार; idx++) {
                list_विस्थापनम् = g_Main_सारणी.taggedAliasArray[idx];
                यदि (list_विस्थापनम् && is_Alias_अन्तर्_List(alias, list_विस्थापनम्)) {
                    uint32_t curr_चिह्नकम्_अङ्कः = idx/g_Main_सारणी.converter_List_आकार;
                    uint32_t curr_Conv_अङ्कः = (idx - curr_चिह्नकम्_अङ्कः*g_Main_सारणी.converter_List_आकार);
                    uint32_t अस्थायी_List_विस्थापनम् = g_Main_सारणी.taggedAliasArray[चिह्नकम्_अङ्कः*g_Main_सारणी.converter_List_आकार + curr_Conv_अङ्कः];
                    यदि (अस्थायी_List_विस्थापनम् && g_Main_सारणी.taggedAliasLists[अस्थायी_List_विस्थापनम् + 1]) {
                        प्रत्यागम अस्थायी_List_विस्थापनम्;
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
find_Tagged_Converter_अङ्कः(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UError_संहिता *p_Error_संहिता) {
    uint32_t idx;
    uint32_t list_विस्थापनम्;
    uint32_t conv_अङ्कः;
    UError_संहिता myErr = U_ZERO_ERROR;
    uint32_t चिह्नकम्_अङ्कः = get_चिह्नकम्_सङ्ख्या(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_अङ्कः = findConverter(alias, शून्यसूचकः, &myErr);
    यदि (myErr != U_ZERO_ERROR) {
        *p_Error_संहिता = myErr;
    }

    यदि (चिह्नकम्_अङ्कः < (g_Main_सारणी.चिह्नकम्_List_आकार - UCNV_अङ्कः_HIDDEN_चिह्नकानि) && conv_अङ्कः < g_Main_सारणी.converter_List_आकार) {
        list_विस्थापनम् = g_Main_सारणी.taggedAliasArray[चिह्नकम्_अङ्कः*g_Main_सारणी.converter_List_आकार + conv_अङ्कः];
        यदि (list_विस्थापनम् && is_Alias_अन्तर्_List(alias, list_विस्थापनम्)) {
            प्रत्यागम conv_अङ्कः;
        }
        यदि (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t convStart = (चिह्नकम्_अङ्कः)*g_Main_सारणी.converter_List_आकार;
            uint32_t conv_सीमा = (चिह्नकम्_अङ्कः+1)*g_Main_सारणी.converter_List_आकार;
            प्रति (idx = convStart; idx < conv_सीमा; idx++) {
                list_विस्थापनम् = g_Main_सारणी.taggedAliasArray[idx];
                यदि (list_विस्थापनम् && is_Alias_अन्तर्_List(alias, list_विस्थापनम्)) {
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
ucnv_io_getConverterName(ध्रुव वर्ण *alias, UBool *अन्तर्भवति_Option, UError_संहिता *p_Error_संहिता) {
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
        यदि(have_Alias_दत्त(p_Error_संहिता) && isAlias(aliasTmp, p_Error_संहिता)) {
            uint32_t conv_अङ्कः = findConverter(aliasTmp, अन्तर्भवति_Option, p_Error_संहिता);
            यदि (conv_अङ्कः < g_Main_सारणी.converter_List_आकार) {
                प्रत्यागम GET_STRING(g_Main_सारणी.converterList[conv_अङ्कः]);
            }
            /* else converter not found */
        } अन्यथा {
            भङ्ग;
        }
    }

    प्रत्यागम शून्यसूचकः;
}

U_CDECL_BEGIN


स्थिर int32_t U_CALLCONV
ucnv_io_गणनम्_Standard_Aliases(UEnumeration *enumerator, UError_संहिता * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_विस्थापनम् = myContext->list_विस्थापनम्;

    यदि (list_विस्थापनम्) {
        value = g_Main_सारणी.taggedAliasLists[list_विस्थापनम्];
    }
    प्रत्यागम value;
}

स्थिर ध्रुव वर्ण * U_CALLCONV
ucnv_io_अग्र_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* परिणाम_दीर्घता,
                            UError_संहिता * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_विस्थापनम् = myContext->list_विस्थापनम्;

    यदि (list_विस्थापनम्) {
        uint32_t list_गणनम् = g_Main_सारणी.taggedAliasLists[list_विस्थापनम्];
        ध्रुव uint16_t *currList = g_Main_सारणी.taggedAliasLists + list_विस्थापनम् + 1;

        यदि (myContext->listIdx < list_गणनम्) {
            ध्रुव वर्ण *myStr = GET_STRING(currList[myContext->listIdx++]);
            यदि (परिणाम_दीर्घता) {
                *परिणाम_दीर्घता = (int32_t)uprv_strlen(myStr);
            }
            प्रत्यागम myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    यदि (परिणाम_दीर्घता) {
        *परिणाम_दीर्घता = 0;
    }
    प्रत्यागम शून्यसूचकः;
}

स्थिर शून्य U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UError_संहिता * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

स्थिर शून्य U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_अन्तः

/* Enumerate the aliases for the specified converter and standard tag */
स्थिर ध्रुव UEnumeration gEnumAliases = {
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_io_closeUEnumeration,
    ucnv_io_गणनम्_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_अग्र_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_Standard_नामानि(ध्रुव वर्ण *convName,
                       ध्रुव वर्ण *standard,
                       UError_संहिता *p_Error_संहिता)
{
    UEnumeration *myEnum = शून्यसूचकः;
    यदि (have_Alias_दत्त(p_Error_संहिता) && isAlias(convName, p_Error_संहिता)) {
        uint32_t list_विस्थापनम् = find_Tagged_Alias_Lists_विस्थापनम्(convName, standard, p_Error_संहिता);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        यदि (list_विस्थापनम् < g_Main_सारणी.tagged_Alias_Lists_आकार) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(परिमाण(UEnumeration)));
            यदि (myEnum == शून्यसूचकः) {
                *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम शून्यसूचकः;
            }
            uprv_memcpy(myEnum, &gEnumAliases, परिमाण(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(परिमाण(UAliasContext)));
            यदि (myContext == शून्यसूचकः) {
                *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                uprv_free(myEnum);
                प्रत्यागम शून्यसूचकः;
            }
            myContext->list_विस्थापनम् = list_विस्थापनम्;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    प्रत्यागम myEnum;
}

स्थिर uint16_t
ucnv_io_गणनम्_Aliases(ध्रुव वर्ण *alias, UError_संहिता *p_Error_संहिता) {
    यदि(have_Alias_दत्त(p_Error_संहिता) && isAlias(alias, p_Error_संहिता)) {
        uint32_t conv_अङ्कः = findConverter(alias, शून्यसूचकः, p_Error_संहिता);
        यदि (conv_अङ्कः < g_Main_सारणी.converter_List_आकार) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_विस्थापनम् = g_Main_सारणी.taggedAliasArray[(g_Main_सारणी.चिह्नकम्_List_आकार - 1)*g_Main_सारणी.converter_List_आकार + conv_अङ्कः];

            यदि (list_विस्थापनम्) {
                प्रत्यागम g_Main_सारणी.taggedAliasLists[list_विस्थापनम्];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    प्रत्यागम 0;
}

स्थिर uint16_t
ucnv_io_getAliases(ध्रुव वर्ण *alias, uint16_t start, ध्रुव वर्ण **aliases, UError_संहिता *p_Error_संहिता) {
    यदि(have_Alias_दत्त(p_Error_संहिता) && isAlias(alias, p_Error_संहिता)) {
        uint32_t currAlias;
        uint32_t conv_अङ्कः = findConverter(alias, शून्यसूचकः, p_Error_संहिता);
        यदि (conv_अङ्कः < g_Main_सारणी.converter_List_आकार) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_विस्थापनम् = g_Main_सारणी.taggedAliasArray[(g_Main_सारणी.चिह्नकम्_List_आकार - 1)*g_Main_सारणी.converter_List_आकार + conv_अङ्कः];

            यदि (list_विस्थापनम्) {
                uint32_t list_गणनम् = g_Main_सारणी.taggedAliasLists[list_विस्थापनम्];
                /* +1 to skip listCount */
                ध्रुव uint16_t *currList = g_Main_सारणी.taggedAliasLists + list_विस्थापनम् + 1;

                प्रति (currAlias = start; currAlias < list_गणनम्; currAlias++) {
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
ucnv_io_getAlias(ध्रुव वर्ण *alias, uint16_t n, UError_संहिता *p_Error_संहिता) {
    यदि(have_Alias_दत्त(p_Error_संहिता) && isAlias(alias, p_Error_संहिता)) {
        uint32_t conv_अङ्कः = findConverter(alias, शून्यसूचकः, p_Error_संहिता);
        यदि (conv_अङ्कः < g_Main_सारणी.converter_List_आकार) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_विस्थापनम् = g_Main_सारणी.taggedAliasArray[(g_Main_सारणी.चिह्नकम्_List_आकार - 1)*g_Main_सारणी.converter_List_आकार + conv_अङ्कः];

            यदि (list_विस्थापनम्) {
                uint32_t list_गणनम् = g_Main_सारणी.taggedAliasLists[list_विस्थापनम्];
                /* +1 to skip listCount */
                ध्रुव uint16_t *currList = g_Main_सारणी.taggedAliasLists + list_विस्थापनम् + 1;

                यदि (n < list_गणनम्)  {
                    प्रत्यागम GET_STRING(currList[n]);
                }
                *p_Error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    प्रत्यागम शून्यसूचकः;
}

स्थिर uint16_t
ucnv_io_गणनम्_Standards(UError_संहिता *p_Error_संहिता) {
    यदि (have_Alias_दत्त(p_Error_संहिता)) {
        /* Don't include the empty list */
        प्रत्यागम static_cast<uint16_t>(g_Main_सारणी.चिह्नकम्_List_आकार - UCNV_अङ्कः_HIDDEN_चिह्नकानि);
    }

    प्रत्यागम 0;
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
ucnv_getStandard(uint16_t n, UError_संहिता *p_Error_संहिता) {
    यदि (have_Alias_दत्त(p_Error_संहिता)) {
        यदि (n < g_Main_सारणी.चिह्नकम्_List_आकार - UCNV_अङ्कः_HIDDEN_चिह्नकानि) {
            प्रत्यागम GET_STRING(g_Main_सारणी.चिह्नकम्_List[n]);
        }
        *p_Error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
    }

    प्रत्यागम शून्यसूचकः;
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
ucnv_getStandardName(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UError_संहिता *p_Error_संहिता) {
    यदि (have_Alias_दत्त(p_Error_संहिता) && isAlias(alias, p_Error_संहिता)) {
        uint32_t list_विस्थापनम् = find_Tagged_Alias_Lists_विस्थापनम्(alias, standard, p_Error_संहिता);

        यदि (0 < list_विस्थापनम् && list_विस्थापनम् < g_Main_सारणी.tagged_Alias_Lists_आकार) {
            ध्रुव uint16_t *currList = g_Main_सारणी.taggedAliasLists + list_विस्थापनम् + 1;

            /* Get the preferred name from this list */
            यदि (currList[0]) {
                प्रत्यागम GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    प्रत्यागम शून्यसूचकः;
}

U_CAPI uint16_t U_EXPORT2
ucnv_गणनम्_Aliases(ध्रुव वर्ण *alias, UError_संहिता *p_Error_संहिता)
{
    प्रत्यागम ucnv_io_गणनम्_Aliases(alias, p_Error_संहिता);
}


U_CAPI ध्रुव वर्ण* U_EXPORT2
ucnv_getAlias(ध्रुव वर्ण *alias, uint16_t n, UError_संहिता *p_Error_संहिता)
{
    प्रत्यागम ucnv_io_getAlias(alias, n, p_Error_संहिता);
}

U_CAPI शून्य U_EXPORT2
ucnv_getAliases(ध्रुव वर्ण *alias, ध्रुव वर्ण **aliases, UError_संहिता *p_Error_संहिता)
{
    ucnv_io_getAliases(alias, 0, aliases, p_Error_संहिता);
}

U_CAPI uint16_t U_EXPORT2
ucnv_गणनम्_Standards()
{
    UError_संहिता err = U_ZERO_ERROR;
    प्रत्यागम ucnv_io_गणनम्_Standards(&err);
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
ucnv_getCanonicalName(ध्रुव वर्ण *alias, ध्रुव वर्ण *standard, UError_संहिता *p_Error_संहिता) {
    यदि (have_Alias_दत्त(p_Error_संहिता) && isAlias(alias, p_Error_संहिता)) {
        uint32_t conv_अङ्कः = find_Tagged_Converter_अङ्कः(alias, standard, p_Error_संहिता);

        यदि (conv_अङ्कः < g_Main_सारणी.converter_List_आकार) {
            प्रत्यागम GET_STRING(g_Main_सारणी.converterList[conv_अङ्कः]);
        }
    }

    प्रत्यागम शून्यसूचकः;
}

U_CDECL_BEGIN


स्थिर int32_t U_CALLCONV
ucnv_io_गणनम्_All_Converters(UEnumeration * /*enumerator*/, UError_संहिता * /*pErrorCode*/) {
    प्रत्यागम g_Main_सारणी.converter_List_आकार;
}

स्थिर ध्रुव वर्ण * U_CALLCONV
ucnv_io_अग्र_All_Converters(UEnumeration *enumerator,
                            int32_t* परिणाम_दीर्घता,
                            UError_संहिता * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    यदि (*myContext < g_Main_सारणी.converter_List_आकार) {
        ध्रुव वर्ण *myStr = GET_STRING(g_Main_सारणी.converterList[(*myContext)++]);
        यदि (परिणाम_दीर्घता) {
            *परिणाम_दीर्घता = (int32_t)uprv_strlen(myStr);
        }
        प्रत्यागम myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    यदि (परिणाम_दीर्घता) {
        *परिणाम_दीर्घता = 0;
    }
    प्रत्यागम शून्यसूचकः;
}

स्थिर शून्य U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UError_संहिता * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_अन्तः
स्थिर ध्रुव UEnumeration gEnumAllConverters = {
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_io_closeUEnumeration,
    ucnv_io_गणनम्_All_Converters,
    uenum_unextDefault,
    ucnv_io_अग्र_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_All_नामानि(UError_संहिता *p_Error_संहिता) {
    UEnumeration *myEnum = शून्यसूचकः;
    यदि (have_Alias_दत्त(p_Error_संहिता)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(परिमाण(UEnumeration)));
        यदि (myEnum == शून्यसूचकः) {
            *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, परिमाण(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(परिमाण(uint16_t)));
        यदि (myContext == शून्यसूचकः) {
            *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            uprv_free(myEnum);
            प्रत्यागम शून्यसूचकः;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    प्रत्यागम myEnum;
}

U_CAPI uint16_t
ucnv_io_गणनम्_ज्ञातम्_Converters(UError_संहिता *p_Error_संहिता) {
    यदि (have_Alias_दत्त(p_Error_संहिता)) {
        प्रत्यागम (uint16_t)g_Main_सारणी.converter_List_आकार;
    }
    प्रत्यागम 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

प्रकारनिर्देश वर्ण * U_CALLCONV StripForCompareFn(वर्ण *dst, ध्रुव वर्ण *name);
U_CDECL_अन्तः


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
प्रकारनिर्देश संरचना अस्थायी_Row {
    uint16_t str_सूचकांक, क्रमय_सूचकांक;
} अस्थायी_Row;

प्रकारनिर्देश संरचना अस्थायी_Alias_सारणी {
    ध्रुव वर्ण *chars;
    अस्थायी_Row *पङ्क्तिकाः;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} अस्थायी_Alias_सारणी;

गणना {
    राशिः_ROW_क्षमता=500
};

स्थिर int32_t U_CALLCONV
io_compare_पङ्क्तिकाः(ध्रुव शून्य *context, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    वर्ण stripped_वामः[UCNV_MAX_CONVERTER_NAME_दीर्घता],
         stripped_दक्षिणः[UCNV_MAX_CONVERTER_NAME_दीर्घता];

    अस्थायी_Alias_सारणी *अस्थायी_सारणी=(अस्थायी_Alias_सारणी *)context;
    ध्रुव वर्ण *chars=अस्थायी_सारणी->chars;

    प्रत्यागम static_cast<int32_t>(uprv_strcmp(
        अस्थायी_सारणी->stripForCompare(stripped_वामः, chars + 2 * static_cast<ध्रुव अस्थायी_Row*>(वामतः)->str_सूचकांक),
        अस्थायी_सारणी->stripForCompare(stripped_दक्षिणः, chars + 2 * static_cast<ध्रुव अस्थायी_Row*>(दक्षिणतः)->str_सूचकांक)));
}

U_CAPI int32_t U_EXPORT2
ucnv_swapAliases(ध्रुव UDataSwapper *ds,
                 ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                 UError_संहिता *p_Error_संहिता) {
    ध्रुव UDataInfo *pInfo;
    int32_t शीर्षकम्_आकार;

    ध्रुव uint16_t *अन्तर्_सारणी;
    ध्रुव uint32_t *अन्तर्_Section_Sizes;
    uint32_t toc[offsets_गणनम्];
    uint32_t offsets[offsets_गणनम्]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_दीर्घता, शिखरम्_विस्थापनम्;

    अस्थायी_Row पङ्क्तिकाः[राशिः_ROW_क्षमता];
    uint16_t resort[राशिः_ROW_क्षमता];
    अस्थायी_Alias_सारणी अस्थायी_सारणी;

    /* udata_swapDataHeader checks the arguments */
    शीर्षकम्_आकार=udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
    यदि(!(
        pInfo->दत्त_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->दत्त_Format[1]==0x76 &&
        pInfo->दत्त_Format[2]==0x41 &&
        pInfo->दत्त_Format[3]==0x6c &&
        pInfo->format_संस्करणम्[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->दत्त_Format[0], pInfo->दत्त_Format[1],
                         pInfo->दत्त_Format[2], pInfo->दत्त_Format[3],
                         pInfo->format_संस्करणम्[0]);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    /* an alias table must contain at least the table of contents array */
    यदि(length>=0 && (length-शीर्षकम्_आकार)<4*(1+min_Toc_दीर्घता)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-शीर्षकम्_आकार);
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_Section_Sizes=(ध्रुव uint32_t *)((ध्रुव वर्ण *)अन्तर्_दत्त+शीर्षकम्_आकार);
    अन्तर्_सारणी=(ध्रुव uint16_t *)अन्तर्_Section_Sizes;
    uprv_memset(toc, 0, परिमाण(toc));
    toc[toc_दीर्घता_सूचकांक]=toc_दीर्घता=ds->readUInt32(अन्तर्_Section_Sizes[toc_दीर्घता_सूचकांक]);
    यदि(toc_दीर्घता<min_Toc_दीर्घता || offsets_गणनम्<=toc_दीर्घता) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_दीर्घता);
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम 0;
    }

    /* read the known part of the table of contents */
    प्रति(i=converter_List_सूचकांक; i<=toc_दीर्घता; ++i) {
        toc[i]=ds->readUInt32(अन्तर्_Section_Sizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, परिमाण(offsets));
    offsets[converter_List_सूचकांक]=2*(1+toc_दीर्घता); /* count two 16-bit units per toc entry */
    प्रति(i=चिह्नकम्_List_सूचकांक; i<=toc_दीर्घता; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    शिखरम्_विस्थापनम्=offsets[i-1]+toc[i-1];

    यदि(length>=0) {
        uint16_t *बहिर्गमनम्_सारणी;
        ध्रुव uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_सूचकांक;

        यदि((length-शीर्षकम्_आकार)<(2*(int32_t)शिखरम्_विस्थापनम्)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-शीर्षकम्_आकार);
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        बहिर्गमनम्_सारणी=(uint16_t *)((वर्ण *)बहिर्गमनम्_दत्त+शीर्षकम्_आकार);

        /* swap the entire table of contents */
        ds->swapArray32(ds, अन्तर्_सारणी, 4*(1+toc_दीर्घता), बहिर्गमनम्_सारणी, p_Error_संहिता);

        /* swap unormalized strings & normalized strings */
        ds->swapInvChars(ds, अन्तर्_सारणी+offsets[string_सारणी_सूचकांक], 2*(int32_t)(toc[string_सारणी_सूचकांक]+toc[normalized_String_सारणी_सूचकांक]),
                             बहिर्गमनम्_सारणी+offsets[string_सारणी_सूचकांक], p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            प्रत्यागम 0;
        }

        यदि(ds->अन्तर्_Charset==ds->बहिर्गमनम्_Charset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->swapArray16(ds,
                            अन्तर्_सारणी+offsets[converter_List_सूचकांक],
                            2*(int32_t)(offsets[string_सारणी_सूचकांक]-offsets[converter_List_सूचकांक]),
                            बहिर्गमनम्_सारणी+offsets[converter_List_सूचकांक],
                            p_Error_संहिता);
        } अन्यथा {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_सूचकांक];

            अस्थायी_सारणी.chars=(ध्रुव वर्ण *)(बहिर्गमनम्_सारणी+offsets[string_सारणी_सूचकांक]); /* sort by outCharset */

            यदि(count<=राशिः_ROW_क्षमता) {
                अस्थायी_सारणी.पङ्क्तिकाः=पङ्क्तिकाः;
                अस्थायी_सारणी.resort=resort;
            } अन्यथा {
                अस्थायी_सारणी.पङ्क्तिकाः=(अस्थायी_Row *)uprv_malloc(count*परिमाण(अस्थायी_Row)+count*2);
                यदि(अस्थायी_सारणी.पङ्क्तिकाः==शून्यसूचकः) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                    प्रत्यागम 0;
                }
                अस्थायी_सारणी.resort=(uint16_t *)(अस्थायी_सारणी.पङ्क्तिकाः+count);
            }

            यदि(ds->बहिर्गमनम्_Charset==U_ASCII_FAMILY) {
                अस्थायी_सारणी.stripForCompare=ucnv_io_stripASCIIForCompare;
            } अन्यथा /* U_EBCDIC_FAMILY */ {
                अस्थायी_सारणी.stripForCompare=ucnv_io_stripEBCDICForCompare;
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
            p=अन्तर्_सारणी+offsets[alias_List_सूचकांक];
            q=बहिर्गमनम्_सारणी+offsets[alias_List_सूचकांक];

            p2=अन्तर्_सारणी+offsets[untagged_Conv_Array_सूचकांक];
            q2=बहिर्गमनम्_सारणी+offsets[untagged_Conv_Array_सूचकांक];

            प्रति(i=0; i<count; ++i) {
                अस्थायी_सारणी.पङ्क्तिकाः[i].str_सूचकांक=ds->readUInt16(p[i]);
                अस्थायी_सारणी.पङ्क्तिकाः[i].क्रमय_सूचकांक=(uint16_t)i;
            }

            uprv_क्रमय_Array(अस्थायी_सारणी.पङ्क्तिकाः, (int32_t)count, परिमाण(अस्थायी_Row),
                           io_compare_पङ्क्तिकाः, &अस्थायी_सारणी,
                           असत्य, p_Error_संहिता);

            यदि(U_SUCCESS(*p_Error_संहिता)) {
                /* copy/swap/permutate items */
                यदि(p!=q) {
                    प्रति(i=0; i<count; ++i) {
                        old_सूचकांक=अस्थायी_सारणी.पङ्क्तिकाः[i].क्रमय_सूचकांक;
                        ds->swapArray16(ds, p+old_सूचकांक, 2, q+i, p_Error_संहिता);
                        ds->swapArray16(ds, p2+old_सूचकांक, 2, q2+i, p_Error_संहिता);
                    }
                } अन्यथा {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=अस्थायी_सारणी.resort;

                    प्रति(i=0; i<count; ++i) {
                        old_सूचकांक=अस्थायी_सारणी.पङ्क्तिकाः[i].क्रमय_सूचकांक;
                        ds->swapArray16(ds, p+old_सूचकांक, 2, r+i, p_Error_संहिता);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    प्रति(i=0; i<count; ++i) {
                        old_सूचकांक=अस्थायी_सारणी.पङ्क्तिकाः[i].क्रमय_सूचकांक;
                        ds->swapArray16(ds, p2+old_सूचकांक, 2, r+i, p_Error_संहिता);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            यदि(अस्थायी_सारणी.पङ्क्तिकाः!=पङ्क्तिकाः) {
                uprv_free(अस्थायी_सारणी.पङ्क्तिकाः);
            }

            यदि(U_FAILURE(*p_Error_संहिता)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                प्रत्यागम 0;
            }

            /* swap remaining 16-bit values */
            ds->swapArray16(ds,
                            अन्तर्_सारणी+offsets[converter_List_सूचकांक],
                            2*(int32_t)(offsets[alias_List_सूचकांक]-offsets[converter_List_सूचकांक]),
                            बहिर्गमनम्_सारणी+offsets[converter_List_सूचकांक],
                            p_Error_संहिता);
            ds->swapArray16(ds,
                            अन्तर्_सारणी+offsets[tagged_Alias_Array_सूचकांक],
                            2*(int32_t)(offsets[string_सारणी_सूचकांक]-offsets[tagged_Alias_Array_सूचकांक]),
                            बहिर्गमनम्_सारणी+offsets[tagged_Alias_Array_सूचकांक],
                            p_Error_संहिता);
        }
    }

    प्रत्यागम शीर्षकम्_आकार+2*(int32_t)शिखरम्_विस्थापनम्;
}

#यदि_अन्तः


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

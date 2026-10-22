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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/udata.h"

#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "udatamem.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ucnv_io.h"
#అన్తర్భావ "uenumimp.h"
#అన్తర్భావ "ucln_cmn.h"

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
రకనిర్వచనం నిర్మాణం UAliasContext {
    uint32_t list_స్థానభ్రంశం;
    uint32_t listIdx;
} UAliasContext;

స్థిర స్థిరరాశి అక్షరం దత్తాంశం_NAME[] = "cnvalias";
స్థిర స్థిరరాశి అక్షరం దత్తాంశం_TYPE[] = "icu";

స్థిర UData_జ్ఞాపకం *g_Alias_దత్తాంశం=శూన్యసూచిక;
స్థిర icu::UInitOnce g_Alias_దత్తాంశం_Init_Once {};

గణన {
    toc_నిడివి_సూచీ=0,
    converter_List_సూచీ=1,
    ట్యాగ్_List_సూచీ=2,
    alias_List_సూచీ=3,
    untagged_Conv_Array_సూచీ=4,
    tagged_Alias_Array_సూచీ=5,
    tagged_Alias_Lists_సూచీ=6,
    పట్టిక_Options_సూచీ=7,
    string_పట్టిక_సూచీ=8,
    normalized_String_పట్టిక_సూచీ=9,
    offsets_లెక్క,    /* length of the swapper's temporary offsets[] */
    min_Toc_నిడివి=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

స్థిర స్థిరరాశి UConverterAliasOptions default_పట్టిక_Options = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
స్థిర UConverterAlias g_Main_పట్టిక;

#నిర్వచించు GET_STRING(idx) (స్థిరరాశి అక్షరం *)(g_Main_పట్టిక.string_పట్టిక + (idx))
#నిర్వచించు GET_NORMALIZED_STRING(idx) (స్థిరరాశి అక్షరం *)(g_Main_పట్టిక.normalized_String_పట్టిక + (idx))

స్థిర UBool U_CALLCONV
isAcceptable(శూన్య * /*context*/,
             స్థిరరాశి అక్షరం * /*type*/, స్థిరరాశి అక్షరం * /*name*/,
             స్థిరరాశి UDataInfo *pInfo) {
    తిరిగి
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->దత్తాంశం_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->దత్తాంశం_Format[1]==0x76 &&
        pInfo->దత్తాంశం_Format[2]==0x41 &&
        pInfo->దత్తాంశం_Format[3]==0x6c &&
        pInfo->format_సంస్కరణ[0]==3;
}

స్థిర UBool U_CALLCONV ucnv_io_cleanup()
{
    ఒకవేళ (g_Alias_దత్తాంశం) {
        udata_close(g_Alias_దత్తాంశం);
        g_Alias_దత్తాంశం = శూన్యసూచిక;
    }
    g_Alias_దత్తాంశం_Init_Once.reset();

    uprv_memset(&g_Main_పట్టిక, 0, పరిమాణం(g_Main_పట్టిక));

    తిరిగి నిజం;                   /* Everything was cleaned up */
}

స్థిర శూన్య U_CALLCONV init_Alias_దత్తాంశం(UError_సంకేతం &err_సంకేతం) {
    UData_జ్ఞాపకం *దత్తాంశం;
    స్థిరరాశి uint16_t *పట్టిక;
    స్థిరరాశి uint32_t *sectionSizes;
    uint32_t పట్టిక_ప్రారంభం;
    uint32_t curr_స్థానభ్రంశం;
    int32_t కొలత_యొక్క_దత్తాంశం;
    int32_t కొలత_యొక్క_TOC;

    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_దత్తాంశం == శూన్యసూచిక);
    దత్తాంశం = udata_openChoice(శూన్యసూచిక, దత్తాంశం_TYPE, దత్తాంశం_NAME, isAcceptable, శూన్యసూచిక, &err_సంకేతం);
    ఒకవేళ (U_FAILURE(err_సంకేతం)) {
        తిరిగి;
    }

    sectionSizes = static_cast<స్థిరరాశి uint32_t*>(udata_get_జ్ఞాపకం(దత్తాంశం));
    int32_t దత్తాంశం_నిడివి = udata_get_నిడివి(దత్తాంశం); // This is the length minus the UDataInfo size
    UBool is_దత్తాంశం_నిడివి_తెలిసిన = దత్తాంశం_నిడివి >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    ఒకవేళ (is_దత్తాంశం_నిడివి_తెలిసిన && దత్తాంశం_నిడివి <= int32_t(పరిమాణం(sectionSizes[0]))) {
        // We don't even have a TOC!
        వెళ్ళు invalidFormat;
    }
    పట్టిక = reinterpret_cast<స్థిరరాశి uint16_t*>(sectionSizes);
    పట్టిక_ప్రారంభం = sectionSizes[0];
    కొలత_యొక్క_TOC = int32_t((పట్టిక_ప్రారంభం + 1) * పరిమాణం(sectionSizes[0]));
    ఒకవేళ (పట్టిక_ప్రారంభం < min_Toc_నిడివి || (is_దత్తాంశం_నిడివి_తెలిసిన && దత్తాంశం_నిడివి <= కొలత_యొక్క_TOC)) {
        // We don't have a whole TOC!
        వెళ్ళు invalidFormat;
    }
    g_Alias_దత్తాంశం = దత్తాంశం;

    g_Main_పట్టిక.converter_List_కొలత      = sectionSizes[1];
    g_Main_పట్టిక.ట్యాగ్_List_కొలత            = sectionSizes[2];
    g_Main_పట్టిక.alias_List_కొలత          = sectionSizes[3];
    g_Main_పట్టిక.untagged_Conv_Array_కొలత  = sectionSizes[4];
    g_Main_పట్టిక.tagged_Alias_Array_కొలత   = sectionSizes[5];
    g_Main_పట్టిక.tagged_Alias_Lists_కొలత   = sectionSizes[6];
    g_Main_పట్టిక.option_పట్టిక_కొలత        = sectionSizes[7];
    g_Main_పట్టిక.string_పట్టిక_కొలత        = sectionSizes[8];

    ఒకవేళ (పట్టిక_ప్రారంభం > min_Toc_నిడివి) {
        g_Main_పట్టిక.normalized_String_పట్టిక_కొలత = sectionSizes[9];
    }

    కొలత_యొక్క_దత్తాంశం = కొలత_యొక్క_TOC;
    కొరకు (uint32_t section = 1; section <= పట్టిక_ప్రారంభం; section++) {
        కొలత_యొక్క_దత్తాంశం += sectionSizes[section] * పరిమాణం(పట్టిక[0]);
    }
    ఒకవేళ (is_దత్తాంశం_నిడివి_తెలిసిన && దత్తాంశం_నిడివి < కొలత_యొక్క_దత్తాంశం) {
        // Truncated file!
        వెళ్ళు invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    curr_స్థానభ్రంశం = (పట్టిక_ప్రారంభం + 1) * (పరిమాణం(uint32_t)/పరిమాణం(uint16_t));
    g_Main_పట్టిక.converterList = పట్టిక + curr_స్థానభ్రంశం;

    curr_స్థానభ్రంశం += g_Main_పట్టిక.converter_List_కొలత;
    g_Main_పట్టిక.ట్యాగ్_List = పట్టిక + curr_స్థానభ్రంశం;

    curr_స్థానభ్రంశం += g_Main_పట్టిక.ట్యాగ్_List_కొలత;
    g_Main_పట్టిక.aliasList = పట్టిక + curr_స్థానభ్రంశం;

    curr_స్థానభ్రంశం += g_Main_పట్టిక.alias_List_కొలత;
    g_Main_పట్టిక.untaggedConvArray = పట్టిక + curr_స్థానభ్రంశం;

    curr_స్థానభ్రంశం += g_Main_పట్టిక.untagged_Conv_Array_కొలత;
    g_Main_పట్టిక.taggedAliasArray = పట్టిక + curr_స్థానభ్రంశం;

    /* aliasLists is a 1's based array, but it has a padding character */
    curr_స్థానభ్రంశం += g_Main_పట్టిక.tagged_Alias_Array_కొలత;
    g_Main_పట్టిక.taggedAliasLists = పట్టిక + curr_స్థానభ్రంశం;

    curr_స్థానభ్రంశం += g_Main_పట్టిక.tagged_Alias_Lists_కొలత;
    ఒకవేళ (g_Main_పట్టిక.option_పట్టిక_కొలత > 0
        && reinterpret_cast<స్థిరరాశి UConverterAliasOptions*>(పట్టిక + curr_స్థానభ్రంశం)->stringNormalizationType < UCNV_IO_NORM_TYPE_లెక్క)
    {
        /* Faster table */
        g_Main_పట్టిక.option_పట్టిక = reinterpret_cast<స్థిరరాశి UConverterAliasOptions*>(పట్టిక + curr_స్థానభ్రంశం);
    }
    లేకపోతే {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        g_Main_పట్టిక.option_పట్టిక = &default_పట్టిక_Options;
    }

    curr_స్థానభ్రంశం += g_Main_పట్టిక.option_పట్టిక_కొలత;
    g_Main_పట్టిక.string_పట్టిక = పట్టిక + curr_స్థానభ్రంశం;

    curr_స్థానభ్రంశం += g_Main_పట్టిక.string_పట్టిక_కొలత;
    g_Main_పట్టిక.normalized_String_పట్టిక = ((g_Main_పట్టిక.option_పట్టిక->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? g_Main_పట్టిక.string_పట్టిక : (పట్టిక + curr_స్థానభ్రంశం));

    తిరిగి;

invalidFormat:
    err_సంకేతం = U_INVALID_FORMAT_ERROR;
    udata_close(దత్తాంశం);
}


స్థిర UBool
have_Alias_దత్తాంశం(UError_సంకేతం *p_Error_సంకేతం) {
    umtx_initOnce(g_Alias_దత్తాంశం_Init_Once, &init_Alias_దత్తాంశం, *p_Error_సంకేతం);
    తిరిగి U_SUCCESS(*p_Error_సంకేతం);
}

స్థిర అంతఃవరుస UBool
isAlias(స్థిరరాశి అక్షరం *alias, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(alias==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి అబద్ధం;
    }
    తిరిగి *alias != 0;
}

స్థిర uint32_t get_ట్యాగ్_సంఖ్య(స్థిరరాశి అక్షరం *tagname) {
    ఒకవేళ (g_Main_పట్టిక.ట్యాగ్_List) {
        uint32_t ట్యాగ్_అంకె;
        కొరకు (ట్యాగ్_అంకె = 0; ట్యాగ్_అంకె < g_Main_పట్టిక.ట్యాగ్_List_కొలత; ట్యాగ్_అంకె++) {
            ఒకవేళ (!uprv_stricmp(GET_STRING(g_Main_పట్టిక.ట్యాగ్_List[ట్యాగ్_అంకె]), tagname)) {
                తిరిగి ట్యాగ్_అంకె;
            }
        }
    }

    తిరిగి UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
గణన {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
స్థిర స్థిరరాశి uint8_t asciiTypes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0
};

#నిర్వచించు GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

/* character types for EBCDIC 80..FF */
స్థిర స్థిరరాశి uint8_t ebcdicTypes[128] = {
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0
};

#నిర్వచించు GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#ఒకవేళ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#లేదావేళ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#లేకపోతే
#   error U_CHARSET_FAMILY is not valid
#అయితే_ముగింపు


/* @see ucnv_compareNames */
U_CAPI అక్షరం* U_CALLCONV
ucnv_io_stripASCIIForCompare(అక్షరం* dst U_LIFETIME_BOUND, స్థిరరాశి అక్షరం* name) {
    అక్షరం *dstItr = dst;
    uint8_t type, తదుపరి_Type;
    అక్షరం c1;
    UBool afterDigit = అబద్ధం;

    అయితే ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        ఎంపిక (type) {
        స్థితి UIGNORE:
            afterDigit = అబద్ధం;
            కొనసాగు; /* ignore all but letters and digits */
        స్థితి ZERO:
            ఒకవేళ (!afterDigit) {
                తదుపరి_Type = GET_ASCII_TYPE(*name);
                ఒకవేళ (తదుపరి_Type == ZERO || తదుపరి_Type == NONZERO) {
                    కొనసాగు; /* ignore leading zero before another digit */
                }
            }
            ఆపు;
        స్థితి NONZERO:
            afterDigit = నిజం;
            ఆపు;
        అప్రమేయ:
            c1 = (అక్షరం)type; /* lowercased letter */
            afterDigit = అబద్ధం;
            ఆపు;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    తిరిగి dst;
}

U_CAPI అక్షరం* U_CALLCONV
ucnv_io_stripEBCDICForCompare(అక్షరం* dst U_LIFETIME_BOUND, స్థిరరాశి అక్షరం* name) {
    అక్షరం *dstItr = dst;
    uint8_t type, తదుపరి_Type;
    అక్షరం c1;
    UBool afterDigit = అబద్ధం;

    అయితే ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        ఎంపిక (type) {
        స్థితి UIGNORE:
            afterDigit = అబద్ధం;
            కొనసాగు; /* ignore all but letters and digits */
        స్థితి ZERO:
            ఒకవేళ (!afterDigit) {
                తదుపరి_Type = GET_EBCDIC_TYPE(*name);
                ఒకవేళ (తదుపరి_Type == ZERO || తదుపరి_Type == NONZERO) {
                    కొనసాగు; /* ignore leading zero before another digit */
                }
            }
            ఆపు;
        స్థితి NONZERO:
            afterDigit = నిజం;
            ఆపు;
        అప్రమేయ:
            c1 = (అక్షరం)type; /* lowercased letter */
            afterDigit = అబద్ధం;
            ఆపు;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    తిరిగి dst;
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
U_CAPI పూర్ణాంకం U_EXPORT2
ucnv_compare_పేర్లు(స్థిరరాశి అక్షరం *name1, స్థిరరాశి అక్షరం *name2) {
    పూర్ణాంకం rc;
    uint8_t type, తదుపరి_Type;
    అక్షరం c1, c2;
    UBool afterDigit1 = అబద్ధం, afterDigit2 = అబద్ధం;

    కొరకు (;;) {
        అయితే ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            ఎంపిక (type) {
            స్థితి UIGNORE:
                afterDigit1 = అబద్ధం;
                కొనసాగు; /* ignore all but letters and digits */
            స్థితి ZERO:
                ఒకవేళ (!afterDigit1) {
                    తదుపరి_Type = GET_CHAR_TYPE(*name1);
                    ఒకవేళ (తదుపరి_Type == ZERO || తదుపరి_Type == NONZERO) {
                        కొనసాగు; /* ignore leading zero before another digit */
                    }
                }
                ఆపు;
            స్థితి NONZERO:
                afterDigit1 = నిజం;
                ఆపు;
            అప్రమేయ:
                c1 = (అక్షరం)type; /* lowercased letter */
                afterDigit1 = అబద్ధం;
                ఆపు;
            }
            ఆపు; /* deliver c1 */
        }
        అయితే ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            ఎంపిక (type) {
            స్థితి UIGNORE:
                afterDigit2 = అబద్ధం;
                కొనసాగు; /* ignore all but letters and digits */
            స్థితి ZERO:
                ఒకవేళ (!afterDigit2) {
                    తదుపరి_Type = GET_CHAR_TYPE(*name2);
                    ఒకవేళ (తదుపరి_Type == ZERO || తదుపరి_Type == NONZERO) {
                        కొనసాగు; /* ignore leading zero before another digit */
                    }
                }
                ఆపు;
            స్థితి NONZERO:
                afterDigit2 = నిజం;
                ఆపు;
            అప్రమేయ:
                c2 = (అక్షరం)type; /* lowercased letter */
                afterDigit2 = అబద్ధం;
                ఆపు;
            }
            ఆపు; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        ఒకవేళ ((c1|c2)==0) {
            తిరిగి 0;
        }

        /* Case-insensitive comparison */
        rc = (పూర్ణాంకం)(గుర్తులేని అక్షరం)c1 - (పూర్ణాంకం)(గుర్తులేని అక్షరం)c2;
        ఒకవేళ (rc != 0) {
            తిరిగి rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
స్థిర అంతఃవరుస uint32_t
కనుగొను_Converter(స్థిరరాశి అక్షరం *alias, UBool *కలిగియుంది_Option, UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t mid, ప్రారంభం, హద్దు;
    uint32_t చివరి_Mid;
    పూర్ణాంకం ఫలితం;
    పూర్ణాంకం isUnnormalized = (g_Main_పట్టిక.option_పట్టిక->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    అక్షరం strippedName[UCNV_MAX_CONVERTER_NAME_నిడివి];

    ఒకవేళ (!isUnnormalized) {
        ఒకవేళ (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_నిడివి) {
            *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
            తిరిగి UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    ప్రారంభం = 0;
    హద్దు = g_Main_పట్టిక.untagged_Conv_Array_కొలత;
    mid = హద్దు;
    చివరి_Mid = UINT32_MAX;

    కొరకు (;;) {
        mid = (ప్రారంభం + హద్దు) / 2;
        ఒకవేళ (చివరి_Mid == mid) {   /* Have we moved? */
            ఆపు;  /* We haven't moved, and it wasn't found. */
        }
        చివరి_Mid = mid;
        ఒకవేళ (isUnnormalized) {
            ఫలితం = ucnv_compare_పేర్లు(alias, GET_STRING(g_Main_పట్టిక.aliasList[mid]));
        }
        లేకపోతే {
            ఫలితం = uprv_strcmp(alias, GET_NORMALIZED_STRING(g_Main_పట్టిక.aliasList[mid]));
        }

        ఒకవేళ (ఫలితం < 0) {
            హద్దు = mid;
        } లేకపోతే ఒకవేళ (ఫలితం > 0) {
            ప్రారంభం = mid;
        } లేకపోతే {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            ఒకవేళ (g_Main_పట్టిక.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_బిట్టు) {
                *p_Error_సంకేతం = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            ఒకవేళ (కలిగియుంది_Option) {
                UBool కలిగియుంది_Cnv_Option_Info = static_cast<UBool>(g_Main_పట్టిక.option_పట్టిక->కలిగియుంది_Cnv_Option_Info);
                *కలిగియుంది_Option = static_cast<UBool>((కలిగియుంది_Cnv_Option_Info
                    && ((g_Main_పట్టిక.untaggedConvArray[mid] & UCNV_కలిగియుంది_OPTION_బిట్టు) != 0))
                    || !కలిగియుంది_Cnv_Option_Info);
            }
            తిరిగి g_Main_పట్టిక.untaggedConvArray[mid] & UCNV_CONVERTER_సూచీ_ముసుగు;
        }
    }

    తిరిగి UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
స్థిర అంతఃవరుస UBool
is_Alias_లోపల_List(స్థిరరాశి అక్షరం *alias, uint32_t list_స్థానభ్రంశం) {
    ఒకవేళ (list_స్థానభ్రంశం) {
        uint32_t currAlias;
        uint32_t list_లెక్క = g_Main_పట్టిక.taggedAliasLists[list_స్థానభ్రంశం];
        /* +1 to skip listCount */
        స్థిరరాశి uint16_t *currList = g_Main_పట్టిక.taggedAliasLists + list_స్థానభ్రంశం + 1;
        కొరకు (currAlias = 0; currAlias < list_లెక్క; currAlias++) {
            ఒకవేళ (currList[currAlias]
                && ucnv_compare_పేర్లు(alias, GET_STRING(currList[currAlias]))==0)
            {
                తిరిగి నిజం;
            }
        }
    }
    తిరిగి అబద్ధం;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
స్థిర uint32_t
కనుగొను_Tagged_Alias_Lists_స్థానభ్రంశం(స్థిరరాశి అక్షరం *alias, స్థిరరాశి అక్షరం *standard, UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t idx;
    uint32_t list_స్థానభ్రంశం;
    uint32_t conv_అంకె;
    UError_సంకేతం myErr = U_ZERO_ERROR;
    uint32_t ట్యాగ్_అంకె = get_ట్యాగ్_సంఖ్య(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_అంకె = కనుగొను_Converter(alias, శూన్యసూచిక, &myErr);
    ఒకవేళ (myErr != U_ZERO_ERROR) {
        *p_Error_సంకేతం = myErr;
    }

    ఒకవేళ (ట్యాగ్_అంకె < (g_Main_పట్టిక.ట్యాగ్_List_కొలత - UCNV_అంకె_HIDDEN_ట్యాగులు) && conv_అంకె < g_Main_పట్టిక.converter_List_కొలత) {
        list_స్థానభ్రంశం = g_Main_పట్టిక.taggedAliasArray[ట్యాగ్_అంకె*g_Main_పట్టిక.converter_List_కొలత + conv_అంకె];
        ఒకవేళ (list_స్థానభ్రంశం && g_Main_పట్టిక.taggedAliasLists[list_స్థానభ్రంశం + 1]) {
            తిరిగి list_స్థానభ్రంశం;
        }
        ఒకవేళ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            కొరకు (idx = 0; idx < g_Main_పట్టిక.tagged_Alias_Array_కొలత; idx++) {
                list_స్థానభ్రంశం = g_Main_పట్టిక.taggedAliasArray[idx];
                ఒకవేళ (list_స్థానభ్రంశం && is_Alias_లోపల_List(alias, list_స్థానభ్రంశం)) {
                    uint32_t curr_ట్యాగ్_అంకె = idx/g_Main_పట్టిక.converter_List_కొలత;
                    uint32_t curr_Conv_అంకె = (idx - curr_ట్యాగ్_అంకె*g_Main_పట్టిక.converter_List_కొలత);
                    uint32_t తాత్కాలిక_List_స్థానభ్రంశం = g_Main_పట్టిక.taggedAliasArray[ట్యాగ్_అంకె*g_Main_పట్టిక.converter_List_కొలత + curr_Conv_అంకె];
                    ఒకవేళ (తాత్కాలిక_List_స్థానభ్రంశం && g_Main_పట్టిక.taggedAliasLists[తాత్కాలిక_List_స్థానభ్రంశం + 1]) {
                        తిరిగి తాత్కాలిక_List_స్థానభ్రంశం;
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
        తిరిగి 0;
    }
    /* else converter or tag not found */

    తిరిగి UINT32_MAX;
}

/* Return the canonical name */
స్థిర uint32_t
కనుగొను_Tagged_Converter_అంకె(స్థిరరాశి అక్షరం *alias, స్థిరరాశి అక్షరం *standard, UError_సంకేతం *p_Error_సంకేతం) {
    uint32_t idx;
    uint32_t list_స్థానభ్రంశం;
    uint32_t conv_అంకె;
    UError_సంకేతం myErr = U_ZERO_ERROR;
    uint32_t ట్యాగ్_అంకె = get_ట్యాగ్_సంఖ్య(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_అంకె = కనుగొను_Converter(alias, శూన్యసూచిక, &myErr);
    ఒకవేళ (myErr != U_ZERO_ERROR) {
        *p_Error_సంకేతం = myErr;
    }

    ఒకవేళ (ట్యాగ్_అంకె < (g_Main_పట్టిక.ట్యాగ్_List_కొలత - UCNV_అంకె_HIDDEN_ట్యాగులు) && conv_అంకె < g_Main_పట్టిక.converter_List_కొలత) {
        list_స్థానభ్రంశం = g_Main_పట్టిక.taggedAliasArray[ట్యాగ్_అంకె*g_Main_పట్టిక.converter_List_కొలత + conv_అంకె];
        ఒకవేళ (list_స్థానభ్రంశం && is_Alias_లోపల_List(alias, list_స్థానభ్రంశం)) {
            తిరిగి conv_అంకె;
        }
        ఒకవేళ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t conv_ప్రారంభం = (ట్యాగ్_అంకె)*g_Main_పట్టిక.converter_List_కొలత;
            uint32_t conv_హద్దు = (ట్యాగ్_అంకె+1)*g_Main_పట్టిక.converter_List_కొలత;
            కొరకు (idx = conv_ప్రారంభం; idx < conv_హద్దు; idx++) {
                list_స్థానభ్రంశం = g_Main_పట్టిక.taggedAliasArray[idx];
                ఒకవేళ (list_స్థానభ్రంశం && is_Alias_లోపల_List(alias, list_స్థానభ్రంశం)) {
                    తిరిగి idx-conv_ప్రారంభం;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    తిరిగి UINT32_MAX;
}

U_CAPI స్థిరరాశి అక్షరం *
ucnv_io_getConverterName(స్థిరరాశి అక్షరం *alias, UBool *కలిగియుంది_Option, UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి అక్షరం *aliasTmp = alias;
    int32_t i = 0;
    కొరకు (i = 0; i < 2; i++) {
        ఒకవేళ (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            ఒకవేళ (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } లేకపోతే {
                ఆపు;
            }
        }
        ఒకవేళ(have_Alias_దత్తాంశం(p_Error_సంకేతం) && isAlias(aliasTmp, p_Error_సంకేతం)) {
            uint32_t conv_అంకె = కనుగొను_Converter(aliasTmp, కలిగియుంది_Option, p_Error_సంకేతం);
            ఒకవేళ (conv_అంకె < g_Main_పట్టిక.converter_List_కొలత) {
                తిరిగి GET_STRING(g_Main_పట్టిక.converterList[conv_అంకె]);
            }
            /* else converter not found */
        } లేకపోతే {
            ఆపు;
        }
    }

    తిరిగి శూన్యసూచిక;
}

U_CDECL_BEGIN


స్థిర int32_t U_CALLCONV
ucnv_io_లెక్క_Standard_Aliases(UEnumeration *enumerator, UError_సంకేతం * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_స్థానభ్రంశం = myContext->list_స్థానభ్రంశం;

    ఒకవేళ (list_స్థానభ్రంశం) {
        value = g_Main_పట్టిక.taggedAliasLists[list_స్థానభ్రంశం];
    }
    తిరిగి value;
}

స్థిర స్థిరరాశి అక్షరం * U_CALLCONV
ucnv_io_తదుపరి_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* ఫలితం_నిడివి,
                            UError_సంకేతం * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_స్థానభ్రంశం = myContext->list_స్థానభ్రంశం;

    ఒకవేళ (list_స్థానభ్రంశం) {
        uint32_t list_లెక్క = g_Main_పట్టిక.taggedAliasLists[list_స్థానభ్రంశం];
        స్థిరరాశి uint16_t *currList = g_Main_పట్టిక.taggedAliasLists + list_స్థానభ్రంశం + 1;

        ఒకవేళ (myContext->listIdx < list_లెక్క) {
            స్థిరరాశి అక్షరం *myStr = GET_STRING(currList[myContext->listIdx++]);
            ఒకవేళ (ఫలితం_నిడివి) {
                *ఫలితం_నిడివి = (int32_t)uprv_strlen(myStr);
            }
            తిరిగి myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ఒకవేళ (ఫలితం_నిడివి) {
        *ఫలితం_నిడివి = 0;
    }
    తిరిగి శూన్యసూచిక;
}

స్థిర శూన్య U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UError_సంకేతం * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

స్థిర శూన్య U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_చివర

/* Enumerate the aliases for the specified converter and standard tag */
స్థిర స్థిరరాశి UEnumeration gEnumAliases = {
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_io_closeUEnumeration,
    ucnv_io_లెక్క_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_తదుపరి_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_Standard_పేర్లు(స్థిరరాశి అక్షరం *convName,
                       స్థిరరాశి అక్షరం *standard,
                       UError_సంకేతం *p_Error_సంకేతం)
{
    UEnumeration *myEnum = శూన్యసూచిక;
    ఒకవేళ (have_Alias_దత్తాంశం(p_Error_సంకేతం) && isAlias(convName, p_Error_సంకేతం)) {
        uint32_t list_స్థానభ్రంశం = కనుగొను_Tagged_Alias_Lists_స్థానభ్రంశం(convName, standard, p_Error_సంకేతం);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        ఒకవేళ (list_స్థానభ్రంశం < g_Main_పట్టిక.tagged_Alias_Lists_కొలత) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(పరిమాణం(UEnumeration)));
            ఒకవేళ (myEnum == శూన్యసూచిక) {
                *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి శూన్యసూచిక;
            }
            uprv_memcpy(myEnum, &gEnumAliases, పరిమాణం(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(పరిమాణం(UAliasContext)));
            ఒకవేళ (myContext == శూన్యసూచిక) {
                *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                uprv_free(myEnum);
                తిరిగి శూన్యసూచిక;
            }
            myContext->list_స్థానభ్రంశం = list_స్థానభ్రంశం;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    తిరిగి myEnum;
}

స్థిర uint16_t
ucnv_io_లెక్క_Aliases(స్థిరరాశి అక్షరం *alias, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(have_Alias_దత్తాంశం(p_Error_సంకేతం) && isAlias(alias, p_Error_సంకేతం)) {
        uint32_t conv_అంకె = కనుగొను_Converter(alias, శూన్యసూచిక, p_Error_సంకేతం);
        ఒకవేళ (conv_అంకె < g_Main_పట్టిక.converter_List_కొలత) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_స్థానభ్రంశం = g_Main_పట్టిక.taggedAliasArray[(g_Main_పట్టిక.ట్యాగ్_List_కొలత - 1)*g_Main_పట్టిక.converter_List_కొలత + conv_అంకె];

            ఒకవేళ (list_స్థానభ్రంశం) {
                తిరిగి g_Main_పట్టిక.taggedAliasLists[list_స్థానభ్రంశం];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    తిరిగి 0;
}

స్థిర uint16_t
ucnv_io_getAliases(స్థిరరాశి అక్షరం *alias, uint16_t ప్రారంభం, స్థిరరాశి అక్షరం **aliases, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(have_Alias_దత్తాంశం(p_Error_సంకేతం) && isAlias(alias, p_Error_సంకేతం)) {
        uint32_t currAlias;
        uint32_t conv_అంకె = కనుగొను_Converter(alias, శూన్యసూచిక, p_Error_సంకేతం);
        ఒకవేళ (conv_అంకె < g_Main_పట్టిక.converter_List_కొలత) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_స్థానభ్రంశం = g_Main_పట్టిక.taggedAliasArray[(g_Main_పట్టిక.ట్యాగ్_List_కొలత - 1)*g_Main_పట్టిక.converter_List_కొలత + conv_అంకె];

            ఒకవేళ (list_స్థానభ్రంశం) {
                uint32_t list_లెక్క = g_Main_పట్టిక.taggedAliasLists[list_స్థానభ్రంశం];
                /* +1 to skip listCount */
                స్థిరరాశి uint16_t *currList = g_Main_పట్టిక.taggedAliasLists + list_స్థానభ్రంశం + 1;

                కొరకు (currAlias = ప్రారంభం; currAlias < list_లెక్క; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    తిరిగి 0;
}

స్థిర స్థిరరాశి అక్షరం *
ucnv_io_getAlias(స్థిరరాశి అక్షరం *alias, uint16_t n, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(have_Alias_దత్తాంశం(p_Error_సంకేతం) && isAlias(alias, p_Error_సంకేతం)) {
        uint32_t conv_అంకె = కనుగొను_Converter(alias, శూన్యసూచిక, p_Error_సంకేతం);
        ఒకవేళ (conv_అంకె < g_Main_పట్టిక.converter_List_కొలత) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_స్థానభ్రంశం = g_Main_పట్టిక.taggedAliasArray[(g_Main_పట్టిక.ట్యాగ్_List_కొలత - 1)*g_Main_పట్టిక.converter_List_కొలత + conv_అంకె];

            ఒకవేళ (list_స్థానభ్రంశం) {
                uint32_t list_లెక్క = g_Main_పట్టిక.taggedAliasLists[list_స్థానభ్రంశం];
                /* +1 to skip listCount */
                స్థిరరాశి uint16_t *currList = g_Main_పట్టిక.taggedAliasLists + list_స్థానభ్రంశం + 1;

                ఒకవేళ (n < list_లెక్క)  {
                    తిరిగి GET_STRING(currList[n]);
                }
                *p_Error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    తిరిగి శూన్యసూచిక;
}

స్థిర uint16_t
ucnv_io_లెక్క_Standards(UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (have_Alias_దత్తాంశం(p_Error_సంకేతం)) {
        /* Don't include the empty list */
        తిరిగి static_cast<uint16_t>(g_Main_పట్టిక.ట్యాగ్_List_కొలత - UCNV_అంకె_HIDDEN_ట్యాగులు);
    }

    తిరిగి 0;
}

U_CAPI స్థిరరాశి అక్షరం * U_EXPORT2
ucnv_getStandard(uint16_t n, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (have_Alias_దత్తాంశం(p_Error_సంకేతం)) {
        ఒకవేళ (n < g_Main_పట్టిక.ట్యాగ్_List_కొలత - UCNV_అంకె_HIDDEN_ట్యాగులు) {
            తిరిగి GET_STRING(g_Main_పట్టిక.ట్యాగ్_List[n]);
        }
        *p_Error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
    }

    తిరిగి శూన్యసూచిక;
}

U_CAPI స్థిరరాశి అక్షరం * U_EXPORT2
ucnv_getStandardName(స్థిరరాశి అక్షరం *alias, స్థిరరాశి అక్షరం *standard, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (have_Alias_దత్తాంశం(p_Error_సంకేతం) && isAlias(alias, p_Error_సంకేతం)) {
        uint32_t list_స్థానభ్రంశం = కనుగొను_Tagged_Alias_Lists_స్థానభ్రంశం(alias, standard, p_Error_సంకేతం);

        ఒకవేళ (0 < list_స్థానభ్రంశం && list_స్థానభ్రంశం < g_Main_పట్టిక.tagged_Alias_Lists_కొలత) {
            స్థిరరాశి uint16_t *currList = g_Main_పట్టిక.taggedAliasLists + list_స్థానభ్రంశం + 1;

            /* Get the preferred name from this list */
            ఒకవేళ (currList[0]) {
                తిరిగి GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    తిరిగి శూన్యసూచిక;
}

U_CAPI uint16_t U_EXPORT2
ucnv_లెక్క_Aliases(స్థిరరాశి అక్షరం *alias, UError_సంకేతం *p_Error_సంకేతం)
{
    తిరిగి ucnv_io_లెక్క_Aliases(alias, p_Error_సంకేతం);
}


U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
ucnv_getAlias(స్థిరరాశి అక్షరం *alias, uint16_t n, UError_సంకేతం *p_Error_సంకేతం)
{
    తిరిగి ucnv_io_getAlias(alias, n, p_Error_సంకేతం);
}

U_CAPI శూన్య U_EXPORT2
ucnv_getAliases(స్థిరరాశి అక్షరం *alias, స్థిరరాశి అక్షరం **aliases, UError_సంకేతం *p_Error_సంకేతం)
{
    ucnv_io_getAliases(alias, 0, aliases, p_Error_సంకేతం);
}

U_CAPI uint16_t U_EXPORT2
ucnv_లెక్క_Standards()
{
    UError_సంకేతం err = U_ZERO_ERROR;
    తిరిగి ucnv_io_లెక్క_Standards(&err);
}

U_CAPI స్థిరరాశి అక్షరం * U_EXPORT2
ucnv_getCanonicalName(స్థిరరాశి అక్షరం *alias, స్థిరరాశి అక్షరం *standard, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (have_Alias_దత్తాంశం(p_Error_సంకేతం) && isAlias(alias, p_Error_సంకేతం)) {
        uint32_t conv_అంకె = కనుగొను_Tagged_Converter_అంకె(alias, standard, p_Error_సంకేతం);

        ఒకవేళ (conv_అంకె < g_Main_పట్టిక.converter_List_కొలత) {
            తిరిగి GET_STRING(g_Main_పట్టిక.converterList[conv_అంకె]);
        }
    }

    తిరిగి శూన్యసూచిక;
}

U_CDECL_BEGIN


స్థిర int32_t U_CALLCONV
ucnv_io_లెక్క_All_Converters(UEnumeration * /*enumerator*/, UError_సంకేతం * /*pErrorCode*/) {
    తిరిగి g_Main_పట్టిక.converter_List_కొలత;
}

స్థిర స్థిరరాశి అక్షరం * U_CALLCONV
ucnv_io_తదుపరి_All_Converters(UEnumeration *enumerator,
                            int32_t* ఫలితం_నిడివి,
                            UError_సంకేతం * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    ఒకవేళ (*myContext < g_Main_పట్టిక.converter_List_కొలత) {
        స్థిరరాశి అక్షరం *myStr = GET_STRING(g_Main_పట్టిక.converterList[(*myContext)++]);
        ఒకవేళ (ఫలితం_నిడివి) {
            *ఫలితం_నిడివి = (int32_t)uprv_strlen(myStr);
        }
        తిరిగి myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ఒకవేళ (ఫలితం_నిడివి) {
        *ఫలితం_నిడివి = 0;
    }
    తిరిగి శూన్యసూచిక;
}

స్థిర శూన్య U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UError_సంకేతం * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_చివర
స్థిర స్థిరరాశి UEnumeration gEnumAllConverters = {
    శూన్యసూచిక,
    శూన్యసూచిక,
    ucnv_io_closeUEnumeration,
    ucnv_io_లెక్క_All_Converters,
    uenum_unextDefault,
    ucnv_io_తదుపరి_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_All_పేర్లు(UError_సంకేతం *p_Error_సంకేతం) {
    UEnumeration *myEnum = శూన్యసూచిక;
    ఒకవేళ (have_Alias_దత్తాంశం(p_Error_సంకేతం)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(పరిమాణం(UEnumeration)));
        ఒకవేళ (myEnum == శూన్యసూచిక) {
            *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, పరిమాణం(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(పరిమాణం(uint16_t)));
        ఒకవేళ (myContext == శూన్యసూచిక) {
            *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            uprv_free(myEnum);
            తిరిగి శూన్యసూచిక;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    తిరిగి myEnum;
}

U_CAPI uint16_t
ucnv_io_లెక్క_తెలిసిన_Converters(UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ (have_Alias_దత్తాంశం(p_Error_సంకేతం)) {
        తిరిగి (uint16_t)g_Main_పట్టిక.converter_List_కొలత;
    }
    తిరిగి 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

రకనిర్వచనం అక్షరం * U_CALLCONV StripForCompareFn(అక్షరం *dst, స్థిరరాశి అక్షరం *name);
U_CDECL_చివర


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
రకనిర్వచనం నిర్మాణం తాత్కాలిక_అడ్డువరుస {
    uint16_t str_సూచీ, క్రమీకరించు_సూచీ;
} తాత్కాలిక_అడ్డువరుస;

రకనిర్వచనం నిర్మాణం తాత్కాలిక_Alias_పట్టిక {
    స్థిరరాశి అక్షరం *chars;
    తాత్కాలిక_అడ్డువరుస *అడ్డువరుసలు;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} తాత్కాలిక_Alias_పట్టిక;

గణన {
    దొంతర_అడ్డువరుస_సామర్థ్యం=500
};

స్థిర int32_t U_CALLCONV
io_compare_అడ్డువరుసలు(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    అక్షరం stripped_ఎడమ[UCNV_MAX_CONVERTER_NAME_నిడివి],
         stripped_కుడి[UCNV_MAX_CONVERTER_NAME_నిడివి];

    తాత్కాలిక_Alias_పట్టిక *తాత్కాలిక_పట్టిక=(తాత్కాలిక_Alias_పట్టిక *)context;
    స్థిరరాశి అక్షరం *chars=తాత్కాలిక_పట్టిక->chars;

    తిరిగి static_cast<int32_t>(uprv_strcmp(
        తాత్కాలిక_పట్టిక->stripForCompare(stripped_ఎడమ, chars + 2 * static_cast<స్థిరరాశి తాత్కాలిక_అడ్డువరుస*>(ఎడమ)->str_సూచీ),
        తాత్కాలిక_పట్టిక->stripForCompare(stripped_కుడి, chars + 2 * static_cast<స్థిరరాశి తాత్కాలిక_అడ్డువరుస*>(కుడి)->str_సూచీ)));
}

U_CAPI int32_t U_EXPORT2
ucnv_మార్పిడి_Aliases(స్థిరరాశి UDataSwapper *ds,
                 స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                 UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UDataInfo *pInfo;
    int32_t శీర్షిక_కొలత;

    స్థిరరాశి uint16_t *లోపల_పట్టిక;
    స్థిరరాశి uint32_t *లోపల_Section_Sizes;
    uint32_t toc[offsets_లెక్క];
    uint32_t offsets[offsets_లెక్క]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_నిడివి, శిఖరం_స్థానభ్రంశం;

    తాత్కాలిక_అడ్డువరుస అడ్డువరుసలు[దొంతర_అడ్డువరుస_సామర్థ్యం];
    uint16_t resort[దొంతర_అడ్డువరుస_సామర్థ్యం];
    తాత్కాలిక_Alias_పట్టిక తాత్కాలిక_పట్టిక;

    /* udata_swapDataHeader checks the arguments */
    శీర్షిక_కొలత=udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* check data format and format version */
    pInfo=(స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
    ఒకవేళ(!(
        pInfo->దత్తాంశం_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->దత్తాంశం_Format[1]==0x76 &&
        pInfo->దత్తాంశం_Format[2]==0x41 &&
        pInfo->దత్తాంశం_Format[3]==0x6c &&
        pInfo->format_సంస్కరణ[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1],
                         pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3],
                         pInfo->format_సంస్కరణ[0]);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    /* an alias table must contain at least the table of contents array */
    ఒకవేళ(length>=0 && (length-శీర్షిక_కొలత)<4*(1+min_Toc_నిడివి)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-శీర్షిక_కొలత);
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }

    లోపల_Section_Sizes=(స్థిరరాశి uint32_t *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+శీర్షిక_కొలత);
    లోపల_పట్టిక=(స్థిరరాశి uint16_t *)లోపల_Section_Sizes;
    uprv_memset(toc, 0, పరిమాణం(toc));
    toc[toc_నిడివి_సూచీ]=toc_నిడివి=ds->readUInt32(లోపల_Section_Sizes[toc_నిడివి_సూచీ]);
    ఒకవేళ(toc_నిడివి<min_Toc_నిడివి || offsets_లెక్క<=toc_నిడివి) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_నిడివి);
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి 0;
    }

    /* read the known part of the table of contents */
    కొరకు(i=converter_List_సూచీ; i<=toc_నిడివి; ++i) {
        toc[i]=ds->readUInt32(లోపల_Section_Sizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, పరిమాణం(offsets));
    offsets[converter_List_సూచీ]=2*(1+toc_నిడివి); /* count two 16-bit units per toc entry */
    కొరకు(i=ట్యాగ్_List_సూచీ; i<=toc_నిడివి; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    శిఖరం_స్థానభ్రంశం=offsets[i-1]+toc[i-1];

    ఒకవేళ(length>=0) {
        uint16_t *బహిర్గతం_పట్టిక;
        స్థిరరాశి uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_సూచీ;

        ఒకవేళ((length-శీర్షిక_కొలత)<(2*(int32_t)శిఖరం_స్థానభ్రంశం)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-శీర్షిక_కొలత);
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }

        బహిర్గతం_పట్టిక=(uint16_t *)((అక్షరం *)బహిర్గతం_దత్తాంశం+శీర్షిక_కొలత);

        /* swap the entire table of contents */
        ds->మార్పిడి_Array32(ds, లోపల_పట్టిక, 4*(1+toc_నిడివి), బహిర్గతం_పట్టిక, p_Error_సంకేతం);

        /* swap unormalized strings & normalized strings */
        ds->మార్పిడి_Inv_Chars(ds, లోపల_పట్టిక+offsets[string_పట్టిక_సూచీ], 2*(int32_t)(toc[string_పట్టిక_సూచీ]+toc[normalized_String_పట్టిక_సూచీ]),
                             బహిర్గతం_పట్టిక+offsets[string_పట్టిక_సూచీ], p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            తిరిగి 0;
        }

        ఒకవేళ(ds->లోపల_Charset==ds->బహిర్గతం_Charset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->మార్పిడి_Array16(ds,
                            లోపల_పట్టిక+offsets[converter_List_సూచీ],
                            2*(int32_t)(offsets[string_పట్టిక_సూచీ]-offsets[converter_List_సూచీ]),
                            బహిర్గతం_పట్టిక+offsets[converter_List_సూచీ],
                            p_Error_సంకేతం);
        } లేకపోతే {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_సూచీ];

            తాత్కాలిక_పట్టిక.chars=(స్థిరరాశి అక్షరం *)(బహిర్గతం_పట్టిక+offsets[string_పట్టిక_సూచీ]); /* sort by outCharset */

            ఒకవేళ(count<=దొంతర_అడ్డువరుస_సామర్థ్యం) {
                తాత్కాలిక_పట్టిక.అడ్డువరుసలు=అడ్డువరుసలు;
                తాత్కాలిక_పట్టిక.resort=resort;
            } లేకపోతే {
                తాత్కాలిక_పట్టిక.అడ్డువరుసలు=(తాత్కాలిక_అడ్డువరుస *)uprv_malloc(count*పరిమాణం(తాత్కాలిక_అడ్డువరుస)+count*2);
                ఒకవేళ(తాత్కాలిక_పట్టిక.అడ్డువరుసలు==శూన్యసూచిక) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    తిరిగి 0;
                }
                తాత్కాలిక_పట్టిక.resort=(uint16_t *)(తాత్కాలిక_పట్టిక.అడ్డువరుసలు+count);
            }

            ఒకవేళ(ds->బహిర్గతం_Charset==U_ASCII_FAMILY) {
                తాత్కాలిక_పట్టిక.stripForCompare=ucnv_io_stripASCIIForCompare;
            } లేకపోతే /* U_EBCDIC_FAMILY */ {
                తాత్కాలిక_పట్టిక.stripForCompare=ucnv_io_stripEBCDICForCompare;
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
            p=లోపల_పట్టిక+offsets[alias_List_సూచీ];
            q=బహిర్గతం_పట్టిక+offsets[alias_List_సూచీ];

            p2=లోపల_పట్టిక+offsets[untagged_Conv_Array_సూచీ];
            q2=బహిర్గతం_పట్టిక+offsets[untagged_Conv_Array_సూచీ];

            కొరకు(i=0; i<count; ++i) {
                తాత్కాలిక_పట్టిక.అడ్డువరుసలు[i].str_సూచీ=ds->readUInt16(p[i]);
                తాత్కాలిక_పట్టిక.అడ్డువరుసలు[i].క్రమీకరించు_సూచీ=(uint16_t)i;
            }

            uprv_క్రమీకరించు_Array(తాత్కాలిక_పట్టిక.అడ్డువరుసలు, (int32_t)count, పరిమాణం(తాత్కాలిక_అడ్డువరుస),
                           io_compare_అడ్డువరుసలు, &తాత్కాలిక_పట్టిక,
                           అబద్ధం, p_Error_సంకేతం);

            ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
                /* copy/swap/permutate items */
                ఒకవేళ(p!=q) {
                    కొరకు(i=0; i<count; ++i) {
                        old_సూచీ=తాత్కాలిక_పట్టిక.అడ్డువరుసలు[i].క్రమీకరించు_సూచీ;
                        ds->మార్పిడి_Array16(ds, p+old_సూచీ, 2, q+i, p_Error_సంకేతం);
                        ds->మార్పిడి_Array16(ds, p2+old_సూచీ, 2, q2+i, p_Error_సంకేతం);
                    }
                } లేకపోతే {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=తాత్కాలిక_పట్టిక.resort;

                    కొరకు(i=0; i<count; ++i) {
                        old_సూచీ=తాత్కాలిక_పట్టిక.అడ్డువరుసలు[i].క్రమీకరించు_సూచీ;
                        ds->మార్పిడి_Array16(ds, p+old_సూచీ, 2, r+i, p_Error_సంకేతం);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    కొరకు(i=0; i<count; ++i) {
                        old_సూచీ=తాత్కాలిక_పట్టిక.అడ్డువరుసలు[i].క్రమీకరించు_సూచీ;
                        ds->మార్పిడి_Array16(ds, p2+old_సూచీ, 2, r+i, p_Error_సంకేతం);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            ఒకవేళ(తాత్కాలిక_పట్టిక.అడ్డువరుసలు!=అడ్డువరుసలు) {
                uprv_free(తాత్కాలిక_పట్టిక.అడ్డువరుసలు);
            }

            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                తిరిగి 0;
            }

            /* swap remaining 16-bit values */
            ds->మార్పిడి_Array16(ds,
                            లోపల_పట్టిక+offsets[converter_List_సూచీ],
                            2*(int32_t)(offsets[alias_List_సూచీ]-offsets[converter_List_సూచీ]),
                            బహిర్గతం_పట్టిక+offsets[converter_List_సూచీ],
                            p_Error_సంకేతం);
            ds->మార్పిడి_Array16(ds,
                            లోపల_పట్టిక+offsets[tagged_Alias_Array_సూచీ],
                            2*(int32_t)(offsets[string_పట్టిక_సూచీ]-offsets[tagged_Alias_Array_సూచీ]),
                            బహిర్గతం_పట్టిక+offsets[tagged_Alias_Array_సూచీ],
                            p_Error_సంకేతం);
        }
    }

    తిరిగి శీర్షిక_కొలత+2*(int32_t)శిఖరం_స్థానభ్రంశం;
}

#అయితే_ముగింపు


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

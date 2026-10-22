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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udatamem.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_io.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uenumimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UAliasContext {
    uint32_t list_ਵਿੱਥ;
    uint32_t listIdx;
} UAliasContext;

ਸਥਿਰ ਅਚਲ ਅੱਖਰ ਅੰਕੜੇ_NAME[] = "cnvalias";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ ਅੰਕੜੇ_TYPE[] = "icu";

ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *g_Alias_ਅੰਕੜੇ=ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::UInitOnce g_Alias_ਅੰਕੜੇ_Init_Once {};

ਗਿਣਤੀ {
    toc_ਲੰਮਾਈ_ਸੂਚਕਾਂਕ=0,
    converter_List_ਸੂਚਕਾਂਕ=1,
    ਟੈਗ_List_ਸੂਚਕਾਂਕ=2,
    alias_List_ਸੂਚਕਾਂਕ=3,
    untagged_Conv_Array_ਸੂਚਕਾਂਕ=4,
    tagged_Alias_Array_ਸੂਚਕਾਂਕ=5,
    tagged_Alias_Lists_ਸੂਚਕਾਂਕ=6,
    ਸਾਰਣੀ_Options_ਸੂਚਕਾਂਕ=7,
    string_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ=8,
    normalized_String_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ=9,
    offsets_ਗਿਣਤੀ,    /* length of the swapper's temporary offsets[] */
    min_Toc_ਲੰਮਾਈ=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

ਸਥਿਰ ਅਚਲ UConverterAliasOptions default_ਸਾਰਣੀ_Options = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
ਸਥਿਰ UConverterAlias g_Main_ਸਾਰਣੀ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_STRING(idx) (ਅਚਲ ਅੱਖਰ *)(g_Main_ਸਾਰਣੀ.string_ਸਾਰਣੀ + (idx))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_NORMALIZED_STRING(idx) (ਅਚਲ ਅੱਖਰ *)(g_Main_ਸਾਰਣੀ.normalized_String_ਸਾਰਣੀ + (idx))

ਸਥਿਰ UBool U_CALLCONV
isAcceptable(ਖਾਲੀ * /*context*/,
             ਅਚਲ ਅੱਖਰ * /*type*/, ਅਚਲ ਅੱਖਰ * /*name*/,
             ਅਚਲ UDataInfo *pInfo) {
    ਵਾਪਸ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ਅੰਕੜੇ_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x76 &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x41 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x6c &&
        pInfo->format_ਸੰਸਕਰਣ[0]==3;
}

ਸਥਿਰ UBool U_CALLCONV ucnv_io_cleanup()
{
    ਜੇ (g_Alias_ਅੰਕੜੇ) {
        udata_close(g_Alias_ਅੰਕੜੇ);
        g_Alias_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
    }
    g_Alias_ਅੰਕੜੇ_Init_Once.reset();

    uprv_memset(&g_Main_ਸਾਰਣੀ, 0, ਆਕਾਰ(g_Main_ਸਾਰਣੀ));

    ਵਾਪਸ ਸੱਚ;                   /* Everything was cleaned up */
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV init_Alias_ਅੰਕੜੇ(UError_ਕੋਡ &err_ਕੋਡ) {
    UData_ਸਮ੍ਰਿਤੀ *ਅੰਕੜੇ;
    ਅਚਲ uint16_t *ਸਾਰਣੀ;
    ਅਚਲ uint32_t *sectionSizes;
    uint32_t ਸਾਰਣੀ_ਸ਼ੁਰੂ;
    uint32_t curr_ਵਿੱਥ;
    int32_t ਮਾਪ_ਦਾ_ਅੰਕੜੇ;
    int32_t ਮਾਪ_ਦਾ_TOC;

    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ);
    ਅੰਕੜੇ = udata_openChoice(ਖਾਲੀ_ਸੂਚਕ, ਅੰਕੜੇ_TYPE, ਅੰਕੜੇ_NAME, isAcceptable, ਖਾਲੀ_ਸੂਚਕ, &err_ਕੋਡ);
    ਜੇ (U_FAILURE(err_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    sectionSizes = static_cast<ਅਚਲ uint32_t*>(udata_get_ਸਮ੍ਰਿਤੀ(ਅੰਕੜੇ));
    int32_t ਅੰਕੜੇ_ਲੰਮਾਈ = udata_get_ਲੰਮਾਈ(ਅੰਕੜੇ); // This is the length minus the UDataInfo size
    UBool is_ਅੰਕੜੇ_ਲੰਮਾਈ_ਗਿਆਤ = ਅੰਕੜੇ_ਲੰਮਾਈ >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    ਜੇ (is_ਅੰਕੜੇ_ਲੰਮਾਈ_ਗਿਆਤ && ਅੰਕੜੇ_ਲੰਮਾਈ <= int32_t(ਆਕਾਰ(sectionSizes[0]))) {
        // We don't even have a TOC!
        ਜਾਓ invalidFormat;
    }
    ਸਾਰਣੀ = reinterpret_cast<ਅਚਲ uint16_t*>(sectionSizes);
    ਸਾਰਣੀ_ਸ਼ੁਰੂ = sectionSizes[0];
    ਮਾਪ_ਦਾ_TOC = int32_t((ਸਾਰਣੀ_ਸ਼ੁਰੂ + 1) * ਆਕਾਰ(sectionSizes[0]));
    ਜੇ (ਸਾਰਣੀ_ਸ਼ੁਰੂ < min_Toc_ਲੰਮਾਈ || (is_ਅੰਕੜੇ_ਲੰਮਾਈ_ਗਿਆਤ && ਅੰਕੜੇ_ਲੰਮਾਈ <= ਮਾਪ_ਦਾ_TOC)) {
        // We don't have a whole TOC!
        ਜਾਓ invalidFormat;
    }
    g_Alias_ਅੰਕੜੇ = ਅੰਕੜੇ;

    g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ      = sectionSizes[1];
    g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ            = sectionSizes[2];
    g_Main_ਸਾਰਣੀ.alias_List_ਮਾਪ          = sectionSizes[3];
    g_Main_ਸਾਰਣੀ.untagged_Conv_Array_ਮਾਪ  = sectionSizes[4];
    g_Main_ਸਾਰਣੀ.tagged_Alias_Array_ਮਾਪ   = sectionSizes[5];
    g_Main_ਸਾਰਣੀ.tagged_Alias_Lists_ਮਾਪ   = sectionSizes[6];
    g_Main_ਸਾਰਣੀ.option_ਸਾਰਣੀ_ਮਾਪ        = sectionSizes[7];
    g_Main_ਸਾਰਣੀ.string_ਸਾਰਣੀ_ਮਾਪ        = sectionSizes[8];

    ਜੇ (ਸਾਰਣੀ_ਸ਼ੁਰੂ > min_Toc_ਲੰਮਾਈ) {
        g_Main_ਸਾਰਣੀ.normalized_String_ਸਾਰਣੀ_ਮਾਪ = sectionSizes[9];
    }

    ਮਾਪ_ਦਾ_ਅੰਕੜੇ = ਮਾਪ_ਦਾ_TOC;
    ਲਈ (uint32_t section = 1; section <= ਸਾਰਣੀ_ਸ਼ੁਰੂ; section++) {
        ਮਾਪ_ਦਾ_ਅੰਕੜੇ += sectionSizes[section] * ਆਕਾਰ(ਸਾਰਣੀ[0]);
    }
    ਜੇ (is_ਅੰਕੜੇ_ਲੰਮਾਈ_ਗਿਆਤ && ਅੰਕੜੇ_ਲੰਮਾਈ < ਮਾਪ_ਦਾ_ਅੰਕੜੇ) {
        // Truncated file!
        ਜਾਓ invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    curr_ਵਿੱਥ = (ਸਾਰਣੀ_ਸ਼ੁਰੂ + 1) * (ਆਕਾਰ(uint32_t)/ਆਕਾਰ(uint16_t));
    g_Main_ਸਾਰਣੀ.converterList = ਸਾਰਣੀ + curr_ਵਿੱਥ;

    curr_ਵਿੱਥ += g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ;
    g_Main_ਸਾਰਣੀ.ਟੈਗ_List = ਸਾਰਣੀ + curr_ਵਿੱਥ;

    curr_ਵਿੱਥ += g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ;
    g_Main_ਸਾਰਣੀ.aliasList = ਸਾਰਣੀ + curr_ਵਿੱਥ;

    curr_ਵਿੱਥ += g_Main_ਸਾਰਣੀ.alias_List_ਮਾਪ;
    g_Main_ਸਾਰਣੀ.untaggedConvArray = ਸਾਰਣੀ + curr_ਵਿੱਥ;

    curr_ਵਿੱਥ += g_Main_ਸਾਰਣੀ.untagged_Conv_Array_ਮਾਪ;
    g_Main_ਸਾਰਣੀ.taggedAliasArray = ਸਾਰਣੀ + curr_ਵਿੱਥ;

    /* aliasLists is a 1's based array, but it has a padding character */
    curr_ਵਿੱਥ += g_Main_ਸਾਰਣੀ.tagged_Alias_Array_ਮਾਪ;
    g_Main_ਸਾਰਣੀ.taggedAliasLists = ਸਾਰਣੀ + curr_ਵਿੱਥ;

    curr_ਵਿੱਥ += g_Main_ਸਾਰਣੀ.tagged_Alias_Lists_ਮਾਪ;
    ਜੇ (g_Main_ਸਾਰਣੀ.option_ਸਾਰਣੀ_ਮਾਪ > 0
        && reinterpret_cast<ਅਚਲ UConverterAliasOptions*>(ਸਾਰਣੀ + curr_ਵਿੱਥ)->stringNormalizationType < UCNV_IO_NORM_TYPE_ਗਿਣਤੀ)
    {
        /* Faster table */
        g_Main_ਸਾਰਣੀ.option_ਸਾਰਣੀ = reinterpret_cast<ਅਚਲ UConverterAliasOptions*>(ਸਾਰਣੀ + curr_ਵਿੱਥ);
    }
    ਨਹੀਂਤਾਂ {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        g_Main_ਸਾਰਣੀ.option_ਸਾਰਣੀ = &default_ਸਾਰਣੀ_Options;
    }

    curr_ਵਿੱਥ += g_Main_ਸਾਰਣੀ.option_ਸਾਰਣੀ_ਮਾਪ;
    g_Main_ਸਾਰਣੀ.string_ਸਾਰਣੀ = ਸਾਰਣੀ + curr_ਵਿੱਥ;

    curr_ਵਿੱਥ += g_Main_ਸਾਰਣੀ.string_ਸਾਰਣੀ_ਮਾਪ;
    g_Main_ਸਾਰਣੀ.normalized_String_ਸਾਰਣੀ = ((g_Main_ਸਾਰਣੀ.option_ਸਾਰਣੀ->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? g_Main_ਸਾਰਣੀ.string_ਸਾਰਣੀ : (ਸਾਰਣੀ + curr_ਵਿੱਥ));

    ਵਾਪਸ;

invalidFormat:
    err_ਕੋਡ = U_INVALID_FORMAT_ERROR;
    udata_close(ਅੰਕੜੇ);
}


ਸਥਿਰ UBool
have_Alias_ਅੰਕੜੇ(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    umtx_initOnce(g_Alias_ਅੰਕੜੇ_Init_Once, &init_Alias_ਅੰਕੜੇ, *p_Error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(*p_Error_ਕੋਡ);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
isAlias(ਅਚਲ ਅੱਖਰ *alias, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(alias==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ *alias != 0;
}

ਸਥਿਰ uint32_t get_ਟੈਗ_ਸੰਖਿਆ(ਅਚਲ ਅੱਖਰ *tagname) {
    ਜੇ (g_Main_ਸਾਰਣੀ.ਟੈਗ_List) {
        uint32_t ਟੈਗ_ਅੰਕ;
        ਲਈ (ਟੈਗ_ਅੰਕ = 0; ਟੈਗ_ਅੰਕ < g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ; ਟੈਗ_ਅੰਕ++) {
            ਜੇ (!uprv_stricmp(GET_STRING(g_Main_ਸਾਰਣੀ.ਟੈਗ_List[ਟੈਗ_ਅੰਕ]), tagname)) {
                ਵਾਪਸ ਟੈਗ_ਅੰਕ;
            }
        }
    }

    ਵਾਪਸ UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
ਗਿਣਤੀ {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
ਸਥਿਰ ਅਚਲ uint8_t asciiTypes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

/* character types for EBCDIC 80..FF */
ਸਥਿਰ ਅਚਲ uint8_t ebcdicTypes[128] = {
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#ਜੇ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#ਜਾਂਜੇ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#ਨਹੀਂਤਾਂ
#   error U_CHARSET_FAMILY is not valid
#ਜੇ_ਅੰਤ


/* @see ucnv_compareNames */
U_CAPI ਅੱਖਰ* U_CALLCONV
ucnv_io_stripASCIIForCompare(ਅੱਖਰ* dst U_LIFETIME_BOUND, ਅਚਲ ਅੱਖਰ* name) {
    ਅੱਖਰ *dstItr = dst;
    uint8_t type, ਅਗਲਾ_Type;
    ਅੱਖਰ c1;
    UBool afterDigit = ਝੂਠ;

    ਜਦੋਂ ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        ਚੋਣ (type) {
        ਹਾਲਤ UIGNORE:
            afterDigit = ਝੂਠ;
            ਜਾਰੀ; /* ignore all but letters and digits */
        ਹਾਲਤ ZERO:
            ਜੇ (!afterDigit) {
                ਅਗਲਾ_Type = GET_ASCII_TYPE(*name);
                ਜੇ (ਅਗਲਾ_Type == ZERO || ਅਗਲਾ_Type == NONZERO) {
                    ਜਾਰੀ; /* ignore leading zero before another digit */
                }
            }
            ਰੋਕੋ;
        ਹਾਲਤ NONZERO:
            afterDigit = ਸੱਚ;
            ਰੋਕੋ;
        ਮੂਲ:
            c1 = (ਅੱਖਰ)type; /* lowercased letter */
            afterDigit = ਝੂਠ;
            ਰੋਕੋ;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ਵਾਪਸ dst;
}

U_CAPI ਅੱਖਰ* U_CALLCONV
ucnv_io_stripEBCDICForCompare(ਅੱਖਰ* dst U_LIFETIME_BOUND, ਅਚਲ ਅੱਖਰ* name) {
    ਅੱਖਰ *dstItr = dst;
    uint8_t type, ਅਗਲਾ_Type;
    ਅੱਖਰ c1;
    UBool afterDigit = ਝੂਠ;

    ਜਦੋਂ ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        ਚੋਣ (type) {
        ਹਾਲਤ UIGNORE:
            afterDigit = ਝੂਠ;
            ਜਾਰੀ; /* ignore all but letters and digits */
        ਹਾਲਤ ZERO:
            ਜੇ (!afterDigit) {
                ਅਗਲਾ_Type = GET_EBCDIC_TYPE(*name);
                ਜੇ (ਅਗਲਾ_Type == ZERO || ਅਗਲਾ_Type == NONZERO) {
                    ਜਾਰੀ; /* ignore leading zero before another digit */
                }
            }
            ਰੋਕੋ;
        ਹਾਲਤ NONZERO:
            afterDigit = ਸੱਚ;
            ਰੋਕੋ;
        ਮੂਲ:
            c1 = (ਅੱਖਰ)type; /* lowercased letter */
            afterDigit = ਝੂਠ;
            ਰੋਕੋ;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ਵਾਪਸ dst;
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
U_CAPI ਪੂਰਨ U_EXPORT2
ucnv_compare_ਨਾਂ(ਅਚਲ ਅੱਖਰ *name1, ਅਚਲ ਅੱਖਰ *name2) {
    ਪੂਰਨ rc;
    uint8_t type, ਅਗਲਾ_Type;
    ਅੱਖਰ c1, c2;
    UBool afterDigit1 = ਝੂਠ, afterDigit2 = ਝੂਠ;

    ਲਈ (;;) {
        ਜਦੋਂ ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            ਚੋਣ (type) {
            ਹਾਲਤ UIGNORE:
                afterDigit1 = ਝੂਠ;
                ਜਾਰੀ; /* ignore all but letters and digits */
            ਹਾਲਤ ZERO:
                ਜੇ (!afterDigit1) {
                    ਅਗਲਾ_Type = GET_CHAR_TYPE(*name1);
                    ਜੇ (ਅਗਲਾ_Type == ZERO || ਅਗਲਾ_Type == NONZERO) {
                        ਜਾਰੀ; /* ignore leading zero before another digit */
                    }
                }
                ਰੋਕੋ;
            ਹਾਲਤ NONZERO:
                afterDigit1 = ਸੱਚ;
                ਰੋਕੋ;
            ਮੂਲ:
                c1 = (ਅੱਖਰ)type; /* lowercased letter */
                afterDigit1 = ਝੂਠ;
                ਰੋਕੋ;
            }
            ਰੋਕੋ; /* deliver c1 */
        }
        ਜਦੋਂ ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            ਚੋਣ (type) {
            ਹਾਲਤ UIGNORE:
                afterDigit2 = ਝੂਠ;
                ਜਾਰੀ; /* ignore all but letters and digits */
            ਹਾਲਤ ZERO:
                ਜੇ (!afterDigit2) {
                    ਅਗਲਾ_Type = GET_CHAR_TYPE(*name2);
                    ਜੇ (ਅਗਲਾ_Type == ZERO || ਅਗਲਾ_Type == NONZERO) {
                        ਜਾਰੀ; /* ignore leading zero before another digit */
                    }
                }
                ਰੋਕੋ;
            ਹਾਲਤ NONZERO:
                afterDigit2 = ਸੱਚ;
                ਰੋਕੋ;
            ਮੂਲ:
                c2 = (ਅੱਖਰ)type; /* lowercased letter */
                afterDigit2 = ਝੂਠ;
                ਰੋਕੋ;
            }
            ਰੋਕੋ; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        ਜੇ ((c1|c2)==0) {
            ਵਾਪਸ 0;
        }

        /* Case-insensitive comparison */
        rc = (ਪੂਰਨ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)c1 - (ਪੂਰਨ)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)c2;
        ਜੇ (rc != 0) {
            ਵਾਪਸ rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t
ਲੱਭੋ_Converter(ਅਚਲ ਅੱਖਰ *alias, UBool *ਸ਼ਾਮਲ_ਹੈ_Option, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t mid, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;
    uint32_t ਆਖਰੀ_Mid;
    ਪੂਰਨ ਨਤੀਜਾ;
    ਪੂਰਨ isUnnormalized = (g_Main_ਸਾਰਣੀ.option_ਸਾਰਣੀ->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    ਅੱਖਰ strippedName[UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ];

    ਜੇ (!isUnnormalized) {
        ਜੇ (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ) {
            *p_Error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਵਾਪਸ UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    ਸ਼ੁਰੂ = 0;
    ਹੱਦਬੰਦੀ = g_Main_ਸਾਰਣੀ.untagged_Conv_Array_ਮਾਪ;
    mid = ਹੱਦਬੰਦੀ;
    ਆਖਰੀ_Mid = UINT32_MAX;

    ਲਈ (;;) {
        mid = (ਸ਼ੁਰੂ + ਹੱਦਬੰਦੀ) / 2;
        ਜੇ (ਆਖਰੀ_Mid == mid) {   /* Have we moved? */
            ਰੋਕੋ;  /* We haven't moved, and it wasn't found. */
        }
        ਆਖਰੀ_Mid = mid;
        ਜੇ (isUnnormalized) {
            ਨਤੀਜਾ = ucnv_compare_ਨਾਂ(alias, GET_STRING(g_Main_ਸਾਰਣੀ.aliasList[mid]));
        }
        ਨਹੀਂਤਾਂ {
            ਨਤੀਜਾ = uprv_strcmp(alias, GET_NORMALIZED_STRING(g_Main_ਸਾਰਣੀ.aliasList[mid]));
        }

        ਜੇ (ਨਤੀਜਾ < 0) {
            ਹੱਦਬੰਦੀ = mid;
        } ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ > 0) {
            ਸ਼ੁਰੂ = mid;
        } ਨਹੀਂਤਾਂ {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            ਜੇ (g_Main_ਸਾਰਣੀ.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_ਦੋਆਧਾਰੀ_ਅੰਕ) {
                *p_Error_ਕੋਡ = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            ਜੇ (ਸ਼ਾਮਲ_ਹੈ_Option) {
                UBool ਸ਼ਾਮਲ_ਹੈ_Cnv_Option_Info = static_cast<UBool>(g_Main_ਸਾਰਣੀ.option_ਸਾਰਣੀ->ਸ਼ਾਮਲ_ਹੈ_Cnv_Option_Info);
                *ਸ਼ਾਮਲ_ਹੈ_Option = static_cast<UBool>((ਸ਼ਾਮਲ_ਹੈ_Cnv_Option_Info
                    && ((g_Main_ਸਾਰਣੀ.untaggedConvArray[mid] & UCNV_ਸ਼ਾਮਲ_ਹੈ_OPTION_ਦੋਆਧਾਰੀ_ਅੰਕ) != 0))
                    || !ਸ਼ਾਮਲ_ਹੈ_Cnv_Option_Info);
            }
            ਵਾਪਸ g_Main_ਸਾਰਣੀ.untaggedConvArray[mid] & UCNV_CONVERTER_ਸੂਚਕਾਂਕ_ਨਕਾਬ;
        }
    }

    ਵਾਪਸ UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
is_Alias_ਵਿੱਚ_List(ਅਚਲ ਅੱਖਰ *alias, uint32_t list_ਵਿੱਥ) {
    ਜੇ (list_ਵਿੱਥ) {
        uint32_t currAlias;
        uint32_t list_ਗਿਣਤੀ = g_Main_ਸਾਰਣੀ.taggedAliasLists[list_ਵਿੱਥ];
        /* +1 to skip listCount */
        ਅਚਲ uint16_t *currList = g_Main_ਸਾਰਣੀ.taggedAliasLists + list_ਵਿੱਥ + 1;
        ਲਈ (currAlias = 0; currAlias < list_ਗਿਣਤੀ; currAlias++) {
            ਜੇ (currList[currAlias]
                && ucnv_compare_ਨਾਂ(alias, GET_STRING(currList[currAlias]))==0)
            {
                ਵਾਪਸ ਸੱਚ;
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
ਸਥਿਰ uint32_t
ਲੱਭੋ_Tagged_Alias_Lists_ਵਿੱਥ(ਅਚਲ ਅੱਖਰ *alias, ਅਚਲ ਅੱਖਰ *standard, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t idx;
    uint32_t list_ਵਿੱਥ;
    uint32_t conv_ਅੰਕ;
    UError_ਕੋਡ myErr = U_ZERO_ERROR;
    uint32_t ਟੈਗ_ਅੰਕ = get_ਟੈਗ_ਸੰਖਿਆ(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_ਅੰਕ = ਲੱਭੋ_Converter(alias, ਖਾਲੀ_ਸੂਚਕ, &myErr);
    ਜੇ (myErr != U_ZERO_ERROR) {
        *p_Error_ਕੋਡ = myErr;
    }

    ਜੇ (ਟੈਗ_ਅੰਕ < (g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ - UCNV_ਅੰਕ_HIDDEN_ਟੈਗ_ਸੂਚੀ) && conv_ਅੰਕ < g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ) {
        list_ਵਿੱਥ = g_Main_ਸਾਰਣੀ.taggedAliasArray[ਟੈਗ_ਅੰਕ*g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ + conv_ਅੰਕ];
        ਜੇ (list_ਵਿੱਥ && g_Main_ਸਾਰਣੀ.taggedAliasLists[list_ਵਿੱਥ + 1]) {
            ਵਾਪਸ list_ਵਿੱਥ;
        }
        ਜੇ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            ਲਈ (idx = 0; idx < g_Main_ਸਾਰਣੀ.tagged_Alias_Array_ਮਾਪ; idx++) {
                list_ਵਿੱਥ = g_Main_ਸਾਰਣੀ.taggedAliasArray[idx];
                ਜੇ (list_ਵਿੱਥ && is_Alias_ਵਿੱਚ_List(alias, list_ਵਿੱਥ)) {
                    uint32_t curr_ਟੈਗ_ਅੰਕ = idx/g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ;
                    uint32_t curr_Conv_ਅੰਕ = (idx - curr_ਟੈਗ_ਅੰਕ*g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ);
                    uint32_t ਆਰਜ਼ੀ_List_ਵਿੱਥ = g_Main_ਸਾਰਣੀ.taggedAliasArray[ਟੈਗ_ਅੰਕ*g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ + curr_Conv_ਅੰਕ];
                    ਜੇ (ਆਰਜ਼ੀ_List_ਵਿੱਥ && g_Main_ਸਾਰਣੀ.taggedAliasLists[ਆਰਜ਼ੀ_List_ਵਿੱਥ + 1]) {
                        ਵਾਪਸ ਆਰਜ਼ੀ_List_ਵਿੱਥ;
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
        ਵਾਪਸ 0;
    }
    /* else converter or tag not found */

    ਵਾਪਸ UINT32_MAX;
}

/* Return the canonical name */
ਸਥਿਰ uint32_t
ਲੱਭੋ_Tagged_Converter_ਅੰਕ(ਅਚਲ ਅੱਖਰ *alias, ਅਚਲ ਅੱਖਰ *standard, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint32_t idx;
    uint32_t list_ਵਿੱਥ;
    uint32_t conv_ਅੰਕ;
    UError_ਕੋਡ myErr = U_ZERO_ERROR;
    uint32_t ਟੈਗ_ਅੰਕ = get_ਟੈਗ_ਸੰਖਿਆ(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_ਅੰਕ = ਲੱਭੋ_Converter(alias, ਖਾਲੀ_ਸੂਚਕ, &myErr);
    ਜੇ (myErr != U_ZERO_ERROR) {
        *p_Error_ਕੋਡ = myErr;
    }

    ਜੇ (ਟੈਗ_ਅੰਕ < (g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ - UCNV_ਅੰਕ_HIDDEN_ਟੈਗ_ਸੂਚੀ) && conv_ਅੰਕ < g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ) {
        list_ਵਿੱਥ = g_Main_ਸਾਰਣੀ.taggedAliasArray[ਟੈਗ_ਅੰਕ*g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ + conv_ਅੰਕ];
        ਜੇ (list_ਵਿੱਥ && is_Alias_ਵਿੱਚ_List(alias, list_ਵਿੱਥ)) {
            ਵਾਪਸ conv_ਅੰਕ;
        }
        ਜੇ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t conv_ਸ਼ੁਰੂ = (ਟੈਗ_ਅੰਕ)*g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ;
            uint32_t conv_ਹੱਦਬੰਦੀ = (ਟੈਗ_ਅੰਕ+1)*g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ;
            ਲਈ (idx = conv_ਸ਼ੁਰੂ; idx < conv_ਹੱਦਬੰਦੀ; idx++) {
                list_ਵਿੱਥ = g_Main_ਸਾਰਣੀ.taggedAliasArray[idx];
                ਜੇ (list_ਵਿੱਥ && is_Alias_ਵਿੱਚ_List(alias, list_ਵਿੱਥ)) {
                    ਵਾਪਸ idx-conv_ਸ਼ੁਰੂ;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    ਵਾਪਸ UINT32_MAX;
}

U_CAPI ਅਚਲ ਅੱਖਰ *
ucnv_io_getConverterName(ਅਚਲ ਅੱਖਰ *alias, UBool *ਸ਼ਾਮਲ_ਹੈ_Option, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ ਅੱਖਰ *aliasTmp = alias;
    int32_t i = 0;
    ਲਈ (i = 0; i < 2; i++) {
        ਜੇ (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            ਜੇ (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } ਨਹੀਂਤਾਂ {
                ਰੋਕੋ;
            }
        }
        ਜੇ(have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ) && isAlias(aliasTmp, p_Error_ਕੋਡ)) {
            uint32_t conv_ਅੰਕ = ਲੱਭੋ_Converter(aliasTmp, ਸ਼ਾਮਲ_ਹੈ_Option, p_Error_ਕੋਡ);
            ਜੇ (conv_ਅੰਕ < g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ) {
                ਵਾਪਸ GET_STRING(g_Main_ਸਾਰਣੀ.converterList[conv_ਅੰਕ]);
            }
            /* else converter not found */
        } ਨਹੀਂਤਾਂ {
            ਰੋਕੋ;
        }
    }

    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CDECL_BEGIN


ਸਥਿਰ int32_t U_CALLCONV
ucnv_io_ਗਿਣਤੀ_Standard_Aliases(UEnumeration *enumerator, UError_ਕੋਡ * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_ਵਿੱਥ = myContext->list_ਵਿੱਥ;

    ਜੇ (list_ਵਿੱਥ) {
        value = g_Main_ਸਾਰਣੀ.taggedAliasLists[list_ਵਿੱਥ];
    }
    ਵਾਪਸ value;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * U_CALLCONV
ucnv_io_ਅਗਲਾ_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                            UError_ਕੋਡ * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_ਵਿੱਥ = myContext->list_ਵਿੱਥ;

    ਜੇ (list_ਵਿੱਥ) {
        uint32_t list_ਗਿਣਤੀ = g_Main_ਸਾਰਣੀ.taggedAliasLists[list_ਵਿੱਥ];
        ਅਚਲ uint16_t *currList = g_Main_ਸਾਰਣੀ.taggedAliasLists + list_ਵਿੱਥ + 1;

        ਜੇ (myContext->listIdx < list_ਗਿਣਤੀ) {
            ਅਚਲ ਅੱਖਰ *myStr = GET_STRING(currList[myContext->listIdx++]);
            ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
                *ਨਤੀਜਾ_ਲੰਮਾਈ = (int32_t)uprv_strlen(myStr);
            }
            ਵਾਪਸ myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
        *ਨਤੀਜਾ_ਲੰਮਾਈ = 0;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UError_ਕੋਡ * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_ਸਿਰਾ

/* Enumerate the aliases for the specified converter and standard tag */
ਸਥਿਰ ਅਚਲ UEnumeration gEnumAliases = {
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_io_closeUEnumeration,
    ucnv_io_ਗਿਣਤੀ_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_ਅਗਲਾ_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_Standard_ਨਾਂ(ਅਚਲ ਅੱਖਰ *convName,
                       ਅਚਲ ਅੱਖਰ *standard,
                       UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    UEnumeration *myEnum = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ) && isAlias(convName, p_Error_ਕੋਡ)) {
        uint32_t list_ਵਿੱਥ = ਲੱਭੋ_Tagged_Alias_Lists_ਵਿੱਥ(convName, standard, p_Error_ਕੋਡ);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        ਜੇ (list_ਵਿੱਥ < g_Main_ਸਾਰਣੀ.tagged_Alias_Lists_ਮਾਪ) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(ਆਕਾਰ(UEnumeration)));
            ਜੇ (myEnum == ਖਾਲੀ_ਸੂਚਕ) {
                *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            uprv_memcpy(myEnum, &gEnumAliases, ਆਕਾਰ(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(ਆਕਾਰ(UAliasContext)));
            ਜੇ (myContext == ਖਾਲੀ_ਸੂਚਕ) {
                *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                uprv_free(myEnum);
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            myContext->list_ਵਿੱਥ = list_ਵਿੱਥ;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    ਵਾਪਸ myEnum;
}

ਸਥਿਰ uint16_t
ucnv_io_ਗਿਣਤੀ_Aliases(ਅਚਲ ਅੱਖਰ *alias, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ) && isAlias(alias, p_Error_ਕੋਡ)) {
        uint32_t conv_ਅੰਕ = ਲੱਭੋ_Converter(alias, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ);
        ਜੇ (conv_ਅੰਕ < g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_ਵਿੱਥ = g_Main_ਸਾਰਣੀ.taggedAliasArray[(g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ - 1)*g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ + conv_ਅੰਕ];

            ਜੇ (list_ਵਿੱਥ) {
                ਵਾਪਸ g_Main_ਸਾਰਣੀ.taggedAliasLists[list_ਵਿੱਥ];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ਵਾਪਸ 0;
}

ਸਥਿਰ uint16_t
ucnv_io_getAliases(ਅਚਲ ਅੱਖਰ *alias, uint16_t ਸ਼ੁਰੂ, ਅਚਲ ਅੱਖਰ **aliases, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ) && isAlias(alias, p_Error_ਕੋਡ)) {
        uint32_t currAlias;
        uint32_t conv_ਅੰਕ = ਲੱਭੋ_Converter(alias, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ);
        ਜੇ (conv_ਅੰਕ < g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_ਵਿੱਥ = g_Main_ਸਾਰਣੀ.taggedAliasArray[(g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ - 1)*g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ + conv_ਅੰਕ];

            ਜੇ (list_ਵਿੱਥ) {
                uint32_t list_ਗਿਣਤੀ = g_Main_ਸਾਰਣੀ.taggedAliasLists[list_ਵਿੱਥ];
                /* +1 to skip listCount */
                ਅਚਲ uint16_t *currList = g_Main_ਸਾਰਣੀ.taggedAliasLists + list_ਵਿੱਥ + 1;

                ਲਈ (currAlias = ਸ਼ੁਰੂ; currAlias < list_ਗਿਣਤੀ; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *
ucnv_io_getAlias(ਅਚਲ ਅੱਖਰ *alias, uint16_t n, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ) && isAlias(alias, p_Error_ਕੋਡ)) {
        uint32_t conv_ਅੰਕ = ਲੱਭੋ_Converter(alias, ਖਾਲੀ_ਸੂਚਕ, p_Error_ਕੋਡ);
        ਜੇ (conv_ਅੰਕ < g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_ਵਿੱਥ = g_Main_ਸਾਰਣੀ.taggedAliasArray[(g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ - 1)*g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ + conv_ਅੰਕ];

            ਜੇ (list_ਵਿੱਥ) {
                uint32_t list_ਗਿਣਤੀ = g_Main_ਸਾਰਣੀ.taggedAliasLists[list_ਵਿੱਥ];
                /* +1 to skip listCount */
                ਅਚਲ uint16_t *currList = g_Main_ਸਾਰਣੀ.taggedAliasLists + list_ਵਿੱਥ + 1;

                ਜੇ (n < list_ਗਿਣਤੀ)  {
                    ਵਾਪਸ GET_STRING(currList[n]);
                }
                *p_Error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਸਥਿਰ uint16_t
ucnv_io_ਗਿਣਤੀ_Standards(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ)) {
        /* Don't include the empty list */
        ਵਾਪਸ static_cast<uint16_t>(g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ - UCNV_ਅੰਕ_HIDDEN_ਟੈਗ_ਸੂਚੀ);
    }

    ਵਾਪਸ 0;
}

U_CAPI ਅਚਲ ਅੱਖਰ * U_EXPORT2
ucnv_getStandard(uint16_t n, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ)) {
        ਜੇ (n < g_Main_ਸਾਰਣੀ.ਟੈਗ_List_ਮਾਪ - UCNV_ਅੰਕ_HIDDEN_ਟੈਗ_ਸੂਚੀ) {
            ਵਾਪਸ GET_STRING(g_Main_ਸਾਰਣੀ.ਟੈਗ_List[n]);
        }
        *p_Error_ਕੋਡ = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    }

    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI ਅਚਲ ਅੱਖਰ * U_EXPORT2
ucnv_getStandardName(ਅਚਲ ਅੱਖਰ *alias, ਅਚਲ ਅੱਖਰ *standard, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ) && isAlias(alias, p_Error_ਕੋਡ)) {
        uint32_t list_ਵਿੱਥ = ਲੱਭੋ_Tagged_Alias_Lists_ਵਿੱਥ(alias, standard, p_Error_ਕੋਡ);

        ਜੇ (0 < list_ਵਿੱਥ && list_ਵਿੱਥ < g_Main_ਸਾਰਣੀ.tagged_Alias_Lists_ਮਾਪ) {
            ਅਚਲ uint16_t *currList = g_Main_ਸਾਰਣੀ.taggedAliasLists + list_ਵਿੱਥ + 1;

            /* Get the preferred name from this list */
            ਜੇ (currList[0]) {
                ਵਾਪਸ GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI uint16_t U_EXPORT2
ucnv_ਗਿਣਤੀ_Aliases(ਅਚਲ ਅੱਖਰ *alias, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਵਾਪਸ ucnv_io_ਗਿਣਤੀ_Aliases(alias, p_Error_ਕੋਡ);
}


U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
ucnv_getAlias(ਅਚਲ ਅੱਖਰ *alias, uint16_t n, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਵਾਪਸ ucnv_io_getAlias(alias, n, p_Error_ਕੋਡ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
ucnv_getAliases(ਅਚਲ ਅੱਖਰ *alias, ਅਚਲ ਅੱਖਰ **aliases, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ucnv_io_getAliases(alias, 0, aliases, p_Error_ਕੋਡ);
}

U_CAPI uint16_t U_EXPORT2
ucnv_ਗਿਣਤੀ_Standards()
{
    UError_ਕੋਡ err = U_ZERO_ERROR;
    ਵਾਪਸ ucnv_io_ਗਿਣਤੀ_Standards(&err);
}

U_CAPI ਅਚਲ ਅੱਖਰ * U_EXPORT2
ucnv_getCanonicalName(ਅਚਲ ਅੱਖਰ *alias, ਅਚਲ ਅੱਖਰ *standard, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ) && isAlias(alias, p_Error_ਕੋਡ)) {
        uint32_t conv_ਅੰਕ = ਲੱਭੋ_Tagged_Converter_ਅੰਕ(alias, standard, p_Error_ਕੋਡ);

        ਜੇ (conv_ਅੰਕ < g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ) {
            ਵਾਪਸ GET_STRING(g_Main_ਸਾਰਣੀ.converterList[conv_ਅੰਕ]);
        }
    }

    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CDECL_BEGIN


ਸਥਿਰ int32_t U_CALLCONV
ucnv_io_ਗਿਣਤੀ_All_Converters(UEnumeration * /*enumerator*/, UError_ਕੋਡ * /*pErrorCode*/) {
    ਵਾਪਸ g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * U_CALLCONV
ucnv_io_ਅਗਲਾ_All_Converters(UEnumeration *enumerator,
                            int32_t* ਨਤੀਜਾ_ਲੰਮਾਈ,
                            UError_ਕੋਡ * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    ਜੇ (*myContext < g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ) {
        ਅਚਲ ਅੱਖਰ *myStr = GET_STRING(g_Main_ਸਾਰਣੀ.converterList[(*myContext)++]);
        ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
            *ਨਤੀਜਾ_ਲੰਮਾਈ = (int32_t)uprv_strlen(myStr);
        }
        ਵਾਪਸ myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ਜੇ (ਨਤੀਜਾ_ਲੰਮਾਈ) {
        *ਨਤੀਜਾ_ਲੰਮਾਈ = 0;
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UError_ਕੋਡ * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_ਸਿਰਾ
ਸਥਿਰ ਅਚਲ UEnumeration gEnumAllConverters = {
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    ucnv_io_closeUEnumeration,
    ucnv_io_ਗਿਣਤੀ_All_Converters,
    uenum_unextDefault,
    ucnv_io_ਅਗਲਾ_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_All_ਨਾਂ(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UEnumeration *myEnum = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(ਆਕਾਰ(UEnumeration)));
        ਜੇ (myEnum == ਖਾਲੀ_ਸੂਚਕ) {
            *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, ਆਕਾਰ(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(ਆਕਾਰ(uint16_t)));
        ਜੇ (myContext == ਖਾਲੀ_ਸੂਚਕ) {
            *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            uprv_free(myEnum);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    ਵਾਪਸ myEnum;
}

U_CAPI uint16_t
ucnv_io_ਗਿਣਤੀ_ਗਿਆਤ_Converters(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ (have_Alias_ਅੰਕੜੇ(p_Error_ਕੋਡ)) {
        ਵਾਪਸ (uint16_t)g_Main_ਸਾਰਣੀ.converter_List_ਮਾਪ;
    }
    ਵਾਪਸ 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅੱਖਰ * U_CALLCONV StripForCompareFn(ਅੱਖਰ *dst, ਅਚਲ ਅੱਖਰ *name);
U_CDECL_ਸਿਰਾ


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਆਰਜ਼ੀ_ਪੰਕਤੀ {
    uint16_t str_ਸੂਚਕਾਂਕ, ਛਾਂਟੋ_ਸੂਚਕਾਂਕ;
} ਆਰਜ਼ੀ_ਪੰਕਤੀ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਆਰਜ਼ੀ_Alias_ਸਾਰਣੀ {
    ਅਚਲ ਅੱਖਰ *chars;
    ਆਰਜ਼ੀ_ਪੰਕਤੀ *ਪੰਕਤੀਆਂ;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} ਆਰਜ਼ੀ_Alias_ਸਾਰਣੀ;

ਗਿਣਤੀ {
    ਢੇਰ_ਪੰਕਤੀ_ਸਮਰੱਥਾ=500
};

ਸਥਿਰ int32_t U_CALLCONV
io_compare_ਪੰਕਤੀਆਂ(ਅਚਲ ਖਾਲੀ *context, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    ਅੱਖਰ stripped_ਖੱਬਾ[UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ],
         stripped_ਸੱਜਾ[UCNV_MAX_CONVERTER_NAME_ਲੰਮਾਈ];

    ਆਰਜ਼ੀ_Alias_ਸਾਰਣੀ *ਆਰਜ਼ੀ_ਸਾਰਣੀ=(ਆਰਜ਼ੀ_Alias_ਸਾਰਣੀ *)context;
    ਅਚਲ ਅੱਖਰ *chars=ਆਰਜ਼ੀ_ਸਾਰਣੀ->chars;

    ਵਾਪਸ static_cast<int32_t>(uprv_strcmp(
        ਆਰਜ਼ੀ_ਸਾਰਣੀ->stripForCompare(stripped_ਖੱਬਾ, chars + 2 * static_cast<ਅਚਲ ਆਰਜ਼ੀ_ਪੰਕਤੀ*>(ਖੱਬੇ)->str_ਸੂਚਕਾਂਕ),
        ਆਰਜ਼ੀ_ਸਾਰਣੀ->stripForCompare(stripped_ਸੱਜਾ, chars + 2 * static_cast<ਅਚਲ ਆਰਜ਼ੀ_ਪੰਕਤੀ*>(ਸੱਜੇ)->str_ਸੂਚਕਾਂਕ)));
}

U_CAPI int32_t U_EXPORT2
ucnv_ਅਦਲਾ_ਬਦਲੀ_Aliases(ਅਚਲ UDataSwapper *ds,
                 ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UDataInfo *pInfo;
    int32_t ਸਿਰਲੇਖ_ਮਾਪ;

    ਅਚਲ uint16_t *ਵਿੱਚ_ਸਾਰਣੀ;
    ਅਚਲ uint32_t *ਵਿੱਚ_Section_Sizes;
    uint32_t toc[offsets_ਗਿਣਤੀ];
    uint32_t offsets[offsets_ਗਿਣਤੀ]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_ਲੰਮਾਈ, ਸਿਖਰ_ਵਿੱਥ;

    ਆਰਜ਼ੀ_ਪੰਕਤੀ ਪੰਕਤੀਆਂ[ਢੇਰ_ਪੰਕਤੀ_ਸਮਰੱਥਾ];
    uint16_t resort[ਢੇਰ_ਪੰਕਤੀ_ਸਮਰੱਥਾ];
    ਆਰਜ਼ੀ_Alias_ਸਾਰਣੀ ਆਰਜ਼ੀ_ਸਾਰਣੀ;

    /* udata_swapDataHeader checks the arguments */
    ਸਿਰਲੇਖ_ਮਾਪ=udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* check data format and format version */
    pInfo=(ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
    ਜੇ(!(
        pInfo->ਅੰਕੜੇ_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x76 &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x41 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x6c &&
        pInfo->format_ਸੰਸਕਰਣ[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1],
                         pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3],
                         pInfo->format_ਸੰਸਕਰਣ[0]);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    /* an alias table must contain at least the table of contents array */
    ਜੇ(length>=0 && (length-ਸਿਰਲੇਖ_ਮਾਪ)<4*(1+min_Toc_ਲੰਮਾਈ)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-ਸਿਰਲੇਖ_ਮਾਪ);
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_Section_Sizes=(ਅਚਲ uint32_t *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ);
    ਵਿੱਚ_ਸਾਰਣੀ=(ਅਚਲ uint16_t *)ਵਿੱਚ_Section_Sizes;
    uprv_memset(toc, 0, ਆਕਾਰ(toc));
    toc[toc_ਲੰਮਾਈ_ਸੂਚਕਾਂਕ]=toc_ਲੰਮਾਈ=ds->readUInt32(ਵਿੱਚ_Section_Sizes[toc_ਲੰਮਾਈ_ਸੂਚਕਾਂਕ]);
    ਜੇ(toc_ਲੰਮਾਈ<min_Toc_ਲੰਮਾਈ || offsets_ਗਿਣਤੀ<=toc_ਲੰਮਾਈ) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_ਲੰਮਾਈ);
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ 0;
    }

    /* read the known part of the table of contents */
    ਲਈ(i=converter_List_ਸੂਚਕਾਂਕ; i<=toc_ਲੰਮਾਈ; ++i) {
        toc[i]=ds->readUInt32(ਵਿੱਚ_Section_Sizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, ਆਕਾਰ(offsets));
    offsets[converter_List_ਸੂਚਕਾਂਕ]=2*(1+toc_ਲੰਮਾਈ); /* count two 16-bit units per toc entry */
    ਲਈ(i=ਟੈਗ_List_ਸੂਚਕਾਂਕ; i<=toc_ਲੰਮਾਈ; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    ਸਿਖਰ_ਵਿੱਥ=offsets[i-1]+toc[i-1];

    ਜੇ(length>=0) {
        uint16_t *ਬਾਹਰਲਾ_ਸਾਰਣੀ;
        ਅਚਲ uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_ਸੂਚਕਾਂਕ;

        ਜੇ((length-ਸਿਰਲੇਖ_ਮਾਪ)<(2*(int32_t)ਸਿਖਰ_ਵਿੱਥ)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-ਸਿਰਲੇਖ_ਮਾਪ);
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }

        ਬਾਹਰਲਾ_ਸਾਰਣੀ=(uint16_t *)((ਅੱਖਰ *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ);

        /* swap the entire table of contents */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਸਾਰਣੀ, 4*(1+toc_ਲੰਮਾਈ), ਬਾਹਰਲਾ_ਸਾਰਣੀ, p_Error_ਕੋਡ);

        /* swap unormalized strings & normalized strings */
        ds->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars(ds, ਵਿੱਚ_ਸਾਰਣੀ+offsets[string_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ], 2*(int32_t)(toc[string_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ]+toc[normalized_String_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ]),
                             ਬਾਹਰਲਾ_ਸਾਰਣੀ+offsets[string_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ], p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            ਵਾਪਸ 0;
        }

        ਜੇ(ds->ਵਿੱਚ_Charset==ds->ਬਾਹਰਲਾ_Charset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds,
                            ਵਿੱਚ_ਸਾਰਣੀ+offsets[converter_List_ਸੂਚਕਾਂਕ],
                            2*(int32_t)(offsets[string_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ]-offsets[converter_List_ਸੂਚਕਾਂਕ]),
                            ਬਾਹਰਲਾ_ਸਾਰਣੀ+offsets[converter_List_ਸੂਚਕਾਂਕ],
                            p_Error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_ਸੂਚਕਾਂਕ];

            ਆਰਜ਼ੀ_ਸਾਰਣੀ.chars=(ਅਚਲ ਅੱਖਰ *)(ਬਾਹਰਲਾ_ਸਾਰਣੀ+offsets[string_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ]); /* sort by outCharset */

            ਜੇ(count<=ਢੇਰ_ਪੰਕਤੀ_ਸਮਰੱਥਾ) {
                ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ=ਪੰਕਤੀਆਂ;
                ਆਰਜ਼ੀ_ਸਾਰਣੀ.resort=resort;
            } ਨਹੀਂਤਾਂ {
                ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ=(ਆਰਜ਼ੀ_ਪੰਕਤੀ *)uprv_malloc(count*ਆਕਾਰ(ਆਰਜ਼ੀ_ਪੰਕਤੀ)+count*2);
                ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ==ਖਾਲੀ_ਸੂਚਕ) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਵਾਪਸ 0;
                }
                ਆਰਜ਼ੀ_ਸਾਰਣੀ.resort=(uint16_t *)(ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ+count);
            }

            ਜੇ(ds->ਬਾਹਰਲਾ_Charset==U_ASCII_FAMILY) {
                ਆਰਜ਼ੀ_ਸਾਰਣੀ.stripForCompare=ucnv_io_stripASCIIForCompare;
            } ਨਹੀਂਤਾਂ /* U_EBCDIC_FAMILY */ {
                ਆਰਜ਼ੀ_ਸਾਰਣੀ.stripForCompare=ucnv_io_stripEBCDICForCompare;
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
            p=ਵਿੱਚ_ਸਾਰਣੀ+offsets[alias_List_ਸੂਚਕਾਂਕ];
            q=ਬਾਹਰਲਾ_ਸਾਰਣੀ+offsets[alias_List_ਸੂਚਕਾਂਕ];

            p2=ਵਿੱਚ_ਸਾਰਣੀ+offsets[untagged_Conv_Array_ਸੂਚਕਾਂਕ];
            q2=ਬਾਹਰਲਾ_ਸਾਰਣੀ+offsets[untagged_Conv_Array_ਸੂਚਕਾਂਕ];

            ਲਈ(i=0; i<count; ++i) {
                ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ[i].str_ਸੂਚਕਾਂਕ=ds->readUInt16(p[i]);
                ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ[i].ਛਾਂਟੋ_ਸੂਚਕਾਂਕ=(uint16_t)i;
            }

            uprv_ਛਾਂਟੋ_Array(ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ, (int32_t)count, ਆਕਾਰ(ਆਰਜ਼ੀ_ਪੰਕਤੀ),
                           io_compare_ਪੰਕਤੀਆਂ, &ਆਰਜ਼ੀ_ਸਾਰਣੀ,
                           ਝੂਠ, p_Error_ਕੋਡ);

            ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
                /* copy/swap/permutate items */
                ਜੇ(p!=q) {
                    ਲਈ(i=0; i<count; ++i) {
                        old_ਸੂਚਕਾਂਕ=ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ[i].ਛਾਂਟੋ_ਸੂਚਕਾਂਕ;
                        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, p+old_ਸੂਚਕਾਂਕ, 2, q+i, p_Error_ਕੋਡ);
                        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, p2+old_ਸੂਚਕਾਂਕ, 2, q2+i, p_Error_ਕੋਡ);
                    }
                } ਨਹੀਂਤਾਂ {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=ਆਰਜ਼ੀ_ਸਾਰਣੀ.resort;

                    ਲਈ(i=0; i<count; ++i) {
                        old_ਸੂਚਕਾਂਕ=ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ[i].ਛਾਂਟੋ_ਸੂਚਕਾਂਕ;
                        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, p+old_ਸੂਚਕਾਂਕ, 2, r+i, p_Error_ਕੋਡ);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    ਲਈ(i=0; i<count; ++i) {
                        old_ਸੂਚਕਾਂਕ=ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ[i].ਛਾਂਟੋ_ਸੂਚਕਾਂਕ;
                        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, p2+old_ਸੂਚਕਾਂਕ, 2, r+i, p_Error_ਕੋਡ);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ!=ਪੰਕਤੀਆਂ) {
                uprv_free(ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ);
            }

            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                ਵਾਪਸ 0;
            }

            /* swap remaining 16-bit values */
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds,
                            ਵਿੱਚ_ਸਾਰਣੀ+offsets[converter_List_ਸੂਚਕਾਂਕ],
                            2*(int32_t)(offsets[alias_List_ਸੂਚਕਾਂਕ]-offsets[converter_List_ਸੂਚਕਾਂਕ]),
                            ਬਾਹਰਲਾ_ਸਾਰਣੀ+offsets[converter_List_ਸੂਚਕਾਂਕ],
                            p_Error_ਕੋਡ);
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds,
                            ਵਿੱਚ_ਸਾਰਣੀ+offsets[tagged_Alias_Array_ਸੂਚਕਾਂਕ],
                            2*(int32_t)(offsets[string_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ]-offsets[tagged_Alias_Array_ਸੂਚਕਾਂਕ]),
                            ਬਾਹਰਲਾ_ਸਾਰਣੀ+offsets[tagged_Alias_Array_ਸੂਚਕਾਂਕ],
                            p_Error_ਕੋਡ);
        }
    }

    ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ+2*(int32_t)ਸਿਖਰ_ਵਿੱਥ;
}

#ਜੇ_ਅੰਤ


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

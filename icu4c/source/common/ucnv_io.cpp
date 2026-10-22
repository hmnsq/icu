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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"

#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "udatamem.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ucnv_io.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UAliasContext {
    uint32_t list_ಸ್ಥಾನಾಂತರ;
    uint32_t listIdx;
} UAliasContext;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ದತ್ತಾಂಶ_NAME[] = "cnvalias";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ದತ್ತಾಂಶ_TYPE[] = "icu";

ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *g_Alias_ದತ್ತಾಂಶ=ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::UInitOnce g_Alias_ದತ್ತಾಂಶ_Init_Once {};

ಎಣಿಕೆ {
    toc_ಉದ್ದಳತೆ_ಸೂಚ್ಯಂಕ=0,
    converter_List_ಸೂಚ್ಯಂಕ=1,
    ಟ್ಯಾಗ್_List_ಸೂಚ್ಯಂಕ=2,
    alias_List_ಸೂಚ್ಯಂಕ=3,
    untagged_Conv_Array_ಸೂಚ್ಯಂಕ=4,
    tagged_Alias_Array_ಸೂಚ್ಯಂಕ=5,
    tagged_Alias_Lists_ಸೂಚ್ಯಂಕ=6,
    ಕೋಷ್ಟಕ_Options_ಸೂಚ್ಯಂಕ=7,
    string_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ=8,
    normalized_String_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ=9,
    offsets_ಎಣಿಕೆ,    /* length of the swapper's temporary offsets[] */
    min_Toc_ಉದ್ದಳತೆ=8 /* min. tocLength in the file, does not count the tocLengthIndex! */
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterAliasOptions default_ಕೋಷ್ಟಕ_Options = {
    UCNV_IO_UNNORMALIZED,
    0 /* containsCnvOptionInfo */
};
ಸ್ಥಿರ UConverterAlias g_Main_ಕೋಷ್ಟಕ;

#ವ್ಯಾಖ್ಯಾನಿಸು GET_STRING(idx) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(g_Main_ಕೋಷ್ಟಕ.string_ಕೋಷ್ಟಕ + (idx))
#ವ್ಯಾಖ್ಯಾನಿಸು GET_NORMALIZED_STRING(idx) (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(g_Main_ಕೋಷ್ಟಕ.normalized_String_ಕೋಷ್ಟಕ + (idx))

ಸ್ಥಿರ UBool U_CALLCONV
isAcceptable(ಶೂನ್ಯ * /*context*/,
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*type*/, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*name*/,
             ಸ್ಥಿರಾಂಕ UDataInfo *pInfo) {
    ಹಿಂತಿರುಗಿ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x76 &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x41 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x6c &&
        pInfo->format_ಆವೃತ್ತಿ[0]==3;
}

ಸ್ಥಿರ UBool U_CALLCONV ucnv_io_cleanup()
{
    ಆದರೆ (g_Alias_ದತ್ತಾಂಶ) {
        udata_close(g_Alias_ದತ್ತಾಂಶ);
        g_Alias_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    }
    g_Alias_ದತ್ತಾಂಶ_Init_Once.reset();

    uprv_memset(&g_Main_ಕೋಷ್ಟಕ, 0, ಗಾತ್ರ(g_Main_ಕೋಷ್ಟಕ));

    ಹಿಂತಿರುಗಿ ನಿಜ;                   /* Everything was cleaned up */
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV init_Alias_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &err_ಸಂಕೇತ) {
    UData_ಸ್ಮರಣೆ *ದತ್ತಾಂಶ;
    ಸ್ಥಿರಾಂಕ uint16_t *ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ uint32_t *sectionSizes;
    uint32_t ಕೋಷ್ಟಕ_ಆರಂಭ;
    uint32_t curr_ಸ್ಥಾನಾಂತರ;
    int32_t ಗಾತ್ರ_ದ_ದತ್ತಾಂಶ;
    int32_t ಗಾತ್ರ_ದ_TOC;

    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_UCNV_IO, ucnv_io_cleanup);

    U_ASSERT(g_Alias_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ);
    ದತ್ತಾಂಶ = udata_openChoice(ಶೂನ್ಯಸೂಚಕ, ದತ್ತಾಂಶ_TYPE, ದತ್ತಾಂಶ_NAME, isAcceptable, ಶೂನ್ಯಸೂಚಕ, &err_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(err_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    sectionSizes = static_cast<ಸ್ಥಿರಾಂಕ uint32_t*>(udata_get_ಸ್ಮರಣೆ(ದತ್ತಾಂಶ));
    int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = udata_get_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ); // This is the length minus the UDataInfo size
    UBool is_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ_ತಿಳಿದ = ದತ್ತಾಂಶ_ಉದ್ದಳತೆ >= 0; // Only false when using a pointer table of contents (not files nor a common data archive)
    ಆದರೆ (is_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ_ತಿಳಿದ && ದತ್ತಾಂಶ_ಉದ್ದಳತೆ <= int32_t(ಗಾತ್ರ(sectionSizes[0]))) {
        // We don't even have a TOC!
        ಹೋಗು invalidFormat;
    }
    ಕೋಷ್ಟಕ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(sectionSizes);
    ಕೋಷ್ಟಕ_ಆರಂಭ = sectionSizes[0];
    ಗಾತ್ರ_ದ_TOC = int32_t((ಕೋಷ್ಟಕ_ಆರಂಭ + 1) * ಗಾತ್ರ(sectionSizes[0]));
    ಆದರೆ (ಕೋಷ್ಟಕ_ಆರಂಭ < min_Toc_ಉದ್ದಳತೆ || (is_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ_ತಿಳಿದ && ದತ್ತಾಂಶ_ಉದ್ದಳತೆ <= ಗಾತ್ರ_ದ_TOC)) {
        // We don't have a whole TOC!
        ಹೋಗು invalidFormat;
    }
    g_Alias_ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;

    g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ      = sectionSizes[1];
    g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ            = sectionSizes[2];
    g_Main_ಕೋಷ್ಟಕ.alias_List_ಗಾತ್ರ          = sectionSizes[3];
    g_Main_ಕೋಷ್ಟಕ.untagged_Conv_Array_ಗಾತ್ರ  = sectionSizes[4];
    g_Main_ಕೋಷ್ಟಕ.tagged_Alias_Array_ಗಾತ್ರ   = sectionSizes[5];
    g_Main_ಕೋಷ್ಟಕ.tagged_Alias_Lists_ಗಾತ್ರ   = sectionSizes[6];
    g_Main_ಕೋಷ್ಟಕ.option_ಕೋಷ್ಟಕ_ಗಾತ್ರ        = sectionSizes[7];
    g_Main_ಕೋಷ್ಟಕ.string_ಕೋಷ್ಟಕ_ಗಾತ್ರ        = sectionSizes[8];

    ಆದರೆ (ಕೋಷ್ಟಕ_ಆರಂಭ > min_Toc_ಉದ್ದಳತೆ) {
        g_Main_ಕೋಷ್ಟಕ.normalized_String_ಕೋಷ್ಟಕ_ಗಾತ್ರ = sectionSizes[9];
    }

    ಗಾತ್ರ_ದ_ದತ್ತಾಂಶ = ಗಾತ್ರ_ದ_TOC;
    ಗಾಗಿ (uint32_t section = 1; section <= ಕೋಷ್ಟಕ_ಆರಂಭ; section++) {
        ಗಾತ್ರ_ದ_ದತ್ತಾಂಶ += sectionSizes[section] * ಗಾತ್ರ(ಕೋಷ್ಟಕ[0]);
    }
    ಆದರೆ (is_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ_ತಿಳಿದ && ದತ್ತಾಂಶ_ಉದ್ದಳತೆ < ಗಾತ್ರ_ದ_ದತ್ತಾಂಶ) {
        // Truncated file!
        ಹೋಗು invalidFormat;
    }
    // There may be some extra padding at the end, or this is a new file format with extra data that we can't read yet.

    curr_ಸ್ಥಾನಾಂತರ = (ಕೋಷ್ಟಕ_ಆರಂಭ + 1) * (ಗಾತ್ರ(uint32_t)/ಗಾತ್ರ(uint16_t));
    g_Main_ಕೋಷ್ಟಕ.converterList = ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ;

    curr_ಸ್ಥಾನಾಂತರ += g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ;
    g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List = ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ;

    curr_ಸ್ಥಾನಾಂತರ += g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ;
    g_Main_ಕೋಷ್ಟಕ.aliasList = ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ;

    curr_ಸ್ಥಾನಾಂತರ += g_Main_ಕೋಷ್ಟಕ.alias_List_ಗಾತ್ರ;
    g_Main_ಕೋಷ್ಟಕ.untaggedConvArray = ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ;

    curr_ಸ್ಥಾನಾಂತರ += g_Main_ಕೋಷ್ಟಕ.untagged_Conv_Array_ಗಾತ್ರ;
    g_Main_ಕೋಷ್ಟಕ.taggedAliasArray = ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ;

    /* aliasLists is a 1's based array, but it has a padding character */
    curr_ಸ್ಥಾನಾಂತರ += g_Main_ಕೋಷ್ಟಕ.tagged_Alias_Array_ಗಾತ್ರ;
    g_Main_ಕೋಷ್ಟಕ.taggedAliasLists = ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ;

    curr_ಸ್ಥಾನಾಂತರ += g_Main_ಕೋಷ್ಟಕ.tagged_Alias_Lists_ಗಾತ್ರ;
    ಆದರೆ (g_Main_ಕೋಷ್ಟಕ.option_ಕೋಷ್ಟಕ_ಗಾತ್ರ > 0
        && reinterpret_cast<ಸ್ಥಿರಾಂಕ UConverterAliasOptions*>(ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ)->stringNormalizationType < UCNV_IO_NORM_TYPE_ಎಣಿಕೆ)
    {
        /* Faster table */
        g_Main_ಕೋಷ್ಟಕ.option_ಕೋಷ್ಟಕ = reinterpret_cast<ಸ್ಥಿರಾಂಕ UConverterAliasOptions*>(ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        /* Smaller table, or I can't handle this normalization mode!
        Use the original slower table lookup. */
        g_Main_ಕೋಷ್ಟಕ.option_ಕೋಷ್ಟಕ = &default_ಕೋಷ್ಟಕ_Options;
    }

    curr_ಸ್ಥಾನಾಂತರ += g_Main_ಕೋಷ್ಟಕ.option_ಕೋಷ್ಟಕ_ಗಾತ್ರ;
    g_Main_ಕೋಷ್ಟಕ.string_ಕೋಷ್ಟಕ = ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ;

    curr_ಸ್ಥಾನಾಂತರ += g_Main_ಕೋಷ್ಟಕ.string_ಕೋಷ್ಟಕ_ಗಾತ್ರ;
    g_Main_ಕೋಷ್ಟಕ.normalized_String_ಕೋಷ್ಟಕ = ((g_Main_ಕೋಷ್ಟಕ.option_ಕೋಷ್ಟಕ->stringNormalizationType == UCNV_IO_UNNORMALIZED)
        ? g_Main_ಕೋಷ್ಟಕ.string_ಕೋಷ್ಟಕ : (ಕೋಷ್ಟಕ + curr_ಸ್ಥಾನಾಂತರ));

    ಹಿಂತಿರುಗಿ;

invalidFormat:
    err_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
    udata_close(ದತ್ತಾಂಶ);
}


ಸ್ಥಿರ UBool
have_Alias_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    umtx_initOnce(g_Alias_ದತ್ತಾಂಶ_Init_Once, &init_Alias_ದತ್ತಾಂಶ, *p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(*p_Error_ಸಂಕೇತ);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
isAlias(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(alias==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ *alias != 0;
}

ಸ್ಥಿರ uint32_t get_ಟ್ಯಾಗ್_ಸಂಖ್ಯೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tagname) {
    ಆದರೆ (g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List) {
        uint32_t ಟ್ಯಾಗ್_ಅಂಕೆ;
        ಗಾಗಿ (ಟ್ಯಾಗ್_ಅಂಕೆ = 0; ಟ್ಯಾಗ್_ಅಂಕೆ < g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ; ಟ್ಯಾಗ್_ಅಂಕೆ++) {
            ಆದರೆ (!uprv_stricmp(GET_STRING(g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List[ಟ್ಯಾಗ್_ಅಂಕೆ]), tagname)) {
                ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್_ಅಂಕೆ;
            }
        }
    }

    ಹಿಂತಿರುಗಿ UINT32_MAX;
}

/* character types relevant for ucnv_compareNames() */
ಎಣಿಕೆ {
    UIGNORE,
    ZERO,
    NONZERO,
    MINLETTER /* any values from here on are lowercase letter mappings */
};

/* character types for ASCII 00..7F */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t asciiTypes[128] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0,
    0, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0, 0, 0, 0, 0
};

#ವ್ಯಾಖ್ಯಾನಿಸು GET_ASCII_TYPE(c) ((int8_t)(c) >= 0 ? asciiTypes[(uint8_t)c] : (uint8_t)UIGNORE)

/* character types for EBCDIC 80..FF */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t ebcdicTypes[128] = {
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0, 0, 0, 0, 0, 0,
    0,    0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0, 0, 0, 0, 0, 0,
    0,    0,    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0, 0, 0, 0, 0, 0,
    ZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, NONZERO, 0, 0, 0, 0, 0, 0
};

#ವ್ಯಾಖ್ಯಾನಿಸು GET_EBCDIC_TYPE(c) ((int8_t)(c) < 0 ? ebcdicTypes[(c)&0x7f] : (uint8_t)UIGNORE)

#ಆದರೆ U_CHARSET_FAMILY==U_ASCII_FAMILY
#   define GET_CHAR_TYPE(c) GET_ASCII_TYPE(c)
#ಅಥವಾಆದರೆ U_CHARSET_FAMILY==U_EBCDIC_FAMILY
#   define GET_CHAR_TYPE(c) GET_EBCDIC_TYPE(c)
#ಇಲ್ಲದಿದ್ದರೆ
#   error U_CHARSET_FAMILY is not valid
#ಆದರೆ_ಅಂತ್ಯ


/* @see ucnv_compareNames */
U_CAPI ಅಕ್ಷರ* U_CALLCONV
ucnv_io_stripASCIIForCompare(ಅಕ್ಷರ* dst U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name) {
    ಅಕ್ಷರ *dstItr = dst;
    uint8_t type, ಮುಂದಿನ_Type;
    ಅಕ್ಷರ c1;
    UBool afterDigit = ಸುಳ್ಳು;

    ಆಗ ((c1 = *name++) != 0) {
        type = GET_ASCII_TYPE(c1);
        ಆಯ್ಕೆ (type) {
        ಸ್ಥಿತಿ UIGNORE:
            afterDigit = ಸುಳ್ಳು;
            ಮುಂದುವರಿ; /* ignore all but letters and digits */
        ಸ್ಥಿತಿ ZERO:
            ಆದರೆ (!afterDigit) {
                ಮುಂದಿನ_Type = GET_ASCII_TYPE(*name);
                ಆದರೆ (ಮುಂದಿನ_Type == ZERO || ಮುಂದಿನ_Type == NONZERO) {
                    ಮುಂದುವರಿ; /* ignore leading zero before another digit */
                }
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ NONZERO:
            afterDigit = ನಿಜ;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            c1 = (ಅಕ್ಷರ)type; /* lowercased letter */
            afterDigit = ಸುಳ್ಳು;
            ನಿಲ್ಲಿಸು;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ಹಿಂತಿರುಗಿ dst;
}

U_CAPI ಅಕ್ಷರ* U_CALLCONV
ucnv_io_stripEBCDICForCompare(ಅಕ್ಷರ* dst U_LIFETIME_BOUND, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name) {
    ಅಕ್ಷರ *dstItr = dst;
    uint8_t type, ಮುಂದಿನ_Type;
    ಅಕ್ಷರ c1;
    UBool afterDigit = ಸುಳ್ಳು;

    ಆಗ ((c1 = *name++) != 0) {
        type = GET_EBCDIC_TYPE(c1);
        ಆಯ್ಕೆ (type) {
        ಸ್ಥಿತಿ UIGNORE:
            afterDigit = ಸುಳ್ಳು;
            ಮುಂದುವರಿ; /* ignore all but letters and digits */
        ಸ್ಥಿತಿ ZERO:
            ಆದರೆ (!afterDigit) {
                ಮುಂದಿನ_Type = GET_EBCDIC_TYPE(*name);
                ಆದರೆ (ಮುಂದಿನ_Type == ZERO || ಮುಂದಿನ_Type == NONZERO) {
                    ಮುಂದುವರಿ; /* ignore leading zero before another digit */
                }
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ NONZERO:
            afterDigit = ನಿಜ;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            c1 = (ಅಕ್ಷರ)type; /* lowercased letter */
            afterDigit = ಸುಳ್ಳು;
            ನಿಲ್ಲಿಸು;
        }
        *dstItr++ = c1;
    }
    *dstItr = 0;
    ಹಿಂತಿರುಗಿ dst;
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
U_CAPI ಪೂರ್ಣಾಂಕ U_EXPORT2
ucnv_compare_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name2) {
    ಪೂರ್ಣಾಂಕ rc;
    uint8_t type, ಮುಂದಿನ_Type;
    ಅಕ್ಷರ c1, c2;
    UBool afterDigit1 = ಸುಳ್ಳು, afterDigit2 = ಸುಳ್ಳು;

    ಗಾಗಿ (;;) {
        ಆಗ ((c1 = *name1++) != 0) {
            type = GET_CHAR_TYPE(c1);
            ಆಯ್ಕೆ (type) {
            ಸ್ಥಿತಿ UIGNORE:
                afterDigit1 = ಸುಳ್ಳು;
                ಮುಂದುವರಿ; /* ignore all but letters and digits */
            ಸ್ಥಿತಿ ZERO:
                ಆದರೆ (!afterDigit1) {
                    ಮುಂದಿನ_Type = GET_CHAR_TYPE(*name1);
                    ಆದರೆ (ಮುಂದಿನ_Type == ZERO || ಮುಂದಿನ_Type == NONZERO) {
                        ಮುಂದುವರಿ; /* ignore leading zero before another digit */
                    }
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ NONZERO:
                afterDigit1 = ನಿಜ;
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                c1 = (ಅಕ್ಷರ)type; /* lowercased letter */
                afterDigit1 = ಸುಳ್ಳು;
                ನಿಲ್ಲಿಸು;
            }
            ನಿಲ್ಲಿಸು; /* deliver c1 */
        }
        ಆಗ ((c2 = *name2++) != 0) {
            type = GET_CHAR_TYPE(c2);
            ಆಯ್ಕೆ (type) {
            ಸ್ಥಿತಿ UIGNORE:
                afterDigit2 = ಸುಳ್ಳು;
                ಮುಂದುವರಿ; /* ignore all but letters and digits */
            ಸ್ಥಿತಿ ZERO:
                ಆದರೆ (!afterDigit2) {
                    ಮುಂದಿನ_Type = GET_CHAR_TYPE(*name2);
                    ಆದರೆ (ಮುಂದಿನ_Type == ZERO || ಮುಂದಿನ_Type == NONZERO) {
                        ಮುಂದುವರಿ; /* ignore leading zero before another digit */
                    }
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ NONZERO:
                afterDigit2 = ನಿಜ;
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                c2 = (ಅಕ್ಷರ)type; /* lowercased letter */
                afterDigit2 = ಸುಳ್ಳು;
                ನಿಲ್ಲಿಸು;
            }
            ನಿಲ್ಲಿಸು; /* deliver c2 */
        }

        /* If we reach the ends of both strings then they match */
        ಆದರೆ ((c1|c2)==0) {
            ಹಿಂತಿರುಗಿ 0;
        }

        /* Case-insensitive comparison */
        rc = (ಪೂರ್ಣಾಂಕ)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)c1 - (ಪೂರ್ಣಾಂಕ)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)c2;
        ಆದರೆ (rc != 0) {
            ಹಿಂತಿರುಗಿ rc;
        }
    }
}

/*
 * search for an alias
 * return the converter number index for gConverterList
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t
ಕಂಡುಹಿಡಿ_Converter(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, UBool *ಒಳಗೊಂಡಿದೆ_Option, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t mid, ಆರಂಭ, ಎಲ್ಲೆ;
    uint32_t ಕೊನೆಯ_Mid;
    ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;
    ಪೂರ್ಣಾಂಕ isUnnormalized = (g_Main_ಕೋಷ್ಟಕ.option_ಕೋಷ್ಟಕ->stringNormalizationType == UCNV_IO_UNNORMALIZED);
    ಅಕ್ಷರ strippedName[UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ];

    ಆದರೆ (!isUnnormalized) {
        ಆದರೆ (uprv_strlen(alias) >= UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ) {
            *p_Error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ಹಿಂತಿರುಗಿ UINT32_MAX;
        }

        /* Lower case and remove ignoreable characters. */
        ucnv_io_stripForCompare(strippedName, alias);
        alias = strippedName;
    }

    /* do a binary search for the alias */
    ಆರಂಭ = 0;
    ಎಲ್ಲೆ = g_Main_ಕೋಷ್ಟಕ.untagged_Conv_Array_ಗಾತ್ರ;
    mid = ಎಲ್ಲೆ;
    ಕೊನೆಯ_Mid = UINT32_MAX;

    ಗಾಗಿ (;;) {
        mid = (ಆರಂಭ + ಎಲ್ಲೆ) / 2;
        ಆದರೆ (ಕೊನೆಯ_Mid == mid) {   /* Have we moved? */
            ನಿಲ್ಲಿಸು;  /* We haven't moved, and it wasn't found. */
        }
        ಕೊನೆಯ_Mid = mid;
        ಆದರೆ (isUnnormalized) {
            ಫಲಿತಾಂಶ = ucnv_compare_ಹೆಸರುಗಳು(alias, GET_STRING(g_Main_ಕೋಷ್ಟಕ.aliasList[mid]));
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ಫಲಿತಾಂಶ = uprv_strcmp(alias, GET_NORMALIZED_STRING(g_Main_ಕೋಷ್ಟಕ.aliasList[mid]));
        }

        ಆದರೆ (ಫಲಿತಾಂಶ < 0) {
            ಎಲ್ಲೆ = mid;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ > 0) {
            ಆರಂಭ = mid;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* Since the gencnval tool folds duplicates into one entry,
             * this alias in gAliasList is unique, but different standards
             * may map an alias to different converters.
             */
            ಆದರೆ (g_Main_ಕೋಷ್ಟಕ.untaggedConvArray[mid] & UCNV_AMBIGUOUS_ALIAS_MAP_ಬಿಟ್) {
                *p_Error_ಸಂಕೇತ = U_AMBIGUOUS_ALIAS_WARNING;
            }
            /* State whether the canonical converter name contains an option.
            This information is contained in this list in order to maintain backward & forward compatibility. */
            ಆದರೆ (ಒಳಗೊಂಡಿದೆ_Option) {
                UBool ಒಳಗೊಂಡಿದೆ_Cnv_Option_Info = static_cast<UBool>(g_Main_ಕೋಷ್ಟಕ.option_ಕೋಷ್ಟಕ->ಒಳಗೊಂಡಿದೆ_Cnv_Option_Info);
                *ಒಳಗೊಂಡಿದೆ_Option = static_cast<UBool>((ಒಳಗೊಂಡಿದೆ_Cnv_Option_Info
                    && ((g_Main_ಕೋಷ್ಟಕ.untaggedConvArray[mid] & UCNV_ಒಳಗೊಂಡಿದೆ_OPTION_ಬಿಟ್) != 0))
                    || !ಒಳಗೊಂಡಿದೆ_Cnv_Option_Info);
            }
            ಹಿಂತಿರುಗಿ g_Main_ಕೋಷ್ಟಕ.untaggedConvArray[mid] & UCNV_CONVERTER_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ;
        }
    }

    ಹಿಂತಿರುಗಿ UINT32_MAX;
}

/*
 * Is this alias in this list?
 * alias and listOffset should be non-nullptr.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
is_Alias_ಒಳ_List(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, uint32_t list_ಸ್ಥಾನಾಂತರ) {
    ಆದರೆ (list_ಸ್ಥಾನಾಂತರ) {
        uint32_t currAlias;
        uint32_t list_ಎಣಿಕೆ = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists[list_ಸ್ಥಾನಾಂತರ];
        /* +1 to skip listCount */
        ಸ್ಥಿರಾಂಕ uint16_t *currList = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists + list_ಸ್ಥಾನಾಂತರ + 1;
        ಗಾಗಿ (currAlias = 0; currAlias < list_ಎಣಿಕೆ; currAlias++) {
            ಆದರೆ (currList[currAlias]
                && ucnv_compare_ಹೆಸರುಗಳು(alias, GET_STRING(currList[currAlias]))==0)
            {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/*
 * Search for an standard name of an alias (what is the default name
 * that this standard uses?)
 * return the listOffset for gTaggedAliasLists. If it's 0,
 * the it couldn't be found, but the parameters are valid.
 */
ಸ್ಥಿರ uint32_t
ಕಂಡುಹಿಡಿ_Tagged_Alias_Lists_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *standard, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t idx;
    uint32_t list_ಸ್ಥಾನಾಂತರ;
    uint32_t conv_ಅಂಕೆ;
    UError_ಸಂಕೇತ myErr = U_ZERO_ERROR;
    uint32_t ಟ್ಯಾಗ್_ಅಂಕೆ = get_ಟ್ಯಾಗ್_ಸಂಖ್ಯೆ(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_ಅಂಕೆ = ಕಂಡುಹಿಡಿ_Converter(alias, ಶೂನ್ಯಸೂಚಕ, &myErr);
    ಆದರೆ (myErr != U_ZERO_ERROR) {
        *p_Error_ಸಂಕೇತ = myErr;
    }

    ಆದರೆ (ಟ್ಯಾಗ್_ಅಂಕೆ < (g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ - UCNV_ಅಂಕೆ_HIDDEN_ಟ್ಯಾಗುಗಳು) && conv_ಅಂಕೆ < g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ) {
        list_ಸ್ಥಾನಾಂತರ = g_Main_ಕೋಷ್ಟಕ.taggedAliasArray[ಟ್ಯಾಗ್_ಅಂಕೆ*g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ + conv_ಅಂಕೆ];
        ಆದರೆ (list_ಸ್ಥಾನಾಂತರ && g_Main_ಕೋಷ್ಟಕ.taggedAliasLists[list_ಸ್ಥಾನಾಂತರ + 1]) {
            ಹಿಂತಿರುಗಿ list_ಸ್ಥಾನಾಂತರ;
        }
        ಆದರೆ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search the whole swiss cheese starting
               at the highest standard affinity.
               This may take a while.
            */
            ಗಾಗಿ (idx = 0; idx < g_Main_ಕೋಷ್ಟಕ.tagged_Alias_Array_ಗಾತ್ರ; idx++) {
                list_ಸ್ಥಾನಾಂತರ = g_Main_ಕೋಷ್ಟಕ.taggedAliasArray[idx];
                ಆದರೆ (list_ಸ್ಥಾನಾಂತರ && is_Alias_ಒಳ_List(alias, list_ಸ್ಥಾನಾಂತರ)) {
                    uint32_t curr_ಟ್ಯಾಗ್_ಅಂಕೆ = idx/g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ;
                    uint32_t curr_Conv_ಅಂಕೆ = (idx - curr_ಟ್ಯಾಗ್_ಅಂಕೆ*g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ);
                    uint32_t ತಾತ್ಕಾಲಿಕ_List_ಸ್ಥಾನಾಂತರ = g_Main_ಕೋಷ್ಟಕ.taggedAliasArray[ಟ್ಯಾಗ್_ಅಂಕೆ*g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ + curr_Conv_ಅಂಕೆ];
                    ಆದರೆ (ತಾತ್ಕಾಲಿಕ_List_ಸ್ಥಾನಾಂತರ && g_Main_ಕೋಷ್ಟಕ.taggedAliasLists[ತಾತ್ಕಾಲಿಕ_List_ಸ್ಥಾನಾಂತರ + 1]) {
                        ಹಿಂತಿರುಗಿ ತಾತ್ಕಾಲಿಕ_List_ಸ್ಥಾನಾಂತರ;
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
        ಹಿಂತಿರುಗಿ 0;
    }
    /* else converter or tag not found */

    ಹಿಂತಿರುಗಿ UINT32_MAX;
}

/* Return the canonical name */
ಸ್ಥಿರ uint32_t
ಕಂಡುಹಿಡಿ_Tagged_Converter_ಅಂಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *standard, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint32_t idx;
    uint32_t list_ಸ್ಥಾನಾಂತರ;
    uint32_t conv_ಅಂಕೆ;
    UError_ಸಂಕೇತ myErr = U_ZERO_ERROR;
    uint32_t ಟ್ಯಾಗ್_ಅಂಕೆ = get_ಟ್ಯಾಗ್_ಸಂಖ್ಯೆ(standard);

    /* Make a quick guess. Hopefully they used a TR22 canonical alias. */
    conv_ಅಂಕೆ = ಕಂಡುಹಿಡಿ_Converter(alias, ಶೂನ್ಯಸೂಚಕ, &myErr);
    ಆದರೆ (myErr != U_ZERO_ERROR) {
        *p_Error_ಸಂಕೇತ = myErr;
    }

    ಆದರೆ (ಟ್ಯಾಗ್_ಅಂಕೆ < (g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ - UCNV_ಅಂಕೆ_HIDDEN_ಟ್ಯಾಗುಗಳು) && conv_ಅಂಕೆ < g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ) {
        list_ಸ್ಥಾನಾಂತರ = g_Main_ಕೋಷ್ಟಕ.taggedAliasArray[ಟ್ಯಾಗ್_ಅಂಕೆ*g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ + conv_ಅಂಕೆ];
        ಆದರೆ (list_ಸ್ಥಾನಾಂತರ && is_Alias_ಒಳ_List(alias, list_ಸ್ಥಾನಾಂತರ)) {
            ಹಿಂತಿರುಗಿ conv_ಅಂಕೆ;
        }
        ಆದರೆ (myErr == U_AMBIGUOUS_ALIAS_WARNING) {
            /* Uh Oh! They used an ambiguous alias.
               We have to search one slice of the swiss cheese.
               We search only in the requested tag, not the whole thing.
               This may take a while.
            */
            uint32_t conv_ಆರಂಭ = (ಟ್ಯಾಗ್_ಅಂಕೆ)*g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ;
            uint32_t conv_ಎಲ್ಲೆ = (ಟ್ಯಾಗ್_ಅಂಕೆ+1)*g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ;
            ಗಾಗಿ (idx = conv_ಆರಂಭ; idx < conv_ಎಲ್ಲೆ; idx++) {
                list_ಸ್ಥಾನಾಂತರ = g_Main_ಕೋಷ್ಟಕ.taggedAliasArray[idx];
                ಆದರೆ (list_ಸ್ಥಾನಾಂತರ && is_Alias_ಒಳ_List(alias, list_ಸ್ಥಾನಾಂತರ)) {
                    ಹಿಂತಿರುಗಿ idx-conv_ಆರಂಭ;
                }
            }
            /* The standard doesn't know about the alias */
        }
        /* else no canonical name */
    }
    /* else converter or tag not found */

    ಹಿಂತಿರುಗಿ UINT32_MAX;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
ucnv_io_getConverterName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, UBool *ಒಳಗೊಂಡಿದೆ_Option, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *aliasTmp = alias;
    int32_t i = 0;
    ಗಾಗಿ (i = 0; i < 2; i++) {
        ಆದರೆ (i == 1) {
            /*
             * After the first unsuccess converter lookup, check to see if
             * the name begins with 'x-'. If it does, strip it off and try
             * again.  This behaviour is similar to how ICU4J does it.
             */
            ಆದರೆ (aliasTmp[0] == 'x' && aliasTmp[1] == '-') {
                aliasTmp = aliasTmp+2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        }
        ಆದರೆ(have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ) && isAlias(aliasTmp, p_Error_ಸಂಕೇತ)) {
            uint32_t conv_ಅಂಕೆ = ಕಂಡುಹಿಡಿ_Converter(aliasTmp, ಒಳಗೊಂಡಿದೆ_Option, p_Error_ಸಂಕೇತ);
            ಆದರೆ (conv_ಅಂಕೆ < g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ) {
                ಹಿಂತಿರುಗಿ GET_STRING(g_Main_ಕೋಷ್ಟಕ.converterList[conv_ಅಂಕೆ]);
            }
            /* else converter not found */
        } ಇಲ್ಲದಿದ್ದರೆ {
            ನಿಲ್ಲಿಸು;
        }
    }

    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CDECL_BEGIN


ಸ್ಥಿರ int32_t U_CALLCONV
ucnv_io_ಎಣಿಕೆ_Standard_Aliases(UEnumeration *enumerator, UError_ಸಂಕೇತ * /*pErrorCode*/) {
    int32_t value = 0;
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_ಸ್ಥಾನಾಂತರ = myContext->list_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (list_ಸ್ಥಾನಾಂತರ) {
        value = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists[list_ಸ್ಥಾನಾಂತರ];
    }
    ಹಿಂತಿರುಗಿ value;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_CALLCONV
ucnv_io_ಮುಂದಿನ_Standard_Aliases(UEnumeration *enumerator,
                            int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                            UError_ಸಂಕೇತ * /*pErrorCode*/)
{
    UAliasContext *myContext = (UAliasContext *)(enumerator->context);
    uint32_t list_ಸ್ಥಾನಾಂತರ = myContext->list_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (list_ಸ್ಥಾನಾಂತರ) {
        uint32_t list_ಎಣಿಕೆ = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists[list_ಸ್ಥಾನಾಂತರ];
        ಸ್ಥಿರಾಂಕ uint16_t *currList = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists + list_ಸ್ಥಾನಾಂತರ + 1;

        ಆದರೆ (myContext->listIdx < list_ಎಣಿಕೆ) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *myStr = GET_STRING(currList[myContext->listIdx++]);
            ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
                *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = (int32_t)uprv_strlen(myStr);
            }
            ಹಿಂತಿರುಗಿ myStr;
        }
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
        *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = 0;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_io_resetStandardAliases(UEnumeration *enumerator, UError_ಸಂಕೇತ * /*pErrorCode*/) {
    ((UAliasContext *)(enumerator->context))->listIdx = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_io_closeUEnumeration(UEnumeration *enumerator) {
    uprv_free(enumerator->context);
    uprv_free(enumerator);
}

U_CDECL_ಕೊನೆ

/* Enumerate the aliases for the specified converter and standard tag */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration gEnumAliases = {
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_io_closeUEnumeration,
    ucnv_io_ಎಣಿಕೆ_Standard_Aliases,
    uenum_unextDefault,
    ucnv_io_ಮುಂದಿನ_Standard_Aliases,
    ucnv_io_resetStandardAliases
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_Standard_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *convName,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *standard,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    UEnumeration *myEnum = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ) && isAlias(convName, p_Error_ಸಂಕೇತ)) {
        uint32_t list_ಸ್ಥಾನಾಂತರ = ಕಂಡುಹಿಡಿ_Tagged_Alias_Lists_ಸ್ಥಾನಾಂತರ(convName, standard, p_Error_ಸಂಕೇತ);

        /* When listOffset == 0, we want to acknowledge that the
           converter name and standard are okay, but there
           is nothing to enumerate. */
        ಆದರೆ (list_ಸ್ಥಾನಾಂತರ < g_Main_ಕೋಷ್ಟಕ.tagged_Alias_Lists_ಗಾತ್ರ) {
            UAliasContext *myContext;

            myEnum = static_cast<UEnumeration *>(uprv_malloc(ಗಾತ್ರ(UEnumeration)));
            ಆದರೆ (myEnum == ಶೂನ್ಯಸೂಚಕ) {
                *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            uprv_memcpy(myEnum, &gEnumAliases, ಗಾತ್ರ(UEnumeration));
            myContext = static_cast<UAliasContext *>(uprv_malloc(ಗಾತ್ರ(UAliasContext)));
            ಆದರೆ (myContext == ಶೂನ್ಯಸೂಚಕ) {
                *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                uprv_free(myEnum);
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            myContext->list_ಸ್ಥಾನಾಂತರ = list_ಸ್ಥಾನಾಂತರ;
            myContext->listIdx = 0;
            myEnum->context = myContext;
        }
        /* else converter or tag not found */
    }
    ಹಿಂತಿರುಗಿ myEnum;
}

ಸ್ಥಿರ uint16_t
ucnv_io_ಎಣಿಕೆ_Aliases(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ) && isAlias(alias, p_Error_ಸಂಕೇತ)) {
        uint32_t conv_ಅಂಕೆ = ಕಂಡುಹಿಡಿ_Converter(alias, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ);
        ಆದರೆ (conv_ಅಂಕೆ < g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_ಸ್ಥಾನಾಂತರ = g_Main_ಕೋಷ್ಟಕ.taggedAliasArray[(g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ - 1)*g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ + conv_ಅಂಕೆ];

            ಆದರೆ (list_ಸ್ಥಾನಾಂತರ) {
                ಹಿಂತಿರುಗಿ g_Main_ಕೋಷ್ಟಕ.taggedAliasLists[list_ಸ್ಥಾನಾಂತರ];
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ uint16_t
ucnv_io_getAliases(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, uint16_t ಆರಂಭ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **aliases, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ) && isAlias(alias, p_Error_ಸಂಕೇತ)) {
        uint32_t currAlias;
        uint32_t conv_ಅಂಕೆ = ಕಂಡುಹಿಡಿ_Converter(alias, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ);
        ಆದರೆ (conv_ಅಂಕೆ < g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_ಸ್ಥಾನಾಂತರ = g_Main_ಕೋಷ್ಟಕ.taggedAliasArray[(g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ - 1)*g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ + conv_ಅಂಕೆ];

            ಆದರೆ (list_ಸ್ಥಾನಾಂತರ) {
                uint32_t list_ಎಣಿಕೆ = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists[list_ಸ್ಥಾನಾಂತರ];
                /* +1 to skip listCount */
                ಸ್ಥಿರಾಂಕ uint16_t *currList = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists + list_ಸ್ಥಾನಾಂತರ + 1;

                ಗಾಗಿ (currAlias = ಆರಂಭ; currAlias < list_ಎಣಿಕೆ; currAlias++) {
                    aliases[currAlias] = GET_STRING(currList[currAlias]);
                }
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *
ucnv_io_getAlias(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, uint16_t n, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ) && isAlias(alias, p_Error_ಸಂಕೇತ)) {
        uint32_t conv_ಅಂಕೆ = ಕಂಡುಹಿಡಿ_Converter(alias, ಶೂನ್ಯಸೂಚಕ, p_Error_ಸಂಕೇತ);
        ಆದರೆ (conv_ಅಂಕೆ < g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ) {
            /* tagListNum - 1 is the ALL tag */
            int32_t list_ಸ್ಥಾನಾಂತರ = g_Main_ಕೋಷ್ಟಕ.taggedAliasArray[(g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ - 1)*g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ + conv_ಅಂಕೆ];

            ಆದರೆ (list_ಸ್ಥಾನಾಂತರ) {
                uint32_t list_ಎಣಿಕೆ = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists[list_ಸ್ಥಾನಾಂತರ];
                /* +1 to skip listCount */
                ಸ್ಥಿರಾಂಕ uint16_t *currList = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists + list_ಸ್ಥಾನಾಂತರ + 1;

                ಆದರೆ (n < list_ಎಣಿಕೆ)  {
                    ಹಿಂತಿರುಗಿ GET_STRING(currList[n]);
                }
                *p_Error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            }
            /* else this shouldn't happen. internal program error */
        }
        /* else converter not found */
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರ uint16_t
ucnv_io_ಎಣಿಕೆ_Standards(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ)) {
        /* Don't include the empty list */
        ಹಿಂತಿರುಗಿ static_cast<uint16_t>(g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ - UCNV_ಅಂಕೆ_HIDDEN_ಟ್ಯಾಗುಗಳು);
    }

    ಹಿಂತಿರುಗಿ 0;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_EXPORT2
ucnv_getStandard(uint16_t n, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ)) {
        ಆದರೆ (n < g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List_ಗಾತ್ರ - UCNV_ಅಂಕೆ_HIDDEN_ಟ್ಯಾಗುಗಳು) {
            ಹಿಂತಿರುಗಿ GET_STRING(g_Main_ಕೋಷ್ಟಕ.ಟ್ಯಾಗ್_List[n]);
        }
        *p_Error_ಸಂಕೇತ = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    }

    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_EXPORT2
ucnv_getStandardName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *standard, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ) && isAlias(alias, p_Error_ಸಂಕೇತ)) {
        uint32_t list_ಸ್ಥಾನಾಂತರ = ಕಂಡುಹಿಡಿ_Tagged_Alias_Lists_ಸ್ಥಾನಾಂತರ(alias, standard, p_Error_ಸಂಕೇತ);

        ಆದರೆ (0 < list_ಸ್ಥಾನಾಂತರ && list_ಸ್ಥಾನಾಂತರ < g_Main_ಕೋಷ್ಟಕ.tagged_Alias_Lists_ಗಾತ್ರ) {
            ಸ್ಥಿರಾಂಕ uint16_t *currList = g_Main_ಕೋಷ್ಟಕ.taggedAliasLists + list_ಸ್ಥಾನಾಂತರ + 1;

            /* Get the preferred name from this list */
            ಆದರೆ (currList[0]) {
                ಹಿಂತಿರುಗಿ GET_STRING(currList[0]);
            }
            /* else someone screwed up the alias table. */
            /* *pErrorCode = U_INVALID_FORMAT_ERROR */
        }
    }

    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI uint16_t U_EXPORT2
ucnv_ಎಣಿಕೆ_Aliases(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಹಿಂತಿರುಗಿ ucnv_io_ಎಣಿಕೆ_Aliases(alias, p_Error_ಸಂಕೇತ);
}


U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
ucnv_getAlias(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, uint16_t n, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಹಿಂತಿರುಗಿ ucnv_io_getAlias(alias, n, p_Error_ಸಂಕೇತ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_getAliases(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **aliases, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ucnv_io_getAliases(alias, 0, aliases, p_Error_ಸಂಕೇತ);
}

U_CAPI uint16_t U_EXPORT2
ucnv_ಎಣಿಕೆ_Standards()
{
    UError_ಸಂಕೇತ err = U_ZERO_ERROR;
    ಹಿಂತಿರುಗಿ ucnv_io_ಎಣಿಕೆ_Standards(&err);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_EXPORT2
ucnv_getCanonicalName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *standard, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ) && isAlias(alias, p_Error_ಸಂಕೇತ)) {
        uint32_t conv_ಅಂಕೆ = ಕಂಡುಹಿಡಿ_Tagged_Converter_ಅಂಕೆ(alias, standard, p_Error_ಸಂಕೇತ);

        ಆದರೆ (conv_ಅಂಕೆ < g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ) {
            ಹಿಂತಿರುಗಿ GET_STRING(g_Main_ಕೋಷ್ಟಕ.converterList[conv_ಅಂಕೆ]);
        }
    }

    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CDECL_BEGIN


ಸ್ಥಿರ int32_t U_CALLCONV
ucnv_io_ಎಣಿಕೆ_All_Converters(UEnumeration * /*enumerator*/, UError_ಸಂಕೇತ * /*pErrorCode*/) {
    ಹಿಂತಿರುಗಿ g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_CALLCONV
ucnv_io_ಮುಂದಿನ_All_Converters(UEnumeration *enumerator,
                            int32_t* ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ,
                            UError_ಸಂಕೇತ * /*pErrorCode*/)
{
    uint16_t *myContext = (uint16_t *)(enumerator->context);

    ಆದರೆ (*myContext < g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *myStr = GET_STRING(g_Main_ಕೋಷ್ಟಕ.converterList[(*myContext)++]);
        ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
            *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = (int32_t)uprv_strlen(myStr);
        }
        ಹಿಂತಿರುಗಿ myStr;
    }
    /* Either we accessed a zero length list, or we enumerated too far. */
    ಆದರೆ (ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ) {
        *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = 0;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucnv_io_resetAllConverters(UEnumeration *enumerator, UError_ಸಂಕೇತ * /*pErrorCode*/) {
    *((uint16_t *)(enumerator->context)) = 0;
}
U_CDECL_ಕೊನೆ
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UEnumeration gEnumAllConverters = {
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    ucnv_io_closeUEnumeration,
    ucnv_io_ಎಣಿಕೆ_All_Converters,
    uenum_unextDefault,
    ucnv_io_ಮುಂದಿನ_All_Converters,
    ucnv_io_resetAllConverters
};

U_CAPI UEnumeration * U_EXPORT2
ucnv_open_All_ಹೆಸರುಗಳು(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UEnumeration *myEnum = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ)) {
        uint16_t *myContext;

        myEnum = static_cast<UEnumeration *>(uprv_malloc(ಗಾತ್ರ(UEnumeration)));
        ಆದರೆ (myEnum == ಶೂನ್ಯಸೂಚಕ) {
            *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        uprv_memcpy(myEnum, &gEnumAllConverters, ಗಾತ್ರ(UEnumeration));
        myContext = static_cast<uint16_t *>(uprv_malloc(ಗಾತ್ರ(uint16_t)));
        ಆದರೆ (myContext == ಶೂನ್ಯಸೂಚಕ) {
            *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            uprv_free(myEnum);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        *myContext = 0;
        myEnum->context = myContext;
    }
    ಹಿಂತಿರುಗಿ myEnum;
}

U_CAPI uint16_t
ucnv_io_ಎಣಿಕೆ_ತಿಳಿದ_Converters(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ (have_Alias_ದತ್ತಾಂಶ(p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ (uint16_t)g_Main_ಕೋಷ್ಟಕ.converter_List_ಗಾತ್ರ;
    }
    ಹಿಂತಿರುಗಿ 0;
}

/* alias table swapping ----------------------------------------------------- */

U_CDECL_BEGIN

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಅಕ್ಷರ * U_CALLCONV StripForCompareFn(ಅಕ್ಷರ *dst, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
U_CDECL_ಕೊನೆ


/*
 * row of a temporary array
 *
 * gets platform-endian charset string indexes and sorting indexes;
 * after sorting this array by strings, the actual arrays are permutated
 * according to the sorting indexes
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ತಾತ್ಕಾಲಿಕ_ಅಡ್ಡಸಾಲು {
    uint16_t str_ಸೂಚ್ಯಂಕ, ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ;
} ತಾತ್ಕಾಲಿಕ_ಅಡ್ಡಸಾಲು;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ತಾತ್ಕಾಲಿಕ_Alias_ಕೋಷ್ಟಕ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chars;
    ತಾತ್ಕಾಲಿಕ_ಅಡ್ಡಸಾಲು *ಅಡ್ಡಸಾಲುಗಳು;
    uint16_t *resort;
    StripForCompareFn *stripForCompare;
} ತಾತ್ಕಾಲಿಕ_Alias_ಕೋಷ್ಟಕ;

ಎಣಿಕೆ {
    ರಾಶಿ_ಅಡ್ಡಸಾಲು_ಸಾಮರ್ಥ್ಯ=500
};

ಸ್ಥಿರ int32_t U_CALLCONV
io_compare_ಅಡ್ಡಸಾಲುಗಳು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    ಅಕ್ಷರ stripped_ಎಡ[UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ],
         stripped_ಬಲ[UCNV_MAX_CONVERTER_NAME_ಉದ್ದಳತೆ];

    ತಾತ್ಕಾಲಿಕ_Alias_ಕೋಷ್ಟಕ *ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ=(ತಾತ್ಕಾಲಿಕ_Alias_ಕೋಷ್ಟಕ *)context;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *chars=ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->chars;

    ಹಿಂತಿರುಗಿ static_cast<int32_t>(uprv_strcmp(
        ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->stripForCompare(stripped_ಎಡ, chars + 2 * static_cast<ಸ್ಥಿರಾಂಕ ತಾತ್ಕಾಲಿಕ_ಅಡ್ಡಸಾಲು*>(ಎಡ)->str_ಸೂಚ್ಯಂಕ),
        ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->stripForCompare(stripped_ಬಲ, chars + 2 * static_cast<ಸ್ಥಿರಾಂಕ ತಾತ್ಕಾಲಿಕ_ಅಡ್ಡಸಾಲು*>(ಬಲ)->str_ಸೂಚ್ಯಂಕ)));
}

U_CAPI int32_t U_EXPORT2
ucnv_ಅದಲುಬದಲು_Aliases(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo;
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಸ್ಥಿರಾಂಕ uint16_t *ಒಳ_ಕೋಷ್ಟಕ;
    ಸ್ಥಿರಾಂಕ uint32_t *ಒಳ_Section_Sizes;
    uint32_t toc[offsets_ಎಣಿಕೆ];
    uint32_t offsets[offsets_ಎಣಿಕೆ]; /* 16-bit-addressed offsets from inTable/outTable */
    uint32_t i, count, toc_ಉದ್ದಳತೆ, ತುದಿ_ಸ್ಥಾನಾಂತರ;

    ತಾತ್ಕಾಲಿಕ_ಅಡ್ಡಸಾಲು ಅಡ್ಡಸಾಲುಗಳು[ರಾಶಿ_ಅಡ್ಡಸಾಲು_ಸಾಮರ್ಥ್ಯ];
    uint16_t resort[ರಾಶಿ_ಅಡ್ಡಸಾಲು_ಸಾಮರ್ಥ್ಯ];
    ತಾತ್ಕಾಲಿಕ_Alias_ಕೋಷ್ಟಕ ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ;

    /* udata_swapDataHeader checks the arguments */
    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* check data format and format version */
    pInfo=(ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
    ಆದರೆ(!(
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x43 &&   /* dataFormat="CvAl" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x76 &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x41 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x6c &&
        pInfo->format_ಆವೃತ್ತಿ[0]==3
    )) {
        udata_printError(ds, "ucnv_swapAliases(): data format %02x.%02x.%02x.%02x (format version %02x) is not an alias table\n",
                         pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1],
                         pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3],
                         pInfo->format_ಆವೃತ್ತಿ[0]);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* an alias table must contain at least the table of contents array */
    ಆದರೆ(length>=0 && (length-ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)<4*(1+min_Toc_ಉದ್ದಳತೆ)) {
        udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                         length-ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_Section_Sizes=(ಸ್ಥಿರಾಂಕ uint32_t *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
    ಒಳ_ಕೋಷ್ಟಕ=(ಸ್ಥಿರಾಂಕ uint16_t *)ಒಳ_Section_Sizes;
    uprv_memset(toc, 0, ಗಾತ್ರ(toc));
    toc[toc_ಉದ್ದಳತೆ_ಸೂಚ್ಯಂಕ]=toc_ಉದ್ದಳತೆ=ds->readUInt32(ಒಳ_Section_Sizes[toc_ಉದ್ದಳತೆ_ಸೂಚ್ಯಂಕ]);
    ಆದರೆ(toc_ಉದ್ದಳತೆ<min_Toc_ಉದ್ದಳತೆ || offsets_ಎಣಿಕೆ<=toc_ಉದ್ದಳತೆ) {
        udata_printError(ds, "ucnv_swapAliases(): table of contents contains unsupported number of sections (%u sections)\n", toc_ಉದ್ದಳತೆ);
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* read the known part of the table of contents */
    ಗಾಗಿ(i=converter_List_ಸೂಚ್ಯಂಕ; i<=toc_ಉದ್ದಳತೆ; ++i) {
        toc[i]=ds->readUInt32(ಒಳ_Section_Sizes[i]);
    }

    /* compute offsets */
    uprv_memset(offsets, 0, ಗಾತ್ರ(offsets));
    offsets[converter_List_ಸೂಚ್ಯಂಕ]=2*(1+toc_ಉದ್ದಳತೆ); /* count two 16-bit units per toc entry */
    ಗಾಗಿ(i=ಟ್ಯಾಗ್_List_ಸೂಚ್ಯಂಕ; i<=toc_ಉದ್ದಳತೆ; ++i) {
        offsets[i]=offsets[i-1]+toc[i-1];
    }

    /* compute the overall size of the after-header data, in numbers of 16-bit units */
    ತುದಿ_ಸ್ಥಾನಾಂತರ=offsets[i-1]+toc[i-1];

    ಆದರೆ(length>=0) {
        uint16_t *ಹೊರಹೋಗು_ಕೋಷ್ಟಕ;
        ಸ್ಥಿರಾಂಕ uint16_t *p, *p2;
        uint16_t *q, *q2;
        uint16_t old_ಸೂಚ್ಯಂಕ;

        ಆದರೆ((length-ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)<(2*(int32_t)ತುದಿ_ಸ್ಥಾನಾಂತರ)) {
            udata_printError(ds, "ucnv_swapAliases(): too few bytes (%d after header) for an alias table\n",
                             length-ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        ಹೊರಹೋಗು_ಕೋಷ್ಟಕ=(uint16_t *)((ಅಕ್ಷರ *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);

        /* swap the entire table of contents */
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಕೋಷ್ಟಕ, 4*(1+toc_ಉದ್ದಳತೆ), ಹೊರಹೋಗು_ಕೋಷ್ಟಕ, p_Error_ಸಂಕೇತ);

        /* swap unormalized strings & normalized strings */
        ds->ಅದಲುಬದಲು_Inv_Chars(ds, ಒಳ_ಕೋಷ್ಟಕ+offsets[string_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ], 2*(int32_t)(toc[string_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ]+toc[normalized_String_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ]),
                             ಹೊರಹೋಗು_ಕೋಷ್ಟಕ+offsets[string_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ], p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            udata_printError(ds, "ucnv_swapAliases().swapInvChars(charset names) failed\n");
            ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ(ds->ಒಳ_Charset==ds->ಹೊರಹೋಗು_Charset) {
            /* no need to sort, just swap all 16-bit values together */
            ds->ಅದಲುಬದಲು_Array16(ds,
                            ಒಳ_ಕೋಷ್ಟಕ+offsets[converter_List_ಸೂಚ್ಯಂಕ],
                            2*(int32_t)(offsets[string_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ]-offsets[converter_List_ಸೂಚ್ಯಂಕ]),
                            ಹೊರಹೋಗು_ಕೋಷ್ಟಕ+offsets[converter_List_ಸೂಚ್ಯಂಕ],
                            p_Error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* allocate the temporary table for sorting */
            count=toc[alias_List_ಸೂಚ್ಯಂಕ];

            ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.chars=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(ಹೊರಹೋಗು_ಕೋಷ್ಟಕ+offsets[string_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ]); /* sort by outCharset */

            ಆದರೆ(count<=ರಾಶಿ_ಅಡ್ಡಸಾಲು_ಸಾಮರ್ಥ್ಯ) {
                ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು=ಅಡ್ಡಸಾಲುಗಳು;
                ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resort=resort;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು=(ತಾತ್ಕಾಲಿಕ_ಅಡ್ಡಸಾಲು *)uprv_malloc(count*ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ_ಅಡ್ಡಸಾಲು)+count*2);
                ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು==ಶೂನ್ಯಸೂಚಕ) {
                    udata_printError(ds, "ucnv_swapAliases(): unable to allocate memory for sorting tables (max length: %u)\n",
                                     count);
                    *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಹಿಂತಿರುಗಿ 0;
                }
                ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resort=(uint16_t *)(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು+count);
            }

            ಆದರೆ(ds->ಹೊರಹೋಗು_Charset==U_ASCII_FAMILY) {
                ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.stripForCompare=ucnv_io_stripASCIIForCompare;
            } ಇಲ್ಲದಿದ್ದರೆ /* U_EBCDIC_FAMILY */ {
                ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.stripForCompare=ucnv_io_stripEBCDICForCompare;
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
            p=ಒಳ_ಕೋಷ್ಟಕ+offsets[alias_List_ಸೂಚ್ಯಂಕ];
            q=ಹೊರಹೋಗು_ಕೋಷ್ಟಕ+offsets[alias_List_ಸೂಚ್ಯಂಕ];

            p2=ಒಳ_ಕೋಷ್ಟಕ+offsets[untagged_Conv_Array_ಸೂಚ್ಯಂಕ];
            q2=ಹೊರಹೋಗು_ಕೋಷ್ಟಕ+offsets[untagged_Conv_Array_ಸೂಚ್ಯಂಕ];

            ಗಾಗಿ(i=0; i<count; ++i) {
                ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು[i].str_ಸೂಚ್ಯಂಕ=ds->readUInt16(p[i]);
                ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು[i].ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ=(uint16_t)i;
            }

            uprv_ವಿಂಗಡಿಸು_Array(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು, (int32_t)count, ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ_ಅಡ್ಡಸಾಲು),
                           io_compare_ಅಡ್ಡಸಾಲುಗಳು, &ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ,
                           ಸುಳ್ಳು, p_Error_ಸಂಕೇತ);

            ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
                /* copy/swap/permutate items */
                ಆದರೆ(p!=q) {
                    ಗಾಗಿ(i=0; i<count; ++i) {
                        old_ಸೂಚ್ಯಂಕ=ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು[i].ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ;
                        ds->ಅದಲುಬದಲು_Array16(ds, p+old_ಸೂಚ್ಯಂಕ, 2, q+i, p_Error_ಸಂಕೇತ);
                        ds->ಅದಲುಬದಲು_Array16(ds, p2+old_ಸೂಚ್ಯಂಕ, 2, q2+i, p_Error_ಸಂಕೇತ);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /*
                     * If we swap in-place, then the permutation must use another
                     * temporary array (tempTable.resort)
                     * before the results are copied to the outBundle.
                     */
                    uint16_t *r=ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resort;

                    ಗಾಗಿ(i=0; i<count; ++i) {
                        old_ಸೂಚ್ಯಂಕ=ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು[i].ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ;
                        ds->ಅದಲುಬದಲು_Array16(ds, p+old_ಸೂಚ್ಯಂಕ, 2, r+i, p_Error_ಸಂಕೇತ);
                    }
                    uprv_memcpy(q, r, 2*(size_t)count);

                    ಗಾಗಿ(i=0; i<count; ++i) {
                        old_ಸೂಚ್ಯಂಕ=ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು[i].ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ;
                        ds->ಅದಲುಬದಲು_Array16(ds, p2+old_ಸೂಚ್ಯಂಕ, 2, r+i, p_Error_ಸಂಕೇತ);
                    }
                    uprv_memcpy(q2, r, 2*(size_t)count);
                }
            }

            ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು!=ಅಡ್ಡಸಾಲುಗಳು) {
                uprv_free(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು);
            }

            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                udata_printError(ds, "ucnv_swapAliases().uprv_sortArray(%u items) failed\n",
                                 count);
                ಹಿಂತಿರುಗಿ 0;
            }

            /* swap remaining 16-bit values */
            ds->ಅದಲುಬದಲು_Array16(ds,
                            ಒಳ_ಕೋಷ್ಟಕ+offsets[converter_List_ಸೂಚ್ಯಂಕ],
                            2*(int32_t)(offsets[alias_List_ಸೂಚ್ಯಂಕ]-offsets[converter_List_ಸೂಚ್ಯಂಕ]),
                            ಹೊರಹೋಗು_ಕೋಷ್ಟಕ+offsets[converter_List_ಸೂಚ್ಯಂಕ],
                            p_Error_ಸಂಕೇತ);
            ds->ಅದಲುಬದಲು_Array16(ds,
                            ಒಳ_ಕೋಷ್ಟಕ+offsets[tagged_Alias_Array_ಸೂಚ್ಯಂಕ],
                            2*(int32_t)(offsets[string_ಕೋಷ್ಟಕ_ಸೂಚ್ಯಂಕ]-offsets[tagged_Alias_Array_ಸೂಚ್ಯಂಕ]),
                            ಹೊರಹೋಗು_ಕೋಷ್ಟಕ+offsets[tagged_Alias_Array_ಸೂಚ್ಯಂಕ],
                            p_Error_ಸಂಕೇತ);
        }
    }

    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ+2*(int32_t)ತುದಿ_ಸ್ಥಾನಾಂತರ;
}

#ಆದರೆ_ಅಂತ್ಯ


/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

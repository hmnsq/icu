// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2001-2008, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  utrie2_impl.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2008sep26 (split off from utrie2.c)
*   created by: Markus W. Scherer
*
*   Definitions needed for both runtime and builder code for UTrie2,
*   used by utrie2.c and utrie2_builder.c.
*/

#यदि_अपरिभाषितम् __UTRIE2_IMPL_H__
#परिभाषय __UTRIE2_IMPL_H__

#यदि_परिभाषितम् UCPTRIE_DEBUG
#अन्तर्भाव "unicode/umutablecptrie.h"
#यदि_अन्तः
#अन्तर्भाव "utrie2.h"

/* Public UTrie2 API implementation ----------------------------------------- */

/*
 * These definitions are mostly needed by utrie2.cpp,
 * but also by utrie2_serialize() and utrie2_swap().
 */

// UTrie2 signature values, in platform endianness and opposite endianness.
// The UTrie2 signature ASCII byte values spell "Tri2".
#परिभाषय UTRIE2_SIG      0x54726932
#परिभाषय UTRIE2_OE_SIG   0x32697254

/**
 * Trie data structure in serialized form:
 *
 * UTrie2Header header;
 * uint16_t index[header.index2Length];
 * uint16_t data[header.shiftedDataLength<<2];  -- or uint32_t data[...]
 * @internal
 */
प्रकारनिर्देश संरचना UTrie2_शीर्षकम् {
    /** "Tri2" in big-endian US-ASCII (0x54726932) */
    uint32_t signature;

    /**
     * options bit field:
     * 15.. 4   reserved (0)
     *  3.. 0   UTrie2ValueBits valueBits
     */
    uint16_t options;

    /** UTRIE2_INDEX_1_OFFSET..UTRIE2_MAX_INDEX_LENGTH */
    uint16_t सूचकांक_दीर्घता;

    /** (UTRIE2_DATA_START_OFFSET..UTRIE2_MAX_DATA_LENGTH)>>UTRIE2_INDEX_SHIFT */
    uint16_t shifted_दत्त_दीर्घता;

    /** Null index and data blocks, not shifted. */
    uint16_t index2_Null_विस्थापनम्, दत्त_Null_विस्थापनम्;

    /**
     * First code point of the single-value range ending with U+10ffff,
     * rounded up and then shifted right by UTRIE2_SHIFT_1.
     */
    uint16_t shifted_उच्चः_Start;
} UTrie2_शीर्षकम्;

/**
 * Constants for use with UTrie2Header.options.
 * @internal
 */
गणना {
    /** Mask to get the UTrie2ValueBits valueBits from options. */
    UTRIE2_OPTIONS_मानम्_द्वयङ्काः_आवरणम्=0xf
};

/* Building a trie ---------------------------------------------------------- */

/*
 * These definitions are mostly needed by utrie2_builder.c, but also by
 * utrie2_get32() and utrie2_enum().
 */

गणना {
    /**
     * At build time, leave a gap in the index-2 table,
     * at least as long as the maximum lengths of the 2-byte UTF-8 index-2 table
     * and the supplementary index-1 table.
     * Round up to UTRIE2_INDEX_2_BLOCK_LENGTH for proper compacting.
     */
    UNEWTRIE2_सूचकांक_GAP_विस्थापनम्=UTRIE2_सूचकांक_2_BMP_दीर्घता,
    UNEWTRIE2_सूचकांक_GAP_दीर्घता=
        ((UTRIE2_UTF8_2_B_सूचकांक_2_दीर्घता+UTRIE2_MAX_सूचकांक_1_दीर्घता)+UTRIE2_सूचकांक_2_आवरणम्)&
        ~UTRIE2_सूचकांक_2_आवरणम्,

    /**
     * Maximum length of the build-time index-2 array.
     * Maximum number of Unicode code points (0x110000) shifted right by UTRIE2_SHIFT_2,
     * plus the part of the index-2 table for lead surrogate code points,
     * plus the build-time index gap,
     * plus the null index-2 block.
     */
    UNEWTRIE2_MAX_सूचकांक_2_दीर्घता=
        (0x110000>>UTRIE2_सरणम्_2)+
        UTRIE2_LSCP_सूचकांक_2_दीर्घता+
        UNEWTRIE2_सूचकांक_GAP_दीर्घता+
        UTRIE2_सूचकांक_2_खण्डम्_दीर्घता,

    UNEWTRIE2_सूचकांक_1_दीर्घता=0x110000>>UTRIE2_सरणम्_1
};

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points, plus the illegal-UTF-8 block and the null block,
 * plus values for the 0x400 surrogate code units.
 */
#परिभाषय UNEWTRIE2_MAX_दत्त_दीर्घता (0x110000+0x40+0x40+0x400)

/*
 * Build-time trie structure.
 *
 * Just using a boolean flag for "repeat use" could lead to data array overflow
 * because we would not be able to detect when a data block becomes unused.
 * It also leads to orphan data blocks that are kept through serialization.
 *
 * Need to use reference counting for data blocks,
 * and allocDataBlock() needs to look for a free block before increasing dataLength.
 *
 * This scheme seems like overkill for index-2 blocks since the whole index array is
 * preallocated anyway (unlike the growable data array).
 * Just allocating multiple index-2 blocks as needed.
 */
संरचना UNewTrie2 {
    int32_t index1[UNEWTRIE2_सूचकांक_1_दीर्घता];
    int32_t index2[UNEWTRIE2_MAX_सूचकांक_2_दीर्घता];
    uint32_t *दत्त;
#यदि_परिभाषितम् UCPTRIE_DEBUG
    UMutableCPTrie *t3;
#यदि_अन्तः

    uint32_t initial_मानम्, error_मानम्;
    int32_t index2_दीर्घता, दत्त_क्षमता, दत्त_दीर्घता;
    int32_t first_Free_खण्डम्;
    int32_t index2_Null_विस्थापनम्, दत्त_Null_विस्थापनम्;
    UChar32 उच्चः_Start;
    UBool isCompacted;

    /**
     * Multi-purpose per-data-block table.
     *
     * Before compacting:
     *
     * Per-data-block reference counters/free-block list.
     *  0: unused
     * >0: reference counter (number of index-2 entries pointing here)
     * <0: next free data block in free-block list
     *
     * While compacting:
     *
     * Map of adjusted indexes, used in compactData() and compactIndex2().
     * Maps from original indexes to new ones.
     */
    int32_t map[UNEWTRIE2_MAX_दत्त_दीर्घता>>UTRIE2_सरणम्_2];
};

#यदि_अन्तः

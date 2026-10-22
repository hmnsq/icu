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

#నిర్వచితం_కాకపోతే __UTRIE2_IMPL_H__
#నిర్వచించు __UTRIE2_IMPL_H__

#నిర్వచితమైతే UCPTRIE_DEBUG
#అన్తర్భావ "unicode/umutablecptrie.h"
#అయితే_ముగింపు
#అన్తర్భావ "utrie2.h"

/* Public UTrie2 API implementation ----------------------------------------- */

/*
 * These definitions are mostly needed by utrie2.cpp,
 * but also by utrie2_serialize() and utrie2_swap().
 */

// UTrie2 signature values, in platform endianness and opposite endianness.
// The UTrie2 signature ASCII byte values spell "Tri2".
#నిర్వచించు UTRIE2_SIG      0x54726932
#నిర్వచించు UTRIE2_OE_SIG   0x32697254

/**
 * Trie data structure in serialized form:
 *
 * UTrie2Header header;
 * uint16_t index[header.index2Length];
 * uint16_t data[header.shiftedDataLength<<2];  -- or uint32_t data[...]
 * @internal
 */
రకనిర్వచనం నిర్మాణం UTrie2_శీర్షిక {
    /** "Tri2" in big-endian US-ASCII (0x54726932) */
    uint32_t signature;

    /**
     * options bit field:
     * 15.. 4   reserved (0)
     *  3.. 0   UTrie2ValueBits valueBits
     */
    uint16_t options;

    /** UTRIE2_INDEX_1_OFFSET..UTRIE2_MAX_INDEX_LENGTH */
    uint16_t సూచీ_నిడివి;

    /** (UTRIE2_DATA_START_OFFSET..UTRIE2_MAX_DATA_LENGTH)>>UTRIE2_INDEX_SHIFT */
    uint16_t shifted_దత్తాంశం_నిడివి;

    /** Null index and data blocks, not shifted. */
    uint16_t index2_Null_స్థానభ్రంశం, దత్తాంశం_Null_స్థానభ్రంశం;

    /**
     * First code point of the single-value range ending with U+10ffff,
     * rounded up and then shifted right by UTRIE2_SHIFT_1.
     */
    uint16_t shifted_ఎగువ_ప్రారంభం;
} UTrie2_శీర్షిక;

/**
 * Constants for use with UTrie2Header.options.
 * @internal
 */
గణన {
    /** Mask to get the UTrie2ValueBits valueBits from options. */
    UTRIE2_OPTIONS_విలువ_బిట్లు_ముసుగు=0xf
};

/* Building a trie ---------------------------------------------------------- */

/*
 * These definitions are mostly needed by utrie2_builder.c, but also by
 * utrie2_get32() and utrie2_enum().
 */

గణన {
    /**
     * At build time, leave a gap in the index-2 table,
     * at least as long as the maximum lengths of the 2-byte UTF-8 index-2 table
     * and the supplementary index-1 table.
     * Round up to UTRIE2_INDEX_2_BLOCK_LENGTH for proper compacting.
     */
    UNEWTRIE2_సూచీ_GAP_స్థానభ్రంశం=UTRIE2_సూచీ_2_BMP_నిడివి,
    UNEWTRIE2_సూచీ_GAP_నిడివి=
        ((UTRIE2_UTF8_2_B_సూచీ_2_నిడివి+UTRIE2_MAX_సూచీ_1_నిడివి)+UTRIE2_సూచీ_2_ముసుగు)&
        ~UTRIE2_సూచీ_2_ముసుగు,

    /**
     * Maximum length of the build-time index-2 array.
     * Maximum number of Unicode code points (0x110000) shifted right by UTRIE2_SHIFT_2,
     * plus the part of the index-2 table for lead surrogate code points,
     * plus the build-time index gap,
     * plus the null index-2 block.
     */
    UNEWTRIE2_MAX_సూచీ_2_నిడివి=
        (0x110000>>UTRIE2_జరుపు_2)+
        UTRIE2_LSCP_సూచీ_2_నిడివి+
        UNEWTRIE2_సూచీ_GAP_నిడివి+
        UTRIE2_సూచీ_2_ఖండం_నిడివి,

    UNEWTRIE2_సూచీ_1_నిడివి=0x110000>>UTRIE2_జరుపు_1
};

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points, plus the illegal-UTF-8 block and the null block,
 * plus values for the 0x400 surrogate code units.
 */
#నిర్వచించు UNEWTRIE2_MAX_దత్తాంశం_నిడివి (0x110000+0x40+0x40+0x400)

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
నిర్మాణం UNewTrie2 {
    int32_t index1[UNEWTRIE2_సూచీ_1_నిడివి];
    int32_t index2[UNEWTRIE2_MAX_సూచీ_2_నిడివి];
    uint32_t *దత్తాంశం;
#నిర్వచితమైతే UCPTRIE_DEBUG
    UMutableCPTrie *t3;
#అయితే_ముగింపు

    uint32_t initial_విలువ, error_విలువ;
    int32_t index2_నిడివి, దత్తాంశం_సామర్థ్యం, దత్తాంశం_నిడివి;
    int32_t మొదటి_Free_ఖండం;
    int32_t index2_Null_స్థానభ్రంశం, దత్తాంశం_Null_స్థానభ్రంశం;
    UChar32 ఎగువ_ప్రారంభం;
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
    int32_t map[UNEWTRIE2_MAX_దత్తాంశం_నిడివి>>UTRIE2_జరుపు_2];
};

#అయితే_ముగింపు

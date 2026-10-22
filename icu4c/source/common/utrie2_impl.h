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

#جے_بنا_تعریف __UTRIE2_IMPL_H__
#تعریف_کرو __UTRIE2_IMPL_H__

#جے_تعریف_شدہ UCPTRIE_DEBUG
#ضمن "unicode/umutablecptrie.h"
#جے_انت
#ضمن "utrie2.h"

/* Public UTrie2 API implementation ----------------------------------------- */

/*
 * These definitions are mostly needed by utrie2.cpp,
 * but also by utrie2_serialize() and utrie2_swap().
 */

// UTrie2 signature values, in platform endianness and opposite endianness.
// The UTrie2 signature ASCII byte values spell "Tri2".
#تعریف_کرو UTRIE2_SIG      0x54726932
#تعریف_کرو UTRIE2_OE_SIG   0x32697254

/**
 * Trie data structure in serialized form:
 *
 * UTrie2Header header;
 * uint16_t index[header.index2Length];
 * uint16_t data[header.shiftedDataLength<<2];  -- or uint32_t data[...]
 * @internal
 */
تعريفنوع بنية UTrie2_سرخی {
    /** "Tri2" in big-endian US-ASCII (0x54726932) */
    uint32_t signature;

    /**
     * options bit field:
     * 15.. 4   reserved (0)
     *  3.. 0   UTrie2ValueBits valueBits
     */
    uint16_t options;

    /** UTRIE2_INDEX_1_OFFSET..UTRIE2_MAX_INDEX_LENGTH */
    uint16_t اشاریہ_لمائی;

    /** (UTRIE2_DATA_START_OFFSET..UTRIE2_MAX_DATA_LENGTH)>>UTRIE2_INDEX_SHIFT */
    uint16_t shifted_انکڑے_لمائی;

    /** Null index and data blocks, not shifted. */
    uint16_t index2_Null_وتھ, انکڑے_Null_وتھ;

    /**
     * First code point of the single-value range ending with U+10ffff,
     * rounded up and then shifted right by UTRIE2_SHIFT_1.
     */
    uint16_t shifted_بلند_شروع;
} UTrie2_سرخی;

/**
 * Constants for use with UTrie2Header.options.
 * @internal
 */
تعداد {
    /** Mask to get the UTrie2ValueBits valueBits from options. */
    UTRIE2_OPTIONS_قدر_بٹ_نقاب=0xf
};

/* Building a trie ---------------------------------------------------------- */

/*
 * These definitions are mostly needed by utrie2_builder.c, but also by
 * utrie2_get32() and utrie2_enum().
 */

تعداد {
    /**
     * At build time, leave a gap in the index-2 table,
     * at least as long as the maximum lengths of the 2-byte UTF-8 index-2 table
     * and the supplementary index-1 table.
     * Round up to UTRIE2_INDEX_2_BLOCK_LENGTH for proper compacting.
     */
    UNEWTRIE2_اشاریہ_GAP_وتھ=UTRIE2_اشاریہ_2_BMP_لمائی,
    UNEWTRIE2_اشاریہ_GAP_لمائی=
        ((UTRIE2_UTF8_2_B_اشاریہ_2_لمائی+UTRIE2_MAX_اشاریہ_1_لمائی)+UTRIE2_اشاریہ_2_نقاب)&
        ~UTRIE2_اشاریہ_2_نقاب,

    /**
     * Maximum length of the build-time index-2 array.
     * Maximum number of Unicode code points (0x110000) shifted right by UTRIE2_SHIFT_2,
     * plus the part of the index-2 table for lead surrogate code points,
     * plus the build-time index gap,
     * plus the null index-2 block.
     */
    UNEWTRIE2_MAX_اشاریہ_2_لمائی=
        (0x110000>>UTRIE2_کھسکاؤ_2)+
        UTRIE2_LSCP_اشاریہ_2_لمائی+
        UNEWTRIE2_اشاریہ_GAP_لمائی+
        UTRIE2_اشاریہ_2_بلاک_لمائی,

    UNEWTRIE2_اشاریہ_1_لمائی=0x110000>>UTRIE2_کھسکاؤ_1
};

/**
 * Maximum length of the build-time data array.
 * One entry per 0x110000 code points, plus the illegal-UTF-8 block and the null block,
 * plus values for the 0x400 surrogate code units.
 */
#تعریف_کرو UNEWTRIE2_MAX_انکڑے_لمائی (0x110000+0x40+0x40+0x400)

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
بنية UNewTrie2 {
    int32_t index1[UNEWTRIE2_اشاریہ_1_لمائی];
    int32_t index2[UNEWTRIE2_MAX_اشاریہ_2_لمائی];
    uint32_t *انکڑے;
#جے_تعریف_شدہ UCPTRIE_DEBUG
    UMutableCPTrie *t3;
#جے_انت

    uint32_t initial_قدر, error_قدر;
    int32_t index2_لمائی, انکڑے_سمرتھا, انکڑے_لمائی;
    int32_t پہلا_Free_بلاک;
    int32_t index2_Null_وتھ, انکڑے_Null_وتھ;
    UChar32 بلند_شروع;
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
    int32_t map[UNEWTRIE2_MAX_انکڑے_لمائی>>UTRIE2_کھسکاؤ_2];
};

#جے_انت

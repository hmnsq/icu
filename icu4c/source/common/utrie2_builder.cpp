// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2001-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  utrie2_builder.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2008sep26 (split off from utrie2.c)
*   created by: Markus W. Scherer
*
*   This is a common implementation of a Unicode trie.
*   It is a kind of compressed, serializable table of 16- or 32-bit values associated with
*   Unicode code points (0..0x10ffff).
*   This is the second common version of a Unicode trie (hence the name UTrie2).
*   See utrie2.h for a comparison.
*
*   This file contains only the builder code.
*   See utrie2.c for the runtime and enumeration code.
*/
// #define UTRIE2_DEBUG
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
#   include <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ
// #define UCPTRIE_DEBUG

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/umutablecptrie.h"
#ಅನ್ತರ್ಭಾವ "ucptrie_impl.h"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "utrie2.h"
#ಅನ್ತರ್ಭಾವ "utrie2_impl.h"

#ಅನ್ತರ್ಭಾವ "utrie.h"  // for utrie2_fromUTrie()

/* Implementation notes ----------------------------------------------------- */

/*
 * The UTRIE2_SHIFT_1, UTRIE2_SHIFT_2, UTRIE2_INDEX_SHIFT and other values
 * have been chosen to minimize trie sizes overall.
 * Most of the code is flexible enough to work with a range of values,
 * within certain limits.
 *
 * Exception: Support for separate values for lead surrogate code _units_
 * vs. code _points_ was added after the constants were fixed,
 * and has not been tested nor particularly designed for different constant values.
 * (Especially the utrie2_enum() code that jumps to the special LSCP index-2
 * part and back.)
 *
 * Requires UTRIE2_SHIFT_2<=6. Otherwise 0xc0 which is the top of the ASCII-linear data
 * including the bad-UTF-8-data block is not a multiple of UTRIE2_DATA_BLOCK_LENGTH
 * and map[block>>UTRIE2_SHIFT_2] (used in reference counting and compaction
 * remapping) stops working.
 *
 * Requires UTRIE2_SHIFT_1>=10 because utrie2_enumForLeadSurrogate()
 * assumes that a single index-2 block is used for 0x400 code points
 * corresponding to one lead surrogate.
 *
 * Requires UTRIE2_SHIFT_1<=16. Otherwise one single index-2 block contains
 * more than one Unicode plane, and the split of the index-2 table into a BMP
 * part and a supplementary part, with a gap in between, would not work.
 *
 * Requires UTRIE2_INDEX_SHIFT>=1 not because of the code but because
 * there is data with more than 64k distinct values,
 * for example for Unihan collation with a separate collation weight per
 * Han character.
 */

/* Building a trie ----------------------------------------------------------*/

ಎಣಿಕೆ {
    /** The null index-2 block, following the gap in the index-2 table. */
    UNEWTRIE2_ಸೂಚ್ಯಂಕ_2_NULL_ಸ್ಥಾನಾಂತರ=UNEWTRIE2_ಸೂಚ್ಯಂಕ_GAP_ಸ್ಥಾನಾಂತರ+UNEWTRIE2_ಸೂಚ್ಯಂಕ_GAP_ಉದ್ದಳತೆ,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_ಸೂಚ್ಯಂಕ_2_ಆರಂಭ_ಸ್ಥಾನಾಂತರ=UNEWTRIE2_ಸೂಚ್ಯಂಕ_2_NULL_ಸ್ಥಾನಾಂತರ+UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_ದತ್ತಾಂಶ_NULL_ಸ್ಥಾನಾಂತರ=UTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ,

    /** The start of allocated data blocks. */
    UNEWTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ=UNEWTRIE2_ದತ್ತಾಂಶ_NULL_ಸ್ಥಾನಾಂತರ+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_ದತ್ತಾಂಶ_0800_ಸ್ಥಾನಾಂತರ=UNEWTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+0x780
};

/* Start with allocation of 16k data entries. */
#ವ್ಯಾಖ್ಯಾನಿಸು UNEWTRIE2_INITIAL_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ ((int32_t)1<<14)

/* Grow about 8x each time. */
#ವ್ಯಾಖ್ಯಾನಿಸು UNEWTRIE2_MEDIUM_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ ((int32_t)1<<17)

ಸ್ಥಿರ int32_t
alloc_Index2_ಖಂಡ(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_ಮೌಲ್ಯ, uint32_t error_ಮೌಲ್ಯ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *ದತ್ತಾಂಶ;
    int32_t i, j;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    trie=(UTrie2 *)uprv_malloc(ಗಾತ್ರ(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(ಗಾತ್ರ(UNewTrie2));
    ದತ್ತಾಂಶ=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4);
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ || newTrie==ಶೂನ್ಯಸೂಚಕ || ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(ದತ್ತಾಂಶ);
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    uprv_memset(trie, 0, ಗಾತ್ರ(UTrie2));
    trie->initial_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
    trie->error_ಮೌಲ್ಯ=error_ಮೌಲ್ಯ;
    trie->ಮೇಲಿನ_ಆರಂಭ=0x110000;
    trie->newTrie=newTrie;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    trie->name="open";
#ಆದರೆ_ಅಂತ್ಯ

    newTrie->ದತ್ತಾಂಶ=ದತ್ತಾಂಶ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_ಮೌಲ್ಯ, error_ಮೌಲ್ಯ, p_Error_ಸಂಕೇತ);
#ಆದರೆ_ಅಂತ್ಯ
    newTrie->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ=UNEWTRIE2_INITIAL_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    newTrie->initial_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
    newTrie->error_ಮೌಲ್ಯ=error_ಮೌಲ್ಯ;
    newTrie->ಮೇಲಿನ_ಆರಂಭ=0x110000;
    newTrie->ಮೊದಲ_Free_ಖಂಡ=0;  /* no free block in the list */
    newTrie->isCompacted=ಸುಳ್ಳು;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    ಗಾಗಿ(i=0; i<0x80; ++i) {
        newTrie->ದತ್ತಾಂಶ[i]=initial_ಮೌಲ್ಯ;
    }
    ಗಾಗಿ(; i<0xc0; ++i) {
        newTrie->ದತ್ತಾಂಶ[i]=error_ಮೌಲ್ಯ;
    }
    ಗಾಗಿ(i=UNEWTRIE2_ದತ್ತಾಂಶ_NULL_ಸ್ಥಾನಾಂತರ; i<UNEWTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ; ++i) {
        newTrie->ದತ್ತಾಂಶ[i]=initial_ಮೌಲ್ಯ;
    }
    newTrie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ=UNEWTRIE2_ದತ್ತಾಂಶ_NULL_ಸ್ಥಾನಾಂತರ;
    newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=UNEWTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    ಗಾಗಿ(i=0, j=0; j<0x80; ++i, j+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    ಗಾಗಿ(; j<0xc0; ++i, j+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_ಸರಿಸು_2)-
        (0x80>>UTRIE2_ಸರಿಸು_2)+
        1+
        UTRIE2_LSCP_ಸೂಚ್ಯಂಕ_2_ಉದ್ದಳತೆ;
    j+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    ಗಾಗಿ(; j<UNEWTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ; ++i, j+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    ಗಾಗಿ(i=0x80>>UTRIE2_ಸರಿಸು_2; i<UTRIE2_ಸೂಚ್ಯಂಕ_2_BMP_ಉದ್ದಳತೆ; ++i) {
        newTrie->index2[i]=UNEWTRIE2_ದತ್ತಾಂಶ_NULL_ಸ್ಥಾನಾಂತರ;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    ಗಾಗಿ(i=0; i<UNEWTRIE2_ಸೂಚ್ಯಂಕ_GAP_ಉದ್ದಳತೆ; ++i) {
        newTrie->index2[UNEWTRIE2_ಸೂಚ್ಯಂಕ_GAP_ಸ್ಥಾನಾಂತರ+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    ಗಾಗಿ(i=0; i<UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ; ++i) {
        newTrie->index2[UNEWTRIE2_ಸೂಚ್ಯಂಕ_2_NULL_ಸ್ಥಾನಾಂತರ+i]=UNEWTRIE2_ದತ್ತಾಂಶ_NULL_ಸ್ಥಾನಾಂತರ;
    }
    newTrie->index2_Null_ಸ್ಥಾನಾಂತರ=UNEWTRIE2_ಸೂಚ್ಯಂಕ_2_NULL_ಸ್ಥಾನಾಂತರ;
    newTrie->index2_ಉದ್ದಳತೆ=UNEWTRIE2_ಸೂಚ್ಯಂಕ_2_ಆರಂಭ_ಸ್ಥಾನಾಂತರ;

    /* set the index-1 indexes for the linear index-2 block */
    ಗಾಗಿ(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_ಸೂಚ್ಯಂಕ_1_ಉದ್ದಳತೆ;
        ++i, j+=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    ಗಾಗಿ(; i<UNEWTRIE2_ಸೂಚ್ಯಂಕ_1_ಉದ್ದಳತೆ; ++i) {
        newTrie->index1[i]=UNEWTRIE2_ಸೂಚ್ಯಂಕ_2_NULL_ಸ್ಥಾನಾಂತರ;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    ಗಾಗಿ(i=0x80; i<0x800; i+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ) {
        utrie2_set32(trie, i, initial_ಮೌಲ್ಯ, p_Error_ಸಂಕೇತ);
    }

    ಹಿಂತಿರುಗಿ trie;
}

ಸ್ಥಿರ UNewTrie2 *
cloneBuilder(ಸ್ಥಿರಾಂಕ UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(ಗಾತ್ರ(UNewTrie2)));
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    trie->ದತ್ತಾಂಶ = static_cast<uint32_t*>(uprv_malloc(other->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ * 4));
    ಆದರೆ(trie->ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(trie);
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    ಆದರೆ(other->t3==ಶೂನ್ಯಸೂಚಕ) {
        trie->t3=ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &error_ಸಂಕೇತ);
    }
#ಆದರೆ_ಅಂತ್ಯ
    trie->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ=other->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, ಗಾತ್ರ(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_ಉದ್ದಳತೆ*4);
    trie->index2_Null_ಸ್ಥಾನಾಂತರ=other->index2_Null_ಸ್ಥಾನಾಂತರ;
    trie->index2_ಉದ್ದಳತೆ=other->index2_ಉದ್ದಳತೆ;

    uprv_memcpy(trie->ದತ್ತಾಂಶ, other->ದತ್ತಾಂಶ, (size_t)other->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4);
    trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ=other->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=other->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;

    /* reference counters */
    ಆದರೆ(other->isCompacted) {
        trie->ಮೊದಲ_Free_ಖಂಡ=0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        uprv_memcpy(trie->map, other->map, ((size_t)other->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ>>UTRIE2_ಸರಿಸು_2)*4);
        trie->ಮೊದಲ_Free_ಖಂಡ=other->ಮೊದಲ_Free_ಖಂಡ;
    }

    trie->initial_ಮೌಲ್ಯ=other->initial_ಮೌಲ್ಯ;
    trie->error_ಮೌಲ್ಯ=other->error_ಮೌಲ್ಯ;
    trie->ಮೇಲಿನ_ಆರಂಭ=other->ಮೇಲಿನ_ಆರಂಭ;
    trie->isCompacted=other->isCompacted;

    ಹಿಂತಿರುಗಿ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ಸ್ಥಿರಾಂಕ UTrie2 *other, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UTrie2 *trie;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(other==ಶೂನ್ಯಸೂಚಕ || (other->ಸ್ಮರಣೆ==ಶೂನ್ಯಸೂಚಕ && other->newTrie==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    trie=(UTrie2 *)uprv_malloc(ಗಾತ್ರ(UTrie2));
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memcpy(trie, other, ಗಾತ್ರ(UTrie2));

    ಆದರೆ(other->ಸ್ಮರಣೆ!=ಶೂನ್ಯಸೂಚಕ) {
        trie->ಸ್ಮರಣೆ=uprv_malloc(other->length);
        ಆದರೆ(trie->ಸ್ಮರಣೆ!=ಶೂನ್ಯಸೂಚಕ) {
            trie->is_ಸ್ಮರಣೆ_Owned=ನಿಜ;
            uprv_memcpy(trie->ಸ್ಮರಣೆ, other->ಸ್ಮರಣೆ, other->length);

            /* make the clone's pointers point to its own memory */
            trie->ಸೂಚ್ಯಂಕ=(uint16_t *)trie->ಸ್ಮರಣೆ+(other->ಸೂಚ್ಯಂಕ-(uint16_t *)other->ಸ್ಮರಣೆ);
            ಆದರೆ(other->data16!=ಶೂನ್ಯಸೂಚಕ) {
                trie->data16=(uint16_t *)trie->ಸ್ಮರಣೆ+(other->data16-(uint16_t *)other->ಸ್ಮರಣೆ);
            }
            ಆದರೆ(other->data32!=ಶೂನ್ಯಸೂಚಕ) {
                trie->data32=(uint32_t *)trie->ಸ್ಮರಣೆ+(other->data32-(uint32_t *)other->ಸ್ಮರಣೆ);
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    ಆದರೆ(trie->ಸ್ಮರಣೆ==ಶೂನ್ಯಸೂಚಕ && trie->newTrie==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        uprv_free(trie);
        trie=ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ trie;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ New_Trie_And_ಸ್ಥಿತಿ {
    UTrie2 *trie;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ;
    UBool exclusive_ಎಲ್ಲೆ;  /* rather than inclusive range end */
} New_Trie_And_ಸ್ಥಿತಿ;

ಸ್ಥಿರ UBool U_CALLCONV
copyEnumRange(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ, uint32_t value) {
    New_Trie_And_ಸ್ಥಿತಿ *nt=(New_Trie_And_ಸ್ಥಿತಿ *)context;
    ಆದರೆ(value!=nt->trie->initial_ಮೌಲ್ಯ) {
        ಆದರೆ(nt->exclusive_ಎಲ್ಲೆ) {
            --ಕೊನೆ;
        }
        ಆದರೆ(ಆರಂಭ==ಕೊನೆ) {
            utrie2_set32(nt->trie, ಆರಂಭ, value, &nt->error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            utrie2_ಗಣ_Range32(nt->trie, ಆರಂಭ, ಕೊನೆ, value, ನಿಜ, &nt->error_ಸಂಕೇತ);
        }
        ಹಿಂತಿರುಗಿ U_SUCCESS(nt->error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
ಸ್ಥಿರ ದೀರ್ಘ ಎಣಿಕೆ_Initial(ಸ್ಥಿರಾಂಕ UTrie2 *trie) {
    uint32_t initial_ಮೌಲ್ಯ=trie->initial_ಮೌಲ್ಯ;
    int32_t length=trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ದೀರ್ಘ count=0;
    ಆದರೆ(trie->data16!=ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ(int32_t i=0; i<length; ++i) {
            ಆದರೆ(trie->data16[i]==initial_ಮೌಲ್ಯ) { ++count; }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಾಗಿ(int32_t i=0; i<length; ++i) {
            ಆದರೆ(trie->data32[i]==initial_ಮೌಲ್ಯ) { ++count; }
        }
    }
    ಹಿಂತಿರುಗಿ count;
}

ಸ್ಥಿರ ಶೂನ್ಯ
utrie_printLengths(ಸ್ಥಿರಾಂಕ UTrie *trie) {
    ದೀರ್ಘ ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ದೀರ್ಘ ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=(ದೀರ್ಘ)trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ದೀರ್ಘ ಒಟ್ಟು_ಉದ್ದಳತೆ=(ದೀರ್ಘ)ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ)+ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2+ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*(trie->data32!=ಶೂನ್ಯಸೂಚಕ ? 4 : 2);
    ಬರೆ("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, ಒಟ್ಟು_ಉದ್ದಳತೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ
utrie2_printLengths(ಸ್ಥಿರಾಂಕ UTrie2 *trie, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *which) {
    ದೀರ್ಘ ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ದೀರ್ಘ ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=(ದೀರ್ಘ)trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ದೀರ್ಘ ಒಟ್ಟು_ಉದ್ದಳತೆ=(ದೀರ್ಘ)ಗಾತ್ರ(UTrie2_ಶೀರ್ಷಿಕೆ)+ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2+ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*(trie->data32!=ಶೂನ್ಯಸೂಚಕ ? 4 : 2);
    ಬರೆ("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, ಎಣಿಕೆ_Initial(trie), ಒಟ್ಟು_ಉದ್ದಳತೆ);
}
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ಸ್ಥಿರಾಂಕ UTrie2 *other, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    New_Trie_And_ಸ್ಥಿತಿ context;
    char16_t lead;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(other==ಶೂನ್ಯಸೂಚಕ || (other->ಸ್ಮರಣೆ==ಶೂನ್ಯಸೂಚಕ && other->newTrie==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(other->newTrie!=ಶೂನ್ಯಸೂಚಕ && !other->newTrie->isCompacted) {
        ಹಿಂತಿರುಗಿ utrie2_clone(other, p_Error_ಸಂಕೇತ);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_ಮೌಲ್ಯ, other->error_ಮೌಲ್ಯ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    context.exclusive_ಎಲ್ಲೆ=ಸುಳ್ಳು;
    context.error_ಸಂಕೇತ=*p_Error_ಸಂಕೇತ;
    utrie2_enum(other, ಶೂನ್ಯಸೂಚಕ, copyEnumRange, &context);
    *p_Error_ಸಂಕೇತ=context.error_ಸಂಕೇತ;
    ಗಾಗಿ(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        ಆದರೆ(other->data32==ಶೂನ್ಯಸೂಚಕ) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } ಇಲ್ಲದಿದ್ದರೆ {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        ಆದರೆ(value!=other->initial_ಮೌಲ್ಯ) {
            utrie2_set32_For_Lead_Surrogate_ಸಂಕೇತ_Unit(context.trie, lead, value, p_Error_ಸಂಕೇತ);
        }
    }
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        utrie2_close(context.trie);
        context.trie=ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ಸ್ಥಿರಾಂಕ UTrie *trie1, uint32_t error_ಮೌಲ್ಯ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    New_Trie_And_ಸ್ಥಿತಿ context;
    char16_t lead;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(trie1==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    context.trie=utrie2_open(trie1->initial_ಮೌಲ್ಯ, error_ಮೌಲ್ಯ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    context.exclusive_ಎಲ್ಲೆ=ನಿಜ;
    context.error_ಸಂಕೇತ=*p_Error_ಸಂಕೇತ;
    utrie_enum(trie1, ಶೂನ್ಯಸೂಚಕ, copyEnumRange, &context);
    *p_Error_ಸಂಕೇತ=context.error_ಸಂಕೇತ;
    ಗಾಗಿ(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        ಆದರೆ(trie1->data32==ಶೂನ್ಯಸೂಚಕ) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } ಇಲ್ಲದಿದ್ದರೆ {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        ಆದರೆ(value!=trie1->initial_ಮೌಲ್ಯ) {
            utrie2_set32_For_Lead_Surrogate_ಸಂಕೇತ_Unit(context.trie, lead, value, p_Error_ಸಂಕೇತ);
        }
    }
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=ಶೂನ್ಯಸೂಚಕ ? UTRIE2_32_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು : UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು,
                      p_Error_ಸಂಕೇತ);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        utrie2_close(context.trie);
        context.trie=ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ context.trie;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
is_ಒಳ_Null_ಖಂಡ(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, ಖಂಡ;

    ಆದರೆ(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_ಸೂಚ್ಯಂಕ_2_ಸ್ಥಾನಾಂತರ-(0xd800>>UTRIE2_ಸರಿಸು_2))+
            (c>>UTRIE2_ಸರಿಸು_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        i2=trie->index1[c>>UTRIE2_ಸರಿಸು_1]+
            ((c>>UTRIE2_ಸರಿಸು_2)&UTRIE2_ಸೂಚ್ಯಂಕ_2_ಮುಖವಾಡ);
    }
    ಖಂಡ=trie->index2[i2];
    ಹಿಂತಿರುಗಿ ಖಂಡ == trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
}

ಸ್ಥಿರ int32_t
alloc_Index2_ಖಂಡ(UNewTrie2 *trie) {
    int32_t new_ಖಂಡ, new_ತುದಿ;

    new_ಖಂಡ=trie->index2_ಉದ್ದಳತೆ;
    new_ತುದಿ=new_ಖಂಡ+UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ;
    ಆದರೆ(new_ತುದಿ>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        ಹಿಂತಿರುಗಿ -1;
    }
    trie->index2_ಉದ್ದಳತೆ=new_ತುದಿ;
    uprv_memcpy(trie->index2+new_ಖಂಡ, trie->index2+trie->index2_Null_ಸ್ಥಾನಾಂತರ, UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ*4);
    ಹಿಂತಿರುಗಿ new_ಖಂಡ;
}

ಸ್ಥಿರ int32_t
get_Index2_ಖಂಡ(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    ಆದರೆ(U_IS_LEAD(c) && forLSCP) {
        ಹಿಂತಿರುಗಿ UTRIE2_LSCP_ಸೂಚ್ಯಂಕ_2_ಸ್ಥಾನಾಂತರ;
    }

    i1=c>>UTRIE2_ಸರಿಸು_1;
    i2=trie->index1[i1];
    ಆದರೆ(i2==trie->index2_Null_ಸ್ಥಾನಾಂತರ) {
        i2=alloc_Index2_ಖಂಡ(trie);
        ಆದರೆ(i2<0) {
            ಹಿಂತಿರುಗಿ -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    ಹಿಂತಿರುಗಿ i2;
}

ಸ್ಥಿರ int32_t
alloc_ದತ್ತಾಂಶ_ಖಂಡ(UNewTrie2 *trie, int32_t copy_ಖಂಡ) {
    int32_t new_ಖಂಡ, new_ತುದಿ;

    ಆದರೆ(trie->ಮೊದಲ_Free_ಖಂಡ!=0) {
        /* get the first free block */
        new_ಖಂಡ=trie->ಮೊದಲ_Free_ಖಂಡ;
        trie->ಮೊದಲ_Free_ಖಂಡ=-trie->map[new_ಖಂಡ>>UTRIE2_ಸರಿಸು_2];
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* get a new block from the high end */
        new_ಖಂಡ=trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
        new_ತುದಿ=new_ಖಂಡ+UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        ಆದರೆ(new_ತುದಿ>trie->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *ದತ್ತಾಂಶ;

            ಆದರೆ(trie->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ<UNEWTRIE2_MEDIUM_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
                capacity=UNEWTRIE2_MEDIUM_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(trie->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ<UNEWTRIE2_MAX_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
                capacity=UNEWTRIE2_MAX_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                ಹಿಂತಿರುಗಿ -1;
            }
            ದತ್ತಾಂಶ = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            ಆದರೆ(ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ -1;
            }
            uprv_memcpy(ದತ್ತಾಂಶ, trie->ದತ್ತಾಂಶ, (size_t)trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4);
            uprv_free(trie->ದತ್ತಾಂಶ);
            trie->ದತ್ತಾಂಶ=ದತ್ತಾಂಶ;
            trie->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ=capacity;
        }
        trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=new_ತುದಿ;
    }
    uprv_memcpy(trie->ದತ್ತಾಂಶ+new_ಖಂಡ, trie->ದತ್ತಾಂಶ+copy_ಖಂಡ, UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ*4);
    trie->map[new_ಖಂಡ>>UTRIE2_ಸರಿಸು_2]=0;
    ಹಿಂತಿರುಗಿ new_ಖಂಡ;
}

/* call when the block's reference counter reaches 0 */
ಸ್ಥಿರ ಶೂನ್ಯ
release_ದತ್ತಾಂಶ_ಖಂಡ(UNewTrie2 *trie, int32_t ಖಂಡ) {
    /* put this block at the front of the free-block chain */
    trie->map[ಖಂಡ>>UTRIE2_ಸರಿಸು_2]=-trie->ಮೊದಲ_Free_ಖಂಡ;
    trie->ಮೊದಲ_Free_ಖಂಡ=ಖಂಡ;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
is_Writable_ಖಂಡ(UNewTrie2 *trie, int32_t ಖಂಡ) {
    ಹಿಂತಿರುಗಿ ಖಂಡ != trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ && 1 == trie->map[ಖಂಡ >> UTRIE2_ಸರಿಸು_2];
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
ಗಣ_Index2_ನಮೂದು(UNewTrie2 *trie, int32_t i2, int32_t ಖಂಡ) {
    int32_t old_ಖಂಡ;
    ++trie->map[ಖಂಡ>>UTRIE2_ಸರಿಸು_2];  /* increment first, in case block==oldBlock! */
    old_ಖಂಡ=trie->index2[i2];
    ಆದರೆ(0 == --trie->map[old_ಖಂಡ>>UTRIE2_ಸರಿಸು_2]) {
        release_ದತ್ತಾಂಶ_ಖಂಡ(trie, old_ಖಂಡ);
    }
    trie->index2[i2]=ಖಂಡ;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ಸ್ಥಿರ int32_t
get_ದತ್ತಾಂಶ_ಖಂಡ(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, old_ಖಂಡ, new_ಖಂಡ;

    i2=get_Index2_ಖಂಡ(trie, c, forLSCP);
    ಆದರೆ(i2<0) {
        ಹಿಂತಿರುಗಿ -1;  /* program error */
    }

    i2+=(c>>UTRIE2_ಸರಿಸು_2)&UTRIE2_ಸೂಚ್ಯಂಕ_2_ಮುಖವಾಡ;
    old_ಖಂಡ=trie->index2[i2];
    ಆದರೆ(is_Writable_ಖಂಡ(trie, old_ಖಂಡ)) {
        ಹಿಂತಿರುಗಿ old_ಖಂಡ;
    }

    /* allocate a new data block */
    new_ಖಂಡ=alloc_ದತ್ತಾಂಶ_ಖಂಡ(trie, old_ಖಂಡ);
    ಆದರೆ(new_ಖಂಡ<0) {
        /* out of memory in the data array */
        ಹಿಂತಿರುಗಿ -1;
    }
    ಗಣ_Index2_ನಮೂದು(trie, i2, new_ಖಂಡ);
    ಹಿಂತಿರುಗಿ new_ಖಂಡ;
}

/**
 * @return true if the value was successfully set
 */
ಸ್ಥಿರ ಶೂನ್ಯ
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t ಖಂಡ;

    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ || trie->isCompacted) {
        *p_Error_ಸಂಕೇತ=U_NO_WRITE_PERMISSION;
        ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    umutablecptrie_ಗಣ(trie->t3, c, value, p_Error_ಸಂಕೇತ);
#ಆದರೆ_ಅಂತ್ಯ

    ಖಂಡ=get_ದತ್ತಾಂಶ_ಖಂಡ(trie, c, forLSCP);
    ಆದರೆ(ಖಂಡ<0) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    trie->ದತ್ತಾಂಶ[ಖಂಡ+(c&UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ)]=value;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ((uint32_t)c>0x10ffff) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    set32(trie->newTrie, c, ನಿಜ, value, p_Error_ಸಂಕೇತ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_set32_For_Lead_Surrogate_ಸಂಕೇತ_Unit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(!U_IS_LEAD(c)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    set32(trie->newTrie, c, ಸುಳ್ಳು, value, p_Error_ಸಂಕೇತ);
}

ಸ್ಥಿರ ಶೂನ್ಯ
write_ಖಂಡ(uint32_t *ಖಂಡ, uint32_t value) {
    uint32_t *ಎಲ್ಲೆ=ಖಂಡ+UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    ಆಗ(ಖಂಡ<ಎಲ್ಲೆ) {
        *ಖಂಡ++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
ಸ್ಥಿರ ಶೂನ್ಯ
fill_ಖಂಡ(uint32_t *ಖಂಡ, UChar32 ಆರಂಭ, UChar32 ಎಲ್ಲೆ,
          uint32_t value, uint32_t initial_ಮೌಲ್ಯ, UBool overwrite) {
    uint32_t *p_ಎಲ್ಲೆ;

    p_ಎಲ್ಲೆ=ಖಂಡ+ಎಲ್ಲೆ;
    ಖಂಡ+=ಆರಂಭ;
    ಆದರೆ(overwrite) {
        ಆಗ(ಖಂಡ<p_ಎಲ್ಲೆ) {
            *ಖಂಡ++=value;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಗ(ಖಂಡ<p_ಎಲ್ಲೆ) {
            ಆದರೆ(*ಖಂಡ==initial_ಮೌಲ್ಯ) {
                *ಖಂಡ=value;
            }
            ++ಖಂಡ;
        }
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_ಗಣ_Range32(UTrie2 *trie,
                  UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
                  uint32_t value, UBool overwrite,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t ಖಂಡ, rest, repeat_ಖಂಡ;
    UChar32 ಎಲ್ಲೆ;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ((uint32_t)ಆರಂಭ>0x10ffff || (uint32_t)ಕೊನೆ>0x10ffff || ಆರಂಭ>ಕೊನೆ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    newTrie=trie->newTrie;
    ಆದರೆ(newTrie==ಶೂನ್ಯಸೂಚಕ || newTrie->isCompacted) {
        *p_Error_ಸಂಕೇತ=U_NO_WRITE_PERMISSION;
        ಹಿಂತಿರುಗಿ;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    umutablecptrie_ಗಣ_Range(newTrie->t3, ಆರಂಭ, ಕೊನೆ, value, p_Error_ಸಂಕೇತ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(!overwrite && value==newTrie->initial_ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ; /* nothing to do */
    }

    ಎಲ್ಲೆ=ಕೊನೆ+1;
    ಆದರೆ(ಆರಂಭ&UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ) {
        UChar32 ಮುಂದಿನ_ಆರಂಭ;

        /* set partial block at [start..following block boundary[ */
        ಖಂಡ=get_ದತ್ತಾಂಶ_ಖಂಡ(newTrie, ಆರಂಭ, ನಿಜ);
        ಆದರೆ(ಖಂಡ<0) {
            *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }

        ಮುಂದಿನ_ಆರಂಭ=(ಆರಂಭ+UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ)&~UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ;
        ಆದರೆ(ಮುಂದಿನ_ಆರಂಭ<=ಎಲ್ಲೆ) {
            fill_ಖಂಡ(newTrie->ದತ್ತಾಂಶ+ಖಂಡ, ಆರಂಭ&UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ, UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ,
                      value, newTrie->initial_ಮೌಲ್ಯ, overwrite);
            ಆರಂಭ=ಮುಂದಿನ_ಆರಂಭ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            fill_ಖಂಡ(newTrie->ದತ್ತಾಂಶ+ಖಂಡ, ಆರಂಭ&UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ, ಎಲ್ಲೆ&UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ,
                      value, newTrie->initial_ಮೌಲ್ಯ, overwrite);
            ಹಿಂತಿರುಗಿ;
        }
    }

    /* number of positions in the last, partial block */
    rest=ಎಲ್ಲೆ&UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ;

    /* round down limit to a block boundary */
    ಎಲ್ಲೆ&=~UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ;

    /* iterate over all-value blocks */
    ಆದರೆ(value==newTrie->initial_ಮೌಲ್ಯ) {
        repeat_ಖಂಡ=newTrie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        repeat_ಖಂಡ=-1;
    }

    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ) {
        int32_t i2;
        UBool ಗಣ_Repeat_ಖಂಡ=ಸುಳ್ಳು;

        ಆದರೆ(value==newTrie->initial_ಮೌಲ್ಯ && is_ಒಳ_Null_ಖಂಡ(newTrie, ಆರಂಭ, ನಿಜ)) {
            ಆರಂಭ+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ; /* nothing to do */
            ಮುಂದುವರಿ;
        }

        /* get index value */
        i2=get_Index2_ಖಂಡ(newTrie, ಆರಂಭ, ನಿಜ);
        ಆದರೆ(i2<0) {
            *p_Error_ಸಂಕೇತ=U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        i2+=(ಆರಂಭ>>UTRIE2_ಸರಿಸು_2)&UTRIE2_ಸೂಚ್ಯಂಕ_2_ಮುಖವಾಡ;
        ಖಂಡ=newTrie->index2[i2];
        ಆದರೆ(is_Writable_ಖಂಡ(newTrie, ಖಂಡ)) {
            /* already allocated */
            ಆದರೆ(overwrite && ಖಂಡ>=UNEWTRIE2_ದತ್ತಾಂಶ_0800_ಸ್ಥಾನಾಂತರ) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                ಗಣ_Repeat_ಖಂಡ=ನಿಜ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* !overwrite, or protected block: just write the values into this block */
                fill_ಖಂಡ(newTrie->ದತ್ತಾಂಶ+ಖಂಡ,
                          0, UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ,
                          value, newTrie->initial_ಮೌಲ್ಯ, overwrite);
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(newTrie->ದತ್ತಾಂಶ[ಖಂಡ]!=value && (overwrite || ಖಂಡ==newTrie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ)) {
            /*
             * Set the repeatBlock instead of the null block or previous repeat block:
             *
             * If !isWritableBlock() then all entries in the block have the same value
             * because it's the null block or a range block (the repeatBlock from a previous
             * call to utrie2_setRange32()).
             * No other blocks are used multiple times before compacting.
             *
             * The null block is the only non-writable block with the initialValue because
             * of the repeatBlock initialization above. (If value==initialValue, then
             * the repeatBlock will be the null data block.)
             *
             * We set our repeatBlock if the desired value differs from the block's value,
             * and if we overwrite any data or if the data is all initial values
             * (which is the same as the block being the null block, see above).
             */
            ಗಣ_Repeat_ಖಂಡ=ನಿಜ;
        }
        ಆದರೆ(ಗಣ_Repeat_ಖಂಡ) {
            ಆದರೆ(repeat_ಖಂಡ>=0) {
                ಗಣ_Index2_ನಮೂದು(newTrie, i2, repeat_ಖಂಡ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* create and set and fill the repeatBlock */
                repeat_ಖಂಡ=get_ದತ್ತಾಂಶ_ಖಂಡ(newTrie, ಆರಂಭ, ನಿಜ);
                ಆದರೆ(repeat_ಖಂಡ<0) {
                    *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಹಿಂತಿರುಗಿ;
                }
                write_ಖಂಡ(newTrie->ದತ್ತಾಂಶ+repeat_ಖಂಡ, value);
            }
        }

        ಆರಂಭ+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    }

    ಆದರೆ(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        ಖಂಡ=get_ದತ್ತಾಂಶ_ಖಂಡ(newTrie, ಆರಂಭ, ನಿಜ);
        ಆದರೆ(ಖಂಡ<0) {
            *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }

        fill_ಖಂಡ(newTrie->ದತ್ತಾಂಶ+ಖಂಡ, 0, rest, value, newTrie->initial_ಮೌಲ್ಯ, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
equal_int32(ಸ್ಥಿರಾಂಕ int32_t *s, ಸ್ಥಿರಾಂಕ int32_t *t, int32_t length) {
    ಆಗ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ಹಿಂತಿರುಗಿ length == 0;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
equal_uint32(ಸ್ಥಿರಾಂಕ uint32_t *s, ಸ್ಥಿರಾಂಕ uint32_t *t, int32_t length) {
    ಆಗ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ಹಿಂತಿರುಗಿ length == 0;
}

ಸ್ಥಿರ int32_t
ಕಂಡುಹಿಡಿ_Same_Index2_ಖಂಡ(ಸ್ಥಿರಾಂಕ int32_t *idx, int32_t index2_ಉದ್ದಳತೆ, int32_t other_ಖಂಡ) {
    int32_t ಖಂಡ;

    /* ensure that we do not even partially get past index2Length */
    index2_ಉದ್ದಳತೆ-=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ;

    ಗಾಗಿ(ಖಂಡ=0; ಖಂಡ<=index2_ಉದ್ದಳತೆ; ++ಖಂಡ) {
        ಆದರೆ(equal_int32(idx+ಖಂಡ, idx+other_ಖಂಡ, UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ)) {
            ಹಿಂತಿರುಗಿ ಖಂಡ;
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

ಸ್ಥಿರ int32_t
ಕಂಡುಹಿಡಿ_Same_ದತ್ತಾಂಶ_ಖಂಡ_2(ಸ್ಥಿರಾಂಕ uint32_t *ದತ್ತಾಂಶ, int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, int32_t other_ಖಂಡ, int32_t ಖಂಡ_ಉದ್ದಳತೆ) {
    int32_t ಖಂಡ;

    /* ensure that we do not even partially get past dataLength */
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ-=ಖಂಡ_ಉದ್ದಳತೆ;

    ಗಾಗಿ(ಖಂಡ=0; ಖಂಡ<=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ; ಖಂಡ+=UTRIE2_ದತ್ತಾಂಶ_GRANULARITY) {
        ಆದರೆ(equal_uint32(ದತ್ತಾಂಶ+ಖಂಡ, ದತ್ತಾಂಶ+other_ಖಂಡ, ಖಂಡ_ಉದ್ದಳತೆ)) {
            ಹಿಂತಿರುಗಿ ಖಂಡ;
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
ಸ್ಥಿರ UChar32
ಕಂಡುಹಿಡಿ_ಮೇಲಿನ_ಆರಂಭ(UNewTrie2 *trie, uint32_t ಮೇಲಿನ_ಮೌಲ್ಯ) {
    ಸ್ಥಿರಾಂಕ uint32_t *data32;

    uint32_t value, initial_ಮೌಲ್ಯ;
    UChar32 c, prev;
    int32_t i1, i2, j, i2_ಖಂಡ, prev_I2_ಖಂಡ, index2_Null_ಸ್ಥಾನಾಂತರ, ಖಂಡ, prev_ಖಂಡ, null_ಖಂಡ;

    data32=trie->ದತ್ತಾಂಶ;
    initial_ಮೌಲ್ಯ=trie->initial_ಮೌಲ್ಯ;

    index2_Null_ಸ್ಥಾನಾಂತರ=trie->index2_Null_ಸ್ಥಾನಾಂತರ;
    null_ಖಂಡ=trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;

    /* set variables for previous range */
    ಆದರೆ(ಮೇಲಿನ_ಮೌಲ್ಯ==initial_ಮೌಲ್ಯ) {
        prev_I2_ಖಂಡ=index2_Null_ಸ್ಥಾನಾಂತರ;
        prev_ಖಂಡ=null_ಖಂಡ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        prev_I2_ಖಂಡ=-1;
        prev_ಖಂಡ=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_ಸೂಚ್ಯಂಕ_1_ಉದ್ದಳತೆ;
    c=prev;
    ಆಗ(c>0) {
        i2_ಖಂಡ=trie->index1[--i1];
        ಆದರೆ(i2_ಖಂಡ==prev_I2_ಖಂಡ) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_1_ನಮೂದು;
            ಮುಂದುವರಿ;
        }
        prev_I2_ಖಂಡ=i2_ಖಂಡ;
        ಆದರೆ(i2_ಖಂಡ==index2_Null_ಸ್ಥಾನಾಂತರ) {
            /* this is the null index-2 block */
            ಆದರೆ(ಮೇಲಿನ_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                ಹಿಂತಿರುಗಿ c;
            }
            c-=UTRIE2_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_1_ನಮೂದು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* enumerate data blocks for one index-2 block */
            ಗಾಗಿ(i2=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ; i2>0;) {
                ಖಂಡ=trie->index2[i2_ಖಂಡ+ --i2];
                ಆದರೆ(ಖಂಡ==prev_ಖಂಡ) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
                    ಮುಂದುವರಿ;
                }
                prev_ಖಂಡ=ಖಂಡ;
                ಆದರೆ(ಖಂಡ==null_ಖಂಡ) {
                    /* this is the null data block */
                    ಆದರೆ(ಮೇಲಿನ_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                        ಹಿಂತಿರುಗಿ c;
                    }
                    c-=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಗಾಗಿ(j=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ; j>0;) {
                        value=data32[ಖಂಡ+ --j];
                        ಆದರೆ(value!=ಮೇಲಿನ_ಮೌಲ್ಯ) {
                            ಹಿಂತಿರುಗಿ c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Compact a build-time trie.
 *
 * The compaction
 * - removes blocks that are identical with earlier ones
 * - overlaps adjacent blocks as much as possible (if overlap==true)
 * - moves blocks in steps of the data granularity
 * - moves and overlaps blocks that overlap with multiple values in the overlap region
 *
 * It does not
 * - try to move and overlap blocks that are not already adjacent
 */
ಸ್ಥಿರ ಶೂನ್ಯ
compact_ದತ್ತಾಂಶ(UNewTrie2 *trie) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    int32_t ಎಣಿಕೆ_Same=0, ಮೊತ್ತ_Overlaps=0;
#ಆದರೆ_ಅಂತ್ಯ

    int32_t ಆರಂಭ, new_ಆರಂಭ, moved_ಆರಂಭ;
    int32_t ಖಂಡ_ಉದ್ದಳತೆ, overlap;
    int32_t i, map_ಸೂಚ್ಯಂಕ, ಖಂಡ_ಎಣಿಕೆ;

    /* do not compact linear-ASCII data */
    new_ಆರಂಭ=UTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ;
    ಗಾಗಿ(ಆರಂಭ=0, i=0; ಆರಂಭ<new_ಆರಂಭ; ಆರಂಭ+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ, ++i) {
        trie->map[i]=ಆರಂಭ;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    ಖಂಡ_ಉದ್ದಳತೆ=64;
    ಖಂಡ_ಎಣಿಕೆ=ಖಂಡ_ಉದ್ದಳತೆ>>UTRIE2_ಸರಿಸು_2;
    ಗಾಗಿ(ಆರಂಭ=new_ಆರಂಭ; ಆರಂಭ<trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        ಆದರೆ(ಆರಂಭ==UNEWTRIE2_ದತ್ತಾಂಶ_0800_ಸ್ಥಾನಾಂತರ) {
            ಖಂಡ_ಉದ್ದಳತೆ=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            ಖಂಡ_ಎಣಿಕೆ=1;
        }

        /* skip blocks that are not used */
        ಆದರೆ(trie->map[ಆರಂಭ>>UTRIE2_ಸರಿಸು_2]<=0) {
            /* advance start to the next block */
            ಆರಂಭ+=ಖಂಡ_ಉದ್ದಳತೆ;

            /* leave newStart with the previous block! */
            ಮುಂದುವರಿ;
        }

        /* search for an identical block */
        ಆದರೆ( (moved_ಆರಂಭ=ಕಂಡುಹಿಡಿ_Same_ದತ್ತಾಂಶ_ಖಂಡ_2(trie->ದತ್ತಾಂಶ, new_ಆರಂಭ, ಆರಂಭ, ಖಂಡ_ಉದ್ದಳತೆ))
             >=0
        ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
            ++ಎಣಿಕೆ_Same;
#ಆದರೆ_ಅಂತ್ಯ
            /* found an identical block, set the other block's index value for the current block */
            ಗಾಗಿ(i=ಖಂಡ_ಎಣಿಕೆ, map_ಸೂಚ್ಯಂಕ=ಆರಂಭ>>UTRIE2_ಸರಿಸು_2; i>0; --i) {
                trie->map[map_ಸೂಚ್ಯಂಕ++]=moved_ಆರಂಭ;
                moved_ಆರಂಭ+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            }

            /* advance start to the next block */
            ಆರಂಭ+=ಖಂಡ_ಉದ್ದಳತೆ;

            /* leave newStart with the previous block! */
            ಮುಂದುವರಿ;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        ಗಾಗಿ(overlap=ಖಂಡ_ಉದ್ದಳತೆ-UTRIE2_ದತ್ತಾಂಶ_GRANULARITY;
            overlap>0 && !equal_uint32(trie->ದತ್ತಾಂಶ+(new_ಆರಂಭ-overlap), trie->ದತ್ತಾಂಶ+ಆರಂಭ, overlap);
            overlap-=UTRIE2_ದತ್ತಾಂಶ_GRANULARITY) {}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
            ಮೊತ್ತ_Overlaps+=overlap;
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ(overlap>0 || new_ಆರಂಭ<ಆರಂಭ) {
            /* some overlap, or just move the whole block */
            moved_ಆರಂಭ=new_ಆರಂಭ-overlap;
            ಗಾಗಿ(i=ಖಂಡ_ಎಣಿಕೆ, map_ಸೂಚ್ಯಂಕ=ಆರಂಭ>>UTRIE2_ಸರಿಸು_2; i>0; --i) {
                trie->map[map_ಸೂಚ್ಯಂಕ++]=moved_ಆರಂಭ;
                moved_ಆರಂಭ+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            }

            /* move the non-overlapping indexes to their new positions */
            ಆರಂಭ+=overlap;
            ಗಾಗಿ(i=ಖಂಡ_ಉದ್ದಳತೆ-overlap; i>0; --i) {
                trie->ದತ್ತಾಂಶ[new_ಆರಂಭ++]=trie->ದತ್ತಾಂಶ[ಆರಂಭ++];
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* no overlap && newStart==start */ {
            ಗಾಗಿ(i=ಖಂಡ_ಎಣಿಕೆ, map_ಸೂಚ್ಯಂಕ=ಆರಂಭ>>UTRIE2_ಸರಿಸು_2; i>0; --i) {
                trie->map[map_ಸೂಚ್ಯಂಕ++]=ಆರಂಭ;
                ಆರಂಭ+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            }
            new_ಆರಂಭ=ಆರಂಭ;
        }
    }

    /* now adjust the index-2 table */
    ಗಾಗಿ(i=0; i<trie->index2_ಉದ್ದಳತೆ; ++i) {
        ಆದರೆ(i==UNEWTRIE2_ಸೂಚ್ಯಂಕ_GAP_ಸ್ಥಾನಾಂತರ) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_ಸೂಚ್ಯಂಕ_GAP_ಉದ್ದಳತೆ;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_ಸರಿಸು_2];
    }
    trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ=trie->map[trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ>>UTRIE2_ಸರಿಸು_2];

    /* ensure dataLength alignment */
    ಆಗ((new_ಆರಂಭ&(UTRIE2_ದತ್ತಾಂಶ_GRANULARITY-1))!=0) {
        trie->ದತ್ತಾಂಶ[new_ಆರಂಭ++]=trie->initial_ಮೌಲ್ಯ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    /* we saved some space */
    ಬರೆ("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (ದೀರ್ಘ)trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, (ದೀರ್ಘ)new_ಆರಂಭ, (ದೀರ್ಘ)ಎಣಿಕೆ_Same, (ದೀರ್ಘ)ಮೊತ್ತ_Overlaps);
#ಆದರೆ_ಅಂತ್ಯ

    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=new_ಆರಂಭ;
}

ಸ್ಥಿರ ಶೂನ್ಯ
compactIndex2(UNewTrie2 *trie) {
    int32_t i, ಆರಂಭ, new_ಆರಂಭ, moved_ಆರಂಭ, overlap;

    /* do not compact linear-BMP index-2 blocks */
    new_ಆರಂಭ=UTRIE2_ಸೂಚ್ಯಂಕ_2_BMP_ಉದ್ದಳತೆ;
    ಗಾಗಿ(ಆರಂಭ=0, i=0; ಆರಂಭ<new_ಆರಂಭ; ಆರಂಭ+=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ, ++i) {
        trie->map[i]=ಆರಂಭ;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    new_ಆರಂಭ+=UTRIE2_UTF8_2_B_ಸೂಚ್ಯಂಕ_2_ಉದ್ದಳತೆ+((trie->ಮೇಲಿನ_ಆರಂಭ-0x10000)>>UTRIE2_ಸರಿಸು_1);

    ಗಾಗಿ(ಆರಂಭ=UNEWTRIE2_ಸೂಚ್ಯಂಕ_2_NULL_ಸ್ಥಾನಾಂತರ; ಆರಂಭ<trie->index2_ಉದ್ದಳತೆ;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        ಆದರೆ( (moved_ಆರಂಭ=ಕಂಡುಹಿಡಿ_Same_Index2_ಖಂಡ(trie->index2, new_ಆರಂಭ, ಆರಂಭ))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[ಆರಂಭ>>UTRIE2_ಸರಿಸು_1_2]=moved_ಆರಂಭ;

            /* advance start to the next block */
            ಆರಂಭ+=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ;

            /* leave newStart with the previous block! */
            ಮುಂದುವರಿ;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        ಗಾಗಿ(overlap=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ-1;
            overlap>0 && !equal_int32(trie->index2+(new_ಆರಂಭ-overlap), trie->index2+ಆರಂಭ, overlap);
            --overlap) {}

        ಆದರೆ(overlap>0 || new_ಆರಂಭ<ಆರಂಭ) {
            /* some overlap, or just move the whole block */
            trie->map[ಆರಂಭ>>UTRIE2_ಸರಿಸು_1_2]=new_ಆರಂಭ-overlap;

            /* move the non-overlapping indexes to their new positions */
            ಆರಂಭ+=overlap;
            ಗಾಗಿ(i=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ-overlap; i>0; --i) {
                trie->index2[new_ಆರಂಭ++]=trie->index2[ಆರಂಭ++];
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* no overlap && newStart==start */ {
            trie->map[ಆರಂಭ>>UTRIE2_ಸರಿಸು_1_2]=ಆರಂಭ;
            ಆರಂಭ+=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ;
            new_ಆರಂಭ=ಆರಂಭ;
        }
    }

    /* now adjust the index-1 table */
    ಗಾಗಿ(i=0; i<UNEWTRIE2_ಸೂಚ್ಯಂಕ_1_ಉದ್ದಳತೆ; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_ಸರಿಸು_1_2];
    }
    trie->index2_Null_ಸ್ಥಾನಾಂತರ=trie->map[trie->index2_Null_ಸ್ಥಾನಾಂತರ>>UTRIE2_ಸರಿಸು_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    ಆಗ((new_ಆರಂಭ&((UTRIE2_ದತ್ತಾಂಶ_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[new_ಆರಂಭ++] = static_cast<int32_t>(0xffff) << UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    /* we saved some space */
    ಬರೆ("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (ದೀರ್ಘ)trie->index2_ಉದ್ದಳತೆ, (ದೀರ್ಘ)new_ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ

    trie->index2_ಉದ್ದಳತೆ=new_ಆರಂಭ;
}

ಸ್ಥಿರ ಶೂನ್ಯ
compactTrie(UTrie2 *trie, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UNewTrie2 *newTrie;
    UChar32 ಮೇಲಿನ_ಆರಂಭ, supp_ಮೇಲಿನ_ಆರಂಭ;
    uint32_t ಮೇಲಿನ_ಮೌಲ್ಯ;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    ಮೇಲಿನ_ಮೌಲ್ಯ=utrie2_get32(trie, 0x10ffff);
    ಮೇಲಿನ_ಆರಂಭ=ಕಂಡುಹಿಡಿ_ಮೇಲಿನ_ಆರಂಭ(newTrie, ಮೇಲಿನ_ಮೌಲ್ಯ);
    ಮೇಲಿನ_ಆರಂಭ=(ಮೇಲಿನ_ಆರಂಭ+(UTRIE2_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_1_ನಮೂದು-1))&~(UTRIE2_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_1_ನಮೂದು-1);
    ಆದರೆ(ಮೇಲಿನ_ಆರಂಭ==0x110000) {
        ಮೇಲಿನ_ಮೌಲ್ಯ=trie->error_ಮೌಲ್ಯ;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->ಮೇಲಿನ_ಆರಂಭ=newTrie->ಮೇಲಿನ_ಆರಂಭ=ಮೇಲಿನ_ಆರಂಭ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    ಬರೆ("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (ದೀರ್ಘ)ಮೇಲಿನ_ಆರಂಭ, (ದೀರ್ಘ)ಮೇಲಿನ_ಮೌಲ್ಯ, (ದೀರ್ಘ)trie->initial_ಮೌಲ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(ಮೇಲಿನ_ಆರಂಭ<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_ಮೇಲಿನ_ಆರಂಭ= ಮೇಲಿನ_ಆರಂಭ<=0x10000 ? 0x10000 : ಮೇಲಿನ_ಆರಂಭ;
        utrie2_ಗಣ_Range32(trie, supp_ಮೇಲಿನ_ಆರಂಭ, 0x10ffff, trie->initial_ಮೌಲ್ಯ, ನಿಜ, p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
    }

    compact_ದತ್ತಾಂಶ(newTrie);
    ಆದರೆ(ಮೇಲಿನ_ಆರಂಭ>0x10000) {
        compactIndex2(newTrie);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (ದೀರ್ಘ)ಮೇಲಿನ_ಆರಂಭ, (ದೀರ್ಘ)trie->newTrie->index2_ಉದ್ದಳತೆ, (ದೀರ್ಘ)UTRIE2_ಸೂಚ್ಯಂಕ_1_ಸ್ಥಾನಾಂತರ);
#ಆದರೆ_ಅಂತ್ಯ
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->ದತ್ತಾಂಶ[newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++]=ಮೇಲಿನ_ಮೌಲ್ಯ;
    ಆಗ((newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ&(UTRIE2_ದತ್ತಾಂಶ_GRANULARITY-1))!=0) {
        newTrie->ದತ್ತಾಂಶ[newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ++]=trie->initial_ಮೌಲ್ಯ;
    }

    newTrie->isCompacted=ನಿಜ;
}

/* serialization ------------------------------------------------------------ */

/**
 * Maximum length of the runtime index array.
 * Limited by its own 16-bit index values, and by uint16_t UTrie2Header.indexLength.
 * (The actual maximum length is lower,
 * (0x110000>>UTRIE2_SHIFT_2)+UTRIE2_UTF8_2B_INDEX_2_LENGTH+UTRIE2_MAX_INDEX_1_LENGTH.)
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UTRIE2_MAX_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು UTRIE2_MAX_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ (0xffff<<UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು)

/* Compact and internally serialize the trie. */
U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UNewTrie2 *newTrie;
    UTrie2_ಶೀರ್ಷಿಕೆ *ಶೀರ್ಷಿಕೆ;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_ಉದ್ದಳತೆ;
    int32_t ದತ್ತಾಂಶ_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 ಮೇಲಿನ_ಆರಂಭ;

    /* argument check */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ( trie==ಶೂನ್ಯಸೂಚಕ ||
        ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು<0 || UTRIE2_ಎಣಿಕೆ_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು<=ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    newTrie=trie->newTrie;
    ಆದರೆ(newTrie==ಶೂನ್ಯಸೂಚಕ) {
        /* already frozen */
        UTrie2_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು frozen_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು=
            trie->data16!=ಶೂನ್ಯಸೂಚಕ ? UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು : UTRIE2_32_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು;
        ಆದರೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು!=frozen_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
            *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        }
        ಹಿಂತಿರುಗಿ;
    }

    /* compact if necessary */
    ಆದರೆ(!newTrie->isCompacted) {
        compactTrie(trie, p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
    }
    ಮೇಲಿನ_ಆರಂಭ=trie->ಮೇಲಿನ_ಆರಂಭ;

    ಆದರೆ(ಮೇಲಿನ_ಆರಂಭ<=0x10000) {
        all_Indexes_ಉದ್ದಳತೆ=UTRIE2_ಸೂಚ್ಯಂಕ_1_ಸ್ಥಾನಾಂತರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        all_Indexes_ಉದ್ದಳತೆ=newTrie->index2_ಉದ್ದಳತೆ;
    }
    ಆದರೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು==UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
        ದತ್ತಾಂಶ_Move=all_Indexes_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ದತ್ತಾಂಶ_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    ಆದರೆ( /* for unshifted indexLength */
        all_Indexes_ಉದ್ದಳತೆ>UTRIE2_MAX_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ ||
        /* for unshifted dataNullOffset */
        (ದತ್ತಾಂಶ_Move+newTrie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (ದತ್ತಾಂಶ_Move+UNEWTRIE2_ದತ್ತಾಂಶ_0800_ಸ್ಥಾನಾಂತರ)>0xffff ||
        /* for shiftedDataLength */
        (ದತ್ತಾಂಶ_Move+newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ)>UTRIE2_MAX_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ
    ) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    /* calculate the total serialized length */
    length=ಗಾತ್ರ(UTrie2_ಶೀರ್ಷಿಕೆ)+all_Indexes_ಉದ್ದಳತೆ*2;
    ಆದರೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು==UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
        length+=newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        length+=newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4;
    }

    trie->ಸ್ಮರಣೆ=uprv_malloc(length);
    ಆದರೆ(trie->ಸ್ಮರಣೆ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    trie->length=length;
    trie->is_ಸ್ಮರಣೆ_Owned=ನಿಜ;

    trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=all_Indexes_ಉದ್ದಳತೆ;
    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ಆದರೆ(ಮೇಲಿನ_ಆರಂಭ<=0x10000) {
        trie->index2_Null_ಸ್ಥಾನಾಂತರ=0xffff;
    } ಇಲ್ಲದಿದ್ದರೆ {
        trie->index2_Null_ಸ್ಥಾನಾಂತರ=static_cast<uint16_t>(UTRIE2_ಸೂಚ್ಯಂಕ_2_ಸ್ಥಾನಾಂತರ+newTrie->index2_Null_ಸ್ಥಾನಾಂತರ);
    }
    trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ=(uint16_t)(ದತ್ತಾಂಶ_Move+newTrie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ);
    trie->ಮೇಲಿನ_ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ=ದತ್ತಾಂಶ_Move+trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ-UTRIE2_ದತ್ತಾಂಶ_GRANULARITY;

    /* set the header fields */
    ಶೀರ್ಷಿಕೆ=(UTrie2_ಶೀರ್ಷಿಕೆ *)trie->ಸ್ಮರಣೆ;

    ಶೀರ್ಷಿಕೆ->signature=UTRIE2_SIG; /* "Tri2" */
    ಶೀರ್ಷಿಕೆ->options=(uint16_t)ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು;

    ಶೀರ್ಷಿಕೆ->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=(uint16_t)trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ಶೀರ್ಷಿಕೆ->shifted_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=(uint16_t)(trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ>>UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
    ಶೀರ್ಷಿಕೆ->index2_Null_ಸ್ಥಾನಾಂತರ=trie->index2_Null_ಸ್ಥಾನಾಂತರ;
    ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ=trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
    ಶೀರ್ಷಿಕೆ->shifted_ಮೇಲಿನ_ಆರಂಭ=(uint16_t)(ಮೇಲಿನ_ಆರಂಭ>>UTRIE2_ಸರಿಸು_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(ಶೀರ್ಷಿಕೆ+1);
    trie->ಸೂಚ್ಯಂಕ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    ಗಾಗಿ(i=UTRIE2_ಸೂಚ್ಯಂಕ_2_BMP_ಉದ್ದಳತೆ; i>0; --i) {
        *dest16++=(uint16_t)((ದತ್ತಾಂಶ_Move + *p++)>>UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    ಗಾಗಿ(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(ದತ್ತಾಂಶ_Move+UTRIE2_BAD_UTF8_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
    }
    ಗಾಗಿ(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(ದತ್ತಾಂಶ_Move+newTrie->index2[i<<(6-UTRIE2_ಸರಿಸು_2)]);
    }

    ಆದರೆ(ಮೇಲಿನ_ಆರಂಭ>0x10000) {
        int32_t index1_ಉದ್ದಳತೆ=(ಮೇಲಿನ_ಆರಂಭ-0x10000)>>UTRIE2_ಸರಿಸು_1;
        int32_t index2_ಸ್ಥಾನಾಂತರ=UTRIE2_ಸೂಚ್ಯಂಕ_2_BMP_ಉದ್ದಳತೆ+UTRIE2_UTF8_2_B_ಸೂಚ್ಯಂಕ_2_ಉದ್ದಳತೆ+index1_ಉದ್ದಳತೆ;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_ಸೂಚ್ಯಂಕ_1_ಉದ್ದಳತೆ;
        ಗಾಗಿ(i=index1_ಉದ್ದಳತೆ; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_ಸೂಚ್ಯಂಕ_2_ಸ್ಥಾನಾಂತರ + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2_ಸ್ಥಾನಾಂತರ;
        ಗಾಗಿ(i=newTrie->index2_ಉದ್ದಳತೆ-index2_ಸ್ಥಾನಾಂತರ; i>0; --i) {
            *dest16++=(uint16_t)((ದತ್ತಾಂಶ_Move + *p++)>>UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
        }
    }

    /* write the 16/32-bit data array */
    ಆಯ್ಕೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
    ಸ್ಥಿತಿ UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=ಶೂನ್ಯಸೂಚಕ;
        p=newTrie->ದತ್ತಾಂಶ;
        ಗಾಗಿ(i=newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UTRIE2_32_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
        /* write 32-bit data values */
        trie->data16=ಶೂನ್ಯಸೂಚಕ;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->ದತ್ತಾಂಶ, (size_t)newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
    umutablecptrie_ಗಣ_Name(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_ಮೌಲ್ಯ_ಅಗಲ)ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು, p_Error_ಸಂಕೇತ));
#ಆದರೆ_ಅಂತ್ಯ
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->ದತ್ತಾಂಶ);
    uprv_free(newTrie);
    trie->newTrie=ಶೂನ್ಯಸೂಚಕ;
}

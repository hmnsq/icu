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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
#   include <stdio.h>
#ਜੇ_ਅੰਤ
// #define UCPTRIE_DEBUG

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/umutablecptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucptrie_impl.h"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie2_impl.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "utrie.h"  // for utrie2_fromUTrie()

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

ਗਿਣਤੀ {
    /** The null index-2 block, following the gap in the index-2 table. */
    UNEWTRIE2_ਸੂਚਕਾਂਕ_2_NULL_ਵਿੱਥ=UNEWTRIE2_ਸੂਚਕਾਂਕ_GAP_ਵਿੱਥ+UNEWTRIE2_ਸੂਚਕਾਂਕ_GAP_ਲੰਮਾਈ,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_ਸੂਚਕਾਂਕ_2_ਸ਼ੁਰੂ_ਵਿੱਥ=UNEWTRIE2_ਸੂਚਕਾਂਕ_2_NULL_ਵਿੱਥ+UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_ਅੰਕੜੇ_NULL_ਵਿੱਥ=UTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ,

    /** The start of allocated data blocks. */
    UNEWTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ=UNEWTRIE2_ਅੰਕੜੇ_NULL_ਵਿੱਥ+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_ਅੰਕੜੇ_0800_ਵਿੱਥ=UNEWTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ+0x780
};

/* Start with allocation of 16k data entries. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNEWTRIE2_INITIAL_ਅੰਕੜੇ_ਲੰਮਾਈ ((int32_t)1<<14)

/* Grow about 8x each time. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNEWTRIE2_MEDIUM_ਅੰਕੜੇ_ਲੰਮਾਈ ((int32_t)1<<17)

ਸਥਿਰ int32_t
alloc_Index2_ਖੰਡ(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_ਮੁੱਲ, uint32_t error_ਮੁੱਲ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *ਅੰਕੜੇ;
    int32_t i, j;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    trie=(UTrie2 *)uprv_malloc(ਆਕਾਰ(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(ਆਕਾਰ(UNewTrie2));
    ਅੰਕੜੇ=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_ਅੰਕੜੇ_ਲੰਮਾਈ*4);
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ || newTrie==ਖਾਲੀ_ਸੂਚਕ || ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(ਅੰਕੜੇ);
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    uprv_memset(trie, 0, ਆਕਾਰ(UTrie2));
    trie->initial_ਮੁੱਲ=initial_ਮੁੱਲ;
    trie->error_ਮੁੱਲ=error_ਮੁੱਲ;
    trie->ਉੱਚਾ_ਸ਼ੁਰੂ=0x110000;
    trie->newTrie=newTrie;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    trie->name="open";
#ਜੇ_ਅੰਤ

    newTrie->ਅੰਕੜੇ=ਅੰਕੜੇ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_ਮੁੱਲ, error_ਮੁੱਲ, p_Error_ਕੋਡ);
#ਜੇ_ਅੰਤ
    newTrie->ਅੰਕੜੇ_ਸਮਰੱਥਾ=UNEWTRIE2_INITIAL_ਅੰਕੜੇ_ਲੰਮਾਈ;
    newTrie->initial_ਮੁੱਲ=initial_ਮੁੱਲ;
    newTrie->error_ਮੁੱਲ=error_ਮੁੱਲ;
    newTrie->ਉੱਚਾ_ਸ਼ੁਰੂ=0x110000;
    newTrie->ਪਹਿਲਾ_Free_ਖੰਡ=0;  /* no free block in the list */
    newTrie->isCompacted=ਝੂਠ;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    ਲਈ(i=0; i<0x80; ++i) {
        newTrie->ਅੰਕੜੇ[i]=initial_ਮੁੱਲ;
    }
    ਲਈ(; i<0xc0; ++i) {
        newTrie->ਅੰਕੜੇ[i]=error_ਮੁੱਲ;
    }
    ਲਈ(i=UNEWTRIE2_ਅੰਕੜੇ_NULL_ਵਿੱਥ; i<UNEWTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ; ++i) {
        newTrie->ਅੰਕੜੇ[i]=initial_ਮੁੱਲ;
    }
    newTrie->ਅੰਕੜੇ_Null_ਵਿੱਥ=UNEWTRIE2_ਅੰਕੜੇ_NULL_ਵਿੱਥ;
    newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ=UNEWTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    ਲਈ(i=0, j=0; j<0x80; ++i, j+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    ਲਈ(; j<0xc0; ++i, j+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_ਖਿਸਕਾਓ_2)-
        (0x80>>UTRIE2_ਖਿਸਕਾਓ_2)+
        1+
        UTRIE2_LSCP_ਸੂਚਕਾਂਕ_2_ਲੰਮਾਈ;
    j+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    ਲਈ(; j<UNEWTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ; ++i, j+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    ਲਈ(i=0x80>>UTRIE2_ਖਿਸਕਾਓ_2; i<UTRIE2_ਸੂਚਕਾਂਕ_2_BMP_ਲੰਮਾਈ; ++i) {
        newTrie->index2[i]=UNEWTRIE2_ਅੰਕੜੇ_NULL_ਵਿੱਥ;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    ਲਈ(i=0; i<UNEWTRIE2_ਸੂਚਕਾਂਕ_GAP_ਲੰਮਾਈ; ++i) {
        newTrie->index2[UNEWTRIE2_ਸੂਚਕਾਂਕ_GAP_ਵਿੱਥ+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    ਲਈ(i=0; i<UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ; ++i) {
        newTrie->index2[UNEWTRIE2_ਸੂਚਕਾਂਕ_2_NULL_ਵਿੱਥ+i]=UNEWTRIE2_ਅੰਕੜੇ_NULL_ਵਿੱਥ;
    }
    newTrie->index2_Null_ਵਿੱਥ=UNEWTRIE2_ਸੂਚਕਾਂਕ_2_NULL_ਵਿੱਥ;
    newTrie->index2_ਲੰਮਾਈ=UNEWTRIE2_ਸੂਚਕਾਂਕ_2_ਸ਼ੁਰੂ_ਵਿੱਥ;

    /* set the index-1 indexes for the linear index-2 block */
    ਲਈ(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_ਸੂਚਕਾਂਕ_1_ਲੰਮਾਈ;
        ++i, j+=UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    ਲਈ(; i<UNEWTRIE2_ਸੂਚਕਾਂਕ_1_ਲੰਮਾਈ; ++i) {
        newTrie->index1[i]=UNEWTRIE2_ਸੂਚਕਾਂਕ_2_NULL_ਵਿੱਥ;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    ਲਈ(i=0x80; i<0x800; i+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ) {
        utrie2_set32(trie, i, initial_ਮੁੱਲ, p_Error_ਕੋਡ);
    }

    ਵਾਪਸ trie;
}

ਸਥਿਰ UNewTrie2 *
cloneBuilder(ਅਚਲ UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(ਆਕਾਰ(UNewTrie2)));
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    trie->ਅੰਕੜੇ = static_cast<uint32_t*>(uprv_malloc(other->ਅੰਕੜੇ_ਸਮਰੱਥਾ * 4));
    ਜੇ(trie->ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(trie);
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    ਜੇ(other->t3==ਖਾਲੀ_ਸੂਚਕ) {
        trie->t3=ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &error_ਕੋਡ);
    }
#ਜੇ_ਅੰਤ
    trie->ਅੰਕੜੇ_ਸਮਰੱਥਾ=other->ਅੰਕੜੇ_ਸਮਰੱਥਾ;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, ਆਕਾਰ(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_ਲੰਮਾਈ*4);
    trie->index2_Null_ਵਿੱਥ=other->index2_Null_ਵਿੱਥ;
    trie->index2_ਲੰਮਾਈ=other->index2_ਲੰਮਾਈ;

    uprv_memcpy(trie->ਅੰਕੜੇ, other->ਅੰਕੜੇ, (size_t)other->ਅੰਕੜੇ_ਲੰਮਾਈ*4);
    trie->ਅੰਕੜੇ_Null_ਵਿੱਥ=other->ਅੰਕੜੇ_Null_ਵਿੱਥ;
    trie->ਅੰਕੜੇ_ਲੰਮਾਈ=other->ਅੰਕੜੇ_ਲੰਮਾਈ;

    /* reference counters */
    ਜੇ(other->isCompacted) {
        trie->ਪਹਿਲਾ_Free_ਖੰਡ=0;
    } ਨਹੀਂਤਾਂ {
        uprv_memcpy(trie->map, other->map, ((size_t)other->ਅੰਕੜੇ_ਲੰਮਾਈ>>UTRIE2_ਖਿਸਕਾਓ_2)*4);
        trie->ਪਹਿਲਾ_Free_ਖੰਡ=other->ਪਹਿਲਾ_Free_ਖੰਡ;
    }

    trie->initial_ਮੁੱਲ=other->initial_ਮੁੱਲ;
    trie->error_ਮੁੱਲ=other->error_ਮੁੱਲ;
    trie->ਉੱਚਾ_ਸ਼ੁਰੂ=other->ਉੱਚਾ_ਸ਼ੁਰੂ;
    trie->isCompacted=other->isCompacted;

    ਵਾਪਸ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ਅਚਲ UTrie2 *other, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UTrie2 *trie;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(other==ਖਾਲੀ_ਸੂਚਕ || (other->ਸਮ੍ਰਿਤੀ==ਖਾਲੀ_ਸੂਚਕ && other->newTrie==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    trie=(UTrie2 *)uprv_malloc(ਆਕਾਰ(UTrie2));
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memcpy(trie, other, ਆਕਾਰ(UTrie2));

    ਜੇ(other->ਸਮ੍ਰਿਤੀ!=ਖਾਲੀ_ਸੂਚਕ) {
        trie->ਸਮ੍ਰਿਤੀ=uprv_malloc(other->length);
        ਜੇ(trie->ਸਮ੍ਰਿਤੀ!=ਖਾਲੀ_ਸੂਚਕ) {
            trie->is_ਸਮ੍ਰਿਤੀ_Owned=ਸੱਚ;
            uprv_memcpy(trie->ਸਮ੍ਰਿਤੀ, other->ਸਮ੍ਰਿਤੀ, other->length);

            /* make the clone's pointers point to its own memory */
            trie->ਸੂਚਕਾਂਕ=(uint16_t *)trie->ਸਮ੍ਰਿਤੀ+(other->ਸੂਚਕਾਂਕ-(uint16_t *)other->ਸਮ੍ਰਿਤੀ);
            ਜੇ(other->data16!=ਖਾਲੀ_ਸੂਚਕ) {
                trie->data16=(uint16_t *)trie->ਸਮ੍ਰਿਤੀ+(other->data16-(uint16_t *)other->ਸਮ੍ਰਿਤੀ);
            }
            ਜੇ(other->data32!=ਖਾਲੀ_ਸੂਚਕ) {
                trie->data32=(uint32_t *)trie->ਸਮ੍ਰਿਤੀ+(other->data32-(uint32_t *)other->ਸਮ੍ਰਿਤੀ);
            }
        }
    } ਨਹੀਂਤਾਂ /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    ਜੇ(trie->ਸਮ੍ਰਿਤੀ==ਖਾਲੀ_ਸੂਚਕ && trie->newTrie==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        uprv_free(trie);
        trie=ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ trie;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ New_Trie_And_ਸਥਿਤੀ {
    UTrie2 *trie;
    UError_ਕੋਡ error_ਕੋਡ;
    UBool exclusive_ਹੱਦਬੰਦੀ;  /* rather than inclusive range end */
} New_Trie_And_ਸਥਿਤੀ;

ਸਥਿਰ UBool U_CALLCONV
copyEnumRange(ਅਚਲ ਖਾਲੀ *context, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ, uint32_t value) {
    New_Trie_And_ਸਥਿਤੀ *nt=(New_Trie_And_ਸਥਿਤੀ *)context;
    ਜੇ(value!=nt->trie->initial_ਮੁੱਲ) {
        ਜੇ(nt->exclusive_ਹੱਦਬੰਦੀ) {
            --ਸਿਰਾ;
        }
        ਜੇ(ਸ਼ੁਰੂ==ਸਿਰਾ) {
            utrie2_set32(nt->trie, ਸ਼ੁਰੂ, value, &nt->error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            utrie2_ਸੈੱਟ_Range32(nt->trie, ਸ਼ੁਰੂ, ਸਿਰਾ, value, ਸੱਚ, &nt->error_ਕੋਡ);
        }
        ਵਾਪਸ U_SUCCESS(nt->error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਸੱਚ;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
ਸਥਿਰ ਲੰਮਾ ਗਿਣਤੀ_Initial(ਅਚਲ UTrie2 *trie) {
    uint32_t initial_ਮੁੱਲ=trie->initial_ਮੁੱਲ;
    int32_t length=trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਲੰਮਾ count=0;
    ਜੇ(trie->data16!=ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ(int32_t i=0; i<length; ++i) {
            ਜੇ(trie->data16[i]==initial_ਮੁੱਲ) { ++count; }
        }
    } ਨਹੀਂਤਾਂ {
        ਲਈ(int32_t i=0; i<length; ++i) {
            ਜੇ(trie->data32[i]==initial_ਮੁੱਲ) { ++count; }
        }
    }
    ਵਾਪਸ count;
}

ਸਥਿਰ ਖਾਲੀ
utrie_printLengths(ਅਚਲ UTrie *trie) {
    ਲੰਮਾ ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਲੰਮਾ ਅੰਕੜੇ_ਲੰਮਾਈ=(ਲੰਮਾ)trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਲੰਮਾ ਕੁੱਲ_ਲੰਮਾਈ=(ਲੰਮਾ)ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ)+ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2+ਅੰਕੜੇ_ਲੰਮਾਈ*(trie->data32!=ਖਾਲੀ_ਸੂਚਕ ? 4 : 2);
    ਲਿਖੋ("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਅੰਕੜੇ_ਲੰਮਾਈ, ਕੁੱਲ_ਲੰਮਾਈ);
}

ਸਥਿਰ ਖਾਲੀ
utrie2_printLengths(ਅਚਲ UTrie2 *trie, ਅਚਲ ਅੱਖਰ *which) {
    ਲੰਮਾ ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਲੰਮਾ ਅੰਕੜੇ_ਲੰਮਾਈ=(ਲੰਮਾ)trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਲੰਮਾ ਕੁੱਲ_ਲੰਮਾਈ=(ਲੰਮਾ)ਆਕਾਰ(UTrie2_ਸਿਰਲੇਖ)+ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2+ਅੰਕੜੇ_ਲੰਮਾਈ*(trie->data32!=ਖਾਲੀ_ਸੂਚਕ ? 4 : 2);
    ਲਿਖੋ("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਅੰਕੜੇ_ਲੰਮਾਈ, ਗਿਣਤੀ_Initial(trie), ਕੁੱਲ_ਲੰਮਾਈ);
}
#ਜੇ_ਅੰਤ

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ਅਚਲ UTrie2 *other, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    New_Trie_And_ਸਥਿਤੀ context;
    char16_t lead;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(other==ਖਾਲੀ_ਸੂਚਕ || (other->ਸਮ੍ਰਿਤੀ==ਖਾਲੀ_ਸੂਚਕ && other->newTrie==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(other->newTrie!=ਖਾਲੀ_ਸੂਚਕ && !other->newTrie->isCompacted) {
        ਵਾਪਸ utrie2_clone(other, p_Error_ਕੋਡ);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_ਮੁੱਲ, other->error_ਮੁੱਲ, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    context.exclusive_ਹੱਦਬੰਦੀ=ਝੂਠ;
    context.error_ਕੋਡ=*p_Error_ਕੋਡ;
    utrie2_enum(other, ਖਾਲੀ_ਸੂਚਕ, copyEnumRange, &context);
    *p_Error_ਕੋਡ=context.error_ਕੋਡ;
    ਲਈ(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        ਜੇ(other->data32==ਖਾਲੀ_ਸੂਚਕ) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } ਨਹੀਂਤਾਂ {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        ਜੇ(value!=other->initial_ਮੁੱਲ) {
            utrie2_set32_For_Lead_Surrogate_ਕੋਡ_Unit(context.trie, lead, value, p_Error_ਕੋਡ);
        }
    }
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        utrie2_close(context.trie);
        context.trie=ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ਅਚਲ UTrie *trie1, uint32_t error_ਮੁੱਲ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    New_Trie_And_ਸਥਿਤੀ context;
    char16_t lead;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(trie1==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    context.trie=utrie2_open(trie1->initial_ਮੁੱਲ, error_ਮੁੱਲ, p_Error_ਕੋਡ);
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    context.exclusive_ਹੱਦਬੰਦੀ=ਸੱਚ;
    context.error_ਕੋਡ=*p_Error_ਕੋਡ;
    utrie_enum(trie1, ਖਾਲੀ_ਸੂਚਕ, copyEnumRange, &context);
    *p_Error_ਕੋਡ=context.error_ਕੋਡ;
    ਲਈ(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        ਜੇ(trie1->data32==ਖਾਲੀ_ਸੂਚਕ) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } ਨਹੀਂਤਾਂ {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        ਜੇ(value!=trie1->initial_ਮੁੱਲ) {
            utrie2_set32_For_Lead_Surrogate_ਕੋਡ_Unit(context.trie, lead, value, p_Error_ਕੋਡ);
        }
    }
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=ਖਾਲੀ_ਸੂਚਕ ? UTRIE2_32_ਮੁੱਲ_ਬਿੱਟ : UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ,
                      p_Error_ਕੋਡ);
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    ਜੇ(U_SUCCESS(*p_Error_ਕੋਡ)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#ਜੇ_ਅੰਤ
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        utrie2_close(context.trie);
        context.trie=ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ context.trie;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
is_ਵਿੱਚ_Null_ਖੰਡ(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, ਖੰਡ;

    ਜੇ(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_ਸੂਚਕਾਂਕ_2_ਵਿੱਥ-(0xd800>>UTRIE2_ਖਿਸਕਾਓ_2))+
            (c>>UTRIE2_ਖਿਸਕਾਓ_2);
    } ਨਹੀਂਤਾਂ {
        i2=trie->index1[c>>UTRIE2_ਖਿਸਕਾਓ_1]+
            ((c>>UTRIE2_ਖਿਸਕਾਓ_2)&UTRIE2_ਸੂਚਕਾਂਕ_2_ਨਕਾਬ);
    }
    ਖੰਡ=trie->index2[i2];
    ਵਾਪਸ ਖੰਡ == trie->ਅੰਕੜੇ_Null_ਵਿੱਥ;
}

ਸਥਿਰ int32_t
alloc_Index2_ਖੰਡ(UNewTrie2 *trie) {
    int32_t new_ਖੰਡ, new_ਸਿਖਰ;

    new_ਖੰਡ=trie->index2_ਲੰਮਾਈ;
    new_ਸਿਖਰ=new_ਖੰਡ+UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ;
    ਜੇ(new_ਸਿਖਰ>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        ਵਾਪਸ -1;
    }
    trie->index2_ਲੰਮਾਈ=new_ਸਿਖਰ;
    uprv_memcpy(trie->index2+new_ਖੰਡ, trie->index2+trie->index2_Null_ਵਿੱਥ, UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ*4);
    ਵਾਪਸ new_ਖੰਡ;
}

ਸਥਿਰ int32_t
get_Index2_ਖੰਡ(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    ਜੇ(U_IS_LEAD(c) && forLSCP) {
        ਵਾਪਸ UTRIE2_LSCP_ਸੂਚਕਾਂਕ_2_ਵਿੱਥ;
    }

    i1=c>>UTRIE2_ਖਿਸਕਾਓ_1;
    i2=trie->index1[i1];
    ਜੇ(i2==trie->index2_Null_ਵਿੱਥ) {
        i2=alloc_Index2_ਖੰਡ(trie);
        ਜੇ(i2<0) {
            ਵਾਪਸ -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    ਵਾਪਸ i2;
}

ਸਥਿਰ int32_t
alloc_ਅੰਕੜੇ_ਖੰਡ(UNewTrie2 *trie, int32_t copy_ਖੰਡ) {
    int32_t new_ਖੰਡ, new_ਸਿਖਰ;

    ਜੇ(trie->ਪਹਿਲਾ_Free_ਖੰਡ!=0) {
        /* get the first free block */
        new_ਖੰਡ=trie->ਪਹਿਲਾ_Free_ਖੰਡ;
        trie->ਪਹਿਲਾ_Free_ਖੰਡ=-trie->map[new_ਖੰਡ>>UTRIE2_ਖਿਸਕਾਓ_2];
    } ਨਹੀਂਤਾਂ {
        /* get a new block from the high end */
        new_ਖੰਡ=trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
        new_ਸਿਖਰ=new_ਖੰਡ+UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        ਜੇ(new_ਸਿਖਰ>trie->ਅੰਕੜੇ_ਸਮਰੱਥਾ) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *ਅੰਕੜੇ;

            ਜੇ(trie->ਅੰਕੜੇ_ਸਮਰੱਥਾ<UNEWTRIE2_MEDIUM_ਅੰਕੜੇ_ਲੰਮਾਈ) {
                capacity=UNEWTRIE2_MEDIUM_ਅੰਕੜੇ_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ ਜੇ(trie->ਅੰਕੜੇ_ਸਮਰੱਥਾ<UNEWTRIE2_MAX_ਅੰਕੜੇ_ਲੰਮਾਈ) {
                capacity=UNEWTRIE2_MAX_ਅੰਕੜੇ_ਲੰਮਾਈ;
            } ਨਹੀਂਤਾਂ {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                ਵਾਪਸ -1;
            }
            ਅੰਕੜੇ = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            ਜੇ(ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ -1;
            }
            uprv_memcpy(ਅੰਕੜੇ, trie->ਅੰਕੜੇ, (size_t)trie->ਅੰਕੜੇ_ਲੰਮਾਈ*4);
            uprv_free(trie->ਅੰਕੜੇ);
            trie->ਅੰਕੜੇ=ਅੰਕੜੇ;
            trie->ਅੰਕੜੇ_ਸਮਰੱਥਾ=capacity;
        }
        trie->ਅੰਕੜੇ_ਲੰਮਾਈ=new_ਸਿਖਰ;
    }
    uprv_memcpy(trie->ਅੰਕੜੇ+new_ਖੰਡ, trie->ਅੰਕੜੇ+copy_ਖੰਡ, UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ*4);
    trie->map[new_ਖੰਡ>>UTRIE2_ਖਿਸਕਾਓ_2]=0;
    ਵਾਪਸ new_ਖੰਡ;
}

/* call when the block's reference counter reaches 0 */
ਸਥਿਰ ਖਾਲੀ
release_ਅੰਕੜੇ_ਖੰਡ(UNewTrie2 *trie, int32_t ਖੰਡ) {
    /* put this block at the front of the free-block chain */
    trie->map[ਖੰਡ>>UTRIE2_ਖਿਸਕਾਓ_2]=-trie->ਪਹਿਲਾ_Free_ਖੰਡ;
    trie->ਪਹਿਲਾ_Free_ਖੰਡ=ਖੰਡ;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
is_Writable_ਖੰਡ(UNewTrie2 *trie, int32_t ਖੰਡ) {
    ਵਾਪਸ ਖੰਡ != trie->ਅੰਕੜੇ_Null_ਵਿੱਥ && 1 == trie->map[ਖੰਡ >> UTRIE2_ਖਿਸਕਾਓ_2];
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
ਸੈੱਟ_Index2_ਇੰਦਰਾਜ(UNewTrie2 *trie, int32_t i2, int32_t ਖੰਡ) {
    int32_t old_ਖੰਡ;
    ++trie->map[ਖੰਡ>>UTRIE2_ਖਿਸਕਾਓ_2];  /* increment first, in case block==oldBlock! */
    old_ਖੰਡ=trie->index2[i2];
    ਜੇ(0 == --trie->map[old_ਖੰਡ>>UTRIE2_ਖਿਸਕਾਓ_2]) {
        release_ਅੰਕੜੇ_ਖੰਡ(trie, old_ਖੰਡ);
    }
    trie->index2[i2]=ਖੰਡ;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ਸਥਿਰ int32_t
get_ਅੰਕੜੇ_ਖੰਡ(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, old_ਖੰਡ, new_ਖੰਡ;

    i2=get_Index2_ਖੰਡ(trie, c, forLSCP);
    ਜੇ(i2<0) {
        ਵਾਪਸ -1;  /* program error */
    }

    i2+=(c>>UTRIE2_ਖਿਸਕਾਓ_2)&UTRIE2_ਸੂਚਕਾਂਕ_2_ਨਕਾਬ;
    old_ਖੰਡ=trie->index2[i2];
    ਜੇ(is_Writable_ਖੰਡ(trie, old_ਖੰਡ)) {
        ਵਾਪਸ old_ਖੰਡ;
    }

    /* allocate a new data block */
    new_ਖੰਡ=alloc_ਅੰਕੜੇ_ਖੰਡ(trie, old_ਖੰਡ);
    ਜੇ(new_ਖੰਡ<0) {
        /* out of memory in the data array */
        ਵਾਪਸ -1;
    }
    ਸੈੱਟ_Index2_ਇੰਦਰਾਜ(trie, i2, new_ਖੰਡ);
    ਵਾਪਸ new_ਖੰਡ;
}

/**
 * @return true if the value was successfully set
 */
ਸਥਿਰ ਖਾਲੀ
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t ਖੰਡ;

    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ || trie->isCompacted) {
        *p_Error_ਕੋਡ=U_NO_WRITE_PERMISSION;
        ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    umutablecptrie_ਸੈੱਟ(trie->t3, c, value, p_Error_ਕੋਡ);
#ਜੇ_ਅੰਤ

    ਖੰਡ=get_ਅੰਕੜੇ_ਖੰਡ(trie, c, forLSCP);
    ਜੇ(ਖੰਡ<0) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    trie->ਅੰਕੜੇ[ਖੰਡ+(c&UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ)]=value;
}

U_CAPI ਖਾਲੀ U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ((uint32_t)c>0x10ffff) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    set32(trie->newTrie, c, ਸੱਚ, value, p_Error_ਕੋਡ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
utrie2_set32_For_Lead_Surrogate_ਕੋਡ_Unit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ(!U_IS_LEAD(c)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    set32(trie->newTrie, c, ਝੂਠ, value, p_Error_ਕੋਡ);
}

ਸਥਿਰ ਖਾਲੀ
write_ਖੰਡ(uint32_t *ਖੰਡ, uint32_t value) {
    uint32_t *ਹੱਦਬੰਦੀ=ਖੰਡ+UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    ਜਦੋਂ(ਖੰਡ<ਹੱਦਬੰਦੀ) {
        *ਖੰਡ++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
ਸਥਿਰ ਖਾਲੀ
fill_ਖੰਡ(uint32_t *ਖੰਡ, UChar32 ਸ਼ੁਰੂ, UChar32 ਹੱਦਬੰਦੀ,
          uint32_t value, uint32_t initial_ਮੁੱਲ, UBool overwrite) {
    uint32_t *p_ਹੱਦਬੰਦੀ;

    p_ਹੱਦਬੰਦੀ=ਖੰਡ+ਹੱਦਬੰਦੀ;
    ਖੰਡ+=ਸ਼ੁਰੂ;
    ਜੇ(overwrite) {
        ਜਦੋਂ(ਖੰਡ<p_ਹੱਦਬੰਦੀ) {
            *ਖੰਡ++=value;
        }
    } ਨਹੀਂਤਾਂ {
        ਜਦੋਂ(ਖੰਡ<p_ਹੱਦਬੰਦੀ) {
            ਜੇ(*ਖੰਡ==initial_ਮੁੱਲ) {
                *ਖੰਡ=value;
            }
            ++ਖੰਡ;
        }
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
utrie2_ਸੈੱਟ_Range32(UTrie2 *trie,
                  UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ,
                  uint32_t value, UBool overwrite,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t ਖੰਡ, rest, repeat_ਖੰਡ;
    UChar32 ਹੱਦਬੰਦੀ;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ((uint32_t)ਸ਼ੁਰੂ>0x10ffff || (uint32_t)ਸਿਰਾ>0x10ffff || ਸ਼ੁਰੂ>ਸਿਰਾ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    newTrie=trie->newTrie;
    ਜੇ(newTrie==ਖਾਲੀ_ਸੂਚਕ || newTrie->isCompacted) {
        *p_Error_ਕੋਡ=U_NO_WRITE_PERMISSION;
        ਵਾਪਸ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    umutablecptrie_ਸੈੱਟ_Range(newTrie->t3, ਸ਼ੁਰੂ, ਸਿਰਾ, value, p_Error_ਕੋਡ);
#ਜੇ_ਅੰਤ
    ਜੇ(!overwrite && value==newTrie->initial_ਮੁੱਲ) {
        ਵਾਪਸ; /* nothing to do */
    }

    ਹੱਦਬੰਦੀ=ਸਿਰਾ+1;
    ਜੇ(ਸ਼ੁਰੂ&UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ) {
        UChar32 ਅਗਲਾ_ਸ਼ੁਰੂ;

        /* set partial block at [start..following block boundary[ */
        ਖੰਡ=get_ਅੰਕੜੇ_ਖੰਡ(newTrie, ਸ਼ੁਰੂ, ਸੱਚ);
        ਜੇ(ਖੰਡ<0) {
            *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }

        ਅਗਲਾ_ਸ਼ੁਰੂ=(ਸ਼ੁਰੂ+UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ)&~UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ;
        ਜੇ(ਅਗਲਾ_ਸ਼ੁਰੂ<=ਹੱਦਬੰਦੀ) {
            fill_ਖੰਡ(newTrie->ਅੰਕੜੇ+ਖੰਡ, ਸ਼ੁਰੂ&UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ, UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ,
                      value, newTrie->initial_ਮੁੱਲ, overwrite);
            ਸ਼ੁਰੂ=ਅਗਲਾ_ਸ਼ੁਰੂ;
        } ਨਹੀਂਤਾਂ {
            fill_ਖੰਡ(newTrie->ਅੰਕੜੇ+ਖੰਡ, ਸ਼ੁਰੂ&UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ, ਹੱਦਬੰਦੀ&UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ,
                      value, newTrie->initial_ਮੁੱਲ, overwrite);
            ਵਾਪਸ;
        }
    }

    /* number of positions in the last, partial block */
    rest=ਹੱਦਬੰਦੀ&UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ;

    /* round down limit to a block boundary */
    ਹੱਦਬੰਦੀ&=~UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ;

    /* iterate over all-value blocks */
    ਜੇ(value==newTrie->initial_ਮੁੱਲ) {
        repeat_ਖੰਡ=newTrie->ਅੰਕੜੇ_Null_ਵਿੱਥ;
    } ਨਹੀਂਤਾਂ {
        repeat_ਖੰਡ=-1;
    }

    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
        int32_t i2;
        UBool ਸੈੱਟ_Repeat_ਖੰਡ=ਝੂਠ;

        ਜੇ(value==newTrie->initial_ਮੁੱਲ && is_ਵਿੱਚ_Null_ਖੰਡ(newTrie, ਸ਼ੁਰੂ, ਸੱਚ)) {
            ਸ਼ੁਰੂ+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ; /* nothing to do */
            ਜਾਰੀ;
        }

        /* get index value */
        i2=get_Index2_ਖੰਡ(newTrie, ਸ਼ੁਰੂ, ਸੱਚ);
        ਜੇ(i2<0) {
            *p_Error_ਕੋਡ=U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
            ਵਾਪਸ;
        }
        i2+=(ਸ਼ੁਰੂ>>UTRIE2_ਖਿਸਕਾਓ_2)&UTRIE2_ਸੂਚਕਾਂਕ_2_ਨਕਾਬ;
        ਖੰਡ=newTrie->index2[i2];
        ਜੇ(is_Writable_ਖੰਡ(newTrie, ਖੰਡ)) {
            /* already allocated */
            ਜੇ(overwrite && ਖੰਡ>=UNEWTRIE2_ਅੰਕੜੇ_0800_ਵਿੱਥ) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                ਸੈੱਟ_Repeat_ਖੰਡ=ਸੱਚ;
            } ਨਹੀਂਤਾਂ {
                /* !overwrite, or protected block: just write the values into this block */
                fill_ਖੰਡ(newTrie->ਅੰਕੜੇ+ਖੰਡ,
                          0, UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ,
                          value, newTrie->initial_ਮੁੱਲ, overwrite);
            }
        } ਨਹੀਂਤਾਂ ਜੇ(newTrie->ਅੰਕੜੇ[ਖੰਡ]!=value && (overwrite || ਖੰਡ==newTrie->ਅੰਕੜੇ_Null_ਵਿੱਥ)) {
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
            ਸੈੱਟ_Repeat_ਖੰਡ=ਸੱਚ;
        }
        ਜੇ(ਸੈੱਟ_Repeat_ਖੰਡ) {
            ਜੇ(repeat_ਖੰਡ>=0) {
                ਸੈੱਟ_Index2_ਇੰਦਰਾਜ(newTrie, i2, repeat_ਖੰਡ);
            } ਨਹੀਂਤਾਂ {
                /* create and set and fill the repeatBlock */
                repeat_ਖੰਡ=get_ਅੰਕੜੇ_ਖੰਡ(newTrie, ਸ਼ੁਰੂ, ਸੱਚ);
                ਜੇ(repeat_ਖੰਡ<0) {
                    *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਵਾਪਸ;
                }
                write_ਖੰਡ(newTrie->ਅੰਕੜੇ+repeat_ਖੰਡ, value);
            }
        }

        ਸ਼ੁਰੂ+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    }

    ਜੇ(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        ਖੰਡ=get_ਅੰਕੜੇ_ਖੰਡ(newTrie, ਸ਼ੁਰੂ, ਸੱਚ);
        ਜੇ(ਖੰਡ<0) {
            *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }

        fill_ਖੰਡ(newTrie->ਅੰਕੜੇ+ਖੰਡ, 0, rest, value, newTrie->initial_ਮੁੱਲ, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
equal_int32(ਅਚਲ int32_t *s, ਅਚਲ int32_t *t, int32_t length) {
    ਜਦੋਂ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ਵਾਪਸ length == 0;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
equal_uint32(ਅਚਲ uint32_t *s, ਅਚਲ uint32_t *t, int32_t length) {
    ਜਦੋਂ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ਵਾਪਸ length == 0;
}

ਸਥਿਰ int32_t
ਲੱਭੋ_Same_Index2_ਖੰਡ(ਅਚਲ int32_t *idx, int32_t index2_ਲੰਮਾਈ, int32_t other_ਖੰਡ) {
    int32_t ਖੰਡ;

    /* ensure that we do not even partially get past index2Length */
    index2_ਲੰਮਾਈ-=UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ;

    ਲਈ(ਖੰਡ=0; ਖੰਡ<=index2_ਲੰਮਾਈ; ++ਖੰਡ) {
        ਜੇ(equal_int32(idx+ਖੰਡ, idx+other_ਖੰਡ, UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ)) {
            ਵਾਪਸ ਖੰਡ;
        }
    }
    ਵਾਪਸ -1;
}

ਸਥਿਰ int32_t
ਲੱਭੋ_Same_ਅੰਕੜੇ_ਖੰਡ_2(ਅਚਲ uint32_t *ਅੰਕੜੇ, int32_t ਅੰਕੜੇ_ਲੰਮਾਈ, int32_t other_ਖੰਡ, int32_t ਖੰਡ_ਲੰਮਾਈ) {
    int32_t ਖੰਡ;

    /* ensure that we do not even partially get past dataLength */
    ਅੰਕੜੇ_ਲੰਮਾਈ-=ਖੰਡ_ਲੰਮਾਈ;

    ਲਈ(ਖੰਡ=0; ਖੰਡ<=ਅੰਕੜੇ_ਲੰਮਾਈ; ਖੰਡ+=UTRIE2_ਅੰਕੜੇ_GRANULARITY) {
        ਜੇ(equal_uint32(ਅੰਕੜੇ+ਖੰਡ, ਅੰਕੜੇ+other_ਖੰਡ, ਖੰਡ_ਲੰਮਾਈ)) {
            ਵਾਪਸ ਖੰਡ;
        }
    }
    ਵਾਪਸ -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
ਸਥਿਰ UChar32
ਲੱਭੋ_ਉੱਚਾ_ਸ਼ੁਰੂ(UNewTrie2 *trie, uint32_t ਉੱਚਾ_ਮੁੱਲ) {
    ਅਚਲ uint32_t *data32;

    uint32_t value, initial_ਮੁੱਲ;
    UChar32 c, prev;
    int32_t i1, i2, j, i2_ਖੰਡ, prev_I2_ਖੰਡ, index2_Null_ਵਿੱਥ, ਖੰਡ, prev_ਖੰਡ, null_ਖੰਡ;

    data32=trie->ਅੰਕੜੇ;
    initial_ਮੁੱਲ=trie->initial_ਮੁੱਲ;

    index2_Null_ਵਿੱਥ=trie->index2_Null_ਵਿੱਥ;
    null_ਖੰਡ=trie->ਅੰਕੜੇ_Null_ਵਿੱਥ;

    /* set variables for previous range */
    ਜੇ(ਉੱਚਾ_ਮੁੱਲ==initial_ਮੁੱਲ) {
        prev_I2_ਖੰਡ=index2_Null_ਵਿੱਥ;
        prev_ਖੰਡ=null_ਖੰਡ;
    } ਨਹੀਂਤਾਂ {
        prev_I2_ਖੰਡ=-1;
        prev_ਖੰਡ=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_ਸੂਚਕਾਂਕ_1_ਲੰਮਾਈ;
    c=prev;
    ਜਦੋਂ(c>0) {
        i2_ਖੰਡ=trie->index1[--i1];
        ਜੇ(i2_ਖੰਡ==prev_I2_ਖੰਡ) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_1_ਇੰਦਰਾਜ;
            ਜਾਰੀ;
        }
        prev_I2_ਖੰਡ=i2_ਖੰਡ;
        ਜੇ(i2_ਖੰਡ==index2_Null_ਵਿੱਥ) {
            /* this is the null index-2 block */
            ਜੇ(ਉੱਚਾ_ਮੁੱਲ!=initial_ਮੁੱਲ) {
                ਵਾਪਸ c;
            }
            c-=UTRIE2_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_1_ਇੰਦਰਾਜ;
        } ਨਹੀਂਤਾਂ {
            /* enumerate data blocks for one index-2 block */
            ਲਈ(i2=UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ; i2>0;) {
                ਖੰਡ=trie->index2[i2_ਖੰਡ+ --i2];
                ਜੇ(ਖੰਡ==prev_ਖੰਡ) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
                    ਜਾਰੀ;
                }
                prev_ਖੰਡ=ਖੰਡ;
                ਜੇ(ਖੰਡ==null_ਖੰਡ) {
                    /* this is the null data block */
                    ਜੇ(ਉੱਚਾ_ਮੁੱਲ!=initial_ਮੁੱਲ) {
                        ਵਾਪਸ c;
                    }
                    c-=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
                } ਨਹੀਂਤਾਂ {
                    ਲਈ(j=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ; j>0;) {
                        value=data32[ਖੰਡ+ --j];
                        ਜੇ(value!=ਉੱਚਾ_ਮੁੱਲ) {
                            ਵਾਪਸ c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    ਵਾਪਸ 0;
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
ਸਥਿਰ ਖਾਲੀ
compact_ਅੰਕੜੇ(UNewTrie2 *trie) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    int32_t ਗਿਣਤੀ_Same=0, ਜੋੜ_Overlaps=0;
#ਜੇ_ਅੰਤ

    int32_t ਸ਼ੁਰੂ, new_ਸ਼ੁਰੂ, moved_ਸ਼ੁਰੂ;
    int32_t ਖੰਡ_ਲੰਮਾਈ, overlap;
    int32_t i, map_ਸੂਚਕਾਂਕ, ਖੰਡ_ਗਿਣਤੀ;

    /* do not compact linear-ASCII data */
    new_ਸ਼ੁਰੂ=UTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ;
    ਲਈ(ਸ਼ੁਰੂ=0, i=0; ਸ਼ੁਰੂ<new_ਸ਼ੁਰੂ; ਸ਼ੁਰੂ+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ, ++i) {
        trie->map[i]=ਸ਼ੁਰੂ;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    ਖੰਡ_ਲੰਮਾਈ=64;
    ਖੰਡ_ਗਿਣਤੀ=ਖੰਡ_ਲੰਮਾਈ>>UTRIE2_ਖਿਸਕਾਓ_2;
    ਲਈ(ਸ਼ੁਰੂ=new_ਸ਼ੁਰੂ; ਸ਼ੁਰੂ<trie->ਅੰਕੜੇ_ਲੰਮਾਈ;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        ਜੇ(ਸ਼ੁਰੂ==UNEWTRIE2_ਅੰਕੜੇ_0800_ਵਿੱਥ) {
            ਖੰਡ_ਲੰਮਾਈ=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            ਖੰਡ_ਗਿਣਤੀ=1;
        }

        /* skip blocks that are not used */
        ਜੇ(trie->map[ਸ਼ੁਰੂ>>UTRIE2_ਖਿਸਕਾਓ_2]<=0) {
            /* advance start to the next block */
            ਸ਼ੁਰੂ+=ਖੰਡ_ਲੰਮਾਈ;

            /* leave newStart with the previous block! */
            ਜਾਰੀ;
        }

        /* search for an identical block */
        ਜੇ( (moved_ਸ਼ੁਰੂ=ਲੱਭੋ_Same_ਅੰਕੜੇ_ਖੰਡ_2(trie->ਅੰਕੜੇ, new_ਸ਼ੁਰੂ, ਸ਼ੁਰੂ, ਖੰਡ_ਲੰਮਾਈ))
             >=0
        ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
            ++ਗਿਣਤੀ_Same;
#ਜੇ_ਅੰਤ
            /* found an identical block, set the other block's index value for the current block */
            ਲਈ(i=ਖੰਡ_ਗਿਣਤੀ, map_ਸੂਚਕਾਂਕ=ਸ਼ੁਰੂ>>UTRIE2_ਖਿਸਕਾਓ_2; i>0; --i) {
                trie->map[map_ਸੂਚਕਾਂਕ++]=moved_ਸ਼ੁਰੂ;
                moved_ਸ਼ੁਰੂ+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            }

            /* advance start to the next block */
            ਸ਼ੁਰੂ+=ਖੰਡ_ਲੰਮਾਈ;

            /* leave newStart with the previous block! */
            ਜਾਰੀ;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        ਲਈ(overlap=ਖੰਡ_ਲੰਮਾਈ-UTRIE2_ਅੰਕੜੇ_GRANULARITY;
            overlap>0 && !equal_uint32(trie->ਅੰਕੜੇ+(new_ਸ਼ੁਰੂ-overlap), trie->ਅੰਕੜੇ+ਸ਼ੁਰੂ, overlap);
            overlap-=UTRIE2_ਅੰਕੜੇ_GRANULARITY) {}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
            ਜੋੜ_Overlaps+=overlap;
#ਜੇ_ਅੰਤ
        ਜੇ(overlap>0 || new_ਸ਼ੁਰੂ<ਸ਼ੁਰੂ) {
            /* some overlap, or just move the whole block */
            moved_ਸ਼ੁਰੂ=new_ਸ਼ੁਰੂ-overlap;
            ਲਈ(i=ਖੰਡ_ਗਿਣਤੀ, map_ਸੂਚਕਾਂਕ=ਸ਼ੁਰੂ>>UTRIE2_ਖਿਸਕਾਓ_2; i>0; --i) {
                trie->map[map_ਸੂਚਕਾਂਕ++]=moved_ਸ਼ੁਰੂ;
                moved_ਸ਼ੁਰੂ+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            }

            /* move the non-overlapping indexes to their new positions */
            ਸ਼ੁਰੂ+=overlap;
            ਲਈ(i=ਖੰਡ_ਲੰਮਾਈ-overlap; i>0; --i) {
                trie->ਅੰਕੜੇ[new_ਸ਼ੁਰੂ++]=trie->ਅੰਕੜੇ[ਸ਼ੁਰੂ++];
            }
        } ਨਹੀਂਤਾਂ /* no overlap && newStart==start */ {
            ਲਈ(i=ਖੰਡ_ਗਿਣਤੀ, map_ਸੂਚਕਾਂਕ=ਸ਼ੁਰੂ>>UTRIE2_ਖਿਸਕਾਓ_2; i>0; --i) {
                trie->map[map_ਸੂਚਕਾਂਕ++]=ਸ਼ੁਰੂ;
                ਸ਼ੁਰੂ+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            }
            new_ਸ਼ੁਰੂ=ਸ਼ੁਰੂ;
        }
    }

    /* now adjust the index-2 table */
    ਲਈ(i=0; i<trie->index2_ਲੰਮਾਈ; ++i) {
        ਜੇ(i==UNEWTRIE2_ਸੂਚਕਾਂਕ_GAP_ਵਿੱਥ) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_ਸੂਚਕਾਂਕ_GAP_ਲੰਮਾਈ;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_ਖਿਸਕਾਓ_2];
    }
    trie->ਅੰਕੜੇ_Null_ਵਿੱਥ=trie->map[trie->ਅੰਕੜੇ_Null_ਵਿੱਥ>>UTRIE2_ਖਿਸਕਾਓ_2];

    /* ensure dataLength alignment */
    ਜਦੋਂ((new_ਸ਼ੁਰੂ&(UTRIE2_ਅੰਕੜੇ_GRANULARITY-1))!=0) {
        trie->ਅੰਕੜੇ[new_ਸ਼ੁਰੂ++]=trie->initial_ਮੁੱਲ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    /* we saved some space */
    ਲਿਖੋ("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (ਲੰਮਾ)trie->ਅੰਕੜੇ_ਲੰਮਾਈ, (ਲੰਮਾ)new_ਸ਼ੁਰੂ, (ਲੰਮਾ)ਗਿਣਤੀ_Same, (ਲੰਮਾ)ਜੋੜ_Overlaps);
#ਜੇ_ਅੰਤ

    trie->ਅੰਕੜੇ_ਲੰਮਾਈ=new_ਸ਼ੁਰੂ;
}

ਸਥਿਰ ਖਾਲੀ
compactIndex2(UNewTrie2 *trie) {
    int32_t i, ਸ਼ੁਰੂ, new_ਸ਼ੁਰੂ, moved_ਸ਼ੁਰੂ, overlap;

    /* do not compact linear-BMP index-2 blocks */
    new_ਸ਼ੁਰੂ=UTRIE2_ਸੂਚਕਾਂਕ_2_BMP_ਲੰਮਾਈ;
    ਲਈ(ਸ਼ੁਰੂ=0, i=0; ਸ਼ੁਰੂ<new_ਸ਼ੁਰੂ; ਸ਼ੁਰੂ+=UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ, ++i) {
        trie->map[i]=ਸ਼ੁਰੂ;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    new_ਸ਼ੁਰੂ+=UTRIE2_UTF8_2_B_ਸੂਚਕਾਂਕ_2_ਲੰਮਾਈ+((trie->ਉੱਚਾ_ਸ਼ੁਰੂ-0x10000)>>UTRIE2_ਖਿਸਕਾਓ_1);

    ਲਈ(ਸ਼ੁਰੂ=UNEWTRIE2_ਸੂਚਕਾਂਕ_2_NULL_ਵਿੱਥ; ਸ਼ੁਰੂ<trie->index2_ਲੰਮਾਈ;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        ਜੇ( (moved_ਸ਼ੁਰੂ=ਲੱਭੋ_Same_Index2_ਖੰਡ(trie->index2, new_ਸ਼ੁਰੂ, ਸ਼ੁਰੂ))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[ਸ਼ੁਰੂ>>UTRIE2_ਖਿਸਕਾਓ_1_2]=moved_ਸ਼ੁਰੂ;

            /* advance start to the next block */
            ਸ਼ੁਰੂ+=UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ;

            /* leave newStart with the previous block! */
            ਜਾਰੀ;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        ਲਈ(overlap=UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ-1;
            overlap>0 && !equal_int32(trie->index2+(new_ਸ਼ੁਰੂ-overlap), trie->index2+ਸ਼ੁਰੂ, overlap);
            --overlap) {}

        ਜੇ(overlap>0 || new_ਸ਼ੁਰੂ<ਸ਼ੁਰੂ) {
            /* some overlap, or just move the whole block */
            trie->map[ਸ਼ੁਰੂ>>UTRIE2_ਖਿਸਕਾਓ_1_2]=new_ਸ਼ੁਰੂ-overlap;

            /* move the non-overlapping indexes to their new positions */
            ਸ਼ੁਰੂ+=overlap;
            ਲਈ(i=UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ-overlap; i>0; --i) {
                trie->index2[new_ਸ਼ੁਰੂ++]=trie->index2[ਸ਼ੁਰੂ++];
            }
        } ਨਹੀਂਤਾਂ /* no overlap && newStart==start */ {
            trie->map[ਸ਼ੁਰੂ>>UTRIE2_ਖਿਸਕਾਓ_1_2]=ਸ਼ੁਰੂ;
            ਸ਼ੁਰੂ+=UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ;
            new_ਸ਼ੁਰੂ=ਸ਼ੁਰੂ;
        }
    }

    /* now adjust the index-1 table */
    ਲਈ(i=0; i<UNEWTRIE2_ਸੂਚਕਾਂਕ_1_ਲੰਮਾਈ; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_ਖਿਸਕਾਓ_1_2];
    }
    trie->index2_Null_ਵਿੱਥ=trie->map[trie->index2_Null_ਵਿੱਥ>>UTRIE2_ਖਿਸਕਾਓ_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    ਜਦੋਂ((new_ਸ਼ੁਰੂ&((UTRIE2_ਅੰਕੜੇ_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[new_ਸ਼ੁਰੂ++] = static_cast<int32_t>(0xffff) << UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    /* we saved some space */
    ਲਿਖੋ("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (ਲੰਮਾ)trie->index2_ਲੰਮਾਈ, (ਲੰਮਾ)new_ਸ਼ੁਰੂ);
#ਜੇ_ਅੰਤ

    trie->index2_ਲੰਮਾਈ=new_ਸ਼ੁਰੂ;
}

ਸਥਿਰ ਖਾਲੀ
compactTrie(UTrie2 *trie, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UNewTrie2 *newTrie;
    UChar32 ਉੱਚਾ_ਸ਼ੁਰੂ, supp_ਉੱਚਾ_ਸ਼ੁਰੂ;
    uint32_t ਉੱਚਾ_ਮੁੱਲ;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    ਉੱਚਾ_ਮੁੱਲ=utrie2_get32(trie, 0x10ffff);
    ਉੱਚਾ_ਸ਼ੁਰੂ=ਲੱਭੋ_ਉੱਚਾ_ਸ਼ੁਰੂ(newTrie, ਉੱਚਾ_ਮੁੱਲ);
    ਉੱਚਾ_ਸ਼ੁਰੂ=(ਉੱਚਾ_ਸ਼ੁਰੂ+(UTRIE2_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_1_ਇੰਦਰਾਜ-1))&~(UTRIE2_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_1_ਇੰਦਰਾਜ-1);
    ਜੇ(ਉੱਚਾ_ਸ਼ੁਰੂ==0x110000) {
        ਉੱਚਾ_ਮੁੱਲ=trie->error_ਮੁੱਲ;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->ਉੱਚਾ_ਸ਼ੁਰੂ=newTrie->ਉੱਚਾ_ਸ਼ੁਰੂ=ਉੱਚਾ_ਸ਼ੁਰੂ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    ਲਿਖੋ("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (ਲੰਮਾ)ਉੱਚਾ_ਸ਼ੁਰੂ, (ਲੰਮਾ)ਉੱਚਾ_ਮੁੱਲ, (ਲੰਮਾ)trie->initial_ਮੁੱਲ);
#ਜੇ_ਅੰਤ

    ਜੇ(ਉੱਚਾ_ਸ਼ੁਰੂ<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_ਉੱਚਾ_ਸ਼ੁਰੂ= ਉੱਚਾ_ਸ਼ੁਰੂ<=0x10000 ? 0x10000 : ਉੱਚਾ_ਸ਼ੁਰੂ;
        utrie2_ਸੈੱਟ_Range32(trie, supp_ਉੱਚਾ_ਸ਼ੁਰੂ, 0x10ffff, trie->initial_ਮੁੱਲ, ਸੱਚ, p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
    }

    compact_ਅੰਕੜੇ(newTrie);
    ਜੇ(ਉੱਚਾ_ਸ਼ੁਰੂ>0x10000) {
        compactIndex2(newTrie);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (ਲੰਮਾ)ਉੱਚਾ_ਸ਼ੁਰੂ, (ਲੰਮਾ)trie->newTrie->index2_ਲੰਮਾਈ, (ਲੰਮਾ)UTRIE2_ਸੂਚਕਾਂਕ_1_ਵਿੱਥ);
#ਜੇ_ਅੰਤ
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->ਅੰਕੜੇ[newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ++]=ਉੱਚਾ_ਮੁੱਲ;
    ਜਦੋਂ((newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ&(UTRIE2_ਅੰਕੜੇ_GRANULARITY-1))!=0) {
        newTrie->ਅੰਕੜੇ[newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ++]=trie->initial_ਮੁੱਲ;
    }

    newTrie->isCompacted=ਸੱਚ;
}

/* serialization ------------------------------------------------------------ */

/**
 * Maximum length of the runtime index array.
 * Limited by its own 16-bit index values, and by uint16_t UTrie2Header.indexLength.
 * (The actual maximum length is lower,
 * (0x110000>>UTRIE2_SHIFT_2)+UTRIE2_UTF8_2B_INDEX_2_LENGTH+UTRIE2_MAX_INDEX_1_LENGTH.)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTRIE2_MAX_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTRIE2_MAX_ਅੰਕੜੇ_ਲੰਮਾਈ (0xffff<<UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ)

/* Compact and internally serialize the trie. */
U_CAPI ਖਾਲੀ U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_ਮੁੱਲ_ਬਿੱਟ ਮੁੱਲ_ਬਿੱਟ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UNewTrie2 *newTrie;
    UTrie2_ਸਿਰਲੇਖ *ਸਿਰਲੇਖ;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_ਲੰਮਾਈ;
    int32_t ਅੰਕੜੇ_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 ਉੱਚਾ_ਸ਼ੁਰੂ;

    /* argument check */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ( trie==ਖਾਲੀ_ਸੂਚਕ ||
        ਮੁੱਲ_ਬਿੱਟ<0 || UTRIE2_ਗਿਣਤੀ_ਮੁੱਲ_ਬਿੱਟ<=ਮੁੱਲ_ਬਿੱਟ
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    newTrie=trie->newTrie;
    ਜੇ(newTrie==ਖਾਲੀ_ਸੂਚਕ) {
        /* already frozen */
        UTrie2_ਮੁੱਲ_ਬਿੱਟ frozen_ਮੁੱਲ_ਬਿੱਟ=
            trie->data16!=ਖਾਲੀ_ਸੂਚਕ ? UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ : UTRIE2_32_ਮੁੱਲ_ਬਿੱਟ;
        ਜੇ(ਮੁੱਲ_ਬਿੱਟ!=frozen_ਮੁੱਲ_ਬਿੱਟ) {
            *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        }
        ਵਾਪਸ;
    }

    /* compact if necessary */
    ਜੇ(!newTrie->isCompacted) {
        compactTrie(trie, p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
    }
    ਉੱਚਾ_ਸ਼ੁਰੂ=trie->ਉੱਚਾ_ਸ਼ੁਰੂ;

    ਜੇ(ਉੱਚਾ_ਸ਼ੁਰੂ<=0x10000) {
        all_Indexes_ਲੰਮਾਈ=UTRIE2_ਸੂਚਕਾਂਕ_1_ਵਿੱਥ;
    } ਨਹੀਂਤਾਂ {
        all_Indexes_ਲੰਮਾਈ=newTrie->index2_ਲੰਮਾਈ;
    }
    ਜੇ(ਮੁੱਲ_ਬਿੱਟ==UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ) {
        ਅੰਕੜੇ_Move=all_Indexes_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        ਅੰਕੜੇ_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    ਜੇ( /* for unshifted indexLength */
        all_Indexes_ਲੰਮਾਈ>UTRIE2_MAX_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ ||
        /* for unshifted dataNullOffset */
        (ਅੰਕੜੇ_Move+newTrie->ਅੰਕੜੇ_Null_ਵਿੱਥ)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (ਅੰਕੜੇ_Move+UNEWTRIE2_ਅੰਕੜੇ_0800_ਵਿੱਥ)>0xffff ||
        /* for shiftedDataLength */
        (ਅੰਕੜੇ_Move+newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ)>UTRIE2_MAX_ਅੰਕੜੇ_ਲੰਮਾਈ
    ) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ;
    }

    /* calculate the total serialized length */
    length=ਆਕਾਰ(UTrie2_ਸਿਰਲੇਖ)+all_Indexes_ਲੰਮਾਈ*2;
    ਜੇ(ਮੁੱਲ_ਬਿੱਟ==UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ) {
        length+=newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ*2;
    } ਨਹੀਂਤਾਂ {
        length+=newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ*4;
    }

    trie->ਸਮ੍ਰਿਤੀ=uprv_malloc(length);
    ਜੇ(trie->ਸਮ੍ਰਿਤੀ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    trie->length=length;
    trie->is_ਸਮ੍ਰਿਤੀ_Owned=ਸੱਚ;

    trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=all_Indexes_ਲੰਮਾਈ;
    trie->ਅੰਕੜੇ_ਲੰਮਾਈ=newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਜੇ(ਉੱਚਾ_ਸ਼ੁਰੂ<=0x10000) {
        trie->index2_Null_ਵਿੱਥ=0xffff;
    } ਨਹੀਂਤਾਂ {
        trie->index2_Null_ਵਿੱਥ=static_cast<uint16_t>(UTRIE2_ਸੂਚਕਾਂਕ_2_ਵਿੱਥ+newTrie->index2_Null_ਵਿੱਥ);
    }
    trie->ਅੰਕੜੇ_Null_ਵਿੱਥ=(uint16_t)(ਅੰਕੜੇ_Move+newTrie->ਅੰਕੜੇ_Null_ਵਿੱਥ);
    trie->ਉੱਚਾ_ਮੁੱਲ_ਸੂਚਕਾਂਕ=ਅੰਕੜੇ_Move+trie->ਅੰਕੜੇ_ਲੰਮਾਈ-UTRIE2_ਅੰਕੜੇ_GRANULARITY;

    /* set the header fields */
    ਸਿਰਲੇਖ=(UTrie2_ਸਿਰਲੇਖ *)trie->ਸਮ੍ਰਿਤੀ;

    ਸਿਰਲੇਖ->signature=UTRIE2_SIG; /* "Tri2" */
    ਸਿਰਲੇਖ->options=(uint16_t)ਮੁੱਲ_ਬਿੱਟ;

    ਸਿਰਲੇਖ->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=(uint16_t)trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਸਿਰਲੇਖ->shifted_ਅੰਕੜੇ_ਲੰਮਾਈ=(uint16_t)(trie->ਅੰਕੜੇ_ਲੰਮਾਈ>>UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
    ਸਿਰਲੇਖ->index2_Null_ਵਿੱਥ=trie->index2_Null_ਵਿੱਥ;
    ਸਿਰਲੇਖ->ਅੰਕੜੇ_Null_ਵਿੱਥ=trie->ਅੰਕੜੇ_Null_ਵਿੱਥ;
    ਸਿਰਲੇਖ->shifted_ਉੱਚਾ_ਸ਼ੁਰੂ=(uint16_t)(ਉੱਚਾ_ਸ਼ੁਰੂ>>UTRIE2_ਖਿਸਕਾਓ_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(ਸਿਰਲੇਖ+1);
    trie->ਸੂਚਕਾਂਕ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    ਲਈ(i=UTRIE2_ਸੂਚਕਾਂਕ_2_BMP_ਲੰਮਾਈ; i>0; --i) {
        *dest16++=(uint16_t)((ਅੰਕੜੇ_Move + *p++)>>UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    ਲਈ(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(ਅੰਕੜੇ_Move+UTRIE2_BAD_UTF8_ਅੰਕੜੇ_ਵਿੱਥ);
    }
    ਲਈ(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(ਅੰਕੜੇ_Move+newTrie->index2[i<<(6-UTRIE2_ਖਿਸਕਾਓ_2)]);
    }

    ਜੇ(ਉੱਚਾ_ਸ਼ੁਰੂ>0x10000) {
        int32_t index1_ਲੰਮਾਈ=(ਉੱਚਾ_ਸ਼ੁਰੂ-0x10000)>>UTRIE2_ਖਿਸਕਾਓ_1;
        int32_t index2_ਵਿੱਥ=UTRIE2_ਸੂਚਕਾਂਕ_2_BMP_ਲੰਮਾਈ+UTRIE2_UTF8_2_B_ਸੂਚਕਾਂਕ_2_ਲੰਮਾਈ+index1_ਲੰਮਾਈ;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_ਸੂਚਕਾਂਕ_1_ਲੰਮਾਈ;
        ਲਈ(i=index1_ਲੰਮਾਈ; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_ਸੂਚਕਾਂਕ_2_ਵਿੱਥ + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2_ਵਿੱਥ;
        ਲਈ(i=newTrie->index2_ਲੰਮਾਈ-index2_ਵਿੱਥ; i>0; --i) {
            *dest16++=(uint16_t)((ਅੰਕੜੇ_Move + *p++)>>UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
        }
    }

    /* write the 16/32-bit data array */
    ਚੋਣ(ਮੁੱਲ_ਬਿੱਟ) {
    ਹਾਲਤ UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=ਖਾਲੀ_ਸੂਚਕ;
        p=newTrie->ਅੰਕੜੇ;
        ਲਈ(i=newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        ਰੋਕੋ;
    ਹਾਲਤ UTRIE2_32_ਮੁੱਲ_ਬਿੱਟ:
        /* write 32-bit data values */
        trie->data16=ਖਾਲੀ_ਸੂਚਕ;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->ਅੰਕੜੇ, (size_t)newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ*4);
        ਰੋਕੋ;
    ਮੂਲ:
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
    umutablecptrie_ਸੈੱਟ_Name(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_ਮੁੱਲ_ਚੌੜਾਈ)ਮੁੱਲ_ਬਿੱਟ, p_Error_ਕੋਡ));
#ਜੇ_ਅੰਤ
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->ਅੰਕੜੇ);
    uprv_free(newTrie);
    trie->newTrie=ਖਾਲੀ_ਸੂਚਕ;
}

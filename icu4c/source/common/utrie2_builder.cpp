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
#جے_تعریف_شدہ UTRIE2_DEBUG
#   include <stdio.h>
#جے_انت
// #define UCPTRIE_DEBUG

#ضمن "unicode/utypes.h"
#جے_تعریف_شدہ UCPTRIE_DEBUG
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "ucptrie_impl.h"
#جے_انت
#ضمن "cmemory.h"
#ضمن "utrie2.h"
#ضمن "utrie2_impl.h"

#ضمن "utrie.h"  // for utrie2_fromUTrie()

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

تعداد {
    /** The null index-2 block, following the gap in the index-2 table. */
    UNEWTRIE2_اشاریہ_2_NULL_وتھ=UNEWTRIE2_اشاریہ_GAP_وتھ+UNEWTRIE2_اشاریہ_GAP_لمائی,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_اشاریہ_2_شروع_وتھ=UNEWTRIE2_اشاریہ_2_NULL_وتھ+UTRIE2_اشاریہ_2_بلاک_لمائی,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_انکڑے_NULL_وتھ=UTRIE2_انکڑے_شروع_وتھ,

    /** The start of allocated data blocks. */
    UNEWTRIE2_انکڑے_شروع_وتھ=UNEWTRIE2_انکڑے_NULL_وتھ+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_انکڑے_0800_وتھ=UNEWTRIE2_انکڑے_شروع_وتھ+0x780
};

/* Start with allocation of 16k data entries. */
#تعریف_کرو UNEWTRIE2_INITIAL_انکڑے_لمائی ((int32_t)1<<14)

/* Grow about 8x each time. */
#تعریف_کرو UNEWTRIE2_MEDIUM_انکڑے_لمائی ((int32_t)1<<17)

ساكن int32_t
alloc_Index2_بلاک(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_قدر, uint32_t error_قدر, UError_کوڈ *p_Error_کوڈ) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *انکڑے;
    int32_t i, j;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(حجم(UNewTrie2));
    انکڑے=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_انکڑے_لمائی*4);
    إذا(trie==خالی_سوچک || newTrie==خالی_سوچک || انکڑے==خالی_سوچک) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(انکڑے);
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }

    uprv_memset(trie, 0, حجم(UTrie2));
    trie->initial_قدر=initial_قدر;
    trie->error_قدر=error_قدر;
    trie->بلند_شروع=0x110000;
    trie->newTrie=newTrie;
#جے_تعریف_شدہ UTRIE2_DEBUG
    trie->name="open";
#جے_انت

    newTrie->انکڑے=انکڑے;
#جے_تعریف_شدہ UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_قدر, error_قدر, p_Error_کوڈ);
#جے_انت
    newTrie->انکڑے_سمرتھا=UNEWTRIE2_INITIAL_انکڑے_لمائی;
    newTrie->initial_قدر=initial_قدر;
    newTrie->error_قدر=error_قدر;
    newTrie->بلند_شروع=0x110000;
    newTrie->پہلا_Free_بلاک=0;  /* no free block in the list */
    newTrie->isCompacted=خطأ;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    لكل(i=0; i<0x80; ++i) {
        newTrie->انکڑے[i]=initial_قدر;
    }
    لكل(; i<0xc0; ++i) {
        newTrie->انکڑے[i]=error_قدر;
    }
    لكل(i=UNEWTRIE2_انکڑے_NULL_وتھ; i<UNEWTRIE2_انکڑے_شروع_وتھ; ++i) {
        newTrie->انکڑے[i]=initial_قدر;
    }
    newTrie->انکڑے_Null_وتھ=UNEWTRIE2_انکڑے_NULL_وتھ;
    newTrie->انکڑے_لمائی=UNEWTRIE2_انکڑے_شروع_وتھ;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    لكل(i=0, j=0; j<0x80; ++i, j+=UTRIE2_انکڑے_بلاک_لمائی) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    لكل(; j<0xc0; ++i, j+=UTRIE2_انکڑے_بلاک_لمائی) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_کھسکاؤ_2)-
        (0x80>>UTRIE2_کھسکاؤ_2)+
        1+
        UTRIE2_LSCP_اشاریہ_2_لمائی;
    j+=UTRIE2_انکڑے_بلاک_لمائی;
    لكل(; j<UNEWTRIE2_انکڑے_شروع_وتھ; ++i, j+=UTRIE2_انکڑے_بلاک_لمائی) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    لكل(i=0x80>>UTRIE2_کھسکاؤ_2; i<UTRIE2_اشاریہ_2_BMP_لمائی; ++i) {
        newTrie->index2[i]=UNEWTRIE2_انکڑے_NULL_وتھ;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    لكل(i=0; i<UNEWTRIE2_اشاریہ_GAP_لمائی; ++i) {
        newTrie->index2[UNEWTRIE2_اشاریہ_GAP_وتھ+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    لكل(i=0; i<UTRIE2_اشاریہ_2_بلاک_لمائی; ++i) {
        newTrie->index2[UNEWTRIE2_اشاریہ_2_NULL_وتھ+i]=UNEWTRIE2_انکڑے_NULL_وتھ;
    }
    newTrie->index2_Null_وتھ=UNEWTRIE2_اشاریہ_2_NULL_وتھ;
    newTrie->index2_لمائی=UNEWTRIE2_اشاریہ_2_شروع_وتھ;

    /* set the index-1 indexes for the linear index-2 block */
    لكل(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_اشاریہ_1_لمائی;
        ++i, j+=UTRIE2_اشاریہ_2_بلاک_لمائی
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    لكل(; i<UNEWTRIE2_اشاریہ_1_لمائی; ++i) {
        newTrie->index1[i]=UNEWTRIE2_اشاریہ_2_NULL_وتھ;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    لكل(i=0x80; i<0x800; i+=UTRIE2_انکڑے_بلاک_لمائی) {
        utrie2_set32(trie, i, initial_قدر, p_Error_کوڈ);
    }

    أرجع trie;
}

ساكن UNewTrie2 *
cloneBuilder(ثابت UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(حجم(UNewTrie2)));
    إذا(trie==خالی_سوچک) {
        أرجع خالی_سوچک;
    }

    trie->انکڑے = static_cast<uint32_t*>(uprv_malloc(other->انکڑے_سمرتھا * 4));
    إذا(trie->انکڑے==خالی_سوچک) {
        uprv_free(trie);
        أرجع خالی_سوچک;
    }
#جے_تعریف_شدہ UCPTRIE_DEBUG
    إذا(other->t3==خالی_سوچک) {
        trie->t3=خالی_سوچک;
    } وإلا {
        UError_کوڈ error_کوڈ=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &error_کوڈ);
    }
#جے_انت
    trie->انکڑے_سمرتھا=other->انکڑے_سمرتھا;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, حجم(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_لمائی*4);
    trie->index2_Null_وتھ=other->index2_Null_وتھ;
    trie->index2_لمائی=other->index2_لمائی;

    uprv_memcpy(trie->انکڑے, other->انکڑے, (size_t)other->انکڑے_لمائی*4);
    trie->انکڑے_Null_وتھ=other->انکڑے_Null_وتھ;
    trie->انکڑے_لمائی=other->انکڑے_لمائی;

    /* reference counters */
    إذا(other->isCompacted) {
        trie->پہلا_Free_بلاک=0;
    } وإلا {
        uprv_memcpy(trie->map, other->map, ((size_t)other->انکڑے_لمائی>>UTRIE2_کھسکاؤ_2)*4);
        trie->پہلا_Free_بلاک=other->پہلا_Free_بلاک;
    }

    trie->initial_قدر=other->initial_قدر;
    trie->error_قدر=other->error_قدر;
    trie->بلند_شروع=other->بلند_شروع;
    trie->isCompacted=other->isCompacted;

    أرجع trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ثابت UTrie2 *other, UError_کوڈ *p_Error_کوڈ) {
    UTrie2 *trie;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا(other==خالی_سوچک || (other->حافظہ==خالی_سوچک && other->newTrie==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memcpy(trie, other, حجم(UTrie2));

    إذا(other->حافظہ!=خالی_سوچک) {
        trie->حافظہ=uprv_malloc(other->length);
        إذا(trie->حافظہ!=خالی_سوچک) {
            trie->is_حافظہ_Owned=سچ;
            uprv_memcpy(trie->حافظہ, other->حافظہ, other->length);

            /* make the clone's pointers point to its own memory */
            trie->اشاریہ=(uint16_t *)trie->حافظہ+(other->اشاریہ-(uint16_t *)other->حافظہ);
            إذا(other->data16!=خالی_سوچک) {
                trie->data16=(uint16_t *)trie->حافظہ+(other->data16-(uint16_t *)other->حافظہ);
            }
            إذا(other->data32!=خالی_سوچک) {
                trie->data32=(uint32_t *)trie->حافظہ+(other->data32-(uint32_t *)other->حافظہ);
            }
        }
    } وإلا /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    إذا(trie->حافظہ==خالی_سوچک && trie->newTrie==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        uprv_free(trie);
        trie=خالی_سوچک;
    }
    أرجع trie;
}

تعريفنوع بنية New_Trie_And_حالت {
    UTrie2 *trie;
    UError_کوڈ error_کوڈ;
    UBool exclusive_حد_بندی;  /* rather than inclusive range end */
} New_Trie_And_حالت;

ساكن UBool U_CALLCONV
copyEnumRange(ثابت فراغ *context, UChar32 شروع, UChar32 سرا, uint32_t value) {
    New_Trie_And_حالت *nt=(New_Trie_And_حالت *)context;
    إذا(value!=nt->trie->initial_قدر) {
        إذا(nt->exclusive_حد_بندی) {
            --سرا;
        }
        إذا(شروع==سرا) {
            utrie2_set32(nt->trie, شروع, value, &nt->error_کوڈ);
        } وإلا {
            utrie2_سیٹ_Range32(nt->trie, شروع, سرا, value, سچ, &nt->error_کوڈ);
        }
        أرجع U_SUCCESS(nt->error_کوڈ);
    } وإلا {
        أرجع سچ;
    }
}

#جے_تعریف_شدہ UTRIE2_DEBUG
ساكن لما گنتی_Initial(ثابت UTrie2 *trie) {
    uint32_t initial_قدر=trie->initial_قدر;
    int32_t length=trie->انکڑے_لمائی;
    لما count=0;
    إذا(trie->data16!=خالی_سوچک) {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data16[i]==initial_قدر) { ++count; }
        }
    } وإلا {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data32[i]==initial_قدر) { ++count; }
        }
    }
    أرجع count;
}

ساكن فراغ
utrie_printLengths(ثابت UTrie *trie) {
    لما اشاریہ_لمائی=trie->اشاریہ_لمائی;
    لما انکڑے_لمائی=(لما)trie->انکڑے_لمائی;
    لما کل_لمائی=(لما)حجم(UTrie_سرخی)+اشاریہ_لمائی*2+انکڑے_لمائی*(trie->data32!=خالی_سوچک ? 4 : 2);
    لکھو("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           اشاریہ_لمائی, انکڑے_لمائی, کل_لمائی);
}

ساكن فراغ
utrie2_printLengths(ثابت UTrie2 *trie, ثابت حرف *which) {
    لما اشاریہ_لمائی=trie->اشاریہ_لمائی;
    لما انکڑے_لمائی=(لما)trie->انکڑے_لمائی;
    لما کل_لمائی=(لما)حجم(UTrie2_سرخی)+اشاریہ_لمائی*2+انکڑے_لمائی*(trie->data32!=خالی_سوچک ? 4 : 2);
    لکھو("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, اشاریہ_لمائی, انکڑے_لمائی, گنتی_Initial(trie), کل_لمائی);
}
#جے_انت

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ثابت UTrie2 *other, UError_کوڈ *p_Error_کوڈ) {
    New_Trie_And_حالت context;
    char16_t lead;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا(other==خالی_سوچک || (other->حافظہ==خالی_سوچک && other->newTrie==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    إذا(other->newTrie!=خالی_سوچک && !other->newTrie->isCompacted) {
        أرجع utrie2_clone(other, p_Error_کوڈ);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_قدر, other->error_قدر, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    context.exclusive_حد_بندی=خطأ;
    context.error_کوڈ=*p_Error_کوڈ;
    utrie2_enum(other, خالی_سوچک, copyEnumRange, &context);
    *p_Error_کوڈ=context.error_کوڈ;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(other->data32==خالی_سوچک) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } وإلا {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        إذا(value!=other->initial_قدر) {
            utrie2_set32_For_Lead_Surrogate_کوڈ_Unit(context.trie, lead, value, p_Error_کوڈ);
        }
    }
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        utrie2_close(context.trie);
        context.trie=خالی_سوچک;
    }
    أرجع context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ثابت UTrie *trie1, uint32_t error_قدر, UError_کوڈ *p_Error_کوڈ) {
    New_Trie_And_حالت context;
    char16_t lead;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا(trie1==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    context.trie=utrie2_open(trie1->initial_قدر, error_قدر, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    context.exclusive_حد_بندی=سچ;
    context.error_کوڈ=*p_Error_کوڈ;
    utrie_enum(trie1, خالی_سوچک, copyEnumRange, &context);
    *p_Error_کوڈ=context.error_کوڈ;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(trie1->data32==خالی_سوچک) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } وإلا {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        إذا(value!=trie1->initial_قدر) {
            utrie2_set32_For_Lead_Surrogate_کوڈ_Unit(context.trie, lead, value, p_Error_کوڈ);
        }
    }
    إذا(U_SUCCESS(*p_Error_کوڈ)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=خالی_سوچک ? UTRIE2_32_قدر_بٹ : UTRIE2_16_قدر_بٹ,
                      p_Error_کوڈ);
    }
#جے_تعریف_شدہ UTRIE2_DEBUG
    إذا(U_SUCCESS(*p_Error_کوڈ)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#جے_انت
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        utrie2_close(context.trie);
        context.trie=خالی_سوچک;
    }
    أرجع context.trie;
}

ساكن مضمن UBool
is_وچ_Null_بلاک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, بلاک;

    إذا(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_اشاریہ_2_وتھ-(0xd800>>UTRIE2_کھسکاؤ_2))+
            (c>>UTRIE2_کھسکاؤ_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_کھسکاؤ_1]+
            ((c>>UTRIE2_کھسکاؤ_2)&UTRIE2_اشاریہ_2_نقاب);
    }
    بلاک=trie->index2[i2];
    أرجع بلاک == trie->انکڑے_Null_وتھ;
}

ساكن int32_t
alloc_Index2_بلاک(UNewTrie2 *trie) {
    int32_t new_بلاک, new_چوٹی;

    new_بلاک=trie->index2_لمائی;
    new_چوٹی=new_بلاک+UTRIE2_اشاریہ_2_بلاک_لمائی;
    إذا(new_چوٹی>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        أرجع -1;
    }
    trie->index2_لمائی=new_چوٹی;
    uprv_memcpy(trie->index2+new_بلاک, trie->index2+trie->index2_Null_وتھ, UTRIE2_اشاریہ_2_بلاک_لمائی*4);
    أرجع new_بلاک;
}

ساكن int32_t
get_Index2_بلاک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    إذا(U_IS_LEAD(c) && forLSCP) {
        أرجع UTRIE2_LSCP_اشاریہ_2_وتھ;
    }

    i1=c>>UTRIE2_کھسکاؤ_1;
    i2=trie->index1[i1];
    إذا(i2==trie->index2_Null_وتھ) {
        i2=alloc_Index2_بلاک(trie);
        إذا(i2<0) {
            أرجع -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    أرجع i2;
}

ساكن int32_t
alloc_انکڑے_بلاک(UNewTrie2 *trie, int32_t copy_بلاک) {
    int32_t new_بلاک, new_چوٹی;

    إذا(trie->پہلا_Free_بلاک!=0) {
        /* get the first free block */
        new_بلاک=trie->پہلا_Free_بلاک;
        trie->پہلا_Free_بلاک=-trie->map[new_بلاک>>UTRIE2_کھسکاؤ_2];
    } وإلا {
        /* get a new block from the high end */
        new_بلاک=trie->انکڑے_لمائی;
        new_چوٹی=new_بلاک+UTRIE2_انکڑے_بلاک_لمائی;
        إذا(new_چوٹی>trie->انکڑے_سمرتھا) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *انکڑے;

            إذا(trie->انکڑے_سمرتھا<UNEWTRIE2_MEDIUM_انکڑے_لمائی) {
                capacity=UNEWTRIE2_MEDIUM_انکڑے_لمائی;
            } وإلا إذا(trie->انکڑے_سمرتھا<UNEWTRIE2_MAX_انکڑے_لمائی) {
                capacity=UNEWTRIE2_MAX_انکڑے_لمائی;
            } وإلا {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                أرجع -1;
            }
            انکڑے = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            إذا(انکڑے==خالی_سوچک) {
                أرجع -1;
            }
            uprv_memcpy(انکڑے, trie->انکڑے, (size_t)trie->انکڑے_لمائی*4);
            uprv_free(trie->انکڑے);
            trie->انکڑے=انکڑے;
            trie->انکڑے_سمرتھا=capacity;
        }
        trie->انکڑے_لمائی=new_چوٹی;
    }
    uprv_memcpy(trie->انکڑے+new_بلاک, trie->انکڑے+copy_بلاک, UTRIE2_انکڑے_بلاک_لمائی*4);
    trie->map[new_بلاک>>UTRIE2_کھسکاؤ_2]=0;
    أرجع new_بلاک;
}

/* call when the block's reference counter reaches 0 */
ساكن فراغ
release_انکڑے_بلاک(UNewTrie2 *trie, int32_t بلاک) {
    /* put this block at the front of the free-block chain */
    trie->map[بلاک>>UTRIE2_کھسکاؤ_2]=-trie->پہلا_Free_بلاک;
    trie->پہلا_Free_بلاک=بلاک;
}

ساكن مضمن UBool
is_Writable_بلاک(UNewTrie2 *trie, int32_t بلاک) {
    أرجع بلاک != trie->انکڑے_Null_وتھ && 1 == trie->map[بلاک >> UTRIE2_کھسکاؤ_2];
}

ساكن مضمن فراغ
سیٹ_Index2_اندراج(UNewTrie2 *trie, int32_t i2, int32_t بلاک) {
    int32_t old_بلاک;
    ++trie->map[بلاک>>UTRIE2_کھسکاؤ_2];  /* increment first, in case block==oldBlock! */
    old_بلاک=trie->index2[i2];
    إذا(0 == --trie->map[old_بلاک>>UTRIE2_کھسکاؤ_2]) {
        release_انکڑے_بلاک(trie, old_بلاک);
    }
    trie->index2[i2]=بلاک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ساكن int32_t
get_انکڑے_بلاک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, old_بلاک, new_بلاک;

    i2=get_Index2_بلاک(trie, c, forLSCP);
    إذا(i2<0) {
        أرجع -1;  /* program error */
    }

    i2+=(c>>UTRIE2_کھسکاؤ_2)&UTRIE2_اشاریہ_2_نقاب;
    old_بلاک=trie->index2[i2];
    إذا(is_Writable_بلاک(trie, old_بلاک)) {
        أرجع old_بلاک;
    }

    /* allocate a new data block */
    new_بلاک=alloc_انکڑے_بلاک(trie, old_بلاک);
    إذا(new_بلاک<0) {
        /* out of memory in the data array */
        أرجع -1;
    }
    سیٹ_Index2_اندراج(trie, i2, new_بلاک);
    أرجع new_بلاک;
}

/**
 * @return true if the value was successfully set
 */
ساكن فراغ
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UError_کوڈ *p_Error_کوڈ) {
    int32_t بلاک;

    إذا(trie==خالی_سوچک || trie->isCompacted) {
        *p_Error_کوڈ=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#جے_تعریف_شدہ UCPTRIE_DEBUG
    umutablecptrie_سیٹ(trie->t3, c, value, p_Error_کوڈ);
#جے_انت

    بلاک=get_انکڑے_بلاک(trie, c, forLSCP);
    إذا(بلاک<0) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع;
    }

    trie->انکڑے[بلاک+(c&UTRIE2_انکڑے_نقاب)]=value;
}

U_CAPI فراغ U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    إذا((uint32_t)c>0x10ffff) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, سچ, value, p_Error_کوڈ);
}

U_CAPI فراغ U_EXPORT2
utrie2_set32_For_Lead_Surrogate_کوڈ_Unit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    إذا(!U_IS_LEAD(c)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, خطأ, value, p_Error_کوڈ);
}

ساكن فراغ
write_بلاک(uint32_t *بلاک, uint32_t value) {
    uint32_t *حد_بندی=بلاک+UTRIE2_انکڑے_بلاک_لمائی;
    طالما(بلاک<حد_بندی) {
        *بلاک++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
ساكن فراغ
fill_بلاک(uint32_t *بلاک, UChar32 شروع, UChar32 حد_بندی,
          uint32_t value, uint32_t initial_قدر, UBool overwrite) {
    uint32_t *p_حد_بندی;

    p_حد_بندی=بلاک+حد_بندی;
    بلاک+=شروع;
    إذا(overwrite) {
        طالما(بلاک<p_حد_بندی) {
            *بلاک++=value;
        }
    } وإلا {
        طالما(بلاک<p_حد_بندی) {
            إذا(*بلاک==initial_قدر) {
                *بلاک=value;
            }
            ++بلاک;
        }
    }
}

U_CAPI فراغ U_EXPORT2
utrie2_سیٹ_Range32(UTrie2 *trie,
                  UChar32 شروع, UChar32 سرا,
                  uint32_t value, UBool overwrite,
                  UError_کوڈ *p_Error_کوڈ) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t بلاک, rest, repeat_بلاک;
    UChar32 حد_بندی;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    إذا((uint32_t)شروع>0x10ffff || (uint32_t)سرا>0x10ffff || شروع>سرا) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==خالی_سوچک || newTrie->isCompacted) {
        *p_Error_کوڈ=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#جے_تعریف_شدہ UCPTRIE_DEBUG
    umutablecptrie_سیٹ_Range(newTrie->t3, شروع, سرا, value, p_Error_کوڈ);
#جے_انت
    إذا(!overwrite && value==newTrie->initial_قدر) {
        أرجع; /* nothing to do */
    }

    حد_بندی=سرا+1;
    إذا(شروع&UTRIE2_انکڑے_نقاب) {
        UChar32 اگلا_شروع;

        /* set partial block at [start..following block boundary[ */
        بلاک=get_انکڑے_بلاک(newTrie, شروع, سچ);
        إذا(بلاک<0) {
            *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }

        اگلا_شروع=(شروع+UTRIE2_انکڑے_نقاب)&~UTRIE2_انکڑے_نقاب;
        إذا(اگلا_شروع<=حد_بندی) {
            fill_بلاک(newTrie->انکڑے+بلاک, شروع&UTRIE2_انکڑے_نقاب, UTRIE2_انکڑے_بلاک_لمائی,
                      value, newTrie->initial_قدر, overwrite);
            شروع=اگلا_شروع;
        } وإلا {
            fill_بلاک(newTrie->انکڑے+بلاک, شروع&UTRIE2_انکڑے_نقاب, حد_بندی&UTRIE2_انکڑے_نقاب,
                      value, newTrie->initial_قدر, overwrite);
            أرجع;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد_بندی&UTRIE2_انکڑے_نقاب;

    /* round down limit to a block boundary */
    حد_بندی&=~UTRIE2_انکڑے_نقاب;

    /* iterate over all-value blocks */
    إذا(value==newTrie->initial_قدر) {
        repeat_بلاک=newTrie->انکڑے_Null_وتھ;
    } وإلا {
        repeat_بلاک=-1;
    }

    طالما(شروع<حد_بندی) {
        int32_t i2;
        UBool سیٹ_Repeat_بلاک=خطأ;

        إذا(value==newTrie->initial_قدر && is_وچ_Null_بلاک(newTrie, شروع, سچ)) {
            شروع+=UTRIE2_انکڑے_بلاک_لمائی; /* nothing to do */
            استمر;
        }

        /* get index value */
        i2=get_Index2_بلاک(newTrie, شروع, سچ);
        إذا(i2<0) {
            *p_Error_کوڈ=U_اندرونی_پروگرام_ERROR;
            أرجع;
        }
        i2+=(شروع>>UTRIE2_کھسکاؤ_2)&UTRIE2_اشاریہ_2_نقاب;
        بلاک=newTrie->index2[i2];
        إذا(is_Writable_بلاک(newTrie, بلاک)) {
            /* already allocated */
            إذا(overwrite && بلاک>=UNEWTRIE2_انکڑے_0800_وتھ) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                سیٹ_Repeat_بلاک=سچ;
            } وإلا {
                /* !overwrite, or protected block: just write the values into this block */
                fill_بلاک(newTrie->انکڑے+بلاک,
                          0, UTRIE2_انکڑے_بلاک_لمائی,
                          value, newTrie->initial_قدر, overwrite);
            }
        } وإلا إذا(newTrie->انکڑے[بلاک]!=value && (overwrite || بلاک==newTrie->انکڑے_Null_وتھ)) {
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
            سیٹ_Repeat_بلاک=سچ;
        }
        إذا(سیٹ_Repeat_بلاک) {
            إذا(repeat_بلاک>=0) {
                سیٹ_Index2_اندراج(newTrie, i2, repeat_بلاک);
            } وإلا {
                /* create and set and fill the repeatBlock */
                repeat_بلاک=get_انکڑے_بلاک(newTrie, شروع, سچ);
                إذا(repeat_بلاک<0) {
                    *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
                    أرجع;
                }
                write_بلاک(newTrie->انکڑے+repeat_بلاک, value);
            }
        }

        شروع+=UTRIE2_انکڑے_بلاک_لمائی;
    }

    إذا(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلاک=get_انکڑے_بلاک(newTrie, شروع, سچ);
        إذا(بلاک<0) {
            *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }

        fill_بلاک(newTrie->انکڑے+بلاک, 0, rest, value, newTrie->initial_قدر, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

ساكن مضمن UBool
equal_int32(ثابت int32_t *s, ثابت int32_t *t, int32_t length) {
    طالما(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    أرجع length == 0;
}

ساكن مضمن UBool
equal_uint32(ثابت uint32_t *s, ثابت uint32_t *t, int32_t length) {
    طالما(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    أرجع length == 0;
}

ساكن int32_t
لبھو_Same_Index2_بلاک(ثابت int32_t *idx, int32_t index2_لمائی, int32_t other_بلاک) {
    int32_t بلاک;

    /* ensure that we do not even partially get past index2Length */
    index2_لمائی-=UTRIE2_اشاریہ_2_بلاک_لمائی;

    لكل(بلاک=0; بلاک<=index2_لمائی; ++بلاک) {
        إذا(equal_int32(idx+بلاک, idx+other_بلاک, UTRIE2_اشاریہ_2_بلاک_لمائی)) {
            أرجع بلاک;
        }
    }
    أرجع -1;
}

ساكن int32_t
لبھو_Same_انکڑے_بلاک_2(ثابت uint32_t *انکڑے, int32_t انکڑے_لمائی, int32_t other_بلاک, int32_t بلاک_لمائی) {
    int32_t بلاک;

    /* ensure that we do not even partially get past dataLength */
    انکڑے_لمائی-=بلاک_لمائی;

    لكل(بلاک=0; بلاک<=انکڑے_لمائی; بلاک+=UTRIE2_انکڑے_GRANULARITY) {
        إذا(equal_uint32(انکڑے+بلاک, انکڑے+other_بلاک, بلاک_لمائی)) {
            أرجع بلاک;
        }
    }
    أرجع -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
ساكن UChar32
لبھو_بلند_شروع(UNewTrie2 *trie, uint32_t بلند_قدر) {
    ثابت uint32_t *data32;

    uint32_t value, initial_قدر;
    UChar32 c, prev;
    int32_t i1, i2, j, i2_بلاک, prev_I2_بلاک, index2_Null_وتھ, بلاک, prev_بلاک, null_بلاک;

    data32=trie->انکڑے;
    initial_قدر=trie->initial_قدر;

    index2_Null_وتھ=trie->index2_Null_وتھ;
    null_بلاک=trie->انکڑے_Null_وتھ;

    /* set variables for previous range */
    إذا(بلند_قدر==initial_قدر) {
        prev_I2_بلاک=index2_Null_وتھ;
        prev_بلاک=null_بلاک;
    } وإلا {
        prev_I2_بلاک=-1;
        prev_بلاک=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_اشاریہ_1_لمائی;
    c=prev;
    طالما(c>0) {
        i2_بلاک=trie->index1[--i1];
        إذا(i2_بلاک==prev_I2_بلاک) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_فی_اشاریہ_1_اندراج;
            استمر;
        }
        prev_I2_بلاک=i2_بلاک;
        إذا(i2_بلاک==index2_Null_وتھ) {
            /* this is the null index-2 block */
            إذا(بلند_قدر!=initial_قدر) {
                أرجع c;
            }
            c-=UTRIE2_CP_فی_اشاریہ_1_اندراج;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            لكل(i2=UTRIE2_اشاریہ_2_بلاک_لمائی; i2>0;) {
                بلاک=trie->index2[i2_بلاک+ --i2];
                إذا(بلاک==prev_بلاک) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_انکڑے_بلاک_لمائی;
                    استمر;
                }
                prev_بلاک=بلاک;
                إذا(بلاک==null_بلاک) {
                    /* this is the null data block */
                    إذا(بلند_قدر!=initial_قدر) {
                        أرجع c;
                    }
                    c-=UTRIE2_انکڑے_بلاک_لمائی;
                } وإلا {
                    لكل(j=UTRIE2_انکڑے_بلاک_لمائی; j>0;) {
                        value=data32[بلاک+ --j];
                        إذا(value!=بلند_قدر) {
                            أرجع c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    أرجع 0;
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
ساكن فراغ
compact_انکڑے(UNewTrie2 *trie) {
#جے_تعریف_شدہ UTRIE2_DEBUG
    int32_t گنتی_Same=0, جوڑ_Overlaps=0;
#جے_انت

    int32_t شروع, new_شروع, moved_شروع;
    int32_t بلاک_لمائی, overlap;
    int32_t i, map_اشاریہ, بلاک_گنتی;

    /* do not compact linear-ASCII data */
    new_شروع=UTRIE2_انکڑے_شروع_وتھ;
    لكل(شروع=0, i=0; شروع<new_شروع; شروع+=UTRIE2_انکڑے_بلاک_لمائی, ++i) {
        trie->map[i]=شروع;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    بلاک_لمائی=64;
    بلاک_گنتی=بلاک_لمائی>>UTRIE2_کھسکاؤ_2;
    لكل(شروع=new_شروع; شروع<trie->انکڑے_لمائی;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        إذا(شروع==UNEWTRIE2_انکڑے_0800_وتھ) {
            بلاک_لمائی=UTRIE2_انکڑے_بلاک_لمائی;
            بلاک_گنتی=1;
        }

        /* skip blocks that are not used */
        إذا(trie->map[شروع>>UTRIE2_کھسکاؤ_2]<=0) {
            /* advance start to the next block */
            شروع+=بلاک_لمائی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* search for an identical block */
        إذا( (moved_شروع=لبھو_Same_انکڑے_بلاک_2(trie->انکڑے, new_شروع, شروع, بلاک_لمائی))
             >=0
        ) {
#جے_تعریف_شدہ UTRIE2_DEBUG
            ++گنتی_Same;
#جے_انت
            /* found an identical block, set the other block's index value for the current block */
            لكل(i=بلاک_گنتی, map_اشاریہ=شروع>>UTRIE2_کھسکاؤ_2; i>0; --i) {
                trie->map[map_اشاریہ++]=moved_شروع;
                moved_شروع+=UTRIE2_انکڑے_بلاک_لمائی;
            }

            /* advance start to the next block */
            شروع+=بلاک_لمائی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        لكل(overlap=بلاک_لمائی-UTRIE2_انکڑے_GRANULARITY;
            overlap>0 && !equal_uint32(trie->انکڑے+(new_شروع-overlap), trie->انکڑے+شروع, overlap);
            overlap-=UTRIE2_انکڑے_GRANULARITY) {}

#جے_تعریف_شدہ UTRIE2_DEBUG
            جوڑ_Overlaps+=overlap;
#جے_انت
        إذا(overlap>0 || new_شروع<شروع) {
            /* some overlap, or just move the whole block */
            moved_شروع=new_شروع-overlap;
            لكل(i=بلاک_گنتی, map_اشاریہ=شروع>>UTRIE2_کھسکاؤ_2; i>0; --i) {
                trie->map[map_اشاریہ++]=moved_شروع;
                moved_شروع+=UTRIE2_انکڑے_بلاک_لمائی;
            }

            /* move the non-overlapping indexes to their new positions */
            شروع+=overlap;
            لكل(i=بلاک_لمائی-overlap; i>0; --i) {
                trie->انکڑے[new_شروع++]=trie->انکڑے[شروع++];
            }
        } وإلا /* no overlap && newStart==start */ {
            لكل(i=بلاک_گنتی, map_اشاریہ=شروع>>UTRIE2_کھسکاؤ_2; i>0; --i) {
                trie->map[map_اشاریہ++]=شروع;
                شروع+=UTRIE2_انکڑے_بلاک_لمائی;
            }
            new_شروع=شروع;
        }
    }

    /* now adjust the index-2 table */
    لكل(i=0; i<trie->index2_لمائی; ++i) {
        إذا(i==UNEWTRIE2_اشاریہ_GAP_وتھ) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_اشاریہ_GAP_لمائی;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_کھسکاؤ_2];
    }
    trie->انکڑے_Null_وتھ=trie->map[trie->انکڑے_Null_وتھ>>UTRIE2_کھسکاؤ_2];

    /* ensure dataLength alignment */
    طالما((new_شروع&(UTRIE2_انکڑے_GRANULARITY-1))!=0) {
        trie->انکڑے[new_شروع++]=trie->initial_قدر;
    }

#جے_تعریف_شدہ UTRIE2_DEBUG
    /* we saved some space */
    لکھو("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (لما)trie->انکڑے_لمائی, (لما)new_شروع, (لما)گنتی_Same, (لما)جوڑ_Overlaps);
#جے_انت

    trie->انکڑے_لمائی=new_شروع;
}

ساكن فراغ
compactIndex2(UNewTrie2 *trie) {
    int32_t i, شروع, new_شروع, moved_شروع, overlap;

    /* do not compact linear-BMP index-2 blocks */
    new_شروع=UTRIE2_اشاریہ_2_BMP_لمائی;
    لكل(شروع=0, i=0; شروع<new_شروع; شروع+=UTRIE2_اشاریہ_2_بلاک_لمائی, ++i) {
        trie->map[i]=شروع;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    new_شروع+=UTRIE2_UTF8_2_B_اشاریہ_2_لمائی+((trie->بلند_شروع-0x10000)>>UTRIE2_کھسکاؤ_1);

    لكل(شروع=UNEWTRIE2_اشاریہ_2_NULL_وتھ; شروع<trie->index2_لمائی;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        إذا( (moved_شروع=لبھو_Same_Index2_بلاک(trie->index2, new_شروع, شروع))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[شروع>>UTRIE2_کھسکاؤ_1_2]=moved_شروع;

            /* advance start to the next block */
            شروع+=UTRIE2_اشاریہ_2_بلاک_لمائی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        لكل(overlap=UTRIE2_اشاریہ_2_بلاک_لمائی-1;
            overlap>0 && !equal_int32(trie->index2+(new_شروع-overlap), trie->index2+شروع, overlap);
            --overlap) {}

        إذا(overlap>0 || new_شروع<شروع) {
            /* some overlap, or just move the whole block */
            trie->map[شروع>>UTRIE2_کھسکاؤ_1_2]=new_شروع-overlap;

            /* move the non-overlapping indexes to their new positions */
            شروع+=overlap;
            لكل(i=UTRIE2_اشاریہ_2_بلاک_لمائی-overlap; i>0; --i) {
                trie->index2[new_شروع++]=trie->index2[شروع++];
            }
        } وإلا /* no overlap && newStart==start */ {
            trie->map[شروع>>UTRIE2_کھسکاؤ_1_2]=شروع;
            شروع+=UTRIE2_اشاریہ_2_بلاک_لمائی;
            new_شروع=شروع;
        }
    }

    /* now adjust the index-1 table */
    لكل(i=0; i<UNEWTRIE2_اشاریہ_1_لمائی; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_کھسکاؤ_1_2];
    }
    trie->index2_Null_وتھ=trie->map[trie->index2_Null_وتھ>>UTRIE2_کھسکاؤ_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    طالما((new_شروع&((UTRIE2_انکڑے_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[new_شروع++] = static_cast<int32_t>(0xffff) << UTRIE2_اشاریہ_کھسکاؤ;
    }

#جے_تعریف_شدہ UTRIE2_DEBUG
    /* we saved some space */
    لکھو("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (لما)trie->index2_لمائی, (لما)new_شروع);
#جے_انت

    trie->index2_لمائی=new_شروع;
}

ساكن فراغ
compactTrie(UTrie2 *trie, UError_کوڈ *p_Error_کوڈ) {
    UNewTrie2 *newTrie;
    UChar32 بلند_شروع, supp_بلند_شروع;
    uint32_t بلند_قدر;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    بلند_قدر=utrie2_get32(trie, 0x10ffff);
    بلند_شروع=لبھو_بلند_شروع(newTrie, بلند_قدر);
    بلند_شروع=(بلند_شروع+(UTRIE2_CP_فی_اشاریہ_1_اندراج-1))&~(UTRIE2_CP_فی_اشاریہ_1_اندراج-1);
    إذا(بلند_شروع==0x110000) {
        بلند_قدر=trie->error_قدر;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->بلند_شروع=newTrie->بلند_شروع=بلند_شروع;

#جے_تعریف_شدہ UTRIE2_DEBUG
    لکھو("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (لما)بلند_شروع, (لما)بلند_قدر, (لما)trie->initial_قدر);
#جے_انت

    إذا(بلند_شروع<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_بلند_شروع= بلند_شروع<=0x10000 ? 0x10000 : بلند_شروع;
        utrie2_سیٹ_Range32(trie, supp_بلند_شروع, 0x10ffff, trie->initial_قدر, سچ, p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            أرجع;
        }
    }

    compact_انکڑے(newTrie);
    إذا(بلند_شروع>0x10000) {
        compactIndex2(newTrie);
#جے_تعریف_شدہ UTRIE2_DEBUG
    } وإلا {
        لکھو("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (لما)بلند_شروع, (لما)trie->newTrie->index2_لمائی, (لما)UTRIE2_اشاریہ_1_وتھ);
#جے_انت
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->انکڑے[newTrie->انکڑے_لمائی++]=بلند_قدر;
    طالما((newTrie->انکڑے_لمائی&(UTRIE2_انکڑے_GRANULARITY-1))!=0) {
        newTrie->انکڑے[newTrie->انکڑے_لمائی++]=trie->initial_قدر;
    }

    newTrie->isCompacted=سچ;
}

/* serialization ------------------------------------------------------------ */

/**
 * Maximum length of the runtime index array.
 * Limited by its own 16-bit index values, and by uint16_t UTrie2Header.indexLength.
 * (The actual maximum length is lower,
 * (0x110000>>UTRIE2_SHIFT_2)+UTRIE2_UTF8_2B_INDEX_2_LENGTH+UTRIE2_MAX_INDEX_1_LENGTH.)
 */
#تعریف_کرو UTRIE2_MAX_اشاریہ_لمائی 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#تعریف_کرو UTRIE2_MAX_انکڑے_لمائی (0xffff<<UTRIE2_اشاریہ_کھسکاؤ)

/* Compact and internally serialize the trie. */
U_CAPI فراغ U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_قدر_بٹ قدر_بٹ, UError_کوڈ *p_Error_کوڈ) {
    UNewTrie2 *newTrie;
    UTrie2_سرخی *سرخی;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_لمائی;
    int32_t انکڑے_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 بلند_شروع;

    /* argument check */
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    إذا( trie==خالی_سوچک ||
        قدر_بٹ<0 || UTRIE2_گنتی_قدر_بٹ<=قدر_بٹ
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==خالی_سوچک) {
        /* already frozen */
        UTrie2_قدر_بٹ frozen_قدر_بٹ=
            trie->data16!=خالی_سوچک ? UTRIE2_16_قدر_بٹ : UTRIE2_32_قدر_بٹ;
        إذا(قدر_بٹ!=frozen_قدر_بٹ) {
            *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        }
        أرجع;
    }

    /* compact if necessary */
    إذا(!newTrie->isCompacted) {
        compactTrie(trie, p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            أرجع;
        }
    }
    بلند_شروع=trie->بلند_شروع;

    إذا(بلند_شروع<=0x10000) {
        all_Indexes_لمائی=UTRIE2_اشاریہ_1_وتھ;
    } وإلا {
        all_Indexes_لمائی=newTrie->index2_لمائی;
    }
    إذا(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        انکڑے_Move=all_Indexes_لمائی;
    } وإلا {
        انکڑے_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    إذا( /* for unshifted indexLength */
        all_Indexes_لمائی>UTRIE2_MAX_اشاریہ_لمائی ||
        /* for unshifted dataNullOffset */
        (انکڑے_Move+newTrie->انکڑے_Null_وتھ)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (انکڑے_Move+UNEWTRIE2_انکڑے_0800_وتھ)>0xffff ||
        /* for shiftedDataLength */
        (انکڑے_Move+newTrie->انکڑے_لمائی)>UTRIE2_MAX_انکڑے_لمائی
    ) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    /* calculate the total serialized length */
    length=حجم(UTrie2_سرخی)+all_Indexes_لمائی*2;
    إذا(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        length+=newTrie->انکڑے_لمائی*2;
    } وإلا {
        length+=newTrie->انکڑے_لمائی*4;
    }

    trie->حافظہ=uprv_malloc(length);
    إذا(trie->حافظہ==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    trie->length=length;
    trie->is_حافظہ_Owned=سچ;

    trie->اشاریہ_لمائی=all_Indexes_لمائی;
    trie->انکڑے_لمائی=newTrie->انکڑے_لمائی;
    إذا(بلند_شروع<=0x10000) {
        trie->index2_Null_وتھ=0xffff;
    } وإلا {
        trie->index2_Null_وتھ=static_cast<uint16_t>(UTRIE2_اشاریہ_2_وتھ+newTrie->index2_Null_وتھ);
    }
    trie->انکڑے_Null_وتھ=(uint16_t)(انکڑے_Move+newTrie->انکڑے_Null_وتھ);
    trie->بلند_قدر_اشاریہ=انکڑے_Move+trie->انکڑے_لمائی-UTRIE2_انکڑے_GRANULARITY;

    /* set the header fields */
    سرخی=(UTrie2_سرخی *)trie->حافظہ;

    سرخی->signature=UTRIE2_SIG; /* "Tri2" */
    سرخی->options=(uint16_t)قدر_بٹ;

    سرخی->اشاریہ_لمائی=(uint16_t)trie->اشاریہ_لمائی;
    سرخی->shifted_انکڑے_لمائی=(uint16_t)(trie->انکڑے_لمائی>>UTRIE2_اشاریہ_کھسکاؤ);
    سرخی->index2_Null_وتھ=trie->index2_Null_وتھ;
    سرخی->انکڑے_Null_وتھ=trie->انکڑے_Null_وتھ;
    سرخی->shifted_بلند_شروع=(uint16_t)(بلند_شروع>>UTRIE2_کھسکاؤ_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرخی+1);
    trie->اشاریہ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    لكل(i=UTRIE2_اشاریہ_2_BMP_لمائی; i>0; --i) {
        *dest16++=(uint16_t)((انکڑے_Move + *p++)>>UTRIE2_اشاریہ_کھسکاؤ);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(انکڑے_Move+UTRIE2_BAD_UTF8_انکڑے_وتھ);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(انکڑے_Move+newTrie->index2[i<<(6-UTRIE2_کھسکاؤ_2)]);
    }

    إذا(بلند_شروع>0x10000) {
        int32_t index1_لمائی=(بلند_شروع-0x10000)>>UTRIE2_کھسکاؤ_1;
        int32_t index2_وتھ=UTRIE2_اشاریہ_2_BMP_لمائی+UTRIE2_UTF8_2_B_اشاریہ_2_لمائی+index1_لمائی;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_اشاریہ_1_لمائی;
        لكل(i=index1_لمائی; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_اشاریہ_2_وتھ + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2_وتھ;
        لكل(i=newTrie->index2_لمائی-index2_وتھ; i>0; --i) {
            *dest16++=(uint16_t)((انکڑے_Move + *p++)>>UTRIE2_اشاریہ_کھسکاؤ);
        }
    }

    /* write the 16/32-bit data array */
    بدل(قدر_بٹ) {
    حالة UTRIE2_16_قدر_بٹ:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=خالی_سوچک;
        p=newTrie->انکڑے;
        لكل(i=newTrie->انکڑے_لمائی; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        اكسر;
    حالة UTRIE2_32_قدر_بٹ:
        /* write 32-bit data values */
        trie->data16=خالی_سوچک;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->انکڑے, (size_t)newTrie->انکڑے_لمائی*4);
        اكسر;
    افتراضي:
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

#جے_تعریف_شدہ UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#جے_انت

#جے_تعریف_شدہ UCPTRIE_DEBUG
    umutablecptrie_سیٹ_Name(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_قدر_چوڑائی)قدر_بٹ, p_Error_کوڈ));
#جے_انت
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->انکڑے);
    uprv_free(newTrie);
    trie->newTrie=خالی_سوچک;
}

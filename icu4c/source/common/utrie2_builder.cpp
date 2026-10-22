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
#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
#   include <stdio.h>
#جيڪڏهن_پورو
// #define UCPTRIE_DEBUG

#ضمن "unicode/utypes.h"
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "ucptrie_impl.h"
#جيڪڏهن_پورو
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
    UNEWTRIE2_اشاريو_2_NULL_وٿي=UNEWTRIE2_اشاريو_GAP_وٿي+UNEWTRIE2_اشاريو_GAP_ڊيگهائي,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_اشاريو_2_شروعات_وٿي=UNEWTRIE2_اشاريو_2_NULL_وٿي+UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_معلومات_NULL_وٿي=UTRIE2_معلومات_شروعات_وٿي,

    /** The start of allocated data blocks. */
    UNEWTRIE2_معلومات_شروعات_وٿي=UNEWTRIE2_معلومات_NULL_وٿي+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_معلومات_0800_وٿي=UNEWTRIE2_معلومات_شروعات_وٿي+0x780
};

/* Start with allocation of 16k data entries. */
#وصف_ڪريو UNEWTRIE2_INITIAL_معلومات_ڊيگهائي ((int32_t)1<<14)

/* Grow about 8x each time. */
#وصف_ڪريو UNEWTRIE2_MEDIUM_معلومات_ڊيگهائي ((int32_t)1<<17)

ساكن int32_t
alloc_Index2_بلاڪ(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_قدر, uint32_t error_قدر, UError_ڪوڊ *p_Error_ڪوڊ) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *معلومات;
    int32_t i, j;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(حجم(UNewTrie2));
    معلومات=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_معلومات_ڊيگهائي*4);
    إذا(trie==خالي_اشارو || newTrie==خالي_اشارو || معلومات==خالي_اشارو) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(معلومات);
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }

    uprv_memset(trie, 0, حجم(UTrie2));
    trie->initial_قدر=initial_قدر;
    trie->error_قدر=error_قدر;
    trie->مٿاهون_شروعات=0x110000;
    trie->newTrie=newTrie;
#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    trie->name="open";
#جيڪڏهن_پورو

    newTrie->معلومات=معلومات;
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_قدر, error_قدر, p_Error_ڪوڊ);
#جيڪڏهن_پورو
    newTrie->معلومات_صلاحيت=UNEWTRIE2_INITIAL_معلومات_ڊيگهائي;
    newTrie->initial_قدر=initial_قدر;
    newTrie->error_قدر=error_قدر;
    newTrie->مٿاهون_شروعات=0x110000;
    newTrie->پهريون_Free_بلاڪ=0;  /* no free block in the list */
    newTrie->isCompacted=خطأ;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    لكل(i=0; i<0x80; ++i) {
        newTrie->معلومات[i]=initial_قدر;
    }
    لكل(; i<0xc0; ++i) {
        newTrie->معلومات[i]=error_قدر;
    }
    لكل(i=UNEWTRIE2_معلومات_NULL_وٿي; i<UNEWTRIE2_معلومات_شروعات_وٿي; ++i) {
        newTrie->معلومات[i]=initial_قدر;
    }
    newTrie->معلومات_Null_وٿي=UNEWTRIE2_معلومات_NULL_وٿي;
    newTrie->معلومات_ڊيگهائي=UNEWTRIE2_معلومات_شروعات_وٿي;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    لكل(i=0, j=0; j<0x80; ++i, j+=UTRIE2_معلومات_بلاڪ_ڊيگهائي) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    لكل(; j<0xc0; ++i, j+=UTRIE2_معلومات_بلاڪ_ڊيگهائي) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_سرڪايو_2)-
        (0x80>>UTRIE2_سرڪايو_2)+
        1+
        UTRIE2_LSCP_اشاريو_2_ڊيگهائي;
    j+=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
    لكل(; j<UNEWTRIE2_معلومات_شروعات_وٿي; ++i, j+=UTRIE2_معلومات_بلاڪ_ڊيگهائي) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    لكل(i=0x80>>UTRIE2_سرڪايو_2; i<UTRIE2_اشاريو_2_BMP_ڊيگهائي; ++i) {
        newTrie->index2[i]=UNEWTRIE2_معلومات_NULL_وٿي;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    لكل(i=0; i<UNEWTRIE2_اشاريو_GAP_ڊيگهائي; ++i) {
        newTrie->index2[UNEWTRIE2_اشاريو_GAP_وٿي+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    لكل(i=0; i<UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي; ++i) {
        newTrie->index2[UNEWTRIE2_اشاريو_2_NULL_وٿي+i]=UNEWTRIE2_معلومات_NULL_وٿي;
    }
    newTrie->index2_Null_وٿي=UNEWTRIE2_اشاريو_2_NULL_وٿي;
    newTrie->index2_ڊيگهائي=UNEWTRIE2_اشاريو_2_شروعات_وٿي;

    /* set the index-1 indexes for the linear index-2 block */
    لكل(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_اشاريو_1_ڊيگهائي;
        ++i, j+=UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    لكل(; i<UNEWTRIE2_اشاريو_1_ڊيگهائي; ++i) {
        newTrie->index1[i]=UNEWTRIE2_اشاريو_2_NULL_وٿي;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    لكل(i=0x80; i<0x800; i+=UTRIE2_معلومات_بلاڪ_ڊيگهائي) {
        utrie2_set32(trie, i, initial_قدر, p_Error_ڪوڊ);
    }

    أرجع trie;
}

ساكن UNewTrie2 *
cloneBuilder(ثابت UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(حجم(UNewTrie2)));
    إذا(trie==خالي_اشارو) {
        أرجع خالي_اشارو;
    }

    trie->معلومات = static_cast<uint32_t*>(uprv_malloc(other->معلومات_صلاحيت * 4));
    إذا(trie->معلومات==خالي_اشارو) {
        uprv_free(trie);
        أرجع خالي_اشارو;
    }
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    إذا(other->t3==خالي_اشارو) {
        trie->t3=خالي_اشارو;
    } وإلا {
        UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &error_ڪوڊ);
    }
#جيڪڏهن_پورو
    trie->معلومات_صلاحيت=other->معلومات_صلاحيت;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, حجم(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_ڊيگهائي*4);
    trie->index2_Null_وٿي=other->index2_Null_وٿي;
    trie->index2_ڊيگهائي=other->index2_ڊيگهائي;

    uprv_memcpy(trie->معلومات, other->معلومات, (size_t)other->معلومات_ڊيگهائي*4);
    trie->معلومات_Null_وٿي=other->معلومات_Null_وٿي;
    trie->معلومات_ڊيگهائي=other->معلومات_ڊيگهائي;

    /* reference counters */
    إذا(other->isCompacted) {
        trie->پهريون_Free_بلاڪ=0;
    } وإلا {
        uprv_memcpy(trie->map, other->map, ((size_t)other->معلومات_ڊيگهائي>>UTRIE2_سرڪايو_2)*4);
        trie->پهريون_Free_بلاڪ=other->پهريون_Free_بلاڪ;
    }

    trie->initial_قدر=other->initial_قدر;
    trie->error_قدر=other->error_قدر;
    trie->مٿاهون_شروعات=other->مٿاهون_شروعات;
    trie->isCompacted=other->isCompacted;

    أرجع trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ثابت UTrie2 *other, UError_ڪوڊ *p_Error_ڪوڊ) {
    UTrie2 *trie;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا(other==خالي_اشارو || (other->يادگيري==خالي_اشارو && other->newTrie==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memcpy(trie, other, حجم(UTrie2));

    إذا(other->يادگيري!=خالي_اشارو) {
        trie->يادگيري=uprv_malloc(other->length);
        إذا(trie->يادگيري!=خالي_اشارو) {
            trie->is_يادگيري_Owned=سچ;
            uprv_memcpy(trie->يادگيري, other->يادگيري, other->length);

            /* make the clone's pointers point to its own memory */
            trie->اشاريو=(uint16_t *)trie->يادگيري+(other->اشاريو-(uint16_t *)other->يادگيري);
            إذا(other->data16!=خالي_اشارو) {
                trie->data16=(uint16_t *)trie->يادگيري+(other->data16-(uint16_t *)other->يادگيري);
            }
            إذا(other->data32!=خالي_اشارو) {
                trie->data32=(uint32_t *)trie->يادگيري+(other->data32-(uint32_t *)other->يادگيري);
            }
        }
    } وإلا /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    إذا(trie->يادگيري==خالي_اشارو && trie->newTrie==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        uprv_free(trie);
        trie=خالي_اشارو;
    }
    أرجع trie;
}

تعريفنوع بنية New_Trie_And_حالت {
    UTrie2 *trie;
    UError_ڪوڊ error_ڪوڊ;
    UBool exclusive_حد_بندي;  /* rather than inclusive range end */
} New_Trie_And_حالت;

ساكن UBool U_CALLCONV
copyEnumRange(ثابت فراغ *context, UChar32 شروعات, UChar32 پڇاڙي, uint32_t value) {
    New_Trie_And_حالت *nt=(New_Trie_And_حالت *)context;
    إذا(value!=nt->trie->initial_قدر) {
        إذا(nt->exclusive_حد_بندي) {
            --پڇاڙي;
        }
        إذا(شروعات==پڇاڙي) {
            utrie2_set32(nt->trie, شروعات, value, &nt->error_ڪوڊ);
        } وإلا {
            utrie2_سيٽ_Range32(nt->trie, شروعات, پڇاڙي, value, سچ, &nt->error_ڪوڊ);
        }
        أرجع U_SUCCESS(nt->error_ڪوڊ);
    } وإلا {
        أرجع سچ;
    }
}

#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
ساكن ڊگهو ڳڻپ_Initial(ثابت UTrie2 *trie) {
    uint32_t initial_قدر=trie->initial_قدر;
    int32_t length=trie->معلومات_ڊيگهائي;
    ڊگهو count=0;
    إذا(trie->data16!=خالي_اشارو) {
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
    ڊگهو اشاريو_ڊيگهائي=trie->اشاريو_ڊيگهائي;
    ڊگهو معلومات_ڊيگهائي=(ڊگهو)trie->معلومات_ڊيگهائي;
    ڊگهو ڪل_ڊيگهائي=(ڊگهو)حجم(UTrie_سرو)+اشاريو_ڊيگهائي*2+معلومات_ڊيگهائي*(trie->data32!=خالي_اشارو ? 4 : 2);
    لکو("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           اشاريو_ڊيگهائي, معلومات_ڊيگهائي, ڪل_ڊيگهائي);
}

ساكن فراغ
utrie2_printLengths(ثابت UTrie2 *trie, ثابت حرف *which) {
    ڊگهو اشاريو_ڊيگهائي=trie->اشاريو_ڊيگهائي;
    ڊگهو معلومات_ڊيگهائي=(ڊگهو)trie->معلومات_ڊيگهائي;
    ڊگهو ڪل_ڊيگهائي=(ڊگهو)حجم(UTrie2_سرو)+اشاريو_ڊيگهائي*2+معلومات_ڊيگهائي*(trie->data32!=خالي_اشارو ? 4 : 2);
    لکو("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, اشاريو_ڊيگهائي, معلومات_ڊيگهائي, ڳڻپ_Initial(trie), ڪل_ڊيگهائي);
}
#جيڪڏهن_پورو

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ثابت UTrie2 *other, UError_ڪوڊ *p_Error_ڪوڊ) {
    New_Trie_And_حالت context;
    char16_t lead;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا(other==خالي_اشارو || (other->يادگيري==خالي_اشارو && other->newTrie==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    إذا(other->newTrie!=خالي_اشارو && !other->newTrie->isCompacted) {
        أرجع utrie2_clone(other, p_Error_ڪوڊ);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_قدر, other->error_قدر, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    context.exclusive_حد_بندي=خطأ;
    context.error_ڪوڊ=*p_Error_ڪوڊ;
    utrie2_enum(other, خالي_اشارو, copyEnumRange, &context);
    *p_Error_ڪوڊ=context.error_ڪوڊ;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(other->data32==خالي_اشارو) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } وإلا {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        إذا(value!=other->initial_قدر) {
            utrie2_set32_For_Lead_Surrogate_ڪوڊ_Unit(context.trie, lead, value, p_Error_ڪوڊ);
        }
    }
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        utrie2_close(context.trie);
        context.trie=خالي_اشارو;
    }
    أرجع context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ثابت UTrie *trie1, uint32_t error_قدر, UError_ڪوڊ *p_Error_ڪوڊ) {
    New_Trie_And_حالت context;
    char16_t lead;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا(trie1==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    context.trie=utrie2_open(trie1->initial_قدر, error_قدر, p_Error_ڪوڊ);
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    context.exclusive_حد_بندي=سچ;
    context.error_ڪوڊ=*p_Error_ڪوڊ;
    utrie_enum(trie1, خالي_اشارو, copyEnumRange, &context);
    *p_Error_ڪوڊ=context.error_ڪوڊ;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(trie1->data32==خالي_اشارو) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } وإلا {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        إذا(value!=trie1->initial_قدر) {
            utrie2_set32_For_Lead_Surrogate_ڪوڊ_Unit(context.trie, lead, value, p_Error_ڪوڊ);
        }
    }
    إذا(U_SUCCESS(*p_Error_ڪوڊ)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=خالي_اشارو ? UTRIE2_32_قدر_بٽ : UTRIE2_16_قدر_بٽ,
                      p_Error_ڪوڊ);
    }
#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    إذا(U_SUCCESS(*p_Error_ڪوڊ)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#جيڪڏهن_پورو
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        utrie2_close(context.trie);
        context.trie=خالي_اشارو;
    }
    أرجع context.trie;
}

ساكن مضمن UBool
is_اندريون_Null_بلاڪ(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, بلاڪ;

    إذا(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_اشاريو_2_وٿي-(0xd800>>UTRIE2_سرڪايو_2))+
            (c>>UTRIE2_سرڪايو_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_سرڪايو_1]+
            ((c>>UTRIE2_سرڪايو_2)&UTRIE2_اشاريو_2_نقاب);
    }
    بلاڪ=trie->index2[i2];
    أرجع بلاڪ == trie->معلومات_Null_وٿي;
}

ساكن int32_t
alloc_Index2_بلاڪ(UNewTrie2 *trie) {
    int32_t new_بلاڪ, new_چوٽي;

    new_بلاڪ=trie->index2_ڊيگهائي;
    new_چوٽي=new_بلاڪ+UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي;
    إذا(new_چوٽي>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        أرجع -1;
    }
    trie->index2_ڊيگهائي=new_چوٽي;
    uprv_memcpy(trie->index2+new_بلاڪ, trie->index2+trie->index2_Null_وٿي, UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي*4);
    أرجع new_بلاڪ;
}

ساكن int32_t
get_Index2_بلاڪ(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    إذا(U_IS_LEAD(c) && forLSCP) {
        أرجع UTRIE2_LSCP_اشاريو_2_وٿي;
    }

    i1=c>>UTRIE2_سرڪايو_1;
    i2=trie->index1[i1];
    إذا(i2==trie->index2_Null_وٿي) {
        i2=alloc_Index2_بلاڪ(trie);
        إذا(i2<0) {
            أرجع -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    أرجع i2;
}

ساكن int32_t
alloc_معلومات_بلاڪ(UNewTrie2 *trie, int32_t copy_بلاڪ) {
    int32_t new_بلاڪ, new_چوٽي;

    إذا(trie->پهريون_Free_بلاڪ!=0) {
        /* get the first free block */
        new_بلاڪ=trie->پهريون_Free_بلاڪ;
        trie->پهريون_Free_بلاڪ=-trie->map[new_بلاڪ>>UTRIE2_سرڪايو_2];
    } وإلا {
        /* get a new block from the high end */
        new_بلاڪ=trie->معلومات_ڊيگهائي;
        new_چوٽي=new_بلاڪ+UTRIE2_معلومات_بلاڪ_ڊيگهائي;
        إذا(new_چوٽي>trie->معلومات_صلاحيت) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *معلومات;

            إذا(trie->معلومات_صلاحيت<UNEWTRIE2_MEDIUM_معلومات_ڊيگهائي) {
                capacity=UNEWTRIE2_MEDIUM_معلومات_ڊيگهائي;
            } وإلا إذا(trie->معلومات_صلاحيت<UNEWTRIE2_MAX_معلومات_ڊيگهائي) {
                capacity=UNEWTRIE2_MAX_معلومات_ڊيگهائي;
            } وإلا {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                أرجع -1;
            }
            معلومات = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            إذا(معلومات==خالي_اشارو) {
                أرجع -1;
            }
            uprv_memcpy(معلومات, trie->معلومات, (size_t)trie->معلومات_ڊيگهائي*4);
            uprv_free(trie->معلومات);
            trie->معلومات=معلومات;
            trie->معلومات_صلاحيت=capacity;
        }
        trie->معلومات_ڊيگهائي=new_چوٽي;
    }
    uprv_memcpy(trie->معلومات+new_بلاڪ, trie->معلومات+copy_بلاڪ, UTRIE2_معلومات_بلاڪ_ڊيگهائي*4);
    trie->map[new_بلاڪ>>UTRIE2_سرڪايو_2]=0;
    أرجع new_بلاڪ;
}

/* call when the block's reference counter reaches 0 */
ساكن فراغ
release_معلومات_بلاڪ(UNewTrie2 *trie, int32_t بلاڪ) {
    /* put this block at the front of the free-block chain */
    trie->map[بلاڪ>>UTRIE2_سرڪايو_2]=-trie->پهريون_Free_بلاڪ;
    trie->پهريون_Free_بلاڪ=بلاڪ;
}

ساكن مضمن UBool
is_Writable_بلاڪ(UNewTrie2 *trie, int32_t بلاڪ) {
    أرجع بلاڪ != trie->معلومات_Null_وٿي && 1 == trie->map[بلاڪ >> UTRIE2_سرڪايو_2];
}

ساكن مضمن فراغ
سيٽ_Index2_داخلا(UNewTrie2 *trie, int32_t i2, int32_t بلاڪ) {
    int32_t old_بلاڪ;
    ++trie->map[بلاڪ>>UTRIE2_سرڪايو_2];  /* increment first, in case block==oldBlock! */
    old_بلاڪ=trie->index2[i2];
    إذا(0 == --trie->map[old_بلاڪ>>UTRIE2_سرڪايو_2]) {
        release_معلومات_بلاڪ(trie, old_بلاڪ);
    }
    trie->index2[i2]=بلاڪ;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ساكن int32_t
get_معلومات_بلاڪ(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, old_بلاڪ, new_بلاڪ;

    i2=get_Index2_بلاڪ(trie, c, forLSCP);
    إذا(i2<0) {
        أرجع -1;  /* program error */
    }

    i2+=(c>>UTRIE2_سرڪايو_2)&UTRIE2_اشاريو_2_نقاب;
    old_بلاڪ=trie->index2[i2];
    إذا(is_Writable_بلاڪ(trie, old_بلاڪ)) {
        أرجع old_بلاڪ;
    }

    /* allocate a new data block */
    new_بلاڪ=alloc_معلومات_بلاڪ(trie, old_بلاڪ);
    إذا(new_بلاڪ<0) {
        /* out of memory in the data array */
        أرجع -1;
    }
    سيٽ_Index2_داخلا(trie, i2, new_بلاڪ);
    أرجع new_بلاڪ;
}

/**
 * @return true if the value was successfully set
 */
ساكن فراغ
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UError_ڪوڊ *p_Error_ڪوڊ) {
    int32_t بلاڪ;

    إذا(trie==خالي_اشارو || trie->isCompacted) {
        *p_Error_ڪوڊ=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    umutablecptrie_سيٽ(trie->t3, c, value, p_Error_ڪوڊ);
#جيڪڏهن_پورو

    بلاڪ=get_معلومات_بلاڪ(trie, c, forLSCP);
    إذا(بلاڪ<0) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    trie->معلومات[بلاڪ+(c&UTRIE2_معلومات_نقاب)]=value;
}

U_CAPI فراغ U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    إذا((uint32_t)c>0x10ffff) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, سچ, value, p_Error_ڪوڊ);
}

U_CAPI فراغ U_EXPORT2
utrie2_set32_For_Lead_Surrogate_ڪوڊ_Unit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    إذا(!U_IS_LEAD(c)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, خطأ, value, p_Error_ڪوڊ);
}

ساكن فراغ
write_بلاڪ(uint32_t *بلاڪ, uint32_t value) {
    uint32_t *حد_بندي=بلاڪ+UTRIE2_معلومات_بلاڪ_ڊيگهائي;
    طالما(بلاڪ<حد_بندي) {
        *بلاڪ++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
ساكن فراغ
fill_بلاڪ(uint32_t *بلاڪ, UChar32 شروعات, UChar32 حد_بندي,
          uint32_t value, uint32_t initial_قدر, UBool overwrite) {
    uint32_t *p_حد_بندي;

    p_حد_بندي=بلاڪ+حد_بندي;
    بلاڪ+=شروعات;
    إذا(overwrite) {
        طالما(بلاڪ<p_حد_بندي) {
            *بلاڪ++=value;
        }
    } وإلا {
        طالما(بلاڪ<p_حد_بندي) {
            إذا(*بلاڪ==initial_قدر) {
                *بلاڪ=value;
            }
            ++بلاڪ;
        }
    }
}

U_CAPI فراغ U_EXPORT2
utrie2_سيٽ_Range32(UTrie2 *trie,
                  UChar32 شروعات, UChar32 پڇاڙي,
                  uint32_t value, UBool overwrite,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t بلاڪ, rest, repeat_بلاڪ;
    UChar32 حد_بندي;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    إذا((uint32_t)شروعات>0x10ffff || (uint32_t)پڇاڙي>0x10ffff || شروعات>پڇاڙي) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==خالي_اشارو || newTrie->isCompacted) {
        *p_Error_ڪوڊ=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    umutablecptrie_سيٽ_Range(newTrie->t3, شروعات, پڇاڙي, value, p_Error_ڪوڊ);
#جيڪڏهن_پورو
    إذا(!overwrite && value==newTrie->initial_قدر) {
        أرجع; /* nothing to do */
    }

    حد_بندي=پڇاڙي+1;
    إذا(شروعات&UTRIE2_معلومات_نقاب) {
        UChar32 ايندڙ_شروعات;

        /* set partial block at [start..following block boundary[ */
        بلاڪ=get_معلومات_بلاڪ(newTrie, شروعات, سچ);
        إذا(بلاڪ<0) {
            *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع;
        }

        ايندڙ_شروعات=(شروعات+UTRIE2_معلومات_نقاب)&~UTRIE2_معلومات_نقاب;
        إذا(ايندڙ_شروعات<=حد_بندي) {
            fill_بلاڪ(newTrie->معلومات+بلاڪ, شروعات&UTRIE2_معلومات_نقاب, UTRIE2_معلومات_بلاڪ_ڊيگهائي,
                      value, newTrie->initial_قدر, overwrite);
            شروعات=ايندڙ_شروعات;
        } وإلا {
            fill_بلاڪ(newTrie->معلومات+بلاڪ, شروعات&UTRIE2_معلومات_نقاب, حد_بندي&UTRIE2_معلومات_نقاب,
                      value, newTrie->initial_قدر, overwrite);
            أرجع;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد_بندي&UTRIE2_معلومات_نقاب;

    /* round down limit to a block boundary */
    حد_بندي&=~UTRIE2_معلومات_نقاب;

    /* iterate over all-value blocks */
    إذا(value==newTrie->initial_قدر) {
        repeat_بلاڪ=newTrie->معلومات_Null_وٿي;
    } وإلا {
        repeat_بلاڪ=-1;
    }

    طالما(شروعات<حد_بندي) {
        int32_t i2;
        UBool سيٽ_Repeat_بلاڪ=خطأ;

        إذا(value==newTrie->initial_قدر && is_اندريون_Null_بلاڪ(newTrie, شروعات, سچ)) {
            شروعات+=UTRIE2_معلومات_بلاڪ_ڊيگهائي; /* nothing to do */
            استمر;
        }

        /* get index value */
        i2=get_Index2_بلاڪ(newTrie, شروعات, سچ);
        إذا(i2<0) {
            *p_Error_ڪوڊ=U_اندروني_پروگرام_ERROR;
            أرجع;
        }
        i2+=(شروعات>>UTRIE2_سرڪايو_2)&UTRIE2_اشاريو_2_نقاب;
        بلاڪ=newTrie->index2[i2];
        إذا(is_Writable_بلاڪ(newTrie, بلاڪ)) {
            /* already allocated */
            إذا(overwrite && بلاڪ>=UNEWTRIE2_معلومات_0800_وٿي) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                سيٽ_Repeat_بلاڪ=سچ;
            } وإلا {
                /* !overwrite, or protected block: just write the values into this block */
                fill_بلاڪ(newTrie->معلومات+بلاڪ,
                          0, UTRIE2_معلومات_بلاڪ_ڊيگهائي,
                          value, newTrie->initial_قدر, overwrite);
            }
        } وإلا إذا(newTrie->معلومات[بلاڪ]!=value && (overwrite || بلاڪ==newTrie->معلومات_Null_وٿي)) {
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
            سيٽ_Repeat_بلاڪ=سچ;
        }
        إذا(سيٽ_Repeat_بلاڪ) {
            إذا(repeat_بلاڪ>=0) {
                سيٽ_Index2_داخلا(newTrie, i2, repeat_بلاڪ);
            } وإلا {
                /* create and set and fill the repeatBlock */
                repeat_بلاڪ=get_معلومات_بلاڪ(newTrie, شروعات, سچ);
                إذا(repeat_بلاڪ<0) {
                    *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                    أرجع;
                }
                write_بلاڪ(newTrie->معلومات+repeat_بلاڪ, value);
            }
        }

        شروعات+=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
    }

    إذا(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلاڪ=get_معلومات_بلاڪ(newTrie, شروعات, سچ);
        إذا(بلاڪ<0) {
            *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع;
        }

        fill_بلاڪ(newTrie->معلومات+بلاڪ, 0, rest, value, newTrie->initial_قدر, overwrite);
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
ڳوليو_Same_Index2_بلاڪ(ثابت int32_t *idx, int32_t index2_ڊيگهائي, int32_t other_بلاڪ) {
    int32_t بلاڪ;

    /* ensure that we do not even partially get past index2Length */
    index2_ڊيگهائي-=UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي;

    لكل(بلاڪ=0; بلاڪ<=index2_ڊيگهائي; ++بلاڪ) {
        إذا(equal_int32(idx+بلاڪ, idx+other_بلاڪ, UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي)) {
            أرجع بلاڪ;
        }
    }
    أرجع -1;
}

ساكن int32_t
ڳوليو_Same_معلومات_بلاڪ_2(ثابت uint32_t *معلومات, int32_t معلومات_ڊيگهائي, int32_t other_بلاڪ, int32_t بلاڪ_ڊيگهائي) {
    int32_t بلاڪ;

    /* ensure that we do not even partially get past dataLength */
    معلومات_ڊيگهائي-=بلاڪ_ڊيگهائي;

    لكل(بلاڪ=0; بلاڪ<=معلومات_ڊيگهائي; بلاڪ+=UTRIE2_معلومات_GRANULARITY) {
        إذا(equal_uint32(معلومات+بلاڪ, معلومات+other_بلاڪ, بلاڪ_ڊيگهائي)) {
            أرجع بلاڪ;
        }
    }
    أرجع -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
ساكن UChar32
ڳوليو_مٿاهون_شروعات(UNewTrie2 *trie, uint32_t مٿاهون_قدر) {
    ثابت uint32_t *data32;

    uint32_t value, initial_قدر;
    UChar32 c, prev;
    int32_t i1, i2, j, i2_بلاڪ, prev_I2_بلاڪ, index2_Null_وٿي, بلاڪ, prev_بلاڪ, null_بلاڪ;

    data32=trie->معلومات;
    initial_قدر=trie->initial_قدر;

    index2_Null_وٿي=trie->index2_Null_وٿي;
    null_بلاڪ=trie->معلومات_Null_وٿي;

    /* set variables for previous range */
    إذا(مٿاهون_قدر==initial_قدر) {
        prev_I2_بلاڪ=index2_Null_وٿي;
        prev_بلاڪ=null_بلاڪ;
    } وإلا {
        prev_I2_بلاڪ=-1;
        prev_بلاڪ=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_اشاريو_1_ڊيگهائي;
    c=prev;
    طالما(c>0) {
        i2_بلاڪ=trie->index1[--i1];
        إذا(i2_بلاڪ==prev_I2_بلاڪ) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_في_اشاريو_1_داخلا;
            استمر;
        }
        prev_I2_بلاڪ=i2_بلاڪ;
        إذا(i2_بلاڪ==index2_Null_وٿي) {
            /* this is the null index-2 block */
            إذا(مٿاهون_قدر!=initial_قدر) {
                أرجع c;
            }
            c-=UTRIE2_CP_في_اشاريو_1_داخلا;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            لكل(i2=UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي; i2>0;) {
                بلاڪ=trie->index2[i2_بلاڪ+ --i2];
                إذا(بلاڪ==prev_بلاڪ) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
                    استمر;
                }
                prev_بلاڪ=بلاڪ;
                إذا(بلاڪ==null_بلاڪ) {
                    /* this is the null data block */
                    إذا(مٿاهون_قدر!=initial_قدر) {
                        أرجع c;
                    }
                    c-=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
                } وإلا {
                    لكل(j=UTRIE2_معلومات_بلاڪ_ڊيگهائي; j>0;) {
                        value=data32[بلاڪ+ --j];
                        إذا(value!=مٿاهون_قدر) {
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
compact_معلومات(UNewTrie2 *trie) {
#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    int32_t ڳڻپ_Same=0, جوڙ_Overlaps=0;
#جيڪڏهن_پورو

    int32_t شروعات, new_شروعات, moved_شروعات;
    int32_t بلاڪ_ڊيگهائي, overlap;
    int32_t i, map_اشاريو, بلاڪ_ڳڻپ;

    /* do not compact linear-ASCII data */
    new_شروعات=UTRIE2_معلومات_شروعات_وٿي;
    لكل(شروعات=0, i=0; شروعات<new_شروعات; شروعات+=UTRIE2_معلومات_بلاڪ_ڊيگهائي, ++i) {
        trie->map[i]=شروعات;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    بلاڪ_ڊيگهائي=64;
    بلاڪ_ڳڻپ=بلاڪ_ڊيگهائي>>UTRIE2_سرڪايو_2;
    لكل(شروعات=new_شروعات; شروعات<trie->معلومات_ڊيگهائي;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        إذا(شروعات==UNEWTRIE2_معلومات_0800_وٿي) {
            بلاڪ_ڊيگهائي=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
            بلاڪ_ڳڻپ=1;
        }

        /* skip blocks that are not used */
        إذا(trie->map[شروعات>>UTRIE2_سرڪايو_2]<=0) {
            /* advance start to the next block */
            شروعات+=بلاڪ_ڊيگهائي;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* search for an identical block */
        إذا( (moved_شروعات=ڳوليو_Same_معلومات_بلاڪ_2(trie->معلومات, new_شروعات, شروعات, بلاڪ_ڊيگهائي))
             >=0
        ) {
#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
            ++ڳڻپ_Same;
#جيڪڏهن_پورو
            /* found an identical block, set the other block's index value for the current block */
            لكل(i=بلاڪ_ڳڻپ, map_اشاريو=شروعات>>UTRIE2_سرڪايو_2; i>0; --i) {
                trie->map[map_اشاريو++]=moved_شروعات;
                moved_شروعات+=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
            }

            /* advance start to the next block */
            شروعات+=بلاڪ_ڊيگهائي;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        لكل(overlap=بلاڪ_ڊيگهائي-UTRIE2_معلومات_GRANULARITY;
            overlap>0 && !equal_uint32(trie->معلومات+(new_شروعات-overlap), trie->معلومات+شروعات, overlap);
            overlap-=UTRIE2_معلومات_GRANULARITY) {}

#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
            جوڙ_Overlaps+=overlap;
#جيڪڏهن_پورو
        إذا(overlap>0 || new_شروعات<شروعات) {
            /* some overlap, or just move the whole block */
            moved_شروعات=new_شروعات-overlap;
            لكل(i=بلاڪ_ڳڻپ, map_اشاريو=شروعات>>UTRIE2_سرڪايو_2; i>0; --i) {
                trie->map[map_اشاريو++]=moved_شروعات;
                moved_شروعات+=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
            }

            /* move the non-overlapping indexes to their new positions */
            شروعات+=overlap;
            لكل(i=بلاڪ_ڊيگهائي-overlap; i>0; --i) {
                trie->معلومات[new_شروعات++]=trie->معلومات[شروعات++];
            }
        } وإلا /* no overlap && newStart==start */ {
            لكل(i=بلاڪ_ڳڻپ, map_اشاريو=شروعات>>UTRIE2_سرڪايو_2; i>0; --i) {
                trie->map[map_اشاريو++]=شروعات;
                شروعات+=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
            }
            new_شروعات=شروعات;
        }
    }

    /* now adjust the index-2 table */
    لكل(i=0; i<trie->index2_ڊيگهائي; ++i) {
        إذا(i==UNEWTRIE2_اشاريو_GAP_وٿي) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_اشاريو_GAP_ڊيگهائي;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_سرڪايو_2];
    }
    trie->معلومات_Null_وٿي=trie->map[trie->معلومات_Null_وٿي>>UTRIE2_سرڪايو_2];

    /* ensure dataLength alignment */
    طالما((new_شروعات&(UTRIE2_معلومات_GRANULARITY-1))!=0) {
        trie->معلومات[new_شروعات++]=trie->initial_قدر;
    }

#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    /* we saved some space */
    لکو("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (ڊگهو)trie->معلومات_ڊيگهائي, (ڊگهو)new_شروعات, (ڊگهو)ڳڻپ_Same, (ڊگهو)جوڙ_Overlaps);
#جيڪڏهن_پورو

    trie->معلومات_ڊيگهائي=new_شروعات;
}

ساكن فراغ
compactIndex2(UNewTrie2 *trie) {
    int32_t i, شروعات, new_شروعات, moved_شروعات, overlap;

    /* do not compact linear-BMP index-2 blocks */
    new_شروعات=UTRIE2_اشاريو_2_BMP_ڊيگهائي;
    لكل(شروعات=0, i=0; شروعات<new_شروعات; شروعات+=UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي, ++i) {
        trie->map[i]=شروعات;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    new_شروعات+=UTRIE2_UTF8_2_B_اشاريو_2_ڊيگهائي+((trie->مٿاهون_شروعات-0x10000)>>UTRIE2_سرڪايو_1);

    لكل(شروعات=UNEWTRIE2_اشاريو_2_NULL_وٿي; شروعات<trie->index2_ڊيگهائي;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        إذا( (moved_شروعات=ڳوليو_Same_Index2_بلاڪ(trie->index2, new_شروعات, شروعات))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[شروعات>>UTRIE2_سرڪايو_1_2]=moved_شروعات;

            /* advance start to the next block */
            شروعات+=UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        لكل(overlap=UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي-1;
            overlap>0 && !equal_int32(trie->index2+(new_شروعات-overlap), trie->index2+شروعات, overlap);
            --overlap) {}

        إذا(overlap>0 || new_شروعات<شروعات) {
            /* some overlap, or just move the whole block */
            trie->map[شروعات>>UTRIE2_سرڪايو_1_2]=new_شروعات-overlap;

            /* move the non-overlapping indexes to their new positions */
            شروعات+=overlap;
            لكل(i=UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي-overlap; i>0; --i) {
                trie->index2[new_شروعات++]=trie->index2[شروعات++];
            }
        } وإلا /* no overlap && newStart==start */ {
            trie->map[شروعات>>UTRIE2_سرڪايو_1_2]=شروعات;
            شروعات+=UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي;
            new_شروعات=شروعات;
        }
    }

    /* now adjust the index-1 table */
    لكل(i=0; i<UNEWTRIE2_اشاريو_1_ڊيگهائي; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_سرڪايو_1_2];
    }
    trie->index2_Null_وٿي=trie->map[trie->index2_Null_وٿي>>UTRIE2_سرڪايو_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    طالما((new_شروعات&((UTRIE2_معلومات_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[new_شروعات++] = static_cast<int32_t>(0xffff) << UTRIE2_اشاريو_سرڪايو;
    }

#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    /* we saved some space */
    لکو("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (ڊگهو)trie->index2_ڊيگهائي, (ڊگهو)new_شروعات);
#جيڪڏهن_پورو

    trie->index2_ڊيگهائي=new_شروعات;
}

ساكن فراغ
compactTrie(UTrie2 *trie, UError_ڪوڊ *p_Error_ڪوڊ) {
    UNewTrie2 *newTrie;
    UChar32 مٿاهون_شروعات, supp_مٿاهون_شروعات;
    uint32_t مٿاهون_قدر;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    مٿاهون_قدر=utrie2_get32(trie, 0x10ffff);
    مٿاهون_شروعات=ڳوليو_مٿاهون_شروعات(newTrie, مٿاهون_قدر);
    مٿاهون_شروعات=(مٿاهون_شروعات+(UTRIE2_CP_في_اشاريو_1_داخلا-1))&~(UTRIE2_CP_في_اشاريو_1_داخلا-1);
    إذا(مٿاهون_شروعات==0x110000) {
        مٿاهون_قدر=trie->error_قدر;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->مٿاهون_شروعات=newTrie->مٿاهون_شروعات=مٿاهون_شروعات;

#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    لکو("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (ڊگهو)مٿاهون_شروعات, (ڊگهو)مٿاهون_قدر, (ڊگهو)trie->initial_قدر);
#جيڪڏهن_پورو

    إذا(مٿاهون_شروعات<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_مٿاهون_شروعات= مٿاهون_شروعات<=0x10000 ? 0x10000 : مٿاهون_شروعات;
        utrie2_سيٽ_Range32(trie, supp_مٿاهون_شروعات, 0x10ffff, trie->initial_قدر, سچ, p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع;
        }
    }

    compact_معلومات(newTrie);
    إذا(مٿاهون_شروعات>0x10000) {
        compactIndex2(newTrie);
#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    } وإلا {
        لکو("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (ڊگهو)مٿاهون_شروعات, (ڊگهو)trie->newTrie->index2_ڊيگهائي, (ڊگهو)UTRIE2_اشاريو_1_وٿي);
#جيڪڏهن_پورو
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->معلومات[newTrie->معلومات_ڊيگهائي++]=مٿاهون_قدر;
    طالما((newTrie->معلومات_ڊيگهائي&(UTRIE2_معلومات_GRANULARITY-1))!=0) {
        newTrie->معلومات[newTrie->معلومات_ڊيگهائي++]=trie->initial_قدر;
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
#وصف_ڪريو UTRIE2_MAX_اشاريو_ڊيگهائي 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#وصف_ڪريو UTRIE2_MAX_معلومات_ڊيگهائي (0xffff<<UTRIE2_اشاريو_سرڪايو)

/* Compact and internally serialize the trie. */
U_CAPI فراغ U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_قدر_بٽ قدر_بٽ, UError_ڪوڊ *p_Error_ڪوڊ) {
    UNewTrie2 *newTrie;
    UTrie2_سرو *سرو;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_ڊيگهائي;
    int32_t معلومات_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 مٿاهون_شروعات;

    /* argument check */
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    إذا( trie==خالي_اشارو ||
        قدر_بٽ<0 || UTRIE2_ڳڻپ_قدر_بٽ<=قدر_بٽ
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==خالي_اشارو) {
        /* already frozen */
        UTrie2_قدر_بٽ frozen_قدر_بٽ=
            trie->data16!=خالي_اشارو ? UTRIE2_16_قدر_بٽ : UTRIE2_32_قدر_بٽ;
        إذا(قدر_بٽ!=frozen_قدر_بٽ) {
            *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        }
        أرجع;
    }

    /* compact if necessary */
    إذا(!newTrie->isCompacted) {
        compactTrie(trie, p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع;
        }
    }
    مٿاهون_شروعات=trie->مٿاهون_شروعات;

    إذا(مٿاهون_شروعات<=0x10000) {
        all_Indexes_ڊيگهائي=UTRIE2_اشاريو_1_وٿي;
    } وإلا {
        all_Indexes_ڊيگهائي=newTrie->index2_ڊيگهائي;
    }
    إذا(قدر_بٽ==UTRIE2_16_قدر_بٽ) {
        معلومات_Move=all_Indexes_ڊيگهائي;
    } وإلا {
        معلومات_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    إذا( /* for unshifted indexLength */
        all_Indexes_ڊيگهائي>UTRIE2_MAX_اشاريو_ڊيگهائي ||
        /* for unshifted dataNullOffset */
        (معلومات_Move+newTrie->معلومات_Null_وٿي)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (معلومات_Move+UNEWTRIE2_معلومات_0800_وٿي)>0xffff ||
        /* for shiftedDataLength */
        (معلومات_Move+newTrie->معلومات_ڊيگهائي)>UTRIE2_MAX_معلومات_ڊيگهائي
    ) {
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    /* calculate the total serialized length */
    length=حجم(UTrie2_سرو)+all_Indexes_ڊيگهائي*2;
    إذا(قدر_بٽ==UTRIE2_16_قدر_بٽ) {
        length+=newTrie->معلومات_ڊيگهائي*2;
    } وإلا {
        length+=newTrie->معلومات_ڊيگهائي*4;
    }

    trie->يادگيري=uprv_malloc(length);
    إذا(trie->يادگيري==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    trie->length=length;
    trie->is_يادگيري_Owned=سچ;

    trie->اشاريو_ڊيگهائي=all_Indexes_ڊيگهائي;
    trie->معلومات_ڊيگهائي=newTrie->معلومات_ڊيگهائي;
    إذا(مٿاهون_شروعات<=0x10000) {
        trie->index2_Null_وٿي=0xffff;
    } وإلا {
        trie->index2_Null_وٿي=static_cast<uint16_t>(UTRIE2_اشاريو_2_وٿي+newTrie->index2_Null_وٿي);
    }
    trie->معلومات_Null_وٿي=(uint16_t)(معلومات_Move+newTrie->معلومات_Null_وٿي);
    trie->مٿاهون_قدر_اشاريو=معلومات_Move+trie->معلومات_ڊيگهائي-UTRIE2_معلومات_GRANULARITY;

    /* set the header fields */
    سرو=(UTrie2_سرو *)trie->يادگيري;

    سرو->signature=UTRIE2_SIG; /* "Tri2" */
    سرو->options=(uint16_t)قدر_بٽ;

    سرو->اشاريو_ڊيگهائي=(uint16_t)trie->اشاريو_ڊيگهائي;
    سرو->shifted_معلومات_ڊيگهائي=(uint16_t)(trie->معلومات_ڊيگهائي>>UTRIE2_اشاريو_سرڪايو);
    سرو->index2_Null_وٿي=trie->index2_Null_وٿي;
    سرو->معلومات_Null_وٿي=trie->معلومات_Null_وٿي;
    سرو->shifted_مٿاهون_شروعات=(uint16_t)(مٿاهون_شروعات>>UTRIE2_سرڪايو_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرو+1);
    trie->اشاريو=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    لكل(i=UTRIE2_اشاريو_2_BMP_ڊيگهائي; i>0; --i) {
        *dest16++=(uint16_t)((معلومات_Move + *p++)>>UTRIE2_اشاريو_سرڪايو);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(معلومات_Move+UTRIE2_BAD_UTF8_معلومات_وٿي);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(معلومات_Move+newTrie->index2[i<<(6-UTRIE2_سرڪايو_2)]);
    }

    إذا(مٿاهون_شروعات>0x10000) {
        int32_t index1_ڊيگهائي=(مٿاهون_شروعات-0x10000)>>UTRIE2_سرڪايو_1;
        int32_t index2_وٿي=UTRIE2_اشاريو_2_BMP_ڊيگهائي+UTRIE2_UTF8_2_B_اشاريو_2_ڊيگهائي+index1_ڊيگهائي;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_اشاريو_1_ڊيگهائي;
        لكل(i=index1_ڊيگهائي; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_اشاريو_2_وٿي + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2_وٿي;
        لكل(i=newTrie->index2_ڊيگهائي-index2_وٿي; i>0; --i) {
            *dest16++=(uint16_t)((معلومات_Move + *p++)>>UTRIE2_اشاريو_سرڪايو);
        }
    }

    /* write the 16/32-bit data array */
    بدل(قدر_بٽ) {
    حالة UTRIE2_16_قدر_بٽ:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=خالي_اشارو;
        p=newTrie->معلومات;
        لكل(i=newTrie->معلومات_ڊيگهائي; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        اكسر;
    حالة UTRIE2_32_قدر_بٽ:
        /* write 32-bit data values */
        trie->data16=خالي_اشارو;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->معلومات, (size_t)newTrie->معلومات_ڊيگهائي*4);
        اكسر;
    افتراضي:
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
    umutablecptrie_سيٽ_Name(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_قدر_ويڪر)قدر_بٽ, p_Error_ڪوڊ));
#جيڪڏهن_پورو
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->معلومات);
    uprv_free(newTrie);
    trie->newTrie=خالي_اشارو;
}

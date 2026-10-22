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
#اگر_تعریف_شدہ UTRIE2_DEBUG
#   include <stdio.h>
#اگر_ختم
// #define UCPTRIE_DEBUG

#شامل "unicode/utypes.h"
#اگر_تعریف_شدہ UCPTRIE_DEBUG
#شامل "unicode/ucptrie.h"
#شامل "unicode/umutablecptrie.h"
#شامل "ucptrie_impl.h"
#اگر_ختم
#شامل "cmemory.h"
#شامل "utrie2.h"
#شامل "utrie2_impl.h"

#شامل "utrie.h"  // for utrie2_fromUTrie()

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

شمار {
    /** The null index-2 block, following the gap in the index-2 table. */
    UNEWTRIE2_اشاریہ_2_NULL_انحراف=UNEWTRIE2_اشاریہ_GAP_انحراف+UNEWTRIE2_اشاریہ_GAP_لمبائی,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_اشاریہ_2_آغاز_انحراف=UNEWTRIE2_اشاریہ_2_NULL_انحراف+UTRIE2_اشاریہ_2_بلاک_لمبائی,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_ڈیٹا_NULL_انحراف=UTRIE2_ڈیٹا_آغاز_انحراف,

    /** The start of allocated data blocks. */
    UNEWTRIE2_ڈیٹا_آغاز_انحراف=UNEWTRIE2_ڈیٹا_NULL_انحراف+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_ڈیٹا_0800_انحراف=UNEWTRIE2_ڈیٹا_آغاز_انحراف+0x780
};

/* Start with allocation of 16k data entries. */
#تعریف_کرو UNEWTRIE2_INITIAL_ڈیٹا_لمبائی ((int32_t)1<<14)

/* Grow about 8x each time. */
#تعریف_کرو UNEWTRIE2_MEDIUM_ڈیٹا_لمبائی ((int32_t)1<<17)

ساکن int32_t
alloc_Index2_بلاک(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_قدر, uint32_t error_قدر, UError_رمز *p_Error_رمز) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *ڈیٹا;
    int32_t i, j;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(حجم(UNewTrie2));
    ڈیٹا=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_ڈیٹا_لمبائی*4);
    اگر(trie==خالی_اشاریہ || newTrie==خالی_اشاریہ || ڈیٹا==خالی_اشاریہ) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(ڈیٹا);
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }

    uprv_memset(trie, 0, حجم(UTrie2));
    trie->initial_قدر=initial_قدر;
    trie->error_قدر=error_قدر;
    trie->بلند_آغاز=0x110000;
    trie->newTrie=newTrie;
#اگر_تعریف_شدہ UTRIE2_DEBUG
    trie->name="open";
#اگر_ختم

    newTrie->ڈیٹا=ڈیٹا;
#اگر_تعریف_شدہ UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_قدر, error_قدر, p_Error_رمز);
#اگر_ختم
    newTrie->ڈیٹا_استعداد=UNEWTRIE2_INITIAL_ڈیٹا_لمبائی;
    newTrie->initial_قدر=initial_قدر;
    newTrie->error_قدر=error_قدر;
    newTrie->بلند_آغاز=0x110000;
    newTrie->پہلا_Free_بلاک=0;  /* no free block in the list */
    newTrie->isCompacted=جھوٹ;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    برائے(i=0; i<0x80; ++i) {
        newTrie->ڈیٹا[i]=initial_قدر;
    }
    برائے(; i<0xc0; ++i) {
        newTrie->ڈیٹا[i]=error_قدر;
    }
    برائے(i=UNEWTRIE2_ڈیٹا_NULL_انحراف; i<UNEWTRIE2_ڈیٹا_آغاز_انحراف; ++i) {
        newTrie->ڈیٹا[i]=initial_قدر;
    }
    newTrie->ڈیٹا_Null_انحراف=UNEWTRIE2_ڈیٹا_NULL_انحراف;
    newTrie->ڈیٹا_لمبائی=UNEWTRIE2_ڈیٹا_آغاز_انحراف;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    برائے(i=0, j=0; j<0x80; ++i, j+=UTRIE2_ڈیٹا_بلاک_لمبائی) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    برائے(; j<0xc0; ++i, j+=UTRIE2_ڈیٹا_بلاک_لمبائی) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_سرکاؤ_2)-
        (0x80>>UTRIE2_سرکاؤ_2)+
        1+
        UTRIE2_LSCP_اشاریہ_2_لمبائی;
    j+=UTRIE2_ڈیٹا_بلاک_لمبائی;
    برائے(; j<UNEWTRIE2_ڈیٹا_آغاز_انحراف; ++i, j+=UTRIE2_ڈیٹا_بلاک_لمبائی) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    برائے(i=0x80>>UTRIE2_سرکاؤ_2; i<UTRIE2_اشاریہ_2_BMP_لمبائی; ++i) {
        newTrie->index2[i]=UNEWTRIE2_ڈیٹا_NULL_انحراف;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    برائے(i=0; i<UNEWTRIE2_اشاریہ_GAP_لمبائی; ++i) {
        newTrie->index2[UNEWTRIE2_اشاریہ_GAP_انحراف+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    برائے(i=0; i<UTRIE2_اشاریہ_2_بلاک_لمبائی; ++i) {
        newTrie->index2[UNEWTRIE2_اشاریہ_2_NULL_انحراف+i]=UNEWTRIE2_ڈیٹا_NULL_انحراف;
    }
    newTrie->index2_Null_انحراف=UNEWTRIE2_اشاریہ_2_NULL_انحراف;
    newTrie->index2_لمبائی=UNEWTRIE2_اشاریہ_2_آغاز_انحراف;

    /* set the index-1 indexes for the linear index-2 block */
    برائے(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_اشاریہ_1_لمبائی;
        ++i, j+=UTRIE2_اشاریہ_2_بلاک_لمبائی
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    برائے(; i<UNEWTRIE2_اشاریہ_1_لمبائی; ++i) {
        newTrie->index1[i]=UNEWTRIE2_اشاریہ_2_NULL_انحراف;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    برائے(i=0x80; i<0x800; i+=UTRIE2_ڈیٹا_بلاک_لمبائی) {
        utrie2_set32(trie, i, initial_قدر, p_Error_رمز);
    }

    واپس trie;
}

ساکن UNewTrie2 *
cloneBuilder(ثابت UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(حجم(UNewTrie2)));
    اگر(trie==خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }

    trie->ڈیٹا = static_cast<uint32_t*>(uprv_malloc(other->ڈیٹا_استعداد * 4));
    اگر(trie->ڈیٹا==خالی_اشاریہ) {
        uprv_free(trie);
        واپس خالی_اشاریہ;
    }
#اگر_تعریف_شدہ UCPTRIE_DEBUG
    اگر(other->t3==خالی_اشاریہ) {
        trie->t3=خالی_اشاریہ;
    } ورنہ {
        UError_رمز error_رمز=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &error_رمز);
    }
#اگر_ختم
    trie->ڈیٹا_استعداد=other->ڈیٹا_استعداد;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, حجم(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_لمبائی*4);
    trie->index2_Null_انحراف=other->index2_Null_انحراف;
    trie->index2_لمبائی=other->index2_لمبائی;

    uprv_memcpy(trie->ڈیٹا, other->ڈیٹا, (size_t)other->ڈیٹا_لمبائی*4);
    trie->ڈیٹا_Null_انحراف=other->ڈیٹا_Null_انحراف;
    trie->ڈیٹا_لمبائی=other->ڈیٹا_لمبائی;

    /* reference counters */
    اگر(other->isCompacted) {
        trie->پہلا_Free_بلاک=0;
    } ورنہ {
        uprv_memcpy(trie->map, other->map, ((size_t)other->ڈیٹا_لمبائی>>UTRIE2_سرکاؤ_2)*4);
        trie->پہلا_Free_بلاک=other->پہلا_Free_بلاک;
    }

    trie->initial_قدر=other->initial_قدر;
    trie->error_قدر=other->error_قدر;
    trie->بلند_آغاز=other->بلند_آغاز;
    trie->isCompacted=other->isCompacted;

    واپس trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ثابت UTrie2 *other, UError_رمز *p_Error_رمز) {
    UTrie2 *trie;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر(other==خالی_اشاریہ || (other->حافظہ==خالی_اشاریہ && other->newTrie==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    اگر(trie==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memcpy(trie, other, حجم(UTrie2));

    اگر(other->حافظہ!=خالی_اشاریہ) {
        trie->حافظہ=uprv_malloc(other->length);
        اگر(trie->حافظہ!=خالی_اشاریہ) {
            trie->is_حافظہ_Owned=سچ;
            uprv_memcpy(trie->حافظہ, other->حافظہ, other->length);

            /* make the clone's pointers point to its own memory */
            trie->اشاریہ=(uint16_t *)trie->حافظہ+(other->اشاریہ-(uint16_t *)other->حافظہ);
            اگر(other->data16!=خالی_اشاریہ) {
                trie->data16=(uint16_t *)trie->حافظہ+(other->data16-(uint16_t *)other->حافظہ);
            }
            اگر(other->data32!=خالی_اشاریہ) {
                trie->data32=(uint32_t *)trie->حافظہ+(other->data32-(uint32_t *)other->حافظہ);
            }
        }
    } ورنہ /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    اگر(trie->حافظہ==خالی_اشاریہ && trie->newTrie==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        uprv_free(trie);
        trie=خالی_اشاریہ;
    }
    واپس trie;
}

قسمتعریف ساخت New_Trie_And_حالت {
    UTrie2 *trie;
    UError_رمز error_رمز;
    UBool exclusive_حد_بندی;  /* rather than inclusive range end */
} New_Trie_And_حالت;

ساکن UBool U_CALLCONV
copyEnumRange(ثابت خالی *context, UChar32 آغاز, UChar32 انتہا, uint32_t value) {
    New_Trie_And_حالت *nt=(New_Trie_And_حالت *)context;
    اگر(value!=nt->trie->initial_قدر) {
        اگر(nt->exclusive_حد_بندی) {
            --انتہا;
        }
        اگر(آغاز==انتہا) {
            utrie2_set32(nt->trie, آغاز, value, &nt->error_رمز);
        } ورنہ {
            utrie2_سیٹ_Range32(nt->trie, آغاز, انتہا, value, سچ, &nt->error_رمز);
        }
        واپس U_SUCCESS(nt->error_رمز);
    } ورنہ {
        واپس سچ;
    }
}

#اگر_تعریف_شدہ UTRIE2_DEBUG
ساکن طویل گنتی_Initial(ثابت UTrie2 *trie) {
    uint32_t initial_قدر=trie->initial_قدر;
    int32_t length=trie->ڈیٹا_لمبائی;
    طویل count=0;
    اگر(trie->data16!=خالی_اشاریہ) {
        برائے(int32_t i=0; i<length; ++i) {
            اگر(trie->data16[i]==initial_قدر) { ++count; }
        }
    } ورنہ {
        برائے(int32_t i=0; i<length; ++i) {
            اگر(trie->data32[i]==initial_قدر) { ++count; }
        }
    }
    واپس count;
}

ساکن خالی
utrie_printLengths(ثابت UTrie *trie) {
    طویل اشاریہ_لمبائی=trie->اشاریہ_لمبائی;
    طویل ڈیٹا_لمبائی=(طویل)trie->ڈیٹا_لمبائی;
    طویل کل_لمبائی=(طویل)حجم(UTrie_سرخی)+اشاریہ_لمبائی*2+ڈیٹا_لمبائی*(trie->data32!=خالی_اشاریہ ? 4 : 2);
    لکھو("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           اشاریہ_لمبائی, ڈیٹا_لمبائی, کل_لمبائی);
}

ساکن خالی
utrie2_printLengths(ثابت UTrie2 *trie, ثابت حرف *which) {
    طویل اشاریہ_لمبائی=trie->اشاریہ_لمبائی;
    طویل ڈیٹا_لمبائی=(طویل)trie->ڈیٹا_لمبائی;
    طویل کل_لمبائی=(طویل)حجم(UTrie2_سرخی)+اشاریہ_لمبائی*2+ڈیٹا_لمبائی*(trie->data32!=خالی_اشاریہ ? 4 : 2);
    لکھو("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, اشاریہ_لمبائی, ڈیٹا_لمبائی, گنتی_Initial(trie), کل_لمبائی);
}
#اگر_ختم

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ثابت UTrie2 *other, UError_رمز *p_Error_رمز) {
    New_Trie_And_حالت context;
    char16_t lead;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر(other==خالی_اشاریہ || (other->حافظہ==خالی_اشاریہ && other->newTrie==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    اگر(other->newTrie!=خالی_اشاریہ && !other->newTrie->isCompacted) {
        واپس utrie2_clone(other, p_Error_رمز);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_قدر, other->error_قدر, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    context.exclusive_حد_بندی=جھوٹ;
    context.error_رمز=*p_Error_رمز;
    utrie2_enum(other, خالی_اشاریہ, copyEnumRange, &context);
    *p_Error_رمز=context.error_رمز;
    برائے(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        اگر(other->data32==خالی_اشاریہ) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } ورنہ {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        اگر(value!=other->initial_قدر) {
            utrie2_set32_For_Lead_Surrogate_رمز_Unit(context.trie, lead, value, p_Error_رمز);
        }
    }
    اگر(U_FAILURE(*p_Error_رمز)) {
        utrie2_close(context.trie);
        context.trie=خالی_اشاریہ;
    }
    واپس context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ثابت UTrie *trie1, uint32_t error_قدر, UError_رمز *p_Error_رمز) {
    New_Trie_And_حالت context;
    char16_t lead;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر(trie1==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    context.trie=utrie2_open(trie1->initial_قدر, error_قدر, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }
    context.exclusive_حد_بندی=سچ;
    context.error_رمز=*p_Error_رمز;
    utrie_enum(trie1, خالی_اشاریہ, copyEnumRange, &context);
    *p_Error_رمز=context.error_رمز;
    برائے(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        اگر(trie1->data32==خالی_اشاریہ) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } ورنہ {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        اگر(value!=trie1->initial_قدر) {
            utrie2_set32_For_Lead_Surrogate_رمز_Unit(context.trie, lead, value, p_Error_رمز);
        }
    }
    اگر(U_SUCCESS(*p_Error_رمز)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=خالی_اشاریہ ? UTRIE2_32_قدر_بٹ : UTRIE2_16_قدر_بٹ,
                      p_Error_رمز);
    }
#اگر_تعریف_شدہ UTRIE2_DEBUG
    اگر(U_SUCCESS(*p_Error_رمز)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#اگر_ختم
    اگر(U_FAILURE(*p_Error_رمز)) {
        utrie2_close(context.trie);
        context.trie=خالی_اشاریہ;
    }
    واپس context.trie;
}

ساکن اندرونی UBool
is_اندر_Null_بلاک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, بلاک;

    اگر(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_اشاریہ_2_انحراف-(0xd800>>UTRIE2_سرکاؤ_2))+
            (c>>UTRIE2_سرکاؤ_2);
    } ورنہ {
        i2=trie->index1[c>>UTRIE2_سرکاؤ_1]+
            ((c>>UTRIE2_سرکاؤ_2)&UTRIE2_اشاریہ_2_نقاب);
    }
    بلاک=trie->index2[i2];
    واپس بلاک == trie->ڈیٹا_Null_انحراف;
}

ساکن int32_t
alloc_Index2_بلاک(UNewTrie2 *trie) {
    int32_t new_بلاک, new_چوٹی;

    new_بلاک=trie->index2_لمبائی;
    new_چوٹی=new_بلاک+UTRIE2_اشاریہ_2_بلاک_لمبائی;
    اگر(new_چوٹی>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        واپس -1;
    }
    trie->index2_لمبائی=new_چوٹی;
    uprv_memcpy(trie->index2+new_بلاک, trie->index2+trie->index2_Null_انحراف, UTRIE2_اشاریہ_2_بلاک_لمبائی*4);
    واپس new_بلاک;
}

ساکن int32_t
get_Index2_بلاک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    اگر(U_IS_LEAD(c) && forLSCP) {
        واپس UTRIE2_LSCP_اشاریہ_2_انحراف;
    }

    i1=c>>UTRIE2_سرکاؤ_1;
    i2=trie->index1[i1];
    اگر(i2==trie->index2_Null_انحراف) {
        i2=alloc_Index2_بلاک(trie);
        اگر(i2<0) {
            واپس -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    واپس i2;
}

ساکن int32_t
alloc_ڈیٹا_بلاک(UNewTrie2 *trie, int32_t copy_بلاک) {
    int32_t new_بلاک, new_چوٹی;

    اگر(trie->پہلا_Free_بلاک!=0) {
        /* get the first free block */
        new_بلاک=trie->پہلا_Free_بلاک;
        trie->پہلا_Free_بلاک=-trie->map[new_بلاک>>UTRIE2_سرکاؤ_2];
    } ورنہ {
        /* get a new block from the high end */
        new_بلاک=trie->ڈیٹا_لمبائی;
        new_چوٹی=new_بلاک+UTRIE2_ڈیٹا_بلاک_لمبائی;
        اگر(new_چوٹی>trie->ڈیٹا_استعداد) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *ڈیٹا;

            اگر(trie->ڈیٹا_استعداد<UNEWTRIE2_MEDIUM_ڈیٹا_لمبائی) {
                capacity=UNEWTRIE2_MEDIUM_ڈیٹا_لمبائی;
            } ورنہ اگر(trie->ڈیٹا_استعداد<UNEWTRIE2_MAX_ڈیٹا_لمبائی) {
                capacity=UNEWTRIE2_MAX_ڈیٹا_لمبائی;
            } ورنہ {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                واپس -1;
            }
            ڈیٹا = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            اگر(ڈیٹا==خالی_اشاریہ) {
                واپس -1;
            }
            uprv_memcpy(ڈیٹا, trie->ڈیٹا, (size_t)trie->ڈیٹا_لمبائی*4);
            uprv_free(trie->ڈیٹا);
            trie->ڈیٹا=ڈیٹا;
            trie->ڈیٹا_استعداد=capacity;
        }
        trie->ڈیٹا_لمبائی=new_چوٹی;
    }
    uprv_memcpy(trie->ڈیٹا+new_بلاک, trie->ڈیٹا+copy_بلاک, UTRIE2_ڈیٹا_بلاک_لمبائی*4);
    trie->map[new_بلاک>>UTRIE2_سرکاؤ_2]=0;
    واپس new_بلاک;
}

/* call when the block's reference counter reaches 0 */
ساکن خالی
release_ڈیٹا_بلاک(UNewTrie2 *trie, int32_t بلاک) {
    /* put this block at the front of the free-block chain */
    trie->map[بلاک>>UTRIE2_سرکاؤ_2]=-trie->پہلا_Free_بلاک;
    trie->پہلا_Free_بلاک=بلاک;
}

ساکن اندرونی UBool
is_Writable_بلاک(UNewTrie2 *trie, int32_t بلاک) {
    واپس بلاک != trie->ڈیٹا_Null_انحراف && 1 == trie->map[بلاک >> UTRIE2_سرکاؤ_2];
}

ساکن اندرونی خالی
سیٹ_Index2_اندراج(UNewTrie2 *trie, int32_t i2, int32_t بلاک) {
    int32_t old_بلاک;
    ++trie->map[بلاک>>UTRIE2_سرکاؤ_2];  /* increment first, in case block==oldBlock! */
    old_بلاک=trie->index2[i2];
    اگر(0 == --trie->map[old_بلاک>>UTRIE2_سرکاؤ_2]) {
        release_ڈیٹا_بلاک(trie, old_بلاک);
    }
    trie->index2[i2]=بلاک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ساکن int32_t
get_ڈیٹا_بلاک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, old_بلاک, new_بلاک;

    i2=get_Index2_بلاک(trie, c, forLSCP);
    اگر(i2<0) {
        واپس -1;  /* program error */
    }

    i2+=(c>>UTRIE2_سرکاؤ_2)&UTRIE2_اشاریہ_2_نقاب;
    old_بلاک=trie->index2[i2];
    اگر(is_Writable_بلاک(trie, old_بلاک)) {
        واپس old_بلاک;
    }

    /* allocate a new data block */
    new_بلاک=alloc_ڈیٹا_بلاک(trie, old_بلاک);
    اگر(new_بلاک<0) {
        /* out of memory in the data array */
        واپس -1;
    }
    سیٹ_Index2_اندراج(trie, i2, new_بلاک);
    واپس new_بلاک;
}

/**
 * @return true if the value was successfully set
 */
ساکن خالی
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UError_رمز *p_Error_رمز) {
    int32_t بلاک;

    اگر(trie==خالی_اشاریہ || trie->isCompacted) {
        *p_Error_رمز=U_NO_WRITE_PERMISSION;
        واپس;
    }
#اگر_تعریف_شدہ UCPTRIE_DEBUG
    umutablecptrie_سیٹ(trie->t3, c, value, p_Error_رمز);
#اگر_ختم

    بلاک=get_ڈیٹا_بلاک(trie, c, forLSCP);
    اگر(بلاک<0) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    trie->ڈیٹا[بلاک+(c&UTRIE2_ڈیٹا_نقاب)]=value;
}

U_CAPI خالی U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    اگر((uint32_t)c>0x10ffff) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    set32(trie->newTrie, c, سچ, value, p_Error_رمز);
}

U_CAPI خالی U_EXPORT2
utrie2_set32_For_Lead_Surrogate_رمز_Unit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    اگر(!U_IS_LEAD(c)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    set32(trie->newTrie, c, جھوٹ, value, p_Error_رمز);
}

ساکن خالی
write_بلاک(uint32_t *بلاک, uint32_t value) {
    uint32_t *حد_بندی=بلاک+UTRIE2_ڈیٹا_بلاک_لمبائی;
    جب(بلاک<حد_بندی) {
        *بلاک++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
ساکن خالی
fill_بلاک(uint32_t *بلاک, UChar32 آغاز, UChar32 حد_بندی,
          uint32_t value, uint32_t initial_قدر, UBool overwrite) {
    uint32_t *p_حد_بندی;

    p_حد_بندی=بلاک+حد_بندی;
    بلاک+=آغاز;
    اگر(overwrite) {
        جب(بلاک<p_حد_بندی) {
            *بلاک++=value;
        }
    } ورنہ {
        جب(بلاک<p_حد_بندی) {
            اگر(*بلاک==initial_قدر) {
                *بلاک=value;
            }
            ++بلاک;
        }
    }
}

U_CAPI خالی U_EXPORT2
utrie2_سیٹ_Range32(UTrie2 *trie,
                  UChar32 آغاز, UChar32 انتہا,
                  uint32_t value, UBool overwrite,
                  UError_رمز *p_Error_رمز) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t بلاک, rest, repeat_بلاک;
    UChar32 حد_بندی;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    اگر((uint32_t)آغاز>0x10ffff || (uint32_t)انتہا>0x10ffff || آغاز>انتہا) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    newTrie=trie->newTrie;
    اگر(newTrie==خالی_اشاریہ || newTrie->isCompacted) {
        *p_Error_رمز=U_NO_WRITE_PERMISSION;
        واپس;
    }
#اگر_تعریف_شدہ UCPTRIE_DEBUG
    umutablecptrie_سیٹ_Range(newTrie->t3, آغاز, انتہا, value, p_Error_رمز);
#اگر_ختم
    اگر(!overwrite && value==newTrie->initial_قدر) {
        واپس; /* nothing to do */
    }

    حد_بندی=انتہا+1;
    اگر(آغاز&UTRIE2_ڈیٹا_نقاب) {
        UChar32 اگلا_آغاز;

        /* set partial block at [start..following block boundary[ */
        بلاک=get_ڈیٹا_بلاک(newTrie, آغاز, سچ);
        اگر(بلاک<0) {
            *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }

        اگلا_آغاز=(آغاز+UTRIE2_ڈیٹا_نقاب)&~UTRIE2_ڈیٹا_نقاب;
        اگر(اگلا_آغاز<=حد_بندی) {
            fill_بلاک(newTrie->ڈیٹا+بلاک, آغاز&UTRIE2_ڈیٹا_نقاب, UTRIE2_ڈیٹا_بلاک_لمبائی,
                      value, newTrie->initial_قدر, overwrite);
            آغاز=اگلا_آغاز;
        } ورنہ {
            fill_بلاک(newTrie->ڈیٹا+بلاک, آغاز&UTRIE2_ڈیٹا_نقاب, حد_بندی&UTRIE2_ڈیٹا_نقاب,
                      value, newTrie->initial_قدر, overwrite);
            واپس;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد_بندی&UTRIE2_ڈیٹا_نقاب;

    /* round down limit to a block boundary */
    حد_بندی&=~UTRIE2_ڈیٹا_نقاب;

    /* iterate over all-value blocks */
    اگر(value==newTrie->initial_قدر) {
        repeat_بلاک=newTrie->ڈیٹا_Null_انحراف;
    } ورنہ {
        repeat_بلاک=-1;
    }

    جب(آغاز<حد_بندی) {
        int32_t i2;
        UBool سیٹ_Repeat_بلاک=جھوٹ;

        اگر(value==newTrie->initial_قدر && is_اندر_Null_بلاک(newTrie, آغاز, سچ)) {
            آغاز+=UTRIE2_ڈیٹا_بلاک_لمبائی; /* nothing to do */
            جاری;
        }

        /* get index value */
        i2=get_Index2_بلاک(newTrie, آغاز, سچ);
        اگر(i2<0) {
            *p_Error_رمز=U_اندرونی_پروگرام_ERROR;
            واپس;
        }
        i2+=(آغاز>>UTRIE2_سرکاؤ_2)&UTRIE2_اشاریہ_2_نقاب;
        بلاک=newTrie->index2[i2];
        اگر(is_Writable_بلاک(newTrie, بلاک)) {
            /* already allocated */
            اگر(overwrite && بلاک>=UNEWTRIE2_ڈیٹا_0800_انحراف) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                سیٹ_Repeat_بلاک=سچ;
            } ورنہ {
                /* !overwrite, or protected block: just write the values into this block */
                fill_بلاک(newTrie->ڈیٹا+بلاک,
                          0, UTRIE2_ڈیٹا_بلاک_لمبائی,
                          value, newTrie->initial_قدر, overwrite);
            }
        } ورنہ اگر(newTrie->ڈیٹا[بلاک]!=value && (overwrite || بلاک==newTrie->ڈیٹا_Null_انحراف)) {
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
        اگر(سیٹ_Repeat_بلاک) {
            اگر(repeat_بلاک>=0) {
                سیٹ_Index2_اندراج(newTrie, i2, repeat_بلاک);
            } ورنہ {
                /* create and set and fill the repeatBlock */
                repeat_بلاک=get_ڈیٹا_بلاک(newTrie, آغاز, سچ);
                اگر(repeat_بلاک<0) {
                    *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
                    واپس;
                }
                write_بلاک(newTrie->ڈیٹا+repeat_بلاک, value);
            }
        }

        آغاز+=UTRIE2_ڈیٹا_بلاک_لمبائی;
    }

    اگر(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلاک=get_ڈیٹا_بلاک(newTrie, آغاز, سچ);
        اگر(بلاک<0) {
            *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }

        fill_بلاک(newTrie->ڈیٹا+بلاک, 0, rest, value, newTrie->initial_قدر, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

ساکن اندرونی UBool
equal_int32(ثابت int32_t *s, ثابت int32_t *t, int32_t length) {
    جب(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    واپس length == 0;
}

ساکن اندرونی UBool
equal_uint32(ثابت uint32_t *s, ثابت uint32_t *t, int32_t length) {
    جب(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    واپس length == 0;
}

ساکن int32_t
ڈھونڈو_Same_Index2_بلاک(ثابت int32_t *idx, int32_t index2_لمبائی, int32_t other_بلاک) {
    int32_t بلاک;

    /* ensure that we do not even partially get past index2Length */
    index2_لمبائی-=UTRIE2_اشاریہ_2_بلاک_لمبائی;

    برائے(بلاک=0; بلاک<=index2_لمبائی; ++بلاک) {
        اگر(equal_int32(idx+بلاک, idx+other_بلاک, UTRIE2_اشاریہ_2_بلاک_لمبائی)) {
            واپس بلاک;
        }
    }
    واپس -1;
}

ساکن int32_t
ڈھونڈو_Same_ڈیٹا_بلاک_2(ثابت uint32_t *ڈیٹا, int32_t ڈیٹا_لمبائی, int32_t other_بلاک, int32_t بلاک_لمبائی) {
    int32_t بلاک;

    /* ensure that we do not even partially get past dataLength */
    ڈیٹا_لمبائی-=بلاک_لمبائی;

    برائے(بلاک=0; بلاک<=ڈیٹا_لمبائی; بلاک+=UTRIE2_ڈیٹا_GRANULARITY) {
        اگر(equal_uint32(ڈیٹا+بلاک, ڈیٹا+other_بلاک, بلاک_لمبائی)) {
            واپس بلاک;
        }
    }
    واپس -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
ساکن UChar32
ڈھونڈو_بلند_آغاز(UNewTrie2 *trie, uint32_t بلند_قدر) {
    ثابت uint32_t *data32;

    uint32_t value, initial_قدر;
    UChar32 c, پچھلا;
    int32_t i1, i2, j, i2_بلاک, پچھلا_I2_بلاک, index2_Null_انحراف, بلاک, پچھلا_بلاک, null_بلاک;

    data32=trie->ڈیٹا;
    initial_قدر=trie->initial_قدر;

    index2_Null_انحراف=trie->index2_Null_انحراف;
    null_بلاک=trie->ڈیٹا_Null_انحراف;

    /* set variables for previous range */
    اگر(بلند_قدر==initial_قدر) {
        پچھلا_I2_بلاک=index2_Null_انحراف;
        پچھلا_بلاک=null_بلاک;
    } ورنہ {
        پچھلا_I2_بلاک=-1;
        پچھلا_بلاک=-1;
    }
    پچھلا=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_اشاریہ_1_لمبائی;
    c=پچھلا;
    جب(c>0) {
        i2_بلاک=trie->index1[--i1];
        اگر(i2_بلاک==پچھلا_I2_بلاک) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_فی_اشاریہ_1_اندراج;
            جاری;
        }
        پچھلا_I2_بلاک=i2_بلاک;
        اگر(i2_بلاک==index2_Null_انحراف) {
            /* this is the null index-2 block */
            اگر(بلند_قدر!=initial_قدر) {
                واپس c;
            }
            c-=UTRIE2_CP_فی_اشاریہ_1_اندراج;
        } ورنہ {
            /* enumerate data blocks for one index-2 block */
            برائے(i2=UTRIE2_اشاریہ_2_بلاک_لمبائی; i2>0;) {
                بلاک=trie->index2[i2_بلاک+ --i2];
                اگر(بلاک==پچھلا_بلاک) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_ڈیٹا_بلاک_لمبائی;
                    جاری;
                }
                پچھلا_بلاک=بلاک;
                اگر(بلاک==null_بلاک) {
                    /* this is the null data block */
                    اگر(بلند_قدر!=initial_قدر) {
                        واپس c;
                    }
                    c-=UTRIE2_ڈیٹا_بلاک_لمبائی;
                } ورنہ {
                    برائے(j=UTRIE2_ڈیٹا_بلاک_لمبائی; j>0;) {
                        value=data32[بلاک+ --j];
                        اگر(value!=بلند_قدر) {
                            واپس c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    واپس 0;
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
ساکن خالی
compact_ڈیٹا(UNewTrie2 *trie) {
#اگر_تعریف_شدہ UTRIE2_DEBUG
    int32_t گنتی_Same=0, مجموع_Overlaps=0;
#اگر_ختم

    int32_t آغاز, new_آغاز, moved_آغاز;
    int32_t بلاک_لمبائی, overlap;
    int32_t i, map_اشاریہ, بلاک_گنتی;

    /* do not compact linear-ASCII data */
    new_آغاز=UTRIE2_ڈیٹا_آغاز_انحراف;
    برائے(آغاز=0, i=0; آغاز<new_آغاز; آغاز+=UTRIE2_ڈیٹا_بلاک_لمبائی, ++i) {
        trie->map[i]=آغاز;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    بلاک_لمبائی=64;
    بلاک_گنتی=بلاک_لمبائی>>UTRIE2_سرکاؤ_2;
    برائے(آغاز=new_آغاز; آغاز<trie->ڈیٹا_لمبائی;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        اگر(آغاز==UNEWTRIE2_ڈیٹا_0800_انحراف) {
            بلاک_لمبائی=UTRIE2_ڈیٹا_بلاک_لمبائی;
            بلاک_گنتی=1;
        }

        /* skip blocks that are not used */
        اگر(trie->map[آغاز>>UTRIE2_سرکاؤ_2]<=0) {
            /* advance start to the next block */
            آغاز+=بلاک_لمبائی;

            /* leave newStart with the previous block! */
            جاری;
        }

        /* search for an identical block */
        اگر( (moved_آغاز=ڈھونڈو_Same_ڈیٹا_بلاک_2(trie->ڈیٹا, new_آغاز, آغاز, بلاک_لمبائی))
             >=0
        ) {
#اگر_تعریف_شدہ UTRIE2_DEBUG
            ++گنتی_Same;
#اگر_ختم
            /* found an identical block, set the other block's index value for the current block */
            برائے(i=بلاک_گنتی, map_اشاریہ=آغاز>>UTRIE2_سرکاؤ_2; i>0; --i) {
                trie->map[map_اشاریہ++]=moved_آغاز;
                moved_آغاز+=UTRIE2_ڈیٹا_بلاک_لمبائی;
            }

            /* advance start to the next block */
            آغاز+=بلاک_لمبائی;

            /* leave newStart with the previous block! */
            جاری;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        برائے(overlap=بلاک_لمبائی-UTRIE2_ڈیٹا_GRANULARITY;
            overlap>0 && !equal_uint32(trie->ڈیٹا+(new_آغاز-overlap), trie->ڈیٹا+آغاز, overlap);
            overlap-=UTRIE2_ڈیٹا_GRANULARITY) {}

#اگر_تعریف_شدہ UTRIE2_DEBUG
            مجموع_Overlaps+=overlap;
#اگر_ختم
        اگر(overlap>0 || new_آغاز<آغاز) {
            /* some overlap, or just move the whole block */
            moved_آغاز=new_آغاز-overlap;
            برائے(i=بلاک_گنتی, map_اشاریہ=آغاز>>UTRIE2_سرکاؤ_2; i>0; --i) {
                trie->map[map_اشاریہ++]=moved_آغاز;
                moved_آغاز+=UTRIE2_ڈیٹا_بلاک_لمبائی;
            }

            /* move the non-overlapping indexes to their new positions */
            آغاز+=overlap;
            برائے(i=بلاک_لمبائی-overlap; i>0; --i) {
                trie->ڈیٹا[new_آغاز++]=trie->ڈیٹا[آغاز++];
            }
        } ورنہ /* no overlap && newStart==start */ {
            برائے(i=بلاک_گنتی, map_اشاریہ=آغاز>>UTRIE2_سرکاؤ_2; i>0; --i) {
                trie->map[map_اشاریہ++]=آغاز;
                آغاز+=UTRIE2_ڈیٹا_بلاک_لمبائی;
            }
            new_آغاز=آغاز;
        }
    }

    /* now adjust the index-2 table */
    برائے(i=0; i<trie->index2_لمبائی; ++i) {
        اگر(i==UNEWTRIE2_اشاریہ_GAP_انحراف) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_اشاریہ_GAP_لمبائی;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_سرکاؤ_2];
    }
    trie->ڈیٹا_Null_انحراف=trie->map[trie->ڈیٹا_Null_انحراف>>UTRIE2_سرکاؤ_2];

    /* ensure dataLength alignment */
    جب((new_آغاز&(UTRIE2_ڈیٹا_GRANULARITY-1))!=0) {
        trie->ڈیٹا[new_آغاز++]=trie->initial_قدر;
    }

#اگر_تعریف_شدہ UTRIE2_DEBUG
    /* we saved some space */
    لکھو("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (طویل)trie->ڈیٹا_لمبائی, (طویل)new_آغاز, (طویل)گنتی_Same, (طویل)مجموع_Overlaps);
#اگر_ختم

    trie->ڈیٹا_لمبائی=new_آغاز;
}

ساکن خالی
compactIndex2(UNewTrie2 *trie) {
    int32_t i, آغاز, new_آغاز, moved_آغاز, overlap;

    /* do not compact linear-BMP index-2 blocks */
    new_آغاز=UTRIE2_اشاریہ_2_BMP_لمبائی;
    برائے(آغاز=0, i=0; آغاز<new_آغاز; آغاز+=UTRIE2_اشاریہ_2_بلاک_لمبائی, ++i) {
        trie->map[i]=آغاز;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    new_آغاز+=UTRIE2_UTF8_2_B_اشاریہ_2_لمبائی+((trie->بلند_آغاز-0x10000)>>UTRIE2_سرکاؤ_1);

    برائے(آغاز=UNEWTRIE2_اشاریہ_2_NULL_انحراف; آغاز<trie->index2_لمبائی;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        اگر( (moved_آغاز=ڈھونڈو_Same_Index2_بلاک(trie->index2, new_آغاز, آغاز))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[آغاز>>UTRIE2_سرکاؤ_1_2]=moved_آغاز;

            /* advance start to the next block */
            آغاز+=UTRIE2_اشاریہ_2_بلاک_لمبائی;

            /* leave newStart with the previous block! */
            جاری;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        برائے(overlap=UTRIE2_اشاریہ_2_بلاک_لمبائی-1;
            overlap>0 && !equal_int32(trie->index2+(new_آغاز-overlap), trie->index2+آغاز, overlap);
            --overlap) {}

        اگر(overlap>0 || new_آغاز<آغاز) {
            /* some overlap, or just move the whole block */
            trie->map[آغاز>>UTRIE2_سرکاؤ_1_2]=new_آغاز-overlap;

            /* move the non-overlapping indexes to their new positions */
            آغاز+=overlap;
            برائے(i=UTRIE2_اشاریہ_2_بلاک_لمبائی-overlap; i>0; --i) {
                trie->index2[new_آغاز++]=trie->index2[آغاز++];
            }
        } ورنہ /* no overlap && newStart==start */ {
            trie->map[آغاز>>UTRIE2_سرکاؤ_1_2]=آغاز;
            آغاز+=UTRIE2_اشاریہ_2_بلاک_لمبائی;
            new_آغاز=آغاز;
        }
    }

    /* now adjust the index-1 table */
    برائے(i=0; i<UNEWTRIE2_اشاریہ_1_لمبائی; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_سرکاؤ_1_2];
    }
    trie->index2_Null_انحراف=trie->map[trie->index2_Null_انحراف>>UTRIE2_سرکاؤ_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    جب((new_آغاز&((UTRIE2_ڈیٹا_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[new_آغاز++] = static_cast<int32_t>(0xffff) << UTRIE2_اشاریہ_سرکاؤ;
    }

#اگر_تعریف_شدہ UTRIE2_DEBUG
    /* we saved some space */
    لکھو("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (طویل)trie->index2_لمبائی, (طویل)new_آغاز);
#اگر_ختم

    trie->index2_لمبائی=new_آغاز;
}

ساکن خالی
compactTrie(UTrie2 *trie, UError_رمز *p_Error_رمز) {
    UNewTrie2 *newTrie;
    UChar32 بلند_آغاز, supp_بلند_آغاز;
    uint32_t بلند_قدر;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    بلند_قدر=utrie2_get32(trie, 0x10ffff);
    بلند_آغاز=ڈھونڈو_بلند_آغاز(newTrie, بلند_قدر);
    بلند_آغاز=(بلند_آغاز+(UTRIE2_CP_فی_اشاریہ_1_اندراج-1))&~(UTRIE2_CP_فی_اشاریہ_1_اندراج-1);
    اگر(بلند_آغاز==0x110000) {
        بلند_قدر=trie->error_قدر;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->بلند_آغاز=newTrie->بلند_آغاز=بلند_آغاز;

#اگر_تعریف_شدہ UTRIE2_DEBUG
    لکھو("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (طویل)بلند_آغاز, (طویل)بلند_قدر, (طویل)trie->initial_قدر);
#اگر_ختم

    اگر(بلند_آغاز<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_بلند_آغاز= بلند_آغاز<=0x10000 ? 0x10000 : بلند_آغاز;
        utrie2_سیٹ_Range32(trie, supp_بلند_آغاز, 0x10ffff, trie->initial_قدر, سچ, p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            واپس;
        }
    }

    compact_ڈیٹا(newTrie);
    اگر(بلند_آغاز>0x10000) {
        compactIndex2(newTrie);
#اگر_تعریف_شدہ UTRIE2_DEBUG
    } ورنہ {
        لکھو("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (طویل)بلند_آغاز, (طویل)trie->newTrie->index2_لمبائی, (طویل)UTRIE2_اشاریہ_1_انحراف);
#اگر_ختم
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->ڈیٹا[newTrie->ڈیٹا_لمبائی++]=بلند_قدر;
    جب((newTrie->ڈیٹا_لمبائی&(UTRIE2_ڈیٹا_GRANULARITY-1))!=0) {
        newTrie->ڈیٹا[newTrie->ڈیٹا_لمبائی++]=trie->initial_قدر;
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
#تعریف_کرو UTRIE2_MAX_اشاریہ_لمبائی 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#تعریف_کرو UTRIE2_MAX_ڈیٹا_لمبائی (0xffff<<UTRIE2_اشاریہ_سرکاؤ)

/* Compact and internally serialize the trie. */
U_CAPI خالی U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_قدر_بٹ قدر_بٹ, UError_رمز *p_Error_رمز) {
    UNewTrie2 *newTrie;
    UTrie2_سرخی *سرخی;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_لمبائی;
    int32_t ڈیٹا_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 بلند_آغاز;

    /* argument check */
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    اگر( trie==خالی_اشاریہ ||
        قدر_بٹ<0 || UTRIE2_گنتی_قدر_بٹ<=قدر_بٹ
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    newTrie=trie->newTrie;
    اگر(newTrie==خالی_اشاریہ) {
        /* already frozen */
        UTrie2_قدر_بٹ frozen_قدر_بٹ=
            trie->data16!=خالی_اشاریہ ? UTRIE2_16_قدر_بٹ : UTRIE2_32_قدر_بٹ;
        اگر(قدر_بٹ!=frozen_قدر_بٹ) {
            *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        }
        واپس;
    }

    /* compact if necessary */
    اگر(!newTrie->isCompacted) {
        compactTrie(trie, p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            واپس;
        }
    }
    بلند_آغاز=trie->بلند_آغاز;

    اگر(بلند_آغاز<=0x10000) {
        all_Indexes_لمبائی=UTRIE2_اشاریہ_1_انحراف;
    } ورنہ {
        all_Indexes_لمبائی=newTrie->index2_لمبائی;
    }
    اگر(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        ڈیٹا_Move=all_Indexes_لمبائی;
    } ورنہ {
        ڈیٹا_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    اگر( /* for unshifted indexLength */
        all_Indexes_لمبائی>UTRIE2_MAX_اشاریہ_لمبائی ||
        /* for unshifted dataNullOffset */
        (ڈیٹا_Move+newTrie->ڈیٹا_Null_انحراف)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (ڈیٹا_Move+UNEWTRIE2_ڈیٹا_0800_انحراف)>0xffff ||
        /* for shiftedDataLength */
        (ڈیٹا_Move+newTrie->ڈیٹا_لمبائی)>UTRIE2_MAX_ڈیٹا_لمبائی
    ) {
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس;
    }

    /* calculate the total serialized length */
    length=حجم(UTrie2_سرخی)+all_Indexes_لمبائی*2;
    اگر(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        length+=newTrie->ڈیٹا_لمبائی*2;
    } ورنہ {
        length+=newTrie->ڈیٹا_لمبائی*4;
    }

    trie->حافظہ=uprv_malloc(length);
    اگر(trie->حافظہ==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    trie->length=length;
    trie->is_حافظہ_Owned=سچ;

    trie->اشاریہ_لمبائی=all_Indexes_لمبائی;
    trie->ڈیٹا_لمبائی=newTrie->ڈیٹا_لمبائی;
    اگر(بلند_آغاز<=0x10000) {
        trie->index2_Null_انحراف=0xffff;
    } ورنہ {
        trie->index2_Null_انحراف=static_cast<uint16_t>(UTRIE2_اشاریہ_2_انحراف+newTrie->index2_Null_انحراف);
    }
    trie->ڈیٹا_Null_انحراف=(uint16_t)(ڈیٹا_Move+newTrie->ڈیٹا_Null_انحراف);
    trie->بلند_قدر_اشاریہ=ڈیٹا_Move+trie->ڈیٹا_لمبائی-UTRIE2_ڈیٹا_GRANULARITY;

    /* set the header fields */
    سرخی=(UTrie2_سرخی *)trie->حافظہ;

    سرخی->signature=UTRIE2_SIG; /* "Tri2" */
    سرخی->options=(uint16_t)قدر_بٹ;

    سرخی->اشاریہ_لمبائی=(uint16_t)trie->اشاریہ_لمبائی;
    سرخی->shifted_ڈیٹا_لمبائی=(uint16_t)(trie->ڈیٹا_لمبائی>>UTRIE2_اشاریہ_سرکاؤ);
    سرخی->index2_Null_انحراف=trie->index2_Null_انحراف;
    سرخی->ڈیٹا_Null_انحراف=trie->ڈیٹا_Null_انحراف;
    سرخی->shifted_بلند_آغاز=(uint16_t)(بلند_آغاز>>UTRIE2_سرکاؤ_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرخی+1);
    trie->اشاریہ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    برائے(i=UTRIE2_اشاریہ_2_BMP_لمبائی; i>0; --i) {
        *dest16++=(uint16_t)((ڈیٹا_Move + *p++)>>UTRIE2_اشاریہ_سرکاؤ);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    برائے(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(ڈیٹا_Move+UTRIE2_BAD_UTF8_ڈیٹا_انحراف);
    }
    برائے(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(ڈیٹا_Move+newTrie->index2[i<<(6-UTRIE2_سرکاؤ_2)]);
    }

    اگر(بلند_آغاز>0x10000) {
        int32_t index1_لمبائی=(بلند_آغاز-0x10000)>>UTRIE2_سرکاؤ_1;
        int32_t index2_انحراف=UTRIE2_اشاریہ_2_BMP_لمبائی+UTRIE2_UTF8_2_B_اشاریہ_2_لمبائی+index1_لمبائی;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_اشاریہ_1_لمبائی;
        برائے(i=index1_لمبائی; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_اشاریہ_2_انحراف + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2_انحراف;
        برائے(i=newTrie->index2_لمبائی-index2_انحراف; i>0; --i) {
            *dest16++=(uint16_t)((ڈیٹا_Move + *p++)>>UTRIE2_اشاریہ_سرکاؤ);
        }
    }

    /* write the 16/32-bit data array */
    انتخاب(قدر_بٹ) {
    حالت UTRIE2_16_قدر_بٹ:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=خالی_اشاریہ;
        p=newTrie->ڈیٹا;
        برائے(i=newTrie->ڈیٹا_لمبائی; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        توڑو;
    حالت UTRIE2_32_قدر_بٹ:
        /* write 32-bit data values */
        trie->data16=خالی_اشاریہ;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->ڈیٹا, (size_t)newTrie->ڈیٹا_لمبائی*4);
        توڑو;
    طےشدہ:
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

#اگر_تعریف_شدہ UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#اگر_ختم

#اگر_تعریف_شدہ UCPTRIE_DEBUG
    umutablecptrie_سیٹ_Name(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_قدر_چوڑائی)قدر_بٹ, p_Error_رمز));
#اگر_ختم
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->ڈیٹا);
    uprv_free(newTrie);
    trie->newTrie=خالی_اشاریہ;
}

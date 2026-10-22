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
#اگر_تعریف_شده UTRIE2_DEBUG
#   include <stdio.h>
#پایان_اگر
// #define UCPTRIE_DEBUG

#ضمن "unicode/utypes.h"
#اگر_تعریف_شده UCPTRIE_DEBUG
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "ucptrie_impl.h"
#پایان_اگر
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
    UNEWTRIE2_شاخص_2_NULL_جابه_جایی=UNEWTRIE2_شاخص_GAP_جابه_جایی+UNEWTRIE2_شاخص_GAP_درازی,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_شاخص_2_شروع_جابه_جایی=UNEWTRIE2_شاخص_2_NULL_جابه_جایی+UTRIE2_شاخص_2_بلوک_درازی,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_داده_NULL_جابه_جایی=UTRIE2_داده_شروع_جابه_جایی,

    /** The start of allocated data blocks. */
    UNEWTRIE2_داده_شروع_جابه_جایی=UNEWTRIE2_داده_NULL_جابه_جایی+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_داده_0800_جابه_جایی=UNEWTRIE2_داده_شروع_جابه_جایی+0x780
};

/* Start with allocation of 16k data entries. */
#تعریف_کن UNEWTRIE2_INITIAL_داده_درازی ((int32_t)1<<14)

/* Grow about 8x each time. */
#تعریف_کن UNEWTRIE2_MEDIUM_داده_درازی ((int32_t)1<<17)

ساكن int32_t
alloc_Index2_بلوک(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_مقدار, uint32_t error_مقدار, UError_کد *p_Error_کد) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *داده;
    int32_t i, j;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(حجم(UNewTrie2));
    داده=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_داده_درازی*4);
    إذا(trie==اشاره_تهی || newTrie==اشاره_تهی || داده==اشاره_تهی) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(داده);
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }

    uprv_memset(trie, 0, حجم(UTrie2));
    trie->initial_مقدار=initial_مقدار;
    trie->error_مقدار=error_مقدار;
    trie->بالا_شروع=0x110000;
    trie->newTrie=newTrie;
#اگر_تعریف_شده UTRIE2_DEBUG
    trie->name="open";
#پایان_اگر

    newTrie->داده=داده;
#اگر_تعریف_شده UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_مقدار, error_مقدار, p_Error_کد);
#پایان_اگر
    newTrie->داده_ظرفیت=UNEWTRIE2_INITIAL_داده_درازی;
    newTrie->initial_مقدار=initial_مقدار;
    newTrie->error_مقدار=error_مقدار;
    newTrie->بالا_شروع=0x110000;
    newTrie->اولی_Free_بلوک=0;  /* no free block in the list */
    newTrie->isCompacted=خطأ;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    لكل(i=0; i<0x80; ++i) {
        newTrie->داده[i]=initial_مقدار;
    }
    لكل(; i<0xc0; ++i) {
        newTrie->داده[i]=error_مقدار;
    }
    لكل(i=UNEWTRIE2_داده_NULL_جابه_جایی; i<UNEWTRIE2_داده_شروع_جابه_جایی; ++i) {
        newTrie->داده[i]=initial_مقدار;
    }
    newTrie->داده_Null_جابه_جایی=UNEWTRIE2_داده_NULL_جابه_جایی;
    newTrie->داده_درازی=UNEWTRIE2_داده_شروع_جابه_جایی;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    لكل(i=0, j=0; j<0x80; ++i, j+=UTRIE2_داده_بلوک_درازی) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    لكل(; j<0xc0; ++i, j+=UTRIE2_داده_بلوک_درازی) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_لغزش_2)-
        (0x80>>UTRIE2_لغزش_2)+
        1+
        UTRIE2_LSCP_شاخص_2_درازی;
    j+=UTRIE2_داده_بلوک_درازی;
    لكل(; j<UNEWTRIE2_داده_شروع_جابه_جایی; ++i, j+=UTRIE2_داده_بلوک_درازی) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    لكل(i=0x80>>UTRIE2_لغزش_2; i<UTRIE2_شاخص_2_BMP_درازی; ++i) {
        newTrie->index2[i]=UNEWTRIE2_داده_NULL_جابه_جایی;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    لكل(i=0; i<UNEWTRIE2_شاخص_GAP_درازی; ++i) {
        newTrie->index2[UNEWTRIE2_شاخص_GAP_جابه_جایی+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    لكل(i=0; i<UTRIE2_شاخص_2_بلوک_درازی; ++i) {
        newTrie->index2[UNEWTRIE2_شاخص_2_NULL_جابه_جایی+i]=UNEWTRIE2_داده_NULL_جابه_جایی;
    }
    newTrie->index2_Null_جابه_جایی=UNEWTRIE2_شاخص_2_NULL_جابه_جایی;
    newTrie->index2_درازی=UNEWTRIE2_شاخص_2_شروع_جابه_جایی;

    /* set the index-1 indexes for the linear index-2 block */
    لكل(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_شاخص_1_درازی;
        ++i, j+=UTRIE2_شاخص_2_بلوک_درازی
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    لكل(; i<UNEWTRIE2_شاخص_1_درازی; ++i) {
        newTrie->index1[i]=UNEWTRIE2_شاخص_2_NULL_جابه_جایی;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    لكل(i=0x80; i<0x800; i+=UTRIE2_داده_بلوک_درازی) {
        utrie2_set32(trie, i, initial_مقدار, p_Error_کد);
    }

    أرجع trie;
}

ساكن UNewTrie2 *
cloneBuilder(ثابت UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(حجم(UNewTrie2)));
    إذا(trie==اشاره_تهی) {
        أرجع اشاره_تهی;
    }

    trie->داده = static_cast<uint32_t*>(uprv_malloc(other->داده_ظرفیت * 4));
    إذا(trie->داده==اشاره_تهی) {
        uprv_free(trie);
        أرجع اشاره_تهی;
    }
#اگر_تعریف_شده UCPTRIE_DEBUG
    إذا(other->t3==اشاره_تهی) {
        trie->t3=اشاره_تهی;
    } وإلا {
        UError_کد error_کد=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &error_کد);
    }
#پایان_اگر
    trie->داده_ظرفیت=other->داده_ظرفیت;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, حجم(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_درازی*4);
    trie->index2_Null_جابه_جایی=other->index2_Null_جابه_جایی;
    trie->index2_درازی=other->index2_درازی;

    uprv_memcpy(trie->داده, other->داده, (size_t)other->داده_درازی*4);
    trie->داده_Null_جابه_جایی=other->داده_Null_جابه_جایی;
    trie->داده_درازی=other->داده_درازی;

    /* reference counters */
    إذا(other->isCompacted) {
        trie->اولی_Free_بلوک=0;
    } وإلا {
        uprv_memcpy(trie->map, other->map, ((size_t)other->داده_درازی>>UTRIE2_لغزش_2)*4);
        trie->اولی_Free_بلوک=other->اولی_Free_بلوک;
    }

    trie->initial_مقدار=other->initial_مقدار;
    trie->error_مقدار=other->error_مقدار;
    trie->بالا_شروع=other->بالا_شروع;
    trie->isCompacted=other->isCompacted;

    أرجع trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ثابت UTrie2 *other, UError_کد *p_Error_کد) {
    UTrie2 *trie;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا(other==اشاره_تهی || (other->حافظه==اشاره_تهی && other->newTrie==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memcpy(trie, other, حجم(UTrie2));

    إذا(other->حافظه!=اشاره_تهی) {
        trie->حافظه=uprv_malloc(other->length);
        إذا(trie->حافظه!=اشاره_تهی) {
            trie->is_حافظه_Owned=درست;
            uprv_memcpy(trie->حافظه, other->حافظه, other->length);

            /* make the clone's pointers point to its own memory */
            trie->شاخص=(uint16_t *)trie->حافظه+(other->شاخص-(uint16_t *)other->حافظه);
            إذا(other->data16!=اشاره_تهی) {
                trie->data16=(uint16_t *)trie->حافظه+(other->data16-(uint16_t *)other->حافظه);
            }
            إذا(other->data32!=اشاره_تهی) {
                trie->data32=(uint32_t *)trie->حافظه+(other->data32-(uint32_t *)other->حافظه);
            }
        }
    } وإلا /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    إذا(trie->حافظه==اشاره_تهی && trie->newTrie==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        uprv_free(trie);
        trie=اشاره_تهی;
    }
    أرجع trie;
}

تعريفنوع بنية New_Trie_And_وضعیت {
    UTrie2 *trie;
    UError_کد error_کد;
    UBool exclusive_حد;  /* rather than inclusive range end */
} New_Trie_And_وضعیت;

ساكن UBool U_CALLCONV
copyEnumRange(ثابت فراغ *context, UChar32 شروع, UChar32 انتها, uint32_t value) {
    New_Trie_And_وضعیت *nt=(New_Trie_And_وضعیت *)context;
    إذا(value!=nt->trie->initial_مقدار) {
        إذا(nt->exclusive_حد) {
            --انتها;
        }
        إذا(شروع==انتها) {
            utrie2_set32(nt->trie, شروع, value, &nt->error_کد);
        } وإلا {
            utrie2_مجموعه_Range32(nt->trie, شروع, انتها, value, درست, &nt->error_کد);
        }
        أرجع U_SUCCESS(nt->error_کد);
    } وإلا {
        أرجع درست;
    }
}

#اگر_تعریف_شده UTRIE2_DEBUG
ساكن دراز شمار_Initial(ثابت UTrie2 *trie) {
    uint32_t initial_مقدار=trie->initial_مقدار;
    int32_t length=trie->داده_درازی;
    دراز count=0;
    إذا(trie->data16!=اشاره_تهی) {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data16[i]==initial_مقدار) { ++count; }
        }
    } وإلا {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data32[i]==initial_مقدار) { ++count; }
        }
    }
    أرجع count;
}

ساكن فراغ
utrie_printLengths(ثابت UTrie *trie) {
    دراز شاخص_درازی=trie->شاخص_درازی;
    دراز داده_درازی=(دراز)trie->داده_درازی;
    دراز مجموع_درازی=(دراز)حجم(UTrie_سرآیند)+شاخص_درازی*2+داده_درازی*(trie->data32!=اشاره_تهی ? 4 : 2);
    چاپ_کن("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           شاخص_درازی, داده_درازی, مجموع_درازی);
}

ساكن فراغ
utrie2_printLengths(ثابت UTrie2 *trie, ثابت حرف *which) {
    دراز شاخص_درازی=trie->شاخص_درازی;
    دراز داده_درازی=(دراز)trie->داده_درازی;
    دراز مجموع_درازی=(دراز)حجم(UTrie2_سرآیند)+شاخص_درازی*2+داده_درازی*(trie->data32!=اشاره_تهی ? 4 : 2);
    چاپ_کن("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, شاخص_درازی, داده_درازی, شمار_Initial(trie), مجموع_درازی);
}
#پایان_اگر

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ثابت UTrie2 *other, UError_کد *p_Error_کد) {
    New_Trie_And_وضعیت context;
    char16_t lead;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا(other==اشاره_تهی || (other->حافظه==اشاره_تهی && other->newTrie==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    إذا(other->newTrie!=اشاره_تهی && !other->newTrie->isCompacted) {
        أرجع utrie2_clone(other, p_Error_کد);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_مقدار, other->error_مقدار, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    context.exclusive_حد=خطأ;
    context.error_کد=*p_Error_کد;
    utrie2_enum(other, اشاره_تهی, copyEnumRange, &context);
    *p_Error_کد=context.error_کد;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(other->data32==اشاره_تهی) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } وإلا {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        إذا(value!=other->initial_مقدار) {
            utrie2_set32_For_Lead_Surrogate_کد_Unit(context.trie, lead, value, p_Error_کد);
        }
    }
    إذا(U_FAILURE(*p_Error_کد)) {
        utrie2_close(context.trie);
        context.trie=اشاره_تهی;
    }
    أرجع context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ثابت UTrie *trie1, uint32_t error_مقدار, UError_کد *p_Error_کد) {
    New_Trie_And_وضعیت context;
    char16_t lead;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا(trie1==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    context.trie=utrie2_open(trie1->initial_مقدار, error_مقدار, p_Error_کد);
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }
    context.exclusive_حد=درست;
    context.error_کد=*p_Error_کد;
    utrie_enum(trie1, اشاره_تهی, copyEnumRange, &context);
    *p_Error_کد=context.error_کد;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(trie1->data32==اشاره_تهی) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } وإلا {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        إذا(value!=trie1->initial_مقدار) {
            utrie2_set32_For_Lead_Surrogate_کد_Unit(context.trie, lead, value, p_Error_کد);
        }
    }
    إذا(U_SUCCESS(*p_Error_کد)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=اشاره_تهی ? UTRIE2_32_مقدار_بیتها : UTRIE2_16_مقدار_بیتها,
                      p_Error_کد);
    }
#اگر_تعریف_شده UTRIE2_DEBUG
    إذا(U_SUCCESS(*p_Error_کد)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#پایان_اگر
    إذا(U_FAILURE(*p_Error_کد)) {
        utrie2_close(context.trie);
        context.trie=اشاره_تهی;
    }
    أرجع context.trie;
}

ساكن مضمن UBool
is_درون_Null_بلوک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, بلوک;

    إذا(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_شاخص_2_جابه_جایی-(0xd800>>UTRIE2_لغزش_2))+
            (c>>UTRIE2_لغزش_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_لغزش_1]+
            ((c>>UTRIE2_لغزش_2)&UTRIE2_شاخص_2_ماسک);
    }
    بلوک=trie->index2[i2];
    أرجع بلوک == trie->داده_Null_جابه_جایی;
}

ساكن int32_t
alloc_Index2_بلوک(UNewTrie2 *trie) {
    int32_t new_بلوک, new_بالاترین;

    new_بلوک=trie->index2_درازی;
    new_بالاترین=new_بلوک+UTRIE2_شاخص_2_بلوک_درازی;
    إذا(new_بالاترین>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        أرجع -1;
    }
    trie->index2_درازی=new_بالاترین;
    uprv_memcpy(trie->index2+new_بلوک, trie->index2+trie->index2_Null_جابه_جایی, UTRIE2_شاخص_2_بلوک_درازی*4);
    أرجع new_بلوک;
}

ساكن int32_t
get_Index2_بلوک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    إذا(U_IS_LEAD(c) && forLSCP) {
        أرجع UTRIE2_LSCP_شاخص_2_جابه_جایی;
    }

    i1=c>>UTRIE2_لغزش_1;
    i2=trie->index1[i1];
    إذا(i2==trie->index2_Null_جابه_جایی) {
        i2=alloc_Index2_بلوک(trie);
        إذا(i2<0) {
            أرجع -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    أرجع i2;
}

ساكن int32_t
alloc_داده_بلوک(UNewTrie2 *trie, int32_t copy_بلوک) {
    int32_t new_بلوک, new_بالاترین;

    إذا(trie->اولی_Free_بلوک!=0) {
        /* get the first free block */
        new_بلوک=trie->اولی_Free_بلوک;
        trie->اولی_Free_بلوک=-trie->map[new_بلوک>>UTRIE2_لغزش_2];
    } وإلا {
        /* get a new block from the high end */
        new_بلوک=trie->داده_درازی;
        new_بالاترین=new_بلوک+UTRIE2_داده_بلوک_درازی;
        إذا(new_بالاترین>trie->داده_ظرفیت) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *داده;

            إذا(trie->داده_ظرفیت<UNEWTRIE2_MEDIUM_داده_درازی) {
                capacity=UNEWTRIE2_MEDIUM_داده_درازی;
            } وإلا إذا(trie->داده_ظرفیت<UNEWTRIE2_MAX_داده_درازی) {
                capacity=UNEWTRIE2_MAX_داده_درازی;
            } وإلا {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                أرجع -1;
            }
            داده = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            إذا(داده==اشاره_تهی) {
                أرجع -1;
            }
            uprv_memcpy(داده, trie->داده, (size_t)trie->داده_درازی*4);
            uprv_free(trie->داده);
            trie->داده=داده;
            trie->داده_ظرفیت=capacity;
        }
        trie->داده_درازی=new_بالاترین;
    }
    uprv_memcpy(trie->داده+new_بلوک, trie->داده+copy_بلوک, UTRIE2_داده_بلوک_درازی*4);
    trie->map[new_بلوک>>UTRIE2_لغزش_2]=0;
    أرجع new_بلوک;
}

/* call when the block's reference counter reaches 0 */
ساكن فراغ
release_داده_بلوک(UNewTrie2 *trie, int32_t بلوک) {
    /* put this block at the front of the free-block chain */
    trie->map[بلوک>>UTRIE2_لغزش_2]=-trie->اولی_Free_بلوک;
    trie->اولی_Free_بلوک=بلوک;
}

ساكن مضمن UBool
is_Writable_بلوک(UNewTrie2 *trie, int32_t بلوک) {
    أرجع بلوک != trie->داده_Null_جابه_جایی && 1 == trie->map[بلوک >> UTRIE2_لغزش_2];
}

ساكن مضمن فراغ
مجموعه_Index2_مدخل(UNewTrie2 *trie, int32_t i2, int32_t بلوک) {
    int32_t old_بلوک;
    ++trie->map[بلوک>>UTRIE2_لغزش_2];  /* increment first, in case block==oldBlock! */
    old_بلوک=trie->index2[i2];
    إذا(0 == --trie->map[old_بلوک>>UTRIE2_لغزش_2]) {
        release_داده_بلوک(trie, old_بلوک);
    }
    trie->index2[i2]=بلوک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ساكن int32_t
get_داده_بلوک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, old_بلوک, new_بلوک;

    i2=get_Index2_بلوک(trie, c, forLSCP);
    إذا(i2<0) {
        أرجع -1;  /* program error */
    }

    i2+=(c>>UTRIE2_لغزش_2)&UTRIE2_شاخص_2_ماسک;
    old_بلوک=trie->index2[i2];
    إذا(is_Writable_بلوک(trie, old_بلوک)) {
        أرجع old_بلوک;
    }

    /* allocate a new data block */
    new_بلوک=alloc_داده_بلوک(trie, old_بلوک);
    إذا(new_بلوک<0) {
        /* out of memory in the data array */
        أرجع -1;
    }
    مجموعه_Index2_مدخل(trie, i2, new_بلوک);
    أرجع new_بلوک;
}

/**
 * @return true if the value was successfully set
 */
ساكن فراغ
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UError_کد *p_Error_کد) {
    int32_t بلوک;

    إذا(trie==اشاره_تهی || trie->isCompacted) {
        *p_Error_کد=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#اگر_تعریف_شده UCPTRIE_DEBUG
    umutablecptrie_مجموعه(trie->t3, c, value, p_Error_کد);
#پایان_اگر

    بلوک=get_داده_بلوک(trie, c, forLSCP);
    إذا(بلوک<0) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    trie->داده[بلوک+(c&UTRIE2_داده_ماسک)]=value;
}

U_CAPI فراغ U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    إذا((uint32_t)c>0x10ffff) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, درست, value, p_Error_کد);
}

U_CAPI فراغ U_EXPORT2
utrie2_set32_For_Lead_Surrogate_کد_Unit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    إذا(!U_IS_LEAD(c)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, خطأ, value, p_Error_کد);
}

ساكن فراغ
write_بلوک(uint32_t *بلوک, uint32_t value) {
    uint32_t *حد=بلوک+UTRIE2_داده_بلوک_درازی;
    طالما(بلوک<حد) {
        *بلوک++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
ساكن فراغ
fill_بلوک(uint32_t *بلوک, UChar32 شروع, UChar32 حد,
          uint32_t value, uint32_t initial_مقدار, UBool overwrite) {
    uint32_t *p_حد;

    p_حد=بلوک+حد;
    بلوک+=شروع;
    إذا(overwrite) {
        طالما(بلوک<p_حد) {
            *بلوک++=value;
        }
    } وإلا {
        طالما(بلوک<p_حد) {
            إذا(*بلوک==initial_مقدار) {
                *بلوک=value;
            }
            ++بلوک;
        }
    }
}

U_CAPI فراغ U_EXPORT2
utrie2_مجموعه_Range32(UTrie2 *trie,
                  UChar32 شروع, UChar32 انتها,
                  uint32_t value, UBool overwrite,
                  UError_کد *p_Error_کد) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t بلوک, rest, repeat_بلوک;
    UChar32 حد;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    إذا((uint32_t)شروع>0x10ffff || (uint32_t)انتها>0x10ffff || شروع>انتها) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==اشاره_تهی || newTrie->isCompacted) {
        *p_Error_کد=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#اگر_تعریف_شده UCPTRIE_DEBUG
    umutablecptrie_مجموعه_Range(newTrie->t3, شروع, انتها, value, p_Error_کد);
#پایان_اگر
    إذا(!overwrite && value==newTrie->initial_مقدار) {
        أرجع; /* nothing to do */
    }

    حد=انتها+1;
    إذا(شروع&UTRIE2_داده_ماسک) {
        UChar32 بعدی_شروع;

        /* set partial block at [start..following block boundary[ */
        بلوک=get_داده_بلوک(newTrie, شروع, درست);
        إذا(بلوک<0) {
            *p_Error_کد=U_حافظه_تخصیص_ERROR;
            أرجع;
        }

        بعدی_شروع=(شروع+UTRIE2_داده_ماسک)&~UTRIE2_داده_ماسک;
        إذا(بعدی_شروع<=حد) {
            fill_بلوک(newTrie->داده+بلوک, شروع&UTRIE2_داده_ماسک, UTRIE2_داده_بلوک_درازی,
                      value, newTrie->initial_مقدار, overwrite);
            شروع=بعدی_شروع;
        } وإلا {
            fill_بلوک(newTrie->داده+بلوک, شروع&UTRIE2_داده_ماسک, حد&UTRIE2_داده_ماسک,
                      value, newTrie->initial_مقدار, overwrite);
            أرجع;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد&UTRIE2_داده_ماسک;

    /* round down limit to a block boundary */
    حد&=~UTRIE2_داده_ماسک;

    /* iterate over all-value blocks */
    إذا(value==newTrie->initial_مقدار) {
        repeat_بلوک=newTrie->داده_Null_جابه_جایی;
    } وإلا {
        repeat_بلوک=-1;
    }

    طالما(شروع<حد) {
        int32_t i2;
        UBool مجموعه_Repeat_بلوک=خطأ;

        إذا(value==newTrie->initial_مقدار && is_درون_Null_بلوک(newTrie, شروع, درست)) {
            شروع+=UTRIE2_داده_بلوک_درازی; /* nothing to do */
            استمر;
        }

        /* get index value */
        i2=get_Index2_بلوک(newTrie, شروع, درست);
        إذا(i2<0) {
            *p_Error_کد=U_داخلی_برنامه_ERROR;
            أرجع;
        }
        i2+=(شروع>>UTRIE2_لغزش_2)&UTRIE2_شاخص_2_ماسک;
        بلوک=newTrie->index2[i2];
        إذا(is_Writable_بلوک(newTrie, بلوک)) {
            /* already allocated */
            إذا(overwrite && بلوک>=UNEWTRIE2_داده_0800_جابه_جایی) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                مجموعه_Repeat_بلوک=درست;
            } وإلا {
                /* !overwrite, or protected block: just write the values into this block */
                fill_بلوک(newTrie->داده+بلوک,
                          0, UTRIE2_داده_بلوک_درازی,
                          value, newTrie->initial_مقدار, overwrite);
            }
        } وإلا إذا(newTrie->داده[بلوک]!=value && (overwrite || بلوک==newTrie->داده_Null_جابه_جایی)) {
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
            مجموعه_Repeat_بلوک=درست;
        }
        إذا(مجموعه_Repeat_بلوک) {
            إذا(repeat_بلوک>=0) {
                مجموعه_Index2_مدخل(newTrie, i2, repeat_بلوک);
            } وإلا {
                /* create and set and fill the repeatBlock */
                repeat_بلوک=get_داده_بلوک(newTrie, شروع, درست);
                إذا(repeat_بلوک<0) {
                    *p_Error_کد=U_حافظه_تخصیص_ERROR;
                    أرجع;
                }
                write_بلوک(newTrie->داده+repeat_بلوک, value);
            }
        }

        شروع+=UTRIE2_داده_بلوک_درازی;
    }

    إذا(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلوک=get_داده_بلوک(newTrie, شروع, درست);
        إذا(بلوک<0) {
            *p_Error_کد=U_حافظه_تخصیص_ERROR;
            أرجع;
        }

        fill_بلوک(newTrie->داده+بلوک, 0, rest, value, newTrie->initial_مقدار, overwrite);
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
پیدا_کن_Same_Index2_بلوک(ثابت int32_t *idx, int32_t index2_درازی, int32_t other_بلوک) {
    int32_t بلوک;

    /* ensure that we do not even partially get past index2Length */
    index2_درازی-=UTRIE2_شاخص_2_بلوک_درازی;

    لكل(بلوک=0; بلوک<=index2_درازی; ++بلوک) {
        إذا(equal_int32(idx+بلوک, idx+other_بلوک, UTRIE2_شاخص_2_بلوک_درازی)) {
            أرجع بلوک;
        }
    }
    أرجع -1;
}

ساكن int32_t
پیدا_کن_Same_داده_بلوک_2(ثابت uint32_t *داده, int32_t داده_درازی, int32_t other_بلوک, int32_t بلوک_درازی) {
    int32_t بلوک;

    /* ensure that we do not even partially get past dataLength */
    داده_درازی-=بلوک_درازی;

    لكل(بلوک=0; بلوک<=داده_درازی; بلوک+=UTRIE2_داده_GRANULARITY) {
        إذا(equal_uint32(داده+بلوک, داده+other_بلوک, بلوک_درازی)) {
            أرجع بلوک;
        }
    }
    أرجع -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
ساكن UChar32
پیدا_کن_بالا_شروع(UNewTrie2 *trie, uint32_t بالا_مقدار) {
    ثابت uint32_t *data32;

    uint32_t value, initial_مقدار;
    UChar32 c, prev;
    int32_t i1, i2, j, i2_بلوک, prev_I2_بلوک, index2_Null_جابه_جایی, بلوک, prev_بلوک, null_بلوک;

    data32=trie->داده;
    initial_مقدار=trie->initial_مقدار;

    index2_Null_جابه_جایی=trie->index2_Null_جابه_جایی;
    null_بلوک=trie->داده_Null_جابه_جایی;

    /* set variables for previous range */
    إذا(بالا_مقدار==initial_مقدار) {
        prev_I2_بلوک=index2_Null_جابه_جایی;
        prev_بلوک=null_بلوک;
    } وإلا {
        prev_I2_بلوک=-1;
        prev_بلوک=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_شاخص_1_درازی;
    c=prev;
    طالما(c>0) {
        i2_بلوک=trie->index1[--i1];
        إذا(i2_بلوک==prev_I2_بلوک) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_در_هر_شاخص_1_مدخل;
            استمر;
        }
        prev_I2_بلوک=i2_بلوک;
        إذا(i2_بلوک==index2_Null_جابه_جایی) {
            /* this is the null index-2 block */
            إذا(بالا_مقدار!=initial_مقدار) {
                أرجع c;
            }
            c-=UTRIE2_CP_در_هر_شاخص_1_مدخل;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            لكل(i2=UTRIE2_شاخص_2_بلوک_درازی; i2>0;) {
                بلوک=trie->index2[i2_بلوک+ --i2];
                إذا(بلوک==prev_بلوک) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_داده_بلوک_درازی;
                    استمر;
                }
                prev_بلوک=بلوک;
                إذا(بلوک==null_بلوک) {
                    /* this is the null data block */
                    إذا(بالا_مقدار!=initial_مقدار) {
                        أرجع c;
                    }
                    c-=UTRIE2_داده_بلوک_درازی;
                } وإلا {
                    لكل(j=UTRIE2_داده_بلوک_درازی; j>0;) {
                        value=data32[بلوک+ --j];
                        إذا(value!=بالا_مقدار) {
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
compact_داده(UNewTrie2 *trie) {
#اگر_تعریف_شده UTRIE2_DEBUG
    int32_t شمار_Same=0, جمع_Overlaps=0;
#پایان_اگر

    int32_t شروع, new_شروع, moved_شروع;
    int32_t بلوک_درازی, overlap;
    int32_t i, map_شاخص, بلوک_شمار;

    /* do not compact linear-ASCII data */
    new_شروع=UTRIE2_داده_شروع_جابه_جایی;
    لكل(شروع=0, i=0; شروع<new_شروع; شروع+=UTRIE2_داده_بلوک_درازی, ++i) {
        trie->map[i]=شروع;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    بلوک_درازی=64;
    بلوک_شمار=بلوک_درازی>>UTRIE2_لغزش_2;
    لكل(شروع=new_شروع; شروع<trie->داده_درازی;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        إذا(شروع==UNEWTRIE2_داده_0800_جابه_جایی) {
            بلوک_درازی=UTRIE2_داده_بلوک_درازی;
            بلوک_شمار=1;
        }

        /* skip blocks that are not used */
        إذا(trie->map[شروع>>UTRIE2_لغزش_2]<=0) {
            /* advance start to the next block */
            شروع+=بلوک_درازی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* search for an identical block */
        إذا( (moved_شروع=پیدا_کن_Same_داده_بلوک_2(trie->داده, new_شروع, شروع, بلوک_درازی))
             >=0
        ) {
#اگر_تعریف_شده UTRIE2_DEBUG
            ++شمار_Same;
#پایان_اگر
            /* found an identical block, set the other block's index value for the current block */
            لكل(i=بلوک_شمار, map_شاخص=شروع>>UTRIE2_لغزش_2; i>0; --i) {
                trie->map[map_شاخص++]=moved_شروع;
                moved_شروع+=UTRIE2_داده_بلوک_درازی;
            }

            /* advance start to the next block */
            شروع+=بلوک_درازی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        لكل(overlap=بلوک_درازی-UTRIE2_داده_GRANULARITY;
            overlap>0 && !equal_uint32(trie->داده+(new_شروع-overlap), trie->داده+شروع, overlap);
            overlap-=UTRIE2_داده_GRANULARITY) {}

#اگر_تعریف_شده UTRIE2_DEBUG
            جمع_Overlaps+=overlap;
#پایان_اگر
        إذا(overlap>0 || new_شروع<شروع) {
            /* some overlap, or just move the whole block */
            moved_شروع=new_شروع-overlap;
            لكل(i=بلوک_شمار, map_شاخص=شروع>>UTRIE2_لغزش_2; i>0; --i) {
                trie->map[map_شاخص++]=moved_شروع;
                moved_شروع+=UTRIE2_داده_بلوک_درازی;
            }

            /* move the non-overlapping indexes to their new positions */
            شروع+=overlap;
            لكل(i=بلوک_درازی-overlap; i>0; --i) {
                trie->داده[new_شروع++]=trie->داده[شروع++];
            }
        } وإلا /* no overlap && newStart==start */ {
            لكل(i=بلوک_شمار, map_شاخص=شروع>>UTRIE2_لغزش_2; i>0; --i) {
                trie->map[map_شاخص++]=شروع;
                شروع+=UTRIE2_داده_بلوک_درازی;
            }
            new_شروع=شروع;
        }
    }

    /* now adjust the index-2 table */
    لكل(i=0; i<trie->index2_درازی; ++i) {
        إذا(i==UNEWTRIE2_شاخص_GAP_جابه_جایی) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_شاخص_GAP_درازی;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_لغزش_2];
    }
    trie->داده_Null_جابه_جایی=trie->map[trie->داده_Null_جابه_جایی>>UTRIE2_لغزش_2];

    /* ensure dataLength alignment */
    طالما((new_شروع&(UTRIE2_داده_GRANULARITY-1))!=0) {
        trie->داده[new_شروع++]=trie->initial_مقدار;
    }

#اگر_تعریف_شده UTRIE2_DEBUG
    /* we saved some space */
    چاپ_کن("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (دراز)trie->داده_درازی, (دراز)new_شروع, (دراز)شمار_Same, (دراز)جمع_Overlaps);
#پایان_اگر

    trie->داده_درازی=new_شروع;
}

ساكن فراغ
compactIndex2(UNewTrie2 *trie) {
    int32_t i, شروع, new_شروع, moved_شروع, overlap;

    /* do not compact linear-BMP index-2 blocks */
    new_شروع=UTRIE2_شاخص_2_BMP_درازی;
    لكل(شروع=0, i=0; شروع<new_شروع; شروع+=UTRIE2_شاخص_2_بلوک_درازی, ++i) {
        trie->map[i]=شروع;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    new_شروع+=UTRIE2_UTF8_2_B_شاخص_2_درازی+((trie->بالا_شروع-0x10000)>>UTRIE2_لغزش_1);

    لكل(شروع=UNEWTRIE2_شاخص_2_NULL_جابه_جایی; شروع<trie->index2_درازی;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        إذا( (moved_شروع=پیدا_کن_Same_Index2_بلوک(trie->index2, new_شروع, شروع))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[شروع>>UTRIE2_لغزش_1_2]=moved_شروع;

            /* advance start to the next block */
            شروع+=UTRIE2_شاخص_2_بلوک_درازی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        لكل(overlap=UTRIE2_شاخص_2_بلوک_درازی-1;
            overlap>0 && !equal_int32(trie->index2+(new_شروع-overlap), trie->index2+شروع, overlap);
            --overlap) {}

        إذا(overlap>0 || new_شروع<شروع) {
            /* some overlap, or just move the whole block */
            trie->map[شروع>>UTRIE2_لغزش_1_2]=new_شروع-overlap;

            /* move the non-overlapping indexes to their new positions */
            شروع+=overlap;
            لكل(i=UTRIE2_شاخص_2_بلوک_درازی-overlap; i>0; --i) {
                trie->index2[new_شروع++]=trie->index2[شروع++];
            }
        } وإلا /* no overlap && newStart==start */ {
            trie->map[شروع>>UTRIE2_لغزش_1_2]=شروع;
            شروع+=UTRIE2_شاخص_2_بلوک_درازی;
            new_شروع=شروع;
        }
    }

    /* now adjust the index-1 table */
    لكل(i=0; i<UNEWTRIE2_شاخص_1_درازی; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_لغزش_1_2];
    }
    trie->index2_Null_جابه_جایی=trie->map[trie->index2_Null_جابه_جایی>>UTRIE2_لغزش_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    طالما((new_شروع&((UTRIE2_داده_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[new_شروع++] = static_cast<int32_t>(0xffff) << UTRIE2_شاخص_لغزش;
    }

#اگر_تعریف_شده UTRIE2_DEBUG
    /* we saved some space */
    چاپ_کن("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (دراز)trie->index2_درازی, (دراز)new_شروع);
#پایان_اگر

    trie->index2_درازی=new_شروع;
}

ساكن فراغ
compactTrie(UTrie2 *trie, UError_کد *p_Error_کد) {
    UNewTrie2 *newTrie;
    UChar32 بالا_شروع, supp_بالا_شروع;
    uint32_t بالا_مقدار;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    بالا_مقدار=utrie2_get32(trie, 0x10ffff);
    بالا_شروع=پیدا_کن_بالا_شروع(newTrie, بالا_مقدار);
    بالا_شروع=(بالا_شروع+(UTRIE2_CP_در_هر_شاخص_1_مدخل-1))&~(UTRIE2_CP_در_هر_شاخص_1_مدخل-1);
    إذا(بالا_شروع==0x110000) {
        بالا_مقدار=trie->error_مقدار;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->بالا_شروع=newTrie->بالا_شروع=بالا_شروع;

#اگر_تعریف_شده UTRIE2_DEBUG
    چاپ_کن("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (دراز)بالا_شروع, (دراز)بالا_مقدار, (دراز)trie->initial_مقدار);
#پایان_اگر

    إذا(بالا_شروع<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_بالا_شروع= بالا_شروع<=0x10000 ? 0x10000 : بالا_شروع;
        utrie2_مجموعه_Range32(trie, supp_بالا_شروع, 0x10ffff, trie->initial_مقدار, درست, p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            أرجع;
        }
    }

    compact_داده(newTrie);
    إذا(بالا_شروع>0x10000) {
        compactIndex2(newTrie);
#اگر_تعریف_شده UTRIE2_DEBUG
    } وإلا {
        چاپ_کن("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (دراز)بالا_شروع, (دراز)trie->newTrie->index2_درازی, (دراز)UTRIE2_شاخص_1_جابه_جایی);
#پایان_اگر
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->داده[newTrie->داده_درازی++]=بالا_مقدار;
    طالما((newTrie->داده_درازی&(UTRIE2_داده_GRANULARITY-1))!=0) {
        newTrie->داده[newTrie->داده_درازی++]=trie->initial_مقدار;
    }

    newTrie->isCompacted=درست;
}

/* serialization ------------------------------------------------------------ */

/**
 * Maximum length of the runtime index array.
 * Limited by its own 16-bit index values, and by uint16_t UTrie2Header.indexLength.
 * (The actual maximum length is lower,
 * (0x110000>>UTRIE2_SHIFT_2)+UTRIE2_UTF8_2B_INDEX_2_LENGTH+UTRIE2_MAX_INDEX_1_LENGTH.)
 */
#تعریف_کن UTRIE2_MAX_شاخص_درازی 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#تعریف_کن UTRIE2_MAX_داده_درازی (0xffff<<UTRIE2_شاخص_لغزش)

/* Compact and internally serialize the trie. */
U_CAPI فراغ U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_مقدار_بیتها مقدار_بیتها, UError_کد *p_Error_کد) {
    UNewTrie2 *newTrie;
    UTrie2_سرآیند *سرآیند;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_درازی;
    int32_t داده_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 بالا_شروع;

    /* argument check */
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    إذا( trie==اشاره_تهی ||
        مقدار_بیتها<0 || UTRIE2_شمار_مقدار_بیتها<=مقدار_بیتها
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==اشاره_تهی) {
        /* already frozen */
        UTrie2_مقدار_بیتها frozen_مقدار_بیتها=
            trie->data16!=اشاره_تهی ? UTRIE2_16_مقدار_بیتها : UTRIE2_32_مقدار_بیتها;
        إذا(مقدار_بیتها!=frozen_مقدار_بیتها) {
            *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        }
        أرجع;
    }

    /* compact if necessary */
    إذا(!newTrie->isCompacted) {
        compactTrie(trie, p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            أرجع;
        }
    }
    بالا_شروع=trie->بالا_شروع;

    إذا(بالا_شروع<=0x10000) {
        all_Indexes_درازی=UTRIE2_شاخص_1_جابه_جایی;
    } وإلا {
        all_Indexes_درازی=newTrie->index2_درازی;
    }
    إذا(مقدار_بیتها==UTRIE2_16_مقدار_بیتها) {
        داده_Move=all_Indexes_درازی;
    } وإلا {
        داده_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    إذا( /* for unshifted indexLength */
        all_Indexes_درازی>UTRIE2_MAX_شاخص_درازی ||
        /* for unshifted dataNullOffset */
        (داده_Move+newTrie->داده_Null_جابه_جایی)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (داده_Move+UNEWTRIE2_داده_0800_جابه_جایی)>0xffff ||
        /* for shiftedDataLength */
        (داده_Move+newTrie->داده_درازی)>UTRIE2_MAX_داده_درازی
    ) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    /* calculate the total serialized length */
    length=حجم(UTrie2_سرآیند)+all_Indexes_درازی*2;
    إذا(مقدار_بیتها==UTRIE2_16_مقدار_بیتها) {
        length+=newTrie->داده_درازی*2;
    } وإلا {
        length+=newTrie->داده_درازی*4;
    }

    trie->حافظه=uprv_malloc(length);
    إذا(trie->حافظه==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    trie->length=length;
    trie->is_حافظه_Owned=درست;

    trie->شاخص_درازی=all_Indexes_درازی;
    trie->داده_درازی=newTrie->داده_درازی;
    إذا(بالا_شروع<=0x10000) {
        trie->index2_Null_جابه_جایی=0xffff;
    } وإلا {
        trie->index2_Null_جابه_جایی=static_cast<uint16_t>(UTRIE2_شاخص_2_جابه_جایی+newTrie->index2_Null_جابه_جایی);
    }
    trie->داده_Null_جابه_جایی=(uint16_t)(داده_Move+newTrie->داده_Null_جابه_جایی);
    trie->بالا_مقدار_شاخص=داده_Move+trie->داده_درازی-UTRIE2_داده_GRANULARITY;

    /* set the header fields */
    سرآیند=(UTrie2_سرآیند *)trie->حافظه;

    سرآیند->signature=UTRIE2_SIG; /* "Tri2" */
    سرآیند->options=(uint16_t)مقدار_بیتها;

    سرآیند->شاخص_درازی=(uint16_t)trie->شاخص_درازی;
    سرآیند->shifted_داده_درازی=(uint16_t)(trie->داده_درازی>>UTRIE2_شاخص_لغزش);
    سرآیند->index2_Null_جابه_جایی=trie->index2_Null_جابه_جایی;
    سرآیند->داده_Null_جابه_جایی=trie->داده_Null_جابه_جایی;
    سرآیند->shifted_بالا_شروع=(uint16_t)(بالا_شروع>>UTRIE2_لغزش_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرآیند+1);
    trie->شاخص=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    لكل(i=UTRIE2_شاخص_2_BMP_درازی; i>0; --i) {
        *dest16++=(uint16_t)((داده_Move + *p++)>>UTRIE2_شاخص_لغزش);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(داده_Move+UTRIE2_BAD_UTF8_داده_جابه_جایی);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(داده_Move+newTrie->index2[i<<(6-UTRIE2_لغزش_2)]);
    }

    إذا(بالا_شروع>0x10000) {
        int32_t index1_درازی=(بالا_شروع-0x10000)>>UTRIE2_لغزش_1;
        int32_t index2_جابه_جایی=UTRIE2_شاخص_2_BMP_درازی+UTRIE2_UTF8_2_B_شاخص_2_درازی+index1_درازی;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_شاخص_1_درازی;
        لكل(i=index1_درازی; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_شاخص_2_جابه_جایی + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2_جابه_جایی;
        لكل(i=newTrie->index2_درازی-index2_جابه_جایی; i>0; --i) {
            *dest16++=(uint16_t)((داده_Move + *p++)>>UTRIE2_شاخص_لغزش);
        }
    }

    /* write the 16/32-bit data array */
    بدل(مقدار_بیتها) {
    حالة UTRIE2_16_مقدار_بیتها:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=اشاره_تهی;
        p=newTrie->داده;
        لكل(i=newTrie->داده_درازی; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        اكسر;
    حالة UTRIE2_32_مقدار_بیتها:
        /* write 32-bit data values */
        trie->data16=اشاره_تهی;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->داده, (size_t)newTrie->داده_درازی*4);
        اكسر;
    افتراضي:
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

#اگر_تعریف_شده UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#پایان_اگر

#اگر_تعریف_شده UCPTRIE_DEBUG
    umutablecptrie_مجموعه_Name(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_مقدار_عرض)مقدار_بیتها, p_Error_کد));
#پایان_اگر
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->داده);
    uprv_free(newTrie);
    trie->newTrie=اشاره_تهی;
}

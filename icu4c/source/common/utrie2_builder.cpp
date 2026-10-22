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
#که_تعریف_شوی UTRIE2_DEBUG
#   include <stdio.h>
#د_که_پای
// #define UCPTRIE_DEBUG

#ضمن "unicode/utypes.h"
#که_تعریف_شوی UCPTRIE_DEBUG
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "ucptrie_impl.h"
#د_که_پای
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
    UNEWTRIE2_شاخص_2_NULL_دننه_واټن=UNEWTRIE2_شاخص_GAP_دننه_واټن+UNEWTRIE2_شاخص_GAP_اوږدتیا,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_شاخص_2_پیل_دننه_واټن=UNEWTRIE2_شاخص_2_NULL_دننه_واټن+UTRIE2_شاخص_2_بلاک_اوږدتیا,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_معلومات_NULL_دننه_واټن=UTRIE2_معلومات_پیل_دننه_واټن,

    /** The start of allocated data blocks. */
    UNEWTRIE2_معلومات_پیل_دننه_واټن=UNEWTRIE2_معلومات_NULL_دننه_واټن+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_معلومات_0800_دننه_واټن=UNEWTRIE2_معلومات_پیل_دننه_واټن+0x780
};

/* Start with allocation of 16k data entries. */
#تعریف_کړه UNEWTRIE2_INITIAL_معلومات_اوږدتیا ((int32_t)1<<14)

/* Grow about 8x each time. */
#تعریف_کړه UNEWTRIE2_MEDIUM_معلومات_اوږدتیا ((int32_t)1<<17)

ساكن int32_t
alloc_Index2_بلاک(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_ارزښت, uint32_t error_ارزښت, UError_کوډ *p_Error_کوډ) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *معلومات;
    int32_t i, j;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(حجم(UNewTrie2));
    معلومات=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_معلومات_اوږدتیا*4);
    إذا(trie==تش_اشاره || newTrie==تش_اشاره || معلومات==تش_اشاره) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(معلومات);
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }

    uprv_memset(trie, 0, حجم(UTrie2));
    trie->initial_ارزښت=initial_ارزښت;
    trie->error_ارزښت=error_ارزښت;
    trie->لوړ_پیل=0x110000;
    trie->newTrie=newTrie;
#که_تعریف_شوی UTRIE2_DEBUG
    trie->name="open";
#د_که_پای

    newTrie->معلومات=معلومات;
#که_تعریف_شوی UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_ارزښت, error_ارزښت, p_Error_کوډ);
#د_که_پای
    newTrie->معلومات_ظرفیت=UNEWTRIE2_INITIAL_معلومات_اوږدتیا;
    newTrie->initial_ارزښت=initial_ارزښت;
    newTrie->error_ارزښت=error_ارزښت;
    newTrie->لوړ_پیل=0x110000;
    newTrie->لومړی_Free_بلاک=0;  /* no free block in the list */
    newTrie->isCompacted=خطأ;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    لكل(i=0; i<0x80; ++i) {
        newTrie->معلومات[i]=initial_ارزښت;
    }
    لكل(; i<0xc0; ++i) {
        newTrie->معلومات[i]=error_ارزښت;
    }
    لكل(i=UNEWTRIE2_معلومات_NULL_دننه_واټن; i<UNEWTRIE2_معلومات_پیل_دننه_واټن; ++i) {
        newTrie->معلومات[i]=initial_ارزښت;
    }
    newTrie->معلومات_Null_دننه_واټن=UNEWTRIE2_معلومات_NULL_دننه_واټن;
    newTrie->معلومات_اوږدتیا=UNEWTRIE2_معلومات_پیل_دننه_واټن;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    لكل(i=0, j=0; j<0x80; ++i, j+=UTRIE2_معلومات_بلاک_اوږدتیا) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    لكل(; j<0xc0; ++i, j+=UTRIE2_معلومات_بلاک_اوږدتیا) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_ښويدنه_2)-
        (0x80>>UTRIE2_ښويدنه_2)+
        1+
        UTRIE2_LSCP_شاخص_2_اوږدتیا;
    j+=UTRIE2_معلومات_بلاک_اوږدتیا;
    لكل(; j<UNEWTRIE2_معلومات_پیل_دننه_واټن; ++i, j+=UTRIE2_معلومات_بلاک_اوږدتیا) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    لكل(i=0x80>>UTRIE2_ښويدنه_2; i<UTRIE2_شاخص_2_BMP_اوږدتیا; ++i) {
        newTrie->index2[i]=UNEWTRIE2_معلومات_NULL_دننه_واټن;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    لكل(i=0; i<UNEWTRIE2_شاخص_GAP_اوږدتیا; ++i) {
        newTrie->index2[UNEWTRIE2_شاخص_GAP_دننه_واټن+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    لكل(i=0; i<UTRIE2_شاخص_2_بلاک_اوږدتیا; ++i) {
        newTrie->index2[UNEWTRIE2_شاخص_2_NULL_دننه_واټن+i]=UNEWTRIE2_معلومات_NULL_دننه_واټن;
    }
    newTrie->index2_Null_دننه_واټن=UNEWTRIE2_شاخص_2_NULL_دننه_واټن;
    newTrie->index2_اوږدتیا=UNEWTRIE2_شاخص_2_پیل_دننه_واټن;

    /* set the index-1 indexes for the linear index-2 block */
    لكل(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_شاخص_1_اوږدتیا;
        ++i, j+=UTRIE2_شاخص_2_بلاک_اوږدتیا
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    لكل(; i<UNEWTRIE2_شاخص_1_اوږدتیا; ++i) {
        newTrie->index1[i]=UNEWTRIE2_شاخص_2_NULL_دننه_واټن;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    لكل(i=0x80; i<0x800; i+=UTRIE2_معلومات_بلاک_اوږدتیا) {
        utrie2_set32(trie, i, initial_ارزښت, p_Error_کوډ);
    }

    أرجع trie;
}

ساكن UNewTrie2 *
cloneBuilder(ثابت UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(حجم(UNewTrie2)));
    إذا(trie==تش_اشاره) {
        أرجع تش_اشاره;
    }

    trie->معلومات = static_cast<uint32_t*>(uprv_malloc(other->معلومات_ظرفیت * 4));
    إذا(trie->معلومات==تش_اشاره) {
        uprv_free(trie);
        أرجع تش_اشاره;
    }
#که_تعریف_شوی UCPTRIE_DEBUG
    إذا(other->t3==تش_اشاره) {
        trie->t3=تش_اشاره;
    } وإلا {
        UError_کوډ error_کوډ=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &error_کوډ);
    }
#د_که_پای
    trie->معلومات_ظرفیت=other->معلومات_ظرفیت;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, حجم(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_اوږدتیا*4);
    trie->index2_Null_دننه_واټن=other->index2_Null_دننه_واټن;
    trie->index2_اوږدتیا=other->index2_اوږدتیا;

    uprv_memcpy(trie->معلومات, other->معلومات, (size_t)other->معلومات_اوږدتیا*4);
    trie->معلومات_Null_دننه_واټن=other->معلومات_Null_دننه_واټن;
    trie->معلومات_اوږدتیا=other->معلومات_اوږدتیا;

    /* reference counters */
    إذا(other->isCompacted) {
        trie->لومړی_Free_بلاک=0;
    } وإلا {
        uprv_memcpy(trie->map, other->map, ((size_t)other->معلومات_اوږدتیا>>UTRIE2_ښويدنه_2)*4);
        trie->لومړی_Free_بلاک=other->لومړی_Free_بلاک;
    }

    trie->initial_ارزښت=other->initial_ارزښت;
    trie->error_ارزښت=other->error_ارزښت;
    trie->لوړ_پیل=other->لوړ_پیل;
    trie->isCompacted=other->isCompacted;

    أرجع trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ثابت UTrie2 *other, UError_کوډ *p_Error_کوډ) {
    UTrie2 *trie;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا(other==تش_اشاره || (other->حافظه==تش_اشاره && other->newTrie==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==تش_اشاره) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memcpy(trie, other, حجم(UTrie2));

    إذا(other->حافظه!=تش_اشاره) {
        trie->حافظه=uprv_malloc(other->length);
        إذا(trie->حافظه!=تش_اشاره) {
            trie->is_حافظه_Owned=ریښتیا;
            uprv_memcpy(trie->حافظه, other->حافظه, other->length);

            /* make the clone's pointers point to its own memory */
            trie->شاخص=(uint16_t *)trie->حافظه+(other->شاخص-(uint16_t *)other->حافظه);
            إذا(other->data16!=تش_اشاره) {
                trie->data16=(uint16_t *)trie->حافظه+(other->data16-(uint16_t *)other->حافظه);
            }
            إذا(other->data32!=تش_اشاره) {
                trie->data32=(uint32_t *)trie->حافظه+(other->data32-(uint32_t *)other->حافظه);
            }
        }
    } وإلا /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    إذا(trie->حافظه==تش_اشاره && trie->newTrie==تش_اشاره) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        uprv_free(trie);
        trie=تش_اشاره;
    }
    أرجع trie;
}

تعريفنوع بنية New_Trie_And_حالت {
    UTrie2 *trie;
    UError_کوډ error_کوډ;
    UBool exclusive_حد;  /* rather than inclusive range end */
} New_Trie_And_حالت;

ساكن UBool U_CALLCONV
copyEnumRange(ثابت فراغ *context, UChar32 پیل, UChar32 پای_ټکی, uint32_t value) {
    New_Trie_And_حالت *nt=(New_Trie_And_حالت *)context;
    إذا(value!=nt->trie->initial_ارزښت) {
        إذا(nt->exclusive_حد) {
            --پای_ټکی;
        }
        إذا(پیل==پای_ټکی) {
            utrie2_set32(nt->trie, پیل, value, &nt->error_کوډ);
        } وإلا {
            utrie2_سېټ_Range32(nt->trie, پیل, پای_ټکی, value, ریښتیا, &nt->error_کوډ);
        }
        أرجع U_SUCCESS(nt->error_کوډ);
    } وإلا {
        أرجع ریښتیا;
    }
}

#که_تعریف_شوی UTRIE2_DEBUG
ساكن اوږد شمېر_Initial(ثابت UTrie2 *trie) {
    uint32_t initial_ارزښت=trie->initial_ارزښت;
    int32_t length=trie->معلومات_اوږدتیا;
    اوږد count=0;
    إذا(trie->data16!=تش_اشاره) {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data16[i]==initial_ارزښت) { ++count; }
        }
    } وإلا {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data32[i]==initial_ارزښت) { ++count; }
        }
    }
    أرجع count;
}

ساكن فراغ
utrie_printLengths(ثابت UTrie *trie) {
    اوږد شاخص_اوږدتیا=trie->شاخص_اوږدتیا;
    اوږد معلومات_اوږدتیا=(اوږد)trie->معلومات_اوږدتیا;
    اوږد ټول_اوږدتیا=(اوږد)حجم(UTrie_سرلیک)+شاخص_اوږدتیا*2+معلومات_اوږدتیا*(trie->data32!=تش_اشاره ? 4 : 2);
    چاپ_کړه("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           شاخص_اوږدتیا, معلومات_اوږدتیا, ټول_اوږدتیا);
}

ساكن فراغ
utrie2_printLengths(ثابت UTrie2 *trie, ثابت حرف *which) {
    اوږد شاخص_اوږدتیا=trie->شاخص_اوږدتیا;
    اوږد معلومات_اوږدتیا=(اوږد)trie->معلومات_اوږدتیا;
    اوږد ټول_اوږدتیا=(اوږد)حجم(UTrie2_سرلیک)+شاخص_اوږدتیا*2+معلومات_اوږدتیا*(trie->data32!=تش_اشاره ? 4 : 2);
    چاپ_کړه("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, شاخص_اوږدتیا, معلومات_اوږدتیا, شمېر_Initial(trie), ټول_اوږدتیا);
}
#د_که_پای

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ثابت UTrie2 *other, UError_کوډ *p_Error_کوډ) {
    New_Trie_And_حالت context;
    char16_t lead;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا(other==تش_اشاره || (other->حافظه==تش_اشاره && other->newTrie==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    إذا(other->newTrie!=تش_اشاره && !other->newTrie->isCompacted) {
        أرجع utrie2_clone(other, p_Error_کوډ);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_ارزښت, other->error_ارزښت, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    context.exclusive_حد=خطأ;
    context.error_کوډ=*p_Error_کوډ;
    utrie2_enum(other, تش_اشاره, copyEnumRange, &context);
    *p_Error_کوډ=context.error_کوډ;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(other->data32==تش_اشاره) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } وإلا {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        إذا(value!=other->initial_ارزښت) {
            utrie2_set32_For_Lead_Surrogate_کوډ_Unit(context.trie, lead, value, p_Error_کوډ);
        }
    }
    إذا(U_FAILURE(*p_Error_کوډ)) {
        utrie2_close(context.trie);
        context.trie=تش_اشاره;
    }
    أرجع context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ثابت UTrie *trie1, uint32_t error_ارزښت, UError_کوډ *p_Error_کوډ) {
    New_Trie_And_حالت context;
    char16_t lead;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا(trie1==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    context.trie=utrie2_open(trie1->initial_ارزښت, error_ارزښت, p_Error_کوډ);
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }
    context.exclusive_حد=ریښتیا;
    context.error_کوډ=*p_Error_کوډ;
    utrie_enum(trie1, تش_اشاره, copyEnumRange, &context);
    *p_Error_کوډ=context.error_کوډ;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(trie1->data32==تش_اشاره) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } وإلا {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        إذا(value!=trie1->initial_ارزښت) {
            utrie2_set32_For_Lead_Surrogate_کوډ_Unit(context.trie, lead, value, p_Error_کوډ);
        }
    }
    إذا(U_SUCCESS(*p_Error_کوډ)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=تش_اشاره ? UTRIE2_32_ارزښت_بټونه : UTRIE2_16_ارزښت_بټونه,
                      p_Error_کوډ);
    }
#که_تعریف_شوی UTRIE2_DEBUG
    إذا(U_SUCCESS(*p_Error_کوډ)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#د_که_پای
    إذا(U_FAILURE(*p_Error_کوډ)) {
        utrie2_close(context.trie);
        context.trie=تش_اشاره;
    }
    أرجع context.trie;
}

ساكن مضمن UBool
is_دننه_Null_بلاک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, بلاک;

    إذا(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_شاخص_2_دننه_واټن-(0xd800>>UTRIE2_ښويدنه_2))+
            (c>>UTRIE2_ښويدنه_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_ښويدنه_1]+
            ((c>>UTRIE2_ښويدنه_2)&UTRIE2_شاخص_2_ماسک);
    }
    بلاک=trie->index2[i2];
    أرجع بلاک == trie->معلومات_Null_دننه_واټن;
}

ساكن int32_t
alloc_Index2_بلاک(UNewTrie2 *trie) {
    int32_t new_بلاک, new_سر;

    new_بلاک=trie->index2_اوږدتیا;
    new_سر=new_بلاک+UTRIE2_شاخص_2_بلاک_اوږدتیا;
    إذا(new_سر>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        أرجع -1;
    }
    trie->index2_اوږدتیا=new_سر;
    uprv_memcpy(trie->index2+new_بلاک, trie->index2+trie->index2_Null_دننه_واټن, UTRIE2_شاخص_2_بلاک_اوږدتیا*4);
    أرجع new_بلاک;
}

ساكن int32_t
get_Index2_بلاک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    إذا(U_IS_LEAD(c) && forLSCP) {
        أرجع UTRIE2_LSCP_شاخص_2_دننه_واټن;
    }

    i1=c>>UTRIE2_ښويدنه_1;
    i2=trie->index1[i1];
    إذا(i2==trie->index2_Null_دننه_واټن) {
        i2=alloc_Index2_بلاک(trie);
        إذا(i2<0) {
            أرجع -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    أرجع i2;
}

ساكن int32_t
alloc_معلومات_بلاک(UNewTrie2 *trie, int32_t copy_بلاک) {
    int32_t new_بلاک, new_سر;

    إذا(trie->لومړی_Free_بلاک!=0) {
        /* get the first free block */
        new_بلاک=trie->لومړی_Free_بلاک;
        trie->لومړی_Free_بلاک=-trie->map[new_بلاک>>UTRIE2_ښويدنه_2];
    } وإلا {
        /* get a new block from the high end */
        new_بلاک=trie->معلومات_اوږدتیا;
        new_سر=new_بلاک+UTRIE2_معلومات_بلاک_اوږدتیا;
        إذا(new_سر>trie->معلومات_ظرفیت) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *معلومات;

            إذا(trie->معلومات_ظرفیت<UNEWTRIE2_MEDIUM_معلومات_اوږدتیا) {
                capacity=UNEWTRIE2_MEDIUM_معلومات_اوږدتیا;
            } وإلا إذا(trie->معلومات_ظرفیت<UNEWTRIE2_MAX_معلومات_اوږدتیا) {
                capacity=UNEWTRIE2_MAX_معلومات_اوږدتیا;
            } وإلا {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                أرجع -1;
            }
            معلومات = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            إذا(معلومات==تش_اشاره) {
                أرجع -1;
            }
            uprv_memcpy(معلومات, trie->معلومات, (size_t)trie->معلومات_اوږدتیا*4);
            uprv_free(trie->معلومات);
            trie->معلومات=معلومات;
            trie->معلومات_ظرفیت=capacity;
        }
        trie->معلومات_اوږدتیا=new_سر;
    }
    uprv_memcpy(trie->معلومات+new_بلاک, trie->معلومات+copy_بلاک, UTRIE2_معلومات_بلاک_اوږدتیا*4);
    trie->map[new_بلاک>>UTRIE2_ښويدنه_2]=0;
    أرجع new_بلاک;
}

/* call when the block's reference counter reaches 0 */
ساكن فراغ
release_معلومات_بلاک(UNewTrie2 *trie, int32_t بلاک) {
    /* put this block at the front of the free-block chain */
    trie->map[بلاک>>UTRIE2_ښويدنه_2]=-trie->لومړی_Free_بلاک;
    trie->لومړی_Free_بلاک=بلاک;
}

ساكن مضمن UBool
is_Writable_بلاک(UNewTrie2 *trie, int32_t بلاک) {
    أرجع بلاک != trie->معلومات_Null_دننه_واټن && 1 == trie->map[بلاک >> UTRIE2_ښويدنه_2];
}

ساكن مضمن فراغ
سېټ_Index2_ثبت(UNewTrie2 *trie, int32_t i2, int32_t بلاک) {
    int32_t old_بلاک;
    ++trie->map[بلاک>>UTRIE2_ښويدنه_2];  /* increment first, in case block==oldBlock! */
    old_بلاک=trie->index2[i2];
    إذا(0 == --trie->map[old_بلاک>>UTRIE2_ښويدنه_2]) {
        release_معلومات_بلاک(trie, old_بلاک);
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
get_معلومات_بلاک(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, old_بلاک, new_بلاک;

    i2=get_Index2_بلاک(trie, c, forLSCP);
    إذا(i2<0) {
        أرجع -1;  /* program error */
    }

    i2+=(c>>UTRIE2_ښويدنه_2)&UTRIE2_شاخص_2_ماسک;
    old_بلاک=trie->index2[i2];
    إذا(is_Writable_بلاک(trie, old_بلاک)) {
        أرجع old_بلاک;
    }

    /* allocate a new data block */
    new_بلاک=alloc_معلومات_بلاک(trie, old_بلاک);
    إذا(new_بلاک<0) {
        /* out of memory in the data array */
        أرجع -1;
    }
    سېټ_Index2_ثبت(trie, i2, new_بلاک);
    أرجع new_بلاک;
}

/**
 * @return true if the value was successfully set
 */
ساكن فراغ
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UError_کوډ *p_Error_کوډ) {
    int32_t بلاک;

    إذا(trie==تش_اشاره || trie->isCompacted) {
        *p_Error_کوډ=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#که_تعریف_شوی UCPTRIE_DEBUG
    umutablecptrie_سېټ(trie->t3, c, value, p_Error_کوډ);
#د_که_پای

    بلاک=get_معلومات_بلاک(trie, c, forLSCP);
    إذا(بلاک<0) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع;
    }

    trie->معلومات[بلاک+(c&UTRIE2_معلومات_ماسک)]=value;
}

U_CAPI فراغ U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    إذا((uint32_t)c>0x10ffff) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, ریښتیا, value, p_Error_کوډ);
}

U_CAPI فراغ U_EXPORT2
utrie2_set32_For_Lead_Surrogate_کوډ_Unit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    إذا(!U_IS_LEAD(c)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, خطأ, value, p_Error_کوډ);
}

ساكن فراغ
write_بلاک(uint32_t *بلاک, uint32_t value) {
    uint32_t *حد=بلاک+UTRIE2_معلومات_بلاک_اوږدتیا;
    طالما(بلاک<حد) {
        *بلاک++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
ساكن فراغ
fill_بلاک(uint32_t *بلاک, UChar32 پیل, UChar32 حد,
          uint32_t value, uint32_t initial_ارزښت, UBool overwrite) {
    uint32_t *p_حد;

    p_حد=بلاک+حد;
    بلاک+=پیل;
    إذا(overwrite) {
        طالما(بلاک<p_حد) {
            *بلاک++=value;
        }
    } وإلا {
        طالما(بلاک<p_حد) {
            إذا(*بلاک==initial_ارزښت) {
                *بلاک=value;
            }
            ++بلاک;
        }
    }
}

U_CAPI فراغ U_EXPORT2
utrie2_سېټ_Range32(UTrie2 *trie,
                  UChar32 پیل, UChar32 پای_ټکی,
                  uint32_t value, UBool overwrite,
                  UError_کوډ *p_Error_کوډ) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t بلاک, rest, repeat_بلاک;
    UChar32 حد;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    إذا((uint32_t)پیل>0x10ffff || (uint32_t)پای_ټکی>0x10ffff || پیل>پای_ټکی) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==تش_اشاره || newTrie->isCompacted) {
        *p_Error_کوډ=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#که_تعریف_شوی UCPTRIE_DEBUG
    umutablecptrie_سېټ_Range(newTrie->t3, پیل, پای_ټکی, value, p_Error_کوډ);
#د_که_پای
    إذا(!overwrite && value==newTrie->initial_ارزښت) {
        أرجع; /* nothing to do */
    }

    حد=پای_ټکی+1;
    إذا(پیل&UTRIE2_معلومات_ماسک) {
        UChar32 بل_پیل;

        /* set partial block at [start..following block boundary[ */
        بلاک=get_معلومات_بلاک(newTrie, پیل, ریښتیا);
        إذا(بلاک<0) {
            *p_Error_کوډ=U_حافظه_ویش_ERROR;
            أرجع;
        }

        بل_پیل=(پیل+UTRIE2_معلومات_ماسک)&~UTRIE2_معلومات_ماسک;
        إذا(بل_پیل<=حد) {
            fill_بلاک(newTrie->معلومات+بلاک, پیل&UTRIE2_معلومات_ماسک, UTRIE2_معلومات_بلاک_اوږدتیا,
                      value, newTrie->initial_ارزښت, overwrite);
            پیل=بل_پیل;
        } وإلا {
            fill_بلاک(newTrie->معلومات+بلاک, پیل&UTRIE2_معلومات_ماسک, حد&UTRIE2_معلومات_ماسک,
                      value, newTrie->initial_ارزښت, overwrite);
            أرجع;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد&UTRIE2_معلومات_ماسک;

    /* round down limit to a block boundary */
    حد&=~UTRIE2_معلومات_ماسک;

    /* iterate over all-value blocks */
    إذا(value==newTrie->initial_ارزښت) {
        repeat_بلاک=newTrie->معلومات_Null_دننه_واټن;
    } وإلا {
        repeat_بلاک=-1;
    }

    طالما(پیل<حد) {
        int32_t i2;
        UBool سېټ_Repeat_بلاک=خطأ;

        إذا(value==newTrie->initial_ارزښت && is_دننه_Null_بلاک(newTrie, پیل, ریښتیا)) {
            پیل+=UTRIE2_معلومات_بلاک_اوږدتیا; /* nothing to do */
            استمر;
        }

        /* get index value */
        i2=get_Index2_بلاک(newTrie, پیل, ریښتیا);
        إذا(i2<0) {
            *p_Error_کوډ=U_دننی_پروګرام_ERROR;
            أرجع;
        }
        i2+=(پیل>>UTRIE2_ښويدنه_2)&UTRIE2_شاخص_2_ماسک;
        بلاک=newTrie->index2[i2];
        إذا(is_Writable_بلاک(newTrie, بلاک)) {
            /* already allocated */
            إذا(overwrite && بلاک>=UNEWTRIE2_معلومات_0800_دننه_واټن) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                سېټ_Repeat_بلاک=ریښتیا;
            } وإلا {
                /* !overwrite, or protected block: just write the values into this block */
                fill_بلاک(newTrie->معلومات+بلاک,
                          0, UTRIE2_معلومات_بلاک_اوږدتیا,
                          value, newTrie->initial_ارزښت, overwrite);
            }
        } وإلا إذا(newTrie->معلومات[بلاک]!=value && (overwrite || بلاک==newTrie->معلومات_Null_دننه_واټن)) {
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
            سېټ_Repeat_بلاک=ریښتیا;
        }
        إذا(سېټ_Repeat_بلاک) {
            إذا(repeat_بلاک>=0) {
                سېټ_Index2_ثبت(newTrie, i2, repeat_بلاک);
            } وإلا {
                /* create and set and fill the repeatBlock */
                repeat_بلاک=get_معلومات_بلاک(newTrie, پیل, ریښتیا);
                إذا(repeat_بلاک<0) {
                    *p_Error_کوډ=U_حافظه_ویش_ERROR;
                    أرجع;
                }
                write_بلاک(newTrie->معلومات+repeat_بلاک, value);
            }
        }

        پیل+=UTRIE2_معلومات_بلاک_اوږدتیا;
    }

    إذا(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلاک=get_معلومات_بلاک(newTrie, پیل, ریښتیا);
        إذا(بلاک<0) {
            *p_Error_کوډ=U_حافظه_ویش_ERROR;
            أرجع;
        }

        fill_بلاک(newTrie->معلومات+بلاک, 0, rest, value, newTrie->initial_ارزښت, overwrite);
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
پیدا_کړه_Same_Index2_بلاک(ثابت int32_t *idx, int32_t index2_اوږدتیا, int32_t other_بلاک) {
    int32_t بلاک;

    /* ensure that we do not even partially get past index2Length */
    index2_اوږدتیا-=UTRIE2_شاخص_2_بلاک_اوږدتیا;

    لكل(بلاک=0; بلاک<=index2_اوږدتیا; ++بلاک) {
        إذا(equal_int32(idx+بلاک, idx+other_بلاک, UTRIE2_شاخص_2_بلاک_اوږدتیا)) {
            أرجع بلاک;
        }
    }
    أرجع -1;
}

ساكن int32_t
پیدا_کړه_Same_معلومات_بلاک_2(ثابت uint32_t *معلومات, int32_t معلومات_اوږدتیا, int32_t other_بلاک, int32_t بلاک_اوږدتیا) {
    int32_t بلاک;

    /* ensure that we do not even partially get past dataLength */
    معلومات_اوږدتیا-=بلاک_اوږدتیا;

    لكل(بلاک=0; بلاک<=معلومات_اوږدتیا; بلاک+=UTRIE2_معلومات_GRANULARITY) {
        إذا(equal_uint32(معلومات+بلاک, معلومات+other_بلاک, بلاک_اوږدتیا)) {
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
پیدا_کړه_لوړ_پیل(UNewTrie2 *trie, uint32_t لوړ_ارزښت) {
    ثابت uint32_t *data32;

    uint32_t value, initial_ارزښت;
    UChar32 c, prev;
    int32_t i1, i2, j, i2_بلاک, prev_I2_بلاک, index2_Null_دننه_واټن, بلاک, prev_بلاک, null_بلاک;

    data32=trie->معلومات;
    initial_ارزښت=trie->initial_ارزښت;

    index2_Null_دننه_واټن=trie->index2_Null_دننه_واټن;
    null_بلاک=trie->معلومات_Null_دننه_واټن;

    /* set variables for previous range */
    إذا(لوړ_ارزښت==initial_ارزښت) {
        prev_I2_بلاک=index2_Null_دننه_واټن;
        prev_بلاک=null_بلاک;
    } وإلا {
        prev_I2_بلاک=-1;
        prev_بلاک=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_شاخص_1_اوږدتیا;
    c=prev;
    طالما(c>0) {
        i2_بلاک=trie->index1[--i1];
        إذا(i2_بلاک==prev_I2_بلاک) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_فی_شاخص_1_ثبت;
            استمر;
        }
        prev_I2_بلاک=i2_بلاک;
        إذا(i2_بلاک==index2_Null_دننه_واټن) {
            /* this is the null index-2 block */
            إذا(لوړ_ارزښت!=initial_ارزښت) {
                أرجع c;
            }
            c-=UTRIE2_CP_فی_شاخص_1_ثبت;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            لكل(i2=UTRIE2_شاخص_2_بلاک_اوږدتیا; i2>0;) {
                بلاک=trie->index2[i2_بلاک+ --i2];
                إذا(بلاک==prev_بلاک) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_معلومات_بلاک_اوږدتیا;
                    استمر;
                }
                prev_بلاک=بلاک;
                إذا(بلاک==null_بلاک) {
                    /* this is the null data block */
                    إذا(لوړ_ارزښت!=initial_ارزښت) {
                        أرجع c;
                    }
                    c-=UTRIE2_معلومات_بلاک_اوږدتیا;
                } وإلا {
                    لكل(j=UTRIE2_معلومات_بلاک_اوږدتیا; j>0;) {
                        value=data32[بلاک+ --j];
                        إذا(value!=لوړ_ارزښت) {
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
#که_تعریف_شوی UTRIE2_DEBUG
    int32_t شمېر_Same=0, مجموعه_Overlaps=0;
#د_که_پای

    int32_t پیل, new_پیل, moved_پیل;
    int32_t بلاک_اوږدتیا, overlap;
    int32_t i, map_شاخص, بلاک_شمېر;

    /* do not compact linear-ASCII data */
    new_پیل=UTRIE2_معلومات_پیل_دننه_واټن;
    لكل(پیل=0, i=0; پیل<new_پیل; پیل+=UTRIE2_معلومات_بلاک_اوږدتیا, ++i) {
        trie->map[i]=پیل;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    بلاک_اوږدتیا=64;
    بلاک_شمېر=بلاک_اوږدتیا>>UTRIE2_ښويدنه_2;
    لكل(پیل=new_پیل; پیل<trie->معلومات_اوږدتیا;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        إذا(پیل==UNEWTRIE2_معلومات_0800_دننه_واټن) {
            بلاک_اوږدتیا=UTRIE2_معلومات_بلاک_اوږدتیا;
            بلاک_شمېر=1;
        }

        /* skip blocks that are not used */
        إذا(trie->map[پیل>>UTRIE2_ښويدنه_2]<=0) {
            /* advance start to the next block */
            پیل+=بلاک_اوږدتیا;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* search for an identical block */
        إذا( (moved_پیل=پیدا_کړه_Same_معلومات_بلاک_2(trie->معلومات, new_پیل, پیل, بلاک_اوږدتیا))
             >=0
        ) {
#که_تعریف_شوی UTRIE2_DEBUG
            ++شمېر_Same;
#د_که_پای
            /* found an identical block, set the other block's index value for the current block */
            لكل(i=بلاک_شمېر, map_شاخص=پیل>>UTRIE2_ښويدنه_2; i>0; --i) {
                trie->map[map_شاخص++]=moved_پیل;
                moved_پیل+=UTRIE2_معلومات_بلاک_اوږدتیا;
            }

            /* advance start to the next block */
            پیل+=بلاک_اوږدتیا;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        لكل(overlap=بلاک_اوږدتیا-UTRIE2_معلومات_GRANULARITY;
            overlap>0 && !equal_uint32(trie->معلومات+(new_پیل-overlap), trie->معلومات+پیل, overlap);
            overlap-=UTRIE2_معلومات_GRANULARITY) {}

#که_تعریف_شوی UTRIE2_DEBUG
            مجموعه_Overlaps+=overlap;
#د_که_پای
        إذا(overlap>0 || new_پیل<پیل) {
            /* some overlap, or just move the whole block */
            moved_پیل=new_پیل-overlap;
            لكل(i=بلاک_شمېر, map_شاخص=پیل>>UTRIE2_ښويدنه_2; i>0; --i) {
                trie->map[map_شاخص++]=moved_پیل;
                moved_پیل+=UTRIE2_معلومات_بلاک_اوږدتیا;
            }

            /* move the non-overlapping indexes to their new positions */
            پیل+=overlap;
            لكل(i=بلاک_اوږدتیا-overlap; i>0; --i) {
                trie->معلومات[new_پیل++]=trie->معلومات[پیل++];
            }
        } وإلا /* no overlap && newStart==start */ {
            لكل(i=بلاک_شمېر, map_شاخص=پیل>>UTRIE2_ښويدنه_2; i>0; --i) {
                trie->map[map_شاخص++]=پیل;
                پیل+=UTRIE2_معلومات_بلاک_اوږدتیا;
            }
            new_پیل=پیل;
        }
    }

    /* now adjust the index-2 table */
    لكل(i=0; i<trie->index2_اوږدتیا; ++i) {
        إذا(i==UNEWTRIE2_شاخص_GAP_دننه_واټن) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_شاخص_GAP_اوږدتیا;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_ښويدنه_2];
    }
    trie->معلومات_Null_دننه_واټن=trie->map[trie->معلومات_Null_دننه_واټن>>UTRIE2_ښويدنه_2];

    /* ensure dataLength alignment */
    طالما((new_پیل&(UTRIE2_معلومات_GRANULARITY-1))!=0) {
        trie->معلومات[new_پیل++]=trie->initial_ارزښت;
    }

#که_تعریف_شوی UTRIE2_DEBUG
    /* we saved some space */
    چاپ_کړه("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (اوږد)trie->معلومات_اوږدتیا, (اوږد)new_پیل, (اوږد)شمېر_Same, (اوږد)مجموعه_Overlaps);
#د_که_پای

    trie->معلومات_اوږدتیا=new_پیل;
}

ساكن فراغ
compactIndex2(UNewTrie2 *trie) {
    int32_t i, پیل, new_پیل, moved_پیل, overlap;

    /* do not compact linear-BMP index-2 blocks */
    new_پیل=UTRIE2_شاخص_2_BMP_اوږدتیا;
    لكل(پیل=0, i=0; پیل<new_پیل; پیل+=UTRIE2_شاخص_2_بلاک_اوږدتیا, ++i) {
        trie->map[i]=پیل;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    new_پیل+=UTRIE2_UTF8_2_B_شاخص_2_اوږدتیا+((trie->لوړ_پیل-0x10000)>>UTRIE2_ښويدنه_1);

    لكل(پیل=UNEWTRIE2_شاخص_2_NULL_دننه_واټن; پیل<trie->index2_اوږدتیا;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        إذا( (moved_پیل=پیدا_کړه_Same_Index2_بلاک(trie->index2, new_پیل, پیل))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[پیل>>UTRIE2_ښويدنه_1_2]=moved_پیل;

            /* advance start to the next block */
            پیل+=UTRIE2_شاخص_2_بلاک_اوږدتیا;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        لكل(overlap=UTRIE2_شاخص_2_بلاک_اوږدتیا-1;
            overlap>0 && !equal_int32(trie->index2+(new_پیل-overlap), trie->index2+پیل, overlap);
            --overlap) {}

        إذا(overlap>0 || new_پیل<پیل) {
            /* some overlap, or just move the whole block */
            trie->map[پیل>>UTRIE2_ښويدنه_1_2]=new_پیل-overlap;

            /* move the non-overlapping indexes to their new positions */
            پیل+=overlap;
            لكل(i=UTRIE2_شاخص_2_بلاک_اوږدتیا-overlap; i>0; --i) {
                trie->index2[new_پیل++]=trie->index2[پیل++];
            }
        } وإلا /* no overlap && newStart==start */ {
            trie->map[پیل>>UTRIE2_ښويدنه_1_2]=پیل;
            پیل+=UTRIE2_شاخص_2_بلاک_اوږدتیا;
            new_پیل=پیل;
        }
    }

    /* now adjust the index-1 table */
    لكل(i=0; i<UNEWTRIE2_شاخص_1_اوږدتیا; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_ښويدنه_1_2];
    }
    trie->index2_Null_دننه_واټن=trie->map[trie->index2_Null_دننه_واټن>>UTRIE2_ښويدنه_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    طالما((new_پیل&((UTRIE2_معلومات_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[new_پیل++] = static_cast<int32_t>(0xffff) << UTRIE2_شاخص_ښويدنه;
    }

#که_تعریف_شوی UTRIE2_DEBUG
    /* we saved some space */
    چاپ_کړه("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (اوږد)trie->index2_اوږدتیا, (اوږد)new_پیل);
#د_که_پای

    trie->index2_اوږدتیا=new_پیل;
}

ساكن فراغ
compactTrie(UTrie2 *trie, UError_کوډ *p_Error_کوډ) {
    UNewTrie2 *newTrie;
    UChar32 لوړ_پیل, supp_لوړ_پیل;
    uint32_t لوړ_ارزښت;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    لوړ_ارزښت=utrie2_get32(trie, 0x10ffff);
    لوړ_پیل=پیدا_کړه_لوړ_پیل(newTrie, لوړ_ارزښت);
    لوړ_پیل=(لوړ_پیل+(UTRIE2_CP_فی_شاخص_1_ثبت-1))&~(UTRIE2_CP_فی_شاخص_1_ثبت-1);
    إذا(لوړ_پیل==0x110000) {
        لوړ_ارزښت=trie->error_ارزښت;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->لوړ_پیل=newTrie->لوړ_پیل=لوړ_پیل;

#که_تعریف_شوی UTRIE2_DEBUG
    چاپ_کړه("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (اوږد)لوړ_پیل, (اوږد)لوړ_ارزښت, (اوږد)trie->initial_ارزښت);
#د_که_پای

    إذا(لوړ_پیل<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_لوړ_پیل= لوړ_پیل<=0x10000 ? 0x10000 : لوړ_پیل;
        utrie2_سېټ_Range32(trie, supp_لوړ_پیل, 0x10ffff, trie->initial_ارزښت, ریښتیا, p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            أرجع;
        }
    }

    compact_معلومات(newTrie);
    إذا(لوړ_پیل>0x10000) {
        compactIndex2(newTrie);
#که_تعریف_شوی UTRIE2_DEBUG
    } وإلا {
        چاپ_کړه("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (اوږد)لوړ_پیل, (اوږد)trie->newTrie->index2_اوږدتیا, (اوږد)UTRIE2_شاخص_1_دننه_واټن);
#د_که_پای
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->معلومات[newTrie->معلومات_اوږدتیا++]=لوړ_ارزښت;
    طالما((newTrie->معلومات_اوږدتیا&(UTRIE2_معلومات_GRANULARITY-1))!=0) {
        newTrie->معلومات[newTrie->معلومات_اوږدتیا++]=trie->initial_ارزښت;
    }

    newTrie->isCompacted=ریښتیا;
}

/* serialization ------------------------------------------------------------ */

/**
 * Maximum length of the runtime index array.
 * Limited by its own 16-bit index values, and by uint16_t UTrie2Header.indexLength.
 * (The actual maximum length is lower,
 * (0x110000>>UTRIE2_SHIFT_2)+UTRIE2_UTF8_2B_INDEX_2_LENGTH+UTRIE2_MAX_INDEX_1_LENGTH.)
 */
#تعریف_کړه UTRIE2_MAX_شاخص_اوږدتیا 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#تعریف_کړه UTRIE2_MAX_معلومات_اوږدتیا (0xffff<<UTRIE2_شاخص_ښويدنه)

/* Compact and internally serialize the trie. */
U_CAPI فراغ U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_ارزښت_بټونه ارزښت_بټونه, UError_کوډ *p_Error_کوډ) {
    UNewTrie2 *newTrie;
    UTrie2_سرلیک *سرلیک;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_اوږدتیا;
    int32_t معلومات_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 لوړ_پیل;

    /* argument check */
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    إذا( trie==تش_اشاره ||
        ارزښت_بټونه<0 || UTRIE2_شمېر_ارزښت_بټونه<=ارزښت_بټونه
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==تش_اشاره) {
        /* already frozen */
        UTrie2_ارزښت_بټونه frozen_ارزښت_بټونه=
            trie->data16!=تش_اشاره ? UTRIE2_16_ارزښت_بټونه : UTRIE2_32_ارزښت_بټونه;
        إذا(ارزښت_بټونه!=frozen_ارزښت_بټونه) {
            *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        }
        أرجع;
    }

    /* compact if necessary */
    إذا(!newTrie->isCompacted) {
        compactTrie(trie, p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            أرجع;
        }
    }
    لوړ_پیل=trie->لوړ_پیل;

    إذا(لوړ_پیل<=0x10000) {
        all_Indexes_اوږدتیا=UTRIE2_شاخص_1_دننه_واټن;
    } وإلا {
        all_Indexes_اوږدتیا=newTrie->index2_اوږدتیا;
    }
    إذا(ارزښت_بټونه==UTRIE2_16_ارزښت_بټونه) {
        معلومات_Move=all_Indexes_اوږدتیا;
    } وإلا {
        معلومات_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    إذا( /* for unshifted indexLength */
        all_Indexes_اوږدتیا>UTRIE2_MAX_شاخص_اوږدتیا ||
        /* for unshifted dataNullOffset */
        (معلومات_Move+newTrie->معلومات_Null_دننه_واټن)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (معلومات_Move+UNEWTRIE2_معلومات_0800_دننه_واټن)>0xffff ||
        /* for shiftedDataLength */
        (معلومات_Move+newTrie->معلومات_اوږدتیا)>UTRIE2_MAX_معلومات_اوږدتیا
    ) {
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    /* calculate the total serialized length */
    length=حجم(UTrie2_سرلیک)+all_Indexes_اوږدتیا*2;
    إذا(ارزښت_بټونه==UTRIE2_16_ارزښت_بټونه) {
        length+=newTrie->معلومات_اوږدتیا*2;
    } وإلا {
        length+=newTrie->معلومات_اوږدتیا*4;
    }

    trie->حافظه=uprv_malloc(length);
    إذا(trie->حافظه==تش_اشاره) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع;
    }
    trie->length=length;
    trie->is_حافظه_Owned=ریښتیا;

    trie->شاخص_اوږدتیا=all_Indexes_اوږدتیا;
    trie->معلومات_اوږدتیا=newTrie->معلومات_اوږدتیا;
    إذا(لوړ_پیل<=0x10000) {
        trie->index2_Null_دننه_واټن=0xffff;
    } وإلا {
        trie->index2_Null_دننه_واټن=static_cast<uint16_t>(UTRIE2_شاخص_2_دننه_واټن+newTrie->index2_Null_دننه_واټن);
    }
    trie->معلومات_Null_دننه_واټن=(uint16_t)(معلومات_Move+newTrie->معلومات_Null_دننه_واټن);
    trie->لوړ_ارزښت_شاخص=معلومات_Move+trie->معلومات_اوږدتیا-UTRIE2_معلومات_GRANULARITY;

    /* set the header fields */
    سرلیک=(UTrie2_سرلیک *)trie->حافظه;

    سرلیک->signature=UTRIE2_SIG; /* "Tri2" */
    سرلیک->options=(uint16_t)ارزښت_بټونه;

    سرلیک->شاخص_اوږدتیا=(uint16_t)trie->شاخص_اوږدتیا;
    سرلیک->shifted_معلومات_اوږدتیا=(uint16_t)(trie->معلومات_اوږدتیا>>UTRIE2_شاخص_ښويدنه);
    سرلیک->index2_Null_دننه_واټن=trie->index2_Null_دننه_واټن;
    سرلیک->معلومات_Null_دننه_واټن=trie->معلومات_Null_دننه_واټن;
    سرلیک->shifted_لوړ_پیل=(uint16_t)(لوړ_پیل>>UTRIE2_ښويدنه_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرلیک+1);
    trie->شاخص=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    لكل(i=UTRIE2_شاخص_2_BMP_اوږدتیا; i>0; --i) {
        *dest16++=(uint16_t)((معلومات_Move + *p++)>>UTRIE2_شاخص_ښويدنه);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(معلومات_Move+UTRIE2_BAD_UTF8_معلومات_دننه_واټن);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(معلومات_Move+newTrie->index2[i<<(6-UTRIE2_ښويدنه_2)]);
    }

    إذا(لوړ_پیل>0x10000) {
        int32_t index1_اوږدتیا=(لوړ_پیل-0x10000)>>UTRIE2_ښويدنه_1;
        int32_t index2_دننه_واټن=UTRIE2_شاخص_2_BMP_اوږدتیا+UTRIE2_UTF8_2_B_شاخص_2_اوږدتیا+index1_اوږدتیا;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_شاخص_1_اوږدتیا;
        لكل(i=index1_اوږدتیا; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_شاخص_2_دننه_واټن + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2_دننه_واټن;
        لكل(i=newTrie->index2_اوږدتیا-index2_دننه_واټن; i>0; --i) {
            *dest16++=(uint16_t)((معلومات_Move + *p++)>>UTRIE2_شاخص_ښويدنه);
        }
    }

    /* write the 16/32-bit data array */
    بدل(ارزښت_بټونه) {
    حالة UTRIE2_16_ارزښت_بټونه:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=تش_اشاره;
        p=newTrie->معلومات;
        لكل(i=newTrie->معلومات_اوږدتیا; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        اكسر;
    حالة UTRIE2_32_ارزښت_بټونه:
        /* write 32-bit data values */
        trie->data16=تش_اشاره;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->معلومات, (size_t)newTrie->معلومات_اوږدتیا*4);
        اكسر;
    افتراضي:
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

#که_تعریف_شوی UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#د_که_پای

#که_تعریف_شوی UCPTRIE_DEBUG
    umutablecptrie_سېټ_Name(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_ارزښت_پلنوالی)ارزښت_بټونه, p_Error_کوډ));
#د_که_پای
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->معلومات);
    uprv_free(newTrie);
    trie->newTrie=تش_اشاره;
}

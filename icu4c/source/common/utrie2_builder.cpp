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
#ifdef UTRIE2_DEBUG
#   include <stdio.h>
#endif
// #define UCPTRIE_DEBUG

#অন্তর্ভাব "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#অন্তর্ভাব "unicode/ucptrie.h"
#অন্তর্ভাব "unicode/umutablecptrie.h"
#অন্তর্ভাব "ucptrie_impl.h"
#endif
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "utrie2.h"
#অন্তর্ভাব "utrie2_impl.h"

#অন্তর্ভাব "utrie.h"  // for utrie2_fromUTrie()

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

গণনা {
    /** The null index-2 block, following the gap in the index-2 table. */
    UNEWTRIE2_সূচক_2_NULL_OFFSET=UNEWTRIE2_সূচক_GAP_OFFSET+UNEWTRIE2_সূচক_GAP_দৈর্ঘ্য,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_সূচক_2_START_OFFSET=UNEWTRIE2_সূচক_2_NULL_OFFSET+UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_তথ্য_NULL_OFFSET=UTRIE2_তথ্য_START_OFFSET,

    /** The start of allocated data blocks. */
    UNEWTRIE2_তথ্য_START_OFFSET=UNEWTRIE2_তথ্য_NULL_OFFSET+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_তথ্য_0800_OFFSET=UNEWTRIE2_তথ্য_START_OFFSET+0x780
};

/* Start with allocation of 16k data entries. */
#define UNEWTRIE2_INITIAL_তথ্য_দৈর্ঘ্য ((int32_t)1<<14)

/* Grow about 8x each time. */
#define UNEWTRIE2_MEDIUM_তথ্য_দৈর্ঘ্য ((int32_t)1<<17)

স্থির int32_t
allocIndex2Block(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_মান, uint32_t error_মান, UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *তথ্য;
    int32_t i, j;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(আকার(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(আকার(UNewTrie2));
    তথ্য=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_তথ্য_দৈর্ঘ্য*4);
    যদি(trie==nullptr || newTrie==nullptr || তথ্য==nullptr) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(তথ্য);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }

    uprv_memset(trie, 0, আকার(UTrie2));
    trie->initial_মান=initial_মান;
    trie->error_মান=error_মান;
    trie->উপরের_Start=0x110000;
    trie->newTrie=newTrie;
#ifdef UTRIE2_DEBUG
    trie->name="open";
#endif

    newTrie->তথ্য=তথ্য;
#ifdef UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_মান, error_মান, pErrorCode);
#endif
    newTrie->তথ্য_Capacity=UNEWTRIE2_INITIAL_তথ্য_দৈর্ঘ্য;
    newTrie->initial_মান=initial_মান;
    newTrie->error_মান=error_মান;
    newTrie->উপরের_Start=0x110000;
    newTrie->firstFreeBlock=0;  /* no free block in the list */
    newTrie->isCompacted=মিথ্যা;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    জন্য(i=0; i<0x80; ++i) {
        newTrie->তথ্য[i]=initial_মান;
    }
    জন্য(; i<0xc0; ++i) {
        newTrie->তথ্য[i]=error_মান;
    }
    জন্য(i=UNEWTRIE2_তথ্য_NULL_OFFSET; i<UNEWTRIE2_তথ্য_START_OFFSET; ++i) {
        newTrie->তথ্য[i]=initial_মান;
    }
    newTrie->তথ্য_Null_Offset=UNEWTRIE2_তথ্য_NULL_OFFSET;
    newTrie->তথ্য_দৈর্ঘ্য=UNEWTRIE2_তথ্য_START_OFFSET;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    জন্য(i=0, j=0; j<0x80; ++i, j+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    জন্য(; j<0xc0; ++i, j+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_SHIFT_2)-
        (0x80>>UTRIE2_SHIFT_2)+
        1+
        UTRIE2_LSCP_সূচক_2_দৈর্ঘ্য;
    j+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
    জন্য(; j<UNEWTRIE2_তথ্য_START_OFFSET; ++i, j+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    জন্য(i=0x80>>UTRIE2_SHIFT_2; i<UTRIE2_সূচক_2_BMP_দৈর্ঘ্য; ++i) {
        newTrie->index2[i]=UNEWTRIE2_তথ্য_NULL_OFFSET;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    জন্য(i=0; i<UNEWTRIE2_সূচক_GAP_দৈর্ঘ্য; ++i) {
        newTrie->index2[UNEWTRIE2_সূচক_GAP_OFFSET+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    জন্য(i=0; i<UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য; ++i) {
        newTrie->index2[UNEWTRIE2_সূচক_2_NULL_OFFSET+i]=UNEWTRIE2_তথ্য_NULL_OFFSET;
    }
    newTrie->index2NullOffset=UNEWTRIE2_সূচক_2_NULL_OFFSET;
    newTrie->index2_দৈর্ঘ্য=UNEWTRIE2_সূচক_2_START_OFFSET;

    /* set the index-1 indexes for the linear index-2 block */
    জন্য(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_সূচক_1_দৈর্ঘ্য;
        ++i, j+=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    জন্য(; i<UNEWTRIE2_সূচক_1_দৈর্ঘ্য; ++i) {
        newTrie->index1[i]=UNEWTRIE2_সূচক_2_NULL_OFFSET;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    জন্য(i=0x80; i<0x800; i+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য) {
        utrie2_set32(trie, i, initial_মান, pErrorCode);
    }

    ফেরত trie;
}

স্থির UNewTrie2 *
cloneBuilder(ধ্রুবক UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(আকার(UNewTrie2)));
    যদি(trie==nullptr) {
        ফেরত nullptr;
    }

    trie->তথ্য = static_cast<uint32_t*>(uprv_malloc(other->তথ্য_Capacity * 4));
    যদি(trie->তথ্য==nullptr) {
        uprv_free(trie);
        ফেরত nullptr;
    }
#ifdef UCPTRIE_DEBUG
    যদি(other->t3==nullptr) {
        trie->t3=nullptr;
    } অন্যথা {
        UErrorCode errorCode=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &errorCode);
    }
#endif
    trie->তথ্য_Capacity=other->তথ্য_Capacity;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, আকার(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_দৈর্ঘ্য*4);
    trie->index2NullOffset=other->index2NullOffset;
    trie->index2_দৈর্ঘ্য=other->index2_দৈর্ঘ্য;

    uprv_memcpy(trie->তথ্য, other->তথ্য, (size_t)other->তথ্য_দৈর্ঘ্য*4);
    trie->তথ্য_Null_Offset=other->তথ্য_Null_Offset;
    trie->তথ্য_দৈর্ঘ্য=other->তথ্য_দৈর্ঘ্য;

    /* reference counters */
    যদি(other->isCompacted) {
        trie->firstFreeBlock=0;
    } অন্যথা {
        uprv_memcpy(trie->map, other->map, ((size_t)other->তথ্য_দৈর্ঘ্য>>UTRIE2_SHIFT_2)*4);
        trie->firstFreeBlock=other->firstFreeBlock;
    }

    trie->initial_মান=other->initial_মান;
    trie->error_মান=other->error_মান;
    trie->উপরের_Start=other->উপরের_Start;
    trie->isCompacted=other->isCompacted;

    ফেরত trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ধ্রুবক UTrie2 *other, UErrorCode *pErrorCode) {
    UTrie2 *trie;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(আকার(UTrie2));
    যদি(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memcpy(trie, other, আকার(UTrie2));

    যদি(other->memory!=nullptr) {
        trie->memory=uprv_malloc(other->length);
        যদি(trie->memory!=nullptr) {
            trie->isMemoryOwned=সত্য;
            uprv_memcpy(trie->memory, other->memory, other->length);

            /* make the clone's pointers point to its own memory */
            trie->সূচক=(uint16_t *)trie->memory+(other->সূচক-(uint16_t *)other->memory);
            যদি(other->data16!=nullptr) {
                trie->data16=(uint16_t *)trie->memory+(other->data16-(uint16_t *)other->memory);
            }
            যদি(other->data32!=nullptr) {
                trie->data32=(uint32_t *)trie->memory+(other->data32-(uint32_t *)other->memory);
            }
        }
    } অন্যথা /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    যদি(trie->memory==nullptr && trie->newTrie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        uprv_free(trie);
        trie=nullptr;
    }
    ফেরত trie;
}

প্রকারলেখ কাঠামো NewTrieAndStatus {
    UTrie2 *trie;
    UErrorCode errorCode;
    UBool exclusiveLimit;  /* rather than inclusive range end */
} NewTrieAndStatus;

স্থির UBool U_CALLCONV
copyEnumRange(ধ্রুবক শূন্য *context, UChar32 start, UChar32 end, uint32_t value) {
    NewTrieAndStatus *nt=(NewTrieAndStatus *)context;
    যদি(value!=nt->trie->initial_মান) {
        যদি(nt->exclusiveLimit) {
            --end;
        }
        যদি(start==end) {
            utrie2_set32(nt->trie, start, value, &nt->errorCode);
        } অন্যথা {
            utrie2_setRange32(nt->trie, start, end, value, সত্য, &nt->errorCode);
        }
        ফেরত U_SUCCESS(nt->errorCode);
    } অন্যথা {
        ফেরত সত্য;
    }
}

#ifdef UTRIE2_DEBUG
স্থির long গুনতি_Initial(ধ্রুবক UTrie2 *trie) {
    uint32_t initial_মান=trie->initial_মান;
    int32_t length=trie->তথ্য_দৈর্ঘ্য;
    long count=0;
    যদি(trie->data16!=nullptr) {
        জন্য(int32_t i=0; i<length; ++i) {
            যদি(trie->data16[i]==initial_মান) { ++count; }
        }
    } অন্যথা {
        জন্য(int32_t i=0; i<length; ++i) {
            যদি(trie->data32[i]==initial_মান) { ++count; }
        }
    }
    ফেরত count;
}

স্থির শূন্য
utrie_printLengths(ধ্রুবক UTrie *trie) {
    long সূচক_দৈর্ঘ্য=trie->সূচক_দৈর্ঘ্য;
    long তথ্য_দৈর্ঘ্য=(long)trie->তথ্য_দৈর্ঘ্য;
    long যোগ_দৈর্ঘ্য=(long)আকার(UTrieHeader)+সূচক_দৈর্ঘ্য*2+তথ্য_দৈর্ঘ্য*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           সূচক_দৈর্ঘ্য, তথ্য_দৈর্ঘ্য, যোগ_দৈর্ঘ্য);
}

স্থির শূন্য
utrie2_printLengths(ধ্রুবক UTrie2 *trie, ধ্রুবক অক্ষর *which) {
    long সূচক_দৈর্ঘ্য=trie->সূচক_দৈর্ঘ্য;
    long তথ্য_দৈর্ঘ্য=(long)trie->তথ্য_দৈর্ঘ্য;
    long যোগ_দৈর্ঘ্য=(long)আকার(UTrie2Header)+সূচক_দৈর্ঘ্য*2+তথ্য_দৈর্ঘ্য*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, সূচক_দৈর্ঘ্য, তথ্য_দৈর্ঘ্য, গুনতি_Initial(trie), যোগ_দৈর্ঘ্য);
}
#endif

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ধ্রুবক UTrie2 *other, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    যদি(other->newTrie!=nullptr && !other->newTrie->isCompacted) {
        ফেরত utrie2_clone(other, pErrorCode);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_মান, other->error_মান, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    context.exclusiveLimit=মিথ্যা;
    context.errorCode=*pErrorCode;
    utrie2_enum(other, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    জন্য(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        যদি(other->data32==nullptr) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } অন্যথা {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        যদি(value!=other->initial_মান) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    যদি(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    ফেরত context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ধ্রুবক UTrie *trie1, uint32_t error_মান, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    যদি(trie1==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    context.trie=utrie2_open(trie1->initial_মান, error_মান, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }
    context.exclusiveLimit=সত্য;
    context.errorCode=*pErrorCode;
    utrie_enum(trie1, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    জন্য(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        যদি(trie1->data32==nullptr) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } অন্যথা {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        যদি(value!=trie1->initial_মান) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    যদি(U_SUCCESS(*pErrorCode)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=nullptr ? UTRIE2_32_মান_BITS : UTRIE2_16_মান_BITS,
                      pErrorCode);
    }
#ifdef UTRIE2_DEBUG
    যদি(U_SUCCESS(*pErrorCode)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#endif
    যদি(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    ফেরত context.trie;
}

স্থির অন্তঃরেখ UBool
isInNullBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, block;

    যদি(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_সূচক_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } অন্যথা {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_সূচক_2_MASK);
    }
    block=trie->index2[i2];
    ফেরত block == trie->তথ্য_Null_Offset;
}

স্থির int32_t
allocIndex2Block(UNewTrie2 *trie) {
    int32_t newBlock, new_শীর্ষ;

    newBlock=trie->index2_দৈর্ঘ্য;
    new_শীর্ষ=newBlock+UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য;
    যদি(new_শীর্ষ>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        ফেরত -1;
    }
    trie->index2_দৈর্ঘ্য=new_শীর্ষ;
    uprv_memcpy(trie->index2+newBlock, trie->index2+trie->index2NullOffset, UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য*4);
    ফেরত newBlock;
}

স্থির int32_t
getIndex2Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    যদি(U_IS_LEAD(c) && forLSCP) {
        ফেরত UTRIE2_LSCP_সূচক_2_OFFSET;
    }

    i1=c>>UTRIE2_SHIFT_1;
    i2=trie->index1[i1];
    যদি(i2==trie->index2NullOffset) {
        i2=allocIndex2Block(trie);
        যদি(i2<0) {
            ফেরত -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    ফেরত i2;
}

স্থির int32_t
alloc_তথ্য_Block(UNewTrie2 *trie, int32_t copyBlock) {
    int32_t newBlock, new_শীর্ষ;

    যদি(trie->firstFreeBlock!=0) {
        /* get the first free block */
        newBlock=trie->firstFreeBlock;
        trie->firstFreeBlock=-trie->map[newBlock>>UTRIE2_SHIFT_2];
    } অন্যথা {
        /* get a new block from the high end */
        newBlock=trie->তথ্য_দৈর্ঘ্য;
        new_শীর্ষ=newBlock+UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
        যদি(new_শীর্ষ>trie->তথ্য_Capacity) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *তথ্য;

            যদি(trie->তথ্য_Capacity<UNEWTRIE2_MEDIUM_তথ্য_দৈর্ঘ্য) {
                capacity=UNEWTRIE2_MEDIUM_তথ্য_দৈর্ঘ্য;
            } অন্যথা যদি(trie->তথ্য_Capacity<UNEWTRIE2_MAX_তথ্য_দৈর্ঘ্য) {
                capacity=UNEWTRIE2_MAX_তথ্য_দৈর্ঘ্য;
            } অন্যথা {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                ফেরত -1;
            }
            তথ্য = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            যদি(তথ্য==nullptr) {
                ফেরত -1;
            }
            uprv_memcpy(তথ্য, trie->তথ্য, (size_t)trie->তথ্য_দৈর্ঘ্য*4);
            uprv_free(trie->তথ্য);
            trie->তথ্য=তথ্য;
            trie->তথ্য_Capacity=capacity;
        }
        trie->তথ্য_দৈর্ঘ্য=new_শীর্ষ;
    }
    uprv_memcpy(trie->তথ্য+newBlock, trie->তথ্য+copyBlock, UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য*4);
    trie->map[newBlock>>UTRIE2_SHIFT_2]=0;
    ফেরত newBlock;
}

/* call when the block's reference counter reaches 0 */
স্থির শূন্য
release_তথ্য_Block(UNewTrie2 *trie, int32_t block) {
    /* put this block at the front of the free-block chain */
    trie->map[block>>UTRIE2_SHIFT_2]=-trie->firstFreeBlock;
    trie->firstFreeBlock=block;
}

স্থির অন্তঃরেখ UBool
isWritableBlock(UNewTrie2 *trie, int32_t block) {
    ফেরত block != trie->তথ্য_Null_Offset && 1 == trie->map[block >> UTRIE2_SHIFT_2];
}

স্থির অন্তঃরেখ শূন্য
setIndex2Entry(UNewTrie2 *trie, int32_t i2, int32_t block) {
    int32_t oldBlock;
    ++trie->map[block>>UTRIE2_SHIFT_2];  /* increment first, in case block==oldBlock! */
    oldBlock=trie->index2[i2];
    যদি(0 == --trie->map[oldBlock>>UTRIE2_SHIFT_2]) {
        release_তথ্য_Block(trie, oldBlock);
    }
    trie->index2[i2]=block;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
স্থির int32_t
get_তথ্য_Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, oldBlock, newBlock;

    i2=getIndex2Block(trie, c, forLSCP);
    যদি(i2<0) {
        ফেরত -1;  /* program error */
    }

    i2+=(c>>UTRIE2_SHIFT_2)&UTRIE2_সূচক_2_MASK;
    oldBlock=trie->index2[i2];
    যদি(isWritableBlock(trie, oldBlock)) {
        ফেরত oldBlock;
    }

    /* allocate a new data block */
    newBlock=alloc_তথ্য_Block(trie, oldBlock);
    যদি(newBlock<0) {
        /* out of memory in the data array */
        ফেরত -1;
    }
    setIndex2Entry(trie, i2, newBlock);
    ফেরত newBlock;
}

/**
 * @return true if the value was successfully set
 */
স্থির শূন্য
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UErrorCode *pErrorCode) {
    int32_t block;

    যদি(trie==nullptr || trie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        ফেরত;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_set(trie->t3, c, value, pErrorCode);
#endif

    block=get_তথ্য_Block(trie, c, forLSCP);
    যদি(block<0) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    trie->তথ্য[block+(c&UTRIE2_তথ্য_MASK)]=value;
}

U_CAPI শূন্য U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    যদি((uint32_t)c>0x10ffff) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    set32(trie->newTrie, c, সত্য, value, pErrorCode);
}

U_CAPI শূন্য U_EXPORT2
utrie2_set32ForLeadSurrogateCodeUnit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    যদি(!U_IS_LEAD(c)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    set32(trie->newTrie, c, মিথ্যা, value, pErrorCode);
}

স্থির শূন্য
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit=block+UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
    যখন(block<limit) {
        *block++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
স্থির শূন্য
fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
          uint32_t value, uint32_t initial_মান, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    যদি(overwrite) {
        যখন(block<pLimit) {
            *block++=value;
        }
    } অন্যথা {
        যখন(block<pLimit) {
            যদি(*block==initial_মান) {
                *block=value;
            }
            ++block;
        }
    }
}

U_CAPI শূন্য U_EXPORT2
utrie2_setRange32(UTrie2 *trie,
                  UChar32 start, UChar32 end,
                  uint32_t value, UBool overwrite,
                  UErrorCode *pErrorCode) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t block, rest, repeatBlock;
    UChar32 limit;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    যদি((uint32_t)start>0x10ffff || (uint32_t)end>0x10ffff || start>end) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    newTrie=trie->newTrie;
    যদি(newTrie==nullptr || newTrie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        ফেরত;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_setRange(newTrie->t3, start, end, value, pErrorCode);
#endif
    যদি(!overwrite && value==newTrie->initial_মান) {
        ফেরত; /* nothing to do */
    }

    limit=end+1;
    যদি(start&UTRIE2_তথ্য_MASK) {
        UChar32 পরবর্তী_Start;

        /* set partial block at [start..following block boundary[ */
        block=get_তথ্য_Block(newTrie, start, সত্য);
        যদি(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }

        পরবর্তী_Start=(start+UTRIE2_তথ্য_MASK)&~UTRIE2_তথ্য_MASK;
        যদি(পরবর্তী_Start<=limit) {
            fillBlock(newTrie->তথ্য+block, start&UTRIE2_তথ্য_MASK, UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য,
                      value, newTrie->initial_মান, overwrite);
            start=পরবর্তী_Start;
        } অন্যথা {
            fillBlock(newTrie->তথ্য+block, start&UTRIE2_তথ্য_MASK, limit&UTRIE2_তথ্য_MASK,
                      value, newTrie->initial_মান, overwrite);
            ফেরত;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE2_তথ্য_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE2_তথ্য_MASK;

    /* iterate over all-value blocks */
    যদি(value==newTrie->initial_মান) {
        repeatBlock=newTrie->তথ্য_Null_Offset;
    } অন্যথা {
        repeatBlock=-1;
    }

    যখন(start<limit) {
        int32_t i2;
        UBool setRepeatBlock=মিথ্যা;

        যদি(value==newTrie->initial_মান && isInNullBlock(newTrie, start, সত্য)) {
            start+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য; /* nothing to do */
            চালু;
        }

        /* get index value */
        i2=getIndex2Block(newTrie, start, সত্য);
        যদি(i2<0) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            ফেরত;
        }
        i2+=(start>>UTRIE2_SHIFT_2)&UTRIE2_সূচক_2_MASK;
        block=newTrie->index2[i2];
        যদি(isWritableBlock(newTrie, block)) {
            /* already allocated */
            যদি(overwrite && block>=UNEWTRIE2_তথ্য_0800_OFFSET) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                setRepeatBlock=সত্য;
            } অন্যথা {
                /* !overwrite, or protected block: just write the values into this block */
                fillBlock(newTrie->তথ্য+block,
                          0, UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য,
                          value, newTrie->initial_মান, overwrite);
            }
        } অন্যথা যদি(newTrie->তথ্য[block]!=value && (overwrite || block==newTrie->তথ্য_Null_Offset)) {
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
            setRepeatBlock=সত্য;
        }
        যদি(setRepeatBlock) {
            যদি(repeatBlock>=0) {
                setIndex2Entry(newTrie, i2, repeatBlock);
            } অন্যথা {
                /* create and set and fill the repeatBlock */
                repeatBlock=get_তথ্য_Block(newTrie, start, সত্য);
                যদি(repeatBlock<0) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    ফেরত;
                }
                writeBlock(newTrie->তথ্য+repeatBlock, value);
            }
        }

        start+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
    }

    যদি(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=get_তথ্য_Block(newTrie, start, সত্য);
        যদি(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }

        fillBlock(newTrie->তথ্য+block, 0, rest, value, newTrie->initial_মান, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

স্থির অন্তঃরেখ UBool
equal_int32(ধ্রুবক int32_t *s, ধ্রুবক int32_t *t, int32_t length) {
    যখন(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ফেরত length == 0;
}

স্থির অন্তঃরেখ UBool
equal_uint32(ধ্রুবক uint32_t *s, ধ্রুবক uint32_t *t, int32_t length) {
    যখন(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ফেরত length == 0;
}

স্থির int32_t
findSameIndex2Block(ধ্রুবক int32_t *idx, int32_t index2_দৈর্ঘ্য, int32_t otherBlock) {
    int32_t block;

    /* ensure that we do not even partially get past index2Length */
    index2_দৈর্ঘ্য-=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য;

    জন্য(block=0; block<=index2_দৈর্ঘ্য; ++block) {
        যদি(equal_int32(idx+block, idx+otherBlock, UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য)) {
            ফেরত block;
        }
    }
    ফেরত -1;
}

স্থির int32_t
find_Same_তথ্য_Block_2(ধ্রুবক uint32_t *তথ্য, int32_t তথ্য_দৈর্ঘ্য, int32_t otherBlock, int32_t block_দৈর্ঘ্য) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    তথ্য_দৈর্ঘ্য-=block_দৈর্ঘ্য;

    জন্য(block=0; block<=তথ্য_দৈর্ঘ্য; block+=UTRIE2_তথ্য_GRANULARITY) {
        যদি(equal_uint32(তথ্য+block, তথ্য+otherBlock, block_দৈর্ঘ্য)) {
            ফেরত block;
        }
    }
    ফেরত -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
স্থির UChar32
find_উপরের_Start(UNewTrie2 *trie, uint32_t উপরের_মান) {
    ধ্রুবক uint32_t *data32;

    uint32_t value, initial_মান;
    UChar32 c, পূর্ব;
    int32_t i1, i2, j, i2Block, পূর্ব_I2_Block, index2NullOffset, block, পূর্ব_Block, nullBlock;

    data32=trie->তথ্য;
    initial_মান=trie->initial_মান;

    index2NullOffset=trie->index2NullOffset;
    nullBlock=trie->তথ্য_Null_Offset;

    /* set variables for previous range */
    যদি(উপরের_মান==initial_মান) {
        পূর্ব_I2_Block=index2NullOffset;
        পূর্ব_Block=nullBlock;
    } অন্যথা {
        পূর্ব_I2_Block=-1;
        পূর্ব_Block=-1;
    }
    পূর্ব=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_সূচক_1_দৈর্ঘ্য;
    c=পূর্ব;
    যখন(c>0) {
        i2Block=trie->index1[--i1];
        যদি(i2Block==পূর্ব_I2_Block) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_PER_সূচক_1_ENTRY;
            চালু;
        }
        পূর্ব_I2_Block=i2Block;
        যদি(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            যদি(উপরের_মান!=initial_মান) {
                ফেরত c;
            }
            c-=UTRIE2_CP_PER_সূচক_1_ENTRY;
        } অন্যথা {
            /* enumerate data blocks for one index-2 block */
            জন্য(i2=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য; i2>0;) {
                block=trie->index2[i2Block+ --i2];
                যদি(block==পূর্ব_Block) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
                    চালু;
                }
                পূর্ব_Block=block;
                যদি(block==nullBlock) {
                    /* this is the null data block */
                    যদি(উপরের_মান!=initial_মান) {
                        ফেরত c;
                    }
                    c-=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
                } অন্যথা {
                    জন্য(j=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য; j>0;) {
                        value=data32[block+ --j];
                        যদি(value!=উপরের_মান) {
                            ফেরত c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    ফেরত 0;
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
স্থির শূন্য
compact_তথ্য(UNewTrie2 *trie) {
#ifdef UTRIE2_DEBUG
    int32_t গুনতি_Same=0, যোগ_Overlaps=0;
#endif

    int32_t start, newStart, movedStart;
    int32_t block_দৈর্ঘ্য, overlap;
    int32_t i, map_সূচক, block_গুনতি;

    /* do not compact linear-ASCII data */
    newStart=UTRIE2_তথ্য_START_OFFSET;
    জন্য(start=0, i=0; start<newStart; start+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য, ++i) {
        trie->map[i]=start;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    block_দৈর্ঘ্য=64;
    block_গুনতি=block_দৈর্ঘ্য>>UTRIE2_SHIFT_2;
    জন্য(start=newStart; start<trie->তথ্য_দৈর্ঘ্য;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        যদি(start==UNEWTRIE2_তথ্য_0800_OFFSET) {
            block_দৈর্ঘ্য=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
            block_গুনতি=1;
        }

        /* skip blocks that are not used */
        যদি(trie->map[start>>UTRIE2_SHIFT_2]<=0) {
            /* advance start to the next block */
            start+=block_দৈর্ঘ্য;

            /* leave newStart with the previous block! */
            চালু;
        }

        /* search for an identical block */
        যদি( (movedStart=find_Same_তথ্য_Block_2(trie->তথ্য, newStart, start, block_দৈর্ঘ্য))
             >=0
        ) {
#ifdef UTRIE2_DEBUG
            ++গুনতি_Same;
#endif
            /* found an identical block, set the other block's index value for the current block */
            জন্য(i=block_গুনতি, map_সূচক=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[map_সূচক++]=movedStart;
                movedStart+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
            }

            /* advance start to the next block */
            start+=block_দৈর্ঘ্য;

            /* leave newStart with the previous block! */
            চালু;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        জন্য(overlap=block_দৈর্ঘ্য-UTRIE2_তথ্য_GRANULARITY;
            overlap>0 && !equal_uint32(trie->তথ্য+(newStart-overlap), trie->তথ্য+start, overlap);
            overlap-=UTRIE2_তথ্য_GRANULARITY) {}

#ifdef UTRIE2_DEBUG
            যোগ_Overlaps+=overlap;
#endif
        যদি(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            movedStart=newStart-overlap;
            জন্য(i=block_গুনতি, map_সূচক=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[map_সূচক++]=movedStart;
                movedStart+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
            }

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            জন্য(i=block_দৈর্ঘ্য-overlap; i>0; --i) {
                trie->তথ্য[newStart++]=trie->তথ্য[start++];
            }
        } অন্যথা /* no overlap && newStart==start */ {
            জন্য(i=block_গুনতি, map_সূচক=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[map_সূচক++]=start;
                start+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
            }
            newStart=start;
        }
    }

    /* now adjust the index-2 table */
    জন্য(i=0; i<trie->index2_দৈর্ঘ্য; ++i) {
        যদি(i==UNEWTRIE2_সূচক_GAP_OFFSET) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_সূচক_GAP_দৈর্ঘ্য;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_SHIFT_2];
    }
    trie->তথ্য_Null_Offset=trie->map[trie->তথ্য_Null_Offset>>UTRIE2_SHIFT_2];

    /* ensure dataLength alignment */
    যখন((newStart&(UTRIE2_তথ্য_GRANULARITY-1))!=0) {
        trie->তথ্য[newStart++]=trie->initial_মান;
    }

#ifdef UTRIE2_DEBUG
    /* we saved some space */
    printf("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)trie->তথ্য_দৈর্ঘ্য, (long)newStart, (long)গুনতি_Same, (long)যোগ_Overlaps);
#endif

    trie->তথ্য_দৈর্ঘ্য=newStart;
}

স্থির শূন্য
compactIndex2(UNewTrie2 *trie) {
    int32_t i, start, newStart, movedStart, overlap;

    /* do not compact linear-BMP index-2 blocks */
    newStart=UTRIE2_সূচক_2_BMP_দৈর্ঘ্য;
    জন্য(start=0, i=0; start<newStart; start+=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য, ++i) {
        trie->map[i]=start;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    newStart+=UTRIE2_UTF8_2_B_সূচক_2_দৈর্ঘ্য+((trie->উপরের_Start-0x10000)>>UTRIE2_SHIFT_1);

    জন্য(start=UNEWTRIE2_সূচক_2_NULL_OFFSET; start<trie->index2_দৈর্ঘ্য;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        যদি( (movedStart=findSameIndex2Block(trie->index2, newStart, start))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=movedStart;

            /* advance start to the next block */
            start+=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য;

            /* leave newStart with the previous block! */
            চালু;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        জন্য(overlap=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য-1;
            overlap>0 && !equal_int32(trie->index2+(newStart-overlap), trie->index2+start, overlap);
            --overlap) {}

        যদি(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=newStart-overlap;

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            জন্য(i=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য-overlap; i>0; --i) {
                trie->index2[newStart++]=trie->index2[start++];
            }
        } অন্যথা /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE2_SHIFT_1_2]=start;
            start+=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য;
            newStart=start;
        }
    }

    /* now adjust the index-1 table */
    জন্য(i=0; i<UNEWTRIE2_সূচক_1_দৈর্ঘ্য; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_SHIFT_1_2];
    }
    trie->index2NullOffset=trie->map[trie->index2NullOffset>>UTRIE2_SHIFT_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    যখন((newStart&((UTRIE2_তথ্য_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[newStart++] = static_cast<int32_t>(0xffff) << UTRIE2_সূচক_SHIFT;
    }

#ifdef UTRIE2_DEBUG
    /* we saved some space */
    printf("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (long)trie->index2_দৈর্ঘ্য, (long)newStart);
#endif

    trie->index2_দৈর্ঘ্য=newStart;
}

স্থির শূন্য
compactTrie(UTrie2 *trie, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UChar32 উপরের_Start, supp_উপরের_Start;
    uint32_t উপরের_মান;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    উপরের_মান=utrie2_get32(trie, 0x10ffff);
    উপরের_Start=find_উপরের_Start(newTrie, উপরের_মান);
    উপরের_Start=(উপরের_Start+(UTRIE2_CP_PER_সূচক_1_ENTRY-1))&~(UTRIE2_CP_PER_সূচক_1_ENTRY-1);
    যদি(উপরের_Start==0x110000) {
        উপরের_মান=trie->error_মান;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->উপরের_Start=newTrie->উপরের_Start=উপরের_Start;

#ifdef UTRIE2_DEBUG
    printf("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)উপরের_Start, (long)উপরের_মান, (long)trie->initial_মান);
#endif

    যদি(উপরের_Start<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_উপরের_Start= উপরের_Start<=0x10000 ? 0x10000 : উপরের_Start;
        utrie2_setRange32(trie, supp_উপরের_Start, 0x10ffff, trie->initial_মান, সত্য, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            ফেরত;
        }
    }

    compact_তথ্য(newTrie);
    যদি(উপরের_Start>0x10000) {
        compactIndex2(newTrie);
#ifdef UTRIE2_DEBUG
    } অন্যথা {
        printf("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (long)উপরের_Start, (long)trie->newTrie->index2_দৈর্ঘ্য, (long)UTRIE2_সূচক_1_OFFSET);
#endif
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->তথ্য[newTrie->তথ্য_দৈর্ঘ্য++]=উপরের_মান;
    যখন((newTrie->তথ্য_দৈর্ঘ্য&(UTRIE2_তথ্য_GRANULARITY-1))!=0) {
        newTrie->তথ্য[newTrie->তথ্য_দৈর্ঘ্য++]=trie->initial_মান;
    }

    newTrie->isCompacted=সত্য;
}

/* serialization ------------------------------------------------------------ */

/**
 * Maximum length of the runtime index array.
 * Limited by its own 16-bit index values, and by uint16_t UTrie2Header.indexLength.
 * (The actual maximum length is lower,
 * (0x110000>>UTRIE2_SHIFT_2)+UTRIE2_UTF8_2B_INDEX_2_LENGTH+UTRIE2_MAX_INDEX_1_LENGTH.)
 */
#define UTRIE2_MAX_সূচক_দৈর্ঘ্য 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#define UTRIE2_MAX_তথ্য_দৈর্ঘ্য (0xffff<<UTRIE2_সূচক_SHIFT)

/* Compact and internally serialize the trie. */
U_CAPI শূন্য U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_মান_Bits মান_Bits, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_দৈর্ঘ্য;
    int32_t তথ্য_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 উপরের_Start;

    /* argument check */
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত;
    }
    যদি( trie==nullptr ||
        মান_Bits<0 || UTRIE2_গুনতি_মান_BITS<=মান_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    newTrie=trie->newTrie;
    যদি(newTrie==nullptr) {
        /* already frozen */
        UTrie2_মান_Bits frozen_মান_Bits=
            trie->data16!=nullptr ? UTRIE2_16_মান_BITS : UTRIE2_32_মান_BITS;
        যদি(মান_Bits!=frozen_মান_Bits) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
        ফেরত;
    }

    /* compact if necessary */
    যদি(!newTrie->isCompacted) {
        compactTrie(trie, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            ফেরত;
        }
    }
    উপরের_Start=trie->উপরের_Start;

    যদি(উপরের_Start<=0x10000) {
        all_Indexes_দৈর্ঘ্য=UTRIE2_সূচক_1_OFFSET;
    } অন্যথা {
        all_Indexes_দৈর্ঘ্য=newTrie->index2_দৈর্ঘ্য;
    }
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        তথ্য_Move=all_Indexes_দৈর্ঘ্য;
    } অন্যথা {
        তথ্য_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    যদি( /* for unshifted indexLength */
        all_Indexes_দৈর্ঘ্য>UTRIE2_MAX_সূচক_দৈর্ঘ্য ||
        /* for unshifted dataNullOffset */
        (তথ্য_Move+newTrie->তথ্য_Null_Offset)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (তথ্য_Move+UNEWTRIE2_তথ্য_0800_OFFSET)>0xffff ||
        /* for shiftedDataLength */
        (তথ্য_Move+newTrie->তথ্য_দৈর্ঘ্য)>UTRIE2_MAX_তথ্য_দৈর্ঘ্য
    ) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত;
    }

    /* calculate the total serialized length */
    length=আকার(UTrie2Header)+all_Indexes_দৈর্ঘ্য*2;
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        length+=newTrie->তথ্য_দৈর্ঘ্য*2;
    } অন্যথা {
        length+=newTrie->তথ্য_দৈর্ঘ্য*4;
    }

    trie->memory=uprv_malloc(length);
    যদি(trie->memory==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    trie->length=length;
    trie->isMemoryOwned=সত্য;

    trie->সূচক_দৈর্ঘ্য=all_Indexes_দৈর্ঘ্য;
    trie->তথ্য_দৈর্ঘ্য=newTrie->তথ্য_দৈর্ঘ্য;
    যদি(উপরের_Start<=0x10000) {
        trie->index2NullOffset=0xffff;
    } অন্যথা {
        trie->index2NullOffset=static_cast<uint16_t>(UTRIE2_সূচক_2_OFFSET+newTrie->index2NullOffset);
    }
    trie->তথ্য_Null_Offset=(uint16_t)(তথ্য_Move+newTrie->তথ্য_Null_Offset);
    trie->উপরের_মান_সূচক=তথ্য_Move+trie->তথ্য_দৈর্ঘ্য-UTRIE2_তথ্য_GRANULARITY;

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)মান_Bits;

    header->সূচক_দৈর্ঘ্য=(uint16_t)trie->সূচক_দৈর্ঘ্য;
    header->shifted_তথ্য_দৈর্ঘ্য=(uint16_t)(trie->তথ্য_দৈর্ঘ্য>>UTRIE2_সূচক_SHIFT);
    header->index2NullOffset=trie->index2NullOffset;
    header->তথ্য_Null_Offset=trie->তথ্য_Null_Offset;
    header->shifted_উপরের_Start=(uint16_t)(উপরের_Start>>UTRIE2_SHIFT_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->সূচক=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    জন্য(i=UTRIE2_সূচক_2_BMP_দৈর্ঘ্য; i>0; --i) {
        *dest16++=(uint16_t)((তথ্য_Move + *p++)>>UTRIE2_সূচক_SHIFT);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    জন্য(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(তথ্য_Move+UTRIE2_BAD_UTF8_তথ্য_OFFSET);
    }
    জন্য(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(তথ্য_Move+newTrie->index2[i<<(6-UTRIE2_SHIFT_2)]);
    }

    যদি(উপরের_Start>0x10000) {
        int32_t index1_দৈর্ঘ্য=(উপরের_Start-0x10000)>>UTRIE2_SHIFT_1;
        int32_t index2Offset=UTRIE2_সূচক_2_BMP_দৈর্ঘ্য+UTRIE2_UTF8_2_B_সূচক_2_দৈর্ঘ্য+index1_দৈর্ঘ্য;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_সূচক_1_দৈর্ঘ্য;
        জন্য(i=index1_দৈর্ঘ্য; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_সূচক_2_OFFSET + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2Offset;
        জন্য(i=newTrie->index2_দৈর্ঘ্য-index2Offset; i>0; --i) {
            *dest16++=(uint16_t)((তথ্য_Move + *p++)>>UTRIE2_সূচক_SHIFT);
        }
    }

    /* write the 16/32-bit data array */
    নির্বাচন(মান_Bits) {
    ক্ষেত্র UTRIE2_16_মান_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        p=newTrie->তথ্য;
        জন্য(i=newTrie->তথ্য_দৈর্ঘ্য; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        বিরতি;
    ক্ষেত্র UTRIE2_32_মান_BITS:
        /* write 32-bit data values */
        trie->data16=nullptr;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->তথ্য, (size_t)newTrie->তথ্য_দৈর্ঘ্য*4);
        বিরতি;
    পূর্বনির্ধারিত:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }

#ifdef UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#endif

#ifdef UCPTRIE_DEBUG
    umutablecptrie_setName(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_মান_Width)মান_Bits, pErrorCode));
#endif
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->তথ্য);
    uprv_free(newTrie);
    trie->newTrie=nullptr;
}

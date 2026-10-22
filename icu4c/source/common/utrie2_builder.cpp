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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/umutablecptrie.h"
#ଅନ୍ତର୍ଭାଵ "ucptrie_impl.h"
#endif
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "utrie2.h"
#ଅନ୍ତର୍ଭାଵ "utrie2_impl.h"

#ଅନ୍ତର୍ଭାଵ "utrie.h"  // for utrie2_fromUTrie()

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

ଗଣନା {
    /** The null index-2 block, following the gap in the index-2 table. */
    UNEWTRIE2_INDEX_2_NULL_OFFSET=UNEWTRIE2_INDEX_GAP_OFFSET+UNEWTRIE2_INDEX_GAP_LENGTH,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_INDEX_2_START_OFFSET=UNEWTRIE2_INDEX_2_NULL_OFFSET+UTRIE2_INDEX_2_BLOCK_LENGTH,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_DATA_NULL_OFFSET=UTRIE2_DATA_START_OFFSET,

    /** The start of allocated data blocks. */
    UNEWTRIE2_DATA_START_OFFSET=UNEWTRIE2_DATA_NULL_OFFSET+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_DATA_0800_OFFSET=UNEWTRIE2_DATA_START_OFFSET+0x780
};

/* Start with allocation of 16k data entries. */
#define UNEWTRIE2_INITIAL_DATA_LENGTH ((int32_t)1<<14)

/* Grow about 8x each time. */
#define UNEWTRIE2_MEDIUM_DATA_LENGTH ((int32_t)1<<17)

ସ୍ଥିର int32_t
allocIndex2Block(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_ମୂଲ୍ୟ, uint32_t error_ମୂଲ୍ୟ, UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *data;
    int32_t i, j;

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(ଆକାର(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(ଆକାର(UNewTrie2));
    data=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_DATA_LENGTH*4);
    ଯଦି(trie==nullptr || newTrie==nullptr || data==nullptr) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(data);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    uprv_memset(trie, 0, ଆକାର(UTrie2));
    trie->initial_ମୂଲ୍ୟ=initial_ମୂଲ୍ୟ;
    trie->error_ମୂଲ୍ୟ=error_ମୂଲ୍ୟ;
    trie->ଉପର_Start=0x110000;
    trie->newTrie=newTrie;
#ifdef UTRIE2_DEBUG
    trie->name="open";
#endif

    newTrie->data=data;
#ifdef UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_ମୂଲ୍ୟ, error_ମୂଲ୍ୟ, pErrorCode);
#endif
    newTrie->dataCapacity=UNEWTRIE2_INITIAL_DATA_LENGTH;
    newTrie->initial_ମୂଲ୍ୟ=initial_ମୂଲ୍ୟ;
    newTrie->error_ମୂଲ୍ୟ=error_ମୂଲ୍ୟ;
    newTrie->ଉପର_Start=0x110000;
    newTrie->firstFreeBlock=0;  /* no free block in the list */
    newTrie->isCompacted=ମିଥ୍ୟା;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    ପାଇଁ(i=0; i<0x80; ++i) {
        newTrie->data[i]=initial_ମୂଲ୍ୟ;
    }
    ପାଇଁ(; i<0xc0; ++i) {
        newTrie->data[i]=error_ମୂଲ୍ୟ;
    }
    ପାଇଁ(i=UNEWTRIE2_DATA_NULL_OFFSET; i<UNEWTRIE2_DATA_START_OFFSET; ++i) {
        newTrie->data[i]=initial_ମୂଲ୍ୟ;
    }
    newTrie->dataNullOffset=UNEWTRIE2_DATA_NULL_OFFSET;
    newTrie->dataLength=UNEWTRIE2_DATA_START_OFFSET;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    ପାଇଁ(i=0, j=0; j<0x80; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    ପାଇଁ(; j<0xc0; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
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
        UTRIE2_LSCP_INDEX_2_LENGTH;
    j+=UTRIE2_DATA_BLOCK_LENGTH;
    ପାଇଁ(; j<UNEWTRIE2_DATA_START_OFFSET; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    ପାଇଁ(i=0x80>>UTRIE2_SHIFT_2; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        newTrie->index2[i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    ପାଇଁ(i=0; i<UNEWTRIE2_INDEX_GAP_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_GAP_OFFSET+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    ପାଇଁ(i=0; i<UTRIE2_INDEX_2_BLOCK_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_2_NULL_OFFSET+i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }
    newTrie->index2NullOffset=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    newTrie->index2Length=UNEWTRIE2_INDEX_2_START_OFFSET;

    /* set the index-1 indexes for the linear index-2 block */
    ପାଇଁ(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        ++i, j+=UTRIE2_INDEX_2_BLOCK_LENGTH
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    ପାଇଁ(; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        newTrie->index1[i]=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    ପାଇଁ(i=0x80; i<0x800; i+=UTRIE2_DATA_BLOCK_LENGTH) {
        utrie2_set32(trie, i, initial_ମୂଲ୍ୟ, pErrorCode);
    }

    ଫେରନ୍ତୁ trie;
}

ସ୍ଥିର UNewTrie2 *
cloneBuilder(ଅଚଳ UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(ଆକାର(UNewTrie2)));
    ଯଦି(trie==nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }

    trie->data = static_cast<uint32_t*>(uprv_malloc(other->dataCapacity * 4));
    ଯଦି(trie->data==nullptr) {
        uprv_free(trie);
        ଫେରନ୍ତୁ nullptr;
    }
#ifdef UCPTRIE_DEBUG
    ଯଦି(other->t3==nullptr) {
        trie->t3=nullptr;
    } ଅନ୍ୟଥା {
        UErrorCode errorCode=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &errorCode);
    }
#endif
    trie->dataCapacity=other->dataCapacity;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, ଆକାର(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2Length*4);
    trie->index2NullOffset=other->index2NullOffset;
    trie->index2Length=other->index2Length;

    uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
    trie->dataNullOffset=other->dataNullOffset;
    trie->dataLength=other->dataLength;

    /* reference counters */
    ଯଦି(other->isCompacted) {
        trie->firstFreeBlock=0;
    } ଅନ୍ୟଥା {
        uprv_memcpy(trie->map, other->map, ((size_t)other->dataLength>>UTRIE2_SHIFT_2)*4);
        trie->firstFreeBlock=other->firstFreeBlock;
    }

    trie->initial_ମୂଲ୍ୟ=other->initial_ମୂଲ୍ୟ;
    trie->error_ମୂଲ୍ୟ=other->error_ମୂଲ୍ୟ;
    trie->ଉପର_Start=other->ଉପର_Start;
    trie->isCompacted=other->isCompacted;

    ଫେରନ୍ତୁ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ଅଚଳ UTrie2 *other, UErrorCode *pErrorCode) {
    UTrie2 *trie;

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(ଆକାର(UTrie2));
    ଯଦି(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memcpy(trie, other, ଆକାର(UTrie2));

    ଯଦି(other->memory!=nullptr) {
        trie->memory=uprv_malloc(other->length);
        ଯଦି(trie->memory!=nullptr) {
            trie->isMemoryOwned=ସତ୍ୟ;
            uprv_memcpy(trie->memory, other->memory, other->length);

            /* make the clone's pointers point to its own memory */
            trie->index=(uint16_t *)trie->memory+(other->index-(uint16_t *)other->memory);
            ଯଦି(other->data16!=nullptr) {
                trie->data16=(uint16_t *)trie->memory+(other->data16-(uint16_t *)other->memory);
            }
            ଯଦି(other->data32!=nullptr) {
                trie->data32=(uint32_t *)trie->memory+(other->data32-(uint32_t *)other->memory);
            }
        }
    } ଅନ୍ୟଥା /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    ଯଦି(trie->memory==nullptr && trie->newTrie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        uprv_free(trie);
        trie=nullptr;
    }
    ଫେରନ୍ତୁ trie;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା NewTrieAndStatus {
    UTrie2 *trie;
    UErrorCode errorCode;
    UBool exclusiveLimit;  /* rather than inclusive range end */
} NewTrieAndStatus;

ସ୍ଥିର UBool U_CALLCONV
copyEnumRange(ଅଚଳ ଶୂନ୍ୟ *context, UChar32 start, UChar32 end, uint32_t value) {
    NewTrieAndStatus *nt=(NewTrieAndStatus *)context;
    ଯଦି(value!=nt->trie->initial_ମୂଲ୍ୟ) {
        ଯଦି(nt->exclusiveLimit) {
            --end;
        }
        ଯଦି(start==end) {
            utrie2_set32(nt->trie, start, value, &nt->errorCode);
        } ଅନ୍ୟଥା {
            utrie2_setRange32(nt->trie, start, end, value, ସତ୍ୟ, &nt->errorCode);
        }
        ଫେରନ୍ତୁ U_SUCCESS(nt->errorCode);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
}

#ifdef UTRIE2_DEBUG
ସ୍ଥିର long ଗଣନା_Initial(ଅଚଳ UTrie2 *trie) {
    uint32_t initial_ମୂଲ୍ୟ=trie->initial_ମୂଲ୍ୟ;
    int32_t length=trie->dataLength;
    long count=0;
    ଯଦି(trie->data16!=nullptr) {
        ପାଇଁ(int32_t i=0; i<length; ++i) {
            ଯଦି(trie->data16[i]==initial_ମୂଲ୍ୟ) { ++count; }
        }
    } ଅନ୍ୟଥା {
        ପାଇଁ(int32_t i=0; i<length; ++i) {
            ଯଦି(trie->data32[i]==initial_ମୂଲ୍ୟ) { ++count; }
        }
    }
    ଫେରନ୍ତୁ count;
}

ସ୍ଥିର ଶୂନ୍ୟ
utrie_printLengths(ଅଚଳ UTrie *trie) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)ଆକାର(UTrieHeader)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           indexLength, dataLength, totalLength);
}

ସ୍ଥିର ଶୂନ୍ୟ
utrie2_printLengths(ଅଚଳ UTrie2 *trie, ଅଚଳ ଅକ୍ଷର *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)ଆକାର(UTrie2Header)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, ଗଣନା_Initial(trie), totalLength);
}
#endif

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ଅଚଳ UTrie2 *other, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(other->newTrie!=nullptr && !other->newTrie->isCompacted) {
        ଫେରନ୍ତୁ utrie2_clone(other, pErrorCode);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_ମୂଲ୍ୟ, other->error_ମୂଲ୍ୟ, pErrorCode);
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    context.exclusiveLimit=ମିଥ୍ୟା;
    context.errorCode=*pErrorCode;
    utrie2_enum(other, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    ପାଇଁ(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        ଯଦି(other->data32==nullptr) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } ଅନ୍ୟଥା {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        ଯଦି(value!=other->initial_ମୂଲ୍ୟ) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    ଯଦି(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    ଫେରନ୍ତୁ context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ଅଚଳ UTrie *trie1, uint32_t error_ମୂଲ୍ୟ, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(trie1==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    context.trie=utrie2_open(trie1->initial_ମୂଲ୍ୟ, error_ମୂଲ୍ୟ, pErrorCode);
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    context.exclusiveLimit=ସତ୍ୟ;
    context.errorCode=*pErrorCode;
    utrie_enum(trie1, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    ପାଇଁ(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        ଯଦି(trie1->data32==nullptr) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } ଅନ୍ୟଥା {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        ଯଦି(value!=trie1->initial_ମୂଲ୍ୟ) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    ଯଦି(U_SUCCESS(*pErrorCode)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=nullptr ? UTRIE2_32_ମୂଲ୍ୟ_BITS : UTRIE2_16_ମୂଲ୍ୟ_BITS,
                      pErrorCode);
    }
#ifdef UTRIE2_DEBUG
    ଯଦି(U_SUCCESS(*pErrorCode)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#endif
    ଯଦି(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    ଫେରନ୍ତୁ context.trie;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
isInNullBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, block;

    ଯଦି(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } ଅନ୍ୟଥା {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    ଫେରନ୍ତୁ block == trie->dataNullOffset;
}

ସ୍ଥିର int32_t
allocIndex2Block(UNewTrie2 *trie) {
    int32_t newBlock, new_ଶୀର୍ଷ;

    newBlock=trie->index2Length;
    new_ଶୀର୍ଷ=newBlock+UTRIE2_INDEX_2_BLOCK_LENGTH;
    ଯଦି(new_ଶୀର୍ଷ>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        ଫେରନ୍ତୁ -1;
    }
    trie->index2Length=new_ଶୀର୍ଷ;
    uprv_memcpy(trie->index2+newBlock, trie->index2+trie->index2NullOffset, UTRIE2_INDEX_2_BLOCK_LENGTH*4);
    ଫେରନ୍ତୁ newBlock;
}

ସ୍ଥିର int32_t
getIndex2Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    ଯଦି(U_IS_LEAD(c) && forLSCP) {
        ଫେରନ୍ତୁ UTRIE2_LSCP_INDEX_2_OFFSET;
    }

    i1=c>>UTRIE2_SHIFT_1;
    i2=trie->index1[i1];
    ଯଦି(i2==trie->index2NullOffset) {
        i2=allocIndex2Block(trie);
        ଯଦି(i2<0) {
            ଫେରନ୍ତୁ -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    ଫେରନ୍ତୁ i2;
}

ସ୍ଥିର int32_t
allocDataBlock(UNewTrie2 *trie, int32_t copyBlock) {
    int32_t newBlock, new_ଶୀର୍ଷ;

    ଯଦି(trie->firstFreeBlock!=0) {
        /* get the first free block */
        newBlock=trie->firstFreeBlock;
        trie->firstFreeBlock=-trie->map[newBlock>>UTRIE2_SHIFT_2];
    } ଅନ୍ୟଥା {
        /* get a new block from the high end */
        newBlock=trie->dataLength;
        new_ଶୀର୍ଷ=newBlock+UTRIE2_DATA_BLOCK_LENGTH;
        ଯଦି(new_ଶୀର୍ଷ>trie->dataCapacity) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *data;

            ଯଦି(trie->dataCapacity<UNEWTRIE2_MEDIUM_DATA_LENGTH) {
                capacity=UNEWTRIE2_MEDIUM_DATA_LENGTH;
            } ଅନ୍ୟଥା ଯଦି(trie->dataCapacity<UNEWTRIE2_MAX_DATA_LENGTH) {
                capacity=UNEWTRIE2_MAX_DATA_LENGTH;
            } ଅନ୍ୟଥା {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                ଫେରନ୍ତୁ -1;
            }
            data = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            ଯଦି(data==nullptr) {
                ଫେରନ୍ତୁ -1;
            }
            uprv_memcpy(data, trie->data, (size_t)trie->dataLength*4);
            uprv_free(trie->data);
            trie->data=data;
            trie->dataCapacity=capacity;
        }
        trie->dataLength=new_ଶୀର୍ଷ;
    }
    uprv_memcpy(trie->data+newBlock, trie->data+copyBlock, UTRIE2_DATA_BLOCK_LENGTH*4);
    trie->map[newBlock>>UTRIE2_SHIFT_2]=0;
    ଫେରନ୍ତୁ newBlock;
}

/* call when the block's reference counter reaches 0 */
ସ୍ଥିର ଶୂନ୍ୟ
releaseDataBlock(UNewTrie2 *trie, int32_t block) {
    /* put this block at the front of the free-block chain */
    trie->map[block>>UTRIE2_SHIFT_2]=-trie->firstFreeBlock;
    trie->firstFreeBlock=block;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
isWritableBlock(UNewTrie2 *trie, int32_t block) {
    ଫେରନ୍ତୁ block != trie->dataNullOffset && 1 == trie->map[block >> UTRIE2_SHIFT_2];
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
setIndex2Entry(UNewTrie2 *trie, int32_t i2, int32_t block) {
    int32_t oldBlock;
    ++trie->map[block>>UTRIE2_SHIFT_2];  /* increment first, in case block==oldBlock! */
    oldBlock=trie->index2[i2];
    ଯଦି(0 == --trie->map[oldBlock>>UTRIE2_SHIFT_2]) {
        releaseDataBlock(trie, oldBlock);
    }
    trie->index2[i2]=block;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ସ୍ଥିର int32_t
getDataBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, oldBlock, newBlock;

    i2=getIndex2Block(trie, c, forLSCP);
    ଯଦି(i2<0) {
        ଫେରନ୍ତୁ -1;  /* program error */
    }

    i2+=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
    oldBlock=trie->index2[i2];
    ଯଦି(isWritableBlock(trie, oldBlock)) {
        ଫେରନ୍ତୁ oldBlock;
    }

    /* allocate a new data block */
    newBlock=allocDataBlock(trie, oldBlock);
    ଯଦି(newBlock<0) {
        /* out of memory in the data array */
        ଫେରନ୍ତୁ -1;
    }
    setIndex2Entry(trie, i2, newBlock);
    ଫେରନ୍ତୁ newBlock;
}

/**
 * @return true if the value was successfully set
 */
ସ୍ଥିର ଶୂନ୍ୟ
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UErrorCode *pErrorCode) {
    int32_t block;

    ଯଦି(trie==nullptr || trie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        ଫେରନ୍ତୁ;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_set(trie->t3, c, value, pErrorCode);
#endif

    block=getDataBlock(trie, c, forLSCP);
    ଯଦି(block<0) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }

    trie->data[block+(c&UTRIE2_DATA_MASK)]=value;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି((uint32_t)c>0x10ffff) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    set32(trie->newTrie, c, ସତ୍ୟ, value, pErrorCode);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrie2_set32ForLeadSurrogateCodeUnit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(!U_IS_LEAD(c)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    set32(trie->newTrie, c, ମିଥ୍ୟା, value, pErrorCode);
}

ସ୍ଥିର ଶୂନ୍ୟ
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit=block+UTRIE2_DATA_BLOCK_LENGTH;
    ଯେତେବେଳେ(block<limit) {
        *block++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
ସ୍ଥିର ଶୂନ୍ୟ
fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
          uint32_t value, uint32_t initial_ମୂଲ୍ୟ, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    ଯଦି(overwrite) {
        ଯେତେବେଳେ(block<pLimit) {
            *block++=value;
        }
    } ଅନ୍ୟଥା {
        ଯେତେବେଳେ(block<pLimit) {
            ଯଦି(*block==initial_ମୂଲ୍ୟ) {
                *block=value;
            }
            ++block;
        }
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
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

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି((uint32_t)start>0x10ffff || (uint32_t)end>0x10ffff || start>end) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    newTrie=trie->newTrie;
    ଯଦି(newTrie==nullptr || newTrie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        ଫେରନ୍ତୁ;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_setRange(newTrie->t3, start, end, value, pErrorCode);
#endif
    ଯଦି(!overwrite && value==newTrie->initial_ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ; /* nothing to do */
    }

    limit=end+1;
    ଯଦି(start&UTRIE2_DATA_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=getDataBlock(newTrie, start, ସତ୍ୟ);
        ଯଦି(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }

        nextStart=(start+UTRIE2_DATA_MASK)&~UTRIE2_DATA_MASK;
        ଯଦି(nextStart<=limit) {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, UTRIE2_DATA_BLOCK_LENGTH,
                      value, newTrie->initial_ମୂଲ୍ୟ, overwrite);
            start=nextStart;
        } ଅନ୍ୟଥା {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, limit&UTRIE2_DATA_MASK,
                      value, newTrie->initial_ମୂଲ୍ୟ, overwrite);
            ଫେରନ୍ତୁ;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE2_DATA_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE2_DATA_MASK;

    /* iterate over all-value blocks */
    ଯଦି(value==newTrie->initial_ମୂଲ୍ୟ) {
        repeatBlock=newTrie->dataNullOffset;
    } ଅନ୍ୟଥା {
        repeatBlock=-1;
    }

    ଯେତେବେଳେ(start<limit) {
        int32_t i2;
        UBool setRepeatBlock=ମିଥ୍ୟା;

        ଯଦି(value==newTrie->initial_ମୂଲ୍ୟ && isInNullBlock(newTrie, start, ସତ୍ୟ)) {
            start+=UTRIE2_DATA_BLOCK_LENGTH; /* nothing to do */
            ଜାରି;
        }

        /* get index value */
        i2=getIndex2Block(newTrie, start, ସତ୍ୟ);
        ଯଦି(i2<0) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            ଫେରନ୍ତୁ;
        }
        i2+=(start>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
        block=newTrie->index2[i2];
        ଯଦି(isWritableBlock(newTrie, block)) {
            /* already allocated */
            ଯଦି(overwrite && block>=UNEWTRIE2_DATA_0800_OFFSET) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                setRepeatBlock=ସତ୍ୟ;
            } ଅନ୍ୟଥା {
                /* !overwrite, or protected block: just write the values into this block */
                fillBlock(newTrie->data+block,
                          0, UTRIE2_DATA_BLOCK_LENGTH,
                          value, newTrie->initial_ମୂଲ୍ୟ, overwrite);
            }
        } ଅନ୍ୟଥା ଯଦି(newTrie->data[block]!=value && (overwrite || block==newTrie->dataNullOffset)) {
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
            setRepeatBlock=ସତ୍ୟ;
        }
        ଯଦି(setRepeatBlock) {
            ଯଦି(repeatBlock>=0) {
                setIndex2Entry(newTrie, i2, repeatBlock);
            } ଅନ୍ୟଥା {
                /* create and set and fill the repeatBlock */
                repeatBlock=getDataBlock(newTrie, start, ସତ୍ୟ);
                ଯଦି(repeatBlock<0) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    ଫେରନ୍ତୁ;
                }
                writeBlock(newTrie->data+repeatBlock, value);
            }
        }

        start+=UTRIE2_DATA_BLOCK_LENGTH;
    }

    ଯଦି(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=getDataBlock(newTrie, start, ସତ୍ୟ);
        ଯଦି(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }

        fillBlock(newTrie->data+block, 0, rest, value, newTrie->initial_ମୂଲ୍ୟ, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
equal_int32(ଅଚଳ int32_t *s, ଅଚଳ int32_t *t, int32_t length) {
    ଯେତେବେଳେ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ଫେରନ୍ତୁ length == 0;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool
equal_uint32(ଅଚଳ uint32_t *s, ଅଚଳ uint32_t *t, int32_t length) {
    ଯେତେବେଳେ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ଫେରନ୍ତୁ length == 0;
}

ସ୍ଥିର int32_t
findSameIndex2Block(ଅଚଳ int32_t *idx, int32_t index2Length, int32_t otherBlock) {
    int32_t block;

    /* ensure that we do not even partially get past index2Length */
    index2Length-=UTRIE2_INDEX_2_BLOCK_LENGTH;

    ପାଇଁ(block=0; block<=index2Length; ++block) {
        ଯଦି(equal_int32(idx+block, idx+otherBlock, UTRIE2_INDEX_2_BLOCK_LENGTH)) {
            ଫେରନ୍ତୁ block;
        }
    }
    ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର int32_t
findSameDataBlock(ଅଚଳ uint32_t *data, int32_t dataLength, int32_t otherBlock, int32_t blockLength) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=blockLength;

    ପାଇଁ(block=0; block<=dataLength; block+=UTRIE2_DATA_GRANULARITY) {
        ଯଦି(equal_uint32(data+block, data+otherBlock, blockLength)) {
            ଫେରନ୍ତୁ block;
        }
    }
    ଫେରନ୍ତୁ -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
ସ୍ଥିର UChar32
find_ଉପର_Start(UNewTrie2 *trie, uint32_t ଉପର_ମୂଲ୍ୟ) {
    ଅଚଳ uint32_t *data32;

    uint32_t value, initial_ମୂଲ୍ୟ;
    UChar32 c, prev;
    int32_t i1, i2, j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    data32=trie->data;
    initial_ମୂଲ୍ୟ=trie->initial_ମୂଲ୍ୟ;

    index2NullOffset=trie->index2NullOffset;
    nullBlock=trie->dataNullOffset;

    /* set variables for previous range */
    ଯଦି(ଉପର_ମୂଲ୍ୟ==initial_ମୂଲ୍ୟ) {
        prevI2Block=index2NullOffset;
        prevBlock=nullBlock;
    } ଅନ୍ୟଥା {
        prevI2Block=-1;
        prevBlock=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_INDEX_1_LENGTH;
    c=prev;
    ଯେତେବେଳେ(c>0) {
        i2Block=trie->index1[--i1];
        ଯଦି(i2Block==prevI2Block) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
            ଜାରି;
        }
        prevI2Block=i2Block;
        ଯଦି(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            ଯଦି(ଉପର_ମୂଲ୍ୟ!=initial_ମୂଲ୍ୟ) {
                ଫେରନ୍ତୁ c;
            }
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } ଅନ୍ୟଥା {
            /* enumerate data blocks for one index-2 block */
            ପାଇଁ(i2=UTRIE2_INDEX_2_BLOCK_LENGTH; i2>0;) {
                block=trie->index2[i2Block+ --i2];
                ଯଦି(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                    ଜାରି;
                }
                prevBlock=block;
                ଯଦି(block==nullBlock) {
                    /* this is the null data block */
                    ଯଦି(ଉପର_ମୂଲ୍ୟ!=initial_ମୂଲ୍ୟ) {
                        ଫେରନ୍ତୁ c;
                    }
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                } ଅନ୍ୟଥା {
                    ପାଇଁ(j=UTRIE2_DATA_BLOCK_LENGTH; j>0;) {
                        value=data32[block+ --j];
                        ଯଦି(value!=ଉପର_ମୂଲ୍ୟ) {
                            ଫେରନ୍ତୁ c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    ଫେରନ୍ତୁ 0;
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
ସ୍ଥିର ଶୂନ୍ୟ
compactData(UNewTrie2 *trie) {
#ifdef UTRIE2_DEBUG
    int32_t ଗଣନା_Same=0, sumOverlaps=0;
#endif

    int32_t start, newStart, movedStart;
    int32_t blockLength, overlap;
    int32_t i, mapIndex, block_ଗଣନା;

    /* do not compact linear-ASCII data */
    newStart=UTRIE2_DATA_START_OFFSET;
    ପାଇଁ(start=0, i=0; start<newStart; start+=UTRIE2_DATA_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    blockLength=64;
    block_ଗଣନା=blockLength>>UTRIE2_SHIFT_2;
    ପାଇଁ(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        ଯଦି(start==UNEWTRIE2_DATA_0800_OFFSET) {
            blockLength=UTRIE2_DATA_BLOCK_LENGTH;
            block_ଗଣନା=1;
        }

        /* skip blocks that are not used */
        ଯଦି(trie->map[start>>UTRIE2_SHIFT_2]<=0) {
            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            ଜାରି;
        }

        /* search for an identical block */
        ଯଦି( (movedStart=findSameDataBlock(trie->data, newStart, start, blockLength))
             >=0
        ) {
#ifdef UTRIE2_DEBUG
            ++ଗଣନା_Same;
#endif
            /* found an identical block, set the other block's index value for the current block */
            ପାଇଁ(i=block_ଗଣନା, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            ଜାରି;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        ପାଇଁ(overlap=blockLength-UTRIE2_DATA_GRANULARITY;
            overlap>0 && !equal_uint32(trie->data+(newStart-overlap), trie->data+start, overlap);
            overlap-=UTRIE2_DATA_GRANULARITY) {}

#ifdef UTRIE2_DEBUG
            sumOverlaps+=overlap;
#endif
        ଯଦି(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            movedStart=newStart-overlap;
            ପାଇଁ(i=block_ଗଣନା, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            ପାଇଁ(i=blockLength-overlap; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } ଅନ୍ୟଥା /* no overlap && newStart==start */ {
            ପାଇଁ(i=block_ଗଣନା, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=start;
                start+=UTRIE2_DATA_BLOCK_LENGTH;
            }
            newStart=start;
        }
    }

    /* now adjust the index-2 table */
    ପାଇଁ(i=0; i<trie->index2Length; ++i) {
        ଯଦି(i==UNEWTRIE2_INDEX_GAP_OFFSET) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_INDEX_GAP_LENGTH;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_SHIFT_2];
    }
    trie->dataNullOffset=trie->map[trie->dataNullOffset>>UTRIE2_SHIFT_2];

    /* ensure dataLength alignment */
    ଯେତେବେଳେ((newStart&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        trie->data[newStart++]=trie->initial_ମୂଲ୍ୟ;
    }

#ifdef UTRIE2_DEBUG
    /* we saved some space */
    printf("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)trie->dataLength, (long)newStart, (long)ଗଣନା_Same, (long)sumOverlaps);
#endif

    trie->dataLength=newStart;
}

ସ୍ଥିର ଶୂନ୍ୟ
compactIndex2(UNewTrie2 *trie) {
    int32_t i, start, newStart, movedStart, overlap;

    /* do not compact linear-BMP index-2 blocks */
    newStart=UTRIE2_INDEX_2_BMP_LENGTH;
    ପାଇଁ(start=0, i=0; start<newStart; start+=UTRIE2_INDEX_2_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    newStart+=UTRIE2_UTF8_2B_INDEX_2_LENGTH+((trie->ଉପର_Start-0x10000)>>UTRIE2_SHIFT_1);

    ପାଇଁ(start=UNEWTRIE2_INDEX_2_NULL_OFFSET; start<trie->index2Length;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        ଯଦି( (movedStart=findSameIndex2Block(trie->index2, newStart, start))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=movedStart;

            /* advance start to the next block */
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            ଜାରି;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        ପାଇଁ(overlap=UTRIE2_INDEX_2_BLOCK_LENGTH-1;
            overlap>0 && !equal_int32(trie->index2+(newStart-overlap), trie->index2+start, overlap);
            --overlap) {}

        ଯଦି(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=newStart-overlap;

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            ପାଇଁ(i=UTRIE2_INDEX_2_BLOCK_LENGTH-overlap; i>0; --i) {
                trie->index2[newStart++]=trie->index2[start++];
            }
        } ଅନ୍ୟଥା /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE2_SHIFT_1_2]=start;
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;
            newStart=start;
        }
    }

    /* now adjust the index-1 table */
    ପାଇଁ(i=0; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_SHIFT_1_2];
    }
    trie->index2NullOffset=trie->map[trie->index2NullOffset>>UTRIE2_SHIFT_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    ଯେତେବେଳେ((newStart&((UTRIE2_DATA_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[newStart++] = static_cast<int32_t>(0xffff) << UTRIE2_INDEX_SHIFT;
    }

#ifdef UTRIE2_DEBUG
    /* we saved some space */
    printf("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (long)trie->index2Length, (long)newStart);
#endif

    trie->index2Length=newStart;
}

ସ୍ଥିର ଶୂନ୍ୟ
compactTrie(UTrie2 *trie, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UChar32 ଉପର_Start, supp_ଉପର_Start;
    uint32_t ଉପର_ମୂଲ୍ୟ;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    ଉପର_ମୂଲ୍ୟ=utrie2_get32(trie, 0x10ffff);
    ଉପର_Start=find_ଉପର_Start(newTrie, ଉପର_ମୂଲ୍ୟ);
    ଉପର_Start=(ଉପର_Start+(UTRIE2_CP_PER_INDEX_1_ENTRY-1))&~(UTRIE2_CP_PER_INDEX_1_ENTRY-1);
    ଯଦି(ଉପର_Start==0x110000) {
        ଉପର_ମୂଲ୍ୟ=trie->error_ମୂଲ୍ୟ;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->ଉପର_Start=newTrie->ଉପର_Start=ଉପର_Start;

#ifdef UTRIE2_DEBUG
    printf("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)ଉପର_Start, (long)ଉପର_ମୂଲ୍ୟ, (long)trie->initial_ମୂଲ୍ୟ);
#endif

    ଯଦି(ଉପର_Start<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_ଉପର_Start= ଉପର_Start<=0x10000 ? 0x10000 : ଉପର_Start;
        utrie2_setRange32(trie, supp_ଉପର_Start, 0x10ffff, trie->initial_ମୂଲ୍ୟ, ସତ୍ୟ, pErrorCode);
        ଯଦି(U_FAILURE(*pErrorCode)) {
            ଫେରନ୍ତୁ;
        }
    }

    compactData(newTrie);
    ଯଦି(ଉପର_Start>0x10000) {
        compactIndex2(newTrie);
#ifdef UTRIE2_DEBUG
    } ଅନ୍ୟଥା {
        printf("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (long)ଉପର_Start, (long)trie->newTrie->index2Length, (long)UTRIE2_INDEX_1_OFFSET);
#endif
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->data[newTrie->dataLength++]=ଉପର_ମୂଲ୍ୟ;
    ଯେତେବେଳେ((newTrie->dataLength&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        newTrie->data[newTrie->dataLength++]=trie->initial_ମୂଲ୍ୟ;
    }

    newTrie->isCompacted=ସତ୍ୟ;
}

/* serialization ------------------------------------------------------------ */

/**
 * Maximum length of the runtime index array.
 * Limited by its own 16-bit index values, and by uint16_t UTrie2Header.indexLength.
 * (The actual maximum length is lower,
 * (0x110000>>UTRIE2_SHIFT_2)+UTRIE2_UTF8_2B_INDEX_2_LENGTH+UTRIE2_MAX_INDEX_1_LENGTH.)
 */
#define UTRIE2_MAX_INDEX_LENGTH 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#define UTRIE2_MAX_DATA_LENGTH (0xffff<<UTRIE2_INDEX_SHIFT)

/* Compact and internally serialize the trie. */
U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_ମୂଲ୍ୟ_Bits ମୂଲ୍ୟ_Bits, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t allIndexesLength;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */
    UChar32 ଉପର_Start;

    /* argument check */
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି( trie==nullptr ||
        ମୂଲ୍ୟ_Bits<0 || UTRIE2_ଗଣନା_ମୂଲ୍ୟ_BITS<=ମୂଲ୍ୟ_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    newTrie=trie->newTrie;
    ଯଦି(newTrie==nullptr) {
        /* already frozen */
        UTrie2_ମୂଲ୍ୟ_Bits frozen_ମୂଲ୍ୟ_Bits=
            trie->data16!=nullptr ? UTRIE2_16_ମୂଲ୍ୟ_BITS : UTRIE2_32_ମୂଲ୍ୟ_BITS;
        ଯଦି(ମୂଲ୍ୟ_Bits!=frozen_ମୂଲ୍ୟ_Bits) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
        ଫେରନ୍ତୁ;
    }

    /* compact if necessary */
    ଯଦି(!newTrie->isCompacted) {
        compactTrie(trie, pErrorCode);
        ଯଦି(U_FAILURE(*pErrorCode)) {
            ଫେରନ୍ତୁ;
        }
    }
    ଉପର_Start=trie->ଉପର_Start;

    ଯଦି(ଉପର_Start<=0x10000) {
        allIndexesLength=UTRIE2_INDEX_1_OFFSET;
    } ଅନ୍ୟଥା {
        allIndexesLength=newTrie->index2Length;
    }
    ଯଦି(ମୂଲ୍ୟ_Bits==UTRIE2_16_ମୂଲ୍ୟ_BITS) {
        dataMove=allIndexesLength;
    } ଅନ୍ୟଥା {
        dataMove=0;
    }

    /* are indexLength and dataLength within limits? */
    ଯଦି( /* for unshifted indexLength */
        allIndexesLength>UTRIE2_MAX_INDEX_LENGTH ||
        /* for unshifted dataNullOffset */
        (dataMove+newTrie->dataNullOffset)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (dataMove+UNEWTRIE2_DATA_0800_OFFSET)>0xffff ||
        /* for shiftedDataLength */
        (dataMove+newTrie->dataLength)>UTRIE2_MAX_DATA_LENGTH
    ) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ;
    }

    /* calculate the total serialized length */
    length=ଆକାର(UTrie2Header)+allIndexesLength*2;
    ଯଦି(ମୂଲ୍ୟ_Bits==UTRIE2_16_ମୂଲ୍ୟ_BITS) {
        length+=newTrie->dataLength*2;
    } ଅନ୍ୟଥା {
        length+=newTrie->dataLength*4;
    }

    trie->memory=uprv_malloc(length);
    ଯଦି(trie->memory==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    trie->length=length;
    trie->isMemoryOwned=ସତ୍ୟ;

    trie->indexLength=allIndexesLength;
    trie->dataLength=newTrie->dataLength;
    ଯଦି(ଉପର_Start<=0x10000) {
        trie->index2NullOffset=0xffff;
    } ଅନ୍ୟଥା {
        trie->index2NullOffset=static_cast<uint16_t>(UTRIE2_INDEX_2_OFFSET+newTrie->index2NullOffset);
    }
    trie->dataNullOffset=(uint16_t)(dataMove+newTrie->dataNullOffset);
    trie->ଉପର_ମୂଲ୍ୟ_Index=dataMove+trie->dataLength-UTRIE2_DATA_GRANULARITY;

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)ମୂଲ୍ୟ_Bits;

    header->indexLength=(uint16_t)trie->indexLength;
    header->shiftedDataLength=(uint16_t)(trie->dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=trie->index2NullOffset;
    header->dataNullOffset=trie->dataNullOffset;
    header->shifted_ଉପର_Start=(uint16_t)(ଉପର_Start>>UTRIE2_SHIFT_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    ପାଇଁ(i=UTRIE2_INDEX_2_BMP_LENGTH; i>0; --i) {
        *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    ପାଇଁ(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    ପାଇଁ(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(dataMove+newTrie->index2[i<<(6-UTRIE2_SHIFT_2)]);
    }

    ଯଦି(ଉପର_Start>0x10000) {
        int32_t index1Length=(ଉପର_Start-0x10000)>>UTRIE2_SHIFT_1;
        int32_t index2Offset=UTRIE2_INDEX_2_BMP_LENGTH+UTRIE2_UTF8_2B_INDEX_2_LENGTH+index1Length;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        ପାଇଁ(i=index1Length; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_INDEX_2_OFFSET + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2Offset;
        ପାଇଁ(i=newTrie->index2Length-index2Offset; i>0; --i) {
            *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
        }
    }

    /* write the 16/32-bit data array */
    ବାଛ(ମୂଲ୍ୟ_Bits) {
    ସ୍ଥିତି UTRIE2_16_ମୂଲ୍ୟ_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        p=newTrie->data;
        ପାଇଁ(i=newTrie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UTRIE2_32_ମୂଲ୍ୟ_BITS:
        /* write 32-bit data values */
        trie->data16=nullptr;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->data, (size_t)newTrie->dataLength*4);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

#ifdef UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#endif

#ifdef UCPTRIE_DEBUG
    umutablecptrie_setName(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_ମୂଲ୍ୟ_Width)ମୂଲ୍ୟ_Bits, pErrorCode));
#endif
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->data);
    uprv_free(newTrie);
    trie->newTrie=nullptr;
}

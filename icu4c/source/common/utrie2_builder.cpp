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

#ضمن "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/umutablecptrie.h"
#ضمن "ucptrie_impl.h"
#endif
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

ساكن int32_t
allocIndex2Block(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_قىممەت, uint32_t error_قىممەت, UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *data;
    int32_t i, j;

    إذا(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(حجم(UNewTrie2));
    data=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_DATA_LENGTH*4);
    إذا(trie==nullptr || newTrie==nullptr || data==nullptr) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(data);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }

    uprv_memset(trie, 0, حجم(UTrie2));
    trie->initial_قىممەت=initial_قىممەت;
    trie->error_قىممەت=error_قىممەت;
    trie->يۇقىرى_Start=0x110000;
    trie->newTrie=newTrie;
#ifdef UTRIE2_DEBUG
    trie->name="open";
#endif

    newTrie->data=data;
#ifdef UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_قىممەت, error_قىممەت, pErrorCode);
#endif
    newTrie->dataCapacity=UNEWTRIE2_INITIAL_DATA_LENGTH;
    newTrie->initial_قىممەت=initial_قىممەت;
    newTrie->error_قىممەت=error_قىممەت;
    newTrie->يۇقىرى_Start=0x110000;
    newTrie->firstFreeBlock=0;  /* no free block in the list */
    newTrie->isCompacted=خطأ;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    لكل(i=0; i<0x80; ++i) {
        newTrie->data[i]=initial_قىممەت;
    }
    لكل(; i<0xc0; ++i) {
        newTrie->data[i]=error_قىممەت;
    }
    لكل(i=UNEWTRIE2_DATA_NULL_OFFSET; i<UNEWTRIE2_DATA_START_OFFSET; ++i) {
        newTrie->data[i]=initial_قىممەت;
    }
    newTrie->dataNullOffset=UNEWTRIE2_DATA_NULL_OFFSET;
    newTrie->dataLength=UNEWTRIE2_DATA_START_OFFSET;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    لكل(i=0, j=0; j<0x80; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    لكل(; j<0xc0; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
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
    لكل(; j<UNEWTRIE2_DATA_START_OFFSET; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    لكل(i=0x80>>UTRIE2_SHIFT_2; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        newTrie->index2[i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    لكل(i=0; i<UNEWTRIE2_INDEX_GAP_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_GAP_OFFSET+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    لكل(i=0; i<UTRIE2_INDEX_2_BLOCK_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_2_NULL_OFFSET+i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }
    newTrie->index2NullOffset=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    newTrie->index2Length=UNEWTRIE2_INDEX_2_START_OFFSET;

    /* set the index-1 indexes for the linear index-2 block */
    لكل(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        ++i, j+=UTRIE2_INDEX_2_BLOCK_LENGTH
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    لكل(; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        newTrie->index1[i]=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    لكل(i=0x80; i<0x800; i+=UTRIE2_DATA_BLOCK_LENGTH) {
        utrie2_set32(trie, i, initial_قىممەت, pErrorCode);
    }

    أرجع trie;
}

ساكن UNewTrie2 *
cloneBuilder(ثابت UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(حجم(UNewTrie2)));
    إذا(trie==nullptr) {
        أرجع nullptr;
    }

    trie->data = static_cast<uint32_t*>(uprv_malloc(other->dataCapacity * 4));
    إذا(trie->data==nullptr) {
        uprv_free(trie);
        أرجع nullptr;
    }
#ifdef UCPTRIE_DEBUG
    إذا(other->t3==nullptr) {
        trie->t3=nullptr;
    } وإلا {
        UErrorCode errorCode=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &errorCode);
    }
#endif
    trie->dataCapacity=other->dataCapacity;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, حجم(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2Length*4);
    trie->index2NullOffset=other->index2NullOffset;
    trie->index2Length=other->index2Length;

    uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
    trie->dataNullOffset=other->dataNullOffset;
    trie->dataLength=other->dataLength;

    /* reference counters */
    إذا(other->isCompacted) {
        trie->firstFreeBlock=0;
    } وإلا {
        uprv_memcpy(trie->map, other->map, ((size_t)other->dataLength>>UTRIE2_SHIFT_2)*4);
        trie->firstFreeBlock=other->firstFreeBlock;
    }

    trie->initial_قىممەت=other->initial_قىممەت;
    trie->error_قىممەت=other->error_قىممەت;
    trie->يۇقىرى_Start=other->يۇقىرى_Start;
    trie->isCompacted=other->isCompacted;

    أرجع trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ثابت UTrie2 *other, UErrorCode *pErrorCode) {
    UTrie2 *trie;

    إذا(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    إذا(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memcpy(trie, other, حجم(UTrie2));

    إذا(other->memory!=nullptr) {
        trie->memory=uprv_malloc(other->length);
        إذا(trie->memory!=nullptr) {
            trie->isMemoryOwned=true;
            uprv_memcpy(trie->memory, other->memory, other->length);

            /* make the clone's pointers point to its own memory */
            trie->index=(uint16_t *)trie->memory+(other->index-(uint16_t *)other->memory);
            إذا(other->data16!=nullptr) {
                trie->data16=(uint16_t *)trie->memory+(other->data16-(uint16_t *)other->memory);
            }
            إذا(other->data32!=nullptr) {
                trie->data32=(uint32_t *)trie->memory+(other->data32-(uint32_t *)other->memory);
            }
        }
    } وإلا /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    إذا(trie->memory==nullptr && trie->newTrie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        uprv_free(trie);
        trie=nullptr;
    }
    أرجع trie;
}

تعريفنوع بنية NewTrieAndStatus {
    UTrie2 *trie;
    UErrorCode errorCode;
    UBool exclusiveLimit;  /* rather than inclusive range end */
} NewTrieAndStatus;

ساكن UBool U_CALLCONV
copyEnumRange(ثابت فراغ *context, UChar32 start, UChar32 end, uint32_t value) {
    NewTrieAndStatus *nt=(NewTrieAndStatus *)context;
    إذا(value!=nt->trie->initial_قىممەت) {
        إذا(nt->exclusiveLimit) {
            --end;
        }
        إذا(start==end) {
            utrie2_set32(nt->trie, start, value, &nt->errorCode);
        } وإلا {
            utrie2_setRange32(nt->trie, start, end, value, true, &nt->errorCode);
        }
        أرجع U_SUCCESS(nt->errorCode);
    } وإلا {
        أرجع true;
    }
}

#ifdef UTRIE2_DEBUG
ساكن long سانى_Initial(ثابت UTrie2 *trie) {
    uint32_t initial_قىممەت=trie->initial_قىممەت;
    int32_t length=trie->dataLength;
    long count=0;
    إذا(trie->data16!=nullptr) {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data16[i]==initial_قىممەت) { ++count; }
        }
    } وإلا {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(trie->data32[i]==initial_قىممەت) { ++count; }
        }
    }
    أرجع count;
}

ساكن فراغ
utrie_printLengths(ثابت UTrie *trie) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)حجم(UTrieHeader)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           indexLength, dataLength, totalLength);
}

ساكن فراغ
utrie2_printLengths(ثابت UTrie2 *trie, ثابت حرف *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)حجم(UTrie2Header)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, سانى_Initial(trie), totalLength);
}
#endif

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ثابت UTrie2 *other, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    إذا(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    إذا(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    إذا(other->newTrie!=nullptr && !other->newTrie->isCompacted) {
        أرجع utrie2_clone(other, pErrorCode);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_قىممەت, other->error_قىممەت, pErrorCode);
    إذا(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    context.exclusiveLimit=خطأ;
    context.errorCode=*pErrorCode;
    utrie2_enum(other, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(other->data32==nullptr) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } وإلا {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        إذا(value!=other->initial_قىممەت) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    إذا(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    أرجع context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ثابت UTrie *trie1, uint32_t error_قىممەت, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    إذا(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    إذا(trie1==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    context.trie=utrie2_open(trie1->initial_قىممەت, error_قىممەت, pErrorCode);
    إذا(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }
    context.exclusiveLimit=true;
    context.errorCode=*pErrorCode;
    utrie_enum(trie1, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    لكل(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        إذا(trie1->data32==nullptr) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } وإلا {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        إذا(value!=trie1->initial_قىممەت) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    إذا(U_SUCCESS(*pErrorCode)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=nullptr ? UTRIE2_32_قىممەت_BITS : UTRIE2_16_قىممەت_BITS,
                      pErrorCode);
    }
#ifdef UTRIE2_DEBUG
    إذا(U_SUCCESS(*pErrorCode)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#endif
    إذا(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    أرجع context.trie;
}

ساكن مضمن UBool
isInNullBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, block;

    إذا(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    أرجع block == trie->dataNullOffset;
}

ساكن int32_t
allocIndex2Block(UNewTrie2 *trie) {
    int32_t newBlock, new_چوققا;

    newBlock=trie->index2Length;
    new_چوققا=newBlock+UTRIE2_INDEX_2_BLOCK_LENGTH;
    إذا(new_چوققا>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        أرجع -1;
    }
    trie->index2Length=new_چوققا;
    uprv_memcpy(trie->index2+newBlock, trie->index2+trie->index2NullOffset, UTRIE2_INDEX_2_BLOCK_LENGTH*4);
    أرجع newBlock;
}

ساكن int32_t
getIndex2Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    إذا(U_IS_LEAD(c) && forLSCP) {
        أرجع UTRIE2_LSCP_INDEX_2_OFFSET;
    }

    i1=c>>UTRIE2_SHIFT_1;
    i2=trie->index1[i1];
    إذا(i2==trie->index2NullOffset) {
        i2=allocIndex2Block(trie);
        إذا(i2<0) {
            أرجع -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    أرجع i2;
}

ساكن int32_t
allocDataBlock(UNewTrie2 *trie, int32_t copyBlock) {
    int32_t newBlock, new_چوققا;

    إذا(trie->firstFreeBlock!=0) {
        /* get the first free block */
        newBlock=trie->firstFreeBlock;
        trie->firstFreeBlock=-trie->map[newBlock>>UTRIE2_SHIFT_2];
    } وإلا {
        /* get a new block from the high end */
        newBlock=trie->dataLength;
        new_چوققا=newBlock+UTRIE2_DATA_BLOCK_LENGTH;
        إذا(new_چوققا>trie->dataCapacity) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *data;

            إذا(trie->dataCapacity<UNEWTRIE2_MEDIUM_DATA_LENGTH) {
                capacity=UNEWTRIE2_MEDIUM_DATA_LENGTH;
            } وإلا إذا(trie->dataCapacity<UNEWTRIE2_MAX_DATA_LENGTH) {
                capacity=UNEWTRIE2_MAX_DATA_LENGTH;
            } وإلا {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                أرجع -1;
            }
            data = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            إذا(data==nullptr) {
                أرجع -1;
            }
            uprv_memcpy(data, trie->data, (size_t)trie->dataLength*4);
            uprv_free(trie->data);
            trie->data=data;
            trie->dataCapacity=capacity;
        }
        trie->dataLength=new_چوققا;
    }
    uprv_memcpy(trie->data+newBlock, trie->data+copyBlock, UTRIE2_DATA_BLOCK_LENGTH*4);
    trie->map[newBlock>>UTRIE2_SHIFT_2]=0;
    أرجع newBlock;
}

/* call when the block's reference counter reaches 0 */
ساكن فراغ
releaseDataBlock(UNewTrie2 *trie, int32_t block) {
    /* put this block at the front of the free-block chain */
    trie->map[block>>UTRIE2_SHIFT_2]=-trie->firstFreeBlock;
    trie->firstFreeBlock=block;
}

ساكن مضمن UBool
isWritableBlock(UNewTrie2 *trie, int32_t block) {
    أرجع block != trie->dataNullOffset && 1 == trie->map[block >> UTRIE2_SHIFT_2];
}

ساكن مضمن فراغ
setIndex2Entry(UNewTrie2 *trie, int32_t i2, int32_t block) {
    int32_t oldBlock;
    ++trie->map[block>>UTRIE2_SHIFT_2];  /* increment first, in case block==oldBlock! */
    oldBlock=trie->index2[i2];
    إذا(0 == --trie->map[oldBlock>>UTRIE2_SHIFT_2]) {
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
ساكن int32_t
getDataBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, oldBlock, newBlock;

    i2=getIndex2Block(trie, c, forLSCP);
    إذا(i2<0) {
        أرجع -1;  /* program error */
    }

    i2+=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
    oldBlock=trie->index2[i2];
    إذا(isWritableBlock(trie, oldBlock)) {
        أرجع oldBlock;
    }

    /* allocate a new data block */
    newBlock=allocDataBlock(trie, oldBlock);
    إذا(newBlock<0) {
        /* out of memory in the data array */
        أرجع -1;
    }
    setIndex2Entry(trie, i2, newBlock);
    أرجع newBlock;
}

/**
 * @return true if the value was successfully set
 */
ساكن فراغ
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UErrorCode *pErrorCode) {
    int32_t block;

    إذا(trie==nullptr || trie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_set(trie->t3, c, value, pErrorCode);
#endif

    block=getDataBlock(trie, c, forLSCP);
    إذا(block<0) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }

    trie->data[block+(c&UTRIE2_DATA_MASK)]=value;
}

U_CAPI فراغ U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    إذا(U_FAILURE(*pErrorCode)) {
        أرجع;
    }
    إذا((uint32_t)c>0x10ffff) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, true, value, pErrorCode);
}

U_CAPI فراغ U_EXPORT2
utrie2_set32ForLeadSurrogateCodeUnit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UErrorCode *pErrorCode) {
    إذا(U_FAILURE(*pErrorCode)) {
        أرجع;
    }
    إذا(!U_IS_LEAD(c)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    set32(trie->newTrie, c, خطأ, value, pErrorCode);
}

ساكن فراغ
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit=block+UTRIE2_DATA_BLOCK_LENGTH;
    طالما(block<limit) {
        *block++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
ساكن فراغ
fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
          uint32_t value, uint32_t initial_قىممەت, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    إذا(overwrite) {
        طالما(block<pLimit) {
            *block++=value;
        }
    } وإلا {
        طالما(block<pLimit) {
            إذا(*block==initial_قىممەت) {
                *block=value;
            }
            ++block;
        }
    }
}

U_CAPI فراغ U_EXPORT2
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

    إذا(U_FAILURE(*pErrorCode)) {
        أرجع;
    }
    إذا((uint32_t)start>0x10ffff || (uint32_t)end>0x10ffff || start>end) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==nullptr || newTrie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        أرجع;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_setRange(newTrie->t3, start, end, value, pErrorCode);
#endif
    إذا(!overwrite && value==newTrie->initial_قىممەت) {
        أرجع; /* nothing to do */
    }

    limit=end+1;
    إذا(start&UTRIE2_DATA_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=getDataBlock(newTrie, start, true);
        إذا(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }

        nextStart=(start+UTRIE2_DATA_MASK)&~UTRIE2_DATA_MASK;
        إذا(nextStart<=limit) {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, UTRIE2_DATA_BLOCK_LENGTH,
                      value, newTrie->initial_قىممەت, overwrite);
            start=nextStart;
        } وإلا {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, limit&UTRIE2_DATA_MASK,
                      value, newTrie->initial_قىممەت, overwrite);
            أرجع;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE2_DATA_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE2_DATA_MASK;

    /* iterate over all-value blocks */
    إذا(value==newTrie->initial_قىممەت) {
        repeatBlock=newTrie->dataNullOffset;
    } وإلا {
        repeatBlock=-1;
    }

    طالما(start<limit) {
        int32_t i2;
        UBool setRepeatBlock=خطأ;

        إذا(value==newTrie->initial_قىممەت && isInNullBlock(newTrie, start, true)) {
            start+=UTRIE2_DATA_BLOCK_LENGTH; /* nothing to do */
            استمر;
        }

        /* get index value */
        i2=getIndex2Block(newTrie, start, true);
        إذا(i2<0) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            أرجع;
        }
        i2+=(start>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
        block=newTrie->index2[i2];
        إذا(isWritableBlock(newTrie, block)) {
            /* already allocated */
            إذا(overwrite && block>=UNEWTRIE2_DATA_0800_OFFSET) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                setRepeatBlock=true;
            } وإلا {
                /* !overwrite, or protected block: just write the values into this block */
                fillBlock(newTrie->data+block,
                          0, UTRIE2_DATA_BLOCK_LENGTH,
                          value, newTrie->initial_قىممەت, overwrite);
            }
        } وإلا إذا(newTrie->data[block]!=value && (overwrite || block==newTrie->dataNullOffset)) {
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
            setRepeatBlock=true;
        }
        إذا(setRepeatBlock) {
            إذا(repeatBlock>=0) {
                setIndex2Entry(newTrie, i2, repeatBlock);
            } وإلا {
                /* create and set and fill the repeatBlock */
                repeatBlock=getDataBlock(newTrie, start, true);
                إذا(repeatBlock<0) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    أرجع;
                }
                writeBlock(newTrie->data+repeatBlock, value);
            }
        }

        start+=UTRIE2_DATA_BLOCK_LENGTH;
    }

    إذا(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=getDataBlock(newTrie, start, true);
        إذا(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }

        fillBlock(newTrie->data+block, 0, rest, value, newTrie->initial_قىممەت, overwrite);
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
findSameIndex2Block(ثابت int32_t *idx, int32_t index2Length, int32_t otherBlock) {
    int32_t block;

    /* ensure that we do not even partially get past index2Length */
    index2Length-=UTRIE2_INDEX_2_BLOCK_LENGTH;

    لكل(block=0; block<=index2Length; ++block) {
        إذا(equal_int32(idx+block, idx+otherBlock, UTRIE2_INDEX_2_BLOCK_LENGTH)) {
            أرجع block;
        }
    }
    أرجع -1;
}

ساكن int32_t
findSameDataBlock(ثابت uint32_t *data, int32_t dataLength, int32_t otherBlock, int32_t blockLength) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=blockLength;

    لكل(block=0; block<=dataLength; block+=UTRIE2_DATA_GRANULARITY) {
        إذا(equal_uint32(data+block, data+otherBlock, blockLength)) {
            أرجع block;
        }
    }
    أرجع -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
ساكن UChar32
find_يۇقىرى_Start(UNewTrie2 *trie, uint32_t يۇقىرى_قىممەت) {
    ثابت uint32_t *data32;

    uint32_t value, initial_قىممەت;
    UChar32 c, prev;
    int32_t i1, i2, j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    data32=trie->data;
    initial_قىممەت=trie->initial_قىممەت;

    index2NullOffset=trie->index2NullOffset;
    nullBlock=trie->dataNullOffset;

    /* set variables for previous range */
    إذا(يۇقىرى_قىممەت==initial_قىممەت) {
        prevI2Block=index2NullOffset;
        prevBlock=nullBlock;
    } وإلا {
        prevI2Block=-1;
        prevBlock=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_INDEX_1_LENGTH;
    c=prev;
    طالما(c>0) {
        i2Block=trie->index1[--i1];
        إذا(i2Block==prevI2Block) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
            استمر;
        }
        prevI2Block=i2Block;
        إذا(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            إذا(يۇقىرى_قىممەت!=initial_قىممەت) {
                أرجع c;
            }
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            لكل(i2=UTRIE2_INDEX_2_BLOCK_LENGTH; i2>0;) {
                block=trie->index2[i2Block+ --i2];
                إذا(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                    استمر;
                }
                prevBlock=block;
                إذا(block==nullBlock) {
                    /* this is the null data block */
                    إذا(يۇقىرى_قىممەت!=initial_قىممەت) {
                        أرجع c;
                    }
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                } وإلا {
                    لكل(j=UTRIE2_DATA_BLOCK_LENGTH; j>0;) {
                        value=data32[block+ --j];
                        إذا(value!=يۇقىرى_قىممەت) {
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
compactData(UNewTrie2 *trie) {
#ifdef UTRIE2_DEBUG
    int32_t سانى_Same=0, sumOverlaps=0;
#endif

    int32_t start, newStart, movedStart;
    int32_t blockLength, overlap;
    int32_t i, mapIndex, block_سانى;

    /* do not compact linear-ASCII data */
    newStart=UTRIE2_DATA_START_OFFSET;
    لكل(start=0, i=0; start<newStart; start+=UTRIE2_DATA_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    blockLength=64;
    block_سانى=blockLength>>UTRIE2_SHIFT_2;
    لكل(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        إذا(start==UNEWTRIE2_DATA_0800_OFFSET) {
            blockLength=UTRIE2_DATA_BLOCK_LENGTH;
            block_سانى=1;
        }

        /* skip blocks that are not used */
        إذا(trie->map[start>>UTRIE2_SHIFT_2]<=0) {
            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* search for an identical block */
        إذا( (movedStart=findSameDataBlock(trie->data, newStart, start, blockLength))
             >=0
        ) {
#ifdef UTRIE2_DEBUG
            ++سانى_Same;
#endif
            /* found an identical block, set the other block's index value for the current block */
            لكل(i=block_سانى, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        لكل(overlap=blockLength-UTRIE2_DATA_GRANULARITY;
            overlap>0 && !equal_uint32(trie->data+(newStart-overlap), trie->data+start, overlap);
            overlap-=UTRIE2_DATA_GRANULARITY) {}

#ifdef UTRIE2_DEBUG
            sumOverlaps+=overlap;
#endif
        إذا(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            movedStart=newStart-overlap;
            لكل(i=block_سانى, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            لكل(i=blockLength-overlap; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } وإلا /* no overlap && newStart==start */ {
            لكل(i=block_سانى, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=start;
                start+=UTRIE2_DATA_BLOCK_LENGTH;
            }
            newStart=start;
        }
    }

    /* now adjust the index-2 table */
    لكل(i=0; i<trie->index2Length; ++i) {
        إذا(i==UNEWTRIE2_INDEX_GAP_OFFSET) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_INDEX_GAP_LENGTH;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_SHIFT_2];
    }
    trie->dataNullOffset=trie->map[trie->dataNullOffset>>UTRIE2_SHIFT_2];

    /* ensure dataLength alignment */
    طالما((newStart&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        trie->data[newStart++]=trie->initial_قىممەت;
    }

#ifdef UTRIE2_DEBUG
    /* we saved some space */
    printf("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)trie->dataLength, (long)newStart, (long)سانى_Same, (long)sumOverlaps);
#endif

    trie->dataLength=newStart;
}

ساكن فراغ
compactIndex2(UNewTrie2 *trie) {
    int32_t i, start, newStart, movedStart, overlap;

    /* do not compact linear-BMP index-2 blocks */
    newStart=UTRIE2_INDEX_2_BMP_LENGTH;
    لكل(start=0, i=0; start<newStart; start+=UTRIE2_INDEX_2_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    newStart+=UTRIE2_UTF8_2B_INDEX_2_LENGTH+((trie->يۇقىرى_Start-0x10000)>>UTRIE2_SHIFT_1);

    لكل(start=UNEWTRIE2_INDEX_2_NULL_OFFSET; start<trie->index2Length;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        إذا( (movedStart=findSameIndex2Block(trie->index2, newStart, start))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=movedStart;

            /* advance start to the next block */
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        لكل(overlap=UTRIE2_INDEX_2_BLOCK_LENGTH-1;
            overlap>0 && !equal_int32(trie->index2+(newStart-overlap), trie->index2+start, overlap);
            --overlap) {}

        إذا(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=newStart-overlap;

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            لكل(i=UTRIE2_INDEX_2_BLOCK_LENGTH-overlap; i>0; --i) {
                trie->index2[newStart++]=trie->index2[start++];
            }
        } وإلا /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE2_SHIFT_1_2]=start;
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;
            newStart=start;
        }
    }

    /* now adjust the index-1 table */
    لكل(i=0; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_SHIFT_1_2];
    }
    trie->index2NullOffset=trie->map[trie->index2NullOffset>>UTRIE2_SHIFT_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    طالما((newStart&((UTRIE2_DATA_GRANULARITY-1)|1))!=0) {
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

ساكن فراغ
compactTrie(UTrie2 *trie, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UChar32 يۇقىرى_Start, supp_يۇقىرى_Start;
    uint32_t يۇقىرى_قىممەت;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    يۇقىرى_قىممەت=utrie2_get32(trie, 0x10ffff);
    يۇقىرى_Start=find_يۇقىرى_Start(newTrie, يۇقىرى_قىممەت);
    يۇقىرى_Start=(يۇقىرى_Start+(UTRIE2_CP_PER_INDEX_1_ENTRY-1))&~(UTRIE2_CP_PER_INDEX_1_ENTRY-1);
    إذا(يۇقىرى_Start==0x110000) {
        يۇقىرى_قىممەت=trie->error_قىممەت;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->يۇقىرى_Start=newTrie->يۇقىرى_Start=يۇقىرى_Start;

#ifdef UTRIE2_DEBUG
    printf("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)يۇقىرى_Start, (long)يۇقىرى_قىممەت, (long)trie->initial_قىممەت);
#endif

    إذا(يۇقىرى_Start<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_يۇقىرى_Start= يۇقىرى_Start<=0x10000 ? 0x10000 : يۇقىرى_Start;
        utrie2_setRange32(trie, supp_يۇقىرى_Start, 0x10ffff, trie->initial_قىممەت, true, pErrorCode);
        إذا(U_FAILURE(*pErrorCode)) {
            أرجع;
        }
    }

    compactData(newTrie);
    إذا(يۇقىرى_Start>0x10000) {
        compactIndex2(newTrie);
#ifdef UTRIE2_DEBUG
    } وإلا {
        printf("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (long)يۇقىرى_Start, (long)trie->newTrie->index2Length, (long)UTRIE2_INDEX_1_OFFSET);
#endif
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->data[newTrie->dataLength++]=يۇقىرى_قىممەت;
    طالما((newTrie->dataLength&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        newTrie->data[newTrie->dataLength++]=trie->initial_قىممەت;
    }

    newTrie->isCompacted=true;
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
U_CAPI فراغ U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_قىممەت_Bits قىممەت_Bits, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t allIndexesLength;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */
    UChar32 يۇقىرى_Start;

    /* argument check */
    إذا(U_FAILURE(*pErrorCode)) {
        أرجع;
    }
    إذا( trie==nullptr ||
        قىممەت_Bits<0 || UTRIE2_سانى_قىممەت_BITS<=قىممەت_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    newTrie=trie->newTrie;
    إذا(newTrie==nullptr) {
        /* already frozen */
        UTrie2_قىممەت_Bits frozen_قىممەت_Bits=
            trie->data16!=nullptr ? UTRIE2_16_قىممەت_BITS : UTRIE2_32_قىممەت_BITS;
        إذا(قىممەت_Bits!=frozen_قىممەت_Bits) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
        أرجع;
    }

    /* compact if necessary */
    إذا(!newTrie->isCompacted) {
        compactTrie(trie, pErrorCode);
        إذا(U_FAILURE(*pErrorCode)) {
            أرجع;
        }
    }
    يۇقىرى_Start=trie->يۇقىرى_Start;

    إذا(يۇقىرى_Start<=0x10000) {
        allIndexesLength=UTRIE2_INDEX_1_OFFSET;
    } وإلا {
        allIndexesLength=newTrie->index2Length;
    }
    إذا(قىممەت_Bits==UTRIE2_16_قىممەت_BITS) {
        dataMove=allIndexesLength;
    } وإلا {
        dataMove=0;
    }

    /* are indexLength and dataLength within limits? */
    إذا( /* for unshifted indexLength */
        allIndexesLength>UTRIE2_MAX_INDEX_LENGTH ||
        /* for unshifted dataNullOffset */
        (dataMove+newTrie->dataNullOffset)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (dataMove+UNEWTRIE2_DATA_0800_OFFSET)>0xffff ||
        /* for shiftedDataLength */
        (dataMove+newTrie->dataLength)>UTRIE2_MAX_DATA_LENGTH
    ) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    /* calculate the total serialized length */
    length=حجم(UTrie2Header)+allIndexesLength*2;
    إذا(قىممەت_Bits==UTRIE2_16_قىممەت_BITS) {
        length+=newTrie->dataLength*2;
    } وإلا {
        length+=newTrie->dataLength*4;
    }

    trie->memory=uprv_malloc(length);
    إذا(trie->memory==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    trie->length=length;
    trie->isMemoryOwned=true;

    trie->indexLength=allIndexesLength;
    trie->dataLength=newTrie->dataLength;
    إذا(يۇقىرى_Start<=0x10000) {
        trie->index2NullOffset=0xffff;
    } وإلا {
        trie->index2NullOffset=static_cast<uint16_t>(UTRIE2_INDEX_2_OFFSET+newTrie->index2NullOffset);
    }
    trie->dataNullOffset=(uint16_t)(dataMove+newTrie->dataNullOffset);
    trie->يۇقىرى_قىممەت_Index=dataMove+trie->dataLength-UTRIE2_DATA_GRANULARITY;

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)قىممەت_Bits;

    header->indexLength=(uint16_t)trie->indexLength;
    header->shiftedDataLength=(uint16_t)(trie->dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=trie->index2NullOffset;
    header->dataNullOffset=trie->dataNullOffset;
    header->shifted_يۇقىرى_Start=(uint16_t)(يۇقىرى_Start>>UTRIE2_SHIFT_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    لكل(i=UTRIE2_INDEX_2_BMP_LENGTH; i>0; --i) {
        *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(dataMove+newTrie->index2[i<<(6-UTRIE2_SHIFT_2)]);
    }

    إذا(يۇقىرى_Start>0x10000) {
        int32_t index1Length=(يۇقىرى_Start-0x10000)>>UTRIE2_SHIFT_1;
        int32_t index2Offset=UTRIE2_INDEX_2_BMP_LENGTH+UTRIE2_UTF8_2B_INDEX_2_LENGTH+index1Length;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        لكل(i=index1Length; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_INDEX_2_OFFSET + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2Offset;
        لكل(i=newTrie->index2Length-index2Offset; i>0; --i) {
            *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
        }
    }

    /* write the 16/32-bit data array */
    بدل(قىممەت_Bits) {
    حالة UTRIE2_16_قىممەت_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        p=newTrie->data;
        لكل(i=newTrie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        اكسر;
    حالة UTRIE2_32_قىممەت_BITS:
        /* write 32-bit data values */
        trie->data16=nullptr;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->data, (size_t)newTrie->dataLength*4);
        اكسر;
    افتراضي:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

#ifdef UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#endif

#ifdef UCPTRIE_DEBUG
    umutablecptrie_setName(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_قىممەت_Width)قىممەت_Bits, pErrorCode));
#endif
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->data);
    uprv_free(newTrie);
    trie->newTrie=nullptr;
}

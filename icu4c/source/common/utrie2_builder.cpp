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

#અન્તર્ભાવ "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/umutablecptrie.h"
#અન્તર્ભાવ "ucptrie_impl.h"
#endif
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "utrie2.h"
#અન્તર્ભાવ "utrie2_impl.h"

#અન્તર્ભાવ "utrie.h"  // for utrie2_fromUTrie()

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

ગણના {
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

સ્થિર int32_t
allocIndex2Block(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_કિંમત, uint32_t error_કિંમત, UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *data;
    int32_t i, j;

    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(કદ(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(કદ(UNewTrie2));
    data=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_DATA_LENGTH*4);
    જો(trie==nullptr || newTrie==nullptr || data==nullptr) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(data);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }

    uprv_memset(trie, 0, કદ(UTrie2));
    trie->initial_કિંમત=initial_કિંમત;
    trie->error_કિંમત=error_કિંમત;
    trie->ઉપલું_Start=0x110000;
    trie->newTrie=newTrie;
#ifdef UTRIE2_DEBUG
    trie->name="open";
#endif

    newTrie->data=data;
#ifdef UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_કિંમત, error_કિંમત, pErrorCode);
#endif
    newTrie->dataCapacity=UNEWTRIE2_INITIAL_DATA_LENGTH;
    newTrie->initial_કિંમત=initial_કિંમત;
    newTrie->error_કિંમત=error_કિંમત;
    newTrie->ઉપલું_Start=0x110000;
    newTrie->firstFreeBlock=0;  /* no free block in the list */
    newTrie->isCompacted=ખોટું;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    માટે(i=0; i<0x80; ++i) {
        newTrie->data[i]=initial_કિંમત;
    }
    માટે(; i<0xc0; ++i) {
        newTrie->data[i]=error_કિંમત;
    }
    માટે(i=UNEWTRIE2_DATA_NULL_OFFSET; i<UNEWTRIE2_DATA_START_OFFSET; ++i) {
        newTrie->data[i]=initial_કિંમત;
    }
    newTrie->dataNullOffset=UNEWTRIE2_DATA_NULL_OFFSET;
    newTrie->dataLength=UNEWTRIE2_DATA_START_OFFSET;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    માટે(i=0, j=0; j<0x80; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    માટે(; j<0xc0; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
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
    માટે(; j<UNEWTRIE2_DATA_START_OFFSET; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    માટે(i=0x80>>UTRIE2_SHIFT_2; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        newTrie->index2[i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    માટે(i=0; i<UNEWTRIE2_INDEX_GAP_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_GAP_OFFSET+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    માટે(i=0; i<UTRIE2_INDEX_2_BLOCK_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_2_NULL_OFFSET+i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }
    newTrie->index2NullOffset=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    newTrie->index2Length=UNEWTRIE2_INDEX_2_START_OFFSET;

    /* set the index-1 indexes for the linear index-2 block */
    માટે(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        ++i, j+=UTRIE2_INDEX_2_BLOCK_LENGTH
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    માટે(; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        newTrie->index1[i]=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    માટે(i=0x80; i<0x800; i+=UTRIE2_DATA_BLOCK_LENGTH) {
        utrie2_set32(trie, i, initial_કિંમત, pErrorCode);
    }

    પાછા trie;
}

સ્થિર UNewTrie2 *
cloneBuilder(અચલ UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(કદ(UNewTrie2)));
    જો(trie==nullptr) {
        પાછા nullptr;
    }

    trie->data = static_cast<uint32_t*>(uprv_malloc(other->dataCapacity * 4));
    જો(trie->data==nullptr) {
        uprv_free(trie);
        પાછા nullptr;
    }
#ifdef UCPTRIE_DEBUG
    જો(other->t3==nullptr) {
        trie->t3=nullptr;
    } નહીંતર {
        UErrorCode errorCode=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &errorCode);
    }
#endif
    trie->dataCapacity=other->dataCapacity;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, કદ(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2Length*4);
    trie->index2NullOffset=other->index2NullOffset;
    trie->index2Length=other->index2Length;

    uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
    trie->dataNullOffset=other->dataNullOffset;
    trie->dataLength=other->dataLength;

    /* reference counters */
    જો(other->isCompacted) {
        trie->firstFreeBlock=0;
    } નહીંતર {
        uprv_memcpy(trie->map, other->map, ((size_t)other->dataLength>>UTRIE2_SHIFT_2)*4);
        trie->firstFreeBlock=other->firstFreeBlock;
    }

    trie->initial_કિંમત=other->initial_કિંમત;
    trie->error_કિંમત=other->error_કિંમત;
    trie->ઉપલું_Start=other->ઉપલું_Start;
    trie->isCompacted=other->isCompacted;

    પાછા trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(અચલ UTrie2 *other, UErrorCode *pErrorCode) {
    UTrie2 *trie;

    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    જો(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(કદ(UTrie2));
    જો(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    uprv_memcpy(trie, other, કદ(UTrie2));

    જો(other->memory!=nullptr) {
        trie->memory=uprv_malloc(other->length);
        જો(trie->memory!=nullptr) {
            trie->isMemoryOwned=સાચું;
            uprv_memcpy(trie->memory, other->memory, other->length);

            /* make the clone's pointers point to its own memory */
            trie->index=(uint16_t *)trie->memory+(other->index-(uint16_t *)other->memory);
            જો(other->data16!=nullptr) {
                trie->data16=(uint16_t *)trie->memory+(other->data16-(uint16_t *)other->memory);
            }
            જો(other->data32!=nullptr) {
                trie->data32=(uint32_t *)trie->memory+(other->data32-(uint32_t *)other->memory);
            }
        }
    } નહીંતર /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    જો(trie->memory==nullptr && trie->newTrie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        uprv_free(trie);
        trie=nullptr;
    }
    પાછા trie;
}

પ્રકારવ્યાખ્યા સંરચના NewTrieAndStatus {
    UTrie2 *trie;
    UErrorCode errorCode;
    UBool exclusiveLimit;  /* rather than inclusive range end */
} NewTrieAndStatus;

સ્થિર UBool U_CALLCONV
copyEnumRange(અચલ શૂન્ય *context, UChar32 start, UChar32 end, uint32_t value) {
    NewTrieAndStatus *nt=(NewTrieAndStatus *)context;
    જો(value!=nt->trie->initial_કિંમત) {
        જો(nt->exclusiveLimit) {
            --end;
        }
        જો(start==end) {
            utrie2_set32(nt->trie, start, value, &nt->errorCode);
        } નહીંતર {
            utrie2_setRange32(nt->trie, start, end, value, સાચું, &nt->errorCode);
        }
        પાછા U_SUCCESS(nt->errorCode);
    } નહીંતર {
        પાછા સાચું;
    }
}

#ifdef UTRIE2_DEBUG
સ્થિર long ગણતરી_Initial(અચલ UTrie2 *trie) {
    uint32_t initial_કિંમત=trie->initial_કિંમત;
    int32_t length=trie->dataLength;
    long count=0;
    જો(trie->data16!=nullptr) {
        માટે(int32_t i=0; i<length; ++i) {
            જો(trie->data16[i]==initial_કિંમત) { ++count; }
        }
    } નહીંતર {
        માટે(int32_t i=0; i<length; ++i) {
            જો(trie->data32[i]==initial_કિંમત) { ++count; }
        }
    }
    પાછા count;
}

સ્થિર શૂન્ય
utrie_printLengths(અચલ UTrie *trie) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)કદ(UTrieHeader)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           indexLength, dataLength, totalLength);
}

સ્થિર શૂન્ય
utrie2_printLengths(અચલ UTrie2 *trie, અચલ અક્ષર *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)કદ(UTrie2Header)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, ગણતરી_Initial(trie), totalLength);
}
#endif

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(અચલ UTrie2 *other, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    જો(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    જો(other->newTrie!=nullptr && !other->newTrie->isCompacted) {
        પાછા utrie2_clone(other, pErrorCode);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_કિંમત, other->error_કિંમત, pErrorCode);
    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    context.exclusiveLimit=ખોટું;
    context.errorCode=*pErrorCode;
    utrie2_enum(other, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    માટે(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        જો(other->data32==nullptr) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } નહીંતર {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        જો(value!=other->initial_કિંમત) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    જો(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    પાછા context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(અચલ UTrie *trie1, uint32_t error_કિંમત, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    જો(trie1==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    context.trie=utrie2_open(trie1->initial_કિંમત, error_કિંમત, pErrorCode);
    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }
    context.exclusiveLimit=સાચું;
    context.errorCode=*pErrorCode;
    utrie_enum(trie1, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    માટે(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        જો(trie1->data32==nullptr) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } નહીંતર {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        જો(value!=trie1->initial_કિંમત) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    જો(U_SUCCESS(*pErrorCode)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=nullptr ? UTRIE2_32_કિંમત_BITS : UTRIE2_16_કિંમત_BITS,
                      pErrorCode);
    }
#ifdef UTRIE2_DEBUG
    જો(U_SUCCESS(*pErrorCode)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#endif
    જો(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    પાછા context.trie;
}

સ્થિર અંતઃરેખ UBool
isInNullBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, block;

    જો(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } નહીંતર {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    પાછા block == trie->dataNullOffset;
}

સ્થિર int32_t
allocIndex2Block(UNewTrie2 *trie) {
    int32_t newBlock, new_ટોચ;

    newBlock=trie->index2Length;
    new_ટોચ=newBlock+UTRIE2_INDEX_2_BLOCK_LENGTH;
    જો(new_ટોચ>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        પાછા -1;
    }
    trie->index2Length=new_ટોચ;
    uprv_memcpy(trie->index2+newBlock, trie->index2+trie->index2NullOffset, UTRIE2_INDEX_2_BLOCK_LENGTH*4);
    પાછા newBlock;
}

સ્થિર int32_t
getIndex2Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    જો(U_IS_LEAD(c) && forLSCP) {
        પાછા UTRIE2_LSCP_INDEX_2_OFFSET;
    }

    i1=c>>UTRIE2_SHIFT_1;
    i2=trie->index1[i1];
    જો(i2==trie->index2NullOffset) {
        i2=allocIndex2Block(trie);
        જો(i2<0) {
            પાછા -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    પાછા i2;
}

સ્થિર int32_t
allocDataBlock(UNewTrie2 *trie, int32_t copyBlock) {
    int32_t newBlock, new_ટોચ;

    જો(trie->firstFreeBlock!=0) {
        /* get the first free block */
        newBlock=trie->firstFreeBlock;
        trie->firstFreeBlock=-trie->map[newBlock>>UTRIE2_SHIFT_2];
    } નહીંતર {
        /* get a new block from the high end */
        newBlock=trie->dataLength;
        new_ટોચ=newBlock+UTRIE2_DATA_BLOCK_LENGTH;
        જો(new_ટોચ>trie->dataCapacity) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *data;

            જો(trie->dataCapacity<UNEWTRIE2_MEDIUM_DATA_LENGTH) {
                capacity=UNEWTRIE2_MEDIUM_DATA_LENGTH;
            } નહીંતર જો(trie->dataCapacity<UNEWTRIE2_MAX_DATA_LENGTH) {
                capacity=UNEWTRIE2_MAX_DATA_LENGTH;
            } નહીંતર {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                પાછા -1;
            }
            data = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            જો(data==nullptr) {
                પાછા -1;
            }
            uprv_memcpy(data, trie->data, (size_t)trie->dataLength*4);
            uprv_free(trie->data);
            trie->data=data;
            trie->dataCapacity=capacity;
        }
        trie->dataLength=new_ટોચ;
    }
    uprv_memcpy(trie->data+newBlock, trie->data+copyBlock, UTRIE2_DATA_BLOCK_LENGTH*4);
    trie->map[newBlock>>UTRIE2_SHIFT_2]=0;
    પાછા newBlock;
}

/* call when the block's reference counter reaches 0 */
સ્થિર શૂન્ય
releaseDataBlock(UNewTrie2 *trie, int32_t block) {
    /* put this block at the front of the free-block chain */
    trie->map[block>>UTRIE2_SHIFT_2]=-trie->firstFreeBlock;
    trie->firstFreeBlock=block;
}

સ્થિર અંતઃરેખ UBool
isWritableBlock(UNewTrie2 *trie, int32_t block) {
    પાછા block != trie->dataNullOffset && 1 == trie->map[block >> UTRIE2_SHIFT_2];
}

સ્થિર અંતઃરેખ શૂન્ય
setIndex2Entry(UNewTrie2 *trie, int32_t i2, int32_t block) {
    int32_t oldBlock;
    ++trie->map[block>>UTRIE2_SHIFT_2];  /* increment first, in case block==oldBlock! */
    oldBlock=trie->index2[i2];
    જો(0 == --trie->map[oldBlock>>UTRIE2_SHIFT_2]) {
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
સ્થિર int32_t
getDataBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, oldBlock, newBlock;

    i2=getIndex2Block(trie, c, forLSCP);
    જો(i2<0) {
        પાછા -1;  /* program error */
    }

    i2+=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
    oldBlock=trie->index2[i2];
    જો(isWritableBlock(trie, oldBlock)) {
        પાછા oldBlock;
    }

    /* allocate a new data block */
    newBlock=allocDataBlock(trie, oldBlock);
    જો(newBlock<0) {
        /* out of memory in the data array */
        પાછા -1;
    }
    setIndex2Entry(trie, i2, newBlock);
    પાછા newBlock;
}

/**
 * @return true if the value was successfully set
 */
સ્થિર શૂન્ય
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UErrorCode *pErrorCode) {
    int32_t block;

    જો(trie==nullptr || trie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        પાછા;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_set(trie->t3, c, value, pErrorCode);
#endif

    block=getDataBlock(trie, c, forLSCP);
    જો(block<0) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }

    trie->data[block+(c&UTRIE2_DATA_MASK)]=value;
}

U_CAPI શૂન્ય U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    જો((uint32_t)c>0x10ffff) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    set32(trie->newTrie, c, સાચું, value, pErrorCode);
}

U_CAPI શૂન્ય U_EXPORT2
utrie2_set32ForLeadSurrogateCodeUnit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UErrorCode *pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    જો(!U_IS_LEAD(c)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    set32(trie->newTrie, c, ખોટું, value, pErrorCode);
}

સ્થિર શૂન્ય
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit=block+UTRIE2_DATA_BLOCK_LENGTH;
    જ્યારે(block<limit) {
        *block++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
સ્થિર શૂન્ય
fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
          uint32_t value, uint32_t initial_કિંમત, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    જો(overwrite) {
        જ્યારે(block<pLimit) {
            *block++=value;
        }
    } નહીંતર {
        જ્યારે(block<pLimit) {
            જો(*block==initial_કિંમત) {
                *block=value;
            }
            ++block;
        }
    }
}

U_CAPI શૂન્ય U_EXPORT2
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

    જો(U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    જો((uint32_t)start>0x10ffff || (uint32_t)end>0x10ffff || start>end) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    newTrie=trie->newTrie;
    જો(newTrie==nullptr || newTrie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        પાછા;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_setRange(newTrie->t3, start, end, value, pErrorCode);
#endif
    જો(!overwrite && value==newTrie->initial_કિંમત) {
        પાછા; /* nothing to do */
    }

    limit=end+1;
    જો(start&UTRIE2_DATA_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=getDataBlock(newTrie, start, સાચું);
        જો(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }

        nextStart=(start+UTRIE2_DATA_MASK)&~UTRIE2_DATA_MASK;
        જો(nextStart<=limit) {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, UTRIE2_DATA_BLOCK_LENGTH,
                      value, newTrie->initial_કિંમત, overwrite);
            start=nextStart;
        } નહીંતર {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, limit&UTRIE2_DATA_MASK,
                      value, newTrie->initial_કિંમત, overwrite);
            પાછા;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE2_DATA_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE2_DATA_MASK;

    /* iterate over all-value blocks */
    જો(value==newTrie->initial_કિંમત) {
        repeatBlock=newTrie->dataNullOffset;
    } નહીંતર {
        repeatBlock=-1;
    }

    જ્યારે(start<limit) {
        int32_t i2;
        UBool setRepeatBlock=ખોટું;

        જો(value==newTrie->initial_કિંમત && isInNullBlock(newTrie, start, સાચું)) {
            start+=UTRIE2_DATA_BLOCK_LENGTH; /* nothing to do */
            ચાલુ;
        }

        /* get index value */
        i2=getIndex2Block(newTrie, start, સાચું);
        જો(i2<0) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            પાછા;
        }
        i2+=(start>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
        block=newTrie->index2[i2];
        જો(isWritableBlock(newTrie, block)) {
            /* already allocated */
            જો(overwrite && block>=UNEWTRIE2_DATA_0800_OFFSET) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                setRepeatBlock=સાચું;
            } નહીંતર {
                /* !overwrite, or protected block: just write the values into this block */
                fillBlock(newTrie->data+block,
                          0, UTRIE2_DATA_BLOCK_LENGTH,
                          value, newTrie->initial_કિંમત, overwrite);
            }
        } નહીંતર જો(newTrie->data[block]!=value && (overwrite || block==newTrie->dataNullOffset)) {
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
            setRepeatBlock=સાચું;
        }
        જો(setRepeatBlock) {
            જો(repeatBlock>=0) {
                setIndex2Entry(newTrie, i2, repeatBlock);
            } નહીંતર {
                /* create and set and fill the repeatBlock */
                repeatBlock=getDataBlock(newTrie, start, સાચું);
                જો(repeatBlock<0) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    પાછા;
                }
                writeBlock(newTrie->data+repeatBlock, value);
            }
        }

        start+=UTRIE2_DATA_BLOCK_LENGTH;
    }

    જો(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=getDataBlock(newTrie, start, સાચું);
        જો(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }

        fillBlock(newTrie->data+block, 0, rest, value, newTrie->initial_કિંમત, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

સ્થિર અંતઃરેખ UBool
equal_int32(અચલ int32_t *s, અચલ int32_t *t, int32_t length) {
    જ્યારે(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    પાછા length == 0;
}

સ્થિર અંતઃરેખ UBool
equal_uint32(અચલ uint32_t *s, અચલ uint32_t *t, int32_t length) {
    જ્યારે(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    પાછા length == 0;
}

સ્થિર int32_t
findSameIndex2Block(અચલ int32_t *idx, int32_t index2Length, int32_t otherBlock) {
    int32_t block;

    /* ensure that we do not even partially get past index2Length */
    index2Length-=UTRIE2_INDEX_2_BLOCK_LENGTH;

    માટે(block=0; block<=index2Length; ++block) {
        જો(equal_int32(idx+block, idx+otherBlock, UTRIE2_INDEX_2_BLOCK_LENGTH)) {
            પાછા block;
        }
    }
    પાછા -1;
}

સ્થિર int32_t
findSameDataBlock(અચલ uint32_t *data, int32_t dataLength, int32_t otherBlock, int32_t blockLength) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=blockLength;

    માટે(block=0; block<=dataLength; block+=UTRIE2_DATA_GRANULARITY) {
        જો(equal_uint32(data+block, data+otherBlock, blockLength)) {
            પાછા block;
        }
    }
    પાછા -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
સ્થિર UChar32
find_ઉપલું_Start(UNewTrie2 *trie, uint32_t ઉપલું_કિંમત) {
    અચલ uint32_t *data32;

    uint32_t value, initial_કિંમત;
    UChar32 c, prev;
    int32_t i1, i2, j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    data32=trie->data;
    initial_કિંમત=trie->initial_કિંમત;

    index2NullOffset=trie->index2NullOffset;
    nullBlock=trie->dataNullOffset;

    /* set variables for previous range */
    જો(ઉપલું_કિંમત==initial_કિંમત) {
        prevI2Block=index2NullOffset;
        prevBlock=nullBlock;
    } નહીંતર {
        prevI2Block=-1;
        prevBlock=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_INDEX_1_LENGTH;
    c=prev;
    જ્યારે(c>0) {
        i2Block=trie->index1[--i1];
        જો(i2Block==prevI2Block) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
            ચાલુ;
        }
        prevI2Block=i2Block;
        જો(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            જો(ઉપલું_કિંમત!=initial_કિંમત) {
                પાછા c;
            }
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } નહીંતર {
            /* enumerate data blocks for one index-2 block */
            માટે(i2=UTRIE2_INDEX_2_BLOCK_LENGTH; i2>0;) {
                block=trie->index2[i2Block+ --i2];
                જો(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                    ચાલુ;
                }
                prevBlock=block;
                જો(block==nullBlock) {
                    /* this is the null data block */
                    જો(ઉપલું_કિંમત!=initial_કિંમત) {
                        પાછા c;
                    }
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                } નહીંતર {
                    માટે(j=UTRIE2_DATA_BLOCK_LENGTH; j>0;) {
                        value=data32[block+ --j];
                        જો(value!=ઉપલું_કિંમત) {
                            પાછા c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    પાછા 0;
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
સ્થિર શૂન્ય
compactData(UNewTrie2 *trie) {
#ifdef UTRIE2_DEBUG
    int32_t ગણતરી_Same=0, sumOverlaps=0;
#endif

    int32_t start, newStart, movedStart;
    int32_t blockLength, overlap;
    int32_t i, mapIndex, block_ગણતરી;

    /* do not compact linear-ASCII data */
    newStart=UTRIE2_DATA_START_OFFSET;
    માટે(start=0, i=0; start<newStart; start+=UTRIE2_DATA_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    blockLength=64;
    block_ગણતરી=blockLength>>UTRIE2_SHIFT_2;
    માટે(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        જો(start==UNEWTRIE2_DATA_0800_OFFSET) {
            blockLength=UTRIE2_DATA_BLOCK_LENGTH;
            block_ગણતરી=1;
        }

        /* skip blocks that are not used */
        જો(trie->map[start>>UTRIE2_SHIFT_2]<=0) {
            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            ચાલુ;
        }

        /* search for an identical block */
        જો( (movedStart=findSameDataBlock(trie->data, newStart, start, blockLength))
             >=0
        ) {
#ifdef UTRIE2_DEBUG
            ++ગણતરી_Same;
#endif
            /* found an identical block, set the other block's index value for the current block */
            માટે(i=block_ગણતરી, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            ચાલુ;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        માટે(overlap=blockLength-UTRIE2_DATA_GRANULARITY;
            overlap>0 && !equal_uint32(trie->data+(newStart-overlap), trie->data+start, overlap);
            overlap-=UTRIE2_DATA_GRANULARITY) {}

#ifdef UTRIE2_DEBUG
            sumOverlaps+=overlap;
#endif
        જો(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            movedStart=newStart-overlap;
            માટે(i=block_ગણતરી, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            માટે(i=blockLength-overlap; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } નહીંતર /* no overlap && newStart==start */ {
            માટે(i=block_ગણતરી, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=start;
                start+=UTRIE2_DATA_BLOCK_LENGTH;
            }
            newStart=start;
        }
    }

    /* now adjust the index-2 table */
    માટે(i=0; i<trie->index2Length; ++i) {
        જો(i==UNEWTRIE2_INDEX_GAP_OFFSET) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_INDEX_GAP_LENGTH;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_SHIFT_2];
    }
    trie->dataNullOffset=trie->map[trie->dataNullOffset>>UTRIE2_SHIFT_2];

    /* ensure dataLength alignment */
    જ્યારે((newStart&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        trie->data[newStart++]=trie->initial_કિંમત;
    }

#ifdef UTRIE2_DEBUG
    /* we saved some space */
    printf("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)trie->dataLength, (long)newStart, (long)ગણતરી_Same, (long)sumOverlaps);
#endif

    trie->dataLength=newStart;
}

સ્થિર શૂન્ય
compactIndex2(UNewTrie2 *trie) {
    int32_t i, start, newStart, movedStart, overlap;

    /* do not compact linear-BMP index-2 blocks */
    newStart=UTRIE2_INDEX_2_BMP_LENGTH;
    માટે(start=0, i=0; start<newStart; start+=UTRIE2_INDEX_2_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    newStart+=UTRIE2_UTF8_2B_INDEX_2_LENGTH+((trie->ઉપલું_Start-0x10000)>>UTRIE2_SHIFT_1);

    માટે(start=UNEWTRIE2_INDEX_2_NULL_OFFSET; start<trie->index2Length;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        જો( (movedStart=findSameIndex2Block(trie->index2, newStart, start))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=movedStart;

            /* advance start to the next block */
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            ચાલુ;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        માટે(overlap=UTRIE2_INDEX_2_BLOCK_LENGTH-1;
            overlap>0 && !equal_int32(trie->index2+(newStart-overlap), trie->index2+start, overlap);
            --overlap) {}

        જો(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=newStart-overlap;

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            માટે(i=UTRIE2_INDEX_2_BLOCK_LENGTH-overlap; i>0; --i) {
                trie->index2[newStart++]=trie->index2[start++];
            }
        } નહીંતર /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE2_SHIFT_1_2]=start;
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;
            newStart=start;
        }
    }

    /* now adjust the index-1 table */
    માટે(i=0; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_SHIFT_1_2];
    }
    trie->index2NullOffset=trie->map[trie->index2NullOffset>>UTRIE2_SHIFT_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    જ્યારે((newStart&((UTRIE2_DATA_GRANULARITY-1)|1))!=0) {
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

સ્થિર શૂન્ય
compactTrie(UTrie2 *trie, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UChar32 ઉપલું_Start, supp_ઉપલું_Start;
    uint32_t ઉપલું_કિંમત;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    ઉપલું_કિંમત=utrie2_get32(trie, 0x10ffff);
    ઉપલું_Start=find_ઉપલું_Start(newTrie, ઉપલું_કિંમત);
    ઉપલું_Start=(ઉપલું_Start+(UTRIE2_CP_PER_INDEX_1_ENTRY-1))&~(UTRIE2_CP_PER_INDEX_1_ENTRY-1);
    જો(ઉપલું_Start==0x110000) {
        ઉપલું_કિંમત=trie->error_કિંમત;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->ઉપલું_Start=newTrie->ઉપલું_Start=ઉપલું_Start;

#ifdef UTRIE2_DEBUG
    printf("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)ઉપલું_Start, (long)ઉપલું_કિંમત, (long)trie->initial_કિંમત);
#endif

    જો(ઉપલું_Start<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_ઉપલું_Start= ઉપલું_Start<=0x10000 ? 0x10000 : ઉપલું_Start;
        utrie2_setRange32(trie, supp_ઉપલું_Start, 0x10ffff, trie->initial_કિંમત, સાચું, pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            પાછા;
        }
    }

    compactData(newTrie);
    જો(ઉપલું_Start>0x10000) {
        compactIndex2(newTrie);
#ifdef UTRIE2_DEBUG
    } નહીંતર {
        printf("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (long)ઉપલું_Start, (long)trie->newTrie->index2Length, (long)UTRIE2_INDEX_1_OFFSET);
#endif
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->data[newTrie->dataLength++]=ઉપલું_કિંમત;
    જ્યારે((newTrie->dataLength&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        newTrie->data[newTrie->dataLength++]=trie->initial_કિંમત;
    }

    newTrie->isCompacted=સાચું;
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
U_CAPI શૂન્ય U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_કિંમત_Bits કિંમત_Bits, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t allIndexesLength;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */
    UChar32 ઉપલું_Start;

    /* argument check */
    જો(U_FAILURE(*pErrorCode)) {
        પાછા;
    }
    જો( trie==nullptr ||
        કિંમત_Bits<0 || UTRIE2_ગણતરી_કિંમત_BITS<=કિંમત_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    newTrie=trie->newTrie;
    જો(newTrie==nullptr) {
        /* already frozen */
        UTrie2_કિંમત_Bits frozen_કિંમત_Bits=
            trie->data16!=nullptr ? UTRIE2_16_કિંમત_BITS : UTRIE2_32_કિંમત_BITS;
        જો(કિંમત_Bits!=frozen_કિંમત_Bits) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
        પાછા;
    }

    /* compact if necessary */
    જો(!newTrie->isCompacted) {
        compactTrie(trie, pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            પાછા;
        }
    }
    ઉપલું_Start=trie->ઉપલું_Start;

    જો(ઉપલું_Start<=0x10000) {
        allIndexesLength=UTRIE2_INDEX_1_OFFSET;
    } નહીંતર {
        allIndexesLength=newTrie->index2Length;
    }
    જો(કિંમત_Bits==UTRIE2_16_કિંમત_BITS) {
        dataMove=allIndexesLength;
    } નહીંતર {
        dataMove=0;
    }

    /* are indexLength and dataLength within limits? */
    જો( /* for unshifted indexLength */
        allIndexesLength>UTRIE2_MAX_INDEX_LENGTH ||
        /* for unshifted dataNullOffset */
        (dataMove+newTrie->dataNullOffset)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (dataMove+UNEWTRIE2_DATA_0800_OFFSET)>0xffff ||
        /* for shiftedDataLength */
        (dataMove+newTrie->dataLength)>UTRIE2_MAX_DATA_LENGTH
    ) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા;
    }

    /* calculate the total serialized length */
    length=કદ(UTrie2Header)+allIndexesLength*2;
    જો(કિંમત_Bits==UTRIE2_16_કિંમત_BITS) {
        length+=newTrie->dataLength*2;
    } નહીંતર {
        length+=newTrie->dataLength*4;
    }

    trie->memory=uprv_malloc(length);
    જો(trie->memory==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    trie->length=length;
    trie->isMemoryOwned=સાચું;

    trie->indexLength=allIndexesLength;
    trie->dataLength=newTrie->dataLength;
    જો(ઉપલું_Start<=0x10000) {
        trie->index2NullOffset=0xffff;
    } નહીંતર {
        trie->index2NullOffset=static_cast<uint16_t>(UTRIE2_INDEX_2_OFFSET+newTrie->index2NullOffset);
    }
    trie->dataNullOffset=(uint16_t)(dataMove+newTrie->dataNullOffset);
    trie->ઉપલું_કિંમત_Index=dataMove+trie->dataLength-UTRIE2_DATA_GRANULARITY;

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)કિંમત_Bits;

    header->indexLength=(uint16_t)trie->indexLength;
    header->shiftedDataLength=(uint16_t)(trie->dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=trie->index2NullOffset;
    header->dataNullOffset=trie->dataNullOffset;
    header->shifted_ઉપલું_Start=(uint16_t)(ઉપલું_Start>>UTRIE2_SHIFT_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    માટે(i=UTRIE2_INDEX_2_BMP_LENGTH; i>0; --i) {
        *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    માટે(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    માટે(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(dataMove+newTrie->index2[i<<(6-UTRIE2_SHIFT_2)]);
    }

    જો(ઉપલું_Start>0x10000) {
        int32_t index1Length=(ઉપલું_Start-0x10000)>>UTRIE2_SHIFT_1;
        int32_t index2Offset=UTRIE2_INDEX_2_BMP_LENGTH+UTRIE2_UTF8_2B_INDEX_2_LENGTH+index1Length;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        માટે(i=index1Length; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_INDEX_2_OFFSET + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2Offset;
        માટે(i=newTrie->index2Length-index2Offset; i>0; --i) {
            *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
        }
    }

    /* write the 16/32-bit data array */
    પસંદગી(કિંમત_Bits) {
    સ્થિતિ UTRIE2_16_કિંમત_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        p=newTrie->data;
        માટે(i=newTrie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        તોડો;
    સ્થિતિ UTRIE2_32_કિંમત_BITS:
        /* write 32-bit data values */
        trie->data16=nullptr;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->data, (size_t)newTrie->dataLength*4);
        તોડો;
    મૂળભૂત:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

#ifdef UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#endif

#ifdef UCPTRIE_DEBUG
    umutablecptrie_setName(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_કિંમત_Width)કિંમત_Bits, pErrorCode));
#endif
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->data);
    uprv_free(newTrie);
    trie->newTrie=nullptr;
}

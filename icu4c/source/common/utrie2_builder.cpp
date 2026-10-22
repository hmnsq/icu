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

#अन्तर्भाव "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/umutablecptrie.h"
#अन्तर्भाव "ucptrie_impl.h"
#endif
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "utrie2.h"
#अन्तर्भाव "utrie2_impl.h"

#अन्तर्भाव "utrie.h"  // for utrie2_fromUTrie()

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

गणना {
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

स्थिर int32_t
allocIndex2Block(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_मानम्, uint32_t error_मानम्, UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *data;
    int32_t i, j;

    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(परिमाण(UNewTrie2));
    data=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_DATA_LENGTH*4);
    सचे(trie==nullptr || newTrie==nullptr || data==nullptr) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(data);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }

    uprv_memset(trie, 0, परिमाण(UTrie2));
    trie->initial_मानम्=initial_मानम्;
    trie->error_मानम्=error_मानम्;
    trie->उच्चः_Start=0x110000;
    trie->newTrie=newTrie;
#ifdef UTRIE2_DEBUG
    trie->name="open";
#endif

    newTrie->data=data;
#ifdef UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_मानम्, error_मानम्, pErrorCode);
#endif
    newTrie->dataCapacity=UNEWTRIE2_INITIAL_DATA_LENGTH;
    newTrie->initial_मानम्=initial_मानम्;
    newTrie->error_मानम्=error_मानम्;
    newTrie->उच्चः_Start=0x110000;
    newTrie->firstFreeBlock=0;  /* no free block in the list */
    newTrie->isCompacted=मिच्छा;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    अत्थाय(i=0; i<0x80; ++i) {
        newTrie->data[i]=initial_मानम्;
    }
    अत्थाय(; i<0xc0; ++i) {
        newTrie->data[i]=error_मानम्;
    }
    अत्थाय(i=UNEWTRIE2_DATA_NULL_OFFSET; i<UNEWTRIE2_DATA_START_OFFSET; ++i) {
        newTrie->data[i]=initial_मानम्;
    }
    newTrie->dataNullOffset=UNEWTRIE2_DATA_NULL_OFFSET;
    newTrie->dataLength=UNEWTRIE2_DATA_START_OFFSET;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    अत्थाय(i=0, j=0; j<0x80; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    अत्थाय(; j<0xc0; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
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
    अत्थाय(; j<UNEWTRIE2_DATA_START_OFFSET; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    अत्थाय(i=0x80>>UTRIE2_SHIFT_2; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        newTrie->index2[i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    अत्थाय(i=0; i<UNEWTRIE2_INDEX_GAP_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_GAP_OFFSET+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    अत्थाय(i=0; i<UTRIE2_INDEX_2_BLOCK_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_2_NULL_OFFSET+i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }
    newTrie->index2NullOffset=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    newTrie->index2Length=UNEWTRIE2_INDEX_2_START_OFFSET;

    /* set the index-1 indexes for the linear index-2 block */
    अत्थाय(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        ++i, j+=UTRIE2_INDEX_2_BLOCK_LENGTH
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    अत्थाय(; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        newTrie->index1[i]=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    अत्थाय(i=0x80; i<0x800; i+=UTRIE2_DATA_BLOCK_LENGTH) {
        utrie2_set32(trie, i, initial_मानम्, pErrorCode);
    }

    पटिनिवत्त trie;
}

स्थिर UNewTrie2 *
cloneBuilder(ध्रुव UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(परिमाण(UNewTrie2)));
    सचे(trie==nullptr) {
        पटिनिवत्त nullptr;
    }

    trie->data = static_cast<uint32_t*>(uprv_malloc(other->dataCapacity * 4));
    सचे(trie->data==nullptr) {
        uprv_free(trie);
        पटिनिवत्त nullptr;
    }
#ifdef UCPTRIE_DEBUG
    सचे(other->t3==nullptr) {
        trie->t3=nullptr;
    } अञ्ञथा {
        UErrorCode errorCode=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &errorCode);
    }
#endif
    trie->dataCapacity=other->dataCapacity;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, परिमाण(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2Length*4);
    trie->index2NullOffset=other->index2NullOffset;
    trie->index2Length=other->index2Length;

    uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
    trie->dataNullOffset=other->dataNullOffset;
    trie->dataLength=other->dataLength;

    /* reference counters */
    सचे(other->isCompacted) {
        trie->firstFreeBlock=0;
    } अञ्ञथा {
        uprv_memcpy(trie->map, other->map, ((size_t)other->dataLength>>UTRIE2_SHIFT_2)*4);
        trie->firstFreeBlock=other->firstFreeBlock;
    }

    trie->initial_मानम्=other->initial_मानम्;
    trie->error_मानम्=other->error_मानम्;
    trie->उच्चः_Start=other->उच्चः_Start;
    trie->isCompacted=other->isCompacted;

    पटिनिवत्त trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ध्रुव UTrie2 *other, UErrorCode *pErrorCode) {
    UTrie2 *trie;

    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    सचे(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    uprv_memcpy(trie, other, परिमाण(UTrie2));

    सचे(other->memory!=nullptr) {
        trie->memory=uprv_malloc(other->length);
        सचे(trie->memory!=nullptr) {
            trie->isMemoryOwned=सच्च;
            uprv_memcpy(trie->memory, other->memory, other->length);

            /* make the clone's pointers point to its own memory */
            trie->index=(uint16_t *)trie->memory+(other->index-(uint16_t *)other->memory);
            सचे(other->data16!=nullptr) {
                trie->data16=(uint16_t *)trie->memory+(other->data16-(uint16_t *)other->memory);
            }
            सचे(other->data32!=nullptr) {
                trie->data32=(uint32_t *)trie->memory+(other->data32-(uint32_t *)other->memory);
            }
        }
    } अञ्ञथा /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    सचे(trie->memory==nullptr && trie->newTrie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        uprv_free(trie);
        trie=nullptr;
    }
    पटिनिवत्त trie;
}

प्रकारनिर्देश संरचना NewTrieAndStatus {
    UTrie2 *trie;
    UErrorCode errorCode;
    UBool exclusiveLimit;  /* rather than inclusive range end */
} NewTrieAndStatus;

स्थिर UBool U_CALLCONV
copyEnumRange(ध्रुव शून्य *context, UChar32 start, UChar32 end, uint32_t value) {
    NewTrieAndStatus *nt=(NewTrieAndStatus *)context;
    सचे(value!=nt->trie->initial_मानम्) {
        सचे(nt->exclusiveLimit) {
            --end;
        }
        सचे(start==end) {
            utrie2_set32(nt->trie, start, value, &nt->errorCode);
        } अञ्ञथा {
            utrie2_setRange32(nt->trie, start, end, value, सच्च, &nt->errorCode);
        }
        पटिनिवत्त U_SUCCESS(nt->errorCode);
    } अञ्ञथा {
        पटिनिवत्त सच्च;
    }
}

#ifdef UTRIE2_DEBUG
स्थिर long गणनम्_Initial(ध्रुव UTrie2 *trie) {
    uint32_t initial_मानम्=trie->initial_मानम्;
    int32_t length=trie->dataLength;
    long count=0;
    सचे(trie->data16!=nullptr) {
        अत्थाय(int32_t i=0; i<length; ++i) {
            सचे(trie->data16[i]==initial_मानम्) { ++count; }
        }
    } अञ्ञथा {
        अत्थाय(int32_t i=0; i<length; ++i) {
            सचे(trie->data32[i]==initial_मानम्) { ++count; }
        }
    }
    पटिनिवत्त count;
}

स्थिर शून्य
utrie_printLengths(ध्रुव UTrie *trie) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)परिमाण(UTrieHeader)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           indexLength, dataLength, totalLength);
}

स्थिर शून्य
utrie2_printLengths(ध्रुव UTrie2 *trie, ध्रुव वर्ण *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)परिमाण(UTrie2Header)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, गणनम्_Initial(trie), totalLength);
}
#endif

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ध्रुव UTrie2 *other, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    सचे(other->newTrie!=nullptr && !other->newTrie->isCompacted) {
        पटिनिवत्त utrie2_clone(other, pErrorCode);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_मानम्, other->error_मानम्, pErrorCode);
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    context.exclusiveLimit=मिच्छा;
    context.errorCode=*pErrorCode;
    utrie2_enum(other, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    अत्थाय(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        सचे(other->data32==nullptr) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } अञ्ञथा {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        सचे(value!=other->initial_मानम्) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    सचे(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    पटिनिवत्त context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ध्रुव UTrie *trie1, uint32_t error_मानम्, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे(trie1==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    context.trie=utrie2_open(trie1->initial_मानम्, error_मानम्, pErrorCode);
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }
    context.exclusiveLimit=सच्च;
    context.errorCode=*pErrorCode;
    utrie_enum(trie1, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    अत्थाय(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        सचे(trie1->data32==nullptr) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } अञ्ञथा {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        सचे(value!=trie1->initial_मानम्) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    सचे(U_SUCCESS(*pErrorCode)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=nullptr ? UTRIE2_32_मानम्_BITS : UTRIE2_16_मानम्_BITS,
                      pErrorCode);
    }
#ifdef UTRIE2_DEBUG
    सचे(U_SUCCESS(*pErrorCode)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#endif
    सचे(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    पटिनिवत्त context.trie;
}

स्थिर अन्तःस्थ UBool
isInNullBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, block;

    सचे(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } अञ्ञथा {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    पटिनिवत्त block == trie->dataNullOffset;
}

स्थिर int32_t
allocIndex2Block(UNewTrie2 *trie) {
    int32_t newBlock, new_शिखरम्;

    newBlock=trie->index2Length;
    new_शिखरम्=newBlock+UTRIE2_INDEX_2_BLOCK_LENGTH;
    सचे(new_शिखरम्>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        पटिनिवत्त -1;
    }
    trie->index2Length=new_शिखरम्;
    uprv_memcpy(trie->index2+newBlock, trie->index2+trie->index2NullOffset, UTRIE2_INDEX_2_BLOCK_LENGTH*4);
    पटिनिवत्त newBlock;
}

स्थिर int32_t
getIndex2Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    सचे(U_IS_LEAD(c) && forLSCP) {
        पटिनिवत्त UTRIE2_LSCP_INDEX_2_OFFSET;
    }

    i1=c>>UTRIE2_SHIFT_1;
    i2=trie->index1[i1];
    सचे(i2==trie->index2NullOffset) {
        i2=allocIndex2Block(trie);
        सचे(i2<0) {
            पटिनिवत्त -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    पटिनिवत्त i2;
}

स्थिर int32_t
allocDataBlock(UNewTrie2 *trie, int32_t copyBlock) {
    int32_t newBlock, new_शिखरम्;

    सचे(trie->firstFreeBlock!=0) {
        /* get the first free block */
        newBlock=trie->firstFreeBlock;
        trie->firstFreeBlock=-trie->map[newBlock>>UTRIE2_SHIFT_2];
    } अञ्ञथा {
        /* get a new block from the high end */
        newBlock=trie->dataLength;
        new_शिखरम्=newBlock+UTRIE2_DATA_BLOCK_LENGTH;
        सचे(new_शिखरम्>trie->dataCapacity) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *data;

            सचे(trie->dataCapacity<UNEWTRIE2_MEDIUM_DATA_LENGTH) {
                capacity=UNEWTRIE2_MEDIUM_DATA_LENGTH;
            } अञ्ञथा सचे(trie->dataCapacity<UNEWTRIE2_MAX_DATA_LENGTH) {
                capacity=UNEWTRIE2_MAX_DATA_LENGTH;
            } अञ्ञथा {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                पटिनिवत्त -1;
            }
            data = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            सचे(data==nullptr) {
                पटिनिवत्त -1;
            }
            uprv_memcpy(data, trie->data, (size_t)trie->dataLength*4);
            uprv_free(trie->data);
            trie->data=data;
            trie->dataCapacity=capacity;
        }
        trie->dataLength=new_शिखरम्;
    }
    uprv_memcpy(trie->data+newBlock, trie->data+copyBlock, UTRIE2_DATA_BLOCK_LENGTH*4);
    trie->map[newBlock>>UTRIE2_SHIFT_2]=0;
    पटिनिवत्त newBlock;
}

/* call when the block's reference counter reaches 0 */
स्थिर शून्य
releaseDataBlock(UNewTrie2 *trie, int32_t block) {
    /* put this block at the front of the free-block chain */
    trie->map[block>>UTRIE2_SHIFT_2]=-trie->firstFreeBlock;
    trie->firstFreeBlock=block;
}

स्थिर अन्तःस्थ UBool
isWritableBlock(UNewTrie2 *trie, int32_t block) {
    पटिनिवत्त block != trie->dataNullOffset && 1 == trie->map[block >> UTRIE2_SHIFT_2];
}

स्थिर अन्तःस्थ शून्य
setIndex2Entry(UNewTrie2 *trie, int32_t i2, int32_t block) {
    int32_t oldBlock;
    ++trie->map[block>>UTRIE2_SHIFT_2];  /* increment first, in case block==oldBlock! */
    oldBlock=trie->index2[i2];
    सचे(0 == --trie->map[oldBlock>>UTRIE2_SHIFT_2]) {
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
स्थिर int32_t
getDataBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, oldBlock, newBlock;

    i2=getIndex2Block(trie, c, forLSCP);
    सचे(i2<0) {
        पटिनिवत्त -1;  /* program error */
    }

    i2+=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
    oldBlock=trie->index2[i2];
    सचे(isWritableBlock(trie, oldBlock)) {
        पटिनिवत्त oldBlock;
    }

    /* allocate a new data block */
    newBlock=allocDataBlock(trie, oldBlock);
    सचे(newBlock<0) {
        /* out of memory in the data array */
        पटिनिवत्त -1;
    }
    setIndex2Entry(trie, i2, newBlock);
    पटिनिवत्त newBlock;
}

/**
 * @return true if the value was successfully set
 */
स्थिर शून्य
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UErrorCode *pErrorCode) {
    int32_t block;

    सचे(trie==nullptr || trie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        पटिनिवत्त;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_set(trie->t3, c, value, pErrorCode);
#endif

    block=getDataBlock(trie, c, forLSCP);
    सचे(block<0) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }

    trie->data[block+(c&UTRIE2_DATA_MASK)]=value;
}

U_CAPI शून्य U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }
    सचे((uint32_t)c>0x10ffff) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    set32(trie->newTrie, c, सच्च, value, pErrorCode);
}

U_CAPI शून्य U_EXPORT2
utrie2_set32ForLeadSurrogateCodeUnit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UErrorCode *pErrorCode) {
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }
    सचे(!U_IS_LEAD(c)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    set32(trie->newTrie, c, मिच्छा, value, pErrorCode);
}

स्थिर शून्य
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit=block+UTRIE2_DATA_BLOCK_LENGTH;
    याव(block<limit) {
        *block++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
स्थिर शून्य
fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
          uint32_t value, uint32_t initial_मानम्, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    सचे(overwrite) {
        याव(block<pLimit) {
            *block++=value;
        }
    } अञ्ञथा {
        याव(block<pLimit) {
            सचे(*block==initial_मानम्) {
                *block=value;
            }
            ++block;
        }
    }
}

U_CAPI शून्य U_EXPORT2
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

    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }
    सचे((uint32_t)start>0x10ffff || (uint32_t)end>0x10ffff || start>end) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    newTrie=trie->newTrie;
    सचे(newTrie==nullptr || newTrie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        पटिनिवत्त;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_setRange(newTrie->t3, start, end, value, pErrorCode);
#endif
    सचे(!overwrite && value==newTrie->initial_मानम्) {
        पटिनिवत्त; /* nothing to do */
    }

    limit=end+1;
    सचे(start&UTRIE2_DATA_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=getDataBlock(newTrie, start, सच्च);
        सचे(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }

        nextStart=(start+UTRIE2_DATA_MASK)&~UTRIE2_DATA_MASK;
        सचे(nextStart<=limit) {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, UTRIE2_DATA_BLOCK_LENGTH,
                      value, newTrie->initial_मानम्, overwrite);
            start=nextStart;
        } अञ्ञथा {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, limit&UTRIE2_DATA_MASK,
                      value, newTrie->initial_मानम्, overwrite);
            पटिनिवत्त;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE2_DATA_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE2_DATA_MASK;

    /* iterate over all-value blocks */
    सचे(value==newTrie->initial_मानम्) {
        repeatBlock=newTrie->dataNullOffset;
    } अञ्ञथा {
        repeatBlock=-1;
    }

    याव(start<limit) {
        int32_t i2;
        UBool setRepeatBlock=मिच्छा;

        सचे(value==newTrie->initial_मानम् && isInNullBlock(newTrie, start, सच्च)) {
            start+=UTRIE2_DATA_BLOCK_LENGTH; /* nothing to do */
            अनुवर्त;
        }

        /* get index value */
        i2=getIndex2Block(newTrie, start, सच्च);
        सचे(i2<0) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            पटिनिवत्त;
        }
        i2+=(start>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
        block=newTrie->index2[i2];
        सचे(isWritableBlock(newTrie, block)) {
            /* already allocated */
            सचे(overwrite && block>=UNEWTRIE2_DATA_0800_OFFSET) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                setRepeatBlock=सच्च;
            } अञ्ञथा {
                /* !overwrite, or protected block: just write the values into this block */
                fillBlock(newTrie->data+block,
                          0, UTRIE2_DATA_BLOCK_LENGTH,
                          value, newTrie->initial_मानम्, overwrite);
            }
        } अञ्ञथा सचे(newTrie->data[block]!=value && (overwrite || block==newTrie->dataNullOffset)) {
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
            setRepeatBlock=सच्च;
        }
        सचे(setRepeatBlock) {
            सचे(repeatBlock>=0) {
                setIndex2Entry(newTrie, i2, repeatBlock);
            } अञ्ञथा {
                /* create and set and fill the repeatBlock */
                repeatBlock=getDataBlock(newTrie, start, सच्च);
                सचे(repeatBlock<0) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    पटिनिवत्त;
                }
                writeBlock(newTrie->data+repeatBlock, value);
            }
        }

        start+=UTRIE2_DATA_BLOCK_LENGTH;
    }

    सचे(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=getDataBlock(newTrie, start, सच्च);
        सचे(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }

        fillBlock(newTrie->data+block, 0, rest, value, newTrie->initial_मानम्, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

स्थिर अन्तःस्थ UBool
equal_int32(ध्रुव int32_t *s, ध्रुव int32_t *t, int32_t length) {
    याव(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    पटिनिवत्त length == 0;
}

स्थिर अन्तःस्थ UBool
equal_uint32(ध्रुव uint32_t *s, ध्रुव uint32_t *t, int32_t length) {
    याव(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    पटिनिवत्त length == 0;
}

स्थिर int32_t
findSameIndex2Block(ध्रुव int32_t *idx, int32_t index2Length, int32_t otherBlock) {
    int32_t block;

    /* ensure that we do not even partially get past index2Length */
    index2Length-=UTRIE2_INDEX_2_BLOCK_LENGTH;

    अत्थाय(block=0; block<=index2Length; ++block) {
        सचे(equal_int32(idx+block, idx+otherBlock, UTRIE2_INDEX_2_BLOCK_LENGTH)) {
            पटिनिवत्त block;
        }
    }
    पटिनिवत्त -1;
}

स्थिर int32_t
findSameDataBlock(ध्रुव uint32_t *data, int32_t dataLength, int32_t otherBlock, int32_t blockLength) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=blockLength;

    अत्थाय(block=0; block<=dataLength; block+=UTRIE2_DATA_GRANULARITY) {
        सचे(equal_uint32(data+block, data+otherBlock, blockLength)) {
            पटिनिवत्त block;
        }
    }
    पटिनिवत्त -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
स्थिर UChar32
find_उच्चः_Start(UNewTrie2 *trie, uint32_t उच्चः_मानम्) {
    ध्रुव uint32_t *data32;

    uint32_t value, initial_मानम्;
    UChar32 c, prev;
    int32_t i1, i2, j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    data32=trie->data;
    initial_मानम्=trie->initial_मानम्;

    index2NullOffset=trie->index2NullOffset;
    nullBlock=trie->dataNullOffset;

    /* set variables for previous range */
    सचे(उच्चः_मानम्==initial_मानम्) {
        prevI2Block=index2NullOffset;
        prevBlock=nullBlock;
    } अञ्ञथा {
        prevI2Block=-1;
        prevBlock=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_INDEX_1_LENGTH;
    c=prev;
    याव(c>0) {
        i2Block=trie->index1[--i1];
        सचे(i2Block==prevI2Block) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
            अनुवर्त;
        }
        prevI2Block=i2Block;
        सचे(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            सचे(उच्चः_मानम्!=initial_मानम्) {
                पटिनिवत्त c;
            }
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } अञ्ञथा {
            /* enumerate data blocks for one index-2 block */
            अत्थाय(i2=UTRIE2_INDEX_2_BLOCK_LENGTH; i2>0;) {
                block=trie->index2[i2Block+ --i2];
                सचे(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                    अनुवर्त;
                }
                prevBlock=block;
                सचे(block==nullBlock) {
                    /* this is the null data block */
                    सचे(उच्चः_मानम्!=initial_मानम्) {
                        पटिनिवत्त c;
                    }
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                } अञ्ञथा {
                    अत्थाय(j=UTRIE2_DATA_BLOCK_LENGTH; j>0;) {
                        value=data32[block+ --j];
                        सचे(value!=उच्चः_मानम्) {
                            पटिनिवत्त c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    पटिनिवत्त 0;
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
स्थिर शून्य
compactData(UNewTrie2 *trie) {
#ifdef UTRIE2_DEBUG
    int32_t गणनम्_Same=0, sumOverlaps=0;
#endif

    int32_t start, newStart, movedStart;
    int32_t blockLength, overlap;
    int32_t i, mapIndex, block_गणनम्;

    /* do not compact linear-ASCII data */
    newStart=UTRIE2_DATA_START_OFFSET;
    अत्थाय(start=0, i=0; start<newStart; start+=UTRIE2_DATA_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    blockLength=64;
    block_गणनम्=blockLength>>UTRIE2_SHIFT_2;
    अत्थाय(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        सचे(start==UNEWTRIE2_DATA_0800_OFFSET) {
            blockLength=UTRIE2_DATA_BLOCK_LENGTH;
            block_गणनम्=1;
        }

        /* skip blocks that are not used */
        सचे(trie->map[start>>UTRIE2_SHIFT_2]<=0) {
            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* search for an identical block */
        सचे( (movedStart=findSameDataBlock(trie->data, newStart, start, blockLength))
             >=0
        ) {
#ifdef UTRIE2_DEBUG
            ++गणनम्_Same;
#endif
            /* found an identical block, set the other block's index value for the current block */
            अत्थाय(i=block_गणनम्, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        अत्थाय(overlap=blockLength-UTRIE2_DATA_GRANULARITY;
            overlap>0 && !equal_uint32(trie->data+(newStart-overlap), trie->data+start, overlap);
            overlap-=UTRIE2_DATA_GRANULARITY) {}

#ifdef UTRIE2_DEBUG
            sumOverlaps+=overlap;
#endif
        सचे(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            movedStart=newStart-overlap;
            अत्थाय(i=block_गणनम्, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            अत्थाय(i=blockLength-overlap; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } अञ्ञथा /* no overlap && newStart==start */ {
            अत्थाय(i=block_गणनम्, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=start;
                start+=UTRIE2_DATA_BLOCK_LENGTH;
            }
            newStart=start;
        }
    }

    /* now adjust the index-2 table */
    अत्थाय(i=0; i<trie->index2Length; ++i) {
        सचे(i==UNEWTRIE2_INDEX_GAP_OFFSET) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_INDEX_GAP_LENGTH;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_SHIFT_2];
    }
    trie->dataNullOffset=trie->map[trie->dataNullOffset>>UTRIE2_SHIFT_2];

    /* ensure dataLength alignment */
    याव((newStart&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        trie->data[newStart++]=trie->initial_मानम्;
    }

#ifdef UTRIE2_DEBUG
    /* we saved some space */
    printf("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)trie->dataLength, (long)newStart, (long)गणनम्_Same, (long)sumOverlaps);
#endif

    trie->dataLength=newStart;
}

स्थिर शून्य
compactIndex2(UNewTrie2 *trie) {
    int32_t i, start, newStart, movedStart, overlap;

    /* do not compact linear-BMP index-2 blocks */
    newStart=UTRIE2_INDEX_2_BMP_LENGTH;
    अत्थाय(start=0, i=0; start<newStart; start+=UTRIE2_INDEX_2_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    newStart+=UTRIE2_UTF8_2B_INDEX_2_LENGTH+((trie->उच्चः_Start-0x10000)>>UTRIE2_SHIFT_1);

    अत्थाय(start=UNEWTRIE2_INDEX_2_NULL_OFFSET; start<trie->index2Length;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        सचे( (movedStart=findSameIndex2Block(trie->index2, newStart, start))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=movedStart;

            /* advance start to the next block */
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        अत्थाय(overlap=UTRIE2_INDEX_2_BLOCK_LENGTH-1;
            overlap>0 && !equal_int32(trie->index2+(newStart-overlap), trie->index2+start, overlap);
            --overlap) {}

        सचे(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=newStart-overlap;

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            अत्थाय(i=UTRIE2_INDEX_2_BLOCK_LENGTH-overlap; i>0; --i) {
                trie->index2[newStart++]=trie->index2[start++];
            }
        } अञ्ञथा /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE2_SHIFT_1_2]=start;
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;
            newStart=start;
        }
    }

    /* now adjust the index-1 table */
    अत्थाय(i=0; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_SHIFT_1_2];
    }
    trie->index2NullOffset=trie->map[trie->index2NullOffset>>UTRIE2_SHIFT_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    याव((newStart&((UTRIE2_DATA_GRANULARITY-1)|1))!=0) {
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

स्थिर शून्य
compactTrie(UTrie2 *trie, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UChar32 उच्चः_Start, supp_उच्चः_Start;
    uint32_t उच्चः_मानम्;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    उच्चः_मानम्=utrie2_get32(trie, 0x10ffff);
    उच्चः_Start=find_उच्चः_Start(newTrie, उच्चः_मानम्);
    उच्चः_Start=(उच्चः_Start+(UTRIE2_CP_PER_INDEX_1_ENTRY-1))&~(UTRIE2_CP_PER_INDEX_1_ENTRY-1);
    सचे(उच्चः_Start==0x110000) {
        उच्चः_मानम्=trie->error_मानम्;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->उच्चः_Start=newTrie->उच्चः_Start=उच्चः_Start;

#ifdef UTRIE2_DEBUG
    printf("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)उच्चः_Start, (long)उच्चः_मानम्, (long)trie->initial_मानम्);
#endif

    सचे(उच्चः_Start<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_उच्चः_Start= उच्चः_Start<=0x10000 ? 0x10000 : उच्चः_Start;
        utrie2_setRange32(trie, supp_उच्चः_Start, 0x10ffff, trie->initial_मानम्, सच्च, pErrorCode);
        सचे(U_FAILURE(*pErrorCode)) {
            पटिनिवत्त;
        }
    }

    compactData(newTrie);
    सचे(उच्चः_Start>0x10000) {
        compactIndex2(newTrie);
#ifdef UTRIE2_DEBUG
    } अञ्ञथा {
        printf("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (long)उच्चः_Start, (long)trie->newTrie->index2Length, (long)UTRIE2_INDEX_1_OFFSET);
#endif
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->data[newTrie->dataLength++]=उच्चः_मानम्;
    याव((newTrie->dataLength&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        newTrie->data[newTrie->dataLength++]=trie->initial_मानम्;
    }

    newTrie->isCompacted=सच्च;
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
U_CAPI शून्य U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_मानम्_Bits मानम्_Bits, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t allIndexesLength;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */
    UChar32 उच्चः_Start;

    /* argument check */
    सचे(U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }
    सचे( trie==nullptr ||
        मानम्_Bits<0 || UTRIE2_गणनम्_मानम्_BITS<=मानम्_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }
    newTrie=trie->newTrie;
    सचे(newTrie==nullptr) {
        /* already frozen */
        UTrie2_मानम्_Bits frozen_मानम्_Bits=
            trie->data16!=nullptr ? UTRIE2_16_मानम्_BITS : UTRIE2_32_मानम्_BITS;
        सचे(मानम्_Bits!=frozen_मानम्_Bits) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
        पटिनिवत्त;
    }

    /* compact if necessary */
    सचे(!newTrie->isCompacted) {
        compactTrie(trie, pErrorCode);
        सचे(U_FAILURE(*pErrorCode)) {
            पटिनिवत्त;
        }
    }
    उच्चः_Start=trie->उच्चः_Start;

    सचे(उच्चः_Start<=0x10000) {
        allIndexesLength=UTRIE2_INDEX_1_OFFSET;
    } अञ्ञथा {
        allIndexesLength=newTrie->index2Length;
    }
    सचे(मानम्_Bits==UTRIE2_16_मानम्_BITS) {
        dataMove=allIndexesLength;
    } अञ्ञथा {
        dataMove=0;
    }

    /* are indexLength and dataLength within limits? */
    सचे( /* for unshifted indexLength */
        allIndexesLength>UTRIE2_MAX_INDEX_LENGTH ||
        /* for unshifted dataNullOffset */
        (dataMove+newTrie->dataNullOffset)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (dataMove+UNEWTRIE2_DATA_0800_OFFSET)>0xffff ||
        /* for shiftedDataLength */
        (dataMove+newTrie->dataLength)>UTRIE2_MAX_DATA_LENGTH
    ) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त;
    }

    /* calculate the total serialized length */
    length=परिमाण(UTrie2Header)+allIndexesLength*2;
    सचे(मानम्_Bits==UTRIE2_16_मानम्_BITS) {
        length+=newTrie->dataLength*2;
    } अञ्ञथा {
        length+=newTrie->dataLength*4;
    }

    trie->memory=uprv_malloc(length);
    सचे(trie->memory==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    trie->length=length;
    trie->isMemoryOwned=सच्च;

    trie->indexLength=allIndexesLength;
    trie->dataLength=newTrie->dataLength;
    सचे(उच्चः_Start<=0x10000) {
        trie->index2NullOffset=0xffff;
    } अञ्ञथा {
        trie->index2NullOffset=static_cast<uint16_t>(UTRIE2_INDEX_2_OFFSET+newTrie->index2NullOffset);
    }
    trie->dataNullOffset=(uint16_t)(dataMove+newTrie->dataNullOffset);
    trie->उच्चः_मानम्_Index=dataMove+trie->dataLength-UTRIE2_DATA_GRANULARITY;

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)मानम्_Bits;

    header->indexLength=(uint16_t)trie->indexLength;
    header->shiftedDataLength=(uint16_t)(trie->dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=trie->index2NullOffset;
    header->dataNullOffset=trie->dataNullOffset;
    header->shifted_उच्चः_Start=(uint16_t)(उच्चः_Start>>UTRIE2_SHIFT_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    अत्थाय(i=UTRIE2_INDEX_2_BMP_LENGTH; i>0; --i) {
        *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    अत्थाय(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    अत्थाय(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(dataMove+newTrie->index2[i<<(6-UTRIE2_SHIFT_2)]);
    }

    सचे(उच्चः_Start>0x10000) {
        int32_t index1Length=(उच्चः_Start-0x10000)>>UTRIE2_SHIFT_1;
        int32_t index2Offset=UTRIE2_INDEX_2_BMP_LENGTH+UTRIE2_UTF8_2B_INDEX_2_LENGTH+index1Length;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        अत्थाय(i=index1Length; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_INDEX_2_OFFSET + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2Offset;
        अत्थाय(i=newTrie->index2Length-index2Offset; i>0; --i) {
            *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
        }
    }

    /* write the 16/32-bit data array */
    विकल्प(मानम्_Bits) {
    पक्ष UTRIE2_16_मानम्_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        p=newTrie->data;
        अत्थाय(i=newTrie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        भङ्ग;
    पक्ष UTRIE2_32_मानम्_BITS:
        /* write 32-bit data values */
        trie->data16=nullptr;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->data, (size_t)newTrie->dataLength*4);
        भङ्ग;
    स्वभाव:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
    }

#ifdef UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#endif

#ifdef UCPTRIE_DEBUG
    umutablecptrie_setName(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_मानम्_Width)मानम्_Bits, pErrorCode));
#endif
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->data);
    uprv_free(newTrie);
    trie->newTrie=nullptr;
}

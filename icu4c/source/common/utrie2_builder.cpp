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

স্থির int32_t
allocIndex2Block(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_মান, uint32_t error_মান, UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *data;
    int32_t i, j;

    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(পরিমাণ(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(পরিমাণ(UNewTrie2));
    data=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_DATA_LENGTH*4);
    যদি(trie==nullptr || newTrie==nullptr || data==nullptr) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(data);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম nullptr;
    }

    uprv_memset(trie, 0, পরিমাণ(UTrie2));
    trie->initial_মান=initial_মান;
    trie->error_মান=error_মান;
    trie->উপরের_Start=0x110000;
    trie->newTrie=newTrie;
#ifdef UTRIE2_DEBUG
    trie->name="open";
#endif

    newTrie->data=data;
#ifdef UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_মান, error_মান, pErrorCode);
#endif
    newTrie->dataCapacity=UNEWTRIE2_INITIAL_DATA_LENGTH;
    newTrie->initial_মান=initial_মান;
    newTrie->error_মান=error_মান;
    newTrie->উপরের_Start=0x110000;
    newTrie->firstFreeBlock=0;  /* no free block in the list */
    newTrie->isCompacted=অসত্য;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    প্রতি(i=0; i<0x80; ++i) {
        newTrie->data[i]=initial_মান;
    }
    প্রতি(; i<0xc0; ++i) {
        newTrie->data[i]=error_মান;
    }
    প্রতি(i=UNEWTRIE2_DATA_NULL_OFFSET; i<UNEWTRIE2_DATA_START_OFFSET; ++i) {
        newTrie->data[i]=initial_মান;
    }
    newTrie->dataNullOffset=UNEWTRIE2_DATA_NULL_OFFSET;
    newTrie->dataLength=UNEWTRIE2_DATA_START_OFFSET;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    প্রতি(i=0, j=0; j<0x80; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    প্রতি(; j<0xc0; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
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
    প্রতি(; j<UNEWTRIE2_DATA_START_OFFSET; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    প্রতি(i=0x80>>UTRIE2_SHIFT_2; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        newTrie->index2[i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    প্রতি(i=0; i<UNEWTRIE2_INDEX_GAP_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_GAP_OFFSET+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    প্রতি(i=0; i<UTRIE2_INDEX_2_BLOCK_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_2_NULL_OFFSET+i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }
    newTrie->index2NullOffset=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    newTrie->index2Length=UNEWTRIE2_INDEX_2_START_OFFSET;

    /* set the index-1 indexes for the linear index-2 block */
    প্রতি(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        ++i, j+=UTRIE2_INDEX_2_BLOCK_LENGTH
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    প্রতি(; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        newTrie->index1[i]=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    প্রতি(i=0x80; i<0x800; i+=UTRIE2_DATA_BLOCK_LENGTH) {
        utrie2_set32(trie, i, initial_মান, pErrorCode);
    }

    প্রত্যাগম trie;
}

স্থির UNewTrie2 *
cloneBuilder(ধ্রুব UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(পরিমাণ(UNewTrie2)));
    যদি(trie==nullptr) {
        প্রত্যাগম nullptr;
    }

    trie->data = static_cast<uint32_t*>(uprv_malloc(other->dataCapacity * 4));
    যদি(trie->data==nullptr) {
        uprv_free(trie);
        প্রত্যাগম nullptr;
    }
#ifdef UCPTRIE_DEBUG
    যদি(other->t3==nullptr) {
        trie->t3=nullptr;
    } অন্যথা {
        UErrorCode errorCode=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &errorCode);
    }
#endif
    trie->dataCapacity=other->dataCapacity;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, পরিমাণ(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2Length*4);
    trie->index2NullOffset=other->index2NullOffset;
    trie->index2Length=other->index2Length;

    uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
    trie->dataNullOffset=other->dataNullOffset;
    trie->dataLength=other->dataLength;

    /* reference counters */
    যদি(other->isCompacted) {
        trie->firstFreeBlock=0;
    } অন্যথা {
        uprv_memcpy(trie->map, other->map, ((size_t)other->dataLength>>UTRIE2_SHIFT_2)*4);
        trie->firstFreeBlock=other->firstFreeBlock;
    }

    trie->initial_মান=other->initial_মান;
    trie->error_মান=other->error_মান;
    trie->উপরের_Start=other->উপরের_Start;
    trie->isCompacted=other->isCompacted;

    প্রত্যাগম trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ধ্রুব UTrie2 *other, UErrorCode *pErrorCode) {
    UTrie2 *trie;

    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম nullptr;
    }
    যদি(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(পরিমাণ(UTrie2));
    যদি(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম nullptr;
    }
    uprv_memcpy(trie, other, পরিমাণ(UTrie2));

    যদি(other->memory!=nullptr) {
        trie->memory=uprv_malloc(other->length);
        যদি(trie->memory!=nullptr) {
            trie->isMemoryOwned=সত্য;
            uprv_memcpy(trie->memory, other->memory, other->length);

            /* make the clone's pointers point to its own memory */
            trie->index=(uint16_t *)trie->memory+(other->index-(uint16_t *)other->memory);
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
    প্রত্যাগম trie;
}

প্রকারনির্দেশ সংরচনা NewTrieAndStatus {
    UTrie2 *trie;
    UErrorCode errorCode;
    UBool exclusiveLimit;  /* rather than inclusive range end */
} NewTrieAndStatus;

স্থির UBool U_CALLCONV
copyEnumRange(ধ্রুব শূন্য *context, UChar32 start, UChar32 end, uint32_t value) {
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
        প্রত্যাগম U_SUCCESS(nt->errorCode);
    } অন্যথা {
        প্রত্যাগম সত্য;
    }
}

#ifdef UTRIE2_DEBUG
স্থির long গুনতি_Initial(ধ্রুব UTrie2 *trie) {
    uint32_t initial_মান=trie->initial_মান;
    int32_t length=trie->dataLength;
    long count=0;
    যদি(trie->data16!=nullptr) {
        প্রতি(int32_t i=0; i<length; ++i) {
            যদি(trie->data16[i]==initial_মান) { ++count; }
        }
    } অন্যথা {
        প্রতি(int32_t i=0; i<length; ++i) {
            যদি(trie->data32[i]==initial_মান) { ++count; }
        }
    }
    প্রত্যাগম count;
}

স্থির শূন্য
utrie_printLengths(ধ্রুব UTrie *trie) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)পরিমাণ(UTrieHeader)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           indexLength, dataLength, totalLength);
}

স্থির শূন্য
utrie2_printLengths(ধ্রুব UTrie2 *trie, ধ্রুব বর্ণ *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)পরিমাণ(UTrie2Header)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, গুনতি_Initial(trie), totalLength);
}
#endif

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ধ্রুব UTrie2 *other, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম nullptr;
    }
    যদি(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    যদি(other->newTrie!=nullptr && !other->newTrie->isCompacted) {
        প্রত্যাগম utrie2_clone(other, pErrorCode);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_মান, other->error_মান, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম nullptr;
    }
    context.exclusiveLimit=অসত্য;
    context.errorCode=*pErrorCode;
    utrie2_enum(other, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    প্রতি(lead=0xd800; lead<0xdc00; ++lead) {
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
    প্রত্যাগম context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ধ্রুব UTrie *trie1, uint32_t error_মান, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম nullptr;
    }
    যদি(trie1==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    context.trie=utrie2_open(trie1->initial_মান, error_মান, pErrorCode);
    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম nullptr;
    }
    context.exclusiveLimit=সত্য;
    context.errorCode=*pErrorCode;
    utrie_enum(trie1, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    প্রতি(lead=0xd800; lead<0xdc00; ++lead) {
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
    প্রত্যাগম context.trie;
}

স্থির অন্তঃস্থ UBool
isInNullBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, block;

    যদি(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } অন্যথা {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    প্রত্যাগম block == trie->dataNullOffset;
}

স্থির int32_t
allocIndex2Block(UNewTrie2 *trie) {
    int32_t newBlock, new_শীর্ষ;

    newBlock=trie->index2Length;
    new_শীর্ষ=newBlock+UTRIE2_INDEX_2_BLOCK_LENGTH;
    যদি(new_শীর্ষ>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        প্রত্যাগম -1;
    }
    trie->index2Length=new_শীর্ষ;
    uprv_memcpy(trie->index2+newBlock, trie->index2+trie->index2NullOffset, UTRIE2_INDEX_2_BLOCK_LENGTH*4);
    প্রত্যাগম newBlock;
}

স্থির int32_t
getIndex2Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    যদি(U_IS_LEAD(c) && forLSCP) {
        প্রত্যাগম UTRIE2_LSCP_INDEX_2_OFFSET;
    }

    i1=c>>UTRIE2_SHIFT_1;
    i2=trie->index1[i1];
    যদি(i2==trie->index2NullOffset) {
        i2=allocIndex2Block(trie);
        যদি(i2<0) {
            প্রত্যাগম -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    প্রত্যাগম i2;
}

স্থির int32_t
allocDataBlock(UNewTrie2 *trie, int32_t copyBlock) {
    int32_t newBlock, new_শীর্ষ;

    যদি(trie->firstFreeBlock!=0) {
        /* get the first free block */
        newBlock=trie->firstFreeBlock;
        trie->firstFreeBlock=-trie->map[newBlock>>UTRIE2_SHIFT_2];
    } অন্যথা {
        /* get a new block from the high end */
        newBlock=trie->dataLength;
        new_শীর্ষ=newBlock+UTRIE2_DATA_BLOCK_LENGTH;
        যদি(new_শীর্ষ>trie->dataCapacity) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *data;

            যদি(trie->dataCapacity<UNEWTRIE2_MEDIUM_DATA_LENGTH) {
                capacity=UNEWTRIE2_MEDIUM_DATA_LENGTH;
            } অন্যথা যদি(trie->dataCapacity<UNEWTRIE2_MAX_DATA_LENGTH) {
                capacity=UNEWTRIE2_MAX_DATA_LENGTH;
            } অন্যথা {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                প্রত্যাগম -1;
            }
            data = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            যদি(data==nullptr) {
                প্রত্যাগম -1;
            }
            uprv_memcpy(data, trie->data, (size_t)trie->dataLength*4);
            uprv_free(trie->data);
            trie->data=data;
            trie->dataCapacity=capacity;
        }
        trie->dataLength=new_শীর্ষ;
    }
    uprv_memcpy(trie->data+newBlock, trie->data+copyBlock, UTRIE2_DATA_BLOCK_LENGTH*4);
    trie->map[newBlock>>UTRIE2_SHIFT_2]=0;
    প্রত্যাগম newBlock;
}

/* call when the block's reference counter reaches 0 */
স্থির শূন্য
releaseDataBlock(UNewTrie2 *trie, int32_t block) {
    /* put this block at the front of the free-block chain */
    trie->map[block>>UTRIE2_SHIFT_2]=-trie->firstFreeBlock;
    trie->firstFreeBlock=block;
}

স্থির অন্তঃস্থ UBool
isWritableBlock(UNewTrie2 *trie, int32_t block) {
    প্রত্যাগম block != trie->dataNullOffset && 1 == trie->map[block >> UTRIE2_SHIFT_2];
}

স্থির অন্তঃস্থ শূন্য
setIndex2Entry(UNewTrie2 *trie, int32_t i2, int32_t block) {
    int32_t oldBlock;
    ++trie->map[block>>UTRIE2_SHIFT_2];  /* increment first, in case block==oldBlock! */
    oldBlock=trie->index2[i2];
    যদি(0 == --trie->map[oldBlock>>UTRIE2_SHIFT_2]) {
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
স্থির int32_t
getDataBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, oldBlock, newBlock;

    i2=getIndex2Block(trie, c, forLSCP);
    যদি(i2<0) {
        প্রত্যাগম -1;  /* program error */
    }

    i2+=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
    oldBlock=trie->index2[i2];
    যদি(isWritableBlock(trie, oldBlock)) {
        প্রত্যাগম oldBlock;
    }

    /* allocate a new data block */
    newBlock=allocDataBlock(trie, oldBlock);
    যদি(newBlock<0) {
        /* out of memory in the data array */
        প্রত্যাগম -1;
    }
    setIndex2Entry(trie, i2, newBlock);
    প্রত্যাগম newBlock;
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
        প্রত্যাগম;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_set(trie->t3, c, value, pErrorCode);
#endif

    block=getDataBlock(trie, c, forLSCP);
    যদি(block<0) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }

    trie->data[block+(c&UTRIE2_DATA_MASK)]=value;
}

U_CAPI শূন্য U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম;
    }
    যদি((uint32_t)c>0x10ffff) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম;
    }
    set32(trie->newTrie, c, সত্য, value, pErrorCode);
}

U_CAPI শূন্য U_EXPORT2
utrie2_set32ForLeadSurrogateCodeUnit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম;
    }
    যদি(!U_IS_LEAD(c)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম;
    }
    set32(trie->newTrie, c, অসত্য, value, pErrorCode);
}

স্থির শূন্য
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit=block+UTRIE2_DATA_BLOCK_LENGTH;
    যাবত্(block<limit) {
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
        যাবত্(block<pLimit) {
            *block++=value;
        }
    } অন্যথা {
        যাবত্(block<pLimit) {
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
        প্রত্যাগম;
    }
    যদি((uint32_t)start>0x10ffff || (uint32_t)end>0x10ffff || start>end) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম;
    }
    newTrie=trie->newTrie;
    যদি(newTrie==nullptr || newTrie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        প্রত্যাগম;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_setRange(newTrie->t3, start, end, value, pErrorCode);
#endif
    যদি(!overwrite && value==newTrie->initial_মান) {
        প্রত্যাগম; /* nothing to do */
    }

    limit=end+1;
    যদি(start&UTRIE2_DATA_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=getDataBlock(newTrie, start, সত্য);
        যদি(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }

        nextStart=(start+UTRIE2_DATA_MASK)&~UTRIE2_DATA_MASK;
        যদি(nextStart<=limit) {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, UTRIE2_DATA_BLOCK_LENGTH,
                      value, newTrie->initial_মান, overwrite);
            start=nextStart;
        } অন্যথা {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, limit&UTRIE2_DATA_MASK,
                      value, newTrie->initial_মান, overwrite);
            প্রত্যাগম;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE2_DATA_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE2_DATA_MASK;

    /* iterate over all-value blocks */
    যদি(value==newTrie->initial_মান) {
        repeatBlock=newTrie->dataNullOffset;
    } অন্যথা {
        repeatBlock=-1;
    }

    যাবত্(start<limit) {
        int32_t i2;
        UBool setRepeatBlock=অসত্য;

        যদি(value==newTrie->initial_মান && isInNullBlock(newTrie, start, সত্য)) {
            start+=UTRIE2_DATA_BLOCK_LENGTH; /* nothing to do */
            অনুবর্ত;
        }

        /* get index value */
        i2=getIndex2Block(newTrie, start, সত্য);
        যদি(i2<0) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            প্রত্যাগম;
        }
        i2+=(start>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
        block=newTrie->index2[i2];
        যদি(isWritableBlock(newTrie, block)) {
            /* already allocated */
            যদি(overwrite && block>=UNEWTRIE2_DATA_0800_OFFSET) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                setRepeatBlock=সত্য;
            } অন্যথা {
                /* !overwrite, or protected block: just write the values into this block */
                fillBlock(newTrie->data+block,
                          0, UTRIE2_DATA_BLOCK_LENGTH,
                          value, newTrie->initial_মান, overwrite);
            }
        } অন্যথা যদি(newTrie->data[block]!=value && (overwrite || block==newTrie->dataNullOffset)) {
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
                repeatBlock=getDataBlock(newTrie, start, সত্য);
                যদি(repeatBlock<0) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    প্রত্যাগম;
                }
                writeBlock(newTrie->data+repeatBlock, value);
            }
        }

        start+=UTRIE2_DATA_BLOCK_LENGTH;
    }

    যদি(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=getDataBlock(newTrie, start, সত্য);
        যদি(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }

        fillBlock(newTrie->data+block, 0, rest, value, newTrie->initial_মান, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

স্থির অন্তঃস্থ UBool
equal_int32(ধ্রুব int32_t *s, ধ্রুব int32_t *t, int32_t length) {
    যাবত্(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    প্রত্যাগম length == 0;
}

স্থির অন্তঃস্থ UBool
equal_uint32(ধ্রুব uint32_t *s, ধ্রুব uint32_t *t, int32_t length) {
    যাবত্(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    প্রত্যাগম length == 0;
}

স্থির int32_t
findSameIndex2Block(ধ্রুব int32_t *idx, int32_t index2Length, int32_t otherBlock) {
    int32_t block;

    /* ensure that we do not even partially get past index2Length */
    index2Length-=UTRIE2_INDEX_2_BLOCK_LENGTH;

    প্রতি(block=0; block<=index2Length; ++block) {
        যদি(equal_int32(idx+block, idx+otherBlock, UTRIE2_INDEX_2_BLOCK_LENGTH)) {
            প্রত্যাগম block;
        }
    }
    প্রত্যাগম -1;
}

স্থির int32_t
findSameDataBlock(ধ্রুব uint32_t *data, int32_t dataLength, int32_t otherBlock, int32_t blockLength) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=blockLength;

    প্রতি(block=0; block<=dataLength; block+=UTRIE2_DATA_GRANULARITY) {
        যদি(equal_uint32(data+block, data+otherBlock, blockLength)) {
            প্রত্যাগম block;
        }
    }
    প্রত্যাগম -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
স্থির UChar32
find_উপরের_Start(UNewTrie2 *trie, uint32_t উপরের_মান) {
    ধ্রুব uint32_t *data32;

    uint32_t value, initial_মান;
    UChar32 c, prev;
    int32_t i1, i2, j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    data32=trie->data;
    initial_মান=trie->initial_মান;

    index2NullOffset=trie->index2NullOffset;
    nullBlock=trie->dataNullOffset;

    /* set variables for previous range */
    যদি(উপরের_মান==initial_মান) {
        prevI2Block=index2NullOffset;
        prevBlock=nullBlock;
    } অন্যথা {
        prevI2Block=-1;
        prevBlock=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_INDEX_1_LENGTH;
    c=prev;
    যাবত্(c>0) {
        i2Block=trie->index1[--i1];
        যদি(i2Block==prevI2Block) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
            অনুবর্ত;
        }
        prevI2Block=i2Block;
        যদি(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            যদি(উপরের_মান!=initial_মান) {
                প্রত্যাগম c;
            }
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } অন্যথা {
            /* enumerate data blocks for one index-2 block */
            প্রতি(i2=UTRIE2_INDEX_2_BLOCK_LENGTH; i2>0;) {
                block=trie->index2[i2Block+ --i2];
                যদি(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                    অনুবর্ত;
                }
                prevBlock=block;
                যদি(block==nullBlock) {
                    /* this is the null data block */
                    যদি(উপরের_মান!=initial_মান) {
                        প্রত্যাগম c;
                    }
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                } অন্যথা {
                    প্রতি(j=UTRIE2_DATA_BLOCK_LENGTH; j>0;) {
                        value=data32[block+ --j];
                        যদি(value!=উপরের_মান) {
                            প্রত্যাগম c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    প্রত্যাগম 0;
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
compactData(UNewTrie2 *trie) {
#ifdef UTRIE2_DEBUG
    int32_t গুনতি_Same=0, sumOverlaps=0;
#endif

    int32_t start, newStart, movedStart;
    int32_t blockLength, overlap;
    int32_t i, mapIndex, block_গুনতি;

    /* do not compact linear-ASCII data */
    newStart=UTRIE2_DATA_START_OFFSET;
    প্রতি(start=0, i=0; start<newStart; start+=UTRIE2_DATA_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    blockLength=64;
    block_গুনতি=blockLength>>UTRIE2_SHIFT_2;
    প্রতি(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        যদি(start==UNEWTRIE2_DATA_0800_OFFSET) {
            blockLength=UTRIE2_DATA_BLOCK_LENGTH;
            block_গুনতি=1;
        }

        /* skip blocks that are not used */
        যদি(trie->map[start>>UTRIE2_SHIFT_2]<=0) {
            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            অনুবর্ত;
        }

        /* search for an identical block */
        যদি( (movedStart=findSameDataBlock(trie->data, newStart, start, blockLength))
             >=0
        ) {
#ifdef UTRIE2_DEBUG
            ++গুনতি_Same;
#endif
            /* found an identical block, set the other block's index value for the current block */
            প্রতি(i=block_গুনতি, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            অনুবর্ত;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        প্রতি(overlap=blockLength-UTRIE2_DATA_GRANULARITY;
            overlap>0 && !equal_uint32(trie->data+(newStart-overlap), trie->data+start, overlap);
            overlap-=UTRIE2_DATA_GRANULARITY) {}

#ifdef UTRIE2_DEBUG
            sumOverlaps+=overlap;
#endif
        যদি(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            movedStart=newStart-overlap;
            প্রতি(i=block_গুনতি, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            প্রতি(i=blockLength-overlap; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } অন্যথা /* no overlap && newStart==start */ {
            প্রতি(i=block_গুনতি, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=start;
                start+=UTRIE2_DATA_BLOCK_LENGTH;
            }
            newStart=start;
        }
    }

    /* now adjust the index-2 table */
    প্রতি(i=0; i<trie->index2Length; ++i) {
        যদি(i==UNEWTRIE2_INDEX_GAP_OFFSET) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_INDEX_GAP_LENGTH;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_SHIFT_2];
    }
    trie->dataNullOffset=trie->map[trie->dataNullOffset>>UTRIE2_SHIFT_2];

    /* ensure dataLength alignment */
    যাবত্((newStart&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        trie->data[newStart++]=trie->initial_মান;
    }

#ifdef UTRIE2_DEBUG
    /* we saved some space */
    printf("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)trie->dataLength, (long)newStart, (long)গুনতি_Same, (long)sumOverlaps);
#endif

    trie->dataLength=newStart;
}

স্থির শূন্য
compactIndex2(UNewTrie2 *trie) {
    int32_t i, start, newStart, movedStart, overlap;

    /* do not compact linear-BMP index-2 blocks */
    newStart=UTRIE2_INDEX_2_BMP_LENGTH;
    প্রতি(start=0, i=0; start<newStart; start+=UTRIE2_INDEX_2_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    newStart+=UTRIE2_UTF8_2B_INDEX_2_LENGTH+((trie->উপরের_Start-0x10000)>>UTRIE2_SHIFT_1);

    প্রতি(start=UNEWTRIE2_INDEX_2_NULL_OFFSET; start<trie->index2Length;) {
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
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            অনুবর্ত;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        প্রতি(overlap=UTRIE2_INDEX_2_BLOCK_LENGTH-1;
            overlap>0 && !equal_int32(trie->index2+(newStart-overlap), trie->index2+start, overlap);
            --overlap) {}

        যদি(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=newStart-overlap;

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            প্রতি(i=UTRIE2_INDEX_2_BLOCK_LENGTH-overlap; i>0; --i) {
                trie->index2[newStart++]=trie->index2[start++];
            }
        } অন্যথা /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE2_SHIFT_1_2]=start;
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;
            newStart=start;
        }
    }

    /* now adjust the index-1 table */
    প্রতি(i=0; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_SHIFT_1_2];
    }
    trie->index2NullOffset=trie->map[trie->index2NullOffset>>UTRIE2_SHIFT_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    যাবত্((newStart&((UTRIE2_DATA_GRANULARITY-1)|1))!=0) {
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

স্থির শূন্য
compactTrie(UTrie2 *trie, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UChar32 উপরের_Start, supp_উপরের_Start;
    uint32_t উপরের_মান;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    উপরের_মান=utrie2_get32(trie, 0x10ffff);
    উপরের_Start=find_উপরের_Start(newTrie, উপরের_মান);
    উপরের_Start=(উপরের_Start+(UTRIE2_CP_PER_INDEX_1_ENTRY-1))&~(UTRIE2_CP_PER_INDEX_1_ENTRY-1);
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
            প্রত্যাগম;
        }
    }

    compactData(newTrie);
    যদি(উপরের_Start>0x10000) {
        compactIndex2(newTrie);
#ifdef UTRIE2_DEBUG
    } অন্যথা {
        printf("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (long)উপরের_Start, (long)trie->newTrie->index2Length, (long)UTRIE2_INDEX_1_OFFSET);
#endif
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->data[newTrie->dataLength++]=উপরের_মান;
    যাবত্((newTrie->dataLength&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        newTrie->data[newTrie->dataLength++]=trie->initial_মান;
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
#define UTRIE2_MAX_INDEX_LENGTH 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#define UTRIE2_MAX_DATA_LENGTH (0xffff<<UTRIE2_INDEX_SHIFT)

/* Compact and internally serialize the trie. */
U_CAPI শূন্য U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_মান_Bits মান_Bits, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t allIndexesLength;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */
    UChar32 উপরের_Start;

    /* argument check */
    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম;
    }
    যদি( trie==nullptr ||
        মান_Bits<0 || UTRIE2_গুনতি_মান_BITS<=মান_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম;
    }
    newTrie=trie->newTrie;
    যদি(newTrie==nullptr) {
        /* already frozen */
        UTrie2_মান_Bits frozen_মান_Bits=
            trie->data16!=nullptr ? UTRIE2_16_মান_BITS : UTRIE2_32_মান_BITS;
        যদি(মান_Bits!=frozen_মান_Bits) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
        প্রত্যাগম;
    }

    /* compact if necessary */
    যদি(!newTrie->isCompacted) {
        compactTrie(trie, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            প্রত্যাগম;
        }
    }
    উপরের_Start=trie->উপরের_Start;

    যদি(উপরের_Start<=0x10000) {
        allIndexesLength=UTRIE2_INDEX_1_OFFSET;
    } অন্যথা {
        allIndexesLength=newTrie->index2Length;
    }
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        dataMove=allIndexesLength;
    } অন্যথা {
        dataMove=0;
    }

    /* are indexLength and dataLength within limits? */
    যদি( /* for unshifted indexLength */
        allIndexesLength>UTRIE2_MAX_INDEX_LENGTH ||
        /* for unshifted dataNullOffset */
        (dataMove+newTrie->dataNullOffset)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (dataMove+UNEWTRIE2_DATA_0800_OFFSET)>0xffff ||
        /* for shiftedDataLength */
        (dataMove+newTrie->dataLength)>UTRIE2_MAX_DATA_LENGTH
    ) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম;
    }

    /* calculate the total serialized length */
    length=পরিমাণ(UTrie2Header)+allIndexesLength*2;
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        length+=newTrie->dataLength*2;
    } অন্যথা {
        length+=newTrie->dataLength*4;
    }

    trie->memory=uprv_malloc(length);
    যদি(trie->memory==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    trie->length=length;
    trie->isMemoryOwned=সত্য;

    trie->indexLength=allIndexesLength;
    trie->dataLength=newTrie->dataLength;
    যদি(উপরের_Start<=0x10000) {
        trie->index2NullOffset=0xffff;
    } অন্যথা {
        trie->index2NullOffset=static_cast<uint16_t>(UTRIE2_INDEX_2_OFFSET+newTrie->index2NullOffset);
    }
    trie->dataNullOffset=(uint16_t)(dataMove+newTrie->dataNullOffset);
    trie->উপরের_মান_Index=dataMove+trie->dataLength-UTRIE2_DATA_GRANULARITY;

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)মান_Bits;

    header->indexLength=(uint16_t)trie->indexLength;
    header->shiftedDataLength=(uint16_t)(trie->dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=trie->index2NullOffset;
    header->dataNullOffset=trie->dataNullOffset;
    header->shifted_উপরের_Start=(uint16_t)(উপরের_Start>>UTRIE2_SHIFT_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    প্রতি(i=UTRIE2_INDEX_2_BMP_LENGTH; i>0; --i) {
        *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    প্রতি(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    প্রতি(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(dataMove+newTrie->index2[i<<(6-UTRIE2_SHIFT_2)]);
    }

    যদি(উপরের_Start>0x10000) {
        int32_t index1Length=(উপরের_Start-0x10000)>>UTRIE2_SHIFT_1;
        int32_t index2Offset=UTRIE2_INDEX_2_BMP_LENGTH+UTRIE2_UTF8_2B_INDEX_2_LENGTH+index1Length;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        প্রতি(i=index1Length; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_INDEX_2_OFFSET + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2Offset;
        প্রতি(i=newTrie->index2Length-index2Offset; i>0; --i) {
            *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
        }
    }

    /* write the 16/32-bit data array */
    বিকল্প(মান_Bits) {
    পক্ষ UTRIE2_16_মান_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        p=newTrie->data;
        প্রতি(i=newTrie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        ভঙ্গ;
    পক্ষ UTRIE2_32_মান_BITS:
        /* write 32-bit data values */
        trie->data16=nullptr;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->data, (size_t)newTrie->dataLength*4);
        ভঙ্গ;
    স্বভাব:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম;
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
    uprv_free(newTrie->data);
    uprv_free(newTrie);
    trie->newTrie=nullptr;
}

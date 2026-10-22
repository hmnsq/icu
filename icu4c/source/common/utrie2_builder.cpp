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

#അന്തര്ഭാവ "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/umutablecptrie.h"
#അന്തര്ഭാവ "ucptrie_impl.h"
#endif
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "utrie2.h"
#അന്തര്ഭാവ "utrie2_impl.h"

#അന്തര്ഭാവ "utrie.h"  // for utrie2_fromUTrie()

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

എണ്ണൽ {
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

സ്ഥിതം int32_t
allocIndex2Block(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_മൂല്യം, uint32_t error_മൂല്യം, UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *data;
    int32_t i, j;

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(വലുപ്പം(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(വലുപ്പം(UNewTrie2));
    data=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_DATA_LENGTH*4);
    എങ്കിൽ(trie==nullptr || newTrie==nullptr || data==nullptr) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(data);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }

    uprv_memset(trie, 0, വലുപ്പം(UTrie2));
    trie->initial_മൂല്യം=initial_മൂല്യം;
    trie->error_മൂല്യം=error_മൂല്യം;
    trie->മുകളിലെ_Start=0x110000;
    trie->newTrie=newTrie;
#ifdef UTRIE2_DEBUG
    trie->name="open";
#endif

    newTrie->data=data;
#ifdef UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_മൂല്യം, error_മൂല്യം, pErrorCode);
#endif
    newTrie->dataCapacity=UNEWTRIE2_INITIAL_DATA_LENGTH;
    newTrie->initial_മൂല്യം=initial_മൂല്യം;
    newTrie->error_മൂല്യം=error_മൂല്യം;
    newTrie->മുകളിലെ_Start=0x110000;
    newTrie->firstFreeBlock=0;  /* no free block in the list */
    newTrie->isCompacted=അസത്യം;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    വേണ്ടി(i=0; i<0x80; ++i) {
        newTrie->data[i]=initial_മൂല്യം;
    }
    വേണ്ടി(; i<0xc0; ++i) {
        newTrie->data[i]=error_മൂല്യം;
    }
    വേണ്ടി(i=UNEWTRIE2_DATA_NULL_OFFSET; i<UNEWTRIE2_DATA_START_OFFSET; ++i) {
        newTrie->data[i]=initial_മൂല്യം;
    }
    newTrie->dataNullOffset=UNEWTRIE2_DATA_NULL_OFFSET;
    newTrie->dataLength=UNEWTRIE2_DATA_START_OFFSET;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    വേണ്ടി(i=0, j=0; j<0x80; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    വേണ്ടി(; j<0xc0; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
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
    വേണ്ടി(; j<UNEWTRIE2_DATA_START_OFFSET; ++i, j+=UTRIE2_DATA_BLOCK_LENGTH) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    വേണ്ടി(i=0x80>>UTRIE2_SHIFT_2; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        newTrie->index2[i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    വേണ്ടി(i=0; i<UNEWTRIE2_INDEX_GAP_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_GAP_OFFSET+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    വേണ്ടി(i=0; i<UTRIE2_INDEX_2_BLOCK_LENGTH; ++i) {
        newTrie->index2[UNEWTRIE2_INDEX_2_NULL_OFFSET+i]=UNEWTRIE2_DATA_NULL_OFFSET;
    }
    newTrie->index2NullOffset=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    newTrie->index2Length=UNEWTRIE2_INDEX_2_START_OFFSET;

    /* set the index-1 indexes for the linear index-2 block */
    വേണ്ടി(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        ++i, j+=UTRIE2_INDEX_2_BLOCK_LENGTH
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    വേണ്ടി(; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        newTrie->index1[i]=UNEWTRIE2_INDEX_2_NULL_OFFSET;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    വേണ്ടി(i=0x80; i<0x800; i+=UTRIE2_DATA_BLOCK_LENGTH) {
        utrie2_set32(trie, i, initial_മൂല്യം, pErrorCode);
    }

    തിരികെ trie;
}

സ്ഥിതം UNewTrie2 *
cloneBuilder(സ്ഥിരം UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(വലുപ്പം(UNewTrie2)));
    എങ്കിൽ(trie==nullptr) {
        തിരികെ nullptr;
    }

    trie->data = static_cast<uint32_t*>(uprv_malloc(other->dataCapacity * 4));
    എങ്കിൽ(trie->data==nullptr) {
        uprv_free(trie);
        തിരികെ nullptr;
    }
#ifdef UCPTRIE_DEBUG
    എങ്കിൽ(other->t3==nullptr) {
        trie->t3=nullptr;
    } അല്ലെങ്കിൽ {
        UErrorCode errorCode=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &errorCode);
    }
#endif
    trie->dataCapacity=other->dataCapacity;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, വലുപ്പം(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2Length*4);
    trie->index2NullOffset=other->index2NullOffset;
    trie->index2Length=other->index2Length;

    uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
    trie->dataNullOffset=other->dataNullOffset;
    trie->dataLength=other->dataLength;

    /* reference counters */
    എങ്കിൽ(other->isCompacted) {
        trie->firstFreeBlock=0;
    } അല്ലെങ്കിൽ {
        uprv_memcpy(trie->map, other->map, ((size_t)other->dataLength>>UTRIE2_SHIFT_2)*4);
        trie->firstFreeBlock=other->firstFreeBlock;
    }

    trie->initial_മൂല്യം=other->initial_മൂല്യം;
    trie->error_മൂല്യം=other->error_മൂല്യം;
    trie->മുകളിലെ_Start=other->മുകളിലെ_Start;
    trie->isCompacted=other->isCompacted;

    തിരികെ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(സ്ഥിരം UTrie2 *other, UErrorCode *pErrorCode) {
    UTrie2 *trie;

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(വലുപ്പം(UTrie2));
    എങ്കിൽ(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memcpy(trie, other, വലുപ്പം(UTrie2));

    എങ്കിൽ(other->memory!=nullptr) {
        trie->memory=uprv_malloc(other->length);
        എങ്കിൽ(trie->memory!=nullptr) {
            trie->isMemoryOwned=സത്യം;
            uprv_memcpy(trie->memory, other->memory, other->length);

            /* make the clone's pointers point to its own memory */
            trie->index=(uint16_t *)trie->memory+(other->index-(uint16_t *)other->memory);
            എങ്കിൽ(other->data16!=nullptr) {
                trie->data16=(uint16_t *)trie->memory+(other->data16-(uint16_t *)other->memory);
            }
            എങ്കിൽ(other->data32!=nullptr) {
                trie->data32=(uint32_t *)trie->memory+(other->data32-(uint32_t *)other->memory);
            }
        }
    } അല്ലെങ്കിൽ /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    എങ്കിൽ(trie->memory==nullptr && trie->newTrie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        uprv_free(trie);
        trie=nullptr;
    }
    തിരികെ trie;
}

തരനിർവചനം ഘടന NewTrieAndStatus {
    UTrie2 *trie;
    UErrorCode errorCode;
    UBool exclusiveLimit;  /* rather than inclusive range end */
} NewTrieAndStatus;

സ്ഥിതം UBool U_CALLCONV
copyEnumRange(സ്ഥിരം ശൂന്യം *context, UChar32 start, UChar32 end, uint32_t value) {
    NewTrieAndStatus *nt=(NewTrieAndStatus *)context;
    എങ്കിൽ(value!=nt->trie->initial_മൂല്യം) {
        എങ്കിൽ(nt->exclusiveLimit) {
            --end;
        }
        എങ്കിൽ(start==end) {
            utrie2_set32(nt->trie, start, value, &nt->errorCode);
        } അല്ലെങ്കിൽ {
            utrie2_setRange32(nt->trie, start, end, value, സത്യം, &nt->errorCode);
        }
        തിരികെ U_SUCCESS(nt->errorCode);
    } അല്ലെങ്കിൽ {
        തിരികെ സത്യം;
    }
}

#ifdef UTRIE2_DEBUG
സ്ഥിതം long എണ്ണം_Initial(സ്ഥിരം UTrie2 *trie) {
    uint32_t initial_മൂല്യം=trie->initial_മൂല്യം;
    int32_t length=trie->dataLength;
    long count=0;
    എങ്കിൽ(trie->data16!=nullptr) {
        വേണ്ടി(int32_t i=0; i<length; ++i) {
            എങ്കിൽ(trie->data16[i]==initial_മൂല്യം) { ++count; }
        }
    } അല്ലെങ്കിൽ {
        വേണ്ടി(int32_t i=0; i<length; ++i) {
            എങ്കിൽ(trie->data32[i]==initial_മൂല്യം) { ++count; }
        }
    }
    തിരികെ count;
}

സ്ഥിതം ശൂന്യം
utrie_printLengths(സ്ഥിരം UTrie *trie) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)വലുപ്പം(UTrieHeader)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           indexLength, dataLength, totalLength);
}

സ്ഥിതം ശൂന്യം
utrie2_printLengths(സ്ഥിരം UTrie2 *trie, സ്ഥിരം അക്ഷരം *which) {
    long indexLength=trie->indexLength;
    long dataLength=(long)trie->dataLength;
    long totalLength=(long)വലുപ്പം(UTrie2Header)+indexLength*2+dataLength*(trie->data32!=nullptr ? 4 : 2);
    printf("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, indexLength, dataLength, എണ്ണം_Initial(trie), totalLength);
}
#endif

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(സ്ഥിരം UTrie2 *other, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    എങ്കിൽ(other->newTrie!=nullptr && !other->newTrie->isCompacted) {
        തിരികെ utrie2_clone(other, pErrorCode);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_മൂല്യം, other->error_മൂല്യം, pErrorCode);
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    context.exclusiveLimit=അസത്യം;
    context.errorCode=*pErrorCode;
    utrie2_enum(other, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    വേണ്ടി(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        എങ്കിൽ(other->data32==nullptr) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } അല്ലെങ്കിൽ {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        എങ്കിൽ(value!=other->initial_മൂല്യം) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    തിരികെ context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(സ്ഥിരം UTrie *trie1, uint32_t error_മൂല്യം, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(trie1==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    context.trie=utrie2_open(trie1->initial_മൂല്യം, error_മൂല്യം, pErrorCode);
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }
    context.exclusiveLimit=സത്യം;
    context.errorCode=*pErrorCode;
    utrie_enum(trie1, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    വേണ്ടി(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        എങ്കിൽ(trie1->data32==nullptr) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } അല്ലെങ്കിൽ {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        എങ്കിൽ(value!=trie1->initial_മൂല്യം) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    എങ്കിൽ(U_SUCCESS(*pErrorCode)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=nullptr ? UTRIE2_32_മൂല്യം_BITS : UTRIE2_16_മൂല്യം_BITS,
                      pErrorCode);
    }
#ifdef UTRIE2_DEBUG
    എങ്കിൽ(U_SUCCESS(*pErrorCode)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#endif
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    തിരികെ context.trie;
}

സ്ഥിതം വരിയിൽ UBool
isInNullBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, block;

    എങ്കിൽ(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } അല്ലെങ്കിൽ {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    തിരികെ block == trie->dataNullOffset;
}

സ്ഥിതം int32_t
allocIndex2Block(UNewTrie2 *trie) {
    int32_t newBlock, new_മുകൾ;

    newBlock=trie->index2Length;
    new_മുകൾ=newBlock+UTRIE2_INDEX_2_BLOCK_LENGTH;
    എങ്കിൽ(new_മുകൾ>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        തിരികെ -1;
    }
    trie->index2Length=new_മുകൾ;
    uprv_memcpy(trie->index2+newBlock, trie->index2+trie->index2NullOffset, UTRIE2_INDEX_2_BLOCK_LENGTH*4);
    തിരികെ newBlock;
}

സ്ഥിതം int32_t
getIndex2Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    എങ്കിൽ(U_IS_LEAD(c) && forLSCP) {
        തിരികെ UTRIE2_LSCP_INDEX_2_OFFSET;
    }

    i1=c>>UTRIE2_SHIFT_1;
    i2=trie->index1[i1];
    എങ്കിൽ(i2==trie->index2NullOffset) {
        i2=allocIndex2Block(trie);
        എങ്കിൽ(i2<0) {
            തിരികെ -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    തിരികെ i2;
}

സ്ഥിതം int32_t
allocDataBlock(UNewTrie2 *trie, int32_t copyBlock) {
    int32_t newBlock, new_മുകൾ;

    എങ്കിൽ(trie->firstFreeBlock!=0) {
        /* get the first free block */
        newBlock=trie->firstFreeBlock;
        trie->firstFreeBlock=-trie->map[newBlock>>UTRIE2_SHIFT_2];
    } അല്ലെങ്കിൽ {
        /* get a new block from the high end */
        newBlock=trie->dataLength;
        new_മുകൾ=newBlock+UTRIE2_DATA_BLOCK_LENGTH;
        എങ്കിൽ(new_മുകൾ>trie->dataCapacity) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *data;

            എങ്കിൽ(trie->dataCapacity<UNEWTRIE2_MEDIUM_DATA_LENGTH) {
                capacity=UNEWTRIE2_MEDIUM_DATA_LENGTH;
            } അല്ലെങ്കിൽ എങ്കിൽ(trie->dataCapacity<UNEWTRIE2_MAX_DATA_LENGTH) {
                capacity=UNEWTRIE2_MAX_DATA_LENGTH;
            } അല്ലെങ്കിൽ {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                തിരികെ -1;
            }
            data = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            എങ്കിൽ(data==nullptr) {
                തിരികെ -1;
            }
            uprv_memcpy(data, trie->data, (size_t)trie->dataLength*4);
            uprv_free(trie->data);
            trie->data=data;
            trie->dataCapacity=capacity;
        }
        trie->dataLength=new_മുകൾ;
    }
    uprv_memcpy(trie->data+newBlock, trie->data+copyBlock, UTRIE2_DATA_BLOCK_LENGTH*4);
    trie->map[newBlock>>UTRIE2_SHIFT_2]=0;
    തിരികെ newBlock;
}

/* call when the block's reference counter reaches 0 */
സ്ഥിതം ശൂന്യം
releaseDataBlock(UNewTrie2 *trie, int32_t block) {
    /* put this block at the front of the free-block chain */
    trie->map[block>>UTRIE2_SHIFT_2]=-trie->firstFreeBlock;
    trie->firstFreeBlock=block;
}

സ്ഥിതം വരിയിൽ UBool
isWritableBlock(UNewTrie2 *trie, int32_t block) {
    തിരികെ block != trie->dataNullOffset && 1 == trie->map[block >> UTRIE2_SHIFT_2];
}

സ്ഥിതം വരിയിൽ ശൂന്യം
setIndex2Entry(UNewTrie2 *trie, int32_t i2, int32_t block) {
    int32_t oldBlock;
    ++trie->map[block>>UTRIE2_SHIFT_2];  /* increment first, in case block==oldBlock! */
    oldBlock=trie->index2[i2];
    എങ്കിൽ(0 == --trie->map[oldBlock>>UTRIE2_SHIFT_2]) {
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
സ്ഥിതം int32_t
getDataBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, oldBlock, newBlock;

    i2=getIndex2Block(trie, c, forLSCP);
    എങ്കിൽ(i2<0) {
        തിരികെ -1;  /* program error */
    }

    i2+=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
    oldBlock=trie->index2[i2];
    എങ്കിൽ(isWritableBlock(trie, oldBlock)) {
        തിരികെ oldBlock;
    }

    /* allocate a new data block */
    newBlock=allocDataBlock(trie, oldBlock);
    എങ്കിൽ(newBlock<0) {
        /* out of memory in the data array */
        തിരികെ -1;
    }
    setIndex2Entry(trie, i2, newBlock);
    തിരികെ newBlock;
}

/**
 * @return true if the value was successfully set
 */
സ്ഥിതം ശൂന്യം
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UErrorCode *pErrorCode) {
    int32_t block;

    എങ്കിൽ(trie==nullptr || trie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        തിരികെ;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_set(trie->t3, c, value, pErrorCode);
#endif

    block=getDataBlock(trie, c, forLSCP);
    എങ്കിൽ(block<0) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }

    trie->data[block+(c&UTRIE2_DATA_MASK)]=value;
}

U_CAPI ശൂന്യം U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    എങ്കിൽ((uint32_t)c>0x10ffff) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    set32(trie->newTrie, c, സത്യം, value, pErrorCode);
}

U_CAPI ശൂന്യം U_EXPORT2
utrie2_set32ForLeadSurrogateCodeUnit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UErrorCode *pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    എങ്കിൽ(!U_IS_LEAD(c)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    set32(trie->newTrie, c, അസത്യം, value, pErrorCode);
}

സ്ഥിതം ശൂന്യം
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit=block+UTRIE2_DATA_BLOCK_LENGTH;
    ആകുമ്പോൾ(block<limit) {
        *block++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
സ്ഥിതം ശൂന്യം
fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
          uint32_t value, uint32_t initial_മൂല്യം, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    എങ്കിൽ(overwrite) {
        ആകുമ്പോൾ(block<pLimit) {
            *block++=value;
        }
    } അല്ലെങ്കിൽ {
        ആകുമ്പോൾ(block<pLimit) {
            എങ്കിൽ(*block==initial_മൂല്യം) {
                *block=value;
            }
            ++block;
        }
    }
}

U_CAPI ശൂന്യം U_EXPORT2
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

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    എങ്കിൽ((uint32_t)start>0x10ffff || (uint32_t)end>0x10ffff || start>end) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    newTrie=trie->newTrie;
    എങ്കിൽ(newTrie==nullptr || newTrie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        തിരികെ;
    }
#ifdef UCPTRIE_DEBUG
    umutablecptrie_setRange(newTrie->t3, start, end, value, pErrorCode);
#endif
    എങ്കിൽ(!overwrite && value==newTrie->initial_മൂല്യം) {
        തിരികെ; /* nothing to do */
    }

    limit=end+1;
    എങ്കിൽ(start&UTRIE2_DATA_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=getDataBlock(newTrie, start, സത്യം);
        എങ്കിൽ(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }

        nextStart=(start+UTRIE2_DATA_MASK)&~UTRIE2_DATA_MASK;
        എങ്കിൽ(nextStart<=limit) {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, UTRIE2_DATA_BLOCK_LENGTH,
                      value, newTrie->initial_മൂല്യം, overwrite);
            start=nextStart;
        } അല്ലെങ്കിൽ {
            fillBlock(newTrie->data+block, start&UTRIE2_DATA_MASK, limit&UTRIE2_DATA_MASK,
                      value, newTrie->initial_മൂല്യം, overwrite);
            തിരികെ;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE2_DATA_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE2_DATA_MASK;

    /* iterate over all-value blocks */
    എങ്കിൽ(value==newTrie->initial_മൂല്യം) {
        repeatBlock=newTrie->dataNullOffset;
    } അല്ലെങ്കിൽ {
        repeatBlock=-1;
    }

    ആകുമ്പോൾ(start<limit) {
        int32_t i2;
        UBool setRepeatBlock=അസത്യം;

        എങ്കിൽ(value==newTrie->initial_മൂല്യം && isInNullBlock(newTrie, start, സത്യം)) {
            start+=UTRIE2_DATA_BLOCK_LENGTH; /* nothing to do */
            തുടരുക;
        }

        /* get index value */
        i2=getIndex2Block(newTrie, start, സത്യം);
        എങ്കിൽ(i2<0) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            തിരികെ;
        }
        i2+=(start>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
        block=newTrie->index2[i2];
        എങ്കിൽ(isWritableBlock(newTrie, block)) {
            /* already allocated */
            എങ്കിൽ(overwrite && block>=UNEWTRIE2_DATA_0800_OFFSET) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                setRepeatBlock=സത്യം;
            } അല്ലെങ്കിൽ {
                /* !overwrite, or protected block: just write the values into this block */
                fillBlock(newTrie->data+block,
                          0, UTRIE2_DATA_BLOCK_LENGTH,
                          value, newTrie->initial_മൂല്യം, overwrite);
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(newTrie->data[block]!=value && (overwrite || block==newTrie->dataNullOffset)) {
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
            setRepeatBlock=സത്യം;
        }
        എങ്കിൽ(setRepeatBlock) {
            എങ്കിൽ(repeatBlock>=0) {
                setIndex2Entry(newTrie, i2, repeatBlock);
            } അല്ലെങ്കിൽ {
                /* create and set and fill the repeatBlock */
                repeatBlock=getDataBlock(newTrie, start, സത്യം);
                എങ്കിൽ(repeatBlock<0) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    തിരികെ;
                }
                writeBlock(newTrie->data+repeatBlock, value);
            }
        }

        start+=UTRIE2_DATA_BLOCK_LENGTH;
    }

    എങ്കിൽ(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=getDataBlock(newTrie, start, സത്യം);
        എങ്കിൽ(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }

        fillBlock(newTrie->data+block, 0, rest, value, newTrie->initial_മൂല്യം, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

സ്ഥിതം വരിയിൽ UBool
equal_int32(സ്ഥിരം int32_t *s, സ്ഥിരം int32_t *t, int32_t length) {
    ആകുമ്പോൾ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    തിരികെ length == 0;
}

സ്ഥിതം വരിയിൽ UBool
equal_uint32(സ്ഥിരം uint32_t *s, സ്ഥിരം uint32_t *t, int32_t length) {
    ആകുമ്പോൾ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    തിരികെ length == 0;
}

സ്ഥിതം int32_t
findSameIndex2Block(സ്ഥിരം int32_t *idx, int32_t index2Length, int32_t otherBlock) {
    int32_t block;

    /* ensure that we do not even partially get past index2Length */
    index2Length-=UTRIE2_INDEX_2_BLOCK_LENGTH;

    വേണ്ടി(block=0; block<=index2Length; ++block) {
        എങ്കിൽ(equal_int32(idx+block, idx+otherBlock, UTRIE2_INDEX_2_BLOCK_LENGTH)) {
            തിരികെ block;
        }
    }
    തിരികെ -1;
}

സ്ഥിതം int32_t
findSameDataBlock(സ്ഥിരം uint32_t *data, int32_t dataLength, int32_t otherBlock, int32_t blockLength) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=blockLength;

    വേണ്ടി(block=0; block<=dataLength; block+=UTRIE2_DATA_GRANULARITY) {
        എങ്കിൽ(equal_uint32(data+block, data+otherBlock, blockLength)) {
            തിരികെ block;
        }
    }
    തിരികെ -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
സ്ഥിതം UChar32
find_മുകളിലെ_Start(UNewTrie2 *trie, uint32_t മുകളിലെ_മൂല്യം) {
    സ്ഥിരം uint32_t *data32;

    uint32_t value, initial_മൂല്യം;
    UChar32 c, prev;
    int32_t i1, i2, j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    data32=trie->data;
    initial_മൂല്യം=trie->initial_മൂല്യം;

    index2NullOffset=trie->index2NullOffset;
    nullBlock=trie->dataNullOffset;

    /* set variables for previous range */
    എങ്കിൽ(മുകളിലെ_മൂല്യം==initial_മൂല്യം) {
        prevI2Block=index2NullOffset;
        prevBlock=nullBlock;
    } അല്ലെങ്കിൽ {
        prevI2Block=-1;
        prevBlock=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_INDEX_1_LENGTH;
    c=prev;
    ആകുമ്പോൾ(c>0) {
        i2Block=trie->index1[--i1];
        എങ്കിൽ(i2Block==prevI2Block) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
            തുടരുക;
        }
        prevI2Block=i2Block;
        എങ്കിൽ(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            എങ്കിൽ(മുകളിലെ_മൂല്യം!=initial_മൂല്യം) {
                തിരികെ c;
            }
            c-=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } അല്ലെങ്കിൽ {
            /* enumerate data blocks for one index-2 block */
            വേണ്ടി(i2=UTRIE2_INDEX_2_BLOCK_LENGTH; i2>0;) {
                block=trie->index2[i2Block+ --i2];
                എങ്കിൽ(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                    തുടരുക;
                }
                prevBlock=block;
                എങ്കിൽ(block==nullBlock) {
                    /* this is the null data block */
                    എങ്കിൽ(മുകളിലെ_മൂല്യം!=initial_മൂല്യം) {
                        തിരികെ c;
                    }
                    c-=UTRIE2_DATA_BLOCK_LENGTH;
                } അല്ലെങ്കിൽ {
                    വേണ്ടി(j=UTRIE2_DATA_BLOCK_LENGTH; j>0;) {
                        value=data32[block+ --j];
                        എങ്കിൽ(value!=മുകളിലെ_മൂല്യം) {
                            തിരികെ c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    തിരികെ 0;
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
സ്ഥിതം ശൂന്യം
compactData(UNewTrie2 *trie) {
#ifdef UTRIE2_DEBUG
    int32_t എണ്ണം_Same=0, sumOverlaps=0;
#endif

    int32_t start, newStart, movedStart;
    int32_t blockLength, overlap;
    int32_t i, mapIndex, block_എണ്ണം;

    /* do not compact linear-ASCII data */
    newStart=UTRIE2_DATA_START_OFFSET;
    വേണ്ടി(start=0, i=0; start<newStart; start+=UTRIE2_DATA_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    blockLength=64;
    block_എണ്ണം=blockLength>>UTRIE2_SHIFT_2;
    വേണ്ടി(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        എങ്കിൽ(start==UNEWTRIE2_DATA_0800_OFFSET) {
            blockLength=UTRIE2_DATA_BLOCK_LENGTH;
            block_എണ്ണം=1;
        }

        /* skip blocks that are not used */
        എങ്കിൽ(trie->map[start>>UTRIE2_SHIFT_2]<=0) {
            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            തുടരുക;
        }

        /* search for an identical block */
        എങ്കിൽ( (movedStart=findSameDataBlock(trie->data, newStart, start, blockLength))
             >=0
        ) {
#ifdef UTRIE2_DEBUG
            ++എണ്ണം_Same;
#endif
            /* found an identical block, set the other block's index value for the current block */
            വേണ്ടി(i=block_എണ്ണം, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* advance start to the next block */
            start+=blockLength;

            /* leave newStart with the previous block! */
            തുടരുക;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        വേണ്ടി(overlap=blockLength-UTRIE2_DATA_GRANULARITY;
            overlap>0 && !equal_uint32(trie->data+(newStart-overlap), trie->data+start, overlap);
            overlap-=UTRIE2_DATA_GRANULARITY) {}

#ifdef UTRIE2_DEBUG
            sumOverlaps+=overlap;
#endif
        എങ്കിൽ(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            movedStart=newStart-overlap;
            വേണ്ടി(i=block_എണ്ണം, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=movedStart;
                movedStart+=UTRIE2_DATA_BLOCK_LENGTH;
            }

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            വേണ്ടി(i=blockLength-overlap; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } അല്ലെങ്കിൽ /* no overlap && newStart==start */ {
            വേണ്ടി(i=block_എണ്ണം, mapIndex=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[mapIndex++]=start;
                start+=UTRIE2_DATA_BLOCK_LENGTH;
            }
            newStart=start;
        }
    }

    /* now adjust the index-2 table */
    വേണ്ടി(i=0; i<trie->index2Length; ++i) {
        എങ്കിൽ(i==UNEWTRIE2_INDEX_GAP_OFFSET) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_INDEX_GAP_LENGTH;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_SHIFT_2];
    }
    trie->dataNullOffset=trie->map[trie->dataNullOffset>>UTRIE2_SHIFT_2];

    /* ensure dataLength alignment */
    ആകുമ്പോൾ((newStart&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        trie->data[newStart++]=trie->initial_മൂല്യം;
    }

#ifdef UTRIE2_DEBUG
    /* we saved some space */
    printf("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (long)trie->dataLength, (long)newStart, (long)എണ്ണം_Same, (long)sumOverlaps);
#endif

    trie->dataLength=newStart;
}

സ്ഥിതം ശൂന്യം
compactIndex2(UNewTrie2 *trie) {
    int32_t i, start, newStart, movedStart, overlap;

    /* do not compact linear-BMP index-2 blocks */
    newStart=UTRIE2_INDEX_2_BMP_LENGTH;
    വേണ്ടി(start=0, i=0; start<newStart; start+=UTRIE2_INDEX_2_BLOCK_LENGTH, ++i) {
        trie->map[i]=start;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    newStart+=UTRIE2_UTF8_2B_INDEX_2_LENGTH+((trie->മുകളിലെ_Start-0x10000)>>UTRIE2_SHIFT_1);

    വേണ്ടി(start=UNEWTRIE2_INDEX_2_NULL_OFFSET; start<trie->index2Length;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        എങ്കിൽ( (movedStart=findSameIndex2Block(trie->index2, newStart, start))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=movedStart;

            /* advance start to the next block */
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            തുടരുക;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        വേണ്ടി(overlap=UTRIE2_INDEX_2_BLOCK_LENGTH-1;
            overlap>0 && !equal_int32(trie->index2+(newStart-overlap), trie->index2+start, overlap);
            --overlap) {}

        എങ്കിൽ(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=newStart-overlap;

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            വേണ്ടി(i=UTRIE2_INDEX_2_BLOCK_LENGTH-overlap; i>0; --i) {
                trie->index2[newStart++]=trie->index2[start++];
            }
        } അല്ലെങ്കിൽ /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE2_SHIFT_1_2]=start;
            start+=UTRIE2_INDEX_2_BLOCK_LENGTH;
            newStart=start;
        }
    }

    /* now adjust the index-1 table */
    വേണ്ടി(i=0; i<UNEWTRIE2_INDEX_1_LENGTH; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_SHIFT_1_2];
    }
    trie->index2NullOffset=trie->map[trie->index2NullOffset>>UTRIE2_SHIFT_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    ആകുമ്പോൾ((newStart&((UTRIE2_DATA_GRANULARITY-1)|1))!=0) {
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

സ്ഥിതം ശൂന്യം
compactTrie(UTrie2 *trie, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UChar32 മുകളിലെ_Start, supp_മുകളിലെ_Start;
    uint32_t മുകളിലെ_മൂല്യം;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    മുകളിലെ_മൂല്യം=utrie2_get32(trie, 0x10ffff);
    മുകളിലെ_Start=find_മുകളിലെ_Start(newTrie, മുകളിലെ_മൂല്യം);
    മുകളിലെ_Start=(മുകളിലെ_Start+(UTRIE2_CP_PER_INDEX_1_ENTRY-1))&~(UTRIE2_CP_PER_INDEX_1_ENTRY-1);
    എങ്കിൽ(മുകളിലെ_Start==0x110000) {
        മുകളിലെ_മൂല്യം=trie->error_മൂല്യം;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->മുകളിലെ_Start=newTrie->മുകളിലെ_Start=മുകളിലെ_Start;

#ifdef UTRIE2_DEBUG
    printf("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (long)മുകളിലെ_Start, (long)മുകളിലെ_മൂല്യം, (long)trie->initial_മൂല്യം);
#endif

    എങ്കിൽ(മുകളിലെ_Start<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_മുകളിലെ_Start= മുകളിലെ_Start<=0x10000 ? 0x10000 : മുകളിലെ_Start;
        utrie2_setRange32(trie, supp_മുകളിലെ_Start, 0x10ffff, trie->initial_മൂല്യം, സത്യം, pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            തിരികെ;
        }
    }

    compactData(newTrie);
    എങ്കിൽ(മുകളിലെ_Start>0x10000) {
        compactIndex2(newTrie);
#ifdef UTRIE2_DEBUG
    } അല്ലെങ്കിൽ {
        printf("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (long)മുകളിലെ_Start, (long)trie->newTrie->index2Length, (long)UTRIE2_INDEX_1_OFFSET);
#endif
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->data[newTrie->dataLength++]=മുകളിലെ_മൂല്യം;
    ആകുമ്പോൾ((newTrie->dataLength&(UTRIE2_DATA_GRANULARITY-1))!=0) {
        newTrie->data[newTrie->dataLength++]=trie->initial_മൂല്യം;
    }

    newTrie->isCompacted=സത്യം;
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
U_CAPI ശൂന്യം U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_മൂല്യം_Bits മൂല്യം_Bits, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t allIndexesLength;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */
    UChar32 മുകളിലെ_Start;

    /* argument check */
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    എങ്കിൽ( trie==nullptr ||
        മൂല്യം_Bits<0 || UTRIE2_എണ്ണം_മൂല്യം_BITS<=മൂല്യം_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    newTrie=trie->newTrie;
    എങ്കിൽ(newTrie==nullptr) {
        /* already frozen */
        UTrie2_മൂല്യം_Bits frozen_മൂല്യം_Bits=
            trie->data16!=nullptr ? UTRIE2_16_മൂല്യം_BITS : UTRIE2_32_മൂല്യം_BITS;
        എങ്കിൽ(മൂല്യം_Bits!=frozen_മൂല്യം_Bits) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
        തിരികെ;
    }

    /* compact if necessary */
    എങ്കിൽ(!newTrie->isCompacted) {
        compactTrie(trie, pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            തിരികെ;
        }
    }
    മുകളിലെ_Start=trie->മുകളിലെ_Start;

    എങ്കിൽ(മുകളിലെ_Start<=0x10000) {
        allIndexesLength=UTRIE2_INDEX_1_OFFSET;
    } അല്ലെങ്കിൽ {
        allIndexesLength=newTrie->index2Length;
    }
    എങ്കിൽ(മൂല്യം_Bits==UTRIE2_16_മൂല്യം_BITS) {
        dataMove=allIndexesLength;
    } അല്ലെങ്കിൽ {
        dataMove=0;
    }

    /* are indexLength and dataLength within limits? */
    എങ്കിൽ( /* for unshifted indexLength */
        allIndexesLength>UTRIE2_MAX_INDEX_LENGTH ||
        /* for unshifted dataNullOffset */
        (dataMove+newTrie->dataNullOffset)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (dataMove+UNEWTRIE2_DATA_0800_OFFSET)>0xffff ||
        /* for shiftedDataLength */
        (dataMove+newTrie->dataLength)>UTRIE2_MAX_DATA_LENGTH
    ) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ;
    }

    /* calculate the total serialized length */
    length=വലുപ്പം(UTrie2Header)+allIndexesLength*2;
    എങ്കിൽ(മൂല്യം_Bits==UTRIE2_16_മൂല്യം_BITS) {
        length+=newTrie->dataLength*2;
    } അല്ലെങ്കിൽ {
        length+=newTrie->dataLength*4;
    }

    trie->memory=uprv_malloc(length);
    എങ്കിൽ(trie->memory==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    trie->length=length;
    trie->isMemoryOwned=സത്യം;

    trie->indexLength=allIndexesLength;
    trie->dataLength=newTrie->dataLength;
    എങ്കിൽ(മുകളിലെ_Start<=0x10000) {
        trie->index2NullOffset=0xffff;
    } അല്ലെങ്കിൽ {
        trie->index2NullOffset=static_cast<uint16_t>(UTRIE2_INDEX_2_OFFSET+newTrie->index2NullOffset);
    }
    trie->dataNullOffset=(uint16_t)(dataMove+newTrie->dataNullOffset);
    trie->മുകളിലെ_മൂല്യം_Index=dataMove+trie->dataLength-UTRIE2_DATA_GRANULARITY;

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)മൂല്യം_Bits;

    header->indexLength=(uint16_t)trie->indexLength;
    header->shiftedDataLength=(uint16_t)(trie->dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=trie->index2NullOffset;
    header->dataNullOffset=trie->dataNullOffset;
    header->shifted_മുകളിലെ_Start=(uint16_t)(മുകളിലെ_Start>>UTRIE2_SHIFT_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    വേണ്ടി(i=UTRIE2_INDEX_2_BMP_LENGTH; i>0; --i) {
        *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    വേണ്ടി(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    വേണ്ടി(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(dataMove+newTrie->index2[i<<(6-UTRIE2_SHIFT_2)]);
    }

    എങ്കിൽ(മുകളിലെ_Start>0x10000) {
        int32_t index1Length=(മുകളിലെ_Start-0x10000)>>UTRIE2_SHIFT_1;
        int32_t index2Offset=UTRIE2_INDEX_2_BMP_LENGTH+UTRIE2_UTF8_2B_INDEX_2_LENGTH+index1Length;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_INDEX_1_LENGTH;
        വേണ്ടി(i=index1Length; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_INDEX_2_OFFSET + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2Offset;
        വേണ്ടി(i=newTrie->index2Length-index2Offset; i>0; --i) {
            *dest16++=(uint16_t)((dataMove + *p++)>>UTRIE2_INDEX_SHIFT);
        }
    }

    /* write the 16/32-bit data array */
    തിരഞ്ഞെടുപ്പ്(മൂല്യം_Bits) {
    അവസ്ഥ UTRIE2_16_മൂല്യം_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        p=newTrie->data;
        വേണ്ടി(i=newTrie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        നിർത്തുക;
    അവസ്ഥ UTRIE2_32_മൂല്യം_BITS:
        /* write 32-bit data values */
        trie->data16=nullptr;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->data, (size_t)newTrie->dataLength*4);
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

#ifdef UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#endif

#ifdef UCPTRIE_DEBUG
    umutablecptrie_setName(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_മൂല്യം_Width)മൂല്യം_Bits, pErrorCode));
#endif
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->data);
    uprv_free(newTrie);
    trie->newTrie=nullptr;
}

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
#अगर_घोषित UTRIE2_DEBUG
#   include <stdio.h>
#पूर्ण_अगर
// #define UCPTRIE_DEBUG

#समावेश "unicode/utypes.h"
#अगर_घोषित UCPTRIE_DEBUG
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/umutablecptrie.h"
#समावेश "ucptrie_impl.h"
#पूर्ण_अगर
#समावेश "cmemory.h"
#समावेश "utrie2.h"
#समावेश "utrie2_impl.h"

#समावेश "utrie.h"  // for utrie2_fromUTrie()

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

क्रमागत {
    /** The null index-2 block, following the gap in the index-2 table. */
    UNEWTRIE2_सूचकांक_2_NULL_OFFSET=UNEWTRIE2_सूचकांक_GAP_OFFSET+UNEWTRIE2_सूचकांक_GAP_लंबाई,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_सूचकांक_2_START_OFFSET=UNEWTRIE2_सूचकांक_2_NULL_OFFSET+UTRIE2_सूचकांक_2_BLOCK_लंबाई,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_तथ्य_NULL_OFFSET=UTRIE2_तथ्य_START_OFFSET,

    /** The start of allocated data blocks. */
    UNEWTRIE2_तथ्य_START_OFFSET=UNEWTRIE2_तथ्य_NULL_OFFSET+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_तथ्य_0800_OFFSET=UNEWTRIE2_तथ्य_START_OFFSET+0x780
};

/* Start with allocation of 16k data entries. */
#घोषणा UNEWTRIE2_INITIAL_तथ्य_लंबाई ((int32_t)1<<14)

/* Grow about 8x each time. */
#घोषणा UNEWTRIE2_MEDIUM_तथ्य_लंबाई ((int32_t)1<<17)

अटल int32_t
allocIndex2Block(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_मूल्य, uint32_t error_मूल्य, UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *तथ्य;
    int32_t i, j;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(माप(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(माप(UNewTrie2));
    तथ्य=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_तथ्य_लंबाई*4);
    अगर(trie==nullptr || newTrie==nullptr || तथ्य==nullptr) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(तथ्य);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }

    uprv_memset(trie, 0, माप(UTrie2));
    trie->initial_मूल्य=initial_मूल्य;
    trie->error_मूल्य=error_मूल्य;
    trie->ऊपरी_Start=0x110000;
    trie->newTrie=newTrie;
#अगर_घोषित UTRIE2_DEBUG
    trie->name="open";
#पूर्ण_अगर

    newTrie->तथ्य=तथ्य;
#अगर_घोषित UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_मूल्य, error_मूल्य, pErrorCode);
#पूर्ण_अगर
    newTrie->तथ्य_Capacity=UNEWTRIE2_INITIAL_तथ्य_लंबाई;
    newTrie->initial_मूल्य=initial_मूल्य;
    newTrie->error_मूल्य=error_मूल्य;
    newTrie->ऊपरी_Start=0x110000;
    newTrie->firstFreeBlock=0;  /* no free block in the list */
    newTrie->isCompacted=false;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    क्रम(i=0; i<0x80; ++i) {
        newTrie->तथ्य[i]=initial_मूल्य;
    }
    क्रम(; i<0xc0; ++i) {
        newTrie->तथ्य[i]=error_मूल्य;
    }
    क्रम(i=UNEWTRIE2_तथ्य_NULL_OFFSET; i<UNEWTRIE2_तथ्य_START_OFFSET; ++i) {
        newTrie->तथ्य[i]=initial_मूल्य;
    }
    newTrie->तथ्य_Null_Offset=UNEWTRIE2_तथ्य_NULL_OFFSET;
    newTrie->तथ्य_लंबाई=UNEWTRIE2_तथ्य_START_OFFSET;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    क्रम(i=0, j=0; j<0x80; ++i, j+=UTRIE2_तथ्य_BLOCK_लंबाई) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    क्रम(; j<0xc0; ++i, j+=UTRIE2_तथ्य_BLOCK_लंबाई) {
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
        UTRIE2_LSCP_सूचकांक_2_लंबाई;
    j+=UTRIE2_तथ्य_BLOCK_लंबाई;
    क्रम(; j<UNEWTRIE2_तथ्य_START_OFFSET; ++i, j+=UTRIE2_तथ्य_BLOCK_लंबाई) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    क्रम(i=0x80>>UTRIE2_SHIFT_2; i<UTRIE2_सूचकांक_2_BMP_लंबाई; ++i) {
        newTrie->index2[i]=UNEWTRIE2_तथ्य_NULL_OFFSET;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    क्रम(i=0; i<UNEWTRIE2_सूचकांक_GAP_लंबाई; ++i) {
        newTrie->index2[UNEWTRIE2_सूचकांक_GAP_OFFSET+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    क्रम(i=0; i<UTRIE2_सूचकांक_2_BLOCK_लंबाई; ++i) {
        newTrie->index2[UNEWTRIE2_सूचकांक_2_NULL_OFFSET+i]=UNEWTRIE2_तथ्य_NULL_OFFSET;
    }
    newTrie->index2NullOffset=UNEWTRIE2_सूचकांक_2_NULL_OFFSET;
    newTrie->index2_लंबाई=UNEWTRIE2_सूचकांक_2_START_OFFSET;

    /* set the index-1 indexes for the linear index-2 block */
    क्रम(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_सूचकांक_1_लंबाई;
        ++i, j+=UTRIE2_सूचकांक_2_BLOCK_लंबाई
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    क्रम(; i<UNEWTRIE2_सूचकांक_1_लंबाई; ++i) {
        newTrie->index1[i]=UNEWTRIE2_सूचकांक_2_NULL_OFFSET;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    क्रम(i=0x80; i<0x800; i+=UTRIE2_तथ्य_BLOCK_लंबाई) {
        utrie2_set32(trie, i, initial_मूल्य, pErrorCode);
    }

    वापस trie;
}

अटल UNewTrie2 *
cloneBuilder(स्थिर UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(माप(UNewTrie2)));
    अगर(trie==nullptr) {
        वापस nullptr;
    }

    trie->तथ्य = static_cast<uint32_t*>(uprv_malloc(other->तथ्य_Capacity * 4));
    अगर(trie->तथ्य==nullptr) {
        uprv_free(trie);
        वापस nullptr;
    }
#अगर_घोषित UCPTRIE_DEBUG
    अगर(other->t3==nullptr) {
        trie->t3=nullptr;
    } अन्यथा {
        UErrorCode errorCode=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &errorCode);
    }
#पूर्ण_अगर
    trie->तथ्य_Capacity=other->तथ्य_Capacity;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, माप(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (माप_प्रकार)other->index2_लंबाई*4);
    trie->index2NullOffset=other->index2NullOffset;
    trie->index2_लंबाई=other->index2_लंबाई;

    uprv_memcpy(trie->तथ्य, other->तथ्य, (माप_प्रकार)other->तथ्य_लंबाई*4);
    trie->तथ्य_Null_Offset=other->तथ्य_Null_Offset;
    trie->तथ्य_लंबाई=other->तथ्य_लंबाई;

    /* reference counters */
    अगर(other->isCompacted) {
        trie->firstFreeBlock=0;
    } अन्यथा {
        uprv_memcpy(trie->map, other->map, ((माप_प्रकार)other->तथ्य_लंबाई>>UTRIE2_SHIFT_2)*4);
        trie->firstFreeBlock=other->firstFreeBlock;
    }

    trie->initial_मूल्य=other->initial_मूल्य;
    trie->error_मूल्य=other->error_मूल्य;
    trie->ऊपरी_Start=other->ऊपरी_Start;
    trie->isCompacted=other->isCompacted;

    वापस trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(स्थिर UTrie2 *other, UErrorCode *pErrorCode) {
    UTrie2 *trie;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    trie=(UTrie2 *)uprv_malloc(माप(UTrie2));
    अगर(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memcpy(trie, other, माप(UTrie2));

    अगर(other->memory!=nullptr) {
        trie->memory=uprv_malloc(other->length);
        अगर(trie->memory!=nullptr) {
            trie->isMemoryOwned=true;
            uprv_memcpy(trie->memory, other->memory, other->length);

            /* make the clone's pointers point to its own memory */
            trie->सूचकांक=(uint16_t *)trie->memory+(other->सूचकांक-(uint16_t *)other->memory);
            अगर(other->data16!=nullptr) {
                trie->data16=(uint16_t *)trie->memory+(other->data16-(uint16_t *)other->memory);
            }
            अगर(other->data32!=nullptr) {
                trie->data32=(uint32_t *)trie->memory+(other->data32-(uint32_t *)other->memory);
            }
        }
    } अन्यथा /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    अगर(trie->memory==nullptr && trie->newTrie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        uprv_free(trie);
        trie=nullptr;
    }
    वापस trie;
}

प्रकार काष्ठा NewTrieAndStatus {
    UTrie2 *trie;
    UErrorCode errorCode;
    UBool exclusiveLimit;  /* rather than inclusive range end */
} NewTrieAndStatus;

अटल UBool U_CALLCONV
copyEnumRange(स्थिर व्योम *context, UChar32 start, UChar32 end, uint32_t value) {
    NewTrieAndStatus *nt=(NewTrieAndStatus *)context;
    अगर(value!=nt->trie->initial_मूल्य) {
        अगर(nt->exclusiveLimit) {
            --end;
        }
        अगर(start==end) {
            utrie2_set32(nt->trie, start, value, &nt->errorCode);
        } अन्यथा {
            utrie2_setRange32(nt->trie, start, end, value, true, &nt->errorCode);
        }
        वापस U_SUCCESS(nt->errorCode);
    } अन्यथा {
        वापस true;
    }
}

#अगर_घोषित UTRIE2_DEBUG
अटल दीर्घ गिनती_Initial(स्थिर UTrie2 *trie) {
    uint32_t initial_मूल्य=trie->initial_मूल्य;
    int32_t length=trie->तथ्य_लंबाई;
    दीर्घ count=0;
    अगर(trie->data16!=nullptr) {
        क्रम(int32_t i=0; i<length; ++i) {
            अगर(trie->data16[i]==initial_मूल्य) { ++count; }
        }
    } अन्यथा {
        क्रम(int32_t i=0; i<length; ++i) {
            अगर(trie->data32[i]==initial_मूल्य) { ++count; }
        }
    }
    वापस count;
}

अटल व्योम
utrie_printLengths(स्थिर UTrie *trie) {
    दीर्घ सूचकांक_लंबाई=trie->सूचकांक_लंबाई;
    दीर्घ तथ्य_लंबाई=(दीर्घ)trie->तथ्य_लंबाई;
    दीर्घ योग_लंबाई=(दीर्घ)माप(UTrieHeader)+सूचकांक_लंबाई*2+तथ्य_लंबाई*(trie->data32!=nullptr ? 4 : 2);
    म_लिखो("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           सूचकांक_लंबाई, तथ्य_लंबाई, योग_लंबाई);
}

अटल व्योम
utrie2_printLengths(स्थिर UTrie2 *trie, स्थिर अक्षर *which) {
    दीर्घ सूचकांक_लंबाई=trie->सूचकांक_लंबाई;
    दीर्घ तथ्य_लंबाई=(दीर्घ)trie->तथ्य_लंबाई;
    दीर्घ योग_लंबाई=(दीर्घ)माप(UTrie2Header)+सूचकांक_लंबाई*2+तथ्य_लंबाई*(trie->data32!=nullptr ? 4 : 2);
    म_लिखो("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, सूचकांक_लंबाई, तथ्य_लंबाई, गिनती_Initial(trie), योग_लंबाई);
}
#पूर्ण_अगर

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(स्थिर UTrie2 *other, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर(other==nullptr || (other->memory==nullptr && other->newTrie==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    अगर(other->newTrie!=nullptr && !other->newTrie->isCompacted) {
        वापस utrie2_clone(other, pErrorCode);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_मूल्य, other->error_मूल्य, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    context.exclusiveLimit=false;
    context.errorCode=*pErrorCode;
    utrie2_enum(other, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    क्रम(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        अगर(other->data32==nullptr) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } अन्यथा {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        अगर(value!=other->initial_मूल्य) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    अगर(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    वापस context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(स्थिर UTrie *trie1, uint32_t error_मूल्य, UErrorCode *pErrorCode) {
    NewTrieAndStatus context;
    char16_t lead;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    अगर(trie1==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    context.trie=utrie2_open(trie1->initial_मूल्य, error_मूल्य, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }
    context.exclusiveLimit=true;
    context.errorCode=*pErrorCode;
    utrie_enum(trie1, nullptr, copyEnumRange, &context);
    *pErrorCode=context.errorCode;
    क्रम(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        अगर(trie1->data32==nullptr) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } अन्यथा {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        अगर(value!=trie1->initial_मूल्य) {
            utrie2_set32ForLeadSurrogateCodeUnit(context.trie, lead, value, pErrorCode);
        }
    }
    अगर(U_SUCCESS(*pErrorCode)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=nullptr ? UTRIE2_32_मूल्य_BITS : UTRIE2_16_मूल्य_BITS,
                      pErrorCode);
    }
#अगर_घोषित UTRIE2_DEBUG
    अगर(U_SUCCESS(*pErrorCode)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#पूर्ण_अगर
    अगर(U_FAILURE(*pErrorCode)) {
        utrie2_close(context.trie);
        context.trie=nullptr;
    }
    वापस context.trie;
}

अटल अंतरभूत UBool
isInNullBlock(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, block;

    अगर(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_सूचकांक_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } अन्यथा {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_सूचकांक_2_MASK);
    }
    block=trie->index2[i2];
    वापस block == trie->तथ्य_Null_Offset;
}

अटल int32_t
allocIndex2Block(UNewTrie2 *trie) {
    int32_t newBlock, new_शीर्ष;

    newBlock=trie->index2_लंबाई;
    new_शीर्ष=newBlock+UTRIE2_सूचकांक_2_BLOCK_लंबाई;
    अगर(new_शीर्ष>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        वापस -1;
    }
    trie->index2_लंबाई=new_शीर्ष;
    uprv_memcpy(trie->index2+newBlock, trie->index2+trie->index2NullOffset, UTRIE2_सूचकांक_2_BLOCK_लंबाई*4);
    वापस newBlock;
}

अटल int32_t
getIndex2Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    अगर(U_IS_LEAD(c) && forLSCP) {
        वापस UTRIE2_LSCP_सूचकांक_2_OFFSET;
    }

    i1=c>>UTRIE2_SHIFT_1;
    i2=trie->index1[i1];
    अगर(i2==trie->index2NullOffset) {
        i2=allocIndex2Block(trie);
        अगर(i2<0) {
            वापस -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    वापस i2;
}

अटल int32_t
alloc_तथ्य_Block(UNewTrie2 *trie, int32_t copyBlock) {
    int32_t newBlock, new_शीर्ष;

    अगर(trie->firstFreeBlock!=0) {
        /* get the first free block */
        newBlock=trie->firstFreeBlock;
        trie->firstFreeBlock=-trie->map[newBlock>>UTRIE2_SHIFT_2];
    } अन्यथा {
        /* get a new block from the high end */
        newBlock=trie->तथ्य_लंबाई;
        new_शीर्ष=newBlock+UTRIE2_तथ्य_BLOCK_लंबाई;
        अगर(new_शीर्ष>trie->तथ्य_Capacity) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *तथ्य;

            अगर(trie->तथ्य_Capacity<UNEWTRIE2_MEDIUM_तथ्य_लंबाई) {
                capacity=UNEWTRIE2_MEDIUM_तथ्य_लंबाई;
            } अन्यथा अगर(trie->तथ्य_Capacity<UNEWTRIE2_MAX_तथ्य_लंबाई) {
                capacity=UNEWTRIE2_MAX_तथ्य_लंबाई;
            } अन्यथा {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                वापस -1;
            }
            तथ्य = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            अगर(तथ्य==nullptr) {
                वापस -1;
            }
            uprv_memcpy(तथ्य, trie->तथ्य, (माप_प्रकार)trie->तथ्य_लंबाई*4);
            uprv_free(trie->तथ्य);
            trie->तथ्य=तथ्य;
            trie->तथ्य_Capacity=capacity;
        }
        trie->तथ्य_लंबाई=new_शीर्ष;
    }
    uprv_memcpy(trie->तथ्य+newBlock, trie->तथ्य+copyBlock, UTRIE2_तथ्य_BLOCK_लंबाई*4);
    trie->map[newBlock>>UTRIE2_SHIFT_2]=0;
    वापस newBlock;
}

/* call when the block's reference counter reaches 0 */
अटल व्योम
release_तथ्य_Block(UNewTrie2 *trie, int32_t block) {
    /* put this block at the front of the free-block chain */
    trie->map[block>>UTRIE2_SHIFT_2]=-trie->firstFreeBlock;
    trie->firstFreeBlock=block;
}

अटल अंतरभूत UBool
isWritableBlock(UNewTrie2 *trie, int32_t block) {
    वापस block != trie->तथ्य_Null_Offset && 1 == trie->map[block >> UTRIE2_SHIFT_2];
}

अटल अंतरभूत व्योम
setIndex2Entry(UNewTrie2 *trie, int32_t i2, int32_t block) {
    int32_t oldBlock;
    ++trie->map[block>>UTRIE2_SHIFT_2];  /* increment first, in case block==oldBlock! */
    oldBlock=trie->index2[i2];
    अगर(0 == --trie->map[oldBlock>>UTRIE2_SHIFT_2]) {
        release_तथ्य_Block(trie, oldBlock);
    }
    trie->index2[i2]=block;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
अटल int32_t
get_तथ्य_Block(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, oldBlock, newBlock;

    i2=getIndex2Block(trie, c, forLSCP);
    अगर(i2<0) {
        वापस -1;  /* program error */
    }

    i2+=(c>>UTRIE2_SHIFT_2)&UTRIE2_सूचकांक_2_MASK;
    oldBlock=trie->index2[i2];
    अगर(isWritableBlock(trie, oldBlock)) {
        वापस oldBlock;
    }

    /* allocate a new data block */
    newBlock=alloc_तथ्य_Block(trie, oldBlock);
    अगर(newBlock<0) {
        /* out of memory in the data array */
        वापस -1;
    }
    setIndex2Entry(trie, i2, newBlock);
    वापस newBlock;
}

/**
 * @return true if the value was successfully set
 */
अटल व्योम
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UErrorCode *pErrorCode) {
    int32_t block;

    अगर(trie==nullptr || trie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        वापस;
    }
#अगर_घोषित UCPTRIE_DEBUG
    umutablecptrie_set(trie->t3, c, value, pErrorCode);
#पूर्ण_अगर

    block=get_तथ्य_Block(trie, c, forLSCP);
    अगर(block<0) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    trie->तथ्य[block+(c&UTRIE2_तथ्य_MASK)]=value;
}

U_CAPI व्योम U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    अगर((uint32_t)c>0x10ffff) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    set32(trie->newTrie, c, true, value, pErrorCode);
}

U_CAPI व्योम U_EXPORT2
utrie2_set32ForLeadSurrogateCodeUnit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    अगर(!U_IS_LEAD(c)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    set32(trie->newTrie, c, false, value, pErrorCode);
}

अटल व्योम
writeBlock(uint32_t *block, uint32_t value) {
    uint32_t *limit=block+UTRIE2_तथ्य_BLOCK_लंबाई;
    जबतक(block<limit) {
        *block++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
अटल व्योम
fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
          uint32_t value, uint32_t initial_मूल्य, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    अगर(overwrite) {
        जबतक(block<pLimit) {
            *block++=value;
        }
    } अन्यथा {
        जबतक(block<pLimit) {
            अगर(*block==initial_मूल्य) {
                *block=value;
            }
            ++block;
        }
    }
}

U_CAPI व्योम U_EXPORT2
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

    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    अगर((uint32_t)start>0x10ffff || (uint32_t)end>0x10ffff || start>end) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    newTrie=trie->newTrie;
    अगर(newTrie==nullptr || newTrie->isCompacted) {
        *pErrorCode=U_NO_WRITE_PERMISSION;
        वापस;
    }
#अगर_घोषित UCPTRIE_DEBUG
    umutablecptrie_setRange(newTrie->t3, start, end, value, pErrorCode);
#पूर्ण_अगर
    अगर(!overwrite && value==newTrie->initial_मूल्य) {
        वापस; /* nothing to do */
    }

    limit=end+1;
    अगर(start&UTRIE2_तथ्य_MASK) {
        UChar32 अग्र_Start;

        /* set partial block at [start..following block boundary[ */
        block=get_तथ्य_Block(newTrie, start, true);
        अगर(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }

        अग्र_Start=(start+UTRIE2_तथ्य_MASK)&~UTRIE2_तथ्य_MASK;
        अगर(अग्र_Start<=limit) {
            fillBlock(newTrie->तथ्य+block, start&UTRIE2_तथ्य_MASK, UTRIE2_तथ्य_BLOCK_लंबाई,
                      value, newTrie->initial_मूल्य, overwrite);
            start=अग्र_Start;
        } अन्यथा {
            fillBlock(newTrie->तथ्य+block, start&UTRIE2_तथ्य_MASK, limit&UTRIE2_तथ्य_MASK,
                      value, newTrie->initial_मूल्य, overwrite);
            वापस;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE2_तथ्य_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE2_तथ्य_MASK;

    /* iterate over all-value blocks */
    अगर(value==newTrie->initial_मूल्य) {
        repeatBlock=newTrie->तथ्य_Null_Offset;
    } अन्यथा {
        repeatBlock=-1;
    }

    जबतक(start<limit) {
        int32_t i2;
        UBool setRepeatBlock=false;

        अगर(value==newTrie->initial_मूल्य && isInNullBlock(newTrie, start, true)) {
            start+=UTRIE2_तथ्य_BLOCK_लंबाई; /* nothing to do */
            जारी;
        }

        /* get index value */
        i2=getIndex2Block(newTrie, start, true);
        अगर(i2<0) {
            *pErrorCode=U_INTERNAL_PROGRAM_ERROR;
            वापस;
        }
        i2+=(start>>UTRIE2_SHIFT_2)&UTRIE2_सूचकांक_2_MASK;
        block=newTrie->index2[i2];
        अगर(isWritableBlock(newTrie, block)) {
            /* already allocated */
            अगर(overwrite && block>=UNEWTRIE2_तथ्य_0800_OFFSET) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                setRepeatBlock=true;
            } अन्यथा {
                /* !overwrite, or protected block: just write the values into this block */
                fillBlock(newTrie->तथ्य+block,
                          0, UTRIE2_तथ्य_BLOCK_लंबाई,
                          value, newTrie->initial_मूल्य, overwrite);
            }
        } अन्यथा अगर(newTrie->तथ्य[block]!=value && (overwrite || block==newTrie->तथ्य_Null_Offset)) {
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
        अगर(setRepeatBlock) {
            अगर(repeatBlock>=0) {
                setIndex2Entry(newTrie, i2, repeatBlock);
            } अन्यथा {
                /* create and set and fill the repeatBlock */
                repeatBlock=get_तथ्य_Block(newTrie, start, true);
                अगर(repeatBlock<0) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    वापस;
                }
                writeBlock(newTrie->तथ्य+repeatBlock, value);
            }
        }

        start+=UTRIE2_तथ्य_BLOCK_लंबाई;
    }

    अगर(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=get_तथ्य_Block(newTrie, start, true);
        अगर(block<0) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }

        fillBlock(newTrie->तथ्य+block, 0, rest, value, newTrie->initial_मूल्य, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

अटल अंतरभूत UBool
equal_int32(स्थिर int32_t *s, स्थिर int32_t *t, int32_t length) {
    जबतक(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    वापस length == 0;
}

अटल अंतरभूत UBool
equal_uint32(स्थिर uint32_t *s, स्थिर uint32_t *t, int32_t length) {
    जबतक(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    वापस length == 0;
}

अटल int32_t
findSameIndex2Block(स्थिर int32_t *idx, int32_t index2_लंबाई, int32_t otherBlock) {
    int32_t block;

    /* ensure that we do not even partially get past index2Length */
    index2_लंबाई-=UTRIE2_सूचकांक_2_BLOCK_लंबाई;

    क्रम(block=0; block<=index2_लंबाई; ++block) {
        अगर(equal_int32(idx+block, idx+otherBlock, UTRIE2_सूचकांक_2_BLOCK_लंबाई)) {
            वापस block;
        }
    }
    वापस -1;
}

अटल int32_t
find_Same_तथ्य_Block_2(स्थिर uint32_t *तथ्य, int32_t तथ्य_लंबाई, int32_t otherBlock, int32_t block_लंबाई) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    तथ्य_लंबाई-=block_लंबाई;

    क्रम(block=0; block<=तथ्य_लंबाई; block+=UTRIE2_तथ्य_GRANULARITY) {
        अगर(equal_uint32(तथ्य+block, तथ्य+otherBlock, block_लंबाई)) {
            वापस block;
        }
    }
    वापस -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
अटल UChar32
find_ऊपरी_Start(UNewTrie2 *trie, uint32_t ऊपरी_मूल्य) {
    स्थिर uint32_t *data32;

    uint32_t value, initial_मूल्य;
    UChar32 c, पूर्व;
    int32_t i1, i2, j, i2Block, पूर्व_I2_Block, index2NullOffset, block, पूर्व_Block, nullBlock;

    data32=trie->तथ्य;
    initial_मूल्य=trie->initial_मूल्य;

    index2NullOffset=trie->index2NullOffset;
    nullBlock=trie->तथ्य_Null_Offset;

    /* set variables for previous range */
    अगर(ऊपरी_मूल्य==initial_मूल्य) {
        पूर्व_I2_Block=index2NullOffset;
        पूर्व_Block=nullBlock;
    } अन्यथा {
        पूर्व_I2_Block=-1;
        पूर्व_Block=-1;
    }
    पूर्व=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_सूचकांक_1_लंबाई;
    c=पूर्व;
    जबतक(c>0) {
        i2Block=trie->index1[--i1];
        अगर(i2Block==पूर्व_I2_Block) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_PER_सूचकांक_1_ENTRY;
            जारी;
        }
        पूर्व_I2_Block=i2Block;
        अगर(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            अगर(ऊपरी_मूल्य!=initial_मूल्य) {
                वापस c;
            }
            c-=UTRIE2_CP_PER_सूचकांक_1_ENTRY;
        } अन्यथा {
            /* enumerate data blocks for one index-2 block */
            क्रम(i2=UTRIE2_सूचकांक_2_BLOCK_लंबाई; i2>0;) {
                block=trie->index2[i2Block+ --i2];
                अगर(block==पूर्व_Block) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_तथ्य_BLOCK_लंबाई;
                    जारी;
                }
                पूर्व_Block=block;
                अगर(block==nullBlock) {
                    /* this is the null data block */
                    अगर(ऊपरी_मूल्य!=initial_मूल्य) {
                        वापस c;
                    }
                    c-=UTRIE2_तथ्य_BLOCK_लंबाई;
                } अन्यथा {
                    क्रम(j=UTRIE2_तथ्य_BLOCK_लंबाई; j>0;) {
                        value=data32[block+ --j];
                        अगर(value!=ऊपरी_मूल्य) {
                            वापस c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    वापस 0;
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
अटल व्योम
compact_तथ्य(UNewTrie2 *trie) {
#अगर_घोषित UTRIE2_DEBUG
    int32_t गिनती_Same=0, योग_Overlaps=0;
#पूर्ण_अगर

    int32_t start, newStart, movedStart;
    int32_t block_लंबाई, overlap;
    int32_t i, map_सूचकांक, block_गिनती;

    /* do not compact linear-ASCII data */
    newStart=UTRIE2_तथ्य_START_OFFSET;
    क्रम(start=0, i=0; start<newStart; start+=UTRIE2_तथ्य_BLOCK_लंबाई, ++i) {
        trie->map[i]=start;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    block_लंबाई=64;
    block_गिनती=block_लंबाई>>UTRIE2_SHIFT_2;
    क्रम(start=newStart; start<trie->तथ्य_लंबाई;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        अगर(start==UNEWTRIE2_तथ्य_0800_OFFSET) {
            block_लंबाई=UTRIE2_तथ्य_BLOCK_लंबाई;
            block_गिनती=1;
        }

        /* skip blocks that are not used */
        अगर(trie->map[start>>UTRIE2_SHIFT_2]<=0) {
            /* advance start to the next block */
            start+=block_लंबाई;

            /* leave newStart with the previous block! */
            जारी;
        }

        /* search for an identical block */
        अगर( (movedStart=find_Same_तथ्य_Block_2(trie->तथ्य, newStart, start, block_लंबाई))
             >=0
        ) {
#अगर_घोषित UTRIE2_DEBUG
            ++गिनती_Same;
#पूर्ण_अगर
            /* found an identical block, set the other block's index value for the current block */
            क्रम(i=block_गिनती, map_सूचकांक=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[map_सूचकांक++]=movedStart;
                movedStart+=UTRIE2_तथ्य_BLOCK_लंबाई;
            }

            /* advance start to the next block */
            start+=block_लंबाई;

            /* leave newStart with the previous block! */
            जारी;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        क्रम(overlap=block_लंबाई-UTRIE2_तथ्य_GRANULARITY;
            overlap>0 && !equal_uint32(trie->तथ्य+(newStart-overlap), trie->तथ्य+start, overlap);
            overlap-=UTRIE2_तथ्य_GRANULARITY) {}

#अगर_घोषित UTRIE2_DEBUG
            योग_Overlaps+=overlap;
#पूर्ण_अगर
        अगर(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            movedStart=newStart-overlap;
            क्रम(i=block_गिनती, map_सूचकांक=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[map_सूचकांक++]=movedStart;
                movedStart+=UTRIE2_तथ्य_BLOCK_लंबाई;
            }

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            क्रम(i=block_लंबाई-overlap; i>0; --i) {
                trie->तथ्य[newStart++]=trie->तथ्य[start++];
            }
        } अन्यथा /* no overlap && newStart==start */ {
            क्रम(i=block_गिनती, map_सूचकांक=start>>UTRIE2_SHIFT_2; i>0; --i) {
                trie->map[map_सूचकांक++]=start;
                start+=UTRIE2_तथ्य_BLOCK_लंबाई;
            }
            newStart=start;
        }
    }

    /* now adjust the index-2 table */
    क्रम(i=0; i<trie->index2_लंबाई; ++i) {
        अगर(i==UNEWTRIE2_सूचकांक_GAP_OFFSET) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_सूचकांक_GAP_लंबाई;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_SHIFT_2];
    }
    trie->तथ्य_Null_Offset=trie->map[trie->तथ्य_Null_Offset>>UTRIE2_SHIFT_2];

    /* ensure dataLength alignment */
    जबतक((newStart&(UTRIE2_तथ्य_GRANULARITY-1))!=0) {
        trie->तथ्य[newStart++]=trie->initial_मूल्य;
    }

#अगर_घोषित UTRIE2_DEBUG
    /* we saved some space */
    म_लिखो("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (दीर्घ)trie->तथ्य_लंबाई, (दीर्घ)newStart, (दीर्घ)गिनती_Same, (दीर्घ)योग_Overlaps);
#पूर्ण_अगर

    trie->तथ्य_लंबाई=newStart;
}

अटल व्योम
compactIndex2(UNewTrie2 *trie) {
    int32_t i, start, newStart, movedStart, overlap;

    /* do not compact linear-BMP index-2 blocks */
    newStart=UTRIE2_सूचकांक_2_BMP_लंबाई;
    क्रम(start=0, i=0; start<newStart; start+=UTRIE2_सूचकांक_2_BLOCK_लंबाई, ++i) {
        trie->map[i]=start;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    newStart+=UTRIE2_UTF8_2_B_सूचकांक_2_लंबाई+((trie->ऊपरी_Start-0x10000)>>UTRIE2_SHIFT_1);

    क्रम(start=UNEWTRIE2_सूचकांक_2_NULL_OFFSET; start<trie->index2_लंबाई;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        अगर( (movedStart=findSameIndex2Block(trie->index2, newStart, start))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=movedStart;

            /* advance start to the next block */
            start+=UTRIE2_सूचकांक_2_BLOCK_लंबाई;

            /* leave newStart with the previous block! */
            जारी;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        क्रम(overlap=UTRIE2_सूचकांक_2_BLOCK_लंबाई-1;
            overlap>0 && !equal_int32(trie->index2+(newStart-overlap), trie->index2+start, overlap);
            --overlap) {}

        अगर(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            trie->map[start>>UTRIE2_SHIFT_1_2]=newStart-overlap;

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            क्रम(i=UTRIE2_सूचकांक_2_BLOCK_लंबाई-overlap; i>0; --i) {
                trie->index2[newStart++]=trie->index2[start++];
            }
        } अन्यथा /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE2_SHIFT_1_2]=start;
            start+=UTRIE2_सूचकांक_2_BLOCK_लंबाई;
            newStart=start;
        }
    }

    /* now adjust the index-1 table */
    क्रम(i=0; i<UNEWTRIE2_सूचकांक_1_लंबाई; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_SHIFT_1_2];
    }
    trie->index2NullOffset=trie->map[trie->index2NullOffset>>UTRIE2_SHIFT_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    जबतक((newStart&((UTRIE2_तथ्य_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[newStart++] = static_cast<int32_t>(0xffff) << UTRIE2_सूचकांक_SHIFT;
    }

#अगर_घोषित UTRIE2_DEBUG
    /* we saved some space */
    म_लिखो("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (दीर्घ)trie->index2_लंबाई, (दीर्घ)newStart);
#पूर्ण_अगर

    trie->index2_लंबाई=newStart;
}

अटल व्योम
compactTrie(UTrie2 *trie, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UChar32 ऊपरी_Start, supp_ऊपरी_Start;
    uint32_t ऊपरी_मूल्य;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    ऊपरी_मूल्य=utrie2_get32(trie, 0x10ffff);
    ऊपरी_Start=find_ऊपरी_Start(newTrie, ऊपरी_मूल्य);
    ऊपरी_Start=(ऊपरी_Start+(UTRIE2_CP_PER_सूचकांक_1_ENTRY-1))&~(UTRIE2_CP_PER_सूचकांक_1_ENTRY-1);
    अगर(ऊपरी_Start==0x110000) {
        ऊपरी_मूल्य=trie->error_मूल्य;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->ऊपरी_Start=newTrie->ऊपरी_Start=ऊपरी_Start;

#अगर_घोषित UTRIE2_DEBUG
    म_लिखो("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (दीर्घ)ऊपरी_Start, (दीर्घ)ऊपरी_मूल्य, (दीर्घ)trie->initial_मूल्य);
#पूर्ण_अगर

    अगर(ऊपरी_Start<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_ऊपरी_Start= ऊपरी_Start<=0x10000 ? 0x10000 : ऊपरी_Start;
        utrie2_setRange32(trie, supp_ऊपरी_Start, 0x10ffff, trie->initial_मूल्य, true, pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            वापस;
        }
    }

    compact_तथ्य(newTrie);
    अगर(ऊपरी_Start>0x10000) {
        compactIndex2(newTrie);
#अगर_घोषित UTRIE2_DEBUG
    } अन्यथा {
        म_लिखो("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (दीर्घ)ऊपरी_Start, (दीर्घ)trie->newTrie->index2_लंबाई, (दीर्घ)UTRIE2_सूचकांक_1_OFFSET);
#पूर्ण_अगर
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->तथ्य[newTrie->तथ्य_लंबाई++]=ऊपरी_मूल्य;
    जबतक((newTrie->तथ्य_लंबाई&(UTRIE2_तथ्य_GRANULARITY-1))!=0) {
        newTrie->तथ्य[newTrie->तथ्य_लंबाई++]=trie->initial_मूल्य;
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
#घोषणा UTRIE2_MAX_सूचकांक_लंबाई 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#घोषणा UTRIE2_MAX_तथ्य_लंबाई (0xffff<<UTRIE2_सूचकांक_SHIFT)

/* Compact and internally serialize the trie. */
U_CAPI व्योम U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_मूल्य_Bits मूल्य_Bits, UErrorCode *pErrorCode) {
    UNewTrie2 *newTrie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_लंबाई;
    int32_t तथ्य_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 ऊपरी_Start;

    /* argument check */
    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }
    अगर( trie==nullptr ||
        मूल्य_Bits<0 || UTRIE2_गिनती_मूल्य_BITS<=मूल्य_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    newTrie=trie->newTrie;
    अगर(newTrie==nullptr) {
        /* already frozen */
        UTrie2_मूल्य_Bits frozen_मूल्य_Bits=
            trie->data16!=nullptr ? UTRIE2_16_मूल्य_BITS : UTRIE2_32_मूल्य_BITS;
        अगर(मूल्य_Bits!=frozen_मूल्य_Bits) {
            *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        }
        वापस;
    }

    /* compact if necessary */
    अगर(!newTrie->isCompacted) {
        compactTrie(trie, pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            वापस;
        }
    }
    ऊपरी_Start=trie->ऊपरी_Start;

    अगर(ऊपरी_Start<=0x10000) {
        all_Indexes_लंबाई=UTRIE2_सूचकांक_1_OFFSET;
    } अन्यथा {
        all_Indexes_लंबाई=newTrie->index2_लंबाई;
    }
    अगर(मूल्य_Bits==UTRIE2_16_मूल्य_BITS) {
        तथ्य_Move=all_Indexes_लंबाई;
    } अन्यथा {
        तथ्य_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    अगर( /* for unshifted indexLength */
        all_Indexes_लंबाई>UTRIE2_MAX_सूचकांक_लंबाई ||
        /* for unshifted dataNullOffset */
        (तथ्य_Move+newTrie->तथ्य_Null_Offset)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (तथ्य_Move+UNEWTRIE2_तथ्य_0800_OFFSET)>0xffff ||
        /* for shiftedDataLength */
        (तथ्य_Move+newTrie->तथ्य_लंबाई)>UTRIE2_MAX_तथ्य_लंबाई
    ) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस;
    }

    /* calculate the total serialized length */
    length=माप(UTrie2Header)+all_Indexes_लंबाई*2;
    अगर(मूल्य_Bits==UTRIE2_16_मूल्य_BITS) {
        length+=newTrie->तथ्य_लंबाई*2;
    } अन्यथा {
        length+=newTrie->तथ्य_लंबाई*4;
    }

    trie->memory=uprv_malloc(length);
    अगर(trie->memory==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    trie->length=length;
    trie->isMemoryOwned=true;

    trie->सूचकांक_लंबाई=all_Indexes_लंबाई;
    trie->तथ्य_लंबाई=newTrie->तथ्य_लंबाई;
    अगर(ऊपरी_Start<=0x10000) {
        trie->index2NullOffset=0xffff;
    } अन्यथा {
        trie->index2NullOffset=static_cast<uint16_t>(UTRIE2_सूचकांक_2_OFFSET+newTrie->index2NullOffset);
    }
    trie->तथ्य_Null_Offset=(uint16_t)(तथ्य_Move+newTrie->तथ्य_Null_Offset);
    trie->ऊपरी_मूल्य_सूचकांक=तथ्य_Move+trie->तथ्य_लंबाई-UTRIE2_तथ्य_GRANULARITY;

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)मूल्य_Bits;

    header->सूचकांक_लंबाई=(uint16_t)trie->सूचकांक_लंबाई;
    header->shifted_तथ्य_लंबाई=(uint16_t)(trie->तथ्य_लंबाई>>UTRIE2_सूचकांक_SHIFT);
    header->index2NullOffset=trie->index2NullOffset;
    header->तथ्य_Null_Offset=trie->तथ्य_Null_Offset;
    header->shifted_ऊपरी_Start=(uint16_t)(ऊपरी_Start>>UTRIE2_SHIFT_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->सूचकांक=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    क्रम(i=UTRIE2_सूचकांक_2_BMP_लंबाई; i>0; --i) {
        *dest16++=(uint16_t)((तथ्य_Move + *p++)>>UTRIE2_सूचकांक_SHIFT);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    क्रम(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(तथ्य_Move+UTRIE2_BAD_UTF8_तथ्य_OFFSET);
    }
    क्रम(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(तथ्य_Move+newTrie->index2[i<<(6-UTRIE2_SHIFT_2)]);
    }

    अगर(ऊपरी_Start>0x10000) {
        int32_t index1_लंबाई=(ऊपरी_Start-0x10000)>>UTRIE2_SHIFT_1;
        int32_t index2Offset=UTRIE2_सूचकांक_2_BMP_लंबाई+UTRIE2_UTF8_2_B_सूचकांक_2_लंबाई+index1_लंबाई;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_सूचकांक_1_लंबाई;
        क्रम(i=index1_लंबाई; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_सूचकांक_2_OFFSET + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2Offset;
        क्रम(i=newTrie->index2_लंबाई-index2Offset; i>0; --i) {
            *dest16++=(uint16_t)((तथ्य_Move + *p++)>>UTRIE2_सूचकांक_SHIFT);
        }
    }

    /* write the 16/32-bit data array */
    चयन(मूल्य_Bits) {
    हाल UTRIE2_16_मूल्य_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        p=newTrie->तथ्य;
        क्रम(i=newTrie->तथ्य_लंबाई; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        अवरोध;
    हाल UTRIE2_32_मूल्य_BITS:
        /* write 32-bit data values */
        trie->data16=nullptr;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->तथ्य, (माप_प्रकार)newTrie->तथ्य_लंबाई*4);
        अवरोध;
    शेष:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

#अगर_घोषित UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#पूर्ण_अगर

#अगर_घोषित UCPTRIE_DEBUG
    umutablecptrie_setName(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_मूल्य_Width)मूल्य_Bits, pErrorCode));
#पूर्ण_अगर
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->तथ्य);
    uprv_free(newTrie);
    trie->newTrie=nullptr;
}

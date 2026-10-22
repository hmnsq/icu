// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2001-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  utrie.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001oct20
*   created by: Markus W. Scherer
*
*   This is a common implementation of a "folded" trie.
*   It is a kind of compressed, serializable table of 16- or 32-bit values associated with
*   Unicode code points (0..0x10ffff).
*/

#ifdef UTRIE_DEBUG
#   include <stdio.h>
#endif

#include "unicode/utypes.h"
#include "cmemory.h"
#include "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#undef ABS
#define ABS(x) ((x)>=0 ? (x) : -(x))

static anta_hstha UBool
equal_uint32(const uint32_t *s, const uint32_t *t, int32_t length) {
    ᯀᯬᯝ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ᯀᯮᯞ length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fillIn,
           uint32_t *aliasData, int32_t maxDataLength,
           uint32_t initialValue, uint32_t leadUnitValue,
           UBool latin1Linear) {
    UNewTrie *trie;
    int32_t i, j;

    ᯀᯮᯝ( maxDataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (latin1Linear && maxDataLength<1024)
    ) {
        ᯀᯮᯞ nullptr;
    }

    ᯀᯮᯝ(fillIn!=nullptr) {
        trie=fillIn;
    } ᯀᯮᯝᯂᯪ {
        trie=(UNewTrie *)uprv_malloc(parimaa_na(UNewTrie));
        ᯀᯮᯝ(trie==nullptr) {
            ᯀᯮᯞ nullptr;
        }
    }
    uprv_memset(trie, 0, parimaa_na(UNewTrie));
    trie->isAllocated = fillIn == nullptr;

    ᯀᯮᯝ(aliasData!=nullptr) {
        trie->data=aliasData;
        trie->isDataAllocated=false;
    } ᯀᯮᯝᯂᯪ {
        trie->data=(uint32_t *)uprv_malloc(maxDataLength*4);
        ᯀᯮᯝ(trie->data==nullptr) {
            uprv_free(trie);
            ᯀᯮᯞ nullptr;
        }
        trie->isDataAllocated=true;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_DATA_BLOCK_LENGTH;

    ᯀᯮᯝ(latin1Linear) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        do {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->index[i++]=j;
            j+=UTRIE_DATA_BLOCK_LENGTH;
        } ᯀᯬᯝ(i<(256>>UTRIE_SHIFT));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->dataLength=j;
    ᯀᯬᯝ(j>0) {
        trie->data[--j]=initialValue;
    }

    trie->leadUnitValue=leadUnitValue;
    trie->indexLength=UTRIE_MAX_INDEX_LENGTH;
    trie->dataCapacity=maxDataLength;
    trie->isLatin1Linear=latin1Linear;
    trie->isCompacted=false;
    ᯀᯮᯞ trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fillIn, const UNewTrie *other, uint32_t *aliasData, int32_t aliasDataCapacity) {
    UNewTrie *trie;
    UBool isDataAllocated;

    /* do not clone if other is not valid or already compacted */
    ᯀᯮᯝ(other==nullptr || other->data==nullptr || other->isCompacted) {
        ᯀᯮᯞ nullptr;
    }

    /* clone data */
    ᯀᯮᯝ(aliasData!=nullptr && aliasDataCapacity>=other->dataCapacity) {
        isDataAllocated=false;
    } ᯀᯮᯝᯂᯪ {
        aliasDataCapacity=other->dataCapacity;
        aliasData=(uint32_t *)uprv_malloc(other->dataCapacity*4);
        ᯀᯮᯝ(aliasData==nullptr) {
            ᯀᯮᯞ nullptr;
        }
        isDataAllocated=true;
    }

    trie=utrie_open(fillIn, aliasData, aliasDataCapacity,
                    other->data[0], other->leadUnitValue,
                    other->isLatin1Linear);
    ᯀᯮᯝ(trie==nullptr) {
        uprv_free(aliasData);
    } ᯀᯮᯝᯂᯪ {
        uprv_memcpy(trie->index, other->index, parimaa_na(trie->index));
        uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
        trie->dataLength=other->dataLength;
        trie->isDataAllocated=isDataAllocated;
    }

    ᯀᯮᯞ trie;
}

U_CAPI void U_EXPORT2
utrie_close(UNewTrie *trie) {
    ᯀᯮᯝ(trie!=nullptr) {
        ᯀᯮᯝ(trie->isDataAllocated) {
            uprv_free(trie->data);
            trie->data=nullptr;
        }
        ᯀᯮᯝ(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_getData(UNewTrie *trie, int32_t *pLength) {
    ᯀᯮᯝ(trie==nullptr || pLength==nullptr) {
        ᯀᯮᯞ nullptr;
    }

    *pLength=trie->dataLength;
    ᯀᯮᯞ trie->data;
}

static int32_t
utrie_allocDataBlock(UNewTrie *trie) {
    int32_t newBlock, newTop;

    newBlock=trie->dataLength;
    newTop=newBlock+UTRIE_DATA_BLOCK_LENGTH;
    ᯀᯮᯝ(newTop>trie->dataCapacity) {
        /* out of memory in the data array */
        ᯀᯮᯞ -1;
    }
    trie->dataLength=newTop;
    ᯀᯮᯞ newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
static int32_t
utrie_getDataBlock(UNewTrie *trie, UChar32 c) {
    int32_t indexValue, newBlock;

    c>>=UTRIE_SHIFT;
    indexValue=trie->index[c];
    ᯀᯮᯝ(indexValue>0) {
        ᯀᯮᯞ indexValue;
    }

    /* allocate a new data block */
    newBlock=utrie_allocDataBlock(trie);
    ᯀᯮᯝ(newBlock<0) {
        /* out of memory in the data array */
        ᯀᯮᯞ -1;
    }
    trie->index[c]=newBlock;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->data+newBlock, trie->data-indexValue, 4*UTRIE_DATA_BLOCK_LENGTH);
    ᯀᯮᯞ newBlock;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    ᯀᯮᯝ(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        ᯀᯮᯞ false;
    }

    block=utrie_getDataBlock(trie, c);
    ᯀᯮᯝ(block<0) {
        ᯀᯮᯞ false;
    }

    trie->data[block+(c&UTRIE_MASK)]=value;
    ᯀᯮᯞ true;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *pInBlockZero) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    ᯀᯮᯝ(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        ᯀᯮᯝ(pInBlockZero!=nullptr) {
            *pInBlockZero=true;
        }
        ᯀᯮᯞ 0;
    }

    block=trie->index[c>>UTRIE_SHIFT];
    ᯀᯮᯝ(pInBlockZero!=nullptr) {
        *pInBlockZero = block == 0;
    }

    ᯀᯮᯞ trie->data[ABS(block)+(c&UTRIE_MASK)];
}

/**
 * @internal
 */
static void
utrie_fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
                uint32_t value, uint32_t initialValue, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    ᯀᯮᯝ(overwrite) {
        ᯀᯬᯝ(block<pLimit) {
            *block++=value;
        }
    } ᯀᯮᯝᯂᯪ {
        ᯀᯬᯝ(block<pLimit) {
            ᯀᯮᯝ(*block==initialValue) {
                *block=value;
            }
            ++block;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_setRange32(UNewTrie *trie, UChar32 start, UChar32 limit, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initialValue;
    int32_t block, rest, repeatBlock;

    /* valid, uncompacted trie and valid indexes? */
    ᯀᯮᯝ( trie==nullptr || trie->isCompacted ||
        (uint32_t)start>0x10ffff || (uint32_t)limit>0x110000 || start>limit
    ) {
        ᯀᯮᯞ false;
    }
    ᯀᯮᯝ(start==limit) {
        ᯀᯮᯞ true; /* nothing to do */
    }

    initialValue=trie->data[0];
    ᯀᯮᯝ(start&UTRIE_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=utrie_getDataBlock(trie, start);
        ᯀᯮᯝ(block<0) {
            ᯀᯮᯞ false;
        }

        nextStart=(start+UTRIE_DATA_BLOCK_LENGTH)&~UTRIE_MASK;
        ᯀᯮᯝ(nextStart<=limit) {
            utrie_fillBlock(trie->data+block, start&UTRIE_MASK, UTRIE_DATA_BLOCK_LENGTH,
                            value, initialValue, overwrite);
            start=nextStart;
        } ᯀᯮᯝᯂᯪ {
            utrie_fillBlock(trie->data+block, start&UTRIE_MASK, limit&UTRIE_MASK,
                            value, initialValue, overwrite);
            ᯀᯮᯞ true;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE_MASK;

    /* iterate over all-value blocks */
    ᯀᯮᯝ(value==initialValue) {
        repeatBlock=0;
    } ᯀᯮᯝᯂᯪ {
        repeatBlock=-1;
    }
    ᯀᯬᯝ(start<limit) {
        /* get index value */
        block=trie->index[start>>UTRIE_SHIFT];
        ᯀᯮᯝ(block>0) {
            /* already allocated, fill in value */
            utrie_fillBlock(trie->data+block, 0, UTRIE_DATA_BLOCK_LENGTH, value, initialValue, overwrite);
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(trie->data[-block]!=value && (block==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            ᯀᯮᯝ(repeatBlock>=0) {
                trie->index[start>>UTRIE_SHIFT]=-repeatBlock;
            } ᯀᯮᯝᯂᯪ {
                /* create and set and fill the repeatBlock */
                repeatBlock=utrie_getDataBlock(trie, start);
                ᯀᯮᯝ(repeatBlock<0) {
                    ᯀᯮᯞ false;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->index[start>>UTRIE_SHIFT]=-repeatBlock;
                utrie_fillBlock(trie->data+repeatBlock, 0, UTRIE_DATA_BLOCK_LENGTH, value, initialValue, true);
            }
        }

        start+=UTRIE_DATA_BLOCK_LENGTH;
    }

    ᯀᯮᯝ(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=utrie_getDataBlock(trie, start);
        ᯀᯮᯝ(block<0) {
            ᯀᯮᯞ false;
        }

        utrie_fillBlock(trie->data+block, 0, rest, value, initialValue, overwrite);
    }

    ᯀᯮᯞ true;
}

static int32_t
_findSameIndexBlock(const int32_t *idx, int32_t indexLength,
                    int32_t otherBlock) {
    int32_t block, i;

    for(block=UTRIE_BMP_INDEX_LENGTH; block<indexLength; block+=UTRIE_SURROGATE_BLOCK_COUNT) {
        for(i=0; i<UTRIE_SURROGATE_BLOCK_COUNT; ++i) {
            ᯀᯮᯝ(idx[block+i]!=idx[otherBlock+i]) {
                bha_nga;
            }
        }
        ᯀᯮᯝ(i==UTRIE_SURROGATE_BLOCK_COUNT) {
            ᯀᯮᯞ block;
        }
    }
    ᯀᯮᯞ indexLength;
}

/*
 * Fold the normalization data for supplementary code points into
 * a compact area on top of the BMP-part of the trie index,
 * with the lead surrogates indexing this compact area.
 *
 * Duplicate the index values for lead surrogates:
 * From inside the BMP area, where some may be overridden with folded values,
 * to just after the BMP area, where they can be retrieved for
 * code point lookups.
 */
static void
utrie_fold(UNewTrie *trie, UNewTrieGetFoldedValue *getFoldedValue, UErrorCode *pErrorCode) {
    int32_t leadIndexes[UTRIE_SURROGATE_BLOCK_COUNT];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t indexLength, block;
#ifdef UTRIE_DEBUG
    puur_naa_nka countLeadCUWithData=0;
#endif

    idx=trie->index;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_SHIFT), 4*UTRIE_SURROGATE_BLOCK_COUNT);

    /*
     * set all values for lead surrogate code *units* to leadUnitValue
     * so that, by default, runtime lookups will find no data for associated
     * supplementary code points, unless there is data for such code points
     * which will result in a non-zero folding value below that is set for
     * the respective lead units
     *
     * the above saved the indexes for surrogate code *points*
     * fill the indexes with simplified code from utrie_setRange32()
     */
    ᯀᯮᯝ(trie->leadUnitValue==trie->data[0]) {
        block=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } ᯀᯮᯝᯂᯪ {
        /* create and fill the repeatBlock */
        block=utrie_allocDataBlock(trie);
        ᯀᯮᯝ(block<0) {
            /* data table overflow */
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            ᯀᯮᯞ;
        }
        utrie_fillBlock(trie->data+block, 0, UTRIE_DATA_BLOCK_LENGTH, trie->leadUnitValue, trie->data[0], true);
        block=-block; /* negative block number to indicate that it is a repeat block */
    }
    for(c=(0xd800>>UTRIE_SHIFT); c<(0xdc00>>UTRIE_SHIFT); ++c) {
        trie->index[c]=block;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    indexLength=UTRIE_BMP_INDEX_LENGTH;

    /* search for any index (stage 1) entries for supplementary code points */
    for(c=0x10000; c<0x110000;) {
        ᯀᯮᯝ(idx[c>>UTRIE_SHIFT]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#ifdef UTRIE_DEBUG
            ++countLeadCUWithData;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#endif

            /* is there an identical index block? */
            block=_findSameIndexBlock(idx, indexLength, c>>UTRIE_SHIFT);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=getFoldedValue(trie, c, block+UTRIE_SURROGATE_BLOCK_COUNT);
            ᯀᯮᯝ(value!=utrie_get32(trie, U16_LEAD(c), nullptr)) {
                ᯀᯮᯝ(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    ᯀᯮᯞ;
                }

                /* if we did not find an identical index block... */
                ᯀᯮᯝ(block==indexLength) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+indexLength,
                                 idx+(c>>UTRIE_SHIFT),
                                 4*UTRIE_SURROGATE_BLOCK_COUNT);
                    indexLength+=UTRIE_SURROGATE_BLOCK_COUNT;
                }
            }
            c+=0x400;
        } ᯀᯮᯝᯂᯪ {
            c+=UTRIE_DATA_BLOCK_LENGTH;
        }
    }
#ifdef UTRIE_DEBUG
    ᯀᯮᯝ(countLeadCUWithData>0) {
        printf("supplementary data for %d lead surrogates\n", countLeadCUWithData);
    }
#endif

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    ᯀᯮᯝ(indexLength>=UTRIE_MAX_INDEX_LENGTH) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ᯀᯮᯞ;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_INDEX_LENGTH+UTRIE_SURROGATE_BLOCK_COUNT,
                 idx+UTRIE_BMP_INDEX_LENGTH,
                 4*(indexLength-UTRIE_BMP_INDEX_LENGTH));
    uprv_memcpy(idx+UTRIE_BMP_INDEX_LENGTH,
                leadIndexes,
                4*UTRIE_SURROGATE_BLOCK_COUNT);
    indexLength+=UTRIE_SURROGATE_BLOCK_COUNT;

#ifdef UTRIE_DEBUG
    printf("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_INDEX_LENGTH, (long)UTRIE_MAX_INDEX_LENGTH, indexLength);
#endif

    trie->indexLength=indexLength;
}

/*
 * Set a value in the trie index map to indicate which data block
 * is referenced and which one is not.
 * utrie_compact() will remove data blocks that are not used at all.
 * Set
 * - 0 if it is used
 * - -1 if it is not used
 */
static void
_findUnusedBlocks(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_TIME_DATA_LENGTH>>UTRIE_SHIFT)*4);

    /* mark each block that _is_ used with 0 */
    for(i=0; i<trie->indexLength; ++i) {
        trie->map[ABS(trie->index[i])>>UTRIE_SHIFT]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

static int32_t
_findSameDataBlock(const uint32_t *data, int32_t dataLength,
                   int32_t otherBlock, int32_t step) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=UTRIE_DATA_BLOCK_LENGTH;

    for(block=0; block<=dataLength; block+=step) {
        ᯀᯮᯝ(equal_uint32(data+block, data+otherBlock, UTRIE_DATA_BLOCK_LENGTH)) {
            ᯀᯮᯞ block;
        }
    }
    ᯀᯮᯞ -1;
}

/*
 * Compact a folded build-time trie.
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
static void
utrie_compact(UNewTrie *trie, UBool overlap, UErrorCode *pErrorCode) {
    int32_t i, start, newStart, overlapStart;

    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ;
    }

    /* valid, uncompacted trie? */
    ᯀᯮᯝ(trie==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ(trie->isCompacted) {
        ᯀᯮᯞ; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    _findUnusedBlocks(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    ᯀᯮᯝ(trie->isLatin1Linear && UTRIE_SHIFT<=8) {
        overlapStart=UTRIE_DATA_BLOCK_LENGTH+256;
    } ᯀᯮᯝᯂᯪ {
        overlapStart=UTRIE_DATA_BLOCK_LENGTH;
    }

    newStart=UTRIE_DATA_BLOCK_LENGTH;
    for(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        ᯀᯮᯝ(trie->map[start>>UTRIE_SHIFT]<0) {
            /* advance start to the next block */
            start+=UTRIE_DATA_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            continue;
        }

        /* search for an identical block */
        ᯀᯮᯝ( start>=overlapStart &&
            (i=_findSameDataBlock(trie->data, newStart, start,
                            overlap ? UTRIE_DATA_GRANULARITY : UTRIE_DATA_BLOCK_LENGTH))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE_SHIFT]=i;

            /* advance start to the next block */
            start+=UTRIE_DATA_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            continue;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        ᯀᯮᯝ(overlap && start>=overlapStart) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            for(i=UTRIE_DATA_BLOCK_LENGTH-UTRIE_DATA_GRANULARITY;
                i>0 && !equal_uint32(trie->data+(newStart-i), trie->data+start, i);
                i-=UTRIE_DATA_GRANULARITY) {}
        } ᯀᯮᯝᯂᯪ {
            i=0;
        }

        ᯀᯮᯝ(i>0) {
            /* some overlap */
            trie->map[start>>UTRIE_SHIFT]=newStart-i;

            /* move the non-overlapping indexes to their new positions */
            start+=i;
            for(i=UTRIE_DATA_BLOCK_LENGTH-i; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(newStart<start) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[start>>UTRIE_SHIFT]=newStart;
            for(i=UTRIE_DATA_BLOCK_LENGTH; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } ᯀᯮᯝᯂᯪ /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE_SHIFT]=start;
            newStart+=UTRIE_DATA_BLOCK_LENGTH;
            start=newStart;
        }
    }

    /* now adjust the index (stage 1) table */
    for(i=0; i<trie->indexLength; ++i) {
        trie->index[i]=trie->map[ABS(trie->index[i])>>UTRIE_SHIFT];
    }

#ifdef UTRIE_DEBUG
    /* we saved some space */
    printf("compacting trie: count of 32-bit words %lu->%lu\n",
            (long)trie->dataLength, (long)newStart);
#endif

    trie->dataLength=newStart;
}

/* serialization ------------------------------------------------------------ */

/*
 * Default function for the folding value:
 * Just store the offset (16 bits) if there is any non-initial-value entry.
 *
 * The offset parameter is never 0.
 * Returning the offset itself is safe for UTRIE_SHIFT>=5 because
 * for UTRIE_SHIFT==5 the maximum index length is UTRIE_MAX_INDEX_LENGTH==0x8800
 * which fits into 16-bit trie values;
 * for higher UTRIE_SHIFT, UTRIE_MAX_INDEX_LENGTH decreases.
 *
 * Theoretically, it would be safer for all possible UTRIE_SHIFT including
 * those of 4 and lower to return offset>>UTRIE_SURROGATE_BLOCK_BITS
 * which would always result in a value of 0x40..0x43f
 * (start/end 1k blocks of supplementary Unicode code points).
 * However, this would be uglier, and would not work for some existing
 * binary data file formats.
 *
 * Also, we do not plan to change UTRIE_SHIFT because it would change binary
 * data file formats, and we would probably not make it smaller because of
 * the then even larger BMP index length even for empty tries.
 */
static uint32_t U_CALLCONV
defaultGetFoldedValue(UNewTrie *trie, UChar32 start, int32_t offset) {
    uint32_t value, initialValue;
    UChar32 limit;
    UBool inBlockZero;

    initialValue=trie->data[0];
    limit=start+0x400;
    ᯀᯬᯝ(start<limit) {
        value=utrie_get32(trie, start, &inBlockZero);
        ᯀᯮᯝ(inBlockZero) {
            start+=UTRIE_DATA_BLOCK_LENGTH;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(value!=initialValue) {
            ᯀᯮᯞ static_cast<uint32_t>(offset);
        } ᯀᯮᯝᯂᯪ {
            ++start;
        }
    }
    ᯀᯮᯞ 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, void *dt, int32_t capacity,
                UNewTrieGetFoldedValue *getFoldedValue,
                UBool reduceTo16Bits,
                UErrorCode *pErrorCode) {
    UTrieHeader *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* data = nullptr;

    /* argument check */
    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ 0;
    }

    ᯀᯮᯝ(trie==nullptr || capacity<0 || (capacity>0 && dt==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ 0;
    }
    ᯀᯮᯝ(getFoldedValue==nullptr) {
        getFoldedValue=defaultGetFoldedValue;
    }

    data = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    ᯀᯮᯝ(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, false, pErrorCode);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, getFoldedValue, pErrorCode);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, true, pErrorCode);

        trie->isCompacted=true;
        ᯀᯮᯝ(U_FAILURE(*pErrorCode)) {
            ᯀᯮᯞ 0;
        }
    }

    /* is dataLength within limits? */
    ᯀᯮᯝ( (reduceTo16Bits ? (trie->dataLength+trie->indexLength) : trie->dataLength) >= UTRIE_MAX_DATA_LENGTH) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    }

    length=parimaa_na(UTrieHeader)+2*trie->indexLength;
    ᯀᯮᯝ(reduceTo16Bits) {
        length+=2*trie->dataLength;
    } ᯀᯮᯝᯂᯪ {
        length+=4*trie->dataLength;
    }

    ᯀᯮᯝ(length>capacity) {
        ᯀᯮᯞ length; /* preflighting */
    }

#ifdef UTRIE_DEBUG
    printf("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (long)trie->indexLength, (long)trie->dataLength, (long)length);
#endif

    /* set the header fields */
    header=(UTrieHeader *)data;
    data+=parimaa_na(UTrieHeader);

    header->signature=0x54726965; /* "Trie" */
    header->options=UTRIE_SHIFT | (UTRIE_INDEX_SHIFT<<UTRIE_OPTIONS_INDEX_SHIFT);

    ᯀᯮᯝ(!reduceTo16Bits) {
        header->options|=UTRIE_OPTIONS_DATA_IS_32_BIT;
    }
    ᯀᯮᯝ(trie->isLatin1Linear) {
        header->options|=UTRIE_OPTIONS_LATIN1_IS_LINEAR;
    }

    header->indexLength=trie->indexLength;
    header->dataLength=trie->dataLength;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    ᯀᯮᯝ(reduceTo16Bits) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->index;
        dest16=(uint16_t *)data;
        for(i=trie->indexLength; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->indexLength)>>UTRIE_INDEX_SHIFT);
        }

        /* write 16-bit data values */
        p=trie->data;
        for(i=trie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } ᯀᯮᯝᯂᯪ {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->index;
        dest16=(uint16_t *)data;
        for(i=trie->indexLength; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_INDEX_SHIFT);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->data, 4*(size_t)trie->dataLength);
    }

    ᯀᯮᯞ length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_defaultGetFoldingOffset(uint32_t data) {
    ᯀᯮᯞ (int32_t)data;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, const void *data, int32_t length, UErrorCode *pErrorCode) {
    const UTrieHeader *header;
    const uint16_t *p16;
    uint32_t options;

    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ -1;
    }

    /* enough data for a trie header? */
    ᯀᯮᯝ(length<(int32_t)parimaa_na(UTrieHeader)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ᯀᯮᯞ -1;
    }

    /* check the signature */
    header=(const UTrieHeader *)data;
    ᯀᯮᯝ(header->signature!=0x54726965) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ᯀᯮᯞ -1;
    }

    /* get the options and check the shift values */
    options=header->options;
    ᯀᯮᯝ( (options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ᯀᯮᯞ -1;
    }
    trie->isLatin1Linear = (options & UTRIE_OPTIONS_LATIN1_IS_LINEAR) != 0;

    /* get the length values */
    trie->indexLength=header->indexLength;
    trie->dataLength=header->dataLength;

    length-=(int32_t)parimaa_na(UTrieHeader);

    /* enough data for the index? */
    ᯀᯮᯝ(length<2*trie->indexLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ᯀᯮᯞ -1;
    }
    p16=(const uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;
    length-=2*trie->indexLength;

    /* get the data */
    ᯀᯮᯝ(options&UTRIE_OPTIONS_DATA_IS_32_BIT) {
        ᯀᯮᯝ(length<4*trie->dataLength) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ᯀᯮᯞ -1;
        }
        trie->data32=(const uint32_t *)p16;
        trie->initialValue=trie->data32[0];
        length=(int32_t)parimaa_na(UTrieHeader)+2*trie->indexLength+4*trie->dataLength;
    } ᯀᯮᯝᯂᯪ {
        ᯀᯮᯝ(length<2*trie->dataLength) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ᯀᯮᯞ -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=nullptr;
        trie->initialValue=trie->index[trie->indexLength];
        length=(int32_t)parimaa_na(UTrieHeader)+2*trie->indexLength+2*trie->dataLength;
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    ᯀᯮᯞ length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       void *data, int32_t length,
                       uint32_t initialValue, uint32_t leadUnitValue,
                       UBool make16BitTrie,
                       UErrorCode *pErrorCode) {
    uint16_t *p16;
    int32_t actualLength, latin1Length, i, limit;
    uint16_t block;

    ᯀᯮᯝ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᯀᯮᯞ -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1Length= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->indexLength=UTRIE_BMP_INDEX_LENGTH+UTRIE_SURROGATE_BLOCK_COUNT;
    trie->dataLength=latin1Length;
    ᯀᯮᯝ(leadUnitValue!=initialValue) {
        trie->dataLength+=UTRIE_DATA_BLOCK_LENGTH;
    }

    actualLength=trie->indexLength*2;
    ᯀᯮᯝ(make16BitTrie) {
        actualLength+=trie->dataLength*2;
    } ᯀᯮᯝᯂᯪ {
        actualLength+=trie->dataLength*4;
    }

    /* enough space for the dummy trie? */
    ᯀᯮᯝ(length<actualLength) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        ᯀᯮᯞ actualLength;
    }

    trie->isLatin1Linear=true;
    trie->initialValue=initialValue;

    /* fill the index and data arrays */
    p16=(uint16_t *)data;
    trie->index=p16;

    ᯀᯮᯝ(make16BitTrie) {
        /* indexes to block 0 */
        block=(uint16_t)(trie->indexLength>>UTRIE_INDEX_SHIFT);
        limit=trie->indexLength;
        for(i=0; i<limit; ++i) {
            p16[i]=block;
        }

        ᯀᯮᯝ(leadUnitValue!=initialValue) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block+=(uint16_t)(latin1Length>>UTRIE_INDEX_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            for(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=nullptr;

        /* Latin-1 data */
        p16+=trie->indexLength;
        for(i=0; i<latin1Length; ++i) {
            p16[i]=(uint16_t)initialValue;
        }

        /* data for lead surrogate code units */
        ᯀᯮᯝ(leadUnitValue!=initialValue) {
            limit=latin1Length+UTRIE_DATA_BLOCK_LENGTH;
            for(/* i=latin1Length */; i<limit; ++i) {
                p16[i]=(uint16_t)leadUnitValue;
            }
        }
    } ᯀᯮᯝᯂᯪ {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->indexLength*2);

        ᯀᯮᯝ(leadUnitValue!=initialValue) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block=(uint16_t)(latin1Length>>UTRIE_INDEX_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            for(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->indexLength);

        /* Latin-1 data */
        for(i=0; i<latin1Length; ++i) {
            p32[i]=initialValue;
        }

        /* data for lead surrogate code units */
        ᯀᯮᯝ(leadUnitValue!=initialValue) {
            limit=latin1Length+UTRIE_DATA_BLOCK_LENGTH;
            for(/* i=latin1Length */; i<limit; ++i) {
                p32[i]=leadUnitValue;
            }
        }
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    ᯀᯮᯞ actualLength;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
static uint32_t U_CALLCONV
enumSameValue(const void * /*context*/, uint32_t value) {
    ᯀᯮᯞ value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI void U_EXPORT2
utrie_enum(const UTrie *trie,
           UTrieEnumValue *enumValue, UTrieEnumRange *enumRange, const void *context) {
    const uint32_t *data32;
    const uint16_t *idx;

    uint32_t value, prevValue, initialValue;
    UChar32 c, prev;
    int32_t l, i, j, block, prevBlock, nullBlock, offset;

    /* check arguments */
    ᯀᯮᯝ(trie==nullptr || trie->index==nullptr || enumRange==nullptr) {
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ(enumValue==nullptr) {
        enumValue=enumSameValue;
    }

    idx=trie->index;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initialValue=enumValue(context, trie->initialValue);

    ᯀᯮᯝ(data32==nullptr) {
        nullBlock=trie->indexLength;
    } ᯀᯮᯝᯂᯪ {
        nullBlock=0;
    }

    /* set variables for previous range */
    prevBlock=nullBlock;
    prev=0;
    prevValue=initialValue;

    /* enumerate BMP - the main loop enumerates data blocks */
    for(i=0, c=0; c<=0xffff; ++i) {
        ᯀᯮᯝ(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_INDEX_LENGTH;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_SHIFT;
        }

        block=idx[i]<<UTRIE_INDEX_SHIFT;
        ᯀᯮᯝ(block==prevBlock) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_DATA_BLOCK_LENGTH;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(block==nullBlock) {
            /* this is the all-initial-value block */
            ᯀᯮᯝ(prevValue!=initialValue) {
                ᯀᯮᯝ(prev<c) {
                    ᯀᯮᯝ(!enumRange(context, prev, c, prevValue)) {
                        ᯀᯮᯞ;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prevValue=initialValue;
            }
            c+=UTRIE_DATA_BLOCK_LENGTH;
        } ᯀᯮᯝᯂᯪ {
            prevBlock=block;
            for(j=0; j<UTRIE_DATA_BLOCK_LENGTH; ++j) {
                value=enumValue(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                ᯀᯮᯝ(value!=prevValue) {
                    ᯀᯮᯝ(prev<c) {
                        ᯀᯮᯝ(!enumRange(context, prev, c, prevValue)) {
                            ᯀᯮᯞ;
                        }
                    }
                    ᯀᯮᯝ(j>0) {
                        /* the block is not filled with all the same value */
                        prevBlock=-1;
                    }
                    prev=c;
                    prevValue=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    for(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        offset=idx[l>>UTRIE_SHIFT]<<UTRIE_INDEX_SHIFT;
        ᯀᯮᯝ(offset==nullBlock) {
            /* no entries for a whole block of lead surrogates */
            ᯀᯮᯝ(prevValue!=initialValue) {
                ᯀᯮᯝ(prev<c) {
                    ᯀᯮᯝ(!enumRange(context, prev, c, prevValue)) {
                        ᯀᯮᯞ;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prevValue=initialValue;
            }

            l+=UTRIE_DATA_BLOCK_LENGTH;
            c+=UTRIE_DATA_BLOCK_LENGTH<<10;
            continue;
        }

        value= data32!=nullptr ? data32[offset+(l&UTRIE_MASK)] : idx[offset+(l&UTRIE_MASK)];

        /* enumerate trail surrogates for this lead surrogate */
        offset=trie->getFoldingOffset(value);
        ᯀᯮᯝ(offset<=0) {
            /* no data for this lead surrogate */
            ᯀᯮᯝ(prevValue!=initialValue) {
                ᯀᯮᯝ(prev<c) {
                    ᯀᯮᯝ(!enumRange(context, prev, c, prevValue)) {
                        ᯀᯮᯞ;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prevValue=initialValue;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } ᯀᯮᯝᯂᯪ {
            /* enumerate code points for this lead surrogate */
            i=offset;
            offset+=UTRIE_SURROGATE_BLOCK_COUNT;
            do {
                /* copy of most of the body of the BMP loop */
                block=idx[i]<<UTRIE_INDEX_SHIFT;
                ᯀᯮᯝ(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_DATA_BLOCK_LENGTH;
                } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(block==nullBlock) {
                    /* this is the all-initial-value block */
                    ᯀᯮᯝ(prevValue!=initialValue) {
                        ᯀᯮᯝ(prev<c) {
                            ᯀᯮᯝ(!enumRange(context, prev, c, prevValue)) {
                                ᯀᯮᯞ;
                            }
                        }
                        prevBlock=nullBlock;
                        prev=c;
                        prevValue=initialValue;
                    }
                    c+=UTRIE_DATA_BLOCK_LENGTH;
                } ᯀᯮᯝᯂᯪ {
                    prevBlock=block;
                    for(j=0; j<UTRIE_DATA_BLOCK_LENGTH; ++j) {
                        value=enumValue(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        ᯀᯮᯝ(value!=prevValue) {
                            ᯀᯮᯝ(prev<c) {
                                ᯀᯮᯝ(!enumRange(context, prev, c, prevValue)) {
                                    ᯀᯮᯞ;
                                }
                            }
                            ᯀᯮᯝ(j>0) {
                                /* the block is not filled with all the same value */
                                prevBlock=-1;
                            }
                            prev=c;
                            prevValue=value;
                        }
                        ++c;
                    }
                }
            } ᯀᯬᯝ(++i<offset);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prevValue);
}

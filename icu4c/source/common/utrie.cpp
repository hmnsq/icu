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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#undef ABS
#define ABS(x) ((x)>=0 ? (x) : -(x))

স্থির অন্তঃস্থ UBool
equal_uint32(ধ্রুৱ uint32_t *s, ধ্রুৱ uint32_t *t, int32_t length) {
    যেতিয়ালৈ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ঘূৰাই length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fillIn,
           uint32_t *aliasData, int32_t maxDataLength,
           uint32_t initial_মান, uint32_t lead_Unit_মান,
           UBool latin1Linear) {
    UNewTrie *trie;
    int32_t i, j;

    যদি( maxDataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (latin1Linear && maxDataLength<1024)
    ) {
        ঘূৰাই nullptr;
    }

    যদি(fillIn!=nullptr) {
        trie=fillIn;
    } অন্যথা {
        trie=(UNewTrie *)uprv_malloc(পরিমাণ(UNewTrie));
        যদি(trie==nullptr) {
            ঘূৰাই nullptr;
        }
    }
    uprv_memset(trie, 0, পরিমাণ(UNewTrie));
    trie->isAllocated = fillIn == nullptr;

    যদি(aliasData!=nullptr) {
        trie->data=aliasData;
        trie->isDataAllocated=মিছা;
    } অন্যথা {
        trie->data=(uint32_t *)uprv_malloc(maxDataLength*4);
        যদি(trie->data==nullptr) {
            uprv_free(trie);
            ঘূৰাই nullptr;
        }
        trie->isDataAllocated=সঁচা;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_DATA_BLOCK_LENGTH;

    যদি(latin1Linear) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        কৰা {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->index[i++]=j;
            j+=UTRIE_DATA_BLOCK_LENGTH;
        } যেতিয়ালৈ(i<(256>>UTRIE_SHIFT));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->dataLength=j;
    যেতিয়ালৈ(j>0) {
        trie->data[--j]=initial_মান;
    }

    trie->lead_Unit_মান=lead_Unit_মান;
    trie->indexLength=UTRIE_MAX_INDEX_LENGTH;
    trie->dataCapacity=maxDataLength;
    trie->isLatin1Linear=latin1Linear;
    trie->isCompacted=মিছা;
    ঘূৰাই trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fillIn, ধ্রুৱ UNewTrie *other, uint32_t *aliasData, int32_t aliasDataCapacity) {
    UNewTrie *trie;
    UBool isDataAllocated;

    /* do not clone if other is not valid or already compacted */
    যদি(other==nullptr || other->data==nullptr || other->isCompacted) {
        ঘূৰাই nullptr;
    }

    /* clone data */
    যদি(aliasData!=nullptr && aliasDataCapacity>=other->dataCapacity) {
        isDataAllocated=মিছা;
    } অন্যথা {
        aliasDataCapacity=other->dataCapacity;
        aliasData=(uint32_t *)uprv_malloc(other->dataCapacity*4);
        যদি(aliasData==nullptr) {
            ঘূৰাই nullptr;
        }
        isDataAllocated=সঁচা;
    }

    trie=utrie_open(fillIn, aliasData, aliasDataCapacity,
                    other->data[0], other->lead_Unit_মান,
                    other->isLatin1Linear);
    যদি(trie==nullptr) {
        uprv_free(aliasData);
    } অন্যথা {
        uprv_memcpy(trie->index, other->index, পরিমাণ(trie->index));
        uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
        trie->dataLength=other->dataLength;
        trie->isDataAllocated=isDataAllocated;
    }

    ঘূৰাই trie;
}

U_CAPI শূন্য U_EXPORT2
utrie_close(UNewTrie *trie) {
    যদি(trie!=nullptr) {
        যদি(trie->isDataAllocated) {
            uprv_free(trie->data);
            trie->data=nullptr;
        }
        যদি(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_getData(UNewTrie *trie, int32_t *pLength) {
    যদি(trie==nullptr || pLength==nullptr) {
        ঘূৰাই nullptr;
    }

    *pLength=trie->dataLength;
    ঘূৰাই trie->data;
}

স্থির int32_t
utrie_allocDataBlock(UNewTrie *trie) {
    int32_t newBlock, new_শীৰ্ষ;

    newBlock=trie->dataLength;
    new_শীৰ্ষ=newBlock+UTRIE_DATA_BLOCK_LENGTH;
    যদি(new_শীৰ্ষ>trie->dataCapacity) {
        /* out of memory in the data array */
        ঘূৰাই -1;
    }
    trie->dataLength=new_শীৰ্ষ;
    ঘূৰাই newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
স্থির int32_t
utrie_getDataBlock(UNewTrie *trie, UChar32 c) {
    int32_t index_মান, newBlock;

    c>>=UTRIE_SHIFT;
    index_মান=trie->index[c];
    যদি(index_মান>0) {
        ঘূৰাই index_মান;
    }

    /* allocate a new data block */
    newBlock=utrie_allocDataBlock(trie);
    যদি(newBlock<0) {
        /* out of memory in the data array */
        ঘূৰাই -1;
    }
    trie->index[c]=newBlock;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->data+newBlock, trie->data-index_মান, 4*UTRIE_DATA_BLOCK_LENGTH);
    ঘূৰাই newBlock;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    যদি(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        ঘূৰাই মিছা;
    }

    block=utrie_getDataBlock(trie, c);
    যদি(block<0) {
        ঘূৰাই মিছা;
    }

    trie->data[block+(c&UTRIE_MASK)]=value;
    ঘূৰাই সঁচা;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *pInBlockZero) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    যদি(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        যদি(pInBlockZero!=nullptr) {
            *pInBlockZero=সঁচা;
        }
        ঘূৰাই 0;
    }

    block=trie->index[c>>UTRIE_SHIFT];
    যদি(pInBlockZero!=nullptr) {
        *pInBlockZero = block == 0;
    }

    ঘূৰাই trie->data[ABS(block)+(c&UTRIE_MASK)];
}

/**
 * @internal
 */
স্থির শূন্য
utrie_fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
                uint32_t value, uint32_t initial_মান, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    যদি(overwrite) {
        যেতিয়ালৈ(block<pLimit) {
            *block++=value;
        }
    } অন্যথা {
        যেতিয়ালৈ(block<pLimit) {
            যদি(*block==initial_মান) {
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
    uint32_t initial_মান;
    int32_t block, rest, repeatBlock;

    /* valid, uncompacted trie and valid indexes? */
    যদি( trie==nullptr || trie->isCompacted ||
        (uint32_t)start>0x10ffff || (uint32_t)limit>0x110000 || start>limit
    ) {
        ঘূৰাই মিছা;
    }
    যদি(start==limit) {
        ঘূৰাই সঁচা; /* nothing to do */
    }

    initial_মান=trie->data[0];
    যদি(start&UTRIE_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=utrie_getDataBlock(trie, start);
        যদি(block<0) {
            ঘূৰাই মিছা;
        }

        nextStart=(start+UTRIE_DATA_BLOCK_LENGTH)&~UTRIE_MASK;
        যদি(nextStart<=limit) {
            utrie_fillBlock(trie->data+block, start&UTRIE_MASK, UTRIE_DATA_BLOCK_LENGTH,
                            value, initial_মান, overwrite);
            start=nextStart;
        } অন্যথা {
            utrie_fillBlock(trie->data+block, start&UTRIE_MASK, limit&UTRIE_MASK,
                            value, initial_মান, overwrite);
            ঘূৰাই সঁচা;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE_MASK;

    /* iterate over all-value blocks */
    যদি(value==initial_মান) {
        repeatBlock=0;
    } অন্যথা {
        repeatBlock=-1;
    }
    যেতিয়ালৈ(start<limit) {
        /* get index value */
        block=trie->index[start>>UTRIE_SHIFT];
        যদি(block>0) {
            /* already allocated, fill in value */
            utrie_fillBlock(trie->data+block, 0, UTRIE_DATA_BLOCK_LENGTH, value, initial_মান, overwrite);
        } অন্যথা যদি(trie->data[-block]!=value && (block==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            যদি(repeatBlock>=0) {
                trie->index[start>>UTRIE_SHIFT]=-repeatBlock;
            } অন্যথা {
                /* create and set and fill the repeatBlock */
                repeatBlock=utrie_getDataBlock(trie, start);
                যদি(repeatBlock<0) {
                    ঘূৰাই মিছা;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->index[start>>UTRIE_SHIFT]=-repeatBlock;
                utrie_fillBlock(trie->data+repeatBlock, 0, UTRIE_DATA_BLOCK_LENGTH, value, initial_মান, সঁচা);
            }
        }

        start+=UTRIE_DATA_BLOCK_LENGTH;
    }

    যদি(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=utrie_getDataBlock(trie, start);
        যদি(block<0) {
            ঘূৰাই মিছা;
        }

        utrie_fillBlock(trie->data+block, 0, rest, value, initial_মান, overwrite);
    }

    ঘূৰাই সঁচা;
}

স্থির int32_t
_findSameIndexBlock(ধ্রুৱ int32_t *idx, int32_t indexLength,
                    int32_t otherBlock) {
    int32_t block, i;

    বাবে(block=UTRIE_BMP_INDEX_LENGTH; block<indexLength; block+=UTRIE_SURROGATE_BLOCK_গুনতি) {
        বাবে(i=0; i<UTRIE_SURROGATE_BLOCK_গুনতি; ++i) {
            যদি(idx[block+i]!=idx[otherBlock+i]) {
                ভঙ্গ;
            }
        }
        যদি(i==UTRIE_SURROGATE_BLOCK_গুনতি) {
            ঘূৰাই block;
        }
    }
    ঘূৰাই indexLength;
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
স্থির শূন্য
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_মান *get_Folded_মান, UErrorCode *pErrorCode) {
    int32_t leadIndexes[UTRIE_SURROGATE_BLOCK_গুনতি];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t indexLength, block;
#ifdef UTRIE_DEBUG
    পূর্ণাঙ্ক গুনতি_Lead_CUWith_Data=0;
#endif

    idx=trie->index;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_SHIFT), 4*UTRIE_SURROGATE_BLOCK_গুনতি);

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
    যদি(trie->lead_Unit_মান==trie->data[0]) {
        block=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } অন্যথা {
        /* create and fill the repeatBlock */
        block=utrie_allocDataBlock(trie);
        যদি(block<0) {
            /* data table overflow */
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই;
        }
        utrie_fillBlock(trie->data+block, 0, UTRIE_DATA_BLOCK_LENGTH, trie->lead_Unit_মান, trie->data[0], সঁচা);
        block=-block; /* negative block number to indicate that it is a repeat block */
    }
    বাবে(c=(0xd800>>UTRIE_SHIFT); c<(0xdc00>>UTRIE_SHIFT); ++c) {
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
    বাবে(c=0x10000; c<0x110000;) {
        যদি(idx[c>>UTRIE_SHIFT]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#ifdef UTRIE_DEBUG
            ++গুনতি_Lead_CUWith_Data;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#endif

            /* is there an identical index block? */
            block=_findSameIndexBlock(idx, indexLength, c>>UTRIE_SHIFT);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_মান(trie, c, block+UTRIE_SURROGATE_BLOCK_গুনতি);
            যদি(value!=utrie_get32(trie, U16_LEAD(c), nullptr)) {
                যদি(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    ঘূৰাই;
                }

                /* if we did not find an identical index block... */
                যদি(block==indexLength) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+indexLength,
                                 idx+(c>>UTRIE_SHIFT),
                                 4*UTRIE_SURROGATE_BLOCK_গুনতি);
                    indexLength+=UTRIE_SURROGATE_BLOCK_গুনতি;
                }
            }
            c+=0x400;
        } অন্যথা {
            c+=UTRIE_DATA_BLOCK_LENGTH;
        }
    }
#ifdef UTRIE_DEBUG
    যদি(গুনতি_Lead_CUWith_Data>0) {
        printf("supplementary data for %d lead surrogates\n", গুনতি_Lead_CUWith_Data);
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
    যদি(indexLength>=UTRIE_MAX_INDEX_LENGTH) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ঘূৰাই;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_INDEX_LENGTH+UTRIE_SURROGATE_BLOCK_গুনতি,
                 idx+UTRIE_BMP_INDEX_LENGTH,
                 4*(indexLength-UTRIE_BMP_INDEX_LENGTH));
    uprv_memcpy(idx+UTRIE_BMP_INDEX_LENGTH,
                leadIndexes,
                4*UTRIE_SURROGATE_BLOCK_গুনতি);
    indexLength+=UTRIE_SURROGATE_BLOCK_গুনতি;

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
স্থির শূন্য
_findUnusedBlocks(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_TIME_DATA_LENGTH>>UTRIE_SHIFT)*4);

    /* mark each block that _is_ used with 0 */
    বাবে(i=0; i<trie->indexLength; ++i) {
        trie->map[ABS(trie->index[i])>>UTRIE_SHIFT]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

স্থির int32_t
_findSameDataBlock(ধ্রুৱ uint32_t *data, int32_t dataLength,
                   int32_t otherBlock, int32_t step) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=UTRIE_DATA_BLOCK_LENGTH;

    বাবে(block=0; block<=dataLength; block+=step) {
        যদি(equal_uint32(data+block, data+otherBlock, UTRIE_DATA_BLOCK_LENGTH)) {
            ঘূৰাই block;
        }
    }
    ঘূৰাই -1;
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
স্থির শূন্য
utrie_compact(UNewTrie *trie, UBool overlap, UErrorCode *pErrorCode) {
    int32_t i, start, newStart, overlapStart;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই;
    }

    /* valid, uncompacted trie? */
    যদি(trie==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
    }
    যদি(trie->isCompacted) {
        ঘূৰাই; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    _findUnusedBlocks(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    যদি(trie->isLatin1Linear && UTRIE_SHIFT<=8) {
        overlapStart=UTRIE_DATA_BLOCK_LENGTH+256;
    } অন্যথা {
        overlapStart=UTRIE_DATA_BLOCK_LENGTH;
    }

    newStart=UTRIE_DATA_BLOCK_LENGTH;
    বাবে(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        যদি(trie->map[start>>UTRIE_SHIFT]<0) {
            /* advance start to the next block */
            start+=UTRIE_DATA_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            অনুৱর্ত;
        }

        /* search for an identical block */
        যদি( start>=overlapStart &&
            (i=_findSameDataBlock(trie->data, newStart, start,
                            overlap ? UTRIE_DATA_GRANULARITY : UTRIE_DATA_BLOCK_LENGTH))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE_SHIFT]=i;

            /* advance start to the next block */
            start+=UTRIE_DATA_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            অনুৱর্ত;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        যদি(overlap && start>=overlapStart) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            বাবে(i=UTRIE_DATA_BLOCK_LENGTH-UTRIE_DATA_GRANULARITY;
                i>0 && !equal_uint32(trie->data+(newStart-i), trie->data+start, i);
                i-=UTRIE_DATA_GRANULARITY) {}
        } অন্যথা {
            i=0;
        }

        যদি(i>0) {
            /* some overlap */
            trie->map[start>>UTRIE_SHIFT]=newStart-i;

            /* move the non-overlapping indexes to their new positions */
            start+=i;
            বাবে(i=UTRIE_DATA_BLOCK_LENGTH-i; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } অন্যথা যদি(newStart<start) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[start>>UTRIE_SHIFT]=newStart;
            বাবে(i=UTRIE_DATA_BLOCK_LENGTH; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } অন্যথা /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE_SHIFT]=start;
            newStart+=UTRIE_DATA_BLOCK_LENGTH;
            start=newStart;
        }
    }

    /* now adjust the index (stage 1) table */
    বাবে(i=0; i<trie->indexLength; ++i) {
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
স্থির uint32_t U_CALLCONV
default_Get_Folded_মান(UNewTrie *trie, UChar32 start, int32_t offset) {
    uint32_t value, initial_মান;
    UChar32 limit;
    UBool inBlockZero;

    initial_মান=trie->data[0];
    limit=start+0x400;
    যেতিয়ালৈ(start<limit) {
        value=utrie_get32(trie, start, &inBlockZero);
        যদি(inBlockZero) {
            start+=UTRIE_DATA_BLOCK_LENGTH;
        } অন্যথা যদি(value!=initial_মান) {
            ঘূৰাই static_cast<uint32_t>(offset);
        } অন্যথা {
            ++start;
        }
    }
    ঘূৰাই 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, শূন্য *dt, int32_t capacity,
                UNew_Trie_Get_Folded_মান *get_Folded_মান,
                UBool reduceTo16Bits,
                UErrorCode *pErrorCode) {
    UTrieHeader *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* data = nullptr;

    /* argument check */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই 0;
    }

    যদি(trie==nullptr || capacity<0 || (capacity>0 && dt==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }
    যদি(get_Folded_মান==nullptr) {
        get_Folded_মান=default_Get_Folded_মান;
    }

    data = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    যদি(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, মিছা, pErrorCode);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_মান, pErrorCode);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, সঁচা, pErrorCode);

        trie->isCompacted=সঁচা;
        যদি(U_FAILURE(*pErrorCode)) {
            ঘূৰাই 0;
        }
    }

    /* is dataLength within limits? */
    যদি( (reduceTo16Bits ? (trie->dataLength+trie->indexLength) : trie->dataLength) >= UTRIE_MAX_DATA_LENGTH) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    }

    length=পরিমাণ(UTrieHeader)+2*trie->indexLength;
    যদি(reduceTo16Bits) {
        length+=2*trie->dataLength;
    } অন্যথা {
        length+=4*trie->dataLength;
    }

    যদি(length>capacity) {
        ঘূৰাই length; /* preflighting */
    }

#ifdef UTRIE_DEBUG
    printf("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (long)trie->indexLength, (long)trie->dataLength, (long)length);
#endif

    /* set the header fields */
    header=(UTrieHeader *)data;
    data+=পরিমাণ(UTrieHeader);

    header->signature=0x54726965; /* "Trie" */
    header->options=UTRIE_SHIFT | (UTRIE_INDEX_SHIFT<<UTRIE_OPTIONS_INDEX_SHIFT);

    যদি(!reduceTo16Bits) {
        header->options|=UTRIE_OPTIONS_DATA_IS_32_BIT;
    }
    যদি(trie->isLatin1Linear) {
        header->options|=UTRIE_OPTIONS_LATIN1_IS_LINEAR;
    }

    header->indexLength=trie->indexLength;
    header->dataLength=trie->dataLength;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    যদি(reduceTo16Bits) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->index;
        dest16=(uint16_t *)data;
        বাবে(i=trie->indexLength; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->indexLength)>>UTRIE_INDEX_SHIFT);
        }

        /* write 16-bit data values */
        p=trie->data;
        বাবে(i=trie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } অন্যথা {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->index;
        dest16=(uint16_t *)data;
        বাবে(i=trie->indexLength; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_INDEX_SHIFT);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->data, 4*(size_t)trie->dataLength);
    }

    ঘূৰাই length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_defaultGetFoldingOffset(uint32_t data) {
    ঘূৰাই (int32_t)data;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ধ্রুৱ শূন্য *data, int32_t length, UErrorCode *pErrorCode) {
    ধ্রুৱ UTrieHeader *header;
    ধ্রুৱ uint16_t *p16;
    uint32_t options;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই -1;
    }

    /* enough data for a trie header? */
    যদি(length<(int32_t)পরিমাণ(UTrieHeader)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ঘূৰাই -1;
    }

    /* check the signature */
    header=(ধ্রুৱ UTrieHeader *)data;
    যদি(header->signature!=0x54726965) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ঘূৰাই -1;
    }

    /* get the options and check the shift values */
    options=header->options;
    যদি( (options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ঘূৰাই -1;
    }
    trie->isLatin1Linear = (options & UTRIE_OPTIONS_LATIN1_IS_LINEAR) != 0;

    /* get the length values */
    trie->indexLength=header->indexLength;
    trie->dataLength=header->dataLength;

    length-=(int32_t)পরিমাণ(UTrieHeader);

    /* enough data for the index? */
    যদি(length<2*trie->indexLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ঘূৰাই -1;
    }
    p16=(ধ্রুৱ uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;
    length-=2*trie->indexLength;

    /* get the data */
    যদি(options&UTRIE_OPTIONS_DATA_IS_32_BIT) {
        যদি(length<4*trie->dataLength) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ঘূৰাই -1;
        }
        trie->data32=(ধ্রুৱ uint32_t *)p16;
        trie->initial_মান=trie->data32[0];
        length=(int32_t)পরিমাণ(UTrieHeader)+2*trie->indexLength+4*trie->dataLength;
    } অন্যথা {
        যদি(length<2*trie->dataLength) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ঘূৰাই -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=nullptr;
        trie->initial_মান=trie->index[trie->indexLength];
        length=(int32_t)পরিমাণ(UTrieHeader)+2*trie->indexLength+2*trie->dataLength;
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    ঘূৰাই length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       শূন্য *data, int32_t length,
                       uint32_t initial_মান, uint32_t lead_Unit_মান,
                       UBool make16BitTrie,
                       UErrorCode *pErrorCode) {
    uint16_t *p16;
    int32_t actualLength, latin1Length, i, limit;
    uint16_t block;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1Length= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->indexLength=UTRIE_BMP_INDEX_LENGTH+UTRIE_SURROGATE_BLOCK_গুনতি;
    trie->dataLength=latin1Length;
    যদি(lead_Unit_মান!=initial_মান) {
        trie->dataLength+=UTRIE_DATA_BLOCK_LENGTH;
    }

    actualLength=trie->indexLength*2;
    যদি(make16BitTrie) {
        actualLength+=trie->dataLength*2;
    } অন্যথা {
        actualLength+=trie->dataLength*4;
    }

    /* enough space for the dummy trie? */
    যদি(length<actualLength) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        ঘূৰাই actualLength;
    }

    trie->isLatin1Linear=সঁচা;
    trie->initial_মান=initial_মান;

    /* fill the index and data arrays */
    p16=(uint16_t *)data;
    trie->index=p16;

    যদি(make16BitTrie) {
        /* indexes to block 0 */
        block=(uint16_t)(trie->indexLength>>UTRIE_INDEX_SHIFT);
        limit=trie->indexLength;
        বাবে(i=0; i<limit; ++i) {
            p16[i]=block;
        }

        যদি(lead_Unit_মান!=initial_মান) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block+=(uint16_t)(latin1Length>>UTRIE_INDEX_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            বাবে(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=nullptr;

        /* Latin-1 data */
        p16+=trie->indexLength;
        বাবে(i=0; i<latin1Length; ++i) {
            p16[i]=(uint16_t)initial_মান;
        }

        /* data for lead surrogate code units */
        যদি(lead_Unit_মান!=initial_মান) {
            limit=latin1Length+UTRIE_DATA_BLOCK_LENGTH;
            বাবে(/* i=latin1Length */; i<limit; ++i) {
                p16[i]=(uint16_t)lead_Unit_মান;
            }
        }
    } অন্যথা {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->indexLength*2);

        যদি(lead_Unit_মান!=initial_মান) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block=(uint16_t)(latin1Length>>UTRIE_INDEX_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            বাবে(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->indexLength);

        /* Latin-1 data */
        বাবে(i=0; i<latin1Length; ++i) {
            p32[i]=initial_মান;
        }

        /* data for lead surrogate code units */
        যদি(lead_Unit_মান!=initial_মান) {
            limit=latin1Length+UTRIE_DATA_BLOCK_LENGTH;
            বাবে(/* i=latin1Length */; i<limit; ++i) {
                p32[i]=lead_Unit_মান;
            }
        }
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    ঘূৰাই actualLength;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
স্থির uint32_t U_CALLCONV
enum_Same_মান(ধ্রুৱ শূন্য * /*context*/, uint32_t value) {
    ঘূৰাই value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI শূন্য U_EXPORT2
utrie_enum(ধ্রুৱ UTrie *trie,
           UTrie_Enum_মান *enum_মান, UTrieEnumRange *enumRange, ধ্রুৱ শূন্য *context) {
    ধ্রুৱ uint32_t *data32;
    ধ্রুৱ uint16_t *idx;

    uint32_t value, prev_মান, initial_মান;
    UChar32 c, prev;
    int32_t l, i, j, block, prevBlock, nullBlock, offset;

    /* check arguments */
    যদি(trie==nullptr || trie->index==nullptr || enumRange==nullptr) {
        ঘূৰাই;
    }
    যদি(enum_মান==nullptr) {
        enum_মান=enum_Same_মান;
    }

    idx=trie->index;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_মান=enum_মান(context, trie->initial_মান);

    যদি(data32==nullptr) {
        nullBlock=trie->indexLength;
    } অন্যথা {
        nullBlock=0;
    }

    /* set variables for previous range */
    prevBlock=nullBlock;
    prev=0;
    prev_মান=initial_মান;

    /* enumerate BMP - the main loop enumerates data blocks */
    বাবে(i=0, c=0; c<=0xffff; ++i) {
        যদি(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_INDEX_LENGTH;
        } অন্যথা যদি(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_SHIFT;
        }

        block=idx[i]<<UTRIE_INDEX_SHIFT;
        যদি(block==prevBlock) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_DATA_BLOCK_LENGTH;
        } অন্যথা যদি(block==nullBlock) {
            /* this is the all-initial-value block */
            যদি(prev_মান!=initial_মান) {
                যদি(prev<c) {
                    যদি(!enumRange(context, prev, c, prev_মান)) {
                        ঘূৰাই;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prev_মান=initial_মান;
            }
            c+=UTRIE_DATA_BLOCK_LENGTH;
        } অন্যথা {
            prevBlock=block;
            বাবে(j=0; j<UTRIE_DATA_BLOCK_LENGTH; ++j) {
                value=enum_মান(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                যদি(value!=prev_মান) {
                    যদি(prev<c) {
                        যদি(!enumRange(context, prev, c, prev_মান)) {
                            ঘূৰাই;
                        }
                    }
                    যদি(j>0) {
                        /* the block is not filled with all the same value */
                        prevBlock=-1;
                    }
                    prev=c;
                    prev_মান=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    বাবে(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        offset=idx[l>>UTRIE_SHIFT]<<UTRIE_INDEX_SHIFT;
        যদি(offset==nullBlock) {
            /* no entries for a whole block of lead surrogates */
            যদি(prev_মান!=initial_মান) {
                যদি(prev<c) {
                    যদি(!enumRange(context, prev, c, prev_মান)) {
                        ঘূৰাই;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prev_মান=initial_মান;
            }

            l+=UTRIE_DATA_BLOCK_LENGTH;
            c+=UTRIE_DATA_BLOCK_LENGTH<<10;
            অনুৱর্ত;
        }

        value= data32!=nullptr ? data32[offset+(l&UTRIE_MASK)] : idx[offset+(l&UTRIE_MASK)];

        /* enumerate trail surrogates for this lead surrogate */
        offset=trie->getFoldingOffset(value);
        যদি(offset<=0) {
            /* no data for this lead surrogate */
            যদি(prev_মান!=initial_মান) {
                যদি(prev<c) {
                    যদি(!enumRange(context, prev, c, prev_মান)) {
                        ঘূৰাই;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prev_মান=initial_মান;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } অন্যথা {
            /* enumerate code points for this lead surrogate */
            i=offset;
            offset+=UTRIE_SURROGATE_BLOCK_গুনতি;
            কৰা {
                /* copy of most of the body of the BMP loop */
                block=idx[i]<<UTRIE_INDEX_SHIFT;
                যদি(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_DATA_BLOCK_LENGTH;
                } অন্যথা যদি(block==nullBlock) {
                    /* this is the all-initial-value block */
                    যদি(prev_মান!=initial_মান) {
                        যদি(prev<c) {
                            যদি(!enumRange(context, prev, c, prev_মান)) {
                                ঘূৰাই;
                            }
                        }
                        prevBlock=nullBlock;
                        prev=c;
                        prev_মান=initial_মান;
                    }
                    c+=UTRIE_DATA_BLOCK_LENGTH;
                } অন্যথা {
                    prevBlock=block;
                    বাবে(j=0; j<UTRIE_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_মান(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        যদি(value!=prev_মান) {
                            যদি(prev<c) {
                                যদি(!enumRange(context, prev, c, prev_মান)) {
                                    ঘূৰাই;
                                }
                            }
                            যদি(j>0) {
                                /* the block is not filled with all the same value */
                                prevBlock=-1;
                            }
                            prev=c;
                            prev_মান=value;
                        }
                        ++c;
                    }
                }
            } যেতিয়ালৈ(++i<offset);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_মান);
}

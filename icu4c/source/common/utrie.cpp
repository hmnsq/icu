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

#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#undef ABS
#define ABS(x) ((x)>=0 ? (x) : -(x))

ایستا درونخطی UBool
equal_uint32(ثابت uint32_t *s, ثابت uint32_t *t, int32_t length) {
    وقتی(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    بازگشت length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fillIn,
           uint32_t *aliasData, int32_t maxDataLength,
           uint32_t initial_مقدار, uint32_t lead_Unit_مقدار,
           UBool latin1Linear) {
    UNewTrie *trie;
    int32_t i, j;

    اگر( maxDataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (latin1Linear && maxDataLength<1024)
    ) {
        بازگشت nullptr;
    }

    اگر(fillIn!=nullptr) {
        trie=fillIn;
    } وگرنه {
        trie=(UNewTrie *)uprv_malloc(اندازه(UNewTrie));
        اگر(trie==nullptr) {
            بازگشت nullptr;
        }
    }
    uprv_memset(trie, 0, اندازه(UNewTrie));
    trie->isAllocated = fillIn == nullptr;

    اگر(aliasData!=nullptr) {
        trie->data=aliasData;
        trie->isDataAllocated=نادرست;
    } وگرنه {
        trie->data=(uint32_t *)uprv_malloc(maxDataLength*4);
        اگر(trie->data==nullptr) {
            uprv_free(trie);
            بازگشت nullptr;
        }
        trie->isDataAllocated=درست;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_DATA_BLOCK_LENGTH;

    اگر(latin1Linear) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        انجامبده {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->index[i++]=j;
            j+=UTRIE_DATA_BLOCK_LENGTH;
        } وقتی(i<(256>>UTRIE_SHIFT));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->dataLength=j;
    وقتی(j>0) {
        trie->data[--j]=initial_مقدار;
    }

    trie->lead_Unit_مقدار=lead_Unit_مقدار;
    trie->indexLength=UTRIE_MAX_INDEX_LENGTH;
    trie->dataCapacity=maxDataLength;
    trie->isLatin1Linear=latin1Linear;
    trie->isCompacted=نادرست;
    بازگشت trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fillIn, ثابت UNewTrie *other, uint32_t *aliasData, int32_t aliasDataCapacity) {
    UNewTrie *trie;
    UBool isDataAllocated;

    /* do not clone if other is not valid or already compacted */
    اگر(other==nullptr || other->data==nullptr || other->isCompacted) {
        بازگشت nullptr;
    }

    /* clone data */
    اگر(aliasData!=nullptr && aliasDataCapacity>=other->dataCapacity) {
        isDataAllocated=نادرست;
    } وگرنه {
        aliasDataCapacity=other->dataCapacity;
        aliasData=(uint32_t *)uprv_malloc(other->dataCapacity*4);
        اگر(aliasData==nullptr) {
            بازگشت nullptr;
        }
        isDataAllocated=درست;
    }

    trie=utrie_open(fillIn, aliasData, aliasDataCapacity,
                    other->data[0], other->lead_Unit_مقدار,
                    other->isLatin1Linear);
    اگر(trie==nullptr) {
        uprv_free(aliasData);
    } وگرنه {
        uprv_memcpy(trie->index, other->index, اندازه(trie->index));
        uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
        trie->dataLength=other->dataLength;
        trie->isDataAllocated=isDataAllocated;
    }

    بازگشت trie;
}

U_CAPI تهی U_EXPORT2
utrie_close(UNewTrie *trie) {
    اگر(trie!=nullptr) {
        اگر(trie->isDataAllocated) {
            uprv_free(trie->data);
            trie->data=nullptr;
        }
        اگر(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_getData(UNewTrie *trie, int32_t *pLength) {
    اگر(trie==nullptr || pLength==nullptr) {
        بازگشت nullptr;
    }

    *pLength=trie->dataLength;
    بازگشت trie->data;
}

ایستا int32_t
utrie_allocDataBlock(UNewTrie *trie) {
    int32_t newBlock, new_بالاترین;

    newBlock=trie->dataLength;
    new_بالاترین=newBlock+UTRIE_DATA_BLOCK_LENGTH;
    اگر(new_بالاترین>trie->dataCapacity) {
        /* out of memory in the data array */
        بازگشت -1;
    }
    trie->dataLength=new_بالاترین;
    بازگشت newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ایستا int32_t
utrie_getDataBlock(UNewTrie *trie, UChar32 c) {
    int32_t index_مقدار, newBlock;

    c>>=UTRIE_SHIFT;
    index_مقدار=trie->index[c];
    اگر(index_مقدار>0) {
        بازگشت index_مقدار;
    }

    /* allocate a new data block */
    newBlock=utrie_allocDataBlock(trie);
    اگر(newBlock<0) {
        /* out of memory in the data array */
        بازگشت -1;
    }
    trie->index[c]=newBlock;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->data+newBlock, trie->data-index_مقدار, 4*UTRIE_DATA_BLOCK_LENGTH);
    بازگشت newBlock;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    اگر(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        بازگشت نادرست;
    }

    block=utrie_getDataBlock(trie, c);
    اگر(block<0) {
        بازگشت نادرست;
    }

    trie->data[block+(c&UTRIE_MASK)]=value;
    بازگشت درست;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *pInBlockZero) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    اگر(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        اگر(pInBlockZero!=nullptr) {
            *pInBlockZero=درست;
        }
        بازگشت 0;
    }

    block=trie->index[c>>UTRIE_SHIFT];
    اگر(pInBlockZero!=nullptr) {
        *pInBlockZero = block == 0;
    }

    بازگشت trie->data[ABS(block)+(c&UTRIE_MASK)];
}

/**
 * @internal
 */
ایستا تهی
utrie_fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
                uint32_t value, uint32_t initial_مقدار, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    اگر(overwrite) {
        وقتی(block<pLimit) {
            *block++=value;
        }
    } وگرنه {
        وقتی(block<pLimit) {
            اگر(*block==initial_مقدار) {
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
    uint32_t initial_مقدار;
    int32_t block, rest, repeatBlock;

    /* valid, uncompacted trie and valid indexes? */
    اگر( trie==nullptr || trie->isCompacted ||
        (uint32_t)start>0x10ffff || (uint32_t)limit>0x110000 || start>limit
    ) {
        بازگشت نادرست;
    }
    اگر(start==limit) {
        بازگشت درست; /* nothing to do */
    }

    initial_مقدار=trie->data[0];
    اگر(start&UTRIE_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=utrie_getDataBlock(trie, start);
        اگر(block<0) {
            بازگشت نادرست;
        }

        nextStart=(start+UTRIE_DATA_BLOCK_LENGTH)&~UTRIE_MASK;
        اگر(nextStart<=limit) {
            utrie_fillBlock(trie->data+block, start&UTRIE_MASK, UTRIE_DATA_BLOCK_LENGTH,
                            value, initial_مقدار, overwrite);
            start=nextStart;
        } وگرنه {
            utrie_fillBlock(trie->data+block, start&UTRIE_MASK, limit&UTRIE_MASK,
                            value, initial_مقدار, overwrite);
            بازگشت درست;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE_MASK;

    /* iterate over all-value blocks */
    اگر(value==initial_مقدار) {
        repeatBlock=0;
    } وگرنه {
        repeatBlock=-1;
    }
    وقتی(start<limit) {
        /* get index value */
        block=trie->index[start>>UTRIE_SHIFT];
        اگر(block>0) {
            /* already allocated, fill in value */
            utrie_fillBlock(trie->data+block, 0, UTRIE_DATA_BLOCK_LENGTH, value, initial_مقدار, overwrite);
        } وگرنه اگر(trie->data[-block]!=value && (block==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            اگر(repeatBlock>=0) {
                trie->index[start>>UTRIE_SHIFT]=-repeatBlock;
            } وگرنه {
                /* create and set and fill the repeatBlock */
                repeatBlock=utrie_getDataBlock(trie, start);
                اگر(repeatBlock<0) {
                    بازگشت نادرست;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->index[start>>UTRIE_SHIFT]=-repeatBlock;
                utrie_fillBlock(trie->data+repeatBlock, 0, UTRIE_DATA_BLOCK_LENGTH, value, initial_مقدار, درست);
            }
        }

        start+=UTRIE_DATA_BLOCK_LENGTH;
    }

    اگر(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=utrie_getDataBlock(trie, start);
        اگر(block<0) {
            بازگشت نادرست;
        }

        utrie_fillBlock(trie->data+block, 0, rest, value, initial_مقدار, overwrite);
    }

    بازگشت درست;
}

ایستا int32_t
_findSameIndexBlock(ثابت int32_t *idx, int32_t indexLength,
                    int32_t otherBlock) {
    int32_t block, i;

    برای(block=UTRIE_BMP_INDEX_LENGTH; block<indexLength; block+=UTRIE_SURROGATE_BLOCK_شمار) {
        برای(i=0; i<UTRIE_SURROGATE_BLOCK_شمار; ++i) {
            اگر(idx[block+i]!=idx[otherBlock+i]) {
                بشکن;
            }
        }
        اگر(i==UTRIE_SURROGATE_BLOCK_شمار) {
            بازگشت block;
        }
    }
    بازگشت indexLength;
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
ایستا تهی
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_مقدار *get_Folded_مقدار, UErrorCode *pErrorCode) {
    int32_t leadIndexes[UTRIE_SURROGATE_BLOCK_شمار];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t indexLength, block;
#ifdef UTRIE_DEBUG
    صحیح شمار_Lead_CUWith_Data=0;
#endif

    idx=trie->index;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_SHIFT), 4*UTRIE_SURROGATE_BLOCK_شمار);

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
    اگر(trie->lead_Unit_مقدار==trie->data[0]) {
        block=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } وگرنه {
        /* create and fill the repeatBlock */
        block=utrie_allocDataBlock(trie);
        اگر(block<0) {
            /* data table overflow */
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
        utrie_fillBlock(trie->data+block, 0, UTRIE_DATA_BLOCK_LENGTH, trie->lead_Unit_مقدار, trie->data[0], درست);
        block=-block; /* negative block number to indicate that it is a repeat block */
    }
    برای(c=(0xd800>>UTRIE_SHIFT); c<(0xdc00>>UTRIE_SHIFT); ++c) {
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
    برای(c=0x10000; c<0x110000;) {
        اگر(idx[c>>UTRIE_SHIFT]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#ifdef UTRIE_DEBUG
            ++شمار_Lead_CUWith_Data;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#endif

            /* is there an identical index block? */
            block=_findSameIndexBlock(idx, indexLength, c>>UTRIE_SHIFT);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_مقدار(trie, c, block+UTRIE_SURROGATE_BLOCK_شمار);
            اگر(value!=utrie_get32(trie, U16_LEAD(c), nullptr)) {
                اگر(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    بازگشت;
                }

                /* if we did not find an identical index block... */
                اگر(block==indexLength) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+indexLength,
                                 idx+(c>>UTRIE_SHIFT),
                                 4*UTRIE_SURROGATE_BLOCK_شمار);
                    indexLength+=UTRIE_SURROGATE_BLOCK_شمار;
                }
            }
            c+=0x400;
        } وگرنه {
            c+=UTRIE_DATA_BLOCK_LENGTH;
        }
    }
#ifdef UTRIE_DEBUG
    اگر(شمار_Lead_CUWith_Data>0) {
        printf("supplementary data for %d lead surrogates\n", شمار_Lead_CUWith_Data);
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
    اگر(indexLength>=UTRIE_MAX_INDEX_LENGTH) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_INDEX_LENGTH+UTRIE_SURROGATE_BLOCK_شمار,
                 idx+UTRIE_BMP_INDEX_LENGTH,
                 4*(indexLength-UTRIE_BMP_INDEX_LENGTH));
    uprv_memcpy(idx+UTRIE_BMP_INDEX_LENGTH,
                leadIndexes,
                4*UTRIE_SURROGATE_BLOCK_شمار);
    indexLength+=UTRIE_SURROGATE_BLOCK_شمار;

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
ایستا تهی
_findUnusedBlocks(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_TIME_DATA_LENGTH>>UTRIE_SHIFT)*4);

    /* mark each block that _is_ used with 0 */
    برای(i=0; i<trie->indexLength; ++i) {
        trie->map[ABS(trie->index[i])>>UTRIE_SHIFT]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

ایستا int32_t
_findSameDataBlock(ثابت uint32_t *data, int32_t dataLength,
                   int32_t otherBlock, int32_t step) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=UTRIE_DATA_BLOCK_LENGTH;

    برای(block=0; block<=dataLength; block+=step) {
        اگر(equal_uint32(data+block, data+otherBlock, UTRIE_DATA_BLOCK_LENGTH)) {
            بازگشت block;
        }
    }
    بازگشت -1;
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
ایستا تهی
utrie_compact(UNewTrie *trie, UBool overlap, UErrorCode *pErrorCode) {
    int32_t i, start, newStart, overlapStart;

    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت;
    }

    /* valid, uncompacted trie? */
    اگر(trie==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    اگر(trie->isCompacted) {
        بازگشت; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    _findUnusedBlocks(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    اگر(trie->isLatin1Linear && UTRIE_SHIFT<=8) {
        overlapStart=UTRIE_DATA_BLOCK_LENGTH+256;
    } وگرنه {
        overlapStart=UTRIE_DATA_BLOCK_LENGTH;
    }

    newStart=UTRIE_DATA_BLOCK_LENGTH;
    برای(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        اگر(trie->map[start>>UTRIE_SHIFT]<0) {
            /* advance start to the next block */
            start+=UTRIE_DATA_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            ادامه;
        }

        /* search for an identical block */
        اگر( start>=overlapStart &&
            (i=_findSameDataBlock(trie->data, newStart, start,
                            overlap ? UTRIE_DATA_GRANULARITY : UTRIE_DATA_BLOCK_LENGTH))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE_SHIFT]=i;

            /* advance start to the next block */
            start+=UTRIE_DATA_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            ادامه;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        اگر(overlap && start>=overlapStart) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            برای(i=UTRIE_DATA_BLOCK_LENGTH-UTRIE_DATA_GRANULARITY;
                i>0 && !equal_uint32(trie->data+(newStart-i), trie->data+start, i);
                i-=UTRIE_DATA_GRANULARITY) {}
        } وگرنه {
            i=0;
        }

        اگر(i>0) {
            /* some overlap */
            trie->map[start>>UTRIE_SHIFT]=newStart-i;

            /* move the non-overlapping indexes to their new positions */
            start+=i;
            برای(i=UTRIE_DATA_BLOCK_LENGTH-i; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } وگرنه اگر(newStart<start) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[start>>UTRIE_SHIFT]=newStart;
            برای(i=UTRIE_DATA_BLOCK_LENGTH; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } وگرنه /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE_SHIFT]=start;
            newStart+=UTRIE_DATA_BLOCK_LENGTH;
            start=newStart;
        }
    }

    /* now adjust the index (stage 1) table */
    برای(i=0; i<trie->indexLength; ++i) {
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
ایستا uint32_t U_CALLCONV
default_Get_Folded_مقدار(UNewTrie *trie, UChar32 start, int32_t offset) {
    uint32_t value, initial_مقدار;
    UChar32 limit;
    UBool inBlockZero;

    initial_مقدار=trie->data[0];
    limit=start+0x400;
    وقتی(start<limit) {
        value=utrie_get32(trie, start, &inBlockZero);
        اگر(inBlockZero) {
            start+=UTRIE_DATA_BLOCK_LENGTH;
        } وگرنه اگر(value!=initial_مقدار) {
            بازگشت static_cast<uint32_t>(offset);
        } وگرنه {
            ++start;
        }
    }
    بازگشت 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, تهی *dt, int32_t capacity,
                UNew_Trie_Get_Folded_مقدار *get_Folded_مقدار,
                UBool reduceTo16Bits,
                UErrorCode *pErrorCode) {
    UTrieHeader *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* data = nullptr;

    /* argument check */
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    اگر(trie==nullptr || capacity<0 || (capacity>0 && dt==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    اگر(get_Folded_مقدار==nullptr) {
        get_Folded_مقدار=default_Get_Folded_مقدار;
    }

    data = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    اگر(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, نادرست, pErrorCode);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_مقدار, pErrorCode);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, درست, pErrorCode);

        trie->isCompacted=درست;
        اگر(U_FAILURE(*pErrorCode)) {
            بازگشت 0;
        }
    }

    /* is dataLength within limits? */
    اگر( (reduceTo16Bits ? (trie->dataLength+trie->indexLength) : trie->dataLength) >= UTRIE_MAX_DATA_LENGTH) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    }

    length=اندازه(UTrieHeader)+2*trie->indexLength;
    اگر(reduceTo16Bits) {
        length+=2*trie->dataLength;
    } وگرنه {
        length+=4*trie->dataLength;
    }

    اگر(length>capacity) {
        بازگشت length; /* preflighting */
    }

#ifdef UTRIE_DEBUG
    printf("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (long)trie->indexLength, (long)trie->dataLength, (long)length);
#endif

    /* set the header fields */
    header=(UTrieHeader *)data;
    data+=اندازه(UTrieHeader);

    header->signature=0x54726965; /* "Trie" */
    header->options=UTRIE_SHIFT | (UTRIE_INDEX_SHIFT<<UTRIE_OPTIONS_INDEX_SHIFT);

    اگر(!reduceTo16Bits) {
        header->options|=UTRIE_OPTIONS_DATA_IS_32_BIT;
    }
    اگر(trie->isLatin1Linear) {
        header->options|=UTRIE_OPTIONS_LATIN1_IS_LINEAR;
    }

    header->indexLength=trie->indexLength;
    header->dataLength=trie->dataLength;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    اگر(reduceTo16Bits) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->index;
        dest16=(uint16_t *)data;
        برای(i=trie->indexLength; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->indexLength)>>UTRIE_INDEX_SHIFT);
        }

        /* write 16-bit data values */
        p=trie->data;
        برای(i=trie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } وگرنه {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->index;
        dest16=(uint16_t *)data;
        برای(i=trie->indexLength; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_INDEX_SHIFT);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->data, 4*(size_t)trie->dataLength);
    }

    بازگشت length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_defaultGetFoldingOffset(uint32_t data) {
    بازگشت (int32_t)data;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ثابت تهی *data, int32_t length, UErrorCode *pErrorCode) {
    ثابت UTrieHeader *header;
    ثابت uint16_t *p16;
    uint32_t options;

    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت -1;
    }

    /* enough data for a trie header? */
    اگر(length<(int32_t)اندازه(UTrieHeader)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        بازگشت -1;
    }

    /* check the signature */
    header=(ثابت UTrieHeader *)data;
    اگر(header->signature!=0x54726965) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        بازگشت -1;
    }

    /* get the options and check the shift values */
    options=header->options;
    اگر( (options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        بازگشت -1;
    }
    trie->isLatin1Linear = (options & UTRIE_OPTIONS_LATIN1_IS_LINEAR) != 0;

    /* get the length values */
    trie->indexLength=header->indexLength;
    trie->dataLength=header->dataLength;

    length-=(int32_t)اندازه(UTrieHeader);

    /* enough data for the index? */
    اگر(length<2*trie->indexLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        بازگشت -1;
    }
    p16=(ثابت uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;
    length-=2*trie->indexLength;

    /* get the data */
    اگر(options&UTRIE_OPTIONS_DATA_IS_32_BIT) {
        اگر(length<4*trie->dataLength) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            بازگشت -1;
        }
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_مقدار=trie->data32[0];
        length=(int32_t)اندازه(UTrieHeader)+2*trie->indexLength+4*trie->dataLength;
    } وگرنه {
        اگر(length<2*trie->dataLength) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            بازگشت -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=nullptr;
        trie->initial_مقدار=trie->index[trie->indexLength];
        length=(int32_t)اندازه(UTrieHeader)+2*trie->indexLength+2*trie->dataLength;
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    بازگشت length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       تهی *data, int32_t length,
                       uint32_t initial_مقدار, uint32_t lead_Unit_مقدار,
                       UBool make16BitTrie,
                       UErrorCode *pErrorCode) {
    uint16_t *p16;
    int32_t actualLength, latin1Length, i, limit;
    uint16_t block;

    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1Length= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->indexLength=UTRIE_BMP_INDEX_LENGTH+UTRIE_SURROGATE_BLOCK_شمار;
    trie->dataLength=latin1Length;
    اگر(lead_Unit_مقدار!=initial_مقدار) {
        trie->dataLength+=UTRIE_DATA_BLOCK_LENGTH;
    }

    actualLength=trie->indexLength*2;
    اگر(make16BitTrie) {
        actualLength+=trie->dataLength*2;
    } وگرنه {
        actualLength+=trie->dataLength*4;
    }

    /* enough space for the dummy trie? */
    اگر(length<actualLength) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        بازگشت actualLength;
    }

    trie->isLatin1Linear=درست;
    trie->initial_مقدار=initial_مقدار;

    /* fill the index and data arrays */
    p16=(uint16_t *)data;
    trie->index=p16;

    اگر(make16BitTrie) {
        /* indexes to block 0 */
        block=(uint16_t)(trie->indexLength>>UTRIE_INDEX_SHIFT);
        limit=trie->indexLength;
        برای(i=0; i<limit; ++i) {
            p16[i]=block;
        }

        اگر(lead_Unit_مقدار!=initial_مقدار) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block+=(uint16_t)(latin1Length>>UTRIE_INDEX_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            برای(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=nullptr;

        /* Latin-1 data */
        p16+=trie->indexLength;
        برای(i=0; i<latin1Length; ++i) {
            p16[i]=(uint16_t)initial_مقدار;
        }

        /* data for lead surrogate code units */
        اگر(lead_Unit_مقدار!=initial_مقدار) {
            limit=latin1Length+UTRIE_DATA_BLOCK_LENGTH;
            برای(/* i=latin1Length */; i<limit; ++i) {
                p16[i]=(uint16_t)lead_Unit_مقدار;
            }
        }
    } وگرنه {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->indexLength*2);

        اگر(lead_Unit_مقدار!=initial_مقدار) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block=(uint16_t)(latin1Length>>UTRIE_INDEX_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            برای(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->indexLength);

        /* Latin-1 data */
        برای(i=0; i<latin1Length; ++i) {
            p32[i]=initial_مقدار;
        }

        /* data for lead surrogate code units */
        اگر(lead_Unit_مقدار!=initial_مقدار) {
            limit=latin1Length+UTRIE_DATA_BLOCK_LENGTH;
            برای(/* i=latin1Length */; i<limit; ++i) {
                p32[i]=lead_Unit_مقدار;
            }
        }
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    بازگشت actualLength;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
ایستا uint32_t U_CALLCONV
enum_Same_مقدار(ثابت تهی * /*context*/, uint32_t value) {
    بازگشت value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI تهی U_EXPORT2
utrie_enum(ثابت UTrie *trie,
           UTrie_Enum_مقدار *enum_مقدار, UTrieEnumRange *enumRange, ثابت تهی *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prev_مقدار, initial_مقدار;
    UChar32 c, prev;
    int32_t l, i, j, block, prevBlock, nullBlock, offset;

    /* check arguments */
    اگر(trie==nullptr || trie->index==nullptr || enumRange==nullptr) {
        بازگشت;
    }
    اگر(enum_مقدار==nullptr) {
        enum_مقدار=enum_Same_مقدار;
    }

    idx=trie->index;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_مقدار=enum_مقدار(context, trie->initial_مقدار);

    اگر(data32==nullptr) {
        nullBlock=trie->indexLength;
    } وگرنه {
        nullBlock=0;
    }

    /* set variables for previous range */
    prevBlock=nullBlock;
    prev=0;
    prev_مقدار=initial_مقدار;

    /* enumerate BMP - the main loop enumerates data blocks */
    برای(i=0, c=0; c<=0xffff; ++i) {
        اگر(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_INDEX_LENGTH;
        } وگرنه اگر(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_SHIFT;
        }

        block=idx[i]<<UTRIE_INDEX_SHIFT;
        اگر(block==prevBlock) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_DATA_BLOCK_LENGTH;
        } وگرنه اگر(block==nullBlock) {
            /* this is the all-initial-value block */
            اگر(prev_مقدار!=initial_مقدار) {
                اگر(prev<c) {
                    اگر(!enumRange(context, prev, c, prev_مقدار)) {
                        بازگشت;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prev_مقدار=initial_مقدار;
            }
            c+=UTRIE_DATA_BLOCK_LENGTH;
        } وگرنه {
            prevBlock=block;
            برای(j=0; j<UTRIE_DATA_BLOCK_LENGTH; ++j) {
                value=enum_مقدار(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                اگر(value!=prev_مقدار) {
                    اگر(prev<c) {
                        اگر(!enumRange(context, prev, c, prev_مقدار)) {
                            بازگشت;
                        }
                    }
                    اگر(j>0) {
                        /* the block is not filled with all the same value */
                        prevBlock=-1;
                    }
                    prev=c;
                    prev_مقدار=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    برای(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        offset=idx[l>>UTRIE_SHIFT]<<UTRIE_INDEX_SHIFT;
        اگر(offset==nullBlock) {
            /* no entries for a whole block of lead surrogates */
            اگر(prev_مقدار!=initial_مقدار) {
                اگر(prev<c) {
                    اگر(!enumRange(context, prev, c, prev_مقدار)) {
                        بازگشت;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prev_مقدار=initial_مقدار;
            }

            l+=UTRIE_DATA_BLOCK_LENGTH;
            c+=UTRIE_DATA_BLOCK_LENGTH<<10;
            ادامه;
        }

        value= data32!=nullptr ? data32[offset+(l&UTRIE_MASK)] : idx[offset+(l&UTRIE_MASK)];

        /* enumerate trail surrogates for this lead surrogate */
        offset=trie->getFoldingOffset(value);
        اگر(offset<=0) {
            /* no data for this lead surrogate */
            اگر(prev_مقدار!=initial_مقدار) {
                اگر(prev<c) {
                    اگر(!enumRange(context, prev, c, prev_مقدار)) {
                        بازگشت;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prev_مقدار=initial_مقدار;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } وگرنه {
            /* enumerate code points for this lead surrogate */
            i=offset;
            offset+=UTRIE_SURROGATE_BLOCK_شمار;
            انجامبده {
                /* copy of most of the body of the BMP loop */
                block=idx[i]<<UTRIE_INDEX_SHIFT;
                اگر(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_DATA_BLOCK_LENGTH;
                } وگرنه اگر(block==nullBlock) {
                    /* this is the all-initial-value block */
                    اگر(prev_مقدار!=initial_مقدار) {
                        اگر(prev<c) {
                            اگر(!enumRange(context, prev, c, prev_مقدار)) {
                                بازگشت;
                            }
                        }
                        prevBlock=nullBlock;
                        prev=c;
                        prev_مقدار=initial_مقدار;
                    }
                    c+=UTRIE_DATA_BLOCK_LENGTH;
                } وگرنه {
                    prevBlock=block;
                    برای(j=0; j<UTRIE_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_مقدار(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        اگر(value!=prev_مقدار) {
                            اگر(prev<c) {
                                اگر(!enumRange(context, prev, c, prev_مقدار)) {
                                    بازگشت;
                                }
                            }
                            اگر(j>0) {
                                /* the block is not filled with all the same value */
                                prevBlock=-1;
                            }
                            prev=c;
                            prev_مقدار=value;
                        }
                        ++c;
                    }
                }
            } وقتی(++i<offset);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_مقدار);
}

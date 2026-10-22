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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#undef ABS
#define ABS(x) ((x)>=0 ? (x) : -(x))

स्थिर अन्तःस्थ UBool
equal_uint32(ध्रुव uint32_t *s, ध्रुव uint32_t *t, int32_t length) {
    यावत्(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    प्रत्यागम length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fillIn,
           uint32_t *aliasData, int32_t maxDataLength,
           uint32_t initial_मूल्य, uint32_t lead_Unit_मूल्य,
           UBool latin1Linear) {
    UNewTrie *trie;
    int32_t i, j;

    यदि( maxDataLength<UTRIE_DATA_BLOCK_LENGTH ||
        (latin1Linear && maxDataLength<1024)
    ) {
        प्रत्यागम nullptr;
    }

    यदि(fillIn!=nullptr) {
        trie=fillIn;
    } अन्यथा {
        trie=(UNewTrie *)uprv_malloc(परिमाण(UNewTrie));
        यदि(trie==nullptr) {
            प्रत्यागम nullptr;
        }
    }
    uprv_memset(trie, 0, परिमाण(UNewTrie));
    trie->isAllocated = fillIn == nullptr;

    यदि(aliasData!=nullptr) {
        trie->data=aliasData;
        trie->isDataAllocated=असत्य;
    } अन्यथा {
        trie->data=(uint32_t *)uprv_malloc(maxDataLength*4);
        यदि(trie->data==nullptr) {
            uprv_free(trie);
            प्रत्यागम nullptr;
        }
        trie->isDataAllocated=सत्य;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_DATA_BLOCK_LENGTH;

    यदि(latin1Linear) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        कुरु {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->index[i++]=j;
            j+=UTRIE_DATA_BLOCK_LENGTH;
        } यावत्(i<(256>>UTRIE_SHIFT));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->dataLength=j;
    यावत्(j>0) {
        trie->data[--j]=initial_मूल्य;
    }

    trie->lead_Unit_मूल्य=lead_Unit_मूल्य;
    trie->indexLength=UTRIE_MAX_INDEX_LENGTH;
    trie->dataCapacity=maxDataLength;
    trie->isLatin1Linear=latin1Linear;
    trie->isCompacted=असत्य;
    प्रत्यागम trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fillIn, ध्रुव UNewTrie *other, uint32_t *aliasData, int32_t aliasDataCapacity) {
    UNewTrie *trie;
    UBool isDataAllocated;

    /* do not clone if other is not valid or already compacted */
    यदि(other==nullptr || other->data==nullptr || other->isCompacted) {
        प्रत्यागम nullptr;
    }

    /* clone data */
    यदि(aliasData!=nullptr && aliasDataCapacity>=other->dataCapacity) {
        isDataAllocated=असत्य;
    } अन्यथा {
        aliasDataCapacity=other->dataCapacity;
        aliasData=(uint32_t *)uprv_malloc(other->dataCapacity*4);
        यदि(aliasData==nullptr) {
            प्रत्यागम nullptr;
        }
        isDataAllocated=सत्य;
    }

    trie=utrie_open(fillIn, aliasData, aliasDataCapacity,
                    other->data[0], other->lead_Unit_मूल्य,
                    other->isLatin1Linear);
    यदि(trie==nullptr) {
        uprv_free(aliasData);
    } अन्यथा {
        uprv_memcpy(trie->index, other->index, परिमाण(trie->index));
        uprv_memcpy(trie->data, other->data, (size_t)other->dataLength*4);
        trie->dataLength=other->dataLength;
        trie->isDataAllocated=isDataAllocated;
    }

    प्रत्यागम trie;
}

U_CAPI शून्य U_EXPORT2
utrie_close(UNewTrie *trie) {
    यदि(trie!=nullptr) {
        यदि(trie->isDataAllocated) {
            uprv_free(trie->data);
            trie->data=nullptr;
        }
        यदि(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_getData(UNewTrie *trie, int32_t *pLength) {
    यदि(trie==nullptr || pLength==nullptr) {
        प्रत्यागम nullptr;
    }

    *pLength=trie->dataLength;
    प्रत्यागम trie->data;
}

स्थिर int32_t
utrie_allocDataBlock(UNewTrie *trie) {
    int32_t newBlock, new_शीर्ष;

    newBlock=trie->dataLength;
    new_शीर्ष=newBlock+UTRIE_DATA_BLOCK_LENGTH;
    यदि(new_शीर्ष>trie->dataCapacity) {
        /* out of memory in the data array */
        प्रत्यागम -1;
    }
    trie->dataLength=new_शीर्ष;
    प्रत्यागम newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
स्थिर int32_t
utrie_getDataBlock(UNewTrie *trie, UChar32 c) {
    int32_t index_मूल्य, newBlock;

    c>>=UTRIE_SHIFT;
    index_मूल्य=trie->index[c];
    यदि(index_मूल्य>0) {
        प्रत्यागम index_मूल्य;
    }

    /* allocate a new data block */
    newBlock=utrie_allocDataBlock(trie);
    यदि(newBlock<0) {
        /* out of memory in the data array */
        प्रत्यागम -1;
    }
    trie->index[c]=newBlock;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->data+newBlock, trie->data-index_मूल्य, 4*UTRIE_DATA_BLOCK_LENGTH);
    प्रत्यागम newBlock;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    यदि(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        प्रत्यागम असत्य;
    }

    block=utrie_getDataBlock(trie, c);
    यदि(block<0) {
        प्रत्यागम असत्य;
    }

    trie->data[block+(c&UTRIE_MASK)]=value;
    प्रत्यागम सत्य;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *pInBlockZero) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    यदि(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        यदि(pInBlockZero!=nullptr) {
            *pInBlockZero=सत्य;
        }
        प्रत्यागम 0;
    }

    block=trie->index[c>>UTRIE_SHIFT];
    यदि(pInBlockZero!=nullptr) {
        *pInBlockZero = block == 0;
    }

    प्रत्यागम trie->data[ABS(block)+(c&UTRIE_MASK)];
}

/**
 * @internal
 */
स्थिर शून्य
utrie_fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
                uint32_t value, uint32_t initial_मूल्य, UBool overwrite) {
    uint32_t *pLimit;

    pLimit=block+limit;
    block+=start;
    यदि(overwrite) {
        यावत्(block<pLimit) {
            *block++=value;
        }
    } अन्यथा {
        यावत्(block<pLimit) {
            यदि(*block==initial_मूल्य) {
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
    uint32_t initial_मूल्य;
    int32_t block, rest, repeatBlock;

    /* valid, uncompacted trie and valid indexes? */
    यदि( trie==nullptr || trie->isCompacted ||
        (uint32_t)start>0x10ffff || (uint32_t)limit>0x110000 || start>limit
    ) {
        प्रत्यागम असत्य;
    }
    यदि(start==limit) {
        प्रत्यागम सत्य; /* nothing to do */
    }

    initial_मूल्य=trie->data[0];
    यदि(start&UTRIE_MASK) {
        UChar32 nextStart;

        /* set partial block at [start..following block boundary[ */
        block=utrie_getDataBlock(trie, start);
        यदि(block<0) {
            प्रत्यागम असत्य;
        }

        nextStart=(start+UTRIE_DATA_BLOCK_LENGTH)&~UTRIE_MASK;
        यदि(nextStart<=limit) {
            utrie_fillBlock(trie->data+block, start&UTRIE_MASK, UTRIE_DATA_BLOCK_LENGTH,
                            value, initial_मूल्य, overwrite);
            start=nextStart;
        } अन्यथा {
            utrie_fillBlock(trie->data+block, start&UTRIE_MASK, limit&UTRIE_MASK,
                            value, initial_मूल्य, overwrite);
            प्रत्यागम सत्य;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE_MASK;

    /* iterate over all-value blocks */
    यदि(value==initial_मूल्य) {
        repeatBlock=0;
    } अन्यथा {
        repeatBlock=-1;
    }
    यावत्(start<limit) {
        /* get index value */
        block=trie->index[start>>UTRIE_SHIFT];
        यदि(block>0) {
            /* already allocated, fill in value */
            utrie_fillBlock(trie->data+block, 0, UTRIE_DATA_BLOCK_LENGTH, value, initial_मूल्य, overwrite);
        } अन्यथा यदि(trie->data[-block]!=value && (block==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            यदि(repeatBlock>=0) {
                trie->index[start>>UTRIE_SHIFT]=-repeatBlock;
            } अन्यथा {
                /* create and set and fill the repeatBlock */
                repeatBlock=utrie_getDataBlock(trie, start);
                यदि(repeatBlock<0) {
                    प्रत्यागम असत्य;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->index[start>>UTRIE_SHIFT]=-repeatBlock;
                utrie_fillBlock(trie->data+repeatBlock, 0, UTRIE_DATA_BLOCK_LENGTH, value, initial_मूल्य, सत्य);
            }
        }

        start+=UTRIE_DATA_BLOCK_LENGTH;
    }

    यदि(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=utrie_getDataBlock(trie, start);
        यदि(block<0) {
            प्रत्यागम असत्य;
        }

        utrie_fillBlock(trie->data+block, 0, rest, value, initial_मूल्य, overwrite);
    }

    प्रत्यागम सत्य;
}

स्थिर int32_t
_findSameIndexBlock(ध्रुव int32_t *idx, int32_t indexLength,
                    int32_t otherBlock) {
    int32_t block, i;

    प्रति(block=UTRIE_BMP_INDEX_LENGTH; block<indexLength; block+=UTRIE_SURROGATE_BLOCK_गिनती) {
        प्रति(i=0; i<UTRIE_SURROGATE_BLOCK_गिनती; ++i) {
            यदि(idx[block+i]!=idx[otherBlock+i]) {
                भङ्ग;
            }
        }
        यदि(i==UTRIE_SURROGATE_BLOCK_गिनती) {
            प्रत्यागम block;
        }
    }
    प्रत्यागम indexLength;
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
स्थिर शून्य
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_मूल्य *get_Folded_मूल्य, UErrorCode *pErrorCode) {
    int32_t leadIndexes[UTRIE_SURROGATE_BLOCK_गिनती];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t indexLength, block;
#ifdef UTRIE_DEBUG
    पूर्णाङ्क गिनती_Lead_CUWith_Data=0;
#endif

    idx=trie->index;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_SHIFT), 4*UTRIE_SURROGATE_BLOCK_गिनती);

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
    यदि(trie->lead_Unit_मूल्य==trie->data[0]) {
        block=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } अन्यथा {
        /* create and fill the repeatBlock */
        block=utrie_allocDataBlock(trie);
        यदि(block<0) {
            /* data table overflow */
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
        utrie_fillBlock(trie->data+block, 0, UTRIE_DATA_BLOCK_LENGTH, trie->lead_Unit_मूल्य, trie->data[0], सत्य);
        block=-block; /* negative block number to indicate that it is a repeat block */
    }
    प्रति(c=(0xd800>>UTRIE_SHIFT); c<(0xdc00>>UTRIE_SHIFT); ++c) {
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
    प्रति(c=0x10000; c<0x110000;) {
        यदि(idx[c>>UTRIE_SHIFT]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#ifdef UTRIE_DEBUG
            ++गिनती_Lead_CUWith_Data;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#endif

            /* is there an identical index block? */
            block=_findSameIndexBlock(idx, indexLength, c>>UTRIE_SHIFT);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_मूल्य(trie, c, block+UTRIE_SURROGATE_BLOCK_गिनती);
            यदि(value!=utrie_get32(trie, U16_LEAD(c), nullptr)) {
                यदि(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    प्रत्यागम;
                }

                /* if we did not find an identical index block... */
                यदि(block==indexLength) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+indexLength,
                                 idx+(c>>UTRIE_SHIFT),
                                 4*UTRIE_SURROGATE_BLOCK_गिनती);
                    indexLength+=UTRIE_SURROGATE_BLOCK_गिनती;
                }
            }
            c+=0x400;
        } अन्यथा {
            c+=UTRIE_DATA_BLOCK_LENGTH;
        }
    }
#ifdef UTRIE_DEBUG
    यदि(गिनती_Lead_CUWith_Data>0) {
        printf("supplementary data for %d lead surrogates\n", गिनती_Lead_CUWith_Data);
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
    यदि(indexLength>=UTRIE_MAX_INDEX_LENGTH) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_INDEX_LENGTH+UTRIE_SURROGATE_BLOCK_गिनती,
                 idx+UTRIE_BMP_INDEX_LENGTH,
                 4*(indexLength-UTRIE_BMP_INDEX_LENGTH));
    uprv_memcpy(idx+UTRIE_BMP_INDEX_LENGTH,
                leadIndexes,
                4*UTRIE_SURROGATE_BLOCK_गिनती);
    indexLength+=UTRIE_SURROGATE_BLOCK_गिनती;

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
स्थिर शून्य
_findUnusedBlocks(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_TIME_DATA_LENGTH>>UTRIE_SHIFT)*4);

    /* mark each block that _is_ used with 0 */
    प्रति(i=0; i<trie->indexLength; ++i) {
        trie->map[ABS(trie->index[i])>>UTRIE_SHIFT]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

स्थिर int32_t
_findSameDataBlock(ध्रुव uint32_t *data, int32_t dataLength,
                   int32_t otherBlock, int32_t step) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    dataLength-=UTRIE_DATA_BLOCK_LENGTH;

    प्रति(block=0; block<=dataLength; block+=step) {
        यदि(equal_uint32(data+block, data+otherBlock, UTRIE_DATA_BLOCK_LENGTH)) {
            प्रत्यागम block;
        }
    }
    प्रत्यागम -1;
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
स्थिर शून्य
utrie_compact(UNewTrie *trie, UBool overlap, UErrorCode *pErrorCode) {
    int32_t i, start, newStart, overlapStart;

    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }

    /* valid, uncompacted trie? */
    यदि(trie==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    यदि(trie->isCompacted) {
        प्रत्यागम; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    _findUnusedBlocks(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    यदि(trie->isLatin1Linear && UTRIE_SHIFT<=8) {
        overlapStart=UTRIE_DATA_BLOCK_LENGTH+256;
    } अन्यथा {
        overlapStart=UTRIE_DATA_BLOCK_LENGTH;
    }

    newStart=UTRIE_DATA_BLOCK_LENGTH;
    प्रति(start=newStart; start<trie->dataLength;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        यदि(trie->map[start>>UTRIE_SHIFT]<0) {
            /* advance start to the next block */
            start+=UTRIE_DATA_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* search for an identical block */
        यदि( start>=overlapStart &&
            (i=_findSameDataBlock(trie->data, newStart, start,
                            overlap ? UTRIE_DATA_GRANULARITY : UTRIE_DATA_BLOCK_LENGTH))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE_SHIFT]=i;

            /* advance start to the next block */
            start+=UTRIE_DATA_BLOCK_LENGTH;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        यदि(overlap && start>=overlapStart) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            प्रति(i=UTRIE_DATA_BLOCK_LENGTH-UTRIE_DATA_GRANULARITY;
                i>0 && !equal_uint32(trie->data+(newStart-i), trie->data+start, i);
                i-=UTRIE_DATA_GRANULARITY) {}
        } अन्यथा {
            i=0;
        }

        यदि(i>0) {
            /* some overlap */
            trie->map[start>>UTRIE_SHIFT]=newStart-i;

            /* move the non-overlapping indexes to their new positions */
            start+=i;
            प्रति(i=UTRIE_DATA_BLOCK_LENGTH-i; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } अन्यथा यदि(newStart<start) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[start>>UTRIE_SHIFT]=newStart;
            प्रति(i=UTRIE_DATA_BLOCK_LENGTH; i>0; --i) {
                trie->data[newStart++]=trie->data[start++];
            }
        } अन्यथा /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE_SHIFT]=start;
            newStart+=UTRIE_DATA_BLOCK_LENGTH;
            start=newStart;
        }
    }

    /* now adjust the index (stage 1) table */
    प्रति(i=0; i<trie->indexLength; ++i) {
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
स्थिर uint32_t U_CALLCONV
default_Get_Folded_मूल्य(UNewTrie *trie, UChar32 start, int32_t offset) {
    uint32_t value, initial_मूल्य;
    UChar32 limit;
    UBool inBlockZero;

    initial_मूल्य=trie->data[0];
    limit=start+0x400;
    यावत्(start<limit) {
        value=utrie_get32(trie, start, &inBlockZero);
        यदि(inBlockZero) {
            start+=UTRIE_DATA_BLOCK_LENGTH;
        } अन्यथा यदि(value!=initial_मूल्य) {
            प्रत्यागम static_cast<uint32_t>(offset);
        } अन्यथा {
            ++start;
        }
    }
    प्रत्यागम 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, शून्य *dt, int32_t capacity,
                UNew_Trie_Get_Folded_मूल्य *get_Folded_मूल्य,
                UBool reduceTo16Bits,
                UErrorCode *pErrorCode) {
    UTrieHeader *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* data = nullptr;

    /* argument check */
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    यदि(trie==nullptr || capacity<0 || (capacity>0 && dt==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    यदि(get_Folded_मूल्य==nullptr) {
        get_Folded_मूल्य=default_Get_Folded_मूल्य;
    }

    data = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    यदि(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, असत्य, pErrorCode);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_मूल्य, pErrorCode);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, सत्य, pErrorCode);

        trie->isCompacted=सत्य;
        यदि(U_FAILURE(*pErrorCode)) {
            प्रत्यागम 0;
        }
    }

    /* is dataLength within limits? */
    यदि( (reduceTo16Bits ? (trie->dataLength+trie->indexLength) : trie->dataLength) >= UTRIE_MAX_DATA_LENGTH) {
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
    }

    length=परिमाण(UTrieHeader)+2*trie->indexLength;
    यदि(reduceTo16Bits) {
        length+=2*trie->dataLength;
    } अन्यथा {
        length+=4*trie->dataLength;
    }

    यदि(length>capacity) {
        प्रत्यागम length; /* preflighting */
    }

#ifdef UTRIE_DEBUG
    printf("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (long)trie->indexLength, (long)trie->dataLength, (long)length);
#endif

    /* set the header fields */
    header=(UTrieHeader *)data;
    data+=परिमाण(UTrieHeader);

    header->signature=0x54726965; /* "Trie" */
    header->options=UTRIE_SHIFT | (UTRIE_INDEX_SHIFT<<UTRIE_OPTIONS_INDEX_SHIFT);

    यदि(!reduceTo16Bits) {
        header->options|=UTRIE_OPTIONS_DATA_IS_32_BIT;
    }
    यदि(trie->isLatin1Linear) {
        header->options|=UTRIE_OPTIONS_LATIN1_IS_LINEAR;
    }

    header->indexLength=trie->indexLength;
    header->dataLength=trie->dataLength;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    यदि(reduceTo16Bits) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->index;
        dest16=(uint16_t *)data;
        प्रति(i=trie->indexLength; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->indexLength)>>UTRIE_INDEX_SHIFT);
        }

        /* write 16-bit data values */
        p=trie->data;
        प्रति(i=trie->dataLength; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } अन्यथा {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->index;
        dest16=(uint16_t *)data;
        प्रति(i=trie->indexLength; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_INDEX_SHIFT);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->data, 4*(size_t)trie->dataLength);
    }

    प्रत्यागम length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_defaultGetFoldingOffset(uint32_t data) {
    प्रत्यागम (int32_t)data;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ध्रुव शून्य *data, int32_t length, UErrorCode *pErrorCode) {
    ध्रुव UTrieHeader *header;
    ध्रुव uint16_t *p16;
    uint32_t options;

    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम -1;
    }

    /* enough data for a trie header? */
    यदि(length<(int32_t)परिमाण(UTrieHeader)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम -1;
    }

    /* check the signature */
    header=(ध्रुव UTrieHeader *)data;
    यदि(header->signature!=0x54726965) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम -1;
    }

    /* get the options and check the shift values */
    options=header->options;
    यदि( (options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((options>>UTRIE_OPTIONS_INDEX_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_INDEX_SHIFT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम -1;
    }
    trie->isLatin1Linear = (options & UTRIE_OPTIONS_LATIN1_IS_LINEAR) != 0;

    /* get the length values */
    trie->indexLength=header->indexLength;
    trie->dataLength=header->dataLength;

    length-=(int32_t)परिमाण(UTrieHeader);

    /* enough data for the index? */
    यदि(length<2*trie->indexLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम -1;
    }
    p16=(ध्रुव uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;
    length-=2*trie->indexLength;

    /* get the data */
    यदि(options&UTRIE_OPTIONS_DATA_IS_32_BIT) {
        यदि(length<4*trie->dataLength) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            प्रत्यागम -1;
        }
        trie->data32=(ध्रुव uint32_t *)p16;
        trie->initial_मूल्य=trie->data32[0];
        length=(int32_t)परिमाण(UTrieHeader)+2*trie->indexLength+4*trie->dataLength;
    } अन्यथा {
        यदि(length<2*trie->dataLength) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            प्रत्यागम -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=nullptr;
        trie->initial_मूल्य=trie->index[trie->indexLength];
        length=(int32_t)परिमाण(UTrieHeader)+2*trie->indexLength+2*trie->dataLength;
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    प्रत्यागम length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       शून्य *data, int32_t length,
                       uint32_t initial_मूल्य, uint32_t lead_Unit_मूल्य,
                       UBool make16BitTrie,
                       UErrorCode *pErrorCode) {
    uint16_t *p16;
    int32_t actualLength, latin1Length, i, limit;
    uint16_t block;

    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1Length= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->indexLength=UTRIE_BMP_INDEX_LENGTH+UTRIE_SURROGATE_BLOCK_गिनती;
    trie->dataLength=latin1Length;
    यदि(lead_Unit_मूल्य!=initial_मूल्य) {
        trie->dataLength+=UTRIE_DATA_BLOCK_LENGTH;
    }

    actualLength=trie->indexLength*2;
    यदि(make16BitTrie) {
        actualLength+=trie->dataLength*2;
    } अन्यथा {
        actualLength+=trie->dataLength*4;
    }

    /* enough space for the dummy trie? */
    यदि(length<actualLength) {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
        प्रत्यागम actualLength;
    }

    trie->isLatin1Linear=सत्य;
    trie->initial_मूल्य=initial_मूल्य;

    /* fill the index and data arrays */
    p16=(uint16_t *)data;
    trie->index=p16;

    यदि(make16BitTrie) {
        /* indexes to block 0 */
        block=(uint16_t)(trie->indexLength>>UTRIE_INDEX_SHIFT);
        limit=trie->indexLength;
        प्रति(i=0; i<limit; ++i) {
            p16[i]=block;
        }

        यदि(lead_Unit_मूल्य!=initial_मूल्य) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block+=(uint16_t)(latin1Length>>UTRIE_INDEX_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            प्रति(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=nullptr;

        /* Latin-1 data */
        p16+=trie->indexLength;
        प्रति(i=0; i<latin1Length; ++i) {
            p16[i]=(uint16_t)initial_मूल्य;
        }

        /* data for lead surrogate code units */
        यदि(lead_Unit_मूल्य!=initial_मूल्य) {
            limit=latin1Length+UTRIE_DATA_BLOCK_LENGTH;
            प्रति(/* i=latin1Length */; i<limit; ++i) {
                p16[i]=(uint16_t)lead_Unit_मूल्य;
            }
        }
    } अन्यथा {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->indexLength*2);

        यदि(lead_Unit_मूल्य!=initial_मूल्य) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block=(uint16_t)(latin1Length>>UTRIE_INDEX_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            प्रति(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->indexLength);

        /* Latin-1 data */
        प्रति(i=0; i<latin1Length; ++i) {
            p32[i]=initial_मूल्य;
        }

        /* data for lead surrogate code units */
        यदि(lead_Unit_मूल्य!=initial_मूल्य) {
            limit=latin1Length+UTRIE_DATA_BLOCK_LENGTH;
            प्रति(/* i=latin1Length */; i<limit; ++i) {
                p32[i]=lead_Unit_मूल्य;
            }
        }
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    प्रत्यागम actualLength;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
स्थिर uint32_t U_CALLCONV
enum_Same_मूल्य(ध्रुव शून्य * /*context*/, uint32_t value) {
    प्रत्यागम value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI शून्य U_EXPORT2
utrie_enum(ध्रुव UTrie *trie,
           UTrie_Enum_मूल्य *enum_मूल्य, UTrieEnumRange *enumRange, ध्रुव शून्य *context) {
    ध्रुव uint32_t *data32;
    ध्रुव uint16_t *idx;

    uint32_t value, prev_मूल्य, initial_मूल्य;
    UChar32 c, prev;
    int32_t l, i, j, block, prevBlock, nullBlock, offset;

    /* check arguments */
    यदि(trie==nullptr || trie->index==nullptr || enumRange==nullptr) {
        प्रत्यागम;
    }
    यदि(enum_मूल्य==nullptr) {
        enum_मूल्य=enum_Same_मूल्य;
    }

    idx=trie->index;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_मूल्य=enum_मूल्य(context, trie->initial_मूल्य);

    यदि(data32==nullptr) {
        nullBlock=trie->indexLength;
    } अन्यथा {
        nullBlock=0;
    }

    /* set variables for previous range */
    prevBlock=nullBlock;
    prev=0;
    prev_मूल्य=initial_मूल्य;

    /* enumerate BMP - the main loop enumerates data blocks */
    प्रति(i=0, c=0; c<=0xffff; ++i) {
        यदि(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_INDEX_LENGTH;
        } अन्यथा यदि(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_SHIFT;
        }

        block=idx[i]<<UTRIE_INDEX_SHIFT;
        यदि(block==prevBlock) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_DATA_BLOCK_LENGTH;
        } अन्यथा यदि(block==nullBlock) {
            /* this is the all-initial-value block */
            यदि(prev_मूल्य!=initial_मूल्य) {
                यदि(prev<c) {
                    यदि(!enumRange(context, prev, c, prev_मूल्य)) {
                        प्रत्यागम;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prev_मूल्य=initial_मूल्य;
            }
            c+=UTRIE_DATA_BLOCK_LENGTH;
        } अन्यथा {
            prevBlock=block;
            प्रति(j=0; j<UTRIE_DATA_BLOCK_LENGTH; ++j) {
                value=enum_मूल्य(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                यदि(value!=prev_मूल्य) {
                    यदि(prev<c) {
                        यदि(!enumRange(context, prev, c, prev_मूल्य)) {
                            प्रत्यागम;
                        }
                    }
                    यदि(j>0) {
                        /* the block is not filled with all the same value */
                        prevBlock=-1;
                    }
                    prev=c;
                    prev_मूल्य=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    प्रति(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        offset=idx[l>>UTRIE_SHIFT]<<UTRIE_INDEX_SHIFT;
        यदि(offset==nullBlock) {
            /* no entries for a whole block of lead surrogates */
            यदि(prev_मूल्य!=initial_मूल्य) {
                यदि(prev<c) {
                    यदि(!enumRange(context, prev, c, prev_मूल्य)) {
                        प्रत्यागम;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prev_मूल्य=initial_मूल्य;
            }

            l+=UTRIE_DATA_BLOCK_LENGTH;
            c+=UTRIE_DATA_BLOCK_LENGTH<<10;
            अनुवर्त;
        }

        value= data32!=nullptr ? data32[offset+(l&UTRIE_MASK)] : idx[offset+(l&UTRIE_MASK)];

        /* enumerate trail surrogates for this lead surrogate */
        offset=trie->getFoldingOffset(value);
        यदि(offset<=0) {
            /* no data for this lead surrogate */
            यदि(prev_मूल्य!=initial_मूल्य) {
                यदि(prev<c) {
                    यदि(!enumRange(context, prev, c, prev_मूल्य)) {
                        प्रत्यागम;
                    }
                }
                prevBlock=nullBlock;
                prev=c;
                prev_मूल्य=initial_मूल्य;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } अन्यथा {
            /* enumerate code points for this lead surrogate */
            i=offset;
            offset+=UTRIE_SURROGATE_BLOCK_गिनती;
            कुरु {
                /* copy of most of the body of the BMP loop */
                block=idx[i]<<UTRIE_INDEX_SHIFT;
                यदि(block==prevBlock) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_DATA_BLOCK_LENGTH;
                } अन्यथा यदि(block==nullBlock) {
                    /* this is the all-initial-value block */
                    यदि(prev_मूल्य!=initial_मूल्य) {
                        यदि(prev<c) {
                            यदि(!enumRange(context, prev, c, prev_मूल्य)) {
                                प्रत्यागम;
                            }
                        }
                        prevBlock=nullBlock;
                        prev=c;
                        prev_मूल्य=initial_मूल्य;
                    }
                    c+=UTRIE_DATA_BLOCK_LENGTH;
                } अन्यथा {
                    prevBlock=block;
                    प्रति(j=0; j<UTRIE_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_मूल्य(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        यदि(value!=prev_मूल्य) {
                            यदि(prev<c) {
                                यदि(!enumRange(context, prev, c, prev_मूल्य)) {
                                    प्रत्यागम;
                                }
                            }
                            यदि(j>0) {
                                /* the block is not filled with all the same value */
                                prevBlock=-1;
                            }
                            prev=c;
                            prev_मूल्य=value;
                        }
                        ++c;
                    }
                }
            } यावत्(++i<offset);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_मूल्य);
}

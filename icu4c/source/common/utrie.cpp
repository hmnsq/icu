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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#undef ABS
#define ABS(x) ((x)>=0 ? (x) : -(x))

স্থির অন্তঃরেখ UBool
equal_uint32(ধ্রুবক uint32_t *s, ধ্রুবক uint32_t *t, int32_t length) {
    যখন(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ফেরত length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fillIn,
           uint32_t *alias_তথ্য, int32_t max_তথ্য_দৈর্ঘ্য,
           uint32_t initial_মান, uint32_t lead_Unit_মান,
           UBool latin1Linear) {
    UNewTrie *trie;
    int32_t i, j;

    যদি( max_তথ্য_দৈর্ঘ্য<UTRIE_তথ্য_BLOCK_দৈর্ঘ্য ||
        (latin1Linear && max_তথ্য_দৈর্ঘ্য<1024)
    ) {
        ফেরত nullptr;
    }

    যদি(fillIn!=nullptr) {
        trie=fillIn;
    } অন্যথা {
        trie=(UNewTrie *)uprv_malloc(আকার(UNewTrie));
        যদি(trie==nullptr) {
            ফেরত nullptr;
        }
    }
    uprv_memset(trie, 0, আকার(UNewTrie));
    trie->isAllocated = fillIn == nullptr;

    যদি(alias_তথ্য!=nullptr) {
        trie->তথ্য=alias_তথ্য;
        trie->is_তথ্য_Allocated=মিথ্যা;
    } অন্যথা {
        trie->তথ্য=(uint32_t *)uprv_malloc(max_তথ্য_দৈর্ঘ্য*4);
        যদি(trie->তথ্য==nullptr) {
            uprv_free(trie);
            ফেরত nullptr;
        }
        trie->is_তথ্য_Allocated=সত্য;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;

    যদি(latin1Linear) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        করো {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->সূচক[i++]=j;
            j+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
        } যখন(i<(256>>UTRIE_SHIFT));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->তথ্য_দৈর্ঘ্য=j;
    যখন(j>0) {
        trie->তথ্য[--j]=initial_মান;
    }

    trie->lead_Unit_মান=lead_Unit_মান;
    trie->সূচক_দৈর্ঘ্য=UTRIE_MAX_সূচক_দৈর্ঘ্য;
    trie->তথ্য_Capacity=max_তথ্য_দৈর্ঘ্য;
    trie->isLatin1Linear=latin1Linear;
    trie->isCompacted=মিথ্যা;
    ফেরত trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fillIn, ধ্রুবক UNewTrie *other, uint32_t *alias_তথ্য, int32_t alias_তথ্য_Capacity) {
    UNewTrie *trie;
    UBool is_তথ্য_Allocated;

    /* do not clone if other is not valid or already compacted */
    যদি(other==nullptr || other->তথ্য==nullptr || other->isCompacted) {
        ফেরত nullptr;
    }

    /* clone data */
    যদি(alias_তথ্য!=nullptr && alias_তথ্য_Capacity>=other->তথ্য_Capacity) {
        is_তথ্য_Allocated=মিথ্যা;
    } অন্যথা {
        alias_তথ্য_Capacity=other->তথ্য_Capacity;
        alias_তথ্য=(uint32_t *)uprv_malloc(other->তথ্য_Capacity*4);
        যদি(alias_তথ্য==nullptr) {
            ফেরত nullptr;
        }
        is_তথ্য_Allocated=সত্য;
    }

    trie=utrie_open(fillIn, alias_তথ্য, alias_তথ্য_Capacity,
                    other->তথ্য[0], other->lead_Unit_মান,
                    other->isLatin1Linear);
    যদি(trie==nullptr) {
        uprv_free(alias_তথ্য);
    } অন্যথা {
        uprv_memcpy(trie->সূচক, other->সূচক, আকার(trie->সূচক));
        uprv_memcpy(trie->তথ্য, other->তথ্য, (size_t)other->তথ্য_দৈর্ঘ্য*4);
        trie->তথ্য_দৈর্ঘ্য=other->তথ্য_দৈর্ঘ্য;
        trie->is_তথ্য_Allocated=is_তথ্য_Allocated;
    }

    ফেরত trie;
}

U_CAPI শূন্য U_EXPORT2
utrie_close(UNewTrie *trie) {
    যদি(trie!=nullptr) {
        যদি(trie->is_তথ্য_Allocated) {
            uprv_free(trie->তথ্য);
            trie->তথ্য=nullptr;
        }
        যদি(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_তথ্য(UNewTrie *trie, int32_t *p_দৈর্ঘ্য) {
    যদি(trie==nullptr || p_দৈর্ঘ্য==nullptr) {
        ফেরত nullptr;
    }

    *p_দৈর্ঘ্য=trie->তথ্য_দৈর্ঘ্য;
    ফেরত trie->তথ্য;
}

স্থির int32_t
utrie_alloc_তথ্য_Block(UNewTrie *trie) {
    int32_t newBlock, new_শীর্ষ;

    newBlock=trie->তথ্য_দৈর্ঘ্য;
    new_শীর্ষ=newBlock+UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
    যদি(new_শীর্ষ>trie->তথ্য_Capacity) {
        /* out of memory in the data array */
        ফেরত -1;
    }
    trie->তথ্য_দৈর্ঘ্য=new_শীর্ষ;
    ফেরত newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
স্থির int32_t
utrie_get_তথ্য_Block(UNewTrie *trie, UChar32 c) {
    int32_t সূচক_মান, newBlock;

    c>>=UTRIE_SHIFT;
    সূচক_মান=trie->সূচক[c];
    যদি(সূচক_মান>0) {
        ফেরত সূচক_মান;
    }

    /* allocate a new data block */
    newBlock=utrie_alloc_তথ্য_Block(trie);
    যদি(newBlock<0) {
        /* out of memory in the data array */
        ফেরত -1;
    }
    trie->সূচক[c]=newBlock;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->তথ্য+newBlock, trie->তথ্য-সূচক_মান, 4*UTRIE_তথ্য_BLOCK_দৈর্ঘ্য);
    ফেরত newBlock;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    যদি(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        ফেরত মিথ্যা;
    }

    block=utrie_get_তথ্য_Block(trie, c);
    যদি(block<0) {
        ফেরত মিথ্যা;
    }

    trie->তথ্য[block+(c&UTRIE_MASK)]=value;
    ফেরত সত্য;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *pInBlockZero) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    যদি(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        যদি(pInBlockZero!=nullptr) {
            *pInBlockZero=সত্য;
        }
        ফেরত 0;
    }

    block=trie->সূচক[c>>UTRIE_SHIFT];
    যদি(pInBlockZero!=nullptr) {
        *pInBlockZero = block == 0;
    }

    ফেরত trie->তথ্য[ABS(block)+(c&UTRIE_MASK)];
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
        যখন(block<pLimit) {
            *block++=value;
        }
    } অন্যথা {
        যখন(block<pLimit) {
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
        ফেরত মিথ্যা;
    }
    যদি(start==limit) {
        ফেরত সত্য; /* nothing to do */
    }

    initial_মান=trie->তথ্য[0];
    যদি(start&UTRIE_MASK) {
        UChar32 পরবর্তী_Start;

        /* set partial block at [start..following block boundary[ */
        block=utrie_get_তথ্য_Block(trie, start);
        যদি(block<0) {
            ফেরত মিথ্যা;
        }

        পরবর্তী_Start=(start+UTRIE_তথ্য_BLOCK_দৈর্ঘ্য)&~UTRIE_MASK;
        যদি(পরবর্তী_Start<=limit) {
            utrie_fillBlock(trie->তথ্য+block, start&UTRIE_MASK, UTRIE_তথ্য_BLOCK_দৈর্ঘ্য,
                            value, initial_মান, overwrite);
            start=পরবর্তী_Start;
        } অন্যথা {
            utrie_fillBlock(trie->তথ্য+block, start&UTRIE_MASK, limit&UTRIE_MASK,
                            value, initial_মান, overwrite);
            ফেরত সত্য;
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
    যখন(start<limit) {
        /* get index value */
        block=trie->সূচক[start>>UTRIE_SHIFT];
        যদি(block>0) {
            /* already allocated, fill in value */
            utrie_fillBlock(trie->তথ্য+block, 0, UTRIE_তথ্য_BLOCK_দৈর্ঘ্য, value, initial_মান, overwrite);
        } অন্যথা যদি(trie->তথ্য[-block]!=value && (block==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            যদি(repeatBlock>=0) {
                trie->সূচক[start>>UTRIE_SHIFT]=-repeatBlock;
            } অন্যথা {
                /* create and set and fill the repeatBlock */
                repeatBlock=utrie_get_তথ্য_Block(trie, start);
                যদি(repeatBlock<0) {
                    ফেরত মিথ্যা;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->সূচক[start>>UTRIE_SHIFT]=-repeatBlock;
                utrie_fillBlock(trie->তথ্য+repeatBlock, 0, UTRIE_তথ্য_BLOCK_দৈর্ঘ্য, value, initial_মান, সত্য);
            }
        }

        start+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
    }

    যদি(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=utrie_get_তথ্য_Block(trie, start);
        যদি(block<0) {
            ফেরত মিথ্যা;
        }

        utrie_fillBlock(trie->তথ্য+block, 0, rest, value, initial_মান, overwrite);
    }

    ফেরত সত্য;
}

স্থির int32_t
find_Same_সূচক_Block(ধ্রুবক int32_t *idx, int32_t সূচক_দৈর্ঘ্য,
                    int32_t otherBlock) {
    int32_t block, i;

    জন্য(block=UTRIE_BMP_সূচক_দৈর্ঘ্য; block<সূচক_দৈর্ঘ্য; block+=UTRIE_SURROGATE_BLOCK_গুনতি) {
        জন্য(i=0; i<UTRIE_SURROGATE_BLOCK_গুনতি; ++i) {
            যদি(idx[block+i]!=idx[otherBlock+i]) {
                বিরতি;
            }
        }
        যদি(i==UTRIE_SURROGATE_BLOCK_গুনতি) {
            ফেরত block;
        }
    }
    ফেরত সূচক_দৈর্ঘ্য;
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
    int32_t সূচক_দৈর্ঘ্য, block;
#ifdef UTRIE_DEBUG
    পূর্ণসংখ্যা গুনতি_Lead_CUWith_তথ্য=0;
#endif

    idx=trie->সূচক;

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
    যদি(trie->lead_Unit_মান==trie->তথ্য[0]) {
        block=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } অন্যথা {
        /* create and fill the repeatBlock */
        block=utrie_alloc_তথ্য_Block(trie);
        যদি(block<0) {
            /* data table overflow */
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
        utrie_fillBlock(trie->তথ্য+block, 0, UTRIE_তথ্য_BLOCK_দৈর্ঘ্য, trie->lead_Unit_মান, trie->তথ্য[0], সত্য);
        block=-block; /* negative block number to indicate that it is a repeat block */
    }
    জন্য(c=(0xd800>>UTRIE_SHIFT); c<(0xdc00>>UTRIE_SHIFT); ++c) {
        trie->সূচক[c]=block;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    সূচক_দৈর্ঘ্য=UTRIE_BMP_সূচক_দৈর্ঘ্য;

    /* search for any index (stage 1) entries for supplementary code points */
    জন্য(c=0x10000; c<0x110000;) {
        যদি(idx[c>>UTRIE_SHIFT]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#ifdef UTRIE_DEBUG
            ++গুনতি_Lead_CUWith_তথ্য;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#endif

            /* is there an identical index block? */
            block=find_Same_সূচক_Block(idx, সূচক_দৈর্ঘ্য, c>>UTRIE_SHIFT);

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
                    ফেরত;
                }

                /* if we did not find an identical index block... */
                যদি(block==সূচক_দৈর্ঘ্য) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+সূচক_দৈর্ঘ্য,
                                 idx+(c>>UTRIE_SHIFT),
                                 4*UTRIE_SURROGATE_BLOCK_গুনতি);
                    সূচক_দৈর্ঘ্য+=UTRIE_SURROGATE_BLOCK_গুনতি;
                }
            }
            c+=0x400;
        } অন্যথা {
            c+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
        }
    }
#ifdef UTRIE_DEBUG
    যদি(গুনতি_Lead_CUWith_তথ্য>0) {
        printf("supplementary data for %d lead surrogates\n", গুনতি_Lead_CUWith_তথ্য);
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
    যদি(সূচক_দৈর্ঘ্য>=UTRIE_MAX_সূচক_দৈর্ঘ্য) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_সূচক_দৈর্ঘ্য+UTRIE_SURROGATE_BLOCK_গুনতি,
                 idx+UTRIE_BMP_সূচক_দৈর্ঘ্য,
                 4*(সূচক_দৈর্ঘ্য-UTRIE_BMP_সূচক_দৈর্ঘ্য));
    uprv_memcpy(idx+UTRIE_BMP_সূচক_দৈর্ঘ্য,
                leadIndexes,
                4*UTRIE_SURROGATE_BLOCK_গুনতি);
    সূচক_দৈর্ঘ্য+=UTRIE_SURROGATE_BLOCK_গুনতি;

#ifdef UTRIE_DEBUG
    printf("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_সূচক_দৈর্ঘ্য, (long)UTRIE_MAX_সূচক_দৈর্ঘ্য, সূচক_দৈর্ঘ্য);
#endif

    trie->সূচক_দৈর্ঘ্য=সূচক_দৈর্ঘ্য;
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
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_TIME_তথ্য_দৈর্ঘ্য>>UTRIE_SHIFT)*4);

    /* mark each block that _is_ used with 0 */
    জন্য(i=0; i<trie->সূচক_দৈর্ঘ্য; ++i) {
        trie->map[ABS(trie->সূচক[i])>>UTRIE_SHIFT]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

স্থির int32_t
find_Same_তথ্য_Block(ধ্রুবক uint32_t *তথ্য, int32_t তথ্য_দৈর্ঘ্য,
                   int32_t otherBlock, int32_t step) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    তথ্য_দৈর্ঘ্য-=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;

    জন্য(block=0; block<=তথ্য_দৈর্ঘ্য; block+=step) {
        যদি(equal_uint32(তথ্য+block, তথ্য+otherBlock, UTRIE_তথ্য_BLOCK_দৈর্ঘ্য)) {
            ফেরত block;
        }
    }
    ফেরত -1;
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
        ফেরত;
    }

    /* valid, uncompacted trie? */
    যদি(trie==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    যদি(trie->isCompacted) {
        ফেরত; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    _findUnusedBlocks(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    যদি(trie->isLatin1Linear && UTRIE_SHIFT<=8) {
        overlapStart=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য+256;
    } অন্যথা {
        overlapStart=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
    }

    newStart=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
    জন্য(start=newStart; start<trie->তথ্য_দৈর্ঘ্য;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        যদি(trie->map[start>>UTRIE_SHIFT]<0) {
            /* advance start to the next block */
            start+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;

            /* leave newStart with the previous block! */
            চালু;
        }

        /* search for an identical block */
        যদি( start>=overlapStart &&
            (i=find_Same_তথ্য_Block(trie->তথ্য, newStart, start,
                            overlap ? UTRIE_তথ্য_GRANULARITY : UTRIE_তথ্য_BLOCK_দৈর্ঘ্য))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE_SHIFT]=i;

            /* advance start to the next block */
            start+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;

            /* leave newStart with the previous block! */
            চালু;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        যদি(overlap && start>=overlapStart) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            জন্য(i=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য-UTRIE_তথ্য_GRANULARITY;
                i>0 && !equal_uint32(trie->তথ্য+(newStart-i), trie->তথ্য+start, i);
                i-=UTRIE_তথ্য_GRANULARITY) {}
        } অন্যথা {
            i=0;
        }

        যদি(i>0) {
            /* some overlap */
            trie->map[start>>UTRIE_SHIFT]=newStart-i;

            /* move the non-overlapping indexes to their new positions */
            start+=i;
            জন্য(i=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য-i; i>0; --i) {
                trie->তথ্য[newStart++]=trie->তথ্য[start++];
            }
        } অন্যথা যদি(newStart<start) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[start>>UTRIE_SHIFT]=newStart;
            জন্য(i=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য; i>0; --i) {
                trie->তথ্য[newStart++]=trie->তথ্য[start++];
            }
        } অন্যথা /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE_SHIFT]=start;
            newStart+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
            start=newStart;
        }
    }

    /* now adjust the index (stage 1) table */
    জন্য(i=0; i<trie->সূচক_দৈর্ঘ্য; ++i) {
        trie->সূচক[i]=trie->map[ABS(trie->সূচক[i])>>UTRIE_SHIFT];
    }

#ifdef UTRIE_DEBUG
    /* we saved some space */
    printf("compacting trie: count of 32-bit words %lu->%lu\n",
            (long)trie->তথ্য_দৈর্ঘ্য, (long)newStart);
#endif

    trie->তথ্য_দৈর্ঘ্য=newStart;
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

    initial_মান=trie->তথ্য[0];
    limit=start+0x400;
    যখন(start<limit) {
        value=utrie_get32(trie, start, &inBlockZero);
        যদি(inBlockZero) {
            start+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
        } অন্যথা যদি(value!=initial_মান) {
            ফেরত static_cast<uint32_t>(offset);
        } অন্যথা {
            ++start;
        }
    }
    ফেরত 0;
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
    uint8_t* তথ্য = nullptr;

    /* argument check */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    যদি(trie==nullptr || capacity<0 || (capacity>0 && dt==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    যদি(get_Folded_মান==nullptr) {
        get_Folded_মান=default_Get_Folded_মান;
    }

    তথ্য = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    যদি(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, মিথ্যা, pErrorCode);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_মান, pErrorCode);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, সত্য, pErrorCode);

        trie->isCompacted=সত্য;
        যদি(U_FAILURE(*pErrorCode)) {
            ফেরত 0;
        }
    }

    /* is dataLength within limits? */
    যদি( (reduceTo16Bits ? (trie->তথ্য_দৈর্ঘ্য+trie->সূচক_দৈর্ঘ্য) : trie->তথ্য_দৈর্ঘ্য) >= UTRIE_MAX_তথ্য_দৈর্ঘ্য) {
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
    }

    length=আকার(UTrieHeader)+2*trie->সূচক_দৈর্ঘ্য;
    যদি(reduceTo16Bits) {
        length+=2*trie->তথ্য_দৈর্ঘ্য;
    } অন্যথা {
        length+=4*trie->তথ্য_দৈর্ঘ্য;
    }

    যদি(length>capacity) {
        ফেরত length; /* preflighting */
    }

#ifdef UTRIE_DEBUG
    printf("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (long)trie->সূচক_দৈর্ঘ্য, (long)trie->তথ্য_দৈর্ঘ্য, (long)length);
#endif

    /* set the header fields */
    header=(UTrieHeader *)তথ্য;
    তথ্য+=আকার(UTrieHeader);

    header->signature=0x54726965; /* "Trie" */
    header->options=UTRIE_SHIFT | (UTRIE_সূচক_SHIFT<<UTRIE_OPTIONS_সূচক_SHIFT);

    যদি(!reduceTo16Bits) {
        header->options|=UTRIE_OPTIONS_তথ্য_IS_32_BIT;
    }
    যদি(trie->isLatin1Linear) {
        header->options|=UTRIE_OPTIONS_LATIN1_IS_LINEAR;
    }

    header->সূচক_দৈর্ঘ্য=trie->সূচক_দৈর্ঘ্য;
    header->তথ্য_দৈর্ঘ্য=trie->তথ্য_দৈর্ঘ্য;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    যদি(reduceTo16Bits) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->সূচক;
        dest16=(uint16_t *)তথ্য;
        জন্য(i=trie->সূচক_দৈর্ঘ্য; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->সূচক_দৈর্ঘ্য)>>UTRIE_সূচক_SHIFT);
        }

        /* write 16-bit data values */
        p=trie->তথ্য;
        জন্য(i=trie->তথ্য_দৈর্ঘ্য; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } অন্যথা {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->সূচক;
        dest16=(uint16_t *)তথ্য;
        জন্য(i=trie->সূচক_দৈর্ঘ্য; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_সূচক_SHIFT);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->তথ্য, 4*(size_t)trie->তথ্য_দৈর্ঘ্য);
    }

    ফেরত length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_defaultGetFoldingOffset(uint32_t তথ্য) {
    ফেরত (int32_t)তথ্য;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ধ্রুবক শূন্য *তথ্য, int32_t length, UErrorCode *pErrorCode) {
    ধ্রুবক UTrieHeader *header;
    ধ্রুবক uint16_t *p16;
    uint32_t options;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত -1;
    }

    /* enough data for a trie header? */
    যদি(length<(int32_t)আকার(UTrieHeader)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত -1;
    }

    /* check the signature */
    header=(ধ্রুবক UTrieHeader *)তথ্য;
    যদি(header->signature!=0x54726965) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত -1;
    }

    /* get the options and check the shift values */
    options=header->options;
    যদি( (options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((options>>UTRIE_OPTIONS_সূচক_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_সূচক_SHIFT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত -1;
    }
    trie->isLatin1Linear = (options & UTRIE_OPTIONS_LATIN1_IS_LINEAR) != 0;

    /* get the length values */
    trie->সূচক_দৈর্ঘ্য=header->সূচক_দৈর্ঘ্য;
    trie->তথ্য_দৈর্ঘ্য=header->তথ্য_দৈর্ঘ্য;

    length-=(int32_t)আকার(UTrieHeader);

    /* enough data for the index? */
    যদি(length<2*trie->সূচক_দৈর্ঘ্য) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত -1;
    }
    p16=(ধ্রুবক uint16_t *)(header+1);
    trie->সূচক=p16;
    p16+=trie->সূচক_দৈর্ঘ্য;
    length-=2*trie->সূচক_দৈর্ঘ্য;

    /* get the data */
    যদি(options&UTRIE_OPTIONS_তথ্য_IS_32_BIT) {
        যদি(length<4*trie->তথ্য_দৈর্ঘ্য) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ফেরত -1;
        }
        trie->data32=(ধ্রুবক uint32_t *)p16;
        trie->initial_মান=trie->data32[0];
        length=(int32_t)আকার(UTrieHeader)+2*trie->সূচক_দৈর্ঘ্য+4*trie->তথ্য_দৈর্ঘ্য;
    } অন্যথা {
        যদি(length<2*trie->তথ্য_দৈর্ঘ্য) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            ফেরত -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=nullptr;
        trie->initial_মান=trie->সূচক[trie->সূচক_দৈর্ঘ্য];
        length=(int32_t)আকার(UTrieHeader)+2*trie->সূচক_দৈর্ঘ্য+2*trie->তথ্য_দৈর্ঘ্য;
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    ফেরত length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       শূন্য *তথ্য, int32_t length,
                       uint32_t initial_মান, uint32_t lead_Unit_মান,
                       UBool make16BitTrie,
                       UErrorCode *pErrorCode) {
    uint16_t *p16;
    int32_t actual_দৈর্ঘ্য, latin1_দৈর্ঘ্য, i, limit;
    uint16_t block;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_দৈর্ঘ্য= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->সূচক_দৈর্ঘ্য=UTRIE_BMP_সূচক_দৈর্ঘ্য+UTRIE_SURROGATE_BLOCK_গুনতি;
    trie->তথ্য_দৈর্ঘ্য=latin1_দৈর্ঘ্য;
    যদি(lead_Unit_মান!=initial_মান) {
        trie->তথ্য_দৈর্ঘ্য+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
    }

    actual_দৈর্ঘ্য=trie->সূচক_দৈর্ঘ্য*2;
    যদি(make16BitTrie) {
        actual_দৈর্ঘ্য+=trie->তথ্য_দৈর্ঘ্য*2;
    } অন্যথা {
        actual_দৈর্ঘ্য+=trie->তথ্য_দৈর্ঘ্য*4;
    }

    /* enough space for the dummy trie? */
    যদি(length<actual_দৈর্ঘ্য) {
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
        ফেরত actual_দৈর্ঘ্য;
    }

    trie->isLatin1Linear=সত্য;
    trie->initial_মান=initial_মান;

    /* fill the index and data arrays */
    p16=(uint16_t *)তথ্য;
    trie->সূচক=p16;

    যদি(make16BitTrie) {
        /* indexes to block 0 */
        block=(uint16_t)(trie->সূচক_দৈর্ঘ্য>>UTRIE_সূচক_SHIFT);
        limit=trie->সূচক_দৈর্ঘ্য;
        জন্য(i=0; i<limit; ++i) {
            p16[i]=block;
        }

        যদি(lead_Unit_মান!=initial_মান) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block+=(uint16_t)(latin1_দৈর্ঘ্য>>UTRIE_সূচক_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            জন্য(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=nullptr;

        /* Latin-1 data */
        p16+=trie->সূচক_দৈর্ঘ্য;
        জন্য(i=0; i<latin1_দৈর্ঘ্য; ++i) {
            p16[i]=(uint16_t)initial_মান;
        }

        /* data for lead surrogate code units */
        যদি(lead_Unit_মান!=initial_মান) {
            limit=latin1_দৈর্ঘ্য+UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
            জন্য(/* i=latin1Length */; i<limit; ++i) {
                p16[i]=(uint16_t)lead_Unit_মান;
            }
        }
    } অন্যথা {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->সূচক_দৈর্ঘ্য*2);

        যদি(lead_Unit_মান!=initial_মান) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block=(uint16_t)(latin1_দৈর্ঘ্য>>UTRIE_সূচক_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            জন্য(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->সূচক_দৈর্ঘ্য);

        /* Latin-1 data */
        জন্য(i=0; i<latin1_দৈর্ঘ্য; ++i) {
            p32[i]=initial_মান;
        }

        /* data for lead surrogate code units */
        যদি(lead_Unit_মান!=initial_মান) {
            limit=latin1_দৈর্ঘ্য+UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
            জন্য(/* i=latin1Length */; i<limit; ++i) {
                p32[i]=lead_Unit_মান;
            }
        }
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    ফেরত actual_দৈর্ঘ্য;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
স্থির uint32_t U_CALLCONV
enum_Same_মান(ধ্রুবক শূন্য * /*context*/, uint32_t value) {
    ফেরত value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI শূন্য U_EXPORT2
utrie_enum(ধ্রুবক UTrie *trie,
           UTrie_Enum_মান *enum_মান, UTrieEnumRange *enumRange, ধ্রুবক শূন্য *context) {
    ধ্রুবক uint32_t *data32;
    ধ্রুবক uint16_t *idx;

    uint32_t value, পূর্ব_মান, initial_মান;
    UChar32 c, পূর্ব;
    int32_t l, i, j, block, পূর্ব_Block, nullBlock, offset;

    /* check arguments */
    যদি(trie==nullptr || trie->সূচক==nullptr || enumRange==nullptr) {
        ফেরত;
    }
    যদি(enum_মান==nullptr) {
        enum_মান=enum_Same_মান;
    }

    idx=trie->সূচক;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_মান=enum_মান(context, trie->initial_মান);

    যদি(data32==nullptr) {
        nullBlock=trie->সূচক_দৈর্ঘ্য;
    } অন্যথা {
        nullBlock=0;
    }

    /* set variables for previous range */
    পূর্ব_Block=nullBlock;
    পূর্ব=0;
    পূর্ব_মান=initial_মান;

    /* enumerate BMP - the main loop enumerates data blocks */
    জন্য(i=0, c=0; c<=0xffff; ++i) {
        যদি(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_সূচক_দৈর্ঘ্য;
        } অন্যথা যদি(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_SHIFT;
        }

        block=idx[i]<<UTRIE_সূচক_SHIFT;
        যদি(block==পূর্ব_Block) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
        } অন্যথা যদি(block==nullBlock) {
            /* this is the all-initial-value block */
            যদি(পূর্ব_মান!=initial_মান) {
                যদি(পূর্ব<c) {
                    যদি(!enumRange(context, পূর্ব, c, পূর্ব_মান)) {
                        ফেরত;
                    }
                }
                পূর্ব_Block=nullBlock;
                পূর্ব=c;
                পূর্ব_মান=initial_মান;
            }
            c+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
        } অন্যথা {
            পূর্ব_Block=block;
            জন্য(j=0; j<UTRIE_তথ্য_BLOCK_দৈর্ঘ্য; ++j) {
                value=enum_মান(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                যদি(value!=পূর্ব_মান) {
                    যদি(পূর্ব<c) {
                        যদি(!enumRange(context, পূর্ব, c, পূর্ব_মান)) {
                            ফেরত;
                        }
                    }
                    যদি(j>0) {
                        /* the block is not filled with all the same value */
                        পূর্ব_Block=-1;
                    }
                    পূর্ব=c;
                    পূর্ব_মান=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    জন্য(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        offset=idx[l>>UTRIE_SHIFT]<<UTRIE_সূচক_SHIFT;
        যদি(offset==nullBlock) {
            /* no entries for a whole block of lead surrogates */
            যদি(পূর্ব_মান!=initial_মান) {
                যদি(পূর্ব<c) {
                    যদি(!enumRange(context, পূর্ব, c, পূর্ব_মান)) {
                        ফেরত;
                    }
                }
                পূর্ব_Block=nullBlock;
                পূর্ব=c;
                পূর্ব_মান=initial_মান;
            }

            l+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
            c+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য<<10;
            চালু;
        }

        value= data32!=nullptr ? data32[offset+(l&UTRIE_MASK)] : idx[offset+(l&UTRIE_MASK)];

        /* enumerate trail surrogates for this lead surrogate */
        offset=trie->getFoldingOffset(value);
        যদি(offset<=0) {
            /* no data for this lead surrogate */
            যদি(পূর্ব_মান!=initial_মান) {
                যদি(পূর্ব<c) {
                    যদি(!enumRange(context, পূর্ব, c, পূর্ব_মান)) {
                        ফেরত;
                    }
                }
                পূর্ব_Block=nullBlock;
                পূর্ব=c;
                পূর্ব_মান=initial_মান;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } অন্যথা {
            /* enumerate code points for this lead surrogate */
            i=offset;
            offset+=UTRIE_SURROGATE_BLOCK_গুনতি;
            করো {
                /* copy of most of the body of the BMP loop */
                block=idx[i]<<UTRIE_সূচক_SHIFT;
                যদি(block==পূর্ব_Block) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
                } অন্যথা যদি(block==nullBlock) {
                    /* this is the all-initial-value block */
                    যদি(পূর্ব_মান!=initial_মান) {
                        যদি(পূর্ব<c) {
                            যদি(!enumRange(context, পূর্ব, c, পূর্ব_মান)) {
                                ফেরত;
                            }
                        }
                        পূর্ব_Block=nullBlock;
                        পূর্ব=c;
                        পূর্ব_মান=initial_মান;
                    }
                    c+=UTRIE_তথ্য_BLOCK_দৈর্ঘ্য;
                } অন্যথা {
                    পূর্ব_Block=block;
                    জন্য(j=0; j<UTRIE_তথ্য_BLOCK_দৈর্ঘ্য; ++j) {
                        value=enum_মান(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        যদি(value!=পূর্ব_মান) {
                            যদি(পূর্ব<c) {
                                যদি(!enumRange(context, পূর্ব, c, পূর্ব_মান)) {
                                    ফেরত;
                                }
                            }
                            যদি(j>0) {
                                /* the block is not filled with all the same value */
                                পূর্ব_Block=-1;
                            }
                            পূর্ব=c;
                            পূর্ব_মান=value;
                        }
                        ++c;
                    }
                }
            } যখন(++i<offset);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, পূর্ব, c, পূর্ব_মান);
}

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

#अगर_घोषित UTRIE_DEBUG
#   include <stdio.h>
#पूर्ण_अगर

#समावेश "unicode/utypes.h"
#समावेश "cmemory.h"
#समावेश "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#अघोषित ABS
#घोषणा ABS(x) ((x)>=0 ? (x) : -(x))

अटल अंतरभूत UBool
equal_uint32(स्थिर uint32_t *s, स्थिर uint32_t *t, int32_t length) {
    जबतक(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    वापस length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fillIn,
           uint32_t *alias_तथ्य, int32_t max_तथ्य_लंबाई,
           uint32_t initial_मूल्य, uint32_t lead_Unit_मूल्य,
           UBool latin1Linear) {
    UNewTrie *trie;
    int32_t i, j;

    अगर( max_तथ्य_लंबाई<UTRIE_तथ्य_BLOCK_लंबाई ||
        (latin1Linear && max_तथ्य_लंबाई<1024)
    ) {
        वापस nullptr;
    }

    अगर(fillIn!=nullptr) {
        trie=fillIn;
    } अन्यथा {
        trie=(UNewTrie *)uprv_malloc(माप(UNewTrie));
        अगर(trie==nullptr) {
            वापस nullptr;
        }
    }
    uprv_memset(trie, 0, माप(UNewTrie));
    trie->isAllocated = fillIn == nullptr;

    अगर(alias_तथ्य!=nullptr) {
        trie->तथ्य=alias_तथ्य;
        trie->is_तथ्य_Allocated=false;
    } अन्यथा {
        trie->तथ्य=(uint32_t *)uprv_malloc(max_तथ्य_लंबाई*4);
        अगर(trie->तथ्य==nullptr) {
            uprv_free(trie);
            वापस nullptr;
        }
        trie->is_तथ्य_Allocated=true;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_तथ्य_BLOCK_लंबाई;

    अगर(latin1Linear) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        करो {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->सूचकांक[i++]=j;
            j+=UTRIE_तथ्य_BLOCK_लंबाई;
        } जबतक(i<(256>>UTRIE_SHIFT));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->तथ्य_लंबाई=j;
    जबतक(j>0) {
        trie->तथ्य[--j]=initial_मूल्य;
    }

    trie->lead_Unit_मूल्य=lead_Unit_मूल्य;
    trie->सूचकांक_लंबाई=UTRIE_MAX_सूचकांक_लंबाई;
    trie->तथ्य_Capacity=max_तथ्य_लंबाई;
    trie->isLatin1Linear=latin1Linear;
    trie->isCompacted=false;
    वापस trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fillIn, स्थिर UNewTrie *other, uint32_t *alias_तथ्य, int32_t alias_तथ्य_Capacity) {
    UNewTrie *trie;
    UBool is_तथ्य_Allocated;

    /* do not clone if other is not valid or already compacted */
    अगर(other==nullptr || other->तथ्य==nullptr || other->isCompacted) {
        वापस nullptr;
    }

    /* clone data */
    अगर(alias_तथ्य!=nullptr && alias_तथ्य_Capacity>=other->तथ्य_Capacity) {
        is_तथ्य_Allocated=false;
    } अन्यथा {
        alias_तथ्य_Capacity=other->तथ्य_Capacity;
        alias_तथ्य=(uint32_t *)uprv_malloc(other->तथ्य_Capacity*4);
        अगर(alias_तथ्य==nullptr) {
            वापस nullptr;
        }
        is_तथ्य_Allocated=true;
    }

    trie=utrie_open(fillIn, alias_तथ्य, alias_तथ्य_Capacity,
                    other->तथ्य[0], other->lead_Unit_मूल्य,
                    other->isLatin1Linear);
    अगर(trie==nullptr) {
        uprv_free(alias_तथ्य);
    } अन्यथा {
        uprv_memcpy(trie->सूचकांक, other->सूचकांक, माप(trie->सूचकांक));
        uprv_memcpy(trie->तथ्य, other->तथ्य, (माप_प्रकार)other->तथ्य_लंबाई*4);
        trie->तथ्य_लंबाई=other->तथ्य_लंबाई;
        trie->is_तथ्य_Allocated=is_तथ्य_Allocated;
    }

    वापस trie;
}

U_CAPI व्योम U_EXPORT2
utrie_close(UNewTrie *trie) {
    अगर(trie!=nullptr) {
        अगर(trie->is_तथ्य_Allocated) {
            uprv_free(trie->तथ्य);
            trie->तथ्य=nullptr;
        }
        अगर(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_तथ्य(UNewTrie *trie, int32_t *p_लंबाई) {
    अगर(trie==nullptr || p_लंबाई==nullptr) {
        वापस nullptr;
    }

    *p_लंबाई=trie->तथ्य_लंबाई;
    वापस trie->तथ्य;
}

अटल int32_t
utrie_alloc_तथ्य_Block(UNewTrie *trie) {
    int32_t newBlock, new_शीर्ष;

    newBlock=trie->तथ्य_लंबाई;
    new_शीर्ष=newBlock+UTRIE_तथ्य_BLOCK_लंबाई;
    अगर(new_शीर्ष>trie->तथ्य_Capacity) {
        /* out of memory in the data array */
        वापस -1;
    }
    trie->तथ्य_लंबाई=new_शीर्ष;
    वापस newBlock;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
अटल int32_t
utrie_get_तथ्य_Block(UNewTrie *trie, UChar32 c) {
    int32_t सूचकांक_मूल्य, newBlock;

    c>>=UTRIE_SHIFT;
    सूचकांक_मूल्य=trie->सूचकांक[c];
    अगर(सूचकांक_मूल्य>0) {
        वापस सूचकांक_मूल्य;
    }

    /* allocate a new data block */
    newBlock=utrie_alloc_तथ्य_Block(trie);
    अगर(newBlock<0) {
        /* out of memory in the data array */
        वापस -1;
    }
    trie->सूचकांक[c]=newBlock;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->तथ्य+newBlock, trie->तथ्य-सूचकांक_मूल्य, 4*UTRIE_तथ्य_BLOCK_लंबाई);
    वापस newBlock;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    अगर(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        वापस false;
    }

    block=utrie_get_तथ्य_Block(trie, c);
    अगर(block<0) {
        वापस false;
    }

    trie->तथ्य[block+(c&UTRIE_MASK)]=value;
    वापस true;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *pInBlockZero) {
    int32_t block;

    /* valid, uncompacted trie and valid c? */
    अगर(trie==nullptr || trie->isCompacted || (uint32_t)c>0x10ffff) {
        अगर(pInBlockZero!=nullptr) {
            *pInBlockZero=true;
        }
        वापस 0;
    }

    block=trie->सूचकांक[c>>UTRIE_SHIFT];
    अगर(pInBlockZero!=nullptr) {
        *pInBlockZero = block == 0;
    }

    वापस trie->तथ्य[ABS(block)+(c&UTRIE_MASK)];
}

/**
 * @internal
 */
अटल व्योम
utrie_fillBlock(uint32_t *block, UChar32 start, UChar32 limit,
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
    अगर( trie==nullptr || trie->isCompacted ||
        (uint32_t)start>0x10ffff || (uint32_t)limit>0x110000 || start>limit
    ) {
        वापस false;
    }
    अगर(start==limit) {
        वापस true; /* nothing to do */
    }

    initial_मूल्य=trie->तथ्य[0];
    अगर(start&UTRIE_MASK) {
        UChar32 अग्र_Start;

        /* set partial block at [start..following block boundary[ */
        block=utrie_get_तथ्य_Block(trie, start);
        अगर(block<0) {
            वापस false;
        }

        अग्र_Start=(start+UTRIE_तथ्य_BLOCK_लंबाई)&~UTRIE_MASK;
        अगर(अग्र_Start<=limit) {
            utrie_fillBlock(trie->तथ्य+block, start&UTRIE_MASK, UTRIE_तथ्य_BLOCK_लंबाई,
                            value, initial_मूल्य, overwrite);
            start=अग्र_Start;
        } अन्यथा {
            utrie_fillBlock(trie->तथ्य+block, start&UTRIE_MASK, limit&UTRIE_MASK,
                            value, initial_मूल्य, overwrite);
            वापस true;
        }
    }

    /* number of positions in the last, partial block */
    rest=limit&UTRIE_MASK;

    /* round down limit to a block boundary */
    limit&=~UTRIE_MASK;

    /* iterate over all-value blocks */
    अगर(value==initial_मूल्य) {
        repeatBlock=0;
    } अन्यथा {
        repeatBlock=-1;
    }
    जबतक(start<limit) {
        /* get index value */
        block=trie->सूचकांक[start>>UTRIE_SHIFT];
        अगर(block>0) {
            /* already allocated, fill in value */
            utrie_fillBlock(trie->तथ्य+block, 0, UTRIE_तथ्य_BLOCK_लंबाई, value, initial_मूल्य, overwrite);
        } अन्यथा अगर(trie->तथ्य[-block]!=value && (block==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            अगर(repeatBlock>=0) {
                trie->सूचकांक[start>>UTRIE_SHIFT]=-repeatBlock;
            } अन्यथा {
                /* create and set and fill the repeatBlock */
                repeatBlock=utrie_get_तथ्य_Block(trie, start);
                अगर(repeatBlock<0) {
                    वापस false;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->सूचकांक[start>>UTRIE_SHIFT]=-repeatBlock;
                utrie_fillBlock(trie->तथ्य+repeatBlock, 0, UTRIE_तथ्य_BLOCK_लंबाई, value, initial_मूल्य, true);
            }
        }

        start+=UTRIE_तथ्य_BLOCK_लंबाई;
    }

    अगर(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        block=utrie_get_तथ्य_Block(trie, start);
        अगर(block<0) {
            वापस false;
        }

        utrie_fillBlock(trie->तथ्य+block, 0, rest, value, initial_मूल्य, overwrite);
    }

    वापस true;
}

अटल int32_t
find_Same_सूचकांक_Block(स्थिर int32_t *idx, int32_t सूचकांक_लंबाई,
                    int32_t otherBlock) {
    int32_t block, i;

    क्रम(block=UTRIE_BMP_सूचकांक_लंबाई; block<सूचकांक_लंबाई; block+=UTRIE_SURROGATE_BLOCK_गिनती) {
        क्रम(i=0; i<UTRIE_SURROGATE_BLOCK_गिनती; ++i) {
            अगर(idx[block+i]!=idx[otherBlock+i]) {
                अवरोध;
            }
        }
        अगर(i==UTRIE_SURROGATE_BLOCK_गिनती) {
            वापस block;
        }
    }
    वापस सूचकांक_लंबाई;
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
अटल व्योम
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_मूल्य *get_Folded_मूल्य, UErrorCode *pErrorCode) {
    int32_t leadIndexes[UTRIE_SURROGATE_BLOCK_गिनती];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t सूचकांक_लंबाई, block;
#अगर_घोषित UTRIE_DEBUG
    पूर्णांक गिनती_Lead_CUWith_तथ्य=0;
#पूर्ण_अगर

    idx=trie->सूचकांक;

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
    अगर(trie->lead_Unit_मूल्य==trie->तथ्य[0]) {
        block=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } अन्यथा {
        /* create and fill the repeatBlock */
        block=utrie_alloc_तथ्य_Block(trie);
        अगर(block<0) {
            /* data table overflow */
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        utrie_fillBlock(trie->तथ्य+block, 0, UTRIE_तथ्य_BLOCK_लंबाई, trie->lead_Unit_मूल्य, trie->तथ्य[0], true);
        block=-block; /* negative block number to indicate that it is a repeat block */
    }
    क्रम(c=(0xd800>>UTRIE_SHIFT); c<(0xdc00>>UTRIE_SHIFT); ++c) {
        trie->सूचकांक[c]=block;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    सूचकांक_लंबाई=UTRIE_BMP_सूचकांक_लंबाई;

    /* search for any index (stage 1) entries for supplementary code points */
    क्रम(c=0x10000; c<0x110000;) {
        अगर(idx[c>>UTRIE_SHIFT]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#अगर_घोषित UTRIE_DEBUG
            ++गिनती_Lead_CUWith_तथ्य;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#पूर्ण_अगर

            /* is there an identical index block? */
            block=find_Same_सूचकांक_Block(idx, सूचकांक_लंबाई, c>>UTRIE_SHIFT);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_मूल्य(trie, c, block+UTRIE_SURROGATE_BLOCK_गिनती);
            अगर(value!=utrie_get32(trie, U16_LEAD(c), nullptr)) {
                अगर(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    वापस;
                }

                /* if we did not find an identical index block... */
                अगर(block==सूचकांक_लंबाई) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+सूचकांक_लंबाई,
                                 idx+(c>>UTRIE_SHIFT),
                                 4*UTRIE_SURROGATE_BLOCK_गिनती);
                    सूचकांक_लंबाई+=UTRIE_SURROGATE_BLOCK_गिनती;
                }
            }
            c+=0x400;
        } अन्यथा {
            c+=UTRIE_तथ्य_BLOCK_लंबाई;
        }
    }
#अगर_घोषित UTRIE_DEBUG
    अगर(गिनती_Lead_CUWith_तथ्य>0) {
        म_लिखो("supplementary data for %d lead surrogates\n", गिनती_Lead_CUWith_तथ्य);
    }
#पूर्ण_अगर

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    अगर(सूचकांक_लंबाई>=UTRIE_MAX_सूचकांक_लंबाई) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_सूचकांक_लंबाई+UTRIE_SURROGATE_BLOCK_गिनती,
                 idx+UTRIE_BMP_सूचकांक_लंबाई,
                 4*(सूचकांक_लंबाई-UTRIE_BMP_सूचकांक_लंबाई));
    uprv_memcpy(idx+UTRIE_BMP_सूचकांक_लंबाई,
                leadIndexes,
                4*UTRIE_SURROGATE_BLOCK_गिनती);
    सूचकांक_लंबाई+=UTRIE_SURROGATE_BLOCK_गिनती;

#अगर_घोषित UTRIE_DEBUG
    म_लिखो("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_सूचकांक_लंबाई, (दीर्घ)UTRIE_MAX_सूचकांक_लंबाई, सूचकांक_लंबाई);
#पूर्ण_अगर

    trie->सूचकांक_लंबाई=सूचकांक_लंबाई;
}

/*
 * Set a value in the trie index map to indicate which data block
 * is referenced and which one is not.
 * utrie_compact() will remove data blocks that are not used at all.
 * Set
 * - 0 if it is used
 * - -1 if it is not used
 */
अटल व्योम
_findUnusedBlocks(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_TIME_तथ्य_लंबाई>>UTRIE_SHIFT)*4);

    /* mark each block that _is_ used with 0 */
    क्रम(i=0; i<trie->सूचकांक_लंबाई; ++i) {
        trie->map[ABS(trie->सूचकांक[i])>>UTRIE_SHIFT]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

अटल int32_t
find_Same_तथ्य_Block(स्थिर uint32_t *तथ्य, int32_t तथ्य_लंबाई,
                   int32_t otherBlock, int32_t step) {
    int32_t block;

    /* ensure that we do not even partially get past dataLength */
    तथ्य_लंबाई-=UTRIE_तथ्य_BLOCK_लंबाई;

    क्रम(block=0; block<=तथ्य_लंबाई; block+=step) {
        अगर(equal_uint32(तथ्य+block, तथ्य+otherBlock, UTRIE_तथ्य_BLOCK_लंबाई)) {
            वापस block;
        }
    }
    वापस -1;
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
अटल व्योम
utrie_compact(UNewTrie *trie, UBool overlap, UErrorCode *pErrorCode) {
    int32_t i, start, newStart, overlapStart;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस;
    }

    /* valid, uncompacted trie? */
    अगर(trie==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर(trie->isCompacted) {
        वापस; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    _findUnusedBlocks(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    अगर(trie->isLatin1Linear && UTRIE_SHIFT<=8) {
        overlapStart=UTRIE_तथ्य_BLOCK_लंबाई+256;
    } अन्यथा {
        overlapStart=UTRIE_तथ्य_BLOCK_लंबाई;
    }

    newStart=UTRIE_तथ्य_BLOCK_लंबाई;
    क्रम(start=newStart; start<trie->तथ्य_लंबाई;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        अगर(trie->map[start>>UTRIE_SHIFT]<0) {
            /* advance start to the next block */
            start+=UTRIE_तथ्य_BLOCK_लंबाई;

            /* leave newStart with the previous block! */
            जारी;
        }

        /* search for an identical block */
        अगर( start>=overlapStart &&
            (i=find_Same_तथ्य_Block(trie->तथ्य, newStart, start,
                            overlap ? UTRIE_तथ्य_GRANULARITY : UTRIE_तथ्य_BLOCK_लंबाई))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE_SHIFT]=i;

            /* advance start to the next block */
            start+=UTRIE_तथ्य_BLOCK_लंबाई;

            /* leave newStart with the previous block! */
            जारी;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        अगर(overlap && start>=overlapStart) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            क्रम(i=UTRIE_तथ्य_BLOCK_लंबाई-UTRIE_तथ्य_GRANULARITY;
                i>0 && !equal_uint32(trie->तथ्य+(newStart-i), trie->तथ्य+start, i);
                i-=UTRIE_तथ्य_GRANULARITY) {}
        } अन्यथा {
            i=0;
        }

        अगर(i>0) {
            /* some overlap */
            trie->map[start>>UTRIE_SHIFT]=newStart-i;

            /* move the non-overlapping indexes to their new positions */
            start+=i;
            क्रम(i=UTRIE_तथ्य_BLOCK_लंबाई-i; i>0; --i) {
                trie->तथ्य[newStart++]=trie->तथ्य[start++];
            }
        } अन्यथा अगर(newStart<start) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[start>>UTRIE_SHIFT]=newStart;
            क्रम(i=UTRIE_तथ्य_BLOCK_लंबाई; i>0; --i) {
                trie->तथ्य[newStart++]=trie->तथ्य[start++];
            }
        } अन्यथा /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE_SHIFT]=start;
            newStart+=UTRIE_तथ्य_BLOCK_लंबाई;
            start=newStart;
        }
    }

    /* now adjust the index (stage 1) table */
    क्रम(i=0; i<trie->सूचकांक_लंबाई; ++i) {
        trie->सूचकांक[i]=trie->map[ABS(trie->सूचकांक[i])>>UTRIE_SHIFT];
    }

#अगर_घोषित UTRIE_DEBUG
    /* we saved some space */
    म_लिखो("compacting trie: count of 32-bit words %lu->%lu\n",
            (दीर्घ)trie->तथ्य_लंबाई, (दीर्घ)newStart);
#पूर्ण_अगर

    trie->तथ्य_लंबाई=newStart;
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
अटल uint32_t U_CALLCONV
default_Get_Folded_मूल्य(UNewTrie *trie, UChar32 start, int32_t offset) {
    uint32_t value, initial_मूल्य;
    UChar32 limit;
    UBool inBlockZero;

    initial_मूल्य=trie->तथ्य[0];
    limit=start+0x400;
    जबतक(start<limit) {
        value=utrie_get32(trie, start, &inBlockZero);
        अगर(inBlockZero) {
            start+=UTRIE_तथ्य_BLOCK_लंबाई;
        } अन्यथा अगर(value!=initial_मूल्य) {
            वापस static_cast<uint32_t>(offset);
        } अन्यथा {
            ++start;
        }
    }
    वापस 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, व्योम *dt, int32_t capacity,
                UNew_Trie_Get_Folded_मूल्य *get_Folded_मूल्य,
                UBool reduceTo16Bits,
                UErrorCode *pErrorCode) {
    UTrieHeader *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* तथ्य = nullptr;

    /* argument check */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    अगर(trie==nullptr || capacity<0 || (capacity>0 && dt==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    अगर(get_Folded_मूल्य==nullptr) {
        get_Folded_मूल्य=default_Get_Folded_मूल्य;
    }

    तथ्य = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    अगर(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, false, pErrorCode);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_मूल्य, pErrorCode);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, true, pErrorCode);

        trie->isCompacted=true;
        अगर(U_FAILURE(*pErrorCode)) {
            वापस 0;
        }
    }

    /* is dataLength within limits? */
    अगर( (reduceTo16Bits ? (trie->तथ्य_लंबाई+trie->सूचकांक_लंबाई) : trie->तथ्य_लंबाई) >= UTRIE_MAX_तथ्य_लंबाई) {
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
    }

    length=माप(UTrieHeader)+2*trie->सूचकांक_लंबाई;
    अगर(reduceTo16Bits) {
        length+=2*trie->तथ्य_लंबाई;
    } अन्यथा {
        length+=4*trie->तथ्य_लंबाई;
    }

    अगर(length>capacity) {
        वापस length; /* preflighting */
    }

#अगर_घोषित UTRIE_DEBUG
    म_लिखो("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (दीर्घ)trie->सूचकांक_लंबाई, (दीर्घ)trie->तथ्य_लंबाई, (दीर्घ)length);
#पूर्ण_अगर

    /* set the header fields */
    header=(UTrieHeader *)तथ्य;
    तथ्य+=माप(UTrieHeader);

    header->signature=0x54726965; /* "Trie" */
    header->options=UTRIE_SHIFT | (UTRIE_सूचकांक_SHIFT<<UTRIE_OPTIONS_सूचकांक_SHIFT);

    अगर(!reduceTo16Bits) {
        header->options|=UTRIE_OPTIONS_तथ्य_IS_32_BIT;
    }
    अगर(trie->isLatin1Linear) {
        header->options|=UTRIE_OPTIONS_LATIN1_IS_LINEAR;
    }

    header->सूचकांक_लंबाई=trie->सूचकांक_लंबाई;
    header->तथ्य_लंबाई=trie->तथ्य_लंबाई;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    अगर(reduceTo16Bits) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->सूचकांक;
        dest16=(uint16_t *)तथ्य;
        क्रम(i=trie->सूचकांक_लंबाई; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->सूचकांक_लंबाई)>>UTRIE_सूचकांक_SHIFT);
        }

        /* write 16-bit data values */
        p=trie->तथ्य;
        क्रम(i=trie->तथ्य_लंबाई; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } अन्यथा {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->सूचकांक;
        dest16=(uint16_t *)तथ्य;
        क्रम(i=trie->सूचकांक_लंबाई; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_सूचकांक_SHIFT);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->तथ्य, 4*(माप_प्रकार)trie->तथ्य_लंबाई);
    }

    वापस length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_defaultGetFoldingOffset(uint32_t तथ्य) {
    वापस (int32_t)तथ्य;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, स्थिर व्योम *तथ्य, int32_t length, UErrorCode *pErrorCode) {
    स्थिर UTrieHeader *header;
    स्थिर uint16_t *p16;
    uint32_t options;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस -1;
    }

    /* enough data for a trie header? */
    अगर(length<(int32_t)माप(UTrieHeader)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस -1;
    }

    /* check the signature */
    header=(स्थिर UTrieHeader *)तथ्य;
    अगर(header->signature!=0x54726965) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस -1;
    }

    /* get the options and check the shift values */
    options=header->options;
    अगर( (options&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_SHIFT ||
        ((options>>UTRIE_OPTIONS_सूचकांक_SHIFT)&UTRIE_OPTIONS_SHIFT_MASK)!=UTRIE_सूचकांक_SHIFT
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस -1;
    }
    trie->isLatin1Linear = (options & UTRIE_OPTIONS_LATIN1_IS_LINEAR) != 0;

    /* get the length values */
    trie->सूचकांक_लंबाई=header->सूचकांक_लंबाई;
    trie->तथ्य_लंबाई=header->तथ्य_लंबाई;

    length-=(int32_t)माप(UTrieHeader);

    /* enough data for the index? */
    अगर(length<2*trie->सूचकांक_लंबाई) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस -1;
    }
    p16=(स्थिर uint16_t *)(header+1);
    trie->सूचकांक=p16;
    p16+=trie->सूचकांक_लंबाई;
    length-=2*trie->सूचकांक_लंबाई;

    /* get the data */
    अगर(options&UTRIE_OPTIONS_तथ्य_IS_32_BIT) {
        अगर(length<4*trie->तथ्य_लंबाई) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            वापस -1;
        }
        trie->data32=(स्थिर uint32_t *)p16;
        trie->initial_मूल्य=trie->data32[0];
        length=(int32_t)माप(UTrieHeader)+2*trie->सूचकांक_लंबाई+4*trie->तथ्य_लंबाई;
    } अन्यथा {
        अगर(length<2*trie->तथ्य_लंबाई) {
            *pErrorCode=U_INVALID_FORMAT_ERROR;
            वापस -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=nullptr;
        trie->initial_मूल्य=trie->सूचकांक[trie->सूचकांक_लंबाई];
        length=(int32_t)माप(UTrieHeader)+2*trie->सूचकांक_लंबाई+2*trie->तथ्य_लंबाई;
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    वापस length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       व्योम *तथ्य, int32_t length,
                       uint32_t initial_मूल्य, uint32_t lead_Unit_मूल्य,
                       UBool make16BitTrie,
                       UErrorCode *pErrorCode) {
    uint16_t *p16;
    int32_t actual_लंबाई, latin1_लंबाई, i, limit;
    uint16_t block;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_लंबाई= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->सूचकांक_लंबाई=UTRIE_BMP_सूचकांक_लंबाई+UTRIE_SURROGATE_BLOCK_गिनती;
    trie->तथ्य_लंबाई=latin1_लंबाई;
    अगर(lead_Unit_मूल्य!=initial_मूल्य) {
        trie->तथ्य_लंबाई+=UTRIE_तथ्य_BLOCK_लंबाई;
    }

    actual_लंबाई=trie->सूचकांक_लंबाई*2;
    अगर(make16BitTrie) {
        actual_लंबाई+=trie->तथ्य_लंबाई*2;
    } अन्यथा {
        actual_लंबाई+=trie->तथ्य_लंबाई*4;
    }

    /* enough space for the dummy trie? */
    अगर(length<actual_लंबाई) {
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
        वापस actual_लंबाई;
    }

    trie->isLatin1Linear=true;
    trie->initial_मूल्य=initial_मूल्य;

    /* fill the index and data arrays */
    p16=(uint16_t *)तथ्य;
    trie->सूचकांक=p16;

    अगर(make16BitTrie) {
        /* indexes to block 0 */
        block=(uint16_t)(trie->सूचकांक_लंबाई>>UTRIE_सूचकांक_SHIFT);
        limit=trie->सूचकांक_लंबाई;
        क्रम(i=0; i<limit; ++i) {
            p16[i]=block;
        }

        अगर(lead_Unit_मूल्य!=initial_मूल्य) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block+=(uint16_t)(latin1_लंबाई>>UTRIE_सूचकांक_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            क्रम(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=nullptr;

        /* Latin-1 data */
        p16+=trie->सूचकांक_लंबाई;
        क्रम(i=0; i<latin1_लंबाई; ++i) {
            p16[i]=(uint16_t)initial_मूल्य;
        }

        /* data for lead surrogate code units */
        अगर(lead_Unit_मूल्य!=initial_मूल्य) {
            limit=latin1_लंबाई+UTRIE_तथ्य_BLOCK_लंबाई;
            क्रम(/* i=latin1Length */; i<limit; ++i) {
                p16[i]=(uint16_t)lead_Unit_मूल्य;
            }
        }
    } अन्यथा {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->सूचकांक_लंबाई*2);

        अगर(lead_Unit_मूल्य!=initial_मूल्य) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            block=(uint16_t)(latin1_लंबाई>>UTRIE_सूचकांक_SHIFT);
            i=0xd800>>UTRIE_SHIFT;
            limit=0xdc00>>UTRIE_SHIFT;
            क्रम(; i<limit; ++i) {
                p16[i]=block;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->सूचकांक_लंबाई);

        /* Latin-1 data */
        क्रम(i=0; i<latin1_लंबाई; ++i) {
            p32[i]=initial_मूल्य;
        }

        /* data for lead surrogate code units */
        अगर(lead_Unit_मूल्य!=initial_मूल्य) {
            limit=latin1_लंबाई+UTRIE_तथ्य_BLOCK_लंबाई;
            क्रम(/* i=latin1Length */; i<limit; ++i) {
                p32[i]=lead_Unit_मूल्य;
            }
        }
    }

    trie->getFoldingOffset=utrie_defaultGetFoldingOffset;

    वापस actual_लंबाई;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
अटल uint32_t U_CALLCONV
enum_Same_मूल्य(स्थिर व्योम * /*context*/, uint32_t value) {
    वापस value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI व्योम U_EXPORT2
utrie_enum(स्थिर UTrie *trie,
           UTrie_Enum_मूल्य *enum_मूल्य, UTrieEnumRange *enumRange, स्थिर व्योम *context) {
    स्थिर uint32_t *data32;
    स्थिर uint16_t *idx;

    uint32_t value, पूर्व_मूल्य, initial_मूल्य;
    UChar32 c, पूर्व;
    int32_t l, i, j, block, पूर्व_Block, nullBlock, offset;

    /* check arguments */
    अगर(trie==nullptr || trie->सूचकांक==nullptr || enumRange==nullptr) {
        वापस;
    }
    अगर(enum_मूल्य==nullptr) {
        enum_मूल्य=enum_Same_मूल्य;
    }

    idx=trie->सूचकांक;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_मूल्य=enum_मूल्य(context, trie->initial_मूल्य);

    अगर(data32==nullptr) {
        nullBlock=trie->सूचकांक_लंबाई;
    } अन्यथा {
        nullBlock=0;
    }

    /* set variables for previous range */
    पूर्व_Block=nullBlock;
    पूर्व=0;
    पूर्व_मूल्य=initial_मूल्य;

    /* enumerate BMP - the main loop enumerates data blocks */
    क्रम(i=0, c=0; c<=0xffff; ++i) {
        अगर(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_सूचकांक_लंबाई;
        } अन्यथा अगर(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_SHIFT;
        }

        block=idx[i]<<UTRIE_सूचकांक_SHIFT;
        अगर(block==पूर्व_Block) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_तथ्य_BLOCK_लंबाई;
        } अन्यथा अगर(block==nullBlock) {
            /* this is the all-initial-value block */
            अगर(पूर्व_मूल्य!=initial_मूल्य) {
                अगर(पूर्व<c) {
                    अगर(!enumRange(context, पूर्व, c, पूर्व_मूल्य)) {
                        वापस;
                    }
                }
                पूर्व_Block=nullBlock;
                पूर्व=c;
                पूर्व_मूल्य=initial_मूल्य;
            }
            c+=UTRIE_तथ्य_BLOCK_लंबाई;
        } अन्यथा {
            पूर्व_Block=block;
            क्रम(j=0; j<UTRIE_तथ्य_BLOCK_लंबाई; ++j) {
                value=enum_मूल्य(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                अगर(value!=पूर्व_मूल्य) {
                    अगर(पूर्व<c) {
                        अगर(!enumRange(context, पूर्व, c, पूर्व_मूल्य)) {
                            वापस;
                        }
                    }
                    अगर(j>0) {
                        /* the block is not filled with all the same value */
                        पूर्व_Block=-1;
                    }
                    पूर्व=c;
                    पूर्व_मूल्य=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    क्रम(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        offset=idx[l>>UTRIE_SHIFT]<<UTRIE_सूचकांक_SHIFT;
        अगर(offset==nullBlock) {
            /* no entries for a whole block of lead surrogates */
            अगर(पूर्व_मूल्य!=initial_मूल्य) {
                अगर(पूर्व<c) {
                    अगर(!enumRange(context, पूर्व, c, पूर्व_मूल्य)) {
                        वापस;
                    }
                }
                पूर्व_Block=nullBlock;
                पूर्व=c;
                पूर्व_मूल्य=initial_मूल्य;
            }

            l+=UTRIE_तथ्य_BLOCK_लंबाई;
            c+=UTRIE_तथ्य_BLOCK_लंबाई<<10;
            जारी;
        }

        value= data32!=nullptr ? data32[offset+(l&UTRIE_MASK)] : idx[offset+(l&UTRIE_MASK)];

        /* enumerate trail surrogates for this lead surrogate */
        offset=trie->getFoldingOffset(value);
        अगर(offset<=0) {
            /* no data for this lead surrogate */
            अगर(पूर्व_मूल्य!=initial_मूल्य) {
                अगर(पूर्व<c) {
                    अगर(!enumRange(context, पूर्व, c, पूर्व_मूल्य)) {
                        वापस;
                    }
                }
                पूर्व_Block=nullBlock;
                पूर्व=c;
                पूर्व_मूल्य=initial_मूल्य;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } अन्यथा {
            /* enumerate code points for this lead surrogate */
            i=offset;
            offset+=UTRIE_SURROGATE_BLOCK_गिनती;
            करो {
                /* copy of most of the body of the BMP loop */
                block=idx[i]<<UTRIE_सूचकांक_SHIFT;
                अगर(block==पूर्व_Block) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_तथ्य_BLOCK_लंबाई;
                } अन्यथा अगर(block==nullBlock) {
                    /* this is the all-initial-value block */
                    अगर(पूर्व_मूल्य!=initial_मूल्य) {
                        अगर(पूर्व<c) {
                            अगर(!enumRange(context, पूर्व, c, पूर्व_मूल्य)) {
                                वापस;
                            }
                        }
                        पूर्व_Block=nullBlock;
                        पूर्व=c;
                        पूर्व_मूल्य=initial_मूल्य;
                    }
                    c+=UTRIE_तथ्य_BLOCK_लंबाई;
                } अन्यथा {
                    पूर्व_Block=block;
                    क्रम(j=0; j<UTRIE_तथ्य_BLOCK_लंबाई; ++j) {
                        value=enum_मूल्य(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        अगर(value!=पूर्व_मूल्य) {
                            अगर(पूर्व<c) {
                                अगर(!enumRange(context, पूर्व, c, पूर्व_मूल्य)) {
                                    वापस;
                                }
                            }
                            अगर(j>0) {
                                /* the block is not filled with all the same value */
                                पूर्व_Block=-1;
                            }
                            पूर्व=c;
                            पूर्व_मूल्य=value;
                        }
                        ++c;
                    }
                }
            } जबतक(++i<offset);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, पूर्व, c, पूर्व_मूल्य);
}

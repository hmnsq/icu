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

#اگر_تعریف_شده UTRIE_DEBUG
#   include <stdio.h>
#پایان_اگر

#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#لغو_تعریف ABS
#تعریف_کن ABS(x) ((x)>=0 ? (x) : -(x))

ساكن مضمن UBool
equal_uint32(ثابت uint32_t *s, ثابت uint32_t *t, int32_t length) {
    طالما(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    أرجع length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fill_درون,
           uint32_t *alias_داده, int32_t max_داده_درازی,
           uint32_t initial_مقدار, uint32_t lead_Unit_مقدار,
           UBool latin1_خطی) {
    UNewTrie *trie;
    int32_t i, j;

    إذا( max_داده_درازی<UTRIE_داده_بلوک_درازی ||
        (latin1_خطی && max_داده_درازی<1024)
    ) {
        أرجع اشاره_تهی;
    }

    إذا(fill_درون!=اشاره_تهی) {
        trie=fill_درون;
    } وإلا {
        trie=(UNewTrie *)uprv_malloc(حجم(UNewTrie));
        إذا(trie==اشاره_تهی) {
            أرجع اشاره_تهی;
        }
    }
    uprv_memset(trie, 0, حجم(UNewTrie));
    trie->isAllocated = fill_درون == اشاره_تهی;

    إذا(alias_داده!=اشاره_تهی) {
        trie->داده=alias_داده;
        trie->is_داده_Allocated=خطأ;
    } وإلا {
        trie->داده=(uint32_t *)uprv_malloc(max_داده_درازی*4);
        إذا(trie->داده==اشاره_تهی) {
            uprv_free(trie);
            أرجع اشاره_تهی;
        }
        trie->is_داده_Allocated=درست;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_داده_بلوک_درازی;

    إذا(latin1_خطی) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        افعل {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->شاخص[i++]=j;
            j+=UTRIE_داده_بلوک_درازی;
        } طالما(i<(256>>UTRIE_لغزش));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->داده_درازی=j;
    طالما(j>0) {
        trie->داده[--j]=initial_مقدار;
    }

    trie->lead_Unit_مقدار=lead_Unit_مقدار;
    trie->شاخص_درازی=UTRIE_MAX_شاخص_درازی;
    trie->داده_ظرفیت=max_داده_درازی;
    trie->is_Latin1_خطی=latin1_خطی;
    trie->isCompacted=خطأ;
    أرجع trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fill_درون, ثابت UNewTrie *other, uint32_t *alias_داده, int32_t alias_داده_ظرفیت) {
    UNewTrie *trie;
    UBool is_داده_Allocated;

    /* do not clone if other is not valid or already compacted */
    إذا(other==اشاره_تهی || other->داده==اشاره_تهی || other->isCompacted) {
        أرجع اشاره_تهی;
    }

    /* clone data */
    إذا(alias_داده!=اشاره_تهی && alias_داده_ظرفیت>=other->داده_ظرفیت) {
        is_داده_Allocated=خطأ;
    } وإلا {
        alias_داده_ظرفیت=other->داده_ظرفیت;
        alias_داده=(uint32_t *)uprv_malloc(other->داده_ظرفیت*4);
        إذا(alias_داده==اشاره_تهی) {
            أرجع اشاره_تهی;
        }
        is_داده_Allocated=درست;
    }

    trie=utrie_open(fill_درون, alias_داده, alias_داده_ظرفیت,
                    other->داده[0], other->lead_Unit_مقدار,
                    other->is_Latin1_خطی);
    إذا(trie==اشاره_تهی) {
        uprv_free(alias_داده);
    } وإلا {
        uprv_memcpy(trie->شاخص, other->شاخص, حجم(trie->شاخص));
        uprv_memcpy(trie->داده, other->داده, (size_t)other->داده_درازی*4);
        trie->داده_درازی=other->داده_درازی;
        trie->is_داده_Allocated=is_داده_Allocated;
    }

    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
utrie_close(UNewTrie *trie) {
    إذا(trie!=اشاره_تهی) {
        إذا(trie->is_داده_Allocated) {
            uprv_free(trie->داده);
            trie->داده=اشاره_تهی;
        }
        إذا(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_داده(UNewTrie *trie, int32_t *p_درازی) {
    إذا(trie==اشاره_تهی || p_درازی==اشاره_تهی) {
        أرجع اشاره_تهی;
    }

    *p_درازی=trie->داده_درازی;
    أرجع trie->داده;
}

ساكن int32_t
utrie_alloc_داده_بلوک(UNewTrie *trie) {
    int32_t new_بلوک, new_بالاترین;

    new_بلوک=trie->داده_درازی;
    new_بالاترین=new_بلوک+UTRIE_داده_بلوک_درازی;
    إذا(new_بالاترین>trie->داده_ظرفیت) {
        /* out of memory in the data array */
        أرجع -1;
    }
    trie->داده_درازی=new_بالاترین;
    أرجع new_بلوک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ساكن int32_t
utrie_get_داده_بلوک(UNewTrie *trie, UChar32 c) {
    int32_t شاخص_مقدار, new_بلوک;

    c>>=UTRIE_لغزش;
    شاخص_مقدار=trie->شاخص[c];
    إذا(شاخص_مقدار>0) {
        أرجع شاخص_مقدار;
    }

    /* allocate a new data block */
    new_بلوک=utrie_alloc_داده_بلوک(trie);
    إذا(new_بلوک<0) {
        /* out of memory in the data array */
        أرجع -1;
    }
    trie->شاخص[c]=new_بلوک;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->داده+new_بلوک, trie->داده-شاخص_مقدار, 4*UTRIE_داده_بلوک_درازی);
    أرجع new_بلوک;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t بلوک;

    /* valid, uncompacted trie and valid c? */
    إذا(trie==اشاره_تهی || trie->isCompacted || (uint32_t)c>0x10ffff) {
        أرجع خطأ;
    }

    بلوک=utrie_get_داده_بلوک(trie, c);
    إذا(بلوک<0) {
        أرجع خطأ;
    }

    trie->داده[بلوک+(c&UTRIE_ماسک)]=value;
    أرجع درست;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *p_درون_بلوک_Zero) {
    int32_t بلوک;

    /* valid, uncompacted trie and valid c? */
    إذا(trie==اشاره_تهی || trie->isCompacted || (uint32_t)c>0x10ffff) {
        إذا(p_درون_بلوک_Zero!=اشاره_تهی) {
            *p_درون_بلوک_Zero=درست;
        }
        أرجع 0;
    }

    بلوک=trie->شاخص[c>>UTRIE_لغزش];
    إذا(p_درون_بلوک_Zero!=اشاره_تهی) {
        *p_درون_بلوک_Zero = بلوک == 0;
    }

    أرجع trie->داده[ABS(بلوک)+(c&UTRIE_ماسک)];
}

/**
 * @internal
 */
ساكن فراغ
utrie_fill_بلوک(uint32_t *بلوک, UChar32 شروع, UChar32 حد,
                uint32_t value, uint32_t initial_مقدار, UBool overwrite) {
    uint32_t *p_حد;

    p_حد=بلوک+حد;
    بلوک+=شروع;
    إذا(overwrite) {
        طالما(بلوک<p_حد) {
            *بلوک++=value;
        }
    } وإلا {
        طالما(بلوک<p_حد) {
            إذا(*بلوک==initial_مقدار) {
                *بلوک=value;
            }
            ++بلوک;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_مجموعه_Range32(UNewTrie *trie, UChar32 شروع, UChar32 حد, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initial_مقدار;
    int32_t بلوک, rest, repeat_بلوک;

    /* valid, uncompacted trie and valid indexes? */
    إذا( trie==اشاره_تهی || trie->isCompacted ||
        (uint32_t)شروع>0x10ffff || (uint32_t)حد>0x110000 || شروع>حد
    ) {
        أرجع خطأ;
    }
    إذا(شروع==حد) {
        أرجع درست; /* nothing to do */
    }

    initial_مقدار=trie->داده[0];
    إذا(شروع&UTRIE_ماسک) {
        UChar32 بعدی_شروع;

        /* set partial block at [start..following block boundary[ */
        بلوک=utrie_get_داده_بلوک(trie, شروع);
        إذا(بلوک<0) {
            أرجع خطأ;
        }

        بعدی_شروع=(شروع+UTRIE_داده_بلوک_درازی)&~UTRIE_ماسک;
        إذا(بعدی_شروع<=حد) {
            utrie_fill_بلوک(trie->داده+بلوک, شروع&UTRIE_ماسک, UTRIE_داده_بلوک_درازی,
                            value, initial_مقدار, overwrite);
            شروع=بعدی_شروع;
        } وإلا {
            utrie_fill_بلوک(trie->داده+بلوک, شروع&UTRIE_ماسک, حد&UTRIE_ماسک,
                            value, initial_مقدار, overwrite);
            أرجع درست;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد&UTRIE_ماسک;

    /* round down limit to a block boundary */
    حد&=~UTRIE_ماسک;

    /* iterate over all-value blocks */
    إذا(value==initial_مقدار) {
        repeat_بلوک=0;
    } وإلا {
        repeat_بلوک=-1;
    }
    طالما(شروع<حد) {
        /* get index value */
        بلوک=trie->شاخص[شروع>>UTRIE_لغزش];
        إذا(بلوک>0) {
            /* already allocated, fill in value */
            utrie_fill_بلوک(trie->داده+بلوک, 0, UTRIE_داده_بلوک_درازی, value, initial_مقدار, overwrite);
        } وإلا إذا(trie->داده[-بلوک]!=value && (بلوک==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            إذا(repeat_بلوک>=0) {
                trie->شاخص[شروع>>UTRIE_لغزش]=-repeat_بلوک;
            } وإلا {
                /* create and set and fill the repeatBlock */
                repeat_بلوک=utrie_get_داده_بلوک(trie, شروع);
                إذا(repeat_بلوک<0) {
                    أرجع خطأ;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->شاخص[شروع>>UTRIE_لغزش]=-repeat_بلوک;
                utrie_fill_بلوک(trie->داده+repeat_بلوک, 0, UTRIE_داده_بلوک_درازی, value, initial_مقدار, درست);
            }
        }

        شروع+=UTRIE_داده_بلوک_درازی;
    }

    إذا(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلوک=utrie_get_داده_بلوک(trie, شروع);
        إذا(بلوک<0) {
            أرجع خطأ;
        }

        utrie_fill_بلوک(trie->داده+بلوک, 0, rest, value, initial_مقدار, overwrite);
    }

    أرجع درست;
}

ساكن int32_t
پیدا_کن_Same_شاخص_بلوک(ثابت int32_t *idx, int32_t شاخص_درازی,
                    int32_t other_بلوک) {
    int32_t بلوک, i;

    لكل(بلوک=UTRIE_BMP_شاخص_درازی; بلوک<شاخص_درازی; بلوک+=UTRIE_SURROGATE_بلوک_شمار) {
        لكل(i=0; i<UTRIE_SURROGATE_بلوک_شمار; ++i) {
            إذا(idx[بلوک+i]!=idx[other_بلوک+i]) {
                اكسر;
            }
        }
        إذا(i==UTRIE_SURROGATE_بلوک_شمار) {
            أرجع بلوک;
        }
    }
    أرجع شاخص_درازی;
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
ساكن فراغ
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_مقدار *get_Folded_مقدار, UError_کد *p_Error_کد) {
    int32_t leadIndexes[UTRIE_SURROGATE_بلوک_شمار];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t شاخص_درازی, بلوک;
#اگر_تعریف_شده UTRIE_DEBUG
    صحيح شمار_Lead_CUWith_داده=0;
#پایان_اگر

    idx=trie->شاخص;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_لغزش), 4*UTRIE_SURROGATE_بلوک_شمار);

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
    إذا(trie->lead_Unit_مقدار==trie->داده[0]) {
        بلوک=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } وإلا {
        /* create and fill the repeatBlock */
        بلوک=utrie_alloc_داده_بلوک(trie);
        إذا(بلوک<0) {
            /* data table overflow */
            *p_Error_کد=U_حافظه_تخصیص_ERROR;
            أرجع;
        }
        utrie_fill_بلوک(trie->داده+بلوک, 0, UTRIE_داده_بلوک_درازی, trie->lead_Unit_مقدار, trie->داده[0], درست);
        بلوک=-بلوک; /* negative block number to indicate that it is a repeat block */
    }
    لكل(c=(0xd800>>UTRIE_لغزش); c<(0xdc00>>UTRIE_لغزش); ++c) {
        trie->شاخص[c]=بلوک;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    شاخص_درازی=UTRIE_BMP_شاخص_درازی;

    /* search for any index (stage 1) entries for supplementary code points */
    لكل(c=0x10000; c<0x110000;) {
        إذا(idx[c>>UTRIE_لغزش]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#اگر_تعریف_شده UTRIE_DEBUG
            ++شمار_Lead_CUWith_داده;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#پایان_اگر

            /* is there an identical index block? */
            بلوک=پیدا_کن_Same_شاخص_بلوک(idx, شاخص_درازی, c>>UTRIE_لغزش);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_مقدار(trie, c, بلوک+UTRIE_SURROGATE_بلوک_شمار);
            إذا(value!=utrie_get32(trie, U16_LEAD(c), اشاره_تهی)) {
                إذا(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *p_Error_کد=U_حافظه_تخصیص_ERROR;
                    أرجع;
                }

                /* if we did not find an identical index block... */
                إذا(بلوک==شاخص_درازی) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+شاخص_درازی,
                                 idx+(c>>UTRIE_لغزش),
                                 4*UTRIE_SURROGATE_بلوک_شمار);
                    شاخص_درازی+=UTRIE_SURROGATE_بلوک_شمار;
                }
            }
            c+=0x400;
        } وإلا {
            c+=UTRIE_داده_بلوک_درازی;
        }
    }
#اگر_تعریف_شده UTRIE_DEBUG
    إذا(شمار_Lead_CUWith_داده>0) {
        چاپ_کن("supplementary data for %d lead surrogates\n", شمار_Lead_CUWith_داده);
    }
#پایان_اگر

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    إذا(شاخص_درازی>=UTRIE_MAX_شاخص_درازی) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_شاخص_درازی+UTRIE_SURROGATE_بلوک_شمار,
                 idx+UTRIE_BMP_شاخص_درازی,
                 4*(شاخص_درازی-UTRIE_BMP_شاخص_درازی));
    uprv_memcpy(idx+UTRIE_BMP_شاخص_درازی,
                leadIndexes,
                4*UTRIE_SURROGATE_بلوک_شمار);
    شاخص_درازی+=UTRIE_SURROGATE_بلوک_شمار;

#اگر_تعریف_شده UTRIE_DEBUG
    چاپ_کن("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_شاخص_درازی, (دراز)UTRIE_MAX_شاخص_درازی, شاخص_درازی);
#پایان_اگر

    trie->شاخص_درازی=شاخص_درازی;
}

/*
 * Set a value in the trie index map to indicate which data block
 * is referenced and which one is not.
 * utrie_compact() will remove data blocks that are not used at all.
 * Set
 * - 0 if it is used
 * - -1 if it is not used
 */
ساكن فراغ
پیدا_کن_استفادهنشده_بلوکها(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_زمان_داده_درازی>>UTRIE_لغزش)*4);

    /* mark each block that _is_ used with 0 */
    لكل(i=0; i<trie->شاخص_درازی; ++i) {
        trie->map[ABS(trie->شاخص[i])>>UTRIE_لغزش]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

ساكن int32_t
پیدا_کن_Same_داده_بلوک(ثابت uint32_t *داده, int32_t داده_درازی,
                   int32_t other_بلوک, int32_t گام) {
    int32_t بلوک;

    /* ensure that we do not even partially get past dataLength */
    داده_درازی-=UTRIE_داده_بلوک_درازی;

    لكل(بلوک=0; بلوک<=داده_درازی; بلوک+=گام) {
        إذا(equal_uint32(داده+بلوک, داده+other_بلوک, UTRIE_داده_بلوک_درازی)) {
            أرجع بلوک;
        }
    }
    أرجع -1;
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
ساكن فراغ
utrie_compact(UNewTrie *trie, UBool overlap, UError_کد *p_Error_کد) {
    int32_t i, شروع, new_شروع, overlap_شروع;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع;
    }

    /* valid, uncompacted trie? */
    إذا(trie==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    إذا(trie->isCompacted) {
        أرجع; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    پیدا_کن_استفادهنشده_بلوکها(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    إذا(trie->is_Latin1_خطی && UTRIE_لغزش<=8) {
        overlap_شروع=UTRIE_داده_بلوک_درازی+256;
    } وإلا {
        overlap_شروع=UTRIE_داده_بلوک_درازی;
    }

    new_شروع=UTRIE_داده_بلوک_درازی;
    لكل(شروع=new_شروع; شروع<trie->داده_درازی;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        إذا(trie->map[شروع>>UTRIE_لغزش]<0) {
            /* advance start to the next block */
            شروع+=UTRIE_داده_بلوک_درازی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* search for an identical block */
        إذا( شروع>=overlap_شروع &&
            (i=پیدا_کن_Same_داده_بلوک(trie->داده, new_شروع, شروع,
                            overlap ? UTRIE_داده_GRANULARITY : UTRIE_داده_بلوک_درازی))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[شروع>>UTRIE_لغزش]=i;

            /* advance start to the next block */
            شروع+=UTRIE_داده_بلوک_درازی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        إذا(overlap && شروع>=overlap_شروع) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            لكل(i=UTRIE_داده_بلوک_درازی-UTRIE_داده_GRANULARITY;
                i>0 && !equal_uint32(trie->داده+(new_شروع-i), trie->داده+شروع, i);
                i-=UTRIE_داده_GRANULARITY) {}
        } وإلا {
            i=0;
        }

        إذا(i>0) {
            /* some overlap */
            trie->map[شروع>>UTRIE_لغزش]=new_شروع-i;

            /* move the non-overlapping indexes to their new positions */
            شروع+=i;
            لكل(i=UTRIE_داده_بلوک_درازی-i; i>0; --i) {
                trie->داده[new_شروع++]=trie->داده[شروع++];
            }
        } وإلا إذا(new_شروع<شروع) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[شروع>>UTRIE_لغزش]=new_شروع;
            لكل(i=UTRIE_داده_بلوک_درازی; i>0; --i) {
                trie->داده[new_شروع++]=trie->داده[شروع++];
            }
        } وإلا /* no overlap && newStart==start */ {
            trie->map[شروع>>UTRIE_لغزش]=شروع;
            new_شروع+=UTRIE_داده_بلوک_درازی;
            شروع=new_شروع;
        }
    }

    /* now adjust the index (stage 1) table */
    لكل(i=0; i<trie->شاخص_درازی; ++i) {
        trie->شاخص[i]=trie->map[ABS(trie->شاخص[i])>>UTRIE_لغزش];
    }

#اگر_تعریف_شده UTRIE_DEBUG
    /* we saved some space */
    چاپ_کن("compacting trie: count of 32-bit words %lu->%lu\n",
            (دراز)trie->داده_درازی, (دراز)new_شروع);
#پایان_اگر

    trie->داده_درازی=new_شروع;
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
ساكن uint32_t U_CALLCONV
default_Get_Folded_مقدار(UNewTrie *trie, UChar32 شروع, int32_t جابه_جایی) {
    uint32_t value, initial_مقدار;
    UChar32 حد;
    UBool درون_بلوک_Zero;

    initial_مقدار=trie->داده[0];
    حد=شروع+0x400;
    طالما(شروع<حد) {
        value=utrie_get32(trie, شروع, &درون_بلوک_Zero);
        إذا(درون_بلوک_Zero) {
            شروع+=UTRIE_داده_بلوک_درازی;
        } وإلا إذا(value!=initial_مقدار) {
            أرجع static_cast<uint32_t>(جابه_جایی);
        } وإلا {
            ++شروع;
        }
    }
    أرجع 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, فراغ *dt, int32_t capacity,
                UNew_Trie_Get_Folded_مقدار *get_Folded_مقدار,
                UBool reduce_To16_بیتها,
                UError_کد *p_Error_کد) {
    UTrie_سرآیند *سرآیند;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* داده = اشاره_تهی;

    /* argument check */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    إذا(trie==اشاره_تهی || capacity<0 || (capacity>0 && dt==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    إذا(get_Folded_مقدار==اشاره_تهی) {
        get_Folded_مقدار=default_Get_Folded_مقدار;
    }

    داده = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    إذا(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, خطأ, p_Error_کد);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_مقدار, p_Error_کد);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, درست, p_Error_کد);

        trie->isCompacted=درست;
        إذا(U_FAILURE(*p_Error_کد)) {
            أرجع 0;
        }
    }

    /* is dataLength within limits? */
    إذا( (reduce_To16_بیتها ? (trie->داده_درازی+trie->شاخص_درازی) : trie->داده_درازی) >= UTRIE_MAX_داده_درازی) {
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
    }

    length=حجم(UTrie_سرآیند)+2*trie->شاخص_درازی;
    إذا(reduce_To16_بیتها) {
        length+=2*trie->داده_درازی;
    } وإلا {
        length+=4*trie->داده_درازی;
    }

    إذا(length>capacity) {
        أرجع length; /* preflighting */
    }

#اگر_تعریف_شده UTRIE_DEBUG
    چاپ_کن("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (دراز)trie->شاخص_درازی, (دراز)trie->داده_درازی, (دراز)length);
#پایان_اگر

    /* set the header fields */
    سرآیند=(UTrie_سرآیند *)داده;
    داده+=حجم(UTrie_سرآیند);

    سرآیند->signature=0x54726965; /* "Trie" */
    سرآیند->options=UTRIE_لغزش | (UTRIE_شاخص_لغزش<<UTRIE_OPTIONS_شاخص_لغزش);

    إذا(!reduce_To16_بیتها) {
        سرآیند->options|=UTRIE_OPTIONS_داده_IS_32_بیت;
    }
    إذا(trie->is_Latin1_خطی) {
        سرآیند->options|=UTRIE_OPTIONS_LATIN1_IS_خطی;
    }

    سرآیند->شاخص_درازی=trie->شاخص_درازی;
    سرآیند->داده_درازی=trie->داده_درازی;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    إذا(reduce_To16_بیتها) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->شاخص;
        dest16=(uint16_t *)داده;
        لكل(i=trie->شاخص_درازی; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->شاخص_درازی)>>UTRIE_شاخص_لغزش);
        }

        /* write 16-bit data values */
        p=trie->داده;
        لكل(i=trie->داده_درازی; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } وإلا {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->شاخص;
        dest16=(uint16_t *)داده;
        لكل(i=trie->شاخص_درازی; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_شاخص_لغزش);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->داده, 4*(size_t)trie->داده_درازی);
    }

    أرجع length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_default_Get_Folding_جابه_جایی(uint32_t داده) {
    أرجع (int32_t)داده;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ثابت فراغ *داده, int32_t length, UError_کد *p_Error_کد) {
    ثابت UTrie_سرآیند *سرآیند;
    ثابت uint16_t *p16;
    uint32_t options;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع -1;
    }

    /* enough data for a trie header? */
    إذا(length<(int32_t)حجم(UTrie_سرآیند)) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }

    /* check the signature */
    سرآیند=(ثابت UTrie_سرآیند *)داده;
    إذا(سرآیند->signature!=0x54726965) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }

    /* get the options and check the shift values */
    options=سرآیند->options;
    إذا( (options&UTRIE_OPTIONS_لغزش_ماسک)!=UTRIE_لغزش ||
        ((options>>UTRIE_OPTIONS_شاخص_لغزش)&UTRIE_OPTIONS_لغزش_ماسک)!=UTRIE_شاخص_لغزش
    ) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }
    trie->is_Latin1_خطی = (options & UTRIE_OPTIONS_LATIN1_IS_خطی) != 0;

    /* get the length values */
    trie->شاخص_درازی=سرآیند->شاخص_درازی;
    trie->داده_درازی=سرآیند->داده_درازی;

    length-=(int32_t)حجم(UTrie_سرآیند);

    /* enough data for the index? */
    إذا(length<2*trie->شاخص_درازی) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }
    p16=(ثابت uint16_t *)(سرآیند+1);
    trie->شاخص=p16;
    p16+=trie->شاخص_درازی;
    length-=2*trie->شاخص_درازی;

    /* get the data */
    إذا(options&UTRIE_OPTIONS_داده_IS_32_بیت) {
        إذا(length<4*trie->داده_درازی) {
            *p_Error_کد=U_INVALID_FORMAT_ERROR;
            أرجع -1;
        }
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_مقدار=trie->data32[0];
        length=(int32_t)حجم(UTrie_سرآیند)+2*trie->شاخص_درازی+4*trie->داده_درازی;
    } وإلا {
        إذا(length<2*trie->داده_درازی) {
            *p_Error_کد=U_INVALID_FORMAT_ERROR;
            أرجع -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=اشاره_تهی;
        trie->initial_مقدار=trie->شاخص[trie->شاخص_درازی];
        length=(int32_t)حجم(UTrie_سرآیند)+2*trie->شاخص_درازی+2*trie->داده_درازی;
    }

    trie->get_Folding_جابه_جایی=utrie_default_Get_Folding_جابه_جایی;

    أرجع length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       فراغ *داده, int32_t length,
                       uint32_t initial_مقدار, uint32_t lead_Unit_مقدار,
                       UBool make16_بیت_Trie,
                       UError_کد *p_Error_کد) {
    uint16_t *p16;
    int32_t actual_درازی, latin1_درازی, i, حد;
    uint16_t بلوک;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_درازی= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->شاخص_درازی=UTRIE_BMP_شاخص_درازی+UTRIE_SURROGATE_بلوک_شمار;
    trie->داده_درازی=latin1_درازی;
    إذا(lead_Unit_مقدار!=initial_مقدار) {
        trie->داده_درازی+=UTRIE_داده_بلوک_درازی;
    }

    actual_درازی=trie->شاخص_درازی*2;
    إذا(make16_بیت_Trie) {
        actual_درازی+=trie->داده_درازی*2;
    } وإلا {
        actual_درازی+=trie->داده_درازی*4;
    }

    /* enough space for the dummy trie? */
    إذا(length<actual_درازی) {
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
        أرجع actual_درازی;
    }

    trie->is_Latin1_خطی=درست;
    trie->initial_مقدار=initial_مقدار;

    /* fill the index and data arrays */
    p16=(uint16_t *)داده;
    trie->شاخص=p16;

    إذا(make16_بیت_Trie) {
        /* indexes to block 0 */
        بلوک=(uint16_t)(trie->شاخص_درازی>>UTRIE_شاخص_لغزش);
        حد=trie->شاخص_درازی;
        لكل(i=0; i<حد; ++i) {
            p16[i]=بلوک;
        }

        إذا(lead_Unit_مقدار!=initial_مقدار) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلوک+=(uint16_t)(latin1_درازی>>UTRIE_شاخص_لغزش);
            i=0xd800>>UTRIE_لغزش;
            حد=0xdc00>>UTRIE_لغزش;
            لكل(; i<حد; ++i) {
                p16[i]=بلوک;
            }
        }

        trie->data32=اشاره_تهی;

        /* Latin-1 data */
        p16+=trie->شاخص_درازی;
        لكل(i=0; i<latin1_درازی; ++i) {
            p16[i]=(uint16_t)initial_مقدار;
        }

        /* data for lead surrogate code units */
        إذا(lead_Unit_مقدار!=initial_مقدار) {
            حد=latin1_درازی+UTRIE_داده_بلوک_درازی;
            لكل(/* i=latin1Length */; i<حد; ++i) {
                p16[i]=(uint16_t)lead_Unit_مقدار;
            }
        }
    } وإلا {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->شاخص_درازی*2);

        إذا(lead_Unit_مقدار!=initial_مقدار) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلوک=(uint16_t)(latin1_درازی>>UTRIE_شاخص_لغزش);
            i=0xd800>>UTRIE_لغزش;
            حد=0xdc00>>UTRIE_لغزش;
            لكل(; i<حد; ++i) {
                p16[i]=بلوک;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->شاخص_درازی);

        /* Latin-1 data */
        لكل(i=0; i<latin1_درازی; ++i) {
            p32[i]=initial_مقدار;
        }

        /* data for lead surrogate code units */
        إذا(lead_Unit_مقدار!=initial_مقدار) {
            حد=latin1_درازی+UTRIE_داده_بلوک_درازی;
            لكل(/* i=latin1Length */; i<حد; ++i) {
                p32[i]=lead_Unit_مقدار;
            }
        }
    }

    trie->get_Folding_جابه_جایی=utrie_default_Get_Folding_جابه_جایی;

    أرجع actual_درازی;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
ساكن uint32_t U_CALLCONV
enum_Same_مقدار(ثابت فراغ * /*context*/, uint32_t value) {
    أرجع value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI فراغ U_EXPORT2
utrie_enum(ثابت UTrie *trie,
           UTrie_Enum_مقدار *enum_مقدار, UTrieEnumRange *enumRange, ثابت فراغ *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prev_مقدار, initial_مقدار;
    UChar32 c, prev;
    int32_t l, i, j, بلوک, prev_بلوک, null_بلوک, جابه_جایی;

    /* check arguments */
    إذا(trie==اشاره_تهی || trie->شاخص==اشاره_تهی || enumRange==اشاره_تهی) {
        أرجع;
    }
    إذا(enum_مقدار==اشاره_تهی) {
        enum_مقدار=enum_Same_مقدار;
    }

    idx=trie->شاخص;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_مقدار=enum_مقدار(context, trie->initial_مقدار);

    إذا(data32==اشاره_تهی) {
        null_بلوک=trie->شاخص_درازی;
    } وإلا {
        null_بلوک=0;
    }

    /* set variables for previous range */
    prev_بلوک=null_بلوک;
    prev=0;
    prev_مقدار=initial_مقدار;

    /* enumerate BMP - the main loop enumerates data blocks */
    لكل(i=0, c=0; c<=0xffff; ++i) {
        إذا(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_شاخص_درازی;
        } وإلا إذا(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_لغزش;
        }

        بلوک=idx[i]<<UTRIE_شاخص_لغزش;
        إذا(بلوک==prev_بلوک) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_داده_بلوک_درازی;
        } وإلا إذا(بلوک==null_بلوک) {
            /* this is the all-initial-value block */
            إذا(prev_مقدار!=initial_مقدار) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_مقدار)) {
                        أرجع;
                    }
                }
                prev_بلوک=null_بلوک;
                prev=c;
                prev_مقدار=initial_مقدار;
            }
            c+=UTRIE_داده_بلوک_درازی;
        } وإلا {
            prev_بلوک=بلوک;
            لكل(j=0; j<UTRIE_داده_بلوک_درازی; ++j) {
                value=enum_مقدار(context, data32!=اشاره_تهی ? data32[بلوک+j] : idx[بلوک+j]);
                إذا(value!=prev_مقدار) {
                    إذا(prev<c) {
                        إذا(!enumRange(context, prev, c, prev_مقدار)) {
                            أرجع;
                        }
                    }
                    إذا(j>0) {
                        /* the block is not filled with all the same value */
                        prev_بلوک=-1;
                    }
                    prev=c;
                    prev_مقدار=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    لكل(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        جابه_جایی=idx[l>>UTRIE_لغزش]<<UTRIE_شاخص_لغزش;
        إذا(جابه_جایی==null_بلوک) {
            /* no entries for a whole block of lead surrogates */
            إذا(prev_مقدار!=initial_مقدار) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_مقدار)) {
                        أرجع;
                    }
                }
                prev_بلوک=null_بلوک;
                prev=c;
                prev_مقدار=initial_مقدار;
            }

            l+=UTRIE_داده_بلوک_درازی;
            c+=UTRIE_داده_بلوک_درازی<<10;
            استمر;
        }

        value= data32!=اشاره_تهی ? data32[جابه_جایی+(l&UTRIE_ماسک)] : idx[جابه_جایی+(l&UTRIE_ماسک)];

        /* enumerate trail surrogates for this lead surrogate */
        جابه_جایی=trie->get_Folding_جابه_جایی(value);
        إذا(جابه_جایی<=0) {
            /* no data for this lead surrogate */
            إذا(prev_مقدار!=initial_مقدار) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_مقدار)) {
                        أرجع;
                    }
                }
                prev_بلوک=null_بلوک;
                prev=c;
                prev_مقدار=initial_مقدار;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } وإلا {
            /* enumerate code points for this lead surrogate */
            i=جابه_جایی;
            جابه_جایی+=UTRIE_SURROGATE_بلوک_شمار;
            افعل {
                /* copy of most of the body of the BMP loop */
                بلوک=idx[i]<<UTRIE_شاخص_لغزش;
                إذا(بلوک==prev_بلوک) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_داده_بلوک_درازی;
                } وإلا إذا(بلوک==null_بلوک) {
                    /* this is the all-initial-value block */
                    إذا(prev_مقدار!=initial_مقدار) {
                        إذا(prev<c) {
                            إذا(!enumRange(context, prev, c, prev_مقدار)) {
                                أرجع;
                            }
                        }
                        prev_بلوک=null_بلوک;
                        prev=c;
                        prev_مقدار=initial_مقدار;
                    }
                    c+=UTRIE_داده_بلوک_درازی;
                } وإلا {
                    prev_بلوک=بلوک;
                    لكل(j=0; j<UTRIE_داده_بلوک_درازی; ++j) {
                        value=enum_مقدار(context, data32!=اشاره_تهی ? data32[بلوک+j] : idx[بلوک+j]);
                        إذا(value!=prev_مقدار) {
                            إذا(prev<c) {
                                إذا(!enumRange(context, prev, c, prev_مقدار)) {
                                    أرجع;
                                }
                            }
                            إذا(j>0) {
                                /* the block is not filled with all the same value */
                                prev_بلوک=-1;
                            }
                            prev=c;
                            prev_مقدار=value;
                        }
                        ++c;
                    }
                }
            } طالما(++i<جابه_جایی);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_مقدار);
}

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

#جے_تعریف_شدہ UTRIE_DEBUG
#   include <stdio.h>
#جے_انت

#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#تعریف_رد ABS
#تعریف_کرو ABS(x) ((x)>=0 ? (x) : -(x))

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
utrie_open(UNewTrie *fill_وچ,
           uint32_t *alias_انکڑے, int32_t max_انکڑے_لمائی,
           uint32_t initial_قدر, uint32_t lead_Unit_قدر,
           UBool latin1_خطی) {
    UNewTrie *trie;
    int32_t i, j;

    إذا( max_انکڑے_لمائی<UTRIE_انکڑے_بلاک_لمائی ||
        (latin1_خطی && max_انکڑے_لمائی<1024)
    ) {
        أرجع خالی_سوچک;
    }

    إذا(fill_وچ!=خالی_سوچک) {
        trie=fill_وچ;
    } وإلا {
        trie=(UNewTrie *)uprv_malloc(حجم(UNewTrie));
        إذا(trie==خالی_سوچک) {
            أرجع خالی_سوچک;
        }
    }
    uprv_memset(trie, 0, حجم(UNewTrie));
    trie->isAllocated = fill_وچ == خالی_سوچک;

    إذا(alias_انکڑے!=خالی_سوچک) {
        trie->انکڑے=alias_انکڑے;
        trie->is_انکڑے_Allocated=خطأ;
    } وإلا {
        trie->انکڑے=(uint32_t *)uprv_malloc(max_انکڑے_لمائی*4);
        إذا(trie->انکڑے==خالی_سوچک) {
            uprv_free(trie);
            أرجع خالی_سوچک;
        }
        trie->is_انکڑے_Allocated=سچ;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_انکڑے_بلاک_لمائی;

    إذا(latin1_خطی) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        افعل {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->اشاریہ[i++]=j;
            j+=UTRIE_انکڑے_بلاک_لمائی;
        } طالما(i<(256>>UTRIE_کھسکاؤ));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->انکڑے_لمائی=j;
    طالما(j>0) {
        trie->انکڑے[--j]=initial_قدر;
    }

    trie->lead_Unit_قدر=lead_Unit_قدر;
    trie->اشاریہ_لمائی=UTRIE_MAX_اشاریہ_لمائی;
    trie->انکڑے_سمرتھا=max_انکڑے_لمائی;
    trie->is_Latin1_خطی=latin1_خطی;
    trie->isCompacted=خطأ;
    أرجع trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fill_وچ, ثابت UNewTrie *other, uint32_t *alias_انکڑے, int32_t alias_انکڑے_سمرتھا) {
    UNewTrie *trie;
    UBool is_انکڑے_Allocated;

    /* do not clone if other is not valid or already compacted */
    إذا(other==خالی_سوچک || other->انکڑے==خالی_سوچک || other->isCompacted) {
        أرجع خالی_سوچک;
    }

    /* clone data */
    إذا(alias_انکڑے!=خالی_سوچک && alias_انکڑے_سمرتھا>=other->انکڑے_سمرتھا) {
        is_انکڑے_Allocated=خطأ;
    } وإلا {
        alias_انکڑے_سمرتھا=other->انکڑے_سمرتھا;
        alias_انکڑے=(uint32_t *)uprv_malloc(other->انکڑے_سمرتھا*4);
        إذا(alias_انکڑے==خالی_سوچک) {
            أرجع خالی_سوچک;
        }
        is_انکڑے_Allocated=سچ;
    }

    trie=utrie_open(fill_وچ, alias_انکڑے, alias_انکڑے_سمرتھا,
                    other->انکڑے[0], other->lead_Unit_قدر,
                    other->is_Latin1_خطی);
    إذا(trie==خالی_سوچک) {
        uprv_free(alias_انکڑے);
    } وإلا {
        uprv_memcpy(trie->اشاریہ, other->اشاریہ, حجم(trie->اشاریہ));
        uprv_memcpy(trie->انکڑے, other->انکڑے, (size_t)other->انکڑے_لمائی*4);
        trie->انکڑے_لمائی=other->انکڑے_لمائی;
        trie->is_انکڑے_Allocated=is_انکڑے_Allocated;
    }

    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
utrie_close(UNewTrie *trie) {
    إذا(trie!=خالی_سوچک) {
        إذا(trie->is_انکڑے_Allocated) {
            uprv_free(trie->انکڑے);
            trie->انکڑے=خالی_سوچک;
        }
        إذا(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_انکڑے(UNewTrie *trie, int32_t *p_لمائی) {
    إذا(trie==خالی_سوچک || p_لمائی==خالی_سوچک) {
        أرجع خالی_سوچک;
    }

    *p_لمائی=trie->انکڑے_لمائی;
    أرجع trie->انکڑے;
}

ساكن int32_t
utrie_alloc_انکڑے_بلاک(UNewTrie *trie) {
    int32_t new_بلاک, new_چوٹی;

    new_بلاک=trie->انکڑے_لمائی;
    new_چوٹی=new_بلاک+UTRIE_انکڑے_بلاک_لمائی;
    إذا(new_چوٹی>trie->انکڑے_سمرتھا) {
        /* out of memory in the data array */
        أرجع -1;
    }
    trie->انکڑے_لمائی=new_چوٹی;
    أرجع new_بلاک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ساكن int32_t
utrie_get_انکڑے_بلاک(UNewTrie *trie, UChar32 c) {
    int32_t اشاریہ_قدر, new_بلاک;

    c>>=UTRIE_کھسکاؤ;
    اشاریہ_قدر=trie->اشاریہ[c];
    إذا(اشاریہ_قدر>0) {
        أرجع اشاریہ_قدر;
    }

    /* allocate a new data block */
    new_بلاک=utrie_alloc_انکڑے_بلاک(trie);
    إذا(new_بلاک<0) {
        /* out of memory in the data array */
        أرجع -1;
    }
    trie->اشاریہ[c]=new_بلاک;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->انکڑے+new_بلاک, trie->انکڑے-اشاریہ_قدر, 4*UTRIE_انکڑے_بلاک_لمائی);
    أرجع new_بلاک;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t بلاک;

    /* valid, uncompacted trie and valid c? */
    إذا(trie==خالی_سوچک || trie->isCompacted || (uint32_t)c>0x10ffff) {
        أرجع خطأ;
    }

    بلاک=utrie_get_انکڑے_بلاک(trie, c);
    إذا(بلاک<0) {
        أرجع خطأ;
    }

    trie->انکڑے[بلاک+(c&UTRIE_نقاب)]=value;
    أرجع سچ;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *p_وچ_بلاک_Zero) {
    int32_t بلاک;

    /* valid, uncompacted trie and valid c? */
    إذا(trie==خالی_سوچک || trie->isCompacted || (uint32_t)c>0x10ffff) {
        إذا(p_وچ_بلاک_Zero!=خالی_سوچک) {
            *p_وچ_بلاک_Zero=سچ;
        }
        أرجع 0;
    }

    بلاک=trie->اشاریہ[c>>UTRIE_کھسکاؤ];
    إذا(p_وچ_بلاک_Zero!=خالی_سوچک) {
        *p_وچ_بلاک_Zero = بلاک == 0;
    }

    أرجع trie->انکڑے[ABS(بلاک)+(c&UTRIE_نقاب)];
}

/**
 * @internal
 */
ساكن فراغ
utrie_fill_بلاک(uint32_t *بلاک, UChar32 شروع, UChar32 حد_بندی,
                uint32_t value, uint32_t initial_قدر, UBool overwrite) {
    uint32_t *p_حد_بندی;

    p_حد_بندی=بلاک+حد_بندی;
    بلاک+=شروع;
    إذا(overwrite) {
        طالما(بلاک<p_حد_بندی) {
            *بلاک++=value;
        }
    } وإلا {
        طالما(بلاک<p_حد_بندی) {
            إذا(*بلاک==initial_قدر) {
                *بلاک=value;
            }
            ++بلاک;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_سیٹ_Range32(UNewTrie *trie, UChar32 شروع, UChar32 حد_بندی, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initial_قدر;
    int32_t بلاک, rest, repeat_بلاک;

    /* valid, uncompacted trie and valid indexes? */
    إذا( trie==خالی_سوچک || trie->isCompacted ||
        (uint32_t)شروع>0x10ffff || (uint32_t)حد_بندی>0x110000 || شروع>حد_بندی
    ) {
        أرجع خطأ;
    }
    إذا(شروع==حد_بندی) {
        أرجع سچ; /* nothing to do */
    }

    initial_قدر=trie->انکڑے[0];
    إذا(شروع&UTRIE_نقاب) {
        UChar32 اگلا_شروع;

        /* set partial block at [start..following block boundary[ */
        بلاک=utrie_get_انکڑے_بلاک(trie, شروع);
        إذا(بلاک<0) {
            أرجع خطأ;
        }

        اگلا_شروع=(شروع+UTRIE_انکڑے_بلاک_لمائی)&~UTRIE_نقاب;
        إذا(اگلا_شروع<=حد_بندی) {
            utrie_fill_بلاک(trie->انکڑے+بلاک, شروع&UTRIE_نقاب, UTRIE_انکڑے_بلاک_لمائی,
                            value, initial_قدر, overwrite);
            شروع=اگلا_شروع;
        } وإلا {
            utrie_fill_بلاک(trie->انکڑے+بلاک, شروع&UTRIE_نقاب, حد_بندی&UTRIE_نقاب,
                            value, initial_قدر, overwrite);
            أرجع سچ;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد_بندی&UTRIE_نقاب;

    /* round down limit to a block boundary */
    حد_بندی&=~UTRIE_نقاب;

    /* iterate over all-value blocks */
    إذا(value==initial_قدر) {
        repeat_بلاک=0;
    } وإلا {
        repeat_بلاک=-1;
    }
    طالما(شروع<حد_بندی) {
        /* get index value */
        بلاک=trie->اشاریہ[شروع>>UTRIE_کھسکاؤ];
        إذا(بلاک>0) {
            /* already allocated, fill in value */
            utrie_fill_بلاک(trie->انکڑے+بلاک, 0, UTRIE_انکڑے_بلاک_لمائی, value, initial_قدر, overwrite);
        } وإلا إذا(trie->انکڑے[-بلاک]!=value && (بلاک==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            إذا(repeat_بلاک>=0) {
                trie->اشاریہ[شروع>>UTRIE_کھسکاؤ]=-repeat_بلاک;
            } وإلا {
                /* create and set and fill the repeatBlock */
                repeat_بلاک=utrie_get_انکڑے_بلاک(trie, شروع);
                إذا(repeat_بلاک<0) {
                    أرجع خطأ;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->اشاریہ[شروع>>UTRIE_کھسکاؤ]=-repeat_بلاک;
                utrie_fill_بلاک(trie->انکڑے+repeat_بلاک, 0, UTRIE_انکڑے_بلاک_لمائی, value, initial_قدر, سچ);
            }
        }

        شروع+=UTRIE_انکڑے_بلاک_لمائی;
    }

    إذا(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلاک=utrie_get_انکڑے_بلاک(trie, شروع);
        إذا(بلاک<0) {
            أرجع خطأ;
        }

        utrie_fill_بلاک(trie->انکڑے+بلاک, 0, rest, value, initial_قدر, overwrite);
    }

    أرجع سچ;
}

ساكن int32_t
لبھو_Same_اشاریہ_بلاک(ثابت int32_t *idx, int32_t اشاریہ_لمائی,
                    int32_t other_بلاک) {
    int32_t بلاک, i;

    لكل(بلاک=UTRIE_BMP_اشاریہ_لمائی; بلاک<اشاریہ_لمائی; بلاک+=UTRIE_SURROGATE_بلاک_گنتی) {
        لكل(i=0; i<UTRIE_SURROGATE_بلاک_گنتی; ++i) {
            إذا(idx[بلاک+i]!=idx[other_بلاک+i]) {
                اكسر;
            }
        }
        إذا(i==UTRIE_SURROGATE_بلاک_گنتی) {
            أرجع بلاک;
        }
    }
    أرجع اشاریہ_لمائی;
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
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_قدر *get_Folded_قدر, UError_کوڈ *p_Error_کوڈ) {
    int32_t leadIndexes[UTRIE_SURROGATE_بلاک_گنتی];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t اشاریہ_لمائی, بلاک;
#جے_تعریف_شدہ UTRIE_DEBUG
    صحيح گنتی_Lead_CUWith_انکڑے=0;
#جے_انت

    idx=trie->اشاریہ;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_کھسکاؤ), 4*UTRIE_SURROGATE_بلاک_گنتی);

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
    إذا(trie->lead_Unit_قدر==trie->انکڑے[0]) {
        بلاک=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } وإلا {
        /* create and fill the repeatBlock */
        بلاک=utrie_alloc_انکڑے_بلاک(trie);
        إذا(بلاک<0) {
            /* data table overflow */
            *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        utrie_fill_بلاک(trie->انکڑے+بلاک, 0, UTRIE_انکڑے_بلاک_لمائی, trie->lead_Unit_قدر, trie->انکڑے[0], سچ);
        بلاک=-بلاک; /* negative block number to indicate that it is a repeat block */
    }
    لكل(c=(0xd800>>UTRIE_کھسکاؤ); c<(0xdc00>>UTRIE_کھسکاؤ); ++c) {
        trie->اشاریہ[c]=بلاک;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    اشاریہ_لمائی=UTRIE_BMP_اشاریہ_لمائی;

    /* search for any index (stage 1) entries for supplementary code points */
    لكل(c=0x10000; c<0x110000;) {
        إذا(idx[c>>UTRIE_کھسکاؤ]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#جے_تعریف_شدہ UTRIE_DEBUG
            ++گنتی_Lead_CUWith_انکڑے;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#جے_انت

            /* is there an identical index block? */
            بلاک=لبھو_Same_اشاریہ_بلاک(idx, اشاریہ_لمائی, c>>UTRIE_کھسکاؤ);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_قدر(trie, c, بلاک+UTRIE_SURROGATE_بلاک_گنتی);
            إذا(value!=utrie_get32(trie, U16_LEAD(c), خالی_سوچک)) {
                إذا(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
                    أرجع;
                }

                /* if we did not find an identical index block... */
                إذا(بلاک==اشاریہ_لمائی) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+اشاریہ_لمائی,
                                 idx+(c>>UTRIE_کھسکاؤ),
                                 4*UTRIE_SURROGATE_بلاک_گنتی);
                    اشاریہ_لمائی+=UTRIE_SURROGATE_بلاک_گنتی;
                }
            }
            c+=0x400;
        } وإلا {
            c+=UTRIE_انکڑے_بلاک_لمائی;
        }
    }
#جے_تعریف_شدہ UTRIE_DEBUG
    إذا(گنتی_Lead_CUWith_انکڑے>0) {
        لکھو("supplementary data for %d lead surrogates\n", گنتی_Lead_CUWith_انکڑے);
    }
#جے_انت

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    إذا(اشاریہ_لمائی>=UTRIE_MAX_اشاریہ_لمائی) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_اشاریہ_لمائی+UTRIE_SURROGATE_بلاک_گنتی,
                 idx+UTRIE_BMP_اشاریہ_لمائی,
                 4*(اشاریہ_لمائی-UTRIE_BMP_اشاریہ_لمائی));
    uprv_memcpy(idx+UTRIE_BMP_اشاریہ_لمائی,
                leadIndexes,
                4*UTRIE_SURROGATE_بلاک_گنتی);
    اشاریہ_لمائی+=UTRIE_SURROGATE_بلاک_گنتی;

#جے_تعریف_شدہ UTRIE_DEBUG
    لکھو("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_اشاریہ_لمائی, (لما)UTRIE_MAX_اشاریہ_لمائی, اشاریہ_لمائی);
#جے_انت

    trie->اشاریہ_لمائی=اشاریہ_لمائی;
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
لبھو_بن_ورتیا_بلاکاں(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_ویلا_انکڑے_لمائی>>UTRIE_کھسکاؤ)*4);

    /* mark each block that _is_ used with 0 */
    لكل(i=0; i<trie->اشاریہ_لمائی; ++i) {
        trie->map[ABS(trie->اشاریہ[i])>>UTRIE_کھسکاؤ]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

ساكن int32_t
لبھو_Same_انکڑے_بلاک(ثابت uint32_t *انکڑے, int32_t انکڑے_لمائی,
                   int32_t other_بلاک, int32_t قدم) {
    int32_t بلاک;

    /* ensure that we do not even partially get past dataLength */
    انکڑے_لمائی-=UTRIE_انکڑے_بلاک_لمائی;

    لكل(بلاک=0; بلاک<=انکڑے_لمائی; بلاک+=قدم) {
        إذا(equal_uint32(انکڑے+بلاک, انکڑے+other_بلاک, UTRIE_انکڑے_بلاک_لمائی)) {
            أرجع بلاک;
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
utrie_compact(UNewTrie *trie, UBool overlap, UError_کوڈ *p_Error_کوڈ) {
    int32_t i, شروع, new_شروع, overlap_شروع;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }

    /* valid, uncompacted trie? */
    إذا(trie==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا(trie->isCompacted) {
        أرجع; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    لبھو_بن_ورتیا_بلاکاں(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    إذا(trie->is_Latin1_خطی && UTRIE_کھسکاؤ<=8) {
        overlap_شروع=UTRIE_انکڑے_بلاک_لمائی+256;
    } وإلا {
        overlap_شروع=UTRIE_انکڑے_بلاک_لمائی;
    }

    new_شروع=UTRIE_انکڑے_بلاک_لمائی;
    لكل(شروع=new_شروع; شروع<trie->انکڑے_لمائی;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        إذا(trie->map[شروع>>UTRIE_کھسکاؤ]<0) {
            /* advance start to the next block */
            شروع+=UTRIE_انکڑے_بلاک_لمائی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* search for an identical block */
        إذا( شروع>=overlap_شروع &&
            (i=لبھو_Same_انکڑے_بلاک(trie->انکڑے, new_شروع, شروع,
                            overlap ? UTRIE_انکڑے_GRANULARITY : UTRIE_انکڑے_بلاک_لمائی))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[شروع>>UTRIE_کھسکاؤ]=i;

            /* advance start to the next block */
            شروع+=UTRIE_انکڑے_بلاک_لمائی;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        إذا(overlap && شروع>=overlap_شروع) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            لكل(i=UTRIE_انکڑے_بلاک_لمائی-UTRIE_انکڑے_GRANULARITY;
                i>0 && !equal_uint32(trie->انکڑے+(new_شروع-i), trie->انکڑے+شروع, i);
                i-=UTRIE_انکڑے_GRANULARITY) {}
        } وإلا {
            i=0;
        }

        إذا(i>0) {
            /* some overlap */
            trie->map[شروع>>UTRIE_کھسکاؤ]=new_شروع-i;

            /* move the non-overlapping indexes to their new positions */
            شروع+=i;
            لكل(i=UTRIE_انکڑے_بلاک_لمائی-i; i>0; --i) {
                trie->انکڑے[new_شروع++]=trie->انکڑے[شروع++];
            }
        } وإلا إذا(new_شروع<شروع) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[شروع>>UTRIE_کھسکاؤ]=new_شروع;
            لكل(i=UTRIE_انکڑے_بلاک_لمائی; i>0; --i) {
                trie->انکڑے[new_شروع++]=trie->انکڑے[شروع++];
            }
        } وإلا /* no overlap && newStart==start */ {
            trie->map[شروع>>UTRIE_کھسکاؤ]=شروع;
            new_شروع+=UTRIE_انکڑے_بلاک_لمائی;
            شروع=new_شروع;
        }
    }

    /* now adjust the index (stage 1) table */
    لكل(i=0; i<trie->اشاریہ_لمائی; ++i) {
        trie->اشاریہ[i]=trie->map[ABS(trie->اشاریہ[i])>>UTRIE_کھسکاؤ];
    }

#جے_تعریف_شدہ UTRIE_DEBUG
    /* we saved some space */
    لکھو("compacting trie: count of 32-bit words %lu->%lu\n",
            (لما)trie->انکڑے_لمائی, (لما)new_شروع);
#جے_انت

    trie->انکڑے_لمائی=new_شروع;
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
default_Get_Folded_قدر(UNewTrie *trie, UChar32 شروع, int32_t وتھ) {
    uint32_t value, initial_قدر;
    UChar32 حد_بندی;
    UBool وچ_بلاک_Zero;

    initial_قدر=trie->انکڑے[0];
    حد_بندی=شروع+0x400;
    طالما(شروع<حد_بندی) {
        value=utrie_get32(trie, شروع, &وچ_بلاک_Zero);
        إذا(وچ_بلاک_Zero) {
            شروع+=UTRIE_انکڑے_بلاک_لمائی;
        } وإلا إذا(value!=initial_قدر) {
            أرجع static_cast<uint32_t>(وتھ);
        } وإلا {
            ++شروع;
        }
    }
    أرجع 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, فراغ *dt, int32_t capacity,
                UNew_Trie_Get_Folded_قدر *get_Folded_قدر,
                UBool reduce_To16_بٹ,
                UError_کوڈ *p_Error_کوڈ) {
    UTrie_سرخی *سرخی;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* انکڑے = خالی_سوچک;

    /* argument check */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    إذا(trie==خالی_سوچک || capacity<0 || (capacity>0 && dt==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    إذا(get_Folded_قدر==خالی_سوچک) {
        get_Folded_قدر=default_Get_Folded_قدر;
    }

    انکڑے = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    إذا(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, خطأ, p_Error_کوڈ);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_قدر, p_Error_کوڈ);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, سچ, p_Error_کوڈ);

        trie->isCompacted=سچ;
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            أرجع 0;
        }
    }

    /* is dataLength within limits? */
    إذا( (reduce_To16_بٹ ? (trie->انکڑے_لمائی+trie->اشاریہ_لمائی) : trie->انکڑے_لمائی) >= UTRIE_MAX_انکڑے_لمائی) {
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
    }

    length=حجم(UTrie_سرخی)+2*trie->اشاریہ_لمائی;
    إذا(reduce_To16_بٹ) {
        length+=2*trie->انکڑے_لمائی;
    } وإلا {
        length+=4*trie->انکڑے_لمائی;
    }

    إذا(length>capacity) {
        أرجع length; /* preflighting */
    }

#جے_تعریف_شدہ UTRIE_DEBUG
    لکھو("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (لما)trie->اشاریہ_لمائی, (لما)trie->انکڑے_لمائی, (لما)length);
#جے_انت

    /* set the header fields */
    سرخی=(UTrie_سرخی *)انکڑے;
    انکڑے+=حجم(UTrie_سرخی);

    سرخی->signature=0x54726965; /* "Trie" */
    سرخی->options=UTRIE_کھسکاؤ | (UTRIE_اشاریہ_کھسکاؤ<<UTRIE_OPTIONS_اشاریہ_کھسکاؤ);

    إذا(!reduce_To16_بٹ) {
        سرخی->options|=UTRIE_OPTIONS_انکڑے_IS_32_ثنائی_ہندسہ;
    }
    إذا(trie->is_Latin1_خطی) {
        سرخی->options|=UTRIE_OPTIONS_LATIN1_IS_خطی;
    }

    سرخی->اشاریہ_لمائی=trie->اشاریہ_لمائی;
    سرخی->انکڑے_لمائی=trie->انکڑے_لمائی;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    إذا(reduce_To16_بٹ) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->اشاریہ;
        dest16=(uint16_t *)انکڑے;
        لكل(i=trie->اشاریہ_لمائی; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->اشاریہ_لمائی)>>UTRIE_اشاریہ_کھسکاؤ);
        }

        /* write 16-bit data values */
        p=trie->انکڑے;
        لكل(i=trie->انکڑے_لمائی; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } وإلا {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->اشاریہ;
        dest16=(uint16_t *)انکڑے;
        لكل(i=trie->اشاریہ_لمائی; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_اشاریہ_کھسکاؤ);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->انکڑے, 4*(size_t)trie->انکڑے_لمائی);
    }

    أرجع length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_default_Get_Folding_وتھ(uint32_t انکڑے) {
    أرجع (int32_t)انکڑے;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ثابت فراغ *انکڑے, int32_t length, UError_کوڈ *p_Error_کوڈ) {
    ثابت UTrie_سرخی *سرخی;
    ثابت uint16_t *p16;
    uint32_t options;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع -1;
    }

    /* enough data for a trie header? */
    إذا(length<(int32_t)حجم(UTrie_سرخی)) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }

    /* check the signature */
    سرخی=(ثابت UTrie_سرخی *)انکڑے;
    إذا(سرخی->signature!=0x54726965) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }

    /* get the options and check the shift values */
    options=سرخی->options;
    إذا( (options&UTRIE_OPTIONS_کھسکاؤ_نقاب)!=UTRIE_کھسکاؤ ||
        ((options>>UTRIE_OPTIONS_اشاریہ_کھسکاؤ)&UTRIE_OPTIONS_کھسکاؤ_نقاب)!=UTRIE_اشاریہ_کھسکاؤ
    ) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }
    trie->is_Latin1_خطی = (options & UTRIE_OPTIONS_LATIN1_IS_خطی) != 0;

    /* get the length values */
    trie->اشاریہ_لمائی=سرخی->اشاریہ_لمائی;
    trie->انکڑے_لمائی=سرخی->انکڑے_لمائی;

    length-=(int32_t)حجم(UTrie_سرخی);

    /* enough data for the index? */
    إذا(length<2*trie->اشاریہ_لمائی) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }
    p16=(ثابت uint16_t *)(سرخی+1);
    trie->اشاریہ=p16;
    p16+=trie->اشاریہ_لمائی;
    length-=2*trie->اشاریہ_لمائی;

    /* get the data */
    إذا(options&UTRIE_OPTIONS_انکڑے_IS_32_ثنائی_ہندسہ) {
        إذا(length<4*trie->انکڑے_لمائی) {
            *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
            أرجع -1;
        }
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_قدر=trie->data32[0];
        length=(int32_t)حجم(UTrie_سرخی)+2*trie->اشاریہ_لمائی+4*trie->انکڑے_لمائی;
    } وإلا {
        إذا(length<2*trie->انکڑے_لمائی) {
            *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
            أرجع -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=خالی_سوچک;
        trie->initial_قدر=trie->اشاریہ[trie->اشاریہ_لمائی];
        length=(int32_t)حجم(UTrie_سرخی)+2*trie->اشاریہ_لمائی+2*trie->انکڑے_لمائی;
    }

    trie->get_Folding_وتھ=utrie_default_Get_Folding_وتھ;

    أرجع length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       فراغ *انکڑے, int32_t length,
                       uint32_t initial_قدر, uint32_t lead_Unit_قدر,
                       UBool make16_ثنائی_ہندسہ_Trie,
                       UError_کوڈ *p_Error_کوڈ) {
    uint16_t *p16;
    int32_t actual_لمائی, latin1_لمائی, i, حد_بندی;
    uint16_t بلاک;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_لمائی= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->اشاریہ_لمائی=UTRIE_BMP_اشاریہ_لمائی+UTRIE_SURROGATE_بلاک_گنتی;
    trie->انکڑے_لمائی=latin1_لمائی;
    إذا(lead_Unit_قدر!=initial_قدر) {
        trie->انکڑے_لمائی+=UTRIE_انکڑے_بلاک_لمائی;
    }

    actual_لمائی=trie->اشاریہ_لمائی*2;
    إذا(make16_ثنائی_ہندسہ_Trie) {
        actual_لمائی+=trie->انکڑے_لمائی*2;
    } وإلا {
        actual_لمائی+=trie->انکڑے_لمائی*4;
    }

    /* enough space for the dummy trie? */
    إذا(length<actual_لمائی) {
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
        أرجع actual_لمائی;
    }

    trie->is_Latin1_خطی=سچ;
    trie->initial_قدر=initial_قدر;

    /* fill the index and data arrays */
    p16=(uint16_t *)انکڑے;
    trie->اشاریہ=p16;

    إذا(make16_ثنائی_ہندسہ_Trie) {
        /* indexes to block 0 */
        بلاک=(uint16_t)(trie->اشاریہ_لمائی>>UTRIE_اشاریہ_کھسکاؤ);
        حد_بندی=trie->اشاریہ_لمائی;
        لكل(i=0; i<حد_بندی; ++i) {
            p16[i]=بلاک;
        }

        إذا(lead_Unit_قدر!=initial_قدر) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلاک+=(uint16_t)(latin1_لمائی>>UTRIE_اشاریہ_کھسکاؤ);
            i=0xd800>>UTRIE_کھسکاؤ;
            حد_بندی=0xdc00>>UTRIE_کھسکاؤ;
            لكل(; i<حد_بندی; ++i) {
                p16[i]=بلاک;
            }
        }

        trie->data32=خالی_سوچک;

        /* Latin-1 data */
        p16+=trie->اشاریہ_لمائی;
        لكل(i=0; i<latin1_لمائی; ++i) {
            p16[i]=(uint16_t)initial_قدر;
        }

        /* data for lead surrogate code units */
        إذا(lead_Unit_قدر!=initial_قدر) {
            حد_بندی=latin1_لمائی+UTRIE_انکڑے_بلاک_لمائی;
            لكل(/* i=latin1Length */; i<حد_بندی; ++i) {
                p16[i]=(uint16_t)lead_Unit_قدر;
            }
        }
    } وإلا {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->اشاریہ_لمائی*2);

        إذا(lead_Unit_قدر!=initial_قدر) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلاک=(uint16_t)(latin1_لمائی>>UTRIE_اشاریہ_کھسکاؤ);
            i=0xd800>>UTRIE_کھسکاؤ;
            حد_بندی=0xdc00>>UTRIE_کھسکاؤ;
            لكل(; i<حد_بندی; ++i) {
                p16[i]=بلاک;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->اشاریہ_لمائی);

        /* Latin-1 data */
        لكل(i=0; i<latin1_لمائی; ++i) {
            p32[i]=initial_قدر;
        }

        /* data for lead surrogate code units */
        إذا(lead_Unit_قدر!=initial_قدر) {
            حد_بندی=latin1_لمائی+UTRIE_انکڑے_بلاک_لمائی;
            لكل(/* i=latin1Length */; i<حد_بندی; ++i) {
                p32[i]=lead_Unit_قدر;
            }
        }
    }

    trie->get_Folding_وتھ=utrie_default_Get_Folding_وتھ;

    أرجع actual_لمائی;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
ساكن uint32_t U_CALLCONV
enum_Same_قدر(ثابت فراغ * /*context*/, uint32_t value) {
    أرجع value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI فراغ U_EXPORT2
utrie_enum(ثابت UTrie *trie,
           UTrie_Enum_قدر *enum_قدر, UTrieEnumRange *enumRange, ثابت فراغ *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prev_قدر, initial_قدر;
    UChar32 c, prev;
    int32_t l, i, j, بلاک, prev_بلاک, null_بلاک, وتھ;

    /* check arguments */
    إذا(trie==خالی_سوچک || trie->اشاریہ==خالی_سوچک || enumRange==خالی_سوچک) {
        أرجع;
    }
    إذا(enum_قدر==خالی_سوچک) {
        enum_قدر=enum_Same_قدر;
    }

    idx=trie->اشاریہ;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_قدر=enum_قدر(context, trie->initial_قدر);

    إذا(data32==خالی_سوچک) {
        null_بلاک=trie->اشاریہ_لمائی;
    } وإلا {
        null_بلاک=0;
    }

    /* set variables for previous range */
    prev_بلاک=null_بلاک;
    prev=0;
    prev_قدر=initial_قدر;

    /* enumerate BMP - the main loop enumerates data blocks */
    لكل(i=0, c=0; c<=0xffff; ++i) {
        إذا(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_اشاریہ_لمائی;
        } وإلا إذا(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_کھسکاؤ;
        }

        بلاک=idx[i]<<UTRIE_اشاریہ_کھسکاؤ;
        إذا(بلاک==prev_بلاک) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_انکڑے_بلاک_لمائی;
        } وإلا إذا(بلاک==null_بلاک) {
            /* this is the all-initial-value block */
            إذا(prev_قدر!=initial_قدر) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_قدر)) {
                        أرجع;
                    }
                }
                prev_بلاک=null_بلاک;
                prev=c;
                prev_قدر=initial_قدر;
            }
            c+=UTRIE_انکڑے_بلاک_لمائی;
        } وإلا {
            prev_بلاک=بلاک;
            لكل(j=0; j<UTRIE_انکڑے_بلاک_لمائی; ++j) {
                value=enum_قدر(context, data32!=خالی_سوچک ? data32[بلاک+j] : idx[بلاک+j]);
                إذا(value!=prev_قدر) {
                    إذا(prev<c) {
                        إذا(!enumRange(context, prev, c, prev_قدر)) {
                            أرجع;
                        }
                    }
                    إذا(j>0) {
                        /* the block is not filled with all the same value */
                        prev_بلاک=-1;
                    }
                    prev=c;
                    prev_قدر=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    لكل(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        وتھ=idx[l>>UTRIE_کھسکاؤ]<<UTRIE_اشاریہ_کھسکاؤ;
        إذا(وتھ==null_بلاک) {
            /* no entries for a whole block of lead surrogates */
            إذا(prev_قدر!=initial_قدر) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_قدر)) {
                        أرجع;
                    }
                }
                prev_بلاک=null_بلاک;
                prev=c;
                prev_قدر=initial_قدر;
            }

            l+=UTRIE_انکڑے_بلاک_لمائی;
            c+=UTRIE_انکڑے_بلاک_لمائی<<10;
            استمر;
        }

        value= data32!=خالی_سوچک ? data32[وتھ+(l&UTRIE_نقاب)] : idx[وتھ+(l&UTRIE_نقاب)];

        /* enumerate trail surrogates for this lead surrogate */
        وتھ=trie->get_Folding_وتھ(value);
        إذا(وتھ<=0) {
            /* no data for this lead surrogate */
            إذا(prev_قدر!=initial_قدر) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_قدر)) {
                        أرجع;
                    }
                }
                prev_بلاک=null_بلاک;
                prev=c;
                prev_قدر=initial_قدر;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } وإلا {
            /* enumerate code points for this lead surrogate */
            i=وتھ;
            وتھ+=UTRIE_SURROGATE_بلاک_گنتی;
            افعل {
                /* copy of most of the body of the BMP loop */
                بلاک=idx[i]<<UTRIE_اشاریہ_کھسکاؤ;
                إذا(بلاک==prev_بلاک) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_انکڑے_بلاک_لمائی;
                } وإلا إذا(بلاک==null_بلاک) {
                    /* this is the all-initial-value block */
                    إذا(prev_قدر!=initial_قدر) {
                        إذا(prev<c) {
                            إذا(!enumRange(context, prev, c, prev_قدر)) {
                                أرجع;
                            }
                        }
                        prev_بلاک=null_بلاک;
                        prev=c;
                        prev_قدر=initial_قدر;
                    }
                    c+=UTRIE_انکڑے_بلاک_لمائی;
                } وإلا {
                    prev_بلاک=بلاک;
                    لكل(j=0; j<UTRIE_انکڑے_بلاک_لمائی; ++j) {
                        value=enum_قدر(context, data32!=خالی_سوچک ? data32[بلاک+j] : idx[بلاک+j]);
                        إذا(value!=prev_قدر) {
                            إذا(prev<c) {
                                إذا(!enumRange(context, prev, c, prev_قدر)) {
                                    أرجع;
                                }
                            }
                            إذا(j>0) {
                                /* the block is not filled with all the same value */
                                prev_بلاک=-1;
                            }
                            prev=c;
                            prev_قدر=value;
                        }
                        ++c;
                    }
                }
            } طالما(++i<وتھ);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_قدر);
}

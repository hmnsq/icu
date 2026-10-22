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

#اگر_تعریف_شدہ UTRIE_DEBUG
#   include <stdio.h>
#اگر_ختم

#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#تعریف_منسوخ ABS
#تعریف_کرو ABS(x) ((x)>=0 ? (x) : -(x))

ساکن اندرونی UBool
equal_uint32(ثابت uint32_t *s, ثابت uint32_t *t, int32_t length) {
    جب(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    واپس length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fill_اندر,
           uint32_t *alias_ڈیٹا, int32_t max_ڈیٹا_لمبائی,
           uint32_t initial_قدر, uint32_t lead_Unit_قدر,
           UBool latin1_خطی) {
    UNewTrie *trie;
    int32_t i, j;

    اگر( max_ڈیٹا_لمبائی<UTRIE_ڈیٹا_بلاک_لمبائی ||
        (latin1_خطی && max_ڈیٹا_لمبائی<1024)
    ) {
        واپس خالی_اشاریہ;
    }

    اگر(fill_اندر!=خالی_اشاریہ) {
        trie=fill_اندر;
    } ورنہ {
        trie=(UNewTrie *)uprv_malloc(حجم(UNewTrie));
        اگر(trie==خالی_اشاریہ) {
            واپس خالی_اشاریہ;
        }
    }
    uprv_memset(trie, 0, حجم(UNewTrie));
    trie->isAllocated = fill_اندر == خالی_اشاریہ;

    اگر(alias_ڈیٹا!=خالی_اشاریہ) {
        trie->ڈیٹا=alias_ڈیٹا;
        trie->is_ڈیٹا_Allocated=جھوٹ;
    } ورنہ {
        trie->ڈیٹا=(uint32_t *)uprv_malloc(max_ڈیٹا_لمبائی*4);
        اگر(trie->ڈیٹا==خالی_اشاریہ) {
            uprv_free(trie);
            واپس خالی_اشاریہ;
        }
        trie->is_ڈیٹا_Allocated=سچ;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_ڈیٹا_بلاک_لمبائی;

    اگر(latin1_خطی) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        کرو {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->اشاریہ[i++]=j;
            j+=UTRIE_ڈیٹا_بلاک_لمبائی;
        } جب(i<(256>>UTRIE_سرکاؤ));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->ڈیٹا_لمبائی=j;
    جب(j>0) {
        trie->ڈیٹا[--j]=initial_قدر;
    }

    trie->lead_Unit_قدر=lead_Unit_قدر;
    trie->اشاریہ_لمبائی=UTRIE_MAX_اشاریہ_لمبائی;
    trie->ڈیٹا_استعداد=max_ڈیٹا_لمبائی;
    trie->is_Latin1_خطی=latin1_خطی;
    trie->isCompacted=جھوٹ;
    واپس trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fill_اندر, ثابت UNewTrie *other, uint32_t *alias_ڈیٹا, int32_t alias_ڈیٹا_استعداد) {
    UNewTrie *trie;
    UBool is_ڈیٹا_Allocated;

    /* do not clone if other is not valid or already compacted */
    اگر(other==خالی_اشاریہ || other->ڈیٹا==خالی_اشاریہ || other->isCompacted) {
        واپس خالی_اشاریہ;
    }

    /* clone data */
    اگر(alias_ڈیٹا!=خالی_اشاریہ && alias_ڈیٹا_استعداد>=other->ڈیٹا_استعداد) {
        is_ڈیٹا_Allocated=جھوٹ;
    } ورنہ {
        alias_ڈیٹا_استعداد=other->ڈیٹا_استعداد;
        alias_ڈیٹا=(uint32_t *)uprv_malloc(other->ڈیٹا_استعداد*4);
        اگر(alias_ڈیٹا==خالی_اشاریہ) {
            واپس خالی_اشاریہ;
        }
        is_ڈیٹا_Allocated=سچ;
    }

    trie=utrie_open(fill_اندر, alias_ڈیٹا, alias_ڈیٹا_استعداد,
                    other->ڈیٹا[0], other->lead_Unit_قدر,
                    other->is_Latin1_خطی);
    اگر(trie==خالی_اشاریہ) {
        uprv_free(alias_ڈیٹا);
    } ورنہ {
        uprv_memcpy(trie->اشاریہ, other->اشاریہ, حجم(trie->اشاریہ));
        uprv_memcpy(trie->ڈیٹا, other->ڈیٹا, (size_t)other->ڈیٹا_لمبائی*4);
        trie->ڈیٹا_لمبائی=other->ڈیٹا_لمبائی;
        trie->is_ڈیٹا_Allocated=is_ڈیٹا_Allocated;
    }

    واپس trie;
}

U_CAPI خالی U_EXPORT2
utrie_close(UNewTrie *trie) {
    اگر(trie!=خالی_اشاریہ) {
        اگر(trie->is_ڈیٹا_Allocated) {
            uprv_free(trie->ڈیٹا);
            trie->ڈیٹا=خالی_اشاریہ;
        }
        اگر(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_ڈیٹا(UNewTrie *trie, int32_t *p_لمبائی) {
    اگر(trie==خالی_اشاریہ || p_لمبائی==خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }

    *p_لمبائی=trie->ڈیٹا_لمبائی;
    واپس trie->ڈیٹا;
}

ساکن int32_t
utrie_alloc_ڈیٹا_بلاک(UNewTrie *trie) {
    int32_t new_بلاک, new_چوٹی;

    new_بلاک=trie->ڈیٹا_لمبائی;
    new_چوٹی=new_بلاک+UTRIE_ڈیٹا_بلاک_لمبائی;
    اگر(new_چوٹی>trie->ڈیٹا_استعداد) {
        /* out of memory in the data array */
        واپس -1;
    }
    trie->ڈیٹا_لمبائی=new_چوٹی;
    واپس new_بلاک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ساکن int32_t
utrie_get_ڈیٹا_بلاک(UNewTrie *trie, UChar32 c) {
    int32_t اشاریہ_قدر, new_بلاک;

    c>>=UTRIE_سرکاؤ;
    اشاریہ_قدر=trie->اشاریہ[c];
    اگر(اشاریہ_قدر>0) {
        واپس اشاریہ_قدر;
    }

    /* allocate a new data block */
    new_بلاک=utrie_alloc_ڈیٹا_بلاک(trie);
    اگر(new_بلاک<0) {
        /* out of memory in the data array */
        واپس -1;
    }
    trie->اشاریہ[c]=new_بلاک;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->ڈیٹا+new_بلاک, trie->ڈیٹا-اشاریہ_قدر, 4*UTRIE_ڈیٹا_بلاک_لمبائی);
    واپس new_بلاک;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t بلاک;

    /* valid, uncompacted trie and valid c? */
    اگر(trie==خالی_اشاریہ || trie->isCompacted || (uint32_t)c>0x10ffff) {
        واپس جھوٹ;
    }

    بلاک=utrie_get_ڈیٹا_بلاک(trie, c);
    اگر(بلاک<0) {
        واپس جھوٹ;
    }

    trie->ڈیٹا[بلاک+(c&UTRIE_نقاب)]=value;
    واپس سچ;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *p_اندر_بلاک_Zero) {
    int32_t بلاک;

    /* valid, uncompacted trie and valid c? */
    اگر(trie==خالی_اشاریہ || trie->isCompacted || (uint32_t)c>0x10ffff) {
        اگر(p_اندر_بلاک_Zero!=خالی_اشاریہ) {
            *p_اندر_بلاک_Zero=سچ;
        }
        واپس 0;
    }

    بلاک=trie->اشاریہ[c>>UTRIE_سرکاؤ];
    اگر(p_اندر_بلاک_Zero!=خالی_اشاریہ) {
        *p_اندر_بلاک_Zero = بلاک == 0;
    }

    واپس trie->ڈیٹا[ABS(بلاک)+(c&UTRIE_نقاب)];
}

/**
 * @internal
 */
ساکن خالی
utrie_fill_بلاک(uint32_t *بلاک, UChar32 آغاز, UChar32 حد_بندی,
                uint32_t value, uint32_t initial_قدر, UBool overwrite) {
    uint32_t *p_حد_بندی;

    p_حد_بندی=بلاک+حد_بندی;
    بلاک+=آغاز;
    اگر(overwrite) {
        جب(بلاک<p_حد_بندی) {
            *بلاک++=value;
        }
    } ورنہ {
        جب(بلاک<p_حد_بندی) {
            اگر(*بلاک==initial_قدر) {
                *بلاک=value;
            }
            ++بلاک;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_سیٹ_Range32(UNewTrie *trie, UChar32 آغاز, UChar32 حد_بندی, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initial_قدر;
    int32_t بلاک, rest, repeat_بلاک;

    /* valid, uncompacted trie and valid indexes? */
    اگر( trie==خالی_اشاریہ || trie->isCompacted ||
        (uint32_t)آغاز>0x10ffff || (uint32_t)حد_بندی>0x110000 || آغاز>حد_بندی
    ) {
        واپس جھوٹ;
    }
    اگر(آغاز==حد_بندی) {
        واپس سچ; /* nothing to do */
    }

    initial_قدر=trie->ڈیٹا[0];
    اگر(آغاز&UTRIE_نقاب) {
        UChar32 اگلا_آغاز;

        /* set partial block at [start..following block boundary[ */
        بلاک=utrie_get_ڈیٹا_بلاک(trie, آغاز);
        اگر(بلاک<0) {
            واپس جھوٹ;
        }

        اگلا_آغاز=(آغاز+UTRIE_ڈیٹا_بلاک_لمبائی)&~UTRIE_نقاب;
        اگر(اگلا_آغاز<=حد_بندی) {
            utrie_fill_بلاک(trie->ڈیٹا+بلاک, آغاز&UTRIE_نقاب, UTRIE_ڈیٹا_بلاک_لمبائی,
                            value, initial_قدر, overwrite);
            آغاز=اگلا_آغاز;
        } ورنہ {
            utrie_fill_بلاک(trie->ڈیٹا+بلاک, آغاز&UTRIE_نقاب, حد_بندی&UTRIE_نقاب,
                            value, initial_قدر, overwrite);
            واپس سچ;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد_بندی&UTRIE_نقاب;

    /* round down limit to a block boundary */
    حد_بندی&=~UTRIE_نقاب;

    /* iterate over all-value blocks */
    اگر(value==initial_قدر) {
        repeat_بلاک=0;
    } ورنہ {
        repeat_بلاک=-1;
    }
    جب(آغاز<حد_بندی) {
        /* get index value */
        بلاک=trie->اشاریہ[آغاز>>UTRIE_سرکاؤ];
        اگر(بلاک>0) {
            /* already allocated, fill in value */
            utrie_fill_بلاک(trie->ڈیٹا+بلاک, 0, UTRIE_ڈیٹا_بلاک_لمبائی, value, initial_قدر, overwrite);
        } ورنہ اگر(trie->ڈیٹا[-بلاک]!=value && (بلاک==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            اگر(repeat_بلاک>=0) {
                trie->اشاریہ[آغاز>>UTRIE_سرکاؤ]=-repeat_بلاک;
            } ورنہ {
                /* create and set and fill the repeatBlock */
                repeat_بلاک=utrie_get_ڈیٹا_بلاک(trie, آغاز);
                اگر(repeat_بلاک<0) {
                    واپس جھوٹ;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->اشاریہ[آغاز>>UTRIE_سرکاؤ]=-repeat_بلاک;
                utrie_fill_بلاک(trie->ڈیٹا+repeat_بلاک, 0, UTRIE_ڈیٹا_بلاک_لمبائی, value, initial_قدر, سچ);
            }
        }

        آغاز+=UTRIE_ڈیٹا_بلاک_لمبائی;
    }

    اگر(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلاک=utrie_get_ڈیٹا_بلاک(trie, آغاز);
        اگر(بلاک<0) {
            واپس جھوٹ;
        }

        utrie_fill_بلاک(trie->ڈیٹا+بلاک, 0, rest, value, initial_قدر, overwrite);
    }

    واپس سچ;
}

ساکن int32_t
ڈھونڈو_Same_اشاریہ_بلاک(ثابت int32_t *idx, int32_t اشاریہ_لمبائی,
                    int32_t other_بلاک) {
    int32_t بلاک, i;

    برائے(بلاک=UTRIE_BMP_اشاریہ_لمبائی; بلاک<اشاریہ_لمبائی; بلاک+=UTRIE_SURROGATE_بلاک_گنتی) {
        برائے(i=0; i<UTRIE_SURROGATE_بلاک_گنتی; ++i) {
            اگر(idx[بلاک+i]!=idx[other_بلاک+i]) {
                توڑو;
            }
        }
        اگر(i==UTRIE_SURROGATE_بلاک_گنتی) {
            واپس بلاک;
        }
    }
    واپس اشاریہ_لمبائی;
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
ساکن خالی
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_قدر *get_Folded_قدر, UError_رمز *p_Error_رمز) {
    int32_t leadIndexes[UTRIE_SURROGATE_بلاک_گنتی];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t اشاریہ_لمبائی, بلاک;
#اگر_تعریف_شدہ UTRIE_DEBUG
    صحیح گنتی_Lead_CUWith_ڈیٹا=0;
#اگر_ختم

    idx=trie->اشاریہ;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_سرکاؤ), 4*UTRIE_SURROGATE_بلاک_گنتی);

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
    اگر(trie->lead_Unit_قدر==trie->ڈیٹا[0]) {
        بلاک=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } ورنہ {
        /* create and fill the repeatBlock */
        بلاک=utrie_alloc_ڈیٹا_بلاک(trie);
        اگر(بلاک<0) {
            /* data table overflow */
            *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        utrie_fill_بلاک(trie->ڈیٹا+بلاک, 0, UTRIE_ڈیٹا_بلاک_لمبائی, trie->lead_Unit_قدر, trie->ڈیٹا[0], سچ);
        بلاک=-بلاک; /* negative block number to indicate that it is a repeat block */
    }
    برائے(c=(0xd800>>UTRIE_سرکاؤ); c<(0xdc00>>UTRIE_سرکاؤ); ++c) {
        trie->اشاریہ[c]=بلاک;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    اشاریہ_لمبائی=UTRIE_BMP_اشاریہ_لمبائی;

    /* search for any index (stage 1) entries for supplementary code points */
    برائے(c=0x10000; c<0x110000;) {
        اگر(idx[c>>UTRIE_سرکاؤ]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#اگر_تعریف_شدہ UTRIE_DEBUG
            ++گنتی_Lead_CUWith_ڈیٹا;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#اگر_ختم

            /* is there an identical index block? */
            بلاک=ڈھونڈو_Same_اشاریہ_بلاک(idx, اشاریہ_لمبائی, c>>UTRIE_سرکاؤ);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_قدر(trie, c, بلاک+UTRIE_SURROGATE_بلاک_گنتی);
            اگر(value!=utrie_get32(trie, U16_LEAD(c), خالی_اشاریہ)) {
                اگر(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
                    واپس;
                }

                /* if we did not find an identical index block... */
                اگر(بلاک==اشاریہ_لمبائی) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+اشاریہ_لمبائی,
                                 idx+(c>>UTRIE_سرکاؤ),
                                 4*UTRIE_SURROGATE_بلاک_گنتی);
                    اشاریہ_لمبائی+=UTRIE_SURROGATE_بلاک_گنتی;
                }
            }
            c+=0x400;
        } ورنہ {
            c+=UTRIE_ڈیٹا_بلاک_لمبائی;
        }
    }
#اگر_تعریف_شدہ UTRIE_DEBUG
    اگر(گنتی_Lead_CUWith_ڈیٹا>0) {
        لکھو("supplementary data for %d lead surrogates\n", گنتی_Lead_CUWith_ڈیٹا);
    }
#اگر_ختم

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    اگر(اشاریہ_لمبائی>=UTRIE_MAX_اشاریہ_لمبائی) {
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_اشاریہ_لمبائی+UTRIE_SURROGATE_بلاک_گنتی,
                 idx+UTRIE_BMP_اشاریہ_لمبائی,
                 4*(اشاریہ_لمبائی-UTRIE_BMP_اشاریہ_لمبائی));
    uprv_memcpy(idx+UTRIE_BMP_اشاریہ_لمبائی,
                leadIndexes,
                4*UTRIE_SURROGATE_بلاک_گنتی);
    اشاریہ_لمبائی+=UTRIE_SURROGATE_بلاک_گنتی;

#اگر_تعریف_شدہ UTRIE_DEBUG
    لکھو("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_اشاریہ_لمبائی, (طویل)UTRIE_MAX_اشاریہ_لمبائی, اشاریہ_لمبائی);
#اگر_ختم

    trie->اشاریہ_لمبائی=اشاریہ_لمبائی;
}

/*
 * Set a value in the trie index map to indicate which data block
 * is referenced and which one is not.
 * utrie_compact() will remove data blocks that are not used at all.
 * Set
 * - 0 if it is used
 * - -1 if it is not used
 */
ساکن خالی
ڈھونڈو_غیر_مستعمل_بلاکیں(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_وقت_ڈیٹا_لمبائی>>UTRIE_سرکاؤ)*4);

    /* mark each block that _is_ used with 0 */
    برائے(i=0; i<trie->اشاریہ_لمبائی; ++i) {
        trie->map[ABS(trie->اشاریہ[i])>>UTRIE_سرکاؤ]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

ساکن int32_t
ڈھونڈو_Same_ڈیٹا_بلاک(ثابت uint32_t *ڈیٹا, int32_t ڈیٹا_لمبائی,
                   int32_t other_بلاک, int32_t قدم) {
    int32_t بلاک;

    /* ensure that we do not even partially get past dataLength */
    ڈیٹا_لمبائی-=UTRIE_ڈیٹا_بلاک_لمبائی;

    برائے(بلاک=0; بلاک<=ڈیٹا_لمبائی; بلاک+=قدم) {
        اگر(equal_uint32(ڈیٹا+بلاک, ڈیٹا+other_بلاک, UTRIE_ڈیٹا_بلاک_لمبائی)) {
            واپس بلاک;
        }
    }
    واپس -1;
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
ساکن خالی
utrie_compact(UNewTrie *trie, UBool overlap, UError_رمز *p_Error_رمز) {
    int32_t i, آغاز, new_آغاز, overlap_آغاز;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس;
    }

    /* valid, uncompacted trie? */
    اگر(trie==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر(trie->isCompacted) {
        واپس; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    ڈھونڈو_غیر_مستعمل_بلاکیں(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    اگر(trie->is_Latin1_خطی && UTRIE_سرکاؤ<=8) {
        overlap_آغاز=UTRIE_ڈیٹا_بلاک_لمبائی+256;
    } ورنہ {
        overlap_آغاز=UTRIE_ڈیٹا_بلاک_لمبائی;
    }

    new_آغاز=UTRIE_ڈیٹا_بلاک_لمبائی;
    برائے(آغاز=new_آغاز; آغاز<trie->ڈیٹا_لمبائی;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        اگر(trie->map[آغاز>>UTRIE_سرکاؤ]<0) {
            /* advance start to the next block */
            آغاز+=UTRIE_ڈیٹا_بلاک_لمبائی;

            /* leave newStart with the previous block! */
            جاری;
        }

        /* search for an identical block */
        اگر( آغاز>=overlap_آغاز &&
            (i=ڈھونڈو_Same_ڈیٹا_بلاک(trie->ڈیٹا, new_آغاز, آغاز,
                            overlap ? UTRIE_ڈیٹا_GRANULARITY : UTRIE_ڈیٹا_بلاک_لمبائی))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[آغاز>>UTRIE_سرکاؤ]=i;

            /* advance start to the next block */
            آغاز+=UTRIE_ڈیٹا_بلاک_لمبائی;

            /* leave newStart with the previous block! */
            جاری;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        اگر(overlap && آغاز>=overlap_آغاز) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            برائے(i=UTRIE_ڈیٹا_بلاک_لمبائی-UTRIE_ڈیٹا_GRANULARITY;
                i>0 && !equal_uint32(trie->ڈیٹا+(new_آغاز-i), trie->ڈیٹا+آغاز, i);
                i-=UTRIE_ڈیٹا_GRANULARITY) {}
        } ورنہ {
            i=0;
        }

        اگر(i>0) {
            /* some overlap */
            trie->map[آغاز>>UTRIE_سرکاؤ]=new_آغاز-i;

            /* move the non-overlapping indexes to their new positions */
            آغاز+=i;
            برائے(i=UTRIE_ڈیٹا_بلاک_لمبائی-i; i>0; --i) {
                trie->ڈیٹا[new_آغاز++]=trie->ڈیٹا[آغاز++];
            }
        } ورنہ اگر(new_آغاز<آغاز) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[آغاز>>UTRIE_سرکاؤ]=new_آغاز;
            برائے(i=UTRIE_ڈیٹا_بلاک_لمبائی; i>0; --i) {
                trie->ڈیٹا[new_آغاز++]=trie->ڈیٹا[آغاز++];
            }
        } ورنہ /* no overlap && newStart==start */ {
            trie->map[آغاز>>UTRIE_سرکاؤ]=آغاز;
            new_آغاز+=UTRIE_ڈیٹا_بلاک_لمبائی;
            آغاز=new_آغاز;
        }
    }

    /* now adjust the index (stage 1) table */
    برائے(i=0; i<trie->اشاریہ_لمبائی; ++i) {
        trie->اشاریہ[i]=trie->map[ABS(trie->اشاریہ[i])>>UTRIE_سرکاؤ];
    }

#اگر_تعریف_شدہ UTRIE_DEBUG
    /* we saved some space */
    لکھو("compacting trie: count of 32-bit words %lu->%lu\n",
            (طویل)trie->ڈیٹا_لمبائی, (طویل)new_آغاز);
#اگر_ختم

    trie->ڈیٹا_لمبائی=new_آغاز;
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
ساکن uint32_t U_CALLCONV
default_Get_Folded_قدر(UNewTrie *trie, UChar32 آغاز, int32_t انحراف) {
    uint32_t value, initial_قدر;
    UChar32 حد_بندی;
    UBool اندر_بلاک_Zero;

    initial_قدر=trie->ڈیٹا[0];
    حد_بندی=آغاز+0x400;
    جب(آغاز<حد_بندی) {
        value=utrie_get32(trie, آغاز, &اندر_بلاک_Zero);
        اگر(اندر_بلاک_Zero) {
            آغاز+=UTRIE_ڈیٹا_بلاک_لمبائی;
        } ورنہ اگر(value!=initial_قدر) {
            واپس static_cast<uint32_t>(انحراف);
        } ورنہ {
            ++آغاز;
        }
    }
    واپس 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, خالی *dt, int32_t capacity,
                UNew_Trie_Get_Folded_قدر *get_Folded_قدر,
                UBool reduce_To16_بٹ,
                UError_رمز *p_Error_رمز) {
    UTrie_سرخی *سرخی;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* ڈیٹا = خالی_اشاریہ;

    /* argument check */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    اگر(trie==خالی_اشاریہ || capacity<0 || (capacity>0 && dt==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    اگر(get_Folded_قدر==خالی_اشاریہ) {
        get_Folded_قدر=default_Get_Folded_قدر;
    }

    ڈیٹا = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    اگر(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, جھوٹ, p_Error_رمز);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_قدر, p_Error_رمز);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, سچ, p_Error_رمز);

        trie->isCompacted=سچ;
        اگر(U_FAILURE(*p_Error_رمز)) {
            واپس 0;
        }
    }

    /* is dataLength within limits? */
    اگر( (reduce_To16_بٹ ? (trie->ڈیٹا_لمبائی+trie->اشاریہ_لمبائی) : trie->ڈیٹا_لمبائی) >= UTRIE_MAX_ڈیٹا_لمبائی) {
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
    }

    length=حجم(UTrie_سرخی)+2*trie->اشاریہ_لمبائی;
    اگر(reduce_To16_بٹ) {
        length+=2*trie->ڈیٹا_لمبائی;
    } ورنہ {
        length+=4*trie->ڈیٹا_لمبائی;
    }

    اگر(length>capacity) {
        واپس length; /* preflighting */
    }

#اگر_تعریف_شدہ UTRIE_DEBUG
    لکھو("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (طویل)trie->اشاریہ_لمبائی, (طویل)trie->ڈیٹا_لمبائی, (طویل)length);
#اگر_ختم

    /* set the header fields */
    سرخی=(UTrie_سرخی *)ڈیٹا;
    ڈیٹا+=حجم(UTrie_سرخی);

    سرخی->signature=0x54726965; /* "Trie" */
    سرخی->options=UTRIE_سرکاؤ | (UTRIE_اشاریہ_سرکاؤ<<UTRIE_OPTIONS_اشاریہ_سرکاؤ);

    اگر(!reduce_To16_بٹ) {
        سرخی->options|=UTRIE_OPTIONS_ڈیٹا_IS_32_ثنائی_ہندسہ;
    }
    اگر(trie->is_Latin1_خطی) {
        سرخی->options|=UTRIE_OPTIONS_LATIN1_IS_خطی;
    }

    سرخی->اشاریہ_لمبائی=trie->اشاریہ_لمبائی;
    سرخی->ڈیٹا_لمبائی=trie->ڈیٹا_لمبائی;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    اگر(reduce_To16_بٹ) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->اشاریہ;
        dest16=(uint16_t *)ڈیٹا;
        برائے(i=trie->اشاریہ_لمبائی; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->اشاریہ_لمبائی)>>UTRIE_اشاریہ_سرکاؤ);
        }

        /* write 16-bit data values */
        p=trie->ڈیٹا;
        برائے(i=trie->ڈیٹا_لمبائی; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } ورنہ {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->اشاریہ;
        dest16=(uint16_t *)ڈیٹا;
        برائے(i=trie->اشاریہ_لمبائی; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_اشاریہ_سرکاؤ);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->ڈیٹا, 4*(size_t)trie->ڈیٹا_لمبائی);
    }

    واپس length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_default_Get_Folding_انحراف(uint32_t ڈیٹا) {
    واپس (int32_t)ڈیٹا;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ثابت خالی *ڈیٹا, int32_t length, UError_رمز *p_Error_رمز) {
    ثابت UTrie_سرخی *سرخی;
    ثابت uint16_t *p16;
    uint32_t options;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس -1;
    }

    /* enough data for a trie header? */
    اگر(length<(int32_t)حجم(UTrie_سرخی)) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس -1;
    }

    /* check the signature */
    سرخی=(ثابت UTrie_سرخی *)ڈیٹا;
    اگر(سرخی->signature!=0x54726965) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس -1;
    }

    /* get the options and check the shift values */
    options=سرخی->options;
    اگر( (options&UTRIE_OPTIONS_سرکاؤ_نقاب)!=UTRIE_سرکاؤ ||
        ((options>>UTRIE_OPTIONS_اشاریہ_سرکاؤ)&UTRIE_OPTIONS_سرکاؤ_نقاب)!=UTRIE_اشاریہ_سرکاؤ
    ) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس -1;
    }
    trie->is_Latin1_خطی = (options & UTRIE_OPTIONS_LATIN1_IS_خطی) != 0;

    /* get the length values */
    trie->اشاریہ_لمبائی=سرخی->اشاریہ_لمبائی;
    trie->ڈیٹا_لمبائی=سرخی->ڈیٹا_لمبائی;

    length-=(int32_t)حجم(UTrie_سرخی);

    /* enough data for the index? */
    اگر(length<2*trie->اشاریہ_لمبائی) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس -1;
    }
    p16=(ثابت uint16_t *)(سرخی+1);
    trie->اشاریہ=p16;
    p16+=trie->اشاریہ_لمبائی;
    length-=2*trie->اشاریہ_لمبائی;

    /* get the data */
    اگر(options&UTRIE_OPTIONS_ڈیٹا_IS_32_ثنائی_ہندسہ) {
        اگر(length<4*trie->ڈیٹا_لمبائی) {
            *p_Error_رمز=U_INVALID_FORMAT_ERROR;
            واپس -1;
        }
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_قدر=trie->data32[0];
        length=(int32_t)حجم(UTrie_سرخی)+2*trie->اشاریہ_لمبائی+4*trie->ڈیٹا_لمبائی;
    } ورنہ {
        اگر(length<2*trie->ڈیٹا_لمبائی) {
            *p_Error_رمز=U_INVALID_FORMAT_ERROR;
            واپس -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=خالی_اشاریہ;
        trie->initial_قدر=trie->اشاریہ[trie->اشاریہ_لمبائی];
        length=(int32_t)حجم(UTrie_سرخی)+2*trie->اشاریہ_لمبائی+2*trie->ڈیٹا_لمبائی;
    }

    trie->get_Folding_انحراف=utrie_default_Get_Folding_انحراف;

    واپس length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       خالی *ڈیٹا, int32_t length,
                       uint32_t initial_قدر, uint32_t lead_Unit_قدر,
                       UBool make16_ثنائی_ہندسہ_Trie,
                       UError_رمز *p_Error_رمز) {
    uint16_t *p16;
    int32_t actual_لمبائی, latin1_لمبائی, i, حد_بندی;
    uint16_t بلاک;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_لمبائی= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->اشاریہ_لمبائی=UTRIE_BMP_اشاریہ_لمبائی+UTRIE_SURROGATE_بلاک_گنتی;
    trie->ڈیٹا_لمبائی=latin1_لمبائی;
    اگر(lead_Unit_قدر!=initial_قدر) {
        trie->ڈیٹا_لمبائی+=UTRIE_ڈیٹا_بلاک_لمبائی;
    }

    actual_لمبائی=trie->اشاریہ_لمبائی*2;
    اگر(make16_ثنائی_ہندسہ_Trie) {
        actual_لمبائی+=trie->ڈیٹا_لمبائی*2;
    } ورنہ {
        actual_لمبائی+=trie->ڈیٹا_لمبائی*4;
    }

    /* enough space for the dummy trie? */
    اگر(length<actual_لمبائی) {
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        واپس actual_لمبائی;
    }

    trie->is_Latin1_خطی=سچ;
    trie->initial_قدر=initial_قدر;

    /* fill the index and data arrays */
    p16=(uint16_t *)ڈیٹا;
    trie->اشاریہ=p16;

    اگر(make16_ثنائی_ہندسہ_Trie) {
        /* indexes to block 0 */
        بلاک=(uint16_t)(trie->اشاریہ_لمبائی>>UTRIE_اشاریہ_سرکاؤ);
        حد_بندی=trie->اشاریہ_لمبائی;
        برائے(i=0; i<حد_بندی; ++i) {
            p16[i]=بلاک;
        }

        اگر(lead_Unit_قدر!=initial_قدر) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلاک+=(uint16_t)(latin1_لمبائی>>UTRIE_اشاریہ_سرکاؤ);
            i=0xd800>>UTRIE_سرکاؤ;
            حد_بندی=0xdc00>>UTRIE_سرکاؤ;
            برائے(; i<حد_بندی; ++i) {
                p16[i]=بلاک;
            }
        }

        trie->data32=خالی_اشاریہ;

        /* Latin-1 data */
        p16+=trie->اشاریہ_لمبائی;
        برائے(i=0; i<latin1_لمبائی; ++i) {
            p16[i]=(uint16_t)initial_قدر;
        }

        /* data for lead surrogate code units */
        اگر(lead_Unit_قدر!=initial_قدر) {
            حد_بندی=latin1_لمبائی+UTRIE_ڈیٹا_بلاک_لمبائی;
            برائے(/* i=latin1Length */; i<حد_بندی; ++i) {
                p16[i]=(uint16_t)lead_Unit_قدر;
            }
        }
    } ورنہ {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->اشاریہ_لمبائی*2);

        اگر(lead_Unit_قدر!=initial_قدر) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلاک=(uint16_t)(latin1_لمبائی>>UTRIE_اشاریہ_سرکاؤ);
            i=0xd800>>UTRIE_سرکاؤ;
            حد_بندی=0xdc00>>UTRIE_سرکاؤ;
            برائے(; i<حد_بندی; ++i) {
                p16[i]=بلاک;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->اشاریہ_لمبائی);

        /* Latin-1 data */
        برائے(i=0; i<latin1_لمبائی; ++i) {
            p32[i]=initial_قدر;
        }

        /* data for lead surrogate code units */
        اگر(lead_Unit_قدر!=initial_قدر) {
            حد_بندی=latin1_لمبائی+UTRIE_ڈیٹا_بلاک_لمبائی;
            برائے(/* i=latin1Length */; i<حد_بندی; ++i) {
                p32[i]=lead_Unit_قدر;
            }
        }
    }

    trie->get_Folding_انحراف=utrie_default_Get_Folding_انحراف;

    واپس actual_لمبائی;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
ساکن uint32_t U_CALLCONV
enum_Same_قدر(ثابت خالی * /*context*/, uint32_t value) {
    واپس value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI خالی U_EXPORT2
utrie_enum(ثابت UTrie *trie,
           UTrie_Enum_قدر *enum_قدر, UTrieEnumRange *enumRange, ثابت خالی *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, پچھلا_قدر, initial_قدر;
    UChar32 c, پچھلا;
    int32_t l, i, j, بلاک, پچھلا_بلاک, null_بلاک, انحراف;

    /* check arguments */
    اگر(trie==خالی_اشاریہ || trie->اشاریہ==خالی_اشاریہ || enumRange==خالی_اشاریہ) {
        واپس;
    }
    اگر(enum_قدر==خالی_اشاریہ) {
        enum_قدر=enum_Same_قدر;
    }

    idx=trie->اشاریہ;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_قدر=enum_قدر(context, trie->initial_قدر);

    اگر(data32==خالی_اشاریہ) {
        null_بلاک=trie->اشاریہ_لمبائی;
    } ورنہ {
        null_بلاک=0;
    }

    /* set variables for previous range */
    پچھلا_بلاک=null_بلاک;
    پچھلا=0;
    پچھلا_قدر=initial_قدر;

    /* enumerate BMP - the main loop enumerates data blocks */
    برائے(i=0, c=0; c<=0xffff; ++i) {
        اگر(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_اشاریہ_لمبائی;
        } ورنہ اگر(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_سرکاؤ;
        }

        بلاک=idx[i]<<UTRIE_اشاریہ_سرکاؤ;
        اگر(بلاک==پچھلا_بلاک) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_ڈیٹا_بلاک_لمبائی;
        } ورنہ اگر(بلاک==null_بلاک) {
            /* this is the all-initial-value block */
            اگر(پچھلا_قدر!=initial_قدر) {
                اگر(پچھلا<c) {
                    اگر(!enumRange(context, پچھلا, c, پچھلا_قدر)) {
                        واپس;
                    }
                }
                پچھلا_بلاک=null_بلاک;
                پچھلا=c;
                پچھلا_قدر=initial_قدر;
            }
            c+=UTRIE_ڈیٹا_بلاک_لمبائی;
        } ورنہ {
            پچھلا_بلاک=بلاک;
            برائے(j=0; j<UTRIE_ڈیٹا_بلاک_لمبائی; ++j) {
                value=enum_قدر(context, data32!=خالی_اشاریہ ? data32[بلاک+j] : idx[بلاک+j]);
                اگر(value!=پچھلا_قدر) {
                    اگر(پچھلا<c) {
                        اگر(!enumRange(context, پچھلا, c, پچھلا_قدر)) {
                            واپس;
                        }
                    }
                    اگر(j>0) {
                        /* the block is not filled with all the same value */
                        پچھلا_بلاک=-1;
                    }
                    پچھلا=c;
                    پچھلا_قدر=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    برائے(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        انحراف=idx[l>>UTRIE_سرکاؤ]<<UTRIE_اشاریہ_سرکاؤ;
        اگر(انحراف==null_بلاک) {
            /* no entries for a whole block of lead surrogates */
            اگر(پچھلا_قدر!=initial_قدر) {
                اگر(پچھلا<c) {
                    اگر(!enumRange(context, پچھلا, c, پچھلا_قدر)) {
                        واپس;
                    }
                }
                پچھلا_بلاک=null_بلاک;
                پچھلا=c;
                پچھلا_قدر=initial_قدر;
            }

            l+=UTRIE_ڈیٹا_بلاک_لمبائی;
            c+=UTRIE_ڈیٹا_بلاک_لمبائی<<10;
            جاری;
        }

        value= data32!=خالی_اشاریہ ? data32[انحراف+(l&UTRIE_نقاب)] : idx[انحراف+(l&UTRIE_نقاب)];

        /* enumerate trail surrogates for this lead surrogate */
        انحراف=trie->get_Folding_انحراف(value);
        اگر(انحراف<=0) {
            /* no data for this lead surrogate */
            اگر(پچھلا_قدر!=initial_قدر) {
                اگر(پچھلا<c) {
                    اگر(!enumRange(context, پچھلا, c, پچھلا_قدر)) {
                        واپس;
                    }
                }
                پچھلا_بلاک=null_بلاک;
                پچھلا=c;
                پچھلا_قدر=initial_قدر;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } ورنہ {
            /* enumerate code points for this lead surrogate */
            i=انحراف;
            انحراف+=UTRIE_SURROGATE_بلاک_گنتی;
            کرو {
                /* copy of most of the body of the BMP loop */
                بلاک=idx[i]<<UTRIE_اشاریہ_سرکاؤ;
                اگر(بلاک==پچھلا_بلاک) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_ڈیٹا_بلاک_لمبائی;
                } ورنہ اگر(بلاک==null_بلاک) {
                    /* this is the all-initial-value block */
                    اگر(پچھلا_قدر!=initial_قدر) {
                        اگر(پچھلا<c) {
                            اگر(!enumRange(context, پچھلا, c, پچھلا_قدر)) {
                                واپس;
                            }
                        }
                        پچھلا_بلاک=null_بلاک;
                        پچھلا=c;
                        پچھلا_قدر=initial_قدر;
                    }
                    c+=UTRIE_ڈیٹا_بلاک_لمبائی;
                } ورنہ {
                    پچھلا_بلاک=بلاک;
                    برائے(j=0; j<UTRIE_ڈیٹا_بلاک_لمبائی; ++j) {
                        value=enum_قدر(context, data32!=خالی_اشاریہ ? data32[بلاک+j] : idx[بلاک+j]);
                        اگر(value!=پچھلا_قدر) {
                            اگر(پچھلا<c) {
                                اگر(!enumRange(context, پچھلا, c, پچھلا_قدر)) {
                                    واپس;
                                }
                            }
                            اگر(j>0) {
                                /* the block is not filled with all the same value */
                                پچھلا_بلاک=-1;
                            }
                            پچھلا=c;
                            پچھلا_قدر=value;
                        }
                        ++c;
                    }
                }
            } جب(++i<انحراف);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, پچھلا, c, پچھلا_قدر);
}

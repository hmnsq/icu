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

#جيڪڏهن_وصف_ٿيل UTRIE_DEBUG
#   include <stdio.h>
#جيڪڏهن_پورو

#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#وصف_رد ABS
#وصف_ڪريو ABS(x) ((x)>=0 ? (x) : -(x))

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
utrie_open(UNewTrie *fill_اندريون,
           uint32_t *alias_معلومات, int32_t max_معلومات_ڊيگهائي,
           uint32_t initial_قدر, uint32_t lead_Unit_قدر,
           UBool latin1_لڪيري) {
    UNewTrie *trie;
    int32_t i, j;

    إذا( max_معلومات_ڊيگهائي<UTRIE_معلومات_بلاڪ_ڊيگهائي ||
        (latin1_لڪيري && max_معلومات_ڊيگهائي<1024)
    ) {
        أرجع خالي_اشارو;
    }

    إذا(fill_اندريون!=خالي_اشارو) {
        trie=fill_اندريون;
    } وإلا {
        trie=(UNewTrie *)uprv_malloc(حجم(UNewTrie));
        إذا(trie==خالي_اشارو) {
            أرجع خالي_اشارو;
        }
    }
    uprv_memset(trie, 0, حجم(UNewTrie));
    trie->isAllocated = fill_اندريون == خالي_اشارو;

    إذا(alias_معلومات!=خالي_اشارو) {
        trie->معلومات=alias_معلومات;
        trie->is_معلومات_Allocated=خطأ;
    } وإلا {
        trie->معلومات=(uint32_t *)uprv_malloc(max_معلومات_ڊيگهائي*4);
        إذا(trie->معلومات==خالي_اشارو) {
            uprv_free(trie);
            أرجع خالي_اشارو;
        }
        trie->is_معلومات_Allocated=سچ;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_معلومات_بلاڪ_ڊيگهائي;

    إذا(latin1_لڪيري) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        افعل {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->اشاريو[i++]=j;
            j+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
        } طالما(i<(256>>UTRIE_سرڪايو));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->معلومات_ڊيگهائي=j;
    طالما(j>0) {
        trie->معلومات[--j]=initial_قدر;
    }

    trie->lead_Unit_قدر=lead_Unit_قدر;
    trie->اشاريو_ڊيگهائي=UTRIE_MAX_اشاريو_ڊيگهائي;
    trie->معلومات_صلاحيت=max_معلومات_ڊيگهائي;
    trie->is_Latin1_لڪيري=latin1_لڪيري;
    trie->isCompacted=خطأ;
    أرجع trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fill_اندريون, ثابت UNewTrie *other, uint32_t *alias_معلومات, int32_t alias_معلومات_صلاحيت) {
    UNewTrie *trie;
    UBool is_معلومات_Allocated;

    /* do not clone if other is not valid or already compacted */
    إذا(other==خالي_اشارو || other->معلومات==خالي_اشارو || other->isCompacted) {
        أرجع خالي_اشارو;
    }

    /* clone data */
    إذا(alias_معلومات!=خالي_اشارو && alias_معلومات_صلاحيت>=other->معلومات_صلاحيت) {
        is_معلومات_Allocated=خطأ;
    } وإلا {
        alias_معلومات_صلاحيت=other->معلومات_صلاحيت;
        alias_معلومات=(uint32_t *)uprv_malloc(other->معلومات_صلاحيت*4);
        إذا(alias_معلومات==خالي_اشارو) {
            أرجع خالي_اشارو;
        }
        is_معلومات_Allocated=سچ;
    }

    trie=utrie_open(fill_اندريون, alias_معلومات, alias_معلومات_صلاحيت,
                    other->معلومات[0], other->lead_Unit_قدر,
                    other->is_Latin1_لڪيري);
    إذا(trie==خالي_اشارو) {
        uprv_free(alias_معلومات);
    } وإلا {
        uprv_memcpy(trie->اشاريو, other->اشاريو, حجم(trie->اشاريو));
        uprv_memcpy(trie->معلومات, other->معلومات, (size_t)other->معلومات_ڊيگهائي*4);
        trie->معلومات_ڊيگهائي=other->معلومات_ڊيگهائي;
        trie->is_معلومات_Allocated=is_معلومات_Allocated;
    }

    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
utrie_close(UNewTrie *trie) {
    إذا(trie!=خالي_اشارو) {
        إذا(trie->is_معلومات_Allocated) {
            uprv_free(trie->معلومات);
            trie->معلومات=خالي_اشارو;
        }
        إذا(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_معلومات(UNewTrie *trie, int32_t *p_ڊيگهائي) {
    إذا(trie==خالي_اشارو || p_ڊيگهائي==خالي_اشارو) {
        أرجع خالي_اشارو;
    }

    *p_ڊيگهائي=trie->معلومات_ڊيگهائي;
    أرجع trie->معلومات;
}

ساكن int32_t
utrie_alloc_معلومات_بلاڪ(UNewTrie *trie) {
    int32_t new_بلاڪ, new_چوٽي;

    new_بلاڪ=trie->معلومات_ڊيگهائي;
    new_چوٽي=new_بلاڪ+UTRIE_معلومات_بلاڪ_ڊيگهائي;
    إذا(new_چوٽي>trie->معلومات_صلاحيت) {
        /* out of memory in the data array */
        أرجع -1;
    }
    trie->معلومات_ڊيگهائي=new_چوٽي;
    أرجع new_بلاڪ;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ساكن int32_t
utrie_get_معلومات_بلاڪ(UNewTrie *trie, UChar32 c) {
    int32_t اشاريو_قدر, new_بلاڪ;

    c>>=UTRIE_سرڪايو;
    اشاريو_قدر=trie->اشاريو[c];
    إذا(اشاريو_قدر>0) {
        أرجع اشاريو_قدر;
    }

    /* allocate a new data block */
    new_بلاڪ=utrie_alloc_معلومات_بلاڪ(trie);
    إذا(new_بلاڪ<0) {
        /* out of memory in the data array */
        أرجع -1;
    }
    trie->اشاريو[c]=new_بلاڪ;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->معلومات+new_بلاڪ, trie->معلومات-اشاريو_قدر, 4*UTRIE_معلومات_بلاڪ_ڊيگهائي);
    أرجع new_بلاڪ;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t بلاڪ;

    /* valid, uncompacted trie and valid c? */
    إذا(trie==خالي_اشارو || trie->isCompacted || (uint32_t)c>0x10ffff) {
        أرجع خطأ;
    }

    بلاڪ=utrie_get_معلومات_بلاڪ(trie, c);
    إذا(بلاڪ<0) {
        أرجع خطأ;
    }

    trie->معلومات[بلاڪ+(c&UTRIE_نقاب)]=value;
    أرجع سچ;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *p_اندريون_بلاڪ_Zero) {
    int32_t بلاڪ;

    /* valid, uncompacted trie and valid c? */
    إذا(trie==خالي_اشارو || trie->isCompacted || (uint32_t)c>0x10ffff) {
        إذا(p_اندريون_بلاڪ_Zero!=خالي_اشارو) {
            *p_اندريون_بلاڪ_Zero=سچ;
        }
        أرجع 0;
    }

    بلاڪ=trie->اشاريو[c>>UTRIE_سرڪايو];
    إذا(p_اندريون_بلاڪ_Zero!=خالي_اشارو) {
        *p_اندريون_بلاڪ_Zero = بلاڪ == 0;
    }

    أرجع trie->معلومات[ABS(بلاڪ)+(c&UTRIE_نقاب)];
}

/**
 * @internal
 */
ساكن فراغ
utrie_fill_بلاڪ(uint32_t *بلاڪ, UChar32 شروعات, UChar32 حد_بندي,
                uint32_t value, uint32_t initial_قدر, UBool overwrite) {
    uint32_t *p_حد_بندي;

    p_حد_بندي=بلاڪ+حد_بندي;
    بلاڪ+=شروعات;
    إذا(overwrite) {
        طالما(بلاڪ<p_حد_بندي) {
            *بلاڪ++=value;
        }
    } وإلا {
        طالما(بلاڪ<p_حد_بندي) {
            إذا(*بلاڪ==initial_قدر) {
                *بلاڪ=value;
            }
            ++بلاڪ;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_سيٽ_Range32(UNewTrie *trie, UChar32 شروعات, UChar32 حد_بندي, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initial_قدر;
    int32_t بلاڪ, rest, repeat_بلاڪ;

    /* valid, uncompacted trie and valid indexes? */
    إذا( trie==خالي_اشارو || trie->isCompacted ||
        (uint32_t)شروعات>0x10ffff || (uint32_t)حد_بندي>0x110000 || شروعات>حد_بندي
    ) {
        أرجع خطأ;
    }
    إذا(شروعات==حد_بندي) {
        أرجع سچ; /* nothing to do */
    }

    initial_قدر=trie->معلومات[0];
    إذا(شروعات&UTRIE_نقاب) {
        UChar32 ايندڙ_شروعات;

        /* set partial block at [start..following block boundary[ */
        بلاڪ=utrie_get_معلومات_بلاڪ(trie, شروعات);
        إذا(بلاڪ<0) {
            أرجع خطأ;
        }

        ايندڙ_شروعات=(شروعات+UTRIE_معلومات_بلاڪ_ڊيگهائي)&~UTRIE_نقاب;
        إذا(ايندڙ_شروعات<=حد_بندي) {
            utrie_fill_بلاڪ(trie->معلومات+بلاڪ, شروعات&UTRIE_نقاب, UTRIE_معلومات_بلاڪ_ڊيگهائي,
                            value, initial_قدر, overwrite);
            شروعات=ايندڙ_شروعات;
        } وإلا {
            utrie_fill_بلاڪ(trie->معلومات+بلاڪ, شروعات&UTRIE_نقاب, حد_بندي&UTRIE_نقاب,
                            value, initial_قدر, overwrite);
            أرجع سچ;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد_بندي&UTRIE_نقاب;

    /* round down limit to a block boundary */
    حد_بندي&=~UTRIE_نقاب;

    /* iterate over all-value blocks */
    إذا(value==initial_قدر) {
        repeat_بلاڪ=0;
    } وإلا {
        repeat_بلاڪ=-1;
    }
    طالما(شروعات<حد_بندي) {
        /* get index value */
        بلاڪ=trie->اشاريو[شروعات>>UTRIE_سرڪايو];
        إذا(بلاڪ>0) {
            /* already allocated, fill in value */
            utrie_fill_بلاڪ(trie->معلومات+بلاڪ, 0, UTRIE_معلومات_بلاڪ_ڊيگهائي, value, initial_قدر, overwrite);
        } وإلا إذا(trie->معلومات[-بلاڪ]!=value && (بلاڪ==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            إذا(repeat_بلاڪ>=0) {
                trie->اشاريو[شروعات>>UTRIE_سرڪايو]=-repeat_بلاڪ;
            } وإلا {
                /* create and set and fill the repeatBlock */
                repeat_بلاڪ=utrie_get_معلومات_بلاڪ(trie, شروعات);
                إذا(repeat_بلاڪ<0) {
                    أرجع خطأ;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->اشاريو[شروعات>>UTRIE_سرڪايو]=-repeat_بلاڪ;
                utrie_fill_بلاڪ(trie->معلومات+repeat_بلاڪ, 0, UTRIE_معلومات_بلاڪ_ڊيگهائي, value, initial_قدر, سچ);
            }
        }

        شروعات+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
    }

    إذا(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلاڪ=utrie_get_معلومات_بلاڪ(trie, شروعات);
        إذا(بلاڪ<0) {
            أرجع خطأ;
        }

        utrie_fill_بلاڪ(trie->معلومات+بلاڪ, 0, rest, value, initial_قدر, overwrite);
    }

    أرجع سچ;
}

ساكن int32_t
ڳوليو_Same_اشاريو_بلاڪ(ثابت int32_t *idx, int32_t اشاريو_ڊيگهائي,
                    int32_t other_بلاڪ) {
    int32_t بلاڪ, i;

    لكل(بلاڪ=UTRIE_BMP_اشاريو_ڊيگهائي; بلاڪ<اشاريو_ڊيگهائي; بلاڪ+=UTRIE_SURROGATE_بلاڪ_ڳڻپ) {
        لكل(i=0; i<UTRIE_SURROGATE_بلاڪ_ڳڻپ; ++i) {
            إذا(idx[بلاڪ+i]!=idx[other_بلاڪ+i]) {
                اكسر;
            }
        }
        إذا(i==UTRIE_SURROGATE_بلاڪ_ڳڻپ) {
            أرجع بلاڪ;
        }
    }
    أرجع اشاريو_ڊيگهائي;
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
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_قدر *get_Folded_قدر, UError_ڪوڊ *p_Error_ڪوڊ) {
    int32_t leadIndexes[UTRIE_SURROGATE_بلاڪ_ڳڻپ];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t اشاريو_ڊيگهائي, بلاڪ;
#جيڪڏهن_وصف_ٿيل UTRIE_DEBUG
    صحيح ڳڻپ_Lead_CUWith_معلومات=0;
#جيڪڏهن_پورو

    idx=trie->اشاريو;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_سرڪايو), 4*UTRIE_SURROGATE_بلاڪ_ڳڻپ);

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
    إذا(trie->lead_Unit_قدر==trie->معلومات[0]) {
        بلاڪ=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } وإلا {
        /* create and fill the repeatBlock */
        بلاڪ=utrie_alloc_معلومات_بلاڪ(trie);
        إذا(بلاڪ<0) {
            /* data table overflow */
            *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
        utrie_fill_بلاڪ(trie->معلومات+بلاڪ, 0, UTRIE_معلومات_بلاڪ_ڊيگهائي, trie->lead_Unit_قدر, trie->معلومات[0], سچ);
        بلاڪ=-بلاڪ; /* negative block number to indicate that it is a repeat block */
    }
    لكل(c=(0xd800>>UTRIE_سرڪايو); c<(0xdc00>>UTRIE_سرڪايو); ++c) {
        trie->اشاريو[c]=بلاڪ;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    اشاريو_ڊيگهائي=UTRIE_BMP_اشاريو_ڊيگهائي;

    /* search for any index (stage 1) entries for supplementary code points */
    لكل(c=0x10000; c<0x110000;) {
        إذا(idx[c>>UTRIE_سرڪايو]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#جيڪڏهن_وصف_ٿيل UTRIE_DEBUG
            ++ڳڻپ_Lead_CUWith_معلومات;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#جيڪڏهن_پورو

            /* is there an identical index block? */
            بلاڪ=ڳوليو_Same_اشاريو_بلاڪ(idx, اشاريو_ڊيگهائي, c>>UTRIE_سرڪايو);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_قدر(trie, c, بلاڪ+UTRIE_SURROGATE_بلاڪ_ڳڻپ);
            إذا(value!=utrie_get32(trie, U16_LEAD(c), خالي_اشارو)) {
                إذا(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                    أرجع;
                }

                /* if we did not find an identical index block... */
                إذا(بلاڪ==اشاريو_ڊيگهائي) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+اشاريو_ڊيگهائي,
                                 idx+(c>>UTRIE_سرڪايو),
                                 4*UTRIE_SURROGATE_بلاڪ_ڳڻپ);
                    اشاريو_ڊيگهائي+=UTRIE_SURROGATE_بلاڪ_ڳڻپ;
                }
            }
            c+=0x400;
        } وإلا {
            c+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
        }
    }
#جيڪڏهن_وصف_ٿيل UTRIE_DEBUG
    إذا(ڳڻپ_Lead_CUWith_معلومات>0) {
        لکو("supplementary data for %d lead surrogates\n", ڳڻپ_Lead_CUWith_معلومات);
    }
#جيڪڏهن_پورو

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    إذا(اشاريو_ڊيگهائي>=UTRIE_MAX_اشاريو_ڊيگهائي) {
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_اشاريو_ڊيگهائي+UTRIE_SURROGATE_بلاڪ_ڳڻپ,
                 idx+UTRIE_BMP_اشاريو_ڊيگهائي,
                 4*(اشاريو_ڊيگهائي-UTRIE_BMP_اشاريو_ڊيگهائي));
    uprv_memcpy(idx+UTRIE_BMP_اشاريو_ڊيگهائي,
                leadIndexes,
                4*UTRIE_SURROGATE_بلاڪ_ڳڻپ);
    اشاريو_ڊيگهائي+=UTRIE_SURROGATE_بلاڪ_ڳڻپ;

#جيڪڏهن_وصف_ٿيل UTRIE_DEBUG
    لکو("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_اشاريو_ڊيگهائي, (ڊگهو)UTRIE_MAX_اشاريو_ڊيگهائي, اشاريو_ڊيگهائي);
#جيڪڏهن_پورو

    trie->اشاريو_ڊيگهائي=اشاريو_ڊيگهائي;
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
ڳوليو_اڻ_استعمال_ٿيل_بلاڪون(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_وقت_معلومات_ڊيگهائي>>UTRIE_سرڪايو)*4);

    /* mark each block that _is_ used with 0 */
    لكل(i=0; i<trie->اشاريو_ڊيگهائي; ++i) {
        trie->map[ABS(trie->اشاريو[i])>>UTRIE_سرڪايو]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

ساكن int32_t
ڳوليو_Same_معلومات_بلاڪ(ثابت uint32_t *معلومات, int32_t معلومات_ڊيگهائي,
                   int32_t other_بلاڪ, int32_t قدم) {
    int32_t بلاڪ;

    /* ensure that we do not even partially get past dataLength */
    معلومات_ڊيگهائي-=UTRIE_معلومات_بلاڪ_ڊيگهائي;

    لكل(بلاڪ=0; بلاڪ<=معلومات_ڊيگهائي; بلاڪ+=قدم) {
        إذا(equal_uint32(معلومات+بلاڪ, معلومات+other_بلاڪ, UTRIE_معلومات_بلاڪ_ڊيگهائي)) {
            أرجع بلاڪ;
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
utrie_compact(UNewTrie *trie, UBool overlap, UError_ڪوڊ *p_Error_ڪوڊ) {
    int32_t i, شروعات, new_شروعات, overlap_شروعات;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }

    /* valid, uncompacted trie? */
    إذا(trie==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    إذا(trie->isCompacted) {
        أرجع; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    ڳوليو_اڻ_استعمال_ٿيل_بلاڪون(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    إذا(trie->is_Latin1_لڪيري && UTRIE_سرڪايو<=8) {
        overlap_شروعات=UTRIE_معلومات_بلاڪ_ڊيگهائي+256;
    } وإلا {
        overlap_شروعات=UTRIE_معلومات_بلاڪ_ڊيگهائي;
    }

    new_شروعات=UTRIE_معلومات_بلاڪ_ڊيگهائي;
    لكل(شروعات=new_شروعات; شروعات<trie->معلومات_ڊيگهائي;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        إذا(trie->map[شروعات>>UTRIE_سرڪايو]<0) {
            /* advance start to the next block */
            شروعات+=UTRIE_معلومات_بلاڪ_ڊيگهائي;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* search for an identical block */
        إذا( شروعات>=overlap_شروعات &&
            (i=ڳوليو_Same_معلومات_بلاڪ(trie->معلومات, new_شروعات, شروعات,
                            overlap ? UTRIE_معلومات_GRANULARITY : UTRIE_معلومات_بلاڪ_ڊيگهائي))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[شروعات>>UTRIE_سرڪايو]=i;

            /* advance start to the next block */
            شروعات+=UTRIE_معلومات_بلاڪ_ڊيگهائي;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        إذا(overlap && شروعات>=overlap_شروعات) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            لكل(i=UTRIE_معلومات_بلاڪ_ڊيگهائي-UTRIE_معلومات_GRANULARITY;
                i>0 && !equal_uint32(trie->معلومات+(new_شروعات-i), trie->معلومات+شروعات, i);
                i-=UTRIE_معلومات_GRANULARITY) {}
        } وإلا {
            i=0;
        }

        إذا(i>0) {
            /* some overlap */
            trie->map[شروعات>>UTRIE_سرڪايو]=new_شروعات-i;

            /* move the non-overlapping indexes to their new positions */
            شروعات+=i;
            لكل(i=UTRIE_معلومات_بلاڪ_ڊيگهائي-i; i>0; --i) {
                trie->معلومات[new_شروعات++]=trie->معلومات[شروعات++];
            }
        } وإلا إذا(new_شروعات<شروعات) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[شروعات>>UTRIE_سرڪايو]=new_شروعات;
            لكل(i=UTRIE_معلومات_بلاڪ_ڊيگهائي; i>0; --i) {
                trie->معلومات[new_شروعات++]=trie->معلومات[شروعات++];
            }
        } وإلا /* no overlap && newStart==start */ {
            trie->map[شروعات>>UTRIE_سرڪايو]=شروعات;
            new_شروعات+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
            شروعات=new_شروعات;
        }
    }

    /* now adjust the index (stage 1) table */
    لكل(i=0; i<trie->اشاريو_ڊيگهائي; ++i) {
        trie->اشاريو[i]=trie->map[ABS(trie->اشاريو[i])>>UTRIE_سرڪايو];
    }

#جيڪڏهن_وصف_ٿيل UTRIE_DEBUG
    /* we saved some space */
    لکو("compacting trie: count of 32-bit words %lu->%lu\n",
            (ڊگهو)trie->معلومات_ڊيگهائي, (ڊگهو)new_شروعات);
#جيڪڏهن_پورو

    trie->معلومات_ڊيگهائي=new_شروعات;
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
default_Get_Folded_قدر(UNewTrie *trie, UChar32 شروعات, int32_t وٿي) {
    uint32_t value, initial_قدر;
    UChar32 حد_بندي;
    UBool اندريون_بلاڪ_Zero;

    initial_قدر=trie->معلومات[0];
    حد_بندي=شروعات+0x400;
    طالما(شروعات<حد_بندي) {
        value=utrie_get32(trie, شروعات, &اندريون_بلاڪ_Zero);
        إذا(اندريون_بلاڪ_Zero) {
            شروعات+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
        } وإلا إذا(value!=initial_قدر) {
            أرجع static_cast<uint32_t>(وٿي);
        } وإلا {
            ++شروعات;
        }
    }
    أرجع 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, فراغ *dt, int32_t capacity,
                UNew_Trie_Get_Folded_قدر *get_Folded_قدر,
                UBool reduce_To16_بٽ,
                UError_ڪوڊ *p_Error_ڪوڊ) {
    UTrie_سرو *سرو;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* معلومات = خالي_اشارو;

    /* argument check */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    إذا(trie==خالي_اشارو || capacity<0 || (capacity>0 && dt==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    إذا(get_Folded_قدر==خالي_اشارو) {
        get_Folded_قدر=default_Get_Folded_قدر;
    }

    معلومات = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    إذا(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, خطأ, p_Error_ڪوڊ);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_قدر, p_Error_ڪوڊ);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, سچ, p_Error_ڪوڊ);

        trie->isCompacted=سچ;
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع 0;
        }
    }

    /* is dataLength within limits? */
    إذا( (reduce_To16_بٽ ? (trie->معلومات_ڊيگهائي+trie->اشاريو_ڊيگهائي) : trie->معلومات_ڊيگهائي) >= UTRIE_MAX_معلومات_ڊيگهائي) {
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
    }

    length=حجم(UTrie_سرو)+2*trie->اشاريو_ڊيگهائي;
    إذا(reduce_To16_بٽ) {
        length+=2*trie->معلومات_ڊيگهائي;
    } وإلا {
        length+=4*trie->معلومات_ڊيگهائي;
    }

    إذا(length>capacity) {
        أرجع length; /* preflighting */
    }

#جيڪڏهن_وصف_ٿيل UTRIE_DEBUG
    لکو("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (ڊگهو)trie->اشاريو_ڊيگهائي, (ڊگهو)trie->معلومات_ڊيگهائي, (ڊگهو)length);
#جيڪڏهن_پورو

    /* set the header fields */
    سرو=(UTrie_سرو *)معلومات;
    معلومات+=حجم(UTrie_سرو);

    سرو->signature=0x54726965; /* "Trie" */
    سرو->options=UTRIE_سرڪايو | (UTRIE_اشاريو_سرڪايو<<UTRIE_OPTIONS_اشاريو_سرڪايو);

    إذا(!reduce_To16_بٽ) {
        سرو->options|=UTRIE_OPTIONS_معلومات_IS_32_ٻٽو_انگ;
    }
    إذا(trie->is_Latin1_لڪيري) {
        سرو->options|=UTRIE_OPTIONS_LATIN1_IS_لڪيري;
    }

    سرو->اشاريو_ڊيگهائي=trie->اشاريو_ڊيگهائي;
    سرو->معلومات_ڊيگهائي=trie->معلومات_ڊيگهائي;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    إذا(reduce_To16_بٽ) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->اشاريو;
        dest16=(uint16_t *)معلومات;
        لكل(i=trie->اشاريو_ڊيگهائي; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->اشاريو_ڊيگهائي)>>UTRIE_اشاريو_سرڪايو);
        }

        /* write 16-bit data values */
        p=trie->معلومات;
        لكل(i=trie->معلومات_ڊيگهائي; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } وإلا {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->اشاريو;
        dest16=(uint16_t *)معلومات;
        لكل(i=trie->اشاريو_ڊيگهائي; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_اشاريو_سرڪايو);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->معلومات, 4*(size_t)trie->معلومات_ڊيگهائي);
    }

    أرجع length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_default_Get_Folding_وٿي(uint32_t معلومات) {
    أرجع (int32_t)معلومات;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ثابت فراغ *معلومات, int32_t length, UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UTrie_سرو *سرو;
    ثابت uint16_t *p16;
    uint32_t options;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع -1;
    }

    /* enough data for a trie header? */
    إذا(length<(int32_t)حجم(UTrie_سرو)) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }

    /* check the signature */
    سرو=(ثابت UTrie_سرو *)معلومات;
    إذا(سرو->signature!=0x54726965) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }

    /* get the options and check the shift values */
    options=سرو->options;
    إذا( (options&UTRIE_OPTIONS_سرڪايو_نقاب)!=UTRIE_سرڪايو ||
        ((options>>UTRIE_OPTIONS_اشاريو_سرڪايو)&UTRIE_OPTIONS_سرڪايو_نقاب)!=UTRIE_اشاريو_سرڪايو
    ) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }
    trie->is_Latin1_لڪيري = (options & UTRIE_OPTIONS_LATIN1_IS_لڪيري) != 0;

    /* get the length values */
    trie->اشاريو_ڊيگهائي=سرو->اشاريو_ڊيگهائي;
    trie->معلومات_ڊيگهائي=سرو->معلومات_ڊيگهائي;

    length-=(int32_t)حجم(UTrie_سرو);

    /* enough data for the index? */
    إذا(length<2*trie->اشاريو_ڊيگهائي) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }
    p16=(ثابت uint16_t *)(سرو+1);
    trie->اشاريو=p16;
    p16+=trie->اشاريو_ڊيگهائي;
    length-=2*trie->اشاريو_ڊيگهائي;

    /* get the data */
    إذا(options&UTRIE_OPTIONS_معلومات_IS_32_ٻٽو_انگ) {
        إذا(length<4*trie->معلومات_ڊيگهائي) {
            *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
            أرجع -1;
        }
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_قدر=trie->data32[0];
        length=(int32_t)حجم(UTrie_سرو)+2*trie->اشاريو_ڊيگهائي+4*trie->معلومات_ڊيگهائي;
    } وإلا {
        إذا(length<2*trie->معلومات_ڊيگهائي) {
            *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
            أرجع -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=خالي_اشارو;
        trie->initial_قدر=trie->اشاريو[trie->اشاريو_ڊيگهائي];
        length=(int32_t)حجم(UTrie_سرو)+2*trie->اشاريو_ڊيگهائي+2*trie->معلومات_ڊيگهائي;
    }

    trie->get_Folding_وٿي=utrie_default_Get_Folding_وٿي;

    أرجع length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       فراغ *معلومات, int32_t length,
                       uint32_t initial_قدر, uint32_t lead_Unit_قدر,
                       UBool make16_ٻٽو_انگ_Trie,
                       UError_ڪوڊ *p_Error_ڪوڊ) {
    uint16_t *p16;
    int32_t actual_ڊيگهائي, latin1_ڊيگهائي, i, حد_بندي;
    uint16_t بلاڪ;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_ڊيگهائي= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->اشاريو_ڊيگهائي=UTRIE_BMP_اشاريو_ڊيگهائي+UTRIE_SURROGATE_بلاڪ_ڳڻپ;
    trie->معلومات_ڊيگهائي=latin1_ڊيگهائي;
    إذا(lead_Unit_قدر!=initial_قدر) {
        trie->معلومات_ڊيگهائي+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
    }

    actual_ڊيگهائي=trie->اشاريو_ڊيگهائي*2;
    إذا(make16_ٻٽو_انگ_Trie) {
        actual_ڊيگهائي+=trie->معلومات_ڊيگهائي*2;
    } وإلا {
        actual_ڊيگهائي+=trie->معلومات_ڊيگهائي*4;
    }

    /* enough space for the dummy trie? */
    إذا(length<actual_ڊيگهائي) {
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
        أرجع actual_ڊيگهائي;
    }

    trie->is_Latin1_لڪيري=سچ;
    trie->initial_قدر=initial_قدر;

    /* fill the index and data arrays */
    p16=(uint16_t *)معلومات;
    trie->اشاريو=p16;

    إذا(make16_ٻٽو_انگ_Trie) {
        /* indexes to block 0 */
        بلاڪ=(uint16_t)(trie->اشاريو_ڊيگهائي>>UTRIE_اشاريو_سرڪايو);
        حد_بندي=trie->اشاريو_ڊيگهائي;
        لكل(i=0; i<حد_بندي; ++i) {
            p16[i]=بلاڪ;
        }

        إذا(lead_Unit_قدر!=initial_قدر) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلاڪ+=(uint16_t)(latin1_ڊيگهائي>>UTRIE_اشاريو_سرڪايو);
            i=0xd800>>UTRIE_سرڪايو;
            حد_بندي=0xdc00>>UTRIE_سرڪايو;
            لكل(; i<حد_بندي; ++i) {
                p16[i]=بلاڪ;
            }
        }

        trie->data32=خالي_اشارو;

        /* Latin-1 data */
        p16+=trie->اشاريو_ڊيگهائي;
        لكل(i=0; i<latin1_ڊيگهائي; ++i) {
            p16[i]=(uint16_t)initial_قدر;
        }

        /* data for lead surrogate code units */
        إذا(lead_Unit_قدر!=initial_قدر) {
            حد_بندي=latin1_ڊيگهائي+UTRIE_معلومات_بلاڪ_ڊيگهائي;
            لكل(/* i=latin1Length */; i<حد_بندي; ++i) {
                p16[i]=(uint16_t)lead_Unit_قدر;
            }
        }
    } وإلا {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->اشاريو_ڊيگهائي*2);

        إذا(lead_Unit_قدر!=initial_قدر) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلاڪ=(uint16_t)(latin1_ڊيگهائي>>UTRIE_اشاريو_سرڪايو);
            i=0xd800>>UTRIE_سرڪايو;
            حد_بندي=0xdc00>>UTRIE_سرڪايو;
            لكل(; i<حد_بندي; ++i) {
                p16[i]=بلاڪ;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->اشاريو_ڊيگهائي);

        /* Latin-1 data */
        لكل(i=0; i<latin1_ڊيگهائي; ++i) {
            p32[i]=initial_قدر;
        }

        /* data for lead surrogate code units */
        إذا(lead_Unit_قدر!=initial_قدر) {
            حد_بندي=latin1_ڊيگهائي+UTRIE_معلومات_بلاڪ_ڊيگهائي;
            لكل(/* i=latin1Length */; i<حد_بندي; ++i) {
                p32[i]=lead_Unit_قدر;
            }
        }
    }

    trie->get_Folding_وٿي=utrie_default_Get_Folding_وٿي;

    أرجع actual_ڊيگهائي;
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
    int32_t l, i, j, بلاڪ, prev_بلاڪ, null_بلاڪ, وٿي;

    /* check arguments */
    إذا(trie==خالي_اشارو || trie->اشاريو==خالي_اشارو || enumRange==خالي_اشارو) {
        أرجع;
    }
    إذا(enum_قدر==خالي_اشارو) {
        enum_قدر=enum_Same_قدر;
    }

    idx=trie->اشاريو;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_قدر=enum_قدر(context, trie->initial_قدر);

    إذا(data32==خالي_اشارو) {
        null_بلاڪ=trie->اشاريو_ڊيگهائي;
    } وإلا {
        null_بلاڪ=0;
    }

    /* set variables for previous range */
    prev_بلاڪ=null_بلاڪ;
    prev=0;
    prev_قدر=initial_قدر;

    /* enumerate BMP - the main loop enumerates data blocks */
    لكل(i=0, c=0; c<=0xffff; ++i) {
        إذا(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_اشاريو_ڊيگهائي;
        } وإلا إذا(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_سرڪايو;
        }

        بلاڪ=idx[i]<<UTRIE_اشاريو_سرڪايو;
        إذا(بلاڪ==prev_بلاڪ) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
        } وإلا إذا(بلاڪ==null_بلاڪ) {
            /* this is the all-initial-value block */
            إذا(prev_قدر!=initial_قدر) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_قدر)) {
                        أرجع;
                    }
                }
                prev_بلاڪ=null_بلاڪ;
                prev=c;
                prev_قدر=initial_قدر;
            }
            c+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
        } وإلا {
            prev_بلاڪ=بلاڪ;
            لكل(j=0; j<UTRIE_معلومات_بلاڪ_ڊيگهائي; ++j) {
                value=enum_قدر(context, data32!=خالي_اشارو ? data32[بلاڪ+j] : idx[بلاڪ+j]);
                إذا(value!=prev_قدر) {
                    إذا(prev<c) {
                        إذا(!enumRange(context, prev, c, prev_قدر)) {
                            أرجع;
                        }
                    }
                    إذا(j>0) {
                        /* the block is not filled with all the same value */
                        prev_بلاڪ=-1;
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
        وٿي=idx[l>>UTRIE_سرڪايو]<<UTRIE_اشاريو_سرڪايو;
        إذا(وٿي==null_بلاڪ) {
            /* no entries for a whole block of lead surrogates */
            إذا(prev_قدر!=initial_قدر) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_قدر)) {
                        أرجع;
                    }
                }
                prev_بلاڪ=null_بلاڪ;
                prev=c;
                prev_قدر=initial_قدر;
            }

            l+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
            c+=UTRIE_معلومات_بلاڪ_ڊيگهائي<<10;
            استمر;
        }

        value= data32!=خالي_اشارو ? data32[وٿي+(l&UTRIE_نقاب)] : idx[وٿي+(l&UTRIE_نقاب)];

        /* enumerate trail surrogates for this lead surrogate */
        وٿي=trie->get_Folding_وٿي(value);
        إذا(وٿي<=0) {
            /* no data for this lead surrogate */
            إذا(prev_قدر!=initial_قدر) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_قدر)) {
                        أرجع;
                    }
                }
                prev_بلاڪ=null_بلاڪ;
                prev=c;
                prev_قدر=initial_قدر;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } وإلا {
            /* enumerate code points for this lead surrogate */
            i=وٿي;
            وٿي+=UTRIE_SURROGATE_بلاڪ_ڳڻپ;
            افعل {
                /* copy of most of the body of the BMP loop */
                بلاڪ=idx[i]<<UTRIE_اشاريو_سرڪايو;
                إذا(بلاڪ==prev_بلاڪ) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
                } وإلا إذا(بلاڪ==null_بلاڪ) {
                    /* this is the all-initial-value block */
                    إذا(prev_قدر!=initial_قدر) {
                        إذا(prev<c) {
                            إذا(!enumRange(context, prev, c, prev_قدر)) {
                                أرجع;
                            }
                        }
                        prev_بلاڪ=null_بلاڪ;
                        prev=c;
                        prev_قدر=initial_قدر;
                    }
                    c+=UTRIE_معلومات_بلاڪ_ڊيگهائي;
                } وإلا {
                    prev_بلاڪ=بلاڪ;
                    لكل(j=0; j<UTRIE_معلومات_بلاڪ_ڊيگهائي; ++j) {
                        value=enum_قدر(context, data32!=خالي_اشارو ? data32[بلاڪ+j] : idx[بلاڪ+j]);
                        إذا(value!=prev_قدر) {
                            إذا(prev<c) {
                                إذا(!enumRange(context, prev, c, prev_قدر)) {
                                    أرجع;
                                }
                            }
                            إذا(j>0) {
                                /* the block is not filled with all the same value */
                                prev_بلاڪ=-1;
                            }
                            prev=c;
                            prev_قدر=value;
                        }
                        ++c;
                    }
                }
            } طالما(++i<وٿي);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_قدر);
}

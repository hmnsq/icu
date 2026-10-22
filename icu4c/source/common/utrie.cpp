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

#که_تعریف_شوی UTRIE_DEBUG
#   include <stdio.h>
#د_که_پای

#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#تعریف_لغوه ABS
#تعریف_کړه ABS(x) ((x)>=0 ? (x) : -(x))

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
utrie_open(UNewTrie *fill_دننه,
           uint32_t *alias_معلومات, int32_t max_معلومات_اوږدتیا,
           uint32_t initial_ارزښت, uint32_t lead_Unit_ارزښت,
           UBool latin1_خطي) {
    UNewTrie *trie;
    int32_t i, j;

    إذا( max_معلومات_اوږدتیا<UTRIE_معلومات_بلاک_اوږدتیا ||
        (latin1_خطي && max_معلومات_اوږدتیا<1024)
    ) {
        أرجع تش_اشاره;
    }

    إذا(fill_دننه!=تش_اشاره) {
        trie=fill_دننه;
    } وإلا {
        trie=(UNewTrie *)uprv_malloc(حجم(UNewTrie));
        إذا(trie==تش_اشاره) {
            أرجع تش_اشاره;
        }
    }
    uprv_memset(trie, 0, حجم(UNewTrie));
    trie->isAllocated = fill_دننه == تش_اشاره;

    إذا(alias_معلومات!=تش_اشاره) {
        trie->معلومات=alias_معلومات;
        trie->is_معلومات_Allocated=خطأ;
    } وإلا {
        trie->معلومات=(uint32_t *)uprv_malloc(max_معلومات_اوږدتیا*4);
        إذا(trie->معلومات==تش_اشاره) {
            uprv_free(trie);
            أرجع تش_اشاره;
        }
        trie->is_معلومات_Allocated=ریښتیا;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_معلومات_بلاک_اوږدتیا;

    إذا(latin1_خطي) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        افعل {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->شاخص[i++]=j;
            j+=UTRIE_معلومات_بلاک_اوږدتیا;
        } طالما(i<(256>>UTRIE_ښويدنه));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->معلومات_اوږدتیا=j;
    طالما(j>0) {
        trie->معلومات[--j]=initial_ارزښت;
    }

    trie->lead_Unit_ارزښت=lead_Unit_ارزښت;
    trie->شاخص_اوږدتیا=UTRIE_MAX_شاخص_اوږدتیا;
    trie->معلومات_ظرفیت=max_معلومات_اوږدتیا;
    trie->is_Latin1_خطي=latin1_خطي;
    trie->isCompacted=خطأ;
    أرجع trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fill_دننه, ثابت UNewTrie *other, uint32_t *alias_معلومات, int32_t alias_معلومات_ظرفیت) {
    UNewTrie *trie;
    UBool is_معلومات_Allocated;

    /* do not clone if other is not valid or already compacted */
    إذا(other==تش_اشاره || other->معلومات==تش_اشاره || other->isCompacted) {
        أرجع تش_اشاره;
    }

    /* clone data */
    إذا(alias_معلومات!=تش_اشاره && alias_معلومات_ظرفیت>=other->معلومات_ظرفیت) {
        is_معلومات_Allocated=خطأ;
    } وإلا {
        alias_معلومات_ظرفیت=other->معلومات_ظرفیت;
        alias_معلومات=(uint32_t *)uprv_malloc(other->معلومات_ظرفیت*4);
        إذا(alias_معلومات==تش_اشاره) {
            أرجع تش_اشاره;
        }
        is_معلومات_Allocated=ریښتیا;
    }

    trie=utrie_open(fill_دننه, alias_معلومات, alias_معلومات_ظرفیت,
                    other->معلومات[0], other->lead_Unit_ارزښت,
                    other->is_Latin1_خطي);
    إذا(trie==تش_اشاره) {
        uprv_free(alias_معلومات);
    } وإلا {
        uprv_memcpy(trie->شاخص, other->شاخص, حجم(trie->شاخص));
        uprv_memcpy(trie->معلومات, other->معلومات, (size_t)other->معلومات_اوږدتیا*4);
        trie->معلومات_اوږدتیا=other->معلومات_اوږدتیا;
        trie->is_معلومات_Allocated=is_معلومات_Allocated;
    }

    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
utrie_close(UNewTrie *trie) {
    إذا(trie!=تش_اشاره) {
        إذا(trie->is_معلومات_Allocated) {
            uprv_free(trie->معلومات);
            trie->معلومات=تش_اشاره;
        }
        إذا(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_معلومات(UNewTrie *trie, int32_t *p_اوږدتیا) {
    إذا(trie==تش_اشاره || p_اوږدتیا==تش_اشاره) {
        أرجع تش_اشاره;
    }

    *p_اوږدتیا=trie->معلومات_اوږدتیا;
    أرجع trie->معلومات;
}

ساكن int32_t
utrie_alloc_معلومات_بلاک(UNewTrie *trie) {
    int32_t new_بلاک, new_سر;

    new_بلاک=trie->معلومات_اوږدتیا;
    new_سر=new_بلاک+UTRIE_معلومات_بلاک_اوږدتیا;
    إذا(new_سر>trie->معلومات_ظرفیت) {
        /* out of memory in the data array */
        أرجع -1;
    }
    trie->معلومات_اوږدتیا=new_سر;
    أرجع new_بلاک;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ساكن int32_t
utrie_get_معلومات_بلاک(UNewTrie *trie, UChar32 c) {
    int32_t شاخص_ارزښت, new_بلاک;

    c>>=UTRIE_ښويدنه;
    شاخص_ارزښت=trie->شاخص[c];
    إذا(شاخص_ارزښت>0) {
        أرجع شاخص_ارزښت;
    }

    /* allocate a new data block */
    new_بلاک=utrie_alloc_معلومات_بلاک(trie);
    إذا(new_بلاک<0) {
        /* out of memory in the data array */
        أرجع -1;
    }
    trie->شاخص[c]=new_بلاک;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->معلومات+new_بلاک, trie->معلومات-شاخص_ارزښت, 4*UTRIE_معلومات_بلاک_اوږدتیا);
    أرجع new_بلاک;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t بلاک;

    /* valid, uncompacted trie and valid c? */
    إذا(trie==تش_اشاره || trie->isCompacted || (uint32_t)c>0x10ffff) {
        أرجع خطأ;
    }

    بلاک=utrie_get_معلومات_بلاک(trie, c);
    إذا(بلاک<0) {
        أرجع خطأ;
    }

    trie->معلومات[بلاک+(c&UTRIE_ماسک)]=value;
    أرجع ریښتیا;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *p_دننه_بلاک_Zero) {
    int32_t بلاک;

    /* valid, uncompacted trie and valid c? */
    إذا(trie==تش_اشاره || trie->isCompacted || (uint32_t)c>0x10ffff) {
        إذا(p_دننه_بلاک_Zero!=تش_اشاره) {
            *p_دننه_بلاک_Zero=ریښتیا;
        }
        أرجع 0;
    }

    بلاک=trie->شاخص[c>>UTRIE_ښويدنه];
    إذا(p_دننه_بلاک_Zero!=تش_اشاره) {
        *p_دننه_بلاک_Zero = بلاک == 0;
    }

    أرجع trie->معلومات[ABS(بلاک)+(c&UTRIE_ماسک)];
}

/**
 * @internal
 */
ساكن فراغ
utrie_fill_بلاک(uint32_t *بلاک, UChar32 پیل, UChar32 حد,
                uint32_t value, uint32_t initial_ارزښت, UBool overwrite) {
    uint32_t *p_حد;

    p_حد=بلاک+حد;
    بلاک+=پیل;
    إذا(overwrite) {
        طالما(بلاک<p_حد) {
            *بلاک++=value;
        }
    } وإلا {
        طالما(بلاک<p_حد) {
            إذا(*بلاک==initial_ارزښت) {
                *بلاک=value;
            }
            ++بلاک;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_سېټ_Range32(UNewTrie *trie, UChar32 پیل, UChar32 حد, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initial_ارزښت;
    int32_t بلاک, rest, repeat_بلاک;

    /* valid, uncompacted trie and valid indexes? */
    إذا( trie==تش_اشاره || trie->isCompacted ||
        (uint32_t)پیل>0x10ffff || (uint32_t)حد>0x110000 || پیل>حد
    ) {
        أرجع خطأ;
    }
    إذا(پیل==حد) {
        أرجع ریښتیا; /* nothing to do */
    }

    initial_ارزښت=trie->معلومات[0];
    إذا(پیل&UTRIE_ماسک) {
        UChar32 بل_پیل;

        /* set partial block at [start..following block boundary[ */
        بلاک=utrie_get_معلومات_بلاک(trie, پیل);
        إذا(بلاک<0) {
            أرجع خطأ;
        }

        بل_پیل=(پیل+UTRIE_معلومات_بلاک_اوږدتیا)&~UTRIE_ماسک;
        إذا(بل_پیل<=حد) {
            utrie_fill_بلاک(trie->معلومات+بلاک, پیل&UTRIE_ماسک, UTRIE_معلومات_بلاک_اوږدتیا,
                            value, initial_ارزښت, overwrite);
            پیل=بل_پیل;
        } وإلا {
            utrie_fill_بلاک(trie->معلومات+بلاک, پیل&UTRIE_ماسک, حد&UTRIE_ماسک,
                            value, initial_ارزښت, overwrite);
            أرجع ریښتیا;
        }
    }

    /* number of positions in the last, partial block */
    rest=حد&UTRIE_ماسک;

    /* round down limit to a block boundary */
    حد&=~UTRIE_ماسک;

    /* iterate over all-value blocks */
    إذا(value==initial_ارزښت) {
        repeat_بلاک=0;
    } وإلا {
        repeat_بلاک=-1;
    }
    طالما(پیل<حد) {
        /* get index value */
        بلاک=trie->شاخص[پیل>>UTRIE_ښويدنه];
        إذا(بلاک>0) {
            /* already allocated, fill in value */
            utrie_fill_بلاک(trie->معلومات+بلاک, 0, UTRIE_معلومات_بلاک_اوږدتیا, value, initial_ارزښت, overwrite);
        } وإلا إذا(trie->معلومات[-بلاک]!=value && (بلاک==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            إذا(repeat_بلاک>=0) {
                trie->شاخص[پیل>>UTRIE_ښويدنه]=-repeat_بلاک;
            } وإلا {
                /* create and set and fill the repeatBlock */
                repeat_بلاک=utrie_get_معلومات_بلاک(trie, پیل);
                إذا(repeat_بلاک<0) {
                    أرجع خطأ;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->شاخص[پیل>>UTRIE_ښويدنه]=-repeat_بلاک;
                utrie_fill_بلاک(trie->معلومات+repeat_بلاک, 0, UTRIE_معلومات_بلاک_اوږدتیا, value, initial_ارزښت, ریښتیا);
            }
        }

        پیل+=UTRIE_معلومات_بلاک_اوږدتیا;
    }

    إذا(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        بلاک=utrie_get_معلومات_بلاک(trie, پیل);
        إذا(بلاک<0) {
            أرجع خطأ;
        }

        utrie_fill_بلاک(trie->معلومات+بلاک, 0, rest, value, initial_ارزښت, overwrite);
    }

    أرجع ریښتیا;
}

ساكن int32_t
پیدا_کړه_Same_شاخص_بلاک(ثابت int32_t *idx, int32_t شاخص_اوږدتیا,
                    int32_t other_بلاک) {
    int32_t بلاک, i;

    لكل(بلاک=UTRIE_BMP_شاخص_اوږدتیا; بلاک<شاخص_اوږدتیا; بلاک+=UTRIE_SURROGATE_بلاک_شمېر) {
        لكل(i=0; i<UTRIE_SURROGATE_بلاک_شمېر; ++i) {
            إذا(idx[بلاک+i]!=idx[other_بلاک+i]) {
                اكسر;
            }
        }
        إذا(i==UTRIE_SURROGATE_بلاک_شمېر) {
            أرجع بلاک;
        }
    }
    أرجع شاخص_اوږدتیا;
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
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_ارزښت *get_Folded_ارزښت, UError_کوډ *p_Error_کوډ) {
    int32_t leadIndexes[UTRIE_SURROGATE_بلاک_شمېر];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t شاخص_اوږدتیا, بلاک;
#که_تعریف_شوی UTRIE_DEBUG
    صحيح شمېر_Lead_CUWith_معلومات=0;
#د_که_پای

    idx=trie->شاخص;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_ښويدنه), 4*UTRIE_SURROGATE_بلاک_شمېر);

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
    إذا(trie->lead_Unit_ارزښت==trie->معلومات[0]) {
        بلاک=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } وإلا {
        /* create and fill the repeatBlock */
        بلاک=utrie_alloc_معلومات_بلاک(trie);
        إذا(بلاک<0) {
            /* data table overflow */
            *p_Error_کوډ=U_حافظه_ویش_ERROR;
            أرجع;
        }
        utrie_fill_بلاک(trie->معلومات+بلاک, 0, UTRIE_معلومات_بلاک_اوږدتیا, trie->lead_Unit_ارزښت, trie->معلومات[0], ریښتیا);
        بلاک=-بلاک; /* negative block number to indicate that it is a repeat block */
    }
    لكل(c=(0xd800>>UTRIE_ښويدنه); c<(0xdc00>>UTRIE_ښويدنه); ++c) {
        trie->شاخص[c]=بلاک;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    شاخص_اوږدتیا=UTRIE_BMP_شاخص_اوږدتیا;

    /* search for any index (stage 1) entries for supplementary code points */
    لكل(c=0x10000; c<0x110000;) {
        إذا(idx[c>>UTRIE_ښويدنه]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#که_تعریف_شوی UTRIE_DEBUG
            ++شمېر_Lead_CUWith_معلومات;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#د_که_پای

            /* is there an identical index block? */
            بلاک=پیدا_کړه_Same_شاخص_بلاک(idx, شاخص_اوږدتیا, c>>UTRIE_ښويدنه);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_ارزښت(trie, c, بلاک+UTRIE_SURROGATE_بلاک_شمېر);
            إذا(value!=utrie_get32(trie, U16_LEAD(c), تش_اشاره)) {
                إذا(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *p_Error_کوډ=U_حافظه_ویش_ERROR;
                    أرجع;
                }

                /* if we did not find an identical index block... */
                إذا(بلاک==شاخص_اوږدتیا) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+شاخص_اوږدتیا,
                                 idx+(c>>UTRIE_ښويدنه),
                                 4*UTRIE_SURROGATE_بلاک_شمېر);
                    شاخص_اوږدتیا+=UTRIE_SURROGATE_بلاک_شمېر;
                }
            }
            c+=0x400;
        } وإلا {
            c+=UTRIE_معلومات_بلاک_اوږدتیا;
        }
    }
#که_تعریف_شوی UTRIE_DEBUG
    إذا(شمېر_Lead_CUWith_معلومات>0) {
        چاپ_کړه("supplementary data for %d lead surrogates\n", شمېر_Lead_CUWith_معلومات);
    }
#د_که_پای

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    إذا(شاخص_اوږدتیا>=UTRIE_MAX_شاخص_اوږدتیا) {
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_شاخص_اوږدتیا+UTRIE_SURROGATE_بلاک_شمېر,
                 idx+UTRIE_BMP_شاخص_اوږدتیا,
                 4*(شاخص_اوږدتیا-UTRIE_BMP_شاخص_اوږدتیا));
    uprv_memcpy(idx+UTRIE_BMP_شاخص_اوږدتیا,
                leadIndexes,
                4*UTRIE_SURROGATE_بلاک_شمېر);
    شاخص_اوږدتیا+=UTRIE_SURROGATE_بلاک_شمېر;

#که_تعریف_شوی UTRIE_DEBUG
    چاپ_کړه("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_شاخص_اوږدتیا, (اوږد)UTRIE_MAX_شاخص_اوږدتیا, شاخص_اوږدتیا);
#د_که_پای

    trie->شاخص_اوږدتیا=شاخص_اوږدتیا;
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
پیدا_کړه_ناکارول_شوی_بلاکونه(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_وخت_معلومات_اوږدتیا>>UTRIE_ښويدنه)*4);

    /* mark each block that _is_ used with 0 */
    لكل(i=0; i<trie->شاخص_اوږدتیا; ++i) {
        trie->map[ABS(trie->شاخص[i])>>UTRIE_ښويدنه]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

ساكن int32_t
پیدا_کړه_Same_معلومات_بلاک(ثابت uint32_t *معلومات, int32_t معلومات_اوږدتیا,
                   int32_t other_بلاک, int32_t ګام) {
    int32_t بلاک;

    /* ensure that we do not even partially get past dataLength */
    معلومات_اوږدتیا-=UTRIE_معلومات_بلاک_اوږدتیا;

    لكل(بلاک=0; بلاک<=معلومات_اوږدتیا; بلاک+=ګام) {
        إذا(equal_uint32(معلومات+بلاک, معلومات+other_بلاک, UTRIE_معلومات_بلاک_اوږدتیا)) {
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
utrie_compact(UNewTrie *trie, UBool overlap, UError_کوډ *p_Error_کوډ) {
    int32_t i, پیل, new_پیل, overlap_پیل;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }

    /* valid, uncompacted trie? */
    إذا(trie==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا(trie->isCompacted) {
        أرجع; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    پیدا_کړه_ناکارول_شوی_بلاکونه(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    إذا(trie->is_Latin1_خطي && UTRIE_ښويدنه<=8) {
        overlap_پیل=UTRIE_معلومات_بلاک_اوږدتیا+256;
    } وإلا {
        overlap_پیل=UTRIE_معلومات_بلاک_اوږدتیا;
    }

    new_پیل=UTRIE_معلومات_بلاک_اوږدتیا;
    لكل(پیل=new_پیل; پیل<trie->معلومات_اوږدتیا;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        إذا(trie->map[پیل>>UTRIE_ښويدنه]<0) {
            /* advance start to the next block */
            پیل+=UTRIE_معلومات_بلاک_اوږدتیا;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* search for an identical block */
        إذا( پیل>=overlap_پیل &&
            (i=پیدا_کړه_Same_معلومات_بلاک(trie->معلومات, new_پیل, پیل,
                            overlap ? UTRIE_معلومات_GRANULARITY : UTRIE_معلومات_بلاک_اوږدتیا))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[پیل>>UTRIE_ښويدنه]=i;

            /* advance start to the next block */
            پیل+=UTRIE_معلومات_بلاک_اوږدتیا;

            /* leave newStart with the previous block! */
            استمر;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        إذا(overlap && پیل>=overlap_پیل) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            لكل(i=UTRIE_معلومات_بلاک_اوږدتیا-UTRIE_معلومات_GRANULARITY;
                i>0 && !equal_uint32(trie->معلومات+(new_پیل-i), trie->معلومات+پیل, i);
                i-=UTRIE_معلومات_GRANULARITY) {}
        } وإلا {
            i=0;
        }

        إذا(i>0) {
            /* some overlap */
            trie->map[پیل>>UTRIE_ښويدنه]=new_پیل-i;

            /* move the non-overlapping indexes to their new positions */
            پیل+=i;
            لكل(i=UTRIE_معلومات_بلاک_اوږدتیا-i; i>0; --i) {
                trie->معلومات[new_پیل++]=trie->معلومات[پیل++];
            }
        } وإلا إذا(new_پیل<پیل) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[پیل>>UTRIE_ښويدنه]=new_پیل;
            لكل(i=UTRIE_معلومات_بلاک_اوږدتیا; i>0; --i) {
                trie->معلومات[new_پیل++]=trie->معلومات[پیل++];
            }
        } وإلا /* no overlap && newStart==start */ {
            trie->map[پیل>>UTRIE_ښويدنه]=پیل;
            new_پیل+=UTRIE_معلومات_بلاک_اوږدتیا;
            پیل=new_پیل;
        }
    }

    /* now adjust the index (stage 1) table */
    لكل(i=0; i<trie->شاخص_اوږدتیا; ++i) {
        trie->شاخص[i]=trie->map[ABS(trie->شاخص[i])>>UTRIE_ښويدنه];
    }

#که_تعریف_شوی UTRIE_DEBUG
    /* we saved some space */
    چاپ_کړه("compacting trie: count of 32-bit words %lu->%lu\n",
            (اوږد)trie->معلومات_اوږدتیا, (اوږد)new_پیل);
#د_که_پای

    trie->معلومات_اوږدتیا=new_پیل;
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
default_Get_Folded_ارزښت(UNewTrie *trie, UChar32 پیل, int32_t دننه_واټن) {
    uint32_t value, initial_ارزښت;
    UChar32 حد;
    UBool دننه_بلاک_Zero;

    initial_ارزښت=trie->معلومات[0];
    حد=پیل+0x400;
    طالما(پیل<حد) {
        value=utrie_get32(trie, پیل, &دننه_بلاک_Zero);
        إذا(دننه_بلاک_Zero) {
            پیل+=UTRIE_معلومات_بلاک_اوږدتیا;
        } وإلا إذا(value!=initial_ارزښت) {
            أرجع static_cast<uint32_t>(دننه_واټن);
        } وإلا {
            ++پیل;
        }
    }
    أرجع 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, فراغ *dt, int32_t capacity,
                UNew_Trie_Get_Folded_ارزښت *get_Folded_ارزښت,
                UBool reduce_To16_بټونه,
                UError_کوډ *p_Error_کوډ) {
    UTrie_سرلیک *سرلیک;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* معلومات = تش_اشاره;

    /* argument check */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    إذا(trie==تش_اشاره || capacity<0 || (capacity>0 && dt==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    إذا(get_Folded_ارزښت==تش_اشاره) {
        get_Folded_ارزښت=default_Get_Folded_ارزښت;
    }

    معلومات = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    إذا(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, خطأ, p_Error_کوډ);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_ارزښت, p_Error_کوډ);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, ریښتیا, p_Error_کوډ);

        trie->isCompacted=ریښتیا;
        إذا(U_FAILURE(*p_Error_کوډ)) {
            أرجع 0;
        }
    }

    /* is dataLength within limits? */
    إذا( (reduce_To16_بټونه ? (trie->معلومات_اوږدتیا+trie->شاخص_اوږدتیا) : trie->معلومات_اوږدتیا) >= UTRIE_MAX_معلومات_اوږدتیا) {
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
    }

    length=حجم(UTrie_سرلیک)+2*trie->شاخص_اوږدتیا;
    إذا(reduce_To16_بټونه) {
        length+=2*trie->معلومات_اوږدتیا;
    } وإلا {
        length+=4*trie->معلومات_اوږدتیا;
    }

    إذا(length>capacity) {
        أرجع length; /* preflighting */
    }

#که_تعریف_شوی UTRIE_DEBUG
    چاپ_کړه("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (اوږد)trie->شاخص_اوږدتیا, (اوږد)trie->معلومات_اوږدتیا, (اوږد)length);
#د_که_پای

    /* set the header fields */
    سرلیک=(UTrie_سرلیک *)معلومات;
    معلومات+=حجم(UTrie_سرلیک);

    سرلیک->signature=0x54726965; /* "Trie" */
    سرلیک->options=UTRIE_ښويدنه | (UTRIE_شاخص_ښويدنه<<UTRIE_OPTIONS_شاخص_ښويدنه);

    إذا(!reduce_To16_بټونه) {
        سرلیک->options|=UTRIE_OPTIONS_معلومات_IS_32_بټ;
    }
    إذا(trie->is_Latin1_خطي) {
        سرلیک->options|=UTRIE_OPTIONS_LATIN1_IS_خطي;
    }

    سرلیک->شاخص_اوږدتیا=trie->شاخص_اوږدتیا;
    سرلیک->معلومات_اوږدتیا=trie->معلومات_اوږدتیا;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    إذا(reduce_To16_بټونه) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->شاخص;
        dest16=(uint16_t *)معلومات;
        لكل(i=trie->شاخص_اوږدتیا; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->شاخص_اوږدتیا)>>UTRIE_شاخص_ښويدنه);
        }

        /* write 16-bit data values */
        p=trie->معلومات;
        لكل(i=trie->معلومات_اوږدتیا; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } وإلا {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->شاخص;
        dest16=(uint16_t *)معلومات;
        لكل(i=trie->شاخص_اوږدتیا; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_شاخص_ښويدنه);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->معلومات, 4*(size_t)trie->معلومات_اوږدتیا);
    }

    أرجع length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_default_Get_Folding_دننه_واټن(uint32_t معلومات) {
    أرجع (int32_t)معلومات;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ثابت فراغ *معلومات, int32_t length, UError_کوډ *p_Error_کوډ) {
    ثابت UTrie_سرلیک *سرلیک;
    ثابت uint16_t *p16;
    uint32_t options;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع -1;
    }

    /* enough data for a trie header? */
    إذا(length<(int32_t)حجم(UTrie_سرلیک)) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }

    /* check the signature */
    سرلیک=(ثابت UTrie_سرلیک *)معلومات;
    إذا(سرلیک->signature!=0x54726965) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }

    /* get the options and check the shift values */
    options=سرلیک->options;
    إذا( (options&UTRIE_OPTIONS_ښويدنه_ماسک)!=UTRIE_ښويدنه ||
        ((options>>UTRIE_OPTIONS_شاخص_ښويدنه)&UTRIE_OPTIONS_ښويدنه_ماسک)!=UTRIE_شاخص_ښويدنه
    ) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }
    trie->is_Latin1_خطي = (options & UTRIE_OPTIONS_LATIN1_IS_خطي) != 0;

    /* get the length values */
    trie->شاخص_اوږدتیا=سرلیک->شاخص_اوږدتیا;
    trie->معلومات_اوږدتیا=سرلیک->معلومات_اوږدتیا;

    length-=(int32_t)حجم(UTrie_سرلیک);

    /* enough data for the index? */
    إذا(length<2*trie->شاخص_اوږدتیا) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع -1;
    }
    p16=(ثابت uint16_t *)(سرلیک+1);
    trie->شاخص=p16;
    p16+=trie->شاخص_اوږدتیا;
    length-=2*trie->شاخص_اوږدتیا;

    /* get the data */
    إذا(options&UTRIE_OPTIONS_معلومات_IS_32_بټ) {
        إذا(length<4*trie->معلومات_اوږدتیا) {
            *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
            أرجع -1;
        }
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_ارزښت=trie->data32[0];
        length=(int32_t)حجم(UTrie_سرلیک)+2*trie->شاخص_اوږدتیا+4*trie->معلومات_اوږدتیا;
    } وإلا {
        إذا(length<2*trie->معلومات_اوږدتیا) {
            *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
            أرجع -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=تش_اشاره;
        trie->initial_ارزښت=trie->شاخص[trie->شاخص_اوږدتیا];
        length=(int32_t)حجم(UTrie_سرلیک)+2*trie->شاخص_اوږدتیا+2*trie->معلومات_اوږدتیا;
    }

    trie->get_Folding_دننه_واټن=utrie_default_Get_Folding_دننه_واټن;

    أرجع length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       فراغ *معلومات, int32_t length,
                       uint32_t initial_ارزښت, uint32_t lead_Unit_ارزښت,
                       UBool make16_بټ_Trie,
                       UError_کوډ *p_Error_کوډ) {
    uint16_t *p16;
    int32_t actual_اوږدتیا, latin1_اوږدتیا, i, حد;
    uint16_t بلاک;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_اوږدتیا= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->شاخص_اوږدتیا=UTRIE_BMP_شاخص_اوږدتیا+UTRIE_SURROGATE_بلاک_شمېر;
    trie->معلومات_اوږدتیا=latin1_اوږدتیا;
    إذا(lead_Unit_ارزښت!=initial_ارزښت) {
        trie->معلومات_اوږدتیا+=UTRIE_معلومات_بلاک_اوږدتیا;
    }

    actual_اوږدتیا=trie->شاخص_اوږدتیا*2;
    إذا(make16_بټ_Trie) {
        actual_اوږدتیا+=trie->معلومات_اوږدتیا*2;
    } وإلا {
        actual_اوږدتیا+=trie->معلومات_اوږدتیا*4;
    }

    /* enough space for the dummy trie? */
    إذا(length<actual_اوږدتیا) {
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        أرجع actual_اوږدتیا;
    }

    trie->is_Latin1_خطي=ریښتیا;
    trie->initial_ارزښت=initial_ارزښت;

    /* fill the index and data arrays */
    p16=(uint16_t *)معلومات;
    trie->شاخص=p16;

    إذا(make16_بټ_Trie) {
        /* indexes to block 0 */
        بلاک=(uint16_t)(trie->شاخص_اوږدتیا>>UTRIE_شاخص_ښويدنه);
        حد=trie->شاخص_اوږدتیا;
        لكل(i=0; i<حد; ++i) {
            p16[i]=بلاک;
        }

        إذا(lead_Unit_ارزښت!=initial_ارزښت) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلاک+=(uint16_t)(latin1_اوږدتیا>>UTRIE_شاخص_ښويدنه);
            i=0xd800>>UTRIE_ښويدنه;
            حد=0xdc00>>UTRIE_ښويدنه;
            لكل(; i<حد; ++i) {
                p16[i]=بلاک;
            }
        }

        trie->data32=تش_اشاره;

        /* Latin-1 data */
        p16+=trie->شاخص_اوږدتیا;
        لكل(i=0; i<latin1_اوږدتیا; ++i) {
            p16[i]=(uint16_t)initial_ارزښت;
        }

        /* data for lead surrogate code units */
        إذا(lead_Unit_ارزښت!=initial_ارزښت) {
            حد=latin1_اوږدتیا+UTRIE_معلومات_بلاک_اوږدتیا;
            لكل(/* i=latin1Length */; i<حد; ++i) {
                p16[i]=(uint16_t)lead_Unit_ارزښت;
            }
        }
    } وإلا {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->شاخص_اوږدتیا*2);

        إذا(lead_Unit_ارزښت!=initial_ارزښت) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            بلاک=(uint16_t)(latin1_اوږدتیا>>UTRIE_شاخص_ښويدنه);
            i=0xd800>>UTRIE_ښويدنه;
            حد=0xdc00>>UTRIE_ښويدنه;
            لكل(; i<حد; ++i) {
                p16[i]=بلاک;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->شاخص_اوږدتیا);

        /* Latin-1 data */
        لكل(i=0; i<latin1_اوږدتیا; ++i) {
            p32[i]=initial_ارزښت;
        }

        /* data for lead surrogate code units */
        إذا(lead_Unit_ارزښت!=initial_ارزښت) {
            حد=latin1_اوږدتیا+UTRIE_معلومات_بلاک_اوږدتیا;
            لكل(/* i=latin1Length */; i<حد; ++i) {
                p32[i]=lead_Unit_ارزښت;
            }
        }
    }

    trie->get_Folding_دننه_واټن=utrie_default_Get_Folding_دننه_واټن;

    أرجع actual_اوږدتیا;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
ساكن uint32_t U_CALLCONV
enum_Same_ارزښت(ثابت فراغ * /*context*/, uint32_t value) {
    أرجع value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI فراغ U_EXPORT2
utrie_enum(ثابت UTrie *trie,
           UTrie_Enum_ارزښت *enum_ارزښت, UTrieEnumRange *enumRange, ثابت فراغ *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prev_ارزښت, initial_ارزښت;
    UChar32 c, prev;
    int32_t l, i, j, بلاک, prev_بلاک, null_بلاک, دننه_واټن;

    /* check arguments */
    إذا(trie==تش_اشاره || trie->شاخص==تش_اشاره || enumRange==تش_اشاره) {
        أرجع;
    }
    إذا(enum_ارزښت==تش_اشاره) {
        enum_ارزښت=enum_Same_ارزښت;
    }

    idx=trie->شاخص;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_ارزښت=enum_ارزښت(context, trie->initial_ارزښت);

    إذا(data32==تش_اشاره) {
        null_بلاک=trie->شاخص_اوږدتیا;
    } وإلا {
        null_بلاک=0;
    }

    /* set variables for previous range */
    prev_بلاک=null_بلاک;
    prev=0;
    prev_ارزښت=initial_ارزښت;

    /* enumerate BMP - the main loop enumerates data blocks */
    لكل(i=0, c=0; c<=0xffff; ++i) {
        إذا(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_شاخص_اوږدتیا;
        } وإلا إذا(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_ښويدنه;
        }

        بلاک=idx[i]<<UTRIE_شاخص_ښويدنه;
        إذا(بلاک==prev_بلاک) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_معلومات_بلاک_اوږدتیا;
        } وإلا إذا(بلاک==null_بلاک) {
            /* this is the all-initial-value block */
            إذا(prev_ارزښت!=initial_ارزښت) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_ارزښت)) {
                        أرجع;
                    }
                }
                prev_بلاک=null_بلاک;
                prev=c;
                prev_ارزښت=initial_ارزښت;
            }
            c+=UTRIE_معلومات_بلاک_اوږدتیا;
        } وإلا {
            prev_بلاک=بلاک;
            لكل(j=0; j<UTRIE_معلومات_بلاک_اوږدتیا; ++j) {
                value=enum_ارزښت(context, data32!=تش_اشاره ? data32[بلاک+j] : idx[بلاک+j]);
                إذا(value!=prev_ارزښت) {
                    إذا(prev<c) {
                        إذا(!enumRange(context, prev, c, prev_ارزښت)) {
                            أرجع;
                        }
                    }
                    إذا(j>0) {
                        /* the block is not filled with all the same value */
                        prev_بلاک=-1;
                    }
                    prev=c;
                    prev_ارزښت=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    لكل(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        دننه_واټن=idx[l>>UTRIE_ښويدنه]<<UTRIE_شاخص_ښويدنه;
        إذا(دننه_واټن==null_بلاک) {
            /* no entries for a whole block of lead surrogates */
            إذا(prev_ارزښت!=initial_ارزښت) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_ارزښت)) {
                        أرجع;
                    }
                }
                prev_بلاک=null_بلاک;
                prev=c;
                prev_ارزښت=initial_ارزښت;
            }

            l+=UTRIE_معلومات_بلاک_اوږدتیا;
            c+=UTRIE_معلومات_بلاک_اوږدتیا<<10;
            استمر;
        }

        value= data32!=تش_اشاره ? data32[دننه_واټن+(l&UTRIE_ماسک)] : idx[دننه_واټن+(l&UTRIE_ماسک)];

        /* enumerate trail surrogates for this lead surrogate */
        دننه_واټن=trie->get_Folding_دننه_واټن(value);
        إذا(دننه_واټن<=0) {
            /* no data for this lead surrogate */
            إذا(prev_ارزښت!=initial_ارزښت) {
                إذا(prev<c) {
                    إذا(!enumRange(context, prev, c, prev_ارزښت)) {
                        أرجع;
                    }
                }
                prev_بلاک=null_بلاک;
                prev=c;
                prev_ارزښت=initial_ارزښت;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } وإلا {
            /* enumerate code points for this lead surrogate */
            i=دننه_واټن;
            دننه_واټن+=UTRIE_SURROGATE_بلاک_شمېر;
            افعل {
                /* copy of most of the body of the BMP loop */
                بلاک=idx[i]<<UTRIE_شاخص_ښويدنه;
                إذا(بلاک==prev_بلاک) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_معلومات_بلاک_اوږدتیا;
                } وإلا إذا(بلاک==null_بلاک) {
                    /* this is the all-initial-value block */
                    إذا(prev_ارزښت!=initial_ارزښت) {
                        إذا(prev<c) {
                            إذا(!enumRange(context, prev, c, prev_ارزښت)) {
                                أرجع;
                            }
                        }
                        prev_بلاک=null_بلاک;
                        prev=c;
                        prev_ارزښت=initial_ارزښت;
                    }
                    c+=UTRIE_معلومات_بلاک_اوږدتیا;
                } وإلا {
                    prev_بلاک=بلاک;
                    لكل(j=0; j<UTRIE_معلومات_بلاک_اوږدتیا; ++j) {
                        value=enum_ارزښت(context, data32!=تش_اشاره ? data32[بلاک+j] : idx[بلاک+j]);
                        إذا(value!=prev_ارزښت) {
                            إذا(prev<c) {
                                إذا(!enumRange(context, prev, c, prev_ارزښت)) {
                                    أرجع;
                                }
                            }
                            إذا(j>0) {
                                /* the block is not filled with all the same value */
                                prev_بلاک=-1;
                            }
                            prev=c;
                            prev_ارزښت=value;
                        }
                        ++c;
                    }
                }
            } طالما(++i<دننه_واټن);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_ارزښت);
}

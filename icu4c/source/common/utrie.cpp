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

#నిర్వచితమైతే UTRIE_DEBUG
#   include <stdio.h>
#అయితే_ముగింపు

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#నిర్వచనం_రద్దు ABS
#నిర్వచించు ABS(x) ((x)>=0 ? (x) : -(x))

స్థిర అంతఃవరుస UBool
equal_uint32(స్థిరరాశి uint32_t *s, స్థిరరాశి uint32_t *t, int32_t length) {
    అయితే(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    తిరిగి length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fill_లోపల,
           uint32_t *alias_దత్తాంశం, int32_t max_దత్తాంశం_నిడివి,
           uint32_t initial_విలువ, uint32_t lead_Unit_విలువ,
           UBool latin1_రేఖీయ) {
    UNewTrie *trie;
    int32_t i, j;

    ఒకవేళ( max_దత్తాంశం_నిడివి<UTRIE_దత్తాంశం_ఖండం_నిడివి ||
        (latin1_రేఖీయ && max_దత్తాంశం_నిడివి<1024)
    ) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(fill_లోపల!=శూన్యసూచిక) {
        trie=fill_లోపల;
    } లేకపోతే {
        trie=(UNewTrie *)uprv_malloc(పరిమాణం(UNewTrie));
        ఒకవేళ(trie==శూన్యసూచిక) {
            తిరిగి శూన్యసూచిక;
        }
    }
    uprv_memset(trie, 0, పరిమాణం(UNewTrie));
    trie->isAllocated = fill_లోపల == శూన్యసూచిక;

    ఒకవేళ(alias_దత్తాంశం!=శూన్యసూచిక) {
        trie->దత్తాంశం=alias_దత్తాంశం;
        trie->is_దత్తాంశం_Allocated=అబద్ధం;
    } లేకపోతే {
        trie->దత్తాంశం=(uint32_t *)uprv_malloc(max_దత్తాంశం_నిడివి*4);
        ఒకవేళ(trie->దత్తాంశం==శూన్యసూచిక) {
            uprv_free(trie);
            తిరిగి శూన్యసూచిక;
        }
        trie->is_దత్తాంశం_Allocated=నిజం;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_దత్తాంశం_ఖండం_నిడివి;

    ఒకవేళ(latin1_రేఖీయ) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        చేయి {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->సూచీ[i++]=j;
            j+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
        } అయితే(i<(256>>UTRIE_జరుపు));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->దత్తాంశం_నిడివి=j;
    అయితే(j>0) {
        trie->దత్తాంశం[--j]=initial_విలువ;
    }

    trie->lead_Unit_విలువ=lead_Unit_విలువ;
    trie->సూచీ_నిడివి=UTRIE_MAX_సూచీ_నిడివి;
    trie->దత్తాంశం_సామర్థ్యం=max_దత్తాంశం_నిడివి;
    trie->is_Latin1_రేఖీయ=latin1_రేఖీయ;
    trie->isCompacted=అబద్ధం;
    తిరిగి trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fill_లోపల, స్థిరరాశి UNewTrie *other, uint32_t *alias_దత్తాంశం, int32_t alias_దత్తాంశం_సామర్థ్యం) {
    UNewTrie *trie;
    UBool is_దత్తాంశం_Allocated;

    /* do not clone if other is not valid or already compacted */
    ఒకవేళ(other==శూన్యసూచిక || other->దత్తాంశం==శూన్యసూచిక || other->isCompacted) {
        తిరిగి శూన్యసూచిక;
    }

    /* clone data */
    ఒకవేళ(alias_దత్తాంశం!=శూన్యసూచిక && alias_దత్తాంశం_సామర్థ్యం>=other->దత్తాంశం_సామర్థ్యం) {
        is_దత్తాంశం_Allocated=అబద్ధం;
    } లేకపోతే {
        alias_దత్తాంశం_సామర్థ్యం=other->దత్తాంశం_సామర్థ్యం;
        alias_దత్తాంశం=(uint32_t *)uprv_malloc(other->దత్తాంశం_సామర్థ్యం*4);
        ఒకవేళ(alias_దత్తాంశం==శూన్యసూచిక) {
            తిరిగి శూన్యసూచిక;
        }
        is_దత్తాంశం_Allocated=నిజం;
    }

    trie=utrie_open(fill_లోపల, alias_దత్తాంశం, alias_దత్తాంశం_సామర్థ్యం,
                    other->దత్తాంశం[0], other->lead_Unit_విలువ,
                    other->is_Latin1_రేఖీయ);
    ఒకవేళ(trie==శూన్యసూచిక) {
        uprv_free(alias_దత్తాంశం);
    } లేకపోతే {
        uprv_memcpy(trie->సూచీ, other->సూచీ, పరిమాణం(trie->సూచీ));
        uprv_memcpy(trie->దత్తాంశం, other->దత్తాంశం, (size_t)other->దత్తాంశం_నిడివి*4);
        trie->దత్తాంశం_నిడివి=other->దత్తాంశం_నిడివి;
        trie->is_దత్తాంశం_Allocated=is_దత్తాంశం_Allocated;
    }

    తిరిగి trie;
}

U_CAPI శూన్య U_EXPORT2
utrie_close(UNewTrie *trie) {
    ఒకవేళ(trie!=శూన్యసూచిక) {
        ఒకవేళ(trie->is_దత్తాంశం_Allocated) {
            uprv_free(trie->దత్తాంశం);
            trie->దత్తాంశం=శూన్యసూచిక;
        }
        ఒకవేళ(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_దత్తాంశం(UNewTrie *trie, int32_t *p_నిడివి) {
    ఒకవేళ(trie==శూన్యసూచిక || p_నిడివి==శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }

    *p_నిడివి=trie->దత్తాంశం_నిడివి;
    తిరిగి trie->దత్తాంశం;
}

స్థిర int32_t
utrie_alloc_దత్తాంశం_ఖండం(UNewTrie *trie) {
    int32_t new_ఖండం, new_శిఖరం;

    new_ఖండం=trie->దత్తాంశం_నిడివి;
    new_శిఖరం=new_ఖండం+UTRIE_దత్తాంశం_ఖండం_నిడివి;
    ఒకవేళ(new_శిఖరం>trie->దత్తాంశం_సామర్థ్యం) {
        /* out of memory in the data array */
        తిరిగి -1;
    }
    trie->దత్తాంశం_నిడివి=new_శిఖరం;
    తిరిగి new_ఖండం;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
స్థిర int32_t
utrie_get_దత్తాంశం_ఖండం(UNewTrie *trie, UChar32 c) {
    int32_t సూచీ_విలువ, new_ఖండం;

    c>>=UTRIE_జరుపు;
    సూచీ_విలువ=trie->సూచీ[c];
    ఒకవేళ(సూచీ_విలువ>0) {
        తిరిగి సూచీ_విలువ;
    }

    /* allocate a new data block */
    new_ఖండం=utrie_alloc_దత్తాంశం_ఖండం(trie);
    ఒకవేళ(new_ఖండం<0) {
        /* out of memory in the data array */
        తిరిగి -1;
    }
    trie->సూచీ[c]=new_ఖండం;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->దత్తాంశం+new_ఖండం, trie->దత్తాంశం-సూచీ_విలువ, 4*UTRIE_దత్తాంశం_ఖండం_నిడివి);
    తిరిగి new_ఖండం;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t ఖండం;

    /* valid, uncompacted trie and valid c? */
    ఒకవేళ(trie==శూన్యసూచిక || trie->isCompacted || (uint32_t)c>0x10ffff) {
        తిరిగి అబద్ధం;
    }

    ఖండం=utrie_get_దత్తాంశం_ఖండం(trie, c);
    ఒకవేళ(ఖండం<0) {
        తిరిగి అబద్ధం;
    }

    trie->దత్తాంశం[ఖండం+(c&UTRIE_ముసుగు)]=value;
    తిరిగి నిజం;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *p_లోపల_ఖండం_Zero) {
    int32_t ఖండం;

    /* valid, uncompacted trie and valid c? */
    ఒకవేళ(trie==శూన్యసూచిక || trie->isCompacted || (uint32_t)c>0x10ffff) {
        ఒకవేళ(p_లోపల_ఖండం_Zero!=శూన్యసూచిక) {
            *p_లోపల_ఖండం_Zero=నిజం;
        }
        తిరిగి 0;
    }

    ఖండం=trie->సూచీ[c>>UTRIE_జరుపు];
    ఒకవేళ(p_లోపల_ఖండం_Zero!=శూన్యసూచిక) {
        *p_లోపల_ఖండం_Zero = ఖండం == 0;
    }

    తిరిగి trie->దత్తాంశం[ABS(ఖండం)+(c&UTRIE_ముసుగు)];
}

/**
 * @internal
 */
స్థిర శూన్య
utrie_fill_ఖండం(uint32_t *ఖండం, UChar32 ప్రారంభం, UChar32 హద్దు,
                uint32_t value, uint32_t initial_విలువ, UBool overwrite) {
    uint32_t *p_హద్దు;

    p_హద్దు=ఖండం+హద్దు;
    ఖండం+=ప్రారంభం;
    ఒకవేళ(overwrite) {
        అయితే(ఖండం<p_హద్దు) {
            *ఖండం++=value;
        }
    } లేకపోతే {
        అయితే(ఖండం<p_హద్దు) {
            ఒకవేళ(*ఖండం==initial_విలువ) {
                *ఖండం=value;
            }
            ++ఖండం;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_సమితి_Range32(UNewTrie *trie, UChar32 ప్రారంభం, UChar32 హద్దు, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initial_విలువ;
    int32_t ఖండం, rest, repeat_ఖండం;

    /* valid, uncompacted trie and valid indexes? */
    ఒకవేళ( trie==శూన్యసూచిక || trie->isCompacted ||
        (uint32_t)ప్రారంభం>0x10ffff || (uint32_t)హద్దు>0x110000 || ప్రారంభం>హద్దు
    ) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(ప్రారంభం==హద్దు) {
        తిరిగి నిజం; /* nothing to do */
    }

    initial_విలువ=trie->దత్తాంశం[0];
    ఒకవేళ(ప్రారంభం&UTRIE_ముసుగు) {
        UChar32 తదుపరి_ప్రారంభం;

        /* set partial block at [start..following block boundary[ */
        ఖండం=utrie_get_దత్తాంశం_ఖండం(trie, ప్రారంభం);
        ఒకవేళ(ఖండం<0) {
            తిరిగి అబద్ధం;
        }

        తదుపరి_ప్రారంభం=(ప్రారంభం+UTRIE_దత్తాంశం_ఖండం_నిడివి)&~UTRIE_ముసుగు;
        ఒకవేళ(తదుపరి_ప్రారంభం<=హద్దు) {
            utrie_fill_ఖండం(trie->దత్తాంశం+ఖండం, ప్రారంభం&UTRIE_ముసుగు, UTRIE_దత్తాంశం_ఖండం_నిడివి,
                            value, initial_విలువ, overwrite);
            ప్రారంభం=తదుపరి_ప్రారంభం;
        } లేకపోతే {
            utrie_fill_ఖండం(trie->దత్తాంశం+ఖండం, ప్రారంభం&UTRIE_ముసుగు, హద్దు&UTRIE_ముసుగు,
                            value, initial_విలువ, overwrite);
            తిరిగి నిజం;
        }
    }

    /* number of positions in the last, partial block */
    rest=హద్దు&UTRIE_ముసుగు;

    /* round down limit to a block boundary */
    హద్దు&=~UTRIE_ముసుగు;

    /* iterate over all-value blocks */
    ఒకవేళ(value==initial_విలువ) {
        repeat_ఖండం=0;
    } లేకపోతే {
        repeat_ఖండం=-1;
    }
    అయితే(ప్రారంభం<హద్దు) {
        /* get index value */
        ఖండం=trie->సూచీ[ప్రారంభం>>UTRIE_జరుపు];
        ఒకవేళ(ఖండం>0) {
            /* already allocated, fill in value */
            utrie_fill_ఖండం(trie->దత్తాంశం+ఖండం, 0, UTRIE_దత్తాంశం_ఖండం_నిడివి, value, initial_విలువ, overwrite);
        } లేకపోతే ఒకవేళ(trie->దత్తాంశం[-ఖండం]!=value && (ఖండం==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            ఒకవేళ(repeat_ఖండం>=0) {
                trie->సూచీ[ప్రారంభం>>UTRIE_జరుపు]=-repeat_ఖండం;
            } లేకపోతే {
                /* create and set and fill the repeatBlock */
                repeat_ఖండం=utrie_get_దత్తాంశం_ఖండం(trie, ప్రారంభం);
                ఒకవేళ(repeat_ఖండం<0) {
                    తిరిగి అబద్ధం;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->సూచీ[ప్రారంభం>>UTRIE_జరుపు]=-repeat_ఖండం;
                utrie_fill_ఖండం(trie->దత్తాంశం+repeat_ఖండం, 0, UTRIE_దత్తాంశం_ఖండం_నిడివి, value, initial_విలువ, నిజం);
            }
        }

        ప్రారంభం+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
    }

    ఒకవేళ(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        ఖండం=utrie_get_దత్తాంశం_ఖండం(trie, ప్రారంభం);
        ఒకవేళ(ఖండం<0) {
            తిరిగి అబద్ధం;
        }

        utrie_fill_ఖండం(trie->దత్తాంశం+ఖండం, 0, rest, value, initial_విలువ, overwrite);
    }

    తిరిగి నిజం;
}

స్థిర int32_t
కనుగొను_Same_సూచీ_ఖండం(స్థిరరాశి int32_t *idx, int32_t సూచీ_నిడివి,
                    int32_t other_ఖండం) {
    int32_t ఖండం, i;

    కొరకు(ఖండం=UTRIE_BMP_సూచీ_నిడివి; ఖండం<సూచీ_నిడివి; ఖండం+=UTRIE_SURROGATE_ఖండం_లెక్క) {
        కొరకు(i=0; i<UTRIE_SURROGATE_ఖండం_లెక్క; ++i) {
            ఒకవేళ(idx[ఖండం+i]!=idx[other_ఖండం+i]) {
                ఆపు;
            }
        }
        ఒకవేళ(i==UTRIE_SURROGATE_ఖండం_లెక్క) {
            తిరిగి ఖండం;
        }
    }
    తిరిగి సూచీ_నిడివి;
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
స్థిర శూన్య
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_విలువ *get_Folded_విలువ, UError_సంకేతం *p_Error_సంకేతం) {
    int32_t leadIndexes[UTRIE_SURROGATE_ఖండం_లెక్క];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t సూచీ_నిడివి, ఖండం;
#నిర్వచితమైతే UTRIE_DEBUG
    పూర్ణాంకం లెక్క_Lead_CUWith_దత్తాంశం=0;
#అయితే_ముగింపు

    idx=trie->సూచీ;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_జరుపు), 4*UTRIE_SURROGATE_ఖండం_లెక్క);

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
    ఒకవేళ(trie->lead_Unit_విలువ==trie->దత్తాంశం[0]) {
        ఖండం=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } లేకపోతే {
        /* create and fill the repeatBlock */
        ఖండం=utrie_alloc_దత్తాంశం_ఖండం(trie);
        ఒకవేళ(ఖండం<0) {
            /* data table overflow */
            *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        utrie_fill_ఖండం(trie->దత్తాంశం+ఖండం, 0, UTRIE_దత్తాంశం_ఖండం_నిడివి, trie->lead_Unit_విలువ, trie->దత్తాంశం[0], నిజం);
        ఖండం=-ఖండం; /* negative block number to indicate that it is a repeat block */
    }
    కొరకు(c=(0xd800>>UTRIE_జరుపు); c<(0xdc00>>UTRIE_జరుపు); ++c) {
        trie->సూచీ[c]=ఖండం;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    సూచీ_నిడివి=UTRIE_BMP_సూచీ_నిడివి;

    /* search for any index (stage 1) entries for supplementary code points */
    కొరకు(c=0x10000; c<0x110000;) {
        ఒకవేళ(idx[c>>UTRIE_జరుపు]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#నిర్వచితమైతే UTRIE_DEBUG
            ++లెక్క_Lead_CUWith_దత్తాంశం;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#అయితే_ముగింపు

            /* is there an identical index block? */
            ఖండం=కనుగొను_Same_సూచీ_ఖండం(idx, సూచీ_నిడివి, c>>UTRIE_జరుపు);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_విలువ(trie, c, ఖండం+UTRIE_SURROGATE_ఖండం_లెక్క);
            ఒకవేళ(value!=utrie_get32(trie, U16_LEAD(c), శూన్యసూచిక)) {
                ఒకవేళ(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    తిరిగి;
                }

                /* if we did not find an identical index block... */
                ఒకవేళ(ఖండం==సూచీ_నిడివి) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+సూచీ_నిడివి,
                                 idx+(c>>UTRIE_జరుపు),
                                 4*UTRIE_SURROGATE_ఖండం_లెక్క);
                    సూచీ_నిడివి+=UTRIE_SURROGATE_ఖండం_లెక్క;
                }
            }
            c+=0x400;
        } లేకపోతే {
            c+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
        }
    }
#నిర్వచితమైతే UTRIE_DEBUG
    ఒకవేళ(లెక్క_Lead_CUWith_దత్తాంశం>0) {
        రాయి("supplementary data for %d lead surrogates\n", లెక్క_Lead_CUWith_దత్తాంశం);
    }
#అయితే_ముగింపు

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    ఒకవేళ(సూచీ_నిడివి>=UTRIE_MAX_సూచీ_నిడివి) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_సూచీ_నిడివి+UTRIE_SURROGATE_ఖండం_లెక్క,
                 idx+UTRIE_BMP_సూచీ_నిడివి,
                 4*(సూచీ_నిడివి-UTRIE_BMP_సూచీ_నిడివి));
    uprv_memcpy(idx+UTRIE_BMP_సూచీ_నిడివి,
                leadIndexes,
                4*UTRIE_SURROGATE_ఖండం_లెక్క);
    సూచీ_నిడివి+=UTRIE_SURROGATE_ఖండం_లెక్క;

#నిర్వచితమైతే UTRIE_DEBUG
    రాయి("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_సూచీ_నిడివి, (దీర్ఘ)UTRIE_MAX_సూచీ_నిడివి, సూచీ_నిడివి);
#అయితే_ముగింపు

    trie->సూచీ_నిడివి=సూచీ_నిడివి;
}

/*
 * Set a value in the trie index map to indicate which data block
 * is referenced and which one is not.
 * utrie_compact() will remove data blocks that are not used at all.
 * Set
 * - 0 if it is used
 * - -1 if it is not used
 */
స్థిర శూన్య
కనుగొను_వాడని_ఖండాలు(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_సమయం_దత్తాంశం_నిడివి>>UTRIE_జరుపు)*4);

    /* mark each block that _is_ used with 0 */
    కొరకు(i=0; i<trie->సూచీ_నిడివి; ++i) {
        trie->map[ABS(trie->సూచీ[i])>>UTRIE_జరుపు]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

స్థిర int32_t
కనుగొను_Same_దత్తాంశం_ఖండం(స్థిరరాశి uint32_t *దత్తాంశం, int32_t దత్తాంశం_నిడివి,
                   int32_t other_ఖండం, int32_t అడుగు) {
    int32_t ఖండం;

    /* ensure that we do not even partially get past dataLength */
    దత్తాంశం_నిడివి-=UTRIE_దత్తాంశం_ఖండం_నిడివి;

    కొరకు(ఖండం=0; ఖండం<=దత్తాంశం_నిడివి; ఖండం+=అడుగు) {
        ఒకవేళ(equal_uint32(దత్తాంశం+ఖండం, దత్తాంశం+other_ఖండం, UTRIE_దత్తాంశం_ఖండం_నిడివి)) {
            తిరిగి ఖండం;
        }
    }
    తిరిగి -1;
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
స్థిర శూన్య
utrie_compact(UNewTrie *trie, UBool overlap, UError_సంకేతం *p_Error_సంకేతం) {
    int32_t i, ప్రారంభం, new_ప్రారంభం, overlap_ప్రారంభం;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }

    /* valid, uncompacted trie? */
    ఒకవేళ(trie==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ(trie->isCompacted) {
        తిరిగి; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    కనుగొను_వాడని_ఖండాలు(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    ఒకవేళ(trie->is_Latin1_రేఖీయ && UTRIE_జరుపు<=8) {
        overlap_ప్రారంభం=UTRIE_దత్తాంశం_ఖండం_నిడివి+256;
    } లేకపోతే {
        overlap_ప్రారంభం=UTRIE_దత్తాంశం_ఖండం_నిడివి;
    }

    new_ప్రారంభం=UTRIE_దత్తాంశం_ఖండం_నిడివి;
    కొరకు(ప్రారంభం=new_ప్రారంభం; ప్రారంభం<trie->దత్తాంశం_నిడివి;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        ఒకవేళ(trie->map[ప్రారంభం>>UTRIE_జరుపు]<0) {
            /* advance start to the next block */
            ప్రారంభం+=UTRIE_దత్తాంశం_ఖండం_నిడివి;

            /* leave newStart with the previous block! */
            కొనసాగు;
        }

        /* search for an identical block */
        ఒకవేళ( ప్రారంభం>=overlap_ప్రారంభం &&
            (i=కనుగొను_Same_దత్తాంశం_ఖండం(trie->దత్తాంశం, new_ప్రారంభం, ప్రారంభం,
                            overlap ? UTRIE_దత్తాంశం_GRANULARITY : UTRIE_దత్తాంశం_ఖండం_నిడివి))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[ప్రారంభం>>UTRIE_జరుపు]=i;

            /* advance start to the next block */
            ప్రారంభం+=UTRIE_దత్తాంశం_ఖండం_నిడివి;

            /* leave newStart with the previous block! */
            కొనసాగు;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        ఒకవేళ(overlap && ప్రారంభం>=overlap_ప్రారంభం) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            కొరకు(i=UTRIE_దత్తాంశం_ఖండం_నిడివి-UTRIE_దత్తాంశం_GRANULARITY;
                i>0 && !equal_uint32(trie->దత్తాంశం+(new_ప్రారంభం-i), trie->దత్తాంశం+ప్రారంభం, i);
                i-=UTRIE_దత్తాంశం_GRANULARITY) {}
        } లేకపోతే {
            i=0;
        }

        ఒకవేళ(i>0) {
            /* some overlap */
            trie->map[ప్రారంభం>>UTRIE_జరుపు]=new_ప్రారంభం-i;

            /* move the non-overlapping indexes to their new positions */
            ప్రారంభం+=i;
            కొరకు(i=UTRIE_దత్తాంశం_ఖండం_నిడివి-i; i>0; --i) {
                trie->దత్తాంశం[new_ప్రారంభం++]=trie->దత్తాంశం[ప్రారంభం++];
            }
        } లేకపోతే ఒకవేళ(new_ప్రారంభం<ప్రారంభం) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[ప్రారంభం>>UTRIE_జరుపు]=new_ప్రారంభం;
            కొరకు(i=UTRIE_దత్తాంశం_ఖండం_నిడివి; i>0; --i) {
                trie->దత్తాంశం[new_ప్రారంభం++]=trie->దత్తాంశం[ప్రారంభం++];
            }
        } లేకపోతే /* no overlap && newStart==start */ {
            trie->map[ప్రారంభం>>UTRIE_జరుపు]=ప్రారంభం;
            new_ప్రారంభం+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
            ప్రారంభం=new_ప్రారంభం;
        }
    }

    /* now adjust the index (stage 1) table */
    కొరకు(i=0; i<trie->సూచీ_నిడివి; ++i) {
        trie->సూచీ[i]=trie->map[ABS(trie->సూచీ[i])>>UTRIE_జరుపు];
    }

#నిర్వచితమైతే UTRIE_DEBUG
    /* we saved some space */
    రాయి("compacting trie: count of 32-bit words %lu->%lu\n",
            (దీర్ఘ)trie->దత్తాంశం_నిడివి, (దీర్ఘ)new_ప్రారంభం);
#అయితే_ముగింపు

    trie->దత్తాంశం_నిడివి=new_ప్రారంభం;
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
స్థిర uint32_t U_CALLCONV
default_Get_Folded_విలువ(UNewTrie *trie, UChar32 ప్రారంభం, int32_t స్థానభ్రంశం) {
    uint32_t value, initial_విలువ;
    UChar32 హద్దు;
    UBool లోపల_ఖండం_Zero;

    initial_విలువ=trie->దత్తాంశం[0];
    హద్దు=ప్రారంభం+0x400;
    అయితే(ప్రారంభం<హద్దు) {
        value=utrie_get32(trie, ప్రారంభం, &లోపల_ఖండం_Zero);
        ఒకవేళ(లోపల_ఖండం_Zero) {
            ప్రారంభం+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
        } లేకపోతే ఒకవేళ(value!=initial_విలువ) {
            తిరిగి static_cast<uint32_t>(స్థానభ్రంశం);
        } లేకపోతే {
            ++ప్రారంభం;
        }
    }
    తిరిగి 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, శూన్య *dt, int32_t capacity,
                UNew_Trie_Get_Folded_విలువ *get_Folded_విలువ,
                UBool reduce_To16_బిట్లు,
                UError_సంకేతం *p_Error_సంకేతం) {
    UTrie_శీర్షిక *శీర్షిక;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* దత్తాంశం = శూన్యసూచిక;

    /* argument check */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ(trie==శూన్యసూచిక || capacity<0 || (capacity>0 && dt==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    ఒకవేళ(get_Folded_విలువ==శూన్యసూచిక) {
        get_Folded_విలువ=default_Get_Folded_విలువ;
    }

    దత్తాంశం = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    ఒకవేళ(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, అబద్ధం, p_Error_సంకేతం);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_విలువ, p_Error_సంకేతం);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, నిజం, p_Error_సంకేతం);

        trie->isCompacted=నిజం;
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి 0;
        }
    }

    /* is dataLength within limits? */
    ఒకవేళ( (reduce_To16_బిట్లు ? (trie->దత్తాంశం_నిడివి+trie->సూచీ_నిడివి) : trie->దత్తాంశం_నిడివి) >= UTRIE_MAX_దత్తాంశం_నిడివి) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
    }

    length=పరిమాణం(UTrie_శీర్షిక)+2*trie->సూచీ_నిడివి;
    ఒకవేళ(reduce_To16_బిట్లు) {
        length+=2*trie->దత్తాంశం_నిడివి;
    } లేకపోతే {
        length+=4*trie->దత్తాంశం_నిడివి;
    }

    ఒకవేళ(length>capacity) {
        తిరిగి length; /* preflighting */
    }

#నిర్వచితమైతే UTRIE_DEBUG
    రాయి("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (దీర్ఘ)trie->సూచీ_నిడివి, (దీర్ఘ)trie->దత్తాంశం_నిడివి, (దీర్ఘ)length);
#అయితే_ముగింపు

    /* set the header fields */
    శీర్షిక=(UTrie_శీర్షిక *)దత్తాంశం;
    దత్తాంశం+=పరిమాణం(UTrie_శీర్షిక);

    శీర్షిక->signature=0x54726965; /* "Trie" */
    శీర్షిక->options=UTRIE_జరుపు | (UTRIE_సూచీ_జరుపు<<UTRIE_OPTIONS_సూచీ_జరుపు);

    ఒకవేళ(!reduce_To16_బిట్లు) {
        శీర్షిక->options|=UTRIE_OPTIONS_దత్తాంశం_IS_32_బిట్టు;
    }
    ఒకవేళ(trie->is_Latin1_రేఖీయ) {
        శీర్షిక->options|=UTRIE_OPTIONS_LATIN1_IS_రేఖీయ;
    }

    శీర్షిక->సూచీ_నిడివి=trie->సూచీ_నిడివి;
    శీర్షిక->దత్తాంశం_నిడివి=trie->దత్తాంశం_నిడివి;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    ఒకవేళ(reduce_To16_బిట్లు) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->సూచీ;
        dest16=(uint16_t *)దత్తాంశం;
        కొరకు(i=trie->సూచీ_నిడివి; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->సూచీ_నిడివి)>>UTRIE_సూచీ_జరుపు);
        }

        /* write 16-bit data values */
        p=trie->దత్తాంశం;
        కొరకు(i=trie->దత్తాంశం_నిడివి; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } లేకపోతే {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->సూచీ;
        dest16=(uint16_t *)దత్తాంశం;
        కొరకు(i=trie->సూచీ_నిడివి; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_సూచీ_జరుపు);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->దత్తాంశం, 4*(size_t)trie->దత్తాంశం_నిడివి);
    }

    తిరిగి length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_default_Get_Folding_స్థానభ్రంశం(uint32_t దత్తాంశం) {
    తిరిగి (int32_t)దత్తాంశం;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, స్థిరరాశి శూన్య *దత్తాంశం, int32_t length, UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UTrie_శీర్షిక *శీర్షిక;
    స్థిరరాశి uint16_t *p16;
    uint32_t options;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి -1;
    }

    /* enough data for a trie header? */
    ఒకవేళ(length<(int32_t)పరిమాణం(UTrie_శీర్షిక)) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి -1;
    }

    /* check the signature */
    శీర్షిక=(స్థిరరాశి UTrie_శీర్షిక *)దత్తాంశం;
    ఒకవేళ(శీర్షిక->signature!=0x54726965) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి -1;
    }

    /* get the options and check the shift values */
    options=శీర్షిక->options;
    ఒకవేళ( (options&UTRIE_OPTIONS_జరుపు_ముసుగు)!=UTRIE_జరుపు ||
        ((options>>UTRIE_OPTIONS_సూచీ_జరుపు)&UTRIE_OPTIONS_జరుపు_ముసుగు)!=UTRIE_సూచీ_జరుపు
    ) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి -1;
    }
    trie->is_Latin1_రేఖీయ = (options & UTRIE_OPTIONS_LATIN1_IS_రేఖీయ) != 0;

    /* get the length values */
    trie->సూచీ_నిడివి=శీర్షిక->సూచీ_నిడివి;
    trie->దత్తాంశం_నిడివి=శీర్షిక->దత్తాంశం_నిడివి;

    length-=(int32_t)పరిమాణం(UTrie_శీర్షిక);

    /* enough data for the index? */
    ఒకవేళ(length<2*trie->సూచీ_నిడివి) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి -1;
    }
    p16=(స్థిరరాశి uint16_t *)(శీర్షిక+1);
    trie->సూచీ=p16;
    p16+=trie->సూచీ_నిడివి;
    length-=2*trie->సూచీ_నిడివి;

    /* get the data */
    ఒకవేళ(options&UTRIE_OPTIONS_దత్తాంశం_IS_32_బిట్టు) {
        ఒకవేళ(length<4*trie->దత్తాంశం_నిడివి) {
            *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
            తిరిగి -1;
        }
        trie->data32=(స్థిరరాశి uint32_t *)p16;
        trie->initial_విలువ=trie->data32[0];
        length=(int32_t)పరిమాణం(UTrie_శీర్షిక)+2*trie->సూచీ_నిడివి+4*trie->దత్తాంశం_నిడివి;
    } లేకపోతే {
        ఒకవేళ(length<2*trie->దత్తాంశం_నిడివి) {
            *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
            తిరిగి -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=శూన్యసూచిక;
        trie->initial_విలువ=trie->సూచీ[trie->సూచీ_నిడివి];
        length=(int32_t)పరిమాణం(UTrie_శీర్షిక)+2*trie->సూచీ_నిడివి+2*trie->దత్తాంశం_నిడివి;
    }

    trie->get_Folding_స్థానభ్రంశం=utrie_default_Get_Folding_స్థానభ్రంశం;

    తిరిగి length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       శూన్య *దత్తాంశం, int32_t length,
                       uint32_t initial_విలువ, uint32_t lead_Unit_విలువ,
                       UBool make16_బిట్టు_Trie,
                       UError_సంకేతం *p_Error_సంకేతం) {
    uint16_t *p16;
    int32_t actual_నిడివి, latin1_నిడివి, i, హద్దు;
    uint16_t ఖండం;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_నిడివి= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->సూచీ_నిడివి=UTRIE_BMP_సూచీ_నిడివి+UTRIE_SURROGATE_ఖండం_లెక్క;
    trie->దత్తాంశం_నిడివి=latin1_నిడివి;
    ఒకవేళ(lead_Unit_విలువ!=initial_విలువ) {
        trie->దత్తాంశం_నిడివి+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
    }

    actual_నిడివి=trie->సూచీ_నిడివి*2;
    ఒకవేళ(make16_బిట్టు_Trie) {
        actual_నిడివి+=trie->దత్తాంశం_నిడివి*2;
    } లేకపోతే {
        actual_నిడివి+=trie->దత్తాంశం_నిడివి*4;
    }

    /* enough space for the dummy trie? */
    ఒకవేళ(length<actual_నిడివి) {
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి actual_నిడివి;
    }

    trie->is_Latin1_రేఖీయ=నిజం;
    trie->initial_విలువ=initial_విలువ;

    /* fill the index and data arrays */
    p16=(uint16_t *)దత్తాంశం;
    trie->సూచీ=p16;

    ఒకవేళ(make16_బిట్టు_Trie) {
        /* indexes to block 0 */
        ఖండం=(uint16_t)(trie->సూచీ_నిడివి>>UTRIE_సూచీ_జరుపు);
        హద్దు=trie->సూచీ_నిడివి;
        కొరకు(i=0; i<హద్దు; ++i) {
            p16[i]=ఖండం;
        }

        ఒకవేళ(lead_Unit_విలువ!=initial_విలువ) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            ఖండం+=(uint16_t)(latin1_నిడివి>>UTRIE_సూచీ_జరుపు);
            i=0xd800>>UTRIE_జరుపు;
            హద్దు=0xdc00>>UTRIE_జరుపు;
            కొరకు(; i<హద్దు; ++i) {
                p16[i]=ఖండం;
            }
        }

        trie->data32=శూన్యసూచిక;

        /* Latin-1 data */
        p16+=trie->సూచీ_నిడివి;
        కొరకు(i=0; i<latin1_నిడివి; ++i) {
            p16[i]=(uint16_t)initial_విలువ;
        }

        /* data for lead surrogate code units */
        ఒకవేళ(lead_Unit_విలువ!=initial_విలువ) {
            హద్దు=latin1_నిడివి+UTRIE_దత్తాంశం_ఖండం_నిడివి;
            కొరకు(/* i=latin1Length */; i<హద్దు; ++i) {
                p16[i]=(uint16_t)lead_Unit_విలువ;
            }
        }
    } లేకపోతే {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->సూచీ_నిడివి*2);

        ఒకవేళ(lead_Unit_విలువ!=initial_విలువ) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            ఖండం=(uint16_t)(latin1_నిడివి>>UTRIE_సూచీ_జరుపు);
            i=0xd800>>UTRIE_జరుపు;
            హద్దు=0xdc00>>UTRIE_జరుపు;
            కొరకు(; i<హద్దు; ++i) {
                p16[i]=ఖండం;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->సూచీ_నిడివి);

        /* Latin-1 data */
        కొరకు(i=0; i<latin1_నిడివి; ++i) {
            p32[i]=initial_విలువ;
        }

        /* data for lead surrogate code units */
        ఒకవేళ(lead_Unit_విలువ!=initial_విలువ) {
            హద్దు=latin1_నిడివి+UTRIE_దత్తాంశం_ఖండం_నిడివి;
            కొరకు(/* i=latin1Length */; i<హద్దు; ++i) {
                p32[i]=lead_Unit_విలువ;
            }
        }
    }

    trie->get_Folding_స్థానభ్రంశం=utrie_default_Get_Folding_స్థానభ్రంశం;

    తిరిగి actual_నిడివి;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
స్థిర uint32_t U_CALLCONV
enum_Same_విలువ(స్థిరరాశి శూన్య * /*context*/, uint32_t value) {
    తిరిగి value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI శూన్య U_EXPORT2
utrie_enum(స్థిరరాశి UTrie *trie,
           UTrie_Enum_విలువ *enum_విలువ, UTrieEnumRange *enumRange, స్థిరరాశి శూన్య *context) {
    స్థిరరాశి uint32_t *data32;
    స్థిరరాశి uint16_t *idx;

    uint32_t value, prev_విలువ, initial_విలువ;
    UChar32 c, prev;
    int32_t l, i, j, ఖండం, prev_ఖండం, null_ఖండం, స్థానభ్రంశం;

    /* check arguments */
    ఒకవేళ(trie==శూన్యసూచిక || trie->సూచీ==శూన్యసూచిక || enumRange==శూన్యసూచిక) {
        తిరిగి;
    }
    ఒకవేళ(enum_విలువ==శూన్యసూచిక) {
        enum_విలువ=enum_Same_విలువ;
    }

    idx=trie->సూచీ;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_విలువ=enum_విలువ(context, trie->initial_విలువ);

    ఒకవేళ(data32==శూన్యసూచిక) {
        null_ఖండం=trie->సూచీ_నిడివి;
    } లేకపోతే {
        null_ఖండం=0;
    }

    /* set variables for previous range */
    prev_ఖండం=null_ఖండం;
    prev=0;
    prev_విలువ=initial_విలువ;

    /* enumerate BMP - the main loop enumerates data blocks */
    కొరకు(i=0, c=0; c<=0xffff; ++i) {
        ఒకవేళ(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_సూచీ_నిడివి;
        } లేకపోతే ఒకవేళ(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_జరుపు;
        }

        ఖండం=idx[i]<<UTRIE_సూచీ_జరుపు;
        ఒకవేళ(ఖండం==prev_ఖండం) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
        } లేకపోతే ఒకవేళ(ఖండం==null_ఖండం) {
            /* this is the all-initial-value block */
            ఒకవేళ(prev_విలువ!=initial_విలువ) {
                ఒకవేళ(prev<c) {
                    ఒకవేళ(!enumRange(context, prev, c, prev_విలువ)) {
                        తిరిగి;
                    }
                }
                prev_ఖండం=null_ఖండం;
                prev=c;
                prev_విలువ=initial_విలువ;
            }
            c+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
        } లేకపోతే {
            prev_ఖండం=ఖండం;
            కొరకు(j=0; j<UTRIE_దత్తాంశం_ఖండం_నిడివి; ++j) {
                value=enum_విలువ(context, data32!=శూన్యసూచిక ? data32[ఖండం+j] : idx[ఖండం+j]);
                ఒకవేళ(value!=prev_విలువ) {
                    ఒకవేళ(prev<c) {
                        ఒకవేళ(!enumRange(context, prev, c, prev_విలువ)) {
                            తిరిగి;
                        }
                    }
                    ఒకవేళ(j>0) {
                        /* the block is not filled with all the same value */
                        prev_ఖండం=-1;
                    }
                    prev=c;
                    prev_విలువ=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    కొరకు(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        స్థానభ్రంశం=idx[l>>UTRIE_జరుపు]<<UTRIE_సూచీ_జరుపు;
        ఒకవేళ(స్థానభ్రంశం==null_ఖండం) {
            /* no entries for a whole block of lead surrogates */
            ఒకవేళ(prev_విలువ!=initial_విలువ) {
                ఒకవేళ(prev<c) {
                    ఒకవేళ(!enumRange(context, prev, c, prev_విలువ)) {
                        తిరిగి;
                    }
                }
                prev_ఖండం=null_ఖండం;
                prev=c;
                prev_విలువ=initial_విలువ;
            }

            l+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
            c+=UTRIE_దత్తాంశం_ఖండం_నిడివి<<10;
            కొనసాగు;
        }

        value= data32!=శూన్యసూచిక ? data32[స్థానభ్రంశం+(l&UTRIE_ముసుగు)] : idx[స్థానభ్రంశం+(l&UTRIE_ముసుగు)];

        /* enumerate trail surrogates for this lead surrogate */
        స్థానభ్రంశం=trie->get_Folding_స్థానభ్రంశం(value);
        ఒకవేళ(స్థానభ్రంశం<=0) {
            /* no data for this lead surrogate */
            ఒకవేళ(prev_విలువ!=initial_విలువ) {
                ఒకవేళ(prev<c) {
                    ఒకవేళ(!enumRange(context, prev, c, prev_విలువ)) {
                        తిరిగి;
                    }
                }
                prev_ఖండం=null_ఖండం;
                prev=c;
                prev_విలువ=initial_విలువ;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } లేకపోతే {
            /* enumerate code points for this lead surrogate */
            i=స్థానభ్రంశం;
            స్థానభ్రంశం+=UTRIE_SURROGATE_ఖండం_లెక్క;
            చేయి {
                /* copy of most of the body of the BMP loop */
                ఖండం=idx[i]<<UTRIE_సూచీ_జరుపు;
                ఒకవేళ(ఖండం==prev_ఖండం) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
                } లేకపోతే ఒకవేళ(ఖండం==null_ఖండం) {
                    /* this is the all-initial-value block */
                    ఒకవేళ(prev_విలువ!=initial_విలువ) {
                        ఒకవేళ(prev<c) {
                            ఒకవేళ(!enumRange(context, prev, c, prev_విలువ)) {
                                తిరిగి;
                            }
                        }
                        prev_ఖండం=null_ఖండం;
                        prev=c;
                        prev_విలువ=initial_విలువ;
                    }
                    c+=UTRIE_దత్తాంశం_ఖండం_నిడివి;
                } లేకపోతే {
                    prev_ఖండం=ఖండం;
                    కొరకు(j=0; j<UTRIE_దత్తాంశం_ఖండం_నిడివి; ++j) {
                        value=enum_విలువ(context, data32!=శూన్యసూచిక ? data32[ఖండం+j] : idx[ఖండం+j]);
                        ఒకవేళ(value!=prev_విలువ) {
                            ఒకవేళ(prev<c) {
                                ఒకవేళ(!enumRange(context, prev, c, prev_విలువ)) {
                                    తిరిగి;
                                }
                            }
                            ఒకవేళ(j>0) {
                                /* the block is not filled with all the same value */
                                prev_ఖండం=-1;
                            }
                            prev=c;
                            prev_విలువ=value;
                        }
                        ++c;
                    }
                }
            } అయితే(++i<స్థానభ్రంశం);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_విలువ);
}

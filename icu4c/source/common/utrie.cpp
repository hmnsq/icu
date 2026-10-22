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

#यदि_परिभाषितम् UTRIE_DEBUG
#   include <stdio.h>
#यदि_अन्तः

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#अपरिभाषय ABS
#परिभाषय ABS(x) ((x)>=0 ? (x) : -(x))

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
utrie_open(UNewTrie *fill_अन्तर्,
           uint32_t *alias_दत्त, int32_t max_दत्त_दीर्घता,
           uint32_t initial_मानम्, uint32_t lead_Unit_मानम्,
           UBool latin1_रैखिकम्) {
    UNewTrie *trie;
    int32_t i, j;

    यदि( max_दत्त_दीर्घता<UTRIE_दत्त_खण्डम्_दीर्घता ||
        (latin1_रैखिकम् && max_दत्त_दीर्घता<1024)
    ) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि(fill_अन्तर्!=शून्यसूचकः) {
        trie=fill_अन्तर्;
    } अन्यथा {
        trie=(UNewTrie *)uprv_malloc(परिमाण(UNewTrie));
        यदि(trie==शून्यसूचकः) {
            प्रत्यागम शून्यसूचकः;
        }
    }
    uprv_memset(trie, 0, परिमाण(UNewTrie));
    trie->isAllocated = fill_अन्तर् == शून्यसूचकः;

    यदि(alias_दत्त!=शून्यसूचकः) {
        trie->दत्त=alias_दत्त;
        trie->is_दत्त_Allocated=असत्य;
    } अन्यथा {
        trie->दत्त=(uint32_t *)uprv_malloc(max_दत्त_दीर्घता*4);
        यदि(trie->दत्त==शून्यसूचकः) {
            uprv_free(trie);
            प्रत्यागम शून्यसूचकः;
        }
        trie->is_दत्त_Allocated=सत्य;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_दत्त_खण्डम्_दीर्घता;

    यदि(latin1_रैखिकम्) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        कुरु {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->सूचकांक[i++]=j;
            j+=UTRIE_दत्त_खण्डम्_दीर्घता;
        } यावत्(i<(256>>UTRIE_सरणम्));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->दत्त_दीर्घता=j;
    यावत्(j>0) {
        trie->दत्त[--j]=initial_मानम्;
    }

    trie->lead_Unit_मानम्=lead_Unit_मानम्;
    trie->सूचकांक_दीर्घता=UTRIE_MAX_सूचकांक_दीर्घता;
    trie->दत्त_क्षमता=max_दत्त_दीर्घता;
    trie->is_Latin1_रैखिकम्=latin1_रैखिकम्;
    trie->isCompacted=असत्य;
    प्रत्यागम trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fill_अन्तर्, ध्रुव UNewTrie *other, uint32_t *alias_दत्त, int32_t alias_दत्त_क्षमता) {
    UNewTrie *trie;
    UBool is_दत्त_Allocated;

    /* do not clone if other is not valid or already compacted */
    यदि(other==शून्यसूचकः || other->दत्त==शून्यसूचकः || other->isCompacted) {
        प्रत्यागम शून्यसूचकः;
    }

    /* clone data */
    यदि(alias_दत्त!=शून्यसूचकः && alias_दत्त_क्षमता>=other->दत्त_क्षमता) {
        is_दत्त_Allocated=असत्य;
    } अन्यथा {
        alias_दत्त_क्षमता=other->दत्त_क्षमता;
        alias_दत्त=(uint32_t *)uprv_malloc(other->दत्त_क्षमता*4);
        यदि(alias_दत्त==शून्यसूचकः) {
            प्रत्यागम शून्यसूचकः;
        }
        is_दत्त_Allocated=सत्य;
    }

    trie=utrie_open(fill_अन्तर्, alias_दत्त, alias_दत्त_क्षमता,
                    other->दत्त[0], other->lead_Unit_मानम्,
                    other->is_Latin1_रैखिकम्);
    यदि(trie==शून्यसूचकः) {
        uprv_free(alias_दत्त);
    } अन्यथा {
        uprv_memcpy(trie->सूचकांक, other->सूचकांक, परिमाण(trie->सूचकांक));
        uprv_memcpy(trie->दत्त, other->दत्त, (size_t)other->दत्त_दीर्घता*4);
        trie->दत्त_दीर्घता=other->दत्त_दीर्घता;
        trie->is_दत्त_Allocated=is_दत्त_Allocated;
    }

    प्रत्यागम trie;
}

U_CAPI शून्य U_EXPORT2
utrie_close(UNewTrie *trie) {
    यदि(trie!=शून्यसूचकः) {
        यदि(trie->is_दत्त_Allocated) {
            uprv_free(trie->दत्त);
            trie->दत्त=शून्यसूचकः;
        }
        यदि(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_दत्त(UNewTrie *trie, int32_t *p_दीर्घता) {
    यदि(trie==शून्यसूचकः || p_दीर्घता==शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }

    *p_दीर्घता=trie->दत्त_दीर्घता;
    प्रत्यागम trie->दत्त;
}

स्थिर int32_t
utrie_alloc_दत्त_खण्डम्(UNewTrie *trie) {
    int32_t new_खण्डम्, new_शिखरम्;

    new_खण्डम्=trie->दत्त_दीर्घता;
    new_शिखरम्=new_खण्डम्+UTRIE_दत्त_खण्डम्_दीर्घता;
    यदि(new_शिखरम्>trie->दत्त_क्षमता) {
        /* out of memory in the data array */
        प्रत्यागम -1;
    }
    trie->दत्त_दीर्घता=new_शिखरम्;
    प्रत्यागम new_खण्डम्;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
स्थिर int32_t
utrie_get_दत्त_खण्डम्(UNewTrie *trie, UChar32 c) {
    int32_t सूचकांक_मानम्, new_खण्डम्;

    c>>=UTRIE_सरणम्;
    सूचकांक_मानम्=trie->सूचकांक[c];
    यदि(सूचकांक_मानम्>0) {
        प्रत्यागम सूचकांक_मानम्;
    }

    /* allocate a new data block */
    new_खण्डम्=utrie_alloc_दत्त_खण्डम्(trie);
    यदि(new_खण्डम्<0) {
        /* out of memory in the data array */
        प्रत्यागम -1;
    }
    trie->सूचकांक[c]=new_खण्डम्;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->दत्त+new_खण्डम्, trie->दत्त-सूचकांक_मानम्, 4*UTRIE_दत्त_खण्डम्_दीर्घता);
    प्रत्यागम new_खण्डम्;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t खण्डम्;

    /* valid, uncompacted trie and valid c? */
    यदि(trie==शून्यसूचकः || trie->isCompacted || (uint32_t)c>0x10ffff) {
        प्रत्यागम असत्य;
    }

    खण्डम्=utrie_get_दत्त_खण्डम्(trie, c);
    यदि(खण्डम्<0) {
        प्रत्यागम असत्य;
    }

    trie->दत्त[खण्डम्+(c&UTRIE_आवरणम्)]=value;
    प्रत्यागम सत्य;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *p_अन्तर्_खण्डम्_Zero) {
    int32_t खण्डम्;

    /* valid, uncompacted trie and valid c? */
    यदि(trie==शून्यसूचकः || trie->isCompacted || (uint32_t)c>0x10ffff) {
        यदि(p_अन्तर्_खण्डम्_Zero!=शून्यसूचकः) {
            *p_अन्तर्_खण्डम्_Zero=सत्य;
        }
        प्रत्यागम 0;
    }

    खण्डम्=trie->सूचकांक[c>>UTRIE_सरणम्];
    यदि(p_अन्तर्_खण्डम्_Zero!=शून्यसूचकः) {
        *p_अन्तर्_खण्डम्_Zero = खण्डम् == 0;
    }

    प्रत्यागम trie->दत्त[ABS(खण्डम्)+(c&UTRIE_आवरणम्)];
}

/**
 * @internal
 */
स्थिर शून्य
utrie_fill_खण्डम्(uint32_t *खण्डम्, UChar32 start, UChar32 सीमा,
                uint32_t value, uint32_t initial_मानम्, UBool overwrite) {
    uint32_t *p_सीमा;

    p_सीमा=खण्डम्+सीमा;
    खण्डम्+=start;
    यदि(overwrite) {
        यावत्(खण्डम्<p_सीमा) {
            *खण्डम्++=value;
        }
    } अन्यथा {
        यावत्(खण्डम्<p_सीमा) {
            यदि(*खण्डम्==initial_मानम्) {
                *खण्डम्=value;
            }
            ++खण्डम्;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_समुच्चयः_Range32(UNewTrie *trie, UChar32 start, UChar32 सीमा, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initial_मानम्;
    int32_t खण्डम्, rest, repeat_खण्डम्;

    /* valid, uncompacted trie and valid indexes? */
    यदि( trie==शून्यसूचकः || trie->isCompacted ||
        (uint32_t)start>0x10ffff || (uint32_t)सीमा>0x110000 || start>सीमा
    ) {
        प्रत्यागम असत्य;
    }
    यदि(start==सीमा) {
        प्रत्यागम सत्य; /* nothing to do */
    }

    initial_मानम्=trie->दत्त[0];
    यदि(start&UTRIE_आवरणम्) {
        UChar32 अग्र_Start;

        /* set partial block at [start..following block boundary[ */
        खण्डम्=utrie_get_दत्त_खण्डम्(trie, start);
        यदि(खण्डम्<0) {
            प्रत्यागम असत्य;
        }

        अग्र_Start=(start+UTRIE_दत्त_खण्डम्_दीर्घता)&~UTRIE_आवरणम्;
        यदि(अग्र_Start<=सीमा) {
            utrie_fill_खण्डम्(trie->दत्त+खण्डम्, start&UTRIE_आवरणम्, UTRIE_दत्त_खण्डम्_दीर्घता,
                            value, initial_मानम्, overwrite);
            start=अग्र_Start;
        } अन्यथा {
            utrie_fill_खण्डम्(trie->दत्त+खण्डम्, start&UTRIE_आवरणम्, सीमा&UTRIE_आवरणम्,
                            value, initial_मानम्, overwrite);
            प्रत्यागम सत्य;
        }
    }

    /* number of positions in the last, partial block */
    rest=सीमा&UTRIE_आवरणम्;

    /* round down limit to a block boundary */
    सीमा&=~UTRIE_आवरणम्;

    /* iterate over all-value blocks */
    यदि(value==initial_मानम्) {
        repeat_खण्डम्=0;
    } अन्यथा {
        repeat_खण्डम्=-1;
    }
    यावत्(start<सीमा) {
        /* get index value */
        खण्डम्=trie->सूचकांक[start>>UTRIE_सरणम्];
        यदि(खण्डम्>0) {
            /* already allocated, fill in value */
            utrie_fill_खण्डम्(trie->दत्त+खण्डम्, 0, UTRIE_दत्त_खण्डम्_दीर्घता, value, initial_मानम्, overwrite);
        } अन्यथा यदि(trie->दत्त[-खण्डम्]!=value && (खण्डम्==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            यदि(repeat_खण्डम्>=0) {
                trie->सूचकांक[start>>UTRIE_सरणम्]=-repeat_खण्डम्;
            } अन्यथा {
                /* create and set and fill the repeatBlock */
                repeat_खण्डम्=utrie_get_दत्त_खण्डम्(trie, start);
                यदि(repeat_खण्डम्<0) {
                    प्रत्यागम असत्य;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->सूचकांक[start>>UTRIE_सरणम्]=-repeat_खण्डम्;
                utrie_fill_खण्डम्(trie->दत्त+repeat_खण्डम्, 0, UTRIE_दत्त_खण्डम्_दीर्घता, value, initial_मानम्, सत्य);
            }
        }

        start+=UTRIE_दत्त_खण्डम्_दीर्घता;
    }

    यदि(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        खण्डम्=utrie_get_दत्त_खण्डम्(trie, start);
        यदि(खण्डम्<0) {
            प्रत्यागम असत्य;
        }

        utrie_fill_खण्डम्(trie->दत्त+खण्डम्, 0, rest, value, initial_मानम्, overwrite);
    }

    प्रत्यागम सत्य;
}

स्थिर int32_t
find_Same_सूचकांक_खण्डम्(ध्रुव int32_t *idx, int32_t सूचकांक_दीर्घता,
                    int32_t other_खण्डम्) {
    int32_t खण्डम्, i;

    प्रति(खण्डम्=UTRIE_BMP_सूचकांक_दीर्घता; खण्डम्<सूचकांक_दीर्घता; खण्डम्+=UTRIE_SURROGATE_खण्डम्_गणनम्) {
        प्रति(i=0; i<UTRIE_SURROGATE_खण्डम्_गणनम्; ++i) {
            यदि(idx[खण्डम्+i]!=idx[other_खण्डम्+i]) {
                भङ्ग;
            }
        }
        यदि(i==UTRIE_SURROGATE_खण्डम्_गणनम्) {
            प्रत्यागम खण्डम्;
        }
    }
    प्रत्यागम सूचकांक_दीर्घता;
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
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_मानम् *get_Folded_मानम्, UError_संहिता *p_Error_संहिता) {
    int32_t leadIndexes[UTRIE_SURROGATE_खण्डम्_गणनम्];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t सूचकांक_दीर्घता, खण्डम्;
#यदि_परिभाषितम् UTRIE_DEBUG
    पूर्णाङ्क गणनम्_Lead_CUWith_दत्त=0;
#यदि_अन्तः

    idx=trie->सूचकांक;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_सरणम्), 4*UTRIE_SURROGATE_खण्डम्_गणनम्);

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
    यदि(trie->lead_Unit_मानम्==trie->दत्त[0]) {
        खण्डम्=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } अन्यथा {
        /* create and fill the repeatBlock */
        खण्डम्=utrie_alloc_दत्त_खण्डम्(trie);
        यदि(खण्डम्<0) {
            /* data table overflow */
            *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        utrie_fill_खण्डम्(trie->दत्त+खण्डम्, 0, UTRIE_दत्त_खण्डम्_दीर्घता, trie->lead_Unit_मानम्, trie->दत्त[0], सत्य);
        खण्डम्=-खण्डम्; /* negative block number to indicate that it is a repeat block */
    }
    प्रति(c=(0xd800>>UTRIE_सरणम्); c<(0xdc00>>UTRIE_सरणम्); ++c) {
        trie->सूचकांक[c]=खण्डम्;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    सूचकांक_दीर्घता=UTRIE_BMP_सूचकांक_दीर्घता;

    /* search for any index (stage 1) entries for supplementary code points */
    प्रति(c=0x10000; c<0x110000;) {
        यदि(idx[c>>UTRIE_सरणम्]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#यदि_परिभाषितम् UTRIE_DEBUG
            ++गणनम्_Lead_CUWith_दत्त;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#यदि_अन्तः

            /* is there an identical index block? */
            खण्डम्=find_Same_सूचकांक_खण्डम्(idx, सूचकांक_दीर्घता, c>>UTRIE_सरणम्);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_मानम्(trie, c, खण्डम्+UTRIE_SURROGATE_खण्डम्_गणनम्);
            यदि(value!=utrie_get32(trie, U16_LEAD(c), शून्यसूचकः)) {
                यदि(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                    प्रत्यागम;
                }

                /* if we did not find an identical index block... */
                यदि(खण्डम्==सूचकांक_दीर्घता) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+सूचकांक_दीर्घता,
                                 idx+(c>>UTRIE_सरणम्),
                                 4*UTRIE_SURROGATE_खण्डम्_गणनम्);
                    सूचकांक_दीर्घता+=UTRIE_SURROGATE_खण्डम्_गणनम्;
                }
            }
            c+=0x400;
        } अन्यथा {
            c+=UTRIE_दत्त_खण्डम्_दीर्घता;
        }
    }
#यदि_परिभाषितम् UTRIE_DEBUG
    यदि(गणनम्_Lead_CUWith_दत्त>0) {
        प्रारूप_लिख("supplementary data for %d lead surrogates\n", गणनम्_Lead_CUWith_दत्त);
    }
#यदि_अन्तः

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    यदि(सूचकांक_दीर्घता>=UTRIE_MAX_सूचकांक_दीर्घता) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_सूचकांक_दीर्घता+UTRIE_SURROGATE_खण्डम्_गणनम्,
                 idx+UTRIE_BMP_सूचकांक_दीर्घता,
                 4*(सूचकांक_दीर्घता-UTRIE_BMP_सूचकांक_दीर्घता));
    uprv_memcpy(idx+UTRIE_BMP_सूचकांक_दीर्घता,
                leadIndexes,
                4*UTRIE_SURROGATE_खण्डम्_गणनम्);
    सूचकांक_दीर्घता+=UTRIE_SURROGATE_खण्डम्_गणनम्;

#यदि_परिभाषितम् UTRIE_DEBUG
    प्रारूप_लिख("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_सूचकांक_दीर्घता, (दीर्घ)UTRIE_MAX_सूचकांक_दीर्घता, सूचकांक_दीर्घता);
#यदि_अन्तः

    trie->सूचकांक_दीर्घता=सूचकांक_दीर्घता;
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
find_अप्रयुक्तम्_खण्डानि(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_TIME_दत्त_दीर्घता>>UTRIE_सरणम्)*4);

    /* mark each block that _is_ used with 0 */
    प्रति(i=0; i<trie->सूचकांक_दीर्घता; ++i) {
        trie->map[ABS(trie->सूचकांक[i])>>UTRIE_सरणम्]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

स्थिर int32_t
find_Same_दत्त_खण्डम्(ध्रुव uint32_t *दत्त, int32_t दत्त_दीर्घता,
                   int32_t other_खण्डम्, int32_t चरणः) {
    int32_t खण्डम्;

    /* ensure that we do not even partially get past dataLength */
    दत्त_दीर्घता-=UTRIE_दत्त_खण्डम्_दीर्घता;

    प्रति(खण्डम्=0; खण्डम्<=दत्त_दीर्घता; खण्डम्+=चरणः) {
        यदि(equal_uint32(दत्त+खण्डम्, दत्त+other_खण्डम्, UTRIE_दत्त_खण्डम्_दीर्घता)) {
            प्रत्यागम खण्डम्;
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
utrie_compact(UNewTrie *trie, UBool overlap, UError_संहिता *p_Error_संहिता) {
    int32_t i, start, newStart, overlapStart;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }

    /* valid, uncompacted trie? */
    यदि(trie==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि(trie->isCompacted) {
        प्रत्यागम; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    find_अप्रयुक्तम्_खण्डानि(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    यदि(trie->is_Latin1_रैखिकम् && UTRIE_सरणम्<=8) {
        overlapStart=UTRIE_दत्त_खण्डम्_दीर्घता+256;
    } अन्यथा {
        overlapStart=UTRIE_दत्त_खण्डम्_दीर्घता;
    }

    newStart=UTRIE_दत्त_खण्डम्_दीर्घता;
    प्रति(start=newStart; start<trie->दत्त_दीर्घता;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        यदि(trie->map[start>>UTRIE_सरणम्]<0) {
            /* advance start to the next block */
            start+=UTRIE_दत्त_खण्डम्_दीर्घता;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* search for an identical block */
        यदि( start>=overlapStart &&
            (i=find_Same_दत्त_खण्डम्(trie->दत्त, newStart, start,
                            overlap ? UTRIE_दत्त_GRANULARITY : UTRIE_दत्त_खण्डम्_दीर्घता))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE_सरणम्]=i;

            /* advance start to the next block */
            start+=UTRIE_दत्त_खण्डम्_दीर्घता;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        यदि(overlap && start>=overlapStart) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            प्रति(i=UTRIE_दत्त_खण्डम्_दीर्घता-UTRIE_दत्त_GRANULARITY;
                i>0 && !equal_uint32(trie->दत्त+(newStart-i), trie->दत्त+start, i);
                i-=UTRIE_दत्त_GRANULARITY) {}
        } अन्यथा {
            i=0;
        }

        यदि(i>0) {
            /* some overlap */
            trie->map[start>>UTRIE_सरणम्]=newStart-i;

            /* move the non-overlapping indexes to their new positions */
            start+=i;
            प्रति(i=UTRIE_दत्त_खण्डम्_दीर्घता-i; i>0; --i) {
                trie->दत्त[newStart++]=trie->दत्त[start++];
            }
        } अन्यथा यदि(newStart<start) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[start>>UTRIE_सरणम्]=newStart;
            प्रति(i=UTRIE_दत्त_खण्डम्_दीर्घता; i>0; --i) {
                trie->दत्त[newStart++]=trie->दत्त[start++];
            }
        } अन्यथा /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE_सरणम्]=start;
            newStart+=UTRIE_दत्त_खण्डम्_दीर्घता;
            start=newStart;
        }
    }

    /* now adjust the index (stage 1) table */
    प्रति(i=0; i<trie->सूचकांक_दीर्घता; ++i) {
        trie->सूचकांक[i]=trie->map[ABS(trie->सूचकांक[i])>>UTRIE_सरणम्];
    }

#यदि_परिभाषितम् UTRIE_DEBUG
    /* we saved some space */
    प्रारूप_लिख("compacting trie: count of 32-bit words %lu->%lu\n",
            (दीर्घ)trie->दत्त_दीर्घता, (दीर्घ)newStart);
#यदि_अन्तः

    trie->दत्त_दीर्घता=newStart;
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
default_Get_Folded_मानम्(UNewTrie *trie, UChar32 start, int32_t विस्थापनम्) {
    uint32_t value, initial_मानम्;
    UChar32 सीमा;
    UBool अन्तर्_खण्डम्_Zero;

    initial_मानम्=trie->दत्त[0];
    सीमा=start+0x400;
    यावत्(start<सीमा) {
        value=utrie_get32(trie, start, &अन्तर्_खण्डम्_Zero);
        यदि(अन्तर्_खण्डम्_Zero) {
            start+=UTRIE_दत्त_खण्डम्_दीर्घता;
        } अन्यथा यदि(value!=initial_मानम्) {
            प्रत्यागम static_cast<uint32_t>(विस्थापनम्);
        } अन्यथा {
            ++start;
        }
    }
    प्रत्यागम 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, शून्य *dt, int32_t capacity,
                UNew_Trie_Get_Folded_मानम् *get_Folded_मानम्,
                UBool reduce_To16_द्वयङ्काः,
                UError_संहिता *p_Error_संहिता) {
    UTrie_शीर्षकम् *शीर्षकम्;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* दत्त = शून्यसूचकः;

    /* argument check */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि(trie==शून्यसूचकः || capacity<0 || (capacity>0 && dt==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    यदि(get_Folded_मानम्==शून्यसूचकः) {
        get_Folded_मानम्=default_Get_Folded_मानम्;
    }

    दत्त = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    यदि(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, असत्य, p_Error_संहिता);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_मानम्, p_Error_संहिता);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, सत्य, p_Error_संहिता);

        trie->isCompacted=सत्य;
        यदि(U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम 0;
        }
    }

    /* is dataLength within limits? */
    यदि( (reduce_To16_द्वयङ्काः ? (trie->दत्त_दीर्घता+trie->सूचकांक_दीर्घता) : trie->दत्त_दीर्घता) >= UTRIE_MAX_दत्त_दीर्घता) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
    }

    length=परिमाण(UTrie_शीर्षकम्)+2*trie->सूचकांक_दीर्घता;
    यदि(reduce_To16_द्वयङ्काः) {
        length+=2*trie->दत्त_दीर्घता;
    } अन्यथा {
        length+=4*trie->दत्त_दीर्घता;
    }

    यदि(length>capacity) {
        प्रत्यागम length; /* preflighting */
    }

#यदि_परिभाषितम् UTRIE_DEBUG
    प्रारूप_लिख("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (दीर्घ)trie->सूचकांक_दीर्घता, (दीर्घ)trie->दत्त_दीर्घता, (दीर्घ)length);
#यदि_अन्तः

    /* set the header fields */
    शीर्षकम्=(UTrie_शीर्षकम् *)दत्त;
    दत्त+=परिमाण(UTrie_शीर्षकम्);

    शीर्षकम्->signature=0x54726965; /* "Trie" */
    शीर्षकम्->options=UTRIE_सरणम् | (UTRIE_सूचकांक_सरणम्<<UTRIE_OPTIONS_सूचकांक_सरणम्);

    यदि(!reduce_To16_द्वयङ्काः) {
        शीर्षकम्->options|=UTRIE_OPTIONS_दत्त_IS_32_BIT;
    }
    यदि(trie->is_Latin1_रैखिकम्) {
        शीर्षकम्->options|=UTRIE_OPTIONS_LATIN1_IS_रैखिकम्;
    }

    शीर्षकम्->सूचकांक_दीर्घता=trie->सूचकांक_दीर्घता;
    शीर्षकम्->दत्त_दीर्घता=trie->दत्त_दीर्घता;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    यदि(reduce_To16_द्वयङ्काः) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->सूचकांक;
        dest16=(uint16_t *)दत्त;
        प्रति(i=trie->सूचकांक_दीर्घता; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->सूचकांक_दीर्घता)>>UTRIE_सूचकांक_सरणम्);
        }

        /* write 16-bit data values */
        p=trie->दत्त;
        प्रति(i=trie->दत्त_दीर्घता; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } अन्यथा {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->सूचकांक;
        dest16=(uint16_t *)दत्त;
        प्रति(i=trie->सूचकांक_दीर्घता; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_सूचकांक_सरणम्);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->दत्त, 4*(size_t)trie->दत्त_दीर्घता);
    }

    प्रत्यागम length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_default_Get_Folding_विस्थापनम्(uint32_t दत्त) {
    प्रत्यागम (int32_t)दत्त;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ध्रुव शून्य *दत्त, int32_t length, UError_संहिता *p_Error_संहिता) {
    ध्रुव UTrie_शीर्षकम् *शीर्षकम्;
    ध्रुव uint16_t *p16;
    uint32_t options;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम -1;
    }

    /* enough data for a trie header? */
    यदि(length<(int32_t)परिमाण(UTrie_शीर्षकम्)) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम -1;
    }

    /* check the signature */
    शीर्षकम्=(ध्रुव UTrie_शीर्षकम् *)दत्त;
    यदि(शीर्षकम्->signature!=0x54726965) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम -1;
    }

    /* get the options and check the shift values */
    options=शीर्षकम्->options;
    यदि( (options&UTRIE_OPTIONS_सरणम्_आवरणम्)!=UTRIE_सरणम् ||
        ((options>>UTRIE_OPTIONS_सूचकांक_सरणम्)&UTRIE_OPTIONS_सरणम्_आवरणम्)!=UTRIE_सूचकांक_सरणम्
    ) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम -1;
    }
    trie->is_Latin1_रैखिकम् = (options & UTRIE_OPTIONS_LATIN1_IS_रैखिकम्) != 0;

    /* get the length values */
    trie->सूचकांक_दीर्घता=शीर्षकम्->सूचकांक_दीर्घता;
    trie->दत्त_दीर्घता=शीर्षकम्->दत्त_दीर्घता;

    length-=(int32_t)परिमाण(UTrie_शीर्षकम्);

    /* enough data for the index? */
    यदि(length<2*trie->सूचकांक_दीर्घता) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम -1;
    }
    p16=(ध्रुव uint16_t *)(शीर्षकम्+1);
    trie->सूचकांक=p16;
    p16+=trie->सूचकांक_दीर्घता;
    length-=2*trie->सूचकांक_दीर्घता;

    /* get the data */
    यदि(options&UTRIE_OPTIONS_दत्त_IS_32_BIT) {
        यदि(length<4*trie->दत्त_दीर्घता) {
            *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
            प्रत्यागम -1;
        }
        trie->data32=(ध्रुव uint32_t *)p16;
        trie->initial_मानम्=trie->data32[0];
        length=(int32_t)परिमाण(UTrie_शीर्षकम्)+2*trie->सूचकांक_दीर्घता+4*trie->दत्त_दीर्घता;
    } अन्यथा {
        यदि(length<2*trie->दत्त_दीर्घता) {
            *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
            प्रत्यागम -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=शून्यसूचकः;
        trie->initial_मानम्=trie->सूचकांक[trie->सूचकांक_दीर्घता];
        length=(int32_t)परिमाण(UTrie_शीर्षकम्)+2*trie->सूचकांक_दीर्घता+2*trie->दत्त_दीर्घता;
    }

    trie->get_Folding_विस्थापनम्=utrie_default_Get_Folding_विस्थापनम्;

    प्रत्यागम length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       शून्य *दत्त, int32_t length,
                       uint32_t initial_मानम्, uint32_t lead_Unit_मानम्,
                       UBool make16BitTrie,
                       UError_संहिता *p_Error_संहिता) {
    uint16_t *p16;
    int32_t actual_दीर्घता, latin1_दीर्घता, i, सीमा;
    uint16_t खण्डम्;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_दीर्घता= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->सूचकांक_दीर्घता=UTRIE_BMP_सूचकांक_दीर्घता+UTRIE_SURROGATE_खण्डम्_गणनम्;
    trie->दत्त_दीर्घता=latin1_दीर्घता;
    यदि(lead_Unit_मानम्!=initial_मानम्) {
        trie->दत्त_दीर्घता+=UTRIE_दत्त_खण्डम्_दीर्घता;
    }

    actual_दीर्घता=trie->सूचकांक_दीर्घता*2;
    यदि(make16BitTrie) {
        actual_दीर्घता+=trie->दत्त_दीर्घता*2;
    } अन्यथा {
        actual_दीर्घता+=trie->दत्त_दीर्घता*4;
    }

    /* enough space for the dummy trie? */
    यदि(length<actual_दीर्घता) {
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम actual_दीर्घता;
    }

    trie->is_Latin1_रैखिकम्=सत्य;
    trie->initial_मानम्=initial_मानम्;

    /* fill the index and data arrays */
    p16=(uint16_t *)दत्त;
    trie->सूचकांक=p16;

    यदि(make16BitTrie) {
        /* indexes to block 0 */
        खण्डम्=(uint16_t)(trie->सूचकांक_दीर्घता>>UTRIE_सूचकांक_सरणम्);
        सीमा=trie->सूचकांक_दीर्घता;
        प्रति(i=0; i<सीमा; ++i) {
            p16[i]=खण्डम्;
        }

        यदि(lead_Unit_मानम्!=initial_मानम्) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            खण्डम्+=(uint16_t)(latin1_दीर्घता>>UTRIE_सूचकांक_सरणम्);
            i=0xd800>>UTRIE_सरणम्;
            सीमा=0xdc00>>UTRIE_सरणम्;
            प्रति(; i<सीमा; ++i) {
                p16[i]=खण्डम्;
            }
        }

        trie->data32=शून्यसूचकः;

        /* Latin-1 data */
        p16+=trie->सूचकांक_दीर्घता;
        प्रति(i=0; i<latin1_दीर्घता; ++i) {
            p16[i]=(uint16_t)initial_मानम्;
        }

        /* data for lead surrogate code units */
        यदि(lead_Unit_मानम्!=initial_मानम्) {
            सीमा=latin1_दीर्घता+UTRIE_दत्त_खण्डम्_दीर्घता;
            प्रति(/* i=latin1Length */; i<सीमा; ++i) {
                p16[i]=(uint16_t)lead_Unit_मानम्;
            }
        }
    } अन्यथा {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->सूचकांक_दीर्घता*2);

        यदि(lead_Unit_मानम्!=initial_मानम्) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            खण्डम्=(uint16_t)(latin1_दीर्घता>>UTRIE_सूचकांक_सरणम्);
            i=0xd800>>UTRIE_सरणम्;
            सीमा=0xdc00>>UTRIE_सरणम्;
            प्रति(; i<सीमा; ++i) {
                p16[i]=खण्डम्;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->सूचकांक_दीर्घता);

        /* Latin-1 data */
        प्रति(i=0; i<latin1_दीर्घता; ++i) {
            p32[i]=initial_मानम्;
        }

        /* data for lead surrogate code units */
        यदि(lead_Unit_मानम्!=initial_मानम्) {
            सीमा=latin1_दीर्घता+UTRIE_दत्त_खण्डम्_दीर्घता;
            प्रति(/* i=latin1Length */; i<सीमा; ++i) {
                p32[i]=lead_Unit_मानम्;
            }
        }
    }

    trie->get_Folding_विस्थापनम्=utrie_default_Get_Folding_विस्थापनम्;

    प्रत्यागम actual_दीर्घता;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
स्थिर uint32_t U_CALLCONV
enum_Same_मानम्(ध्रुव शून्य * /*context*/, uint32_t value) {
    प्रत्यागम value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI शून्य U_EXPORT2
utrie_enum(ध्रुव UTrie *trie,
           UTrie_Enum_मानम् *enum_मानम्, UTrieEnumRange *enumRange, ध्रुव शून्य *context) {
    ध्रुव uint32_t *data32;
    ध्रुव uint16_t *idx;

    uint32_t value, पूर्व_मानम्, initial_मानम्;
    UChar32 c, पूर्व;
    int32_t l, i, j, खण्डम्, पूर्व_खण्डम्, null_खण्डम्, विस्थापनम्;

    /* check arguments */
    यदि(trie==शून्यसूचकः || trie->सूचकांक==शून्यसूचकः || enumRange==शून्यसूचकः) {
        प्रत्यागम;
    }
    यदि(enum_मानम्==शून्यसूचकः) {
        enum_मानम्=enum_Same_मानम्;
    }

    idx=trie->सूचकांक;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_मानम्=enum_मानम्(context, trie->initial_मानम्);

    यदि(data32==शून्यसूचकः) {
        null_खण्डम्=trie->सूचकांक_दीर्घता;
    } अन्यथा {
        null_खण्डम्=0;
    }

    /* set variables for previous range */
    पूर्व_खण्डम्=null_खण्डम्;
    पूर्व=0;
    पूर्व_मानम्=initial_मानम्;

    /* enumerate BMP - the main loop enumerates data blocks */
    प्रति(i=0, c=0; c<=0xffff; ++i) {
        यदि(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_सूचकांक_दीर्घता;
        } अन्यथा यदि(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_सरणम्;
        }

        खण्डम्=idx[i]<<UTRIE_सूचकांक_सरणम्;
        यदि(खण्डम्==पूर्व_खण्डम्) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_दत्त_खण्डम्_दीर्घता;
        } अन्यथा यदि(खण्डम्==null_खण्डम्) {
            /* this is the all-initial-value block */
            यदि(पूर्व_मानम्!=initial_मानम्) {
                यदि(पूर्व<c) {
                    यदि(!enumRange(context, पूर्व, c, पूर्व_मानम्)) {
                        प्रत्यागम;
                    }
                }
                पूर्व_खण्डम्=null_खण्डम्;
                पूर्व=c;
                पूर्व_मानम्=initial_मानम्;
            }
            c+=UTRIE_दत्त_खण्डम्_दीर्घता;
        } अन्यथा {
            पूर्व_खण्डम्=खण्डम्;
            प्रति(j=0; j<UTRIE_दत्त_खण्डम्_दीर्घता; ++j) {
                value=enum_मानम्(context, data32!=शून्यसूचकः ? data32[खण्डम्+j] : idx[खण्डम्+j]);
                यदि(value!=पूर्व_मानम्) {
                    यदि(पूर्व<c) {
                        यदि(!enumRange(context, पूर्व, c, पूर्व_मानम्)) {
                            प्रत्यागम;
                        }
                    }
                    यदि(j>0) {
                        /* the block is not filled with all the same value */
                        पूर्व_खण्डम्=-1;
                    }
                    पूर्व=c;
                    पूर्व_मानम्=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    प्रति(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        विस्थापनम्=idx[l>>UTRIE_सरणम्]<<UTRIE_सूचकांक_सरणम्;
        यदि(विस्थापनम्==null_खण्डम्) {
            /* no entries for a whole block of lead surrogates */
            यदि(पूर्व_मानम्!=initial_मानम्) {
                यदि(पूर्व<c) {
                    यदि(!enumRange(context, पूर्व, c, पूर्व_मानम्)) {
                        प्रत्यागम;
                    }
                }
                पूर्व_खण्डम्=null_खण्डम्;
                पूर्व=c;
                पूर्व_मानम्=initial_मानम्;
            }

            l+=UTRIE_दत्त_खण्डम्_दीर्घता;
            c+=UTRIE_दत्त_खण्डम्_दीर्घता<<10;
            अनुवर्त;
        }

        value= data32!=शून्यसूचकः ? data32[विस्थापनम्+(l&UTRIE_आवरणम्)] : idx[विस्थापनम्+(l&UTRIE_आवरणम्)];

        /* enumerate trail surrogates for this lead surrogate */
        विस्थापनम्=trie->get_Folding_विस्थापनम्(value);
        यदि(विस्थापनम्<=0) {
            /* no data for this lead surrogate */
            यदि(पूर्व_मानम्!=initial_मानम्) {
                यदि(पूर्व<c) {
                    यदि(!enumRange(context, पूर्व, c, पूर्व_मानम्)) {
                        प्रत्यागम;
                    }
                }
                पूर्व_खण्डम्=null_खण्डम्;
                पूर्व=c;
                पूर्व_मानम्=initial_मानम्;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } अन्यथा {
            /* enumerate code points for this lead surrogate */
            i=विस्थापनम्;
            विस्थापनम्+=UTRIE_SURROGATE_खण्डम्_गणनम्;
            कुरु {
                /* copy of most of the body of the BMP loop */
                खण्डम्=idx[i]<<UTRIE_सूचकांक_सरणम्;
                यदि(खण्डम्==पूर्व_खण्डम्) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_दत्त_खण्डम्_दीर्घता;
                } अन्यथा यदि(खण्डम्==null_खण्डम्) {
                    /* this is the all-initial-value block */
                    यदि(पूर्व_मानम्!=initial_मानम्) {
                        यदि(पूर्व<c) {
                            यदि(!enumRange(context, पूर्व, c, पूर्व_मानम्)) {
                                प्रत्यागम;
                            }
                        }
                        पूर्व_खण्डम्=null_खण्डम्;
                        पूर्व=c;
                        पूर्व_मानम्=initial_मानम्;
                    }
                    c+=UTRIE_दत्त_खण्डम्_दीर्घता;
                } अन्यथा {
                    पूर्व_खण्डम्=खण्डम्;
                    प्रति(j=0; j<UTRIE_दत्त_खण्डम्_दीर्घता; ++j) {
                        value=enum_मानम्(context, data32!=शून्यसूचकः ? data32[खण्डम्+j] : idx[खण्डम्+j]);
                        यदि(value!=पूर्व_मानम्) {
                            यदि(पूर्व<c) {
                                यदि(!enumRange(context, पूर्व, c, पूर्व_मानम्)) {
                                    प्रत्यागम;
                                }
                            }
                            यदि(j>0) {
                                /* the block is not filled with all the same value */
                                पूर्व_खण्डम्=-1;
                            }
                            पूर्व=c;
                            पूर्व_मानम्=value;
                        }
                        ++c;
                    }
                }
            } यावत्(++i<विस्थापनम्);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, पूर्व, c, पूर्व_मानम्);
}

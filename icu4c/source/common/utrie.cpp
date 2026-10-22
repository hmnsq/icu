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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE_DEBUG
#   include <stdio.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ABS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ABS(x) ((x)>=0 ? (x) : -(x))

ਸਥਿਰ ਸਤਰਅੰਦਰ UBool
equal_uint32(ਅਚਲ uint32_t *s, ਅਚਲ uint32_t *t, int32_t length) {
    ਜਦੋਂ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ਵਾਪਸ length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fill_ਵਿੱਚ,
           uint32_t *alias_ਅੰਕੜੇ, int32_t max_ਅੰਕੜੇ_ਲੰਮਾਈ,
           uint32_t initial_ਮੁੱਲ, uint32_t lead_Unit_ਮੁੱਲ,
           UBool latin1_ਰੇਖਿਕ) {
    UNewTrie *trie;
    int32_t i, j;

    ਜੇ( max_ਅੰਕੜੇ_ਲੰਮਾਈ<UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ ||
        (latin1_ਰੇਖਿਕ && max_ਅੰਕੜੇ_ਲੰਮਾਈ<1024)
    ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(fill_ਵਿੱਚ!=ਖਾਲੀ_ਸੂਚਕ) {
        trie=fill_ਵਿੱਚ;
    } ਨਹੀਂਤਾਂ {
        trie=(UNewTrie *)uprv_malloc(ਆਕਾਰ(UNewTrie));
        ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }
    uprv_memset(trie, 0, ਆਕਾਰ(UNewTrie));
    trie->isAllocated = fill_ਵਿੱਚ == ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ(alias_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ) {
        trie->ਅੰਕੜੇ=alias_ਅੰਕੜੇ;
        trie->is_ਅੰਕੜੇ_Allocated=ਝੂਠ;
    } ਨਹੀਂਤਾਂ {
        trie->ਅੰਕੜੇ=(uint32_t *)uprv_malloc(max_ਅੰਕੜੇ_ਲੰਮਾਈ*4);
        ਜੇ(trie->ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(trie);
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        trie->is_ਅੰਕੜੇ_Allocated=ਸੱਚ;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;

    ਜੇ(latin1_ਰੇਖਿਕ) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        ਕਰੋ {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->ਸੂਚਕਾਂਕ[i++]=j;
            j+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        } ਜਦੋਂ(i<(256>>UTRIE_ਖਿਸਕਾਓ));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->ਅੰਕੜੇ_ਲੰਮਾਈ=j;
    ਜਦੋਂ(j>0) {
        trie->ਅੰਕੜੇ[--j]=initial_ਮੁੱਲ;
    }

    trie->lead_Unit_ਮੁੱਲ=lead_Unit_ਮੁੱਲ;
    trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=UTRIE_MAX_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    trie->ਅੰਕੜੇ_ਸਮਰੱਥਾ=max_ਅੰਕੜੇ_ਲੰਮਾਈ;
    trie->is_Latin1_ਰੇਖਿਕ=latin1_ਰੇਖਿਕ;
    trie->isCompacted=ਝੂਠ;
    ਵਾਪਸ trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fill_ਵਿੱਚ, ਅਚਲ UNewTrie *other, uint32_t *alias_ਅੰਕੜੇ, int32_t alias_ਅੰਕੜੇ_ਸਮਰੱਥਾ) {
    UNewTrie *trie;
    UBool is_ਅੰਕੜੇ_Allocated;

    /* do not clone if other is not valid or already compacted */
    ਜੇ(other==ਖਾਲੀ_ਸੂਚਕ || other->ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || other->isCompacted) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* clone data */
    ਜੇ(alias_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ && alias_ਅੰਕੜੇ_ਸਮਰੱਥਾ>=other->ਅੰਕੜੇ_ਸਮਰੱਥਾ) {
        is_ਅੰਕੜੇ_Allocated=ਝੂਠ;
    } ਨਹੀਂਤਾਂ {
        alias_ਅੰਕੜੇ_ਸਮਰੱਥਾ=other->ਅੰਕੜੇ_ਸਮਰੱਥਾ;
        alias_ਅੰਕੜੇ=(uint32_t *)uprv_malloc(other->ਅੰਕੜੇ_ਸਮਰੱਥਾ*4);
        ਜੇ(alias_ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        is_ਅੰਕੜੇ_Allocated=ਸੱਚ;
    }

    trie=utrie_open(fill_ਵਿੱਚ, alias_ਅੰਕੜੇ, alias_ਅੰਕੜੇ_ਸਮਰੱਥਾ,
                    other->ਅੰਕੜੇ[0], other->lead_Unit_ਮੁੱਲ,
                    other->is_Latin1_ਰੇਖਿਕ);
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(alias_ਅੰਕੜੇ);
    } ਨਹੀਂਤਾਂ {
        uprv_memcpy(trie->ਸੂਚਕਾਂਕ, other->ਸੂਚਕਾਂਕ, ਆਕਾਰ(trie->ਸੂਚਕਾਂਕ));
        uprv_memcpy(trie->ਅੰਕੜੇ, other->ਅੰਕੜੇ, (size_t)other->ਅੰਕੜੇ_ਲੰਮਾਈ*4);
        trie->ਅੰਕੜੇ_ਲੰਮਾਈ=other->ਅੰਕੜੇ_ਲੰਮਾਈ;
        trie->is_ਅੰਕੜੇ_Allocated=is_ਅੰਕੜੇ_Allocated;
    }

    ਵਾਪਸ trie;
}

U_CAPI ਖਾਲੀ U_EXPORT2
utrie_close(UNewTrie *trie) {
    ਜੇ(trie!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(trie->is_ਅੰਕੜੇ_Allocated) {
            uprv_free(trie->ਅੰਕੜੇ);
            trie->ਅੰਕੜੇ=ਖਾਲੀ_ਸੂਚਕ;
        }
        ਜੇ(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_ਅੰਕੜੇ(UNewTrie *trie, int32_t *p_ਲੰਮਾਈ) {
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ || p_ਲੰਮਾਈ==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    *p_ਲੰਮਾਈ=trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    ਵਾਪਸ trie->ਅੰਕੜੇ;
}

ਸਥਿਰ int32_t
utrie_alloc_ਅੰਕੜੇ_ਖੰਡ(UNewTrie *trie) {
    int32_t new_ਖੰਡ, new_ਸਿਖਰ;

    new_ਖੰਡ=trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    new_ਸਿਖਰ=new_ਖੰਡ+UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    ਜੇ(new_ਸਿਖਰ>trie->ਅੰਕੜੇ_ਸਮਰੱਥਾ) {
        /* out of memory in the data array */
        ਵਾਪਸ -1;
    }
    trie->ਅੰਕੜੇ_ਲੰਮਾਈ=new_ਸਿਖਰ;
    ਵਾਪਸ new_ਖੰਡ;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ਸਥਿਰ int32_t
utrie_get_ਅੰਕੜੇ_ਖੰਡ(UNewTrie *trie, UChar32 c) {
    int32_t ਸੂਚਕਾਂਕ_ਮੁੱਲ, new_ਖੰਡ;

    c>>=UTRIE_ਖਿਸਕਾਓ;
    ਸੂਚਕਾਂਕ_ਮੁੱਲ=trie->ਸੂਚਕਾਂਕ[c];
    ਜੇ(ਸੂਚਕਾਂਕ_ਮੁੱਲ>0) {
        ਵਾਪਸ ਸੂਚਕਾਂਕ_ਮੁੱਲ;
    }

    /* allocate a new data block */
    new_ਖੰਡ=utrie_alloc_ਅੰਕੜੇ_ਖੰਡ(trie);
    ਜੇ(new_ਖੰਡ<0) {
        /* out of memory in the data array */
        ਵਾਪਸ -1;
    }
    trie->ਸੂਚਕਾਂਕ[c]=new_ਖੰਡ;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->ਅੰਕੜੇ+new_ਖੰਡ, trie->ਅੰਕੜੇ-ਸੂਚਕਾਂਕ_ਮੁੱਲ, 4*UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ);
    ਵਾਪਸ new_ਖੰਡ;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t ਖੰਡ;

    /* valid, uncompacted trie and valid c? */
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ || trie->isCompacted || (uint32_t)c>0x10ffff) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਖੰਡ=utrie_get_ਅੰਕੜੇ_ਖੰਡ(trie, c);
    ਜੇ(ਖੰਡ<0) {
        ਵਾਪਸ ਝੂਠ;
    }

    trie->ਅੰਕੜੇ[ਖੰਡ+(c&UTRIE_ਨਕਾਬ)]=value;
    ਵਾਪਸ ਸੱਚ;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *p_ਵਿੱਚ_ਖੰਡ_Zero) {
    int32_t ਖੰਡ;

    /* valid, uncompacted trie and valid c? */
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ || trie->isCompacted || (uint32_t)c>0x10ffff) {
        ਜੇ(p_ਵਿੱਚ_ਖੰਡ_Zero!=ਖਾਲੀ_ਸੂਚਕ) {
            *p_ਵਿੱਚ_ਖੰਡ_Zero=ਸੱਚ;
        }
        ਵਾਪਸ 0;
    }

    ਖੰਡ=trie->ਸੂਚਕਾਂਕ[c>>UTRIE_ਖਿਸਕਾਓ];
    ਜੇ(p_ਵਿੱਚ_ਖੰਡ_Zero!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਵਿੱਚ_ਖੰਡ_Zero = ਖੰਡ == 0;
    }

    ਵਾਪਸ trie->ਅੰਕੜੇ[ABS(ਖੰਡ)+(c&UTRIE_ਨਕਾਬ)];
}

/**
 * @internal
 */
ਸਥਿਰ ਖਾਲੀ
utrie_fill_ਖੰਡ(uint32_t *ਖੰਡ, UChar32 ਸ਼ੁਰੂ, UChar32 ਹੱਦਬੰਦੀ,
                uint32_t value, uint32_t initial_ਮੁੱਲ, UBool overwrite) {
    uint32_t *p_ਹੱਦਬੰਦੀ;

    p_ਹੱਦਬੰਦੀ=ਖੰਡ+ਹੱਦਬੰਦੀ;
    ਖੰਡ+=ਸ਼ੁਰੂ;
    ਜੇ(overwrite) {
        ਜਦੋਂ(ਖੰਡ<p_ਹੱਦਬੰਦੀ) {
            *ਖੰਡ++=value;
        }
    } ਨਹੀਂਤਾਂ {
        ਜਦੋਂ(ਖੰਡ<p_ਹੱਦਬੰਦੀ) {
            ਜੇ(*ਖੰਡ==initial_ਮੁੱਲ) {
                *ਖੰਡ=value;
            }
            ++ਖੰਡ;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_ਸੈੱਟ_Range32(UNewTrie *trie, UChar32 ਸ਼ੁਰੂ, UChar32 ਹੱਦਬੰਦੀ, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initial_ਮੁੱਲ;
    int32_t ਖੰਡ, rest, repeat_ਖੰਡ;

    /* valid, uncompacted trie and valid indexes? */
    ਜੇ( trie==ਖਾਲੀ_ਸੂਚਕ || trie->isCompacted ||
        (uint32_t)ਸ਼ੁਰੂ>0x10ffff || (uint32_t)ਹੱਦਬੰਦੀ>0x110000 || ਸ਼ੁਰੂ>ਹੱਦਬੰਦੀ
    ) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(ਸ਼ੁਰੂ==ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ ਸੱਚ; /* nothing to do */
    }

    initial_ਮੁੱਲ=trie->ਅੰਕੜੇ[0];
    ਜੇ(ਸ਼ੁਰੂ&UTRIE_ਨਕਾਬ) {
        UChar32 ਅਗਲਾ_ਸ਼ੁਰੂ;

        /* set partial block at [start..following block boundary[ */
        ਖੰਡ=utrie_get_ਅੰਕੜੇ_ਖੰਡ(trie, ਸ਼ੁਰੂ);
        ਜੇ(ਖੰਡ<0) {
            ਵਾਪਸ ਝੂਠ;
        }

        ਅਗਲਾ_ਸ਼ੁਰੂ=(ਸ਼ੁਰੂ+UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ)&~UTRIE_ਨਕਾਬ;
        ਜੇ(ਅਗਲਾ_ਸ਼ੁਰੂ<=ਹੱਦਬੰਦੀ) {
            utrie_fill_ਖੰਡ(trie->ਅੰਕੜੇ+ਖੰਡ, ਸ਼ੁਰੂ&UTRIE_ਨਕਾਬ, UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ,
                            value, initial_ਮੁੱਲ, overwrite);
            ਸ਼ੁਰੂ=ਅਗਲਾ_ਸ਼ੁਰੂ;
        } ਨਹੀਂਤਾਂ {
            utrie_fill_ਖੰਡ(trie->ਅੰਕੜੇ+ਖੰਡ, ਸ਼ੁਰੂ&UTRIE_ਨਕਾਬ, ਹੱਦਬੰਦੀ&UTRIE_ਨਕਾਬ,
                            value, initial_ਮੁੱਲ, overwrite);
            ਵਾਪਸ ਸੱਚ;
        }
    }

    /* number of positions in the last, partial block */
    rest=ਹੱਦਬੰਦੀ&UTRIE_ਨਕਾਬ;

    /* round down limit to a block boundary */
    ਹੱਦਬੰਦੀ&=~UTRIE_ਨਕਾਬ;

    /* iterate over all-value blocks */
    ਜੇ(value==initial_ਮੁੱਲ) {
        repeat_ਖੰਡ=0;
    } ਨਹੀਂਤਾਂ {
        repeat_ਖੰਡ=-1;
    }
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
        /* get index value */
        ਖੰਡ=trie->ਸੂਚਕਾਂਕ[ਸ਼ੁਰੂ>>UTRIE_ਖਿਸਕਾਓ];
        ਜੇ(ਖੰਡ>0) {
            /* already allocated, fill in value */
            utrie_fill_ਖੰਡ(trie->ਅੰਕੜੇ+ਖੰਡ, 0, UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ, value, initial_ਮੁੱਲ, overwrite);
        } ਨਹੀਂਤਾਂ ਜੇ(trie->ਅੰਕੜੇ[-ਖੰਡ]!=value && (ਖੰਡ==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            ਜੇ(repeat_ਖੰਡ>=0) {
                trie->ਸੂਚਕਾਂਕ[ਸ਼ੁਰੂ>>UTRIE_ਖਿਸਕਾਓ]=-repeat_ਖੰਡ;
            } ਨਹੀਂਤਾਂ {
                /* create and set and fill the repeatBlock */
                repeat_ਖੰਡ=utrie_get_ਅੰਕੜੇ_ਖੰਡ(trie, ਸ਼ੁਰੂ);
                ਜੇ(repeat_ਖੰਡ<0) {
                    ਵਾਪਸ ਝੂਠ;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->ਸੂਚਕਾਂਕ[ਸ਼ੁਰੂ>>UTRIE_ਖਿਸਕਾਓ]=-repeat_ਖੰਡ;
                utrie_fill_ਖੰਡ(trie->ਅੰਕੜੇ+repeat_ਖੰਡ, 0, UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ, value, initial_ਮੁੱਲ, ਸੱਚ);
            }
        }

        ਸ਼ੁਰੂ+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    }

    ਜੇ(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        ਖੰਡ=utrie_get_ਅੰਕੜੇ_ਖੰਡ(trie, ਸ਼ੁਰੂ);
        ਜੇ(ਖੰਡ<0) {
            ਵਾਪਸ ਝੂਠ;
        }

        utrie_fill_ਖੰਡ(trie->ਅੰਕੜੇ+ਖੰਡ, 0, rest, value, initial_ਮੁੱਲ, overwrite);
    }

    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ int32_t
ਲੱਭੋ_Same_ਸੂਚਕਾਂਕ_ਖੰਡ(ਅਚਲ int32_t *idx, int32_t ਸੂਚਕਾਂਕ_ਲੰਮਾਈ,
                    int32_t other_ਖੰਡ) {
    int32_t ਖੰਡ, i;

    ਲਈ(ਖੰਡ=UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ; ਖੰਡ<ਸੂਚਕਾਂਕ_ਲੰਮਾਈ; ਖੰਡ+=UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ) {
        ਲਈ(i=0; i<UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ; ++i) {
            ਜੇ(idx[ਖੰਡ+i]!=idx[other_ਖੰਡ+i]) {
                ਰੋਕੋ;
            }
        }
        ਜੇ(i==UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ) {
            ਵਾਪਸ ਖੰਡ;
        }
    }
    ਵਾਪਸ ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
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
ਸਥਿਰ ਖਾਲੀ
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_ਮੁੱਲ *get_Folded_ਮੁੱਲ, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t leadIndexes[UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਖੰਡ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE_DEBUG
    ਪੂਰਨ ਗਿਣਤੀ_Lead_CUWith_ਅੰਕੜੇ=0;
#ਜੇ_ਅੰਤ

    idx=trie->ਸੂਚਕਾਂਕ;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_ਖਿਸਕਾਓ), 4*UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ);

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
    ਜੇ(trie->lead_Unit_ਮੁੱਲ==trie->ਅੰਕੜੇ[0]) {
        ਖੰਡ=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } ਨਹੀਂਤਾਂ {
        /* create and fill the repeatBlock */
        ਖੰਡ=utrie_alloc_ਅੰਕੜੇ_ਖੰਡ(trie);
        ਜੇ(ਖੰਡ<0) {
            /* data table overflow */
            *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        utrie_fill_ਖੰਡ(trie->ਅੰਕੜੇ+ਖੰਡ, 0, UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ, trie->lead_Unit_ਮੁੱਲ, trie->ਅੰਕੜੇ[0], ਸੱਚ);
        ਖੰਡ=-ਖੰਡ; /* negative block number to indicate that it is a repeat block */
    }
    ਲਈ(c=(0xd800>>UTRIE_ਖਿਸਕਾਓ); c<(0xdc00>>UTRIE_ਖਿਸਕਾਓ); ++c) {
        trie->ਸੂਚਕਾਂਕ[c]=ਖੰਡ;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;

    /* search for any index (stage 1) entries for supplementary code points */
    ਲਈ(c=0x10000; c<0x110000;) {
        ਜੇ(idx[c>>UTRIE_ਖਿਸਕਾਓ]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE_DEBUG
            ++ਗਿਣਤੀ_Lead_CUWith_ਅੰਕੜੇ;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#ਜੇ_ਅੰਤ

            /* is there an identical index block? */
            ਖੰਡ=ਲੱਭੋ_Same_ਸੂਚਕਾਂਕ_ਖੰਡ(idx, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, c>>UTRIE_ਖਿਸਕਾਓ);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_ਮੁੱਲ(trie, c, ਖੰਡ+UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ);
            ਜੇ(value!=utrie_get32(trie, U16_LEAD(c), ਖਾਲੀ_ਸੂਚਕ)) {
                ਜੇ(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਵਾਪਸ;
                }

                /* if we did not find an identical index block... */
                ਜੇ(ਖੰਡ==ਸੂਚਕਾਂਕ_ਲੰਮਾਈ) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+ਸੂਚਕਾਂਕ_ਲੰਮਾਈ,
                                 idx+(c>>UTRIE_ਖਿਸਕਾਓ),
                                 4*UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ);
                    ਸੂਚਕਾਂਕ_ਲੰਮਾਈ+=UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ;
                }
            }
            c+=0x400;
        } ਨਹੀਂਤਾਂ {
            c+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        }
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE_DEBUG
    ਜੇ(ਗਿਣਤੀ_Lead_CUWith_ਅੰਕੜੇ>0) {
        ਲਿਖੋ("supplementary data for %d lead surrogates\n", ਗਿਣਤੀ_Lead_CUWith_ਅੰਕੜੇ);
    }
#ਜੇ_ਅੰਤ

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    ਜੇ(ਸੂਚਕਾਂਕ_ਲੰਮਾਈ>=UTRIE_MAX_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ+UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ,
                 idx+UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ,
                 4*(ਸੂਚਕਾਂਕ_ਲੰਮਾਈ-UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ));
    uprv_memcpy(idx+UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ,
                leadIndexes,
                4*UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ);
    ਸੂਚਕਾਂਕ_ਲੰਮਾਈ+=UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE_DEBUG
    ਲਿਖੋ("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, (ਲੰਮਾ)UTRIE_MAX_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);
#ਜੇ_ਅੰਤ

    trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
}

/*
 * Set a value in the trie index map to indicate which data block
 * is referenced and which one is not.
 * utrie_compact() will remove data blocks that are not used at all.
 * Set
 * - 0 if it is used
 * - -1 if it is not used
 */
ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_ਅਣਵਰਤਿਆ_ਖੰਡਾਂ(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_ਸਮਾਂ_ਅੰਕੜੇ_ਲੰਮਾਈ>>UTRIE_ਖਿਸਕਾਓ)*4);

    /* mark each block that _is_ used with 0 */
    ਲਈ(i=0; i<trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ; ++i) {
        trie->map[ABS(trie->ਸੂਚਕਾਂਕ[i])>>UTRIE_ਖਿਸਕਾਓ]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

ਸਥਿਰ int32_t
ਲੱਭੋ_Same_ਅੰਕੜੇ_ਖੰਡ(ਅਚਲ uint32_t *ਅੰਕੜੇ, int32_t ਅੰਕੜੇ_ਲੰਮਾਈ,
                   int32_t other_ਖੰਡ, int32_t ਕਦਮ) {
    int32_t ਖੰਡ;

    /* ensure that we do not even partially get past dataLength */
    ਅੰਕੜੇ_ਲੰਮਾਈ-=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;

    ਲਈ(ਖੰਡ=0; ਖੰਡ<=ਅੰਕੜੇ_ਲੰਮਾਈ; ਖੰਡ+=ਕਦਮ) {
        ਜੇ(equal_uint32(ਅੰਕੜੇ+ਖੰਡ, ਅੰਕੜੇ+other_ਖੰਡ, UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ)) {
            ਵਾਪਸ ਖੰਡ;
        }
    }
    ਵਾਪਸ -1;
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
ਸਥਿਰ ਖਾਲੀ
utrie_compact(UNewTrie *trie, UBool overlap, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    int32_t i, ਸ਼ੁਰੂ, new_ਸ਼ੁਰੂ, overlap_ਸ਼ੁਰੂ;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    /* valid, uncompacted trie? */
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    ਜੇ(trie->isCompacted) {
        ਵਾਪਸ; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    ਲੱਭੋ_ਅਣਵਰਤਿਆ_ਖੰਡਾਂ(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    ਜੇ(trie->is_Latin1_ਰੇਖਿਕ && UTRIE_ਖਿਸਕਾਓ<=8) {
        overlap_ਸ਼ੁਰੂ=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ+256;
    } ਨਹੀਂਤਾਂ {
        overlap_ਸ਼ੁਰੂ=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    }

    new_ਸ਼ੁਰੂ=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    ਲਈ(ਸ਼ੁਰੂ=new_ਸ਼ੁਰੂ; ਸ਼ੁਰੂ<trie->ਅੰਕੜੇ_ਲੰਮਾਈ;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        ਜੇ(trie->map[ਸ਼ੁਰੂ>>UTRIE_ਖਿਸਕਾਓ]<0) {
            /* advance start to the next block */
            ਸ਼ੁਰੂ+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;

            /* leave newStart with the previous block! */
            ਜਾਰੀ;
        }

        /* search for an identical block */
        ਜੇ( ਸ਼ੁਰੂ>=overlap_ਸ਼ੁਰੂ &&
            (i=ਲੱਭੋ_Same_ਅੰਕੜੇ_ਖੰਡ(trie->ਅੰਕੜੇ, new_ਸ਼ੁਰੂ, ਸ਼ੁਰੂ,
                            overlap ? UTRIE_ਅੰਕੜੇ_GRANULARITY : UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[ਸ਼ੁਰੂ>>UTRIE_ਖਿਸਕਾਓ]=i;

            /* advance start to the next block */
            ਸ਼ੁਰੂ+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;

            /* leave newStart with the previous block! */
            ਜਾਰੀ;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        ਜੇ(overlap && ਸ਼ੁਰੂ>=overlap_ਸ਼ੁਰੂ) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            ਲਈ(i=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ-UTRIE_ਅੰਕੜੇ_GRANULARITY;
                i>0 && !equal_uint32(trie->ਅੰਕੜੇ+(new_ਸ਼ੁਰੂ-i), trie->ਅੰਕੜੇ+ਸ਼ੁਰੂ, i);
                i-=UTRIE_ਅੰਕੜੇ_GRANULARITY) {}
        } ਨਹੀਂਤਾਂ {
            i=0;
        }

        ਜੇ(i>0) {
            /* some overlap */
            trie->map[ਸ਼ੁਰੂ>>UTRIE_ਖਿਸਕਾਓ]=new_ਸ਼ੁਰੂ-i;

            /* move the non-overlapping indexes to their new positions */
            ਸ਼ੁਰੂ+=i;
            ਲਈ(i=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ-i; i>0; --i) {
                trie->ਅੰਕੜੇ[new_ਸ਼ੁਰੂ++]=trie->ਅੰਕੜੇ[ਸ਼ੁਰੂ++];
            }
        } ਨਹੀਂਤਾਂ ਜੇ(new_ਸ਼ੁਰੂ<ਸ਼ੁਰੂ) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[ਸ਼ੁਰੂ>>UTRIE_ਖਿਸਕਾਓ]=new_ਸ਼ੁਰੂ;
            ਲਈ(i=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ; i>0; --i) {
                trie->ਅੰਕੜੇ[new_ਸ਼ੁਰੂ++]=trie->ਅੰਕੜੇ[ਸ਼ੁਰੂ++];
            }
        } ਨਹੀਂਤਾਂ /* no overlap && newStart==start */ {
            trie->map[ਸ਼ੁਰੂ>>UTRIE_ਖਿਸਕਾਓ]=ਸ਼ੁਰੂ;
            new_ਸ਼ੁਰੂ+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            ਸ਼ੁਰੂ=new_ਸ਼ੁਰੂ;
        }
    }

    /* now adjust the index (stage 1) table */
    ਲਈ(i=0; i<trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ; ++i) {
        trie->ਸੂਚਕਾਂਕ[i]=trie->map[ABS(trie->ਸੂਚਕਾਂਕ[i])>>UTRIE_ਖਿਸਕਾਓ];
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE_DEBUG
    /* we saved some space */
    ਲਿਖੋ("compacting trie: count of 32-bit words %lu->%lu\n",
            (ਲੰਮਾ)trie->ਅੰਕੜੇ_ਲੰਮਾਈ, (ਲੰਮਾ)new_ਸ਼ੁਰੂ);
#ਜੇ_ਅੰਤ

    trie->ਅੰਕੜੇ_ਲੰਮਾਈ=new_ਸ਼ੁਰੂ;
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
ਸਥਿਰ uint32_t U_CALLCONV
default_Get_Folded_ਮੁੱਲ(UNewTrie *trie, UChar32 ਸ਼ੁਰੂ, int32_t ਵਿੱਥ) {
    uint32_t value, initial_ਮੁੱਲ;
    UChar32 ਹੱਦਬੰਦੀ;
    UBool ਵਿੱਚ_ਖੰਡ_Zero;

    initial_ਮੁੱਲ=trie->ਅੰਕੜੇ[0];
    ਹੱਦਬੰਦੀ=ਸ਼ੁਰੂ+0x400;
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
        value=utrie_get32(trie, ਸ਼ੁਰੂ, &ਵਿੱਚ_ਖੰਡ_Zero);
        ਜੇ(ਵਿੱਚ_ਖੰਡ_Zero) {
            ਸ਼ੁਰੂ+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ(value!=initial_ਮੁੱਲ) {
            ਵਾਪਸ static_cast<uint32_t>(ਵਿੱਥ);
        } ਨਹੀਂਤਾਂ {
            ++ਸ਼ੁਰੂ;
        }
    }
    ਵਾਪਸ 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, ਖਾਲੀ *dt, int32_t capacity,
                UNew_Trie_Get_Folded_ਮੁੱਲ *get_Folded_ਮੁੱਲ,
                UBool reduce_To16_ਬਿੱਟ,
                UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UTrie_ਸਿਰਲੇਖ *ਸਿਰਲੇਖ;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;

    /* argument check */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ || capacity<0 || (capacity>0 && dt==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    ਜੇ(get_Folded_ਮੁੱਲ==ਖਾਲੀ_ਸੂਚਕ) {
        get_Folded_ਮੁੱਲ=default_Get_Folded_ਮੁੱਲ;
    }

    ਅੰਕੜੇ = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    ਜੇ(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, ਝੂਠ, p_Error_ਕੋਡ);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_ਮੁੱਲ, p_Error_ਕੋਡ);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, ਸੱਚ, p_Error_ਕੋਡ);

        trie->isCompacted=ਸੱਚ;
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
    }

    /* is dataLength within limits? */
    ਜੇ( (reduce_To16_ਬਿੱਟ ? (trie->ਅੰਕੜੇ_ਲੰਮਾਈ+trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ) : trie->ਅੰਕੜੇ_ਲੰਮਾਈ) >= UTRIE_MAX_ਅੰਕੜੇ_ਲੰਮਾਈ) {
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
    }

    length=ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ)+2*trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਜੇ(reduce_To16_ਬਿੱਟ) {
        length+=2*trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        length+=4*trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    }

    ਜੇ(length>capacity) {
        ਵਾਪਸ length; /* preflighting */
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE_DEBUG
    ਲਿਖੋ("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (ਲੰਮਾ)trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, (ਲੰਮਾ)trie->ਅੰਕੜੇ_ਲੰਮਾਈ, (ਲੰਮਾ)length);
#ਜੇ_ਅੰਤ

    /* set the header fields */
    ਸਿਰਲੇਖ=(UTrie_ਸਿਰਲੇਖ *)ਅੰਕੜੇ;
    ਅੰਕੜੇ+=ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ);

    ਸਿਰਲੇਖ->signature=0x54726965; /* "Trie" */
    ਸਿਰਲੇਖ->options=UTRIE_ਖਿਸਕਾਓ | (UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ<<UTRIE_OPTIONS_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);

    ਜੇ(!reduce_To16_ਬਿੱਟ) {
        ਸਿਰਲੇਖ->options|=UTRIE_OPTIONS_ਅੰਕੜੇ_IS_32_ਦੋਆਧਾਰੀ_ਅੰਕ;
    }
    ਜੇ(trie->is_Latin1_ਰੇਖਿਕ) {
        ਸਿਰਲੇਖ->options|=UTRIE_OPTIONS_LATIN1_IS_ਰੇਖਿਕ;
    }

    ਸਿਰਲੇਖ->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਲੰਮਾਈ=trie->ਅੰਕੜੇ_ਲੰਮਾਈ;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    ਜੇ(reduce_To16_ਬਿੱਟ) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->ਸੂਚਕਾਂਕ;
        dest16=(uint16_t *)ਅੰਕੜੇ;
        ਲਈ(i=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ)>>UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
        }

        /* write 16-bit data values */
        p=trie->ਅੰਕੜੇ;
        ਲਈ(i=trie->ਅੰਕੜੇ_ਲੰਮਾਈ; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } ਨਹੀਂਤਾਂ {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->ਸੂਚਕਾਂਕ;
        dest16=(uint16_t *)ਅੰਕੜੇ;
        ਲਈ(i=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->ਅੰਕੜੇ, 4*(size_t)trie->ਅੰਕੜੇ_ਲੰਮਾਈ);
    }

    ਵਾਪਸ length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_default_Get_Folding_ਵਿੱਥ(uint32_t ਅੰਕੜੇ) {
    ਵਾਪਸ (int32_t)ਅੰਕੜੇ;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, int32_t length, UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UTrie_ਸਿਰਲੇਖ *ਸਿਰਲੇਖ;
    ਅਚਲ uint16_t *p16;
    uint32_t options;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ -1;
    }

    /* enough data for a trie header? */
    ਜੇ(length<(int32_t)ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ)) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ -1;
    }

    /* check the signature */
    ਸਿਰਲੇਖ=(ਅਚਲ UTrie_ਸਿਰਲੇਖ *)ਅੰਕੜੇ;
    ਜੇ(ਸਿਰਲੇਖ->signature!=0x54726965) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ -1;
    }

    /* get the options and check the shift values */
    options=ਸਿਰਲੇਖ->options;
    ਜੇ( (options&UTRIE_OPTIONS_ਖਿਸਕਾਓ_ਨਕਾਬ)!=UTRIE_ਖਿਸਕਾਓ ||
        ((options>>UTRIE_OPTIONS_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ)&UTRIE_OPTIONS_ਖਿਸਕਾਓ_ਨਕਾਬ)!=UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ
    ) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ -1;
    }
    trie->is_Latin1_ਰੇਖਿਕ = (options & UTRIE_OPTIONS_LATIN1_IS_ਰੇਖਿਕ) != 0;

    /* get the length values */
    trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=ਸਿਰਲੇਖ->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    trie->ਅੰਕੜੇ_ਲੰਮਾਈ=ਸਿਰਲੇਖ->ਅੰਕੜੇ_ਲੰਮਾਈ;

    length-=(int32_t)ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ);

    /* enough data for the index? */
    ਜੇ(length<2*trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ -1;
    }
    p16=(ਅਚਲ uint16_t *)(ਸਿਰਲੇਖ+1);
    trie->ਸੂਚਕਾਂਕ=p16;
    p16+=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    length-=2*trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;

    /* get the data */
    ਜੇ(options&UTRIE_OPTIONS_ਅੰਕੜੇ_IS_32_ਦੋਆਧਾਰੀ_ਅੰਕ) {
        ਜੇ(length<4*trie->ਅੰਕੜੇ_ਲੰਮਾਈ) {
            *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
            ਵਾਪਸ -1;
        }
        trie->data32=(ਅਚਲ uint32_t *)p16;
        trie->initial_ਮੁੱਲ=trie->data32[0];
        length=(int32_t)ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ)+2*trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ+4*trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        ਜੇ(length<2*trie->ਅੰਕੜੇ_ਲੰਮਾਈ) {
            *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
            ਵਾਪਸ -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=ਖਾਲੀ_ਸੂਚਕ;
        trie->initial_ਮੁੱਲ=trie->ਸੂਚਕਾਂਕ[trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ];
        length=(int32_t)ਆਕਾਰ(UTrie_ਸਿਰਲੇਖ)+2*trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ+2*trie->ਅੰਕੜੇ_ਲੰਮਾਈ;
    }

    trie->get_Folding_ਵਿੱਥ=utrie_default_Get_Folding_ਵਿੱਥ;

    ਵਾਪਸ length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       ਖਾਲੀ *ਅੰਕੜੇ, int32_t length,
                       uint32_t initial_ਮੁੱਲ, uint32_t lead_Unit_ਮੁੱਲ,
                       UBool make16_ਦੋਆਧਾਰੀ_ਅੰਕ_Trie,
                       UError_ਕੋਡ *p_Error_ਕੋਡ) {
    uint16_t *p16;
    int32_t actual_ਲੰਮਾਈ, latin1_ਲੰਮਾਈ, i, ਹੱਦਬੰਦੀ;
    uint16_t ਖੰਡ;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_ਲੰਮਾਈ= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ+UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ;
    trie->ਅੰਕੜੇ_ਲੰਮਾਈ=latin1_ਲੰਮਾਈ;
    ਜੇ(lead_Unit_ਮੁੱਲ!=initial_ਮੁੱਲ) {
        trie->ਅੰਕੜੇ_ਲੰਮਾਈ+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
    }

    actual_ਲੰਮਾਈ=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2;
    ਜੇ(make16_ਦੋਆਧਾਰੀ_ਅੰਕ_Trie) {
        actual_ਲੰਮਾਈ+=trie->ਅੰਕੜੇ_ਲੰਮਾਈ*2;
    } ਨਹੀਂਤਾਂ {
        actual_ਲੰਮਾਈ+=trie->ਅੰਕੜੇ_ਲੰਮਾਈ*4;
    }

    /* enough space for the dummy trie? */
    ਜੇ(length<actual_ਲੰਮਾਈ) {
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ actual_ਲੰਮਾਈ;
    }

    trie->is_Latin1_ਰੇਖਿਕ=ਸੱਚ;
    trie->initial_ਮੁੱਲ=initial_ਮੁੱਲ;

    /* fill the index and data arrays */
    p16=(uint16_t *)ਅੰਕੜੇ;
    trie->ਸੂਚਕਾਂਕ=p16;

    ਜੇ(make16_ਦੋਆਧਾਰੀ_ਅੰਕ_Trie) {
        /* indexes to block 0 */
        ਖੰਡ=(uint16_t)(trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ>>UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
        ਹੱਦਬੰਦੀ=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
        ਲਈ(i=0; i<ਹੱਦਬੰਦੀ; ++i) {
            p16[i]=ਖੰਡ;
        }

        ਜੇ(lead_Unit_ਮੁੱਲ!=initial_ਮੁੱਲ) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            ਖੰਡ+=(uint16_t)(latin1_ਲੰਮਾਈ>>UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
            i=0xd800>>UTRIE_ਖਿਸਕਾਓ;
            ਹੱਦਬੰਦੀ=0xdc00>>UTRIE_ਖਿਸਕਾਓ;
            ਲਈ(; i<ਹੱਦਬੰਦੀ; ++i) {
                p16[i]=ਖੰਡ;
            }
        }

        trie->data32=ਖਾਲੀ_ਸੂਚਕ;

        /* Latin-1 data */
        p16+=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
        ਲਈ(i=0; i<latin1_ਲੰਮਾਈ; ++i) {
            p16[i]=(uint16_t)initial_ਮੁੱਲ;
        }

        /* data for lead surrogate code units */
        ਜੇ(lead_Unit_ਮੁੱਲ!=initial_ਮੁੱਲ) {
            ਹੱਦਬੰਦੀ=latin1_ਲੰਮਾਈ+UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            ਲਈ(/* i=latin1Length */; i<ਹੱਦਬੰਦੀ; ++i) {
                p16[i]=(uint16_t)lead_Unit_ਮੁੱਲ;
            }
        }
    } ਨਹੀਂਤਾਂ {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2);

        ਜੇ(lead_Unit_ਮੁੱਲ!=initial_ਮੁੱਲ) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            ਖੰਡ=(uint16_t)(latin1_ਲੰਮਾਈ>>UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
            i=0xd800>>UTRIE_ਖਿਸਕਾਓ;
            ਹੱਦਬੰਦੀ=0xdc00>>UTRIE_ਖਿਸਕਾਓ;
            ਲਈ(; i<ਹੱਦਬੰਦੀ; ++i) {
                p16[i]=ਖੰਡ;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ);

        /* Latin-1 data */
        ਲਈ(i=0; i<latin1_ਲੰਮਾਈ; ++i) {
            p32[i]=initial_ਮੁੱਲ;
        }

        /* data for lead surrogate code units */
        ਜੇ(lead_Unit_ਮੁੱਲ!=initial_ਮੁੱਲ) {
            ਹੱਦਬੰਦੀ=latin1_ਲੰਮਾਈ+UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            ਲਈ(/* i=latin1Length */; i<ਹੱਦਬੰਦੀ; ++i) {
                p32[i]=lead_Unit_ਮੁੱਲ;
            }
        }
    }

    trie->get_Folding_ਵਿੱਥ=utrie_default_Get_Folding_ਵਿੱਥ;

    ਵਾਪਸ actual_ਲੰਮਾਈ;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
ਸਥਿਰ uint32_t U_CALLCONV
enum_Same_ਮੁੱਲ(ਅਚਲ ਖਾਲੀ * /*context*/, uint32_t value) {
    ਵਾਪਸ value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI ਖਾਲੀ U_EXPORT2
utrie_enum(ਅਚਲ UTrie *trie,
           UTrie_Enum_ਮੁੱਲ *enum_ਮੁੱਲ, UTrieEnumRange *enumRange, ਅਚਲ ਖਾਲੀ *context) {
    ਅਚਲ uint32_t *data32;
    ਅਚਲ uint16_t *idx;

    uint32_t value, prev_ਮੁੱਲ, initial_ਮੁੱਲ;
    UChar32 c, prev;
    int32_t l, i, j, ਖੰਡ, prev_ਖੰਡ, null_ਖੰਡ, ਵਿੱਥ;

    /* check arguments */
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ || trie->ਸੂਚਕਾਂਕ==ਖਾਲੀ_ਸੂਚਕ || enumRange==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਜੇ(enum_ਮੁੱਲ==ਖਾਲੀ_ਸੂਚਕ) {
        enum_ਮੁੱਲ=enum_Same_ਮੁੱਲ;
    }

    idx=trie->ਸੂਚਕਾਂਕ;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_ਮੁੱਲ=enum_ਮੁੱਲ(context, trie->initial_ਮੁੱਲ);

    ਜੇ(data32==ਖਾਲੀ_ਸੂਚਕ) {
        null_ਖੰਡ=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        null_ਖੰਡ=0;
    }

    /* set variables for previous range */
    prev_ਖੰਡ=null_ਖੰਡ;
    prev=0;
    prev_ਮੁੱਲ=initial_ਮੁੱਲ;

    /* enumerate BMP - the main loop enumerates data blocks */
    ਲਈ(i=0, c=0; c<=0xffff; ++i) {
        ਜੇ(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_ਖਿਸਕਾਓ;
        }

        ਖੰਡ=idx[i]<<UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
        ਜੇ(ਖੰਡ==prev_ਖੰਡ) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ ਜੇ(ਖੰਡ==null_ਖੰਡ) {
            /* this is the all-initial-value block */
            ਜੇ(prev_ਮੁੱਲ!=initial_ਮੁੱਲ) {
                ਜੇ(prev<c) {
                    ਜੇ(!enumRange(context, prev, c, prev_ਮੁੱਲ)) {
                        ਵਾਪਸ;
                    }
                }
                prev_ਖੰਡ=null_ਖੰਡ;
                prev=c;
                prev_ਮੁੱਲ=initial_ਮੁੱਲ;
            }
            c+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
        } ਨਹੀਂਤਾਂ {
            prev_ਖੰਡ=ਖੰਡ;
            ਲਈ(j=0; j<UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ; ++j) {
                value=enum_ਮੁੱਲ(context, data32!=ਖਾਲੀ_ਸੂਚਕ ? data32[ਖੰਡ+j] : idx[ਖੰਡ+j]);
                ਜੇ(value!=prev_ਮੁੱਲ) {
                    ਜੇ(prev<c) {
                        ਜੇ(!enumRange(context, prev, c, prev_ਮੁੱਲ)) {
                            ਵਾਪਸ;
                        }
                    }
                    ਜੇ(j>0) {
                        /* the block is not filled with all the same value */
                        prev_ਖੰਡ=-1;
                    }
                    prev=c;
                    prev_ਮੁੱਲ=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    ਲਈ(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        ਵਿੱਥ=idx[l>>UTRIE_ਖਿਸਕਾਓ]<<UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
        ਜੇ(ਵਿੱਥ==null_ਖੰਡ) {
            /* no entries for a whole block of lead surrogates */
            ਜੇ(prev_ਮੁੱਲ!=initial_ਮੁੱਲ) {
                ਜੇ(prev<c) {
                    ਜੇ(!enumRange(context, prev, c, prev_ਮੁੱਲ)) {
                        ਵਾਪਸ;
                    }
                }
                prev_ਖੰਡ=null_ਖੰਡ;
                prev=c;
                prev_ਮੁੱਲ=initial_ਮੁੱਲ;
            }

            l+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
            c+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ<<10;
            ਜਾਰੀ;
        }

        value= data32!=ਖਾਲੀ_ਸੂਚਕ ? data32[ਵਿੱਥ+(l&UTRIE_ਨਕਾਬ)] : idx[ਵਿੱਥ+(l&UTRIE_ਨਕਾਬ)];

        /* enumerate trail surrogates for this lead surrogate */
        ਵਿੱਥ=trie->get_Folding_ਵਿੱਥ(value);
        ਜੇ(ਵਿੱਥ<=0) {
            /* no data for this lead surrogate */
            ਜੇ(prev_ਮੁੱਲ!=initial_ਮੁੱਲ) {
                ਜੇ(prev<c) {
                    ਜੇ(!enumRange(context, prev, c, prev_ਮੁੱਲ)) {
                        ਵਾਪਸ;
                    }
                }
                prev_ਖੰਡ=null_ਖੰਡ;
                prev=c;
                prev_ਮੁੱਲ=initial_ਮੁੱਲ;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } ਨਹੀਂਤਾਂ {
            /* enumerate code points for this lead surrogate */
            i=ਵਿੱਥ;
            ਵਿੱਥ+=UTRIE_SURROGATE_ਖੰਡ_ਗਿਣਤੀ;
            ਕਰੋ {
                /* copy of most of the body of the BMP loop */
                ਖੰਡ=idx[i]<<UTRIE_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
                ਜੇ(ਖੰਡ==prev_ਖੰਡ) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
                } ਨਹੀਂਤਾਂ ਜੇ(ਖੰਡ==null_ਖੰਡ) {
                    /* this is the all-initial-value block */
                    ਜੇ(prev_ਮੁੱਲ!=initial_ਮੁੱਲ) {
                        ਜੇ(prev<c) {
                            ਜੇ(!enumRange(context, prev, c, prev_ਮੁੱਲ)) {
                                ਵਾਪਸ;
                            }
                        }
                        prev_ਖੰਡ=null_ਖੰਡ;
                        prev=c;
                        prev_ਮੁੱਲ=initial_ਮੁੱਲ;
                    }
                    c+=UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
                } ਨਹੀਂਤਾਂ {
                    prev_ਖੰਡ=ਖੰਡ;
                    ਲਈ(j=0; j<UTRIE_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ; ++j) {
                        value=enum_ਮੁੱਲ(context, data32!=ਖਾਲੀ_ਸੂਚਕ ? data32[ਖੰਡ+j] : idx[ਖੰਡ+j]);
                        ਜੇ(value!=prev_ਮੁੱਲ) {
                            ਜੇ(prev<c) {
                                ਜੇ(!enumRange(context, prev, c, prev_ਮੁੱਲ)) {
                                    ਵਾਪਸ;
                                }
                            }
                            ਜੇ(j>0) {
                                /* the block is not filled with all the same value */
                                prev_ਖੰਡ=-1;
                            }
                            prev=c;
                            prev_ਮੁੱਲ=value;
                        }
                        ++c;
                    }
                }
            } ਜਦੋਂ(++i<ਵਿੱਥ);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_ਮੁੱਲ);
}

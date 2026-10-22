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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE_DEBUG
#   include <stdio.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "utrie.h"

/* miscellaneous ------------------------------------------------------------ */

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ABS
#ವ್ಯಾಖ್ಯಾನಿಸು ABS(x) ((x)>=0 ? (x) : -(x))

ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool
equal_uint32(ಸ್ಥಿರಾಂಕ uint32_t *s, ಸ್ಥಿರಾಂಕ uint32_t *t, int32_t length) {
    ಆಗ(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    ಹಿಂತಿರುಗಿ length == 0;
}

/* Building a trie ----------------------------------------------------------*/

U_CAPI UNewTrie * U_EXPORT2
utrie_open(UNewTrie *fill_ಒಳ,
           uint32_t *alias_ದತ್ತಾಂಶ, int32_t max_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ,
           uint32_t initial_ಮೌಲ್ಯ, uint32_t lead_Unit_ಮೌಲ್ಯ,
           UBool latin1_ರೇಖೀಯ) {
    UNewTrie *trie;
    int32_t i, j;

    ಆದರೆ( max_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ<UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ ||
        (latin1_ರೇಖೀಯ && max_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ<1024)
    ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(fill_ಒಳ!=ಶೂನ್ಯಸೂಚಕ) {
        trie=fill_ಒಳ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        trie=(UNewTrie *)uprv_malloc(ಗಾತ್ರ(UNewTrie));
        ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }
    uprv_memset(trie, 0, ಗಾತ್ರ(UNewTrie));
    trie->isAllocated = fill_ಒಳ == ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ(alias_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ) {
        trie->ದತ್ತಾಂಶ=alias_ದತ್ತಾಂಶ;
        trie->is_ದತ್ತಾಂಶ_Allocated=ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        trie->ದತ್ತಾಂಶ=(uint32_t *)uprv_malloc(max_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4);
        ಆದರೆ(trie->ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(trie);
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        trie->is_ದತ್ತಾಂಶ_Allocated=ನಿಜ;
    }

    /* preallocate and reset the first data block (block index 0) */
    j=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;

    ಆದರೆ(latin1_ರೇಖೀಯ) {
        /* preallocate and reset the first block (number 0) and Latin-1 (U+0000..U+00ff) after that */
        /* made sure above that maxDataLength>=1024 */

        /* set indexes to point to consecutive data blocks */
        i=0;
        ಮಾಡು {
            /* do this at least for trie->index[0] even if that block is only partly used for Latin-1 */
            trie->ಸೂಚ್ಯಂಕ[i++]=j;
            j+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        } ಆಗ(i<(256>>UTRIE_ಸರಿಸು));
    }

    /* reset the initially allocated blocks to the initial value */
    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=j;
    ಆಗ(j>0) {
        trie->ದತ್ತಾಂಶ[--j]=initial_ಮೌಲ್ಯ;
    }

    trie->lead_Unit_ಮೌಲ್ಯ=lead_Unit_ಮೌಲ್ಯ;
    trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=UTRIE_MAX_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    trie->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ=max_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    trie->is_Latin1_ರೇಖೀಯ=latin1_ರೇಖೀಯ;
    trie->isCompacted=ಸುಳ್ಳು;
    ಹಿಂತಿರುಗಿ trie;
}

U_CAPI UNewTrie * U_EXPORT2
utrie_clone(UNewTrie *fill_ಒಳ, ಸ್ಥಿರಾಂಕ UNewTrie *other, uint32_t *alias_ದತ್ತಾಂಶ, int32_t alias_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ) {
    UNewTrie *trie;
    UBool is_ದತ್ತಾಂಶ_Allocated;

    /* do not clone if other is not valid or already compacted */
    ಆದರೆ(other==ಶೂನ್ಯಸೂಚಕ || other->ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || other->isCompacted) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* clone data */
    ಆದರೆ(alias_ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ && alias_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ>=other->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ) {
        is_ದತ್ತಾಂಶ_Allocated=ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        alias_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ=other->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ;
        alias_ದತ್ತಾಂಶ=(uint32_t *)uprv_malloc(other->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ*4);
        ಆದರೆ(alias_ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        is_ದತ್ತಾಂಶ_Allocated=ನಿಜ;
    }

    trie=utrie_open(fill_ಒಳ, alias_ದತ್ತಾಂಶ, alias_ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ,
                    other->ದತ್ತಾಂಶ[0], other->lead_Unit_ಮೌಲ್ಯ,
                    other->is_Latin1_ರೇಖೀಯ);
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(alias_ದತ್ತಾಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        uprv_memcpy(trie->ಸೂಚ್ಯಂಕ, other->ಸೂಚ್ಯಂಕ, ಗಾತ್ರ(trie->ಸೂಚ್ಯಂಕ));
        uprv_memcpy(trie->ದತ್ತಾಂಶ, other->ದತ್ತಾಂಶ, (size_t)other->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4);
        trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=other->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
        trie->is_ದತ್ತಾಂಶ_Allocated=is_ದತ್ತಾಂಶ_Allocated;
    }

    ಹಿಂತಿರುಗಿ trie;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie_close(UNewTrie *trie) {
    ಆದರೆ(trie!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(trie->is_ದತ್ತಾಂಶ_Allocated) {
            uprv_free(trie->ದತ್ತಾಂಶ);
            trie->ದತ್ತಾಂಶ=ಶೂನ್ಯಸೂಚಕ;
        }
        ಆದರೆ(trie->isAllocated) {
            uprv_free(trie);
        }
    }
}

U_CAPI uint32_t * U_EXPORT2
utrie_get_ದತ್ತಾಂಶ(UNewTrie *trie, int32_t *p_ಉದ್ದಳತೆ) {
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ || p_ಉದ್ದಳತೆ==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    *p_ಉದ್ದಳತೆ=trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    ಹಿಂತಿರುಗಿ trie->ದತ್ತಾಂಶ;
}

ಸ್ಥಿರ int32_t
utrie_alloc_ದತ್ತಾಂಶ_ಖಂಡ(UNewTrie *trie) {
    int32_t new_ಖಂಡ, new_ತುದಿ;

    new_ಖಂಡ=trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    new_ತುದಿ=new_ಖಂಡ+UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    ಆದರೆ(new_ತುದಿ>trie->ದತ್ತಾಂಶ_ಸಾಮರ್ಥ್ಯ) {
        /* out of memory in the data array */
        ಹಿಂತಿರುಗಿ -1;
    }
    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=new_ತುದಿ;
    ಹಿಂತಿರುಗಿ new_ಖಂಡ;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
ಸ್ಥಿರ int32_t
utrie_get_ದತ್ತಾಂಶ_ಖಂಡ(UNewTrie *trie, UChar32 c) {
    int32_t ಸೂಚ್ಯಂಕ_ಮೌಲ್ಯ, new_ಖಂಡ;

    c>>=UTRIE_ಸರಿಸು;
    ಸೂಚ್ಯಂಕ_ಮೌಲ್ಯ=trie->ಸೂಚ್ಯಂಕ[c];
    ಆದರೆ(ಸೂಚ್ಯಂಕ_ಮೌಲ್ಯ>0) {
        ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_ಮೌಲ್ಯ;
    }

    /* allocate a new data block */
    new_ಖಂಡ=utrie_alloc_ದತ್ತಾಂಶ_ಖಂಡ(trie);
    ಆದರೆ(new_ಖಂಡ<0) {
        /* out of memory in the data array */
        ಹಿಂತಿರುಗಿ -1;
    }
    trie->ಸೂಚ್ಯಂಕ[c]=new_ಖಂಡ;

    /* copy-on-write for a block from a setRange() */
    uprv_memcpy(trie->ದತ್ತಾಂಶ+new_ಖಂಡ, trie->ದತ್ತಾಂಶ-ಸೂಚ್ಯಂಕ_ಮೌಲ್ಯ, 4*UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ new_ಖಂಡ;
}

/**
 * @return true if the value was successfully set
 */
U_CAPI UBool U_EXPORT2
utrie_set32(UNewTrie *trie, UChar32 c, uint32_t value) {
    int32_t ಖಂಡ;

    /* valid, uncompacted trie and valid c? */
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ || trie->isCompacted || (uint32_t)c>0x10ffff) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಖಂಡ=utrie_get_ದತ್ತಾಂಶ_ಖಂಡ(trie, c);
    ಆದರೆ(ಖಂಡ<0) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    trie->ದತ್ತಾಂಶ[ಖಂಡ+(c&UTRIE_ಮುಖವಾಡ)]=value;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

U_CAPI uint32_t U_EXPORT2
utrie_get32(UNewTrie *trie, UChar32 c, UBool *p_ಒಳ_ಖಂಡ_Zero) {
    int32_t ಖಂಡ;

    /* valid, uncompacted trie and valid c? */
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ || trie->isCompacted || (uint32_t)c>0x10ffff) {
        ಆದರೆ(p_ಒಳ_ಖಂಡ_Zero!=ಶೂನ್ಯಸೂಚಕ) {
            *p_ಒಳ_ಖಂಡ_Zero=ನಿಜ;
        }
        ಹಿಂತಿರುಗಿ 0;
    }

    ಖಂಡ=trie->ಸೂಚ್ಯಂಕ[c>>UTRIE_ಸರಿಸು];
    ಆದರೆ(p_ಒಳ_ಖಂಡ_Zero!=ಶೂನ್ಯಸೂಚಕ) {
        *p_ಒಳ_ಖಂಡ_Zero = ಖಂಡ == 0;
    }

    ಹಿಂತಿರುಗಿ trie->ದತ್ತಾಂಶ[ABS(ಖಂಡ)+(c&UTRIE_ಮುಖವಾಡ)];
}

/**
 * @internal
 */
ಸ್ಥಿರ ಶೂನ್ಯ
utrie_fill_ಖಂಡ(uint32_t *ಖಂಡ, UChar32 ಆರಂಭ, UChar32 ಎಲ್ಲೆ,
                uint32_t value, uint32_t initial_ಮೌಲ್ಯ, UBool overwrite) {
    uint32_t *p_ಎಲ್ಲೆ;

    p_ಎಲ್ಲೆ=ಖಂಡ+ಎಲ್ಲೆ;
    ಖಂಡ+=ಆರಂಭ;
    ಆದರೆ(overwrite) {
        ಆಗ(ಖಂಡ<p_ಎಲ್ಲೆ) {
            *ಖಂಡ++=value;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಗ(ಖಂಡ<p_ಎಲ್ಲೆ) {
            ಆದರೆ(*ಖಂಡ==initial_ಮೌಲ್ಯ) {
                *ಖಂಡ=value;
            }
            ++ಖಂಡ;
        }
    }
}

U_CAPI UBool U_EXPORT2
utrie_ಗಣ_Range32(UNewTrie *trie, UChar32 ಆರಂಭ, UChar32 ಎಲ್ಲೆ, uint32_t value, UBool overwrite) {
    /*
     * repeat value in [start..limit[
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    uint32_t initial_ಮೌಲ್ಯ;
    int32_t ಖಂಡ, rest, repeat_ಖಂಡ;

    /* valid, uncompacted trie and valid indexes? */
    ಆದರೆ( trie==ಶೂನ್ಯಸೂಚಕ || trie->isCompacted ||
        (uint32_t)ಆರಂಭ>0x10ffff || (uint32_t)ಎಲ್ಲೆ>0x110000 || ಆರಂಭ>ಎಲ್ಲೆ
    ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(ಆರಂಭ==ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ ನಿಜ; /* nothing to do */
    }

    initial_ಮೌಲ್ಯ=trie->ದತ್ತಾಂಶ[0];
    ಆದರೆ(ಆರಂಭ&UTRIE_ಮುಖವಾಡ) {
        UChar32 ಮುಂದಿನ_ಆರಂಭ;

        /* set partial block at [start..following block boundary[ */
        ಖಂಡ=utrie_get_ದತ್ತಾಂಶ_ಖಂಡ(trie, ಆರಂಭ);
        ಆದರೆ(ಖಂಡ<0) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        ಮುಂದಿನ_ಆರಂಭ=(ಆರಂಭ+UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ)&~UTRIE_ಮುಖವಾಡ;
        ಆದರೆ(ಮುಂದಿನ_ಆರಂಭ<=ಎಲ್ಲೆ) {
            utrie_fill_ಖಂಡ(trie->ದತ್ತಾಂಶ+ಖಂಡ, ಆರಂಭ&UTRIE_ಮುಖವಾಡ, UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ,
                            value, initial_ಮೌಲ್ಯ, overwrite);
            ಆರಂಭ=ಮುಂದಿನ_ಆರಂಭ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            utrie_fill_ಖಂಡ(trie->ದತ್ತಾಂಶ+ಖಂಡ, ಆರಂಭ&UTRIE_ಮುಖವಾಡ, ಎಲ್ಲೆ&UTRIE_ಮುಖವಾಡ,
                            value, initial_ಮೌಲ್ಯ, overwrite);
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }

    /* number of positions in the last, partial block */
    rest=ಎಲ್ಲೆ&UTRIE_ಮುಖವಾಡ;

    /* round down limit to a block boundary */
    ಎಲ್ಲೆ&=~UTRIE_ಮುಖವಾಡ;

    /* iterate over all-value blocks */
    ಆದರೆ(value==initial_ಮೌಲ್ಯ) {
        repeat_ಖಂಡ=0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        repeat_ಖಂಡ=-1;
    }
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ) {
        /* get index value */
        ಖಂಡ=trie->ಸೂಚ್ಯಂಕ[ಆರಂಭ>>UTRIE_ಸರಿಸು];
        ಆದರೆ(ಖಂಡ>0) {
            /* already allocated, fill in value */
            utrie_fill_ಖಂಡ(trie->ದತ್ತಾಂಶ+ಖಂಡ, 0, UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ, value, initial_ಮೌಲ್ಯ, overwrite);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(trie->ದತ್ತಾಂಶ[-ಖಂಡ]!=value && (ಖಂಡ==0 || overwrite)) {
            /* set the repeatBlock instead of the current block 0 or range block */
            ಆದರೆ(repeat_ಖಂಡ>=0) {
                trie->ಸೂಚ್ಯಂಕ[ಆರಂಭ>>UTRIE_ಸರಿಸು]=-repeat_ಖಂಡ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* create and set and fill the repeatBlock */
                repeat_ಖಂಡ=utrie_get_ದತ್ತಾಂಶ_ಖಂಡ(trie, ಆರಂಭ);
                ಆದರೆ(repeat_ಖಂಡ<0) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }

                /* set the negative block number to indicate that it is a repeat block */
                trie->ಸೂಚ್ಯಂಕ[ಆರಂಭ>>UTRIE_ಸರಿಸು]=-repeat_ಖಂಡ;
                utrie_fill_ಖಂಡ(trie->ದತ್ತಾಂಶ+repeat_ಖಂಡ, 0, UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ, value, initial_ಮೌಲ್ಯ, ನಿಜ);
            }
        }

        ಆರಂಭ+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    }

    ಆದರೆ(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        ಖಂಡ=utrie_get_ದತ್ತಾಂಶ_ಖಂಡ(trie, ಆರಂಭ);
        ಆದರೆ(ಖಂಡ<0) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        utrie_fill_ಖಂಡ(trie->ದತ್ತಾಂಶ+ಖಂಡ, 0, rest, value, initial_ಮೌಲ್ಯ, overwrite);
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ int32_t
ಕಂಡುಹಿಡಿ_Same_ಸೂಚ್ಯಂಕ_ಖಂಡ(ಸ್ಥಿರಾಂಕ int32_t *idx, int32_t ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ,
                    int32_t other_ಖಂಡ) {
    int32_t ಖಂಡ, i;

    ಗಾಗಿ(ಖಂಡ=UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ; ಖಂಡ<ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ; ಖಂಡ+=UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ) {
        ಗಾಗಿ(i=0; i<UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ; ++i) {
            ಆದರೆ(idx[ಖಂಡ+i]!=idx[other_ಖಂಡ+i]) {
                ನಿಲ್ಲಿಸು;
            }
        }
        ಆದರೆ(i==UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ) {
            ಹಿಂತಿರುಗಿ ಖಂಡ;
        }
    }
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
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
ಸ್ಥಿರ ಶೂನ್ಯ
utrie_fold(UNewTrie *trie, UNew_Trie_Get_Folded_ಮೌಲ್ಯ *get_Folded_ಮೌಲ್ಯ, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t leadIndexes[UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ];
    int32_t *idx;
    uint32_t value;
    UChar32 c;
    int32_t ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ಖಂಡ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE_DEBUG
    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_Lead_CUWith_ದತ್ತಾಂಶ=0;
#ಆದರೆ_ಅಂತ್ಯ

    idx=trie->ಸೂಚ್ಯಂಕ;

    /* copy the lead surrogate indexes into a temporary array */
    uprv_memcpy(leadIndexes, idx+(0xd800>>UTRIE_ಸರಿಸು), 4*UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ);

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
    ಆದರೆ(trie->lead_Unit_ಮೌಲ್ಯ==trie->ದತ್ತಾಂಶ[0]) {
        ಖಂಡ=0; /* leadUnitValue==initialValue, use all-initial-value block */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* create and fill the repeatBlock */
        ಖಂಡ=utrie_alloc_ದತ್ತಾಂಶ_ಖಂಡ(trie);
        ಆದರೆ(ಖಂಡ<0) {
            /* data table overflow */
            *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        utrie_fill_ಖಂಡ(trie->ದತ್ತಾಂಶ+ಖಂಡ, 0, UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ, trie->lead_Unit_ಮೌಲ್ಯ, trie->ದತ್ತಾಂಶ[0], ನಿಜ);
        ಖಂಡ=-ಖಂಡ; /* negative block number to indicate that it is a repeat block */
    }
    ಗಾಗಿ(c=(0xd800>>UTRIE_ಸರಿಸು); c<(0xdc00>>UTRIE_ಸರಿಸು); ++c) {
        trie->ಸೂಚ್ಯಂಕ[c]=ಖಂಡ;
    }

    /*
     * Fold significant index values into the area just after the BMP indexes.
     * In case the first lead surrogate has significant data,
     * its index block must be used first (in which case the folding is a no-op).
     * Later all folded index blocks are moved up one to insert the copied
     * lead surrogate indexes.
     */
    ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;

    /* search for any index (stage 1) entries for supplementary code points */
    ಗಾಗಿ(c=0x10000; c<0x110000;) {
        ಆದರೆ(idx[c>>UTRIE_ಸರಿಸು]!=0) {
            /* there is data, treat the full block for a lead surrogate */
            c&=~0x3ff;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE_DEBUG
            ++ಎಣಿಕೆ_Lead_CUWith_ದತ್ತಾಂಶ;
            /* printf("supplementary data for lead surrogate U+%04lx\n", (long)(0xd7c0+(c>>10))); */
#ಆದರೆ_ಅಂತ್ಯ

            /* is there an identical index block? */
            ಖಂಡ=ಕಂಡುಹಿಡಿ_Same_ಸೂಚ್ಯಂಕ_ಖಂಡ(idx, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, c>>UTRIE_ಸರಿಸು);

            /*
             * get a folded value for [c..c+0x400[ and,
             * if different from the value for the lead surrogate code point,
             * set it for the lead surrogate code unit
             */
            value=get_Folded_ಮೌಲ್ಯ(trie, c, ಖಂಡ+UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ);
            ಆದರೆ(value!=utrie_get32(trie, U16_LEAD(c), ಶೂನ್ಯಸೂಚಕ)) {
                ಆದರೆ(!utrie_set32(trie, U16_LEAD(c), value)) {
                    /* data table overflow */
                    *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಹಿಂತಿರುಗಿ;
                }

                /* if we did not find an identical index block... */
                ಆದರೆ(ಖಂಡ==ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ) {
                    /* move the actual index (stage 1) entries from the supplementary position to the new one */
                    uprv_memmove(idx+ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ,
                                 idx+(c>>UTRIE_ಸರಿಸು),
                                 4*UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ);
                    ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ+=UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ;
                }
            }
            c+=0x400;
        } ಇಲ್ಲದಿದ್ದರೆ {
            c+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        }
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE_DEBUG
    ಆದರೆ(ಎಣಿಕೆ_Lead_CUWith_ದತ್ತಾಂಶ>0) {
        ಬರೆ("supplementary data for %d lead surrogates\n", ಎಣಿಕೆ_Lead_CUWith_ದತ್ತಾಂಶ);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * index array overflow?
     * This is to guarantee that a folding offset is of the form
     * UTRIE_BMP_INDEX_LENGTH+n*UTRIE_SURROGATE_BLOCK_COUNT with n=0..1023.
     * If the index is too large, then n>=1024 and more than 10 bits are necessary.
     *
     * In fact, it can only ever become n==1024 with completely unfoldable data and
     * the additional block of duplicated values for lead surrogates.
     */
    ಆದರೆ(ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ>=UTRIE_MAX_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    /*
     * make space for the lead surrogate index block and
     * insert it between the BMP indexes and the folded ones
     */
    uprv_memmove(idx+UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ+UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ,
                 idx+UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ,
                 4*(ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ-UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ));
    uprv_memcpy(idx+UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ,
                leadIndexes,
                4*UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ);
    ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ+=UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE_DEBUG
    ಬರೆ("trie index count: BMP %ld  all Unicode %ld  folded %ld\n",
           UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, (ದೀರ್ಘ)UTRIE_MAX_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);
#ಆದರೆ_ಅಂತ್ಯ

    trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
}

/*
 * Set a value in the trie index map to indicate which data block
 * is referenced and which one is not.
 * utrie_compact() will remove data blocks that are not used at all.
 * Set
 * - 0 if it is used
 * - -1 if it is not used
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_ಬಳಸದ_ಖಂಡಗಳು(UNewTrie *trie) {
    int32_t i;

    /* fill the entire map with "not used" */
    uprv_memset(trie->map, 0xff, (UTRIE_MAX_BUILD_ಸಮಯ_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ>>UTRIE_ಸರಿಸು)*4);

    /* mark each block that _is_ used with 0 */
    ಗಾಗಿ(i=0; i<trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ; ++i) {
        trie->map[ABS(trie->ಸೂಚ್ಯಂಕ[i])>>UTRIE_ಸರಿಸು]=0;
    }

    /* never move the all-initial-value block 0 */
    trie->map[0]=0;
}

ಸ್ಥಿರ int32_t
ಕಂಡುಹಿಡಿ_Same_ದತ್ತಾಂಶ_ಖಂಡ(ಸ್ಥಿರಾಂಕ uint32_t *ದತ್ತಾಂಶ, int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ,
                   int32_t other_ಖಂಡ, int32_t ಹೆಜ್ಜೆ) {
    int32_t ಖಂಡ;

    /* ensure that we do not even partially get past dataLength */
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ-=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;

    ಗಾಗಿ(ಖಂಡ=0; ಖಂಡ<=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ; ಖಂಡ+=ಹೆಜ್ಜೆ) {
        ಆದರೆ(equal_uint32(ದತ್ತಾಂಶ+ಖಂಡ, ದತ್ತಾಂಶ+other_ಖಂಡ, UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ)) {
            ಹಿಂತಿರುಗಿ ಖಂಡ;
        }
    }
    ಹಿಂತಿರುಗಿ -1;
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
ಸ್ಥಿರ ಶೂನ್ಯ
utrie_compact(UNewTrie *trie, UBool overlap, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    int32_t i, ಆರಂಭ, new_ಆರಂಭ, overlap_ಆರಂಭ;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* valid, uncompacted trie? */
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(trie->isCompacted) {
        ಹಿಂತಿರುಗಿ; /* nothing left to do */
    }

    /* compaction */

    /* initialize the index map with "block is used/unused" flags */
    ಕಂಡುಹಿಡಿ_ಬಳಸದ_ಖಂಡಗಳು(trie);

    /* if Latin-1 is preallocated and linear, then do not compact Latin-1 data */
    ಆದರೆ(trie->is_Latin1_ರೇಖೀಯ && UTRIE_ಸರಿಸು<=8) {
        overlap_ಆರಂಭ=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ+256;
    } ಇಲ್ಲದಿದ್ದರೆ {
        overlap_ಆರಂಭ=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    }

    new_ಆರಂಭ=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    ಗಾಗಿ(ಆರಂಭ=new_ಆರಂಭ; ಆರಂಭ<trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* skip blocks that are not used */
        ಆದರೆ(trie->map[ಆರಂಭ>>UTRIE_ಸರಿಸು]<0) {
            /* advance start to the next block */
            ಆರಂಭ+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;

            /* leave newStart with the previous block! */
            ಮುಂದುವರಿ;
        }

        /* search for an identical block */
        ಆದರೆ( ಆರಂಭ>=overlap_ಆರಂಭ &&
            (i=ಕಂಡುಹಿಡಿ_Same_ದತ್ತಾಂಶ_ಖಂಡ(trie->ದತ್ತಾಂಶ, new_ಆರಂಭ, ಆರಂಭ,
                            overlap ? UTRIE_ದತ್ತಾಂಶ_GRANULARITY : UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[ಆರಂಭ>>UTRIE_ಸರಿಸು]=i;

            /* advance start to the next block */
            ಆರಂಭ+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;

            /* leave newStart with the previous block! */
            ಮುಂದುವರಿ;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        ಆದರೆ(overlap && ಆರಂಭ>=overlap_ಆರಂಭ) {
            /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
            ಗಾಗಿ(i=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ-UTRIE_ದತ್ತಾಂಶ_GRANULARITY;
                i>0 && !equal_uint32(trie->ದತ್ತಾಂಶ+(new_ಆರಂಭ-i), trie->ದತ್ತಾಂಶ+ಆರಂಭ, i);
                i-=UTRIE_ದತ್ತಾಂಶ_GRANULARITY) {}
        } ಇಲ್ಲದಿದ್ದರೆ {
            i=0;
        }

        ಆದರೆ(i>0) {
            /* some overlap */
            trie->map[ಆರಂಭ>>UTRIE_ಸರಿಸು]=new_ಆರಂಭ-i;

            /* move the non-overlapping indexes to their new positions */
            ಆರಂಭ+=i;
            ಗಾಗಿ(i=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ-i; i>0; --i) {
                trie->ದತ್ತಾಂಶ[new_ಆರಂಭ++]=trie->ದತ್ತಾಂಶ[ಆರಂಭ++];
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(new_ಆರಂಭ<ಆರಂಭ) {
            /* no overlap, just move the indexes to their new positions */
            trie->map[ಆರಂಭ>>UTRIE_ಸರಿಸು]=new_ಆರಂಭ;
            ಗಾಗಿ(i=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ; i>0; --i) {
                trie->ದತ್ತಾಂಶ[new_ಆರಂಭ++]=trie->ದತ್ತಾಂಶ[ಆರಂಭ++];
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* no overlap && newStart==start */ {
            trie->map[ಆರಂಭ>>UTRIE_ಸರಿಸು]=ಆರಂಭ;
            new_ಆರಂಭ+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            ಆರಂಭ=new_ಆರಂಭ;
        }
    }

    /* now adjust the index (stage 1) table */
    ಗಾಗಿ(i=0; i<trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ; ++i) {
        trie->ಸೂಚ್ಯಂಕ[i]=trie->map[ABS(trie->ಸೂಚ್ಯಂಕ[i])>>UTRIE_ಸರಿಸು];
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE_DEBUG
    /* we saved some space */
    ಬರೆ("compacting trie: count of 32-bit words %lu->%lu\n",
            (ದೀರ್ಘ)trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, (ದೀರ್ಘ)new_ಆರಂಭ);
#ಆದರೆ_ಅಂತ್ಯ

    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=new_ಆರಂಭ;
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
ಸ್ಥಿರ uint32_t U_CALLCONV
default_Get_Folded_ಮೌಲ್ಯ(UNewTrie *trie, UChar32 ಆರಂಭ, int32_t ಸ್ಥಾನಾಂತರ) {
    uint32_t value, initial_ಮೌಲ್ಯ;
    UChar32 ಎಲ್ಲೆ;
    UBool ಒಳ_ಖಂಡ_Zero;

    initial_ಮೌಲ್ಯ=trie->ದತ್ತಾಂಶ[0];
    ಎಲ್ಲೆ=ಆರಂಭ+0x400;
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ) {
        value=utrie_get32(trie, ಆರಂಭ, &ಒಳ_ಖಂಡ_Zero);
        ಆದರೆ(ಒಳ_ಖಂಡ_Zero) {
            ಆರಂಭ+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(value!=initial_ಮೌಲ್ಯ) {
            ಹಿಂತಿರುಗಿ static_cast<uint32_t>(ಸ್ಥಾನಾಂತರ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ++ಆರಂಭ;
        }
    }
    ಹಿಂತಿರುಗಿ 0;
}

U_CAPI int32_t U_EXPORT2
utrie_serialize(UNewTrie *trie, ಶೂನ್ಯ *dt, int32_t capacity,
                UNew_Trie_Get_Folded_ಮೌಲ್ಯ *get_Folded_ಮೌಲ್ಯ,
                UBool reduce_To16_ಬಿಟ್ಟುಗಳು,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UTrie_ಶೀರ್ಷಿಕೆ *ಶೀರ್ಷಿಕೆ;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    uint8_t* ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;

    /* argument check */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ || capacity<0 || (capacity>0 && dt==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(get_Folded_ಮೌಲ್ಯ==ಶೂನ್ಯಸೂಚಕ) {
        get_Folded_ಮೌಲ್ಯ=default_Get_Folded_ಮೌಲ್ಯ;
    }

    ದತ್ತಾಂಶ = (uint8_t*)dt;
    /* fold and compact if necessary, also checks that indexLength is within limits */
    ಆದರೆ(!trie->isCompacted) {
        /* compact once without overlap to improve folding */
        utrie_compact(trie, ಸುಳ್ಳು, p_Error_ಸಂಕೇತ);

        /* fold the supplementary part of the index array */
        utrie_fold(trie, get_Folded_ಮೌಲ್ಯ, p_Error_ಸಂಕೇತ);

        /* compact again with overlap for minimum data array length */
        utrie_compact(trie, ನಿಜ, p_Error_ಸಂಕೇತ);

        trie->isCompacted=ನಿಜ;
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    /* is dataLength within limits? */
    ಆದರೆ( (reduce_To16_ಬಿಟ್ಟುಗಳು ? (trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ+trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ) : trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) >= UTRIE_MAX_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
    }

    length=ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ)+2*trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ಆದರೆ(reduce_To16_ಬಿಟ್ಟುಗಳು) {
        length+=2*trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        length+=4*trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    }

    ಆದರೆ(length>capacity) {
        ಹಿಂತಿರುಗಿ length; /* preflighting */
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE_DEBUG
    ಬರೆ("**UTrieLengths(serialize)** index:%6ld  data:%6ld  serialized:%6ld\n",
           (ದೀರ್ಘ)trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, (ದೀರ್ಘ)trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, (ದೀರ್ಘ)length);
#ಆದರೆ_ಅಂತ್ಯ

    /* set the header fields */
    ಶೀರ್ಷಿಕೆ=(UTrie_ಶೀರ್ಷಿಕೆ *)ದತ್ತಾಂಶ;
    ದತ್ತಾಂಶ+=ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ);

    ಶೀರ್ಷಿಕೆ->signature=0x54726965; /* "Trie" */
    ಶೀರ್ಷಿಕೆ->options=UTRIE_ಸರಿಸು | (UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು<<UTRIE_OPTIONS_ಸೂಚ್ಯಂಕ_ಸರಿಸು);

    ಆದರೆ(!reduce_To16_ಬಿಟ್ಟುಗಳು) {
        ಶೀರ್ಷಿಕೆ->options|=UTRIE_OPTIONS_ದತ್ತಾಂಶ_IS_32_ಬಿಟ್;
    }
    ಆದರೆ(trie->is_Latin1_ರೇಖೀಯ) {
        ಶೀರ್ಷಿಕೆ->options|=UTRIE_OPTIONS_LATIN1_IS_ರೇಖೀಯ;
    }

    ಶೀರ್ಷಿಕೆ->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;

    /* write the index (stage 1) array and the 16/32-bit data (stage 2) array */
    ಆದರೆ(reduce_To16_ಬಿಟ್ಟುಗಳು) {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT, after adding indexLength */
        p=(uint32_t *)trie->ಸೂಚ್ಯಂಕ;
        dest16=(uint16_t *)ದತ್ತಾಂಶ;
        ಗಾಗಿ(i=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ; i>0; --i) {
            *dest16++=(uint16_t)((*p++ + trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ)>>UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
        }

        /* write 16-bit data values */
        p=trie->ದತ್ತಾಂಶ;
        ಗಾಗಿ(i=trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* write 16-bit index values shifted right by UTRIE_INDEX_SHIFT */
        p=(uint32_t *)trie->ಸೂಚ್ಯಂಕ;
        dest16=(uint16_t *)ದತ್ತಾಂಶ;
        ಗಾಗಿ(i=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ; i>0; --i) {
            *dest16++=(uint16_t)(*p++ >> UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
        }

        /* write 32-bit data values */
        uprv_memcpy(dest16, trie->ದತ್ತಾಂಶ, 4*(size_t)trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ);
    }

    ಹಿಂತಿರುಗಿ length;
}

/* inverse to defaultGetFoldedValue() */
U_CAPI int32_t U_EXPORT2
utrie_default_Get_Folding_ಸ್ಥಾನಾಂತರ(uint32_t ದತ್ತಾಂಶ) {
    ಹಿಂತಿರುಗಿ (int32_t)ದತ್ತಾಂಶ;
}

U_CAPI int32_t U_EXPORT2
utrie_unserialize(UTrie *trie, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, int32_t length, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UTrie_ಶೀರ್ಷಿಕೆ *ಶೀರ್ಷಿಕೆ;
    ಸ್ಥಿರಾಂಕ uint16_t *p16;
    uint32_t options;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ -1;
    }

    /* enough data for a trie header? */
    ಆದರೆ(length<(int32_t)ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ)) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ -1;
    }

    /* check the signature */
    ಶೀರ್ಷಿಕೆ=(ಸ್ಥಿರಾಂಕ UTrie_ಶೀರ್ಷಿಕೆ *)ದತ್ತಾಂಶ;
    ಆದರೆ(ಶೀರ್ಷಿಕೆ->signature!=0x54726965) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ -1;
    }

    /* get the options and check the shift values */
    options=ಶೀರ್ಷಿಕೆ->options;
    ಆದರೆ( (options&UTRIE_OPTIONS_ಸರಿಸು_ಮುಖವಾಡ)!=UTRIE_ಸರಿಸು ||
        ((options>>UTRIE_OPTIONS_ಸೂಚ್ಯಂಕ_ಸರಿಸು)&UTRIE_OPTIONS_ಸರಿಸು_ಮುಖವಾಡ)!=UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು
    ) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ -1;
    }
    trie->is_Latin1_ರೇಖೀಯ = (options & UTRIE_OPTIONS_LATIN1_IS_ರೇಖೀಯ) != 0;

    /* get the length values */
    trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=ಶೀರ್ಷಿಕೆ->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;

    length-=(int32_t)ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ);

    /* enough data for the index? */
    ಆದರೆ(length<2*trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ -1;
    }
    p16=(ಸ್ಥಿರಾಂಕ uint16_t *)(ಶೀರ್ಷಿಕೆ+1);
    trie->ಸೂಚ್ಯಂಕ=p16;
    p16+=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    length-=2*trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;

    /* get the data */
    ಆದರೆ(options&UTRIE_OPTIONS_ದತ್ತಾಂಶ_IS_32_ಬಿಟ್) {
        ಆದರೆ(length<4*trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
            *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
            ಹಿಂತಿರುಗಿ -1;
        }
        trie->data32=(ಸ್ಥಿರಾಂಕ uint32_t *)p16;
        trie->initial_ಮೌಲ್ಯ=trie->data32[0];
        length=(int32_t)ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ)+2*trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ+4*trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(length<2*trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
            *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
            ಹಿಂತಿರುಗಿ -1;
        }

        /* the "data16" data is used via the index pointer */
        trie->data32=ಶೂನ್ಯಸೂಚಕ;
        trie->initial_ಮೌಲ್ಯ=trie->ಸೂಚ್ಯಂಕ[trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ];
        length=(int32_t)ಗಾತ್ರ(UTrie_ಶೀರ್ಷಿಕೆ)+2*trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ+2*trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    }

    trie->get_Folding_ಸ್ಥಾನಾಂತರ=utrie_default_Get_Folding_ಸ್ಥಾನಾಂತರ;

    ಹಿಂತಿರುಗಿ length;
}

U_CAPI int32_t U_EXPORT2
utrie_unserializeDummy(UTrie *trie,
                       ಶೂನ್ಯ *ದತ್ತಾಂಶ, int32_t length,
                       uint32_t initial_ಮೌಲ್ಯ, uint32_t lead_Unit_ಮೌಲ್ಯ,
                       UBool make16_ಬಿಟ್_Trie,
                       UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    uint16_t *p16;
    int32_t actual_ಉದ್ದಳತೆ, latin1_ಉದ್ದಳತೆ, i, ಎಲ್ಲೆ;
    uint16_t ಖಂಡ;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ -1;
    }

    /* calculate the actual size of the dummy trie data */

    /* max(Latin-1, block 0) */
    latin1_ಉದ್ದಳತೆ= 256; /*UTRIE_SHIFT<=8 ? 256 : UTRIE_DATA_BLOCK_LENGTH;*/

    trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ+UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ;
    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=latin1_ಉದ್ದಳತೆ;
    ಆದರೆ(lead_Unit_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
        trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
    }

    actual_ಉದ್ದಳತೆ=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2;
    ಆದರೆ(make16_ಬಿಟ್_Trie) {
        actual_ಉದ್ದಳತೆ+=trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        actual_ಉದ್ದಳತೆ+=trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4;
    }

    /* enough space for the dummy trie? */
    ಆದರೆ(length<actual_ಉದ್ದಳತೆ) {
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ actual_ಉದ್ದಳತೆ;
    }

    trie->is_Latin1_ರೇಖೀಯ=ನಿಜ;
    trie->initial_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;

    /* fill the index and data arrays */
    p16=(uint16_t *)ದತ್ತಾಂಶ;
    trie->ಸೂಚ್ಯಂಕ=p16;

    ಆದರೆ(make16_ಬಿಟ್_Trie) {
        /* indexes to block 0 */
        ಖಂಡ=(uint16_t)(trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ>>UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
        ಎಲ್ಲೆ=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
        ಗಾಗಿ(i=0; i<ಎಲ್ಲೆ; ++i) {
            p16[i]=ಖಂಡ;
        }

        ಆದರೆ(lead_Unit_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            ಖಂಡ+=(uint16_t)(latin1_ಉದ್ದಳತೆ>>UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
            i=0xd800>>UTRIE_ಸರಿಸು;
            ಎಲ್ಲೆ=0xdc00>>UTRIE_ಸರಿಸು;
            ಗಾಗಿ(; i<ಎಲ್ಲೆ; ++i) {
                p16[i]=ಖಂಡ;
            }
        }

        trie->data32=ಶೂನ್ಯಸೂಚಕ;

        /* Latin-1 data */
        p16+=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
        ಗಾಗಿ(i=0; i<latin1_ಉದ್ದಳತೆ; ++i) {
            p16[i]=(uint16_t)initial_ಮೌಲ್ಯ;
        }

        /* data for lead surrogate code units */
        ಆದರೆ(lead_Unit_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
            ಎಲ್ಲೆ=latin1_ಉದ್ದಳತೆ+UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            ಗಾಗಿ(/* i=latin1Length */; i<ಎಲ್ಲೆ; ++i) {
                p16[i]=(uint16_t)lead_Unit_ಮೌಲ್ಯ;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        uint32_t *p32;

        /* indexes to block 0 */
        uprv_memset(p16, 0, trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2);

        ಆದರೆ(lead_Unit_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
            /* indexes for lead surrogate code units to the block after Latin-1 */
            ಖಂಡ=(uint16_t)(latin1_ಉದ್ದಳತೆ>>UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
            i=0xd800>>UTRIE_ಸರಿಸು;
            ಎಲ್ಲೆ=0xdc00>>UTRIE_ಸರಿಸು;
            ಗಾಗಿ(; i<ಎಲ್ಲೆ; ++i) {
                p16[i]=ಖಂಡ;
            }
        }

        trie->data32=p32=(uint32_t *)(p16+trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ);

        /* Latin-1 data */
        ಗಾಗಿ(i=0; i<latin1_ಉದ್ದಳತೆ; ++i) {
            p32[i]=initial_ಮೌಲ್ಯ;
        }

        /* data for lead surrogate code units */
        ಆದರೆ(lead_Unit_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
            ಎಲ್ಲೆ=latin1_ಉದ್ದಳತೆ+UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            ಗಾಗಿ(/* i=latin1Length */; i<ಎಲ್ಲೆ; ++i) {
                p32[i]=lead_Unit_ಮೌಲ್ಯ;
            }
        }
    }

    trie->get_Folding_ಸ್ಥಾನಾಂತರ=utrie_default_Get_Folding_ಸ್ಥಾನಾಂತರ;

    ಹಿಂತಿರುಗಿ actual_ಉದ್ದಳತೆ;
}

/* enumeration -------------------------------------------------------------- */

/* default UTrieEnumValue() returns the input value itself */
ಸ್ಥಿರ uint32_t U_CALLCONV
enum_Same_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * /*context*/, uint32_t value) {
    ಹಿಂತಿರುಗಿ value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie_enum(ಸ್ಥಿರಾಂಕ UTrie *trie,
           UTrie_Enum_ಮೌಲ್ಯ *enum_ಮೌಲ್ಯ, UTrieEnumRange *enumRange, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context) {
    ಸ್ಥಿರಾಂಕ uint32_t *data32;
    ಸ್ಥಿರಾಂಕ uint16_t *idx;

    uint32_t value, prev_ಮೌಲ್ಯ, initial_ಮೌಲ್ಯ;
    UChar32 c, prev;
    int32_t l, i, j, ಖಂಡ, prev_ಖಂಡ, null_ಖಂಡ, ಸ್ಥಾನಾಂತರ;

    /* check arguments */
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ || trie->ಸೂಚ್ಯಂಕ==ಶೂನ್ಯಸೂಚಕ || enumRange==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(enum_ಮೌಲ್ಯ==ಶೂನ್ಯಸೂಚಕ) {
        enum_ಮೌಲ್ಯ=enum_Same_ಮೌಲ್ಯ;
    }

    idx=trie->ಸೂಚ್ಯಂಕ;
    data32=trie->data32;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_ಮೌಲ್ಯ=enum_ಮೌಲ್ಯ(context, trie->initial_ಮೌಲ್ಯ);

    ಆದರೆ(data32==ಶೂನ್ಯಸೂಚಕ) {
        null_ಖಂಡ=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        null_ಖಂಡ=0;
    }

    /* set variables for previous range */
    prev_ಖಂಡ=null_ಖಂಡ;
    prev=0;
    prev_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;

    /* enumerate BMP - the main loop enumerates data blocks */
    ಗಾಗಿ(i=0, c=0; c<=0xffff; ++i) {
        ಆದರೆ(c==0xd800) {
            /* skip lead surrogate code _units_, go to lead surr. code _points_ */
            i=UTRIE_BMP_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xdc00) {
            /* go back to regular BMP code points */
            i=c>>UTRIE_ಸರಿಸು;
        }

        ಖಂಡ=idx[i]<<UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು;
        ಆದರೆ(ಖಂಡ==prev_ಖಂಡ) {
            /* the block is the same as the previous one, and filled with value */
            c+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಖಂಡ==null_ಖಂಡ) {
            /* this is the all-initial-value block */
            ಆದರೆ(prev_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                ಆದರೆ(prev<c) {
                    ಆದರೆ(!enumRange(context, prev, c, prev_ಮೌಲ್ಯ)) {
                        ಹಿಂತಿರುಗಿ;
                    }
                }
                prev_ಖಂಡ=null_ಖಂಡ;
                prev=c;
                prev_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
            }
            c+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            prev_ಖಂಡ=ಖಂಡ;
            ಗಾಗಿ(j=0; j<UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ; ++j) {
                value=enum_ಮೌಲ್ಯ(context, data32!=ಶೂನ್ಯಸೂಚಕ ? data32[ಖಂಡ+j] : idx[ಖಂಡ+j]);
                ಆದರೆ(value!=prev_ಮೌಲ್ಯ) {
                    ಆದರೆ(prev<c) {
                        ಆದರೆ(!enumRange(context, prev, c, prev_ಮೌಲ್ಯ)) {
                            ಹಿಂತಿರುಗಿ;
                        }
                    }
                    ಆದರೆ(j>0) {
                        /* the block is not filled with all the same value */
                        prev_ಖಂಡ=-1;
                    }
                    prev=c;
                    prev_ಮೌಲ್ಯ=value;
                }
                ++c;
            }
        }
    }

    /* enumerate supplementary code points */
    ಗಾಗಿ(l=0xd800; l<0xdc00;) {
        /* lead surrogate access */
        ಸ್ಥಾನಾಂತರ=idx[l>>UTRIE_ಸರಿಸು]<<UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು;
        ಆದರೆ(ಸ್ಥಾನಾಂತರ==null_ಖಂಡ) {
            /* no entries for a whole block of lead surrogates */
            ಆದರೆ(prev_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                ಆದರೆ(prev<c) {
                    ಆದರೆ(!enumRange(context, prev, c, prev_ಮೌಲ್ಯ)) {
                        ಹಿಂತಿರುಗಿ;
                    }
                }
                prev_ಖಂಡ=null_ಖಂಡ;
                prev=c;
                prev_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
            }

            l+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
            c+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ<<10;
            ಮುಂದುವರಿ;
        }

        value= data32!=ಶೂನ್ಯಸೂಚಕ ? data32[ಸ್ಥಾನಾಂತರ+(l&UTRIE_ಮುಖವಾಡ)] : idx[ಸ್ಥಾನಾಂತರ+(l&UTRIE_ಮುಖವಾಡ)];

        /* enumerate trail surrogates for this lead surrogate */
        ಸ್ಥಾನಾಂತರ=trie->get_Folding_ಸ್ಥಾನಾಂತರ(value);
        ಆದರೆ(ಸ್ಥಾನಾಂತರ<=0) {
            /* no data for this lead surrogate */
            ಆದರೆ(prev_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                ಆದರೆ(prev<c) {
                    ಆದರೆ(!enumRange(context, prev, c, prev_ಮೌಲ್ಯ)) {
                        ಹಿಂತಿರುಗಿ;
                    }
                }
                prev_ಖಂಡ=null_ಖಂಡ;
                prev=c;
                prev_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
            }

            /* nothing else to do for the supplementary code points for this lead surrogate */
            c+=0x400;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* enumerate code points for this lead surrogate */
            i=ಸ್ಥಾನಾಂತರ;
            ಸ್ಥಾನಾಂತರ+=UTRIE_SURROGATE_ಖಂಡ_ಎಣಿಕೆ;
            ಮಾಡು {
                /* copy of most of the body of the BMP loop */
                ಖಂಡ=idx[i]<<UTRIE_ಸೂಚ್ಯಂಕ_ಸರಿಸು;
                ಆದರೆ(ಖಂಡ==prev_ಖಂಡ) {
                    /* the block is the same as the previous one, and filled with value */
                    c+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಖಂಡ==null_ಖಂಡ) {
                    /* this is the all-initial-value block */
                    ಆದರೆ(prev_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                        ಆದರೆ(prev<c) {
                            ಆದರೆ(!enumRange(context, prev, c, prev_ಮೌಲ್ಯ)) {
                                ಹಿಂತಿರುಗಿ;
                            }
                        }
                        prev_ಖಂಡ=null_ಖಂಡ;
                        prev=c;
                        prev_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
                    }
                    c+=UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    prev_ಖಂಡ=ಖಂಡ;
                    ಗಾಗಿ(j=0; j<UTRIE_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ; ++j) {
                        value=enum_ಮೌಲ್ಯ(context, data32!=ಶೂನ್ಯಸೂಚಕ ? data32[ಖಂಡ+j] : idx[ಖಂಡ+j]);
                        ಆದರೆ(value!=prev_ಮೌಲ್ಯ) {
                            ಆದರೆ(prev<c) {
                                ಆದರೆ(!enumRange(context, prev, c, prev_ಮೌಲ್ಯ)) {
                                    ಹಿಂತಿರುಗಿ;
                                }
                            }
                            ಆದರೆ(j>0) {
                                /* the block is not filled with all the same value */
                                prev_ಖಂಡ=-1;
                            }
                            prev=c;
                            prev_ಮೌಲ್ಯ=value;
                        }
                        ++c;
                    }
                }
            } ಆಗ(++i<ಸ್ಥಾನಾಂತರ);
        }

        ++l;
    }

    /* deliver last range */
    enumRange(context, prev, c, prev_ಮೌಲ್ಯ);
}

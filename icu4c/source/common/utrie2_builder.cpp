// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2001-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  utrie2_builder.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2008sep26 (split off from utrie2.c)
*   created by: Markus W. Scherer
*
*   This is a common implementation of a Unicode trie.
*   It is a kind of compressed, serializable table of 16- or 32-bit values associated with
*   Unicode code points (0..0x10ffff).
*   This is the second common version of a Unicode trie (hence the name UTrie2).
*   See utrie2.h for a comparison.
*
*   This file contains only the builder code.
*   See utrie2.c for the runtime and enumeration code.
*/
// #define UTRIE2_DEBUG
#నిర్వచితమైతే UTRIE2_DEBUG
#   include <stdio.h>
#అయితే_ముగింపు
// #define UCPTRIE_DEBUG

#అన్తర్భావ "unicode/utypes.h"
#నిర్వచితమైతే UCPTRIE_DEBUG
#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/umutablecptrie.h"
#అన్తర్భావ "ucptrie_impl.h"
#అయితే_ముగింపు
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "utrie2.h"
#అన్తర్భావ "utrie2_impl.h"

#అన్తర్భావ "utrie.h"  // for utrie2_fromUTrie()

/* Implementation notes ----------------------------------------------------- */

/*
 * The UTRIE2_SHIFT_1, UTRIE2_SHIFT_2, UTRIE2_INDEX_SHIFT and other values
 * have been chosen to minimize trie sizes overall.
 * Most of the code is flexible enough to work with a range of values,
 * within certain limits.
 *
 * Exception: Support for separate values for lead surrogate code _units_
 * vs. code _points_ was added after the constants were fixed,
 * and has not been tested nor particularly designed for different constant values.
 * (Especially the utrie2_enum() code that jumps to the special LSCP index-2
 * part and back.)
 *
 * Requires UTRIE2_SHIFT_2<=6. Otherwise 0xc0 which is the top of the ASCII-linear data
 * including the bad-UTF-8-data block is not a multiple of UTRIE2_DATA_BLOCK_LENGTH
 * and map[block>>UTRIE2_SHIFT_2] (used in reference counting and compaction
 * remapping) stops working.
 *
 * Requires UTRIE2_SHIFT_1>=10 because utrie2_enumForLeadSurrogate()
 * assumes that a single index-2 block is used for 0x400 code points
 * corresponding to one lead surrogate.
 *
 * Requires UTRIE2_SHIFT_1<=16. Otherwise one single index-2 block contains
 * more than one Unicode plane, and the split of the index-2 table into a BMP
 * part and a supplementary part, with a gap in between, would not work.
 *
 * Requires UTRIE2_INDEX_SHIFT>=1 not because of the code but because
 * there is data with more than 64k distinct values,
 * for example for Unihan collation with a separate collation weight per
 * Han character.
 */

/* Building a trie ----------------------------------------------------------*/

గణన {
    /** The null index-2 block, following the gap in the index-2 table. */
    UNEWTRIE2_సూచీ_2_NULL_స్థానభ్రంశం=UNEWTRIE2_సూచీ_GAP_స్థానభ్రంశం+UNEWTRIE2_సూచీ_GAP_నిడివి,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_సూచీ_2_ప్రారంభం_స్థానభ్రంశం=UNEWTRIE2_సూచీ_2_NULL_స్థానభ్రంశం+UTRIE2_సూచీ_2_ఖండం_నిడివి,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_దత్తాంశం_NULL_స్థానభ్రంశం=UTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం,

    /** The start of allocated data blocks. */
    UNEWTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం=UNEWTRIE2_దత్తాంశం_NULL_స్థానభ్రంశం+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_దత్తాంశం_0800_స్థానభ్రంశం=UNEWTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం+0x780
};

/* Start with allocation of 16k data entries. */
#నిర్వచించు UNEWTRIE2_INITIAL_దత్తాంశం_నిడివి ((int32_t)1<<14)

/* Grow about 8x each time. */
#నిర్వచించు UNEWTRIE2_MEDIUM_దత్తాంశం_నిడివి ((int32_t)1<<17)

స్థిర int32_t
alloc_Index2_ఖండం(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_విలువ, uint32_t error_విలువ, UError_సంకేతం *p_Error_సంకేతం) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *దత్తాంశం;
    int32_t i, j;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }

    trie=(UTrie2 *)uprv_malloc(పరిమాణం(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(పరిమాణం(UNewTrie2));
    దత్తాంశం=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_దత్తాంశం_నిడివి*4);
    ఒకవేళ(trie==శూన్యసూచిక || newTrie==శూన్యసూచిక || దత్తాంశం==శూన్యసూచిక) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(దత్తాంశం);
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    uprv_memset(trie, 0, పరిమాణం(UTrie2));
    trie->initial_విలువ=initial_విలువ;
    trie->error_విలువ=error_విలువ;
    trie->ఎగువ_ప్రారంభం=0x110000;
    trie->newTrie=newTrie;
#నిర్వచితమైతే UTRIE2_DEBUG
    trie->name="open";
#అయితే_ముగింపు

    newTrie->దత్తాంశం=దత్తాంశం;
#నిర్వచితమైతే UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_విలువ, error_విలువ, p_Error_సంకేతం);
#అయితే_ముగింపు
    newTrie->దత్తాంశం_సామర్థ్యం=UNEWTRIE2_INITIAL_దత్తాంశం_నిడివి;
    newTrie->initial_విలువ=initial_విలువ;
    newTrie->error_విలువ=error_విలువ;
    newTrie->ఎగువ_ప్రారంభం=0x110000;
    newTrie->మొదటి_Free_ఖండం=0;  /* no free block in the list */
    newTrie->isCompacted=అబద్ధం;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    కొరకు(i=0; i<0x80; ++i) {
        newTrie->దత్తాంశం[i]=initial_విలువ;
    }
    కొరకు(; i<0xc0; ++i) {
        newTrie->దత్తాంశం[i]=error_విలువ;
    }
    కొరకు(i=UNEWTRIE2_దత్తాంశం_NULL_స్థానభ్రంశం; i<UNEWTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం; ++i) {
        newTrie->దత్తాంశం[i]=initial_విలువ;
    }
    newTrie->దత్తాంశం_Null_స్థానభ్రంశం=UNEWTRIE2_దత్తాంశం_NULL_స్థానభ్రంశం;
    newTrie->దత్తాంశం_నిడివి=UNEWTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    కొరకు(i=0, j=0; j<0x80; ++i, j+=UTRIE2_దత్తాంశం_ఖండం_నిడివి) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    కొరకు(; j<0xc0; ++i, j+=UTRIE2_దత్తాంశం_ఖండం_నిడివి) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_జరుపు_2)-
        (0x80>>UTRIE2_జరుపు_2)+
        1+
        UTRIE2_LSCP_సూచీ_2_నిడివి;
    j+=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
    కొరకు(; j<UNEWTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం; ++i, j+=UTRIE2_దత్తాంశం_ఖండం_నిడివి) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    కొరకు(i=0x80>>UTRIE2_జరుపు_2; i<UTRIE2_సూచీ_2_BMP_నిడివి; ++i) {
        newTrie->index2[i]=UNEWTRIE2_దత్తాంశం_NULL_స్థానభ్రంశం;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    కొరకు(i=0; i<UNEWTRIE2_సూచీ_GAP_నిడివి; ++i) {
        newTrie->index2[UNEWTRIE2_సూచీ_GAP_స్థానభ్రంశం+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    కొరకు(i=0; i<UTRIE2_సూచీ_2_ఖండం_నిడివి; ++i) {
        newTrie->index2[UNEWTRIE2_సూచీ_2_NULL_స్థానభ్రంశం+i]=UNEWTRIE2_దత్తాంశం_NULL_స్థానభ్రంశం;
    }
    newTrie->index2_Null_స్థానభ్రంశం=UNEWTRIE2_సూచీ_2_NULL_స్థానభ్రంశం;
    newTrie->index2_నిడివి=UNEWTRIE2_సూచీ_2_ప్రారంభం_స్థానభ్రంశం;

    /* set the index-1 indexes for the linear index-2 block */
    కొరకు(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_సూచీ_1_నిడివి;
        ++i, j+=UTRIE2_సూచీ_2_ఖండం_నిడివి
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    కొరకు(; i<UNEWTRIE2_సూచీ_1_నిడివి; ++i) {
        newTrie->index1[i]=UNEWTRIE2_సూచీ_2_NULL_స్థానభ్రంశం;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    కొరకు(i=0x80; i<0x800; i+=UTRIE2_దత్తాంశం_ఖండం_నిడివి) {
        utrie2_set32(trie, i, initial_విలువ, p_Error_సంకేతం);
    }

    తిరిగి trie;
}

స్థిర UNewTrie2 *
cloneBuilder(స్థిరరాశి UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(పరిమాణం(UNewTrie2)));
    ఒకవేళ(trie==శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }

    trie->దత్తాంశం = static_cast<uint32_t*>(uprv_malloc(other->దత్తాంశం_సామర్థ్యం * 4));
    ఒకవేళ(trie->దత్తాంశం==శూన్యసూచిక) {
        uprv_free(trie);
        తిరిగి శూన్యసూచిక;
    }
#నిర్వచితమైతే UCPTRIE_DEBUG
    ఒకవేళ(other->t3==శూన్యసూచిక) {
        trie->t3=శూన్యసూచిక;
    } లేకపోతే {
        UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &error_సంకేతం);
    }
#అయితే_ముగింపు
    trie->దత్తాంశం_సామర్థ్యం=other->దత్తాంశం_సామర్థ్యం;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, పరిమాణం(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_నిడివి*4);
    trie->index2_Null_స్థానభ్రంశం=other->index2_Null_స్థానభ్రంశం;
    trie->index2_నిడివి=other->index2_నిడివి;

    uprv_memcpy(trie->దత్తాంశం, other->దత్తాంశం, (size_t)other->దత్తాంశం_నిడివి*4);
    trie->దత్తాంశం_Null_స్థానభ్రంశం=other->దత్తాంశం_Null_స్థానభ్రంశం;
    trie->దత్తాంశం_నిడివి=other->దత్తాంశం_నిడివి;

    /* reference counters */
    ఒకవేళ(other->isCompacted) {
        trie->మొదటి_Free_ఖండం=0;
    } లేకపోతే {
        uprv_memcpy(trie->map, other->map, ((size_t)other->దత్తాంశం_నిడివి>>UTRIE2_జరుపు_2)*4);
        trie->మొదటి_Free_ఖండం=other->మొదటి_Free_ఖండం;
    }

    trie->initial_విలువ=other->initial_విలువ;
    trie->error_విలువ=other->error_విలువ;
    trie->ఎగువ_ప్రారంభం=other->ఎగువ_ప్రారంభం;
    trie->isCompacted=other->isCompacted;

    తిరిగి trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(స్థిరరాశి UTrie2 *other, UError_సంకేతం *p_Error_సంకేతం) {
    UTrie2 *trie;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(other==శూన్యసూచిక || (other->జ్ఞాపకం==శూన్యసూచిక && other->newTrie==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    trie=(UTrie2 *)uprv_malloc(పరిమాణం(UTrie2));
    ఒకవేళ(trie==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memcpy(trie, other, పరిమాణం(UTrie2));

    ఒకవేళ(other->జ్ఞాపకం!=శూన్యసూచిక) {
        trie->జ్ఞాపకం=uprv_malloc(other->length);
        ఒకవేళ(trie->జ్ఞాపకం!=శూన్యసూచిక) {
            trie->is_జ్ఞాపకం_Owned=నిజం;
            uprv_memcpy(trie->జ్ఞాపకం, other->జ్ఞాపకం, other->length);

            /* make the clone's pointers point to its own memory */
            trie->సూచీ=(uint16_t *)trie->జ్ఞాపకం+(other->సూచీ-(uint16_t *)other->జ్ఞాపకం);
            ఒకవేళ(other->data16!=శూన్యసూచిక) {
                trie->data16=(uint16_t *)trie->జ్ఞాపకం+(other->data16-(uint16_t *)other->జ్ఞాపకం);
            }
            ఒకవేళ(other->data32!=శూన్యసూచిక) {
                trie->data32=(uint32_t *)trie->జ్ఞాపకం+(other->data32-(uint32_t *)other->జ్ఞాపకం);
            }
        }
    } లేకపోతే /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    ఒకవేళ(trie->జ్ఞాపకం==శూన్యసూచిక && trie->newTrie==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        uprv_free(trie);
        trie=శూన్యసూచిక;
    }
    తిరిగి trie;
}

రకనిర్వచనం నిర్మాణం New_Trie_And_స్థితి {
    UTrie2 *trie;
    UError_సంకేతం error_సంకేతం;
    UBool exclusive_హద్దు;  /* rather than inclusive range end */
} New_Trie_And_స్థితి;

స్థిర UBool U_CALLCONV
copyEnumRange(స్థిరరాశి శూన్య *context, UChar32 ప్రారంభం, UChar32 చివర, uint32_t value) {
    New_Trie_And_స్థితి *nt=(New_Trie_And_స్థితి *)context;
    ఒకవేళ(value!=nt->trie->initial_విలువ) {
        ఒకవేళ(nt->exclusive_హద్దు) {
            --చివర;
        }
        ఒకవేళ(ప్రారంభం==చివర) {
            utrie2_set32(nt->trie, ప్రారంభం, value, &nt->error_సంకేతం);
        } లేకపోతే {
            utrie2_సమితి_Range32(nt->trie, ప్రారంభం, చివర, value, నిజం, &nt->error_సంకేతం);
        }
        తిరిగి U_SUCCESS(nt->error_సంకేతం);
    } లేకపోతే {
        తిరిగి నిజం;
    }
}

#నిర్వచితమైతే UTRIE2_DEBUG
స్థిర దీర్ఘ లెక్క_Initial(స్థిరరాశి UTrie2 *trie) {
    uint32_t initial_విలువ=trie->initial_విలువ;
    int32_t length=trie->దత్తాంశం_నిడివి;
    దీర్ఘ count=0;
    ఒకవేళ(trie->data16!=శూన్యసూచిక) {
        కొరకు(int32_t i=0; i<length; ++i) {
            ఒకవేళ(trie->data16[i]==initial_విలువ) { ++count; }
        }
    } లేకపోతే {
        కొరకు(int32_t i=0; i<length; ++i) {
            ఒకవేళ(trie->data32[i]==initial_విలువ) { ++count; }
        }
    }
    తిరిగి count;
}

స్థిర శూన్య
utrie_printLengths(స్థిరరాశి UTrie *trie) {
    దీర్ఘ సూచీ_నిడివి=trie->సూచీ_నిడివి;
    దీర్ఘ దత్తాంశం_నిడివి=(దీర్ఘ)trie->దత్తాంశం_నిడివి;
    దీర్ఘ మొత్తం_నిడివి=(దీర్ఘ)పరిమాణం(UTrie_శీర్షిక)+సూచీ_నిడివి*2+దత్తాంశం_నిడివి*(trie->data32!=శూన్యసూచిక ? 4 : 2);
    రాయి("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           సూచీ_నిడివి, దత్తాంశం_నిడివి, మొత్తం_నిడివి);
}

స్థిర శూన్య
utrie2_printLengths(స్థిరరాశి UTrie2 *trie, స్థిరరాశి అక్షరం *which) {
    దీర్ఘ సూచీ_నిడివి=trie->సూచీ_నిడివి;
    దీర్ఘ దత్తాంశం_నిడివి=(దీర్ఘ)trie->దత్తాంశం_నిడివి;
    దీర్ఘ మొత్తం_నిడివి=(దీర్ఘ)పరిమాణం(UTrie2_శీర్షిక)+సూచీ_నిడివి*2+దత్తాంశం_నిడివి*(trie->data32!=శూన్యసూచిక ? 4 : 2);
    రాయి("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, సూచీ_నిడివి, దత్తాంశం_నిడివి, లెక్క_Initial(trie), మొత్తం_నిడివి);
}
#అయితే_ముగింపు

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(స్థిరరాశి UTrie2 *other, UError_సంకేతం *p_Error_సంకేతం) {
    New_Trie_And_స్థితి context;
    char16_t lead;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(other==శూన్యసూచిక || (other->జ్ఞాపకం==శూన్యసూచిక && other->newTrie==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(other->newTrie!=శూన్యసూచిక && !other->newTrie->isCompacted) {
        తిరిగి utrie2_clone(other, p_Error_సంకేతం);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_విలువ, other->error_విలువ, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    context.exclusive_హద్దు=అబద్ధం;
    context.error_సంకేతం=*p_Error_సంకేతం;
    utrie2_enum(other, శూన్యసూచిక, copyEnumRange, &context);
    *p_Error_సంకేతం=context.error_సంకేతం;
    కొరకు(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        ఒకవేళ(other->data32==శూన్యసూచిక) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } లేకపోతే {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        ఒకవేళ(value!=other->initial_విలువ) {
            utrie2_set32_For_Lead_Surrogate_సంకేతం_Unit(context.trie, lead, value, p_Error_సంకేతం);
        }
    }
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        utrie2_close(context.trie);
        context.trie=శూన్యసూచిక;
    }
    తిరిగి context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(స్థిరరాశి UTrie *trie1, uint32_t error_విలువ, UError_సంకేతం *p_Error_సంకేతం) {
    New_Trie_And_స్థితి context;
    char16_t lead;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(trie1==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    context.trie=utrie2_open(trie1->initial_విలువ, error_విలువ, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    context.exclusive_హద్దు=నిజం;
    context.error_సంకేతం=*p_Error_సంకేతం;
    utrie_enum(trie1, శూన్యసూచిక, copyEnumRange, &context);
    *p_Error_సంకేతం=context.error_సంకేతం;
    కొరకు(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        ఒకవేళ(trie1->data32==శూన్యసూచిక) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } లేకపోతే {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        ఒకవేళ(value!=trie1->initial_విలువ) {
            utrie2_set32_For_Lead_Surrogate_సంకేతం_Unit(context.trie, lead, value, p_Error_సంకేతం);
        }
    }
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=శూన్యసూచిక ? UTRIE2_32_విలువ_బిట్లు : UTRIE2_16_విలువ_బిట్లు,
                      p_Error_సంకేతం);
    }
#నిర్వచితమైతే UTRIE2_DEBUG
    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#అయితే_ముగింపు
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        utrie2_close(context.trie);
        context.trie=శూన్యసూచిక;
    }
    తిరిగి context.trie;
}

స్థిర అంతఃవరుస UBool
is_లోపల_Null_ఖండం(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, ఖండం;

    ఒకవేళ(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_సూచీ_2_స్థానభ్రంశం-(0xd800>>UTRIE2_జరుపు_2))+
            (c>>UTRIE2_జరుపు_2);
    } లేకపోతే {
        i2=trie->index1[c>>UTRIE2_జరుపు_1]+
            ((c>>UTRIE2_జరుపు_2)&UTRIE2_సూచీ_2_ముసుగు);
    }
    ఖండం=trie->index2[i2];
    తిరిగి ఖండం == trie->దత్తాంశం_Null_స్థానభ్రంశం;
}

స్థిర int32_t
alloc_Index2_ఖండం(UNewTrie2 *trie) {
    int32_t new_ఖండం, new_శిఖరం;

    new_ఖండం=trie->index2_నిడివి;
    new_శిఖరం=new_ఖండం+UTRIE2_సూచీ_2_ఖండం_నిడివి;
    ఒకవేళ(new_శిఖరం>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        తిరిగి -1;
    }
    trie->index2_నిడివి=new_శిఖరం;
    uprv_memcpy(trie->index2+new_ఖండం, trie->index2+trie->index2_Null_స్థానభ్రంశం, UTRIE2_సూచీ_2_ఖండం_నిడివి*4);
    తిరిగి new_ఖండం;
}

స్థిర int32_t
get_Index2_ఖండం(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    ఒకవేళ(U_IS_LEAD(c) && forLSCP) {
        తిరిగి UTRIE2_LSCP_సూచీ_2_స్థానభ్రంశం;
    }

    i1=c>>UTRIE2_జరుపు_1;
    i2=trie->index1[i1];
    ఒకవేళ(i2==trie->index2_Null_స్థానభ్రంశం) {
        i2=alloc_Index2_ఖండం(trie);
        ఒకవేళ(i2<0) {
            తిరిగి -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    తిరిగి i2;
}

స్థిర int32_t
alloc_దత్తాంశం_ఖండం(UNewTrie2 *trie, int32_t copy_ఖండం) {
    int32_t new_ఖండం, new_శిఖరం;

    ఒకవేళ(trie->మొదటి_Free_ఖండం!=0) {
        /* get the first free block */
        new_ఖండం=trie->మొదటి_Free_ఖండం;
        trie->మొదటి_Free_ఖండం=-trie->map[new_ఖండం>>UTRIE2_జరుపు_2];
    } లేకపోతే {
        /* get a new block from the high end */
        new_ఖండం=trie->దత్తాంశం_నిడివి;
        new_శిఖరం=new_ఖండం+UTRIE2_దత్తాంశం_ఖండం_నిడివి;
        ఒకవేళ(new_శిఖరం>trie->దత్తాంశం_సామర్థ్యం) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *దత్తాంశం;

            ఒకవేళ(trie->దత్తాంశం_సామర్థ్యం<UNEWTRIE2_MEDIUM_దత్తాంశం_నిడివి) {
                capacity=UNEWTRIE2_MEDIUM_దత్తాంశం_నిడివి;
            } లేకపోతే ఒకవేళ(trie->దత్తాంశం_సామర్థ్యం<UNEWTRIE2_MAX_దత్తాంశం_నిడివి) {
                capacity=UNEWTRIE2_MAX_దత్తాంశం_నిడివి;
            } లేకపోతే {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                తిరిగి -1;
            }
            దత్తాంశం = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            ఒకవేళ(దత్తాంశం==శూన్యసూచిక) {
                తిరిగి -1;
            }
            uprv_memcpy(దత్తాంశం, trie->దత్తాంశం, (size_t)trie->దత్తాంశం_నిడివి*4);
            uprv_free(trie->దత్తాంశం);
            trie->దత్తాంశం=దత్తాంశం;
            trie->దత్తాంశం_సామర్థ్యం=capacity;
        }
        trie->దత్తాంశం_నిడివి=new_శిఖరం;
    }
    uprv_memcpy(trie->దత్తాంశం+new_ఖండం, trie->దత్తాంశం+copy_ఖండం, UTRIE2_దత్తాంశం_ఖండం_నిడివి*4);
    trie->map[new_ఖండం>>UTRIE2_జరుపు_2]=0;
    తిరిగి new_ఖండం;
}

/* call when the block's reference counter reaches 0 */
స్థిర శూన్య
release_దత్తాంశం_ఖండం(UNewTrie2 *trie, int32_t ఖండం) {
    /* put this block at the front of the free-block chain */
    trie->map[ఖండం>>UTRIE2_జరుపు_2]=-trie->మొదటి_Free_ఖండం;
    trie->మొదటి_Free_ఖండం=ఖండం;
}

స్థిర అంతఃవరుస UBool
is_Writable_ఖండం(UNewTrie2 *trie, int32_t ఖండం) {
    తిరిగి ఖండం != trie->దత్తాంశం_Null_స్థానభ్రంశం && 1 == trie->map[ఖండం >> UTRIE2_జరుపు_2];
}

స్థిర అంతఃవరుస శూన్య
సమితి_Index2_నమోదు(UNewTrie2 *trie, int32_t i2, int32_t ఖండం) {
    int32_t old_ఖండం;
    ++trie->map[ఖండం>>UTRIE2_జరుపు_2];  /* increment first, in case block==oldBlock! */
    old_ఖండం=trie->index2[i2];
    ఒకవేళ(0 == --trie->map[old_ఖండం>>UTRIE2_జరుపు_2]) {
        release_దత్తాంశం_ఖండం(trie, old_ఖండం);
    }
    trie->index2[i2]=ఖండం;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
స్థిర int32_t
get_దత్తాంశం_ఖండం(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, old_ఖండం, new_ఖండం;

    i2=get_Index2_ఖండం(trie, c, forLSCP);
    ఒకవేళ(i2<0) {
        తిరిగి -1;  /* program error */
    }

    i2+=(c>>UTRIE2_జరుపు_2)&UTRIE2_సూచీ_2_ముసుగు;
    old_ఖండం=trie->index2[i2];
    ఒకవేళ(is_Writable_ఖండం(trie, old_ఖండం)) {
        తిరిగి old_ఖండం;
    }

    /* allocate a new data block */
    new_ఖండం=alloc_దత్తాంశం_ఖండం(trie, old_ఖండం);
    ఒకవేళ(new_ఖండం<0) {
        /* out of memory in the data array */
        తిరిగి -1;
    }
    సమితి_Index2_నమోదు(trie, i2, new_ఖండం);
    తిరిగి new_ఖండం;
}

/**
 * @return true if the value was successfully set
 */
స్థిర శూన్య
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UError_సంకేతం *p_Error_సంకేతం) {
    int32_t ఖండం;

    ఒకవేళ(trie==శూన్యసూచిక || trie->isCompacted) {
        *p_Error_సంకేతం=U_NO_WRITE_PERMISSION;
        తిరిగి;
    }
#నిర్వచితమైతే UCPTRIE_DEBUG
    umutablecptrie_సమితి(trie->t3, c, value, p_Error_సంకేతం);
#అయితే_ముగింపు

    ఖండం=get_దత్తాంశం_ఖండం(trie, c, forLSCP);
    ఒకవేళ(ఖండం<0) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    trie->దత్తాంశం[ఖండం+(c&UTRIE2_దత్తాంశం_ముసుగు)]=value;
}

U_CAPI శూన్య U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ((uint32_t)c>0x10ffff) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    set32(trie->newTrie, c, నిజం, value, p_Error_సంకేతం);
}

U_CAPI శూన్య U_EXPORT2
utrie2_set32_For_Lead_Surrogate_సంకేతం_Unit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ(!U_IS_LEAD(c)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    set32(trie->newTrie, c, అబద్ధం, value, p_Error_సంకేతం);
}

స్థిర శూన్య
write_ఖండం(uint32_t *ఖండం, uint32_t value) {
    uint32_t *హద్దు=ఖండం+UTRIE2_దత్తాంశం_ఖండం_నిడివి;
    అయితే(ఖండం<హద్దు) {
        *ఖండం++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
స్థిర శూన్య
fill_ఖండం(uint32_t *ఖండం, UChar32 ప్రారంభం, UChar32 హద్దు,
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

U_CAPI శూన్య U_EXPORT2
utrie2_సమితి_Range32(UTrie2 *trie,
                  UChar32 ప్రారంభం, UChar32 చివర,
                  uint32_t value, UBool overwrite,
                  UError_సంకేతం *p_Error_సంకేతం) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t ఖండం, rest, repeat_ఖండం;
    UChar32 హద్దు;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ((uint32_t)ప్రారంభం>0x10ffff || (uint32_t)చివర>0x10ffff || ప్రారంభం>చివర) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    newTrie=trie->newTrie;
    ఒకవేళ(newTrie==శూన్యసూచిక || newTrie->isCompacted) {
        *p_Error_సంకేతం=U_NO_WRITE_PERMISSION;
        తిరిగి;
    }
#నిర్వచితమైతే UCPTRIE_DEBUG
    umutablecptrie_సమితి_Range(newTrie->t3, ప్రారంభం, చివర, value, p_Error_సంకేతం);
#అయితే_ముగింపు
    ఒకవేళ(!overwrite && value==newTrie->initial_విలువ) {
        తిరిగి; /* nothing to do */
    }

    హద్దు=చివర+1;
    ఒకవేళ(ప్రారంభం&UTRIE2_దత్తాంశం_ముసుగు) {
        UChar32 తదుపరి_ప్రారంభం;

        /* set partial block at [start..following block boundary[ */
        ఖండం=get_దత్తాంశం_ఖండం(newTrie, ప్రారంభం, నిజం);
        ఒకవేళ(ఖండం<0) {
            *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }

        తదుపరి_ప్రారంభం=(ప్రారంభం+UTRIE2_దత్తాంశం_ముసుగు)&~UTRIE2_దత్తాంశం_ముసుగు;
        ఒకవేళ(తదుపరి_ప్రారంభం<=హద్దు) {
            fill_ఖండం(newTrie->దత్తాంశం+ఖండం, ప్రారంభం&UTRIE2_దత్తాంశం_ముసుగు, UTRIE2_దత్తాంశం_ఖండం_నిడివి,
                      value, newTrie->initial_విలువ, overwrite);
            ప్రారంభం=తదుపరి_ప్రారంభం;
        } లేకపోతే {
            fill_ఖండం(newTrie->దత్తాంశం+ఖండం, ప్రారంభం&UTRIE2_దత్తాంశం_ముసుగు, హద్దు&UTRIE2_దత్తాంశం_ముసుగు,
                      value, newTrie->initial_విలువ, overwrite);
            తిరిగి;
        }
    }

    /* number of positions in the last, partial block */
    rest=హద్దు&UTRIE2_దత్తాంశం_ముసుగు;

    /* round down limit to a block boundary */
    హద్దు&=~UTRIE2_దత్తాంశం_ముసుగు;

    /* iterate over all-value blocks */
    ఒకవేళ(value==newTrie->initial_విలువ) {
        repeat_ఖండం=newTrie->దత్తాంశం_Null_స్థానభ్రంశం;
    } లేకపోతే {
        repeat_ఖండం=-1;
    }

    అయితే(ప్రారంభం<హద్దు) {
        int32_t i2;
        UBool సమితి_Repeat_ఖండం=అబద్ధం;

        ఒకవేళ(value==newTrie->initial_విలువ && is_లోపల_Null_ఖండం(newTrie, ప్రారంభం, నిజం)) {
            ప్రారంభం+=UTRIE2_దత్తాంశం_ఖండం_నిడివి; /* nothing to do */
            కొనసాగు;
        }

        /* get index value */
        i2=get_Index2_ఖండం(newTrie, ప్రారంభం, నిజం);
        ఒకవేళ(i2<0) {
            *p_Error_సంకేతం=U_అంతర్గత_కార్యక్రమం_ERROR;
            తిరిగి;
        }
        i2+=(ప్రారంభం>>UTRIE2_జరుపు_2)&UTRIE2_సూచీ_2_ముసుగు;
        ఖండం=newTrie->index2[i2];
        ఒకవేళ(is_Writable_ఖండం(newTrie, ఖండం)) {
            /* already allocated */
            ఒకవేళ(overwrite && ఖండం>=UNEWTRIE2_దత్తాంశం_0800_స్థానభ్రంశం) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                సమితి_Repeat_ఖండం=నిజం;
            } లేకపోతే {
                /* !overwrite, or protected block: just write the values into this block */
                fill_ఖండం(newTrie->దత్తాంశం+ఖండం,
                          0, UTRIE2_దత్తాంశం_ఖండం_నిడివి,
                          value, newTrie->initial_విలువ, overwrite);
            }
        } లేకపోతే ఒకవేళ(newTrie->దత్తాంశం[ఖండం]!=value && (overwrite || ఖండం==newTrie->దత్తాంశం_Null_స్థానభ్రంశం)) {
            /*
             * Set the repeatBlock instead of the null block or previous repeat block:
             *
             * If !isWritableBlock() then all entries in the block have the same value
             * because it's the null block or a range block (the repeatBlock from a previous
             * call to utrie2_setRange32()).
             * No other blocks are used multiple times before compacting.
             *
             * The null block is the only non-writable block with the initialValue because
             * of the repeatBlock initialization above. (If value==initialValue, then
             * the repeatBlock will be the null data block.)
             *
             * We set our repeatBlock if the desired value differs from the block's value,
             * and if we overwrite any data or if the data is all initial values
             * (which is the same as the block being the null block, see above).
             */
            సమితి_Repeat_ఖండం=నిజం;
        }
        ఒకవేళ(సమితి_Repeat_ఖండం) {
            ఒకవేళ(repeat_ఖండం>=0) {
                సమితి_Index2_నమోదు(newTrie, i2, repeat_ఖండం);
            } లేకపోతే {
                /* create and set and fill the repeatBlock */
                repeat_ఖండం=get_దత్తాంశం_ఖండం(newTrie, ప్రారంభం, నిజం);
                ఒకవేళ(repeat_ఖండం<0) {
                    *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    తిరిగి;
                }
                write_ఖండం(newTrie->దత్తాంశం+repeat_ఖండం, value);
            }
        }

        ప్రారంభం+=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
    }

    ఒకవేళ(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        ఖండం=get_దత్తాంశం_ఖండం(newTrie, ప్రారంభం, నిజం);
        ఒకవేళ(ఖండం<0) {
            *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }

        fill_ఖండం(newTrie->దత్తాంశం+ఖండం, 0, rest, value, newTrie->initial_విలువ, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

స్థిర అంతఃవరుస UBool
equal_int32(స్థిరరాశి int32_t *s, స్థిరరాశి int32_t *t, int32_t length) {
    అయితే(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    తిరిగి length == 0;
}

స్థిర అంతఃవరుస UBool
equal_uint32(స్థిరరాశి uint32_t *s, స్థిరరాశి uint32_t *t, int32_t length) {
    అయితే(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    తిరిగి length == 0;
}

స్థిర int32_t
కనుగొను_Same_Index2_ఖండం(స్థిరరాశి int32_t *idx, int32_t index2_నిడివి, int32_t other_ఖండం) {
    int32_t ఖండం;

    /* ensure that we do not even partially get past index2Length */
    index2_నిడివి-=UTRIE2_సూచీ_2_ఖండం_నిడివి;

    కొరకు(ఖండం=0; ఖండం<=index2_నిడివి; ++ఖండం) {
        ఒకవేళ(equal_int32(idx+ఖండం, idx+other_ఖండం, UTRIE2_సూచీ_2_ఖండం_నిడివి)) {
            తిరిగి ఖండం;
        }
    }
    తిరిగి -1;
}

స్థిర int32_t
కనుగొను_Same_దత్తాంశం_ఖండం_2(స్థిరరాశి uint32_t *దత్తాంశం, int32_t దత్తాంశం_నిడివి, int32_t other_ఖండం, int32_t ఖండం_నిడివి) {
    int32_t ఖండం;

    /* ensure that we do not even partially get past dataLength */
    దత్తాంశం_నిడివి-=ఖండం_నిడివి;

    కొరకు(ఖండం=0; ఖండం<=దత్తాంశం_నిడివి; ఖండం+=UTRIE2_దత్తాంశం_GRANULARITY) {
        ఒకవేళ(equal_uint32(దత్తాంశం+ఖండం, దత్తాంశం+other_ఖండం, ఖండం_నిడివి)) {
            తిరిగి ఖండం;
        }
    }
    తిరిగి -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
స్థిర UChar32
కనుగొను_ఎగువ_ప్రారంభం(UNewTrie2 *trie, uint32_t ఎగువ_విలువ) {
    స్థిరరాశి uint32_t *data32;

    uint32_t value, initial_విలువ;
    UChar32 c, prev;
    int32_t i1, i2, j, i2_ఖండం, prev_I2_ఖండం, index2_Null_స్థానభ్రంశం, ఖండం, prev_ఖండం, null_ఖండం;

    data32=trie->దత్తాంశం;
    initial_విలువ=trie->initial_విలువ;

    index2_Null_స్థానభ్రంశం=trie->index2_Null_స్థానభ్రంశం;
    null_ఖండం=trie->దత్తాంశం_Null_స్థానభ్రంశం;

    /* set variables for previous range */
    ఒకవేళ(ఎగువ_విలువ==initial_విలువ) {
        prev_I2_ఖండం=index2_Null_స్థానభ్రంశం;
        prev_ఖండం=null_ఖండం;
    } లేకపోతే {
        prev_I2_ఖండం=-1;
        prev_ఖండం=-1;
    }
    prev=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_సూచీ_1_నిడివి;
    c=prev;
    అయితే(c>0) {
        i2_ఖండం=trie->index1[--i1];
        ఒకవేళ(i2_ఖండం==prev_I2_ఖండం) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_చొప్పున_సూచీ_1_నమోదు;
            కొనసాగు;
        }
        prev_I2_ఖండం=i2_ఖండం;
        ఒకవేళ(i2_ఖండం==index2_Null_స్థానభ్రంశం) {
            /* this is the null index-2 block */
            ఒకవేళ(ఎగువ_విలువ!=initial_విలువ) {
                తిరిగి c;
            }
            c-=UTRIE2_CP_చొప్పున_సూచీ_1_నమోదు;
        } లేకపోతే {
            /* enumerate data blocks for one index-2 block */
            కొరకు(i2=UTRIE2_సూచీ_2_ఖండం_నిడివి; i2>0;) {
                ఖండం=trie->index2[i2_ఖండం+ --i2];
                ఒకవేళ(ఖండం==prev_ఖండం) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
                    కొనసాగు;
                }
                prev_ఖండం=ఖండం;
                ఒకవేళ(ఖండం==null_ఖండం) {
                    /* this is the null data block */
                    ఒకవేళ(ఎగువ_విలువ!=initial_విలువ) {
                        తిరిగి c;
                    }
                    c-=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
                } లేకపోతే {
                    కొరకు(j=UTRIE2_దత్తాంశం_ఖండం_నిడివి; j>0;) {
                        value=data32[ఖండం+ --j];
                        ఒకవేళ(value!=ఎగువ_విలువ) {
                            తిరిగి c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    తిరిగి 0;
}

/*
 * Compact a build-time trie.
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
compact_దత్తాంశం(UNewTrie2 *trie) {
#నిర్వచితమైతే UTRIE2_DEBUG
    int32_t లెక్క_Same=0, కూడిక_Overlaps=0;
#అయితే_ముగింపు

    int32_t ప్రారంభం, new_ప్రారంభం, moved_ప్రారంభం;
    int32_t ఖండం_నిడివి, overlap;
    int32_t i, map_సూచీ, ఖండం_లెక్క;

    /* do not compact linear-ASCII data */
    new_ప్రారంభం=UTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం;
    కొరకు(ప్రారంభం=0, i=0; ప్రారంభం<new_ప్రారంభం; ప్రారంభం+=UTRIE2_దత్తాంశం_ఖండం_నిడివి, ++i) {
        trie->map[i]=ప్రారంభం;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    ఖండం_నిడివి=64;
    ఖండం_లెక్క=ఖండం_నిడివి>>UTRIE2_జరుపు_2;
    కొరకు(ప్రారంభం=new_ప్రారంభం; ప్రారంభం<trie->దత్తాంశం_నిడివి;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        ఒకవేళ(ప్రారంభం==UNEWTRIE2_దత్తాంశం_0800_స్థానభ్రంశం) {
            ఖండం_నిడివి=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
            ఖండం_లెక్క=1;
        }

        /* skip blocks that are not used */
        ఒకవేళ(trie->map[ప్రారంభం>>UTRIE2_జరుపు_2]<=0) {
            /* advance start to the next block */
            ప్రారంభం+=ఖండం_నిడివి;

            /* leave newStart with the previous block! */
            కొనసాగు;
        }

        /* search for an identical block */
        ఒకవేళ( (moved_ప్రారంభం=కనుగొను_Same_దత్తాంశం_ఖండం_2(trie->దత్తాంశం, new_ప్రారంభం, ప్రారంభం, ఖండం_నిడివి))
             >=0
        ) {
#నిర్వచితమైతే UTRIE2_DEBUG
            ++లెక్క_Same;
#అయితే_ముగింపు
            /* found an identical block, set the other block's index value for the current block */
            కొరకు(i=ఖండం_లెక్క, map_సూచీ=ప్రారంభం>>UTRIE2_జరుపు_2; i>0; --i) {
                trie->map[map_సూచీ++]=moved_ప్రారంభం;
                moved_ప్రారంభం+=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
            }

            /* advance start to the next block */
            ప్రారంభం+=ఖండం_నిడివి;

            /* leave newStart with the previous block! */
            కొనసాగు;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        కొరకు(overlap=ఖండం_నిడివి-UTRIE2_దత్తాంశం_GRANULARITY;
            overlap>0 && !equal_uint32(trie->దత్తాంశం+(new_ప్రారంభం-overlap), trie->దత్తాంశం+ప్రారంభం, overlap);
            overlap-=UTRIE2_దత్తాంశం_GRANULARITY) {}

#నిర్వచితమైతే UTRIE2_DEBUG
            కూడిక_Overlaps+=overlap;
#అయితే_ముగింపు
        ఒకవేళ(overlap>0 || new_ప్రారంభం<ప్రారంభం) {
            /* some overlap, or just move the whole block */
            moved_ప్రారంభం=new_ప్రారంభం-overlap;
            కొరకు(i=ఖండం_లెక్క, map_సూచీ=ప్రారంభం>>UTRIE2_జరుపు_2; i>0; --i) {
                trie->map[map_సూచీ++]=moved_ప్రారంభం;
                moved_ప్రారంభం+=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
            }

            /* move the non-overlapping indexes to their new positions */
            ప్రారంభం+=overlap;
            కొరకు(i=ఖండం_నిడివి-overlap; i>0; --i) {
                trie->దత్తాంశం[new_ప్రారంభం++]=trie->దత్తాంశం[ప్రారంభం++];
            }
        } లేకపోతే /* no overlap && newStart==start */ {
            కొరకు(i=ఖండం_లెక్క, map_సూచీ=ప్రారంభం>>UTRIE2_జరుపు_2; i>0; --i) {
                trie->map[map_సూచీ++]=ప్రారంభం;
                ప్రారంభం+=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
            }
            new_ప్రారంభం=ప్రారంభం;
        }
    }

    /* now adjust the index-2 table */
    కొరకు(i=0; i<trie->index2_నిడివి; ++i) {
        ఒకవేళ(i==UNEWTRIE2_సూచీ_GAP_స్థానభ్రంశం) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_సూచీ_GAP_నిడివి;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_జరుపు_2];
    }
    trie->దత్తాంశం_Null_స్థానభ్రంశం=trie->map[trie->దత్తాంశం_Null_స్థానభ్రంశం>>UTRIE2_జరుపు_2];

    /* ensure dataLength alignment */
    అయితే((new_ప్రారంభం&(UTRIE2_దత్తాంశం_GRANULARITY-1))!=0) {
        trie->దత్తాంశం[new_ప్రారంభం++]=trie->initial_విలువ;
    }

#నిర్వచితమైతే UTRIE2_DEBUG
    /* we saved some space */
    రాయి("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (దీర్ఘ)trie->దత్తాంశం_నిడివి, (దీర్ఘ)new_ప్రారంభం, (దీర్ఘ)లెక్క_Same, (దీర్ఘ)కూడిక_Overlaps);
#అయితే_ముగింపు

    trie->దత్తాంశం_నిడివి=new_ప్రారంభం;
}

స్థిర శూన్య
compactIndex2(UNewTrie2 *trie) {
    int32_t i, ప్రారంభం, new_ప్రారంభం, moved_ప్రారంభం, overlap;

    /* do not compact linear-BMP index-2 blocks */
    new_ప్రారంభం=UTRIE2_సూచీ_2_BMP_నిడివి;
    కొరకు(ప్రారంభం=0, i=0; ప్రారంభం<new_ప్రారంభం; ప్రారంభం+=UTRIE2_సూచీ_2_ఖండం_నిడివి, ++i) {
        trie->map[i]=ప్రారంభం;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    new_ప్రారంభం+=UTRIE2_UTF8_2_B_సూచీ_2_నిడివి+((trie->ఎగువ_ప్రారంభం-0x10000)>>UTRIE2_జరుపు_1);

    కొరకు(ప్రారంభం=UNEWTRIE2_సూచీ_2_NULL_స్థానభ్రంశం; ప్రారంభం<trie->index2_నిడివి;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        ఒకవేళ( (moved_ప్రారంభం=కనుగొను_Same_Index2_ఖండం(trie->index2, new_ప్రారంభం, ప్రారంభం))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[ప్రారంభం>>UTRIE2_జరుపు_1_2]=moved_ప్రారంభం;

            /* advance start to the next block */
            ప్రారంభం+=UTRIE2_సూచీ_2_ఖండం_నిడివి;

            /* leave newStart with the previous block! */
            కొనసాగు;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        కొరకు(overlap=UTRIE2_సూచీ_2_ఖండం_నిడివి-1;
            overlap>0 && !equal_int32(trie->index2+(new_ప్రారంభం-overlap), trie->index2+ప్రారంభం, overlap);
            --overlap) {}

        ఒకవేళ(overlap>0 || new_ప్రారంభం<ప్రారంభం) {
            /* some overlap, or just move the whole block */
            trie->map[ప్రారంభం>>UTRIE2_జరుపు_1_2]=new_ప్రారంభం-overlap;

            /* move the non-overlapping indexes to their new positions */
            ప్రారంభం+=overlap;
            కొరకు(i=UTRIE2_సూచీ_2_ఖండం_నిడివి-overlap; i>0; --i) {
                trie->index2[new_ప్రారంభం++]=trie->index2[ప్రారంభం++];
            }
        } లేకపోతే /* no overlap && newStart==start */ {
            trie->map[ప్రారంభం>>UTRIE2_జరుపు_1_2]=ప్రారంభం;
            ప్రారంభం+=UTRIE2_సూచీ_2_ఖండం_నిడివి;
            new_ప్రారంభం=ప్రారంభం;
        }
    }

    /* now adjust the index-1 table */
    కొరకు(i=0; i<UNEWTRIE2_సూచీ_1_నిడివి; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_జరుపు_1_2];
    }
    trie->index2_Null_స్థానభ్రంశం=trie->map[trie->index2_Null_స్థానభ్రంశం>>UTRIE2_జరుపు_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    అయితే((new_ప్రారంభం&((UTRIE2_దత్తాంశం_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[new_ప్రారంభం++] = static_cast<int32_t>(0xffff) << UTRIE2_సూచీ_జరుపు;
    }

#నిర్వచితమైతే UTRIE2_DEBUG
    /* we saved some space */
    రాయి("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (దీర్ఘ)trie->index2_నిడివి, (దీర్ఘ)new_ప్రారంభం);
#అయితే_ముగింపు

    trie->index2_నిడివి=new_ప్రారంభం;
}

స్థిర శూన్య
compactTrie(UTrie2 *trie, UError_సంకేతం *p_Error_సంకేతం) {
    UNewTrie2 *newTrie;
    UChar32 ఎగువ_ప్రారంభం, supp_ఎగువ_ప్రారంభం;
    uint32_t ఎగువ_విలువ;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    ఎగువ_విలువ=utrie2_get32(trie, 0x10ffff);
    ఎగువ_ప్రారంభం=కనుగొను_ఎగువ_ప్రారంభం(newTrie, ఎగువ_విలువ);
    ఎగువ_ప్రారంభం=(ఎగువ_ప్రారంభం+(UTRIE2_CP_చొప్పున_సూచీ_1_నమోదు-1))&~(UTRIE2_CP_చొప్పున_సూచీ_1_నమోదు-1);
    ఒకవేళ(ఎగువ_ప్రారంభం==0x110000) {
        ఎగువ_విలువ=trie->error_విలువ;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->ఎగువ_ప్రారంభం=newTrie->ఎగువ_ప్రారంభం=ఎగువ_ప్రారంభం;

#నిర్వచితమైతే UTRIE2_DEBUG
    రాయి("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (దీర్ఘ)ఎగువ_ప్రారంభం, (దీర్ఘ)ఎగువ_విలువ, (దీర్ఘ)trie->initial_విలువ);
#అయితే_ముగింపు

    ఒకవేళ(ఎగువ_ప్రారంభం<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_ఎగువ_ప్రారంభం= ఎగువ_ప్రారంభం<=0x10000 ? 0x10000 : ఎగువ_ప్రారంభం;
        utrie2_సమితి_Range32(trie, supp_ఎగువ_ప్రారంభం, 0x10ffff, trie->initial_విలువ, నిజం, p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి;
        }
    }

    compact_దత్తాంశం(newTrie);
    ఒకవేళ(ఎగువ_ప్రారంభం>0x10000) {
        compactIndex2(newTrie);
#నిర్వచితమైతే UTRIE2_DEBUG
    } లేకపోతే {
        రాయి("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (దీర్ఘ)ఎగువ_ప్రారంభం, (దీర్ఘ)trie->newTrie->index2_నిడివి, (దీర్ఘ)UTRIE2_సూచీ_1_స్థానభ్రంశం);
#అయితే_ముగింపు
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->దత్తాంశం[newTrie->దత్తాంశం_నిడివి++]=ఎగువ_విలువ;
    అయితే((newTrie->దత్తాంశం_నిడివి&(UTRIE2_దత్తాంశం_GRANULARITY-1))!=0) {
        newTrie->దత్తాంశం[newTrie->దత్తాంశం_నిడివి++]=trie->initial_విలువ;
    }

    newTrie->isCompacted=నిజం;
}

/* serialization ------------------------------------------------------------ */

/**
 * Maximum length of the runtime index array.
 * Limited by its own 16-bit index values, and by uint16_t UTrie2Header.indexLength.
 * (The actual maximum length is lower,
 * (0x110000>>UTRIE2_SHIFT_2)+UTRIE2_UTF8_2B_INDEX_2_LENGTH+UTRIE2_MAX_INDEX_1_LENGTH.)
 */
#నిర్వచించు UTRIE2_MAX_సూచీ_నిడివి 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#నిర్వచించు UTRIE2_MAX_దత్తాంశం_నిడివి (0xffff<<UTRIE2_సూచీ_జరుపు)

/* Compact and internally serialize the trie. */
U_CAPI శూన్య U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_విలువ_బిట్లు విలువ_బిట్లు, UError_సంకేతం *p_Error_సంకేతం) {
    UNewTrie2 *newTrie;
    UTrie2_శీర్షిక *శీర్షిక;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_నిడివి;
    int32_t దత్తాంశం_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 ఎగువ_ప్రారంభం;

    /* argument check */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ( trie==శూన్యసూచిక ||
        విలువ_బిట్లు<0 || UTRIE2_లెక్క_విలువ_బిట్లు<=విలువ_బిట్లు
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    newTrie=trie->newTrie;
    ఒకవేళ(newTrie==శూన్యసూచిక) {
        /* already frozen */
        UTrie2_విలువ_బిట్లు frozen_విలువ_బిట్లు=
            trie->data16!=శూన్యసూచిక ? UTRIE2_16_విలువ_బిట్లు : UTRIE2_32_విలువ_బిట్లు;
        ఒకవేళ(విలువ_బిట్లు!=frozen_విలువ_బిట్లు) {
            *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        }
        తిరిగి;
    }

    /* compact if necessary */
    ఒకవేళ(!newTrie->isCompacted) {
        compactTrie(trie, p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి;
        }
    }
    ఎగువ_ప్రారంభం=trie->ఎగువ_ప్రారంభం;

    ఒకవేళ(ఎగువ_ప్రారంభం<=0x10000) {
        all_Indexes_నిడివి=UTRIE2_సూచీ_1_స్థానభ్రంశం;
    } లేకపోతే {
        all_Indexes_నిడివి=newTrie->index2_నిడివి;
    }
    ఒకవేళ(విలువ_బిట్లు==UTRIE2_16_విలువ_బిట్లు) {
        దత్తాంశం_Move=all_Indexes_నిడివి;
    } లేకపోతే {
        దత్తాంశం_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    ఒకవేళ( /* for unshifted indexLength */
        all_Indexes_నిడివి>UTRIE2_MAX_సూచీ_నిడివి ||
        /* for unshifted dataNullOffset */
        (దత్తాంశం_Move+newTrie->దత్తాంశం_Null_స్థానభ్రంశం)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (దత్తాంశం_Move+UNEWTRIE2_దత్తాంశం_0800_స్థానభ్రంశం)>0xffff ||
        /* for shiftedDataLength */
        (దత్తాంశం_Move+newTrie->దత్తాంశం_నిడివి)>UTRIE2_MAX_దత్తాంశం_నిడివి
    ) {
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి;
    }

    /* calculate the total serialized length */
    length=పరిమాణం(UTrie2_శీర్షిక)+all_Indexes_నిడివి*2;
    ఒకవేళ(విలువ_బిట్లు==UTRIE2_16_విలువ_బిట్లు) {
        length+=newTrie->దత్తాంశం_నిడివి*2;
    } లేకపోతే {
        length+=newTrie->దత్తాంశం_నిడివి*4;
    }

    trie->జ్ఞాపకం=uprv_malloc(length);
    ఒకవేళ(trie->జ్ఞాపకం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    trie->length=length;
    trie->is_జ్ఞాపకం_Owned=నిజం;

    trie->సూచీ_నిడివి=all_Indexes_నిడివి;
    trie->దత్తాంశం_నిడివి=newTrie->దత్తాంశం_నిడివి;
    ఒకవేళ(ఎగువ_ప్రారంభం<=0x10000) {
        trie->index2_Null_స్థానభ్రంశం=0xffff;
    } లేకపోతే {
        trie->index2_Null_స్థానభ్రంశం=static_cast<uint16_t>(UTRIE2_సూచీ_2_స్థానభ్రంశం+newTrie->index2_Null_స్థానభ్రంశం);
    }
    trie->దత్తాంశం_Null_స్థానభ్రంశం=(uint16_t)(దత్తాంశం_Move+newTrie->దత్తాంశం_Null_స్థానభ్రంశం);
    trie->ఎగువ_విలువ_సూచీ=దత్తాంశం_Move+trie->దత్తాంశం_నిడివి-UTRIE2_దత్తాంశం_GRANULARITY;

    /* set the header fields */
    శీర్షిక=(UTrie2_శీర్షిక *)trie->జ్ఞాపకం;

    శీర్షిక->signature=UTRIE2_SIG; /* "Tri2" */
    శీర్షిక->options=(uint16_t)విలువ_బిట్లు;

    శీర్షిక->సూచీ_నిడివి=(uint16_t)trie->సూచీ_నిడివి;
    శీర్షిక->shifted_దత్తాంశం_నిడివి=(uint16_t)(trie->దత్తాంశం_నిడివి>>UTRIE2_సూచీ_జరుపు);
    శీర్షిక->index2_Null_స్థానభ్రంశం=trie->index2_Null_స్థానభ్రంశం;
    శీర్షిక->దత్తాంశం_Null_స్థానభ్రంశం=trie->దత్తాంశం_Null_స్థానభ్రంశం;
    శీర్షిక->shifted_ఎగువ_ప్రారంభం=(uint16_t)(ఎగువ_ప్రారంభం>>UTRIE2_జరుపు_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(శీర్షిక+1);
    trie->సూచీ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    కొరకు(i=UTRIE2_సూచీ_2_BMP_నిడివి; i>0; --i) {
        *dest16++=(uint16_t)((దత్తాంశం_Move + *p++)>>UTRIE2_సూచీ_జరుపు);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    కొరకు(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(దత్తాంశం_Move+UTRIE2_BAD_UTF8_దత్తాంశం_స్థానభ్రంశం);
    }
    కొరకు(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(దత్తాంశం_Move+newTrie->index2[i<<(6-UTRIE2_జరుపు_2)]);
    }

    ఒకవేళ(ఎగువ_ప్రారంభం>0x10000) {
        int32_t index1_నిడివి=(ఎగువ_ప్రారంభం-0x10000)>>UTRIE2_జరుపు_1;
        int32_t index2_స్థానభ్రంశం=UTRIE2_సూచీ_2_BMP_నిడివి+UTRIE2_UTF8_2_B_సూచీ_2_నిడివి+index1_నిడివి;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_సూచీ_1_నిడివి;
        కొరకు(i=index1_నిడివి; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_సూచీ_2_స్థానభ్రంశం + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2_స్థానభ్రంశం;
        కొరకు(i=newTrie->index2_నిడివి-index2_స్థానభ్రంశం; i>0; --i) {
            *dest16++=(uint16_t)((దత్తాంశం_Move + *p++)>>UTRIE2_సూచీ_జరుపు);
        }
    }

    /* write the 16/32-bit data array */
    ఎంపిక(విలువ_బిట్లు) {
    స్థితి UTRIE2_16_విలువ_బిట్లు:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=శూన్యసూచిక;
        p=newTrie->దత్తాంశం;
        కొరకు(i=newTrie->దత్తాంశం_నిడివి; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        ఆపు;
    స్థితి UTRIE2_32_విలువ_బిట్లు:
        /* write 32-bit data values */
        trie->data16=శూన్యసూచిక;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->దత్తాంశం, (size_t)newTrie->దత్తాంశం_నిడివి*4);
        ఆపు;
    అప్రమేయ:
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

#నిర్వచితమైతే UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#అయితే_ముగింపు

#నిర్వచితమైతే UCPTRIE_DEBUG
    umutablecptrie_సమితి_Name(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_విలువ_వెడల్పు)విలువ_బిట్లు, p_Error_సంకేతం));
#అయితే_ముగింపు
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->దత్తాంశం);
    uprv_free(newTrie);
    trie->newTrie=శూన్యసూచిక;
}

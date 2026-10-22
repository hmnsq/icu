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
#यदि_परिभाषितम् UTRIE2_DEBUG
#   include <stdio.h>
#यदि_अन्तः
// #define UCPTRIE_DEBUG

#अन्तर्भाव "unicode/utypes.h"
#यदि_परिभाषितम् UCPTRIE_DEBUG
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/umutablecptrie.h"
#अन्तर्भाव "ucptrie_impl.h"
#यदि_अन्तः
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "utrie2.h"
#अन्तर्भाव "utrie2_impl.h"

#अन्तर्भाव "utrie.h"  // for utrie2_fromUTrie()

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

गणना {
    /** The null index-2 block, following the gap in the index-2 table. */
    UNEWTRIE2_सूचकांक_2_NULL_विस्थापनम्=UNEWTRIE2_सूचकांक_GAP_विस्थापनम्+UNEWTRIE2_सूचकांक_GAP_दीर्घता,

    /** The start of allocated index-2 blocks. */
    UNEWTRIE2_सूचकांक_2_START_विस्थापनम्=UNEWTRIE2_सूचकांक_2_NULL_विस्थापनम्+UTRIE2_सूचकांक_2_खण्डम्_दीर्घता,

    /**
     * The null data block.
     * Length 64=0x40 even if UTRIE2_DATA_BLOCK_LENGTH is smaller,
     * to work with 6-bit trail bytes from 2-byte UTF-8.
     */
    UNEWTRIE2_दत्त_NULL_विस्थापनम्=UTRIE2_दत्त_START_विस्थापनम्,

    /** The start of allocated data blocks. */
    UNEWTRIE2_दत्त_START_विस्थापनम्=UNEWTRIE2_दत्त_NULL_विस्थापनम्+0x40,

    /**
     * The start of data blocks for U+0800 and above.
     * Below, compaction uses a block length of 64 for 2-byte UTF-8.
     * From here on, compaction uses UTRIE2_DATA_BLOCK_LENGTH.
     * Data values for 0x780 code points beyond ASCII.
     */
    UNEWTRIE2_दत्त_0800_विस्थापनम्=UNEWTRIE2_दत्त_START_विस्थापनम्+0x780
};

/* Start with allocation of 16k data entries. */
#परिभाषय UNEWTRIE2_INITIAL_दत्त_दीर्घता ((int32_t)1<<14)

/* Grow about 8x each time. */
#परिभाषय UNEWTRIE2_MEDIUM_दत्त_दीर्घता ((int32_t)1<<17)

स्थिर int32_t
alloc_Index2_खण्डम्(UNewTrie2 *trie);

U_CAPI UTrie2 * U_EXPORT2
utrie2_open(uint32_t initial_मानम्, uint32_t error_मानम्, UError_संहिता *p_Error_संहिता) {
    UTrie2 *trie;
    UNewTrie2 *newTrie;
    uint32_t *दत्त;
    int32_t i, j;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }

    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    newTrie=(UNewTrie2 *)uprv_malloc(परिमाण(UNewTrie2));
    दत्त=(uint32_t *)uprv_malloc(UNEWTRIE2_INITIAL_दत्त_दीर्घता*4);
    यदि(trie==शून्यसूचकः || newTrie==शून्यसूचकः || दत्त==शून्यसूचकः) {
        uprv_free(trie);
        uprv_free(newTrie);
        uprv_free(दत्त);
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    uprv_memset(trie, 0, परिमाण(UTrie2));
    trie->initial_मानम्=initial_मानम्;
    trie->error_मानम्=error_मानम्;
    trie->उच्चः_Start=0x110000;
    trie->newTrie=newTrie;
#यदि_परिभाषितम् UTRIE2_DEBUG
    trie->name="open";
#यदि_अन्तः

    newTrie->दत्त=दत्त;
#यदि_परिभाषितम् UCPTRIE_DEBUG
    newTrie->t3=umutablecptrie_open(initial_मानम्, error_मानम्, p_Error_संहिता);
#यदि_अन्तः
    newTrie->दत्त_क्षमता=UNEWTRIE2_INITIAL_दत्त_दीर्घता;
    newTrie->initial_मानम्=initial_मानम्;
    newTrie->error_मानम्=error_मानम्;
    newTrie->उच्चः_Start=0x110000;
    newTrie->first_Free_खण्डम्=0;  /* no free block in the list */
    newTrie->isCompacted=असत्य;

    /*
     * preallocate and reset
     * - ASCII
     * - the bad-UTF-8-data block
     * - the null data block
     */
    प्रति(i=0; i<0x80; ++i) {
        newTrie->दत्त[i]=initial_मानम्;
    }
    प्रति(; i<0xc0; ++i) {
        newTrie->दत्त[i]=error_मानम्;
    }
    प्रति(i=UNEWTRIE2_दत्त_NULL_विस्थापनम्; i<UNEWTRIE2_दत्त_START_विस्थापनम्; ++i) {
        newTrie->दत्त[i]=initial_मानम्;
    }
    newTrie->दत्त_Null_विस्थापनम्=UNEWTRIE2_दत्त_NULL_विस्थापनम्;
    newTrie->दत्त_दीर्घता=UNEWTRIE2_दत्त_START_विस्थापनम्;

    /* set the index-2 indexes for the 2=0x80>>UTRIE2_SHIFT_2 ASCII data blocks */
    प्रति(i=0, j=0; j<0x80; ++i, j+=UTRIE2_दत्त_खण्डम्_दीर्घता) {
        newTrie->index2[i]=j;
        newTrie->map[i]=1;
    }
    /* reference counts for the bad-UTF-8-data block */
    प्रति(; j<0xc0; ++i, j+=UTRIE2_दत्त_खण्डम्_दीर्घता) {
        newTrie->map[i]=0;
    }
    /*
     * Reference counts for the null data block: all blocks except for the ASCII blocks.
     * Plus 1 so that we don't drop this block during compaction.
     * Plus as many as needed for lead surrogate code points.
     */
    /* i==newTrie->dataNullOffset */
    newTrie->map[i++]=
        (0x110000>>UTRIE2_सरणम्_2)-
        (0x80>>UTRIE2_सरणम्_2)+
        1+
        UTRIE2_LSCP_सूचकांक_2_दीर्घता;
    j+=UTRIE2_दत्त_खण्डम्_दीर्घता;
    प्रति(; j<UNEWTRIE2_दत्त_START_विस्थापनम्; ++i, j+=UTRIE2_दत्त_खण्डम्_दीर्घता) {
        newTrie->map[i]=0;
    }

    /*
     * set the remaining indexes in the BMP index-2 block
     * to the null data block
     */
    प्रति(i=0x80>>UTRIE2_सरणम्_2; i<UTRIE2_सूचकांक_2_BMP_दीर्घता; ++i) {
        newTrie->index2[i]=UNEWTRIE2_दत्त_NULL_विस्थापनम्;
    }

    /*
     * Fill the index gap with impossible values so that compaction
     * does not overlap other index-2 blocks with the gap.
     */
    प्रति(i=0; i<UNEWTRIE2_सूचकांक_GAP_दीर्घता; ++i) {
        newTrie->index2[UNEWTRIE2_सूचकांक_GAP_विस्थापनम्+i]=-1;
    }

    /* set the indexes in the null index-2 block */
    प्रति(i=0; i<UTRIE2_सूचकांक_2_खण्डम्_दीर्घता; ++i) {
        newTrie->index2[UNEWTRIE2_सूचकांक_2_NULL_विस्थापनम्+i]=UNEWTRIE2_दत्त_NULL_विस्थापनम्;
    }
    newTrie->index2_Null_विस्थापनम्=UNEWTRIE2_सूचकांक_2_NULL_विस्थापनम्;
    newTrie->index2_दीर्घता=UNEWTRIE2_सूचकांक_2_START_विस्थापनम्;

    /* set the index-1 indexes for the linear index-2 block */
    प्रति(i=0, j=0;
        i<UTRIE2_OMITTED_BMP_सूचकांक_1_दीर्घता;
        ++i, j+=UTRIE2_सूचकांक_2_खण्डम्_दीर्घता
    ) {
        newTrie->index1[i]=j;
    }

    /* set the remaining index-1 indexes to the null index-2 block */
    प्रति(; i<UNEWTRIE2_सूचकांक_1_दीर्घता; ++i) {
        newTrie->index1[i]=UNEWTRIE2_सूचकांक_2_NULL_विस्थापनम्;
    }

    /*
     * Preallocate and reset data for U+0080..U+07ff,
     * for 2-byte UTF-8 which will be compacted in 64-blocks
     * even if UTRIE2_DATA_BLOCK_LENGTH is smaller.
     */
    प्रति(i=0x80; i<0x800; i+=UTRIE2_दत्त_खण्डम्_दीर्घता) {
        utrie2_set32(trie, i, initial_मानम्, p_Error_संहिता);
    }

    प्रत्यागम trie;
}

स्थिर UNewTrie2 *
cloneBuilder(ध्रुव UNewTrie2 *other) {
    UNewTrie2 *trie;

    trie = static_cast<UNewTrie2*>(uprv_malloc(परिमाण(UNewTrie2)));
    यदि(trie==शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }

    trie->दत्त = static_cast<uint32_t*>(uprv_malloc(other->दत्त_क्षमता * 4));
    यदि(trie->दत्त==शून्यसूचकः) {
        uprv_free(trie);
        प्रत्यागम शून्यसूचकः;
    }
#यदि_परिभाषितम् UCPTRIE_DEBUG
    यदि(other->t3==शून्यसूचकः) {
        trie->t3=शून्यसूचकः;
    } अन्यथा {
        UError_संहिता error_संहिता=U_ZERO_ERROR;
        trie->t3=umutablecptrie_clone(other->t3, &error_संहिता);
    }
#यदि_अन्तः
    trie->दत्त_क्षमता=other->दत्त_क्षमता;

    /* clone data */
    uprv_memcpy(trie->index1, other->index1, परिमाण(trie->index1));
    uprv_memcpy(trie->index2, other->index2, (size_t)other->index2_दीर्घता*4);
    trie->index2_Null_विस्थापनम्=other->index2_Null_विस्थापनम्;
    trie->index2_दीर्घता=other->index2_दीर्घता;

    uprv_memcpy(trie->दत्त, other->दत्त, (size_t)other->दत्त_दीर्घता*4);
    trie->दत्त_Null_विस्थापनम्=other->दत्त_Null_विस्थापनम्;
    trie->दत्त_दीर्घता=other->दत्त_दीर्घता;

    /* reference counters */
    यदि(other->isCompacted) {
        trie->first_Free_खण्डम्=0;
    } अन्यथा {
        uprv_memcpy(trie->map, other->map, ((size_t)other->दत्त_दीर्घता>>UTRIE2_सरणम्_2)*4);
        trie->first_Free_खण्डम्=other->first_Free_खण्डम्;
    }

    trie->initial_मानम्=other->initial_मानम्;
    trie->error_मानम्=other->error_मानम्;
    trie->उच्चः_Start=other->उच्चः_Start;
    trie->isCompacted=other->isCompacted;

    प्रत्यागम trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_clone(ध्रुव UTrie2 *other, UError_संहिता *p_Error_संहिता) {
    UTrie2 *trie;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(other==शून्यसूचकः || (other->स्मृतिः==शून्यसूचकः && other->newTrie==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    यदि(trie==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memcpy(trie, other, परिमाण(UTrie2));

    यदि(other->स्मृतिः!=शून्यसूचकः) {
        trie->स्मृतिः=uprv_malloc(other->length);
        यदि(trie->स्मृतिः!=शून्यसूचकः) {
            trie->is_स्मृतिः_Owned=सत्य;
            uprv_memcpy(trie->स्मृतिः, other->स्मृतिः, other->length);

            /* make the clone's pointers point to its own memory */
            trie->सूचकांक=(uint16_t *)trie->स्मृतिः+(other->सूचकांक-(uint16_t *)other->स्मृतिः);
            यदि(other->data16!=शून्यसूचकः) {
                trie->data16=(uint16_t *)trie->स्मृतिः+(other->data16-(uint16_t *)other->स्मृतिः);
            }
            यदि(other->data32!=शून्यसूचकः) {
                trie->data32=(uint32_t *)trie->स्मृतिः+(other->data32-(uint32_t *)other->स्मृतिः);
            }
        }
    } अन्यथा /* other->newTrie!=nullptr */ {
        trie->newTrie=cloneBuilder(other->newTrie);
    }

    यदि(trie->स्मृतिः==शून्यसूचकः && trie->newTrie==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        uprv_free(trie);
        trie=शून्यसूचकः;
    }
    प्रत्यागम trie;
}

प्रकारनिर्देश संरचना New_Trie_And_स्थितिः {
    UTrie2 *trie;
    UError_संहिता error_संहिता;
    UBool exclusive_सीमा;  /* rather than inclusive range end */
} New_Trie_And_स्थितिः;

स्थिर UBool U_CALLCONV
copyEnumRange(ध्रुव शून्य *context, UChar32 start, UChar32 अन्तः, uint32_t value) {
    New_Trie_And_स्थितिः *nt=(New_Trie_And_स्थितिः *)context;
    यदि(value!=nt->trie->initial_मानम्) {
        यदि(nt->exclusive_सीमा) {
            --अन्तः;
        }
        यदि(start==अन्तः) {
            utrie2_set32(nt->trie, start, value, &nt->error_संहिता);
        } अन्यथा {
            utrie2_समुच्चयः_Range32(nt->trie, start, अन्तः, value, सत्य, &nt->error_संहिता);
        }
        प्रत्यागम U_SUCCESS(nt->error_संहिता);
    } अन्यथा {
        प्रत्यागम सत्य;
    }
}

#यदि_परिभाषितम् UTRIE2_DEBUG
स्थिर दीर्घ गणनम्_Initial(ध्रुव UTrie2 *trie) {
    uint32_t initial_मानम्=trie->initial_मानम्;
    int32_t length=trie->दत्त_दीर्घता;
    दीर्घ count=0;
    यदि(trie->data16!=शून्यसूचकः) {
        प्रति(int32_t i=0; i<length; ++i) {
            यदि(trie->data16[i]==initial_मानम्) { ++count; }
        }
    } अन्यथा {
        प्रति(int32_t i=0; i<length; ++i) {
            यदि(trie->data32[i]==initial_मानम्) { ++count; }
        }
    }
    प्रत्यागम count;
}

स्थिर शून्य
utrie_printLengths(ध्रुव UTrie *trie) {
    दीर्घ सूचकांक_दीर्घता=trie->सूचकांक_दीर्घता;
    दीर्घ दत्त_दीर्घता=(दीर्घ)trie->दत्त_दीर्घता;
    दीर्घ योग_दीर्घता=(दीर्घ)परिमाण(UTrie_शीर्षकम्)+सूचकांक_दीर्घता*2+दत्त_दीर्घता*(trie->data32!=शून्यसूचकः ? 4 : 2);
    प्रारूप_लिख("**UTrieLengths** index:%6ld  data:%6ld  serialized:%6ld\n",
           सूचकांक_दीर्घता, दत्त_दीर्घता, योग_दीर्घता);
}

स्थिर शून्य
utrie2_printLengths(ध्रुव UTrie2 *trie, ध्रुव वर्ण *which) {
    दीर्घ सूचकांक_दीर्घता=trie->सूचकांक_दीर्घता;
    दीर्घ दत्त_दीर्घता=(दीर्घ)trie->दत्त_दीर्घता;
    दीर्घ योग_दीर्घता=(दीर्घ)परिमाण(UTrie2_शीर्षकम्)+सूचकांक_दीर्घता*2+दत्त_दीर्घता*(trie->data32!=शून्यसूचकः ? 4 : 2);
    प्रारूप_लिख("**UTrie2Lengths(%s %s)** index:%6ld  data:%6ld  countInitial:%6ld  serialized:%6ld\n",
           which, trie->name, सूचकांक_दीर्घता, दत्त_दीर्घता, गणनम्_Initial(trie), योग_दीर्घता);
}
#यदि_अन्तः

U_CAPI UTrie2 * U_EXPORT2
utrie2_cloneAsThawed(ध्रुव UTrie2 *other, UError_संहिता *p_Error_संहिता) {
    New_Trie_And_स्थितिः context;
    char16_t lead;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(other==शून्यसूचकः || (other->स्मृतिः==शून्यसूचकः && other->newTrie==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    यदि(other->newTrie!=शून्यसूचकः && !other->newTrie->isCompacted) {
        प्रत्यागम utrie2_clone(other, p_Error_संहिता);  /* clone an unfrozen trie */
    }

    /* Clone the frozen trie by enumerating it and building a new one. */
    context.trie=utrie2_open(other->initial_मानम्, other->error_मानम्, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    context.exclusive_सीमा=असत्य;
    context.error_संहिता=*p_Error_संहिता;
    utrie2_enum(other, शून्यसूचकः, copyEnumRange, &context);
    *p_Error_संहिता=context.error_संहिता;
    प्रति(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        यदि(other->data32==शून्यसूचकः) {
            value=UTRIE2_GET16_FROM_U16_SINGLE_LEAD(other, lead);
        } अन्यथा {
            value=UTRIE2_GET32_FROM_U16_SINGLE_LEAD(other, lead);
        }
        यदि(value!=other->initial_मानम्) {
            utrie2_set32_For_Lead_Surrogate_संहिता_Unit(context.trie, lead, value, p_Error_संहिता);
        }
    }
    यदि(U_FAILURE(*p_Error_संहिता)) {
        utrie2_close(context.trie);
        context.trie=शून्यसूचकः;
    }
    प्रत्यागम context.trie;
}

/* Almost the same as utrie2_cloneAsThawed() but copies a UTrie and freezes the clone. */
U_CAPI UTrie2 * U_EXPORT2
utrie2_fromUTrie(ध्रुव UTrie *trie1, uint32_t error_मानम्, UError_संहिता *p_Error_संहिता) {
    New_Trie_And_स्थितिः context;
    char16_t lead;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(trie1==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    context.trie=utrie2_open(trie1->initial_मानम्, error_मानम्, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    context.exclusive_सीमा=सत्य;
    context.error_संहिता=*p_Error_संहिता;
    utrie_enum(trie1, शून्यसूचकः, copyEnumRange, &context);
    *p_Error_संहिता=context.error_संहिता;
    प्रति(lead=0xd800; lead<0xdc00; ++lead) {
        uint32_t value;
        यदि(trie1->data32==शून्यसूचकः) {
            value=UTRIE_GET16_FROM_LEAD(trie1, lead);
        } अन्यथा {
            value=UTRIE_GET32_FROM_LEAD(trie1, lead);
        }
        यदि(value!=trie1->initial_मानम्) {
            utrie2_set32_For_Lead_Surrogate_संहिता_Unit(context.trie, lead, value, p_Error_संहिता);
        }
    }
    यदि(U_SUCCESS(*p_Error_संहिता)) {
        utrie2_freeze(context.trie,
                      trie1->data32!=शून्यसूचकः ? UTRIE2_32_मानम्_द्वयङ्काः : UTRIE2_16_मानम्_द्वयङ्काः,
                      p_Error_संहिता);
    }
#यदि_परिभाषितम् UTRIE2_DEBUG
    यदि(U_SUCCESS(*p_Error_संहिता)) {
        utrie_printLengths(trie1);
        utrie2_printLengths(context.trie, "fromUTrie");
    }
#यदि_अन्तः
    यदि(U_FAILURE(*p_Error_संहिता)) {
        utrie2_close(context.trie);
        context.trie=शून्यसूचकः;
    }
    प्रत्यागम context.trie;
}

स्थिर अन्तःस्थ UBool
is_अन्तर्_Null_खण्डम्(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, खण्डम्;

    यदि(U_IS_LEAD(c) && forLSCP) {
        i2=(UTRIE2_LSCP_सूचकांक_2_विस्थापनम्-(0xd800>>UTRIE2_सरणम्_2))+
            (c>>UTRIE2_सरणम्_2);
    } अन्यथा {
        i2=trie->index1[c>>UTRIE2_सरणम्_1]+
            ((c>>UTRIE2_सरणम्_2)&UTRIE2_सूचकांक_2_आवरणम्);
    }
    खण्डम्=trie->index2[i2];
    प्रत्यागम खण्डम् == trie->दत्त_Null_विस्थापनम्;
}

स्थिर int32_t
alloc_Index2_खण्डम्(UNewTrie2 *trie) {
    int32_t new_खण्डम्, new_शिखरम्;

    new_खण्डम्=trie->index2_दीर्घता;
    new_शिखरम्=new_खण्डम्+UTRIE2_सूचकांक_2_खण्डम्_दीर्घता;
    यदि(new_शिखरम्>UPRV_LENGTHOF(trie->index2)) {
        /*
         * Should never occur.
         * Either UTRIE2_MAX_BUILD_TIME_INDEX_LENGTH is incorrect,
         * or the code writes more values than should be possible.
         */
        प्रत्यागम -1;
    }
    trie->index2_दीर्घता=new_शिखरम्;
    uprv_memcpy(trie->index2+new_खण्डम्, trie->index2+trie->index2_Null_विस्थापनम्, UTRIE2_सूचकांक_2_खण्डम्_दीर्घता*4);
    प्रत्यागम new_खण्डम्;
}

स्थिर int32_t
get_Index2_खण्डम्(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i1, i2;

    यदि(U_IS_LEAD(c) && forLSCP) {
        प्रत्यागम UTRIE2_LSCP_सूचकांक_2_विस्थापनम्;
    }

    i1=c>>UTRIE2_सरणम्_1;
    i2=trie->index1[i1];
    यदि(i2==trie->index2_Null_विस्थापनम्) {
        i2=alloc_Index2_खण्डम्(trie);
        यदि(i2<0) {
            प्रत्यागम -1;  /* program error */
        }
        trie->index1[i1]=i2;
    }
    प्रत्यागम i2;
}

स्थिर int32_t
alloc_दत्त_खण्डम्(UNewTrie2 *trie, int32_t copy_खण्डम्) {
    int32_t new_खण्डम्, new_शिखरम्;

    यदि(trie->first_Free_खण्डम्!=0) {
        /* get the first free block */
        new_खण्डम्=trie->first_Free_खण्डम्;
        trie->first_Free_खण्डम्=-trie->map[new_खण्डम्>>UTRIE2_सरणम्_2];
    } अन्यथा {
        /* get a new block from the high end */
        new_खण्डम्=trie->दत्त_दीर्घता;
        new_शिखरम्=new_खण्डम्+UTRIE2_दत्त_खण्डम्_दीर्घता;
        यदि(new_शिखरम्>trie->दत्त_क्षमता) {
            /* out of memory in the data array */
            int32_t capacity;
            uint32_t *दत्त;

            यदि(trie->दत्त_क्षमता<UNEWTRIE2_MEDIUM_दत्त_दीर्घता) {
                capacity=UNEWTRIE2_MEDIUM_दत्त_दीर्घता;
            } अन्यथा यदि(trie->दत्त_क्षमता<UNEWTRIE2_MAX_दत्त_दीर्घता) {
                capacity=UNEWTRIE2_MAX_दत्त_दीर्घता;
            } अन्यथा {
                /*
                 * Should never occur.
                 * Either UNEWTRIE2_MAX_DATA_LENGTH is incorrect,
                 * or the code writes more values than should be possible.
                 */
                प्रत्यागम -1;
            }
            दत्त = static_cast<uint32_t*>(uprv_malloc(capacity * 4));
            यदि(दत्त==शून्यसूचकः) {
                प्रत्यागम -1;
            }
            uprv_memcpy(दत्त, trie->दत्त, (size_t)trie->दत्त_दीर्घता*4);
            uprv_free(trie->दत्त);
            trie->दत्त=दत्त;
            trie->दत्त_क्षमता=capacity;
        }
        trie->दत्त_दीर्घता=new_शिखरम्;
    }
    uprv_memcpy(trie->दत्त+new_खण्डम्, trie->दत्त+copy_खण्डम्, UTRIE2_दत्त_खण्डम्_दीर्घता*4);
    trie->map[new_खण्डम्>>UTRIE2_सरणम्_2]=0;
    प्रत्यागम new_खण्डम्;
}

/* call when the block's reference counter reaches 0 */
स्थिर शून्य
release_दत्त_खण्डम्(UNewTrie2 *trie, int32_t खण्डम्) {
    /* put this block at the front of the free-block chain */
    trie->map[खण्डम्>>UTRIE2_सरणम्_2]=-trie->first_Free_खण्डम्;
    trie->first_Free_खण्डम्=खण्डम्;
}

स्थिर अन्तःस्थ UBool
is_Writable_खण्डम्(UNewTrie2 *trie, int32_t खण्डम्) {
    प्रत्यागम खण्डम् != trie->दत्त_Null_विस्थापनम् && 1 == trie->map[खण्डम् >> UTRIE2_सरणम्_2];
}

स्थिर अन्तःस्थ शून्य
समुच्चयः_Index2_प्रविष्टिः(UNewTrie2 *trie, int32_t i2, int32_t खण्डम्) {
    int32_t old_खण्डम्;
    ++trie->map[खण्डम्>>UTRIE2_सरणम्_2];  /* increment first, in case block==oldBlock! */
    old_खण्डम्=trie->index2[i2];
    यदि(0 == --trie->map[old_खण्डम्>>UTRIE2_सरणम्_2]) {
        release_दत्त_खण्डम्(trie, old_खण्डम्);
    }
    trie->index2[i2]=खण्डम्;
}

/**
 * No error checking for illegal arguments.
 *
 * @return -1 if no new data block available (out of memory in data array)
 * @internal
 */
स्थिर int32_t
get_दत्त_खण्डम्(UNewTrie2 *trie, UChar32 c, UBool forLSCP) {
    int32_t i2, old_खण्डम्, new_खण्डम्;

    i2=get_Index2_खण्डम्(trie, c, forLSCP);
    यदि(i2<0) {
        प्रत्यागम -1;  /* program error */
    }

    i2+=(c>>UTRIE2_सरणम्_2)&UTRIE2_सूचकांक_2_आवरणम्;
    old_खण्डम्=trie->index2[i2];
    यदि(is_Writable_खण्डम्(trie, old_खण्डम्)) {
        प्रत्यागम old_खण्डम्;
    }

    /* allocate a new data block */
    new_खण्डम्=alloc_दत्त_खण्डम्(trie, old_खण्डम्);
    यदि(new_खण्डम्<0) {
        /* out of memory in the data array */
        प्रत्यागम -1;
    }
    समुच्चयः_Index2_प्रविष्टिः(trie, i2, new_खण्डम्);
    प्रत्यागम new_खण्डम्;
}

/**
 * @return true if the value was successfully set
 */
स्थिर शून्य
set32(UNewTrie2 *trie,
      UChar32 c, UBool forLSCP, uint32_t value,
      UError_संहिता *p_Error_संहिता) {
    int32_t खण्डम्;

    यदि(trie==शून्यसूचकः || trie->isCompacted) {
        *p_Error_संहिता=U_NO_WRITE_PERMISSION;
        प्रत्यागम;
    }
#यदि_परिभाषितम् UCPTRIE_DEBUG
    umutablecptrie_समुच्चयः(trie->t3, c, value, p_Error_संहिता);
#यदि_अन्तः

    खण्डम्=get_दत्त_खण्डम्(trie, c, forLSCP);
    यदि(खण्डम्<0) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    trie->दत्त[खण्डम्+(c&UTRIE2_दत्त_आवरणम्)]=value;
}

U_CAPI शून्य U_EXPORT2
utrie2_set32(UTrie2 *trie, UChar32 c, uint32_t value, UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    यदि((uint32_t)c>0x10ffff) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    set32(trie->newTrie, c, सत्य, value, p_Error_संहिता);
}

U_CAPI शून्य U_EXPORT2
utrie2_set32_For_Lead_Surrogate_संहिता_Unit(UTrie2 *trie,
                                     UChar32 c, uint32_t value,
                                     UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    यदि(!U_IS_LEAD(c)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    set32(trie->newTrie, c, असत्य, value, p_Error_संहिता);
}

स्थिर शून्य
write_खण्डम्(uint32_t *खण्डम्, uint32_t value) {
    uint32_t *सीमा=खण्डम्+UTRIE2_दत्त_खण्डम्_दीर्घता;
    यावत्(खण्डम्<सीमा) {
        *खण्डम्++=value;
    }
}

/**
 * initialValue is ignored if overwrite=true
 * @internal
 */
स्थिर शून्य
fill_खण्डम्(uint32_t *खण्डम्, UChar32 start, UChar32 सीमा,
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

U_CAPI शून्य U_EXPORT2
utrie2_समुच्चयः_Range32(UTrie2 *trie,
                  UChar32 start, UChar32 अन्तः,
                  uint32_t value, UBool overwrite,
                  UError_संहिता *p_Error_संहिता) {
    /*
     * repeat value in [start..end]
     * mark index values for repeat-data blocks by setting bit 31 of the index values
     * fill around existing values if any, if(overwrite)
     */
    UNewTrie2 *newTrie;
    int32_t खण्डम्, rest, repeat_खण्डम्;
    UChar32 सीमा;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    यदि((uint32_t)start>0x10ffff || (uint32_t)अन्तः>0x10ffff || start>अन्तः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    newTrie=trie->newTrie;
    यदि(newTrie==शून्यसूचकः || newTrie->isCompacted) {
        *p_Error_संहिता=U_NO_WRITE_PERMISSION;
        प्रत्यागम;
    }
#यदि_परिभाषितम् UCPTRIE_DEBUG
    umutablecptrie_समुच्चयः_Range(newTrie->t3, start, अन्तः, value, p_Error_संहिता);
#यदि_अन्तः
    यदि(!overwrite && value==newTrie->initial_मानम्) {
        प्रत्यागम; /* nothing to do */
    }

    सीमा=अन्तः+1;
    यदि(start&UTRIE2_दत्त_आवरणम्) {
        UChar32 अग्र_Start;

        /* set partial block at [start..following block boundary[ */
        खण्डम्=get_दत्त_खण्डम्(newTrie, start, सत्य);
        यदि(खण्डम्<0) {
            *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }

        अग्र_Start=(start+UTRIE2_दत्त_आवरणम्)&~UTRIE2_दत्त_आवरणम्;
        यदि(अग्र_Start<=सीमा) {
            fill_खण्डम्(newTrie->दत्त+खण्डम्, start&UTRIE2_दत्त_आवरणम्, UTRIE2_दत्त_खण्डम्_दीर्घता,
                      value, newTrie->initial_मानम्, overwrite);
            start=अग्र_Start;
        } अन्यथा {
            fill_खण्डम्(newTrie->दत्त+खण्डम्, start&UTRIE2_दत्त_आवरणम्, सीमा&UTRIE2_दत्त_आवरणम्,
                      value, newTrie->initial_मानम्, overwrite);
            प्रत्यागम;
        }
    }

    /* number of positions in the last, partial block */
    rest=सीमा&UTRIE2_दत्त_आवरणम्;

    /* round down limit to a block boundary */
    सीमा&=~UTRIE2_दत्त_आवरणम्;

    /* iterate over all-value blocks */
    यदि(value==newTrie->initial_मानम्) {
        repeat_खण्डम्=newTrie->दत्त_Null_विस्थापनम्;
    } अन्यथा {
        repeat_खण्डम्=-1;
    }

    यावत्(start<सीमा) {
        int32_t i2;
        UBool समुच्चयः_Repeat_खण्डम्=असत्य;

        यदि(value==newTrie->initial_मानम् && is_अन्तर्_Null_खण्डम्(newTrie, start, सत्य)) {
            start+=UTRIE2_दत्त_खण्डम्_दीर्घता; /* nothing to do */
            अनुवर्त;
        }

        /* get index value */
        i2=get_Index2_खण्डम्(newTrie, start, सत्य);
        यदि(i2<0) {
            *p_Error_संहिता=U_आन्तरिकम्_क्रमादेशः_ERROR;
            प्रत्यागम;
        }
        i2+=(start>>UTRIE2_सरणम्_2)&UTRIE2_सूचकांक_2_आवरणम्;
        खण्डम्=newTrie->index2[i2];
        यदि(is_Writable_खण्डम्(newTrie, खण्डम्)) {
            /* already allocated */
            यदि(overwrite && खण्डम्>=UNEWTRIE2_दत्त_0800_विस्थापनम्) {
                /*
                 * We overwrite all values, and it's not a
                 * protected (ASCII-linear or 2-byte UTF-8) block:
                 * replace with the repeatBlock.
                 */
                समुच्चयः_Repeat_खण्डम्=सत्य;
            } अन्यथा {
                /* !overwrite, or protected block: just write the values into this block */
                fill_खण्डम्(newTrie->दत्त+खण्डम्,
                          0, UTRIE2_दत्त_खण्डम्_दीर्घता,
                          value, newTrie->initial_मानम्, overwrite);
            }
        } अन्यथा यदि(newTrie->दत्त[खण्डम्]!=value && (overwrite || खण्डम्==newTrie->दत्त_Null_विस्थापनम्)) {
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
            समुच्चयः_Repeat_खण्डम्=सत्य;
        }
        यदि(समुच्चयः_Repeat_खण्डम्) {
            यदि(repeat_खण्डम्>=0) {
                समुच्चयः_Index2_प्रविष्टिः(newTrie, i2, repeat_खण्डम्);
            } अन्यथा {
                /* create and set and fill the repeatBlock */
                repeat_खण्डम्=get_दत्त_खण्डम्(newTrie, start, सत्य);
                यदि(repeat_खण्डम्<0) {
                    *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                    प्रत्यागम;
                }
                write_खण्डम्(newTrie->दत्त+repeat_खण्डम्, value);
            }
        }

        start+=UTRIE2_दत्त_खण्डम्_दीर्घता;
    }

    यदि(rest>0) {
        /* set partial block at [last block boundary..limit[ */
        खण्डम्=get_दत्त_खण्डम्(newTrie, start, सत्य);
        यदि(खण्डम्<0) {
            *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }

        fill_खण्डम्(newTrie->दत्त+खण्डम्, 0, rest, value, newTrie->initial_मानम्, overwrite);
    }
}

/* compaction --------------------------------------------------------------- */

स्थिर अन्तःस्थ UBool
equal_int32(ध्रुव int32_t *s, ध्रुव int32_t *t, int32_t length) {
    यावत्(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    प्रत्यागम length == 0;
}

स्थिर अन्तःस्थ UBool
equal_uint32(ध्रुव uint32_t *s, ध्रुव uint32_t *t, int32_t length) {
    यावत्(length>0 && *s==*t) {
        ++s;
        ++t;
        --length;
    }
    प्रत्यागम length == 0;
}

स्थिर int32_t
find_Same_Index2_खण्डम्(ध्रुव int32_t *idx, int32_t index2_दीर्घता, int32_t other_खण्डम्) {
    int32_t खण्डम्;

    /* ensure that we do not even partially get past index2Length */
    index2_दीर्घता-=UTRIE2_सूचकांक_2_खण्डम्_दीर्घता;

    प्रति(खण्डम्=0; खण्डम्<=index2_दीर्घता; ++खण्डम्) {
        यदि(equal_int32(idx+खण्डम्, idx+other_खण्डम्, UTRIE2_सूचकांक_2_खण्डम्_दीर्घता)) {
            प्रत्यागम खण्डम्;
        }
    }
    प्रत्यागम -1;
}

स्थिर int32_t
find_Same_दत्त_खण्डम्_2(ध्रुव uint32_t *दत्त, int32_t दत्त_दीर्घता, int32_t other_खण्डम्, int32_t खण्डम्_दीर्घता) {
    int32_t खण्डम्;

    /* ensure that we do not even partially get past dataLength */
    दत्त_दीर्घता-=खण्डम्_दीर्घता;

    प्रति(खण्डम्=0; खण्डम्<=दत्त_दीर्घता; खण्डम्+=UTRIE2_दत्त_GRANULARITY) {
        यदि(equal_uint32(दत्त+खण्डम्, दत्त+other_खण्डम्, खण्डम्_दीर्घता)) {
            प्रत्यागम खण्डम्;
        }
    }
    प्रत्यागम -1;
}

/*
 * Find the start of the last range in the trie by enumerating backward.
 * Indexes for supplementary code points higher than this will be omitted.
 */
स्थिर UChar32
find_उच्चः_Start(UNewTrie2 *trie, uint32_t उच्चः_मानम्) {
    ध्रुव uint32_t *data32;

    uint32_t value, initial_मानम्;
    UChar32 c, पूर्व;
    int32_t i1, i2, j, i2_खण्डम्, पूर्व_I2_खण्डम्, index2_Null_विस्थापनम्, खण्डम्, पूर्व_खण्डम्, null_खण्डम्;

    data32=trie->दत्त;
    initial_मानम्=trie->initial_मानम्;

    index2_Null_विस्थापनम्=trie->index2_Null_विस्थापनम्;
    null_खण्डम्=trie->दत्त_Null_विस्थापनम्;

    /* set variables for previous range */
    यदि(उच्चः_मानम्==initial_मानम्) {
        पूर्व_I2_खण्डम्=index2_Null_विस्थापनम्;
        पूर्व_खण्डम्=null_खण्डम्;
    } अन्यथा {
        पूर्व_I2_खण्डम्=-1;
        पूर्व_खण्डम्=-1;
    }
    पूर्व=0x110000;

    /* enumerate index-2 blocks */
    i1=UNEWTRIE2_सूचकांक_1_दीर्घता;
    c=पूर्व;
    यावत्(c>0) {
        i2_खण्डम्=trie->index1[--i1];
        यदि(i2_खण्डम्==पूर्व_I2_खण्डम्) {
            /* the index-2 block is the same as the previous one, and filled with highValue */
            c-=UTRIE2_CP_प्रत्येकम्_सूचकांक_1_प्रविष्टिः;
            अनुवर्त;
        }
        पूर्व_I2_खण्डम्=i2_खण्डम्;
        यदि(i2_खण्डम्==index2_Null_विस्थापनम्) {
            /* this is the null index-2 block */
            यदि(उच्चः_मानम्!=initial_मानम्) {
                प्रत्यागम c;
            }
            c-=UTRIE2_CP_प्रत्येकम्_सूचकांक_1_प्रविष्टिः;
        } अन्यथा {
            /* enumerate data blocks for one index-2 block */
            प्रति(i2=UTRIE2_सूचकांक_2_खण्डम्_दीर्घता; i2>0;) {
                खण्डम्=trie->index2[i2_खण्डम्+ --i2];
                यदि(खण्डम्==पूर्व_खण्डम्) {
                    /* the block is the same as the previous one, and filled with highValue */
                    c-=UTRIE2_दत्त_खण्डम्_दीर्घता;
                    अनुवर्त;
                }
                पूर्व_खण्डम्=खण्डम्;
                यदि(खण्डम्==null_खण्डम्) {
                    /* this is the null data block */
                    यदि(उच्चः_मानम्!=initial_मानम्) {
                        प्रत्यागम c;
                    }
                    c-=UTRIE2_दत्त_खण्डम्_दीर्घता;
                } अन्यथा {
                    प्रति(j=UTRIE2_दत्त_खण्डम्_दीर्घता; j>0;) {
                        value=data32[खण्डम्+ --j];
                        यदि(value!=उच्चः_मानम्) {
                            प्रत्यागम c;
                        }
                        --c;
                    }
                }
            }
        }
    }

    /* deliver last range */
    प्रत्यागम 0;
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
स्थिर शून्य
compact_दत्त(UNewTrie2 *trie) {
#यदि_परिभाषितम् UTRIE2_DEBUG
    int32_t गणनम्_Same=0, योग_Overlaps=0;
#यदि_अन्तः

    int32_t start, newStart, movedStart;
    int32_t खण्डम्_दीर्घता, overlap;
    int32_t i, map_सूचकांक, खण्डम्_गणनम्;

    /* do not compact linear-ASCII data */
    newStart=UTRIE2_दत्त_START_विस्थापनम्;
    प्रति(start=0, i=0; start<newStart; start+=UTRIE2_दत्त_खण्डम्_दीर्घता, ++i) {
        trie->map[i]=start;
    }

    /*
     * Start with a block length of 64 for 2-byte UTF-8,
     * then switch to UTRIE2_DATA_BLOCK_LENGTH.
     */
    खण्डम्_दीर्घता=64;
    खण्डम्_गणनम्=खण्डम्_दीर्घता>>UTRIE2_सरणम्_2;
    प्रति(start=newStart; start<trie->दत्त_दीर्घता;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */
        यदि(start==UNEWTRIE2_दत्त_0800_विस्थापनम्) {
            खण्डम्_दीर्घता=UTRIE2_दत्त_खण्डम्_दीर्घता;
            खण्डम्_गणनम्=1;
        }

        /* skip blocks that are not used */
        यदि(trie->map[start>>UTRIE2_सरणम्_2]<=0) {
            /* advance start to the next block */
            start+=खण्डम्_दीर्घता;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* search for an identical block */
        यदि( (movedStart=find_Same_दत्त_खण्डम्_2(trie->दत्त, newStart, start, खण्डम्_दीर्घता))
             >=0
        ) {
#यदि_परिभाषितम् UTRIE2_DEBUG
            ++गणनम्_Same;
#यदि_अन्तः
            /* found an identical block, set the other block's index value for the current block */
            प्रति(i=खण्डम्_गणनम्, map_सूचकांक=start>>UTRIE2_सरणम्_2; i>0; --i) {
                trie->map[map_सूचकांक++]=movedStart;
                movedStart+=UTRIE2_दत्त_खण्डम्_दीर्घता;
            }

            /* advance start to the next block */
            start+=खण्डम्_दीर्घता;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap (modulo granularity) with the previous, adjacent block */
        प्रति(overlap=खण्डम्_दीर्घता-UTRIE2_दत्त_GRANULARITY;
            overlap>0 && !equal_uint32(trie->दत्त+(newStart-overlap), trie->दत्त+start, overlap);
            overlap-=UTRIE2_दत्त_GRANULARITY) {}

#यदि_परिभाषितम् UTRIE2_DEBUG
            योग_Overlaps+=overlap;
#यदि_अन्तः
        यदि(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            movedStart=newStart-overlap;
            प्रति(i=खण्डम्_गणनम्, map_सूचकांक=start>>UTRIE2_सरणम्_2; i>0; --i) {
                trie->map[map_सूचकांक++]=movedStart;
                movedStart+=UTRIE2_दत्त_खण्डम्_दीर्घता;
            }

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            प्रति(i=खण्डम्_दीर्घता-overlap; i>0; --i) {
                trie->दत्त[newStart++]=trie->दत्त[start++];
            }
        } अन्यथा /* no overlap && newStart==start */ {
            प्रति(i=खण्डम्_गणनम्, map_सूचकांक=start>>UTRIE2_सरणम्_2; i>0; --i) {
                trie->map[map_सूचकांक++]=start;
                start+=UTRIE2_दत्त_खण्डम्_दीर्घता;
            }
            newStart=start;
        }
    }

    /* now adjust the index-2 table */
    प्रति(i=0; i<trie->index2_दीर्घता; ++i) {
        यदि(i==UNEWTRIE2_सूचकांक_GAP_विस्थापनम्) {
            /* Gap indexes are invalid (-1). Skip over the gap. */
            i+=UNEWTRIE2_सूचकांक_GAP_दीर्घता;
        }
        trie->index2[i]=trie->map[trie->index2[i]>>UTRIE2_सरणम्_2];
    }
    trie->दत्त_Null_विस्थापनम्=trie->map[trie->दत्त_Null_विस्थापनम्>>UTRIE2_सरणम्_2];

    /* ensure dataLength alignment */
    यावत्((newStart&(UTRIE2_दत्त_GRANULARITY-1))!=0) {
        trie->दत्त[newStart++]=trie->initial_मानम्;
    }

#यदि_परिभाषितम् UTRIE2_DEBUG
    /* we saved some space */
    प्रारूप_लिख("compacting UTrie2: count of 32-bit data words %lu->%lu  countSame=%ld  sumOverlaps=%ld\n",
            (दीर्घ)trie->दत्त_दीर्घता, (दीर्घ)newStart, (दीर्घ)गणनम्_Same, (दीर्घ)योग_Overlaps);
#यदि_अन्तः

    trie->दत्त_दीर्घता=newStart;
}

स्थिर शून्य
compactIndex2(UNewTrie2 *trie) {
    int32_t i, start, newStart, movedStart, overlap;

    /* do not compact linear-BMP index-2 blocks */
    newStart=UTRIE2_सूचकांक_2_BMP_दीर्घता;
    प्रति(start=0, i=0; start<newStart; start+=UTRIE2_सूचकांक_2_खण्डम्_दीर्घता, ++i) {
        trie->map[i]=start;
    }

    /* Reduce the index table gap to what will be needed at runtime. */
    newStart+=UTRIE2_UTF8_2_B_सूचकांक_2_दीर्घता+((trie->उच्चः_Start-0x10000)>>UTRIE2_सरणम्_1);

    प्रति(start=UNEWTRIE2_सूचकांक_2_NULL_विस्थापनम्; start<trie->index2_दीर्घता;) {
        /*
         * start: index of first entry of current block
         * newStart: index where the current block is to be moved
         *           (right after current end of already-compacted data)
         */

        /* search for an identical block */
        यदि( (movedStart=find_Same_Index2_खण्डम्(trie->index2, newStart, start))
             >=0
        ) {
            /* found an identical block, set the other block's index value for the current block */
            trie->map[start>>UTRIE2_सरणम्_1_2]=movedStart;

            /* advance start to the next block */
            start+=UTRIE2_सूचकांक_2_खण्डम्_दीर्घता;

            /* leave newStart with the previous block! */
            अनुवर्त;
        }

        /* see if the beginning of this block can be overlapped with the end of the previous block */
        /* look for maximum overlap with the previous, adjacent block */
        प्रति(overlap=UTRIE2_सूचकांक_2_खण्डम्_दीर्घता-1;
            overlap>0 && !equal_int32(trie->index2+(newStart-overlap), trie->index2+start, overlap);
            --overlap) {}

        यदि(overlap>0 || newStart<start) {
            /* some overlap, or just move the whole block */
            trie->map[start>>UTRIE2_सरणम्_1_2]=newStart-overlap;

            /* move the non-overlapping indexes to their new positions */
            start+=overlap;
            प्रति(i=UTRIE2_सूचकांक_2_खण्डम्_दीर्घता-overlap; i>0; --i) {
                trie->index2[newStart++]=trie->index2[start++];
            }
        } अन्यथा /* no overlap && newStart==start */ {
            trie->map[start>>UTRIE2_सरणम्_1_2]=start;
            start+=UTRIE2_सूचकांक_2_खण्डम्_दीर्घता;
            newStart=start;
        }
    }

    /* now adjust the index-1 table */
    प्रति(i=0; i<UNEWTRIE2_सूचकांक_1_दीर्घता; ++i) {
        trie->index1[i]=trie->map[trie->index1[i]>>UTRIE2_सरणम्_1_2];
    }
    trie->index2_Null_विस्थापनम्=trie->map[trie->index2_Null_विस्थापनम्>>UTRIE2_सरणम्_1_2];

    /*
     * Ensure data table alignment:
     * Needs to be granularity-aligned for 16-bit trie
     * (so that dataMove will be down-shiftable),
     * and 2-aligned for uint32_t data.
     */
    यावत्((newStart&((UTRIE2_दत्त_GRANULARITY-1)|1))!=0) {
        /* Arbitrary value: 0x3fffc not possible for real data. */
        trie->index2[newStart++] = static_cast<int32_t>(0xffff) << UTRIE2_सूचकांक_सरणम्;
    }

#यदि_परिभाषितम् UTRIE2_DEBUG
    /* we saved some space */
    प्रारूप_लिख("compacting UTrie2: count of 16-bit index words %lu->%lu\n",
            (दीर्घ)trie->index2_दीर्घता, (दीर्घ)newStart);
#यदि_अन्तः

    trie->index2_दीर्घता=newStart;
}

स्थिर शून्य
compactTrie(UTrie2 *trie, UError_संहिता *p_Error_संहिता) {
    UNewTrie2 *newTrie;
    UChar32 उच्चः_Start, supp_उच्चः_Start;
    uint32_t उच्चः_मानम्;

    newTrie=trie->newTrie;

    /* find highStart and round it up */
    उच्चः_मानम्=utrie2_get32(trie, 0x10ffff);
    उच्चः_Start=find_उच्चः_Start(newTrie, उच्चः_मानम्);
    उच्चः_Start=(उच्चः_Start+(UTRIE2_CP_प्रत्येकम्_सूचकांक_1_प्रविष्टिः-1))&~(UTRIE2_CP_प्रत्येकम्_सूचकांक_1_प्रविष्टिः-1);
    यदि(उच्चः_Start==0x110000) {
        उच्चः_मानम्=trie->error_मानम्;
    }

    /*
     * Set trie->highStart only after utrie2_get32(trie, highStart).
     * Otherwise utrie2_get32(trie, highStart) would try to read the highValue.
     */
    trie->उच्चः_Start=newTrie->उच्चः_Start=उच्चः_Start;

#यदि_परिभाषितम् UTRIE2_DEBUG
    प्रारूप_लिख("UTrie2: highStart U+%06lx  highValue 0x%lx  initialValue 0x%lx\n",
            (दीर्घ)उच्चः_Start, (दीर्घ)उच्चः_मानम्, (दीर्घ)trie->initial_मानम्);
#यदि_अन्तः

    यदि(उच्चः_Start<0x110000) {
        /* Blank out [highStart..10ffff] to release associated data blocks. */
        supp_उच्चः_Start= उच्चः_Start<=0x10000 ? 0x10000 : उच्चः_Start;
        utrie2_समुच्चयः_Range32(trie, supp_उच्चः_Start, 0x10ffff, trie->initial_मानम्, सत्य, p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम;
        }
    }

    compact_दत्त(newTrie);
    यदि(उच्चः_Start>0x10000) {
        compactIndex2(newTrie);
#यदि_परिभाषितम् UTRIE2_DEBUG
    } अन्यथा {
        प्रारूप_लिख("UTrie2: highStart U+%04lx  count of 16-bit index words %lu->%lu\n",
                (दीर्घ)उच्चः_Start, (दीर्घ)trie->newTrie->index2_दीर्घता, (दीर्घ)UTRIE2_सूचकांक_1_विस्थापनम्);
#यदि_अन्तः
    }

    /*
     * Store the highValue in the data array and round up the dataLength.
     * Must be done after compactData() because that assumes that dataLength
     * is a multiple of UTRIE2_DATA_BLOCK_LENGTH.
     */
    newTrie->दत्त[newTrie->दत्त_दीर्घता++]=उच्चः_मानम्;
    यावत्((newTrie->दत्त_दीर्घता&(UTRIE2_दत्त_GRANULARITY-1))!=0) {
        newTrie->दत्त[newTrie->दत्त_दीर्घता++]=trie->initial_मानम्;
    }

    newTrie->isCompacted=सत्य;
}

/* serialization ------------------------------------------------------------ */

/**
 * Maximum length of the runtime index array.
 * Limited by its own 16-bit index values, and by uint16_t UTrie2Header.indexLength.
 * (The actual maximum length is lower,
 * (0x110000>>UTRIE2_SHIFT_2)+UTRIE2_UTF8_2B_INDEX_2_LENGTH+UTRIE2_MAX_INDEX_1_LENGTH.)
 */
#परिभाषय UTRIE2_MAX_सूचकांक_दीर्घता 0xffff

/**
 * Maximum length of the runtime data array.
 * Limited by 16-bit index values that are left-shifted by UTRIE2_INDEX_SHIFT,
 * and by uint16_t UTrie2Header.shiftedDataLength.
 */
#परिभाषय UTRIE2_MAX_दत्त_दीर्घता (0xffff<<UTRIE2_सूचकांक_सरणम्)

/* Compact and internally serialize the trie. */
U_CAPI शून्य U_EXPORT2
utrie2_freeze(UTrie2 *trie, UTrie2_मानम्_द्वयङ्काः मानम्_द्वयङ्काः, UError_संहिता *p_Error_संहिता) {
    UNewTrie2 *newTrie;
    UTrie2_शीर्षकम् *शीर्षकम्;
    uint32_t *p;
    uint16_t *dest16;
    int32_t i, length;
    int32_t all_Indexes_दीर्घता;
    int32_t दत्त_Move;  /* >0 if the data is moved to the end of the index array */
    UChar32 उच्चः_Start;

    /* argument check */
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    यदि( trie==शून्यसूचकः ||
        मानम्_द्वयङ्काः<0 || UTRIE2_गणनम्_मानम्_द्वयङ्काः<=मानम्_द्वयङ्काः
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    newTrie=trie->newTrie;
    यदि(newTrie==शून्यसूचकः) {
        /* already frozen */
        UTrie2_मानम्_द्वयङ्काः frozen_मानम्_द्वयङ्काः=
            trie->data16!=शून्यसूचकः ? UTRIE2_16_मानम्_द्वयङ्काः : UTRIE2_32_मानम्_द्वयङ्काः;
        यदि(मानम्_द्वयङ्काः!=frozen_मानम्_द्वयङ्काः) {
            *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        }
        प्रत्यागम;
    }

    /* compact if necessary */
    यदि(!newTrie->isCompacted) {
        compactTrie(trie, p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम;
        }
    }
    उच्चः_Start=trie->उच्चः_Start;

    यदि(उच्चः_Start<=0x10000) {
        all_Indexes_दीर्घता=UTRIE2_सूचकांक_1_विस्थापनम्;
    } अन्यथा {
        all_Indexes_दीर्घता=newTrie->index2_दीर्घता;
    }
    यदि(मानम्_द्वयङ्काः==UTRIE2_16_मानम्_द्वयङ्काः) {
        दत्त_Move=all_Indexes_दीर्घता;
    } अन्यथा {
        दत्त_Move=0;
    }

    /* are indexLength and dataLength within limits? */
    यदि( /* for unshifted indexLength */
        all_Indexes_दीर्घता>UTRIE2_MAX_सूचकांक_दीर्घता ||
        /* for unshifted dataNullOffset */
        (दत्त_Move+newTrie->दत्त_Null_विस्थापनम्)>0xffff ||
        /* for unshifted 2-byte UTF-8 index-2 values */
        (दत्त_Move+UNEWTRIE2_दत्त_0800_विस्थापनम्)>0xffff ||
        /* for shiftedDataLength */
        (दत्त_Move+newTrie->दत्त_दीर्घता)>UTRIE2_MAX_दत्त_दीर्घता
    ) {
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }

    /* calculate the total serialized length */
    length=परिमाण(UTrie2_शीर्षकम्)+all_Indexes_दीर्घता*2;
    यदि(मानम्_द्वयङ्काः==UTRIE2_16_मानम्_द्वयङ्काः) {
        length+=newTrie->दत्त_दीर्घता*2;
    } अन्यथा {
        length+=newTrie->दत्त_दीर्घता*4;
    }

    trie->स्मृतिः=uprv_malloc(length);
    यदि(trie->स्मृतिः==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    trie->length=length;
    trie->is_स्मृतिः_Owned=सत्य;

    trie->सूचकांक_दीर्घता=all_Indexes_दीर्घता;
    trie->दत्त_दीर्घता=newTrie->दत्त_दीर्घता;
    यदि(उच्चः_Start<=0x10000) {
        trie->index2_Null_विस्थापनम्=0xffff;
    } अन्यथा {
        trie->index2_Null_विस्थापनम्=static_cast<uint16_t>(UTRIE2_सूचकांक_2_विस्थापनम्+newTrie->index2_Null_विस्थापनम्);
    }
    trie->दत्त_Null_विस्थापनम्=(uint16_t)(दत्त_Move+newTrie->दत्त_Null_विस्थापनम्);
    trie->उच्चः_मानम्_सूचकांक=दत्त_Move+trie->दत्त_दीर्घता-UTRIE2_दत्त_GRANULARITY;

    /* set the header fields */
    शीर्षकम्=(UTrie2_शीर्षकम् *)trie->स्मृतिः;

    शीर्षकम्->signature=UTRIE2_SIG; /* "Tri2" */
    शीर्षकम्->options=(uint16_t)मानम्_द्वयङ्काः;

    शीर्षकम्->सूचकांक_दीर्घता=(uint16_t)trie->सूचकांक_दीर्घता;
    शीर्षकम्->shifted_दत्त_दीर्घता=(uint16_t)(trie->दत्त_दीर्घता>>UTRIE2_सूचकांक_सरणम्);
    शीर्षकम्->index2_Null_विस्थापनम्=trie->index2_Null_विस्थापनम्;
    शीर्षकम्->दत्त_Null_विस्थापनम्=trie->दत्त_Null_विस्थापनम्;
    शीर्षकम्->shifted_उच्चः_Start=(uint16_t)(उच्चः_Start>>UTRIE2_सरणम्_1);

    /* fill the index and data arrays */
    dest16=(uint16_t *)(शीर्षकम्+1);
    trie->सूचकांक=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove */
    p=(uint32_t *)newTrie->index2;
    प्रति(i=UTRIE2_सूचकांक_2_BMP_दीर्घता; i>0; --i) {
        *dest16++=(uint16_t)((दत्त_Move + *p++)>>UTRIE2_सूचकांक_सरणम्);
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    प्रति(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(दत्त_Move+UTRIE2_BAD_UTF8_दत्त_विस्थापनम्);
    }
    प्रति(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)(दत्त_Move+newTrie->index2[i<<(6-UTRIE2_सरणम्_2)]);
    }

    यदि(उच्चः_Start>0x10000) {
        int32_t index1_दीर्घता=(उच्चः_Start-0x10000)>>UTRIE2_सरणम्_1;
        int32_t index2_विस्थापनम्=UTRIE2_सूचकांक_2_BMP_दीर्घता+UTRIE2_UTF8_2_B_सूचकांक_2_दीर्घता+index1_दीर्घता;

        /* write 16-bit index-1 values for supplementary code points */
        p=(uint32_t *)newTrie->index1+UTRIE2_OMITTED_BMP_सूचकांक_1_दीर्घता;
        प्रति(i=index1_दीर्घता; i>0; --i) {
            *dest16++=(uint16_t)(UTRIE2_सूचकांक_2_विस्थापनम् + *p++);
        }

        /*
         * write the index-2 array values for supplementary code points,
         * shifted right by UTRIE2_INDEX_SHIFT, after adding dataMove
         */
        p=(uint32_t *)newTrie->index2+index2_विस्थापनम्;
        प्रति(i=newTrie->index2_दीर्घता-index2_विस्थापनम्; i>0; --i) {
            *dest16++=(uint16_t)((दत्त_Move + *p++)>>UTRIE2_सूचकांक_सरणम्);
        }
    }

    /* write the 16/32-bit data array */
    विकल्प(मानम्_द्वयङ्काः) {
    पक्ष UTRIE2_16_मानम्_द्वयङ्काः:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=शून्यसूचकः;
        p=newTrie->दत्त;
        प्रति(i=newTrie->दत्त_दीर्घता; i>0; --i) {
            *dest16++=(uint16_t)*p++;
        }
        भङ्ग;
    पक्ष UTRIE2_32_मानम्_द्वयङ्काः:
        /* write 32-bit data values */
        trie->data16=शून्यसूचकः;
        trie->data32=(uint32_t *)dest16;
        uprv_memcpy(dest16, newTrie->दत्त, (size_t)newTrie->दत्त_दीर्घता*4);
        भङ्ग;
    स्वभाव:
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

#यदि_परिभाषितम् UTRIE2_DEBUG
    utrie2_printLengths(trie, "");
#यदि_अन्तः

#यदि_परिभाषितम् UCPTRIE_DEBUG
    umutablecptrie_समुच्चयः_Name(newTrie->t3, trie->name);
    ucptrie_close(
        umutablecptrie_buildImmutable(
            newTrie->t3, UCPTRIE_TYPE_FAST, (UCPTrie_मानम्_विस्तारः)मानम्_द्वयङ्काः, p_Error_संहिता));
#यदि_अन्तः
    /* Delete the UNewTrie2. */
    uprv_free(newTrie->दत्त);
    uprv_free(newTrie);
    trie->newTrie=शून्यसूचकः;
}

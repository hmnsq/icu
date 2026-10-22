// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2001-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  utrie2.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2008aug16 (starting from a copy of utrie.c)
*   created by: Markus W. Scherer
*
*   This is a common implementation of a Unicode trie.
*   It is a kind of compressed, serializable table of 16- or 32-bit values associated with
*   Unicode code points (0..0x10ffff).
*   This is the second common version of a Unicode trie (hence the name UTrie2).
*   See utrie2.h for a comparison.
*
*   This file contains only the runtime and enumeration code, for read-only access.
*   See utrie2_builder.c for the builder code.
*/
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#ଅନ୍ତର୍ଭାଵ "unicode/umutablecptrie.h"
#endif
#ଅନ୍ତର୍ଭାଵ "unicode/utf.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf8.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "utrie2.h"
#ଅନ୍ତର୍ଭାଵ "utrie2_impl.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

ସ୍ଥିର uint32_t
get32(ଅଚଳ UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    ଯଦି(c>=trie->ଉପର_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        ଫେରନ୍ତୁ trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    ଯଦି(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } ଅନ୍ୟଥା {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    ଫେରନ୍ତୁ trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ଅଚଳ UTrie2 *trie, UChar32 c) {
    ଯଦି(trie->data16!=nullptr) {
        ଫେରନ୍ତୁ UTRIE2_GET16(trie, c);
    } ଅନ୍ୟଥା ଯଦି(trie->data32!=nullptr) {
        ଫେରନ୍ତୁ UTRIE2_GET32(trie, c);
    } ଅନ୍ୟଥା ଯଦି((uint32_t)c>0x10ffff) {
        ଫେରନ୍ତୁ trie->error_ମୂଲ୍ୟ;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ get32(trie->newTrie, c, ସତ୍ୟ);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(ଅଚଳ UTrie2 *trie, UChar32 c) {
    ଯଦି(!U_IS_LEAD(c)) {
        ଫେରନ୍ତୁ trie->error_ମୂଲ୍ୟ;
    }
    ଯଦି(trie->data16!=nullptr) {
        ଫେରନ୍ତୁ UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } ଅନ୍ୟଥା ଯଦି(trie->data32!=nullptr) {
        ଫେରନ୍ତୁ UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ get32(trie->newTrie, c, ମିଥ୍ୟା);
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
u8Index(ଅଚଳ UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    ଫେରନ୍ତୁ (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(ଅଚଳ UTrie2 *trie, UChar32 c,
                           ଅଚଳ uint8_t *src, ଅଚଳ uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ଯଦି((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } ଅନ୍ୟଥା {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    ଫେରନ୍ତୁ u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(ଅଚଳ UTrie2 *trie, UChar32 c,
                           ଅଚଳ uint8_t *start, ଅଚଳ uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ଯଦି((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } ଅନ୍ୟଥା {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    ଫେରନ୍ତୁ u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_ମୂଲ୍ୟ_Bits ମୂଲ୍ୟ_Bits,
                          ଅଚଳ ଶୂନ୍ୟ *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    ଅଚଳ UTrie2Header *header;
    ଅଚଳ uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        ମୂଲ୍ୟ_Bits<0 || UTRIE2_ଗଣନା_ମୂଲ୍ୟ_BITS<=ମୂଲ୍ୟ_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    /* enough data for a trie header? */
    ଯଦି(length<(int32_t)ଆକାର(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    /* check the signature */
    header=(ଅଚଳ UTrie2Header *)data;
    ଯଦି(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    /* get the options */
    ଯଦି(ମୂଲ୍ୟ_Bits!=(UTrie2_ମୂଲ୍ୟ_Bits)(header->options&UTRIE2_OPTIONS_ମୂଲ୍ୟ_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, ଆକାର(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.ଉପର_Start=header->shifted_ଉପର_Start<<UTRIE2_SHIFT_1;
    tempTrie.ଉପର_ମୂଲ୍ୟ_Index=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    ଯଦି(ମୂଲ୍ୟ_Bits==UTRIE2_16_ମୂଲ୍ୟ_BITS) {
        tempTrie.ଉପର_ମୂଲ୍ୟ_Index+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)ଆକାର(UTrie2Header)+tempTrie.indexLength*2;
    ଯଦି(ମୂଲ୍ୟ_Bits==UTRIE2_16_ମୂଲ୍ୟ_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } ଅନ୍ୟଥା {
        actualLength+=tempTrie.dataLength*4;
    }
    ଯଦି(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        ଫେରନ୍ତୁ nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(ଆକାର(UTrie2));
    ଯଦି(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, ଆକାର(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=ମିଥ୍ୟା;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(ଅଚଳ uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    ବାଛ(ମୂଲ୍ୟ_Bits) {
    ସ୍ଥିତି UTRIE2_16_ମୂଲ୍ୟ_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_ମୂଲ୍ୟ=trie->index[trie->dataNullOffset];
        trie->error_ମୂଲ୍ୟ=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UTRIE2_32_ମୂଲ୍ୟ_BITS:
        trie->data16=nullptr;
        trie->data32=(ଅଚଳ uint32_t *)p16;
        trie->initial_ମୂଲ୍ୟ=trie->data32[trie->dataNullOffset];
        trie->error_ମୂଲ୍ୟ=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    ଫେରନ୍ତୁ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_ମୂଲ୍ୟ_Bits ମୂଲ୍ୟ_Bits,
                 uint32_t initial_ମୂଲ୍ୟ, uint32_t error_ମୂଲ୍ୟ,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି(ମୂଲ୍ୟ_Bits<0 || UTRIE2_ଗଣନା_ମୂଲ୍ୟ_BITS<=ମୂଲ୍ୟ_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)ଆକାର(UTrie2Header)+indexLength*2;
    ଯଦି(ମୂଲ୍ୟ_Bits==UTRIE2_16_ମୂଲ୍ୟ_BITS) {
        length+=dataLength*2;
    } ଅନ୍ୟଥା {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(ଆକାର(UTrie2));
    ଯଦି(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    uprv_memset(trie, 0, ଆକାର(UTrie2));
    trie->memory=uprv_malloc(length);
    ଯଦି(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=ସତ୍ୟ;

    /* set the UTrie2 fields */
    ଯଦି(ମୂଲ୍ୟ_Bits==UTRIE2_16_ମୂଲ୍ୟ_BITS) {
        dataMove=indexLength;
    } ଅନ୍ୟଥା {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initial_ମୂଲ୍ୟ=initial_ମୂଲ୍ୟ;
    trie->error_ମୂଲ୍ୟ=error_ମୂଲ୍ୟ;
    trie->ଉପର_Start=0;
    trie->ଉପର_ମୂଲ୍ୟ_Index=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)ମୂଲ୍ୟ_Bits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shifted_ଉପର_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    ପାଇଁ(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    ପାଇଁ(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    ପାଇଁ(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    ବାଛ(ମୂଲ୍ୟ_Bits) {
    ସ୍ଥିତି UTRIE2_16_ମୂଲ୍ୟ_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        ପାଇଁ(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_ମୂଲ୍ୟ;
        }
        ପାଇଁ(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_ମୂଲ୍ୟ;
        }
        /* highValue and reserved values */
        ପାଇଁ(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_ମୂଲ୍ୟ;
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି UTRIE2_32_ମୂଲ୍ୟ_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        ପାଇଁ(i=0; i<0x80; ++i) {
            *p++=initial_ମୂଲ୍ୟ;
        }
        ପାଇଁ(; i<0xc0; ++i) {
            *p++=error_ମୂଲ୍ୟ;
        }
        /* highValue and reserved values */
        ପାଇଁ(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initial_ମୂଲ୍ୟ;
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

    ଫେରନ୍ତୁ trie;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrie2_close(UTrie2 *trie) {
    ଯଦି(trie!=nullptr) {
        ଯଦି(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        ଯଦି(trie->newTrie!=nullptr) {
            uprv_free(trie->newTrie->data);
#ifdef UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#endif
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ଅଚଳ UTrie2 *trie) {
    ଫେରନ୍ତୁ trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ଅଚଳ UTrie2 *trie,
                 ଶୂନ୍ୟ *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } ଅନ୍ୟଥା {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    ଫେରନ୍ତୁ trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_ମୂଲ୍ୟ(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ସ୍ଥିର uint32_t U_CALLCONV
enum_Same_ମୂଲ୍ୟ(ଅଚଳ ଶୂନ୍ୟ * /*context*/, uint32_t value) {
    ଫେରନ୍ତୁ value;
}

/**
 * Enumerate all ranges of code points with the same relevant values.
 * The values are transformed from the raw trie entries by the enumValue function.
 *
 * Currently requires start<limit and both start and limit must be multiples
 * of UTRIE2_DATA_BLOCK_LENGTH.
 *
 * Optimizations:
 * - Skip a whole block if we know that it is filled with a single value,
 *   and it is the same as we visited just before.
 * - Handle the null block specially because we know a priori that it is filled
 *   with a single value.
 */
ସ୍ଥିର ଶୂନ୍ୟ
enumEitherTrie(ଅଚଳ UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_ମୂଲ୍ୟ *enum_ମୂଲ୍ୟ, UTrie2EnumRange *enumRange, ଅଚଳ ଶୂନ୍ୟ *context) {
    ଅଚଳ uint32_t *data32;
    ଅଚଳ uint16_t *idx;

    uint32_t value, prev_ମୂଲ୍ୟ, initial_ମୂଲ୍ୟ;
    UChar32 c, prev, ଉପର_Start;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    ଯଦି(enumRange==nullptr) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(enum_ମୂଲ୍ୟ==nullptr) {
        enum_ମୂଲ୍ୟ=enum_Same_ମୂଲ୍ୟ;
    }

    ଯଦି(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } ଅନ୍ୟଥା {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    ଉପର_Start=trie->ଉପର_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_ମୂଲ୍ୟ=enum_ମୂଲ୍ୟ(context, trie->initial_ମୂଲ୍ୟ);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prev_ମୂଲ୍ୟ=0;

    /* enumerate index-2 blocks */
    ପାଇଁ(c=start; c<limit && c<ଉପର_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        ଯଦି(limit<tempLimit) {
            tempLimit=limit;
        }
        ଯଦି(c<=0xffff) {
            ଯଦି(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } ଅନ୍ୟଥା ଯଦି(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_ମୂଲ୍ୟ(0xdc00, limit);
            } ଅନ୍ୟଥା {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_ମୂଲ୍ୟ(0xe000, limit);
            }
        } ଅନ୍ୟଥା {
            /* supplementary code points */
            ଯଦି(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } ଅନ୍ୟଥା {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            ଯଦି(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                ଜାରି;
            }
        }
        prevI2Block=i2Block;
        ଯଦି(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            ଯଦି(prev_ମୂଲ୍ୟ!=initial_ମୂଲ୍ୟ) {
                ଯଦି(prev<c && !enumRange(context, prev, c-1, prev_ମୂଲ୍ୟ)) {
                    ଫେରନ୍ତୁ;
                }
                prevBlock=nullBlock;
                prev=c;
                prev_ମୂଲ୍ୟ=initial_ମୂଲ୍ୟ;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } ଅନ୍ୟଥା {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            ଯଦି((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } ଅନ୍ୟଥା {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            ପାଇଁ(; i2<i2Limit; ++i2) {
                ଯଦି(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } ଅନ୍ୟଥା {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                ଯଦି(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    ଜାରି;
                }
                prevBlock=block;
                ଯଦି(block==nullBlock) {
                    /* this is the null data block */
                    ଯଦି(prev_ମୂଲ୍ୟ!=initial_ମୂଲ୍ୟ) {
                        ଯଦି(prev<c && !enumRange(context, prev, c-1, prev_ମୂଲ୍ୟ)) {
                            ଫେରନ୍ତୁ;
                        }
                        prev=c;
                        prev_ମୂଲ୍ୟ=initial_ମୂଲ୍ୟ;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } ଅନ୍ୟଥା {
                    ପାଇଁ(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_ମୂଲ୍ୟ(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        ଯଦି(value!=prev_ମୂଲ୍ୟ) {
                            ଯଦି(prev<c && !enumRange(context, prev, c-1, prev_ମୂଲ୍ୟ)) {
                                ଫେରନ୍ତୁ;
                            }
                            prev=c;
                            prev_ମୂଲ୍ୟ=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    ଯଦି(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } ଅନ୍ୟଥା ଯଦି(c<limit) {
        /* c==highStart<limit */
        uint32_t ଉପର_ମୂଲ୍ୟ;
        ଯଦି(idx!=nullptr) {
            ଉପର_ମୂଲ୍ୟ=
                data32!=nullptr ?
                    data32[trie->ଉପର_ମୂଲ୍ୟ_Index] :
                    idx[trie->ଉପର_ମୂଲ୍ୟ_Index];
        } ଅନ୍ୟଥା {
            ଉପର_ମୂଲ୍ୟ=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enum_ମୂଲ୍ୟ(context, ଉପର_ମୂଲ୍ୟ);
        ଯଦି(value!=prev_ମୂଲ୍ୟ) {
            ଯଦି(prev<c && !enumRange(context, prev, c-1, prev_ମୂଲ୍ୟ)) {
                ଫେରନ୍ତୁ;
            }
            prev=c;
            prev_ମୂଲ୍ୟ=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_ମୂଲ୍ୟ);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrie2_enum(ଅଚଳ UTrie2 *trie,
            UTrie2_Enum_ମୂଲ୍ୟ *enum_ମୂଲ୍ୟ, UTrie2EnumRange *enumRange, ଅଚଳ ଶୂନ୍ୟ *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_ମୂଲ୍ୟ, enumRange, context);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrie2_enumForLeadSurrogate(ଅଚଳ UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_ମୂଲ୍ୟ *enum_ମୂଲ୍ୟ, UTrie2EnumRange *enumRange,
                            ଅଚଳ ଶୂନ୍ୟ *context) {
    ଯଦି(!U16_IS_LEAD(lead)) {
        ଫେରନ୍ତୁ;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_ମୂଲ୍ୟ, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    ଯଦି(start>=codePointStart) {
        codePoint=U_SENTINEL;
        ଫେରନ୍ତୁ static_cast<uint16_t>(trie->error_ମୂଲ୍ୟ);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    ଫେରନ୍ତୁ result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    ଯଦି(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        ଫେରନ୍ତୁ static_cast<uint16_t>(trie->error_ମୂଲ୍ୟ);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    ଫେରନ୍ତୁ result;
}

U_NAMESPACE_END

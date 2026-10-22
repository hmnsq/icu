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
#അന്തര്ഭാവ "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#അന്തര്ഭാവ "unicode/umutablecptrie.h"
#endif
#അന്തര്ഭാവ "unicode/utf.h"
#അന്തര്ഭാവ "unicode/utf8.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "utrie2.h"
#അന്തര്ഭാവ "utrie2_impl.h"
#അന്തര്ഭാവ "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

സ്ഥിതം uint32_t
get32(സ്ഥിരം UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    എങ്കിൽ(c>=trie->മുകളിലെ_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        തിരികെ trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    എങ്കിൽ(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } അല്ലെങ്കിൽ {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    തിരികെ trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(സ്ഥിരം UTrie2 *trie, UChar32 c) {
    എങ്കിൽ(trie->data16!=nullptr) {
        തിരികെ UTRIE2_GET16(trie, c);
    } അല്ലെങ്കിൽ എങ്കിൽ(trie->data32!=nullptr) {
        തിരികെ UTRIE2_GET32(trie, c);
    } അല്ലെങ്കിൽ എങ്കിൽ((uint32_t)c>0x10ffff) {
        തിരികെ trie->error_മൂല്യം;
    } അല്ലെങ്കിൽ {
        തിരികെ get32(trie->newTrie, c, സത്യം);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(സ്ഥിരം UTrie2 *trie, UChar32 c) {
    എങ്കിൽ(!U_IS_LEAD(c)) {
        തിരികെ trie->error_മൂല്യം;
    }
    എങ്കിൽ(trie->data16!=nullptr) {
        തിരികെ UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } അല്ലെങ്കിൽ എങ്കിൽ(trie->data32!=nullptr) {
        തിരികെ UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } അല്ലെങ്കിൽ {
        തിരികെ get32(trie->newTrie, c, അസത്യം);
    }
}

സ്ഥിതം വരിയിൽ int32_t
u8Index(സ്ഥിരം UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    തിരികെ (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(സ്ഥിരം UTrie2 *trie, UChar32 c,
                           സ്ഥിരം uint8_t *src, സ്ഥിരം uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    എങ്കിൽ((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } അല്ലെങ്കിൽ {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    തിരികെ u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(സ്ഥിരം UTrie2 *trie, UChar32 c,
                           സ്ഥിരം uint8_t *start, സ്ഥിരം uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    എങ്കിൽ((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } അല്ലെങ്കിൽ {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    തിരികെ u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_മൂല്യം_Bits മൂല്യം_Bits,
                          സ്ഥിരം ശൂന്യം *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    സ്ഥിരം UTrie2Header *header;
    സ്ഥിരം uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }

    എങ്കിൽ( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        മൂല്യം_Bits<0 || UTRIE2_എണ്ണം_മൂല്യം_BITS<=മൂല്യം_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    /* enough data for a trie header? */
    എങ്കിൽ(length<(int32_t)വലുപ്പം(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }

    /* check the signature */
    header=(സ്ഥിരം UTrie2Header *)data;
    എങ്കിൽ(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }

    /* get the options */
    എങ്കിൽ(മൂല്യം_Bits!=(UTrie2_മൂല്യം_Bits)(header->options&UTRIE2_OPTIONS_മൂല്യം_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, വലുപ്പം(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.മുകളിലെ_Start=header->shifted_മുകളിലെ_Start<<UTRIE2_SHIFT_1;
    tempTrie.മുകളിലെ_മൂല്യം_Index=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    എങ്കിൽ(മൂല്യം_Bits==UTRIE2_16_മൂല്യം_BITS) {
        tempTrie.മുകളിലെ_മൂല്യം_Index+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)വലുപ്പം(UTrie2Header)+tempTrie.indexLength*2;
    എങ്കിൽ(മൂല്യം_Bits==UTRIE2_16_മൂല്യം_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } അല്ലെങ്കിൽ {
        actualLength+=tempTrie.dataLength*4;
    }
    എങ്കിൽ(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        തിരികെ nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(വലുപ്പം(UTrie2));
    എങ്കിൽ(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, വലുപ്പം(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=അസത്യം;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(സ്ഥിരം uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    തിരഞ്ഞെടുപ്പ്(മൂല്യം_Bits) {
    അവസ്ഥ UTRIE2_16_മൂല്യം_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_മൂല്യം=trie->index[trie->dataNullOffset];
        trie->error_മൂല്യം=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        നിർത്തുക;
    അവസ്ഥ UTRIE2_32_മൂല്യം_BITS:
        trie->data16=nullptr;
        trie->data32=(സ്ഥിരം uint32_t *)p16;
        trie->initial_മൂല്യം=trie->data32[trie->dataNullOffset];
        trie->error_മൂല്യം=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        തിരികെ nullptr;
    }

    എങ്കിൽ(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    തിരികെ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_മൂല്യം_Bits മൂല്യം_Bits,
                 uint32_t initial_മൂല്യം, uint32_t error_മൂല്യം,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }

    എങ്കിൽ(മൂല്യം_Bits<0 || UTRIE2_എണ്ണം_മൂല്യം_BITS<=മൂല്യം_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)വലുപ്പം(UTrie2Header)+indexLength*2;
    എങ്കിൽ(മൂല്യം_Bits==UTRIE2_16_മൂല്യം_BITS) {
        length+=dataLength*2;
    } അല്ലെങ്കിൽ {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(വലുപ്പം(UTrie2));
    എങ്കിൽ(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    uprv_memset(trie, 0, വലുപ്പം(UTrie2));
    trie->memory=uprv_malloc(length);
    എങ്കിൽ(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=സത്യം;

    /* set the UTrie2 fields */
    എങ്കിൽ(മൂല്യം_Bits==UTRIE2_16_മൂല്യം_BITS) {
        dataMove=indexLength;
    } അല്ലെങ്കിൽ {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initial_മൂല്യം=initial_മൂല്യം;
    trie->error_മൂല്യം=error_മൂല്യം;
    trie->മുകളിലെ_Start=0;
    trie->മുകളിലെ_മൂല്യം_Index=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)മൂല്യം_Bits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shifted_മുകളിലെ_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    വേണ്ടി(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    വേണ്ടി(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    വേണ്ടി(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    തിരഞ്ഞെടുപ്പ്(മൂല്യം_Bits) {
    അവസ്ഥ UTRIE2_16_മൂല്യം_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        വേണ്ടി(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_മൂല്യം;
        }
        വേണ്ടി(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_മൂല്യം;
        }
        /* highValue and reserved values */
        വേണ്ടി(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_മൂല്യം;
        }
        നിർത്തുക;
    അവസ്ഥ UTRIE2_32_മൂല്യം_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        വേണ്ടി(i=0; i<0x80; ++i) {
            *p++=initial_മൂല്യം;
        }
        വേണ്ടി(; i<0xc0; ++i) {
            *p++=error_മൂല്യം;
        }
        /* highValue and reserved values */
        വേണ്ടി(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initial_മൂല്യം;
        }
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }

    തിരികെ trie;
}

U_CAPI ശൂന്യം U_EXPORT2
utrie2_close(UTrie2 *trie) {
    എങ്കിൽ(trie!=nullptr) {
        എങ്കിൽ(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        എങ്കിൽ(trie->newTrie!=nullptr) {
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
utrie2_isFrozen(സ്ഥിരം UTrie2 *trie) {
    തിരികെ trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(സ്ഥിരം UTrie2 *trie,
                 ശൂന്യം *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    എങ്കിൽ( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } അല്ലെങ്കിൽ {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    തിരികെ trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_മൂല്യം(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
സ്ഥിതം uint32_t U_CALLCONV
enum_Same_മൂല്യം(സ്ഥിരം ശൂന്യം * /*context*/, uint32_t value) {
    തിരികെ value;
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
സ്ഥിതം ശൂന്യം
enumEitherTrie(സ്ഥിരം UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_മൂല്യം *enum_മൂല്യം, UTrie2EnumRange *enumRange, സ്ഥിരം ശൂന്യം *context) {
    സ്ഥിരം uint32_t *data32;
    സ്ഥിരം uint16_t *idx;

    uint32_t value, prev_മൂല്യം, initial_മൂല്യം;
    UChar32 c, prev, മുകളിലെ_Start;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    എങ്കിൽ(enumRange==nullptr) {
        തിരികെ;
    }
    എങ്കിൽ(enum_മൂല്യം==nullptr) {
        enum_മൂല്യം=enum_Same_മൂല്യം;
    }

    എങ്കിൽ(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } അല്ലെങ്കിൽ {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    മുകളിലെ_Start=trie->മുകളിലെ_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_മൂല്യം=enum_മൂല്യം(context, trie->initial_മൂല്യം);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prev_മൂല്യം=0;

    /* enumerate index-2 blocks */
    വേണ്ടി(c=start; c<limit && c<മുകളിലെ_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        എങ്കിൽ(limit<tempLimit) {
            tempLimit=limit;
        }
        എങ്കിൽ(c<=0xffff) {
            എങ്കിൽ(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } അല്ലെങ്കിൽ എങ്കിൽ(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_മൂല്യം(0xdc00, limit);
            } അല്ലെങ്കിൽ {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_മൂല്യം(0xe000, limit);
            }
        } അല്ലെങ്കിൽ {
            /* supplementary code points */
            എങ്കിൽ(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } അല്ലെങ്കിൽ {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            എങ്കിൽ(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                തുടരുക;
            }
        }
        prevI2Block=i2Block;
        എങ്കിൽ(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            എങ്കിൽ(prev_മൂല്യം!=initial_മൂല്യം) {
                എങ്കിൽ(prev<c && !enumRange(context, prev, c-1, prev_മൂല്യം)) {
                    തിരികെ;
                }
                prevBlock=nullBlock;
                prev=c;
                prev_മൂല്യം=initial_മൂല്യം;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } അല്ലെങ്കിൽ {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            എങ്കിൽ((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } അല്ലെങ്കിൽ {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            വേണ്ടി(; i2<i2Limit; ++i2) {
                എങ്കിൽ(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } അല്ലെങ്കിൽ {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                എങ്കിൽ(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    തുടരുക;
                }
                prevBlock=block;
                എങ്കിൽ(block==nullBlock) {
                    /* this is the null data block */
                    എങ്കിൽ(prev_മൂല്യം!=initial_മൂല്യം) {
                        എങ്കിൽ(prev<c && !enumRange(context, prev, c-1, prev_മൂല്യം)) {
                            തിരികെ;
                        }
                        prev=c;
                        prev_മൂല്യം=initial_മൂല്യം;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } അല്ലെങ്കിൽ {
                    വേണ്ടി(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_മൂല്യം(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        എങ്കിൽ(value!=prev_മൂല്യം) {
                            എങ്കിൽ(prev<c && !enumRange(context, prev, c-1, prev_മൂല്യം)) {
                                തിരികെ;
                            }
                            prev=c;
                            prev_മൂല്യം=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    എങ്കിൽ(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } അല്ലെങ്കിൽ എങ്കിൽ(c<limit) {
        /* c==highStart<limit */
        uint32_t മുകളിലെ_മൂല്യം;
        എങ്കിൽ(idx!=nullptr) {
            മുകളിലെ_മൂല്യം=
                data32!=nullptr ?
                    data32[trie->മുകളിലെ_മൂല്യം_Index] :
                    idx[trie->മുകളിലെ_മൂല്യം_Index];
        } അല്ലെങ്കിൽ {
            മുകളിലെ_മൂല്യം=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enum_മൂല്യം(context, മുകളിലെ_മൂല്യം);
        എങ്കിൽ(value!=prev_മൂല്യം) {
            എങ്കിൽ(prev<c && !enumRange(context, prev, c-1, prev_മൂല്യം)) {
                തിരികെ;
            }
            prev=c;
            prev_മൂല്യം=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_മൂല്യം);
}

U_CAPI ശൂന്യം U_EXPORT2
utrie2_enum(സ്ഥിരം UTrie2 *trie,
            UTrie2_Enum_മൂല്യം *enum_മൂല്യം, UTrie2EnumRange *enumRange, സ്ഥിരം ശൂന്യം *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_മൂല്യം, enumRange, context);
}

U_CAPI ശൂന്യം U_EXPORT2
utrie2_enumForLeadSurrogate(സ്ഥിരം UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_മൂല്യം *enum_മൂല്യം, UTrie2EnumRange *enumRange,
                            സ്ഥിരം ശൂന്യം *context) {
    എങ്കിൽ(!U16_IS_LEAD(lead)) {
        തിരികെ;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_മൂല്യം, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    എങ്കിൽ(start>=codePointStart) {
        codePoint=U_SENTINEL;
        തിരികെ static_cast<uint16_t>(trie->error_മൂല്യം);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    തിരികെ result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    എങ്കിൽ(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        തിരികെ static_cast<uint16_t>(trie->error_മൂല്യം);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    തിരികെ result;
}

U_NAMESPACE_END

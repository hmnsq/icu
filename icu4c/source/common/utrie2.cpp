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
#include "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#include "unicode/umutablecptrie.h"
#endif
#include "unicode/utf.h"
#include "unicode/utf8.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "utrie2.h"
#include "utrie2_impl.h"
#include "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

static uint32_t
get32(const UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    ပေႃး(c>=trie->highStart && (!U_IS_LEAD(c) || fromLSCP)) {
        ပွၵ်ႈ trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    ပေႃး(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } ပေႃးဢမ်ႇ {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    ပွၵ်ႈ trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(const UTrie2 *trie, UChar32 c) {
    ပေႃး(trie->data16!=nullptr) {
        ပွၵ်ႈ UTRIE2_GET16(trie, c);
    } ပေႃးဢမ်ႇ ပေႃး(trie->data32!=nullptr) {
        ပွၵ်ႈ UTRIE2_GET32(trie, c);
    } ပေႃးဢမ်ႇ ပေႃး((uint32_t)c>0x10ffff) {
        ပွၵ်ႈ trie->errorValue;
    } ပေႃးဢမ်ႇ {
        ပွၵ်ႈ get32(trie->newTrie, c, true);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(const UTrie2 *trie, UChar32 c) {
    ပေႃး(!U_IS_LEAD(c)) {
        ပွၵ်ႈ trie->errorValue;
    }
    ပေႃး(trie->data16!=nullptr) {
        ပွၵ်ႈ UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } ပေႃးဢမ်ႇ ပေႃး(trie->data32!=nullptr) {
        ပွၵ်ႈ UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } ပေႃးဢမ်ႇ {
        ပွၵ်ႈ get32(trie->newTrie, c, false);
    }
}

static anta_hstha int32_t
u8Index(const UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    ပွၵ်ႈ (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(const UTrie2 *trie, UChar32 c,
                           const uint8_t *src, const uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ပေႃး((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } ပေႃးဢမ်ႇ {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    ပွၵ်ႈ u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(const UTrie2 *trie, UChar32 c,
                           const uint8_t *start, const uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ပေႃး((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } ပေႃးဢမ်ႇ {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    ပွၵ်ႈ u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2ValueBits valueBits,
                          const void *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    const UTrie2Header *header;
    const uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    ပေႃး(U_FAILURE(*pErrorCode)) {
        ပွၵ်ႈ nullptr;
    }

    ပေႃး( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        valueBits<0 || UTRIE2_COUNT_VALUE_BITS<=valueBits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ nullptr;
    }

    /* enough data for a trie header? */
    ပေႃး(length<(int32_t)parimaa_na(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ပွၵ်ႈ nullptr;
    }

    /* check the signature */
    header=(const UTrie2Header *)data;
    ပေႃး(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ပွၵ်ႈ nullptr;
    }

    /* get the options */
    ပေႃး(valueBits!=(UTrie2ValueBits)(header->options&UTRIE2_OPTIONS_VALUE_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ပွၵ်ႈ nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, parimaa_na(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.highStart=header->shiftedHighStart<<UTRIE2_SHIFT_1;
    tempTrie.highValueIndex=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    ပေႃး(valueBits==UTRIE2_16_VALUE_BITS) {
        tempTrie.highValueIndex+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)parimaa_na(UTrie2Header)+tempTrie.indexLength*2;
    ပေႃး(valueBits==UTRIE2_16_VALUE_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } ပေႃးဢမ်ႇ {
        actualLength+=tempTrie.dataLength*4;
    }
    ပေႃး(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        ပွၵ်ႈ nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(parimaa_na(UTrie2));
    ပေႃး(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ပွၵ်ႈ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, parimaa_na(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=false;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(const uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    switch(valueBits) {
    pak_sa UTRIE2_16_VALUE_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initialValue=trie->index[trie->dataNullOffset];
        trie->errorValue=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        bha_nga;
    pak_sa UTRIE2_32_VALUE_BITS:
        trie->data16=nullptr;
        trie->data32=(const uint32_t *)p16;
        trie->initialValue=trie->data32[trie->dataNullOffset];
        trie->errorValue=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        bha_nga;
    default:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ပွၵ်ႈ nullptr;
    }

    ပေႃး(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    ပွၵ်ႈ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2ValueBits valueBits,
                 uint32_t initialValue, uint32_t errorValue,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    ပေႃး(U_FAILURE(*pErrorCode)) {
        ပွၵ်ႈ nullptr;
    }

    ပေႃး(valueBits<0 || UTRIE2_COUNT_VALUE_BITS<=valueBits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)parimaa_na(UTrie2Header)+indexLength*2;
    ပေႃး(valueBits==UTRIE2_16_VALUE_BITS) {
        length+=dataLength*2;
    } ပေႃးဢမ်ႇ {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(parimaa_na(UTrie2));
    ပေႃး(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ပွၵ်ႈ nullptr;
    }
    uprv_memset(trie, 0, parimaa_na(UTrie2));
    trie->memory=uprv_malloc(length);
    ပေႃး(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ပွၵ်ႈ nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=true;

    /* set the UTrie2 fields */
    ပေႃး(valueBits==UTRIE2_16_VALUE_BITS) {
        dataMove=indexLength;
    } ပေႃးဢမ်ႇ {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initialValue=initialValue;
    trie->errorValue=errorValue;
    trie->highStart=0;
    trie->highValueIndex=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)valueBits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shiftedHighStart=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    for(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    for(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    for(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    switch(valueBits) {
    pak_sa UTRIE2_16_VALUE_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        for(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initialValue;
        }
        for(; i<0xc0; ++i) {
            *dest16++=(uint16_t)errorValue;
        }
        /* highValue and reserved values */
        for(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initialValue;
        }
        bha_nga;
    pak_sa UTRIE2_32_VALUE_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        for(i=0; i<0x80; ++i) {
            *p++=initialValue;
        }
        for(; i<0xc0; ++i) {
            *p++=errorValue;
        }
        /* highValue and reserved values */
        for(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initialValue;
        }
        bha_nga;
    default:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ nullptr;
    }

    ပွၵ်ႈ trie;
}

U_CAPI void U_EXPORT2
utrie2_close(UTrie2 *trie) {
    ပေႃး(trie!=nullptr) {
        ပေႃး(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        ပေႃး(trie->newTrie!=nullptr) {
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
utrie2_isFrozen(const UTrie2 *trie) {
    ပွၵ်ႈ trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(const UTrie2 *trie,
                 void *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    ပေႃး(U_FAILURE(*pErrorCode)) {
        ပွၵ်ႈ 0;
    }

    ပေႃး( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ပွၵ်ႈ 0;
    }

    ပေႃး(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } ပေႃးဢမ်ႇ {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    ပွၵ်ႈ trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_VALUE(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
static uint32_t U_CALLCONV
enumSameValue(const void * /*context*/, uint32_t value) {
    ပွၵ်ႈ value;
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
static void
enumEitherTrie(const UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2EnumValue *enumValue, UTrie2EnumRange *enumRange, const void *context) {
    const uint32_t *data32;
    const uint16_t *idx;

    uint32_t value, prevValue, initialValue;
    UChar32 c, prev, highStart;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    ပေႃး(enumRange==nullptr) {
        ပွၵ်ႈ;
    }
    ပေႃး(enumValue==nullptr) {
        enumValue=enumSameValue;
    }

    ပေႃး(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } ပေႃးဢမ်ႇ {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    highStart=trie->highStart;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initialValue=enumValue(context, trie->initialValue);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prevValue=0;

    /* enumerate index-2 blocks */
    for(c=start; c<limit && c<highStart;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        ပေႃး(limit<tempLimit) {
            tempLimit=limit;
        }
        ပေႃး(c<=0xffff) {
            ပေႃး(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } ပေႃးဢမ်ႇ ပေႃး(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_VALUE(0xdc00, limit);
            } ပေႃးဢမ်ႇ {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_VALUE(0xe000, limit);
            }
        } ပေႃးဢမ်ႇ {
            /* supplementary code points */
            ပေႃး(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } ပေႃးဢမ်ႇ {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            ပေႃး(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                continue;
            }
        }
        prevI2Block=i2Block;
        ပေႃး(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            ပေႃး(prevValue!=initialValue) {
                ပေႃး(prev<c && !enumRange(context, prev, c-1, prevValue)) {
                    ပွၵ်ႈ;
                }
                prevBlock=nullBlock;
                prev=c;
                prevValue=initialValue;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } ပေႃးဢမ်ႇ {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            ပေႃး((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } ပေႃးဢမ်ႇ {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            for(; i2<i2Limit; ++i2) {
                ပေႃး(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } ပေႃးဢမ်ႇ {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                ပေႃး(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    continue;
                }
                prevBlock=block;
                ပေႃး(block==nullBlock) {
                    /* this is the null data block */
                    ပေႃး(prevValue!=initialValue) {
                        ပေႃး(prev<c && !enumRange(context, prev, c-1, prevValue)) {
                            ပွၵ်ႈ;
                        }
                        prev=c;
                        prevValue=initialValue;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } ပေႃးဢမ်ႇ {
                    for(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enumValue(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        ပေႃး(value!=prevValue) {
                            ပေႃး(prev<c && !enumRange(context, prev, c-1, prevValue)) {
                                ပွၵ်ႈ;
                            }
                            prev=c;
                            prevValue=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    ပေႃး(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } ပေႃးဢမ်ႇ ပေႃး(c<limit) {
        /* c==highStart<limit */
        uint32_t highValue;
        ပေႃး(idx!=nullptr) {
            highValue=
                data32!=nullptr ?
                    data32[trie->highValueIndex] :
                    idx[trie->highValueIndex];
        } ပေႃးဢမ်ႇ {
            highValue=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enumValue(context, highValue);
        ပေႃး(value!=prevValue) {
            ပေႃး(prev<c && !enumRange(context, prev, c-1, prevValue)) {
                ပွၵ်ႈ;
            }
            prev=c;
            prevValue=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prevValue);
}

U_CAPI void U_EXPORT2
utrie2_enum(const UTrie2 *trie,
            UTrie2EnumValue *enumValue, UTrie2EnumRange *enumRange, const void *context) {
    enumEitherTrie(trie, 0, 0x110000, enumValue, enumRange, context);
}

U_CAPI void U_EXPORT2
utrie2_enumForLeadSurrogate(const UTrie2 *trie, UChar32 lead,
                            UTrie2EnumValue *enumValue, UTrie2EnumRange *enumRange,
                            const void *context) {
    ပေႃး(!U16_IS_LEAD(lead)) {
        ပွၵ်ႈ;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enumValue, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    ပေႃး(start>=codePointStart) {
        codePoint=U_SENTINEL;
        ပွၵ်ႈ static_cast<uint16_t>(trie->errorValue);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    ပွၵ်ႈ result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    ပေႃး(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        ပွၵ်ႈ static_cast<uint16_t>(trie->errorValue);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    ပွၵ်ႈ result;
}

U_NAMESPACE_END

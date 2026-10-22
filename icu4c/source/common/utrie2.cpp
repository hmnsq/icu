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
#અન્તર્ભાવ "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#અન્તર્ભાવ "unicode/umutablecptrie.h"
#endif
#અન્તર્ભાવ "unicode/utf.h"
#અન્તર્ભાવ "unicode/utf8.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "utrie2.h"
#અન્તર્ભાવ "utrie2_impl.h"
#અન્તર્ભાવ "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

સ્થિર uint32_t
get32(અચલ UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    જો(c>=trie->ઉપલું_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        પાછા trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    જો(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } નહીંતર {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    પાછા trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(અચલ UTrie2 *trie, UChar32 c) {
    જો(trie->data16!=nullptr) {
        પાછા UTRIE2_GET16(trie, c);
    } નહીંતર જો(trie->data32!=nullptr) {
        પાછા UTRIE2_GET32(trie, c);
    } નહીંતર જો((uint32_t)c>0x10ffff) {
        પાછા trie->error_કિંમત;
    } નહીંતર {
        પાછા get32(trie->newTrie, c, સાચું);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(અચલ UTrie2 *trie, UChar32 c) {
    જો(!U_IS_LEAD(c)) {
        પાછા trie->error_કિંમત;
    }
    જો(trie->data16!=nullptr) {
        પાછા UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } નહીંતર જો(trie->data32!=nullptr) {
        પાછા UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } નહીંતર {
        પાછા get32(trie->newTrie, c, ખોટું);
    }
}

સ્થિર અંતઃરેખ int32_t
u8Index(અચલ UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    પાછા (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(અચલ UTrie2 *trie, UChar32 c,
                           અચલ uint8_t *src, અચલ uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    જો((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } નહીંતર {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    પાછા u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(અચલ UTrie2 *trie, UChar32 c,
                           અચલ uint8_t *start, અચલ uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    જો((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } નહીંતર {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    પાછા u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_કિંમત_Bits કિંમત_Bits,
                          અચલ શૂન્ય *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    અચલ UTrie2Header *header;
    અચલ uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }

    જો( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        કિંમત_Bits<0 || UTRIE2_ગણતરી_કિંમત_BITS<=કિંમત_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    /* enough data for a trie header? */
    જો(length<(int32_t)કદ(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }

    /* check the signature */
    header=(અચલ UTrie2Header *)data;
    જો(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }

    /* get the options */
    જો(કિંમત_Bits!=(UTrie2_કિંમત_Bits)(header->options&UTRIE2_OPTIONS_કિંમત_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, કદ(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.ઉપલું_Start=header->shifted_ઉપલું_Start<<UTRIE2_SHIFT_1;
    tempTrie.ઉપલું_કિંમત_Index=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    જો(કિંમત_Bits==UTRIE2_16_કિંમત_BITS) {
        tempTrie.ઉપલું_કિંમત_Index+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)કદ(UTrie2Header)+tempTrie.indexLength*2;
    જો(કિંમત_Bits==UTRIE2_16_કિંમત_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } નહીંતર {
        actualLength+=tempTrie.dataLength*4;
    }
    જો(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        પાછા nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(કદ(UTrie2));
    જો(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    uprv_memcpy(trie, &tempTrie, કદ(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=ખોટું;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(અચલ uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    પસંદગી(કિંમત_Bits) {
    સ્થિતિ UTRIE2_16_કિંમત_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_કિંમત=trie->index[trie->dataNullOffset];
        trie->error_કિંમત=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        તોડો;
    સ્થિતિ UTRIE2_32_કિંમત_BITS:
        trie->data16=nullptr;
        trie->data32=(અચલ uint32_t *)p16;
        trie->initial_કિંમત=trie->data32[trie->dataNullOffset];
        trie->error_કિંમત=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        તોડો;
    મૂળભૂત:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        પાછા nullptr;
    }

    જો(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    પાછા trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_કિંમત_Bits કિંમત_Bits,
                 uint32_t initial_કિંમત, uint32_t error_કિંમત,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    જો(U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }

    જો(કિંમત_Bits<0 || UTRIE2_ગણતરી_કિંમત_BITS<=કિંમત_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)કદ(UTrie2Header)+indexLength*2;
    જો(કિંમત_Bits==UTRIE2_16_કિંમત_BITS) {
        length+=dataLength*2;
    } નહીંતર {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(કદ(UTrie2));
    જો(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    uprv_memset(trie, 0, કદ(UTrie2));
    trie->memory=uprv_malloc(length);
    જો(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=સાચું;

    /* set the UTrie2 fields */
    જો(કિંમત_Bits==UTRIE2_16_કિંમત_BITS) {
        dataMove=indexLength;
    } નહીંતર {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initial_કિંમત=initial_કિંમત;
    trie->error_કિંમત=error_કિંમત;
    trie->ઉપલું_Start=0;
    trie->ઉપલું_કિંમત_Index=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)કિંમત_Bits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shifted_ઉપલું_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    માટે(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    માટે(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    માટે(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    પસંદગી(કિંમત_Bits) {
    સ્થિતિ UTRIE2_16_કિંમત_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        માટે(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_કિંમત;
        }
        માટે(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_કિંમત;
        }
        /* highValue and reserved values */
        માટે(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_કિંમત;
        }
        તોડો;
    સ્થિતિ UTRIE2_32_કિંમત_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        માટે(i=0; i<0x80; ++i) {
            *p++=initial_કિંમત;
        }
        માટે(; i<0xc0; ++i) {
            *p++=error_કિંમત;
        }
        /* highValue and reserved values */
        માટે(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initial_કિંમત;
        }
        તોડો;
    મૂળભૂત:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

    પાછા trie;
}

U_CAPI શૂન્ય U_EXPORT2
utrie2_close(UTrie2 *trie) {
    જો(trie!=nullptr) {
        જો(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        જો(trie->newTrie!=nullptr) {
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
utrie2_isFrozen(અચલ UTrie2 *trie) {
    પાછા trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(અચલ UTrie2 *trie,
                 શૂન્ય *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    જો(U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }

    જો( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    જો(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } નહીંતર {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    પાછા trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_કિંમત(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
સ્થિર uint32_t U_CALLCONV
enum_Same_કિંમત(અચલ શૂન્ય * /*context*/, uint32_t value) {
    પાછા value;
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
સ્થિર શૂન્ય
enumEitherTrie(અચલ UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_કિંમત *enum_કિંમત, UTrie2EnumRange *enumRange, અચલ શૂન્ય *context) {
    અચલ uint32_t *data32;
    અચલ uint16_t *idx;

    uint32_t value, prev_કિંમત, initial_કિંમત;
    UChar32 c, prev, ઉપલું_Start;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    જો(enumRange==nullptr) {
        પાછા;
    }
    જો(enum_કિંમત==nullptr) {
        enum_કિંમત=enum_Same_કિંમત;
    }

    જો(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } નહીંતર {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    ઉપલું_Start=trie->ઉપલું_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_કિંમત=enum_કિંમત(context, trie->initial_કિંમત);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prev_કિંમત=0;

    /* enumerate index-2 blocks */
    માટે(c=start; c<limit && c<ઉપલું_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        જો(limit<tempLimit) {
            tempLimit=limit;
        }
        જો(c<=0xffff) {
            જો(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } નહીંતર જો(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_કિંમત(0xdc00, limit);
            } નહીંતર {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_કિંમત(0xe000, limit);
            }
        } નહીંતર {
            /* supplementary code points */
            જો(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } નહીંતર {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            જો(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                ચાલુ;
            }
        }
        prevI2Block=i2Block;
        જો(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            જો(prev_કિંમત!=initial_કિંમત) {
                જો(prev<c && !enumRange(context, prev, c-1, prev_કિંમત)) {
                    પાછા;
                }
                prevBlock=nullBlock;
                prev=c;
                prev_કિંમત=initial_કિંમત;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } નહીંતર {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            જો((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } નહીંતર {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            માટે(; i2<i2Limit; ++i2) {
                જો(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } નહીંતર {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                જો(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    ચાલુ;
                }
                prevBlock=block;
                જો(block==nullBlock) {
                    /* this is the null data block */
                    જો(prev_કિંમત!=initial_કિંમત) {
                        જો(prev<c && !enumRange(context, prev, c-1, prev_કિંમત)) {
                            પાછા;
                        }
                        prev=c;
                        prev_કિંમત=initial_કિંમત;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } નહીંતર {
                    માટે(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_કિંમત(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        જો(value!=prev_કિંમત) {
                            જો(prev<c && !enumRange(context, prev, c-1, prev_કિંમત)) {
                                પાછા;
                            }
                            prev=c;
                            prev_કિંમત=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    જો(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } નહીંતર જો(c<limit) {
        /* c==highStart<limit */
        uint32_t ઉપલું_કિંમત;
        જો(idx!=nullptr) {
            ઉપલું_કિંમત=
                data32!=nullptr ?
                    data32[trie->ઉપલું_કિંમત_Index] :
                    idx[trie->ઉપલું_કિંમત_Index];
        } નહીંતર {
            ઉપલું_કિંમત=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enum_કિંમત(context, ઉપલું_કિંમત);
        જો(value!=prev_કિંમત) {
            જો(prev<c && !enumRange(context, prev, c-1, prev_કિંમત)) {
                પાછા;
            }
            prev=c;
            prev_કિંમત=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_કિંમત);
}

U_CAPI શૂન્ય U_EXPORT2
utrie2_enum(અચલ UTrie2 *trie,
            UTrie2_Enum_કિંમત *enum_કિંમત, UTrie2EnumRange *enumRange, અચલ શૂન્ય *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_કિંમત, enumRange, context);
}

U_CAPI શૂન્ય U_EXPORT2
utrie2_enumForLeadSurrogate(અચલ UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_કિંમત *enum_કિંમત, UTrie2EnumRange *enumRange,
                            અચલ શૂન્ય *context) {
    જો(!U16_IS_LEAD(lead)) {
        પાછા;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_કિંમત, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    જો(start>=codePointStart) {
        codePoint=U_SENTINEL;
        પાછા static_cast<uint16_t>(trie->error_કિંમત);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    પાછા result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    જો(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        પાછા static_cast<uint16_t>(trie->error_કિંમત);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    પાછા result;
}

U_NAMESPACE_END

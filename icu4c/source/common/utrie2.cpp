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
#अन्तर्भाव "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#अन्तर्भाव "unicode/umutablecptrie.h"
#endif
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "utrie2.h"
#अन्तर्भाव "utrie2_impl.h"
#अन्तर्भाव "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

स्थिर uint32_t
get32(ध्रुव UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    जइ(c>=trie->उच्चः_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        प्रत्यागम trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    जइ(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } अण्णहा {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    प्रत्यागम trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ध्रुव UTrie2 *trie, UChar32 c) {
    जइ(trie->data16!=nullptr) {
        प्रत्यागम UTRIE2_GET16(trie, c);
    } अण्णहा जइ(trie->data32!=nullptr) {
        प्रत्यागम UTRIE2_GET32(trie, c);
    } अण्णहा जइ((uint32_t)c>0x10ffff) {
        प्रत्यागम trie->error_मानम्;
    } अण्णहा {
        प्रत्यागम get32(trie->newTrie, c, सच्च);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(ध्रुव UTrie2 *trie, UChar32 c) {
    जइ(!U_IS_LEAD(c)) {
        प्रत्यागम trie->error_मानम्;
    }
    जइ(trie->data16!=nullptr) {
        प्रत्यागम UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } अण्णहा जइ(trie->data32!=nullptr) {
        प्रत्यागम UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } अण्णहा {
        प्रत्यागम get32(trie->newTrie, c, मिच्छ);
    }
}

स्थिर अन्तःस्थ int32_t
u8Index(ध्रुव UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    प्रत्यागम (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(ध्रुव UTrie2 *trie, UChar32 c,
                           ध्रुव uint8_t *src, ध्रुव uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    जइ((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } अण्णहा {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    प्रत्यागम u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(ध्रुव UTrie2 *trie, UChar32 c,
                           ध्रुव uint8_t *start, ध्रुव uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    जइ((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } अण्णहा {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    प्रत्यागम u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_मानम्_Bits मानम्_Bits,
                          ध्रुव शून्य *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    ध्रुव UTrie2Header *header;
    ध्रुव uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }

    जइ( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        मानम्_Bits<0 || UTRIE2_गणनम्_मानम्_BITS<=मानम्_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    /* enough data for a trie header? */
    जइ(length<(int32_t)परिमाण(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम nullptr;
    }

    /* check the signature */
    header=(ध्रुव UTrie2Header *)data;
    जइ(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम nullptr;
    }

    /* get the options */
    जइ(मानम्_Bits!=(UTrie2_मानम्_Bits)(header->options&UTRIE2_OPTIONS_मानम्_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, परिमाण(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.उच्चः_Start=header->shifted_उच्चः_Start<<UTRIE2_SHIFT_1;
    tempTrie.उच्चः_मानम्_Index=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    जइ(मानम्_Bits==UTRIE2_16_मानम्_BITS) {
        tempTrie.उच्चः_मानम्_Index+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)परिमाण(UTrie2Header)+tempTrie.indexLength*2;
    जइ(मानम्_Bits==UTRIE2_16_मानम्_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } अण्णहा {
        actualLength+=tempTrie.dataLength*4;
    }
    जइ(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        प्रत्यागम nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    जइ(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    uprv_memcpy(trie, &tempTrie, परिमाण(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=मिच्छ;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(ध्रुव uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    विकल्प(मानम्_Bits) {
    पक्ष UTRIE2_16_मानम्_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_मानम्=trie->index[trie->dataNullOffset];
        trie->error_मानम्=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        भङ्ग;
    पक्ष UTRIE2_32_मानम्_BITS:
        trie->data16=nullptr;
        trie->data32=(ध्रुव uint32_t *)p16;
        trie->initial_मानम्=trie->data32[trie->dataNullOffset];
        trie->error_मानम्=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        भङ्ग;
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम nullptr;
    }

    जइ(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    प्रत्यागम trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_मानम्_Bits मानम्_Bits,
                 uint32_t initial_मानम्, uint32_t error_मानम्,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम nullptr;
    }

    जइ(मानम्_Bits<0 || UTRIE2_गणनम्_मानम्_BITS<=मानम्_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)परिमाण(UTrie2Header)+indexLength*2;
    जइ(मानम्_Bits==UTRIE2_16_मानम्_BITS) {
        length+=dataLength*2;
    } अण्णहा {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    जइ(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    uprv_memset(trie, 0, परिमाण(UTrie2));
    trie->memory=uprv_malloc(length);
    जइ(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=सच्च;

    /* set the UTrie2 fields */
    जइ(मानम्_Bits==UTRIE2_16_मानम्_BITS) {
        dataMove=indexLength;
    } अण्णहा {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initial_मानम्=initial_मानम्;
    trie->error_मानम्=error_मानम्;
    trie->उच्चः_Start=0;
    trie->उच्चः_मानम्_Index=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)मानम्_Bits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shifted_उच्चः_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    प्रति(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    प्रति(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    प्रति(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    विकल्प(मानम्_Bits) {
    पक्ष UTRIE2_16_मानम्_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        प्रति(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_मानम्;
        }
        प्रति(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_मानम्;
        }
        /* highValue and reserved values */
        प्रति(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_मानम्;
        }
        भङ्ग;
    पक्ष UTRIE2_32_मानम्_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        प्रति(i=0; i<0x80; ++i) {
            *p++=initial_मानम्;
        }
        प्रति(; i<0xc0; ++i) {
            *p++=error_मानम्;
        }
        /* highValue and reserved values */
        प्रति(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initial_मानम्;
        }
        भङ्ग;
    स्वभाव:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }

    प्रत्यागम trie;
}

U_CAPI शून्य U_EXPORT2
utrie2_close(UTrie2 *trie) {
    जइ(trie!=nullptr) {
        जइ(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        जइ(trie->newTrie!=nullptr) {
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
utrie2_isFrozen(ध्रुव UTrie2 *trie) {
    प्रत्यागम trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ध्रुव UTrie2 *trie,
                 शून्य *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    जइ( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    जइ(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } अण्णहा {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    प्रत्यागम trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_मानम्(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
स्थिर uint32_t U_CALLCONV
enum_Same_मानम्(ध्रुव शून्य * /*context*/, uint32_t value) {
    प्रत्यागम value;
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
स्थिर शून्य
enumEitherTrie(ध्रुव UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_मानम् *enum_मानम्, UTrie2EnumRange *enumRange, ध्रुव शून्य *context) {
    ध्रुव uint32_t *data32;
    ध्रुव uint16_t *idx;

    uint32_t value, prev_मानम्, initial_मानम्;
    UChar32 c, prev, उच्चः_Start;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    जइ(enumRange==nullptr) {
        प्रत्यागम;
    }
    जइ(enum_मानम्==nullptr) {
        enum_मानम्=enum_Same_मानम्;
    }

    जइ(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } अण्णहा {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    उच्चः_Start=trie->उच्चः_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_मानम्=enum_मानम्(context, trie->initial_मानम्);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prev_मानम्=0;

    /* enumerate index-2 blocks */
    प्रति(c=start; c<limit && c<उच्चः_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        जइ(limit<tempLimit) {
            tempLimit=limit;
        }
        जइ(c<=0xffff) {
            जइ(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } अण्णहा जइ(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_मानम्(0xdc00, limit);
            } अण्णहा {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_मानम्(0xe000, limit);
            }
        } अण्णहा {
            /* supplementary code points */
            जइ(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } अण्णहा {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            जइ(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                अनुवर्त;
            }
        }
        prevI2Block=i2Block;
        जइ(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            जइ(prev_मानम्!=initial_मानम्) {
                जइ(prev<c && !enumRange(context, prev, c-1, prev_मानम्)) {
                    प्रत्यागम;
                }
                prevBlock=nullBlock;
                prev=c;
                prev_मानम्=initial_मानम्;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } अण्णहा {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            जइ((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } अण्णहा {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            प्रति(; i2<i2Limit; ++i2) {
                जइ(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } अण्णहा {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                जइ(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    अनुवर्त;
                }
                prevBlock=block;
                जइ(block==nullBlock) {
                    /* this is the null data block */
                    जइ(prev_मानम्!=initial_मानम्) {
                        जइ(prev<c && !enumRange(context, prev, c-1, prev_मानम्)) {
                            प्रत्यागम;
                        }
                        prev=c;
                        prev_मानम्=initial_मानम्;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } अण्णहा {
                    प्रति(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_मानम्(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        जइ(value!=prev_मानम्) {
                            जइ(prev<c && !enumRange(context, prev, c-1, prev_मानम्)) {
                                प्रत्यागम;
                            }
                            prev=c;
                            prev_मानम्=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    जइ(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } अण्णहा जइ(c<limit) {
        /* c==highStart<limit */
        uint32_t उच्चः_मानम्;
        जइ(idx!=nullptr) {
            उच्चः_मानम्=
                data32!=nullptr ?
                    data32[trie->उच्चः_मानम्_Index] :
                    idx[trie->उच्चः_मानम्_Index];
        } अण्णहा {
            उच्चः_मानम्=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enum_मानम्(context, उच्चः_मानम्);
        जइ(value!=prev_मानम्) {
            जइ(prev<c && !enumRange(context, prev, c-1, prev_मानम्)) {
                प्रत्यागम;
            }
            prev=c;
            prev_मानम्=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_मानम्);
}

U_CAPI शून्य U_EXPORT2
utrie2_enum(ध्रुव UTrie2 *trie,
            UTrie2_Enum_मानम् *enum_मानम्, UTrie2EnumRange *enumRange, ध्रुव शून्य *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_मानम्, enumRange, context);
}

U_CAPI शून्य U_EXPORT2
utrie2_enumForLeadSurrogate(ध्रुव UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_मानम् *enum_मानम्, UTrie2EnumRange *enumRange,
                            ध्रुव शून्य *context) {
    जइ(!U16_IS_LEAD(lead)) {
        प्रत्यागम;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_मानम्, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    जइ(start>=codePointStart) {
        codePoint=U_SENTINEL;
        प्रत्यागम static_cast<uint16_t>(trie->error_मानम्);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    प्रत्यागम result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    जइ(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        प्रत्यागम static_cast<uint16_t>(trie->error_मानम्);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    प्रत्यागम result;
}

U_NAMESPACE_END

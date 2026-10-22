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
#அந்தர்பாவ "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#அந்தர்பாவ "unicode/umutablecptrie.h"
#endif
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf8.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "utrie2.h"
#அந்தர்பாவ "utrie2_impl.h"
#அந்தர்பாவ "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

நிலையான uint32_t
get32(மாறிலி UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    எனில்(c>=trie->உயர்வு_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        திரும்பு trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    எனில்(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } இல்லையெனில் {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    திரும்பு trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(மாறிலி UTrie2 *trie, UChar32 c) {
    எனில்(trie->data16!=nullptr) {
        திரும்பு UTRIE2_GET16(trie, c);
    } இல்லையெனில் எனில்(trie->data32!=nullptr) {
        திரும்பு UTRIE2_GET32(trie, c);
    } இல்லையெனில் எனில்((uint32_t)c>0x10ffff) {
        திரும்பு trie->error_மதிப்பு;
    } இல்லையெனில் {
        திரும்பு get32(trie->newTrie, c, உண்மை);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(மாறிலி UTrie2 *trie, UChar32 c) {
    எனில்(!U_IS_LEAD(c)) {
        திரும்பு trie->error_மதிப்பு;
    }
    எனில்(trie->data16!=nullptr) {
        திரும்பு UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } இல்லையெனில் எனில்(trie->data32!=nullptr) {
        திரும்பு UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } இல்லையெனில் {
        திரும்பு get32(trie->newTrie, c, பொய்);
    }
}

நிலையான உள்வரி int32_t
u8Index(மாறிலி UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    திரும்பு (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(மாறிலி UTrie2 *trie, UChar32 c,
                           மாறிலி uint8_t *src, மாறிலி uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    எனில்((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } இல்லையெனில் {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    திரும்பு u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(மாறிலி UTrie2 *trie, UChar32 c,
                           மாறிலி uint8_t *start, மாறிலி uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    எனில்((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } இல்லையெனில் {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    திரும்பு u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_மதிப்பு_Bits மதிப்பு_Bits,
                          மாறிலி வெறுமை *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    மாறிலி UTrie2Header *header;
    மாறிலி uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }

    எனில்( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        மதிப்பு_Bits<0 || UTRIE2_மொத்தம்_மதிப்பு_BITS<=மதிப்பு_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    /* enough data for a trie header? */
    எனில்(length<(int32_t)அளவு(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }

    /* check the signature */
    header=(மாறிலி UTrie2Header *)data;
    எனில்(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }

    /* get the options */
    எனில்(மதிப்பு_Bits!=(UTrie2_மதிப்பு_Bits)(header->options&UTRIE2_OPTIONS_மதிப்பு_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, அளவு(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.உயர்வு_Start=header->shifted_உயர்வு_Start<<UTRIE2_SHIFT_1;
    tempTrie.உயர்வு_மதிப்பு_Index=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    எனில்(மதிப்பு_Bits==UTRIE2_16_மதிப்பு_BITS) {
        tempTrie.உயர்வு_மதிப்பு_Index+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)அளவு(UTrie2Header)+tempTrie.indexLength*2;
    எனில்(மதிப்பு_Bits==UTRIE2_16_மதிப்பு_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } இல்லையெனில் {
        actualLength+=tempTrie.dataLength*4;
    }
    எனில்(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        திரும்பு nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(அளவு(UTrie2));
    எனில்(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    uprv_memcpy(trie, &tempTrie, அளவு(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=பொய்;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(மாறிலி uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    தேர்வு(மதிப்பு_Bits) {
    நிலை UTRIE2_16_மதிப்பு_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_மதிப்பு=trie->index[trie->dataNullOffset];
        trie->error_மதிப்பு=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        நிறுத்து;
    நிலை UTRIE2_32_மதிப்பு_BITS:
        trie->data16=nullptr;
        trie->data32=(மாறிலி uint32_t *)p16;
        trie->initial_மதிப்பு=trie->data32[trie->dataNullOffset];
        trie->error_மதிப்பு=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        நிறுத்து;
    இயல்பு:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு nullptr;
    }

    எனில்(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    திரும்பு trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_மதிப்பு_Bits மதிப்பு_Bits,
                 uint32_t initial_மதிப்பு, uint32_t error_மதிப்பு,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }

    எனில்(மதிப்பு_Bits<0 || UTRIE2_மொத்தம்_மதிப்பு_BITS<=மதிப்பு_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)அளவு(UTrie2Header)+indexLength*2;
    எனில்(மதிப்பு_Bits==UTRIE2_16_மதிப்பு_BITS) {
        length+=dataLength*2;
    } இல்லையெனில் {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(அளவு(UTrie2));
    எனில்(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    uprv_memset(trie, 0, அளவு(UTrie2));
    trie->memory=uprv_malloc(length);
    எனில்(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=உண்மை;

    /* set the UTrie2 fields */
    எனில்(மதிப்பு_Bits==UTRIE2_16_மதிப்பு_BITS) {
        dataMove=indexLength;
    } இல்லையெனில் {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initial_மதிப்பு=initial_மதிப்பு;
    trie->error_மதிப்பு=error_மதிப்பு;
    trie->உயர்வு_Start=0;
    trie->உயர்வு_மதிப்பு_Index=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)மதிப்பு_Bits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shifted_உயர்வு_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    க்கு(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    க்கு(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    க்கு(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    தேர்வு(மதிப்பு_Bits) {
    நிலை UTRIE2_16_மதிப்பு_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        க்கு(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_மதிப்பு;
        }
        க்கு(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_மதிப்பு;
        }
        /* highValue and reserved values */
        க்கு(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_மதிப்பு;
        }
        நிறுத்து;
    நிலை UTRIE2_32_மதிப்பு_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        க்கு(i=0; i<0x80; ++i) {
            *p++=initial_மதிப்பு;
        }
        க்கு(; i<0xc0; ++i) {
            *p++=error_மதிப்பு;
        }
        /* highValue and reserved values */
        க்கு(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initial_மதிப்பு;
        }
        நிறுத்து;
    இயல்பு:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

    திரும்பு trie;
}

U_CAPI வெறுமை U_EXPORT2
utrie2_close(UTrie2 *trie) {
    எனில்(trie!=nullptr) {
        எனில்(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        எனில்(trie->newTrie!=nullptr) {
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
utrie2_isFrozen(மாறிலி UTrie2 *trie) {
    திரும்பு trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(மாறிலி UTrie2 *trie,
                 வெறுமை *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    எனில்( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    எனில்(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } இல்லையெனில் {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    திரும்பு trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_மதிப்பு(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
நிலையான uint32_t U_CALLCONV
enum_Same_மதிப்பு(மாறிலி வெறுமை * /*context*/, uint32_t value) {
    திரும்பு value;
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
நிலையான வெறுமை
enumEitherTrie(மாறிலி UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_மதிப்பு *enum_மதிப்பு, UTrie2EnumRange *enumRange, மாறிலி வெறுமை *context) {
    மாறிலி uint32_t *data32;
    மாறிலி uint16_t *idx;

    uint32_t value, prev_மதிப்பு, initial_மதிப்பு;
    UChar32 c, prev, உயர்வு_Start;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    எனில்(enumRange==nullptr) {
        திரும்பு;
    }
    எனில்(enum_மதிப்பு==nullptr) {
        enum_மதிப்பு=enum_Same_மதிப்பு;
    }

    எனில்(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } இல்லையெனில் {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    உயர்வு_Start=trie->உயர்வு_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_மதிப்பு=enum_மதிப்பு(context, trie->initial_மதிப்பு);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prev_மதிப்பு=0;

    /* enumerate index-2 blocks */
    க்கு(c=start; c<limit && c<உயர்வு_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        எனில்(limit<tempLimit) {
            tempLimit=limit;
        }
        எனில்(c<=0xffff) {
            எனில்(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } இல்லையெனில் எனில்(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_மதிப்பு(0xdc00, limit);
            } இல்லையெனில் {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_மதிப்பு(0xe000, limit);
            }
        } இல்லையெனில் {
            /* supplementary code points */
            எனில்(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } இல்லையெனில் {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            எனில்(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                தொடர்;
            }
        }
        prevI2Block=i2Block;
        எனில்(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            எனில்(prev_மதிப்பு!=initial_மதிப்பு) {
                எனில்(prev<c && !enumRange(context, prev, c-1, prev_மதிப்பு)) {
                    திரும்பு;
                }
                prevBlock=nullBlock;
                prev=c;
                prev_மதிப்பு=initial_மதிப்பு;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } இல்லையெனில் {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            எனில்((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } இல்லையெனில் {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            க்கு(; i2<i2Limit; ++i2) {
                எனில்(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } இல்லையெனில் {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                எனில்(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    தொடர்;
                }
                prevBlock=block;
                எனில்(block==nullBlock) {
                    /* this is the null data block */
                    எனில்(prev_மதிப்பு!=initial_மதிப்பு) {
                        எனில்(prev<c && !enumRange(context, prev, c-1, prev_மதிப்பு)) {
                            திரும்பு;
                        }
                        prev=c;
                        prev_மதிப்பு=initial_மதிப்பு;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } இல்லையெனில் {
                    க்கு(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_மதிப்பு(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        எனில்(value!=prev_மதிப்பு) {
                            எனில்(prev<c && !enumRange(context, prev, c-1, prev_மதிப்பு)) {
                                திரும்பு;
                            }
                            prev=c;
                            prev_மதிப்பு=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    எனில்(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } இல்லையெனில் எனில்(c<limit) {
        /* c==highStart<limit */
        uint32_t உயர்வு_மதிப்பு;
        எனில்(idx!=nullptr) {
            உயர்வு_மதிப்பு=
                data32!=nullptr ?
                    data32[trie->உயர்வு_மதிப்பு_Index] :
                    idx[trie->உயர்வு_மதிப்பு_Index];
        } இல்லையெனில் {
            உயர்வு_மதிப்பு=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enum_மதிப்பு(context, உயர்வு_மதிப்பு);
        எனில்(value!=prev_மதிப்பு) {
            எனில்(prev<c && !enumRange(context, prev, c-1, prev_மதிப்பு)) {
                திரும்பு;
            }
            prev=c;
            prev_மதிப்பு=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_மதிப்பு);
}

U_CAPI வெறுமை U_EXPORT2
utrie2_enum(மாறிலி UTrie2 *trie,
            UTrie2_Enum_மதிப்பு *enum_மதிப்பு, UTrie2EnumRange *enumRange, மாறிலி வெறுமை *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_மதிப்பு, enumRange, context);
}

U_CAPI வெறுமை U_EXPORT2
utrie2_enumForLeadSurrogate(மாறிலி UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_மதிப்பு *enum_மதிப்பு, UTrie2EnumRange *enumRange,
                            மாறிலி வெறுமை *context) {
    எனில்(!U16_IS_LEAD(lead)) {
        திரும்பு;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_மதிப்பு, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    எனில்(start>=codePointStart) {
        codePoint=U_SENTINEL;
        திரும்பு static_cast<uint16_t>(trie->error_மதிப்பு);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    திரும்பு result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    எனில்(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        திரும்பு static_cast<uint16_t>(trie->error_மதிப்பு);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    திரும்பு result;
}

U_NAMESPACE_END

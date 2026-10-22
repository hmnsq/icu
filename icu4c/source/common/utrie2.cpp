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
#شامل "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#شامل "unicode/umutablecptrie.h"
#endif
#شامل "unicode/utf.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "utrie2.h"
#شامل "utrie2_impl.h"
#شامل "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

ایستا uint32_t
get32(ثابت UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    اگر(c>=trie->بالا_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        بازگشت trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    اگر(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } وگرنه {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    بازگشت trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ثابت UTrie2 *trie, UChar32 c) {
    اگر(trie->data16!=nullptr) {
        بازگشت UTRIE2_GET16(trie, c);
    } وگرنه اگر(trie->data32!=nullptr) {
        بازگشت UTRIE2_GET32(trie, c);
    } وگرنه اگر((uint32_t)c>0x10ffff) {
        بازگشت trie->error_مقدار;
    } وگرنه {
        بازگشت get32(trie->newTrie, c, درست);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(ثابت UTrie2 *trie, UChar32 c) {
    اگر(!U_IS_LEAD(c)) {
        بازگشت trie->error_مقدار;
    }
    اگر(trie->data16!=nullptr) {
        بازگشت UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } وگرنه اگر(trie->data32!=nullptr) {
        بازگشت UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } وگرنه {
        بازگشت get32(trie->newTrie, c, نادرست);
    }
}

ایستا درونخطی int32_t
u8Index(ثابت UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    بازگشت (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *src, ثابت uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    اگر((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } وگرنه {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    بازگشت u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *start, ثابت uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    اگر((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } وگرنه {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    بازگشت u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_مقدار_Bits مقدار_Bits,
                          ثابت تهی *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    ثابت UTrie2Header *header;
    ثابت uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت nullptr;
    }

    اگر( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        مقدار_Bits<0 || UTRIE2_شمار_مقدار_BITS<=مقدار_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    /* enough data for a trie header? */
    اگر(length<(int32_t)اندازه(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        بازگشت nullptr;
    }

    /* check the signature */
    header=(ثابت UTrie2Header *)data;
    اگر(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        بازگشت nullptr;
    }

    /* get the options */
    اگر(مقدار_Bits!=(UTrie2_مقدار_Bits)(header->options&UTRIE2_OPTIONS_مقدار_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        بازگشت nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, اندازه(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.بالا_Start=header->shifted_بالا_Start<<UTRIE2_SHIFT_1;
    tempTrie.بالا_مقدار_Index=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    اگر(مقدار_Bits==UTRIE2_16_مقدار_BITS) {
        tempTrie.بالا_مقدار_Index+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)اندازه(UTrie2Header)+tempTrie.indexLength*2;
    اگر(مقدار_Bits==UTRIE2_16_مقدار_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } وگرنه {
        actualLength+=tempTrie.dataLength*4;
    }
    اگر(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        بازگشت nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(اندازه(UTrie2));
    اگر(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    uprv_memcpy(trie, &tempTrie, اندازه(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=نادرست;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(ثابت uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    انتخاب(مقدار_Bits) {
    حالت UTRIE2_16_مقدار_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_مقدار=trie->index[trie->dataNullOffset];
        trie->error_مقدار=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        بشکن;
    حالت UTRIE2_32_مقدار_BITS:
        trie->data16=nullptr;
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_مقدار=trie->data32[trie->dataNullOffset];
        trie->error_مقدار=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        بشکن;
    پیشفرض:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        بازگشت nullptr;
    }

    اگر(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    بازگشت trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_مقدار_Bits مقدار_Bits,
                 uint32_t initial_مقدار, uint32_t error_مقدار,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت nullptr;
    }

    اگر(مقدار_Bits<0 || UTRIE2_شمار_مقدار_BITS<=مقدار_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)اندازه(UTrie2Header)+indexLength*2;
    اگر(مقدار_Bits==UTRIE2_16_مقدار_BITS) {
        length+=dataLength*2;
    } وگرنه {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(اندازه(UTrie2));
    اگر(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    uprv_memset(trie, 0, اندازه(UTrie2));
    trie->memory=uprv_malloc(length);
    اگر(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=درست;

    /* set the UTrie2 fields */
    اگر(مقدار_Bits==UTRIE2_16_مقدار_BITS) {
        dataMove=indexLength;
    } وگرنه {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initial_مقدار=initial_مقدار;
    trie->error_مقدار=error_مقدار;
    trie->بالا_Start=0;
    trie->بالا_مقدار_Index=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)مقدار_Bits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shifted_بالا_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    برای(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    برای(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    برای(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    انتخاب(مقدار_Bits) {
    حالت UTRIE2_16_مقدار_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        برای(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_مقدار;
        }
        برای(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_مقدار;
        }
        /* highValue and reserved values */
        برای(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_مقدار;
        }
        بشکن;
    حالت UTRIE2_32_مقدار_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        برای(i=0; i<0x80; ++i) {
            *p++=initial_مقدار;
        }
        برای(; i<0xc0; ++i) {
            *p++=error_مقدار;
        }
        /* highValue and reserved values */
        برای(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initial_مقدار;
        }
        بشکن;
    پیشفرض:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

    بازگشت trie;
}

U_CAPI تهی U_EXPORT2
utrie2_close(UTrie2 *trie) {
    اگر(trie!=nullptr) {
        اگر(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        اگر(trie->newTrie!=nullptr) {
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
utrie2_isFrozen(ثابت UTrie2 *trie) {
    بازگشت trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ثابت UTrie2 *trie,
                 تهی *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    اگر( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    اگر(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } وگرنه {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    بازگشت trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_مقدار(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ایستا uint32_t U_CALLCONV
enum_Same_مقدار(ثابت تهی * /*context*/, uint32_t value) {
    بازگشت value;
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
ایستا تهی
enumEitherTrie(ثابت UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_مقدار *enum_مقدار, UTrie2EnumRange *enumRange, ثابت تهی *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prev_مقدار, initial_مقدار;
    UChar32 c, prev, بالا_Start;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    اگر(enumRange==nullptr) {
        بازگشت;
    }
    اگر(enum_مقدار==nullptr) {
        enum_مقدار=enum_Same_مقدار;
    }

    اگر(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } وگرنه {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    بالا_Start=trie->بالا_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_مقدار=enum_مقدار(context, trie->initial_مقدار);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prev_مقدار=0;

    /* enumerate index-2 blocks */
    برای(c=start; c<limit && c<بالا_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        اگر(limit<tempLimit) {
            tempLimit=limit;
        }
        اگر(c<=0xffff) {
            اگر(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } وگرنه اگر(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_مقدار(0xdc00, limit);
            } وگرنه {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_مقدار(0xe000, limit);
            }
        } وگرنه {
            /* supplementary code points */
            اگر(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } وگرنه {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            اگر(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                ادامه;
            }
        }
        prevI2Block=i2Block;
        اگر(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            اگر(prev_مقدار!=initial_مقدار) {
                اگر(prev<c && !enumRange(context, prev, c-1, prev_مقدار)) {
                    بازگشت;
                }
                prevBlock=nullBlock;
                prev=c;
                prev_مقدار=initial_مقدار;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } وگرنه {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            اگر((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } وگرنه {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            برای(; i2<i2Limit; ++i2) {
                اگر(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } وگرنه {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                اگر(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    ادامه;
                }
                prevBlock=block;
                اگر(block==nullBlock) {
                    /* this is the null data block */
                    اگر(prev_مقدار!=initial_مقدار) {
                        اگر(prev<c && !enumRange(context, prev, c-1, prev_مقدار)) {
                            بازگشت;
                        }
                        prev=c;
                        prev_مقدار=initial_مقدار;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } وگرنه {
                    برای(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_مقدار(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        اگر(value!=prev_مقدار) {
                            اگر(prev<c && !enumRange(context, prev, c-1, prev_مقدار)) {
                                بازگشت;
                            }
                            prev=c;
                            prev_مقدار=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    اگر(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } وگرنه اگر(c<limit) {
        /* c==highStart<limit */
        uint32_t بالا_مقدار;
        اگر(idx!=nullptr) {
            بالا_مقدار=
                data32!=nullptr ?
                    data32[trie->بالا_مقدار_Index] :
                    idx[trie->بالا_مقدار_Index];
        } وگرنه {
            بالا_مقدار=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enum_مقدار(context, بالا_مقدار);
        اگر(value!=prev_مقدار) {
            اگر(prev<c && !enumRange(context, prev, c-1, prev_مقدار)) {
                بازگشت;
            }
            prev=c;
            prev_مقدار=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_مقدار);
}

U_CAPI تهی U_EXPORT2
utrie2_enum(ثابت UTrie2 *trie,
            UTrie2_Enum_مقدار *enum_مقدار, UTrie2EnumRange *enumRange, ثابت تهی *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_مقدار, enumRange, context);
}

U_CAPI تهی U_EXPORT2
utrie2_enumForLeadSurrogate(ثابت UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_مقدار *enum_مقدار, UTrie2EnumRange *enumRange,
                            ثابت تهی *context) {
    اگر(!U16_IS_LEAD(lead)) {
        بازگشت;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_مقدار, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    اگر(start>=codePointStart) {
        codePoint=U_SENTINEL;
        بازگشت static_cast<uint16_t>(trie->error_مقدار);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    بازگشت result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    اگر(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        بازگشت static_cast<uint16_t>(trie->error_مقدار);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    بازگشت result;
}

U_NAMESPACE_END

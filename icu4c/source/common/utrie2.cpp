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
#ضمن "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#ضمن "unicode/umutablecptrie.h"
#endif
#ضمن "unicode/utf.h"
#ضمن "unicode/utf8.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "utrie2.h"
#ضمن "utrie2_impl.h"
#ضمن "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

ساكن uint32_t
get32(ثابت UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    𐤀𐤌(c>=trie->highStart && (!U_IS_LEAD(c) || fromLSCP)) {
        أرجع trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    𐤀𐤌(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    أرجع trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ثابت UTrie2 *trie, UChar32 c) {
    𐤀𐤌(trie->data16!=nullptr) {
        أرجع UTRIE2_GET16(trie, c);
    } وإلا 𐤀𐤌(trie->data32!=nullptr) {
        أرجع UTRIE2_GET32(trie, c);
    } وإلا 𐤀𐤌((uint32_t)c>0x10ffff) {
        أرجع trie->errorValue;
    } وإلا {
        أرجع get32(trie->newTrie, c, true);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(ثابت UTrie2 *trie, UChar32 c) {
    𐤀𐤌(!U_IS_LEAD(c)) {
        أرجع trie->errorValue;
    }
    𐤀𐤌(trie->data16!=nullptr) {
        أرجع UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } وإلا 𐤀𐤌(trie->data32!=nullptr) {
        أرجع UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } وإلا {
        أرجع get32(trie->newTrie, c, خطأ);
    }
}

ساكن مضمن int32_t
u8Index(ثابت UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    أرجع (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *src, ثابت uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    𐤀𐤌((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } وإلا {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    أرجع u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *start, ثابت uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    𐤀𐤌((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } وإلا {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    أرجع u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2ValueBits valueBits,
                          ثابت فراغ *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    ثابت UTrie2Header *header;
    ثابت uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    𐤀𐤌(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }

    𐤀𐤌( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        valueBits<0 || UTRIE2_COUNT_VALUE_BITS<=valueBits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    /* enough data for a trie header? */
    𐤀𐤌(length<(int32_t)حجم(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }

    /* check the signature */
    header=(ثابت UTrie2Header *)data;
    𐤀𐤌(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }

    /* get the options */
    𐤀𐤌(valueBits!=(UTrie2ValueBits)(header->options&UTRIE2_OPTIONS_VALUE_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, حجم(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.highStart=header->shiftedHighStart<<UTRIE2_SHIFT_1;
    tempTrie.highValueIndex=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    𐤀𐤌(valueBits==UTRIE2_16_VALUE_BITS) {
        tempTrie.highValueIndex+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)حجم(UTrie2Header)+tempTrie.indexLength*2;
    𐤀𐤌(valueBits==UTRIE2_16_VALUE_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } وإلا {
        actualLength+=tempTrie.dataLength*4;
    }
    𐤀𐤌(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        أرجع nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    𐤀𐤌(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memcpy(trie, &tempTrie, حجم(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=خطأ;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(ثابت uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    بدل(valueBits) {
    حالة UTRIE2_16_VALUE_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initialValue=trie->index[trie->dataNullOffset];
        trie->errorValue=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        اكسر;
    حالة UTRIE2_32_VALUE_BITS:
        trie->data16=nullptr;
        trie->data32=(ثابت uint32_t *)p16;
        trie->initialValue=trie->data32[trie->dataNullOffset];
        trie->errorValue=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        اكسر;
    افتراضي:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        أرجع nullptr;
    }

    𐤀𐤌(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    أرجع trie;
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

    𐤀𐤌(U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }

    𐤀𐤌(valueBits<0 || UTRIE2_COUNT_VALUE_BITS<=valueBits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)حجم(UTrie2Header)+indexLength*2;
    𐤀𐤌(valueBits==UTRIE2_16_VALUE_BITS) {
        length+=dataLength*2;
    } وإلا {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    𐤀𐤌(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    uprv_memset(trie, 0, حجم(UTrie2));
    trie->memory=uprv_malloc(length);
    𐤀𐤌(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=true;

    /* set the UTrie2 fields */
    𐤀𐤌(valueBits==UTRIE2_16_VALUE_BITS) {
        dataMove=indexLength;
    } وإلا {
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
    لكل(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    بدل(valueBits) {
    حالة UTRIE2_16_VALUE_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        لكل(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initialValue;
        }
        لكل(; i<0xc0; ++i) {
            *dest16++=(uint16_t)errorValue;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initialValue;
        }
        اكسر;
    حالة UTRIE2_32_VALUE_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        لكل(i=0; i<0x80; ++i) {
            *p++=initialValue;
        }
        لكل(; i<0xc0; ++i) {
            *p++=errorValue;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initialValue;
        }
        اكسر;
    افتراضي:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
utrie2_close(UTrie2 *trie) {
    𐤀𐤌(trie!=nullptr) {
        𐤀𐤌(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        𐤀𐤌(trie->newTrie!=nullptr) {
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
    أرجع trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ثابت UTrie2 *trie,
                 فراغ *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    𐤀𐤌(U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    𐤀𐤌( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    𐤀𐤌(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } وإلا {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    أرجع trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_VALUE(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ساكن uint32_t U_CALLCONV
enumSameValue(ثابت فراغ * /*context*/, uint32_t value) {
    أرجع value;
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
ساكن فراغ
enumEitherTrie(ثابت UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2EnumValue *enumValue, UTrie2EnumRange *enumRange, ثابت فراغ *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prevValue, initialValue;
    UChar32 c, prev, highStart;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    𐤀𐤌(enumRange==nullptr) {
        أرجع;
    }
    𐤀𐤌(enumValue==nullptr) {
        enumValue=enumSameValue;
    }

    𐤀𐤌(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } وإلا {
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
    لكل(c=start; c<limit && c<highStart;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        𐤀𐤌(limit<tempLimit) {
            tempLimit=limit;
        }
        𐤀𐤌(c<=0xffff) {
            𐤀𐤌(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } وإلا 𐤀𐤌(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_VALUE(0xdc00, limit);
            } وإلا {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_VALUE(0xe000, limit);
            }
        } وإلا {
            /* supplementary code points */
            𐤀𐤌(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } وإلا {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            𐤀𐤌(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                استمر;
            }
        }
        prevI2Block=i2Block;
        𐤀𐤌(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            𐤀𐤌(prevValue!=initialValue) {
                𐤀𐤌(prev<c && !enumRange(context, prev, c-1, prevValue)) {
                    أرجع;
                }
                prevBlock=nullBlock;
                prev=c;
                prevValue=initialValue;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            𐤀𐤌((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } وإلا {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            لكل(; i2<i2Limit; ++i2) {
                𐤀𐤌(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } وإلا {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                𐤀𐤌(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    استمر;
                }
                prevBlock=block;
                𐤀𐤌(block==nullBlock) {
                    /* this is the null data block */
                    𐤀𐤌(prevValue!=initialValue) {
                        𐤀𐤌(prev<c && !enumRange(context, prev, c-1, prevValue)) {
                            أرجع;
                        }
                        prev=c;
                        prevValue=initialValue;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } وإلا {
                    لكل(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enumValue(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        𐤀𐤌(value!=prevValue) {
                            𐤀𐤌(prev<c && !enumRange(context, prev, c-1, prevValue)) {
                                أرجع;
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

    𐤀𐤌(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } وإلا 𐤀𐤌(c<limit) {
        /* c==highStart<limit */
        uint32_t highValue;
        𐤀𐤌(idx!=nullptr) {
            highValue=
                data32!=nullptr ?
                    data32[trie->highValueIndex] :
                    idx[trie->highValueIndex];
        } وإلا {
            highValue=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enumValue(context, highValue);
        𐤀𐤌(value!=prevValue) {
            𐤀𐤌(prev<c && !enumRange(context, prev, c-1, prevValue)) {
                أرجع;
            }
            prev=c;
            prevValue=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prevValue);
}

U_CAPI فراغ U_EXPORT2
utrie2_enum(ثابت UTrie2 *trie,
            UTrie2EnumValue *enumValue, UTrie2EnumRange *enumRange, ثابت فراغ *context) {
    enumEitherTrie(trie, 0, 0x110000, enumValue, enumRange, context);
}

U_CAPI فراغ U_EXPORT2
utrie2_enumForLeadSurrogate(ثابت UTrie2 *trie, UChar32 lead,
                            UTrie2EnumValue *enumValue, UTrie2EnumRange *enumRange,
                            ثابت فراغ *context) {
    𐤀𐤌(!U16_IS_LEAD(lead)) {
        أرجع;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enumValue, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    𐤀𐤌(start>=codePointStart) {
        codePoint=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->errorValue);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    أرجع result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    𐤀𐤌(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->errorValue);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    أرجع result;
}

U_NAMESPACE_END

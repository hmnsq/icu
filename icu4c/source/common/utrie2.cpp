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
get32(अचर UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    यदि(c>=trie->माथिल्लो_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        फर्काऊ trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    यदि(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } अन्यथा {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    फर्काऊ trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(अचर UTrie2 *trie, UChar32 c) {
    यदि(trie->data16!=nullptr) {
        फर्काऊ UTRIE2_GET16(trie, c);
    } अन्यथा यदि(trie->data32!=nullptr) {
        फर्काऊ UTRIE2_GET32(trie, c);
    } अन्यथा यदि((uint32_t)c>0x10ffff) {
        फर्काऊ trie->error_मान;
    } अन्यथा {
        फर्काऊ get32(trie->newTrie, c, सत्य);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(अचर UTrie2 *trie, UChar32 c) {
    यदि(!U_IS_LEAD(c)) {
        फर्काऊ trie->error_मान;
    }
    यदि(trie->data16!=nullptr) {
        फर्काऊ UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } अन्यथा यदि(trie->data32!=nullptr) {
        फर्काऊ UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } अन्यथा {
        फर्काऊ get32(trie->newTrie, c, असत्य);
    }
}

स्थिर अन्तःस्थ int32_t
u8Index(अचर UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    फर्काऊ (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(अचर UTrie2 *trie, UChar32 c,
                           अचर uint8_t *src, अचर uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    यदि((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } अन्यथा {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    फर्काऊ u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(अचर UTrie2 *trie, UChar32 c,
                           अचर uint8_t *start, अचर uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    यदि((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } अन्यथा {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    फर्काऊ u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_मान_Bits मान_Bits,
                          अचर शून्य *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    अचर UTrie2Header *header;
    अचर uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    यदि(U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }

    यदि( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        मान_Bits<0 || UTRIE2_गन्ती_मान_BITS<=मान_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }

    /* enough data for a trie header? */
    यदि(length<(int32_t)परिमाण(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
    }

    /* check the signature */
    header=(अचर UTrie2Header *)data;
    यदि(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
    }

    /* get the options */
    यदि(मान_Bits!=(UTrie2_मान_Bits)(header->options&UTRIE2_OPTIONS_मान_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, परिमाण(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.माथिल्लो_Start=header->shifted_माथिल्लो_Start<<UTRIE2_SHIFT_1;
    tempTrie.माथिल्लो_मान_Index=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    यदि(मान_Bits==UTRIE2_16_मान_BITS) {
        tempTrie.माथिल्लो_मान_Index+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)परिमाण(UTrie2Header)+tempTrie.indexLength*2;
    यदि(मान_Bits==UTRIE2_16_मान_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } अन्यथा {
        actualLength+=tempTrie.dataLength*4;
    }
    यदि(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        फर्काऊ nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    यदि(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, परिमाण(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=असत्य;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(अचर uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    विकल्प(मान_Bits) {
    पक्ष UTRIE2_16_मान_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_मान=trie->index[trie->dataNullOffset];
        trie->error_मान=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        रोक;
    पक्ष UTRIE2_32_मान_BITS:
        trie->data16=nullptr;
        trie->data32=(अचर uint32_t *)p16;
        trie->initial_मान=trie->data32[trie->dataNullOffset];
        trie->error_मान=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        रोक;
    स्वभाव:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        फर्काऊ nullptr;
    }

    यदि(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    फर्काऊ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_मान_Bits मान_Bits,
                 uint32_t initial_मान, uint32_t error_मान,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    यदि(U_FAILURE(*pErrorCode)) {
        फर्काऊ nullptr;
    }

    यदि(मान_Bits<0 || UTRIE2_गन्ती_मान_BITS<=मान_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)परिमाण(UTrie2Header)+indexLength*2;
    यदि(मान_Bits==UTRIE2_16_मान_BITS) {
        length+=dataLength*2;
    } अन्यथा {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    यदि(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    uprv_memset(trie, 0, परिमाण(UTrie2));
    trie->memory=uprv_malloc(length);
    यदि(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=सत्य;

    /* set the UTrie2 fields */
    यदि(मान_Bits==UTRIE2_16_मान_BITS) {
        dataMove=indexLength;
    } अन्यथा {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initial_मान=initial_मान;
    trie->error_मान=error_मान;
    trie->माथिल्लो_Start=0;
    trie->माथिल्लो_मान_Index=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)मान_Bits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shifted_माथिल्लो_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    लागि(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    लागि(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    लागि(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    विकल्प(मान_Bits) {
    पक्ष UTRIE2_16_मान_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        लागि(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_मान;
        }
        लागि(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_मान;
        }
        /* highValue and reserved values */
        लागि(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_मान;
        }
        रोक;
    पक्ष UTRIE2_32_मान_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        लागि(i=0; i<0x80; ++i) {
            *p++=initial_मान;
        }
        लागि(; i<0xc0; ++i) {
            *p++=error_मान;
        }
        /* highValue and reserved values */
        लागि(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initial_मान;
        }
        रोक;
    स्वभाव:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }

    फर्काऊ trie;
}

U_CAPI शून्य U_EXPORT2
utrie2_close(UTrie2 *trie) {
    यदि(trie!=nullptr) {
        यदि(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        यदि(trie->newTrie!=nullptr) {
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
utrie2_isFrozen(अचर UTrie2 *trie) {
    फर्काऊ trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(अचर UTrie2 *trie,
                 शून्य *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    यदि(U_FAILURE(*pErrorCode)) {
        फर्काऊ 0;
    }

    यदि( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    यदि(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } अन्यथा {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    फर्काऊ trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_मान(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
स्थिर uint32_t U_CALLCONV
enum_Same_मान(अचर शून्य * /*context*/, uint32_t value) {
    फर्काऊ value;
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
enumEitherTrie(अचर UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_मान *enum_मान, UTrie2EnumRange *enumRange, अचर शून्य *context) {
    अचर uint32_t *data32;
    अचर uint16_t *idx;

    uint32_t value, prev_मान, initial_मान;
    UChar32 c, prev, माथिल्लो_Start;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    यदि(enumRange==nullptr) {
        फर्काऊ;
    }
    यदि(enum_मान==nullptr) {
        enum_मान=enum_Same_मान;
    }

    यदि(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } अन्यथा {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    माथिल्लो_Start=trie->माथिल्लो_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_मान=enum_मान(context, trie->initial_मान);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prev_मान=0;

    /* enumerate index-2 blocks */
    लागि(c=start; c<limit && c<माथिल्लो_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        यदि(limit<tempLimit) {
            tempLimit=limit;
        }
        यदि(c<=0xffff) {
            यदि(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } अन्यथा यदि(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_मान(0xdc00, limit);
            } अन्यथा {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_मान(0xe000, limit);
            }
        } अन्यथा {
            /* supplementary code points */
            यदि(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } अन्यथा {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            यदि(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                जारी;
            }
        }
        prevI2Block=i2Block;
        यदि(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            यदि(prev_मान!=initial_मान) {
                यदि(prev<c && !enumRange(context, prev, c-1, prev_मान)) {
                    फर्काऊ;
                }
                prevBlock=nullBlock;
                prev=c;
                prev_मान=initial_मान;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } अन्यथा {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            यदि((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } अन्यथा {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            लागि(; i2<i2Limit; ++i2) {
                यदि(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } अन्यथा {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                यदि(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    जारी;
                }
                prevBlock=block;
                यदि(block==nullBlock) {
                    /* this is the null data block */
                    यदि(prev_मान!=initial_मान) {
                        यदि(prev<c && !enumRange(context, prev, c-1, prev_मान)) {
                            फर्काऊ;
                        }
                        prev=c;
                        prev_मान=initial_मान;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } अन्यथा {
                    लागि(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_मान(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        यदि(value!=prev_मान) {
                            यदि(prev<c && !enumRange(context, prev, c-1, prev_मान)) {
                                फर्काऊ;
                            }
                            prev=c;
                            prev_मान=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    यदि(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } अन्यथा यदि(c<limit) {
        /* c==highStart<limit */
        uint32_t माथिल्लो_मान;
        यदि(idx!=nullptr) {
            माथिल्लो_मान=
                data32!=nullptr ?
                    data32[trie->माथिल्लो_मान_Index] :
                    idx[trie->माथिल्लो_मान_Index];
        } अन्यथा {
            माथिल्लो_मान=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enum_मान(context, माथिल्लो_मान);
        यदि(value!=prev_मान) {
            यदि(prev<c && !enumRange(context, prev, c-1, prev_मान)) {
                फर्काऊ;
            }
            prev=c;
            prev_मान=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_मान);
}

U_CAPI शून्य U_EXPORT2
utrie2_enum(अचर UTrie2 *trie,
            UTrie2_Enum_मान *enum_मान, UTrie2EnumRange *enumRange, अचर शून्य *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_मान, enumRange, context);
}

U_CAPI शून्य U_EXPORT2
utrie2_enumForLeadSurrogate(अचर UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_मान *enum_मान, UTrie2EnumRange *enumRange,
                            अचर शून्य *context) {
    यदि(!U16_IS_LEAD(lead)) {
        फर्काऊ;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_मान, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    यदि(start>=codePointStart) {
        codePoint=U_SENTINEL;
        फर्काऊ static_cast<uint16_t>(trie->error_मान);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    फर्काऊ result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    यदि(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        फर्काऊ static_cast<uint16_t>(trie->error_मान);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    फर्काऊ result;
}

U_NAMESPACE_END

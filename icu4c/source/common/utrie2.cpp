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
#অন্তর্ভাব "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#অন্তর্ভাব "unicode/umutablecptrie.h"
#endif
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf8.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "utrie2.h"
#অন্তর্ভাব "utrie2_impl.h"
#অন্তর্ভাব "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

স্থির uint32_t
get32(ধ্রুব UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    যদি(c>=trie->উপরের_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        প্রত্যাগম trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    যদি(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } অন্যথা {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    প্রত্যাগম trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ধ্রুব UTrie2 *trie, UChar32 c) {
    যদি(trie->data16!=nullptr) {
        প্রত্যাগম UTRIE2_GET16(trie, c);
    } অন্যথা যদি(trie->data32!=nullptr) {
        প্রত্যাগম UTRIE2_GET32(trie, c);
    } অন্যথা যদি((uint32_t)c>0x10ffff) {
        প্রত্যাগম trie->error_মান;
    } অন্যথা {
        প্রত্যাগম get32(trie->newTrie, c, সত্য);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(ধ্রুব UTrie2 *trie, UChar32 c) {
    যদি(!U_IS_LEAD(c)) {
        প্রত্যাগম trie->error_মান;
    }
    যদি(trie->data16!=nullptr) {
        প্রত্যাগম UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } অন্যথা যদি(trie->data32!=nullptr) {
        প্রত্যাগম UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } অন্যথা {
        প্রত্যাগম get32(trie->newTrie, c, অসত্য);
    }
}

স্থির অন্তঃস্থ int32_t
u8Index(ধ্রুব UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    প্রত্যাগম (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(ধ্রুব UTrie2 *trie, UChar32 c,
                           ধ্রুব uint8_t *src, ধ্রুব uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    যদি((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } অন্যথা {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    প্রত্যাগম u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(ধ্রুব UTrie2 *trie, UChar32 c,
                           ধ্রুব uint8_t *start, ধ্রুব uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    যদি((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } অন্যথা {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    প্রত্যাগম u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_মান_Bits মান_Bits,
                          ধ্রুব শূন্য *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    ধ্রুব UTrie2Header *header;
    ধ্রুব uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম nullptr;
    }

    যদি( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        মান_Bits<0 || UTRIE2_গুনতি_মান_BITS<=মান_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }

    /* enough data for a trie header? */
    যদি(length<(int32_t)পরিমাণ(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        প্রত্যাগম nullptr;
    }

    /* check the signature */
    header=(ধ্রুব UTrie2Header *)data;
    যদি(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        প্রত্যাগম nullptr;
    }

    /* get the options */
    যদি(মান_Bits!=(UTrie2_মান_Bits)(header->options&UTRIE2_OPTIONS_মান_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        প্রত্যাগম nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, পরিমাণ(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.উপরের_Start=header->shifted_উপরের_Start<<UTRIE2_SHIFT_1;
    tempTrie.উপরের_মান_Index=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        tempTrie.উপরের_মান_Index+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)পরিমাণ(UTrie2Header)+tempTrie.indexLength*2;
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } অন্যথা {
        actualLength+=tempTrie.dataLength*4;
    }
    যদি(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        প্রত্যাগম nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(পরিমাণ(UTrie2));
    যদি(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম nullptr;
    }
    uprv_memcpy(trie, &tempTrie, পরিমাণ(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=অসত্য;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(ধ্রুব uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    বিকল্প(মান_Bits) {
    পক্ষ UTRIE2_16_মান_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_মান=trie->index[trie->dataNullOffset];
        trie->error_মান=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        ভঙ্গ;
    পক্ষ UTRIE2_32_মান_BITS:
        trie->data16=nullptr;
        trie->data32=(ধ্রুব uint32_t *)p16;
        trie->initial_মান=trie->data32[trie->dataNullOffset];
        trie->error_মান=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        ভঙ্গ;
    স্বভাব:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        প্রত্যাগম nullptr;
    }

    যদি(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    প্রত্যাগম trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_মান_Bits মান_Bits,
                 uint32_t initial_মান, uint32_t error_মান,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম nullptr;
    }

    যদি(মান_Bits<0 || UTRIE2_গুনতি_মান_BITS<=মান_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)পরিমাণ(UTrie2Header)+indexLength*2;
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        length+=dataLength*2;
    } অন্যথা {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(পরিমাণ(UTrie2));
    যদি(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম nullptr;
    }
    uprv_memset(trie, 0, পরিমাণ(UTrie2));
    trie->memory=uprv_malloc(length);
    যদি(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=সত্য;

    /* set the UTrie2 fields */
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        dataMove=indexLength;
    } অন্যথা {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initial_মান=initial_মান;
    trie->error_মান=error_মান;
    trie->উপরের_Start=0;
    trie->উপরের_মান_Index=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)মান_Bits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shifted_উপরের_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    প্রতি(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    প্রতি(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    প্রতি(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    বিকল্প(মান_Bits) {
    পক্ষ UTRIE2_16_মান_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        প্রতি(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_মান;
        }
        প্রতি(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_মান;
        }
        /* highValue and reserved values */
        প্রতি(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_মান;
        }
        ভঙ্গ;
    পক্ষ UTRIE2_32_মান_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        প্রতি(i=0; i<0x80; ++i) {
            *p++=initial_মান;
        }
        প্রতি(; i<0xc0; ++i) {
            *p++=error_মান;
        }
        /* highValue and reserved values */
        প্রতি(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initial_মান;
        }
        ভঙ্গ;
    স্বভাব:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }

    প্রত্যাগম trie;
}

U_CAPI শূন্য U_EXPORT2
utrie2_close(UTrie2 *trie) {
    যদি(trie!=nullptr) {
        যদি(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        যদি(trie->newTrie!=nullptr) {
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
utrie2_isFrozen(ধ্রুব UTrie2 *trie) {
    প্রত্যাগম trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ধ্রুব UTrie2 *trie,
                 শূন্য *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    }

    যদি( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    যদি(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } অন্যথা {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    প্রত্যাগম trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_মান(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
স্থির uint32_t U_CALLCONV
enum_Same_মান(ধ্রুব শূন্য * /*context*/, uint32_t value) {
    প্রত্যাগম value;
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
স্থির শূন্য
enumEitherTrie(ধ্রুব UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_মান *enum_মান, UTrie2EnumRange *enumRange, ধ্রুব শূন্য *context) {
    ধ্রুব uint32_t *data32;
    ধ্রুব uint16_t *idx;

    uint32_t value, prev_মান, initial_মান;
    UChar32 c, prev, উপরের_Start;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    যদি(enumRange==nullptr) {
        প্রত্যাগম;
    }
    যদি(enum_মান==nullptr) {
        enum_মান=enum_Same_মান;
    }

    যদি(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } অন্যথা {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    উপরের_Start=trie->উপরের_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_মান=enum_মান(context, trie->initial_মান);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prev_মান=0;

    /* enumerate index-2 blocks */
    প্রতি(c=start; c<limit && c<উপরের_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        যদি(limit<tempLimit) {
            tempLimit=limit;
        }
        যদি(c<=0xffff) {
            যদি(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } অন্যথা যদি(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_মান(0xdc00, limit);
            } অন্যথা {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_মান(0xe000, limit);
            }
        } অন্যথা {
            /* supplementary code points */
            যদি(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } অন্যথা {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            যদি(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                অনুবর্ত;
            }
        }
        prevI2Block=i2Block;
        যদি(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            যদি(prev_মান!=initial_মান) {
                যদি(prev<c && !enumRange(context, prev, c-1, prev_মান)) {
                    প্রত্যাগম;
                }
                prevBlock=nullBlock;
                prev=c;
                prev_মান=initial_মান;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } অন্যথা {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            যদি((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } অন্যথা {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            প্রতি(; i2<i2Limit; ++i2) {
                যদি(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } অন্যথা {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                যদি(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    অনুবর্ত;
                }
                prevBlock=block;
                যদি(block==nullBlock) {
                    /* this is the null data block */
                    যদি(prev_মান!=initial_মান) {
                        যদি(prev<c && !enumRange(context, prev, c-1, prev_মান)) {
                            প্রত্যাগম;
                        }
                        prev=c;
                        prev_মান=initial_মান;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } অন্যথা {
                    প্রতি(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_মান(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        যদি(value!=prev_মান) {
                            যদি(prev<c && !enumRange(context, prev, c-1, prev_মান)) {
                                প্রত্যাগম;
                            }
                            prev=c;
                            prev_মান=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    যদি(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } অন্যথা যদি(c<limit) {
        /* c==highStart<limit */
        uint32_t উপরের_মান;
        যদি(idx!=nullptr) {
            উপরের_মান=
                data32!=nullptr ?
                    data32[trie->উপরের_মান_Index] :
                    idx[trie->উপরের_মান_Index];
        } অন্যথা {
            উপরের_মান=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enum_মান(context, উপরের_মান);
        যদি(value!=prev_মান) {
            যদি(prev<c && !enumRange(context, prev, c-1, prev_মান)) {
                প্রত্যাগম;
            }
            prev=c;
            prev_মান=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_মান);
}

U_CAPI শূন্য U_EXPORT2
utrie2_enum(ধ্রুব UTrie2 *trie,
            UTrie2_Enum_মান *enum_মান, UTrie2EnumRange *enumRange, ধ্রুব শূন্য *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_মান, enumRange, context);
}

U_CAPI শূন্য U_EXPORT2
utrie2_enumForLeadSurrogate(ধ্রুব UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_মান *enum_মান, UTrie2EnumRange *enumRange,
                            ধ্রুব শূন্য *context) {
    যদি(!U16_IS_LEAD(lead)) {
        প্রত্যাগম;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_মান, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    যদি(start>=codePointStart) {
        codePoint=U_SENTINEL;
        প্রত্যাগম static_cast<uint16_t>(trie->error_মান);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    প্রত্যাগম result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    যদি(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        প্রত্যাগম static_cast<uint16_t>(trie->error_মান);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    প্রত্যাগম result;
}

U_NAMESPACE_END

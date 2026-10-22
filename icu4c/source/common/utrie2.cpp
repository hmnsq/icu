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
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ifdef UCPTRIE_DEBUG
#ಅನ್ತರ್ಭಾವ "unicode/umutablecptrie.h"
#endif
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "utrie2.h"
#ಅನ್ತರ್ಭಾವ "utrie2_impl.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

ಸ್ಥಿರ uint32_t
get32(ಧ್ರುವ UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    ಯದಿ(c>=trie->ಮೇಲಿನ_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        ಪ್ರತ್ಯಾಗಮ trie->data[trie->dataLength-UTRIE2_DATA_GRANULARITY];
    }

    ಯದಿ(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } ಅನ್ಯಥಾ {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK);
    }
    block=trie->index2[i2];
    ಪ್ರತ್ಯಾಗಮ trie->data[block+(c&UTRIE2_DATA_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ಧ್ರುವ UTrie2 *trie, UChar32 c) {
    ಯದಿ(trie->data16!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ UTRIE2_GET16(trie, c);
    } ಅನ್ಯಥಾ ಯದಿ(trie->data32!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ UTRIE2_GET32(trie, c);
    } ಅನ್ಯಥಾ ಯದಿ((uint32_t)c>0x10ffff) {
        ಪ್ರತ್ಯಾಗಮ trie->error_ಮೌಲ್ಯ;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ get32(trie->newTrie, c, ಸತ್ಯ);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(ಧ್ರುವ UTrie2 *trie, UChar32 c) {
    ಯದಿ(!U_IS_LEAD(c)) {
        ಪ್ರತ್ಯಾಗಮ trie->error_ಮೌಲ್ಯ;
    }
    ಯದಿ(trie->data16!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } ಅನ್ಯಥಾ ಯದಿ(trie->data32!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ get32(trie->newTrie, c, ಅಸತ್ಯ);
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ int32_t
u8Index(ಧ್ರುವ UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        _UTRIE2_INDEX_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->indexLength : 0,
            c);
    ಪ್ರತ್ಯಾಗಮ (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8NextIndex(ಧ್ರುವ UTrie2 *trie, UChar32 c,
                           ಧ್ರುವ uint8_t *src, ಧ್ರುವ uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ಯದಿ((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } ಅನ್ಯಥಾ {
        length=7;
    }
    c=utf8_nextCharSafeBody(src, &i, length, c, -1);
    ಪ್ರತ್ಯಾಗಮ u8Index(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internalU8PrevIndex(ಧ್ರುವ UTrie2 *trie, UChar32 c,
                           ಧ್ರುವ uint8_t *start, ಧ್ರುವ uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ಯದಿ((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } ಅನ್ಯಥಾ {
        i=length=7;
        start=src-7;
    }
    c=utf8_prevCharSafeBody(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    ಪ್ರತ್ಯಾಗಮ u8Index(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_ಮೌಲ್ಯ_Bits ಮೌಲ್ಯ_Bits,
                          ಧ್ರುವ ಶೂನ್ಯ *data, int32_t length, int32_t *pActualLength,
                          UErrorCode *pErrorCode) {
    ಧ್ರುವ UTrie2Header *header;
    ಧ್ರುವ uint16_t *p16;
    int32_t actualLength;

    UTrie2 tempTrie;
    UTrie2 *trie;

    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ( length<=0 || (U_POINTER_MASK_LSB(data, 3)!=0) ||
        ಮೌಲ್ಯ_Bits<0 || UTRIE2_ಎಣಿಕೆ_ಮೌಲ್ಯ_BITS<=ಮೌಲ್ಯ_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* enough data for a trie header? */
    ಯದಿ(length<(int32_t)ಪರಿಮಾಣ(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* check the signature */
    header=(ಧ್ರುವ UTrie2Header *)data;
    ಯದಿ(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* get the options */
    ಯದಿ(ಮೌಲ್ಯ_Bits!=(UTrie2_ಮೌಲ್ಯ_Bits)(header->options&UTRIE2_OPTIONS_ಮೌಲ್ಯ_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&tempTrie, 0, ಪರಿಮಾಣ(tempTrie));
    tempTrie.indexLength=header->indexLength;
    tempTrie.dataLength=header->shiftedDataLength<<UTRIE2_INDEX_SHIFT;
    tempTrie.index2NullOffset=header->index2NullOffset;
    tempTrie.dataNullOffset=header->dataNullOffset;

    tempTrie.ಮೇಲಿನ_Start=header->shifted_ಮೇಲಿನ_Start<<UTRIE2_SHIFT_1;
    tempTrie.ಮೇಲಿನ_ಮೌಲ್ಯ_Index=tempTrie.dataLength-UTRIE2_DATA_GRANULARITY;
    ಯದಿ(ಮೌಲ್ಯ_Bits==UTRIE2_16_ಮೌಲ್ಯ_BITS) {
        tempTrie.ಮೇಲಿನ_ಮೌಲ್ಯ_Index+=tempTrie.indexLength;
    }

    /* calculate the actual length */
    actualLength=(int32_t)ಪರಿಮಾಣ(UTrie2Header)+tempTrie.indexLength*2;
    ಯದಿ(ಮೌಲ್ಯ_Bits==UTRIE2_16_ಮೌಲ್ಯ_BITS) {
        actualLength+=tempTrie.dataLength*2;
    } ಅನ್ಯಥಾ {
        actualLength+=tempTrie.dataLength*4;
    }
    ಯದಿ(length<actualLength) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(ಪರಿಮಾಣ(UTrie2));
    ಯದಿ(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memcpy(trie, &tempTrie, ಪರಿಮಾಣ(tempTrie));
    trie->memory=(uint32_t *)data;
    trie->length=actualLength;
    trie->isMemoryOwned=ಅಸತ್ಯ;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(ಧ್ರುವ uint16_t *)(header+1);
    trie->index=p16;
    p16+=trie->indexLength;

    /* get the data */
    ವಿಕಲ್ಪ(ಮೌಲ್ಯ_Bits) {
    ಪಕ್ಷ UTRIE2_16_ಮೌಲ್ಯ_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_ಮೌಲ್ಯ=trie->index[trie->dataNullOffset];
        trie->error_ಮೌಲ್ಯ=trie->data16[UTRIE2_BAD_UTF8_DATA_OFFSET];
        ಭಙ್ಗ;
    ಪಕ್ಷ UTRIE2_32_ಮೌಲ್ಯ_BITS:
        trie->data16=nullptr;
        trie->data32=(ಧ್ರುವ uint32_t *)p16;
        trie->initial_ಮೌಲ್ಯ=trie->data32[trie->dataNullOffset];
        trie->error_ಮೌಲ್ಯ=trie->data32[UTRIE2_BAD_UTF8_DATA_OFFSET];
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ(pActualLength!=nullptr) {
        *pActualLength=actualLength;
    }
    ಪ್ರತ್ಯಾಗಮ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_ಮೌಲ್ಯ_Bits ಮೌಲ್ಯ_Bits,
                 uint32_t initial_ಮೌಲ್ಯ, uint32_t error_ಮೌಲ್ಯ,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t indexLength, dataLength, length, i;
    int32_t dataMove;  /* >0 if the data is moved to the end of the index array */

    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ(ಮೌಲ್ಯ_Bits<0 || UTRIE2_ಎಣಿಕೆ_ಮೌಲ್ಯ_BITS<=ಮೌಲ್ಯ_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    /* calculate the total length of the dummy trie data */
    indexLength=UTRIE2_INDEX_1_OFFSET;
    dataLength=UTRIE2_DATA_START_OFFSET+UTRIE2_DATA_GRANULARITY;
    length=(int32_t)ಪರಿಮಾಣ(UTrie2Header)+indexLength*2;
    ಯದಿ(ಮೌಲ್ಯ_Bits==UTRIE2_16_ಮೌಲ್ಯ_BITS) {
        length+=dataLength*2;
    } ಅನ್ಯಥಾ {
        length+=dataLength*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(ಪರಿಮಾಣ(UTrie2));
    ಯದಿ(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    uprv_memset(trie, 0, ಪರಿಮಾಣ(UTrie2));
    trie->memory=uprv_malloc(length);
    ಯದಿ(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=ಸತ್ಯ;

    /* set the UTrie2 fields */
    ಯದಿ(ಮೌಲ್ಯ_Bits==UTRIE2_16_ಮೌಲ್ಯ_BITS) {
        dataMove=indexLength;
    } ಅನ್ಯಥಾ {
        dataMove=0;
    }

    trie->indexLength=indexLength;
    trie->dataLength=dataLength;
    trie->index2NullOffset=UTRIE2_INDEX_2_OFFSET;
    trie->dataNullOffset=(uint16_t)dataMove;
    trie->initial_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
    trie->error_ಮೌಲ್ಯ=error_ಮೌಲ್ಯ;
    trie->ಮೇಲಿನ_Start=0;
    trie->ಮೇಲಿನ_ಮೌಲ್ಯ_Index=dataMove+UTRIE2_DATA_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)ಮೌಲ್ಯ_Bits;

    header->indexLength=(uint16_t)indexLength;
    header->shiftedDataLength=(uint16_t)(dataLength>>UTRIE2_INDEX_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_INDEX_2_OFFSET;
    header->dataNullOffset=(uint16_t)dataMove;
    header->shifted_ಮೇಲಿನ_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->index=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    ಪ್ರತಿ(i=0; i<UTRIE2_INDEX_2_BMP_LENGTH; ++i) {
        *dest16++=(uint16_t)(dataMove>>UTRIE2_INDEX_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    ಪ್ರತಿ(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(dataMove+UTRIE2_BAD_UTF8_DATA_OFFSET);
    }
    ಪ್ರತಿ(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)dataMove;
    }

    /* write the 16/32-bit data array */
    ವಿಕಲ್ಪ(ಮೌಲ್ಯ_Bits) {
    ಪಕ್ಷ UTRIE2_16_ಮೌಲ್ಯ_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        ಪ್ರತಿ(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_ಮೌಲ್ಯ;
        }
        ಪ್ರತಿ(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_ಮೌಲ್ಯ;
        }
        /* highValue and reserved values */
        ಪ್ರತಿ(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_ಮೌಲ್ಯ;
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ UTRIE2_32_ಮೌಲ್ಯ_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        ಪ್ರತಿ(i=0; i<0x80; ++i) {
            *p++=initial_ಮೌಲ್ಯ;
        }
        ಪ್ರತಿ(; i<0xc0; ++i) {
            *p++=error_ಮೌಲ್ಯ;
        }
        /* highValue and reserved values */
        ಪ್ರತಿ(i=0; i<UTRIE2_DATA_GRANULARITY; ++i) {
            *p++=initial_ಮೌಲ್ಯ;
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಪ್ರತ್ಯಾಗಮ trie;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_close(UTrie2 *trie) {
    ಯದಿ(trie!=nullptr) {
        ಯದಿ(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        ಯದಿ(trie->newTrie!=nullptr) {
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
utrie2_isFrozen(ಧ್ರುವ UTrie2 *trie) {
    ಪ್ರತ್ಯಾಗಮ trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ಧ್ರುವ UTrie2 *trie,
                 ಶೂನ್ಯ *data, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (data==nullptr || (U_POINTER_MASK_LSB(data, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(capacity>=trie->length) {
        uprv_memcpy(data, trie->memory, trie->length);
    } ಅನ್ಯಥಾ {
        *pErrorCode=U_BUFFER_OVERFLOW_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_ಮೌಲ್ಯ(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ಸ್ಥಿರ uint32_t U_CALLCONV
enum_Same_ಮೌಲ್ಯ(ಧ್ರುವ ಶೂನ್ಯ * /*context*/, uint32_t value) {
    ಪ್ರತ್ಯಾಗಮ value;
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
ಸ್ಥಿರ ಶೂನ್ಯ
enumEitherTrie(ಧ್ರುವ UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_ಮೌಲ್ಯ *enum_ಮೌಲ್ಯ, UTrie2EnumRange *enumRange, ಧ್ರುವ ಶೂನ್ಯ *context) {
    ಧ್ರುವ uint32_t *data32;
    ಧ್ರುವ uint16_t *idx;

    uint32_t value, prev_ಮೌಲ್ಯ, initial_ಮೌಲ್ಯ;
    UChar32 c, prev, ಮೇಲಿನ_Start;
    int32_t j, i2Block, prevI2Block, index2NullOffset, block, prevBlock, nullBlock;

    ಯದಿ(enumRange==nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(enum_ಮೌಲ್ಯ==nullptr) {
        enum_ಮೌಲ್ಯ=enum_Same_ಮೌಲ್ಯ;
    }

    ಯದಿ(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->index;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->dataNullOffset;
    } ಅನ್ಯಥಾ {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->data;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->dataNullOffset;
    }

    ಮೇಲಿನ_Start=trie->ಮೇಲಿನ_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_ಮೌಲ್ಯ=enum_ಮೌಲ್ಯ(context, trie->initial_ಮೌಲ್ಯ);

    /* set variables for previous range */
    prevI2Block=-1;
    prevBlock=-1;
    prev=start;
    prev_ಮೌಲ್ಯ=0;

    /* enumerate index-2 blocks */
    ಪ್ರತಿ(c=start; c<limit && c<ಮೇಲಿನ_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 tempLimit=c+UTRIE2_CP_PER_INDEX_1_ENTRY;
        ಯದಿ(limit<tempLimit) {
            tempLimit=limit;
        }
        ಯದಿ(c<=0xffff) {
            ಯದಿ(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } ಅನ್ಯಥಾ ಯದಿ(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_INDEX_2_OFFSET;
                tempLimit=MIN_ಮೌಲ್ಯ(0xdc00, limit);
            } ಅನ್ಯಥಾ {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                tempLimit=MIN_ಮೌಲ್ಯ(0xe000, limit);
            }
        } ಅನ್ಯಥಾ {
            /* supplementary code points */
            ಯದಿ(idx!=nullptr) {
                i2Block=idx[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+
                              (c>>UTRIE2_SHIFT_1)];
            } ಅನ್ಯಥಾ {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            ಯದಿ(i2Block==prevI2Block && (c-prev)>=UTRIE2_CP_PER_INDEX_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
                ಅನುವರ್ತ;
            }
        }
        prevI2Block=i2Block;
        ಯದಿ(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            ಯದಿ(prev_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                ಯದಿ(prev<c && !enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ)) {
                    ಪ್ರತ್ಯಾಗಮ;
                }
                prevBlock=nullBlock;
                prev=c;
                prev_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
            }
            c+=UTRIE2_CP_PER_INDEX_1_ENTRY;
        } ಅನ್ಯಥಾ {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            ಯದಿ((c>>UTRIE2_SHIFT_1)==(tempLimit>>UTRIE2_SHIFT_1)) {
                i2Limit=(tempLimit>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK;
            } ಅನ್ಯಥಾ {
                i2Limit=UTRIE2_INDEX_2_BLOCK_LENGTH;
            }
            ಪ್ರತಿ(; i2<i2Limit; ++i2) {
                ಯದಿ(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_INDEX_SHIFT;
                } ಅನ್ಯಥಾ {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                ಯದಿ(block==prevBlock && (c-prev)>=UTRIE2_DATA_BLOCK_LENGTH) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                    ಅನುವರ್ತ;
                }
                prevBlock=block;
                ಯದಿ(block==nullBlock) {
                    /* this is the null data block */
                    ಯದಿ(prev_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                        ಯದಿ(prev<c && !enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ)) {
                            ಪ್ರತ್ಯಾಗಮ;
                        }
                        prev=c;
                        prev_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
                    }
                    c+=UTRIE2_DATA_BLOCK_LENGTH;
                } ಅನ್ಯಥಾ {
                    ಪ್ರತಿ(j=0; j<UTRIE2_DATA_BLOCK_LENGTH; ++j) {
                        value=enum_ಮೌಲ್ಯ(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        ಯದಿ(value!=prev_ಮೌಲ್ಯ) {
                            ಯದಿ(prev<c && !enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ)) {
                                ಪ್ರತ್ಯಾಗಮ;
                            }
                            prev=c;
                            prev_ಮೌಲ್ಯ=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    ಯದಿ(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } ಅನ್ಯಥಾ ಯದಿ(c<limit) {
        /* c==highStart<limit */
        uint32_t ಮೇಲಿನ_ಮೌಲ್ಯ;
        ಯದಿ(idx!=nullptr) {
            ಮೇಲಿನ_ಮೌಲ್ಯ=
                data32!=nullptr ?
                    data32[trie->ಮೇಲಿನ_ಮೌಲ್ಯ_Index] :
                    idx[trie->ಮೇಲಿನ_ಮೌಲ್ಯ_Index];
        } ಅನ್ಯಥಾ {
            ಮೇಲಿನ_ಮೌಲ್ಯ=trie->newTrie->data[trie->newTrie->dataLength-UTRIE2_DATA_GRANULARITY];
        }
        value=enum_ಮೌಲ್ಯ(context, ಮೇಲಿನ_ಮೌಲ್ಯ);
        ಯದಿ(value!=prev_ಮೌಲ್ಯ) {
            ಯದಿ(prev<c && !enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            prev=c;
            prev_ಮೌಲ್ಯ=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_enum(ಧ್ರುವ UTrie2 *trie,
            UTrie2_Enum_ಮೌಲ್ಯ *enum_ಮೌಲ್ಯ, UTrie2EnumRange *enumRange, ಧ್ರುವ ಶೂನ್ಯ *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_ಮೌಲ್ಯ, enumRange, context);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_enumForLeadSurrogate(ಧ್ರುವ UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_ಮೌಲ್ಯ *enum_ಮೌಲ್ಯ, UTrie2EnumRange *enumRange,
                            ಧ್ರುವ ಶೂನ್ಯ *context) {
    ಯದಿ(!U16_IS_LEAD(lead)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_ಮೌಲ್ಯ, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    ಯದಿ(start>=codePointStart) {
        codePoint=U_SENTINEL;
        ಪ್ರತ್ಯಾಗಮ static_cast<uint16_t>(trie->error_ಮೌಲ್ಯ);
    }
    uint16_t result;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, result);
    ಪ್ರತ್ಯಾಗಮ result;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    ಯದಿ(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        ಪ್ರತ್ಯಾಗಮ static_cast<uint16_t>(trie->error_ಮೌಲ್ಯ);
    }
    uint16_t result;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, result);
    ಪ್ರತ್ಯಾಗಮ result;
}

U_NAMESPACE_END

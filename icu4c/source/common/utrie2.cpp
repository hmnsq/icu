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
get32(ধ্রুবক UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    যদি(c>=trie->উপরের_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        ফেরত trie->তথ্য[trie->তথ্য_দৈর্ঘ্য-UTRIE2_তথ্য_GRANULARITY];
    }

    যদি(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_সূচক_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } অন্যথা {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_সূচক_2_MASK);
    }
    block=trie->index2[i2];
    ফেরত trie->তথ্য[block+(c&UTRIE2_তথ্য_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ধ্রুবক UTrie2 *trie, UChar32 c) {
    যদি(trie->data16!=nullptr) {
        ফেরত UTRIE2_GET16(trie, c);
    } অন্যথা যদি(trie->data32!=nullptr) {
        ফেরত UTRIE2_GET32(trie, c);
    } অন্যথা যদি((uint32_t)c>0x10ffff) {
        ফেরত trie->error_মান;
    } অন্যথা {
        ফেরত get32(trie->newTrie, c, সত্য);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(ধ্রুবক UTrie2 *trie, UChar32 c) {
    যদি(!U_IS_LEAD(c)) {
        ফেরত trie->error_মান;
    }
    যদি(trie->data16!=nullptr) {
        ফেরত UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } অন্যথা যদি(trie->data32!=nullptr) {
        ফেরত UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } অন্যথা {
        ফেরত get32(trie->newTrie, c, মিথ্যা);
    }
}

স্থির অন্তঃরেখ int32_t
u8_সূচক(ধ্রুবক UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_সূচক_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->সূচক_দৈর্ঘ্য : 0,
            c);
    ফেরত (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internal_U8_পরবর্তী_সূচক(ধ্রুবক UTrie2 *trie, UChar32 c,
                           ধ্রুবক uint8_t *src, ধ্রুবক uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    যদি((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } অন্যথা {
        length=7;
    }
    c=utf8_পরবর্তী_Char_Safe_Body(src, &i, length, c, -1);
    ফেরত u8_সূচক(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internal_U8_পূর্ব_সূচক(ধ্রুবক UTrie2 *trie, UChar32 c,
                           ধ্রুবক uint8_t *start, ধ্রুবক uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    যদি((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } অন্যথা {
        i=length=7;
        start=src-7;
    }
    c=utf8_পূর্ব_Char_Safe_Body(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    ফেরত u8_সূচক(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_মান_Bits মান_Bits,
                          ধ্রুবক শূন্য *তথ্য, int32_t length, int32_t *p_Actual_দৈর্ঘ্য,
                          UErrorCode *pErrorCode) {
    ধ্রুবক UTrie2Header *header;
    ধ্রুবক uint16_t *p16;
    int32_t actual_দৈর্ঘ্য;

    UTrie2 অস্থায়ী_Trie;
    UTrie2 *trie;

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }

    যদি( length<=0 || (U_POINTER_MASK_LSB(তথ্য, 3)!=0) ||
        মান_Bits<0 || UTRIE2_গুনতি_মান_BITS<=মান_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    /* enough data for a trie header? */
    যদি(length<(int32_t)আকার(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত nullptr;
    }

    /* check the signature */
    header=(ধ্রুবক UTrie2Header *)তথ্য;
    যদি(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত nullptr;
    }

    /* get the options */
    যদি(মান_Bits!=(UTrie2_মান_Bits)(header->options&UTRIE2_OPTIONS_মান_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&অস্থায়ী_Trie, 0, আকার(অস্থায়ী_Trie));
    অস্থায়ী_Trie.সূচক_দৈর্ঘ্য=header->সূচক_দৈর্ঘ্য;
    অস্থায়ী_Trie.তথ্য_দৈর্ঘ্য=header->shifted_তথ্য_দৈর্ঘ্য<<UTRIE2_সূচক_SHIFT;
    অস্থায়ী_Trie.index2NullOffset=header->index2NullOffset;
    অস্থায়ী_Trie.তথ্য_Null_Offset=header->তথ্য_Null_Offset;

    অস্থায়ী_Trie.উপরের_Start=header->shifted_উপরের_Start<<UTRIE2_SHIFT_1;
    অস্থায়ী_Trie.উপরের_মান_সূচক=অস্থায়ী_Trie.তথ্য_দৈর্ঘ্য-UTRIE2_তথ্য_GRANULARITY;
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        অস্থায়ী_Trie.উপরের_মান_সূচক+=অস্থায়ী_Trie.সূচক_দৈর্ঘ্য;
    }

    /* calculate the actual length */
    actual_দৈর্ঘ্য=(int32_t)আকার(UTrie2Header)+অস্থায়ী_Trie.সূচক_দৈর্ঘ্য*2;
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        actual_দৈর্ঘ্য+=অস্থায়ী_Trie.তথ্য_দৈর্ঘ্য*2;
    } অন্যথা {
        actual_দৈর্ঘ্য+=অস্থায়ী_Trie.তথ্য_দৈর্ঘ্য*4;
    }
    যদি(length<actual_দৈর্ঘ্য) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        ফেরত nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(আকার(UTrie2));
    যদি(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memcpy(trie, &অস্থায়ী_Trie, আকার(অস্থায়ী_Trie));
    trie->memory=(uint32_t *)তথ্য;
    trie->length=actual_দৈর্ঘ্য;
    trie->isMemoryOwned=মিথ্যা;
#ifdef UTRIE2_DEBUG
    trie->name="fromSerialized";
#endif

    /* set the pointers to its index and data arrays */
    p16=(ধ্রুবক uint16_t *)(header+1);
    trie->সূচক=p16;
    p16+=trie->সূচক_দৈর্ঘ্য;

    /* get the data */
    নির্বাচন(মান_Bits) {
    ক্ষেত্র UTRIE2_16_মান_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_মান=trie->সূচক[trie->তথ্য_Null_Offset];
        trie->error_মান=trie->data16[UTRIE2_BAD_UTF8_তথ্য_OFFSET];
        বিরতি;
    ক্ষেত্র UTRIE2_32_মান_BITS:
        trie->data16=nullptr;
        trie->data32=(ধ্রুবক uint32_t *)p16;
        trie->initial_মান=trie->data32[trie->তথ্য_Null_Offset];
        trie->error_মান=trie->data32[UTRIE2_BAD_UTF8_তথ্য_OFFSET];
        বিরতি;
    পূর্বনির্ধারিত:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        ফেরত nullptr;
    }

    যদি(p_Actual_দৈর্ঘ্য!=nullptr) {
        *p_Actual_দৈর্ঘ্য=actual_দৈর্ঘ্য;
    }
    ফেরত trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_মান_Bits মান_Bits,
                 uint32_t initial_মান, uint32_t error_মান,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t সূচক_দৈর্ঘ্য, তথ্য_দৈর্ঘ্য, length, i;
    int32_t তথ্য_Move;  /* >0 if the data is moved to the end of the index array */

    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত nullptr;
    }

    যদি(মান_Bits<0 || UTRIE2_গুনতি_মান_BITS<=মান_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    /* calculate the total length of the dummy trie data */
    সূচক_দৈর্ঘ্য=UTRIE2_সূচক_1_OFFSET;
    তথ্য_দৈর্ঘ্য=UTRIE2_তথ্য_START_OFFSET+UTRIE2_তথ্য_GRANULARITY;
    length=(int32_t)আকার(UTrie2Header)+সূচক_দৈর্ঘ্য*2;
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        length+=তথ্য_দৈর্ঘ্য*2;
    } অন্যথা {
        length+=তথ্য_দৈর্ঘ্য*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(আকার(UTrie2));
    যদি(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    uprv_memset(trie, 0, আকার(UTrie2));
    trie->memory=uprv_malloc(length);
    যদি(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=সত্য;

    /* set the UTrie2 fields */
    যদি(মান_Bits==UTRIE2_16_মান_BITS) {
        তথ্য_Move=সূচক_দৈর্ঘ্য;
    } অন্যথা {
        তথ্য_Move=0;
    }

    trie->সূচক_দৈর্ঘ্য=সূচক_দৈর্ঘ্য;
    trie->তথ্য_দৈর্ঘ্য=তথ্য_দৈর্ঘ্য;
    trie->index2NullOffset=UTRIE2_সূচক_2_OFFSET;
    trie->তথ্য_Null_Offset=(uint16_t)তথ্য_Move;
    trie->initial_মান=initial_মান;
    trie->error_মান=error_মান;
    trie->উপরের_Start=0;
    trie->উপরের_মান_সূচক=তথ্য_Move+UTRIE2_তথ্য_START_OFFSET;
#ifdef UTRIE2_DEBUG
    trie->name="dummy";
#endif

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)মান_Bits;

    header->সূচক_দৈর্ঘ্য=(uint16_t)সূচক_দৈর্ঘ্য;
    header->shifted_তথ্য_দৈর্ঘ্য=(uint16_t)(তথ্য_দৈর্ঘ্য>>UTRIE2_সূচক_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_সূচক_2_OFFSET;
    header->তথ্য_Null_Offset=(uint16_t)তথ্য_Move;
    header->shifted_উপরের_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->সূচক=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    জন্য(i=0; i<UTRIE2_সূচক_2_BMP_দৈর্ঘ্য; ++i) {
        *dest16++=(uint16_t)(তথ্য_Move>>UTRIE2_সূচক_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    জন্য(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(তথ্য_Move+UTRIE2_BAD_UTF8_তথ্য_OFFSET);
    }
    জন্য(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)তথ্য_Move;
    }

    /* write the 16/32-bit data array */
    নির্বাচন(মান_Bits) {
    ক্ষেত্র UTRIE2_16_মান_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        জন্য(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_মান;
        }
        জন্য(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_মান;
        }
        /* highValue and reserved values */
        জন্য(i=0; i<UTRIE2_তথ্য_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_মান;
        }
        বিরতি;
    ক্ষেত্র UTRIE2_32_মান_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        জন্য(i=0; i<0x80; ++i) {
            *p++=initial_মান;
        }
        জন্য(; i<0xc0; ++i) {
            *p++=error_মান;
        }
        /* highValue and reserved values */
        জন্য(i=0; i<UTRIE2_তথ্য_GRANULARITY; ++i) {
            *p++=initial_মান;
        }
        বিরতি;
    পূর্বনির্ধারিত:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

    ফেরত trie;
}

U_CAPI শূন্য U_EXPORT2
utrie2_close(UTrie2 *trie) {
    যদি(trie!=nullptr) {
        যদি(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        যদি(trie->newTrie!=nullptr) {
            uprv_free(trie->newTrie->তথ্য);
#ifdef UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#endif
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ধ্রুবক UTrie2 *trie) {
    ফেরত trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ধ্রুবক UTrie2 *trie,
                 শূন্য *তথ্য, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    যদি( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (তথ্য==nullptr || (U_POINTER_MASK_LSB(তথ্য, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি(capacity>=trie->length) {
        uprv_memcpy(তথ্য, trie->memory, trie->length);
    } অন্যথা {
        *pErrorCode=U_বাফার_OVERFLOW_ERROR;
    }
    ফেরত trie->length;
}

/* enumeration -------------------------------------------------------------- */

#define MIN_মান(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
স্থির uint32_t U_CALLCONV
enum_Same_মান(ধ্রুবক শূন্য * /*context*/, uint32_t value) {
    ফেরত value;
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
enumEitherTrie(ধ্রুবক UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_মান *enum_মান, UTrie2EnumRange *enumRange, ধ্রুবক শূন্য *context) {
    ধ্রুবক uint32_t *data32;
    ধ্রুবক uint16_t *idx;

    uint32_t value, পূর্ব_মান, initial_মান;
    UChar32 c, পূর্ব, উপরের_Start;
    int32_t j, i2Block, পূর্ব_I2_Block, index2NullOffset, block, পূর্ব_Block, nullBlock;

    যদি(enumRange==nullptr) {
        ফেরত;
    }
    যদি(enum_মান==nullptr) {
        enum_মান=enum_Same_মান;
    }

    যদি(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->সূচক;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->তথ্য_Null_Offset;
    } অন্যথা {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->তথ্য;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->তথ্য_Null_Offset;
    }

    উপরের_Start=trie->উপরের_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_মান=enum_মান(context, trie->initial_মান);

    /* set variables for previous range */
    পূর্ব_I2_Block=-1;
    পূর্ব_Block=-1;
    পূর্ব=start;
    পূর্ব_মান=0;

    /* enumerate index-2 blocks */
    জন্য(c=start; c<limit && c<উপরের_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 অস্থায়ী_Limit=c+UTRIE2_CP_PER_সূচক_1_ENTRY;
        যদি(limit<অস্থায়ী_Limit) {
            অস্থায়ী_Limit=limit;
        }
        যদি(c<=0xffff) {
            যদি(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } অন্যথা যদি(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_সূচক_2_OFFSET;
                অস্থায়ী_Limit=MIN_মান(0xdc00, limit);
            } অন্যথা {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                অস্থায়ী_Limit=MIN_মান(0xe000, limit);
            }
        } অন্যথা {
            /* supplementary code points */
            যদি(idx!=nullptr) {
                i2Block=idx[(UTRIE2_সূচক_1_OFFSET-UTRIE2_OMITTED_BMP_সূচক_1_দৈর্ঘ্য)+
                              (c>>UTRIE2_SHIFT_1)];
            } অন্যথা {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            যদি(i2Block==পূর্ব_I2_Block && (c-পূর্ব)>=UTRIE2_CP_PER_সূচক_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_সূচক_1_ENTRY;
                চালু;
            }
        }
        পূর্ব_I2_Block=i2Block;
        যদি(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            যদি(পূর্ব_মান!=initial_মান) {
                যদি(পূর্ব<c && !enumRange(context, পূর্ব, c-1, পূর্ব_মান)) {
                    ফেরত;
                }
                পূর্ব_Block=nullBlock;
                পূর্ব=c;
                পূর্ব_মান=initial_মান;
            }
            c+=UTRIE2_CP_PER_সূচক_1_ENTRY;
        } অন্যথা {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_সূচক_2_MASK;
            যদি((c>>UTRIE2_SHIFT_1)==(অস্থায়ী_Limit>>UTRIE2_SHIFT_1)) {
                i2Limit=(অস্থায়ী_Limit>>UTRIE2_SHIFT_2)&UTRIE2_সূচক_2_MASK;
            } অন্যথা {
                i2Limit=UTRIE2_সূচক_2_BLOCK_দৈর্ঘ্য;
            }
            জন্য(; i2<i2Limit; ++i2) {
                যদি(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_সূচক_SHIFT;
                } অন্যথা {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                যদি(block==পূর্ব_Block && (c-পূর্ব)>=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
                    চালু;
                }
                পূর্ব_Block=block;
                যদি(block==nullBlock) {
                    /* this is the null data block */
                    যদি(পূর্ব_মান!=initial_মান) {
                        যদি(পূর্ব<c && !enumRange(context, পূর্ব, c-1, পূর্ব_মান)) {
                            ফেরত;
                        }
                        পূর্ব=c;
                        পূর্ব_মান=initial_মান;
                    }
                    c+=UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য;
                } অন্যথা {
                    জন্য(j=0; j<UTRIE2_তথ্য_BLOCK_দৈর্ঘ্য; ++j) {
                        value=enum_মান(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        যদি(value!=পূর্ব_মান) {
                            যদি(পূর্ব<c && !enumRange(context, পূর্ব, c-1, পূর্ব_মান)) {
                                ফেরত;
                            }
                            পূর্ব=c;
                            পূর্ব_মান=value;
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
                    data32[trie->উপরের_মান_সূচক] :
                    idx[trie->উপরের_মান_সূচক];
        } অন্যথা {
            উপরের_মান=trie->newTrie->তথ্য[trie->newTrie->তথ্য_দৈর্ঘ্য-UTRIE2_তথ্য_GRANULARITY];
        }
        value=enum_মান(context, উপরের_মান);
        যদি(value!=পূর্ব_মান) {
            যদি(পূর্ব<c && !enumRange(context, পূর্ব, c-1, পূর্ব_মান)) {
                ফেরত;
            }
            পূর্ব=c;
            পূর্ব_মান=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, পূর্ব, c-1, পূর্ব_মান);
}

U_CAPI শূন্য U_EXPORT2
utrie2_enum(ধ্রুবক UTrie2 *trie,
            UTrie2_Enum_মান *enum_মান, UTrie2EnumRange *enumRange, ধ্রুবক শূন্য *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_মান, enumRange, context);
}

U_CAPI শূন্য U_EXPORT2
utrie2_enumForLeadSurrogate(ধ্রুবক UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_মান *enum_মান, UTrie2EnumRange *enumRange,
                            ধ্রুবক শূন্য *context) {
    যদি(!U16_IS_LEAD(lead)) {
        ফেরত;
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
        ফেরত static_cast<uint16_t>(trie->error_মান);
    }
    uint16_t ফলাফল;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, ফলাফল);
    ফেরত ফলাফল;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    যদি(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        ফেরত static_cast<uint16_t>(trie->error_মান);
    }
    uint16_t ফলাফল;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, ফলাফল);
    ফেরত ফলাফল;
}

U_NAMESPACE_END

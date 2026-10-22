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
#समावेश "unicode/utypes.h"
#अगर_घोषित UCPTRIE_DEBUG
#समावेश "unicode/umutablecptrie.h"
#पूर्ण_अगर
#समावेश "unicode/utf.h"
#समावेश "unicode/utf8.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "utrie2.h"
#समावेश "utrie2_impl.h"
#समावेश "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

अटल uint32_t
get32(स्थिर UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, block;

    अगर(c>=trie->ऊपरी_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        वापस trie->तथ्य[trie->तथ्य_लंबाई-UTRIE2_तथ्य_GRANULARITY];
    }

    अगर(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_सूचकांक_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2))+
            (c>>UTRIE2_SHIFT_2);
    } अन्यथा {
        i2=trie->index1[c>>UTRIE2_SHIFT_1]+
            ((c>>UTRIE2_SHIFT_2)&UTRIE2_सूचकांक_2_MASK);
    }
    block=trie->index2[i2];
    वापस trie->तथ्य[block+(c&UTRIE2_तथ्य_MASK)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(स्थिर UTrie2 *trie, UChar32 c) {
    अगर(trie->data16!=nullptr) {
        वापस UTRIE2_GET16(trie, c);
    } अन्यथा अगर(trie->data32!=nullptr) {
        वापस UTRIE2_GET32(trie, c);
    } अन्यथा अगर((uint32_t)c>0x10ffff) {
        वापस trie->error_मूल्य;
    } अन्यथा {
        वापस get32(trie->newTrie, c, true);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32FromLeadSurrogateCodeUnit(स्थिर UTrie2 *trie, UChar32 c) {
    अगर(!U_IS_LEAD(c)) {
        वापस trie->error_मूल्य;
    }
    अगर(trie->data16!=nullptr) {
        वापस UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } अन्यथा अगर(trie->data32!=nullptr) {
        वापस UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } अन्यथा {
        वापस get32(trie->newTrie, c, false);
    }
}

अटल अंतरभूत int32_t
u8_सूचकांक(स्थिर UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_सूचकांक_FROM_CP(
            trie,
            trie->data32==nullptr ? trie->सूचकांक_लंबाई : 0,
            c);
    वापस (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_internal_U8_अग्र_सूचकांक(स्थिर UTrie2 *trie, UChar32 c,
                           स्थिर uint8_t *src, स्थिर uint8_t *limit) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    अगर((limit-src)<=7) {
        length=(int32_t)(limit-src);
    } अन्यथा {
        length=7;
    }
    c=utf8_अग्र_Char_Safe_Body(src, &i, length, c, -1);
    वापस u8_सूचकांक(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_internal_U8_पूर्व_सूचकांक(स्थिर UTrie2 *trie, UChar32 c,
                           स्थिर uint8_t *start, स्थिर uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    अगर((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } अन्यथा {
        i=length=7;
        start=src-7;
    }
    c=utf8_पूर्व_Char_Safe_Body(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    वापस u8_सूचकांक(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_मूल्य_Bits मूल्य_Bits,
                          स्थिर व्योम *तथ्य, int32_t length, int32_t *p_Actual_लंबाई,
                          UErrorCode *pErrorCode) {
    स्थिर UTrie2Header *header;
    स्थिर uint16_t *p16;
    int32_t actual_लंबाई;

    UTrie2 अस्थायी_Trie;
    UTrie2 *trie;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }

    अगर( length<=0 || (U_POINTER_MASK_LSB(तथ्य, 3)!=0) ||
        मूल्य_Bits<0 || UTRIE2_गिनती_मूल्य_BITS<=मूल्य_Bits
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    /* enough data for a trie header? */
    अगर(length<(int32_t)माप(UTrie2Header)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }

    /* check the signature */
    header=(स्थिर UTrie2Header *)तथ्य;
    अगर(header->signature!=UTRIE2_SIG) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }

    /* get the options */
    अगर(मूल्य_Bits!=(UTrie2_मूल्य_Bits)(header->options&UTRIE2_OPTIONS_मूल्य_BITS_MASK)) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }

    /* get the length values and offsets */
    uprv_memset(&अस्थायी_Trie, 0, माप(अस्थायी_Trie));
    अस्थायी_Trie.सूचकांक_लंबाई=header->सूचकांक_लंबाई;
    अस्थायी_Trie.तथ्य_लंबाई=header->shifted_तथ्य_लंबाई<<UTRIE2_सूचकांक_SHIFT;
    अस्थायी_Trie.index2NullOffset=header->index2NullOffset;
    अस्थायी_Trie.तथ्य_Null_Offset=header->तथ्य_Null_Offset;

    अस्थायी_Trie.ऊपरी_Start=header->shifted_ऊपरी_Start<<UTRIE2_SHIFT_1;
    अस्थायी_Trie.ऊपरी_मूल्य_सूचकांक=अस्थायी_Trie.तथ्य_लंबाई-UTRIE2_तथ्य_GRANULARITY;
    अगर(मूल्य_Bits==UTRIE2_16_मूल्य_BITS) {
        अस्थायी_Trie.ऊपरी_मूल्य_सूचकांक+=अस्थायी_Trie.सूचकांक_लंबाई;
    }

    /* calculate the actual length */
    actual_लंबाई=(int32_t)माप(UTrie2Header)+अस्थायी_Trie.सूचकांक_लंबाई*2;
    अगर(मूल्य_Bits==UTRIE2_16_मूल्य_BITS) {
        actual_लंबाई+=अस्थायी_Trie.तथ्य_लंबाई*2;
    } अन्यथा {
        actual_लंबाई+=अस्थायी_Trie.तथ्य_लंबाई*4;
    }
    अगर(length<actual_लंबाई) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        वापस nullptr;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(माप(UTrie2));
    अगर(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memcpy(trie, &अस्थायी_Trie, माप(अस्थायी_Trie));
    trie->memory=(uint32_t *)तथ्य;
    trie->length=actual_लंबाई;
    trie->isMemoryOwned=false;
#अगर_घोषित UTRIE2_DEBUG
    trie->name="fromSerialized";
#पूर्ण_अगर

    /* set the pointers to its index and data arrays */
    p16=(स्थिर uint16_t *)(header+1);
    trie->सूचकांक=p16;
    p16+=trie->सूचकांक_लंबाई;

    /* get the data */
    चयन(मूल्य_Bits) {
    हाल UTRIE2_16_मूल्य_BITS:
        trie->data16=p16;
        trie->data32=nullptr;
        trie->initial_मूल्य=trie->सूचकांक[trie->तथ्य_Null_Offset];
        trie->error_मूल्य=trie->data16[UTRIE2_BAD_UTF8_तथ्य_OFFSET];
        अवरोध;
    हाल UTRIE2_32_मूल्य_BITS:
        trie->data16=nullptr;
        trie->data32=(स्थिर uint32_t *)p16;
        trie->initial_मूल्य=trie->data32[trie->तथ्य_Null_Offset];
        trie->error_मूल्य=trie->data32[UTRIE2_BAD_UTF8_तथ्य_OFFSET];
        अवरोध;
    शेष:
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        वापस nullptr;
    }

    अगर(p_Actual_लंबाई!=nullptr) {
        *p_Actual_लंबाई=actual_लंबाई;
    }
    वापस trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_मूल्य_Bits मूल्य_Bits,
                 uint32_t initial_मूल्य, uint32_t error_मूल्य,
                 UErrorCode *pErrorCode) {
    UTrie2 *trie;
    UTrie2Header *header;
    uint32_t *p;
    uint16_t *dest16;
    int32_t सूचकांक_लंबाई, तथ्य_लंबाई, length, i;
    int32_t तथ्य_Move;  /* >0 if the data is moved to the end of the index array */

    अगर(U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }

    अगर(मूल्य_Bits<0 || UTRIE2_गिनती_मूल्य_BITS<=मूल्य_Bits) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    /* calculate the total length of the dummy trie data */
    सूचकांक_लंबाई=UTRIE2_सूचकांक_1_OFFSET;
    तथ्य_लंबाई=UTRIE2_तथ्य_START_OFFSET+UTRIE2_तथ्य_GRANULARITY;
    length=(int32_t)माप(UTrie2Header)+सूचकांक_लंबाई*2;
    अगर(मूल्य_Bits==UTRIE2_16_मूल्य_BITS) {
        length+=तथ्य_लंबाई*2;
    } अन्यथा {
        length+=तथ्य_लंबाई*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(माप(UTrie2));
    अगर(trie==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    uprv_memset(trie, 0, माप(UTrie2));
    trie->memory=uprv_malloc(length);
    अगर(trie->memory==nullptr) {
        uprv_free(trie);
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    trie->length=length;
    trie->isMemoryOwned=true;

    /* set the UTrie2 fields */
    अगर(मूल्य_Bits==UTRIE2_16_मूल्य_BITS) {
        तथ्य_Move=सूचकांक_लंबाई;
    } अन्यथा {
        तथ्य_Move=0;
    }

    trie->सूचकांक_लंबाई=सूचकांक_लंबाई;
    trie->तथ्य_लंबाई=तथ्य_लंबाई;
    trie->index2NullOffset=UTRIE2_सूचकांक_2_OFFSET;
    trie->तथ्य_Null_Offset=(uint16_t)तथ्य_Move;
    trie->initial_मूल्य=initial_मूल्य;
    trie->error_मूल्य=error_मूल्य;
    trie->ऊपरी_Start=0;
    trie->ऊपरी_मूल्य_सूचकांक=तथ्य_Move+UTRIE2_तथ्य_START_OFFSET;
#अगर_घोषित UTRIE2_DEBUG
    trie->name="dummy";
#पूर्ण_अगर

    /* set the header fields */
    header=(UTrie2Header *)trie->memory;

    header->signature=UTRIE2_SIG; /* "Tri2" */
    header->options=(uint16_t)मूल्य_Bits;

    header->सूचकांक_लंबाई=(uint16_t)सूचकांक_लंबाई;
    header->shifted_तथ्य_लंबाई=(uint16_t)(तथ्य_लंबाई>>UTRIE2_सूचकांक_SHIFT);
    header->index2NullOffset=(uint16_t)UTRIE2_सूचकांक_2_OFFSET;
    header->तथ्य_Null_Offset=(uint16_t)तथ्य_Move;
    header->shifted_ऊपरी_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(header+1);
    trie->सूचकांक=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    क्रम(i=0; i<UTRIE2_सूचकांक_2_BMP_लंबाई; ++i) {
        *dest16++=(uint16_t)(तथ्य_Move>>UTRIE2_सूचकांक_SHIFT);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    क्रम(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(तथ्य_Move+UTRIE2_BAD_UTF8_तथ्य_OFFSET);
    }
    क्रम(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)तथ्य_Move;
    }

    /* write the 16/32-bit data array */
    चयन(मूल्य_Bits) {
    हाल UTRIE2_16_मूल्य_BITS:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=nullptr;
        क्रम(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_मूल्य;
        }
        क्रम(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_मूल्य;
        }
        /* highValue and reserved values */
        क्रम(i=0; i<UTRIE2_तथ्य_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_मूल्य;
        }
        अवरोध;
    हाल UTRIE2_32_मूल्य_BITS:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=nullptr;
        trie->data32=p;
        क्रम(i=0; i<0x80; ++i) {
            *p++=initial_मूल्य;
        }
        क्रम(; i<0xc0; ++i) {
            *p++=error_मूल्य;
        }
        /* highValue and reserved values */
        क्रम(i=0; i<UTRIE2_तथ्य_GRANULARITY; ++i) {
            *p++=initial_मूल्य;
        }
        अवरोध;
    शेष:
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

    वापस trie;
}

U_CAPI व्योम U_EXPORT2
utrie2_close(UTrie2 *trie) {
    अगर(trie!=nullptr) {
        अगर(trie->isMemoryOwned) {
            uprv_free(trie->memory);
        }
        अगर(trie->newTrie!=nullptr) {
            uprv_free(trie->newTrie->तथ्य);
#अगर_घोषित UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#पूर्ण_अगर
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(स्थिर UTrie2 *trie) {
    वापस trie->newTrie==nullptr;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(स्थिर UTrie2 *trie,
                 व्योम *तथ्य, int32_t capacity,
                 UErrorCode *pErrorCode) {
    /* argument check */
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    अगर( trie==nullptr || trie->memory==nullptr || trie->newTrie!=nullptr ||
        capacity<0 || (capacity>0 && (तथ्य==nullptr || (U_POINTER_MASK_LSB(तथ्य, 3)!=0)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर(capacity>=trie->length) {
        uprv_memcpy(तथ्य, trie->memory, trie->length);
    } अन्यथा {
        *pErrorCode=U_बफर_OVERFLOW_ERROR;
    }
    वापस trie->length;
}

/* enumeration -------------------------------------------------------------- */

#घोषणा MIN_मूल्य(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
अटल uint32_t U_CALLCONV
enum_Same_मूल्य(स्थिर व्योम * /*context*/, uint32_t value) {
    वापस value;
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
अटल व्योम
enumEitherTrie(स्थिर UTrie2 *trie,
               UChar32 start, UChar32 limit,
               UTrie2_Enum_मूल्य *enum_मूल्य, UTrie2EnumRange *enumRange, स्थिर व्योम *context) {
    स्थिर uint32_t *data32;
    स्थिर uint16_t *idx;

    uint32_t value, पूर्व_मूल्य, initial_मूल्य;
    UChar32 c, पूर्व, ऊपरी_Start;
    int32_t j, i2Block, पूर्व_I2_Block, index2NullOffset, block, पूर्व_Block, nullBlock;

    अगर(enumRange==nullptr) {
        वापस;
    }
    अगर(enum_मूल्य==nullptr) {
        enum_मूल्य=enum_Same_मूल्य;
    }

    अगर(trie->newTrie==nullptr) {
        /* frozen trie */
        idx=trie->सूचकांक;
        U_ASSERT(idx!=nullptr); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2NullOffset=trie->index2NullOffset;
        nullBlock=trie->तथ्य_Null_Offset;
    } अन्यथा {
        /* unfrozen, mutable trie */
        idx=nullptr;
        data32=trie->newTrie->तथ्य;
        U_ASSERT(data32!=nullptr); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2NullOffset=trie->newTrie->index2NullOffset;
        nullBlock=trie->newTrie->तथ्य_Null_Offset;
    }

    ऊपरी_Start=trie->ऊपरी_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_मूल्य=enum_मूल्य(context, trie->initial_मूल्य);

    /* set variables for previous range */
    पूर्व_I2_Block=-1;
    पूर्व_Block=-1;
    पूर्व=start;
    पूर्व_मूल्य=0;

    /* enumerate index-2 blocks */
    क्रम(c=start; c<limit && c<ऊपरी_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 अस्थायी_Limit=c+UTRIE2_CP_PER_सूचकांक_1_ENTRY;
        अगर(limit<अस्थायी_Limit) {
            अस्थायी_Limit=limit;
        }
        अगर(c<=0xffff) {
            अगर(!U_IS_SURROGATE(c)) {
                i2Block=c>>UTRIE2_SHIFT_2;
            } अन्यथा अगर(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2Block=UTRIE2_LSCP_सूचकांक_2_OFFSET;
                अस्थायी_Limit=MIN_मूल्य(0xdc00, limit);
            } अन्यथा {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2Block=0xd800>>UTRIE2_SHIFT_2;
                अस्थायी_Limit=MIN_मूल्य(0xe000, limit);
            }
        } अन्यथा {
            /* supplementary code points */
            अगर(idx!=nullptr) {
                i2Block=idx[(UTRIE2_सूचकांक_1_OFFSET-UTRIE2_OMITTED_BMP_सूचकांक_1_लंबाई)+
                              (c>>UTRIE2_SHIFT_1)];
            } अन्यथा {
                i2Block=trie->newTrie->index1[c>>UTRIE2_SHIFT_1];
            }
            अगर(i2Block==पूर्व_I2_Block && (c-पूर्व)>=UTRIE2_CP_PER_सूचकांक_1_ENTRY) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_PER_सूचकांक_1_ENTRY;
                जारी;
            }
        }
        पूर्व_I2_Block=i2Block;
        अगर(i2Block==index2NullOffset) {
            /* this is the null index-2 block */
            अगर(पूर्व_मूल्य!=initial_मूल्य) {
                अगर(पूर्व<c && !enumRange(context, पूर्व, c-1, पूर्व_मूल्य)) {
                    वापस;
                }
                पूर्व_Block=nullBlock;
                पूर्व=c;
                पूर्व_मूल्य=initial_मूल्य;
            }
            c+=UTRIE2_CP_PER_सूचकांक_1_ENTRY;
        } अन्यथा {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2Limit;
            i2=(c>>UTRIE2_SHIFT_2)&UTRIE2_सूचकांक_2_MASK;
            अगर((c>>UTRIE2_SHIFT_1)==(अस्थायी_Limit>>UTRIE2_SHIFT_1)) {
                i2Limit=(अस्थायी_Limit>>UTRIE2_SHIFT_2)&UTRIE2_सूचकांक_2_MASK;
            } अन्यथा {
                i2Limit=UTRIE2_सूचकांक_2_BLOCK_लंबाई;
            }
            क्रम(; i2<i2Limit; ++i2) {
                अगर(idx!=nullptr) {
                    block = static_cast<int32_t>(idx[i2Block + i2]) << UTRIE2_सूचकांक_SHIFT;
                } अन्यथा {
                    block=trie->newTrie->index2[i2Block+i2];
                }
                अगर(block==पूर्व_Block && (c-पूर्व)>=UTRIE2_तथ्य_BLOCK_लंबाई) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_तथ्य_BLOCK_लंबाई;
                    जारी;
                }
                पूर्व_Block=block;
                अगर(block==nullBlock) {
                    /* this is the null data block */
                    अगर(पूर्व_मूल्य!=initial_मूल्य) {
                        अगर(पूर्व<c && !enumRange(context, पूर्व, c-1, पूर्व_मूल्य)) {
                            वापस;
                        }
                        पूर्व=c;
                        पूर्व_मूल्य=initial_मूल्य;
                    }
                    c+=UTRIE2_तथ्य_BLOCK_लंबाई;
                } अन्यथा {
                    क्रम(j=0; j<UTRIE2_तथ्य_BLOCK_लंबाई; ++j) {
                        value=enum_मूल्य(context, data32!=nullptr ? data32[block+j] : idx[block+j]);
                        अगर(value!=पूर्व_मूल्य) {
                            अगर(पूर्व<c && !enumRange(context, पूर्व, c-1, पूर्व_मूल्य)) {
                                वापस;
                            }
                            पूर्व=c;
                            पूर्व_मूल्य=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    अगर(c>limit) {
        c=limit;  /* could be higher if in the index2NullOffset */
    } अन्यथा अगर(c<limit) {
        /* c==highStart<limit */
        uint32_t ऊपरी_मूल्य;
        अगर(idx!=nullptr) {
            ऊपरी_मूल्य=
                data32!=nullptr ?
                    data32[trie->ऊपरी_मूल्य_सूचकांक] :
                    idx[trie->ऊपरी_मूल्य_सूचकांक];
        } अन्यथा {
            ऊपरी_मूल्य=trie->newTrie->तथ्य[trie->newTrie->तथ्य_लंबाई-UTRIE2_तथ्य_GRANULARITY];
        }
        value=enum_मूल्य(context, ऊपरी_मूल्य);
        अगर(value!=पूर्व_मूल्य) {
            अगर(पूर्व<c && !enumRange(context, पूर्व, c-1, पूर्व_मूल्य)) {
                वापस;
            }
            पूर्व=c;
            पूर्व_मूल्य=value;
        }
        c=limit;
    }

    /* deliver last range */
    enumRange(context, पूर्व, c-1, पूर्व_मूल्य);
}

U_CAPI व्योम U_EXPORT2
utrie2_enum(स्थिर UTrie2 *trie,
            UTrie2_Enum_मूल्य *enum_मूल्य, UTrie2EnumRange *enumRange, स्थिर व्योम *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_मूल्य, enumRange, context);
}

U_CAPI व्योम U_EXPORT2
utrie2_enumForLeadSurrogate(स्थिर UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_मूल्य *enum_मूल्य, UTrie2EnumRange *enumRange,
                            स्थिर व्योम *context) {
    अगर(!U16_IS_LEAD(lead)) {
        वापस;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_मूल्य, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    codePointLimit=codePointStart;
    अगर(start>=codePointStart) {
        codePoint=U_SENTINEL;
        वापस static_cast<uint16_t>(trie->error_मूल्य);
    }
    uint16_t परिणाम;
    UTRIE2_U16_PREV16(trie, start, codePointStart, codePoint, परिणाम);
    वापस परिणाम;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    codePointStart=codePointLimit;
    अगर(codePointLimit==limit) {
        codePoint=U_SENTINEL;
        वापस static_cast<uint16_t>(trie->error_मूल्य);
    }
    uint16_t परिणाम;
    UTRIE2_U16_NEXT16(trie, codePointLimit, limit, codePoint, परिणाम);
    वापस परिणाम;
}

U_NAMESPACE_END

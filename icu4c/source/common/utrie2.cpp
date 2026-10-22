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
#यदि_परिभाषितम् UCPTRIE_DEBUG
#अन्तर्भाव "unicode/umutablecptrie.h"
#यदि_अन्तः
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
    int32_t i2, खण्डम्;

    यदि(c>=trie->उच्चः_Start && (!U_IS_LEAD(c) || fromLSCP)) {
        प्रत्यागम trie->दत्त[trie->दत्त_दीर्घता-UTRIE2_दत्त_GRANULARITY];
    }

    यदि(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_सूचकांक_2_विस्थापनम्-(0xd800>>UTRIE2_सरणम्_2))+
            (c>>UTRIE2_सरणम्_2);
    } अन्यथा {
        i2=trie->index1[c>>UTRIE2_सरणम्_1]+
            ((c>>UTRIE2_सरणम्_2)&UTRIE2_सूचकांक_2_आवरणम्);
    }
    खण्डम्=trie->index2[i2];
    प्रत्यागम trie->दत्त[खण्डम्+(c&UTRIE2_दत्त_आवरणम्)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ध्रुव UTrie2 *trie, UChar32 c) {
    यदि(trie->data16!=शून्यसूचकः) {
        प्रत्यागम UTRIE2_GET16(trie, c);
    } अन्यथा यदि(trie->data32!=शून्यसूचकः) {
        प्रत्यागम UTRIE2_GET32(trie, c);
    } अन्यथा यदि((uint32_t)c>0x10ffff) {
        प्रत्यागम trie->error_मानम्;
    } अन्यथा {
        प्रत्यागम get32(trie->newTrie, c, सत्य);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_संहिता_Unit(ध्रुव UTrie2 *trie, UChar32 c) {
    यदि(!U_IS_LEAD(c)) {
        प्रत्यागम trie->error_मानम्;
    }
    यदि(trie->data16!=शून्यसूचकः) {
        प्रत्यागम UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } अन्यथा यदि(trie->data32!=शून्यसूचकः) {
        प्रत्यागम UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } अन्यथा {
        प्रत्यागम get32(trie->newTrie, c, असत्य);
    }
}

स्थिर अन्तःस्थ int32_t
u8_सूचकांक(ध्रुव UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_सूचकांक_FROM_CP(
            trie,
            trie->data32==शून्यसूचकः ? trie->सूचकांक_दीर्घता : 0,
            c);
    प्रत्यागम (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_आन्तरिकम्_U8_अग्र_सूचकांक(ध्रुव UTrie2 *trie, UChar32 c,
                           ध्रुव uint8_t *src, ध्रुव uint8_t *सीमा) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    यदि((सीमा-src)<=7) {
        length=(int32_t)(सीमा-src);
    } अन्यथा {
        length=7;
    }
    c=utf8_अग्र_Char_Safe_Body(src, &i, length, c, -1);
    प्रत्यागम u8_सूचकांक(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_आन्तरिकम्_U8_पूर्व_सूचकांक(ध्रुव UTrie2 *trie, UChar32 c,
                           ध्रुव uint8_t *start, ध्रुव uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    यदि((src-start)<=7) {
        i=length=(int32_t)(src-start);
    } अन्यथा {
        i=length=7;
        start=src-7;
    }
    c=utf8_पूर्व_Char_Safe_Body(start, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    प्रत्यागम u8_सूचकांक(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_मानम्_द्वयङ्काः मानम्_द्वयङ्काः,
                          ध्रुव शून्य *दत्त, int32_t length, int32_t *p_Actual_दीर्घता,
                          UError_संहिता *p_Error_संहिता) {
    ध्रुव UTrie2_शीर्षकम् *शीर्षकम्;
    ध्रुव uint16_t *p16;
    int32_t actual_दीर्घता;

    UTrie2 अस्थायी_Trie;
    UTrie2 *trie;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि( length<=0 || (U_POINTER_आवरणम्_LSB(दत्त, 3)!=0) ||
        मानम्_द्वयङ्काः<0 || UTRIE2_गणनम्_मानम्_द्वयङ्काः<=मानम्_द्वयङ्काः
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    /* enough data for a trie header? */
    यदि(length<(int32_t)परिमाण(UTrie2_शीर्षकम्)) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    /* check the signature */
    शीर्षकम्=(ध्रुव UTrie2_शीर्षकम् *)दत्त;
    यदि(शीर्षकम्->signature!=UTRIE2_SIG) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    /* get the options */
    यदि(मानम्_द्वयङ्काः!=(UTrie2_मानम्_द्वयङ्काः)(शीर्षकम्->options&UTRIE2_OPTIONS_मानम्_द्वयङ्काः_आवरणम्)) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    /* get the length values and offsets */
    uprv_memset(&अस्थायी_Trie, 0, परिमाण(अस्थायी_Trie));
    अस्थायी_Trie.सूचकांक_दीर्घता=शीर्षकम्->सूचकांक_दीर्घता;
    अस्थायी_Trie.दत्त_दीर्घता=शीर्षकम्->shifted_दत्त_दीर्घता<<UTRIE2_सूचकांक_सरणम्;
    अस्थायी_Trie.index2_Null_विस्थापनम्=शीर्षकम्->index2_Null_विस्थापनम्;
    अस्थायी_Trie.दत्त_Null_विस्थापनम्=शीर्षकम्->दत्त_Null_विस्थापनम्;

    अस्थायी_Trie.उच्चः_Start=शीर्षकम्->shifted_उच्चः_Start<<UTRIE2_सरणम्_1;
    अस्थायी_Trie.उच्चः_मानम्_सूचकांक=अस्थायी_Trie.दत्त_दीर्घता-UTRIE2_दत्त_GRANULARITY;
    यदि(मानम्_द्वयङ्काः==UTRIE2_16_मानम्_द्वयङ्काः) {
        अस्थायी_Trie.उच्चः_मानम्_सूचकांक+=अस्थायी_Trie.सूचकांक_दीर्घता;
    }

    /* calculate the actual length */
    actual_दीर्घता=(int32_t)परिमाण(UTrie2_शीर्षकम्)+अस्थायी_Trie.सूचकांक_दीर्घता*2;
    यदि(मानम्_द्वयङ्काः==UTRIE2_16_मानम्_द्वयङ्काः) {
        actual_दीर्घता+=अस्थायी_Trie.दत्त_दीर्घता*2;
    } अन्यथा {
        actual_दीर्घता+=अस्थायी_Trie.दत्त_दीर्घता*4;
    }
    यदि(length<actual_दीर्घता) {
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        प्रत्यागम शून्यसूचकः;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    यदि(trie==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memcpy(trie, &अस्थायी_Trie, परिमाण(अस्थायी_Trie));
    trie->स्मृतिः=(uint32_t *)दत्त;
    trie->length=actual_दीर्घता;
    trie->is_स्मृतिः_Owned=असत्य;
#यदि_परिभाषितम् UTRIE2_DEBUG
    trie->name="fromSerialized";
#यदि_अन्तः

    /* set the pointers to its index and data arrays */
    p16=(ध्रुव uint16_t *)(शीर्षकम्+1);
    trie->सूचकांक=p16;
    p16+=trie->सूचकांक_दीर्घता;

    /* get the data */
    विकल्प(मानम्_द्वयङ्काः) {
    पक्ष UTRIE2_16_मानम्_द्वयङ्काः:
        trie->data16=p16;
        trie->data32=शून्यसूचकः;
        trie->initial_मानम्=trie->सूचकांक[trie->दत्त_Null_विस्थापनम्];
        trie->error_मानम्=trie->data16[UTRIE2_BAD_UTF8_दत्त_विस्थापनम्];
        भङ्ग;
    पक्ष UTRIE2_32_मानम्_द्वयङ्काः:
        trie->data16=शून्यसूचकः;
        trie->data32=(ध्रुव uint32_t *)p16;
        trie->initial_मानम्=trie->data32[trie->दत्त_Null_विस्थापनम्];
        trie->error_मानम्=trie->data32[UTRIE2_BAD_UTF8_दत्त_विस्थापनम्];
        भङ्ग;
    स्वभाव:
        *p_Error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    यदि(p_Actual_दीर्घता!=शून्यसूचकः) {
        *p_Actual_दीर्घता=actual_दीर्घता;
    }
    प्रत्यागम trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_मानम्_द्वयङ्काः मानम्_द्वयङ्काः,
                 uint32_t initial_मानम्, uint32_t error_मानम्,
                 UError_संहिता *p_Error_संहिता) {
    UTrie2 *trie;
    UTrie2_शीर्षकम् *शीर्षकम्;
    uint32_t *p;
    uint16_t *dest16;
    int32_t सूचकांक_दीर्घता, दत्त_दीर्घता, length, i;
    int32_t दत्त_Move;  /* >0 if the data is moved to the end of the index array */

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }

    यदि(मानम्_द्वयङ्काः<0 || UTRIE2_गणनम्_मानम्_द्वयङ्काः<=मानम्_द्वयङ्काः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    /* calculate the total length of the dummy trie data */
    सूचकांक_दीर्घता=UTRIE2_सूचकांक_1_विस्थापनम्;
    दत्त_दीर्घता=UTRIE2_दत्त_START_विस्थापनम्+UTRIE2_दत्त_GRANULARITY;
    length=(int32_t)परिमाण(UTrie2_शीर्षकम्)+सूचकांक_दीर्घता*2;
    यदि(मानम्_द्वयङ्काः==UTRIE2_16_मानम्_द्वयङ्काः) {
        length+=दत्त_दीर्घता*2;
    } अन्यथा {
        length+=दत्त_दीर्घता*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(परिमाण(UTrie2));
    यदि(trie==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    uprv_memset(trie, 0, परिमाण(UTrie2));
    trie->स्मृतिः=uprv_malloc(length);
    यदि(trie->स्मृतिः==शून्यसूचकः) {
        uprv_free(trie);
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    trie->length=length;
    trie->is_स्मृतिः_Owned=सत्य;

    /* set the UTrie2 fields */
    यदि(मानम्_द्वयङ्काः==UTRIE2_16_मानम्_द्वयङ्काः) {
        दत्त_Move=सूचकांक_दीर्घता;
    } अन्यथा {
        दत्त_Move=0;
    }

    trie->सूचकांक_दीर्घता=सूचकांक_दीर्घता;
    trie->दत्त_दीर्घता=दत्त_दीर्घता;
    trie->index2_Null_विस्थापनम्=UTRIE2_सूचकांक_2_विस्थापनम्;
    trie->दत्त_Null_विस्थापनम्=(uint16_t)दत्त_Move;
    trie->initial_मानम्=initial_मानम्;
    trie->error_मानम्=error_मानम्;
    trie->उच्चः_Start=0;
    trie->उच्चः_मानम्_सूचकांक=दत्त_Move+UTRIE2_दत्त_START_विस्थापनम्;
#यदि_परिभाषितम् UTRIE2_DEBUG
    trie->name="dummy";
#यदि_अन्तः

    /* set the header fields */
    शीर्षकम्=(UTrie2_शीर्षकम् *)trie->स्मृतिः;

    शीर्षकम्->signature=UTRIE2_SIG; /* "Tri2" */
    शीर्षकम्->options=(uint16_t)मानम्_द्वयङ्काः;

    शीर्षकम्->सूचकांक_दीर्घता=(uint16_t)सूचकांक_दीर्घता;
    शीर्षकम्->shifted_दत्त_दीर्घता=(uint16_t)(दत्त_दीर्घता>>UTRIE2_सूचकांक_सरणम्);
    शीर्षकम्->index2_Null_विस्थापनम्=(uint16_t)UTRIE2_सूचकांक_2_विस्थापनम्;
    शीर्षकम्->दत्त_Null_विस्थापनम्=(uint16_t)दत्त_Move;
    शीर्षकम्->shifted_उच्चः_Start=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(शीर्षकम्+1);
    trie->सूचकांक=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    प्रति(i=0; i<UTRIE2_सूचकांक_2_BMP_दीर्घता; ++i) {
        *dest16++=(uint16_t)(दत्त_Move>>UTRIE2_सूचकांक_सरणम्);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    प्रति(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(दत्त_Move+UTRIE2_BAD_UTF8_दत्त_विस्थापनम्);
    }
    प्रति(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)दत्त_Move;
    }

    /* write the 16/32-bit data array */
    विकल्प(मानम्_द्वयङ्काः) {
    पक्ष UTRIE2_16_मानम्_द्वयङ्काः:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=शून्यसूचकः;
        प्रति(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_मानम्;
        }
        प्रति(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_मानम्;
        }
        /* highValue and reserved values */
        प्रति(i=0; i<UTRIE2_दत्त_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_मानम्;
        }
        भङ्ग;
    पक्ष UTRIE2_32_मानम्_द्वयङ्काः:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=शून्यसूचकः;
        trie->data32=p;
        प्रति(i=0; i<0x80; ++i) {
            *p++=initial_मानम्;
        }
        प्रति(; i<0xc0; ++i) {
            *p++=error_मानम्;
        }
        /* highValue and reserved values */
        प्रति(i=0; i<UTRIE2_दत्त_GRANULARITY; ++i) {
            *p++=initial_मानम्;
        }
        भङ्ग;
    स्वभाव:
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    प्रत्यागम trie;
}

U_CAPI शून्य U_EXPORT2
utrie2_close(UTrie2 *trie) {
    यदि(trie!=शून्यसूचकः) {
        यदि(trie->is_स्मृतिः_Owned) {
            uprv_free(trie->स्मृतिः);
        }
        यदि(trie->newTrie!=शून्यसूचकः) {
            uprv_free(trie->newTrie->दत्त);
#यदि_परिभाषितम् UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#यदि_अन्तः
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ध्रुव UTrie2 *trie) {
    प्रत्यागम trie->newTrie==शून्यसूचकः;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ध्रुव UTrie2 *trie,
                 शून्य *दत्त, int32_t capacity,
                 UError_संहिता *p_Error_संहिता) {
    /* argument check */
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    यदि( trie==शून्यसूचकः || trie->स्मृतिः==शून्यसूचकः || trie->newTrie!=शून्यसूचकः ||
        capacity<0 || (capacity>0 && (दत्त==शून्यसूचकः || (U_POINTER_आवरणम्_LSB(दत्त, 3)!=0)))
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि(capacity>=trie->length) {
        uprv_memcpy(दत्त, trie->स्मृतिः, trie->length);
    } अन्यथा {
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
    }
    प्रत्यागम trie->length;
}

/* enumeration -------------------------------------------------------------- */

#परिभाषय MIN_मानम्(a, b) ((a)<(b) ? (a) : (b))

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
               UChar32 start, UChar32 सीमा,
               UTrie2_Enum_मानम् *enum_मानम्, UTrie2EnumRange *enumRange, ध्रुव शून्य *context) {
    ध्रुव uint32_t *data32;
    ध्रुव uint16_t *idx;

    uint32_t value, पूर्व_मानम्, initial_मानम्;
    UChar32 c, पूर्व, उच्चः_Start;
    int32_t j, i2_खण्डम्, पूर्व_I2_खण्डम्, index2_Null_विस्थापनम्, खण्डम्, पूर्व_खण्डम्, null_खण्डम्;

    यदि(enumRange==शून्यसूचकः) {
        प्रत्यागम;
    }
    यदि(enum_मानम्==शून्यसूचकः) {
        enum_मानम्=enum_Same_मानम्;
    }

    यदि(trie->newTrie==शून्यसूचकः) {
        /* frozen trie */
        idx=trie->सूचकांक;
        U_ASSERT(idx!=शून्यसूचकः); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2_Null_विस्थापनम्=trie->index2_Null_विस्थापनम्;
        null_खण्डम्=trie->दत्त_Null_विस्थापनम्;
    } अन्यथा {
        /* unfrozen, mutable trie */
        idx=शून्यसूचकः;
        data32=trie->newTrie->दत्त;
        U_ASSERT(data32!=शून्यसूचकः); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2_Null_विस्थापनम्=trie->newTrie->index2_Null_विस्थापनम्;
        null_खण्डम्=trie->newTrie->दत्त_Null_विस्थापनम्;
    }

    उच्चः_Start=trie->उच्चः_Start;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_मानम्=enum_मानम्(context, trie->initial_मानम्);

    /* set variables for previous range */
    पूर्व_I2_खण्डम्=-1;
    पूर्व_खण्डम्=-1;
    पूर्व=start;
    पूर्व_मानम्=0;

    /* enumerate index-2 blocks */
    प्रति(c=start; c<सीमा && c<उच्चः_Start;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 अस्थायी_सीमा=c+UTRIE2_CP_प्रत्येकम्_सूचकांक_1_प्रविष्टिः;
        यदि(सीमा<अस्थायी_सीमा) {
            अस्थायी_सीमा=सीमा;
        }
        यदि(c<=0xffff) {
            यदि(!U_IS_SURROGATE(c)) {
                i2_खण्डम्=c>>UTRIE2_सरणम्_2;
            } अन्यथा यदि(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2_खण्डम्=UTRIE2_LSCP_सूचकांक_2_विस्थापनम्;
                अस्थायी_सीमा=MIN_मानम्(0xdc00, सीमा);
            } अन्यथा {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2_खण्डम्=0xd800>>UTRIE2_सरणम्_2;
                अस्थायी_सीमा=MIN_मानम्(0xe000, सीमा);
            }
        } अन्यथा {
            /* supplementary code points */
            यदि(idx!=शून्यसूचकः) {
                i2_खण्डम्=idx[(UTRIE2_सूचकांक_1_विस्थापनम्-UTRIE2_OMITTED_BMP_सूचकांक_1_दीर्घता)+
                              (c>>UTRIE2_सरणम्_1)];
            } अन्यथा {
                i2_खण्डम्=trie->newTrie->index1[c>>UTRIE2_सरणम्_1];
            }
            यदि(i2_खण्डम्==पूर्व_I2_खण्डम् && (c-पूर्व)>=UTRIE2_CP_प्रत्येकम्_सूचकांक_1_प्रविष्टिः) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_प्रत्येकम्_सूचकांक_1_प्रविष्टिः;
                अनुवर्त;
            }
        }
        पूर्व_I2_खण्डम्=i2_खण्डम्;
        यदि(i2_खण्डम्==index2_Null_विस्थापनम्) {
            /* this is the null index-2 block */
            यदि(पूर्व_मानम्!=initial_मानम्) {
                यदि(पूर्व<c && !enumRange(context, पूर्व, c-1, पूर्व_मानम्)) {
                    प्रत्यागम;
                }
                पूर्व_खण्डम्=null_खण्डम्;
                पूर्व=c;
                पूर्व_मानम्=initial_मानम्;
            }
            c+=UTRIE2_CP_प्रत्येकम्_सूचकांक_1_प्रविष्टिः;
        } अन्यथा {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2_सीमा;
            i2=(c>>UTRIE2_सरणम्_2)&UTRIE2_सूचकांक_2_आवरणम्;
            यदि((c>>UTRIE2_सरणम्_1)==(अस्थायी_सीमा>>UTRIE2_सरणम्_1)) {
                i2_सीमा=(अस्थायी_सीमा>>UTRIE2_सरणम्_2)&UTRIE2_सूचकांक_2_आवरणम्;
            } अन्यथा {
                i2_सीमा=UTRIE2_सूचकांक_2_खण्डम्_दीर्घता;
            }
            प्रति(; i2<i2_सीमा; ++i2) {
                यदि(idx!=शून्यसूचकः) {
                    खण्डम् = static_cast<int32_t>(idx[i2_खण्डम् + i2]) << UTRIE2_सूचकांक_सरणम्;
                } अन्यथा {
                    खण्डम्=trie->newTrie->index2[i2_खण्डम्+i2];
                }
                यदि(खण्डम्==पूर्व_खण्डम् && (c-पूर्व)>=UTRIE2_दत्त_खण्डम्_दीर्घता) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_दत्त_खण्डम्_दीर्घता;
                    अनुवर्त;
                }
                पूर्व_खण्डम्=खण्डम्;
                यदि(खण्डम्==null_खण्डम्) {
                    /* this is the null data block */
                    यदि(पूर्व_मानम्!=initial_मानम्) {
                        यदि(पूर्व<c && !enumRange(context, पूर्व, c-1, पूर्व_मानम्)) {
                            प्रत्यागम;
                        }
                        पूर्व=c;
                        पूर्व_मानम्=initial_मानम्;
                    }
                    c+=UTRIE2_दत्त_खण्डम्_दीर्घता;
                } अन्यथा {
                    प्रति(j=0; j<UTRIE2_दत्त_खण्डम्_दीर्घता; ++j) {
                        value=enum_मानम्(context, data32!=शून्यसूचकः ? data32[खण्डम्+j] : idx[खण्डम्+j]);
                        यदि(value!=पूर्व_मानम्) {
                            यदि(पूर्व<c && !enumRange(context, पूर्व, c-1, पूर्व_मानम्)) {
                                प्रत्यागम;
                            }
                            पूर्व=c;
                            पूर्व_मानम्=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    यदि(c>सीमा) {
        c=सीमा;  /* could be higher if in the index2NullOffset */
    } अन्यथा यदि(c<सीमा) {
        /* c==highStart<limit */
        uint32_t उच्चः_मानम्;
        यदि(idx!=शून्यसूचकः) {
            उच्चः_मानम्=
                data32!=शून्यसूचकः ?
                    data32[trie->उच्चः_मानम्_सूचकांक] :
                    idx[trie->उच्चः_मानम्_सूचकांक];
        } अन्यथा {
            उच्चः_मानम्=trie->newTrie->दत्त[trie->newTrie->दत्त_दीर्घता-UTRIE2_दत्त_GRANULARITY];
        }
        value=enum_मानम्(context, उच्चः_मानम्);
        यदि(value!=पूर्व_मानम्) {
            यदि(पूर्व<c && !enumRange(context, पूर्व, c-1, पूर्व_मानम्)) {
                प्रत्यागम;
            }
            पूर्व=c;
            पूर्व_मानम्=value;
        }
        c=सीमा;
    }

    /* deliver last range */
    enumRange(context, पूर्व, c-1, पूर्व_मानम्);
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
    यदि(!U16_IS_LEAD(lead)) {
        प्रत्यागम;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_मानम्, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    संहिता_Point_सीमा=संहिता_Point_Start;
    यदि(start>=संहिता_Point_Start) {
        संहिता_Point=U_SENTINEL;
        प्रत्यागम static_cast<uint16_t>(trie->error_मानम्);
    }
    uint16_t परिणाम;
    UTRIE2_U16_PREV16(trie, start, संहिता_Point_Start, संहिता_Point, परिणाम);
    प्रत्यागम परिणाम;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    संहिता_Point_Start=संहिता_Point_सीमा;
    यदि(संहिता_Point_सीमा==सीमा) {
        संहिता_Point=U_SENTINEL;
        प्रत्यागम static_cast<uint16_t>(trie->error_मानम्);
    }
    uint16_t परिणाम;
    UTRIE2_U16_NEXT16(trie, संहिता_Point_सीमा, सीमा, संहिता_Point, परिणाम);
    प्रत्यागम परिणाम;
}

U_NAMESPACE_अन्तः

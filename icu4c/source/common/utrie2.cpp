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
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
#ضمن "unicode/umutablecptrie.h"
#جيڪڏهن_پورو
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
    int32_t i2, بلاڪ;

    إذا(c>=trie->مٿاهون_شروعات && (!U_IS_LEAD(c) || fromLSCP)) {
        أرجع trie->معلومات[trie->معلومات_ڊيگهائي-UTRIE2_معلومات_GRANULARITY];
    }

    إذا(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_اشاريو_2_وٿي-(0xd800>>UTRIE2_سرڪايو_2))+
            (c>>UTRIE2_سرڪايو_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_سرڪايو_1]+
            ((c>>UTRIE2_سرڪايو_2)&UTRIE2_اشاريو_2_نقاب);
    }
    بلاڪ=trie->index2[i2];
    أرجع trie->معلومات[بلاڪ+(c&UTRIE2_معلومات_نقاب)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ثابت UTrie2 *trie, UChar32 c) {
    إذا(trie->data16!=خالي_اشارو) {
        أرجع UTRIE2_GET16(trie, c);
    } وإلا إذا(trie->data32!=خالي_اشارو) {
        أرجع UTRIE2_GET32(trie, c);
    } وإلا إذا((uint32_t)c>0x10ffff) {
        أرجع trie->error_قدر;
    } وإلا {
        أرجع get32(trie->newTrie, c, سچ);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_ڪوڊ_Unit(ثابت UTrie2 *trie, UChar32 c) {
    إذا(!U_IS_LEAD(c)) {
        أرجع trie->error_قدر;
    }
    إذا(trie->data16!=خالي_اشارو) {
        أرجع UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } وإلا إذا(trie->data32!=خالي_اشارو) {
        أرجع UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } وإلا {
        أرجع get32(trie->newTrie, c, خطأ);
    }
}

ساكن مضمن int32_t
u8_اشاريو(ثابت UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_اشاريو_FROM_CP(
            trie,
            trie->data32==خالي_اشارو ? trie->اشاريو_ڊيگهائي : 0,
            c);
    أرجع (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_اندروني_U8_ايندڙ_اشاريو(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *src, ثابت uint8_t *حد_بندي) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    إذا((حد_بندي-src)<=7) {
        length=(int32_t)(حد_بندي-src);
    } وإلا {
        length=7;
    }
    c=utf8_ايندڙ_Char_محفوظ_Body(src, &i, length, c, -1);
    أرجع u8_اشاريو(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_اندروني_U8_Prev_اشاريو(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *شروعات, ثابت uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    إذا((src-شروعات)<=7) {
        i=length=(int32_t)(src-شروعات);
    } وإلا {
        i=length=7;
        شروعات=src-7;
    }
    c=utf8_prev_Char_محفوظ_Body(شروعات, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    أرجع u8_اشاريو(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_قدر_بٽ قدر_بٽ,
                          ثابت فراغ *معلومات, int32_t length, int32_t *p_Actual_ڊيگهائي,
                          UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UTrie2_سرو *سرو;
    ثابت uint16_t *p16;
    int32_t actual_ڊيگهائي;

    UTrie2 عارضي_Trie;
    UTrie2 *trie;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }

    إذا( length<=0 || (U_اشارو_نقاب_LSB(معلومات, 3)!=0) ||
        قدر_بٽ<0 || UTRIE2_ڳڻپ_قدر_بٽ<=قدر_بٽ
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    /* enough data for a trie header? */
    إذا(length<(int32_t)حجم(UTrie2_سرو)) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع خالي_اشارو;
    }

    /* check the signature */
    سرو=(ثابت UTrie2_سرو *)معلومات;
    إذا(سرو->signature!=UTRIE2_SIG) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع خالي_اشارو;
    }

    /* get the options */
    إذا(قدر_بٽ!=(UTrie2_قدر_بٽ)(سرو->options&UTRIE2_OPTIONS_قدر_بٽ_نقاب)) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع خالي_اشارو;
    }

    /* get the length values and offsets */
    uprv_memset(&عارضي_Trie, 0, حجم(عارضي_Trie));
    عارضي_Trie.اشاريو_ڊيگهائي=سرو->اشاريو_ڊيگهائي;
    عارضي_Trie.معلومات_ڊيگهائي=سرو->shifted_معلومات_ڊيگهائي<<UTRIE2_اشاريو_سرڪايو;
    عارضي_Trie.index2_Null_وٿي=سرو->index2_Null_وٿي;
    عارضي_Trie.معلومات_Null_وٿي=سرو->معلومات_Null_وٿي;

    عارضي_Trie.مٿاهون_شروعات=سرو->shifted_مٿاهون_شروعات<<UTRIE2_سرڪايو_1;
    عارضي_Trie.مٿاهون_قدر_اشاريو=عارضي_Trie.معلومات_ڊيگهائي-UTRIE2_معلومات_GRANULARITY;
    إذا(قدر_بٽ==UTRIE2_16_قدر_بٽ) {
        عارضي_Trie.مٿاهون_قدر_اشاريو+=عارضي_Trie.اشاريو_ڊيگهائي;
    }

    /* calculate the actual length */
    actual_ڊيگهائي=(int32_t)حجم(UTrie2_سرو)+عارضي_Trie.اشاريو_ڊيگهائي*2;
    إذا(قدر_بٽ==UTRIE2_16_قدر_بٽ) {
        actual_ڊيگهائي+=عارضي_Trie.معلومات_ڊيگهائي*2;
    } وإلا {
        actual_ڊيگهائي+=عارضي_Trie.معلومات_ڊيگهائي*4;
    }
    إذا(length<actual_ڊيگهائي) {
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        أرجع خالي_اشارو;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memcpy(trie, &عارضي_Trie, حجم(عارضي_Trie));
    trie->يادگيري=(uint32_t *)معلومات;
    trie->length=actual_ڊيگهائي;
    trie->is_يادگيري_Owned=خطأ;
#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    trie->name="fromSerialized";
#جيڪڏهن_پورو

    /* set the pointers to its index and data arrays */
    p16=(ثابت uint16_t *)(سرو+1);
    trie->اشاريو=p16;
    p16+=trie->اشاريو_ڊيگهائي;

    /* get the data */
    بدل(قدر_بٽ) {
    حالة UTRIE2_16_قدر_بٽ:
        trie->data16=p16;
        trie->data32=خالي_اشارو;
        trie->initial_قدر=trie->اشاريو[trie->معلومات_Null_وٿي];
        trie->error_قدر=trie->data16[UTRIE2_BAD_UTF8_معلومات_وٿي];
        اكسر;
    حالة UTRIE2_32_قدر_بٽ:
        trie->data16=خالي_اشارو;
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_قدر=trie->data32[trie->معلومات_Null_وٿي];
        trie->error_قدر=trie->data32[UTRIE2_BAD_UTF8_معلومات_وٿي];
        اكسر;
    افتراضي:
        *p_Error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع خالي_اشارو;
    }

    إذا(p_Actual_ڊيگهائي!=خالي_اشارو) {
        *p_Actual_ڊيگهائي=actual_ڊيگهائي;
    }
    أرجع trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_قدر_بٽ قدر_بٽ,
                 uint32_t initial_قدر, uint32_t error_قدر,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    UTrie2 *trie;
    UTrie2_سرو *سرو;
    uint32_t *p;
    uint16_t *dest16;
    int32_t اشاريو_ڊيگهائي, معلومات_ڊيگهائي, length, i;
    int32_t معلومات_Move;  /* >0 if the data is moved to the end of the index array */

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }

    إذا(قدر_بٽ<0 || UTRIE2_ڳڻپ_قدر_بٽ<=قدر_بٽ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    /* calculate the total length of the dummy trie data */
    اشاريو_ڊيگهائي=UTRIE2_اشاريو_1_وٿي;
    معلومات_ڊيگهائي=UTRIE2_معلومات_شروعات_وٿي+UTRIE2_معلومات_GRANULARITY;
    length=(int32_t)حجم(UTrie2_سرو)+اشاريو_ڊيگهائي*2;
    إذا(قدر_بٽ==UTRIE2_16_قدر_بٽ) {
        length+=معلومات_ڊيگهائي*2;
    } وإلا {
        length+=معلومات_ڊيگهائي*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==خالي_اشارو) {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    uprv_memset(trie, 0, حجم(UTrie2));
    trie->يادگيري=uprv_malloc(length);
    إذا(trie->يادگيري==خالي_اشارو) {
        uprv_free(trie);
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    trie->length=length;
    trie->is_يادگيري_Owned=سچ;

    /* set the UTrie2 fields */
    إذا(قدر_بٽ==UTRIE2_16_قدر_بٽ) {
        معلومات_Move=اشاريو_ڊيگهائي;
    } وإلا {
        معلومات_Move=0;
    }

    trie->اشاريو_ڊيگهائي=اشاريو_ڊيگهائي;
    trie->معلومات_ڊيگهائي=معلومات_ڊيگهائي;
    trie->index2_Null_وٿي=UTRIE2_اشاريو_2_وٿي;
    trie->معلومات_Null_وٿي=(uint16_t)معلومات_Move;
    trie->initial_قدر=initial_قدر;
    trie->error_قدر=error_قدر;
    trie->مٿاهون_شروعات=0;
    trie->مٿاهون_قدر_اشاريو=معلومات_Move+UTRIE2_معلومات_شروعات_وٿي;
#جيڪڏهن_وصف_ٿيل UTRIE2_DEBUG
    trie->name="dummy";
#جيڪڏهن_پورو

    /* set the header fields */
    سرو=(UTrie2_سرو *)trie->يادگيري;

    سرو->signature=UTRIE2_SIG; /* "Tri2" */
    سرو->options=(uint16_t)قدر_بٽ;

    سرو->اشاريو_ڊيگهائي=(uint16_t)اشاريو_ڊيگهائي;
    سرو->shifted_معلومات_ڊيگهائي=(uint16_t)(معلومات_ڊيگهائي>>UTRIE2_اشاريو_سرڪايو);
    سرو->index2_Null_وٿي=(uint16_t)UTRIE2_اشاريو_2_وٿي;
    سرو->معلومات_Null_وٿي=(uint16_t)معلومات_Move;
    سرو->shifted_مٿاهون_شروعات=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرو+1);
    trie->اشاريو=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    لكل(i=0; i<UTRIE2_اشاريو_2_BMP_ڊيگهائي; ++i) {
        *dest16++=(uint16_t)(معلومات_Move>>UTRIE2_اشاريو_سرڪايو);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(معلومات_Move+UTRIE2_BAD_UTF8_معلومات_وٿي);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)معلومات_Move;
    }

    /* write the 16/32-bit data array */
    بدل(قدر_بٽ) {
    حالة UTRIE2_16_قدر_بٽ:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=خالي_اشارو;
        لكل(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_قدر;
        }
        لكل(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_قدر;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_معلومات_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_قدر;
        }
        اكسر;
    حالة UTRIE2_32_قدر_بٽ:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=خالي_اشارو;
        trie->data32=p;
        لكل(i=0; i<0x80; ++i) {
            *p++=initial_قدر;
        }
        لكل(; i<0xc0; ++i) {
            *p++=error_قدر;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_معلومات_GRANULARITY; ++i) {
            *p++=initial_قدر;
        }
        اكسر;
    افتراضي:
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
utrie2_close(UTrie2 *trie) {
    إذا(trie!=خالي_اشارو) {
        إذا(trie->is_يادگيري_Owned) {
            uprv_free(trie->يادگيري);
        }
        إذا(trie->newTrie!=خالي_اشارو) {
            uprv_free(trie->newTrie->معلومات);
#جيڪڏهن_وصف_ٿيل UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#جيڪڏهن_پورو
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ثابت UTrie2 *trie) {
    أرجع trie->newTrie==خالي_اشارو;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ثابت UTrie2 *trie,
                 فراغ *معلومات, int32_t capacity,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    /* argument check */
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    إذا( trie==خالي_اشارو || trie->يادگيري==خالي_اشارو || trie->newTrie!=خالي_اشارو ||
        capacity<0 || (capacity>0 && (معلومات==خالي_اشارو || (U_اشارو_نقاب_LSB(معلومات, 3)!=0)))
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    إذا(capacity>=trie->length) {
        uprv_memcpy(معلومات, trie->يادگيري, trie->length);
    } وإلا {
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
    }
    أرجع trie->length;
}

/* enumeration -------------------------------------------------------------- */

#وصف_ڪريو MIN_قدر(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ساكن uint32_t U_CALLCONV
enum_Same_قدر(ثابت فراغ * /*context*/, uint32_t value) {
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
               UChar32 شروعات, UChar32 حد_بندي,
               UTrie2_Enum_قدر *enum_قدر, UTrie2EnumRange *enumRange, ثابت فراغ *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prev_قدر, initial_قدر;
    UChar32 c, prev, مٿاهون_شروعات;
    int32_t j, i2_بلاڪ, prev_I2_بلاڪ, index2_Null_وٿي, بلاڪ, prev_بلاڪ, null_بلاڪ;

    إذا(enumRange==خالي_اشارو) {
        أرجع;
    }
    إذا(enum_قدر==خالي_اشارو) {
        enum_قدر=enum_Same_قدر;
    }

    إذا(trie->newTrie==خالي_اشارو) {
        /* frozen trie */
        idx=trie->اشاريو;
        U_ASSERT(idx!=خالي_اشارو); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2_Null_وٿي=trie->index2_Null_وٿي;
        null_بلاڪ=trie->معلومات_Null_وٿي;
    } وإلا {
        /* unfrozen, mutable trie */
        idx=خالي_اشارو;
        data32=trie->newTrie->معلومات;
        U_ASSERT(data32!=خالي_اشارو); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2_Null_وٿي=trie->newTrie->index2_Null_وٿي;
        null_بلاڪ=trie->newTrie->معلومات_Null_وٿي;
    }

    مٿاهون_شروعات=trie->مٿاهون_شروعات;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_قدر=enum_قدر(context, trie->initial_قدر);

    /* set variables for previous range */
    prev_I2_بلاڪ=-1;
    prev_بلاڪ=-1;
    prev=شروعات;
    prev_قدر=0;

    /* enumerate index-2 blocks */
    لكل(c=شروعات; c<حد_بندي && c<مٿاهون_شروعات;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 عارضي_حد_بندي=c+UTRIE2_CP_في_اشاريو_1_داخلا;
        إذا(حد_بندي<عارضي_حد_بندي) {
            عارضي_حد_بندي=حد_بندي;
        }
        إذا(c<=0xffff) {
            إذا(!U_IS_SURROGATE(c)) {
                i2_بلاڪ=c>>UTRIE2_سرڪايو_2;
            } وإلا إذا(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2_بلاڪ=UTRIE2_LSCP_اشاريو_2_وٿي;
                عارضي_حد_بندي=MIN_قدر(0xdc00, حد_بندي);
            } وإلا {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2_بلاڪ=0xd800>>UTRIE2_سرڪايو_2;
                عارضي_حد_بندي=MIN_قدر(0xe000, حد_بندي);
            }
        } وإلا {
            /* supplementary code points */
            إذا(idx!=خالي_اشارو) {
                i2_بلاڪ=idx[(UTRIE2_اشاريو_1_وٿي-UTRIE2_OMITTED_BMP_اشاريو_1_ڊيگهائي)+
                              (c>>UTRIE2_سرڪايو_1)];
            } وإلا {
                i2_بلاڪ=trie->newTrie->index1[c>>UTRIE2_سرڪايو_1];
            }
            إذا(i2_بلاڪ==prev_I2_بلاڪ && (c-prev)>=UTRIE2_CP_في_اشاريو_1_داخلا) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_في_اشاريو_1_داخلا;
                استمر;
            }
        }
        prev_I2_بلاڪ=i2_بلاڪ;
        إذا(i2_بلاڪ==index2_Null_وٿي) {
            /* this is the null index-2 block */
            إذا(prev_قدر!=initial_قدر) {
                إذا(prev<c && !enumRange(context, prev, c-1, prev_قدر)) {
                    أرجع;
                }
                prev_بلاڪ=null_بلاڪ;
                prev=c;
                prev_قدر=initial_قدر;
            }
            c+=UTRIE2_CP_في_اشاريو_1_داخلا;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2_حد_بندي;
            i2=(c>>UTRIE2_سرڪايو_2)&UTRIE2_اشاريو_2_نقاب;
            إذا((c>>UTRIE2_سرڪايو_1)==(عارضي_حد_بندي>>UTRIE2_سرڪايو_1)) {
                i2_حد_بندي=(عارضي_حد_بندي>>UTRIE2_سرڪايو_2)&UTRIE2_اشاريو_2_نقاب;
            } وإلا {
                i2_حد_بندي=UTRIE2_اشاريو_2_بلاڪ_ڊيگهائي;
            }
            لكل(; i2<i2_حد_بندي; ++i2) {
                إذا(idx!=خالي_اشارو) {
                    بلاڪ = static_cast<int32_t>(idx[i2_بلاڪ + i2]) << UTRIE2_اشاريو_سرڪايو;
                } وإلا {
                    بلاڪ=trie->newTrie->index2[i2_بلاڪ+i2];
                }
                إذا(بلاڪ==prev_بلاڪ && (c-prev)>=UTRIE2_معلومات_بلاڪ_ڊيگهائي) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
                    استمر;
                }
                prev_بلاڪ=بلاڪ;
                إذا(بلاڪ==null_بلاڪ) {
                    /* this is the null data block */
                    إذا(prev_قدر!=initial_قدر) {
                        إذا(prev<c && !enumRange(context, prev, c-1, prev_قدر)) {
                            أرجع;
                        }
                        prev=c;
                        prev_قدر=initial_قدر;
                    }
                    c+=UTRIE2_معلومات_بلاڪ_ڊيگهائي;
                } وإلا {
                    لكل(j=0; j<UTRIE2_معلومات_بلاڪ_ڊيگهائي; ++j) {
                        value=enum_قدر(context, data32!=خالي_اشارو ? data32[بلاڪ+j] : idx[بلاڪ+j]);
                        إذا(value!=prev_قدر) {
                            إذا(prev<c && !enumRange(context, prev, c-1, prev_قدر)) {
                                أرجع;
                            }
                            prev=c;
                            prev_قدر=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    إذا(c>حد_بندي) {
        c=حد_بندي;  /* could be higher if in the index2NullOffset */
    } وإلا إذا(c<حد_بندي) {
        /* c==highStart<limit */
        uint32_t مٿاهون_قدر;
        إذا(idx!=خالي_اشارو) {
            مٿاهون_قدر=
                data32!=خالي_اشارو ?
                    data32[trie->مٿاهون_قدر_اشاريو] :
                    idx[trie->مٿاهون_قدر_اشاريو];
        } وإلا {
            مٿاهون_قدر=trie->newTrie->معلومات[trie->newTrie->معلومات_ڊيگهائي-UTRIE2_معلومات_GRANULARITY];
        }
        value=enum_قدر(context, مٿاهون_قدر);
        إذا(value!=prev_قدر) {
            إذا(prev<c && !enumRange(context, prev, c-1, prev_قدر)) {
                أرجع;
            }
            prev=c;
            prev_قدر=value;
        }
        c=حد_بندي;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_قدر);
}

U_CAPI فراغ U_EXPORT2
utrie2_enum(ثابت UTrie2 *trie,
            UTrie2_Enum_قدر *enum_قدر, UTrie2EnumRange *enumRange, ثابت فراغ *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_قدر, enumRange, context);
}

U_CAPI فراغ U_EXPORT2
utrie2_enumForLeadSurrogate(ثابت UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_قدر *enum_قدر, UTrie2EnumRange *enumRange,
                            ثابت فراغ *context) {
    إذا(!U16_IS_LEAD(lead)) {
        أرجع;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_قدر, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    ڪوڊ_نقطو_حد_بندي=ڪوڊ_نقطو_شروعات;
    إذا(شروعات>=ڪوڊ_نقطو_شروعات) {
        ڪوڊ_نقطو=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->error_قدر);
    }
    uint16_t نتيجو;
    UTRIE2_U16_PREV16(trie, شروعات, ڪوڊ_نقطو_شروعات, ڪوڊ_نقطو, نتيجو);
    أرجع نتيجو;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    ڪوڊ_نقطو_شروعات=ڪوڊ_نقطو_حد_بندي;
    إذا(ڪوڊ_نقطو_حد_بندي==حد_بندي) {
        ڪوڊ_نقطو=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->error_قدر);
    }
    uint16_t نتيجو;
    UTRIE2_U16_NEXT16(trie, ڪوڊ_نقطو_حد_بندي, حد_بندي, ڪوڊ_نقطو, نتيجو);
    أرجع نتيجو;
}

U_NAMESPACE_پڇاڙي

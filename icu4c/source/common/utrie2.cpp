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
#جے_تعریف_شدہ UCPTRIE_DEBUG
#ضمن "unicode/umutablecptrie.h"
#جے_انت
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
    int32_t i2, بلاک;

    إذا(c>=trie->بلند_شروع && (!U_IS_LEAD(c) || fromLSCP)) {
        أرجع trie->انکڑے[trie->انکڑے_لمائی-UTRIE2_انکڑے_GRANULARITY];
    }

    إذا(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_اشاریہ_2_وتھ-(0xd800>>UTRIE2_کھسکاؤ_2))+
            (c>>UTRIE2_کھسکاؤ_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_کھسکاؤ_1]+
            ((c>>UTRIE2_کھسکاؤ_2)&UTRIE2_اشاریہ_2_نقاب);
    }
    بلاک=trie->index2[i2];
    أرجع trie->انکڑے[بلاک+(c&UTRIE2_انکڑے_نقاب)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ثابت UTrie2 *trie, UChar32 c) {
    إذا(trie->data16!=خالی_سوچک) {
        أرجع UTRIE2_GET16(trie, c);
    } وإلا إذا(trie->data32!=خالی_سوچک) {
        أرجع UTRIE2_GET32(trie, c);
    } وإلا إذا((uint32_t)c>0x10ffff) {
        أرجع trie->error_قدر;
    } وإلا {
        أرجع get32(trie->newTrie, c, سچ);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_کوڈ_Unit(ثابت UTrie2 *trie, UChar32 c) {
    إذا(!U_IS_LEAD(c)) {
        أرجع trie->error_قدر;
    }
    إذا(trie->data16!=خالی_سوچک) {
        أرجع UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } وإلا إذا(trie->data32!=خالی_سوچک) {
        أرجع UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } وإلا {
        أرجع get32(trie->newTrie, c, خطأ);
    }
}

ساكن مضمن int32_t
u8_اشاریہ(ثابت UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_اشاریہ_FROM_CP(
            trie,
            trie->data32==خالی_سوچک ? trie->اشاریہ_لمائی : 0,
            c);
    أرجع (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_اندرونی_U8_اگلا_اشاریہ(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *src, ثابت uint8_t *حد_بندی) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    إذا((حد_بندی-src)<=7) {
        length=(int32_t)(حد_بندی-src);
    } وإلا {
        length=7;
    }
    c=utf8_اگلا_Char_محفوظ_Body(src, &i, length, c, -1);
    أرجع u8_اشاریہ(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_اندرونی_U8_Prev_اشاریہ(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *شروع, ثابت uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    إذا((src-شروع)<=7) {
        i=length=(int32_t)(src-شروع);
    } وإلا {
        i=length=7;
        شروع=src-7;
    }
    c=utf8_prev_Char_محفوظ_Body(شروع, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    أرجع u8_اشاریہ(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_قدر_بٹ قدر_بٹ,
                          ثابت فراغ *انکڑے, int32_t length, int32_t *p_Actual_لمائی,
                          UError_کوڈ *p_Error_کوڈ) {
    ثابت UTrie2_سرخی *سرخی;
    ثابت uint16_t *p16;
    int32_t actual_لمائی;

    UTrie2 عارضی_Trie;
    UTrie2 *trie;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }

    إذا( length<=0 || (U_اشارہ_گر_نقاب_LSB(انکڑے, 3)!=0) ||
        قدر_بٹ<0 || UTRIE2_گنتی_قدر_بٹ<=قدر_بٹ
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    /* enough data for a trie header? */
    إذا(length<(int32_t)حجم(UTrie2_سرخی)) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع خالی_سوچک;
    }

    /* check the signature */
    سرخی=(ثابت UTrie2_سرخی *)انکڑے;
    إذا(سرخی->signature!=UTRIE2_SIG) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع خالی_سوچک;
    }

    /* get the options */
    إذا(قدر_بٹ!=(UTrie2_قدر_بٹ)(سرخی->options&UTRIE2_OPTIONS_قدر_بٹ_نقاب)) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع خالی_سوچک;
    }

    /* get the length values and offsets */
    uprv_memset(&عارضی_Trie, 0, حجم(عارضی_Trie));
    عارضی_Trie.اشاریہ_لمائی=سرخی->اشاریہ_لمائی;
    عارضی_Trie.انکڑے_لمائی=سرخی->shifted_انکڑے_لمائی<<UTRIE2_اشاریہ_کھسکاؤ;
    عارضی_Trie.index2_Null_وتھ=سرخی->index2_Null_وتھ;
    عارضی_Trie.انکڑے_Null_وتھ=سرخی->انکڑے_Null_وتھ;

    عارضی_Trie.بلند_شروع=سرخی->shifted_بلند_شروع<<UTRIE2_کھسکاؤ_1;
    عارضی_Trie.بلند_قدر_اشاریہ=عارضی_Trie.انکڑے_لمائی-UTRIE2_انکڑے_GRANULARITY;
    إذا(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        عارضی_Trie.بلند_قدر_اشاریہ+=عارضی_Trie.اشاریہ_لمائی;
    }

    /* calculate the actual length */
    actual_لمائی=(int32_t)حجم(UTrie2_سرخی)+عارضی_Trie.اشاریہ_لمائی*2;
    إذا(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        actual_لمائی+=عارضی_Trie.انکڑے_لمائی*2;
    } وإلا {
        actual_لمائی+=عارضی_Trie.انکڑے_لمائی*4;
    }
    إذا(length<actual_لمائی) {
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        أرجع خالی_سوچک;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memcpy(trie, &عارضی_Trie, حجم(عارضی_Trie));
    trie->حافظہ=(uint32_t *)انکڑے;
    trie->length=actual_لمائی;
    trie->is_حافظہ_Owned=خطأ;
#جے_تعریف_شدہ UTRIE2_DEBUG
    trie->name="fromSerialized";
#جے_انت

    /* set the pointers to its index and data arrays */
    p16=(ثابت uint16_t *)(سرخی+1);
    trie->اشاریہ=p16;
    p16+=trie->اشاریہ_لمائی;

    /* get the data */
    بدل(قدر_بٹ) {
    حالة UTRIE2_16_قدر_بٹ:
        trie->data16=p16;
        trie->data32=خالی_سوچک;
        trie->initial_قدر=trie->اشاریہ[trie->انکڑے_Null_وتھ];
        trie->error_قدر=trie->data16[UTRIE2_BAD_UTF8_انکڑے_وتھ];
        اكسر;
    حالة UTRIE2_32_قدر_بٹ:
        trie->data16=خالی_سوچک;
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_قدر=trie->data32[trie->انکڑے_Null_وتھ];
        trie->error_قدر=trie->data32[UTRIE2_BAD_UTF8_انکڑے_وتھ];
        اكسر;
    افتراضي:
        *p_Error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع خالی_سوچک;
    }

    إذا(p_Actual_لمائی!=خالی_سوچک) {
        *p_Actual_لمائی=actual_لمائی;
    }
    أرجع trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_قدر_بٹ قدر_بٹ,
                 uint32_t initial_قدر, uint32_t error_قدر,
                 UError_کوڈ *p_Error_کوڈ) {
    UTrie2 *trie;
    UTrie2_سرخی *سرخی;
    uint32_t *p;
    uint16_t *dest16;
    int32_t اشاریہ_لمائی, انکڑے_لمائی, length, i;
    int32_t انکڑے_Move;  /* >0 if the data is moved to the end of the index array */

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }

    إذا(قدر_بٹ<0 || UTRIE2_گنتی_قدر_بٹ<=قدر_بٹ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    /* calculate the total length of the dummy trie data */
    اشاریہ_لمائی=UTRIE2_اشاریہ_1_وتھ;
    انکڑے_لمائی=UTRIE2_انکڑے_شروع_وتھ+UTRIE2_انکڑے_GRANULARITY;
    length=(int32_t)حجم(UTrie2_سرخی)+اشاریہ_لمائی*2;
    إذا(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        length+=انکڑے_لمائی*2;
    } وإلا {
        length+=انکڑے_لمائی*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    uprv_memset(trie, 0, حجم(UTrie2));
    trie->حافظہ=uprv_malloc(length);
    إذا(trie->حافظہ==خالی_سوچک) {
        uprv_free(trie);
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    trie->length=length;
    trie->is_حافظہ_Owned=سچ;

    /* set the UTrie2 fields */
    إذا(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        انکڑے_Move=اشاریہ_لمائی;
    } وإلا {
        انکڑے_Move=0;
    }

    trie->اشاریہ_لمائی=اشاریہ_لمائی;
    trie->انکڑے_لمائی=انکڑے_لمائی;
    trie->index2_Null_وتھ=UTRIE2_اشاریہ_2_وتھ;
    trie->انکڑے_Null_وتھ=(uint16_t)انکڑے_Move;
    trie->initial_قدر=initial_قدر;
    trie->error_قدر=error_قدر;
    trie->بلند_شروع=0;
    trie->بلند_قدر_اشاریہ=انکڑے_Move+UTRIE2_انکڑے_شروع_وتھ;
#جے_تعریف_شدہ UTRIE2_DEBUG
    trie->name="dummy";
#جے_انت

    /* set the header fields */
    سرخی=(UTrie2_سرخی *)trie->حافظہ;

    سرخی->signature=UTRIE2_SIG; /* "Tri2" */
    سرخی->options=(uint16_t)قدر_بٹ;

    سرخی->اشاریہ_لمائی=(uint16_t)اشاریہ_لمائی;
    سرخی->shifted_انکڑے_لمائی=(uint16_t)(انکڑے_لمائی>>UTRIE2_اشاریہ_کھسکاؤ);
    سرخی->index2_Null_وتھ=(uint16_t)UTRIE2_اشاریہ_2_وتھ;
    سرخی->انکڑے_Null_وتھ=(uint16_t)انکڑے_Move;
    سرخی->shifted_بلند_شروع=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرخی+1);
    trie->اشاریہ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    لكل(i=0; i<UTRIE2_اشاریہ_2_BMP_لمائی; ++i) {
        *dest16++=(uint16_t)(انکڑے_Move>>UTRIE2_اشاریہ_کھسکاؤ);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(انکڑے_Move+UTRIE2_BAD_UTF8_انکڑے_وتھ);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)انکڑے_Move;
    }

    /* write the 16/32-bit data array */
    بدل(قدر_بٹ) {
    حالة UTRIE2_16_قدر_بٹ:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=خالی_سوچک;
        لكل(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_قدر;
        }
        لكل(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_قدر;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_انکڑے_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_قدر;
        }
        اكسر;
    حالة UTRIE2_32_قدر_بٹ:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=خالی_سوچک;
        trie->data32=p;
        لكل(i=0; i<0x80; ++i) {
            *p++=initial_قدر;
        }
        لكل(; i<0xc0; ++i) {
            *p++=error_قدر;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_انکڑے_GRANULARITY; ++i) {
            *p++=initial_قدر;
        }
        اكسر;
    افتراضي:
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
utrie2_close(UTrie2 *trie) {
    إذا(trie!=خالی_سوچک) {
        إذا(trie->is_حافظہ_Owned) {
            uprv_free(trie->حافظہ);
        }
        إذا(trie->newTrie!=خالی_سوچک) {
            uprv_free(trie->newTrie->انکڑے);
#جے_تعریف_شدہ UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#جے_انت
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ثابت UTrie2 *trie) {
    أرجع trie->newTrie==خالی_سوچک;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ثابت UTrie2 *trie,
                 فراغ *انکڑے, int32_t capacity,
                 UError_کوڈ *p_Error_کوڈ) {
    /* argument check */
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    إذا( trie==خالی_سوچک || trie->حافظہ==خالی_سوچک || trie->newTrie!=خالی_سوچک ||
        capacity<0 || (capacity>0 && (انکڑے==خالی_سوچک || (U_اشارہ_گر_نقاب_LSB(انکڑے, 3)!=0)))
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(capacity>=trie->length) {
        uprv_memcpy(انکڑے, trie->حافظہ, trie->length);
    } وإلا {
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
    }
    أرجع trie->length;
}

/* enumeration -------------------------------------------------------------- */

#تعریف_کرو MIN_قدر(a, b) ((a)<(b) ? (a) : (b))

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
               UChar32 شروع, UChar32 حد_بندی,
               UTrie2_Enum_قدر *enum_قدر, UTrie2EnumRange *enumRange, ثابت فراغ *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prev_قدر, initial_قدر;
    UChar32 c, prev, بلند_شروع;
    int32_t j, i2_بلاک, prev_I2_بلاک, index2_Null_وتھ, بلاک, prev_بلاک, null_بلاک;

    إذا(enumRange==خالی_سوچک) {
        أرجع;
    }
    إذا(enum_قدر==خالی_سوچک) {
        enum_قدر=enum_Same_قدر;
    }

    إذا(trie->newTrie==خالی_سوچک) {
        /* frozen trie */
        idx=trie->اشاریہ;
        U_ASSERT(idx!=خالی_سوچک); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2_Null_وتھ=trie->index2_Null_وتھ;
        null_بلاک=trie->انکڑے_Null_وتھ;
    } وإلا {
        /* unfrozen, mutable trie */
        idx=خالی_سوچک;
        data32=trie->newTrie->انکڑے;
        U_ASSERT(data32!=خالی_سوچک); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2_Null_وتھ=trie->newTrie->index2_Null_وتھ;
        null_بلاک=trie->newTrie->انکڑے_Null_وتھ;
    }

    بلند_شروع=trie->بلند_شروع;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_قدر=enum_قدر(context, trie->initial_قدر);

    /* set variables for previous range */
    prev_I2_بلاک=-1;
    prev_بلاک=-1;
    prev=شروع;
    prev_قدر=0;

    /* enumerate index-2 blocks */
    لكل(c=شروع; c<حد_بندی && c<بلند_شروع;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 عارضی_حد_بندی=c+UTRIE2_CP_فی_اشاریہ_1_اندراج;
        إذا(حد_بندی<عارضی_حد_بندی) {
            عارضی_حد_بندی=حد_بندی;
        }
        إذا(c<=0xffff) {
            إذا(!U_IS_SURROGATE(c)) {
                i2_بلاک=c>>UTRIE2_کھسکاؤ_2;
            } وإلا إذا(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2_بلاک=UTRIE2_LSCP_اشاریہ_2_وتھ;
                عارضی_حد_بندی=MIN_قدر(0xdc00, حد_بندی);
            } وإلا {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2_بلاک=0xd800>>UTRIE2_کھسکاؤ_2;
                عارضی_حد_بندی=MIN_قدر(0xe000, حد_بندی);
            }
        } وإلا {
            /* supplementary code points */
            إذا(idx!=خالی_سوچک) {
                i2_بلاک=idx[(UTRIE2_اشاریہ_1_وتھ-UTRIE2_OMITTED_BMP_اشاریہ_1_لمائی)+
                              (c>>UTRIE2_کھسکاؤ_1)];
            } وإلا {
                i2_بلاک=trie->newTrie->index1[c>>UTRIE2_کھسکاؤ_1];
            }
            إذا(i2_بلاک==prev_I2_بلاک && (c-prev)>=UTRIE2_CP_فی_اشاریہ_1_اندراج) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_فی_اشاریہ_1_اندراج;
                استمر;
            }
        }
        prev_I2_بلاک=i2_بلاک;
        إذا(i2_بلاک==index2_Null_وتھ) {
            /* this is the null index-2 block */
            إذا(prev_قدر!=initial_قدر) {
                إذا(prev<c && !enumRange(context, prev, c-1, prev_قدر)) {
                    أرجع;
                }
                prev_بلاک=null_بلاک;
                prev=c;
                prev_قدر=initial_قدر;
            }
            c+=UTRIE2_CP_فی_اشاریہ_1_اندراج;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2_حد_بندی;
            i2=(c>>UTRIE2_کھسکاؤ_2)&UTRIE2_اشاریہ_2_نقاب;
            إذا((c>>UTRIE2_کھسکاؤ_1)==(عارضی_حد_بندی>>UTRIE2_کھسکاؤ_1)) {
                i2_حد_بندی=(عارضی_حد_بندی>>UTRIE2_کھسکاؤ_2)&UTRIE2_اشاریہ_2_نقاب;
            } وإلا {
                i2_حد_بندی=UTRIE2_اشاریہ_2_بلاک_لمائی;
            }
            لكل(; i2<i2_حد_بندی; ++i2) {
                إذا(idx!=خالی_سوچک) {
                    بلاک = static_cast<int32_t>(idx[i2_بلاک + i2]) << UTRIE2_اشاریہ_کھسکاؤ;
                } وإلا {
                    بلاک=trie->newTrie->index2[i2_بلاک+i2];
                }
                إذا(بلاک==prev_بلاک && (c-prev)>=UTRIE2_انکڑے_بلاک_لمائی) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_انکڑے_بلاک_لمائی;
                    استمر;
                }
                prev_بلاک=بلاک;
                إذا(بلاک==null_بلاک) {
                    /* this is the null data block */
                    إذا(prev_قدر!=initial_قدر) {
                        إذا(prev<c && !enumRange(context, prev, c-1, prev_قدر)) {
                            أرجع;
                        }
                        prev=c;
                        prev_قدر=initial_قدر;
                    }
                    c+=UTRIE2_انکڑے_بلاک_لمائی;
                } وإلا {
                    لكل(j=0; j<UTRIE2_انکڑے_بلاک_لمائی; ++j) {
                        value=enum_قدر(context, data32!=خالی_سوچک ? data32[بلاک+j] : idx[بلاک+j]);
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

    إذا(c>حد_بندی) {
        c=حد_بندی;  /* could be higher if in the index2NullOffset */
    } وإلا إذا(c<حد_بندی) {
        /* c==highStart<limit */
        uint32_t بلند_قدر;
        إذا(idx!=خالی_سوچک) {
            بلند_قدر=
                data32!=خالی_سوچک ?
                    data32[trie->بلند_قدر_اشاریہ] :
                    idx[trie->بلند_قدر_اشاریہ];
        } وإلا {
            بلند_قدر=trie->newTrie->انکڑے[trie->newTrie->انکڑے_لمائی-UTRIE2_انکڑے_GRANULARITY];
        }
        value=enum_قدر(context, بلند_قدر);
        إذا(value!=prev_قدر) {
            إذا(prev<c && !enumRange(context, prev, c-1, prev_قدر)) {
                أرجع;
            }
            prev=c;
            prev_قدر=value;
        }
        c=حد_بندی;
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
    کوڈ_نقطہ_حد_بندی=کوڈ_نقطہ_شروع;
    إذا(شروع>=کوڈ_نقطہ_شروع) {
        کوڈ_نقطہ=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->error_قدر);
    }
    uint16_t نتیجہ;
    UTRIE2_U16_PREV16(trie, شروع, کوڈ_نقطہ_شروع, کوڈ_نقطہ, نتیجہ);
    أرجع نتیجہ;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    کوڈ_نقطہ_شروع=کوڈ_نقطہ_حد_بندی;
    إذا(کوڈ_نقطہ_حد_بندی==حد_بندی) {
        کوڈ_نقطہ=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->error_قدر);
    }
    uint16_t نتیجہ;
    UTRIE2_U16_NEXT16(trie, کوڈ_نقطہ_حد_بندی, حد_بندی, کوڈ_نقطہ, نتیجہ);
    أرجع نتیجہ;
}

U_NAMESPACE_سرا

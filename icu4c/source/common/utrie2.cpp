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
#اگر_تعریف_شدہ UCPTRIE_DEBUG
#شامل "unicode/umutablecptrie.h"
#اگر_ختم
#شامل "unicode/utf.h"
#شامل "unicode/utf8.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "utrie2.h"
#شامل "utrie2_impl.h"
#شامل "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

ساکن uint32_t
get32(ثابت UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, بلاک;

    اگر(c>=trie->بلند_آغاز && (!U_IS_LEAD(c) || fromLSCP)) {
        واپس trie->ڈیٹا[trie->ڈیٹا_لمبائی-UTRIE2_ڈیٹا_GRANULARITY];
    }

    اگر(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_اشاریہ_2_انحراف-(0xd800>>UTRIE2_سرکاؤ_2))+
            (c>>UTRIE2_سرکاؤ_2);
    } ورنہ {
        i2=trie->index1[c>>UTRIE2_سرکاؤ_1]+
            ((c>>UTRIE2_سرکاؤ_2)&UTRIE2_اشاریہ_2_نقاب);
    }
    بلاک=trie->index2[i2];
    واپس trie->ڈیٹا[بلاک+(c&UTRIE2_ڈیٹا_نقاب)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ثابت UTrie2 *trie, UChar32 c) {
    اگر(trie->data16!=خالی_اشاریہ) {
        واپس UTRIE2_GET16(trie, c);
    } ورنہ اگر(trie->data32!=خالی_اشاریہ) {
        واپس UTRIE2_GET32(trie, c);
    } ورنہ اگر((uint32_t)c>0x10ffff) {
        واپس trie->error_قدر;
    } ورنہ {
        واپس get32(trie->newTrie, c, سچ);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_رمز_Unit(ثابت UTrie2 *trie, UChar32 c) {
    اگر(!U_IS_LEAD(c)) {
        واپس trie->error_قدر;
    }
    اگر(trie->data16!=خالی_اشاریہ) {
        واپس UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } ورنہ اگر(trie->data32!=خالی_اشاریہ) {
        واپس UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } ورنہ {
        واپس get32(trie->newTrie, c, جھوٹ);
    }
}

ساکن اندرونی int32_t
u8_اشاریہ(ثابت UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_اشاریہ_FROM_CP(
            trie,
            trie->data32==خالی_اشاریہ ? trie->اشاریہ_لمبائی : 0,
            c);
    واپس (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_اندرونی_U8_اگلا_اشاریہ(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *src, ثابت uint8_t *حد_بندی) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    اگر((حد_بندی-src)<=7) {
        length=(int32_t)(حد_بندی-src);
    } ورنہ {
        length=7;
    }
    c=utf8_اگلا_Char_محفوظ_Body(src, &i, length, c, -1);
    واپس u8_اشاریہ(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_اندرونی_U8_پچھلا_اشاریہ(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *آغاز, ثابت uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    اگر((src-آغاز)<=7) {
        i=length=(int32_t)(src-آغاز);
    } ورنہ {
        i=length=7;
        آغاز=src-7;
    }
    c=utf8_پچھلا_Char_محفوظ_Body(آغاز, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    واپس u8_اشاریہ(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_قدر_بٹ قدر_بٹ,
                          ثابت خالی *ڈیٹا, int32_t length, int32_t *p_Actual_لمبائی,
                          UError_رمز *p_Error_رمز) {
    ثابت UTrie2_سرخی *سرخی;
    ثابت uint16_t *p16;
    int32_t actual_لمبائی;

    UTrie2 عارضی_Trie;
    UTrie2 *trie;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }

    اگر( length<=0 || (U_اشارہ_گر_نقاب_LSB(ڈیٹا, 3)!=0) ||
        قدر_بٹ<0 || UTRIE2_گنتی_قدر_بٹ<=قدر_بٹ
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    /* enough data for a trie header? */
    اگر(length<(int32_t)حجم(UTrie2_سرخی)) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس خالی_اشاریہ;
    }

    /* check the signature */
    سرخی=(ثابت UTrie2_سرخی *)ڈیٹا;
    اگر(سرخی->signature!=UTRIE2_SIG) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس خالی_اشاریہ;
    }

    /* get the options */
    اگر(قدر_بٹ!=(UTrie2_قدر_بٹ)(سرخی->options&UTRIE2_OPTIONS_قدر_بٹ_نقاب)) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس خالی_اشاریہ;
    }

    /* get the length values and offsets */
    uprv_memset(&عارضی_Trie, 0, حجم(عارضی_Trie));
    عارضی_Trie.اشاریہ_لمبائی=سرخی->اشاریہ_لمبائی;
    عارضی_Trie.ڈیٹا_لمبائی=سرخی->shifted_ڈیٹا_لمبائی<<UTRIE2_اشاریہ_سرکاؤ;
    عارضی_Trie.index2_Null_انحراف=سرخی->index2_Null_انحراف;
    عارضی_Trie.ڈیٹا_Null_انحراف=سرخی->ڈیٹا_Null_انحراف;

    عارضی_Trie.بلند_آغاز=سرخی->shifted_بلند_آغاز<<UTRIE2_سرکاؤ_1;
    عارضی_Trie.بلند_قدر_اشاریہ=عارضی_Trie.ڈیٹا_لمبائی-UTRIE2_ڈیٹا_GRANULARITY;
    اگر(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        عارضی_Trie.بلند_قدر_اشاریہ+=عارضی_Trie.اشاریہ_لمبائی;
    }

    /* calculate the actual length */
    actual_لمبائی=(int32_t)حجم(UTrie2_سرخی)+عارضی_Trie.اشاریہ_لمبائی*2;
    اگر(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        actual_لمبائی+=عارضی_Trie.ڈیٹا_لمبائی*2;
    } ورنہ {
        actual_لمبائی+=عارضی_Trie.ڈیٹا_لمبائی*4;
    }
    اگر(length<actual_لمبائی) {
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        واپس خالی_اشاریہ;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    اگر(trie==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memcpy(trie, &عارضی_Trie, حجم(عارضی_Trie));
    trie->حافظہ=(uint32_t *)ڈیٹا;
    trie->length=actual_لمبائی;
    trie->is_حافظہ_Owned=جھوٹ;
#اگر_تعریف_شدہ UTRIE2_DEBUG
    trie->name="fromSerialized";
#اگر_ختم

    /* set the pointers to its index and data arrays */
    p16=(ثابت uint16_t *)(سرخی+1);
    trie->اشاریہ=p16;
    p16+=trie->اشاریہ_لمبائی;

    /* get the data */
    انتخاب(قدر_بٹ) {
    حالت UTRIE2_16_قدر_بٹ:
        trie->data16=p16;
        trie->data32=خالی_اشاریہ;
        trie->initial_قدر=trie->اشاریہ[trie->ڈیٹا_Null_انحراف];
        trie->error_قدر=trie->data16[UTRIE2_BAD_UTF8_ڈیٹا_انحراف];
        توڑو;
    حالت UTRIE2_32_قدر_بٹ:
        trie->data16=خالی_اشاریہ;
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_قدر=trie->data32[trie->ڈیٹا_Null_انحراف];
        trie->error_قدر=trie->data32[UTRIE2_BAD_UTF8_ڈیٹا_انحراف];
        توڑو;
    طےشدہ:
        *p_Error_رمز=U_INVALID_FORMAT_ERROR;
        واپس خالی_اشاریہ;
    }

    اگر(p_Actual_لمبائی!=خالی_اشاریہ) {
        *p_Actual_لمبائی=actual_لمبائی;
    }
    واپس trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_قدر_بٹ قدر_بٹ,
                 uint32_t initial_قدر, uint32_t error_قدر,
                 UError_رمز *p_Error_رمز) {
    UTrie2 *trie;
    UTrie2_سرخی *سرخی;
    uint32_t *p;
    uint16_t *dest16;
    int32_t اشاریہ_لمبائی, ڈیٹا_لمبائی, length, i;
    int32_t ڈیٹا_Move;  /* >0 if the data is moved to the end of the index array */

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }

    اگر(قدر_بٹ<0 || UTRIE2_گنتی_قدر_بٹ<=قدر_بٹ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    /* calculate the total length of the dummy trie data */
    اشاریہ_لمبائی=UTRIE2_اشاریہ_1_انحراف;
    ڈیٹا_لمبائی=UTRIE2_ڈیٹا_آغاز_انحراف+UTRIE2_ڈیٹا_GRANULARITY;
    length=(int32_t)حجم(UTrie2_سرخی)+اشاریہ_لمبائی*2;
    اگر(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        length+=ڈیٹا_لمبائی*2;
    } ورنہ {
        length+=ڈیٹا_لمبائی*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    اگر(trie==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    uprv_memset(trie, 0, حجم(UTrie2));
    trie->حافظہ=uprv_malloc(length);
    اگر(trie->حافظہ==خالی_اشاریہ) {
        uprv_free(trie);
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    trie->length=length;
    trie->is_حافظہ_Owned=سچ;

    /* set the UTrie2 fields */
    اگر(قدر_بٹ==UTRIE2_16_قدر_بٹ) {
        ڈیٹا_Move=اشاریہ_لمبائی;
    } ورنہ {
        ڈیٹا_Move=0;
    }

    trie->اشاریہ_لمبائی=اشاریہ_لمبائی;
    trie->ڈیٹا_لمبائی=ڈیٹا_لمبائی;
    trie->index2_Null_انحراف=UTRIE2_اشاریہ_2_انحراف;
    trie->ڈیٹا_Null_انحراف=(uint16_t)ڈیٹا_Move;
    trie->initial_قدر=initial_قدر;
    trie->error_قدر=error_قدر;
    trie->بلند_آغاز=0;
    trie->بلند_قدر_اشاریہ=ڈیٹا_Move+UTRIE2_ڈیٹا_آغاز_انحراف;
#اگر_تعریف_شدہ UTRIE2_DEBUG
    trie->name="dummy";
#اگر_ختم

    /* set the header fields */
    سرخی=(UTrie2_سرخی *)trie->حافظہ;

    سرخی->signature=UTRIE2_SIG; /* "Tri2" */
    سرخی->options=(uint16_t)قدر_بٹ;

    سرخی->اشاریہ_لمبائی=(uint16_t)اشاریہ_لمبائی;
    سرخی->shifted_ڈیٹا_لمبائی=(uint16_t)(ڈیٹا_لمبائی>>UTRIE2_اشاریہ_سرکاؤ);
    سرخی->index2_Null_انحراف=(uint16_t)UTRIE2_اشاریہ_2_انحراف;
    سرخی->ڈیٹا_Null_انحراف=(uint16_t)ڈیٹا_Move;
    سرخی->shifted_بلند_آغاز=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرخی+1);
    trie->اشاریہ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    برائے(i=0; i<UTRIE2_اشاریہ_2_BMP_لمبائی; ++i) {
        *dest16++=(uint16_t)(ڈیٹا_Move>>UTRIE2_اشاریہ_سرکاؤ);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    برائے(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(ڈیٹا_Move+UTRIE2_BAD_UTF8_ڈیٹا_انحراف);
    }
    برائے(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)ڈیٹا_Move;
    }

    /* write the 16/32-bit data array */
    انتخاب(قدر_بٹ) {
    حالت UTRIE2_16_قدر_بٹ:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=خالی_اشاریہ;
        برائے(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_قدر;
        }
        برائے(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_قدر;
        }
        /* highValue and reserved values */
        برائے(i=0; i<UTRIE2_ڈیٹا_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_قدر;
        }
        توڑو;
    حالت UTRIE2_32_قدر_بٹ:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=خالی_اشاریہ;
        trie->data32=p;
        برائے(i=0; i<0x80; ++i) {
            *p++=initial_قدر;
        }
        برائے(; i<0xc0; ++i) {
            *p++=error_قدر;
        }
        /* highValue and reserved values */
        برائے(i=0; i<UTRIE2_ڈیٹا_GRANULARITY; ++i) {
            *p++=initial_قدر;
        }
        توڑو;
    طےشدہ:
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

    واپس trie;
}

U_CAPI خالی U_EXPORT2
utrie2_close(UTrie2 *trie) {
    اگر(trie!=خالی_اشاریہ) {
        اگر(trie->is_حافظہ_Owned) {
            uprv_free(trie->حافظہ);
        }
        اگر(trie->newTrie!=خالی_اشاریہ) {
            uprv_free(trie->newTrie->ڈیٹا);
#اگر_تعریف_شدہ UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#اگر_ختم
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ثابت UTrie2 *trie) {
    واپس trie->newTrie==خالی_اشاریہ;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ثابت UTrie2 *trie,
                 خالی *ڈیٹا, int32_t capacity,
                 UError_رمز *p_Error_رمز) {
    /* argument check */
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    اگر( trie==خالی_اشاریہ || trie->حافظہ==خالی_اشاریہ || trie->newTrie!=خالی_اشاریہ ||
        capacity<0 || (capacity>0 && (ڈیٹا==خالی_اشاریہ || (U_اشارہ_گر_نقاب_LSB(ڈیٹا, 3)!=0)))
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر(capacity>=trie->length) {
        uprv_memcpy(ڈیٹا, trie->حافظہ, trie->length);
    } ورنہ {
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
    }
    واپس trie->length;
}

/* enumeration -------------------------------------------------------------- */

#تعریف_کرو MIN_قدر(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ساکن uint32_t U_CALLCONV
enum_Same_قدر(ثابت خالی * /*context*/, uint32_t value) {
    واپس value;
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
ساکن خالی
enumEitherTrie(ثابت UTrie2 *trie,
               UChar32 آغاز, UChar32 حد_بندی,
               UTrie2_Enum_قدر *enum_قدر, UTrie2EnumRange *enumRange, ثابت خالی *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, پچھلا_قدر, initial_قدر;
    UChar32 c, پچھلا, بلند_آغاز;
    int32_t j, i2_بلاک, پچھلا_I2_بلاک, index2_Null_انحراف, بلاک, پچھلا_بلاک, null_بلاک;

    اگر(enumRange==خالی_اشاریہ) {
        واپس;
    }
    اگر(enum_قدر==خالی_اشاریہ) {
        enum_قدر=enum_Same_قدر;
    }

    اگر(trie->newTrie==خالی_اشاریہ) {
        /* frozen trie */
        idx=trie->اشاریہ;
        U_ASSERT(idx!=خالی_اشاریہ); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2_Null_انحراف=trie->index2_Null_انحراف;
        null_بلاک=trie->ڈیٹا_Null_انحراف;
    } ورنہ {
        /* unfrozen, mutable trie */
        idx=خالی_اشاریہ;
        data32=trie->newTrie->ڈیٹا;
        U_ASSERT(data32!=خالی_اشاریہ); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2_Null_انحراف=trie->newTrie->index2_Null_انحراف;
        null_بلاک=trie->newTrie->ڈیٹا_Null_انحراف;
    }

    بلند_آغاز=trie->بلند_آغاز;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_قدر=enum_قدر(context, trie->initial_قدر);

    /* set variables for previous range */
    پچھلا_I2_بلاک=-1;
    پچھلا_بلاک=-1;
    پچھلا=آغاز;
    پچھلا_قدر=0;

    /* enumerate index-2 blocks */
    برائے(c=آغاز; c<حد_بندی && c<بلند_آغاز;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 عارضی_حد_بندی=c+UTRIE2_CP_فی_اشاریہ_1_اندراج;
        اگر(حد_بندی<عارضی_حد_بندی) {
            عارضی_حد_بندی=حد_بندی;
        }
        اگر(c<=0xffff) {
            اگر(!U_IS_SURROGATE(c)) {
                i2_بلاک=c>>UTRIE2_سرکاؤ_2;
            } ورنہ اگر(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2_بلاک=UTRIE2_LSCP_اشاریہ_2_انحراف;
                عارضی_حد_بندی=MIN_قدر(0xdc00, حد_بندی);
            } ورنہ {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2_بلاک=0xd800>>UTRIE2_سرکاؤ_2;
                عارضی_حد_بندی=MIN_قدر(0xe000, حد_بندی);
            }
        } ورنہ {
            /* supplementary code points */
            اگر(idx!=خالی_اشاریہ) {
                i2_بلاک=idx[(UTRIE2_اشاریہ_1_انحراف-UTRIE2_OMITTED_BMP_اشاریہ_1_لمبائی)+
                              (c>>UTRIE2_سرکاؤ_1)];
            } ورنہ {
                i2_بلاک=trie->newTrie->index1[c>>UTRIE2_سرکاؤ_1];
            }
            اگر(i2_بلاک==پچھلا_I2_بلاک && (c-پچھلا)>=UTRIE2_CP_فی_اشاریہ_1_اندراج) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_فی_اشاریہ_1_اندراج;
                جاری;
            }
        }
        پچھلا_I2_بلاک=i2_بلاک;
        اگر(i2_بلاک==index2_Null_انحراف) {
            /* this is the null index-2 block */
            اگر(پچھلا_قدر!=initial_قدر) {
                اگر(پچھلا<c && !enumRange(context, پچھلا, c-1, پچھلا_قدر)) {
                    واپس;
                }
                پچھلا_بلاک=null_بلاک;
                پچھلا=c;
                پچھلا_قدر=initial_قدر;
            }
            c+=UTRIE2_CP_فی_اشاریہ_1_اندراج;
        } ورنہ {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2_حد_بندی;
            i2=(c>>UTRIE2_سرکاؤ_2)&UTRIE2_اشاریہ_2_نقاب;
            اگر((c>>UTRIE2_سرکاؤ_1)==(عارضی_حد_بندی>>UTRIE2_سرکاؤ_1)) {
                i2_حد_بندی=(عارضی_حد_بندی>>UTRIE2_سرکاؤ_2)&UTRIE2_اشاریہ_2_نقاب;
            } ورنہ {
                i2_حد_بندی=UTRIE2_اشاریہ_2_بلاک_لمبائی;
            }
            برائے(; i2<i2_حد_بندی; ++i2) {
                اگر(idx!=خالی_اشاریہ) {
                    بلاک = static_cast<int32_t>(idx[i2_بلاک + i2]) << UTRIE2_اشاریہ_سرکاؤ;
                } ورنہ {
                    بلاک=trie->newTrie->index2[i2_بلاک+i2];
                }
                اگر(بلاک==پچھلا_بلاک && (c-پچھلا)>=UTRIE2_ڈیٹا_بلاک_لمبائی) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_ڈیٹا_بلاک_لمبائی;
                    جاری;
                }
                پچھلا_بلاک=بلاک;
                اگر(بلاک==null_بلاک) {
                    /* this is the null data block */
                    اگر(پچھلا_قدر!=initial_قدر) {
                        اگر(پچھلا<c && !enumRange(context, پچھلا, c-1, پچھلا_قدر)) {
                            واپس;
                        }
                        پچھلا=c;
                        پچھلا_قدر=initial_قدر;
                    }
                    c+=UTRIE2_ڈیٹا_بلاک_لمبائی;
                } ورنہ {
                    برائے(j=0; j<UTRIE2_ڈیٹا_بلاک_لمبائی; ++j) {
                        value=enum_قدر(context, data32!=خالی_اشاریہ ? data32[بلاک+j] : idx[بلاک+j]);
                        اگر(value!=پچھلا_قدر) {
                            اگر(پچھلا<c && !enumRange(context, پچھلا, c-1, پچھلا_قدر)) {
                                واپس;
                            }
                            پچھلا=c;
                            پچھلا_قدر=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    اگر(c>حد_بندی) {
        c=حد_بندی;  /* could be higher if in the index2NullOffset */
    } ورنہ اگر(c<حد_بندی) {
        /* c==highStart<limit */
        uint32_t بلند_قدر;
        اگر(idx!=خالی_اشاریہ) {
            بلند_قدر=
                data32!=خالی_اشاریہ ?
                    data32[trie->بلند_قدر_اشاریہ] :
                    idx[trie->بلند_قدر_اشاریہ];
        } ورنہ {
            بلند_قدر=trie->newTrie->ڈیٹا[trie->newTrie->ڈیٹا_لمبائی-UTRIE2_ڈیٹا_GRANULARITY];
        }
        value=enum_قدر(context, بلند_قدر);
        اگر(value!=پچھلا_قدر) {
            اگر(پچھلا<c && !enumRange(context, پچھلا, c-1, پچھلا_قدر)) {
                واپس;
            }
            پچھلا=c;
            پچھلا_قدر=value;
        }
        c=حد_بندی;
    }

    /* deliver last range */
    enumRange(context, پچھلا, c-1, پچھلا_قدر);
}

U_CAPI خالی U_EXPORT2
utrie2_enum(ثابت UTrie2 *trie,
            UTrie2_Enum_قدر *enum_قدر, UTrie2EnumRange *enumRange, ثابت خالی *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_قدر, enumRange, context);
}

U_CAPI خالی U_EXPORT2
utrie2_enumForLeadSurrogate(ثابت UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_قدر *enum_قدر, UTrie2EnumRange *enumRange,
                            ثابت خالی *context) {
    اگر(!U16_IS_LEAD(lead)) {
        واپس;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_قدر, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    رمز_نقطہ_حد_بندی=رمز_نقطہ_آغاز;
    اگر(آغاز>=رمز_نقطہ_آغاز) {
        رمز_نقطہ=U_SENTINEL;
        واپس static_cast<uint16_t>(trie->error_قدر);
    }
    uint16_t نتیجہ;
    UTRIE2_U16_PREV16(trie, آغاز, رمز_نقطہ_آغاز, رمز_نقطہ, نتیجہ);
    واپس نتیجہ;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    رمز_نقطہ_آغاز=رمز_نقطہ_حد_بندی;
    اگر(رمز_نقطہ_حد_بندی==حد_بندی) {
        رمز_نقطہ=U_SENTINEL;
        واپس static_cast<uint16_t>(trie->error_قدر);
    }
    uint16_t نتیجہ;
    UTRIE2_U16_NEXT16(trie, رمز_نقطہ_حد_بندی, حد_بندی, رمز_نقطہ, نتیجہ);
    واپس نتیجہ;
}

U_NAMESPACE_انتہا

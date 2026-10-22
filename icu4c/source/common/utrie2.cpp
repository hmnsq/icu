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
#اگر_تعریف_شده UCPTRIE_DEBUG
#ضمن "unicode/umutablecptrie.h"
#پایان_اگر
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
    int32_t i2, بلوک;

    إذا(c>=trie->بالا_شروع && (!U_IS_LEAD(c) || fromLSCP)) {
        أرجع trie->داده[trie->داده_درازی-UTRIE2_داده_GRANULARITY];
    }

    إذا(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_شاخص_2_جابه_جایی-(0xd800>>UTRIE2_لغزش_2))+
            (c>>UTRIE2_لغزش_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_لغزش_1]+
            ((c>>UTRIE2_لغزش_2)&UTRIE2_شاخص_2_ماسک);
    }
    بلوک=trie->index2[i2];
    أرجع trie->داده[بلوک+(c&UTRIE2_داده_ماسک)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ثابت UTrie2 *trie, UChar32 c) {
    إذا(trie->data16!=اشاره_تهی) {
        أرجع UTRIE2_GET16(trie, c);
    } وإلا إذا(trie->data32!=اشاره_تهی) {
        أرجع UTRIE2_GET32(trie, c);
    } وإلا إذا((uint32_t)c>0x10ffff) {
        أرجع trie->error_مقدار;
    } وإلا {
        أرجع get32(trie->newTrie, c, درست);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_کد_Unit(ثابت UTrie2 *trie, UChar32 c) {
    إذا(!U_IS_LEAD(c)) {
        أرجع trie->error_مقدار;
    }
    إذا(trie->data16!=اشاره_تهی) {
        أرجع UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } وإلا إذا(trie->data32!=اشاره_تهی) {
        أرجع UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } وإلا {
        أرجع get32(trie->newTrie, c, خطأ);
    }
}

ساكن مضمن int32_t
u8_شاخص(ثابت UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_شاخص_FROM_CP(
            trie,
            trie->data32==اشاره_تهی ? trie->شاخص_درازی : 0,
            c);
    أرجع (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_داخلی_U8_بعدی_شاخص(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *src, ثابت uint8_t *حد) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    إذا((حد-src)<=7) {
        length=(int32_t)(حد-src);
    } وإلا {
        length=7;
    }
    c=utf8_بعدی_Char_امن_Body(src, &i, length, c, -1);
    أرجع u8_شاخص(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_داخلی_U8_Prev_شاخص(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *شروع, ثابت uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    إذا((src-شروع)<=7) {
        i=length=(int32_t)(src-شروع);
    } وإلا {
        i=length=7;
        شروع=src-7;
    }
    c=utf8_prev_Char_امن_Body(شروع, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    أرجع u8_شاخص(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_مقدار_بیتها مقدار_بیتها,
                          ثابت فراغ *داده, int32_t length, int32_t *p_Actual_درازی,
                          UError_کد *p_Error_کد) {
    ثابت UTrie2_سرآیند *سرآیند;
    ثابت uint16_t *p16;
    int32_t actual_درازی;

    UTrie2 موقت_Trie;
    UTrie2 *trie;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }

    إذا( length<=0 || (U_اشاره_گر_ماسک_LSB(داده, 3)!=0) ||
        مقدار_بیتها<0 || UTRIE2_شمار_مقدار_بیتها<=مقدار_بیتها
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    /* enough data for a trie header? */
    إذا(length<(int32_t)حجم(UTrie2_سرآیند)) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع اشاره_تهی;
    }

    /* check the signature */
    سرآیند=(ثابت UTrie2_سرآیند *)داده;
    إذا(سرآیند->signature!=UTRIE2_SIG) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع اشاره_تهی;
    }

    /* get the options */
    إذا(مقدار_بیتها!=(UTrie2_مقدار_بیتها)(سرآیند->options&UTRIE2_OPTIONS_مقدار_بیتها_ماسک)) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع اشاره_تهی;
    }

    /* get the length values and offsets */
    uprv_memset(&موقت_Trie, 0, حجم(موقت_Trie));
    موقت_Trie.شاخص_درازی=سرآیند->شاخص_درازی;
    موقت_Trie.داده_درازی=سرآیند->shifted_داده_درازی<<UTRIE2_شاخص_لغزش;
    موقت_Trie.index2_Null_جابه_جایی=سرآیند->index2_Null_جابه_جایی;
    موقت_Trie.داده_Null_جابه_جایی=سرآیند->داده_Null_جابه_جایی;

    موقت_Trie.بالا_شروع=سرآیند->shifted_بالا_شروع<<UTRIE2_لغزش_1;
    موقت_Trie.بالا_مقدار_شاخص=موقت_Trie.داده_درازی-UTRIE2_داده_GRANULARITY;
    إذا(مقدار_بیتها==UTRIE2_16_مقدار_بیتها) {
        موقت_Trie.بالا_مقدار_شاخص+=موقت_Trie.شاخص_درازی;
    }

    /* calculate the actual length */
    actual_درازی=(int32_t)حجم(UTrie2_سرآیند)+موقت_Trie.شاخص_درازی*2;
    إذا(مقدار_بیتها==UTRIE2_16_مقدار_بیتها) {
        actual_درازی+=موقت_Trie.داده_درازی*2;
    } وإلا {
        actual_درازی+=موقت_Trie.داده_درازی*4;
    }
    إذا(length<actual_درازی) {
        *p_Error_کد=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        أرجع اشاره_تهی;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memcpy(trie, &موقت_Trie, حجم(موقت_Trie));
    trie->حافظه=(uint32_t *)داده;
    trie->length=actual_درازی;
    trie->is_حافظه_Owned=خطأ;
#اگر_تعریف_شده UTRIE2_DEBUG
    trie->name="fromSerialized";
#پایان_اگر

    /* set the pointers to its index and data arrays */
    p16=(ثابت uint16_t *)(سرآیند+1);
    trie->شاخص=p16;
    p16+=trie->شاخص_درازی;

    /* get the data */
    بدل(مقدار_بیتها) {
    حالة UTRIE2_16_مقدار_بیتها:
        trie->data16=p16;
        trie->data32=اشاره_تهی;
        trie->initial_مقدار=trie->شاخص[trie->داده_Null_جابه_جایی];
        trie->error_مقدار=trie->data16[UTRIE2_BAD_UTF8_داده_جابه_جایی];
        اكسر;
    حالة UTRIE2_32_مقدار_بیتها:
        trie->data16=اشاره_تهی;
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_مقدار=trie->data32[trie->داده_Null_جابه_جایی];
        trie->error_مقدار=trie->data32[UTRIE2_BAD_UTF8_داده_جابه_جایی];
        اكسر;
    افتراضي:
        *p_Error_کد=U_INVALID_FORMAT_ERROR;
        أرجع اشاره_تهی;
    }

    إذا(p_Actual_درازی!=اشاره_تهی) {
        *p_Actual_درازی=actual_درازی;
    }
    أرجع trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_مقدار_بیتها مقدار_بیتها,
                 uint32_t initial_مقدار, uint32_t error_مقدار,
                 UError_کد *p_Error_کد) {
    UTrie2 *trie;
    UTrie2_سرآیند *سرآیند;
    uint32_t *p;
    uint16_t *dest16;
    int32_t شاخص_درازی, داده_درازی, length, i;
    int32_t داده_Move;  /* >0 if the data is moved to the end of the index array */

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }

    إذا(مقدار_بیتها<0 || UTRIE2_شمار_مقدار_بیتها<=مقدار_بیتها) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    /* calculate the total length of the dummy trie data */
    شاخص_درازی=UTRIE2_شاخص_1_جابه_جایی;
    داده_درازی=UTRIE2_داده_شروع_جابه_جایی+UTRIE2_داده_GRANULARITY;
    length=(int32_t)حجم(UTrie2_سرآیند)+شاخص_درازی*2;
    إذا(مقدار_بیتها==UTRIE2_16_مقدار_بیتها) {
        length+=داده_درازی*2;
    } وإلا {
        length+=داده_درازی*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==اشاره_تهی) {
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    uprv_memset(trie, 0, حجم(UTrie2));
    trie->حافظه=uprv_malloc(length);
    إذا(trie->حافظه==اشاره_تهی) {
        uprv_free(trie);
        *p_Error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    trie->length=length;
    trie->is_حافظه_Owned=درست;

    /* set the UTrie2 fields */
    إذا(مقدار_بیتها==UTRIE2_16_مقدار_بیتها) {
        داده_Move=شاخص_درازی;
    } وإلا {
        داده_Move=0;
    }

    trie->شاخص_درازی=شاخص_درازی;
    trie->داده_درازی=داده_درازی;
    trie->index2_Null_جابه_جایی=UTRIE2_شاخص_2_جابه_جایی;
    trie->داده_Null_جابه_جایی=(uint16_t)داده_Move;
    trie->initial_مقدار=initial_مقدار;
    trie->error_مقدار=error_مقدار;
    trie->بالا_شروع=0;
    trie->بالا_مقدار_شاخص=داده_Move+UTRIE2_داده_شروع_جابه_جایی;
#اگر_تعریف_شده UTRIE2_DEBUG
    trie->name="dummy";
#پایان_اگر

    /* set the header fields */
    سرآیند=(UTrie2_سرآیند *)trie->حافظه;

    سرآیند->signature=UTRIE2_SIG; /* "Tri2" */
    سرآیند->options=(uint16_t)مقدار_بیتها;

    سرآیند->شاخص_درازی=(uint16_t)شاخص_درازی;
    سرآیند->shifted_داده_درازی=(uint16_t)(داده_درازی>>UTRIE2_شاخص_لغزش);
    سرآیند->index2_Null_جابه_جایی=(uint16_t)UTRIE2_شاخص_2_جابه_جایی;
    سرآیند->داده_Null_جابه_جایی=(uint16_t)داده_Move;
    سرآیند->shifted_بالا_شروع=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرآیند+1);
    trie->شاخص=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    لكل(i=0; i<UTRIE2_شاخص_2_BMP_درازی; ++i) {
        *dest16++=(uint16_t)(داده_Move>>UTRIE2_شاخص_لغزش);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(داده_Move+UTRIE2_BAD_UTF8_داده_جابه_جایی);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)داده_Move;
    }

    /* write the 16/32-bit data array */
    بدل(مقدار_بیتها) {
    حالة UTRIE2_16_مقدار_بیتها:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=اشاره_تهی;
        لكل(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_مقدار;
        }
        لكل(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_مقدار;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_داده_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_مقدار;
        }
        اكسر;
    حالة UTRIE2_32_مقدار_بیتها:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=اشاره_تهی;
        trie->data32=p;
        لكل(i=0; i<0x80; ++i) {
            *p++=initial_مقدار;
        }
        لكل(; i<0xc0; ++i) {
            *p++=error_مقدار;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_داده_GRANULARITY; ++i) {
            *p++=initial_مقدار;
        }
        اكسر;
    افتراضي:
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
utrie2_close(UTrie2 *trie) {
    إذا(trie!=اشاره_تهی) {
        إذا(trie->is_حافظه_Owned) {
            uprv_free(trie->حافظه);
        }
        إذا(trie->newTrie!=اشاره_تهی) {
            uprv_free(trie->newTrie->داده);
#اگر_تعریف_شده UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#پایان_اگر
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ثابت UTrie2 *trie) {
    أرجع trie->newTrie==اشاره_تهی;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ثابت UTrie2 *trie,
                 فراغ *داده, int32_t capacity,
                 UError_کد *p_Error_کد) {
    /* argument check */
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    إذا( trie==اشاره_تهی || trie->حافظه==اشاره_تهی || trie->newTrie!=اشاره_تهی ||
        capacity<0 || (capacity>0 && (داده==اشاره_تهی || (U_اشاره_گر_ماسک_LSB(داده, 3)!=0)))
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    إذا(capacity>=trie->length) {
        uprv_memcpy(داده, trie->حافظه, trie->length);
    } وإلا {
        *p_Error_کد=U_میانگیر_سرریز_ERROR;
    }
    أرجع trie->length;
}

/* enumeration -------------------------------------------------------------- */

#تعریف_کن MIN_مقدار(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ساكن uint32_t U_CALLCONV
enum_Same_مقدار(ثابت فراغ * /*context*/, uint32_t value) {
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
               UChar32 شروع, UChar32 حد,
               UTrie2_Enum_مقدار *enum_مقدار, UTrie2EnumRange *enumRange, ثابت فراغ *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prev_مقدار, initial_مقدار;
    UChar32 c, prev, بالا_شروع;
    int32_t j, i2_بلوک, prev_I2_بلوک, index2_Null_جابه_جایی, بلوک, prev_بلوک, null_بلوک;

    إذا(enumRange==اشاره_تهی) {
        أرجع;
    }
    إذا(enum_مقدار==اشاره_تهی) {
        enum_مقدار=enum_Same_مقدار;
    }

    إذا(trie->newTrie==اشاره_تهی) {
        /* frozen trie */
        idx=trie->شاخص;
        U_ASSERT(idx!=اشاره_تهی); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2_Null_جابه_جایی=trie->index2_Null_جابه_جایی;
        null_بلوک=trie->داده_Null_جابه_جایی;
    } وإلا {
        /* unfrozen, mutable trie */
        idx=اشاره_تهی;
        data32=trie->newTrie->داده;
        U_ASSERT(data32!=اشاره_تهی); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2_Null_جابه_جایی=trie->newTrie->index2_Null_جابه_جایی;
        null_بلوک=trie->newTrie->داده_Null_جابه_جایی;
    }

    بالا_شروع=trie->بالا_شروع;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_مقدار=enum_مقدار(context, trie->initial_مقدار);

    /* set variables for previous range */
    prev_I2_بلوک=-1;
    prev_بلوک=-1;
    prev=شروع;
    prev_مقدار=0;

    /* enumerate index-2 blocks */
    لكل(c=شروع; c<حد && c<بالا_شروع;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 موقت_حد=c+UTRIE2_CP_در_هر_شاخص_1_مدخل;
        إذا(حد<موقت_حد) {
            موقت_حد=حد;
        }
        إذا(c<=0xffff) {
            إذا(!U_IS_SURROGATE(c)) {
                i2_بلوک=c>>UTRIE2_لغزش_2;
            } وإلا إذا(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2_بلوک=UTRIE2_LSCP_شاخص_2_جابه_جایی;
                موقت_حد=MIN_مقدار(0xdc00, حد);
            } وإلا {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2_بلوک=0xd800>>UTRIE2_لغزش_2;
                موقت_حد=MIN_مقدار(0xe000, حد);
            }
        } وإلا {
            /* supplementary code points */
            إذا(idx!=اشاره_تهی) {
                i2_بلوک=idx[(UTRIE2_شاخص_1_جابه_جایی-UTRIE2_OMITTED_BMP_شاخص_1_درازی)+
                              (c>>UTRIE2_لغزش_1)];
            } وإلا {
                i2_بلوک=trie->newTrie->index1[c>>UTRIE2_لغزش_1];
            }
            إذا(i2_بلوک==prev_I2_بلوک && (c-prev)>=UTRIE2_CP_در_هر_شاخص_1_مدخل) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_در_هر_شاخص_1_مدخل;
                استمر;
            }
        }
        prev_I2_بلوک=i2_بلوک;
        إذا(i2_بلوک==index2_Null_جابه_جایی) {
            /* this is the null index-2 block */
            إذا(prev_مقدار!=initial_مقدار) {
                إذا(prev<c && !enumRange(context, prev, c-1, prev_مقدار)) {
                    أرجع;
                }
                prev_بلوک=null_بلوک;
                prev=c;
                prev_مقدار=initial_مقدار;
            }
            c+=UTRIE2_CP_در_هر_شاخص_1_مدخل;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2_حد;
            i2=(c>>UTRIE2_لغزش_2)&UTRIE2_شاخص_2_ماسک;
            إذا((c>>UTRIE2_لغزش_1)==(موقت_حد>>UTRIE2_لغزش_1)) {
                i2_حد=(موقت_حد>>UTRIE2_لغزش_2)&UTRIE2_شاخص_2_ماسک;
            } وإلا {
                i2_حد=UTRIE2_شاخص_2_بلوک_درازی;
            }
            لكل(; i2<i2_حد; ++i2) {
                إذا(idx!=اشاره_تهی) {
                    بلوک = static_cast<int32_t>(idx[i2_بلوک + i2]) << UTRIE2_شاخص_لغزش;
                } وإلا {
                    بلوک=trie->newTrie->index2[i2_بلوک+i2];
                }
                إذا(بلوک==prev_بلوک && (c-prev)>=UTRIE2_داده_بلوک_درازی) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_داده_بلوک_درازی;
                    استمر;
                }
                prev_بلوک=بلوک;
                إذا(بلوک==null_بلوک) {
                    /* this is the null data block */
                    إذا(prev_مقدار!=initial_مقدار) {
                        إذا(prev<c && !enumRange(context, prev, c-1, prev_مقدار)) {
                            أرجع;
                        }
                        prev=c;
                        prev_مقدار=initial_مقدار;
                    }
                    c+=UTRIE2_داده_بلوک_درازی;
                } وإلا {
                    لكل(j=0; j<UTRIE2_داده_بلوک_درازی; ++j) {
                        value=enum_مقدار(context, data32!=اشاره_تهی ? data32[بلوک+j] : idx[بلوک+j]);
                        إذا(value!=prev_مقدار) {
                            إذا(prev<c && !enumRange(context, prev, c-1, prev_مقدار)) {
                                أرجع;
                            }
                            prev=c;
                            prev_مقدار=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    إذا(c>حد) {
        c=حد;  /* could be higher if in the index2NullOffset */
    } وإلا إذا(c<حد) {
        /* c==highStart<limit */
        uint32_t بالا_مقدار;
        إذا(idx!=اشاره_تهی) {
            بالا_مقدار=
                data32!=اشاره_تهی ?
                    data32[trie->بالا_مقدار_شاخص] :
                    idx[trie->بالا_مقدار_شاخص];
        } وإلا {
            بالا_مقدار=trie->newTrie->داده[trie->newTrie->داده_درازی-UTRIE2_داده_GRANULARITY];
        }
        value=enum_مقدار(context, بالا_مقدار);
        إذا(value!=prev_مقدار) {
            إذا(prev<c && !enumRange(context, prev, c-1, prev_مقدار)) {
                أرجع;
            }
            prev=c;
            prev_مقدار=value;
        }
        c=حد;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_مقدار);
}

U_CAPI فراغ U_EXPORT2
utrie2_enum(ثابت UTrie2 *trie,
            UTrie2_Enum_مقدار *enum_مقدار, UTrie2EnumRange *enumRange, ثابت فراغ *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_مقدار, enumRange, context);
}

U_CAPI فراغ U_EXPORT2
utrie2_enumForLeadSurrogate(ثابت UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_مقدار *enum_مقدار, UTrie2EnumRange *enumRange,
                            ثابت فراغ *context) {
    إذا(!U16_IS_LEAD(lead)) {
        أرجع;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_مقدار, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    کد_نقطه_حد=کد_نقطه_شروع;
    إذا(شروع>=کد_نقطه_شروع) {
        کد_نقطه=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->error_مقدار);
    }
    uint16_t نتیجه;
    UTRIE2_U16_PREV16(trie, شروع, کد_نقطه_شروع, کد_نقطه, نتیجه);
    أرجع نتیجه;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    کد_نقطه_شروع=کد_نقطه_حد;
    إذا(کد_نقطه_حد==حد) {
        کد_نقطه=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->error_مقدار);
    }
    uint16_t نتیجه;
    UTRIE2_U16_NEXT16(trie, کد_نقطه_حد, حد, کد_نقطه, نتیجه);
    أرجع نتیجه;
}

U_NAMESPACE_انتها

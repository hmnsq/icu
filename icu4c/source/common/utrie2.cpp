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
#که_تعریف_شوی UCPTRIE_DEBUG
#ضمن "unicode/umutablecptrie.h"
#د_که_پای
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

    إذا(c>=trie->لوړ_پیل && (!U_IS_LEAD(c) || fromLSCP)) {
        أرجع trie->معلومات[trie->معلومات_اوږدتیا-UTRIE2_معلومات_GRANULARITY];
    }

    إذا(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_شاخص_2_دننه_واټن-(0xd800>>UTRIE2_ښويدنه_2))+
            (c>>UTRIE2_ښويدنه_2);
    } وإلا {
        i2=trie->index1[c>>UTRIE2_ښويدنه_1]+
            ((c>>UTRIE2_ښويدنه_2)&UTRIE2_شاخص_2_ماسک);
    }
    بلاک=trie->index2[i2];
    أرجع trie->معلومات[بلاک+(c&UTRIE2_معلومات_ماسک)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ثابت UTrie2 *trie, UChar32 c) {
    إذا(trie->data16!=تش_اشاره) {
        أرجع UTRIE2_GET16(trie, c);
    } وإلا إذا(trie->data32!=تش_اشاره) {
        أرجع UTRIE2_GET32(trie, c);
    } وإلا إذا((uint32_t)c>0x10ffff) {
        أرجع trie->error_ارزښت;
    } وإلا {
        أرجع get32(trie->newTrie, c, ریښتیا);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_کوډ_Unit(ثابت UTrie2 *trie, UChar32 c) {
    إذا(!U_IS_LEAD(c)) {
        أرجع trie->error_ارزښت;
    }
    إذا(trie->data16!=تش_اشاره) {
        أرجع UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } وإلا إذا(trie->data32!=تش_اشاره) {
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
            trie->data32==تش_اشاره ? trie->شاخص_اوږدتیا : 0,
            c);
    أرجع (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_دننی_U8_بل_شاخص(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *src, ثابت uint8_t *حد) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    إذا((حد-src)<=7) {
        length=(int32_t)(حد-src);
    } وإلا {
        length=7;
    }
    c=utf8_بل_Char_خوندي_Body(src, &i, length, c, -1);
    أرجع u8_شاخص(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_دننی_U8_Prev_شاخص(ثابت UTrie2 *trie, UChar32 c,
                           ثابت uint8_t *پیل, ثابت uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    إذا((src-پیل)<=7) {
        i=length=(int32_t)(src-پیل);
    } وإلا {
        i=length=7;
        پیل=src-7;
    }
    c=utf8_prev_Char_خوندي_Body(پیل, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    أرجع u8_شاخص(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_ارزښت_بټونه ارزښت_بټونه,
                          ثابت فراغ *معلومات, int32_t length, int32_t *p_Actual_اوږدتیا,
                          UError_کوډ *p_Error_کوډ) {
    ثابت UTrie2_سرلیک *سرلیک;
    ثابت uint16_t *p16;
    int32_t actual_اوږدتیا;

    UTrie2 لنډمهاله_Trie;
    UTrie2 *trie;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }

    إذا( length<=0 || (U_اشاره_ګر_ماسک_LSB(معلومات, 3)!=0) ||
        ارزښت_بټونه<0 || UTRIE2_شمېر_ارزښت_بټونه<=ارزښت_بټونه
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    /* enough data for a trie header? */
    إذا(length<(int32_t)حجم(UTrie2_سرلیک)) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع تش_اشاره;
    }

    /* check the signature */
    سرلیک=(ثابت UTrie2_سرلیک *)معلومات;
    إذا(سرلیک->signature!=UTRIE2_SIG) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع تش_اشاره;
    }

    /* get the options */
    إذا(ارزښت_بټونه!=(UTrie2_ارزښت_بټونه)(سرلیک->options&UTRIE2_OPTIONS_ارزښت_بټونه_ماسک)) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع تش_اشاره;
    }

    /* get the length values and offsets */
    uprv_memset(&لنډمهاله_Trie, 0, حجم(لنډمهاله_Trie));
    لنډمهاله_Trie.شاخص_اوږدتیا=سرلیک->شاخص_اوږدتیا;
    لنډمهاله_Trie.معلومات_اوږدتیا=سرلیک->shifted_معلومات_اوږدتیا<<UTRIE2_شاخص_ښويدنه;
    لنډمهاله_Trie.index2_Null_دننه_واټن=سرلیک->index2_Null_دننه_واټن;
    لنډمهاله_Trie.معلومات_Null_دننه_واټن=سرلیک->معلومات_Null_دننه_واټن;

    لنډمهاله_Trie.لوړ_پیل=سرلیک->shifted_لوړ_پیل<<UTRIE2_ښويدنه_1;
    لنډمهاله_Trie.لوړ_ارزښت_شاخص=لنډمهاله_Trie.معلومات_اوږدتیا-UTRIE2_معلومات_GRANULARITY;
    إذا(ارزښت_بټونه==UTRIE2_16_ارزښت_بټونه) {
        لنډمهاله_Trie.لوړ_ارزښت_شاخص+=لنډمهاله_Trie.شاخص_اوږدتیا;
    }

    /* calculate the actual length */
    actual_اوږدتیا=(int32_t)حجم(UTrie2_سرلیک)+لنډمهاله_Trie.شاخص_اوږدتیا*2;
    إذا(ارزښت_بټونه==UTRIE2_16_ارزښت_بټونه) {
        actual_اوږدتیا+=لنډمهاله_Trie.معلومات_اوږدتیا*2;
    } وإلا {
        actual_اوږدتیا+=لنډمهاله_Trie.معلومات_اوږدتیا*4;
    }
    إذا(length<actual_اوږدتیا) {
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        أرجع تش_اشاره;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==تش_اشاره) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memcpy(trie, &لنډمهاله_Trie, حجم(لنډمهاله_Trie));
    trie->حافظه=(uint32_t *)معلومات;
    trie->length=actual_اوږدتیا;
    trie->is_حافظه_Owned=خطأ;
#که_تعریف_شوی UTRIE2_DEBUG
    trie->name="fromSerialized";
#د_که_پای

    /* set the pointers to its index and data arrays */
    p16=(ثابت uint16_t *)(سرلیک+1);
    trie->شاخص=p16;
    p16+=trie->شاخص_اوږدتیا;

    /* get the data */
    بدل(ارزښت_بټونه) {
    حالة UTRIE2_16_ارزښت_بټونه:
        trie->data16=p16;
        trie->data32=تش_اشاره;
        trie->initial_ارزښت=trie->شاخص[trie->معلومات_Null_دننه_واټن];
        trie->error_ارزښت=trie->data16[UTRIE2_BAD_UTF8_معلومات_دننه_واټن];
        اكسر;
    حالة UTRIE2_32_ارزښت_بټونه:
        trie->data16=تش_اشاره;
        trie->data32=(ثابت uint32_t *)p16;
        trie->initial_ارزښت=trie->data32[trie->معلومات_Null_دننه_واټن];
        trie->error_ارزښت=trie->data32[UTRIE2_BAD_UTF8_معلومات_دننه_واټن];
        اكسر;
    افتراضي:
        *p_Error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع تش_اشاره;
    }

    إذا(p_Actual_اوږدتیا!=تش_اشاره) {
        *p_Actual_اوږدتیا=actual_اوږدتیا;
    }
    أرجع trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_ارزښت_بټونه ارزښت_بټونه,
                 uint32_t initial_ارزښت, uint32_t error_ارزښت,
                 UError_کوډ *p_Error_کوډ) {
    UTrie2 *trie;
    UTrie2_سرلیک *سرلیک;
    uint32_t *p;
    uint16_t *dest16;
    int32_t شاخص_اوږدتیا, معلومات_اوږدتیا, length, i;
    int32_t معلومات_Move;  /* >0 if the data is moved to the end of the index array */

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }

    إذا(ارزښت_بټونه<0 || UTRIE2_شمېر_ارزښت_بټونه<=ارزښت_بټونه) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    /* calculate the total length of the dummy trie data */
    شاخص_اوږدتیا=UTRIE2_شاخص_1_دننه_واټن;
    معلومات_اوږدتیا=UTRIE2_معلومات_پیل_دننه_واټن+UTRIE2_معلومات_GRANULARITY;
    length=(int32_t)حجم(UTrie2_سرلیک)+شاخص_اوږدتیا*2;
    إذا(ارزښت_بټونه==UTRIE2_16_ارزښت_بټونه) {
        length+=معلومات_اوږدتیا*2;
    } وإلا {
        length+=معلومات_اوږدتیا*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(حجم(UTrie2));
    إذا(trie==تش_اشاره) {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    uprv_memset(trie, 0, حجم(UTrie2));
    trie->حافظه=uprv_malloc(length);
    إذا(trie->حافظه==تش_اشاره) {
        uprv_free(trie);
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    trie->length=length;
    trie->is_حافظه_Owned=ریښتیا;

    /* set the UTrie2 fields */
    إذا(ارزښت_بټونه==UTRIE2_16_ارزښت_بټونه) {
        معلومات_Move=شاخص_اوږدتیا;
    } وإلا {
        معلومات_Move=0;
    }

    trie->شاخص_اوږدتیا=شاخص_اوږدتیا;
    trie->معلومات_اوږدتیا=معلومات_اوږدتیا;
    trie->index2_Null_دننه_واټن=UTRIE2_شاخص_2_دننه_واټن;
    trie->معلومات_Null_دننه_واټن=(uint16_t)معلومات_Move;
    trie->initial_ارزښت=initial_ارزښت;
    trie->error_ارزښت=error_ارزښت;
    trie->لوړ_پیل=0;
    trie->لوړ_ارزښت_شاخص=معلومات_Move+UTRIE2_معلومات_پیل_دننه_واټن;
#که_تعریف_شوی UTRIE2_DEBUG
    trie->name="dummy";
#د_که_پای

    /* set the header fields */
    سرلیک=(UTrie2_سرلیک *)trie->حافظه;

    سرلیک->signature=UTRIE2_SIG; /* "Tri2" */
    سرلیک->options=(uint16_t)ارزښت_بټونه;

    سرلیک->شاخص_اوږدتیا=(uint16_t)شاخص_اوږدتیا;
    سرلیک->shifted_معلومات_اوږدتیا=(uint16_t)(معلومات_اوږدتیا>>UTRIE2_شاخص_ښويدنه);
    سرلیک->index2_Null_دننه_واټن=(uint16_t)UTRIE2_شاخص_2_دننه_واټن;
    سرلیک->معلومات_Null_دننه_واټن=(uint16_t)معلومات_Move;
    سرلیک->shifted_لوړ_پیل=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(سرلیک+1);
    trie->شاخص=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    لكل(i=0; i<UTRIE2_شاخص_2_BMP_اوږدتیا; ++i) {
        *dest16++=(uint16_t)(معلومات_Move>>UTRIE2_شاخص_ښويدنه);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    لكل(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(معلومات_Move+UTRIE2_BAD_UTF8_معلومات_دننه_واټن);
    }
    لكل(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)معلومات_Move;
    }

    /* write the 16/32-bit data array */
    بدل(ارزښت_بټونه) {
    حالة UTRIE2_16_ارزښت_بټونه:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=تش_اشاره;
        لكل(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_ارزښت;
        }
        لكل(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_ارزښت;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_معلومات_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_ارزښت;
        }
        اكسر;
    حالة UTRIE2_32_ارزښت_بټونه:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=تش_اشاره;
        trie->data32=p;
        لكل(i=0; i<0x80; ++i) {
            *p++=initial_ارزښت;
        }
        لكل(; i<0xc0; ++i) {
            *p++=error_ارزښت;
        }
        /* highValue and reserved values */
        لكل(i=0; i<UTRIE2_معلومات_GRANULARITY; ++i) {
            *p++=initial_ارزښت;
        }
        اكسر;
    افتراضي:
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

    أرجع trie;
}

U_CAPI فراغ U_EXPORT2
utrie2_close(UTrie2 *trie) {
    إذا(trie!=تش_اشاره) {
        إذا(trie->is_حافظه_Owned) {
            uprv_free(trie->حافظه);
        }
        إذا(trie->newTrie!=تش_اشاره) {
            uprv_free(trie->newTrie->معلومات);
#که_تعریف_شوی UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#د_که_پای
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ثابت UTrie2 *trie) {
    أرجع trie->newTrie==تش_اشاره;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ثابت UTrie2 *trie,
                 فراغ *معلومات, int32_t capacity,
                 UError_کوډ *p_Error_کوډ) {
    /* argument check */
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    إذا( trie==تش_اشاره || trie->حافظه==تش_اشاره || trie->newTrie!=تش_اشاره ||
        capacity<0 || (capacity>0 && (معلومات==تش_اشاره || (U_اشاره_ګر_ماسک_LSB(معلومات, 3)!=0)))
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(capacity>=trie->length) {
        uprv_memcpy(معلومات, trie->حافظه, trie->length);
    } وإلا {
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
    }
    أرجع trie->length;
}

/* enumeration -------------------------------------------------------------- */

#تعریف_کړه MIN_ارزښت(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ساكن uint32_t U_CALLCONV
enum_Same_ارزښت(ثابت فراغ * /*context*/, uint32_t value) {
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
               UChar32 پیل, UChar32 حد,
               UTrie2_Enum_ارزښت *enum_ارزښت, UTrie2EnumRange *enumRange, ثابت فراغ *context) {
    ثابت uint32_t *data32;
    ثابت uint16_t *idx;

    uint32_t value, prev_ارزښت, initial_ارزښت;
    UChar32 c, prev, لوړ_پیل;
    int32_t j, i2_بلاک, prev_I2_بلاک, index2_Null_دننه_واټن, بلاک, prev_بلاک, null_بلاک;

    إذا(enumRange==تش_اشاره) {
        أرجع;
    }
    إذا(enum_ارزښت==تش_اشاره) {
        enum_ارزښت=enum_Same_ارزښت;
    }

    إذا(trie->newTrie==تش_اشاره) {
        /* frozen trie */
        idx=trie->شاخص;
        U_ASSERT(idx!=تش_اشاره); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2_Null_دننه_واټن=trie->index2_Null_دننه_واټن;
        null_بلاک=trie->معلومات_Null_دننه_واټن;
    } وإلا {
        /* unfrozen, mutable trie */
        idx=تش_اشاره;
        data32=trie->newTrie->معلومات;
        U_ASSERT(data32!=تش_اشاره); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2_Null_دننه_واټن=trie->newTrie->index2_Null_دننه_واټن;
        null_بلاک=trie->newTrie->معلومات_Null_دننه_واټن;
    }

    لوړ_پیل=trie->لوړ_پیل;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_ارزښت=enum_ارزښت(context, trie->initial_ارزښت);

    /* set variables for previous range */
    prev_I2_بلاک=-1;
    prev_بلاک=-1;
    prev=پیل;
    prev_ارزښت=0;

    /* enumerate index-2 blocks */
    لكل(c=پیل; c<حد && c<لوړ_پیل;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 لنډمهاله_حد=c+UTRIE2_CP_فی_شاخص_1_ثبت;
        إذا(حد<لنډمهاله_حد) {
            لنډمهاله_حد=حد;
        }
        إذا(c<=0xffff) {
            إذا(!U_IS_SURROGATE(c)) {
                i2_بلاک=c>>UTRIE2_ښويدنه_2;
            } وإلا إذا(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2_بلاک=UTRIE2_LSCP_شاخص_2_دننه_واټن;
                لنډمهاله_حد=MIN_ارزښت(0xdc00, حد);
            } وإلا {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2_بلاک=0xd800>>UTRIE2_ښويدنه_2;
                لنډمهاله_حد=MIN_ارزښت(0xe000, حد);
            }
        } وإلا {
            /* supplementary code points */
            إذا(idx!=تش_اشاره) {
                i2_بلاک=idx[(UTRIE2_شاخص_1_دننه_واټن-UTRIE2_OMITTED_BMP_شاخص_1_اوږدتیا)+
                              (c>>UTRIE2_ښويدنه_1)];
            } وإلا {
                i2_بلاک=trie->newTrie->index1[c>>UTRIE2_ښويدنه_1];
            }
            إذا(i2_بلاک==prev_I2_بلاک && (c-prev)>=UTRIE2_CP_فی_شاخص_1_ثبت) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_فی_شاخص_1_ثبت;
                استمر;
            }
        }
        prev_I2_بلاک=i2_بلاک;
        إذا(i2_بلاک==index2_Null_دننه_واټن) {
            /* this is the null index-2 block */
            إذا(prev_ارزښت!=initial_ارزښت) {
                إذا(prev<c && !enumRange(context, prev, c-1, prev_ارزښت)) {
                    أرجع;
                }
                prev_بلاک=null_بلاک;
                prev=c;
                prev_ارزښت=initial_ارزښت;
            }
            c+=UTRIE2_CP_فی_شاخص_1_ثبت;
        } وإلا {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2_حد;
            i2=(c>>UTRIE2_ښويدنه_2)&UTRIE2_شاخص_2_ماسک;
            إذا((c>>UTRIE2_ښويدنه_1)==(لنډمهاله_حد>>UTRIE2_ښويدنه_1)) {
                i2_حد=(لنډمهاله_حد>>UTRIE2_ښويدنه_2)&UTRIE2_شاخص_2_ماسک;
            } وإلا {
                i2_حد=UTRIE2_شاخص_2_بلاک_اوږدتیا;
            }
            لكل(; i2<i2_حد; ++i2) {
                إذا(idx!=تش_اشاره) {
                    بلاک = static_cast<int32_t>(idx[i2_بلاک + i2]) << UTRIE2_شاخص_ښويدنه;
                } وإلا {
                    بلاک=trie->newTrie->index2[i2_بلاک+i2];
                }
                إذا(بلاک==prev_بلاک && (c-prev)>=UTRIE2_معلومات_بلاک_اوږدتیا) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_معلومات_بلاک_اوږدتیا;
                    استمر;
                }
                prev_بلاک=بلاک;
                إذا(بلاک==null_بلاک) {
                    /* this is the null data block */
                    إذا(prev_ارزښت!=initial_ارزښت) {
                        إذا(prev<c && !enumRange(context, prev, c-1, prev_ارزښت)) {
                            أرجع;
                        }
                        prev=c;
                        prev_ارزښت=initial_ارزښت;
                    }
                    c+=UTRIE2_معلومات_بلاک_اوږدتیا;
                } وإلا {
                    لكل(j=0; j<UTRIE2_معلومات_بلاک_اوږدتیا; ++j) {
                        value=enum_ارزښت(context, data32!=تش_اشاره ? data32[بلاک+j] : idx[بلاک+j]);
                        إذا(value!=prev_ارزښت) {
                            إذا(prev<c && !enumRange(context, prev, c-1, prev_ارزښت)) {
                                أرجع;
                            }
                            prev=c;
                            prev_ارزښت=value;
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
        uint32_t لوړ_ارزښت;
        إذا(idx!=تش_اشاره) {
            لوړ_ارزښت=
                data32!=تش_اشاره ?
                    data32[trie->لوړ_ارزښت_شاخص] :
                    idx[trie->لوړ_ارزښت_شاخص];
        } وإلا {
            لوړ_ارزښت=trie->newTrie->معلومات[trie->newTrie->معلومات_اوږدتیا-UTRIE2_معلومات_GRANULARITY];
        }
        value=enum_ارزښت(context, لوړ_ارزښت);
        إذا(value!=prev_ارزښت) {
            إذا(prev<c && !enumRange(context, prev, c-1, prev_ارزښت)) {
                أرجع;
            }
            prev=c;
            prev_ارزښت=value;
        }
        c=حد;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_ارزښت);
}

U_CAPI فراغ U_EXPORT2
utrie2_enum(ثابت UTrie2 *trie,
            UTrie2_Enum_ارزښت *enum_ارزښت, UTrie2EnumRange *enumRange, ثابت فراغ *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_ارزښت, enumRange, context);
}

U_CAPI فراغ U_EXPORT2
utrie2_enumForLeadSurrogate(ثابت UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_ارزښت *enum_ارزښت, UTrie2EnumRange *enumRange,
                            ثابت فراغ *context) {
    إذا(!U16_IS_LEAD(lead)) {
        أرجع;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_ارزښت, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    کوډ_ټکی_حد=کوډ_ټکی_پیل;
    إذا(پیل>=کوډ_ټکی_پیل) {
        کوډ_ټکی=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->error_ارزښت);
    }
    uint16_t پایله;
    UTRIE2_U16_PREV16(trie, پیل, کوډ_ټکی_پیل, کوډ_ټکی, پایله);
    أرجع پایله;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    کوډ_ټکی_پیل=کوډ_ټکی_حد;
    إذا(کوډ_ټکی_حد==حد) {
        کوډ_ټکی=U_SENTINEL;
        أرجع static_cast<uint16_t>(trie->error_ارزښت);
    }
    uint16_t پایله;
    UTRIE2_U16_NEXT16(trie, کوډ_ټکی_حد, حد, کوډ_ټکی, پایله);
    أرجع پایله;
}

U_NAMESPACE_پای_ټکی

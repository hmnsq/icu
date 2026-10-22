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
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/umutablecptrie.h"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf8.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utrie2_impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

ਸਥਿਰ uint32_t
get32(ਅਚਲ UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, ਖੰਡ;

    ਜੇ(c>=trie->ਉੱਚਾ_ਸ਼ੁਰੂ && (!U_IS_LEAD(c) || fromLSCP)) {
        ਵਾਪਸ trie->ਅੰਕੜੇ[trie->ਅੰਕੜੇ_ਲੰਮਾਈ-UTRIE2_ਅੰਕੜੇ_GRANULARITY];
    }

    ਜੇ(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_ਸੂਚਕਾਂਕ_2_ਵਿੱਥ-(0xd800>>UTRIE2_ਖਿਸਕਾਓ_2))+
            (c>>UTRIE2_ਖਿਸਕਾਓ_2);
    } ਨਹੀਂਤਾਂ {
        i2=trie->index1[c>>UTRIE2_ਖਿਸਕਾਓ_1]+
            ((c>>UTRIE2_ਖਿਸਕਾਓ_2)&UTRIE2_ਸੂਚਕਾਂਕ_2_ਨਕਾਬ);
    }
    ਖੰਡ=trie->index2[i2];
    ਵਾਪਸ trie->ਅੰਕੜੇ[ਖੰਡ+(c&UTRIE2_ਅੰਕੜੇ_ਨਕਾਬ)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ਅਚਲ UTrie2 *trie, UChar32 c) {
    ਜੇ(trie->data16!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ UTRIE2_GET16(trie, c);
    } ਨਹੀਂਤਾਂ ਜੇ(trie->data32!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ UTRIE2_GET32(trie, c);
    } ਨਹੀਂਤਾਂ ਜੇ((uint32_t)c>0x10ffff) {
        ਵਾਪਸ trie->error_ਮੁੱਲ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ get32(trie->newTrie, c, ਸੱਚ);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_ਕੋਡ_Unit(ਅਚਲ UTrie2 *trie, UChar32 c) {
    ਜੇ(!U_IS_LEAD(c)) {
        ਵਾਪਸ trie->error_ਮੁੱਲ;
    }
    ਜੇ(trie->data16!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } ਨਹੀਂਤਾਂ ਜੇ(trie->data32!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ get32(trie->newTrie, c, ਝੂਠ);
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
u8_ਸੂਚਕਾਂਕ(ਅਚਲ UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_ਸੂਚਕਾਂਕ_FROM_CP(
            trie,
            trie->data32==ਖਾਲੀ_ਸੂਚਕ ? trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ : 0,
            c);
    ਵਾਪਸ (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_ਅੰਦਰੂਨੀ_U8_ਅਗਲਾ_ਸੂਚਕਾਂਕ(ਅਚਲ UTrie2 *trie, UChar32 c,
                           ਅਚਲ uint8_t *src, ਅਚਲ uint8_t *ਹੱਦਬੰਦੀ) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ਜੇ((ਹੱਦਬੰਦੀ-src)<=7) {
        length=(int32_t)(ਹੱਦਬੰਦੀ-src);
    } ਨਹੀਂਤਾਂ {
        length=7;
    }
    c=utf8_ਅਗਲਾ_Char_ਸੁਰੱਖਿਅਤ_Body(src, &i, length, c, -1);
    ਵਾਪਸ u8_ਸੂਚਕਾਂਕ(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_ਅੰਦਰੂਨੀ_U8_Prev_ਸੂਚਕਾਂਕ(ਅਚਲ UTrie2 *trie, UChar32 c,
                           ਅਚਲ uint8_t *ਸ਼ੁਰੂ, ਅਚਲ uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ਜੇ((src-ਸ਼ੁਰੂ)<=7) {
        i=length=(int32_t)(src-ਸ਼ੁਰੂ);
    } ਨਹੀਂਤਾਂ {
        i=length=7;
        ਸ਼ੁਰੂ=src-7;
    }
    c=utf8_prev_Char_ਸੁਰੱਖਿਅਤ_Body(ਸ਼ੁਰੂ, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    ਵਾਪਸ u8_ਸੂਚਕਾਂਕ(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_ਮੁੱਲ_ਬਿੱਟ ਮੁੱਲ_ਬਿੱਟ,
                          ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, int32_t length, int32_t *p_Actual_ਲੰਮਾਈ,
                          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UTrie2_ਸਿਰਲੇਖ *ਸਿਰਲੇਖ;
    ਅਚਲ uint16_t *p16;
    int32_t actual_ਲੰਮਾਈ;

    UTrie2 ਆਰਜ਼ੀ_Trie;
    UTrie2 *trie;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ( length<=0 || (U_ਸੂਚਕ_ਨਕਾਬ_LSB(ਅੰਕੜੇ, 3)!=0) ||
        ਮੁੱਲ_ਬਿੱਟ<0 || UTRIE2_ਗਿਣਤੀ_ਮੁੱਲ_ਬਿੱਟ<=ਮੁੱਲ_ਬਿੱਟ
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* enough data for a trie header? */
    ਜੇ(length<(int32_t)ਆਕਾਰ(UTrie2_ਸਿਰਲੇਖ)) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* check the signature */
    ਸਿਰਲੇਖ=(ਅਚਲ UTrie2_ਸਿਰਲੇਖ *)ਅੰਕੜੇ;
    ਜੇ(ਸਿਰਲੇਖ->signature!=UTRIE2_SIG) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* get the options */
    ਜੇ(ਮੁੱਲ_ਬਿੱਟ!=(UTrie2_ਮੁੱਲ_ਬਿੱਟ)(ਸਿਰਲੇਖ->options&UTRIE2_OPTIONS_ਮੁੱਲ_ਬਿੱਟ_ਨਕਾਬ)) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* get the length values and offsets */
    uprv_memset(&ਆਰਜ਼ੀ_Trie, 0, ਆਕਾਰ(ਆਰਜ਼ੀ_Trie));
    ਆਰਜ਼ੀ_Trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=ਸਿਰਲੇਖ->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_ਲੰਮਾਈ=ਸਿਰਲੇਖ->shifted_ਅੰਕੜੇ_ਲੰਮਾਈ<<UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
    ਆਰਜ਼ੀ_Trie.index2_Null_ਵਿੱਥ=ਸਿਰਲੇਖ->index2_Null_ਵਿੱਥ;
    ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_Null_ਵਿੱਥ=ਸਿਰਲੇਖ->ਅੰਕੜੇ_Null_ਵਿੱਥ;

    ਆਰਜ਼ੀ_Trie.ਉੱਚਾ_ਸ਼ੁਰੂ=ਸਿਰਲੇਖ->shifted_ਉੱਚਾ_ਸ਼ੁਰੂ<<UTRIE2_ਖਿਸਕਾਓ_1;
    ਆਰਜ਼ੀ_Trie.ਉੱਚਾ_ਮੁੱਲ_ਸੂਚਕਾਂਕ=ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_ਲੰਮਾਈ-UTRIE2_ਅੰਕੜੇ_GRANULARITY;
    ਜੇ(ਮੁੱਲ_ਬਿੱਟ==UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ) {
        ਆਰਜ਼ੀ_Trie.ਉੱਚਾ_ਮੁੱਲ_ਸੂਚਕਾਂਕ+=ਆਰਜ਼ੀ_Trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    }

    /* calculate the actual length */
    actual_ਲੰਮਾਈ=(int32_t)ਆਕਾਰ(UTrie2_ਸਿਰਲੇਖ)+ਆਰਜ਼ੀ_Trie.ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2;
    ਜੇ(ਮੁੱਲ_ਬਿੱਟ==UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ) {
        actual_ਲੰਮਾਈ+=ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_ਲੰਮਾਈ*2;
    } ਨਹੀਂਤਾਂ {
        actual_ਲੰਮਾਈ+=ਆਰਜ਼ੀ_Trie.ਅੰਕੜੇ_ਲੰਮਾਈ*4;
    }
    ਜੇ(length<actual_ਲੰਮਾਈ) {
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(ਆਕਾਰ(UTrie2));
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memcpy(trie, &ਆਰਜ਼ੀ_Trie, ਆਕਾਰ(ਆਰਜ਼ੀ_Trie));
    trie->ਸਮ੍ਰਿਤੀ=(uint32_t *)ਅੰਕੜੇ;
    trie->length=actual_ਲੰਮਾਈ;
    trie->is_ਸਮ੍ਰਿਤੀ_Owned=ਝੂਠ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    trie->name="fromSerialized";
#ਜੇ_ਅੰਤ

    /* set the pointers to its index and data arrays */
    p16=(ਅਚਲ uint16_t *)(ਸਿਰਲੇਖ+1);
    trie->ਸੂਚਕਾਂਕ=p16;
    p16+=trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;

    /* get the data */
    ਚੋਣ(ਮੁੱਲ_ਬਿੱਟ) {
    ਹਾਲਤ UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ:
        trie->data16=p16;
        trie->data32=ਖਾਲੀ_ਸੂਚਕ;
        trie->initial_ਮੁੱਲ=trie->ਸੂਚਕਾਂਕ[trie->ਅੰਕੜੇ_Null_ਵਿੱਥ];
        trie->error_ਮੁੱਲ=trie->data16[UTRIE2_BAD_UTF8_ਅੰਕੜੇ_ਵਿੱਥ];
        ਰੋਕੋ;
    ਹਾਲਤ UTRIE2_32_ਮੁੱਲ_ਬਿੱਟ:
        trie->data16=ਖਾਲੀ_ਸੂਚਕ;
        trie->data32=(ਅਚਲ uint32_t *)p16;
        trie->initial_ਮੁੱਲ=trie->data32[trie->ਅੰਕੜੇ_Null_ਵਿੱਥ];
        trie->error_ਮੁੱਲ=trie->data32[UTRIE2_BAD_UTF8_ਅੰਕੜੇ_ਵਿੱਥ];
        ਰੋਕੋ;
    ਮੂਲ:
        *p_Error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(p_Actual_ਲੰਮਾਈ!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_Actual_ਲੰਮਾਈ=actual_ਲੰਮਾਈ;
    }
    ਵਾਪਸ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_ਮੁੱਲ_ਬਿੱਟ ਮੁੱਲ_ਬਿੱਟ,
                 uint32_t initial_ਮੁੱਲ, uint32_t error_ਮੁੱਲ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UTrie2 *trie;
    UTrie2_ਸਿਰਲੇਖ *ਸਿਰਲੇਖ;
    uint32_t *p;
    uint16_t *dest16;
    int32_t ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਅੰਕੜੇ_ਲੰਮਾਈ, length, i;
    int32_t ਅੰਕੜੇ_Move;  /* >0 if the data is moved to the end of the index array */

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ(ਮੁੱਲ_ਬਿੱਟ<0 || UTRIE2_ਗਿਣਤੀ_ਮੁੱਲ_ਬਿੱਟ<=ਮੁੱਲ_ਬਿੱਟ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* calculate the total length of the dummy trie data */
    ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=UTRIE2_ਸੂਚਕਾਂਕ_1_ਵਿੱਥ;
    ਅੰਕੜੇ_ਲੰਮਾਈ=UTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ+UTRIE2_ਅੰਕੜੇ_GRANULARITY;
    length=(int32_t)ਆਕਾਰ(UTrie2_ਸਿਰਲੇਖ)+ਸੂਚਕਾਂਕ_ਲੰਮਾਈ*2;
    ਜੇ(ਮੁੱਲ_ਬਿੱਟ==UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ) {
        length+=ਅੰਕੜੇ_ਲੰਮਾਈ*2;
    } ਨਹੀਂਤਾਂ {
        length+=ਅੰਕੜੇ_ਲੰਮਾਈ*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(ਆਕਾਰ(UTrie2));
    ਜੇ(trie==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_memset(trie, 0, ਆਕਾਰ(UTrie2));
    trie->ਸਮ੍ਰਿਤੀ=uprv_malloc(length);
    ਜੇ(trie->ਸਮ੍ਰਿਤੀ==ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(trie);
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    trie->length=length;
    trie->is_ਸਮ੍ਰਿਤੀ_Owned=ਸੱਚ;

    /* set the UTrie2 fields */
    ਜੇ(ਮੁੱਲ_ਬਿੱਟ==UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ) {
        ਅੰਕੜੇ_Move=ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        ਅੰਕੜੇ_Move=0;
    }

    trie->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    trie->ਅੰਕੜੇ_ਲੰਮਾਈ=ਅੰਕੜੇ_ਲੰਮਾਈ;
    trie->index2_Null_ਵਿੱਥ=UTRIE2_ਸੂਚਕਾਂਕ_2_ਵਿੱਥ;
    trie->ਅੰਕੜੇ_Null_ਵਿੱਥ=(uint16_t)ਅੰਕੜੇ_Move;
    trie->initial_ਮੁੱਲ=initial_ਮੁੱਲ;
    trie->error_ਮੁੱਲ=error_ਮੁੱਲ;
    trie->ਉੱਚਾ_ਸ਼ੁਰੂ=0;
    trie->ਉੱਚਾ_ਮੁੱਲ_ਸੂਚਕਾਂਕ=ਅੰਕੜੇ_Move+UTRIE2_ਅੰਕੜੇ_ਸ਼ੁਰੂ_ਵਿੱਥ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UTRIE2_DEBUG
    trie->name="dummy";
#ਜੇ_ਅੰਤ

    /* set the header fields */
    ਸਿਰਲੇਖ=(UTrie2_ਸਿਰਲੇਖ *)trie->ਸਮ੍ਰਿਤੀ;

    ਸਿਰਲੇਖ->signature=UTRIE2_SIG; /* "Tri2" */
    ਸਿਰਲੇਖ->options=(uint16_t)ਮੁੱਲ_ਬਿੱਟ;

    ਸਿਰਲੇਖ->ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=(uint16_t)ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਸਿਰਲੇਖ->shifted_ਅੰਕੜੇ_ਲੰਮਾਈ=(uint16_t)(ਅੰਕੜੇ_ਲੰਮਾਈ>>UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);
    ਸਿਰਲੇਖ->index2_Null_ਵਿੱਥ=(uint16_t)UTRIE2_ਸੂਚਕਾਂਕ_2_ਵਿੱਥ;
    ਸਿਰਲੇਖ->ਅੰਕੜੇ_Null_ਵਿੱਥ=(uint16_t)ਅੰਕੜੇ_Move;
    ਸਿਰਲੇਖ->shifted_ਉੱਚਾ_ਸ਼ੁਰੂ=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(ਸਿਰਲੇਖ+1);
    trie->ਸੂਚਕਾਂਕ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    ਲਈ(i=0; i<UTRIE2_ਸੂਚਕਾਂਕ_2_BMP_ਲੰਮਾਈ; ++i) {
        *dest16++=(uint16_t)(ਅੰਕੜੇ_Move>>UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    ਲਈ(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(ਅੰਕੜੇ_Move+UTRIE2_BAD_UTF8_ਅੰਕੜੇ_ਵਿੱਥ);
    }
    ਲਈ(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)ਅੰਕੜੇ_Move;
    }

    /* write the 16/32-bit data array */
    ਚੋਣ(ਮੁੱਲ_ਬਿੱਟ) {
    ਹਾਲਤ UTRIE2_16_ਮੁੱਲ_ਬਿੱਟ:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=ਖਾਲੀ_ਸੂਚਕ;
        ਲਈ(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_ਮੁੱਲ;
        }
        ਲਈ(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_ਮੁੱਲ;
        }
        /* highValue and reserved values */
        ਲਈ(i=0; i<UTRIE2_ਅੰਕੜੇ_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_ਮੁੱਲ;
        }
        ਰੋਕੋ;
    ਹਾਲਤ UTRIE2_32_ਮੁੱਲ_ਬਿੱਟ:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=ਖਾਲੀ_ਸੂਚਕ;
        trie->data32=p;
        ਲਈ(i=0; i<0x80; ++i) {
            *p++=initial_ਮੁੱਲ;
        }
        ਲਈ(; i<0xc0; ++i) {
            *p++=error_ਮੁੱਲ;
        }
        /* highValue and reserved values */
        ਲਈ(i=0; i<UTRIE2_ਅੰਕੜੇ_GRANULARITY; ++i) {
            *p++=initial_ਮੁੱਲ;
        }
        ਰੋਕੋ;
    ਮੂਲ:
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ trie;
}

U_CAPI ਖਾਲੀ U_EXPORT2
utrie2_close(UTrie2 *trie) {
    ਜੇ(trie!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(trie->is_ਸਮ੍ਰਿਤੀ_Owned) {
            uprv_free(trie->ਸਮ੍ਰਿਤੀ);
        }
        ਜੇ(trie->newTrie!=ਖਾਲੀ_ਸੂਚਕ) {
            uprv_free(trie->newTrie->ਅੰਕੜੇ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#ਜੇ_ਅੰਤ
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ਅਚਲ UTrie2 *trie) {
    ਵਾਪਸ trie->newTrie==ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ਅਚਲ UTrie2 *trie,
                 ਖਾਲੀ *ਅੰਕੜੇ, int32_t capacity,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    /* argument check */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    ਜੇ( trie==ਖਾਲੀ_ਸੂਚਕ || trie->ਸਮ੍ਰਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || trie->newTrie!=ਖਾਲੀ_ਸੂਚਕ ||
        capacity<0 || (capacity>0 && (ਅੰਕੜੇ==ਖਾਲੀ_ਸੂਚਕ || (U_ਸੂਚਕ_ਨਕਾਬ_LSB(ਅੰਕੜੇ, 3)!=0)))
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(capacity>=trie->length) {
        uprv_memcpy(ਅੰਕੜੇ, trie->ਸਮ੍ਰਿਤੀ, trie->length);
    } ਨਹੀਂਤਾਂ {
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
    }
    ਵਾਪਸ trie->length;
}

/* enumeration -------------------------------------------------------------- */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIN_ਮੁੱਲ(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ਸਥਿਰ uint32_t U_CALLCONV
enum_Same_ਮੁੱਲ(ਅਚਲ ਖਾਲੀ * /*context*/, uint32_t value) {
    ਵਾਪਸ value;
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
ਸਥਿਰ ਖਾਲੀ
enumEitherTrie(ਅਚਲ UTrie2 *trie,
               UChar32 ਸ਼ੁਰੂ, UChar32 ਹੱਦਬੰਦੀ,
               UTrie2_Enum_ਮੁੱਲ *enum_ਮੁੱਲ, UTrie2EnumRange *enumRange, ਅਚਲ ਖਾਲੀ *context) {
    ਅਚਲ uint32_t *data32;
    ਅਚਲ uint16_t *idx;

    uint32_t value, prev_ਮੁੱਲ, initial_ਮੁੱਲ;
    UChar32 c, prev, ਉੱਚਾ_ਸ਼ੁਰੂ;
    int32_t j, i2_ਖੰਡ, prev_I2_ਖੰਡ, index2_Null_ਵਿੱਥ, ਖੰਡ, prev_ਖੰਡ, null_ਖੰਡ;

    ਜੇ(enumRange==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }
    ਜੇ(enum_ਮੁੱਲ==ਖਾਲੀ_ਸੂਚਕ) {
        enum_ਮੁੱਲ=enum_Same_ਮੁੱਲ;
    }

    ਜੇ(trie->newTrie==ਖਾਲੀ_ਸੂਚਕ) {
        /* frozen trie */
        idx=trie->ਸੂਚਕਾਂਕ;
        U_ASSERT(idx!=ਖਾਲੀ_ਸੂਚਕ); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2_Null_ਵਿੱਥ=trie->index2_Null_ਵਿੱਥ;
        null_ਖੰਡ=trie->ਅੰਕੜੇ_Null_ਵਿੱਥ;
    } ਨਹੀਂਤਾਂ {
        /* unfrozen, mutable trie */
        idx=ਖਾਲੀ_ਸੂਚਕ;
        data32=trie->newTrie->ਅੰਕੜੇ;
        U_ASSERT(data32!=ਖਾਲੀ_ਸੂਚਕ); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2_Null_ਵਿੱਥ=trie->newTrie->index2_Null_ਵਿੱਥ;
        null_ਖੰਡ=trie->newTrie->ਅੰਕੜੇ_Null_ਵਿੱਥ;
    }

    ਉੱਚਾ_ਸ਼ੁਰੂ=trie->ਉੱਚਾ_ਸ਼ੁਰੂ;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_ਮੁੱਲ=enum_ਮੁੱਲ(context, trie->initial_ਮੁੱਲ);

    /* set variables for previous range */
    prev_I2_ਖੰਡ=-1;
    prev_ਖੰਡ=-1;
    prev=ਸ਼ੁਰੂ;
    prev_ਮੁੱਲ=0;

    /* enumerate index-2 blocks */
    ਲਈ(c=ਸ਼ੁਰੂ; c<ਹੱਦਬੰਦੀ && c<ਉੱਚਾ_ਸ਼ੁਰੂ;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 ਆਰਜ਼ੀ_ਹੱਦਬੰਦੀ=c+UTRIE2_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_1_ਇੰਦਰਾਜ;
        ਜੇ(ਹੱਦਬੰਦੀ<ਆਰਜ਼ੀ_ਹੱਦਬੰਦੀ) {
            ਆਰਜ਼ੀ_ਹੱਦਬੰਦੀ=ਹੱਦਬੰਦੀ;
        }
        ਜੇ(c<=0xffff) {
            ਜੇ(!U_IS_SURROGATE(c)) {
                i2_ਖੰਡ=c>>UTRIE2_ਖਿਸਕਾਓ_2;
            } ਨਹੀਂਤਾਂ ਜੇ(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2_ਖੰਡ=UTRIE2_LSCP_ਸੂਚਕਾਂਕ_2_ਵਿੱਥ;
                ਆਰਜ਼ੀ_ਹੱਦਬੰਦੀ=MIN_ਮੁੱਲ(0xdc00, ਹੱਦਬੰਦੀ);
            } ਨਹੀਂਤਾਂ {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2_ਖੰਡ=0xd800>>UTRIE2_ਖਿਸਕਾਓ_2;
                ਆਰਜ਼ੀ_ਹੱਦਬੰਦੀ=MIN_ਮੁੱਲ(0xe000, ਹੱਦਬੰਦੀ);
            }
        } ਨਹੀਂਤਾਂ {
            /* supplementary code points */
            ਜੇ(idx!=ਖਾਲੀ_ਸੂਚਕ) {
                i2_ਖੰਡ=idx[(UTRIE2_ਸੂਚਕਾਂਕ_1_ਵਿੱਥ-UTRIE2_OMITTED_BMP_ਸੂਚਕਾਂਕ_1_ਲੰਮਾਈ)+
                              (c>>UTRIE2_ਖਿਸਕਾਓ_1)];
            } ਨਹੀਂਤਾਂ {
                i2_ਖੰਡ=trie->newTrie->index1[c>>UTRIE2_ਖਿਸਕਾਓ_1];
            }
            ਜੇ(i2_ਖੰਡ==prev_I2_ਖੰਡ && (c-prev)>=UTRIE2_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_1_ਇੰਦਰਾਜ) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_1_ਇੰਦਰਾਜ;
                ਜਾਰੀ;
            }
        }
        prev_I2_ਖੰਡ=i2_ਖੰਡ;
        ਜੇ(i2_ਖੰਡ==index2_Null_ਵਿੱਥ) {
            /* this is the null index-2 block */
            ਜੇ(prev_ਮੁੱਲ!=initial_ਮੁੱਲ) {
                ਜੇ(prev<c && !enumRange(context, prev, c-1, prev_ਮੁੱਲ)) {
                    ਵਾਪਸ;
                }
                prev_ਖੰਡ=null_ਖੰਡ;
                prev=c;
                prev_ਮੁੱਲ=initial_ਮੁੱਲ;
            }
            c+=UTRIE2_CP_ਪ੍ਰਤੀ_ਸੂਚਕਾਂਕ_1_ਇੰਦਰਾਜ;
        } ਨਹੀਂਤਾਂ {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2_ਹੱਦਬੰਦੀ;
            i2=(c>>UTRIE2_ਖਿਸਕਾਓ_2)&UTRIE2_ਸੂਚਕਾਂਕ_2_ਨਕਾਬ;
            ਜੇ((c>>UTRIE2_ਖਿਸਕਾਓ_1)==(ਆਰਜ਼ੀ_ਹੱਦਬੰਦੀ>>UTRIE2_ਖਿਸਕਾਓ_1)) {
                i2_ਹੱਦਬੰਦੀ=(ਆਰਜ਼ੀ_ਹੱਦਬੰਦੀ>>UTRIE2_ਖਿਸਕਾਓ_2)&UTRIE2_ਸੂਚਕਾਂਕ_2_ਨਕਾਬ;
            } ਨਹੀਂਤਾਂ {
                i2_ਹੱਦਬੰਦੀ=UTRIE2_ਸੂਚਕਾਂਕ_2_ਖੰਡ_ਲੰਮਾਈ;
            }
            ਲਈ(; i2<i2_ਹੱਦਬੰਦੀ; ++i2) {
                ਜੇ(idx!=ਖਾਲੀ_ਸੂਚਕ) {
                    ਖੰਡ = static_cast<int32_t>(idx[i2_ਖੰਡ + i2]) << UTRIE2_ਸੂਚਕਾਂਕ_ਖਿਸਕਾਓ;
                } ਨਹੀਂਤਾਂ {
                    ਖੰਡ=trie->newTrie->index2[i2_ਖੰਡ+i2];
                }
                ਜੇ(ਖੰਡ==prev_ਖੰਡ && (c-prev)>=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
                    ਜਾਰੀ;
                }
                prev_ਖੰਡ=ਖੰਡ;
                ਜੇ(ਖੰਡ==null_ਖੰਡ) {
                    /* this is the null data block */
                    ਜੇ(prev_ਮੁੱਲ!=initial_ਮੁੱਲ) {
                        ਜੇ(prev<c && !enumRange(context, prev, c-1, prev_ਮੁੱਲ)) {
                            ਵਾਪਸ;
                        }
                        prev=c;
                        prev_ਮੁੱਲ=initial_ਮੁੱਲ;
                    }
                    c+=UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ;
                } ਨਹੀਂਤਾਂ {
                    ਲਈ(j=0; j<UTRIE2_ਅੰਕੜੇ_ਖੰਡ_ਲੰਮਾਈ; ++j) {
                        value=enum_ਮੁੱਲ(context, data32!=ਖਾਲੀ_ਸੂਚਕ ? data32[ਖੰਡ+j] : idx[ਖੰਡ+j]);
                        ਜੇ(value!=prev_ਮੁੱਲ) {
                            ਜੇ(prev<c && !enumRange(context, prev, c-1, prev_ਮੁੱਲ)) {
                                ਵਾਪਸ;
                            }
                            prev=c;
                            prev_ਮੁੱਲ=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    ਜੇ(c>ਹੱਦਬੰਦੀ) {
        c=ਹੱਦਬੰਦੀ;  /* could be higher if in the index2NullOffset */
    } ਨਹੀਂਤਾਂ ਜੇ(c<ਹੱਦਬੰਦੀ) {
        /* c==highStart<limit */
        uint32_t ਉੱਚਾ_ਮੁੱਲ;
        ਜੇ(idx!=ਖਾਲੀ_ਸੂਚਕ) {
            ਉੱਚਾ_ਮੁੱਲ=
                data32!=ਖਾਲੀ_ਸੂਚਕ ?
                    data32[trie->ਉੱਚਾ_ਮੁੱਲ_ਸੂਚਕਾਂਕ] :
                    idx[trie->ਉੱਚਾ_ਮੁੱਲ_ਸੂਚਕਾਂਕ];
        } ਨਹੀਂਤਾਂ {
            ਉੱਚਾ_ਮੁੱਲ=trie->newTrie->ਅੰਕੜੇ[trie->newTrie->ਅੰਕੜੇ_ਲੰਮਾਈ-UTRIE2_ਅੰਕੜੇ_GRANULARITY];
        }
        value=enum_ਮੁੱਲ(context, ਉੱਚਾ_ਮੁੱਲ);
        ਜੇ(value!=prev_ਮੁੱਲ) {
            ਜੇ(prev<c && !enumRange(context, prev, c-1, prev_ਮੁੱਲ)) {
                ਵਾਪਸ;
            }
            prev=c;
            prev_ਮੁੱਲ=value;
        }
        c=ਹੱਦਬੰਦੀ;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_ਮੁੱਲ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
utrie2_enum(ਅਚਲ UTrie2 *trie,
            UTrie2_Enum_ਮੁੱਲ *enum_ਮੁੱਲ, UTrie2EnumRange *enumRange, ਅਚਲ ਖਾਲੀ *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_ਮੁੱਲ, enumRange, context);
}

U_CAPI ਖਾਲੀ U_EXPORT2
utrie2_enumForLeadSurrogate(ਅਚਲ UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_ਮੁੱਲ *enum_ਮੁੱਲ, UTrie2EnumRange *enumRange,
                            ਅਚਲ ਖਾਲੀ *context) {
    ਜੇ(!U16_IS_LEAD(lead)) {
        ਵਾਪਸ;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_ਮੁੱਲ, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ=ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ;
    ਜੇ(ਸ਼ੁਰੂ>=ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ) {
        ਕੋਡ_ਬਿੰਦੂ=U_SENTINEL;
        ਵਾਪਸ static_cast<uint16_t>(trie->error_ਮੁੱਲ);
    }
    uint16_t ਨਤੀਜਾ;
    UTRIE2_U16_PREV16(trie, ਸ਼ੁਰੂ, ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ, ਕੋਡ_ਬਿੰਦੂ, ਨਤੀਜਾ);
    ਵਾਪਸ ਨਤੀਜਾ;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    ਕੋਡ_ਬਿੰਦੂ_ਸ਼ੁਰੂ=ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ;
    ਜੇ(ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ==ਹੱਦਬੰਦੀ) {
        ਕੋਡ_ਬਿੰਦੂ=U_SENTINEL;
        ਵਾਪਸ static_cast<uint16_t>(trie->error_ਮੁੱਲ);
    }
    uint16_t ਨਤੀਜਾ;
    UTRIE2_U16_NEXT16(trie, ਕੋਡ_ਬਿੰਦੂ_ਹੱਦਬੰਦੀ, ਹੱਦਬੰਦੀ, ਕੋਡ_ਬਿੰਦੂ, ਨਤੀਜਾ);
    ਵਾਪਸ ਨਤੀਜਾ;
}

U_NAMESPACE_ਸਿਰਾ

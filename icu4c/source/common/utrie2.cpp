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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
#ಅನ್ತರ್ಭಾವ "unicode/umutablecptrie.h"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf8.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "utrie2.h"
#ಅನ್ತರ್ಭಾವ "utrie2_impl.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

ಸ್ಥಿರ uint32_t
get32(ಸ್ಥಿರಾಂಕ UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, ಖಂಡ;

    ಆದರೆ(c>=trie->ಮೇಲಿನ_ಆರಂಭ && (!U_IS_LEAD(c) || fromLSCP)) {
        ಹಿಂತಿರುಗಿ trie->ದತ್ತಾಂಶ[trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ-UTRIE2_ದತ್ತಾಂಶ_GRANULARITY];
    }

    ಆದರೆ(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_ಸೂಚ್ಯಂಕ_2_ಸ್ಥಾನಾಂತರ-(0xd800>>UTRIE2_ಸರಿಸು_2))+
            (c>>UTRIE2_ಸರಿಸು_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        i2=trie->index1[c>>UTRIE2_ಸರಿಸು_1]+
            ((c>>UTRIE2_ಸರಿಸು_2)&UTRIE2_ಸೂಚ್ಯಂಕ_2_ಮುಖವಾಡ);
    }
    ಖಂಡ=trie->index2[i2];
    ಹಿಂತಿರುಗಿ trie->ದತ್ತಾಂಶ[ಖಂಡ+(c&UTRIE2_ದತ್ತಾಂಶ_ಮುಖವಾಡ)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(ಸ್ಥಿರಾಂಕ UTrie2 *trie, UChar32 c) {
    ಆದರೆ(trie->data16!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ UTRIE2_GET16(trie, c);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(trie->data32!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ UTRIE2_GET32(trie, c);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint32_t)c>0x10ffff) {
        ಹಿಂತಿರುಗಿ trie->error_ಮೌಲ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ get32(trie->newTrie, c, ನಿಜ);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_ಸಂಕೇತ_Unit(ಸ್ಥಿರಾಂಕ UTrie2 *trie, UChar32 c) {
    ಆದರೆ(!U_IS_LEAD(c)) {
        ಹಿಂತಿರುಗಿ trie->error_ಮೌಲ್ಯ;
    }
    ಆದರೆ(trie->data16!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(trie->data32!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ get32(trie->newTrie, c, ಸುಳ್ಳು);
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
u8_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_ಸೂಚ್ಯಂಕ_FROM_CP(
            trie,
            trie->data32==ಶೂನ್ಯಸೂಚಕ ? trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ : 0,
            c);
    ಹಿಂತಿರುಗಿ (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_ಆಂತರಿಕ_U8_ಮುಂದಿನ_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UTrie2 *trie, UChar32 c,
                           ಸ್ಥಿರಾಂಕ uint8_t *src, ಸ್ಥಿರಾಂಕ uint8_t *ಎಲ್ಲೆ) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ಆದರೆ((ಎಲ್ಲೆ-src)<=7) {
        length=(int32_t)(ಎಲ್ಲೆ-src);
    } ಇಲ್ಲದಿದ್ದರೆ {
        length=7;
    }
    c=utf8_ಮುಂದಿನ_Char_ಸುರಕ್ಷಿತ_Body(src, &i, length, c, -1);
    ಹಿಂತಿರುಗಿ u8_ಸೂಚ್ಯಂಕ(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_ಆಂತರಿಕ_U8_Prev_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ UTrie2 *trie, UChar32 c,
                           ಸ್ಥಿರಾಂಕ uint8_t *ಆರಂಭ, ಸ್ಥಿರಾಂಕ uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ಆದರೆ((src-ಆರಂಭ)<=7) {
        i=length=(int32_t)(src-ಆರಂಭ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        i=length=7;
        ಆರಂಭ=src-7;
    }
    c=utf8_prev_Char_ಸುರಕ್ಷಿತ_Body(ಆರಂಭ, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    ಹಿಂತಿರುಗಿ u8_ಸೂಚ್ಯಂಕ(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, int32_t length, int32_t *p_Actual_ಉದ್ದಳತೆ,
                          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UTrie2_ಶೀರ್ಷಿಕೆ *ಶೀರ್ಷಿಕೆ;
    ಸ್ಥಿರಾಂಕ uint16_t *p16;
    int32_t actual_ಉದ್ದಳತೆ;

    UTrie2 ತಾತ್ಕಾಲಿಕ_Trie;
    UTrie2 *trie;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ( length<=0 || (U_ಸೂಚಕ_ಮುಖವಾಡ_LSB(ದತ್ತಾಂಶ, 3)!=0) ||
        ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು<0 || UTRIE2_ಎಣಿಕೆ_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು<=ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* enough data for a trie header? */
    ಆದರೆ(length<(int32_t)ಗಾತ್ರ(UTrie2_ಶೀರ್ಷಿಕೆ)) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* check the signature */
    ಶೀರ್ಷಿಕೆ=(ಸ್ಥಿರಾಂಕ UTrie2_ಶೀರ್ಷಿಕೆ *)ದತ್ತಾಂಶ;
    ಆದರೆ(ಶೀರ್ಷಿಕೆ->signature!=UTRIE2_SIG) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* get the options */
    ಆದರೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು!=(UTrie2_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು)(ಶೀರ್ಷಿಕೆ->options&UTRIE2_OPTIONS_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_ಮುಖವಾಡ)) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* get the length values and offsets */
    uprv_memset(&ತಾತ್ಕಾಲಿಕ_Trie, 0, ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ_Trie));
    ತಾತ್ಕಾಲಿಕ_Trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=ಶೀರ್ಷಿಕೆ->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=ಶೀರ್ಷಿಕೆ->shifted_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ<<UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು;
    ತಾತ್ಕಾಲಿಕ_Trie.index2_Null_ಸ್ಥಾನಾಂತರ=ಶೀರ್ಷಿಕೆ->index2_Null_ಸ್ಥಾನಾಂತರ;
    ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ=ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;

    ತಾತ್ಕಾಲಿಕ_Trie.ಮೇಲಿನ_ಆರಂಭ=ಶೀರ್ಷಿಕೆ->shifted_ಮೇಲಿನ_ಆರಂಭ<<UTRIE2_ಸರಿಸು_1;
    ತಾತ್ಕಾಲಿಕ_Trie.ಮೇಲಿನ_ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ=ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ-UTRIE2_ದತ್ತಾಂಶ_GRANULARITY;
    ಆದರೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು==UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
        ತಾತ್ಕಾಲಿಕ_Trie.ಮೇಲಿನ_ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ+=ತಾತ್ಕಾಲಿಕ_Trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    }

    /* calculate the actual length */
    actual_ಉದ್ದಳತೆ=(int32_t)ಗಾತ್ರ(UTrie2_ಶೀರ್ಷಿಕೆ)+ತಾತ್ಕಾಲಿಕ_Trie.ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2;
    ಆದರೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು==UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
        actual_ಉದ್ದಳತೆ+=ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        actual_ಉದ್ದಳತೆ+=ತಾತ್ಕಾಲಿಕ_Trie.ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4;
    }
    ಆದರೆ(length<actual_ಉದ್ದಳತೆ) {
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(ಗಾತ್ರ(UTrie2));
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memcpy(trie, &ತಾತ್ಕಾಲಿಕ_Trie, ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ_Trie));
    trie->ಸ್ಮರಣೆ=(uint32_t *)ದತ್ತಾಂಶ;
    trie->length=actual_ಉದ್ದಳತೆ;
    trie->is_ಸ್ಮರಣೆ_Owned=ಸುಳ್ಳು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    trie->name="fromSerialized";
#ಆದರೆ_ಅಂತ್ಯ

    /* set the pointers to its index and data arrays */
    p16=(ಸ್ಥಿರಾಂಕ uint16_t *)(ಶೀರ್ಷಿಕೆ+1);
    trie->ಸೂಚ್ಯಂಕ=p16;
    p16+=trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;

    /* get the data */
    ಆಯ್ಕೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
    ಸ್ಥಿತಿ UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
        trie->data16=p16;
        trie->data32=ಶೂನ್ಯಸೂಚಕ;
        trie->initial_ಮೌಲ್ಯ=trie->ಸೂಚ್ಯಂಕ[trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ];
        trie->error_ಮೌಲ್ಯ=trie->data16[UTRIE2_BAD_UTF8_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UTRIE2_32_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
        trie->data16=ಶೂನ್ಯಸೂಚಕ;
        trie->data32=(ಸ್ಥಿರಾಂಕ uint32_t *)p16;
        trie->initial_ಮೌಲ್ಯ=trie->data32[trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ];
        trie->error_ಮೌಲ್ಯ=trie->data32[UTRIE2_BAD_UTF8_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        *p_Error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(p_Actual_ಉದ್ದಳತೆ!=ಶೂನ್ಯಸೂಚಕ) {
        *p_Actual_ಉದ್ದಳತೆ=actual_ಉದ್ದಳತೆ;
    }
    ಹಿಂತಿರುಗಿ trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು,
                 uint32_t initial_ಮೌಲ್ಯ, uint32_t error_ಮೌಲ್ಯ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UTrie2 *trie;
    UTrie2_ಶೀರ್ಷಿಕೆ *ಶೀರ್ಷಿಕೆ;
    uint32_t *p;
    uint16_t *dest16;
    int32_t ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ, length, i;
    int32_t ದತ್ತಾಂಶ_Move;  /* >0 if the data is moved to the end of the index array */

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು<0 || UTRIE2_ಎಣಿಕೆ_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು<=ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* calculate the total length of the dummy trie data */
    ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=UTRIE2_ಸೂಚ್ಯಂಕ_1_ಸ್ಥಾನಾಂತರ;
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=UTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ+UTRIE2_ದತ್ತಾಂಶ_GRANULARITY;
    length=(int32_t)ಗಾತ್ರ(UTrie2_ಶೀರ್ಷಿಕೆ)+ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ*2;
    ಆದರೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು==UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
        length+=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        length+=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(ಗಾತ್ರ(UTrie2));
    ಆದರೆ(trie==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_memset(trie, 0, ಗಾತ್ರ(UTrie2));
    trie->ಸ್ಮರಣೆ=uprv_malloc(length);
    ಆದರೆ(trie->ಸ್ಮರಣೆ==ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(trie);
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    trie->length=length;
    trie->is_ಸ್ಮರಣೆ_Owned=ನಿಜ;

    /* set the UTrie2 fields */
    ಆದರೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು==UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
        ದತ್ತಾಂಶ_Move=ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ದತ್ತಾಂಶ_Move=0;
    }

    trie->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    trie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=ದತ್ತಾಂಶ_ಉದ್ದಳತೆ;
    trie->index2_Null_ಸ್ಥಾನಾಂತರ=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಸ್ಥಾನಾಂತರ;
    trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ=(uint16_t)ದತ್ತಾಂಶ_Move;
    trie->initial_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
    trie->error_ಮೌಲ್ಯ=error_ಮೌಲ್ಯ;
    trie->ಮೇಲಿನ_ಆರಂಭ=0;
    trie->ಮೇಲಿನ_ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ=ದತ್ತಾಂಶ_Move+UTRIE2_ದತ್ತಾಂಶ_ಆರಂಭ_ಸ್ಥಾನಾಂತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UTRIE2_DEBUG
    trie->name="dummy";
#ಆದರೆ_ಅಂತ್ಯ

    /* set the header fields */
    ಶೀರ್ಷಿಕೆ=(UTrie2_ಶೀರ್ಷಿಕೆ *)trie->ಸ್ಮರಣೆ;

    ಶೀರ್ಷಿಕೆ->signature=UTRIE2_SIG; /* "Tri2" */
    ಶೀರ್ಷಿಕೆ->options=(uint16_t)ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು;

    ಶೀರ್ಷಿಕೆ->ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=(uint16_t)ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ಶೀರ್ಷಿಕೆ->shifted_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ=(uint16_t)(ದತ್ತಾಂಶ_ಉದ್ದಳತೆ>>UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು);
    ಶೀರ್ಷಿಕೆ->index2_Null_ಸ್ಥಾನಾಂತರ=(uint16_t)UTRIE2_ಸೂಚ್ಯಂಕ_2_ಸ್ಥಾನಾಂತರ;
    ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ=(uint16_t)ದತ್ತಾಂಶ_Move;
    ಶೀರ್ಷಿಕೆ->shifted_ಮೇಲಿನ_ಆರಂಭ=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(ಶೀರ್ಷಿಕೆ+1);
    trie->ಸೂಚ್ಯಂಕ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    ಗಾಗಿ(i=0; i<UTRIE2_ಸೂಚ್ಯಂಕ_2_BMP_ಉದ್ದಳತೆ; ++i) {
        *dest16++=(uint16_t)(ದತ್ತಾಂಶ_Move>>UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    ಗಾಗಿ(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(ದತ್ತಾಂಶ_Move+UTRIE2_BAD_UTF8_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ);
    }
    ಗಾಗಿ(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)ದತ್ತಾಂಶ_Move;
    }

    /* write the 16/32-bit data array */
    ಆಯ್ಕೆ(ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು) {
    ಸ್ಥಿತಿ UTRIE2_16_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=ಶೂನ್ಯಸೂಚಕ;
        ಗಾಗಿ(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_ಮೌಲ್ಯ;
        }
        ಗಾಗಿ(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_ಮೌಲ್ಯ;
        }
        /* highValue and reserved values */
        ಗಾಗಿ(i=0; i<UTRIE2_ದತ್ತಾಂಶ_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_ಮೌಲ್ಯ;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ UTRIE2_32_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=ಶೂನ್ಯಸೂಚಕ;
        trie->data32=p;
        ಗಾಗಿ(i=0; i<0x80; ++i) {
            *p++=initial_ಮೌಲ್ಯ;
        }
        ಗಾಗಿ(; i<0xc0; ++i) {
            *p++=error_ಮೌಲ್ಯ;
        }
        /* highValue and reserved values */
        ಗಾಗಿ(i=0; i<UTRIE2_ದತ್ತಾಂಶ_GRANULARITY; ++i) {
            *p++=initial_ಮೌಲ್ಯ;
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ trie;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_close(UTrie2 *trie) {
    ಆದರೆ(trie!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(trie->is_ಸ್ಮರಣೆ_Owned) {
            uprv_free(trie->ಸ್ಮರಣೆ);
        }
        ಆದರೆ(trie->newTrie!=ಶೂನ್ಯಸೂಚಕ) {
            uprv_free(trie->newTrie->ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#ಆದರೆ_ಅಂತ್ಯ
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(ಸ್ಥಿರಾಂಕ UTrie2 *trie) {
    ಹಿಂತಿರುಗಿ trie->newTrie==ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(ಸ್ಥಿರಾಂಕ UTrie2 *trie,
                 ಶೂನ್ಯ *ದತ್ತಾಂಶ, int32_t capacity,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /* argument check */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ( trie==ಶೂನ್ಯಸೂಚಕ || trie->ಸ್ಮರಣೆ==ಶೂನ್ಯಸೂಚಕ || trie->newTrie!=ಶೂನ್ಯಸೂಚಕ ||
        capacity<0 || (capacity>0 && (ದತ್ತಾಂಶ==ಶೂನ್ಯಸೂಚಕ || (U_ಸೂಚಕ_ಮುಖವಾಡ_LSB(ದತ್ತಾಂಶ, 3)!=0)))
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(capacity>=trie->length) {
        uprv_memcpy(ದತ್ತಾಂಶ, trie->ಸ್ಮರಣೆ, trie->length);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
    }
    ಹಿಂತಿರುಗಿ trie->length;
}

/* enumeration -------------------------------------------------------------- */

#ವ್ಯಾಖ್ಯಾನಿಸು MIN_ಮೌಲ್ಯ(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
ಸ್ಥಿರ uint32_t U_CALLCONV
enum_Same_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * /*context*/, uint32_t value) {
    ಹಿಂತಿರುಗಿ value;
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
enumEitherTrie(ಸ್ಥಿರಾಂಕ UTrie2 *trie,
               UChar32 ಆರಂಭ, UChar32 ಎಲ್ಲೆ,
               UTrie2_Enum_ಮೌಲ್ಯ *enum_ಮೌಲ್ಯ, UTrie2EnumRange *enumRange, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context) {
    ಸ್ಥಿರಾಂಕ uint32_t *data32;
    ಸ್ಥಿರಾಂಕ uint16_t *idx;

    uint32_t value, prev_ಮೌಲ್ಯ, initial_ಮೌಲ್ಯ;
    UChar32 c, prev, ಮೇಲಿನ_ಆರಂಭ;
    int32_t j, i2_ಖಂಡ, prev_I2_ಖಂಡ, index2_Null_ಸ್ಥಾನಾಂತರ, ಖಂಡ, prev_ಖಂಡ, null_ಖಂಡ;

    ಆದರೆ(enumRange==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(enum_ಮೌಲ್ಯ==ಶೂನ್ಯಸೂಚಕ) {
        enum_ಮೌಲ್ಯ=enum_Same_ಮೌಲ್ಯ;
    }

    ಆದರೆ(trie->newTrie==ಶೂನ್ಯಸೂಚಕ) {
        /* frozen trie */
        idx=trie->ಸೂಚ್ಯಂಕ;
        U_ASSERT(idx!=ಶೂನ್ಯಸೂಚಕ); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2_Null_ಸ್ಥಾನಾಂತರ=trie->index2_Null_ಸ್ಥಾನಾಂತರ;
        null_ಖಂಡ=trie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* unfrozen, mutable trie */
        idx=ಶೂನ್ಯಸೂಚಕ;
        data32=trie->newTrie->ದತ್ತಾಂಶ;
        U_ASSERT(data32!=ಶೂನ್ಯಸೂಚಕ); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2_Null_ಸ್ಥಾನಾಂತರ=trie->newTrie->index2_Null_ಸ್ಥಾನಾಂತರ;
        null_ಖಂಡ=trie->newTrie->ದತ್ತಾಂಶ_Null_ಸ್ಥಾನಾಂತರ;
    }

    ಮೇಲಿನ_ಆರಂಭ=trie->ಮೇಲಿನ_ಆರಂಭ;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_ಮೌಲ್ಯ=enum_ಮೌಲ್ಯ(context, trie->initial_ಮೌಲ್ಯ);

    /* set variables for previous range */
    prev_I2_ಖಂಡ=-1;
    prev_ಖಂಡ=-1;
    prev=ಆರಂಭ;
    prev_ಮೌಲ್ಯ=0;

    /* enumerate index-2 blocks */
    ಗಾಗಿ(c=ಆರಂಭ; c<ಎಲ್ಲೆ && c<ಮೇಲಿನ_ಆರಂಭ;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 ತಾತ್ಕಾಲಿಕ_ಎಲ್ಲೆ=c+UTRIE2_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_1_ನಮೂದು;
        ಆದರೆ(ಎಲ್ಲೆ<ತಾತ್ಕಾಲಿಕ_ಎಲ್ಲೆ) {
            ತಾತ್ಕಾಲಿಕ_ಎಲ್ಲೆ=ಎಲ್ಲೆ;
        }
        ಆದರೆ(c<=0xffff) {
            ಆದರೆ(!U_IS_SURROGATE(c)) {
                i2_ಖಂಡ=c>>UTRIE2_ಸರಿಸು_2;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2_ಖಂಡ=UTRIE2_LSCP_ಸೂಚ್ಯಂಕ_2_ಸ್ಥಾನಾಂತರ;
                ತಾತ್ಕಾಲಿಕ_ಎಲ್ಲೆ=MIN_ಮೌಲ್ಯ(0xdc00, ಎಲ್ಲೆ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2_ಖಂಡ=0xd800>>UTRIE2_ಸರಿಸು_2;
                ತಾತ್ಕಾಲಿಕ_ಎಲ್ಲೆ=MIN_ಮೌಲ್ಯ(0xe000, ಎಲ್ಲೆ);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* supplementary code points */
            ಆದರೆ(idx!=ಶೂನ್ಯಸೂಚಕ) {
                i2_ಖಂಡ=idx[(UTRIE2_ಸೂಚ್ಯಂಕ_1_ಸ್ಥಾನಾಂತರ-UTRIE2_OMITTED_BMP_ಸೂಚ್ಯಂಕ_1_ಉದ್ದಳತೆ)+
                              (c>>UTRIE2_ಸರಿಸು_1)];
            } ಇಲ್ಲದಿದ್ದರೆ {
                i2_ಖಂಡ=trie->newTrie->index1[c>>UTRIE2_ಸರಿಸು_1];
            }
            ಆದರೆ(i2_ಖಂಡ==prev_I2_ಖಂಡ && (c-prev)>=UTRIE2_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_1_ನಮೂದು) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_1_ನಮೂದು;
                ಮುಂದುವರಿ;
            }
        }
        prev_I2_ಖಂಡ=i2_ಖಂಡ;
        ಆದರೆ(i2_ಖಂಡ==index2_Null_ಸ್ಥಾನಾಂತರ) {
            /* this is the null index-2 block */
            ಆದರೆ(prev_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                ಆದರೆ(prev<c && !enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ)) {
                    ಹಿಂತಿರುಗಿ;
                }
                prev_ಖಂಡ=null_ಖಂಡ;
                prev=c;
                prev_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
            }
            c+=UTRIE2_CP_ಪ್ರತಿ_ಸೂಚ್ಯಂಕ_1_ನಮೂದು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2_ಎಲ್ಲೆ;
            i2=(c>>UTRIE2_ಸರಿಸು_2)&UTRIE2_ಸೂಚ್ಯಂಕ_2_ಮುಖವಾಡ;
            ಆದರೆ((c>>UTRIE2_ಸರಿಸು_1)==(ತಾತ್ಕಾಲಿಕ_ಎಲ್ಲೆ>>UTRIE2_ಸರಿಸು_1)) {
                i2_ಎಲ್ಲೆ=(ತಾತ್ಕಾಲಿಕ_ಎಲ್ಲೆ>>UTRIE2_ಸರಿಸು_2)&UTRIE2_ಸೂಚ್ಯಂಕ_2_ಮುಖವಾಡ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                i2_ಎಲ್ಲೆ=UTRIE2_ಸೂಚ್ಯಂಕ_2_ಖಂಡ_ಉದ್ದಳತೆ;
            }
            ಗಾಗಿ(; i2<i2_ಎಲ್ಲೆ; ++i2) {
                ಆದರೆ(idx!=ಶೂನ್ಯಸೂಚಕ) {
                    ಖಂಡ = static_cast<int32_t>(idx[i2_ಖಂಡ + i2]) << UTRIE2_ಸೂಚ್ಯಂಕ_ಸರಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಖಂಡ=trie->newTrie->index2[i2_ಖಂಡ+i2];
                }
                ಆದರೆ(ಖಂಡ==prev_ಖಂಡ && (c-prev)>=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
                    ಮುಂದುವರಿ;
                }
                prev_ಖಂಡ=ಖಂಡ;
                ಆದರೆ(ಖಂಡ==null_ಖಂಡ) {
                    /* this is the null data block */
                    ಆದರೆ(prev_ಮೌಲ್ಯ!=initial_ಮೌಲ್ಯ) {
                        ಆದರೆ(prev<c && !enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ)) {
                            ಹಿಂತಿರುಗಿ;
                        }
                        prev=c;
                        prev_ಮೌಲ್ಯ=initial_ಮೌಲ್ಯ;
                    }
                    c+=UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಗಾಗಿ(j=0; j<UTRIE2_ದತ್ತಾಂಶ_ಖಂಡ_ಉದ್ದಳತೆ; ++j) {
                        value=enum_ಮೌಲ್ಯ(context, data32!=ಶೂನ್ಯಸೂಚಕ ? data32[ಖಂಡ+j] : idx[ಖಂಡ+j]);
                        ಆದರೆ(value!=prev_ಮೌಲ್ಯ) {
                            ಆದರೆ(prev<c && !enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ)) {
                                ಹಿಂತಿರುಗಿ;
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

    ಆದರೆ(c>ಎಲ್ಲೆ) {
        c=ಎಲ್ಲೆ;  /* could be higher if in the index2NullOffset */
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<ಎಲ್ಲೆ) {
        /* c==highStart<limit */
        uint32_t ಮೇಲಿನ_ಮೌಲ್ಯ;
        ಆದರೆ(idx!=ಶೂನ್ಯಸೂಚಕ) {
            ಮೇಲಿನ_ಮೌಲ್ಯ=
                data32!=ಶೂನ್ಯಸೂಚಕ ?
                    data32[trie->ಮೇಲಿನ_ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ] :
                    idx[trie->ಮೇಲಿನ_ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ];
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಮೇಲಿನ_ಮೌಲ್ಯ=trie->newTrie->ದತ್ತಾಂಶ[trie->newTrie->ದತ್ತಾಂಶ_ಉದ್ದಳತೆ-UTRIE2_ದತ್ತಾಂಶ_GRANULARITY];
        }
        value=enum_ಮೌಲ್ಯ(context, ಮೇಲಿನ_ಮೌಲ್ಯ);
        ಆದರೆ(value!=prev_ಮೌಲ್ಯ) {
            ಆದರೆ(prev<c && !enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ)) {
                ಹಿಂತಿರುಗಿ;
            }
            prev=c;
            prev_ಮೌಲ್ಯ=value;
        }
        c=ಎಲ್ಲೆ;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_ಮೌಲ್ಯ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_enum(ಸ್ಥಿರಾಂಕ UTrie2 *trie,
            UTrie2_Enum_ಮೌಲ್ಯ *enum_ಮೌಲ್ಯ, UTrie2EnumRange *enumRange, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_ಮೌಲ್ಯ, enumRange, context);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrie2_enumForLeadSurrogate(ಸ್ಥಿರಾಂಕ UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_ಮೌಲ್ಯ *enum_ಮೌಲ್ಯ, UTrie2EnumRange *enumRange,
                            ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context) {
    ಆದರೆ(!U16_IS_LEAD(lead)) {
        ಹಿಂತಿರುಗಿ;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_ಮೌಲ್ಯ, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ=ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ;
    ಆದರೆ(ಆರಂಭ>=ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ) {
        ಸಂಕೇತ_ಬಿಂದು=U_SENTINEL;
        ಹಿಂತಿರುಗಿ static_cast<uint16_t>(trie->error_ಮೌಲ್ಯ);
    }
    uint16_t ಫಲಿತಾಂಶ;
    UTRIE2_U16_PREV16(trie, ಆರಂಭ, ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ, ಸಂಕೇತ_ಬಿಂದು, ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    ಸಂಕೇತ_ಬಿಂದು_ಆರಂಭ=ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ;
    ಆದರೆ(ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ==ಎಲ್ಲೆ) {
        ಸಂಕೇತ_ಬಿಂದು=U_SENTINEL;
        ಹಿಂತಿರುಗಿ static_cast<uint16_t>(trie->error_ಮೌಲ್ಯ);
    }
    uint16_t ಫಲಿತಾಂಶ;
    UTRIE2_U16_NEXT16(trie, ಸಂಕೇತ_ಬಿಂದು_ಎಲ್ಲೆ, ಎಲ್ಲೆ, ಸಂಕೇತ_ಬಿಂದು, ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_NAMESPACE_ಕೊನೆ

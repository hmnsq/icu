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
#అన్తర్భావ "unicode/utypes.h"
#నిర్వచితమైతే UCPTRIE_DEBUG
#అన్తర్భావ "unicode/umutablecptrie.h"
#అయితే_ముగింపు
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf8.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "utrie2.h"
#అన్తర్భావ "utrie2_impl.h"
#అన్తర్భావ "uassert.h"

/* Public UTrie2 API implementation ----------------------------------------- */

స్థిర uint32_t
get32(స్థిరరాశి UNewTrie2 *trie, UChar32 c, UBool fromLSCP) {
    int32_t i2, ఖండం;

    ఒకవేళ(c>=trie->ఎగువ_ప్రారంభం && (!U_IS_LEAD(c) || fromLSCP)) {
        తిరిగి trie->దత్తాంశం[trie->దత్తాంశం_నిడివి-UTRIE2_దత్తాంశం_GRANULARITY];
    }

    ఒకవేళ(U_IS_LEAD(c) && fromLSCP) {
        i2=(UTRIE2_LSCP_సూచీ_2_స్థానభ్రంశం-(0xd800>>UTRIE2_జరుపు_2))+
            (c>>UTRIE2_జరుపు_2);
    } లేకపోతే {
        i2=trie->index1[c>>UTRIE2_జరుపు_1]+
            ((c>>UTRIE2_జరుపు_2)&UTRIE2_సూచీ_2_ముసుగు);
    }
    ఖండం=trie->index2[i2];
    తిరిగి trie->దత్తాంశం[ఖండం+(c&UTRIE2_దత్తాంశం_ముసుగు)];
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32(స్థిరరాశి UTrie2 *trie, UChar32 c) {
    ఒకవేళ(trie->data16!=శూన్యసూచిక) {
        తిరిగి UTRIE2_GET16(trie, c);
    } లేకపోతే ఒకవేళ(trie->data32!=శూన్యసూచిక) {
        తిరిగి UTRIE2_GET32(trie, c);
    } లేకపోతే ఒకవేళ((uint32_t)c>0x10ffff) {
        తిరిగి trie->error_విలువ;
    } లేకపోతే {
        తిరిగి get32(trie->newTrie, c, నిజం);
    }
}

U_CAPI uint32_t U_EXPORT2
utrie2_get32_From_Lead_Surrogate_సంకేతం_Unit(స్థిరరాశి UTrie2 *trie, UChar32 c) {
    ఒకవేళ(!U_IS_LEAD(c)) {
        తిరిగి trie->error_విలువ;
    }
    ఒకవేళ(trie->data16!=శూన్యసూచిక) {
        తిరిగి UTRIE2_GET16_FROM_U16_SINGLE_LEAD(trie, c);
    } లేకపోతే ఒకవేళ(trie->data32!=శూన్యసూచిక) {
        తిరిగి UTRIE2_GET32_FROM_U16_SINGLE_LEAD(trie, c);
    } లేకపోతే {
        తిరిగి get32(trie->newTrie, c, అబద్ధం);
    }
}

స్థిర అంతఃవరుస int32_t
u8_సూచీ(స్థిరరాశి UTrie2 *trie, UChar32 c, int32_t i) {
    int32_t idx=
        UTRIE2_సూచీ_FROM_CP(
            trie,
            trie->data32==శూన్యసూచిక ? trie->సూచీ_నిడివి : 0,
            c);
    తిరిగి (idx<<3)|i;
}

U_CAPI int32_t U_EXPORT2
utrie2_అంతర్గత_U8_తదుపరి_సూచీ(స్థిరరాశి UTrie2 *trie, UChar32 c,
                           స్థిరరాశి uint8_t *src, స్థిరరాశి uint8_t *హద్దు) {
    int32_t i, length;
    i=0;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ఒకవేళ((హద్దు-src)<=7) {
        length=(int32_t)(హద్దు-src);
    } లేకపోతే {
        length=7;
    }
    c=utf8_తదుపరి_Char_సురక్షిత_Body(src, &i, length, c, -1);
    తిరిగి u8_సూచీ(trie, c, i);
}

U_CAPI int32_t U_EXPORT2
utrie2_అంతర్గత_U8_Prev_సూచీ(స్థిరరాశి UTrie2 *trie, UChar32 c,
                           స్థిరరాశి uint8_t *ప్రారంభం, స్థిరరాశి uint8_t *src) {
    int32_t i, length;
    /* support 64-bit pointers by avoiding cast of arbitrary difference */
    ఒకవేళ((src-ప్రారంభం)<=7) {
        i=length=(int32_t)(src-ప్రారంభం);
    } లేకపోతే {
        i=length=7;
        ప్రారంభం=src-7;
    }
    c=utf8_prev_Char_సురక్షిత_Body(ప్రారంభం, 0, &i, c, -1);
    i=length-i;  /* number of bytes read backward from src */
    తిరిగి u8_సూచీ(trie, c, i);
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openFromSerialized(UTrie2_విలువ_బిట్లు విలువ_బిట్లు,
                          స్థిరరాశి శూన్య *దత్తాంశం, int32_t length, int32_t *p_Actual_నిడివి,
                          UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UTrie2_శీర్షిక *శీర్షిక;
    స్థిరరాశి uint16_t *p16;
    int32_t actual_నిడివి;

    UTrie2 తాత్కాలిక_Trie;
    UTrie2 *trie;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ( length<=0 || (U_సూచిక_ముసుగు_LSB(దత్తాంశం, 3)!=0) ||
        విలువ_బిట్లు<0 || UTRIE2_లెక్క_విలువ_బిట్లు<=విలువ_బిట్లు
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    /* enough data for a trie header? */
    ఒకవేళ(length<(int32_t)పరిమాణం(UTrie2_శీర్షిక)) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    /* check the signature */
    శీర్షిక=(స్థిరరాశి UTrie2_శీర్షిక *)దత్తాంశం;
    ఒకవేళ(శీర్షిక->signature!=UTRIE2_SIG) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    /* get the options */
    ఒకవేళ(విలువ_బిట్లు!=(UTrie2_విలువ_బిట్లు)(శీర్షిక->options&UTRIE2_OPTIONS_విలువ_బిట్లు_ముసుగు)) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    /* get the length values and offsets */
    uprv_memset(&తాత్కాలిక_Trie, 0, పరిమాణం(తాత్కాలిక_Trie));
    తాత్కాలిక_Trie.సూచీ_నిడివి=శీర్షిక->సూచీ_నిడివి;
    తాత్కాలిక_Trie.దత్తాంశం_నిడివి=శీర్షిక->shifted_దత్తాంశం_నిడివి<<UTRIE2_సూచీ_జరుపు;
    తాత్కాలిక_Trie.index2_Null_స్థానభ్రంశం=శీర్షిక->index2_Null_స్థానభ్రంశం;
    తాత్కాలిక_Trie.దత్తాంశం_Null_స్థానభ్రంశం=శీర్షిక->దత్తాంశం_Null_స్థానభ్రంశం;

    తాత్కాలిక_Trie.ఎగువ_ప్రారంభం=శీర్షిక->shifted_ఎగువ_ప్రారంభం<<UTRIE2_జరుపు_1;
    తాత్కాలిక_Trie.ఎగువ_విలువ_సూచీ=తాత్కాలిక_Trie.దత్తాంశం_నిడివి-UTRIE2_దత్తాంశం_GRANULARITY;
    ఒకవేళ(విలువ_బిట్లు==UTRIE2_16_విలువ_బిట్లు) {
        తాత్కాలిక_Trie.ఎగువ_విలువ_సూచీ+=తాత్కాలిక_Trie.సూచీ_నిడివి;
    }

    /* calculate the actual length */
    actual_నిడివి=(int32_t)పరిమాణం(UTrie2_శీర్షిక)+తాత్కాలిక_Trie.సూచీ_నిడివి*2;
    ఒకవేళ(విలువ_బిట్లు==UTRIE2_16_విలువ_బిట్లు) {
        actual_నిడివి+=తాత్కాలిక_Trie.దత్తాంశం_నిడివి*2;
    } లేకపోతే {
        actual_నిడివి+=తాత్కాలిక_Trie.దత్తాంశం_నిడివి*4;
    }
    ఒకవేళ(length<actual_నిడివి) {
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;  /* not enough bytes */
        తిరిగి శూన్యసూచిక;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(పరిమాణం(UTrie2));
    ఒకవేళ(trie==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memcpy(trie, &తాత్కాలిక_Trie, పరిమాణం(తాత్కాలిక_Trie));
    trie->జ్ఞాపకం=(uint32_t *)దత్తాంశం;
    trie->length=actual_నిడివి;
    trie->is_జ్ఞాపకం_Owned=అబద్ధం;
#నిర్వచితమైతే UTRIE2_DEBUG
    trie->name="fromSerialized";
#అయితే_ముగింపు

    /* set the pointers to its index and data arrays */
    p16=(స్థిరరాశి uint16_t *)(శీర్షిక+1);
    trie->సూచీ=p16;
    p16+=trie->సూచీ_నిడివి;

    /* get the data */
    ఎంపిక(విలువ_బిట్లు) {
    స్థితి UTRIE2_16_విలువ_బిట్లు:
        trie->data16=p16;
        trie->data32=శూన్యసూచిక;
        trie->initial_విలువ=trie->సూచీ[trie->దత్తాంశం_Null_స్థానభ్రంశం];
        trie->error_విలువ=trie->data16[UTRIE2_BAD_UTF8_దత్తాంశం_స్థానభ్రంశం];
        ఆపు;
    స్థితి UTRIE2_32_విలువ_బిట్లు:
        trie->data16=శూన్యసూచిక;
        trie->data32=(స్థిరరాశి uint32_t *)p16;
        trie->initial_విలువ=trie->data32[trie->దత్తాంశం_Null_స్థానభ్రంశం];
        trie->error_విలువ=trie->data32[UTRIE2_BAD_UTF8_దత్తాంశం_స్థానభ్రంశం];
        ఆపు;
    అప్రమేయ:
        *p_Error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(p_Actual_నిడివి!=శూన్యసూచిక) {
        *p_Actual_నిడివి=actual_నిడివి;
    }
    తిరిగి trie;
}

U_CAPI UTrie2 * U_EXPORT2
utrie2_openDummy(UTrie2_విలువ_బిట్లు విలువ_బిట్లు,
                 uint32_t initial_విలువ, uint32_t error_విలువ,
                 UError_సంకేతం *p_Error_సంకేతం) {
    UTrie2 *trie;
    UTrie2_శీర్షిక *శీర్షిక;
    uint32_t *p;
    uint16_t *dest16;
    int32_t సూచీ_నిడివి, దత్తాంశం_నిడివి, length, i;
    int32_t దత్తాంశం_Move;  /* >0 if the data is moved to the end of the index array */

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ(విలువ_బిట్లు<0 || UTRIE2_లెక్క_విలువ_బిట్లు<=విలువ_బిట్లు) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    /* calculate the total length of the dummy trie data */
    సూచీ_నిడివి=UTRIE2_సూచీ_1_స్థానభ్రంశం;
    దత్తాంశం_నిడివి=UTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం+UTRIE2_దత్తాంశం_GRANULARITY;
    length=(int32_t)పరిమాణం(UTrie2_శీర్షిక)+సూచీ_నిడివి*2;
    ఒకవేళ(విలువ_బిట్లు==UTRIE2_16_విలువ_బిట్లు) {
        length+=దత్తాంశం_నిడివి*2;
    } లేకపోతే {
        length+=దత్తాంశం_నిడివి*4;
    }

    /* allocate the trie */
    trie=(UTrie2 *)uprv_malloc(పరిమాణం(UTrie2));
    ఒకవేళ(trie==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    uprv_memset(trie, 0, పరిమాణం(UTrie2));
    trie->జ్ఞాపకం=uprv_malloc(length);
    ఒకవేళ(trie->జ్ఞాపకం==శూన్యసూచిక) {
        uprv_free(trie);
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    trie->length=length;
    trie->is_జ్ఞాపకం_Owned=నిజం;

    /* set the UTrie2 fields */
    ఒకవేళ(విలువ_బిట్లు==UTRIE2_16_విలువ_బిట్లు) {
        దత్తాంశం_Move=సూచీ_నిడివి;
    } లేకపోతే {
        దత్తాంశం_Move=0;
    }

    trie->సూచీ_నిడివి=సూచీ_నిడివి;
    trie->దత్తాంశం_నిడివి=దత్తాంశం_నిడివి;
    trie->index2_Null_స్థానభ్రంశం=UTRIE2_సూచీ_2_స్థానభ్రంశం;
    trie->దత్తాంశం_Null_స్థానభ్రంశం=(uint16_t)దత్తాంశం_Move;
    trie->initial_విలువ=initial_విలువ;
    trie->error_విలువ=error_విలువ;
    trie->ఎగువ_ప్రారంభం=0;
    trie->ఎగువ_విలువ_సూచీ=దత్తాంశం_Move+UTRIE2_దత్తాంశం_ప్రారంభం_స్థానభ్రంశం;
#నిర్వచితమైతే UTRIE2_DEBUG
    trie->name="dummy";
#అయితే_ముగింపు

    /* set the header fields */
    శీర్షిక=(UTrie2_శీర్షిక *)trie->జ్ఞాపకం;

    శీర్షిక->signature=UTRIE2_SIG; /* "Tri2" */
    శీర్షిక->options=(uint16_t)విలువ_బిట్లు;

    శీర్షిక->సూచీ_నిడివి=(uint16_t)సూచీ_నిడివి;
    శీర్షిక->shifted_దత్తాంశం_నిడివి=(uint16_t)(దత్తాంశం_నిడివి>>UTRIE2_సూచీ_జరుపు);
    శీర్షిక->index2_Null_స్థానభ్రంశం=(uint16_t)UTRIE2_సూచీ_2_స్థానభ్రంశం;
    శీర్షిక->దత్తాంశం_Null_స్థానభ్రంశం=(uint16_t)దత్తాంశం_Move;
    శీర్షిక->shifted_ఎగువ_ప్రారంభం=0;

    /* fill the index and data arrays */
    dest16=(uint16_t *)(శీర్షిక+1);
    trie->సూచీ=dest16;

    /* write the index-2 array values shifted right by UTRIE2_INDEX_SHIFT */
    కొరకు(i=0; i<UTRIE2_సూచీ_2_BMP_నిడివి; ++i) {
        *dest16++=(uint16_t)(దత్తాంశం_Move>>UTRIE2_సూచీ_జరుపు);  /* null data block */
    }

    /* write UTF-8 2-byte index-2 values, not right-shifted */
    కొరకు(i=0; i<(0xc2-0xc0); ++i) {                                  /* C0..C1 */
        *dest16++=(uint16_t)(దత్తాంశం_Move+UTRIE2_BAD_UTF8_దత్తాంశం_స్థానభ్రంశం);
    }
    కొరకు(; i<(0xe0-0xc0); ++i) {                                     /* C2..DF */
        *dest16++=(uint16_t)దత్తాంశం_Move;
    }

    /* write the 16/32-bit data array */
    ఎంపిక(విలువ_బిట్లు) {
    స్థితి UTRIE2_16_విలువ_బిట్లు:
        /* write 16-bit data values */
        trie->data16=dest16;
        trie->data32=శూన్యసూచిక;
        కొరకు(i=0; i<0x80; ++i) {
            *dest16++=(uint16_t)initial_విలువ;
        }
        కొరకు(; i<0xc0; ++i) {
            *dest16++=(uint16_t)error_విలువ;
        }
        /* highValue and reserved values */
        కొరకు(i=0; i<UTRIE2_దత్తాంశం_GRANULARITY; ++i) {
            *dest16++=(uint16_t)initial_విలువ;
        }
        ఆపు;
    స్థితి UTRIE2_32_విలువ_బిట్లు:
        /* write 32-bit data values */
        p=(uint32_t *)dest16;
        trie->data16=శూన్యసూచిక;
        trie->data32=p;
        కొరకు(i=0; i<0x80; ++i) {
            *p++=initial_విలువ;
        }
        కొరకు(; i<0xc0; ++i) {
            *p++=error_విలువ;
        }
        /* highValue and reserved values */
        కొరకు(i=0; i<UTRIE2_దత్తాంశం_GRANULARITY; ++i) {
            *p++=initial_విలువ;
        }
        ఆపు;
    అప్రమేయ:
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    తిరిగి trie;
}

U_CAPI శూన్య U_EXPORT2
utrie2_close(UTrie2 *trie) {
    ఒకవేళ(trie!=శూన్యసూచిక) {
        ఒకవేళ(trie->is_జ్ఞాపకం_Owned) {
            uprv_free(trie->జ్ఞాపకం);
        }
        ఒకవేళ(trie->newTrie!=శూన్యసూచిక) {
            uprv_free(trie->newTrie->దత్తాంశం);
#నిర్వచితమైతే UCPTRIE_DEBUG
            umutablecptrie_close(trie->newTrie->t3);
#అయితే_ముగింపు
            uprv_free(trie->newTrie);
        }
        uprv_free(trie);
    }
}

U_CAPI UBool U_EXPORT2
utrie2_isFrozen(స్థిరరాశి UTrie2 *trie) {
    తిరిగి trie->newTrie==శూన్యసూచిక;
}

U_CAPI int32_t U_EXPORT2
utrie2_serialize(స్థిరరాశి UTrie2 *trie,
                 శూన్య *దత్తాంశం, int32_t capacity,
                 UError_సంకేతం *p_Error_సంకేతం) {
    /* argument check */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    ఒకవేళ( trie==శూన్యసూచిక || trie->జ్ఞాపకం==శూన్యసూచిక || trie->newTrie!=శూన్యసూచిక ||
        capacity<0 || (capacity>0 && (దత్తాంశం==శూన్యసూచిక || (U_సూచిక_ముసుగు_LSB(దత్తాంశం, 3)!=0)))
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(capacity>=trie->length) {
        uprv_memcpy(దత్తాంశం, trie->జ్ఞాపకం, trie->length);
    } లేకపోతే {
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
    }
    తిరిగి trie->length;
}

/* enumeration -------------------------------------------------------------- */

#నిర్వచించు MIN_విలువ(a, b) ((a)<(b) ? (a) : (b))

/* default UTrie2EnumValue() returns the input value itself */
స్థిర uint32_t U_CALLCONV
enum_Same_విలువ(స్థిరరాశి శూన్య * /*context*/, uint32_t value) {
    తిరిగి value;
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
స్థిర శూన్య
enumEitherTrie(స్థిరరాశి UTrie2 *trie,
               UChar32 ప్రారంభం, UChar32 హద్దు,
               UTrie2_Enum_విలువ *enum_విలువ, UTrie2EnumRange *enumRange, స్థిరరాశి శూన్య *context) {
    స్థిరరాశి uint32_t *data32;
    స్థిరరాశి uint16_t *idx;

    uint32_t value, prev_విలువ, initial_విలువ;
    UChar32 c, prev, ఎగువ_ప్రారంభం;
    int32_t j, i2_ఖండం, prev_I2_ఖండం, index2_Null_స్థానభ్రంశం, ఖండం, prev_ఖండం, null_ఖండం;

    ఒకవేళ(enumRange==శూన్యసూచిక) {
        తిరిగి;
    }
    ఒకవేళ(enum_విలువ==శూన్యసూచిక) {
        enum_విలువ=enum_Same_విలువ;
    }

    ఒకవేళ(trie->newTrie==శూన్యసూచిక) {
        /* frozen trie */
        idx=trie->సూచీ;
        U_ASSERT(idx!=శూన్యసూచిక); /* the following code assumes trie->newTrie is not nullptr when idx is nullptr */
        data32=trie->data32;

        index2_Null_స్థానభ్రంశం=trie->index2_Null_స్థానభ్రంశం;
        null_ఖండం=trie->దత్తాంశం_Null_స్థానభ్రంశం;
    } లేకపోతే {
        /* unfrozen, mutable trie */
        idx=శూన్యసూచిక;
        data32=trie->newTrie->దత్తాంశం;
        U_ASSERT(data32!=శూన్యసూచిక); /* the following code assumes idx is not nullptr when data32 is nullptr */

        index2_Null_స్థానభ్రంశం=trie->newTrie->index2_Null_స్థానభ్రంశం;
        null_ఖండం=trie->newTrie->దత్తాంశం_Null_స్థానభ్రంశం;
    }

    ఎగువ_ప్రారంభం=trie->ఎగువ_ప్రారంభం;

    /* get the enumeration value that corresponds to an initial-value trie data entry */
    initial_విలువ=enum_విలువ(context, trie->initial_విలువ);

    /* set variables for previous range */
    prev_I2_ఖండం=-1;
    prev_ఖండం=-1;
    prev=ప్రారంభం;
    prev_విలువ=0;

    /* enumerate index-2 blocks */
    కొరకు(c=ప్రారంభం; c<హద్దు && c<ఎగువ_ప్రారంభం;) {
        /* Code point limit for iterating inside this i2Block. */
        UChar32 తాత్కాలిక_హద్దు=c+UTRIE2_CP_చొప్పున_సూచీ_1_నమోదు;
        ఒకవేళ(హద్దు<తాత్కాలిక_హద్దు) {
            తాత్కాలిక_హద్దు=హద్దు;
        }
        ఒకవేళ(c<=0xffff) {
            ఒకవేళ(!U_IS_SURROGATE(c)) {
                i2_ఖండం=c>>UTRIE2_జరుపు_2;
            } లేకపోతే ఒకవేళ(U_IS_SURROGATE_LEAD(c)) {
                /*
                 * Enumerate values for lead surrogate code points, not code units:
                 * This special block has half the normal length.
                 */
                i2_ఖండం=UTRIE2_LSCP_సూచీ_2_స్థానభ్రంశం;
                తాత్కాలిక_హద్దు=MIN_విలువ(0xdc00, హద్దు);
            } లేకపోతే {
                /*
                 * Switch back to the normal part of the index-2 table.
                 * Enumerate the second half of the surrogates block.
                 */
                i2_ఖండం=0xd800>>UTRIE2_జరుపు_2;
                తాత్కాలిక_హద్దు=MIN_విలువ(0xe000, హద్దు);
            }
        } లేకపోతే {
            /* supplementary code points */
            ఒకవేళ(idx!=శూన్యసూచిక) {
                i2_ఖండం=idx[(UTRIE2_సూచీ_1_స్థానభ్రంశం-UTRIE2_OMITTED_BMP_సూచీ_1_నిడివి)+
                              (c>>UTRIE2_జరుపు_1)];
            } లేకపోతే {
                i2_ఖండం=trie->newTrie->index1[c>>UTRIE2_జరుపు_1];
            }
            ఒకవేళ(i2_ఖండం==prev_I2_ఖండం && (c-prev)>=UTRIE2_CP_చొప్పున_సూచీ_1_నమోదు) {
                /*
                 * The index-2 block is the same as the previous one, and filled with prevValue.
                 * Only possible for supplementary code points because the linear-BMP index-2
                 * table creates unique i2Block values.
                 */
                c+=UTRIE2_CP_చొప్పున_సూచీ_1_నమోదు;
                కొనసాగు;
            }
        }
        prev_I2_ఖండం=i2_ఖండం;
        ఒకవేళ(i2_ఖండం==index2_Null_స్థానభ్రంశం) {
            /* this is the null index-2 block */
            ఒకవేళ(prev_విలువ!=initial_విలువ) {
                ఒకవేళ(prev<c && !enumRange(context, prev, c-1, prev_విలువ)) {
                    తిరిగి;
                }
                prev_ఖండం=null_ఖండం;
                prev=c;
                prev_విలువ=initial_విలువ;
            }
            c+=UTRIE2_CP_చొప్పున_సూచీ_1_నమోదు;
        } లేకపోతే {
            /* enumerate data blocks for one index-2 block */
            int32_t i2, i2_హద్దు;
            i2=(c>>UTRIE2_జరుపు_2)&UTRIE2_సూచీ_2_ముసుగు;
            ఒకవేళ((c>>UTRIE2_జరుపు_1)==(తాత్కాలిక_హద్దు>>UTRIE2_జరుపు_1)) {
                i2_హద్దు=(తాత్కాలిక_హద్దు>>UTRIE2_జరుపు_2)&UTRIE2_సూచీ_2_ముసుగు;
            } లేకపోతే {
                i2_హద్దు=UTRIE2_సూచీ_2_ఖండం_నిడివి;
            }
            కొరకు(; i2<i2_హద్దు; ++i2) {
                ఒకవేళ(idx!=శూన్యసూచిక) {
                    ఖండం = static_cast<int32_t>(idx[i2_ఖండం + i2]) << UTRIE2_సూచీ_జరుపు;
                } లేకపోతే {
                    ఖండం=trie->newTrie->index2[i2_ఖండం+i2];
                }
                ఒకవేళ(ఖండం==prev_ఖండం && (c-prev)>=UTRIE2_దత్తాంశం_ఖండం_నిడివి) {
                    /* the block is the same as the previous one, and filled with prevValue */
                    c+=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
                    కొనసాగు;
                }
                prev_ఖండం=ఖండం;
                ఒకవేళ(ఖండం==null_ఖండం) {
                    /* this is the null data block */
                    ఒకవేళ(prev_విలువ!=initial_విలువ) {
                        ఒకవేళ(prev<c && !enumRange(context, prev, c-1, prev_విలువ)) {
                            తిరిగి;
                        }
                        prev=c;
                        prev_విలువ=initial_విలువ;
                    }
                    c+=UTRIE2_దత్తాంశం_ఖండం_నిడివి;
                } లేకపోతే {
                    కొరకు(j=0; j<UTRIE2_దత్తాంశం_ఖండం_నిడివి; ++j) {
                        value=enum_విలువ(context, data32!=శూన్యసూచిక ? data32[ఖండం+j] : idx[ఖండం+j]);
                        ఒకవేళ(value!=prev_విలువ) {
                            ఒకవేళ(prev<c && !enumRange(context, prev, c-1, prev_విలువ)) {
                                తిరిగి;
                            }
                            prev=c;
                            prev_విలువ=value;
                        }
                        ++c;
                    }
                }
            }
        }
    }

    ఒకవేళ(c>హద్దు) {
        c=హద్దు;  /* could be higher if in the index2NullOffset */
    } లేకపోతే ఒకవేళ(c<హద్దు) {
        /* c==highStart<limit */
        uint32_t ఎగువ_విలువ;
        ఒకవేళ(idx!=శూన్యసూచిక) {
            ఎగువ_విలువ=
                data32!=శూన్యసూచిక ?
                    data32[trie->ఎగువ_విలువ_సూచీ] :
                    idx[trie->ఎగువ_విలువ_సూచీ];
        } లేకపోతే {
            ఎగువ_విలువ=trie->newTrie->దత్తాంశం[trie->newTrie->దత్తాంశం_నిడివి-UTRIE2_దత్తాంశం_GRANULARITY];
        }
        value=enum_విలువ(context, ఎగువ_విలువ);
        ఒకవేళ(value!=prev_విలువ) {
            ఒకవేళ(prev<c && !enumRange(context, prev, c-1, prev_విలువ)) {
                తిరిగి;
            }
            prev=c;
            prev_విలువ=value;
        }
        c=హద్దు;
    }

    /* deliver last range */
    enumRange(context, prev, c-1, prev_విలువ);
}

U_CAPI శూన్య U_EXPORT2
utrie2_enum(స్థిరరాశి UTrie2 *trie,
            UTrie2_Enum_విలువ *enum_విలువ, UTrie2EnumRange *enumRange, స్థిరరాశి శూన్య *context) {
    enumEitherTrie(trie, 0, 0x110000, enum_విలువ, enumRange, context);
}

U_CAPI శూన్య U_EXPORT2
utrie2_enumForLeadSurrogate(స్థిరరాశి UTrie2 *trie, UChar32 lead,
                            UTrie2_Enum_విలువ *enum_విలువ, UTrie2EnumRange *enumRange,
                            స్థిరరాశి శూన్య *context) {
    ఒకవేళ(!U16_IS_LEAD(lead)) {
        తిరిగి;
    }
    lead=(lead-0xd7c0)<<10;   /* start code point */
    enumEitherTrie(trie, lead, lead+0x400, enum_విలువ, enumRange, context);
}

/* C++ convenience wrappers ------------------------------------------------- */

U_NAMESPACE_BEGIN

uint16_t BackwardUTrie2StringIterator::previous16() {
    సంకేతం_బిందువు_హద్దు=సంకేతం_బిందువు_ప్రారంభం;
    ఒకవేళ(ప్రారంభం>=సంకేతం_బిందువు_ప్రారంభం) {
        సంకేతం_బిందువు=U_SENTINEL;
        తిరిగి static_cast<uint16_t>(trie->error_విలువ);
    }
    uint16_t ఫలితం;
    UTRIE2_U16_PREV16(trie, ప్రారంభం, సంకేతం_బిందువు_ప్రారంభం, సంకేతం_బిందువు, ఫలితం);
    తిరిగి ఫలితం;
}

uint16_t ForwardUTrie2StringIterator::next16() {
    సంకేతం_బిందువు_ప్రారంభం=సంకేతం_బిందువు_హద్దు;
    ఒకవేళ(సంకేతం_బిందువు_హద్దు==హద్దు) {
        సంకేతం_బిందువు=U_SENTINEL;
        తిరిగి static_cast<uint16_t>(trie->error_విలువ);
    }
    uint16_t ఫలితం;
    UTRIE2_U16_NEXT16(trie, సంకేతం_బిందువు_హద్దు, హద్దు, సంకేతం_బిందువు, ఫలితం);
    తిరిగి ఫలితం;
}

U_NAMESPACE_చివర

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 1999-2016, International Business Machines Corporation
*               and others. All Rights Reserved.
*******************************************************************************
*   file name:  uresdata.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999dec08
*   created by: Markus W. Scherer
* Modification History:
*
*   Date        Name        Description
*   06/20/2000  helena      OS/400 port changes; mostly typecast.
*   06/24/02    weiv        Added support for resource sharing
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "resource.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucol_swp.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "uinvchar.h"
#అన్తర్భావ "uresdata.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#నిర్వచించు RES_GET_KEY16(p_Res_దత్తాంశం, కీలకం_స్థానభ్రంశం) \
    ((కీలకం_స్థానభ్రంశం)<(p_Res_దత్తాంశం)->local_కీలకం_హద్దు ? \
        (స్థిరరాశి అక్షరం *)(p_Res_దత్తాంశం)->p_మూలం+(కీలకం_స్థానభ్రంశం) : \
        (p_Res_దత్తాంశం)->pool_Bundle_కీలకాలు+(కీలకం_స్థానభ్రంశం)-(p_Res_దత్తాంశం)->local_కీలకం_హద్దు)

#నిర్వచించు RES_GET_KEY32(p_Res_దత్తాంశం, కీలకం_స్థానభ్రంశం) \
    ((కీలకం_స్థానభ్రంశం)>=0 ? \
        (స్థిరరాశి అక్షరం *)(p_Res_దత్తాంశం)->p_మూలం+(కీలకం_స్థానభ్రంశం) : \
        (p_Res_దత్తాంశం)->pool_Bundle_కీలకాలు+((కీలకం_స్థానభ్రంశం)&0x7fffffff))

#నిర్వచించు URESDATA_అంశం_NOT_దొరికింది -1

/* empty resources, returned when the resource offset is 0 */
స్థిర స్థిరరాశి uint16_t gEmpty16=0;

స్థిర స్థిరరాశి నిర్మాణం {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

స్థిర స్థిరరాశి నిర్మాణం {
    int32_t length;
    char16_t nul;
    char16_t pad;
} g_ఖాళీలు_String={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

స్థిర int32_t
res_కనుగొను_పట్టిక_అంశం(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, స్థిరరాశి uint16_t *కీలకం_Offsets, int32_t length,
                   స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం **real_కీలకం) {
    స్థిరరాశి అక్షరం *పట్టిక_కీలకం;
    int32_t mid, ప్రారంభం, హద్దు;
    పూర్ణాంకం ఫలితం;

    /* do a binary search for the key */
    ప్రారంభం=0;
    హద్దు=length;
    అయితే(ప్రారంభం<హద్దు) {
        mid = (ప్రారంభం + హద్దు) / 2;
        పట్టిక_కీలకం = RES_GET_KEY16(p_Res_దత్తాంశం, కీలకం_Offsets[mid]);
        ఒకవేళ (p_Res_దత్తాంశం->useNativeStrcmp) {
            ఫలితం = uprv_strcmp(కీలకం, పట్టిక_కీలకం);
        } లేకపోతే {
            ఫలితం = uprv_compareInvCharsAsAscii(కీలకం, పట్టిక_కీలకం);
        }
        ఒకవేళ (ఫలితం < 0) {
            హద్దు = mid;
        } లేకపోతే ఒకవేళ (ఫలితం > 0) {
            ప్రారంభం = mid + 1;
        } లేకపోతే {
            /* We found it! */
            *real_కీలకం=పట్టిక_కీలకం;
            తిరిగి mid;
        }
    }
    తిరిగి URESDATA_అంశం_NOT_దొరికింది;  /* not found or table is empty. */
}

స్థిర int32_t
res_కనుగొను_Table32_అంశం(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, స్థిరరాశి int32_t *కీలకం_Offsets, int32_t length,
                     స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం **real_కీలకం) {
    స్థిరరాశి అక్షరం *పట్టిక_కీలకం;
    int32_t mid, ప్రారంభం, హద్దు;
    పూర్ణాంకం ఫలితం;

    /* do a binary search for the key */
    ప్రారంభం=0;
    హద్దు=length;
    అయితే(ప్రారంభం<హద్దు) {
        mid = (ప్రారంభం + హద్దు) / 2;
        పట్టిక_కీలకం = RES_GET_KEY32(p_Res_దత్తాంశం, కీలకం_Offsets[mid]);
        ఒకవేళ (p_Res_దత్తాంశం->useNativeStrcmp) {
            ఫలితం = uprv_strcmp(కీలకం, పట్టిక_కీలకం);
        } లేకపోతే {
            ఫలితం = uprv_compareInvCharsAsAscii(కీలకం, పట్టిక_కీలకం);
        }
        ఒకవేళ (ఫలితం < 0) {
            హద్దు = mid;
        } లేకపోతే ఒకవేళ (ఫలితం > 0) {
            ప్రారంభం = mid + 1;
        } లేకపోతే {
            /* We found it! */
            *real_కీలకం=పట్టిక_కీలకం;
            తిరిగి mid;
        }
    }
    తిరిగి URESDATA_అంశం_NOT_దొరికింది;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

స్థిర UBool U_CALLCONV
isAcceptable(శూన్య *context,
             స్థిరరాశి అక్షరం * /*type*/, స్థిరరాశి అక్షరం * /*name*/,
             స్థిరరాశి UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->format_సంస్కరణ, 4);
    తిరిగి
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->దత్తాంశం_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->దత్తాంశం_Format[1]==0x65 &&
        pInfo->దత్తాంశం_Format[2]==0x73 &&
        pInfo->దత్తాంశం_Format[3]==0x42 &&
        (1<=pInfo->format_సంస్కరణ[0] && pInfo->format_సంస్కరణ[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

స్థిర శూన్య
res_init(Resource_దత్తాంశం *p_Res_దత్తాంశం,
         UVersionInfo format_సంస్కరణ, స్థిరరాశి శూన్య *లోపల_బైట్లు, int32_t length,
         UError_సంకేతం *error_సంకేతం) {
    UResType మూలం_Type;

    /* get the root resource */
    p_Res_దత్తాంశం->p_మూలం = static_cast<స్థిరరాశి int32_t*>(లోపల_బైట్లు);
    p_Res_దత్తాంశం->మూలం_Res = static_cast<Resource>(*p_Res_దత్తాంశం->p_మూలం);
    p_Res_దత్తాంశం->p16_బిట్టు_Units=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    ఒకవేళ(length>=0 && (length/4)<((format_సంస్కరణ[0]==1 && format_సంస్కరణ[1]==0) ? 1 : 1+5)) {
        *error_సంకేతం=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_దత్తాంశం);
        తిరిగి;
    }

    /* currently, we accept only resources that have a Table as their roots */
    మూలం_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_దత్తాంశం->మూలం_Res));
    ఒకవేళ(!URES_IS_పట్టిక(మూలం_Type)) {
        *error_సంకేతం=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_దత్తాంశం);
        తిరిగి;
    }

    ఒకవేళ(format_సంస్కరణ[0]==1 && format_సంస్కరణ[1]==0) {
        p_Res_దత్తాంశం->local_కీలకం_హద్దు=0x10000;  /* greater than any 16-bit key string offset */
    } లేకపోతే {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        స్థిరరాశి int32_t *indexes=p_Res_దత్తాంశం->p_మూలం+1;
        int32_t సూచీ_నిడివి=indexes[URES_సూచీ_నిడివి]&0xff;
        ఒకవేళ(సూచీ_నిడివి<=URES_సూచీ_MAX_పట్టిక_నిడివి) {
            *error_సంకేతం=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_దత్తాంశం);
            తిరిగి;
        }
        ఒకవేళ( length>=0 &&
            (length<((1+సూచీ_నిడివి)<<2) ||
             length<(indexes[URES_సూచీ_BUNDLE_శిఖరం]<<2))
        ) {
            *error_సంకేతం=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_దత్తాంశం);
            తిరిగి;
        }
        ఒకవేళ(indexes[URES_సూచీ_కీలకాలు_శిఖరం]>(1+సూచీ_నిడివి)) {
            p_Res_దత్తాంశం->local_కీలకం_హద్దు=indexes[URES_సూచీ_కీలకాలు_శిఖరం]<<2;
        }
        ఒకవేళ(format_సంస్కరణ[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_దత్తాంశం->pool_String_సూచీ_హద్దు = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_సూచీ_నిడివి]) >> 8);
        }
        ఒకవేళ(సూచీ_నిడివి>URES_సూచీ_లక్షణాలు) {
            int32_t att=indexes[URES_సూచీ_లక్షణాలు];
            p_Res_దత్తాంశం->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_దత్తాంశం->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_దత్తాంశం->ఉపయోగిస్తుంది_Pool_Bundle = static_cast<UBool>((att & URES_ATT_ఉపయోగిస్తుంది_POOL_BUNDLE) != 0);
            p_Res_దత్తాంశం->pool_String_సూచీ_హద్దు|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_దత్తాంశం->pool_String_Index16_హద్దు = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        ఒకవేళ((p_Res_దత్తాంశం->isPoolBundle || p_Res_దత్తాంశం->ఉపయోగిస్తుంది_Pool_Bundle) && సూచీ_నిడివి<=URES_సూచీ_POOL_CHECKSUM) {
            *error_సంకేతం=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_దత్తాంశం);
            తిరిగి;
        }
        ఒకవేళ( సూచీ_నిడివి>URES_సూచీ_16_బిట్టు_శిఖరం &&
            indexes[URES_సూచీ_16_బిట్టు_శిఖరం]>indexes[URES_సూచీ_కీలకాలు_శిఖరం]
        ) {
            p_Res_దత్తాంశం->p16_బిట్టు_Units = reinterpret_cast<స్థిరరాశి uint16_t*>(p_Res_దత్తాంశం->p_మూలం + indexes[URES_సూచీ_కీలకాలు_శిఖరం]);
        }
    }

    ఒకవేళ(format_సంస్కరణ[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_దత్తాంశం->useNativeStrcmp=నిజం;
    }
}

U_CAPI శూన్య U_EXPORT2
res_read(Resource_దత్తాంశం *p_Res_దత్తాంశం,
         స్థిరరాశి UDataInfo *pInfo, స్థిరరాశి శూన్య *లోపల_బైట్లు, int32_t length,
         UError_సంకేతం *error_సంకేతం) {
    UVersionInfo format_సంస్కరణ;

    uprv_memset(p_Res_దత్తాంశం, 0, పరిమాణం(Resource_దత్తాంశం));
    ఒకవేళ(U_FAILURE(*error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ(!isAcceptable(format_సంస్కరణ, శూన్యసూచిక, శూన్యసూచిక, pInfo)) {
        *error_సంకేతం=U_INVALID_FORMAT_ERROR;
        తిరిగి;
    }
    res_init(p_Res_దత్తాంశం, format_సంస్కరణ, లోపల_బైట్లు, length, error_సంకేతం);
}

U_CFUNC శూన్య
res_ఎక్కించు(Resource_దత్తాంశం *p_Res_దత్తాంశం,
         స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, UError_సంకేతం *error_సంకేతం) {
    UVersionInfo format_సంస్కరణ;

    uprv_memset(p_Res_దత్తాంశం, 0, పరిమాణం(Resource_దత్తాంశం));

    /* load the ResourceBundle file */
    p_Res_దత్తాంశం->దత్తాంశం=udata_openChoice(మార్గం, "res", name, isAcceptable, format_సంస్కరణ, error_సంకేతం);
    ఒకవేళ(U_FAILURE(*error_సంకేతం)) {
        తిరిగి;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_దత్తాంశం, format_సంస్కరణ, udata_get_జ్ఞాపకం(p_Res_దత్తాంశం->దత్తాంశం), -1, error_సంకేతం);
}

U_CFUNC శూన్య
res_unload(Resource_దత్తాంశం *p_Res_దత్తాంశం) {
    ఒకవేళ(p_Res_దత్తాంశం->దత్తాంశం!=శూన్యసూచిక) {
        udata_close(p_Res_దత్తాంశం->దత్తాంశం);
        p_Res_దత్తాంశం->దత్తాంశం=శూన్యసూచిక;
    }
}

స్థిర స్థిరరాశి int8_t g_ప్రజా_Types[URES_హద్దు] = {
    URES_STRING,
    URES_ద్విమాన,
    URES_పట్టిక,
    URES_ALIAS,

    URES_పట్టిక,     /* URES_TABLE32 */
    URES_పట్టిక,     /* URES_TABLE16 */
    URES_STRING,    /* URES_STRING_V2 */
    URES_INT,

    URES_ARRAY,
    URES_ARRAY,     /* URES_ARRAY16 */
    URES_NONE,
    URES_NONE,

    URES_NONE,
    URES_NONE,
    URES_INT_సదిశ,
    URES_NONE
};

U_CAPI UResType U_EXPORT2
res_get_ప్రజా_Type(Resource res) {
    తిరిగి (UResType)g_ప్రజా_Types[RES_GET_TYPE(res)];
}

U_CAPI స్థిరరాశి char16_t * U_EXPORT2
res_getStringNoTrace(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource res, int32_t *p_నిడివి) {
    స్థిరరాశి char16_t *p;
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(res);
    int32_t length;
    ఒకవేళ(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        ఒకవేళ((int32_t)స్థానభ్రంశం<p_Res_దత్తాంశం->pool_String_సూచీ_హద్దు) {
            p=(స్థిరరాశి char16_t *)p_Res_దత్తాంశం->pool_Bundle_అక్షరశ్రేణులు+స్థానభ్రంశం;
        } లేకపోతే {
            p=(స్థిరరాశి char16_t *)p_Res_దత్తాంశం->p16_బిట్టు_Units+(స్థానభ్రంశం-p_Res_దత్తాంశం->pool_String_సూచీ_హద్దు);
        }
        first=*p;
        ఒకవేళ(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } లేకపోతే ఒకవేళ(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } లేకపోతే ఒకవేళ(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } లేకపోతే {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } లేకపోతే ఒకవేళ(res==స్థానభ్రంశం) /* RES_GET_TYPE(res)==URES_STRING */ {
        స్థిరరాశి int32_t *p32= res==0 ? &g_ఖాళీలు_String.length : p_Res_దత్తాంశం->p_మూలం+res;
        length=*p32++;
        p=(స్థిరరాశి char16_t *)p32;
    } లేకపోతే {
        p=శూన్యసూచిక;
        length=0;
    }
    ఒకవేళ(p_నిడివి) {
        *p_నిడివి=length;
    }
    తిరిగి p;
}

నామాకాశ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource res) {
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(res);
    ఒకవేళ (స్థానభ్రంశం == 0) {
        // empty string
    } లేకపోతే ఒకవేళ (res == స్థానభ్రంశం) {
        స్థిరరాశి int32_t *p32=p_Res_దత్తాంశం->p_మూలం+res;
        int32_t length=*p32;
        స్థిరరాశి char16_t* p = reinterpret_cast<స్థిరరాశి char16_t*>(p32);
        తిరిగి length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } లేకపోతే ఒకవేళ (RES_GET_TYPE(res) == URES_STRING_V2) {
        స్థిరరాశి char16_t *p;
        ఒకవేళ (static_cast<int32_t>(స్థానభ్రంశం) < p_Res_దత్తాంశం->pool_String_సూచీ_హద్దు) {
            p = reinterpret_cast<స్థిరరాశి char16_t*>(p_Res_దత్తాంశం->pool_Bundle_అక్షరశ్రేణులు) + స్థానభ్రంశం;
        } లేకపోతే {
            p = reinterpret_cast<స్థిరరాశి char16_t*>(p_Res_దత్తాంశం->p16_బిట్టు_Units) + (స్థానభ్రంశం - p_Res_దత్తాంశం->pool_String_సూచీ_హద్దు);
        }
        int32_t first=*p;
        ఒకవేళ (first == 0x2205) {  // implicit length
            తిరిగి p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } లేకపోతే ఒకవేళ (first == 0xdc03) {  // explicit length 3 (should not occur)
            తిరిగి p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } లేకపోతే {
            // Assume that the string has not been stored with more length units than necessary.
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి అబద్ధం;
}

int32_t getStringArray(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, స్థిరరాశి icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest == శూన్యసూచిక ? capacity != 0 : capacity < 0) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    int32_t length = array.get_కొలత();
    ఒకవేళ(length == 0) {
        తిరిగి 0;
    }
    ఒకవేళ(length > capacity) {
        error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి length;
    }
    కొరకు(int32_t i = 0; i < length; ++i) {
        int32_t s_నిడివి;
        // No tracing: handled by the caller
        స్థిరరాశి char16_t *s = res_getStringNoTrace(p_Res_దత్తాంశం, array.అంతర్గత_Get_Resource(p_Res_దత్తాంశం, i), &s_నిడివి);
        ఒకవేళ(s == శూన్యసూచిక) {
            error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
            తిరిగి 0;
        }
        dest[i].సమితి_గమ్యం(నిజం, s, s_నిడివి);
    }
    తిరిగి length;
}

}  // namespace

U_CAPI స్థిరరాశి char16_t * U_EXPORT2
res_getAlias(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource res, int32_t *p_నిడివి) {
    స్థిరరాశి char16_t *p;
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(res);
    int32_t length;
    ఒకవేళ(RES_GET_TYPE(res)==URES_ALIAS) {
        స్థిరరాశి int32_t *p32= స్థానభ్రంశం==0 ? &g_ఖాళీలు_String.length : p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం;
        length=*p32++;
        p=(స్థిరరాశి char16_t *)p32;
    } లేకపోతే {
        p=శూన్యసూచిక;
        length=0;
    }
    ఒకవేళ(p_నిడివి) {
        *p_నిడివి=length;
    }
    తిరిగి p;
}

U_CAPI స్థిరరాశి uint8_t * U_EXPORT2
res_get_ద్విమాన_No_Trace(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource res, int32_t *p_నిడివి) {
    స్థిరరాశి uint8_t *p;
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(res);
    int32_t length;
    ఒకవేళ(RES_GET_TYPE(res)==URES_ద్విమాన) {
        స్థిరరాశి int32_t *p32= స్థానభ్రంశం==0 ? (స్థిరరాశి int32_t*)&gEmpty32 : p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం;
        length=*p32++;
        p=(స్థిరరాశి uint8_t *)p32;
    } లేకపోతే {
        p=శూన్యసూచిక;
        length=0;
    }
    ఒకవేళ(p_నిడివి) {
        *p_నిడివి=length;
    }
    తిరిగి p;
}


U_CAPI స్థిరరాశి int32_t * U_EXPORT2
res_get_Int_సదిశ_No_Trace(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource res, int32_t *p_నిడివి) {
    స్థిరరాశి int32_t *p;
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(res);
    int32_t length;
    ఒకవేళ(RES_GET_TYPE(res)==URES_INT_సదిశ) {
        p= స్థానభ్రంశం==0 ? (స్థిరరాశి int32_t *)&gEmpty32 : p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం;
        length=*p++;
    } లేకపోతే {
        p=శూన్యసూచిక;
        length=0;
    }
    ఒకవేళ(p_నిడివి) {
        *p_నిడివి=length;
    }
    తిరిగి p;
}

U_CAPI int32_t U_EXPORT2
res_లెక్క_Array_వస్తువులు(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource res) {
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(res);
    ఎంపిక(RES_GET_TYPE(res)) {
    స్థితి URES_STRING:
    స్థితి URES_STRING_V2:
    స్థితి URES_ద్విమాన:
    స్థితి URES_ALIAS:
    స్థితి URES_INT:
    స్థితి URES_INT_సదిశ:
        తిరిగి 1;
    స్థితి URES_ARRAY:
    స్థితి URES_TABLE32:
        తిరిగి స్థానభ్రంశం==0 ? 0 : *(p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం);
    స్థితి URES_పట్టిక:
        తిరిగి స్థానభ్రంశం==0 ? 0 : *((స్థిరరాశి uint16_t *)(p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం));
    స్థితి URES_ARRAY16:
    స్థితి URES_TABLE16:
        తిరిగి p_Res_దత్తాంశం->p16_బిట్టు_Units[స్థానభ్రంశం];
    అప్రమేయ:
        తిరిగి 0;
    }
}

U_NAMESPACE_BEGIN

Resource_దత్తాంశం_విలువ::~Resource_దత్తాంశం_విలువ() {}

UResType Resource_దత్తాంశం_విలువ::getType() స్థిరరాశి {
    తిరిగి res_get_ప్రజా_Type(res);
}

స్థిరరాశి char16_t *Resource_దత్తాంశం_విలువ::getString(int32_t &length, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి char16_t *s = res_getString(fTraceInfo, &get_దత్తాంశం(), res, &length);
    ఒకవేళ(s == శూన్యసూచిక) {
        error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
    }
    తిరిగి s;
}

స్థిరరాశి char16_t *Resource_దత్తాంశం_విలువ::getAliasString(int32_t &length, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి char16_t *s = res_getAlias(&get_దత్తాంశం(), res, &length);
    ఒకవేళ(s == శూన్యసూచిక) {
        error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
    }
    తిరిగి s;
}

int32_t Resource_దత్తాంశం_విలువ::getInt(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(RES_GET_TYPE(res) != URES_INT) {
        error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
    }
    తిరిగి res_getInt(fTraceInfo, res);
}

uint32_t Resource_దత్తాంశం_విలువ::getUInt(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(RES_GET_TYPE(res) != URES_INT) {
        error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
    }
    తిరిగి res_getUInt(fTraceInfo, res);
}

స్థిరరాశి int32_t *Resource_దత్తాంశం_విలువ::get_Int_సదిశ(int32_t &length, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి int32_t *iv = res_get_Int_సదిశ(fTraceInfo, &get_దత్తాంశం(), res, &length);
    ఒకవేళ(iv == శూన్యసూచిక) {
        error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
    }
    తిరిగి iv;
}

స్థిరరాశి uint8_t *Resource_దత్తాంశం_విలువ::get_ద్విమాన(int32_t &length, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి uint8_t *b = res_get_ద్విమాన(fTraceInfo, &get_దత్తాంశం(), res, &length);
    ఒకవేళ(b == శూన్యసూచిక) {
        error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
    }
    తిరిగి b;
}

ResourceArray Resource_దత్తాంశం_విలువ::getArray(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి {};
    }
    స్థిరరాశి uint16_t *items16 = శూన్యసూచిక;
    స్థిరరాశి Resource *items32 = శూన్యసూచిక;
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(res);
    int32_t length = 0;
    ఎంపిక(RES_GET_TYPE(res)) {
    స్థితి URES_ARRAY:
        ఒకవేళ (స్థానభ్రంశం!=0) {  // empty if offset==0
            items32 = reinterpret_cast<స్థిరరాశి Resource*>(get_దత్తాంశం().p_మూలం) + స్థానభ్రంశం;
            length = *items32++;
        }
        ఆపు;
    స్థితి URES_ARRAY16:
        items16 = get_దత్తాంశం().p16_బిట్టు_Units+స్థానభ్రంశం;
        length = *items16++;
        ఆపు;
    అప్రమేయ:
        error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
        తిరిగి {};
    }
    తిరిగి ResourceArray(items16, items32, length, fTraceInfo);
}

Resource_పట్టిక Resource_దత్తాంశం_విలువ::get_పట్టిక(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి {};
    }
    స్థిరరాశి uint16_t *keys16 = శూన్యసూచిక;
    స్థిరరాశి int32_t *keys32 = శూన్యసూచిక;
    స్థిరరాశి uint16_t *items16 = శూన్యసూచిక;
    స్థిరరాశి Resource *items32 = శూన్యసూచిక;
    uint32_t స్థానభ్రంశం = RES_GET_స్థానభ్రంశం(res);
    int32_t length = 0;
    ఎంపిక(RES_GET_TYPE(res)) {
    స్థితి URES_పట్టిక:
        ఒకవేళ (స్థానభ్రంశం != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<స్థిరరాశి uint16_t*>(get_దత్తాంశం().p_మూలం + స్థానభ్రంశం);
            length = *keys16++;
            items32 = reinterpret_cast<స్థిరరాశి Resource*>(keys16 + length + (~length & 1));
        }
        ఆపు;
    స్థితి URES_TABLE16:
        keys16 = get_దత్తాంశం().p16_బిట్టు_Units+స్థానభ్రంశం;
        length = *keys16++;
        items16 = keys16 + length;
        ఆపు;
    స్థితి URES_TABLE32:
        ఒకవేళ (స్థానభ్రంశం != 0) {  // empty if offset==0
            keys32 = get_దత్తాంశం().p_మూలం+స్థానభ్రంశం;
            length = *keys32++;
            items32 = reinterpret_cast<స్థిరరాశి Resource*>(keys32) + length;
        }
        ఆపు;
    అప్రమేయ:
        error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
        తిరిగి {};
    }
    తిరిగి Resource_పట్టిక(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_దత్తాంశం_విలువ::isNoInheritanceMarker() స్థిరరాశి {
    తిరిగి ::isNoInheritanceMarker(&get_దత్తాంశం(), res);
}

int32_t Resource_దత్తాంశం_విలువ::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    తిరిగి ::getStringArray(&get_దత్తాంశం(), getArray(error_సంకేతం), dest, capacity, error_సంకేతం);
}

int32_t Resource_దత్తాంశం_విలువ::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ(URES_IS_ARRAY(res)) {
        తిరిగి ::getStringArray(&get_దత్తాంశం(), getArray(error_సంకేతం), dest, capacity, error_సంకేతం);
    }
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest == శూన్యసూచిక ? capacity != 0 : capacity < 0) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    ఒకవేళ(capacity < 1) {
        error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి 1;
    }
    int32_t s_నిడివి;
    స్థిరరాశి char16_t *s = res_getString(fTraceInfo, &get_దత్తాంశం(), res, &s_నిడివి);
    ఒకవేళ(s != శూన్యసూచిక) {
        dest[0].సమితి_గమ్యం(నిజం, s, s_నిడివి);
        తిరిగి 1;
    }
    error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
    తిరిగి 0;
}

UnicodeString Resource_దత్తాంశం_విలువ::get_String_Or_మొదటి_యొక్క_Array(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    UnicodeString us;
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి us;
    }
    int32_t s_నిడివి;
    స్థిరరాశి char16_t *s = res_getString(fTraceInfo, &get_దత్తాంశం(), res, &s_నిడివి);
    ఒకవేళ(s != శూన్యసూచిక) {
        us.సమితి_గమ్యం(నిజం, s, s_నిడివి);
        తిరిగి us;
    }
    ResourceArray array = getArray(error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి us;
    }
    ఒకవేళ(array.get_కొలత() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_దత్తాంశం(), array.అంతర్గత_Get_Resource(&get_దత్తాంశం(), 0), &s_నిడివి);
        ఒకవేళ(s != శూన్యసూచిక) {
            us.సమితి_గమ్యం(నిజం, s, s_నిడివి);
            తిరిగి us;
        }
    }
    error_సంకేతం = U_RESOURCE_TYPE_MISMATCH;
    తిరిగి us;
}

U_NAMESPACE_చివర

స్థిర Resource
makeResourceFrom16(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, int32_t res16) {
    ఒకవేళ(res16<p_Res_దత్తాంశం->pool_String_Index16_హద్దు) {
        // Pool string, nothing to do.
    } లేకపోతే {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_దత్తాంశం->pool_String_Index16_హద్దు+p_Res_దత్తాంశం->pool_String_సూచీ_హద్దు;
    }
    తిరిగి URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_పట్టిక_అంశం_By_కీలకం(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource పట్టిక,
                      int32_t *సూచీ_R, స్థిరరాశి అక్షరం **కీలకం) {
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(పట్టిక);
    int32_t length;
    int32_t idx;
    ఒకవేళ(కీలకం == శూన్యసూచిక || *కీలకం == శూన్యసూచిక) {
        తిరిగి RES_BOGUS;
    }
    ఎంపిక(RES_GET_TYPE(పట్టిక)) {
    స్థితి URES_పట్టిక: {
        ఒకవేళ (స్థానభ్రంశం!=0) { /* empty if offset==0 */
            స్థిరరాశి uint16_t *p= (స్థిరరాశి uint16_t *)(p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం);
            length=*p++;
            *సూచీ_R=idx=res_కనుగొను_పట్టిక_అంశం(p_Res_దత్తాంశం, p, length, *కీలకం, కీలకం);
            ఒకవేళ(idx>=0) {
                స్థిరరాశి Resource *p32=(స్థిరరాశి Resource *)(p+length+(~length&1));
                తిరిగి p32[idx];
            }
        }
        ఆపు;
    }
    స్థితి URES_TABLE16: {
        స్థిరరాశి uint16_t *p=p_Res_దత్తాంశం->p16_బిట్టు_Units+స్థానభ్రంశం;
        length=*p++;
        *సూచీ_R=idx=res_కనుగొను_పట్టిక_అంశం(p_Res_దత్తాంశం, p, length, *కీలకం, కీలకం);
        ఒకవేళ(idx>=0) {
            తిరిగి makeResourceFrom16(p_Res_దత్తాంశం, p[length+idx]);
        }
        ఆపు;
    }
    స్థితి URES_TABLE32: {
        ఒకవేళ (స్థానభ్రంశం!=0) { /* empty if offset==0 */
            స్థిరరాశి int32_t *p= p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం;
            length=*p++;
            *సూచీ_R=idx=res_కనుగొను_Table32_అంశం(p_Res_దత్తాంశం, p, length, *కీలకం, కీలకం);
            ఒకవేళ(idx>=0) {
                తిరిగి (Resource)p[length+idx];
            }
        }
        ఆపు;
    }
    అప్రమేయ:
        ఆపు;
    }
    తిరిగి RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_get_పట్టిక_అంశం_By_సూచీ(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource పట్టిక,
                        int32_t సూచీ_R, స్థిరరాశి అక్షరం **కీలకం) {
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(పట్టిక);
    int32_t length;
    ఒకవేళ (సూచీ_R < 0) {
        తిరిగి RES_BOGUS;
    }
    ఎంపిక(RES_GET_TYPE(పట్టిక)) {
    స్థితి URES_పట్టిక: {
        ఒకవేళ (స్థానభ్రంశం != 0) { /* empty if offset==0 */
            స్థిరరాశి uint16_t *p= (స్థిరరాశి uint16_t *)(p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం);
            length=*p++;
            ఒకవేళ(సూచీ_R<length) {
                స్థిరరాశి Resource *p32=(స్థిరరాశి Resource *)(p+length+(~length&1));
                ఒకవేళ(కీలకం!=శూన్యసూచిక) {
                    *కీలకం=RES_GET_KEY16(p_Res_దత్తాంశం, p[సూచీ_R]);
                }
                తిరిగి p32[సూచీ_R];
            }
        }
        ఆపు;
    }
    స్థితి URES_TABLE16: {
        స్థిరరాశి uint16_t *p=p_Res_దత్తాంశం->p16_బిట్టు_Units+స్థానభ్రంశం;
        length=*p++;
        ఒకవేళ(సూచీ_R<length) {
            ఒకవేళ(కీలకం!=శూన్యసూచిక) {
                *కీలకం=RES_GET_KEY16(p_Res_దత్తాంశం, p[సూచీ_R]);
            }
            తిరిగి makeResourceFrom16(p_Res_దత్తాంశం, p[length+సూచీ_R]);
        }
        ఆపు;
    }
    స్థితి URES_TABLE32: {
        ఒకవేళ (స్థానభ్రంశం != 0) { /* empty if offset==0 */
            స్థిరరాశి int32_t *p= p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం;
            length=*p++;
            ఒకవేళ(సూచీ_R<length) {
                ఒకవేళ(కీలకం!=శూన్యసూచిక) {
                    *కీలకం=RES_GET_KEY32(p_Res_దత్తాంశం, p[సూచీ_R]);
                }
                తిరిగి (Resource)p[length+సూచీ_R];
            }
        }
        ఆపు;
    }
    అప్రమేయ:
        ఆపు;
    }
    తిరిగి RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, స్థిరరాశి అక్షరం *కీలకం) {
    స్థిరరాశి అక్షరం *real_కీలకం=కీలకం;
    int32_t idx;
    తిరిగి res_get_పట్టిక_అంశం_By_కీలకం(p_Res_దత్తాంశం, p_Res_దత్తాంశం->మూలం_Res, &idx, &real_కీలకం);
}


UBool icu::Resource_పట్టిక::get_కీలకం_And_విలువ(int32_t i,
                                         స్థిరరాశి అక్షరం *&కీలకం, icu::Resource_విలువ &value) స్థిరరాశి {
    ఒకవేళ(0 <= i && i < length) {
        icu::Resource_దత్తాంశం_విలువ &rd_విలువ = static_cast<icu::Resource_దత్తాంశం_విలువ &>(value);
        ఒకవేళ (keys16 != శూన్యసూచిక) {
            కీలకం = RES_GET_KEY16(&rd_విలువ.get_దత్తాంశం(), keys16[i]);
        } లేకపోతే {
            కీలకం = RES_GET_KEY32(&rd_విలువ.get_దత్తాంశం(), keys32[i]);
        }
        Resource res;
        ఒకవేళ (items16 != శూన్యసూచిక) {
            res = makeResourceFrom16(&rd_విలువ.get_దత్తాంశం(), items16[i]);
        } లేకపోతే {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_విలువ.సమితి_Resource(res, ResourceTracer(fTraceInfo, కీలకం));
        తిరిగి నిజం;
    }
    తిరిగి అబద్ధం;
}

UBool icu::Resource_పట్టిక::కనుగొను_విలువ(స్థిరరాశి అక్షరం *కీలకం, Resource_విలువ &value) స్థిరరాశి {
    icu::Resource_దత్తాంశం_విలువ &rd_విలువ = static_cast<icu::Resource_దత్తాంశం_విలువ &>(value);
    స్థిరరాశి అక్షరం *real_కీలకం = శూన్యసూచిక;
    int32_t i;
    ఒకవేళ (keys16 != శూన్యసూచిక) {
        i = res_కనుగొను_పట్టిక_అంశం(&rd_విలువ.get_దత్తాంశం(), keys16, length, కీలకం, &real_కీలకం);
    } లేకపోతే {
        i = res_కనుగొను_Table32_అంశం(&rd_విలువ.get_దత్తాంశం(), keys32, length, కీలకం, &real_కీలకం);
    }
    ఒకవేళ (i >= 0) {
        Resource res;
        ఒకవేళ (items16 != శూన్యసూచిక) {
            res = makeResourceFrom16(&rd_విలువ.get_దత్తాంశం(), items16[i]);
        } లేకపోతే {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_విలువ.సమితి_Resource(res, ResourceTracer(fTraceInfo, కీలకం));
        తిరిగి నిజం;
    }
    తిరిగి అబద్ధం;
}

U_CAPI Resource U_EXPORT2
res_get_Array_అంశం(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource array, int32_t సూచీ_R) {
    uint32_t స్థానభ్రంశం=RES_GET_స్థానభ్రంశం(array);
    ఒకవేళ (సూచీ_R < 0) {
        తిరిగి RES_BOGUS;
    }
    ఎంపిక(RES_GET_TYPE(array)) {
    స్థితి URES_ARRAY: {
        ఒకవేళ (స్థానభ్రంశం!=0) { /* empty if offset==0 */
            స్థిరరాశి int32_t *p= p_Res_దత్తాంశం->p_మూలం+స్థానభ్రంశం;
            ఒకవేళ(సూచీ_R<*p) {
                తిరిగి (Resource)p[1+సూచీ_R];
            }
        }
        ఆపు;
    }
    స్థితి URES_ARRAY16: {
        స్థిరరాశి uint16_t *p=p_Res_దత్తాంశం->p16_బిట్టు_Units+స్థానభ్రంశం;
        ఒకవేళ(సూచీ_R<*p) {
            తిరిగి makeResourceFrom16(p_Res_దత్తాంశం, p[1+సూచీ_R]);
        }
        ఆపు;
    }
    అప్రమేయ:
        ఆపు;
    }
    తిరిగి RES_BOGUS;
}

uint32_t icu::ResourceArray::అంతర్గత_Get_Resource(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, int32_t i) స్థిరరాశి {
    ఒకవేళ (items16 != శూన్యసూచిక) {
        తిరిగి makeResourceFrom16(p_Res_దత్తాంశం, items16[i]);
    } లేకపోతే {
        తిరిగి items32[i];
    }
}

UBool icu::ResourceArray::get_విలువ(int32_t i, icu::Resource_విలువ &value) స్థిరరాశి {
    ఒకవేళ(0 <= i && i < length) {
        icu::Resource_దత్తాంశం_విలువ &rd_విలువ = static_cast<icu::Resource_దత్తాంశం_విలువ &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_విలువ.సమితి_Resource(
            అంతర్గత_Get_Resource(&rd_విలువ.get_దత్తాంశం(), i),
            ResourceTracer(fTraceInfo, i));
        తిరిగి నిజం;
    }
    తిరిగి అబద్ధం;
}

U_CFUNC Resource
res_కనుగొను_Resource(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, Resource r, అక్షరం** మార్గం, స్థిరరాశి అక్షరం** కీలకం) {
  అక్షరం *మార్గం_P = *మార్గం, *తదుపరి_Sep_P = *మార్గం;
  అక్షరం *close_సూచీ = శూన్యసూచిక;
  Resource t1 = r;
  Resource t2;
  int32_t సూచీ_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  ఒకవేళ(!uprv_strlen(మార్గం_P)) {
      తిరిగి r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  ఒకవేళ(!URES_IS_CONTAINER(type)) {
      తిరిగి RES_BOGUS;
  }
  
  అయితే(తదుపరి_Sep_P && *మార్గం_P && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    తదుపరి_Sep_P = uprv_strchr(మార్గం_P, RES_మార్గం_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    ఒకవేళ(తదుపరి_Sep_P != శూన్యసూచిక) {
      ఒకవేళ(తదుపరి_Sep_P == మార్గం_P) {
        // Empty key string.
        తిరిగి RES_BOGUS;
      }
      *తదుపరి_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *మార్గం = తదుపరి_Sep_P+1;
    } లేకపోతే {
      *మార్గం = uprv_strchr(మార్గం_P, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    ఒకవేళ(URES_IS_పట్టిక(type)) {
      *కీలకం = మార్గం_P;
      t2 = res_get_పట్టిక_అంశం_By_కీలకం(p_Res_దత్తాంశం, t1, &సూచీ_R, కీలకం);
    } లేకపోతే ఒకవేళ(URES_IS_ARRAY(type)) {
      సూచీ_R = uprv_strtol(మార్గం_P, &close_సూచీ, 10);
      ఒకవేళ(సూచీ_R >= 0 && *close_సూచీ == 0) {
        t2 = res_get_Array_అంశం(p_Res_దత్తాంశం, t1, సూచీ_R);
      } లేకపోతే {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *కీలకం = శూన్యసూచిక;
    } లేకపోతే { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    మార్గం_P = *మార్గం;
  }

  తిరిగి t1;
}

/* resource bundle swapping ------------------------------------------------- */

/*
 * Need to always enumerate the entire item tree,
 * track the lowest address of any item to use as the limit for char keys[],
 * track the highest address of any item to return the size of the data.
 *
 * We should have thought of storing those in the data...
 * It is possible to extend the data structure by putting additional values
 * in places that are inaccessible by ordinary enumeration of the item tree.
 * For example, additional integers could be stored at the beginning or
 * end of the key strings; this could be indicated by a minor version number,
 * and the data swapping would have to know about these values.
 *
 * The data structure does not forbid keys to be shared, so we must swap
 * all keys once instead of each key when it is referenced.
 *
 * These swapping functions assume that a resource bundle always has a length
 * that is a multiple of 4 bytes.
 * Currently, this is trivially true because genrb writes bundle tree leaves
 * physically first, before their branches, so that the root table with its
 * array of resource items (uint32_t values) is always last.
 */

/* definitions for table sorting ------------------------ */

/*
 * row of a temporary array
 *
 * gets platform-endian key string indexes and sorting indexes;
 * after sorting this array by keys, the actual key/value arrays are permutated
 * according to the sorting indexes
 */
రకనిర్వచనం నిర్మాణం అడ్డువరుస_2 {
    int32_t కీలకం_సూచీ, క్రమీకరించు_సూచీ;
} అడ్డువరుస_2;

స్థిర int32_t U_CALLCONV
ures_compare_అడ్డువరుసలు(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    స్థిరరాశి అక్షరం* కీలకం_Chars = static_cast<స్థిరరాశి అక్షరం*>(context);
    తిరిగి static_cast<int32_t>(uprv_strcmp(కీలకం_Chars + static_cast<స్థిరరాశి అడ్డువరుస_2*>(ఎడమ)->కీలకం_సూచీ,
                                            కీలకం_Chars + static_cast<స్థిరరాశి అడ్డువరుస_2*>(కుడి)->కీలకం_సూచీ));
}

రకనిర్వచనం నిర్మాణం తాత్కాలిక_పట్టిక_2 {
    స్థిరరాశి అక్షరం *కీలకం_Chars;
    అడ్డువరుస_2 *అడ్డువరుసలు;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_కీలకం_హద్దు;
    uint8_t major_Format_సంస్కరణ;
} తాత్కాలిక_పట్టిక_2;

గణన {
    దొంతర_అడ్డువరుస_సామర్థ్యం=200
};

/* The table item key string is not locally available. */
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి g_Unknown_కీలకం="";

#ఒకవేళ !UCONFIG_NO_COLLATION
// resource table key for collation binaries
స్థిర స్థిరరాశి char16_t g_Collation_Bin_కీలకం[]=u"%%CollationBin";
#అయితే_ముగింపు

/*
 * swap one resource item
 */
స్థిర శూన్య
ures_మార్పిడి_Resource(స్థిరరాశి UDataSwapper *ds,
                  స్థిరరాశి Resource *లోపల_Bundle, Resource *బహిర్గతం_Bundle,
                  Resource res, /* caller swaps res itself */
                  స్థిరరాశి అక్షరం *కీలకం,
                  తాత్కాలిక_పట్టిక_2 *p_తాత్కాలిక_పట్టిక,
                  UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి Resource *p;
    Resource *q;
    int32_t స్థానభ్రంశం, count;

    ఎంపిక(RES_GET_TYPE(res)) {
    స్థితి URES_TABLE16:
    స్థితి URES_STRING_V2:
    స్థితి URES_INT:
    స్థితి URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        తిరిగి;
    అప్రమేయ:
        ఆపు;
    }

    /* all other types use an offset to point to their data */
    స్థానభ్రంశం = static_cast<int32_t>(RES_GET_స్థానభ్రంశం(res));
    ఒకవేళ(స్థానభ్రంశం==0) {
        /* special offset indicating an empty item */
        తిరిగి;
    }
    ఒకవేళ (p_తాత్కాలిక_పట్టిక->resFlags[స్థానభ్రంశం >> 5] & (static_cast<uint32_t>(1) << (స్థానభ్రంశం & 0x1f))) {
        /* we already swapped this resource item */
        తిరిగి;
    } లేకపోతే {
        /* mark it as swapped now */
        p_తాత్కాలిక_పట్టిక->resFlags[స్థానభ్రంశం >> 5] |= static_cast<uint32_t>(1) << (స్థానభ్రంశం & 0x1f);
    }

    p=లోపల_Bundle+స్థానభ్రంశం;
    q=బహిర్గతం_Bundle+స్థానభ్రంశం;

    ఎంపిక(RES_GET_TYPE(res)) {
    స్థితి URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    స్థితి URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->మార్పిడి_Array32(ds, p, 4, q, p_Error_సంకేతం);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->మార్పిడి_Array16(ds, p+1, 2*count, q+1, p_Error_సంకేతం);
        ఆపు;
    స్థితి URES_ద్విమాన:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->మార్పిడి_Array32(ds, p, 4, q, p_Error_సంకేతం);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#ఒకవేళ !UCONFIG_NO_COLLATION
        ఒకవేళ( కీలకం!=శూన్యసూచిక &&  /* the binary is in a table */
            (కీలకం!=g_Unknown_కీలకం ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, కీలకం, -1,
                                       g_Collation_Bin_కీలకం, UPRV_LENGTHOF(g_Collation_Bin_కీలకం)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_ద్విమాన(ds, p+1, count))
        ) {
            ucol_మార్పిడి(ds, p+1, count, q+1, p_Error_సంకేతం);
        }
#అయితే_ముగింపు
        ఆపు;
    స్థితి URES_పట్టిక:
    స్థితి URES_TABLE32:
        {
            స్థిరరాశి uint16_t *pKey16;
            uint16_t *qKey16;

            స్థిరరాశి int32_t *pKey32;
            int32_t *qKey32;

            Resource అంశం;
            int32_t i, old_సూచీ;

            ఒకవేళ(RES_GET_TYPE(res)==URES_పట్టిక) {
                /* get table item count */
                pKey16 = reinterpret_cast<స్థిరరాశి uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=శూన్యసూచిక;

                /* swap count */
                ds->మార్పిడి_Array16(ds, pKey16++, 2, qKey16++, p_Error_సంకేతం);

                స్థానభ్రంశం+=((1+count)+1)/2;
            } లేకపోతే {
                /* get table item count */
                pKey32 = reinterpret_cast<స్థిరరాశి int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=శూన్యసూచిక;

                /* swap count */
                ds->మార్పిడి_Array32(ds, pKey32++, 4, qKey32++, p_Error_సంకేతం);

                స్థానభ్రంశం+=1+count;
            }

            ఒకవేళ(count==0) {
                ఆపు;
            }

            p=లోపల_Bundle+స్థానభ్రంశం; /* pointer to table resources */
            q=బహిర్గతం_Bundle+స్థానభ్రంశం;

            /* recurse */
            కొరకు(i=0; i<count; ++i) {
                స్థిరరాశి అక్షరం *అంశం_కీలకం=g_Unknown_కీలకం;
                ఒకవేళ(pKey16!=శూన్యసూచిక) {
                    int32_t కీలకం_స్థానభ్రంశం=ds->readUInt16(pKey16[i]);
                    ఒకవేళ(కీలకం_స్థానభ్రంశం<p_తాత్కాలిక_పట్టిక->local_కీలకం_హద్దు) {
                        అంశం_కీలకం = reinterpret_cast<స్థిరరాశి అక్షరం*>(బహిర్గతం_Bundle) + కీలకం_స్థానభ్రంశం;
                    }
                } లేకపోతే {
                    int32_t కీలకం_స్థానభ్రంశం=udata_readInt32(ds, pKey32[i]);
                    ఒకవేళ(కీలకం_స్థానభ్రంశం>=0) {
                        అంశం_కీలకం = reinterpret_cast<స్థిరరాశి అక్షరం*>(బహిర్గతం_Bundle) + కీలకం_స్థానభ్రంశం;
                    }
                }
                అంశం=ds->readUInt32(p[i]);
                ures_మార్పిడి_Resource(ds, లోపల_Bundle, బహిర్గతం_Bundle, అంశం, అంశం_కీలకం, p_తాత్కాలిక_పట్టిక, p_Error_సంకేతం);
                ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, అంశం);
                    తిరిగి;
                }
            }

            ఒకవేళ(p_తాత్కాలిక_పట్టిక->major_Format_సంస్కరణ>1 || ds->లోపల_Charset==ds->బహిర్గతం_Charset) {
                /* no need to sort, just swap the offset/value arrays */
                ఒకవేళ(pKey16!=శూన్యసూచిక) {
                    ds->మార్పిడి_Array16(ds, pKey16, count*2, qKey16, p_Error_సంకేతం);
                    ds->మార్పిడి_Array32(ds, p, count*4, q, p_Error_సంకేతం);
                } లేకపోతే {
                    /* swap key offsets and items as one array */
                    ds->మార్పిడి_Array32(ds, pKey32, count*2*4, qKey32, p_Error_సంకేతం);
                }
                ఆపు;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            ఒకవేళ(pKey16!=శూన్యసూచిక) {
                కొరకు(i=0; i<count; ++i) {
                    p_తాత్కాలిక_పట్టిక->అడ్డువరుసలు[i].కీలకం_సూచీ=ds->readUInt16(pKey16[i]);
                    p_తాత్కాలిక_పట్టిక->అడ్డువరుసలు[i].క్రమీకరించు_సూచీ=i;
                }
            } లేకపోతే {
                కొరకు(i=0; i<count; ++i) {
                    p_తాత్కాలిక_పట్టిక->అడ్డువరుసలు[i].కీలకం_సూచీ=udata_readInt32(ds, pKey32[i]);
                    p_తాత్కాలిక_పట్టిక->అడ్డువరుసలు[i].క్రమీకరించు_సూచీ=i;
                }
            }
            uprv_క్రమీకరించు_Array(p_తాత్కాలిక_పట్టిక->అడ్డువరుసలు, count, పరిమాణం(అడ్డువరుస_2),
                           ures_compare_అడ్డువరుసలు, p_తాత్కాలిక_పట్టిక->కీలకం_Chars,
                           అబద్ధం, p_Error_సంకేతం);
            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                తిరిగి;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*కీలకాలు*/
            ఒకవేళ(pKey16!=శూన్యసూచిక) {
                uint16_t *rKey16;

                ఒకవేళ(pKey16!=qKey16) {
                    rKey16=qKey16;
                } లేకపోతే {
                    rKey16 = reinterpret_cast<uint16_t*>(p_తాత్కాలిక_పట్టిక->resort);
                }
                కొరకు(i=0; i<count; ++i) {
                    old_సూచీ=p_తాత్కాలిక_పట్టిక->అడ్డువరుసలు[i].క్రమీకరించు_సూచీ;
                    ds->మార్పిడి_Array16(ds, pKey16+old_సూచీ, 2, rKey16+i, p_Error_సంకేతం);
                }
                ఒకవేళ(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } లేకపోతే {
                int32_t *rKey32;

                ఒకవేళ(pKey32!=qKey32) {
                    rKey32=qKey32;
                } లేకపోతే {
                    rKey32=p_తాత్కాలిక_పట్టిక->resort;
                }
                కొరకు(i=0; i<count; ++i) {
                    old_సూచీ=p_తాత్కాలిక_పట్టిక->అడ్డువరుసలు[i].క్రమీకరించు_సూచీ;
                    ds->మార్పిడి_Array32(ds, pKey32+old_సూచీ, 4, rKey32+i, p_Error_సంకేతం);
                }
                ఒకవేళ(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                ఒకవేళ(p!=q) {
                    r=q;
                } లేకపోతే {
                    r = reinterpret_cast<Resource*>(p_తాత్కాలిక_పట్టిక->resort);
                }
                కొరకు(i=0; i<count; ++i) {
                    old_సూచీ=p_తాత్కాలిక_పట్టిక->అడ్డువరుసలు[i].క్రమీకరించు_సూచీ;
                    ds->మార్పిడి_Array32(ds, p+old_సూచీ, 4, r+i, p_Error_సంకేతం);
                }
                ఒకవేళ(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        ఆపు;
    స్థితి URES_ARRAY:
        {
            Resource అంశం;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->మార్పిడి_Array32(ds, p++, 4, q++, p_Error_సంకేతం);

            /* recurse */
            కొరకు(i=0; i<count; ++i) {
                అంశం=ds->readUInt32(p[i]);
                ures_మార్పిడి_Resource(ds, లోపల_Bundle, బహిర్గతం_Bundle, అంశం, శూన్యసూచిక, p_తాత్కాలిక_పట్టిక, p_Error_సంకేతం);
                ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, అంశం);
                    తిరిగి;
                }
            }

            /* swap items */
            ds->మార్పిడి_Array32(ds, p, 4*count, q, p_Error_సంకేతం);
        }
        ఆపు;
    స్థితి URES_INT_సదిశ:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->మార్పిడి_Array32(ds, p, 4*(1+count), q, p_Error_సంకేతం);
        ఆపు;
    అప్రమేయ:
        /* also catches RES_BOGUS */
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        ఆపు;
    }
}

U_CAPI int32_t U_EXPORT2
ures_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
          స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
          UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UDataInfo *pInfo;
    స్థిరరాశి Resource *లోపల_Bundle;
    Resource మూలం_Res;
    int32_t శీర్షిక_కొలత, max_పట్టిక_నిడివి;

    అడ్డువరుస_2 అడ్డువరుసలు[దొంతర_అడ్డువరుస_సామర్థ్యం];
    int32_t resort[దొంతర_అడ్డువరుస_సామర్థ్యం];
    తాత్కాలిక_పట్టిక_2 తాత్కాలిక_పట్టిక;

    స్థిరరాశి int32_t *లోపల_Indexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_నిడివి, సూచీ_నిడివి, కీలకాలు_Bottom, కీలకాలు_శిఖరం, resBottom, శిఖరం;

    /* udata_swapDataHeader checks the arguments */
    శీర్షిక_కొలత=udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* check data format and format version */
    pInfo=(స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
    ఒకవేళ(!(
        pInfo->దత్తాంశం_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->దత్తాంశం_Format[1]==0x65 &&
        pInfo->దత్తాంశం_Format[2]==0x73 &&
        pInfo->దత్తాంశం_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->format_సంస్కరణ[0]==1 && pInfo->format_సంస్కరణ[1]>=1) ||
            pInfo->format_సంస్కరణ[0]==2 || pInfo->format_సంస్కరణ[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1],
                         pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3],
                         pInfo->format_సంస్కరణ[0], pInfo->format_సంస్కరణ[1]);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }
    తాత్కాలిక_పట్టిక.major_Format_సంస్కరణ=pInfo->format_సంస్కరణ[0];

    /* a resource bundle must contain at least one resource item */
    ఒకవేళ(length<0) {
        bundle_నిడివి=-1;
    } లేకపోతే {
        bundle_నిడివి=(length-శీర్షిక_కొలత)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        ఒకవేళ(bundle_నిడివి<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-శీర్షిక_కొలత);
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
    }

    లోపల_Bundle=(స్థిరరాశి Resource *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+శీర్షిక_కొలత);
    మూలం_Res=ds->readUInt32(*లోపల_Bundle);

    /* formatVersion 1.1 adds the indexes[] array */
    లోపల_Indexes=(స్థిరరాశి int32_t *)(లోపల_Bundle+1);

    సూచీ_నిడివి=udata_readInt32(ds, లోపల_Indexes[URES_సూచీ_నిడివి])&0xff;
    ఒకవేళ(సూచీ_నిడివి<=URES_సూచీ_MAX_పట్టిక_నిడివి) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }
    కీలకాలు_Bottom=1+సూచీ_నిడివి;
    కీలకాలు_శిఖరం=udata_readInt32(ds, లోపల_Indexes[URES_సూచీ_కీలకాలు_శిఖరం]);
    ఒకవేళ(సూచీ_నిడివి>URES_సూచీ_16_బిట్టు_శిఖరం) {
        resBottom=udata_readInt32(ds, లోపల_Indexes[URES_సూచీ_16_బిట్టు_శిఖరం]);
    } లేకపోతే {
        resBottom=కీలకాలు_శిఖరం;
    }
    శిఖరం=udata_readInt32(ds, లోపల_Indexes[URES_సూచీ_BUNDLE_శిఖరం]);
    max_పట్టిక_నిడివి=udata_readInt32(ds, లోపల_Indexes[URES_సూచీ_MAX_పట్టిక_నిడివి]);

    ఒకవేళ(0<=bundle_నిడివి && bundle_నిడివి<శిఖరం) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         శిఖరం, bundle_నిడివి);
        *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }
    ఒకవేళ(కీలకాలు_శిఖరం>(1+సూచీ_నిడివి)) {
        తాత్కాలిక_పట్టిక.local_కీలకం_హద్దు=కీలకాలు_శిఖరం<<2;
    } లేకపోతే {
        తాత్కాలిక_పట్టిక.local_కీలకం_హద్దు=0;
    }

    ఒకవేళ(length>=0) {
        Resource *బహిర్గతం_Bundle=(Resource *)((అక్షరం *)బహిర్గతం_దత్తాంశం+శీర్షిక_కొలత);

        /* track which resources we have already swapped */
        uint32_t దొంతర_Res_Flags[దొంతర_అడ్డువరుస_సామర్థ్యం];
        int32_t res_Flags_నిడివి;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_నిడివి=(length+31)>>5;          /* number of bytes needed */
        res_Flags_నిడివి=(res_Flags_నిడివి+3)&~3;   /* multiple of 4 bytes for uint32_t */
        ఒకవేళ(res_Flags_నిడివి<=(int32_t)పరిమాణం(దొంతర_Res_Flags)) {
            తాత్కాలిక_పట్టిక.resFlags=దొంతర_Res_Flags;
        } లేకపోతే {
            తాత్కాలిక_పట్టిక.resFlags=(uint32_t *)uprv_malloc(res_Flags_నిడివి);
            ఒకవేళ(తాత్కాలిక_పట్టిక.resFlags==శూన్యసూచిక) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి 0;
            }
        }
        uprv_memset(తాత్కాలిక_పట్టిక.resFlags, 0, res_Flags_నిడివి);

        /* copy the bundle for binary and inaccessible data */
        ఒకవేళ(లోపల_దత్తాంశం!=బహిర్గతం_దత్తాంశం) {
            uprv_memcpy(బహిర్గతం_Bundle, లోపల_Bundle, 4*శిఖరం);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_మార్పిడి_Inv_String_ఖండం(ds, లోపల_Bundle+కీలకాలు_Bottom, 4*(కీలకాలు_శిఖరం-కీలకాలు_Bottom),
                                    బహిర్గతం_Bundle+కీలకాలు_Bottom, p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(కీలకాలు_శిఖరం-కీలకాలు_Bottom));
            ఒకవేళ(తాత్కాలిక_పట్టిక.resFlags!=దొంతర_Res_Flags) {
                uprv_free(తాత్కాలిక_పట్టిక.resFlags);
            }
            తిరిగి 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        ఒకవేళ(కీలకాలు_శిఖరం<resBottom) {
            ds->మార్పిడి_Array16(ds, లోపల_Bundle+కీలకాలు_శిఖరం, (resBottom-కీలకాలు_శిఖరం)*4, బహిర్గతం_Bundle+కీలకాలు_శిఖరం, p_Error_సంకేతం);
            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-కీలకాలు_శిఖరం));
                ఒకవేళ(తాత్కాలిక_పట్టిక.resFlags!=దొంతర_Res_Flags) {
                    uprv_free(తాత్కాలిక_పట్టిక.resFlags);
                }
                తిరిగి 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        తాత్కాలిక_పట్టిక.కీలకం_Chars=(స్థిరరాశి అక్షరం *)బహిర్గతం_Bundle; /* sort by outCharset */
        ఒకవేళ(తాత్కాలిక_పట్టిక.major_Format_సంస్కరణ>1 || max_పట్టిక_నిడివి<=దొంతర_అడ్డువరుస_సామర్థ్యం) {
            తాత్కాలిక_పట్టిక.అడ్డువరుసలు=అడ్డువరుసలు;
            తాత్కాలిక_పట్టిక.resort=resort;
        } లేకపోతే {
            తాత్కాలిక_పట్టిక.అడ్డువరుసలు=(అడ్డువరుస_2 *)uprv_malloc(max_పట్టిక_నిడివి*పరిమాణం(అడ్డువరుస_2)+max_పట్టిక_నిడివి*4);
            ఒకవేళ(తాత్కాలిక_పట్టిక.అడ్డువరుసలు==శూన్యసూచిక) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_పట్టిక_నిడివి);
                *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                ఒకవేళ(తాత్కాలిక_పట్టిక.resFlags!=దొంతర_Res_Flags) {
                    uprv_free(తాత్కాలిక_పట్టిక.resFlags);
                }
                తిరిగి 0;
            }
            తాత్కాలిక_పట్టిక.resort=(int32_t *)(తాత్కాలిక_పట్టిక.అడ్డువరుసలు+max_పట్టిక_నిడివి);
        }

        /* swap the resources */
        ures_మార్పిడి_Resource(ds, లోపల_Bundle, బహిర్గతం_Bundle, మూలం_Res, శూన్యసూచిక, &తాత్కాలిక_పట్టిక, p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             మూలం_Res);
        }

        ఒకవేళ(తాత్కాలిక_పట్టిక.అడ్డువరుసలు!=అడ్డువరుసలు) {
            uprv_free(తాత్కాలిక_పట్టిక.అడ్డువరుసలు);
        }
        ఒకవేళ(తాత్కాలిక_పట్టిక.resFlags!=దొంతర_Res_Flags) {
            uprv_free(తాత్కాలిక_పట్టిక.resFlags);
        }

        /* swap the root resource and indexes */
        ds->మార్పిడి_Array32(ds, లోపల_Bundle, కీలకాలు_Bottom*4, బహిర్గతం_Bundle, p_Error_సంకేతం);
    }

    తిరిగి శీర్షిక_కొలత+4*శిఖరం;
}

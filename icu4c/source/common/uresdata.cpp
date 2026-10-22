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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "resource.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucol_swp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresdata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RES_GET_KEY16(p_Res_ਅੰਕੜੇ, ਕੁੰਜੀ_ਵਿੱਥ) \
    ((ਕੁੰਜੀ_ਵਿੱਥ)<(p_Res_ਅੰਕੜੇ)->local_ਕੁੰਜੀ_ਹੱਦਬੰਦੀ ? \
        (ਅਚਲ ਅੱਖਰ *)(p_Res_ਅੰਕੜੇ)->p_ਜੜ੍ਹ+(ਕੁੰਜੀ_ਵਿੱਥ) : \
        (p_Res_ਅੰਕੜੇ)->pool_Bundle_ਕੁੰਜੀਆਂ+(ਕੁੰਜੀ_ਵਿੱਥ)-(p_Res_ਅੰਕੜੇ)->local_ਕੁੰਜੀ_ਹੱਦਬੰਦੀ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RES_GET_KEY32(p_Res_ਅੰਕੜੇ, ਕੁੰਜੀ_ਵਿੱਥ) \
    ((ਕੁੰਜੀ_ਵਿੱਥ)>=0 ? \
        (ਅਚਲ ਅੱਖਰ *)(p_Res_ਅੰਕੜੇ)->p_ਜੜ੍ਹ+(ਕੁੰਜੀ_ਵਿੱਥ) : \
        (p_Res_ਅੰਕੜੇ)->pool_Bundle_ਕੁੰਜੀਆਂ+((ਕੁੰਜੀ_ਵਿੱਥ)&0x7fffffff))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ URESDATA_ਵਸਤੂ_NOT_ਮਿਲਿਆ -1

/* empty resources, returned when the resource offset is 0 */
ਸਥਿਰ ਅਚਲ uint16_t gEmpty16=0;

ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

ਸਥਿਰ ਅਚਲ ਬਣਤਰ {
    int32_t length;
    char16_t nul;
    char16_t pad;
} g_ਖਾਲੀ_ਥਾਵਾਂ_String={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ਸਥਿਰ int32_t
res_ਲੱਭੋ_ਸਾਰਣੀ_ਵਸਤੂ(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, ਅਚਲ uint16_t *ਕੁੰਜੀ_Offsets, int32_t length,
                   ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ **real_ਕੁੰਜੀ) {
    ਅਚਲ ਅੱਖਰ *ਸਾਰਣੀ_ਕੁੰਜੀ;
    int32_t mid, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;
    ਪੂਰਨ ਨਤੀਜਾ;

    /* do a binary search for the key */
    ਸ਼ੁਰੂ=0;
    ਹੱਦਬੰਦੀ=length;
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
        mid = (ਸ਼ੁਰੂ + ਹੱਦਬੰਦੀ) / 2;
        ਸਾਰਣੀ_ਕੁੰਜੀ = RES_GET_KEY16(p_Res_ਅੰਕੜੇ, ਕੁੰਜੀ_Offsets[mid]);
        ਜੇ (p_Res_ਅੰਕੜੇ->useNativeStrcmp) {
            ਨਤੀਜਾ = uprv_strcmp(ਕੁੰਜੀ, ਸਾਰਣੀ_ਕੁੰਜੀ);
        } ਨਹੀਂਤਾਂ {
            ਨਤੀਜਾ = uprv_compareInvCharsAsAscii(ਕੁੰਜੀ, ਸਾਰਣੀ_ਕੁੰਜੀ);
        }
        ਜੇ (ਨਤੀਜਾ < 0) {
            ਹੱਦਬੰਦੀ = mid;
        } ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ > 0) {
            ਸ਼ੁਰੂ = mid + 1;
        } ਨਹੀਂਤਾਂ {
            /* We found it! */
            *real_ਕੁੰਜੀ=ਸਾਰਣੀ_ਕੁੰਜੀ;
            ਵਾਪਸ mid;
        }
    }
    ਵਾਪਸ URESDATA_ਵਸਤੂ_NOT_ਮਿਲਿਆ;  /* not found or table is empty. */
}

ਸਥਿਰ int32_t
res_ਲੱਭੋ_Table32_ਵਸਤੂ(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, ਅਚਲ int32_t *ਕੁੰਜੀ_Offsets, int32_t length,
                     ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ **real_ਕੁੰਜੀ) {
    ਅਚਲ ਅੱਖਰ *ਸਾਰਣੀ_ਕੁੰਜੀ;
    int32_t mid, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;
    ਪੂਰਨ ਨਤੀਜਾ;

    /* do a binary search for the key */
    ਸ਼ੁਰੂ=0;
    ਹੱਦਬੰਦੀ=length;
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
        mid = (ਸ਼ੁਰੂ + ਹੱਦਬੰਦੀ) / 2;
        ਸਾਰਣੀ_ਕੁੰਜੀ = RES_GET_KEY32(p_Res_ਅੰਕੜੇ, ਕੁੰਜੀ_Offsets[mid]);
        ਜੇ (p_Res_ਅੰਕੜੇ->useNativeStrcmp) {
            ਨਤੀਜਾ = uprv_strcmp(ਕੁੰਜੀ, ਸਾਰਣੀ_ਕੁੰਜੀ);
        } ਨਹੀਂਤਾਂ {
            ਨਤੀਜਾ = uprv_compareInvCharsAsAscii(ਕੁੰਜੀ, ਸਾਰਣੀ_ਕੁੰਜੀ);
        }
        ਜੇ (ਨਤੀਜਾ < 0) {
            ਹੱਦਬੰਦੀ = mid;
        } ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ > 0) {
            ਸ਼ੁਰੂ = mid + 1;
        } ਨਹੀਂਤਾਂ {
            /* We found it! */
            *real_ਕੁੰਜੀ=ਸਾਰਣੀ_ਕੁੰਜੀ;
            ਵਾਪਸ mid;
        }
    }
    ਵਾਪਸ URESDATA_ਵਸਤੂ_NOT_ਮਿਲਿਆ;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ਸਥਿਰ UBool U_CALLCONV
isAcceptable(ਖਾਲੀ *context,
             ਅਚਲ ਅੱਖਰ * /*type*/, ਅਚਲ ਅੱਖਰ * /*name*/,
             ਅਚਲ UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->format_ਸੰਸਕਰਣ, 4);
    ਵਾਪਸ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->ਅੰਕੜੇ_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x65 &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x73 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x42 &&
        (1<=pInfo->format_ਸੰਸਕਰਣ[0] && pInfo->format_ਸੰਸਕਰਣ[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

ਸਥਿਰ ਖਾਲੀ
res_init(Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ,
         UVersionInfo format_ਸੰਸਕਰਣ, ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਬਾਈਟਾਂ, int32_t length,
         UError_ਕੋਡ *error_ਕੋਡ) {
    UResType ਜੜ੍ਹ_Type;

    /* get the root resource */
    p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ = static_cast<ਅਚਲ int32_t*>(ਵਿੱਚ_ਬਾਈਟਾਂ);
    p_Res_ਅੰਕੜੇ->ਜੜ੍ਹ_Res = static_cast<Resource>(*p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ);
    p_Res_ਅੰਕੜੇ->p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    ਜੇ(length>=0 && (length/4)<((format_ਸੰਸਕਰਣ[0]==1 && format_ਸੰਸਕਰਣ[1]==0) ? 1 : 1+5)) {
        *error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_ਅੰਕੜੇ);
        ਵਾਪਸ;
    }

    /* currently, we accept only resources that have a Table as their roots */
    ਜੜ੍ਹ_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_ਅੰਕੜੇ->ਜੜ੍ਹ_Res));
    ਜੇ(!URES_IS_ਸਾਰਣੀ(ਜੜ੍ਹ_Type)) {
        *error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_ਅੰਕੜੇ);
        ਵਾਪਸ;
    }

    ਜੇ(format_ਸੰਸਕਰਣ[0]==1 && format_ਸੰਸਕਰਣ[1]==0) {
        p_Res_ਅੰਕੜੇ->local_ਕੁੰਜੀ_ਹੱਦਬੰਦੀ=0x10000;  /* greater than any 16-bit key string offset */
    } ਨਹੀਂਤਾਂ {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ਅਚਲ int32_t *indexes=p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+1;
        int32_t ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=indexes[URES_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ]&0xff;
        ਜੇ(ਸੂਚਕਾਂਕ_ਲੰਮਾਈ<=URES_ਸੂਚਕਾਂਕ_MAX_ਸਾਰਣੀ_ਲੰਮਾਈ) {
            *error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_ਅੰਕੜੇ);
            ਵਾਪਸ;
        }
        ਜੇ( length>=0 &&
            (length<((1+ਸੂਚਕਾਂਕ_ਲੰਮਾਈ)<<2) ||
             length<(indexes[URES_ਸੂਚਕਾਂਕ_BUNDLE_ਸਿਖਰ]<<2))
        ) {
            *error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_ਅੰਕੜੇ);
            ਵਾਪਸ;
        }
        ਜੇ(indexes[URES_ਸੂਚਕਾਂਕ_ਕੁੰਜੀਆਂ_ਸਿਖਰ]>(1+ਸੂਚਕਾਂਕ_ਲੰਮਾਈ)) {
            p_Res_ਅੰਕੜੇ->local_ਕੁੰਜੀ_ਹੱਦਬੰਦੀ=indexes[URES_ਸੂਚਕਾਂਕ_ਕੁੰਜੀਆਂ_ਸਿਖਰ]<<2;
        }
        ਜੇ(format_ਸੰਸਕਰਣ[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_ਅੰਕੜੇ->pool_String_ਸੂਚਕਾਂਕ_ਹੱਦਬੰਦੀ = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ]) >> 8);
        }
        ਜੇ(ਸੂਚਕਾਂਕ_ਲੰਮਾਈ>URES_ਸੂਚਕਾਂਕ_ਗੁਣ) {
            int32_t att=indexes[URES_ਸੂਚਕਾਂਕ_ਗੁਣ];
            p_Res_ਅੰਕੜੇ->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_ਅੰਕੜੇ->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_ਅੰਕੜੇ->ਵਰਤਦਾ_Pool_Bundle = static_cast<UBool>((att & URES_ATT_ਵਰਤਦਾ_POOL_BUNDLE) != 0);
            p_Res_ਅੰਕੜੇ->pool_String_ਸੂਚਕਾਂਕ_ਹੱਦਬੰਦੀ|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_ਅੰਕੜੇ->pool_String_Index16_ਹੱਦਬੰਦੀ = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        ਜੇ((p_Res_ਅੰਕੜੇ->isPoolBundle || p_Res_ਅੰਕੜੇ->ਵਰਤਦਾ_Pool_Bundle) && ਸੂਚਕਾਂਕ_ਲੰਮਾਈ<=URES_ਸੂਚਕਾਂਕ_POOL_CHECKSUM) {
            *error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_ਅੰਕੜੇ);
            ਵਾਪਸ;
        }
        ਜੇ( ਸੂਚਕਾਂਕ_ਲੰਮਾਈ>URES_ਸੂਚਕਾਂਕ_16_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸਿਖਰ &&
            indexes[URES_ਸੂਚਕਾਂਕ_16_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸਿਖਰ]>indexes[URES_ਸੂਚਕਾਂਕ_ਕੁੰਜੀਆਂ_ਸਿਖਰ]
        ) {
            p_Res_ਅੰਕੜੇ->p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units = reinterpret_cast<ਅਚਲ uint16_t*>(p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ + indexes[URES_ਸੂਚਕਾਂਕ_ਕੁੰਜੀਆਂ_ਸਿਖਰ]);
        }
    }

    ਜੇ(format_ਸੰਸਕਰਣ[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_ਅੰਕੜੇ->useNativeStrcmp=ਸੱਚ;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
res_read(Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ,
         ਅਚਲ UDataInfo *pInfo, ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਬਾਈਟਾਂ, int32_t length,
         UError_ਕੋਡ *error_ਕੋਡ) {
    UVersionInfo format_ਸੰਸਕਰਣ;

    uprv_memset(p_Res_ਅੰਕੜੇ, 0, ਆਕਾਰ(Resource_ਅੰਕੜੇ));
    ਜੇ(U_FAILURE(*error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ(!isAcceptable(format_ਸੰਸਕਰਣ, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, pInfo)) {
        *error_ਕੋਡ=U_INVALID_FORMAT_ERROR;
        ਵਾਪਸ;
    }
    res_init(p_Res_ਅੰਕੜੇ, format_ਸੰਸਕਰਣ, ਵਿੱਚ_ਬਾਈਟਾਂ, length, error_ਕੋਡ);
}

U_CFUNC ਖਾਲੀ
res_ਲੱਦੋ(Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ,
         ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, UError_ਕੋਡ *error_ਕੋਡ) {
    UVersionInfo format_ਸੰਸਕਰਣ;

    uprv_memset(p_Res_ਅੰਕੜੇ, 0, ਆਕਾਰ(Resource_ਅੰਕੜੇ));

    /* load the ResourceBundle file */
    p_Res_ਅੰਕੜੇ->ਅੰਕੜੇ=udata_openChoice(ਰਸਤਾ, "res", name, isAcceptable, format_ਸੰਸਕਰਣ, error_ਕੋਡ);
    ਜੇ(U_FAILURE(*error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_ਅੰਕੜੇ, format_ਸੰਸਕਰਣ, udata_get_ਸਮ੍ਰਿਤੀ(p_Res_ਅੰਕੜੇ->ਅੰਕੜੇ), -1, error_ਕੋਡ);
}

U_CFUNC ਖਾਲੀ
res_unload(Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ) {
    ਜੇ(p_Res_ਅੰਕੜੇ->ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ) {
        udata_close(p_Res_ਅੰਕੜੇ->ਅੰਕੜੇ);
        p_Res_ਅੰਕੜੇ->ਅੰਕੜੇ=ਖਾਲੀ_ਸੂਚਕ;
    }
}

ਸਥਿਰ ਅਚਲ int8_t g_ਜਨਤਕ_Types[URES_ਹੱਦਬੰਦੀ] = {
    URES_STRING,
    URES_ਦੁਆਧਾਰੀ,
    URES_ਸਾਰਣੀ,
    URES_ALIAS,

    URES_ਸਾਰਣੀ,     /* URES_TABLE32 */
    URES_ਸਾਰਣੀ,     /* URES_TABLE16 */
    URES_STRING,    /* URES_STRING_V2 */
    URES_INT,

    URES_ARRAY,
    URES_ARRAY,     /* URES_ARRAY16 */
    URES_NONE,
    URES_NONE,

    URES_NONE,
    URES_NONE,
    URES_INT_ਸਦਿਸ਼,
    URES_NONE
};

U_CAPI UResType U_EXPORT2
res_get_ਜਨਤਕ_Type(Resource res) {
    ਵਾਪਸ (UResType)g_ਜਨਤਕ_Types[RES_GET_TYPE(res)];
}

U_CAPI ਅਚਲ char16_t * U_EXPORT2
res_getStringNoTrace(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource res, int32_t *p_ਲੰਮਾਈ) {
    ਅਚਲ char16_t *p;
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(res);
    int32_t length;
    ਜੇ(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        ਜੇ((int32_t)ਵਿੱਥ<p_Res_ਅੰਕੜੇ->pool_String_ਸੂਚਕਾਂਕ_ਹੱਦਬੰਦੀ) {
            p=(ਅਚਲ char16_t *)p_Res_ਅੰਕੜੇ->pool_Bundle_ਲੜੀਆਂ+ਵਿੱਥ;
        } ਨਹੀਂਤਾਂ {
            p=(ਅਚਲ char16_t *)p_Res_ਅੰਕੜੇ->p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units+(ਵਿੱਥ-p_Res_ਅੰਕੜੇ->pool_String_ਸੂਚਕਾਂਕ_ਹੱਦਬੰਦੀ);
        }
        first=*p;
        ਜੇ(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } ਨਹੀਂਤਾਂ ਜੇ(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } ਨਹੀਂਤਾਂ ਜੇ(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } ਨਹੀਂਤਾਂ {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } ਨਹੀਂਤਾਂ ਜੇ(res==ਵਿੱਥ) /* RES_GET_TYPE(res)==URES_STRING */ {
        ਅਚਲ int32_t *p32= res==0 ? &g_ਖਾਲੀ_ਥਾਵਾਂ_String.length : p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+res;
        length=*p32++;
        p=(ਅਚਲ char16_t *)p32;
    } ਨਹੀਂਤਾਂ {
        p=ਖਾਲੀ_ਸੂਚਕ;
        length=0;
    }
    ਜੇ(p_ਲੰਮਾਈ) {
        *p_ਲੰਮਾਈ=length;
    }
    ਵਾਪਸ p;
}

ਨਾਮਾਕਾਸ਼ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource res) {
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(res);
    ਜੇ (ਵਿੱਥ == 0) {
        // empty string
    } ਨਹੀਂਤਾਂ ਜੇ (res == ਵਿੱਥ) {
        ਅਚਲ int32_t *p32=p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+res;
        int32_t length=*p32;
        ਅਚਲ char16_t* p = reinterpret_cast<ਅਚਲ char16_t*>(p32);
        ਵਾਪਸ length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } ਨਹੀਂਤਾਂ ਜੇ (RES_GET_TYPE(res) == URES_STRING_V2) {
        ਅਚਲ char16_t *p;
        ਜੇ (static_cast<int32_t>(ਵਿੱਥ) < p_Res_ਅੰਕੜੇ->pool_String_ਸੂਚਕਾਂਕ_ਹੱਦਬੰਦੀ) {
            p = reinterpret_cast<ਅਚਲ char16_t*>(p_Res_ਅੰਕੜੇ->pool_Bundle_ਲੜੀਆਂ) + ਵਿੱਥ;
        } ਨਹੀਂਤਾਂ {
            p = reinterpret_cast<ਅਚਲ char16_t*>(p_Res_ਅੰਕੜੇ->p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units) + (ਵਿੱਥ - p_Res_ਅੰਕੜੇ->pool_String_ਸੂਚਕਾਂਕ_ਹੱਦਬੰਦੀ);
        }
        int32_t first=*p;
        ਜੇ (first == 0x2205) {  // implicit length
            ਵਾਪਸ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } ਨਹੀਂਤਾਂ ਜੇ (first == 0xdc03) {  // explicit length 3 (should not occur)
            ਵਾਪਸ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } ਨਹੀਂਤਾਂ {
            // Assume that the string has not been stored with more length units than necessary.
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

int32_t getStringArray(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, ਅਚਲ icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest == ਖਾਲੀ_ਸੂਚਕ ? capacity != 0 : capacity < 0) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    int32_t length = array.get_ਮਾਪ();
    ਜੇ(length == 0) {
        ਵਾਪਸ 0;
    }
    ਜੇ(length > capacity) {
        error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ length;
    }
    ਲਈ(int32_t i = 0; i < length; ++i) {
        int32_t s_ਲੰਮਾਈ;
        // No tracing: handled by the caller
        ਅਚਲ char16_t *s = res_getStringNoTrace(p_Res_ਅੰਕੜੇ, array.ਅੰਦਰੂਨੀ_Get_Resource(p_Res_ਅੰਕੜੇ, i), &s_ਲੰਮਾਈ);
        ਜੇ(s == ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
            ਵਾਪਸ 0;
        }
        dest[i].ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਸੱਚ, s, s_ਲੰਮਾਈ);
    }
    ਵਾਪਸ length;
}

}  // namespace

U_CAPI ਅਚਲ char16_t * U_EXPORT2
res_getAlias(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource res, int32_t *p_ਲੰਮਾਈ) {
    ਅਚਲ char16_t *p;
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(res);
    int32_t length;
    ਜੇ(RES_GET_TYPE(res)==URES_ALIAS) {
        ਅਚਲ int32_t *p32= ਵਿੱਥ==0 ? &g_ਖਾਲੀ_ਥਾਵਾਂ_String.length : p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ;
        length=*p32++;
        p=(ਅਚਲ char16_t *)p32;
    } ਨਹੀਂਤਾਂ {
        p=ਖਾਲੀ_ਸੂਚਕ;
        length=0;
    }
    ਜੇ(p_ਲੰਮਾਈ) {
        *p_ਲੰਮਾਈ=length;
    }
    ਵਾਪਸ p;
}

U_CAPI ਅਚਲ uint8_t * U_EXPORT2
res_get_ਦੁਆਧਾਰੀ_No_Trace(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource res, int32_t *p_ਲੰਮਾਈ) {
    ਅਚਲ uint8_t *p;
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(res);
    int32_t length;
    ਜੇ(RES_GET_TYPE(res)==URES_ਦੁਆਧਾਰੀ) {
        ਅਚਲ int32_t *p32= ਵਿੱਥ==0 ? (ਅਚਲ int32_t*)&gEmpty32 : p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ;
        length=*p32++;
        p=(ਅਚਲ uint8_t *)p32;
    } ਨਹੀਂਤਾਂ {
        p=ਖਾਲੀ_ਸੂਚਕ;
        length=0;
    }
    ਜੇ(p_ਲੰਮਾਈ) {
        *p_ਲੰਮਾਈ=length;
    }
    ਵਾਪਸ p;
}


U_CAPI ਅਚਲ int32_t * U_EXPORT2
res_get_Int_ਸਦਿਸ਼_No_Trace(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource res, int32_t *p_ਲੰਮਾਈ) {
    ਅਚਲ int32_t *p;
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(res);
    int32_t length;
    ਜੇ(RES_GET_TYPE(res)==URES_INT_ਸਦਿਸ਼) {
        p= ਵਿੱਥ==0 ? (ਅਚਲ int32_t *)&gEmpty32 : p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ;
        length=*p++;
    } ਨਹੀਂਤਾਂ {
        p=ਖਾਲੀ_ਸੂਚਕ;
        length=0;
    }
    ਜੇ(p_ਲੰਮਾਈ) {
        *p_ਲੰਮਾਈ=length;
    }
    ਵਾਪਸ p;
}

U_CAPI int32_t U_EXPORT2
res_ਗਿਣਤੀ_Array_ਚੀਜ਼ਾਂ(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource res) {
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(res);
    ਚੋਣ(RES_GET_TYPE(res)) {
    ਹਾਲਤ URES_STRING:
    ਹਾਲਤ URES_STRING_V2:
    ਹਾਲਤ URES_ਦੁਆਧਾਰੀ:
    ਹਾਲਤ URES_ALIAS:
    ਹਾਲਤ URES_INT:
    ਹਾਲਤ URES_INT_ਸਦਿਸ਼:
        ਵਾਪਸ 1;
    ਹਾਲਤ URES_ARRAY:
    ਹਾਲਤ URES_TABLE32:
        ਵਾਪਸ ਵਿੱਥ==0 ? 0 : *(p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ);
    ਹਾਲਤ URES_ਸਾਰਣੀ:
        ਵਾਪਸ ਵਿੱਥ==0 ? 0 : *((ਅਚਲ uint16_t *)(p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ));
    ਹਾਲਤ URES_ARRAY16:
    ਹਾਲਤ URES_TABLE16:
        ਵਾਪਸ p_Res_ਅੰਕੜੇ->p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units[ਵਿੱਥ];
    ਮੂਲ:
        ਵਾਪਸ 0;
    }
}

U_NAMESPACE_BEGIN

Resource_ਅੰਕੜੇ_ਮੁੱਲ::~Resource_ਅੰਕੜੇ_ਮੁੱਲ() {}

UResType Resource_ਅੰਕੜੇ_ਮੁੱਲ::getType() ਅਚਲ {
    ਵਾਪਸ res_get_ਜਨਤਕ_Type(res);
}

ਅਚਲ char16_t *Resource_ਅੰਕੜੇ_ਮੁੱਲ::getString(int32_t &length, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ char16_t *s = res_getString(fTraceInfo, &get_ਅੰਕੜੇ(), res, &length);
    ਜੇ(s == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
    }
    ਵਾਪਸ s;
}

ਅਚਲ char16_t *Resource_ਅੰਕੜੇ_ਮੁੱਲ::getAliasString(int32_t &length, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ char16_t *s = res_getAlias(&get_ਅੰਕੜੇ(), res, &length);
    ਜੇ(s == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
    }
    ਵਾਪਸ s;
}

int32_t Resource_ਅੰਕੜੇ_ਮੁੱਲ::getInt(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(RES_GET_TYPE(res) != URES_INT) {
        error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
    }
    ਵਾਪਸ res_getInt(fTraceInfo, res);
}

uint32_t Resource_ਅੰਕੜੇ_ਮੁੱਲ::getUInt(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(RES_GET_TYPE(res) != URES_INT) {
        error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
    }
    ਵਾਪਸ res_getUInt(fTraceInfo, res);
}

ਅਚਲ int32_t *Resource_ਅੰਕੜੇ_ਮੁੱਲ::get_Int_ਸਦਿਸ਼(int32_t &length, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ int32_t *iv = res_get_Int_ਸਦਿਸ਼(fTraceInfo, &get_ਅੰਕੜੇ(), res, &length);
    ਜੇ(iv == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
    }
    ਵਾਪਸ iv;
}

ਅਚਲ uint8_t *Resource_ਅੰਕੜੇ_ਮੁੱਲ::get_ਦੁਆਧਾਰੀ(int32_t &length, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ uint8_t *b = res_get_ਦੁਆਧਾਰੀ(fTraceInfo, &get_ਅੰਕੜੇ(), res, &length);
    ਜੇ(b == ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
    }
    ਵਾਪਸ b;
}

ResourceArray Resource_ਅੰਕੜੇ_ਮੁੱਲ::getArray(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ {};
    }
    ਅਚਲ uint16_t *items16 = ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ Resource *items32 = ਖਾਲੀ_ਸੂਚਕ;
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(res);
    int32_t length = 0;
    ਚੋਣ(RES_GET_TYPE(res)) {
    ਹਾਲਤ URES_ARRAY:
        ਜੇ (ਵਿੱਥ!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ਅਚਲ Resource*>(get_ਅੰਕੜੇ().p_ਜੜ੍ਹ) + ਵਿੱਥ;
            length = *items32++;
        }
        ਰੋਕੋ;
    ਹਾਲਤ URES_ARRAY16:
        items16 = get_ਅੰਕੜੇ().p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units+ਵਿੱਥ;
        length = *items16++;
        ਰੋਕੋ;
    ਮੂਲ:
        error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
        ਵਾਪਸ {};
    }
    ਵਾਪਸ ResourceArray(items16, items32, length, fTraceInfo);
}

Resource_ਸਾਰਣੀ Resource_ਅੰਕੜੇ_ਮੁੱਲ::get_ਸਾਰਣੀ(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ {};
    }
    ਅਚਲ uint16_t *keys16 = ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ int32_t *keys32 = ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ uint16_t *items16 = ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ Resource *items32 = ਖਾਲੀ_ਸੂਚਕ;
    uint32_t ਵਿੱਥ = RES_GET_ਵਿੱਥ(res);
    int32_t length = 0;
    ਚੋਣ(RES_GET_TYPE(res)) {
    ਹਾਲਤ URES_ਸਾਰਣੀ:
        ਜੇ (ਵਿੱਥ != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ਅਚਲ uint16_t*>(get_ਅੰਕੜੇ().p_ਜੜ੍ਹ + ਵਿੱਥ);
            length = *keys16++;
            items32 = reinterpret_cast<ਅਚਲ Resource*>(keys16 + length + (~length & 1));
        }
        ਰੋਕੋ;
    ਹਾਲਤ URES_TABLE16:
        keys16 = get_ਅੰਕੜੇ().p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units+ਵਿੱਥ;
        length = *keys16++;
        items16 = keys16 + length;
        ਰੋਕੋ;
    ਹਾਲਤ URES_TABLE32:
        ਜੇ (ਵਿੱਥ != 0) {  // empty if offset==0
            keys32 = get_ਅੰਕੜੇ().p_ਜੜ੍ਹ+ਵਿੱਥ;
            length = *keys32++;
            items32 = reinterpret_cast<ਅਚਲ Resource*>(keys32) + length;
        }
        ਰੋਕੋ;
    ਮੂਲ:
        error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
        ਵਾਪਸ {};
    }
    ਵਾਪਸ Resource_ਸਾਰਣੀ(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_ਅੰਕੜੇ_ਮੁੱਲ::isNoInheritanceMarker() ਅਚਲ {
    ਵਾਪਸ ::isNoInheritanceMarker(&get_ਅੰਕੜੇ(), res);
}

int32_t Resource_ਅੰਕੜੇ_ਮੁੱਲ::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਵਾਪਸ ::getStringArray(&get_ਅੰਕੜੇ(), getArray(error_ਕੋਡ), dest, capacity, error_ਕੋਡ);
}

int32_t Resource_ਅੰਕੜੇ_ਮੁੱਲ::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ(URES_IS_ARRAY(res)) {
        ਵਾਪਸ ::getStringArray(&get_ਅੰਕੜੇ(), getArray(error_ਕੋਡ), dest, capacity, error_ਕੋਡ);
    }
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest == ਖਾਲੀ_ਸੂਚਕ ? capacity != 0 : capacity < 0) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    ਜੇ(capacity < 1) {
        error_ਕੋਡ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
        ਵਾਪਸ 1;
    }
    int32_t s_ਲੰਮਾਈ;
    ਅਚਲ char16_t *s = res_getString(fTraceInfo, &get_ਅੰਕੜੇ(), res, &s_ਲੰਮਾਈ);
    ਜੇ(s != ਖਾਲੀ_ਸੂਚਕ) {
        dest[0].ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਸੱਚ, s, s_ਲੰਮਾਈ);
        ਵਾਪਸ 1;
    }
    error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
    ਵਾਪਸ 0;
}

UnicodeString Resource_ਅੰਕੜੇ_ਮੁੱਲ::get_String_Or_ਪਹਿਲਾ_ਦਾ_Array(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    UnicodeString us;
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ us;
    }
    int32_t s_ਲੰਮਾਈ;
    ਅਚਲ char16_t *s = res_getString(fTraceInfo, &get_ਅੰਕੜੇ(), res, &s_ਲੰਮਾਈ);
    ਜੇ(s != ਖਾਲੀ_ਸੂਚਕ) {
        us.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਸੱਚ, s, s_ਲੰਮਾਈ);
        ਵਾਪਸ us;
    }
    ResourceArray array = getArray(error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ us;
    }
    ਜੇ(array.get_ਮਾਪ() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_ਅੰਕੜੇ(), array.ਅੰਦਰੂਨੀ_Get_Resource(&get_ਅੰਕੜੇ(), 0), &s_ਲੰਮਾਈ);
        ਜੇ(s != ਖਾਲੀ_ਸੂਚਕ) {
            us.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਸੱਚ, s, s_ਲੰਮਾਈ);
            ਵਾਪਸ us;
        }
    }
    error_ਕੋਡ = U_RESOURCE_TYPE_MISMATCH;
    ਵਾਪਸ us;
}

U_NAMESPACE_ਸਿਰਾ

ਸਥਿਰ Resource
makeResourceFrom16(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, int32_t res16) {
    ਜੇ(res16<p_Res_ਅੰਕੜੇ->pool_String_Index16_ਹੱਦਬੰਦੀ) {
        // Pool string, nothing to do.
    } ਨਹੀਂਤਾਂ {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_ਅੰਕੜੇ->pool_String_Index16_ਹੱਦਬੰਦੀ+p_Res_ਅੰਕੜੇ->pool_String_ਸੂਚਕਾਂਕ_ਹੱਦਬੰਦੀ;
    }
    ਵਾਪਸ URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource ਸਾਰਣੀ,
                      int32_t *ਸੂਚਕਾਂਕ_R, ਅਚਲ ਅੱਖਰ **ਕੁੰਜੀ) {
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(ਸਾਰਣੀ);
    int32_t length;
    int32_t idx;
    ਜੇ(ਕੁੰਜੀ == ਖਾਲੀ_ਸੂਚਕ || *ਕੁੰਜੀ == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ RES_BOGUS;
    }
    ਚੋਣ(RES_GET_TYPE(ਸਾਰਣੀ)) {
    ਹਾਲਤ URES_ਸਾਰਣੀ: {
        ਜੇ (ਵਿੱਥ!=0) { /* empty if offset==0 */
            ਅਚਲ uint16_t *p= (ਅਚਲ uint16_t *)(p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ);
            length=*p++;
            *ਸੂਚਕਾਂਕ_R=idx=res_ਲੱਭੋ_ਸਾਰਣੀ_ਵਸਤੂ(p_Res_ਅੰਕੜੇ, p, length, *ਕੁੰਜੀ, ਕੁੰਜੀ);
            ਜੇ(idx>=0) {
                ਅਚਲ Resource *p32=(ਅਚਲ Resource *)(p+length+(~length&1));
                ਵਾਪਸ p32[idx];
            }
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ URES_TABLE16: {
        ਅਚਲ uint16_t *p=p_Res_ਅੰਕੜੇ->p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units+ਵਿੱਥ;
        length=*p++;
        *ਸੂਚਕਾਂਕ_R=idx=res_ਲੱਭੋ_ਸਾਰਣੀ_ਵਸਤੂ(p_Res_ਅੰਕੜੇ, p, length, *ਕੁੰਜੀ, ਕੁੰਜੀ);
        ਜੇ(idx>=0) {
            ਵਾਪਸ makeResourceFrom16(p_Res_ਅੰਕੜੇ, p[length+idx]);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ URES_TABLE32: {
        ਜੇ (ਵਿੱਥ!=0) { /* empty if offset==0 */
            ਅਚਲ int32_t *p= p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ;
            length=*p++;
            *ਸੂਚਕਾਂਕ_R=idx=res_ਲੱਭੋ_Table32_ਵਸਤੂ(p_Res_ਅੰਕੜੇ, p, length, *ਕੁੰਜੀ, ਕੁੰਜੀ);
            ਜੇ(idx>=0) {
                ਵਾਪਸ (Resource)p[length+idx];
            }
        }
        ਰੋਕੋ;
    }
    ਮੂਲ:
        ਰੋਕੋ;
    }
    ਵਾਪਸ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਸੂਚਕਾਂਕ(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource ਸਾਰਣੀ,
                        int32_t ਸੂਚਕਾਂਕ_R, ਅਚਲ ਅੱਖਰ **ਕੁੰਜੀ) {
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(ਸਾਰਣੀ);
    int32_t length;
    ਜੇ (ਸੂਚਕਾਂਕ_R < 0) {
        ਵਾਪਸ RES_BOGUS;
    }
    ਚੋਣ(RES_GET_TYPE(ਸਾਰਣੀ)) {
    ਹਾਲਤ URES_ਸਾਰਣੀ: {
        ਜੇ (ਵਿੱਥ != 0) { /* empty if offset==0 */
            ਅਚਲ uint16_t *p= (ਅਚਲ uint16_t *)(p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ);
            length=*p++;
            ਜੇ(ਸੂਚਕਾਂਕ_R<length) {
                ਅਚਲ Resource *p32=(ਅਚਲ Resource *)(p+length+(~length&1));
                ਜੇ(ਕੁੰਜੀ!=ਖਾਲੀ_ਸੂਚਕ) {
                    *ਕੁੰਜੀ=RES_GET_KEY16(p_Res_ਅੰਕੜੇ, p[ਸੂਚਕਾਂਕ_R]);
                }
                ਵਾਪਸ p32[ਸੂਚਕਾਂਕ_R];
            }
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ URES_TABLE16: {
        ਅਚਲ uint16_t *p=p_Res_ਅੰਕੜੇ->p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units+ਵਿੱਥ;
        length=*p++;
        ਜੇ(ਸੂਚਕਾਂਕ_R<length) {
            ਜੇ(ਕੁੰਜੀ!=ਖਾਲੀ_ਸੂਚਕ) {
                *ਕੁੰਜੀ=RES_GET_KEY16(p_Res_ਅੰਕੜੇ, p[ਸੂਚਕਾਂਕ_R]);
            }
            ਵਾਪਸ makeResourceFrom16(p_Res_ਅੰਕੜੇ, p[length+ਸੂਚਕਾਂਕ_R]);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ URES_TABLE32: {
        ਜੇ (ਵਿੱਥ != 0) { /* empty if offset==0 */
            ਅਚਲ int32_t *p= p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ;
            length=*p++;
            ਜੇ(ਸੂਚਕਾਂਕ_R<length) {
                ਜੇ(ਕੁੰਜੀ!=ਖਾਲੀ_ਸੂਚਕ) {
                    *ਕੁੰਜੀ=RES_GET_KEY32(p_Res_ਅੰਕੜੇ, p[ਸੂਚਕਾਂਕ_R]);
                }
                ਵਾਪਸ (Resource)p[length+ਸੂਚਕਾਂਕ_R];
            }
        }
        ਰੋਕੋ;
    }
    ਮੂਲ:
        ਰੋਕੋ;
    }
    ਵਾਪਸ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ) {
    ਅਚਲ ਅੱਖਰ *real_ਕੁੰਜੀ=ਕੁੰਜੀ;
    int32_t idx;
    ਵਾਪਸ res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ(p_Res_ਅੰਕੜੇ, p_Res_ਅੰਕੜੇ->ਜੜ੍ਹ_Res, &idx, &real_ਕੁੰਜੀ);
}


UBool icu::Resource_ਸਾਰਣੀ::get_ਕੁੰਜੀ_And_ਮੁੱਲ(int32_t i,
                                         ਅਚਲ ਅੱਖਰ *&ਕੁੰਜੀ, icu::Resource_ਮੁੱਲ &value) ਅਚਲ {
    ਜੇ(0 <= i && i < length) {
        icu::Resource_ਅੰਕੜੇ_ਮੁੱਲ &rd_ਮੁੱਲ = static_cast<icu::Resource_ਅੰਕੜੇ_ਮੁੱਲ &>(value);
        ਜੇ (keys16 != ਖਾਲੀ_ਸੂਚਕ) {
            ਕੁੰਜੀ = RES_GET_KEY16(&rd_ਮੁੱਲ.get_ਅੰਕੜੇ(), keys16[i]);
        } ਨਹੀਂਤਾਂ {
            ਕੁੰਜੀ = RES_GET_KEY32(&rd_ਮੁੱਲ.get_ਅੰਕੜੇ(), keys32[i]);
        }
        Resource res;
        ਜੇ (items16 != ਖਾਲੀ_ਸੂਚਕ) {
            res = makeResourceFrom16(&rd_ਮੁੱਲ.get_ਅੰਕੜੇ(), items16[i]);
        } ਨਹੀਂਤਾਂ {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ਮੁੱਲ.ਸੈੱਟ_Resource(res, ResourceTracer(fTraceInfo, ਕੁੰਜੀ));
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ ਝੂਠ;
}

UBool icu::Resource_ਸਾਰਣੀ::ਲੱਭੋ_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, Resource_ਮੁੱਲ &value) ਅਚਲ {
    icu::Resource_ਅੰਕੜੇ_ਮੁੱਲ &rd_ਮੁੱਲ = static_cast<icu::Resource_ਅੰਕੜੇ_ਮੁੱਲ &>(value);
    ਅਚਲ ਅੱਖਰ *real_ਕੁੰਜੀ = ਖਾਲੀ_ਸੂਚਕ;
    int32_t i;
    ਜੇ (keys16 != ਖਾਲੀ_ਸੂਚਕ) {
        i = res_ਲੱਭੋ_ਸਾਰਣੀ_ਵਸਤੂ(&rd_ਮੁੱਲ.get_ਅੰਕੜੇ(), keys16, length, ਕੁੰਜੀ, &real_ਕੁੰਜੀ);
    } ਨਹੀਂਤਾਂ {
        i = res_ਲੱਭੋ_Table32_ਵਸਤੂ(&rd_ਮੁੱਲ.get_ਅੰਕੜੇ(), keys32, length, ਕੁੰਜੀ, &real_ਕੁੰਜੀ);
    }
    ਜੇ (i >= 0) {
        Resource res;
        ਜੇ (items16 != ਖਾਲੀ_ਸੂਚਕ) {
            res = makeResourceFrom16(&rd_ਮੁੱਲ.get_ਅੰਕੜੇ(), items16[i]);
        } ਨਹੀਂਤਾਂ {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_ਮੁੱਲ.ਸੈੱਟ_Resource(res, ResourceTracer(fTraceInfo, ਕੁੰਜੀ));
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ ਝੂਠ;
}

U_CAPI Resource U_EXPORT2
res_get_Array_ਵਸਤੂ(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource array, int32_t ਸੂਚਕਾਂਕ_R) {
    uint32_t ਵਿੱਥ=RES_GET_ਵਿੱਥ(array);
    ਜੇ (ਸੂਚਕਾਂਕ_R < 0) {
        ਵਾਪਸ RES_BOGUS;
    }
    ਚੋਣ(RES_GET_TYPE(array)) {
    ਹਾਲਤ URES_ARRAY: {
        ਜੇ (ਵਿੱਥ!=0) { /* empty if offset==0 */
            ਅਚਲ int32_t *p= p_Res_ਅੰਕੜੇ->p_ਜੜ੍ਹ+ਵਿੱਥ;
            ਜੇ(ਸੂਚਕਾਂਕ_R<*p) {
                ਵਾਪਸ (Resource)p[1+ਸੂਚਕਾਂਕ_R];
            }
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ URES_ARRAY16: {
        ਅਚਲ uint16_t *p=p_Res_ਅੰਕੜੇ->p16_ਦੋਆਧਾਰੀ_ਅੰਕ_Units+ਵਿੱਥ;
        ਜੇ(ਸੂਚਕਾਂਕ_R<*p) {
            ਵਾਪਸ makeResourceFrom16(p_Res_ਅੰਕੜੇ, p[1+ਸੂਚਕਾਂਕ_R]);
        }
        ਰੋਕੋ;
    }
    ਮੂਲ:
        ਰੋਕੋ;
    }
    ਵਾਪਸ RES_BOGUS;
}

uint32_t icu::ResourceArray::ਅੰਦਰੂਨੀ_Get_Resource(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, int32_t i) ਅਚਲ {
    ਜੇ (items16 != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ makeResourceFrom16(p_Res_ਅੰਕੜੇ, items16[i]);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ items32[i];
    }
}

UBool icu::ResourceArray::get_ਮੁੱਲ(int32_t i, icu::Resource_ਮੁੱਲ &value) ਅਚਲ {
    ਜੇ(0 <= i && i < length) {
        icu::Resource_ਅੰਕੜੇ_ਮੁੱਲ &rd_ਮੁੱਲ = static_cast<icu::Resource_ਅੰਕੜੇ_ਮੁੱਲ &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ਮੁੱਲ.ਸੈੱਟ_Resource(
            ਅੰਦਰੂਨੀ_Get_Resource(&rd_ਮੁੱਲ.get_ਅੰਕੜੇ(), i),
            ResourceTracer(fTraceInfo, i));
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ ਝੂਠ;
}

U_CFUNC Resource
res_ਲੱਭੋ_Resource(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, Resource r, ਅੱਖਰ** ਰਸਤਾ, ਅਚਲ ਅੱਖਰ** ਕੁੰਜੀ) {
  ਅੱਖਰ *ਰਸਤਾ_P = *ਰਸਤਾ, *ਅਗਲਾ_Sep_P = *ਰਸਤਾ;
  ਅੱਖਰ *close_ਸੂਚਕਾਂਕ = ਖਾਲੀ_ਸੂਚਕ;
  Resource t1 = r;
  Resource t2;
  int32_t ਸੂਚਕਾਂਕ_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  ਜੇ(!uprv_strlen(ਰਸਤਾ_P)) {
      ਵਾਪਸ r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  ਜੇ(!URES_IS_CONTAINER(type)) {
      ਵਾਪਸ RES_BOGUS;
  }
  
  ਜਦੋਂ(ਅਗਲਾ_Sep_P && *ਰਸਤਾ_P && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    ਅਗਲਾ_Sep_P = uprv_strchr(ਰਸਤਾ_P, RES_ਰਸਤਾ_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    ਜੇ(ਅਗਲਾ_Sep_P != ਖਾਲੀ_ਸੂਚਕ) {
      ਜੇ(ਅਗਲਾ_Sep_P == ਰਸਤਾ_P) {
        // Empty key string.
        ਵਾਪਸ RES_BOGUS;
      }
      *ਅਗਲਾ_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *ਰਸਤਾ = ਅਗਲਾ_Sep_P+1;
    } ਨਹੀਂਤਾਂ {
      *ਰਸਤਾ = uprv_strchr(ਰਸਤਾ_P, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    ਜੇ(URES_IS_ਸਾਰਣੀ(type)) {
      *ਕੁੰਜੀ = ਰਸਤਾ_P;
      t2 = res_get_ਸਾਰਣੀ_ਵਸਤੂ_By_ਕੁੰਜੀ(p_Res_ਅੰਕੜੇ, t1, &ਸੂਚਕਾਂਕ_R, ਕੁੰਜੀ);
    } ਨਹੀਂਤਾਂ ਜੇ(URES_IS_ARRAY(type)) {
      ਸੂਚਕਾਂਕ_R = uprv_strtol(ਰਸਤਾ_P, &close_ਸੂਚਕਾਂਕ, 10);
      ਜੇ(ਸੂਚਕਾਂਕ_R >= 0 && *close_ਸੂਚਕਾਂਕ == 0) {
        t2 = res_get_Array_ਵਸਤੂ(p_Res_ਅੰਕੜੇ, t1, ਸੂਚਕਾਂਕ_R);
      } ਨਹੀਂਤਾਂ {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *ਕੁੰਜੀ = ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    ਰਸਤਾ_P = *ਰਸਤਾ;
  }

  ਵਾਪਸ t1;
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਪੰਕਤੀ_2 {
    int32_t ਕੁੰਜੀ_ਸੂਚਕਾਂਕ, ਛਾਂਟੋ_ਸੂਚਕਾਂਕ;
} ਪੰਕਤੀ_2;

ਸਥਿਰ int32_t U_CALLCONV
ures_compare_ਪੰਕਤੀਆਂ(ਅਚਲ ਖਾਲੀ *context, ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ_Chars = static_cast<ਅਚਲ ਅੱਖਰ*>(context);
    ਵਾਪਸ static_cast<int32_t>(uprv_strcmp(ਕੁੰਜੀ_Chars + static_cast<ਅਚਲ ਪੰਕਤੀ_2*>(ਖੱਬੇ)->ਕੁੰਜੀ_ਸੂਚਕਾਂਕ,
                                            ਕੁੰਜੀ_Chars + static_cast<ਅਚਲ ਪੰਕਤੀ_2*>(ਸੱਜੇ)->ਕੁੰਜੀ_ਸੂਚਕਾਂਕ));
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਆਰਜ਼ੀ_ਸਾਰਣੀ_2 {
    ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_Chars;
    ਪੰਕਤੀ_2 *ਪੰਕਤੀਆਂ;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_ਕੁੰਜੀ_ਹੱਦਬੰਦੀ;
    uint8_t major_Format_ਸੰਸਕਰਣ;
} ਆਰਜ਼ੀ_ਸਾਰਣੀ_2;

ਗਿਣਤੀ {
    ਢੇਰ_ਪੰਕਤੀ_ਸਮਰੱਥਾ=200
};

/* The table item key string is not locally available. */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ g_Unknown_ਕੁੰਜੀ="";

#ਜੇ !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ਸਥਿਰ ਅਚਲ char16_t g_Collation_Bin_ਕੁੰਜੀ[]=u"%%CollationBin";
#ਜੇ_ਅੰਤ

/*
 * swap one resource item
 */
ਸਥਿਰ ਖਾਲੀ
ures_ਅਦਲਾ_ਬਦਲੀ_Resource(ਅਚਲ UDataSwapper *ds,
                  ਅਚਲ Resource *ਵਿੱਚ_Bundle, Resource *ਬਾਹਰਲਾ_Bundle,
                  Resource res, /* caller swaps res itself */
                  ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ,
                  ਆਰਜ਼ੀ_ਸਾਰਣੀ_2 *p_ਆਰਜ਼ੀ_ਸਾਰਣੀ,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ Resource *p;
    Resource *q;
    int32_t ਵਿੱਥ, count;

    ਚੋਣ(RES_GET_TYPE(res)) {
    ਹਾਲਤ URES_TABLE16:
    ਹਾਲਤ URES_STRING_V2:
    ਹਾਲਤ URES_INT:
    ਹਾਲਤ URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        ਵਾਪਸ;
    ਮੂਲ:
        ਰੋਕੋ;
    }

    /* all other types use an offset to point to their data */
    ਵਿੱਥ = static_cast<int32_t>(RES_GET_ਵਿੱਥ(res));
    ਜੇ(ਵਿੱਥ==0) {
        /* special offset indicating an empty item */
        ਵਾਪਸ;
    }
    ਜੇ (p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->resFlags[ਵਿੱਥ >> 5] & (static_cast<uint32_t>(1) << (ਵਿੱਥ & 0x1f))) {
        /* we already swapped this resource item */
        ਵਾਪਸ;
    } ਨਹੀਂਤਾਂ {
        /* mark it as swapped now */
        p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->resFlags[ਵਿੱਥ >> 5] |= static_cast<uint32_t>(1) << (ਵਿੱਥ & 0x1f);
    }

    p=ਵਿੱਚ_Bundle+ਵਿੱਥ;
    q=ਬਾਹਰਲਾ_Bundle+ਵਿੱਥ;

    ਚੋਣ(RES_GET_TYPE(res)) {
    ਹਾਲਤ URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    ਹਾਲਤ URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, p, 4, q, p_Error_ਕੋਡ);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, p+1, 2*count, q+1, p_Error_ਕੋਡ);
        ਰੋਕੋ;
    ਹਾਲਤ URES_ਦੁਆਧਾਰੀ:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, p, 4, q, p_Error_ਕੋਡ);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#ਜੇ !UCONFIG_NO_COLLATION
        ਜੇ( ਕੁੰਜੀ!=ਖਾਲੀ_ਸੂਚਕ &&  /* the binary is in a table */
            (ਕੁੰਜੀ!=g_Unknown_ਕੁੰਜੀ ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, ਕੁੰਜੀ, -1,
                                       g_Collation_Bin_ਕੁੰਜੀ, UPRV_LENGTHOF(g_Collation_Bin_ਕੁੰਜੀ)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_ਦੁਆਧਾਰੀ(ds, p+1, count))
        ) {
            ucol_ਅਦਲਾ_ਬਦਲੀ(ds, p+1, count, q+1, p_Error_ਕੋਡ);
        }
#ਜੇ_ਅੰਤ
        ਰੋਕੋ;
    ਹਾਲਤ URES_ਸਾਰਣੀ:
    ਹਾਲਤ URES_TABLE32:
        {
            ਅਚਲ uint16_t *pKey16;
            uint16_t *qKey16;

            ਅਚਲ int32_t *pKey32;
            int32_t *qKey32;

            Resource ਵਸਤੂ;
            int32_t i, old_ਸੂਚਕਾਂਕ;

            ਜੇ(RES_GET_TYPE(res)==URES_ਸਾਰਣੀ) {
                /* get table item count */
                pKey16 = reinterpret_cast<ਅਚਲ uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=ਖਾਲੀ_ਸੂਚਕ;

                /* swap count */
                ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, pKey16++, 2, qKey16++, p_Error_ਕੋਡ);

                ਵਿੱਥ+=((1+count)+1)/2;
            } ਨਹੀਂਤਾਂ {
                /* get table item count */
                pKey32 = reinterpret_cast<ਅਚਲ int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=ਖਾਲੀ_ਸੂਚਕ;

                /* swap count */
                ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, pKey32++, 4, qKey32++, p_Error_ਕੋਡ);

                ਵਿੱਥ+=1+count;
            }

            ਜੇ(count==0) {
                ਰੋਕੋ;
            }

            p=ਵਿੱਚ_Bundle+ਵਿੱਥ; /* pointer to table resources */
            q=ਬਾਹਰਲਾ_Bundle+ਵਿੱਥ;

            /* recurse */
            ਲਈ(i=0; i<count; ++i) {
                ਅਚਲ ਅੱਖਰ *ਵਸਤੂ_ਕੁੰਜੀ=g_Unknown_ਕੁੰਜੀ;
                ਜੇ(pKey16!=ਖਾਲੀ_ਸੂਚਕ) {
                    int32_t ਕੁੰਜੀ_ਵਿੱਥ=ds->readUInt16(pKey16[i]);
                    ਜੇ(ਕੁੰਜੀ_ਵਿੱਥ<p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->local_ਕੁੰਜੀ_ਹੱਦਬੰਦੀ) {
                        ਵਸਤੂ_ਕੁੰਜੀ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਬਾਹਰਲਾ_Bundle) + ਕੁੰਜੀ_ਵਿੱਥ;
                    }
                } ਨਹੀਂਤਾਂ {
                    int32_t ਕੁੰਜੀ_ਵਿੱਥ=udata_readInt32(ds, pKey32[i]);
                    ਜੇ(ਕੁੰਜੀ_ਵਿੱਥ>=0) {
                        ਵਸਤੂ_ਕੁੰਜੀ = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(ਬਾਹਰਲਾ_Bundle) + ਕੁੰਜੀ_ਵਿੱਥ;
                    }
                }
                ਵਸਤੂ=ds->readUInt32(p[i]);
                ures_ਅਦਲਾ_ਬਦਲੀ_Resource(ds, ਵਿੱਚ_Bundle, ਬਾਹਰਲਾ_Bundle, ਵਸਤੂ, ਵਸਤੂ_ਕੁੰਜੀ, p_ਆਰਜ਼ੀ_ਸਾਰਣੀ, p_Error_ਕੋਡ);
                ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, ਵਸਤੂ);
                    ਵਾਪਸ;
                }
            }

            ਜੇ(p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->major_Format_ਸੰਸਕਰਣ>1 || ds->ਵਿੱਚ_Charset==ds->ਬਾਹਰਲਾ_Charset) {
                /* no need to sort, just swap the offset/value arrays */
                ਜੇ(pKey16!=ਖਾਲੀ_ਸੂਚਕ) {
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, pKey16, count*2, qKey16, p_Error_ਕੋਡ);
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, p, count*4, q, p_Error_ਕੋਡ);
                } ਨਹੀਂਤਾਂ {
                    /* swap key offsets and items as one array */
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, pKey32, count*2*4, qKey32, p_Error_ਕੋਡ);
                }
                ਰੋਕੋ;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            ਜੇ(pKey16!=ਖਾਲੀ_ਸੂਚਕ) {
                ਲਈ(i=0; i<count; ++i) {
                    p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->ਪੰਕਤੀਆਂ[i].ਕੁੰਜੀ_ਸੂਚਕਾਂਕ=ds->readUInt16(pKey16[i]);
                    p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->ਪੰਕਤੀਆਂ[i].ਛਾਂਟੋ_ਸੂਚਕਾਂਕ=i;
                }
            } ਨਹੀਂਤਾਂ {
                ਲਈ(i=0; i<count; ++i) {
                    p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->ਪੰਕਤੀਆਂ[i].ਕੁੰਜੀ_ਸੂਚਕਾਂਕ=udata_readInt32(ds, pKey32[i]);
                    p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->ਪੰਕਤੀਆਂ[i].ਛਾਂਟੋ_ਸੂਚਕਾਂਕ=i;
                }
            }
            uprv_ਛਾਂਟੋ_Array(p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->ਪੰਕਤੀਆਂ, count, ਆਕਾਰ(ਪੰਕਤੀ_2),
                           ures_compare_ਪੰਕਤੀਆਂ, p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->ਕੁੰਜੀ_Chars,
                           ਝੂਠ, p_Error_ਕੋਡ);
            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                ਵਾਪਸ;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*ਕੁੰਜੀਆਂ*/
            ਜੇ(pKey16!=ਖਾਲੀ_ਸੂਚਕ) {
                uint16_t *rKey16;

                ਜੇ(pKey16!=qKey16) {
                    rKey16=qKey16;
                } ਨਹੀਂਤਾਂ {
                    rKey16 = reinterpret_cast<uint16_t*>(p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->resort);
                }
                ਲਈ(i=0; i<count; ++i) {
                    old_ਸੂਚਕਾਂਕ=p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->ਪੰਕਤੀਆਂ[i].ਛਾਂਟੋ_ਸੂਚਕਾਂਕ;
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, pKey16+old_ਸੂਚਕਾਂਕ, 2, rKey16+i, p_Error_ਕੋਡ);
                }
                ਜੇ(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } ਨਹੀਂਤਾਂ {
                int32_t *rKey32;

                ਜੇ(pKey32!=qKey32) {
                    rKey32=qKey32;
                } ਨਹੀਂਤਾਂ {
                    rKey32=p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->resort;
                }
                ਲਈ(i=0; i<count; ++i) {
                    old_ਸੂਚਕਾਂਕ=p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->ਪੰਕਤੀਆਂ[i].ਛਾਂਟੋ_ਸੂਚਕਾਂਕ;
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, pKey32+old_ਸੂਚਕਾਂਕ, 4, rKey32+i, p_Error_ਕੋਡ);
                }
                ਜੇ(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                ਜੇ(p!=q) {
                    r=q;
                } ਨਹੀਂਤਾਂ {
                    r = reinterpret_cast<Resource*>(p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->resort);
                }
                ਲਈ(i=0; i<count; ++i) {
                    old_ਸੂਚਕਾਂਕ=p_ਆਰਜ਼ੀ_ਸਾਰਣੀ->ਪੰਕਤੀਆਂ[i].ਛਾਂਟੋ_ਸੂਚਕਾਂਕ;
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, p+old_ਸੂਚਕਾਂਕ, 4, r+i, p_Error_ਕੋਡ);
                }
                ਜੇ(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        ਰੋਕੋ;
    ਹਾਲਤ URES_ARRAY:
        {
            Resource ਵਸਤੂ;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, p++, 4, q++, p_Error_ਕੋਡ);

            /* recurse */
            ਲਈ(i=0; i<count; ++i) {
                ਵਸਤੂ=ds->readUInt32(p[i]);
                ures_ਅਦਲਾ_ਬਦਲੀ_Resource(ds, ਵਿੱਚ_Bundle, ਬਾਹਰਲਾ_Bundle, ਵਸਤੂ, ਖਾਲੀ_ਸੂਚਕ, p_ਆਰਜ਼ੀ_ਸਾਰਣੀ, p_Error_ਕੋਡ);
                ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, ਵਸਤੂ);
                    ਵਾਪਸ;
                }
            }

            /* swap items */
            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, p, 4*count, q, p_Error_ਕੋਡ);
        }
        ਰੋਕੋ;
    ਹਾਲਤ URES_INT_ਸਦਿਸ਼:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, p, 4*(1+count), q, p_Error_ਕੋਡ);
        ਰੋਕੋ;
    ਮੂਲ:
        /* also catches RES_BOGUS */
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਰੋਕੋ;
    }
}

U_CAPI int32_t U_EXPORT2
ures_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
          ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UDataInfo *pInfo;
    ਅਚਲ Resource *ਵਿੱਚ_Bundle;
    Resource ਜੜ੍ਹ_Res;
    int32_t ਸਿਰਲੇਖ_ਮਾਪ, max_ਸਾਰਣੀ_ਲੰਮਾਈ;

    ਪੰਕਤੀ_2 ਪੰਕਤੀਆਂ[ਢੇਰ_ਪੰਕਤੀ_ਸਮਰੱਥਾ];
    int32_t resort[ਢੇਰ_ਪੰਕਤੀ_ਸਮਰੱਥਾ];
    ਆਰਜ਼ੀ_ਸਾਰਣੀ_2 ਆਰਜ਼ੀ_ਸਾਰਣੀ;

    ਅਚਲ int32_t *ਵਿੱਚ_Indexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_ਲੰਮਾਈ, ਸੂਚਕਾਂਕ_ਲੰਮਾਈ, ਕੁੰਜੀਆਂ_Bottom, ਕੁੰਜੀਆਂ_ਸਿਖਰ, resBottom, ਸਿਖਰ;

    /* udata_swapDataHeader checks the arguments */
    ਸਿਰਲੇਖ_ਮਾਪ=udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* check data format and format version */
    pInfo=(ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
    ਜੇ(!(
        pInfo->ਅੰਕੜੇ_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x65 &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x73 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->format_ਸੰਸਕਰਣ[0]==1 && pInfo->format_ਸੰਸਕਰਣ[1]>=1) ||
            pInfo->format_ਸੰਸਕਰਣ[0]==2 || pInfo->format_ਸੰਸਕਰਣ[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1],
                         pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3],
                         pInfo->format_ਸੰਸਕਰਣ[0], pInfo->format_ਸੰਸਕਰਣ[1]);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }
    ਆਰਜ਼ੀ_ਸਾਰਣੀ.major_Format_ਸੰਸਕਰਣ=pInfo->format_ਸੰਸਕਰਣ[0];

    /* a resource bundle must contain at least one resource item */
    ਜੇ(length<0) {
        bundle_ਲੰਮਾਈ=-1;
    } ਨਹੀਂਤਾਂ {
        bundle_ਲੰਮਾਈ=(length-ਸਿਰਲੇਖ_ਮਾਪ)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        ਜੇ(bundle_ਲੰਮਾਈ<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-ਸਿਰਲੇਖ_ਮਾਪ);
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
    }

    ਵਿੱਚ_Bundle=(ਅਚਲ Resource *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ);
    ਜੜ੍ਹ_Res=ds->readUInt32(*ਵਿੱਚ_Bundle);

    /* formatVersion 1.1 adds the indexes[] array */
    ਵਿੱਚ_Indexes=(ਅਚਲ int32_t *)(ਵਿੱਚ_Bundle+1);

    ਸੂਚਕਾਂਕ_ਲੰਮਾਈ=udata_readInt32(ds, ਵਿੱਚ_Indexes[URES_ਸੂਚਕਾਂਕ_ਲੰਮਾਈ])&0xff;
    ਜੇ(ਸੂਚਕਾਂਕ_ਲੰਮਾਈ<=URES_ਸੂਚਕਾਂਕ_MAX_ਸਾਰਣੀ_ਲੰਮਾਈ) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }
    ਕੁੰਜੀਆਂ_Bottom=1+ਸੂਚਕਾਂਕ_ਲੰਮਾਈ;
    ਕੁੰਜੀਆਂ_ਸਿਖਰ=udata_readInt32(ds, ਵਿੱਚ_Indexes[URES_ਸੂਚਕਾਂਕ_ਕੁੰਜੀਆਂ_ਸਿਖਰ]);
    ਜੇ(ਸੂਚਕਾਂਕ_ਲੰਮਾਈ>URES_ਸੂਚਕਾਂਕ_16_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸਿਖਰ) {
        resBottom=udata_readInt32(ds, ਵਿੱਚ_Indexes[URES_ਸੂਚਕਾਂਕ_16_ਦੋਆਧਾਰੀ_ਅੰਕ_ਸਿਖਰ]);
    } ਨਹੀਂਤਾਂ {
        resBottom=ਕੁੰਜੀਆਂ_ਸਿਖਰ;
    }
    ਸਿਖਰ=udata_readInt32(ds, ਵਿੱਚ_Indexes[URES_ਸੂਚਕਾਂਕ_BUNDLE_ਸਿਖਰ]);
    max_ਸਾਰਣੀ_ਲੰਮਾਈ=udata_readInt32(ds, ਵਿੱਚ_Indexes[URES_ਸੂਚਕਾਂਕ_MAX_ਸਾਰਣੀ_ਲੰਮਾਈ]);

    ਜੇ(0<=bundle_ਲੰਮਾਈ && bundle_ਲੰਮਾਈ<ਸਿਖਰ) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         ਸਿਖਰ, bundle_ਲੰਮਾਈ);
        *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
        ਵਾਪਸ 0;
    }
    ਜੇ(ਕੁੰਜੀਆਂ_ਸਿਖਰ>(1+ਸੂਚਕਾਂਕ_ਲੰਮਾਈ)) {
        ਆਰਜ਼ੀ_ਸਾਰਣੀ.local_ਕੁੰਜੀ_ਹੱਦਬੰਦੀ=ਕੁੰਜੀਆਂ_ਸਿਖਰ<<2;
    } ਨਹੀਂਤਾਂ {
        ਆਰਜ਼ੀ_ਸਾਰਣੀ.local_ਕੁੰਜੀ_ਹੱਦਬੰਦੀ=0;
    }

    ਜੇ(length>=0) {
        Resource *ਬਾਹਰਲਾ_Bundle=(Resource *)((ਅੱਖਰ *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ);

        /* track which resources we have already swapped */
        uint32_t ਢੇਰ_Res_Flags[ਢੇਰ_ਪੰਕਤੀ_ਸਮਰੱਥਾ];
        int32_t res_Flags_ਲੰਮਾਈ;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_ਲੰਮਾਈ=(length+31)>>5;          /* number of bytes needed */
        res_Flags_ਲੰਮਾਈ=(res_Flags_ਲੰਮਾਈ+3)&~3;   /* multiple of 4 bytes for uint32_t */
        ਜੇ(res_Flags_ਲੰਮਾਈ<=(int32_t)ਆਕਾਰ(ਢੇਰ_Res_Flags)) {
            ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags=ਢੇਰ_Res_Flags;
        } ਨਹੀਂਤਾਂ {
            ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags=(uint32_t *)uprv_malloc(res_Flags_ਲੰਮਾਈ);
            ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags==ਖਾਲੀ_ਸੂਚਕ) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ 0;
            }
        }
        uprv_memset(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags, 0, res_Flags_ਲੰਮਾਈ);

        /* copy the bundle for binary and inaccessible data */
        ਜੇ(ਵਿੱਚ_ਅੰਕੜੇ!=ਬਾਹਰਲਾ_ਅੰਕੜੇ) {
            uprv_memcpy(ਬਾਹਰਲਾ_Bundle, ਵਿੱਚ_Bundle, 4*ਸਿਖਰ);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_ਅਦਲਾ_ਬਦਲੀ_Inv_String_ਖੰਡ(ds, ਵਿੱਚ_Bundle+ਕੁੰਜੀਆਂ_Bottom, 4*(ਕੁੰਜੀਆਂ_ਸਿਖਰ-ਕੁੰਜੀਆਂ_Bottom),
                                    ਬਾਹਰਲਾ_Bundle+ਕੁੰਜੀਆਂ_Bottom, p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(ਕੁੰਜੀਆਂ_ਸਿਖਰ-ਕੁੰਜੀਆਂ_Bottom));
            ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags!=ਢੇਰ_Res_Flags) {
                uprv_free(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags);
            }
            ਵਾਪਸ 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        ਜੇ(ਕੁੰਜੀਆਂ_ਸਿਖਰ<resBottom) {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_Bundle+ਕੁੰਜੀਆਂ_ਸਿਖਰ, (resBottom-ਕੁੰਜੀਆਂ_ਸਿਖਰ)*4, ਬਾਹਰਲਾ_Bundle+ਕੁੰਜੀਆਂ_ਸਿਖਰ, p_Error_ਕੋਡ);
            ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-ਕੁੰਜੀਆਂ_ਸਿਖਰ));
                ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags!=ਢੇਰ_Res_Flags) {
                    uprv_free(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags);
                }
                ਵਾਪਸ 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਕੁੰਜੀ_Chars=(ਅਚਲ ਅੱਖਰ *)ਬਾਹਰਲਾ_Bundle; /* sort by outCharset */
        ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.major_Format_ਸੰਸਕਰਣ>1 || max_ਸਾਰਣੀ_ਲੰਮਾਈ<=ਢੇਰ_ਪੰਕਤੀ_ਸਮਰੱਥਾ) {
            ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ=ਪੰਕਤੀਆਂ;
            ਆਰਜ਼ੀ_ਸਾਰਣੀ.resort=resort;
        } ਨਹੀਂਤਾਂ {
            ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ=(ਪੰਕਤੀ_2 *)uprv_malloc(max_ਸਾਰਣੀ_ਲੰਮਾਈ*ਆਕਾਰ(ਪੰਕਤੀ_2)+max_ਸਾਰਣੀ_ਲੰਮਾਈ*4);
            ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ==ਖਾਲੀ_ਸੂਚਕ) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_ਸਾਰਣੀ_ਲੰਮਾਈ);
                *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags!=ਢੇਰ_Res_Flags) {
                    uprv_free(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags);
                }
                ਵਾਪਸ 0;
            }
            ਆਰਜ਼ੀ_ਸਾਰਣੀ.resort=(int32_t *)(ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ+max_ਸਾਰਣੀ_ਲੰਮਾਈ);
        }

        /* swap the resources */
        ures_ਅਦਲਾ_ਬਦਲੀ_Resource(ds, ਵਿੱਚ_Bundle, ਬਾਹਰਲਾ_Bundle, ਜੜ੍ਹ_Res, ਖਾਲੀ_ਸੂਚਕ, &ਆਰਜ਼ੀ_ਸਾਰਣੀ, p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             ਜੜ੍ਹ_Res);
        }

        ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ!=ਪੰਕਤੀਆਂ) {
            uprv_free(ਆਰਜ਼ੀ_ਸਾਰਣੀ.ਪੰਕਤੀਆਂ);
        }
        ਜੇ(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags!=ਢੇਰ_Res_Flags) {
            uprv_free(ਆਰਜ਼ੀ_ਸਾਰਣੀ.resFlags);
        }

        /* swap the root resource and indexes */
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_Bundle, ਕੁੰਜੀਆਂ_Bottom*4, ਬਾਹਰਲਾ_Bundle, p_Error_ਕੋਡ);
    }

    ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ+4*ਸਿਖਰ;
}

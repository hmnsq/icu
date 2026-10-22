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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "resource.h"
#അന്തര്ഭാവ "uarrsort.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucol_swp.h"
#അന്തര്ഭാവ "udataswp.h"
#അന്തര്ഭാവ "uinvchar.h"
#അന്തര്ഭാവ "uresdata.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, താക്കോൽ_Offset) \
    ((താക്കോൽ_Offset)<(pResData)->local_താക്കോൽ_Limit ? \
        (സ്ഥിരം അക്ഷരം *)(pResData)->p_വേര്+(താക്കോൽ_Offset) : \
        (pResData)->pool_Bundle_താക്കോലുകൾ+(താക്കോൽ_Offset)-(pResData)->local_താക്കോൽ_Limit)

#define RES_GET_KEY32(pResData, താക്കോൽ_Offset) \
    ((താക്കോൽ_Offset)>=0 ? \
        (സ്ഥിരം അക്ഷരം *)(pResData)->p_വേര്+(താക്കോൽ_Offset) : \
        (pResData)->pool_Bundle_താക്കോലുകൾ+((താക്കോൽ_Offset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
സ്ഥിതം സ്ഥിരം uint16_t gEmpty16=0;

സ്ഥിതം സ്ഥിരം ഘടന {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

സ്ഥിതം സ്ഥിരം ഘടന {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

സ്ഥിതം int32_t
_res_findTableItem(സ്ഥിരം ResourceData *pResData, സ്ഥിരം uint16_t *താക്കോൽ_Offsets, int32_t length,
                   സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം **real_താക്കോൽ) {
    സ്ഥിരം അക്ഷരം *table_താക്കോൽ;
    int32_t mid, start, limit;
    പൂർണ്ണസംഖ്യ result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    ആകുമ്പോൾ(start<limit) {
        mid = (start + limit) / 2;
        table_താക്കോൽ = RES_GET_KEY16(pResData, താക്കോൽ_Offsets[mid]);
        എങ്കിൽ (pResData->useNativeStrcmp) {
            result = uprv_strcmp(താക്കോൽ, table_താക്കോൽ);
        } അല്ലെങ്കിൽ {
            result = uprv_compareInvCharsAsAscii(താക്കോൽ, table_താക്കോൽ);
        }
        എങ്കിൽ (result < 0) {
            limit = mid;
        } അല്ലെങ്കിൽ എങ്കിൽ (result > 0) {
            start = mid + 1;
        } അല്ലെങ്കിൽ {
            /* We found it! */
            *real_താക്കോൽ=table_താക്കോൽ;
            തിരികെ mid;
        }
    }
    തിരികെ URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

സ്ഥിതം int32_t
_res_findTable32Item(സ്ഥിരം ResourceData *pResData, സ്ഥിരം int32_t *താക്കോൽ_Offsets, int32_t length,
                     സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം **real_താക്കോൽ) {
    സ്ഥിരം അക്ഷരം *table_താക്കോൽ;
    int32_t mid, start, limit;
    പൂർണ്ണസംഖ്യ result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    ആകുമ്പോൾ(start<limit) {
        mid = (start + limit) / 2;
        table_താക്കോൽ = RES_GET_KEY32(pResData, താക്കോൽ_Offsets[mid]);
        എങ്കിൽ (pResData->useNativeStrcmp) {
            result = uprv_strcmp(താക്കോൽ, table_താക്കോൽ);
        } അല്ലെങ്കിൽ {
            result = uprv_compareInvCharsAsAscii(താക്കോൽ, table_താക്കോൽ);
        }
        എങ്കിൽ (result < 0) {
            limit = mid;
        } അല്ലെങ്കിൽ എങ്കിൽ (result > 0) {
            start = mid + 1;
        } അല്ലെങ്കിൽ {
            /* We found it! */
            *real_താക്കോൽ=table_താക്കോൽ;
            തിരികെ mid;
        }
    }
    തിരികെ URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

സ്ഥിതം UBool U_CALLCONV
isAcceptable(ശൂന്യം *context,
             സ്ഥിരം അക്ഷരം * /*type*/, സ്ഥിരം അക്ഷരം * /*name*/,
             സ്ഥിരം UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    തിരികെ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->dataFormat[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->dataFormat[1]==0x65 &&
        pInfo->dataFormat[2]==0x73 &&
        pInfo->dataFormat[3]==0x42 &&
        (1<=pInfo->formatVersion[0] && pInfo->formatVersion[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

സ്ഥിതം ശൂന്യം
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, സ്ഥിരം ശൂന്യം *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType വേര്_Type;

    /* get the root resource */
    pResData->p_വേര് = static_cast<സ്ഥിരം int32_t*>(inBytes);
    pResData->വേര്_Res = static_cast<Resource>(*pResData->p_വേര്);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    എങ്കിൽ(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        തിരികെ;
    }

    /* currently, we accept only resources that have a Table as their roots */
    വേര്_Type = static_cast<UResType>(RES_GET_TYPE(pResData->വേര്_Res));
    എങ്കിൽ(!URES_IS_TABLE(വേര്_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        തിരികെ;
    }

    എങ്കിൽ(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_താക്കോൽ_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } അല്ലെങ്കിൽ {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        സ്ഥിരം int32_t *indexes=pResData->p_വേര്+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        എങ്കിൽ(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            തിരികെ;
        }
        എങ്കിൽ( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_മുകൾ]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            തിരികെ;
        }
        എങ്കിൽ(indexes[URES_INDEX_താക്കോലുകൾ_മുകൾ]>(1+indexLength)) {
            pResData->local_താക്കോൽ_Limit=indexes[URES_INDEX_താക്കോലുകൾ_മുകൾ]<<2;
        }
        എങ്കിൽ(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        എങ്കിൽ(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        എങ്കിൽ((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            തിരികെ;
        }
        എങ്കിൽ( indexLength>URES_INDEX_16_BIT_മുകൾ &&
            indexes[URES_INDEX_16_BIT_മുകൾ]>indexes[URES_INDEX_താക്കോലുകൾ_മുകൾ]
        ) {
            pResData->p16BitUnits = reinterpret_cast<സ്ഥിരം uint16_t*>(pResData->p_വേര് + indexes[URES_INDEX_താക്കോലുകൾ_മുകൾ]);
        }
    }

    എങ്കിൽ(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=സത്യം;
    }
}

U_CAPI ശൂന്യം U_EXPORT2
res_read(ResourceData *pResData,
         സ്ഥിരം UDataInfo *pInfo, സ്ഥിരം ശൂന്യം *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, വലുപ്പം(ResourceData));
    എങ്കിൽ(U_FAILURE(*errorCode)) {
        തിരികെ;
    }
    എങ്കിൽ(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        തിരികെ;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC ശൂന്യം
res_load(ResourceData *pResData,
         സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, വലുപ്പം(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    എങ്കിൽ(U_FAILURE(*errorCode)) {
        തിരികെ;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC ശൂന്യം
res_unload(ResourceData *pResData) {
    എങ്കിൽ(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

സ്ഥിതം സ്ഥിരം int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_ദ്വിമാന,
    URES_TABLE,
    URES_ALIAS,

    URES_TABLE,     /* URES_TABLE32 */
    URES_TABLE,     /* URES_TABLE16 */
    URES_STRING,    /* URES_STRING_V2 */
    URES_INT,

    URES_ARRAY,
    URES_ARRAY,     /* URES_ARRAY16 */
    URES_NONE,
    URES_NONE,

    URES_NONE,
    URES_NONE,
    URES_INT_VECTOR,
    URES_NONE
};

U_CAPI UResType U_EXPORT2
res_getPublicType(Resource res) {
    തിരികെ (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI സ്ഥിരം char16_t * U_EXPORT2
res_getStringNoTrace(സ്ഥിരം ResourceData *pResData, Resource res, int32_t *pLength) {
    സ്ഥിരം char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    എങ്കിൽ(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        എങ്കിൽ((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(സ്ഥിരം char16_t *)pResData->poolBundleStrings+offset;
        } അല്ലെങ്കിൽ {
            p=(സ്ഥിരം char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        എങ്കിൽ(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } അല്ലെങ്കിൽ എങ്കിൽ(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } അല്ലെങ്കിൽ എങ്കിൽ(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } അല്ലെങ്കിൽ {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        സ്ഥിരം int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_വേര്+res;
        length=*p32++;
        p=(സ്ഥിരം char16_t *)p32;
    } അല്ലെങ്കിൽ {
        p=nullptr;
        length=0;
    }
    എങ്കിൽ(pLength) {
        *pLength=length;
    }
    തിരികെ p;
}

നാമാകാശ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(സ്ഥിരം ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    എങ്കിൽ (offset == 0) {
        // empty string
    } അല്ലെങ്കിൽ എങ്കിൽ (res == offset) {
        സ്ഥിരം int32_t *p32=pResData->p_വേര്+res;
        int32_t length=*p32;
        സ്ഥിരം char16_t* p = reinterpret_cast<സ്ഥിരം char16_t*>(p32);
        തിരികെ length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } അല്ലെങ്കിൽ എങ്കിൽ (RES_GET_TYPE(res) == URES_STRING_V2) {
        സ്ഥിരം char16_t *p;
        എങ്കിൽ (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<സ്ഥിരം char16_t*>(pResData->poolBundleStrings) + offset;
        } അല്ലെങ്കിൽ {
            p = reinterpret_cast<സ്ഥിരം char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        എങ്കിൽ (first == 0x2205) {  // implicit length
            തിരികെ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } അല്ലെങ്കിൽ എങ്കിൽ (first == 0xdc03) {  // explicit length 3 (should not occur)
            തിരികെ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } അല്ലെങ്കിൽ {
            // Assume that the string has not been stored with more length units than necessary.
            തിരികെ അസത്യം;
        }
    }
    തിരികെ അസത്യം;
}

int32_t getStringArray(സ്ഥിരം ResourceData *pResData, സ്ഥിരം icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    int32_t length = array.getSize();
    എങ്കിൽ(length == 0) {
        തിരികെ 0;
    }
    എങ്കിൽ(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        തിരികെ length;
    }
    വേണ്ടി(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        സ്ഥിരം char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        എങ്കിൽ(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            തിരികെ 0;
        }
        dest[i].setTo(സത്യം, s, sLength);
    }
    തിരികെ length;
}

}  // namespace

U_CAPI സ്ഥിരം char16_t * U_EXPORT2
res_getAlias(സ്ഥിരം ResourceData *pResData, Resource res, int32_t *pLength) {
    സ്ഥിരം char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    എങ്കിൽ(RES_GET_TYPE(res)==URES_ALIAS) {
        സ്ഥിരം int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_വേര്+offset;
        length=*p32++;
        p=(സ്ഥിരം char16_t *)p32;
    } അല്ലെങ്കിൽ {
        p=nullptr;
        length=0;
    }
    എങ്കിൽ(pLength) {
        *pLength=length;
    }
    തിരികെ p;
}

U_CAPI സ്ഥിരം uint8_t * U_EXPORT2
res_get_ദ്വിമാന_No_Trace(സ്ഥിരം ResourceData *pResData, Resource res, int32_t *pLength) {
    സ്ഥിരം uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    എങ്കിൽ(RES_GET_TYPE(res)==URES_ദ്വിമാന) {
        സ്ഥിരം int32_t *p32= offset==0 ? (സ്ഥിരം int32_t*)&gEmpty32 : pResData->p_വേര്+offset;
        length=*p32++;
        p=(സ്ഥിരം uint8_t *)p32;
    } അല്ലെങ്കിൽ {
        p=nullptr;
        length=0;
    }
    എങ്കിൽ(pLength) {
        *pLength=length;
    }
    തിരികെ p;
}


U_CAPI സ്ഥിരം int32_t * U_EXPORT2
res_getIntVectorNoTrace(സ്ഥിരം ResourceData *pResData, Resource res, int32_t *pLength) {
    സ്ഥിരം int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    എങ്കിൽ(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (സ്ഥിരം int32_t *)&gEmpty32 : pResData->p_വേര്+offset;
        length=*p++;
    } അല്ലെങ്കിൽ {
        p=nullptr;
        length=0;
    }
    എങ്കിൽ(pLength) {
        *pLength=length;
    }
    തിരികെ p;
}

U_CAPI int32_t U_EXPORT2
res_എണ്ണം_Array_ഇനങ്ങൾ(സ്ഥിരം ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(res)) {
    അവസ്ഥ URES_STRING:
    അവസ്ഥ URES_STRING_V2:
    അവസ്ഥ URES_ദ്വിമാന:
    അവസ്ഥ URES_ALIAS:
    അവസ്ഥ URES_INT:
    അവസ്ഥ URES_INT_VECTOR:
        തിരികെ 1;
    അവസ്ഥ URES_ARRAY:
    അവസ്ഥ URES_TABLE32:
        തിരികെ offset==0 ? 0 : *(pResData->p_വേര്+offset);
    അവസ്ഥ URES_TABLE:
        തിരികെ offset==0 ? 0 : *((സ്ഥിരം uint16_t *)(pResData->p_വേര്+offset));
    അവസ്ഥ URES_ARRAY16:
    അവസ്ഥ URES_TABLE16:
        തിരികെ pResData->p16BitUnits[offset];
    സ്ഥിരസ്ഥിതി:
        തിരികെ 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_മൂല്യം::~Resource_Data_മൂല്യം() {}

UResType Resource_Data_മൂല്യം::getType() സ്ഥിരം {
    തിരികെ res_getPublicType(res);
}

സ്ഥിരം char16_t *Resource_Data_മൂല്യം::getString(int32_t &length, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    സ്ഥിരം char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    എങ്കിൽ(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    തിരികെ s;
}

സ്ഥിരം char16_t *Resource_Data_മൂല്യം::getAliasString(int32_t &length, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    സ്ഥിരം char16_t *s = res_getAlias(&getData(), res, &length);
    എങ്കിൽ(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    തിരികെ s;
}

int32_t Resource_Data_മൂല്യം::getInt(UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    തിരികെ res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_മൂല്യം::getUInt(UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    തിരികെ res_getUInt(fTraceInfo, res);
}

സ്ഥിരം int32_t *Resource_Data_മൂല്യം::getIntVector(int32_t &length, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    സ്ഥിരം int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    എങ്കിൽ(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    തിരികെ iv;
}

സ്ഥിരം uint8_t *Resource_Data_മൂല്യം::get_ദ്വിമാന(int32_t &length, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    സ്ഥിരം uint8_t *b = res_get_ദ്വിമാന(fTraceInfo, &getData(), res, &length);
    എങ്കിൽ(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    തിരികെ b;
}

ResourceArray Resource_Data_മൂല്യം::getArray(UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ {};
    }
    സ്ഥിരം uint16_t *items16 = nullptr;
    സ്ഥിരം Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(res)) {
    അവസ്ഥ URES_ARRAY:
        എങ്കിൽ (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<സ്ഥിരം Resource*>(getData().p_വേര്) + offset;
            length = *items32++;
        }
        നിർത്തുക;
    അവസ്ഥ URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        തിരികെ {};
    }
    തിരികെ ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_Data_മൂല്യം::getTable(UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ {};
    }
    സ്ഥിരം uint16_t *keys16 = nullptr;
    സ്ഥിരം int32_t *keys32 = nullptr;
    സ്ഥിരം uint16_t *items16 = nullptr;
    സ്ഥിരം Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(res)) {
    അവസ്ഥ URES_TABLE:
        എങ്കിൽ (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<സ്ഥിരം uint16_t*>(getData().p_വേര് + offset);
            length = *keys16++;
            items32 = reinterpret_cast<സ്ഥിരം Resource*>(keys16 + length + (~length & 1));
        }
        നിർത്തുക;
    അവസ്ഥ URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        നിർത്തുക;
    അവസ്ഥ URES_TABLE32:
        എങ്കിൽ (offset != 0) {  // empty if offset==0
            keys32 = getData().p_വേര്+offset;
            length = *keys32++;
            items32 = reinterpret_cast<സ്ഥിരം Resource*>(keys32) + length;
        }
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        തിരികെ {};
    }
    തിരികെ ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_Data_മൂല്യം::isNoInheritanceMarker() സ്ഥിരം {
    തിരികെ ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_മൂല്യം::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) സ്ഥിരം {
    തിരികെ ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_മൂല്യം::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ(URES_IS_ARRAY(res)) {
        തിരികെ ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    എങ്കിൽ(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        തിരികെ 1;
    }
    int32_t sLength;
    സ്ഥിരം char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    എങ്കിൽ(s != nullptr) {
        dest[0].setTo(സത്യം, s, sLength);
        തിരികെ 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    തിരികെ 0;
}

UnicodeString Resource_Data_മൂല്യം::getStringOrFirstOfArray(UErrorCode &errorCode) സ്ഥിരം {
    UnicodeString us;
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ us;
    }
    int32_t sLength;
    സ്ഥിരം char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    എങ്കിൽ(s != nullptr) {
        us.setTo(സത്യം, s, sLength);
        തിരികെ us;
    }
    ResourceArray array = getArray(errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ us;
    }
    എങ്കിൽ(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        എങ്കിൽ(s != nullptr) {
            us.setTo(സത്യം, s, sLength);
            തിരികെ us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    തിരികെ us;
}

U_NAMESPACE_END

സ്ഥിതം Resource
makeResourceFrom16(സ്ഥിരം ResourceData *pResData, int32_t res16) {
    എങ്കിൽ(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } അല്ലെങ്കിൽ {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    തിരികെ URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_താക്കോൽ(സ്ഥിരം ResourceData *pResData, Resource table,
                      int32_t *indexR, സ്ഥിരം അക്ഷരം **താക്കോൽ) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    എങ്കിൽ(താക്കോൽ == nullptr || *താക്കോൽ == nullptr) {
        തിരികെ RES_BOGUS;
    }
    തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(table)) {
    അവസ്ഥ URES_TABLE: {
        എങ്കിൽ (offset!=0) { /* empty if offset==0 */
            സ്ഥിരം uint16_t *p= (സ്ഥിരം uint16_t *)(pResData->p_വേര്+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *താക്കോൽ, താക്കോൽ);
            എങ്കിൽ(idx>=0) {
                സ്ഥിരം Resource *p32=(സ്ഥിരം Resource *)(p+length+(~length&1));
                തിരികെ p32[idx];
            }
        }
        നിർത്തുക;
    }
    അവസ്ഥ URES_TABLE16: {
        സ്ഥിരം uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *താക്കോൽ, താക്കോൽ);
        എങ്കിൽ(idx>=0) {
            തിരികെ makeResourceFrom16(pResData, p[length+idx]);
        }
        നിർത്തുക;
    }
    അവസ്ഥ URES_TABLE32: {
        എങ്കിൽ (offset!=0) { /* empty if offset==0 */
            സ്ഥിരം int32_t *p= pResData->p_വേര്+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *താക്കോൽ, താക്കോൽ);
            എങ്കിൽ(idx>=0) {
                തിരികെ (Resource)p[length+idx];
            }
        }
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }
    തിരികെ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(സ്ഥിരം ResourceData *pResData, Resource table,
                        int32_t indexR, സ്ഥിരം അക്ഷരം **താക്കോൽ) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    എങ്കിൽ (indexR < 0) {
        തിരികെ RES_BOGUS;
    }
    തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(table)) {
    അവസ്ഥ URES_TABLE: {
        എങ്കിൽ (offset != 0) { /* empty if offset==0 */
            സ്ഥിരം uint16_t *p= (സ്ഥിരം uint16_t *)(pResData->p_വേര്+offset);
            length=*p++;
            എങ്കിൽ(indexR<length) {
                സ്ഥിരം Resource *p32=(സ്ഥിരം Resource *)(p+length+(~length&1));
                എങ്കിൽ(താക്കോൽ!=nullptr) {
                    *താക്കോൽ=RES_GET_KEY16(pResData, p[indexR]);
                }
                തിരികെ p32[indexR];
            }
        }
        നിർത്തുക;
    }
    അവസ്ഥ URES_TABLE16: {
        സ്ഥിരം uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        എങ്കിൽ(indexR<length) {
            എങ്കിൽ(താക്കോൽ!=nullptr) {
                *താക്കോൽ=RES_GET_KEY16(pResData, p[indexR]);
            }
            തിരികെ makeResourceFrom16(pResData, p[length+indexR]);
        }
        നിർത്തുക;
    }
    അവസ്ഥ URES_TABLE32: {
        എങ്കിൽ (offset != 0) { /* empty if offset==0 */
            സ്ഥിരം int32_t *p= pResData->p_വേര്+offset;
            length=*p++;
            എങ്കിൽ(indexR<length) {
                എങ്കിൽ(താക്കോൽ!=nullptr) {
                    *താക്കോൽ=RES_GET_KEY32(pResData, p[indexR]);
                }
                തിരികെ (Resource)p[length+indexR];
            }
        }
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }
    തിരികെ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(സ്ഥിരം ResourceData *pResData, സ്ഥിരം അക്ഷരം *താക്കോൽ) {
    സ്ഥിരം അക്ഷരം *real_താക്കോൽ=താക്കോൽ;
    int32_t idx;
    തിരികെ res_get_Table_Item_By_താക്കോൽ(pResData, pResData->വേര്_Res, &idx, &real_താക്കോൽ);
}


UBool icu::ResourceTable::get_താക്കോൽ_And_മൂല്യം(int32_t i,
                                         സ്ഥിരം അക്ഷരം *&താക്കോൽ, icu::Resource_മൂല്യം &value) സ്ഥിരം {
    എങ്കിൽ(0 <= i && i < length) {
        icu::Resource_Data_മൂല്യം &rd_മൂല്യം = static_cast<icu::Resource_Data_മൂല്യം &>(value);
        എങ്കിൽ (keys16 != nullptr) {
            താക്കോൽ = RES_GET_KEY16(&rd_മൂല്യം.getData(), keys16[i]);
        } അല്ലെങ്കിൽ {
            താക്കോൽ = RES_GET_KEY32(&rd_മൂല്യം.getData(), keys32[i]);
        }
        Resource res;
        എങ്കിൽ (items16 != nullptr) {
            res = makeResourceFrom16(&rd_മൂല്യം.getData(), items16[i]);
        } അല്ലെങ്കിൽ {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_മൂല്യം.setResource(res, ResourceTracer(fTraceInfo, താക്കോൽ));
        തിരികെ സത്യം;
    }
    തിരികെ അസത്യം;
}

UBool icu::ResourceTable::find_മൂല്യം(സ്ഥിരം അക്ഷരം *താക്കോൽ, Resource_മൂല്യം &value) സ്ഥിരം {
    icu::Resource_Data_മൂല്യം &rd_മൂല്യം = static_cast<icu::Resource_Data_മൂല്യം &>(value);
    സ്ഥിരം അക്ഷരം *real_താക്കോൽ = nullptr;
    int32_t i;
    എങ്കിൽ (keys16 != nullptr) {
        i = _res_findTableItem(&rd_മൂല്യം.getData(), keys16, length, താക്കോൽ, &real_താക്കോൽ);
    } അല്ലെങ്കിൽ {
        i = _res_findTable32Item(&rd_മൂല്യം.getData(), keys32, length, താക്കോൽ, &real_താക്കോൽ);
    }
    എങ്കിൽ (i >= 0) {
        Resource res;
        എങ്കിൽ (items16 != nullptr) {
            res = makeResourceFrom16(&rd_മൂല്യം.getData(), items16[i]);
        } അല്ലെങ്കിൽ {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_മൂല്യം.setResource(res, ResourceTracer(fTraceInfo, താക്കോൽ));
        തിരികെ സത്യം;
    }
    തിരികെ അസത്യം;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(സ്ഥിരം ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    എങ്കിൽ (indexR < 0) {
        തിരികെ RES_BOGUS;
    }
    തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(array)) {
    അവസ്ഥ URES_ARRAY: {
        എങ്കിൽ (offset!=0) { /* empty if offset==0 */
            സ്ഥിരം int32_t *p= pResData->p_വേര്+offset;
            എങ്കിൽ(indexR<*p) {
                തിരികെ (Resource)p[1+indexR];
            }
        }
        നിർത്തുക;
    }
    അവസ്ഥ URES_ARRAY16: {
        സ്ഥിരം uint16_t *p=pResData->p16BitUnits+offset;
        എങ്കിൽ(indexR<*p) {
            തിരികെ makeResourceFrom16(pResData, p[1+indexR]);
        }
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }
    തിരികെ RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(സ്ഥിരം ResourceData *pResData, int32_t i) സ്ഥിരം {
    എങ്കിൽ (items16 != nullptr) {
        തിരികെ makeResourceFrom16(pResData, items16[i]);
    } അല്ലെങ്കിൽ {
        തിരികെ items32[i];
    }
}

UBool icu::ResourceArray::get_മൂല്യം(int32_t i, icu::Resource_മൂല്യം &value) സ്ഥിരം {
    എങ്കിൽ(0 <= i && i < length) {
        icu::Resource_Data_മൂല്യം &rd_മൂല്യം = static_cast<icu::Resource_Data_മൂല്യം &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_മൂല്യം.setResource(
            internalGetResource(&rd_മൂല്യം.getData(), i),
            ResourceTracer(fTraceInfo, i));
        തിരികെ സത്യം;
    }
    തിരികെ അസത്യം;
}

U_CFUNC Resource
res_findResource(സ്ഥിരം ResourceData *pResData, Resource r, അക്ഷരം** path, സ്ഥിരം അക്ഷരം** താക്കോൽ) {
  അക്ഷരം *pathP = *path, *nextSepP = *path;
  അക്ഷരം *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  എങ്കിൽ(!uprv_strlen(pathP)) {
      തിരികെ r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  എങ്കിൽ(!URES_IS_CONTAINER(type)) {
      തിരികെ RES_BOGUS;
  }
  
  ആകുമ്പോൾ(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    എങ്കിൽ(nextSepP != nullptr) {
      എങ്കിൽ(nextSepP == pathP) {
        // Empty key string.
        തിരികെ RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } അല്ലെങ്കിൽ {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    എങ്കിൽ(URES_IS_TABLE(type)) {
      *താക്കോൽ = pathP;
      t2 = res_get_Table_Item_By_താക്കോൽ(pResData, t1, &indexR, താക്കോൽ);
    } അല്ലെങ്കിൽ എങ്കിൽ(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      എങ്കിൽ(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } അല്ലെങ്കിൽ {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *താക്കോൽ = nullptr;
    } അല്ലെങ്കിൽ { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  തിരികെ t1;
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
തരനിർവചനം ഘടന Row {
    int32_t താക്കോൽ_Index, sortIndex;
} Row;

സ്ഥിതം int32_t U_CALLCONV
ures_compareRows(സ്ഥിരം ശൂന്യം *context, സ്ഥിരം ശൂന്യം *left, സ്ഥിരം ശൂന്യം *വലത്) {
    സ്ഥിരം അക്ഷരം* താക്കോൽ_Chars = static_cast<സ്ഥിരം അക്ഷരം*>(context);
    തിരികെ static_cast<int32_t>(uprv_strcmp(താക്കോൽ_Chars + static_cast<സ്ഥിരം Row*>(left)->താക്കോൽ_Index,
                                            താക്കോൽ_Chars + static_cast<സ്ഥിരം Row*>(വലത്)->താക്കോൽ_Index));
}

തരനിർവചനം ഘടന TempTable {
    സ്ഥിരം അക്ഷരം *താക്കോൽ_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_താക്കോൽ_Limit;
    uint8_t majorFormatVersion;
} TempTable;

എണ്ണൽ {
    അട്ടി_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം g_Unknown_താക്കോൽ="";

#എങ്കിൽ !UCONFIG_NO_COLLATION
// resource table key for collation binaries
സ്ഥിതം സ്ഥിരം char16_t g_Collation_Bin_താക്കോൽ[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
സ്ഥിതം ശൂന്യം
ures_swapResource(സ്ഥിരം UDataSwapper *ds,
                  സ്ഥിരം Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  സ്ഥിരം അക്ഷരം *താക്കോൽ,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    സ്ഥിരം Resource *p;
    Resource *q;
    int32_t offset, count;

    തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(res)) {
    അവസ്ഥ URES_TABLE16:
    അവസ്ഥ URES_STRING_V2:
    അവസ്ഥ URES_INT:
    അവസ്ഥ URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        തിരികെ;
    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    എങ്കിൽ(offset==0) {
        /* special offset indicating an empty item */
        തിരികെ;
    }
    എങ്കിൽ (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        തിരികെ;
    } അല്ലെങ്കിൽ {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    തിരഞ്ഞെടുപ്പ്(RES_GET_TYPE(res)) {
    അവസ്ഥ URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    അവസ്ഥ URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        നിർത്തുക;
    അവസ്ഥ URES_ദ്വിമാന:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#എങ്കിൽ !UCONFIG_NO_COLLATION
        എങ്കിൽ( താക്കോൽ!=nullptr &&  /* the binary is in a table */
            (താക്കോൽ!=g_Unknown_താക്കോൽ ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, താക്കോൽ, -1,
                                       g_Collation_Bin_താക്കോൽ, UPRV_LENGTHOF(g_Collation_Bin_താക്കോൽ)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_ദ്വിമാന(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#endif
        നിർത്തുക;
    അവസ്ഥ URES_TABLE:
    അവസ്ഥ URES_TABLE32:
        {
            സ്ഥിരം uint16_t *pKey16;
            uint16_t *qKey16;

            സ്ഥിരം int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            എങ്കിൽ(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<സ്ഥിരം uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } അല്ലെങ്കിൽ {
                /* get table item count */
                pKey32 = reinterpret_cast<സ്ഥിരം int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            എങ്കിൽ(count==0) {
                നിർത്തുക;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            വേണ്ടി(i=0; i<count; ++i) {
                സ്ഥിരം അക്ഷരം *item_താക്കോൽ=g_Unknown_താക്കോൽ;
                എങ്കിൽ(pKey16!=nullptr) {
                    int32_t താക്കോൽ_Offset=ds->readUInt16(pKey16[i]);
                    എങ്കിൽ(താക്കോൽ_Offset<pTempTable->local_താക്കോൽ_Limit) {
                        item_താക്കോൽ = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(outBundle) + താക്കോൽ_Offset;
                    }
                } അല്ലെങ്കിൽ {
                    int32_t താക്കോൽ_Offset=udata_readInt32(ds, pKey32[i]);
                    എങ്കിൽ(താക്കോൽ_Offset>=0) {
                        item_താക്കോൽ = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(outBundle) + താക്കോൽ_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_താക്കോൽ, pTempTable, pErrorCode);
                എങ്കിൽ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    തിരികെ;
                }
            }

            എങ്കിൽ(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                എങ്കിൽ(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } അല്ലെങ്കിൽ {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                നിർത്തുക;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            എങ്കിൽ(pKey16!=nullptr) {
                വേണ്ടി(i=0; i<count; ++i) {
                    pTempTable->rows[i].താക്കോൽ_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } അല്ലെങ്കിൽ {
                വേണ്ടി(i=0; i<count; ++i) {
                    pTempTable->rows[i].താക്കോൽ_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, വലുപ്പം(Row),
                           ures_compareRows, pTempTable->താക്കോൽ_Chars,
                           അസത്യം, pErrorCode);
            എങ്കിൽ(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                തിരികെ;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*താക്കോലുകൾ*/
            എങ്കിൽ(pKey16!=nullptr) {
                uint16_t *rKey16;

                എങ്കിൽ(pKey16!=qKey16) {
                    rKey16=qKey16;
                } അല്ലെങ്കിൽ {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                വേണ്ടി(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                എങ്കിൽ(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } അല്ലെങ്കിൽ {
                int32_t *rKey32;

                എങ്കിൽ(pKey32!=qKey32) {
                    rKey32=qKey32;
                } അല്ലെങ്കിൽ {
                    rKey32=pTempTable->resort;
                }
                വേണ്ടി(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                എങ്കിൽ(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                എങ്കിൽ(p!=q) {
                    r=q;
                } അല്ലെങ്കിൽ {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                വേണ്ടി(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                എങ്കിൽ(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        നിർത്തുക;
    അവസ്ഥ URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            വേണ്ടി(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                എങ്കിൽ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    തിരികെ;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        നിർത്തുക;
    അവസ്ഥ URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        നിർത്തുക;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(സ്ഥിരം UDataSwapper *ds,
          സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
          UErrorCode *pErrorCode) {
    സ്ഥിരം UDataInfo *pInfo;
    സ്ഥിരം Resource *inBundle;
    Resource വേര്_Res;
    int32_t headerSize, maxTableLength;

    Row rows[അട്ടി_ROW_CAPACITY];
    int32_t resort[അട്ടി_ROW_CAPACITY];
    TempTable tempTable;

    സ്ഥിരം int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, താക്കോലുകൾ_Bottom, താക്കോലുകൾ_മുകൾ, resBottom, മുകൾ;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    /* check data format and format version */
    pInfo=(സ്ഥിരം UDataInfo *)((സ്ഥിരം അക്ഷരം *)inData+4);
    എങ്കിൽ(!(
        pInfo->dataFormat[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->dataFormat[1]==0x65 &&
        pInfo->dataFormat[2]==0x73 &&
        pInfo->dataFormat[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->formatVersion[0]==1 && pInfo->formatVersion[1]>=1) ||
            pInfo->formatVersion[0]==2 || pInfo->formatVersion[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0], pInfo->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        തിരികെ 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    എങ്കിൽ(length<0) {
        bundleLength=-1;
    } അല്ലെങ്കിൽ {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        എങ്കിൽ(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
    }

    inBundle=(സ്ഥിരം Resource *)((സ്ഥിരം അക്ഷരം *)inData+headerSize);
    വേര്_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(സ്ഥിരം int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    എങ്കിൽ(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }
    താക്കോലുകൾ_Bottom=1+indexLength;
    താക്കോലുകൾ_മുകൾ=udata_readInt32(ds, inIndexes[URES_INDEX_താക്കോലുകൾ_മുകൾ]);
    എങ്കിൽ(indexLength>URES_INDEX_16_BIT_മുകൾ) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_മുകൾ]);
    } അല്ലെങ്കിൽ {
        resBottom=താക്കോലുകൾ_മുകൾ;
    }
    മുകൾ=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_മുകൾ]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    എങ്കിൽ(0<=bundleLength && bundleLength<മുകൾ) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         മുകൾ, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }
    എങ്കിൽ(താക്കോലുകൾ_മുകൾ>(1+indexLength)) {
        tempTable.local_താക്കോൽ_Limit=താക്കോലുകൾ_മുകൾ<<2;
    } അല്ലെങ്കിൽ {
        tempTable.local_താക്കോൽ_Limit=0;
    }

    എങ്കിൽ(length>=0) {
        Resource *outBundle=(Resource *)((അക്ഷരം *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t അട്ടി_Res_Flags[അട്ടി_ROW_CAPACITY];
        int32_t resFlagsLength;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        resFlagsLength=(length+31)>>5;          /* number of bytes needed */
        resFlagsLength=(resFlagsLength+3)&~3;   /* multiple of 4 bytes for uint32_t */
        എങ്കിൽ(resFlagsLength<=(int32_t)വലുപ്പം(അട്ടി_Res_Flags)) {
            tempTable.resFlags=അട്ടി_Res_Flags;
        } അല്ലെങ്കിൽ {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            എങ്കിൽ(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                തിരികെ 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        എങ്കിൽ(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*മുകൾ);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+താക്കോലുകൾ_Bottom, 4*(താക്കോലുകൾ_മുകൾ-താക്കോലുകൾ_Bottom),
                                    outBundle+താക്കോലുകൾ_Bottom, pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(താക്കോലുകൾ_മുകൾ-താക്കോലുകൾ_Bottom));
            എങ്കിൽ(tempTable.resFlags!=അട്ടി_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            തിരികെ 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        എങ്കിൽ(താക്കോലുകൾ_മുകൾ<resBottom) {
            ds->swapArray16(ds, inBundle+താക്കോലുകൾ_മുകൾ, (resBottom-താക്കോലുകൾ_മുകൾ)*4, outBundle+താക്കോലുകൾ_മുകൾ, pErrorCode);
            എങ്കിൽ(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-താക്കോലുകൾ_മുകൾ));
                എങ്കിൽ(tempTable.resFlags!=അട്ടി_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                തിരികെ 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.താക്കോൽ_Chars=(സ്ഥിരം അക്ഷരം *)outBundle; /* sort by outCharset */
        എങ്കിൽ(tempTable.majorFormatVersion>1 || maxTableLength<=അട്ടി_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } അല്ലെങ്കിൽ {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*വലുപ്പം(Row)+maxTableLength*4);
            എങ്കിൽ(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                എങ്കിൽ(tempTable.resFlags!=അട്ടി_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                തിരികെ 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, വേര്_Res, nullptr, &tempTable, pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             വേര്_Res);
        }

        എങ്കിൽ(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        എങ്കിൽ(tempTable.resFlags!=അട്ടി_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, താക്കോലുകൾ_Bottom*4, outBundle, pErrorCode);
    }

    തിരികെ headerSize+4*മുകൾ;
}

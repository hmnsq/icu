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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "resource.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucol_swp.h"
#ଅନ୍ତର୍ଭାଵ "udataswp.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"
#ଅନ୍ତର୍ଭାଵ "uresdata.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, ଚାବି_Offset) \
    ((ଚାବି_Offset)<(pResData)->local_ଚାବି_Limit ? \
        (ଅଚଳ ଅକ୍ଷର *)(pResData)->p_ମୂଳ+(ଚାବି_Offset) : \
        (pResData)->pool_Bundle_ଚାବିଗୁଡ଼ିକ+(ଚାବି_Offset)-(pResData)->local_ଚାବି_Limit)

#define RES_GET_KEY32(pResData, ଚାବି_Offset) \
    ((ଚାବି_Offset)>=0 ? \
        (ଅଚଳ ଅକ୍ଷର *)(pResData)->p_ମୂଳ+(ଚାବି_Offset) : \
        (pResData)->pool_Bundle_ଚାବିଗୁଡ଼ିକ+((ଚାବି_Offset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
ସ୍ଥିର ଅଚଳ uint16_t gEmpty16=0;

ସ୍ଥିର ଅଚଳ ସଂରଚନା {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

ସ୍ଥିର ଅଚଳ ସଂରଚନା {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ସ୍ଥିର int32_t
_res_findTableItem(ଅଚଳ ResourceData *pResData, ଅଚଳ uint16_t *ଚାବି_Offsets, int32_t length,
                   ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର **real_ଚାବି) {
    ଅଚଳ ଅକ୍ଷର *table_ଚାବି;
    int32_t mid, start, limit;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    ଯେତେବେଳେ(start<limit) {
        mid = (start + limit) / 2;
        table_ଚାବି = RES_GET_KEY16(pResData, ଚାବି_Offsets[mid]);
        ଯଦି (pResData->useNativeStrcmp) {
            result = uprv_strcmp(ଚାବି, table_ଚାବି);
        } ଅନ୍ୟଥା {
            result = uprv_compareInvCharsAsAscii(ଚାବି, table_ଚାବି);
        }
        ଯଦି (result < 0) {
            limit = mid;
        } ଅନ୍ୟଥା ଯଦି (result > 0) {
            start = mid + 1;
        } ଅନ୍ୟଥା {
            /* We found it! */
            *real_ଚାବି=table_ଚାବି;
            ଫେରନ୍ତୁ mid;
        }
    }
    ଫେରନ୍ତୁ URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

ସ୍ଥିର int32_t
_res_findTable32Item(ଅଚଳ ResourceData *pResData, ଅଚଳ int32_t *ଚାବି_Offsets, int32_t length,
                     ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର **real_ଚାବି) {
    ଅଚଳ ଅକ୍ଷର *table_ଚାବି;
    int32_t mid, start, limit;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    ଯେତେବେଳେ(start<limit) {
        mid = (start + limit) / 2;
        table_ଚାବି = RES_GET_KEY32(pResData, ଚାବି_Offsets[mid]);
        ଯଦି (pResData->useNativeStrcmp) {
            result = uprv_strcmp(ଚାବି, table_ଚାବି);
        } ଅନ୍ୟଥା {
            result = uprv_compareInvCharsAsAscii(ଚାବି, table_ଚାବି);
        }
        ଯଦି (result < 0) {
            limit = mid;
        } ଅନ୍ୟଥା ଯଦି (result > 0) {
            start = mid + 1;
        } ଅନ୍ୟଥା {
            /* We found it! */
            *real_ଚାବି=table_ଚାବି;
            ଫେରନ୍ତୁ mid;
        }
    }
    ଫେରନ୍ତୁ URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ସ୍ଥିର UBool U_CALLCONV
isAcceptable(ଶୂନ୍ୟ *context,
             ଅଚଳ ଅକ୍ଷର * /*type*/, ଅଚଳ ଅକ୍ଷର * /*name*/,
             ଅଚଳ UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    ଫେରନ୍ତୁ
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

ସ୍ଥିର ଶୂନ୍ୟ
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, ଅଚଳ ଶୂନ୍ୟ *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType ମୂଳ_Type;

    /* get the root resource */
    pResData->p_ମୂଳ = static_cast<ଅଚଳ int32_t*>(inBytes);
    pResData->ମୂଳ_Res = static_cast<Resource>(*pResData->p_ମୂଳ);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    ଯଦି(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        ଫେରନ୍ତୁ;
    }

    /* currently, we accept only resources that have a Table as their roots */
    ମୂଳ_Type = static_cast<UResType>(RES_GET_TYPE(pResData->ମୂଳ_Res));
    ଯଦି(!URES_IS_TABLE(ମୂଳ_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        ଫେରନ୍ତୁ;
    }

    ଯଦି(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_ଚାବି_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } ଅନ୍ୟଥା {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ଅଚଳ int32_t *indexes=pResData->p_ମୂଳ+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        ଯଦି(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            ଫେରନ୍ତୁ;
        }
        ଯଦି( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_ଶୀର୍ଷ]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            ଫେରନ୍ତୁ;
        }
        ଯଦି(indexes[URES_INDEX_ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ]>(1+indexLength)) {
            pResData->local_ଚାବି_Limit=indexes[URES_INDEX_ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ]<<2;
        }
        ଯଦି(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        ଯଦି(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        ଯଦି((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            ଫେରନ୍ତୁ;
        }
        ଯଦି( indexLength>URES_INDEX_16_BIT_ଶୀର୍ଷ &&
            indexes[URES_INDEX_16_BIT_ଶୀର୍ଷ]>indexes[URES_INDEX_ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ]
        ) {
            pResData->p16BitUnits = reinterpret_cast<ଅଚଳ uint16_t*>(pResData->p_ମୂଳ + indexes[URES_INDEX_ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ]);
        }
    }

    ଯଦି(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=ସତ୍ୟ;
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
res_read(ResourceData *pResData,
         ଅଚଳ UDataInfo *pInfo, ଅଚଳ ଶୂନ୍ୟ *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, ଆକାର(ResourceData));
    ଯଦି(U_FAILURE(*errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        ଫେରନ୍ତୁ;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC ଶୂନ୍ୟ
res_load(ResourceData *pResData,
         ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, ଆକାର(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    ଯଦି(U_FAILURE(*errorCode)) {
        ଫେରନ୍ତୁ;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC ଶୂନ୍ୟ
res_unload(ResourceData *pResData) {
    ଯଦି(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

ସ୍ଥିର ଅଚଳ int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_ଦ୍ୱିମିକ,
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
    ଫେରନ୍ତୁ (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI ଅଚଳ char16_t * U_EXPORT2
res_getStringNoTrace(ଅଚଳ ResourceData *pResData, Resource res, int32_t *pLength) {
    ଅଚଳ char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ଯଦି(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        ଯଦି((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(ଅଚଳ char16_t *)pResData->poolBundleStrings+offset;
        } ଅନ୍ୟଥା {
            p=(ଅଚଳ char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        ଯଦି(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } ଅନ୍ୟଥା ଯଦି(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } ଅନ୍ୟଥା ଯଦି(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } ଅନ୍ୟଥା {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } ଅନ୍ୟଥା ଯଦି(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        ଅଚଳ int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_ମୂଳ+res;
        length=*p32++;
        p=(ଅଚଳ char16_t *)p32;
    } ଅନ୍ୟଥା {
        p=nullptr;
        length=0;
    }
    ଯଦି(pLength) {
        *pLength=length;
    }
    ଫେରନ୍ତୁ p;
}

ନାମାକାଶ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ଅଚଳ ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    ଯଦି (offset == 0) {
        // empty string
    } ଅନ୍ୟଥା ଯଦି (res == offset) {
        ଅଚଳ int32_t *p32=pResData->p_ମୂଳ+res;
        int32_t length=*p32;
        ଅଚଳ char16_t* p = reinterpret_cast<ଅଚଳ char16_t*>(p32);
        ଫେରନ୍ତୁ length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } ଅନ୍ୟଥା ଯଦି (RES_GET_TYPE(res) == URES_STRING_V2) {
        ଅଚଳ char16_t *p;
        ଯଦି (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<ଅଚଳ char16_t*>(pResData->poolBundleStrings) + offset;
        } ଅନ୍ୟଥା {
            p = reinterpret_cast<ଅଚଳ char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        ଯଦି (first == 0x2205) {  // implicit length
            ଫେରନ୍ତୁ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } ଅନ୍ୟଥା ଯଦି (first == 0xdc03) {  // explicit length 3 (should not occur)
            ଫେରନ୍ତୁ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } ଅନ୍ୟଥା {
            // Assume that the string has not been stored with more length units than necessary.
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

int32_t getStringArray(ଅଚଳ ResourceData *pResData, ଅଚଳ icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    int32_t length = array.getSize();
    ଯଦି(length == 0) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        ଫେରନ୍ତୁ length;
    }
    ପାଇଁ(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        ଅଚଳ char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        ଯଦି(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            ଫେରନ୍ତୁ 0;
        }
        dest[i].setTo(ସତ୍ୟ, s, sLength);
    }
    ଫେରନ୍ତୁ length;
}

}  // namespace

U_CAPI ଅଚଳ char16_t * U_EXPORT2
res_getAlias(ଅଚଳ ResourceData *pResData, Resource res, int32_t *pLength) {
    ଅଚଳ char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ଯଦି(RES_GET_TYPE(res)==URES_ALIAS) {
        ଅଚଳ int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_ମୂଳ+offset;
        length=*p32++;
        p=(ଅଚଳ char16_t *)p32;
    } ଅନ୍ୟଥା {
        p=nullptr;
        length=0;
    }
    ଯଦି(pLength) {
        *pLength=length;
    }
    ଫେରନ୍ତୁ p;
}

U_CAPI ଅଚଳ uint8_t * U_EXPORT2
res_get_ଦ୍ୱିମିକ_No_Trace(ଅଚଳ ResourceData *pResData, Resource res, int32_t *pLength) {
    ଅଚଳ uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ଯଦି(RES_GET_TYPE(res)==URES_ଦ୍ୱିମିକ) {
        ଅଚଳ int32_t *p32= offset==0 ? (ଅଚଳ int32_t*)&gEmpty32 : pResData->p_ମୂଳ+offset;
        length=*p32++;
        p=(ଅଚଳ uint8_t *)p32;
    } ଅନ୍ୟଥା {
        p=nullptr;
        length=0;
    }
    ଯଦି(pLength) {
        *pLength=length;
    }
    ଫେରନ୍ତୁ p;
}


U_CAPI ଅଚଳ int32_t * U_EXPORT2
res_getIntVectorNoTrace(ଅଚଳ ResourceData *pResData, Resource res, int32_t *pLength) {
    ଅଚଳ int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ଯଦି(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (ଅଚଳ int32_t *)&gEmpty32 : pResData->p_ମୂଳ+offset;
        length=*p++;
    } ଅନ୍ୟଥା {
        p=nullptr;
        length=0;
    }
    ଯଦି(pLength) {
        *pLength=length;
    }
    ଫେରନ୍ତୁ p;
}

U_CAPI int32_t U_EXPORT2
res_ଗଣନା_Array_ବସ୍ତୁଗୁଡ଼ିକ(ଅଚଳ ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    ବାଛ(RES_GET_TYPE(res)) {
    ସ୍ଥିତି URES_STRING:
    ସ୍ଥିତି URES_STRING_V2:
    ସ୍ଥିତି URES_ଦ୍ୱିମିକ:
    ସ୍ଥିତି URES_ALIAS:
    ସ୍ଥିତି URES_INT:
    ସ୍ଥିତି URES_INT_VECTOR:
        ଫେରନ୍ତୁ 1;
    ସ୍ଥିତି URES_ARRAY:
    ସ୍ଥିତି URES_TABLE32:
        ଫେରନ୍ତୁ offset==0 ? 0 : *(pResData->p_ମୂଳ+offset);
    ସ୍ଥିତି URES_TABLE:
        ଫେରନ୍ତୁ offset==0 ? 0 : *((ଅଚଳ uint16_t *)(pResData->p_ମୂଳ+offset));
    ସ୍ଥିତି URES_ARRAY16:
    ସ୍ଥିତି URES_TABLE16:
        ଫେରନ୍ତୁ pResData->p16BitUnits[offset];
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_ମୂଲ୍ୟ::~Resource_Data_ମୂଲ୍ୟ() {}

UResType Resource_Data_ମୂଲ୍ୟ::getType() ଅଚଳ {
    ଫେରନ୍ତୁ res_getPublicType(res);
}

ଅଚଳ char16_t *Resource_Data_ମୂଲ୍ୟ::getString(int32_t &length, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    ଯଦି(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ଫେରନ୍ତୁ s;
}

ଅଚଳ char16_t *Resource_Data_ମୂଲ୍ୟ::getAliasString(int32_t &length, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ char16_t *s = res_getAlias(&getData(), res, &length);
    ଯଦି(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ଫେରନ୍ତୁ s;
}

int32_t Resource_Data_ମୂଲ୍ୟ::getInt(UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ଫେରନ୍ତୁ res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_ମୂଲ୍ୟ::getUInt(UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ଫେରନ୍ତୁ res_getUInt(fTraceInfo, res);
}

ଅଚଳ int32_t *Resource_Data_ମୂଲ୍ୟ::getIntVector(int32_t &length, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    ଯଦି(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ଫେରନ୍ତୁ iv;
}

ଅଚଳ uint8_t *Resource_Data_ମୂଲ୍ୟ::get_ଦ୍ୱିମିକ(int32_t &length, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ uint8_t *b = res_get_ଦ୍ୱିମିକ(fTraceInfo, &getData(), res, &length);
    ଯଦି(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ଫେରନ୍ତୁ b;
}

ResourceArray Resource_Data_ମୂଲ୍ୟ::getArray(UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ {};
    }
    ଅଚଳ uint16_t *items16 = nullptr;
    ଅଚଳ Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    ବାଛ(RES_GET_TYPE(res)) {
    ସ୍ଥିତି URES_ARRAY:
        ଯଦି (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ଅଚଳ Resource*>(getData().p_ମୂଳ) + offset;
            length = *items32++;
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        ଫେରନ୍ତୁ {};
    }
    ଫେରନ୍ତୁ ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_Data_ମୂଲ୍ୟ::getTable(UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ {};
    }
    ଅଚଳ uint16_t *keys16 = nullptr;
    ଅଚଳ int32_t *keys32 = nullptr;
    ଅଚଳ uint16_t *items16 = nullptr;
    ଅଚଳ Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    ବାଛ(RES_GET_TYPE(res)) {
    ସ୍ଥିତି URES_TABLE:
        ଯଦି (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ଅଚଳ uint16_t*>(getData().p_ମୂଳ + offset);
            length = *keys16++;
            items32 = reinterpret_cast<ଅଚଳ Resource*>(keys16 + length + (~length & 1));
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି URES_TABLE32:
        ଯଦି (offset != 0) {  // empty if offset==0
            keys32 = getData().p_ମୂଳ+offset;
            length = *keys32++;
            items32 = reinterpret_cast<ଅଚଳ Resource*>(keys32) + length;
        }
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        ଫେରନ୍ତୁ {};
    }
    ଫେରନ୍ତୁ ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_Data_ମୂଲ୍ୟ::isNoInheritanceMarker() ଅଚଳ {
    ଫେରନ୍ତୁ ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_ମୂଲ୍ୟ::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) ଅଚଳ {
    ଫେରନ୍ତୁ ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_ମୂଲ୍ୟ::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) ଅଚଳ {
    ଯଦି(URES_IS_ARRAY(res)) {
        ଫେରନ୍ତୁ ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        ଫେରନ୍ତୁ 1;
    }
    int32_t sLength;
    ଅଚଳ char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    ଯଦି(s != nullptr) {
        dest[0].setTo(ସତ୍ୟ, s, sLength);
        ଫେରନ୍ତୁ 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    ଫେରନ୍ତୁ 0;
}

UnicodeString Resource_Data_ମୂଲ୍ୟ::getStringOrFirstOfArray(UErrorCode &errorCode) ଅଚଳ {
    UnicodeString us;
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ us;
    }
    int32_t sLength;
    ଅଚଳ char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    ଯଦି(s != nullptr) {
        us.setTo(ସତ୍ୟ, s, sLength);
        ଫେରନ୍ତୁ us;
    }
    ResourceArray array = getArray(errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ us;
    }
    ଯଦି(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        ଯଦି(s != nullptr) {
            us.setTo(ସତ୍ୟ, s, sLength);
            ଫେରନ୍ତୁ us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    ଫେରନ୍ତୁ us;
}

U_NAMESPACE_END

ସ୍ଥିର Resource
makeResourceFrom16(ଅଚଳ ResourceData *pResData, int32_t res16) {
    ଯଦି(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } ଅନ୍ୟଥା {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    ଫେରନ୍ତୁ URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_ଚାବି(ଅଚଳ ResourceData *pResData, Resource table,
                      int32_t *indexR, ଅଚଳ ଅକ୍ଷର **ଚାବି) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    ଯଦି(ଚାବି == nullptr || *ଚାବି == nullptr) {
        ଫେରନ୍ତୁ RES_BOGUS;
    }
    ବାଛ(RES_GET_TYPE(table)) {
    ସ୍ଥିତି URES_TABLE: {
        ଯଦି (offset!=0) { /* empty if offset==0 */
            ଅଚଳ uint16_t *p= (ଅଚଳ uint16_t *)(pResData->p_ମୂଳ+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *ଚାବି, ଚାବି);
            ଯଦି(idx>=0) {
                ଅଚଳ Resource *p32=(ଅଚଳ Resource *)(p+length+(~length&1));
                ଫେରନ୍ତୁ p32[idx];
            }
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି URES_TABLE16: {
        ଅଚଳ uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *ଚାବି, ଚାବି);
        ଯଦି(idx>=0) {
            ଫେରନ୍ତୁ makeResourceFrom16(pResData, p[length+idx]);
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି URES_TABLE32: {
        ଯଦି (offset!=0) { /* empty if offset==0 */
            ଅଚଳ int32_t *p= pResData->p_ମୂଳ+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *ଚାବି, ଚାବି);
            ଯଦି(idx>=0) {
                ଫେରନ୍ତୁ (Resource)p[length+idx];
            }
        }
        ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(ଅଚଳ ResourceData *pResData, Resource table,
                        int32_t indexR, ଅଚଳ ଅକ୍ଷର **ଚାବି) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    ଯଦି (indexR < 0) {
        ଫେରନ୍ତୁ RES_BOGUS;
    }
    ବାଛ(RES_GET_TYPE(table)) {
    ସ୍ଥିତି URES_TABLE: {
        ଯଦି (offset != 0) { /* empty if offset==0 */
            ଅଚଳ uint16_t *p= (ଅଚଳ uint16_t *)(pResData->p_ମୂଳ+offset);
            length=*p++;
            ଯଦି(indexR<length) {
                ଅଚଳ Resource *p32=(ଅଚଳ Resource *)(p+length+(~length&1));
                ଯଦି(ଚାବି!=nullptr) {
                    *ଚାବି=RES_GET_KEY16(pResData, p[indexR]);
                }
                ଫେରନ୍ତୁ p32[indexR];
            }
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି URES_TABLE16: {
        ଅଚଳ uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        ଯଦି(indexR<length) {
            ଯଦି(ଚାବି!=nullptr) {
                *ଚାବି=RES_GET_KEY16(pResData, p[indexR]);
            }
            ଫେରନ୍ତୁ makeResourceFrom16(pResData, p[length+indexR]);
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି URES_TABLE32: {
        ଯଦି (offset != 0) { /* empty if offset==0 */
            ଅଚଳ int32_t *p= pResData->p_ମୂଳ+offset;
            length=*p++;
            ଯଦି(indexR<length) {
                ଯଦି(ଚାବି!=nullptr) {
                    *ଚାବି=RES_GET_KEY32(pResData, p[indexR]);
                }
                ଫେରନ୍ତୁ (Resource)p[length+indexR];
            }
        }
        ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ଅଚଳ ResourceData *pResData, ଅଚଳ ଅକ୍ଷର *ଚାବି) {
    ଅଚଳ ଅକ୍ଷର *real_ଚାବି=ଚାବି;
    int32_t idx;
    ଫେରନ୍ତୁ res_get_Table_Item_By_ଚାବି(pResData, pResData->ମୂଳ_Res, &idx, &real_ଚାବି);
}


UBool icu::ResourceTable::get_ଚାବି_And_ମୂଲ୍ୟ(int32_t i,
                                         ଅଚଳ ଅକ୍ଷର *&ଚାବି, icu::Resource_ମୂଲ୍ୟ &value) ଅଚଳ {
    ଯଦି(0 <= i && i < length) {
        icu::Resource_Data_ମୂଲ୍ୟ &rd_ମୂଲ୍ୟ = static_cast<icu::Resource_Data_ମୂଲ୍ୟ &>(value);
        ଯଦି (keys16 != nullptr) {
            ଚାବି = RES_GET_KEY16(&rd_ମୂଲ୍ୟ.getData(), keys16[i]);
        } ଅନ୍ୟଥା {
            ଚାବି = RES_GET_KEY32(&rd_ମୂଲ୍ୟ.getData(), keys32[i]);
        }
        Resource res;
        ଯଦି (items16 != nullptr) {
            res = makeResourceFrom16(&rd_ମୂଲ୍ୟ.getData(), items16[i]);
        } ଅନ୍ୟଥା {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ମୂଲ୍ୟ.setResource(res, ResourceTracer(fTraceInfo, ଚାବି));
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

UBool icu::ResourceTable::find_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର *ଚାବି, Resource_ମୂଲ୍ୟ &value) ଅଚଳ {
    icu::Resource_Data_ମୂଲ୍ୟ &rd_ମୂଲ୍ୟ = static_cast<icu::Resource_Data_ମୂଲ୍ୟ &>(value);
    ଅଚଳ ଅକ୍ଷର *real_ଚାବି = nullptr;
    int32_t i;
    ଯଦି (keys16 != nullptr) {
        i = _res_findTableItem(&rd_ମୂଲ୍ୟ.getData(), keys16, length, ଚାବି, &real_ଚାବି);
    } ଅନ୍ୟଥା {
        i = _res_findTable32Item(&rd_ମୂଲ୍ୟ.getData(), keys32, length, ଚାବି, &real_ଚାବି);
    }
    ଯଦି (i >= 0) {
        Resource res;
        ଯଦି (items16 != nullptr) {
            res = makeResourceFrom16(&rd_ମୂଲ୍ୟ.getData(), items16[i]);
        } ଅନ୍ୟଥା {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_ମୂଲ୍ୟ.setResource(res, ResourceTracer(fTraceInfo, ଚାବି));
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ଅଚଳ ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    ଯଦି (indexR < 0) {
        ଫେରନ୍ତୁ RES_BOGUS;
    }
    ବାଛ(RES_GET_TYPE(array)) {
    ସ୍ଥିତି URES_ARRAY: {
        ଯଦି (offset!=0) { /* empty if offset==0 */
            ଅଚଳ int32_t *p= pResData->p_ମୂଳ+offset;
            ଯଦି(indexR<*p) {
                ଫେରନ୍ତୁ (Resource)p[1+indexR];
            }
        }
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି URES_ARRAY16: {
        ଅଚଳ uint16_t *p=pResData->p16BitUnits+offset;
        ଯଦି(indexR<*p) {
            ଫେରନ୍ତୁ makeResourceFrom16(pResData, p[1+indexR]);
        }
        ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(ଅଚଳ ResourceData *pResData, int32_t i) ଅଚଳ {
    ଯଦି (items16 != nullptr) {
        ଫେରନ୍ତୁ makeResourceFrom16(pResData, items16[i]);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ items32[i];
    }
}

UBool icu::ResourceArray::get_ମୂଲ୍ୟ(int32_t i, icu::Resource_ମୂଲ୍ୟ &value) ଅଚଳ {
    ଯଦି(0 <= i && i < length) {
        icu::Resource_Data_ମୂଲ୍ୟ &rd_ମୂଲ୍ୟ = static_cast<icu::Resource_Data_ମୂଲ୍ୟ &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ମୂଲ୍ୟ.setResource(
            internalGetResource(&rd_ମୂଲ୍ୟ.getData(), i),
            ResourceTracer(fTraceInfo, i));
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

U_CFUNC Resource
res_findResource(ଅଚଳ ResourceData *pResData, Resource r, ଅକ୍ଷର** path, ଅଚଳ ଅକ୍ଷର** ଚାବି) {
  ଅକ୍ଷର *pathP = *path, *nextSepP = *path;
  ଅକ୍ଷର *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  ଯଦି(!uprv_strlen(pathP)) {
      ଫେରନ୍ତୁ r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  ଯଦି(!URES_IS_CONTAINER(type)) {
      ଫେରନ୍ତୁ RES_BOGUS;
  }
  
  ଯେତେବେଳେ(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    ଯଦି(nextSepP != nullptr) {
      ଯଦି(nextSepP == pathP) {
        // Empty key string.
        ଫେରନ୍ତୁ RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } ଅନ୍ୟଥା {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    ଯଦି(URES_IS_TABLE(type)) {
      *ଚାବି = pathP;
      t2 = res_get_Table_Item_By_ଚାବି(pResData, t1, &indexR, ଚାବି);
    } ଅନ୍ୟଥା ଯଦି(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      ଯଦି(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } ଅନ୍ୟଥା {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *ଚାବି = nullptr;
    } ଅନ୍ୟଥା { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  ଫେରନ୍ତୁ t1;
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା Row {
    int32_t ଚାବି_Index, sortIndex;
} Row;

ସ୍ଥିର int32_t U_CALLCONV
ures_compareRows(ଅଚଳ ଶୂନ୍ୟ *context, ଅଚଳ ଶୂନ୍ୟ *left, ଅଚଳ ଶୂନ୍ୟ *ଡାହାଣ) {
    ଅଚଳ ଅକ୍ଷର* ଚାବି_Chars = static_cast<ଅଚଳ ଅକ୍ଷର*>(context);
    ଫେରନ୍ତୁ static_cast<int32_t>(uprv_strcmp(ଚାବି_Chars + static_cast<ଅଚଳ Row*>(left)->ଚାବି_Index,
                                            ଚାବି_Chars + static_cast<ଅଚଳ Row*>(ଡାହାଣ)->ଚାବି_Index));
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା TempTable {
    ଅଚଳ ଅକ୍ଷର *ଚାବି_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_ଚାବି_Limit;
    uint8_t majorFormatVersion;
} TempTable;

ଗଣନା {
    ଗଦା_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ g_Unknown_ଚାବି="";

#ଯଦି !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ସ୍ଥିର ଅଚଳ char16_t g_Collation_Bin_ଚାବି[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
ସ୍ଥିର ଶୂନ୍ୟ
ures_swapResource(ଅଚଳ UDataSwapper *ds,
                  ଅଚଳ Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  ଅଚଳ ଅକ୍ଷର *ଚାବି,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    ଅଚଳ Resource *p;
    Resource *q;
    int32_t offset, count;

    ବାଛ(RES_GET_TYPE(res)) {
    ସ୍ଥିତି URES_TABLE16:
    ସ୍ଥିତି URES_STRING_V2:
    ସ୍ଥିତି URES_INT:
    ସ୍ଥିତି URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        ଫେରନ୍ତୁ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    ଯଦି(offset==0) {
        /* special offset indicating an empty item */
        ଫେରନ୍ତୁ;
    }
    ଯଦି (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        ଫେରନ୍ତୁ;
    } ଅନ୍ୟଥା {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    ବାଛ(RES_GET_TYPE(res)) {
    ସ୍ଥିତି URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    ସ୍ଥିତି URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି URES_ଦ୍ୱିମିକ:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#ଯଦି !UCONFIG_NO_COLLATION
        ଯଦି( ଚାବି!=nullptr &&  /* the binary is in a table */
            (ଚାବି!=g_Unknown_ଚାବି ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, ଚାବି, -1,
                                       g_Collation_Bin_ଚାବି, UPRV_LENGTHOF(g_Collation_Bin_ଚାବି)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_ଦ୍ୱିମିକ(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#endif
        ଭାଙ୍ଗ;
    ସ୍ଥିତି URES_TABLE:
    ସ୍ଥିତି URES_TABLE32:
        {
            ଅଚଳ uint16_t *pKey16;
            uint16_t *qKey16;

            ଅଚଳ int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            ଯଦି(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<ଅଚଳ uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } ଅନ୍ୟଥା {
                /* get table item count */
                pKey32 = reinterpret_cast<ଅଚଳ int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            ଯଦି(count==0) {
                ଭାଙ୍ଗ;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            ପାଇଁ(i=0; i<count; ++i) {
                ଅଚଳ ଅକ୍ଷର *item_ଚାବି=g_Unknown_ଚାବି;
                ଯଦି(pKey16!=nullptr) {
                    int32_t ଚାବି_Offset=ds->readUInt16(pKey16[i]);
                    ଯଦି(ଚାବି_Offset<pTempTable->local_ଚାବି_Limit) {
                        item_ଚାବି = reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(outBundle) + ଚାବି_Offset;
                    }
                } ଅନ୍ୟଥା {
                    int32_t ଚାବି_Offset=udata_readInt32(ds, pKey32[i]);
                    ଯଦି(ଚାବି_Offset>=0) {
                        item_ଚାବି = reinterpret_cast<ଅଚଳ ଅକ୍ଷର*>(outBundle) + ଚାବି_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_ଚାବି, pTempTable, pErrorCode);
                ଯଦି(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    ଫେରନ୍ତୁ;
                }
            }

            ଯଦି(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                ଯଦି(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } ଅନ୍ୟଥା {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                ଭାଙ୍ଗ;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            ଯଦି(pKey16!=nullptr) {
                ପାଇଁ(i=0; i<count; ++i) {
                    pTempTable->rows[i].ଚାବି_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } ଅନ୍ୟଥା {
                ପାଇଁ(i=0; i<count; ++i) {
                    pTempTable->rows[i].ଚାବି_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, ଆକାର(Row),
                           ures_compareRows, pTempTable->ଚାବି_Chars,
                           ମିଥ୍ୟା, pErrorCode);
            ଯଦି(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                ଫେରନ୍ତୁ;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*ଚାବିଗୁଡ଼ିକ*/
            ଯଦି(pKey16!=nullptr) {
                uint16_t *rKey16;

                ଯଦି(pKey16!=qKey16) {
                    rKey16=qKey16;
                } ଅନ୍ୟଥା {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                ପାଇଁ(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                ଯଦି(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } ଅନ୍ୟଥା {
                int32_t *rKey32;

                ଯଦି(pKey32!=qKey32) {
                    rKey32=qKey32;
                } ଅନ୍ୟଥା {
                    rKey32=pTempTable->resort;
                }
                ପାଇଁ(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                ଯଦି(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                ଯଦି(p!=q) {
                    r=q;
                } ଅନ୍ୟଥା {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                ପାଇଁ(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                ଯଦି(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            ପାଇଁ(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                ଯଦି(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    ଫେରନ୍ତୁ;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ଭାଙ୍ଗ;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(ଅଚଳ UDataSwapper *ds,
          ଅଚଳ ଶୂନ୍ୟ *inData, int32_t length, ଶୂନ୍ୟ *outData,
          UErrorCode *pErrorCode) {
    ଅଚଳ UDataInfo *pInfo;
    ଅଚଳ Resource *inBundle;
    Resource ମୂଳ_Res;
    int32_t headerSize, maxTableLength;

    Row rows[ଗଦା_ROW_CAPACITY];
    int32_t resort[ଗଦା_ROW_CAPACITY];
    TempTable tempTable;

    ଅଚଳ int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, ଚାବିଗୁଡ଼ିକ_Bottom, ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ, resBottom, ଶୀର୍ଷ;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }

    /* check data format and format version */
    pInfo=(ଅଚଳ UDataInfo *)((ଅଚଳ ଅକ୍ଷର *)inData+4);
    ଯଦି(!(
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
        ଫେରନ୍ତୁ 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    ଯଦି(length<0) {
        bundleLength=-1;
    } ଅନ୍ୟଥା {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        ଯଦି(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }
    }

    inBundle=(ଅଚଳ Resource *)((ଅଚଳ ଅକ୍ଷର *)inData+headerSize);
    ମୂଳ_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(ଅଚଳ int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    ଯଦି(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ଚାବିଗୁଡ଼ିକ_Bottom=1+indexLength;
    ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ=udata_readInt32(ds, inIndexes[URES_INDEX_ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ]);
    ଯଦି(indexLength>URES_INDEX_16_BIT_ଶୀର୍ଷ) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_ଶୀର୍ଷ]);
    } ଅନ୍ୟଥା {
        resBottom=ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ;
    }
    ଶୀର୍ଷ=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_ଶୀର୍ଷ]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    ଯଦି(0<=bundleLength && bundleLength<ଶୀର୍ଷ) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         ଶୀର୍ଷ, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ>(1+indexLength)) {
        tempTable.local_ଚାବି_Limit=ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ<<2;
    } ଅନ୍ୟଥା {
        tempTable.local_ଚାବି_Limit=0;
    }

    ଯଦି(length>=0) {
        Resource *outBundle=(Resource *)((ଅକ୍ଷର *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t ଗଦା_Res_Flags[ଗଦା_ROW_CAPACITY];
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
        ଯଦି(resFlagsLength<=(int32_t)ଆକାର(ଗଦା_Res_Flags)) {
            tempTable.resFlags=ଗଦା_Res_Flags;
        } ଅନ୍ୟଥା {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            ଯଦି(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        ଯଦି(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*ଶୀର୍ଷ);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+ଚାବିଗୁଡ଼ିକ_Bottom, 4*(ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ-ଚାବିଗୁଡ଼ିକ_Bottom),
                                    outBundle+ଚାବିଗୁଡ଼ିକ_Bottom, pErrorCode);
        ଯଦି(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ-ଚାବିଗୁଡ଼ିକ_Bottom));
            ଯଦି(tempTable.resFlags!=ଗଦା_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            ଫେରନ୍ତୁ 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        ଯଦି(ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ<resBottom) {
            ds->swapArray16(ds, inBundle+ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ, (resBottom-ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ)*4, outBundle+ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ, pErrorCode);
            ଯଦି(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-ଚାବିଗୁଡ଼ିକ_ଶୀର୍ଷ));
                ଯଦି(tempTable.resFlags!=ଗଦା_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                ଫେରନ୍ତୁ 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.ଚାବି_Chars=(ଅଚଳ ଅକ୍ଷର *)outBundle; /* sort by outCharset */
        ଯଦି(tempTable.majorFormatVersion>1 || maxTableLength<=ଗଦା_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } ଅନ୍ୟଥା {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*ଆକାର(Row)+maxTableLength*4);
            ଯଦି(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ଯଦି(tempTable.resFlags!=ଗଦା_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                ଫେରନ୍ତୁ 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, ମୂଳ_Res, nullptr, &tempTable, pErrorCode);
        ଯଦି(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             ମୂଳ_Res);
        }

        ଯଦି(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        ଯଦି(tempTable.resFlags!=ଗଦା_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, ଚାବିଗୁଡ଼ିକ_Bottom*4, outBundle, pErrorCode);
    }

    ଫେରନ୍ତୁ headerSize+4*ଶୀର୍ଷ;
}

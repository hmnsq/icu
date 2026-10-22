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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "resource.h"
#அந்தர்பாவ "uarrsort.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucol_swp.h"
#அந்தர்பாவ "udataswp.h"
#அந்தர்பாவ "uinvchar.h"
#அந்தர்பாவ "uresdata.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, சாவி_Offset) \
    ((சாவி_Offset)<(pResData)->local_சாவி_Limit ? \
        (மாறிலி எழுத்து *)(pResData)->p_வேர்+(சாவி_Offset) : \
        (pResData)->pool_Bundle_சாவிகள்+(சாவி_Offset)-(pResData)->local_சாவி_Limit)

#define RES_GET_KEY32(pResData, சாவி_Offset) \
    ((சாவி_Offset)>=0 ? \
        (மாறிலி எழுத்து *)(pResData)->p_வேர்+(சாவி_Offset) : \
        (pResData)->pool_Bundle_சாவிகள்+((சாவி_Offset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
நிலையான மாறிலி uint16_t gEmpty16=0;

நிலையான மாறிலி கட்டமைப்பு {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

நிலையான மாறிலி கட்டமைப்பு {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

நிலையான int32_t
_res_findTableItem(மாறிலி ResourceData *pResData, மாறிலி uint16_t *சாவி_Offsets, int32_t length,
                   மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து **real_சாவி) {
    மாறிலி எழுத்து *table_சாவி;
    int32_t mid, start, limit;
    முழு result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    போது(start<limit) {
        mid = (start + limit) / 2;
        table_சாவி = RES_GET_KEY16(pResData, சாவி_Offsets[mid]);
        எனில் (pResData->useNativeStrcmp) {
            result = uprv_strcmp(சாவி, table_சாவி);
        } இல்லையெனில் {
            result = uprv_compareInvCharsAsAscii(சாவி, table_சாவி);
        }
        எனில் (result < 0) {
            limit = mid;
        } இல்லையெனில் எனில் (result > 0) {
            start = mid + 1;
        } இல்லையெனில் {
            /* We found it! */
            *real_சாவி=table_சாவி;
            திரும்பு mid;
        }
    }
    திரும்பு URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

நிலையான int32_t
_res_findTable32Item(மாறிலி ResourceData *pResData, மாறிலி int32_t *சாவி_Offsets, int32_t length,
                     மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து **real_சாவி) {
    மாறிலி எழுத்து *table_சாவி;
    int32_t mid, start, limit;
    முழு result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    போது(start<limit) {
        mid = (start + limit) / 2;
        table_சாவி = RES_GET_KEY32(pResData, சாவி_Offsets[mid]);
        எனில் (pResData->useNativeStrcmp) {
            result = uprv_strcmp(சாவி, table_சாவி);
        } இல்லையெனில் {
            result = uprv_compareInvCharsAsAscii(சாவி, table_சாவி);
        }
        எனில் (result < 0) {
            limit = mid;
        } இல்லையெனில் எனில் (result > 0) {
            start = mid + 1;
        } இல்லையெனில் {
            /* We found it! */
            *real_சாவி=table_சாவி;
            திரும்பு mid;
        }
    }
    திரும்பு URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

நிலையான UBool U_CALLCONV
isAcceptable(வெறுமை *context,
             மாறிலி எழுத்து * /*type*/, மாறிலி எழுத்து * /*name*/,
             மாறிலி UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    திரும்பு
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

நிலையான வெறுமை
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, மாறிலி வெறுமை *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType வேர்_Type;

    /* get the root resource */
    pResData->p_வேர் = static_cast<மாறிலி int32_t*>(inBytes);
    pResData->வேர்_Res = static_cast<Resource>(*pResData->p_வேர்);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    எனில்(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        திரும்பு;
    }

    /* currently, we accept only resources that have a Table as their roots */
    வேர்_Type = static_cast<UResType>(RES_GET_TYPE(pResData->வேர்_Res));
    எனில்(!URES_IS_TABLE(வேர்_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        திரும்பு;
    }

    எனில்(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_சாவி_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } இல்லையெனில் {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        மாறிலி int32_t *indexes=pResData->p_வேர்+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        எனில்(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            திரும்பு;
        }
        எனில்( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_உச்சி]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            திரும்பு;
        }
        எனில்(indexes[URES_INDEX_சாவிகள்_உச்சி]>(1+indexLength)) {
            pResData->local_சாவி_Limit=indexes[URES_INDEX_சாவிகள்_உச்சி]<<2;
        }
        எனில்(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        எனில்(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        எனில்((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            திரும்பு;
        }
        எனில்( indexLength>URES_INDEX_16_BIT_உச்சி &&
            indexes[URES_INDEX_16_BIT_உச்சி]>indexes[URES_INDEX_சாவிகள்_உச்சி]
        ) {
            pResData->p16BitUnits = reinterpret_cast<மாறிலி uint16_t*>(pResData->p_வேர் + indexes[URES_INDEX_சாவிகள்_உச்சி]);
        }
    }

    எனில்(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=உண்மை;
    }
}

U_CAPI வெறுமை U_EXPORT2
res_read(ResourceData *pResData,
         மாறிலி UDataInfo *pInfo, மாறிலி வெறுமை *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, அளவு(ResourceData));
    எனில்(U_FAILURE(*errorCode)) {
        திரும்பு;
    }
    எனில்(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        திரும்பு;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC வெறுமை
res_load(ResourceData *pResData,
         மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, அளவு(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    எனில்(U_FAILURE(*errorCode)) {
        திரும்பு;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC வெறுமை
res_unload(ResourceData *pResData) {
    எனில்(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

நிலையான மாறிலி int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_இருமம்,
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
    திரும்பு (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI மாறிலி char16_t * U_EXPORT2
res_getStringNoTrace(மாறிலி ResourceData *pResData, Resource res, int32_t *pLength) {
    மாறிலி char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    எனில்(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        எனில்((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(மாறிலி char16_t *)pResData->poolBundleStrings+offset;
        } இல்லையெனில் {
            p=(மாறிலி char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        எனில்(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } இல்லையெனில் எனில்(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } இல்லையெனில் எனில்(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } இல்லையெனில் {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } இல்லையெனில் எனில்(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        மாறிலி int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_வேர்+res;
        length=*p32++;
        p=(மாறிலி char16_t *)p32;
    } இல்லையெனில் {
        p=nullptr;
        length=0;
    }
    எனில்(pLength) {
        *pLength=length;
    }
    திரும்பு p;
}

நாமாகாஶ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(மாறிலி ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    எனில் (offset == 0) {
        // empty string
    } இல்லையெனில் எனில் (res == offset) {
        மாறிலி int32_t *p32=pResData->p_வேர்+res;
        int32_t length=*p32;
        மாறிலி char16_t* p = reinterpret_cast<மாறிலி char16_t*>(p32);
        திரும்பு length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } இல்லையெனில் எனில் (RES_GET_TYPE(res) == URES_STRING_V2) {
        மாறிலி char16_t *p;
        எனில் (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<மாறிலி char16_t*>(pResData->poolBundleStrings) + offset;
        } இல்லையெனில் {
            p = reinterpret_cast<மாறிலி char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        எனில் (first == 0x2205) {  // implicit length
            திரும்பு p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } இல்லையெனில் எனில் (first == 0xdc03) {  // explicit length 3 (should not occur)
            திரும்பு p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } இல்லையெனில் {
            // Assume that the string has not been stored with more length units than necessary.
            திரும்பு பொய்;
        }
    }
    திரும்பு பொய்;
}

int32_t getStringArray(மாறிலி ResourceData *pResData, மாறிலி icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    எனில்(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    int32_t length = array.getSize();
    எனில்(length == 0) {
        திரும்பு 0;
    }
    எனில்(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        திரும்பு length;
    }
    க்கு(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        மாறிலி char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        எனில்(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            திரும்பு 0;
        }
        dest[i].setTo(உண்மை, s, sLength);
    }
    திரும்பு length;
}

}  // namespace

U_CAPI மாறிலி char16_t * U_EXPORT2
res_getAlias(மாறிலி ResourceData *pResData, Resource res, int32_t *pLength) {
    மாறிலி char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    எனில்(RES_GET_TYPE(res)==URES_ALIAS) {
        மாறிலி int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_வேர்+offset;
        length=*p32++;
        p=(மாறிலி char16_t *)p32;
    } இல்லையெனில் {
        p=nullptr;
        length=0;
    }
    எனில்(pLength) {
        *pLength=length;
    }
    திரும்பு p;
}

U_CAPI மாறிலி uint8_t * U_EXPORT2
res_get_இருமம்_No_Trace(மாறிலி ResourceData *pResData, Resource res, int32_t *pLength) {
    மாறிலி uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    எனில்(RES_GET_TYPE(res)==URES_இருமம்) {
        மாறிலி int32_t *p32= offset==0 ? (மாறிலி int32_t*)&gEmpty32 : pResData->p_வேர்+offset;
        length=*p32++;
        p=(மாறிலி uint8_t *)p32;
    } இல்லையெனில் {
        p=nullptr;
        length=0;
    }
    எனில்(pLength) {
        *pLength=length;
    }
    திரும்பு p;
}


U_CAPI மாறிலி int32_t * U_EXPORT2
res_getIntVectorNoTrace(மாறிலி ResourceData *pResData, Resource res, int32_t *pLength) {
    மாறிலி int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    எனில்(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (மாறிலி int32_t *)&gEmpty32 : pResData->p_வேர்+offset;
        length=*p++;
    } இல்லையெனில் {
        p=nullptr;
        length=0;
    }
    எனில்(pLength) {
        *pLength=length;
    }
    திரும்பு p;
}

U_CAPI int32_t U_EXPORT2
res_மொத்தம்_Array_பொருட்கள்(மாறிலி ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    தேர்வு(RES_GET_TYPE(res)) {
    நிலை URES_STRING:
    நிலை URES_STRING_V2:
    நிலை URES_இருமம்:
    நிலை URES_ALIAS:
    நிலை URES_INT:
    நிலை URES_INT_VECTOR:
        திரும்பு 1;
    நிலை URES_ARRAY:
    நிலை URES_TABLE32:
        திரும்பு offset==0 ? 0 : *(pResData->p_வேர்+offset);
    நிலை URES_TABLE:
        திரும்பு offset==0 ? 0 : *((மாறிலி uint16_t *)(pResData->p_வேர்+offset));
    நிலை URES_ARRAY16:
    நிலை URES_TABLE16:
        திரும்பு pResData->p16BitUnits[offset];
    இயல்பு:
        திரும்பு 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_மதிப்பு::~Resource_Data_மதிப்பு() {}

UResType Resource_Data_மதிப்பு::getType() மாறிலி {
    திரும்பு res_getPublicType(res);
}

மாறிலி char16_t *Resource_Data_மதிப்பு::getString(int32_t &length, UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    மாறிலி char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    எனில்(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    திரும்பு s;
}

மாறிலி char16_t *Resource_Data_மதிப்பு::getAliasString(int32_t &length, UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    மாறிலி char16_t *s = res_getAlias(&getData(), res, &length);
    எனில்(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    திரும்பு s;
}

int32_t Resource_Data_மதிப்பு::getInt(UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    எனில்(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    திரும்பு res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_மதிப்பு::getUInt(UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    எனில்(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    திரும்பு res_getUInt(fTraceInfo, res);
}

மாறிலி int32_t *Resource_Data_மதிப்பு::getIntVector(int32_t &length, UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    மாறிலி int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    எனில்(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    திரும்பு iv;
}

மாறிலி uint8_t *Resource_Data_மதிப்பு::get_இருமம்(int32_t &length, UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    மாறிலி uint8_t *b = res_get_இருமம்(fTraceInfo, &getData(), res, &length);
    எனில்(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    திரும்பு b;
}

ResourceArray Resource_Data_மதிப்பு::getArray(UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு {};
    }
    மாறிலி uint16_t *items16 = nullptr;
    மாறிலி Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    தேர்வு(RES_GET_TYPE(res)) {
    நிலை URES_ARRAY:
        எனில் (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<மாறிலி Resource*>(getData().p_வேர்) + offset;
            length = *items32++;
        }
        நிறுத்து;
    நிலை URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        நிறுத்து;
    இயல்பு:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        திரும்பு {};
    }
    திரும்பு ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_Data_மதிப்பு::getTable(UErrorCode &errorCode) மாறிலி {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு {};
    }
    மாறிலி uint16_t *keys16 = nullptr;
    மாறிலி int32_t *keys32 = nullptr;
    மாறிலி uint16_t *items16 = nullptr;
    மாறிலி Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    தேர்வு(RES_GET_TYPE(res)) {
    நிலை URES_TABLE:
        எனில் (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<மாறிலி uint16_t*>(getData().p_வேர் + offset);
            length = *keys16++;
            items32 = reinterpret_cast<மாறிலி Resource*>(keys16 + length + (~length & 1));
        }
        நிறுத்து;
    நிலை URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        நிறுத்து;
    நிலை URES_TABLE32:
        எனில் (offset != 0) {  // empty if offset==0
            keys32 = getData().p_வேர்+offset;
            length = *keys32++;
            items32 = reinterpret_cast<மாறிலி Resource*>(keys32) + length;
        }
        நிறுத்து;
    இயல்பு:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        திரும்பு {};
    }
    திரும்பு ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_Data_மதிப்பு::isNoInheritanceMarker() மாறிலி {
    திரும்பு ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_மதிப்பு::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) மாறிலி {
    திரும்பு ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_மதிப்பு::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) மாறிலி {
    எனில்(URES_IS_ARRAY(res)) {
        திரும்பு ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    எனில்(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    எனில்(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        திரும்பு 1;
    }
    int32_t sLength;
    மாறிலி char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    எனில்(s != nullptr) {
        dest[0].setTo(உண்மை, s, sLength);
        திரும்பு 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    திரும்பு 0;
}

UnicodeString Resource_Data_மதிப்பு::getStringOrFirstOfArray(UErrorCode &errorCode) மாறிலி {
    UnicodeString us;
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு us;
    }
    int32_t sLength;
    மாறிலி char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    எனில்(s != nullptr) {
        us.setTo(உண்மை, s, sLength);
        திரும்பு us;
    }
    ResourceArray array = getArray(errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு us;
    }
    எனில்(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        எனில்(s != nullptr) {
            us.setTo(உண்மை, s, sLength);
            திரும்பு us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    திரும்பு us;
}

U_NAMESPACE_END

நிலையான Resource
makeResourceFrom16(மாறிலி ResourceData *pResData, int32_t res16) {
    எனில்(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } இல்லையெனில் {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    திரும்பு URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_சாவி(மாறிலி ResourceData *pResData, Resource table,
                      int32_t *indexR, மாறிலி எழுத்து **சாவி) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    எனில்(சாவி == nullptr || *சாவி == nullptr) {
        திரும்பு RES_BOGUS;
    }
    தேர்வு(RES_GET_TYPE(table)) {
    நிலை URES_TABLE: {
        எனில் (offset!=0) { /* empty if offset==0 */
            மாறிலி uint16_t *p= (மாறிலி uint16_t *)(pResData->p_வேர்+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *சாவி, சாவி);
            எனில்(idx>=0) {
                மாறிலி Resource *p32=(மாறிலி Resource *)(p+length+(~length&1));
                திரும்பு p32[idx];
            }
        }
        நிறுத்து;
    }
    நிலை URES_TABLE16: {
        மாறிலி uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *சாவி, சாவி);
        எனில்(idx>=0) {
            திரும்பு makeResourceFrom16(pResData, p[length+idx]);
        }
        நிறுத்து;
    }
    நிலை URES_TABLE32: {
        எனில் (offset!=0) { /* empty if offset==0 */
            மாறிலி int32_t *p= pResData->p_வேர்+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *சாவி, சாவி);
            எனில்(idx>=0) {
                திரும்பு (Resource)p[length+idx];
            }
        }
        நிறுத்து;
    }
    இயல்பு:
        நிறுத்து;
    }
    திரும்பு RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(மாறிலி ResourceData *pResData, Resource table,
                        int32_t indexR, மாறிலி எழுத்து **சாவி) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    எனில் (indexR < 0) {
        திரும்பு RES_BOGUS;
    }
    தேர்வு(RES_GET_TYPE(table)) {
    நிலை URES_TABLE: {
        எனில் (offset != 0) { /* empty if offset==0 */
            மாறிலி uint16_t *p= (மாறிலி uint16_t *)(pResData->p_வேர்+offset);
            length=*p++;
            எனில்(indexR<length) {
                மாறிலி Resource *p32=(மாறிலி Resource *)(p+length+(~length&1));
                எனில்(சாவி!=nullptr) {
                    *சாவி=RES_GET_KEY16(pResData, p[indexR]);
                }
                திரும்பு p32[indexR];
            }
        }
        நிறுத்து;
    }
    நிலை URES_TABLE16: {
        மாறிலி uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        எனில்(indexR<length) {
            எனில்(சாவி!=nullptr) {
                *சாவி=RES_GET_KEY16(pResData, p[indexR]);
            }
            திரும்பு makeResourceFrom16(pResData, p[length+indexR]);
        }
        நிறுத்து;
    }
    நிலை URES_TABLE32: {
        எனில் (offset != 0) { /* empty if offset==0 */
            மாறிலி int32_t *p= pResData->p_வேர்+offset;
            length=*p++;
            எனில்(indexR<length) {
                எனில்(சாவி!=nullptr) {
                    *சாவி=RES_GET_KEY32(pResData, p[indexR]);
                }
                திரும்பு (Resource)p[length+indexR];
            }
        }
        நிறுத்து;
    }
    இயல்பு:
        நிறுத்து;
    }
    திரும்பு RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(மாறிலி ResourceData *pResData, மாறிலி எழுத்து *சாவி) {
    மாறிலி எழுத்து *real_சாவி=சாவி;
    int32_t idx;
    திரும்பு res_get_Table_Item_By_சாவி(pResData, pResData->வேர்_Res, &idx, &real_சாவி);
}


UBool icu::ResourceTable::get_சாவி_And_மதிப்பு(int32_t i,
                                         மாறிலி எழுத்து *&சாவி, icu::Resource_மதிப்பு &value) மாறிலி {
    எனில்(0 <= i && i < length) {
        icu::Resource_Data_மதிப்பு &rd_மதிப்பு = static_cast<icu::Resource_Data_மதிப்பு &>(value);
        எனில் (keys16 != nullptr) {
            சாவி = RES_GET_KEY16(&rd_மதிப்பு.getData(), keys16[i]);
        } இல்லையெனில் {
            சாவி = RES_GET_KEY32(&rd_மதிப்பு.getData(), keys32[i]);
        }
        Resource res;
        எனில் (items16 != nullptr) {
            res = makeResourceFrom16(&rd_மதிப்பு.getData(), items16[i]);
        } இல்லையெனில் {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_மதிப்பு.setResource(res, ResourceTracer(fTraceInfo, சாவி));
        திரும்பு உண்மை;
    }
    திரும்பு பொய்;
}

UBool icu::ResourceTable::find_மதிப்பு(மாறிலி எழுத்து *சாவி, Resource_மதிப்பு &value) மாறிலி {
    icu::Resource_Data_மதிப்பு &rd_மதிப்பு = static_cast<icu::Resource_Data_மதிப்பு &>(value);
    மாறிலி எழுத்து *real_சாவி = nullptr;
    int32_t i;
    எனில் (keys16 != nullptr) {
        i = _res_findTableItem(&rd_மதிப்பு.getData(), keys16, length, சாவி, &real_சாவி);
    } இல்லையெனில் {
        i = _res_findTable32Item(&rd_மதிப்பு.getData(), keys32, length, சாவி, &real_சாவி);
    }
    எனில் (i >= 0) {
        Resource res;
        எனில் (items16 != nullptr) {
            res = makeResourceFrom16(&rd_மதிப்பு.getData(), items16[i]);
        } இல்லையெனில் {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_மதிப்பு.setResource(res, ResourceTracer(fTraceInfo, சாவி));
        திரும்பு உண்மை;
    }
    திரும்பு பொய்;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(மாறிலி ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    எனில் (indexR < 0) {
        திரும்பு RES_BOGUS;
    }
    தேர்வு(RES_GET_TYPE(array)) {
    நிலை URES_ARRAY: {
        எனில் (offset!=0) { /* empty if offset==0 */
            மாறிலி int32_t *p= pResData->p_வேர்+offset;
            எனில்(indexR<*p) {
                திரும்பு (Resource)p[1+indexR];
            }
        }
        நிறுத்து;
    }
    நிலை URES_ARRAY16: {
        மாறிலி uint16_t *p=pResData->p16BitUnits+offset;
        எனில்(indexR<*p) {
            திரும்பு makeResourceFrom16(pResData, p[1+indexR]);
        }
        நிறுத்து;
    }
    இயல்பு:
        நிறுத்து;
    }
    திரும்பு RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(மாறிலி ResourceData *pResData, int32_t i) மாறிலி {
    எனில் (items16 != nullptr) {
        திரும்பு makeResourceFrom16(pResData, items16[i]);
    } இல்லையெனில் {
        திரும்பு items32[i];
    }
}

UBool icu::ResourceArray::get_மதிப்பு(int32_t i, icu::Resource_மதிப்பு &value) மாறிலி {
    எனில்(0 <= i && i < length) {
        icu::Resource_Data_மதிப்பு &rd_மதிப்பு = static_cast<icu::Resource_Data_மதிப்பு &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_மதிப்பு.setResource(
            internalGetResource(&rd_மதிப்பு.getData(), i),
            ResourceTracer(fTraceInfo, i));
        திரும்பு உண்மை;
    }
    திரும்பு பொய்;
}

U_CFUNC Resource
res_findResource(மாறிலி ResourceData *pResData, Resource r, எழுத்து** path, மாறிலி எழுத்து** சாவி) {
  எழுத்து *pathP = *path, *nextSepP = *path;
  எழுத்து *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  எனில்(!uprv_strlen(pathP)) {
      திரும்பு r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  எனில்(!URES_IS_CONTAINER(type)) {
      திரும்பு RES_BOGUS;
  }
  
  போது(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    எனில்(nextSepP != nullptr) {
      எனில்(nextSepP == pathP) {
        // Empty key string.
        திரும்பு RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } இல்லையெனில் {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    எனில்(URES_IS_TABLE(type)) {
      *சாவி = pathP;
      t2 = res_get_Table_Item_By_சாவி(pResData, t1, &indexR, சாவி);
    } இல்லையெனில் எனில்(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      எனில்(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } இல்லையெனில் {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *சாவி = nullptr;
    } இல்லையெனில் { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  திரும்பு t1;
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
வகைவரை கட்டமைப்பு Row {
    int32_t சாவி_Index, sortIndex;
} Row;

நிலையான int32_t U_CALLCONV
ures_compareRows(மாறிலி வெறுமை *context, மாறிலி வெறுமை *left, மாறிலி வெறுமை *வலது) {
    மாறிலி எழுத்து* சாவி_Chars = static_cast<மாறிலி எழுத்து*>(context);
    திரும்பு static_cast<int32_t>(uprv_strcmp(சாவி_Chars + static_cast<மாறிலி Row*>(left)->சாவி_Index,
                                            சாவி_Chars + static_cast<மாறிலி Row*>(வலது)->சாவி_Index));
}

வகைவரை கட்டமைப்பு TempTable {
    மாறிலி எழுத்து *சாவி_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_சாவி_Limit;
    uint8_t majorFormatVersion;
} TempTable;

எண்ணிக்கை {
    அடுக்கு_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
நிலையான மாறிலி எழுத்து *மாறிலி g_Unknown_சாவி="";

#எனில் !UCONFIG_NO_COLLATION
// resource table key for collation binaries
நிலையான மாறிலி char16_t g_Collation_Bin_சாவி[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
நிலையான வெறுமை
ures_swapResource(மாறிலி UDataSwapper *ds,
                  மாறிலி Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  மாறிலி எழுத்து *சாவி,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    மாறிலி Resource *p;
    Resource *q;
    int32_t offset, count;

    தேர்வு(RES_GET_TYPE(res)) {
    நிலை URES_TABLE16:
    நிலை URES_STRING_V2:
    நிலை URES_INT:
    நிலை URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        திரும்பு;
    இயல்பு:
        நிறுத்து;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    எனில்(offset==0) {
        /* special offset indicating an empty item */
        திரும்பு;
    }
    எனில் (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        திரும்பு;
    } இல்லையெனில் {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    தேர்வு(RES_GET_TYPE(res)) {
    நிலை URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    நிலை URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        நிறுத்து;
    நிலை URES_இருமம்:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#எனில் !UCONFIG_NO_COLLATION
        எனில்( சாவி!=nullptr &&  /* the binary is in a table */
            (சாவி!=g_Unknown_சாவி ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, சாவி, -1,
                                       g_Collation_Bin_சாவி, UPRV_LENGTHOF(g_Collation_Bin_சாவி)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_இருமம்(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#endif
        நிறுத்து;
    நிலை URES_TABLE:
    நிலை URES_TABLE32:
        {
            மாறிலி uint16_t *pKey16;
            uint16_t *qKey16;

            மாறிலி int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            எனில்(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<மாறிலி uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } இல்லையெனில் {
                /* get table item count */
                pKey32 = reinterpret_cast<மாறிலி int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            எனில்(count==0) {
                நிறுத்து;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            க்கு(i=0; i<count; ++i) {
                மாறிலி எழுத்து *item_சாவி=g_Unknown_சாவி;
                எனில்(pKey16!=nullptr) {
                    int32_t சாவி_Offset=ds->readUInt16(pKey16[i]);
                    எனில்(சாவி_Offset<pTempTable->local_சாவி_Limit) {
                        item_சாவி = reinterpret_cast<மாறிலி எழுத்து*>(outBundle) + சாவி_Offset;
                    }
                } இல்லையெனில் {
                    int32_t சாவி_Offset=udata_readInt32(ds, pKey32[i]);
                    எனில்(சாவி_Offset>=0) {
                        item_சாவி = reinterpret_cast<மாறிலி எழுத்து*>(outBundle) + சாவி_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_சாவி, pTempTable, pErrorCode);
                எனில்(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    திரும்பு;
                }
            }

            எனில்(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                எனில்(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } இல்லையெனில் {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                நிறுத்து;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            எனில்(pKey16!=nullptr) {
                க்கு(i=0; i<count; ++i) {
                    pTempTable->rows[i].சாவி_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } இல்லையெனில் {
                க்கு(i=0; i<count; ++i) {
                    pTempTable->rows[i].சாவி_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, அளவு(Row),
                           ures_compareRows, pTempTable->சாவி_Chars,
                           பொய், pErrorCode);
            எனில்(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                திரும்பு;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*சாவிகள்*/
            எனில்(pKey16!=nullptr) {
                uint16_t *rKey16;

                எனில்(pKey16!=qKey16) {
                    rKey16=qKey16;
                } இல்லையெனில் {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                க்கு(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                எனில்(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } இல்லையெனில் {
                int32_t *rKey32;

                எனில்(pKey32!=qKey32) {
                    rKey32=qKey32;
                } இல்லையெனில் {
                    rKey32=pTempTable->resort;
                }
                க்கு(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                எனில்(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                எனில்(p!=q) {
                    r=q;
                } இல்லையெனில் {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                க்கு(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                எனில்(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        நிறுத்து;
    நிலை URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            க்கு(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                எனில்(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    திரும்பு;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        நிறுத்து;
    நிலை URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        நிறுத்து;
    இயல்பு:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        நிறுத்து;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(மாறிலி UDataSwapper *ds,
          மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
          UErrorCode *pErrorCode) {
    மாறிலி UDataInfo *pInfo;
    மாறிலி Resource *inBundle;
    Resource வேர்_Res;
    int32_t headerSize, maxTableLength;

    Row rows[அடுக்கு_ROW_CAPACITY];
    int32_t resort[அடுக்கு_ROW_CAPACITY];
    TempTable tempTable;

    மாறிலி int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, சாவிகள்_Bottom, சாவிகள்_உச்சி, resBottom, உச்சி;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    /* check data format and format version */
    pInfo=(மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData+4);
    எனில்(!(
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
        திரும்பு 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    எனில்(length<0) {
        bundleLength=-1;
    } இல்லையெனில் {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        எனில்(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }
    }

    inBundle=(மாறிலி Resource *)((மாறிலி எழுத்து *)inData+headerSize);
    வேர்_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(மாறிலி int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    எனில்(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }
    சாவிகள்_Bottom=1+indexLength;
    சாவிகள்_உச்சி=udata_readInt32(ds, inIndexes[URES_INDEX_சாவிகள்_உச்சி]);
    எனில்(indexLength>URES_INDEX_16_BIT_உச்சி) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_உச்சி]);
    } இல்லையெனில் {
        resBottom=சாவிகள்_உச்சி;
    }
    உச்சி=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_உச்சி]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    எனில்(0<=bundleLength && bundleLength<உச்சி) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         உச்சி, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }
    எனில்(சாவிகள்_உச்சி>(1+indexLength)) {
        tempTable.local_சாவி_Limit=சாவிகள்_உச்சி<<2;
    } இல்லையெனில் {
        tempTable.local_சாவி_Limit=0;
    }

    எனில்(length>=0) {
        Resource *outBundle=(Resource *)((எழுத்து *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t அடுக்கு_Res_Flags[அடுக்கு_ROW_CAPACITY];
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
        எனில்(resFlagsLength<=(int32_t)அளவு(அடுக்கு_Res_Flags)) {
            tempTable.resFlags=அடுக்கு_Res_Flags;
        } இல்லையெனில் {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            எனில்(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                திரும்பு 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        எனில்(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*உச்சி);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+சாவிகள்_Bottom, 4*(சாவிகள்_உச்சி-சாவிகள்_Bottom),
                                    outBundle+சாவிகள்_Bottom, pErrorCode);
        எனில்(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(சாவிகள்_உச்சி-சாவிகள்_Bottom));
            எனில்(tempTable.resFlags!=அடுக்கு_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            திரும்பு 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        எனில்(சாவிகள்_உச்சி<resBottom) {
            ds->swapArray16(ds, inBundle+சாவிகள்_உச்சி, (resBottom-சாவிகள்_உச்சி)*4, outBundle+சாவிகள்_உச்சி, pErrorCode);
            எனில்(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-சாவிகள்_உச்சி));
                எனில்(tempTable.resFlags!=அடுக்கு_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                திரும்பு 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.சாவி_Chars=(மாறிலி எழுத்து *)outBundle; /* sort by outCharset */
        எனில்(tempTable.majorFormatVersion>1 || maxTableLength<=அடுக்கு_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } இல்லையெனில் {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*அளவு(Row)+maxTableLength*4);
            எனில்(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                எனில்(tempTable.resFlags!=அடுக்கு_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                திரும்பு 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, வேர்_Res, nullptr, &tempTable, pErrorCode);
        எனில்(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             வேர்_Res);
        }

        எனில்(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        எனில்(tempTable.resFlags!=அடுக்கு_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, சாவிகள்_Bottom*4, outBundle, pErrorCode);
    }

    திரும்பு headerSize+4*உச்சி;
}

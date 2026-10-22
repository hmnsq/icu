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

#ضمن "unicode/utypes.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "resource.h"
#ضمن "uarrsort.h"
#ضمن "uassert.h"
#ضمن "ucol_swp.h"
#ضمن "udataswp.h"
#ضمن "uinvchar.h"
#ضمن "uresdata.h"
#ضمن "uresimp.h"
#ضمن "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, keyOffset) \
    ((keyOffset)<(pResData)->localKeyLimit ? \
        (ثابت حرف *)(pResData)->pRoot+(keyOffset) : \
        (pResData)->poolBundleKeys+(keyOffset)-(pResData)->localKeyLimit)

#define RES_GET_KEY32(pResData, keyOffset) \
    ((keyOffset)>=0 ? \
        (ثابت حرف *)(pResData)->pRoot+(keyOffset) : \
        (pResData)->poolBundleKeys+((keyOffset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
ساكن ثابت uint16_t gEmpty16=0;

ساكن ثابت بنية {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

ساكن ثابت بنية {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ساكن int32_t
_res_findTableItem(ثابت ResourceData *pResData, ثابت uint16_t *keyOffsets, int32_t length,
                   ثابت حرف *key, ثابت حرف **realKey) {
    ثابت حرف *tableKey;
    int32_t mid, start, limit;
    صحيح result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    طالما(start<limit) {
        mid = (start + limit) / 2;
        tableKey = RES_GET_KEY16(pResData, keyOffsets[mid]);
        𐤀𐤌 (pResData->useNativeStrcmp) {
            result = uprv_strcmp(key, tableKey);
        } وإلا {
            result = uprv_compareInvCharsAsAscii(key, tableKey);
        }
        𐤀𐤌 (result < 0) {
            limit = mid;
        } وإلا 𐤀𐤌 (result > 0) {
            start = mid + 1;
        } وإلا {
            /* We found it! */
            *realKey=tableKey;
            أرجع mid;
        }
    }
    أرجع URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

ساكن int32_t
_res_findTable32Item(ثابت ResourceData *pResData, ثابت int32_t *keyOffsets, int32_t length,
                     ثابت حرف *key, ثابت حرف **realKey) {
    ثابت حرف *tableKey;
    int32_t mid, start, limit;
    صحيح result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    طالما(start<limit) {
        mid = (start + limit) / 2;
        tableKey = RES_GET_KEY32(pResData, keyOffsets[mid]);
        𐤀𐤌 (pResData->useNativeStrcmp) {
            result = uprv_strcmp(key, tableKey);
        } وإلا {
            result = uprv_compareInvCharsAsAscii(key, tableKey);
        }
        𐤀𐤌 (result < 0) {
            limit = mid;
        } وإلا 𐤀𐤌 (result > 0) {
            start = mid + 1;
        } وإلا {
            /* We found it! */
            *realKey=tableKey;
            أرجع mid;
        }
    }
    أرجع URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ساكن UBool U_CALLCONV
isAcceptable(فراغ *context,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    أرجع
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

ساكن فراغ
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, ثابت فراغ *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType rootType;

    /* get the root resource */
    pResData->pRoot = static_cast<ثابت int32_t*>(inBytes);
    pResData->rootRes = static_cast<Resource>(*pResData->pRoot);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    𐤀𐤌(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        أرجع;
    }

    /* currently, we accept only resources that have a Table as their roots */
    rootType = static_cast<UResType>(RES_GET_TYPE(pResData->rootRes));
    𐤀𐤌(!URES_IS_TABLE(rootType)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        أرجع;
    }

    𐤀𐤌(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->localKeyLimit=0x10000;  /* greater than any 16-bit key string offset */
    } وإلا {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ثابت int32_t *indexes=pResData->pRoot+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        𐤀𐤌(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            أرجع;
        }
        𐤀𐤌( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_TOP]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            أرجع;
        }
        𐤀𐤌(indexes[URES_INDEX_KEYS_TOP]>(1+indexLength)) {
            pResData->localKeyLimit=indexes[URES_INDEX_KEYS_TOP]<<2;
        }
        𐤀𐤌(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        𐤀𐤌(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        𐤀𐤌((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            أرجع;
        }
        𐤀𐤌( indexLength>URES_INDEX_16BIT_TOP &&
            indexes[URES_INDEX_16BIT_TOP]>indexes[URES_INDEX_KEYS_TOP]
        ) {
            pResData->p16BitUnits = reinterpret_cast<ثابت uint16_t*>(pResData->pRoot + indexes[URES_INDEX_KEYS_TOP]);
        }
    }

    𐤀𐤌(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=true;
    }
}

U_CAPI فراغ U_EXPORT2
res_read(ResourceData *pResData,
         ثابت UDataInfo *pInfo, ثابت فراغ *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, حجم(ResourceData));
    𐤀𐤌(U_FAILURE(*errorCode)) {
        أرجع;
    }
    𐤀𐤌(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC فراغ
res_load(ResourceData *pResData,
         ثابت حرف *path, ثابت حرف *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, حجم(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    𐤀𐤌(U_FAILURE(*errorCode)) {
        أرجع;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC فراغ
res_unload(ResourceData *pResData) {
    𐤀𐤌(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

ساكن ثابت int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_BINARY,
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
    أرجع (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI ثابت char16_t * U_EXPORT2
res_getStringNoTrace(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    𐤀𐤌(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        𐤀𐤌((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(ثابت char16_t *)pResData->poolBundleStrings+offset;
        } وإلا {
            p=(ثابت char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        𐤀𐤌(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } وإلا 𐤀𐤌(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } وإلا 𐤀𐤌(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } وإلا {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } وإلا 𐤀𐤌(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        ثابت int32_t *p32= res==0 ? &gEmptyString.length : pResData->pRoot+res;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=nullptr;
        length=0;
    }
    𐤀𐤌(pLength) {
        *pLength=length;
    }
    أرجع p;
}

نطاق {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ثابت ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    𐤀𐤌 (offset == 0) {
        // empty string
    } وإلا 𐤀𐤌 (res == offset) {
        ثابت int32_t *p32=pResData->pRoot+res;
        int32_t length=*p32;
        ثابت char16_t* p = reinterpret_cast<ثابت char16_t*>(p32);
        أرجع length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } وإلا 𐤀𐤌 (RES_GET_TYPE(res) == URES_STRING_V2) {
        ثابت char16_t *p;
        𐤀𐤌 (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<ثابت char16_t*>(pResData->poolBundleStrings) + offset;
        } وإلا {
            p = reinterpret_cast<ثابت char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        𐤀𐤌 (first == 0x2205) {  // implicit length
            أرجع p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } وإلا 𐤀𐤌 (first == 0xdc03) {  // explicit length 3 (should not occur)
            أرجع p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } وإلا {
            // Assume that the string has not been stored with more length units than necessary.
            أرجع خطأ;
        }
    }
    أرجع خطأ;
}

int32_t getStringArray(ثابت ResourceData *pResData, ثابت icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    𐤀𐤌(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    int32_t length = array.getSize();
    𐤀𐤌(length == 0) {
        أرجع 0;
    }
    𐤀𐤌(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        أرجع length;
    }
    لكل(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        ثابت char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        𐤀𐤌(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            أرجع 0;
        }
        dest[i].setTo(true, s, sLength);
    }
    أرجع length;
}

}  // namespace

U_CAPI ثابت char16_t * U_EXPORT2
res_getAlias(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    𐤀𐤌(RES_GET_TYPE(res)==URES_ALIAS) {
        ثابت int32_t *p32= offset==0 ? &gEmptyString.length : pResData->pRoot+offset;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=nullptr;
        length=0;
    }
    𐤀𐤌(pLength) {
        *pLength=length;
    }
    أرجع p;
}

U_CAPI ثابت uint8_t * U_EXPORT2
res_getBinaryNoTrace(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    𐤀𐤌(RES_GET_TYPE(res)==URES_BINARY) {
        ثابت int32_t *p32= offset==0 ? (ثابت int32_t*)&gEmpty32 : pResData->pRoot+offset;
        length=*p32++;
        p=(ثابت uint8_t *)p32;
    } وإلا {
        p=nullptr;
        length=0;
    }
    𐤀𐤌(pLength) {
        *pLength=length;
    }
    أرجع p;
}


U_CAPI ثابت int32_t * U_EXPORT2
res_getIntVectorNoTrace(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    𐤀𐤌(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (ثابت int32_t *)&gEmpty32 : pResData->pRoot+offset;
        length=*p++;
    } وإلا {
        p=nullptr;
        length=0;
    }
    𐤀𐤌(pLength) {
        *pLength=length;
    }
    أرجع p;
}

U_CAPI int32_t U_EXPORT2
res_countArrayItems(ثابت ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    بدل(RES_GET_TYPE(res)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
    حالة URES_BINARY:
    حالة URES_ALIAS:
    حالة URES_INT:
    حالة URES_INT_VECTOR:
        أرجع 1;
    حالة URES_ARRAY:
    حالة URES_TABLE32:
        أرجع offset==0 ? 0 : *(pResData->pRoot+offset);
    حالة URES_TABLE:
        أرجع offset==0 ? 0 : *((ثابت uint16_t *)(pResData->pRoot+offset));
    حالة URES_ARRAY16:
    حالة URES_TABLE16:
        أرجع pResData->p16BitUnits[offset];
    افتراضي:
        أرجع 0;
    }
}

U_NAMESPACE_BEGIN

ResourceDataValue::~ResourceDataValue() {}

UResType ResourceDataValue::getType() ثابت {
    أرجع res_getPublicType(res);
}

ثابت char16_t *ResourceDataValue::getString(int32_t &length, UErrorCode &errorCode) ثابت {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    ثابت char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    𐤀𐤌(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ثابت char16_t *ResourceDataValue::getAliasString(int32_t &length, UErrorCode &errorCode) ثابت {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    ثابت char16_t *s = res_getAlias(&getData(), res, &length);
    𐤀𐤌(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

int32_t ResourceDataValue::getInt(UErrorCode &errorCode) ثابت {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    𐤀𐤌(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getInt(fTraceInfo, res);
}

uint32_t ResourceDataValue::getUInt(UErrorCode &errorCode) ثابت {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    𐤀𐤌(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getUInt(fTraceInfo, res);
}

ثابت int32_t *ResourceDataValue::getIntVector(int32_t &length, UErrorCode &errorCode) ثابت {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    ثابت int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    𐤀𐤌(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع iv;
}

ثابت uint8_t *ResourceDataValue::getBinary(int32_t &length, UErrorCode &errorCode) ثابت {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    ثابت uint8_t *b = res_getBinary(fTraceInfo, &getData(), res, &length);
    𐤀𐤌(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع b;
}

ResourceArray ResourceDataValue::getArray(UErrorCode &errorCode) ثابت {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع {};
    }
    ثابت uint16_t *items16 = nullptr;
    ثابت Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_ARRAY:
        𐤀𐤌 (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ثابت Resource*>(getData().pRoot) + offset;
            length = *items32++;
        }
        اكسر;
    حالة URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        اكسر;
    افتراضي:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable ResourceDataValue::getTable(UErrorCode &errorCode) ثابت {
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع {};
    }
    ثابت uint16_t *keys16 = nullptr;
    ثابت int32_t *keys32 = nullptr;
    ثابت uint16_t *items16 = nullptr;
    ثابت Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_TABLE:
        𐤀𐤌 (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ثابت uint16_t*>(getData().pRoot + offset);
            length = *keys16++;
            items32 = reinterpret_cast<ثابت Resource*>(keys16 + length + (~length & 1));
        }
        اكسر;
    حالة URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        اكسر;
    حالة URES_TABLE32:
        𐤀𐤌 (offset != 0) {  // empty if offset==0
            keys32 = getData().pRoot+offset;
            length = *keys32++;
            items32 = reinterpret_cast<ثابت Resource*>(keys32) + length;
        }
        اكسر;
    افتراضي:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool ResourceDataValue::isNoInheritanceMarker() ثابت {
    أرجع ::isNoInheritanceMarker(&getData(), res);
}

int32_t ResourceDataValue::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) ثابت {
    أرجع ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t ResourceDataValue::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) ثابت {
    𐤀𐤌(URES_IS_ARRAY(res)) {
        أرجع ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    𐤀𐤌(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    𐤀𐤌(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        أرجع 1;
    }
    int32_t sLength;
    ثابت char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    𐤀𐤌(s != nullptr) {
        dest[0].setTo(true, s, sLength);
        أرجع 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0;
}

UnicodeString ResourceDataValue::getStringOrFirstOfArray(UErrorCode &errorCode) ثابت {
    UnicodeString us;
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع us;
    }
    int32_t sLength;
    ثابت char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    𐤀𐤌(s != nullptr) {
        us.setTo(true, s, sLength);
        أرجع us;
    }
    ResourceArray array = getArray(errorCode);
    𐤀𐤌(U_FAILURE(errorCode)) {
        أرجع us;
    }
    𐤀𐤌(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        𐤀𐤌(s != nullptr) {
            us.setTo(true, s, sLength);
            أرجع us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    أرجع us;
}

U_NAMESPACE_END

ساكن Resource
makeResourceFrom16(ثابت ResourceData *pResData, int32_t res16) {
    𐤀𐤌(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } وإلا {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    أرجع URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_getTableItemByKey(ثابت ResourceData *pResData, Resource table,
                      int32_t *indexR, ثابت حرف **key) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    𐤀𐤌(key == nullptr || *key == nullptr) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(table)) {
    حالة URES_TABLE: {
        𐤀𐤌 (offset!=0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(pResData->pRoot+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *key, key);
            𐤀𐤌(idx>=0) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                أرجع p32[idx];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *key, key);
        𐤀𐤌(idx>=0) {
            أرجع makeResourceFrom16(pResData, p[length+idx]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        𐤀𐤌 (offset!=0) { /* empty if offset==0 */
            ثابت int32_t *p= pResData->pRoot+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *key, key);
            𐤀𐤌(idx>=0) {
                أرجع (Resource)p[length+idx];
            }
        }
        اكسر;
    }
    افتراضي:
        اكسر;
    }
    أرجع RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(ثابت ResourceData *pResData, Resource table,
                        int32_t indexR, ثابت حرف **key) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    𐤀𐤌 (indexR < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(table)) {
    حالة URES_TABLE: {
        𐤀𐤌 (offset != 0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(pResData->pRoot+offset);
            length=*p++;
            𐤀𐤌(indexR<length) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                𐤀𐤌(key!=nullptr) {
                    *key=RES_GET_KEY16(pResData, p[indexR]);
                }
                أرجع p32[indexR];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        𐤀𐤌(indexR<length) {
            𐤀𐤌(key!=nullptr) {
                *key=RES_GET_KEY16(pResData, p[indexR]);
            }
            أرجع makeResourceFrom16(pResData, p[length+indexR]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        𐤀𐤌 (offset != 0) { /* empty if offset==0 */
            ثابت int32_t *p= pResData->pRoot+offset;
            length=*p++;
            𐤀𐤌(indexR<length) {
                𐤀𐤌(key!=nullptr) {
                    *key=RES_GET_KEY32(pResData, p[indexR]);
                }
                أرجع (Resource)p[length+indexR];
            }
        }
        اكسر;
    }
    افتراضي:
        اكسر;
    }
    أرجع RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ثابت ResourceData *pResData, ثابت حرف *key) {
    ثابت حرف *realKey=key;
    int32_t idx;
    أرجع res_getTableItemByKey(pResData, pResData->rootRes, &idx, &realKey);
}


UBool icu::ResourceTable::getKeyAndValue(int32_t i,
                                         ثابت حرف *&key, icu::ResourceValue &value) ثابت {
    𐤀𐤌(0 <= i && i < length) {
        icu::ResourceDataValue &rdValue = static_cast<icu::ResourceDataValue &>(value);
        𐤀𐤌 (keys16 != nullptr) {
            key = RES_GET_KEY16(&rdValue.getData(), keys16[i]);
        } وإلا {
            key = RES_GET_KEY32(&rdValue.getData(), keys32[i]);
        }
        Resource res;
        𐤀𐤌 (items16 != nullptr) {
            res = makeResourceFrom16(&rdValue.getData(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rdValue.setResource(res, ResourceTracer(fTraceInfo, key));
        أرجع true;
    }
    أرجع خطأ;
}

UBool icu::ResourceTable::findValue(ثابت حرف *key, ResourceValue &value) ثابت {
    icu::ResourceDataValue &rdValue = static_cast<icu::ResourceDataValue &>(value);
    ثابت حرف *realKey = nullptr;
    int32_t i;
    𐤀𐤌 (keys16 != nullptr) {
        i = _res_findTableItem(&rdValue.getData(), keys16, length, key, &realKey);
    } وإلا {
        i = _res_findTable32Item(&rdValue.getData(), keys32, length, key, &realKey);
    }
    𐤀𐤌 (i >= 0) {
        Resource res;
        𐤀𐤌 (items16 != nullptr) {
            res = makeResourceFrom16(&rdValue.getData(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rdValue.setResource(res, ResourceTracer(fTraceInfo, key));
        أرجع true;
    }
    أرجع خطأ;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ثابت ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    𐤀𐤌 (indexR < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(array)) {
    حالة URES_ARRAY: {
        𐤀𐤌 (offset!=0) { /* empty if offset==0 */
            ثابت int32_t *p= pResData->pRoot+offset;
            𐤀𐤌(indexR<*p) {
                أرجع (Resource)p[1+indexR];
            }
        }
        اكسر;
    }
    حالة URES_ARRAY16: {
        ثابت uint16_t *p=pResData->p16BitUnits+offset;
        𐤀𐤌(indexR<*p) {
            أرجع makeResourceFrom16(pResData, p[1+indexR]);
        }
        اكسر;
    }
    افتراضي:
        اكسر;
    }
    أرجع RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(ثابت ResourceData *pResData, int32_t i) ثابت {
    𐤀𐤌 (items16 != nullptr) {
        أرجع makeResourceFrom16(pResData, items16[i]);
    } وإلا {
        أرجع items32[i];
    }
}

UBool icu::ResourceArray::getValue(int32_t i, icu::ResourceValue &value) ثابت {
    𐤀𐤌(0 <= i && i < length) {
        icu::ResourceDataValue &rdValue = static_cast<icu::ResourceDataValue &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rdValue.setResource(
            internalGetResource(&rdValue.getData(), i),
            ResourceTracer(fTraceInfo, i));
        أرجع true;
    }
    أرجع خطأ;
}

U_CFUNC Resource
res_findResource(ثابت ResourceData *pResData, Resource r, حرف** path, ثابت حرف** key) {
  حرف *pathP = *path, *nextSepP = *path;
  حرف *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  𐤀𐤌(!uprv_strlen(pathP)) {
      أرجع r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  𐤀𐤌(!URES_IS_CONTAINER(type)) {
      أرجع RES_BOGUS;
  }
  
  طالما(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    𐤀𐤌(nextSepP != nullptr) {
      𐤀𐤌(nextSepP == pathP) {
        // Empty key string.
        أرجع RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } وإلا {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    𐤀𐤌(URES_IS_TABLE(type)) {
      *key = pathP;
      t2 = res_getTableItemByKey(pResData, t1, &indexR, key);
    } وإلا 𐤀𐤌(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      𐤀𐤌(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } وإلا {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *key = nullptr;
    } وإلا { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  أرجع t1;
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
تعريفنوع بنية Row {
    int32_t keyIndex, sortIndex;
} Row;

ساكن int32_t U_CALLCONV
ures_compareRows(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *right) {
    ثابت حرف* keyChars = static_cast<ثابت حرف*>(context);
    أرجع static_cast<int32_t>(uprv_strcmp(keyChars + static_cast<ثابت Row*>(left)->keyIndex,
                                            keyChars + static_cast<ثابت Row*>(right)->keyIndex));
}

تعريفنوع بنية TempTable {
    ثابت حرف *keyChars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t localKeyLimit;
    uint8_t majorFormatVersion;
} TempTable;

تعداد {
    STACK_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
ساكن ثابت حرف *ثابت gUnknownKey="";

#𐤀𐤌 !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ساكن ثابت char16_t gCollationBinKey[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
ساكن فراغ
ures_swapResource(ثابت UDataSwapper *ds,
                  ثابت Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  ثابت حرف *key,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    ثابت Resource *p;
    Resource *q;
    int32_t offset, count;

    بدل(RES_GET_TYPE(res)) {
    حالة URES_TABLE16:
    حالة URES_STRING_V2:
    حالة URES_INT:
    حالة URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        أرجع;
    افتراضي:
        اكسر;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    𐤀𐤌(offset==0) {
        /* special offset indicating an empty item */
        أرجع;
    }
    𐤀𐤌 (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        أرجع;
    } وإلا {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    بدل(RES_GET_TYPE(res)) {
    حالة URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    حالة URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        اكسر;
    حالة URES_BINARY:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#𐤀𐤌 !UCONFIG_NO_COLLATION
        𐤀𐤌( key!=nullptr &&  /* the binary is in a table */
            (key!=gUnknownKey ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, key, -1,
                                       gCollationBinKey, UPRV_LENGTHOF(gCollationBinKey)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looksLikeCollationBinary(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#endif
        اكسر;
    حالة URES_TABLE:
    حالة URES_TABLE32:
        {
            ثابت uint16_t *pKey16;
            uint16_t *qKey16;

            ثابت int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            𐤀𐤌(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<ثابت uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } وإلا {
                /* get table item count */
                pKey32 = reinterpret_cast<ثابت int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            𐤀𐤌(count==0) {
                اكسر;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            لكل(i=0; i<count; ++i) {
                ثابت حرف *itemKey=gUnknownKey;
                𐤀𐤌(pKey16!=nullptr) {
                    int32_t keyOffset=ds->readUInt16(pKey16[i]);
                    𐤀𐤌(keyOffset<pTempTable->localKeyLimit) {
                        itemKey = reinterpret_cast<ثابت حرف*>(outBundle) + keyOffset;
                    }
                } وإلا {
                    int32_t keyOffset=udata_readInt32(ds, pKey32[i]);
                    𐤀𐤌(keyOffset>=0) {
                        itemKey = reinterpret_cast<ثابت حرف*>(outBundle) + keyOffset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, itemKey, pTempTable, pErrorCode);
                𐤀𐤌(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    أرجع;
                }
            }

            𐤀𐤌(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                𐤀𐤌(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } وإلا {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                اكسر;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            𐤀𐤌(pKey16!=nullptr) {
                لكل(i=0; i<count; ++i) {
                    pTempTable->rows[i].keyIndex=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } وإلا {
                لكل(i=0; i<count; ++i) {
                    pTempTable->rows[i].keyIndex=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, حجم(Row),
                           ures_compareRows, pTempTable->keyChars,
                           خطأ, pErrorCode);
            𐤀𐤌(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                أرجع;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /* keys */
            𐤀𐤌(pKey16!=nullptr) {
                uint16_t *rKey16;

                𐤀𐤌(pKey16!=qKey16) {
                    rKey16=qKey16;
                } وإلا {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                لكل(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                𐤀𐤌(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } وإلا {
                int32_t *rKey32;

                𐤀𐤌(pKey32!=qKey32) {
                    rKey32=qKey32;
                } وإلا {
                    rKey32=pTempTable->resort;
                }
                لكل(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                𐤀𐤌(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                𐤀𐤌(p!=q) {
                    r=q;
                } وإلا {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                لكل(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                𐤀𐤌(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        اكسر;
    حالة URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            لكل(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                𐤀𐤌(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    أرجع;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        اكسر;
    حالة URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        اكسر;
    افتراضي:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        اكسر;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(ثابت UDataSwapper *ds,
          ثابت فراغ *inData, int32_t length, فراغ *outData,
          UErrorCode *pErrorCode) {
    ثابت UDataInfo *pInfo;
    ثابت Resource *inBundle;
    Resource rootRes;
    int32_t headerSize, maxTableLength;

    Row rows[STACK_ROW_CAPACITY];
    int32_t resort[STACK_ROW_CAPACITY];
    TempTable tempTable;

    ثابت int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, keysBottom, keysTop, resBottom, top;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    𐤀𐤌(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)inData+4);
    𐤀𐤌(!(
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
        أرجع 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    𐤀𐤌(length<0) {
        bundleLength=-1;
    } وإلا {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        𐤀𐤌(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    inBundle=(ثابت Resource *)((ثابت حرف *)inData+headerSize);
    rootRes=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(ثابت int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    𐤀𐤌(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    keysBottom=1+indexLength;
    keysTop=udata_readInt32(ds, inIndexes[URES_INDEX_KEYS_TOP]);
    𐤀𐤌(indexLength>URES_INDEX_16BIT_TOP) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16BIT_TOP]);
    } وإلا {
        resBottom=keysTop;
    }
    top=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_TOP]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    𐤀𐤌(0<=bundleLength && bundleLength<top) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         top, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    𐤀𐤌(keysTop>(1+indexLength)) {
        tempTable.localKeyLimit=keysTop<<2;
    } وإلا {
        tempTable.localKeyLimit=0;
    }

    𐤀𐤌(length>=0) {
        Resource *outBundle=(Resource *)((حرف *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t stackResFlags[STACK_ROW_CAPACITY];
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
        𐤀𐤌(resFlagsLength<=(int32_t)حجم(stackResFlags)) {
            tempTable.resFlags=stackResFlags;
        } وإلا {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            𐤀𐤌(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                أرجع 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        𐤀𐤌(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*top);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+keysBottom, 4*(keysTop-keysBottom),
                                    outBundle+keysBottom, pErrorCode);
        𐤀𐤌(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(keysTop-keysBottom));
            𐤀𐤌(tempTable.resFlags!=stackResFlags) {
                uprv_free(tempTable.resFlags);
            }
            أرجع 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        𐤀𐤌(keysTop<resBottom) {
            ds->swapArray16(ds, inBundle+keysTop, (resBottom-keysTop)*4, outBundle+keysTop, pErrorCode);
            𐤀𐤌(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-keysTop));
                𐤀𐤌(tempTable.resFlags!=stackResFlags) {
                    uprv_free(tempTable.resFlags);
                }
                أرجع 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.keyChars=(ثابت حرف *)outBundle; /* sort by outCharset */
        𐤀𐤌(tempTable.majorFormatVersion>1 || maxTableLength<=STACK_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } وإلا {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*حجم(Row)+maxTableLength*4);
            𐤀𐤌(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                𐤀𐤌(tempTable.resFlags!=stackResFlags) {
                    uprv_free(tempTable.resFlags);
                }
                أرجع 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, rootRes, nullptr, &tempTable, pErrorCode);
        𐤀𐤌(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             rootRes);
        }

        𐤀𐤌(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        𐤀𐤌(tempTable.resFlags!=stackResFlags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, keysBottom*4, outBundle, pErrorCode);
    }

    أرجع headerSize+4*top;
}

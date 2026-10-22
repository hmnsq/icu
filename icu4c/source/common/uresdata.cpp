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

#include "unicode/utypes.h"
#include "unicode/udata.h"
#include "unicode/ustring.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "cstring.h"
#include "resource.h"
#include "uarrsort.h"
#include "uassert.h"
#include "ucol_swp.h"
#include "udataswp.h"
#include "uinvchar.h"
#include "uresdata.h"
#include "uresimp.h"
#include "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, keyOffset) \
    ((keyOffset)<(pResData)->localKeyLimit ? \
        (const var_na *)(pResData)->pRoot+(keyOffset) : \
        (pResData)->poolBundleKeys+(keyOffset)-(pResData)->localKeyLimit)

#define RES_GET_KEY32(pResData, keyOffset) \
    ((keyOffset)>=0 ? \
        (const var_na *)(pResData)->pRoot+(keyOffset) : \
        (pResData)->poolBundleKeys+((keyOffset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
static const uint16_t gEmpty16=0;

static const sa_nracanaa {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

static const sa_nracanaa {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

static int32_t
_res_findTableItem(const ResourceData *pResData, const uint16_t *keyOffsets, int32_t length,
                   const var_na *key, const var_na **realKey) {
    const var_na *tableKey;
    int32_t mid, start, limit;
    puur_naa_nka result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    ᤀᤠ᤺(start<limit) {
        mid = (start + limit) / 2;
        tableKey = RES_GET_KEY16(pResData, keyOffsets[mid]);
        ᤀᤡ᤺ (pResData->useNativeStrcmp) {
            result = uprv_strcmp(key, tableKey);
        } ᤀᤡ᤺ᤆᤡ {
            result = uprv_compareInvCharsAsAscii(key, tableKey);
        }
        ᤀᤡ᤺ (result < 0) {
            limit = mid;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (result > 0) {
            start = mid + 1;
        } ᤀᤡ᤺ᤆᤡ {
            /* We found it! */
            *realKey=tableKey;
            ᤀᤡᤴ mid;
        }
    }
    ᤀᤡᤴ URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

static int32_t
_res_findTable32Item(const ResourceData *pResData, const int32_t *keyOffsets, int32_t length,
                     const var_na *key, const var_na **realKey) {
    const var_na *tableKey;
    int32_t mid, start, limit;
    puur_naa_nka result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    ᤀᤠ᤺(start<limit) {
        mid = (start + limit) / 2;
        tableKey = RES_GET_KEY32(pResData, keyOffsets[mid]);
        ᤀᤡ᤺ (pResData->useNativeStrcmp) {
            result = uprv_strcmp(key, tableKey);
        } ᤀᤡ᤺ᤆᤡ {
            result = uprv_compareInvCharsAsAscii(key, tableKey);
        }
        ᤀᤡ᤺ (result < 0) {
            limit = mid;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (result > 0) {
            start = mid + 1;
        } ᤀᤡ᤺ᤆᤡ {
            /* We found it! */
            *realKey=tableKey;
            ᤀᤡᤴ mid;
        }
    }
    ᤀᤡᤴ URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

static UBool U_CALLCONV
isAcceptable(void *context,
             const var_na * /*type*/, const var_na * /*name*/,
             const UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    ᤀᤡᤴ
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

static void
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, const void *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType rootType;

    /* get the root resource */
    pResData->pRoot = static_cast<const int32_t*>(inBytes);
    pResData->rootRes = static_cast<Resource>(*pResData->pRoot);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    ᤀᤡ᤺(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        ᤀᤡᤴ;
    }

    /* currently, we accept only resources that have a Table as their roots */
    rootType = static_cast<UResType>(RES_GET_TYPE(pResData->rootRes));
    ᤀᤡ᤺(!URES_IS_TABLE(rootType)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        ᤀᤡᤴ;
    }

    ᤀᤡ᤺(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->localKeyLimit=0x10000;  /* greater than any 16-bit key string offset */
    } ᤀᤡ᤺ᤆᤡ {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        const int32_t *indexes=pResData->pRoot+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        ᤀᤡ᤺(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            ᤀᤡᤴ;
        }
        ᤀᤡ᤺( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_TOP]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            ᤀᤡᤴ;
        }
        ᤀᤡ᤺(indexes[URES_INDEX_KEYS_TOP]>(1+indexLength)) {
            pResData->localKeyLimit=indexes[URES_INDEX_KEYS_TOP]<<2;
        }
        ᤀᤡ᤺(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        ᤀᤡ᤺(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        ᤀᤡ᤺((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            ᤀᤡᤴ;
        }
        ᤀᤡ᤺( indexLength>URES_INDEX_16BIT_TOP &&
            indexes[URES_INDEX_16BIT_TOP]>indexes[URES_INDEX_KEYS_TOP]
        ) {
            pResData->p16BitUnits = reinterpret_cast<const uint16_t*>(pResData->pRoot + indexes[URES_INDEX_KEYS_TOP]);
        }
    }

    ᤀᤡ᤺(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=true;
    }
}

U_CAPI void U_EXPORT2
res_read(ResourceData *pResData,
         const UDataInfo *pInfo, const void *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, parimaa_na(ResourceData));
    ᤀᤡ᤺(U_FAILURE(*errorCode)) {
        ᤀᤡᤴ;
    }
    ᤀᤡ᤺(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        ᤀᤡᤴ;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC void
res_load(ResourceData *pResData,
         const var_na *path, const var_na *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, parimaa_na(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    ᤀᤡ᤺(U_FAILURE(*errorCode)) {
        ᤀᤡᤴ;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC void
res_unload(ResourceData *pResData) {
    ᤀᤡ᤺(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

static const int8_t gPublicTypes[URES_LIMIT] = {
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
    ᤀᤡᤴ (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI const char16_t * U_EXPORT2
res_getStringNoTrace(const ResourceData *pResData, Resource res, int32_t *pLength) {
    const char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ᤀᤡ᤺(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        ᤀᤡ᤺((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(const char16_t *)pResData->poolBundleStrings+offset;
        } ᤀᤡ᤺ᤆᤡ {
            p=(const char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        ᤀᤡ᤺(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } ᤀᤡ᤺ᤆᤡ {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        const int32_t *p32= res==0 ? &gEmptyString.length : pResData->pRoot+res;
        length=*p32++;
        p=(const char16_t *)p32;
    } ᤀᤡ᤺ᤆᤡ {
        p=nullptr;
        length=0;
    }
    ᤀᤡ᤺(pLength) {
        *pLength=length;
    }
    ᤀᤡᤴ p;
}

namespace {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(const ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    ᤀᤡ᤺ (offset == 0) {
        // empty string
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (res == offset) {
        const int32_t *p32=pResData->pRoot+res;
        int32_t length=*p32;
        const char16_t* p = reinterpret_cast<const char16_t*>(p32);
        ᤀᤡᤴ length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (RES_GET_TYPE(res) == URES_STRING_V2) {
        const char16_t *p;
        ᤀᤡ᤺ (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<const char16_t*>(pResData->poolBundleStrings) + offset;
        } ᤀᤡ᤺ᤆᤡ {
            p = reinterpret_cast<const char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        ᤀᤡ᤺ (first == 0x2205) {  // implicit length
            ᤀᤡᤴ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (first == 0xdc03) {  // explicit length 3 (should not occur)
            ᤀᤡᤴ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } ᤀᤡ᤺ᤆᤡ {
            // Assume that the string has not been stored with more length units than necessary.
            ᤀᤡᤴ false;
        }
    }
    ᤀᤡᤴ false;
}

int32_t getStringArray(const ResourceData *pResData, const icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ 0;
    }
    int32_t length = array.getSize();
    ᤀᤡ᤺(length == 0) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        ᤀᤡᤴ length;
    }
    for(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        const char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        ᤀᤡ᤺(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            ᤀᤡᤴ 0;
        }
        dest[i].setTo(true, s, sLength);
    }
    ᤀᤡᤴ length;
}

}  // namespace

U_CAPI const char16_t * U_EXPORT2
res_getAlias(const ResourceData *pResData, Resource res, int32_t *pLength) {
    const char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ᤀᤡ᤺(RES_GET_TYPE(res)==URES_ALIAS) {
        const int32_t *p32= offset==0 ? &gEmptyString.length : pResData->pRoot+offset;
        length=*p32++;
        p=(const char16_t *)p32;
    } ᤀᤡ᤺ᤆᤡ {
        p=nullptr;
        length=0;
    }
    ᤀᤡ᤺(pLength) {
        *pLength=length;
    }
    ᤀᤡᤴ p;
}

U_CAPI const uint8_t * U_EXPORT2
res_getBinaryNoTrace(const ResourceData *pResData, Resource res, int32_t *pLength) {
    const uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ᤀᤡ᤺(RES_GET_TYPE(res)==URES_BINARY) {
        const int32_t *p32= offset==0 ? (const int32_t*)&gEmpty32 : pResData->pRoot+offset;
        length=*p32++;
        p=(const uint8_t *)p32;
    } ᤀᤡ᤺ᤆᤡ {
        p=nullptr;
        length=0;
    }
    ᤀᤡ᤺(pLength) {
        *pLength=length;
    }
    ᤀᤡᤴ p;
}


U_CAPI const int32_t * U_EXPORT2
res_getIntVectorNoTrace(const ResourceData *pResData, Resource res, int32_t *pLength) {
    const int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ᤀᤡ᤺(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (const int32_t *)&gEmpty32 : pResData->pRoot+offset;
        length=*p++;
    } ᤀᤡ᤺ᤆᤡ {
        p=nullptr;
        length=0;
    }
    ᤀᤡ᤺(pLength) {
        *pLength=length;
    }
    ᤀᤡᤴ p;
}

U_CAPI int32_t U_EXPORT2
res_countArrayItems(const ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    switch(RES_GET_TYPE(res)) {
    pak_sa URES_STRING:
    pak_sa URES_STRING_V2:
    pak_sa URES_BINARY:
    pak_sa URES_ALIAS:
    pak_sa URES_INT:
    pak_sa URES_INT_VECTOR:
        ᤀᤡᤴ 1;
    pak_sa URES_ARRAY:
    pak_sa URES_TABLE32:
        ᤀᤡᤴ offset==0 ? 0 : *(pResData->pRoot+offset);
    pak_sa URES_TABLE:
        ᤀᤡᤴ offset==0 ? 0 : *((const uint16_t *)(pResData->pRoot+offset));
    pak_sa URES_ARRAY16:
    pak_sa URES_TABLE16:
        ᤀᤡᤴ pResData->p16BitUnits[offset];
    default:
        ᤀᤡᤴ 0;
    }
}

U_NAMESPACE_BEGIN

ResourceDataValue::~ResourceDataValue() {}

UResType ResourceDataValue::getType() const {
    ᤀᤡᤴ res_getPublicType(res);
}

const char16_t *ResourceDataValue::getString(int32_t &length, UErrorCode &errorCode) const {
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ nullptr;
    }
    const char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    ᤀᤡ᤺(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ᤀᤡᤴ s;
}

const char16_t *ResourceDataValue::getAliasString(int32_t &length, UErrorCode &errorCode) const {
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ nullptr;
    }
    const char16_t *s = res_getAlias(&getData(), res, &length);
    ᤀᤡ᤺(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ᤀᤡᤴ s;
}

int32_t ResourceDataValue::getInt(UErrorCode &errorCode) const {
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ᤀᤡᤴ res_getInt(fTraceInfo, res);
}

uint32_t ResourceDataValue::getUInt(UErrorCode &errorCode) const {
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ᤀᤡᤴ res_getUInt(fTraceInfo, res);
}

const int32_t *ResourceDataValue::getIntVector(int32_t &length, UErrorCode &errorCode) const {
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ nullptr;
    }
    const int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    ᤀᤡ᤺(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ᤀᤡᤴ iv;
}

const uint8_t *ResourceDataValue::getBinary(int32_t &length, UErrorCode &errorCode) const {
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ nullptr;
    }
    const uint8_t *b = res_getBinary(fTraceInfo, &getData(), res, &length);
    ᤀᤡ᤺(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ᤀᤡᤴ b;
}

ResourceArray ResourceDataValue::getArray(UErrorCode &errorCode) const {
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ {};
    }
    const uint16_t *items16 = nullptr;
    const Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    switch(RES_GET_TYPE(res)) {
    pak_sa URES_ARRAY:
        ᤀᤡ᤺ (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<const Resource*>(getData().pRoot) + offset;
            length = *items32++;
        }
        bha_nga;
    pak_sa URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        bha_nga;
    default:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        ᤀᤡᤴ {};
    }
    ᤀᤡᤴ ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable ResourceDataValue::getTable(UErrorCode &errorCode) const {
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ {};
    }
    const uint16_t *keys16 = nullptr;
    const int32_t *keys32 = nullptr;
    const uint16_t *items16 = nullptr;
    const Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    switch(RES_GET_TYPE(res)) {
    pak_sa URES_TABLE:
        ᤀᤡ᤺ (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<const uint16_t*>(getData().pRoot + offset);
            length = *keys16++;
            items32 = reinterpret_cast<const Resource*>(keys16 + length + (~length & 1));
        }
        bha_nga;
    pak_sa URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        bha_nga;
    pak_sa URES_TABLE32:
        ᤀᤡ᤺ (offset != 0) {  // empty if offset==0
            keys32 = getData().pRoot+offset;
            length = *keys32++;
            items32 = reinterpret_cast<const Resource*>(keys32) + length;
        }
        bha_nga;
    default:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        ᤀᤡᤴ {};
    }
    ᤀᤡᤴ ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool ResourceDataValue::isNoInheritanceMarker() const {
    ᤀᤡᤴ ::isNoInheritanceMarker(&getData(), res);
}

int32_t ResourceDataValue::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) const {
    ᤀᤡᤴ ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t ResourceDataValue::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) const {
    ᤀᤡ᤺(URES_IS_ARRAY(res)) {
        ᤀᤡᤴ ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        ᤀᤡᤴ 1;
    }
    int32_t sLength;
    const char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    ᤀᤡ᤺(s != nullptr) {
        dest[0].setTo(true, s, sLength);
        ᤀᤡᤴ 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    ᤀᤡᤴ 0;
}

UnicodeString ResourceDataValue::getStringOrFirstOfArray(UErrorCode &errorCode) const {
    UnicodeString us;
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ us;
    }
    int32_t sLength;
    const char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    ᤀᤡ᤺(s != nullptr) {
        us.setTo(true, s, sLength);
        ᤀᤡᤴ us;
    }
    ResourceArray array = getArray(errorCode);
    ᤀᤡ᤺(U_FAILURE(errorCode)) {
        ᤀᤡᤴ us;
    }
    ᤀᤡ᤺(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        ᤀᤡ᤺(s != nullptr) {
            us.setTo(true, s, sLength);
            ᤀᤡᤴ us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    ᤀᤡᤴ us;
}

U_NAMESPACE_END

static Resource
makeResourceFrom16(const ResourceData *pResData, int32_t res16) {
    ᤀᤡ᤺(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } ᤀᤡ᤺ᤆᤡ {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    ᤀᤡᤴ URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_getTableItemByKey(const ResourceData *pResData, Resource table,
                      int32_t *indexR, const var_na **key) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    ᤀᤡ᤺(key == nullptr || *key == nullptr) {
        ᤀᤡᤴ RES_BOGUS;
    }
    switch(RES_GET_TYPE(table)) {
    pak_sa URES_TABLE: {
        ᤀᤡ᤺ (offset!=0) { /* empty if offset==0 */
            const uint16_t *p= (const uint16_t *)(pResData->pRoot+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *key, key);
            ᤀᤡ᤺(idx>=0) {
                const Resource *p32=(const Resource *)(p+length+(~length&1));
                ᤀᤡᤴ p32[idx];
            }
        }
        bha_nga;
    }
    pak_sa URES_TABLE16: {
        const uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *key, key);
        ᤀᤡ᤺(idx>=0) {
            ᤀᤡᤴ makeResourceFrom16(pResData, p[length+idx]);
        }
        bha_nga;
    }
    pak_sa URES_TABLE32: {
        ᤀᤡ᤺ (offset!=0) { /* empty if offset==0 */
            const int32_t *p= pResData->pRoot+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *key, key);
            ᤀᤡ᤺(idx>=0) {
                ᤀᤡᤴ (Resource)p[length+idx];
            }
        }
        bha_nga;
    }
    default:
        bha_nga;
    }
    ᤀᤡᤴ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(const ResourceData *pResData, Resource table,
                        int32_t indexR, const var_na **key) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    ᤀᤡ᤺ (indexR < 0) {
        ᤀᤡᤴ RES_BOGUS;
    }
    switch(RES_GET_TYPE(table)) {
    pak_sa URES_TABLE: {
        ᤀᤡ᤺ (offset != 0) { /* empty if offset==0 */
            const uint16_t *p= (const uint16_t *)(pResData->pRoot+offset);
            length=*p++;
            ᤀᤡ᤺(indexR<length) {
                const Resource *p32=(const Resource *)(p+length+(~length&1));
                ᤀᤡ᤺(key!=nullptr) {
                    *key=RES_GET_KEY16(pResData, p[indexR]);
                }
                ᤀᤡᤴ p32[indexR];
            }
        }
        bha_nga;
    }
    pak_sa URES_TABLE16: {
        const uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        ᤀᤡ᤺(indexR<length) {
            ᤀᤡ᤺(key!=nullptr) {
                *key=RES_GET_KEY16(pResData, p[indexR]);
            }
            ᤀᤡᤴ makeResourceFrom16(pResData, p[length+indexR]);
        }
        bha_nga;
    }
    pak_sa URES_TABLE32: {
        ᤀᤡ᤺ (offset != 0) { /* empty if offset==0 */
            const int32_t *p= pResData->pRoot+offset;
            length=*p++;
            ᤀᤡ᤺(indexR<length) {
                ᤀᤡ᤺(key!=nullptr) {
                    *key=RES_GET_KEY32(pResData, p[indexR]);
                }
                ᤀᤡᤴ (Resource)p[length+indexR];
            }
        }
        bha_nga;
    }
    default:
        bha_nga;
    }
    ᤀᤡᤴ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(const ResourceData *pResData, const var_na *key) {
    const var_na *realKey=key;
    int32_t idx;
    ᤀᤡᤴ res_getTableItemByKey(pResData, pResData->rootRes, &idx, &realKey);
}


UBool icu::ResourceTable::getKeyAndValue(int32_t i,
                                         const var_na *&key, icu::ResourceValue &value) const {
    ᤀᤡ᤺(0 <= i && i < length) {
        icu::ResourceDataValue &rdValue = static_cast<icu::ResourceDataValue &>(value);
        ᤀᤡ᤺ (keys16 != nullptr) {
            key = RES_GET_KEY16(&rdValue.getData(), keys16[i]);
        } ᤀᤡ᤺ᤆᤡ {
            key = RES_GET_KEY32(&rdValue.getData(), keys32[i]);
        }
        Resource res;
        ᤀᤡ᤺ (items16 != nullptr) {
            res = makeResourceFrom16(&rdValue.getData(), items16[i]);
        } ᤀᤡ᤺ᤆᤡ {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rdValue.setResource(res, ResourceTracer(fTraceInfo, key));
        ᤀᤡᤴ true;
    }
    ᤀᤡᤴ false;
}

UBool icu::ResourceTable::findValue(const var_na *key, ResourceValue &value) const {
    icu::ResourceDataValue &rdValue = static_cast<icu::ResourceDataValue &>(value);
    const var_na *realKey = nullptr;
    int32_t i;
    ᤀᤡ᤺ (keys16 != nullptr) {
        i = _res_findTableItem(&rdValue.getData(), keys16, length, key, &realKey);
    } ᤀᤡ᤺ᤆᤡ {
        i = _res_findTable32Item(&rdValue.getData(), keys32, length, key, &realKey);
    }
    ᤀᤡ᤺ (i >= 0) {
        Resource res;
        ᤀᤡ᤺ (items16 != nullptr) {
            res = makeResourceFrom16(&rdValue.getData(), items16[i]);
        } ᤀᤡ᤺ᤆᤡ {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rdValue.setResource(res, ResourceTracer(fTraceInfo, key));
        ᤀᤡᤴ true;
    }
    ᤀᤡᤴ false;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(const ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    ᤀᤡ᤺ (indexR < 0) {
        ᤀᤡᤴ RES_BOGUS;
    }
    switch(RES_GET_TYPE(array)) {
    pak_sa URES_ARRAY: {
        ᤀᤡ᤺ (offset!=0) { /* empty if offset==0 */
            const int32_t *p= pResData->pRoot+offset;
            ᤀᤡ᤺(indexR<*p) {
                ᤀᤡᤴ (Resource)p[1+indexR];
            }
        }
        bha_nga;
    }
    pak_sa URES_ARRAY16: {
        const uint16_t *p=pResData->p16BitUnits+offset;
        ᤀᤡ᤺(indexR<*p) {
            ᤀᤡᤴ makeResourceFrom16(pResData, p[1+indexR]);
        }
        bha_nga;
    }
    default:
        bha_nga;
    }
    ᤀᤡᤴ RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(const ResourceData *pResData, int32_t i) const {
    ᤀᤡ᤺ (items16 != nullptr) {
        ᤀᤡᤴ makeResourceFrom16(pResData, items16[i]);
    } ᤀᤡ᤺ᤆᤡ {
        ᤀᤡᤴ items32[i];
    }
}

UBool icu::ResourceArray::getValue(int32_t i, icu::ResourceValue &value) const {
    ᤀᤡ᤺(0 <= i && i < length) {
        icu::ResourceDataValue &rdValue = static_cast<icu::ResourceDataValue &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rdValue.setResource(
            internalGetResource(&rdValue.getData(), i),
            ResourceTracer(fTraceInfo, i));
        ᤀᤡᤴ true;
    }
    ᤀᤡᤴ false;
}

U_CFUNC Resource
res_findResource(const ResourceData *pResData, Resource r, var_na** path, const var_na** key) {
  var_na *pathP = *path, *nextSepP = *path;
  var_na *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  ᤀᤡ᤺(!uprv_strlen(pathP)) {
      ᤀᤡᤴ r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  ᤀᤡ᤺(!URES_IS_CONTAINER(type)) {
      ᤀᤡᤴ RES_BOGUS;
  }
  
  ᤀᤠ᤺(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    ᤀᤡ᤺(nextSepP != nullptr) {
      ᤀᤡ᤺(nextSepP == pathP) {
        // Empty key string.
        ᤀᤡᤴ RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } ᤀᤡ᤺ᤆᤡ {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    ᤀᤡ᤺(URES_IS_TABLE(type)) {
      *key = pathP;
      t2 = res_getTableItemByKey(pResData, t1, &indexR, key);
    } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      ᤀᤡ᤺(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } ᤀᤡ᤺ᤆᤡ {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *key = nullptr;
    } ᤀᤡ᤺ᤆᤡ { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  ᤀᤡᤴ t1;
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
typedef sa_nracanaa Row {
    int32_t keyIndex, sortIndex;
} Row;

static int32_t U_CALLCONV
ures_compareRows(const void *context, const void *left, const void *right) {
    const var_na* keyChars = static_cast<const var_na*>(context);
    ᤀᤡᤴ static_cast<int32_t>(uprv_strcmp(keyChars + static_cast<const Row*>(left)->keyIndex,
                                            keyChars + static_cast<const Row*>(right)->keyIndex));
}

typedef sa_nracanaa TempTable {
    const var_na *keyChars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t localKeyLimit;
    uint8_t majorFormatVersion;
} TempTable;

ga_nanaa {
    STACK_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
static const var_na *const gUnknownKey="";

#ᤀᤡ᤺ !UCONFIG_NO_COLLATION
// resource table key for collation binaries
static const char16_t gCollationBinKey[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
static void
ures_swapResource(const UDataSwapper *ds,
                  const Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  const var_na *key,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    const Resource *p;
    Resource *q;
    int32_t offset, count;

    switch(RES_GET_TYPE(res)) {
    pak_sa URES_TABLE16:
    pak_sa URES_STRING_V2:
    pak_sa URES_INT:
    pak_sa URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        ᤀᤡᤴ;
    default:
        bha_nga;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    ᤀᤡ᤺(offset==0) {
        /* special offset indicating an empty item */
        ᤀᤡᤴ;
    }
    ᤀᤡ᤺ (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        ᤀᤡᤴ;
    } ᤀᤡ᤺ᤆᤡ {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    switch(RES_GET_TYPE(res)) {
    pak_sa URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    pak_sa URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        bha_nga;
    pak_sa URES_BINARY:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#ᤀᤡ᤺ !UCONFIG_NO_COLLATION
        ᤀᤡ᤺( key!=nullptr &&  /* the binary is in a table */
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
        bha_nga;
    pak_sa URES_TABLE:
    pak_sa URES_TABLE32:
        {
            const uint16_t *pKey16;
            uint16_t *qKey16;

            const int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            ᤀᤡ᤺(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<const uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } ᤀᤡ᤺ᤆᤡ {
                /* get table item count */
                pKey32 = reinterpret_cast<const int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            ᤀᤡ᤺(count==0) {
                bha_nga;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            for(i=0; i<count; ++i) {
                const var_na *itemKey=gUnknownKey;
                ᤀᤡ᤺(pKey16!=nullptr) {
                    int32_t keyOffset=ds->readUInt16(pKey16[i]);
                    ᤀᤡ᤺(keyOffset<pTempTable->localKeyLimit) {
                        itemKey = reinterpret_cast<const var_na*>(outBundle) + keyOffset;
                    }
                } ᤀᤡ᤺ᤆᤡ {
                    int32_t keyOffset=udata_readInt32(ds, pKey32[i]);
                    ᤀᤡ᤺(keyOffset>=0) {
                        itemKey = reinterpret_cast<const var_na*>(outBundle) + keyOffset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, itemKey, pTempTable, pErrorCode);
                ᤀᤡ᤺(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    ᤀᤡᤴ;
                }
            }

            ᤀᤡ᤺(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                ᤀᤡ᤺(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } ᤀᤡ᤺ᤆᤡ {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                bha_nga;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            ᤀᤡ᤺(pKey16!=nullptr) {
                for(i=0; i<count; ++i) {
                    pTempTable->rows[i].keyIndex=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } ᤀᤡ᤺ᤆᤡ {
                for(i=0; i<count; ++i) {
                    pTempTable->rows[i].keyIndex=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, parimaa_na(Row),
                           ures_compareRows, pTempTable->keyChars,
                           false, pErrorCode);
            ᤀᤡ᤺(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                ᤀᤡᤴ;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /* keys */
            ᤀᤡ᤺(pKey16!=nullptr) {
                uint16_t *rKey16;

                ᤀᤡ᤺(pKey16!=qKey16) {
                    rKey16=qKey16;
                } ᤀᤡ᤺ᤆᤡ {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                for(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                ᤀᤡ᤺(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } ᤀᤡ᤺ᤆᤡ {
                int32_t *rKey32;

                ᤀᤡ᤺(pKey32!=qKey32) {
                    rKey32=qKey32;
                } ᤀᤡ᤺ᤆᤡ {
                    rKey32=pTempTable->resort;
                }
                for(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                ᤀᤡ᤺(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                ᤀᤡ᤺(p!=q) {
                    r=q;
                } ᤀᤡ᤺ᤆᤡ {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                for(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                ᤀᤡ᤺(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        bha_nga;
    pak_sa URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            for(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                ᤀᤡ᤺(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    ᤀᤡᤴ;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        bha_nga;
    pak_sa URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        bha_nga;
    default:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        bha_nga;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(const UDataSwapper *ds,
          const void *inData, int32_t length, void *outData,
          UErrorCode *pErrorCode) {
    const UDataInfo *pInfo;
    const Resource *inBundle;
    Resource rootRes;
    int32_t headerSize, maxTableLength;

    Row rows[STACK_ROW_CAPACITY];
    int32_t resort[STACK_ROW_CAPACITY];
    TempTable tempTable;

    const int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, keysBottom, keysTop, resBottom, top;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ᤀᤡ᤺(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ᤀᤡᤴ 0;
    }

    /* check data format and format version */
    pInfo=(const UDataInfo *)((const var_na *)inData+4);
    ᤀᤡ᤺(!(
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
        ᤀᤡᤴ 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    ᤀᤡ᤺(length<0) {
        bundleLength=-1;
    } ᤀᤡ᤺ᤆᤡ {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        ᤀᤡ᤺(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ᤀᤡᤴ 0;
        }
    }

    inBundle=(const Resource *)((const var_na *)inData+headerSize);
    rootRes=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(const int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    ᤀᤡ᤺(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ᤀᤡᤴ 0;
    }
    keysBottom=1+indexLength;
    keysTop=udata_readInt32(ds, inIndexes[URES_INDEX_KEYS_TOP]);
    ᤀᤡ᤺(indexLength>URES_INDEX_16BIT_TOP) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16BIT_TOP]);
    } ᤀᤡ᤺ᤆᤡ {
        resBottom=keysTop;
    }
    top=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_TOP]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    ᤀᤡ᤺(0<=bundleLength && bundleLength<top) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         top, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(keysTop>(1+indexLength)) {
        tempTable.localKeyLimit=keysTop<<2;
    } ᤀᤡ᤺ᤆᤡ {
        tempTable.localKeyLimit=0;
    }

    ᤀᤡ᤺(length>=0) {
        Resource *outBundle=(Resource *)((var_na *)outData+headerSize);

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
        ᤀᤡ᤺(resFlagsLength<=(int32_t)parimaa_na(stackResFlags)) {
            tempTable.resFlags=stackResFlags;
        } ᤀᤡ᤺ᤆᤡ {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            ᤀᤡ᤺(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ᤀᤡᤴ 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        ᤀᤡ᤺(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*top);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+keysBottom, 4*(keysTop-keysBottom),
                                    outBundle+keysBottom, pErrorCode);
        ᤀᤡ᤺(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(keysTop-keysBottom));
            ᤀᤡ᤺(tempTable.resFlags!=stackResFlags) {
                uprv_free(tempTable.resFlags);
            }
            ᤀᤡᤴ 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        ᤀᤡ᤺(keysTop<resBottom) {
            ds->swapArray16(ds, inBundle+keysTop, (resBottom-keysTop)*4, outBundle+keysTop, pErrorCode);
            ᤀᤡ᤺(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-keysTop));
                ᤀᤡ᤺(tempTable.resFlags!=stackResFlags) {
                    uprv_free(tempTable.resFlags);
                }
                ᤀᤡᤴ 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.keyChars=(const var_na *)outBundle; /* sort by outCharset */
        ᤀᤡ᤺(tempTable.majorFormatVersion>1 || maxTableLength<=STACK_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } ᤀᤡ᤺ᤆᤡ {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*parimaa_na(Row)+maxTableLength*4);
            ᤀᤡ᤺(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ᤀᤡ᤺(tempTable.resFlags!=stackResFlags) {
                    uprv_free(tempTable.resFlags);
                }
                ᤀᤡᤴ 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, rootRes, nullptr, &tempTable, pErrorCode);
        ᤀᤡ᤺(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             rootRes);
        }

        ᤀᤡ᤺(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        ᤀᤡ᤺(tempTable.resFlags!=stackResFlags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, keysBottom*4, outBundle, pErrorCode);
    }

    ᤀᤡᤴ headerSize+4*top;
}

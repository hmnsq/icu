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
#define RES_GET_KEY16(pResData, کلیل_Offset) \
    ((کلیل_Offset)<(pResData)->local_کلیل_Limit ? \
        (ثابت حرف *)(pResData)->p_ڕەگ+(کلیل_Offset) : \
        (pResData)->pool_Bundle_کلیلەکان+(کلیل_Offset)-(pResData)->local_کلیل_Limit)

#define RES_GET_KEY32(pResData, کلیل_Offset) \
    ((کلیل_Offset)>=0 ? \
        (ثابت حرف *)(pResData)->p_ڕەگ+(کلیل_Offset) : \
        (pResData)->pool_Bundle_کلیلەکان+((کلیل_Offset)&0x7fffffff))

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
_res_findTableItem(ثابت ResourceData *pResData, ثابت uint16_t *کلیل_Offsets, int32_t length,
                   ثابت حرف *کلیل, ثابت حرف **real_کلیل) {
    ثابت حرف *table_کلیل;
    int32_t mid, start, limit;
    صحيح result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    طالما(start<limit) {
        mid = (start + limit) / 2;
        table_کلیل = RES_GET_KEY16(pResData, کلیل_Offsets[mid]);
        إذا (pResData->useNativeStrcmp) {
            result = uprv_strcmp(کلیل, table_کلیل);
        } وإلا {
            result = uprv_compareInvCharsAsAscii(کلیل, table_کلیل);
        }
        إذا (result < 0) {
            limit = mid;
        } وإلا إذا (result > 0) {
            start = mid + 1;
        } وإلا {
            /* We found it! */
            *real_کلیل=table_کلیل;
            أرجع mid;
        }
    }
    أرجع URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

ساكن int32_t
_res_findTable32Item(ثابت ResourceData *pResData, ثابت int32_t *کلیل_Offsets, int32_t length,
                     ثابت حرف *کلیل, ثابت حرف **real_کلیل) {
    ثابت حرف *table_کلیل;
    int32_t mid, start, limit;
    صحيح result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    طالما(start<limit) {
        mid = (start + limit) / 2;
        table_کلیل = RES_GET_KEY32(pResData, کلیل_Offsets[mid]);
        إذا (pResData->useNativeStrcmp) {
            result = uprv_strcmp(کلیل, table_کلیل);
        } وإلا {
            result = uprv_compareInvCharsAsAscii(کلیل, table_کلیل);
        }
        إذا (result < 0) {
            limit = mid;
        } وإلا إذا (result > 0) {
            start = mid + 1;
        } وإلا {
            /* We found it! */
            *real_کلیل=table_کلیل;
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
    UResType ڕەگ_Type;

    /* get the root resource */
    pResData->p_ڕەگ = static_cast<ثابت int32_t*>(inBytes);
    pResData->ڕەگ_Res = static_cast<Resource>(*pResData->p_ڕەگ);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    إذا(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        أرجع;
    }

    /* currently, we accept only resources that have a Table as their roots */
    ڕەگ_Type = static_cast<UResType>(RES_GET_TYPE(pResData->ڕەگ_Res));
    إذا(!URES_IS_TABLE(ڕەگ_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        أرجع;
    }

    إذا(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_کلیل_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } وإلا {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ثابت int32_t *indexes=pResData->p_ڕەگ+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        إذا(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            أرجع;
        }
        إذا( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_لووتکە]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            أرجع;
        }
        إذا(indexes[URES_INDEX_کلیلەکان_لووتکە]>(1+indexLength)) {
            pResData->local_کلیل_Limit=indexes[URES_INDEX_کلیلەکان_لووتکە]<<2;
        }
        إذا(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        إذا(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        إذا((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            أرجع;
        }
        إذا( indexLength>URES_INDEX_16_BIT_لووتکە &&
            indexes[URES_INDEX_16_BIT_لووتکە]>indexes[URES_INDEX_کلیلەکان_لووتکە]
        ) {
            pResData->p16BitUnits = reinterpret_cast<ثابت uint16_t*>(pResData->p_ڕەگ + indexes[URES_INDEX_کلیلەکان_لووتکە]);
        }
    }

    إذا(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
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
    إذا(U_FAILURE(*errorCode)) {
        أرجع;
    }
    إذا(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
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
    إذا(U_FAILURE(*errorCode)) {
        أرجع;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC فراغ
res_unload(ResourceData *pResData) {
    إذا(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

ساكن ثابت int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_دووانەیی,
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
    إذا(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        إذا((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(ثابت char16_t *)pResData->poolBundleStrings+offset;
        } وإلا {
            p=(ثابت char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        إذا(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } وإلا إذا(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } وإلا إذا(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } وإلا {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } وإلا إذا(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        ثابت int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_ڕەگ+res;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=nullptr;
        length=0;
    }
    إذا(pLength) {
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
    إذا (offset == 0) {
        // empty string
    } وإلا إذا (res == offset) {
        ثابت int32_t *p32=pResData->p_ڕەگ+res;
        int32_t length=*p32;
        ثابت char16_t* p = reinterpret_cast<ثابت char16_t*>(p32);
        أرجع length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } وإلا إذا (RES_GET_TYPE(res) == URES_STRING_V2) {
        ثابت char16_t *p;
        إذا (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<ثابت char16_t*>(pResData->poolBundleStrings) + offset;
        } وإلا {
            p = reinterpret_cast<ثابت char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        إذا (first == 0x2205) {  // implicit length
            أرجع p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } وإلا إذا (first == 0xdc03) {  // explicit length 3 (should not occur)
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
    إذا(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    إذا(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    int32_t length = array.getSize();
    إذا(length == 0) {
        أرجع 0;
    }
    إذا(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        أرجع length;
    }
    لكل(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        ثابت char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        إذا(s == nullptr) {
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
    إذا(RES_GET_TYPE(res)==URES_ALIAS) {
        ثابت int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_ڕەگ+offset;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=nullptr;
        length=0;
    }
    إذا(pLength) {
        *pLength=length;
    }
    أرجع p;
}

U_CAPI ثابت uint8_t * U_EXPORT2
res_get_دووانەیی_No_Trace(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_دووانەیی) {
        ثابت int32_t *p32= offset==0 ? (ثابت int32_t*)&gEmpty32 : pResData->p_ڕەگ+offset;
        length=*p32++;
        p=(ثابت uint8_t *)p32;
    } وإلا {
        p=nullptr;
        length=0;
    }
    إذا(pLength) {
        *pLength=length;
    }
    أرجع p;
}


U_CAPI ثابت int32_t * U_EXPORT2
res_getIntVectorNoTrace(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (ثابت int32_t *)&gEmpty32 : pResData->p_ڕەگ+offset;
        length=*p++;
    } وإلا {
        p=nullptr;
        length=0;
    }
    إذا(pLength) {
        *pLength=length;
    }
    أرجع p;
}

U_CAPI int32_t U_EXPORT2
res_ژماردن_Array_کاڵاکان(ثابت ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    بدل(RES_GET_TYPE(res)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
    حالة URES_دووانەیی:
    حالة URES_ALIAS:
    حالة URES_INT:
    حالة URES_INT_VECTOR:
        أرجع 1;
    حالة URES_ARRAY:
    حالة URES_TABLE32:
        أرجع offset==0 ? 0 : *(pResData->p_ڕەگ+offset);
    حالة URES_TABLE:
        أرجع offset==0 ? 0 : *((ثابت uint16_t *)(pResData->p_ڕەگ+offset));
    حالة URES_ARRAY16:
    حالة URES_TABLE16:
        أرجع pResData->p16BitUnits[offset];
    افتراضي:
        أرجع 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_بەها::~Resource_Data_بەها() {}

UResType Resource_Data_بەها::getType() ثابت {
    أرجع res_getPublicType(res);
}

ثابت char16_t *Resource_Data_بەها::getString(int32_t &length, UErrorCode &errorCode) ثابت {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    ثابت char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    إذا(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ثابت char16_t *Resource_Data_بەها::getAliasString(int32_t &length, UErrorCode &errorCode) ثابت {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    ثابت char16_t *s = res_getAlias(&getData(), res, &length);
    إذا(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

int32_t Resource_Data_بەها::getInt(UErrorCode &errorCode) ثابت {
    إذا(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_بەها::getUInt(UErrorCode &errorCode) ثابت {
    إذا(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getUInt(fTraceInfo, res);
}

ثابت int32_t *Resource_Data_بەها::getIntVector(int32_t &length, UErrorCode &errorCode) ثابت {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    ثابت int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    إذا(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع iv;
}

ثابت uint8_t *Resource_Data_بەها::get_دووانەیی(int32_t &length, UErrorCode &errorCode) ثابت {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    ثابت uint8_t *b = res_get_دووانەیی(fTraceInfo, &getData(), res, &length);
    إذا(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع b;
}

ResourceArray Resource_Data_بەها::getArray(UErrorCode &errorCode) ثابت {
    إذا(U_FAILURE(errorCode)) {
        أرجع {};
    }
    ثابت uint16_t *items16 = nullptr;
    ثابت Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_ARRAY:
        إذا (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ثابت Resource*>(getData().p_ڕەگ) + offset;
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

ResourceTable Resource_Data_بەها::getTable(UErrorCode &errorCode) ثابت {
    إذا(U_FAILURE(errorCode)) {
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
        إذا (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ثابت uint16_t*>(getData().p_ڕەگ + offset);
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
        إذا (offset != 0) {  // empty if offset==0
            keys32 = getData().p_ڕەگ+offset;
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

UBool Resource_Data_بەها::isNoInheritanceMarker() ثابت {
    أرجع ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_بەها::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) ثابت {
    أرجع ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_بەها::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) ثابت {
    إذا(URES_IS_ARRAY(res)) {
        أرجع ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    إذا(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    إذا(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    إذا(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        أرجع 1;
    }
    int32_t sLength;
    ثابت char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    إذا(s != nullptr) {
        dest[0].setTo(true, s, sLength);
        أرجع 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0;
}

UnicodeString Resource_Data_بەها::getStringOrFirstOfArray(UErrorCode &errorCode) ثابت {
    UnicodeString us;
    إذا(U_FAILURE(errorCode)) {
        أرجع us;
    }
    int32_t sLength;
    ثابت char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    إذا(s != nullptr) {
        us.setTo(true, s, sLength);
        أرجع us;
    }
    ResourceArray array = getArray(errorCode);
    إذا(U_FAILURE(errorCode)) {
        أرجع us;
    }
    إذا(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        إذا(s != nullptr) {
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
    إذا(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } وإلا {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    أرجع URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_کلیل(ثابت ResourceData *pResData, Resource table,
                      int32_t *indexR, ثابت حرف **کلیل) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    إذا(کلیل == nullptr || *کلیل == nullptr) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(table)) {
    حالة URES_TABLE: {
        إذا (offset!=0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(pResData->p_ڕەگ+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *کلیل, کلیل);
            إذا(idx>=0) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                أرجع p32[idx];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *کلیل, کلیل);
        إذا(idx>=0) {
            أرجع makeResourceFrom16(pResData, p[length+idx]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (offset!=0) { /* empty if offset==0 */
            ثابت int32_t *p= pResData->p_ڕەگ+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *کلیل, کلیل);
            إذا(idx>=0) {
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
                        int32_t indexR, ثابت حرف **کلیل) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    إذا (indexR < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(table)) {
    حالة URES_TABLE: {
        إذا (offset != 0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(pResData->p_ڕەگ+offset);
            length=*p++;
            إذا(indexR<length) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                إذا(کلیل!=nullptr) {
                    *کلیل=RES_GET_KEY16(pResData, p[indexR]);
                }
                أرجع p32[indexR];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        إذا(indexR<length) {
            إذا(کلیل!=nullptr) {
                *کلیل=RES_GET_KEY16(pResData, p[indexR]);
            }
            أرجع makeResourceFrom16(pResData, p[length+indexR]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (offset != 0) { /* empty if offset==0 */
            ثابت int32_t *p= pResData->p_ڕەگ+offset;
            length=*p++;
            إذا(indexR<length) {
                إذا(کلیل!=nullptr) {
                    *کلیل=RES_GET_KEY32(pResData, p[indexR]);
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
res_getResource(ثابت ResourceData *pResData, ثابت حرف *کلیل) {
    ثابت حرف *real_کلیل=کلیل;
    int32_t idx;
    أرجع res_get_Table_Item_By_کلیل(pResData, pResData->ڕەگ_Res, &idx, &real_کلیل);
}


UBool icu::ResourceTable::get_کلیل_And_بەها(int32_t i,
                                         ثابت حرف *&کلیل, icu::Resource_بەها &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_Data_بەها &rd_بەها = static_cast<icu::Resource_Data_بەها &>(value);
        إذا (keys16 != nullptr) {
            کلیل = RES_GET_KEY16(&rd_بەها.getData(), keys16[i]);
        } وإلا {
            کلیل = RES_GET_KEY32(&rd_بەها.getData(), keys32[i]);
        }
        Resource res;
        إذا (items16 != nullptr) {
            res = makeResourceFrom16(&rd_بەها.getData(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_بەها.setResource(res, ResourceTracer(fTraceInfo, کلیل));
        أرجع true;
    }
    أرجع خطأ;
}

UBool icu::ResourceTable::find_بەها(ثابت حرف *کلیل, Resource_بەها &value) ثابت {
    icu::Resource_Data_بەها &rd_بەها = static_cast<icu::Resource_Data_بەها &>(value);
    ثابت حرف *real_کلیل = nullptr;
    int32_t i;
    إذا (keys16 != nullptr) {
        i = _res_findTableItem(&rd_بەها.getData(), keys16, length, کلیل, &real_کلیل);
    } وإلا {
        i = _res_findTable32Item(&rd_بەها.getData(), keys32, length, کلیل, &real_کلیل);
    }
    إذا (i >= 0) {
        Resource res;
        إذا (items16 != nullptr) {
            res = makeResourceFrom16(&rd_بەها.getData(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_بەها.setResource(res, ResourceTracer(fTraceInfo, کلیل));
        أرجع true;
    }
    أرجع خطأ;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ثابت ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    إذا (indexR < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(array)) {
    حالة URES_ARRAY: {
        إذا (offset!=0) { /* empty if offset==0 */
            ثابت int32_t *p= pResData->p_ڕەگ+offset;
            إذا(indexR<*p) {
                أرجع (Resource)p[1+indexR];
            }
        }
        اكسر;
    }
    حالة URES_ARRAY16: {
        ثابت uint16_t *p=pResData->p16BitUnits+offset;
        إذا(indexR<*p) {
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
    إذا (items16 != nullptr) {
        أرجع makeResourceFrom16(pResData, items16[i]);
    } وإلا {
        أرجع items32[i];
    }
}

UBool icu::ResourceArray::get_بەها(int32_t i, icu::Resource_بەها &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_Data_بەها &rd_بەها = static_cast<icu::Resource_Data_بەها &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_بەها.setResource(
            internalGetResource(&rd_بەها.getData(), i),
            ResourceTracer(fTraceInfo, i));
        أرجع true;
    }
    أرجع خطأ;
}

U_CFUNC Resource
res_findResource(ثابت ResourceData *pResData, Resource r, حرف** path, ثابت حرف** کلیل) {
  حرف *pathP = *path, *nextSepP = *path;
  حرف *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  إذا(!uprv_strlen(pathP)) {
      أرجع r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  إذا(!URES_IS_CONTAINER(type)) {
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
    إذا(nextSepP != nullptr) {
      إذا(nextSepP == pathP) {
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
    إذا(URES_IS_TABLE(type)) {
      *کلیل = pathP;
      t2 = res_get_Table_Item_By_کلیل(pResData, t1, &indexR, کلیل);
    } وإلا إذا(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      إذا(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } وإلا {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *کلیل = nullptr;
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
    int32_t کلیل_Index, sortIndex;
} Row;

ساكن int32_t U_CALLCONV
ures_compareRows(ثابت فراغ *context, ثابت فراغ *left, ثابت فراغ *ڕاست) {
    ثابت حرف* کلیل_Chars = static_cast<ثابت حرف*>(context);
    أرجع static_cast<int32_t>(uprv_strcmp(کلیل_Chars + static_cast<ثابت Row*>(left)->کلیل_Index,
                                            کلیل_Chars + static_cast<ثابت Row*>(ڕاست)->کلیل_Index));
}

تعريفنوع بنية TempTable {
    ثابت حرف *کلیل_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_کلیل_Limit;
    uint8_t majorFormatVersion;
} TempTable;

تعداد {
    کەڵەکە_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
ساكن ثابت حرف *ثابت g_Unknown_کلیل="";

#إذا !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ساكن ثابت char16_t g_Collation_Bin_کلیل[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
ساكن فراغ
ures_swapResource(ثابت UDataSwapper *ds,
                  ثابت Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  ثابت حرف *کلیل,
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
    إذا(offset==0) {
        /* special offset indicating an empty item */
        أرجع;
    }
    إذا (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
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
    حالة URES_دووانەیی:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#إذا !UCONFIG_NO_COLLATION
        إذا( کلیل!=nullptr &&  /* the binary is in a table */
            (کلیل!=g_Unknown_کلیل ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, کلیل, -1,
                                       g_Collation_Bin_کلیل, UPRV_LENGTHOF(g_Collation_Bin_کلیل)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_دووانەیی(ds, p+1, count))
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

            إذا(RES_GET_TYPE(res)==URES_TABLE) {
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

            إذا(count==0) {
                اكسر;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            لكل(i=0; i<count; ++i) {
                ثابت حرف *item_کلیل=g_Unknown_کلیل;
                إذا(pKey16!=nullptr) {
                    int32_t کلیل_Offset=ds->readUInt16(pKey16[i]);
                    إذا(کلیل_Offset<pTempTable->local_کلیل_Limit) {
                        item_کلیل = reinterpret_cast<ثابت حرف*>(outBundle) + کلیل_Offset;
                    }
                } وإلا {
                    int32_t کلیل_Offset=udata_readInt32(ds, pKey32[i]);
                    إذا(کلیل_Offset>=0) {
                        item_کلیل = reinterpret_cast<ثابت حرف*>(outBundle) + کلیل_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_کلیل, pTempTable, pErrorCode);
                إذا(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    أرجع;
                }
            }

            إذا(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                إذا(pKey16!=nullptr) {
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
            إذا(pKey16!=nullptr) {
                لكل(i=0; i<count; ++i) {
                    pTempTable->rows[i].کلیل_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } وإلا {
                لكل(i=0; i<count; ++i) {
                    pTempTable->rows[i].کلیل_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, حجم(Row),
                           ures_compareRows, pTempTable->کلیل_Chars,
                           خطأ, pErrorCode);
            إذا(U_FAILURE(*pErrorCode)) {
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
            /*کلیلەکان*/
            إذا(pKey16!=nullptr) {
                uint16_t *rKey16;

                إذا(pKey16!=qKey16) {
                    rKey16=qKey16;
                } وإلا {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                لكل(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                إذا(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } وإلا {
                int32_t *rKey32;

                إذا(pKey32!=qKey32) {
                    rKey32=qKey32;
                } وإلا {
                    rKey32=pTempTable->resort;
                }
                لكل(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                إذا(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                إذا(p!=q) {
                    r=q;
                } وإلا {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                لكل(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                إذا(q!=r) {
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
                إذا(U_FAILURE(*pErrorCode)) {
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
    Resource ڕەگ_Res;
    int32_t headerSize, maxTableLength;

    Row rows[کەڵەکە_ROW_CAPACITY];
    int32_t resort[کەڵەکە_ROW_CAPACITY];
    TempTable tempTable;

    ثابت int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, کلیلەکان_Bottom, کلیلەکان_لووتکە, resBottom, لووتکە;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)inData+4);
    إذا(!(
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
    إذا(length<0) {
        bundleLength=-1;
    } وإلا {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        إذا(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    inBundle=(ثابت Resource *)((ثابت حرف *)inData+headerSize);
    ڕەگ_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(ثابت int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    إذا(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    کلیلەکان_Bottom=1+indexLength;
    کلیلەکان_لووتکە=udata_readInt32(ds, inIndexes[URES_INDEX_کلیلەکان_لووتکە]);
    إذا(indexLength>URES_INDEX_16_BIT_لووتکە) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_لووتکە]);
    } وإلا {
        resBottom=کلیلەکان_لووتکە;
    }
    لووتکە=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_لووتکە]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    إذا(0<=bundleLength && bundleLength<لووتکە) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         لووتکە, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا(کلیلەکان_لووتکە>(1+indexLength)) {
        tempTable.local_کلیل_Limit=کلیلەکان_لووتکە<<2;
    } وإلا {
        tempTable.local_کلیل_Limit=0;
    }

    إذا(length>=0) {
        Resource *outBundle=(Resource *)((حرف *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t کەڵەکە_Res_Flags[کەڵەکە_ROW_CAPACITY];
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
        إذا(resFlagsLength<=(int32_t)حجم(کەڵەکە_Res_Flags)) {
            tempTable.resFlags=کەڵەکە_Res_Flags;
        } وإلا {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            إذا(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                أرجع 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        إذا(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*لووتکە);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+کلیلەکان_Bottom, 4*(کلیلەکان_لووتکە-کلیلەکان_Bottom),
                                    outBundle+کلیلەکان_Bottom, pErrorCode);
        إذا(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(کلیلەکان_لووتکە-کلیلەکان_Bottom));
            إذا(tempTable.resFlags!=کەڵەکە_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            أرجع 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        إذا(کلیلەکان_لووتکە<resBottom) {
            ds->swapArray16(ds, inBundle+کلیلەکان_لووتکە, (resBottom-کلیلەکان_لووتکە)*4, outBundle+کلیلەکان_لووتکە, pErrorCode);
            إذا(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-کلیلەکان_لووتکە));
                إذا(tempTable.resFlags!=کەڵەکە_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                أرجع 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.کلیل_Chars=(ثابت حرف *)outBundle; /* sort by outCharset */
        إذا(tempTable.majorFormatVersion>1 || maxTableLength<=کەڵەکە_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } وإلا {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*حجم(Row)+maxTableLength*4);
            إذا(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                إذا(tempTable.resFlags!=کەڵەکە_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                أرجع 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, ڕەگ_Res, nullptr, &tempTable, pErrorCode);
        إذا(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             ڕەگ_Res);
        }

        إذا(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        إذا(tempTable.resFlags!=کەڵەکە_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, کلیلەکان_Bottom*4, outBundle, pErrorCode);
    }

    أرجع headerSize+4*لووتکە;
}

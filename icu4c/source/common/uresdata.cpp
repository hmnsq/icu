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

#شامل "unicode/utypes.h"
#شامل "unicode/udata.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "resource.h"
#شامل "uarrsort.h"
#شامل "uassert.h"
#شامل "ucol_swp.h"
#شامل "udataswp.h"
#شامل "uinvchar.h"
#شامل "uresdata.h"
#شامل "uresimp.h"
#شامل "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, کلید_Offset) \
    ((کلید_Offset)<(pResData)->local_کلید_Limit ? \
        (ثابت نویسه *)(pResData)->p_ریشه+(کلید_Offset) : \
        (pResData)->pool_Bundle_کلیدها+(کلید_Offset)-(pResData)->local_کلید_Limit)

#define RES_GET_KEY32(pResData, کلید_Offset) \
    ((کلید_Offset)>=0 ? \
        (ثابت نویسه *)(pResData)->p_ریشه+(کلید_Offset) : \
        (pResData)->pool_Bundle_کلیدها+((کلید_Offset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
ایستا ثابت uint16_t gEmpty16=0;

ایستا ثابت ساختار {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

ایستا ثابت ساختار {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ایستا int32_t
_res_findTableItem(ثابت ResourceData *pResData, ثابت uint16_t *کلید_Offsets, int32_t length,
                   ثابت نویسه *کلید, ثابت نویسه **real_کلید) {
    ثابت نویسه *table_کلید;
    int32_t mid, start, limit;
    صحیح result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    وقتی(start<limit) {
        mid = (start + limit) / 2;
        table_کلید = RES_GET_KEY16(pResData, کلید_Offsets[mid]);
        اگر (pResData->useNativeStrcmp) {
            result = uprv_strcmp(کلید, table_کلید);
        } وگرنه {
            result = uprv_compareInvCharsAsAscii(کلید, table_کلید);
        }
        اگر (result < 0) {
            limit = mid;
        } وگرنه اگر (result > 0) {
            start = mid + 1;
        } وگرنه {
            /* We found it! */
            *real_کلید=table_کلید;
            بازگشت mid;
        }
    }
    بازگشت URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

ایستا int32_t
_res_findTable32Item(ثابت ResourceData *pResData, ثابت int32_t *کلید_Offsets, int32_t length,
                     ثابت نویسه *کلید, ثابت نویسه **real_کلید) {
    ثابت نویسه *table_کلید;
    int32_t mid, start, limit;
    صحیح result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    وقتی(start<limit) {
        mid = (start + limit) / 2;
        table_کلید = RES_GET_KEY32(pResData, کلید_Offsets[mid]);
        اگر (pResData->useNativeStrcmp) {
            result = uprv_strcmp(کلید, table_کلید);
        } وگرنه {
            result = uprv_compareInvCharsAsAscii(کلید, table_کلید);
        }
        اگر (result < 0) {
            limit = mid;
        } وگرنه اگر (result > 0) {
            start = mid + 1;
        } وگرنه {
            /* We found it! */
            *real_کلید=table_کلید;
            بازگشت mid;
        }
    }
    بازگشت URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ایستا UBool U_CALLCONV
isAcceptable(تهی *context,
             ثابت نویسه * /*type*/, ثابت نویسه * /*name*/,
             ثابت UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    بازگشت
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

ایستا تهی
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, ثابت تهی *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType ریشه_Type;

    /* get the root resource */
    pResData->p_ریشه = static_cast<ثابت int32_t*>(inBytes);
    pResData->ریشه_Res = static_cast<Resource>(*pResData->p_ریشه);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    اگر(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        بازگشت;
    }

    /* currently, we accept only resources that have a Table as their roots */
    ریشه_Type = static_cast<UResType>(RES_GET_TYPE(pResData->ریشه_Res));
    اگر(!URES_IS_TABLE(ریشه_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        بازگشت;
    }

    اگر(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_کلید_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } وگرنه {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ثابت int32_t *indexes=pResData->p_ریشه+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        اگر(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            بازگشت;
        }
        اگر( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_بالاترین]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            بازگشت;
        }
        اگر(indexes[URES_INDEX_کلیدها_بالاترین]>(1+indexLength)) {
            pResData->local_کلید_Limit=indexes[URES_INDEX_کلیدها_بالاترین]<<2;
        }
        اگر(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        اگر(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        اگر((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            بازگشت;
        }
        اگر( indexLength>URES_INDEX_16_BIT_بالاترین &&
            indexes[URES_INDEX_16_BIT_بالاترین]>indexes[URES_INDEX_کلیدها_بالاترین]
        ) {
            pResData->p16BitUnits = reinterpret_cast<ثابت uint16_t*>(pResData->p_ریشه + indexes[URES_INDEX_کلیدها_بالاترین]);
        }
    }

    اگر(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=درست;
    }
}

U_CAPI تهی U_EXPORT2
res_read(ResourceData *pResData,
         ثابت UDataInfo *pInfo, ثابت تهی *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, اندازه(ResourceData));
    اگر(U_FAILURE(*errorCode)) {
        بازگشت;
    }
    اگر(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        بازگشت;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC تهی
res_load(ResourceData *pResData,
         ثابت نویسه *path, ثابت نویسه *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, اندازه(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    اگر(U_FAILURE(*errorCode)) {
        بازگشت;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC تهی
res_unload(ResourceData *pResData) {
    اگر(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

ایستا ثابت int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_دودویی,
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
    بازگشت (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI ثابت char16_t * U_EXPORT2
res_getStringNoTrace(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    اگر(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        اگر((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(ثابت char16_t *)pResData->poolBundleStrings+offset;
        } وگرنه {
            p=(ثابت char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        اگر(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } وگرنه اگر(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } وگرنه اگر(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } وگرنه {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } وگرنه اگر(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        ثابت int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_ریشه+res;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وگرنه {
        p=nullptr;
        length=0;
    }
    اگر(pLength) {
        *pLength=length;
    }
    بازگشت p;
}

فضاینام {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ثابت ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    اگر (offset == 0) {
        // empty string
    } وگرنه اگر (res == offset) {
        ثابت int32_t *p32=pResData->p_ریشه+res;
        int32_t length=*p32;
        ثابت char16_t* p = reinterpret_cast<ثابت char16_t*>(p32);
        بازگشت length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } وگرنه اگر (RES_GET_TYPE(res) == URES_STRING_V2) {
        ثابت char16_t *p;
        اگر (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<ثابت char16_t*>(pResData->poolBundleStrings) + offset;
        } وگرنه {
            p = reinterpret_cast<ثابت char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        اگر (first == 0x2205) {  // implicit length
            بازگشت p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } وگرنه اگر (first == 0xdc03) {  // explicit length 3 (should not occur)
            بازگشت p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } وگرنه {
            // Assume that the string has not been stored with more length units than necessary.
            بازگشت نادرست;
        }
    }
    بازگشت نادرست;
}

int32_t getStringArray(ثابت ResourceData *pResData, ثابت icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    اگر(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    int32_t length = array.getSize();
    اگر(length == 0) {
        بازگشت 0;
    }
    اگر(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        بازگشت length;
    }
    برای(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        ثابت char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        اگر(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            بازگشت 0;
        }
        dest[i].setTo(درست, s, sLength);
    }
    بازگشت length;
}

}  // namespace

U_CAPI ثابت char16_t * U_EXPORT2
res_getAlias(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    اگر(RES_GET_TYPE(res)==URES_ALIAS) {
        ثابت int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_ریشه+offset;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وگرنه {
        p=nullptr;
        length=0;
    }
    اگر(pLength) {
        *pLength=length;
    }
    بازگشت p;
}

U_CAPI ثابت uint8_t * U_EXPORT2
res_get_دودویی_No_Trace(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    اگر(RES_GET_TYPE(res)==URES_دودویی) {
        ثابت int32_t *p32= offset==0 ? (ثابت int32_t*)&gEmpty32 : pResData->p_ریشه+offset;
        length=*p32++;
        p=(ثابت uint8_t *)p32;
    } وگرنه {
        p=nullptr;
        length=0;
    }
    اگر(pLength) {
        *pLength=length;
    }
    بازگشت p;
}


U_CAPI ثابت int32_t * U_EXPORT2
res_getIntVectorNoTrace(ثابت ResourceData *pResData, Resource res, int32_t *pLength) {
    ثابت int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    اگر(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (ثابت int32_t *)&gEmpty32 : pResData->p_ریشه+offset;
        length=*p++;
    } وگرنه {
        p=nullptr;
        length=0;
    }
    اگر(pLength) {
        *pLength=length;
    }
    بازگشت p;
}

U_CAPI int32_t U_EXPORT2
res_شمار_Array_اقلام(ثابت ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_STRING:
    حالت URES_STRING_V2:
    حالت URES_دودویی:
    حالت URES_ALIAS:
    حالت URES_INT:
    حالت URES_INT_VECTOR:
        بازگشت 1;
    حالت URES_ARRAY:
    حالت URES_TABLE32:
        بازگشت offset==0 ? 0 : *(pResData->p_ریشه+offset);
    حالت URES_TABLE:
        بازگشت offset==0 ? 0 : *((ثابت uint16_t *)(pResData->p_ریشه+offset));
    حالت URES_ARRAY16:
    حالت URES_TABLE16:
        بازگشت pResData->p16BitUnits[offset];
    پیشفرض:
        بازگشت 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_مقدار::~Resource_Data_مقدار() {}

UResType Resource_Data_مقدار::getType() ثابت {
    بازگشت res_getPublicType(res);
}

ثابت char16_t *Resource_Data_مقدار::getString(int32_t &length, UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    ثابت char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    اگر(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    بازگشت s;
}

ثابت char16_t *Resource_Data_مقدار::getAliasString(int32_t &length, UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    ثابت char16_t *s = res_getAlias(&getData(), res, &length);
    اگر(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    بازگشت s;
}

int32_t Resource_Data_مقدار::getInt(UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    اگر(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    بازگشت res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_مقدار::getUInt(UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    اگر(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    بازگشت res_getUInt(fTraceInfo, res);
}

ثابت int32_t *Resource_Data_مقدار::getIntVector(int32_t &length, UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    ثابت int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    اگر(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    بازگشت iv;
}

ثابت uint8_t *Resource_Data_مقدار::get_دودویی(int32_t &length, UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    ثابت uint8_t *b = res_get_دودویی(fTraceInfo, &getData(), res, &length);
    اگر(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    بازگشت b;
}

ResourceArray Resource_Data_مقدار::getArray(UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت {};
    }
    ثابت uint16_t *items16 = nullptr;
    ثابت Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_ARRAY:
        اگر (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ثابت Resource*>(getData().p_ریشه) + offset;
            length = *items32++;
        }
        بشکن;
    حالت URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        بشکن;
    پیشفرض:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        بازگشت {};
    }
    بازگشت ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_Data_مقدار::getTable(UErrorCode &errorCode) ثابت {
    اگر(U_FAILURE(errorCode)) {
        بازگشت {};
    }
    ثابت uint16_t *keys16 = nullptr;
    ثابت int32_t *keys32 = nullptr;
    ثابت uint16_t *items16 = nullptr;
    ثابت Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_TABLE:
        اگر (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ثابت uint16_t*>(getData().p_ریشه + offset);
            length = *keys16++;
            items32 = reinterpret_cast<ثابت Resource*>(keys16 + length + (~length & 1));
        }
        بشکن;
    حالت URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        بشکن;
    حالت URES_TABLE32:
        اگر (offset != 0) {  // empty if offset==0
            keys32 = getData().p_ریشه+offset;
            length = *keys32++;
            items32 = reinterpret_cast<ثابت Resource*>(keys32) + length;
        }
        بشکن;
    پیشفرض:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        بازگشت {};
    }
    بازگشت ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_Data_مقدار::isNoInheritanceMarker() ثابت {
    بازگشت ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_مقدار::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) ثابت {
    بازگشت ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_مقدار::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) ثابت {
    اگر(URES_IS_ARRAY(res)) {
        بازگشت ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    اگر(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    اگر(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        بازگشت 1;
    }
    int32_t sLength;
    ثابت char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    اگر(s != nullptr) {
        dest[0].setTo(درست, s, sLength);
        بازگشت 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    بازگشت 0;
}

UnicodeString Resource_Data_مقدار::getStringOrFirstOfArray(UErrorCode &errorCode) ثابت {
    UnicodeString us;
    اگر(U_FAILURE(errorCode)) {
        بازگشت us;
    }
    int32_t sLength;
    ثابت char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    اگر(s != nullptr) {
        us.setTo(درست, s, sLength);
        بازگشت us;
    }
    ResourceArray array = getArray(errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت us;
    }
    اگر(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        اگر(s != nullptr) {
            us.setTo(درست, s, sLength);
            بازگشت us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    بازگشت us;
}

U_NAMESPACE_END

ایستا Resource
makeResourceFrom16(ثابت ResourceData *pResData, int32_t res16) {
    اگر(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } وگرنه {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    بازگشت URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_کلید(ثابت ResourceData *pResData, Resource table,
                      int32_t *indexR, ثابت نویسه **کلید) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    اگر(کلید == nullptr || *کلید == nullptr) {
        بازگشت RES_BOGUS;
    }
    انتخاب(RES_GET_TYPE(table)) {
    حالت URES_TABLE: {
        اگر (offset!=0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(pResData->p_ریشه+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *کلید, کلید);
            اگر(idx>=0) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                بازگشت p32[idx];
            }
        }
        بشکن;
    }
    حالت URES_TABLE16: {
        ثابت uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *کلید, کلید);
        اگر(idx>=0) {
            بازگشت makeResourceFrom16(pResData, p[length+idx]);
        }
        بشکن;
    }
    حالت URES_TABLE32: {
        اگر (offset!=0) { /* empty if offset==0 */
            ثابت int32_t *p= pResData->p_ریشه+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *کلید, کلید);
            اگر(idx>=0) {
                بازگشت (Resource)p[length+idx];
            }
        }
        بشکن;
    }
    پیشفرض:
        بشکن;
    }
    بازگشت RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(ثابت ResourceData *pResData, Resource table,
                        int32_t indexR, ثابت نویسه **کلید) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    اگر (indexR < 0) {
        بازگشت RES_BOGUS;
    }
    انتخاب(RES_GET_TYPE(table)) {
    حالت URES_TABLE: {
        اگر (offset != 0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(pResData->p_ریشه+offset);
            length=*p++;
            اگر(indexR<length) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                اگر(کلید!=nullptr) {
                    *کلید=RES_GET_KEY16(pResData, p[indexR]);
                }
                بازگشت p32[indexR];
            }
        }
        بشکن;
    }
    حالت URES_TABLE16: {
        ثابت uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        اگر(indexR<length) {
            اگر(کلید!=nullptr) {
                *کلید=RES_GET_KEY16(pResData, p[indexR]);
            }
            بازگشت makeResourceFrom16(pResData, p[length+indexR]);
        }
        بشکن;
    }
    حالت URES_TABLE32: {
        اگر (offset != 0) { /* empty if offset==0 */
            ثابت int32_t *p= pResData->p_ریشه+offset;
            length=*p++;
            اگر(indexR<length) {
                اگر(کلید!=nullptr) {
                    *کلید=RES_GET_KEY32(pResData, p[indexR]);
                }
                بازگشت (Resource)p[length+indexR];
            }
        }
        بشکن;
    }
    پیشفرض:
        بشکن;
    }
    بازگشت RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ثابت ResourceData *pResData, ثابت نویسه *کلید) {
    ثابت نویسه *real_کلید=کلید;
    int32_t idx;
    بازگشت res_get_Table_Item_By_کلید(pResData, pResData->ریشه_Res, &idx, &real_کلید);
}


UBool icu::ResourceTable::get_کلید_And_مقدار(int32_t i,
                                         ثابت نویسه *&کلید, icu::Resource_مقدار &value) ثابت {
    اگر(0 <= i && i < length) {
        icu::Resource_Data_مقدار &rd_مقدار = static_cast<icu::Resource_Data_مقدار &>(value);
        اگر (keys16 != nullptr) {
            کلید = RES_GET_KEY16(&rd_مقدار.getData(), keys16[i]);
        } وگرنه {
            کلید = RES_GET_KEY32(&rd_مقدار.getData(), keys32[i]);
        }
        Resource res;
        اگر (items16 != nullptr) {
            res = makeResourceFrom16(&rd_مقدار.getData(), items16[i]);
        } وگرنه {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_مقدار.setResource(res, ResourceTracer(fTraceInfo, کلید));
        بازگشت درست;
    }
    بازگشت نادرست;
}

UBool icu::ResourceTable::find_مقدار(ثابت نویسه *کلید, Resource_مقدار &value) ثابت {
    icu::Resource_Data_مقدار &rd_مقدار = static_cast<icu::Resource_Data_مقدار &>(value);
    ثابت نویسه *real_کلید = nullptr;
    int32_t i;
    اگر (keys16 != nullptr) {
        i = _res_findTableItem(&rd_مقدار.getData(), keys16, length, کلید, &real_کلید);
    } وگرنه {
        i = _res_findTable32Item(&rd_مقدار.getData(), keys32, length, کلید, &real_کلید);
    }
    اگر (i >= 0) {
        Resource res;
        اگر (items16 != nullptr) {
            res = makeResourceFrom16(&rd_مقدار.getData(), items16[i]);
        } وگرنه {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_مقدار.setResource(res, ResourceTracer(fTraceInfo, کلید));
        بازگشت درست;
    }
    بازگشت نادرست;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ثابت ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    اگر (indexR < 0) {
        بازگشت RES_BOGUS;
    }
    انتخاب(RES_GET_TYPE(array)) {
    حالت URES_ARRAY: {
        اگر (offset!=0) { /* empty if offset==0 */
            ثابت int32_t *p= pResData->p_ریشه+offset;
            اگر(indexR<*p) {
                بازگشت (Resource)p[1+indexR];
            }
        }
        بشکن;
    }
    حالت URES_ARRAY16: {
        ثابت uint16_t *p=pResData->p16BitUnits+offset;
        اگر(indexR<*p) {
            بازگشت makeResourceFrom16(pResData, p[1+indexR]);
        }
        بشکن;
    }
    پیشفرض:
        بشکن;
    }
    بازگشت RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(ثابت ResourceData *pResData, int32_t i) ثابت {
    اگر (items16 != nullptr) {
        بازگشت makeResourceFrom16(pResData, items16[i]);
    } وگرنه {
        بازگشت items32[i];
    }
}

UBool icu::ResourceArray::get_مقدار(int32_t i, icu::Resource_مقدار &value) ثابت {
    اگر(0 <= i && i < length) {
        icu::Resource_Data_مقدار &rd_مقدار = static_cast<icu::Resource_Data_مقدار &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_مقدار.setResource(
            internalGetResource(&rd_مقدار.getData(), i),
            ResourceTracer(fTraceInfo, i));
        بازگشت درست;
    }
    بازگشت نادرست;
}

U_CFUNC Resource
res_findResource(ثابت ResourceData *pResData, Resource r, نویسه** path, ثابت نویسه** کلید) {
  نویسه *pathP = *path, *nextSepP = *path;
  نویسه *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  اگر(!uprv_strlen(pathP)) {
      بازگشت r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  اگر(!URES_IS_CONTAINER(type)) {
      بازگشت RES_BOGUS;
  }
  
  وقتی(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    اگر(nextSepP != nullptr) {
      اگر(nextSepP == pathP) {
        // Empty key string.
        بازگشت RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } وگرنه {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    اگر(URES_IS_TABLE(type)) {
      *کلید = pathP;
      t2 = res_get_Table_Item_By_کلید(pResData, t1, &indexR, کلید);
    } وگرنه اگر(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      اگر(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } وگرنه {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *کلید = nullptr;
    } وگرنه { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  بازگشت t1;
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
تعریفنوع ساختار Row {
    int32_t کلید_Index, sortIndex;
} Row;

ایستا int32_t U_CALLCONV
ures_compareRows(ثابت تهی *context, ثابت تهی *left, ثابت تهی *راست) {
    ثابت نویسه* کلید_Chars = static_cast<ثابت نویسه*>(context);
    بازگشت static_cast<int32_t>(uprv_strcmp(کلید_Chars + static_cast<ثابت Row*>(left)->کلید_Index,
                                            کلید_Chars + static_cast<ثابت Row*>(راست)->کلید_Index));
}

تعریفنوع ساختار TempTable {
    ثابت نویسه *کلید_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_کلید_Limit;
    uint8_t majorFormatVersion;
} TempTable;

شمارش {
    پشته_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
ایستا ثابت نویسه *ثابت g_Unknown_کلید="";

#اگر !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ایستا ثابت char16_t g_Collation_Bin_کلید[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
ایستا تهی
ures_swapResource(ثابت UDataSwapper *ds,
                  ثابت Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  ثابت نویسه *کلید,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    ثابت Resource *p;
    Resource *q;
    int32_t offset, count;

    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_TABLE16:
    حالت URES_STRING_V2:
    حالت URES_INT:
    حالت URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        بازگشت;
    پیشفرض:
        بشکن;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    اگر(offset==0) {
        /* special offset indicating an empty item */
        بازگشت;
    }
    اگر (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        بازگشت;
    } وگرنه {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    حالت URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        بشکن;
    حالت URES_دودویی:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#اگر !UCONFIG_NO_COLLATION
        اگر( کلید!=nullptr &&  /* the binary is in a table */
            (کلید!=g_Unknown_کلید ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, کلید, -1,
                                       g_Collation_Bin_کلید, UPRV_LENGTHOF(g_Collation_Bin_کلید)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_دودویی(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#endif
        بشکن;
    حالت URES_TABLE:
    حالت URES_TABLE32:
        {
            ثابت uint16_t *pKey16;
            uint16_t *qKey16;

            ثابت int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            اگر(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<ثابت uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } وگرنه {
                /* get table item count */
                pKey32 = reinterpret_cast<ثابت int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            اگر(count==0) {
                بشکن;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            برای(i=0; i<count; ++i) {
                ثابت نویسه *item_کلید=g_Unknown_کلید;
                اگر(pKey16!=nullptr) {
                    int32_t کلید_Offset=ds->readUInt16(pKey16[i]);
                    اگر(کلید_Offset<pTempTable->local_کلید_Limit) {
                        item_کلید = reinterpret_cast<ثابت نویسه*>(outBundle) + کلید_Offset;
                    }
                } وگرنه {
                    int32_t کلید_Offset=udata_readInt32(ds, pKey32[i]);
                    اگر(کلید_Offset>=0) {
                        item_کلید = reinterpret_cast<ثابت نویسه*>(outBundle) + کلید_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_کلید, pTempTable, pErrorCode);
                اگر(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    بازگشت;
                }
            }

            اگر(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                اگر(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } وگرنه {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                بشکن;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            اگر(pKey16!=nullptr) {
                برای(i=0; i<count; ++i) {
                    pTempTable->rows[i].کلید_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } وگرنه {
                برای(i=0; i<count; ++i) {
                    pTempTable->rows[i].کلید_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, اندازه(Row),
                           ures_compareRows, pTempTable->کلید_Chars,
                           نادرست, pErrorCode);
            اگر(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                بازگشت;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*کلیدها*/
            اگر(pKey16!=nullptr) {
                uint16_t *rKey16;

                اگر(pKey16!=qKey16) {
                    rKey16=qKey16;
                } وگرنه {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                برای(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                اگر(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } وگرنه {
                int32_t *rKey32;

                اگر(pKey32!=qKey32) {
                    rKey32=qKey32;
                } وگرنه {
                    rKey32=pTempTable->resort;
                }
                برای(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                اگر(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                اگر(p!=q) {
                    r=q;
                } وگرنه {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                برای(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                اگر(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        بشکن;
    حالت URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            برای(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                اگر(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    بازگشت;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        بشکن;
    حالت URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        بشکن;
    پیشفرض:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        بشکن;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(ثابت UDataSwapper *ds,
          ثابت تهی *inData, int32_t length, تهی *outData,
          UErrorCode *pErrorCode) {
    ثابت UDataInfo *pInfo;
    ثابت Resource *inBundle;
    Resource ریشه_Res;
    int32_t headerSize, maxTableLength;

    Row rows[پشته_ROW_CAPACITY];
    int32_t resort[پشته_ROW_CAPACITY];
    TempTable tempTable;

    ثابت int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, کلیدها_Bottom, کلیدها_بالاترین, resBottom, بالاترین;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت نویسه *)inData+4);
    اگر(!(
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
        بازگشت 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    اگر(length<0) {
        bundleLength=-1;
    } وگرنه {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        اگر(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }
    }

    inBundle=(ثابت Resource *)((ثابت نویسه *)inData+headerSize);
    ریشه_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(ثابت int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    اگر(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }
    کلیدها_Bottom=1+indexLength;
    کلیدها_بالاترین=udata_readInt32(ds, inIndexes[URES_INDEX_کلیدها_بالاترین]);
    اگر(indexLength>URES_INDEX_16_BIT_بالاترین) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_بالاترین]);
    } وگرنه {
        resBottom=کلیدها_بالاترین;
    }
    بالاترین=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_بالاترین]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    اگر(0<=bundleLength && bundleLength<بالاترین) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         بالاترین, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }
    اگر(کلیدها_بالاترین>(1+indexLength)) {
        tempTable.local_کلید_Limit=کلیدها_بالاترین<<2;
    } وگرنه {
        tempTable.local_کلید_Limit=0;
    }

    اگر(length>=0) {
        Resource *outBundle=(Resource *)((نویسه *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t پشته_Res_Flags[پشته_ROW_CAPACITY];
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
        اگر(resFlagsLength<=(int32_t)اندازه(پشته_Res_Flags)) {
            tempTable.resFlags=پشته_Res_Flags;
        } وگرنه {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            اگر(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                بازگشت 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        اگر(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*بالاترین);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+کلیدها_Bottom, 4*(کلیدها_بالاترین-کلیدها_Bottom),
                                    outBundle+کلیدها_Bottom, pErrorCode);
        اگر(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(کلیدها_بالاترین-کلیدها_Bottom));
            اگر(tempTable.resFlags!=پشته_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            بازگشت 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        اگر(کلیدها_بالاترین<resBottom) {
            ds->swapArray16(ds, inBundle+کلیدها_بالاترین, (resBottom-کلیدها_بالاترین)*4, outBundle+کلیدها_بالاترین, pErrorCode);
            اگر(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-کلیدها_بالاترین));
                اگر(tempTable.resFlags!=پشته_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                بازگشت 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.کلید_Chars=(ثابت نویسه *)outBundle; /* sort by outCharset */
        اگر(tempTable.majorFormatVersion>1 || maxTableLength<=پشته_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } وگرنه {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*اندازه(Row)+maxTableLength*4);
            اگر(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                اگر(tempTable.resFlags!=پشته_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                بازگشت 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, ریشه_Res, nullptr, &tempTable, pErrorCode);
        اگر(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             ریشه_Res);
        }

        اگر(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        اگر(tempTable.resFlags!=پشته_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, کلیدها_Bottom*4, outBundle, pErrorCode);
    }

    بازگشت headerSize+4*بالاترین;
}

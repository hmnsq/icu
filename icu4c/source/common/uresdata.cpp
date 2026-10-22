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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "resource.h"
#অন্তর্ভাব "uarrsort.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucol_swp.h"
#অন্তর্ভাব "udataswp.h"
#অন্তর্ভাব "uinvchar.h"
#অন্তর্ভাব "uresdata.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, চাবি_Offset) \
    ((চাবি_Offset)<(pResData)->local_চাবি_Limit ? \
        (ধ্রুব বর্ণ *)(pResData)->p_মূল+(চাবি_Offset) : \
        (pResData)->pool_Bundle_চাবিগুলি+(চাবি_Offset)-(pResData)->local_চাবি_Limit)

#define RES_GET_KEY32(pResData, চাবি_Offset) \
    ((চাবি_Offset)>=0 ? \
        (ধ্রুব বর্ণ *)(pResData)->p_মূল+(চাবি_Offset) : \
        (pResData)->pool_Bundle_চাবিগুলি+((চাবি_Offset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
স্থির ধ্রুব uint16_t gEmpty16=0;

স্থির ধ্রুব সংরচনা {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

স্থির ধ্রুব সংরচনা {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

স্থির int32_t
_res_findTableItem(ধ্রুব ResourceData *pResData, ধ্রুব uint16_t *চাবি_Offsets, int32_t length,
                   ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ **real_চাবি) {
    ধ্রুব বর্ণ *table_চাবি;
    int32_t mid, start, limit;
    পূর্ণাঙ্ক result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    যাবত্(start<limit) {
        mid = (start + limit) / 2;
        table_চাবি = RES_GET_KEY16(pResData, চাবি_Offsets[mid]);
        যদি (pResData->useNativeStrcmp) {
            result = uprv_strcmp(চাবি, table_চাবি);
        } অন্যথা {
            result = uprv_compareInvCharsAsAscii(চাবি, table_চাবি);
        }
        যদি (result < 0) {
            limit = mid;
        } অন্যথা যদি (result > 0) {
            start = mid + 1;
        } অন্যথা {
            /* We found it! */
            *real_চাবি=table_চাবি;
            প্রত্যাগম mid;
        }
    }
    প্রত্যাগম URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

স্থির int32_t
_res_findTable32Item(ধ্রুব ResourceData *pResData, ধ্রুব int32_t *চাবি_Offsets, int32_t length,
                     ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ **real_চাবি) {
    ধ্রুব বর্ণ *table_চাবি;
    int32_t mid, start, limit;
    পূর্ণাঙ্ক result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    যাবত্(start<limit) {
        mid = (start + limit) / 2;
        table_চাবি = RES_GET_KEY32(pResData, চাবি_Offsets[mid]);
        যদি (pResData->useNativeStrcmp) {
            result = uprv_strcmp(চাবি, table_চাবি);
        } অন্যথা {
            result = uprv_compareInvCharsAsAscii(চাবি, table_চাবি);
        }
        যদি (result < 0) {
            limit = mid;
        } অন্যথা যদি (result > 0) {
            start = mid + 1;
        } অন্যথা {
            /* We found it! */
            *real_চাবি=table_চাবি;
            প্রত্যাগম mid;
        }
    }
    প্রত্যাগম URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

স্থির UBool U_CALLCONV
isAcceptable(শূন্য *context,
             ধ্রুব বর্ণ * /*type*/, ধ্রুব বর্ণ * /*name*/,
             ধ্রুব UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    প্রত্যাগম
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

স্থির শূন্য
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, ধ্রুব শূন্য *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType মূল_Type;

    /* get the root resource */
    pResData->p_মূল = static_cast<ধ্রুব int32_t*>(inBytes);
    pResData->মূল_Res = static_cast<Resource>(*pResData->p_মূল);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    যদি(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        প্রত্যাগম;
    }

    /* currently, we accept only resources that have a Table as their roots */
    মূল_Type = static_cast<UResType>(RES_GET_TYPE(pResData->মূল_Res));
    যদি(!URES_IS_TABLE(মূল_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        প্রত্যাগম;
    }

    যদি(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_চাবি_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } অন্যথা {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ধ্রুব int32_t *indexes=pResData->p_মূল+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        যদি(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            প্রত্যাগম;
        }
        যদি( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_শীর্ষ]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            প্রত্যাগম;
        }
        যদি(indexes[URES_INDEX_চাবিগুলি_শীর্ষ]>(1+indexLength)) {
            pResData->local_চাবি_Limit=indexes[URES_INDEX_চাবিগুলি_শীর্ষ]<<2;
        }
        যদি(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        যদি(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        যদি((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            প্রত্যাগম;
        }
        যদি( indexLength>URES_INDEX_16_BIT_শীর্ষ &&
            indexes[URES_INDEX_16_BIT_শীর্ষ]>indexes[URES_INDEX_চাবিগুলি_শীর্ষ]
        ) {
            pResData->p16BitUnits = reinterpret_cast<ধ্রুব uint16_t*>(pResData->p_মূল + indexes[URES_INDEX_চাবিগুলি_শীর্ষ]);
        }
    }

    যদি(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=সত্য;
    }
}

U_CAPI শূন্য U_EXPORT2
res_read(ResourceData *pResData,
         ধ্রুব UDataInfo *pInfo, ধ্রুব শূন্য *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, পরিমাণ(ResourceData));
    যদি(U_FAILURE(*errorCode)) {
        প্রত্যাগম;
    }
    যদি(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        প্রত্যাগম;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC শূন্য
res_load(ResourceData *pResData,
         ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, পরিমাণ(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    যদি(U_FAILURE(*errorCode)) {
        প্রত্যাগম;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC শূন্য
res_unload(ResourceData *pResData) {
    যদি(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

স্থির ধ্রুব int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_দ্বিমিক,
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
    প্রত্যাগম (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI ধ্রুব char16_t * U_EXPORT2
res_getStringNoTrace(ধ্রুব ResourceData *pResData, Resource res, int32_t *pLength) {
    ধ্রুব char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    যদি(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        যদি((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(ধ্রুব char16_t *)pResData->poolBundleStrings+offset;
        } অন্যথা {
            p=(ধ্রুব char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        যদি(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } অন্যথা যদি(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } অন্যথা যদি(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } অন্যথা {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } অন্যথা যদি(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        ধ্রুব int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_মূল+res;
        length=*p32++;
        p=(ধ্রুব char16_t *)p32;
    } অন্যথা {
        p=nullptr;
        length=0;
    }
    যদি(pLength) {
        *pLength=length;
    }
    প্রত্যাগম p;
}

নামাকাশ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ধ্রুব ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    যদি (offset == 0) {
        // empty string
    } অন্যথা যদি (res == offset) {
        ধ্রুব int32_t *p32=pResData->p_মূল+res;
        int32_t length=*p32;
        ধ্রুব char16_t* p = reinterpret_cast<ধ্রুব char16_t*>(p32);
        প্রত্যাগম length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } অন্যথা যদি (RES_GET_TYPE(res) == URES_STRING_V2) {
        ধ্রুব char16_t *p;
        যদি (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<ধ্রুব char16_t*>(pResData->poolBundleStrings) + offset;
        } অন্যথা {
            p = reinterpret_cast<ধ্রুব char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        যদি (first == 0x2205) {  // implicit length
            প্রত্যাগম p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } অন্যথা যদি (first == 0xdc03) {  // explicit length 3 (should not occur)
            প্রত্যাগম p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } অন্যথা {
            // Assume that the string has not been stored with more length units than necessary.
            প্রত্যাগম অসত্য;
        }
    }
    প্রত্যাগম অসত্য;
}

int32_t getStringArray(ধ্রুব ResourceData *pResData, ধ্রুব icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }
    যদি(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }
    int32_t length = array.getSize();
    যদি(length == 0) {
        প্রত্যাগম 0;
    }
    যদি(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        প্রত্যাগম length;
    }
    প্রতি(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        ধ্রুব char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        যদি(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            প্রত্যাগম 0;
        }
        dest[i].setTo(সত্য, s, sLength);
    }
    প্রত্যাগম length;
}

}  // namespace

U_CAPI ধ্রুব char16_t * U_EXPORT2
res_getAlias(ধ্রুব ResourceData *pResData, Resource res, int32_t *pLength) {
    ধ্রুব char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    যদি(RES_GET_TYPE(res)==URES_ALIAS) {
        ধ্রুব int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_মূল+offset;
        length=*p32++;
        p=(ধ্রুব char16_t *)p32;
    } অন্যথা {
        p=nullptr;
        length=0;
    }
    যদি(pLength) {
        *pLength=length;
    }
    প্রত্যাগম p;
}

U_CAPI ধ্রুব uint8_t * U_EXPORT2
res_get_দ্বিমিক_No_Trace(ধ্রুব ResourceData *pResData, Resource res, int32_t *pLength) {
    ধ্রুব uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    যদি(RES_GET_TYPE(res)==URES_দ্বিমিক) {
        ধ্রুব int32_t *p32= offset==0 ? (ধ্রুব int32_t*)&gEmpty32 : pResData->p_মূল+offset;
        length=*p32++;
        p=(ধ্রুব uint8_t *)p32;
    } অন্যথা {
        p=nullptr;
        length=0;
    }
    যদি(pLength) {
        *pLength=length;
    }
    প্রত্যাগম p;
}


U_CAPI ধ্রুব int32_t * U_EXPORT2
res_getIntVectorNoTrace(ধ্রুব ResourceData *pResData, Resource res, int32_t *pLength) {
    ধ্রুব int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    যদি(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (ধ্রুব int32_t *)&gEmpty32 : pResData->p_মূল+offset;
        length=*p++;
    } অন্যথা {
        p=nullptr;
        length=0;
    }
    যদি(pLength) {
        *pLength=length;
    }
    প্রত্যাগম p;
}

U_CAPI int32_t U_EXPORT2
res_গুনতি_Array_বস্তুসমূহ(ধ্রুব ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    বিকল্প(RES_GET_TYPE(res)) {
    পক্ষ URES_STRING:
    পক্ষ URES_STRING_V2:
    পক্ষ URES_দ্বিমিক:
    পক্ষ URES_ALIAS:
    পক্ষ URES_INT:
    পক্ষ URES_INT_VECTOR:
        প্রত্যাগম 1;
    পক্ষ URES_ARRAY:
    পক্ষ URES_TABLE32:
        প্রত্যাগম offset==0 ? 0 : *(pResData->p_মূল+offset);
    পক্ষ URES_TABLE:
        প্রত্যাগম offset==0 ? 0 : *((ধ্রুব uint16_t *)(pResData->p_মূল+offset));
    পক্ষ URES_ARRAY16:
    পক্ষ URES_TABLE16:
        প্রত্যাগম pResData->p16BitUnits[offset];
    স্বভাব:
        প্রত্যাগম 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_মান::~Resource_Data_মান() {}

UResType Resource_Data_মান::getType() ধ্রুব {
    প্রত্যাগম res_getPublicType(res);
}

ধ্রুব char16_t *Resource_Data_মান::getString(int32_t &length, UErrorCode &errorCode) ধ্রুব {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম nullptr;
    }
    ধ্রুব char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    যদি(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    প্রত্যাগম s;
}

ধ্রুব char16_t *Resource_Data_মান::getAliasString(int32_t &length, UErrorCode &errorCode) ধ্রুব {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম nullptr;
    }
    ধ্রুব char16_t *s = res_getAlias(&getData(), res, &length);
    যদি(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    প্রত্যাগম s;
}

int32_t Resource_Data_মান::getInt(UErrorCode &errorCode) ধ্রুব {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }
    যদি(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    প্রত্যাগম res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_মান::getUInt(UErrorCode &errorCode) ধ্রুব {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }
    যদি(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    প্রত্যাগম res_getUInt(fTraceInfo, res);
}

ধ্রুব int32_t *Resource_Data_মান::getIntVector(int32_t &length, UErrorCode &errorCode) ধ্রুব {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম nullptr;
    }
    ধ্রুব int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    যদি(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    প্রত্যাগম iv;
}

ধ্রুব uint8_t *Resource_Data_মান::get_দ্বিমিক(int32_t &length, UErrorCode &errorCode) ধ্রুব {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম nullptr;
    }
    ধ্রুব uint8_t *b = res_get_দ্বিমিক(fTraceInfo, &getData(), res, &length);
    যদি(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    প্রত্যাগম b;
}

ResourceArray Resource_Data_মান::getArray(UErrorCode &errorCode) ধ্রুব {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম {};
    }
    ধ্রুব uint16_t *items16 = nullptr;
    ধ্রুব Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    বিকল্প(RES_GET_TYPE(res)) {
    পক্ষ URES_ARRAY:
        যদি (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ধ্রুব Resource*>(getData().p_মূল) + offset;
            length = *items32++;
        }
        ভঙ্গ;
    পক্ষ URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        ভঙ্গ;
    স্বভাব:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        প্রত্যাগম {};
    }
    প্রত্যাগম ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_Data_মান::getTable(UErrorCode &errorCode) ধ্রুব {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম {};
    }
    ধ্রুব uint16_t *keys16 = nullptr;
    ধ্রুব int32_t *keys32 = nullptr;
    ধ্রুব uint16_t *items16 = nullptr;
    ধ্রুব Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    বিকল্প(RES_GET_TYPE(res)) {
    পক্ষ URES_TABLE:
        যদি (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ধ্রুব uint16_t*>(getData().p_মূল + offset);
            length = *keys16++;
            items32 = reinterpret_cast<ধ্রুব Resource*>(keys16 + length + (~length & 1));
        }
        ভঙ্গ;
    পক্ষ URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        ভঙ্গ;
    পক্ষ URES_TABLE32:
        যদি (offset != 0) {  // empty if offset==0
            keys32 = getData().p_মূল+offset;
            length = *keys32++;
            items32 = reinterpret_cast<ধ্রুব Resource*>(keys32) + length;
        }
        ভঙ্গ;
    স্বভাব:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        প্রত্যাগম {};
    }
    প্রত্যাগম ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_Data_মান::isNoInheritanceMarker() ধ্রুব {
    প্রত্যাগম ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_মান::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) ধ্রুব {
    প্রত্যাগম ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_মান::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) ধ্রুব {
    যদি(URES_IS_ARRAY(res)) {
        প্রত্যাগম ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }
    যদি(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }
    যদি(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        প্রত্যাগম 1;
    }
    int32_t sLength;
    ধ্রুব char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    যদি(s != nullptr) {
        dest[0].setTo(সত্য, s, sLength);
        প্রত্যাগম 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    প্রত্যাগম 0;
}

UnicodeString Resource_Data_মান::getStringOrFirstOfArray(UErrorCode &errorCode) ধ্রুব {
    UnicodeString us;
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম us;
    }
    int32_t sLength;
    ধ্রুব char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    যদি(s != nullptr) {
        us.setTo(সত্য, s, sLength);
        প্রত্যাগম us;
    }
    ResourceArray array = getArray(errorCode);
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম us;
    }
    যদি(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        যদি(s != nullptr) {
            us.setTo(সত্য, s, sLength);
            প্রত্যাগম us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    প্রত্যাগম us;
}

U_NAMESPACE_END

স্থির Resource
makeResourceFrom16(ধ্রুব ResourceData *pResData, int32_t res16) {
    যদি(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } অন্যথা {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    প্রত্যাগম URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_চাবি(ধ্রুব ResourceData *pResData, Resource table,
                      int32_t *indexR, ধ্রুব বর্ণ **চাবি) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    যদি(চাবি == nullptr || *চাবি == nullptr) {
        প্রত্যাগম RES_BOGUS;
    }
    বিকল্প(RES_GET_TYPE(table)) {
    পক্ষ URES_TABLE: {
        যদি (offset!=0) { /* empty if offset==0 */
            ধ্রুব uint16_t *p= (ধ্রুব uint16_t *)(pResData->p_মূল+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *চাবি, চাবি);
            যদি(idx>=0) {
                ধ্রুব Resource *p32=(ধ্রুব Resource *)(p+length+(~length&1));
                প্রত্যাগম p32[idx];
            }
        }
        ভঙ্গ;
    }
    পক্ষ URES_TABLE16: {
        ধ্রুব uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *চাবি, চাবি);
        যদি(idx>=0) {
            প্রত্যাগম makeResourceFrom16(pResData, p[length+idx]);
        }
        ভঙ্গ;
    }
    পক্ষ URES_TABLE32: {
        যদি (offset!=0) { /* empty if offset==0 */
            ধ্রুব int32_t *p= pResData->p_মূল+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *চাবি, চাবি);
            যদি(idx>=0) {
                প্রত্যাগম (Resource)p[length+idx];
            }
        }
        ভঙ্গ;
    }
    স্বভাব:
        ভঙ্গ;
    }
    প্রত্যাগম RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(ধ্রুব ResourceData *pResData, Resource table,
                        int32_t indexR, ধ্রুব বর্ণ **চাবি) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    যদি (indexR < 0) {
        প্রত্যাগম RES_BOGUS;
    }
    বিকল্প(RES_GET_TYPE(table)) {
    পক্ষ URES_TABLE: {
        যদি (offset != 0) { /* empty if offset==0 */
            ধ্রুব uint16_t *p= (ধ্রুব uint16_t *)(pResData->p_মূল+offset);
            length=*p++;
            যদি(indexR<length) {
                ধ্রুব Resource *p32=(ধ্রুব Resource *)(p+length+(~length&1));
                যদি(চাবি!=nullptr) {
                    *চাবি=RES_GET_KEY16(pResData, p[indexR]);
                }
                প্রত্যাগম p32[indexR];
            }
        }
        ভঙ্গ;
    }
    পক্ষ URES_TABLE16: {
        ধ্রুব uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        যদি(indexR<length) {
            যদি(চাবি!=nullptr) {
                *চাবি=RES_GET_KEY16(pResData, p[indexR]);
            }
            প্রত্যাগম makeResourceFrom16(pResData, p[length+indexR]);
        }
        ভঙ্গ;
    }
    পক্ষ URES_TABLE32: {
        যদি (offset != 0) { /* empty if offset==0 */
            ধ্রুব int32_t *p= pResData->p_মূল+offset;
            length=*p++;
            যদি(indexR<length) {
                যদি(চাবি!=nullptr) {
                    *চাবি=RES_GET_KEY32(pResData, p[indexR]);
                }
                প্রত্যাগম (Resource)p[length+indexR];
            }
        }
        ভঙ্গ;
    }
    স্বভাব:
        ভঙ্গ;
    }
    প্রত্যাগম RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ধ্রুব ResourceData *pResData, ধ্রুব বর্ণ *চাবি) {
    ধ্রুব বর্ণ *real_চাবি=চাবি;
    int32_t idx;
    প্রত্যাগম res_get_Table_Item_By_চাবি(pResData, pResData->মূল_Res, &idx, &real_চাবি);
}


UBool icu::ResourceTable::get_চাবি_And_মান(int32_t i,
                                         ধ্রুব বর্ণ *&চাবি, icu::Resource_মান &value) ধ্রুব {
    যদি(0 <= i && i < length) {
        icu::Resource_Data_মান &rd_মান = static_cast<icu::Resource_Data_মান &>(value);
        যদি (keys16 != nullptr) {
            চাবি = RES_GET_KEY16(&rd_মান.getData(), keys16[i]);
        } অন্যথা {
            চাবি = RES_GET_KEY32(&rd_মান.getData(), keys32[i]);
        }
        Resource res;
        যদি (items16 != nullptr) {
            res = makeResourceFrom16(&rd_মান.getData(), items16[i]);
        } অন্যথা {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_মান.setResource(res, ResourceTracer(fTraceInfo, চাবি));
        প্রত্যাগম সত্য;
    }
    প্রত্যাগম অসত্য;
}

UBool icu::ResourceTable::find_মান(ধ্রুব বর্ণ *চাবি, Resource_মান &value) ধ্রুব {
    icu::Resource_Data_মান &rd_মান = static_cast<icu::Resource_Data_মান &>(value);
    ধ্রুব বর্ণ *real_চাবি = nullptr;
    int32_t i;
    যদি (keys16 != nullptr) {
        i = _res_findTableItem(&rd_মান.getData(), keys16, length, চাবি, &real_চাবি);
    } অন্যথা {
        i = _res_findTable32Item(&rd_মান.getData(), keys32, length, চাবি, &real_চাবি);
    }
    যদি (i >= 0) {
        Resource res;
        যদি (items16 != nullptr) {
            res = makeResourceFrom16(&rd_মান.getData(), items16[i]);
        } অন্যথা {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_মান.setResource(res, ResourceTracer(fTraceInfo, চাবি));
        প্রত্যাগম সত্য;
    }
    প্রত্যাগম অসত্য;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ধ্রুব ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    যদি (indexR < 0) {
        প্রত্যাগম RES_BOGUS;
    }
    বিকল্প(RES_GET_TYPE(array)) {
    পক্ষ URES_ARRAY: {
        যদি (offset!=0) { /* empty if offset==0 */
            ধ্রুব int32_t *p= pResData->p_মূল+offset;
            যদি(indexR<*p) {
                প্রত্যাগম (Resource)p[1+indexR];
            }
        }
        ভঙ্গ;
    }
    পক্ষ URES_ARRAY16: {
        ধ্রুব uint16_t *p=pResData->p16BitUnits+offset;
        যদি(indexR<*p) {
            প্রত্যাগম makeResourceFrom16(pResData, p[1+indexR]);
        }
        ভঙ্গ;
    }
    স্বভাব:
        ভঙ্গ;
    }
    প্রত্যাগম RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(ধ্রুব ResourceData *pResData, int32_t i) ধ্রুব {
    যদি (items16 != nullptr) {
        প্রত্যাগম makeResourceFrom16(pResData, items16[i]);
    } অন্যথা {
        প্রত্যাগম items32[i];
    }
}

UBool icu::ResourceArray::get_মান(int32_t i, icu::Resource_মান &value) ধ্রুব {
    যদি(0 <= i && i < length) {
        icu::Resource_Data_মান &rd_মান = static_cast<icu::Resource_Data_মান &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_মান.setResource(
            internalGetResource(&rd_মান.getData(), i),
            ResourceTracer(fTraceInfo, i));
        প্রত্যাগম সত্য;
    }
    প্রত্যাগম অসত্য;
}

U_CFUNC Resource
res_findResource(ধ্রুব ResourceData *pResData, Resource r, বর্ণ** path, ধ্রুব বর্ণ** চাবি) {
  বর্ণ *pathP = *path, *nextSepP = *path;
  বর্ণ *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  যদি(!uprv_strlen(pathP)) {
      প্রত্যাগম r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  যদি(!URES_IS_CONTAINER(type)) {
      প্রত্যাগম RES_BOGUS;
  }
  
  যাবত্(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    যদি(nextSepP != nullptr) {
      যদি(nextSepP == pathP) {
        // Empty key string.
        প্রত্যাগম RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } অন্যথা {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    যদি(URES_IS_TABLE(type)) {
      *চাবি = pathP;
      t2 = res_get_Table_Item_By_চাবি(pResData, t1, &indexR, চাবি);
    } অন্যথা যদি(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      যদি(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } অন্যথা {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *চাবি = nullptr;
    } অন্যথা { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  প্রত্যাগম t1;
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
প্রকারনির্দেশ সংরচনা Row {
    int32_t চাবি_Index, sortIndex;
} Row;

স্থির int32_t U_CALLCONV
ures_compareRows(ধ্রুব শূন্য *context, ধ্রুব শূন্য *left, ধ্রুব শূন্য *ডান) {
    ধ্রুব বর্ণ* চাবি_Chars = static_cast<ধ্রুব বর্ণ*>(context);
    প্রত্যাগম static_cast<int32_t>(uprv_strcmp(চাবি_Chars + static_cast<ধ্রুব Row*>(left)->চাবি_Index,
                                            চাবি_Chars + static_cast<ধ্রুব Row*>(ডান)->চাবি_Index));
}

প্রকারনির্দেশ সংরচনা TempTable {
    ধ্রুব বর্ণ *চাবি_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_চাবি_Limit;
    uint8_t majorFormatVersion;
} TempTable;

গণনা {
    স্তূপ_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
স্থির ধ্রুব বর্ণ *ধ্রুব g_Unknown_চাবি="";

#যদি !UCONFIG_NO_COLLATION
// resource table key for collation binaries
স্থির ধ্রুব char16_t g_Collation_Bin_চাবি[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
স্থির শূন্য
ures_swapResource(ধ্রুব UDataSwapper *ds,
                  ধ্রুব Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  ধ্রুব বর্ণ *চাবি,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    ধ্রুব Resource *p;
    Resource *q;
    int32_t offset, count;

    বিকল্প(RES_GET_TYPE(res)) {
    পক্ষ URES_TABLE16:
    পক্ষ URES_STRING_V2:
    পক্ষ URES_INT:
    পক্ষ URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        প্রত্যাগম;
    স্বভাব:
        ভঙ্গ;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    যদি(offset==0) {
        /* special offset indicating an empty item */
        প্রত্যাগম;
    }
    যদি (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        প্রত্যাগম;
    } অন্যথা {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    বিকল্প(RES_GET_TYPE(res)) {
    পক্ষ URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    পক্ষ URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        ভঙ্গ;
    পক্ষ URES_দ্বিমিক:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#যদি !UCONFIG_NO_COLLATION
        যদি( চাবি!=nullptr &&  /* the binary is in a table */
            (চাবি!=g_Unknown_চাবি ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, চাবি, -1,
                                       g_Collation_Bin_চাবি, UPRV_LENGTHOF(g_Collation_Bin_চাবি)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_দ্বিমিক(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#endif
        ভঙ্গ;
    পক্ষ URES_TABLE:
    পক্ষ URES_TABLE32:
        {
            ধ্রুব uint16_t *pKey16;
            uint16_t *qKey16;

            ধ্রুব int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            যদি(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<ধ্রুব uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } অন্যথা {
                /* get table item count */
                pKey32 = reinterpret_cast<ধ্রুব int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            যদি(count==0) {
                ভঙ্গ;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            প্রতি(i=0; i<count; ++i) {
                ধ্রুব বর্ণ *item_চাবি=g_Unknown_চাবি;
                যদি(pKey16!=nullptr) {
                    int32_t চাবি_Offset=ds->readUInt16(pKey16[i]);
                    যদি(চাবি_Offset<pTempTable->local_চাবি_Limit) {
                        item_চাবি = reinterpret_cast<ধ্রুব বর্ণ*>(outBundle) + চাবি_Offset;
                    }
                } অন্যথা {
                    int32_t চাবি_Offset=udata_readInt32(ds, pKey32[i]);
                    যদি(চাবি_Offset>=0) {
                        item_চাবি = reinterpret_cast<ধ্রুব বর্ণ*>(outBundle) + চাবি_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_চাবি, pTempTable, pErrorCode);
                যদি(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    প্রত্যাগম;
                }
            }

            যদি(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                যদি(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } অন্যথা {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                ভঙ্গ;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            যদি(pKey16!=nullptr) {
                প্রতি(i=0; i<count; ++i) {
                    pTempTable->rows[i].চাবি_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } অন্যথা {
                প্রতি(i=0; i<count; ++i) {
                    pTempTable->rows[i].চাবি_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, পরিমাণ(Row),
                           ures_compareRows, pTempTable->চাবি_Chars,
                           অসত্য, pErrorCode);
            যদি(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                প্রত্যাগম;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*চাবিগুলি*/
            যদি(pKey16!=nullptr) {
                uint16_t *rKey16;

                যদি(pKey16!=qKey16) {
                    rKey16=qKey16;
                } অন্যথা {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                প্রতি(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                যদি(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } অন্যথা {
                int32_t *rKey32;

                যদি(pKey32!=qKey32) {
                    rKey32=qKey32;
                } অন্যথা {
                    rKey32=pTempTable->resort;
                }
                প্রতি(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                যদি(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                যদি(p!=q) {
                    r=q;
                } অন্যথা {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                প্রতি(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                যদি(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        ভঙ্গ;
    পক্ষ URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            প্রতি(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                যদি(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    প্রত্যাগম;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        ভঙ্গ;
    পক্ষ URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        ভঙ্গ;
    স্বভাব:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ভঙ্গ;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(ধ্রুব UDataSwapper *ds,
          ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
          UErrorCode *pErrorCode) {
    ধ্রুব UDataInfo *pInfo;
    ধ্রুব Resource *inBundle;
    Resource মূল_Res;
    int32_t headerSize, maxTableLength;

    Row rows[স্তূপ_ROW_CAPACITY];
    int32_t resort[স্তূপ_ROW_CAPACITY];
    TempTable tempTable;

    ধ্রুব int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, চাবিগুলি_Bottom, চাবিগুলি_শীর্ষ, resBottom, শীর্ষ;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুব UDataInfo *)((ধ্রুব বর্ণ *)inData+4);
    যদি(!(
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
        প্রত্যাগম 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    যদি(length<0) {
        bundleLength=-1;
    } অন্যথা {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        যদি(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }
    }

    inBundle=(ধ্রুব Resource *)((ধ্রুব বর্ণ *)inData+headerSize);
    মূল_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(ধ্রুব int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    যদি(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
    }
    চাবিগুলি_Bottom=1+indexLength;
    চাবিগুলি_শীর্ষ=udata_readInt32(ds, inIndexes[URES_INDEX_চাবিগুলি_শীর্ষ]);
    যদি(indexLength>URES_INDEX_16_BIT_শীর্ষ) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_শীর্ষ]);
    } অন্যথা {
        resBottom=চাবিগুলি_শীর্ষ;
    }
    শীর্ষ=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_শীর্ষ]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    যদি(0<=bundleLength && bundleLength<শীর্ষ) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         শীর্ষ, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
    }
    যদি(চাবিগুলি_শীর্ষ>(1+indexLength)) {
        tempTable.local_চাবি_Limit=চাবিগুলি_শীর্ষ<<2;
    } অন্যথা {
        tempTable.local_চাবি_Limit=0;
    }

    যদি(length>=0) {
        Resource *outBundle=(Resource *)((বর্ণ *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t স্তূপ_Res_Flags[স্তূপ_ROW_CAPACITY];
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
        যদি(resFlagsLength<=(int32_t)পরিমাণ(স্তূপ_Res_Flags)) {
            tempTable.resFlags=স্তূপ_Res_Flags;
        } অন্যথা {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            যদি(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        যদি(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*শীর্ষ);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+চাবিগুলি_Bottom, 4*(চাবিগুলি_শীর্ষ-চাবিগুলি_Bottom),
                                    outBundle+চাবিগুলি_Bottom, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(চাবিগুলি_শীর্ষ-চাবিগুলি_Bottom));
            যদি(tempTable.resFlags!=স্তূপ_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            প্রত্যাগম 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        যদি(চাবিগুলি_শীর্ষ<resBottom) {
            ds->swapArray16(ds, inBundle+চাবিগুলি_শীর্ষ, (resBottom-চাবিগুলি_শীর্ষ)*4, outBundle+চাবিগুলি_শীর্ষ, pErrorCode);
            যদি(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-চাবিগুলি_শীর্ষ));
                যদি(tempTable.resFlags!=স্তূপ_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                প্রত্যাগম 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.চাবি_Chars=(ধ্রুব বর্ণ *)outBundle; /* sort by outCharset */
        যদি(tempTable.majorFormatVersion>1 || maxTableLength<=স্তূপ_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } অন্যথা {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*পরিমাণ(Row)+maxTableLength*4);
            যদি(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                যদি(tempTable.resFlags!=স্তূপ_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                প্রত্যাগম 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, মূল_Res, nullptr, &tempTable, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             মূল_Res);
        }

        যদি(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        যদি(tempTable.resFlags!=স্তূপ_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, চাবিগুলি_Bottom*4, outBundle, pErrorCode);
    }

    প্রত্যাগম headerSize+4*শীর্ষ;
}

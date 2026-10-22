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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "resource.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucol_swp.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "uresdata.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, किल्ली_Offset) \
    ((किल्ली_Offset)<(pResData)->local_किल्ली_Limit ? \
        (ध्रुव वर्ण *)(pResData)->p_मूळ+(किल्ली_Offset) : \
        (pResData)->pool_Bundle_किल्ल्या+(किल्ली_Offset)-(pResData)->local_किल्ली_Limit)

#define RES_GET_KEY32(pResData, किल्ली_Offset) \
    ((किल्ली_Offset)>=0 ? \
        (ध्रुव वर्ण *)(pResData)->p_मूळ+(किल्ली_Offset) : \
        (pResData)->pool_Bundle_किल्ल्या+((किल्ली_Offset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
स्थिर ध्रुव uint16_t gEmpty16=0;

स्थिर ध्रुव संरचना {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

स्थिर ध्रुव संरचना {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

स्थिर int32_t
_res_findTableItem(ध्रुव ResourceData *pResData, ध्रुव uint16_t *किल्ली_Offsets, int32_t length,
                   ध्रुव वर्ण *किल्ली, ध्रुव वर्ण **real_किल्ली) {
    ध्रुव वर्ण *table_किल्ली;
    int32_t mid, start, limit;
    पूर्णाङ्क result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    यावत्(start<limit) {
        mid = (start + limit) / 2;
        table_किल्ली = RES_GET_KEY16(pResData, किल्ली_Offsets[mid]);
        यदि (pResData->useNativeStrcmp) {
            result = uprv_strcmp(किल्ली, table_किल्ली);
        } अन्यथा {
            result = uprv_compareInvCharsAsAscii(किल्ली, table_किल्ली);
        }
        यदि (result < 0) {
            limit = mid;
        } अन्यथा यदि (result > 0) {
            start = mid + 1;
        } अन्यथा {
            /* We found it! */
            *real_किल्ली=table_किल्ली;
            प्रत्यागम mid;
        }
    }
    प्रत्यागम URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

स्थिर int32_t
_res_findTable32Item(ध्रुव ResourceData *pResData, ध्रुव int32_t *किल्ली_Offsets, int32_t length,
                     ध्रुव वर्ण *किल्ली, ध्रुव वर्ण **real_किल्ली) {
    ध्रुव वर्ण *table_किल्ली;
    int32_t mid, start, limit;
    पूर्णाङ्क result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    यावत्(start<limit) {
        mid = (start + limit) / 2;
        table_किल्ली = RES_GET_KEY32(pResData, किल्ली_Offsets[mid]);
        यदि (pResData->useNativeStrcmp) {
            result = uprv_strcmp(किल्ली, table_किल्ली);
        } अन्यथा {
            result = uprv_compareInvCharsAsAscii(किल्ली, table_किल्ली);
        }
        यदि (result < 0) {
            limit = mid;
        } अन्यथा यदि (result > 0) {
            start = mid + 1;
        } अन्यथा {
            /* We found it! */
            *real_किल्ली=table_किल्ली;
            प्रत्यागम mid;
        }
    }
    प्रत्यागम URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

स्थिर UBool U_CALLCONV
isAcceptable(शून्य *context,
             ध्रुव वर्ण * /*type*/, ध्रुव वर्ण * /*name*/,
             ध्रुव UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    प्रत्यागम
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

स्थिर शून्य
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, ध्रुव शून्य *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType मूळ_Type;

    /* get the root resource */
    pResData->p_मूळ = static_cast<ध्रुव int32_t*>(inBytes);
    pResData->मूळ_Res = static_cast<Resource>(*pResData->p_मूळ);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    यदि(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        प्रत्यागम;
    }

    /* currently, we accept only resources that have a Table as their roots */
    मूळ_Type = static_cast<UResType>(RES_GET_TYPE(pResData->मूळ_Res));
    यदि(!URES_IS_TABLE(मूळ_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        प्रत्यागम;
    }

    यदि(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_किल्ली_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } अन्यथा {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ध्रुव int32_t *indexes=pResData->p_मूळ+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        यदि(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            प्रत्यागम;
        }
        यदि( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_शीर्ष]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            प्रत्यागम;
        }
        यदि(indexes[URES_INDEX_किल्ल्या_शीर्ष]>(1+indexLength)) {
            pResData->local_किल्ली_Limit=indexes[URES_INDEX_किल्ल्या_शीर्ष]<<2;
        }
        यदि(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        यदि(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        यदि((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            प्रत्यागम;
        }
        यदि( indexLength>URES_INDEX_16_BIT_शीर्ष &&
            indexes[URES_INDEX_16_BIT_शीर्ष]>indexes[URES_INDEX_किल्ल्या_शीर्ष]
        ) {
            pResData->p16BitUnits = reinterpret_cast<ध्रुव uint16_t*>(pResData->p_मूळ + indexes[URES_INDEX_किल्ल्या_शीर्ष]);
        }
    }

    यदि(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=सत्य;
    }
}

U_CAPI शून्य U_EXPORT2
res_read(ResourceData *pResData,
         ध्रुव UDataInfo *pInfo, ध्रुव शून्य *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, परिमाण(ResourceData));
    यदि(U_FAILURE(*errorCode)) {
        प्रत्यागम;
    }
    यदि(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        प्रत्यागम;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC शून्य
res_load(ResourceData *pResData,
         ध्रुव वर्ण *path, ध्रुव वर्ण *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, परिमाण(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    यदि(U_FAILURE(*errorCode)) {
        प्रत्यागम;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC शून्य
res_unload(ResourceData *pResData) {
    यदि(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

स्थिर ध्रुव int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_द्विमान,
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
    प्रत्यागम (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI ध्रुव char16_t * U_EXPORT2
res_getStringNoTrace(ध्रुव ResourceData *pResData, Resource res, int32_t *pLength) {
    ध्रुव char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    यदि(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        यदि((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(ध्रुव char16_t *)pResData->poolBundleStrings+offset;
        } अन्यथा {
            p=(ध्रुव char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        यदि(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } अन्यथा यदि(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } अन्यथा यदि(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } अन्यथा {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } अन्यथा यदि(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        ध्रुव int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_मूळ+res;
        length=*p32++;
        p=(ध्रुव char16_t *)p32;
    } अन्यथा {
        p=nullptr;
        length=0;
    }
    यदि(pLength) {
        *pLength=length;
    }
    प्रत्यागम p;
}

नामाकाश {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ध्रुव ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    यदि (offset == 0) {
        // empty string
    } अन्यथा यदि (res == offset) {
        ध्रुव int32_t *p32=pResData->p_मूळ+res;
        int32_t length=*p32;
        ध्रुव char16_t* p = reinterpret_cast<ध्रुव char16_t*>(p32);
        प्रत्यागम length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } अन्यथा यदि (RES_GET_TYPE(res) == URES_STRING_V2) {
        ध्रुव char16_t *p;
        यदि (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<ध्रुव char16_t*>(pResData->poolBundleStrings) + offset;
        } अन्यथा {
            p = reinterpret_cast<ध्रुव char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        यदि (first == 0x2205) {  // implicit length
            प्रत्यागम p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } अन्यथा यदि (first == 0xdc03) {  // explicit length 3 (should not occur)
            प्रत्यागम p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } अन्यथा {
            // Assume that the string has not been stored with more length units than necessary.
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम असत्य;
}

int32_t getStringArray(ध्रुव ResourceData *pResData, ध्रुव icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम 0;
    }
    यदि(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    int32_t length = array.getSize();
    यदि(length == 0) {
        प्रत्यागम 0;
    }
    यदि(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        प्रत्यागम length;
    }
    प्रति(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        ध्रुव char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        यदि(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            प्रत्यागम 0;
        }
        dest[i].setTo(सत्य, s, sLength);
    }
    प्रत्यागम length;
}

}  // namespace

U_CAPI ध्रुव char16_t * U_EXPORT2
res_getAlias(ध्रुव ResourceData *pResData, Resource res, int32_t *pLength) {
    ध्रुव char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    यदि(RES_GET_TYPE(res)==URES_ALIAS) {
        ध्रुव int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_मूळ+offset;
        length=*p32++;
        p=(ध्रुव char16_t *)p32;
    } अन्यथा {
        p=nullptr;
        length=0;
    }
    यदि(pLength) {
        *pLength=length;
    }
    प्रत्यागम p;
}

U_CAPI ध्रुव uint8_t * U_EXPORT2
res_get_द्विमान_No_Trace(ध्रुव ResourceData *pResData, Resource res, int32_t *pLength) {
    ध्रुव uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    यदि(RES_GET_TYPE(res)==URES_द्विमान) {
        ध्रुव int32_t *p32= offset==0 ? (ध्रुव int32_t*)&gEmpty32 : pResData->p_मूळ+offset;
        length=*p32++;
        p=(ध्रुव uint8_t *)p32;
    } अन्यथा {
        p=nullptr;
        length=0;
    }
    यदि(pLength) {
        *pLength=length;
    }
    प्रत्यागम p;
}


U_CAPI ध्रुव int32_t * U_EXPORT2
res_getIntVectorNoTrace(ध्रुव ResourceData *pResData, Resource res, int32_t *pLength) {
    ध्रुव int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    यदि(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (ध्रुव int32_t *)&gEmpty32 : pResData->p_मूळ+offset;
        length=*p++;
    } अन्यथा {
        p=nullptr;
        length=0;
    }
    यदि(pLength) {
        *pLength=length;
    }
    प्रत्यागम p;
}

U_CAPI int32_t U_EXPORT2
res_मोजणी_Array_वस्तुसंच(ध्रुव ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_STRING:
    पक्ष URES_STRING_V2:
    पक्ष URES_द्विमान:
    पक्ष URES_ALIAS:
    पक्ष URES_INT:
    पक्ष URES_INT_VECTOR:
        प्रत्यागम 1;
    पक्ष URES_ARRAY:
    पक्ष URES_TABLE32:
        प्रत्यागम offset==0 ? 0 : *(pResData->p_मूळ+offset);
    पक्ष URES_TABLE:
        प्रत्यागम offset==0 ? 0 : *((ध्रुव uint16_t *)(pResData->p_मूळ+offset));
    पक्ष URES_ARRAY16:
    पक्ष URES_TABLE16:
        प्रत्यागम pResData->p16BitUnits[offset];
    स्वभाव:
        प्रत्यागम 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_मूल्य::~Resource_Data_मूल्य() {}

UResType Resource_Data_मूल्य::getType() ध्रुव {
    प्रत्यागम res_getPublicType(res);
}

ध्रुव char16_t *Resource_Data_मूल्य::getString(int32_t &length, UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    ध्रुव char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    यदि(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम s;
}

ध्रुव char16_t *Resource_Data_मूल्य::getAliasString(int32_t &length, UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    ध्रुव char16_t *s = res_getAlias(&getData(), res, &length);
    यदि(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम s;
}

int32_t Resource_Data_मूल्य::getInt(UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम 0;
    }
    यदि(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_मूल्य::getUInt(UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम 0;
    }
    यदि(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम res_getUInt(fTraceInfo, res);
}

ध्रुव int32_t *Resource_Data_मूल्य::getIntVector(int32_t &length, UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    ध्रुव int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    यदि(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम iv;
}

ध्रुव uint8_t *Resource_Data_मूल्य::get_द्विमान(int32_t &length, UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    ध्रुव uint8_t *b = res_get_द्विमान(fTraceInfo, &getData(), res, &length);
    यदि(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम b;
}

ResourceArray Resource_Data_मूल्य::getArray(UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम {};
    }
    ध्रुव uint16_t *items16 = nullptr;
    ध्रुव Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_ARRAY:
        यदि (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ध्रुव Resource*>(getData().p_मूळ) + offset;
            length = *items32++;
        }
        भङ्ग;
    पक्ष URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        भङ्ग;
    स्वभाव:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        प्रत्यागम {};
    }
    प्रत्यागम ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_Data_मूल्य::getTable(UErrorCode &errorCode) ध्रुव {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम {};
    }
    ध्रुव uint16_t *keys16 = nullptr;
    ध्रुव int32_t *keys32 = nullptr;
    ध्रुव uint16_t *items16 = nullptr;
    ध्रुव Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_TABLE:
        यदि (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ध्रुव uint16_t*>(getData().p_मूळ + offset);
            length = *keys16++;
            items32 = reinterpret_cast<ध्रुव Resource*>(keys16 + length + (~length & 1));
        }
        भङ्ग;
    पक्ष URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        भङ्ग;
    पक्ष URES_TABLE32:
        यदि (offset != 0) {  // empty if offset==0
            keys32 = getData().p_मूळ+offset;
            length = *keys32++;
            items32 = reinterpret_cast<ध्रुव Resource*>(keys32) + length;
        }
        भङ्ग;
    स्वभाव:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        प्रत्यागम {};
    }
    प्रत्यागम ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_Data_मूल्य::isNoInheritanceMarker() ध्रुव {
    प्रत्यागम ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_मूल्य::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) ध्रुव {
    प्रत्यागम ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_मूल्य::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) ध्रुव {
    यदि(URES_IS_ARRAY(res)) {
        प्रत्यागम ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम 0;
    }
    यदि(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    यदि(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        प्रत्यागम 1;
    }
    int32_t sLength;
    ध्रुव char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    यदि(s != nullptr) {
        dest[0].setTo(सत्य, s, sLength);
        प्रत्यागम 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    प्रत्यागम 0;
}

UnicodeString Resource_Data_मूल्य::getStringOrFirstOfArray(UErrorCode &errorCode) ध्रुव {
    UnicodeString us;
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम us;
    }
    int32_t sLength;
    ध्रुव char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    यदि(s != nullptr) {
        us.setTo(सत्य, s, sLength);
        प्रत्यागम us;
    }
    ResourceArray array = getArray(errorCode);
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम us;
    }
    यदि(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        यदि(s != nullptr) {
            us.setTo(सत्य, s, sLength);
            प्रत्यागम us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    प्रत्यागम us;
}

U_NAMESPACE_END

स्थिर Resource
makeResourceFrom16(ध्रुव ResourceData *pResData, int32_t res16) {
    यदि(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } अन्यथा {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    प्रत्यागम URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_किल्ली(ध्रुव ResourceData *pResData, Resource table,
                      int32_t *indexR, ध्रुव वर्ण **किल्ली) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    यदि(किल्ली == nullptr || *किल्ली == nullptr) {
        प्रत्यागम RES_BOGUS;
    }
    विकल्प(RES_GET_TYPE(table)) {
    पक्ष URES_TABLE: {
        यदि (offset!=0) { /* empty if offset==0 */
            ध्रुव uint16_t *p= (ध्रुव uint16_t *)(pResData->p_मूळ+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *किल्ली, किल्ली);
            यदि(idx>=0) {
                ध्रुव Resource *p32=(ध्रुव Resource *)(p+length+(~length&1));
                प्रत्यागम p32[idx];
            }
        }
        भङ्ग;
    }
    पक्ष URES_TABLE16: {
        ध्रुव uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *किल्ली, किल्ली);
        यदि(idx>=0) {
            प्रत्यागम makeResourceFrom16(pResData, p[length+idx]);
        }
        भङ्ग;
    }
    पक्ष URES_TABLE32: {
        यदि (offset!=0) { /* empty if offset==0 */
            ध्रुव int32_t *p= pResData->p_मूळ+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *किल्ली, किल्ली);
            यदि(idx>=0) {
                प्रत्यागम (Resource)p[length+idx];
            }
        }
        भङ्ग;
    }
    स्वभाव:
        भङ्ग;
    }
    प्रत्यागम RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(ध्रुव ResourceData *pResData, Resource table,
                        int32_t indexR, ध्रुव वर्ण **किल्ली) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    यदि (indexR < 0) {
        प्रत्यागम RES_BOGUS;
    }
    विकल्प(RES_GET_TYPE(table)) {
    पक्ष URES_TABLE: {
        यदि (offset != 0) { /* empty if offset==0 */
            ध्रुव uint16_t *p= (ध्रुव uint16_t *)(pResData->p_मूळ+offset);
            length=*p++;
            यदि(indexR<length) {
                ध्रुव Resource *p32=(ध्रुव Resource *)(p+length+(~length&1));
                यदि(किल्ली!=nullptr) {
                    *किल्ली=RES_GET_KEY16(pResData, p[indexR]);
                }
                प्रत्यागम p32[indexR];
            }
        }
        भङ्ग;
    }
    पक्ष URES_TABLE16: {
        ध्रुव uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        यदि(indexR<length) {
            यदि(किल्ली!=nullptr) {
                *किल्ली=RES_GET_KEY16(pResData, p[indexR]);
            }
            प्रत्यागम makeResourceFrom16(pResData, p[length+indexR]);
        }
        भङ्ग;
    }
    पक्ष URES_TABLE32: {
        यदि (offset != 0) { /* empty if offset==0 */
            ध्रुव int32_t *p= pResData->p_मूळ+offset;
            length=*p++;
            यदि(indexR<length) {
                यदि(किल्ली!=nullptr) {
                    *किल्ली=RES_GET_KEY32(pResData, p[indexR]);
                }
                प्रत्यागम (Resource)p[length+indexR];
            }
        }
        भङ्ग;
    }
    स्वभाव:
        भङ्ग;
    }
    प्रत्यागम RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ध्रुव ResourceData *pResData, ध्रुव वर्ण *किल्ली) {
    ध्रुव वर्ण *real_किल्ली=किल्ली;
    int32_t idx;
    प्रत्यागम res_get_Table_Item_By_किल्ली(pResData, pResData->मूळ_Res, &idx, &real_किल्ली);
}


UBool icu::ResourceTable::get_किल्ली_And_मूल्य(int32_t i,
                                         ध्रुव वर्ण *&किल्ली, icu::Resource_मूल्य &value) ध्रुव {
    यदि(0 <= i && i < length) {
        icu::Resource_Data_मूल्य &rd_मूल्य = static_cast<icu::Resource_Data_मूल्य &>(value);
        यदि (keys16 != nullptr) {
            किल्ली = RES_GET_KEY16(&rd_मूल्य.getData(), keys16[i]);
        } अन्यथा {
            किल्ली = RES_GET_KEY32(&rd_मूल्य.getData(), keys32[i]);
        }
        Resource res;
        यदि (items16 != nullptr) {
            res = makeResourceFrom16(&rd_मूल्य.getData(), items16[i]);
        } अन्यथा {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_मूल्य.setResource(res, ResourceTracer(fTraceInfo, किल्ली));
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

UBool icu::ResourceTable::find_मूल्य(ध्रुव वर्ण *किल्ली, Resource_मूल्य &value) ध्रुव {
    icu::Resource_Data_मूल्य &rd_मूल्य = static_cast<icu::Resource_Data_मूल्य &>(value);
    ध्रुव वर्ण *real_किल्ली = nullptr;
    int32_t i;
    यदि (keys16 != nullptr) {
        i = _res_findTableItem(&rd_मूल्य.getData(), keys16, length, किल्ली, &real_किल्ली);
    } अन्यथा {
        i = _res_findTable32Item(&rd_मूल्य.getData(), keys32, length, किल्ली, &real_किल्ली);
    }
    यदि (i >= 0) {
        Resource res;
        यदि (items16 != nullptr) {
            res = makeResourceFrom16(&rd_मूल्य.getData(), items16[i]);
        } अन्यथा {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_मूल्य.setResource(res, ResourceTracer(fTraceInfo, किल्ली));
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ध्रुव ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    यदि (indexR < 0) {
        प्रत्यागम RES_BOGUS;
    }
    विकल्प(RES_GET_TYPE(array)) {
    पक्ष URES_ARRAY: {
        यदि (offset!=0) { /* empty if offset==0 */
            ध्रुव int32_t *p= pResData->p_मूळ+offset;
            यदि(indexR<*p) {
                प्रत्यागम (Resource)p[1+indexR];
            }
        }
        भङ्ग;
    }
    पक्ष URES_ARRAY16: {
        ध्रुव uint16_t *p=pResData->p16BitUnits+offset;
        यदि(indexR<*p) {
            प्रत्यागम makeResourceFrom16(pResData, p[1+indexR]);
        }
        भङ्ग;
    }
    स्वभाव:
        भङ्ग;
    }
    प्रत्यागम RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(ध्रुव ResourceData *pResData, int32_t i) ध्रुव {
    यदि (items16 != nullptr) {
        प्रत्यागम makeResourceFrom16(pResData, items16[i]);
    } अन्यथा {
        प्रत्यागम items32[i];
    }
}

UBool icu::ResourceArray::get_मूल्य(int32_t i, icu::Resource_मूल्य &value) ध्रुव {
    यदि(0 <= i && i < length) {
        icu::Resource_Data_मूल्य &rd_मूल्य = static_cast<icu::Resource_Data_मूल्य &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_मूल्य.setResource(
            internalGetResource(&rd_मूल्य.getData(), i),
            ResourceTracer(fTraceInfo, i));
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

U_CFUNC Resource
res_findResource(ध्रुव ResourceData *pResData, Resource r, वर्ण** path, ध्रुव वर्ण** किल्ली) {
  वर्ण *pathP = *path, *nextSepP = *path;
  वर्ण *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  यदि(!uprv_strlen(pathP)) {
      प्रत्यागम r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  यदि(!URES_IS_CONTAINER(type)) {
      प्रत्यागम RES_BOGUS;
  }
  
  यावत्(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    यदि(nextSepP != nullptr) {
      यदि(nextSepP == pathP) {
        // Empty key string.
        प्रत्यागम RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } अन्यथा {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    यदि(URES_IS_TABLE(type)) {
      *किल्ली = pathP;
      t2 = res_get_Table_Item_By_किल्ली(pResData, t1, &indexR, किल्ली);
    } अन्यथा यदि(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      यदि(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } अन्यथा {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *किल्ली = nullptr;
    } अन्यथा { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  प्रत्यागम t1;
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
प्रकारनिर्देश संरचना Row {
    int32_t किल्ली_Index, sortIndex;
} Row;

स्थिर int32_t U_CALLCONV
ures_compareRows(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *उजवा) {
    ध्रुव वर्ण* किल्ली_Chars = static_cast<ध्रुव वर्ण*>(context);
    प्रत्यागम static_cast<int32_t>(uprv_strcmp(किल्ली_Chars + static_cast<ध्रुव Row*>(left)->किल्ली_Index,
                                            किल्ली_Chars + static_cast<ध्रुव Row*>(उजवा)->किल्ली_Index));
}

प्रकारनिर्देश संरचना TempTable {
    ध्रुव वर्ण *किल्ली_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_किल्ली_Limit;
    uint8_t majorFormatVersion;
} TempTable;

गणना {
    चळत_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
स्थिर ध्रुव वर्ण *ध्रुव g_Unknown_किल्ली="";

#यदि !UCONFIG_NO_COLLATION
// resource table key for collation binaries
स्थिर ध्रुव char16_t g_Collation_Bin_किल्ली[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
स्थिर शून्य
ures_swapResource(ध्रुव UDataSwapper *ds,
                  ध्रुव Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  ध्रुव वर्ण *किल्ली,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    ध्रुव Resource *p;
    Resource *q;
    int32_t offset, count;

    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_TABLE16:
    पक्ष URES_STRING_V2:
    पक्ष URES_INT:
    पक्ष URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        प्रत्यागम;
    स्वभाव:
        भङ्ग;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    यदि(offset==0) {
        /* special offset indicating an empty item */
        प्रत्यागम;
    }
    यदि (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        प्रत्यागम;
    } अन्यथा {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    पक्ष URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        भङ्ग;
    पक्ष URES_द्विमान:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#यदि !UCONFIG_NO_COLLATION
        यदि( किल्ली!=nullptr &&  /* the binary is in a table */
            (किल्ली!=g_Unknown_किल्ली ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, किल्ली, -1,
                                       g_Collation_Bin_किल्ली, UPRV_LENGTHOF(g_Collation_Bin_किल्ली)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_द्विमान(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#endif
        भङ्ग;
    पक्ष URES_TABLE:
    पक्ष URES_TABLE32:
        {
            ध्रुव uint16_t *pKey16;
            uint16_t *qKey16;

            ध्रुव int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            यदि(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<ध्रुव uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } अन्यथा {
                /* get table item count */
                pKey32 = reinterpret_cast<ध्रुव int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            यदि(count==0) {
                भङ्ग;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            प्रति(i=0; i<count; ++i) {
                ध्रुव वर्ण *item_किल्ली=g_Unknown_किल्ली;
                यदि(pKey16!=nullptr) {
                    int32_t किल्ली_Offset=ds->readUInt16(pKey16[i]);
                    यदि(किल्ली_Offset<pTempTable->local_किल्ली_Limit) {
                        item_किल्ली = reinterpret_cast<ध्रुव वर्ण*>(outBundle) + किल्ली_Offset;
                    }
                } अन्यथा {
                    int32_t किल्ली_Offset=udata_readInt32(ds, pKey32[i]);
                    यदि(किल्ली_Offset>=0) {
                        item_किल्ली = reinterpret_cast<ध्रुव वर्ण*>(outBundle) + किल्ली_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_किल्ली, pTempTable, pErrorCode);
                यदि(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    प्रत्यागम;
                }
            }

            यदि(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                यदि(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } अन्यथा {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                भङ्ग;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            यदि(pKey16!=nullptr) {
                प्रति(i=0; i<count; ++i) {
                    pTempTable->rows[i].किल्ली_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } अन्यथा {
                प्रति(i=0; i<count; ++i) {
                    pTempTable->rows[i].किल्ली_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, परिमाण(Row),
                           ures_compareRows, pTempTable->किल्ली_Chars,
                           असत्य, pErrorCode);
            यदि(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                प्रत्यागम;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*किल्ल्या*/
            यदि(pKey16!=nullptr) {
                uint16_t *rKey16;

                यदि(pKey16!=qKey16) {
                    rKey16=qKey16;
                } अन्यथा {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                प्रति(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                यदि(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } अन्यथा {
                int32_t *rKey32;

                यदि(pKey32!=qKey32) {
                    rKey32=qKey32;
                } अन्यथा {
                    rKey32=pTempTable->resort;
                }
                प्रति(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                यदि(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                यदि(p!=q) {
                    r=q;
                } अन्यथा {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                प्रति(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                यदि(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        भङ्ग;
    पक्ष URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            प्रति(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                यदि(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    प्रत्यागम;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        भङ्ग;
    पक्ष URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        भङ्ग;
    स्वभाव:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        भङ्ग;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(ध्रुव UDataSwapper *ds,
          ध्रुव शून्य *inData, int32_t length, शून्य *outData,
          UErrorCode *pErrorCode) {
    ध्रुव UDataInfo *pInfo;
    ध्रुव Resource *inBundle;
    Resource मूळ_Res;
    int32_t headerSize, maxTableLength;

    Row rows[चळत_ROW_CAPACITY];
    int32_t resort[चळत_ROW_CAPACITY];
    TempTable tempTable;

    ध्रुव int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, किल्ल्या_Bottom, किल्ल्या_शीर्ष, resBottom, शीर्ष;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    यदि(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)inData+4);
    यदि(!(
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
        प्रत्यागम 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    यदि(length<0) {
        bundleLength=-1;
    } अन्यथा {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        यदि(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    inBundle=(ध्रुव Resource *)((ध्रुव वर्ण *)inData+headerSize);
    मूळ_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(ध्रुव int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    यदि(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    किल्ल्या_Bottom=1+indexLength;
    किल्ल्या_शीर्ष=udata_readInt32(ds, inIndexes[URES_INDEX_किल्ल्या_शीर्ष]);
    यदि(indexLength>URES_INDEX_16_BIT_शीर्ष) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_शीर्ष]);
    } अन्यथा {
        resBottom=किल्ल्या_शीर्ष;
    }
    शीर्ष=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_शीर्ष]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    यदि(0<=bundleLength && bundleLength<शीर्ष) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         शीर्ष, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    यदि(किल्ल्या_शीर्ष>(1+indexLength)) {
        tempTable.local_किल्ली_Limit=किल्ल्या_शीर्ष<<2;
    } अन्यथा {
        tempTable.local_किल्ली_Limit=0;
    }

    यदि(length>=0) {
        Resource *outBundle=(Resource *)((वर्ण *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t चळत_Res_Flags[चळत_ROW_CAPACITY];
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
        यदि(resFlagsLength<=(int32_t)परिमाण(चळत_Res_Flags)) {
            tempTable.resFlags=चळत_Res_Flags;
        } अन्यथा {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            यदि(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        यदि(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*शीर्ष);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+किल्ल्या_Bottom, 4*(किल्ल्या_शीर्ष-किल्ल्या_Bottom),
                                    outBundle+किल्ल्या_Bottom, pErrorCode);
        यदि(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(किल्ल्या_शीर्ष-किल्ल्या_Bottom));
            यदि(tempTable.resFlags!=चळत_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            प्रत्यागम 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        यदि(किल्ल्या_शीर्ष<resBottom) {
            ds->swapArray16(ds, inBundle+किल्ल्या_शीर्ष, (resBottom-किल्ल्या_शीर्ष)*4, outBundle+किल्ल्या_शीर्ष, pErrorCode);
            यदि(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-किल्ल्या_शीर्ष));
                यदि(tempTable.resFlags!=चळत_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                प्रत्यागम 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.किल्ली_Chars=(ध्रुव वर्ण *)outBundle; /* sort by outCharset */
        यदि(tempTable.majorFormatVersion>1 || maxTableLength<=चळत_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } अन्यथा {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*परिमाण(Row)+maxTableLength*4);
            यदि(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                यदि(tempTable.resFlags!=चळत_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                प्रत्यागम 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, मूळ_Res, nullptr, &tempTable, pErrorCode);
        यदि(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             मूळ_Res);
        }

        यदि(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        यदि(tempTable.resFlags!=चळत_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, किल्ल्या_Bottom*4, outBundle, pErrorCode);
    }

    प्रत्यागम headerSize+4*शीर्ष;
}

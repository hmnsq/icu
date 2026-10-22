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
#define RES_GET_KEY16(pResData, कुञ्चिका_Offset) \
    ((कुञ्चिका_Offset)<(pResData)->local_कुञ्चिका_Limit ? \
        (ध्रुव वर्ण *)(pResData)->p_मूलम्+(कुञ्चिका_Offset) : \
        (pResData)->pool_Bundle_कुञ्चिकाः+(कुञ्चिका_Offset)-(pResData)->local_कुञ्चिका_Limit)

#define RES_GET_KEY32(pResData, कुञ्चिका_Offset) \
    ((कुञ्चिका_Offset)>=0 ? \
        (ध्रुव वर्ण *)(pResData)->p_मूलम्+(कुञ्चिका_Offset) : \
        (pResData)->pool_Bundle_कुञ्चिकाः+((कुञ्चिका_Offset)&0x7fffffff))

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
_res_findTableItem(ध्रुव ResourceData *pResData, ध्रुव uint16_t *कुञ्चिका_Offsets, int32_t length,
                   ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण **real_कुञ्चिका) {
    ध्रुव वर्ण *table_कुञ्चिका;
    int32_t mid, start, limit;
    पूर्णाङ्क result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    याव(start<limit) {
        mid = (start + limit) / 2;
        table_कुञ्चिका = RES_GET_KEY16(pResData, कुञ्चिका_Offsets[mid]);
        सचे (pResData->useNativeStrcmp) {
            result = uprv_strcmp(कुञ्चिका, table_कुञ्चिका);
        } अञ्ञथा {
            result = uprv_compareInvCharsAsAscii(कुञ्चिका, table_कुञ्चिका);
        }
        सचे (result < 0) {
            limit = mid;
        } अञ्ञथा सचे (result > 0) {
            start = mid + 1;
        } अञ्ञथा {
            /* We found it! */
            *real_कुञ्चिका=table_कुञ्चिका;
            पटिनिवत्त mid;
        }
    }
    पटिनिवत्त URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

स्थिर int32_t
_res_findTable32Item(ध्रुव ResourceData *pResData, ध्रुव int32_t *कुञ्चिका_Offsets, int32_t length,
                     ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण **real_कुञ्चिका) {
    ध्रुव वर्ण *table_कुञ्चिका;
    int32_t mid, start, limit;
    पूर्णाङ्क result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    याव(start<limit) {
        mid = (start + limit) / 2;
        table_कुञ्चिका = RES_GET_KEY32(pResData, कुञ्चिका_Offsets[mid]);
        सचे (pResData->useNativeStrcmp) {
            result = uprv_strcmp(कुञ्चिका, table_कुञ्चिका);
        } अञ्ञथा {
            result = uprv_compareInvCharsAsAscii(कुञ्चिका, table_कुञ्चिका);
        }
        सचे (result < 0) {
            limit = mid;
        } अञ्ञथा सचे (result > 0) {
            start = mid + 1;
        } अञ्ञथा {
            /* We found it! */
            *real_कुञ्चिका=table_कुञ्चिका;
            पटिनिवत्त mid;
        }
    }
    पटिनिवत्त URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

स्थिर UBool U_CALLCONV
isAcceptable(शून्य *context,
             ध्रुव वर्ण * /*type*/, ध्रुव वर्ण * /*name*/,
             ध्रुव UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    पटिनिवत्त
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
    UResType मूलम्_Type;

    /* get the root resource */
    pResData->p_मूलम् = static_cast<ध्रुव int32_t*>(inBytes);
    pResData->मूलम्_Res = static_cast<Resource>(*pResData->p_मूलम्);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    सचे(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        पटिनिवत्त;
    }

    /* currently, we accept only resources that have a Table as their roots */
    मूलम्_Type = static_cast<UResType>(RES_GET_TYPE(pResData->मूलम्_Res));
    सचे(!URES_IS_TABLE(मूलम्_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        पटिनिवत्त;
    }

    सचे(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_कुञ्चिका_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } अञ्ञथा {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ध्रुव int32_t *indexes=pResData->p_मूलम्+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        सचे(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            पटिनिवत्त;
        }
        सचे( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_शिखरम्]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            पटिनिवत्त;
        }
        सचे(indexes[URES_INDEX_कुञ्चिकाः_शिखरम्]>(1+indexLength)) {
            pResData->local_कुञ्चिका_Limit=indexes[URES_INDEX_कुञ्चिकाः_शिखरम्]<<2;
        }
        सचे(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        सचे(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        सचे((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            पटिनिवत्त;
        }
        सचे( indexLength>URES_INDEX_16_BIT_शिखरम् &&
            indexes[URES_INDEX_16_BIT_शिखरम्]>indexes[URES_INDEX_कुञ्चिकाः_शिखरम्]
        ) {
            pResData->p16BitUnits = reinterpret_cast<ध्रुव uint16_t*>(pResData->p_मूलम् + indexes[URES_INDEX_कुञ्चिकाः_शिखरम्]);
        }
    }

    सचे(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=सच्च;
    }
}

U_CAPI शून्य U_EXPORT2
res_read(ResourceData *pResData,
         ध्रुव UDataInfo *pInfo, ध्रुव शून्य *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, परिमाण(ResourceData));
    सचे(U_FAILURE(*errorCode)) {
        पटिनिवत्त;
    }
    सचे(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        पटिनिवत्त;
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
    सचे(U_FAILURE(*errorCode)) {
        पटिनिवत्त;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC शून्य
res_unload(ResourceData *pResData) {
    सचे(pResData->data!=nullptr) {
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
    पटिनिवत्त (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI ध्रुव char16_t * U_EXPORT2
res_getStringNoTrace(ध्रुव ResourceData *pResData, Resource res, int32_t *pLength) {
    ध्रुव char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    सचे(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        सचे((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(ध्रुव char16_t *)pResData->poolBundleStrings+offset;
        } अञ्ञथा {
            p=(ध्रुव char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        सचे(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } अञ्ञथा सचे(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } अञ्ञथा सचे(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } अञ्ञथा {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } अञ्ञथा सचे(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        ध्रुव int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_मूलम्+res;
        length=*p32++;
        p=(ध्रुव char16_t *)p32;
    } अञ्ञथा {
        p=nullptr;
        length=0;
    }
    सचे(pLength) {
        *pLength=length;
    }
    पटिनिवत्त p;
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
    सचे (offset == 0) {
        // empty string
    } अञ्ञथा सचे (res == offset) {
        ध्रुव int32_t *p32=pResData->p_मूलम्+res;
        int32_t length=*p32;
        ध्रुव char16_t* p = reinterpret_cast<ध्रुव char16_t*>(p32);
        पटिनिवत्त length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } अञ्ञथा सचे (RES_GET_TYPE(res) == URES_STRING_V2) {
        ध्रुव char16_t *p;
        सचे (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<ध्रुव char16_t*>(pResData->poolBundleStrings) + offset;
        } अञ्ञथा {
            p = reinterpret_cast<ध्रुव char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        सचे (first == 0x2205) {  // implicit length
            पटिनिवत्त p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } अञ्ञथा सचे (first == 0xdc03) {  // explicit length 3 (should not occur)
            पटिनिवत्त p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } अञ्ञथा {
            // Assume that the string has not been stored with more length units than necessary.
            पटिनिवत्त मिच्छा;
        }
    }
    पटिनिवत्त मिच्छा;
}

int32_t getStringArray(ध्रुव ResourceData *pResData, ध्रुव icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    सचे(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }
    int32_t length = array.getSize();
    सचे(length == 0) {
        पटिनिवत्त 0;
    }
    सचे(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        पटिनिवत्त length;
    }
    अत्थाय(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        ध्रुव char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        सचे(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            पटिनिवत्त 0;
        }
        dest[i].setTo(सच्च, s, sLength);
    }
    पटिनिवत्त length;
}

}  // namespace

U_CAPI ध्रुव char16_t * U_EXPORT2
res_getAlias(ध्रुव ResourceData *pResData, Resource res, int32_t *pLength) {
    ध्रुव char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    सचे(RES_GET_TYPE(res)==URES_ALIAS) {
        ध्रुव int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_मूलम्+offset;
        length=*p32++;
        p=(ध्रुव char16_t *)p32;
    } अञ्ञथा {
        p=nullptr;
        length=0;
    }
    सचे(pLength) {
        *pLength=length;
    }
    पटिनिवत्त p;
}

U_CAPI ध्रुव uint8_t * U_EXPORT2
res_get_द्विमान_No_Trace(ध्रुव ResourceData *pResData, Resource res, int32_t *pLength) {
    ध्रुव uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    सचे(RES_GET_TYPE(res)==URES_द्विमान) {
        ध्रुव int32_t *p32= offset==0 ? (ध्रुव int32_t*)&gEmpty32 : pResData->p_मूलम्+offset;
        length=*p32++;
        p=(ध्रुव uint8_t *)p32;
    } अञ्ञथा {
        p=nullptr;
        length=0;
    }
    सचे(pLength) {
        *pLength=length;
    }
    पटिनिवत्त p;
}


U_CAPI ध्रुव int32_t * U_EXPORT2
res_getIntVectorNoTrace(ध्रुव ResourceData *pResData, Resource res, int32_t *pLength) {
    ध्रुव int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    सचे(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (ध्रुव int32_t *)&gEmpty32 : pResData->p_मूलम्+offset;
        length=*p++;
    } अञ्ञथा {
        p=nullptr;
        length=0;
    }
    सचे(pLength) {
        *pLength=length;
    }
    पटिनिवत्त p;
}

U_CAPI int32_t U_EXPORT2
res_गणनम्_Array_वस्तूनि(ध्रुव ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_STRING:
    पक्ष URES_STRING_V2:
    पक्ष URES_द्विमान:
    पक्ष URES_ALIAS:
    पक्ष URES_INT:
    पक्ष URES_INT_VECTOR:
        पटिनिवत्त 1;
    पक्ष URES_ARRAY:
    पक्ष URES_TABLE32:
        पटिनिवत्त offset==0 ? 0 : *(pResData->p_मूलम्+offset);
    पक्ष URES_TABLE:
        पटिनिवत्त offset==0 ? 0 : *((ध्रुव uint16_t *)(pResData->p_मूलम्+offset));
    पक्ष URES_ARRAY16:
    पक्ष URES_TABLE16:
        पटिनिवत्त pResData->p16BitUnits[offset];
    स्वभाव:
        पटिनिवत्त 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_मानम्::~Resource_Data_मानम्() {}

UResType Resource_Data_मानम्::getType() ध्रुव {
    पटिनिवत्त res_getPublicType(res);
}

ध्रुव char16_t *Resource_Data_मानम्::getString(int32_t &length, UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    ध्रुव char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    सचे(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    पटिनिवत्त s;
}

ध्रुव char16_t *Resource_Data_मानम्::getAliasString(int32_t &length, UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    ध्रुव char16_t *s = res_getAlias(&getData(), res, &length);
    सचे(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    पटिनिवत्त s;
}

int32_t Resource_Data_मानम्::getInt(UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    सचे(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    पटिनिवत्त res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_मानम्::getUInt(UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    सचे(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    पटिनिवत्त res_getUInt(fTraceInfo, res);
}

ध्रुव int32_t *Resource_Data_मानम्::getIntVector(int32_t &length, UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    ध्रुव int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    सचे(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    पटिनिवत्त iv;
}

ध्रुव uint8_t *Resource_Data_मानम्::get_द्विमान(int32_t &length, UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    ध्रुव uint8_t *b = res_get_द्विमान(fTraceInfo, &getData(), res, &length);
    सचे(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    पटिनिवत्त b;
}

ResourceArray Resource_Data_मानम्::getArray(UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त {};
    }
    ध्रुव uint16_t *items16 = nullptr;
    ध्रुव Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_ARRAY:
        सचे (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ध्रुव Resource*>(getData().p_मूलम्) + offset;
            length = *items32++;
        }
        भङ्ग;
    पक्ष URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        भङ्ग;
    स्वभाव:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        पटिनिवत्त {};
    }
    पटिनिवत्त ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_Data_मानम्::getTable(UErrorCode &errorCode) ध्रुव {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त {};
    }
    ध्रुव uint16_t *keys16 = nullptr;
    ध्रुव int32_t *keys32 = nullptr;
    ध्रुव uint16_t *items16 = nullptr;
    ध्रुव Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_TABLE:
        सचे (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ध्रुव uint16_t*>(getData().p_मूलम् + offset);
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
        सचे (offset != 0) {  // empty if offset==0
            keys32 = getData().p_मूलम्+offset;
            length = *keys32++;
            items32 = reinterpret_cast<ध्रुव Resource*>(keys32) + length;
        }
        भङ्ग;
    स्वभाव:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        पटिनिवत्त {};
    }
    पटिनिवत्त ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_Data_मानम्::isNoInheritanceMarker() ध्रुव {
    पटिनिवत्त ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_मानम्::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) ध्रुव {
    पटिनिवत्त ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_मानम्::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) ध्रुव {
    सचे(URES_IS_ARRAY(res)) {
        पटिनिवत्त ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    सचे(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }
    सचे(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        पटिनिवत्त 1;
    }
    int32_t sLength;
    ध्रुव char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    सचे(s != nullptr) {
        dest[0].setTo(सच्च, s, sLength);
        पटिनिवत्त 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    पटिनिवत्त 0;
}

UnicodeString Resource_Data_मानम्::getStringOrFirstOfArray(UErrorCode &errorCode) ध्रुव {
    UnicodeString us;
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त us;
    }
    int32_t sLength;
    ध्रुव char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    सचे(s != nullptr) {
        us.setTo(सच्च, s, sLength);
        पटिनिवत्त us;
    }
    ResourceArray array = getArray(errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त us;
    }
    सचे(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        सचे(s != nullptr) {
            us.setTo(सच्च, s, sLength);
            पटिनिवत्त us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    पटिनिवत्त us;
}

U_NAMESPACE_END

स्थिर Resource
makeResourceFrom16(ध्रुव ResourceData *pResData, int32_t res16) {
    सचे(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } अञ्ञथा {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    पटिनिवत्त URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_कुञ्चिका(ध्रुव ResourceData *pResData, Resource table,
                      int32_t *indexR, ध्रुव वर्ण **कुञ्चिका) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    सचे(कुञ्चिका == nullptr || *कुञ्चिका == nullptr) {
        पटिनिवत्त RES_BOGUS;
    }
    विकल्प(RES_GET_TYPE(table)) {
    पक्ष URES_TABLE: {
        सचे (offset!=0) { /* empty if offset==0 */
            ध्रुव uint16_t *p= (ध्रुव uint16_t *)(pResData->p_मूलम्+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *कुञ्चिका, कुञ्चिका);
            सचे(idx>=0) {
                ध्रुव Resource *p32=(ध्रुव Resource *)(p+length+(~length&1));
                पटिनिवत्त p32[idx];
            }
        }
        भङ्ग;
    }
    पक्ष URES_TABLE16: {
        ध्रुव uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *कुञ्चिका, कुञ्चिका);
        सचे(idx>=0) {
            पटिनिवत्त makeResourceFrom16(pResData, p[length+idx]);
        }
        भङ्ग;
    }
    पक्ष URES_TABLE32: {
        सचे (offset!=0) { /* empty if offset==0 */
            ध्रुव int32_t *p= pResData->p_मूलम्+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *कुञ्चिका, कुञ्चिका);
            सचे(idx>=0) {
                पटिनिवत्त (Resource)p[length+idx];
            }
        }
        भङ्ग;
    }
    स्वभाव:
        भङ्ग;
    }
    पटिनिवत्त RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(ध्रुव ResourceData *pResData, Resource table,
                        int32_t indexR, ध्रुव वर्ण **कुञ्चिका) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    सचे (indexR < 0) {
        पटिनिवत्त RES_BOGUS;
    }
    विकल्प(RES_GET_TYPE(table)) {
    पक्ष URES_TABLE: {
        सचे (offset != 0) { /* empty if offset==0 */
            ध्रुव uint16_t *p= (ध्रुव uint16_t *)(pResData->p_मूलम्+offset);
            length=*p++;
            सचे(indexR<length) {
                ध्रुव Resource *p32=(ध्रुव Resource *)(p+length+(~length&1));
                सचे(कुञ्चिका!=nullptr) {
                    *कुञ्चिका=RES_GET_KEY16(pResData, p[indexR]);
                }
                पटिनिवत्त p32[indexR];
            }
        }
        भङ्ग;
    }
    पक्ष URES_TABLE16: {
        ध्रुव uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        सचे(indexR<length) {
            सचे(कुञ्चिका!=nullptr) {
                *कुञ्चिका=RES_GET_KEY16(pResData, p[indexR]);
            }
            पटिनिवत्त makeResourceFrom16(pResData, p[length+indexR]);
        }
        भङ्ग;
    }
    पक्ष URES_TABLE32: {
        सचे (offset != 0) { /* empty if offset==0 */
            ध्रुव int32_t *p= pResData->p_मूलम्+offset;
            length=*p++;
            सचे(indexR<length) {
                सचे(कुञ्चिका!=nullptr) {
                    *कुञ्चिका=RES_GET_KEY32(pResData, p[indexR]);
                }
                पटिनिवत्त (Resource)p[length+indexR];
            }
        }
        भङ्ग;
    }
    स्वभाव:
        भङ्ग;
    }
    पटिनिवत्त RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ध्रुव ResourceData *pResData, ध्रुव वर्ण *कुञ्चिका) {
    ध्रुव वर्ण *real_कुञ्चिका=कुञ्चिका;
    int32_t idx;
    पटिनिवत्त res_get_Table_Item_By_कुञ्चिका(pResData, pResData->मूलम्_Res, &idx, &real_कुञ्चिका);
}


UBool icu::ResourceTable::get_कुञ्चिका_And_मानम्(int32_t i,
                                         ध्रुव वर्ण *&कुञ्चिका, icu::Resource_मानम् &value) ध्रुव {
    सचे(0 <= i && i < length) {
        icu::Resource_Data_मानम् &rd_मानम् = static_cast<icu::Resource_Data_मानम् &>(value);
        सचे (keys16 != nullptr) {
            कुञ्चिका = RES_GET_KEY16(&rd_मानम्.getData(), keys16[i]);
        } अञ्ञथा {
            कुञ्चिका = RES_GET_KEY32(&rd_मानम्.getData(), keys32[i]);
        }
        Resource res;
        सचे (items16 != nullptr) {
            res = makeResourceFrom16(&rd_मानम्.getData(), items16[i]);
        } अञ्ञथा {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_मानम्.setResource(res, ResourceTracer(fTraceInfo, कुञ्चिका));
        पटिनिवत्त सच्च;
    }
    पटिनिवत्त मिच्छा;
}

UBool icu::ResourceTable::find_मानम्(ध्रुव वर्ण *कुञ्चिका, Resource_मानम् &value) ध्रुव {
    icu::Resource_Data_मानम् &rd_मानम् = static_cast<icu::Resource_Data_मानम् &>(value);
    ध्रुव वर्ण *real_कुञ्चिका = nullptr;
    int32_t i;
    सचे (keys16 != nullptr) {
        i = _res_findTableItem(&rd_मानम्.getData(), keys16, length, कुञ्चिका, &real_कुञ्चिका);
    } अञ्ञथा {
        i = _res_findTable32Item(&rd_मानम्.getData(), keys32, length, कुञ्चिका, &real_कुञ्चिका);
    }
    सचे (i >= 0) {
        Resource res;
        सचे (items16 != nullptr) {
            res = makeResourceFrom16(&rd_मानम्.getData(), items16[i]);
        } अञ्ञथा {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_मानम्.setResource(res, ResourceTracer(fTraceInfo, कुञ्चिका));
        पटिनिवत्त सच्च;
    }
    पटिनिवत्त मिच्छा;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ध्रुव ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    सचे (indexR < 0) {
        पटिनिवत्त RES_BOGUS;
    }
    विकल्प(RES_GET_TYPE(array)) {
    पक्ष URES_ARRAY: {
        सचे (offset!=0) { /* empty if offset==0 */
            ध्रुव int32_t *p= pResData->p_मूलम्+offset;
            सचे(indexR<*p) {
                पटिनिवत्त (Resource)p[1+indexR];
            }
        }
        भङ्ग;
    }
    पक्ष URES_ARRAY16: {
        ध्रुव uint16_t *p=pResData->p16BitUnits+offset;
        सचे(indexR<*p) {
            पटिनिवत्त makeResourceFrom16(pResData, p[1+indexR]);
        }
        भङ्ग;
    }
    स्वभाव:
        भङ्ग;
    }
    पटिनिवत्त RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(ध्रुव ResourceData *pResData, int32_t i) ध्रुव {
    सचे (items16 != nullptr) {
        पटिनिवत्त makeResourceFrom16(pResData, items16[i]);
    } अञ्ञथा {
        पटिनिवत्त items32[i];
    }
}

UBool icu::ResourceArray::get_मानम्(int32_t i, icu::Resource_मानम् &value) ध्रुव {
    सचे(0 <= i && i < length) {
        icu::Resource_Data_मानम् &rd_मानम् = static_cast<icu::Resource_Data_मानम् &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_मानम्.setResource(
            internalGetResource(&rd_मानम्.getData(), i),
            ResourceTracer(fTraceInfo, i));
        पटिनिवत्त सच्च;
    }
    पटिनिवत्त मिच्छा;
}

U_CFUNC Resource
res_findResource(ध्रुव ResourceData *pResData, Resource r, वर्ण** path, ध्रुव वर्ण** कुञ्चिका) {
  वर्ण *pathP = *path, *nextSepP = *path;
  वर्ण *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  सचे(!uprv_strlen(pathP)) {
      पटिनिवत्त r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  सचे(!URES_IS_CONTAINER(type)) {
      पटिनिवत्त RES_BOGUS;
  }
  
  याव(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    सचे(nextSepP != nullptr) {
      सचे(nextSepP == pathP) {
        // Empty key string.
        पटिनिवत्त RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } अञ्ञथा {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    सचे(URES_IS_TABLE(type)) {
      *कुञ्चिका = pathP;
      t2 = res_get_Table_Item_By_कुञ्चिका(pResData, t1, &indexR, कुञ्चिका);
    } अञ्ञथा सचे(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      सचे(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } अञ्ञथा {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *कुञ्चिका = nullptr;
    } अञ्ञथा { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  पटिनिवत्त t1;
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
    int32_t कुञ्चिका_Index, sortIndex;
} Row;

स्थिर int32_t U_CALLCONV
ures_compareRows(ध्रुव शून्य *context, ध्रुव शून्य *left, ध्रुव शून्य *दक्षिणः) {
    ध्रुव वर्ण* कुञ्चिका_Chars = static_cast<ध्रुव वर्ण*>(context);
    पटिनिवत्त static_cast<int32_t>(uprv_strcmp(कुञ्चिका_Chars + static_cast<ध्रुव Row*>(left)->कुञ्चिका_Index,
                                            कुञ्चिका_Chars + static_cast<ध्रुव Row*>(दक्षिणः)->कुञ्चिका_Index));
}

प्रकारनिर्देश संरचना TempTable {
    ध्रुव वर्ण *कुञ्चिका_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_कुञ्चिका_Limit;
    uint8_t majorFormatVersion;
} TempTable;

गणना {
    राशिः_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
स्थिर ध्रुव वर्ण *ध्रुव g_Unknown_कुञ्चिका="";

#सचे !UCONFIG_NO_COLLATION
// resource table key for collation binaries
स्थिर ध्रुव char16_t g_Collation_Bin_कुञ्चिका[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
स्थिर शून्य
ures_swapResource(ध्रुव UDataSwapper *ds,
                  ध्रुव Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  ध्रुव वर्ण *कुञ्चिका,
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
        पटिनिवत्त;
    स्वभाव:
        भङ्ग;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    सचे(offset==0) {
        /* special offset indicating an empty item */
        पटिनिवत्त;
    }
    सचे (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        पटिनिवत्त;
    } अञ्ञथा {
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
#सचे !UCONFIG_NO_COLLATION
        सचे( कुञ्चिका!=nullptr &&  /* the binary is in a table */
            (कुञ्चिका!=g_Unknown_कुञ्चिका ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, कुञ्चिका, -1,
                                       g_Collation_Bin_कुञ्चिका, UPRV_LENGTHOF(g_Collation_Bin_कुञ्चिका)-1) :
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

            सचे(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<ध्रुव uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } अञ्ञथा {
                /* get table item count */
                pKey32 = reinterpret_cast<ध्रुव int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            सचे(count==0) {
                भङ्ग;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            अत्थाय(i=0; i<count; ++i) {
                ध्रुव वर्ण *item_कुञ्चिका=g_Unknown_कुञ्चिका;
                सचे(pKey16!=nullptr) {
                    int32_t कुञ्चिका_Offset=ds->readUInt16(pKey16[i]);
                    सचे(कुञ्चिका_Offset<pTempTable->local_कुञ्चिका_Limit) {
                        item_कुञ्चिका = reinterpret_cast<ध्रुव वर्ण*>(outBundle) + कुञ्चिका_Offset;
                    }
                } अञ्ञथा {
                    int32_t कुञ्चिका_Offset=udata_readInt32(ds, pKey32[i]);
                    सचे(कुञ्चिका_Offset>=0) {
                        item_कुञ्चिका = reinterpret_cast<ध्रुव वर्ण*>(outBundle) + कुञ्चिका_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_कुञ्चिका, pTempTable, pErrorCode);
                सचे(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    पटिनिवत्त;
                }
            }

            सचे(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                सचे(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } अञ्ञथा {
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
            सचे(pKey16!=nullptr) {
                अत्थाय(i=0; i<count; ++i) {
                    pTempTable->rows[i].कुञ्चिका_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } अञ्ञथा {
                अत्थाय(i=0; i<count; ++i) {
                    pTempTable->rows[i].कुञ्चिका_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, परिमाण(Row),
                           ures_compareRows, pTempTable->कुञ्चिका_Chars,
                           मिच्छा, pErrorCode);
            सचे(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                पटिनिवत्त;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*कुञ्चिकाः*/
            सचे(pKey16!=nullptr) {
                uint16_t *rKey16;

                सचे(pKey16!=qKey16) {
                    rKey16=qKey16;
                } अञ्ञथा {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                अत्थाय(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                सचे(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } अञ्ञथा {
                int32_t *rKey32;

                सचे(pKey32!=qKey32) {
                    rKey32=qKey32;
                } अञ्ञथा {
                    rKey32=pTempTable->resort;
                }
                अत्थाय(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                सचे(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                सचे(p!=q) {
                    r=q;
                } अञ्ञथा {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                अत्थाय(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                सचे(q!=r) {
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
            अत्थाय(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                सचे(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    पटिनिवत्त;
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
    Resource मूलम्_Res;
    int32_t headerSize, maxTableLength;

    Row rows[राशिः_ROW_CAPACITY];
    int32_t resort[राशिः_ROW_CAPACITY];
    TempTable tempTable;

    ध्रुव int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, कुञ्चिकाः_Bottom, कुञ्चिकाः_शिखरम्, resBottom, शिखरम्;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    सचे(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        पटिनिवत्त 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)inData+4);
    सचे(!(
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
        पटिनिवत्त 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    सचे(length<0) {
        bundleLength=-1;
    } अञ्ञथा {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        सचे(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त 0;
        }
    }

    inBundle=(ध्रुव Resource *)((ध्रुव वर्ण *)inData+headerSize);
    मूलम्_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(ध्रुव int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    सचे(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }
    कुञ्चिकाः_Bottom=1+indexLength;
    कुञ्चिकाः_शिखरम्=udata_readInt32(ds, inIndexes[URES_INDEX_कुञ्चिकाः_शिखरम्]);
    सचे(indexLength>URES_INDEX_16_BIT_शिखरम्) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_शिखरम्]);
    } अञ्ञथा {
        resBottom=कुञ्चिकाः_शिखरम्;
    }
    शिखरम्=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_शिखरम्]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    सचे(0<=bundleLength && bundleLength<शिखरम्) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         शिखरम्, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }
    सचे(कुञ्चिकाः_शिखरम्>(1+indexLength)) {
        tempTable.local_कुञ्चिका_Limit=कुञ्चिकाः_शिखरम्<<2;
    } अञ्ञथा {
        tempTable.local_कुञ्चिका_Limit=0;
    }

    सचे(length>=0) {
        Resource *outBundle=(Resource *)((वर्ण *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t राशिः_Res_Flags[राशिः_ROW_CAPACITY];
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
        सचे(resFlagsLength<=(int32_t)परिमाण(राशिः_Res_Flags)) {
            tempTable.resFlags=राशिः_Res_Flags;
        } अञ्ञथा {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            सचे(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        सचे(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*शिखरम्);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+कुञ्चिकाः_Bottom, 4*(कुञ्चिकाः_शिखरम्-कुञ्चिकाः_Bottom),
                                    outBundle+कुञ्चिकाः_Bottom, pErrorCode);
        सचे(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(कुञ्चिकाः_शिखरम्-कुञ्चिकाः_Bottom));
            सचे(tempTable.resFlags!=राशिः_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            पटिनिवत्त 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        सचे(कुञ्चिकाः_शिखरम्<resBottom) {
            ds->swapArray16(ds, inBundle+कुञ्चिकाः_शिखरम्, (resBottom-कुञ्चिकाः_शिखरम्)*4, outBundle+कुञ्चिकाः_शिखरम्, pErrorCode);
            सचे(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-कुञ्चिकाः_शिखरम्));
                सचे(tempTable.resFlags!=राशिः_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                पटिनिवत्त 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.कुञ्चिका_Chars=(ध्रुव वर्ण *)outBundle; /* sort by outCharset */
        सचे(tempTable.majorFormatVersion>1 || maxTableLength<=राशिः_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } अञ्ञथा {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*परिमाण(Row)+maxTableLength*4);
            सचे(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                सचे(tempTable.resFlags!=राशिः_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                पटिनिवत्त 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, मूलम्_Res, nullptr, &tempTable, pErrorCode);
        सचे(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             मूलम्_Res);
        }

        सचे(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        सचे(tempTable.resFlags!=राशिः_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, कुञ्चिकाः_Bottom*4, outBundle, pErrorCode);
    }

    पटिनिवत्त headerSize+4*शिखरम्;
}

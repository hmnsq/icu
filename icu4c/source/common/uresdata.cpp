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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "resource.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucol_swp.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "uresdata.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, ಕೀಲಿ_Offset) \
    ((ಕೀಲಿ_Offset)<(pResData)->local_ಕೀಲಿ_Limit ? \
        (ಧ್ರುವ ವರ್ಣ *)(pResData)->p_ಬೇರು+(ಕೀಲಿ_Offset) : \
        (pResData)->pool_Bundle_ಕೀಲಿಗಳು+(ಕೀಲಿ_Offset)-(pResData)->local_ಕೀಲಿ_Limit)

#define RES_GET_KEY32(pResData, ಕೀಲಿ_Offset) \
    ((ಕೀಲಿ_Offset)>=0 ? \
        (ಧ್ರುವ ವರ್ಣ *)(pResData)->p_ಬೇರು+(ಕೀಲಿ_Offset) : \
        (pResData)->pool_Bundle_ಕೀಲಿಗಳು+((ಕೀಲಿ_Offset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
ಸ್ಥಿರ ಧ್ರುವ uint16_t gEmpty16=0;

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ಸ್ಥಿರ int32_t
_res_findTableItem(ಧ್ರುವ ResourceData *pResData, ಧ್ರುವ uint16_t *ಕೀಲಿ_Offsets, int32_t length,
                   ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ **real_ಕೀಲಿ) {
    ಧ್ರುವ ವರ್ಣ *table_ಕೀಲಿ;
    int32_t mid, start, limit;
    ಪೂರ್ಣಾಙ್ಕ result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    ಯಾವತ್(start<limit) {
        mid = (start + limit) / 2;
        table_ಕೀಲಿ = RES_GET_KEY16(pResData, ಕೀಲಿ_Offsets[mid]);
        ಯದಿ (pResData->useNativeStrcmp) {
            result = uprv_strcmp(ಕೀಲಿ, table_ಕೀಲಿ);
        } ಅನ್ಯಥಾ {
            result = uprv_compareInvCharsAsAscii(ಕೀಲಿ, table_ಕೀಲಿ);
        }
        ಯದಿ (result < 0) {
            limit = mid;
        } ಅನ್ಯಥಾ ಯದಿ (result > 0) {
            start = mid + 1;
        } ಅನ್ಯಥಾ {
            /* We found it! */
            *real_ಕೀಲಿ=table_ಕೀಲಿ;
            ಪ್ರತ್ಯಾಗಮ mid;
        }
    }
    ಪ್ರತ್ಯಾಗಮ URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

ಸ್ಥಿರ int32_t
_res_findTable32Item(ಧ್ರುವ ResourceData *pResData, ಧ್ರುವ int32_t *ಕೀಲಿ_Offsets, int32_t length,
                     ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ **real_ಕೀಲಿ) {
    ಧ್ರುವ ವರ್ಣ *table_ಕೀಲಿ;
    int32_t mid, start, limit;
    ಪೂರ್ಣಾಙ್ಕ result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    ಯಾವತ್(start<limit) {
        mid = (start + limit) / 2;
        table_ಕೀಲಿ = RES_GET_KEY32(pResData, ಕೀಲಿ_Offsets[mid]);
        ಯದಿ (pResData->useNativeStrcmp) {
            result = uprv_strcmp(ಕೀಲಿ, table_ಕೀಲಿ);
        } ಅನ್ಯಥಾ {
            result = uprv_compareInvCharsAsAscii(ಕೀಲಿ, table_ಕೀಲಿ);
        }
        ಯದಿ (result < 0) {
            limit = mid;
        } ಅನ್ಯಥಾ ಯದಿ (result > 0) {
            start = mid + 1;
        } ಅನ್ಯಥಾ {
            /* We found it! */
            *real_ಕೀಲಿ=table_ಕೀಲಿ;
            ಪ್ರತ್ಯಾಗಮ mid;
        }
    }
    ಪ್ರತ್ಯಾಗಮ URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ಸ್ಥಿರ UBool U_CALLCONV
isAcceptable(ಶೂನ್ಯ *context,
             ಧ್ರುವ ವರ್ಣ * /*type*/, ಧ್ರುವ ವರ್ಣ * /*name*/,
             ಧ್ರುವ UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    ಪ್ರತ್ಯಾಗಮ
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

ಸ್ಥಿರ ಶೂನ್ಯ
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, ಧ್ರುವ ಶೂನ್ಯ *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType ಬೇರು_Type;

    /* get the root resource */
    pResData->p_ಬೇರು = static_cast<ಧ್ರುವ int32_t*>(inBytes);
    pResData->ಬೇರು_Res = static_cast<Resource>(*pResData->p_ಬೇರು);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    ಯದಿ(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* currently, we accept only resources that have a Table as their roots */
    ಬೇರು_Type = static_cast<UResType>(RES_GET_TYPE(pResData->ಬೇರು_Res));
    ಯದಿ(!URES_IS_TABLE(ಬೇರು_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_ಕೀಲಿ_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } ಅನ್ಯಥಾ {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ಧ್ರುವ int32_t *indexes=pResData->p_ಬೇರು+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        ಯದಿ(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_ತುದಿ]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ(indexes[URES_INDEX_ಕೀಲಿಗಳು_ತುದಿ]>(1+indexLength)) {
            pResData->local_ಕೀಲಿ_Limit=indexes[URES_INDEX_ಕೀಲಿಗಳು_ತುದಿ]<<2;
        }
        ಯದಿ(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        ಯದಿ(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        ಯದಿ((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            ಪ್ರತ್ಯಾಗಮ;
        }
        ಯದಿ( indexLength>URES_INDEX_16_BIT_ತುದಿ &&
            indexes[URES_INDEX_16_BIT_ತುದಿ]>indexes[URES_INDEX_ಕೀಲಿಗಳು_ತುದಿ]
        ) {
            pResData->p16BitUnits = reinterpret_cast<ಧ್ರುವ uint16_t*>(pResData->p_ಬೇರು + indexes[URES_INDEX_ಕೀಲಿಗಳು_ತುದಿ]);
        }
    }

    ಯದಿ(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=ಸತ್ಯ;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
res_read(ResourceData *pResData,
         ಧ್ರುವ UDataInfo *pInfo, ಧ್ರುವ ಶೂನ್ಯ *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, ಪರಿಮಾಣ(ResourceData));
    ಯದಿ(U_FAILURE(*errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC ಶೂನ್ಯ
res_load(ResourceData *pResData,
         ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, ಪರಿಮಾಣ(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    ಯದಿ(U_FAILURE(*errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC ಶೂನ್ಯ
res_unload(ResourceData *pResData) {
    ಯದಿ(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

ಸ್ಥಿರ ಧ್ರುವ int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_ದ್ವಿಮಾನ,
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
    ಪ್ರತ್ಯಾಗಮ (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI ಧ್ರುವ char16_t * U_EXPORT2
res_getStringNoTrace(ಧ್ರುವ ResourceData *pResData, Resource res, int32_t *pLength) {
    ಧ್ರುವ char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ಯದಿ(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        ಯದಿ((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(ಧ್ರುವ char16_t *)pResData->poolBundleStrings+offset;
        } ಅನ್ಯಥಾ {
            p=(ಧ್ರುವ char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        ಯದಿ(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } ಅನ್ಯಥಾ ಯದಿ(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } ಅನ್ಯಥಾ ಯದಿ(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } ಅನ್ಯಥಾ {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } ಅನ್ಯಥಾ ಯದಿ(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        ಧ್ರುವ int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_ಬೇರು+res;
        length=*p32++;
        p=(ಧ್ರುವ char16_t *)p32;
    } ಅನ್ಯಥಾ {
        p=nullptr;
        length=0;
    }
    ಯದಿ(pLength) {
        *pLength=length;
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

ನಾಮಾಕಾಶ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ಧ್ರುವ ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    ಯದಿ (offset == 0) {
        // empty string
    } ಅನ್ಯಥಾ ಯದಿ (res == offset) {
        ಧ್ರುವ int32_t *p32=pResData->p_ಬೇರು+res;
        int32_t length=*p32;
        ಧ್ರುವ char16_t* p = reinterpret_cast<ಧ್ರುವ char16_t*>(p32);
        ಪ್ರತ್ಯಾಗಮ length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } ಅನ್ಯಥಾ ಯದಿ (RES_GET_TYPE(res) == URES_STRING_V2) {
        ಧ್ರುವ char16_t *p;
        ಯದಿ (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<ಧ್ರುವ char16_t*>(pResData->poolBundleStrings) + offset;
        } ಅನ್ಯಥಾ {
            p = reinterpret_cast<ಧ್ರುವ char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        ಯದಿ (first == 0x2205) {  // implicit length
            ಪ್ರತ್ಯಾಗಮ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } ಅನ್ಯಥಾ ಯದಿ (first == 0xdc03) {  // explicit length 3 (should not occur)
            ಪ್ರತ್ಯಾಗಮ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } ಅನ್ಯಥಾ {
            // Assume that the string has not been stored with more length units than necessary.
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

int32_t getStringArray(ಧ್ರುವ ResourceData *pResData, ಧ್ರುವ icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t length = array.getSize();
    ಯದಿ(length == 0) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ length;
    }
    ಪ್ರತಿ(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        ಧ್ರುವ char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        ಯದಿ(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        dest[i].setTo(ಸತ್ಯ, s, sLength);
    }
    ಪ್ರತ್ಯಾಗಮ length;
}

}  // namespace

U_CAPI ಧ್ರುವ char16_t * U_EXPORT2
res_getAlias(ಧ್ರುವ ResourceData *pResData, Resource res, int32_t *pLength) {
    ಧ್ರುವ char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ಯದಿ(RES_GET_TYPE(res)==URES_ALIAS) {
        ಧ್ರುವ int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_ಬೇರು+offset;
        length=*p32++;
        p=(ಧ್ರುವ char16_t *)p32;
    } ಅನ್ಯಥಾ {
        p=nullptr;
        length=0;
    }
    ಯದಿ(pLength) {
        *pLength=length;
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

U_CAPI ಧ್ರುವ uint8_t * U_EXPORT2
res_get_ದ್ವಿಮಾನ_No_Trace(ಧ್ರುವ ResourceData *pResData, Resource res, int32_t *pLength) {
    ಧ್ರುವ uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ಯದಿ(RES_GET_TYPE(res)==URES_ದ್ವಿಮಾನ) {
        ಧ್ರುವ int32_t *p32= offset==0 ? (ಧ್ರುವ int32_t*)&gEmpty32 : pResData->p_ಬೇರು+offset;
        length=*p32++;
        p=(ಧ್ರುವ uint8_t *)p32;
    } ಅನ್ಯಥಾ {
        p=nullptr;
        length=0;
    }
    ಯದಿ(pLength) {
        *pLength=length;
    }
    ಪ್ರತ್ಯಾಗಮ p;
}


U_CAPI ಧ್ರುವ int32_t * U_EXPORT2
res_getIntVectorNoTrace(ಧ್ರುವ ResourceData *pResData, Resource res, int32_t *pLength) {
    ಧ್ರುವ int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    ಯದಿ(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (ಧ್ರುವ int32_t *)&gEmpty32 : pResData->p_ಬೇರು+offset;
        length=*p++;
    } ಅನ್ಯಥಾ {
        p=nullptr;
        length=0;
    }
    ಯದಿ(pLength) {
        *pLength=length;
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

U_CAPI int32_t U_EXPORT2
res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(ಧ್ರುವ ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    ವಿಕಲ್ಪ(RES_GET_TYPE(res)) {
    ಪಕ್ಷ URES_STRING:
    ಪಕ್ಷ URES_STRING_V2:
    ಪಕ್ಷ URES_ದ್ವಿಮಾನ:
    ಪಕ್ಷ URES_ALIAS:
    ಪಕ್ಷ URES_INT:
    ಪಕ್ಷ URES_INT_VECTOR:
        ಪ್ರತ್ಯಾಗಮ 1;
    ಪಕ್ಷ URES_ARRAY:
    ಪಕ್ಷ URES_TABLE32:
        ಪ್ರತ್ಯಾಗಮ offset==0 ? 0 : *(pResData->p_ಬೇರು+offset);
    ಪಕ್ಷ URES_TABLE:
        ಪ್ರತ್ಯಾಗಮ offset==0 ? 0 : *((ಧ್ರುವ uint16_t *)(pResData->p_ಬೇರು+offset));
    ಪಕ್ಷ URES_ARRAY16:
    ಪಕ್ಷ URES_TABLE16:
        ಪ್ರತ್ಯಾಗಮ pResData->p16BitUnits[offset];
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_ಮೌಲ್ಯ::~Resource_Data_ಮೌಲ್ಯ() {}

UResType Resource_Data_ಮೌಲ್ಯ::getType() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ res_getPublicType(res);
}

ಧ್ರುವ char16_t *Resource_Data_ಮೌಲ್ಯ::getString(int32_t &length, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    ಯದಿ(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

ಧ್ರುವ char16_t *Resource_Data_ಮೌಲ್ಯ::getAliasString(int32_t &length, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ char16_t *s = res_getAlias(&getData(), res, &length);
    ಯದಿ(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

int32_t Resource_Data_ಮೌಲ್ಯ::getInt(UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ಪ್ರತ್ಯಾಗಮ res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_ಮೌಲ್ಯ::getUInt(UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ಪ್ರತ್ಯಾಗಮ res_getUInt(fTraceInfo, res);
}

ಧ್ರುವ int32_t *Resource_Data_ಮೌಲ್ಯ::getIntVector(int32_t &length, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    ಯದಿ(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ಪ್ರತ್ಯಾಗಮ iv;
}

ಧ್ರುವ uint8_t *Resource_Data_ಮೌಲ್ಯ::get_ದ್ವಿಮಾನ(int32_t &length, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ uint8_t *b = res_get_ದ್ವಿಮಾನ(fTraceInfo, &getData(), res, &length);
    ಯದಿ(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ಪ್ರತ್ಯಾಗಮ b;
}

ResourceArray Resource_Data_ಮೌಲ್ಯ::getArray(UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ {};
    }
    ಧ್ರುವ uint16_t *items16 = nullptr;
    ಧ್ರುವ Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    ವಿಕಲ್ಪ(RES_GET_TYPE(res)) {
    ಪಕ್ಷ URES_ARRAY:
        ಯದಿ (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ಧ್ರುವ Resource*>(getData().p_ಬೇರು) + offset;
            length = *items32++;
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        ಪ್ರತ್ಯಾಗಮ {};
    }
    ಪ್ರತ್ಯಾಗಮ ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_Data_ಮೌಲ್ಯ::getTable(UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ {};
    }
    ಧ್ರುವ uint16_t *keys16 = nullptr;
    ಧ್ರುವ int32_t *keys32 = nullptr;
    ಧ್ರುವ uint16_t *items16 = nullptr;
    ಧ್ರುವ Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    ವಿಕಲ್ಪ(RES_GET_TYPE(res)) {
    ಪಕ್ಷ URES_TABLE:
        ಯದಿ (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ಧ್ರುವ uint16_t*>(getData().p_ಬೇರು + offset);
            length = *keys16++;
            items32 = reinterpret_cast<ಧ್ರುವ Resource*>(keys16 + length + (~length & 1));
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        ಭಙ್ಗ;
    ಪಕ್ಷ URES_TABLE32:
        ಯದಿ (offset != 0) {  // empty if offset==0
            keys32 = getData().p_ಬೇರು+offset;
            length = *keys32++;
            items32 = reinterpret_cast<ಧ್ರುವ Resource*>(keys32) + length;
        }
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        ಪ್ರತ್ಯಾಗಮ {};
    }
    ಪ್ರತ್ಯಾಗಮ ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_Data_ಮೌಲ್ಯ::isNoInheritanceMarker() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_ಮೌಲ್ಯ::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_ಮೌಲ್ಯ::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ(URES_IS_ARRAY(res)) {
        ಪ್ರತ್ಯಾಗಮ ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    int32_t sLength;
    ಧ್ರುವ char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    ಯದಿ(s != nullptr) {
        dest[0].setTo(ಸತ್ಯ, s, sLength);
        ಪ್ರತ್ಯಾಗಮ 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    ಪ್ರತ್ಯಾಗಮ 0;
}

UnicodeString Resource_Data_ಮೌಲ್ಯ::getStringOrFirstOfArray(UErrorCode &errorCode) ಧ್ರುವ {
    UnicodeString us;
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ us;
    }
    int32_t sLength;
    ಧ್ರುವ char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    ಯದಿ(s != nullptr) {
        us.setTo(ಸತ್ಯ, s, sLength);
        ಪ್ರತ್ಯಾಗಮ us;
    }
    ResourceArray array = getArray(errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ us;
    }
    ಯದಿ(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        ಯದಿ(s != nullptr) {
            us.setTo(ಸತ್ಯ, s, sLength);
            ಪ್ರತ್ಯಾಗಮ us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    ಪ್ರತ್ಯಾಗಮ us;
}

U_NAMESPACE_END

ಸ್ಥಿರ Resource
makeResourceFrom16(ಧ್ರುವ ResourceData *pResData, int32_t res16) {
    ಯದಿ(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } ಅನ್ಯಥಾ {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    ಪ್ರತ್ಯಾಗಮ URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_ಕೀಲಿ(ಧ್ರುವ ResourceData *pResData, Resource table,
                      int32_t *indexR, ಧ್ರುವ ವರ್ಣ **ಕೀಲಿ) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    ಯದಿ(ಕೀಲಿ == nullptr || *ಕೀಲಿ == nullptr) {
        ಪ್ರತ್ಯಾಗಮ RES_BOGUS;
    }
    ವಿಕಲ್ಪ(RES_GET_TYPE(table)) {
    ಪಕ್ಷ URES_TABLE: {
        ಯದಿ (offset!=0) { /* empty if offset==0 */
            ಧ್ರುವ uint16_t *p= (ಧ್ರುವ uint16_t *)(pResData->p_ಬೇರು+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *ಕೀಲಿ, ಕೀಲಿ);
            ಯದಿ(idx>=0) {
                ಧ್ರುವ Resource *p32=(ಧ್ರುವ Resource *)(p+length+(~length&1));
                ಪ್ರತ್ಯಾಗಮ p32[idx];
            }
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ URES_TABLE16: {
        ಧ್ರುವ uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *ಕೀಲಿ, ಕೀಲಿ);
        ಯದಿ(idx>=0) {
            ಪ್ರತ್ಯಾಗಮ makeResourceFrom16(pResData, p[length+idx]);
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ URES_TABLE32: {
        ಯದಿ (offset!=0) { /* empty if offset==0 */
            ಧ್ರುವ int32_t *p= pResData->p_ಬೇರು+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *ಕೀಲಿ, ಕೀಲಿ);
            ಯದಿ(idx>=0) {
                ಪ್ರತ್ಯಾಗಮ (Resource)p[length+idx];
            }
        }
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(ಧ್ರುವ ResourceData *pResData, Resource table,
                        int32_t indexR, ಧ್ರುವ ವರ್ಣ **ಕೀಲಿ) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    ಯದಿ (indexR < 0) {
        ಪ್ರತ್ಯಾಗಮ RES_BOGUS;
    }
    ವಿಕಲ್ಪ(RES_GET_TYPE(table)) {
    ಪಕ್ಷ URES_TABLE: {
        ಯದಿ (offset != 0) { /* empty if offset==0 */
            ಧ್ರುವ uint16_t *p= (ಧ್ರುವ uint16_t *)(pResData->p_ಬೇರು+offset);
            length=*p++;
            ಯದಿ(indexR<length) {
                ಧ್ರುವ Resource *p32=(ಧ್ರುವ Resource *)(p+length+(~length&1));
                ಯದಿ(ಕೀಲಿ!=nullptr) {
                    *ಕೀಲಿ=RES_GET_KEY16(pResData, p[indexR]);
                }
                ಪ್ರತ್ಯಾಗಮ p32[indexR];
            }
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ URES_TABLE16: {
        ಧ್ರುವ uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        ಯದಿ(indexR<length) {
            ಯದಿ(ಕೀಲಿ!=nullptr) {
                *ಕೀಲಿ=RES_GET_KEY16(pResData, p[indexR]);
            }
            ಪ್ರತ್ಯಾಗಮ makeResourceFrom16(pResData, p[length+indexR]);
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ URES_TABLE32: {
        ಯದಿ (offset != 0) { /* empty if offset==0 */
            ಧ್ರುವ int32_t *p= pResData->p_ಬೇರು+offset;
            length=*p++;
            ಯದಿ(indexR<length) {
                ಯದಿ(ಕೀಲಿ!=nullptr) {
                    *ಕೀಲಿ=RES_GET_KEY32(pResData, p[indexR]);
                }
                ಪ್ರತ್ಯಾಗಮ (Resource)p[length+indexR];
            }
        }
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ಧ್ರುವ ResourceData *pResData, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ) {
    ಧ್ರುವ ವರ್ಣ *real_ಕೀಲಿ=ಕೀಲಿ;
    int32_t idx;
    ಪ್ರತ್ಯಾಗಮ res_get_Table_Item_By_ಕೀಲಿ(pResData, pResData->ಬೇರು_Res, &idx, &real_ಕೀಲಿ);
}


UBool icu::ResourceTable::get_ಕೀಲಿ_And_ಮೌಲ್ಯ(int32_t i,
                                         ಧ್ರುವ ವರ್ಣ *&ಕೀಲಿ, icu::Resource_ಮೌಲ್ಯ &value) ಧ್ರುವ {
    ಯದಿ(0 <= i && i < length) {
        icu::Resource_Data_ಮೌಲ್ಯ &rd_ಮೌಲ್ಯ = static_cast<icu::Resource_Data_ಮೌಲ್ಯ &>(value);
        ಯದಿ (keys16 != nullptr) {
            ಕೀಲಿ = RES_GET_KEY16(&rd_ಮೌಲ್ಯ.getData(), keys16[i]);
        } ಅನ್ಯಥಾ {
            ಕೀಲಿ = RES_GET_KEY32(&rd_ಮೌಲ್ಯ.getData(), keys32[i]);
        }
        Resource res;
        ಯದಿ (items16 != nullptr) {
            res = makeResourceFrom16(&rd_ಮೌಲ್ಯ.getData(), items16[i]);
        } ಅನ್ಯಥಾ {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ಮೌಲ್ಯ.setResource(res, ResourceTracer(fTraceInfo, ಕೀಲಿ));
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

UBool icu::ResourceTable::find_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value) ಧ್ರುವ {
    icu::Resource_Data_ಮೌಲ್ಯ &rd_ಮೌಲ್ಯ = static_cast<icu::Resource_Data_ಮೌಲ್ಯ &>(value);
    ಧ್ರುವ ವರ್ಣ *real_ಕೀಲಿ = nullptr;
    int32_t i;
    ಯದಿ (keys16 != nullptr) {
        i = _res_findTableItem(&rd_ಮೌಲ್ಯ.getData(), keys16, length, ಕೀಲಿ, &real_ಕೀಲಿ);
    } ಅನ್ಯಥಾ {
        i = _res_findTable32Item(&rd_ಮೌಲ್ಯ.getData(), keys32, length, ಕೀಲಿ, &real_ಕೀಲಿ);
    }
    ಯದಿ (i >= 0) {
        Resource res;
        ಯದಿ (items16 != nullptr) {
            res = makeResourceFrom16(&rd_ಮೌಲ್ಯ.getData(), items16[i]);
        } ಅನ್ಯಥಾ {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_ಮೌಲ್ಯ.setResource(res, ResourceTracer(fTraceInfo, ಕೀಲಿ));
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ಧ್ರುವ ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    ಯದಿ (indexR < 0) {
        ಪ್ರತ್ಯಾಗಮ RES_BOGUS;
    }
    ವಿಕಲ್ಪ(RES_GET_TYPE(array)) {
    ಪಕ್ಷ URES_ARRAY: {
        ಯದಿ (offset!=0) { /* empty if offset==0 */
            ಧ್ರುವ int32_t *p= pResData->p_ಬೇರು+offset;
            ಯದಿ(indexR<*p) {
                ಪ್ರತ್ಯಾಗಮ (Resource)p[1+indexR];
            }
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ URES_ARRAY16: {
        ಧ್ರುವ uint16_t *p=pResData->p16BitUnits+offset;
        ಯದಿ(indexR<*p) {
            ಪ್ರತ್ಯಾಗಮ makeResourceFrom16(pResData, p[1+indexR]);
        }
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(ಧ್ರುವ ResourceData *pResData, int32_t i) ಧ್ರುವ {
    ಯದಿ (items16 != nullptr) {
        ಪ್ರತ್ಯಾಗಮ makeResourceFrom16(pResData, items16[i]);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ items32[i];
    }
}

UBool icu::ResourceArray::get_ಮೌಲ್ಯ(int32_t i, icu::Resource_ಮೌಲ್ಯ &value) ಧ್ರುವ {
    ಯದಿ(0 <= i && i < length) {
        icu::Resource_Data_ಮೌಲ್ಯ &rd_ಮೌಲ್ಯ = static_cast<icu::Resource_Data_ಮೌಲ್ಯ &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ಮೌಲ್ಯ.setResource(
            internalGetResource(&rd_ಮೌಲ್ಯ.getData(), i),
            ResourceTracer(fTraceInfo, i));
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

U_CFUNC Resource
res_findResource(ಧ್ರುವ ResourceData *pResData, Resource r, ವರ್ಣ** path, ಧ್ರುವ ವರ್ಣ** ಕೀಲಿ) {
  ವರ್ಣ *pathP = *path, *nextSepP = *path;
  ವರ್ಣ *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  ಯದಿ(!uprv_strlen(pathP)) {
      ಪ್ರತ್ಯಾಗಮ r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  ಯದಿ(!URES_IS_CONTAINER(type)) {
      ಪ್ರತ್ಯಾಗಮ RES_BOGUS;
  }
  
  ಯಾವತ್(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    ಯದಿ(nextSepP != nullptr) {
      ಯದಿ(nextSepP == pathP) {
        // Empty key string.
        ಪ್ರತ್ಯಾಗಮ RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } ಅನ್ಯಥಾ {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    ಯದಿ(URES_IS_TABLE(type)) {
      *ಕೀಲಿ = pathP;
      t2 = res_get_Table_Item_By_ಕೀಲಿ(pResData, t1, &indexR, ಕೀಲಿ);
    } ಅನ್ಯಥಾ ಯದಿ(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      ಯದಿ(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } ಅನ್ಯಥಾ {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *ಕೀಲಿ = nullptr;
    } ಅನ್ಯಥಾ { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  ಪ್ರತ್ಯಾಗಮ t1;
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ Row {
    int32_t ಕೀಲಿ_Index, sortIndex;
} Row;

ಸ್ಥಿರ int32_t U_CALLCONV
ures_compareRows(ಧ್ರುವ ಶೂನ್ಯ *context, ಧ್ರುವ ಶೂನ್ಯ *left, ಧ್ರುವ ಶೂನ್ಯ *ಬಲ) {
    ಧ್ರುವ ವರ್ಣ* ಕೀಲಿ_Chars = static_cast<ಧ್ರುವ ವರ್ಣ*>(context);
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(uprv_strcmp(ಕೀಲಿ_Chars + static_cast<ಧ್ರುವ Row*>(left)->ಕೀಲಿ_Index,
                                            ಕೀಲಿ_Chars + static_cast<ಧ್ರುವ Row*>(ಬಲ)->ಕೀಲಿ_Index));
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ TempTable {
    ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_ಕೀಲಿ_Limit;
    uint8_t majorFormatVersion;
} TempTable;

ಗಣನಾ {
    ರಾಶಿ_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ g_Unknown_ಕೀಲಿ="";

#ಯದಿ !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ಸ್ಥಿರ ಧ್ರುವ char16_t g_Collation_Bin_ಕೀಲಿ[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ures_swapResource(ಧ್ರುವ UDataSwapper *ds,
                  ಧ್ರುವ Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    ಧ್ರುವ Resource *p;
    Resource *q;
    int32_t offset, count;

    ವಿಕಲ್ಪ(RES_GET_TYPE(res)) {
    ಪಕ್ಷ URES_TABLE16:
    ಪಕ್ಷ URES_STRING_V2:
    ಪಕ್ಷ URES_INT:
    ಪಕ್ಷ URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        ಪ್ರತ್ಯಾಗಮ;
    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    ಯದಿ(offset==0) {
        /* special offset indicating an empty item */
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        ಪ್ರತ್ಯಾಗಮ;
    } ಅನ್ಯಥಾ {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    ವಿಕಲ್ಪ(RES_GET_TYPE(res)) {
    ಪಕ್ಷ URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    ಪಕ್ಷ URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        ಭಙ್ಗ;
    ಪಕ್ಷ URES_ದ್ವಿಮಾನ:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#ಯದಿ !UCONFIG_NO_COLLATION
        ಯದಿ( ಕೀಲಿ!=nullptr &&  /* the binary is in a table */
            (ಕೀಲಿ!=g_Unknown_ಕೀಲಿ ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, ಕೀಲಿ, -1,
                                       g_Collation_Bin_ಕೀಲಿ, UPRV_LENGTHOF(g_Collation_Bin_ಕೀಲಿ)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_ದ್ವಿಮಾನ(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#endif
        ಭಙ್ಗ;
    ಪಕ್ಷ URES_TABLE:
    ಪಕ್ಷ URES_TABLE32:
        {
            ಧ್ರುವ uint16_t *pKey16;
            uint16_t *qKey16;

            ಧ್ರುವ int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            ಯದಿ(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<ಧ್ರುವ uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } ಅನ್ಯಥಾ {
                /* get table item count */
                pKey32 = reinterpret_cast<ಧ್ರುವ int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            ಯದಿ(count==0) {
                ಭಙ್ಗ;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            ಪ್ರತಿ(i=0; i<count; ++i) {
                ಧ್ರುವ ವರ್ಣ *item_ಕೀಲಿ=g_Unknown_ಕೀಲಿ;
                ಯದಿ(pKey16!=nullptr) {
                    int32_t ಕೀಲಿ_Offset=ds->readUInt16(pKey16[i]);
                    ಯದಿ(ಕೀಲಿ_Offset<pTempTable->local_ಕೀಲಿ_Limit) {
                        item_ಕೀಲಿ = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(outBundle) + ಕೀಲಿ_Offset;
                    }
                } ಅನ್ಯಥಾ {
                    int32_t ಕೀಲಿ_Offset=udata_readInt32(ds, pKey32[i]);
                    ಯದಿ(ಕೀಲಿ_Offset>=0) {
                        item_ಕೀಲಿ = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(outBundle) + ಕೀಲಿ_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_ಕೀಲಿ, pTempTable, pErrorCode);
                ಯದಿ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }

            ಯದಿ(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                ಯದಿ(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } ಅನ್ಯಥಾ {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                ಭಙ್ಗ;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            ಯದಿ(pKey16!=nullptr) {
                ಪ್ರತಿ(i=0; i<count; ++i) {
                    pTempTable->rows[i].ಕೀಲಿ_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } ಅನ್ಯಥಾ {
                ಪ್ರತಿ(i=0; i<count; ++i) {
                    pTempTable->rows[i].ಕೀಲಿ_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, ಪರಿಮಾಣ(Row),
                           ures_compareRows, pTempTable->ಕೀಲಿ_Chars,
                           ಅಸತ್ಯ, pErrorCode);
            ಯದಿ(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                ಪ್ರತ್ಯಾಗಮ;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*ಕೀಲಿಗಳು*/
            ಯದಿ(pKey16!=nullptr) {
                uint16_t *rKey16;

                ಯದಿ(pKey16!=qKey16) {
                    rKey16=qKey16;
                } ಅನ್ಯಥಾ {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                ಪ್ರತಿ(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                ಯದಿ(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } ಅನ್ಯಥಾ {
                int32_t *rKey32;

                ಯದಿ(pKey32!=qKey32) {
                    rKey32=qKey32;
                } ಅನ್ಯಥಾ {
                    rKey32=pTempTable->resort;
                }
                ಪ್ರತಿ(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                ಯದಿ(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                ಯದಿ(p!=q) {
                    r=q;
                } ಅನ್ಯಥಾ {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                ಪ್ರತಿ(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                ಯದಿ(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            ಪ್ರತಿ(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                ಯದಿ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    ಪ್ರತ್ಯಾಗಮ;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ಭಙ್ಗ;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(ಧ್ರುವ UDataSwapper *ds,
          ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
          UErrorCode *pErrorCode) {
    ಧ್ರುವ UDataInfo *pInfo;
    ಧ್ರುವ Resource *inBundle;
    Resource ಬೇರು_Res;
    int32_t headerSize, maxTableLength;

    Row rows[ರಾಶಿ_ROW_CAPACITY];
    int32_t resort[ರಾಶಿ_ROW_CAPACITY];
    TempTable tempTable;

    ಧ್ರುವ int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, ಕೀಲಿಗಳು_Bottom, ಕೀಲಿಗಳು_ತುದಿ, resBottom, ತುದಿ;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* check data format and format version */
    pInfo=(ಧ್ರುವ UDataInfo *)((ಧ್ರುವ ವರ್ಣ *)inData+4);
    ಯದಿ(!(
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
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    ಯದಿ(length<0) {
        bundleLength=-1;
    } ಅನ್ಯಥಾ {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        ಯದಿ(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }

    inBundle=(ಧ್ರುವ Resource *)((ಧ್ರುವ ವರ್ಣ *)inData+headerSize);
    ಬೇರು_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(ಧ್ರುವ int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    ಯದಿ(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಕೀಲಿಗಳು_Bottom=1+indexLength;
    ಕೀಲಿಗಳು_ತುದಿ=udata_readInt32(ds, inIndexes[URES_INDEX_ಕೀಲಿಗಳು_ತುದಿ]);
    ಯದಿ(indexLength>URES_INDEX_16_BIT_ತುದಿ) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_ತುದಿ]);
    } ಅನ್ಯಥಾ {
        resBottom=ಕೀಲಿಗಳು_ತುದಿ;
    }
    ತುದಿ=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_ತುದಿ]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    ಯದಿ(0<=bundleLength && bundleLength<ತುದಿ) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         ತುದಿ, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(ಕೀಲಿಗಳು_ತುದಿ>(1+indexLength)) {
        tempTable.local_ಕೀಲಿ_Limit=ಕೀಲಿಗಳು_ತುದಿ<<2;
    } ಅನ್ಯಥಾ {
        tempTable.local_ಕೀಲಿ_Limit=0;
    }

    ಯದಿ(length>=0) {
        Resource *outBundle=(Resource *)((ವರ್ಣ *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t ರಾಶಿ_Res_Flags[ರಾಶಿ_ROW_CAPACITY];
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
        ಯದಿ(resFlagsLength<=(int32_t)ಪರಿಮಾಣ(ರಾಶಿ_Res_Flags)) {
            tempTable.resFlags=ರಾಶಿ_Res_Flags;
        } ಅನ್ಯಥಾ {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            ಯದಿ(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        ಯದಿ(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*ತುದಿ);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+ಕೀಲಿಗಳು_Bottom, 4*(ಕೀಲಿಗಳು_ತುದಿ-ಕೀಲಿಗಳು_Bottom),
                                    outBundle+ಕೀಲಿಗಳು_Bottom, pErrorCode);
        ಯದಿ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(ಕೀಲಿಗಳು_ತುದಿ-ಕೀಲಿಗಳು_Bottom));
            ಯದಿ(tempTable.resFlags!=ರಾಶಿ_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        ಯದಿ(ಕೀಲಿಗಳು_ತುದಿ<resBottom) {
            ds->swapArray16(ds, inBundle+ಕೀಲಿಗಳು_ತುದಿ, (resBottom-ಕೀಲಿಗಳು_ತುದಿ)*4, outBundle+ಕೀಲಿಗಳು_ತುದಿ, pErrorCode);
            ಯದಿ(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-ಕೀಲಿಗಳು_ತುದಿ));
                ಯದಿ(tempTable.resFlags!=ರಾಶಿ_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                ಪ್ರತ್ಯಾಗಮ 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.ಕೀಲಿ_Chars=(ಧ್ರುವ ವರ್ಣ *)outBundle; /* sort by outCharset */
        ಯದಿ(tempTable.majorFormatVersion>1 || maxTableLength<=ರಾಶಿ_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } ಅನ್ಯಥಾ {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*ಪರಿಮಾಣ(Row)+maxTableLength*4);
            ಯದಿ(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ಯದಿ(tempTable.resFlags!=ರಾಶಿ_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, ಬೇರು_Res, nullptr, &tempTable, pErrorCode);
        ಯದಿ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             ಬೇರು_Res);
        }

        ಯದಿ(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        ಯದಿ(tempTable.resFlags!=ರಾಶಿ_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, ಕೀಲಿಗಳು_Bottom*4, outBundle, pErrorCode);
    }

    ಪ್ರತ್ಯಾಗಮ headerSize+4*ತುದಿ;
}

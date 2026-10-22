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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "resource.h"
#અન્તર્ભાવ "uarrsort.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucol_swp.h"
#અન્તર્ભાવ "udataswp.h"
#અન્તર્ભાવ "uinvchar.h"
#અન્તર્ભાવ "uresdata.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#define RES_GET_KEY16(pResData, ચાવી_Offset) \
    ((ચાવી_Offset)<(pResData)->local_ચાવી_Limit ? \
        (અચલ અક્ષર *)(pResData)->p_મૂળ+(ચાવી_Offset) : \
        (pResData)->pool_Bundle_ચાવીઓ+(ચાવી_Offset)-(pResData)->local_ચાવી_Limit)

#define RES_GET_KEY32(pResData, ચાવી_Offset) \
    ((ચાવી_Offset)>=0 ? \
        (અચલ અક્ષર *)(pResData)->p_મૂળ+(ચાવી_Offset) : \
        (pResData)->pool_Bundle_ચાવીઓ+((ચાવી_Offset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
સ્થિર અચલ uint16_t gEmpty16=0;

સ્થિર અચલ સંરચના {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

સ્થિર અચલ સંરચના {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

સ્થિર int32_t
_res_findTableItem(અચલ ResourceData *pResData, અચલ uint16_t *ચાવી_Offsets, int32_t length,
                   અચલ અક્ષર *ચાવી, અચલ અક્ષર **real_ચાવી) {
    અચલ અક્ષર *table_ચાવી;
    int32_t mid, start, limit;
    પૂર્ણાંક result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    જ્યારે(start<limit) {
        mid = (start + limit) / 2;
        table_ચાવી = RES_GET_KEY16(pResData, ચાવી_Offsets[mid]);
        જો (pResData->useNativeStrcmp) {
            result = uprv_strcmp(ચાવી, table_ચાવી);
        } નહીંતર {
            result = uprv_compareInvCharsAsAscii(ચાવી, table_ચાવી);
        }
        જો (result < 0) {
            limit = mid;
        } નહીંતર જો (result > 0) {
            start = mid + 1;
        } નહીંતર {
            /* We found it! */
            *real_ચાવી=table_ચાવી;
            પાછા mid;
        }
    }
    પાછા URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

સ્થિર int32_t
_res_findTable32Item(અચલ ResourceData *pResData, અચલ int32_t *ચાવી_Offsets, int32_t length,
                     અચલ અક્ષર *ચાવી, અચલ અક્ષર **real_ચાવી) {
    અચલ અક્ષર *table_ચાવી;
    int32_t mid, start, limit;
    પૂર્ણાંક result;

    /* do a binary search for the key */
    start=0;
    limit=length;
    જ્યારે(start<limit) {
        mid = (start + limit) / 2;
        table_ચાવી = RES_GET_KEY32(pResData, ચાવી_Offsets[mid]);
        જો (pResData->useNativeStrcmp) {
            result = uprv_strcmp(ચાવી, table_ચાવી);
        } નહીંતર {
            result = uprv_compareInvCharsAsAscii(ચાવી, table_ચાવી);
        }
        જો (result < 0) {
            limit = mid;
        } નહીંતર જો (result > 0) {
            start = mid + 1;
        } નહીંતર {
            /* We found it! */
            *real_ચાવી=table_ચાવી;
            પાછા mid;
        }
    }
    પાછા URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

સ્થિર UBool U_CALLCONV
isAcceptable(શૂન્ય *context,
             અચલ અક્ષર * /*type*/, અચલ અક્ષર * /*name*/,
             અચલ UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    પાછા
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

સ્થિર શૂન્ય
res_init(ResourceData *pResData,
         UVersionInfo formatVersion, અચલ શૂન્ય *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType મૂળ_Type;

    /* get the root resource */
    pResData->p_મૂળ = static_cast<અચલ int32_t*>(inBytes);
    pResData->મૂળ_Res = static_cast<Resource>(*pResData->p_મૂળ);
    pResData->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    જો(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        પાછા;
    }

    /* currently, we accept only resources that have a Table as their roots */
    મૂળ_Type = static_cast<UResType>(RES_GET_TYPE(pResData->મૂળ_Res));
    જો(!URES_IS_TABLE(મૂળ_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(pResData);
        પાછા;
    }

    જો(formatVersion[0]==1 && formatVersion[1]==0) {
        pResData->local_ચાવી_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } નહીંતર {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        અચલ int32_t *indexes=pResData->p_મૂળ+1;
        int32_t indexLength=indexes[URES_INDEX_LENGTH]&0xff;
        જો(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            પાછા;
        }
        જો( length>=0 &&
            (length<((1+indexLength)<<2) ||
             length<(indexes[URES_INDEX_BUNDLE_ટોચ]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            પાછા;
        }
        જો(indexes[URES_INDEX_ચાવીઓ_ટોચ]>(1+indexLength)) {
            pResData->local_ચાવી_Limit=indexes[URES_INDEX_ચાવીઓ_ટોચ]<<2;
        }
        જો(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            pResData->poolStringIndexLimit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_INDEX_LENGTH]) >> 8);
        }
        જો(indexLength>URES_INDEX_ATTRIBUTES) {
            int32_t att=indexes[URES_INDEX_ATTRIBUTES];
            pResData->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            pResData->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            pResData->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            pResData->poolStringIndexLimit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            pResData->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        જો((pResData->isPoolBundle || pResData->usesPoolBundle) && indexLength<=URES_INDEX_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(pResData);
            પાછા;
        }
        જો( indexLength>URES_INDEX_16_BIT_ટોચ &&
            indexes[URES_INDEX_16_BIT_ટોચ]>indexes[URES_INDEX_ચાવીઓ_ટોચ]
        ) {
            pResData->p16BitUnits = reinterpret_cast<અચલ uint16_t*>(pResData->p_મૂળ + indexes[URES_INDEX_ચાવીઓ_ટોચ]);
        }
    }

    જો(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        pResData->useNativeStrcmp=સાચું;
    }
}

U_CAPI શૂન્ય U_EXPORT2
res_read(ResourceData *pResData,
         અચલ UDataInfo *pInfo, અચલ શૂન્ય *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, કદ(ResourceData));
    જો(U_FAILURE(*errorCode)) {
        પાછા;
    }
    જો(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        પાછા;
    }
    res_init(pResData, formatVersion, inBytes, length, errorCode);
}

U_CFUNC શૂન્ય
res_load(ResourceData *pResData,
         અચલ અક્ષર *path, અચલ અક્ષર *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(pResData, 0, કદ(ResourceData));

    /* load the ResourceBundle file */
    pResData->data=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    જો(U_FAILURE(*errorCode)) {
        પાછા;
    }

    /* get its memory and initialize *pResData */
    res_init(pResData, formatVersion, udata_getMemory(pResData->data), -1, errorCode);
}

U_CFUNC શૂન્ય
res_unload(ResourceData *pResData) {
    જો(pResData->data!=nullptr) {
        udata_close(pResData->data);
        pResData->data=nullptr;
    }
}

સ્થિર અચલ int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_દ્વિઅંકી,
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
    પાછા (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI અચલ char16_t * U_EXPORT2
res_getStringNoTrace(અચલ ResourceData *pResData, Resource res, int32_t *pLength) {
    અચલ char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    જો(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        જો((int32_t)offset<pResData->poolStringIndexLimit) {
            p=(અચલ char16_t *)pResData->poolBundleStrings+offset;
        } નહીંતર {
            p=(અચલ char16_t *)pResData->p16BitUnits+(offset-pResData->poolStringIndexLimit);
        }
        first=*p;
        જો(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } નહીંતર જો(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } નહીંતર જો(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } નહીંતર {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } નહીંતર જો(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        અચલ int32_t *p32= res==0 ? &gEmptyString.length : pResData->p_મૂળ+res;
        length=*p32++;
        p=(અચલ char16_t *)p32;
    } નહીંતર {
        p=nullptr;
        length=0;
    }
    જો(pLength) {
        *pLength=length;
    }
    પાછા p;
}

નામાકાશ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(અચલ ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    જો (offset == 0) {
        // empty string
    } નહીંતર જો (res == offset) {
        અચલ int32_t *p32=pResData->p_મૂળ+res;
        int32_t length=*p32;
        અચલ char16_t* p = reinterpret_cast<અચલ char16_t*>(p32);
        પાછા length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } નહીંતર જો (RES_GET_TYPE(res) == URES_STRING_V2) {
        અચલ char16_t *p;
        જો (static_cast<int32_t>(offset) < pResData->poolStringIndexLimit) {
            p = reinterpret_cast<અચલ char16_t*>(pResData->poolBundleStrings) + offset;
        } નહીંતર {
            p = reinterpret_cast<અચલ char16_t*>(pResData->p16BitUnits) + (offset - pResData->poolStringIndexLimit);
        }
        int32_t first=*p;
        જો (first == 0x2205) {  // implicit length
            પાછા p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } નહીંતર જો (first == 0xdc03) {  // explicit length 3 (should not occur)
            પાછા p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } નહીંતર {
            // Assume that the string has not been stored with more length units than necessary.
            પાછા ખોટું;
        }
    }
    પાછા ખોટું;
}

int32_t getStringArray(અચલ ResourceData *pResData, અચલ icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    જો(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    int32_t length = array.getSize();
    જો(length == 0) {
        પાછા 0;
    }
    જો(length > capacity) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        પાછા length;
    }
    માટે(int32_t i = 0; i < length; ++i) {
        int32_t sLength;
        // No tracing: handled by the caller
        અચલ char16_t *s = res_getStringNoTrace(pResData, array.internalGetResource(pResData, i), &sLength);
        જો(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            પાછા 0;
        }
        dest[i].setTo(સાચું, s, sLength);
    }
    પાછા length;
}

}  // namespace

U_CAPI અચલ char16_t * U_EXPORT2
res_getAlias(અચલ ResourceData *pResData, Resource res, int32_t *pLength) {
    અચલ char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    જો(RES_GET_TYPE(res)==URES_ALIAS) {
        અચલ int32_t *p32= offset==0 ? &gEmptyString.length : pResData->p_મૂળ+offset;
        length=*p32++;
        p=(અચલ char16_t *)p32;
    } નહીંતર {
        p=nullptr;
        length=0;
    }
    જો(pLength) {
        *pLength=length;
    }
    પાછા p;
}

U_CAPI અચલ uint8_t * U_EXPORT2
res_get_દ્વિઅંકી_No_Trace(અચલ ResourceData *pResData, Resource res, int32_t *pLength) {
    અચલ uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    જો(RES_GET_TYPE(res)==URES_દ્વિઅંકી) {
        અચલ int32_t *p32= offset==0 ? (અચલ int32_t*)&gEmpty32 : pResData->p_મૂળ+offset;
        length=*p32++;
        p=(અચલ uint8_t *)p32;
    } નહીંતર {
        p=nullptr;
        length=0;
    }
    જો(pLength) {
        *pLength=length;
    }
    પાછા p;
}


U_CAPI અચલ int32_t * U_EXPORT2
res_getIntVectorNoTrace(અચલ ResourceData *pResData, Resource res, int32_t *pLength) {
    અચલ int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    જો(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (અચલ int32_t *)&gEmpty32 : pResData->p_મૂળ+offset;
        length=*p++;
    } નહીંતર {
        p=nullptr;
        length=0;
    }
    જો(pLength) {
        *pLength=length;
    }
    પાછા p;
}

U_CAPI int32_t U_EXPORT2
res_ગણતરી_Array_વસ્તુઓ(અચલ ResourceData *pResData, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    પસંદગી(RES_GET_TYPE(res)) {
    સ્થિતિ URES_STRING:
    સ્થિતિ URES_STRING_V2:
    સ્થિતિ URES_દ્વિઅંકી:
    સ્થિતિ URES_ALIAS:
    સ્થિતિ URES_INT:
    સ્થિતિ URES_INT_VECTOR:
        પાછા 1;
    સ્થિતિ URES_ARRAY:
    સ્થિતિ URES_TABLE32:
        પાછા offset==0 ? 0 : *(pResData->p_મૂળ+offset);
    સ્થિતિ URES_TABLE:
        પાછા offset==0 ? 0 : *((અચલ uint16_t *)(pResData->p_મૂળ+offset));
    સ્થિતિ URES_ARRAY16:
    સ્થિતિ URES_TABLE16:
        પાછા pResData->p16BitUnits[offset];
    મૂળભૂત:
        પાછા 0;
    }
}

U_NAMESPACE_BEGIN

Resource_Data_કિંમત::~Resource_Data_કિંમત() {}

UResType Resource_Data_કિંમત::getType() અચલ {
    પાછા res_getPublicType(res);
}

અચલ char16_t *Resource_Data_કિંમત::getString(int32_t &length, UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    અચલ char16_t *s = res_getString(fTraceInfo, &getData(), res, &length);
    જો(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    પાછા s;
}

અચલ char16_t *Resource_Data_કિંમત::getAliasString(int32_t &length, UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    અચલ char16_t *s = res_getAlias(&getData(), res, &length);
    જો(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    પાછા s;
}

int32_t Resource_Data_કિંમત::getInt(UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    જો(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    પાછા res_getInt(fTraceInfo, res);
}

uint32_t Resource_Data_કિંમત::getUInt(UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    જો(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    પાછા res_getUInt(fTraceInfo, res);
}

અચલ int32_t *Resource_Data_કિંમત::getIntVector(int32_t &length, UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    અચલ int32_t *iv = res_getIntVector(fTraceInfo, &getData(), res, &length);
    જો(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    પાછા iv;
}

અચલ uint8_t *Resource_Data_કિંમત::get_દ્વિઅંકી(int32_t &length, UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    અચલ uint8_t *b = res_get_દ્વિઅંકી(fTraceInfo, &getData(), res, &length);
    જો(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    પાછા b;
}

ResourceArray Resource_Data_કિંમત::getArray(UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        પાછા {};
    }
    અચલ uint16_t *items16 = nullptr;
    અચલ Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    પસંદગી(RES_GET_TYPE(res)) {
    સ્થિતિ URES_ARRAY:
        જો (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<અચલ Resource*>(getData().p_મૂળ) + offset;
            length = *items32++;
        }
        તોડો;
    સ્થિતિ URES_ARRAY16:
        items16 = getData().p16BitUnits+offset;
        length = *items16++;
        તોડો;
    મૂળભૂત:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        પાછા {};
    }
    પાછા ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_Data_કિંમત::getTable(UErrorCode &errorCode) અચલ {
    જો(U_FAILURE(errorCode)) {
        પાછા {};
    }
    અચલ uint16_t *keys16 = nullptr;
    અચલ int32_t *keys32 = nullptr;
    અચલ uint16_t *items16 = nullptr;
    અચલ Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    પસંદગી(RES_GET_TYPE(res)) {
    સ્થિતિ URES_TABLE:
        જો (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<અચલ uint16_t*>(getData().p_મૂળ + offset);
            length = *keys16++;
            items32 = reinterpret_cast<અચલ Resource*>(keys16 + length + (~length & 1));
        }
        તોડો;
    સ્થિતિ URES_TABLE16:
        keys16 = getData().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        તોડો;
    સ્થિતિ URES_TABLE32:
        જો (offset != 0) {  // empty if offset==0
            keys32 = getData().p_મૂળ+offset;
            length = *keys32++;
            items32 = reinterpret_cast<અચલ Resource*>(keys32) + length;
        }
        તોડો;
    મૂળભૂત:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        પાછા {};
    }
    પાછા ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_Data_કિંમત::isNoInheritanceMarker() અચલ {
    પાછા ::isNoInheritanceMarker(&getData(), res);
}

int32_t Resource_Data_કિંમત::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) અચલ {
    પાછા ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_Data_કિંમત::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) અચલ {
    જો(URES_IS_ARRAY(res)) {
        પાછા ::getStringArray(&getData(), getArray(errorCode), dest, capacity, errorCode);
    }
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    જો(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    જો(capacity < 1) {
        errorCode = U_BUFFER_OVERFLOW_ERROR;
        પાછા 1;
    }
    int32_t sLength;
    અચલ char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    જો(s != nullptr) {
        dest[0].setTo(સાચું, s, sLength);
        પાછા 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    પાછા 0;
}

UnicodeString Resource_Data_કિંમત::getStringOrFirstOfArray(UErrorCode &errorCode) અચલ {
    UnicodeString us;
    જો(U_FAILURE(errorCode)) {
        પાછા us;
    }
    int32_t sLength;
    અચલ char16_t *s = res_getString(fTraceInfo, &getData(), res, &sLength);
    જો(s != nullptr) {
        us.setTo(સાચું, s, sLength);
        પાછા us;
    }
    ResourceArray array = getArray(errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા us;
    }
    જો(array.getSize() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&getData(), array.internalGetResource(&getData(), 0), &sLength);
        જો(s != nullptr) {
            us.setTo(સાચું, s, sLength);
            પાછા us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    પાછા us;
}

U_NAMESPACE_END

સ્થિર Resource
makeResourceFrom16(અચલ ResourceData *pResData, int32_t res16) {
    જો(res16<pResData->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } નહીંતર {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-pResData->poolStringIndex16Limit+pResData->poolStringIndexLimit;
    }
    પાછા URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_ચાવી(અચલ ResourceData *pResData, Resource table,
                      int32_t *indexR, અચલ અક્ષર **ચાવી) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    જો(ચાવી == nullptr || *ચાવી == nullptr) {
        પાછા RES_BOGUS;
    }
    પસંદગી(RES_GET_TYPE(table)) {
    સ્થિતિ URES_TABLE: {
        જો (offset!=0) { /* empty if offset==0 */
            અચલ uint16_t *p= (અચલ uint16_t *)(pResData->p_મૂળ+offset);
            length=*p++;
            *indexR=idx=_res_findTableItem(pResData, p, length, *ચાવી, ચાવી);
            જો(idx>=0) {
                અચલ Resource *p32=(અચલ Resource *)(p+length+(~length&1));
                પાછા p32[idx];
            }
        }
        તોડો;
    }
    સ્થિતિ URES_TABLE16: {
        અચલ uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        *indexR=idx=_res_findTableItem(pResData, p, length, *ચાવી, ચાવી);
        જો(idx>=0) {
            પાછા makeResourceFrom16(pResData, p[length+idx]);
        }
        તોડો;
    }
    સ્થિતિ URES_TABLE32: {
        જો (offset!=0) { /* empty if offset==0 */
            અચલ int32_t *p= pResData->p_મૂળ+offset;
            length=*p++;
            *indexR=idx=_res_findTable32Item(pResData, p, length, *ચાવી, ચાવી);
            જો(idx>=0) {
                પાછા (Resource)p[length+idx];
            }
        }
        તોડો;
    }
    મૂળભૂત:
        તોડો;
    }
    પાછા RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getTableItemByIndex(અચલ ResourceData *pResData, Resource table,
                        int32_t indexR, અચલ અક્ષર **ચાવી) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    જો (indexR < 0) {
        પાછા RES_BOGUS;
    }
    પસંદગી(RES_GET_TYPE(table)) {
    સ્થિતિ URES_TABLE: {
        જો (offset != 0) { /* empty if offset==0 */
            અચલ uint16_t *p= (અચલ uint16_t *)(pResData->p_મૂળ+offset);
            length=*p++;
            જો(indexR<length) {
                અચલ Resource *p32=(અચલ Resource *)(p+length+(~length&1));
                જો(ચાવી!=nullptr) {
                    *ચાવી=RES_GET_KEY16(pResData, p[indexR]);
                }
                પાછા p32[indexR];
            }
        }
        તોડો;
    }
    સ્થિતિ URES_TABLE16: {
        અચલ uint16_t *p=pResData->p16BitUnits+offset;
        length=*p++;
        જો(indexR<length) {
            જો(ચાવી!=nullptr) {
                *ચાવી=RES_GET_KEY16(pResData, p[indexR]);
            }
            પાછા makeResourceFrom16(pResData, p[length+indexR]);
        }
        તોડો;
    }
    સ્થિતિ URES_TABLE32: {
        જો (offset != 0) { /* empty if offset==0 */
            અચલ int32_t *p= pResData->p_મૂળ+offset;
            length=*p++;
            જો(indexR<length) {
                જો(ચાવી!=nullptr) {
                    *ચાવી=RES_GET_KEY32(pResData, p[indexR]);
                }
                પાછા (Resource)p[length+indexR];
            }
        }
        તોડો;
    }
    મૂળભૂત:
        તોડો;
    }
    પાછા RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(અચલ ResourceData *pResData, અચલ અક્ષર *ચાવી) {
    અચલ અક્ષર *real_ચાવી=ચાવી;
    int32_t idx;
    પાછા res_get_Table_Item_By_ચાવી(pResData, pResData->મૂળ_Res, &idx, &real_ચાવી);
}


UBool icu::ResourceTable::get_ચાવી_And_કિંમત(int32_t i,
                                         અચલ અક્ષર *&ચાવી, icu::Resource_કિંમત &value) અચલ {
    જો(0 <= i && i < length) {
        icu::Resource_Data_કિંમત &rd_કિંમત = static_cast<icu::Resource_Data_કિંમત &>(value);
        જો (keys16 != nullptr) {
            ચાવી = RES_GET_KEY16(&rd_કિંમત.getData(), keys16[i]);
        } નહીંતર {
            ચાવી = RES_GET_KEY32(&rd_કિંમત.getData(), keys32[i]);
        }
        Resource res;
        જો (items16 != nullptr) {
            res = makeResourceFrom16(&rd_કિંમત.getData(), items16[i]);
        } નહીંતર {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_કિંમત.setResource(res, ResourceTracer(fTraceInfo, ચાવી));
        પાછા સાચું;
    }
    પાછા ખોટું;
}

UBool icu::ResourceTable::find_કિંમત(અચલ અક્ષર *ચાવી, Resource_કિંમત &value) અચલ {
    icu::Resource_Data_કિંમત &rd_કિંમત = static_cast<icu::Resource_Data_કિંમત &>(value);
    અચલ અક્ષર *real_ચાવી = nullptr;
    int32_t i;
    જો (keys16 != nullptr) {
        i = _res_findTableItem(&rd_કિંમત.getData(), keys16, length, ચાવી, &real_ચાવી);
    } નહીંતર {
        i = _res_findTable32Item(&rd_કિંમત.getData(), keys32, length, ચાવી, &real_ચાવી);
    }
    જો (i >= 0) {
        Resource res;
        જો (items16 != nullptr) {
            res = makeResourceFrom16(&rd_કિંમત.getData(), items16[i]);
        } નહીંતર {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_કિંમત.setResource(res, ResourceTracer(fTraceInfo, ચાવી));
        પાછા સાચું;
    }
    પાછા ખોટું;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(અચલ ResourceData *pResData, Resource array, int32_t indexR) {
    uint32_t offset=RES_GET_OFFSET(array);
    જો (indexR < 0) {
        પાછા RES_BOGUS;
    }
    પસંદગી(RES_GET_TYPE(array)) {
    સ્થિતિ URES_ARRAY: {
        જો (offset!=0) { /* empty if offset==0 */
            અચલ int32_t *p= pResData->p_મૂળ+offset;
            જો(indexR<*p) {
                પાછા (Resource)p[1+indexR];
            }
        }
        તોડો;
    }
    સ્થિતિ URES_ARRAY16: {
        અચલ uint16_t *p=pResData->p16BitUnits+offset;
        જો(indexR<*p) {
            પાછા makeResourceFrom16(pResData, p[1+indexR]);
        }
        તોડો;
    }
    મૂળભૂત:
        તોડો;
    }
    પાછા RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(અચલ ResourceData *pResData, int32_t i) અચલ {
    જો (items16 != nullptr) {
        પાછા makeResourceFrom16(pResData, items16[i]);
    } નહીંતર {
        પાછા items32[i];
    }
}

UBool icu::ResourceArray::get_કિંમત(int32_t i, icu::Resource_કિંમત &value) અચલ {
    જો(0 <= i && i < length) {
        icu::Resource_Data_કિંમત &rd_કિંમત = static_cast<icu::Resource_Data_કિંમત &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_કિંમત.setResource(
            internalGetResource(&rd_કિંમત.getData(), i),
            ResourceTracer(fTraceInfo, i));
        પાછા સાચું;
    }
    પાછા ખોટું;
}

U_CFUNC Resource
res_findResource(અચલ ResourceData *pResData, Resource r, અક્ષર** path, અચલ અક્ષર** ચાવી) {
  અક્ષર *pathP = *path, *nextSepP = *path;
  અક્ષર *closeIndex = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t indexR = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  જો(!uprv_strlen(pathP)) {
      પાછા r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  જો(!URES_IS_CONTAINER(type)) {
      પાછા RES_BOGUS;
  }
  
  જ્યારે(nextSepP && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    nextSepP = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    જો(nextSepP != nullptr) {
      જો(nextSepP == pathP) {
        // Empty key string.
        પાછા RES_BOGUS;
      }
      *nextSepP = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = nextSepP+1;
    } નહીંતર {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    જો(URES_IS_TABLE(type)) {
      *ચાવી = pathP;
      t2 = res_get_Table_Item_By_ચાવી(pResData, t1, &indexR, ચાવી);
    } નહીંતર જો(URES_IS_ARRAY(type)) {
      indexR = uprv_strtol(pathP, &closeIndex, 10);
      જો(indexR >= 0 && *closeIndex == 0) {
        t2 = res_getArrayItem(pResData, t1, indexR);
      } નહીંતર {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *ચાવી = nullptr;
    } નહીંતર { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  પાછા t1;
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
પ્રકારવ્યાખ્યા સંરચના Row {
    int32_t ચાવી_Index, sortIndex;
} Row;

સ્થિર int32_t U_CALLCONV
ures_compareRows(અચલ શૂન્ય *context, અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    અચલ અક્ષર* ચાવી_Chars = static_cast<અચલ અક્ષર*>(context);
    પાછા static_cast<int32_t>(uprv_strcmp(ચાવી_Chars + static_cast<અચલ Row*>(left)->ચાવી_Index,
                                            ચાવી_Chars + static_cast<અચલ Row*>(જમણું)->ચાવી_Index));
}

પ્રકારવ્યાખ્યા સંરચના TempTable {
    અચલ અક્ષર *ચાવી_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_ચાવી_Limit;
    uint8_t majorFormatVersion;
} TempTable;

ગણના {
    ઢગલો_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
સ્થિર અચલ અક્ષર *અચલ g_Unknown_ચાવી="";

#જો !UCONFIG_NO_COLLATION
// resource table key for collation binaries
સ્થિર અચલ char16_t g_Collation_Bin_ચાવી[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
સ્થિર શૂન્ય
ures_swapResource(અચલ UDataSwapper *ds,
                  અચલ Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  અચલ અક્ષર *ચાવી,
                  TempTable *pTempTable,
                  UErrorCode *pErrorCode) {
    અચલ Resource *p;
    Resource *q;
    int32_t offset, count;

    પસંદગી(RES_GET_TYPE(res)) {
    સ્થિતિ URES_TABLE16:
    સ્થિતિ URES_STRING_V2:
    સ્થિતિ URES_INT:
    સ્થિતિ URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        પાછા;
    મૂળભૂત:
        તોડો;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    જો(offset==0) {
        /* special offset indicating an empty item */
        પાછા;
    }
    જો (pTempTable->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        પાછા;
    } નહીંતર {
        /* mark it as swapped now */
        pTempTable->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    પસંદગી(RES_GET_TYPE(res)) {
    સ્થિતિ URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    સ્થિતિ URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        તોડો;
    સ્થિતિ URES_દ્વિઅંકી:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#જો !UCONFIG_NO_COLLATION
        જો( ચાવી!=nullptr &&  /* the binary is in a table */
            (ચાવી!=g_Unknown_ચાવી ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, ચાવી, -1,
                                       g_Collation_Bin_ચાવી, UPRV_LENGTHOF(g_Collation_Bin_ચાવી)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_દ્વિઅંકી(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#endif
        તોડો;
    સ્થિતિ URES_TABLE:
    સ્થિતિ URES_TABLE32:
        {
            અચલ uint16_t *pKey16;
            uint16_t *qKey16;

            અચલ int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, oldIndex;

            જો(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<અચલ uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } નહીંતર {
                /* get table item count */
                pKey32 = reinterpret_cast<અચલ int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            જો(count==0) {
                તોડો;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            માટે(i=0; i<count; ++i) {
                અચલ અક્ષર *item_ચાવી=g_Unknown_ચાવી;
                જો(pKey16!=nullptr) {
                    int32_t ચાવી_Offset=ds->readUInt16(pKey16[i]);
                    જો(ચાવી_Offset<pTempTable->local_ચાવી_Limit) {
                        item_ચાવી = reinterpret_cast<અચલ અક્ષર*>(outBundle) + ચાવી_Offset;
                    }
                } નહીંતર {
                    int32_t ચાવી_Offset=udata_readInt32(ds, pKey32[i]);
                    જો(ચાવી_Offset>=0) {
                        item_ચાવી = reinterpret_cast<અચલ અક્ષર*>(outBundle) + ચાવી_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_ચાવી, pTempTable, pErrorCode);
                જો(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    પાછા;
                }
            }

            જો(pTempTable->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                જો(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } નહીંતર {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                તોડો;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            જો(pKey16!=nullptr) {
                માટે(i=0; i<count; ++i) {
                    pTempTable->rows[i].ચાવી_Index=ds->readUInt16(pKey16[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            } નહીંતર {
                માટે(i=0; i<count; ++i) {
                    pTempTable->rows[i].ચાવી_Index=udata_readInt32(ds, pKey32[i]);
                    pTempTable->rows[i].sortIndex=i;
                }
            }
            uprv_sortArray(pTempTable->rows, count, કદ(Row),
                           ures_compareRows, pTempTable->ચાવી_Chars,
                           ખોટું, pErrorCode);
            જો(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                પાછા;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*ચાવીઓ*/
            જો(pKey16!=nullptr) {
                uint16_t *rKey16;

                જો(pKey16!=qKey16) {
                    rKey16=qKey16;
                } નહીંતર {
                    rKey16 = reinterpret_cast<uint16_t*>(pTempTable->resort);
                }
                માટે(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray16(ds, pKey16+oldIndex, 2, rKey16+i, pErrorCode);
                }
                જો(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } નહીંતર {
                int32_t *rKey32;

                જો(pKey32!=qKey32) {
                    rKey32=qKey32;
                } નહીંતર {
                    rKey32=pTempTable->resort;
                }
                માટે(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, pKey32+oldIndex, 4, rKey32+i, pErrorCode);
                }
                જો(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                જો(p!=q) {
                    r=q;
                } નહીંતર {
                    r = reinterpret_cast<Resource*>(pTempTable->resort);
                }
                માટે(i=0; i<count; ++i) {
                    oldIndex=pTempTable->rows[i].sortIndex;
                    ds->swapArray32(ds, p+oldIndex, 4, r+i, pErrorCode);
                }
                જો(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        તોડો;
    સ્થિતિ URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            માટે(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, pTempTable, pErrorCode);
                જો(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    પાછા;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        તોડો;
    સ્થિતિ URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        તોડો;
    મૂળભૂત:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        તોડો;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(અચલ UDataSwapper *ds,
          અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
          UErrorCode *pErrorCode) {
    અચલ UDataInfo *pInfo;
    અચલ Resource *inBundle;
    Resource મૂળ_Res;
    int32_t headerSize, maxTableLength;

    Row rows[ઢગલો_ROW_CAPACITY];
    int32_t resort[ઢગલો_ROW_CAPACITY];
    TempTable tempTable;

    અચલ int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundleLength, indexLength, ચાવીઓ_Bottom, ચાવીઓ_ટોચ, resBottom, ટોચ;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }

    /* check data format and format version */
    pInfo=(અચલ UDataInfo *)((અચલ અક્ષર *)inData+4);
    જો(!(
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
        પાછા 0;
    }
    tempTable.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    જો(length<0) {
        bundleLength=-1;
    } નહીંતર {
        bundleLength=(length-headerSize)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        જો(bundleLength<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-headerSize);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
    }

    inBundle=(અચલ Resource *)((અચલ અક્ષર *)inData+headerSize);
    મૂળ_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(અચલ int32_t *)(inBundle+1);

    indexLength=udata_readInt32(ds, inIndexes[URES_INDEX_LENGTH])&0xff;
    જો(indexLength<=URES_INDEX_MAX_TABLE_LENGTH) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }
    ચાવીઓ_Bottom=1+indexLength;
    ચાવીઓ_ટોચ=udata_readInt32(ds, inIndexes[URES_INDEX_ચાવીઓ_ટોચ]);
    જો(indexLength>URES_INDEX_16_BIT_ટોચ) {
        resBottom=udata_readInt32(ds, inIndexes[URES_INDEX_16_BIT_ટોચ]);
    } નહીંતર {
        resBottom=ચાવીઓ_ટોચ;
    }
    ટોચ=udata_readInt32(ds, inIndexes[URES_INDEX_BUNDLE_ટોચ]);
    maxTableLength=udata_readInt32(ds, inIndexes[URES_INDEX_MAX_TABLE_LENGTH]);

    જો(0<=bundleLength && bundleLength<ટોચ) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         ટોચ, bundleLength);
        *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }
    જો(ચાવીઓ_ટોચ>(1+indexLength)) {
        tempTable.local_ચાવી_Limit=ચાવીઓ_ટોચ<<2;
    } નહીંતર {
        tempTable.local_ચાવી_Limit=0;
    }

    જો(length>=0) {
        Resource *outBundle=(Resource *)((અક્ષર *)outData+headerSize);

        /* track which resources we have already swapped */
        uint32_t ઢગલો_Res_Flags[ઢગલો_ROW_CAPACITY];
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
        જો(resFlagsLength<=(int32_t)કદ(ઢગલો_Res_Flags)) {
            tempTable.resFlags=ઢગલો_Res_Flags;
        } નહીંતર {
            tempTable.resFlags=(uint32_t *)uprv_malloc(resFlagsLength);
            જો(tempTable.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                પાછા 0;
            }
        }
        uprv_memset(tempTable.resFlags, 0, resFlagsLength);

        /* copy the bundle for binary and inaccessible data */
        જો(inData!=outData) {
            uprv_memcpy(outBundle, inBundle, 4*ટોચ);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+ચાવીઓ_Bottom, 4*(ચાવીઓ_ટોચ-ચાવીઓ_Bottom),
                                    outBundle+ચાવીઓ_Bottom, pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(ચાવીઓ_ટોચ-ચાવીઓ_Bottom));
            જો(tempTable.resFlags!=ઢગલો_Res_Flags) {
                uprv_free(tempTable.resFlags);
            }
            પાછા 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        જો(ચાવીઓ_ટોચ<resBottom) {
            ds->swapArray16(ds, inBundle+ચાવીઓ_ટોચ, (resBottom-ચાવીઓ_ટોચ)*4, outBundle+ચાવીઓ_ટોચ, pErrorCode);
            જો(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-ચાવીઓ_ટોચ));
                જો(tempTable.resFlags!=ઢગલો_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                પાછા 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        tempTable.ચાવી_Chars=(અચલ અક્ષર *)outBundle; /* sort by outCharset */
        જો(tempTable.majorFormatVersion>1 || maxTableLength<=ઢગલો_ROW_CAPACITY) {
            tempTable.rows=rows;
            tempTable.resort=resort;
        } નહીંતર {
            tempTable.rows=(Row *)uprv_malloc(maxTableLength*કદ(Row)+maxTableLength*4);
            જો(tempTable.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 maxTableLength);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                જો(tempTable.resFlags!=ઢગલો_Res_Flags) {
                    uprv_free(tempTable.resFlags);
                }
                પાછા 0;
            }
            tempTable.resort=(int32_t *)(tempTable.rows+maxTableLength);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, મૂળ_Res, nullptr, &tempTable, pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             મૂળ_Res);
        }

        જો(tempTable.rows!=rows) {
            uprv_free(tempTable.rows);
        }
        જો(tempTable.resFlags!=ઢગલો_Res_Flags) {
            uprv_free(tempTable.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, ચાવીઓ_Bottom*4, outBundle, pErrorCode);
    }

    પાછા headerSize+4*ટોચ;
}

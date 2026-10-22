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

#समावेश "unicode/utypes.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "resource.h"
#समावेश "uarrsort.h"
#समावेश "uassert.h"
#समावेश "ucol_swp.h"
#समावेश "udataswp.h"
#समावेश "uinvchar.h"
#समावेश "uresdata.h"
#समावेश "uresimp.h"
#समावेश "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#घोषणा RES_GET_KEY16(p_Res_तथ्य, कुंजी_Offset) \
    ((कुंजी_Offset)<(p_Res_तथ्य)->local_कुंजी_Limit ? \
        (स्थिर अक्षर *)(p_Res_तथ्य)->p_मूल+(कुंजी_Offset) : \
        (p_Res_तथ्य)->pool_Bundle_कुंजियाँ+(कुंजी_Offset)-(p_Res_तथ्य)->local_कुंजी_Limit)

#घोषणा RES_GET_KEY32(p_Res_तथ्य, कुंजी_Offset) \
    ((कुंजी_Offset)>=0 ? \
        (स्थिर अक्षर *)(p_Res_तथ्य)->p_मूल+(कुंजी_Offset) : \
        (p_Res_तथ्य)->pool_Bundle_कुंजियाँ+((कुंजी_Offset)&0x7fffffff))

#घोषणा URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
अटल स्थिर uint16_t gEmpty16=0;

अटल स्थिर काष्ठा {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

अटल स्थिर काष्ठा {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

अटल int32_t
_res_findTableItem(स्थिर Resource_तथ्य *p_Res_तथ्य, स्थिर uint16_t *कुंजी_Offsets, int32_t length,
                   स्थिर अक्षर *कुंजी, स्थिर अक्षर **real_कुंजी) {
    स्थिर अक्षर *table_कुंजी;
    int32_t mid, start, limit;
    पूर्णांक परिणाम;

    /* do a binary search for the key */
    start=0;
    limit=length;
    जबतक(start<limit) {
        mid = (start + limit) / 2;
        table_कुंजी = RES_GET_KEY16(p_Res_तथ्य, कुंजी_Offsets[mid]);
        अगर (p_Res_तथ्य->useNativeStrcmp) {
            परिणाम = uprv_strcmp(कुंजी, table_कुंजी);
        } अन्यथा {
            परिणाम = uprv_compareInvCharsAsAscii(कुंजी, table_कुंजी);
        }
        अगर (परिणाम < 0) {
            limit = mid;
        } अन्यथा अगर (परिणाम > 0) {
            start = mid + 1;
        } अन्यथा {
            /* We found it! */
            *real_कुंजी=table_कुंजी;
            वापस mid;
        }
    }
    वापस URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

अटल int32_t
_res_findTable32Item(स्थिर Resource_तथ्य *p_Res_तथ्य, स्थिर int32_t *कुंजी_Offsets, int32_t length,
                     स्थिर अक्षर *कुंजी, स्थिर अक्षर **real_कुंजी) {
    स्थिर अक्षर *table_कुंजी;
    int32_t mid, start, limit;
    पूर्णांक परिणाम;

    /* do a binary search for the key */
    start=0;
    limit=length;
    जबतक(start<limit) {
        mid = (start + limit) / 2;
        table_कुंजी = RES_GET_KEY32(p_Res_तथ्य, कुंजी_Offsets[mid]);
        अगर (p_Res_तथ्य->useNativeStrcmp) {
            परिणाम = uprv_strcmp(कुंजी, table_कुंजी);
        } अन्यथा {
            परिणाम = uprv_compareInvCharsAsAscii(कुंजी, table_कुंजी);
        }
        अगर (परिणाम < 0) {
            limit = mid;
        } अन्यथा अगर (परिणाम > 0) {
            start = mid + 1;
        } अन्यथा {
            /* We found it! */
            *real_कुंजी=table_कुंजी;
            वापस mid;
        }
    }
    वापस URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

अटल UBool U_CALLCONV
isAcceptable(व्योम *context,
             स्थिर अक्षर * /*type*/, स्थिर अक्षर * /*name*/,
             स्थिर UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    वापस
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->तथ्य_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->तथ्य_Format[1]==0x65 &&
        pInfo->तथ्य_Format[2]==0x73 &&
        pInfo->तथ्य_Format[3]==0x42 &&
        (1<=pInfo->formatVersion[0] && pInfo->formatVersion[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

अटल व्योम
res_init(Resource_तथ्य *p_Res_तथ्य,
         UVersionInfo formatVersion, स्थिर व्योम *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType मूल_Type;

    /* get the root resource */
    p_Res_तथ्य->p_मूल = static_cast<स्थिर int32_t*>(inBytes);
    p_Res_तथ्य->मूल_Res = static_cast<Resource>(*p_Res_तथ्य->p_मूल);
    p_Res_तथ्य->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    अगर(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_तथ्य);
        वापस;
    }

    /* currently, we accept only resources that have a Table as their roots */
    मूल_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_तथ्य->मूल_Res));
    अगर(!URES_IS_TABLE(मूल_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_तथ्य);
        वापस;
    }

    अगर(formatVersion[0]==1 && formatVersion[1]==0) {
        p_Res_तथ्य->local_कुंजी_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } अन्यथा {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        स्थिर int32_t *indexes=p_Res_तथ्य->p_मूल+1;
        int32_t सूचकांक_लंबाई=indexes[URES_सूचकांक_लंबाई]&0xff;
        अगर(सूचकांक_लंबाई<=URES_सूचकांक_MAX_TABLE_लंबाई) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_तथ्य);
            वापस;
        }
        अगर( length>=0 &&
            (length<((1+सूचकांक_लंबाई)<<2) ||
             length<(indexes[URES_सूचकांक_BUNDLE_शीर्ष]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_तथ्य);
            वापस;
        }
        अगर(indexes[URES_सूचकांक_कुंजियाँ_शीर्ष]>(1+सूचकांक_लंबाई)) {
            p_Res_तथ्य->local_कुंजी_Limit=indexes[URES_सूचकांक_कुंजियाँ_शीर्ष]<<2;
        }
        अगर(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_तथ्य->pool_String_सूचकांक_Limit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_सूचकांक_लंबाई]) >> 8);
        }
        अगर(सूचकांक_लंबाई>URES_सूचकांक_ATTRIBUTES) {
            int32_t att=indexes[URES_सूचकांक_ATTRIBUTES];
            p_Res_तथ्य->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_तथ्य->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_तथ्य->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            p_Res_तथ्य->pool_String_सूचकांक_Limit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_तथ्य->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        अगर((p_Res_तथ्य->isPoolBundle || p_Res_तथ्य->usesPoolBundle) && सूचकांक_लंबाई<=URES_सूचकांक_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_तथ्य);
            वापस;
        }
        अगर( सूचकांक_लंबाई>URES_सूचकांक_16_BIT_शीर्ष &&
            indexes[URES_सूचकांक_16_BIT_शीर्ष]>indexes[URES_सूचकांक_कुंजियाँ_शीर्ष]
        ) {
            p_Res_तथ्य->p16BitUnits = reinterpret_cast<स्थिर uint16_t*>(p_Res_तथ्य->p_मूल + indexes[URES_सूचकांक_कुंजियाँ_शीर्ष]);
        }
    }

    अगर(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_तथ्य->useNativeStrcmp=true;
    }
}

U_CAPI व्योम U_EXPORT2
res_read(Resource_तथ्य *p_Res_तथ्य,
         स्थिर UDataInfo *pInfo, स्थिर व्योम *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(p_Res_तथ्य, 0, माप(Resource_तथ्य));
    अगर(U_FAILURE(*errorCode)) {
        वापस;
    }
    अगर(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        वापस;
    }
    res_init(p_Res_तथ्य, formatVersion, inBytes, length, errorCode);
}

U_CFUNC व्योम
res_load(Resource_तथ्य *p_Res_तथ्य,
         स्थिर अक्षर *path, स्थिर अक्षर *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(p_Res_तथ्य, 0, माप(Resource_तथ्य));

    /* load the ResourceBundle file */
    p_Res_तथ्य->तथ्य=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    अगर(U_FAILURE(*errorCode)) {
        वापस;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_तथ्य, formatVersion, udata_getMemory(p_Res_तथ्य->तथ्य), -1, errorCode);
}

U_CFUNC व्योम
res_unload(Resource_तथ्य *p_Res_तथ्य) {
    अगर(p_Res_तथ्य->तथ्य!=nullptr) {
        udata_close(p_Res_तथ्य->तथ्य);
        p_Res_तथ्य->तथ्य=nullptr;
    }
}

अटल स्थिर int8_t gPublicTypes[URES_LIMIT] = {
    URES_STRING,
    URES_द्विआधारी,
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
    वापस (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI स्थिर char16_t * U_EXPORT2
res_getStringNoTrace(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई) {
    स्थिर char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    अगर(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        अगर((int32_t)offset<p_Res_तथ्य->pool_String_सूचकांक_Limit) {
            p=(स्थिर char16_t *)p_Res_तथ्य->poolBundleStrings+offset;
        } अन्यथा {
            p=(स्थिर char16_t *)p_Res_तथ्य->p16BitUnits+(offset-p_Res_तथ्य->pool_String_सूचकांक_Limit);
        }
        first=*p;
        अगर(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } अन्यथा अगर(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } अन्यथा अगर(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } अन्यथा {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } अन्यथा अगर(res==offset) /* RES_GET_TYPE(res)==URES_STRING */ {
        स्थिर int32_t *p32= res==0 ? &gEmptyString.length : p_Res_तथ्य->p_मूल+res;
        length=*p32++;
        p=(स्थिर char16_t *)p32;
    } अन्यथा {
        p=nullptr;
        length=0;
    }
    अगर(p_लंबाई) {
        *p_लंबाई=length;
    }
    वापस p;
}

namespace {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    अगर (offset == 0) {
        // empty string
    } अन्यथा अगर (res == offset) {
        स्थिर int32_t *p32=p_Res_तथ्य->p_मूल+res;
        int32_t length=*p32;
        स्थिर char16_t* p = reinterpret_cast<स्थिर char16_t*>(p32);
        वापस length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } अन्यथा अगर (RES_GET_TYPE(res) == URES_STRING_V2) {
        स्थिर char16_t *p;
        अगर (static_cast<int32_t>(offset) < p_Res_तथ्य->pool_String_सूचकांक_Limit) {
            p = reinterpret_cast<स्थिर char16_t*>(p_Res_तथ्य->poolBundleStrings) + offset;
        } अन्यथा {
            p = reinterpret_cast<स्थिर char16_t*>(p_Res_तथ्य->p16BitUnits) + (offset - p_Res_तथ्य->pool_String_सूचकांक_Limit);
        }
        int32_t first=*p;
        अगर (first == 0x2205) {  // implicit length
            वापस p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } अन्यथा अगर (first == 0xdc03) {  // explicit length 3 (should not occur)
            वापस p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } अन्यथा {
            // Assume that the string has not been stored with more length units than necessary.
            वापस false;
        }
    }
    वापस false;
}

int32_t getStringArray(स्थिर Resource_तथ्य *p_Res_तथ्य, स्थिर icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    अगर(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    int32_t length = array.get_आकार();
    अगर(length == 0) {
        वापस 0;
    }
    अगर(length > capacity) {
        errorCode = U_बफर_OVERFLOW_ERROR;
        वापस length;
    }
    क्रम(int32_t i = 0; i < length; ++i) {
        int32_t s_लंबाई;
        // No tracing: handled by the caller
        स्थिर char16_t *s = res_getStringNoTrace(p_Res_तथ्य, array.internalGetResource(p_Res_तथ्य, i), &s_लंबाई);
        अगर(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            वापस 0;
        }
        dest[i].setTo(true, s, s_लंबाई);
    }
    वापस length;
}

}  // namespace

U_CAPI स्थिर char16_t * U_EXPORT2
res_getAlias(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई) {
    स्थिर char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    अगर(RES_GET_TYPE(res)==URES_ALIAS) {
        स्थिर int32_t *p32= offset==0 ? &gEmptyString.length : p_Res_तथ्य->p_मूल+offset;
        length=*p32++;
        p=(स्थिर char16_t *)p32;
    } अन्यथा {
        p=nullptr;
        length=0;
    }
    अगर(p_लंबाई) {
        *p_लंबाई=length;
    }
    वापस p;
}

U_CAPI स्थिर uint8_t * U_EXPORT2
res_get_द्विआधारी_No_Trace(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई) {
    स्थिर uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    अगर(RES_GET_TYPE(res)==URES_द्विआधारी) {
        स्थिर int32_t *p32= offset==0 ? (स्थिर int32_t*)&gEmpty32 : p_Res_तथ्य->p_मूल+offset;
        length=*p32++;
        p=(स्थिर uint8_t *)p32;
    } अन्यथा {
        p=nullptr;
        length=0;
    }
    अगर(p_लंबाई) {
        *p_लंबाई=length;
    }
    वापस p;
}


U_CAPI स्थिर int32_t * U_EXPORT2
res_getIntVectorNoTrace(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res, int32_t *p_लंबाई) {
    स्थिर int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    अगर(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (स्थिर int32_t *)&gEmpty32 : p_Res_तथ्य->p_मूल+offset;
        length=*p++;
    } अन्यथा {
        p=nullptr;
        length=0;
    }
    अगर(p_लंबाई) {
        *p_लंबाई=length;
    }
    वापस p;
}

U_CAPI int32_t U_EXPORT2
res_गिनती_Array_वस्तुएँ(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    चयन(RES_GET_TYPE(res)) {
    हाल URES_STRING:
    हाल URES_STRING_V2:
    हाल URES_द्विआधारी:
    हाल URES_ALIAS:
    हाल URES_INT:
    हाल URES_INT_VECTOR:
        वापस 1;
    हाल URES_ARRAY:
    हाल URES_TABLE32:
        वापस offset==0 ? 0 : *(p_Res_तथ्य->p_मूल+offset);
    हाल URES_TABLE:
        वापस offset==0 ? 0 : *((स्थिर uint16_t *)(p_Res_तथ्य->p_मूल+offset));
    हाल URES_ARRAY16:
    हाल URES_TABLE16:
        वापस p_Res_तथ्य->p16BitUnits[offset];
    शेष:
        वापस 0;
    }
}

U_NAMESPACE_BEGIN

Resource_तथ्य_मूल्य::~Resource_तथ्य_मूल्य() {}

UResType Resource_तथ्य_मूल्य::getType() स्थिर {
    वापस res_getPublicType(res);
}

स्थिर char16_t *Resource_तथ्य_मूल्य::getString(int32_t &length, UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    स्थिर char16_t *s = res_getString(fTraceInfo, &get_तथ्य(), res, &length);
    अगर(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    वापस s;
}

स्थिर char16_t *Resource_तथ्य_मूल्य::getAliasString(int32_t &length, UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    स्थिर char16_t *s = res_getAlias(&get_तथ्य(), res, &length);
    अगर(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    वापस s;
}

int32_t Resource_तथ्य_मूल्य::getInt(UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    अगर(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    वापस res_getInt(fTraceInfo, res);
}

uint32_t Resource_तथ्य_मूल्य::getUInt(UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    अगर(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    वापस res_getUInt(fTraceInfo, res);
}

स्थिर int32_t *Resource_तथ्य_मूल्य::getIntVector(int32_t &length, UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    स्थिर int32_t *iv = res_getIntVector(fTraceInfo, &get_तथ्य(), res, &length);
    अगर(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    वापस iv;
}

स्थिर uint8_t *Resource_तथ्य_मूल्य::get_द्विआधारी(int32_t &length, UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    स्थिर uint8_t *b = res_get_द्विआधारी(fTraceInfo, &get_तथ्य(), res, &length);
    अगर(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    वापस b;
}

ResourceArray Resource_तथ्य_मूल्य::getArray(UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस {};
    }
    स्थिर uint16_t *items16 = nullptr;
    स्थिर Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    चयन(RES_GET_TYPE(res)) {
    हाल URES_ARRAY:
        अगर (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<स्थिर Resource*>(get_तथ्य().p_मूल) + offset;
            length = *items32++;
        }
        अवरोध;
    हाल URES_ARRAY16:
        items16 = get_तथ्य().p16BitUnits+offset;
        length = *items16++;
        अवरोध;
    शेष:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        वापस {};
    }
    वापस ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_तथ्य_मूल्य::getTable(UErrorCode &errorCode) स्थिर {
    अगर(U_FAILURE(errorCode)) {
        वापस {};
    }
    स्थिर uint16_t *keys16 = nullptr;
    स्थिर int32_t *keys32 = nullptr;
    स्थिर uint16_t *items16 = nullptr;
    स्थिर Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    चयन(RES_GET_TYPE(res)) {
    हाल URES_TABLE:
        अगर (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<स्थिर uint16_t*>(get_तथ्य().p_मूल + offset);
            length = *keys16++;
            items32 = reinterpret_cast<स्थिर Resource*>(keys16 + length + (~length & 1));
        }
        अवरोध;
    हाल URES_TABLE16:
        keys16 = get_तथ्य().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        अवरोध;
    हाल URES_TABLE32:
        अगर (offset != 0) {  // empty if offset==0
            keys32 = get_तथ्य().p_मूल+offset;
            length = *keys32++;
            items32 = reinterpret_cast<स्थिर Resource*>(keys32) + length;
        }
        अवरोध;
    शेष:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        वापस {};
    }
    वापस ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_तथ्य_मूल्य::isNoInheritanceMarker() स्थिर {
    वापस ::isNoInheritanceMarker(&get_तथ्य(), res);
}

int32_t Resource_तथ्य_मूल्य::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) स्थिर {
    वापस ::getStringArray(&get_तथ्य(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_तथ्य_मूल्य::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) स्थिर {
    अगर(URES_IS_ARRAY(res)) {
        वापस ::getStringArray(&get_तथ्य(), getArray(errorCode), dest, capacity, errorCode);
    }
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    अगर(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    अगर(capacity < 1) {
        errorCode = U_बफर_OVERFLOW_ERROR;
        वापस 1;
    }
    int32_t s_लंबाई;
    स्थिर char16_t *s = res_getString(fTraceInfo, &get_तथ्य(), res, &s_लंबाई);
    अगर(s != nullptr) {
        dest[0].setTo(true, s, s_लंबाई);
        वापस 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    वापस 0;
}

UnicodeString Resource_तथ्य_मूल्य::getStringOrFirstOfArray(UErrorCode &errorCode) स्थिर {
    UnicodeString us;
    अगर(U_FAILURE(errorCode)) {
        वापस us;
    }
    int32_t s_लंबाई;
    स्थिर char16_t *s = res_getString(fTraceInfo, &get_तथ्य(), res, &s_लंबाई);
    अगर(s != nullptr) {
        us.setTo(true, s, s_लंबाई);
        वापस us;
    }
    ResourceArray array = getArray(errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस us;
    }
    अगर(array.get_आकार() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_तथ्य(), array.internalGetResource(&get_तथ्य(), 0), &s_लंबाई);
        अगर(s != nullptr) {
            us.setTo(true, s, s_लंबाई);
            वापस us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    वापस us;
}

U_NAMESPACE_END

अटल Resource
makeResourceFrom16(स्थिर Resource_तथ्य *p_Res_तथ्य, int32_t res16) {
    अगर(res16<p_Res_तथ्य->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } अन्यथा {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_तथ्य->poolStringIndex16Limit+p_Res_तथ्य->pool_String_सूचकांक_Limit;
    }
    वापस URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_कुंजी(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource table,
                      int32_t *सूचकांक_R, स्थिर अक्षर **कुंजी) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    अगर(कुंजी == nullptr || *कुंजी == nullptr) {
        वापस RES_BOGUS;
    }
    चयन(RES_GET_TYPE(table)) {
    हाल URES_TABLE: {
        अगर (offset!=0) { /* empty if offset==0 */
            स्थिर uint16_t *p= (स्थिर uint16_t *)(p_Res_तथ्य->p_मूल+offset);
            length=*p++;
            *सूचकांक_R=idx=_res_findTableItem(p_Res_तथ्य, p, length, *कुंजी, कुंजी);
            अगर(idx>=0) {
                स्थिर Resource *p32=(स्थिर Resource *)(p+length+(~length&1));
                वापस p32[idx];
            }
        }
        अवरोध;
    }
    हाल URES_TABLE16: {
        स्थिर uint16_t *p=p_Res_तथ्य->p16BitUnits+offset;
        length=*p++;
        *सूचकांक_R=idx=_res_findTableItem(p_Res_तथ्य, p, length, *कुंजी, कुंजी);
        अगर(idx>=0) {
            वापस makeResourceFrom16(p_Res_तथ्य, p[length+idx]);
        }
        अवरोध;
    }
    हाल URES_TABLE32: {
        अगर (offset!=0) { /* empty if offset==0 */
            स्थिर int32_t *p= p_Res_तथ्य->p_मूल+offset;
            length=*p++;
            *सूचकांक_R=idx=_res_findTable32Item(p_Res_तथ्य, p, length, *कुंजी, कुंजी);
            अगर(idx>=0) {
                वापस (Resource)p[length+idx];
            }
        }
        अवरोध;
    }
    शेष:
        अवरोध;
    }
    वापस RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_सूचकांक(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource table,
                        int32_t सूचकांक_R, स्थिर अक्षर **कुंजी) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    अगर (सूचकांक_R < 0) {
        वापस RES_BOGUS;
    }
    चयन(RES_GET_TYPE(table)) {
    हाल URES_TABLE: {
        अगर (offset != 0) { /* empty if offset==0 */
            स्थिर uint16_t *p= (स्थिर uint16_t *)(p_Res_तथ्य->p_मूल+offset);
            length=*p++;
            अगर(सूचकांक_R<length) {
                स्थिर Resource *p32=(स्थिर Resource *)(p+length+(~length&1));
                अगर(कुंजी!=nullptr) {
                    *कुंजी=RES_GET_KEY16(p_Res_तथ्य, p[सूचकांक_R]);
                }
                वापस p32[सूचकांक_R];
            }
        }
        अवरोध;
    }
    हाल URES_TABLE16: {
        स्थिर uint16_t *p=p_Res_तथ्य->p16BitUnits+offset;
        length=*p++;
        अगर(सूचकांक_R<length) {
            अगर(कुंजी!=nullptr) {
                *कुंजी=RES_GET_KEY16(p_Res_तथ्य, p[सूचकांक_R]);
            }
            वापस makeResourceFrom16(p_Res_तथ्य, p[length+सूचकांक_R]);
        }
        अवरोध;
    }
    हाल URES_TABLE32: {
        अगर (offset != 0) { /* empty if offset==0 */
            स्थिर int32_t *p= p_Res_तथ्य->p_मूल+offset;
            length=*p++;
            अगर(सूचकांक_R<length) {
                अगर(कुंजी!=nullptr) {
                    *कुंजी=RES_GET_KEY32(p_Res_तथ्य, p[सूचकांक_R]);
                }
                वापस (Resource)p[length+सूचकांक_R];
            }
        }
        अवरोध;
    }
    शेष:
        अवरोध;
    }
    वापस RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(स्थिर Resource_तथ्य *p_Res_तथ्य, स्थिर अक्षर *कुंजी) {
    स्थिर अक्षर *real_कुंजी=कुंजी;
    int32_t idx;
    वापस res_get_Table_Item_By_कुंजी(p_Res_तथ्य, p_Res_तथ्य->मूल_Res, &idx, &real_कुंजी);
}


UBool icu::ResourceTable::get_कुंजी_And_मूल्य(int32_t i,
                                         स्थिर अक्षर *&कुंजी, icu::Resource_मूल्य &value) स्थिर {
    अगर(0 <= i && i < length) {
        icu::Resource_तथ्य_मूल्य &rd_मूल्य = static_cast<icu::Resource_तथ्य_मूल्य &>(value);
        अगर (keys16 != nullptr) {
            कुंजी = RES_GET_KEY16(&rd_मूल्य.get_तथ्य(), keys16[i]);
        } अन्यथा {
            कुंजी = RES_GET_KEY32(&rd_मूल्य.get_तथ्य(), keys32[i]);
        }
        Resource res;
        अगर (items16 != nullptr) {
            res = makeResourceFrom16(&rd_मूल्य.get_तथ्य(), items16[i]);
        } अन्यथा {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_मूल्य.setResource(res, ResourceTracer(fTraceInfo, कुंजी));
        वापस true;
    }
    वापस false;
}

UBool icu::ResourceTable::find_मूल्य(स्थिर अक्षर *कुंजी, Resource_मूल्य &value) स्थिर {
    icu::Resource_तथ्य_मूल्य &rd_मूल्य = static_cast<icu::Resource_तथ्य_मूल्य &>(value);
    स्थिर अक्षर *real_कुंजी = nullptr;
    int32_t i;
    अगर (keys16 != nullptr) {
        i = _res_findTableItem(&rd_मूल्य.get_तथ्य(), keys16, length, कुंजी, &real_कुंजी);
    } अन्यथा {
        i = _res_findTable32Item(&rd_मूल्य.get_तथ्य(), keys32, length, कुंजी, &real_कुंजी);
    }
    अगर (i >= 0) {
        Resource res;
        अगर (items16 != nullptr) {
            res = makeResourceFrom16(&rd_मूल्य.get_तथ्य(), items16[i]);
        } अन्यथा {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_मूल्य.setResource(res, ResourceTracer(fTraceInfo, कुंजी));
        वापस true;
    }
    वापस false;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource array, int32_t सूचकांक_R) {
    uint32_t offset=RES_GET_OFFSET(array);
    अगर (सूचकांक_R < 0) {
        वापस RES_BOGUS;
    }
    चयन(RES_GET_TYPE(array)) {
    हाल URES_ARRAY: {
        अगर (offset!=0) { /* empty if offset==0 */
            स्थिर int32_t *p= p_Res_तथ्य->p_मूल+offset;
            अगर(सूचकांक_R<*p) {
                वापस (Resource)p[1+सूचकांक_R];
            }
        }
        अवरोध;
    }
    हाल URES_ARRAY16: {
        स्थिर uint16_t *p=p_Res_तथ्य->p16BitUnits+offset;
        अगर(सूचकांक_R<*p) {
            वापस makeResourceFrom16(p_Res_तथ्य, p[1+सूचकांक_R]);
        }
        अवरोध;
    }
    शेष:
        अवरोध;
    }
    वापस RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(स्थिर Resource_तथ्य *p_Res_तथ्य, int32_t i) स्थिर {
    अगर (items16 != nullptr) {
        वापस makeResourceFrom16(p_Res_तथ्य, items16[i]);
    } अन्यथा {
        वापस items32[i];
    }
}

UBool icu::ResourceArray::get_मूल्य(int32_t i, icu::Resource_मूल्य &value) स्थिर {
    अगर(0 <= i && i < length) {
        icu::Resource_तथ्य_मूल्य &rd_मूल्य = static_cast<icu::Resource_तथ्य_मूल्य &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_मूल्य.setResource(
            internalGetResource(&rd_मूल्य.get_तथ्य(), i),
            ResourceTracer(fTraceInfo, i));
        वापस true;
    }
    वापस false;
}

U_CFUNC Resource
res_findResource(स्थिर Resource_तथ्य *p_Res_तथ्य, Resource r, अक्षर** path, स्थिर अक्षर** कुंजी) {
  अक्षर *pathP = *path, *अग्र_Sep_P = *path;
  अक्षर *close_सूचकांक = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t सूचकांक_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  अगर(!uprv_strlen(pathP)) {
      वापस r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  अगर(!URES_IS_CONTAINER(type)) {
      वापस RES_BOGUS;
  }
  
  जबतक(अग्र_Sep_P && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    अग्र_Sep_P = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    अगर(अग्र_Sep_P != nullptr) {
      अगर(अग्र_Sep_P == pathP) {
        // Empty key string.
        वापस RES_BOGUS;
      }
      *अग्र_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = अग्र_Sep_P+1;
    } अन्यथा {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    अगर(URES_IS_TABLE(type)) {
      *कुंजी = pathP;
      t2 = res_get_Table_Item_By_कुंजी(p_Res_तथ्य, t1, &सूचकांक_R, कुंजी);
    } अन्यथा अगर(URES_IS_ARRAY(type)) {
      सूचकांक_R = uprv_strtol(pathP, &close_सूचकांक, 10);
      अगर(सूचकांक_R >= 0 && *close_सूचकांक == 0) {
        t2 = res_getArrayItem(p_Res_तथ्य, t1, सूचकांक_R);
      } अन्यथा {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *कुंजी = nullptr;
    } अन्यथा { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    pathP = *path;
  }

  वापस t1;
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
प्रकार काष्ठा Row {
    int32_t कुंजी_सूचकांक, sort_सूचकांक;
} Row;

अटल int32_t U_CALLCONV
ures_compareRows(स्थिर व्योम *context, स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    स्थिर अक्षर* कुंजी_Chars = static_cast<स्थिर अक्षर*>(context);
    वापस static_cast<int32_t>(uprv_strcmp(कुंजी_Chars + static_cast<स्थिर Row*>(बाँए)->कुंजी_सूचकांक,
                                            कुंजी_Chars + static_cast<स्थिर Row*>(दाँए)->कुंजी_सूचकांक));
}

प्रकार काष्ठा अस्थायी_Table_2 {
    स्थिर अक्षर *कुंजी_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_कुंजी_Limit;
    uint8_t majorFormatVersion;
} अस्थायी_Table_2;

क्रमागत {
    ढेर_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
अटल स्थिर अक्षर *स्थिर g_Unknown_कुंजी="";

#अगर !UCONFIG_NO_COLLATION
// resource table key for collation binaries
अटल स्थिर char16_t g_Collation_Bin_कुंजी[]=u"%%CollationBin";
#पूर्ण_अगर

/*
 * swap one resource item
 */
अटल व्योम
ures_swapResource(स्थिर UDataSwapper *ds,
                  स्थिर Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  स्थिर अक्षर *कुंजी,
                  अस्थायी_Table_2 *p_अस्थायी_Table,
                  UErrorCode *pErrorCode) {
    स्थिर Resource *p;
    Resource *q;
    int32_t offset, count;

    चयन(RES_GET_TYPE(res)) {
    हाल URES_TABLE16:
    हाल URES_STRING_V2:
    हाल URES_INT:
    हाल URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        वापस;
    शेष:
        अवरोध;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    अगर(offset==0) {
        /* special offset indicating an empty item */
        वापस;
    }
    अगर (p_अस्थायी_Table->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        वापस;
    } अन्यथा {
        /* mark it as swapped now */
        p_अस्थायी_Table->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    चयन(RES_GET_TYPE(res)) {
    हाल URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    हाल URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        अवरोध;
    हाल URES_द्विआधारी:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#अगर !UCONFIG_NO_COLLATION
        अगर( कुंजी!=nullptr &&  /* the binary is in a table */
            (कुंजी!=g_Unknown_कुंजी ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, कुंजी, -1,
                                       g_Collation_Bin_कुंजी, UPRV_LENGTHOF(g_Collation_Bin_कुंजी)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_द्विआधारी(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, pErrorCode);
        }
#पूर्ण_अगर
        अवरोध;
    हाल URES_TABLE:
    हाल URES_TABLE32:
        {
            स्थिर uint16_t *pKey16;
            uint16_t *qKey16;

            स्थिर int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, old_सूचकांक;

            अगर(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<स्थिर uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } अन्यथा {
                /* get table item count */
                pKey32 = reinterpret_cast<स्थिर int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            अगर(count==0) {
                अवरोध;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            क्रम(i=0; i<count; ++i) {
                स्थिर अक्षर *item_कुंजी=g_Unknown_कुंजी;
                अगर(pKey16!=nullptr) {
                    int32_t कुंजी_Offset=ds->readUInt16(pKey16[i]);
                    अगर(कुंजी_Offset<p_अस्थायी_Table->local_कुंजी_Limit) {
                        item_कुंजी = reinterpret_cast<स्थिर अक्षर*>(outBundle) + कुंजी_Offset;
                    }
                } अन्यथा {
                    int32_t कुंजी_Offset=udata_readInt32(ds, pKey32[i]);
                    अगर(कुंजी_Offset>=0) {
                        item_कुंजी = reinterpret_cast<स्थिर अक्षर*>(outBundle) + कुंजी_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_कुंजी, p_अस्थायी_Table, pErrorCode);
                अगर(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    वापस;
                }
            }

            अगर(p_अस्थायी_Table->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                अगर(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } अन्यथा {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                अवरोध;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            अगर(pKey16!=nullptr) {
                क्रम(i=0; i<count; ++i) {
                    p_अस्थायी_Table->rows[i].कुंजी_सूचकांक=ds->readUInt16(pKey16[i]);
                    p_अस्थायी_Table->rows[i].sort_सूचकांक=i;
                }
            } अन्यथा {
                क्रम(i=0; i<count; ++i) {
                    p_अस्थायी_Table->rows[i].कुंजी_सूचकांक=udata_readInt32(ds, pKey32[i]);
                    p_अस्थायी_Table->rows[i].sort_सूचकांक=i;
                }
            }
            uprv_sortArray(p_अस्थायी_Table->rows, count, माप(Row),
                           ures_compareRows, p_अस्थायी_Table->कुंजी_Chars,
                           false, pErrorCode);
            अगर(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                वापस;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*कुंजियाँ*/
            अगर(pKey16!=nullptr) {
                uint16_t *rKey16;

                अगर(pKey16!=qKey16) {
                    rKey16=qKey16;
                } अन्यथा {
                    rKey16 = reinterpret_cast<uint16_t*>(p_अस्थायी_Table->resort);
                }
                क्रम(i=0; i<count; ++i) {
                    old_सूचकांक=p_अस्थायी_Table->rows[i].sort_सूचकांक;
                    ds->swapArray16(ds, pKey16+old_सूचकांक, 2, rKey16+i, pErrorCode);
                }
                अगर(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } अन्यथा {
                int32_t *rKey32;

                अगर(pKey32!=qKey32) {
                    rKey32=qKey32;
                } अन्यथा {
                    rKey32=p_अस्थायी_Table->resort;
                }
                क्रम(i=0; i<count; ++i) {
                    old_सूचकांक=p_अस्थायी_Table->rows[i].sort_सूचकांक;
                    ds->swapArray32(ds, pKey32+old_सूचकांक, 4, rKey32+i, pErrorCode);
                }
                अगर(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                अगर(p!=q) {
                    r=q;
                } अन्यथा {
                    r = reinterpret_cast<Resource*>(p_अस्थायी_Table->resort);
                }
                क्रम(i=0; i<count; ++i) {
                    old_सूचकांक=p_अस्थायी_Table->rows[i].sort_सूचकांक;
                    ds->swapArray32(ds, p+old_सूचकांक, 4, r+i, pErrorCode);
                }
                अगर(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        अवरोध;
    हाल URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            क्रम(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, p_अस्थायी_Table, pErrorCode);
                अगर(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    वापस;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        अवरोध;
    हाल URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        अवरोध;
    शेष:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        अवरोध;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(स्थिर UDataSwapper *ds,
          स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
          UErrorCode *pErrorCode) {
    स्थिर UDataInfo *pInfo;
    स्थिर Resource *inBundle;
    Resource मूल_Res;
    int32_t header_आकार, max_Table_लंबाई;

    Row rows[ढेर_ROW_CAPACITY];
    int32_t resort[ढेर_ROW_CAPACITY];
    अस्थायी_Table_2 अस्थायी_Table;

    स्थिर int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_लंबाई, सूचकांक_लंबाई, कुंजियाँ_Bottom, कुंजियाँ_शीर्ष, resBottom, शीर्ष_2;

    /* udata_swapDataHeader checks the arguments */
    header_आकार=udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* check data format and format version */
    pInfo=(स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
    अगर(!(
        pInfo->तथ्य_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->तथ्य_Format[1]==0x65 &&
        pInfo->तथ्य_Format[2]==0x73 &&
        pInfo->तथ्य_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->formatVersion[0]==1 && pInfo->formatVersion[1]>=1) ||
            pInfo->formatVersion[0]==2 || pInfo->formatVersion[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1],
                         pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3],
                         pInfo->formatVersion[0], pInfo->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }
    अस्थायी_Table.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    अगर(length<0) {
        bundle_लंबाई=-1;
    } अन्यथा {
        bundle_लंबाई=(length-header_आकार)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        अगर(bundle_लंबाई<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-header_आकार);
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
    }

    inBundle=(स्थिर Resource *)((स्थिर अक्षर *)in_तथ्य+header_आकार);
    मूल_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(स्थिर int32_t *)(inBundle+1);

    सूचकांक_लंबाई=udata_readInt32(ds, inIndexes[URES_सूचकांक_लंबाई])&0xff;
    अगर(सूचकांक_लंबाई<=URES_सूचकांक_MAX_TABLE_लंबाई) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }
    कुंजियाँ_Bottom=1+सूचकांक_लंबाई;
    कुंजियाँ_शीर्ष=udata_readInt32(ds, inIndexes[URES_सूचकांक_कुंजियाँ_शीर्ष]);
    अगर(सूचकांक_लंबाई>URES_सूचकांक_16_BIT_शीर्ष) {
        resBottom=udata_readInt32(ds, inIndexes[URES_सूचकांक_16_BIT_शीर्ष]);
    } अन्यथा {
        resBottom=कुंजियाँ_शीर्ष;
    }
    शीर्ष_2=udata_readInt32(ds, inIndexes[URES_सूचकांक_BUNDLE_शीर्ष]);
    max_Table_लंबाई=udata_readInt32(ds, inIndexes[URES_सूचकांक_MAX_TABLE_लंबाई]);

    अगर(0<=bundle_लंबाई && bundle_लंबाई<शीर्ष_2) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         शीर्ष_2, bundle_लंबाई);
        *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }
    अगर(कुंजियाँ_शीर्ष>(1+सूचकांक_लंबाई)) {
        अस्थायी_Table.local_कुंजी_Limit=कुंजियाँ_शीर्ष<<2;
    } अन्यथा {
        अस्थायी_Table.local_कुंजी_Limit=0;
    }

    अगर(length>=0) {
        Resource *outBundle=(Resource *)((अक्षर *)out_तथ्य+header_आकार);

        /* track which resources we have already swapped */
        uint32_t ढेर_Res_Flags[ढेर_ROW_CAPACITY];
        int32_t res_Flags_लंबाई;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_लंबाई=(length+31)>>5;          /* number of bytes needed */
        res_Flags_लंबाई=(res_Flags_लंबाई+3)&~3;   /* multiple of 4 bytes for uint32_t */
        अगर(res_Flags_लंबाई<=(int32_t)माप(ढेर_Res_Flags)) {
            अस्थायी_Table.resFlags=ढेर_Res_Flags;
        } अन्यथा {
            अस्थायी_Table.resFlags=(uint32_t *)uprv_malloc(res_Flags_लंबाई);
            अगर(अस्थायी_Table.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                वापस 0;
            }
        }
        uprv_memset(अस्थायी_Table.resFlags, 0, res_Flags_लंबाई);

        /* copy the bundle for binary and inaccessible data */
        अगर(in_तथ्य!=out_तथ्य) {
            uprv_memcpy(outBundle, inBundle, 4*शीर्ष_2);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+कुंजियाँ_Bottom, 4*(कुंजियाँ_शीर्ष-कुंजियाँ_Bottom),
                                    outBundle+कुंजियाँ_Bottom, pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(कुंजियाँ_शीर्ष-कुंजियाँ_Bottom));
            अगर(अस्थायी_Table.resFlags!=ढेर_Res_Flags) {
                uprv_free(अस्थायी_Table.resFlags);
            }
            वापस 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        अगर(कुंजियाँ_शीर्ष<resBottom) {
            ds->swapArray16(ds, inBundle+कुंजियाँ_शीर्ष, (resBottom-कुंजियाँ_शीर्ष)*4, outBundle+कुंजियाँ_शीर्ष, pErrorCode);
            अगर(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-कुंजियाँ_शीर्ष));
                अगर(अस्थायी_Table.resFlags!=ढेर_Res_Flags) {
                    uprv_free(अस्थायी_Table.resFlags);
                }
                वापस 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        अस्थायी_Table.कुंजी_Chars=(स्थिर अक्षर *)outBundle; /* sort by outCharset */
        अगर(अस्थायी_Table.majorFormatVersion>1 || max_Table_लंबाई<=ढेर_ROW_CAPACITY) {
            अस्थायी_Table.rows=rows;
            अस्थायी_Table.resort=resort;
        } अन्यथा {
            अस्थायी_Table.rows=(Row *)uprv_malloc(max_Table_लंबाई*माप(Row)+max_Table_लंबाई*4);
            अगर(अस्थायी_Table.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_Table_लंबाई);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                अगर(अस्थायी_Table.resFlags!=ढेर_Res_Flags) {
                    uprv_free(अस्थायी_Table.resFlags);
                }
                वापस 0;
            }
            अस्थायी_Table.resort=(int32_t *)(अस्थायी_Table.rows+max_Table_लंबाई);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, मूल_Res, nullptr, &अस्थायी_Table, pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             मूल_Res);
        }

        अगर(अस्थायी_Table.rows!=rows) {
            uprv_free(अस्थायी_Table.rows);
        }
        अगर(अस्थायी_Table.resFlags!=ढेर_Res_Flags) {
            uprv_free(अस्थायी_Table.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, कुंजियाँ_Bottom*4, outBundle, pErrorCode);
    }

    वापस header_आकार+4*शीर्ष_2;
}

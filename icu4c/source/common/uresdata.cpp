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
#define RES_GET_KEY16(p_Res_তথ্য, চাবি_Offset) \
    ((চাবি_Offset)<(p_Res_তথ্য)->local_চাবি_Limit ? \
        (ধ্রুবক অক্ষর *)(p_Res_তথ্য)->p_মূল+(চাবি_Offset) : \
        (p_Res_তথ্য)->pool_Bundle_চাবিগুলি+(চাবি_Offset)-(p_Res_তথ্য)->local_চাবি_Limit)

#define RES_GET_KEY32(p_Res_তথ্য, চাবি_Offset) \
    ((চাবি_Offset)>=0 ? \
        (ধ্রুবক অক্ষর *)(p_Res_তথ্য)->p_মূল+(চাবি_Offset) : \
        (p_Res_তথ্য)->pool_Bundle_চাবিগুলি+((চাবি_Offset)&0x7fffffff))

#define URESDATA_ITEM_NOT_FOUND -1

/* empty resources, returned when the resource offset is 0 */
স্থির ধ্রুবক uint16_t gEmpty16=0;

স্থির ধ্রুবক কাঠামো {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

স্থির ধ্রুবক কাঠামো {
    int32_t length;
    char16_t nul;
    char16_t pad;
} gEmptyString={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

স্থির int32_t
_res_findTableItem(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, ধ্রুবক uint16_t *চাবি_Offsets, int32_t length,
                   ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর **real_চাবি) {
    ধ্রুবক অক্ষর *table_চাবি;
    int32_t mid, start, limit;
    পূর্ণসংখ্যা ফলাফল;

    /* do a binary search for the key */
    start=0;
    limit=length;
    যখন(start<limit) {
        mid = (start + limit) / 2;
        table_চাবি = RES_GET_KEY16(p_Res_তথ্য, চাবি_Offsets[mid]);
        যদি (p_Res_তথ্য->useNativeStrcmp) {
            ফলাফল = uprv_strcmp(চাবি, table_চাবি);
        } অন্যথা {
            ফলাফল = uprv_compareInvCharsAsAscii(চাবি, table_চাবি);
        }
        যদি (ফলাফল < 0) {
            limit = mid;
        } অন্যথা যদি (ফলাফল > 0) {
            start = mid + 1;
        } অন্যথা {
            /* We found it! */
            *real_চাবি=table_চাবি;
            ফেরত mid;
        }
    }
    ফেরত URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

স্থির int32_t
_res_findTable32Item(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, ধ্রুবক int32_t *চাবি_Offsets, int32_t length,
                     ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর **real_চাবি) {
    ধ্রুবক অক্ষর *table_চাবি;
    int32_t mid, start, limit;
    পূর্ণসংখ্যা ফলাফল;

    /* do a binary search for the key */
    start=0;
    limit=length;
    যখন(start<limit) {
        mid = (start + limit) / 2;
        table_চাবি = RES_GET_KEY32(p_Res_তথ্য, চাবি_Offsets[mid]);
        যদি (p_Res_তথ্য->useNativeStrcmp) {
            ফলাফল = uprv_strcmp(চাবি, table_চাবি);
        } অন্যথা {
            ফলাফল = uprv_compareInvCharsAsAscii(চাবি, table_চাবি);
        }
        যদি (ফলাফল < 0) {
            limit = mid;
        } অন্যথা যদি (ফলাফল > 0) {
            start = mid + 1;
        } অন্যথা {
            /* We found it! */
            *real_চাবি=table_চাবি;
            ফেরত mid;
        }
    }
    ফেরত URESDATA_ITEM_NOT_FOUND;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

স্থির UBool U_CALLCONV
isAcceptable(শূন্য *context,
             ধ্রুবক অক্ষর * /*type*/, ধ্রুবক অক্ষর * /*name*/,
             ধ্রুবক UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->formatVersion, 4);
    ফেরত
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->তথ্য_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->তথ্য_Format[1]==0x65 &&
        pInfo->তথ্য_Format[2]==0x73 &&
        pInfo->তথ্য_Format[3]==0x42 &&
        (1<=pInfo->formatVersion[0] && pInfo->formatVersion[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

স্থির শূন্য
res_init(Resource_তথ্য *p_Res_তথ্য,
         UVersionInfo formatVersion, ধ্রুবক শূন্য *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UResType মূল_Type;

    /* get the root resource */
    p_Res_তথ্য->p_মূল = static_cast<ধ্রুবক int32_t*>(inBytes);
    p_Res_তথ্য->মূল_Res = static_cast<Resource>(*p_Res_তথ্য->p_মূল);
    p_Res_তথ্য->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    যদি(length>=0 && (length/4)<((formatVersion[0]==1 && formatVersion[1]==0) ? 1 : 1+5)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_তথ্য);
        ফেরত;
    }

    /* currently, we accept only resources that have a Table as their roots */
    মূল_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_তথ্য->মূল_Res));
    যদি(!URES_IS_TABLE(মূল_Type)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_তথ্য);
        ফেরত;
    }

    যদি(formatVersion[0]==1 && formatVersion[1]==0) {
        p_Res_তথ্য->local_চাবি_Limit=0x10000;  /* greater than any 16-bit key string offset */
    } অন্যথা {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ধ্রুবক int32_t *indexes=p_Res_তথ্য->p_মূল+1;
        int32_t সূচক_দৈর্ঘ্য=indexes[URES_সূচক_দৈর্ঘ্য]&0xff;
        যদি(সূচক_দৈর্ঘ্য<=URES_সূচক_MAX_TABLE_দৈর্ঘ্য) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_তথ্য);
            ফেরত;
        }
        যদি( length>=0 &&
            (length<((1+সূচক_দৈর্ঘ্য)<<2) ||
             length<(indexes[URES_সূচক_BUNDLE_শীর্ষ]<<2))
        ) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_তথ্য);
            ফেরত;
        }
        যদি(indexes[URES_সূচক_চাবিগুলি_শীর্ষ]>(1+সূচক_দৈর্ঘ্য)) {
            p_Res_তথ্য->local_চাবি_Limit=indexes[URES_সূচক_চাবিগুলি_শীর্ষ]<<2;
        }
        যদি(formatVersion[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_তথ্য->pool_String_সূচক_Limit = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_সূচক_দৈর্ঘ্য]) >> 8);
        }
        যদি(সূচক_দৈর্ঘ্য>URES_সূচক_ATTRIBUTES) {
            int32_t att=indexes[URES_সূচক_ATTRIBUTES];
            p_Res_তথ্য->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_তথ্য->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_তথ্য->usesPoolBundle = static_cast<UBool>((att & URES_ATT_USES_POOL_BUNDLE) != 0);
            p_Res_তথ্য->pool_String_সূচক_Limit|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_তথ্য->poolStringIndex16Limit = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        যদি((p_Res_তথ্য->isPoolBundle || p_Res_তথ্য->usesPoolBundle) && সূচক_দৈর্ঘ্য<=URES_সূচক_POOL_CHECKSUM) {
            *errorCode=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_তথ্য);
            ফেরত;
        }
        যদি( সূচক_দৈর্ঘ্য>URES_সূচক_16_BIT_শীর্ষ &&
            indexes[URES_সূচক_16_BIT_শীর্ষ]>indexes[URES_সূচক_চাবিগুলি_শীর্ষ]
        ) {
            p_Res_তথ্য->p16BitUnits = reinterpret_cast<ধ্রুবক uint16_t*>(p_Res_তথ্য->p_মূল + indexes[URES_সূচক_চাবিগুলি_শীর্ষ]);
        }
    }

    যদি(formatVersion[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_তথ্য->useNativeStrcmp=সত্য;
    }
}

U_CAPI শূন্য U_EXPORT2
res_read(Resource_তথ্য *p_Res_তথ্য,
         ধ্রুবক UDataInfo *pInfo, ধ্রুবক শূন্য *inBytes, int32_t length,
         UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(p_Res_তথ্য, 0, আকার(Resource_তথ্য));
    যদি(U_FAILURE(*errorCode)) {
        ফেরত;
    }
    যদি(!isAcceptable(formatVersion, nullptr, nullptr, pInfo)) {
        *errorCode=U_INVALID_FORMAT_ERROR;
        ফেরত;
    }
    res_init(p_Res_তথ্য, formatVersion, inBytes, length, errorCode);
}

U_CFUNC শূন্য
res_load(Resource_তথ্য *p_Res_তথ্য,
         ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, UErrorCode *errorCode) {
    UVersionInfo formatVersion;

    uprv_memset(p_Res_তথ্য, 0, আকার(Resource_তথ্য));

    /* load the ResourceBundle file */
    p_Res_তথ্য->তথ্য=udata_openChoice(path, "res", name, isAcceptable, formatVersion, errorCode);
    যদি(U_FAILURE(*errorCode)) {
        ফেরত;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_তথ্য, formatVersion, udata_getMemory(p_Res_তথ্য->তথ্য), -1, errorCode);
}

U_CFUNC শূন্য
res_unload(Resource_তথ্য *p_Res_তথ্য) {
    যদি(p_Res_তথ্য->তথ্য!=nullptr) {
        udata_close(p_Res_তথ্য->তথ্য);
        p_Res_তথ্য->তথ্য=nullptr;
    }
}

স্থির ধ্রুবক int8_t gPublicTypes[URES_LIMIT] = {
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
    ফেরত (UResType)gPublicTypes[RES_GET_TYPE(res)];
}

U_CAPI ধ্রুবক char16_t * U_EXPORT2
res_getStringNoTrace(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource res, int32_t *p_দৈর্ঘ্য) {
    ধ্রুবক char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    যদি(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        যদি((int32_t)offset<p_Res_তথ্য->pool_String_সূচক_Limit) {
            p=(ধ্রুবক char16_t *)p_Res_তথ্য->poolBundleStrings+offset;
        } অন্যথা {
            p=(ধ্রুবক char16_t *)p_Res_তথ্য->p16BitUnits+(offset-p_Res_তথ্য->pool_String_সূচক_Limit);
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
        ধ্রুবক int32_t *p32= res==0 ? &gEmptyString.length : p_Res_তথ্য->p_মূল+res;
        length=*p32++;
        p=(ধ্রুবক char16_t *)p32;
    } অন্যথা {
        p=nullptr;
        length=0;
    }
    যদি(p_দৈর্ঘ্য) {
        *p_দৈর্ঘ্য=length;
    }
    ফেরত p;
}

নামাকাশ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    যদি (offset == 0) {
        // empty string
    } অন্যথা যদি (res == offset) {
        ধ্রুবক int32_t *p32=p_Res_তথ্য->p_মূল+res;
        int32_t length=*p32;
        ধ্রুবক char16_t* p = reinterpret_cast<ধ্রুবক char16_t*>(p32);
        ফেরত length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } অন্যথা যদি (RES_GET_TYPE(res) == URES_STRING_V2) {
        ধ্রুবক char16_t *p;
        যদি (static_cast<int32_t>(offset) < p_Res_তথ্য->pool_String_সূচক_Limit) {
            p = reinterpret_cast<ধ্রুবক char16_t*>(p_Res_তথ্য->poolBundleStrings) + offset;
        } অন্যথা {
            p = reinterpret_cast<ধ্রুবক char16_t*>(p_Res_তথ্য->p16BitUnits) + (offset - p_Res_তথ্য->pool_String_সূচক_Limit);
        }
        int32_t first=*p;
        যদি (first == 0x2205) {  // implicit length
            ফেরত p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } অন্যথা যদি (first == 0xdc03) {  // explicit length 3 (should not occur)
            ফেরত p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } অন্যথা {
            // Assume that the string has not been stored with more length units than necessary.
            ফেরত মিথ্যা;
        }
    }
    ফেরত মিথ্যা;
}

int32_t getStringArray(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, ধ্রুবক icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    যদি(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    int32_t length = array.get_আকার();
    যদি(length == 0) {
        ফেরত 0;
    }
    যদি(length > capacity) {
        errorCode = U_বাফার_OVERFLOW_ERROR;
        ফেরত length;
    }
    জন্য(int32_t i = 0; i < length; ++i) {
        int32_t s_দৈর্ঘ্য;
        // No tracing: handled by the caller
        ধ্রুবক char16_t *s = res_getStringNoTrace(p_Res_তথ্য, array.internalGetResource(p_Res_তথ্য, i), &s_দৈর্ঘ্য);
        যদি(s == nullptr) {
            errorCode = U_RESOURCE_TYPE_MISMATCH;
            ফেরত 0;
        }
        dest[i].setTo(সত্য, s, s_দৈর্ঘ্য);
    }
    ফেরত length;
}

}  // namespace

U_CAPI ধ্রুবক char16_t * U_EXPORT2
res_getAlias(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource res, int32_t *p_দৈর্ঘ্য) {
    ধ্রুবক char16_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    যদি(RES_GET_TYPE(res)==URES_ALIAS) {
        ধ্রুবক int32_t *p32= offset==0 ? &gEmptyString.length : p_Res_তথ্য->p_মূল+offset;
        length=*p32++;
        p=(ধ্রুবক char16_t *)p32;
    } অন্যথা {
        p=nullptr;
        length=0;
    }
    যদি(p_দৈর্ঘ্য) {
        *p_দৈর্ঘ্য=length;
    }
    ফেরত p;
}

U_CAPI ধ্রুবক uint8_t * U_EXPORT2
res_get_দ্বিমিক_No_Trace(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource res, int32_t *p_দৈর্ঘ্য) {
    ধ্রুবক uint8_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    যদি(RES_GET_TYPE(res)==URES_দ্বিমিক) {
        ধ্রুবক int32_t *p32= offset==0 ? (ধ্রুবক int32_t*)&gEmpty32 : p_Res_তথ্য->p_মূল+offset;
        length=*p32++;
        p=(ধ্রুবক uint8_t *)p32;
    } অন্যথা {
        p=nullptr;
        length=0;
    }
    যদি(p_দৈর্ঘ্য) {
        *p_দৈর্ঘ্য=length;
    }
    ফেরত p;
}


U_CAPI ধ্রুবক int32_t * U_EXPORT2
res_getIntVectorNoTrace(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource res, int32_t *p_দৈর্ঘ্য) {
    ধ্রুবক int32_t *p;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length;
    যদি(RES_GET_TYPE(res)==URES_INT_VECTOR) {
        p= offset==0 ? (ধ্রুবক int32_t *)&gEmpty32 : p_Res_তথ্য->p_মূল+offset;
        length=*p++;
    } অন্যথা {
        p=nullptr;
        length=0;
    }
    যদি(p_দৈর্ঘ্য) {
        *p_দৈর্ঘ্য=length;
    }
    ফেরত p;
}

U_CAPI int32_t U_EXPORT2
res_গুনতি_Array_বস্তুসমূহ(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource res) {
    uint32_t offset=RES_GET_OFFSET(res);
    নির্বাচন(RES_GET_TYPE(res)) {
    ক্ষেত্র URES_STRING:
    ক্ষেত্র URES_STRING_V2:
    ক্ষেত্র URES_দ্বিমিক:
    ক্ষেত্র URES_ALIAS:
    ক্ষেত্র URES_INT:
    ক্ষেত্র URES_INT_VECTOR:
        ফেরত 1;
    ক্ষেত্র URES_ARRAY:
    ক্ষেত্র URES_TABLE32:
        ফেরত offset==0 ? 0 : *(p_Res_তথ্য->p_মূল+offset);
    ক্ষেত্র URES_TABLE:
        ফেরত offset==0 ? 0 : *((ধ্রুবক uint16_t *)(p_Res_তথ্য->p_মূল+offset));
    ক্ষেত্র URES_ARRAY16:
    ক্ষেত্র URES_TABLE16:
        ফেরত p_Res_তথ্য->p16BitUnits[offset];
    পূর্বনির্ধারিত:
        ফেরত 0;
    }
}

U_NAMESPACE_BEGIN

Resource_তথ্য_মান::~Resource_তথ্য_মান() {}

UResType Resource_তথ্য_মান::getType() ধ্রুবক {
    ফেরত res_getPublicType(res);
}

ধ্রুবক char16_t *Resource_তথ্য_মান::getString(int32_t &length, UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    ধ্রুবক char16_t *s = res_getString(fTraceInfo, &get_তথ্য(), res, &length);
    যদি(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ফেরত s;
}

ধ্রুবক char16_t *Resource_তথ্য_মান::getAliasString(int32_t &length, UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    ধ্রুবক char16_t *s = res_getAlias(&get_তথ্য(), res, &length);
    যদি(s == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ফেরত s;
}

int32_t Resource_তথ্য_মান::getInt(UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    যদি(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ফেরত res_getInt(fTraceInfo, res);
}

uint32_t Resource_তথ্য_মান::getUInt(UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    যদি(RES_GET_TYPE(res) != URES_INT) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ফেরত res_getUInt(fTraceInfo, res);
}

ধ্রুবক int32_t *Resource_তথ্য_মান::getIntVector(int32_t &length, UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    ধ্রুবক int32_t *iv = res_getIntVector(fTraceInfo, &get_তথ্য(), res, &length);
    যদি(iv == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ফেরত iv;
}

ধ্রুবক uint8_t *Resource_তথ্য_মান::get_দ্বিমিক(int32_t &length, UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    ধ্রুবক uint8_t *b = res_get_দ্বিমিক(fTraceInfo, &get_তথ্য(), res, &length);
    যদি(b == nullptr) {
        errorCode = U_RESOURCE_TYPE_MISMATCH;
    }
    ফেরত b;
}

ResourceArray Resource_তথ্য_মান::getArray(UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        ফেরত {};
    }
    ধ্রুবক uint16_t *items16 = nullptr;
    ধ্রুবক Resource *items32 = nullptr;
    uint32_t offset=RES_GET_OFFSET(res);
    int32_t length = 0;
    নির্বাচন(RES_GET_TYPE(res)) {
    ক্ষেত্র URES_ARRAY:
        যদি (offset!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ধ্রুবক Resource*>(get_তথ্য().p_মূল) + offset;
            length = *items32++;
        }
        বিরতি;
    ক্ষেত্র URES_ARRAY16:
        items16 = get_তথ্য().p16BitUnits+offset;
        length = *items16++;
        বিরতি;
    পূর্বনির্ধারিত:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        ফেরত {};
    }
    ফেরত ResourceArray(items16, items32, length, fTraceInfo);
}

ResourceTable Resource_তথ্য_মান::getTable(UErrorCode &errorCode) ধ্রুবক {
    যদি(U_FAILURE(errorCode)) {
        ফেরত {};
    }
    ধ্রুবক uint16_t *keys16 = nullptr;
    ধ্রুবক int32_t *keys32 = nullptr;
    ধ্রুবক uint16_t *items16 = nullptr;
    ধ্রুবক Resource *items32 = nullptr;
    uint32_t offset = RES_GET_OFFSET(res);
    int32_t length = 0;
    নির্বাচন(RES_GET_TYPE(res)) {
    ক্ষেত্র URES_TABLE:
        যদি (offset != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ধ্রুবক uint16_t*>(get_তথ্য().p_মূল + offset);
            length = *keys16++;
            items32 = reinterpret_cast<ধ্রুবক Resource*>(keys16 + length + (~length & 1));
        }
        বিরতি;
    ক্ষেত্র URES_TABLE16:
        keys16 = get_তথ্য().p16BitUnits+offset;
        length = *keys16++;
        items16 = keys16 + length;
        বিরতি;
    ক্ষেত্র URES_TABLE32:
        যদি (offset != 0) {  // empty if offset==0
            keys32 = get_তথ্য().p_মূল+offset;
            length = *keys32++;
            items32 = reinterpret_cast<ধ্রুবক Resource*>(keys32) + length;
        }
        বিরতি;
    পূর্বনির্ধারিত:
        errorCode = U_RESOURCE_TYPE_MISMATCH;
        ফেরত {};
    }
    ফেরত ResourceTable(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_তথ্য_মান::isNoInheritanceMarker() ধ্রুবক {
    ফেরত ::isNoInheritanceMarker(&get_তথ্য(), res);
}

int32_t Resource_তথ্য_মান::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UErrorCode &errorCode) ধ্রুবক {
    ফেরত ::getStringArray(&get_তথ্য(), getArray(errorCode), dest, capacity, errorCode);
}

int32_t Resource_তথ্য_মান::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UErrorCode &errorCode) ধ্রুবক {
    যদি(URES_IS_ARRAY(res)) {
        ফেরত ::getStringArray(&get_তথ্য(), getArray(errorCode), dest, capacity, errorCode);
    }
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    যদি(dest == nullptr ? capacity != 0 : capacity < 0) {
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    যদি(capacity < 1) {
        errorCode = U_বাফার_OVERFLOW_ERROR;
        ফেরত 1;
    }
    int32_t s_দৈর্ঘ্য;
    ধ্রুবক char16_t *s = res_getString(fTraceInfo, &get_তথ্য(), res, &s_দৈর্ঘ্য);
    যদি(s != nullptr) {
        dest[0].setTo(সত্য, s, s_দৈর্ঘ্য);
        ফেরত 1;
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    ফেরত 0;
}

UnicodeString Resource_তথ্য_মান::getStringOrFirstOfArray(UErrorCode &errorCode) ধ্রুবক {
    UnicodeString us;
    যদি(U_FAILURE(errorCode)) {
        ফেরত us;
    }
    int32_t s_দৈর্ঘ্য;
    ধ্রুবক char16_t *s = res_getString(fTraceInfo, &get_তথ্য(), res, &s_দৈর্ঘ্য);
    যদি(s != nullptr) {
        us.setTo(সত্য, s, s_দৈর্ঘ্য);
        ফেরত us;
    }
    ResourceArray array = getArray(errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত us;
    }
    যদি(array.get_আকার() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_তথ্য(), array.internalGetResource(&get_তথ্য(), 0), &s_দৈর্ঘ্য);
        যদি(s != nullptr) {
            us.setTo(সত্য, s, s_দৈর্ঘ্য);
            ফেরত us;
        }
    }
    errorCode = U_RESOURCE_TYPE_MISMATCH;
    ফেরত us;
}

U_NAMESPACE_END

স্থির Resource
makeResourceFrom16(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, int32_t res16) {
    যদি(res16<p_Res_তথ্য->poolStringIndex16Limit) {
        // Pool string, nothing to do.
    } অন্যথা {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_তথ্য->poolStringIndex16Limit+p_Res_তথ্য->pool_String_সূচক_Limit;
    }
    ফেরত URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_চাবি(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource table,
                      int32_t *সূচক_R, ধ্রুবক অক্ষর **চাবি) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    int32_t idx;
    যদি(চাবি == nullptr || *চাবি == nullptr) {
        ফেরত RES_BOGUS;
    }
    নির্বাচন(RES_GET_TYPE(table)) {
    ক্ষেত্র URES_TABLE: {
        যদি (offset!=0) { /* empty if offset==0 */
            ধ্রুবক uint16_t *p= (ধ্রুবক uint16_t *)(p_Res_তথ্য->p_মূল+offset);
            length=*p++;
            *সূচক_R=idx=_res_findTableItem(p_Res_তথ্য, p, length, *চাবি, চাবি);
            যদি(idx>=0) {
                ধ্রুবক Resource *p32=(ধ্রুবক Resource *)(p+length+(~length&1));
                ফেরত p32[idx];
            }
        }
        বিরতি;
    }
    ক্ষেত্র URES_TABLE16: {
        ধ্রুবক uint16_t *p=p_Res_তথ্য->p16BitUnits+offset;
        length=*p++;
        *সূচক_R=idx=_res_findTableItem(p_Res_তথ্য, p, length, *চাবি, চাবি);
        যদি(idx>=0) {
            ফেরত makeResourceFrom16(p_Res_তথ্য, p[length+idx]);
        }
        বিরতি;
    }
    ক্ষেত্র URES_TABLE32: {
        যদি (offset!=0) { /* empty if offset==0 */
            ধ্রুবক int32_t *p= p_Res_তথ্য->p_মূল+offset;
            length=*p++;
            *সূচক_R=idx=_res_findTable32Item(p_Res_তথ্য, p, length, *চাবি, চাবি);
            যদি(idx>=0) {
                ফেরত (Resource)p[length+idx];
            }
        }
        বিরতি;
    }
    পূর্বনির্ধারিত:
        বিরতি;
    }
    ফেরত RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_get_Table_Item_By_সূচক(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource table,
                        int32_t সূচক_R, ধ্রুবক অক্ষর **চাবি) {
    uint32_t offset=RES_GET_OFFSET(table);
    int32_t length;
    যদি (সূচক_R < 0) {
        ফেরত RES_BOGUS;
    }
    নির্বাচন(RES_GET_TYPE(table)) {
    ক্ষেত্র URES_TABLE: {
        যদি (offset != 0) { /* empty if offset==0 */
            ধ্রুবক uint16_t *p= (ধ্রুবক uint16_t *)(p_Res_তথ্য->p_মূল+offset);
            length=*p++;
            যদি(সূচক_R<length) {
                ধ্রুবক Resource *p32=(ধ্রুবক Resource *)(p+length+(~length&1));
                যদি(চাবি!=nullptr) {
                    *চাবি=RES_GET_KEY16(p_Res_তথ্য, p[সূচক_R]);
                }
                ফেরত p32[সূচক_R];
            }
        }
        বিরতি;
    }
    ক্ষেত্র URES_TABLE16: {
        ধ্রুবক uint16_t *p=p_Res_তথ্য->p16BitUnits+offset;
        length=*p++;
        যদি(সূচক_R<length) {
            যদি(চাবি!=nullptr) {
                *চাবি=RES_GET_KEY16(p_Res_তথ্য, p[সূচক_R]);
            }
            ফেরত makeResourceFrom16(p_Res_তথ্য, p[length+সূচক_R]);
        }
        বিরতি;
    }
    ক্ষেত্র URES_TABLE32: {
        যদি (offset != 0) { /* empty if offset==0 */
            ধ্রুবক int32_t *p= p_Res_তথ্য->p_মূল+offset;
            length=*p++;
            যদি(সূচক_R<length) {
                যদি(চাবি!=nullptr) {
                    *চাবি=RES_GET_KEY32(p_Res_তথ্য, p[সূচক_R]);
                }
                ফেরত (Resource)p[length+সূচক_R];
            }
        }
        বিরতি;
    }
    পূর্বনির্ধারিত:
        বিরতি;
    }
    ফেরত RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, ধ্রুবক অক্ষর *চাবি) {
    ধ্রুবক অক্ষর *real_চাবি=চাবি;
    int32_t idx;
    ফেরত res_get_Table_Item_By_চাবি(p_Res_তথ্য, p_Res_তথ্য->মূল_Res, &idx, &real_চাবি);
}


UBool icu::ResourceTable::get_চাবি_And_মান(int32_t i,
                                         ধ্রুবক অক্ষর *&চাবি, icu::Resource_মান &value) ধ্রুবক {
    যদি(0 <= i && i < length) {
        icu::Resource_তথ্য_মান &rd_মান = static_cast<icu::Resource_তথ্য_মান &>(value);
        যদি (keys16 != nullptr) {
            চাবি = RES_GET_KEY16(&rd_মান.get_তথ্য(), keys16[i]);
        } অন্যথা {
            চাবি = RES_GET_KEY32(&rd_মান.get_তথ্য(), keys32[i]);
        }
        Resource res;
        যদি (items16 != nullptr) {
            res = makeResourceFrom16(&rd_মান.get_তথ্য(), items16[i]);
        } অন্যথা {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_মান.setResource(res, ResourceTracer(fTraceInfo, চাবি));
        ফেরত সত্য;
    }
    ফেরত মিথ্যা;
}

UBool icu::ResourceTable::find_মান(ধ্রুবক অক্ষর *চাবি, Resource_মান &value) ধ্রুবক {
    icu::Resource_তথ্য_মান &rd_মান = static_cast<icu::Resource_তথ্য_মান &>(value);
    ধ্রুবক অক্ষর *real_চাবি = nullptr;
    int32_t i;
    যদি (keys16 != nullptr) {
        i = _res_findTableItem(&rd_মান.get_তথ্য(), keys16, length, চাবি, &real_চাবি);
    } অন্যথা {
        i = _res_findTable32Item(&rd_মান.get_তথ্য(), keys32, length, চাবি, &real_চাবি);
    }
    যদি (i >= 0) {
        Resource res;
        যদি (items16 != nullptr) {
            res = makeResourceFrom16(&rd_মান.get_তথ্য(), items16[i]);
        } অন্যথা {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_মান.setResource(res, ResourceTracer(fTraceInfo, চাবি));
        ফেরত সত্য;
    }
    ফেরত মিথ্যা;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource array, int32_t সূচক_R) {
    uint32_t offset=RES_GET_OFFSET(array);
    যদি (সূচক_R < 0) {
        ফেরত RES_BOGUS;
    }
    নির্বাচন(RES_GET_TYPE(array)) {
    ক্ষেত্র URES_ARRAY: {
        যদি (offset!=0) { /* empty if offset==0 */
            ধ্রুবক int32_t *p= p_Res_তথ্য->p_মূল+offset;
            যদি(সূচক_R<*p) {
                ফেরত (Resource)p[1+সূচক_R];
            }
        }
        বিরতি;
    }
    ক্ষেত্র URES_ARRAY16: {
        ধ্রুবক uint16_t *p=p_Res_তথ্য->p16BitUnits+offset;
        যদি(সূচক_R<*p) {
            ফেরত makeResourceFrom16(p_Res_তথ্য, p[1+সূচক_R]);
        }
        বিরতি;
    }
    পূর্বনির্ধারিত:
        বিরতি;
    }
    ফেরত RES_BOGUS;
}

uint32_t icu::ResourceArray::internalGetResource(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, int32_t i) ধ্রুবক {
    যদি (items16 != nullptr) {
        ফেরত makeResourceFrom16(p_Res_তথ্য, items16[i]);
    } অন্যথা {
        ফেরত items32[i];
    }
}

UBool icu::ResourceArray::get_মান(int32_t i, icu::Resource_মান &value) ধ্রুবক {
    যদি(0 <= i && i < length) {
        icu::Resource_তথ্য_মান &rd_মান = static_cast<icu::Resource_তথ্য_মান &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_মান.setResource(
            internalGetResource(&rd_মান.get_তথ্য(), i),
            ResourceTracer(fTraceInfo, i));
        ফেরত সত্য;
    }
    ফেরত মিথ্যা;
}

U_CFUNC Resource
res_findResource(ধ্রুবক Resource_তথ্য *p_Res_তথ্য, Resource r, অক্ষর** path, ধ্রুবক অক্ষর** চাবি) {
  অক্ষর *pathP = *path, *পরবর্তী_Sep_P = *path;
  অক্ষর *close_সূচক = nullptr;
  Resource t1 = r;
  Resource t2;
  int32_t সূচক_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  যদি(!uprv_strlen(pathP)) {
      ফেরত r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  যদি(!URES_IS_CONTAINER(type)) {
      ফেরত RES_BOGUS;
  }
  
  যখন(পরবর্তী_Sep_P && *pathP && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    পরবর্তী_Sep_P = uprv_strchr(pathP, RES_PATH_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    যদি(পরবর্তী_Sep_P != nullptr) {
      যদি(পরবর্তী_Sep_P == pathP) {
        // Empty key string.
        ফেরত RES_BOGUS;
      }
      *পরবর্তী_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *path = পরবর্তী_Sep_P+1;
    } অন্যথা {
      *path = uprv_strchr(pathP, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    যদি(URES_IS_TABLE(type)) {
      *চাবি = pathP;
      t2 = res_get_Table_Item_By_চাবি(p_Res_তথ্য, t1, &সূচক_R, চাবি);
    } অন্যথা যদি(URES_IS_ARRAY(type)) {
      সূচক_R = uprv_strtol(pathP, &close_সূচক, 10);
      যদি(সূচক_R >= 0 && *close_সূচক == 0) {
        t2 = res_getArrayItem(p_Res_তথ্য, t1, সূচক_R);
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

  ফেরত t1;
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
প্রকারলেখ কাঠামো Row {
    int32_t চাবি_সূচক, sort_সূচক;
} Row;

স্থির int32_t U_CALLCONV
ures_compareRows(ধ্রুবক শূন্য *context, ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    ধ্রুবক অক্ষর* চাবি_Chars = static_cast<ধ্রুবক অক্ষর*>(context);
    ফেরত static_cast<int32_t>(uprv_strcmp(চাবি_Chars + static_cast<ধ্রুবক Row*>(left)->চাবি_সূচক,
                                            চাবি_Chars + static_cast<ধ্রুবক Row*>(ডান)->চাবি_সূচক));
}

প্রকারলেখ কাঠামো অস্থায়ী_Table_2 {
    ধ্রুবক অক্ষর *চাবি_Chars;
    Row *rows;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_চাবি_Limit;
    uint8_t majorFormatVersion;
} অস্থায়ী_Table_2;

গণনা {
    স্তূপ_ROW_CAPACITY=200
};

/* The table item key string is not locally available. */
স্থির ধ্রুবক অক্ষর *ধ্রুবক g_Unknown_চাবি="";

#যদি !UCONFIG_NO_COLLATION
// resource table key for collation binaries
স্থির ধ্রুবক char16_t g_Collation_Bin_চাবি[]=u"%%CollationBin";
#endif

/*
 * swap one resource item
 */
স্থির শূন্য
ures_swapResource(ধ্রুবক UDataSwapper *ds,
                  ধ্রুবক Resource *inBundle, Resource *outBundle,
                  Resource res, /* caller swaps res itself */
                  ধ্রুবক অক্ষর *চাবি,
                  অস্থায়ী_Table_2 *p_অস্থায়ী_Table,
                  UErrorCode *pErrorCode) {
    ধ্রুবক Resource *p;
    Resource *q;
    int32_t offset, count;

    নির্বাচন(RES_GET_TYPE(res)) {
    ক্ষেত্র URES_TABLE16:
    ক্ষেত্র URES_STRING_V2:
    ক্ষেত্র URES_INT:
    ক্ষেত্র URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        ফেরত;
    পূর্বনির্ধারিত:
        বিরতি;
    }

    /* all other types use an offset to point to their data */
    offset = static_cast<int32_t>(RES_GET_OFFSET(res));
    যদি(offset==0) {
        /* special offset indicating an empty item */
        ফেরত;
    }
    যদি (p_অস্থায়ী_Table->resFlags[offset >> 5] & (static_cast<uint32_t>(1) << (offset & 0x1f))) {
        /* we already swapped this resource item */
        ফেরত;
    } অন্যথা {
        /* mark it as swapped now */
        p_অস্থায়ী_Table->resFlags[offset >> 5] |= static_cast<uint32_t>(1) << (offset & 0x1f);
    }

    p=inBundle+offset;
    q=outBundle+offset;

    নির্বাচন(RES_GET_TYPE(res)) {
    ক্ষেত্র URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    ক্ষেত্র URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, pErrorCode);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, pErrorCode);
        বিরতি;
    ক্ষেত্র URES_দ্বিমিক:
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
        বিরতি;
    ক্ষেত্র URES_TABLE:
    ক্ষেত্র URES_TABLE32:
        {
            ধ্রুবক uint16_t *pKey16;
            uint16_t *qKey16;

            ধ্রুবক int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, old_সূচক;

            যদি(RES_GET_TYPE(res)==URES_TABLE) {
                /* get table item count */
                pKey16 = reinterpret_cast<ধ্রুবক uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=nullptr;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, pErrorCode);

                offset+=((1+count)+1)/2;
            } অন্যথা {
                /* get table item count */
                pKey32 = reinterpret_cast<ধ্রুবক int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=nullptr;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, pErrorCode);

                offset+=1+count;
            }

            যদি(count==0) {
                বিরতি;
            }

            p=inBundle+offset; /* pointer to table resources */
            q=outBundle+offset;

            /* recurse */
            জন্য(i=0; i<count; ++i) {
                ধ্রুবক অক্ষর *item_চাবি=g_Unknown_চাবি;
                যদি(pKey16!=nullptr) {
                    int32_t চাবি_Offset=ds->readUInt16(pKey16[i]);
                    যদি(চাবি_Offset<p_অস্থায়ী_Table->local_চাবি_Limit) {
                        item_চাবি = reinterpret_cast<ধ্রুবক অক্ষর*>(outBundle) + চাবি_Offset;
                    }
                } অন্যথা {
                    int32_t চাবি_Offset=udata_readInt32(ds, pKey32[i]);
                    যদি(চাবি_Offset>=0) {
                        item_চাবি = reinterpret_cast<ধ্রুবক অক্ষর*>(outBundle) + চাবি_Offset;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, item_চাবি, p_অস্থায়ী_Table, pErrorCode);
                যদি(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    ফেরত;
                }
            }

            যদি(p_অস্থায়ী_Table->majorFormatVersion>1 || ds->inCharset==ds->outCharset) {
                /* no need to sort, just swap the offset/value arrays */
                যদি(pKey16!=nullptr) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, pErrorCode);
                    ds->swapArray32(ds, p, count*4, q, pErrorCode);
                } অন্যথা {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, pErrorCode);
                }
                বিরতি;
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
                জন্য(i=0; i<count; ++i) {
                    p_অস্থায়ী_Table->rows[i].চাবি_সূচক=ds->readUInt16(pKey16[i]);
                    p_অস্থায়ী_Table->rows[i].sort_সূচক=i;
                }
            } অন্যথা {
                জন্য(i=0; i<count; ++i) {
                    p_অস্থায়ী_Table->rows[i].চাবি_সূচক=udata_readInt32(ds, pKey32[i]);
                    p_অস্থায়ী_Table->rows[i].sort_সূচক=i;
                }
            }
            uprv_sortArray(p_অস্থায়ী_Table->rows, count, আকার(Row),
                           ures_compareRows, p_অস্থায়ী_Table->চাবি_Chars,
                           মিথ্যা, pErrorCode);
            যদি(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                ফেরত;
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
                    rKey16 = reinterpret_cast<uint16_t*>(p_অস্থায়ী_Table->resort);
                }
                জন্য(i=0; i<count; ++i) {
                    old_সূচক=p_অস্থায়ী_Table->rows[i].sort_সূচক;
                    ds->swapArray16(ds, pKey16+old_সূচক, 2, rKey16+i, pErrorCode);
                }
                যদি(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } অন্যথা {
                int32_t *rKey32;

                যদি(pKey32!=qKey32) {
                    rKey32=qKey32;
                } অন্যথা {
                    rKey32=p_অস্থায়ী_Table->resort;
                }
                জন্য(i=0; i<count; ++i) {
                    old_সূচক=p_অস্থায়ী_Table->rows[i].sort_সূচক;
                    ds->swapArray32(ds, pKey32+old_সূচক, 4, rKey32+i, pErrorCode);
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
                    r = reinterpret_cast<Resource*>(p_অস্থায়ী_Table->resort);
                }
                জন্য(i=0; i<count; ++i) {
                    old_সূচক=p_অস্থায়ী_Table->rows[i].sort_সূচক;
                    ds->swapArray32(ds, p+old_সূচক, 4, r+i, pErrorCode);
                }
                যদি(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        বিরতি;
    ক্ষেত্র URES_ARRAY:
        {
            Resource item;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->swapArray32(ds, p++, 4, q++, pErrorCode);

            /* recurse */
            জন্য(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, inBundle, outBundle, item, nullptr, p_অস্থায়ী_Table, pErrorCode);
                যদি(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    ফেরত;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, pErrorCode);
        }
        বিরতি;
    ক্ষেত্র URES_INT_VECTOR:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, pErrorCode);
        বিরতি;
    পূর্বনির্ধারিত:
        /* also catches RES_BOGUS */
        *pErrorCode=U_UNSUPPORTED_ERROR;
        বিরতি;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(ধ্রুবক UDataSwapper *ds,
          ধ্রুবক শূন্য *in_তথ্য, int32_t length, শূন্য *out_তথ্য,
          UErrorCode *pErrorCode) {
    ধ্রুবক UDataInfo *pInfo;
    ধ্রুবক Resource *inBundle;
    Resource মূল_Res;
    int32_t header_আকার, max_Table_দৈর্ঘ্য;

    Row rows[স্তূপ_ROW_CAPACITY];
    int32_t resort[স্তূপ_ROW_CAPACITY];
    অস্থায়ী_Table_2 অস্থায়ী_Table;

    ধ্রুবক int32_t *inIndexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_দৈর্ঘ্য, সূচক_দৈর্ঘ্য, চাবিগুলি_Bottom, চাবিগুলি_শীর্ষ, resBottom, শীর্ষ_2;

    /* udata_swapDataHeader checks the arguments */
    header_আকার=udata_swap_তথ্য_Header(ds, in_তথ্য, length, out_তথ্য, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুবক UDataInfo *)((ধ্রুবক অক্ষর *)in_তথ্য+4);
    যদি(!(
        pInfo->তথ্য_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->তথ্য_Format[1]==0x65 &&
        pInfo->তথ্য_Format[2]==0x73 &&
        pInfo->তথ্য_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->formatVersion[0]==1 && pInfo->formatVersion[1]>=1) ||
            pInfo->formatVersion[0]==2 || pInfo->formatVersion[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->তথ্য_Format[0], pInfo->তথ্য_Format[1],
                         pInfo->তথ্য_Format[2], pInfo->তথ্য_Format[3],
                         pInfo->formatVersion[0], pInfo->formatVersion[1]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ফেরত 0;
    }
    অস্থায়ী_Table.majorFormatVersion=pInfo->formatVersion[0];

    /* a resource bundle must contain at least one resource item */
    যদি(length<0) {
        bundle_দৈর্ঘ্য=-1;
    } অন্যথা {
        bundle_দৈর্ঘ্য=(length-header_আকার)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        যদি(bundle_দৈর্ঘ্য<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-header_আকার);
            *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
    }

    inBundle=(ধ্রুবক Resource *)((ধ্রুবক অক্ষর *)in_তথ্য+header_আকার);
    মূল_Res=ds->readUInt32(*inBundle);

    /* formatVersion 1.1 adds the indexes[] array */
    inIndexes=(ধ্রুবক int32_t *)(inBundle+1);

    সূচক_দৈর্ঘ্য=udata_readInt32(ds, inIndexes[URES_সূচক_দৈর্ঘ্য])&0xff;
    যদি(সূচক_দৈর্ঘ্য<=URES_সূচক_MAX_TABLE_দৈর্ঘ্য) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }
    চাবিগুলি_Bottom=1+সূচক_দৈর্ঘ্য;
    চাবিগুলি_শীর্ষ=udata_readInt32(ds, inIndexes[URES_সূচক_চাবিগুলি_শীর্ষ]);
    যদি(সূচক_দৈর্ঘ্য>URES_সূচক_16_BIT_শীর্ষ) {
        resBottom=udata_readInt32(ds, inIndexes[URES_সূচক_16_BIT_শীর্ষ]);
    } অন্যথা {
        resBottom=চাবিগুলি_শীর্ষ;
    }
    শীর্ষ_2=udata_readInt32(ds, inIndexes[URES_সূচক_BUNDLE_শীর্ষ]);
    max_Table_দৈর্ঘ্য=udata_readInt32(ds, inIndexes[URES_সূচক_MAX_TABLE_দৈর্ঘ্য]);

    যদি(0<=bundle_দৈর্ঘ্য && bundle_দৈর্ঘ্য<শীর্ষ_2) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         শীর্ষ_2, bundle_দৈর্ঘ্য);
        *pErrorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }
    যদি(চাবিগুলি_শীর্ষ>(1+সূচক_দৈর্ঘ্য)) {
        অস্থায়ী_Table.local_চাবি_Limit=চাবিগুলি_শীর্ষ<<2;
    } অন্যথা {
        অস্থায়ী_Table.local_চাবি_Limit=0;
    }

    যদি(length>=0) {
        Resource *outBundle=(Resource *)((অক্ষর *)out_তথ্য+header_আকার);

        /* track which resources we have already swapped */
        uint32_t স্তূপ_Res_Flags[স্তূপ_ROW_CAPACITY];
        int32_t res_Flags_দৈর্ঘ্য;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_দৈর্ঘ্য=(length+31)>>5;          /* number of bytes needed */
        res_Flags_দৈর্ঘ্য=(res_Flags_দৈর্ঘ্য+3)&~3;   /* multiple of 4 bytes for uint32_t */
        যদি(res_Flags_দৈর্ঘ্য<=(int32_t)আকার(স্তূপ_Res_Flags)) {
            অস্থায়ী_Table.resFlags=স্তূপ_Res_Flags;
        } অন্যথা {
            অস্থায়ী_Table.resFlags=(uint32_t *)uprv_malloc(res_Flags_দৈর্ঘ্য);
            যদি(অস্থায়ী_Table.resFlags==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                ফেরত 0;
            }
        }
        uprv_memset(অস্থায়ী_Table.resFlags, 0, res_Flags_দৈর্ঘ্য);

        /* copy the bundle for binary and inaccessible data */
        যদি(in_তথ্য!=out_তথ্য) {
            uprv_memcpy(outBundle, inBundle, 4*শীর্ষ_2);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swapInvStringBlock(ds, inBundle+চাবিগুলি_Bottom, 4*(চাবিগুলি_শীর্ষ-চাবিগুলি_Bottom),
                                    outBundle+চাবিগুলি_Bottom, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(চাবিগুলি_শীর্ষ-চাবিগুলি_Bottom));
            যদি(অস্থায়ী_Table.resFlags!=স্তূপ_Res_Flags) {
                uprv_free(অস্থায়ী_Table.resFlags);
            }
            ফেরত 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        যদি(চাবিগুলি_শীর্ষ<resBottom) {
            ds->swapArray16(ds, inBundle+চাবিগুলি_শীর্ষ, (resBottom-চাবিগুলি_শীর্ষ)*4, outBundle+চাবিগুলি_শীর্ষ, pErrorCode);
            যদি(U_FAILURE(*pErrorCode)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-চাবিগুলি_শীর্ষ));
                যদি(অস্থায়ী_Table.resFlags!=স্তূপ_Res_Flags) {
                    uprv_free(অস্থায়ী_Table.resFlags);
                }
                ফেরত 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        অস্থায়ী_Table.চাবি_Chars=(ধ্রুবক অক্ষর *)outBundle; /* sort by outCharset */
        যদি(অস্থায়ী_Table.majorFormatVersion>1 || max_Table_দৈর্ঘ্য<=স্তূপ_ROW_CAPACITY) {
            অস্থায়ী_Table.rows=rows;
            অস্থায়ী_Table.resort=resort;
        } অন্যথা {
            অস্থায়ী_Table.rows=(Row *)uprv_malloc(max_Table_দৈর্ঘ্য*আকার(Row)+max_Table_দৈর্ঘ্য*4);
            যদি(অস্থায়ী_Table.rows==nullptr) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_Table_দৈর্ঘ্য);
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                যদি(অস্থায়ী_Table.resFlags!=স্তূপ_Res_Flags) {
                    uprv_free(অস্থায়ী_Table.resFlags);
                }
                ফেরত 0;
            }
            অস্থায়ী_Table.resort=(int32_t *)(অস্থায়ী_Table.rows+max_Table_দৈর্ঘ্য);
        }

        /* swap the resources */
        ures_swapResource(ds, inBundle, outBundle, মূল_Res, nullptr, &অস্থায়ী_Table, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             মূল_Res);
        }

        যদি(অস্থায়ী_Table.rows!=rows) {
            uprv_free(অস্থায়ী_Table.rows);
        }
        যদি(অস্থায়ী_Table.resFlags!=স্তূপ_Res_Flags) {
            uprv_free(অস্থায়ী_Table.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, inBundle, চাবিগুলি_Bottom*4, outBundle, pErrorCode);
    }

    ফেরত header_আকার+4*শীর্ষ_2;
}

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
#تعریف_کن RES_GET_KEY16(p_Res_داده, کلید_جابه_جایی) \
    ((کلید_جابه_جایی)<(p_Res_داده)->local_کلید_حد ? \
        (ثابت حرف *)(p_Res_داده)->p_ریشه+(کلید_جابه_جایی) : \
        (p_Res_داده)->pool_Bundle_کلیدها+(کلید_جابه_جایی)-(p_Res_داده)->local_کلید_حد)

#تعریف_کن RES_GET_KEY32(p_Res_داده, کلید_جابه_جایی) \
    ((کلید_جابه_جایی)>=0 ? \
        (ثابت حرف *)(p_Res_داده)->p_ریشه+(کلید_جابه_جایی) : \
        (p_Res_داده)->pool_Bundle_کلیدها+((کلید_جابه_جایی)&0x7fffffff))

#تعریف_کن URESDATA_قلم_NOT_یافت -1

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
} g_خالی_String={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ساكن int32_t
res_پیدا_کن_جدول_قلم(ثابت Resource_داده *p_Res_داده, ثابت uint16_t *کلید_Offsets, int32_t length,
                   ثابت حرف *کلید, ثابت حرف **real_کلید) {
    ثابت حرف *جدول_کلید;
    int32_t mid, شروع, حد;
    صحيح نتیجه;

    /* do a binary search for the key */
    شروع=0;
    حد=length;
    طالما(شروع<حد) {
        mid = (شروع + حد) / 2;
        جدول_کلید = RES_GET_KEY16(p_Res_داده, کلید_Offsets[mid]);
        إذا (p_Res_داده->useNativeStrcmp) {
            نتیجه = uprv_strcmp(کلید, جدول_کلید);
        } وإلا {
            نتیجه = uprv_compareInvCharsAsAscii(کلید, جدول_کلید);
        }
        إذا (نتیجه < 0) {
            حد = mid;
        } وإلا إذا (نتیجه > 0) {
            شروع = mid + 1;
        } وإلا {
            /* We found it! */
            *real_کلید=جدول_کلید;
            أرجع mid;
        }
    }
    أرجع URESDATA_قلم_NOT_یافت;  /* not found or table is empty. */
}

ساكن int32_t
res_پیدا_کن_Table32_قلم(ثابت Resource_داده *p_Res_داده, ثابت int32_t *کلید_Offsets, int32_t length,
                     ثابت حرف *کلید, ثابت حرف **real_کلید) {
    ثابت حرف *جدول_کلید;
    int32_t mid, شروع, حد;
    صحيح نتیجه;

    /* do a binary search for the key */
    شروع=0;
    حد=length;
    طالما(شروع<حد) {
        mid = (شروع + حد) / 2;
        جدول_کلید = RES_GET_KEY32(p_Res_داده, کلید_Offsets[mid]);
        إذا (p_Res_داده->useNativeStrcmp) {
            نتیجه = uprv_strcmp(کلید, جدول_کلید);
        } وإلا {
            نتیجه = uprv_compareInvCharsAsAscii(کلید, جدول_کلید);
        }
        إذا (نتیجه < 0) {
            حد = mid;
        } وإلا إذا (نتیجه > 0) {
            شروع = mid + 1;
        } وإلا {
            /* We found it! */
            *real_کلید=جدول_کلید;
            أرجع mid;
        }
    }
    أرجع URESDATA_قلم_NOT_یافت;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ساكن UBool U_CALLCONV
isAcceptable(فراغ *context,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->format_نسخه, 4);
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->داده_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->داده_Format[1]==0x65 &&
        pInfo->داده_Format[2]==0x73 &&
        pInfo->داده_Format[3]==0x42 &&
        (1<=pInfo->format_نسخه[0] && pInfo->format_نسخه[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

ساكن فراغ
res_init(Resource_داده *p_Res_داده,
         UVersionInfo format_نسخه, ثابت فراغ *درون_بایتها, int32_t length,
         UError_کد *error_کد) {
    UResType ریشه_Type;

    /* get the root resource */
    p_Res_داده->p_ریشه = static_cast<ثابت int32_t*>(درون_بایتها);
    p_Res_داده->ریشه_Res = static_cast<Resource>(*p_Res_داده->p_ریشه);
    p_Res_داده->p16_بیت_Units=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    إذا(length>=0 && (length/4)<((format_نسخه[0]==1 && format_نسخه[1]==0) ? 1 : 1+5)) {
        *error_کد=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_داده);
        أرجع;
    }

    /* currently, we accept only resources that have a Table as their roots */
    ریشه_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_داده->ریشه_Res));
    إذا(!URES_IS_جدول(ریشه_Type)) {
        *error_کد=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_داده);
        أرجع;
    }

    إذا(format_نسخه[0]==1 && format_نسخه[1]==0) {
        p_Res_داده->local_کلید_حد=0x10000;  /* greater than any 16-bit key string offset */
    } وإلا {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ثابت int32_t *indexes=p_Res_داده->p_ریشه+1;
        int32_t شاخص_درازی=indexes[URES_شاخص_درازی]&0xff;
        إذا(شاخص_درازی<=URES_شاخص_MAX_جدول_درازی) {
            *error_کد=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_داده);
            أرجع;
        }
        إذا( length>=0 &&
            (length<((1+شاخص_درازی)<<2) ||
             length<(indexes[URES_شاخص_BUNDLE_بالاترین]<<2))
        ) {
            *error_کد=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_داده);
            أرجع;
        }
        إذا(indexes[URES_شاخص_کلیدها_بالاترین]>(1+شاخص_درازی)) {
            p_Res_داده->local_کلید_حد=indexes[URES_شاخص_کلیدها_بالاترین]<<2;
        }
        إذا(format_نسخه[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_داده->pool_String_شاخص_حد = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_شاخص_درازی]) >> 8);
        }
        إذا(شاخص_درازی>URES_شاخص_صفات) {
            int32_t att=indexes[URES_شاخص_صفات];
            p_Res_داده->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_داده->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_داده->استفاده_میکند_Pool_Bundle = static_cast<UBool>((att & URES_ATT_استفاده_میکند_POOL_BUNDLE) != 0);
            p_Res_داده->pool_String_شاخص_حد|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_داده->pool_String_Index16_حد = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        إذا((p_Res_داده->isPoolBundle || p_Res_داده->استفاده_میکند_Pool_Bundle) && شاخص_درازی<=URES_شاخص_POOL_CHECKSUM) {
            *error_کد=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_داده);
            أرجع;
        }
        إذا( شاخص_درازی>URES_شاخص_16_بیت_بالاترین &&
            indexes[URES_شاخص_16_بیت_بالاترین]>indexes[URES_شاخص_کلیدها_بالاترین]
        ) {
            p_Res_داده->p16_بیت_Units = reinterpret_cast<ثابت uint16_t*>(p_Res_داده->p_ریشه + indexes[URES_شاخص_کلیدها_بالاترین]);
        }
    }

    إذا(format_نسخه[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_داده->useNativeStrcmp=درست;
    }
}

U_CAPI فراغ U_EXPORT2
res_read(Resource_داده *p_Res_داده,
         ثابت UDataInfo *pInfo, ثابت فراغ *درون_بایتها, int32_t length,
         UError_کد *error_کد) {
    UVersionInfo format_نسخه;

    uprv_memset(p_Res_داده, 0, حجم(Resource_داده));
    إذا(U_FAILURE(*error_کد)) {
        أرجع;
    }
    إذا(!isAcceptable(format_نسخه, اشاره_تهی, اشاره_تهی, pInfo)) {
        *error_کد=U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    res_init(p_Res_داده, format_نسخه, درون_بایتها, length, error_کد);
}

U_CFUNC فراغ
res_بارگیری(Resource_داده *p_Res_داده,
         ثابت حرف *مسیر, ثابت حرف *name, UError_کد *error_کد) {
    UVersionInfo format_نسخه;

    uprv_memset(p_Res_داده, 0, حجم(Resource_داده));

    /* load the ResourceBundle file */
    p_Res_داده->داده=udata_openChoice(مسیر, "res", name, isAcceptable, format_نسخه, error_کد);
    إذا(U_FAILURE(*error_کد)) {
        أرجع;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_داده, format_نسخه, udata_get_حافظه(p_Res_داده->داده), -1, error_کد);
}

U_CFUNC فراغ
res_unload(Resource_داده *p_Res_داده) {
    إذا(p_Res_داده->داده!=اشاره_تهی) {
        udata_close(p_Res_داده->داده);
        p_Res_داده->داده=اشاره_تهی;
    }
}

ساكن ثابت int8_t g_عمومی_Types[URES_حد] = {
    URES_STRING,
    URES_دودویی,
    URES_جدول,
    URES_ALIAS,

    URES_جدول,     /* URES_TABLE32 */
    URES_جدول,     /* URES_TABLE16 */
    URES_STRING,    /* URES_STRING_V2 */
    URES_INT,

    URES_ARRAY,
    URES_ARRAY,     /* URES_ARRAY16 */
    URES_NONE,
    URES_NONE,

    URES_NONE,
    URES_NONE,
    URES_INT_وکتور,
    URES_NONE
};

U_CAPI UResType U_EXPORT2
res_get_عمومی_Type(Resource res) {
    أرجع (UResType)g_عمومی_Types[RES_GET_TYPE(res)];
}

U_CAPI ثابت char16_t * U_EXPORT2
res_getStringNoTrace(ثابت Resource_داده *p_Res_داده, Resource res, int32_t *p_درازی) {
    ثابت char16_t *p;
    uint32_t جابه_جایی=RES_GET_جابه_جایی(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        إذا((int32_t)جابه_جایی<p_Res_داده->pool_String_شاخص_حد) {
            p=(ثابت char16_t *)p_Res_داده->pool_Bundle_رشتهها+جابه_جایی;
        } وإلا {
            p=(ثابت char16_t *)p_Res_داده->p16_بیت_Units+(جابه_جایی-p_Res_داده->pool_String_شاخص_حد);
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
    } وإلا إذا(res==جابه_جایی) /* RES_GET_TYPE(res)==URES_STRING */ {
        ثابت int32_t *p32= res==0 ? &g_خالی_String.length : p_Res_داده->p_ریشه+res;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=اشاره_تهی;
        length=0;
    }
    إذا(p_درازی) {
        *p_درازی=length;
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
UBool isNoInheritanceMarker(ثابت Resource_داده *p_Res_داده, Resource res) {
    uint32_t جابه_جایی=RES_GET_جابه_جایی(res);
    إذا (جابه_جایی == 0) {
        // empty string
    } وإلا إذا (res == جابه_جایی) {
        ثابت int32_t *p32=p_Res_داده->p_ریشه+res;
        int32_t length=*p32;
        ثابت char16_t* p = reinterpret_cast<ثابت char16_t*>(p32);
        أرجع length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } وإلا إذا (RES_GET_TYPE(res) == URES_STRING_V2) {
        ثابت char16_t *p;
        إذا (static_cast<int32_t>(جابه_جایی) < p_Res_داده->pool_String_شاخص_حد) {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_داده->pool_Bundle_رشتهها) + جابه_جایی;
        } وإلا {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_داده->p16_بیت_Units) + (جابه_جایی - p_Res_داده->pool_String_شاخص_حد);
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

int32_t getStringArray(ثابت Resource_داده *p_Res_داده, ثابت icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    إذا(dest == اشاره_تهی ? capacity != 0 : capacity < 0) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    int32_t length = array.get_اندازه();
    إذا(length == 0) {
        أرجع 0;
    }
    إذا(length > capacity) {
        error_کد = U_میانگیر_سرریز_ERROR;
        أرجع length;
    }
    لكل(int32_t i = 0; i < length; ++i) {
        int32_t s_درازی;
        // No tracing: handled by the caller
        ثابت char16_t *s = res_getStringNoTrace(p_Res_داده, array.داخلی_Get_Resource(p_Res_داده, i), &s_درازی);
        إذا(s == اشاره_تهی) {
            error_کد = U_RESOURCE_TYPE_MISMATCH;
            أرجع 0;
        }
        dest[i].مجموعه_مقصد(درست, s, s_درازی);
    }
    أرجع length;
}

}  // namespace

U_CAPI ثابت char16_t * U_EXPORT2
res_getAlias(ثابت Resource_داده *p_Res_داده, Resource res, int32_t *p_درازی) {
    ثابت char16_t *p;
    uint32_t جابه_جایی=RES_GET_جابه_جایی(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_ALIAS) {
        ثابت int32_t *p32= جابه_جایی==0 ? &g_خالی_String.length : p_Res_داده->p_ریشه+جابه_جایی;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=اشاره_تهی;
        length=0;
    }
    إذا(p_درازی) {
        *p_درازی=length;
    }
    أرجع p;
}

U_CAPI ثابت uint8_t * U_EXPORT2
res_get_دودویی_No_Trace(ثابت Resource_داده *p_Res_داده, Resource res, int32_t *p_درازی) {
    ثابت uint8_t *p;
    uint32_t جابه_جایی=RES_GET_جابه_جایی(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_دودویی) {
        ثابت int32_t *p32= جابه_جایی==0 ? (ثابت int32_t*)&gEmpty32 : p_Res_داده->p_ریشه+جابه_جایی;
        length=*p32++;
        p=(ثابت uint8_t *)p32;
    } وإلا {
        p=اشاره_تهی;
        length=0;
    }
    إذا(p_درازی) {
        *p_درازی=length;
    }
    أرجع p;
}


U_CAPI ثابت int32_t * U_EXPORT2
res_get_Int_وکتور_No_Trace(ثابت Resource_داده *p_Res_داده, Resource res, int32_t *p_درازی) {
    ثابت int32_t *p;
    uint32_t جابه_جایی=RES_GET_جابه_جایی(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_INT_وکتور) {
        p= جابه_جایی==0 ? (ثابت int32_t *)&gEmpty32 : p_Res_داده->p_ریشه+جابه_جایی;
        length=*p++;
    } وإلا {
        p=اشاره_تهی;
        length=0;
    }
    إذا(p_درازی) {
        *p_درازی=length;
    }
    أرجع p;
}

U_CAPI int32_t U_EXPORT2
res_شمار_Array_اقلام(ثابت Resource_داده *p_Res_داده, Resource res) {
    uint32_t جابه_جایی=RES_GET_جابه_جایی(res);
    بدل(RES_GET_TYPE(res)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
    حالة URES_دودویی:
    حالة URES_ALIAS:
    حالة URES_INT:
    حالة URES_INT_وکتور:
        أرجع 1;
    حالة URES_ARRAY:
    حالة URES_TABLE32:
        أرجع جابه_جایی==0 ? 0 : *(p_Res_داده->p_ریشه+جابه_جایی);
    حالة URES_جدول:
        أرجع جابه_جایی==0 ? 0 : *((ثابت uint16_t *)(p_Res_داده->p_ریشه+جابه_جایی));
    حالة URES_ARRAY16:
    حالة URES_TABLE16:
        أرجع p_Res_داده->p16_بیت_Units[جابه_جایی];
    افتراضي:
        أرجع 0;
    }
}

U_NAMESPACE_BEGIN

Resource_داده_مقدار::~Resource_داده_مقدار() {}

UResType Resource_داده_مقدار::getType() ثابت {
    أرجع res_get_عمومی_Type(res);
}

ثابت char16_t *Resource_داده_مقدار::getString(int32_t &length, UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    ثابت char16_t *s = res_getString(fTraceInfo, &get_داده(), res, &length);
    إذا(s == اشاره_تهی) {
        error_کد = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ثابت char16_t *Resource_داده_مقدار::getAliasString(int32_t &length, UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    ثابت char16_t *s = res_getAlias(&get_داده(), res, &length);
    إذا(s == اشاره_تهی) {
        error_کد = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

int32_t Resource_داده_مقدار::getInt(UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        error_کد = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getInt(fTraceInfo, res);
}

uint32_t Resource_داده_مقدار::getUInt(UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        error_کد = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getUInt(fTraceInfo, res);
}

ثابت int32_t *Resource_داده_مقدار::get_Int_وکتور(int32_t &length, UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    ثابت int32_t *iv = res_get_Int_وکتور(fTraceInfo, &get_داده(), res, &length);
    إذا(iv == اشاره_تهی) {
        error_کد = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع iv;
}

ثابت uint8_t *Resource_داده_مقدار::get_دودویی(int32_t &length, UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    ثابت uint8_t *b = res_get_دودویی(fTraceInfo, &get_داده(), res, &length);
    إذا(b == اشاره_تهی) {
        error_کد = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع b;
}

ResourceArray Resource_داده_مقدار::getArray(UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع {};
    }
    ثابت uint16_t *items16 = اشاره_تهی;
    ثابت Resource *items32 = اشاره_تهی;
    uint32_t جابه_جایی=RES_GET_جابه_جایی(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_ARRAY:
        إذا (جابه_جایی!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ثابت Resource*>(get_داده().p_ریشه) + جابه_جایی;
            length = *items32++;
        }
        اكسر;
    حالة URES_ARRAY16:
        items16 = get_داده().p16_بیت_Units+جابه_جایی;
        length = *items16++;
        اكسر;
    افتراضي:
        error_کد = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع ResourceArray(items16, items32, length, fTraceInfo);
}

Resource_جدول Resource_داده_مقدار::get_جدول(UError_کد &error_کد) ثابت {
    إذا(U_FAILURE(error_کد)) {
        أرجع {};
    }
    ثابت uint16_t *keys16 = اشاره_تهی;
    ثابت int32_t *keys32 = اشاره_تهی;
    ثابت uint16_t *items16 = اشاره_تهی;
    ثابت Resource *items32 = اشاره_تهی;
    uint32_t جابه_جایی = RES_GET_جابه_جایی(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_جدول:
        إذا (جابه_جایی != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ثابت uint16_t*>(get_داده().p_ریشه + جابه_جایی);
            length = *keys16++;
            items32 = reinterpret_cast<ثابت Resource*>(keys16 + length + (~length & 1));
        }
        اكسر;
    حالة URES_TABLE16:
        keys16 = get_داده().p16_بیت_Units+جابه_جایی;
        length = *keys16++;
        items16 = keys16 + length;
        اكسر;
    حالة URES_TABLE32:
        إذا (جابه_جایی != 0) {  // empty if offset==0
            keys32 = get_داده().p_ریشه+جابه_جایی;
            length = *keys32++;
            items32 = reinterpret_cast<ثابت Resource*>(keys32) + length;
        }
        اكسر;
    افتراضي:
        error_کد = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع Resource_جدول(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_داده_مقدار::isNoInheritanceMarker() ثابت {
    أرجع ::isNoInheritanceMarker(&get_داده(), res);
}

int32_t Resource_داده_مقدار::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UError_کد &error_کد) ثابت {
    أرجع ::getStringArray(&get_داده(), getArray(error_کد), dest, capacity, error_کد);
}

int32_t Resource_داده_مقدار::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UError_کد &error_کد) ثابت {
    إذا(URES_IS_ARRAY(res)) {
        أرجع ::getStringArray(&get_داده(), getArray(error_کد), dest, capacity, error_کد);
    }
    إذا(U_FAILURE(error_کد)) {
        أرجع 0;
    }
    إذا(dest == اشاره_تهی ? capacity != 0 : capacity < 0) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    إذا(capacity < 1) {
        error_کد = U_میانگیر_سرریز_ERROR;
        أرجع 1;
    }
    int32_t s_درازی;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_داده(), res, &s_درازی);
    إذا(s != اشاره_تهی) {
        dest[0].مجموعه_مقصد(درست, s, s_درازی);
        أرجع 1;
    }
    error_کد = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0;
}

UnicodeString Resource_داده_مقدار::get_String_Or_اولی_از_Array(UError_کد &error_کد) ثابت {
    UnicodeString us;
    إذا(U_FAILURE(error_کد)) {
        أرجع us;
    }
    int32_t s_درازی;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_داده(), res, &s_درازی);
    إذا(s != اشاره_تهی) {
        us.مجموعه_مقصد(درست, s, s_درازی);
        أرجع us;
    }
    ResourceArray array = getArray(error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع us;
    }
    إذا(array.get_اندازه() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_داده(), array.داخلی_Get_Resource(&get_داده(), 0), &s_درازی);
        إذا(s != اشاره_تهی) {
            us.مجموعه_مقصد(درست, s, s_درازی);
            أرجع us;
        }
    }
    error_کد = U_RESOURCE_TYPE_MISMATCH;
    أرجع us;
}

U_NAMESPACE_انتها

ساكن Resource
makeResourceFrom16(ثابت Resource_داده *p_Res_داده, int32_t res16) {
    إذا(res16<p_Res_داده->pool_String_Index16_حد) {
        // Pool string, nothing to do.
    } وإلا {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_داده->pool_String_Index16_حد+p_Res_داده->pool_String_شاخص_حد;
    }
    أرجع URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_جدول_قلم_By_کلید(ثابت Resource_داده *p_Res_داده, Resource جدول,
                      int32_t *شاخص_R, ثابت حرف **کلید) {
    uint32_t جابه_جایی=RES_GET_جابه_جایی(جدول);
    int32_t length;
    int32_t idx;
    إذا(کلید == اشاره_تهی || *کلید == اشاره_تهی) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(جدول)) {
    حالة URES_جدول: {
        إذا (جابه_جایی!=0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_داده->p_ریشه+جابه_جایی);
            length=*p++;
            *شاخص_R=idx=res_پیدا_کن_جدول_قلم(p_Res_داده, p, length, *کلید, کلید);
            إذا(idx>=0) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                أرجع p32[idx];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=p_Res_داده->p16_بیت_Units+جابه_جایی;
        length=*p++;
        *شاخص_R=idx=res_پیدا_کن_جدول_قلم(p_Res_داده, p, length, *کلید, کلید);
        إذا(idx>=0) {
            أرجع makeResourceFrom16(p_Res_داده, p[length+idx]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (جابه_جایی!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_داده->p_ریشه+جابه_جایی;
            length=*p++;
            *شاخص_R=idx=res_پیدا_کن_Table32_قلم(p_Res_داده, p, length, *کلید, کلید);
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
res_get_جدول_قلم_By_شاخص(ثابت Resource_داده *p_Res_داده, Resource جدول,
                        int32_t شاخص_R, ثابت حرف **کلید) {
    uint32_t جابه_جایی=RES_GET_جابه_جایی(جدول);
    int32_t length;
    إذا (شاخص_R < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(جدول)) {
    حالة URES_جدول: {
        إذا (جابه_جایی != 0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_داده->p_ریشه+جابه_جایی);
            length=*p++;
            إذا(شاخص_R<length) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                إذا(کلید!=اشاره_تهی) {
                    *کلید=RES_GET_KEY16(p_Res_داده, p[شاخص_R]);
                }
                أرجع p32[شاخص_R];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=p_Res_داده->p16_بیت_Units+جابه_جایی;
        length=*p++;
        إذا(شاخص_R<length) {
            إذا(کلید!=اشاره_تهی) {
                *کلید=RES_GET_KEY16(p_Res_داده, p[شاخص_R]);
            }
            أرجع makeResourceFrom16(p_Res_داده, p[length+شاخص_R]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (جابه_جایی != 0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_داده->p_ریشه+جابه_جایی;
            length=*p++;
            إذا(شاخص_R<length) {
                إذا(کلید!=اشاره_تهی) {
                    *کلید=RES_GET_KEY32(p_Res_داده, p[شاخص_R]);
                }
                أرجع (Resource)p[length+شاخص_R];
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
res_getResource(ثابت Resource_داده *p_Res_داده, ثابت حرف *کلید) {
    ثابت حرف *real_کلید=کلید;
    int32_t idx;
    أرجع res_get_جدول_قلم_By_کلید(p_Res_داده, p_Res_داده->ریشه_Res, &idx, &real_کلید);
}


UBool icu::Resource_جدول::get_کلید_And_مقدار(int32_t i,
                                         ثابت حرف *&کلید, icu::Resource_مقدار &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_داده_مقدار &rd_مقدار = static_cast<icu::Resource_داده_مقدار &>(value);
        إذا (keys16 != اشاره_تهی) {
            کلید = RES_GET_KEY16(&rd_مقدار.get_داده(), keys16[i]);
        } وإلا {
            کلید = RES_GET_KEY32(&rd_مقدار.get_داده(), keys32[i]);
        }
        Resource res;
        إذا (items16 != اشاره_تهی) {
            res = makeResourceFrom16(&rd_مقدار.get_داده(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_مقدار.مجموعه_Resource(res, ResourceTracer(fTraceInfo, کلید));
        أرجع درست;
    }
    أرجع خطأ;
}

UBool icu::Resource_جدول::پیدا_کن_مقدار(ثابت حرف *کلید, Resource_مقدار &value) ثابت {
    icu::Resource_داده_مقدار &rd_مقدار = static_cast<icu::Resource_داده_مقدار &>(value);
    ثابت حرف *real_کلید = اشاره_تهی;
    int32_t i;
    إذا (keys16 != اشاره_تهی) {
        i = res_پیدا_کن_جدول_قلم(&rd_مقدار.get_داده(), keys16, length, کلید, &real_کلید);
    } وإلا {
        i = res_پیدا_کن_Table32_قلم(&rd_مقدار.get_داده(), keys32, length, کلید, &real_کلید);
    }
    إذا (i >= 0) {
        Resource res;
        إذا (items16 != اشاره_تهی) {
            res = makeResourceFrom16(&rd_مقدار.get_داده(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_مقدار.مجموعه_Resource(res, ResourceTracer(fTraceInfo, کلید));
        أرجع درست;
    }
    أرجع خطأ;
}

U_CAPI Resource U_EXPORT2
res_get_Array_قلم(ثابت Resource_داده *p_Res_داده, Resource array, int32_t شاخص_R) {
    uint32_t جابه_جایی=RES_GET_جابه_جایی(array);
    إذا (شاخص_R < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(array)) {
    حالة URES_ARRAY: {
        إذا (جابه_جایی!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_داده->p_ریشه+جابه_جایی;
            إذا(شاخص_R<*p) {
                أرجع (Resource)p[1+شاخص_R];
            }
        }
        اكسر;
    }
    حالة URES_ARRAY16: {
        ثابت uint16_t *p=p_Res_داده->p16_بیت_Units+جابه_جایی;
        إذا(شاخص_R<*p) {
            أرجع makeResourceFrom16(p_Res_داده, p[1+شاخص_R]);
        }
        اكسر;
    }
    افتراضي:
        اكسر;
    }
    أرجع RES_BOGUS;
}

uint32_t icu::ResourceArray::داخلی_Get_Resource(ثابت Resource_داده *p_Res_داده, int32_t i) ثابت {
    إذا (items16 != اشاره_تهی) {
        أرجع makeResourceFrom16(p_Res_داده, items16[i]);
    } وإلا {
        أرجع items32[i];
    }
}

UBool icu::ResourceArray::get_مقدار(int32_t i, icu::Resource_مقدار &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_داده_مقدار &rd_مقدار = static_cast<icu::Resource_داده_مقدار &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_مقدار.مجموعه_Resource(
            داخلی_Get_Resource(&rd_مقدار.get_داده(), i),
            ResourceTracer(fTraceInfo, i));
        أرجع درست;
    }
    أرجع خطأ;
}

U_CFUNC Resource
res_پیدا_کن_Resource(ثابت Resource_داده *p_Res_داده, Resource r, حرف** مسیر, ثابت حرف** کلید) {
  حرف *مسیر_P = *مسیر, *بعدی_Sep_P = *مسیر;
  حرف *close_شاخص = اشاره_تهی;
  Resource t1 = r;
  Resource t2;
  int32_t شاخص_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  إذا(!uprv_strlen(مسیر_P)) {
      أرجع r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  إذا(!URES_IS_CONTAINER(type)) {
      أرجع RES_BOGUS;
  }
  
  طالما(بعدی_Sep_P && *مسیر_P && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    بعدی_Sep_P = uprv_strchr(مسیر_P, RES_مسیر_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    إذا(بعدی_Sep_P != اشاره_تهی) {
      إذا(بعدی_Sep_P == مسیر_P) {
        // Empty key string.
        أرجع RES_BOGUS;
      }
      *بعدی_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *مسیر = بعدی_Sep_P+1;
    } وإلا {
      *مسیر = uprv_strchr(مسیر_P, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    إذا(URES_IS_جدول(type)) {
      *کلید = مسیر_P;
      t2 = res_get_جدول_قلم_By_کلید(p_Res_داده, t1, &شاخص_R, کلید);
    } وإلا إذا(URES_IS_ARRAY(type)) {
      شاخص_R = uprv_strtol(مسیر_P, &close_شاخص, 10);
      إذا(شاخص_R >= 0 && *close_شاخص == 0) {
        t2 = res_get_Array_قلم(p_Res_داده, t1, شاخص_R);
      } وإلا {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *کلید = اشاره_تهی;
    } وإلا { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    مسیر_P = *مسیر;
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
تعريفنوع بنية ردیف_2 {
    int32_t کلید_شاخص, مرتب_سازی_شاخص;
} ردیف_2;

ساكن int32_t U_CALLCONV
ures_compare_ردیفها(ثابت فراغ *context, ثابت فراغ *چپ, ثابت فراغ *راست) {
    ثابت حرف* کلید_Chars = static_cast<ثابت حرف*>(context);
    أرجع static_cast<int32_t>(uprv_strcmp(کلید_Chars + static_cast<ثابت ردیف_2*>(چپ)->کلید_شاخص,
                                            کلید_Chars + static_cast<ثابت ردیف_2*>(راست)->کلید_شاخص));
}

تعريفنوع بنية موقت_جدول_2 {
    ثابت حرف *کلید_Chars;
    ردیف_2 *ردیفها;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_کلید_حد;
    uint8_t major_Format_نسخه;
} موقت_جدول_2;

تعداد {
    پشته_ردیف_ظرفیت=200
};

/* The table item key string is not locally available. */
ساكن ثابت حرف *ثابت g_Unknown_کلید="";

#إذا !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ساكن ثابت char16_t g_Collation_Bin_کلید[]=u"%%CollationBin";
#پایان_اگر

/*
 * swap one resource item
 */
ساكن فراغ
ures_جابجا_Resource(ثابت UDataSwapper *ds,
                  ثابت Resource *درون_Bundle, Resource *بیرون_Bundle,
                  Resource res, /* caller swaps res itself */
                  ثابت حرف *کلید,
                  موقت_جدول_2 *p_موقت_جدول,
                  UError_کد *p_Error_کد) {
    ثابت Resource *p;
    Resource *q;
    int32_t جابه_جایی, count;

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
    جابه_جایی = static_cast<int32_t>(RES_GET_جابه_جایی(res));
    إذا(جابه_جایی==0) {
        /* special offset indicating an empty item */
        أرجع;
    }
    إذا (p_موقت_جدول->resFlags[جابه_جایی >> 5] & (static_cast<uint32_t>(1) << (جابه_جایی & 0x1f))) {
        /* we already swapped this resource item */
        أرجع;
    } وإلا {
        /* mark it as swapped now */
        p_موقت_جدول->resFlags[جابه_جایی >> 5] |= static_cast<uint32_t>(1) << (جابه_جایی & 0x1f);
    }

    p=درون_Bundle+جابه_جایی;
    q=بیرون_Bundle+جابه_جایی;

    بدل(RES_GET_TYPE(res)) {
    حالة URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    حالة URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->جابجا_Array32(ds, p, 4, q, p_Error_کد);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->جابجا_Array16(ds, p+1, 2*count, q+1, p_Error_کد);
        اكسر;
    حالة URES_دودویی:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->جابجا_Array32(ds, p, 4, q, p_Error_کد);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#إذا !UCONFIG_NO_COLLATION
        إذا( کلید!=اشاره_تهی &&  /* the binary is in a table */
            (کلید!=g_Unknown_کلید ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, کلید, -1,
                                       g_Collation_Bin_کلید, UPRV_LENGTHOF(g_Collation_Bin_کلید)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_دودویی(ds, p+1, count))
        ) {
            ucol_جابجا(ds, p+1, count, q+1, p_Error_کد);
        }
#پایان_اگر
        اكسر;
    حالة URES_جدول:
    حالة URES_TABLE32:
        {
            ثابت uint16_t *pKey16;
            uint16_t *qKey16;

            ثابت int32_t *pKey32;
            int32_t *qKey32;

            Resource قلم;
            int32_t i, old_شاخص;

            إذا(RES_GET_TYPE(res)==URES_جدول) {
                /* get table item count */
                pKey16 = reinterpret_cast<ثابت uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=اشاره_تهی;

                /* swap count */
                ds->جابجا_Array16(ds, pKey16++, 2, qKey16++, p_Error_کد);

                جابه_جایی+=((1+count)+1)/2;
            } وإلا {
                /* get table item count */
                pKey32 = reinterpret_cast<ثابت int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=اشاره_تهی;

                /* swap count */
                ds->جابجا_Array32(ds, pKey32++, 4, qKey32++, p_Error_کد);

                جابه_جایی+=1+count;
            }

            إذا(count==0) {
                اكسر;
            }

            p=درون_Bundle+جابه_جایی; /* pointer to table resources */
            q=بیرون_Bundle+جابه_جایی;

            /* recurse */
            لكل(i=0; i<count; ++i) {
                ثابت حرف *قلم_کلید=g_Unknown_کلید;
                إذا(pKey16!=اشاره_تهی) {
                    int32_t کلید_جابه_جایی=ds->readUInt16(pKey16[i]);
                    إذا(کلید_جابه_جایی<p_موقت_جدول->local_کلید_حد) {
                        قلم_کلید = reinterpret_cast<ثابت حرف*>(بیرون_Bundle) + کلید_جابه_جایی;
                    }
                } وإلا {
                    int32_t کلید_جابه_جایی=udata_readInt32(ds, pKey32[i]);
                    إذا(کلید_جابه_جایی>=0) {
                        قلم_کلید = reinterpret_cast<ثابت حرف*>(بیرون_Bundle) + کلید_جابه_جایی;
                    }
                }
                قلم=ds->readUInt32(p[i]);
                ures_جابجا_Resource(ds, درون_Bundle, بیرون_Bundle, قلم, قلم_کلید, p_موقت_جدول, p_Error_کد);
                إذا(U_FAILURE(*p_Error_کد)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, قلم);
                    أرجع;
                }
            }

            إذا(p_موقت_جدول->major_Format_نسخه>1 || ds->درون_Charset==ds->بیرون_Charset) {
                /* no need to sort, just swap the offset/value arrays */
                إذا(pKey16!=اشاره_تهی) {
                    ds->جابجا_Array16(ds, pKey16, count*2, qKey16, p_Error_کد);
                    ds->جابجا_Array32(ds, p, count*4, q, p_Error_کد);
                } وإلا {
                    /* swap key offsets and items as one array */
                    ds->جابجا_Array32(ds, pKey32, count*2*4, qKey32, p_Error_کد);
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
            إذا(pKey16!=اشاره_تهی) {
                لكل(i=0; i<count; ++i) {
                    p_موقت_جدول->ردیفها[i].کلید_شاخص=ds->readUInt16(pKey16[i]);
                    p_موقت_جدول->ردیفها[i].مرتب_سازی_شاخص=i;
                }
            } وإلا {
                لكل(i=0; i<count; ++i) {
                    p_موقت_جدول->ردیفها[i].کلید_شاخص=udata_readInt32(ds, pKey32[i]);
                    p_موقت_جدول->ردیفها[i].مرتب_سازی_شاخص=i;
                }
            }
            uprv_مرتب_سازی_Array(p_موقت_جدول->ردیفها, count, حجم(ردیف_2),
                           ures_compare_ردیفها, p_موقت_جدول->کلید_Chars,
                           خطأ, p_Error_کد);
            إذا(U_FAILURE(*p_Error_کد)) {
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
            /*کلیدها*/
            إذا(pKey16!=اشاره_تهی) {
                uint16_t *rKey16;

                إذا(pKey16!=qKey16) {
                    rKey16=qKey16;
                } وإلا {
                    rKey16 = reinterpret_cast<uint16_t*>(p_موقت_جدول->resort);
                }
                لكل(i=0; i<count; ++i) {
                    old_شاخص=p_موقت_جدول->ردیفها[i].مرتب_سازی_شاخص;
                    ds->جابجا_Array16(ds, pKey16+old_شاخص, 2, rKey16+i, p_Error_کد);
                }
                إذا(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } وإلا {
                int32_t *rKey32;

                إذا(pKey32!=qKey32) {
                    rKey32=qKey32;
                } وإلا {
                    rKey32=p_موقت_جدول->resort;
                }
                لكل(i=0; i<count; ++i) {
                    old_شاخص=p_موقت_جدول->ردیفها[i].مرتب_سازی_شاخص;
                    ds->جابجا_Array32(ds, pKey32+old_شاخص, 4, rKey32+i, p_Error_کد);
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
                    r = reinterpret_cast<Resource*>(p_موقت_جدول->resort);
                }
                لكل(i=0; i<count; ++i) {
                    old_شاخص=p_موقت_جدول->ردیفها[i].مرتب_سازی_شاخص;
                    ds->جابجا_Array32(ds, p+old_شاخص, 4, r+i, p_Error_کد);
                }
                إذا(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        اكسر;
    حالة URES_ARRAY:
        {
            Resource قلم;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->جابجا_Array32(ds, p++, 4, q++, p_Error_کد);

            /* recurse */
            لكل(i=0; i<count; ++i) {
                قلم=ds->readUInt32(p[i]);
                ures_جابجا_Resource(ds, درون_Bundle, بیرون_Bundle, قلم, اشاره_تهی, p_موقت_جدول, p_Error_کد);
                إذا(U_FAILURE(*p_Error_کد)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, قلم);
                    أرجع;
                }
            }

            /* swap items */
            ds->جابجا_Array32(ds, p, 4*count, q, p_Error_کد);
        }
        اكسر;
    حالة URES_INT_وکتور:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->جابجا_Array32(ds, p, 4*(1+count), q, p_Error_کد);
        اكسر;
    افتراضي:
        /* also catches RES_BOGUS */
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        اكسر;
    }
}

U_CAPI int32_t U_EXPORT2
ures_جابجا(ثابت UDataSwapper *ds,
          ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
          UError_کد *p_Error_کد) {
    ثابت UDataInfo *pInfo;
    ثابت Resource *درون_Bundle;
    Resource ریشه_Res;
    int32_t سرآیند_اندازه, max_جدول_درازی;

    ردیف_2 ردیفها[پشته_ردیف_ظرفیت];
    int32_t resort[پشته_ردیف_ظرفیت];
    موقت_جدول_2 موقت_جدول;

    ثابت int32_t *درون_Indexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_درازی, شاخص_درازی, کلیدها_Bottom, کلیدها_بالاترین, resBottom, بالاترین;

    /* udata_swapDataHeader checks the arguments */
    سرآیند_اندازه=udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)درون_داده+4);
    إذا(!(
        pInfo->داده_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->داده_Format[1]==0x65 &&
        pInfo->داده_Format[2]==0x73 &&
        pInfo->داده_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->format_نسخه[0]==1 && pInfo->format_نسخه[1]>=1) ||
            pInfo->format_نسخه[0]==2 || pInfo->format_نسخه[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->داده_Format[0], pInfo->داده_Format[1],
                         pInfo->داده_Format[2], pInfo->داده_Format[3],
                         pInfo->format_نسخه[0], pInfo->format_نسخه[1]);
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }
    موقت_جدول.major_Format_نسخه=pInfo->format_نسخه[0];

    /* a resource bundle must contain at least one resource item */
    إذا(length<0) {
        bundle_درازی=-1;
    } وإلا {
        bundle_درازی=(length-سرآیند_اندازه)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        إذا(bundle_درازی<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-سرآیند_اندازه);
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    درون_Bundle=(ثابت Resource *)((ثابت حرف *)درون_داده+سرآیند_اندازه);
    ریشه_Res=ds->readUInt32(*درون_Bundle);

    /* formatVersion 1.1 adds the indexes[] array */
    درون_Indexes=(ثابت int32_t *)(درون_Bundle+1);

    شاخص_درازی=udata_readInt32(ds, درون_Indexes[URES_شاخص_درازی])&0xff;
    إذا(شاخص_درازی<=URES_شاخص_MAX_جدول_درازی) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    کلیدها_Bottom=1+شاخص_درازی;
    کلیدها_بالاترین=udata_readInt32(ds, درون_Indexes[URES_شاخص_کلیدها_بالاترین]);
    إذا(شاخص_درازی>URES_شاخص_16_بیت_بالاترین) {
        resBottom=udata_readInt32(ds, درون_Indexes[URES_شاخص_16_بیت_بالاترین]);
    } وإلا {
        resBottom=کلیدها_بالاترین;
    }
    بالاترین=udata_readInt32(ds, درون_Indexes[URES_شاخص_BUNDLE_بالاترین]);
    max_جدول_درازی=udata_readInt32(ds, درون_Indexes[URES_شاخص_MAX_جدول_درازی]);

    إذا(0<=bundle_درازی && bundle_درازی<بالاترین) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         بالاترین, bundle_درازی);
        *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا(کلیدها_بالاترین>(1+شاخص_درازی)) {
        موقت_جدول.local_کلید_حد=کلیدها_بالاترین<<2;
    } وإلا {
        موقت_جدول.local_کلید_حد=0;
    }

    إذا(length>=0) {
        Resource *بیرون_Bundle=(Resource *)((حرف *)بیرون_داده+سرآیند_اندازه);

        /* track which resources we have already swapped */
        uint32_t پشته_Res_Flags[پشته_ردیف_ظرفیت];
        int32_t res_Flags_درازی;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_درازی=(length+31)>>5;          /* number of bytes needed */
        res_Flags_درازی=(res_Flags_درازی+3)&~3;   /* multiple of 4 bytes for uint32_t */
        إذا(res_Flags_درازی<=(int32_t)حجم(پشته_Res_Flags)) {
            موقت_جدول.resFlags=پشته_Res_Flags;
        } وإلا {
            موقت_جدول.resFlags=(uint32_t *)uprv_malloc(res_Flags_درازی);
            إذا(موقت_جدول.resFlags==اشاره_تهی) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *p_Error_کد=U_حافظه_تخصیص_ERROR;
                أرجع 0;
            }
        }
        uprv_memset(موقت_جدول.resFlags, 0, res_Flags_درازی);

        /* copy the bundle for binary and inaccessible data */
        إذا(درون_داده!=بیرون_داده) {
            uprv_memcpy(بیرون_Bundle, درون_Bundle, 4*بالاترین);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_جابجا_Inv_String_بلوک(ds, درون_Bundle+کلیدها_Bottom, 4*(کلیدها_بالاترین-کلیدها_Bottom),
                                    بیرون_Bundle+کلیدها_Bottom, p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(کلیدها_بالاترین-کلیدها_Bottom));
            إذا(موقت_جدول.resFlags!=پشته_Res_Flags) {
                uprv_free(موقت_جدول.resFlags);
            }
            أرجع 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        إذا(کلیدها_بالاترین<resBottom) {
            ds->جابجا_Array16(ds, درون_Bundle+کلیدها_بالاترین, (resBottom-کلیدها_بالاترین)*4, بیرون_Bundle+کلیدها_بالاترین, p_Error_کد);
            إذا(U_FAILURE(*p_Error_کد)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-کلیدها_بالاترین));
                إذا(موقت_جدول.resFlags!=پشته_Res_Flags) {
                    uprv_free(موقت_جدول.resFlags);
                }
                أرجع 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        موقت_جدول.کلید_Chars=(ثابت حرف *)بیرون_Bundle; /* sort by outCharset */
        إذا(موقت_جدول.major_Format_نسخه>1 || max_جدول_درازی<=پشته_ردیف_ظرفیت) {
            موقت_جدول.ردیفها=ردیفها;
            موقت_جدول.resort=resort;
        } وإلا {
            موقت_جدول.ردیفها=(ردیف_2 *)uprv_malloc(max_جدول_درازی*حجم(ردیف_2)+max_جدول_درازی*4);
            إذا(موقت_جدول.ردیفها==اشاره_تهی) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_جدول_درازی);
                *p_Error_کد=U_حافظه_تخصیص_ERROR;
                إذا(موقت_جدول.resFlags!=پشته_Res_Flags) {
                    uprv_free(موقت_جدول.resFlags);
                }
                أرجع 0;
            }
            موقت_جدول.resort=(int32_t *)(موقت_جدول.ردیفها+max_جدول_درازی);
        }

        /* swap the resources */
        ures_جابجا_Resource(ds, درون_Bundle, بیرون_Bundle, ریشه_Res, اشاره_تهی, &موقت_جدول, p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             ریشه_Res);
        }

        إذا(موقت_جدول.ردیفها!=ردیفها) {
            uprv_free(موقت_جدول.ردیفها);
        }
        إذا(موقت_جدول.resFlags!=پشته_Res_Flags) {
            uprv_free(موقت_جدول.resFlags);
        }

        /* swap the root resource and indexes */
        ds->جابجا_Array32(ds, درون_Bundle, کلیدها_Bottom*4, بیرون_Bundle, p_Error_کد);
    }

    أرجع سرآیند_اندازه+4*بالاترین;
}

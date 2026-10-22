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
#تعریف_کړه RES_GET_KEY16(p_Res_معلومات, کلی_دننه_واټن) \
    ((کلی_دننه_واټن)<(p_Res_معلومات)->local_کلی_حد ? \
        (ثابت حرف *)(p_Res_معلومات)->p_ریښه+(کلی_دننه_واټن) : \
        (p_Res_معلومات)->pool_Bundle_کلیګانې+(کلی_دننه_واټن)-(p_Res_معلومات)->local_کلی_حد)

#تعریف_کړه RES_GET_KEY32(p_Res_معلومات, کلی_دننه_واټن) \
    ((کلی_دننه_واټن)>=0 ? \
        (ثابت حرف *)(p_Res_معلومات)->p_ریښه+(کلی_دننه_واټن) : \
        (p_Res_معلومات)->pool_Bundle_کلیګانې+((کلی_دننه_واټن)&0x7fffffff))

#تعریف_کړه URESDATA_توکی_NOT_وموندل -1

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
} g_تشه_String={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ساكن int32_t
res_پیدا_کړه_جدول_توکی(ثابت Resource_معلومات *p_Res_معلومات, ثابت uint16_t *کلی_Offsets, int32_t length,
                   ثابت حرف *کلی, ثابت حرف **real_کلی) {
    ثابت حرف *جدول_کلی;
    int32_t mid, پیل, حد;
    صحيح پایله;

    /* do a binary search for the key */
    پیل=0;
    حد=length;
    طالما(پیل<حد) {
        mid = (پیل + حد) / 2;
        جدول_کلی = RES_GET_KEY16(p_Res_معلومات, کلی_Offsets[mid]);
        إذا (p_Res_معلومات->useNativeStrcmp) {
            پایله = uprv_strcmp(کلی, جدول_کلی);
        } وإلا {
            پایله = uprv_compareInvCharsAsAscii(کلی, جدول_کلی);
        }
        إذا (پایله < 0) {
            حد = mid;
        } وإلا إذا (پایله > 0) {
            پیل = mid + 1;
        } وإلا {
            /* We found it! */
            *real_کلی=جدول_کلی;
            أرجع mid;
        }
    }
    أرجع URESDATA_توکی_NOT_وموندل;  /* not found or table is empty. */
}

ساكن int32_t
res_پیدا_کړه_Table32_توکی(ثابت Resource_معلومات *p_Res_معلومات, ثابت int32_t *کلی_Offsets, int32_t length,
                     ثابت حرف *کلی, ثابت حرف **real_کلی) {
    ثابت حرف *جدول_کلی;
    int32_t mid, پیل, حد;
    صحيح پایله;

    /* do a binary search for the key */
    پیل=0;
    حد=length;
    طالما(پیل<حد) {
        mid = (پیل + حد) / 2;
        جدول_کلی = RES_GET_KEY32(p_Res_معلومات, کلی_Offsets[mid]);
        إذا (p_Res_معلومات->useNativeStrcmp) {
            پایله = uprv_strcmp(کلی, جدول_کلی);
        } وإلا {
            پایله = uprv_compareInvCharsAsAscii(کلی, جدول_کلی);
        }
        إذا (پایله < 0) {
            حد = mid;
        } وإلا إذا (پایله > 0) {
            پیل = mid + 1;
        } وإلا {
            /* We found it! */
            *real_کلی=جدول_کلی;
            أرجع mid;
        }
    }
    أرجع URESDATA_توکی_NOT_وموندل;  /* not found or table is empty. */
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
        pInfo->معلومات_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->معلومات_Format[1]==0x65 &&
        pInfo->معلومات_Format[2]==0x73 &&
        pInfo->معلومات_Format[3]==0x42 &&
        (1<=pInfo->format_نسخه[0] && pInfo->format_نسخه[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

ساكن فراغ
res_init(Resource_معلومات *p_Res_معلومات,
         UVersionInfo format_نسخه, ثابت فراغ *دننه_بایټونه, int32_t length,
         UError_کوډ *error_کوډ) {
    UResType ریښه_Type;

    /* get the root resource */
    p_Res_معلومات->p_ریښه = static_cast<ثابت int32_t*>(دننه_بایټونه);
    p_Res_معلومات->ریښه_Res = static_cast<Resource>(*p_Res_معلومات->p_ریښه);
    p_Res_معلومات->p16_بټ_Units=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    إذا(length>=0 && (length/4)<((format_نسخه[0]==1 && format_نسخه[1]==0) ? 1 : 1+5)) {
        *error_کوډ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_معلومات);
        أرجع;
    }

    /* currently, we accept only resources that have a Table as their roots */
    ریښه_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_معلومات->ریښه_Res));
    إذا(!URES_IS_جدول(ریښه_Type)) {
        *error_کوډ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_معلومات);
        أرجع;
    }

    إذا(format_نسخه[0]==1 && format_نسخه[1]==0) {
        p_Res_معلومات->local_کلی_حد=0x10000;  /* greater than any 16-bit key string offset */
    } وإلا {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ثابت int32_t *indexes=p_Res_معلومات->p_ریښه+1;
        int32_t شاخص_اوږدتیا=indexes[URES_شاخص_اوږدتیا]&0xff;
        إذا(شاخص_اوږدتیا<=URES_شاخص_MAX_جدول_اوږدتیا) {
            *error_کوډ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_معلومات);
            أرجع;
        }
        إذا( length>=0 &&
            (length<((1+شاخص_اوږدتیا)<<2) ||
             length<(indexes[URES_شاخص_BUNDLE_سر]<<2))
        ) {
            *error_کوډ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_معلومات);
            أرجع;
        }
        إذا(indexes[URES_شاخص_کلیګانې_سر]>(1+شاخص_اوږدتیا)) {
            p_Res_معلومات->local_کلی_حد=indexes[URES_شاخص_کلیګانې_سر]<<2;
        }
        إذا(format_نسخه[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_معلومات->pool_String_شاخص_حد = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_شاخص_اوږدتیا]) >> 8);
        }
        إذا(شاخص_اوږدتیا>URES_شاخص_ځانګړتیاوې) {
            int32_t att=indexes[URES_شاخص_ځانګړتیاوې];
            p_Res_معلومات->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_معلومات->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_معلومات->کاروي_Pool_Bundle = static_cast<UBool>((att & URES_ATT_کاروي_POOL_BUNDLE) != 0);
            p_Res_معلومات->pool_String_شاخص_حد|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_معلومات->pool_String_Index16_حد = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        إذا((p_Res_معلومات->isPoolBundle || p_Res_معلومات->کاروي_Pool_Bundle) && شاخص_اوږدتیا<=URES_شاخص_POOL_CHECKSUM) {
            *error_کوډ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_معلومات);
            أرجع;
        }
        إذا( شاخص_اوږدتیا>URES_شاخص_16_بټ_سر &&
            indexes[URES_شاخص_16_بټ_سر]>indexes[URES_شاخص_کلیګانې_سر]
        ) {
            p_Res_معلومات->p16_بټ_Units = reinterpret_cast<ثابت uint16_t*>(p_Res_معلومات->p_ریښه + indexes[URES_شاخص_کلیګانې_سر]);
        }
    }

    إذا(format_نسخه[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_معلومات->useNativeStrcmp=ریښتیا;
    }
}

U_CAPI فراغ U_EXPORT2
res_read(Resource_معلومات *p_Res_معلومات,
         ثابت UDataInfo *pInfo, ثابت فراغ *دننه_بایټونه, int32_t length,
         UError_کوډ *error_کوډ) {
    UVersionInfo format_نسخه;

    uprv_memset(p_Res_معلومات, 0, حجم(Resource_معلومات));
    إذا(U_FAILURE(*error_کوډ)) {
        أرجع;
    }
    إذا(!isAcceptable(format_نسخه, تش_اشاره, تش_اشاره, pInfo)) {
        *error_کوډ=U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    res_init(p_Res_معلومات, format_نسخه, دننه_بایټونه, length, error_کوډ);
}

U_CFUNC فراغ
res_بارول(Resource_معلومات *p_Res_معلومات,
         ثابت حرف *لار, ثابت حرف *name, UError_کوډ *error_کوډ) {
    UVersionInfo format_نسخه;

    uprv_memset(p_Res_معلومات, 0, حجم(Resource_معلومات));

    /* load the ResourceBundle file */
    p_Res_معلومات->معلومات=udata_openChoice(لار, "res", name, isAcceptable, format_نسخه, error_کوډ);
    إذا(U_FAILURE(*error_کوډ)) {
        أرجع;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_معلومات, format_نسخه, udata_get_حافظه(p_Res_معلومات->معلومات), -1, error_کوډ);
}

U_CFUNC فراغ
res_unload(Resource_معلومات *p_Res_معلومات) {
    إذا(p_Res_معلومات->معلومات!=تش_اشاره) {
        udata_close(p_Res_معلومات->معلومات);
        p_Res_معلومات->معلومات=تش_اشاره;
    }
}

ساكن ثابت int8_t g_عامه_Types[URES_حد] = {
    URES_STRING,
    URES_دوګونی,
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
    URES_INT_ویکټور,
    URES_NONE
};

U_CAPI UResType U_EXPORT2
res_get_عامه_Type(Resource res) {
    أرجع (UResType)g_عامه_Types[RES_GET_TYPE(res)];
}

U_CAPI ثابت char16_t * U_EXPORT2
res_getStringNoTrace(ثابت Resource_معلومات *p_Res_معلومات, Resource res, int32_t *p_اوږدتیا) {
    ثابت char16_t *p;
    uint32_t دننه_واټن=RES_GET_دننه_واټن(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        إذا((int32_t)دننه_واټن<p_Res_معلومات->pool_String_شاخص_حد) {
            p=(ثابت char16_t *)p_Res_معلومات->pool_Bundle_مزي_ګانې+دننه_واټن;
        } وإلا {
            p=(ثابت char16_t *)p_Res_معلومات->p16_بټ_Units+(دننه_واټن-p_Res_معلومات->pool_String_شاخص_حد);
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
    } وإلا إذا(res==دننه_واټن) /* RES_GET_TYPE(res)==URES_STRING */ {
        ثابت int32_t *p32= res==0 ? &g_تشه_String.length : p_Res_معلومات->p_ریښه+res;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=تش_اشاره;
        length=0;
    }
    إذا(p_اوږدتیا) {
        *p_اوږدتیا=length;
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
UBool isNoInheritanceMarker(ثابت Resource_معلومات *p_Res_معلومات, Resource res) {
    uint32_t دننه_واټن=RES_GET_دننه_واټن(res);
    إذا (دننه_واټن == 0) {
        // empty string
    } وإلا إذا (res == دننه_واټن) {
        ثابت int32_t *p32=p_Res_معلومات->p_ریښه+res;
        int32_t length=*p32;
        ثابت char16_t* p = reinterpret_cast<ثابت char16_t*>(p32);
        أرجع length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } وإلا إذا (RES_GET_TYPE(res) == URES_STRING_V2) {
        ثابت char16_t *p;
        إذا (static_cast<int32_t>(دننه_واټن) < p_Res_معلومات->pool_String_شاخص_حد) {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_معلومات->pool_Bundle_مزي_ګانې) + دننه_واټن;
        } وإلا {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_معلومات->p16_بټ_Units) + (دننه_واټن - p_Res_معلومات->pool_String_شاخص_حد);
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

int32_t getStringArray(ثابت Resource_معلومات *p_Res_معلومات, ثابت icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    إذا(dest == تش_اشاره ? capacity != 0 : capacity < 0) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    int32_t length = array.get_اندازه();
    إذا(length == 0) {
        أرجع 0;
    }
    إذا(length > capacity) {
        error_کوډ = U_بفر_ډکېدنه_ERROR;
        أرجع length;
    }
    لكل(int32_t i = 0; i < length; ++i) {
        int32_t s_اوږدتیا;
        // No tracing: handled by the caller
        ثابت char16_t *s = res_getStringNoTrace(p_Res_معلومات, array.دننی_Get_Resource(p_Res_معلومات, i), &s_اوږدتیا);
        إذا(s == تش_اشاره) {
            error_کوډ = U_RESOURCE_TYPE_MISMATCH;
            أرجع 0;
        }
        dest[i].سېټ_منزل(ریښتیا, s, s_اوږدتیا);
    }
    أرجع length;
}

}  // namespace

U_CAPI ثابت char16_t * U_EXPORT2
res_getAlias(ثابت Resource_معلومات *p_Res_معلومات, Resource res, int32_t *p_اوږدتیا) {
    ثابت char16_t *p;
    uint32_t دننه_واټن=RES_GET_دننه_واټن(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_ALIAS) {
        ثابت int32_t *p32= دننه_واټن==0 ? &g_تشه_String.length : p_Res_معلومات->p_ریښه+دننه_واټن;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=تش_اشاره;
        length=0;
    }
    إذا(p_اوږدتیا) {
        *p_اوږدتیا=length;
    }
    أرجع p;
}

U_CAPI ثابت uint8_t * U_EXPORT2
res_get_دوګونی_No_Trace(ثابت Resource_معلومات *p_Res_معلومات, Resource res, int32_t *p_اوږدتیا) {
    ثابت uint8_t *p;
    uint32_t دننه_واټن=RES_GET_دننه_واټن(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_دوګونی) {
        ثابت int32_t *p32= دننه_واټن==0 ? (ثابت int32_t*)&gEmpty32 : p_Res_معلومات->p_ریښه+دننه_واټن;
        length=*p32++;
        p=(ثابت uint8_t *)p32;
    } وإلا {
        p=تش_اشاره;
        length=0;
    }
    إذا(p_اوږدتیا) {
        *p_اوږدتیا=length;
    }
    أرجع p;
}


U_CAPI ثابت int32_t * U_EXPORT2
res_get_Int_ویکټور_No_Trace(ثابت Resource_معلومات *p_Res_معلومات, Resource res, int32_t *p_اوږدتیا) {
    ثابت int32_t *p;
    uint32_t دننه_واټن=RES_GET_دننه_واټن(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_INT_ویکټور) {
        p= دننه_واټن==0 ? (ثابت int32_t *)&gEmpty32 : p_Res_معلومات->p_ریښه+دننه_واټن;
        length=*p++;
    } وإلا {
        p=تش_اشاره;
        length=0;
    }
    إذا(p_اوږدتیا) {
        *p_اوږدتیا=length;
    }
    أرجع p;
}

U_CAPI int32_t U_EXPORT2
res_شمېر_Array_توکي(ثابت Resource_معلومات *p_Res_معلومات, Resource res) {
    uint32_t دننه_واټن=RES_GET_دننه_واټن(res);
    بدل(RES_GET_TYPE(res)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
    حالة URES_دوګونی:
    حالة URES_ALIAS:
    حالة URES_INT:
    حالة URES_INT_ویکټور:
        أرجع 1;
    حالة URES_ARRAY:
    حالة URES_TABLE32:
        أرجع دننه_واټن==0 ? 0 : *(p_Res_معلومات->p_ریښه+دننه_واټن);
    حالة URES_جدول:
        أرجع دننه_واټن==0 ? 0 : *((ثابت uint16_t *)(p_Res_معلومات->p_ریښه+دننه_واټن));
    حالة URES_ARRAY16:
    حالة URES_TABLE16:
        أرجع p_Res_معلومات->p16_بټ_Units[دننه_واټن];
    افتراضي:
        أرجع 0;
    }
}

U_NAMESPACE_BEGIN

Resource_معلومات_ارزښت::~Resource_معلومات_ارزښت() {}

UResType Resource_معلومات_ارزښت::getType() ثابت {
    أرجع res_get_عامه_Type(res);
}

ثابت char16_t *Resource_معلومات_ارزښت::getString(int32_t &length, UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    ثابت char16_t *s = res_getString(fTraceInfo, &get_معلومات(), res, &length);
    إذا(s == تش_اشاره) {
        error_کوډ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ثابت char16_t *Resource_معلومات_ارزښت::getAliasString(int32_t &length, UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    ثابت char16_t *s = res_getAlias(&get_معلومات(), res, &length);
    إذا(s == تش_اشاره) {
        error_کوډ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

int32_t Resource_معلومات_ارزښت::getInt(UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        error_کوډ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getInt(fTraceInfo, res);
}

uint32_t Resource_معلومات_ارزښت::getUInt(UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        error_کوډ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getUInt(fTraceInfo, res);
}

ثابت int32_t *Resource_معلومات_ارزښت::get_Int_ویکټور(int32_t &length, UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    ثابت int32_t *iv = res_get_Int_ویکټور(fTraceInfo, &get_معلومات(), res, &length);
    إذا(iv == تش_اشاره) {
        error_کوډ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع iv;
}

ثابت uint8_t *Resource_معلومات_ارزښت::get_دوګونی(int32_t &length, UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    ثابت uint8_t *b = res_get_دوګونی(fTraceInfo, &get_معلومات(), res, &length);
    إذا(b == تش_اشاره) {
        error_کوډ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع b;
}

ResourceArray Resource_معلومات_ارزښت::getArray(UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع {};
    }
    ثابت uint16_t *items16 = تش_اشاره;
    ثابت Resource *items32 = تش_اشاره;
    uint32_t دننه_واټن=RES_GET_دننه_واټن(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_ARRAY:
        إذا (دننه_واټن!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ثابت Resource*>(get_معلومات().p_ریښه) + دننه_واټن;
            length = *items32++;
        }
        اكسر;
    حالة URES_ARRAY16:
        items16 = get_معلومات().p16_بټ_Units+دننه_واټن;
        length = *items16++;
        اكسر;
    افتراضي:
        error_کوډ = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع ResourceArray(items16, items32, length, fTraceInfo);
}

Resource_جدول Resource_معلومات_ارزښت::get_جدول(UError_کوډ &error_کوډ) ثابت {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع {};
    }
    ثابت uint16_t *keys16 = تش_اشاره;
    ثابت int32_t *keys32 = تش_اشاره;
    ثابت uint16_t *items16 = تش_اشاره;
    ثابت Resource *items32 = تش_اشاره;
    uint32_t دننه_واټن = RES_GET_دننه_واټن(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_جدول:
        إذا (دننه_واټن != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ثابت uint16_t*>(get_معلومات().p_ریښه + دننه_واټن);
            length = *keys16++;
            items32 = reinterpret_cast<ثابت Resource*>(keys16 + length + (~length & 1));
        }
        اكسر;
    حالة URES_TABLE16:
        keys16 = get_معلومات().p16_بټ_Units+دننه_واټن;
        length = *keys16++;
        items16 = keys16 + length;
        اكسر;
    حالة URES_TABLE32:
        إذا (دننه_واټن != 0) {  // empty if offset==0
            keys32 = get_معلومات().p_ریښه+دننه_واټن;
            length = *keys32++;
            items32 = reinterpret_cast<ثابت Resource*>(keys32) + length;
        }
        اكسر;
    افتراضي:
        error_کوډ = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع Resource_جدول(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_معلومات_ارزښت::isNoInheritanceMarker() ثابت {
    أرجع ::isNoInheritanceMarker(&get_معلومات(), res);
}

int32_t Resource_معلومات_ارزښت::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UError_کوډ &error_کوډ) ثابت {
    أرجع ::getStringArray(&get_معلومات(), getArray(error_کوډ), dest, capacity, error_کوډ);
}

int32_t Resource_معلومات_ارزښت::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UError_کوډ &error_کوډ) ثابت {
    إذا(URES_IS_ARRAY(res)) {
        أرجع ::getStringArray(&get_معلومات(), getArray(error_کوډ), dest, capacity, error_کوډ);
    }
    إذا(U_FAILURE(error_کوډ)) {
        أرجع 0;
    }
    إذا(dest == تش_اشاره ? capacity != 0 : capacity < 0) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    إذا(capacity < 1) {
        error_کوډ = U_بفر_ډکېدنه_ERROR;
        أرجع 1;
    }
    int32_t s_اوږدتیا;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_معلومات(), res, &s_اوږدتیا);
    إذا(s != تش_اشاره) {
        dest[0].سېټ_منزل(ریښتیا, s, s_اوږدتیا);
        أرجع 1;
    }
    error_کوډ = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0;
}

UnicodeString Resource_معلومات_ارزښت::get_String_Or_لومړی_د_Array(UError_کوډ &error_کوډ) ثابت {
    UnicodeString us;
    إذا(U_FAILURE(error_کوډ)) {
        أرجع us;
    }
    int32_t s_اوږدتیا;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_معلومات(), res, &s_اوږدتیا);
    إذا(s != تش_اشاره) {
        us.سېټ_منزل(ریښتیا, s, s_اوږدتیا);
        أرجع us;
    }
    ResourceArray array = getArray(error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع us;
    }
    إذا(array.get_اندازه() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_معلومات(), array.دننی_Get_Resource(&get_معلومات(), 0), &s_اوږدتیا);
        إذا(s != تش_اشاره) {
            us.سېټ_منزل(ریښتیا, s, s_اوږدتیا);
            أرجع us;
        }
    }
    error_کوډ = U_RESOURCE_TYPE_MISMATCH;
    أرجع us;
}

U_NAMESPACE_پای_ټکی

ساكن Resource
makeResourceFrom16(ثابت Resource_معلومات *p_Res_معلومات, int32_t res16) {
    إذا(res16<p_Res_معلومات->pool_String_Index16_حد) {
        // Pool string, nothing to do.
    } وإلا {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_معلومات->pool_String_Index16_حد+p_Res_معلومات->pool_String_شاخص_حد;
    }
    أرجع URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_جدول_توکی_By_کلی(ثابت Resource_معلومات *p_Res_معلومات, Resource جدول,
                      int32_t *شاخص_R, ثابت حرف **کلی) {
    uint32_t دننه_واټن=RES_GET_دننه_واټن(جدول);
    int32_t length;
    int32_t idx;
    إذا(کلی == تش_اشاره || *کلی == تش_اشاره) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(جدول)) {
    حالة URES_جدول: {
        إذا (دننه_واټن!=0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_معلومات->p_ریښه+دننه_واټن);
            length=*p++;
            *شاخص_R=idx=res_پیدا_کړه_جدول_توکی(p_Res_معلومات, p, length, *کلی, کلی);
            إذا(idx>=0) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                أرجع p32[idx];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=p_Res_معلومات->p16_بټ_Units+دننه_واټن;
        length=*p++;
        *شاخص_R=idx=res_پیدا_کړه_جدول_توکی(p_Res_معلومات, p, length, *کلی, کلی);
        إذا(idx>=0) {
            أرجع makeResourceFrom16(p_Res_معلومات, p[length+idx]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (دننه_واټن!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_معلومات->p_ریښه+دننه_واټن;
            length=*p++;
            *شاخص_R=idx=res_پیدا_کړه_Table32_توکی(p_Res_معلومات, p, length, *کلی, کلی);
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
res_get_جدول_توکی_By_شاخص(ثابت Resource_معلومات *p_Res_معلومات, Resource جدول,
                        int32_t شاخص_R, ثابت حرف **کلی) {
    uint32_t دننه_واټن=RES_GET_دننه_واټن(جدول);
    int32_t length;
    إذا (شاخص_R < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(جدول)) {
    حالة URES_جدول: {
        إذا (دننه_واټن != 0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_معلومات->p_ریښه+دننه_واټن);
            length=*p++;
            إذا(شاخص_R<length) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                إذا(کلی!=تش_اشاره) {
                    *کلی=RES_GET_KEY16(p_Res_معلومات, p[شاخص_R]);
                }
                أرجع p32[شاخص_R];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=p_Res_معلومات->p16_بټ_Units+دننه_واټن;
        length=*p++;
        إذا(شاخص_R<length) {
            إذا(کلی!=تش_اشاره) {
                *کلی=RES_GET_KEY16(p_Res_معلومات, p[شاخص_R]);
            }
            أرجع makeResourceFrom16(p_Res_معلومات, p[length+شاخص_R]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (دننه_واټن != 0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_معلومات->p_ریښه+دننه_واټن;
            length=*p++;
            إذا(شاخص_R<length) {
                إذا(کلی!=تش_اشاره) {
                    *کلی=RES_GET_KEY32(p_Res_معلومات, p[شاخص_R]);
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
res_getResource(ثابت Resource_معلومات *p_Res_معلومات, ثابت حرف *کلی) {
    ثابت حرف *real_کلی=کلی;
    int32_t idx;
    أرجع res_get_جدول_توکی_By_کلی(p_Res_معلومات, p_Res_معلومات->ریښه_Res, &idx, &real_کلی);
}


UBool icu::Resource_جدول::get_کلی_And_ارزښت(int32_t i,
                                         ثابت حرف *&کلی, icu::Resource_ارزښت &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_معلومات_ارزښت &rd_ارزښت = static_cast<icu::Resource_معلومات_ارزښت &>(value);
        إذا (keys16 != تش_اشاره) {
            کلی = RES_GET_KEY16(&rd_ارزښت.get_معلومات(), keys16[i]);
        } وإلا {
            کلی = RES_GET_KEY32(&rd_ارزښت.get_معلومات(), keys32[i]);
        }
        Resource res;
        إذا (items16 != تش_اشاره) {
            res = makeResourceFrom16(&rd_ارزښت.get_معلومات(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ارزښت.سېټ_Resource(res, ResourceTracer(fTraceInfo, کلی));
        أرجع ریښتیا;
    }
    أرجع خطأ;
}

UBool icu::Resource_جدول::پیدا_کړه_ارزښت(ثابت حرف *کلی, Resource_ارزښت &value) ثابت {
    icu::Resource_معلومات_ارزښت &rd_ارزښت = static_cast<icu::Resource_معلومات_ارزښت &>(value);
    ثابت حرف *real_کلی = تش_اشاره;
    int32_t i;
    إذا (keys16 != تش_اشاره) {
        i = res_پیدا_کړه_جدول_توکی(&rd_ارزښت.get_معلومات(), keys16, length, کلی, &real_کلی);
    } وإلا {
        i = res_پیدا_کړه_Table32_توکی(&rd_ارزښت.get_معلومات(), keys32, length, کلی, &real_کلی);
    }
    إذا (i >= 0) {
        Resource res;
        إذا (items16 != تش_اشاره) {
            res = makeResourceFrom16(&rd_ارزښت.get_معلومات(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_ارزښت.سېټ_Resource(res, ResourceTracer(fTraceInfo, کلی));
        أرجع ریښتیا;
    }
    أرجع خطأ;
}

U_CAPI Resource U_EXPORT2
res_get_Array_توکی(ثابت Resource_معلومات *p_Res_معلومات, Resource array, int32_t شاخص_R) {
    uint32_t دننه_واټن=RES_GET_دننه_واټن(array);
    إذا (شاخص_R < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(array)) {
    حالة URES_ARRAY: {
        إذا (دننه_واټن!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_معلومات->p_ریښه+دننه_واټن;
            إذا(شاخص_R<*p) {
                أرجع (Resource)p[1+شاخص_R];
            }
        }
        اكسر;
    }
    حالة URES_ARRAY16: {
        ثابت uint16_t *p=p_Res_معلومات->p16_بټ_Units+دننه_واټن;
        إذا(شاخص_R<*p) {
            أرجع makeResourceFrom16(p_Res_معلومات, p[1+شاخص_R]);
        }
        اكسر;
    }
    افتراضي:
        اكسر;
    }
    أرجع RES_BOGUS;
}

uint32_t icu::ResourceArray::دننی_Get_Resource(ثابت Resource_معلومات *p_Res_معلومات, int32_t i) ثابت {
    إذا (items16 != تش_اشاره) {
        أرجع makeResourceFrom16(p_Res_معلومات, items16[i]);
    } وإلا {
        أرجع items32[i];
    }
}

UBool icu::ResourceArray::get_ارزښت(int32_t i, icu::Resource_ارزښت &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_معلومات_ارزښت &rd_ارزښت = static_cast<icu::Resource_معلومات_ارزښت &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ارزښت.سېټ_Resource(
            دننی_Get_Resource(&rd_ارزښت.get_معلومات(), i),
            ResourceTracer(fTraceInfo, i));
        أرجع ریښتیا;
    }
    أرجع خطأ;
}

U_CFUNC Resource
res_پیدا_کړه_Resource(ثابت Resource_معلومات *p_Res_معلومات, Resource r, حرف** لار, ثابت حرف** کلی) {
  حرف *لار_P = *لار, *بل_Sep_P = *لار;
  حرف *close_شاخص = تش_اشاره;
  Resource t1 = r;
  Resource t2;
  int32_t شاخص_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  إذا(!uprv_strlen(لار_P)) {
      أرجع r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  إذا(!URES_IS_CONTAINER(type)) {
      أرجع RES_BOGUS;
  }
  
  طالما(بل_Sep_P && *لار_P && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    بل_Sep_P = uprv_strchr(لار_P, RES_لار_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    إذا(بل_Sep_P != تش_اشاره) {
      إذا(بل_Sep_P == لار_P) {
        // Empty key string.
        أرجع RES_BOGUS;
      }
      *بل_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *لار = بل_Sep_P+1;
    } وإلا {
      *لار = uprv_strchr(لار_P, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    إذا(URES_IS_جدول(type)) {
      *کلی = لار_P;
      t2 = res_get_جدول_توکی_By_کلی(p_Res_معلومات, t1, &شاخص_R, کلی);
    } وإلا إذا(URES_IS_ARRAY(type)) {
      شاخص_R = uprv_strtol(لار_P, &close_شاخص, 10);
      إذا(شاخص_R >= 0 && *close_شاخص == 0) {
        t2 = res_get_Array_توکی(p_Res_معلومات, t1, شاخص_R);
      } وإلا {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *کلی = تش_اشاره;
    } وإلا { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    لار_P = *لار;
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
تعريفنوع بنية لیکه_2 {
    int32_t کلی_شاخص, ترتیبول_شاخص;
} لیکه_2;

ساكن int32_t U_CALLCONV
ures_compare_لیکې(ثابت فراغ *context, ثابت فراغ *کیڼ, ثابت فراغ *ښی) {
    ثابت حرف* کلی_Chars = static_cast<ثابت حرف*>(context);
    أرجع static_cast<int32_t>(uprv_strcmp(کلی_Chars + static_cast<ثابت لیکه_2*>(کیڼ)->کلی_شاخص,
                                            کلی_Chars + static_cast<ثابت لیکه_2*>(ښی)->کلی_شاخص));
}

تعريفنوع بنية لنډمهاله_جدول_2 {
    ثابت حرف *کلی_Chars;
    لیکه_2 *لیکې;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_کلی_حد;
    uint8_t major_Format_نسخه;
} لنډمهاله_جدول_2;

تعداد {
    ډېری_لیکه_ظرفیت=200
};

/* The table item key string is not locally available. */
ساكن ثابت حرف *ثابت g_Unknown_کلی="";

#إذا !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ساكن ثابت char16_t g_Collation_Bin_کلی[]=u"%%CollationBin";
#د_که_پای

/*
 * swap one resource item
 */
ساكن فراغ
ures_بدلون_Resource(ثابت UDataSwapper *ds,
                  ثابت Resource *دننه_Bundle, Resource *بهر_Bundle,
                  Resource res, /* caller swaps res itself */
                  ثابت حرف *کلی,
                  لنډمهاله_جدول_2 *p_لنډمهاله_جدول,
                  UError_کوډ *p_Error_کوډ) {
    ثابت Resource *p;
    Resource *q;
    int32_t دننه_واټن, count;

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
    دننه_واټن = static_cast<int32_t>(RES_GET_دننه_واټن(res));
    إذا(دننه_واټن==0) {
        /* special offset indicating an empty item */
        أرجع;
    }
    إذا (p_لنډمهاله_جدول->resFlags[دننه_واټن >> 5] & (static_cast<uint32_t>(1) << (دننه_واټن & 0x1f))) {
        /* we already swapped this resource item */
        أرجع;
    } وإلا {
        /* mark it as swapped now */
        p_لنډمهاله_جدول->resFlags[دننه_واټن >> 5] |= static_cast<uint32_t>(1) << (دننه_واټن & 0x1f);
    }

    p=دننه_Bundle+دننه_واټن;
    q=بهر_Bundle+دننه_واټن;

    بدل(RES_GET_TYPE(res)) {
    حالة URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    حالة URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->بدلون_Array32(ds, p, 4, q, p_Error_کوډ);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->بدلون_Array16(ds, p+1, 2*count, q+1, p_Error_کوډ);
        اكسر;
    حالة URES_دوګونی:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->بدلون_Array32(ds, p, 4, q, p_Error_کوډ);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#إذا !UCONFIG_NO_COLLATION
        إذا( کلی!=تش_اشاره &&  /* the binary is in a table */
            (کلی!=g_Unknown_کلی ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, کلی, -1,
                                       g_Collation_Bin_کلی, UPRV_LENGTHOF(g_Collation_Bin_کلی)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_دوګونی(ds, p+1, count))
        ) {
            ucol_بدلون(ds, p+1, count, q+1, p_Error_کوډ);
        }
#د_که_پای
        اكسر;
    حالة URES_جدول:
    حالة URES_TABLE32:
        {
            ثابت uint16_t *pKey16;
            uint16_t *qKey16;

            ثابت int32_t *pKey32;
            int32_t *qKey32;

            Resource توکی;
            int32_t i, old_شاخص;

            إذا(RES_GET_TYPE(res)==URES_جدول) {
                /* get table item count */
                pKey16 = reinterpret_cast<ثابت uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=تش_اشاره;

                /* swap count */
                ds->بدلون_Array16(ds, pKey16++, 2, qKey16++, p_Error_کوډ);

                دننه_واټن+=((1+count)+1)/2;
            } وإلا {
                /* get table item count */
                pKey32 = reinterpret_cast<ثابت int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=تش_اشاره;

                /* swap count */
                ds->بدلون_Array32(ds, pKey32++, 4, qKey32++, p_Error_کوډ);

                دننه_واټن+=1+count;
            }

            إذا(count==0) {
                اكسر;
            }

            p=دننه_Bundle+دننه_واټن; /* pointer to table resources */
            q=بهر_Bundle+دننه_واټن;

            /* recurse */
            لكل(i=0; i<count; ++i) {
                ثابت حرف *توکی_کلی=g_Unknown_کلی;
                إذا(pKey16!=تش_اشاره) {
                    int32_t کلی_دننه_واټن=ds->readUInt16(pKey16[i]);
                    إذا(کلی_دننه_واټن<p_لنډمهاله_جدول->local_کلی_حد) {
                        توکی_کلی = reinterpret_cast<ثابت حرف*>(بهر_Bundle) + کلی_دننه_واټن;
                    }
                } وإلا {
                    int32_t کلی_دننه_واټن=udata_readInt32(ds, pKey32[i]);
                    إذا(کلی_دننه_واټن>=0) {
                        توکی_کلی = reinterpret_cast<ثابت حرف*>(بهر_Bundle) + کلی_دننه_واټن;
                    }
                }
                توکی=ds->readUInt32(p[i]);
                ures_بدلون_Resource(ds, دننه_Bundle, بهر_Bundle, توکی, توکی_کلی, p_لنډمهاله_جدول, p_Error_کوډ);
                إذا(U_FAILURE(*p_Error_کوډ)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, توکی);
                    أرجع;
                }
            }

            إذا(p_لنډمهاله_جدول->major_Format_نسخه>1 || ds->دننه_Charset==ds->بهر_Charset) {
                /* no need to sort, just swap the offset/value arrays */
                إذا(pKey16!=تش_اشاره) {
                    ds->بدلون_Array16(ds, pKey16, count*2, qKey16, p_Error_کوډ);
                    ds->بدلون_Array32(ds, p, count*4, q, p_Error_کوډ);
                } وإلا {
                    /* swap key offsets and items as one array */
                    ds->بدلون_Array32(ds, pKey32, count*2*4, qKey32, p_Error_کوډ);
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
            إذا(pKey16!=تش_اشاره) {
                لكل(i=0; i<count; ++i) {
                    p_لنډمهاله_جدول->لیکې[i].کلی_شاخص=ds->readUInt16(pKey16[i]);
                    p_لنډمهاله_جدول->لیکې[i].ترتیبول_شاخص=i;
                }
            } وإلا {
                لكل(i=0; i<count; ++i) {
                    p_لنډمهاله_جدول->لیکې[i].کلی_شاخص=udata_readInt32(ds, pKey32[i]);
                    p_لنډمهاله_جدول->لیکې[i].ترتیبول_شاخص=i;
                }
            }
            uprv_ترتیبول_Array(p_لنډمهاله_جدول->لیکې, count, حجم(لیکه_2),
                           ures_compare_لیکې, p_لنډمهاله_جدول->کلی_Chars,
                           خطأ, p_Error_کوډ);
            إذا(U_FAILURE(*p_Error_کوډ)) {
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
            /*کلیګانې*/
            إذا(pKey16!=تش_اشاره) {
                uint16_t *rKey16;

                إذا(pKey16!=qKey16) {
                    rKey16=qKey16;
                } وإلا {
                    rKey16 = reinterpret_cast<uint16_t*>(p_لنډمهاله_جدول->resort);
                }
                لكل(i=0; i<count; ++i) {
                    old_شاخص=p_لنډمهاله_جدول->لیکې[i].ترتیبول_شاخص;
                    ds->بدلون_Array16(ds, pKey16+old_شاخص, 2, rKey16+i, p_Error_کوډ);
                }
                إذا(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } وإلا {
                int32_t *rKey32;

                إذا(pKey32!=qKey32) {
                    rKey32=qKey32;
                } وإلا {
                    rKey32=p_لنډمهاله_جدول->resort;
                }
                لكل(i=0; i<count; ++i) {
                    old_شاخص=p_لنډمهاله_جدول->لیکې[i].ترتیبول_شاخص;
                    ds->بدلون_Array32(ds, pKey32+old_شاخص, 4, rKey32+i, p_Error_کوډ);
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
                    r = reinterpret_cast<Resource*>(p_لنډمهاله_جدول->resort);
                }
                لكل(i=0; i<count; ++i) {
                    old_شاخص=p_لنډمهاله_جدول->لیکې[i].ترتیبول_شاخص;
                    ds->بدلون_Array32(ds, p+old_شاخص, 4, r+i, p_Error_کوډ);
                }
                إذا(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        اكسر;
    حالة URES_ARRAY:
        {
            Resource توکی;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->بدلون_Array32(ds, p++, 4, q++, p_Error_کوډ);

            /* recurse */
            لكل(i=0; i<count; ++i) {
                توکی=ds->readUInt32(p[i]);
                ures_بدلون_Resource(ds, دننه_Bundle, بهر_Bundle, توکی, تش_اشاره, p_لنډمهاله_جدول, p_Error_کوډ);
                إذا(U_FAILURE(*p_Error_کوډ)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, توکی);
                    أرجع;
                }
            }

            /* swap items */
            ds->بدلون_Array32(ds, p, 4*count, q, p_Error_کوډ);
        }
        اكسر;
    حالة URES_INT_ویکټور:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->بدلون_Array32(ds, p, 4*(1+count), q, p_Error_کوډ);
        اكسر;
    افتراضي:
        /* also catches RES_BOGUS */
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        اكسر;
    }
}

U_CAPI int32_t U_EXPORT2
ures_بدلون(ثابت UDataSwapper *ds,
          ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
          UError_کوډ *p_Error_کوډ) {
    ثابت UDataInfo *pInfo;
    ثابت Resource *دننه_Bundle;
    Resource ریښه_Res;
    int32_t سرلیک_اندازه, max_جدول_اوږدتیا;

    لیکه_2 لیکې[ډېری_لیکه_ظرفیت];
    int32_t resort[ډېری_لیکه_ظرفیت];
    لنډمهاله_جدول_2 لنډمهاله_جدول;

    ثابت int32_t *دننه_Indexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_اوږدتیا, شاخص_اوږدتیا, کلیګانې_Bottom, کلیګانې_سر, resBottom, سر_2;

    /* udata_swapDataHeader checks the arguments */
    سرلیک_اندازه=udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->معلومات_Format[1]==0x65 &&
        pInfo->معلومات_Format[2]==0x73 &&
        pInfo->معلومات_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->format_نسخه[0]==1 && pInfo->format_نسخه[1]>=1) ||
            pInfo->format_نسخه[0]==2 || pInfo->format_نسخه[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخه[0], pInfo->format_نسخه[1]);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }
    لنډمهاله_جدول.major_Format_نسخه=pInfo->format_نسخه[0];

    /* a resource bundle must contain at least one resource item */
    إذا(length<0) {
        bundle_اوږدتیا=-1;
    } وإلا {
        bundle_اوږدتیا=(length-سرلیک_اندازه)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        إذا(bundle_اوږدتیا<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-سرلیک_اندازه);
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    دننه_Bundle=(ثابت Resource *)((ثابت حرف *)دننه_معلومات+سرلیک_اندازه);
    ریښه_Res=ds->readUInt32(*دننه_Bundle);

    /* formatVersion 1.1 adds the indexes[] array */
    دننه_Indexes=(ثابت int32_t *)(دننه_Bundle+1);

    شاخص_اوږدتیا=udata_readInt32(ds, دننه_Indexes[URES_شاخص_اوږدتیا])&0xff;
    إذا(شاخص_اوږدتیا<=URES_شاخص_MAX_جدول_اوږدتیا) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    کلیګانې_Bottom=1+شاخص_اوږدتیا;
    کلیګانې_سر=udata_readInt32(ds, دننه_Indexes[URES_شاخص_کلیګانې_سر]);
    إذا(شاخص_اوږدتیا>URES_شاخص_16_بټ_سر) {
        resBottom=udata_readInt32(ds, دننه_Indexes[URES_شاخص_16_بټ_سر]);
    } وإلا {
        resBottom=کلیګانې_سر;
    }
    سر_2=udata_readInt32(ds, دننه_Indexes[URES_شاخص_BUNDLE_سر]);
    max_جدول_اوږدتیا=udata_readInt32(ds, دننه_Indexes[URES_شاخص_MAX_جدول_اوږدتیا]);

    إذا(0<=bundle_اوږدتیا && bundle_اوږدتیا<سر_2) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         سر_2, bundle_اوږدتیا);
        *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا(کلیګانې_سر>(1+شاخص_اوږدتیا)) {
        لنډمهاله_جدول.local_کلی_حد=کلیګانې_سر<<2;
    } وإلا {
        لنډمهاله_جدول.local_کلی_حد=0;
    }

    إذا(length>=0) {
        Resource *بهر_Bundle=(Resource *)((حرف *)بهر_معلومات+سرلیک_اندازه);

        /* track which resources we have already swapped */
        uint32_t ډېری_Res_Flags[ډېری_لیکه_ظرفیت];
        int32_t res_Flags_اوږدتیا;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_اوږدتیا=(length+31)>>5;          /* number of bytes needed */
        res_Flags_اوږدتیا=(res_Flags_اوږدتیا+3)&~3;   /* multiple of 4 bytes for uint32_t */
        إذا(res_Flags_اوږدتیا<=(int32_t)حجم(ډېری_Res_Flags)) {
            لنډمهاله_جدول.resFlags=ډېری_Res_Flags;
        } وإلا {
            لنډمهاله_جدول.resFlags=(uint32_t *)uprv_malloc(res_Flags_اوږدتیا);
            إذا(لنډمهاله_جدول.resFlags==تش_اشاره) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *p_Error_کوډ=U_حافظه_ویش_ERROR;
                أرجع 0;
            }
        }
        uprv_memset(لنډمهاله_جدول.resFlags, 0, res_Flags_اوږدتیا);

        /* copy the bundle for binary and inaccessible data */
        إذا(دننه_معلومات!=بهر_معلومات) {
            uprv_memcpy(بهر_Bundle, دننه_Bundle, 4*سر_2);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_بدلون_Inv_String_بلاک(ds, دننه_Bundle+کلیګانې_Bottom, 4*(کلیګانې_سر-کلیګانې_Bottom),
                                    بهر_Bundle+کلیګانې_Bottom, p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(کلیګانې_سر-کلیګانې_Bottom));
            إذا(لنډمهاله_جدول.resFlags!=ډېری_Res_Flags) {
                uprv_free(لنډمهاله_جدول.resFlags);
            }
            أرجع 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        إذا(کلیګانې_سر<resBottom) {
            ds->بدلون_Array16(ds, دننه_Bundle+کلیګانې_سر, (resBottom-کلیګانې_سر)*4, بهر_Bundle+کلیګانې_سر, p_Error_کوډ);
            إذا(U_FAILURE(*p_Error_کوډ)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-کلیګانې_سر));
                إذا(لنډمهاله_جدول.resFlags!=ډېری_Res_Flags) {
                    uprv_free(لنډمهاله_جدول.resFlags);
                }
                أرجع 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        لنډمهاله_جدول.کلی_Chars=(ثابت حرف *)بهر_Bundle; /* sort by outCharset */
        إذا(لنډمهاله_جدول.major_Format_نسخه>1 || max_جدول_اوږدتیا<=ډېری_لیکه_ظرفیت) {
            لنډمهاله_جدول.لیکې=لیکې;
            لنډمهاله_جدول.resort=resort;
        } وإلا {
            لنډمهاله_جدول.لیکې=(لیکه_2 *)uprv_malloc(max_جدول_اوږدتیا*حجم(لیکه_2)+max_جدول_اوږدتیا*4);
            إذا(لنډمهاله_جدول.لیکې==تش_اشاره) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_جدول_اوږدتیا);
                *p_Error_کوډ=U_حافظه_ویش_ERROR;
                إذا(لنډمهاله_جدول.resFlags!=ډېری_Res_Flags) {
                    uprv_free(لنډمهاله_جدول.resFlags);
                }
                أرجع 0;
            }
            لنډمهاله_جدول.resort=(int32_t *)(لنډمهاله_جدول.لیکې+max_جدول_اوږدتیا);
        }

        /* swap the resources */
        ures_بدلون_Resource(ds, دننه_Bundle, بهر_Bundle, ریښه_Res, تش_اشاره, &لنډمهاله_جدول, p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             ریښه_Res);
        }

        إذا(لنډمهاله_جدول.لیکې!=لیکې) {
            uprv_free(لنډمهاله_جدول.لیکې);
        }
        إذا(لنډمهاله_جدول.resFlags!=ډېری_Res_Flags) {
            uprv_free(لنډمهاله_جدول.resFlags);
        }

        /* swap the root resource and indexes */
        ds->بدلون_Array32(ds, دننه_Bundle, کلیګانې_Bottom*4, بهر_Bundle, p_Error_کوډ);
    }

    أرجع سرلیک_اندازه+4*سر_2;
}

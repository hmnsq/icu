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
#تعریف_کرو RES_GET_KEY16(p_Res_انکڑے, کلید_وتھ) \
    ((کلید_وتھ)<(p_Res_انکڑے)->local_کلید_حد_بندی ? \
        (ثابت حرف *)(p_Res_انکڑے)->p_جڑ+(کلید_وتھ) : \
        (p_Res_انکڑے)->pool_Bundle_کلیدیں+(کلید_وتھ)-(p_Res_انکڑے)->local_کلید_حد_بندی)

#تعریف_کرو RES_GET_KEY32(p_Res_انکڑے, کلید_وتھ) \
    ((کلید_وتھ)>=0 ? \
        (ثابت حرف *)(p_Res_انکڑے)->p_جڑ+(کلید_وتھ) : \
        (p_Res_انکڑے)->pool_Bundle_کلیدیں+((کلید_وتھ)&0x7fffffff))

#تعریف_کرو URESDATA_شے_NOT_لبھیا -1

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
} g_خالی_تھاواں_String={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ساكن int32_t
res_لبھو_جدول_شے(ثابت Resource_انکڑے *p_Res_انکڑے, ثابت uint16_t *کلید_Offsets, int32_t length,
                   ثابت حرف *کلید, ثابت حرف **real_کلید) {
    ثابت حرف *جدول_کلید;
    int32_t mid, شروع, حد_بندی;
    صحيح نتیجہ;

    /* do a binary search for the key */
    شروع=0;
    حد_بندی=length;
    طالما(شروع<حد_بندی) {
        mid = (شروع + حد_بندی) / 2;
        جدول_کلید = RES_GET_KEY16(p_Res_انکڑے, کلید_Offsets[mid]);
        إذا (p_Res_انکڑے->useNativeStrcmp) {
            نتیجہ = uprv_strcmp(کلید, جدول_کلید);
        } وإلا {
            نتیجہ = uprv_compareInvCharsAsAscii(کلید, جدول_کلید);
        }
        إذا (نتیجہ < 0) {
            حد_بندی = mid;
        } وإلا إذا (نتیجہ > 0) {
            شروع = mid + 1;
        } وإلا {
            /* We found it! */
            *real_کلید=جدول_کلید;
            أرجع mid;
        }
    }
    أرجع URESDATA_شے_NOT_لبھیا;  /* not found or table is empty. */
}

ساكن int32_t
res_لبھو_Table32_شے(ثابت Resource_انکڑے *p_Res_انکڑے, ثابت int32_t *کلید_Offsets, int32_t length,
                     ثابت حرف *کلید, ثابت حرف **real_کلید) {
    ثابت حرف *جدول_کلید;
    int32_t mid, شروع, حد_بندی;
    صحيح نتیجہ;

    /* do a binary search for the key */
    شروع=0;
    حد_بندی=length;
    طالما(شروع<حد_بندی) {
        mid = (شروع + حد_بندی) / 2;
        جدول_کلید = RES_GET_KEY32(p_Res_انکڑے, کلید_Offsets[mid]);
        إذا (p_Res_انکڑے->useNativeStrcmp) {
            نتیجہ = uprv_strcmp(کلید, جدول_کلید);
        } وإلا {
            نتیجہ = uprv_compareInvCharsAsAscii(کلید, جدول_کلید);
        }
        إذا (نتیجہ < 0) {
            حد_بندی = mid;
        } وإلا إذا (نتیجہ > 0) {
            شروع = mid + 1;
        } وإلا {
            /* We found it! */
            *real_کلید=جدول_کلید;
            أرجع mid;
        }
    }
    أرجع URESDATA_شے_NOT_لبھیا;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ساكن UBool U_CALLCONV
isAcceptable(فراغ *context,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->format_نسخہ, 4);
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->انکڑے_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->انکڑے_Format[1]==0x65 &&
        pInfo->انکڑے_Format[2]==0x73 &&
        pInfo->انکڑے_Format[3]==0x42 &&
        (1<=pInfo->format_نسخہ[0] && pInfo->format_نسخہ[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

ساكن فراغ
res_init(Resource_انکڑے *p_Res_انکڑے,
         UVersionInfo format_نسخہ, ثابت فراغ *وچ_بائٹاں, int32_t length,
         UError_کوڈ *error_کوڈ) {
    UResType جڑ_Type;

    /* get the root resource */
    p_Res_انکڑے->p_جڑ = static_cast<ثابت int32_t*>(وچ_بائٹاں);
    p_Res_انکڑے->جڑ_Res = static_cast<Resource>(*p_Res_انکڑے->p_جڑ);
    p_Res_انکڑے->p16_ثنائی_ہندسہ_Units=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    إذا(length>=0 && (length/4)<((format_نسخہ[0]==1 && format_نسخہ[1]==0) ? 1 : 1+5)) {
        *error_کوڈ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_انکڑے);
        أرجع;
    }

    /* currently, we accept only resources that have a Table as their roots */
    جڑ_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_انکڑے->جڑ_Res));
    إذا(!URES_IS_جدول(جڑ_Type)) {
        *error_کوڈ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_انکڑے);
        أرجع;
    }

    إذا(format_نسخہ[0]==1 && format_نسخہ[1]==0) {
        p_Res_انکڑے->local_کلید_حد_بندی=0x10000;  /* greater than any 16-bit key string offset */
    } وإلا {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ثابت int32_t *indexes=p_Res_انکڑے->p_جڑ+1;
        int32_t اشاریہ_لمائی=indexes[URES_اشاریہ_لمائی]&0xff;
        إذا(اشاریہ_لمائی<=URES_اشاریہ_MAX_جدول_لمائی) {
            *error_کوڈ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_انکڑے);
            أرجع;
        }
        إذا( length>=0 &&
            (length<((1+اشاریہ_لمائی)<<2) ||
             length<(indexes[URES_اشاریہ_BUNDLE_چوٹی]<<2))
        ) {
            *error_کوڈ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_انکڑے);
            أرجع;
        }
        إذا(indexes[URES_اشاریہ_کلیدیں_چوٹی]>(1+اشاریہ_لمائی)) {
            p_Res_انکڑے->local_کلید_حد_بندی=indexes[URES_اشاریہ_کلیدیں_چوٹی]<<2;
        }
        إذا(format_نسخہ[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_انکڑے->pool_String_اشاریہ_حد_بندی = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_اشاریہ_لمائی]) >> 8);
        }
        إذا(اشاریہ_لمائی>URES_اشاریہ_گن) {
            int32_t att=indexes[URES_اشاریہ_گن];
            p_Res_انکڑے->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_انکڑے->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_انکڑے->ورتدا_Pool_Bundle = static_cast<UBool>((att & URES_ATT_ورتدا_POOL_BUNDLE) != 0);
            p_Res_انکڑے->pool_String_اشاریہ_حد_بندی|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_انکڑے->pool_String_Index16_حد_بندی = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        إذا((p_Res_انکڑے->isPoolBundle || p_Res_انکڑے->ورتدا_Pool_Bundle) && اشاریہ_لمائی<=URES_اشاریہ_POOL_CHECKSUM) {
            *error_کوڈ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_انکڑے);
            أرجع;
        }
        إذا( اشاریہ_لمائی>URES_اشاریہ_16_ثنائی_ہندسہ_چوٹی &&
            indexes[URES_اشاریہ_16_ثنائی_ہندسہ_چوٹی]>indexes[URES_اشاریہ_کلیدیں_چوٹی]
        ) {
            p_Res_انکڑے->p16_ثنائی_ہندسہ_Units = reinterpret_cast<ثابت uint16_t*>(p_Res_انکڑے->p_جڑ + indexes[URES_اشاریہ_کلیدیں_چوٹی]);
        }
    }

    إذا(format_نسخہ[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_انکڑے->useNativeStrcmp=سچ;
    }
}

U_CAPI فراغ U_EXPORT2
res_read(Resource_انکڑے *p_Res_انکڑے,
         ثابت UDataInfo *pInfo, ثابت فراغ *وچ_بائٹاں, int32_t length,
         UError_کوڈ *error_کوڈ) {
    UVersionInfo format_نسخہ;

    uprv_memset(p_Res_انکڑے, 0, حجم(Resource_انکڑے));
    إذا(U_FAILURE(*error_کوڈ)) {
        أرجع;
    }
    إذا(!isAcceptable(format_نسخہ, خالی_سوچک, خالی_سوچک, pInfo)) {
        *error_کوڈ=U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    res_init(p_Res_انکڑے, format_نسخہ, وچ_بائٹاں, length, error_کوڈ);
}

U_CFUNC فراغ
res_لدو(Resource_انکڑے *p_Res_انکڑے,
         ثابت حرف *رستہ, ثابت حرف *name, UError_کوڈ *error_کوڈ) {
    UVersionInfo format_نسخہ;

    uprv_memset(p_Res_انکڑے, 0, حجم(Resource_انکڑے));

    /* load the ResourceBundle file */
    p_Res_انکڑے->انکڑے=udata_openChoice(رستہ, "res", name, isAcceptable, format_نسخہ, error_کوڈ);
    إذا(U_FAILURE(*error_کوڈ)) {
        أرجع;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_انکڑے, format_نسخہ, udata_get_حافظہ(p_Res_انکڑے->انکڑے), -1, error_کوڈ);
}

U_CFUNC فراغ
res_unload(Resource_انکڑے *p_Res_انکڑے) {
    إذا(p_Res_انکڑے->انکڑے!=خالی_سوچک) {
        udata_close(p_Res_انکڑے->انکڑے);
        p_Res_انکڑے->انکڑے=خالی_سوچک;
    }
}

ساكن ثابت int8_t g_عوامی_Types[URES_حد_بندی] = {
    URES_STRING,
    URES_ثنائی,
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
    URES_INT_سمتیہ,
    URES_NONE
};

U_CAPI UResType U_EXPORT2
res_get_عوامی_Type(Resource res) {
    أرجع (UResType)g_عوامی_Types[RES_GET_TYPE(res)];
}

U_CAPI ثابت char16_t * U_EXPORT2
res_getStringNoTrace(ثابت Resource_انکڑے *p_Res_انکڑے, Resource res, int32_t *p_لمائی) {
    ثابت char16_t *p;
    uint32_t وتھ=RES_GET_وتھ(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        إذا((int32_t)وتھ<p_Res_انکڑے->pool_String_اشاریہ_حد_بندی) {
            p=(ثابت char16_t *)p_Res_انکڑے->pool_Bundle_لڑیاں+وتھ;
        } وإلا {
            p=(ثابت char16_t *)p_Res_انکڑے->p16_ثنائی_ہندسہ_Units+(وتھ-p_Res_انکڑے->pool_String_اشاریہ_حد_بندی);
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
    } وإلا إذا(res==وتھ) /* RES_GET_TYPE(res)==URES_STRING */ {
        ثابت int32_t *p32= res==0 ? &g_خالی_تھاواں_String.length : p_Res_انکڑے->p_جڑ+res;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=خالی_سوچک;
        length=0;
    }
    إذا(p_لمائی) {
        *p_لمائی=length;
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
UBool isNoInheritanceMarker(ثابت Resource_انکڑے *p_Res_انکڑے, Resource res) {
    uint32_t وتھ=RES_GET_وتھ(res);
    إذا (وتھ == 0) {
        // empty string
    } وإلا إذا (res == وتھ) {
        ثابت int32_t *p32=p_Res_انکڑے->p_جڑ+res;
        int32_t length=*p32;
        ثابت char16_t* p = reinterpret_cast<ثابت char16_t*>(p32);
        أرجع length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } وإلا إذا (RES_GET_TYPE(res) == URES_STRING_V2) {
        ثابت char16_t *p;
        إذا (static_cast<int32_t>(وتھ) < p_Res_انکڑے->pool_String_اشاریہ_حد_بندی) {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_انکڑے->pool_Bundle_لڑیاں) + وتھ;
        } وإلا {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_انکڑے->p16_ثنائی_ہندسہ_Units) + (وتھ - p_Res_انکڑے->pool_String_اشاریہ_حد_بندی);
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

int32_t getStringArray(ثابت Resource_انکڑے *p_Res_انکڑے, ثابت icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    إذا(dest == خالی_سوچک ? capacity != 0 : capacity < 0) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    int32_t length = array.get_ناپ();
    إذا(length == 0) {
        أرجع 0;
    }
    إذا(length > capacity) {
        error_کوڈ = U_بفر_حد_توں_ودھ_ERROR;
        أرجع length;
    }
    لكل(int32_t i = 0; i < length; ++i) {
        int32_t s_لمائی;
        // No tracing: handled by the caller
        ثابت char16_t *s = res_getStringNoTrace(p_Res_انکڑے, array.اندرونی_Get_Resource(p_Res_انکڑے, i), &s_لمائی);
        إذا(s == خالی_سوچک) {
            error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
            أرجع 0;
        }
        dest[i].سیٹ_منزل(سچ, s, s_لمائی);
    }
    أرجع length;
}

}  // namespace

U_CAPI ثابت char16_t * U_EXPORT2
res_getAlias(ثابت Resource_انکڑے *p_Res_انکڑے, Resource res, int32_t *p_لمائی) {
    ثابت char16_t *p;
    uint32_t وتھ=RES_GET_وتھ(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_ALIAS) {
        ثابت int32_t *p32= وتھ==0 ? &g_خالی_تھاواں_String.length : p_Res_انکڑے->p_جڑ+وتھ;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=خالی_سوچک;
        length=0;
    }
    إذا(p_لمائی) {
        *p_لمائی=length;
    }
    أرجع p;
}

U_CAPI ثابت uint8_t * U_EXPORT2
res_get_ثنائی_No_Trace(ثابت Resource_انکڑے *p_Res_انکڑے, Resource res, int32_t *p_لمائی) {
    ثابت uint8_t *p;
    uint32_t وتھ=RES_GET_وتھ(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_ثنائی) {
        ثابت int32_t *p32= وتھ==0 ? (ثابت int32_t*)&gEmpty32 : p_Res_انکڑے->p_جڑ+وتھ;
        length=*p32++;
        p=(ثابت uint8_t *)p32;
    } وإلا {
        p=خالی_سوچک;
        length=0;
    }
    إذا(p_لمائی) {
        *p_لمائی=length;
    }
    أرجع p;
}


U_CAPI ثابت int32_t * U_EXPORT2
res_get_Int_سمتیہ_No_Trace(ثابت Resource_انکڑے *p_Res_انکڑے, Resource res, int32_t *p_لمائی) {
    ثابت int32_t *p;
    uint32_t وتھ=RES_GET_وتھ(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_INT_سمتیہ) {
        p= وتھ==0 ? (ثابت int32_t *)&gEmpty32 : p_Res_انکڑے->p_جڑ+وتھ;
        length=*p++;
    } وإلا {
        p=خالی_سوچک;
        length=0;
    }
    إذا(p_لمائی) {
        *p_لمائی=length;
    }
    أرجع p;
}

U_CAPI int32_t U_EXPORT2
res_گنتی_Array_اشیاء(ثابت Resource_انکڑے *p_Res_انکڑے, Resource res) {
    uint32_t وتھ=RES_GET_وتھ(res);
    بدل(RES_GET_TYPE(res)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
    حالة URES_ثنائی:
    حالة URES_ALIAS:
    حالة URES_INT:
    حالة URES_INT_سمتیہ:
        أرجع 1;
    حالة URES_ARRAY:
    حالة URES_TABLE32:
        أرجع وتھ==0 ? 0 : *(p_Res_انکڑے->p_جڑ+وتھ);
    حالة URES_جدول:
        أرجع وتھ==0 ? 0 : *((ثابت uint16_t *)(p_Res_انکڑے->p_جڑ+وتھ));
    حالة URES_ARRAY16:
    حالة URES_TABLE16:
        أرجع p_Res_انکڑے->p16_ثنائی_ہندسہ_Units[وتھ];
    افتراضي:
        أرجع 0;
    }
}

U_NAMESPACE_BEGIN

Resource_انکڑے_قدر::~Resource_انکڑے_قدر() {}

UResType Resource_انکڑے_قدر::getType() ثابت {
    أرجع res_get_عوامی_Type(res);
}

ثابت char16_t *Resource_انکڑے_قدر::getString(int32_t &length, UError_کوڈ &error_کوڈ) ثابت {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    ثابت char16_t *s = res_getString(fTraceInfo, &get_انکڑے(), res, &length);
    إذا(s == خالی_سوچک) {
        error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ثابت char16_t *Resource_انکڑے_قدر::getAliasString(int32_t &length, UError_کوڈ &error_کوڈ) ثابت {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    ثابت char16_t *s = res_getAlias(&get_انکڑے(), res, &length);
    إذا(s == خالی_سوچک) {
        error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

int32_t Resource_انکڑے_قدر::getInt(UError_کوڈ &error_کوڈ) ثابت {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getInt(fTraceInfo, res);
}

uint32_t Resource_انکڑے_قدر::getUInt(UError_کوڈ &error_کوڈ) ثابت {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getUInt(fTraceInfo, res);
}

ثابت int32_t *Resource_انکڑے_قدر::get_Int_سمتیہ(int32_t &length, UError_کوڈ &error_کوڈ) ثابت {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    ثابت int32_t *iv = res_get_Int_سمتیہ(fTraceInfo, &get_انکڑے(), res, &length);
    إذا(iv == خالی_سوچک) {
        error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع iv;
}

ثابت uint8_t *Resource_انکڑے_قدر::get_ثنائی(int32_t &length, UError_کوڈ &error_کوڈ) ثابت {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    ثابت uint8_t *b = res_get_ثنائی(fTraceInfo, &get_انکڑے(), res, &length);
    إذا(b == خالی_سوچک) {
        error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع b;
}

ResourceArray Resource_انکڑے_قدر::getArray(UError_کوڈ &error_کوڈ) ثابت {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع {};
    }
    ثابت uint16_t *items16 = خالی_سوچک;
    ثابت Resource *items32 = خالی_سوچک;
    uint32_t وتھ=RES_GET_وتھ(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_ARRAY:
        إذا (وتھ!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ثابت Resource*>(get_انکڑے().p_جڑ) + وتھ;
            length = *items32++;
        }
        اكسر;
    حالة URES_ARRAY16:
        items16 = get_انکڑے().p16_ثنائی_ہندسہ_Units+وتھ;
        length = *items16++;
        اكسر;
    افتراضي:
        error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع ResourceArray(items16, items32, length, fTraceInfo);
}

Resource_جدول Resource_انکڑے_قدر::get_جدول(UError_کوڈ &error_کوڈ) ثابت {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع {};
    }
    ثابت uint16_t *keys16 = خالی_سوچک;
    ثابت int32_t *keys32 = خالی_سوچک;
    ثابت uint16_t *items16 = خالی_سوچک;
    ثابت Resource *items32 = خالی_سوچک;
    uint32_t وتھ = RES_GET_وتھ(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_جدول:
        إذا (وتھ != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ثابت uint16_t*>(get_انکڑے().p_جڑ + وتھ);
            length = *keys16++;
            items32 = reinterpret_cast<ثابت Resource*>(keys16 + length + (~length & 1));
        }
        اكسر;
    حالة URES_TABLE16:
        keys16 = get_انکڑے().p16_ثنائی_ہندسہ_Units+وتھ;
        length = *keys16++;
        items16 = keys16 + length;
        اكسر;
    حالة URES_TABLE32:
        إذا (وتھ != 0) {  // empty if offset==0
            keys32 = get_انکڑے().p_جڑ+وتھ;
            length = *keys32++;
            items32 = reinterpret_cast<ثابت Resource*>(keys32) + length;
        }
        اكسر;
    افتراضي:
        error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع Resource_جدول(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_انکڑے_قدر::isNoInheritanceMarker() ثابت {
    أرجع ::isNoInheritanceMarker(&get_انکڑے(), res);
}

int32_t Resource_انکڑے_قدر::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UError_کوڈ &error_کوڈ) ثابت {
    أرجع ::getStringArray(&get_انکڑے(), getArray(error_کوڈ), dest, capacity, error_کوڈ);
}

int32_t Resource_انکڑے_قدر::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UError_کوڈ &error_کوڈ) ثابت {
    إذا(URES_IS_ARRAY(res)) {
        أرجع ::getStringArray(&get_انکڑے(), getArray(error_کوڈ), dest, capacity, error_کوڈ);
    }
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع 0;
    }
    إذا(dest == خالی_سوچک ? capacity != 0 : capacity < 0) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    إذا(capacity < 1) {
        error_کوڈ = U_بفر_حد_توں_ودھ_ERROR;
        أرجع 1;
    }
    int32_t s_لمائی;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_انکڑے(), res, &s_لمائی);
    إذا(s != خالی_سوچک) {
        dest[0].سیٹ_منزل(سچ, s, s_لمائی);
        أرجع 1;
    }
    error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0;
}

UnicodeString Resource_انکڑے_قدر::get_String_Or_پہلا_دا_Array(UError_کوڈ &error_کوڈ) ثابت {
    UnicodeString us;
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع us;
    }
    int32_t s_لمائی;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_انکڑے(), res, &s_لمائی);
    إذا(s != خالی_سوچک) {
        us.سیٹ_منزل(سچ, s, s_لمائی);
        أرجع us;
    }
    ResourceArray array = getArray(error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع us;
    }
    إذا(array.get_ناپ() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_انکڑے(), array.اندرونی_Get_Resource(&get_انکڑے(), 0), &s_لمائی);
        إذا(s != خالی_سوچک) {
            us.سیٹ_منزل(سچ, s, s_لمائی);
            أرجع us;
        }
    }
    error_کوڈ = U_RESOURCE_TYPE_MISMATCH;
    أرجع us;
}

U_NAMESPACE_سرا

ساكن Resource
makeResourceFrom16(ثابت Resource_انکڑے *p_Res_انکڑے, int32_t res16) {
    إذا(res16<p_Res_انکڑے->pool_String_Index16_حد_بندی) {
        // Pool string, nothing to do.
    } وإلا {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_انکڑے->pool_String_Index16_حد_بندی+p_Res_انکڑے->pool_String_اشاریہ_حد_بندی;
    }
    أرجع URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_جدول_شے_By_کلید(ثابت Resource_انکڑے *p_Res_انکڑے, Resource جدول,
                      int32_t *اشاریہ_R, ثابت حرف **کلید) {
    uint32_t وتھ=RES_GET_وتھ(جدول);
    int32_t length;
    int32_t idx;
    إذا(کلید == خالی_سوچک || *کلید == خالی_سوچک) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(جدول)) {
    حالة URES_جدول: {
        إذا (وتھ!=0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_انکڑے->p_جڑ+وتھ);
            length=*p++;
            *اشاریہ_R=idx=res_لبھو_جدول_شے(p_Res_انکڑے, p, length, *کلید, کلید);
            إذا(idx>=0) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                أرجع p32[idx];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=p_Res_انکڑے->p16_ثنائی_ہندسہ_Units+وتھ;
        length=*p++;
        *اشاریہ_R=idx=res_لبھو_جدول_شے(p_Res_انکڑے, p, length, *کلید, کلید);
        إذا(idx>=0) {
            أرجع makeResourceFrom16(p_Res_انکڑے, p[length+idx]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (وتھ!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_انکڑے->p_جڑ+وتھ;
            length=*p++;
            *اشاریہ_R=idx=res_لبھو_Table32_شے(p_Res_انکڑے, p, length, *کلید, کلید);
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
res_get_جدول_شے_By_اشاریہ(ثابت Resource_انکڑے *p_Res_انکڑے, Resource جدول,
                        int32_t اشاریہ_R, ثابت حرف **کلید) {
    uint32_t وتھ=RES_GET_وتھ(جدول);
    int32_t length;
    إذا (اشاریہ_R < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(جدول)) {
    حالة URES_جدول: {
        إذا (وتھ != 0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_انکڑے->p_جڑ+وتھ);
            length=*p++;
            إذا(اشاریہ_R<length) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                إذا(کلید!=خالی_سوچک) {
                    *کلید=RES_GET_KEY16(p_Res_انکڑے, p[اشاریہ_R]);
                }
                أرجع p32[اشاریہ_R];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=p_Res_انکڑے->p16_ثنائی_ہندسہ_Units+وتھ;
        length=*p++;
        إذا(اشاریہ_R<length) {
            إذا(کلید!=خالی_سوچک) {
                *کلید=RES_GET_KEY16(p_Res_انکڑے, p[اشاریہ_R]);
            }
            أرجع makeResourceFrom16(p_Res_انکڑے, p[length+اشاریہ_R]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (وتھ != 0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_انکڑے->p_جڑ+وتھ;
            length=*p++;
            إذا(اشاریہ_R<length) {
                إذا(کلید!=خالی_سوچک) {
                    *کلید=RES_GET_KEY32(p_Res_انکڑے, p[اشاریہ_R]);
                }
                أرجع (Resource)p[length+اشاریہ_R];
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
res_getResource(ثابت Resource_انکڑے *p_Res_انکڑے, ثابت حرف *کلید) {
    ثابت حرف *real_کلید=کلید;
    int32_t idx;
    أرجع res_get_جدول_شے_By_کلید(p_Res_انکڑے, p_Res_انکڑے->جڑ_Res, &idx, &real_کلید);
}


UBool icu::Resource_جدول::get_کلید_And_قدر(int32_t i,
                                         ثابت حرف *&کلید, icu::Resource_قدر &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_انکڑے_قدر &rd_قدر = static_cast<icu::Resource_انکڑے_قدر &>(value);
        إذا (keys16 != خالی_سوچک) {
            کلید = RES_GET_KEY16(&rd_قدر.get_انکڑے(), keys16[i]);
        } وإلا {
            کلید = RES_GET_KEY32(&rd_قدر.get_انکڑے(), keys32[i]);
        }
        Resource res;
        إذا (items16 != خالی_سوچک) {
            res = makeResourceFrom16(&rd_قدر.get_انکڑے(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_قدر.سیٹ_Resource(res, ResourceTracer(fTraceInfo, کلید));
        أرجع سچ;
    }
    أرجع خطأ;
}

UBool icu::Resource_جدول::لبھو_قدر(ثابت حرف *کلید, Resource_قدر &value) ثابت {
    icu::Resource_انکڑے_قدر &rd_قدر = static_cast<icu::Resource_انکڑے_قدر &>(value);
    ثابت حرف *real_کلید = خالی_سوچک;
    int32_t i;
    إذا (keys16 != خالی_سوچک) {
        i = res_لبھو_جدول_شے(&rd_قدر.get_انکڑے(), keys16, length, کلید, &real_کلید);
    } وإلا {
        i = res_لبھو_Table32_شے(&rd_قدر.get_انکڑے(), keys32, length, کلید, &real_کلید);
    }
    إذا (i >= 0) {
        Resource res;
        إذا (items16 != خالی_سوچک) {
            res = makeResourceFrom16(&rd_قدر.get_انکڑے(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_قدر.سیٹ_Resource(res, ResourceTracer(fTraceInfo, کلید));
        أرجع سچ;
    }
    أرجع خطأ;
}

U_CAPI Resource U_EXPORT2
res_get_Array_شے(ثابت Resource_انکڑے *p_Res_انکڑے, Resource array, int32_t اشاریہ_R) {
    uint32_t وتھ=RES_GET_وتھ(array);
    إذا (اشاریہ_R < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(array)) {
    حالة URES_ARRAY: {
        إذا (وتھ!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_انکڑے->p_جڑ+وتھ;
            إذا(اشاریہ_R<*p) {
                أرجع (Resource)p[1+اشاریہ_R];
            }
        }
        اكسر;
    }
    حالة URES_ARRAY16: {
        ثابت uint16_t *p=p_Res_انکڑے->p16_ثنائی_ہندسہ_Units+وتھ;
        إذا(اشاریہ_R<*p) {
            أرجع makeResourceFrom16(p_Res_انکڑے, p[1+اشاریہ_R]);
        }
        اكسر;
    }
    افتراضي:
        اكسر;
    }
    أرجع RES_BOGUS;
}

uint32_t icu::ResourceArray::اندرونی_Get_Resource(ثابت Resource_انکڑے *p_Res_انکڑے, int32_t i) ثابت {
    إذا (items16 != خالی_سوچک) {
        أرجع makeResourceFrom16(p_Res_انکڑے, items16[i]);
    } وإلا {
        أرجع items32[i];
    }
}

UBool icu::ResourceArray::get_قدر(int32_t i, icu::Resource_قدر &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_انکڑے_قدر &rd_قدر = static_cast<icu::Resource_انکڑے_قدر &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_قدر.سیٹ_Resource(
            اندرونی_Get_Resource(&rd_قدر.get_انکڑے(), i),
            ResourceTracer(fTraceInfo, i));
        أرجع سچ;
    }
    أرجع خطأ;
}

U_CFUNC Resource
res_لبھو_Resource(ثابت Resource_انکڑے *p_Res_انکڑے, Resource r, حرف** رستہ, ثابت حرف** کلید) {
  حرف *رستہ_P = *رستہ, *اگلا_Sep_P = *رستہ;
  حرف *close_اشاریہ = خالی_سوچک;
  Resource t1 = r;
  Resource t2;
  int32_t اشاریہ_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  إذا(!uprv_strlen(رستہ_P)) {
      أرجع r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  إذا(!URES_IS_CONTAINER(type)) {
      أرجع RES_BOGUS;
  }
  
  طالما(اگلا_Sep_P && *رستہ_P && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    اگلا_Sep_P = uprv_strchr(رستہ_P, RES_رستہ_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    إذا(اگلا_Sep_P != خالی_سوچک) {
      إذا(اگلا_Sep_P == رستہ_P) {
        // Empty key string.
        أرجع RES_BOGUS;
      }
      *اگلا_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *رستہ = اگلا_Sep_P+1;
    } وإلا {
      *رستہ = uprv_strchr(رستہ_P, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    إذا(URES_IS_جدول(type)) {
      *کلید = رستہ_P;
      t2 = res_get_جدول_شے_By_کلید(p_Res_انکڑے, t1, &اشاریہ_R, کلید);
    } وإلا إذا(URES_IS_ARRAY(type)) {
      اشاریہ_R = uprv_strtol(رستہ_P, &close_اشاریہ, 10);
      إذا(اشاریہ_R >= 0 && *close_اشاریہ == 0) {
        t2 = res_get_Array_شے(p_Res_انکڑے, t1, اشاریہ_R);
      } وإلا {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *کلید = خالی_سوچک;
    } وإلا { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    رستہ_P = *رستہ;
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
تعريفنوع بنية صف_2 {
    int32_t کلید_اشاریہ, ترتیب_دیو_اشاریہ;
} صف_2;

ساكن int32_t U_CALLCONV
ures_compare_صفاں(ثابت فراغ *context, ثابت فراغ *کھبے, ثابت فراغ *سجے) {
    ثابت حرف* کلید_Chars = static_cast<ثابت حرف*>(context);
    أرجع static_cast<int32_t>(uprv_strcmp(کلید_Chars + static_cast<ثابت صف_2*>(کھبے)->کلید_اشاریہ,
                                            کلید_Chars + static_cast<ثابت صف_2*>(سجے)->کلید_اشاریہ));
}

تعريفنوع بنية عارضی_جدول_2 {
    ثابت حرف *کلید_Chars;
    صف_2 *صفاں;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_کلید_حد_بندی;
    uint8_t major_Format_نسخہ;
} عارضی_جدول_2;

تعداد {
    ڈھیر_صف_سمرتھا=200
};

/* The table item key string is not locally available. */
ساكن ثابت حرف *ثابت g_Unknown_کلید="";

#إذا !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ساكن ثابت char16_t g_Collation_Bin_کلید[]=u"%%CollationBin";
#جے_انت

/*
 * swap one resource item
 */
ساكن فراغ
ures_وٹاندرا_Resource(ثابت UDataSwapper *ds,
                  ثابت Resource *وچ_Bundle, Resource *باہرلا_Bundle,
                  Resource res, /* caller swaps res itself */
                  ثابت حرف *کلید,
                  عارضی_جدول_2 *p_عارضی_جدول,
                  UError_کوڈ *p_Error_کوڈ) {
    ثابت Resource *p;
    Resource *q;
    int32_t وتھ, count;

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
    وتھ = static_cast<int32_t>(RES_GET_وتھ(res));
    إذا(وتھ==0) {
        /* special offset indicating an empty item */
        أرجع;
    }
    إذا (p_عارضی_جدول->resFlags[وتھ >> 5] & (static_cast<uint32_t>(1) << (وتھ & 0x1f))) {
        /* we already swapped this resource item */
        أرجع;
    } وإلا {
        /* mark it as swapped now */
        p_عارضی_جدول->resFlags[وتھ >> 5] |= static_cast<uint32_t>(1) << (وتھ & 0x1f);
    }

    p=وچ_Bundle+وتھ;
    q=باہرلا_Bundle+وتھ;

    بدل(RES_GET_TYPE(res)) {
    حالة URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    حالة URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->وٹاندرا_Array32(ds, p, 4, q, p_Error_کوڈ);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->وٹاندرا_Array16(ds, p+1, 2*count, q+1, p_Error_کوڈ);
        اكسر;
    حالة URES_ثنائی:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->وٹاندرا_Array32(ds, p, 4, q, p_Error_کوڈ);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#إذا !UCONFIG_NO_COLLATION
        إذا( کلید!=خالی_سوچک &&  /* the binary is in a table */
            (کلید!=g_Unknown_کلید ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, کلید, -1,
                                       g_Collation_Bin_کلید, UPRV_LENGTHOF(g_Collation_Bin_کلید)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_ثنائی(ds, p+1, count))
        ) {
            ucol_وٹاندرا(ds, p+1, count, q+1, p_Error_کوڈ);
        }
#جے_انت
        اكسر;
    حالة URES_جدول:
    حالة URES_TABLE32:
        {
            ثابت uint16_t *pKey16;
            uint16_t *qKey16;

            ثابت int32_t *pKey32;
            int32_t *qKey32;

            Resource شے;
            int32_t i, old_اشاریہ;

            إذا(RES_GET_TYPE(res)==URES_جدول) {
                /* get table item count */
                pKey16 = reinterpret_cast<ثابت uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=خالی_سوچک;

                /* swap count */
                ds->وٹاندرا_Array16(ds, pKey16++, 2, qKey16++, p_Error_کوڈ);

                وتھ+=((1+count)+1)/2;
            } وإلا {
                /* get table item count */
                pKey32 = reinterpret_cast<ثابت int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=خالی_سوچک;

                /* swap count */
                ds->وٹاندرا_Array32(ds, pKey32++, 4, qKey32++, p_Error_کوڈ);

                وتھ+=1+count;
            }

            إذا(count==0) {
                اكسر;
            }

            p=وچ_Bundle+وتھ; /* pointer to table resources */
            q=باہرلا_Bundle+وتھ;

            /* recurse */
            لكل(i=0; i<count; ++i) {
                ثابت حرف *شے_کلید=g_Unknown_کلید;
                إذا(pKey16!=خالی_سوچک) {
                    int32_t کلید_وتھ=ds->readUInt16(pKey16[i]);
                    إذا(کلید_وتھ<p_عارضی_جدول->local_کلید_حد_بندی) {
                        شے_کلید = reinterpret_cast<ثابت حرف*>(باہرلا_Bundle) + کلید_وتھ;
                    }
                } وإلا {
                    int32_t کلید_وتھ=udata_readInt32(ds, pKey32[i]);
                    إذا(کلید_وتھ>=0) {
                        شے_کلید = reinterpret_cast<ثابت حرف*>(باہرلا_Bundle) + کلید_وتھ;
                    }
                }
                شے=ds->readUInt32(p[i]);
                ures_وٹاندرا_Resource(ds, وچ_Bundle, باہرلا_Bundle, شے, شے_کلید, p_عارضی_جدول, p_Error_کوڈ);
                إذا(U_FAILURE(*p_Error_کوڈ)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, شے);
                    أرجع;
                }
            }

            إذا(p_عارضی_جدول->major_Format_نسخہ>1 || ds->وچ_Charset==ds->باہرلا_Charset) {
                /* no need to sort, just swap the offset/value arrays */
                إذا(pKey16!=خالی_سوچک) {
                    ds->وٹاندرا_Array16(ds, pKey16, count*2, qKey16, p_Error_کوڈ);
                    ds->وٹاندرا_Array32(ds, p, count*4, q, p_Error_کوڈ);
                } وإلا {
                    /* swap key offsets and items as one array */
                    ds->وٹاندرا_Array32(ds, pKey32, count*2*4, qKey32, p_Error_کوڈ);
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
            إذا(pKey16!=خالی_سوچک) {
                لكل(i=0; i<count; ++i) {
                    p_عارضی_جدول->صفاں[i].کلید_اشاریہ=ds->readUInt16(pKey16[i]);
                    p_عارضی_جدول->صفاں[i].ترتیب_دیو_اشاریہ=i;
                }
            } وإلا {
                لكل(i=0; i<count; ++i) {
                    p_عارضی_جدول->صفاں[i].کلید_اشاریہ=udata_readInt32(ds, pKey32[i]);
                    p_عارضی_جدول->صفاں[i].ترتیب_دیو_اشاریہ=i;
                }
            }
            uprv_ترتیب_دیو_Array(p_عارضی_جدول->صفاں, count, حجم(صف_2),
                           ures_compare_صفاں, p_عارضی_جدول->کلید_Chars,
                           خطأ, p_Error_کوڈ);
            إذا(U_FAILURE(*p_Error_کوڈ)) {
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
            /*کلیدیں*/
            إذا(pKey16!=خالی_سوچک) {
                uint16_t *rKey16;

                إذا(pKey16!=qKey16) {
                    rKey16=qKey16;
                } وإلا {
                    rKey16 = reinterpret_cast<uint16_t*>(p_عارضی_جدول->resort);
                }
                لكل(i=0; i<count; ++i) {
                    old_اشاریہ=p_عارضی_جدول->صفاں[i].ترتیب_دیو_اشاریہ;
                    ds->وٹاندرا_Array16(ds, pKey16+old_اشاریہ, 2, rKey16+i, p_Error_کوڈ);
                }
                إذا(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } وإلا {
                int32_t *rKey32;

                إذا(pKey32!=qKey32) {
                    rKey32=qKey32;
                } وإلا {
                    rKey32=p_عارضی_جدول->resort;
                }
                لكل(i=0; i<count; ++i) {
                    old_اشاریہ=p_عارضی_جدول->صفاں[i].ترتیب_دیو_اشاریہ;
                    ds->وٹاندرا_Array32(ds, pKey32+old_اشاریہ, 4, rKey32+i, p_Error_کوڈ);
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
                    r = reinterpret_cast<Resource*>(p_عارضی_جدول->resort);
                }
                لكل(i=0; i<count; ++i) {
                    old_اشاریہ=p_عارضی_جدول->صفاں[i].ترتیب_دیو_اشاریہ;
                    ds->وٹاندرا_Array32(ds, p+old_اشاریہ, 4, r+i, p_Error_کوڈ);
                }
                إذا(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        اكسر;
    حالة URES_ARRAY:
        {
            Resource شے;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->وٹاندرا_Array32(ds, p++, 4, q++, p_Error_کوڈ);

            /* recurse */
            لكل(i=0; i<count; ++i) {
                شے=ds->readUInt32(p[i]);
                ures_وٹاندرا_Resource(ds, وچ_Bundle, باہرلا_Bundle, شے, خالی_سوچک, p_عارضی_جدول, p_Error_کوڈ);
                إذا(U_FAILURE(*p_Error_کوڈ)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, شے);
                    أرجع;
                }
            }

            /* swap items */
            ds->وٹاندرا_Array32(ds, p, 4*count, q, p_Error_کوڈ);
        }
        اكسر;
    حالة URES_INT_سمتیہ:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->وٹاندرا_Array32(ds, p, 4*(1+count), q, p_Error_کوڈ);
        اكسر;
    افتراضي:
        /* also catches RES_BOGUS */
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        اكسر;
    }
}

U_CAPI int32_t U_EXPORT2
ures_وٹاندرا(ثابت UDataSwapper *ds,
          ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
          UError_کوڈ *p_Error_کوڈ) {
    ثابت UDataInfo *pInfo;
    ثابت Resource *وچ_Bundle;
    Resource جڑ_Res;
    int32_t سرخی_ناپ, max_جدول_لمائی;

    صف_2 صفاں[ڈھیر_صف_سمرتھا];
    int32_t resort[ڈھیر_صف_سمرتھا];
    عارضی_جدول_2 عارضی_جدول;

    ثابت int32_t *وچ_Indexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_لمائی, اشاریہ_لمائی, کلیدیں_Bottom, کلیدیں_چوٹی, resBottom, چوٹی;

    /* udata_swapDataHeader checks the arguments */
    سرخی_ناپ=udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے+4);
    إذا(!(
        pInfo->انکڑے_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->انکڑے_Format[1]==0x65 &&
        pInfo->انکڑے_Format[2]==0x73 &&
        pInfo->انکڑے_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->format_نسخہ[0]==1 && pInfo->format_نسخہ[1]>=1) ||
            pInfo->format_نسخہ[0]==2 || pInfo->format_نسخہ[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->انکڑے_Format[0], pInfo->انکڑے_Format[1],
                         pInfo->انکڑے_Format[2], pInfo->انکڑے_Format[3],
                         pInfo->format_نسخہ[0], pInfo->format_نسخہ[1]);
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }
    عارضی_جدول.major_Format_نسخہ=pInfo->format_نسخہ[0];

    /* a resource bundle must contain at least one resource item */
    إذا(length<0) {
        bundle_لمائی=-1;
    } وإلا {
        bundle_لمائی=(length-سرخی_ناپ)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        إذا(bundle_لمائی<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-سرخی_ناپ);
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    وچ_Bundle=(ثابت Resource *)((ثابت حرف *)وچ_انکڑے+سرخی_ناپ);
    جڑ_Res=ds->readUInt32(*وچ_Bundle);

    /* formatVersion 1.1 adds the indexes[] array */
    وچ_Indexes=(ثابت int32_t *)(وچ_Bundle+1);

    اشاریہ_لمائی=udata_readInt32(ds, وچ_Indexes[URES_اشاریہ_لمائی])&0xff;
    إذا(اشاریہ_لمائی<=URES_اشاریہ_MAX_جدول_لمائی) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    کلیدیں_Bottom=1+اشاریہ_لمائی;
    کلیدیں_چوٹی=udata_readInt32(ds, وچ_Indexes[URES_اشاریہ_کلیدیں_چوٹی]);
    إذا(اشاریہ_لمائی>URES_اشاریہ_16_ثنائی_ہندسہ_چوٹی) {
        resBottom=udata_readInt32(ds, وچ_Indexes[URES_اشاریہ_16_ثنائی_ہندسہ_چوٹی]);
    } وإلا {
        resBottom=کلیدیں_چوٹی;
    }
    چوٹی=udata_readInt32(ds, وچ_Indexes[URES_اشاریہ_BUNDLE_چوٹی]);
    max_جدول_لمائی=udata_readInt32(ds, وچ_Indexes[URES_اشاریہ_MAX_جدول_لمائی]);

    إذا(0<=bundle_لمائی && bundle_لمائی<چوٹی) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         چوٹی, bundle_لمائی);
        *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا(کلیدیں_چوٹی>(1+اشاریہ_لمائی)) {
        عارضی_جدول.local_کلید_حد_بندی=کلیدیں_چوٹی<<2;
    } وإلا {
        عارضی_جدول.local_کلید_حد_بندی=0;
    }

    إذا(length>=0) {
        Resource *باہرلا_Bundle=(Resource *)((حرف *)باہرلا_انکڑے+سرخی_ناپ);

        /* track which resources we have already swapped */
        uint32_t ڈھیر_Res_Flags[ڈھیر_صف_سمرتھا];
        int32_t res_Flags_لمائی;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_لمائی=(length+31)>>5;          /* number of bytes needed */
        res_Flags_لمائی=(res_Flags_لمائی+3)&~3;   /* multiple of 4 bytes for uint32_t */
        إذا(res_Flags_لمائی<=(int32_t)حجم(ڈھیر_Res_Flags)) {
            عارضی_جدول.resFlags=ڈھیر_Res_Flags;
        } وإلا {
            عارضی_جدول.resFlags=(uint32_t *)uprv_malloc(res_Flags_لمائی);
            إذا(عارضی_جدول.resFlags==خالی_سوچک) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
                أرجع 0;
            }
        }
        uprv_memset(عارضی_جدول.resFlags, 0, res_Flags_لمائی);

        /* copy the bundle for binary and inaccessible data */
        إذا(وچ_انکڑے!=باہرلا_انکڑے) {
            uprv_memcpy(باہرلا_Bundle, وچ_Bundle, 4*چوٹی);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_وٹاندرا_Inv_String_بلاک(ds, وچ_Bundle+کلیدیں_Bottom, 4*(کلیدیں_چوٹی-کلیدیں_Bottom),
                                    باہرلا_Bundle+کلیدیں_Bottom, p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(کلیدیں_چوٹی-کلیدیں_Bottom));
            إذا(عارضی_جدول.resFlags!=ڈھیر_Res_Flags) {
                uprv_free(عارضی_جدول.resFlags);
            }
            أرجع 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        إذا(کلیدیں_چوٹی<resBottom) {
            ds->وٹاندرا_Array16(ds, وچ_Bundle+کلیدیں_چوٹی, (resBottom-کلیدیں_چوٹی)*4, باہرلا_Bundle+کلیدیں_چوٹی, p_Error_کوڈ);
            إذا(U_FAILURE(*p_Error_کوڈ)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-کلیدیں_چوٹی));
                إذا(عارضی_جدول.resFlags!=ڈھیر_Res_Flags) {
                    uprv_free(عارضی_جدول.resFlags);
                }
                أرجع 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        عارضی_جدول.کلید_Chars=(ثابت حرف *)باہرلا_Bundle; /* sort by outCharset */
        إذا(عارضی_جدول.major_Format_نسخہ>1 || max_جدول_لمائی<=ڈھیر_صف_سمرتھا) {
            عارضی_جدول.صفاں=صفاں;
            عارضی_جدول.resort=resort;
        } وإلا {
            عارضی_جدول.صفاں=(صف_2 *)uprv_malloc(max_جدول_لمائی*حجم(صف_2)+max_جدول_لمائی*4);
            إذا(عارضی_جدول.صفاں==خالی_سوچک) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_جدول_لمائی);
                *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
                إذا(عارضی_جدول.resFlags!=ڈھیر_Res_Flags) {
                    uprv_free(عارضی_جدول.resFlags);
                }
                أرجع 0;
            }
            عارضی_جدول.resort=(int32_t *)(عارضی_جدول.صفاں+max_جدول_لمائی);
        }

        /* swap the resources */
        ures_وٹاندرا_Resource(ds, وچ_Bundle, باہرلا_Bundle, جڑ_Res, خالی_سوچک, &عارضی_جدول, p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             جڑ_Res);
        }

        إذا(عارضی_جدول.صفاں!=صفاں) {
            uprv_free(عارضی_جدول.صفاں);
        }
        إذا(عارضی_جدول.resFlags!=ڈھیر_Res_Flags) {
            uprv_free(عارضی_جدول.resFlags);
        }

        /* swap the root resource and indexes */
        ds->وٹاندرا_Array32(ds, وچ_Bundle, کلیدیں_Bottom*4, باہرلا_Bundle, p_Error_کوڈ);
    }

    أرجع سرخی_ناپ+4*چوٹی;
}

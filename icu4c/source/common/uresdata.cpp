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
#وصف_ڪريو RES_GET_KEY16(p_Res_معلومات, ڪنجي_وٿي) \
    ((ڪنجي_وٿي)<(p_Res_معلومات)->local_ڪنجي_حد_بندي ? \
        (ثابت حرف *)(p_Res_معلومات)->p_پاڙ+(ڪنجي_وٿي) : \
        (p_Res_معلومات)->pool_Bundle_ڪنجيون+(ڪنجي_وٿي)-(p_Res_معلومات)->local_ڪنجي_حد_بندي)

#وصف_ڪريو RES_GET_KEY32(p_Res_معلومات, ڪنجي_وٿي) \
    ((ڪنجي_وٿي)>=0 ? \
        (ثابت حرف *)(p_Res_معلومات)->p_پاڙ+(ڪنجي_وٿي) : \
        (p_Res_معلومات)->pool_Bundle_ڪنجيون+((ڪنجي_وٿي)&0x7fffffff))

#وصف_ڪريو URESDATA_شي_NOT_مليو -1

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
} g_خالي_جايون_String={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ساكن int32_t
res_ڳوليو_جدول_شي(ثابت Resource_معلومات *p_Res_معلومات, ثابت uint16_t *ڪنجي_Offsets, int32_t length,
                   ثابت حرف *ڪنجي, ثابت حرف **real_ڪنجي) {
    ثابت حرف *جدول_ڪنجي;
    int32_t mid, شروعات, حد_بندي;
    صحيح نتيجو;

    /* do a binary search for the key */
    شروعات=0;
    حد_بندي=length;
    طالما(شروعات<حد_بندي) {
        mid = (شروعات + حد_بندي) / 2;
        جدول_ڪنجي = RES_GET_KEY16(p_Res_معلومات, ڪنجي_Offsets[mid]);
        إذا (p_Res_معلومات->useNativeStrcmp) {
            نتيجو = uprv_strcmp(ڪنجي, جدول_ڪنجي);
        } وإلا {
            نتيجو = uprv_compareInvCharsAsAscii(ڪنجي, جدول_ڪنجي);
        }
        إذا (نتيجو < 0) {
            حد_بندي = mid;
        } وإلا إذا (نتيجو > 0) {
            شروعات = mid + 1;
        } وإلا {
            /* We found it! */
            *real_ڪنجي=جدول_ڪنجي;
            أرجع mid;
        }
    }
    أرجع URESDATA_شي_NOT_مليو;  /* not found or table is empty. */
}

ساكن int32_t
res_ڳوليو_Table32_شي(ثابت Resource_معلومات *p_Res_معلومات, ثابت int32_t *ڪنجي_Offsets, int32_t length,
                     ثابت حرف *ڪنجي, ثابت حرف **real_ڪنجي) {
    ثابت حرف *جدول_ڪنجي;
    int32_t mid, شروعات, حد_بندي;
    صحيح نتيجو;

    /* do a binary search for the key */
    شروعات=0;
    حد_بندي=length;
    طالما(شروعات<حد_بندي) {
        mid = (شروعات + حد_بندي) / 2;
        جدول_ڪنجي = RES_GET_KEY32(p_Res_معلومات, ڪنجي_Offsets[mid]);
        إذا (p_Res_معلومات->useNativeStrcmp) {
            نتيجو = uprv_strcmp(ڪنجي, جدول_ڪنجي);
        } وإلا {
            نتيجو = uprv_compareInvCharsAsAscii(ڪنجي, جدول_ڪنجي);
        }
        إذا (نتيجو < 0) {
            حد_بندي = mid;
        } وإلا إذا (نتيجو > 0) {
            شروعات = mid + 1;
        } وإلا {
            /* We found it! */
            *real_ڪنجي=جدول_ڪنجي;
            أرجع mid;
        }
    }
    أرجع URESDATA_شي_NOT_مليو;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ساكن UBool U_CALLCONV
isAcceptable(فراغ *context,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->format_نسخو, 4);
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->معلومات_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->معلومات_Format[1]==0x65 &&
        pInfo->معلومات_Format[2]==0x73 &&
        pInfo->معلومات_Format[3]==0x42 &&
        (1<=pInfo->format_نسخو[0] && pInfo->format_نسخو[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

ساكن فراغ
res_init(Resource_معلومات *p_Res_معلومات,
         UVersionInfo format_نسخو, ثابت فراغ *اندريون_بائيٽون, int32_t length,
         UError_ڪوڊ *error_ڪوڊ) {
    UResType پاڙ_Type;

    /* get the root resource */
    p_Res_معلومات->p_پاڙ = static_cast<ثابت int32_t*>(اندريون_بائيٽون);
    p_Res_معلومات->پاڙ_Res = static_cast<Resource>(*p_Res_معلومات->p_پاڙ);
    p_Res_معلومات->p16_ٻٽو_انگ_Units=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    إذا(length>=0 && (length/4)<((format_نسخو[0]==1 && format_نسخو[1]==0) ? 1 : 1+5)) {
        *error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_معلومات);
        أرجع;
    }

    /* currently, we accept only resources that have a Table as their roots */
    پاڙ_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_معلومات->پاڙ_Res));
    إذا(!URES_IS_جدول(پاڙ_Type)) {
        *error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_معلومات);
        أرجع;
    }

    إذا(format_نسخو[0]==1 && format_نسخو[1]==0) {
        p_Res_معلومات->local_ڪنجي_حد_بندي=0x10000;  /* greater than any 16-bit key string offset */
    } وإلا {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ثابت int32_t *indexes=p_Res_معلومات->p_پاڙ+1;
        int32_t اشاريو_ڊيگهائي=indexes[URES_اشاريو_ڊيگهائي]&0xff;
        إذا(اشاريو_ڊيگهائي<=URES_اشاريو_MAX_جدول_ڊيگهائي) {
            *error_ڪوڊ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_معلومات);
            أرجع;
        }
        إذا( length>=0 &&
            (length<((1+اشاريو_ڊيگهائي)<<2) ||
             length<(indexes[URES_اشاريو_BUNDLE_چوٽي]<<2))
        ) {
            *error_ڪوڊ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_معلومات);
            أرجع;
        }
        إذا(indexes[URES_اشاريو_ڪنجيون_چوٽي]>(1+اشاريو_ڊيگهائي)) {
            p_Res_معلومات->local_ڪنجي_حد_بندي=indexes[URES_اشاريو_ڪنجيون_چوٽي]<<2;
        }
        إذا(format_نسخو[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_معلومات->pool_String_اشاريو_حد_بندي = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_اشاريو_ڊيگهائي]) >> 8);
        }
        إذا(اشاريو_ڊيگهائي>URES_اشاريو_خاصيتون) {
            int32_t att=indexes[URES_اشاريو_خاصيتون];
            p_Res_معلومات->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_معلومات->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_معلومات->استعمال_ڪري_Pool_Bundle = static_cast<UBool>((att & URES_ATT_استعمال_ڪري_POOL_BUNDLE) != 0);
            p_Res_معلومات->pool_String_اشاريو_حد_بندي|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_معلومات->pool_String_Index16_حد_بندي = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        إذا((p_Res_معلومات->isPoolBundle || p_Res_معلومات->استعمال_ڪري_Pool_Bundle) && اشاريو_ڊيگهائي<=URES_اشاريو_POOL_CHECKSUM) {
            *error_ڪوڊ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_معلومات);
            أرجع;
        }
        إذا( اشاريو_ڊيگهائي>URES_اشاريو_16_ٻٽو_انگ_چوٽي &&
            indexes[URES_اشاريو_16_ٻٽو_انگ_چوٽي]>indexes[URES_اشاريو_ڪنجيون_چوٽي]
        ) {
            p_Res_معلومات->p16_ٻٽو_انگ_Units = reinterpret_cast<ثابت uint16_t*>(p_Res_معلومات->p_پاڙ + indexes[URES_اشاريو_ڪنجيون_چوٽي]);
        }
    }

    إذا(format_نسخو[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_معلومات->useNativeStrcmp=سچ;
    }
}

U_CAPI فراغ U_EXPORT2
res_read(Resource_معلومات *p_Res_معلومات,
         ثابت UDataInfo *pInfo, ثابت فراغ *اندريون_بائيٽون, int32_t length,
         UError_ڪوڊ *error_ڪوڊ) {
    UVersionInfo format_نسخو;

    uprv_memset(p_Res_معلومات, 0, حجم(Resource_معلومات));
    إذا(U_FAILURE(*error_ڪوڊ)) {
        أرجع;
    }
    إذا(!isAcceptable(format_نسخو, خالي_اشارو, خالي_اشارو, pInfo)) {
        *error_ڪوڊ=U_INVALID_FORMAT_ERROR;
        أرجع;
    }
    res_init(p_Res_معلومات, format_نسخو, اندريون_بائيٽون, length, error_ڪوڊ);
}

U_CFUNC فراغ
res_ڀريو(Resource_معلومات *p_Res_معلومات,
         ثابت حرف *رستو, ثابت حرف *name, UError_ڪوڊ *error_ڪوڊ) {
    UVersionInfo format_نسخو;

    uprv_memset(p_Res_معلومات, 0, حجم(Resource_معلومات));

    /* load the ResourceBundle file */
    p_Res_معلومات->معلومات=udata_openChoice(رستو, "res", name, isAcceptable, format_نسخو, error_ڪوڊ);
    إذا(U_FAILURE(*error_ڪوڊ)) {
        أرجع;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_معلومات, format_نسخو, udata_get_يادگيري(p_Res_معلومات->معلومات), -1, error_ڪوڊ);
}

U_CFUNC فراغ
res_unload(Resource_معلومات *p_Res_معلومات) {
    إذا(p_Res_معلومات->معلومات!=خالي_اشارو) {
        udata_close(p_Res_معلومات->معلومات);
        p_Res_معلومات->معلومات=خالي_اشارو;
    }
}

ساكن ثابت int8_t g_عوامي_Types[URES_حد_بندي] = {
    URES_STRING,
    URES_ٻٽو,
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
    URES_INT_ويڪٽر,
    URES_NONE
};

U_CAPI UResType U_EXPORT2
res_get_عوامي_Type(Resource res) {
    أرجع (UResType)g_عوامي_Types[RES_GET_TYPE(res)];
}

U_CAPI ثابت char16_t * U_EXPORT2
res_getStringNoTrace(ثابت Resource_معلومات *p_Res_معلومات, Resource res, int32_t *p_ڊيگهائي) {
    ثابت char16_t *p;
    uint32_t وٿي=RES_GET_وٿي(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        إذا((int32_t)وٿي<p_Res_معلومات->pool_String_اشاريو_حد_بندي) {
            p=(ثابت char16_t *)p_Res_معلومات->pool_Bundle_لڙيون+وٿي;
        } وإلا {
            p=(ثابت char16_t *)p_Res_معلومات->p16_ٻٽو_انگ_Units+(وٿي-p_Res_معلومات->pool_String_اشاريو_حد_بندي);
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
    } وإلا إذا(res==وٿي) /* RES_GET_TYPE(res)==URES_STRING */ {
        ثابت int32_t *p32= res==0 ? &g_خالي_جايون_String.length : p_Res_معلومات->p_پاڙ+res;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=خالي_اشارو;
        length=0;
    }
    إذا(p_ڊيگهائي) {
        *p_ڊيگهائي=length;
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
    uint32_t وٿي=RES_GET_وٿي(res);
    إذا (وٿي == 0) {
        // empty string
    } وإلا إذا (res == وٿي) {
        ثابت int32_t *p32=p_Res_معلومات->p_پاڙ+res;
        int32_t length=*p32;
        ثابت char16_t* p = reinterpret_cast<ثابت char16_t*>(p32);
        أرجع length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } وإلا إذا (RES_GET_TYPE(res) == URES_STRING_V2) {
        ثابت char16_t *p;
        إذا (static_cast<int32_t>(وٿي) < p_Res_معلومات->pool_String_اشاريو_حد_بندي) {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_معلومات->pool_Bundle_لڙيون) + وٿي;
        } وإلا {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_معلومات->p16_ٻٽو_انگ_Units) + (وٿي - p_Res_معلومات->pool_String_اشاريو_حد_بندي);
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
                       UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(dest == خالي_اشارو ? capacity != 0 : capacity < 0) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    int32_t length = array.get_سائيز();
    إذا(length == 0) {
        أرجع 0;
    }
    إذا(length > capacity) {
        error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
        أرجع length;
    }
    لكل(int32_t i = 0; i < length; ++i) {
        int32_t s_ڊيگهائي;
        // No tracing: handled by the caller
        ثابت char16_t *s = res_getStringNoTrace(p_Res_معلومات, array.اندروني_Get_Resource(p_Res_معلومات, i), &s_ڊيگهائي);
        إذا(s == خالي_اشارو) {
            error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
            أرجع 0;
        }
        dest[i].سيٽ_منزل(سچ, s, s_ڊيگهائي);
    }
    أرجع length;
}

}  // namespace

U_CAPI ثابت char16_t * U_EXPORT2
res_getAlias(ثابت Resource_معلومات *p_Res_معلومات, Resource res, int32_t *p_ڊيگهائي) {
    ثابت char16_t *p;
    uint32_t وٿي=RES_GET_وٿي(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_ALIAS) {
        ثابت int32_t *p32= وٿي==0 ? &g_خالي_جايون_String.length : p_Res_معلومات->p_پاڙ+وٿي;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } وإلا {
        p=خالي_اشارو;
        length=0;
    }
    إذا(p_ڊيگهائي) {
        *p_ڊيگهائي=length;
    }
    أرجع p;
}

U_CAPI ثابت uint8_t * U_EXPORT2
res_get_ٻٽو_No_Trace(ثابت Resource_معلومات *p_Res_معلومات, Resource res, int32_t *p_ڊيگهائي) {
    ثابت uint8_t *p;
    uint32_t وٿي=RES_GET_وٿي(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_ٻٽو) {
        ثابت int32_t *p32= وٿي==0 ? (ثابت int32_t*)&gEmpty32 : p_Res_معلومات->p_پاڙ+وٿي;
        length=*p32++;
        p=(ثابت uint8_t *)p32;
    } وإلا {
        p=خالي_اشارو;
        length=0;
    }
    إذا(p_ڊيگهائي) {
        *p_ڊيگهائي=length;
    }
    أرجع p;
}


U_CAPI ثابت int32_t * U_EXPORT2
res_get_Int_ويڪٽر_No_Trace(ثابت Resource_معلومات *p_Res_معلومات, Resource res, int32_t *p_ڊيگهائي) {
    ثابت int32_t *p;
    uint32_t وٿي=RES_GET_وٿي(res);
    int32_t length;
    إذا(RES_GET_TYPE(res)==URES_INT_ويڪٽر) {
        p= وٿي==0 ? (ثابت int32_t *)&gEmpty32 : p_Res_معلومات->p_پاڙ+وٿي;
        length=*p++;
    } وإلا {
        p=خالي_اشارو;
        length=0;
    }
    إذا(p_ڊيگهائي) {
        *p_ڊيگهائي=length;
    }
    أرجع p;
}

U_CAPI int32_t U_EXPORT2
res_ڳڻپ_Array_شيون(ثابت Resource_معلومات *p_Res_معلومات, Resource res) {
    uint32_t وٿي=RES_GET_وٿي(res);
    بدل(RES_GET_TYPE(res)) {
    حالة URES_STRING:
    حالة URES_STRING_V2:
    حالة URES_ٻٽو:
    حالة URES_ALIAS:
    حالة URES_INT:
    حالة URES_INT_ويڪٽر:
        أرجع 1;
    حالة URES_ARRAY:
    حالة URES_TABLE32:
        أرجع وٿي==0 ? 0 : *(p_Res_معلومات->p_پاڙ+وٿي);
    حالة URES_جدول:
        أرجع وٿي==0 ? 0 : *((ثابت uint16_t *)(p_Res_معلومات->p_پاڙ+وٿي));
    حالة URES_ARRAY16:
    حالة URES_TABLE16:
        أرجع p_Res_معلومات->p16_ٻٽو_انگ_Units[وٿي];
    افتراضي:
        أرجع 0;
    }
}

U_NAMESPACE_BEGIN

Resource_معلومات_قدر::~Resource_معلومات_قدر() {}

UResType Resource_معلومات_قدر::getType() ثابت {
    أرجع res_get_عوامي_Type(res);
}

ثابت char16_t *Resource_معلومات_قدر::getString(int32_t &length, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    ثابت char16_t *s = res_getString(fTraceInfo, &get_معلومات(), res, &length);
    إذا(s == خالي_اشارو) {
        error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

ثابت char16_t *Resource_معلومات_قدر::getAliasString(int32_t &length, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    ثابت char16_t *s = res_getAlias(&get_معلومات(), res, &length);
    إذا(s == خالي_اشارو) {
        error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع s;
}

int32_t Resource_معلومات_قدر::getInt(UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getInt(fTraceInfo, res);
}

uint32_t Resource_معلومات_قدر::getUInt(UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(RES_GET_TYPE(res) != URES_INT) {
        error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع res_getUInt(fTraceInfo, res);
}

ثابت int32_t *Resource_معلومات_قدر::get_Int_ويڪٽر(int32_t &length, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    ثابت int32_t *iv = res_get_Int_ويڪٽر(fTraceInfo, &get_معلومات(), res, &length);
    إذا(iv == خالي_اشارو) {
        error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع iv;
}

ثابت uint8_t *Resource_معلومات_قدر::get_ٻٽو(int32_t &length, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    ثابت uint8_t *b = res_get_ٻٽو(fTraceInfo, &get_معلومات(), res, &length);
    إذا(b == خالي_اشارو) {
        error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
    }
    أرجع b;
}

ResourceArray Resource_معلومات_قدر::getArray(UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع {};
    }
    ثابت uint16_t *items16 = خالي_اشارو;
    ثابت Resource *items32 = خالي_اشارو;
    uint32_t وٿي=RES_GET_وٿي(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_ARRAY:
        إذا (وٿي!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ثابت Resource*>(get_معلومات().p_پاڙ) + وٿي;
            length = *items32++;
        }
        اكسر;
    حالة URES_ARRAY16:
        items16 = get_معلومات().p16_ٻٽو_انگ_Units+وٿي;
        length = *items16++;
        اكسر;
    افتراضي:
        error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع ResourceArray(items16, items32, length, fTraceInfo);
}

Resource_جدول Resource_معلومات_قدر::get_جدول(UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع {};
    }
    ثابت uint16_t *keys16 = خالي_اشارو;
    ثابت int32_t *keys32 = خالي_اشارو;
    ثابت uint16_t *items16 = خالي_اشارو;
    ثابت Resource *items32 = خالي_اشارو;
    uint32_t وٿي = RES_GET_وٿي(res);
    int32_t length = 0;
    بدل(RES_GET_TYPE(res)) {
    حالة URES_جدول:
        إذا (وٿي != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ثابت uint16_t*>(get_معلومات().p_پاڙ + وٿي);
            length = *keys16++;
            items32 = reinterpret_cast<ثابت Resource*>(keys16 + length + (~length & 1));
        }
        اكسر;
    حالة URES_TABLE16:
        keys16 = get_معلومات().p16_ٻٽو_انگ_Units+وٿي;
        length = *keys16++;
        items16 = keys16 + length;
        اكسر;
    حالة URES_TABLE32:
        إذا (وٿي != 0) {  // empty if offset==0
            keys32 = get_معلومات().p_پاڙ+وٿي;
            length = *keys32++;
            items32 = reinterpret_cast<ثابت Resource*>(keys32) + length;
        }
        اكسر;
    افتراضي:
        error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
        أرجع {};
    }
    أرجع Resource_جدول(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_معلومات_قدر::isNoInheritanceMarker() ثابت {
    أرجع ::isNoInheritanceMarker(&get_معلومات(), res);
}

int32_t Resource_معلومات_قدر::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UError_ڪوڊ &error_ڪوڊ) ثابت {
    أرجع ::getStringArray(&get_معلومات(), getArray(error_ڪوڊ), dest, capacity, error_ڪوڊ);
}

int32_t Resource_معلومات_قدر::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا(URES_IS_ARRAY(res)) {
        أرجع ::getStringArray(&get_معلومات(), getArray(error_ڪوڊ), dest, capacity, error_ڪوڊ);
    }
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(dest == خالي_اشارو ? capacity != 0 : capacity < 0) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    إذا(capacity < 1) {
        error_ڪوڊ = U_بفر_حد_کان_مٿي_ERROR;
        أرجع 1;
    }
    int32_t s_ڊيگهائي;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_معلومات(), res, &s_ڊيگهائي);
    إذا(s != خالي_اشارو) {
        dest[0].سيٽ_منزل(سچ, s, s_ڊيگهائي);
        أرجع 1;
    }
    error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
    أرجع 0;
}

UnicodeString Resource_معلومات_قدر::get_String_Or_پهريون_جو_Array(UError_ڪوڊ &error_ڪوڊ) ثابت {
    UnicodeString us;
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع us;
    }
    int32_t s_ڊيگهائي;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_معلومات(), res, &s_ڊيگهائي);
    إذا(s != خالي_اشارو) {
        us.سيٽ_منزل(سچ, s, s_ڊيگهائي);
        أرجع us;
    }
    ResourceArray array = getArray(error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع us;
    }
    إذا(array.get_سائيز() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_معلومات(), array.اندروني_Get_Resource(&get_معلومات(), 0), &s_ڊيگهائي);
        إذا(s != خالي_اشارو) {
            us.سيٽ_منزل(سچ, s, s_ڊيگهائي);
            أرجع us;
        }
    }
    error_ڪوڊ = U_RESOURCE_TYPE_MISMATCH;
    أرجع us;
}

U_NAMESPACE_پڇاڙي

ساكن Resource
makeResourceFrom16(ثابت Resource_معلومات *p_Res_معلومات, int32_t res16) {
    إذا(res16<p_Res_معلومات->pool_String_Index16_حد_بندي) {
        // Pool string, nothing to do.
    } وإلا {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_معلومات->pool_String_Index16_حد_بندي+p_Res_معلومات->pool_String_اشاريو_حد_بندي;
    }
    أرجع URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_جدول_شي_By_ڪنجي(ثابت Resource_معلومات *p_Res_معلومات, Resource جدول,
                      int32_t *اشاريو_R, ثابت حرف **ڪنجي) {
    uint32_t وٿي=RES_GET_وٿي(جدول);
    int32_t length;
    int32_t idx;
    إذا(ڪنجي == خالي_اشارو || *ڪنجي == خالي_اشارو) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(جدول)) {
    حالة URES_جدول: {
        إذا (وٿي!=0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_معلومات->p_پاڙ+وٿي);
            length=*p++;
            *اشاريو_R=idx=res_ڳوليو_جدول_شي(p_Res_معلومات, p, length, *ڪنجي, ڪنجي);
            إذا(idx>=0) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                أرجع p32[idx];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=p_Res_معلومات->p16_ٻٽو_انگ_Units+وٿي;
        length=*p++;
        *اشاريو_R=idx=res_ڳوليو_جدول_شي(p_Res_معلومات, p, length, *ڪنجي, ڪنجي);
        إذا(idx>=0) {
            أرجع makeResourceFrom16(p_Res_معلومات, p[length+idx]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (وٿي!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_معلومات->p_پاڙ+وٿي;
            length=*p++;
            *اشاريو_R=idx=res_ڳوليو_Table32_شي(p_Res_معلومات, p, length, *ڪنجي, ڪنجي);
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
res_get_جدول_شي_By_اشاريو(ثابت Resource_معلومات *p_Res_معلومات, Resource جدول,
                        int32_t اشاريو_R, ثابت حرف **ڪنجي) {
    uint32_t وٿي=RES_GET_وٿي(جدول);
    int32_t length;
    إذا (اشاريو_R < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(جدول)) {
    حالة URES_جدول: {
        إذا (وٿي != 0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_معلومات->p_پاڙ+وٿي);
            length=*p++;
            إذا(اشاريو_R<length) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                إذا(ڪنجي!=خالي_اشارو) {
                    *ڪنجي=RES_GET_KEY16(p_Res_معلومات, p[اشاريو_R]);
                }
                أرجع p32[اشاريو_R];
            }
        }
        اكسر;
    }
    حالة URES_TABLE16: {
        ثابت uint16_t *p=p_Res_معلومات->p16_ٻٽو_انگ_Units+وٿي;
        length=*p++;
        إذا(اشاريو_R<length) {
            إذا(ڪنجي!=خالي_اشارو) {
                *ڪنجي=RES_GET_KEY16(p_Res_معلومات, p[اشاريو_R]);
            }
            أرجع makeResourceFrom16(p_Res_معلومات, p[length+اشاريو_R]);
        }
        اكسر;
    }
    حالة URES_TABLE32: {
        إذا (وٿي != 0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_معلومات->p_پاڙ+وٿي;
            length=*p++;
            إذا(اشاريو_R<length) {
                إذا(ڪنجي!=خالي_اشارو) {
                    *ڪنجي=RES_GET_KEY32(p_Res_معلومات, p[اشاريو_R]);
                }
                أرجع (Resource)p[length+اشاريو_R];
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
res_getResource(ثابت Resource_معلومات *p_Res_معلومات, ثابت حرف *ڪنجي) {
    ثابت حرف *real_ڪنجي=ڪنجي;
    int32_t idx;
    أرجع res_get_جدول_شي_By_ڪنجي(p_Res_معلومات, p_Res_معلومات->پاڙ_Res, &idx, &real_ڪنجي);
}


UBool icu::Resource_جدول::get_ڪنجي_And_قدر(int32_t i,
                                         ثابت حرف *&ڪنجي, icu::Resource_قدر &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_معلومات_قدر &rd_قدر = static_cast<icu::Resource_معلومات_قدر &>(value);
        إذا (keys16 != خالي_اشارو) {
            ڪنجي = RES_GET_KEY16(&rd_قدر.get_معلومات(), keys16[i]);
        } وإلا {
            ڪنجي = RES_GET_KEY32(&rd_قدر.get_معلومات(), keys32[i]);
        }
        Resource res;
        إذا (items16 != خالي_اشارو) {
            res = makeResourceFrom16(&rd_قدر.get_معلومات(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_قدر.سيٽ_Resource(res, ResourceTracer(fTraceInfo, ڪنجي));
        أرجع سچ;
    }
    أرجع خطأ;
}

UBool icu::Resource_جدول::ڳوليو_قدر(ثابت حرف *ڪنجي, Resource_قدر &value) ثابت {
    icu::Resource_معلومات_قدر &rd_قدر = static_cast<icu::Resource_معلومات_قدر &>(value);
    ثابت حرف *real_ڪنجي = خالي_اشارو;
    int32_t i;
    إذا (keys16 != خالي_اشارو) {
        i = res_ڳوليو_جدول_شي(&rd_قدر.get_معلومات(), keys16, length, ڪنجي, &real_ڪنجي);
    } وإلا {
        i = res_ڳوليو_Table32_شي(&rd_قدر.get_معلومات(), keys32, length, ڪنجي, &real_ڪنجي);
    }
    إذا (i >= 0) {
        Resource res;
        إذا (items16 != خالي_اشارو) {
            res = makeResourceFrom16(&rd_قدر.get_معلومات(), items16[i]);
        } وإلا {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_قدر.سيٽ_Resource(res, ResourceTracer(fTraceInfo, ڪنجي));
        أرجع سچ;
    }
    أرجع خطأ;
}

U_CAPI Resource U_EXPORT2
res_get_Array_شي(ثابت Resource_معلومات *p_Res_معلومات, Resource array, int32_t اشاريو_R) {
    uint32_t وٿي=RES_GET_وٿي(array);
    إذا (اشاريو_R < 0) {
        أرجع RES_BOGUS;
    }
    بدل(RES_GET_TYPE(array)) {
    حالة URES_ARRAY: {
        إذا (وٿي!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_معلومات->p_پاڙ+وٿي;
            إذا(اشاريو_R<*p) {
                أرجع (Resource)p[1+اشاريو_R];
            }
        }
        اكسر;
    }
    حالة URES_ARRAY16: {
        ثابت uint16_t *p=p_Res_معلومات->p16_ٻٽو_انگ_Units+وٿي;
        إذا(اشاريو_R<*p) {
            أرجع makeResourceFrom16(p_Res_معلومات, p[1+اشاريو_R]);
        }
        اكسر;
    }
    افتراضي:
        اكسر;
    }
    أرجع RES_BOGUS;
}

uint32_t icu::ResourceArray::اندروني_Get_Resource(ثابت Resource_معلومات *p_Res_معلومات, int32_t i) ثابت {
    إذا (items16 != خالي_اشارو) {
        أرجع makeResourceFrom16(p_Res_معلومات, items16[i]);
    } وإلا {
        أرجع items32[i];
    }
}

UBool icu::ResourceArray::get_قدر(int32_t i, icu::Resource_قدر &value) ثابت {
    إذا(0 <= i && i < length) {
        icu::Resource_معلومات_قدر &rd_قدر = static_cast<icu::Resource_معلومات_قدر &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_قدر.سيٽ_Resource(
            اندروني_Get_Resource(&rd_قدر.get_معلومات(), i),
            ResourceTracer(fTraceInfo, i));
        أرجع سچ;
    }
    أرجع خطأ;
}

U_CFUNC Resource
res_ڳوليو_Resource(ثابت Resource_معلومات *p_Res_معلومات, Resource r, حرف** رستو, ثابت حرف** ڪنجي) {
  حرف *رستو_P = *رستو, *ايندڙ_Sep_P = *رستو;
  حرف *close_اشاريو = خالي_اشارو;
  Resource t1 = r;
  Resource t2;
  int32_t اشاريو_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  إذا(!uprv_strlen(رستو_P)) {
      أرجع r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  إذا(!URES_IS_CONTAINER(type)) {
      أرجع RES_BOGUS;
  }
  
  طالما(ايندڙ_Sep_P && *رستو_P && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    ايندڙ_Sep_P = uprv_strchr(رستو_P, RES_رستو_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    إذا(ايندڙ_Sep_P != خالي_اشارو) {
      إذا(ايندڙ_Sep_P == رستو_P) {
        // Empty key string.
        أرجع RES_BOGUS;
      }
      *ايندڙ_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *رستو = ايندڙ_Sep_P+1;
    } وإلا {
      *رستو = uprv_strchr(رستو_P, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    إذا(URES_IS_جدول(type)) {
      *ڪنجي = رستو_P;
      t2 = res_get_جدول_شي_By_ڪنجي(p_Res_معلومات, t1, &اشاريو_R, ڪنجي);
    } وإلا إذا(URES_IS_ARRAY(type)) {
      اشاريو_R = uprv_strtol(رستو_P, &close_اشاريو, 10);
      إذا(اشاريو_R >= 0 && *close_اشاريو == 0) {
        t2 = res_get_Array_شي(p_Res_معلومات, t1, اشاريو_R);
      } وإلا {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *ڪنجي = خالي_اشارو;
    } وإلا { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    رستو_P = *رستو;
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
    int32_t ڪنجي_اشاريو, ترتيب_ڏيو_اشاريو;
} صف_2;

ساكن int32_t U_CALLCONV
ures_compare_صفون(ثابت فراغ *context, ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    ثابت حرف* ڪنجي_Chars = static_cast<ثابت حرف*>(context);
    أرجع static_cast<int32_t>(uprv_strcmp(ڪنجي_Chars + static_cast<ثابت صف_2*>(کاٻي)->ڪنجي_اشاريو,
                                            ڪنجي_Chars + static_cast<ثابت صف_2*>(ساڄي)->ڪنجي_اشاريو));
}

تعريفنوع بنية عارضي_جدول_2 {
    ثابت حرف *ڪنجي_Chars;
    صف_2 *صفون;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_ڪنجي_حد_بندي;
    uint8_t major_Format_نسخو;
} عارضي_جدول_2;

تعداد {
    ڍير_صف_صلاحيت=200
};

/* The table item key string is not locally available. */
ساكن ثابت حرف *ثابت g_Unknown_ڪنجي="";

#إذا !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ساكن ثابت char16_t g_Collation_Bin_ڪنجي[]=u"%%CollationBin";
#جيڪڏهن_پورو

/*
 * swap one resource item
 */
ساكن فراغ
ures_مٽا_سٽا_Resource(ثابت UDataSwapper *ds,
                  ثابت Resource *اندريون_Bundle, Resource *ٻاهريون_Bundle,
                  Resource res, /* caller swaps res itself */
                  ثابت حرف *ڪنجي,
                  عارضي_جدول_2 *p_عارضي_جدول,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت Resource *p;
    Resource *q;
    int32_t وٿي, count;

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
    وٿي = static_cast<int32_t>(RES_GET_وٿي(res));
    إذا(وٿي==0) {
        /* special offset indicating an empty item */
        أرجع;
    }
    إذا (p_عارضي_جدول->resFlags[وٿي >> 5] & (static_cast<uint32_t>(1) << (وٿي & 0x1f))) {
        /* we already swapped this resource item */
        أرجع;
    } وإلا {
        /* mark it as swapped now */
        p_عارضي_جدول->resFlags[وٿي >> 5] |= static_cast<uint32_t>(1) << (وٿي & 0x1f);
    }

    p=اندريون_Bundle+وٿي;
    q=ٻاهريون_Bundle+وٿي;

    بدل(RES_GET_TYPE(res)) {
    حالة URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    حالة URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->مٽا_سٽا_Array32(ds, p, 4, q, p_Error_ڪوڊ);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->مٽا_سٽا_Array16(ds, p+1, 2*count, q+1, p_Error_ڪوڊ);
        اكسر;
    حالة URES_ٻٽو:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->مٽا_سٽا_Array32(ds, p, 4, q, p_Error_ڪوڊ);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#إذا !UCONFIG_NO_COLLATION
        إذا( ڪنجي!=خالي_اشارو &&  /* the binary is in a table */
            (ڪنجي!=g_Unknown_ڪنجي ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, ڪنجي, -1,
                                       g_Collation_Bin_ڪنجي, UPRV_LENGTHOF(g_Collation_Bin_ڪنجي)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_ٻٽو(ds, p+1, count))
        ) {
            ucol_مٽا_سٽا(ds, p+1, count, q+1, p_Error_ڪوڊ);
        }
#جيڪڏهن_پورو
        اكسر;
    حالة URES_جدول:
    حالة URES_TABLE32:
        {
            ثابت uint16_t *pKey16;
            uint16_t *qKey16;

            ثابت int32_t *pKey32;
            int32_t *qKey32;

            Resource شي;
            int32_t i, old_اشاريو;

            إذا(RES_GET_TYPE(res)==URES_جدول) {
                /* get table item count */
                pKey16 = reinterpret_cast<ثابت uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=خالي_اشارو;

                /* swap count */
                ds->مٽا_سٽا_Array16(ds, pKey16++, 2, qKey16++, p_Error_ڪوڊ);

                وٿي+=((1+count)+1)/2;
            } وإلا {
                /* get table item count */
                pKey32 = reinterpret_cast<ثابت int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=خالي_اشارو;

                /* swap count */
                ds->مٽا_سٽا_Array32(ds, pKey32++, 4, qKey32++, p_Error_ڪوڊ);

                وٿي+=1+count;
            }

            إذا(count==0) {
                اكسر;
            }

            p=اندريون_Bundle+وٿي; /* pointer to table resources */
            q=ٻاهريون_Bundle+وٿي;

            /* recurse */
            لكل(i=0; i<count; ++i) {
                ثابت حرف *شي_ڪنجي=g_Unknown_ڪنجي;
                إذا(pKey16!=خالي_اشارو) {
                    int32_t ڪنجي_وٿي=ds->readUInt16(pKey16[i]);
                    إذا(ڪنجي_وٿي<p_عارضي_جدول->local_ڪنجي_حد_بندي) {
                        شي_ڪنجي = reinterpret_cast<ثابت حرف*>(ٻاهريون_Bundle) + ڪنجي_وٿي;
                    }
                } وإلا {
                    int32_t ڪنجي_وٿي=udata_readInt32(ds, pKey32[i]);
                    إذا(ڪنجي_وٿي>=0) {
                        شي_ڪنجي = reinterpret_cast<ثابت حرف*>(ٻاهريون_Bundle) + ڪنجي_وٿي;
                    }
                }
                شي=ds->readUInt32(p[i]);
                ures_مٽا_سٽا_Resource(ds, اندريون_Bundle, ٻاهريون_Bundle, شي, شي_ڪنجي, p_عارضي_جدول, p_Error_ڪوڊ);
                إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, شي);
                    أرجع;
                }
            }

            إذا(p_عارضي_جدول->major_Format_نسخو>1 || ds->اندريون_Charset==ds->ٻاهريون_Charset) {
                /* no need to sort, just swap the offset/value arrays */
                إذا(pKey16!=خالي_اشارو) {
                    ds->مٽا_سٽا_Array16(ds, pKey16, count*2, qKey16, p_Error_ڪوڊ);
                    ds->مٽا_سٽا_Array32(ds, p, count*4, q, p_Error_ڪوڊ);
                } وإلا {
                    /* swap key offsets and items as one array */
                    ds->مٽا_سٽا_Array32(ds, pKey32, count*2*4, qKey32, p_Error_ڪوڊ);
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
            إذا(pKey16!=خالي_اشارو) {
                لكل(i=0; i<count; ++i) {
                    p_عارضي_جدول->صفون[i].ڪنجي_اشاريو=ds->readUInt16(pKey16[i]);
                    p_عارضي_جدول->صفون[i].ترتيب_ڏيو_اشاريو=i;
                }
            } وإلا {
                لكل(i=0; i<count; ++i) {
                    p_عارضي_جدول->صفون[i].ڪنجي_اشاريو=udata_readInt32(ds, pKey32[i]);
                    p_عارضي_جدول->صفون[i].ترتيب_ڏيو_اشاريو=i;
                }
            }
            uprv_ترتيب_ڏيو_Array(p_عارضي_جدول->صفون, count, حجم(صف_2),
                           ures_compare_صفون, p_عارضي_جدول->ڪنجي_Chars,
                           خطأ, p_Error_ڪوڊ);
            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
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
            /*ڪنجيون*/
            إذا(pKey16!=خالي_اشارو) {
                uint16_t *rKey16;

                إذا(pKey16!=qKey16) {
                    rKey16=qKey16;
                } وإلا {
                    rKey16 = reinterpret_cast<uint16_t*>(p_عارضي_جدول->resort);
                }
                لكل(i=0; i<count; ++i) {
                    old_اشاريو=p_عارضي_جدول->صفون[i].ترتيب_ڏيو_اشاريو;
                    ds->مٽا_سٽا_Array16(ds, pKey16+old_اشاريو, 2, rKey16+i, p_Error_ڪوڊ);
                }
                إذا(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } وإلا {
                int32_t *rKey32;

                إذا(pKey32!=qKey32) {
                    rKey32=qKey32;
                } وإلا {
                    rKey32=p_عارضي_جدول->resort;
                }
                لكل(i=0; i<count; ++i) {
                    old_اشاريو=p_عارضي_جدول->صفون[i].ترتيب_ڏيو_اشاريو;
                    ds->مٽا_سٽا_Array32(ds, pKey32+old_اشاريو, 4, rKey32+i, p_Error_ڪوڊ);
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
                    r = reinterpret_cast<Resource*>(p_عارضي_جدول->resort);
                }
                لكل(i=0; i<count; ++i) {
                    old_اشاريو=p_عارضي_جدول->صفون[i].ترتيب_ڏيو_اشاريو;
                    ds->مٽا_سٽا_Array32(ds, p+old_اشاريو, 4, r+i, p_Error_ڪوڊ);
                }
                إذا(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        اكسر;
    حالة URES_ARRAY:
        {
            Resource شي;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->مٽا_سٽا_Array32(ds, p++, 4, q++, p_Error_ڪوڊ);

            /* recurse */
            لكل(i=0; i<count; ++i) {
                شي=ds->readUInt32(p[i]);
                ures_مٽا_سٽا_Resource(ds, اندريون_Bundle, ٻاهريون_Bundle, شي, خالي_اشارو, p_عارضي_جدول, p_Error_ڪوڊ);
                إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, شي);
                    أرجع;
                }
            }

            /* swap items */
            ds->مٽا_سٽا_Array32(ds, p, 4*count, q, p_Error_ڪوڊ);
        }
        اكسر;
    حالة URES_INT_ويڪٽر:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->مٽا_سٽا_Array32(ds, p, 4*(1+count), q, p_Error_ڪوڊ);
        اكسر;
    افتراضي:
        /* also catches RES_BOGUS */
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        اكسر;
    }
}

U_CAPI int32_t U_EXPORT2
ures_مٽا_سٽا(ثابت UDataSwapper *ds,
          ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
          UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UDataInfo *pInfo;
    ثابت Resource *اندريون_Bundle;
    Resource پاڙ_Res;
    int32_t سرو_سائيز, max_جدول_ڊيگهائي;

    صف_2 صفون[ڍير_صف_صلاحيت];
    int32_t resort[ڍير_صف_صلاحيت];
    عارضي_جدول_2 عارضي_جدول;

    ثابت int32_t *اندريون_Indexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_ڊيگهائي, اشاريو_ڊيگهائي, ڪنجيون_Bottom, ڪنجيون_چوٽي, resBottom, چوٽي;

    /* udata_swapDataHeader checks the arguments */
    سرو_سائيز=udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->معلومات_Format[1]==0x65 &&
        pInfo->معلومات_Format[2]==0x73 &&
        pInfo->معلومات_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->format_نسخو[0]==1 && pInfo->format_نسخو[1]>=1) ||
            pInfo->format_نسخو[0]==2 || pInfo->format_نسخو[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخو[0], pInfo->format_نسخو[1]);
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }
    عارضي_جدول.major_Format_نسخو=pInfo->format_نسخو[0];

    /* a resource bundle must contain at least one resource item */
    إذا(length<0) {
        bundle_ڊيگهائي=-1;
    } وإلا {
        bundle_ڊيگهائي=(length-سرو_سائيز)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        إذا(bundle_ڊيگهائي<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-سرو_سائيز);
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    اندريون_Bundle=(ثابت Resource *)((ثابت حرف *)اندريون_معلومات+سرو_سائيز);
    پاڙ_Res=ds->readUInt32(*اندريون_Bundle);

    /* formatVersion 1.1 adds the indexes[] array */
    اندريون_Indexes=(ثابت int32_t *)(اندريون_Bundle+1);

    اشاريو_ڊيگهائي=udata_readInt32(ds, اندريون_Indexes[URES_اشاريو_ڊيگهائي])&0xff;
    إذا(اشاريو_ڊيگهائي<=URES_اشاريو_MAX_جدول_ڊيگهائي) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    ڪنجيون_Bottom=1+اشاريو_ڊيگهائي;
    ڪنجيون_چوٽي=udata_readInt32(ds, اندريون_Indexes[URES_اشاريو_ڪنجيون_چوٽي]);
    إذا(اشاريو_ڊيگهائي>URES_اشاريو_16_ٻٽو_انگ_چوٽي) {
        resBottom=udata_readInt32(ds, اندريون_Indexes[URES_اشاريو_16_ٻٽو_انگ_چوٽي]);
    } وإلا {
        resBottom=ڪنجيون_چوٽي;
    }
    چوٽي=udata_readInt32(ds, اندريون_Indexes[URES_اشاريو_BUNDLE_چوٽي]);
    max_جدول_ڊيگهائي=udata_readInt32(ds, اندريون_Indexes[URES_اشاريو_MAX_جدول_ڊيگهائي]);

    إذا(0<=bundle_ڊيگهائي && bundle_ڊيگهائي<چوٽي) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         چوٽي, bundle_ڊيگهائي);
        *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    إذا(ڪنجيون_چوٽي>(1+اشاريو_ڊيگهائي)) {
        عارضي_جدول.local_ڪنجي_حد_بندي=ڪنجيون_چوٽي<<2;
    } وإلا {
        عارضي_جدول.local_ڪنجي_حد_بندي=0;
    }

    إذا(length>=0) {
        Resource *ٻاهريون_Bundle=(Resource *)((حرف *)ٻاهريون_معلومات+سرو_سائيز);

        /* track which resources we have already swapped */
        uint32_t ڍير_Res_Flags[ڍير_صف_صلاحيت];
        int32_t res_Flags_ڊيگهائي;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_ڊيگهائي=(length+31)>>5;          /* number of bytes needed */
        res_Flags_ڊيگهائي=(res_Flags_ڊيگهائي+3)&~3;   /* multiple of 4 bytes for uint32_t */
        إذا(res_Flags_ڊيگهائي<=(int32_t)حجم(ڍير_Res_Flags)) {
            عارضي_جدول.resFlags=ڍير_Res_Flags;
        } وإلا {
            عارضي_جدول.resFlags=(uint32_t *)uprv_malloc(res_Flags_ڊيگهائي);
            إذا(عارضي_جدول.resFlags==خالي_اشارو) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                أرجع 0;
            }
        }
        uprv_memset(عارضي_جدول.resFlags, 0, res_Flags_ڊيگهائي);

        /* copy the bundle for binary and inaccessible data */
        إذا(اندريون_معلومات!=ٻاهريون_معلومات) {
            uprv_memcpy(ٻاهريون_Bundle, اندريون_Bundle, 4*چوٽي);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_مٽا_سٽا_Inv_String_بلاڪ(ds, اندريون_Bundle+ڪنجيون_Bottom, 4*(ڪنجيون_چوٽي-ڪنجيون_Bottom),
                                    ٻاهريون_Bundle+ڪنجيون_Bottom, p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(ڪنجيون_چوٽي-ڪنجيون_Bottom));
            إذا(عارضي_جدول.resFlags!=ڍير_Res_Flags) {
                uprv_free(عارضي_جدول.resFlags);
            }
            أرجع 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        إذا(ڪنجيون_چوٽي<resBottom) {
            ds->مٽا_سٽا_Array16(ds, اندريون_Bundle+ڪنجيون_چوٽي, (resBottom-ڪنجيون_چوٽي)*4, ٻاهريون_Bundle+ڪنجيون_چوٽي, p_Error_ڪوڊ);
            إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-ڪنجيون_چوٽي));
                إذا(عارضي_جدول.resFlags!=ڍير_Res_Flags) {
                    uprv_free(عارضي_جدول.resFlags);
                }
                أرجع 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        عارضي_جدول.ڪنجي_Chars=(ثابت حرف *)ٻاهريون_Bundle; /* sort by outCharset */
        إذا(عارضي_جدول.major_Format_نسخو>1 || max_جدول_ڊيگهائي<=ڍير_صف_صلاحيت) {
            عارضي_جدول.صفون=صفون;
            عارضي_جدول.resort=resort;
        } وإلا {
            عارضي_جدول.صفون=(صف_2 *)uprv_malloc(max_جدول_ڊيگهائي*حجم(صف_2)+max_جدول_ڊيگهائي*4);
            إذا(عارضي_جدول.صفون==خالي_اشارو) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_جدول_ڊيگهائي);
                *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                إذا(عارضي_جدول.resFlags!=ڍير_Res_Flags) {
                    uprv_free(عارضي_جدول.resFlags);
                }
                أرجع 0;
            }
            عارضي_جدول.resort=(int32_t *)(عارضي_جدول.صفون+max_جدول_ڊيگهائي);
        }

        /* swap the resources */
        ures_مٽا_سٽا_Resource(ds, اندريون_Bundle, ٻاهريون_Bundle, پاڙ_Res, خالي_اشارو, &عارضي_جدول, p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             پاڙ_Res);
        }

        إذا(عارضي_جدول.صفون!=صفون) {
            uprv_free(عارضي_جدول.صفون);
        }
        إذا(عارضي_جدول.resFlags!=ڍير_Res_Flags) {
            uprv_free(عارضي_جدول.resFlags);
        }

        /* swap the root resource and indexes */
        ds->مٽا_سٽا_Array32(ds, اندريون_Bundle, ڪنجيون_Bottom*4, ٻاهريون_Bundle, p_Error_ڪوڊ);
    }

    أرجع سرو_سائيز+4*چوٽي;
}

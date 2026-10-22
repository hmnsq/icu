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

#شامل "unicode/utypes.h"
#شامل "unicode/udata.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "resource.h"
#شامل "uarrsort.h"
#شامل "uassert.h"
#شامل "ucol_swp.h"
#شامل "udataswp.h"
#شامل "uinvchar.h"
#شامل "uresdata.h"
#شامل "uresimp.h"
#شامل "utracimp.h"

/*
 * Resource access helpers
 */

/* get a const char* pointer to the key with the keyOffset byte offset from pRoot */
#تعریف_کرو RES_GET_KEY16(p_Res_ڈیٹا, کلید_انحراف) \
    ((کلید_انحراف)<(p_Res_ڈیٹا)->local_کلید_حد_بندی ? \
        (ثابت حرف *)(p_Res_ڈیٹا)->p_جڑ+(کلید_انحراف) : \
        (p_Res_ڈیٹا)->pool_Bundle_کلیدیں+(کلید_انحراف)-(p_Res_ڈیٹا)->local_کلید_حد_بندی)

#تعریف_کرو RES_GET_KEY32(p_Res_ڈیٹا, کلید_انحراف) \
    ((کلید_انحراف)>=0 ? \
        (ثابت حرف *)(p_Res_ڈیٹا)->p_جڑ+(کلید_انحراف) : \
        (p_Res_ڈیٹا)->pool_Bundle_کلیدیں+((کلید_انحراف)&0x7fffffff))

#تعریف_کرو URESDATA_شے_NOT_ملا -1

/* empty resources, returned when the resource offset is 0 */
ساکن ثابت uint16_t gEmpty16=0;

ساکن ثابت ساخت {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

ساکن ثابت ساخت {
    int32_t length;
    char16_t nul;
    char16_t pad;
} g_خالی_خانے_String={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ساکن int32_t
res_ڈھونڈو_جدول_شے(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, ثابت uint16_t *کلید_Offsets, int32_t length,
                   ثابت حرف *کلید, ثابت حرف **real_کلید) {
    ثابت حرف *جدول_کلید;
    int32_t mid, آغاز, حد_بندی;
    صحیح نتیجہ;

    /* do a binary search for the key */
    آغاز=0;
    حد_بندی=length;
    جب(آغاز<حد_بندی) {
        mid = (آغاز + حد_بندی) / 2;
        جدول_کلید = RES_GET_KEY16(p_Res_ڈیٹا, کلید_Offsets[mid]);
        اگر (p_Res_ڈیٹا->useNativeStrcmp) {
            نتیجہ = uprv_strcmp(کلید, جدول_کلید);
        } ورنہ {
            نتیجہ = uprv_compareInvCharsAsAscii(کلید, جدول_کلید);
        }
        اگر (نتیجہ < 0) {
            حد_بندی = mid;
        } ورنہ اگر (نتیجہ > 0) {
            آغاز = mid + 1;
        } ورنہ {
            /* We found it! */
            *real_کلید=جدول_کلید;
            واپس mid;
        }
    }
    واپس URESDATA_شے_NOT_ملا;  /* not found or table is empty. */
}

ساکن int32_t
res_ڈھونڈو_Table32_شے(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, ثابت int32_t *کلید_Offsets, int32_t length,
                     ثابت حرف *کلید, ثابت حرف **real_کلید) {
    ثابت حرف *جدول_کلید;
    int32_t mid, آغاز, حد_بندی;
    صحیح نتیجہ;

    /* do a binary search for the key */
    آغاز=0;
    حد_بندی=length;
    جب(آغاز<حد_بندی) {
        mid = (آغاز + حد_بندی) / 2;
        جدول_کلید = RES_GET_KEY32(p_Res_ڈیٹا, کلید_Offsets[mid]);
        اگر (p_Res_ڈیٹا->useNativeStrcmp) {
            نتیجہ = uprv_strcmp(کلید, جدول_کلید);
        } ورنہ {
            نتیجہ = uprv_compareInvCharsAsAscii(کلید, جدول_کلید);
        }
        اگر (نتیجہ < 0) {
            حد_بندی = mid;
        } ورنہ اگر (نتیجہ > 0) {
            آغاز = mid + 1;
        } ورنہ {
            /* We found it! */
            *real_کلید=جدول_کلید;
            واپس mid;
        }
    }
    واپس URESDATA_شے_NOT_ملا;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ساکن UBool U_CALLCONV
isAcceptable(خالی *context,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->format_نسخہ, 4);
    واپس
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->ڈیٹا_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->ڈیٹا_Format[1]==0x65 &&
        pInfo->ڈیٹا_Format[2]==0x73 &&
        pInfo->ڈیٹا_Format[3]==0x42 &&
        (1<=pInfo->format_نسخہ[0] && pInfo->format_نسخہ[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

ساکن خالی
res_init(Resource_ڈیٹا *p_Res_ڈیٹا,
         UVersionInfo format_نسخہ, ثابت خالی *اندر_بائٹیں, int32_t length,
         UError_رمز *error_رمز) {
    UResType جڑ_Type;

    /* get the root resource */
    p_Res_ڈیٹا->p_جڑ = static_cast<ثابت int32_t*>(اندر_بائٹیں);
    p_Res_ڈیٹا->جڑ_Res = static_cast<Resource>(*p_Res_ڈیٹا->p_جڑ);
    p_Res_ڈیٹا->p16_ثنائی_ہندسہ_Units=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    اگر(length>=0 && (length/4)<((format_نسخہ[0]==1 && format_نسخہ[1]==0) ? 1 : 1+5)) {
        *error_رمز=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_ڈیٹا);
        واپس;
    }

    /* currently, we accept only resources that have a Table as their roots */
    جڑ_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_ڈیٹا->جڑ_Res));
    اگر(!URES_IS_جدول(جڑ_Type)) {
        *error_رمز=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_ڈیٹا);
        واپس;
    }

    اگر(format_نسخہ[0]==1 && format_نسخہ[1]==0) {
        p_Res_ڈیٹا->local_کلید_حد_بندی=0x10000;  /* greater than any 16-bit key string offset */
    } ورنہ {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ثابت int32_t *indexes=p_Res_ڈیٹا->p_جڑ+1;
        int32_t اشاریہ_لمبائی=indexes[URES_اشاریہ_لمبائی]&0xff;
        اگر(اشاریہ_لمبائی<=URES_اشاریہ_MAX_جدول_لمبائی) {
            *error_رمز=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_ڈیٹا);
            واپس;
        }
        اگر( length>=0 &&
            (length<((1+اشاریہ_لمبائی)<<2) ||
             length<(indexes[URES_اشاریہ_BUNDLE_چوٹی]<<2))
        ) {
            *error_رمز=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_ڈیٹا);
            واپس;
        }
        اگر(indexes[URES_اشاریہ_کلیدیں_چوٹی]>(1+اشاریہ_لمبائی)) {
            p_Res_ڈیٹا->local_کلید_حد_بندی=indexes[URES_اشاریہ_کلیدیں_چوٹی]<<2;
        }
        اگر(format_نسخہ[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_ڈیٹا->pool_String_اشاریہ_حد_بندی = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_اشاریہ_لمبائی]) >> 8);
        }
        اگر(اشاریہ_لمبائی>URES_اشاریہ_اوصاف) {
            int32_t att=indexes[URES_اشاریہ_اوصاف];
            p_Res_ڈیٹا->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_ڈیٹا->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_ڈیٹا->استعمال_کرتا_Pool_Bundle = static_cast<UBool>((att & URES_ATT_استعمال_کرتا_POOL_BUNDLE) != 0);
            p_Res_ڈیٹا->pool_String_اشاریہ_حد_بندی|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_ڈیٹا->pool_String_Index16_حد_بندی = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        اگر((p_Res_ڈیٹا->isPoolBundle || p_Res_ڈیٹا->استعمال_کرتا_Pool_Bundle) && اشاریہ_لمبائی<=URES_اشاریہ_POOL_CHECKSUM) {
            *error_رمز=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_ڈیٹا);
            واپس;
        }
        اگر( اشاریہ_لمبائی>URES_اشاریہ_16_ثنائی_ہندسہ_چوٹی &&
            indexes[URES_اشاریہ_16_ثنائی_ہندسہ_چوٹی]>indexes[URES_اشاریہ_کلیدیں_چوٹی]
        ) {
            p_Res_ڈیٹا->p16_ثنائی_ہندسہ_Units = reinterpret_cast<ثابت uint16_t*>(p_Res_ڈیٹا->p_جڑ + indexes[URES_اشاریہ_کلیدیں_چوٹی]);
        }
    }

    اگر(format_نسخہ[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_ڈیٹا->useNativeStrcmp=سچ;
    }
}

U_CAPI خالی U_EXPORT2
res_read(Resource_ڈیٹا *p_Res_ڈیٹا,
         ثابت UDataInfo *pInfo, ثابت خالی *اندر_بائٹیں, int32_t length,
         UError_رمز *error_رمز) {
    UVersionInfo format_نسخہ;

    uprv_memset(p_Res_ڈیٹا, 0, حجم(Resource_ڈیٹا));
    اگر(U_FAILURE(*error_رمز)) {
        واپس;
    }
    اگر(!isAcceptable(format_نسخہ, خالی_اشاریہ, خالی_اشاریہ, pInfo)) {
        *error_رمز=U_INVALID_FORMAT_ERROR;
        واپس;
    }
    res_init(p_Res_ڈیٹا, format_نسخہ, اندر_بائٹیں, length, error_رمز);
}

U_CFUNC خالی
res_بھرو(Resource_ڈیٹا *p_Res_ڈیٹا,
         ثابت حرف *راستہ, ثابت حرف *name, UError_رمز *error_رمز) {
    UVersionInfo format_نسخہ;

    uprv_memset(p_Res_ڈیٹا, 0, حجم(Resource_ڈیٹا));

    /* load the ResourceBundle file */
    p_Res_ڈیٹا->ڈیٹا=udata_openChoice(راستہ, "res", name, isAcceptable, format_نسخہ, error_رمز);
    اگر(U_FAILURE(*error_رمز)) {
        واپس;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_ڈیٹا, format_نسخہ, udata_get_حافظہ(p_Res_ڈیٹا->ڈیٹا), -1, error_رمز);
}

U_CFUNC خالی
res_unload(Resource_ڈیٹا *p_Res_ڈیٹا) {
    اگر(p_Res_ڈیٹا->ڈیٹا!=خالی_اشاریہ) {
        udata_close(p_Res_ڈیٹا->ڈیٹا);
        p_Res_ڈیٹا->ڈیٹا=خالی_اشاریہ;
    }
}

ساکن ثابت int8_t g_عوامی_Types[URES_حد_بندی] = {
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
    واپس (UResType)g_عوامی_Types[RES_GET_TYPE(res)];
}

U_CAPI ثابت char16_t * U_EXPORT2
res_getStringNoTrace(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource res, int32_t *p_لمبائی) {
    ثابت char16_t *p;
    uint32_t انحراف=RES_GET_انحراف(res);
    int32_t length;
    اگر(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        اگر((int32_t)انحراف<p_Res_ڈیٹا->pool_String_اشاریہ_حد_بندی) {
            p=(ثابت char16_t *)p_Res_ڈیٹا->pool_Bundle_لڑیاں+انحراف;
        } ورنہ {
            p=(ثابت char16_t *)p_Res_ڈیٹا->p16_ثنائی_ہندسہ_Units+(انحراف-p_Res_ڈیٹا->pool_String_اشاریہ_حد_بندی);
        }
        first=*p;
        اگر(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } ورنہ اگر(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } ورنہ اگر(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } ورنہ {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } ورنہ اگر(res==انحراف) /* RES_GET_TYPE(res)==URES_STRING */ {
        ثابت int32_t *p32= res==0 ? &g_خالی_خانے_String.length : p_Res_ڈیٹا->p_جڑ+res;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } ورنہ {
        p=خالی_اشاریہ;
        length=0;
    }
    اگر(p_لمبائی) {
        *p_لمبائی=length;
    }
    واپس p;
}

نامخلا {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource res) {
    uint32_t انحراف=RES_GET_انحراف(res);
    اگر (انحراف == 0) {
        // empty string
    } ورنہ اگر (res == انحراف) {
        ثابت int32_t *p32=p_Res_ڈیٹا->p_جڑ+res;
        int32_t length=*p32;
        ثابت char16_t* p = reinterpret_cast<ثابت char16_t*>(p32);
        واپس length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } ورنہ اگر (RES_GET_TYPE(res) == URES_STRING_V2) {
        ثابت char16_t *p;
        اگر (static_cast<int32_t>(انحراف) < p_Res_ڈیٹا->pool_String_اشاریہ_حد_بندی) {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_ڈیٹا->pool_Bundle_لڑیاں) + انحراف;
        } ورنہ {
            p = reinterpret_cast<ثابت char16_t*>(p_Res_ڈیٹا->p16_ثنائی_ہندسہ_Units) + (انحراف - p_Res_ڈیٹا->pool_String_اشاریہ_حد_بندی);
        }
        int32_t first=*p;
        اگر (first == 0x2205) {  // implicit length
            واپس p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } ورنہ اگر (first == 0xdc03) {  // explicit length 3 (should not occur)
            واپس p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } ورنہ {
            // Assume that the string has not been stored with more length units than necessary.
            واپس جھوٹ;
        }
    }
    واپس جھوٹ;
}

int32_t getStringArray(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, ثابت icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    اگر(dest == خالی_اشاریہ ? capacity != 0 : capacity < 0) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    int32_t length = array.get_سائز();
    اگر(length == 0) {
        واپس 0;
    }
    اگر(length > capacity) {
        error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        واپس length;
    }
    برائے(int32_t i = 0; i < length; ++i) {
        int32_t s_لمبائی;
        // No tracing: handled by the caller
        ثابت char16_t *s = res_getStringNoTrace(p_Res_ڈیٹا, array.اندرونی_Get_Resource(p_Res_ڈیٹا, i), &s_لمبائی);
        اگر(s == خالی_اشاریہ) {
            error_رمز = U_RESOURCE_TYPE_MISMATCH;
            واپس 0;
        }
        dest[i].سیٹ_مقصد(سچ, s, s_لمبائی);
    }
    واپس length;
}

}  // namespace

U_CAPI ثابت char16_t * U_EXPORT2
res_getAlias(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource res, int32_t *p_لمبائی) {
    ثابت char16_t *p;
    uint32_t انحراف=RES_GET_انحراف(res);
    int32_t length;
    اگر(RES_GET_TYPE(res)==URES_ALIAS) {
        ثابت int32_t *p32= انحراف==0 ? &g_خالی_خانے_String.length : p_Res_ڈیٹا->p_جڑ+انحراف;
        length=*p32++;
        p=(ثابت char16_t *)p32;
    } ورنہ {
        p=خالی_اشاریہ;
        length=0;
    }
    اگر(p_لمبائی) {
        *p_لمبائی=length;
    }
    واپس p;
}

U_CAPI ثابت uint8_t * U_EXPORT2
res_get_ثنائی_No_Trace(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource res, int32_t *p_لمبائی) {
    ثابت uint8_t *p;
    uint32_t انحراف=RES_GET_انحراف(res);
    int32_t length;
    اگر(RES_GET_TYPE(res)==URES_ثنائی) {
        ثابت int32_t *p32= انحراف==0 ? (ثابت int32_t*)&gEmpty32 : p_Res_ڈیٹا->p_جڑ+انحراف;
        length=*p32++;
        p=(ثابت uint8_t *)p32;
    } ورنہ {
        p=خالی_اشاریہ;
        length=0;
    }
    اگر(p_لمبائی) {
        *p_لمبائی=length;
    }
    واپس p;
}


U_CAPI ثابت int32_t * U_EXPORT2
res_get_Int_سمتیہ_No_Trace(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource res, int32_t *p_لمبائی) {
    ثابت int32_t *p;
    uint32_t انحراف=RES_GET_انحراف(res);
    int32_t length;
    اگر(RES_GET_TYPE(res)==URES_INT_سمتیہ) {
        p= انحراف==0 ? (ثابت int32_t *)&gEmpty32 : p_Res_ڈیٹا->p_جڑ+انحراف;
        length=*p++;
    } ورنہ {
        p=خالی_اشاریہ;
        length=0;
    }
    اگر(p_لمبائی) {
        *p_لمبائی=length;
    }
    واپس p;
}

U_CAPI int32_t U_EXPORT2
res_گنتی_Array_اشیاء(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource res) {
    uint32_t انحراف=RES_GET_انحراف(res);
    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_STRING:
    حالت URES_STRING_V2:
    حالت URES_ثنائی:
    حالت URES_ALIAS:
    حالت URES_INT:
    حالت URES_INT_سمتیہ:
        واپس 1;
    حالت URES_ARRAY:
    حالت URES_TABLE32:
        واپس انحراف==0 ? 0 : *(p_Res_ڈیٹا->p_جڑ+انحراف);
    حالت URES_جدول:
        واپس انحراف==0 ? 0 : *((ثابت uint16_t *)(p_Res_ڈیٹا->p_جڑ+انحراف));
    حالت URES_ARRAY16:
    حالت URES_TABLE16:
        واپس p_Res_ڈیٹا->p16_ثنائی_ہندسہ_Units[انحراف];
    طےشدہ:
        واپس 0;
    }
}

U_NAMESPACE_BEGIN

Resource_ڈیٹا_قدر::~Resource_ڈیٹا_قدر() {}

UResType Resource_ڈیٹا_قدر::getType() ثابت {
    واپس res_get_عوامی_Type(res);
}

ثابت char16_t *Resource_ڈیٹا_قدر::getString(int32_t &length, UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    ثابت char16_t *s = res_getString(fTraceInfo, &get_ڈیٹا(), res, &length);
    اگر(s == خالی_اشاریہ) {
        error_رمز = U_RESOURCE_TYPE_MISMATCH;
    }
    واپس s;
}

ثابت char16_t *Resource_ڈیٹا_قدر::getAliasString(int32_t &length, UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    ثابت char16_t *s = res_getAlias(&get_ڈیٹا(), res, &length);
    اگر(s == خالی_اشاریہ) {
        error_رمز = U_RESOURCE_TYPE_MISMATCH;
    }
    واپس s;
}

int32_t Resource_ڈیٹا_قدر::getInt(UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    اگر(RES_GET_TYPE(res) != URES_INT) {
        error_رمز = U_RESOURCE_TYPE_MISMATCH;
    }
    واپس res_getInt(fTraceInfo, res);
}

uint32_t Resource_ڈیٹا_قدر::getUInt(UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    اگر(RES_GET_TYPE(res) != URES_INT) {
        error_رمز = U_RESOURCE_TYPE_MISMATCH;
    }
    واپس res_getUInt(fTraceInfo, res);
}

ثابت int32_t *Resource_ڈیٹا_قدر::get_Int_سمتیہ(int32_t &length, UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    ثابت int32_t *iv = res_get_Int_سمتیہ(fTraceInfo, &get_ڈیٹا(), res, &length);
    اگر(iv == خالی_اشاریہ) {
        error_رمز = U_RESOURCE_TYPE_MISMATCH;
    }
    واپس iv;
}

ثابت uint8_t *Resource_ڈیٹا_قدر::get_ثنائی(int32_t &length, UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    ثابت uint8_t *b = res_get_ثنائی(fTraceInfo, &get_ڈیٹا(), res, &length);
    اگر(b == خالی_اشاریہ) {
        error_رمز = U_RESOURCE_TYPE_MISMATCH;
    }
    واپس b;
}

ResourceArray Resource_ڈیٹا_قدر::getArray(UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس {};
    }
    ثابت uint16_t *items16 = خالی_اشاریہ;
    ثابت Resource *items32 = خالی_اشاریہ;
    uint32_t انحراف=RES_GET_انحراف(res);
    int32_t length = 0;
    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_ARRAY:
        اگر (انحراف!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ثابت Resource*>(get_ڈیٹا().p_جڑ) + انحراف;
            length = *items32++;
        }
        توڑو;
    حالت URES_ARRAY16:
        items16 = get_ڈیٹا().p16_ثنائی_ہندسہ_Units+انحراف;
        length = *items16++;
        توڑو;
    طےشدہ:
        error_رمز = U_RESOURCE_TYPE_MISMATCH;
        واپس {};
    }
    واپس ResourceArray(items16, items32, length, fTraceInfo);
}

Resource_جدول Resource_ڈیٹا_قدر::get_جدول(UError_رمز &error_رمز) ثابت {
    اگر(U_FAILURE(error_رمز)) {
        واپس {};
    }
    ثابت uint16_t *keys16 = خالی_اشاریہ;
    ثابت int32_t *keys32 = خالی_اشاریہ;
    ثابت uint16_t *items16 = خالی_اشاریہ;
    ثابت Resource *items32 = خالی_اشاریہ;
    uint32_t انحراف = RES_GET_انحراف(res);
    int32_t length = 0;
    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_جدول:
        اگر (انحراف != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ثابت uint16_t*>(get_ڈیٹا().p_جڑ + انحراف);
            length = *keys16++;
            items32 = reinterpret_cast<ثابت Resource*>(keys16 + length + (~length & 1));
        }
        توڑو;
    حالت URES_TABLE16:
        keys16 = get_ڈیٹا().p16_ثنائی_ہندسہ_Units+انحراف;
        length = *keys16++;
        items16 = keys16 + length;
        توڑو;
    حالت URES_TABLE32:
        اگر (انحراف != 0) {  // empty if offset==0
            keys32 = get_ڈیٹا().p_جڑ+انحراف;
            length = *keys32++;
            items32 = reinterpret_cast<ثابت Resource*>(keys32) + length;
        }
        توڑو;
    طےشدہ:
        error_رمز = U_RESOURCE_TYPE_MISMATCH;
        واپس {};
    }
    واپس Resource_جدول(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_ڈیٹا_قدر::isNoInheritanceMarker() ثابت {
    واپس ::isNoInheritanceMarker(&get_ڈیٹا(), res);
}

int32_t Resource_ڈیٹا_قدر::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UError_رمز &error_رمز) ثابت {
    واپس ::getStringArray(&get_ڈیٹا(), getArray(error_رمز), dest, capacity, error_رمز);
}

int32_t Resource_ڈیٹا_قدر::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UError_رمز &error_رمز) ثابت {
    اگر(URES_IS_ARRAY(res)) {
        واپس ::getStringArray(&get_ڈیٹا(), getArray(error_رمز), dest, capacity, error_رمز);
    }
    اگر(U_FAILURE(error_رمز)) {
        واپس 0;
    }
    اگر(dest == خالی_اشاریہ ? capacity != 0 : capacity < 0) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    اگر(capacity < 1) {
        error_رمز = U_بفر_حد_سے_تجاوز_ERROR;
        واپس 1;
    }
    int32_t s_لمبائی;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_ڈیٹا(), res, &s_لمبائی);
    اگر(s != خالی_اشاریہ) {
        dest[0].سیٹ_مقصد(سچ, s, s_لمبائی);
        واپس 1;
    }
    error_رمز = U_RESOURCE_TYPE_MISMATCH;
    واپس 0;
}

UnicodeString Resource_ڈیٹا_قدر::get_String_Or_پہلا_کا_Array(UError_رمز &error_رمز) ثابت {
    UnicodeString us;
    اگر(U_FAILURE(error_رمز)) {
        واپس us;
    }
    int32_t s_لمبائی;
    ثابت char16_t *s = res_getString(fTraceInfo, &get_ڈیٹا(), res, &s_لمبائی);
    اگر(s != خالی_اشاریہ) {
        us.سیٹ_مقصد(سچ, s, s_لمبائی);
        واپس us;
    }
    ResourceArray array = getArray(error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس us;
    }
    اگر(array.get_سائز() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_ڈیٹا(), array.اندرونی_Get_Resource(&get_ڈیٹا(), 0), &s_لمبائی);
        اگر(s != خالی_اشاریہ) {
            us.سیٹ_مقصد(سچ, s, s_لمبائی);
            واپس us;
        }
    }
    error_رمز = U_RESOURCE_TYPE_MISMATCH;
    واپس us;
}

U_NAMESPACE_انتہا

ساکن Resource
makeResourceFrom16(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, int32_t res16) {
    اگر(res16<p_Res_ڈیٹا->pool_String_Index16_حد_بندی) {
        // Pool string, nothing to do.
    } ورنہ {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_ڈیٹا->pool_String_Index16_حد_بندی+p_Res_ڈیٹا->pool_String_اشاریہ_حد_بندی;
    }
    واپس URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_جدول_شے_By_کلید(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource جدول,
                      int32_t *اشاریہ_R, ثابت حرف **کلید) {
    uint32_t انحراف=RES_GET_انحراف(جدول);
    int32_t length;
    int32_t idx;
    اگر(کلید == خالی_اشاریہ || *کلید == خالی_اشاریہ) {
        واپس RES_BOGUS;
    }
    انتخاب(RES_GET_TYPE(جدول)) {
    حالت URES_جدول: {
        اگر (انحراف!=0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_ڈیٹا->p_جڑ+انحراف);
            length=*p++;
            *اشاریہ_R=idx=res_ڈھونڈو_جدول_شے(p_Res_ڈیٹا, p, length, *کلید, کلید);
            اگر(idx>=0) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                واپس p32[idx];
            }
        }
        توڑو;
    }
    حالت URES_TABLE16: {
        ثابت uint16_t *p=p_Res_ڈیٹا->p16_ثنائی_ہندسہ_Units+انحراف;
        length=*p++;
        *اشاریہ_R=idx=res_ڈھونڈو_جدول_شے(p_Res_ڈیٹا, p, length, *کلید, کلید);
        اگر(idx>=0) {
            واپس makeResourceFrom16(p_Res_ڈیٹا, p[length+idx]);
        }
        توڑو;
    }
    حالت URES_TABLE32: {
        اگر (انحراف!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_ڈیٹا->p_جڑ+انحراف;
            length=*p++;
            *اشاریہ_R=idx=res_ڈھونڈو_Table32_شے(p_Res_ڈیٹا, p, length, *کلید, کلید);
            اگر(idx>=0) {
                واپس (Resource)p[length+idx];
            }
        }
        توڑو;
    }
    طےشدہ:
        توڑو;
    }
    واپس RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_get_جدول_شے_By_اشاریہ(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource جدول,
                        int32_t اشاریہ_R, ثابت حرف **کلید) {
    uint32_t انحراف=RES_GET_انحراف(جدول);
    int32_t length;
    اگر (اشاریہ_R < 0) {
        واپس RES_BOGUS;
    }
    انتخاب(RES_GET_TYPE(جدول)) {
    حالت URES_جدول: {
        اگر (انحراف != 0) { /* empty if offset==0 */
            ثابت uint16_t *p= (ثابت uint16_t *)(p_Res_ڈیٹا->p_جڑ+انحراف);
            length=*p++;
            اگر(اشاریہ_R<length) {
                ثابت Resource *p32=(ثابت Resource *)(p+length+(~length&1));
                اگر(کلید!=خالی_اشاریہ) {
                    *کلید=RES_GET_KEY16(p_Res_ڈیٹا, p[اشاریہ_R]);
                }
                واپس p32[اشاریہ_R];
            }
        }
        توڑو;
    }
    حالت URES_TABLE16: {
        ثابت uint16_t *p=p_Res_ڈیٹا->p16_ثنائی_ہندسہ_Units+انحراف;
        length=*p++;
        اگر(اشاریہ_R<length) {
            اگر(کلید!=خالی_اشاریہ) {
                *کلید=RES_GET_KEY16(p_Res_ڈیٹا, p[اشاریہ_R]);
            }
            واپس makeResourceFrom16(p_Res_ڈیٹا, p[length+اشاریہ_R]);
        }
        توڑو;
    }
    حالت URES_TABLE32: {
        اگر (انحراف != 0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_ڈیٹا->p_جڑ+انحراف;
            length=*p++;
            اگر(اشاریہ_R<length) {
                اگر(کلید!=خالی_اشاریہ) {
                    *کلید=RES_GET_KEY32(p_Res_ڈیٹا, p[اشاریہ_R]);
                }
                واپس (Resource)p[length+اشاریہ_R];
            }
        }
        توڑو;
    }
    طےشدہ:
        توڑو;
    }
    واپس RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, ثابت حرف *کلید) {
    ثابت حرف *real_کلید=کلید;
    int32_t idx;
    واپس res_get_جدول_شے_By_کلید(p_Res_ڈیٹا, p_Res_ڈیٹا->جڑ_Res, &idx, &real_کلید);
}


UBool icu::Resource_جدول::get_کلید_And_قدر(int32_t i,
                                         ثابت حرف *&کلید, icu::Resource_قدر &value) ثابت {
    اگر(0 <= i && i < length) {
        icu::Resource_ڈیٹا_قدر &rd_قدر = static_cast<icu::Resource_ڈیٹا_قدر &>(value);
        اگر (keys16 != خالی_اشاریہ) {
            کلید = RES_GET_KEY16(&rd_قدر.get_ڈیٹا(), keys16[i]);
        } ورنہ {
            کلید = RES_GET_KEY32(&rd_قدر.get_ڈیٹا(), keys32[i]);
        }
        Resource res;
        اگر (items16 != خالی_اشاریہ) {
            res = makeResourceFrom16(&rd_قدر.get_ڈیٹا(), items16[i]);
        } ورنہ {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_قدر.سیٹ_Resource(res, ResourceTracer(fTraceInfo, کلید));
        واپس سچ;
    }
    واپس جھوٹ;
}

UBool icu::Resource_جدول::ڈھونڈو_قدر(ثابت حرف *کلید, Resource_قدر &value) ثابت {
    icu::Resource_ڈیٹا_قدر &rd_قدر = static_cast<icu::Resource_ڈیٹا_قدر &>(value);
    ثابت حرف *real_کلید = خالی_اشاریہ;
    int32_t i;
    اگر (keys16 != خالی_اشاریہ) {
        i = res_ڈھونڈو_جدول_شے(&rd_قدر.get_ڈیٹا(), keys16, length, کلید, &real_کلید);
    } ورنہ {
        i = res_ڈھونڈو_Table32_شے(&rd_قدر.get_ڈیٹا(), keys32, length, کلید, &real_کلید);
    }
    اگر (i >= 0) {
        Resource res;
        اگر (items16 != خالی_اشاریہ) {
            res = makeResourceFrom16(&rd_قدر.get_ڈیٹا(), items16[i]);
        } ورنہ {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_قدر.سیٹ_Resource(res, ResourceTracer(fTraceInfo, کلید));
        واپس سچ;
    }
    واپس جھوٹ;
}

U_CAPI Resource U_EXPORT2
res_get_Array_شے(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource array, int32_t اشاریہ_R) {
    uint32_t انحراف=RES_GET_انحراف(array);
    اگر (اشاریہ_R < 0) {
        واپس RES_BOGUS;
    }
    انتخاب(RES_GET_TYPE(array)) {
    حالت URES_ARRAY: {
        اگر (انحراف!=0) { /* empty if offset==0 */
            ثابت int32_t *p= p_Res_ڈیٹا->p_جڑ+انحراف;
            اگر(اشاریہ_R<*p) {
                واپس (Resource)p[1+اشاریہ_R];
            }
        }
        توڑو;
    }
    حالت URES_ARRAY16: {
        ثابت uint16_t *p=p_Res_ڈیٹا->p16_ثنائی_ہندسہ_Units+انحراف;
        اگر(اشاریہ_R<*p) {
            واپس makeResourceFrom16(p_Res_ڈیٹا, p[1+اشاریہ_R]);
        }
        توڑو;
    }
    طےشدہ:
        توڑو;
    }
    واپس RES_BOGUS;
}

uint32_t icu::ResourceArray::اندرونی_Get_Resource(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, int32_t i) ثابت {
    اگر (items16 != خالی_اشاریہ) {
        واپس makeResourceFrom16(p_Res_ڈیٹا, items16[i]);
    } ورنہ {
        واپس items32[i];
    }
}

UBool icu::ResourceArray::get_قدر(int32_t i, icu::Resource_قدر &value) ثابت {
    اگر(0 <= i && i < length) {
        icu::Resource_ڈیٹا_قدر &rd_قدر = static_cast<icu::Resource_ڈیٹا_قدر &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_قدر.سیٹ_Resource(
            اندرونی_Get_Resource(&rd_قدر.get_ڈیٹا(), i),
            ResourceTracer(fTraceInfo, i));
        واپس سچ;
    }
    واپس جھوٹ;
}

U_CFUNC Resource
res_ڈھونڈو_Resource(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, Resource r, حرف** راستہ, ثابت حرف** کلید) {
  حرف *راستہ_P = *راستہ, *اگلا_Sep_P = *راستہ;
  حرف *close_اشاریہ = خالی_اشاریہ;
  Resource t1 = r;
  Resource t2;
  int32_t اشاریہ_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  اگر(!uprv_strlen(راستہ_P)) {
      واپس r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  اگر(!URES_IS_CONTAINER(type)) {
      واپس RES_BOGUS;
  }
  
  جب(اگلا_Sep_P && *راستہ_P && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    اگلا_Sep_P = uprv_strchr(راستہ_P, RES_راستہ_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    اگر(اگلا_Sep_P != خالی_اشاریہ) {
      اگر(اگلا_Sep_P == راستہ_P) {
        // Empty key string.
        واپس RES_BOGUS;
      }
      *اگلا_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *راستہ = اگلا_Sep_P+1;
    } ورنہ {
      *راستہ = uprv_strchr(راستہ_P, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    اگر(URES_IS_جدول(type)) {
      *کلید = راستہ_P;
      t2 = res_get_جدول_شے_By_کلید(p_Res_ڈیٹا, t1, &اشاریہ_R, کلید);
    } ورنہ اگر(URES_IS_ARRAY(type)) {
      اشاریہ_R = uprv_strtol(راستہ_P, &close_اشاریہ, 10);
      اگر(اشاریہ_R >= 0 && *close_اشاریہ == 0) {
        t2 = res_get_Array_شے(p_Res_ڈیٹا, t1, اشاریہ_R);
      } ورنہ {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *کلید = خالی_اشاریہ;
    } ورنہ { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    راستہ_P = *راستہ;
  }

  واپس t1;
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
قسمتعریف ساخت صف_2 {
    int32_t کلید_اشاریہ, ترتیب_دو_اشاریہ;
} صف_2;

ساکن int32_t U_CALLCONV
ures_compare_صفیں(ثابت خالی *context, ثابت خالی *بائیں, ثابت خالی *دائیں) {
    ثابت حرف* کلید_Chars = static_cast<ثابت حرف*>(context);
    واپس static_cast<int32_t>(uprv_strcmp(کلید_Chars + static_cast<ثابت صف_2*>(بائیں)->کلید_اشاریہ,
                                            کلید_Chars + static_cast<ثابت صف_2*>(دائیں)->کلید_اشاریہ));
}

قسمتعریف ساخت عارضی_جدول_2 {
    ثابت حرف *کلید_Chars;
    صف_2 *صفیں;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_کلید_حد_بندی;
    uint8_t major_Format_نسخہ;
} عارضی_جدول_2;

شمار {
    ڈھیر_صف_استعداد=200
};

/* The table item key string is not locally available. */
ساکن ثابت حرف *ثابت g_Unknown_کلید="";

#اگر !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ساکن ثابت char16_t g_Collation_Bin_کلید[]=u"%%CollationBin";
#اگر_ختم

/*
 * swap one resource item
 */
ساکن خالی
ures_تبادلہ_Resource(ثابت UDataSwapper *ds,
                  ثابت Resource *اندر_Bundle, Resource *برآمد_Bundle,
                  Resource res, /* caller swaps res itself */
                  ثابت حرف *کلید,
                  عارضی_جدول_2 *p_عارضی_جدول,
                  UError_رمز *p_Error_رمز) {
    ثابت Resource *p;
    Resource *q;
    int32_t انحراف, count;

    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_TABLE16:
    حالت URES_STRING_V2:
    حالت URES_INT:
    حالت URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        واپس;
    طےشدہ:
        توڑو;
    }

    /* all other types use an offset to point to their data */
    انحراف = static_cast<int32_t>(RES_GET_انحراف(res));
    اگر(انحراف==0) {
        /* special offset indicating an empty item */
        واپس;
    }
    اگر (p_عارضی_جدول->resFlags[انحراف >> 5] & (static_cast<uint32_t>(1) << (انحراف & 0x1f))) {
        /* we already swapped this resource item */
        واپس;
    } ورنہ {
        /* mark it as swapped now */
        p_عارضی_جدول->resFlags[انحراف >> 5] |= static_cast<uint32_t>(1) << (انحراف & 0x1f);
    }

    p=اندر_Bundle+انحراف;
    q=برآمد_Bundle+انحراف;

    انتخاب(RES_GET_TYPE(res)) {
    حالت URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    حالت URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->تبادلہ_Array32(ds, p, 4, q, p_Error_رمز);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->تبادلہ_Array16(ds, p+1, 2*count, q+1, p_Error_رمز);
        توڑو;
    حالت URES_ثنائی:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->تبادلہ_Array32(ds, p, 4, q, p_Error_رمز);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#اگر !UCONFIG_NO_COLLATION
        اگر( کلید!=خالی_اشاریہ &&  /* the binary is in a table */
            (کلید!=g_Unknown_کلید ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, کلید, -1,
                                       g_Collation_Bin_کلید, UPRV_LENGTHOF(g_Collation_Bin_کلید)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_ثنائی(ds, p+1, count))
        ) {
            ucol_تبادلہ(ds, p+1, count, q+1, p_Error_رمز);
        }
#اگر_ختم
        توڑو;
    حالت URES_جدول:
    حالت URES_TABLE32:
        {
            ثابت uint16_t *pKey16;
            uint16_t *qKey16;

            ثابت int32_t *pKey32;
            int32_t *qKey32;

            Resource شے;
            int32_t i, old_اشاریہ;

            اگر(RES_GET_TYPE(res)==URES_جدول) {
                /* get table item count */
                pKey16 = reinterpret_cast<ثابت uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=خالی_اشاریہ;

                /* swap count */
                ds->تبادلہ_Array16(ds, pKey16++, 2, qKey16++, p_Error_رمز);

                انحراف+=((1+count)+1)/2;
            } ورنہ {
                /* get table item count */
                pKey32 = reinterpret_cast<ثابت int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=خالی_اشاریہ;

                /* swap count */
                ds->تبادلہ_Array32(ds, pKey32++, 4, qKey32++, p_Error_رمز);

                انحراف+=1+count;
            }

            اگر(count==0) {
                توڑو;
            }

            p=اندر_Bundle+انحراف; /* pointer to table resources */
            q=برآمد_Bundle+انحراف;

            /* recurse */
            برائے(i=0; i<count; ++i) {
                ثابت حرف *شے_کلید=g_Unknown_کلید;
                اگر(pKey16!=خالی_اشاریہ) {
                    int32_t کلید_انحراف=ds->readUInt16(pKey16[i]);
                    اگر(کلید_انحراف<p_عارضی_جدول->local_کلید_حد_بندی) {
                        شے_کلید = reinterpret_cast<ثابت حرف*>(برآمد_Bundle) + کلید_انحراف;
                    }
                } ورنہ {
                    int32_t کلید_انحراف=udata_readInt32(ds, pKey32[i]);
                    اگر(کلید_انحراف>=0) {
                        شے_کلید = reinterpret_cast<ثابت حرف*>(برآمد_Bundle) + کلید_انحراف;
                    }
                }
                شے=ds->readUInt32(p[i]);
                ures_تبادلہ_Resource(ds, اندر_Bundle, برآمد_Bundle, شے, شے_کلید, p_عارضی_جدول, p_Error_رمز);
                اگر(U_FAILURE(*p_Error_رمز)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, شے);
                    واپس;
                }
            }

            اگر(p_عارضی_جدول->major_Format_نسخہ>1 || ds->اندر_Charset==ds->برآمد_Charset) {
                /* no need to sort, just swap the offset/value arrays */
                اگر(pKey16!=خالی_اشاریہ) {
                    ds->تبادلہ_Array16(ds, pKey16, count*2, qKey16, p_Error_رمز);
                    ds->تبادلہ_Array32(ds, p, count*4, q, p_Error_رمز);
                } ورنہ {
                    /* swap key offsets and items as one array */
                    ds->تبادلہ_Array32(ds, pKey32, count*2*4, qKey32, p_Error_رمز);
                }
                توڑو;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            اگر(pKey16!=خالی_اشاریہ) {
                برائے(i=0; i<count; ++i) {
                    p_عارضی_جدول->صفیں[i].کلید_اشاریہ=ds->readUInt16(pKey16[i]);
                    p_عارضی_جدول->صفیں[i].ترتیب_دو_اشاریہ=i;
                }
            } ورنہ {
                برائے(i=0; i<count; ++i) {
                    p_عارضی_جدول->صفیں[i].کلید_اشاریہ=udata_readInt32(ds, pKey32[i]);
                    p_عارضی_جدول->صفیں[i].ترتیب_دو_اشاریہ=i;
                }
            }
            uprv_ترتیب_دو_Array(p_عارضی_جدول->صفیں, count, حجم(صف_2),
                           ures_compare_صفیں, p_عارضی_جدول->کلید_Chars,
                           جھوٹ, p_Error_رمز);
            اگر(U_FAILURE(*p_Error_رمز)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                واپس;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*کلیدیں*/
            اگر(pKey16!=خالی_اشاریہ) {
                uint16_t *rKey16;

                اگر(pKey16!=qKey16) {
                    rKey16=qKey16;
                } ورنہ {
                    rKey16 = reinterpret_cast<uint16_t*>(p_عارضی_جدول->resort);
                }
                برائے(i=0; i<count; ++i) {
                    old_اشاریہ=p_عارضی_جدول->صفیں[i].ترتیب_دو_اشاریہ;
                    ds->تبادلہ_Array16(ds, pKey16+old_اشاریہ, 2, rKey16+i, p_Error_رمز);
                }
                اگر(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } ورنہ {
                int32_t *rKey32;

                اگر(pKey32!=qKey32) {
                    rKey32=qKey32;
                } ورنہ {
                    rKey32=p_عارضی_جدول->resort;
                }
                برائے(i=0; i<count; ++i) {
                    old_اشاریہ=p_عارضی_جدول->صفیں[i].ترتیب_دو_اشاریہ;
                    ds->تبادلہ_Array32(ds, pKey32+old_اشاریہ, 4, rKey32+i, p_Error_رمز);
                }
                اگر(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                اگر(p!=q) {
                    r=q;
                } ورنہ {
                    r = reinterpret_cast<Resource*>(p_عارضی_جدول->resort);
                }
                برائے(i=0; i<count; ++i) {
                    old_اشاریہ=p_عارضی_جدول->صفیں[i].ترتیب_دو_اشاریہ;
                    ds->تبادلہ_Array32(ds, p+old_اشاریہ, 4, r+i, p_Error_رمز);
                }
                اگر(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        توڑو;
    حالت URES_ARRAY:
        {
            Resource شے;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->تبادلہ_Array32(ds, p++, 4, q++, p_Error_رمز);

            /* recurse */
            برائے(i=0; i<count; ++i) {
                شے=ds->readUInt32(p[i]);
                ures_تبادلہ_Resource(ds, اندر_Bundle, برآمد_Bundle, شے, خالی_اشاریہ, p_عارضی_جدول, p_Error_رمز);
                اگر(U_FAILURE(*p_Error_رمز)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, شے);
                    واپس;
                }
            }

            /* swap items */
            ds->تبادلہ_Array32(ds, p, 4*count, q, p_Error_رمز);
        }
        توڑو;
    حالت URES_INT_سمتیہ:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->تبادلہ_Array32(ds, p, 4*(1+count), q, p_Error_رمز);
        توڑو;
    طےشدہ:
        /* also catches RES_BOGUS */
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        توڑو;
    }
}

U_CAPI int32_t U_EXPORT2
ures_تبادلہ(ثابت UDataSwapper *ds,
          ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
          UError_رمز *p_Error_رمز) {
    ثابت UDataInfo *pInfo;
    ثابت Resource *اندر_Bundle;
    Resource جڑ_Res;
    int32_t سرخی_سائز, max_جدول_لمبائی;

    صف_2 صفیں[ڈھیر_صف_استعداد];
    int32_t resort[ڈھیر_صف_استعداد];
    عارضی_جدول_2 عارضی_جدول;

    ثابت int32_t *اندر_Indexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_لمبائی, اشاریہ_لمبائی, کلیدیں_Bottom, کلیدیں_چوٹی, resBottom, چوٹی;

    /* udata_swapDataHeader checks the arguments */
    سرخی_سائز=udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
    اگر(!(
        pInfo->ڈیٹا_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->ڈیٹا_Format[1]==0x65 &&
        pInfo->ڈیٹا_Format[2]==0x73 &&
        pInfo->ڈیٹا_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->format_نسخہ[0]==1 && pInfo->format_نسخہ[1]>=1) ||
            pInfo->format_نسخہ[0]==2 || pInfo->format_نسخہ[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1],
                         pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3],
                         pInfo->format_نسخہ[0], pInfo->format_نسخہ[1]);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }
    عارضی_جدول.major_Format_نسخہ=pInfo->format_نسخہ[0];

    /* a resource bundle must contain at least one resource item */
    اگر(length<0) {
        bundle_لمبائی=-1;
    } ورنہ {
        bundle_لمبائی=(length-سرخی_سائز)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        اگر(bundle_لمبائی<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-سرخی_سائز);
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
    }

    اندر_Bundle=(ثابت Resource *)((ثابت حرف *)اندر_ڈیٹا+سرخی_سائز);
    جڑ_Res=ds->readUInt32(*اندر_Bundle);

    /* formatVersion 1.1 adds the indexes[] array */
    اندر_Indexes=(ثابت int32_t *)(اندر_Bundle+1);

    اشاریہ_لمبائی=udata_readInt32(ds, اندر_Indexes[URES_اشاریہ_لمبائی])&0xff;
    اگر(اشاریہ_لمبائی<=URES_اشاریہ_MAX_جدول_لمبائی) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }
    کلیدیں_Bottom=1+اشاریہ_لمبائی;
    کلیدیں_چوٹی=udata_readInt32(ds, اندر_Indexes[URES_اشاریہ_کلیدیں_چوٹی]);
    اگر(اشاریہ_لمبائی>URES_اشاریہ_16_ثنائی_ہندسہ_چوٹی) {
        resBottom=udata_readInt32(ds, اندر_Indexes[URES_اشاریہ_16_ثنائی_ہندسہ_چوٹی]);
    } ورنہ {
        resBottom=کلیدیں_چوٹی;
    }
    چوٹی=udata_readInt32(ds, اندر_Indexes[URES_اشاریہ_BUNDLE_چوٹی]);
    max_جدول_لمبائی=udata_readInt32(ds, اندر_Indexes[URES_اشاریہ_MAX_جدول_لمبائی]);

    اگر(0<=bundle_لمبائی && bundle_لمبائی<چوٹی) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         چوٹی, bundle_لمبائی);
        *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
        واپس 0;
    }
    اگر(کلیدیں_چوٹی>(1+اشاریہ_لمبائی)) {
        عارضی_جدول.local_کلید_حد_بندی=کلیدیں_چوٹی<<2;
    } ورنہ {
        عارضی_جدول.local_کلید_حد_بندی=0;
    }

    اگر(length>=0) {
        Resource *برآمد_Bundle=(Resource *)((حرف *)برآمد_ڈیٹا+سرخی_سائز);

        /* track which resources we have already swapped */
        uint32_t ڈھیر_Res_Flags[ڈھیر_صف_استعداد];
        int32_t res_Flags_لمبائی;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_لمبائی=(length+31)>>5;          /* number of bytes needed */
        res_Flags_لمبائی=(res_Flags_لمبائی+3)&~3;   /* multiple of 4 bytes for uint32_t */
        اگر(res_Flags_لمبائی<=(int32_t)حجم(ڈھیر_Res_Flags)) {
            عارضی_جدول.resFlags=ڈھیر_Res_Flags;
        } ورنہ {
            عارضی_جدول.resFlags=(uint32_t *)uprv_malloc(res_Flags_لمبائی);
            اگر(عارضی_جدول.resFlags==خالی_اشاریہ) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
                واپس 0;
            }
        }
        uprv_memset(عارضی_جدول.resFlags, 0, res_Flags_لمبائی);

        /* copy the bundle for binary and inaccessible data */
        اگر(اندر_ڈیٹا!=برآمد_ڈیٹا) {
            uprv_memcpy(برآمد_Bundle, اندر_Bundle, 4*چوٹی);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_تبادلہ_Inv_String_بلاک(ds, اندر_Bundle+کلیدیں_Bottom, 4*(کلیدیں_چوٹی-کلیدیں_Bottom),
                                    برآمد_Bundle+کلیدیں_Bottom, p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(کلیدیں_چوٹی-کلیدیں_Bottom));
            اگر(عارضی_جدول.resFlags!=ڈھیر_Res_Flags) {
                uprv_free(عارضی_جدول.resFlags);
            }
            واپس 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        اگر(کلیدیں_چوٹی<resBottom) {
            ds->تبادلہ_Array16(ds, اندر_Bundle+کلیدیں_چوٹی, (resBottom-کلیدیں_چوٹی)*4, برآمد_Bundle+کلیدیں_چوٹی, p_Error_رمز);
            اگر(U_FAILURE(*p_Error_رمز)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-کلیدیں_چوٹی));
                اگر(عارضی_جدول.resFlags!=ڈھیر_Res_Flags) {
                    uprv_free(عارضی_جدول.resFlags);
                }
                واپس 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        عارضی_جدول.کلید_Chars=(ثابت حرف *)برآمد_Bundle; /* sort by outCharset */
        اگر(عارضی_جدول.major_Format_نسخہ>1 || max_جدول_لمبائی<=ڈھیر_صف_استعداد) {
            عارضی_جدول.صفیں=صفیں;
            عارضی_جدول.resort=resort;
        } ورنہ {
            عارضی_جدول.صفیں=(صف_2 *)uprv_malloc(max_جدول_لمبائی*حجم(صف_2)+max_جدول_لمبائی*4);
            اگر(عارضی_جدول.صفیں==خالی_اشاریہ) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_جدول_لمبائی);
                *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
                اگر(عارضی_جدول.resFlags!=ڈھیر_Res_Flags) {
                    uprv_free(عارضی_جدول.resFlags);
                }
                واپس 0;
            }
            عارضی_جدول.resort=(int32_t *)(عارضی_جدول.صفیں+max_جدول_لمبائی);
        }

        /* swap the resources */
        ures_تبادلہ_Resource(ds, اندر_Bundle, برآمد_Bundle, جڑ_Res, خالی_اشاریہ, &عارضی_جدول, p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             جڑ_Res);
        }

        اگر(عارضی_جدول.صفیں!=صفیں) {
            uprv_free(عارضی_جدول.صفیں);
        }
        اگر(عارضی_جدول.resFlags!=ڈھیر_Res_Flags) {
            uprv_free(عارضی_جدول.resFlags);
        }

        /* swap the root resource and indexes */
        ds->تبادلہ_Array32(ds, اندر_Bundle, کلیدیں_Bottom*4, برآمد_Bundle, p_Error_رمز);
    }

    واپس سرخی_سائز+4*چوٹی;
}

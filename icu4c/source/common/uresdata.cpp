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
#परिभाषय RES_GET_KEY16(p_Res_दत्त, कुञ्चिका_विस्थापनम्) \
    ((कुञ्चिका_विस्थापनम्)<(p_Res_दत्त)->local_कुञ्चिका_सीमा ? \
        (ध्रुव वर्ण *)(p_Res_दत्त)->p_मूलम्+(कुञ्चिका_विस्थापनम्) : \
        (p_Res_दत्त)->pool_Bundle_कुञ्चिकाः+(कुञ्चिका_विस्थापनम्)-(p_Res_दत्त)->local_कुञ्चिका_सीमा)

#परिभाषय RES_GET_KEY32(p_Res_दत्त, कुञ्चिका_विस्थापनम्) \
    ((कुञ्चिका_विस्थापनम्)>=0 ? \
        (ध्रुव वर्ण *)(p_Res_दत्त)->p_मूलम्+(कुञ्चिका_विस्थापनम्) : \
        (p_Res_दत्त)->pool_Bundle_कुञ्चिकाः+((कुञ्चिका_विस्थापनम्)&0x7fffffff))

#परिभाषय URESDATA_ITEM_NOT_प्राप्तम् -1

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
} g_रिक्तानि_String={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

स्थिर int32_t
res_find_सारणी_Item(ध्रुव Resource_दत्त *p_Res_दत्त, ध्रुव uint16_t *कुञ्चिका_Offsets, int32_t length,
                   ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण **real_कुञ्चिका) {
    ध्रुव वर्ण *सारणी_कुञ्चिका;
    int32_t mid, start, सीमा;
    पूर्णाङ्क परिणाम;

    /* do a binary search for the key */
    start=0;
    सीमा=length;
    यावत्(start<सीमा) {
        mid = (start + सीमा) / 2;
        सारणी_कुञ्चिका = RES_GET_KEY16(p_Res_दत्त, कुञ्चिका_Offsets[mid]);
        यदि (p_Res_दत्त->useNativeStrcmp) {
            परिणाम = uprv_strcmp(कुञ्चिका, सारणी_कुञ्चिका);
        } अन्यथा {
            परिणाम = uprv_compareInvCharsAsAscii(कुञ्चिका, सारणी_कुञ्चिका);
        }
        यदि (परिणाम < 0) {
            सीमा = mid;
        } अन्यथा यदि (परिणाम > 0) {
            start = mid + 1;
        } अन्यथा {
            /* We found it! */
            *real_कुञ्चिका=सारणी_कुञ्चिका;
            प्रत्यागम mid;
        }
    }
    प्रत्यागम URESDATA_ITEM_NOT_प्राप्तम्;  /* not found or table is empty. */
}

स्थिर int32_t
_res_findTable32Item(ध्रुव Resource_दत्त *p_Res_दत्त, ध्रुव int32_t *कुञ्चिका_Offsets, int32_t length,
                     ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण **real_कुञ्चिका) {
    ध्रुव वर्ण *सारणी_कुञ्चिका;
    int32_t mid, start, सीमा;
    पूर्णाङ्क परिणाम;

    /* do a binary search for the key */
    start=0;
    सीमा=length;
    यावत्(start<सीमा) {
        mid = (start + सीमा) / 2;
        सारणी_कुञ्चिका = RES_GET_KEY32(p_Res_दत्त, कुञ्चिका_Offsets[mid]);
        यदि (p_Res_दत्त->useNativeStrcmp) {
            परिणाम = uprv_strcmp(कुञ्चिका, सारणी_कुञ्चिका);
        } अन्यथा {
            परिणाम = uprv_compareInvCharsAsAscii(कुञ्चिका, सारणी_कुञ्चिका);
        }
        यदि (परिणाम < 0) {
            सीमा = mid;
        } अन्यथा यदि (परिणाम > 0) {
            start = mid + 1;
        } अन्यथा {
            /* We found it! */
            *real_कुञ्चिका=सारणी_कुञ्चिका;
            प्रत्यागम mid;
        }
    }
    प्रत्यागम URESDATA_ITEM_NOT_प्राप्तम्;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

स्थिर UBool U_CALLCONV
isAcceptable(शून्य *context,
             ध्रुव वर्ण * /*type*/, ध्रुव वर्ण * /*name*/,
             ध्रुव UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->format_संस्करणम्, 4);
    प्रत्यागम
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->दत्त_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->दत्त_Format[1]==0x65 &&
        pInfo->दत्त_Format[2]==0x73 &&
        pInfo->दत्त_Format[3]==0x42 &&
        (1<=pInfo->format_संस्करणम्[0] && pInfo->format_संस्करणम्[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

स्थिर शून्य
res_init(Resource_दत्त *p_Res_दत्त,
         UVersionInfo format_संस्करणम्, ध्रुव शून्य *अन्तर्_अष्टकानि, int32_t length,
         UError_संहिता *error_संहिता) {
    UResType मूलम्_Type;

    /* get the root resource */
    p_Res_दत्त->p_मूलम् = static_cast<ध्रुव int32_t*>(अन्तर्_अष्टकानि);
    p_Res_दत्त->मूलम्_Res = static_cast<Resource>(*p_Res_दत्त->p_मूलम्);
    p_Res_दत्त->p16BitUnits=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    यदि(length>=0 && (length/4)<((format_संस्करणम्[0]==1 && format_संस्करणम्[1]==0) ? 1 : 1+5)) {
        *error_संहिता=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_दत्त);
        प्रत्यागम;
    }

    /* currently, we accept only resources that have a Table as their roots */
    मूलम्_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_दत्त->मूलम्_Res));
    यदि(!URES_IS_सारणी(मूलम्_Type)) {
        *error_संहिता=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_दत्त);
        प्रत्यागम;
    }

    यदि(format_संस्करणम्[0]==1 && format_संस्करणम्[1]==0) {
        p_Res_दत्त->local_कुञ्चिका_सीमा=0x10000;  /* greater than any 16-bit key string offset */
    } अन्यथा {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ध्रुव int32_t *indexes=p_Res_दत्त->p_मूलम्+1;
        int32_t सूचकांक_दीर्घता=indexes[URES_सूचकांक_दीर्घता]&0xff;
        यदि(सूचकांक_दीर्घता<=URES_सूचकांक_MAX_सारणी_दीर्घता) {
            *error_संहिता=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_दत्त);
            प्रत्यागम;
        }
        यदि( length>=0 &&
            (length<((1+सूचकांक_दीर्घता)<<2) ||
             length<(indexes[URES_सूचकांक_BUNDLE_शिखरम्]<<2))
        ) {
            *error_संहिता=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_दत्त);
            प्रत्यागम;
        }
        यदि(indexes[URES_सूचकांक_कुञ्चिकाः_शिखरम्]>(1+सूचकांक_दीर्घता)) {
            p_Res_दत्त->local_कुञ्चिका_सीमा=indexes[URES_सूचकांक_कुञ्चिकाः_शिखरम्]<<2;
        }
        यदि(format_संस्करणम्[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_दत्त->pool_String_सूचकांक_सीमा = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_सूचकांक_दीर्घता]) >> 8);
        }
        यदि(सूचकांक_दीर्घता>URES_सूचकांक_गुणाः) {
            int32_t att=indexes[URES_सूचकांक_गुणाः];
            p_Res_दत्त->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_दत्त->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_दत्त->उपयुङ्क्ते_Pool_Bundle = static_cast<UBool>((att & URES_ATT_उपयुङ्क्ते_POOL_BUNDLE) != 0);
            p_Res_दत्त->pool_String_सूचकांक_सीमा|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_दत्त->pool_String_Index16_सीमा = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        यदि((p_Res_दत्त->isPoolBundle || p_Res_दत्त->उपयुङ्क्ते_Pool_Bundle) && सूचकांक_दीर्घता<=URES_सूचकांक_POOL_CHECKSUM) {
            *error_संहिता=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_दत्त);
            प्रत्यागम;
        }
        यदि( सूचकांक_दीर्घता>URES_सूचकांक_16_BIT_शिखरम् &&
            indexes[URES_सूचकांक_16_BIT_शिखरम्]>indexes[URES_सूचकांक_कुञ्चिकाः_शिखरम्]
        ) {
            p_Res_दत्त->p16BitUnits = reinterpret_cast<ध्रुव uint16_t*>(p_Res_दत्त->p_मूलम् + indexes[URES_सूचकांक_कुञ्चिकाः_शिखरम्]);
        }
    }

    यदि(format_संस्करणम्[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_दत्त->useNativeStrcmp=सत्य;
    }
}

U_CAPI शून्य U_EXPORT2
res_read(Resource_दत्त *p_Res_दत्त,
         ध्रुव UDataInfo *pInfo, ध्रुव शून्य *अन्तर्_अष्टकानि, int32_t length,
         UError_संहिता *error_संहिता) {
    UVersionInfo format_संस्करणम्;

    uprv_memset(p_Res_दत्त, 0, परिमाण(Resource_दत्त));
    यदि(U_FAILURE(*error_संहिता)) {
        प्रत्यागम;
    }
    यदि(!isAcceptable(format_संस्करणम्, शून्यसूचकः, शून्यसूचकः, pInfo)) {
        *error_संहिता=U_INVALID_FORMAT_ERROR;
        प्रत्यागम;
    }
    res_init(p_Res_दत्त, format_संस्करणम्, अन्तर्_अष्टकानि, length, error_संहिता);
}

U_CFUNC शून्य
res_आरोपय(Resource_दत्त *p_Res_दत्त,
         ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *name, UError_संहिता *error_संहिता) {
    UVersionInfo format_संस्करणम्;

    uprv_memset(p_Res_दत्त, 0, परिमाण(Resource_दत्त));

    /* load the ResourceBundle file */
    p_Res_दत्त->दत्त=udata_openChoice(मार्गः, "res", name, isAcceptable, format_संस्करणम्, error_संहिता);
    यदि(U_FAILURE(*error_संहिता)) {
        प्रत्यागम;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_दत्त, format_संस्करणम्, udata_get_स्मृतिः(p_Res_दत्त->दत्त), -1, error_संहिता);
}

U_CFUNC शून्य
res_unload(Resource_दत्त *p_Res_दत्त) {
    यदि(p_Res_दत्त->दत्त!=शून्यसूचकः) {
        udata_close(p_Res_दत्त->दत्त);
        p_Res_दत्त->दत्त=शून्यसूचकः;
    }
}

स्थिर ध्रुव int8_t g_लौकिकम्_Types[URES_सीमा] = {
    URES_STRING,
    URES_द्विमान,
    URES_सारणी,
    URES_ALIAS,

    URES_सारणी,     /* URES_TABLE32 */
    URES_सारणी,     /* URES_TABLE16 */
    URES_STRING,    /* URES_STRING_V2 */
    URES_INT,

    URES_ARRAY,
    URES_ARRAY,     /* URES_ARRAY16 */
    URES_NONE,
    URES_NONE,

    URES_NONE,
    URES_NONE,
    URES_INT_सदिशम्,
    URES_NONE
};

U_CAPI UResType U_EXPORT2
res_get_लौकिकम्_Type(Resource res) {
    प्रत्यागम (UResType)g_लौकिकम्_Types[RES_GET_TYPE(res)];
}

U_CAPI ध्रुव char16_t * U_EXPORT2
res_getStringNoTrace(ध्रुव Resource_दत्त *p_Res_दत्त, Resource res, int32_t *p_दीर्घता) {
    ध्रुव char16_t *p;
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(res);
    int32_t length;
    यदि(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        यदि((int32_t)विस्थापनम्<p_Res_दत्त->pool_String_सूचकांक_सीमा) {
            p=(ध्रुव char16_t *)p_Res_दत्त->pool_Bundle_वाक्यानि+विस्थापनम्;
        } अन्यथा {
            p=(ध्रुव char16_t *)p_Res_दत्त->p16BitUnits+(विस्थापनम्-p_Res_दत्त->pool_String_सूचकांक_सीमा);
        }
        first=*p;
        यदि(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } अन्यथा यदि(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } अन्यथा यदि(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } अन्यथा {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } अन्यथा यदि(res==विस्थापनम्) /* RES_GET_TYPE(res)==URES_STRING */ {
        ध्रुव int32_t *p32= res==0 ? &g_रिक्तानि_String.length : p_Res_दत्त->p_मूलम्+res;
        length=*p32++;
        p=(ध्रुव char16_t *)p32;
    } अन्यथा {
        p=शून्यसूचकः;
        length=0;
    }
    यदि(p_दीर्घता) {
        *p_दीर्घता=length;
    }
    प्रत्यागम p;
}

नामाकाश {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ध्रुव Resource_दत्त *p_Res_दत्त, Resource res) {
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(res);
    यदि (विस्थापनम् == 0) {
        // empty string
    } अन्यथा यदि (res == विस्थापनम्) {
        ध्रुव int32_t *p32=p_Res_दत्त->p_मूलम्+res;
        int32_t length=*p32;
        ध्रुव char16_t* p = reinterpret_cast<ध्रुव char16_t*>(p32);
        प्रत्यागम length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } अन्यथा यदि (RES_GET_TYPE(res) == URES_STRING_V2) {
        ध्रुव char16_t *p;
        यदि (static_cast<int32_t>(विस्थापनम्) < p_Res_दत्त->pool_String_सूचकांक_सीमा) {
            p = reinterpret_cast<ध्रुव char16_t*>(p_Res_दत्त->pool_Bundle_वाक्यानि) + विस्थापनम्;
        } अन्यथा {
            p = reinterpret_cast<ध्रुव char16_t*>(p_Res_दत्त->p16BitUnits) + (विस्थापनम् - p_Res_दत्त->pool_String_सूचकांक_सीमा);
        }
        int32_t first=*p;
        यदि (first == 0x2205) {  // implicit length
            प्रत्यागम p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } अन्यथा यदि (first == 0xdc03) {  // explicit length 3 (should not occur)
            प्रत्यागम p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } अन्यथा {
            // Assume that the string has not been stored with more length units than necessary.
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम असत्य;
}

int32_t getStringArray(ध्रुव Resource_दत्त *p_Res_दत्त, ध्रुव icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(dest == शून्यसूचकः ? capacity != 0 : capacity < 0) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    int32_t length = array.get_आकार();
    यदि(length == 0) {
        प्रत्यागम 0;
    }
    यदि(length > capacity) {
        error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम length;
    }
    प्रति(int32_t i = 0; i < length; ++i) {
        int32_t s_दीर्घता;
        // No tracing: handled by the caller
        ध्रुव char16_t *s = res_getStringNoTrace(p_Res_दत्त, array.आन्तरिकम्_Get_Resource(p_Res_दत्त, i), &s_दीर्घता);
        यदि(s == शून्यसूचकः) {
            error_संहिता = U_RESOURCE_TYPE_MISMATCH;
            प्रत्यागम 0;
        }
        dest[i].समुच्चयः_गन्तव्यम्(सत्य, s, s_दीर्घता);
    }
    प्रत्यागम length;
}

}  // namespace

U_CAPI ध्रुव char16_t * U_EXPORT2
res_getAlias(ध्रुव Resource_दत्त *p_Res_दत्त, Resource res, int32_t *p_दीर्घता) {
    ध्रुव char16_t *p;
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(res);
    int32_t length;
    यदि(RES_GET_TYPE(res)==URES_ALIAS) {
        ध्रुव int32_t *p32= विस्थापनम्==0 ? &g_रिक्तानि_String.length : p_Res_दत्त->p_मूलम्+विस्थापनम्;
        length=*p32++;
        p=(ध्रुव char16_t *)p32;
    } अन्यथा {
        p=शून्यसूचकः;
        length=0;
    }
    यदि(p_दीर्घता) {
        *p_दीर्घता=length;
    }
    प्रत्यागम p;
}

U_CAPI ध्रुव uint8_t * U_EXPORT2
res_get_द्विमान_No_Trace(ध्रुव Resource_दत्त *p_Res_दत्त, Resource res, int32_t *p_दीर्घता) {
    ध्रुव uint8_t *p;
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(res);
    int32_t length;
    यदि(RES_GET_TYPE(res)==URES_द्विमान) {
        ध्रुव int32_t *p32= विस्थापनम्==0 ? (ध्रुव int32_t*)&gEmpty32 : p_Res_दत्त->p_मूलम्+विस्थापनम्;
        length=*p32++;
        p=(ध्रुव uint8_t *)p32;
    } अन्यथा {
        p=शून्यसूचकः;
        length=0;
    }
    यदि(p_दीर्घता) {
        *p_दीर्घता=length;
    }
    प्रत्यागम p;
}


U_CAPI ध्रुव int32_t * U_EXPORT2
res_get_Int_सदिशम्_No_Trace(ध्रुव Resource_दत्त *p_Res_दत्त, Resource res, int32_t *p_दीर्घता) {
    ध्रुव int32_t *p;
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(res);
    int32_t length;
    यदि(RES_GET_TYPE(res)==URES_INT_सदिशम्) {
        p= विस्थापनम्==0 ? (ध्रुव int32_t *)&gEmpty32 : p_Res_दत्त->p_मूलम्+विस्थापनम्;
        length=*p++;
    } अन्यथा {
        p=शून्यसूचकः;
        length=0;
    }
    यदि(p_दीर्घता) {
        *p_दीर्घता=length;
    }
    प्रत्यागम p;
}

U_CAPI int32_t U_EXPORT2
res_गणनम्_Array_वस्तूनि(ध्रुव Resource_दत्त *p_Res_दत्त, Resource res) {
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(res);
    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_STRING:
    पक्ष URES_STRING_V2:
    पक्ष URES_द्विमान:
    पक्ष URES_ALIAS:
    पक्ष URES_INT:
    पक्ष URES_INT_सदिशम्:
        प्रत्यागम 1;
    पक्ष URES_ARRAY:
    पक्ष URES_TABLE32:
        प्रत्यागम विस्थापनम्==0 ? 0 : *(p_Res_दत्त->p_मूलम्+विस्थापनम्);
    पक्ष URES_सारणी:
        प्रत्यागम विस्थापनम्==0 ? 0 : *((ध्रुव uint16_t *)(p_Res_दत्त->p_मूलम्+विस्थापनम्));
    पक्ष URES_ARRAY16:
    पक्ष URES_TABLE16:
        प्रत्यागम p_Res_दत्त->p16BitUnits[विस्थापनम्];
    स्वभाव:
        प्रत्यागम 0;
    }
}

U_NAMESPACE_BEGIN

Resource_दत्त_मानम्::~Resource_दत्त_मानम्() {}

UResType Resource_दत्त_मानम्::getType() ध्रुव {
    प्रत्यागम res_get_लौकिकम्_Type(res);
}

ध्रुव char16_t *Resource_दत्त_मानम्::getString(int32_t &length, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव char16_t *s = res_getString(fTraceInfo, &get_दत्त(), res, &length);
    यदि(s == शून्यसूचकः) {
        error_संहिता = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम s;
}

ध्रुव char16_t *Resource_दत्त_मानम्::getAliasString(int32_t &length, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव char16_t *s = res_getAlias(&get_दत्त(), res, &length);
    यदि(s == शून्यसूचकः) {
        error_संहिता = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम s;
}

int32_t Resource_दत्त_मानम्::getInt(UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(RES_GET_TYPE(res) != URES_INT) {
        error_संहिता = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम res_getInt(fTraceInfo, res);
}

uint32_t Resource_दत्त_मानम्::getUInt(UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(RES_GET_TYPE(res) != URES_INT) {
        error_संहिता = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम res_getUInt(fTraceInfo, res);
}

ध्रुव int32_t *Resource_दत्त_मानम्::get_Int_सदिशम्(int32_t &length, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव int32_t *iv = res_get_Int_सदिशम्(fTraceInfo, &get_दत्त(), res, &length);
    यदि(iv == शून्यसूचकः) {
        error_संहिता = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम iv;
}

ध्रुव uint8_t *Resource_दत्त_मानम्::get_द्विमान(int32_t &length, UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव uint8_t *b = res_get_द्विमान(fTraceInfo, &get_दत्त(), res, &length);
    यदि(b == शून्यसूचकः) {
        error_संहिता = U_RESOURCE_TYPE_MISMATCH;
    }
    प्रत्यागम b;
}

ResourceArray Resource_दत्त_मानम्::getArray(UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम {};
    }
    ध्रुव uint16_t *items16 = शून्यसूचकः;
    ध्रुव Resource *items32 = शून्यसूचकः;
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(res);
    int32_t length = 0;
    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_ARRAY:
        यदि (विस्थापनम्!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ध्रुव Resource*>(get_दत्त().p_मूलम्) + विस्थापनम्;
            length = *items32++;
        }
        भङ्ग;
    पक्ष URES_ARRAY16:
        items16 = get_दत्त().p16BitUnits+विस्थापनम्;
        length = *items16++;
        भङ्ग;
    स्वभाव:
        error_संहिता = U_RESOURCE_TYPE_MISMATCH;
        प्रत्यागम {};
    }
    प्रत्यागम ResourceArray(items16, items32, length, fTraceInfo);
}

Resource_सारणी Resource_दत्त_मानम्::get_सारणी(UError_संहिता &error_संहिता) ध्रुव {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम {};
    }
    ध्रुव uint16_t *keys16 = शून्यसूचकः;
    ध्रुव int32_t *keys32 = शून्यसूचकः;
    ध्रुव uint16_t *items16 = शून्यसूचकः;
    ध्रुव Resource *items32 = शून्यसूचकः;
    uint32_t विस्थापनम् = RES_GET_विस्थापनम्(res);
    int32_t length = 0;
    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_सारणी:
        यदि (विस्थापनम् != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ध्रुव uint16_t*>(get_दत्त().p_मूलम् + विस्थापनम्);
            length = *keys16++;
            items32 = reinterpret_cast<ध्रुव Resource*>(keys16 + length + (~length & 1));
        }
        भङ्ग;
    पक्ष URES_TABLE16:
        keys16 = get_दत्त().p16BitUnits+विस्थापनम्;
        length = *keys16++;
        items16 = keys16 + length;
        भङ्ग;
    पक्ष URES_TABLE32:
        यदि (विस्थापनम् != 0) {  // empty if offset==0
            keys32 = get_दत्त().p_मूलम्+विस्थापनम्;
            length = *keys32++;
            items32 = reinterpret_cast<ध्रुव Resource*>(keys32) + length;
        }
        भङ्ग;
    स्वभाव:
        error_संहिता = U_RESOURCE_TYPE_MISMATCH;
        प्रत्यागम {};
    }
    प्रत्यागम Resource_सारणी(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_दत्त_मानम्::isNoInheritanceMarker() ध्रुव {
    प्रत्यागम ::isNoInheritanceMarker(&get_दत्त(), res);
}

int32_t Resource_दत्त_मानम्::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UError_संहिता &error_संहिता) ध्रुव {
    प्रत्यागम ::getStringArray(&get_दत्त(), getArray(error_संहिता), dest, capacity, error_संहिता);
}

int32_t Resource_दत्त_मानम्::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UError_संहिता &error_संहिता) ध्रुव {
    यदि(URES_IS_ARRAY(res)) {
        प्रत्यागम ::getStringArray(&get_दत्त(), getArray(error_संहिता), dest, capacity, error_संहिता);
    }
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(dest == शून्यसूचकः ? capacity != 0 : capacity < 0) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    यदि(capacity < 1) {
        error_संहिता = U_बफर_अतिप्रवाहः_ERROR;
        प्रत्यागम 1;
    }
    int32_t s_दीर्घता;
    ध्रुव char16_t *s = res_getString(fTraceInfo, &get_दत्त(), res, &s_दीर्घता);
    यदि(s != शून्यसूचकः) {
        dest[0].समुच्चयः_गन्तव्यम्(सत्य, s, s_दीर्घता);
        प्रत्यागम 1;
    }
    error_संहिता = U_RESOURCE_TYPE_MISMATCH;
    प्रत्यागम 0;
}

UnicodeString Resource_दत्त_मानम्::get_String_Or_First_सम्बन्धि_Array(UError_संहिता &error_संहिता) ध्रुव {
    UnicodeString us;
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम us;
    }
    int32_t s_दीर्घता;
    ध्रुव char16_t *s = res_getString(fTraceInfo, &get_दत्त(), res, &s_दीर्घता);
    यदि(s != शून्यसूचकः) {
        us.समुच्चयः_गन्तव्यम्(सत्य, s, s_दीर्घता);
        प्रत्यागम us;
    }
    ResourceArray array = getArray(error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम us;
    }
    यदि(array.get_आकार() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_दत्त(), array.आन्तरिकम्_Get_Resource(&get_दत्त(), 0), &s_दीर्घता);
        यदि(s != शून्यसूचकः) {
            us.समुच्चयः_गन्तव्यम्(सत्य, s, s_दीर्घता);
            प्रत्यागम us;
        }
    }
    error_संहिता = U_RESOURCE_TYPE_MISMATCH;
    प्रत्यागम us;
}

U_NAMESPACE_अन्तः

स्थिर Resource
makeResourceFrom16(ध्रुव Resource_दत्त *p_Res_दत्त, int32_t res16) {
    यदि(res16<p_Res_दत्त->pool_String_Index16_सीमा) {
        // Pool string, nothing to do.
    } अन्यथा {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_दत्त->pool_String_Index16_सीमा+p_Res_दत्त->pool_String_सूचकांक_सीमा;
    }
    प्रत्यागम URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_सारणी_Item_By_कुञ्चिका(ध्रुव Resource_दत्त *p_Res_दत्त, Resource सारणी,
                      int32_t *सूचकांक_R, ध्रुव वर्ण **कुञ्चिका) {
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(सारणी);
    int32_t length;
    int32_t idx;
    यदि(कुञ्चिका == शून्यसूचकः || *कुञ्चिका == शून्यसूचकः) {
        प्रत्यागम RES_BOGUS;
    }
    विकल्प(RES_GET_TYPE(सारणी)) {
    पक्ष URES_सारणी: {
        यदि (विस्थापनम्!=0) { /* empty if offset==0 */
            ध्रुव uint16_t *p= (ध्रुव uint16_t *)(p_Res_दत्त->p_मूलम्+विस्थापनम्);
            length=*p++;
            *सूचकांक_R=idx=res_find_सारणी_Item(p_Res_दत्त, p, length, *कुञ्चिका, कुञ्चिका);
            यदि(idx>=0) {
                ध्रुव Resource *p32=(ध्रुव Resource *)(p+length+(~length&1));
                प्रत्यागम p32[idx];
            }
        }
        भङ्ग;
    }
    पक्ष URES_TABLE16: {
        ध्रुव uint16_t *p=p_Res_दत्त->p16BitUnits+विस्थापनम्;
        length=*p++;
        *सूचकांक_R=idx=res_find_सारणी_Item(p_Res_दत्त, p, length, *कुञ्चिका, कुञ्चिका);
        यदि(idx>=0) {
            प्रत्यागम makeResourceFrom16(p_Res_दत्त, p[length+idx]);
        }
        भङ्ग;
    }
    पक्ष URES_TABLE32: {
        यदि (विस्थापनम्!=0) { /* empty if offset==0 */
            ध्रुव int32_t *p= p_Res_दत्त->p_मूलम्+विस्थापनम्;
            length=*p++;
            *सूचकांक_R=idx=_res_findTable32Item(p_Res_दत्त, p, length, *कुञ्चिका, कुञ्चिका);
            यदि(idx>=0) {
                प्रत्यागम (Resource)p[length+idx];
            }
        }
        भङ्ग;
    }
    स्वभाव:
        भङ्ग;
    }
    प्रत्यागम RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_get_सारणी_Item_By_सूचकांक(ध्रुव Resource_दत्त *p_Res_दत्त, Resource सारणी,
                        int32_t सूचकांक_R, ध्रुव वर्ण **कुञ्चिका) {
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(सारणी);
    int32_t length;
    यदि (सूचकांक_R < 0) {
        प्रत्यागम RES_BOGUS;
    }
    विकल्प(RES_GET_TYPE(सारणी)) {
    पक्ष URES_सारणी: {
        यदि (विस्थापनम् != 0) { /* empty if offset==0 */
            ध्रुव uint16_t *p= (ध्रुव uint16_t *)(p_Res_दत्त->p_मूलम्+विस्थापनम्);
            length=*p++;
            यदि(सूचकांक_R<length) {
                ध्रुव Resource *p32=(ध्रुव Resource *)(p+length+(~length&1));
                यदि(कुञ्चिका!=शून्यसूचकः) {
                    *कुञ्चिका=RES_GET_KEY16(p_Res_दत्त, p[सूचकांक_R]);
                }
                प्रत्यागम p32[सूचकांक_R];
            }
        }
        भङ्ग;
    }
    पक्ष URES_TABLE16: {
        ध्रुव uint16_t *p=p_Res_दत्त->p16BitUnits+विस्थापनम्;
        length=*p++;
        यदि(सूचकांक_R<length) {
            यदि(कुञ्चिका!=शून्यसूचकः) {
                *कुञ्चिका=RES_GET_KEY16(p_Res_दत्त, p[सूचकांक_R]);
            }
            प्रत्यागम makeResourceFrom16(p_Res_दत्त, p[length+सूचकांक_R]);
        }
        भङ्ग;
    }
    पक्ष URES_TABLE32: {
        यदि (विस्थापनम् != 0) { /* empty if offset==0 */
            ध्रुव int32_t *p= p_Res_दत्त->p_मूलम्+विस्थापनम्;
            length=*p++;
            यदि(सूचकांक_R<length) {
                यदि(कुञ्चिका!=शून्यसूचकः) {
                    *कुञ्चिका=RES_GET_KEY32(p_Res_दत्त, p[सूचकांक_R]);
                }
                प्रत्यागम (Resource)p[length+सूचकांक_R];
            }
        }
        भङ्ग;
    }
    स्वभाव:
        भङ्ग;
    }
    प्रत्यागम RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ध्रुव Resource_दत्त *p_Res_दत्त, ध्रुव वर्ण *कुञ्चिका) {
    ध्रुव वर्ण *real_कुञ्चिका=कुञ्चिका;
    int32_t idx;
    प्रत्यागम res_get_सारणी_Item_By_कुञ्चिका(p_Res_दत्त, p_Res_दत्त->मूलम्_Res, &idx, &real_कुञ्चिका);
}


UBool icu::Resource_सारणी::get_कुञ्चिका_And_मानम्(int32_t i,
                                         ध्रुव वर्ण *&कुञ्चिका, icu::Resource_मानम् &value) ध्रुव {
    यदि(0 <= i && i < length) {
        icu::Resource_दत्त_मानम् &rd_मानम् = static_cast<icu::Resource_दत्त_मानम् &>(value);
        यदि (keys16 != शून्यसूचकः) {
            कुञ्चिका = RES_GET_KEY16(&rd_मानम्.get_दत्त(), keys16[i]);
        } अन्यथा {
            कुञ्चिका = RES_GET_KEY32(&rd_मानम्.get_दत्त(), keys32[i]);
        }
        Resource res;
        यदि (items16 != शून्यसूचकः) {
            res = makeResourceFrom16(&rd_मानम्.get_दत्त(), items16[i]);
        } अन्यथा {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_मानम्.समुच्चयः_Resource(res, ResourceTracer(fTraceInfo, कुञ्चिका));
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

UBool icu::Resource_सारणी::find_मानम्(ध्रुव वर्ण *कुञ्चिका, Resource_मानम् &value) ध्रुव {
    icu::Resource_दत्त_मानम् &rd_मानम् = static_cast<icu::Resource_दत्त_मानम् &>(value);
    ध्रुव वर्ण *real_कुञ्चिका = शून्यसूचकः;
    int32_t i;
    यदि (keys16 != शून्यसूचकः) {
        i = res_find_सारणी_Item(&rd_मानम्.get_दत्त(), keys16, length, कुञ्चिका, &real_कुञ्चिका);
    } अन्यथा {
        i = _res_findTable32Item(&rd_मानम्.get_दत्त(), keys32, length, कुञ्चिका, &real_कुञ्चिका);
    }
    यदि (i >= 0) {
        Resource res;
        यदि (items16 != शून्यसूचकः) {
            res = makeResourceFrom16(&rd_मानम्.get_दत्त(), items16[i]);
        } अन्यथा {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_मानम्.समुच्चयः_Resource(res, ResourceTracer(fTraceInfo, कुञ्चिका));
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

U_CAPI Resource U_EXPORT2
res_getArrayItem(ध्रुव Resource_दत्त *p_Res_दत्त, Resource array, int32_t सूचकांक_R) {
    uint32_t विस्थापनम्=RES_GET_विस्थापनम्(array);
    यदि (सूचकांक_R < 0) {
        प्रत्यागम RES_BOGUS;
    }
    विकल्प(RES_GET_TYPE(array)) {
    पक्ष URES_ARRAY: {
        यदि (विस्थापनम्!=0) { /* empty if offset==0 */
            ध्रुव int32_t *p= p_Res_दत्त->p_मूलम्+विस्थापनम्;
            यदि(सूचकांक_R<*p) {
                प्रत्यागम (Resource)p[1+सूचकांक_R];
            }
        }
        भङ्ग;
    }
    पक्ष URES_ARRAY16: {
        ध्रुव uint16_t *p=p_Res_दत्त->p16BitUnits+विस्थापनम्;
        यदि(सूचकांक_R<*p) {
            प्रत्यागम makeResourceFrom16(p_Res_दत्त, p[1+सूचकांक_R]);
        }
        भङ्ग;
    }
    स्वभाव:
        भङ्ग;
    }
    प्रत्यागम RES_BOGUS;
}

uint32_t icu::ResourceArray::आन्तरिकम्_Get_Resource(ध्रुव Resource_दत्त *p_Res_दत्त, int32_t i) ध्रुव {
    यदि (items16 != शून्यसूचकः) {
        प्रत्यागम makeResourceFrom16(p_Res_दत्त, items16[i]);
    } अन्यथा {
        प्रत्यागम items32[i];
    }
}

UBool icu::ResourceArray::get_मानम्(int32_t i, icu::Resource_मानम् &value) ध्रुव {
    यदि(0 <= i && i < length) {
        icu::Resource_दत्त_मानम् &rd_मानम् = static_cast<icu::Resource_दत्त_मानम् &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_मानम्.समुच्चयः_Resource(
            आन्तरिकम्_Get_Resource(&rd_मानम्.get_दत्त(), i),
            ResourceTracer(fTraceInfo, i));
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

U_CFUNC Resource
res_findResource(ध्रुव Resource_दत्त *p_Res_दत्त, Resource r, वर्ण** मार्गः, ध्रुव वर्ण** कुञ्चिका) {
  वर्ण *मार्गः_P = *मार्गः, *अग्र_Sep_P = *मार्गः;
  वर्ण *close_सूचकांक = शून्यसूचकः;
  Resource t1 = r;
  Resource t2;
  int32_t सूचकांक_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  यदि(!uprv_strlen(मार्गः_P)) {
      प्रत्यागम r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  यदि(!URES_IS_CONTAINER(type)) {
      प्रत्यागम RES_BOGUS;
  }
  
  यावत्(अग्र_Sep_P && *मार्गः_P && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    अग्र_Sep_P = uprv_strchr(मार्गः_P, RES_मार्गः_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    यदि(अग्र_Sep_P != शून्यसूचकः) {
      यदि(अग्र_Sep_P == मार्गः_P) {
        // Empty key string.
        प्रत्यागम RES_BOGUS;
      }
      *अग्र_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *मार्गः = अग्र_Sep_P+1;
    } अन्यथा {
      *मार्गः = uprv_strchr(मार्गः_P, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    यदि(URES_IS_सारणी(type)) {
      *कुञ्चिका = मार्गः_P;
      t2 = res_get_सारणी_Item_By_कुञ्चिका(p_Res_दत्त, t1, &सूचकांक_R, कुञ्चिका);
    } अन्यथा यदि(URES_IS_ARRAY(type)) {
      सूचकांक_R = uprv_strtol(मार्गः_P, &close_सूचकांक, 10);
      यदि(सूचकांक_R >= 0 && *close_सूचकांक == 0) {
        t2 = res_getArrayItem(p_Res_दत्त, t1, सूचकांक_R);
      } अन्यथा {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *कुञ्चिका = शून्यसूचकः;
    } अन्यथा { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    मार्गः_P = *मार्गः;
  }

  प्रत्यागम t1;
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
    int32_t कुञ्चिका_सूचकांक, क्रमय_सूचकांक;
} Row;

स्थिर int32_t U_CALLCONV
ures_compare_पङ्क्तिकाः(ध्रुव शून्य *context, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    ध्रुव वर्ण* कुञ्चिका_Chars = static_cast<ध्रुव वर्ण*>(context);
    प्रत्यागम static_cast<int32_t>(uprv_strcmp(कुञ्चिका_Chars + static_cast<ध्रुव Row*>(वामतः)->कुञ्चिका_सूचकांक,
                                            कुञ्चिका_Chars + static_cast<ध्रुव Row*>(दक्षिणतः)->कुञ्चिका_सूचकांक));
}

प्रकारनिर्देश संरचना अस्थायी_सारणी_2 {
    ध्रुव वर्ण *कुञ्चिका_Chars;
    Row *पङ्क्तिकाः;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_कुञ्चिका_सीमा;
    uint8_t major_Format_संस्करणम्;
} अस्थायी_सारणी_2;

गणना {
    राशिः_ROW_क्षमता=200
};

/* The table item key string is not locally available. */
स्थिर ध्रुव वर्ण *ध्रुव g_Unknown_कुञ्चिका="";

#यदि !UCONFIG_NO_COLLATION
// resource table key for collation binaries
स्थिर ध्रुव char16_t g_Collation_Bin_कुञ्चिका[]=u"%%CollationBin";
#यदि_अन्तः

/*
 * swap one resource item
 */
स्थिर शून्य
ures_swapResource(ध्रुव UDataSwapper *ds,
                  ध्रुव Resource *अन्तर्_Bundle, Resource *बहिर्गमनम्_Bundle,
                  Resource res, /* caller swaps res itself */
                  ध्रुव वर्ण *कुञ्चिका,
                  अस्थायी_सारणी_2 *p_अस्थायी_सारणी,
                  UError_संहिता *p_Error_संहिता) {
    ध्रुव Resource *p;
    Resource *q;
    int32_t विस्थापनम्, count;

    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_TABLE16:
    पक्ष URES_STRING_V2:
    पक्ष URES_INT:
    पक्ष URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        प्रत्यागम;
    स्वभाव:
        भङ्ग;
    }

    /* all other types use an offset to point to their data */
    विस्थापनम् = static_cast<int32_t>(RES_GET_विस्थापनम्(res));
    यदि(विस्थापनम्==0) {
        /* special offset indicating an empty item */
        प्रत्यागम;
    }
    यदि (p_अस्थायी_सारणी->resFlags[विस्थापनम् >> 5] & (static_cast<uint32_t>(1) << (विस्थापनम् & 0x1f))) {
        /* we already swapped this resource item */
        प्रत्यागम;
    } अन्यथा {
        /* mark it as swapped now */
        p_अस्थायी_सारणी->resFlags[विस्थापनम् >> 5] |= static_cast<uint32_t>(1) << (विस्थापनम् & 0x1f);
    }

    p=अन्तर्_Bundle+विस्थापनम्;
    q=बहिर्गमनम्_Bundle+विस्थापनम्;

    विकल्प(RES_GET_TYPE(res)) {
    पक्ष URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    पक्ष URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, p_Error_संहिता);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->swapArray16(ds, p+1, 2*count, q+1, p_Error_संहिता);
        भङ्ग;
    पक्ष URES_द्विमान:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->swapArray32(ds, p, 4, q, p_Error_संहिता);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#यदि !UCONFIG_NO_COLLATION
        यदि( कुञ्चिका!=शून्यसूचकः &&  /* the binary is in a table */
            (कुञ्चिका!=g_Unknown_कुञ्चिका ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, कुञ्चिका, -1,
                                       g_Collation_Bin_कुञ्चिका, UPRV_LENGTHOF(g_Collation_Bin_कुञ्चिका)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_द्विमान(ds, p+1, count))
        ) {
            ucol_swap(ds, p+1, count, q+1, p_Error_संहिता);
        }
#यदि_अन्तः
        भङ्ग;
    पक्ष URES_सारणी:
    पक्ष URES_TABLE32:
        {
            ध्रुव uint16_t *pKey16;
            uint16_t *qKey16;

            ध्रुव int32_t *pKey32;
            int32_t *qKey32;

            Resource item;
            int32_t i, old_सूचकांक;

            यदि(RES_GET_TYPE(res)==URES_सारणी) {
                /* get table item count */
                pKey16 = reinterpret_cast<ध्रुव uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=शून्यसूचकः;

                /* swap count */
                ds->swapArray16(ds, pKey16++, 2, qKey16++, p_Error_संहिता);

                विस्थापनम्+=((1+count)+1)/2;
            } अन्यथा {
                /* get table item count */
                pKey32 = reinterpret_cast<ध्रुव int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=शून्यसूचकः;

                /* swap count */
                ds->swapArray32(ds, pKey32++, 4, qKey32++, p_Error_संहिता);

                विस्थापनम्+=1+count;
            }

            यदि(count==0) {
                भङ्ग;
            }

            p=अन्तर्_Bundle+विस्थापनम्; /* pointer to table resources */
            q=बहिर्गमनम्_Bundle+विस्थापनम्;

            /* recurse */
            प्रति(i=0; i<count; ++i) {
                ध्रुव वर्ण *item_कुञ्चिका=g_Unknown_कुञ्चिका;
                यदि(pKey16!=शून्यसूचकः) {
                    int32_t कुञ्चिका_विस्थापनम्=ds->readUInt16(pKey16[i]);
                    यदि(कुञ्चिका_विस्थापनम्<p_अस्थायी_सारणी->local_कुञ्चिका_सीमा) {
                        item_कुञ्चिका = reinterpret_cast<ध्रुव वर्ण*>(बहिर्गमनम्_Bundle) + कुञ्चिका_विस्थापनम्;
                    }
                } अन्यथा {
                    int32_t कुञ्चिका_विस्थापनम्=udata_readInt32(ds, pKey32[i]);
                    यदि(कुञ्चिका_विस्थापनम्>=0) {
                        item_कुञ्चिका = reinterpret_cast<ध्रुव वर्ण*>(बहिर्गमनम्_Bundle) + कुञ्चिका_विस्थापनम्;
                    }
                }
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, अन्तर्_Bundle, बहिर्गमनम्_Bundle, item, item_कुञ्चिका, p_अस्थायी_सारणी, p_Error_संहिता);
                यदि(U_FAILURE(*p_Error_संहिता)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    प्रत्यागम;
                }
            }

            यदि(p_अस्थायी_सारणी->major_Format_संस्करणम्>1 || ds->अन्तर्_Charset==ds->बहिर्गमनम्_Charset) {
                /* no need to sort, just swap the offset/value arrays */
                यदि(pKey16!=शून्यसूचकः) {
                    ds->swapArray16(ds, pKey16, count*2, qKey16, p_Error_संहिता);
                    ds->swapArray32(ds, p, count*4, q, p_Error_संहिता);
                } अन्यथा {
                    /* swap key offsets and items as one array */
                    ds->swapArray32(ds, pKey32, count*2*4, qKey32, p_Error_संहिता);
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
            यदि(pKey16!=शून्यसूचकः) {
                प्रति(i=0; i<count; ++i) {
                    p_अस्थायी_सारणी->पङ्क्तिकाः[i].कुञ्चिका_सूचकांक=ds->readUInt16(pKey16[i]);
                    p_अस्थायी_सारणी->पङ्क्तिकाः[i].क्रमय_सूचकांक=i;
                }
            } अन्यथा {
                प्रति(i=0; i<count; ++i) {
                    p_अस्थायी_सारणी->पङ्क्तिकाः[i].कुञ्चिका_सूचकांक=udata_readInt32(ds, pKey32[i]);
                    p_अस्थायी_सारणी->पङ्क्तिकाः[i].क्रमय_सूचकांक=i;
                }
            }
            uprv_क्रमय_Array(p_अस्थायी_सारणी->पङ्क्तिकाः, count, परिमाण(Row),
                           ures_compare_पङ्क्तिकाः, p_अस्थायी_सारणी->कुञ्चिका_Chars,
                           असत्य, p_Error_संहिता);
            यदि(U_FAILURE(*p_Error_संहिता)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                प्रत्यागम;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*कुञ्चिकाः*/
            यदि(pKey16!=शून्यसूचकः) {
                uint16_t *rKey16;

                यदि(pKey16!=qKey16) {
                    rKey16=qKey16;
                } अन्यथा {
                    rKey16 = reinterpret_cast<uint16_t*>(p_अस्थायी_सारणी->resort);
                }
                प्रति(i=0; i<count; ++i) {
                    old_सूचकांक=p_अस्थायी_सारणी->पङ्क्तिकाः[i].क्रमय_सूचकांक;
                    ds->swapArray16(ds, pKey16+old_सूचकांक, 2, rKey16+i, p_Error_संहिता);
                }
                यदि(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } अन्यथा {
                int32_t *rKey32;

                यदि(pKey32!=qKey32) {
                    rKey32=qKey32;
                } अन्यथा {
                    rKey32=p_अस्थायी_सारणी->resort;
                }
                प्रति(i=0; i<count; ++i) {
                    old_सूचकांक=p_अस्थायी_सारणी->पङ्क्तिकाः[i].क्रमय_सूचकांक;
                    ds->swapArray32(ds, pKey32+old_सूचकांक, 4, rKey32+i, p_Error_संहिता);
                }
                यदि(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                यदि(p!=q) {
                    r=q;
                } अन्यथा {
                    r = reinterpret_cast<Resource*>(p_अस्थायी_सारणी->resort);
                }
                प्रति(i=0; i<count; ++i) {
                    old_सूचकांक=p_अस्थायी_सारणी->पङ्क्तिकाः[i].क्रमय_सूचकांक;
                    ds->swapArray32(ds, p+old_सूचकांक, 4, r+i, p_Error_संहिता);
                }
                यदि(q!=r) {
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
            ds->swapArray32(ds, p++, 4, q++, p_Error_संहिता);

            /* recurse */
            प्रति(i=0; i<count; ++i) {
                item=ds->readUInt32(p[i]);
                ures_swapResource(ds, अन्तर्_Bundle, बहिर्गमनम्_Bundle, item, शून्यसूचकः, p_अस्थायी_सारणी, p_Error_संहिता);
                यदि(U_FAILURE(*p_Error_संहिता)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, item);
                    प्रत्यागम;
                }
            }

            /* swap items */
            ds->swapArray32(ds, p, 4*count, q, p_Error_संहिता);
        }
        भङ्ग;
    पक्ष URES_INT_सदिशम्:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->swapArray32(ds, p, 4*(1+count), q, p_Error_संहिता);
        भङ्ग;
    स्वभाव:
        /* also catches RES_BOGUS */
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        भङ्ग;
    }
}

U_CAPI int32_t U_EXPORT2
ures_swap(ध्रुव UDataSwapper *ds,
          ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
          UError_संहिता *p_Error_संहिता) {
    ध्रुव UDataInfo *pInfo;
    ध्रुव Resource *अन्तर्_Bundle;
    Resource मूलम्_Res;
    int32_t शीर्षकम्_आकार, max_सारणी_दीर्घता;

    Row पङ्क्तिकाः[राशिः_ROW_क्षमता];
    int32_t resort[राशिः_ROW_क्षमता];
    अस्थायी_सारणी_2 अस्थायी_सारणी;

    ध्रुव int32_t *अन्तर्_Indexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_दीर्घता, सूचकांक_दीर्घता, कुञ्चिकाः_Bottom, कुञ्चिकाः_शिखरम्, resBottom, शिखरम्;

    /* udata_swapDataHeader checks the arguments */
    शीर्षकम्_आकार=udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
    यदि(!(
        pInfo->दत्त_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->दत्त_Format[1]==0x65 &&
        pInfo->दत्त_Format[2]==0x73 &&
        pInfo->दत्त_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->format_संस्करणम्[0]==1 && pInfo->format_संस्करणम्[1]>=1) ||
            pInfo->format_संस्करणम्[0]==2 || pInfo->format_संस्करणम्[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->दत्त_Format[0], pInfo->दत्त_Format[1],
                         pInfo->दत्त_Format[2], pInfo->दत्त_Format[3],
                         pInfo->format_संस्करणम्[0], pInfo->format_संस्करणम्[1]);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }
    अस्थायी_सारणी.major_Format_संस्करणम्=pInfo->format_संस्करणम्[0];

    /* a resource bundle must contain at least one resource item */
    यदि(length<0) {
        bundle_दीर्घता=-1;
    } अन्यथा {
        bundle_दीर्घता=(length-शीर्षकम्_आकार)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        यदि(bundle_दीर्घता<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-शीर्षकम्_आकार);
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    अन्तर्_Bundle=(ध्रुव Resource *)((ध्रुव वर्ण *)अन्तर्_दत्त+शीर्षकम्_आकार);
    मूलम्_Res=ds->readUInt32(*अन्तर्_Bundle);

    /* formatVersion 1.1 adds the indexes[] array */
    अन्तर्_Indexes=(ध्रुव int32_t *)(अन्तर्_Bundle+1);

    सूचकांक_दीर्घता=udata_readInt32(ds, अन्तर्_Indexes[URES_सूचकांक_दीर्घता])&0xff;
    यदि(सूचकांक_दीर्घता<=URES_सूचकांक_MAX_सारणी_दीर्घता) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    कुञ्चिकाः_Bottom=1+सूचकांक_दीर्घता;
    कुञ्चिकाः_शिखरम्=udata_readInt32(ds, अन्तर्_Indexes[URES_सूचकांक_कुञ्चिकाः_शिखरम्]);
    यदि(सूचकांक_दीर्घता>URES_सूचकांक_16_BIT_शिखरम्) {
        resBottom=udata_readInt32(ds, अन्तर्_Indexes[URES_सूचकांक_16_BIT_शिखरम्]);
    } अन्यथा {
        resBottom=कुञ्चिकाः_शिखरम्;
    }
    शिखरम्=udata_readInt32(ds, अन्तर्_Indexes[URES_सूचकांक_BUNDLE_शिखरम्]);
    max_सारणी_दीर्घता=udata_readInt32(ds, अन्तर्_Indexes[URES_सूचकांक_MAX_सारणी_दीर्घता]);

    यदि(0<=bundle_दीर्घता && bundle_दीर्घता<शिखरम्) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         शिखरम्, bundle_दीर्घता);
        *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    यदि(कुञ्चिकाः_शिखरम्>(1+सूचकांक_दीर्घता)) {
        अस्थायी_सारणी.local_कुञ्चिका_सीमा=कुञ्चिकाः_शिखरम्<<2;
    } अन्यथा {
        अस्थायी_सारणी.local_कुञ्चिका_सीमा=0;
    }

    यदि(length>=0) {
        Resource *बहिर्गमनम्_Bundle=(Resource *)((वर्ण *)बहिर्गमनम्_दत्त+शीर्षकम्_आकार);

        /* track which resources we have already swapped */
        uint32_t राशिः_Res_Flags[राशिः_ROW_क्षमता];
        int32_t res_Flags_दीर्घता;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_दीर्घता=(length+31)>>5;          /* number of bytes needed */
        res_Flags_दीर्घता=(res_Flags_दीर्घता+3)&~3;   /* multiple of 4 bytes for uint32_t */
        यदि(res_Flags_दीर्घता<=(int32_t)परिमाण(राशिः_Res_Flags)) {
            अस्थायी_सारणी.resFlags=राशिः_Res_Flags;
        } अन्यथा {
            अस्थायी_सारणी.resFlags=(uint32_t *)uprv_malloc(res_Flags_दीर्घता);
            यदि(अस्थायी_सारणी.resFlags==शून्यसूचकः) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम 0;
            }
        }
        uprv_memset(अस्थायी_सारणी.resFlags, 0, res_Flags_दीर्घता);

        /* copy the bundle for binary and inaccessible data */
        यदि(अन्तर्_दत्त!=बहिर्गमनम्_दत्त) {
            uprv_memcpy(बहिर्गमनम्_Bundle, अन्तर्_Bundle, 4*शिखरम्);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_swap_Inv_String_खण्डम्(ds, अन्तर्_Bundle+कुञ्चिकाः_Bottom, 4*(कुञ्चिकाः_शिखरम्-कुञ्चिकाः_Bottom),
                                    बहिर्गमनम्_Bundle+कुञ्चिकाः_Bottom, p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(कुञ्चिकाः_शिखरम्-कुञ्चिकाः_Bottom));
            यदि(अस्थायी_सारणी.resFlags!=राशिः_Res_Flags) {
                uprv_free(अस्थायी_सारणी.resFlags);
            }
            प्रत्यागम 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        यदि(कुञ्चिकाः_शिखरम्<resBottom) {
            ds->swapArray16(ds, अन्तर्_Bundle+कुञ्चिकाः_शिखरम्, (resBottom-कुञ्चिकाः_शिखरम्)*4, बहिर्गमनम्_Bundle+कुञ्चिकाः_शिखरम्, p_Error_संहिता);
            यदि(U_FAILURE(*p_Error_संहिता)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-कुञ्चिकाः_शिखरम्));
                यदि(अस्थायी_सारणी.resFlags!=राशिः_Res_Flags) {
                    uprv_free(अस्थायी_सारणी.resFlags);
                }
                प्रत्यागम 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        अस्थायी_सारणी.कुञ्चिका_Chars=(ध्रुव वर्ण *)बहिर्गमनम्_Bundle; /* sort by outCharset */
        यदि(अस्थायी_सारणी.major_Format_संस्करणम्>1 || max_सारणी_दीर्घता<=राशिः_ROW_क्षमता) {
            अस्थायी_सारणी.पङ्क्तिकाः=पङ्क्तिकाः;
            अस्थायी_सारणी.resort=resort;
        } अन्यथा {
            अस्थायी_सारणी.पङ्क्तिकाः=(Row *)uprv_malloc(max_सारणी_दीर्घता*परिमाण(Row)+max_सारणी_दीर्घता*4);
            यदि(अस्थायी_सारणी.पङ्क्तिकाः==शून्यसूचकः) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_सारणी_दीर्घता);
                *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                यदि(अस्थायी_सारणी.resFlags!=राशिः_Res_Flags) {
                    uprv_free(अस्थायी_सारणी.resFlags);
                }
                प्रत्यागम 0;
            }
            अस्थायी_सारणी.resort=(int32_t *)(अस्थायी_सारणी.पङ्क्तिकाः+max_सारणी_दीर्घता);
        }

        /* swap the resources */
        ures_swapResource(ds, अन्तर्_Bundle, बहिर्गमनम्_Bundle, मूलम्_Res, शून्यसूचकः, &अस्थायी_सारणी, p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             मूलम्_Res);
        }

        यदि(अस्थायी_सारणी.पङ्क्तिकाः!=पङ्क्तिकाः) {
            uprv_free(अस्थायी_सारणी.पङ्क्तिकाः);
        }
        यदि(अस्थायी_सारणी.resFlags!=राशिः_Res_Flags) {
            uprv_free(अस्थायी_सारणी.resFlags);
        }

        /* swap the root resource and indexes */
        ds->swapArray32(ds, अन्तर्_Bundle, कुञ्चिकाः_Bottom*4, बहिर्गमनम्_Bundle, p_Error_संहिता);
    }

    प्रत्यागम शीर्षकम्_आकार+4*शिखरम्;
}

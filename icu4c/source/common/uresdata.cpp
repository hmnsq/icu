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
#ವ್ಯಾಖ್ಯಾನಿಸು RES_GET_KEY16(p_Res_ದತ್ತಾಂಶ, ಕೀಲಿ_ಸ್ಥಾನಾಂತರ) \
    ((ಕೀಲಿ_ಸ್ಥಾನಾಂತರ)<(p_Res_ದತ್ತಾಂಶ)->local_ಕೀಲಿ_ಎಲ್ಲೆ ? \
        (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(p_Res_ದತ್ತಾಂಶ)->p_ಬೇರು+(ಕೀಲಿ_ಸ್ಥಾನಾಂತರ) : \
        (p_Res_ದತ್ತಾಂಶ)->pool_Bundle_ಕೀಲಿಗಳು+(ಕೀಲಿ_ಸ್ಥಾನಾಂತರ)-(p_Res_ದತ್ತಾಂಶ)->local_ಕೀಲಿ_ಎಲ್ಲೆ)

#ವ್ಯಾಖ್ಯಾನಿಸು RES_GET_KEY32(p_Res_ದತ್ತಾಂಶ, ಕೀಲಿ_ಸ್ಥಾನಾಂತರ) \
    ((ಕೀಲಿ_ಸ್ಥಾನಾಂತರ)>=0 ? \
        (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(p_Res_ದತ್ತಾಂಶ)->p_ಬೇರು+(ಕೀಲಿ_ಸ್ಥಾನಾಂತರ) : \
        (p_Res_ದತ್ತಾಂಶ)->pool_Bundle_ಕೀಲಿಗಳು+((ಕೀಲಿ_ಸ್ಥಾನಾಂತರ)&0x7fffffff))

#ವ್ಯಾಖ್ಯಾನಿಸು URESDATA_ಅಂಶ_NOT_ಸಿಕ್ಕಿತು -1

/* empty resources, returned when the resource offset is 0 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t gEmpty16=0;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ {
    int32_t length;
    int32_t res;
} gEmpty32={ 0, 0 };

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ {
    int32_t length;
    char16_t nul;
    char16_t pad;
} g_ಖಾಲಿಗಳು_String={ 0, 0, 0 };

/*
 * All the type-access functions assume that
 * the resource is of the expected type.
 */

ಸ್ಥಿರ int32_t
res_ಕಂಡುಹಿಡಿ_ಕೋಷ್ಟಕ_ಅಂಶ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ uint16_t *ಕೀಲಿ_Offsets, int32_t length,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **real_ಕೀಲಿ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ_ಕೀಲಿ;
    int32_t mid, ಆರಂಭ, ಎಲ್ಲೆ;
    ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;

    /* do a binary search for the key */
    ಆರಂಭ=0;
    ಎಲ್ಲೆ=length;
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ) {
        mid = (ಆರಂಭ + ಎಲ್ಲೆ) / 2;
        ಕೋಷ್ಟಕ_ಕೀಲಿ = RES_GET_KEY16(p_Res_ದತ್ತಾಂಶ, ಕೀಲಿ_Offsets[mid]);
        ಆದರೆ (p_Res_ದತ್ತಾಂಶ->useNativeStrcmp) {
            ಫಲಿತಾಂಶ = uprv_strcmp(ಕೀಲಿ, ಕೋಷ್ಟಕ_ಕೀಲಿ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಫಲಿತಾಂಶ = uprv_compareInvCharsAsAscii(ಕೀಲಿ, ಕೋಷ್ಟಕ_ಕೀಲಿ);
        }
        ಆದರೆ (ಫಲಿತಾಂಶ < 0) {
            ಎಲ್ಲೆ = mid;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ > 0) {
            ಆರಂಭ = mid + 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* We found it! */
            *real_ಕೀಲಿ=ಕೋಷ್ಟಕ_ಕೀಲಿ;
            ಹಿಂತಿರುಗಿ mid;
        }
    }
    ಹಿಂತಿರುಗಿ URESDATA_ಅಂಶ_NOT_ಸಿಕ್ಕಿತು;  /* not found or table is empty. */
}

ಸ್ಥಿರ int32_t
res_ಕಂಡುಹಿಡಿ_Table32_ಅಂಶ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ int32_t *ಕೀಲಿ_Offsets, int32_t length,
                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **real_ಕೀಲಿ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೋಷ್ಟಕ_ಕೀಲಿ;
    int32_t mid, ಆರಂಭ, ಎಲ್ಲೆ;
    ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ;

    /* do a binary search for the key */
    ಆರಂಭ=0;
    ಎಲ್ಲೆ=length;
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ) {
        mid = (ಆರಂಭ + ಎಲ್ಲೆ) / 2;
        ಕೋಷ್ಟಕ_ಕೀಲಿ = RES_GET_KEY32(p_Res_ದತ್ತಾಂಶ, ಕೀಲಿ_Offsets[mid]);
        ಆದರೆ (p_Res_ದತ್ತಾಂಶ->useNativeStrcmp) {
            ಫಲಿತಾಂಶ = uprv_strcmp(ಕೀಲಿ, ಕೋಷ್ಟಕ_ಕೀಲಿ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಫಲಿತಾಂಶ = uprv_compareInvCharsAsAscii(ಕೀಲಿ, ಕೋಷ್ಟಕ_ಕೀಲಿ);
        }
        ಆದರೆ (ಫಲಿತಾಂಶ < 0) {
            ಎಲ್ಲೆ = mid;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ > 0) {
            ಆರಂಭ = mid + 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* We found it! */
            *real_ಕೀಲಿ=ಕೋಷ್ಟಕ_ಕೀಲಿ;
            ಹಿಂತಿರುಗಿ mid;
        }
    }
    ಹಿಂತಿರುಗಿ URESDATA_ಅಂಶ_NOT_ಸಿಕ್ಕಿತು;  /* not found or table is empty. */
}

/* helper for res_load() ---------------------------------------------------- */

ಸ್ಥಿರ UBool U_CALLCONV
isAcceptable(ಶೂನ್ಯ *context,
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*type*/, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*name*/,
             ಸ್ಥಿರಾಂಕ UDataInfo *pInfo) {
    uprv_memcpy(context, pInfo->format_ಆವೃತ್ತಿ, 4);
    ಹಿಂತಿರುಗಿ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->sizeofUChar==U_SIZEOF_UCHAR &&
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x65 &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x73 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x42 &&
        (1<=pInfo->format_ಆವೃತ್ತಿ[0] && pInfo->format_ಆವೃತ್ತಿ[0]<=3);
}

/* semi-public functions ---------------------------------------------------- */

ಸ್ಥಿರ ಶೂನ್ಯ
res_init(Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ,
         UVersionInfo format_ಆವೃತ್ತಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ಬೈಟುಗಳು, int32_t length,
         UError_ಸಂಕೇತ *error_ಸಂಕೇತ) {
    UResType ಬೇರು_Type;

    /* get the root resource */
    p_Res_ದತ್ತಾಂಶ->p_ಬೇರು = static_cast<ಸ್ಥಿರಾಂಕ int32_t*>(ಒಳ_ಬೈಟುಗಳು);
    p_Res_ದತ್ತಾಂಶ->ಬೇರು_Res = static_cast<Resource>(*p_Res_ದತ್ತಾಂಶ->p_ಬೇರು);
    p_Res_ದತ್ತಾಂಶ->p16_ಬಿಟ್_Units=&gEmpty16;

    /* formatVersion 1.1 must have a root item and at least 5 indexes */
    ಆದರೆ(length>=0 && (length/4)<((format_ಆವೃತ್ತಿ[0]==1 && format_ಆವೃತ್ತಿ[1]==0) ? 1 : 1+5)) {
        *error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_ದತ್ತಾಂಶ);
        ಹಿಂತಿರುಗಿ;
    }

    /* currently, we accept only resources that have a Table as their roots */
    ಬೇರು_Type = static_cast<UResType>(RES_GET_TYPE(p_Res_ದತ್ತಾಂಶ->ಬೇರು_Res));
    ಆದರೆ(!URES_IS_ಕೋಷ್ಟಕ(ಬೇರು_Type)) {
        *error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        res_unload(p_Res_ದತ್ತಾಂಶ);
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(format_ಆವೃತ್ತಿ[0]==1 && format_ಆವೃತ್ತಿ[1]==0) {
        p_Res_ದತ್ತಾಂಶ->local_ಕೀಲಿ_ಎಲ್ಲೆ=0x10000;  /* greater than any 16-bit key string offset */
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* bundles with formatVersion 1.1 and later contain an indexes[] array */
        ಸ್ಥಿರಾಂಕ int32_t *indexes=p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+1;
        int32_t ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=indexes[URES_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ]&0xff;
        ಆದರೆ(ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ<=URES_ಸೂಚ್ಯಂಕ_MAX_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ) {
            *error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_ದತ್ತಾಂಶ);
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ( length>=0 &&
            (length<((1+ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ)<<2) ||
             length<(indexes[URES_ಸೂಚ್ಯಂಕ_BUNDLE_ತುದಿ]<<2))
        ) {
            *error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_ದತ್ತಾಂಶ);
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ(indexes[URES_ಸೂಚ್ಯಂಕ_ಕೀಲಿಗಳು_ತುದಿ]>(1+ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ)) {
            p_Res_ದತ್ತಾಂಶ->local_ಕೀಲಿ_ಎಲ್ಲೆ=indexes[URES_ಸೂಚ್ಯಂಕ_ಕೀಲಿಗಳು_ತುದಿ]<<2;
        }
        ಆದರೆ(format_ಆವೃತ್ತಿ[0]>=3) {
            // In formatVersion 1, the indexLength took up this whole int.
            // In version 2, bits 31..8 were reserved and always 0.
            // In version 3, they contain bits 23..0 of the poolStringIndexLimit.
            // Bits 27..24 are in indexes[URES_INDEX_ATTRIBUTES] bits 15..12.
            p_Res_ದತ್ತಾಂಶ->pool_String_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ = static_cast<int32_t>(static_cast<uint32_t>(indexes[URES_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ]) >> 8);
        }
        ಆದರೆ(ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ>URES_ಸೂಚ್ಯಂಕ_ಗುಣಲಕ್ಷಣಗಳು) {
            int32_t att=indexes[URES_ಸೂಚ್ಯಂಕ_ಗುಣಲಕ್ಷಣಗಳು];
            p_Res_ದತ್ತಾಂಶ->noFallback = static_cast<UBool>(att & URES_ATT_NO_FALLBACK);
            p_Res_ದತ್ತಾಂಶ->isPoolBundle = static_cast<UBool>((att & URES_ATT_IS_POOL_BUNDLE) != 0);
            p_Res_ದತ್ತಾಂಶ->ಬಳಸುತ್ತದೆ_Pool_Bundle = static_cast<UBool>((att & URES_ATT_ಬಳಸುತ್ತದೆ_POOL_BUNDLE) != 0);
            p_Res_ದತ್ತಾಂಶ->pool_String_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ|=(att&0xf000)<<12;  // bits 15..12 -> 27..24
            p_Res_ದತ್ತಾಂಶ->pool_String_Index16_ಎಲ್ಲೆ = static_cast<int32_t>(static_cast<uint32_t>(att) >> 16);
        }
        ಆದರೆ((p_Res_ದತ್ತಾಂಶ->isPoolBundle || p_Res_ದತ್ತಾಂಶ->ಬಳಸುತ್ತದೆ_Pool_Bundle) && ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ<=URES_ಸೂಚ್ಯಂಕ_POOL_CHECKSUM) {
            *error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
            res_unload(p_Res_ದತ್ತಾಂಶ);
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ( ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ>URES_ಸೂಚ್ಯಂಕ_16_ಬಿಟ್_ತುದಿ &&
            indexes[URES_ಸೂಚ್ಯಂಕ_16_ಬಿಟ್_ತುದಿ]>indexes[URES_ಸೂಚ್ಯಂಕ_ಕೀಲಿಗಳು_ತುದಿ]
        ) {
            p_Res_ದತ್ತಾಂಶ->p16_ಬಿಟ್_Units = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(p_Res_ದತ್ತಾಂಶ->p_ಬೇರು + indexes[URES_ಸೂಚ್ಯಂಕ_ಕೀಲಿಗಳು_ತುದಿ]);
        }
    }

    ಆದರೆ(format_ಆವೃತ್ತಿ[0]==1 || U_CHARSET_FAMILY==U_ASCII_FAMILY) {
        /*
         * formatVersion 1: compare key strings in native-charset order
         * formatVersion 2 and up: compare key strings in ASCII order
         */
        p_Res_ದತ್ತಾಂಶ->useNativeStrcmp=ನಿಜ;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
res_read(Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ,
         ಸ್ಥಿರಾಂಕ UDataInfo *pInfo, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ಬೈಟುಗಳು, int32_t length,
         UError_ಸಂಕೇತ *error_ಸಂಕೇತ) {
    UVersionInfo format_ಆವೃತ್ತಿ;

    uprv_memset(p_Res_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(Resource_ದತ್ತಾಂಶ));
    ಆದರೆ(U_FAILURE(*error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(!isAcceptable(format_ಆವೃತ್ತಿ, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, pInfo)) {
        *error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    res_init(p_Res_ದತ್ತಾಂಶ, format_ಆವೃತ್ತಿ, ಒಳ_ಬೈಟುಗಳು, length, error_ಸಂಕೇತ);
}

U_CFUNC ಶೂನ್ಯ
res_ತುಂಬು(Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ,
         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, UError_ಸಂಕೇತ *error_ಸಂಕೇತ) {
    UVersionInfo format_ಆವೃತ್ತಿ;

    uprv_memset(p_Res_ದತ್ತಾಂಶ, 0, ಗಾತ್ರ(Resource_ದತ್ತಾಂಶ));

    /* load the ResourceBundle file */
    p_Res_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ=udata_openChoice(ಮಾರ್ಗ, "res", name, isAcceptable, format_ಆವೃತ್ತಿ, error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* get its memory and initialize *pResData */
    res_init(p_Res_ದತ್ತಾಂಶ, format_ಆವೃತ್ತಿ, udata_get_ಸ್ಮರಣೆ(p_Res_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ), -1, error_ಸಂಕೇತ);
}

U_CFUNC ಶೂನ್ಯ
res_unload(Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ) {
    ಆದರೆ(p_Res_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ!=ಶೂನ್ಯಸೂಚಕ) {
        udata_close(p_Res_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ);
        p_Res_ದತ್ತಾಂಶ->ದತ್ತಾಂಶ=ಶೂನ್ಯಸೂಚಕ;
    }
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t g_ಬಹಿರಂಗ_Types[URES_ಎಲ್ಲೆ] = {
    URES_STRING,
    URES_ದ್ವಿಮಾನ,
    URES_ಕೋಷ್ಟಕ,
    URES_ALIAS,

    URES_ಕೋಷ್ಟಕ,     /* URES_TABLE32 */
    URES_ಕೋಷ್ಟಕ,     /* URES_TABLE16 */
    URES_STRING,    /* URES_STRING_V2 */
    URES_INT,

    URES_ARRAY,
    URES_ARRAY,     /* URES_ARRAY16 */
    URES_NONE,
    URES_NONE,

    URES_NONE,
    URES_NONE,
    URES_INT_ಸದಿಶ,
    URES_NONE
};

U_CAPI UResType U_EXPORT2
res_get_ಬಹಿರಂಗ_Type(Resource res) {
    ಹಿಂತಿರುಗಿ (UResType)g_ಬಹಿರಂಗ_Types[RES_GET_TYPE(res)];
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t * U_EXPORT2
res_getStringNoTrace(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ char16_t *p;
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(res);
    int32_t length;
    ಆದರೆ(RES_GET_TYPE(res)==URES_STRING_V2) {
        int32_t first;
        ಆದರೆ((int32_t)ಸ್ಥಾನಾಂತರ<p_Res_ದತ್ತಾಂಶ->pool_String_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ) {
            p=(ಸ್ಥಿರಾಂಕ char16_t *)p_Res_ದತ್ತಾಂಶ->pool_Bundle_ಅಕ್ಷರಸರಣಿಗಳು+ಸ್ಥಾನಾಂತರ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            p=(ಸ್ಥಿರಾಂಕ char16_t *)p_Res_ದತ್ತಾಂಶ->p16_ಬಿಟ್_Units+(ಸ್ಥಾನಾಂತರ-p_Res_ದತ್ತಾಂಶ->pool_String_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ);
        }
        first=*p;
        ಆದರೆ(!U16_IS_TRAIL(first)) {
            length=u_strlen(p);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(first<0xdfef) {
            length=first&0x3ff;
            ++p;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(first<0xdfff) {
            length=((first-0xdfef)<<16)|p[1];
            p+=2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            length=((int32_t)p[1]<<16)|p[2];
            p+=3;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(res==ಸ್ಥಾನಾಂತರ) /* RES_GET_TYPE(res)==URES_STRING */ {
        ಸ್ಥಿರಾಂಕ int32_t *p32= res==0 ? &g_ಖಾಲಿಗಳು_String.length : p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+res;
        length=*p32++;
        p=(ಸ್ಥಿರಾಂಕ char16_t *)p32;
    } ಇಲ್ಲದಿದ್ದರೆ {
        p=ಶೂನ್ಯಸೂಚಕ;
        length=0;
    }
    ಆದರೆ(p_ಉದ್ದಳತೆ) {
        *p_ಉದ್ದಳತೆ=length;
    }
    ಹಿಂತಿರುಗಿ p;
}

ನಾಮಾಕಾಶ {

/**
 * CLDR string value (three empty-set symbols)=={2205, 2205, 2205}
 * prevents fallback to the parent bundle.
 * TODO: combine with other code that handles this marker, use EMPTY_SET constant.
 * TODO: maybe move to uresbund.cpp?
 */
UBool isNoInheritanceMarker(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res) {
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(res);
    ಆದರೆ (ಸ್ಥಾನಾಂತರ == 0) {
        // empty string
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (res == ಸ್ಥಾನಾಂತರ) {
        ಸ್ಥಿರಾಂಕ int32_t *p32=p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+res;
        int32_t length=*p32;
        ಸ್ಥಿರಾಂಕ char16_t* p = reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(p32);
        ಹಿಂತಿರುಗಿ length == 3 && p[2] == 0x2205 && p[3] == 0x2205 && p[4] == 0x2205;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (RES_GET_TYPE(res) == URES_STRING_V2) {
        ಸ್ಥಿರಾಂಕ char16_t *p;
        ಆದರೆ (static_cast<int32_t>(ಸ್ಥಾನಾಂತರ) < p_Res_ದತ್ತಾಂಶ->pool_String_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ) {
            p = reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(p_Res_ದತ್ತಾಂಶ->pool_Bundle_ಅಕ್ಷರಸರಣಿಗಳು) + ಸ್ಥಾನಾಂತರ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            p = reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(p_Res_ದತ್ತಾಂಶ->p16_ಬಿಟ್_Units) + (ಸ್ಥಾನಾಂತರ - p_Res_ದತ್ತಾಂಶ->pool_String_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ);
        }
        int32_t first=*p;
        ಆದರೆ (first == 0x2205) {  // implicit length
            ಹಿಂತಿರುಗಿ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (first == 0xdc03) {  // explicit length 3 (should not occur)
            ಹಿಂತಿರುಗಿ p[1] == 0x2205 && p[2] == 0x2205 && p[3] == 0x2205;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Assume that the string has not been stored with more length units than necessary.
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

int32_t getStringArray(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ icu::ResourceArray &array,
                       icu::UnicodeString *dest, int32_t capacity,
                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest == ಶೂನ್ಯಸೂಚಕ ? capacity != 0 : capacity < 0) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t length = array.get_ಗಾತ್ರ();
    ಆದರೆ(length == 0) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(length > capacity) {
        error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ length;
    }
    ಗಾಗಿ(int32_t i = 0; i < length; ++i) {
        int32_t s_ಉದ್ದಳತೆ;
        // No tracing: handled by the caller
        ಸ್ಥಿರಾಂಕ char16_t *s = res_getStringNoTrace(p_Res_ದತ್ತಾಂಶ, array.ಆಂತರಿಕ_Get_Resource(p_Res_ದತ್ತಾಂಶ, i), &s_ಉದ್ದಳತೆ);
        ಆದರೆ(s == ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
            ಹಿಂತಿರುಗಿ 0;
        }
        dest[i].ಗಣ_ಗಮ್ಯ(ನಿಜ, s, s_ಉದ್ದಳತೆ);
    }
    ಹಿಂತಿರುಗಿ length;
}

}  // namespace

U_CAPI ಸ್ಥಿರಾಂಕ char16_t * U_EXPORT2
res_getAlias(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ char16_t *p;
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(res);
    int32_t length;
    ಆದರೆ(RES_GET_TYPE(res)==URES_ALIAS) {
        ಸ್ಥಿರಾಂಕ int32_t *p32= ಸ್ಥಾನಾಂತರ==0 ? &g_ಖಾಲಿಗಳು_String.length : p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ;
        length=*p32++;
        p=(ಸ್ಥಿರಾಂಕ char16_t *)p32;
    } ಇಲ್ಲದಿದ್ದರೆ {
        p=ಶೂನ್ಯಸೂಚಕ;
        length=0;
    }
    ಆದರೆ(p_ಉದ್ದಳತೆ) {
        *p_ಉದ್ದಳತೆ=length;
    }
    ಹಿಂತಿರುಗಿ p;
}

U_CAPI ಸ್ಥಿರಾಂಕ uint8_t * U_EXPORT2
res_get_ದ್ವಿಮಾನ_No_Trace(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ uint8_t *p;
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(res);
    int32_t length;
    ಆದರೆ(RES_GET_TYPE(res)==URES_ದ್ವಿಮಾನ) {
        ಸ್ಥಿರಾಂಕ int32_t *p32= ಸ್ಥಾನಾಂತರ==0 ? (ಸ್ಥಿರಾಂಕ int32_t*)&gEmpty32 : p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ;
        length=*p32++;
        p=(ಸ್ಥಿರಾಂಕ uint8_t *)p32;
    } ಇಲ್ಲದಿದ್ದರೆ {
        p=ಶೂನ್ಯಸೂಚಕ;
        length=0;
    }
    ಆದರೆ(p_ಉದ್ದಳತೆ) {
        *p_ಉದ್ದಳತೆ=length;
    }
    ಹಿಂತಿರುಗಿ p;
}


U_CAPI ಸ್ಥಿರಾಂಕ int32_t * U_EXPORT2
res_get_Int_ಸದಿಶ_No_Trace(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res, int32_t *p_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ int32_t *p;
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(res);
    int32_t length;
    ಆದರೆ(RES_GET_TYPE(res)==URES_INT_ಸದಿಶ) {
        p= ಸ್ಥಾನಾಂತರ==0 ? (ಸ್ಥಿರಾಂಕ int32_t *)&gEmpty32 : p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ;
        length=*p++;
    } ಇಲ್ಲದಿದ್ದರೆ {
        p=ಶೂನ್ಯಸೂಚಕ;
        length=0;
    }
    ಆದರೆ(p_ಉದ್ದಳತೆ) {
        *p_ಉದ್ದಳತೆ=length;
    }
    ಹಿಂತಿರುಗಿ p;
}

U_CAPI int32_t U_EXPORT2
res_ಎಣಿಕೆ_Array_ವಸ್ತುಗಳು(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource res) {
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(res);
    ಆಯ್ಕೆ(RES_GET_TYPE(res)) {
    ಸ್ಥಿತಿ URES_STRING:
    ಸ್ಥಿತಿ URES_STRING_V2:
    ಸ್ಥಿತಿ URES_ದ್ವಿಮಾನ:
    ಸ್ಥಿತಿ URES_ALIAS:
    ಸ್ಥಿತಿ URES_INT:
    ಸ್ಥಿತಿ URES_INT_ಸದಿಶ:
        ಹಿಂತಿರುಗಿ 1;
    ಸ್ಥಿತಿ URES_ARRAY:
    ಸ್ಥಿತಿ URES_TABLE32:
        ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ==0 ? 0 : *(p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ);
    ಸ್ಥಿತಿ URES_ಕೋಷ್ಟಕ:
        ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ==0 ? 0 : *((ಸ್ಥಿರಾಂಕ uint16_t *)(p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ));
    ಸ್ಥಿತಿ URES_ARRAY16:
    ಸ್ಥಿತಿ URES_TABLE16:
        ಹಿಂತಿರುಗಿ p_Res_ದತ್ತಾಂಶ->p16_ಬಿಟ್_Units[ಸ್ಥಾನಾಂತರ];
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ 0;
    }
}

U_NAMESPACE_BEGIN

Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::~Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ() {}

UResType Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::getType() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ res_get_ಬಹಿರಂಗ_Type(res);
}

ಸ್ಥಿರಾಂಕ char16_t *Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::getString(int32_t &length, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ char16_t *s = res_getString(fTraceInfo, &get_ದತ್ತಾಂಶ(), res, &length);
    ಆದರೆ(s == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
    }
    ಹಿಂತಿರುಗಿ s;
}

ಸ್ಥಿರಾಂಕ char16_t *Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::getAliasString(int32_t &length, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ char16_t *s = res_getAlias(&get_ದತ್ತಾಂಶ(), res, &length);
    ಆದರೆ(s == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
    }
    ಹಿಂತಿರುಗಿ s;
}

int32_t Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::getInt(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(RES_GET_TYPE(res) != URES_INT) {
        error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
    }
    ಹಿಂತಿರುಗಿ res_getInt(fTraceInfo, res);
}

uint32_t Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::getUInt(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(RES_GET_TYPE(res) != URES_INT) {
        error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
    }
    ಹಿಂತಿರುಗಿ res_getUInt(fTraceInfo, res);
}

ಸ್ಥಿರಾಂಕ int32_t *Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::get_Int_ಸದಿಶ(int32_t &length, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ int32_t *iv = res_get_Int_ಸದಿಶ(fTraceInfo, &get_ದತ್ತಾಂಶ(), res, &length);
    ಆದರೆ(iv == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
    }
    ಹಿಂತಿರುಗಿ iv;
}

ಸ್ಥಿರಾಂಕ uint8_t *Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::get_ದ್ವಿಮಾನ(int32_t &length, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ uint8_t *b = res_get_ದ್ವಿಮಾನ(fTraceInfo, &get_ದತ್ತಾಂಶ(), res, &length);
    ಆದರೆ(b == ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
    }
    ಹಿಂತಿರುಗಿ b;
}

ResourceArray Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::getArray(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ {};
    }
    ಸ್ಥಿರಾಂಕ uint16_t *items16 = ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ Resource *items32 = ಶೂನ್ಯಸೂಚಕ;
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(res);
    int32_t length = 0;
    ಆಯ್ಕೆ(RES_GET_TYPE(res)) {
    ಸ್ಥಿತಿ URES_ARRAY:
        ಆದರೆ (ಸ್ಥಾನಾಂತರ!=0) {  // empty if offset==0
            items32 = reinterpret_cast<ಸ್ಥಿರಾಂಕ Resource*>(get_ದತ್ತಾಂಶ().p_ಬೇರು) + ಸ್ಥಾನಾಂತರ;
            length = *items32++;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ URES_ARRAY16:
        items16 = get_ದತ್ತಾಂಶ().p16_ಬಿಟ್_Units+ಸ್ಥಾನಾಂತರ;
        length = *items16++;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
        ಹಿಂತಿರುಗಿ {};
    }
    ಹಿಂತಿರುಗಿ ResourceArray(items16, items32, length, fTraceInfo);
}

Resource_ಕೋಷ್ಟಕ Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::get_ಕೋಷ್ಟಕ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ {};
    }
    ಸ್ಥಿರಾಂಕ uint16_t *keys16 = ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ int32_t *keys32 = ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ uint16_t *items16 = ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ Resource *items32 = ಶೂನ್ಯಸೂಚಕ;
    uint32_t ಸ್ಥಾನಾಂತರ = RES_GET_ಸ್ಥಾನಾಂತರ(res);
    int32_t length = 0;
    ಆಯ್ಕೆ(RES_GET_TYPE(res)) {
    ಸ್ಥಿತಿ URES_ಕೋಷ್ಟಕ:
        ಆದರೆ (ಸ್ಥಾನಾಂತರ != 0) {  // empty if offset==0
            keys16 = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(get_ದತ್ತಾಂಶ().p_ಬೇರು + ಸ್ಥಾನಾಂತರ);
            length = *keys16++;
            items32 = reinterpret_cast<ಸ್ಥಿರಾಂಕ Resource*>(keys16 + length + (~length & 1));
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ URES_TABLE16:
        keys16 = get_ದತ್ತಾಂಶ().p16_ಬಿಟ್_Units+ಸ್ಥಾನಾಂತರ;
        length = *keys16++;
        items16 = keys16 + length;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ URES_TABLE32:
        ಆದರೆ (ಸ್ಥಾನಾಂತರ != 0) {  // empty if offset==0
            keys32 = get_ದತ್ತಾಂಶ().p_ಬೇರು+ಸ್ಥಾನಾಂತರ;
            length = *keys32++;
            items32 = reinterpret_cast<ಸ್ಥಿರಾಂಕ Resource*>(keys32) + length;
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
        ಹಿಂತಿರುಗಿ {};
    }
    ಹಿಂತಿರುಗಿ Resource_ಕೋಷ್ಟಕ(keys16, keys32, items16, items32, length, fTraceInfo);
}

UBool Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::isNoInheritanceMarker() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ::isNoInheritanceMarker(&get_ದತ್ತಾಂಶ(), res);
}

int32_t Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::getStringArray(UnicodeString *dest, int32_t capacity,
                                          UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ::getStringArray(&get_ದತ್ತಾಂಶ(), getArray(error_ಸಂಕೇತ), dest, capacity, error_ಸಂಕೇತ);
}

int32_t Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(URES_IS_ARRAY(res)) {
        ಹಿಂತಿರುಗಿ ::getStringArray(&get_ದತ್ತಾಂಶ(), getArray(error_ಸಂಕೇತ), dest, capacity, error_ಸಂಕೇತ);
    }
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest == ಶೂನ್ಯಸೂಚಕ ? capacity != 0 : capacity < 0) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(capacity < 1) {
        error_ಸಂಕೇತ = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        ಹಿಂತಿರುಗಿ 1;
    }
    int32_t s_ಉದ್ದಳತೆ;
    ಸ್ಥಿರಾಂಕ char16_t *s = res_getString(fTraceInfo, &get_ದತ್ತಾಂಶ(), res, &s_ಉದ್ದಳತೆ);
    ಆದರೆ(s != ಶೂನ್ಯಸೂಚಕ) {
        dest[0].ಗಣ_ಗಮ್ಯ(ನಿಜ, s, s_ಉದ್ದಳತೆ);
        ಹಿಂತಿರುಗಿ 1;
    }
    error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
    ಹಿಂತಿರುಗಿ 0;
}

UnicodeString Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ::get_String_Or_ಮೊದಲ_ದ_Array(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    UnicodeString us;
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ us;
    }
    int32_t s_ಉದ್ದಳತೆ;
    ಸ್ಥಿರಾಂಕ char16_t *s = res_getString(fTraceInfo, &get_ದತ್ತಾಂಶ(), res, &s_ಉದ್ದಳತೆ);
    ಆದರೆ(s != ಶೂನ್ಯಸೂಚಕ) {
        us.ಗಣ_ಗಮ್ಯ(ನಿಜ, s, s_ಉದ್ದಳತೆ);
        ಹಿಂತಿರುಗಿ us;
    }
    ResourceArray array = getArray(error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ us;
    }
    ಆದರೆ(array.get_ಗಾತ್ರ() > 0) {
        // Tracing is already performed above (unimportant for trace that this is an array)
        s = res_getStringNoTrace(&get_ದತ್ತಾಂಶ(), array.ಆಂತರಿಕ_Get_Resource(&get_ದತ್ತಾಂಶ(), 0), &s_ಉದ್ದಳತೆ);
        ಆದರೆ(s != ಶೂನ್ಯಸೂಚಕ) {
            us.ಗಣ_ಗಮ್ಯ(ನಿಜ, s, s_ಉದ್ದಳತೆ);
            ಹಿಂತಿರುಗಿ us;
        }
    }
    error_ಸಂಕೇತ = U_RESOURCE_TYPE_MISMATCH;
    ಹಿಂತಿರುಗಿ us;
}

U_NAMESPACE_ಕೊನೆ

ಸ್ಥಿರ Resource
makeResourceFrom16(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, int32_t res16) {
    ಆದರೆ(res16<p_Res_ದತ್ತಾಂಶ->pool_String_Index16_ಎಲ್ಲೆ) {
        // Pool string, nothing to do.
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Local string, adjust the 16-bit offset to a regular one,
        // with a larger pool string index limit.
        res16=res16-p_Res_ದತ್ತಾಂಶ->pool_String_Index16_ಎಲ್ಲೆ+p_Res_ದತ್ತಾಂಶ->pool_String_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ;
    }
    ಹಿಂತಿರುಗಿ URES_MAKE_RESOURCE(URES_STRING_V2, res16);
}

U_CAPI Resource U_EXPORT2
res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource ಕೋಷ್ಟಕ,
                      int32_t *ಸೂಚ್ಯಂಕ_R, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಕೀಲಿ) {
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(ಕೋಷ್ಟಕ);
    int32_t length;
    int32_t idx;
    ಆದರೆ(ಕೀಲಿ == ಶೂನ್ಯಸೂಚಕ || *ಕೀಲಿ == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ RES_BOGUS;
    }
    ಆಯ್ಕೆ(RES_GET_TYPE(ಕೋಷ್ಟಕ)) {
    ಸ್ಥಿತಿ URES_ಕೋಷ್ಟಕ: {
        ಆದರೆ (ಸ್ಥಾನಾಂತರ!=0) { /* empty if offset==0 */
            ಸ್ಥಿರಾಂಕ uint16_t *p= (ಸ್ಥಿರಾಂಕ uint16_t *)(p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ);
            length=*p++;
            *ಸೂಚ್ಯಂಕ_R=idx=res_ಕಂಡುಹಿಡಿ_ಕೋಷ್ಟಕ_ಅಂಶ(p_Res_ದತ್ತಾಂಶ, p, length, *ಕೀಲಿ, ಕೀಲಿ);
            ಆದರೆ(idx>=0) {
                ಸ್ಥಿರಾಂಕ Resource *p32=(ಸ್ಥಿರಾಂಕ Resource *)(p+length+(~length&1));
                ಹಿಂತಿರುಗಿ p32[idx];
            }
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ URES_TABLE16: {
        ಸ್ಥಿರಾಂಕ uint16_t *p=p_Res_ದತ್ತಾಂಶ->p16_ಬಿಟ್_Units+ಸ್ಥಾನಾಂತರ;
        length=*p++;
        *ಸೂಚ್ಯಂಕ_R=idx=res_ಕಂಡುಹಿಡಿ_ಕೋಷ್ಟಕ_ಅಂಶ(p_Res_ದತ್ತಾಂಶ, p, length, *ಕೀಲಿ, ಕೀಲಿ);
        ಆದರೆ(idx>=0) {
            ಹಿಂತಿರುಗಿ makeResourceFrom16(p_Res_ದತ್ತಾಂಶ, p[length+idx]);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ URES_TABLE32: {
        ಆದರೆ (ಸ್ಥಾನಾಂತರ!=0) { /* empty if offset==0 */
            ಸ್ಥಿರಾಂಕ int32_t *p= p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ;
            length=*p++;
            *ಸೂಚ್ಯಂಕ_R=idx=res_ಕಂಡುಹಿಡಿ_Table32_ಅಂಶ(p_Res_ದತ್ತಾಂಶ, p, length, *ಕೀಲಿ, ಕೀಲಿ);
            ಆದರೆ(idx>=0) {
                ಹಿಂತಿರುಗಿ (Resource)p[length+idx];
            }
        }
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource ಕೋಷ್ಟಕ,
                        int32_t ಸೂಚ್ಯಂಕ_R, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಕೀಲಿ) {
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(ಕೋಷ್ಟಕ);
    int32_t length;
    ಆದರೆ (ಸೂಚ್ಯಂಕ_R < 0) {
        ಹಿಂತಿರುಗಿ RES_BOGUS;
    }
    ಆಯ್ಕೆ(RES_GET_TYPE(ಕೋಷ್ಟಕ)) {
    ಸ್ಥಿತಿ URES_ಕೋಷ್ಟಕ: {
        ಆದರೆ (ಸ್ಥಾನಾಂತರ != 0) { /* empty if offset==0 */
            ಸ್ಥಿರಾಂಕ uint16_t *p= (ಸ್ಥಿರಾಂಕ uint16_t *)(p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ);
            length=*p++;
            ಆದರೆ(ಸೂಚ್ಯಂಕ_R<length) {
                ಸ್ಥಿರಾಂಕ Resource *p32=(ಸ್ಥಿರಾಂಕ Resource *)(p+length+(~length&1));
                ಆದರೆ(ಕೀಲಿ!=ಶೂನ್ಯಸೂಚಕ) {
                    *ಕೀಲಿ=RES_GET_KEY16(p_Res_ದತ್ತಾಂಶ, p[ಸೂಚ್ಯಂಕ_R]);
                }
                ಹಿಂತಿರುಗಿ p32[ಸೂಚ್ಯಂಕ_R];
            }
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ URES_TABLE16: {
        ಸ್ಥಿರಾಂಕ uint16_t *p=p_Res_ದತ್ತಾಂಶ->p16_ಬಿಟ್_Units+ಸ್ಥಾನಾಂತರ;
        length=*p++;
        ಆದರೆ(ಸೂಚ್ಯಂಕ_R<length) {
            ಆದರೆ(ಕೀಲಿ!=ಶೂನ್ಯಸೂಚಕ) {
                *ಕೀಲಿ=RES_GET_KEY16(p_Res_ದತ್ತಾಂಶ, p[ಸೂಚ್ಯಂಕ_R]);
            }
            ಹಿಂತಿರುಗಿ makeResourceFrom16(p_Res_ದತ್ತಾಂಶ, p[length+ಸೂಚ್ಯಂಕ_R]);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ URES_TABLE32: {
        ಆದರೆ (ಸ್ಥಾನಾಂತರ != 0) { /* empty if offset==0 */
            ಸ್ಥಿರಾಂಕ int32_t *p= p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ;
            length=*p++;
            ಆದರೆ(ಸೂಚ್ಯಂಕ_R<length) {
                ಆದರೆ(ಕೀಲಿ!=ಶೂನ್ಯಸೂಚಕ) {
                    *ಕೀಲಿ=RES_GET_KEY32(p_Res_ದತ್ತಾಂಶ, p[ಸೂಚ್ಯಂಕ_R]);
                }
                ಹಿಂತಿರುಗಿ (Resource)p[length+ಸೂಚ್ಯಂಕ_R];
            }
        }
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ RES_BOGUS;
}

U_CAPI Resource U_EXPORT2
res_getResource(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *real_ಕೀಲಿ=ಕೀಲಿ;
    int32_t idx;
    ಹಿಂತಿರುಗಿ res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ(p_Res_ದತ್ತಾಂಶ, p_Res_ದತ್ತಾಂಶ->ಬೇರು_Res, &idx, &real_ಕೀಲಿ);
}


UBool icu::Resource_ಕೋಷ್ಟಕ::get_ಕೀಲಿ_And_ಮೌಲ್ಯ(int32_t i,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *&ಕೀಲಿ, icu::Resource_ಮೌಲ್ಯ &value) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(0 <= i && i < length) {
        icu::Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ &rd_ಮೌಲ್ಯ = static_cast<icu::Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ &>(value);
        ಆದರೆ (keys16 != ಶೂನ್ಯಸೂಚಕ) {
            ಕೀಲಿ = RES_GET_KEY16(&rd_ಮೌಲ್ಯ.get_ದತ್ತಾಂಶ(), keys16[i]);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಕೀಲಿ = RES_GET_KEY32(&rd_ಮೌಲ್ಯ.get_ದತ್ತಾಂಶ(), keys32[i]);
        }
        Resource res;
        ಆದರೆ (items16 != ಶೂನ್ಯಸೂಚಕ) {
            res = makeResourceFrom16(&rd_ಮೌಲ್ಯ.get_ದತ್ತಾಂಶ(), items16[i]);
        } ಇಲ್ಲದಿದ್ದರೆ {
            res = items32[i];
        }
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceTable. This is OK because the ResourceTable should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ಮೌಲ್ಯ.ಗಣ_Resource(res, ResourceTracer(fTraceInfo, ಕೀಲಿ));
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

UBool icu::Resource_ಕೋಷ್ಟಕ::ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value) ಸ್ಥಿರಾಂಕ {
    icu::Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ &rd_ಮೌಲ್ಯ = static_cast<icu::Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ &>(value);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *real_ಕೀಲಿ = ಶೂನ್ಯಸೂಚಕ;
    int32_t i;
    ಆದರೆ (keys16 != ಶೂನ್ಯಸೂಚಕ) {
        i = res_ಕಂಡುಹಿಡಿ_ಕೋಷ್ಟಕ_ಅಂಶ(&rd_ಮೌಲ್ಯ.get_ದತ್ತಾಂಶ(), keys16, length, ಕೀಲಿ, &real_ಕೀಲಿ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        i = res_ಕಂಡುಹಿಡಿ_Table32_ಅಂಶ(&rd_ಮೌಲ್ಯ.get_ದತ್ತಾಂಶ(), keys32, length, ಕೀಲಿ, &real_ಕೀಲಿ);
    }
    ಆದರೆ (i >= 0) {
        Resource res;
        ಆದರೆ (items16 != ಶೂನ್ಯಸೂಚಕ) {
            res = makeResourceFrom16(&rd_ಮೌಲ್ಯ.get_ದತ್ತಾಂಶ(), items16[i]);
        } ಇಲ್ಲದಿದ್ದರೆ {
            res = items32[i];
        }
        // Same note about lifetime as in getKeyAndValue().
        rd_ಮೌಲ್ಯ.ಗಣ_Resource(res, ResourceTracer(fTraceInfo, ಕೀಲಿ));
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

U_CAPI Resource U_EXPORT2
res_get_Array_ಅಂಶ(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource array, int32_t ಸೂಚ್ಯಂಕ_R) {
    uint32_t ಸ್ಥಾನಾಂತರ=RES_GET_ಸ್ಥಾನಾಂತರ(array);
    ಆದರೆ (ಸೂಚ್ಯಂಕ_R < 0) {
        ಹಿಂತಿರುಗಿ RES_BOGUS;
    }
    ಆಯ್ಕೆ(RES_GET_TYPE(array)) {
    ಸ್ಥಿತಿ URES_ARRAY: {
        ಆದರೆ (ಸ್ಥಾನಾಂತರ!=0) { /* empty if offset==0 */
            ಸ್ಥಿರಾಂಕ int32_t *p= p_Res_ದತ್ತಾಂಶ->p_ಬೇರು+ಸ್ಥಾನಾಂತರ;
            ಆದರೆ(ಸೂಚ್ಯಂಕ_R<*p) {
                ಹಿಂತಿರುಗಿ (Resource)p[1+ಸೂಚ್ಯಂಕ_R];
            }
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ URES_ARRAY16: {
        ಸ್ಥಿರಾಂಕ uint16_t *p=p_Res_ದತ್ತಾಂಶ->p16_ಬಿಟ್_Units+ಸ್ಥಾನಾಂತರ;
        ಆದರೆ(ಸೂಚ್ಯಂಕ_R<*p) {
            ಹಿಂತಿರುಗಿ makeResourceFrom16(p_Res_ದತ್ತಾಂಶ, p[1+ಸೂಚ್ಯಂಕ_R]);
        }
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ RES_BOGUS;
}

uint32_t icu::ResourceArray::ಆಂತರಿಕ_Get_Resource(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, int32_t i) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (items16 != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ makeResourceFrom16(p_Res_ದತ್ತಾಂಶ, items16[i]);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ items32[i];
    }
}

UBool icu::ResourceArray::get_ಮೌಲ್ಯ(int32_t i, icu::Resource_ಮೌಲ್ಯ &value) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(0 <= i && i < length) {
        icu::Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ &rd_ಮೌಲ್ಯ = static_cast<icu::Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ &>(value);
        // Note: the ResourceTracer keeps a reference to the field of this
        // ResourceArray. This is OK because the ResourceArray should remain
        // alive for the duration that fields are being read from it
        // (including nested fields).
        rd_ಮೌಲ್ಯ.ಗಣ_Resource(
            ಆಂತರಿಕ_Get_Resource(&rd_ಮೌಲ್ಯ.get_ದತ್ತಾಂಶ(), i),
            ResourceTracer(fTraceInfo, i));
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

U_CFUNC Resource
res_ಕಂಡುಹಿಡಿ_Resource(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, Resource r, ಅಕ್ಷರ** ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** ಕೀಲಿ) {
  ಅಕ್ಷರ *ಮಾರ್ಗ_P = *ಮಾರ್ಗ, *ಮುಂದಿನ_Sep_P = *ಮಾರ್ಗ;
  ಅಕ್ಷರ *close_ಸೂಚ್ಯಂಕ = ಶೂನ್ಯಸೂಚಕ;
  Resource t1 = r;
  Resource t2;
  int32_t ಸೂಚ್ಯಂಕ_R = 0;
  UResType type = (UResType)RES_GET_TYPE(t1);

  /* if you come in with an empty path, you'll be getting back the same resource */
  ಆದರೆ(!uprv_strlen(ಮಾರ್ಗ_P)) {
      ಹಿಂತಿರುಗಿ r;
  }

  /* one needs to have an aggregate resource in order to search in it */
  ಆದರೆ(!URES_IS_CONTAINER(type)) {
      ಹಿಂತಿರುಗಿ RES_BOGUS;
  }
  
  ಆಗ(ಮುಂದಿನ_Sep_P && *ಮಾರ್ಗ_P && t1 != RES_BOGUS && URES_IS_CONTAINER(type)) {
    /* Iteration stops if: the path has been consumed, we found a non-existing
     * resource (t1 == RES_BOGUS) or we found a scalar resource (including alias)
     */
    ಮುಂದಿನ_Sep_P = uprv_strchr(ಮಾರ್ಗ_P, RES_ಮಾರ್ಗ_SEPARATOR);
    /* if there are more separators, terminate string 
     * and set path to the remaining part of the string
     */
    ಆದರೆ(ಮುಂದಿನ_Sep_P != ಶೂನ್ಯಸೂಚಕ) {
      ಆದರೆ(ಮುಂದಿನ_Sep_P == ಮಾರ್ಗ_P) {
        // Empty key string.
        ಹಿಂತಿರುಗಿ RES_BOGUS;
      }
      *ಮುಂದಿನ_Sep_P = 0; /* overwrite the separator with a NUL to terminate the key */
      *ಮಾರ್ಗ = ಮುಂದಿನ_Sep_P+1;
    } ಇಲ್ಲದಿದ್ದರೆ {
      *ಮಾರ್ಗ = uprv_strchr(ಮಾರ್ಗ_P, 0);
    }

    /* if the resource is a table */
    /* try the key based access */
    ಆದರೆ(URES_IS_ಕೋಷ್ಟಕ(type)) {
      *ಕೀಲಿ = ಮಾರ್ಗ_P;
      t2 = res_get_ಕೋಷ್ಟಕ_ಅಂಶ_By_ಕೀಲಿ(p_Res_ದತ್ತಾಂಶ, t1, &ಸೂಚ್ಯಂಕ_R, ಕೀಲಿ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(URES_IS_ARRAY(type)) {
      ಸೂಚ್ಯಂಕ_R = uprv_strtol(ಮಾರ್ಗ_P, &close_ಸೂಚ್ಯಂಕ, 10);
      ಆದರೆ(ಸೂಚ್ಯಂಕ_R >= 0 && *close_ಸೂಚ್ಯಂಕ == 0) {
        t2 = res_get_Array_ಅಂಶ(p_Res_ದತ್ತಾಂಶ, t1, ಸೂಚ್ಯಂಕ_R);
      } ಇಲ್ಲದಿದ್ದರೆ {
        t2 = RES_BOGUS; /* have an array, but don't have a valid index */
      }
      *ಕೀಲಿ = ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ { /* can't do much here, except setting t2 to bogus */
      t2 = RES_BOGUS;
    }
    t1 = t2;
    type = (UResType)RES_GET_TYPE(t1);
    /* position pathP to next resource key/index */
    ಮಾರ್ಗ_P = *ಮಾರ್ಗ;
  }

  ಹಿಂತಿರುಗಿ t1;
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಅಡ್ಡಸಾಲು_2 {
    int32_t ಕೀಲಿ_ಸೂಚ್ಯಂಕ, ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ;
} ಅಡ್ಡಸಾಲು_2;

ಸ್ಥಿರ int32_t U_CALLCONV
ures_compare_ಅಡ್ಡಸಾಲುಗಳು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಎಡ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಲ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ_Chars = static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(context);
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(uprv_strcmp(ಕೀಲಿ_Chars + static_cast<ಸ್ಥಿರಾಂಕ ಅಡ್ಡಸಾಲು_2*>(ಎಡ)->ಕೀಲಿ_ಸೂಚ್ಯಂಕ,
                                            ಕೀಲಿ_Chars + static_cast<ಸ್ಥಿರಾಂಕ ಅಡ್ಡಸಾಲು_2*>(ಬಲ)->ಕೀಲಿ_ಸೂಚ್ಯಂಕ));
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ_2 {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_Chars;
    ಅಡ್ಡಸಾಲು_2 *ಅಡ್ಡಸಾಲುಗಳು;
    int32_t *resort;
    uint32_t *resFlags;
    int32_t local_ಕೀಲಿ_ಎಲ್ಲೆ;
    uint8_t major_Format_ಆವೃತ್ತಿ;
} ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ_2;

ಎಣಿಕೆ {
    ರಾಶಿ_ಅಡ್ಡಸಾಲು_ಸಾಮರ್ಥ್ಯ=200
};

/* The table item key string is not locally available. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ g_Unknown_ಕೀಲಿ="";

#ಆದರೆ !UCONFIG_NO_COLLATION
// resource table key for collation binaries
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t g_Collation_Bin_ಕೀಲಿ[]=u"%%CollationBin";
#ಆದರೆ_ಅಂತ್ಯ

/*
 * swap one resource item
 */
ಸ್ಥಿರ ಶೂನ್ಯ
ures_ಅದಲುಬದಲು_Resource(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                  ಸ್ಥಿರಾಂಕ Resource *ಒಳ_Bundle, Resource *ಹೊರಹೋಗು_Bundle,
                  Resource res, /* caller swaps res itself */
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ,
                  ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ_2 *p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Resource *p;
    Resource *q;
    int32_t ಸ್ಥಾನಾಂತರ, count;

    ಆಯ್ಕೆ(RES_GET_TYPE(res)) {
    ಸ್ಥಿತಿ URES_TABLE16:
    ಸ್ಥಿತಿ URES_STRING_V2:
    ಸ್ಥಿತಿ URES_INT:
    ಸ್ಥಿತಿ URES_ARRAY16:
        /* integer, or points to 16-bit units, nothing to do here */
        ಹಿಂತಿರುಗಿ;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    /* all other types use an offset to point to their data */
    ಸ್ಥಾನಾಂತರ = static_cast<int32_t>(RES_GET_ಸ್ಥಾನಾಂತರ(res));
    ಆದರೆ(ಸ್ಥಾನಾಂತರ==0) {
        /* special offset indicating an empty item */
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->resFlags[ಸ್ಥಾನಾಂತರ >> 5] & (static_cast<uint32_t>(1) << (ಸ್ಥಾನಾಂತರ & 0x1f))) {
        /* we already swapped this resource item */
        ಹಿಂತಿರುಗಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* mark it as swapped now */
        p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->resFlags[ಸ್ಥಾನಾಂತರ >> 5] |= static_cast<uint32_t>(1) << (ಸ್ಥಾನಾಂತರ & 0x1f);
    }

    p=ಒಳ_Bundle+ಸ್ಥಾನಾಂತರ;
    q=ಹೊರಹೋಗು_Bundle+ಸ್ಥಾನಾಂತರ;

    ಆಯ್ಕೆ(RES_GET_TYPE(res)) {
    ಸ್ಥಿತಿ URES_ALIAS:
        /* physically same value layout as string, fall through */
        U_FALLTHROUGH;
    ಸ್ಥಿತಿ URES_STRING:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->ಅದಲುಬದಲು_Array32(ds, p, 4, q, p_Error_ಸಂಕೇತ);
        /* swap each char16_t (the terminating NUL would not change) */
        ds->ಅದಲುಬದಲು_Array16(ds, p+1, 2*count, q+1, p_Error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ URES_ದ್ವಿಮಾನ:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length */
        ds->ಅದಲುಬದಲು_Array32(ds, p, 4, q, p_Error_ಸಂಕೇತ);
        /* no need to swap or copy bytes - ures_swap() copied them all */

        /* swap known formats */
#ಆದರೆ !UCONFIG_NO_COLLATION
        ಆದರೆ( ಕೀಲಿ!=ಶೂನ್ಯಸೂಚಕ &&  /* the binary is in a table */
            (ಕೀಲಿ!=g_Unknown_ಕೀಲಿ ?
                /* its table key string is "%%CollationBin" */
                0==ds->compareInvChars(ds, ಕೀಲಿ, -1,
                                       g_Collation_Bin_ಕೀಲಿ, UPRV_LENGTHOF(g_Collation_Bin_ಕೀಲಿ)-1) :
                /* its table key string is unknown but it looks like a collation binary */
                ucol_looks_Like_Collation_ದ್ವಿಮಾನ(ds, p+1, count))
        ) {
            ucol_ಅದಲುಬದಲು(ds, p+1, count, q+1, p_Error_ಸಂಕೇತ);
        }
#ಆದರೆ_ಅಂತ್ಯ
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ URES_ಕೋಷ್ಟಕ:
    ಸ್ಥಿತಿ URES_TABLE32:
        {
            ಸ್ಥಿರಾಂಕ uint16_t *pKey16;
            uint16_t *qKey16;

            ಸ್ಥಿರಾಂಕ int32_t *pKey32;
            int32_t *qKey32;

            Resource ಅಂಶ;
            int32_t i, old_ಸೂಚ್ಯಂಕ;

            ಆದರೆ(RES_GET_TYPE(res)==URES_ಕೋಷ್ಟಕ) {
                /* get table item count */
                pKey16 = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(p);
                qKey16 = reinterpret_cast<uint16_t*>(q);
                count=ds->readUInt16(*pKey16);

                pKey32=qKey32=ಶೂನ್ಯಸೂಚಕ;

                /* swap count */
                ds->ಅದಲುಬದಲು_Array16(ds, pKey16++, 2, qKey16++, p_Error_ಸಂಕೇತ);

                ಸ್ಥಾನಾಂತರ+=((1+count)+1)/2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* get table item count */
                pKey32 = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t*>(p);
                qKey32 = reinterpret_cast<int32_t*>(q);
                count=udata_readInt32(ds, *pKey32);

                pKey16=qKey16=ಶೂನ್ಯಸೂಚಕ;

                /* swap count */
                ds->ಅದಲುಬದಲು_Array32(ds, pKey32++, 4, qKey32++, p_Error_ಸಂಕೇತ);

                ಸ್ಥಾನಾಂತರ+=1+count;
            }

            ಆದರೆ(count==0) {
                ನಿಲ್ಲಿಸು;
            }

            p=ಒಳ_Bundle+ಸ್ಥಾನಾಂತರ; /* pointer to table resources */
            q=ಹೊರಹೋಗು_Bundle+ಸ್ಥಾನಾಂತರ;

            /* recurse */
            ಗಾಗಿ(i=0; i<count; ++i) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಅಂಶ_ಕೀಲಿ=g_Unknown_ಕೀಲಿ;
                ಆದರೆ(pKey16!=ಶೂನ್ಯಸೂಚಕ) {
                    int32_t ಕೀಲಿ_ಸ್ಥಾನಾಂತರ=ds->readUInt16(pKey16[i]);
                    ಆದರೆ(ಕೀಲಿ_ಸ್ಥಾನಾಂತರ<p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->local_ಕೀಲಿ_ಎಲ್ಲೆ) {
                        ಅಂಶ_ಕೀಲಿ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಹೊರಹೋಗು_Bundle) + ಕೀಲಿ_ಸ್ಥಾನಾಂತರ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    int32_t ಕೀಲಿ_ಸ್ಥಾನಾಂತರ=udata_readInt32(ds, pKey32[i]);
                    ಆದರೆ(ಕೀಲಿ_ಸ್ಥಾನಾಂತರ>=0) {
                        ಅಂಶ_ಕೀಲಿ = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(ಹೊರಹೋಗು_Bundle) + ಕೀಲಿ_ಸ್ಥಾನಾಂತರ;
                    }
                }
                ಅಂಶ=ds->readUInt32(p[i]);
                ures_ಅದಲುಬದಲು_Resource(ds, ಒಳ_Bundle, ಹೊರಹೋಗು_Bundle, ಅಂಶ, ಅಂಶ_ಕೀಲಿ, p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ, p_Error_ಸಂಕೇತ);
                ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                    udata_printError(ds, "ures_swapResource(table res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, ಅಂಶ);
                    ಹಿಂತಿರುಗಿ;
                }
            }

            ಆದರೆ(p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->major_Format_ಆವೃತ್ತಿ>1 || ds->ಒಳ_Charset==ds->ಹೊರಹೋಗು_Charset) {
                /* no need to sort, just swap the offset/value arrays */
                ಆದರೆ(pKey16!=ಶೂನ್ಯಸೂಚಕ) {
                    ds->ಅದಲುಬದಲು_Array16(ds, pKey16, count*2, qKey16, p_Error_ಸಂಕೇತ);
                    ds->ಅದಲುಬದಲು_Array32(ds, p, count*4, q, p_Error_ಸಂಕೇತ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* swap key offsets and items as one array */
                    ds->ಅದಲುಬದಲು_Array32(ds, pKey32, count*2*4, qKey32, p_Error_ಸಂಕೇತ);
                }
                ನಿಲ್ಲಿಸು;
            }

            /*
             * We need to sort tables by outCharset key strings because they
             * sort differently for different charset families.
             * ures_swap() already set pTempTable->keyChars appropriately.
             * First we set up a temporary table with the key indexes and
             * sorting indexes and sort that.
             * Then we permutate and copy/swap the actual values.
             */
            ಆದರೆ(pKey16!=ಶೂನ್ಯಸೂಚಕ) {
                ಗಾಗಿ(i=0; i<count; ++i) {
                    p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->ಅಡ್ಡಸಾಲುಗಳು[i].ಕೀಲಿ_ಸೂಚ್ಯಂಕ=ds->readUInt16(pKey16[i]);
                    p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->ಅಡ್ಡಸಾಲುಗಳು[i].ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ=i;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಾಗಿ(i=0; i<count; ++i) {
                    p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->ಅಡ್ಡಸಾಲುಗಳು[i].ಕೀಲಿ_ಸೂಚ್ಯಂಕ=udata_readInt32(ds, pKey32[i]);
                    p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->ಅಡ್ಡಸಾಲುಗಳು[i].ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ=i;
                }
            }
            uprv_ವಿಂಗಡಿಸು_Array(p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->ಅಡ್ಡಸಾಲುಗಳು, count, ಗಾತ್ರ(ಅಡ್ಡಸಾಲು_2),
                           ures_compare_ಅಡ್ಡಸಾಲುಗಳು, p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->ಕೀಲಿ_Chars,
                           ಸುಳ್ಳು, p_Error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                udata_printError(ds, "ures_swapResource(table res=%08x).uprv_sortArray(%d items) failed\n",
                                 res, count);
                ಹಿಂತಿರುಗಿ;
            }

            /*
             * copy/swap/permutate items
             *
             * If we swap in-place, then the permutation must use another
             * temporary array (pTempTable->resort)
             * before the results are copied to the outBundle.
             */
            /*ಕೀಲಿಗಳು*/
            ಆದರೆ(pKey16!=ಶೂನ್ಯಸೂಚಕ) {
                uint16_t *rKey16;

                ಆದರೆ(pKey16!=qKey16) {
                    rKey16=qKey16;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    rKey16 = reinterpret_cast<uint16_t*>(p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->resort);
                }
                ಗಾಗಿ(i=0; i<count; ++i) {
                    old_ಸೂಚ್ಯಂಕ=p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->ಅಡ್ಡಸಾಲುಗಳು[i].ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ;
                    ds->ಅದಲುಬದಲು_Array16(ds, pKey16+old_ಸೂಚ್ಯಂಕ, 2, rKey16+i, p_Error_ಸಂಕೇತ);
                }
                ಆದರೆ(qKey16!=rKey16) {
                    uprv_memcpy(qKey16, rKey16, 2*count);
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t *rKey32;

                ಆದರೆ(pKey32!=qKey32) {
                    rKey32=qKey32;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    rKey32=p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->resort;
                }
                ಗಾಗಿ(i=0; i<count; ++i) {
                    old_ಸೂಚ್ಯಂಕ=p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->ಅಡ್ಡಸಾಲುಗಳು[i].ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ;
                    ds->ಅದಲುಬದಲು_Array32(ds, pKey32+old_ಸೂಚ್ಯಂಕ, 4, rKey32+i, p_Error_ಸಂಕೇತ);
                }
                ಆದರೆ(qKey32!=rKey32) {
                    uprv_memcpy(qKey32, rKey32, 4*count);
                }
            }

            /* resources */
            {
                Resource *r;


                ಆದರೆ(p!=q) {
                    r=q;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    r = reinterpret_cast<Resource*>(p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->resort);
                }
                ಗಾಗಿ(i=0; i<count; ++i) {
                    old_ಸೂಚ್ಯಂಕ=p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ->ಅಡ್ಡಸಾಲುಗಳು[i].ವಿಂಗಡಿಸು_ಸೂಚ್ಯಂಕ;
                    ds->ಅದಲುಬದಲು_Array32(ds, p+old_ಸೂಚ್ಯಂಕ, 4, r+i, p_Error_ಸಂಕೇತ);
                }
                ಆದರೆ(q!=r) {
                    uprv_memcpy(q, r, 4*count);
                }
            }
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ URES_ARRAY:
        {
            Resource ಅಂಶ;
            int32_t i;

            count = udata_readInt32(ds, static_cast<int32_t>(*p));
            /* swap length */
            ds->ಅದಲುಬದಲು_Array32(ds, p++, 4, q++, p_Error_ಸಂಕೇತ);

            /* recurse */
            ಗಾಗಿ(i=0; i<count; ++i) {
                ಅಂಶ=ds->readUInt32(p[i]);
                ures_ಅದಲುಬದಲು_Resource(ds, ಒಳ_Bundle, ಹೊರಹೋಗು_Bundle, ಅಂಶ, ಶೂನ್ಯಸೂಚಕ, p_ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ, p_Error_ಸಂಕೇತ);
                ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                    udata_printError(ds, "ures_swapResource(array res=%08x)[%d].recurse(%08x) failed\n",
                                     res, i, ಅಂಶ);
                    ಹಿಂತಿರುಗಿ;
                }
            }

            /* swap items */
            ds->ಅದಲುಬದಲು_Array32(ds, p, 4*count, q, p_Error_ಸಂಕೇತ);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ URES_INT_ಸದಿಶ:
        count = udata_readInt32(ds, static_cast<int32_t>(*p));
        /* swap length and each integer */
        ds->ಅದಲುಬದಲು_Array32(ds, p, 4*(1+count), q, p_Error_ಸಂಕೇತ);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* also catches RES_BOGUS */
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ನಿಲ್ಲಿಸು;
    }
}

U_CAPI int32_t U_EXPORT2
ures_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo;
    ಸ್ಥಿರಾಂಕ Resource *ಒಳ_Bundle;
    Resource ಬೇರು_Res;
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ, max_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ;

    ಅಡ್ಡಸಾಲು_2 ಅಡ್ಡಸಾಲುಗಳು[ರಾಶಿ_ಅಡ್ಡಸಾಲು_ಸಾಮರ್ಥ್ಯ];
    int32_t resort[ರಾಶಿ_ಅಡ್ಡಸಾಲು_ಸಾಮರ್ಥ್ಯ];
    ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ_2 ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ;

    ಸ್ಥಿರಾಂಕ int32_t *ಒಳ_Indexes;

    /* the following integers count Resource item offsets (4 bytes each), not bytes */
    int32_t bundle_ಉದ್ದಳತೆ, ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ, ಕೀಲಿಗಳು_Bottom, ಕೀಲಿಗಳು_ತುದಿ, resBottom, ತುದಿ;

    /* udata_swapDataHeader checks the arguments */
    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* check data format and format version */
    pInfo=(ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
    ಆದರೆ(!(
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x52 &&   /* dataFormat="ResB" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x65 &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x73 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x42 &&
        /* formatVersion 1.1+ or 2.x or 3.x */
        ((pInfo->format_ಆವೃತ್ತಿ[0]==1 && pInfo->format_ಆವೃತ್ತಿ[1]>=1) ||
            pInfo->format_ಆವೃತ್ತಿ[0]==2 || pInfo->format_ಆವೃತ್ತಿ[0]==3)
    )) {
        udata_printError(ds, "ures_swap(): data format %02x.%02x.%02x.%02x (format version %02x.%02x) is not a resource bundle\n",
                         pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1],
                         pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3],
                         pInfo->format_ಆವೃತ್ತಿ[0], pInfo->format_ಆವೃತ್ತಿ[1]);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.major_Format_ಆವೃತ್ತಿ=pInfo->format_ಆವೃತ್ತಿ[0];

    /* a resource bundle must contain at least one resource item */
    ಆದರೆ(length<0) {
        bundle_ಉದ್ದಳತೆ=-1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        bundle_ಉದ್ದಳತೆ=(length-ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)/4;

        /* formatVersion 1.1 must have a root item and at least 5 indexes */
        ಆದರೆ(bundle_ಉದ್ದಳತೆ<(1+5)) {
            udata_printError(ds, "ures_swap(): too few bytes (%d after header) for a resource bundle\n",
                             length-ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    ಒಳ_Bundle=(ಸ್ಥಿರಾಂಕ Resource *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);
    ಬೇರು_Res=ds->readUInt32(*ಒಳ_Bundle);

    /* formatVersion 1.1 adds the indexes[] array */
    ಒಳ_Indexes=(ಸ್ಥಿರಾಂಕ int32_t *)(ಒಳ_Bundle+1);

    ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ=udata_readInt32(ds, ಒಳ_Indexes[URES_ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ])&0xff;
    ಆದರೆ(ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ<=URES_ಸೂಚ್ಯಂಕ_MAX_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ) {
        udata_printError(ds, "ures_swap(): too few indexes for a 1.1+ resource bundle\n");
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಕೀಲಿಗಳು_Bottom=1+ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ;
    ಕೀಲಿಗಳು_ತುದಿ=udata_readInt32(ds, ಒಳ_Indexes[URES_ಸೂಚ್ಯಂಕ_ಕೀಲಿಗಳು_ತುದಿ]);
    ಆದರೆ(ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ>URES_ಸೂಚ್ಯಂಕ_16_ಬಿಟ್_ತುದಿ) {
        resBottom=udata_readInt32(ds, ಒಳ_Indexes[URES_ಸೂಚ್ಯಂಕ_16_ಬಿಟ್_ತುದಿ]);
    } ಇಲ್ಲದಿದ್ದರೆ {
        resBottom=ಕೀಲಿಗಳು_ತುದಿ;
    }
    ತುದಿ=udata_readInt32(ds, ಒಳ_Indexes[URES_ಸೂಚ್ಯಂಕ_BUNDLE_ತುದಿ]);
    max_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ=udata_readInt32(ds, ಒಳ_Indexes[URES_ಸೂಚ್ಯಂಕ_MAX_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ]);

    ಆದರೆ(0<=bundle_ಉದ್ದಳತೆ && bundle_ಉದ್ದಳತೆ<ತುದಿ) {
        udata_printError(ds, "ures_swap(): resource top %d exceeds bundle length %d\n",
                         ತುದಿ, bundle_ಉದ್ದಳತೆ);
        *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ಕೀಲಿಗಳು_ತುದಿ>(1+ಸೂಚ್ಯಂಕ_ಉದ್ದಳತೆ)) {
        ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.local_ಕೀಲಿ_ಎಲ್ಲೆ=ಕೀಲಿಗಳು_ತುದಿ<<2;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.local_ಕೀಲಿ_ಎಲ್ಲೆ=0;
    }

    ಆದರೆ(length>=0) {
        Resource *ಹೊರಹೋಗು_Bundle=(Resource *)((ಅಕ್ಷರ *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ);

        /* track which resources we have already swapped */
        uint32_t ರಾಶಿ_Res_Flags[ರಾಶಿ_ಅಡ್ಡಸಾಲು_ಸಾಮರ್ಥ್ಯ];
        int32_t res_Flags_ಉದ್ದಳತೆ;

        /*
         * We need one bit per 4 resource bundle bytes so that we can track
         * every possible Resource for whether we have swapped it already.
         * Multiple Resource words can refer to the same bundle offsets
         * for sharing identical values.
         * We could optimize this by allocating only for locations above
         * where Resource values are stored (above keys & strings).
         */
        res_Flags_ಉದ್ದಳತೆ=(length+31)>>5;          /* number of bytes needed */
        res_Flags_ಉದ್ದಳತೆ=(res_Flags_ಉದ್ದಳತೆ+3)&~3;   /* multiple of 4 bytes for uint32_t */
        ಆದರೆ(res_Flags_ಉದ್ದಳತೆ<=(int32_t)ಗಾತ್ರ(ರಾಶಿ_Res_Flags)) {
            ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags=ರಾಶಿ_Res_Flags;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags=(uint32_t *)uprv_malloc(res_Flags_ಉದ್ದಳತೆ);
            ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags==ಶೂನ್ಯಸೂಚಕ) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for tracking resources\n");
                *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
        }
        uprv_memset(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags, 0, res_Flags_ಉದ್ದಳತೆ);

        /* copy the bundle for binary and inaccessible data */
        ಆದರೆ(ಒಳ_ದತ್ತಾಂಶ!=ಹೊರಹೋಗು_ದತ್ತಾಂಶ) {
            uprv_memcpy(ಹೊರಹೋಗು_Bundle, ಒಳ_Bundle, 4*ತುದಿ);
        }

        /* swap the key strings, but not the padding bytes (0xaa) after the last string and its NUL */
        udata_ಅದಲುಬದಲು_Inv_String_ಖಂಡ(ds, ಒಳ_Bundle+ಕೀಲಿಗಳು_Bottom, 4*(ಕೀಲಿಗಳು_ತುದಿ-ಕೀಲಿಗಳು_Bottom),
                                    ಹೊರಹೋಗು_Bundle+ಕೀಲಿಗಳು_Bottom, p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            udata_printError(ds, "ures_swap().udata_swapInvStringBlock(keys[%d]) failed\n", 4*(ಕೀಲಿಗಳು_ತುದಿ-ಕೀಲಿಗಳು_Bottom));
            ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags!=ರಾಶಿ_Res_Flags) {
                uprv_free(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags);
            }
            ಹಿಂತಿರುಗಿ 0;
        }

        /* swap the 16-bit units (strings, table16, array16) */
        ಆದರೆ(ಕೀಲಿಗಳು_ತುದಿ<resBottom) {
            ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_Bundle+ಕೀಲಿಗಳು_ತುದಿ, (resBottom-ಕೀಲಿಗಳು_ತುದಿ)*4, ಹೊರಹೋಗು_Bundle+ಕೀಲಿಗಳು_ತುದಿ, p_Error_ಸಂಕೇತ);
            ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                udata_printError(ds, "ures_swap().swapArray16(16-bit units[%d]) failed\n", 2*(resBottom-ಕೀಲಿಗಳು_ತುದಿ));
                ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags!=ರಾಶಿ_Res_Flags) {
                    uprv_free(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags);
                }
                ಹಿಂತಿರುಗಿ 0;
            }
        }

        /* allocate the temporary table for sorting resource tables */
        ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಕೀಲಿ_Chars=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಹೊರಹೋಗು_Bundle; /* sort by outCharset */
        ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.major_Format_ಆವೃತ್ತಿ>1 || max_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ<=ರಾಶಿ_ಅಡ್ಡಸಾಲು_ಸಾಮರ್ಥ್ಯ) {
            ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು=ಅಡ್ಡಸಾಲುಗಳು;
            ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resort=resort;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು=(ಅಡ್ಡಸಾಲು_2 *)uprv_malloc(max_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ*ಗಾತ್ರ(ಅಡ್ಡಸಾಲು_2)+max_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ*4);
            ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು==ಶೂನ್ಯಸೂಚಕ) {
                udata_printError(ds, "ures_swap(): unable to allocate memory for sorting tables (max length: %d)\n",
                                 max_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ);
                *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags!=ರಾಶಿ_Res_Flags) {
                    uprv_free(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags);
                }
                ಹಿಂತಿರುಗಿ 0;
            }
            ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resort=(int32_t *)(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು+max_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ);
        }

        /* swap the resources */
        ures_ಅದಲುಬದಲು_Resource(ds, ಒಳ_Bundle, ಹೊರಹೋಗು_Bundle, ಬೇರು_Res, ಶೂನ್ಯಸೂಚಕ, &ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ, p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            udata_printError(ds, "ures_swapResource(root res=%08x) failed\n",
                             ಬೇರು_Res);
        }

        ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು!=ಅಡ್ಡಸಾಲುಗಳು) {
            uprv_free(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.ಅಡ್ಡಸಾಲುಗಳು);
        }
        ಆದರೆ(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags!=ರಾಶಿ_Res_Flags) {
            uprv_free(ತಾತ್ಕಾಲಿಕ_ಕೋಷ್ಟಕ.resFlags);
        }

        /* swap the root resource and indexes */
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_Bundle, ಕೀಲಿಗಳು_Bottom*4, ಹೊರಹೋಗು_Bundle, p_Error_ಸಂಕೇತ);
    }

    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ+4*ತುದಿ;
}

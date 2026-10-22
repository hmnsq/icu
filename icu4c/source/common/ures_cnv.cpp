// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1997-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ures_cnv.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug25
*   created by: Markus W. Scherer
*
*   Character conversion functions moved here from uresbund.c
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(ਅਚਲ char16_t *my_ਰਸਤਾ,
           ਅਚਲ ਅੱਖਰ *localeID, 
           UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਅੱਖਰ ਰਸਤਾ_ਬਫਰ[1024];
    int32_t length;
    ਅੱਖਰ *ਰਸਤਾ = ਰਸਤਾ_ਬਫਰ;

    ਜੇ(ਸਥਿਤੀ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(my_ਰਸਤਾ==ਖਾਲੀ_ਸੂਚਕ) {
        ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਨਹੀਂਤਾਂ {
        length=u_strlen(my_ਰਸਤਾ);
        ਜੇ(length>=(int32_t)ਆਕਾਰ(ਰਸਤਾ_ਬਫਰ)) {
            *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        } ਨਹੀਂਤਾਂ ਜੇ(uprv_isInvariantUString(my_ਰਸਤਾ, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UChars_ਮੰਜ਼ਿਲ_Chars(my_ਰਸਤਾ, ਰਸਤਾ, length+1); /* length+1 to include the NUL */
        } ਨਹੀਂਤਾਂ {
#ਜੇ !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(ਸਥਿਤੀ);
            length=ucnv_fromUChars(cnv, ਰਸਤਾ, (int32_t)ਆਕਾਰ(ਰਸਤਾ_ਬਫਰ), my_ਰਸਤਾ, length, ਸਥਿਤੀ);
            u_releaseDefaultConverter(cnv);
            ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            ਜੇ(length>=(int32_t)ਆਕਾਰ(ਰਸਤਾ_ਬਫਰ)) {
                /* not NUL-terminated - path too long */
                *ਸਥਿਤੀ=U_ILLEGAL_ਦਲੀਲ_ERROR;
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
#ਨਹੀਂਤਾਂ
            /* the default converter is not available */
            *ਸਥਿਤੀ=U_UNSUPPORTED_ERROR;
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
#ਜੇ_ਅੰਤ
        }
    }

    ਵਾਪਸ ures_open(ਰਸਤਾ, localeID, ਸਥਿਤੀ);
}

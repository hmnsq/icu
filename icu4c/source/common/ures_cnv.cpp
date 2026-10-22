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

#شامل "unicode/utypes.h"
#شامل "unicode/putil.h"
#شامل "unicode/ustring.h"
#شامل "unicode/ucnv.h"
#شامل "unicode/ures.h"
#شامل "uinvchar.h"
#شامل "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(ثابت char16_t *my_راستہ,
           ثابت حرف *localeID, 
           UError_رمز *حالت_2)
{
    حرف راستہ_بفر[1024];
    int32_t length;
    حرف *راستہ = راستہ_بفر;

    اگر(حالت_2==خالی_اشاریہ || U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    اگر(my_راستہ==خالی_اشاریہ) {
        راستہ = خالی_اشاریہ;
    }
    ورنہ {
        length=u_strlen(my_راستہ);
        اگر(length>=(int32_t)حجم(راستہ_بفر)) {
            *حالت_2=U_ILLEGAL_دلیل_ERROR;
            واپس خالی_اشاریہ;
        } ورنہ اگر(uprv_isInvariantUString(my_راستہ, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UChars_مقصد_Chars(my_راستہ, راستہ, length+1); /* length+1 to include the NUL */
        } ورنہ {
#اگر !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(حالت_2);
            length=ucnv_fromUChars(cnv, راستہ, (int32_t)حجم(راستہ_بفر), my_راستہ, length, حالت_2);
            u_releaseDefaultConverter(cnv);
            اگر(U_FAILURE(*حالت_2)) {
                واپس خالی_اشاریہ;
            }
            اگر(length>=(int32_t)حجم(راستہ_بفر)) {
                /* not NUL-terminated - path too long */
                *حالت_2=U_ILLEGAL_دلیل_ERROR;
                واپس خالی_اشاریہ;
            }
#ورنہ
            /* the default converter is not available */
            *حالت_2=U_UNSUPPORTED_ERROR;
            واپس خالی_اشاریہ;
#اگر_ختم
        }
    }

    واپس ures_open(راستہ, localeID, حالت_2);
}

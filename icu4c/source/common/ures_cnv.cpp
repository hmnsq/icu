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

#ضمن "unicode/utypes.h"
#ضمن "unicode/putil.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/ucnv.h"
#ضمن "unicode/ures.h"
#ضمن "uinvchar.h"
#ضمن "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(ثابت char16_t *my_مسیر,
           ثابت حرف *localeID, 
           UError_کد *وضعیت)
{
    حرف مسیر_میانگیر[1024];
    int32_t length;
    حرف *مسیر = مسیر_میانگیر;

    إذا(وضعیت==اشاره_تهی || U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    إذا(my_مسیر==اشاره_تهی) {
        مسیر = اشاره_تهی;
    }
    وإلا {
        length=u_strlen(my_مسیر);
        إذا(length>=(int32_t)حجم(مسیر_میانگیر)) {
            *وضعیت=U_ILLEGAL_آرگومان_ERROR;
            أرجع اشاره_تهی;
        } وإلا إذا(uprv_isInvariantUString(my_مسیر, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UChars_مقصد_Chars(my_مسیر, مسیر, length+1); /* length+1 to include the NUL */
        } وإلا {
#إذا !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(وضعیت);
            length=ucnv_fromUChars(cnv, مسیر, (int32_t)حجم(مسیر_میانگیر), my_مسیر, length, وضعیت);
            u_releaseDefaultConverter(cnv);
            إذا(U_FAILURE(*وضعیت)) {
                أرجع اشاره_تهی;
            }
            إذا(length>=(int32_t)حجم(مسیر_میانگیر)) {
                /* not NUL-terminated - path too long */
                *وضعیت=U_ILLEGAL_آرگومان_ERROR;
                أرجع اشاره_تهی;
            }
#وإلا
            /* the default converter is not available */
            *وضعیت=U_UNSUPPORTED_ERROR;
            أرجع اشاره_تهی;
#پایان_اگر
        }
    }

    أرجع ures_open(مسیر, localeID, وضعیت);
}

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
ures_openU(ثابت char16_t *my_رستہ,
           ثابت حرف *localeID, 
           UError_کوڈ *حالت)
{
    حرف رستہ_بفر[1024];
    int32_t length;
    حرف *رستہ = رستہ_بفر;

    إذا(حالت==خالی_سوچک || U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    إذا(my_رستہ==خالی_سوچک) {
        رستہ = خالی_سوچک;
    }
    وإلا {
        length=u_strlen(my_رستہ);
        إذا(length>=(int32_t)حجم(رستہ_بفر)) {
            *حالت=U_ILLEGAL_دلیل_ERROR;
            أرجع خالی_سوچک;
        } وإلا إذا(uprv_isInvariantUString(my_رستہ, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UChars_منزل_Chars(my_رستہ, رستہ, length+1); /* length+1 to include the NUL */
        } وإلا {
#إذا !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(حالت);
            length=ucnv_fromUChars(cnv, رستہ, (int32_t)حجم(رستہ_بفر), my_رستہ, length, حالت);
            u_releaseDefaultConverter(cnv);
            إذا(U_FAILURE(*حالت)) {
                أرجع خالی_سوچک;
            }
            إذا(length>=(int32_t)حجم(رستہ_بفر)) {
                /* not NUL-terminated - path too long */
                *حالت=U_ILLEGAL_دلیل_ERROR;
                أرجع خالی_سوچک;
            }
#وإلا
            /* the default converter is not available */
            *حالت=U_UNSUPPORTED_ERROR;
            أرجع خالی_سوچک;
#جے_انت
        }
    }

    أرجع ures_open(رستہ, localeID, حالت);
}

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
ures_openU(ثابت char16_t *myPath,
           ثابت حرف *localeID, 
           UErrorCode *status)
{
    حرف pathBuffer[1024];
    int32_t length;
    حرف *path = pathBuffer;

    إذا(status==nullptr || U_FAILURE(*status)) {
        أرجع nullptr;
    }
    إذا(myPath==nullptr) {
        path = nullptr;
    }
    وإلا {
        length=u_strlen(myPath);
        إذا(length>=(int32_t)حجم(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            أرجع nullptr;
        } وإلا إذا(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } وإلا {
#إذا !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)حجم(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            إذا(U_FAILURE(*status)) {
                أرجع nullptr;
            }
            إذا(length>=(int32_t)حجم(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                أرجع nullptr;
            }
#وإلا
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            أرجع nullptr;
#endif
        }
    }

    أرجع ures_open(path, localeID, status);
}

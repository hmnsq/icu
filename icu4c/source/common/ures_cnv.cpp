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

#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "unicode/ustring.h"
#include "unicode/ucnv.h"
#include "unicode/ures.h"
#include "uinvchar.h"
#include "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(const char16_t *myPath,
           const var_na *localeID, 
           UErrorCode *status)
{
    var_na pathBuffer[1024];
    int32_t length;
    var_na *path = pathBuffer;

    ᰀᰧ(status==nullptr || U_FAILURE(*status)) {
        ᰀᰧᰌ nullptr;
    }
    ᰀᰧ(myPath==nullptr) {
        path = nullptr;
    }
    ᰀᰧᰀ {
        length=u_strlen(myPath);
        ᰀᰧ(length>=(int32_t)parimaa_na(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            ᰀᰧᰌ nullptr;
        } ᰀᰧᰀ ᰀᰧ(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } ᰀᰧᰀ {
#ᰀᰧ !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)parimaa_na(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            ᰀᰧ(U_FAILURE(*status)) {
                ᰀᰧᰌ nullptr;
            }
            ᰀᰧ(length>=(int32_t)parimaa_na(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                ᰀᰧᰌ nullptr;
            }
#ᰀᰧᰀ
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            ᰀᰧᰌ nullptr;
#endif
        }
    }

    ᰀᰧᰌ ures_open(path, localeID, status);
}

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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"
#ଅନ୍ତର୍ଭାଵ "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(ଅଚଳ char16_t *myPath,
           ଅଚଳ ଅକ୍ଷର *localeID, 
           UErrorCode *status)
{
    ଅକ୍ଷର pathBuffer[1024];
    int32_t length;
    ଅକ୍ଷର *path = pathBuffer;

    ଯଦି(status==nullptr || U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(myPath==nullptr) {
        path = nullptr;
    }
    ଅନ୍ୟଥା {
        length=u_strlen(myPath);
        ଯଦି(length>=(int32_t)ଆକାର(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ nullptr;
        } ଅନ୍ୟଥା ଯଦି(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } ଅନ୍ୟଥା {
#ଯଦି !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)ଆକାର(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            ଯଦି(U_FAILURE(*status)) {
                ଫେରନ୍ତୁ nullptr;
            }
            ଯଦି(length>=(int32_t)ଆକାର(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                ଫେରନ୍ତୁ nullptr;
            }
#ଅନ୍ୟଥା
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            ଫେରନ୍ତୁ nullptr;
#endif
        }
    }

    ଫେରନ୍ତୁ ures_open(path, localeID, status);
}

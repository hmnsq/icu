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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/ucnv.h"
#অন্তর্ভাৱ "unicode/ures.h"
#অন্তর্ভাৱ "uinvchar.h"
#অন্তর্ভাৱ "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(ধ্রুৱ char16_t *myPath,
           ধ্রুৱ ৱর্ণ *localeID, 
           UErrorCode *status)
{
    ৱর্ণ pathBuffer[1024];
    int32_t length;
    ৱর্ণ *path = pathBuffer;

    যদি(status==nullptr || U_FAILURE(*status)) {
        ঘূৰাই nullptr;
    }
    যদি(myPath==nullptr) {
        path = nullptr;
    }
    অন্যথা {
        length=u_strlen(myPath);
        যদি(length>=(int32_t)পরিমাণ(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই nullptr;
        } অন্যথা যদি(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } অন্যথা {
#যদি !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)পরিমাণ(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            যদি(U_FAILURE(*status)) {
                ঘূৰাই nullptr;
            }
            যদি(length>=(int32_t)পরিমাণ(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                ঘূৰাই nullptr;
            }
#অন্যথা
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            ঘূৰাই nullptr;
#endif
        }
    }

    ঘূৰাই ures_open(path, localeID, status);
}

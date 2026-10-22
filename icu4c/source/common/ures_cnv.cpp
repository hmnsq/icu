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
ures_openU(ثابت char16_t *myPath,
           ثابت نویسه *localeID, 
           UErrorCode *status)
{
    نویسه pathBuffer[1024];
    int32_t length;
    نویسه *path = pathBuffer;

    اگر(status==nullptr || U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    اگر(myPath==nullptr) {
        path = nullptr;
    }
    وگرنه {
        length=u_strlen(myPath);
        اگر(length>=(int32_t)اندازه(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت nullptr;
        } وگرنه اگر(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } وگرنه {
#اگر !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)اندازه(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            اگر(U_FAILURE(*status)) {
                بازگشت nullptr;
            }
            اگر(length>=(int32_t)اندازه(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                بازگشت nullptr;
            }
#وگرنه
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            بازگشت nullptr;
#endif
        }
    }

    بازگشت ures_open(path, localeID, status);
}

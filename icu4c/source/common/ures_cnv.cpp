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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(ಧ್ರುವ char16_t *myPath,
           ಧ್ರುವ ವರ್ಣ *localeID, 
           UErrorCode *status)
{
    ವರ್ಣ pathBuffer[1024];
    int32_t length;
    ವರ್ಣ *path = pathBuffer;

    ಯದಿ(status==nullptr || U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(myPath==nullptr) {
        path = nullptr;
    }
    ಅನ್ಯಥಾ {
        length=u_strlen(myPath);
        ಯದಿ(length>=(int32_t)ಪರಿಮಾಣ(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        } ಅನ್ಯಥಾ ಯದಿ(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } ಅನ್ಯಥಾ {
#ಯದಿ !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)ಪರಿಮಾಣ(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            ಯದಿ(U_FAILURE(*status)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            ಯದಿ(length>=(int32_t)ಪರಿಮಾಣ(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
#ಅನ್ಯಥಾ
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            ಪ್ರತ್ಯಾಗಮ nullptr;
#endif
        }
    }

    ಪ್ರತ್ಯಾಗಮ ures_open(path, localeID, status);
}

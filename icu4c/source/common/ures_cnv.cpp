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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(स्थिरांक char16_t *myPath,
           स्थिरांक अक्षर *localeID, 
           UErrorCode *status)
{
    अक्षर pathBuffer[1024];
    int32_t length;
    अक्षर *path = pathBuffer;

    जर(status==nullptr || U_FAILURE(*status)) {
        परत nullptr;
    }
    जर(myPath==nullptr) {
        path = nullptr;
    }
    अन्यथा {
        length=u_strlen(myPath);
        जर(length>=(int32_t)आकार(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            परत nullptr;
        } अन्यथा जर(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } अन्यथा {
#जर !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)आकार(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            जर(U_FAILURE(*status)) {
                परत nullptr;
            }
            जर(length>=(int32_t)आकार(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                परत nullptr;
            }
#अन्यथा
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            परत nullptr;
#endif
        }
    }

    परत ures_open(path, localeID, status);
}

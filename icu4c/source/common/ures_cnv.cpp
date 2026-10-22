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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "uinvchar.h"
#അന്തര്ഭാവ "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(സ്ഥിരം char16_t *myPath,
           സ്ഥിരം അക്ഷരം *localeID, 
           UErrorCode *status)
{
    അക്ഷരം pathBuffer[1024];
    int32_t length;
    അക്ഷരം *path = pathBuffer;

    എങ്കിൽ(status==nullptr || U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(myPath==nullptr) {
        path = nullptr;
    }
    അല്ലെങ്കിൽ {
        length=u_strlen(myPath);
        എങ്കിൽ(length>=(int32_t)വലുപ്പം(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ nullptr;
        } അല്ലെങ്കിൽ എങ്കിൽ(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } അല്ലെങ്കിൽ {
#എങ്കിൽ !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)വലുപ്പം(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            എങ്കിൽ(U_FAILURE(*status)) {
                തിരികെ nullptr;
            }
            എങ്കിൽ(length>=(int32_t)വലുപ്പം(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                തിരികെ nullptr;
            }
#അല്ലെങ്കിൽ
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            തിരികെ nullptr;
#endif
        }
    }

    തിരികെ ures_open(path, localeID, status);
}

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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "uinvchar.h"
#அந்தர்பாவ "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(மாறிலி char16_t *myPath,
           மாறிலி எழுத்து *localeID, 
           UErrorCode *status)
{
    எழுத்து pathBuffer[1024];
    int32_t length;
    எழுத்து *path = pathBuffer;

    எனில்(status==nullptr || U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    எனில்(myPath==nullptr) {
        path = nullptr;
    }
    இல்லையெனில் {
        length=u_strlen(myPath);
        எனில்(length>=(int32_t)அளவு(pathBuffer)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு nullptr;
        } இல்லையெனில் எனில்(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } இல்லையெனில் {
#எனில் !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)அளவு(pathBuffer), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            எனில்(U_FAILURE(*status)) {
                திரும்பு nullptr;
            }
            எனில்(length>=(int32_t)அளவு(pathBuffer)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                திரும்பு nullptr;
            }
#இல்லையெனில்
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            திரும்பு nullptr;
#endif
        }
    }

    திரும்பு ures_open(path, localeID, status);
}

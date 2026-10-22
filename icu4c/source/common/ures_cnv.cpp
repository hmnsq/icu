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

#समावेश "unicode/utypes.h"
#समावेश "unicode/putil.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/ucnv.h"
#समावेश "unicode/ures.h"
#समावेश "uinvchar.h"
#समावेश "ustr_cnv.h"

U_CAPI UResourceBundle * U_EXPORT2
ures_openU(स्थिर char16_t *myPath,
           स्थिर अक्षर *localeID, 
           UErrorCode *status)
{
    अक्षर path_बफर[1024];
    int32_t length;
    अक्षर *path = path_बफर;

    अगर(status==nullptr || U_FAILURE(*status)) {
        वापस nullptr;
    }
    अगर(myPath==nullptr) {
        path = nullptr;
    }
    अन्यथा {
        length=u_strlen(myPath);
        अगर(length>=(int32_t)माप(path_बफर)) {
            *status=U_ILLEGAL_ARGUMENT_ERROR;
            वापस nullptr;
        } अन्यथा अगर(uprv_isInvariantUString(myPath, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UCharsToChars(myPath, path, length+1); /* length+1 to include the NUL */
        } अन्यथा {
#अगर !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(status);
            length=ucnv_fromUChars(cnv, path, (int32_t)माप(path_बफर), myPath, length, status);
            u_releaseDefaultConverter(cnv);
            अगर(U_FAILURE(*status)) {
                वापस nullptr;
            }
            अगर(length>=(int32_t)माप(path_बफर)) {
                /* not NUL-terminated - path too long */
                *status=U_ILLEGAL_ARGUMENT_ERROR;
                वापस nullptr;
            }
#अन्यथा
            /* the default converter is not available */
            *status=U_UNSUPPORTED_ERROR;
            वापस nullptr;
#पूर्ण_अगर
        }
    }

    वापस ures_open(path, localeID, status);
}

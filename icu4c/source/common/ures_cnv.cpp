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
ures_openU(ध्रुव char16_t *my_मार्गः,
           ध्रुव वर्ण *localeID, 
           UError_संहिता *स्थितिः)
{
    वर्ण मार्गः_बफर[1024];
    int32_t length;
    वर्ण *मार्गः = मार्गः_बफर;

    यदि(स्थितिः==शून्यसूचकः || U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(my_मार्गः==शून्यसूचकः) {
        मार्गः = शून्यसूचकः;
    }
    अन्यथा {
        length=u_strlen(my_मार्गः);
        यदि(length>=(int32_t)परिमाण(मार्गः_बफर)) {
            *स्थितिः=U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम शून्यसूचकः;
        } अन्यथा यदि(uprv_isInvariantUString(my_मार्गः, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UChars_गन्तव्यम्_Chars(my_मार्गः, मार्गः, length+1); /* length+1 to include the NUL */
        } अन्यथा {
#यदि !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(स्थितिः);
            length=ucnv_fromUChars(cnv, मार्गः, (int32_t)परिमाण(मार्गः_बफर), my_मार्गः, length, स्थितिः);
            u_releaseDefaultConverter(cnv);
            यदि(U_FAILURE(*स्थितिः)) {
                प्रत्यागम शून्यसूचकः;
            }
            यदि(length>=(int32_t)परिमाण(मार्गः_बफर)) {
                /* not NUL-terminated - path too long */
                *स्थितिः=U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम शून्यसूचकः;
            }
#अन्यथा
            /* the default converter is not available */
            *स्थितिः=U_UNSUPPORTED_ERROR;
            प्रत्यागम शून्यसूचकः;
#यदि_अन्तः
        }
    }

    प्रत्यागम ures_open(मार्गः, localeID, स्थितिः);
}

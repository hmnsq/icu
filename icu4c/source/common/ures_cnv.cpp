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
ures_openU(ಸ್ಥಿರಾಂಕ char16_t *my_ಮಾರ್ಗ,
           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *localeID, 
           UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ಅಕ್ಷರ ಮಾರ್ಗ_ಬಫರ್[1024];
    int32_t length;
    ಅಕ್ಷರ *ಮಾರ್ಗ = ಮಾರ್ಗ_ಬಫರ್;

    ಆದರೆ(ಸ್ಥಿತಿ_2==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(my_ಮಾರ್ಗ==ಶೂನ್ಯಸೂಚಕ) {
        ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        length=u_strlen(my_ಮಾರ್ಗ);
        ಆದರೆ(length>=(int32_t)ಗಾತ್ರ(ಮಾರ್ಗ_ಬಫರ್)) {
            *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(uprv_isInvariantUString(my_ಮಾರ್ಗ, length)) {
            /*
             * the invariant converter is sufficient for package and tree names
             * and is more efficient
             */
            u_UChars_ಗಮ್ಯ_Chars(my_ಮಾರ್ಗ, ಮಾರ್ಗ, length+1); /* length+1 to include the NUL */
        } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ !UCONFIG_NO_CONVERSION
            /* use the default converter to support variant-character paths */
            UConverter *cnv=u_getDefaultConverter(ಸ್ಥಿತಿ_2);
            length=ucnv_fromUChars(cnv, ಮಾರ್ಗ, (int32_t)ಗಾತ್ರ(ಮಾರ್ಗ_ಬಫರ್), my_ಮಾರ್ಗ, length, ಸ್ಥಿತಿ_2);
            u_releaseDefaultConverter(cnv);
            ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            ಆದರೆ(length>=(int32_t)ಗಾತ್ರ(ಮಾರ್ಗ_ಬಫರ್)) {
                /* not NUL-terminated - path too long */
                *ಸ್ಥಿತಿ_2=U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
#ಇಲ್ಲದಿದ್ದರೆ
            /* the default converter is not available */
            *ಸ್ಥಿತಿ_2=U_UNSUPPORTED_ERROR;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
#ಆದರೆ_ಅಂತ್ಯ
        }
    }

    ಹಿಂತಿರುಗಿ ures_open(ಮಾರ್ಗ, localeID, ಸ್ಥಿತಿ_2);
}

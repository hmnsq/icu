// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2007, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucnv_set.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004sep07
*   created by: Markus W. Scherer
*
*   Conversion API functions using USet (ucnv_getUnicodeSet())
*   moved here from ucnv.c for removing the dependency of other ucnv_
*   implementation functions on the USet implementation.
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "uset_imp.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnv_get_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                   USet *ಗಣ_Fill_ಒಳ,
                   UConverter_Unicode_ಗಣ which_ಗಣ,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /* argument checking */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(cnv==ಶೂನ್ಯಸೂಚಕ || ಗಣ_Fill_ಒಳ==ಶೂನ್ಯಸೂಚಕ || which_ಗಣ<UCNV_ROUNDTRIP_ಗಣ || UCNV_ಗಣ_ಎಣಿಕೆ<=which_ಗಣ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    /* does this converter support this function? */
    ಆದರೆ(cnv->shared_ದತ್ತಾಂಶ->impl->get_Unicode_ಗಣ==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    {
        USetAdder sa={
            ಶೂನ್ಯಸೂಚಕ,
            uset_ಕೂಡಿಸು,
            uset_ಕೂಡಿಸು_Range,
            uset_ಕೂಡಿಸು_String,
            uset_remove,
            uset_removeRange
        };
        sa.ಗಣ=ಗಣ_Fill_ಒಳ;

        /* empty the set */
        uset_clear(ಗಣ_Fill_ಒಳ);

        /* call the converter to add the code points it supports */
        cnv->shared_ದತ್ತಾಂಶ->impl->get_Unicode_ಗಣ(cnv, &sa, which_ಗಣ, p_Error_ಸಂಕೇತ);
    }
}

#ಆದರೆ_ಅಂತ್ಯ

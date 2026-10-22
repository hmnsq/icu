// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2009-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/icudataver.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h" /* for ures_getVersionByKey */

U_CAPI ಶೂನ್ಯ U_EXPORT2 u_get_ದತ್ತಾಂಶ_ಆವೃತ್ತಿ(UVersionInfo ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Fillin, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UResourceBundle *icudatares = ಶೂನ್ಯಸೂಚಕ;
    
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    
    ಆದರೆ (ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Fillin != ಶೂನ್ಯಸೂಚಕ) {
        icudatares = ures_open_ನೇರ(ಶೂನ್ಯಸೂಚಕ, U_ICU_ಆವೃತ್ತಿ_BUNDLE , ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
            ures_get_ಆವೃತ್ತಿ_By_ಕೀಲಿ(icudatares, U_ICU_ದತ್ತಾಂಶ_ಕೀಲಿ, ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Fillin, ಸ್ಥಿತಿ_2);
        }
        ures_close(icudatares);
    }
}

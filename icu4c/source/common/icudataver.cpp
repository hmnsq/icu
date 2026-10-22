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

U_CAPI ಶೂನ್ಯ U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    
    ಯದಿ (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        ಯದಿ (U_SUCCESS(*status)) {
            ures_get_Version_By_ಕೀಲಿ(icudatares, U_ICU_DATA_ಕೀಲಿ, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

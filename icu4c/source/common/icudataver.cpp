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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/icudataver.h"
#അന്തര്ഭാവ "unicode/ures.h"
#അന്തര്ഭാവ "uresimp.h" /* for ures_getVersionByKey */

U_CAPI ശൂന്യം U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ;
    }
    
    എങ്കിൽ (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        എങ്കിൽ (U_SUCCESS(*status)) {
            ures_get_Version_By_താക്കോൽ(icudatares, U_ICU_DATA_താക്കോൽ, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

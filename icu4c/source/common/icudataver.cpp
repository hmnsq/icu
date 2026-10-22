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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/icudataver.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "uresimp.h" /* for ures_getVersionByKey */

U_CAPI શૂન્ય U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    જો (U_FAILURE(*status)) {
        પાછા;
    }
    
    જો (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        જો (U_SUCCESS(*status)) {
            ures_get_Version_By_ચાવી(icudatares, U_ICU_DATA_ચાવી, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

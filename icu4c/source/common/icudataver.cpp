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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/icudataver.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ures.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h" /* for ures_getVersionByKey */

U_CAPI ଶୂନ୍ୟ U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ;
    }
    
    ଯଦି (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        ଯଦି (U_SUCCESS(*status)) {
            ures_get_Version_By_ଚାବି(icudatares, U_ICU_DATA_ଚାବି, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

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

#شامل "unicode/utypes.h"
#شامل "unicode/icudataver.h"
#شامل "unicode/ures.h"
#شامل "uresimp.h" /* for ures_getVersionByKey */

U_CAPI تهی U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    اگر (U_FAILURE(*status)) {
        بازگشت;
    }
    
    اگر (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        اگر (U_SUCCESS(*status)) {
            ures_get_Version_By_کلید(icudatares, U_ICU_DATA_کلید, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

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

#ضمن "unicode/utypes.h"
#ضمن "unicode/icudataver.h"
#ضمن "unicode/ures.h"
#ضمن "uresimp.h" /* for ures_getVersionByKey */

U_CAPI فراغ U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    إذا (U_FAILURE(*status)) {
        أرجع;
    }
    
    إذا (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        إذا (U_SUCCESS(*status)) {
            ures_get_Version_By_مبودي(icudatares, U_ICU_DATA_مبودي, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

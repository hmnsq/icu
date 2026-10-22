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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/icudataver.h"
#অন্তর্ভাৱ "unicode/ures.h"
#অন্তর্ভাৱ "uresimp.h" /* for ures_getVersionByKey */

U_CAPI শূন্য U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    যদি (U_FAILURE(*status)) {
        ঘূৰাই;
    }
    
    যদি (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        যদি (U_SUCCESS(*status)) {
            ures_get_Version_By_চাবি(icudatares, U_ICU_DATA_চাবি, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

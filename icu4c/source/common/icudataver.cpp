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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/icudataver.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "uresimp.h" /* for ures_getVersionByKey */

U_CAPI शून्य U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    यदि (U_FAILURE(*status)) {
        प्रत्यागम;
    }
    
    यदि (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        यदि (U_SUCCESS(*status)) {
            ures_get_Version_By_किल्ली(icudatares, U_ICU_DATA_किल्ली, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

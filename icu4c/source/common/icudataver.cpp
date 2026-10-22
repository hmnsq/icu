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
    
    सचे (U_FAILURE(*status)) {
        पटिनिवत्त;
    }
    
    सचे (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        सचे (U_SUCCESS(*status)) {
            ures_get_Version_By_कुञ्चिका(icudatares, U_ICU_DATA_कुञ्चिका, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

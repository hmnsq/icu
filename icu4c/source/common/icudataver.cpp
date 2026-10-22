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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/icudataver.h"
#அந்தர்பாவ "unicode/ures.h"
#அந்தர்பாவ "uresimp.h" /* for ures_getVersionByKey */

U_CAPI வெறுமை U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    எனில் (U_FAILURE(*status)) {
        திரும்பு;
    }
    
    எனில் (dataVersionFillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        எனில் (U_SUCCESS(*status)) {
            ures_get_Version_By_சாவி(icudatares, U_ICU_DATA_சாவி, dataVersionFillin, status);
        }
        ures_close(icudatares);
    }
}

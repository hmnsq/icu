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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/icudataver.h"
#অন্তর্ভাব "unicode/ures.h"
#অন্তর্ভাব "uresimp.h" /* for ures_getVersionByKey */

U_CAPI শূন্য U_EXPORT2 u_get_তথ্য_Version(UVersionInfo তথ্য_Version_Fillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    যদি (U_FAILURE(*status)) {
        ফেরত;
    }
    
    যদি (তথ্য_Version_Fillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        যদি (U_SUCCESS(*status)) {
            ures_get_Version_By_চাবি(icudatares, U_ICU_তথ্য_চাবি, তথ্য_Version_Fillin, status);
        }
        ures_close(icudatares);
    }
}

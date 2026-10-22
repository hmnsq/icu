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

#समावेश "unicode/utypes.h"
#समावेश "unicode/icudataver.h"
#समावेश "unicode/ures.h"
#समावेश "uresimp.h" /* for ures_getVersionByKey */

U_CAPI व्योम U_EXPORT2 u_get_तथ्य_Version(UVersionInfo तथ्य_Version_Fillin, UErrorCode *status) {
    UResourceBundle *icudatares = nullptr;
    
    अगर (U_FAILURE(*status)) {
        वापस;
    }
    
    अगर (तथ्य_Version_Fillin != nullptr) {
        icudatares = ures_openDirect(nullptr, U_ICU_VERSION_BUNDLE , status);
        अगर (U_SUCCESS(*status)) {
            ures_get_Version_By_कुंजी(icudatares, U_ICU_तथ्य_कुंजी, तथ्य_Version_Fillin, status);
        }
        ures_close(icudatares);
    }
}

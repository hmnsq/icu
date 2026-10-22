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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/icudataver.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h" /* for ures_getVersionByKey */

U_CAPI ਖਾਲੀ U_EXPORT2 u_get_ਅੰਕੜੇ_ਸੰਸਕਰਣ(UVersionInfo ਅੰਕੜੇ_ਸੰਸਕਰਣ_Fillin, UError_ਕੋਡ *ਸਥਿਤੀ) {
    UResourceBundle *icudatares = ਖਾਲੀ_ਸੂਚਕ;
    
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    
    ਜੇ (ਅੰਕੜੇ_ਸੰਸਕਰਣ_Fillin != ਖਾਲੀ_ਸੂਚਕ) {
        icudatares = ures_open_ਸਿੱਧਾ(ਖਾਲੀ_ਸੂਚਕ, U_ICU_ਸੰਸਕਰਣ_BUNDLE , ਸਥਿਤੀ);
        ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
            ures_get_ਸੰਸਕਰਣ_By_ਕੁੰਜੀ(icudatares, U_ICU_ਅੰਕੜੇ_ਕੁੰਜੀ, ਅੰਕੜੇ_ਸੰਸਕਰਣ_Fillin, ਸਥਿਤੀ);
        }
        ures_close(icudatares);
    }
}

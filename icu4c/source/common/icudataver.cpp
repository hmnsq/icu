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

U_CAPI فراغ U_EXPORT2 u_get_معلومات_نسخو(UVersionInfo معلومات_نسخو_Fillin, UError_ڪوڊ *حالت) {
    UResourceBundle *icudatares = خالي_اشارو;
    
    إذا (U_FAILURE(*حالت)) {
        أرجع;
    }
    
    إذا (معلومات_نسخو_Fillin != خالي_اشارو) {
        icudatares = ures_open_سڌو(خالي_اشارو, U_ICU_نسخو_BUNDLE , حالت);
        إذا (U_SUCCESS(*حالت)) {
            ures_get_نسخو_By_ڪنجي(icudatares, U_ICU_معلومات_ڪنجي, معلومات_نسخو_Fillin, حالت);
        }
        ures_close(icudatares);
    }
}

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

U_CAPI فراغ U_EXPORT2 u_get_انکڑے_نسخہ(UVersionInfo انکڑے_نسخہ_Fillin, UError_کوڈ *حالت) {
    UResourceBundle *icudatares = خالی_سوچک;
    
    إذا (U_FAILURE(*حالت)) {
        أرجع;
    }
    
    إذا (انکڑے_نسخہ_Fillin != خالی_سوچک) {
        icudatares = ures_open_سدھا(خالی_سوچک, U_ICU_نسخہ_BUNDLE , حالت);
        إذا (U_SUCCESS(*حالت)) {
            ures_get_نسخہ_By_کلید(icudatares, U_ICU_انکڑے_کلید, انکڑے_نسخہ_Fillin, حالت);
        }
        ures_close(icudatares);
    }
}

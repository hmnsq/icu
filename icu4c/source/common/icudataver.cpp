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

#شامل "unicode/utypes.h"
#شامل "unicode/icudataver.h"
#شامل "unicode/ures.h"
#شامل "uresimp.h" /* for ures_getVersionByKey */

U_CAPI خالی U_EXPORT2 u_get_ڈیٹا_نسخہ(UVersionInfo ڈیٹا_نسخہ_Fillin, UError_رمز *حالت_2) {
    UResourceBundle *icudatares = خالی_اشاریہ;
    
    اگر (U_FAILURE(*حالت_2)) {
        واپس;
    }
    
    اگر (ڈیٹا_نسخہ_Fillin != خالی_اشاریہ) {
        icudatares = ures_open_براہ_راست(خالی_اشاریہ, U_ICU_نسخہ_BUNDLE , حالت_2);
        اگر (U_SUCCESS(*حالت_2)) {
            ures_get_نسخہ_By_کلید(icudatares, U_ICU_ڈیٹا_کلید, ڈیٹا_نسخہ_Fillin, حالت_2);
        }
        ures_close(icudatares);
    }
}

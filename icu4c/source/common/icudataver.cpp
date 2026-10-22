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

U_CAPI فراغ U_EXPORT2 u_get_معلومات_نسخه(UVersionInfo معلومات_نسخه_Fillin, UError_کوډ *حالت) {
    UResourceBundle *icudatares = تش_اشاره;
    
    إذا (U_FAILURE(*حالت)) {
        أرجع;
    }
    
    إذا (معلومات_نسخه_Fillin != تش_اشاره) {
        icudatares = ures_open_مستقیم(تش_اشاره, U_ICU_نسخه_BUNDLE , حالت);
        إذا (U_SUCCESS(*حالت)) {
            ures_get_نسخه_By_کلی(icudatares, U_ICU_معلومات_کلی, معلومات_نسخه_Fillin, حالت);
        }
        ures_close(icudatares);
    }
}

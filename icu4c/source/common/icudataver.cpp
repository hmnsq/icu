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

U_CAPI فراغ U_EXPORT2 u_get_داده_نسخه(UVersionInfo داده_نسخه_Fillin, UError_کد *وضعیت) {
    UResourceBundle *icudatares = اشاره_تهی;
    
    إذا (U_FAILURE(*وضعیت)) {
        أرجع;
    }
    
    إذا (داده_نسخه_Fillin != اشاره_تهی) {
        icudatares = ures_open_مستقیم(اشاره_تهی, U_ICU_نسخه_BUNDLE , وضعیت);
        إذا (U_SUCCESS(*وضعیت)) {
            ures_get_نسخه_By_کلید(icudatares, U_ICU_داده_کلید, داده_نسخه_Fillin, وضعیت);
        }
        ures_close(icudatares);
    }
}

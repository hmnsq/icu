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

U_CAPI शून्य U_EXPORT2 u_get_दत्त_संस्करणम्(UVersionInfo दत्त_संस्करणम्_Fillin, UError_संहिता *स्थितिः) {
    UResourceBundle *icudatares = शून्यसूचकः;
    
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम;
    }
    
    यदि (दत्त_संस्करणम्_Fillin != शून्यसूचकः) {
        icudatares = ures_open_प्रत्यक्षम्(शून्यसूचकः, U_ICU_संस्करणम्_BUNDLE , स्थितिः);
        यदि (U_SUCCESS(*स्थितिः)) {
            ures_get_संस्करणम्_By_कुञ्चिका(icudatares, U_ICU_दत्त_कुञ्चिका, दत्त_संस्करणम्_Fillin, स्थितिः);
        }
        ures_close(icudatares);
    }
}

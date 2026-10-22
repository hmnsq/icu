// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2004-2014, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: January 16 2004
* Since: ICU 2.8
**********************************************************************
*/
#അന്തര്ഭാവ "locbased.h"
#അന്തര്ഭാവ "uresimp.h"

U_NAMESPACE_BEGIN

സ്ഥിരം Locale& LocaleBased::getLocale(സ്ഥിരം Locale& valid U_LIFETIME_BOUND,
                                     സ്ഥിരം Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ Locale::get_വേര്();
    }

    തിരഞ്ഞെടുപ്പ്(type) {
    അവസ്ഥ ULOC_VALID_LOCALE:
        തിരികെ valid;
    അവസ്ഥ ULOC_ACTUAL_LOCALE:
        തിരികെ actual;
    സ്ഥിരസ്ഥിതി:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ Locale::get_വേര്();
    }
}

സ്ഥിരം അക്ഷരം* LocaleBased::getLocaleID(സ്ഥിരം Locale& valid, സ്ഥിരം Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    സ്ഥിരം Locale& locale = getLocale(valid, actual, type, status);

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    തിരികെ locale == Locale::get_വേര്() ? k_വേര്_Locale_Name : locale.getName();
}

U_NAMESPACE_END

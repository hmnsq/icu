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
#அந்தர்பாவ "locbased.h"
#அந்தர்பாவ "uresimp.h"

U_NAMESPACE_BEGIN

மாறிலி Locale& LocaleBased::getLocale(மாறிலி Locale& valid U_LIFETIME_BOUND,
                                     மாறிலி Locale& actual U_LIFETIME_BOUND,
                                     ULocDataLocaleType type,
                                     UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு Locale::get_வேர்();
    }

    தேர்வு(type) {
    நிலை ULOC_VALID_LOCALE:
        திரும்பு valid;
    நிலை ULOC_ACTUAL_LOCALE:
        திரும்பு actual;
    இயல்பு:
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு Locale::get_வேர்();
    }
}

மாறிலி எழுத்து* LocaleBased::getLocaleID(மாறிலி Locale& valid, மாறிலி Locale& actual,
                                     ULocDataLocaleType type, UErrorCode& status) {
    மாறிலி Locale& locale = getLocale(valid, actual, type, status);

    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }

    திரும்பு locale == Locale::get_வேர்() ? k_வேர்_Locale_Name : locale.getName();
}

U_NAMESPACE_END

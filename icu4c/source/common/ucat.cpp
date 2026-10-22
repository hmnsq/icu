// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2003, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: March 19 2003
* Since: ICU 2.6
**********************************************************************
*/
#ಅನ್ತರ್ಭಾವ "unicode/ucat.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

/* Separator between set_num and msg_num */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_ಕೀಲಿ_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
ಸ್ಥಿರ ವರ್ಣ*
_catkey(ವರ್ಣ* buffer U_LIFETIME_BOUND, int32_t set_ಅಂಕೆ, int32_t ಸಂದೇಶ_ಅಂಕೆ) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_ಅಂಕೆ, 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, ಸಂದೇಶ_ಅಂಕೆ, 10);
    ಪ್ರತ್ಯಾಗಮ buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(ಧ್ರುವ ವರ್ಣ* name, ಧ್ರುವ ವರ್ಣ* locale, UErrorCode* ec) {
    ಪ್ರತ್ಯಾಗಮ (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI ಧ್ರುವ char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_ಅಂಕೆ,
          int32_t ಸಂದೇಶ_ಅಂಕೆ,
          ಧ್ರುವ char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    ವರ್ಣ ಕೀಲಿ[MAX_ಕೀಲಿ_LEN];
    ಧ್ರುವ char16_t* result;

    ಯದಿ (ec == nullptr || U_FAILURE(*ec)) {
        ಗಚ್ಛ ERROR;
    }

    result = ures_get_String_By_ಕೀಲಿ((ಧ್ರುವ UResourceBundle*) catd,
                                 _catkey(ಕೀಲಿ, set_ಅಂಕೆ, ಸಂದೇಶ_ಅಂಕೆ),
                                 len, ec);
    ಯದಿ (U_FAILURE(*ec)) {
        ಗಚ್ಛ ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ result;

 ERROR:
    /* In case of any failure, return s */
    ಯದಿ (len != nullptr) {
        *len = u_strlen(s);
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

/*eof*/

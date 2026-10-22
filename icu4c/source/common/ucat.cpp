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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಕೀಲಿ_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
ಸ್ಥಿರ ಅಕ್ಷರ*
_catkey(ಅಕ್ಷರ* ಬಫರ್ U_LIFETIME_BOUND, int32_t ಗಣ_ಅಂಕೆ, int32_t ಸಂದೇಶ_ಅಂಕೆ) {
    int32_t i = 0;
    i = T_CString_integer_ಗಮ್ಯ_String(ಬಫರ್, ಗಣ_ಅಂಕೆ, 10);
    ಬಫರ್[i++] = SEPARATOR;
    T_CString_integer_ಗಮ್ಯ_String(ಬಫರ್+i, ಸಂದೇಶ_ಅಂಕೆ, 10);
    ಹಿಂತಿರುಗಿ ಬಫರ್;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale, UError_ಸಂಕೇತ* ec) {
    ಹಿಂತಿರುಗಿ (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI ಸ್ಥಿರಾಂಕ char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t ಗಣ_ಅಂಕೆ,
          int32_t ಸಂದೇಶ_ಅಂಕೆ,
          ಸ್ಥಿರಾಂಕ char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UError_ಸಂಕೇತ* ec) {

    ಅಕ್ಷರ ಕೀಲಿ[MAX_ಕೀಲಿ_LEN];
    ಸ್ಥಿರಾಂಕ char16_t* ಫಲಿತಾಂಶ;

    ಆದರೆ (ec == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ec)) {
        ಹೋಗು ERROR;
    }

    ಫಲಿತಾಂಶ = ures_get_String_By_ಕೀಲಿ((ಸ್ಥಿರಾಂಕ UResourceBundle*) catd,
                                 _catkey(ಕೀಲಿ, ಗಣ_ಅಂಕೆ, ಸಂದೇಶ_ಅಂಕೆ),
                                 len, ec);
    ಆದರೆ (U_FAILURE(*ec)) {
        ಹೋಗು ERROR;
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

 ERROR:
    /* In case of any failure, return s */
    ಆದರೆ (len != ಶೂನ್ಯಸೂಚಕ) {
        *len = u_strlen(s);
    }
    ಹಿಂತಿರುಗಿ s;
}

/*eof*/

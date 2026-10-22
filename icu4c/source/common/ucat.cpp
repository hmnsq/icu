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
#അന്തര്ഭാവ "unicode/ucat.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "uassert.h"

/* Separator between set_num and msg_num */
സ്ഥിതം സ്ഥിരം അക്ഷരം SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_താക്കോൽ_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
സ്ഥിതം അക്ഷരം*
_catkey(അക്ഷരം* buffer U_LIFETIME_BOUND, int32_t set_അക്കം, int32_t സന്ദേശം_അക്കം) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_അക്കം, 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, സന്ദേശം_അക്കം, 10);
    തിരികെ buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(സ്ഥിരം അക്ഷരം* name, സ്ഥിരം അക്ഷരം* locale, UErrorCode* ec) {
    തിരികെ (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI ശൂന്യം U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI സ്ഥിരം char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_അക്കം,
          int32_t സന്ദേശം_അക്കം,
          സ്ഥിരം char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    അക്ഷരം താക്കോൽ[MAX_താക്കോൽ_LEN];
    സ്ഥിരം char16_t* result;

    എങ്കിൽ (ec == nullptr || U_FAILURE(*ec)) {
        പോകുക ERROR;
    }

    result = ures_get_String_By_താക്കോൽ((സ്ഥിരം UResourceBundle*) catd,
                                 _catkey(താക്കോൽ, set_അക്കം, സന്ദേശം_അക്കം),
                                 len, ec);
    എങ്കിൽ (U_FAILURE(*ec)) {
        പോകുക ERROR;
    }

    തിരികെ result;

 ERROR:
    /* In case of any failure, return s */
    എങ്കിൽ (len != nullptr) {
        *len = u_strlen(s);
    }
    തിരികെ s;
}

/*eof*/

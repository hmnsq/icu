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
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

/* Separator between set_num and msg_num */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਕੁੰਜੀ_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
ਸਥਿਰ ਅੱਖਰ*
_catkey(ਅੱਖਰ* ਬਫਰ U_LIFETIME_BOUND, int32_t ਸੈੱਟ_ਅੰਕ, int32_t ਸੁਨੇਹਾ_ਅੰਕ) {
    int32_t i = 0;
    i = T_CString_integer_ਮੰਜ਼ਿਲ_String(ਬਫਰ, ਸੈੱਟ_ਅੰਕ, 10);
    ਬਫਰ[i++] = SEPARATOR;
    T_CString_integer_ਮੰਜ਼ਿਲ_String(ਬਫਰ+i, ਸੁਨੇਹਾ_ਅੰਕ, 10);
    ਵਾਪਸ ਬਫਰ;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(ਅਚਲ ਅੱਖਰ* name, ਅਚਲ ਅੱਖਰ* locale, UError_ਕੋਡ* ec) {
    ਵਾਪਸ (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI ਖਾਲੀ U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI ਅਚਲ char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t ਸੈੱਟ_ਅੰਕ,
          int32_t ਸੁਨੇਹਾ_ਅੰਕ,
          ਅਚਲ char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UError_ਕੋਡ* ec) {

    ਅੱਖਰ ਕੁੰਜੀ[MAX_ਕੁੰਜੀ_LEN];
    ਅਚਲ char16_t* ਨਤੀਜਾ;

    ਜੇ (ec == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ec)) {
        ਜਾਓ ERROR;
    }

    ਨਤੀਜਾ = ures_get_String_By_ਕੁੰਜੀ((ਅਚਲ UResourceBundle*) catd,
                                 _catkey(ਕੁੰਜੀ, ਸੈੱਟ_ਅੰਕ, ਸੁਨੇਹਾ_ਅੰਕ),
                                 len, ec);
    ਜੇ (U_FAILURE(*ec)) {
        ਜਾਓ ERROR;
    }

    ਵਾਪਸ ਨਤੀਜਾ;

 ERROR:
    /* In case of any failure, return s */
    ਜੇ (len != ਖਾਲੀ_ਸੂਚਕ) {
        *len = u_strlen(s);
    }
    ਵਾਪਸ s;
}

/*eof*/

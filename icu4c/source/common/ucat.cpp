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
#ضمن "unicode/ucat.h"
#ضمن "unicode/ustring.h"
#ضمن "cstring.h"
#ضمن "uassert.h"

/* Separator between set_num and msg_num */
ساكن ثابت حرف SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_کونچي_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
ساكن حرف*
_catkey(حرف* buffer U_LIFETIME_BOUND, int32_t set_اڠک, int32_t ڤسنن_اڠک) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_اڠک, 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, ڤسنن_اڠک, 10);
    أرجع buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(ثابت حرف* name, ثابت حرف* locale, UErrorCode* ec) {
    أرجع (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI فراغ U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI ثابت char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_اڠک,
          int32_t ڤسنن_اڠک,
          ثابت char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    حرف کونچي[MAX_کونچي_LEN];
    ثابت char16_t* result;

    إذا (ec == nullptr || U_FAILURE(*ec)) {
        اذهب ERROR;
    }

    result = ures_get_String_By_کونچي((ثابت UResourceBundle*) catd,
                                 _catkey(کونچي, set_اڠک, ڤسنن_اڠک),
                                 len, ec);
    إذا (U_FAILURE(*ec)) {
        اذهب ERROR;
    }

    أرجع result;

 ERROR:
    /* In case of any failure, return s */
    إذا (len != nullptr) {
        *len = u_strlen(s);
    }
    أرجع s;
}

/*eof*/

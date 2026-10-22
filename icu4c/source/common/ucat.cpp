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
#અન્તર્ભાવ "unicode/ucat.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "uassert.h"

/* Separator between set_num and msg_num */
સ્થિર અચલ અક્ષર SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_ચાવી_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
સ્થિર અક્ષર*
_catkey(અક્ષર* buffer U_LIFETIME_BOUND, int32_t set_અંક, int32_t સંદેશ_અંક) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_અંક, 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, સંદેશ_અંક, 10);
    પાછા buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(અચલ અક્ષર* name, અચલ અક્ષર* locale, UErrorCode* ec) {
    પાછા (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI શૂન્ય U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI અચલ char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_અંક,
          int32_t સંદેશ_અંક,
          અચલ char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    અક્ષર ચાવી[MAX_ચાવી_LEN];
    અચલ char16_t* result;

    જો (ec == nullptr || U_FAILURE(*ec)) {
        જાઓ ERROR;
    }

    result = ures_get_String_By_ચાવી((અચલ UResourceBundle*) catd,
                                 _catkey(ચાવી, set_અંક, સંદેશ_અંક),
                                 len, ec);
    જો (U_FAILURE(*ec)) {
        જાઓ ERROR;
    }

    પાછા result;

 ERROR:
    /* In case of any failure, return s */
    જો (len != nullptr) {
        *len = u_strlen(s);
    }
    પાછા s;
}

/*eof*/

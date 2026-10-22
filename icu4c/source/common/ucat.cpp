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
#समावेश "unicode/ucat.h"
#समावेश "unicode/ustring.h"
#समावेश "cstring.h"
#समावेश "uassert.h"

/* Separator between set_num and msg_num */
अटल स्थिर अक्षर SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#घोषणा MAX_कुंजी_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
अटल अक्षर*
_catkey(अक्षर* बफर U_LIFETIME_BOUND, int32_t set_अंक, int32_t संदेश_अंक) {
    int32_t i = 0;
    i = T_CString_integerToString(बफर, set_अंक, 10);
    बफर[i++] = SEPARATOR;
    T_CString_integerToString(बफर+i, संदेश_अंक, 10);
    वापस बफर;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(स्थिर अक्षर* name, स्थिर अक्षर* locale, UErrorCode* ec) {
    वापस (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI व्योम U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI स्थिर char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_अंक,
          int32_t संदेश_अंक,
          स्थिर char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    अक्षर कुंजी[MAX_कुंजी_LEN];
    स्थिर char16_t* परिणाम;

    अगर (ec == nullptr || U_FAILURE(*ec)) {
        जाओ ERROR;
    }

    परिणाम = ures_get_String_By_कुंजी((स्थिर UResourceBundle*) catd,
                                 _catkey(कुंजी, set_अंक, संदेश_अंक),
                                 len, ec);
    अगर (U_FAILURE(*ec)) {
        जाओ ERROR;
    }

    वापस परिणाम;

 ERROR:
    /* In case of any failure, return s */
    अगर (len != nullptr) {
        *len = u_strlen(s);
    }
    वापस s;
}

/*eof*/

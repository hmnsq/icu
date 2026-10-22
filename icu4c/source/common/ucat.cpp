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
#अन्तर्भाव "unicode/ucat.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"

/* Separator between set_num and msg_num */
स्थिर स्थिरांक अक्षर SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_किल्ली_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
स्थिर अक्षर*
_catkey(अक्षर* buffer U_LIFETIME_BOUND, int32_t set_अंक, int32_t संदेश_अंक) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_अंक, 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, संदेश_अंक, 10);
    परत buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(स्थिरांक अक्षर* name, स्थिरांक अक्षर* locale, UErrorCode* ec) {
    परत (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI शून्य U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI स्थिरांक char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_अंक,
          int32_t संदेश_अंक,
          स्थिरांक char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    अक्षर किल्ली[MAX_किल्ली_LEN];
    स्थिरांक char16_t* result;

    जर (ec == nullptr || U_FAILURE(*ec)) {
        जा ERROR;
    }

    result = ures_get_String_By_किल्ली((स्थिरांक UResourceBundle*) catd,
                                 _catkey(किल्ली, set_अंक, संदेश_अंक),
                                 len, ec);
    जर (U_FAILURE(*ec)) {
        जा ERROR;
    }

    परत result;

 ERROR:
    /* In case of any failure, return s */
    जर (len != nullptr) {
        *len = u_strlen(s);
    }
    परत s;
}

/*eof*/

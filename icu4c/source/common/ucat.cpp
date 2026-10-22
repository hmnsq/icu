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
स्थिर ध्रुव वर्ण SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_कुंजी_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
स्थिर वर्ण*
_catkey(वर्ण* buffer U_LIFETIME_BOUND, int32_t set_अंक, int32_t संदेश_अंक) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_अंक, 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, संदेश_अंक, 10);
    प्रत्यागम buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(ध्रुव वर्ण* name, ध्रुव वर्ण* locale, UErrorCode* ec) {
    प्रत्यागम (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI शून्य U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI ध्रुव char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_अंक,
          int32_t संदेश_अंक,
          ध्रुव char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    वर्ण कुंजी[MAX_कुंजी_LEN];
    ध्रुव char16_t* result;

    यदि (ec == nullptr || U_FAILURE(*ec)) {
        गच्छ ERROR;
    }

    result = ures_get_String_By_कुंजी((ध्रुव UResourceBundle*) catd,
                                 _catkey(कुंजी, set_अंक, संदेश_अंक),
                                 len, ec);
    यदि (U_FAILURE(*ec)) {
        गच्छ ERROR;
    }

    प्रत्यागम result;

 ERROR:
    /* In case of any failure, return s */
    यदि (len != nullptr) {
        *len = u_strlen(s);
    }
    प्रत्यागम s;
}

/*eof*/

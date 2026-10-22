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
#অন্তর্ভাব "unicode/ucat.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uassert.h"

/* Separator between set_num and msg_num */
স্থির ধ্রুব বর্ণ SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_চাবি_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
স্থির বর্ণ*
_catkey(বর্ণ* buffer U_LIFETIME_BOUND, int32_t set_অঙ্ক, int32_t বার্তা_অঙ্ক) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_অঙ্ক, 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, বার্তা_অঙ্ক, 10);
    প্রত্যাগম buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(ধ্রুব বর্ণ* name, ধ্রুব বর্ণ* locale, UErrorCode* ec) {
    প্রত্যাগম (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI শূন্য U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI ধ্রুব char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_অঙ্ক,
          int32_t বার্তা_অঙ্ক,
          ধ্রুব char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    বর্ণ চাবি[MAX_চাবি_LEN];
    ধ্রুব char16_t* result;

    যদি (ec == nullptr || U_FAILURE(*ec)) {
        গচ্ছ ERROR;
    }

    result = ures_get_String_By_চাবি((ধ্রুব UResourceBundle*) catd,
                                 _catkey(চাবি, set_অঙ্ক, বার্তা_অঙ্ক),
                                 len, ec);
    যদি (U_FAILURE(*ec)) {
        গচ্ছ ERROR;
    }

    প্রত্যাগম result;

 ERROR:
    /* In case of any failure, return s */
    যদি (len != nullptr) {
        *len = u_strlen(s);
    }
    প্রত্যাগম s;
}

/*eof*/

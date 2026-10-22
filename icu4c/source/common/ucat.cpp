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
#شامل "unicode/ucat.h"
#شامل "unicode/ustring.h"
#شامل "cstring.h"
#شامل "uassert.h"

/* Separator between set_num and msg_num */
ایستا ثابت نویسه SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_کلید_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
ایستا نویسه*
_catkey(نویسه* buffer U_LIFETIME_BOUND, int32_t set_رقم, int32_t پیام_رقم) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_رقم, 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, پیام_رقم, 10);
    بازگشت buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(ثابت نویسه* name, ثابت نویسه* locale, UErrorCode* ec) {
    بازگشت (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI تهی U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI ثابت char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_رقم,
          int32_t پیام_رقم,
          ثابت char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    نویسه کلید[MAX_کلید_LEN];
    ثابت char16_t* result;

    اگر (ec == nullptr || U_FAILURE(*ec)) {
        برو ERROR;
    }

    result = ures_get_String_By_کلید((ثابت UResourceBundle*) catd,
                                 _catkey(کلید, set_رقم, پیام_رقم),
                                 len, ec);
    اگر (U_FAILURE(*ec)) {
        برو ERROR;
    }

    بازگشت result;

 ERROR:
    /* In case of any failure, return s */
    اگر (len != nullptr) {
        *len = u_strlen(s);
    }
    بازگشت s;
}

/*eof*/

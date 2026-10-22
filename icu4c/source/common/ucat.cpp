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
#அந்தர்பாவ "unicode/ucat.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uassert.h"

/* Separator between set_num and msg_num */
நிலையான மாறிலி எழுத்து SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_சாவி_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
நிலையான எழுத்து*
_catkey(எழுத்து* buffer U_LIFETIME_BOUND, int32_t set_இலக்கம், int32_t செய்தி_இலக்கம்) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_இலக்கம், 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, செய்தி_இலக்கம், 10);
    திரும்பு buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(மாறிலி எழுத்து* name, மாறிலி எழுத்து* locale, UErrorCode* ec) {
    திரும்பு (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI வெறுமை U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI மாறிலி char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_இலக்கம்,
          int32_t செய்தி_இலக்கம்,
          மாறிலி char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    எழுத்து சாவி[MAX_சாவி_LEN];
    மாறிலி char16_t* result;

    எனில் (ec == nullptr || U_FAILURE(*ec)) {
        செல் ERROR;
    }

    result = ures_get_String_By_சாவி((மாறிலி UResourceBundle*) catd,
                                 _catkey(சாவி, set_இலக்கம், செய்தி_இலக்கம்),
                                 len, ec);
    எனில் (U_FAILURE(*ec)) {
        செல் ERROR;
    }

    திரும்பு result;

 ERROR:
    /* In case of any failure, return s */
    எனில் (len != nullptr) {
        *len = u_strlen(s);
    }
    திரும்பு s;
}

/*eof*/

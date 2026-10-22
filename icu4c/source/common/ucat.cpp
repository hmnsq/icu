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
#ଅନ୍ତର୍ଭାଵ "unicode/ucat.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

/* Separator between set_num and msg_num */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_ଚାବି_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
ସ୍ଥିର ଅକ୍ଷର*
_catkey(ଅକ୍ଷର* buffer U_LIFETIME_BOUND, int32_t set_ଅଙ୍କ, int32_t ସନ୍ଦେଶ_ଅଙ୍କ) {
    int32_t i = 0;
    i = T_CString_integerToString(buffer, set_ଅଙ୍କ, 10);
    buffer[i++] = SEPARATOR;
    T_CString_integerToString(buffer+i, ସନ୍ଦେଶ_ଅଙ୍କ, 10);
    ଫେରନ୍ତୁ buffer;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(ଅଚଳ ଅକ୍ଷର* name, ଅଚଳ ଅକ୍ଷର* locale, UErrorCode* ec) {
    ଫେରନ୍ତୁ (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI ଅଚଳ char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_ଅଙ୍କ,
          int32_t ସନ୍ଦେଶ_ଅଙ୍କ,
          ଅଚଳ char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    ଅକ୍ଷର ଚାବି[MAX_ଚାବି_LEN];
    ଅଚଳ char16_t* result;

    ଯଦି (ec == nullptr || U_FAILURE(*ec)) {
        ଯାଅ ERROR;
    }

    result = ures_get_String_By_ଚାବି((ଅଚଳ UResourceBundle*) catd,
                                 _catkey(ଚାବି, set_ଅଙ୍କ, ସନ୍ଦେଶ_ଅଙ୍କ),
                                 len, ec);
    ଯଦି (U_FAILURE(*ec)) {
        ଯାଅ ERROR;
    }

    ଫେରନ୍ତୁ result;

 ERROR:
    /* In case of any failure, return s */
    ଯଦି (len != nullptr) {
        *len = u_strlen(s);
    }
    ଫେରନ୍ତୁ s;
}

/*eof*/

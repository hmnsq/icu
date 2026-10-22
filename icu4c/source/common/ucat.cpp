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
স্থির ধ্রুবক অক্ষর SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#define MAX_চাবি_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
স্থির অক্ষর*
_catkey(অক্ষর* বাফার U_LIFETIME_BOUND, int32_t set_অঙ্ক, int32_t বার্তা_অঙ্ক) {
    int32_t i = 0;
    i = T_CString_integerToString(বাফার, set_অঙ্ক, 10);
    বাফার[i++] = SEPARATOR;
    T_CString_integerToString(বাফার+i, বার্তা_অঙ্ক, 10);
    ফেরত বাফার;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(ধ্রুবক অক্ষর* name, ধ্রুবক অক্ষর* locale, UErrorCode* ec) {
    ফেরত (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI শূন্য U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI ধ্রুবক char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t set_অঙ্ক,
          int32_t বার্তা_অঙ্ক,
          ধ্রুবক char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UErrorCode* ec) {

    অক্ষর চাবি[MAX_চাবি_LEN];
    ধ্রুবক char16_t* ফলাফল;

    যদি (ec == nullptr || U_FAILURE(*ec)) {
        যাও ERROR;
    }

    ফলাফল = ures_get_String_By_চাবি((ধ্রুবক UResourceBundle*) catd,
                                 _catkey(চাবি, set_অঙ্ক, বার্তা_অঙ্ক),
                                 len, ec);
    যদি (U_FAILURE(*ec)) {
        যাও ERROR;
    }

    ফেরত ফলাফল;

 ERROR:
    /* In case of any failure, return s */
    যদি (len != nullptr) {
        *len = u_strlen(s);
    }
    ফেরত s;
}

/*eof*/

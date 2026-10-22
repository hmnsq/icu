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
#అన్తర్భావ "unicode/ucat.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"

/* Separator between set_num and msg_num */
స్థిర స్థిరరాశి అక్షరం SEPARATOR = '%';

/* Maximum length of a set_num/msg_num key, incl. terminating zero.
 * Longest possible key is "-2147483648%-2147483648" */
#నిర్వచించు MAX_కీలకం_LEN (24)

/**
 * Fill in buffer with a set_num/msg_num key string, given the numeric
 * values. Numeric values must be >= 0. Buffer must be of length
 * MAX_KEY_LEN or more.
 */
స్థిర అక్షరం*
_catkey(అక్షరం* బఫరు U_LIFETIME_BOUND, int32_t సమితి_అంకె, int32_t సందేశం_అంకె) {
    int32_t i = 0;
    i = T_CString_integer_గమ్యం_String(బఫరు, సమితి_అంకె, 10);
    బఫరు[i++] = SEPARATOR;
    T_CString_integer_గమ్యం_String(బఫరు+i, సందేశం_అంకె, 10);
    తిరిగి బఫరు;
}

U_CAPI u_nl_catd U_EXPORT2
u_catopen(స్థిరరాశి అక్షరం* name, స్థిరరాశి అక్షరం* locale, UError_సంకేతం* ec) {
    తిరిగి (u_nl_catd) ures_open(name, locale, ec);
}

U_CAPI శూన్య U_EXPORT2
u_catclose(u_nl_catd catd) {
    ures_close((UResourceBundle*) catd); /* may be nullptr */
}

U_CAPI స్థిరరాశి char16_t* U_EXPORT2
u_catgets(u_nl_catd catd U_LIFETIME_BOUND,
          int32_t సమితి_అంకె,
          int32_t సందేశం_అంకె,
          స్థిరరాశి char16_t* s U_LIFETIME_BOUND,
          int32_t* len,
          UError_సంకేతం* ec) {

    అక్షరం కీలకం[MAX_కీలకం_LEN];
    స్థిరరాశి char16_t* ఫలితం;

    ఒకవేళ (ec == శూన్యసూచిక || U_FAILURE(*ec)) {
        వెళ్ళు ERROR;
    }

    ఫలితం = ures_get_String_By_కీలకం((స్థిరరాశి UResourceBundle*) catd,
                                 _catkey(కీలకం, సమితి_అంకె, సందేశం_అంకె),
                                 len, ec);
    ఒకవేళ (U_FAILURE(*ec)) {
        వెళ్ళు ERROR;
    }

    తిరిగి ఫలితం;

 ERROR:
    /* In case of any failure, return s */
    ఒకవేళ (len != శూన్యసూచిక) {
        *len = u_strlen(s);
    }
    తిరిగి s;
}

/*eof*/

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2007, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucnv_set.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004sep07
*   created by: Markus W. Scherer
*
*   Conversion API functions using USet (ucnv_getUnicodeSet())
*   moved here from ucnv.c for removing the dependency of other ucnv_
*   implementation functions on the USet implementation.
*/

#شامل "unicode/utypes.h"
#شامل "unicode/uset.h"
#شامل "unicode/ucnv.h"
#شامل "ucnv_bld.h"
#شامل "uset_imp.h"

#اگر !UCONFIG_NO_CONVERSION

U_CAPI خالی U_EXPORT2
ucnv_get_Unicode_سیٹ(ثابت UConverter *cnv,
                   USet *سیٹ_Fill_اندر,
                   UConverter_Unicode_سیٹ which_سیٹ,
                   UError_رمز *p_Error_رمز) {
    /* argument checking */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس;
    }
    اگر(cnv==خالی_اشاریہ || سیٹ_Fill_اندر==خالی_اشاریہ || which_سیٹ<UCNV_ROUNDTRIP_سیٹ || UCNV_سیٹ_گنتی<=which_سیٹ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    /* does this converter support this function? */
    اگر(cnv->shared_ڈیٹا->impl->get_Unicode_سیٹ==خالی_اشاریہ) {
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس;
    }

    {
        USetAdder sa={
            خالی_اشاریہ,
            uset_جوڑو,
            uset_جوڑو_Range,
            uset_جوڑو_String,
            uset_remove,
            uset_removeRange
        };
        sa.سیٹ=سیٹ_Fill_اندر;

        /* empty the set */
        uset_clear(سیٹ_Fill_اندر);

        /* call the converter to add the code points it supports */
        cnv->shared_ڈیٹا->impl->get_Unicode_سیٹ(cnv, &sa, which_سیٹ, p_Error_رمز);
    }
}

#اگر_ختم

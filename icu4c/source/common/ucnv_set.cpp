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

#ضمن "unicode/utypes.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/ucnv.h"
#ضمن "ucnv_bld.h"
#ضمن "uset_imp.h"

#إذا !UCONFIG_NO_CONVERSION

U_CAPI فراغ U_EXPORT2
ucnv_get_Unicode_سیٹ(ثابت UConverter *cnv,
                   USet *سیٹ_Fill_وچ,
                   UConverter_Unicode_سیٹ which_سیٹ,
                   UError_کوڈ *p_Error_کوڈ) {
    /* argument checking */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }
    إذا(cnv==خالی_سوچک || سیٹ_Fill_وچ==خالی_سوچک || which_سیٹ<UCNV_ROUNDTRIP_سیٹ || UCNV_سیٹ_گنتی<=which_سیٹ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    /* does this converter support this function? */
    إذا(cnv->shared_انکڑے->impl->get_Unicode_سیٹ==خالی_سوچک) {
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع;
    }

    {
        USetAdder sa={
            خالی_سوچک,
            uset_جوڑو,
            uset_جوڑو_Range,
            uset_جوڑو_String,
            uset_remove,
            uset_removeRange
        };
        sa.سیٹ=سیٹ_Fill_وچ;

        /* empty the set */
        uset_clear(سیٹ_Fill_وچ);

        /* call the converter to add the code points it supports */
        cnv->shared_انکڑے->impl->get_Unicode_سیٹ(cnv, &sa, which_سیٹ, p_Error_کوڈ);
    }
}

#جے_انت

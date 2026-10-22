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
ucnv_get_Unicode_سېټ(ثابت UConverter *cnv,
                   USet *سېټ_Fill_دننه,
                   UConverter_Unicode_سېټ which_سېټ,
                   UError_کوډ *p_Error_کوډ) {
    /* argument checking */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }
    إذا(cnv==تش_اشاره || سېټ_Fill_دننه==تش_اشاره || which_سېټ<UCNV_ROUNDTRIP_سېټ || UCNV_سېټ_شمېر<=which_سېټ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    /* does this converter support this function? */
    إذا(cnv->shared_معلومات->impl->get_Unicode_سېټ==تش_اشاره) {
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع;
    }

    {
        USetAdder sa={
            تش_اشاره,
            uset_جمع_کړه,
            uset_جمع_کړه_Range,
            uset_جمع_کړه_String,
            uset_remove,
            uset_removeRange
        };
        sa.سېټ=سېټ_Fill_دننه;

        /* empty the set */
        uset_clear(سېټ_Fill_دننه);

        /* call the converter to add the code points it supports */
        cnv->shared_معلومات->impl->get_Unicode_سېټ(cnv, &sa, which_سېټ, p_Error_کوډ);
    }
}

#د_که_پای

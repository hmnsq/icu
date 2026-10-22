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
ucnv_get_Unicode_سيٽ(ثابت UConverter *cnv,
                   USet *سيٽ_Fill_اندريون,
                   UConverter_Unicode_سيٽ which_سيٽ,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    /* argument checking */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }
    إذا(cnv==خالي_اشارو || سيٽ_Fill_اندريون==خالي_اشارو || which_سيٽ<UCNV_ROUNDTRIP_سيٽ || UCNV_سيٽ_ڳڻپ<=which_سيٽ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    /* does this converter support this function? */
    إذا(cnv->shared_معلومات->impl->get_Unicode_سيٽ==خالي_اشارو) {
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع;
    }

    {
        USetAdder sa={
            خالي_اشارو,
            uset_جوڙيو,
            uset_جوڙيو_Range,
            uset_جوڙيو_String,
            uset_remove,
            uset_removeRange
        };
        sa.سيٽ=سيٽ_Fill_اندريون;

        /* empty the set */
        uset_clear(سيٽ_Fill_اندريون);

        /* call the converter to add the code points it supports */
        cnv->shared_معلومات->impl->get_Unicode_سيٽ(cnv, &sa, which_سيٽ, p_Error_ڪوڊ);
    }
}

#جيڪڏهن_پورو

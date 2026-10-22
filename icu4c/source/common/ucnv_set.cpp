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
ucnv_get_Unicode_مجموعه(ثابت UConverter *cnv,
                   USet *مجموعه_Fill_درون,
                   UConverter_Unicode_مجموعه which_مجموعه,
                   UError_کد *p_Error_کد) {
    /* argument checking */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع;
    }
    إذا(cnv==اشاره_تهی || مجموعه_Fill_درون==اشاره_تهی || which_مجموعه<UCNV_ROUNDTRIP_مجموعه || UCNV_مجموعه_شمار<=which_مجموعه) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    /* does this converter support this function? */
    إذا(cnv->shared_داده->impl->get_Unicode_مجموعه==اشاره_تهی) {
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع;
    }

    {
        USetAdder sa={
            اشاره_تهی,
            uset_جمع_کن,
            uset_جمع_کن_Range,
            uset_جمع_کن_String,
            uset_remove,
            uset_removeRange
        };
        sa.مجموعه=مجموعه_Fill_درون;

        /* empty the set */
        uset_clear(مجموعه_Fill_درون);

        /* call the converter to add the code points it supports */
        cnv->shared_داده->impl->get_Unicode_مجموعه(cnv, &sa, which_مجموعه, p_Error_کد);
    }
}

#پایان_اگر

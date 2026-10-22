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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uset_imp.h"

#ਜੇ !UCONFIG_NO_CONVERSION

U_CAPI ਖਾਲੀ U_EXPORT2
ucnv_get_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                   USet *ਸੈੱਟ_Fill_ਵਿੱਚ,
                   UConverter_Unicode_ਸੈੱਟ which_ਸੈੱਟ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    /* argument checking */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ(cnv==ਖਾਲੀ_ਸੂਚਕ || ਸੈੱਟ_Fill_ਵਿੱਚ==ਖਾਲੀ_ਸੂਚਕ || which_ਸੈੱਟ<UCNV_ROUNDTRIP_ਸੈੱਟ || UCNV_ਸੈੱਟ_ਗਿਣਤੀ<=which_ਸੈੱਟ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    /* does this converter support this function? */
    ਜੇ(cnv->shared_ਅੰਕੜੇ->impl->get_Unicode_ਸੈੱਟ==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ;
    }

    {
        USetAdder sa={
            ਖਾਲੀ_ਸੂਚਕ,
            uset_ਜੋੜੋ,
            uset_ਜੋੜੋ_Range,
            uset_ਜੋੜੋ_String,
            uset_remove,
            uset_removeRange
        };
        sa.ਸੈੱਟ=ਸੈੱਟ_Fill_ਵਿੱਚ;

        /* empty the set */
        uset_clear(ਸੈੱਟ_Fill_ਵਿੱਚ);

        /* call the converter to add the code points it supports */
        cnv->shared_ਅੰਕੜੇ->impl->get_Unicode_ਸੈੱਟ(cnv, &sa, which_ਸੈੱਟ, p_Error_ਕੋਡ);
    }
}

#ਜੇ_ਅੰਤ

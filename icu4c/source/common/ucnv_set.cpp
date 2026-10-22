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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uset.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "ucnv_bld.h"
#അന്തര്ഭാവ "uset_imp.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

U_CAPI ശൂന്യം U_EXPORT2
ucnv_getUnicodeSet(സ്ഥിരം UConverter *cnv,
                   USet *setFillIn,
                   UConverterUnicodeSet whichSet,
                   UErrorCode *pErrorCode) {
    /* argument checking */
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }
    എങ്കിൽ(cnv==nullptr || setFillIn==nullptr || whichSet<UCNV_ROUNDTRIP_SET || UCNV_SET_എണ്ണം<=whichSet) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    /* does this converter support this function? */
    എങ്കിൽ(cnv->sharedData->impl->getUnicodeSet==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        തിരികെ;
    }

    {
        USetAdder sa={
            nullptr,
            uset_add,
            uset_addRange,
            uset_addString,
            uset_remove,
            uset_removeRange
        };
        sa.set=setFillIn;

        /* empty the set */
        uset_clear(setFillIn);

        /* call the converter to add the code points it supports */
        cnv->sharedData->impl->getUnicodeSet(cnv, &sa, whichSet, pErrorCode);
    }
}

#endif

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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uset.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucnv.h"
#ଅନ୍ତର୍ଭାଵ "ucnv_bld.h"
#ଅନ୍ତର୍ଭାଵ "uset_imp.h"

#ଯଦି !UCONFIG_NO_CONVERSION

U_CAPI ଶୂନ୍ୟ U_EXPORT2
ucnv_getUnicodeSet(ଅଚଳ UConverter *cnv,
                   USet *setFillIn,
                   UConverterUnicodeSet whichSet,
                   UErrorCode *pErrorCode) {
    /* argument checking */
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(cnv==nullptr || setFillIn==nullptr || whichSet<UCNV_ROUNDTRIP_SET || UCNV_SET_ଗଣନା<=whichSet) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }

    /* does this converter support this function? */
    ଯଦି(cnv->sharedData->impl->getUnicodeSet==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ଫେରନ୍ତୁ;
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

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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uset.h"
#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "ucnv_bld.h"
#அந்தர்பாவ "uset_imp.h"

#எனில் !UCONFIG_NO_CONVERSION

U_CAPI வெறுமை U_EXPORT2
ucnv_getUnicodeSet(மாறிலி UConverter *cnv,
                   USet *setFillIn,
                   UConverterUnicodeSet whichSet,
                   UErrorCode *pErrorCode) {
    /* argument checking */
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }
    எனில்(cnv==nullptr || setFillIn==nullptr || whichSet<UCNV_ROUNDTRIP_SET || UCNV_SET_மொத்தம்<=whichSet) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    /* does this converter support this function? */
    எனில்(cnv->sharedData->impl->getUnicodeSet==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        திரும்பு;
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

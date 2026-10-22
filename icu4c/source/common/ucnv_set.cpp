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

#समावेश "unicode/utypes.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/ucnv.h"
#समावेश "ucnv_bld.h"
#समावेश "uset_imp.h"

#अगर !UCONFIG_NO_CONVERSION

U_CAPI व्योम U_EXPORT2
ucnv_getUnicodeSet(स्थिर UConverter *cnv,
                   USet *setFillIn,
                   UConverterUnicodeSet whichSet,
                   UErrorCode *pErrorCode) {
    /* argument checking */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस;
    }
    अगर(cnv==nullptr || setFillIn==nullptr || whichSet<UCNV_ROUNDTRIP_SET || UCNV_SET_गिनती<=whichSet) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    /* does this converter support this function? */
    अगर(cnv->shared_तथ्य->impl->getUnicodeSet==nullptr) {
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस;
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
        cnv->shared_तथ्य->impl->getUnicodeSet(cnv, &sa, whichSet, pErrorCode);
    }
}

#पूर्ण_अगर

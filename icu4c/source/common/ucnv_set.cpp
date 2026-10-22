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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "uset_imp.h"

#यदि !UCONFIG_NO_CONVERSION

U_CAPI शून्य U_EXPORT2
ucnv_get_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                   USet *समुच्चयः_Fill_अन्तर्,
                   UConverter_Unicode_समुच्चयः which_समुच्चयः,
                   UError_संहिता *p_Error_संहिता) {
    /* argument checking */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }
    यदि(cnv==शून्यसूचकः || समुच्चयः_Fill_अन्तर्==शून्यसूचकः || which_समुच्चयः<UCNV_ROUNDTRIP_समुच्चयः || UCNV_समुच्चयः_गणनम्<=which_समुच्चयः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    /* does this converter support this function? */
    यदि(cnv->shared_दत्त->impl->get_Unicode_समुच्चयः==शून्यसूचकः) {
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम;
    }

    {
        USetAdder sa={
            शून्यसूचकः,
            uset_सङ्कलय,
            uset_सङ्कलय_Range,
            uset_सङ्कलय_String,
            uset_remove,
            uset_removeRange
        };
        sa.समुच्चयः=समुच्चयः_Fill_अन्तर्;

        /* empty the set */
        uset_clear(समुच्चयः_Fill_अन्तर्);

        /* call the converter to add the code points it supports */
        cnv->shared_दत्त->impl->get_Unicode_समुच्चयः(cnv, &sa, which_समुच्चयः, p_Error_संहिता);
    }
}

#यदि_अन्तः

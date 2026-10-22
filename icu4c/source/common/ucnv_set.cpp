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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "uset_imp.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

U_CAPI శూన్య U_EXPORT2
ucnv_get_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                   USet *సమితి_Fill_లోపల,
                   UConverter_Unicode_సమితి which_సమితి,
                   UError_సంకేతం *p_Error_సంకేతం) {
    /* argument checking */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ(cnv==శూన్యసూచిక || సమితి_Fill_లోపల==శూన్యసూచిక || which_సమితి<UCNV_ROUNDTRIP_సమితి || UCNV_సమితి_లెక్క<=which_సమితి) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    /* does this converter support this function? */
    ఒకవేళ(cnv->shared_దత్తాంశం->impl->get_Unicode_సమితి==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి;
    }

    {
        USetAdder sa={
            శూన్యసూచిక,
            uset_కూడు,
            uset_కూడు_Range,
            uset_కూడు_String,
            uset_remove,
            uset_removeRange
        };
        sa.సమితి=సమితి_Fill_లోపల;

        /* empty the set */
        uset_clear(సమితి_Fill_లోపల);

        /* call the converter to add the code points it supports */
        cnv->shared_దత్తాంశం->impl->get_Unicode_సమితి(cnv, &sa, which_సమితి, p_Error_సంకేతం);
    }
}

#అయితే_ముగింపు

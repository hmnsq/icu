// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2001-2015, International Business Machines
*                Corporation and others. All Rights Reserved.
******************************************************************************
*   file name:  uinit.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001July05
*   created by: George Rhoten
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/icuplug.h"
#అన్తర్భావ "unicode/uclean.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "icuplugimp.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "ucnv_io.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "utracimp.h"

U_NAMESPACE_BEGIN

స్థిర UInitOnce gICUInitOnce {};

స్థిర UBool U_CALLCONV uinit_cleanup() {
    gICUInitOnce.reset();
    తిరిగి నిజం;
}

స్థిర శూన్య U_CALLCONV
init_దత్తాంశం(UError_సంకేతం &స్థితి_2)
{
#ఒకవేళ UCONFIG_ENABLE_PLUGINS
    /* initialize plugins */
    uplug_init(&స్థితి_2);
#అయితే_ముగింపు

#ఒకవేళ !UCONFIG_NO_CONVERSION
    /*
     * 2005-may-02
     *
     * ICU4C 3.4 (jitterbug 4497) hardcodes the data for Unicode character
     * properties for APIs that want to be fast.
     * Therefore, we need not load them here nor check for errors.
     * Instead, we load the converter alias table to see if any ICU data
     * is available.
     * Users should really open the service objects they need and check
     * for errors there, to make sure that the actual items they need are
     * available.
     */
    ucnv_io_లెక్క_తెలిసిన_Converters(&స్థితి_2);
#అయితే_ముగింపు
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_UINIT, uinit_cleanup);
}

U_NAMESPACE_చివర

U_NAMESPACE_USE

/*
 * ICU Initialization Function. Need not be called.
 */
U_CAPI శూన్య U_EXPORT2
u_init(UError_సంకేతం *స్థితి_2) {
    UTRACE_నమోదు_OC(UTRACE_U_INIT);
    umtx_initOnce(gICUInitOnce, &init_దత్తాంశం, *స్థితి_2);
    UTRACE_EXIT_స్థితి(*స్థితి_2);
}

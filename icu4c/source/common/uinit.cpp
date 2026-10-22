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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/icuplug.h"
#અન્તર્ભાવ "unicode/uclean.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "icuplugimp.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "ucnv_io.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "utracimp.h"

U_NAMESPACE_BEGIN

સ્થિર UInitOnce gICUInitOnce {};

સ્થિર UBool U_CALLCONV uinit_cleanup() {
    gICUInitOnce.reset();
    પાછા સાચું;
}

સ્થિર શૂન્ય U_CALLCONV
initData(UErrorCode &status)
{
#જો UCONFIG_ENABLE_PLUGINS
    /* initialize plugins */
    uplug_init(&status);
#endif

#જો !UCONFIG_NO_CONVERSION
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
    ucnv_io_ગણતરી_Known_Converters(&status);
#endif
    ucln_common_registerCleanup(UCLN_COMMON_UINIT, uinit_cleanup);
}

U_NAMESPACE_END

U_NAMESPACE_USE

/*
 * ICU Initialization Function. Need not be called.
 */
U_CAPI શૂન્ય U_EXPORT2
u_init(UErrorCode *status) {
    UTRACE_ENTRY_OC(UTRACE_U_INIT);
    umtx_initOnce(gICUInitOnce, &initData, *status);
    UTRACE_EXIT_STATUS(*status);
}

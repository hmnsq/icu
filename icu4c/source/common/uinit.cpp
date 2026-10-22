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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/icuplug.h"
#അന്തര്ഭാവ "unicode/uclean.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "icuplugimp.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "ucnv_io.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "utracimp.h"

U_NAMESPACE_BEGIN

സ്ഥിതം UInitOnce gICUInitOnce {};

സ്ഥിതം UBool U_CALLCONV uinit_cleanup() {
    gICUInitOnce.reset();
    തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
initData(UErrorCode &status)
{
#എങ്കിൽ UCONFIG_ENABLE_PLUGINS
    /* initialize plugins */
    uplug_init(&status);
#endif

#എങ്കിൽ !UCONFIG_NO_CONVERSION
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
    ucnv_io_എണ്ണം_Known_Converters(&status);
#endif
    ucln_common_registerCleanup(UCLN_COMMON_UINIT, uinit_cleanup);
}

U_NAMESPACE_END

U_NAMESPACE_USE

/*
 * ICU Initialization Function. Need not be called.
 */
U_CAPI ശൂന്യം U_EXPORT2
u_init(UErrorCode *status) {
    UTRACE_ENTRY_OC(UTRACE_U_INIT);
    umtx_initOnce(gICUInitOnce, &initData, *status);
    UTRACE_EXIT_STATUS(*status);
}

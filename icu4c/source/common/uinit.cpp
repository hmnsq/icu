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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/icuplug.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uclean.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "icuplugimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_io.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utracimp.h"

U_NAMESPACE_BEGIN

ਸਥਿਰ UInitOnce gICUInitOnce {};

ਸਥਿਰ UBool U_CALLCONV uinit_cleanup() {
    gICUInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
init_ਅੰਕੜੇ(UError_ਕੋਡ &ਸਥਿਤੀ)
{
#ਜੇ UCONFIG_ENABLE_PLUGINS
    /* initialize plugins */
    uplug_init(&ਸਥਿਤੀ);
#ਜੇ_ਅੰਤ

#ਜੇ !UCONFIG_NO_CONVERSION
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
    ucnv_io_ਗਿਣਤੀ_ਗਿਆਤ_Converters(&ਸਥਿਤੀ);
#ਜੇ_ਅੰਤ
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_UINIT, uinit_cleanup);
}

U_NAMESPACE_ਸਿਰਾ

U_NAMESPACE_USE

/*
 * ICU Initialization Function. Need not be called.
 */
U_CAPI ਖਾਲੀ U_EXPORT2
u_init(UError_ਕੋਡ *ਸਥਿਤੀ) {
    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_U_INIT);
    umtx_initOnce(gICUInitOnce, &init_ਅੰਕੜੇ, *ਸਥਿਤੀ);
    UTRACE_EXIT_ਸਥਿਤੀ(*ਸਥਿਤੀ);
}

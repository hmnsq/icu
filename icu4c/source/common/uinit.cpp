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

#ضمن "unicode/utypes.h"
#ضمن "unicode/icuplug.h"
#ضمن "unicode/uclean.h"
#ضمن "cmemory.h"
#ضمن "icuplugimp.h"
#ضمن "ucln_cmn.h"
#ضمن "ucnv_io.h"
#ضمن "umutex.h"
#ضمن "utracimp.h"

U_NAMESPACE_BEGIN

ساكن UInitOnce gICUInitOnce {};

ساكن UBool U_CALLCONV uinit_cleanup() {
    gICUInitOnce.reset();
    أرجع ریښتیا;
}

ساكن فراغ U_CALLCONV
init_معلومات(UError_کوډ &حالت)
{
#إذا UCONFIG_ENABLE_PLUGINS
    /* initialize plugins */
    uplug_init(&حالت);
#د_که_پای

#إذا !UCONFIG_NO_CONVERSION
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
    ucnv_io_شمېر_معلوم_Converters(&حالت);
#د_که_پای
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_UINIT, uinit_cleanup);
}

U_NAMESPACE_پای_ټکی

U_NAMESPACE_USE

/*
 * ICU Initialization Function. Need not be called.
 */
U_CAPI فراغ U_EXPORT2
u_init(UError_کوډ *حالت) {
    UTRACE_ثبت_OC(UTRACE_U_INIT);
    umtx_initOnce(gICUInitOnce, &init_معلومات, *حالت);
    UTRACE_EXIT_حالت(*حالت);
}

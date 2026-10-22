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

#شامل "unicode/utypes.h"
#شامل "unicode/icuplug.h"
#شامل "unicode/uclean.h"
#شامل "cmemory.h"
#شامل "icuplugimp.h"
#شامل "ucln_cmn.h"
#شامل "ucnv_io.h"
#شامل "umutex.h"
#شامل "utracimp.h"

U_NAMESPACE_BEGIN

ساکن UInitOnce gICUInitOnce {};

ساکن UBool U_CALLCONV uinit_cleanup() {
    gICUInitOnce.reset();
    واپس سچ;
}

ساکن خالی U_CALLCONV
init_ڈیٹا(UError_رمز &حالت_2)
{
#اگر UCONFIG_ENABLE_PLUGINS
    /* initialize plugins */
    uplug_init(&حالت_2);
#اگر_ختم

#اگر !UCONFIG_NO_CONVERSION
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
    ucnv_io_گنتی_معلوم_Converters(&حالت_2);
#اگر_ختم
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UINIT, uinit_cleanup);
}

U_NAMESPACE_انتہا

U_NAMESPACE_USE

/*
 * ICU Initialization Function. Need not be called.
 */
U_CAPI خالی U_EXPORT2
u_init(UError_رمز *حالت_2) {
    UTRACE_اندراج_OC(UTRACE_U_INIT);
    umtx_initOnce(gICUInitOnce, &init_ڈیٹا, *حالت_2);
    UTRACE_EXIT_حالت(*حالت_2);
}

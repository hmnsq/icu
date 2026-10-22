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

#समावेश "unicode/utypes.h"
#समावेश "unicode/icuplug.h"
#समावेश "unicode/uclean.h"
#समावेश "cmemory.h"
#समावेश "icuplugimp.h"
#समावेश "ucln_cmn.h"
#समावेश "ucnv_io.h"
#समावेश "umutex.h"
#समावेश "utracimp.h"

U_NAMESPACE_BEGIN

अटल UInitOnce gICUInitOnce {};

अटल UBool U_CALLCONV uinit_cleanup() {
    gICUInitOnce.reset();
    वापस true;
}

अटल व्योम U_CALLCONV
init_तथ्य(UErrorCode &status)
{
#अगर UCONFIG_ENABLE_PLUGINS
    /* initialize plugins */
    uplug_init(&status);
#पूर्ण_अगर

#अगर !UCONFIG_NO_CONVERSION
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
    ucnv_io_गिनती_Known_Converters(&status);
#पूर्ण_अगर
    ucln_common_registerCleanup(UCLN_COMMON_UINIT, uinit_cleanup);
}

U_NAMESPACE_END

U_NAMESPACE_USE

/*
 * ICU Initialization Function. Need not be called.
 */
U_CAPI व्योम U_EXPORT2
u_init(UErrorCode *status) {
    UTRACE_ENTRY_OC(UTRACE_U_INIT);
    umtx_initOnce(gICUInitOnce, &init_तथ्य, *status);
    UTRACE_EXIT_STATUS(*status);
}

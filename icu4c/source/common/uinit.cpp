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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/icuplug.h"
#अन्तर्भाव "unicode/uclean.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "icuplugimp.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "ucnv_io.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "utracimp.h"

U_NAMESPACE_BEGIN

स्थिर UInitOnce gICUInitOnce {};

स्थिर UBool U_CALLCONV uinit_cleanup() {
    gICUInitOnce.reset();
    प्रत्यागम सत्य;
}

स्थिर शून्य U_CALLCONV
init_दत्त(UError_संहिता &स्थितिः)
{
#यदि UCONFIG_ENABLE_PLUGINS
    /* initialize plugins */
    uplug_init(&स्थितिः);
#यदि_अन्तः

#यदि !UCONFIG_NO_CONVERSION
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
    ucnv_io_गणनम्_ज्ञातम्_Converters(&स्थितिः);
#यदि_अन्तः
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_UINIT, uinit_cleanup);
}

U_NAMESPACE_अन्तः

U_NAMESPACE_USE

/*
 * ICU Initialization Function. Need not be called.
 */
U_CAPI शून्य U_EXPORT2
u_init(UError_संहिता *स्थितिः) {
    UTRACE_प्रविष्टिः_OC(UTRACE_U_INIT);
    umtx_initOnce(gICUInitOnce, &init_दत्त, *स्थितिः);
    UTRACE_EXIT_स्थितिः(*स्थितिः);
}

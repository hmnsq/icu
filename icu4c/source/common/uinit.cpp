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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/icuplug.h"
#ಅನ್ತರ್ಭಾವ "unicode/uclean.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "icuplugimp.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "ucnv_io.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"

U_NAMESPACE_BEGIN

ಸ್ಥಿರ UInitOnce gICUInitOnce {};

ಸ್ಥಿರ UBool U_CALLCONV uinit_cleanup() {
    gICUInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
init_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2)
{
#ಆದರೆ UCONFIG_ENABLE_PLUGINS
    /* initialize plugins */
    uplug_init(&ಸ್ಥಿತಿ_2);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !UCONFIG_NO_CONVERSION
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
    ucnv_io_ಎಣಿಕೆ_ತಿಳಿದ_Converters(&ಸ್ಥಿತಿ_2);
#ಆದರೆ_ಅಂತ್ಯ
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_UINIT, uinit_cleanup);
}

U_NAMESPACE_ಕೊನೆ

U_NAMESPACE_USE

/*
 * ICU Initialization Function. Need not be called.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
u_init(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UTRACE_ನಮೂದು_OC(UTRACE_U_INIT);
    umtx_initOnce(gICUInitOnce, &init_ದತ್ತಾಂಶ, *ಸ್ಥಿತಿ_2);
    UTRACE_EXIT_ಸ್ಥಿತಿ(*ಸ್ಥಿತಿ_2);
}

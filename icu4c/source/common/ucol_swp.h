// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2003-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucol_swp.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2003sep10
*   created by: Markus W. Scherer
*
*   Swap collation binaries.
*/

#که_نه_تعریف_شوی __UCOL_SWP_H__
#تعریف_کړه __UCOL_SWP_H__

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_COLLATION

#ضمن "udataswp.h"

/*
 * Does the data look like a collation binary?
 * @internal
 */
U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_دوګونی(ثابت UDataSwapper *ds,
                              ثابت فراغ *دننه_معلومات, int32_t اوږدتیا);

/**
 * Swap ICU collation data like ucadata.icu. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_بدلون(ثابت UDataSwapper *ds,
          ثابت فراغ *دننه_معلومات, int32_t اوږدتیا, فراغ *بهر_معلومات,
          UError_کوډ *p_Error_کوډ);

/**
 * Swap inverse UCA collation data (invuca.icu). See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_بدلون_Inverse_UCA(ثابت UDataSwapper *ds,
                    ثابت فراغ *دننه_معلومات, int32_t اوږدتیا, فراغ *بهر_معلومات,
                    UError_کوډ *p_Error_کوډ);

#د_که_پای /* #if !UCONFIG_NO_COLLATION */

#د_که_پای

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

#اگر_غیر_تعریف_شدہ __UCOL_SWP_H__
#تعریف_کرو __UCOL_SWP_H__

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_COLLATION

#شامل "udataswp.h"

/*
 * Does the data look like a collation binary?
 * @internal
 */
U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ثنائی(ثابت UDataSwapper *ds,
                              ثابت خالی *اندر_ڈیٹا, int32_t لمبائی);

/**
 * Swap ICU collation data like ucadata.icu. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_تبادلہ(ثابت UDataSwapper *ds,
          ثابت خالی *اندر_ڈیٹا, int32_t لمبائی, خالی *برآمد_ڈیٹا,
          UError_رمز *p_Error_رمز);

/**
 * Swap inverse UCA collation data (invuca.icu). See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_تبادلہ_Inverse_UCA(ثابت UDataSwapper *ds,
                    ثابت خالی *اندر_ڈیٹا, int32_t لمبائی, خالی *برآمد_ڈیٹا,
                    UError_رمز *p_Error_رمز);

#اگر_ختم /* #if !UCONFIG_NO_COLLATION */

#اگر_ختم

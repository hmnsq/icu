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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UCOL_SWP_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UCOL_SWP_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_COLLATION

#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"

/*
 * Does the data look like a collation binary?
 * @internal
 */
U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ਦੁਆਧਾਰੀ(ਅਚਲ UDataSwapper *ds,
                              ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ);

/**
 * Swap ICU collation data like ucadata.icu. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ UDataSwapper *ds,
          ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
          UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Swap inverse UCA collation data (invuca.icu). See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_ਅਦਲਾ_ਬਦਲੀ_Inverse_UCA(ਅਚਲ UDataSwapper *ds,
                    ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t ਲੰਮਾਈ, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                    UError_ਕੋਡ *p_Error_ਕੋਡ);

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_COLLATION */

#ਜੇ_ਅੰਤ

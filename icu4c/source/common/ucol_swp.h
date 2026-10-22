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

#ifndef __UCOL_SWP_H__
#define __UCOL_SWP_H__

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_COLLATION

#অন্তর্ভাব "udataswp.h"

/*
 * Does the data look like a collation binary?
 * @internal
 */
U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_দ্বিমিক(ধ্রুবক UDataSwapper *ds,
                              ধ্রুবক শূন্য *in_তথ্য, int32_t দৈর্ঘ্য);

/**
 * Swap ICU collation data like ucadata.icu. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_swap(ধ্রুবক UDataSwapper *ds,
          ধ্রুবক শূন্য *in_তথ্য, int32_t দৈর্ঘ্য, শূন্য *out_তথ্য,
          UErrorCode *pErrorCode);

/**
 * Swap inverse UCA collation data (invuca.icu). See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_swapInverseUCA(ধ্রুবক UDataSwapper *ds,
                    ধ্রুবক শূন্য *in_তথ্য, int32_t দৈর্ঘ্য, শূন্য *out_তথ্য,
                    UErrorCode *pErrorCode);

#endif /* #if !UCONFIG_NO_COLLATION */

#endif

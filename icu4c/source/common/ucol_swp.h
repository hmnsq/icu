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
ucol_looks_Like_Collation_দ্বিমিক(ধ্রুব UDataSwapper *ds,
                              ধ্রুব শূন্য *inData, int32_t length);

/**
 * Swap ICU collation data like ucadata.icu. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_swap(ধ্রুব UDataSwapper *ds,
          ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
          UErrorCode *pErrorCode);

/**
 * Swap inverse UCA collation data (invuca.icu). See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_swapInverseUCA(ধ্রুব UDataSwapper *ds,
                    ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
                    UErrorCode *pErrorCode);

#endif /* #if !UCONFIG_NO_COLLATION */

#endif

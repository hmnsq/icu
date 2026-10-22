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

#अगर_अघोषित __UCOL_SWP_H__
#घोषणा __UCOL_SWP_H__

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_COLLATION

#समावेश "udataswp.h"

/*
 * Does the data look like a collation binary?
 * @internal
 */
U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_द्विआधारी(स्थिर UDataSwapper *ds,
                              स्थिर व्योम *in_तथ्य, int32_t लंबाई);

/**
 * Swap ICU collation data like ucadata.icu. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_swap(स्थिर UDataSwapper *ds,
          स्थिर व्योम *in_तथ्य, int32_t लंबाई, व्योम *out_तथ्य,
          UErrorCode *pErrorCode);

/**
 * Swap inverse UCA collation data (invuca.icu). See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_swapInverseUCA(स्थिर UDataSwapper *ds,
                    स्थिर व्योम *in_तथ्य, int32_t लंबाई, व्योम *out_तथ्य,
                    UErrorCode *pErrorCode);

#पूर्ण_अगर /* #if !UCONFIG_NO_COLLATION */

#पूर्ण_अगर

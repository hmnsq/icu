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

#यदि_अपरिभाषितम् __UCOL_SWP_H__
#परिभाषय __UCOL_SWP_H__

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_COLLATION

#अन्तर्भाव "udataswp.h"

/*
 * Does the data look like a collation binary?
 * @internal
 */
U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_द्विमान(ध्रुव UDataSwapper *ds,
                              ध्रुव शून्य *अन्तर्_दत्त, int32_t दीर्घता);

/**
 * Swap ICU collation data like ucadata.icu. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_swap(ध्रुव UDataSwapper *ds,
          ध्रुव शून्य *अन्तर्_दत्त, int32_t दीर्घता, शून्य *बहिर्गमनम्_दत्त,
          UError_संहिता *p_Error_संहिता);

/**
 * Swap inverse UCA collation data (invuca.icu). See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_swapInverseUCA(ध्रुव UDataSwapper *ds,
                    ध्रुव शून्य *अन्तर्_दत्त, int32_t दीर्घता, शून्य *बहिर्गमनम्_दत्त,
                    UError_संहिता *p_Error_संहिता);

#यदि_अन्तः /* #if !UCONFIG_NO_COLLATION */

#यदि_अन्तः

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

#నిర్వచితం_కాకపోతే __UCOL_SWP_H__
#నిర్వచించు __UCOL_SWP_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_COLLATION

#అన్తర్భావ "udataswp.h"

/*
 * Does the data look like a collation binary?
 * @internal
 */
U_CAPI UBool U_EXPORT2
ucol_looks_Like_Collation_ద్విమాన(స్థిరరాశి UDataSwapper *ds,
                              స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t నిడివి);

/**
 * Swap ICU collation data like ucadata.icu. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
          స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t నిడివి, శూన్య *బహిర్గతం_దత్తాంశం,
          UError_సంకేతం *p_Error_సంకేతం);

/**
 * Swap inverse UCA collation data (invuca.icu). See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
ucol_మార్పిడి_Inverse_UCA(స్థిరరాశి UDataSwapper *ds,
                    స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t నిడివి, శూన్య *బహిర్గతం_దత్తాంశం,
                    UError_సంకేతం *p_Error_సంకేతం);

#అయితే_ముగింపు /* #if !UCONFIG_NO_COLLATION */

#అయితే_ముగింపు

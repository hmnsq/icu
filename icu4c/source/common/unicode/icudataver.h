// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2009-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*/


/**
 * \file
 * \brief C API: access to ICU Data Version number
 */

#నిర్వచితం_కాకపోతే __ICU_DATA_VER_H__
#నిర్వచించు __ICU_DATA_VER_H__

#అన్తర్భావ "unicode/utypes.h"

/**
 * @stable ICU 49
 */
#నిర్వచించు U_ICU_సంస్కరణ_BUNDLE "icuver"

/**
 * @stable ICU 49
 */
#నిర్వచించు U_ICU_దత్తాంశం_కీలకం "DataVersion"

/**
 * Retrieves the data version from icuver and stores it in dataVersionFillin.
 * 
 * @param dataVersionFillin icuver data version information to be filled in if not-null
 * @param status stores the error code from the calls to resource bundle
 * 
 * @stable ICU 49
 */
U_CAPI శూన్య U_EXPORT2 u_get_దత్తాంశం_సంస్కరణ(UVersionInfo దత్తాంశం_సంస్కరణ_Fillin, UError_సంకేతం *స్థితి_2);

#అయితే_ముగింపు

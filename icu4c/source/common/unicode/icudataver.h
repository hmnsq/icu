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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ICU_DATA_VER_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __ICU_DATA_VER_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

/**
 * @stable ICU 49
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_ICU_ਸੰਸਕਰਣ_BUNDLE "icuver"

/**
 * @stable ICU 49
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_ICU_ਅੰਕੜੇ_ਕੁੰਜੀ "DataVersion"

/**
 * Retrieves the data version from icuver and stores it in dataVersionFillin.
 * 
 * @param dataVersionFillin icuver data version information to be filled in if not-null
 * @param status stores the error code from the calls to resource bundle
 * 
 * @stable ICU 49
 */
U_CAPI ਖਾਲੀ U_EXPORT2 u_get_ਅੰਕੜੇ_ਸੰਸਕਰਣ(UVersionInfo ਅੰਕੜੇ_ਸੰਸਕਰਣ_Fillin, UError_ਕੋਡ *ਸਥਿਤੀ);

#ਜੇ_ਅੰਤ

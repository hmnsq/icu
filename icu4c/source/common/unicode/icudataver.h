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

#ifndef __ICU_DATA_VER_H__
#define __ICU_DATA_VER_H__

#അന്തര്ഭാവ "unicode/utypes.h"

/**
 * @stable ICU 49
 */
#define U_ICU_VERSION_BUNDLE "icuver"

/**
 * @stable ICU 49
 */
#define U_ICU_DATA_താക്കോൽ "DataVersion"

/**
 * Retrieves the data version from icuver and stores it in dataVersionFillin.
 * 
 * @param dataVersionFillin icuver data version information to be filled in if not-null
 * @param status stores the error code from the calls to resource bundle
 * 
 * @stable ICU 49
 */
U_CAPI ശൂന്യം U_EXPORT2 u_getDataVersion(UVersionInfo dataVersionFillin, UErrorCode *status);

#endif

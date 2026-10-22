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

#جيڪڏهن_وصف_نه_ٿيل __ICU_DATA_VER_H__
#وصف_ڪريو __ICU_DATA_VER_H__

#ضمن "unicode/utypes.h"

/**
 * @stable ICU 49
 */
#وصف_ڪريو U_ICU_نسخو_BUNDLE "icuver"

/**
 * @stable ICU 49
 */
#وصف_ڪريو U_ICU_معلومات_ڪنجي "DataVersion"

/**
 * Retrieves the data version from icuver and stores it in dataVersionFillin.
 * 
 * @param dataVersionFillin icuver data version information to be filled in if not-null
 * @param status stores the error code from the calls to resource bundle
 * 
 * @stable ICU 49
 */
U_CAPI فراغ U_EXPORT2 u_get_معلومات_نسخو(UVersionInfo معلومات_نسخو_Fillin, UError_ڪوڊ *حالت);

#جيڪڏهن_پورو

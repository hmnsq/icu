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

#यदि_अपरिभाषितम् __ICU_DATA_VER_H__
#परिभाषय __ICU_DATA_VER_H__

#अन्तर्भाव "unicode/utypes.h"

/**
 * @stable ICU 49
 */
#परिभाषय U_ICU_संस्करणम्_BUNDLE "icuver"

/**
 * @stable ICU 49
 */
#परिभाषय U_ICU_दत्त_कुञ्चिका "DataVersion"

/**
 * Retrieves the data version from icuver and stores it in dataVersionFillin.
 * 
 * @param dataVersionFillin icuver data version information to be filled in if not-null
 * @param status stores the error code from the calls to resource bundle
 * 
 * @stable ICU 49
 */
U_CAPI शून्य U_EXPORT2 u_get_दत्त_संस्करणम्(UVersionInfo दत्त_संस्करणम्_Fillin, UError_संहिता *स्थितिः);

#यदि_अन्तः

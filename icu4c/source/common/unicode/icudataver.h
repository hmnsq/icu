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

#جے_بنا_تعریف __ICU_DATA_VER_H__
#تعریف_کرو __ICU_DATA_VER_H__

#ضمن "unicode/utypes.h"

/**
 * @stable ICU 49
 */
#تعریف_کرو U_ICU_نسخہ_BUNDLE "icuver"

/**
 * @stable ICU 49
 */
#تعریف_کرو U_ICU_انکڑے_کلید "DataVersion"

/**
 * Retrieves the data version from icuver and stores it in dataVersionFillin.
 * 
 * @param dataVersionFillin icuver data version information to be filled in if not-null
 * @param status stores the error code from the calls to resource bundle
 * 
 * @stable ICU 49
 */
U_CAPI فراغ U_EXPORT2 u_get_انکڑے_نسخہ(UVersionInfo انکڑے_نسخہ_Fillin, UError_کوڈ *حالت);

#جے_انت

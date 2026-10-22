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

#अगर_अघोषित __ICU_DATA_VER_H__
#घोषणा __ICU_DATA_VER_H__

#समावेश "unicode/utypes.h"

/**
 * @stable ICU 49
 */
#घोषणा U_ICU_VERSION_BUNDLE "icuver"

/**
 * @stable ICU 49
 */
#घोषणा U_ICU_तथ्य_कुंजी "DataVersion"

/**
 * Retrieves the data version from icuver and stores it in dataVersionFillin.
 * 
 * @param dataVersionFillin icuver data version information to be filled in if not-null
 * @param status stores the error code from the calls to resource bundle
 * 
 * @stable ICU 49
 */
U_CAPI व्योम U_EXPORT2 u_get_तथ्य_Version(UVersionInfo तथ्य_Version_Fillin, UErrorCode *status);

#पूर्ण_अगर

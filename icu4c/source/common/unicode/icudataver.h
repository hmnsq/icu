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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ICU_DATA_VER_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __ICU_DATA_VER_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

/**
 * @stable ICU 49
 */
#ವ್ಯಾಖ್ಯಾನಿಸು U_ICU_ಆವೃತ್ತಿ_BUNDLE "icuver"

/**
 * @stable ICU 49
 */
#ವ್ಯಾಖ್ಯಾನಿಸು U_ICU_ದತ್ತಾಂಶ_ಕೀಲಿ "DataVersion"

/**
 * Retrieves the data version from icuver and stores it in dataVersionFillin.
 * 
 * @param dataVersionFillin icuver data version information to be filled in if not-null
 * @param status stores the error code from the calls to resource bundle
 * 
 * @stable ICU 49
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2 u_get_ದತ್ತಾಂಶ_ಆವೃತ್ತಿ(UVersionInfo ದತ್ತಾಂಶ_ಆವೃತ್ತಿ_Fillin, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

#ಆದರೆ_ಅಂತ್ಯ

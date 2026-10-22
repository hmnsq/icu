// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1996-2013, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File locmap.h      : Locale Mapping Classes
* 
*
* Created by: Helena Shih
*
* Modification History:
*
*  Date        Name        Description
*  3/11/97     aliu        Added setId().
*  4/20/99     Madhu       Added T_convertToPosix()
* 09/18/00     george      Removed the memory leaks.
* 08/23/01     george      Convert to C
*============================================================================
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LOCMAP_H
#ವ್ಯಾಖ್ಯಾನಿಸು LOCMAP_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ವ್ಯಾಖ್ಯಾನಿಸು LANGUAGE_LCID(hostID) (uint16_t)(0x03FF & hostID)

U_CAPI int32_t uprv_convert_ಗಮ್ಯ_Posix(uint32_t hostid, ಅಕ್ಷರ* posixID, int32_t posixIDCapacity, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

/* Don't call these functions directly. Use uloc_getLCID instead. */
U_CAPI uint32_t uprv_convert_ಗಮ್ಯ_LCIDPlatform(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* localeID, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2); // Leverage platform conversion if possible
U_CAPI uint32_t uprv_convert_ಗಮ್ಯ_LCID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* langID, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* posixID, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

#ಆದರೆ_ಅಂತ್ಯ /* LOCMAP_H */


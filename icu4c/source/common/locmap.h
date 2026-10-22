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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LOCMAP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOCMAP_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LANGUAGE_LCID(hostID) (uint16_t)(0x03FF & hostID)

U_CAPI int32_t uprv_convert_ਮੰਜ਼ਿਲ_Posix(uint32_t hostid, ਅੱਖਰ* posixID, int32_t posixIDCapacity, UError_ਕੋਡ* ਸਥਿਤੀ);

/* Don't call these functions directly. Use uloc_getLCID instead. */
U_CAPI uint32_t uprv_convert_ਮੰਜ਼ਿਲ_LCIDPlatform(ਅਚਲ ਅੱਖਰ* localeID, UError_ਕੋਡ* ਸਥਿਤੀ); // Leverage platform conversion if possible
U_CAPI uint32_t uprv_convert_ਮੰਜ਼ਿਲ_LCID(ਅਚਲ ਅੱਖਰ* langID, ਅਚਲ ਅੱਖਰ* posixID, UError_ਕੋਡ* ਸਥਿਤੀ);

#ਜੇ_ਅੰਤ /* LOCMAP_H */


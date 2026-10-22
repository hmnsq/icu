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

#اگر_غیر_تعریف_شدہ LOCMAP_H
#تعریف_کرو LOCMAP_H

#شامل "unicode/utypes.h"

#تعریف_کرو LANGUAGE_LCID(hostID) (uint16_t)(0x03FF & hostID)

U_CAPI int32_t uprv_convert_مقصد_Posix(uint32_t hostid, حرف* posixID, int32_t posixIDCapacity, UError_رمز* حالت_2);

/* Don't call these functions directly. Use uloc_getLCID instead. */
U_CAPI uint32_t uprv_convert_مقصد_LCIDPlatform(ثابت حرف* localeID, UError_رمز* حالت_2); // Leverage platform conversion if possible
U_CAPI uint32_t uprv_convert_مقصد_LCID(ثابت حرف* langID, ثابت حرف* posixID, UError_رمز* حالت_2);

#اگر_ختم /* LOCMAP_H */


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

#جے_بنا_تعریف LOCMAP_H
#تعریف_کرو LOCMAP_H

#ضمن "unicode/utypes.h"

#تعریف_کرو LANGUAGE_LCID(hostID) (uint16_t)(0x03FF & hostID)

U_CAPI int32_t uprv_convert_منزل_Posix(uint32_t hostid, حرف* posixID, int32_t posixIDCapacity, UError_کوڈ* حالت);

/* Don't call these functions directly. Use uloc_getLCID instead. */
U_CAPI uint32_t uprv_convert_منزل_LCIDPlatform(ثابت حرف* localeID, UError_کوڈ* حالت); // Leverage platform conversion if possible
U_CAPI uint32_t uprv_convert_منزل_LCID(ثابت حرف* langID, ثابت حرف* posixID, UError_کوڈ* حالت);

#جے_انت /* LOCMAP_H */


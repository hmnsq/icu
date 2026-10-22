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

#अगर_अघोषित LOCMAP_H
#घोषणा LOCMAP_H

#समावेश "unicode/utypes.h"

#घोषणा LANGUAGE_LCID(hostID) (uint16_t)(0x03FF & hostID)

U_CAPI int32_t uprv_convertToPosix(uint32_t hostid, अक्षर* posixID, int32_t posixIDCapacity, UErrorCode* status);

/* Don't call these functions directly. Use uloc_getLCID instead. */
U_CAPI uint32_t uprv_convertToLCIDPlatform(स्थिर अक्षर* localeID, UErrorCode* status); // Leverage platform conversion if possible
U_CAPI uint32_t uprv_convertToLCID(स्थिर अक्षर* langID, स्थिर अक्षर* posixID, UErrorCode* status);

#पूर्ण_अगर /* LOCMAP_H */


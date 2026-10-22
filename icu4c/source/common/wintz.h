// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
********************************************************************************
*   Copyright (C) 2005-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
********************************************************************************
*
* File WINTZ.H
*
********************************************************************************
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __WINTZ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __WINTZ

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਸਮਾਂ_ZONE_INFORMATION ਸਮਾਂ_ZONE_INFORMATION_2;
U_CDECL_ਸਿਰਾ

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uprv_detect_Windows_ਸਮਾਂ_Zone();

#ਜੇ_ਅੰਤ /* U_PLATFORM_USES_ONLY_WIN32_API  */

#ਜੇ_ਅੰਤ /* __WINTZ */

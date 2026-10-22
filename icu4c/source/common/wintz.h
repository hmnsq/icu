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

#ifndef __WINTZ
#define __WINTZ

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি U_PLATFORM_USES_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
প্রকারনির্দেশ সংরচনা _TIME_ZONE_INFORMATION TIME_ZONE_INFORMATION;
U_CDECL_END

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
uprv_detectWindowsTimeZone();

#endif /* U_PLATFORM_USES_ONLY_WIN32_API  */

#endif /* __WINTZ */

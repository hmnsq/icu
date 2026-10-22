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

#यदि_अपरिभाषितम् __WINTZ
#परिभाषय __WINTZ

#अन्तर्भाव "unicode/utypes.h"

#यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
प्रकारनिर्देश संरचना _TIME_ZONE_INFORMATION TIME_ZONE_INFORMATION;
U_CDECL_अन्तः

U_CAPI ध्रुव वर्ण* U_EXPORT2
uprv_detectWindowsTimeZone();

#यदि_अन्तः /* U_PLATFORM_USES_ONLY_WIN32_API  */

#यदि_अन्तः /* __WINTZ */

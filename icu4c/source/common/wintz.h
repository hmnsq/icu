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

#اگر_غیر_تعریف_شدہ __WINTZ
#تعریف_کرو __WINTZ

#شامل "unicode/utypes.h"

#اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
قسمتعریف ساخت وقت_ZONE_INFORMATION وقت_ZONE_INFORMATION_2;
U_CDECL_انتہا

U_CAPI ثابت حرف* U_EXPORT2
uprv_detect_Windows_وقت_Zone();

#اگر_ختم /* U_PLATFORM_USES_ONLY_WIN32_API  */

#اگر_ختم /* __WINTZ */

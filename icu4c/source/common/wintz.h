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

#که_نه_تعریف_شوی __WINTZ
#تعریف_کړه __WINTZ

#ضمن "unicode/utypes.h"

#إذا U_PLATFORM_کاروي_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
تعريفنوع بنية وخت_ZONE_INFORMATION وخت_ZONE_INFORMATION_2;
U_CDECL_پای_ټکی

U_CAPI ثابت حرف* U_EXPORT2
uprv_detect_Windows_وخت_Zone();

#د_که_پای /* U_PLATFORM_USES_ONLY_WIN32_API  */

#د_که_پای /* __WINTZ */

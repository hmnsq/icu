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

#جيڪڏهن_وصف_نه_ٿيل __WINTZ
#وصف_ڪريو __WINTZ

#ضمن "unicode/utypes.h"

#إذا U_PLATFORM_استعمال_ڪري_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
تعريفنوع بنية وقت_ZONE_INFORMATION وقت_ZONE_INFORMATION_2;
U_CDECL_پڇاڙي

U_CAPI ثابت حرف* U_EXPORT2
uprv_detect_Windows_وقت_Zone();

#جيڪڏهن_پورو /* U_PLATFORM_USES_ONLY_WIN32_API  */

#جيڪڏهن_پورو /* __WINTZ */

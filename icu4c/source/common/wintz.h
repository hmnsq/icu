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

#اگر_تعریف_نشده __WINTZ
#تعریف_کن __WINTZ

#ضمن "unicode/utypes.h"

#إذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
تعريفنوع بنية زمان_ZONE_INFORMATION زمان_ZONE_INFORMATION_2;
U_CDECL_انتها

U_CAPI ثابت حرف* U_EXPORT2
uprv_detect_Windows_زمان_Zone();

#پایان_اگر /* U_PLATFORM_USES_ONLY_WIN32_API  */

#پایان_اگر /* __WINTZ */

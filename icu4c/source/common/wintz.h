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

#جے_بنا_تعریف __WINTZ
#تعریف_کرو __WINTZ

#ضمن "unicode/utypes.h"

#إذا U_PLATFORM_ورتدا_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
تعريفنوع بنية ویلا_ZONE_INFORMATION ویلا_ZONE_INFORMATION_2;
U_CDECL_سرا

U_CAPI ثابت حرف* U_EXPORT2
uprv_detect_Windows_ویلا_Zone();

#جے_انت /* U_PLATFORM_USES_ONLY_WIN32_API  */

#جے_انت /* __WINTZ */

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

#अगर_अघोषित __WINTZ
#घोषणा __WINTZ

#समावेश "unicode/utypes.h"

#अगर U_PLATFORM_USES_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
प्रकार काष्ठा _TIME_ZONE_INFORMATION TIME_ZONE_INFORMATION;
U_CDECL_END

U_CAPI स्थिर अक्षर* U_EXPORT2
uprv_detectWindowsTimeZone();

#पूर्ण_अगर /* U_PLATFORM_USES_ONLY_WIN32_API  */

#पूर्ण_अगर /* __WINTZ */

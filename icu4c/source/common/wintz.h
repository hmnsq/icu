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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __WINTZ
#ವ್ಯಾಖ್ಯಾನಿಸು __WINTZ

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಸಮಯ_ZONE_INFORMATION ಸಮಯ_ZONE_INFORMATION_2;
U_CDECL_ಕೊನೆ

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uprv_detect_Windows_ಸಮಯ_Zone();

#ಆದರೆ_ಅಂತ್ಯ /* U_PLATFORM_USES_ONLY_WIN32_API  */

#ಆದರೆ_ಅಂತ್ಯ /* __WINTZ */

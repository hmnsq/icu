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

#నిర్వచితం_కాకపోతే __WINTZ
#నిర్వచించు __WINTZ

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API

/**
 * \file 
 * \brief C API: Utilities for dealing w/ Windows time zones.
 */

U_CDECL_BEGIN
/* Forward declarations for Windows types... */
రకనిర్వచనం నిర్మాణం సమయం_ZONE_INFORMATION సమయం_ZONE_INFORMATION_2;
U_CDECL_చివర

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uprv_detect_Windows_సమయం_Zone();

#అయితే_ముగింపు /* U_PLATFORM_USES_ONLY_WIN32_API  */

#అయితే_ముగింపు /* __WINTZ */

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 2006 International Business Machines Corporation        *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#నిర్వచితం_కాకపోతే LOCALSVC_H
#నిర్వచించు LOCALSVC_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ defined(U_LOCAL_SERVICE_HOOK) && U_LOCAL_SERVICE_HOOK
/**
 * Prototype for user-supplied service hook. This function is expected to return
 * a type of factory object specific to the requested service.
 * 
 * @param what service-specific string identifying the specific user hook
 * @param status error status
 * @return a service-specific hook, or NULL on failure.
 */
U_CAPI శూన్య* uprv_svc_hook(స్థిరరాశి అక్షరం *what, UError_సంకేతం *స్థితి_2);
#అయితే_ముగింపు

#అయితే_ముగింపు

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 2006 International Business Machines Corporation        *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#جے_بنا_تعریف LOCALSVC_H
#تعریف_کرو LOCALSVC_H

#ضمن "unicode/utypes.h"

#إذا defined(U_LOCAL_SERVICE_HOOK) && U_LOCAL_SERVICE_HOOK
/**
 * Prototype for user-supplied service hook. This function is expected to return
 * a type of factory object specific to the requested service.
 * 
 * @param what service-specific string identifying the specific user hook
 * @param status error status
 * @return a service-specific hook, or NULL on failure.
 */
U_CAPI فراغ* uprv_svc_hook(ثابت حرف *what, UError_کوڈ *حالت);
#جے_انت

#جے_انت

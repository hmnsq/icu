// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 2006 International Business Machines Corporation        *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LOCALSVC_H
#ವ್ಯಾಖ್ಯಾನಿಸು LOCALSVC_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ defined(U_LOCAL_SERVICE_HOOK) && U_LOCAL_SERVICE_HOOK
/**
 * Prototype for user-supplied service hook. This function is expected to return
 * a type of factory object specific to the requested service.
 * 
 * @param what service-specific string identifying the specific user hook
 * @param status error status
 * @return a service-specific hook, or NULL on failure.
 */
U_CAPI ಶೂನ್ಯ* uprv_svc_hook(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *what, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ

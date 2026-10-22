// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
***************************************************************************
*   Copyright (C) 2006 International Business Machines Corporation        *
*   and others. All rights reserved.                                      *
***************************************************************************
*/

#अगर_अघोषित LOCALSVC_H
#घोषणा LOCALSVC_H

#समावेश "unicode/utypes.h"

#अगर defined(U_LOCAL_SERVICE_HOOK) && U_LOCAL_SERVICE_HOOK
/**
 * Prototype for user-supplied service hook. This function is expected to return
 * a type of factory object specific to the requested service.
 * 
 * @param what service-specific string identifying the specific user hook
 * @param status error status
 * @return a service-specific hook, or NULL on failure.
 */
U_CAPI व्योम* uprv_svc_hook(स्थिर अक्षर *what, UErrorCode *status);
#पूर्ण_अगर

#पूर्ण_अगर

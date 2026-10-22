// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File uassert.h
*
*  Contains the U_ASSERT and UPRV_UNREACHABLE_* macros
*
******************************************************************************
*/
#ifndef U_ASSERT_H
#define U_ASSERT_H

/* utypes.h is included to get the proper define for uint8_t */
#அந்தர்பாவ "unicode/utypes.h"
/* for abort */
#அந்தர்பாவ <stdlib.h>

/**
 * \def U_ASSERT
 * By default, U_ASSERT just wraps the C library assert macro.
 * By changing the definition here, the assert behavior for ICU can be changed
 * without affecting other non - ICU uses of the C library assert().
*/
#எனில் U_DEBUG
#   include <assert.h>
#   define U_ASSERT(exp) ப்ரதிஜ்ஞா(exp)
#அல்லதெனில் U_CPLUSPLUS_VERSION
#   define U_ASSERT(exp) (வெறுமை)0
#இல்லையெனில்
#   define U_ASSERT(exp)
#endif

/**
 * \def UPRV_UNREACHABLE_ASSERT
 * This macro is used in places that we had believed were unreachable, but
 * experience has shown otherwise (possibly due to memory corruption, etc).
 * In this case we call assert() in debug versions as with U_ASSERT, instead
 * of unconditionally calling abort(). However we also allow redefinition as
 * with UPRV_UNREACHABLE_EXIT.
 * @internal
*/
#எனில் defined(UPRV_UNREACHABLE_ASSERT)
    // Use the predefined value.
#அல்லதெனில் U_DEBUG
#   include <assert.h>
#   define UPRV_UNREACHABLE_ASSERT ப்ரதிஜ்ஞா(false)
#அல்லதெனில் U_CPLUSPLUS_VERSION
#   define UPRV_UNREACHABLE_ASSERT (வெறுமை)0
#இல்லையெனில்
#   define UPRV_UNREACHABLE_ASSERT
#endif

/**
 * \def UPRV_UNREACHABLE_EXIT
 * This macro is used to unconditionally abort if unreachable code is ever executed.
 * @internal
*/
#எனில் defined(UPRV_UNREACHABLE_EXIT)
    // Use the predefined value.
#இல்லையெனில்
#   define UPRV_UNREACHABLE_EXIT abort()
#endif

#endif

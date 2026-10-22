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
#অন্তর্ভাৱ "unicode/utypes.h"
/* for abort */
#অন্তর্ভাৱ <stdlib.h>

/**
 * \def U_ASSERT
 * By default, U_ASSERT just wraps the C library assert macro.
 * By changing the definition here, the assert behavior for ICU can be changed
 * without affecting other non - ICU uses of the C library assert().
*/
#যদি U_DEBUG
#   include <assert.h>
#   define U_ASSERT(exp) প্রতিজ্ঞা(exp)
#নোচেদ্যদি U_CPLUSPLUS_VERSION
#   define U_ASSERT(exp) (শূন্য)0
#অন্যথা
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
#যদি defined(UPRV_UNREACHABLE_ASSERT)
    // Use the predefined value.
#নোচেদ্যদি U_DEBUG
#   include <assert.h>
#   define UPRV_UNREACHABLE_ASSERT প্রতিজ্ঞা(false)
#নোচেদ্যদি U_CPLUSPLUS_VERSION
#   define UPRV_UNREACHABLE_ASSERT (শূন্য)0
#অন্যথা
#   define UPRV_UNREACHABLE_ASSERT
#endif

/**
 * \def UPRV_UNREACHABLE_EXIT
 * This macro is used to unconditionally abort if unreachable code is ever executed.
 * @internal
*/
#যদি defined(UPRV_UNREACHABLE_EXIT)
    // Use the predefined value.
#অন্যথা
#   define UPRV_UNREACHABLE_EXIT abort()
#endif

#endif

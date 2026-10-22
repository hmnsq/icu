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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_ASSERT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_ASSERT_H

/* utypes.h is included to get the proper define for uint8_t */
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
/* for abort */
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

/**
 * \def U_ASSERT
 * By default, U_ASSERT just wraps the C library assert macro.
 * By changing the definition here, the assert behavior for ICU can be changed
 * without affecting other non - ICU uses of the C library assert().
*/
#ਜੇ U_DEBUG
#   include <assert.h>
#   define U_ASSERT(ਘਾਤਾਂਕ) ਪ੍ਰਤਿਜ੍ਞਾ(ਘਾਤਾਂਕ)
#ਜਾਂਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ
#   define U_ASSERT(ਘਾਤਾਂਕ) (ਖਾਲੀ)0
#ਨਹੀਂਤਾਂ
#   define U_ASSERT(ਘਾਤਾਂਕ)
#ਜੇ_ਅੰਤ

/**
 * \def UPRV_UNREACHABLE_ASSERT
 * This macro is used in places that we had believed were unreachable, but
 * experience has shown otherwise (possibly due to memory corruption, etc).
 * In this case we call assert() in debug versions as with U_ASSERT, instead
 * of unconditionally calling abort(). However we also allow redefinition as
 * with UPRV_UNREACHABLE_EXIT.
 * @internal
*/
#ਜੇ defined(UPRV_UNREACHABLE_ASSERT)
    // Use the predefined value.
#ਜਾਂਜੇ U_DEBUG
#   include <assert.h>
#   define UPRV_UNREACHABLE_ASSERT ਪ੍ਰਤਿਜ੍ਞਾ(false)
#ਜਾਂਜੇ U_CPLUSPLUS_ਸੰਸਕਰਣ
#   define UPRV_UNREACHABLE_ASSERT (ਖਾਲੀ)0
#ਨਹੀਂਤਾਂ
#   define UPRV_UNREACHABLE_ASSERT
#ਜੇ_ਅੰਤ

/**
 * \def UPRV_UNREACHABLE_EXIT
 * This macro is used to unconditionally abort if unreachable code is ever executed.
 * @internal
*/
#ਜੇ defined(UPRV_UNREACHABLE_EXIT)
    // Use the predefined value.
#ਨਹੀਂਤਾਂ
#   define UPRV_UNREACHABLE_EXIT abort()
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ

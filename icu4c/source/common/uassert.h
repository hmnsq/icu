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
#اگر_غیر_تعریف_شدہ U_ASSERT_H
#تعریف_کرو U_ASSERT_H

/* utypes.h is included to get the proper define for uint8_t */
#شامل "unicode/utypes.h"
/* for abort */
#شامل <stdlib.h>

/**
 * \def U_ASSERT
 * By default, U_ASSERT just wraps the C library assert macro.
 * By changing the definition here, the assert behavior for ICU can be changed
 * without affecting other non - ICU uses of the C library assert().
*/
#اگر U_DEBUG
#   include <assert.h>
#   define U_ASSERT(قوت_نما) أكد(قوت_نما)
#وگرنہ_اگر U_CPLUSPLUS_نسخہ
#   define U_ASSERT(قوت_نما) (خالی)0
#ورنہ
#   define U_ASSERT(قوت_نما)
#اگر_ختم

/**
 * \def UPRV_UNREACHABLE_ASSERT
 * This macro is used in places that we had believed were unreachable, but
 * experience has shown otherwise (possibly due to memory corruption, etc).
 * In this case we call assert() in debug versions as with U_ASSERT, instead
 * of unconditionally calling abort(). However we also allow redefinition as
 * with UPRV_UNREACHABLE_EXIT.
 * @internal
*/
#اگر defined(UPRV_UNREACHABLE_ASSERT)
    // Use the predefined value.
#وگرنہ_اگر U_DEBUG
#   include <assert.h>
#   define UPRV_UNREACHABLE_ASSERT أكد(false)
#وگرنہ_اگر U_CPLUSPLUS_نسخہ
#   define UPRV_UNREACHABLE_ASSERT (خالی)0
#ورنہ
#   define UPRV_UNREACHABLE_ASSERT
#اگر_ختم

/**
 * \def UPRV_UNREACHABLE_EXIT
 * This macro is used to unconditionally abort if unreachable code is ever executed.
 * @internal
*/
#اگر defined(UPRV_UNREACHABLE_EXIT)
    // Use the predefined value.
#ورنہ
#   define UPRV_UNREACHABLE_EXIT abort()
#اگر_ختم

#اگر_ختم

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
#నిర్వచితం_కాకపోతే U_ASSERT_H
#నిర్వచించు U_ASSERT_H

/* utypes.h is included to get the proper define for uint8_t */
#అన్తర్భావ "unicode/utypes.h"
/* for abort */
#అన్తర్భావ <stdlib.h>

/**
 * \def U_ASSERT
 * By default, U_ASSERT just wraps the C library assert macro.
 * By changing the definition here, the assert behavior for ICU can be changed
 * without affecting other non - ICU uses of the C library assert().
*/
#ఒకవేళ U_DEBUG
#   include <assert.h>
#   define U_ASSERT(ఘాతాంకం) ప్రతిజ్ఞా(ఘాతాంకం)
#లేదావేళ U_CPLUSPLUS_సంస్కరణ
#   define U_ASSERT(ఘాతాంకం) (శూన్య)0
#లేకపోతే
#   define U_ASSERT(ఘాతాంకం)
#అయితే_ముగింపు

/**
 * \def UPRV_UNREACHABLE_ASSERT
 * This macro is used in places that we had believed were unreachable, but
 * experience has shown otherwise (possibly due to memory corruption, etc).
 * In this case we call assert() in debug versions as with U_ASSERT, instead
 * of unconditionally calling abort(). However we also allow redefinition as
 * with UPRV_UNREACHABLE_EXIT.
 * @internal
*/
#ఒకవేళ defined(UPRV_UNREACHABLE_ASSERT)
    // Use the predefined value.
#లేదావేళ U_DEBUG
#   include <assert.h>
#   define UPRV_UNREACHABLE_ASSERT ప్రతిజ్ఞా(false)
#లేదావేళ U_CPLUSPLUS_సంస్కరణ
#   define UPRV_UNREACHABLE_ASSERT (శూన్య)0
#లేకపోతే
#   define UPRV_UNREACHABLE_ASSERT
#అయితే_ముగింపు

/**
 * \def UPRV_UNREACHABLE_EXIT
 * This macro is used to unconditionally abort if unreachable code is ever executed.
 * @internal
*/
#ఒకవేళ defined(UPRV_UNREACHABLE_EXIT)
    // Use the predefined value.
#లేకపోతే
#   define UPRV_UNREACHABLE_EXIT abort()
#అయితే_ముగింపు

#అయితే_ముగింపు

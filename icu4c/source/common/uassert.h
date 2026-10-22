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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ASSERT_H
#ವ್ಯಾಖ್ಯಾನಿಸು U_ASSERT_H

/* utypes.h is included to get the proper define for uint8_t */
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
/* for abort */
#ಅನ್ತರ್ಭಾವ <stdlib.h>

/**
 * \def U_ASSERT
 * By default, U_ASSERT just wraps the C library assert macro.
 * By changing the definition here, the assert behavior for ICU can be changed
 * without affecting other non - ICU uses of the C library assert().
*/
#ಆದರೆ U_DEBUG
#   include <assert.h>
#   define U_ASSERT(ಘಾತಾಂಕ) ಪ್ರತಿಜ್ಞಾ(ಘಾತಾಂಕ)
#ಅಥವಾಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ
#   define U_ASSERT(ಘಾತಾಂಕ) (ಶೂನ್ಯ)0
#ಇಲ್ಲದಿದ್ದರೆ
#   define U_ASSERT(ಘಾತಾಂಕ)
#ಆದರೆ_ಅಂತ್ಯ

/**
 * \def UPRV_UNREACHABLE_ASSERT
 * This macro is used in places that we had believed were unreachable, but
 * experience has shown otherwise (possibly due to memory corruption, etc).
 * In this case we call assert() in debug versions as with U_ASSERT, instead
 * of unconditionally calling abort(). However we also allow redefinition as
 * with UPRV_UNREACHABLE_EXIT.
 * @internal
*/
#ಆದರೆ defined(UPRV_UNREACHABLE_ASSERT)
    // Use the predefined value.
#ಅಥವಾಆದರೆ U_DEBUG
#   include <assert.h>
#   define UPRV_UNREACHABLE_ASSERT ಪ್ರತಿಜ್ಞಾ(false)
#ಅಥವಾಆದರೆ U_CPLUSPLUS_ಆವೃತ್ತಿ
#   define UPRV_UNREACHABLE_ASSERT (ಶೂನ್ಯ)0
#ಇಲ್ಲದಿದ್ದರೆ
#   define UPRV_UNREACHABLE_ASSERT
#ಆದರೆ_ಅಂತ್ಯ

/**
 * \def UPRV_UNREACHABLE_EXIT
 * This macro is used to unconditionally abort if unreachable code is ever executed.
 * @internal
*/
#ಆದರೆ defined(UPRV_UNREACHABLE_EXIT)
    // Use the predefined value.
#ಇಲ್ಲದಿದ್ದರೆ
#   define UPRV_UNREACHABLE_EXIT abort()
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ

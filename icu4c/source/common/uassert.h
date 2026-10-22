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
#अगर_अघोषित U_ASSERT_H
#घोषणा U_ASSERT_H

/* utypes.h is included to get the proper define for uint8_t */
#समावेश "unicode/utypes.h"
/* for abort */
#समावेश <stdlib.h>

/**
 * \def U_ASSERT
 * By default, U_ASSERT just wraps the C library assert macro.
 * By changing the definition here, the assert behavior for ICU can be changed
 * without affecting other non - ICU uses of the C library assert().
*/
#अगर U_DEBUG
#   include <assert.h>
#   define U_ASSERT(exp) निश्चित(exp)
#या_अगर U_CPLUSPLUS_VERSION
#   define U_ASSERT(exp) (व्योम)0
#अन्यथा
#   define U_ASSERT(exp)
#पूर्ण_अगर

/**
 * \def UPRV_UNREACHABLE_ASSERT
 * This macro is used in places that we had believed were unreachable, but
 * experience has shown otherwise (possibly due to memory corruption, etc).
 * In this case we call assert() in debug versions as with U_ASSERT, instead
 * of unconditionally calling abort(). However we also allow redefinition as
 * with UPRV_UNREACHABLE_EXIT.
 * @internal
*/
#अगर defined(UPRV_UNREACHABLE_ASSERT)
    // Use the predefined value.
#या_अगर U_DEBUG
#   include <assert.h>
#   define UPRV_UNREACHABLE_ASSERT निश्चित(false)
#या_अगर U_CPLUSPLUS_VERSION
#   define UPRV_UNREACHABLE_ASSERT (व्योम)0
#अन्यथा
#   define UPRV_UNREACHABLE_ASSERT
#पूर्ण_अगर

/**
 * \def UPRV_UNREACHABLE_EXIT
 * This macro is used to unconditionally abort if unreachable code is ever executed.
 * @internal
*/
#अगर defined(UPRV_UNREACHABLE_EXIT)
    // Use the predefined value.
#अन्यथा
#   define UPRV_UNREACHABLE_EXIT पात()
#पूर्ण_अगर

#पूर्ण_अगर

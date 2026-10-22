// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File: cstr.h
*/

#ifndef CSTR_H
#define CSTR_H

#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "unicode/utypes.h"

#અન્તર્ભાવ "charstr.h"

/**
 * ICU-internal class CStr, a small helper class to facilitate passing UnicodeStrings
 * to functions needing (const char *) strings, such as printf().
 *
 * It is intended primarily for use in debugging or in tests. Uses platform 
 * default code page conversion, which will do the best job possible,
 * but may be lossy, depending on the platform.
 *
 * If no other conversion is available, use invariant conversion and substitute
 * '?' for non-invariant characters.
 *
 * Example Usage:
 *   UnicodeString s = whatever;
 *   printf("%s", CStr(s)());
 *
 *   The explicit call to the CStr() constructor creates a temporary object.
 *   Operator () on the temporary object returns a (const char *) pointer.
 *   The lifetime of the (const char *) data is that of the temporary object,
 *   which works well when passing it as a parameter to another function, such as printf.
 */

U_NAMESPACE_BEGIN

class U_COMMON_API_CLASS CStr : જાહેર UMemory {
  જાહેર:
    U_COMMON_API CStr(અચલ UnicodeString &in);
    U_COMMON_API ~CStr();
    U_COMMON_API અચલ અક્ષર * operator ()() અચલ;

  ખાનગી:
    CharString s;
    CStr(અચલ CStr &other) = delete;               //  Forbid copying of this class.
    CStr &operator =(અચલ CStr &other) = delete;   //  Forbid assignment.
};

U_NAMESPACE_END

#endif

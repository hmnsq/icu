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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CSTR_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSTR_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"

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

class U_COMMON_API_CLASS CStr : ਜਨਤਕ UMemory {
  ਜਨਤਕ:
    U_COMMON_API CStr(ਅਚਲ UnicodeString &ਵਿੱਚ);
    U_COMMON_API ~CStr();
    U_COMMON_API ਅਚਲ ਅੱਖਰ * ਸੰਚਾਲਕ ()() ਅਚਲ;

  ਨਿਜੀ:
    CharString s;
    CStr(ਅਚਲ CStr &other) = delete;               //  Forbid copying of this class.
    CStr &ਸੰਚਾਲਕ =(ਅਚਲ CStr &other) = delete;   //  Forbid assignment.
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ

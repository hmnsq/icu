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

#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/utypes.h"

#অন্তর্ভাব "charstr.h"

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

class U_COMMON_API_CLASS CStr : সর্বজনীন UMemory {
  সর্বজনীন:
    U_COMMON_API CStr(ধ্রুবক UnicodeString &in);
    U_COMMON_API ~CStr();
    U_COMMON_API ধ্রুবক অক্ষর * operator ()() ধ্রুবক;

  ব্যক্তিগত:
    CharString s;
    CStr(ধ্রুবক CStr &other) = delete;               //  Forbid copying of this class.
    CStr &operator =(ধ্রুবক CStr &other) = delete;   //  Forbid assignment.
};

U_NAMESPACE_END

#endif

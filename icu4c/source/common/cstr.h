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

#اگر_تعریف_نشده CSTR_H
#تعریف_کن CSTR_H

#ضمن "unicode/unistr.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/utypes.h"

#ضمن "charstr.h"

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

class U_COMMON_API_CLASS CStr : عام UMemory {
  عام:
    U_COMMON_API CStr(ثابت UnicodeString &درون);
    U_COMMON_API ~CStr();
    U_COMMON_API ثابت حرف * عملگر ()() ثابت;

  خاص:
    CharString s;
    CStr(ثابت CStr &other) = delete;               //  Forbid copying of this class.
    CStr &عملگر =(ثابت CStr &other) = delete;   //  Forbid assignment.
};

U_NAMESPACE_انتها

#پایان_اگر

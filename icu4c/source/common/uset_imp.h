// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2004-2007, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uset_imp.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004sep07
*   created by: Markus W. Scherer
*
*   Internal USet definitions.
*/

#اگر_غیر_تعریف_شدہ __USET_IMP_H__
#تعریف_کرو __USET_IMP_H__

#شامل "unicode/utypes.h"
#شامل "unicode/uset.h"

U_CDECL_BEGIN

قسمتعریف خالی U_CALLCONV
USet_جوڑو(USet *سیٹ, UChar32 c);

قسمتعریف خالی U_CALLCONV
USet_جوڑو_Range(USet *سیٹ, UChar32 آغاز, UChar32 انتہا);

قسمتعریف خالی U_CALLCONV
USet_جوڑو_String(USet *سیٹ, ثابت UChar *str, int32_t لمبائی);

قسمتعریف خالی U_CALLCONV
USetRemove(USet *سیٹ, UChar32 c);

قسمتعریف خالی U_CALLCONV
USetRemoveRange(USet *سیٹ, UChar32 آغاز, UChar32 انتہا);

/**
 * Interface for adding items to a USet, to keep low-level code from
 * statically depending on the USet implementation.
 * Calls will look like sa->add(sa->set, c);
 */
ساخت USetAdder {
    USet *سیٹ;
    USet_جوڑو *جوڑو;
    USet_جوڑو_Range *جوڑو_Range;
    USet_جوڑو_String *جوڑو_String;
    USetRemove *remove;
    USetRemoveRange *removeRange;
};
قسمتعریف ساخت USetAdder USetAdder;

U_CDECL_انتہا

#اگر_تعریف_شدہ __cplusplus

namespace {

constexpr int32_t USET_CASE_نقاب = USET_CASE_INSENSITIVE | USET_جوڑو_CASE_MAPPINGS;

}  // namespace

#اگر_ختم  // __cplusplus

#اگر_ختم

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

#جيڪڏهن_وصف_نه_ٿيل __USET_IMP_H__
#وصف_ڪريو __USET_IMP_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uset.h"

U_CDECL_BEGIN

تعريفنوع فراغ U_CALLCONV
USet_جوڙيو(USet *سيٽ, UChar32 c);

تعريفنوع فراغ U_CALLCONV
USet_جوڙيو_Range(USet *سيٽ, UChar32 شروعات, UChar32 پڇاڙي);

تعريفنوع فراغ U_CALLCONV
USet_جوڙيو_String(USet *سيٽ, ثابت UChar *str, int32_t ڊيگهائي);

تعريفنوع فراغ U_CALLCONV
USetRemove(USet *سيٽ, UChar32 c);

تعريفنوع فراغ U_CALLCONV
USetRemoveRange(USet *سيٽ, UChar32 شروعات, UChar32 پڇاڙي);

/**
 * Interface for adding items to a USet, to keep low-level code from
 * statically depending on the USet implementation.
 * Calls will look like sa->add(sa->set, c);
 */
بنية USetAdder {
    USet *سيٽ;
    USet_جوڙيو *جوڙيو;
    USet_جوڙيو_Range *جوڙيو_Range;
    USet_جوڙيو_String *جوڙيو_String;
    USetRemove *remove;
    USetRemoveRange *removeRange;
};
تعريفنوع بنية USetAdder USetAdder;

U_CDECL_پڇاڙي

#جيڪڏهن_وصف_ٿيل __cplusplus

namespace {

constexpr int32_t USET_CASE_نقاب = USET_CASE_INSENSITIVE | USET_جوڙيو_CASE_MAPPINGS;

}  // namespace

#جيڪڏهن_پورو  // __cplusplus

#جيڪڏهن_پورو

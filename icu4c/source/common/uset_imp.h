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

#که_نه_تعریف_شوی __USET_IMP_H__
#تعریف_کړه __USET_IMP_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uset.h"

U_CDECL_BEGIN

تعريفنوع فراغ U_CALLCONV
USet_جمع_کړه(USet *سېټ, UChar32 c);

تعريفنوع فراغ U_CALLCONV
USet_جمع_کړه_Range(USet *سېټ, UChar32 پیل, UChar32 پای_ټکی);

تعريفنوع فراغ U_CALLCONV
USet_جمع_کړه_String(USet *سېټ, ثابت UChar *str, int32_t اوږدتیا);

تعريفنوع فراغ U_CALLCONV
USetRemove(USet *سېټ, UChar32 c);

تعريفنوع فراغ U_CALLCONV
USetRemoveRange(USet *سېټ, UChar32 پیل, UChar32 پای_ټکی);

/**
 * Interface for adding items to a USet, to keep low-level code from
 * statically depending on the USet implementation.
 * Calls will look like sa->add(sa->set, c);
 */
بنية USetAdder {
    USet *سېټ;
    USet_جمع_کړه *جمع_کړه;
    USet_جمع_کړه_Range *جمع_کړه_Range;
    USet_جمع_کړه_String *جمع_کړه_String;
    USetRemove *remove;
    USetRemoveRange *removeRange;
};
تعريفنوع بنية USetAdder USetAdder;

U_CDECL_پای_ټکی

#که_تعریف_شوی __cplusplus

namespace {

constexpr int32_t USET_CASE_ماسک = USET_CASE_INSENSITIVE | USET_جمع_کړه_CASE_MAPPINGS;

}  // namespace

#د_که_پای  // __cplusplus

#د_که_پای

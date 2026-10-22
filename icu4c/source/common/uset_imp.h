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

#اگر_تعریف_نشده __USET_IMP_H__
#تعریف_کن __USET_IMP_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/uset.h"

U_CDECL_BEGIN

تعريفنوع فراغ U_CALLCONV
USet_جمع_کن(USet *مجموعه, UChar32 c);

تعريفنوع فراغ U_CALLCONV
USet_جمع_کن_Range(USet *مجموعه, UChar32 شروع, UChar32 انتها);

تعريفنوع فراغ U_CALLCONV
USet_جمع_کن_String(USet *مجموعه, ثابت UChar *str, int32_t درازی);

تعريفنوع فراغ U_CALLCONV
USetRemove(USet *مجموعه, UChar32 c);

تعريفنوع فراغ U_CALLCONV
USetRemoveRange(USet *مجموعه, UChar32 شروع, UChar32 انتها);

/**
 * Interface for adding items to a USet, to keep low-level code from
 * statically depending on the USet implementation.
 * Calls will look like sa->add(sa->set, c);
 */
بنية USetAdder {
    USet *مجموعه;
    USet_جمع_کن *جمع_کن;
    USet_جمع_کن_Range *جمع_کن_Range;
    USet_جمع_کن_String *جمع_کن_String;
    USetRemove *remove;
    USetRemoveRange *removeRange;
};
تعريفنوع بنية USetAdder USetAdder;

U_CDECL_انتها

#اگر_تعریف_شده __cplusplus

namespace {

constexpr int32_t USET_CASE_ماسک = USET_CASE_INSENSITIVE | USET_جمع_کن_CASE_MAPPINGS;

}  // namespace

#پایان_اگر  // __cplusplus

#پایان_اگر

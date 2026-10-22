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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __USET_IMP_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __USET_IMP_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"

U_CDECL_BEGIN

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV
USet_ਜੋੜੋ(USet *ਸੈੱਟ, UChar32 c);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV
USet_ਜੋੜੋ_Range(USet *ਸੈੱਟ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV
USet_ਜੋੜੋ_String(USet *ਸੈੱਟ, ਅਚਲ UChar *str, int32_t ਲੰਮਾਈ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV
USetRemove(USet *ਸੈੱਟ, UChar32 c);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV
USetRemoveRange(USet *ਸੈੱਟ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ);

/**
 * Interface for adding items to a USet, to keep low-level code from
 * statically depending on the USet implementation.
 * Calls will look like sa->add(sa->set, c);
 */
ਬਣਤਰ USetAdder {
    USet *ਸੈੱਟ;
    USet_ਜੋੜੋ *ਜੋੜੋ;
    USet_ਜੋੜੋ_Range *ਜੋੜੋ_Range;
    USet_ਜੋੜੋ_String *ਜੋੜੋ_String;
    USetRemove *remove;
    USetRemoveRange *removeRange;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ USetAdder USetAdder;

U_CDECL_ਸਿਰਾ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus

namespace {

constexpr int32_t USET_CASE_ਨਕਾਬ = USET_CASE_INSENSITIVE | USET_ਜੋੜੋ_CASE_MAPPINGS;

}  // namespace

#ਜੇ_ਅੰਤ  // __cplusplus

#ਜੇ_ਅੰਤ

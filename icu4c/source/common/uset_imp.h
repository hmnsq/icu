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

#अगर_अघोषित __USET_IMP_H__
#घोषणा __USET_IMP_H__

#समावेश "unicode/utypes.h"
#समावेश "unicode/uset.h"

U_CDECL_BEGIN

प्रकार व्योम U_CALLCONV
USetAdd(USet *set, UChar32 c);

प्रकार व्योम U_CALLCONV
USetAddRange(USet *set, UChar32 start, UChar32 end);

प्रकार व्योम U_CALLCONV
USetAddString(USet *set, स्थिर UChar *str, int32_t लंबाई);

प्रकार व्योम U_CALLCONV
USetRemove(USet *set, UChar32 c);

प्रकार व्योम U_CALLCONV
USetRemoveRange(USet *set, UChar32 start, UChar32 end);

/**
 * Interface for adding items to a USet, to keep low-level code from
 * statically depending on the USet implementation.
 * Calls will look like sa->add(sa->set, c);
 */
काष्ठा USetAdder {
    USet *set;
    USetAdd *add;
    USetAddRange *addRange;
    USetAddString *addString;
    USetRemove *हटाओ;
    USetRemoveRange *removeRange;
};
प्रकार काष्ठा USetAdder USetAdder;

U_CDECL_END

#अगर_घोषित __cplusplus

namespace {

constexpr int32_t USET_CASE_MASK = USET_CASE_INSENSITIVE | USET_ADD_CASE_MAPPINGS;

}  // namespace

#पूर्ण_अगर  // __cplusplus

#पूर्ण_अगर

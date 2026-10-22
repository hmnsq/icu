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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __USET_IMP_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __USET_IMP_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"

U_CDECL_BEGIN

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ U_CALLCONV
USet_ಕೂಡಿಸು(USet *ಗಣ, UChar32 c);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ U_CALLCONV
USet_ಕೂಡಿಸು_Range(USet *ಗಣ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ U_CALLCONV
USet_ಕೂಡಿಸು_String(USet *ಗಣ, ಸ್ಥಿರಾಂಕ UChar *str, int32_t ಉದ್ದಳತೆ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ U_CALLCONV
USetRemove(USet *ಗಣ, UChar32 c);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ U_CALLCONV
USetRemoveRange(USet *ಗಣ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ);

/**
 * Interface for adding items to a USet, to keep low-level code from
 * statically depending on the USet implementation.
 * Calls will look like sa->add(sa->set, c);
 */
ರಚನೆ USetAdder {
    USet *ಗಣ;
    USet_ಕೂಡಿಸು *ಕೂಡಿಸು;
    USet_ಕೂಡಿಸು_Range *ಕೂಡಿಸು_Range;
    USet_ಕೂಡಿಸು_String *ಕೂಡಿಸು_String;
    USetRemove *remove;
    USetRemoveRange *removeRange;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ USetAdder USetAdder;

U_CDECL_ಕೊನೆ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus

namespace {

constexpr int32_t USET_CASE_ಮುಖವಾಡ = USET_CASE_INSENSITIVE | USET_ಕೂಡಿಸು_CASE_MAPPINGS;

}  // namespace

#ಆದರೆ_ಅಂತ್ಯ  // __cplusplus

#ಆದರೆ_ಅಂತ್ಯ

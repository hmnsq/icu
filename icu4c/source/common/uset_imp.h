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

#यदि_अपरिभाषितम् __USET_IMP_H__
#परिभाषय __USET_IMP_H__

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uset.h"

U_CDECL_BEGIN

प्रकारनिर्देश शून्य U_CALLCONV
USet_सङ्कलय(USet *समुच्चयः, UChar32 c);

प्रकारनिर्देश शून्य U_CALLCONV
USet_सङ्कलय_Range(USet *समुच्चयः, UChar32 start, UChar32 अन्तः);

प्रकारनिर्देश शून्य U_CALLCONV
USet_सङ्कलय_String(USet *समुच्चयः, ध्रुव UChar *str, int32_t दीर्घता);

प्रकारनिर्देश शून्य U_CALLCONV
USetRemove(USet *समुच्चयः, UChar32 c);

प्रकारनिर्देश शून्य U_CALLCONV
USetRemoveRange(USet *समुच्चयः, UChar32 start, UChar32 अन्तः);

/**
 * Interface for adding items to a USet, to keep low-level code from
 * statically depending on the USet implementation.
 * Calls will look like sa->add(sa->set, c);
 */
संरचना USetAdder {
    USet *समुच्चयः;
    USet_सङ्कलय *सङ्कलय;
    USet_सङ्कलय_Range *सङ्कलय_Range;
    USet_सङ्कलय_String *सङ्कलय_String;
    USetRemove *remove;
    USetRemoveRange *removeRange;
};
प्रकारनिर्देश संरचना USetAdder USetAdder;

U_CDECL_अन्तः

#यदि_परिभाषितम् __cplusplus

namespace {

constexpr int32_t USET_CASE_आवरणम् = USET_CASE_INSENSITIVE | USET_सङ्कलय_CASE_MAPPINGS;

}  // namespace

#यदि_अन्तः  // __cplusplus

#यदि_अन्तः

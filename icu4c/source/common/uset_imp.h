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

#నిర్వచితం_కాకపోతే __USET_IMP_H__
#నిర్వచించు __USET_IMP_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uset.h"

U_CDECL_BEGIN

రకనిర్వచనం శూన్య U_CALLCONV
USet_కూడు(USet *సమితి, UChar32 c);

రకనిర్వచనం శూన్య U_CALLCONV
USet_కూడు_Range(USet *సమితి, UChar32 ప్రారంభం, UChar32 చివర);

రకనిర్వచనం శూన్య U_CALLCONV
USet_కూడు_String(USet *సమితి, స్థిరరాశి UChar *str, int32_t నిడివి);

రకనిర్వచనం శూన్య U_CALLCONV
USetRemove(USet *సమితి, UChar32 c);

రకనిర్వచనం శూన్య U_CALLCONV
USetRemoveRange(USet *సమితి, UChar32 ప్రారంభం, UChar32 చివర);

/**
 * Interface for adding items to a USet, to keep low-level code from
 * statically depending on the USet implementation.
 * Calls will look like sa->add(sa->set, c);
 */
నిర్మాణం USetAdder {
    USet *సమితి;
    USet_కూడు *కూడు;
    USet_కూడు_Range *కూడు_Range;
    USet_కూడు_String *కూడు_String;
    USetRemove *remove;
    USetRemoveRange *removeRange;
};
రకనిర్వచనం నిర్మాణం USetAdder USetAdder;

U_CDECL_చివర

#నిర్వచితమైతే __cplusplus

namespace {

constexpr int32_t USET_CASE_ముసుగు = USET_CASE_INSENSITIVE | USET_కూడు_CASE_MAPPINGS;

}  // namespace

#అయితే_ముగింపు  // __cplusplus

#అయితే_ముగింపు

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uset_props.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug30
*   created by: Markus W. Scherer
*
*   C wrappers around UnicodeSet functions that are implemented in
*   uniset_props.cpp, split off for modularization.
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openPattern(ಧ್ರುವ char16_t* pattern, int32_t patternLength,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = ನೂತನ UnicodeSet(pat, *ec);
    /* test for nullptr */
    ಯದಿ (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ (USet*) set;
}

U_CAPI USet* U_EXPORT2
uset_openPatternOptions(ಧ್ರುವ char16_t* pattern, int32_t patternLength,
                 uint32_t options,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = ನೂತನ UnicodeSet(pat, options, nullptr, *ec);
    /* test for nullptr */
    ಯದಿ (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ (USet*) set;
}


U_CAPI int32_t U_EXPORT2 
uset_applyPattern(USet *set,
                  ಧ್ರುವ char16_t *pattern, int32_t patternLength,
                  uint32_t options,
                  UErrorCode *status){

    // status code needs to be checked since we 
    // dereference it
    ಯದಿ(status == nullptr || U_FAILURE(*status)){
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    ಯದಿ(set == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    UnicodeString pat(pattern, patternLength);

    ParsePosition pos;
   
    ((UnicodeSet*) set)->applyPattern(pat, pos, options, nullptr, *status);
    
    ಪ್ರತ್ಯಾಗಮ pos.getIndex();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_apply_Int_Property_ಮೌಲ್ಯ(USet* set,
               UProperty prop, int32_t value, UErrorCode* ec) {
    ((UnicodeSet*) set)->apply_Int_Property_ಮೌಲ್ಯ(prop, value, *ec);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_applyPropertyAlias(USet* set,
                        ಧ್ರುವ char16_t *prop, int32_t propLength,
                        ಧ್ರುವ char16_t *value, int32_t ಮೌಲ್ಯ_Length,
            UErrorCode* ec) {

    UnicodeString p(prop, propLength);
    UnicodeString v(value, ಮೌಲ್ಯ_Length);

    ((UnicodeSet*) set)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resemblesPattern(ಧ್ರುವ char16_t *pattern, int32_t patternLength,
                      int32_t pos) {

    UnicodeString pat(pattern, patternLength);

    ಪ್ರತ್ಯಾಗಮ ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            UnicodeSet::resemblesPattern(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_toPattern(ಧ್ರುವ USet* set,
               char16_t* result, int32_t resultCapacity,
               UBool escapeUnprintable,
               UErrorCode* ec) {
    UnicodeString pat;
    ((ಧ್ರುವ UnicodeSet*) set)->toPattern(pat, escapeUnprintable);
    ಪ್ರತ್ಯಾಗಮ pat.extract(result, resultCapacity, *ec);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_closeOver(USet* set, int32_t attributes) {
    ((UnicodeSet*) set)->UnicodeSet::closeOver(attributes);
}

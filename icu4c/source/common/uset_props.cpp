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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openPattern(स्थिरांक char16_t* pattern, int32_t patternLength,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = नवीन UnicodeSet(pat, *ec);
    /* test for nullptr */
    जर (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }

    जर (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    परत (USet*) set;
}

U_CAPI USet* U_EXPORT2
uset_openPatternOptions(स्थिरांक char16_t* pattern, int32_t patternLength,
                 uint32_t options,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = नवीन UnicodeSet(pat, options, nullptr, *ec);
    /* test for nullptr */
    जर (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }

    जर (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    परत (USet*) set;
}


U_CAPI int32_t U_EXPORT2 
uset_applyPattern(USet *set,
                  स्थिरांक char16_t *pattern, int32_t patternLength,
                  uint32_t options,
                  UErrorCode *status){

    // status code needs to be checked since we 
    // dereference it
    जर(status == nullptr || U_FAILURE(*status)){
        परत 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    जर(set == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    UnicodeString pat(pattern, patternLength);

    ParsePosition pos;
   
    ((UnicodeSet*) set)->applyPattern(pat, pos, options, nullptr, *status);
    
    परत pos.getIndex();
}

U_CAPI शून्य U_EXPORT2
uset_apply_Int_Property_मूल्य(USet* set,
               UProperty prop, int32_t value, UErrorCode* ec) {
    ((UnicodeSet*) set)->apply_Int_Property_मूल्य(prop, value, *ec);
}

U_CAPI शून्य U_EXPORT2
uset_applyPropertyAlias(USet* set,
                        स्थिरांक char16_t *prop, int32_t propLength,
                        स्थिरांक char16_t *value, int32_t मूल्य_Length,
            UErrorCode* ec) {

    UnicodeString p(prop, propLength);
    UnicodeString v(value, मूल्य_Length);

    ((UnicodeSet*) set)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resemblesPattern(स्थिरांक char16_t *pattern, int32_t patternLength,
                      int32_t pos) {

    UnicodeString pat(pattern, patternLength);

    परत ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            UnicodeSet::resemblesPattern(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_toPattern(स्थिरांक USet* set,
               char16_t* result, int32_t resultCapacity,
               UBool escapeUnprintable,
               UErrorCode* ec) {
    UnicodeString pat;
    ((स्थिरांक UnicodeSet*) set)->toPattern(pat, escapeUnprintable);
    परत pat.extract(result, resultCapacity, *ec);
}

U_CAPI शून्य U_EXPORT2
uset_closeOver(USet* set, int32_t attributes) {
    ((UnicodeSet*) set)->UnicodeSet::closeOver(attributes);
}

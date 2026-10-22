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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "unicode/uset.h"
#অন্তর্ভাৱ "unicode/uniset.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openPattern(ধ্রুৱ char16_t* pattern, int32_t patternLength,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = নূতন UnicodeSet(pat, *ec);
    /* test for nullptr */
    যদি (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }

    যদি (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    ঘূৰাই (USet*) set;
}

U_CAPI USet* U_EXPORT2
uset_openPatternOptions(ধ্রুৱ char16_t* pattern, int32_t patternLength,
                 uint32_t options,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = নূতন UnicodeSet(pat, options, nullptr, *ec);
    /* test for nullptr */
    যদি (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }

    যদি (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    ঘূৰাই (USet*) set;
}


U_CAPI int32_t U_EXPORT2 
uset_applyPattern(USet *set,
                  ধ্রুৱ char16_t *pattern, int32_t patternLength,
                  uint32_t options,
                  UErrorCode *status){

    // status code needs to be checked since we 
    // dereference it
    যদি(status == nullptr || U_FAILURE(*status)){
        ঘূৰাই 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    যদি(set == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }

    UnicodeString pat(pattern, patternLength);

    ParsePosition pos;
   
    ((UnicodeSet*) set)->applyPattern(pat, pos, options, nullptr, *status);
    
    ঘূৰাই pos.getIndex();
}

U_CAPI শূন্য U_EXPORT2
uset_apply_Int_Property_মান(USet* set,
               UProperty prop, int32_t value, UErrorCode* ec) {
    ((UnicodeSet*) set)->apply_Int_Property_মান(prop, value, *ec);
}

U_CAPI শূন্য U_EXPORT2
uset_applyPropertyAlias(USet* set,
                        ধ্রুৱ char16_t *prop, int32_t propLength,
                        ধ্রুৱ char16_t *value, int32_t মান_Length,
            UErrorCode* ec) {

    UnicodeString p(prop, propLength);
    UnicodeString v(value, মান_Length);

    ((UnicodeSet*) set)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resemblesPattern(ধ্রুৱ char16_t *pattern, int32_t patternLength,
                      int32_t pos) {

    UnicodeString pat(pattern, patternLength);

    ঘূৰাই ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            UnicodeSet::resemblesPattern(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_toPattern(ধ্রুৱ USet* set,
               char16_t* result, int32_t resultCapacity,
               UBool escapeUnprintable,
               UErrorCode* ec) {
    UnicodeString pat;
    ((ধ্রুৱ UnicodeSet*) set)->toPattern(pat, escapeUnprintable);
    ঘূৰাই pat.extract(result, resultCapacity, *ec);
}

U_CAPI শূন্য U_EXPORT2
uset_closeOver(USet* set, int32_t attributes) {
    ((UnicodeSet*) set)->UnicodeSet::closeOver(attributes);
}

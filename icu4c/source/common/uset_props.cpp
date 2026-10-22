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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "unicode/uset.h"
#அந்தர்பாவ "unicode/uniset.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openPattern(மாறிலி char16_t* pattern, int32_t patternLength,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = புதிய UnicodeSet(pat, *ec);
    /* test for nullptr */
    எனில் (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }

    எனில் (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    திரும்பு (USet*) set;
}

U_CAPI USet* U_EXPORT2
uset_openPatternOptions(மாறிலி char16_t* pattern, int32_t patternLength,
                 uint32_t options,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = புதிய UnicodeSet(pat, options, nullptr, *ec);
    /* test for nullptr */
    எனில் (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }

    எனில் (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    திரும்பு (USet*) set;
}


U_CAPI int32_t U_EXPORT2 
uset_applyPattern(USet *set,
                  மாறிலி char16_t *pattern, int32_t patternLength,
                  uint32_t options,
                  UErrorCode *status){

    // status code needs to be checked since we 
    // dereference it
    எனில்(status == nullptr || U_FAILURE(*status)){
        திரும்பு 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    எனில்(set == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    UnicodeString pat(pattern, patternLength);

    ParsePosition pos;
   
    ((UnicodeSet*) set)->applyPattern(pat, pos, options, nullptr, *status);
    
    திரும்பு pos.getIndex();
}

U_CAPI வெறுமை U_EXPORT2
uset_apply_Int_Property_மதிப்பு(USet* set,
               UProperty prop, int32_t value, UErrorCode* ec) {
    ((UnicodeSet*) set)->apply_Int_Property_மதிப்பு(prop, value, *ec);
}

U_CAPI வெறுமை U_EXPORT2
uset_applyPropertyAlias(USet* set,
                        மாறிலி char16_t *prop, int32_t propLength,
                        மாறிலி char16_t *value, int32_t மதிப்பு_Length,
            UErrorCode* ec) {

    UnicodeString p(prop, propLength);
    UnicodeString v(value, மதிப்பு_Length);

    ((UnicodeSet*) set)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resemblesPattern(மாறிலி char16_t *pattern, int32_t patternLength,
                      int32_t pos) {

    UnicodeString pat(pattern, patternLength);

    திரும்பு ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            UnicodeSet::resemblesPattern(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_toPattern(மாறிலி USet* set,
               char16_t* result, int32_t resultCapacity,
               UBool escapeUnprintable,
               UErrorCode* ec) {
    UnicodeString pat;
    ((மாறிலி UnicodeSet*) set)->toPattern(pat, escapeUnprintable);
    திரும்பு pat.extract(result, resultCapacity, *ec);
}

U_CAPI வெறுமை U_EXPORT2
uset_closeOver(USet* set, int32_t attributes) {
    ((UnicodeSet*) set)->UnicodeSet::closeOver(attributes);
}

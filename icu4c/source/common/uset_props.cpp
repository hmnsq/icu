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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/uset.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openPattern(ধ্রুবক char16_t* pattern, int32_t pattern_দৈর্ঘ্য,
                 UErrorCode* ec)
{
    UnicodeString pat(pattern_দৈর্ঘ্য==-1, pattern, pattern_দৈর্ঘ্য);
    UnicodeSet* set = নতুন UnicodeSet(pat, *ec);
    /* test for nullptr */
    যদি (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }

    যদি (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    ফেরত (USet*) set;
}

U_CAPI USet* U_EXPORT2
uset_openPatternOptions(ধ্রুবক char16_t* pattern, int32_t pattern_দৈর্ঘ্য,
                 uint32_t options,
                 UErrorCode* ec)
{
    UnicodeString pat(pattern_দৈর্ঘ্য==-1, pattern, pattern_দৈর্ঘ্য);
    UnicodeSet* set = নতুন UnicodeSet(pat, options, nullptr, *ec);
    /* test for nullptr */
    যদি (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }

    যদি (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    ফেরত (USet*) set;
}


U_CAPI int32_t U_EXPORT2 
uset_applyPattern(USet *set,
                  ধ্রুবক char16_t *pattern, int32_t pattern_দৈর্ঘ্য,
                  uint32_t options,
                  UErrorCode *status){

    // status code needs to be checked since we 
    // dereference it
    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    যদি(set == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    UnicodeString pat(pattern, pattern_দৈর্ঘ্য);

    ParsePosition pos;
   
    ((UnicodeSet*) set)->applyPattern(pat, pos, options, nullptr, *status);
    
    ফেরত pos.get_সূচক();
}

U_CAPI শূন্য U_EXPORT2
uset_apply_Int_Property_মান(USet* set,
               UProperty prop, int32_t value, UErrorCode* ec) {
    ((UnicodeSet*) set)->apply_Int_Property_মান(prop, value, *ec);
}

U_CAPI শূন্য U_EXPORT2
uset_applyPropertyAlias(USet* set,
                        ধ্রুবক char16_t *prop, int32_t prop_দৈর্ঘ্য,
                        ধ্রুবক char16_t *value, int32_t মান_দৈর্ঘ্য,
            UErrorCode* ec) {

    UnicodeString p(prop, prop_দৈর্ঘ্য);
    UnicodeString v(value, মান_দৈর্ঘ্য);

    ((UnicodeSet*) set)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resemblesPattern(ধ্রুবক char16_t *pattern, int32_t pattern_দৈর্ঘ্য,
                      int32_t pos) {

    UnicodeString pat(pattern, pattern_দৈর্ঘ্য);

    ফেরত ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            UnicodeSet::resemblesPattern(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_toPattern(ধ্রুবক USet* set,
               char16_t* ফলাফল, int32_t ফলাফল_Capacity,
               UBool escapeUnprintable,
               UErrorCode* ec) {
    UnicodeString pat;
    ((ধ্রুবক UnicodeSet*) set)->toPattern(pat, escapeUnprintable);
    ফেরত pat.extract(ফলাফল, ফলাফল_Capacity, *ec);
}

U_CAPI শূন্য U_EXPORT2
uset_closeOver(USet* set, int32_t attributes) {
    ((UnicodeSet*) set)->UnicodeSet::closeOver(attributes);
}

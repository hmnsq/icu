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
uset_openPattern(अचर char16_t* pattern, int32_t patternLength,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = नूतन UnicodeSet(pat, *ec);
    /* test for nullptr */
    यदि (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }

    यदि (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    फर्काऊ (USet*) set;
}

U_CAPI USet* U_EXPORT2
uset_openPatternOptions(अचर char16_t* pattern, int32_t patternLength,
                 uint32_t options,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = नूतन UnicodeSet(pat, options, nullptr, *ec);
    /* test for nullptr */
    यदि (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }

    यदि (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    फर्काऊ (USet*) set;
}


U_CAPI int32_t U_EXPORT2 
uset_applyPattern(USet *set,
                  अचर char16_t *pattern, int32_t patternLength,
                  uint32_t options,
                  UErrorCode *status){

    // status code needs to be checked since we 
    // dereference it
    यदि(status == nullptr || U_FAILURE(*status)){
        फर्काऊ 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    यदि(set == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    UnicodeString pat(pattern, patternLength);

    ParsePosition pos;
   
    ((UnicodeSet*) set)->applyPattern(pat, pos, options, nullptr, *status);
    
    फर्काऊ pos.getIndex();
}

U_CAPI शून्य U_EXPORT2
uset_apply_Int_Property_मान(USet* set,
               UProperty prop, int32_t value, UErrorCode* ec) {
    ((UnicodeSet*) set)->apply_Int_Property_मान(prop, value, *ec);
}

U_CAPI शून्य U_EXPORT2
uset_applyPropertyAlias(USet* set,
                        अचर char16_t *prop, int32_t propLength,
                        अचर char16_t *value, int32_t मान_Length,
            UErrorCode* ec) {

    UnicodeString p(prop, propLength);
    UnicodeString v(value, मान_Length);

    ((UnicodeSet*) set)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resemblesPattern(अचर char16_t *pattern, int32_t patternLength,
                      int32_t pos) {

    UnicodeString pat(pattern, patternLength);

    फर्काऊ ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            UnicodeSet::resemblesPattern(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_toPattern(अचर USet* set,
               char16_t* result, int32_t resultCapacity,
               UBool escapeUnprintable,
               UErrorCode* ec) {
    UnicodeString pat;
    ((अचर UnicodeSet*) set)->toPattern(pat, escapeUnprintable);
    फर्काऊ pat.extract(result, resultCapacity, *ec);
}

U_CAPI शून्य U_EXPORT2
uset_closeOver(USet* set, int32_t attributes) {
    ((UnicodeSet*) set)->UnicodeSet::closeOver(attributes);
}

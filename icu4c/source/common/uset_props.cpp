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

#समावेश "unicode/utypes.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/uniset.h"
#समावेश "cmemory.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openPattern(स्थिर char16_t* pattern, int32_t pattern_लंबाई,
                 UErrorCode* ec)
{
    UnicodeString pat(pattern_लंबाई==-1, pattern, pattern_लंबाई);
    UnicodeSet* set = नया UnicodeSet(pat, *ec);
    /* test for nullptr */
    अगर (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }

    अगर (U_FAILURE(*ec)) {
        मिटाओ set;
        set = nullptr;
    }
    वापस (USet*) set;
}

U_CAPI USet* U_EXPORT2
uset_openPatternOptions(स्थिर char16_t* pattern, int32_t pattern_लंबाई,
                 uint32_t options,
                 UErrorCode* ec)
{
    UnicodeString pat(pattern_लंबाई==-1, pattern, pattern_लंबाई);
    UnicodeSet* set = नया UnicodeSet(pat, options, nullptr, *ec);
    /* test for nullptr */
    अगर (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }

    अगर (U_FAILURE(*ec)) {
        मिटाओ set;
        set = nullptr;
    }
    वापस (USet*) set;
}


U_CAPI int32_t U_EXPORT2 
uset_applyPattern(USet *set,
                  स्थिर char16_t *pattern, int32_t pattern_लंबाई,
                  uint32_t options,
                  UErrorCode *status){

    // status code needs to be checked since we 
    // dereference it
    अगर(status == nullptr || U_FAILURE(*status)){
        वापस 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    अगर(set == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    UnicodeString pat(pattern, pattern_लंबाई);

    ParsePosition pos;
   
    ((UnicodeSet*) set)->applyPattern(pat, pos, options, nullptr, *status);
    
    वापस pos.get_सूचकांक();
}

U_CAPI व्योम U_EXPORT2
uset_apply_Int_Property_मूल्य(USet* set,
               UProperty prop, int32_t value, UErrorCode* ec) {
    ((UnicodeSet*) set)->apply_Int_Property_मूल्य(prop, value, *ec);
}

U_CAPI व्योम U_EXPORT2
uset_applyPropertyAlias(USet* set,
                        स्थिर char16_t *prop, int32_t prop_लंबाई,
                        स्थिर char16_t *value, int32_t मूल्य_लंबाई,
            UErrorCode* ec) {

    UnicodeString p(prop, prop_लंबाई);
    UnicodeString v(value, मूल्य_लंबाई);

    ((UnicodeSet*) set)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resemblesPattern(स्थिर char16_t *pattern, int32_t pattern_लंबाई,
                      int32_t pos) {

    UnicodeString pat(pattern, pattern_लंबाई);

    वापस ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            UnicodeSet::resemblesPattern(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_toPattern(स्थिर USet* set,
               char16_t* परिणाम, int32_t परिणाम_Capacity,
               UBool escapeUnprintable,
               UErrorCode* ec) {
    UnicodeString pat;
    ((स्थिर UnicodeSet*) set)->toPattern(pat, escapeUnprintable);
    वापस pat.extract(परिणाम, परिणाम_Capacity, *ec);
}

U_CAPI व्योम U_EXPORT2
uset_closeOver(USet* set, int32_t attributes) {
    ((UnicodeSet*) set)->UnicodeSet::closeOver(attributes);
}

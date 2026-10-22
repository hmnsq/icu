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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/uset.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openPattern(സ്ഥിരം char16_t* pattern, int32_t patternLength,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = പുതിയ UnicodeSet(pat, *ec);
    /* test for nullptr */
    എങ്കിൽ (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }

    എങ്കിൽ (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    തിരികെ (USet*) set;
}

U_CAPI USet* U_EXPORT2
uset_openPatternOptions(സ്ഥിരം char16_t* pattern, int32_t patternLength,
                 uint32_t options,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = പുതിയ UnicodeSet(pat, options, nullptr, *ec);
    /* test for nullptr */
    എങ്കിൽ (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }

    എങ്കിൽ (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    തിരികെ (USet*) set;
}


U_CAPI int32_t U_EXPORT2 
uset_applyPattern(USet *set,
                  സ്ഥിരം char16_t *pattern, int32_t patternLength,
                  uint32_t options,
                  UErrorCode *status){

    // status code needs to be checked since we 
    // dereference it
    എങ്കിൽ(status == nullptr || U_FAILURE(*status)){
        തിരികെ 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    എങ്കിൽ(set == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    UnicodeString pat(pattern, patternLength);

    ParsePosition pos;
   
    ((UnicodeSet*) set)->applyPattern(pat, pos, options, nullptr, *status);
    
    തിരികെ pos.getIndex();
}

U_CAPI ശൂന്യം U_EXPORT2
uset_apply_Int_Property_മൂല്യം(USet* set,
               UProperty prop, int32_t value, UErrorCode* ec) {
    ((UnicodeSet*) set)->apply_Int_Property_മൂല്യം(prop, value, *ec);
}

U_CAPI ശൂന്യം U_EXPORT2
uset_applyPropertyAlias(USet* set,
                        സ്ഥിരം char16_t *prop, int32_t propLength,
                        സ്ഥിരം char16_t *value, int32_t മൂല്യം_Length,
            UErrorCode* ec) {

    UnicodeString p(prop, propLength);
    UnicodeString v(value, മൂല്യം_Length);

    ((UnicodeSet*) set)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resemblesPattern(സ്ഥിരം char16_t *pattern, int32_t patternLength,
                      int32_t pos) {

    UnicodeString pat(pattern, patternLength);

    തിരികെ ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            UnicodeSet::resemblesPattern(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_toPattern(സ്ഥിരം USet* set,
               char16_t* result, int32_t resultCapacity,
               UBool escapeUnprintable,
               UErrorCode* ec) {
    UnicodeString pat;
    ((സ്ഥിരം UnicodeSet*) set)->toPattern(pat, escapeUnprintable);
    തിരികെ pat.extract(result, resultCapacity, *ec);
}

U_CAPI ശൂന്യം U_EXPORT2
uset_closeOver(USet* set, int32_t attributes) {
    ((UnicodeSet*) set)->UnicodeSet::closeOver(attributes);
}

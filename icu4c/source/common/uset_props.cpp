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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "unicode/uset.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_openPattern(અચલ char16_t* pattern, int32_t patternLength,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = નવું UnicodeSet(pat, *ec);
    /* test for nullptr */
    જો (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }

    જો (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    પાછા (USet*) set;
}

U_CAPI USet* U_EXPORT2
uset_openPatternOptions(અચલ char16_t* pattern, int32_t patternLength,
                 uint32_t options,
                 UErrorCode* ec)
{
    UnicodeString pat(patternLength==-1, pattern, patternLength);
    UnicodeSet* set = નવું UnicodeSet(pat, options, nullptr, *ec);
    /* test for nullptr */
    જો (set == nullptr) {
        *ec = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }

    જો (U_FAILURE(*ec)) {
        delete set;
        set = nullptr;
    }
    પાછા (USet*) set;
}


U_CAPI int32_t U_EXPORT2 
uset_applyPattern(USet *set,
                  અચલ char16_t *pattern, int32_t patternLength,
                  uint32_t options,
                  UErrorCode *status){

    // status code needs to be checked since we 
    // dereference it
    જો(status == nullptr || U_FAILURE(*status)){
        પાછા 0;
    }

    // check only the set paramenter
    // if pattern is nullptr or NUL terminated
    // UnicodeString constructor takes care of it
    જો(set == nullptr){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    UnicodeString pat(pattern, patternLength);

    ParsePosition pos;
   
    ((UnicodeSet*) set)->applyPattern(pat, pos, options, nullptr, *status);
    
    પાછા pos.getIndex();
}

U_CAPI શૂન્ય U_EXPORT2
uset_apply_Int_Property_કિંમત(USet* set,
               UProperty prop, int32_t value, UErrorCode* ec) {
    ((UnicodeSet*) set)->apply_Int_Property_કિંમત(prop, value, *ec);
}

U_CAPI શૂન્ય U_EXPORT2
uset_applyPropertyAlias(USet* set,
                        અચલ char16_t *prop, int32_t propLength,
                        અચલ char16_t *value, int32_t કિંમત_Length,
            UErrorCode* ec) {

    UnicodeString p(prop, propLength);
    UnicodeString v(value, કિંમત_Length);

    ((UnicodeSet*) set)->applyPropertyAlias(p, v, *ec);
}

U_CAPI UBool U_EXPORT2
uset_resemblesPattern(અચલ char16_t *pattern, int32_t patternLength,
                      int32_t pos) {

    UnicodeString pat(pattern, patternLength);

    પાછા ((pos+1) < pat.length() &&
            pat.charAt(pos) == (char16_t)91/*[*/) ||
            UnicodeSet::resemblesPattern(pat, pos);
}

U_CAPI int32_t U_EXPORT2
uset_toPattern(અચલ USet* set,
               char16_t* result, int32_t resultCapacity,
               UBool escapeUnprintable,
               UErrorCode* ec) {
    UnicodeString pat;
    ((અચલ UnicodeSet*) set)->toPattern(pat, escapeUnprintable);
    પાછા pat.extract(result, resultCapacity, *ec);
}

U_CAPI શૂન્ય U_EXPORT2
uset_closeOver(USet* set, int32_t attributes) {
    ((UnicodeSet*) set)->UnicodeSet::closeOver(attributes);
}

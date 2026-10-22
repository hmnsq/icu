// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2012,2014 International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*/

/**
 * \file
 * \brief C++: internal template EnumSet<>
 */

#ifndef ENUMSET_H
#define ENUMSET_H

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
template<typename T, uint32_t min_ମୂଲ୍ୟ, uint32_t limit_ମୂଲ୍ୟ>
class EnumSet {
ସର୍ବସାଧାରଣ:
    ଧାଡ଼ିଭିତରେ EnumSet() : fBools(0) {}
    ଧାଡ଼ିଭିତରେ EnumSet(ଅଚଳ EnumSet<T,min_ମୂଲ୍ୟ,limit_ମୂଲ୍ୟ>& other) : fBools(other.fBools) {}
    ଧାଡ଼ିଭିତରେ ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ clear() { fBools=0; }
    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ add(T toAdd) { set(toAdd, 1); }
    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ remove(T toRemove) { set(toRemove, 0); }
    ଧାଡ଼ିଭିତରେ int32_t contains(T toCheck) ଅଚଳ { ଫେରନ୍ତୁ get(toCheck); }
    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    ଧାଡ଼ିଭିତରେ int32_t get(T toCheck) ଅଚଳ { ଫେରନ୍ତୁ (fBools & flag(toCheck))?1:0; }
    ଧାଡ଼ିଭିତରେ UBool isValidEnum(T toCheck) ଅଚଳ {  ଫେରନ୍ତୁ (toCheck>=min_ମୂଲ୍ୟ&&toCheck<limit_ମୂଲ୍ୟ); }
    ଧାଡ଼ିଭିତରେ UBool is_Valid_ମୂଲ୍ୟ(int32_t v) ଅଚଳ { ଫେରନ୍ତୁ (v==0||v==1); }
    ଧାଡ଼ିଭିତରେ ଅଚଳ EnumSet<T, min_ମୂଲ୍ୟ, limit_ମୂଲ୍ୟ>&
    operator=(ଅଚଳ EnumSet<T, min_ମୂଲ୍ୟ, limit_ମୂଲ୍ୟ>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        ଫେରନ୍ତୁ *this;
    }
  
    ଧାଡ଼ିଭିତରେ uint32_t getAll() ଅଚଳ {
        ଫେରନ୍ତୁ fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

ବ୍ୟକ୍ତିଗତ:
    ଧାଡ଼ିଭିତରେ uint32_t flag(T toCheck) ଅଚଳ { ଫେରନ୍ତୁ (1<<(toCheck-min_ମୂଲ୍ୟ)); }
ବ୍ୟକ୍ତିଗତ:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

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

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
template<typename T, uint32_t min_মান, uint32_t limit_মান>
class EnumSet {
সর্বজনীন:
    অন্তঃরেখ EnumSet() : fBools(0) {}
    অন্তঃরেখ EnumSet(ধ্রুবক EnumSet<T,min_মান,limit_মান>& other) : fBools(other.fBools) {}
    অন্তঃরেখ ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    অন্তঃরেখ শূন্য clear() { fBools=0; }
    অন্তঃরেখ শূন্য add(T toAdd) { set(toAdd, 1); }
    অন্তঃরেখ শূন্য remove(T toRemove) { set(toRemove, 0); }
    অন্তঃরেখ int32_t contains(T toCheck) ধ্রুবক { ফেরত get(toCheck); }
    অন্তঃরেখ শূন্য set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    অন্তঃরেখ int32_t get(T toCheck) ধ্রুবক { ফেরত (fBools & flag(toCheck))?1:0; }
    অন্তঃরেখ UBool isValidEnum(T toCheck) ধ্রুবক {  ফেরত (toCheck>=min_মান&&toCheck<limit_মান); }
    অন্তঃরেখ UBool is_Valid_মান(int32_t v) ধ্রুবক { ফেরত (v==0||v==1); }
    অন্তঃরেখ ধ্রুবক EnumSet<T, min_মান, limit_মান>&
    operator=(ধ্রুবক EnumSet<T, min_মান, limit_মান>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        ফেরত *this;
    }
  
    অন্তঃরেখ uint32_t getAll() ধ্রুবক {
        ফেরত fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

ব্যক্তিগত:
    অন্তঃরেখ uint32_t flag(T toCheck) ধ্রুবক { ফেরত (1<<(toCheck-min_মান)); }
ব্যক্তিগত:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

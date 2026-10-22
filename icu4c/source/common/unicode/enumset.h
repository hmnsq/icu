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
সার্বজনিক:
    অন্তঃস্থ EnumSet() : fBools(0) {}
    অন্তঃস্থ EnumSet(ধ্রুব EnumSet<T,min_মান,limit_মান>& other) : fBools(other.fBools) {}
    অন্তঃস্থ ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    অন্তঃস্থ শূন্য clear() { fBools=0; }
    অন্তঃস্থ শূন্য add(T toAdd) { set(toAdd, 1); }
    অন্তঃস্থ শূন্য remove(T toRemove) { set(toRemove, 0); }
    অন্তঃস্থ int32_t contains(T toCheck) ধ্রুব { প্রত্যাগম get(toCheck); }
    অন্তঃস্থ শূন্য set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    অন্তঃস্থ int32_t get(T toCheck) ধ্রুব { প্রত্যাগম (fBools & flag(toCheck))?1:0; }
    অন্তঃস্থ UBool isValidEnum(T toCheck) ধ্রুব {  প্রত্যাগম (toCheck>=min_মান&&toCheck<limit_মান); }
    অন্তঃস্থ UBool is_Valid_মান(int32_t v) ধ্রুব { প্রত্যাগম (v==0||v==1); }
    অন্তঃস্থ ধ্রুব EnumSet<T, min_মান, limit_মান>&
    operator=(ধ্রুব EnumSet<T, min_মান, limit_মান>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        প্রত্যাগম *this;
    }
  
    অন্তঃস্থ uint32_t getAll() ধ্রুব {
        প্রত্যাগম fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

গুপ্ত:
    অন্তঃস্থ uint32_t flag(T toCheck) ধ্রুব { প্রত্যাগম (1<<(toCheck-min_মান)); }
গুপ্ত:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

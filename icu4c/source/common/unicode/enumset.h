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

#ضمن "unicode/utypes.h"

#إذا U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
template<typename T, uint32_t min_قدر, uint32_t limit_قدر>
class EnumSet {
عام:
    مضمن EnumSet() : fBools(0) {}
    مضمن EnumSet(ثابت EnumSet<T,min_قدر,limit_قدر>& other) : fBools(other.fBools) {}
    مضمن ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    مضمن فراغ clear() { fBools=0; }
    مضمن فراغ add(T toAdd) { set(toAdd, 1); }
    مضمن فراغ remove(T toRemove) { set(toRemove, 0); }
    مضمن int32_t contains(T toCheck) ثابت { أرجع get(toCheck); }
    مضمن فراغ set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    مضمن int32_t get(T toCheck) ثابت { أرجع (fBools & flag(toCheck))?1:0; }
    مضمن UBool isValidEnum(T toCheck) ثابت {  أرجع (toCheck>=min_قدر&&toCheck<limit_قدر); }
    مضمن UBool is_Valid_قدر(int32_t v) ثابت { أرجع (v==0||v==1); }
    مضمن ثابت EnumSet<T, min_قدر, limit_قدر>&
    operator=(ثابت EnumSet<T, min_قدر, limit_قدر>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        أرجع *this;
    }
  
    مضمن uint32_t getAll() ثابت {
        أرجع fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

خاص:
    مضمن uint32_t flag(T toCheck) ثابت { أرجع (1<<(toCheck-min_قدر)); }
خاص:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

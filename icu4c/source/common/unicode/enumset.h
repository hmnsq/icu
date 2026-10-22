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

#include "unicode/utypes.h"

#if U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
template<typename T, uint32_t min_हालागा, uint32_t limit_हालागा>
class EnumSet {
public:
    inline EnumSet() : fBools(0) {}
    inline EnumSet(const EnumSet<T,min_हालागा,limit_हालागा>& other) : fBools(other.fBools) {}
    inline ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    inline void clear() { fBools=0; }
    inline void add(T toAdd) { set(toAdd, 1); }
    inline void remove(T toRemove) { set(toRemove, 0); }
    inline int32_t contains(T toCheck) const { return get(toCheck); }
    inline void set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    inline int32_t get(T toCheck) const { return (fBools & flag(toCheck))?1:0; }
    inline UBool isValidEnum(T toCheck) const {  return (toCheck>=min_हालागा&&toCheck<limit_हालागा); }
    inline UBool is_Valid_हालागा(int32_t v) const { return (v==0||v==1); }
    inline const EnumSet<T, min_हालागा, limit_हालागा>&
    operator=(const EnumSet<T, min_हालागा, limit_हालागा>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        return *this;
    }
  
    inline uint32_t getAll() const {
        return fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

private:
    inline uint32_t flag(T toCheck) const { return (1<<(toCheck-min_हालागा)); }
private:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
template<typename T, uint32_t min_ಮೌಲ್ಯ, uint32_t limit_ಮೌಲ್ಯ>
class EnumSet {
ಸಾರ್ವಜನಿಕ:
    ಅನ್ತಃಸ್ಥ EnumSet() : fBools(0) {}
    ಅನ್ತಃಸ್ಥ EnumSet(ಧ್ರುವ EnumSet<T,min_ಮೌಲ್ಯ,limit_ಮೌಲ್ಯ>& other) : fBools(other.fBools) {}
    ಅನ್ತಃಸ್ಥ ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ clear() { fBools=0; }
    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ add(T toAdd) { set(toAdd, 1); }
    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ remove(T toRemove) { set(toRemove, 0); }
    ಅನ್ತಃಸ್ಥ int32_t contains(T toCheck) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ get(toCheck); }
    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    ಅನ್ತಃಸ್ಥ int32_t get(T toCheck) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (fBools & flag(toCheck))?1:0; }
    ಅನ್ತಃಸ್ಥ UBool isValidEnum(T toCheck) ಧ್ರುವ {  ಪ್ರತ್ಯಾಗಮ (toCheck>=min_ಮೌಲ್ಯ&&toCheck<limit_ಮೌಲ್ಯ); }
    ಅನ್ತಃಸ್ಥ UBool is_Valid_ಮೌಲ್ಯ(int32_t v) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (v==0||v==1); }
    ಅನ್ತಃಸ್ಥ ಧ್ರುವ EnumSet<T, min_ಮೌಲ್ಯ, limit_ಮೌಲ್ಯ>&
    operator=(ಧ್ರುವ EnumSet<T, min_ಮೌಲ್ಯ, limit_ಮೌಲ್ಯ>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        ಪ್ರತ್ಯಾಗಮ *this;
    }
  
    ಅನ್ತಃಸ್ಥ uint32_t getAll() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

ಗುಪ್ತ:
    ಅನ್ತಃಸ್ಥ uint32_t flag(T toCheck) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (1<<(toCheck-min_ಮೌಲ್ಯ)); }
ಗುಪ್ತ:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

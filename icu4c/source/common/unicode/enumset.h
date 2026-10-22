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

#अन्तर्भाव "unicode/utypes.h"

#जर U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
ढाँचा<typename T, uint32_t min_मूल्य, uint32_t limit_मूल्य>
class EnumSet {
सार्वजनिक:
    ओळीत EnumSet() : fBools(0) {}
    ओळीत EnumSet(स्थिरांक EnumSet<T,min_मूल्य,limit_मूल्य>& other) : fBools(other.fBools) {}
    ओळीत ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    ओळीत शून्य clear() { fBools=0; }
    ओळीत शून्य add(T toAdd) { set(toAdd, 1); }
    ओळीत शून्य हटाओ(T toRemove) { set(toRemove, 0); }
    ओळीत int32_t contains(T toCheck) स्थिरांक { परत get(toCheck); }
    ओळीत शून्य set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    ओळीत int32_t get(T toCheck) स्थिरांक { परत (fBools & flag(toCheck))?1:0; }
    ओळीत UBool isValidEnum(T toCheck) स्थिरांक {  परत (toCheck>=min_मूल्य&&toCheck<limit_मूल्य); }
    ओळीत UBool is_Valid_मूल्य(int32_t v) स्थिरांक { परत (v==0||v==1); }
    ओळीत स्थिरांक EnumSet<T, min_मूल्य, limit_मूल्य>&
    चालक=(स्थिरांक EnumSet<T, min_मूल्य, limit_मूल्य>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        परत *this;
    }
  
    ओळीत uint32_t getAll() स्थिरांक {
        परत fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

खाजगी:
    ओळीत uint32_t flag(T toCheck) स्थिरांक { परत (1<<(toCheck-min_मूल्य)); }
खाजगी:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

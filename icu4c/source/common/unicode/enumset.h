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

#यदि U_SHOW_CPLUSPLUS_API

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
    अन्तःस्थ EnumSet() : fBools(0) {}
    अन्तःस्थ EnumSet(ध्रुव EnumSet<T,min_मूल्य,limit_मूल्य>& other) : fBools(other.fBools) {}
    अन्तःस्थ ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    अन्तःस्थ शून्य clear() { fBools=0; }
    अन्तःस्थ शून्य add(T toAdd) { set(toAdd, 1); }
    अन्तःस्थ शून्य हटाओ(T toRemove) { set(toRemove, 0); }
    अन्तःस्थ int32_t contains(T toCheck) ध्रुव { प्रत्यागम get(toCheck); }
    अन्तःस्थ शून्य set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    अन्तःस्थ int32_t get(T toCheck) ध्रुव { प्रत्यागम (fBools & flag(toCheck))?1:0; }
    अन्तःस्थ UBool isValidEnum(T toCheck) ध्रुव {  प्रत्यागम (toCheck>=min_मूल्य&&toCheck<limit_मूल्य); }
    अन्तःस्थ UBool is_Valid_मूल्य(int32_t v) ध्रुव { प्रत्यागम (v==0||v==1); }
    अन्तःस्थ ध्रुव EnumSet<T, min_मूल्य, limit_मूल्य>&
    चालक=(ध्रुव EnumSet<T, min_मूल्य, limit_मूल्य>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        प्रत्यागम *this;
    }
  
    अन्तःस्थ uint32_t getAll() ध्रुव {
        प्रत्यागम fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

गुप्त:
    अन्तःस्थ uint32_t flag(T toCheck) ध्रुव { प्रत्यागम (1<<(toCheck-min_मूल्य)); }
गुप्त:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

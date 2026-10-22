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

#અન્તર્ભાવ "unicode/utypes.h"

#જો U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
template<typename T, uint32_t min_કિંમત, uint32_t limit_કિંમત>
class EnumSet {
જાહેર:
    અંતઃરેખ EnumSet() : fBools(0) {}
    અંતઃરેખ EnumSet(અચલ EnumSet<T,min_કિંમત,limit_કિંમત>& other) : fBools(other.fBools) {}
    અંતઃરેખ ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    અંતઃરેખ શૂન્ય clear() { fBools=0; }
    અંતઃરેખ શૂન્ય add(T toAdd) { set(toAdd, 1); }
    અંતઃરેખ શૂન્ય remove(T toRemove) { set(toRemove, 0); }
    અંતઃરેખ int32_t contains(T toCheck) અચલ { પાછા get(toCheck); }
    અંતઃરેખ શૂન્ય set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    અંતઃરેખ int32_t get(T toCheck) અચલ { પાછા (fBools & flag(toCheck))?1:0; }
    અંતઃરેખ UBool isValidEnum(T toCheck) અચલ {  પાછા (toCheck>=min_કિંમત&&toCheck<limit_કિંમત); }
    અંતઃરેખ UBool is_Valid_કિંમત(int32_t v) અચલ { પાછા (v==0||v==1); }
    અંતઃરેખ અચલ EnumSet<T, min_કિંમત, limit_કિંમત>&
    operator=(અચલ EnumSet<T, min_કિંમત, limit_કિંમત>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        પાછા *this;
    }
  
    અંતઃરેખ uint32_t getAll() અચલ {
        પાછા fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

ખાનગી:
    અંતઃરેખ uint32_t flag(T toCheck) અચલ { પાછા (1<<(toCheck-min_કિંમત)); }
ખાનગી:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

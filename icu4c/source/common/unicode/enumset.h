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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
template<typename T, uint32_t min_மதிப்பு, uint32_t limit_மதிப்பு>
class EnumSet {
பொது:
    உள்வரி EnumSet() : fBools(0) {}
    உள்வரி EnumSet(மாறிலி EnumSet<T,min_மதிப்பு,limit_மதிப்பு>& other) : fBools(other.fBools) {}
    உள்வரி ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    உள்வரி வெறுமை clear() { fBools=0; }
    உள்வரி வெறுமை add(T toAdd) { set(toAdd, 1); }
    உள்வரி வெறுமை remove(T toRemove) { set(toRemove, 0); }
    உள்வரி int32_t contains(T toCheck) மாறிலி { திரும்பு get(toCheck); }
    உள்வரி வெறுமை set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    உள்வரி int32_t get(T toCheck) மாறிலி { திரும்பு (fBools & flag(toCheck))?1:0; }
    உள்வரி UBool isValidEnum(T toCheck) மாறிலி {  திரும்பு (toCheck>=min_மதிப்பு&&toCheck<limit_மதிப்பு); }
    உள்வரி UBool is_Valid_மதிப்பு(int32_t v) மாறிலி { திரும்பு (v==0||v==1); }
    உள்வரி மாறிலி EnumSet<T, min_மதிப்பு, limit_மதிப்பு>&
    operator=(மாறிலி EnumSet<T, min_மதிப்பு, limit_மதிப்பு>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        திரும்பு *this;
    }
  
    உள்வரி uint32_t getAll() மாறிலி {
        திரும்பு fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

தனிப்பட்ட:
    உள்வரி uint32_t flag(T toCheck) மாறிலி { திரும்பு (1<<(toCheck-min_மதிப்பு)); }
தனிப்பட்ட:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

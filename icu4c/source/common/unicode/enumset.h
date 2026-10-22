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

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
template<typename T, uint32_t min_مقدار, uint32_t limit_مقدار>
class EnumSet {
عمومی:
    درونخطی EnumSet() : fBools(0) {}
    درونخطی EnumSet(ثابت EnumSet<T,min_مقدار,limit_مقدار>& other) : fBools(other.fBools) {}
    درونخطی ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    درونخطی تهی clear() { fBools=0; }
    درونخطی تهی add(T toAdd) { set(toAdd, 1); }
    درونخطی تهی remove(T toRemove) { set(toRemove, 0); }
    درونخطی int32_t contains(T toCheck) ثابت { بازگشت get(toCheck); }
    درونخطی تهی set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    درونخطی int32_t get(T toCheck) ثابت { بازگشت (fBools & flag(toCheck))?1:0; }
    درونخطی UBool isValidEnum(T toCheck) ثابت {  بازگشت (toCheck>=min_مقدار&&toCheck<limit_مقدار); }
    درونخطی UBool is_Valid_مقدار(int32_t v) ثابت { بازگشت (v==0||v==1); }
    درونخطی ثابت EnumSet<T, min_مقدار, limit_مقدار>&
    operator=(ثابت EnumSet<T, min_مقدار, limit_مقدار>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        بازگشت *this;
    }
  
    درونخطی uint32_t getAll() ثابت {
        بازگشت fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

خصوصی:
    درونخطی uint32_t flag(T toCheck) ثابت { بازگشت (1<<(toCheck-min_مقدار)); }
خصوصی:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

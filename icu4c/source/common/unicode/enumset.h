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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
template<typename T, uint32_t min_മൂല്യം, uint32_t limit_മൂല്യം>
class EnumSet {
പൊതു:
    വരിയിൽ EnumSet() : fBools(0) {}
    വരിയിൽ EnumSet(സ്ഥിരം EnumSet<T,min_മൂല്യം,limit_മൂല്യം>& other) : fBools(other.fBools) {}
    വരിയിൽ ~EnumSet() {}
#ifndef U_HIDE_INTERNAL_API
    വരിയിൽ ശൂന്യം clear() { fBools=0; }
    വരിയിൽ ശൂന്യം add(T toAdd) { set(toAdd, 1); }
    വരിയിൽ ശൂന്യം remove(T toRemove) { set(toRemove, 0); }
    വരിയിൽ int32_t contains(T toCheck) സ്ഥിരം { തിരികെ get(toCheck); }
    വരിയിൽ ശൂന്യം set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    വരിയിൽ int32_t get(T toCheck) സ്ഥിരം { തിരികെ (fBools & flag(toCheck))?1:0; }
    വരിയിൽ UBool isValidEnum(T toCheck) സ്ഥിരം {  തിരികെ (toCheck>=min_മൂല്യം&&toCheck<limit_മൂല്യം); }
    വരിയിൽ UBool is_Valid_മൂല്യം(int32_t v) സ്ഥിരം { തിരികെ (v==0||v==1); }
    വരിയിൽ സ്ഥിരം EnumSet<T, min_മൂല്യം, limit_മൂല്യം>&
    operator=(സ്ഥിരം EnumSet<T, min_മൂല്യം, limit_മൂല്യം>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        തിരികെ *this;
    }
  
    വരിയിൽ uint32_t getAll() സ്ഥിരം {
        തിരികെ fBools; 
    }
#endif /* U_HIDE_INTERNAL_API */

സ്വകാര്യ:
    വരിയിൽ uint32_t flag(T toCheck) സ്ഥിരം { തിരികെ (1<<(toCheck-min_മൂല്യം)); }
സ്വകാര്യ:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */
#endif /* ENUMSET_H */

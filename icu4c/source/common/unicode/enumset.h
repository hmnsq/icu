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

#अगर_अघोषित ENUMSET_H
#घोषणा ENUMSET_H

#समावेश "unicode/utypes.h"

#अगर U_SHOW_CPLUSPLUS_API

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
खुला:
    अंतरभूत EnumSet() : fBools(0) {}
    अंतरभूत EnumSet(स्थिर EnumSet<T,min_मूल्य,limit_मूल्य>& other) : fBools(other.fBools) {}
    अंतरभूत ~EnumSet() {}
#अगर_अघोषित U_HIDE_INTERNAL_API
    अंतरभूत व्योम clear() { fBools=0; }
    अंतरभूत व्योम add(T toAdd) { set(toAdd, 1); }
    अंतरभूत व्योम हटाओ(T toRemove) { set(toRemove, 0); }
    अंतरभूत int32_t contains(T toCheck) स्थिर { वापस get(toCheck); }
    अंतरभूत व्योम set(T toSet, int32_t v) { fBools=(fBools&(~flag(toSet)))|(v?(flag(toSet)):0); }
    अंतरभूत int32_t get(T toCheck) स्थिर { वापस (fBools & flag(toCheck))?1:0; }
    अंतरभूत UBool isValidEnum(T toCheck) स्थिर {  वापस (toCheck>=min_मूल्य&&toCheck<limit_मूल्य); }
    अंतरभूत UBool is_Valid_मूल्य(int32_t v) स्थिर { वापस (v==0||v==1); }
    अंतरभूत स्थिर EnumSet<T, min_मूल्य, limit_मूल्य>&
    चालक=(स्थिर EnumSet<T, min_मूल्य, limit_मूल्य>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        वापस *this;
    }
  
    अंतरभूत uint32_t getAll() स्थिर {
        वापस fBools; 
    }
#पूर्ण_अगर /* U_HIDE_INTERNAL_API */

निजी:
    अंतरभूत uint32_t flag(T toCheck) स्थिर { वापस (1<<(toCheck-min_मूल्य)); }
निजी:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_END

#पूर्ण_अगर /* U_SHOW_CPLUSPLUS_API */
#पूर्ण_अगर /* ENUMSET_H */

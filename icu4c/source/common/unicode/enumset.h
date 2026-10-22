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

#جيڪڏهن_وصف_نه_ٿيل ENUMSET_H
#وصف_ڪريو ENUMSET_H

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
سانچو<typename T, uint32_t min_قدر, uint32_t حد_بندي_قدر>
class Enum_سيٽ {
عام:
    مضمن Enum_سيٽ() : fBools(0) {}
    مضمن Enum_سيٽ(ثابت Enum_سيٽ<T,min_قدر,حد_بندي_قدر>& other) : fBools(other.fBools) {}
    مضمن ~Enum_سيٽ() {}
#جيڪڏهن_وصف_نه_ٿيل U_HIDE_اندروني_API
    مضمن فراغ clear() { fBools=0; }
    مضمن فراغ جوڙيو(T منزل_جوڙيو) { سيٽ(منزل_جوڙيو, 1); }
    مضمن فراغ remove(T منزل_Remove) { سيٽ(منزل_Remove, 0); }
    مضمن int32_t شامل_آهي(T منزل_Check) ثابت { أرجع get(منزل_Check); }
    مضمن فراغ سيٽ(T منزل_سيٽ, int32_t v) { fBools=(fBools&(~flag(منزل_سيٽ)))|(v?(flag(منزل_سيٽ)):0); }
    مضمن int32_t get(T منزل_Check) ثابت { أرجع (fBools & flag(منزل_Check))?1:0; }
    مضمن UBool isValidEnum(T منزل_Check) ثابت {  أرجع (منزل_Check>=min_قدر&&منزل_Check<حد_بندي_قدر); }
    مضمن UBool is_Valid_قدر(int32_t v) ثابت { أرجع (v==0||v==1); }
    مضمن ثابت Enum_سيٽ<T, min_قدر, حد_بندي_قدر>&
    عامل=(ثابت Enum_سيٽ<T, min_قدر, حد_بندي_قدر>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        أرجع *this;
    }
  
    مضمن uint32_t getAll() ثابت {
        أرجع fBools; 
    }
#جيڪڏهن_پورو /* U_HIDE_INTERNAL_API */

خاص:
    مضمن uint32_t flag(T منزل_Check) ثابت { أرجع (1<<(منزل_Check-min_قدر)); }
خاص:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* U_SHOW_CPLUSPLUS_API */
#جيڪڏهن_پورو /* ENUMSET_H */

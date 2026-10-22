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

#که_نه_تعریف_شوی ENUMSET_H
#تعریف_کړه ENUMSET_H

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
کينډۍ<typename T, uint32_t min_ارزښت, uint32_t حد_ارزښت>
class Enum_سېټ {
عام:
    مضمن Enum_سېټ() : fBools(0) {}
    مضمن Enum_سېټ(ثابت Enum_سېټ<T,min_ارزښت,حد_ارزښت>& other) : fBools(other.fBools) {}
    مضمن ~Enum_سېټ() {}
#که_نه_تعریف_شوی U_HIDE_دننی_API
    مضمن فراغ clear() { fBools=0; }
    مضمن فراغ جمع_کړه(T منزل_جمع_کړه) { سېټ(منزل_جمع_کړه, 1); }
    مضمن فراغ remove(T منزل_Remove) { سېټ(منزل_Remove, 0); }
    مضمن int32_t لري(T منزل_Check) ثابت { أرجع get(منزل_Check); }
    مضمن فراغ سېټ(T منزل_سېټ, int32_t v) { fBools=(fBools&(~flag(منزل_سېټ)))|(v?(flag(منزل_سېټ)):0); }
    مضمن int32_t get(T منزل_Check) ثابت { أرجع (fBools & flag(منزل_Check))?1:0; }
    مضمن UBool isValidEnum(T منزل_Check) ثابت {  أرجع (منزل_Check>=min_ارزښت&&منزل_Check<حد_ارزښت); }
    مضمن UBool is_Valid_ارزښت(int32_t v) ثابت { أرجع (v==0||v==1); }
    مضمن ثابت Enum_سېټ<T, min_ارزښت, حد_ارزښت>&
    چلوونکی=(ثابت Enum_سېټ<T, min_ارزښت, حد_ارزښت>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        أرجع *this;
    }
  
    مضمن uint32_t getAll() ثابت {
        أرجع fBools; 
    }
#د_که_پای /* U_HIDE_INTERNAL_API */

خاص:
    مضمن uint32_t flag(T منزل_Check) ثابت { أرجع (1<<(منزل_Check-min_ارزښت)); }
خاص:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_پای_ټکی

#د_که_پای /* U_SHOW_CPLUSPLUS_API */
#د_که_پای /* ENUMSET_H */

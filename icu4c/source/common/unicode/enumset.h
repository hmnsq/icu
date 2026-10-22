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

#جے_بنا_تعریف ENUMSET_H
#تعریف_کرو ENUMSET_H

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
سانچا<typename T, uint32_t min_قدر, uint32_t حد_بندی_قدر>
class Enum_سیٹ {
عام:
    مضمن Enum_سیٹ() : fBools(0) {}
    مضمن Enum_سیٹ(ثابت Enum_سیٹ<T,min_قدر,حد_بندی_قدر>& other) : fBools(other.fBools) {}
    مضمن ~Enum_سیٹ() {}
#جے_بنا_تعریف U_HIDE_اندرونی_API
    مضمن فراغ clear() { fBools=0; }
    مضمن فراغ جوڑو(T منزل_جوڑو) { سیٹ(منزل_جوڑو, 1); }
    مضمن فراغ remove(T منزل_Remove) { سیٹ(منزل_Remove, 0); }
    مضمن int32_t شامل_اے(T منزل_Check) ثابت { أرجع get(منزل_Check); }
    مضمن فراغ سیٹ(T منزل_سیٹ, int32_t v) { fBools=(fBools&(~flag(منزل_سیٹ)))|(v?(flag(منزل_سیٹ)):0); }
    مضمن int32_t get(T منزل_Check) ثابت { أرجع (fBools & flag(منزل_Check))?1:0; }
    مضمن UBool isValidEnum(T منزل_Check) ثابت {  أرجع (منزل_Check>=min_قدر&&منزل_Check<حد_بندی_قدر); }
    مضمن UBool is_Valid_قدر(int32_t v) ثابت { أرجع (v==0||v==1); }
    مضمن ثابت Enum_سیٹ<T, min_قدر, حد_بندی_قدر>&
    عامل=(ثابت Enum_سیٹ<T, min_قدر, حد_بندی_قدر>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        أرجع *this;
    }
  
    مضمن uint32_t getAll() ثابت {
        أرجع fBools; 
    }
#جے_انت /* U_HIDE_INTERNAL_API */

خاص:
    مضمن uint32_t flag(T منزل_Check) ثابت { أرجع (1<<(منزل_Check-min_قدر)); }
خاص:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_سرا

#جے_انت /* U_SHOW_CPLUSPLUS_API */
#جے_انت /* ENUMSET_H */

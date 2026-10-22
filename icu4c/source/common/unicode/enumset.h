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

#اگر_غیر_تعریف_شدہ ENUMSET_H
#تعریف_کرو ENUMSET_H

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
سانچہ<typename T, uint32_t min_قدر, uint32_t حد_بندی_قدر>
class Enum_سیٹ {
عوامی:
    اندرونی Enum_سیٹ() : fBools(0) {}
    اندرونی Enum_سیٹ(ثابت Enum_سیٹ<T,min_قدر,حد_بندی_قدر>& other) : fBools(other.fBools) {}
    اندرونی ~Enum_سیٹ() {}
#اگر_غیر_تعریف_شدہ U_HIDE_اندرونی_API
    اندرونی خالی clear() { fBools=0; }
    اندرونی خالی جوڑو(T مقصد_جوڑو) { سیٹ(مقصد_جوڑو, 1); }
    اندرونی خالی remove(T مقصد_Remove) { سیٹ(مقصد_Remove, 0); }
    اندرونی int32_t شامل_ہے(T مقصد_Check) ثابت { واپس get(مقصد_Check); }
    اندرونی خالی سیٹ(T مقصد_سیٹ, int32_t v) { fBools=(fBools&(~flag(مقصد_سیٹ)))|(v?(flag(مقصد_سیٹ)):0); }
    اندرونی int32_t get(T مقصد_Check) ثابت { واپس (fBools & flag(مقصد_Check))?1:0; }
    اندرونی UBool isValidEnum(T مقصد_Check) ثابت {  واپس (مقصد_Check>=min_قدر&&مقصد_Check<حد_بندی_قدر); }
    اندرونی UBool is_Valid_قدر(int32_t v) ثابت { واپس (v==0||v==1); }
    اندرونی ثابت Enum_سیٹ<T, min_قدر, حد_بندی_قدر>&
    عامل=(ثابت Enum_سیٹ<T, min_قدر, حد_بندی_قدر>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        واپس *this;
    }
  
    اندرونی uint32_t getAll() ثابت {
        واپس fBools; 
    }
#اگر_ختم /* U_HIDE_INTERNAL_API */

نجی:
    اندرونی uint32_t flag(T مقصد_Check) ثابت { واپس (1<<(مقصد_Check-min_قدر)); }
نجی:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_انتہا

#اگر_ختم /* U_SHOW_CPLUSPLUS_API */
#اگر_ختم /* ENUMSET_H */

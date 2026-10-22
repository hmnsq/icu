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

#اگر_تعریف_نشده ENUMSET_H
#تعریف_کن ENUMSET_H

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
قالب<typename T, uint32_t min_مقدار, uint32_t حد_مقدار>
class Enum_مجموعه {
عام:
    مضمن Enum_مجموعه() : fBools(0) {}
    مضمن Enum_مجموعه(ثابت Enum_مجموعه<T,min_مقدار,حد_مقدار>& other) : fBools(other.fBools) {}
    مضمن ~Enum_مجموعه() {}
#اگر_تعریف_نشده U_HIDE_داخلی_API
    مضمن فراغ clear() { fBools=0; }
    مضمن فراغ جمع_کن(T مقصد_جمع_کن) { مجموعه(مقصد_جمع_کن, 1); }
    مضمن فراغ remove(T مقصد_Remove) { مجموعه(مقصد_Remove, 0); }
    مضمن int32_t شامل_است(T مقصد_Check) ثابت { أرجع get(مقصد_Check); }
    مضمن فراغ مجموعه(T مقصد_مجموعه, int32_t v) { fBools=(fBools&(~flag(مقصد_مجموعه)))|(v?(flag(مقصد_مجموعه)):0); }
    مضمن int32_t get(T مقصد_Check) ثابت { أرجع (fBools & flag(مقصد_Check))?1:0; }
    مضمن UBool isValidEnum(T مقصد_Check) ثابت {  أرجع (مقصد_Check>=min_مقدار&&مقصد_Check<حد_مقدار); }
    مضمن UBool is_Valid_مقدار(int32_t v) ثابت { أرجع (v==0||v==1); }
    مضمن ثابت Enum_مجموعه<T, min_مقدار, حد_مقدار>&
    عملگر=(ثابت Enum_مجموعه<T, min_مقدار, حد_مقدار>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        أرجع *this;
    }
  
    مضمن uint32_t getAll() ثابت {
        أرجع fBools; 
    }
#پایان_اگر /* U_HIDE_INTERNAL_API */

خاص:
    مضمن uint32_t flag(T مقصد_Check) ثابت { أرجع (1<<(مقصد_Check-min_مقدار)); }
خاص:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_انتها

#پایان_اگر /* U_SHOW_CPLUSPLUS_API */
#پایان_اگر /* ENUMSET_H */

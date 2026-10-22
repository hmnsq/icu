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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ENUMSET_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ENUMSET_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
ਸਾਂਚਾ<typename T, uint32_t min_ਮੁੱਲ, uint32_t ਹੱਦਬੰਦੀ_ਮੁੱਲ>
class Enum_ਸੈੱਟ {
ਜਨਤਕ:
    ਸਤਰਅੰਦਰ Enum_ਸੈੱਟ() : fBools(0) {}
    ਸਤਰਅੰਦਰ Enum_ਸੈੱਟ(ਅਚਲ Enum_ਸੈੱਟ<T,min_ਮੁੱਲ,ਹੱਦਬੰਦੀ_ਮੁੱਲ>& other) : fBools(other.fBools) {}
    ਸਤਰਅੰਦਰ ~Enum_ਸੈੱਟ() {}
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_HIDE_ਅੰਦਰੂਨੀ_API
    ਸਤਰਅੰਦਰ ਖਾਲੀ clear() { fBools=0; }
    ਸਤਰਅੰਦਰ ਖਾਲੀ ਜੋੜੋ(T ਮੰਜ਼ਿਲ_ਜੋੜੋ) { ਸੈੱਟ(ਮੰਜ਼ਿਲ_ਜੋੜੋ, 1); }
    ਸਤਰਅੰਦਰ ਖਾਲੀ remove(T ਮੰਜ਼ਿਲ_Remove) { ਸੈੱਟ(ਮੰਜ਼ਿਲ_Remove, 0); }
    ਸਤਰਅੰਦਰ int32_t ਸ਼ਾਮਲ_ਹੈ(T ਮੰਜ਼ਿਲ_Check) ਅਚਲ { ਵਾਪਸ get(ਮੰਜ਼ਿਲ_Check); }
    ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੈੱਟ(T ਮੰਜ਼ਿਲ_ਸੈੱਟ, int32_t v) { fBools=(fBools&(~flag(ਮੰਜ਼ਿਲ_ਸੈੱਟ)))|(v?(flag(ਮੰਜ਼ਿਲ_ਸੈੱਟ)):0); }
    ਸਤਰਅੰਦਰ int32_t get(T ਮੰਜ਼ਿਲ_Check) ਅਚਲ { ਵਾਪਸ (fBools & flag(ਮੰਜ਼ਿਲ_Check))?1:0; }
    ਸਤਰਅੰਦਰ UBool isValidEnum(T ਮੰਜ਼ਿਲ_Check) ਅਚਲ {  ਵਾਪਸ (ਮੰਜ਼ਿਲ_Check>=min_ਮੁੱਲ&&ਮੰਜ਼ਿਲ_Check<ਹੱਦਬੰਦੀ_ਮੁੱਲ); }
    ਸਤਰਅੰਦਰ UBool is_Valid_ਮੁੱਲ(int32_t v) ਅਚਲ { ਵਾਪਸ (v==0||v==1); }
    ਸਤਰਅੰਦਰ ਅਚਲ Enum_ਸੈੱਟ<T, min_ਮੁੱਲ, ਹੱਦਬੰਦੀ_ਮੁੱਲ>&
    ਸੰਚਾਲਕ=(ਅਚਲ Enum_ਸੈੱਟ<T, min_ਮੁੱਲ, ਹੱਦਬੰਦੀ_ਮੁੱਲ>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        ਵਾਪਸ *this;
    }
  
    ਸਤਰਅੰਦਰ uint32_t getAll() ਅਚਲ {
        ਵਾਪਸ fBools; 
    }
#ਜੇ_ਅੰਤ /* U_HIDE_INTERNAL_API */

ਨਿਜੀ:
    ਸਤਰਅੰਦਰ uint32_t flag(T ਮੰਜ਼ਿਲ_Check) ਅਚਲ { ਵਾਪਸ (1<<(ਮੰਜ਼ਿਲ_Check-min_ਮੁੱਲ)); }
ਨਿਜੀ:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* U_SHOW_CPLUSPLUS_API */
#ਜੇ_ਅੰਤ /* ENUMSET_H */

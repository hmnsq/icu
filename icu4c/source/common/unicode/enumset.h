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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ENUMSET_H
#ವ್ಯಾಖ್ಯಾನಿಸು ENUMSET_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
ಮಾದರಿ<typename T, uint32_t min_ಮೌಲ್ಯ, uint32_t ಎಲ್ಲೆ_ಮೌಲ್ಯ>
class Enum_ಗಣ {
ಸಾರ್ವಜನಿಕ:
    ಸಾಲೊಳಗೆ Enum_ಗಣ() : fBools(0) {}
    ಸಾಲೊಳಗೆ Enum_ಗಣ(ಸ್ಥಿರಾಂಕ Enum_ಗಣ<T,min_ಮೌಲ್ಯ,ಎಲ್ಲೆ_ಮೌಲ್ಯ>& other) : fBools(other.fBools) {}
    ಸಾಲೊಳಗೆ ~Enum_ಗಣ() {}
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
    ಸಾಲೊಳಗೆ ಶೂನ್ಯ clear() { fBools=0; }
    ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಕೂಡಿಸು(T ಗಮ್ಯ_ಕೂಡಿಸು) { ಗಣ(ಗಮ್ಯ_ಕೂಡಿಸು, 1); }
    ಸಾಲೊಳಗೆ ಶೂನ್ಯ remove(T ಗಮ್ಯ_Remove) { ಗಣ(ಗಮ್ಯ_Remove, 0); }
    ಸಾಲೊಳಗೆ int32_t ಒಳಗೊಂಡಿದೆ(T ಗಮ್ಯ_Check) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ get(ಗಮ್ಯ_Check); }
    ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಗಣ(T ಗಮ್ಯ_ಗಣ, int32_t v) { fBools=(fBools&(~flag(ಗಮ್ಯ_ಗಣ)))|(v?(flag(ಗಮ್ಯ_ಗಣ)):0); }
    ಸಾಲೊಳಗೆ int32_t get(T ಗಮ್ಯ_Check) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (fBools & flag(ಗಮ್ಯ_Check))?1:0; }
    ಸಾಲೊಳಗೆ UBool isValidEnum(T ಗಮ್ಯ_Check) ಸ್ಥಿರಾಂಕ {  ಹಿಂತಿರುಗಿ (ಗಮ್ಯ_Check>=min_ಮೌಲ್ಯ&&ಗಮ್ಯ_Check<ಎಲ್ಲೆ_ಮೌಲ್ಯ); }
    ಸಾಲೊಳಗೆ UBool is_Valid_ಮೌಲ್ಯ(int32_t v) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (v==0||v==1); }
    ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ Enum_ಗಣ<T, min_ಮೌಲ್ಯ, ಎಲ್ಲೆ_ಮೌಲ್ಯ>&
    ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Enum_ಗಣ<T, min_ಮೌಲ್ಯ, ಎಲ್ಲೆ_ಮೌಲ್ಯ>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        ಹಿಂತಿರುಗಿ *this;
    }
  
    ಸಾಲೊಳಗೆ uint32_t getAll() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ fBools; 
    }
#ಆದರೆ_ಅಂತ್ಯ /* U_HIDE_INTERNAL_API */

ಖಾಸಗಿ:
    ಸಾಲೊಳಗೆ uint32_t flag(T ಗಮ್ಯ_Check) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (1<<(ಗಮ್ಯ_Check-min_ಮೌಲ್ಯ)); }
ಖಾಸಗಿ:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */
#ಆದರೆ_ಅಂತ್ಯ /* ENUMSET_H */

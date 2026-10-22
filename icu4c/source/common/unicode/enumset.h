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

#यदि_अपरिभाषितम् ENUMSET_H
#परिभाषय ENUMSET_H

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
प्रतिरूपम्<typename T, uint32_t min_मानम्, uint32_t सीमा_मानम्>
class Enum_समुच्चयः {
सार्वजनिक:
    अन्तःस्थ Enum_समुच्चयः() : fBools(0) {}
    अन्तःस्थ Enum_समुच्चयः(ध्रुव Enum_समुच्चयः<T,min_मानम्,सीमा_मानम्>& other) : fBools(other.fBools) {}
    अन्तःस्थ ~Enum_समुच्चयः() {}
#यदि_अपरिभाषितम् U_HIDE_आन्तरिकम्_API
    अन्तःस्थ शून्य clear() { fBools=0; }
    अन्तःस्थ शून्य सङ्कलय(T गन्तव्यम्_सङ्कलय) { समुच्चयः(गन्तव्यम्_सङ्कलय, 1); }
    अन्तःस्थ शून्य remove(T गन्तव्यम्_Remove) { समुच्चयः(गन्तव्यम्_Remove, 0); }
    अन्तःस्थ int32_t अन्तर्भवति(T गन्तव्यम्_Check) ध्रुव { प्रत्यागम get(गन्तव्यम्_Check); }
    अन्तःस्थ शून्य समुच्चयः(T गन्तव्यम्_समुच्चयः, int32_t v) { fBools=(fBools&(~flag(गन्तव्यम्_समुच्चयः)))|(v?(flag(गन्तव्यम्_समुच्चयः)):0); }
    अन्तःस्थ int32_t get(T गन्तव्यम्_Check) ध्रुव { प्रत्यागम (fBools & flag(गन्तव्यम्_Check))?1:0; }
    अन्तःस्थ UBool isValidEnum(T गन्तव्यम्_Check) ध्रुव {  प्रत्यागम (गन्तव्यम्_Check>=min_मानम्&&गन्तव्यम्_Check<सीमा_मानम्); }
    अन्तःस्थ UBool is_Valid_मानम्(int32_t v) ध्रुव { प्रत्यागम (v==0||v==1); }
    अन्तःस्थ ध्रुव Enum_समुच्चयः<T, min_मानम्, सीमा_मानम्>&
    संक्रियकः=(ध्रुव Enum_समुच्चयः<T, min_मानम्, सीमा_मानम्>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        प्रत्यागम *this;
    }
  
    अन्तःस्थ uint32_t getAll() ध्रुव {
        प्रत्यागम fBools; 
    }
#यदि_अन्तः /* U_HIDE_INTERNAL_API */

गुप्त:
    अन्तःस्थ uint32_t flag(T गन्तव्यम्_Check) ध्रुव { प्रत्यागम (1<<(गन्तव्यम्_Check-min_मानम्)); }
गुप्त:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_अन्तः

#यदि_अन्तः /* U_SHOW_CPLUSPLUS_API */
#यदि_अन्तः /* ENUMSET_H */

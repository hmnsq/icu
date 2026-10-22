// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uset.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002mar07
*   created by: Markus W. Scherer
*
*   There are functions to efficiently serialize a USet into an array of uint16_t
*   and functions to use such a serialized form efficiently without
*   instantiating a new USet.
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/char16ptr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_ಖಾಲಿಗಳು() {
    ಹಿಂತಿರುಗಿ (USet*) ಹೊಸ Unicode_ಗಣ();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    ಹಿಂತಿರುಗಿ (USet*) ಹೊಸ Unicode_ಗಣ(ಆರಂಭ, ಕೊನೆ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_close(USet* ಗಣ) {
    ಅಳಿಸು (Unicode_ಗಣ*) ಗಣ;
}

U_CAPI USet * U_EXPORT2
uset_clone(ಸ್ಥಿರಾಂಕ USet *ಗಣ) {
    ಹಿಂತಿರುಗಿ (USet*) (((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ಸ್ಥಿರಾಂಕ USet *ಗಣ) {
    ಹಿಂತಿರುಗಿ ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::isFrozen();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_freeze(USet *ಗಣ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ಸ್ಥಿರಾಂಕ USet *ಗಣ) {
    ಹಿಂತಿರುಗಿ (USet*) (((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::cloneAsThawed());
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_ಗಣ(USet* ಗಣ,
     UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಗಣ(ಆರಂಭ, ಕೊನೆ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_ಕೂಡಿಸು_All(USet* ಗಣ, ಸ್ಥಿರಾಂಕ USet *additional_ಗಣ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಕೂಡಿಸು_All(*((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*)additional_ಗಣ));
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_ಕೂಡಿಸು(USet* ಗಣ, UChar32 c) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಕೂಡಿಸು(c);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_ಕೂಡಿಸು_Range(USet* ಗಣ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಕೂಡಿಸು(ಆರಂಭ, ಕೊನೆ);    
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_ಕೂಡಿಸು_String(USet* ಗಣ, ಸ್ಥಿರಾಂಕ char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಕೂಡಿಸು(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_ಕೂಡಿಸು_All_ಸಂಕೇತ_Points(USet* ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಕೂಡಿಸು_All(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_remove(USet* ಗಣ, UChar32 c) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::remove(c);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_removeRange(USet* ಗಣ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::remove(ಆರಂಭ, ಕೊನೆ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_removeString(USet* ಗಣ, ಸ್ಥಿರಾಂಕ char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::remove(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_remove_All_ಸಂಕೇತ_Points(USet *ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::removeAll(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_removeAll(USet* ಗಣ, ಸ್ಥಿರಾಂಕ USet* remove) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::removeAll(*(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*)remove);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_retain(USet* ಗಣ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::retain(ಆರಂಭ, ಕೊನೆ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_retainString(USet *ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::retain(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_retain_All_ಸಂಕೇತ_Points(USet *ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::retainAll(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_retainAll(USet* ಗಣ, ಸ್ಥಿರಾಂಕ USet* retain) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::retainAll(*(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*)retain);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_compact(USet* ಗಣ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::compact();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complement(USet* ಗಣ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::complement();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complementRange(USet *ಗಣ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::complement(ಆರಂಭ, ಕೊನೆ);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complementString(USet *ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::complement(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complement_All_ಸಂಕೇತ_Points(USet *ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::complementAll(s);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_complementAll(USet* ಗಣ, ಸ್ಥಿರಾಂಕ USet* complement) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::complementAll(*(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*)complement);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_clear(USet* ಗಣ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::clear();
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_remove_All_ಅಕ್ಷರಸರಣಿಗಳು(USet* ಗಣ) {
    ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::remove_All_ಅಕ್ಷರಸರಣಿಗಳು();
}

U_CAPI UBool U_EXPORT2
uset_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ USet* ಗಣ) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::is_ಖಾಲಿಗಳು();
}

U_CAPI UBool U_EXPORT2
uset_has_ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿರಾಂಕ USet* ಗಣ) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::has_ಅಕ್ಷರಸರಣಿಗಳು();
}

U_CAPI UBool U_EXPORT2
uset_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ USet* ಗಣ, UChar32 c) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ(c);
}

U_CAPI UBool U_EXPORT2
uset_ಒಳಗೊಂಡಿದೆ_Range(ಸ್ಥಿರಾಂಕ USet* ಗಣ, UChar32 ಆರಂಭ, UChar32 ಕೊನೆ) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ(ಆರಂಭ, ಕೊನೆ);
}

U_CAPI UBool U_EXPORT2
uset_ಒಳಗೊಂಡಿದೆ_String(ಸ್ಥಿರಾಂಕ USet* ಗಣ, ಸ್ಥಿರಾಂಕ char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ(s);
}

U_CAPI UBool U_EXPORT2
uset_ಒಳಗೊಂಡಿದೆ_All(ಸ್ಥಿರಾಂಕ USet* set1, ಸ್ಥಿರಾಂಕ USet* set2) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) set1)->Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ_All(* (ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_ಒಳಗೊಂಡಿದೆ_All_ಸಂಕೇತ_Points(ಸ್ಥಿರಾಂಕ USet* ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ_All(s);
}

U_CAPI UBool U_EXPORT2
uset_ಒಳಗೊಂಡಿದೆ_None(ಸ್ಥಿರಾಂಕ USet* set1, ಸ್ಥಿರಾಂಕ USet* set2) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) set1)->Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ_None(* (ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_ಒಳಗೊಂಡಿದೆ_Some(ಸ್ಥಿರಾಂಕ USet* set1, ಸ್ಥಿರಾಂಕ USet* set2) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) set1)->Unicode_ಗಣ::ಒಳಗೊಂಡಿದೆ_Some(* (ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_ಹರವು(ಸ್ಥಿರಾಂಕ USet *ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) {
    ಹಿಂತಿರುಗಿ ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಹರವು(s, length, ಹರವು_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_ಹರವು_ಹಿಂಭಾಗ(ಸ್ಥಿರಾಂಕ USet *ಗಣ, ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) {
    ಹಿಂತಿರುಗಿ ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಹರವು_ಹಿಂಭಾಗ(s, length, ಹರವು_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_ಹರವು_UTF8(ಸ್ಥಿರಾಂಕ USet *ಗಣ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) {
    ಹಿಂತಿರುಗಿ ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಹರವು_UTF8(s, length, ಹರವು_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_ಹರವು_ಹಿಂಭಾಗ_UTF8(ಸ್ಥಿರಾಂಕ USet *ಗಣ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t length, USet_ಹರವು_Condition ಹರವು_Condition) {
    ಹಿಂತಿರುಗಿ ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಹರವು_ಹಿಂಭಾಗ_UTF8(s, length, ಹರವು_Condition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ಸ್ಥಿರಾಂಕ USet* set1, ಸ್ಥಿರಾಂಕ USet* set2) {
    ಹಿಂತಿರುಗಿ *(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*)set1 == *(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_ಸೂಚ್ಯಂಕ_ದ(ಸ್ಥಿರಾಂಕ USet* ಗಣ, UChar32 c) {
    ಹಿಂತಿರುಗಿ ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::ಸೂಚ್ಯಂಕ_ದ(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ಸ್ಥಿರಾಂಕ USet* ಗಣ, int32_t ಸೂಚ್ಯಂಕ) {
    ಹಿಂತಿರುಗಿ ((Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::charAt(ಸೂಚ್ಯಂಕ);
}

U_CAPI int32_t U_EXPORT2
uset_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ USet* ಗಣ) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
ವರ್ಗ USetAccess /* not : public UObject because all methods are static */ {
ಸಾರ್ವಜನಿಕ:
    /* Try to have the compiler inline these*/
    ಸಾಲೊಳಗೆ ಸ್ಥಿರ int32_t get_String_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& ಗಣ) {
        ಹಿಂತಿರುಗಿ ಗಣ.ಅಕ್ಷರಸರಣಿಗಳು_ಗಾತ್ರ();
    }
    ಸಾಲೊಳಗೆ ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UnicodeString* getString(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& ಗಣ U_LIFETIME_BOUND,
                                                 int32_t i) {
        ಹಿಂತಿರುಗಿ ಗಣ.getString(i);
    }
ಖಾಸಗಿ:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_ಕೊನೆ

U_CAPI int32_t U_EXPORT2
uset_get_Range_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ USet *ಗಣ) {
    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *)ಗಣ)->Unicode_ಗಣ::get_Range_ಎಣಿಕೆ();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ USet *uset) {
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಗಣ = *(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *)uset;
    ಹಿಂತಿರುಗಿ USetAccess::get_String_ಎಣಿಕೆ(ಗಣ);
}

U_CAPI int32_t U_EXPORT2
uset_get_ಅಂಶ_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ USet* uset) {
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& ಗಣ = *(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*)uset;
    ಹಿಂತಿರುಗಿ ಗಣ.get_Range_ಎಣಿಕೆ() + USetAccess::get_String_ಎಣಿಕೆ(ಗಣ);
}

U_CAPI ಸ್ಥಿರಾಂಕ UChar* U_EXPORT2
uset_getString(ಸ್ಥಿರಾಂಕ USet* uset U_LIFETIME_BOUND, int32_t ಸೂಚ್ಯಂಕ, int32_t* p_ಉದ್ದಳತೆ) {
    ಆದರೆ (p_ಉದ್ದಳತೆ == ಶೂನ್ಯಸೂಚಕ) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಗಣ = *(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *)uset;
    int32_t count = USetAccess::get_String_ಎಣಿಕೆ(ಗಣ);
    ಆದರೆ (ಸೂಚ್ಯಂಕ < 0 || count <= ಸೂಚ್ಯಂಕ) {
        *p_ಉದ್ದಳತೆ = 0;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ UnicodeString *s = USetAccess::getString(ಗಣ, ಸೂಚ್ಯಂಕ);
    *p_ಉದ್ದಳತೆ = s->length();
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_UChar_Ptr(s->get_ಬಫರ್());
}

U_CAPI int32_t U_EXPORT2
uset_get_ಅಂಶ(ಸ್ಥಿರಾಂಕ USet* uset, int32_t ಅಂಶ_ಸೂಚ್ಯಂಕ,
             UChar32* ಆರಂಭ, UChar32* ಕೊನೆ,
             char16_t* str, int32_t str_ಸಾಮರ್ಥ್ಯ,
             UError_ಸಂಕೇತ* ec) {
    ಆದರೆ (U_FAILURE(*ec)) ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿರಾಂಕ Unicode_ಗಣ& ಗಣ = *(ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*)uset;
    int32_t range_ಎಣಿಕೆ;

    ಆದರೆ (ಅಂಶ_ಸೂಚ್ಯಂಕ < 0) {
        *ec = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ -1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಂಶ_ಸೂಚ್ಯಂಕ < (range_ಎಣಿಕೆ = ಗಣ.get_Range_ಎಣಿಕೆ())) {
        *ಆರಂಭ = ಗಣ.get_Range_ಆರಂಭ(ಅಂಶ_ಸೂಚ್ಯಂಕ);
        *ಕೊನೆ = ಗಣ.get_Range_ಕೊನೆ(ಅಂಶ_ಸೂಚ್ಯಂಕ);
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಅಂಶ_ಸೂಚ್ಯಂಕ -= range_ಎಣಿಕೆ;
        ಆದರೆ (ಅಂಶ_ಸೂಚ್ಯಂಕ < USetAccess::get_String_ಎಣಿಕೆ(ಗಣ)) {
            ಸ್ಥಿರಾಂಕ UnicodeString* s = USetAccess::getString(ಗಣ, ಅಂಶ_ಸೂಚ್ಯಂಕ);
            ಹಿಂತಿರುಗಿ s->extract(str, str_ಸಾಮರ್ಥ್ಯ, *ec);
        } ಇಲ್ಲದಿದ್ದರೆ {
            *ec = U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ -1;
        }
    }
}

//U_CAPI UBool U_EXPORT2
//uset_getRange(const USet* set, int32_t rangeIndex,
//              UChar32* pStart, UChar32* pEnd) {
//    if ((uint32_t) rangeIndex >= (uint32_t) uset_getRangeCount(set)) {
//        return false;
//    }
//    const UnicodeSet* us = (const UnicodeSet*) set;
//    *pStart = us->getRangeStart(rangeIndex);
//    *pEnd = us->getRangeEnd(rangeIndex);
//    return true;
//}

/*
 * Serialize a USet into 16-bit units.
 * Store BMP code points as themselves with one 16-bit unit each.
 *
 * Important: the code points in the array are in ascending order,
 * therefore all BMP code points precede all supplementary code points.
 *
 * Store each supplementary code point in 2 16-bit units,
 * simply with higher-then-lower 16-bit halves.
 *
 * Precede the entire list with the length.
 * If there are supplementary code points, then set bit 15 in the length
 * and add the bmpLength between it and the array.
 *
 * In other words:
 * - all BMP:            (length=bmpLength) BMP, .., BMP
 * - some supplementary: (length|0x8000) (bmpLength<length) BMP, .., BMP, supp-high, supp-low, ..
 */
U_CAPI int32_t U_EXPORT2
uset_serialize(ಸ್ಥಿರಾಂಕ USet* ಗಣ, uint16_t* dest, int32_t dest_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ* ec) {
    ಆದರೆ (ec==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ec)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Unicode_ಗಣ*) ಗಣ)->Unicode_ಗಣ::serialize(dest, dest_ಸಾಮರ್ಥ್ಯ,* ec);
}

U_CAPI UBool U_EXPORT2
uset_get_Serialized_ಗಣ(USerialized_ಗಣ* fill_ಗಣ, ಸ್ಥಿರಾಂಕ uint16_t* src, int32_t src_ಉದ್ದಳತೆ) {
    int32_t length;

    ಆದರೆ(fill_ಗಣ==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(src==ಶೂನ್ಯಸೂಚಕ || src_ಉದ್ದಳತೆ<=0) {
        fill_ಗಣ->length=fill_ಗಣ->bmp_ಉದ್ದಳತೆ=0;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    length=*src++;
    ಆದರೆ(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        ಆದರೆ(src_ಉದ್ದಳತೆ<(2+length)) {
            fill_ಗಣ->length=fill_ಗಣ->bmp_ಉದ್ದಳತೆ=0;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        fill_ಗಣ->bmp_ಉದ್ದಳತೆ=*src++;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* only BMP values */
        ಆದರೆ(src_ಉದ್ದಳತೆ<(1+length)) {
            fill_ಗಣ->length=fill_ಗಣ->bmp_ಉದ್ದಳತೆ=0;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        fill_ಗಣ->bmp_ಉದ್ದಳತೆ=length;
    }
    fill_ಗಣ->array=src;
    fill_ಗಣ->length=length;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uset_ಗಣ_Serialized_ಗಮ್ಯ_One(USerialized_ಗಣ* fill_ಗಣ, UChar32 c) {
    ಆದರೆ(fill_ಗಣ==ಶೂನ್ಯಸೂಚಕ || (uint32_t)c>0x10ffff) {
        ಹಿಂತಿರುಗಿ;
    }

    fill_ಗಣ->array=fill_ಗಣ->staticArray;
    ಆದರೆ(c<0xffff) {
        fill_ಗಣ->bmp_ಉದ್ದಳತೆ=fill_ಗಣ->length=2;
        fill_ಗಣ->staticArray[0]=(uint16_t)c;
        fill_ಗಣ->staticArray[1]=(uint16_t)c+1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xffff) {
        fill_ಗಣ->bmp_ಉದ್ದಳತೆ=1;
        fill_ಗಣ->length=3;
        fill_ಗಣ->staticArray[0]=0xffff;
        fill_ಗಣ->staticArray[1]=1;
        fill_ಗಣ->staticArray[2]=0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0x10ffff) {
        fill_ಗಣ->bmp_ಉದ್ದಳತೆ=0;
        fill_ಗಣ->length=4;
        fill_ಗಣ->staticArray[0]=(uint16_t)(c>>16);
        fill_ಗಣ->staticArray[1]=(uint16_t)c;
        ++c;
        fill_ಗಣ->staticArray[2]=(uint16_t)(c>>16);
        fill_ಗಣ->staticArray[3]=(uint16_t)c;
    } ಇಲ್ಲದಿದ್ದರೆ /* c==0x10ffff */ {
        fill_ಗಣ->bmp_ಉದ್ದಳತೆ=0;
        fill_ಗಣ->length=2;
        fill_ಗಣ->staticArray[0]=0x10;
        fill_ಗಣ->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serialized_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ USerialized_ಗಣ* ಗಣ, UChar32 c) {
    ಸ್ಥಿರಾಂಕ uint16_t* array;

    ಆದರೆ(ಗಣ==ಶೂನ್ಯಸೂಚಕ || (uint32_t)c>0x10ffff) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    array=ಗಣ->array;
    ಆದರೆ(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = ಗಣ->bmp_ಉದ್ದಳತೆ-1;
        ಆದರೆ (c < array[0]) {
            hi = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < array[hi]) {
            ಗಾಗಿ(;;) {
                int32_t i = (lo + hi) >> 1;
                ಆದರೆ (i == lo) {
                    ನಿಲ್ಲಿಸು;  // Done!
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c < array[i]) {
                    hi = i;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    lo = i;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            hi += 1;
        }
        ಹಿಂತಿರುಗಿ hi&1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* find c in the supplementary part */
        uint16_t ಮೇಲಿನ=(uint16_t)(c>>16), ಕೆಳಗಿನ=(uint16_t)c;
        int32_t ತಳ = ಗಣ->bmp_ಉದ್ದಳತೆ;
        int32_t lo = 0;
        int32_t hi = ಗಣ->length - 2 - ತಳ;
        ಆದರೆ (ಮೇಲಿನ < array[ತಳ] || (ಮೇಲಿನ==array[ತಳ] && ಕೆಳಗಿನ<array[ತಳ+1])) {
            hi = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೇಲಿನ < array[ತಳ+hi] || (ಮೇಲಿನ==array[ತಳ+hi] && ಕೆಳಗಿನ<array[ತಳ+hi+1])) {
            ಗಾಗಿ (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + ತಳ;
                ಆದರೆ (i == lo) {
                    ನಿಲ್ಲಿಸು;  // Done!
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮೇಲಿನ < array[iabs] || (ಮೇಲಿನ==array[iabs] && ಕೆಳಗಿನ<array[iabs+1])) {
                    hi = i;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    lo = i;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        ಹಿಂತಿರುಗಿ ((hi+(ತಳ<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ USerialized_ಗಣ* ಗಣ) {
    ಆದರೆ(ಗಣ==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಹಿಂತಿರುಗಿ (ಗಣ->bmp_ಉದ್ದಳತೆ+(ಗಣ->length-ಗಣ->bmp_ಉದ್ದಳತೆ)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ಸ್ಥಿರಾಂಕ USerialized_ಗಣ* ಗಣ, int32_t range_ಸೂಚ್ಯಂಕ,
                        UChar32* p_ಆರಂಭ, UChar32* p_ಕೊನೆ) {
    ಸ್ಥಿರಾಂಕ uint16_t* array;
    int32_t bmp_ಉದ್ದಳತೆ, length;

    ಆದರೆ(ಗಣ==ಶೂನ್ಯಸೂಚಕ || range_ಸೂಚ್ಯಂಕ<0 || p_ಆರಂಭ==ಶೂನ್ಯಸೂಚಕ || p_ಕೊನೆ==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    array=ಗಣ->array;
    length=ಗಣ->length;
    bmp_ಉದ್ದಳತೆ=ಗಣ->bmp_ಉದ್ದಳತೆ;

    range_ಸೂಚ್ಯಂಕ*=2; /* address start/limit pairs */
    ಆದರೆ(range_ಸೂಚ್ಯಂಕ<bmp_ಉದ್ದಳತೆ) {
        *p_ಆರಂಭ=array[range_ಸೂಚ್ಯಂಕ++];
        ಆದರೆ(range_ಸೂಚ್ಯಂಕ<bmp_ಉದ್ದಳತೆ) {
            *p_ಕೊನೆ=array[range_ಸೂಚ್ಯಂಕ]-1;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(range_ಸೂಚ್ಯಂಕ<length) {
            *p_ಕೊನೆ=((((int32_t)array[range_ಸೂಚ್ಯಂಕ])<<16)|array[range_ಸೂಚ್ಯಂಕ+1])-1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            *p_ಕೊನೆ=0x10ffff;
        }
        ಹಿಂತಿರುಗಿ ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        range_ಸೂಚ್ಯಂಕ-=bmp_ಉದ್ದಳತೆ;
        range_ಸೂಚ್ಯಂಕ*=2; /* address pairs of pairs of units */
        length-=bmp_ಉದ್ದಳತೆ;
        ಆದರೆ(range_ಸೂಚ್ಯಂಕ<length) {
            array+=bmp_ಉದ್ದಳತೆ;
            *p_ಆರಂಭ=(((int32_t)array[range_ಸೂಚ್ಯಂಕ])<<16)|array[range_ಸೂಚ್ಯಂಕ+1];
            range_ಸೂಚ್ಯಂಕ+=2;
            ಆದರೆ(range_ಸೂಚ್ಯಂಕ<length) {
                *p_ಕೊನೆ=((((int32_t)array[range_ಸೂಚ್ಯಂಕ])<<16)|array[range_ಸೂಚ್ಯಂಕ+1])-1;
            } ಇಲ್ಲದಿದ್ದರೆ {
                *p_ಕೊನೆ=0x10ffff;
            }
            ಹಿಂತಿರುಗಿ ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
}

// TODO The old, internal uset.c had an efficient uset_containsOne function.
// Returned the one and only code point, or else -1 or something.
// Consider adding such a function to both C and C++ UnicodeSet/uset.
// See tools/gennorm/store.c for usage, now usetContainsOne there.

// TODO Investigate incorporating this code into UnicodeSet to improve
// efficiency.
// ---
// #define USET_GROW_DELTA 20
// 
// static int32_t
// findChar(const UChar32* array, int32_t length, UChar32 c) {
//     int32_t i;
// 
//     /* check the last range limit first for more efficient appending */
//     if(length>0) {
//         if(c>=array[length-1]) {
//             return length;
//         }
// 
//         /* do not check the last range limit again in the loop below */
//         --length;
//     }
// 
//     for(i=0; i<length && c>=array[i]; ++i) {}
//     return i;
// }
// 
// static UBool
// addRemove(USet* set, UChar32 c, int32_t doRemove) {
//     int32_t i, length, more;
// 
//     if(set==nullptr || (uint32_t)c>0x10ffff) {
//         return false;
//     }
// 
//     length=set->length;
//     i=findChar(set->array, length, c);
//     if((i&1)^doRemove) {
//         /* c is already in the set */
//         return true;
//     }
// 
//     /* how many more array items do we need? */
//     if(i<length && (c+1)==set->array[i]) {
//         /* c is just before the following range, extend that in-place by one */
//         set->array[i]=c;
//         if(i>0) {
//             --i;
//             if(c==set->array[i]) {
//                 /* the previous range collapsed, remove it */
//                 set->length=length-=2;
//                 if(i<length) {
//                     uprv_memmove(set->array+i, set->array+i+2, (length-i)*4);
//                 }
//             }
//         }
//         return true;
//     } else if(i>0 && c==set->array[i-1]) {
//         /* c is just after the previous range, extend that in-place by one */
//         if(++c<=0x10ffff) {
//             set->array[i-1]=c;
//             if(i<length && c==set->array[i]) {
//                 /* the following range collapsed, remove it */
//                 --i;
//                 set->length=length-=2;
//                 if(i<length) {
//                     uprv_memmove(set->array+i, set->array+i+2, (length-i)*4);
//                 }
//             }
//         } else {
//             /* extend the previous range (had limit 0x10ffff) to the end of Unicode */
//             set->length=i-1;
//         }
//         return true;
//     } else if(i==length && c==0x10ffff) {
//         /* insert one range limit c */
//         more=1;
//     } else {
//         /* insert two range limits c, c+1 */
//         more=2;
//     }
// 
//     /* insert <more> range limits */
//     if(length+more>set->capacity) {
//         /* reallocate */
//         int32_t newCapacity=set->capacity+set->capacity/2+USET_GROW_DELTA;
//         UChar32* newArray=(UChar32* )uprv_malloc(newCapacity*4);
//         if(newArray==nullptr) {
//             return false;
//         }
//         set->capacity=newCapacity;
//         uprv_memcpy(newArray, set->array, length*4);
// 
//         if(set->array!=set->staticBuffer) {
//             uprv_free(set->array);
//         }
//         set->array=newArray;
//     }
// 
//     if(i<length) {
//         uprv_memmove(set->array+i+more, set->array+i, (length-i)*4);
//     }
//     set->array[i]=c;
//     if(more==2) {
//         set->array[i+1]=c+1;
//     }
//     set->length+=more;
// 
//     return true;
// }
// 
// U_CAPI UBool U_EXPORT2
// uset_add(USet* set, UChar32 c) {
//     return addRemove(set, c, 0);
// }
// 
// U_CAPI void U_EXPORT2
// uset_remove(USet* set, UChar32 c) {
//     addRemove(set, c, 1);
// }

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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/char16ptr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/parsepos.h"

U_NAMESPACE_USE

U_CAPI USet* U_EXPORT2
uset_open_ਖਾਲੀ_ਥਾਵਾਂ() {
    ਵਾਪਸ (USet*) ਨਵਾਂ Unicode_ਸੈੱਟ();
}

U_CAPI USet* U_EXPORT2
uset_open(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    ਵਾਪਸ (USet*) ਨਵਾਂ Unicode_ਸੈੱਟ(ਸ਼ੁਰੂ, ਸਿਰਾ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_close(USet* ਸੈੱਟ) {
    ਮਿਟਾਓ (Unicode_ਸੈੱਟ*) ਸੈੱਟ;
}

U_CAPI USet * U_EXPORT2
uset_clone(ਅਚਲ USet *ਸੈੱਟ) {
    ਵਾਪਸ (USet*) (((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::clone());
}

U_CAPI UBool U_EXPORT2
uset_isFrozen(ਅਚਲ USet *ਸੈੱਟ) {
    ਵਾਪਸ ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::isFrozen();
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_freeze(USet *ਸੈੱਟ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::freeze();
}

U_CAPI USet * U_EXPORT2
uset_cloneAsThawed(ਅਚਲ USet *ਸੈੱਟ) {
    ਵਾਪਸ (USet*) (((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::cloneAsThawed());
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_ਸੈੱਟ(USet* ਸੈੱਟ,
     UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਸੈੱਟ(ਸ਼ੁਰੂ, ਸਿਰਾ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_ਜੋੜੋ_All(USet* ਸੈੱਟ, ਅਚਲ USet *additional_ਸੈੱਟ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਜੋੜੋ_All(*((ਅਚਲ Unicode_ਸੈੱਟ*)additional_ਸੈੱਟ));
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_ਜੋੜੋ(USet* ਸੈੱਟ, UChar32 c) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਜੋੜੋ(c);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_ਜੋੜੋ_Range(USet* ਸੈੱਟ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਜੋੜੋ(ਸ਼ੁਰੂ, ਸਿਰਾ);    
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_ਜੋੜੋ_String(USet* ਸੈੱਟ, ਅਚਲ char16_t* str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(strLen<0, str, strLen);
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਜੋੜੋ(s);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_ਜੋੜੋ_All_ਕੋਡ_Points(USet* ਸੈੱਟ, ਅਚਲ char16_t *str, int32_t strLen) {
    // UnicodeString handles -1 for strLen
    UnicodeString s(str, strLen);
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਜੋੜੋ_All(s);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_remove(USet* ਸੈੱਟ, UChar32 c) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::remove(c);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_removeRange(USet* ਸੈੱਟ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::remove(ਸ਼ੁਰੂ, ਸਿਰਾ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_removeString(USet* ਸੈੱਟ, ਅਚਲ char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::remove(s);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_remove_All_ਕੋਡ_Points(USet *ਸੈੱਟ, ਅਚਲ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::removeAll(s);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_removeAll(USet* ਸੈੱਟ, ਅਚਲ USet* remove) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::removeAll(*(ਅਚਲ Unicode_ਸੈੱਟ*)remove);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_retain(USet* ਸੈੱਟ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::retain(ਸ਼ੁਰੂ, ਸਿਰਾ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_retainString(USet *ਸੈੱਟ, ਅਚਲ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::retain(s);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_retain_All_ਕੋਡ_Points(USet *ਸੈੱਟ, ਅਚਲ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::retainAll(s);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_retainAll(USet* ਸੈੱਟ, ਅਚਲ USet* retain) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::retainAll(*(ਅਚਲ Unicode_ਸੈੱਟ*)retain);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_compact(USet* ਸੈੱਟ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::compact();
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_complement(USet* ਸੈੱਟ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::complement();
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_complementRange(USet *ਸੈੱਟ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::complement(ਸ਼ੁਰੂ, ਸਿਰਾ);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_complementString(USet *ਸੈੱਟ, ਅਚਲ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::complement(s);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_complement_All_ਕੋਡ_Points(USet *ਸੈੱਟ, ਅਚਲ char16_t *str, int32_t length) {
    UnicodeString s(length==-1, str, length);
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::complementAll(s);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_complementAll(USet* ਸੈੱਟ, ਅਚਲ USet* complement) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::complementAll(*(ਅਚਲ Unicode_ਸੈੱਟ*)complement);
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_clear(USet* ਸੈੱਟ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::clear();
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_remove_All_ਲੜੀਆਂ(USet* ਸੈੱਟ) {
    ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::remove_All_ਲੜੀਆਂ();
}

U_CAPI UBool U_EXPORT2
uset_is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ USet* ਸੈੱਟ) {
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::is_ਖਾਲੀ_ਥਾਵਾਂ();
}

U_CAPI UBool U_EXPORT2
uset_has_ਲੜੀਆਂ(ਅਚਲ USet* ਸੈੱਟ) {
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::has_ਲੜੀਆਂ();
}

U_CAPI UBool U_EXPORT2
uset_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ USet* ਸੈੱਟ, UChar32 c) {
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ(c);
}

U_CAPI UBool U_EXPORT2
uset_ਸ਼ਾਮਲ_ਹੈ_Range(ਅਚਲ USet* ਸੈੱਟ, UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ) {
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ(ਸ਼ੁਰੂ, ਸਿਰਾ);
}

U_CAPI UBool U_EXPORT2
uset_ਸ਼ਾਮਲ_ਹੈ_String(ਅਚਲ USet* ਸੈੱਟ, ਅਚਲ char16_t* str, int32_t strLen) {
    UnicodeString s(strLen==-1, str, strLen);
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ(s);
}

U_CAPI UBool U_EXPORT2
uset_ਸ਼ਾਮਲ_ਹੈ_All(ਅਚਲ USet* set1, ਅਚਲ USet* set2) {
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) set1)->Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ_All(* (ਅਚਲ Unicode_ਸੈੱਟ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_ਸ਼ਾਮਲ_ਹੈ_All_ਕੋਡ_Points(ਅਚਲ USet* ਸੈੱਟ, ਅਚਲ char16_t *str, int32_t strLen) {
    // Create a string alias, since nothing is being added to the set.
    UnicodeString s(strLen==-1, str, strLen);
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ_All(s);
}

U_CAPI UBool U_EXPORT2
uset_ਸ਼ਾਮਲ_ਹੈ_None(ਅਚਲ USet* set1, ਅਚਲ USet* set2) {
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) set1)->Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ_None(* (ਅਚਲ Unicode_ਸੈੱਟ*) set2);
}

U_CAPI UBool U_EXPORT2
uset_ਸ਼ਾਮਲ_ਹੈ_Some(ਅਚਲ USet* set1, ਅਚਲ USet* set2) {
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) set1)->Unicode_ਸੈੱਟ::ਸ਼ਾਮਲ_ਹੈ_Some(* (ਅਚਲ Unicode_ਸੈੱਟ*) set2);
}

U_CAPI int32_t U_EXPORT2
uset_ਵਿਸਤਾਰ(ਅਚਲ USet *ਸੈੱਟ, ਅਚਲ char16_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) {
    ਵਾਪਸ ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਵਿਸਤਾਰ(s, length, ਵਿਸਤਾਰ_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_ਵਿਸਤਾਰ_ਪਿੱਠ(ਅਚਲ USet *ਸੈੱਟ, ਅਚਲ char16_t *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) {
    ਵਾਪਸ ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਵਿਸਤਾਰ_ਪਿੱਠ(s, length, ਵਿਸਤਾਰ_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_ਵਿਸਤਾਰ_UTF8(ਅਚਲ USet *ਸੈੱਟ, ਅਚਲ ਅੱਖਰ *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) {
    ਵਾਪਸ ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਵਿਸਤਾਰ_UTF8(s, length, ਵਿਸਤਾਰ_Condition);
}

U_CAPI int32_t U_EXPORT2
uset_ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(ਅਚਲ USet *ਸੈੱਟ, ਅਚਲ ਅੱਖਰ *s, int32_t length, USet_ਵਿਸਤਾਰ_Condition ਵਿਸਤਾਰ_Condition) {
    ਵਾਪਸ ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਵਿਸਤਾਰ_ਪਿੱਠ_UTF8(s, length, ਵਿਸਤਾਰ_Condition);
}

U_CAPI UBool U_EXPORT2
uset_equals(ਅਚਲ USet* set1, ਅਚਲ USet* set2) {
    ਵਾਪਸ *(ਅਚਲ Unicode_ਸੈੱਟ*)set1 == *(ਅਚਲ Unicode_ਸੈੱਟ*)set2;
}

U_CAPI int32_t U_EXPORT2
uset_ਸੂਚਕਾਂਕ_ਦਾ(ਅਚਲ USet* ਸੈੱਟ, UChar32 c) {
    ਵਾਪਸ ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::ਸੂਚਕਾਂਕ_ਦਾ(c);
}

U_CAPI UChar32 U_EXPORT2
uset_charAt(ਅਚਲ USet* ਸੈੱਟ, int32_t ਸੂਚਕਾਂਕ) {
    ਵਾਪਸ ((Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::charAt(ਸੂਚਕਾਂਕ);
}

U_CAPI int32_t U_EXPORT2
uset_ਮਾਪ(ਅਚਲ USet* ਸੈੱਟ) {
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::size();
}

U_NAMESPACE_BEGIN
/**
 * This class only exists to provide access to the UnicodeSet private
 * USet support API.  Declaring a class a friend is more portable than
 * trying to declare extern "C" functions as friends.
 */
ਵਰਗ USetAccess /* not : public UObject because all methods are static */ {
ਜਨਤਕ:
    /* Try to have the compiler inline these*/
    ਸਤਰਅੰਦਰ ਸਥਿਰ int32_t get_String_ਗਿਣਤੀ(ਅਚਲ Unicode_ਸੈੱਟ& ਸੈੱਟ) {
        ਵਾਪਸ ਸੈੱਟ.ਲੜੀਆਂ_ਮਾਪ();
    }
    ਸਤਰਅੰਦਰ ਸਥਿਰ ਅਚਲ UnicodeString* getString(ਅਚਲ Unicode_ਸੈੱਟ& ਸੈੱਟ U_LIFETIME_BOUND,
                                                 int32_t i) {
        ਵਾਪਸ ਸੈੱਟ.getString(i);
    }
ਨਿਜੀ:
    /* do not instantiate*/
    USetAccess();
};
U_NAMESPACE_ਸਿਰਾ

U_CAPI int32_t U_EXPORT2
uset_get_Range_ਗਿਣਤੀ(ਅਚਲ USet *ਸੈੱਟ) {
    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ *)ਸੈੱਟ)->Unicode_ਸੈੱਟ::get_Range_ਗਿਣਤੀ();
}

U_CAPI int32_t U_EXPORT2
uset_get_String_ਗਿਣਤੀ(ਅਚਲ USet *uset) {
    ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ = *(ਅਚਲ Unicode_ਸੈੱਟ *)uset;
    ਵਾਪਸ USetAccess::get_String_ਗਿਣਤੀ(ਸੈੱਟ);
}

U_CAPI int32_t U_EXPORT2
uset_get_ਵਸਤੂ_ਗਿਣਤੀ(ਅਚਲ USet* uset) {
    ਅਚਲ Unicode_ਸੈੱਟ& ਸੈੱਟ = *(ਅਚਲ Unicode_ਸੈੱਟ*)uset;
    ਵਾਪਸ ਸੈੱਟ.get_Range_ਗਿਣਤੀ() + USetAccess::get_String_ਗਿਣਤੀ(ਸੈੱਟ);
}

U_CAPI ਅਚਲ UChar* U_EXPORT2
uset_getString(ਅਚਲ USet* uset U_LIFETIME_BOUND, int32_t ਸੂਚਕਾਂਕ, int32_t* p_ਲੰਮਾਈ) {
    ਜੇ (p_ਲੰਮਾਈ == ਖਾਲੀ_ਸੂਚਕ) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ = *(ਅਚਲ Unicode_ਸੈੱਟ *)uset;
    int32_t count = USetAccess::get_String_ਗਿਣਤੀ(ਸੈੱਟ);
    ਜੇ (ਸੂਚਕਾਂਕ < 0 || count <= ਸੂਚਕਾਂਕ) {
        *p_ਲੰਮਾਈ = 0;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ UnicodeString *s = USetAccess::getString(ਸੈੱਟ, ਸੂਚਕਾਂਕ);
    *p_ਲੰਮਾਈ = s->length();
    ਵਾਪਸ ਮੰਜ਼ਿਲ_UChar_Ptr(s->get_ਬਫਰ());
}

U_CAPI int32_t U_EXPORT2
uset_get_ਵਸਤੂ(ਅਚਲ USet* uset, int32_t ਵਸਤੂ_ਸੂਚਕਾਂਕ,
             UChar32* ਸ਼ੁਰੂ, UChar32* ਸਿਰਾ,
             char16_t* str, int32_t str_ਸਮਰੱਥਾ,
             UError_ਕੋਡ* ec) {
    ਜੇ (U_FAILURE(*ec)) ਵਾਪਸ 0;
    ਅਚਲ Unicode_ਸੈੱਟ& ਸੈੱਟ = *(ਅਚਲ Unicode_ਸੈੱਟ*)uset;
    int32_t range_ਗਿਣਤੀ;

    ਜੇ (ਵਸਤੂ_ਸੂਚਕਾਂਕ < 0) {
        *ec = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ -1;
    } ਨਹੀਂਤਾਂ ਜੇ (ਵਸਤੂ_ਸੂਚਕਾਂਕ < (range_ਗਿਣਤੀ = ਸੈੱਟ.get_Range_ਗਿਣਤੀ())) {
        *ਸ਼ੁਰੂ = ਸੈੱਟ.get_Range_ਸ਼ੁਰੂ(ਵਸਤੂ_ਸੂਚਕਾਂਕ);
        *ਸਿਰਾ = ਸੈੱਟ.get_Range_ਸਿਰਾ(ਵਸਤੂ_ਸੂਚਕਾਂਕ);
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ {
        ਵਸਤੂ_ਸੂਚਕਾਂਕ -= range_ਗਿਣਤੀ;
        ਜੇ (ਵਸਤੂ_ਸੂਚਕਾਂਕ < USetAccess::get_String_ਗਿਣਤੀ(ਸੈੱਟ)) {
            ਅਚਲ UnicodeString* s = USetAccess::getString(ਸੈੱਟ, ਵਸਤੂ_ਸੂਚਕਾਂਕ);
            ਵਾਪਸ s->extract(str, str_ਸਮਰੱਥਾ, *ec);
        } ਨਹੀਂਤਾਂ {
            *ec = U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ -1;
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
uset_serialize(ਅਚਲ USet* ਸੈੱਟ, uint16_t* dest, int32_t dest_ਸਮਰੱਥਾ, UError_ਕੋਡ* ec) {
    ਜੇ (ec==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ec)) {
        ਵਾਪਸ 0;
    }

    ਵਾਪਸ ((ਅਚਲ Unicode_ਸੈੱਟ*) ਸੈੱਟ)->Unicode_ਸੈੱਟ::serialize(dest, dest_ਸਮਰੱਥਾ,* ec);
}

U_CAPI UBool U_EXPORT2
uset_get_Serialized_ਸੈੱਟ(USerialized_ਸੈੱਟ* fill_ਸੈੱਟ, ਅਚਲ uint16_t* src, int32_t src_ਲੰਮਾਈ) {
    int32_t length;

    ਜੇ(fill_ਸੈੱਟ==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਜੇ(src==ਖਾਲੀ_ਸੂਚਕ || src_ਲੰਮਾਈ<=0) {
        fill_ਸੈੱਟ->length=fill_ਸੈੱਟ->bmp_ਲੰਮਾਈ=0;
        ਵਾਪਸ ਝੂਠ;
    }

    length=*src++;
    ਜੇ(length&0x8000) {
        /* there are supplementary values */
        length&=0x7fff;
        ਜੇ(src_ਲੰਮਾਈ<(2+length)) {
            fill_ਸੈੱਟ->length=fill_ਸੈੱਟ->bmp_ਲੰਮਾਈ=0;
            ਵਾਪਸ ਝੂਠ;
        }
        fill_ਸੈੱਟ->bmp_ਲੰਮਾਈ=*src++;
    } ਨਹੀਂਤਾਂ {
        /* only BMP values */
        ਜੇ(src_ਲੰਮਾਈ<(1+length)) {
            fill_ਸੈੱਟ->length=fill_ਸੈੱਟ->bmp_ਲੰਮਾਈ=0;
            ਵਾਪਸ ਝੂਠ;
        }
        fill_ਸੈੱਟ->bmp_ਲੰਮਾਈ=length;
    }
    fill_ਸੈੱਟ->array=src;
    fill_ਸੈੱਟ->length=length;
    ਵਾਪਸ ਸੱਚ;
}

U_CAPI ਖਾਲੀ U_EXPORT2
uset_ਸੈੱਟ_Serialized_ਮੰਜ਼ਿਲ_One(USerialized_ਸੈੱਟ* fill_ਸੈੱਟ, UChar32 c) {
    ਜੇ(fill_ਸੈੱਟ==ਖਾਲੀ_ਸੂਚਕ || (uint32_t)c>0x10ffff) {
        ਵਾਪਸ;
    }

    fill_ਸੈੱਟ->array=fill_ਸੈੱਟ->staticArray;
    ਜੇ(c<0xffff) {
        fill_ਸੈੱਟ->bmp_ਲੰਮਾਈ=fill_ਸੈੱਟ->length=2;
        fill_ਸੈੱਟ->staticArray[0]=(uint16_t)c;
        fill_ਸੈੱਟ->staticArray[1]=(uint16_t)c+1;
    } ਨਹੀਂਤਾਂ ਜੇ(c==0xffff) {
        fill_ਸੈੱਟ->bmp_ਲੰਮਾਈ=1;
        fill_ਸੈੱਟ->length=3;
        fill_ਸੈੱਟ->staticArray[0]=0xffff;
        fill_ਸੈੱਟ->staticArray[1]=1;
        fill_ਸੈੱਟ->staticArray[2]=0;
    } ਨਹੀਂਤਾਂ ਜੇ(c<0x10ffff) {
        fill_ਸੈੱਟ->bmp_ਲੰਮਾਈ=0;
        fill_ਸੈੱਟ->length=4;
        fill_ਸੈੱਟ->staticArray[0]=(uint16_t)(c>>16);
        fill_ਸੈੱਟ->staticArray[1]=(uint16_t)c;
        ++c;
        fill_ਸੈੱਟ->staticArray[2]=(uint16_t)(c>>16);
        fill_ਸੈੱਟ->staticArray[3]=(uint16_t)c;
    } ਨਹੀਂਤਾਂ /* c==0x10ffff */ {
        fill_ਸੈੱਟ->bmp_ਲੰਮਾਈ=0;
        fill_ਸੈੱਟ->length=2;
        fill_ਸੈੱਟ->staticArray[0]=0x10;
        fill_ਸੈੱਟ->staticArray[1]=0xffff;
    }
}

U_CAPI UBool U_EXPORT2
uset_serialized_ਸ਼ਾਮਲ_ਹੈ(ਅਚਲ USerialized_ਸੈੱਟ* ਸੈੱਟ, UChar32 c) {
    ਅਚਲ uint16_t* array;

    ਜੇ(ਸੈੱਟ==ਖਾਲੀ_ਸੂਚਕ || (uint32_t)c>0x10ffff) {
        ਵਾਪਸ ਝੂਠ;
    }

    array=ਸੈੱਟ->array;
    ਜੇ(c<=0xffff) {
        /* find c in the BMP part */
        int32_t lo = 0;
        int32_t hi = ਸੈੱਟ->bmp_ਲੰਮਾਈ-1;
        ਜੇ (c < array[0]) {
            hi = 0;
        } ਨਹੀਂਤਾਂ ਜੇ (c < array[hi]) {
            ਲਈ(;;) {
                int32_t i = (lo + hi) >> 1;
                ਜੇ (i == lo) {
                    ਰੋਕੋ;  // Done!
                } ਨਹੀਂਤਾਂ ਜੇ (c < array[i]) {
                    hi = i;
                } ਨਹੀਂਤਾਂ {
                    lo = i;
                }
            }
        } ਨਹੀਂਤਾਂ {
            hi += 1;
        }
        ਵਾਪਸ hi&1;
    } ਨਹੀਂਤਾਂ {
        /* find c in the supplementary part */
        uint16_t ਉੱਚਾ=(uint16_t)(c>>16), ਨੀਵਾਂ=(uint16_t)c;
        int32_t ਅਧਾਰ = ਸੈੱਟ->bmp_ਲੰਮਾਈ;
        int32_t lo = 0;
        int32_t hi = ਸੈੱਟ->length - 2 - ਅਧਾਰ;
        ਜੇ (ਉੱਚਾ < array[ਅਧਾਰ] || (ਉੱਚਾ==array[ਅਧਾਰ] && ਨੀਵਾਂ<array[ਅਧਾਰ+1])) {
            hi = 0;
        } ਨਹੀਂਤਾਂ ਜੇ (ਉੱਚਾ < array[ਅਧਾਰ+hi] || (ਉੱਚਾ==array[ਅਧਾਰ+hi] && ਨੀਵਾਂ<array[ਅਧਾਰ+hi+1])) {
            ਲਈ (;;) {
                int32_t i = ((lo + hi) >> 1) & ~1;  // Guarantee even result
                int32_t iabs = i + ਅਧਾਰ;
                ਜੇ (i == lo) {
                    ਰੋਕੋ;  // Done!
                } ਨਹੀਂਤਾਂ ਜੇ (ਉੱਚਾ < array[iabs] || (ਉੱਚਾ==array[iabs] && ਨੀਵਾਂ<array[iabs+1])) {
                    hi = i;
                } ਨਹੀਂਤਾਂ {
                    lo = i;
                }
            }
        } ਨਹੀਂਤਾਂ {
            hi += 2;
        }
        /* count pairs of 16-bit units even per BMP and check if the number of pairs is odd */
        ਵਾਪਸ ((hi+(ਅਧਾਰ<<1))&2)!=0;
    }
}

U_CAPI int32_t U_EXPORT2
uset_get_Serialized_Range_ਗਿਣਤੀ(ਅਚਲ USerialized_ਸੈੱਟ* ਸੈੱਟ) {
    ਜੇ(ਸੈੱਟ==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    }

    ਵਾਪਸ (ਸੈੱਟ->bmp_ਲੰਮਾਈ+(ਸੈੱਟ->length-ਸੈੱਟ->bmp_ਲੰਮਾਈ)/2+1)/2;
}

U_CAPI UBool U_EXPORT2
uset_getSerializedRange(ਅਚਲ USerialized_ਸੈੱਟ* ਸੈੱਟ, int32_t range_ਸੂਚਕਾਂਕ,
                        UChar32* p_ਸ਼ੁਰੂ, UChar32* p_ਸਿਰਾ) {
    ਅਚਲ uint16_t* array;
    int32_t bmp_ਲੰਮਾਈ, length;

    ਜੇ(ਸੈੱਟ==ਖਾਲੀ_ਸੂਚਕ || range_ਸੂਚਕਾਂਕ<0 || p_ਸ਼ੁਰੂ==ਖਾਲੀ_ਸੂਚਕ || p_ਸਿਰਾ==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }

    array=ਸੈੱਟ->array;
    length=ਸੈੱਟ->length;
    bmp_ਲੰਮਾਈ=ਸੈੱਟ->bmp_ਲੰਮਾਈ;

    range_ਸੂਚਕਾਂਕ*=2; /* address start/limit pairs */
    ਜੇ(range_ਸੂਚਕਾਂਕ<bmp_ਲੰਮਾਈ) {
        *p_ਸ਼ੁਰੂ=array[range_ਸੂਚਕਾਂਕ++];
        ਜੇ(range_ਸੂਚਕਾਂਕ<bmp_ਲੰਮਾਈ) {
            *p_ਸਿਰਾ=array[range_ਸੂਚਕਾਂਕ]-1;
        } ਨਹੀਂਤਾਂ ਜੇ(range_ਸੂਚਕਾਂਕ<length) {
            *p_ਸਿਰਾ=((((int32_t)array[range_ਸੂਚਕਾਂਕ])<<16)|array[range_ਸੂਚਕਾਂਕ+1])-1;
        } ਨਹੀਂਤਾਂ {
            *p_ਸਿਰਾ=0x10ffff;
        }
        ਵਾਪਸ ਸੱਚ;
    } ਨਹੀਂਤਾਂ {
        range_ਸੂਚਕਾਂਕ-=bmp_ਲੰਮਾਈ;
        range_ਸੂਚਕਾਂਕ*=2; /* address pairs of pairs of units */
        length-=bmp_ਲੰਮਾਈ;
        ਜੇ(range_ਸੂਚਕਾਂਕ<length) {
            array+=bmp_ਲੰਮਾਈ;
            *p_ਸ਼ੁਰੂ=(((int32_t)array[range_ਸੂਚਕਾਂਕ])<<16)|array[range_ਸੂਚਕਾਂਕ+1];
            range_ਸੂਚਕਾਂਕ+=2;
            ਜੇ(range_ਸੂਚਕਾਂਕ<length) {
                *p_ਸਿਰਾ=((((int32_t)array[range_ਸੂਚਕਾਂਕ])<<16)|array[range_ਸੂਚਕਾਂਕ+1])-1;
            } ਨਹੀਂਤਾਂ {
                *p_ਸਿਰਾ=0x10ffff;
            }
            ਵਾਪਸ ਸੱਚ;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਝੂਠ;
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

// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 1997-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*   Date        Name        Description
*   03/28/00    aliu        Creation.
******************************************************************************
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HASH_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASH_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : ਜਨਤਕ UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    ਸਤਰਅੰਦਰ ਖਾਲੀ init(UHashFunction *ਕੁੰਜੀ_Hash, UKeyComparator *ਕੁੰਜੀ_Comp, UValueComparator *ਮੁੱਲ_Comp, UError_ਕੋਡ& ਸਥਿਤੀ);

    ਸਤਰਅੰਦਰ ਖਾਲੀ init_ਮਾਪ(UHashFunction *ਕੁੰਜੀ_Hash, UKeyComparator *ਕੁੰਜੀ_Comp, UValueComparator *ਮੁੱਲ_Comp, int32_t ਮਾਪ_2, UError_ਕੋਡ& ਸਥਿਤੀ);

ਜਨਤਕ:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    ਸਤਰਅੰਦਰ Hashtable(UBool ignore_ਕੁੰਜੀ_Case, UError_ਕੋਡ& ਸਥਿਤੀ);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    ਸਤਰਅੰਦਰ Hashtable(UBool ignore_ਕੁੰਜੀ_Case, int32_t ਮਾਪ_2, UError_ਕੋਡ& ਸਥਿਤੀ);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    ਸਤਰਅੰਦਰ Hashtable(UKeyComparator *ਕੁੰਜੀ_Comp, UValueComparator *ਮੁੱਲ_Comp, UError_ਕੋਡ& ਸਥਿਤੀ);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    ਸਤਰਅੰਦਰ Hashtable(UError_ਕੋਡ& ਸਥਿਤੀ);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    ਸਤਰਅੰਦਰ Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    ਸਤਰਅੰਦਰ ~Hashtable();

    ਸਤਰਅੰਦਰ UObjectDeleter *ਸੈੱਟ_ਮੁੱਲ_Deleter(UObjectDeleter *fn);

    ਸਤਰਅੰਦਰ int32_t ਗਿਣਤੀ_2() ਅਚਲ;

    ਸਤਰਅੰਦਰ ਖਾਲੀ* put(ਅਚਲ UnicodeString& ਕੁੰਜੀ, ਖਾਲੀ* ਮੁੱਲ_2, UError_ਕੋਡ& ਸਥਿਤੀ);

    ਸਤਰਅੰਦਰ int32_t puti(ਅਚਲ UnicodeString& ਕੁੰਜੀ, int32_t ਮੁੱਲ_2, UError_ਕੋਡ& ਸਥਿਤੀ);

    ਸਤਰਅੰਦਰ int32_t putiAllowZero(ਅਚਲ UnicodeString& ਕੁੰਜੀ, int32_t ਮੁੱਲ_2, UError_ਕੋਡ& ਸਥਿਤੀ);

    ਸਤਰਅੰਦਰ ਖਾਲੀ* get(ਅਚਲ UnicodeString& ਕੁੰਜੀ) ਅਚਲ;

    ਸਤਰਅੰਦਰ int32_t geti(ਅਚਲ UnicodeString& ਕੁੰਜੀ) ਅਚਲ;

    ਸਤਰਅੰਦਰ int32_t geti_And_ਮਿਲਿਆ(ਅਚਲ UnicodeString& ਕੁੰਜੀ, UBool &ਮਿਲਿਆ) ਅਚਲ;

    ਸਤਰਅੰਦਰ ਖਾਲੀ* remove(ਅਚਲ UnicodeString& ਕੁੰਜੀ);

    ਸਤਰਅੰਦਰ int32_t removei(ਅਚਲ UnicodeString& ਕੁੰਜੀ);

    ਸਤਰਅੰਦਰ ਖਾਲੀ removeAll();

    ਸਤਰਅੰਦਰ UBool ਸ਼ਾਮਲ_ਹੈ_ਕੁੰਜੀ(ਅਚਲ UnicodeString& ਕੁੰਜੀ) ਅਚਲ;

    ਸਤਰਅੰਦਰ ਅਚਲ UHashElement* ਲੱਭੋ(ਅਚਲ UnicodeString& ਕੁੰਜੀ) ਅਚਲ;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    ਸਤਰਅੰਦਰ ਅਚਲ UHashElement* ਅਗਲਾ_Element(int32_t& pos) ਅਚਲ;

    ਸਤਰਅੰਦਰ UKeyComparator* ਸੈੱਟ_ਕੁੰਜੀ_Comparator(UKeyComparator*ਕੁੰਜੀ_Comp);

    ਸਤਰਅੰਦਰ UValueComparator* ਸੈੱਟ_ਮੁੱਲ_Comparator(UValueComparator* ਮੁੱਲ_Comp);

    ਸਤਰਅੰਦਰ UBool equals(ਅਚਲ Hashtable& that) ਅਚਲ;
ਨਿਜੀ:
    Hashtable(ਅਚਲ Hashtable &other) = delete; // forbid copying of this class
    Hashtable &ਸੰਚਾਲਕ=(ਅਚਲ Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

ਸਤਰਅੰਦਰ ਖਾਲੀ Hashtable::init(UHashFunction *ਕੁੰਜੀ_Hash, UKeyComparator *ਕੁੰਜੀ_Comp,
                            UValueComparator *ਮੁੱਲ_Comp, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    uhash_init(&hashObj, ਕੁੰਜੀ_Hash, ਕੁੰਜੀ_Comp, ਮੁੱਲ_Comp, &ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        hash = &hashObj;
        uhash_ਸੈੱਟ_ਕੁੰਜੀ_Deleter(hash, uprv_deleteUObject);
    }
}

ਸਤਰਅੰਦਰ ਖਾਲੀ Hashtable::init_ਮਾਪ(UHashFunction *ਕੁੰਜੀ_Hash, UKeyComparator *ਕੁੰਜੀ_Comp,
                                UValueComparator *ਮੁੱਲ_Comp, int32_t ਮਾਪ_2, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    uhash_init_ਮਾਪ(&hashObj, ਕੁੰਜੀ_Hash, ਕੁੰਜੀ_Comp, ਮੁੱਲ_Comp, ਮਾਪ_2, &ਸਥਿਤੀ);
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        hash = &hashObj;
        uhash_ਸੈੱਟ_ਕੁੰਜੀ_Deleter(hash, uprv_deleteUObject);
    }
}

ਸਤਰਅੰਦਰ Hashtable::Hashtable(UKeyComparator *ਕੁੰਜੀ_Comp, UValueComparator *ਮੁੱਲ_Comp,
                 UError_ਕੋਡ& ਸਥਿਤੀ) : hash(nullptr) {
    init( uhash_hashUnicodeString, ਕੁੰਜੀ_Comp, ਮੁੱਲ_Comp, ਸਥਿਤੀ);
}

ਸਤਰਅੰਦਰ Hashtable::Hashtable(UBool ignore_ਕੁੰਜੀ_Case, UError_ਕੋਡ& ਸਥਿਤੀ)
 : hash(nullptr)
{
    init(ignore_ਕੁੰਜੀ_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ਕੁੰਜੀ_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            ਸਥਿਤੀ);
}

ਸਤਰਅੰਦਰ Hashtable::Hashtable(UBool ignore_ਕੁੰਜੀ_Case, int32_t ਮਾਪ_2, UError_ਕੋਡ& ਸਥਿਤੀ)
 : hash(nullptr)
{
    init_ਮਾਪ(ignore_ਕੁੰਜੀ_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ਕੁੰਜੀ_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, ਮਾਪ_2,
            ਸਥਿਤੀ);
}

ਸਤਰਅੰਦਰ Hashtable::Hashtable(UError_ਕੋਡ& ਸਥਿਤੀ)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, ਸਥਿਤੀ);
}

ਸਤਰਅੰਦਰ Hashtable::Hashtable()
 : hash(nullptr)
{
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, ਸਥਿਤੀ);
}

ਸਤਰਅੰਦਰ Hashtable::~Hashtable() {
    ਜੇ (hash != nullptr) {
        uhash_close(hash);
    }
}

ਸਤਰਅੰਦਰ UObjectDeleter *Hashtable::ਸੈੱਟ_ਮੁੱਲ_Deleter(UObjectDeleter *fn) {
    ਵਾਪਸ uhash_ਸੈੱਟ_ਮੁੱਲ_Deleter(hash, fn);
}

ਸਤਰਅੰਦਰ int32_t Hashtable::ਗਿਣਤੀ_2() ਅਚਲ {
    ਵਾਪਸ uhash_ਗਿਣਤੀ(hash);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ* Hashtable::put(ਅਚਲ UnicodeString& ਕੁੰਜੀ, ਖਾਲੀ* ਮੁੱਲ_2, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਵਾਪਸ uhash_put(hash, new UnicodeString(ਕੁੰਜੀ), ਮੁੱਲ_2, &ਸਥਿਤੀ);
}

ਸਤਰਅੰਦਰ int32_t Hashtable::puti(ਅਚਲ UnicodeString& ਕੁੰਜੀ, int32_t ਮੁੱਲ_2, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਵਾਪਸ uhash_puti(hash, new UnicodeString(ਕੁੰਜੀ), ਮੁੱਲ_2, &ਸਥਿਤੀ);
}

ਸਤਰਅੰਦਰ int32_t Hashtable::putiAllowZero(ਅਚਲ UnicodeString& ਕੁੰਜੀ, int32_t ਮੁੱਲ_2,
                                        UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਵਾਪਸ uhash_putiAllowZero(hash, new UnicodeString(ਕੁੰਜੀ), ਮੁੱਲ_2, &ਸਥਿਤੀ);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ* Hashtable::get(ਅਚਲ UnicodeString& ਕੁੰਜੀ) ਅਚਲ {
    ਵਾਪਸ uhash_get(hash, &ਕੁੰਜੀ);
}

ਸਤਰਅੰਦਰ int32_t Hashtable::geti(ਅਚਲ UnicodeString& ਕੁੰਜੀ) ਅਚਲ {
    ਵਾਪਸ uhash_geti(hash, &ਕੁੰਜੀ);
}

ਸਤਰਅੰਦਰ int32_t Hashtable::geti_And_ਮਿਲਿਆ(ਅਚਲ UnicodeString& ਕੁੰਜੀ, UBool &ਮਿਲਿਆ) ਅਚਲ {
    ਵਾਪਸ uhash_geti_And_ਮਿਲਿਆ(hash, &ਕੁੰਜੀ, &ਮਿਲਿਆ);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ* Hashtable::remove(ਅਚਲ UnicodeString& ਕੁੰਜੀ) {
    ਵਾਪਸ uhash_remove(hash, &ਕੁੰਜੀ);
}

ਸਤਰਅੰਦਰ int32_t Hashtable::removei(ਅਚਲ UnicodeString& ਕੁੰਜੀ) {
    ਵਾਪਸ uhash_removei(hash, &ਕੁੰਜੀ);
}

ਸਤਰਅੰਦਰ UBool Hashtable::ਸ਼ਾਮਲ_ਹੈ_ਕੁੰਜੀ(ਅਚਲ UnicodeString& ਕੁੰਜੀ) ਅਚਲ {
    ਵਾਪਸ uhash_ਸ਼ਾਮਲ_ਹੈ_ਕੁੰਜੀ(hash, &ਕੁੰਜੀ);
}

ਸਤਰਅੰਦਰ ਅਚਲ UHashElement* Hashtable::ਲੱਭੋ(ਅਚਲ UnicodeString& ਕੁੰਜੀ) ਅਚਲ {
    ਵਾਪਸ uhash_ਲੱਭੋ(hash, &ਕੁੰਜੀ);
}

ਸਤਰਅੰਦਰ ਅਚਲ UHashElement* Hashtable::ਅਗਲਾ_Element(int32_t& pos) ਅਚਲ {
    ਵਾਪਸ uhash_ਅਗਲਾ_Element(hash, &pos);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

ਸਤਰਅੰਦਰ UKeyComparator* Hashtable::ਸੈੱਟ_ਕੁੰਜੀ_Comparator(UKeyComparator*ਕੁੰਜੀ_Comp){
    ਵਾਪਸ uhash_ਸੈੱਟ_ਕੁੰਜੀ_Comparator(hash, ਕੁੰਜੀ_Comp);
}

ਸਤਰਅੰਦਰ UValueComparator* Hashtable::ਸੈੱਟ_ਮੁੱਲ_Comparator(UValueComparator* ਮੁੱਲ_Comp){
    ਵਾਪਸ uhash_ਸੈੱਟ_ਮੁੱਲ_Comparator(hash, ਮੁੱਲ_Comp);
}

ਸਤਰਅੰਦਰ UBool Hashtable::equals(ਅਚਲ Hashtable& that)ਅਚਲ{
   ਵਾਪਸ uhash_equals(hash, that.hash);
}
U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ


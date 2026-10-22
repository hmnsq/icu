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

#جيڪڏهن_وصف_نه_ٿيل HASH_H
#وصف_ڪريو HASH_H

#ضمن "unicode/unistr.h"
#ضمن "unicode/uobject.h"
#ضمن "cmemory.h"
#ضمن "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : عام UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    مضمن فراغ init(UHashFunction *ڪنجي_Hash, UKeyComparator *ڪنجي_Comp, UValueComparator *قدر_Comp, UError_ڪوڊ& حالت);

    مضمن فراغ init_سائيز(UHashFunction *ڪنجي_Hash, UKeyComparator *ڪنجي_Comp, UValueComparator *قدر_Comp, int32_t سائيز_2, UError_ڪوڊ& حالت);

عام:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_ڪنجي_Case, UError_ڪوڊ& حالت);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_ڪنجي_Case, int32_t سائيز_2, UError_ڪوڊ& حالت);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    مضمن Hashtable(UKeyComparator *ڪنجي_Comp, UValueComparator *قدر_Comp, UError_ڪوڊ& حالت);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    مضمن Hashtable(UError_ڪوڊ& حالت);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    مضمن Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    مضمن ~Hashtable();

    مضمن UObjectDeleter *سيٽ_قدر_Deleter(UObjectDeleter *fn);

    مضمن int32_t ڳڻپ() ثابت;

    مضمن فراغ* put(ثابت UnicodeString& ڪنجي, فراغ* قدر_2, UError_ڪوڊ& حالت);

    مضمن int32_t puti(ثابت UnicodeString& ڪنجي, int32_t قدر_2, UError_ڪوڊ& حالت);

    مضمن int32_t putiAllowZero(ثابت UnicodeString& ڪنجي, int32_t قدر_2, UError_ڪوڊ& حالت);

    مضمن فراغ* get(ثابت UnicodeString& ڪنجي) ثابت;

    مضمن int32_t geti(ثابت UnicodeString& ڪنجي) ثابت;

    مضمن int32_t geti_And_مليو(ثابت UnicodeString& ڪنجي, UBool &مليو) ثابت;

    مضمن فراغ* remove(ثابت UnicodeString& ڪنجي);

    مضمن int32_t removei(ثابت UnicodeString& ڪنجي);

    مضمن فراغ removeAll();

    مضمن UBool شامل_آهي_ڪنجي(ثابت UnicodeString& ڪنجي) ثابت;

    مضمن ثابت UHashElement* ڳوليو(ثابت UnicodeString& ڪنجي) ثابت;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    مضمن ثابت UHashElement* ايندڙ_Element(int32_t& pos) ثابت;

    مضمن UKeyComparator* سيٽ_ڪنجي_Comparator(UKeyComparator*ڪنجي_Comp);

    مضمن UValueComparator* سيٽ_قدر_Comparator(UValueComparator* قدر_Comp);

    مضمن UBool equals(ثابت Hashtable& that) ثابت;
خاص:
    Hashtable(ثابت Hashtable &other) = delete; // forbid copying of this class
    Hashtable &عامل=(ثابت Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

مضمن فراغ Hashtable::init(UHashFunction *ڪنجي_Hash, UKeyComparator *ڪنجي_Comp,
                            UValueComparator *قدر_Comp, UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_init(&hashObj, ڪنجي_Hash, ڪنجي_Comp, قدر_Comp, &حالت);
    إذا (U_SUCCESS(حالت)) {
        hash = &hashObj;
        uhash_سيٽ_ڪنجي_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن فراغ Hashtable::init_سائيز(UHashFunction *ڪنجي_Hash, UKeyComparator *ڪنجي_Comp,
                                UValueComparator *قدر_Comp, int32_t سائيز_2, UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_init_سائيز(&hashObj, ڪنجي_Hash, ڪنجي_Comp, قدر_Comp, سائيز_2, &حالت);
    إذا (U_SUCCESS(حالت)) {
        hash = &hashObj;
        uhash_سيٽ_ڪنجي_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن Hashtable::Hashtable(UKeyComparator *ڪنجي_Comp, UValueComparator *قدر_Comp,
                 UError_ڪوڊ& حالت) : hash(nullptr) {
    init( uhash_hashUnicodeString, ڪنجي_Comp, قدر_Comp, حالت);
}

مضمن Hashtable::Hashtable(UBool ignore_ڪنجي_Case, UError_ڪوڊ& حالت)
 : hash(nullptr)
{
    init(ignore_ڪنجي_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ڪنجي_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            حالت);
}

مضمن Hashtable::Hashtable(UBool ignore_ڪنجي_Case, int32_t سائيز_2, UError_ڪوڊ& حالت)
 : hash(nullptr)
{
    init_سائيز(ignore_ڪنجي_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ڪنجي_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, سائيز_2,
            حالت);
}

مضمن Hashtable::Hashtable(UError_ڪوڊ& حالت)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, حالت);
}

مضمن Hashtable::Hashtable()
 : hash(nullptr)
{
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, حالت);
}

مضمن Hashtable::~Hashtable() {
    إذا (hash != nullptr) {
        uhash_close(hash);
    }
}

مضمن UObjectDeleter *Hashtable::سيٽ_قدر_Deleter(UObjectDeleter *fn) {
    أرجع uhash_سيٽ_قدر_Deleter(hash, fn);
}

مضمن int32_t Hashtable::ڳڻپ() ثابت {
    أرجع uhash_ڳڻپ(hash);
}

مضمن فراغ* Hashtable::put(ثابت UnicodeString& ڪنجي, فراغ* قدر_2, UError_ڪوڊ& حالت) {
    أرجع uhash_put(hash, new UnicodeString(ڪنجي), قدر_2, &حالت);
}

مضمن int32_t Hashtable::puti(ثابت UnicodeString& ڪنجي, int32_t قدر_2, UError_ڪوڊ& حالت) {
    أرجع uhash_puti(hash, new UnicodeString(ڪنجي), قدر_2, &حالت);
}

مضمن int32_t Hashtable::putiAllowZero(ثابت UnicodeString& ڪنجي, int32_t قدر_2,
                                        UError_ڪوڊ& حالت) {
    أرجع uhash_putiAllowZero(hash, new UnicodeString(ڪنجي), قدر_2, &حالت);
}

مضمن فراغ* Hashtable::get(ثابت UnicodeString& ڪنجي) ثابت {
    أرجع uhash_get(hash, &ڪنجي);
}

مضمن int32_t Hashtable::geti(ثابت UnicodeString& ڪنجي) ثابت {
    أرجع uhash_geti(hash, &ڪنجي);
}

مضمن int32_t Hashtable::geti_And_مليو(ثابت UnicodeString& ڪنجي, UBool &مليو) ثابت {
    أرجع uhash_geti_And_مليو(hash, &ڪنجي, &مليو);
}

مضمن فراغ* Hashtable::remove(ثابت UnicodeString& ڪنجي) {
    أرجع uhash_remove(hash, &ڪنجي);
}

مضمن int32_t Hashtable::removei(ثابت UnicodeString& ڪنجي) {
    أرجع uhash_removei(hash, &ڪنجي);
}

مضمن UBool Hashtable::شامل_آهي_ڪنجي(ثابت UnicodeString& ڪنجي) ثابت {
    أرجع uhash_شامل_آهي_ڪنجي(hash, &ڪنجي);
}

مضمن ثابت UHashElement* Hashtable::ڳوليو(ثابت UnicodeString& ڪنجي) ثابت {
    أرجع uhash_ڳوليو(hash, &ڪنجي);
}

مضمن ثابت UHashElement* Hashtable::ايندڙ_Element(int32_t& pos) ثابت {
    أرجع uhash_ايندڙ_Element(hash, &pos);
}

مضمن فراغ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

مضمن UKeyComparator* Hashtable::سيٽ_ڪنجي_Comparator(UKeyComparator*ڪنجي_Comp){
    أرجع uhash_سيٽ_ڪنجي_Comparator(hash, ڪنجي_Comp);
}

مضمن UValueComparator* Hashtable::سيٽ_قدر_Comparator(UValueComparator* قدر_Comp){
    أرجع uhash_سيٽ_قدر_Comparator(hash, قدر_Comp);
}

مضمن UBool Hashtable::equals(ثابت Hashtable& that)ثابت{
   أرجع uhash_equals(hash, that.hash);
}
U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو


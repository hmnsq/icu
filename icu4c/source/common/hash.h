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

#جے_بنا_تعریف HASH_H
#تعریف_کرو HASH_H

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

    مضمن فراغ init(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp, UValueComparator *قدر_Comp, UError_کوڈ& حالت);

    مضمن فراغ init_ناپ(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp, UValueComparator *قدر_Comp, int32_t ناپ_2, UError_کوڈ& حالت);

عام:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_کلید_Case, UError_کوڈ& حالت);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_کلید_Case, int32_t ناپ_2, UError_کوڈ& حالت);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    مضمن Hashtable(UKeyComparator *کلید_Comp, UValueComparator *قدر_Comp, UError_کوڈ& حالت);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    مضمن Hashtable(UError_کوڈ& حالت);

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

    مضمن UObjectDeleter *سیٹ_قدر_Deleter(UObjectDeleter *fn);

    مضمن int32_t گنتی() ثابت;

    مضمن فراغ* put(ثابت UnicodeString& کلید, فراغ* قدر_2, UError_کوڈ& حالت);

    مضمن int32_t puti(ثابت UnicodeString& کلید, int32_t قدر_2, UError_کوڈ& حالت);

    مضمن int32_t putiAllowZero(ثابت UnicodeString& کلید, int32_t قدر_2, UError_کوڈ& حالت);

    مضمن فراغ* get(ثابت UnicodeString& کلید) ثابت;

    مضمن int32_t geti(ثابت UnicodeString& کلید) ثابت;

    مضمن int32_t geti_And_لبھیا(ثابت UnicodeString& کلید, UBool &لبھیا) ثابت;

    مضمن فراغ* remove(ثابت UnicodeString& کلید);

    مضمن int32_t removei(ثابت UnicodeString& کلید);

    مضمن فراغ removeAll();

    مضمن UBool شامل_اے_کلید(ثابت UnicodeString& کلید) ثابت;

    مضمن ثابت UHashElement* لبھو(ثابت UnicodeString& کلید) ثابت;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    مضمن ثابت UHashElement* اگلا_Element(int32_t& pos) ثابت;

    مضمن UKeyComparator* سیٹ_کلید_Comparator(UKeyComparator*کلید_Comp);

    مضمن UValueComparator* سیٹ_قدر_Comparator(UValueComparator* قدر_Comp);

    مضمن UBool equals(ثابت Hashtable& that) ثابت;
خاص:
    Hashtable(ثابت Hashtable &other) = delete; // forbid copying of this class
    Hashtable &عامل=(ثابت Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

مضمن فراغ Hashtable::init(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp,
                            UValueComparator *قدر_Comp, UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_init(&hashObj, کلید_Hash, کلید_Comp, قدر_Comp, &حالت);
    إذا (U_SUCCESS(حالت)) {
        hash = &hashObj;
        uhash_سیٹ_کلید_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن فراغ Hashtable::init_ناپ(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp,
                                UValueComparator *قدر_Comp, int32_t ناپ_2, UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_init_ناپ(&hashObj, کلید_Hash, کلید_Comp, قدر_Comp, ناپ_2, &حالت);
    إذا (U_SUCCESS(حالت)) {
        hash = &hashObj;
        uhash_سیٹ_کلید_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن Hashtable::Hashtable(UKeyComparator *کلید_Comp, UValueComparator *قدر_Comp,
                 UError_کوڈ& حالت) : hash(nullptr) {
    init( uhash_hashUnicodeString, کلید_Comp, قدر_Comp, حالت);
}

مضمن Hashtable::Hashtable(UBool ignore_کلید_Case, UError_کوڈ& حالت)
 : hash(nullptr)
{
    init(ignore_کلید_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلید_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            حالت);
}

مضمن Hashtable::Hashtable(UBool ignore_کلید_Case, int32_t ناپ_2, UError_کوڈ& حالت)
 : hash(nullptr)
{
    init_ناپ(ignore_کلید_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلید_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, ناپ_2,
            حالت);
}

مضمن Hashtable::Hashtable(UError_کوڈ& حالت)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, حالت);
}

مضمن Hashtable::Hashtable()
 : hash(nullptr)
{
    UError_کوڈ حالت = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, حالت);
}

مضمن Hashtable::~Hashtable() {
    إذا (hash != nullptr) {
        uhash_close(hash);
    }
}

مضمن UObjectDeleter *Hashtable::سیٹ_قدر_Deleter(UObjectDeleter *fn) {
    أرجع uhash_سیٹ_قدر_Deleter(hash, fn);
}

مضمن int32_t Hashtable::گنتی() ثابت {
    أرجع uhash_گنتی(hash);
}

مضمن فراغ* Hashtable::put(ثابت UnicodeString& کلید, فراغ* قدر_2, UError_کوڈ& حالت) {
    أرجع uhash_put(hash, new UnicodeString(کلید), قدر_2, &حالت);
}

مضمن int32_t Hashtable::puti(ثابت UnicodeString& کلید, int32_t قدر_2, UError_کوڈ& حالت) {
    أرجع uhash_puti(hash, new UnicodeString(کلید), قدر_2, &حالت);
}

مضمن int32_t Hashtable::putiAllowZero(ثابت UnicodeString& کلید, int32_t قدر_2,
                                        UError_کوڈ& حالت) {
    أرجع uhash_putiAllowZero(hash, new UnicodeString(کلید), قدر_2, &حالت);
}

مضمن فراغ* Hashtable::get(ثابت UnicodeString& کلید) ثابت {
    أرجع uhash_get(hash, &کلید);
}

مضمن int32_t Hashtable::geti(ثابت UnicodeString& کلید) ثابت {
    أرجع uhash_geti(hash, &کلید);
}

مضمن int32_t Hashtable::geti_And_لبھیا(ثابت UnicodeString& کلید, UBool &لبھیا) ثابت {
    أرجع uhash_geti_And_لبھیا(hash, &کلید, &لبھیا);
}

مضمن فراغ* Hashtable::remove(ثابت UnicodeString& کلید) {
    أرجع uhash_remove(hash, &کلید);
}

مضمن int32_t Hashtable::removei(ثابت UnicodeString& کلید) {
    أرجع uhash_removei(hash, &کلید);
}

مضمن UBool Hashtable::شامل_اے_کلید(ثابت UnicodeString& کلید) ثابت {
    أرجع uhash_شامل_اے_کلید(hash, &کلید);
}

مضمن ثابت UHashElement* Hashtable::لبھو(ثابت UnicodeString& کلید) ثابت {
    أرجع uhash_لبھو(hash, &کلید);
}

مضمن ثابت UHashElement* Hashtable::اگلا_Element(int32_t& pos) ثابت {
    أرجع uhash_اگلا_Element(hash, &pos);
}

مضمن فراغ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

مضمن UKeyComparator* Hashtable::سیٹ_کلید_Comparator(UKeyComparator*کلید_Comp){
    أرجع uhash_سیٹ_کلید_Comparator(hash, کلید_Comp);
}

مضمن UValueComparator* Hashtable::سیٹ_قدر_Comparator(UValueComparator* قدر_Comp){
    أرجع uhash_سیٹ_قدر_Comparator(hash, قدر_Comp);
}

مضمن UBool Hashtable::equals(ثابت Hashtable& that)ثابت{
   أرجع uhash_equals(hash, that.hash);
}
U_NAMESPACE_سرا

#جے_انت


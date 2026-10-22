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

#که_نه_تعریف_شوی HASH_H
#تعریف_کړه HASH_H

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

    مضمن فراغ init(UHashFunction *کلی_Hash, UKeyComparator *کلی_Comp, UValueComparator *ارزښت_Comp, UError_کوډ& حالت);

    مضمن فراغ init_اندازه(UHashFunction *کلی_Hash, UKeyComparator *کلی_Comp, UValueComparator *ارزښت_Comp, int32_t اندازه_2, UError_کوډ& حالت);

عام:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_کلی_Case, UError_کوډ& حالت);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_کلی_Case, int32_t اندازه_2, UError_کوډ& حالت);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    مضمن Hashtable(UKeyComparator *کلی_Comp, UValueComparator *ارزښت_Comp, UError_کوډ& حالت);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    مضمن Hashtable(UError_کوډ& حالت);

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

    مضمن UObjectDeleter *سېټ_ارزښت_Deleter(UObjectDeleter *fn);

    مضمن int32_t شمېر() ثابت;

    مضمن فراغ* put(ثابت UnicodeString& کلی, فراغ* ارزښت_2, UError_کوډ& حالت);

    مضمن int32_t puti(ثابت UnicodeString& کلی, int32_t ارزښت_2, UError_کوډ& حالت);

    مضمن int32_t putiAllowZero(ثابت UnicodeString& کلی, int32_t ارزښت_2, UError_کوډ& حالت);

    مضمن فراغ* get(ثابت UnicodeString& کلی) ثابت;

    مضمن int32_t geti(ثابت UnicodeString& کلی) ثابت;

    مضمن int32_t geti_And_وموندل(ثابت UnicodeString& کلی, UBool &وموندل) ثابت;

    مضمن فراغ* remove(ثابت UnicodeString& کلی);

    مضمن int32_t removei(ثابت UnicodeString& کلی);

    مضمن فراغ removeAll();

    مضمن UBool لري_کلی(ثابت UnicodeString& کلی) ثابت;

    مضمن ثابت UHashElement* پیدا_کړه(ثابت UnicodeString& کلی) ثابت;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    مضمن ثابت UHashElement* بل_Element(int32_t& pos) ثابت;

    مضمن UKeyComparator* سېټ_کلی_Comparator(UKeyComparator*کلی_Comp);

    مضمن UValueComparator* سېټ_ارزښت_Comparator(UValueComparator* ارزښت_Comp);

    مضمن UBool equals(ثابت Hashtable& that) ثابت;
خاص:
    Hashtable(ثابت Hashtable &other) = delete; // forbid copying of this class
    Hashtable &چلوونکی=(ثابت Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

مضمن فراغ Hashtable::init(UHashFunction *کلی_Hash, UKeyComparator *کلی_Comp,
                            UValueComparator *ارزښت_Comp, UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_init(&hashObj, کلی_Hash, کلی_Comp, ارزښت_Comp, &حالت);
    إذا (U_SUCCESS(حالت)) {
        hash = &hashObj;
        uhash_سېټ_کلی_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن فراغ Hashtable::init_اندازه(UHashFunction *کلی_Hash, UKeyComparator *کلی_Comp,
                                UValueComparator *ارزښت_Comp, int32_t اندازه_2, UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_init_اندازه(&hashObj, کلی_Hash, کلی_Comp, ارزښت_Comp, اندازه_2, &حالت);
    إذا (U_SUCCESS(حالت)) {
        hash = &hashObj;
        uhash_سېټ_کلی_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن Hashtable::Hashtable(UKeyComparator *کلی_Comp, UValueComparator *ارزښت_Comp,
                 UError_کوډ& حالت) : hash(nullptr) {
    init( uhash_hashUnicodeString, کلی_Comp, ارزښت_Comp, حالت);
}

مضمن Hashtable::Hashtable(UBool ignore_کلی_Case, UError_کوډ& حالت)
 : hash(nullptr)
{
    init(ignore_کلی_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلی_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            حالت);
}

مضمن Hashtable::Hashtable(UBool ignore_کلی_Case, int32_t اندازه_2, UError_کوډ& حالت)
 : hash(nullptr)
{
    init_اندازه(ignore_کلی_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلی_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, اندازه_2,
            حالت);
}

مضمن Hashtable::Hashtable(UError_کوډ& حالت)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, حالت);
}

مضمن Hashtable::Hashtable()
 : hash(nullptr)
{
    UError_کوډ حالت = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, حالت);
}

مضمن Hashtable::~Hashtable() {
    إذا (hash != nullptr) {
        uhash_close(hash);
    }
}

مضمن UObjectDeleter *Hashtable::سېټ_ارزښت_Deleter(UObjectDeleter *fn) {
    أرجع uhash_سېټ_ارزښت_Deleter(hash, fn);
}

مضمن int32_t Hashtable::شمېر() ثابت {
    أرجع uhash_شمېر(hash);
}

مضمن فراغ* Hashtable::put(ثابت UnicodeString& کلی, فراغ* ارزښت_2, UError_کوډ& حالت) {
    أرجع uhash_put(hash, new UnicodeString(کلی), ارزښت_2, &حالت);
}

مضمن int32_t Hashtable::puti(ثابت UnicodeString& کلی, int32_t ارزښت_2, UError_کوډ& حالت) {
    أرجع uhash_puti(hash, new UnicodeString(کلی), ارزښت_2, &حالت);
}

مضمن int32_t Hashtable::putiAllowZero(ثابت UnicodeString& کلی, int32_t ارزښت_2,
                                        UError_کوډ& حالت) {
    أرجع uhash_putiAllowZero(hash, new UnicodeString(کلی), ارزښت_2, &حالت);
}

مضمن فراغ* Hashtable::get(ثابت UnicodeString& کلی) ثابت {
    أرجع uhash_get(hash, &کلی);
}

مضمن int32_t Hashtable::geti(ثابت UnicodeString& کلی) ثابت {
    أرجع uhash_geti(hash, &کلی);
}

مضمن int32_t Hashtable::geti_And_وموندل(ثابت UnicodeString& کلی, UBool &وموندل) ثابت {
    أرجع uhash_geti_And_وموندل(hash, &کلی, &وموندل);
}

مضمن فراغ* Hashtable::remove(ثابت UnicodeString& کلی) {
    أرجع uhash_remove(hash, &کلی);
}

مضمن int32_t Hashtable::removei(ثابت UnicodeString& کلی) {
    أرجع uhash_removei(hash, &کلی);
}

مضمن UBool Hashtable::لري_کلی(ثابت UnicodeString& کلی) ثابت {
    أرجع uhash_لري_کلی(hash, &کلی);
}

مضمن ثابت UHashElement* Hashtable::پیدا_کړه(ثابت UnicodeString& کلی) ثابت {
    أرجع uhash_پیدا_کړه(hash, &کلی);
}

مضمن ثابت UHashElement* Hashtable::بل_Element(int32_t& pos) ثابت {
    أرجع uhash_بل_Element(hash, &pos);
}

مضمن فراغ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

مضمن UKeyComparator* Hashtable::سېټ_کلی_Comparator(UKeyComparator*کلی_Comp){
    أرجع uhash_سېټ_کلی_Comparator(hash, کلی_Comp);
}

مضمن UValueComparator* Hashtable::سېټ_ارزښت_Comparator(UValueComparator* ارزښت_Comp){
    أرجع uhash_سېټ_ارزښت_Comparator(hash, ارزښت_Comp);
}

مضمن UBool Hashtable::equals(ثابت Hashtable& that)ثابت{
   أرجع uhash_equals(hash, that.hash);
}
U_NAMESPACE_پای_ټکی

#د_که_پای


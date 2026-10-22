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

#اگر_تعریف_نشده HASH_H
#تعریف_کن HASH_H

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

    مضمن فراغ init(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp, UValueComparator *مقدار_Comp, UError_کد& وضعیت);

    مضمن فراغ init_اندازه(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp, UValueComparator *مقدار_Comp, int32_t اندازه_2, UError_کد& وضعیت);

عام:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_کلید_Case, UError_کد& وضعیت);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_کلید_Case, int32_t اندازه_2, UError_کد& وضعیت);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    مضمن Hashtable(UKeyComparator *کلید_Comp, UValueComparator *مقدار_Comp, UError_کد& وضعیت);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    مضمن Hashtable(UError_کد& وضعیت);

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

    مضمن UObjectDeleter *مجموعه_مقدار_Deleter(UObjectDeleter *fn);

    مضمن int32_t شمار() ثابت;

    مضمن فراغ* put(ثابت UnicodeString& کلید, فراغ* مقدار_2, UError_کد& وضعیت);

    مضمن int32_t puti(ثابت UnicodeString& کلید, int32_t مقدار_2, UError_کد& وضعیت);

    مضمن int32_t putiAllowZero(ثابت UnicodeString& کلید, int32_t مقدار_2, UError_کد& وضعیت);

    مضمن فراغ* get(ثابت UnicodeString& کلید) ثابت;

    مضمن int32_t geti(ثابت UnicodeString& کلید) ثابت;

    مضمن int32_t geti_And_یافت(ثابت UnicodeString& کلید, UBool &یافت) ثابت;

    مضمن فراغ* remove(ثابت UnicodeString& کلید);

    مضمن int32_t removei(ثابت UnicodeString& کلید);

    مضمن فراغ removeAll();

    مضمن UBool شامل_است_کلید(ثابت UnicodeString& کلید) ثابت;

    مضمن ثابت UHashElement* پیدا_کن(ثابت UnicodeString& کلید) ثابت;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    مضمن ثابت UHashElement* بعدی_Element(int32_t& pos) ثابت;

    مضمن UKeyComparator* مجموعه_کلید_Comparator(UKeyComparator*کلید_Comp);

    مضمن UValueComparator* مجموعه_مقدار_Comparator(UValueComparator* مقدار_Comp);

    مضمن UBool equals(ثابت Hashtable& that) ثابت;
خاص:
    Hashtable(ثابت Hashtable &other) = delete; // forbid copying of this class
    Hashtable &عملگر=(ثابت Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

مضمن فراغ Hashtable::init(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp,
                            UValueComparator *مقدار_Comp, UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    uhash_init(&hashObj, کلید_Hash, کلید_Comp, مقدار_Comp, &وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        hash = &hashObj;
        uhash_مجموعه_کلید_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن فراغ Hashtable::init_اندازه(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp,
                                UValueComparator *مقدار_Comp, int32_t اندازه_2, UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    uhash_init_اندازه(&hashObj, کلید_Hash, کلید_Comp, مقدار_Comp, اندازه_2, &وضعیت);
    إذا (U_SUCCESS(وضعیت)) {
        hash = &hashObj;
        uhash_مجموعه_کلید_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن Hashtable::Hashtable(UKeyComparator *کلید_Comp, UValueComparator *مقدار_Comp,
                 UError_کد& وضعیت) : hash(nullptr) {
    init( uhash_hashUnicodeString, کلید_Comp, مقدار_Comp, وضعیت);
}

مضمن Hashtable::Hashtable(UBool ignore_کلید_Case, UError_کد& وضعیت)
 : hash(nullptr)
{
    init(ignore_کلید_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلید_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            وضعیت);
}

مضمن Hashtable::Hashtable(UBool ignore_کلید_Case, int32_t اندازه_2, UError_کد& وضعیت)
 : hash(nullptr)
{
    init_اندازه(ignore_کلید_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلید_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, اندازه_2,
            وضعیت);
}

مضمن Hashtable::Hashtable(UError_کد& وضعیت)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, وضعیت);
}

مضمن Hashtable::Hashtable()
 : hash(nullptr)
{
    UError_کد وضعیت = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, وضعیت);
}

مضمن Hashtable::~Hashtable() {
    إذا (hash != nullptr) {
        uhash_close(hash);
    }
}

مضمن UObjectDeleter *Hashtable::مجموعه_مقدار_Deleter(UObjectDeleter *fn) {
    أرجع uhash_مجموعه_مقدار_Deleter(hash, fn);
}

مضمن int32_t Hashtable::شمار() ثابت {
    أرجع uhash_شمار(hash);
}

مضمن فراغ* Hashtable::put(ثابت UnicodeString& کلید, فراغ* مقدار_2, UError_کد& وضعیت) {
    أرجع uhash_put(hash, new UnicodeString(کلید), مقدار_2, &وضعیت);
}

مضمن int32_t Hashtable::puti(ثابت UnicodeString& کلید, int32_t مقدار_2, UError_کد& وضعیت) {
    أرجع uhash_puti(hash, new UnicodeString(کلید), مقدار_2, &وضعیت);
}

مضمن int32_t Hashtable::putiAllowZero(ثابت UnicodeString& کلید, int32_t مقدار_2,
                                        UError_کد& وضعیت) {
    أرجع uhash_putiAllowZero(hash, new UnicodeString(کلید), مقدار_2, &وضعیت);
}

مضمن فراغ* Hashtable::get(ثابت UnicodeString& کلید) ثابت {
    أرجع uhash_get(hash, &کلید);
}

مضمن int32_t Hashtable::geti(ثابت UnicodeString& کلید) ثابت {
    أرجع uhash_geti(hash, &کلید);
}

مضمن int32_t Hashtable::geti_And_یافت(ثابت UnicodeString& کلید, UBool &یافت) ثابت {
    أرجع uhash_geti_And_یافت(hash, &کلید, &یافت);
}

مضمن فراغ* Hashtable::remove(ثابت UnicodeString& کلید) {
    أرجع uhash_remove(hash, &کلید);
}

مضمن int32_t Hashtable::removei(ثابت UnicodeString& کلید) {
    أرجع uhash_removei(hash, &کلید);
}

مضمن UBool Hashtable::شامل_است_کلید(ثابت UnicodeString& کلید) ثابت {
    أرجع uhash_شامل_است_کلید(hash, &کلید);
}

مضمن ثابت UHashElement* Hashtable::پیدا_کن(ثابت UnicodeString& کلید) ثابت {
    أرجع uhash_پیدا_کن(hash, &کلید);
}

مضمن ثابت UHashElement* Hashtable::بعدی_Element(int32_t& pos) ثابت {
    أرجع uhash_بعدی_Element(hash, &pos);
}

مضمن فراغ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

مضمن UKeyComparator* Hashtable::مجموعه_کلید_Comparator(UKeyComparator*کلید_Comp){
    أرجع uhash_مجموعه_کلید_Comparator(hash, کلید_Comp);
}

مضمن UValueComparator* Hashtable::مجموعه_مقدار_Comparator(UValueComparator* مقدار_Comp){
    أرجع uhash_مجموعه_مقدار_Comparator(hash, مقدار_Comp);
}

مضمن UBool Hashtable::equals(ثابت Hashtable& that)ثابت{
   أرجع uhash_equals(hash, that.hash);
}
U_NAMESPACE_انتها

#پایان_اگر


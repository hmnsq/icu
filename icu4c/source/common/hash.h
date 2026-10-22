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

#ifndef HASH_H
#define HASH_H

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

    مضمن فراغ init(UHashFunction *مفتاح_Hash, UKeyComparator *مفتاح_Comp, UValueComparator *قيمة_Comp, UErrorCode& status);

    مضمن فراغ initSize(UHashFunction *مفتاح_Hash, UKeyComparator *مفتاح_Comp, UValueComparator *قيمة_Comp, int32_t size, UErrorCode& status);

عام:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_مفتاح_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_مفتاح_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    مضمن Hashtable(UKeyComparator *مفتاح_Comp, UValueComparator *قيمة_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    مضمن Hashtable(UErrorCode& status);

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

    مضمن UObjectDeleter *set_قيمة_Deleter(UObjectDeleter *fn);

    مضمن int32_t تعداد_2() ثابت;

    مضمن فراغ* put(ثابت UnicodeString& مفتاح, فراغ* قيمة_2, UErrorCode& status);

    مضمن int32_t puti(ثابت UnicodeString& مفتاح, int32_t قيمة_2, UErrorCode& status);

    مضمن int32_t putiAllowZero(ثابت UnicodeString& مفتاح, int32_t قيمة_2, UErrorCode& status);

    مضمن فراغ* get(ثابت UnicodeString& مفتاح) ثابت;

    مضمن int32_t geti(ثابت UnicodeString& مفتاح) ثابت;

    مضمن int32_t getiAndFound(ثابت UnicodeString& مفتاح, UBool &found) ثابت;

    مضمن فراغ* remove(ثابت UnicodeString& مفتاح);

    مضمن int32_t removei(ثابت UnicodeString& مفتاح);

    مضمن فراغ removeAll();

    مضمن UBool contains_مفتاح(ثابت UnicodeString& مفتاح) ثابت;

    مضمن ثابت UHashElement* find(ثابت UnicodeString& مفتاح) ثابت;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    مضمن ثابت UHashElement* nextElement(int32_t& pos) ثابت;

    مضمن UKeyComparator* set_مفتاح_Comparator(UKeyComparator*مفتاح_Comp);

    مضمن UValueComparator* set_قيمة_Comparator(UValueComparator* قيمة_Comp);

    مضمن UBool equals(ثابت Hashtable& that) ثابت;
خاص:
    Hashtable(ثابت Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ثابت Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

مضمن فراغ Hashtable::init(UHashFunction *مفتاح_Hash, UKeyComparator *مفتاح_Comp,
                            UValueComparator *قيمة_Comp, UErrorCode& status) {
    إذا (U_FAILURE(status)) {
        إرجاع;
    }
    uhash_init(&hashObj, مفتاح_Hash, مفتاح_Comp, قيمة_Comp, &status);
    إذا (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_مفتاح_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن فراغ Hashtable::initSize(UHashFunction *مفتاح_Hash, UKeyComparator *مفتاح_Comp,
                                UValueComparator *قيمة_Comp, int32_t size, UErrorCode& status) {
    إذا (U_FAILURE(status)) {
        إرجاع;
    }
    uhash_initSize(&hashObj, مفتاح_Hash, مفتاح_Comp, قيمة_Comp, size, &status);
    إذا (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_مفتاح_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن Hashtable::Hashtable(UKeyComparator *مفتاح_Comp, UValueComparator *قيمة_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, مفتاح_Comp, قيمة_Comp, status);
}

مضمن Hashtable::Hashtable(UBool ignore_مفتاح_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_مفتاح_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_مفتاح_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

مضمن Hashtable::Hashtable(UBool ignore_مفتاح_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_مفتاح_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_مفتاح_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

مضمن Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

مضمن Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

مضمن Hashtable::~Hashtable() {
    إذا (hash != nullptr) {
        uhash_close(hash);
    }
}

مضمن UObjectDeleter *Hashtable::set_قيمة_Deleter(UObjectDeleter *fn) {
    إرجاع uhash_set_قيمة_Deleter(hash, fn);
}

مضمن int32_t Hashtable::تعداد_2() ثابت {
    إرجاع uhash_تعداد(hash);
}

مضمن فراغ* Hashtable::put(ثابت UnicodeString& مفتاح, فراغ* قيمة_2, UErrorCode& status) {
    إرجاع uhash_put(hash, new UnicodeString(مفتاح), قيمة_2, &status);
}

مضمن int32_t Hashtable::puti(ثابت UnicodeString& مفتاح, int32_t قيمة_2, UErrorCode& status) {
    إرجاع uhash_puti(hash, new UnicodeString(مفتاح), قيمة_2, &status);
}

مضمن int32_t Hashtable::putiAllowZero(ثابت UnicodeString& مفتاح, int32_t قيمة_2,
                                        UErrorCode& status) {
    إرجاع uhash_putiAllowZero(hash, new UnicodeString(مفتاح), قيمة_2, &status);
}

مضمن فراغ* Hashtable::get(ثابت UnicodeString& مفتاح) ثابت {
    إرجاع uhash_get(hash, &مفتاح);
}

مضمن int32_t Hashtable::geti(ثابت UnicodeString& مفتاح) ثابت {
    إرجاع uhash_geti(hash, &مفتاح);
}

مضمن int32_t Hashtable::getiAndFound(ثابت UnicodeString& مفتاح, UBool &found) ثابت {
    إرجاع uhash_getiAndFound(hash, &مفتاح, &found);
}

مضمن فراغ* Hashtable::remove(ثابت UnicodeString& مفتاح) {
    إرجاع uhash_remove(hash, &مفتاح);
}

مضمن int32_t Hashtable::removei(ثابت UnicodeString& مفتاح) {
    إرجاع uhash_removei(hash, &مفتاح);
}

مضمن UBool Hashtable::contains_مفتاح(ثابت UnicodeString& مفتاح) ثابت {
    إرجاع uhash_contains_مفتاح(hash, &مفتاح);
}

مضمن ثابت UHashElement* Hashtable::find(ثابت UnicodeString& مفتاح) ثابت {
    إرجاع uhash_find(hash, &مفتاح);
}

مضمن ثابت UHashElement* Hashtable::nextElement(int32_t& pos) ثابت {
    إرجاع uhash_nextElement(hash, &pos);
}

مضمن فراغ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

مضمن UKeyComparator* Hashtable::set_مفتاح_Comparator(UKeyComparator*مفتاح_Comp){
    إرجاع uhash_set_مفتاح_Comparator(hash, مفتاح_Comp);
}

مضمن UValueComparator* Hashtable::set_قيمة_Comparator(UValueComparator* قيمة_Comp){
    إرجاع uhash_set_قيمة_Comparator(hash, قيمة_Comp);
}

مضمن UBool Hashtable::equals(ثابت Hashtable& that)ثابت{
   إرجاع uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


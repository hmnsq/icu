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

    مضمن فراغ init(UHashFunction *مبودي_Hash, UKeyComparator *مبودي_Comp, UValueComparator *درج_Comp, UErrorCode& status);

    مضمن فراغ initSize(UHashFunction *مبودي_Hash, UKeyComparator *مبودي_Comp, UValueComparator *درج_Comp, int32_t size, UErrorCode& status);

عام:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_مبودي_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    مضمن Hashtable(UBool ignore_مبودي_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    مضمن Hashtable(UKeyComparator *مبودي_Comp, UValueComparator *درج_Comp, UErrorCode& status);

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

    مضمن UObjectDeleter *set_درج_Deleter(UObjectDeleter *fn);

    مضمن int32_t كدي() ثابت;

    مضمن فراغ* put(ثابت UnicodeString& مبودي, فراغ* درج_2, UErrorCode& status);

    مضمن int32_t puti(ثابت UnicodeString& مبودي, int32_t درج_2, UErrorCode& status);

    مضمن int32_t putiAllowZero(ثابت UnicodeString& مبودي, int32_t درج_2, UErrorCode& status);

    مضمن فراغ* get(ثابت UnicodeString& مبودي) ثابت;

    مضمن int32_t geti(ثابت UnicodeString& مبودي) ثابت;

    مضمن int32_t getiAndFound(ثابت UnicodeString& مبودي, UBool &found) ثابت;

    مضمن فراغ* remove(ثابت UnicodeString& مبودي);

    مضمن int32_t removei(ثابت UnicodeString& مبودي);

    مضمن فراغ removeAll();

    مضمن UBool contains_مبودي(ثابت UnicodeString& مبودي) ثابت;

    مضمن ثابت UHashElement* find(ثابت UnicodeString& مبودي) ثابت;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    مضمن ثابت UHashElement* nextElement(int32_t& pos) ثابت;

    مضمن UKeyComparator* set_مبودي_Comparator(UKeyComparator*مبودي_Comp);

    مضمن UValueComparator* set_درج_Comparator(UValueComparator* درج_Comp);

    مضمن UBool equals(ثابت Hashtable& that) ثابت;
خاص:
    Hashtable(ثابت Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ثابت Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

مضمن فراغ Hashtable::init(UHashFunction *مبودي_Hash, UKeyComparator *مبودي_Comp,
                            UValueComparator *درج_Comp, UErrorCode& status) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    uhash_init(&hashObj, مبودي_Hash, مبودي_Comp, درج_Comp, &status);
    إذا (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_مبودي_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن فراغ Hashtable::initSize(UHashFunction *مبودي_Hash, UKeyComparator *مبودي_Comp,
                                UValueComparator *درج_Comp, int32_t size, UErrorCode& status) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    uhash_initSize(&hashObj, مبودي_Hash, مبودي_Comp, درج_Comp, size, &status);
    إذا (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_مبودي_Deleter(hash, uprv_deleteUObject);
    }
}

مضمن Hashtable::Hashtable(UKeyComparator *مبودي_Comp, UValueComparator *درج_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, مبودي_Comp, درج_Comp, status);
}

مضمن Hashtable::Hashtable(UBool ignore_مبودي_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_مبودي_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_مبودي_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

مضمن Hashtable::Hashtable(UBool ignore_مبودي_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_مبودي_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_مبودي_Case ? uhash_compareCaselessUnicodeString
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

مضمن UObjectDeleter *Hashtable::set_درج_Deleter(UObjectDeleter *fn) {
    أرجع uhash_set_درج_Deleter(hash, fn);
}

مضمن int32_t Hashtable::كدي() ثابت {
    أرجع uhash_كدي(hash);
}

مضمن فراغ* Hashtable::put(ثابت UnicodeString& مبودي, فراغ* درج_2, UErrorCode& status) {
    أرجع uhash_put(hash, new UnicodeString(مبودي), درج_2, &status);
}

مضمن int32_t Hashtable::puti(ثابت UnicodeString& مبودي, int32_t درج_2, UErrorCode& status) {
    أرجع uhash_puti(hash, new UnicodeString(مبودي), درج_2, &status);
}

مضمن int32_t Hashtable::putiAllowZero(ثابت UnicodeString& مبودي, int32_t درج_2,
                                        UErrorCode& status) {
    أرجع uhash_putiAllowZero(hash, new UnicodeString(مبودي), درج_2, &status);
}

مضمن فراغ* Hashtable::get(ثابت UnicodeString& مبودي) ثابت {
    أرجع uhash_get(hash, &مبودي);
}

مضمن int32_t Hashtable::geti(ثابت UnicodeString& مبودي) ثابت {
    أرجع uhash_geti(hash, &مبودي);
}

مضمن int32_t Hashtable::getiAndFound(ثابت UnicodeString& مبودي, UBool &found) ثابت {
    أرجع uhash_getiAndFound(hash, &مبودي, &found);
}

مضمن فراغ* Hashtable::remove(ثابت UnicodeString& مبودي) {
    أرجع uhash_remove(hash, &مبودي);
}

مضمن int32_t Hashtable::removei(ثابت UnicodeString& مبودي) {
    أرجع uhash_removei(hash, &مبودي);
}

مضمن UBool Hashtable::contains_مبودي(ثابت UnicodeString& مبودي) ثابت {
    أرجع uhash_contains_مبودي(hash, &مبودي);
}

مضمن ثابت UHashElement* Hashtable::find(ثابت UnicodeString& مبودي) ثابت {
    أرجع uhash_find(hash, &مبودي);
}

مضمن ثابت UHashElement* Hashtable::nextElement(int32_t& pos) ثابت {
    أرجع uhash_nextElement(hash, &pos);
}

مضمن فراغ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

مضمن UKeyComparator* Hashtable::set_مبودي_Comparator(UKeyComparator*مبودي_Comp){
    أرجع uhash_set_مبودي_Comparator(hash, مبودي_Comp);
}

مضمن UValueComparator* Hashtable::set_درج_Comparator(UValueComparator* درج_Comp){
    أرجع uhash_set_درج_Comparator(hash, درج_Comp);
}

مضمن UBool Hashtable::equals(ثابت Hashtable& that)ثابت{
   أرجع uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


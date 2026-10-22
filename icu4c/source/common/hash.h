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

    مضمن فراغ init(UHashFunction *keyHash, UKeyComparator *keyComp, UValueComparator *valueComp, UErrorCode& status);

    مضمن فراغ initSize(UHashFunction *keyHash, UKeyComparator *keyComp, UValueComparator *valueComp, int32_t size, UErrorCode& status);

عام:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    مضمن Hashtable(UBool ignoreKeyCase, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    مضمن Hashtable(UBool ignoreKeyCase, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    مضمن Hashtable(UKeyComparator *keyComp, UValueComparator *valueComp, UErrorCode& status);

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

    مضمن UObjectDeleter *setValueDeleter(UObjectDeleter *fn);

    مضمن int32_t count() ثابت;

    مضمن فراغ* put(ثابت UnicodeString& key, فراغ* value, UErrorCode& status);

    مضمن int32_t puti(ثابت UnicodeString& key, int32_t value, UErrorCode& status);

    مضمن int32_t putiAllowZero(ثابت UnicodeString& key, int32_t value, UErrorCode& status);

    مضمن فراغ* get(ثابت UnicodeString& key) ثابت;

    مضمن int32_t geti(ثابت UnicodeString& key) ثابت;

    مضمن int32_t getiAndFound(ثابت UnicodeString& key, UBool &found) ثابت;

    مضمن فراغ* remove(ثابت UnicodeString& key);

    مضمن int32_t removei(ثابت UnicodeString& key);

    مضمن فراغ removeAll();

    مضمن UBool containsKey(ثابت UnicodeString& key) ثابت;

    مضمن ثابت UHashElement* find(ثابت UnicodeString& key) ثابت;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    مضمن ثابت UHashElement* nextElement(int32_t& pos) ثابت;

    مضمن UKeyComparator* setKeyComparator(UKeyComparator*keyComp);

    مضمن UValueComparator* setValueComparator(UValueComparator* valueComp);

    مضمن UBool equals(ثابت Hashtable& that) ثابت;
خاص:
    Hashtable(ثابت Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ثابت Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

مضمن فراغ Hashtable::init(UHashFunction *keyHash, UKeyComparator *keyComp,
                            UValueComparator *valueComp, UErrorCode& status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    uhash_init(&hashObj, keyHash, keyComp, valueComp, &status);
    ܐܢ (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_setKeyDeleter(hash, uprv_deleteUObject);
    }
}

مضمن فراغ Hashtable::initSize(UHashFunction *keyHash, UKeyComparator *keyComp,
                                UValueComparator *valueComp, int32_t size, UErrorCode& status) {
    ܐܢ (U_FAILURE(status)) {
        ܦܢܐ;
    }
    uhash_initSize(&hashObj, keyHash, keyComp, valueComp, size, &status);
    ܐܢ (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_setKeyDeleter(hash, uprv_deleteUObject);
    }
}

مضمن Hashtable::Hashtable(UKeyComparator *keyComp, UValueComparator *valueComp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, keyComp, valueComp, status);
}

مضمن Hashtable::Hashtable(UBool ignoreKeyCase, UErrorCode& status)
 : hash(nullptr)
{
    init(ignoreKeyCase ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignoreKeyCase ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

مضمن Hashtable::Hashtable(UBool ignoreKeyCase, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignoreKeyCase ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignoreKeyCase ? uhash_compareCaselessUnicodeString
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
    ܐܢ (hash != nullptr) {
        uhash_close(hash);
    }
}

مضمن UObjectDeleter *Hashtable::setValueDeleter(UObjectDeleter *fn) {
    ܦܢܐ uhash_setValueDeleter(hash, fn);
}

مضمن int32_t Hashtable::count() ثابت {
    ܦܢܐ uhash_count(hash);
}

مضمن فراغ* Hashtable::put(ثابت UnicodeString& key, فراغ* value, UErrorCode& status) {
    ܦܢܐ uhash_put(hash, new UnicodeString(key), value, &status);
}

مضمن int32_t Hashtable::puti(ثابت UnicodeString& key, int32_t value, UErrorCode& status) {
    ܦܢܐ uhash_puti(hash, new UnicodeString(key), value, &status);
}

مضمن int32_t Hashtable::putiAllowZero(ثابت UnicodeString& key, int32_t value,
                                        UErrorCode& status) {
    ܦܢܐ uhash_putiAllowZero(hash, new UnicodeString(key), value, &status);
}

مضمن فراغ* Hashtable::get(ثابت UnicodeString& key) ثابت {
    ܦܢܐ uhash_get(hash, &key);
}

مضمن int32_t Hashtable::geti(ثابت UnicodeString& key) ثابت {
    ܦܢܐ uhash_geti(hash, &key);
}

مضمن int32_t Hashtable::getiAndFound(ثابت UnicodeString& key, UBool &found) ثابت {
    ܦܢܐ uhash_getiAndFound(hash, &key, &found);
}

مضمن فراغ* Hashtable::remove(ثابت UnicodeString& key) {
    ܦܢܐ uhash_remove(hash, &key);
}

مضمن int32_t Hashtable::removei(ثابت UnicodeString& key) {
    ܦܢܐ uhash_removei(hash, &key);
}

مضمن UBool Hashtable::containsKey(ثابت UnicodeString& key) ثابت {
    ܦܢܐ uhash_containsKey(hash, &key);
}

مضمن ثابت UHashElement* Hashtable::find(ثابت UnicodeString& key) ثابت {
    ܦܢܐ uhash_find(hash, &key);
}

مضمن ثابت UHashElement* Hashtable::nextElement(int32_t& pos) ثابت {
    ܦܢܐ uhash_nextElement(hash, &pos);
}

مضمن فراغ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

مضمن UKeyComparator* Hashtable::setKeyComparator(UKeyComparator*keyComp){
    ܦܢܐ uhash_setKeyComparator(hash, keyComp);
}

مضمن UValueComparator* Hashtable::setValueComparator(UValueComparator* valueComp){
    ܦܢܐ uhash_setValueComparator(hash, valueComp);
}

مضمن UBool Hashtable::equals(ثابت Hashtable& that)ثابت{
   ܦܢܐ uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


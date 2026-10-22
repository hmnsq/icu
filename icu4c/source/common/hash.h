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

#include "unicode/unistr.h"
#include "unicode/uobject.h"
#include "cmemory.h"
#include "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : public UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    anta_hstha void init(UHashFunction *keyHash, UKeyComparator *keyComp, UValueComparator *valueComp, UErrorCode& status);

    anta_hstha void initSize(UHashFunction *keyHash, UKeyComparator *keyComp, UValueComparator *valueComp, int32_t size, UErrorCode& status);

public:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    anta_hstha Hashtable(UBool ignoreKeyCase, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    anta_hstha Hashtable(UBool ignoreKeyCase, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    anta_hstha Hashtable(UKeyComparator *keyComp, UValueComparator *valueComp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    anta_hstha Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    anta_hstha Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    anta_hstha ~Hashtable();

    anta_hstha UObjectDeleter *setValueDeleter(UObjectDeleter *fn);

    anta_hstha int32_t count() const;

    anta_hstha void* put(const UnicodeString& key, void* value, UErrorCode& status);

    anta_hstha int32_t puti(const UnicodeString& key, int32_t value, UErrorCode& status);

    anta_hstha int32_t putiAllowZero(const UnicodeString& key, int32_t value, UErrorCode& status);

    anta_hstha void* get(const UnicodeString& key) const;

    anta_hstha int32_t geti(const UnicodeString& key) const;

    anta_hstha int32_t getiAndFound(const UnicodeString& key, UBool &found) const;

    anta_hstha void* remove(const UnicodeString& key);

    anta_hstha int32_t removei(const UnicodeString& key);

    anta_hstha void removeAll();

    anta_hstha UBool containsKey(const UnicodeString& key) const;

    anta_hstha const UHashElement* find(const UnicodeString& key) const;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    anta_hstha const UHashElement* nextElement(int32_t& pos) const;

    anta_hstha UKeyComparator* setKeyComparator(UKeyComparator*keyComp);

    anta_hstha UValueComparator* setValueComparator(UValueComparator* valueComp);

    anta_hstha UBool equals(const Hashtable& that) const;
private:
    Hashtable(const Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(const Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

anta_hstha void Hashtable::init(UHashFunction *keyHash, UKeyComparator *keyComp,
                            UValueComparator *valueComp, UErrorCode& status) {
    အကယ်၍ (U_FAILURE(status)) {
        ပြန်ပေး;
    }
    uhash_init(&hashObj, keyHash, keyComp, valueComp, &status);
    အကယ်၍ (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_setKeyDeleter(hash, uprv_deleteUObject);
    }
}

anta_hstha void Hashtable::initSize(UHashFunction *keyHash, UKeyComparator *keyComp,
                                UValueComparator *valueComp, int32_t size, UErrorCode& status) {
    အကယ်၍ (U_FAILURE(status)) {
        ပြန်ပေး;
    }
    uhash_initSize(&hashObj, keyHash, keyComp, valueComp, size, &status);
    အကယ်၍ (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_setKeyDeleter(hash, uprv_deleteUObject);
    }
}

anta_hstha Hashtable::Hashtable(UKeyComparator *keyComp, UValueComparator *valueComp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, keyComp, valueComp, status);
}

anta_hstha Hashtable::Hashtable(UBool ignoreKeyCase, UErrorCode& status)
 : hash(nullptr)
{
    init(ignoreKeyCase ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignoreKeyCase ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

anta_hstha Hashtable::Hashtable(UBool ignoreKeyCase, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignoreKeyCase ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignoreKeyCase ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

anta_hstha Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

anta_hstha Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

anta_hstha Hashtable::~Hashtable() {
    အကယ်၍ (hash != nullptr) {
        uhash_close(hash);
    }
}

anta_hstha UObjectDeleter *Hashtable::setValueDeleter(UObjectDeleter *fn) {
    ပြန်ပေး uhash_setValueDeleter(hash, fn);
}

anta_hstha int32_t Hashtable::count() const {
    ပြန်ပေး uhash_count(hash);
}

anta_hstha void* Hashtable::put(const UnicodeString& key, void* value, UErrorCode& status) {
    ပြန်ပေး uhash_put(hash, new UnicodeString(key), value, &status);
}

anta_hstha int32_t Hashtable::puti(const UnicodeString& key, int32_t value, UErrorCode& status) {
    ပြန်ပေး uhash_puti(hash, new UnicodeString(key), value, &status);
}

anta_hstha int32_t Hashtable::putiAllowZero(const UnicodeString& key, int32_t value,
                                        UErrorCode& status) {
    ပြန်ပေး uhash_putiAllowZero(hash, new UnicodeString(key), value, &status);
}

anta_hstha void* Hashtable::get(const UnicodeString& key) const {
    ပြန်ပေး uhash_get(hash, &key);
}

anta_hstha int32_t Hashtable::geti(const UnicodeString& key) const {
    ပြန်ပေး uhash_geti(hash, &key);
}

anta_hstha int32_t Hashtable::getiAndFound(const UnicodeString& key, UBool &found) const {
    ပြန်ပေး uhash_getiAndFound(hash, &key, &found);
}

anta_hstha void* Hashtable::remove(const UnicodeString& key) {
    ပြန်ပေး uhash_remove(hash, &key);
}

anta_hstha int32_t Hashtable::removei(const UnicodeString& key) {
    ပြန်ပေး uhash_removei(hash, &key);
}

anta_hstha UBool Hashtable::containsKey(const UnicodeString& key) const {
    ပြန်ပေး uhash_containsKey(hash, &key);
}

anta_hstha const UHashElement* Hashtable::find(const UnicodeString& key) const {
    ပြန်ပေး uhash_find(hash, &key);
}

anta_hstha const UHashElement* Hashtable::nextElement(int32_t& pos) const {
    ပြန်ပေး uhash_nextElement(hash, &pos);
}

anta_hstha void Hashtable::removeAll() {
    uhash_removeAll(hash);
}

anta_hstha UKeyComparator* Hashtable::setKeyComparator(UKeyComparator*keyComp){
    ပြန်ပေး uhash_setKeyComparator(hash, keyComp);
}

anta_hstha UValueComparator* Hashtable::setValueComparator(UValueComparator* valueComp){
    ပြန်ပေး uhash_setValueComparator(hash, valueComp);
}

anta_hstha UBool Hashtable::equals(const Hashtable& that)const{
   ပြန်ပေး uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


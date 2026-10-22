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

#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : সার্ৱজনিক UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    অন্তঃস্থ শূন্য init(UHashFunction *চাবি_Hash, UKeyComparator *চাবি_Comp, UValueComparator *মান_Comp, UErrorCode& status);

    অন্তঃস্থ শূন্য initSize(UHashFunction *চাবি_Hash, UKeyComparator *চাবি_Comp, UValueComparator *মান_Comp, int32_t size, UErrorCode& status);

সার্ৱজনিক:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    অন্তঃস্থ Hashtable(UBool ignore_চাবি_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    অন্তঃস্থ Hashtable(UBool ignore_চাবি_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    অন্তঃস্থ Hashtable(UKeyComparator *চাবি_Comp, UValueComparator *মান_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    অন্তঃস্থ Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    অন্তঃস্থ Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    অন্তঃস্থ ~Hashtable();

    অন্তঃস্থ UObjectDeleter *set_মান_Deleter(UObjectDeleter *fn);

    অন্তঃস্থ int32_t গুনতি() ধ্রুৱ;

    অন্তঃস্থ শূন্য* put(ধ্রুৱ UnicodeString& চাবি, শূন্য* মান_2, UErrorCode& status);

    অন্তঃস্থ int32_t puti(ধ্রুৱ UnicodeString& চাবি, int32_t মান_2, UErrorCode& status);

    অন্তঃস্থ int32_t putiAllowZero(ধ্রুৱ UnicodeString& চাবি, int32_t মান_2, UErrorCode& status);

    অন্তঃস্থ শূন্য* get(ধ্রুৱ UnicodeString& চাবি) ধ্রুৱ;

    অন্তঃস্থ int32_t geti(ধ্রুৱ UnicodeString& চাবি) ধ্রুৱ;

    অন্তঃস্থ int32_t getiAndFound(ধ্রুৱ UnicodeString& চাবি, UBool &found) ধ্রুৱ;

    অন্তঃস্থ শূন্য* remove(ধ্রুৱ UnicodeString& চাবি);

    অন্তঃস্থ int32_t removei(ধ্রুৱ UnicodeString& চাবি);

    অন্তঃস্থ শূন্য removeAll();

    অন্তঃস্থ UBool contains_চাবি(ধ্রুৱ UnicodeString& চাবি) ধ্রুৱ;

    অন্তঃস্থ ধ্রুৱ UHashElement* find(ধ্রুৱ UnicodeString& চাবি) ধ্রুৱ;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    অন্তঃস্থ ধ্রুৱ UHashElement* nextElement(int32_t& pos) ধ্রুৱ;

    অন্তঃস্থ UKeyComparator* set_চাবি_Comparator(UKeyComparator*চাবি_Comp);

    অন্তঃস্থ UValueComparator* set_মান_Comparator(UValueComparator* মান_Comp);

    অন্তঃস্থ UBool equals(ধ্রুৱ Hashtable& that) ধ্রুৱ;
গুপ্ত:
    Hashtable(ধ্রুৱ Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ধ্রুৱ Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

অন্তঃস্থ শূন্য Hashtable::init(UHashFunction *চাবি_Hash, UKeyComparator *চাবি_Comp,
                            UValueComparator *মান_Comp, UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    uhash_init(&hashObj, চাবি_Hash, চাবি_Comp, মান_Comp, &status);
    যদি (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_চাবি_Deleter(hash, uprv_deleteUObject);
    }
}

অন্তঃস্থ শূন্য Hashtable::initSize(UHashFunction *চাবি_Hash, UKeyComparator *চাবি_Comp,
                                UValueComparator *মান_Comp, int32_t size, UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ঘূৰাই;
    }
    uhash_initSize(&hashObj, চাবি_Hash, চাবি_Comp, মান_Comp, size, &status);
    যদি (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_চাবি_Deleter(hash, uprv_deleteUObject);
    }
}

অন্তঃস্থ Hashtable::Hashtable(UKeyComparator *চাবি_Comp, UValueComparator *মান_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, চাবি_Comp, মান_Comp, status);
}

অন্তঃস্থ Hashtable::Hashtable(UBool ignore_চাবি_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_চাবি_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_চাবি_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

অন্তঃস্থ Hashtable::Hashtable(UBool ignore_চাবি_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_চাবি_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_চাবি_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

অন্তঃস্থ Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

অন্তঃস্থ Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

অন্তঃস্থ Hashtable::~Hashtable() {
    যদি (hash != nullptr) {
        uhash_close(hash);
    }
}

অন্তঃস্থ UObjectDeleter *Hashtable::set_মান_Deleter(UObjectDeleter *fn) {
    ঘূৰাই uhash_set_মান_Deleter(hash, fn);
}

অন্তঃস্থ int32_t Hashtable::গুনতি() ধ্রুৱ {
    ঘূৰাই uhash_গুনতি(hash);
}

অন্তঃস্থ শূন্য* Hashtable::put(ধ্রুৱ UnicodeString& চাবি, শূন্য* মান_2, UErrorCode& status) {
    ঘূৰাই uhash_put(hash, new UnicodeString(চাবি), মান_2, &status);
}

অন্তঃস্থ int32_t Hashtable::puti(ধ্রুৱ UnicodeString& চাবি, int32_t মান_2, UErrorCode& status) {
    ঘূৰাই uhash_puti(hash, new UnicodeString(চাবি), মান_2, &status);
}

অন্তঃস্থ int32_t Hashtable::putiAllowZero(ধ্রুৱ UnicodeString& চাবি, int32_t মান_2,
                                        UErrorCode& status) {
    ঘূৰাই uhash_putiAllowZero(hash, new UnicodeString(চাবি), মান_2, &status);
}

অন্তঃস্থ শূন্য* Hashtable::get(ধ্রুৱ UnicodeString& চাবি) ধ্রুৱ {
    ঘূৰাই uhash_get(hash, &চাবি);
}

অন্তঃস্থ int32_t Hashtable::geti(ধ্রুৱ UnicodeString& চাবি) ধ্রুৱ {
    ঘূৰাই uhash_geti(hash, &চাবি);
}

অন্তঃস্থ int32_t Hashtable::getiAndFound(ধ্রুৱ UnicodeString& চাবি, UBool &found) ধ্রুৱ {
    ঘূৰাই uhash_getiAndFound(hash, &চাবি, &found);
}

অন্তঃস্থ শূন্য* Hashtable::remove(ধ্রুৱ UnicodeString& চাবি) {
    ঘূৰাই uhash_remove(hash, &চাবি);
}

অন্তঃস্থ int32_t Hashtable::removei(ধ্রুৱ UnicodeString& চাবি) {
    ঘূৰাই uhash_removei(hash, &চাবি);
}

অন্তঃস্থ UBool Hashtable::contains_চাবি(ধ্রুৱ UnicodeString& চাবি) ধ্রুৱ {
    ঘূৰাই uhash_contains_চাবি(hash, &চাবি);
}

অন্তঃস্থ ধ্রুৱ UHashElement* Hashtable::find(ধ্রুৱ UnicodeString& চাবি) ধ্রুৱ {
    ঘূৰাই uhash_find(hash, &চাবি);
}

অন্তঃস্থ ধ্রুৱ UHashElement* Hashtable::nextElement(int32_t& pos) ধ্রুৱ {
    ঘূৰাই uhash_nextElement(hash, &pos);
}

অন্তঃস্থ শূন্য Hashtable::removeAll() {
    uhash_removeAll(hash);
}

অন্তঃস্থ UKeyComparator* Hashtable::set_চাবি_Comparator(UKeyComparator*চাবি_Comp){
    ঘূৰাই uhash_set_চাবি_Comparator(hash, চাবি_Comp);
}

অন্তঃস্থ UValueComparator* Hashtable::set_মান_Comparator(UValueComparator* মান_Comp){
    ঘূৰাই uhash_set_মান_Comparator(hash, মান_Comp);
}

অন্তঃস্থ UBool Hashtable::equals(ধ্রুৱ Hashtable& that)ধ্রুৱ{
   ঘূৰাই uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


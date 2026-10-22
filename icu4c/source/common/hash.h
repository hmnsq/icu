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

#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : সর্বজনীন UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    অন্তঃরেখ শূন্য init(UHashFunction *চাবি_Hash, UKeyComparator *চাবি_Comp, UValueComparator *মান_Comp, UErrorCode& status);

    অন্তঃরেখ শূন্য init_আকার(UHashFunction *চাবি_Hash, UKeyComparator *চাবি_Comp, UValueComparator *মান_Comp, int32_t আকার_3, UErrorCode& status);

সর্বজনীন:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    অন্তঃরেখ Hashtable(UBool ignore_চাবি_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    অন্তঃরেখ Hashtable(UBool ignore_চাবি_Case, int32_t আকার_3, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    অন্তঃরেখ Hashtable(UKeyComparator *চাবি_Comp, UValueComparator *মান_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    অন্তঃরেখ Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    অন্তঃরেখ Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    অন্তঃরেখ ~Hashtable();

    অন্তঃরেখ UObjectDeleter *set_মান_Deleter(UObjectDeleter *fn);

    অন্তঃরেখ int32_t গুনতি() ধ্রুবক;

    অন্তঃরেখ শূন্য* put(ধ্রুবক UnicodeString& চাবি, শূন্য* মান_2, UErrorCode& status);

    অন্তঃরেখ int32_t puti(ধ্রুবক UnicodeString& চাবি, int32_t মান_2, UErrorCode& status);

    অন্তঃরেখ int32_t putiAllowZero(ধ্রুবক UnicodeString& চাবি, int32_t মান_2, UErrorCode& status);

    অন্তঃরেখ শূন্য* get(ধ্রুবক UnicodeString& চাবি) ধ্রুবক;

    অন্তঃরেখ int32_t geti(ধ্রুবক UnicodeString& চাবি) ধ্রুবক;

    অন্তঃরেখ int32_t getiAndFound(ধ্রুবক UnicodeString& চাবি, UBool &found) ধ্রুবক;

    অন্তঃরেখ শূন্য* remove(ধ্রুবক UnicodeString& চাবি);

    অন্তঃরেখ int32_t removei(ধ্রুবক UnicodeString& চাবি);

    অন্তঃরেখ শূন্য removeAll();

    অন্তঃরেখ UBool contains_চাবি(ধ্রুবক UnicodeString& চাবি) ধ্রুবক;

    অন্তঃরেখ ধ্রুবক UHashElement* find(ধ্রুবক UnicodeString& চাবি) ধ্রুবক;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    অন্তঃরেখ ধ্রুবক UHashElement* পরবর্তী_Element(int32_t& pos) ধ্রুবক;

    অন্তঃরেখ UKeyComparator* set_চাবি_Comparator(UKeyComparator*চাবি_Comp);

    অন্তঃরেখ UValueComparator* set_মান_Comparator(UValueComparator* মান_Comp);

    অন্তঃরেখ UBool equals(ধ্রুবক Hashtable& that) ধ্রুবক;
ব্যক্তিগত:
    Hashtable(ধ্রুবক Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ধ্রুবক Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

অন্তঃরেখ শূন্য Hashtable::init(UHashFunction *চাবি_Hash, UKeyComparator *চাবি_Comp,
                            UValueComparator *মান_Comp, UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    uhash_init(&hashObj, চাবি_Hash, চাবি_Comp, মান_Comp, &status);
    যদি (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_চাবি_Deleter(hash, uprv_deleteUObject);
    }
}

অন্তঃরেখ শূন্য Hashtable::init_আকার(UHashFunction *চাবি_Hash, UKeyComparator *চাবি_Comp,
                                UValueComparator *মান_Comp, int32_t আকার_3, UErrorCode& status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    uhash_init_আকার(&hashObj, চাবি_Hash, চাবি_Comp, মান_Comp, আকার_3, &status);
    যদি (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_চাবি_Deleter(hash, uprv_deleteUObject);
    }
}

অন্তঃরেখ Hashtable::Hashtable(UKeyComparator *চাবি_Comp, UValueComparator *মান_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, চাবি_Comp, মান_Comp, status);
}

অন্তঃরেখ Hashtable::Hashtable(UBool ignore_চাবি_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_চাবি_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_চাবি_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

অন্তঃরেখ Hashtable::Hashtable(UBool ignore_চাবি_Case, int32_t আকার_3, UErrorCode& status)
 : hash(nullptr)
{
    init_আকার(ignore_চাবি_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_চাবি_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, আকার_3,
            status);
}

অন্তঃরেখ Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

অন্তঃরেখ Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

অন্তঃরেখ Hashtable::~Hashtable() {
    যদি (hash != nullptr) {
        uhash_close(hash);
    }
}

অন্তঃরেখ UObjectDeleter *Hashtable::set_মান_Deleter(UObjectDeleter *fn) {
    ফেরত uhash_set_মান_Deleter(hash, fn);
}

অন্তঃরেখ int32_t Hashtable::গুনতি() ধ্রুবক {
    ফেরত uhash_গুনতি(hash);
}

অন্তঃরেখ শূন্য* Hashtable::put(ধ্রুবক UnicodeString& চাবি, শূন্য* মান_2, UErrorCode& status) {
    ফেরত uhash_put(hash, new UnicodeString(চাবি), মান_2, &status);
}

অন্তঃরেখ int32_t Hashtable::puti(ধ্রুবক UnicodeString& চাবি, int32_t মান_2, UErrorCode& status) {
    ফেরত uhash_puti(hash, new UnicodeString(চাবি), মান_2, &status);
}

অন্তঃরেখ int32_t Hashtable::putiAllowZero(ধ্রুবক UnicodeString& চাবি, int32_t মান_2,
                                        UErrorCode& status) {
    ফেরত uhash_putiAllowZero(hash, new UnicodeString(চাবি), মান_2, &status);
}

অন্তঃরেখ শূন্য* Hashtable::get(ধ্রুবক UnicodeString& চাবি) ধ্রুবক {
    ফেরত uhash_get(hash, &চাবি);
}

অন্তঃরেখ int32_t Hashtable::geti(ধ্রুবক UnicodeString& চাবি) ধ্রুবক {
    ফেরত uhash_geti(hash, &চাবি);
}

অন্তঃরেখ int32_t Hashtable::getiAndFound(ধ্রুবক UnicodeString& চাবি, UBool &found) ধ্রুবক {
    ফেরত uhash_getiAndFound(hash, &চাবি, &found);
}

অন্তঃরেখ শূন্য* Hashtable::remove(ধ্রুবক UnicodeString& চাবি) {
    ফেরত uhash_remove(hash, &চাবি);
}

অন্তঃরেখ int32_t Hashtable::removei(ধ্রুবক UnicodeString& চাবি) {
    ফেরত uhash_removei(hash, &চাবি);
}

অন্তঃরেখ UBool Hashtable::contains_চাবি(ধ্রুবক UnicodeString& চাবি) ধ্রুবক {
    ফেরত uhash_contains_চাবি(hash, &চাবি);
}

অন্তঃরেখ ধ্রুবক UHashElement* Hashtable::find(ধ্রুবক UnicodeString& চাবি) ধ্রুবক {
    ফেরত uhash_find(hash, &চাবি);
}

অন্তঃরেখ ধ্রুবক UHashElement* Hashtable::পরবর্তী_Element(int32_t& pos) ধ্রুবক {
    ফেরত uhash_পরবর্তী_Element(hash, &pos);
}

অন্তঃরেখ শূন্য Hashtable::removeAll() {
    uhash_removeAll(hash);
}

অন্তঃরেখ UKeyComparator* Hashtable::set_চাবি_Comparator(UKeyComparator*চাবি_Comp){
    ফেরত uhash_set_চাবি_Comparator(hash, চাবি_Comp);
}

অন্তঃরেখ UValueComparator* Hashtable::set_মান_Comparator(UValueComparator* মান_Comp){
    ফেরত uhash_set_মান_Comparator(hash, মান_Comp);
}

অন্তঃরেখ UBool Hashtable::equals(ধ্রুবক Hashtable& that)ধ্রুবক{
   ফেরত uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


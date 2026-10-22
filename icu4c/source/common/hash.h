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

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : सार्वजनिक UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    ओळीत शून्य init(UHashFunction *किल्ली_Hash, UKeyComparator *किल्ली_Comp, UValueComparator *मूल्य_Comp, UErrorCode& status);

    ओळीत शून्य initSize(UHashFunction *किल्ली_Hash, UKeyComparator *किल्ली_Comp, UValueComparator *मूल्य_Comp, int32_t size, UErrorCode& status);

सार्वजनिक:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    ओळीत Hashtable(UBool ignore_किल्ली_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    ओळीत Hashtable(UBool ignore_किल्ली_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    ओळीत Hashtable(UKeyComparator *किल्ली_Comp, UValueComparator *मूल्य_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    ओळीत Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    ओळीत Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    ओळीत ~Hashtable();

    ओळीत UObjectDeleter *set_मूल्य_Deleter(UObjectDeleter *fn);

    ओळीत int32_t मोजणी() स्थिरांक;

    ओळीत शून्य* put(स्थिरांक UnicodeString& किल्ली, शून्य* मूल्य_2, UErrorCode& status);

    ओळीत int32_t puti(स्थिरांक UnicodeString& किल्ली, int32_t मूल्य_2, UErrorCode& status);

    ओळीत int32_t putiAllowZero(स्थिरांक UnicodeString& किल्ली, int32_t मूल्य_2, UErrorCode& status);

    ओळीत शून्य* get(स्थिरांक UnicodeString& किल्ली) स्थिरांक;

    ओळीत int32_t geti(स्थिरांक UnicodeString& किल्ली) स्थिरांक;

    ओळीत int32_t getiAndFound(स्थिरांक UnicodeString& किल्ली, UBool &found) स्थिरांक;

    ओळीत शून्य* हटाओ(स्थिरांक UnicodeString& किल्ली);

    ओळीत int32_t removei(स्थिरांक UnicodeString& किल्ली);

    ओळीत शून्य removeAll();

    ओळीत UBool contains_किल्ली(स्थिरांक UnicodeString& किल्ली) स्थिरांक;

    ओळीत स्थिरांक UHashElement* find(स्थिरांक UnicodeString& किल्ली) स्थिरांक;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    ओळीत स्थिरांक UHashElement* nextElement(int32_t& pos) स्थिरांक;

    ओळीत UKeyComparator* set_किल्ली_Comparator(UKeyComparator*किल्ली_Comp);

    ओळीत UValueComparator* set_मूल्य_Comparator(UValueComparator* मूल्य_Comp);

    ओळीत UBool equals(स्थिरांक Hashtable& that) स्थिरांक;
खाजगी:
    Hashtable(स्थिरांक Hashtable &other) = delete; // forbid copying of this class
    Hashtable &चालक=(स्थिरांक Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

ओळीत शून्य Hashtable::init(UHashFunction *किल्ली_Hash, UKeyComparator *किल्ली_Comp,
                            UValueComparator *मूल्य_Comp, UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत;
    }
    uhash_init(&hashObj, किल्ली_Hash, किल्ली_Comp, मूल्य_Comp, &status);
    जर (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_किल्ली_Deleter(hash, uprv_deleteUObject);
    }
}

ओळीत शून्य Hashtable::initSize(UHashFunction *किल्ली_Hash, UKeyComparator *किल्ली_Comp,
                                UValueComparator *मूल्य_Comp, int32_t size, UErrorCode& status) {
    जर (U_FAILURE(status)) {
        परत;
    }
    uhash_initSize(&hashObj, किल्ली_Hash, किल्ली_Comp, मूल्य_Comp, size, &status);
    जर (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_किल्ली_Deleter(hash, uprv_deleteUObject);
    }
}

ओळीत Hashtable::Hashtable(UKeyComparator *किल्ली_Comp, UValueComparator *मूल्य_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, किल्ली_Comp, मूल्य_Comp, status);
}

ओळीत Hashtable::Hashtable(UBool ignore_किल्ली_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_किल्ली_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_किल्ली_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

ओळीत Hashtable::Hashtable(UBool ignore_किल्ली_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_किल्ली_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_किल्ली_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

ओळीत Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

ओळीत Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

ओळीत Hashtable::~Hashtable() {
    जर (hash != nullptr) {
        uhash_close(hash);
    }
}

ओळीत UObjectDeleter *Hashtable::set_मूल्य_Deleter(UObjectDeleter *fn) {
    परत uhash_set_मूल्य_Deleter(hash, fn);
}

ओळीत int32_t Hashtable::मोजणी() स्थिरांक {
    परत uhash_मोजणी(hash);
}

ओळीत शून्य* Hashtable::put(स्थिरांक UnicodeString& किल्ली, शून्य* मूल्य_2, UErrorCode& status) {
    परत uhash_put(hash, new UnicodeString(किल्ली), मूल्य_2, &status);
}

ओळीत int32_t Hashtable::puti(स्थिरांक UnicodeString& किल्ली, int32_t मूल्य_2, UErrorCode& status) {
    परत uhash_puti(hash, new UnicodeString(किल्ली), मूल्य_2, &status);
}

ओळीत int32_t Hashtable::putiAllowZero(स्थिरांक UnicodeString& किल्ली, int32_t मूल्य_2,
                                        UErrorCode& status) {
    परत uhash_putiAllowZero(hash, new UnicodeString(किल्ली), मूल्य_2, &status);
}

ओळीत शून्य* Hashtable::get(स्थिरांक UnicodeString& किल्ली) स्थिरांक {
    परत uhash_get(hash, &किल्ली);
}

ओळीत int32_t Hashtable::geti(स्थिरांक UnicodeString& किल्ली) स्थिरांक {
    परत uhash_geti(hash, &किल्ली);
}

ओळीत int32_t Hashtable::getiAndFound(स्थिरांक UnicodeString& किल्ली, UBool &found) स्थिरांक {
    परत uhash_getiAndFound(hash, &किल्ली, &found);
}

ओळीत शून्य* Hashtable::हटाओ(स्थिरांक UnicodeString& किल्ली) {
    परत uhash_remove(hash, &किल्ली);
}

ओळीत int32_t Hashtable::removei(स्थिरांक UnicodeString& किल्ली) {
    परत uhash_removei(hash, &किल्ली);
}

ओळीत UBool Hashtable::contains_किल्ली(स्थिरांक UnicodeString& किल्ली) स्थिरांक {
    परत uhash_contains_किल्ली(hash, &किल्ली);
}

ओळीत स्थिरांक UHashElement* Hashtable::find(स्थिरांक UnicodeString& किल्ली) स्थिरांक {
    परत uhash_find(hash, &किल्ली);
}

ओळीत स्थिरांक UHashElement* Hashtable::nextElement(int32_t& pos) स्थिरांक {
    परत uhash_nextElement(hash, &pos);
}

ओळीत शून्य Hashtable::removeAll() {
    uhash_removeAll(hash);
}

ओळीत UKeyComparator* Hashtable::set_किल्ली_Comparator(UKeyComparator*किल्ली_Comp){
    परत uhash_set_किल्ली_Comparator(hash, किल्ली_Comp);
}

ओळीत UValueComparator* Hashtable::set_मूल्य_Comparator(UValueComparator* मूल्य_Comp){
    परत uhash_set_मूल्य_Comparator(hash, मूल्य_Comp);
}

ओळीत UBool Hashtable::equals(स्थिरांक Hashtable& that)स्थिरांक{
   परत uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


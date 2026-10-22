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

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : ಸಾರ್ವಜನಿಕ UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ init(UHashFunction *ಕೀಲಿ_Hash, UKeyComparator *ಕೀಲಿ_Comp, UValueComparator *ಮೌಲ್ಯ_Comp, UErrorCode& status);

    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ initSize(UHashFunction *ಕೀಲಿ_Hash, UKeyComparator *ಕೀಲಿ_Comp, UValueComparator *ಮೌಲ್ಯ_Comp, int32_t size, UErrorCode& status);

ಸಾರ್ವಜನಿಕ:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    ಅನ್ತಃಸ್ಥ Hashtable(UBool ignore_ಕೀಲಿ_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    ಅನ್ತಃಸ್ಥ Hashtable(UBool ignore_ಕೀಲಿ_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    ಅನ್ತಃಸ್ಥ Hashtable(UKeyComparator *ಕೀಲಿ_Comp, UValueComparator *ಮೌಲ್ಯ_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    ಅನ್ತಃಸ್ಥ Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    ಅನ್ತಃಸ್ಥ Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    ಅನ್ತಃಸ್ಥ ~Hashtable();

    ಅನ್ತಃಸ್ಥ UObjectDeleter *set_ಮೌಲ್ಯ_Deleter(UObjectDeleter *fn);

    ಅನ್ತಃಸ್ಥ int32_t ಎಣಿಕೆ() ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* put(ಧ್ರುವ UnicodeString& ಕೀಲಿ, ಶೂನ್ಯ* ಮೌಲ್ಯ_2, UErrorCode& status);

    ಅನ್ತಃಸ್ಥ int32_t puti(ಧ್ರುವ UnicodeString& ಕೀಲಿ, int32_t ಮೌಲ್ಯ_2, UErrorCode& status);

    ಅನ್ತಃಸ್ಥ int32_t putiAllowZero(ಧ್ರುವ UnicodeString& ಕೀಲಿ, int32_t ಮೌಲ್ಯ_2, UErrorCode& status);

    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* get(ಧ್ರುವ UnicodeString& ಕೀಲಿ) ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ int32_t geti(ಧ್ರುವ UnicodeString& ಕೀಲಿ) ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ int32_t getiAndFound(ಧ್ರುವ UnicodeString& ಕೀಲಿ, UBool &found) ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* remove(ಧ್ರುವ UnicodeString& ಕೀಲಿ);

    ಅನ್ತಃಸ್ಥ int32_t removei(ಧ್ರುವ UnicodeString& ಕೀಲಿ);

    ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ removeAll();

    ಅನ್ತಃಸ್ಥ UBool contains_ಕೀಲಿ(ಧ್ರುವ UnicodeString& ಕೀಲಿ) ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ ಧ್ರುವ UHashElement* find(ಧ್ರುವ UnicodeString& ಕೀಲಿ) ಧ್ರುವ;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    ಅನ್ತಃಸ್ಥ ಧ್ರುವ UHashElement* nextElement(int32_t& pos) ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ UKeyComparator* set_ಕೀಲಿ_Comparator(UKeyComparator*ಕೀಲಿ_Comp);

    ಅನ್ತಃಸ್ಥ UValueComparator* set_ಮೌಲ್ಯ_Comparator(UValueComparator* ಮೌಲ್ಯ_Comp);

    ಅನ್ತಃಸ್ಥ UBool equals(ಧ್ರುವ Hashtable& that) ಧ್ರುವ;
ಗುಪ್ತ:
    Hashtable(ಧ್ರುವ Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ಧ್ರುವ Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ Hashtable::init(UHashFunction *ಕೀಲಿ_Hash, UKeyComparator *ಕೀಲಿ_Comp,
                            UValueComparator *ಮೌಲ್ಯ_Comp, UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    uhash_init(&hashObj, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, &status);
    ಯದಿ (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_ಕೀಲಿ_Deleter(hash, uprv_deleteUObject);
    }
}

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ Hashtable::initSize(UHashFunction *ಕೀಲಿ_Hash, UKeyComparator *ಕೀಲಿ_Comp,
                                UValueComparator *ಮೌಲ್ಯ_Comp, int32_t size, UErrorCode& status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    uhash_initSize(&hashObj, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, size, &status);
    ಯದಿ (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_ಕೀಲಿ_Deleter(hash, uprv_deleteUObject);
    }
}

ಅನ್ತಃಸ್ಥ Hashtable::Hashtable(UKeyComparator *ಕೀಲಿ_Comp, UValueComparator *ಮೌಲ್ಯ_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, status);
}

ಅನ್ತಃಸ್ಥ Hashtable::Hashtable(UBool ignore_ಕೀಲಿ_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_ಕೀಲಿ_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ಕೀಲಿ_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

ಅನ್ತಃಸ್ಥ Hashtable::Hashtable(UBool ignore_ಕೀಲಿ_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_ಕೀಲಿ_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ಕೀಲಿ_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

ಅನ್ತಃಸ್ಥ Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

ಅನ್ತಃಸ್ಥ Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

ಅನ್ತಃಸ್ಥ Hashtable::~Hashtable() {
    ಯದಿ (hash != nullptr) {
        uhash_close(hash);
    }
}

ಅನ್ತಃಸ್ಥ UObjectDeleter *Hashtable::set_ಮೌಲ್ಯ_Deleter(UObjectDeleter *fn) {
    ಪ್ರತ್ಯಾಗಮ uhash_set_ಮೌಲ್ಯ_Deleter(hash, fn);
}

ಅನ್ತಃಸ್ಥ int32_t Hashtable::ಎಣಿಕೆ() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uhash_ಎಣಿಕೆ(hash);
}

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* Hashtable::put(ಧ್ರುವ UnicodeString& ಕೀಲಿ, ಶೂನ್ಯ* ಮೌಲ್ಯ_2, UErrorCode& status) {
    ಪ್ರತ್ಯಾಗಮ uhash_put(hash, new UnicodeString(ಕೀಲಿ), ಮೌಲ್ಯ_2, &status);
}

ಅನ್ತಃಸ್ಥ int32_t Hashtable::puti(ಧ್ರುವ UnicodeString& ಕೀಲಿ, int32_t ಮೌಲ್ಯ_2, UErrorCode& status) {
    ಪ್ರತ್ಯಾಗಮ uhash_puti(hash, new UnicodeString(ಕೀಲಿ), ಮೌಲ್ಯ_2, &status);
}

ಅನ್ತಃಸ್ಥ int32_t Hashtable::putiAllowZero(ಧ್ರುವ UnicodeString& ಕೀಲಿ, int32_t ಮೌಲ್ಯ_2,
                                        UErrorCode& status) {
    ಪ್ರತ್ಯಾಗಮ uhash_putiAllowZero(hash, new UnicodeString(ಕೀಲಿ), ಮೌಲ್ಯ_2, &status);
}

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* Hashtable::get(ಧ್ರುವ UnicodeString& ಕೀಲಿ) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uhash_get(hash, &ಕೀಲಿ);
}

ಅನ್ತಃಸ್ಥ int32_t Hashtable::geti(ಧ್ರುವ UnicodeString& ಕೀಲಿ) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uhash_geti(hash, &ಕೀಲಿ);
}

ಅನ್ತಃಸ್ಥ int32_t Hashtable::getiAndFound(ಧ್ರುವ UnicodeString& ಕೀಲಿ, UBool &found) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uhash_getiAndFound(hash, &ಕೀಲಿ, &found);
}

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* Hashtable::remove(ಧ್ರುವ UnicodeString& ಕೀಲಿ) {
    ಪ್ರತ್ಯಾಗಮ uhash_remove(hash, &ಕೀಲಿ);
}

ಅನ್ತಃಸ್ಥ int32_t Hashtable::removei(ಧ್ರುವ UnicodeString& ಕೀಲಿ) {
    ಪ್ರತ್ಯಾಗಮ uhash_removei(hash, &ಕೀಲಿ);
}

ಅನ್ತಃಸ್ಥ UBool Hashtable::contains_ಕೀಲಿ(ಧ್ರುವ UnicodeString& ಕೀಲಿ) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uhash_contains_ಕೀಲಿ(hash, &ಕೀಲಿ);
}

ಅನ್ತಃಸ್ಥ ಧ್ರುವ UHashElement* Hashtable::find(ಧ್ರುವ UnicodeString& ಕೀಲಿ) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uhash_find(hash, &ಕೀಲಿ);
}

ಅನ್ತಃಸ್ಥ ಧ್ರುವ UHashElement* Hashtable::nextElement(int32_t& pos) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uhash_nextElement(hash, &pos);
}

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

ಅನ್ತಃಸ್ಥ UKeyComparator* Hashtable::set_ಕೀಲಿ_Comparator(UKeyComparator*ಕೀಲಿ_Comp){
    ಪ್ರತ್ಯಾಗಮ uhash_set_ಕೀಲಿ_Comparator(hash, ಕೀಲಿ_Comp);
}

ಅನ್ತಃಸ್ಥ UValueComparator* Hashtable::set_ಮೌಲ್ಯ_Comparator(UValueComparator* ಮೌಲ್ಯ_Comp){
    ಪ್ರತ್ಯಾಗಮ uhash_set_ಮೌಲ್ಯ_Comparator(hash, ಮೌಲ್ಯ_Comp);
}

ಅನ್ತಃಸ್ಥ UBool Hashtable::equals(ಧ್ರುವ Hashtable& that)ಧ್ರುವ{
   ಪ್ರತ್ಯಾಗಮ uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


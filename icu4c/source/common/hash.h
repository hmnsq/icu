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

    अन्तःस्थ शून्य init(UHashFunction *किल्ली_Hash, UKeyComparator *किल्ली_Comp, UValueComparator *मूल्य_Comp, UErrorCode& status);

    अन्तःस्थ शून्य initSize(UHashFunction *किल्ली_Hash, UKeyComparator *किल्ली_Comp, UValueComparator *मूल्य_Comp, int32_t size, UErrorCode& status);

सार्वजनिक:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UBool ignore_किल्ली_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UBool ignore_किल्ली_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UKeyComparator *किल्ली_Comp, UValueComparator *मूल्य_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    अन्तःस्थ Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    अन्तःस्थ ~Hashtable();

    अन्तःस्थ UObjectDeleter *set_मूल्य_Deleter(UObjectDeleter *fn);

    अन्तःस्थ int32_t मोजणी() ध्रुव;

    अन्तःस्थ शून्य* put(ध्रुव UnicodeString& किल्ली, शून्य* मूल्य_2, UErrorCode& status);

    अन्तःस्थ int32_t puti(ध्रुव UnicodeString& किल्ली, int32_t मूल्य_2, UErrorCode& status);

    अन्तःस्थ int32_t putiAllowZero(ध्रुव UnicodeString& किल्ली, int32_t मूल्य_2, UErrorCode& status);

    अन्तःस्थ शून्य* get(ध्रुव UnicodeString& किल्ली) ध्रुव;

    अन्तःस्थ int32_t geti(ध्रुव UnicodeString& किल्ली) ध्रुव;

    अन्तःस्थ int32_t getiAndFound(ध्रुव UnicodeString& किल्ली, UBool &found) ध्रुव;

    अन्तःस्थ शून्य* remove(ध्रुव UnicodeString& किल्ली);

    अन्तःस्थ int32_t removei(ध्रुव UnicodeString& किल्ली);

    अन्तःस्थ शून्य removeAll();

    अन्तःस्थ UBool contains_किल्ली(ध्रुव UnicodeString& किल्ली) ध्रुव;

    अन्तःस्थ ध्रुव UHashElement* find(ध्रुव UnicodeString& किल्ली) ध्रुव;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    अन्तःस्थ ध्रुव UHashElement* nextElement(int32_t& pos) ध्रुव;

    अन्तःस्थ UKeyComparator* set_किल्ली_Comparator(UKeyComparator*किल्ली_Comp);

    अन्तःस्थ UValueComparator* set_मूल्य_Comparator(UValueComparator* मूल्य_Comp);

    अन्तःस्थ UBool equals(ध्रुव Hashtable& that) ध्रुव;
गुप्त:
    Hashtable(ध्रुव Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ध्रुव Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

अन्तःस्थ शून्य Hashtable::init(UHashFunction *किल्ली_Hash, UKeyComparator *किल्ली_Comp,
                            UValueComparator *मूल्य_Comp, UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    uhash_init(&hashObj, किल्ली_Hash, किल्ली_Comp, मूल्य_Comp, &status);
    यदि (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_किल्ली_Deleter(hash, uprv_deleteUObject);
    }
}

अन्तःस्थ शून्य Hashtable::initSize(UHashFunction *किल्ली_Hash, UKeyComparator *किल्ली_Comp,
                                UValueComparator *मूल्य_Comp, int32_t size, UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    uhash_initSize(&hashObj, किल्ली_Hash, किल्ली_Comp, मूल्य_Comp, size, &status);
    यदि (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_किल्ली_Deleter(hash, uprv_deleteUObject);
    }
}

अन्तःस्थ Hashtable::Hashtable(UKeyComparator *किल्ली_Comp, UValueComparator *मूल्य_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, किल्ली_Comp, मूल्य_Comp, status);
}

अन्तःस्थ Hashtable::Hashtable(UBool ignore_किल्ली_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_किल्ली_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_किल्ली_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

अन्तःस्थ Hashtable::Hashtable(UBool ignore_किल्ली_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_किल्ली_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_किल्ली_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

अन्तःस्थ Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

अन्तःस्थ Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

अन्तःस्थ Hashtable::~Hashtable() {
    यदि (hash != nullptr) {
        uhash_close(hash);
    }
}

अन्तःस्थ UObjectDeleter *Hashtable::set_मूल्य_Deleter(UObjectDeleter *fn) {
    प्रत्यागम uhash_set_मूल्य_Deleter(hash, fn);
}

अन्तःस्थ int32_t Hashtable::मोजणी() ध्रुव {
    प्रत्यागम uhash_मोजणी(hash);
}

अन्तःस्थ शून्य* Hashtable::put(ध्रुव UnicodeString& किल्ली, शून्य* मूल्य_2, UErrorCode& status) {
    प्रत्यागम uhash_put(hash, new UnicodeString(किल्ली), मूल्य_2, &status);
}

अन्तःस्थ int32_t Hashtable::puti(ध्रुव UnicodeString& किल्ली, int32_t मूल्य_2, UErrorCode& status) {
    प्रत्यागम uhash_puti(hash, new UnicodeString(किल्ली), मूल्य_2, &status);
}

अन्तःस्थ int32_t Hashtable::putiAllowZero(ध्रुव UnicodeString& किल्ली, int32_t मूल्य_2,
                                        UErrorCode& status) {
    प्रत्यागम uhash_putiAllowZero(hash, new UnicodeString(किल्ली), मूल्य_2, &status);
}

अन्तःस्थ शून्य* Hashtable::get(ध्रुव UnicodeString& किल्ली) ध्रुव {
    प्रत्यागम uhash_get(hash, &किल्ली);
}

अन्तःस्थ int32_t Hashtable::geti(ध्रुव UnicodeString& किल्ली) ध्रुव {
    प्रत्यागम uhash_geti(hash, &किल्ली);
}

अन्तःस्थ int32_t Hashtable::getiAndFound(ध्रुव UnicodeString& किल्ली, UBool &found) ध्रुव {
    प्रत्यागम uhash_getiAndFound(hash, &किल्ली, &found);
}

अन्तःस्थ शून्य* Hashtable::remove(ध्रुव UnicodeString& किल्ली) {
    प्रत्यागम uhash_remove(hash, &किल्ली);
}

अन्तःस्थ int32_t Hashtable::removei(ध्रुव UnicodeString& किल्ली) {
    प्रत्यागम uhash_removei(hash, &किल्ली);
}

अन्तःस्थ UBool Hashtable::contains_किल्ली(ध्रुव UnicodeString& किल्ली) ध्रुव {
    प्रत्यागम uhash_contains_किल्ली(hash, &किल्ली);
}

अन्तःस्थ ध्रुव UHashElement* Hashtable::find(ध्रुव UnicodeString& किल्ली) ध्रुव {
    प्रत्यागम uhash_find(hash, &किल्ली);
}

अन्तःस्थ ध्रुव UHashElement* Hashtable::nextElement(int32_t& pos) ध्रुव {
    प्रत्यागम uhash_nextElement(hash, &pos);
}

अन्तःस्थ शून्य Hashtable::removeAll() {
    uhash_removeAll(hash);
}

अन्तःस्थ UKeyComparator* Hashtable::set_किल्ली_Comparator(UKeyComparator*किल्ली_Comp){
    प्रत्यागम uhash_set_किल्ली_Comparator(hash, किल्ली_Comp);
}

अन्तःस्थ UValueComparator* Hashtable::set_मूल्य_Comparator(UValueComparator* मूल्य_Comp){
    प्रत्यागम uhash_set_मूल्य_Comparator(hash, मूल्य_Comp);
}

अन्तःस्थ UBool Hashtable::equals(ध्रुव Hashtable& that)ध्रुव{
   प्रत्यागम uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


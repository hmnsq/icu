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

    अन्तःस्थ शून्य init(UHashFunction *कुञ्चिका_Hash, UKeyComparator *कुञ्चिका_Comp, UValueComparator *मानम्_Comp, UErrorCode& status);

    अन्तःस्थ शून्य initSize(UHashFunction *कुञ्चिका_Hash, UKeyComparator *कुञ्चिका_Comp, UValueComparator *मानम्_Comp, int32_t size, UErrorCode& status);

सार्वजनिक:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UBool ignore_कुञ्चिका_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UBool ignore_कुञ्चिका_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UKeyComparator *कुञ्चिका_Comp, UValueComparator *मानम्_Comp, UErrorCode& status);

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

    अन्तःस्थ UObjectDeleter *set_मानम्_Deleter(UObjectDeleter *fn);

    अन्तःस्थ int32_t गणनम्() ध्रुव;

    अन्तःस्थ शून्य* put(ध्रुव UnicodeString& कुञ्चिका, शून्य* मानम्_2, UErrorCode& status);

    अन्तःस्थ int32_t puti(ध्रुव UnicodeString& कुञ्चिका, int32_t मानम्_2, UErrorCode& status);

    अन्तःस्थ int32_t putiAllowZero(ध्रुव UnicodeString& कुञ्चिका, int32_t मानम्_2, UErrorCode& status);

    अन्तःस्थ शून्य* get(ध्रुव UnicodeString& कुञ्चिका) ध्रुव;

    अन्तःस्थ int32_t geti(ध्रुव UnicodeString& कुञ्चिका) ध्रुव;

    अन्तःस्थ int32_t getiAndFound(ध्रुव UnicodeString& कुञ्चिका, UBool &found) ध्रुव;

    अन्तःस्थ शून्य* remove(ध्रुव UnicodeString& कुञ्चिका);

    अन्तःस्थ int32_t removei(ध्रुव UnicodeString& कुञ्चिका);

    अन्तःस्थ शून्य removeAll();

    अन्तःस्थ UBool contains_कुञ्चिका(ध्रुव UnicodeString& कुञ्चिका) ध्रुव;

    अन्तःस्थ ध्रुव UHashElement* find(ध्रुव UnicodeString& कुञ्चिका) ध्रुव;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    अन्तःस्थ ध्रुव UHashElement* nextElement(int32_t& pos) ध्रुव;

    अन्तःस्थ UKeyComparator* set_कुञ्चिका_Comparator(UKeyComparator*कुञ्चिका_Comp);

    अन्तःस्थ UValueComparator* set_मानम्_Comparator(UValueComparator* मानम्_Comp);

    अन्तःस्थ UBool equals(ध्रुव Hashtable& that) ध्रुव;
गुप्त:
    Hashtable(ध्रुव Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ध्रुव Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

अन्तःस्थ शून्य Hashtable::init(UHashFunction *कुञ्चिका_Hash, UKeyComparator *कुञ्चिका_Comp,
                            UValueComparator *मानम्_Comp, UErrorCode& status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    uhash_init(&hashObj, कुञ्चिका_Hash, कुञ्चिका_Comp, मानम्_Comp, &status);
    सचे (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_कुञ्चिका_Deleter(hash, uprv_deleteUObject);
    }
}

अन्तःस्थ शून्य Hashtable::initSize(UHashFunction *कुञ्चिका_Hash, UKeyComparator *कुञ्चिका_Comp,
                                UValueComparator *मानम्_Comp, int32_t size, UErrorCode& status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    uhash_initSize(&hashObj, कुञ्चिका_Hash, कुञ्चिका_Comp, मानम्_Comp, size, &status);
    सचे (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_कुञ्चिका_Deleter(hash, uprv_deleteUObject);
    }
}

अन्तःस्थ Hashtable::Hashtable(UKeyComparator *कुञ्चिका_Comp, UValueComparator *मानम्_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, कुञ्चिका_Comp, मानम्_Comp, status);
}

अन्तःस्थ Hashtable::Hashtable(UBool ignore_कुञ्चिका_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_कुञ्चिका_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_कुञ्चिका_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

अन्तःस्थ Hashtable::Hashtable(UBool ignore_कुञ्चिका_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_कुञ्चिका_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_कुञ्चिका_Case ? uhash_compareCaselessUnicodeString
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
    सचे (hash != nullptr) {
        uhash_close(hash);
    }
}

अन्तःस्थ UObjectDeleter *Hashtable::set_मानम्_Deleter(UObjectDeleter *fn) {
    पटिनिवत्त uhash_set_मानम्_Deleter(hash, fn);
}

अन्तःस्थ int32_t Hashtable::गणनम्() ध्रुव {
    पटिनिवत्त uhash_गणनम्(hash);
}

अन्तःस्थ शून्य* Hashtable::put(ध्रुव UnicodeString& कुञ्चिका, शून्य* मानम्_2, UErrorCode& status) {
    पटिनिवत्त uhash_put(hash, new UnicodeString(कुञ्चिका), मानम्_2, &status);
}

अन्तःस्थ int32_t Hashtable::puti(ध्रुव UnicodeString& कुञ्चिका, int32_t मानम्_2, UErrorCode& status) {
    पटिनिवत्त uhash_puti(hash, new UnicodeString(कुञ्चिका), मानम्_2, &status);
}

अन्तःस्थ int32_t Hashtable::putiAllowZero(ध्रुव UnicodeString& कुञ्चिका, int32_t मानम्_2,
                                        UErrorCode& status) {
    पटिनिवत्त uhash_putiAllowZero(hash, new UnicodeString(कुञ्चिका), मानम्_2, &status);
}

अन्तःस्थ शून्य* Hashtable::get(ध्रुव UnicodeString& कुञ्चिका) ध्रुव {
    पटिनिवत्त uhash_get(hash, &कुञ्चिका);
}

अन्तःस्थ int32_t Hashtable::geti(ध्रुव UnicodeString& कुञ्चिका) ध्रुव {
    पटिनिवत्त uhash_geti(hash, &कुञ्चिका);
}

अन्तःस्थ int32_t Hashtable::getiAndFound(ध्रुव UnicodeString& कुञ्चिका, UBool &found) ध्रुव {
    पटिनिवत्त uhash_getiAndFound(hash, &कुञ्चिका, &found);
}

अन्तःस्थ शून्य* Hashtable::remove(ध्रुव UnicodeString& कुञ्चिका) {
    पटिनिवत्त uhash_remove(hash, &कुञ्चिका);
}

अन्तःस्थ int32_t Hashtable::removei(ध्रुव UnicodeString& कुञ्चिका) {
    पटिनिवत्त uhash_removei(hash, &कुञ्चिका);
}

अन्तःस्थ UBool Hashtable::contains_कुञ्चिका(ध्रुव UnicodeString& कुञ्चिका) ध्रुव {
    पटिनिवत्त uhash_contains_कुञ्चिका(hash, &कुञ्चिका);
}

अन्तःस्थ ध्रुव UHashElement* Hashtable::find(ध्रुव UnicodeString& कुञ्चिका) ध्रुव {
    पटिनिवत्त uhash_find(hash, &कुञ्चिका);
}

अन्तःस्थ ध्रुव UHashElement* Hashtable::nextElement(int32_t& pos) ध्रुव {
    पटिनिवत्त uhash_nextElement(hash, &pos);
}

अन्तःस्थ शून्य Hashtable::removeAll() {
    uhash_removeAll(hash);
}

अन्तःस्थ UKeyComparator* Hashtable::set_कुञ्चिका_Comparator(UKeyComparator*कुञ्चिका_Comp){
    पटिनिवत्त uhash_set_कुञ्चिका_Comparator(hash, कुञ्चिका_Comp);
}

अन्तःस्थ UValueComparator* Hashtable::set_मानम्_Comparator(UValueComparator* मानम्_Comp){
    पटिनिवत्त uhash_set_मानम्_Comparator(hash, मानम्_Comp);
}

अन्तःस्थ UBool Hashtable::equals(ध्रुव Hashtable& that)ध्रुव{
   पटिनिवत्त uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


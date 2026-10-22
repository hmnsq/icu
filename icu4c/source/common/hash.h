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

    अन्तःस्थ शून्य init(UHashFunction *साँचो_Hash, UKeyComparator *साँचो_Comp, UValueComparator *मान_Comp, UErrorCode& status);

    अन्तःस्थ शून्य initSize(UHashFunction *साँचो_Hash, UKeyComparator *साँचो_Comp, UValueComparator *मान_Comp, int32_t size, UErrorCode& status);

सार्वजनिक:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UBool ignore_साँचो_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UBool ignore_साँचो_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UKeyComparator *साँचो_Comp, UValueComparator *मान_Comp, UErrorCode& status);

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

    अन्तःस्थ UObjectDeleter *set_मान_Deleter(UObjectDeleter *fn);

    अन्तःस्थ int32_t गन्ती() अचर;

    अन्तःस्थ शून्य* put(अचर UnicodeString& साँचो, शून्य* मान_2, UErrorCode& status);

    अन्तःस्थ int32_t puti(अचर UnicodeString& साँचो, int32_t मान_2, UErrorCode& status);

    अन्तःस्थ int32_t putiAllowZero(अचर UnicodeString& साँचो, int32_t मान_2, UErrorCode& status);

    अन्तःस्थ शून्य* get(अचर UnicodeString& साँचो) अचर;

    अन्तःस्थ int32_t geti(अचर UnicodeString& साँचो) अचर;

    अन्तःस्थ int32_t getiAndFound(अचर UnicodeString& साँचो, UBool &found) अचर;

    अन्तःस्थ शून्य* हटाओ(अचर UnicodeString& साँचो);

    अन्तःस्थ int32_t removei(अचर UnicodeString& साँचो);

    अन्तःस्थ शून्य removeAll();

    अन्तःस्थ UBool contains_साँचो(अचर UnicodeString& साँचो) अचर;

    अन्तःस्थ अचर UHashElement* find(अचर UnicodeString& साँचो) अचर;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    अन्तःस्थ अचर UHashElement* nextElement(int32_t& pos) अचर;

    अन्तःस्थ UKeyComparator* set_साँचो_Comparator(UKeyComparator*साँचो_Comp);

    अन्तःस्थ UValueComparator* set_मान_Comparator(UValueComparator* मान_Comp);

    अन्तःस्थ UBool equals(अचर Hashtable& that) अचर;
गुप्त:
    Hashtable(अचर Hashtable &other) = delete; // forbid copying of this class
    Hashtable &चालक=(अचर Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

अन्तःस्थ शून्य Hashtable::init(UHashFunction *साँचो_Hash, UKeyComparator *साँचो_Comp,
                            UValueComparator *मान_Comp, UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    uhash_init(&hashObj, साँचो_Hash, साँचो_Comp, मान_Comp, &status);
    यदि (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_साँचो_Deleter(hash, uprv_deleteUObject);
    }
}

अन्तःस्थ शून्य Hashtable::initSize(UHashFunction *साँचो_Hash, UKeyComparator *साँचो_Comp,
                                UValueComparator *मान_Comp, int32_t size, UErrorCode& status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    uhash_initSize(&hashObj, साँचो_Hash, साँचो_Comp, मान_Comp, size, &status);
    यदि (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_साँचो_Deleter(hash, uprv_deleteUObject);
    }
}

अन्तःस्थ Hashtable::Hashtable(UKeyComparator *साँचो_Comp, UValueComparator *मान_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, साँचो_Comp, मान_Comp, status);
}

अन्तःस्थ Hashtable::Hashtable(UBool ignore_साँचो_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_साँचो_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_साँचो_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

अन्तःस्थ Hashtable::Hashtable(UBool ignore_साँचो_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_साँचो_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_साँचो_Case ? uhash_compareCaselessUnicodeString
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

अन्तःस्थ UObjectDeleter *Hashtable::set_मान_Deleter(UObjectDeleter *fn) {
    फर्काऊ uhash_set_मान_Deleter(hash, fn);
}

अन्तःस्थ int32_t Hashtable::गन्ती() अचर {
    फर्काऊ uhash_गन्ती(hash);
}

अन्तःस्थ शून्य* Hashtable::put(अचर UnicodeString& साँचो, शून्य* मान_2, UErrorCode& status) {
    फर्काऊ uhash_put(hash, new UnicodeString(साँचो), मान_2, &status);
}

अन्तःस्थ int32_t Hashtable::puti(अचर UnicodeString& साँचो, int32_t मान_2, UErrorCode& status) {
    फर्काऊ uhash_puti(hash, new UnicodeString(साँचो), मान_2, &status);
}

अन्तःस्थ int32_t Hashtable::putiAllowZero(अचर UnicodeString& साँचो, int32_t मान_2,
                                        UErrorCode& status) {
    फर्काऊ uhash_putiAllowZero(hash, new UnicodeString(साँचो), मान_2, &status);
}

अन्तःस्थ शून्य* Hashtable::get(अचर UnicodeString& साँचो) अचर {
    फर्काऊ uhash_get(hash, &साँचो);
}

अन्तःस्थ int32_t Hashtable::geti(अचर UnicodeString& साँचो) अचर {
    फर्काऊ uhash_geti(hash, &साँचो);
}

अन्तःस्थ int32_t Hashtable::getiAndFound(अचर UnicodeString& साँचो, UBool &found) अचर {
    फर्काऊ uhash_getiAndFound(hash, &साँचो, &found);
}

अन्तःस्थ शून्य* Hashtable::हटाओ(अचर UnicodeString& साँचो) {
    फर्काऊ uhash_remove(hash, &साँचो);
}

अन्तःस्थ int32_t Hashtable::removei(अचर UnicodeString& साँचो) {
    फर्काऊ uhash_removei(hash, &साँचो);
}

अन्तःस्थ UBool Hashtable::contains_साँचो(अचर UnicodeString& साँचो) अचर {
    फर्काऊ uhash_contains_साँचो(hash, &साँचो);
}

अन्तःस्थ अचर UHashElement* Hashtable::find(अचर UnicodeString& साँचो) अचर {
    फर्काऊ uhash_find(hash, &साँचो);
}

अन्तःस्थ अचर UHashElement* Hashtable::nextElement(int32_t& pos) अचर {
    फर्काऊ uhash_nextElement(hash, &pos);
}

अन्तःस्थ शून्य Hashtable::removeAll() {
    uhash_removeAll(hash);
}

अन्तःस्थ UKeyComparator* Hashtable::set_साँचो_Comparator(UKeyComparator*साँचो_Comp){
    फर्काऊ uhash_set_साँचो_Comparator(hash, साँचो_Comp);
}

अन्तःस्थ UValueComparator* Hashtable::set_मान_Comparator(UValueComparator* मान_Comp){
    फर्काऊ uhash_set_मान_Comparator(hash, मान_Comp);
}

अन्तःस्थ UBool Hashtable::equals(अचर Hashtable& that)अचर{
   फर्काऊ uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


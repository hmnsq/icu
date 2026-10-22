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

#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : പൊതു UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    വരിയിൽ ശൂന്യം init(UHashFunction *താക്കോൽ_Hash, UKeyComparator *താക്കോൽ_Comp, UValueComparator *മൂല്യം_Comp, UErrorCode& status);

    വരിയിൽ ശൂന്യം initSize(UHashFunction *താക്കോൽ_Hash, UKeyComparator *താക്കോൽ_Comp, UValueComparator *മൂല്യം_Comp, int32_t size, UErrorCode& status);

പൊതു:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    വരിയിൽ Hashtable(UBool ignore_താക്കോൽ_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    വരിയിൽ Hashtable(UBool ignore_താക്കോൽ_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    വരിയിൽ Hashtable(UKeyComparator *താക്കോൽ_Comp, UValueComparator *മൂല്യം_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    വരിയിൽ Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    വരിയിൽ Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    വരിയിൽ ~Hashtable();

    വരിയിൽ UObjectDeleter *set_മൂല്യം_Deleter(UObjectDeleter *fn);

    വരിയിൽ int32_t എണ്ണം() സ്ഥിരം;

    വരിയിൽ ശൂന്യം* put(സ്ഥിരം UnicodeString& താക്കോൽ, ശൂന്യം* മൂല്യം_2, UErrorCode& status);

    വരിയിൽ int32_t puti(സ്ഥിരം UnicodeString& താക്കോൽ, int32_t മൂല്യം_2, UErrorCode& status);

    വരിയിൽ int32_t putiAllowZero(സ്ഥിരം UnicodeString& താക്കോൽ, int32_t മൂല്യം_2, UErrorCode& status);

    വരിയിൽ ശൂന്യം* get(സ്ഥിരം UnicodeString& താക്കോൽ) സ്ഥിരം;

    വരിയിൽ int32_t geti(സ്ഥിരം UnicodeString& താക്കോൽ) സ്ഥിരം;

    വരിയിൽ int32_t getiAndFound(സ്ഥിരം UnicodeString& താക്കോൽ, UBool &found) സ്ഥിരം;

    വരിയിൽ ശൂന്യം* remove(സ്ഥിരം UnicodeString& താക്കോൽ);

    വരിയിൽ int32_t removei(സ്ഥിരം UnicodeString& താക്കോൽ);

    വരിയിൽ ശൂന്യം removeAll();

    വരിയിൽ UBool contains_താക്കോൽ(സ്ഥിരം UnicodeString& താക്കോൽ) സ്ഥിരം;

    വരിയിൽ സ്ഥിരം UHashElement* find(സ്ഥിരം UnicodeString& താക്കോൽ) സ്ഥിരം;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    വരിയിൽ സ്ഥിരം UHashElement* nextElement(int32_t& pos) സ്ഥിരം;

    വരിയിൽ UKeyComparator* set_താക്കോൽ_Comparator(UKeyComparator*താക്കോൽ_Comp);

    വരിയിൽ UValueComparator* set_മൂല്യം_Comparator(UValueComparator* മൂല്യം_Comp);

    വരിയിൽ UBool equals(സ്ഥിരം Hashtable& that) സ്ഥിരം;
സ്വകാര്യ:
    Hashtable(സ്ഥിരം Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(സ്ഥിരം Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

വരിയിൽ ശൂന്യം Hashtable::init(UHashFunction *താക്കോൽ_Hash, UKeyComparator *താക്കോൽ_Comp,
                            UValueComparator *മൂല്യം_Comp, UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    uhash_init(&hashObj, താക്കോൽ_Hash, താക്കോൽ_Comp, മൂല്യം_Comp, &status);
    എങ്കിൽ (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_താക്കോൽ_Deleter(hash, uprv_deleteUObject);
    }
}

വരിയിൽ ശൂന്യം Hashtable::initSize(UHashFunction *താക്കോൽ_Hash, UKeyComparator *താക്കോൽ_Comp,
                                UValueComparator *മൂല്യം_Comp, int32_t size, UErrorCode& status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    uhash_initSize(&hashObj, താക്കോൽ_Hash, താക്കോൽ_Comp, മൂല്യം_Comp, size, &status);
    എങ്കിൽ (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_താക്കോൽ_Deleter(hash, uprv_deleteUObject);
    }
}

വരിയിൽ Hashtable::Hashtable(UKeyComparator *താക്കോൽ_Comp, UValueComparator *മൂല്യം_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, താക്കോൽ_Comp, മൂല്യം_Comp, status);
}

വരിയിൽ Hashtable::Hashtable(UBool ignore_താക്കോൽ_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_താക്കോൽ_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_താക്കോൽ_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

വരിയിൽ Hashtable::Hashtable(UBool ignore_താക്കോൽ_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_താക്കോൽ_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_താക്കോൽ_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

വരിയിൽ Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

വരിയിൽ Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

വരിയിൽ Hashtable::~Hashtable() {
    എങ്കിൽ (hash != nullptr) {
        uhash_close(hash);
    }
}

വരിയിൽ UObjectDeleter *Hashtable::set_മൂല്യം_Deleter(UObjectDeleter *fn) {
    തിരികെ uhash_set_മൂല്യം_Deleter(hash, fn);
}

വരിയിൽ int32_t Hashtable::എണ്ണം() സ്ഥിരം {
    തിരികെ uhash_എണ്ണം(hash);
}

വരിയിൽ ശൂന്യം* Hashtable::put(സ്ഥിരം UnicodeString& താക്കോൽ, ശൂന്യം* മൂല്യം_2, UErrorCode& status) {
    തിരികെ uhash_put(hash, new UnicodeString(താക്കോൽ), മൂല്യം_2, &status);
}

വരിയിൽ int32_t Hashtable::puti(സ്ഥിരം UnicodeString& താക്കോൽ, int32_t മൂല്യം_2, UErrorCode& status) {
    തിരികെ uhash_puti(hash, new UnicodeString(താക്കോൽ), മൂല്യം_2, &status);
}

വരിയിൽ int32_t Hashtable::putiAllowZero(സ്ഥിരം UnicodeString& താക്കോൽ, int32_t മൂല്യം_2,
                                        UErrorCode& status) {
    തിരികെ uhash_putiAllowZero(hash, new UnicodeString(താക്കോൽ), മൂല്യം_2, &status);
}

വരിയിൽ ശൂന്യം* Hashtable::get(സ്ഥിരം UnicodeString& താക്കോൽ) സ്ഥിരം {
    തിരികെ uhash_get(hash, &താക്കോൽ);
}

വരിയിൽ int32_t Hashtable::geti(സ്ഥിരം UnicodeString& താക്കോൽ) സ്ഥിരം {
    തിരികെ uhash_geti(hash, &താക്കോൽ);
}

വരിയിൽ int32_t Hashtable::getiAndFound(സ്ഥിരം UnicodeString& താക്കോൽ, UBool &found) സ്ഥിരം {
    തിരികെ uhash_getiAndFound(hash, &താക്കോൽ, &found);
}

വരിയിൽ ശൂന്യം* Hashtable::remove(സ്ഥിരം UnicodeString& താക്കോൽ) {
    തിരികെ uhash_remove(hash, &താക്കോൽ);
}

വരിയിൽ int32_t Hashtable::removei(സ്ഥിരം UnicodeString& താക്കോൽ) {
    തിരികെ uhash_removei(hash, &താക്കോൽ);
}

വരിയിൽ UBool Hashtable::contains_താക്കോൽ(സ്ഥിരം UnicodeString& താക്കോൽ) സ്ഥിരം {
    തിരികെ uhash_contains_താക്കോൽ(hash, &താക്കോൽ);
}

വരിയിൽ സ്ഥിരം UHashElement* Hashtable::find(സ്ഥിരം UnicodeString& താക്കോൽ) സ്ഥിരം {
    തിരികെ uhash_find(hash, &താക്കോൽ);
}

വരിയിൽ സ്ഥിരം UHashElement* Hashtable::nextElement(int32_t& pos) സ്ഥിരം {
    തിരികെ uhash_nextElement(hash, &pos);
}

വരിയിൽ ശൂന്യം Hashtable::removeAll() {
    uhash_removeAll(hash);
}

വരിയിൽ UKeyComparator* Hashtable::set_താക്കോൽ_Comparator(UKeyComparator*താക്കോൽ_Comp){
    തിരികെ uhash_set_താക്കോൽ_Comparator(hash, താക്കോൽ_Comp);
}

വരിയിൽ UValueComparator* Hashtable::set_മൂല്യം_Comparator(UValueComparator* മൂല്യം_Comp){
    തിരികെ uhash_set_മൂല്യം_Comparator(hash, മൂല്യം_Comp);
}

വരിയിൽ UBool Hashtable::equals(സ്ഥിരം Hashtable& that)സ്ഥിരം{
   തിരികെ uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


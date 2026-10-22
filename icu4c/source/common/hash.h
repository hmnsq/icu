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

#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : જાહેર UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    અંતઃરેખ શૂન્ય init(UHashFunction *ચાવી_Hash, UKeyComparator *ચાવી_Comp, UValueComparator *કિંમત_Comp, UErrorCode& status);

    અંતઃરેખ શૂન્ય initSize(UHashFunction *ચાવી_Hash, UKeyComparator *ચાવી_Comp, UValueComparator *કિંમત_Comp, int32_t size, UErrorCode& status);

જાહેર:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    અંતઃરેખ Hashtable(UBool ignore_ચાવી_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    અંતઃરેખ Hashtable(UBool ignore_ચાવી_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    અંતઃરેખ Hashtable(UKeyComparator *ચાવી_Comp, UValueComparator *કિંમત_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    અંતઃરેખ Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    અંતઃરેખ Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    અંતઃરેખ ~Hashtable();

    અંતઃરેખ UObjectDeleter *set_કિંમત_Deleter(UObjectDeleter *fn);

    અંતઃરેખ int32_t ગણતરી() અચલ;

    અંતઃરેખ શૂન્ય* put(અચલ UnicodeString& ચાવી, શૂન્ય* કિંમત_2, UErrorCode& status);

    અંતઃરેખ int32_t puti(અચલ UnicodeString& ચાવી, int32_t કિંમત_2, UErrorCode& status);

    અંતઃરેખ int32_t putiAllowZero(અચલ UnicodeString& ચાવી, int32_t કિંમત_2, UErrorCode& status);

    અંતઃરેખ શૂન્ય* get(અચલ UnicodeString& ચાવી) અચલ;

    અંતઃરેખ int32_t geti(અચલ UnicodeString& ચાવી) અચલ;

    અંતઃરેખ int32_t getiAndFound(અચલ UnicodeString& ચાવી, UBool &found) અચલ;

    અંતઃરેખ શૂન્ય* remove(અચલ UnicodeString& ચાવી);

    અંતઃરેખ int32_t removei(અચલ UnicodeString& ચાવી);

    અંતઃરેખ શૂન્ય removeAll();

    અંતઃરેખ UBool contains_ચાવી(અચલ UnicodeString& ચાવી) અચલ;

    અંતઃરેખ અચલ UHashElement* find(અચલ UnicodeString& ચાવી) અચલ;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    અંતઃરેખ અચલ UHashElement* nextElement(int32_t& pos) અચલ;

    અંતઃરેખ UKeyComparator* set_ચાવી_Comparator(UKeyComparator*ચાવી_Comp);

    અંતઃરેખ UValueComparator* set_કિંમત_Comparator(UValueComparator* કિંમત_Comp);

    અંતઃરેખ UBool equals(અચલ Hashtable& that) અચલ;
ખાનગી:
    Hashtable(અચલ Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(અચલ Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

અંતઃરેખ શૂન્ય Hashtable::init(UHashFunction *ચાવી_Hash, UKeyComparator *ચાવી_Comp,
                            UValueComparator *કિંમત_Comp, UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    uhash_init(&hashObj, ચાવી_Hash, ચાવી_Comp, કિંમત_Comp, &status);
    જો (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_ચાવી_Deleter(hash, uprv_deleteUObject);
    }
}

અંતઃરેખ શૂન્ય Hashtable::initSize(UHashFunction *ચાવી_Hash, UKeyComparator *ચાવી_Comp,
                                UValueComparator *કિંમત_Comp, int32_t size, UErrorCode& status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    uhash_initSize(&hashObj, ચાવી_Hash, ચાવી_Comp, કિંમત_Comp, size, &status);
    જો (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_ચાવી_Deleter(hash, uprv_deleteUObject);
    }
}

અંતઃરેખ Hashtable::Hashtable(UKeyComparator *ચાવી_Comp, UValueComparator *કિંમત_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, ચાવી_Comp, કિંમત_Comp, status);
}

અંતઃરેખ Hashtable::Hashtable(UBool ignore_ચાવી_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_ચાવી_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ચાવી_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

અંતઃરેખ Hashtable::Hashtable(UBool ignore_ચાવી_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_ચાવી_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ચાવી_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

અંતઃરેખ Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

અંતઃરેખ Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

અંતઃરેખ Hashtable::~Hashtable() {
    જો (hash != nullptr) {
        uhash_close(hash);
    }
}

અંતઃરેખ UObjectDeleter *Hashtable::set_કિંમત_Deleter(UObjectDeleter *fn) {
    પાછા uhash_set_કિંમત_Deleter(hash, fn);
}

અંતઃરેખ int32_t Hashtable::ગણતરી() અચલ {
    પાછા uhash_ગણતરી(hash);
}

અંતઃરેખ શૂન્ય* Hashtable::put(અચલ UnicodeString& ચાવી, શૂન્ય* કિંમત_2, UErrorCode& status) {
    પાછા uhash_put(hash, new UnicodeString(ચાવી), કિંમત_2, &status);
}

અંતઃરેખ int32_t Hashtable::puti(અચલ UnicodeString& ચાવી, int32_t કિંમત_2, UErrorCode& status) {
    પાછા uhash_puti(hash, new UnicodeString(ચાવી), કિંમત_2, &status);
}

અંતઃરેખ int32_t Hashtable::putiAllowZero(અચલ UnicodeString& ચાવી, int32_t કિંમત_2,
                                        UErrorCode& status) {
    પાછા uhash_putiAllowZero(hash, new UnicodeString(ચાવી), કિંમત_2, &status);
}

અંતઃરેખ શૂન્ય* Hashtable::get(અચલ UnicodeString& ચાવી) અચલ {
    પાછા uhash_get(hash, &ચાવી);
}

અંતઃરેખ int32_t Hashtable::geti(અચલ UnicodeString& ચાવી) અચલ {
    પાછા uhash_geti(hash, &ચાવી);
}

અંતઃરેખ int32_t Hashtable::getiAndFound(અચલ UnicodeString& ચાવી, UBool &found) અચલ {
    પાછા uhash_getiAndFound(hash, &ચાવી, &found);
}

અંતઃરેખ શૂન્ય* Hashtable::remove(અચલ UnicodeString& ચાવી) {
    પાછા uhash_remove(hash, &ચાવી);
}

અંતઃરેખ int32_t Hashtable::removei(અચલ UnicodeString& ચાવી) {
    પાછા uhash_removei(hash, &ચાવી);
}

અંતઃરેખ UBool Hashtable::contains_ચાવી(અચલ UnicodeString& ચાવી) અચલ {
    પાછા uhash_contains_ચાવી(hash, &ચાવી);
}

અંતઃરેખ અચલ UHashElement* Hashtable::find(અચલ UnicodeString& ચાવી) અચલ {
    પાછા uhash_find(hash, &ચાવી);
}

અંતઃરેખ અચલ UHashElement* Hashtable::nextElement(int32_t& pos) અચલ {
    પાછા uhash_nextElement(hash, &pos);
}

અંતઃરેખ શૂન્ય Hashtable::removeAll() {
    uhash_removeAll(hash);
}

અંતઃરેખ UKeyComparator* Hashtable::set_ચાવી_Comparator(UKeyComparator*ચાવી_Comp){
    પાછા uhash_set_ચાવી_Comparator(hash, ચાવી_Comp);
}

અંતઃરેખ UValueComparator* Hashtable::set_કિંમત_Comparator(UValueComparator* કિંમત_Comp){
    પાછા uhash_set_કિંમત_Comparator(hash, કિંમત_Comp);
}

અંતઃરેખ UBool Hashtable::equals(અચલ Hashtable& that)અચલ{
   પાછા uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


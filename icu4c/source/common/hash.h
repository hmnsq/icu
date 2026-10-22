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

#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : ସର୍ବସାଧାରଣ UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ init(UHashFunction *ଚାବି_Hash, UKeyComparator *ଚାବି_Comp, UValueComparator *ମୂଲ୍ୟ_Comp, UErrorCode& status);

    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ initSize(UHashFunction *ଚାବି_Hash, UKeyComparator *ଚାବି_Comp, UValueComparator *ମୂଲ୍ୟ_Comp, int32_t size, UErrorCode& status);

ସର୍ବସାଧାରଣ:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    ଧାଡ଼ିଭିତରେ Hashtable(UBool ignore_ଚାବି_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    ଧାଡ଼ିଭିତରେ Hashtable(UBool ignore_ଚାବି_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    ଧାଡ଼ିଭିତରେ Hashtable(UKeyComparator *ଚାବି_Comp, UValueComparator *ମୂଲ୍ୟ_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    ଧାଡ଼ିଭିତରେ Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    ଧାଡ଼ିଭିତରେ Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    ଧାଡ଼ିଭିତରେ ~Hashtable();

    ଧାଡ଼ିଭିତରେ UObjectDeleter *set_ମୂଲ୍ୟ_Deleter(UObjectDeleter *fn);

    ଧାଡ଼ିଭିତରେ int32_t ଗଣନା_2() ଅଚଳ;

    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* put(ଅଚଳ UnicodeString& ଚାବି, ଶୂନ୍ୟ* ମୂଲ୍ୟ_2, UErrorCode& status);

    ଧାଡ଼ିଭିତରେ int32_t puti(ଅଚଳ UnicodeString& ଚାବି, int32_t ମୂଲ୍ୟ_2, UErrorCode& status);

    ଧାଡ଼ିଭିତରେ int32_t putiAllowZero(ଅଚଳ UnicodeString& ଚାବି, int32_t ମୂଲ୍ୟ_2, UErrorCode& status);

    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* get(ଅଚଳ UnicodeString& ଚାବି) ଅଚଳ;

    ଧାଡ଼ିଭିତରେ int32_t geti(ଅଚଳ UnicodeString& ଚାବି) ଅଚଳ;

    ଧାଡ଼ିଭିତରେ int32_t getiAndFound(ଅଚଳ UnicodeString& ଚାବି, UBool &found) ଅଚଳ;

    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* remove(ଅଚଳ UnicodeString& ଚାବି);

    ଧାଡ଼ିଭିତରେ int32_t removei(ଅଚଳ UnicodeString& ଚାବି);

    ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ removeAll();

    ଧାଡ଼ିଭିତରେ UBool contains_ଚାବି(ଅଚଳ UnicodeString& ଚାବି) ଅଚଳ;

    ଧାଡ଼ିଭିତରେ ଅଚଳ UHashElement* find(ଅଚଳ UnicodeString& ଚାବି) ଅଚଳ;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    ଧାଡ଼ିଭିତରେ ଅଚଳ UHashElement* nextElement(int32_t& pos) ଅଚଳ;

    ଧାଡ଼ିଭିତରେ UKeyComparator* set_ଚାବି_Comparator(UKeyComparator*ଚାବି_Comp);

    ଧାଡ଼ିଭିତରେ UValueComparator* set_ମୂଲ୍ୟ_Comparator(UValueComparator* ମୂଲ୍ୟ_Comp);

    ଧାଡ଼ିଭିତରେ UBool equals(ଅଚଳ Hashtable& that) ଅଚଳ;
ବ୍ୟକ୍ତିଗତ:
    Hashtable(ଅଚଳ Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ଅଚଳ Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ Hashtable::init(UHashFunction *ଚାବି_Hash, UKeyComparator *ଚାବି_Comp,
                            UValueComparator *ମୂଲ୍ୟ_Comp, UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    uhash_init(&hashObj, ଚାବି_Hash, ଚାବି_Comp, ମୂଲ୍ୟ_Comp, &status);
    ଯଦି (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_ଚାବି_Deleter(hash, uprv_deleteUObject);
    }
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ Hashtable::initSize(UHashFunction *ଚାବି_Hash, UKeyComparator *ଚାବି_Comp,
                                UValueComparator *ମୂଲ୍ୟ_Comp, int32_t size, UErrorCode& status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    uhash_initSize(&hashObj, ଚାବି_Hash, ଚାବି_Comp, ମୂଲ୍ୟ_Comp, size, &status);
    ଯଦି (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_ଚାବି_Deleter(hash, uprv_deleteUObject);
    }
}

ଧାଡ଼ିଭିତରେ Hashtable::Hashtable(UKeyComparator *ଚାବି_Comp, UValueComparator *ମୂଲ୍ୟ_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, ଚାବି_Comp, ମୂଲ୍ୟ_Comp, status);
}

ଧାଡ଼ିଭିତରେ Hashtable::Hashtable(UBool ignore_ଚାବି_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_ଚାବି_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ଚାବି_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

ଧାଡ଼ିଭିତରେ Hashtable::Hashtable(UBool ignore_ଚାବି_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_ଚାବି_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ଚାବି_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

ଧାଡ଼ିଭିତରେ Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

ଧାଡ଼ିଭିତରେ Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

ଧାଡ଼ିଭିତରେ Hashtable::~Hashtable() {
    ଯଦି (hash != nullptr) {
        uhash_close(hash);
    }
}

ଧାଡ଼ିଭିତରେ UObjectDeleter *Hashtable::set_ମୂଲ୍ୟ_Deleter(UObjectDeleter *fn) {
    ଫେରନ୍ତୁ uhash_set_ମୂଲ୍ୟ_Deleter(hash, fn);
}

ଧାଡ଼ିଭିତରେ int32_t Hashtable::ଗଣନା_2() ଅଚଳ {
    ଫେରନ୍ତୁ uhash_ଗଣନା(hash);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* Hashtable::put(ଅଚଳ UnicodeString& ଚାବି, ଶୂନ୍ୟ* ମୂଲ୍ୟ_2, UErrorCode& status) {
    ଫେରନ୍ତୁ uhash_put(hash, new UnicodeString(ଚାବି), ମୂଲ୍ୟ_2, &status);
}

ଧାଡ଼ିଭିତରେ int32_t Hashtable::puti(ଅଚଳ UnicodeString& ଚାବି, int32_t ମୂଲ୍ୟ_2, UErrorCode& status) {
    ଫେରନ୍ତୁ uhash_puti(hash, new UnicodeString(ଚାବି), ମୂଲ୍ୟ_2, &status);
}

ଧାଡ଼ିଭିତରେ int32_t Hashtable::putiAllowZero(ଅଚଳ UnicodeString& ଚାବି, int32_t ମୂଲ୍ୟ_2,
                                        UErrorCode& status) {
    ଫେରନ୍ତୁ uhash_putiAllowZero(hash, new UnicodeString(ଚାବି), ମୂଲ୍ୟ_2, &status);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* Hashtable::get(ଅଚଳ UnicodeString& ଚାବି) ଅଚଳ {
    ଫେରନ୍ତୁ uhash_get(hash, &ଚାବି);
}

ଧାଡ଼ିଭିତରେ int32_t Hashtable::geti(ଅଚଳ UnicodeString& ଚାବି) ଅଚଳ {
    ଫେରନ୍ତୁ uhash_geti(hash, &ଚାବି);
}

ଧାଡ଼ିଭିତରେ int32_t Hashtable::getiAndFound(ଅଚଳ UnicodeString& ଚାବି, UBool &found) ଅଚଳ {
    ଫେରନ୍ତୁ uhash_getiAndFound(hash, &ଚାବି, &found);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* Hashtable::remove(ଅଚଳ UnicodeString& ଚାବି) {
    ଫେରନ୍ତୁ uhash_remove(hash, &ଚାବି);
}

ଧାଡ଼ିଭିତରେ int32_t Hashtable::removei(ଅଚଳ UnicodeString& ଚାବି) {
    ଫେରନ୍ତୁ uhash_removei(hash, &ଚାବି);
}

ଧାଡ଼ିଭିତରେ UBool Hashtable::contains_ଚାବି(ଅଚଳ UnicodeString& ଚାବି) ଅଚଳ {
    ଫେରନ୍ତୁ uhash_contains_ଚାବି(hash, &ଚାବି);
}

ଧାଡ଼ିଭିତରେ ଅଚଳ UHashElement* Hashtable::find(ଅଚଳ UnicodeString& ଚାବି) ଅଚଳ {
    ଫେରନ୍ତୁ uhash_find(hash, &ଚାବି);
}

ଧାଡ଼ିଭିତରେ ଅଚଳ UHashElement* Hashtable::nextElement(int32_t& pos) ଅଚଳ {
    ଫେରନ୍ତୁ uhash_nextElement(hash, &pos);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

ଧାଡ଼ିଭିତରେ UKeyComparator* Hashtable::set_ଚାବି_Comparator(UKeyComparator*ଚାବି_Comp){
    ଫେରନ୍ତୁ uhash_set_ଚାବି_Comparator(hash, ଚାବି_Comp);
}

ଧାଡ଼ିଭିତରେ UValueComparator* Hashtable::set_ମୂଲ୍ୟ_Comparator(UValueComparator* ମୂଲ୍ୟ_Comp){
    ଫେରନ୍ତୁ uhash_set_ମୂଲ୍ୟ_Comparator(hash, ମୂଲ୍ୟ_Comp);
}

ଧାଡ଼ିଭିତରେ UBool Hashtable::equals(ଅଚଳ Hashtable& that)ଅଚଳ{
   ଫେରନ୍ତୁ uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


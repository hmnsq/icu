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

#شامل "unicode/unistr.h"
#شامل "unicode/uobject.h"
#شامل "cmemory.h"
#شامل "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : عمومی UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    درونخطی تهی init(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp, UValueComparator *مقدار_Comp, UErrorCode& status);

    درونخطی تهی initSize(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp, UValueComparator *مقدار_Comp, int32_t size, UErrorCode& status);

عمومی:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    درونخطی Hashtable(UBool ignore_کلید_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    درونخطی Hashtable(UBool ignore_کلید_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    درونخطی Hashtable(UKeyComparator *کلید_Comp, UValueComparator *مقدار_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    درونخطی Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    درونخطی Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    درونخطی ~Hashtable();

    درونخطی UObjectDeleter *set_مقدار_Deleter(UObjectDeleter *fn);

    درونخطی int32_t شمار() ثابت;

    درونخطی تهی* put(ثابت UnicodeString& کلید, تهی* مقدار_2, UErrorCode& status);

    درونخطی int32_t puti(ثابت UnicodeString& کلید, int32_t مقدار_2, UErrorCode& status);

    درونخطی int32_t putiAllowZero(ثابت UnicodeString& کلید, int32_t مقدار_2, UErrorCode& status);

    درونخطی تهی* get(ثابت UnicodeString& کلید) ثابت;

    درونخطی int32_t geti(ثابت UnicodeString& کلید) ثابت;

    درونخطی int32_t getiAndFound(ثابت UnicodeString& کلید, UBool &found) ثابت;

    درونخطی تهی* remove(ثابت UnicodeString& کلید);

    درونخطی int32_t removei(ثابت UnicodeString& کلید);

    درونخطی تهی removeAll();

    درونخطی UBool contains_کلید(ثابت UnicodeString& کلید) ثابت;

    درونخطی ثابت UHashElement* find(ثابت UnicodeString& کلید) ثابت;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    درونخطی ثابت UHashElement* nextElement(int32_t& pos) ثابت;

    درونخطی UKeyComparator* set_کلید_Comparator(UKeyComparator*کلید_Comp);

    درونخطی UValueComparator* set_مقدار_Comparator(UValueComparator* مقدار_Comp);

    درونخطی UBool equals(ثابت Hashtable& that) ثابت;
خصوصی:
    Hashtable(ثابت Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(ثابت Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

درونخطی تهی Hashtable::init(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp,
                            UValueComparator *مقدار_Comp, UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    uhash_init(&hashObj, کلید_Hash, کلید_Comp, مقدار_Comp, &status);
    اگر (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_کلید_Deleter(hash, uprv_deleteUObject);
    }
}

درونخطی تهی Hashtable::initSize(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp,
                                UValueComparator *مقدار_Comp, int32_t size, UErrorCode& status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    uhash_initSize(&hashObj, کلید_Hash, کلید_Comp, مقدار_Comp, size, &status);
    اگر (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_کلید_Deleter(hash, uprv_deleteUObject);
    }
}

درونخطی Hashtable::Hashtable(UKeyComparator *کلید_Comp, UValueComparator *مقدار_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, کلید_Comp, مقدار_Comp, status);
}

درونخطی Hashtable::Hashtable(UBool ignore_کلید_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_کلید_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلید_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

درونخطی Hashtable::Hashtable(UBool ignore_کلید_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_کلید_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلید_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

درونخطی Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

درونخطی Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

درونخطی Hashtable::~Hashtable() {
    اگر (hash != nullptr) {
        uhash_close(hash);
    }
}

درونخطی UObjectDeleter *Hashtable::set_مقدار_Deleter(UObjectDeleter *fn) {
    بازگشت uhash_set_مقدار_Deleter(hash, fn);
}

درونخطی int32_t Hashtable::شمار() ثابت {
    بازگشت uhash_شمار(hash);
}

درونخطی تهی* Hashtable::put(ثابت UnicodeString& کلید, تهی* مقدار_2, UErrorCode& status) {
    بازگشت uhash_put(hash, new UnicodeString(کلید), مقدار_2, &status);
}

درونخطی int32_t Hashtable::puti(ثابت UnicodeString& کلید, int32_t مقدار_2, UErrorCode& status) {
    بازگشت uhash_puti(hash, new UnicodeString(کلید), مقدار_2, &status);
}

درونخطی int32_t Hashtable::putiAllowZero(ثابت UnicodeString& کلید, int32_t مقدار_2,
                                        UErrorCode& status) {
    بازگشت uhash_putiAllowZero(hash, new UnicodeString(کلید), مقدار_2, &status);
}

درونخطی تهی* Hashtable::get(ثابت UnicodeString& کلید) ثابت {
    بازگشت uhash_get(hash, &کلید);
}

درونخطی int32_t Hashtable::geti(ثابت UnicodeString& کلید) ثابت {
    بازگشت uhash_geti(hash, &کلید);
}

درونخطی int32_t Hashtable::getiAndFound(ثابت UnicodeString& کلید, UBool &found) ثابت {
    بازگشت uhash_getiAndFound(hash, &کلید, &found);
}

درونخطی تهی* Hashtable::remove(ثابت UnicodeString& کلید) {
    بازگشت uhash_remove(hash, &کلید);
}

درونخطی int32_t Hashtable::removei(ثابت UnicodeString& کلید) {
    بازگشت uhash_removei(hash, &کلید);
}

درونخطی UBool Hashtable::contains_کلید(ثابت UnicodeString& کلید) ثابت {
    بازگشت uhash_contains_کلید(hash, &کلید);
}

درونخطی ثابت UHashElement* Hashtable::find(ثابت UnicodeString& کلید) ثابت {
    بازگشت uhash_find(hash, &کلید);
}

درونخطی ثابت UHashElement* Hashtable::nextElement(int32_t& pos) ثابت {
    بازگشت uhash_nextElement(hash, &pos);
}

درونخطی تهی Hashtable::removeAll() {
    uhash_removeAll(hash);
}

درونخطی UKeyComparator* Hashtable::set_کلید_Comparator(UKeyComparator*کلید_Comp){
    بازگشت uhash_set_کلید_Comparator(hash, کلید_Comp);
}

درونخطی UValueComparator* Hashtable::set_مقدار_Comparator(UValueComparator* مقدار_Comp){
    بازگشت uhash_set_مقدار_Comparator(hash, مقدار_Comp);
}

درونخطی UBool Hashtable::equals(ثابت Hashtable& that)ثابت{
   بازگشت uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


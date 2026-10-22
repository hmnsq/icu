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

#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : பொது UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    உள்வரி வெறுமை init(UHashFunction *சாவி_Hash, UKeyComparator *சாவி_Comp, UValueComparator *மதிப்பு_Comp, UErrorCode& status);

    உள்வரி வெறுமை initSize(UHashFunction *சாவி_Hash, UKeyComparator *சாவி_Comp, UValueComparator *மதிப்பு_Comp, int32_t size, UErrorCode& status);

பொது:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    உள்வரி Hashtable(UBool ignore_சாவி_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    உள்வரி Hashtable(UBool ignore_சாவி_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    உள்வரி Hashtable(UKeyComparator *சாவி_Comp, UValueComparator *மதிப்பு_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    உள்வரி Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    உள்வரி Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    உள்வரி ~Hashtable();

    உள்வரி UObjectDeleter *set_மதிப்பு_Deleter(UObjectDeleter *fn);

    உள்வரி int32_t மொத்தம்() மாறிலி;

    உள்வரி வெறுமை* put(மாறிலி UnicodeString& சாவி, வெறுமை* மதிப்பு_2, UErrorCode& status);

    உள்வரி int32_t puti(மாறிலி UnicodeString& சாவி, int32_t மதிப்பு_2, UErrorCode& status);

    உள்வரி int32_t putiAllowZero(மாறிலி UnicodeString& சாவி, int32_t மதிப்பு_2, UErrorCode& status);

    உள்வரி வெறுமை* get(மாறிலி UnicodeString& சாவி) மாறிலி;

    உள்வரி int32_t geti(மாறிலி UnicodeString& சாவி) மாறிலி;

    உள்வரி int32_t getiAndFound(மாறிலி UnicodeString& சாவி, UBool &found) மாறிலி;

    உள்வரி வெறுமை* remove(மாறிலி UnicodeString& சாவி);

    உள்வரி int32_t removei(மாறிலி UnicodeString& சாவி);

    உள்வரி வெறுமை removeAll();

    உள்வரி UBool contains_சாவி(மாறிலி UnicodeString& சாவி) மாறிலி;

    உள்வரி மாறிலி UHashElement* find(மாறிலி UnicodeString& சாவி) மாறிலி;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    உள்வரி மாறிலி UHashElement* nextElement(int32_t& pos) மாறிலி;

    உள்வரி UKeyComparator* set_சாவி_Comparator(UKeyComparator*சாவி_Comp);

    உள்வரி UValueComparator* set_மதிப்பு_Comparator(UValueComparator* மதிப்பு_Comp);

    உள்வரி UBool equals(மாறிலி Hashtable& that) மாறிலி;
தனிப்பட்ட:
    Hashtable(மாறிலி Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(மாறிலி Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

உள்வரி வெறுமை Hashtable::init(UHashFunction *சாவி_Hash, UKeyComparator *சாவி_Comp,
                            UValueComparator *மதிப்பு_Comp, UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    uhash_init(&hashObj, சாவி_Hash, சாவி_Comp, மதிப்பு_Comp, &status);
    எனில் (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_சாவி_Deleter(hash, uprv_deleteUObject);
    }
}

உள்வரி வெறுமை Hashtable::initSize(UHashFunction *சாவி_Hash, UKeyComparator *சாவி_Comp,
                                UValueComparator *மதிப்பு_Comp, int32_t size, UErrorCode& status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    uhash_initSize(&hashObj, சாவி_Hash, சாவி_Comp, மதிப்பு_Comp, size, &status);
    எனில் (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_சாவி_Deleter(hash, uprv_deleteUObject);
    }
}

உள்வரி Hashtable::Hashtable(UKeyComparator *சாவி_Comp, UValueComparator *மதிப்பு_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, சாவி_Comp, மதிப்பு_Comp, status);
}

உள்வரி Hashtable::Hashtable(UBool ignore_சாவி_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_சாவி_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_சாவி_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

உள்வரி Hashtable::Hashtable(UBool ignore_சாவி_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_சாவி_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_சாவி_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

உள்வரி Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

உள்வரி Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

உள்வரி Hashtable::~Hashtable() {
    எனில் (hash != nullptr) {
        uhash_close(hash);
    }
}

உள்வரி UObjectDeleter *Hashtable::set_மதிப்பு_Deleter(UObjectDeleter *fn) {
    திரும்பு uhash_set_மதிப்பு_Deleter(hash, fn);
}

உள்வரி int32_t Hashtable::மொத்தம்() மாறிலி {
    திரும்பு uhash_மொத்தம்(hash);
}

உள்வரி வெறுமை* Hashtable::put(மாறிலி UnicodeString& சாவி, வெறுமை* மதிப்பு_2, UErrorCode& status) {
    திரும்பு uhash_put(hash, new UnicodeString(சாவி), மதிப்பு_2, &status);
}

உள்வரி int32_t Hashtable::puti(மாறிலி UnicodeString& சாவி, int32_t மதிப்பு_2, UErrorCode& status) {
    திரும்பு uhash_puti(hash, new UnicodeString(சாவி), மதிப்பு_2, &status);
}

உள்வரி int32_t Hashtable::putiAllowZero(மாறிலி UnicodeString& சாவி, int32_t மதிப்பு_2,
                                        UErrorCode& status) {
    திரும்பு uhash_putiAllowZero(hash, new UnicodeString(சாவி), மதிப்பு_2, &status);
}

உள்வரி வெறுமை* Hashtable::get(மாறிலி UnicodeString& சாவி) மாறிலி {
    திரும்பு uhash_get(hash, &சாவி);
}

உள்வரி int32_t Hashtable::geti(மாறிலி UnicodeString& சாவி) மாறிலி {
    திரும்பு uhash_geti(hash, &சாவி);
}

உள்வரி int32_t Hashtable::getiAndFound(மாறிலி UnicodeString& சாவி, UBool &found) மாறிலி {
    திரும்பு uhash_getiAndFound(hash, &சாவி, &found);
}

உள்வரி வெறுமை* Hashtable::remove(மாறிலி UnicodeString& சாவி) {
    திரும்பு uhash_remove(hash, &சாவி);
}

உள்வரி int32_t Hashtable::removei(மாறிலி UnicodeString& சாவி) {
    திரும்பு uhash_removei(hash, &சாவி);
}

உள்வரி UBool Hashtable::contains_சாவி(மாறிலி UnicodeString& சாவி) மாறிலி {
    திரும்பு uhash_contains_சாவி(hash, &சாவி);
}

உள்வரி மாறிலி UHashElement* Hashtable::find(மாறிலி UnicodeString& சாவி) மாறிலி {
    திரும்பு uhash_find(hash, &சாவி);
}

உள்வரி மாறிலி UHashElement* Hashtable::nextElement(int32_t& pos) மாறிலி {
    திரும்பு uhash_nextElement(hash, &pos);
}

உள்வரி வெறுமை Hashtable::removeAll() {
    uhash_removeAll(hash);
}

உள்வரி UKeyComparator* Hashtable::set_சாவி_Comparator(UKeyComparator*சாவி_Comp){
    திரும்பு uhash_set_சாவி_Comparator(hash, சாவி_Comp);
}

உள்வரி UValueComparator* Hashtable::set_மதிப்பு_Comparator(UValueComparator* மதிப்பு_Comp){
    திரும்பு uhash_set_மதிப்பு_Comparator(hash, மதிப்பு_Comp);
}

உள்வரி UBool Hashtable::equals(மாறிலி Hashtable& that)மாறிலி{
   திரும்பு uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


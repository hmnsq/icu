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

#अगर_अघोषित HASH_H
#घोषणा HASH_H

#समावेश "unicode/unistr.h"
#समावेश "unicode/uobject.h"
#समावेश "cmemory.h"
#समावेश "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : खुला UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    अंतरभूत व्योम init(UHashFunction *कुंजी_Hash, UKeyComparator *कुंजी_Comp, UValueComparator *मूल्य_Comp, UErrorCode& status);

    अंतरभूत व्योम init_आकार(UHashFunction *कुंजी_Hash, UKeyComparator *कुंजी_Comp, UValueComparator *मूल्य_Comp, int32_t आकार_2, UErrorCode& status);

खुला:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    अंतरभूत Hashtable(UBool ignore_कुंजी_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    अंतरभूत Hashtable(UBool ignore_कुंजी_Case, int32_t आकार_2, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    अंतरभूत Hashtable(UKeyComparator *कुंजी_Comp, UValueComparator *मूल्य_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    अंतरभूत Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    अंतरभूत Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    अंतरभूत ~Hashtable();

    अंतरभूत UObjectDeleter *set_मूल्य_Deleter(UObjectDeleter *fn);

    अंतरभूत int32_t गिनती() स्थिर;

    अंतरभूत व्योम* put(स्थिर UnicodeString& कुंजी, व्योम* मूल्य_2, UErrorCode& status);

    अंतरभूत int32_t puti(स्थिर UnicodeString& कुंजी, int32_t मूल्य_2, UErrorCode& status);

    अंतरभूत int32_t putiAllowZero(स्थिर UnicodeString& कुंजी, int32_t मूल्य_2, UErrorCode& status);

    अंतरभूत व्योम* get(स्थिर UnicodeString& कुंजी) स्थिर;

    अंतरभूत int32_t geti(स्थिर UnicodeString& कुंजी) स्थिर;

    अंतरभूत int32_t getiAndFound(स्थिर UnicodeString& कुंजी, UBool &found) स्थिर;

    अंतरभूत व्योम* हटाओ(स्थिर UnicodeString& कुंजी);

    अंतरभूत int32_t removei(स्थिर UnicodeString& कुंजी);

    अंतरभूत व्योम removeAll();

    अंतरभूत UBool contains_कुंजी(स्थिर UnicodeString& कुंजी) स्थिर;

    अंतरभूत स्थिर UHashElement* find(स्थिर UnicodeString& कुंजी) स्थिर;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    अंतरभूत स्थिर UHashElement* अग्र_Element(int32_t& pos) स्थिर;

    अंतरभूत UKeyComparator* set_कुंजी_Comparator(UKeyComparator*कुंजी_Comp);

    अंतरभूत UValueComparator* set_मूल्य_Comparator(UValueComparator* मूल्य_Comp);

    अंतरभूत UBool equals(स्थिर Hashtable& that) स्थिर;
निजी:
    Hashtable(स्थिर Hashtable &other) = delete; // forbid copying of this class
    Hashtable &चालक=(स्थिर Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

अंतरभूत व्योम Hashtable::init(UHashFunction *कुंजी_Hash, UKeyComparator *कुंजी_Comp,
                            UValueComparator *मूल्य_Comp, UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    uhash_init(&hashObj, कुंजी_Hash, कुंजी_Comp, मूल्य_Comp, &status);
    अगर (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_कुंजी_Deleter(hash, uprv_deleteUObject);
    }
}

अंतरभूत व्योम Hashtable::init_आकार(UHashFunction *कुंजी_Hash, UKeyComparator *कुंजी_Comp,
                                UValueComparator *मूल्य_Comp, int32_t आकार_2, UErrorCode& status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    uhash_init_आकार(&hashObj, कुंजी_Hash, कुंजी_Comp, मूल्य_Comp, आकार_2, &status);
    अगर (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_कुंजी_Deleter(hash, uprv_deleteUObject);
    }
}

अंतरभूत Hashtable::Hashtable(UKeyComparator *कुंजी_Comp, UValueComparator *मूल्य_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, कुंजी_Comp, मूल्य_Comp, status);
}

अंतरभूत Hashtable::Hashtable(UBool ignore_कुंजी_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_कुंजी_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_कुंजी_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

अंतरभूत Hashtable::Hashtable(UBool ignore_कुंजी_Case, int32_t आकार_2, UErrorCode& status)
 : hash(nullptr)
{
    init_आकार(ignore_कुंजी_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_कुंजी_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, आकार_2,
            status);
}

अंतरभूत Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

अंतरभूत Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

अंतरभूत Hashtable::~Hashtable() {
    अगर (hash != nullptr) {
        uhash_close(hash);
    }
}

अंतरभूत UObjectDeleter *Hashtable::set_मूल्य_Deleter(UObjectDeleter *fn) {
    वापस uhash_set_मूल्य_Deleter(hash, fn);
}

अंतरभूत int32_t Hashtable::गिनती() स्थिर {
    वापस uhash_गिनती(hash);
}

अंतरभूत व्योम* Hashtable::put(स्थिर UnicodeString& कुंजी, व्योम* मूल्य_2, UErrorCode& status) {
    वापस uhash_put(hash, new UnicodeString(कुंजी), मूल्य_2, &status);
}

अंतरभूत int32_t Hashtable::puti(स्थिर UnicodeString& कुंजी, int32_t मूल्य_2, UErrorCode& status) {
    वापस uhash_puti(hash, new UnicodeString(कुंजी), मूल्य_2, &status);
}

अंतरभूत int32_t Hashtable::putiAllowZero(स्थिर UnicodeString& कुंजी, int32_t मूल्य_2,
                                        UErrorCode& status) {
    वापस uhash_putiAllowZero(hash, new UnicodeString(कुंजी), मूल्य_2, &status);
}

अंतरभूत व्योम* Hashtable::get(स्थिर UnicodeString& कुंजी) स्थिर {
    वापस uhash_get(hash, &कुंजी);
}

अंतरभूत int32_t Hashtable::geti(स्थिर UnicodeString& कुंजी) स्थिर {
    वापस uhash_geti(hash, &कुंजी);
}

अंतरभूत int32_t Hashtable::getiAndFound(स्थिर UnicodeString& कुंजी, UBool &found) स्थिर {
    वापस uhash_getiAndFound(hash, &कुंजी, &found);
}

अंतरभूत व्योम* Hashtable::हटाओ(स्थिर UnicodeString& कुंजी) {
    वापस uhash_remove(hash, &कुंजी);
}

अंतरभूत int32_t Hashtable::removei(स्थिर UnicodeString& कुंजी) {
    वापस uhash_removei(hash, &कुंजी);
}

अंतरभूत UBool Hashtable::contains_कुंजी(स्थिर UnicodeString& कुंजी) स्थिर {
    वापस uhash_contains_कुंजी(hash, &कुंजी);
}

अंतरभूत स्थिर UHashElement* Hashtable::find(स्थिर UnicodeString& कुंजी) स्थिर {
    वापस uhash_find(hash, &कुंजी);
}

अंतरभूत स्थिर UHashElement* Hashtable::अग्र_Element(int32_t& pos) स्थिर {
    वापस uhash_अग्र_Element(hash, &pos);
}

अंतरभूत व्योम Hashtable::removeAll() {
    uhash_removeAll(hash);
}

अंतरभूत UKeyComparator* Hashtable::set_कुंजी_Comparator(UKeyComparator*कुंजी_Comp){
    वापस uhash_set_कुंजी_Comparator(hash, कुंजी_Comp);
}

अंतरभूत UValueComparator* Hashtable::set_मूल्य_Comparator(UValueComparator* मूल्य_Comp){
    वापस uhash_set_मूल्य_Comparator(hash, मूल्य_Comp);
}

अंतरभूत UBool Hashtable::equals(स्थिर Hashtable& that)स्थिर{
   वापस uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#पूर्ण_अगर


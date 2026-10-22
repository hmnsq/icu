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

#include "unicode/unistr.h"
#include "unicode/uobject.h"
#include "cmemory.h"
#include "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : public UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    inline void init(UHashFunction *सुसि_Hash, UKeyComparator *सुसि_Comp, UValueComparator *हालागा_Comp, UErrorCode& status);

    inline void initSize(UHashFunction *सुसि_Hash, UKeyComparator *सुसि_Comp, UValueComparator *हालागा_Comp, int32_t size, UErrorCode& status);

public:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    inline Hashtable(UBool ignore_सुसि_Case, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    inline Hashtable(UBool ignore_सुसि_Case, int32_t size, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    inline Hashtable(UKeyComparator *सुसि_Comp, UValueComparator *हालागा_Comp, UErrorCode& status);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    inline Hashtable(UErrorCode& status);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    inline Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    inline ~Hashtable();

    inline UObjectDeleter *set_हालागा_Deleter(UObjectDeleter *fn);

    inline int32_t बिलांग() const;

    inline void* put(const UnicodeString& सुसि, void* हालागा_2, UErrorCode& status);

    inline int32_t puti(const UnicodeString& सुसि, int32_t हालागा_2, UErrorCode& status);

    inline int32_t putiAllowZero(const UnicodeString& सुसि, int32_t हालागा_2, UErrorCode& status);

    inline void* get(const UnicodeString& सुसि) const;

    inline int32_t geti(const UnicodeString& सुसि) const;

    inline int32_t getiAndFound(const UnicodeString& सुसि, UBool &found) const;

    inline void* remove(const UnicodeString& सुसि);

    inline int32_t removei(const UnicodeString& सुसि);

    inline void removeAll();

    inline UBool contains_सुसि(const UnicodeString& सुसि) const;

    inline const UHashElement* find(const UnicodeString& सुसि) const;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    inline const UHashElement* nextElement(int32_t& pos) const;

    inline UKeyComparator* set_सुसि_Comparator(UKeyComparator*सुसि_Comp);

    inline UValueComparator* set_हालागा_Comparator(UValueComparator* हालागा_Comp);

    inline UBool equals(const Hashtable& that) const;
private:
    Hashtable(const Hashtable &other) = delete; // forbid copying of this class
    Hashtable &operator=(const Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

inline void Hashtable::init(UHashFunction *सुसि_Hash, UKeyComparator *सुसि_Comp,
                            UValueComparator *हालागा_Comp, UErrorCode& status) {
    if (U_FAILURE(status)) {
        return;
    }
    uhash_init(&hashObj, सुसि_Hash, सुसि_Comp, हालागा_Comp, &status);
    if (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_सुसि_Deleter(hash, uprv_deleteUObject);
    }
}

inline void Hashtable::initSize(UHashFunction *सुसि_Hash, UKeyComparator *सुसि_Comp,
                                UValueComparator *हालागा_Comp, int32_t size, UErrorCode& status) {
    if (U_FAILURE(status)) {
        return;
    }
    uhash_initSize(&hashObj, सुसि_Hash, सुसि_Comp, हालागा_Comp, size, &status);
    if (U_SUCCESS(status)) {
        hash = &hashObj;
        uhash_set_सुसि_Deleter(hash, uprv_deleteUObject);
    }
}

inline Hashtable::Hashtable(UKeyComparator *सुसि_Comp, UValueComparator *हालागा_Comp,
                 UErrorCode& status) : hash(nullptr) {
    init( uhash_hashUnicodeString, सुसि_Comp, हालागा_Comp, status);
}

inline Hashtable::Hashtable(UBool ignore_सुसि_Case, UErrorCode& status)
 : hash(nullptr)
{
    init(ignore_सुसि_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_सुसि_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            status);
}

inline Hashtable::Hashtable(UBool ignore_सुसि_Case, int32_t size, UErrorCode& status)
 : hash(nullptr)
{
    initSize(ignore_सुसि_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_सुसि_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, size,
            status);
}

inline Hashtable::Hashtable(UErrorCode& status)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

inline Hashtable::Hashtable()
 : hash(nullptr)
{
    UErrorCode status = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, status);
}

inline Hashtable::~Hashtable() {
    if (hash != nullptr) {
        uhash_close(hash);
    }
}

inline UObjectDeleter *Hashtable::set_हालागा_Deleter(UObjectDeleter *fn) {
    return uhash_set_हालागा_Deleter(hash, fn);
}

inline int32_t Hashtable::बिलांग() const {
    return uhash_बिलांग(hash);
}

inline void* Hashtable::put(const UnicodeString& सुसि, void* हालागा_2, UErrorCode& status) {
    return uhash_put(hash, new UnicodeString(सुसि), हालागा_2, &status);
}

inline int32_t Hashtable::puti(const UnicodeString& सुसि, int32_t हालागा_2, UErrorCode& status) {
    return uhash_puti(hash, new UnicodeString(सुसि), हालागा_2, &status);
}

inline int32_t Hashtable::putiAllowZero(const UnicodeString& सुसि, int32_t हालागा_2,
                                        UErrorCode& status) {
    return uhash_putiAllowZero(hash, new UnicodeString(सुसि), हालागा_2, &status);
}

inline void* Hashtable::get(const UnicodeString& सुसि) const {
    return uhash_get(hash, &सुसि);
}

inline int32_t Hashtable::geti(const UnicodeString& सुसि) const {
    return uhash_geti(hash, &सुसि);
}

inline int32_t Hashtable::getiAndFound(const UnicodeString& सुसि, UBool &found) const {
    return uhash_getiAndFound(hash, &सुसि, &found);
}

inline void* Hashtable::remove(const UnicodeString& सुसि) {
    return uhash_remove(hash, &सुसि);
}

inline int32_t Hashtable::removei(const UnicodeString& सुसि) {
    return uhash_removei(hash, &सुसि);
}

inline UBool Hashtable::contains_सुसि(const UnicodeString& सुसि) const {
    return uhash_contains_सुसि(hash, &सुसि);
}

inline const UHashElement* Hashtable::find(const UnicodeString& सुसि) const {
    return uhash_find(hash, &सुसि);
}

inline const UHashElement* Hashtable::nextElement(int32_t& pos) const {
    return uhash_nextElement(hash, &pos);
}

inline void Hashtable::removeAll() {
    uhash_removeAll(hash);
}

inline UKeyComparator* Hashtable::set_सुसि_Comparator(UKeyComparator*सुसि_Comp){
    return uhash_set_सुसि_Comparator(hash, सुसि_Comp);
}

inline UValueComparator* Hashtable::set_हालागा_Comparator(UValueComparator* हालागा_Comp){
    return uhash_set_हालागा_Comparator(hash, हालागा_Comp);
}

inline UBool Hashtable::equals(const Hashtable& that)const{
   return uhash_equals(hash, that.hash);
}
U_NAMESPACE_END

#endif


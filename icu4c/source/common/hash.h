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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HASH_H
#ವ್ಯಾಖ್ಯಾನಿಸು HASH_H

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : ಸಾರ್ವಜನಿಕ UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ init(UHashFunction *ಕೀಲಿ_Hash, UKeyComparator *ಕೀಲಿ_Comp, UValueComparator *ಮೌಲ್ಯ_Comp, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ init_ಗಾತ್ರ(UHashFunction *ಕೀಲಿ_Hash, UKeyComparator *ಕೀಲಿ_Comp, UValueComparator *ಮೌಲ್ಯ_Comp, int32_t ಗಾತ್ರ_3, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

ಸಾರ್ವಜನಿಕ:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    ಸಾಲೊಳಗೆ Hashtable(UBool ignore_ಕೀಲಿ_Case, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    ಸಾಲೊಳಗೆ Hashtable(UBool ignore_ಕೀಲಿ_Case, int32_t ಗಾತ್ರ_3, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    ಸಾಲೊಳಗೆ Hashtable(UKeyComparator *ಕೀಲಿ_Comp, UValueComparator *ಮೌಲ್ಯ_Comp, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    ಸಾಲೊಳಗೆ Hashtable(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    ಸಾಲೊಳಗೆ Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    ಸಾಲೊಳಗೆ ~Hashtable();

    ಸಾಲೊಳಗೆ UObjectDeleter *ಗಣ_ಮೌಲ್ಯ_Deleter(UObjectDeleter *fn);

    ಸಾಲೊಳಗೆ int32_t ಎಣಿಕೆ_2() ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ* put(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ, ಶೂನ್ಯ* ಮೌಲ್ಯ_2, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    ಸಾಲೊಳಗೆ int32_t puti(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ, int32_t ಮೌಲ್ಯ_2, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    ಸಾಲೊಳಗೆ int32_t putiAllowZero(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ, int32_t ಮೌಲ್ಯ_2, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ* get(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ int32_t geti(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ int32_t geti_And_ಸಿಕ್ಕಿತು(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ, UBool &ಸಿಕ್ಕಿತು) ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ* remove(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ);

    ಸಾಲೊಳಗೆ int32_t removei(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ);

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ removeAll();

    ಸಾಲೊಳಗೆ UBool ಒಳಗೊಂಡಿದೆ_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ UHashElement* ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) ಸ್ಥಿರಾಂಕ;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ UHashElement* ಮುಂದಿನ_Element(int32_t& pos) ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ UKeyComparator* ಗಣ_ಕೀಲಿ_Comparator(UKeyComparator*ಕೀಲಿ_Comp);

    ಸಾಲೊಳಗೆ UValueComparator* ಗಣ_ಮೌಲ್ಯ_Comparator(UValueComparator* ಮೌಲ್ಯ_Comp);

    ಸಾಲೊಳಗೆ UBool equals(ಸ್ಥಿರಾಂಕ Hashtable& that) ಸ್ಥಿರಾಂಕ;
ಖಾಸಗಿ:
    Hashtable(ಸ್ಥಿರಾಂಕ Hashtable &other) = delete; // forbid copying of this class
    Hashtable &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

ಸಾಲೊಳಗೆ ಶೂನ್ಯ Hashtable::init(UHashFunction *ಕೀಲಿ_Hash, UKeyComparator *ಕೀಲಿ_Comp,
                            UValueComparator *ಮೌಲ್ಯ_Comp, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    uhash_init(&hashObj, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        hash = &hashObj;
        uhash_ಗಣ_ಕೀಲಿ_Deleter(hash, uprv_deleteUObject);
    }
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ Hashtable::init_ಗಾತ್ರ(UHashFunction *ಕೀಲಿ_Hash, UKeyComparator *ಕೀಲಿ_Comp,
                                UValueComparator *ಮೌಲ್ಯ_Comp, int32_t ಗಾತ್ರ_3, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    uhash_init_ಗಾತ್ರ(&hashObj, ಕೀಲಿ_Hash, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, ಗಾತ್ರ_3, &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        hash = &hashObj;
        uhash_ಗಣ_ಕೀಲಿ_Deleter(hash, uprv_deleteUObject);
    }
}

ಸಾಲೊಳಗೆ Hashtable::Hashtable(UKeyComparator *ಕೀಲಿ_Comp, UValueComparator *ಮೌಲ್ಯ_Comp,
                 UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) : hash(nullptr) {
    init( uhash_hashUnicodeString, ಕೀಲಿ_Comp, ಮೌಲ್ಯ_Comp, ಸ್ಥಿತಿ_2);
}

ಸಾಲೊಳಗೆ Hashtable::Hashtable(UBool ignore_ಕೀಲಿ_Case, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
 : hash(nullptr)
{
    init(ignore_ಕೀಲಿ_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ಕೀಲಿ_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            ಸ್ಥಿತಿ_2);
}

ಸಾಲೊಳಗೆ Hashtable::Hashtable(UBool ignore_ಕೀಲಿ_Case, int32_t ಗಾತ್ರ_3, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
 : hash(nullptr)
{
    init_ಗಾತ್ರ(ignore_ಕೀಲಿ_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_ಕೀಲಿ_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, ಗಾತ್ರ_3,
            ಸ್ಥಿತಿ_2);
}

ಸಾಲೊಳಗೆ Hashtable::Hashtable(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, ಸ್ಥಿತಿ_2);
}

ಸಾಲೊಳಗೆ Hashtable::Hashtable()
 : hash(nullptr)
{
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, ಸ್ಥಿತಿ_2);
}

ಸಾಲೊಳಗೆ Hashtable::~Hashtable() {
    ಆದರೆ (hash != nullptr) {
        uhash_close(hash);
    }
}

ಸಾಲೊಳಗೆ UObjectDeleter *Hashtable::ಗಣ_ಮೌಲ್ಯ_Deleter(UObjectDeleter *fn) {
    ಹಿಂತಿರುಗಿ uhash_ಗಣ_ಮೌಲ್ಯ_Deleter(hash, fn);
}

ಸಾಲೊಳಗೆ int32_t Hashtable::ಎಣಿಕೆ_2() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uhash_ಎಣಿಕೆ(hash);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* Hashtable::put(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ, ಶೂನ್ಯ* ಮೌಲ್ಯ_2, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ uhash_put(hash, new UnicodeString(ಕೀಲಿ), ಮೌಲ್ಯ_2, &ಸ್ಥಿತಿ_2);
}

ಸಾಲೊಳಗೆ int32_t Hashtable::puti(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ, int32_t ಮೌಲ್ಯ_2, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ uhash_puti(hash, new UnicodeString(ಕೀಲಿ), ಮೌಲ್ಯ_2, &ಸ್ಥಿತಿ_2);
}

ಸಾಲೊಳಗೆ int32_t Hashtable::putiAllowZero(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ, int32_t ಮೌಲ್ಯ_2,
                                        UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಹಿಂತಿರುಗಿ uhash_putiAllowZero(hash, new UnicodeString(ಕೀಲಿ), ಮೌಲ್ಯ_2, &ಸ್ಥಿತಿ_2);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* Hashtable::get(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uhash_get(hash, &ಕೀಲಿ);
}

ಸಾಲೊಳಗೆ int32_t Hashtable::geti(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uhash_geti(hash, &ಕೀಲಿ);
}

ಸಾಲೊಳಗೆ int32_t Hashtable::geti_And_ಸಿಕ್ಕಿತು(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ, UBool &ಸಿಕ್ಕಿತು) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uhash_geti_And_ಸಿಕ್ಕಿತು(hash, &ಕೀಲಿ, &ಸಿಕ್ಕಿತು);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ* Hashtable::remove(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) {
    ಹಿಂತಿರುಗಿ uhash_remove(hash, &ಕೀಲಿ);
}

ಸಾಲೊಳಗೆ int32_t Hashtable::removei(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) {
    ಹಿಂತಿರುಗಿ uhash_removei(hash, &ಕೀಲಿ);
}

ಸಾಲೊಳಗೆ UBool Hashtable::ಒಳಗೊಂಡಿದೆ_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uhash_ಒಳಗೊಂಡಿದೆ_ಕೀಲಿ(hash, &ಕೀಲಿ);
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ UHashElement* Hashtable::ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ UnicodeString& ಕೀಲಿ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uhash_ಕಂಡುಹಿಡಿ(hash, &ಕೀಲಿ);
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ UHashElement* Hashtable::ಮುಂದಿನ_Element(int32_t& pos) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ uhash_ಮುಂದಿನ_Element(hash, &pos);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ Hashtable::removeAll() {
    uhash_removeAll(hash);
}

ಸಾಲೊಳಗೆ UKeyComparator* Hashtable::ಗಣ_ಕೀಲಿ_Comparator(UKeyComparator*ಕೀಲಿ_Comp){
    ಹಿಂತಿರುಗಿ uhash_ಗಣ_ಕೀಲಿ_Comparator(hash, ಕೀಲಿ_Comp);
}

ಸಾಲೊಳಗೆ UValueComparator* Hashtable::ಗಣ_ಮೌಲ್ಯ_Comparator(UValueComparator* ಮೌಲ್ಯ_Comp){
    ಹಿಂತಿರುಗಿ uhash_ಗಣ_ಮೌಲ್ಯ_Comparator(hash, ಮೌಲ್ಯ_Comp);
}

ಸಾಲೊಳಗೆ UBool Hashtable::equals(ಸ್ಥಿರಾಂಕ Hashtable& that)ಸ್ಥಿರಾಂಕ{
   ಹಿಂತಿರುಗಿ uhash_equals(hash, that.hash);
}
U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ


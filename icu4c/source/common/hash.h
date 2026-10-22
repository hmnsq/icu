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

#यदि_अपरिभाषितम् HASH_H
#परिभाषय HASH_H

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

    अन्तःस्थ शून्य init(UHashFunction *कुञ्चिका_Hash, UKeyComparator *कुञ्चिका_Comp, UValueComparator *मानम्_Comp, UError_संहिता& स्थितिः);

    अन्तःस्थ शून्य init_आकार(UHashFunction *कुञ्चिका_Hash, UKeyComparator *कुञ्चिका_Comp, UValueComparator *मानम्_Comp, int32_t आकार_2, UError_संहिता& स्थितिः);

सार्वजनिक:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UBool ignore_कुञ्चिका_Case, UError_संहिता& स्थितिः);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UBool ignore_कुञ्चिका_Case, int32_t आकार_2, UError_संहिता& स्थितिः);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UKeyComparator *कुञ्चिका_Comp, UValueComparator *मानम्_Comp, UError_संहिता& स्थितिः);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    अन्तःस्थ Hashtable(UError_संहिता& स्थितिः);

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

    अन्तःस्थ UObjectDeleter *समुच्चयः_मानम्_Deleter(UObjectDeleter *fn);

    अन्तःस्थ int32_t गणनम्() ध्रुव;

    अन्तःस्थ शून्य* put(ध्रुव UnicodeString& कुञ्चिका, शून्य* मानम्_2, UError_संहिता& स्थितिः);

    अन्तःस्थ int32_t puti(ध्रुव UnicodeString& कुञ्चिका, int32_t मानम्_2, UError_संहिता& स्थितिः);

    अन्तःस्थ int32_t putiAllowZero(ध्रुव UnicodeString& कुञ्चिका, int32_t मानम्_2, UError_संहिता& स्थितिः);

    अन्तःस्थ शून्य* get(ध्रुव UnicodeString& कुञ्चिका) ध्रुव;

    अन्तःस्थ int32_t geti(ध्रुव UnicodeString& कुञ्चिका) ध्रुव;

    अन्तःस्थ int32_t geti_And_प्राप्तम्(ध्रुव UnicodeString& कुञ्चिका, UBool &प्राप्तम्) ध्रुव;

    अन्तःस्थ शून्य* remove(ध्रुव UnicodeString& कुञ्चिका);

    अन्तःस्थ int32_t removei(ध्रुव UnicodeString& कुञ्चिका);

    अन्तःस्थ शून्य removeAll();

    अन्तःस्थ UBool अन्तर्भवति_कुञ्चिका(ध्रुव UnicodeString& कुञ्चिका) ध्रुव;

    अन्तःस्थ ध्रुव UHashElement* find(ध्रुव UnicodeString& कुञ्चिका) ध्रुव;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    अन्तःस्थ ध्रुव UHashElement* अग्र_Element(int32_t& pos) ध्रुव;

    अन्तःस्थ UKeyComparator* समुच्चयः_कुञ्चिका_Comparator(UKeyComparator*कुञ्चिका_Comp);

    अन्तःस्थ UValueComparator* समुच्चयः_मानम्_Comparator(UValueComparator* मानम्_Comp);

    अन्तःस्थ UBool equals(ध्रुव Hashtable& that) ध्रुव;
गुप्त:
    Hashtable(ध्रुव Hashtable &other) = delete; // forbid copying of this class
    Hashtable &संक्रियकः=(ध्रुव Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

अन्तःस्थ शून्य Hashtable::init(UHashFunction *कुञ्चिका_Hash, UKeyComparator *कुञ्चिका_Comp,
                            UValueComparator *मानम्_Comp, UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    uhash_init(&hashObj, कुञ्चिका_Hash, कुञ्चिका_Comp, मानम्_Comp, &स्थितिः);
    यदि (U_SUCCESS(स्थितिः)) {
        hash = &hashObj;
        uhash_समुच्चयः_कुञ्चिका_Deleter(hash, uprv_deleteUObject);
    }
}

अन्तःस्थ शून्य Hashtable::init_आकार(UHashFunction *कुञ्चिका_Hash, UKeyComparator *कुञ्चिका_Comp,
                                UValueComparator *मानम्_Comp, int32_t आकार_2, UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    uhash_init_आकार(&hashObj, कुञ्चिका_Hash, कुञ्चिका_Comp, मानम्_Comp, आकार_2, &स्थितिः);
    यदि (U_SUCCESS(स्थितिः)) {
        hash = &hashObj;
        uhash_समुच्चयः_कुञ्चिका_Deleter(hash, uprv_deleteUObject);
    }
}

अन्तःस्थ Hashtable::Hashtable(UKeyComparator *कुञ्चिका_Comp, UValueComparator *मानम्_Comp,
                 UError_संहिता& स्थितिः) : hash(nullptr) {
    init( uhash_hashUnicodeString, कुञ्चिका_Comp, मानम्_Comp, स्थितिः);
}

अन्तःस्थ Hashtable::Hashtable(UBool ignore_कुञ्चिका_Case, UError_संहिता& स्थितिः)
 : hash(nullptr)
{
    init(ignore_कुञ्चिका_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_कुञ्चिका_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            स्थितिः);
}

अन्तःस्थ Hashtable::Hashtable(UBool ignore_कुञ्चिका_Case, int32_t आकार_2, UError_संहिता& स्थितिः)
 : hash(nullptr)
{
    init_आकार(ignore_कुञ्चिका_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_कुञ्चिका_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, आकार_2,
            स्थितिः);
}

अन्तःस्थ Hashtable::Hashtable(UError_संहिता& स्थितिः)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, स्थितिः);
}

अन्तःस्थ Hashtable::Hashtable()
 : hash(nullptr)
{
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, स्थितिः);
}

अन्तःस्थ Hashtable::~Hashtable() {
    यदि (hash != nullptr) {
        uhash_close(hash);
    }
}

अन्तःस्थ UObjectDeleter *Hashtable::समुच्चयः_मानम्_Deleter(UObjectDeleter *fn) {
    प्रत्यागम uhash_समुच्चयः_मानम्_Deleter(hash, fn);
}

अन्तःस्थ int32_t Hashtable::गणनम्() ध्रुव {
    प्रत्यागम uhash_गणनम्(hash);
}

अन्तःस्थ शून्य* Hashtable::put(ध्रुव UnicodeString& कुञ्चिका, शून्य* मानम्_2, UError_संहिता& स्थितिः) {
    प्रत्यागम uhash_put(hash, new UnicodeString(कुञ्चिका), मानम्_2, &स्थितिः);
}

अन्तःस्थ int32_t Hashtable::puti(ध्रुव UnicodeString& कुञ्चिका, int32_t मानम्_2, UError_संहिता& स्थितिः) {
    प्रत्यागम uhash_puti(hash, new UnicodeString(कुञ्चिका), मानम्_2, &स्थितिः);
}

अन्तःस्थ int32_t Hashtable::putiAllowZero(ध्रुव UnicodeString& कुञ्चिका, int32_t मानम्_2,
                                        UError_संहिता& स्थितिः) {
    प्रत्यागम uhash_putiAllowZero(hash, new UnicodeString(कुञ्चिका), मानम्_2, &स्थितिः);
}

अन्तःस्थ शून्य* Hashtable::get(ध्रुव UnicodeString& कुञ्चिका) ध्रुव {
    प्रत्यागम uhash_get(hash, &कुञ्चिका);
}

अन्तःस्थ int32_t Hashtable::geti(ध्रुव UnicodeString& कुञ्चिका) ध्रुव {
    प्रत्यागम uhash_geti(hash, &कुञ्चिका);
}

अन्तःस्थ int32_t Hashtable::geti_And_प्राप्तम्(ध्रुव UnicodeString& कुञ्चिका, UBool &प्राप्तम्) ध्रुव {
    प्रत्यागम uhash_geti_And_प्राप्तम्(hash, &कुञ्चिका, &प्राप्तम्);
}

अन्तःस्थ शून्य* Hashtable::remove(ध्रुव UnicodeString& कुञ्चिका) {
    प्रत्यागम uhash_remove(hash, &कुञ्चिका);
}

अन्तःस्थ int32_t Hashtable::removei(ध्रुव UnicodeString& कुञ्चिका) {
    प्रत्यागम uhash_removei(hash, &कुञ्चिका);
}

अन्तःस्थ UBool Hashtable::अन्तर्भवति_कुञ्चिका(ध्रुव UnicodeString& कुञ्चिका) ध्रुव {
    प्रत्यागम uhash_अन्तर्भवति_कुञ्चिका(hash, &कुञ्चिका);
}

अन्तःस्थ ध्रुव UHashElement* Hashtable::find(ध्रुव UnicodeString& कुञ्चिका) ध्रुव {
    प्रत्यागम uhash_find(hash, &कुञ्चिका);
}

अन्तःस्थ ध्रुव UHashElement* Hashtable::अग्र_Element(int32_t& pos) ध्रुव {
    प्रत्यागम uhash_अग्र_Element(hash, &pos);
}

अन्तःस्थ शून्य Hashtable::removeAll() {
    uhash_removeAll(hash);
}

अन्तःस्थ UKeyComparator* Hashtable::समुच्चयः_कुञ्चिका_Comparator(UKeyComparator*कुञ्चिका_Comp){
    प्रत्यागम uhash_समुच्चयः_कुञ्चिका_Comparator(hash, कुञ्चिका_Comp);
}

अन्तःस्थ UValueComparator* Hashtable::समुच्चयः_मानम्_Comparator(UValueComparator* मानम्_Comp){
    प्रत्यागम uhash_समुच्चयः_मानम्_Comparator(hash, मानम्_Comp);
}

अन्तःस्थ UBool Hashtable::equals(ध्रुव Hashtable& that)ध्रुव{
   प्रत्यागम uhash_equals(hash, that.hash);
}
U_NAMESPACE_अन्तः

#यदि_अन्तः


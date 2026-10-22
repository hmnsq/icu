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

#اگر_غیر_تعریف_شدہ HASH_H
#تعریف_کرو HASH_H

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
class U_COMMON_API Hashtable : عوامی UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    اندرونی خالی init(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp, UValueComparator *قدر_Comp, UError_رمز& حالت_2);

    اندرونی خالی init_سائز(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp, UValueComparator *قدر_Comp, int32_t سائز_2, UError_رمز& حالت_2);

عوامی:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    اندرونی Hashtable(UBool ignore_کلید_Case, UError_رمز& حالت_2);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    اندرونی Hashtable(UBool ignore_کلید_Case, int32_t سائز_2, UError_رمز& حالت_2);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    اندرونی Hashtable(UKeyComparator *کلید_Comp, UValueComparator *قدر_Comp, UError_رمز& حالت_2);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    اندرونی Hashtable(UError_رمز& حالت_2);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    اندرونی Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    اندرونی ~Hashtable();

    اندرونی UObjectDeleter *سیٹ_قدر_Deleter(UObjectDeleter *fn);

    اندرونی int32_t گنتی() ثابت;

    اندرونی خالی* put(ثابت UnicodeString& کلید, خالی* قدر_2, UError_رمز& حالت_2);

    اندرونی int32_t puti(ثابت UnicodeString& کلید, int32_t قدر_2, UError_رمز& حالت_2);

    اندرونی int32_t putiAllowZero(ثابت UnicodeString& کلید, int32_t قدر_2, UError_رمز& حالت_2);

    اندرونی خالی* get(ثابت UnicodeString& کلید) ثابت;

    اندرونی int32_t geti(ثابت UnicodeString& کلید) ثابت;

    اندرونی int32_t geti_And_ملا(ثابت UnicodeString& کلید, UBool &ملا) ثابت;

    اندرونی خالی* remove(ثابت UnicodeString& کلید);

    اندرونی int32_t removei(ثابت UnicodeString& کلید);

    اندرونی خالی removeAll();

    اندرونی UBool شامل_ہے_کلید(ثابت UnicodeString& کلید) ثابت;

    اندرونی ثابت UHashElement* ڈھونڈو(ثابت UnicodeString& کلید) ثابت;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    اندرونی ثابت UHashElement* اگلا_Element(int32_t& pos) ثابت;

    اندرونی UKeyComparator* سیٹ_کلید_Comparator(UKeyComparator*کلید_Comp);

    اندرونی UValueComparator* سیٹ_قدر_Comparator(UValueComparator* قدر_Comp);

    اندرونی UBool equals(ثابت Hashtable& that) ثابت;
نجی:
    Hashtable(ثابت Hashtable &other) = delete; // forbid copying of this class
    Hashtable &عامل=(ثابت Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

اندرونی خالی Hashtable::init(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp,
                            UValueComparator *قدر_Comp, UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    uhash_init(&hashObj, کلید_Hash, کلید_Comp, قدر_Comp, &حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        hash = &hashObj;
        uhash_سیٹ_کلید_Deleter(hash, uprv_deleteUObject);
    }
}

اندرونی خالی Hashtable::init_سائز(UHashFunction *کلید_Hash, UKeyComparator *کلید_Comp,
                                UValueComparator *قدر_Comp, int32_t سائز_2, UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    uhash_init_سائز(&hashObj, کلید_Hash, کلید_Comp, قدر_Comp, سائز_2, &حالت_2);
    اگر (U_SUCCESS(حالت_2)) {
        hash = &hashObj;
        uhash_سیٹ_کلید_Deleter(hash, uprv_deleteUObject);
    }
}

اندرونی Hashtable::Hashtable(UKeyComparator *کلید_Comp, UValueComparator *قدر_Comp,
                 UError_رمز& حالت_2) : hash(nullptr) {
    init( uhash_hashUnicodeString, کلید_Comp, قدر_Comp, حالت_2);
}

اندرونی Hashtable::Hashtable(UBool ignore_کلید_Case, UError_رمز& حالت_2)
 : hash(nullptr)
{
    init(ignore_کلید_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلید_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            حالت_2);
}

اندرونی Hashtable::Hashtable(UBool ignore_کلید_Case, int32_t سائز_2, UError_رمز& حالت_2)
 : hash(nullptr)
{
    init_سائز(ignore_کلید_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_کلید_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, سائز_2,
            حالت_2);
}

اندرونی Hashtable::Hashtable(UError_رمز& حالت_2)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, حالت_2);
}

اندرونی Hashtable::Hashtable()
 : hash(nullptr)
{
    UError_رمز حالت_2 = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, حالت_2);
}

اندرونی Hashtable::~Hashtable() {
    اگر (hash != nullptr) {
        uhash_close(hash);
    }
}

اندرونی UObjectDeleter *Hashtable::سیٹ_قدر_Deleter(UObjectDeleter *fn) {
    واپس uhash_سیٹ_قدر_Deleter(hash, fn);
}

اندرونی int32_t Hashtable::گنتی() ثابت {
    واپس uhash_گنتی(hash);
}

اندرونی خالی* Hashtable::put(ثابت UnicodeString& کلید, خالی* قدر_2, UError_رمز& حالت_2) {
    واپس uhash_put(hash, new UnicodeString(کلید), قدر_2, &حالت_2);
}

اندرونی int32_t Hashtable::puti(ثابت UnicodeString& کلید, int32_t قدر_2, UError_رمز& حالت_2) {
    واپس uhash_puti(hash, new UnicodeString(کلید), قدر_2, &حالت_2);
}

اندرونی int32_t Hashtable::putiAllowZero(ثابت UnicodeString& کلید, int32_t قدر_2,
                                        UError_رمز& حالت_2) {
    واپس uhash_putiAllowZero(hash, new UnicodeString(کلید), قدر_2, &حالت_2);
}

اندرونی خالی* Hashtable::get(ثابت UnicodeString& کلید) ثابت {
    واپس uhash_get(hash, &کلید);
}

اندرونی int32_t Hashtable::geti(ثابت UnicodeString& کلید) ثابت {
    واپس uhash_geti(hash, &کلید);
}

اندرونی int32_t Hashtable::geti_And_ملا(ثابت UnicodeString& کلید, UBool &ملا) ثابت {
    واپس uhash_geti_And_ملا(hash, &کلید, &ملا);
}

اندرونی خالی* Hashtable::remove(ثابت UnicodeString& کلید) {
    واپس uhash_remove(hash, &کلید);
}

اندرونی int32_t Hashtable::removei(ثابت UnicodeString& کلید) {
    واپس uhash_removei(hash, &کلید);
}

اندرونی UBool Hashtable::شامل_ہے_کلید(ثابت UnicodeString& کلید) ثابت {
    واپس uhash_شامل_ہے_کلید(hash, &کلید);
}

اندرونی ثابت UHashElement* Hashtable::ڈھونڈو(ثابت UnicodeString& کلید) ثابت {
    واپس uhash_ڈھونڈو(hash, &کلید);
}

اندرونی ثابت UHashElement* Hashtable::اگلا_Element(int32_t& pos) ثابت {
    واپس uhash_اگلا_Element(hash, &pos);
}

اندرونی خالی Hashtable::removeAll() {
    uhash_removeAll(hash);
}

اندرونی UKeyComparator* Hashtable::سیٹ_کلید_Comparator(UKeyComparator*کلید_Comp){
    واپس uhash_سیٹ_کلید_Comparator(hash, کلید_Comp);
}

اندرونی UValueComparator* Hashtable::سیٹ_قدر_Comparator(UValueComparator* قدر_Comp){
    واپس uhash_سیٹ_قدر_Comparator(hash, قدر_Comp);
}

اندرونی UBool Hashtable::equals(ثابت Hashtable& that)ثابت{
   واپس uhash_equals(hash, that.hash);
}
U_NAMESPACE_انتہا

#اگر_ختم


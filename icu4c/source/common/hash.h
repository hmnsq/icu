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

#నిర్వచితం_కాకపోతే HASH_H
#నిర్వచించు HASH_H

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uhash.h"

U_NAMESPACE_BEGIN

/**
 * Hashtable is a thin C++ wrapper around UHashtable, a general-purpose void*
 * hashtable implemented in C.  Hashtable is designed to be idiomatic and
 * easy-to-use in C++.
 *
 * Hashtable is an INTERNAL CLASS.
 */
class U_COMMON_API Hashtable : బహిరంగ UMemory {
    UHashtable* hash;
    UHashtable hashObj;

    అంతఃవరుస శూన్య init(UHashFunction *కీలకం_Hash, UKeyComparator *కీలకం_Comp, UValueComparator *విలువ_Comp, UError_సంకేతం& స్థితి_2);

    అంతఃవరుస శూన్య init_కొలత(UHashFunction *కీలకం_Hash, UKeyComparator *కీలకం_Comp, UValueComparator *విలువ_Comp, int32_t కొలత_2, UError_సంకేతం& స్థితి_2);

బహిరంగ:
    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param status Error code
    */
    అంతఃవరుస Hashtable(UBool ignore_కీలకం_Case, UError_సంకేతం& స్థితి_2);

    /**
     * Construct a hashtable
     * @param ignoreKeyCase If true, keys are case insensitive.
     * @param size initial size allocation
     * @param status Error code
    */
    అంతఃవరుస Hashtable(UBool ignore_కీలకం_Case, int32_t కొలత_2, UError_సంకేతం& స్థితి_2);

    /**
     * Construct a hashtable
     * @param keyComp Comparator for comparing the keys
     * @param valueComp Comparator for comparing the values
     * @param status Error code
    */
    అంతఃవరుస Hashtable(UKeyComparator *కీలకం_Comp, UValueComparator *విలువ_Comp, UError_సంకేతం& స్థితి_2);

    /**
     * Construct a hashtable
     * @param status Error code
    */
    అంతఃవరుస Hashtable(UError_సంకేతం& స్థితి_2);

    /**
     * Construct a hashtable, _disregarding any error_.  Use this constructor
     * with caution.
     */
    అంతఃవరుస Hashtable();

    /**
     * Non-virtual destructor; make this virtual if Hashtable is subclassed
     * in the future.
     */
    అంతఃవరుస ~Hashtable();

    అంతఃవరుస UObjectDeleter *సమితి_విలువ_Deleter(UObjectDeleter *fn);

    అంతఃవరుస int32_t లెక్క() స్థిరరాశి;

    అంతఃవరుస శూన్య* put(స్థిరరాశి UnicodeString& కీలకం, శూన్య* విలువ_2, UError_సంకేతం& స్థితి_2);

    అంతఃవరుస int32_t puti(స్థిరరాశి UnicodeString& కీలకం, int32_t విలువ_2, UError_సంకేతం& స్థితి_2);

    అంతఃవరుస int32_t putiAllowZero(స్థిరరాశి UnicodeString& కీలకం, int32_t విలువ_2, UError_సంకేతం& స్థితి_2);

    అంతఃవరుస శూన్య* get(స్థిరరాశి UnicodeString& కీలకం) స్థిరరాశి;

    అంతఃవరుస int32_t geti(స్థిరరాశి UnicodeString& కీలకం) స్థిరరాశి;

    అంతఃవరుస int32_t geti_And_దొరికింది(స్థిరరాశి UnicodeString& కీలకం, UBool &దొరికింది) స్థిరరాశి;

    అంతఃవరుస శూన్య* remove(స్థిరరాశి UnicodeString& కీలకం);

    అంతఃవరుస int32_t removei(స్థిరరాశి UnicodeString& కీలకం);

    అంతఃవరుస శూన్య removeAll();

    అంతఃవరుస UBool కలిగియుంది_కీలకం(స్థిరరాశి UnicodeString& కీలకం) స్థిరరాశి;

    అంతఃవరుస స్థిరరాశి UHashElement* కనుగొను(స్థిరరాశి UnicodeString& కీలకం) స్థిరరాశి;

    /**
     * @param pos - must be UHASH_FIRST on first call, and untouched afterwards.
     * @see uhash_nextElement
     */
    అంతఃవరుస స్థిరరాశి UHashElement* తదుపరి_Element(int32_t& pos) స్థిరరాశి;

    అంతఃవరుస UKeyComparator* సమితి_కీలకం_Comparator(UKeyComparator*కీలకం_Comp);

    అంతఃవరుస UValueComparator* సమితి_విలువ_Comparator(UValueComparator* విలువ_Comp);

    అంతఃవరుస UBool equals(స్థిరరాశి Hashtable& that) స్థిరరాశి;
అంతరంగ:
    Hashtable(స్థిరరాశి Hashtable &other) = delete; // forbid copying of this class
    Hashtable &క్రియాకారకం=(స్థిరరాశి Hashtable &other) = delete; // forbid copying of this class
};

/*********************************************************************
 * Implementation
 ********************************************************************/

అంతఃవరుస శూన్య Hashtable::init(UHashFunction *కీలకం_Hash, UKeyComparator *కీలకం_Comp,
                            UValueComparator *విలువ_Comp, UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    uhash_init(&hashObj, కీలకం_Hash, కీలకం_Comp, విలువ_Comp, &స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        hash = &hashObj;
        uhash_సమితి_కీలకం_Deleter(hash, uprv_deleteUObject);
    }
}

అంతఃవరుస శూన్య Hashtable::init_కొలత(UHashFunction *కీలకం_Hash, UKeyComparator *కీలకం_Comp,
                                UValueComparator *విలువ_Comp, int32_t కొలత_2, UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    uhash_init_కొలత(&hashObj, కీలకం_Hash, కీలకం_Comp, విలువ_Comp, కొలత_2, &స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        hash = &hashObj;
        uhash_సమితి_కీలకం_Deleter(hash, uprv_deleteUObject);
    }
}

అంతఃవరుస Hashtable::Hashtable(UKeyComparator *కీలకం_Comp, UValueComparator *విలువ_Comp,
                 UError_సంకేతం& స్థితి_2) : hash(nullptr) {
    init( uhash_hashUnicodeString, కీలకం_Comp, విలువ_Comp, స్థితి_2);
}

అంతఃవరుస Hashtable::Hashtable(UBool ignore_కీలకం_Case, UError_సంకేతం& స్థితి_2)
 : hash(nullptr)
{
    init(ignore_కీలకం_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_కీలకం_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr,
            స్థితి_2);
}

అంతఃవరుస Hashtable::Hashtable(UBool ignore_కీలకం_Case, int32_t కొలత_2, UError_సంకేతం& స్థితి_2)
 : hash(nullptr)
{
    init_కొలత(ignore_కీలకం_Case ? uhash_hashCaselessUnicodeString
                        : uhash_hashUnicodeString,
            ignore_కీలకం_Case ? uhash_compareCaselessUnicodeString
                        : uhash_compareUnicodeString,
            nullptr, కొలత_2,
            స్థితి_2);
}

అంతఃవరుస Hashtable::Hashtable(UError_సంకేతం& స్థితి_2)
 : hash(nullptr)
{
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, స్థితి_2);
}

అంతఃవరుస Hashtable::Hashtable()
 : hash(nullptr)
{
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    init(uhash_hashUnicodeString, uhash_compareUnicodeString, nullptr, స్థితి_2);
}

అంతఃవరుస Hashtable::~Hashtable() {
    ఒకవేళ (hash != nullptr) {
        uhash_close(hash);
    }
}

అంతఃవరుస UObjectDeleter *Hashtable::సమితి_విలువ_Deleter(UObjectDeleter *fn) {
    తిరిగి uhash_సమితి_విలువ_Deleter(hash, fn);
}

అంతఃవరుస int32_t Hashtable::లెక్క() స్థిరరాశి {
    తిరిగి uhash_లెక్క(hash);
}

అంతఃవరుస శూన్య* Hashtable::put(స్థిరరాశి UnicodeString& కీలకం, శూన్య* విలువ_2, UError_సంకేతం& స్థితి_2) {
    తిరిగి uhash_put(hash, new UnicodeString(కీలకం), విలువ_2, &స్థితి_2);
}

అంతఃవరుస int32_t Hashtable::puti(స్థిరరాశి UnicodeString& కీలకం, int32_t విలువ_2, UError_సంకేతం& స్థితి_2) {
    తిరిగి uhash_puti(hash, new UnicodeString(కీలకం), విలువ_2, &స్థితి_2);
}

అంతఃవరుస int32_t Hashtable::putiAllowZero(స్థిరరాశి UnicodeString& కీలకం, int32_t విలువ_2,
                                        UError_సంకేతం& స్థితి_2) {
    తిరిగి uhash_putiAllowZero(hash, new UnicodeString(కీలకం), విలువ_2, &స్థితి_2);
}

అంతఃవరుస శూన్య* Hashtable::get(స్థిరరాశి UnicodeString& కీలకం) స్థిరరాశి {
    తిరిగి uhash_get(hash, &కీలకం);
}

అంతఃవరుస int32_t Hashtable::geti(స్థిరరాశి UnicodeString& కీలకం) స్థిరరాశి {
    తిరిగి uhash_geti(hash, &కీలకం);
}

అంతఃవరుస int32_t Hashtable::geti_And_దొరికింది(స్థిరరాశి UnicodeString& కీలకం, UBool &దొరికింది) స్థిరరాశి {
    తిరిగి uhash_geti_And_దొరికింది(hash, &కీలకం, &దొరికింది);
}

అంతఃవరుస శూన్య* Hashtable::remove(స్థిరరాశి UnicodeString& కీలకం) {
    తిరిగి uhash_remove(hash, &కీలకం);
}

అంతఃవరుస int32_t Hashtable::removei(స్థిరరాశి UnicodeString& కీలకం) {
    తిరిగి uhash_removei(hash, &కీలకం);
}

అంతఃవరుస UBool Hashtable::కలిగియుంది_కీలకం(స్థిరరాశి UnicodeString& కీలకం) స్థిరరాశి {
    తిరిగి uhash_కలిగియుంది_కీలకం(hash, &కీలకం);
}

అంతఃవరుస స్థిరరాశి UHashElement* Hashtable::కనుగొను(స్థిరరాశి UnicodeString& కీలకం) స్థిరరాశి {
    తిరిగి uhash_కనుగొను(hash, &కీలకం);
}

అంతఃవరుస స్థిరరాశి UHashElement* Hashtable::తదుపరి_Element(int32_t& pos) స్థిరరాశి {
    తిరిగి uhash_తదుపరి_Element(hash, &pos);
}

అంతఃవరుస శూన్య Hashtable::removeAll() {
    uhash_removeAll(hash);
}

అంతఃవరుస UKeyComparator* Hashtable::సమితి_కీలకం_Comparator(UKeyComparator*కీలకం_Comp){
    తిరిగి uhash_సమితి_కీలకం_Comparator(hash, కీలకం_Comp);
}

అంతఃవరుస UValueComparator* Hashtable::సమితి_విలువ_Comparator(UValueComparator* విలువ_Comp){
    తిరిగి uhash_సమితి_విలువ_Comparator(hash, విలువ_Comp);
}

అంతఃవరుస UBool Hashtable::equals(స్థిరరాశి Hashtable& that)స్థిరరాశి{
   తిరిగి uhash_equals(hash, that.hash);
}
U_NAMESPACE_చివర

#అయితే_ముగింపు


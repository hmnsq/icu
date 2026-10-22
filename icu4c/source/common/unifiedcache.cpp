// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*
* File unifiedcache.cpp
******************************************************************************
*/

#अन्तर्भाव "unifiedcache.h"

#अन्तर्भाव <algorithm>      // For std::max()
#अन्तर्भाव <mutex>

#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "ucln_cmn.h"

स्थिर icu::UnifiedCache *gCache = शून्यसूचकः;
स्थिर std::mutex *gCacheMutex = शून्यसूचकः;
स्थिर std::condition_variable *g_अन्तर्_प्रगतिः_मानम्_Added_Cond;
स्थिर icu::UInitOnce gCacheInitOnce {};

स्थिर ध्रुव int32_t MAX_EVICT_ITERATIONS = 10;
स्थिर ध्रुव int32_t DEFAULT_MAX_अप्रयुक्तम् = 1000;
स्थिर ध्रुव int32_t DEFAULT_PERCENTAGE_सम्बन्धि_अन्तर्_USE = 100;


U_CDECL_BEGIN
स्थिर UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    विलोपय gCache;
    gCache = शून्यसूचकः;
    gCacheMutex->~mutex();
    gCacheMutex = शून्यसूचकः;
    g_अन्तर्_प्रगतिः_मानम्_Added_Cond->~condition_variable();
    g_अन्तर्_प्रगतिः_मानम्_Added_Cond = शून्यसूचकः;
    प्रत्यागम सत्य;
}
U_CDECL_अन्तः


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_कुञ्चिकाः(ध्रुव UHashTok कुञ्चिका) {
    ध्रुव Cache_कुञ्चिका_आधारः* ckey = static_cast<ध्रुव Cache_कुञ्चिका_आधारः*>(कुञ्चिका.pointer);
    प्रत्यागम ckey->hash_संहिता();
}

UBool U_EXPORT2
ucache_compare_कुञ्चिकाः(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    ध्रुव Cache_कुञ्चिका_आधारः* p1 = static_cast<ध्रुव Cache_कुञ्चिका_आधारः*>(key1.pointer);
    ध्रुव Cache_कुञ्चिका_आधारः* p2 = static_cast<ध्रुव Cache_कुञ्चिका_आधारः*>(key2.pointer);
    प्रत्यागम *p1 == *p2;
}

शून्य U_EXPORT2
ucache_delete_कुञ्चिका(शून्य *obj) {
    Cache_कुञ्चिका_आधारः* p = static_cast<Cache_कुञ्चिका_आधारः*>(obj);
    विलोपय p;
}

Cache_कुञ्चिका_आधारः::~Cache_कुञ्चिका_आधारः() {
}

स्थिर शून्य U_CALLCONV cacheInit(UError_संहिता &स्थितिः) {
    U_ASSERT(gCache == शून्यसूचकः);
    ucln_common_पञ्जिकाकोशः_Cleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_अन्तर्_प्रगतिः_मानम्_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = नूतन UnifiedCache(स्थितिः);
    यदि (gCache == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
    }
    यदि (U_FAILURE(स्थितिः)) {
        विलोपय gCache;
        gCache = शून्यसूचकः;
        प्रत्यागम;
    }
}

UnifiedCache *UnifiedCache::getInstance(UError_संहिता &स्थितिः) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    U_ASSERT(gCache != शून्यसूचकः);
    प्रत्यागम gCache;
}

UnifiedCache::UnifiedCache(UError_संहिता &स्थितिः) :
        fHashtable(शून्यसूचकः),
        fEvictPos(UHASH_FIRST),
        f_अङ्कः_मानानि_योग(0),
        f_अङ्कः_मानानि_अन्तर्_Use(0),
        f_Max_अप्रयुक्तम्(DEFAULT_MAX_अप्रयुक्तम्),
        f_Max_Percentage_सम्बन्धि_अन्तर्_Use(DEFAULT_PERCENTAGE_सम्बन्धि_अन्तर्_USE),
        f_Auto_Evicted_गणनम्(0),
        f_No_मानम्(शून्यसूचकः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    f_No_मानम् = नूतन SharedObject();
    यदि (f_No_मानम् == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    f_No_मानम्->soft_Ref_गणनम् = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_मानम्->hard_Ref_गणनम् = 1;  // when other references to it are removed.
    f_No_मानम्->cachePtr = एतत्;

    fHashtable = uhash_open(
            &ucache_hash_कुञ्चिकाः,
            &ucache_compare_कुञ्चिकाः,
            शून्यसूचकः,
            &स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    uhash_समुच्चयः_कुञ्चिका_Deleter(fHashtable, &ucache_delete_कुञ्चिका);
}

शून्य UnifiedCache::समुच्चयः_Eviction_Policy(
        int32_t count, int32_t percentage_सम्बन्धि_अन्तर्_Use_वस्तूनि, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    यदि (count < 0 || percentage_सम्बन्धि_अन्तर्_Use_वस्तूनि < 0) {
        स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    std::lock_guard<std::mutex> तालकम्(*gCacheMutex);
    f_Max_अप्रयुक्तम् = count;
    f_Max_Percentage_सम्बन्धि_अन्तर्_Use = percentage_सम्बन्धि_अन्तर्_Use_वस्तूनि;
}

int32_t UnifiedCache::अप्रयुक्तम्_गणनम्() ध्रुव {
    std::lock_guard<std::mutex> तालकम्(*gCacheMutex);
    प्रत्यागम uhash_गणनम्(fHashtable) - f_अङ्कः_मानानि_अन्तर्_Use;
}

int64_t UnifiedCache::auto_Evicted_गणनम्() ध्रुव {
    std::lock_guard<std::mutex> तालकम्(*gCacheMutex);
    प्रत्यागम f_Auto_Evicted_गणनम्;
}

int32_t UnifiedCache::कुञ्चिका_गणनम्() ध्रुव {
    std::lock_guard<std::mutex> तालकम्(*gCacheMutex);
    प्रत्यागम uhash_गणनम्(fHashtable);
}

शून्य UnifiedCache::flush() ध्रुव {
    std::lock_guard<std::mutex> तालकम्(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    यावत् (_flush(असत्य));
}

शून्य UnifiedCache::handleUnreferencedObject() ध्रुव {
    std::lock_guard<std::mutex> तालकम्(*gCacheMutex);
    --f_अङ्कः_मानानि_अन्तर्_Use;
    सततम्_Eviction_खण्डिका();
}

#यदि_परिभाषितम् UNIFIED_CACHE_DEBUG
#अन्तर्भाव <stdio.h>

शून्य UnifiedCache::dump() {
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    ध्रुव UnifiedCache *cache = getInstance(स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        प्रत्यागम;
    }
    cache->dumpContents();
}

शून्य UnifiedCache::dumpContents() ध्रुव {
    std::lock_guard<std::mutex> तालकम्(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
शून्य UnifiedCache::_dumpContents() ध्रुव {
    int32_t pos = UHASH_FIRST;
    ध्रुव UHashElement *element = uhash_अग्र_Element(fHashtable, &pos);
    वर्ण बफर[256];
    int32_t cnt = 0;
    प्रति (; element != शून्यसूचकः; element = uhash_अग्र_Element(fHashtable, &pos)) {
        ध्रुव SharedObject *sharedObject =
                (ध्रुव SharedObject *) element->value.pointer;
        ध्रुव Cache_कुञ्चिका_आधारः *कुञ्चिका =
                (ध्रुव Cache_कुञ्चिका_आधारः *) element->कुञ्चिका.pointer;
        यदि (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    कुञ्चिका->writeDescription(बफर, 256),
                    कुञ्चिका->creation_स्थितिः,
                    sharedObject == f_No_मानम् ? शून्यसूचकः :sharedObject,
                    sharedObject->get_Ref_गणनम्(),
                    sharedObject->get_Soft_Ref_गणनम्());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_गणनम्(fHashtable));
}
#यदि_अन्तः

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> तालकम्(*gCacheMutex);
        _flush(सत्य);
    }
    uhash_close(fHashtable);
    fHashtable = शून्यसूचकः;
    विलोपय f_No_मानम्;
    f_No_मानम् = शून्यसूचकः;
}

ध्रुव UHashElement *
UnifiedCache::अग्र_Element_2() ध्रुव {
    ध्रुव UHashElement *element = uhash_अग्र_Element(fHashtable, &fEvictPos);
    यदि (element == शून्यसूचकः) {
        fEvictPos = UHASH_FIRST;
        प्रत्यागम uhash_अग्र_Element(fHashtable, &fEvictPos);
    }
    प्रत्यागम element;
}

UBool UnifiedCache::_flush(UBool all) ध्रुव {
    UBool परिणाम = असत्य;
    int32_t orig_आकार = uhash_गणनम्(fHashtable);
    प्रति (int32_t i = 0; i < orig_आकार; ++i) {
        ध्रुव UHashElement *element = अग्र_Element_2();
        यदि (element == शून्यसूचकः) {
            भङ्ग;
        }
        यदि (all || _isEvictable(element)) {
            ध्रुव SharedObject *sharedObject =
                    static_cast<ध्रुव SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == एतत्);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            परिणाम = सत्य;
        }
    }
    प्रत्यागम परिणाम;
}

int32_t UnifiedCache::compute_गणनम्_सम्बन्धि_वस्तूनि_गन्तव्यम्_Evict() ध्रुव {
    int32_t योग_वस्तूनि = uhash_गणनम्(fHashtable);
    int32_t evictable_वस्तूनि = योग_वस्तूनि - f_अङ्कः_मानानि_अन्तर्_Use;

    int32_t अप्रयुक्तम्_सीमा_By_Percentage = f_अङ्कः_मानानि_अन्तर्_Use * f_Max_Percentage_सम्बन्धि_अन्तर्_Use / 100;
    int32_t अप्रयुक्तम्_सीमा = std::max(अप्रयुक्तम्_सीमा_By_Percentage, f_Max_अप्रयुक्तम्);
    int32_t गणनम्_सम्बन्धि_वस्तूनि_गन्तव्यम्_Evict = std::max<int32_t>(0, evictable_वस्तूनि - अप्रयुक्तम्_सीमा);
    प्रत्यागम गणनम्_सम्बन्धि_वस्तूनि_गन्तव्यम्_Evict;
}

शून्य UnifiedCache::सततम्_Eviction_खण्डिका() ध्रुव {
    int32_t max_वस्तूनि_गन्तव्यम्_Evict = compute_गणनम्_सम्बन्धि_वस्तूनि_गन्तव्यम्_Evict();
    यदि (max_वस्तूनि_गन्तव्यम्_Evict <= 0) {
        प्रत्यागम;
    }
    प्रति (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ध्रुव UHashElement *element = अग्र_Element_2();
        यदि (element == शून्यसूचकः) {
            भङ्ग;
        }
        यदि (_isEvictable(element)) {
            ध्रुव SharedObject *sharedObject =
                    static_cast<ध्रुव SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_गणनम्;
            यदि (--max_वस्तूनि_गन्तव्यम्_Evict == 0) {
                भङ्ग;
            }
        }
    }
}

शून्य UnifiedCache::_putNew(
        ध्रुव Cache_कुञ्चिका_आधारः &कुञ्चिका,
        ध्रुव SharedObject *value,
        ध्रुव UError_संहिता creation_स्थितिः,
        UError_संहिता &स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    Cache_कुञ्चिका_आधारः *कुञ्चिका_गन्तव्यम्_Adopt = कुञ्चिका.clone();
    यदि (कुञ्चिका_गन्तव्यम्_Adopt == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    कुञ्चिका_गन्तव्यम्_Adopt->f_Creation_स्थितिः = creation_स्थितिः;
    यदि (value->soft_Ref_गणनम् == 0) {
        पञ्जिकाकोशः_Primary(कुञ्चिका_गन्तव्यम्_Adopt, value);
    }
    शून्य *old_मानम् = uhash_put(fHashtable, कुञ्चिका_गन्तव्यम्_Adopt, (शून्य *) value, &स्थितिः);
    U_ASSERT(old_मानम् == शून्यसूचकः);
    (शून्य)old_मानम्;
    यदि (U_SUCCESS(स्थितिः)) {
        value->soft_Ref_गणनम्++;
    }
}

शून्य UnifiedCache::_putIfAbsentAndGet(
        ध्रुव Cache_कुञ्चिका_आधारः &कुञ्चिका,
        ध्रुव SharedObject *&value,
        UError_संहिता &स्थितिः) ध्रुव {
    std::lock_guard<std::mutex> तालकम्(*gCacheMutex);
    ध्रुव UHashElement *element = uhash_find(fHashtable, &कुञ्चिका);
    यदि (element != शून्यसूचकः && !अन्तर्_प्रगतिः(element)) {
        _fetch(element, value, स्थितिः);
        प्रत्यागम;
    }
    यदि (element == शून्यसूचकः) {
        UError_संहिता putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(कुञ्चिका, value, स्थितिः, putError);
    } अन्यथा {
        _put(element, value, स्थितिः);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    सततम्_Eviction_खण्डिका();
}


UBool UnifiedCache::_poll(
        ध्रुव Cache_कुञ्चिका_आधारः &कुञ्चिका,
        ध्रुव SharedObject *&value,
        UError_संहिता &स्थितिः) ध्रुव {
    U_ASSERT(value == शून्यसूचकः);
    U_ASSERT(स्थितिः == U_ZERO_ERROR);
    std::unique_lock<std::mutex> तालकम्(*gCacheMutex);
    ध्रुव UHashElement *element = uhash_find(fHashtable, &कुञ्चिका);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     यावत् (element != शून्यसूचकः && अन्तर्_प्रगतिः(element)) {
         g_अन्तर्_प्रगतिः_मानम्_Added_Cond->प्रतीक्षस्व(तालकम्);
         element = uhash_find(fHashtable, &कुञ्चिका);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    यदि (element != शून्यसूचकः) {
         _fetch(element, value, स्थितिः);
        प्रत्यागम सत्य;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(कुञ्चिका, f_No_मानम्, U_ZERO_ERROR, स्थितिः);
    प्रत्यागम असत्य;
}

शून्य UnifiedCache::_get(
        ध्रुव Cache_कुञ्चिका_आधारः &कुञ्चिका,
        ध्रुव SharedObject *&value,
        ध्रुव शून्य *creationContext,
        UError_संहिता &स्थितिः) ध्रुव {
    U_ASSERT(value == शून्यसूचकः);
    U_ASSERT(स्थितिः == U_ZERO_ERROR);
    यदि (_poll(कुञ्चिका, value, स्थितिः)) {
        यदि (value == f_No_मानम्) {
            SharedObject::clearPtr(value);
        }
        प्रत्यागम;
    }
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    value = कुञ्चिका.createObject(creationContext, स्थितिः);
    U_ASSERT(value == शून्यसूचकः || value->hasHardReferences());
    U_ASSERT(value != शून्यसूचकः || स्थितिः != U_ZERO_ERROR);
    यदि (value == शून्यसूचकः) {
        SharedObject::copyPtr(f_No_मानम्, value);
    }
    _putIfAbsentAndGet(कुञ्चिका, value, स्थितिः);
    यदि (value == f_No_मानम्) {
        SharedObject::clearPtr(value);
    }
}

शून्य UnifiedCache::पञ्जिकाकोशः_Primary(
            ध्रुव Cache_कुञ्चिका_आधारः *the_कुञ्चिका, ध्रुव SharedObject *value) ध्रुव {
    the_कुञ्चिका->fIsPrimary = सत्य;
    value->cachePtr = एतत्;
    ++f_अङ्कः_मानानि_योग;
    ++f_अङ्कः_मानानि_अन्तर्_Use;
}

शून्य UnifiedCache::_put(
        ध्रुव UHashElement *element,
        ध्रुव SharedObject *value,
        ध्रुव UError_संहिता स्थितिः) ध्रुव {
    U_ASSERT(अन्तर्_प्रगतिः(element));
    ध्रुव Cache_कुञ्चिका_आधारः* the_कुञ्चिका = static_cast<ध्रुव Cache_कुञ्चिका_आधारः*>(element->कुञ्चिका.pointer);
    ध्रुव SharedObject* old_मानम् = static_cast<ध्रुव SharedObject*>(element->value.pointer);
    the_कुञ्चिका->f_Creation_स्थितिः = स्थितिः;
    यदि (value->soft_Ref_गणनम् == 0) {
        पञ्जिकाकोशः_Primary(the_कुञ्चिका, value);
    }
    value->soft_Ref_गणनम्++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (शून्य *) value;
    U_ASSERT(old_मानम् == f_No_मानम्);
    removeSoftRef(old_मानम्);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_अन्तर्_प्रगतिः_मानम्_Added_Cond->notify_all();
}

शून्य UnifiedCache::_fetch(
        ध्रुव UHashElement *element,
        ध्रुव SharedObject *&value,
        UError_संहिता &स्थितिः) ध्रुव {
    ध्रुव Cache_कुञ्चिका_आधारः* the_कुञ्चिका = static_cast<ध्रुव Cache_कुञ्चिका_आधारः*>(element->कुञ्चिका.pointer);
    स्थितिः = the_कुञ्चिका->f_Creation_स्थितिः;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ध्रुव SharedObject *>(element->value.pointer);
    सङ्कलय_Hard_Ref(value);
}


UBool UnifiedCache::अन्तर्_प्रगतिः(ध्रुव UHashElement* element) ध्रुव {
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    ध्रुव SharedObject * value = शून्यसूचकः;
    _fetch(element, value, स्थितिः);
    UBool परिणाम = अन्तर्_प्रगतिः(value, स्थितिः);
    removeHardRef(value);
    प्रत्यागम परिणाम;
}

UBool UnifiedCache::अन्तर्_प्रगतिः(
        ध्रुव SharedObject* the_मानम्, UError_संहिता creation_स्थितिः) ध्रुव {
    प्रत्यागम (the_मानम् == f_No_मानम् && creation_स्थितिः == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ध्रुव UHashElement *element) ध्रुव
{
    ध्रुव Cache_कुञ्चिका_आधारः* the_कुञ्चिका = static_cast<ध्रुव Cache_कुञ्चिका_आधारः*>(element->कुञ्चिका.pointer);
    ध्रुव SharedObject* the_मानम् = static_cast<ध्रुव SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    यदि (अन्तर्_प्रगतिः(the_मानम्, the_कुञ्चिका->f_Creation_स्थितिः)) {
        प्रत्यागम असत्य;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    प्रत्यागम (!the_कुञ्चिका->fIsPrimary || (the_मानम्->soft_Ref_गणनम् == 1 && the_मानम्->noHardReferences()));
}

शून्य UnifiedCache::removeSoftRef(ध्रुव SharedObject *value) ध्रुव {
    U_ASSERT(value->cachePtr == एतत्);
    U_ASSERT(value->soft_Ref_गणनम् > 0);
    यदि (--value->soft_Ref_गणनम् == 0) {
        --f_अङ्कः_मानानि_योग;
        यदि (value->noHardReferences()) {
            विलोपय value;
        } अन्यथा {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = शून्यसूचकः;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ध्रुव SharedObject *value) ध्रुव {
    पूर्णाङ्क ref_गणनम् = 0;
    यदि (value) {
        ref_गणनम् = umtx_atomic_dec(&value->hard_Ref_गणनम्);
        U_ASSERT(ref_गणनम् >= 0);
        यदि (ref_गणनम् == 0) {
            --f_अङ्कः_मानानि_अन्तर्_Use;
        }
    }
    प्रत्यागम ref_गणनम्;
}

int32_t UnifiedCache::सङ्कलय_Hard_Ref(ध्रुव SharedObject *value) ध्रुव {
    पूर्णाङ्क ref_गणनम् = 0;
    यदि (value) {
        ref_गणनम् = umtx_atomic_inc(&value->hard_Ref_गणनम्);
        U_ASSERT(ref_गणनम् >= 1);
        यदि (ref_गणनम् == 1) {
            f_अङ्कः_मानानि_अन्तर्_Use++;
        }
    }
    प्रत्यागम ref_गणनम्;
}

U_NAMESPACE_अन्तः

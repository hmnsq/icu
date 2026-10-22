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

#ಅನ್ತರ್ಭಾವ "unifiedcache.h"

#ಅನ್ತರ್ಭಾವ <algorithm>      // For std::max()
#ಅನ್ತರ್ಭಾವ <mutex>

#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"

ಸ್ಥಿರ icu::UnifiedCache *gCache = nullptr;
ಸ್ಥಿರ std::mutex *gCacheMutex = nullptr;
ಸ್ಥಿರ std::condition_variable *g_In_Progress_ಮೌಲ್ಯ_Added_Cond;
ಸ್ಥಿರ icu::UInitOnce gCacheInitOnce {};

ಸ್ಥಿರ ಧ್ರುವ int32_t MAX_EVICT_ITERATIONS = 10;
ಸ್ಥಿರ ಧ್ರುವ int32_t DEFAULT_MAX_UNUSED = 1000;
ಸ್ಥಿರ ಧ್ರುವ int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_ಮೌಲ್ಯ_Added_Cond->~condition_variable();
    g_In_Progress_ಮೌಲ್ಯ_Added_Cond = nullptr;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_ಕೀಲಿಗಳು(ಧ್ರುವ UHashTok ಕೀಲಿ) {
    ಧ್ರುವ Cache_ಕೀಲಿ_Base* ckey = static_cast<ಧ್ರುವ Cache_ಕೀಲಿ_Base*>(ಕೀಲಿ.pointer);
    ಪ್ರತ್ಯಾಗಮ ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_ಕೀಲಿಗಳು(ಧ್ರುವ UHashTok key1, ಧ್ರುವ UHashTok key2) {
    ಧ್ರುವ Cache_ಕೀಲಿ_Base* p1 = static_cast<ಧ್ರುವ Cache_ಕೀಲಿ_Base*>(key1.pointer);
    ಧ್ರುವ Cache_ಕೀಲಿ_Base* p2 = static_cast<ಧ್ರುವ Cache_ಕೀಲಿ_Base*>(key2.pointer);
    ಪ್ರತ್ಯಾಗಮ *p1 == *p2;
}

ಶೂನ್ಯ U_EXPORT2
ucache_delete_ಕೀಲಿ(ಶೂನ್ಯ *obj) {
    Cache_ಕೀಲಿ_Base* p = static_cast<Cache_ಕೀಲಿ_Base*>(obj);
    delete p;
}

Cache_ಕೀಲಿ_Base::~Cache_ಕೀಲಿ_Base() {
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_ಮೌಲ್ಯ_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = ನೂತನ UnifiedCache(status);
    ಯದಿ (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ಯದಿ (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        ಪ್ರತ್ಯಾಗಮ;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    U_ASSERT(gCache != nullptr);
    ಪ್ರತ್ಯಾಗಮ gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_Total(0),
        f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_ಎಣಿಕೆ(0),
        f_No_ಮೌಲ್ಯ(nullptr) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    f_No_ಮೌಲ್ಯ = ನೂತನ SharedObject();
    ಯದಿ (f_No_ಮೌಲ್ಯ == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    f_No_ಮೌಲ್ಯ->soft_Ref_ಎಣಿಕೆ = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_ಮೌಲ್ಯ->hard_Ref_ಎಣಿಕೆ = 1;  // when other references to it are removed.
    f_No_ಮೌಲ್ಯ->cachePtr = ಏತತ್;

    fHashtable = uhash_open(
            &ucache_hash_ಕೀಲಿಗಳು,
            &ucache_compare_ಕೀಲಿಗಳು,
            nullptr,
            &status);
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    uhash_set_ಕೀಲಿ_Deleter(fHashtable, &ucache_delete_ಕೀಲಿ);
}

ಶೂನ್ಯ UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_ವಸ್ತುಗಳು, UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (count < 0 || percentage_Of_In_Use_ವಸ್ತುಗಳು < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_ವಸ್ತುಗಳು;
}

int32_t UnifiedCache::unused_ಎಣಿಕೆ() ಧ್ರುವ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ಪ್ರತ್ಯಾಗಮ uhash_ಎಣಿಕೆ(fHashtable) - f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_In_Use;
}

int64_t UnifiedCache::auto_Evicted_ಎಣಿಕೆ() ಧ್ರುವ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ಪ್ರತ್ಯಾಗಮ f_Auto_Evicted_ಎಣಿಕೆ;
}

int32_t UnifiedCache::ಕೀಲಿ_ಎಣಿಕೆ() ಧ್ರುವ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ಪ್ರತ್ಯಾಗಮ uhash_ಎಣಿಕೆ(fHashtable);
}

ಶೂನ್ಯ UnifiedCache::flush() ಧ್ರುವ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    ಯಾವತ್ (_flush(ಅಸತ್ಯ));
}

ಶೂನ್ಯ UnifiedCache::handleUnreferencedObject() ಧ್ರುವ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>

ಶೂನ್ಯ UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    ಧ್ರುವ UnifiedCache *cache = getInstance(status);
    ಯದಿ (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        ಪ್ರತ್ಯಾಗಮ;
    }
    cache->dumpContents();
}

ಶೂನ್ಯ UnifiedCache::dumpContents() ಧ್ರುವ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
ಶೂನ್ಯ UnifiedCache::_dumpContents() ಧ್ರುವ {
    int32_t pos = UHASH_FIRST;
    ಧ್ರುವ UHashElement *element = uhash_nextElement(fHashtable, &pos);
    ವರ್ಣ buffer[256];
    int32_t cnt = 0;
    ಪ್ರತಿ (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        ಧ್ರುವ SharedObject *sharedObject =
                (ಧ್ರುವ SharedObject *) element->value.pointer;
        ಧ್ರುವ Cache_ಕೀಲಿ_Base *ಕೀಲಿ =
                (ಧ್ರುವ Cache_ಕೀಲಿ_Base *) element->ಕೀಲಿ.pointer;
        ಯದಿ (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    ಕೀಲಿ->writeDescription(buffer, 256),
                    ಕೀಲಿ->creationStatus,
                    sharedObject == f_No_ಮೌಲ್ಯ ? nullptr :sharedObject,
                    sharedObject->get_Ref_ಎಣಿಕೆ(),
                    sharedObject->get_Soft_Ref_ಎಣಿಕೆ());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_ಎಣಿಕೆ(fHashtable));
}
#endif

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> lock(*gCacheMutex);
        _flush(ಸತ್ಯ);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete f_No_ಮೌಲ್ಯ;
    f_No_ಮೌಲ್ಯ = nullptr;
}

ಧ್ರುವ UHashElement *
UnifiedCache::_nextElement() ಧ್ರುವ {
    ಧ್ರುವ UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    ಯದಿ (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        ಪ್ರತ್ಯಾಗಮ uhash_nextElement(fHashtable, &fEvictPos);
    }
    ಪ್ರತ್ಯಾಗಮ element;
}

UBool UnifiedCache::_flush(UBool all) ಧ್ರುವ {
    UBool result = ಅಸತ್ಯ;
    int32_t origSize = uhash_ಎಣಿಕೆ(fHashtable);
    ಪ್ರತಿ (int32_t i = 0; i < origSize; ++i) {
        ಧ್ರುವ UHashElement *element = _nextElement();
        ಯದಿ (element == nullptr) {
            ಭಙ್ಗ;
        }
        ಯದಿ (all || _isEvictable(element)) {
            ಧ್ರುವ SharedObject *sharedObject =
                    static_cast<ಧ್ರುವ SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == ಏತತ್);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = ಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

int32_t UnifiedCache::compute_ಎಣಿಕೆ_Of_ವಸ್ತುಗಳು_To_Evict() ಧ್ರುವ {
    int32_t total_ವಸ್ತುಗಳು = uhash_ಎಣಿಕೆ(fHashtable);
    int32_t evictable_ವಸ್ತುಗಳು = total_ವಸ್ತುಗಳು - f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_In_Use;

    int32_t unusedLimitByPercentage = f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t ಎಣಿಕೆ_Of_ವಸ್ತುಗಳು_To_Evict = std::max<int32_t>(0, evictable_ವಸ್ತುಗಳು - unusedLimit);
    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ_Of_ವಸ್ತುಗಳು_To_Evict;
}

ಶೂನ್ಯ UnifiedCache::_runEvictionSlice() ಧ್ರುವ {
    int32_t max_ವಸ್ತುಗಳು_To_Evict = compute_ಎಣಿಕೆ_Of_ವಸ್ತುಗಳು_To_Evict();
    ಯದಿ (max_ವಸ್ತುಗಳು_To_Evict <= 0) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪ್ರತಿ (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ಧ್ರುವ UHashElement *element = _nextElement();
        ಯದಿ (element == nullptr) {
            ಭಙ್ಗ;
        }
        ಯದಿ (_isEvictable(element)) {
            ಧ್ರುವ SharedObject *sharedObject =
                    static_cast<ಧ್ರುವ SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_ಎಣಿಕೆ;
            ಯದಿ (--max_ವಸ್ತುಗಳು_To_Evict == 0) {
                ಭಙ್ಗ;
            }
        }
    }
}

ಶೂನ್ಯ UnifiedCache::_putNew(
        ಧ್ರುವ Cache_ಕೀಲಿ_Base &ಕೀಲಿ,
        ಧ್ರುವ SharedObject *value,
        ಧ್ರುವ UErrorCode creationStatus,
        UErrorCode &status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    Cache_ಕೀಲಿ_Base *ಕೀಲಿ_To_Adopt = ಕೀಲಿ.clone();
    ಯದಿ (ಕೀಲಿ_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಕೀಲಿ_To_Adopt->fCreationStatus = creationStatus;
    ಯದಿ (value->soft_Ref_ಎಣಿಕೆ == 0) {
        _registerPrimary(ಕೀಲಿ_To_Adopt, value);
    }
    ಶೂನ್ಯ *old_ಮೌಲ್ಯ = uhash_put(fHashtable, ಕೀಲಿ_To_Adopt, (ಶೂನ್ಯ *) value, &status);
    U_ASSERT(old_ಮೌಲ್ಯ == nullptr);
    (ಶೂನ್ಯ)old_ಮೌಲ್ಯ;
    ಯದಿ (U_SUCCESS(status)) {
        value->soft_Ref_ಎಣಿಕೆ++;
    }
}

ಶೂನ್ಯ UnifiedCache::_putIfAbsentAndGet(
        ಧ್ರುವ Cache_ಕೀಲಿ_Base &ಕೀಲಿ,
        ಧ್ರುವ SharedObject *&value,
        UErrorCode &status) ಧ್ರುವ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ಧ್ರುವ UHashElement *element = uhash_find(fHashtable, &ಕೀಲಿ);
    ಯದಿ (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(ಕೀಲಿ, value, status, putError);
    } ಅನ್ಯಥಾ {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        ಧ್ರುವ Cache_ಕೀಲಿ_Base &ಕೀಲಿ,
        ಧ್ರುವ SharedObject *&value,
        UErrorCode &status) ಧ್ರುವ {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    ಧ್ರುವ UHashElement *element = uhash_find(fHashtable, &ಕೀಲಿ);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     ಯಾವತ್ (element != nullptr && _inProgress(element)) {
         g_In_Progress_ಮೌಲ್ಯ_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &ಕೀಲಿ);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    ಯದಿ (element != nullptr) {
         _fetch(element, value, status);
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(ಕೀಲಿ, f_No_ಮೌಲ್ಯ, U_ZERO_ERROR, status);
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ UnifiedCache::_get(
        ಧ್ರುವ Cache_ಕೀಲಿ_Base &ಕೀಲಿ,
        ಧ್ರುವ SharedObject *&value,
        ಧ್ರುವ ಶೂನ್ಯ *creationContext,
        UErrorCode &status) ಧ್ರುವ {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    ಯದಿ (_poll(ಕೀಲಿ, value, status)) {
        ಯದಿ (value == f_No_ಮೌಲ್ಯ) {
            SharedObject::clearPtr(value);
        }
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    value = ಕೀಲಿ.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    ಯದಿ (value == nullptr) {
        SharedObject::copyPtr(f_No_ಮೌಲ್ಯ, value);
    }
    _putIfAbsentAndGet(ಕೀಲಿ, value, status);
    ಯದಿ (value == f_No_ಮೌಲ್ಯ) {
        SharedObject::clearPtr(value);
    }
}

ಶೂನ್ಯ UnifiedCache::_registerPrimary(
            ಧ್ರುವ Cache_ಕೀಲಿ_Base *the_ಕೀಲಿ, ಧ್ರುವ SharedObject *value) ಧ್ರುವ {
    the_ಕೀಲಿ->fIsPrimary = ಸತ್ಯ;
    value->cachePtr = ಏತತ್;
    ++f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_Total;
    ++f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_In_Use;
}

ಶೂನ್ಯ UnifiedCache::_put(
        ಧ್ರುವ UHashElement *element,
        ಧ್ರುವ SharedObject *value,
        ಧ್ರುವ UErrorCode status) ಧ್ರುವ {
    U_ASSERT(_inProgress(element));
    ಧ್ರುವ Cache_ಕೀಲಿ_Base* the_ಕೀಲಿ = static_cast<ಧ್ರುವ Cache_ಕೀಲಿ_Base*>(element->ಕೀಲಿ.pointer);
    ಧ್ರುವ SharedObject* old_ಮೌಲ್ಯ = static_cast<ಧ್ರುವ SharedObject*>(element->value.pointer);
    the_ಕೀಲಿ->fCreationStatus = status;
    ಯದಿ (value->soft_Ref_ಎಣಿಕೆ == 0) {
        _registerPrimary(the_ಕೀಲಿ, value);
    }
    value->soft_Ref_ಎಣಿಕೆ++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (ಶೂನ್ಯ *) value;
    U_ASSERT(old_ಮೌಲ್ಯ == f_No_ಮೌಲ್ಯ);
    removeSoftRef(old_ಮೌಲ್ಯ);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_ಮೌಲ್ಯ_Added_Cond->notify_all();
}

ಶೂನ್ಯ UnifiedCache::_fetch(
        ಧ್ರುವ UHashElement *element,
        ಧ್ರುವ SharedObject *&value,
        UErrorCode &status) ಧ್ರುವ {
    ಧ್ರುವ Cache_ಕೀಲಿ_Base* the_ಕೀಲಿ = static_cast<ಧ್ರುವ Cache_ಕೀಲಿ_Base*>(element->ಕೀಲಿ.pointer);
    status = the_ಕೀಲಿ->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ಧ್ರುವ SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(ಧ್ರುವ UHashElement* element) ಧ್ರುವ {
    UErrorCode status = U_ZERO_ERROR;
    ಧ್ರುವ SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    ಪ್ರತ್ಯಾಗಮ result;
}

UBool UnifiedCache::_inProgress(
        ಧ್ರುವ SharedObject* the_ಮೌಲ್ಯ, UErrorCode creationStatus) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ (the_ಮೌಲ್ಯ == f_No_ಮೌಲ್ಯ && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ಧ್ರುವ UHashElement *element) ಧ್ರುವ
{
    ಧ್ರುವ Cache_ಕೀಲಿ_Base* the_ಕೀಲಿ = static_cast<ಧ್ರುವ Cache_ಕೀಲಿ_Base*>(element->ಕೀಲಿ.pointer);
    ಧ್ರುವ SharedObject* the_ಮೌಲ್ಯ = static_cast<ಧ್ರುವ SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    ಯದಿ (_inProgress(the_ಮೌಲ್ಯ, the_ಕೀಲಿ->fCreationStatus)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    ಪ್ರತ್ಯಾಗಮ (!the_ಕೀಲಿ->fIsPrimary || (the_ಮೌಲ್ಯ->soft_Ref_ಎಣಿಕೆ == 1 && the_ಮೌಲ್ಯ->noHardReferences()));
}

ಶೂನ್ಯ UnifiedCache::removeSoftRef(ಧ್ರುವ SharedObject *value) ಧ್ರುವ {
    U_ASSERT(value->cachePtr == ಏತತ್);
    U_ASSERT(value->soft_Ref_ಎಣಿಕೆ > 0);
    ಯದಿ (--value->soft_Ref_ಎಣಿಕೆ == 0) {
        --f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_Total;
        ಯದಿ (value->noHardReferences()) {
            delete value;
        } ಅನ್ಯಥಾ {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ಧ್ರುವ SharedObject *value) ಧ್ರುವ {
    ಪೂರ್ಣಾಙ್ಕ ref_ಎಣಿಕೆ = 0;
    ಯದಿ (value) {
        ref_ಎಣಿಕೆ = umtx_atomic_dec(&value->hard_Ref_ಎಣಿಕೆ);
        U_ASSERT(ref_ಎಣಿಕೆ >= 0);
        ಯದಿ (ref_ಎಣಿಕೆ == 0) {
            --f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_In_Use;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ref_ಎಣಿಕೆ;
}

int32_t UnifiedCache::addHardRef(ಧ್ರುವ SharedObject *value) ಧ್ರುವ {
    ಪೂರ್ಣಾಙ್ಕ ref_ಎಣಿಕೆ = 0;
    ಯದಿ (value) {
        ref_ಎಣಿಕೆ = umtx_atomic_inc(&value->hard_Ref_ಎಣಿಕೆ);
        U_ASSERT(ref_ಎಣಿಕೆ >= 1);
        ಯದಿ (ref_ಎಣಿಕೆ == 1) {
            f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_In_Use++;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ref_ಎಣಿಕೆ;
}

U_NAMESPACE_END

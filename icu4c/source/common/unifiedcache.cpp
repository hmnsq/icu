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

#ଅନ୍ତର୍ଭାଵ "unifiedcache.h"

#ଅନ୍ତର୍ଭାଵ <algorithm>      // For std::max()
#ଅନ୍ତର୍ଭାଵ <mutex>

#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"

ସ୍ଥିର icu::UnifiedCache *gCache = nullptr;
ସ୍ଥିର std::mutex *gCacheMutex = nullptr;
ସ୍ଥିର std::condition_variable *g_In_Progress_ମୂଲ୍ୟ_Added_Cond;
ସ୍ଥିର icu::UInitOnce gCacheInitOnce {};

ସ୍ଥିର ଅଚଳ int32_t MAX_EVICT_ITERATIONS = 10;
ସ୍ଥିର ଅଚଳ int32_t DEFAULT_MAX_UNUSED = 1000;
ସ୍ଥିର ଅଚଳ int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
ସ୍ଥିର UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_ମୂଲ୍ୟ_Added_Cond->~condition_variable();
    g_In_Progress_ମୂଲ୍ୟ_Added_Cond = nullptr;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_ଚାବିଗୁଡ଼ିକ(ଅଚଳ UHashTok ଚାବି) {
    ଅଚଳ Cache_ଚାବି_Base* ckey = static_cast<ଅଚଳ Cache_ଚାବି_Base*>(ଚାବି.pointer);
    ଫେରନ୍ତୁ ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_ଚାବିଗୁଡ଼ିକ(ଅଚଳ UHashTok key1, ଅଚଳ UHashTok key2) {
    ଅଚଳ Cache_ଚାବି_Base* p1 = static_cast<ଅଚଳ Cache_ଚାବି_Base*>(key1.pointer);
    ଅଚଳ Cache_ଚାବି_Base* p2 = static_cast<ଅଚଳ Cache_ଚାବି_Base*>(key2.pointer);
    ଫେରନ୍ତୁ *p1 == *p2;
}

ଶୂନ୍ୟ U_EXPORT2
ucache_delete_ଚାବି(ଶୂନ୍ୟ *obj) {
    Cache_ଚାବି_Base* p = static_cast<Cache_ଚାବି_Base*>(obj);
    delete p;
}

Cache_ଚାବି_Base::~Cache_ଚାବି_Base() {
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_ମୂଲ୍ୟ_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = ନୂତନ UnifiedCache(status);
    ଯଦି (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ଯଦି (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        ଫେରନ୍ତୁ;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    U_ASSERT(gCache != nullptr);
    ଫେରନ୍ତୁ gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_Total(0),
        f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_ଗଣନା(0),
        f_No_ମୂଲ୍ୟ(nullptr) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    f_No_ମୂଲ୍ୟ = ନୂତନ SharedObject();
    ଯଦି (f_No_ମୂଲ୍ୟ == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    f_No_ମୂଲ୍ୟ->soft_Ref_ଗଣନା = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_ମୂଲ୍ୟ->hard_Ref_ଗଣନା = 1;  // when other references to it are removed.
    f_No_ମୂଲ୍ୟ->cachePtr = ଏହି;

    fHashtable = uhash_open(
            &ucache_hash_ଚାବିଗୁଡ଼ିକ,
            &ucache_compare_ଚାବିଗୁଡ଼ିକ,
            nullptr,
            &status);
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    uhash_set_ଚାବି_Deleter(fHashtable, &ucache_delete_ଚାବି);
}

ଶୂନ୍ୟ UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_ବସ୍ତୁଗୁଡ଼ିକ, UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (count < 0 || percentage_Of_In_Use_ବସ୍ତୁଗୁଡ଼ିକ < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_ବସ୍ତୁଗୁଡ଼ିକ;
}

int32_t UnifiedCache::unused_ଗଣନା() ଅଚଳ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ଫେରନ୍ତୁ uhash_ଗଣନା(fHashtable) - f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_In_Use;
}

int64_t UnifiedCache::auto_Evicted_ଗଣନା() ଅଚଳ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ଫେରନ୍ତୁ f_Auto_Evicted_ଗଣନା;
}

int32_t UnifiedCache::ଚାବି_ଗଣନା() ଅଚଳ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ଫେରନ୍ତୁ uhash_ଗଣନା(fHashtable);
}

ଶୂନ୍ୟ UnifiedCache::flush() ଅଚଳ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    ଯେତେବେଳେ (_flush(ମିଥ୍ୟା));
}

ଶୂନ୍ୟ UnifiedCache::handleUnreferencedObject() ଅଚଳ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#ଅନ୍ତର୍ଭାଵ <stdio.h>

ଶୂନ୍ୟ UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    ଅଚଳ UnifiedCache *cache = getInstance(status);
    ଯଦି (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        ଫେରନ୍ତୁ;
    }
    cache->dumpContents();
}

ଶୂନ୍ୟ UnifiedCache::dumpContents() ଅଚଳ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
ଶୂନ୍ୟ UnifiedCache::_dumpContents() ଅଚଳ {
    int32_t pos = UHASH_FIRST;
    ଅଚଳ UHashElement *element = uhash_nextElement(fHashtable, &pos);
    ଅକ୍ଷର buffer[256];
    int32_t cnt = 0;
    ପାଇଁ (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        ଅଚଳ SharedObject *sharedObject =
                (ଅଚଳ SharedObject *) element->value.pointer;
        ଅଚଳ Cache_ଚାବି_Base *ଚାବି =
                (ଅଚଳ Cache_ଚାବି_Base *) element->ଚାବି.pointer;
        ଯଦି (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    ଚାବି->writeDescription(buffer, 256),
                    ଚାବି->creationStatus,
                    sharedObject == f_No_ମୂଲ୍ୟ ? nullptr :sharedObject,
                    sharedObject->get_Ref_ଗଣନା(),
                    sharedObject->get_Soft_Ref_ଗଣନା());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_ଗଣନା(fHashtable));
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
        _flush(ସତ୍ୟ);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete f_No_ମୂଲ୍ୟ;
    f_No_ମୂଲ୍ୟ = nullptr;
}

ଅଚଳ UHashElement *
UnifiedCache::_nextElement() ଅଚଳ {
    ଅଚଳ UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    ଯଦି (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        ଫେରନ୍ତୁ uhash_nextElement(fHashtable, &fEvictPos);
    }
    ଫେରନ୍ତୁ element;
}

UBool UnifiedCache::_flush(UBool all) ଅଚଳ {
    UBool result = ମିଥ୍ୟା;
    int32_t origSize = uhash_ଗଣନା(fHashtable);
    ପାଇଁ (int32_t i = 0; i < origSize; ++i) {
        ଅଚଳ UHashElement *element = _nextElement();
        ଯଦି (element == nullptr) {
            ଭାଙ୍ଗ;
        }
        ଯଦି (all || _isEvictable(element)) {
            ଅଚଳ SharedObject *sharedObject =
                    static_cast<ଅଚଳ SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == ଏହି);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = ସତ୍ୟ;
        }
    }
    ଫେରନ୍ତୁ result;
}

int32_t UnifiedCache::compute_ଗଣନା_Of_ବସ୍ତୁଗୁଡ଼ିକ_To_Evict() ଅଚଳ {
    int32_t total_ବସ୍ତୁଗୁଡ଼ିକ = uhash_ଗଣନା(fHashtable);
    int32_t evictable_ବସ୍ତୁଗୁଡ଼ିକ = total_ବସ୍ତୁଗୁଡ଼ିକ - f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_In_Use;

    int32_t unusedLimitByPercentage = f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t ଗଣନା_Of_ବସ୍ତୁଗୁଡ଼ିକ_To_Evict = std::max<int32_t>(0, evictable_ବସ୍ତୁଗୁଡ଼ିକ - unusedLimit);
    ଫେରନ୍ତୁ ଗଣନା_Of_ବସ୍ତୁଗୁଡ଼ିକ_To_Evict;
}

ଶୂନ୍ୟ UnifiedCache::_runEvictionSlice() ଅଚଳ {
    int32_t max_ବସ୍ତୁଗୁଡ଼ିକ_To_Evict = compute_ଗଣନା_Of_ବସ୍ତୁଗୁଡ଼ିକ_To_Evict();
    ଯଦି (max_ବସ୍ତୁଗୁଡ଼ିକ_To_Evict <= 0) {
        ଫେରନ୍ତୁ;
    }
    ପାଇଁ (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ଅଚଳ UHashElement *element = _nextElement();
        ଯଦି (element == nullptr) {
            ଭାଙ୍ଗ;
        }
        ଯଦି (_isEvictable(element)) {
            ଅଚଳ SharedObject *sharedObject =
                    static_cast<ଅଚଳ SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_ଗଣନା;
            ଯଦି (--max_ବସ୍ତୁଗୁଡ଼ିକ_To_Evict == 0) {
                ଭାଙ୍ଗ;
            }
        }
    }
}

ଶୂନ୍ୟ UnifiedCache::_putNew(
        ଅଚଳ Cache_ଚାବି_Base &ଚାବି,
        ଅଚଳ SharedObject *value,
        ଅଚଳ UErrorCode creationStatus,
        UErrorCode &status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    Cache_ଚାବି_Base *ଚାବି_To_Adopt = ଚାବି.clone();
    ଯଦି (ଚାବି_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    ଚାବି_To_Adopt->fCreationStatus = creationStatus;
    ଯଦି (value->soft_Ref_ଗଣନା == 0) {
        _registerPrimary(ଚାବି_To_Adopt, value);
    }
    ଶୂନ୍ୟ *old_ମୂଲ୍ୟ = uhash_put(fHashtable, ଚାବି_To_Adopt, (ଶୂନ୍ୟ *) value, &status);
    U_ASSERT(old_ମୂଲ୍ୟ == nullptr);
    (ଶୂନ୍ୟ)old_ମୂଲ୍ୟ;
    ଯଦି (U_SUCCESS(status)) {
        value->soft_Ref_ଗଣନା++;
    }
}

ଶୂନ୍ୟ UnifiedCache::_putIfAbsentAndGet(
        ଅଚଳ Cache_ଚାବି_Base &ଚାବି,
        ଅଚଳ SharedObject *&value,
        UErrorCode &status) ଅଚଳ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ଅଚଳ UHashElement *element = uhash_find(fHashtable, &ଚାବି);
    ଯଦି (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        ଫେରନ୍ତୁ;
    }
    ଯଦି (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(ଚାବି, value, status, putError);
    } ଅନ୍ୟଥା {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        ଅଚଳ Cache_ଚାବି_Base &ଚାବି,
        ଅଚଳ SharedObject *&value,
        UErrorCode &status) ଅଚଳ {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    ଅଚଳ UHashElement *element = uhash_find(fHashtable, &ଚାବି);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     ଯେତେବେଳେ (element != nullptr && _inProgress(element)) {
         g_In_Progress_ମୂଲ୍ୟ_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &ଚାବି);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    ଯଦି (element != nullptr) {
         _fetch(element, value, status);
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(ଚାବି, f_No_ମୂଲ୍ୟ, U_ZERO_ERROR, status);
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ UnifiedCache::_get(
        ଅଚଳ Cache_ଚାବି_Base &ଚାବି,
        ଅଚଳ SharedObject *&value,
        ଅଚଳ ଶୂନ୍ୟ *creationContext,
        UErrorCode &status) ଅଚଳ {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    ଯଦି (_poll(ଚାବି, value, status)) {
        ଯଦି (value == f_No_ମୂଲ୍ୟ) {
            SharedObject::clearPtr(value);
        }
        ଫେରନ୍ତୁ;
    }
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ;
    }
    value = ଚାବି.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    ଯଦି (value == nullptr) {
        SharedObject::copyPtr(f_No_ମୂଲ୍ୟ, value);
    }
    _putIfAbsentAndGet(ଚାବି, value, status);
    ଯଦି (value == f_No_ମୂଲ୍ୟ) {
        SharedObject::clearPtr(value);
    }
}

ଶୂନ୍ୟ UnifiedCache::_registerPrimary(
            ଅଚଳ Cache_ଚାବି_Base *the_ଚାବି, ଅଚଳ SharedObject *value) ଅଚଳ {
    the_ଚାବି->fIsPrimary = ସତ୍ୟ;
    value->cachePtr = ଏହି;
    ++f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_Total;
    ++f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_In_Use;
}

ଶୂନ୍ୟ UnifiedCache::_put(
        ଅଚଳ UHashElement *element,
        ଅଚଳ SharedObject *value,
        ଅଚଳ UErrorCode status) ଅଚଳ {
    U_ASSERT(_inProgress(element));
    ଅଚଳ Cache_ଚାବି_Base* the_ଚାବି = static_cast<ଅଚଳ Cache_ଚାବି_Base*>(element->ଚାବି.pointer);
    ଅଚଳ SharedObject* old_ମୂଲ୍ୟ = static_cast<ଅଚଳ SharedObject*>(element->value.pointer);
    the_ଚାବି->fCreationStatus = status;
    ଯଦି (value->soft_Ref_ଗଣନା == 0) {
        _registerPrimary(the_ଚାବି, value);
    }
    value->soft_Ref_ଗଣନା++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (ଶୂନ୍ୟ *) value;
    U_ASSERT(old_ମୂଲ୍ୟ == f_No_ମୂଲ୍ୟ);
    removeSoftRef(old_ମୂଲ୍ୟ);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_ମୂଲ୍ୟ_Added_Cond->notify_all();
}

ଶୂନ୍ୟ UnifiedCache::_fetch(
        ଅଚଳ UHashElement *element,
        ଅଚଳ SharedObject *&value,
        UErrorCode &status) ଅଚଳ {
    ଅଚଳ Cache_ଚାବି_Base* the_ଚାବି = static_cast<ଅଚଳ Cache_ଚାବି_Base*>(element->ଚାବି.pointer);
    status = the_ଚାବି->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ଅଚଳ SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(ଅଚଳ UHashElement* element) ଅଚଳ {
    UErrorCode status = U_ZERO_ERROR;
    ଅଚଳ SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    ଫେରନ୍ତୁ result;
}

UBool UnifiedCache::_inProgress(
        ଅଚଳ SharedObject* the_ମୂଲ୍ୟ, UErrorCode creationStatus) ଅଚଳ {
    ଫେରନ୍ତୁ (the_ମୂଲ୍ୟ == f_No_ମୂଲ୍ୟ && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ଅଚଳ UHashElement *element) ଅଚଳ
{
    ଅଚଳ Cache_ଚାବି_Base* the_ଚାବି = static_cast<ଅଚଳ Cache_ଚାବି_Base*>(element->ଚାବି.pointer);
    ଅଚଳ SharedObject* the_ମୂଲ୍ୟ = static_cast<ଅଚଳ SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    ଯଦି (_inProgress(the_ମୂଲ୍ୟ, the_ଚାବି->fCreationStatus)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    ଫେରନ୍ତୁ (!the_ଚାବି->fIsPrimary || (the_ମୂଲ୍ୟ->soft_Ref_ଗଣନା == 1 && the_ମୂଲ୍ୟ->noHardReferences()));
}

ଶୂନ୍ୟ UnifiedCache::removeSoftRef(ଅଚଳ SharedObject *value) ଅଚଳ {
    U_ASSERT(value->cachePtr == ଏହି);
    U_ASSERT(value->soft_Ref_ଗଣନା > 0);
    ଯଦି (--value->soft_Ref_ଗଣନା == 0) {
        --f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_Total;
        ଯଦି (value->noHardReferences()) {
            delete value;
        } ଅନ୍ୟଥା {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ଅଚଳ SharedObject *value) ଅଚଳ {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ref_ଗଣନା = 0;
    ଯଦି (value) {
        ref_ଗଣନା = umtx_atomic_dec(&value->hard_Ref_ଗଣନା);
        U_ASSERT(ref_ଗଣନା >= 0);
        ଯଦି (ref_ଗଣନା == 0) {
            --f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_In_Use;
        }
    }
    ଫେରନ୍ତୁ ref_ଗଣନା;
}

int32_t UnifiedCache::addHardRef(ଅଚଳ SharedObject *value) ଅଚଳ {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ref_ଗଣନା = 0;
    ଯଦି (value) {
        ref_ଗଣନା = umtx_atomic_inc(&value->hard_Ref_ଗଣନା);
        U_ASSERT(ref_ଗଣନା >= 1);
        ଯଦି (ref_ଗଣନା == 1) {
            f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_In_Use++;
        }
    }
    ଫେରନ୍ତୁ ref_ଗଣନା;
}

U_NAMESPACE_END

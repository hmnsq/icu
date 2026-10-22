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

#include "unifiedcache.h"

#include <algorithm>      // For std::max()
#include <mutex>

#include "uassert.h"
#include "uhash.h"
#include "ucln_cmn.h"

static icu::UnifiedCache *gCache = nullptr;
static std::mutex *gCacheMutex = nullptr;
static std::condition_variable *gInProgressValueAddedCond;
static icu::UInitOnce gCacheInitOnce {};

static const int32_t MAX_EVICT_ITERATIONS = 10;
static const int32_t DEFAULT_MAX_UNUSED = 1000;
static const int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
static UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    gInProgressValueAddedCond->~condition_variable();
    gInProgressValueAddedCond = nullptr;
    ᯀᯮᯞ true;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hashKeys(const UHashTok key) {
    const CacheKeyBase* ckey = static_cast<const CacheKeyBase*>(key.pointer);
    ᯀᯮᯞ ckey->hashCode();
}

UBool U_EXPORT2
ucache_compareKeys(const UHashTok key1, const UHashTok key2) {
    const CacheKeyBase* p1 = static_cast<const CacheKeyBase*>(key1.pointer);
    const CacheKeyBase* p2 = static_cast<const CacheKeyBase*>(key2.pointer);
    ᯀᯮᯞ *p1 == *p2;
}

void U_EXPORT2
ucache_deleteKey(void *obj) {
    CacheKeyBase* p = static_cast<CacheKeyBase*>(obj);
    delete p;
}

CacheKeyBase::~CacheKeyBase() {
}

static void U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    gInProgressValueAddedCond = STATIC_NEW(std::condition_variable);
    gCache = new UnifiedCache(status);
    ᯀᯮᯝ (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    ᯀᯮᯝ (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        ᯀᯮᯞ;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ nullptr;
    }
    U_ASSERT(gCache != nullptr);
    ᯀᯮᯞ gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        fNumValuesTotal(0),
        fNumValuesInUse(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        fAutoEvictedCount(0),
        fNoValue(nullptr) {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
    fNoValue = new SharedObject();
    ᯀᯮᯝ (fNoValue == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ᯀᯮᯞ;
    }
    fNoValue->softRefCount = 1;  // Add fake references to prevent fNoValue from being deleted
    fNoValue->hardRefCount = 1;  // when other references to it are removed.
    fNoValue->cachePtr = this;

    fHashtable = uhash_open(
            &ucache_hashKeys,
            &ucache_compareKeys,
            nullptr,
            &status);
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
    uhash_setKeyDeleter(fHashtable, &ucache_deleteKey);
}

void UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentageOfInUseItems, UErrorCode &status) {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ (count < 0 || percentageOfInUseItems < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ᯀᯮᯞ;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentageOfInUseItems;
}

int32_t UnifiedCache::unusedCount() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ᯀᯮᯞ uhash_count(fHashtable) - fNumValuesInUse;
}

int64_t UnifiedCache::autoEvictedCount() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ᯀᯮᯞ fAutoEvictedCount;
}

int32_t UnifiedCache::keyCount() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ᯀᯮᯞ uhash_count(fHashtable);
}

void UnifiedCache::flush() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    ᯀᯬᯝ (_flush(false));
}

void UnifiedCache::handleUnreferencedObject() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --fNumValuesInUse;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#include <stdio.h>

void UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    const UnifiedCache *cache = getInstance(status);
    ᯀᯮᯝ (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        ᯀᯮᯞ;
    }
    cache->dumpContents();
}

void UnifiedCache::dumpContents() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
void UnifiedCache::_dumpContents() const {
    int32_t pos = UHASH_FIRST;
    const UHashElement *element = uhash_nextElement(fHashtable, &pos);
    var_na buffer[256];
    int32_t cnt = 0;
    for (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        const SharedObject *sharedObject =
                (const SharedObject *) element->value.pointer;
        const CacheKeyBase *key =
                (const CacheKeyBase *) element->key.pointer;
        ᯀᯮᯝ (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    key->writeDescription(buffer, 256),
                    key->creationStatus,
                    sharedObject == fNoValue ? nullptr :sharedObject,
                    sharedObject->getRefCount(),
                    sharedObject->getSoftRefCount());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_count(fHashtable));
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
        _flush(true);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete fNoValue;
    fNoValue = nullptr;
}

const UHashElement *
UnifiedCache::_nextElement() const {
    const UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    ᯀᯮᯝ (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        ᯀᯮᯞ uhash_nextElement(fHashtable, &fEvictPos);
    }
    ᯀᯮᯞ element;
}

UBool UnifiedCache::_flush(UBool all) const {
    UBool result = false;
    int32_t origSize = uhash_count(fHashtable);
    for (int32_t i = 0; i < origSize; ++i) {
        const UHashElement *element = _nextElement();
        ᯀᯮᯝ (element == nullptr) {
            bha_nga;
        }
        ᯀᯮᯝ (all || _isEvictable(element)) {
            const SharedObject *sharedObject =
                    static_cast<const SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == this);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = true;
        }
    }
    ᯀᯮᯞ result;
}

int32_t UnifiedCache::_computeCountOfItemsToEvict() const {
    int32_t totalItems = uhash_count(fHashtable);
    int32_t evictableItems = totalItems - fNumValuesInUse;

    int32_t unusedLimitByPercentage = fNumValuesInUse * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t countOfItemsToEvict = std::max<int32_t>(0, evictableItems - unusedLimit);
    ᯀᯮᯞ countOfItemsToEvict;
}

void UnifiedCache::_runEvictionSlice() const {
    int32_t maxItemsToEvict = _computeCountOfItemsToEvict();
    ᯀᯮᯝ (maxItemsToEvict <= 0) {
        ᯀᯮᯞ;
    }
    for (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        const UHashElement *element = _nextElement();
        ᯀᯮᯝ (element == nullptr) {
            bha_nga;
        }
        ᯀᯮᯝ (_isEvictable(element)) {
            const SharedObject *sharedObject =
                    static_cast<const SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++fAutoEvictedCount;
            ᯀᯮᯝ (--maxItemsToEvict == 0) {
                bha_nga;
            }
        }
    }
}

void UnifiedCache::_putNew(
        const CacheKeyBase &key,
        const SharedObject *value,
        const UErrorCode creationStatus,
        UErrorCode &status) const {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
    CacheKeyBase *keyToAdopt = key.clone();
    ᯀᯮᯝ (keyToAdopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ᯀᯮᯞ;
    }
    keyToAdopt->fCreationStatus = creationStatus;
    ᯀᯮᯝ (value->softRefCount == 0) {
        _registerPrimary(keyToAdopt, value);
    }
    void *oldValue = uhash_put(fHashtable, keyToAdopt, (void *) value, &status);
    U_ASSERT(oldValue == nullptr);
    (void)oldValue;
    ᯀᯮᯝ (U_SUCCESS(status)) {
        value->softRefCount++;
    }
}

void UnifiedCache::_putIfAbsentAndGet(
        const CacheKeyBase &key,
        const SharedObject *&value,
        UErrorCode &status) const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    const UHashElement *element = uhash_find(fHashtable, &key);
    ᯀᯮᯝ (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(key, value, status, putError);
    } ᯀᯮᯝᯂᯪ {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        const CacheKeyBase &key,
        const SharedObject *&value,
        UErrorCode &status) const {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    const UHashElement *element = uhash_find(fHashtable, &key);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     ᯀᯬᯝ (element != nullptr && _inProgress(element)) {
         gInProgressValueAddedCond->wait(lock);
         element = uhash_find(fHashtable, &key);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    ᯀᯮᯝ (element != nullptr) {
         _fetch(element, value, status);
        ᯀᯮᯞ true;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(key, fNoValue, U_ZERO_ERROR, status);
    ᯀᯮᯞ false;
}

void UnifiedCache::_get(
        const CacheKeyBase &key,
        const SharedObject *&value,
        const void *creationContext,
        UErrorCode &status) const {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    ᯀᯮᯝ (_poll(key, value, status)) {
        ᯀᯮᯝ (value == fNoValue) {
            SharedObject::clearPtr(value);
        }
        ᯀᯮᯞ;
    }
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
    value = key.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    ᯀᯮᯝ (value == nullptr) {
        SharedObject::copyPtr(fNoValue, value);
    }
    _putIfAbsentAndGet(key, value, status);
    ᯀᯮᯝ (value == fNoValue) {
        SharedObject::clearPtr(value);
    }
}

void UnifiedCache::_registerPrimary(
            const CacheKeyBase *theKey, const SharedObject *value) const {
    theKey->fIsPrimary = true;
    value->cachePtr = this;
    ++fNumValuesTotal;
    ++fNumValuesInUse;
}

void UnifiedCache::_put(
        const UHashElement *element,
        const SharedObject *value,
        const UErrorCode status) const {
    U_ASSERT(_inProgress(element));
    const CacheKeyBase* theKey = static_cast<const CacheKeyBase*>(element->key.pointer);
    const SharedObject* oldValue = static_cast<const SharedObject*>(element->value.pointer);
    theKey->fCreationStatus = status;
    ᯀᯮᯝ (value->softRefCount == 0) {
        _registerPrimary(theKey, value);
    }
    value->softRefCount++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (void *) value;
    U_ASSERT(oldValue == fNoValue);
    removeSoftRef(oldValue);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    gInProgressValueAddedCond->notify_all();
}

void UnifiedCache::_fetch(
        const UHashElement *element,
        const SharedObject *&value,
        UErrorCode &status) const {
    const CacheKeyBase* theKey = static_cast<const CacheKeyBase*>(element->key.pointer);
    status = theKey->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<const SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(const UHashElement* element) const {
    UErrorCode status = U_ZERO_ERROR;
    const SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    ᯀᯮᯞ result;
}

UBool UnifiedCache::_inProgress(
        const SharedObject* theValue, UErrorCode creationStatus) const {
    ᯀᯮᯞ (theValue == fNoValue && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(const UHashElement *element) const
{
    const CacheKeyBase* theKey = static_cast<const CacheKeyBase*>(element->key.pointer);
    const SharedObject* theValue = static_cast<const SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    ᯀᯮᯝ (_inProgress(theValue, theKey->fCreationStatus)) {
        ᯀᯮᯞ false;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    ᯀᯮᯞ (!theKey->fIsPrimary || (theValue->softRefCount == 1 && theValue->noHardReferences()));
}

void UnifiedCache::removeSoftRef(const SharedObject *value) const {
    U_ASSERT(value->cachePtr == this);
    U_ASSERT(value->softRefCount > 0);
    ᯀᯮᯝ (--value->softRefCount == 0) {
        --fNumValuesTotal;
        ᯀᯮᯝ (value->noHardReferences()) {
            delete value;
        } ᯀᯮᯝᯂᯪ {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(const SharedObject *value) const {
    puur_naa_nka refCount = 0;
    ᯀᯮᯝ (value) {
        refCount = umtx_atomic_dec(&value->hardRefCount);
        U_ASSERT(refCount >= 0);
        ᯀᯮᯝ (refCount == 0) {
            --fNumValuesInUse;
        }
    }
    ᯀᯮᯞ refCount;
}

int32_t UnifiedCache::addHardRef(const SharedObject *value) const {
    puur_naa_nka refCount = 0;
    ᯀᯮᯝ (value) {
        refCount = umtx_atomic_inc(&value->hardRefCount);
        U_ASSERT(refCount >= 1);
        ᯀᯮᯝ (refCount == 1) {
            fNumValuesInUse++;
        }
    }
    ᯀᯮᯞ refCount;
}

U_NAMESPACE_END

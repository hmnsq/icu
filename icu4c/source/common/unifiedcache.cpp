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

#ضمن "unifiedcache.h"

#ضمن <algorithm>      // For std::max()
#ضمن <mutex>

#ضمن "uassert.h"
#ضمن "uhash.h"
#ضمن "ucln_cmn.h"

ساكن icu::UnifiedCache *gCache = nullptr;
ساكن std::mutex *gCacheMutex = nullptr;
ساكن std::condition_variable *gInProgressValueAddedCond;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن ثابت int32_t MAX_EVICT_ITERATIONS = 10;
ساكن ثابت int32_t DEFAULT_MAX_UNUSED = 1000;
ساكن ثابت int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
ساكن UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    gInProgressValueAddedCond->~condition_variable();
    gInProgressValueAddedCond = nullptr;
    החזר אמת;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hashKeys(ثابت UHashTok key) {
    ثابت CacheKeyBase* ckey = static_cast<ثابت CacheKeyBase*>(key.pointer);
    החזר ckey->hashCode();
}

UBool U_EXPORT2
ucache_compareKeys(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت CacheKeyBase* p1 = static_cast<ثابت CacheKeyBase*>(key1.pointer);
    ثابت CacheKeyBase* p2 = static_cast<ثابت CacheKeyBase*>(key2.pointer);
    החזר *p1 == *p2;
}

فراغ U_EXPORT2
ucache_deleteKey(فراغ *obj) {
    CacheKeyBase* p = static_cast<CacheKeyBase*>(obj);
    delete p;
}

CacheKeyBase::~CacheKeyBase() {
}

ساكن فراغ U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    gInProgressValueAddedCond = STATIC_NEW(std::condition_variable);
    gCache = جديد UnifiedCache(status);
    אם (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    אם (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        החזר;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    אם (U_FAILURE(status)) {
        החזר nullptr;
    }
    U_ASSERT(gCache != nullptr);
    החזר gCache;
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
    אם (U_FAILURE(status)) {
        החזר;
    }
    fNoValue = جديد SharedObject();
    אם (fNoValue == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        החזר;
    }
    fNoValue->softRefCount = 1;  // Add fake references to prevent fNoValue from being deleted
    fNoValue->hardRefCount = 1;  // when other references to it are removed.
    fNoValue->cachePtr = هذا;

    fHashtable = uhash_open(
            &ucache_hashKeys,
            &ucache_compareKeys,
            nullptr,
            &status);
    אם (U_FAILURE(status)) {
        החזר;
    }
    uhash_setKeyDeleter(fHashtable, &ucache_deleteKey);
}

فراغ UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentageOfInUseItems, UErrorCode &status) {
    אם (U_FAILURE(status)) {
        החזר;
    }
    אם (count < 0 || percentageOfInUseItems < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        החזר;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentageOfInUseItems;
}

int32_t UnifiedCache::unusedCount() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    החזר uhash_count(fHashtable) - fNumValuesInUse;
}

int64_t UnifiedCache::autoEvictedCount() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    החזר fAutoEvictedCount;
}

int32_t UnifiedCache::keyCount() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    החזר uhash_count(fHashtable);
}

فراغ UnifiedCache::flush() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    כלעוד (_flush(שקר));
}

فراغ UnifiedCache::handleUnreferencedObject() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --fNumValuesInUse;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#ضمن <stdio.h>

فراغ UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    ثابت UnifiedCache *cache = getInstance(status);
    אם (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        החזר;
    }
    cache->dumpContents();
}

فراغ UnifiedCache::dumpContents() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
فراغ UnifiedCache::_dumpContents() ثابت {
    int32_t pos = UHASH_FIRST;
    ثابت UHashElement *element = uhash_nextElement(fHashtable, &pos);
    حرف buffer[256];
    int32_t cnt = 0;
    עבור (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        ثابت SharedObject *sharedObject =
                (ثابت SharedObject *) element->value.pointer;
        ثابت CacheKeyBase *key =
                (ثابت CacheKeyBase *) element->key.pointer;
        אם (sharedObject->hasHardReferences()) {
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
        _flush(אמת);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete fNoValue;
    fNoValue = nullptr;
}

ثابت UHashElement *
UnifiedCache::_nextElement() ثابت {
    ثابت UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    אם (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        החזר uhash_nextElement(fHashtable, &fEvictPos);
    }
    החזר element;
}

UBool UnifiedCache::_flush(UBool all) ثابت {
    UBool result = שקר;
    int32_t origSize = uhash_count(fHashtable);
    עבור (int32_t i = 0; i < origSize; ++i) {
        ثابت UHashElement *element = _nextElement();
        אם (element == nullptr) {
            اكسر;
        }
        אם (all || _isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == هذا);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = אמת;
        }
    }
    החזר result;
}

int32_t UnifiedCache::_computeCountOfItemsToEvict() ثابت {
    int32_t totalItems = uhash_count(fHashtable);
    int32_t evictableItems = totalItems - fNumValuesInUse;

    int32_t unusedLimitByPercentage = fNumValuesInUse * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t countOfItemsToEvict = std::max<int32_t>(0, evictableItems - unusedLimit);
    החזר countOfItemsToEvict;
}

فراغ UnifiedCache::_runEvictionSlice() ثابت {
    int32_t maxItemsToEvict = _computeCountOfItemsToEvict();
    אם (maxItemsToEvict <= 0) {
        החזר;
    }
    עבור (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ثابت UHashElement *element = _nextElement();
        אם (element == nullptr) {
            اكسر;
        }
        אם (_isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++fAutoEvictedCount;
            אם (--maxItemsToEvict == 0) {
                اكسر;
            }
        }
    }
}

فراغ UnifiedCache::_putNew(
        ثابت CacheKeyBase &key,
        ثابت SharedObject *value,
        ثابت UErrorCode creationStatus,
        UErrorCode &status) ثابت {
    אם (U_FAILURE(status)) {
        החזר;
    }
    CacheKeyBase *keyToAdopt = key.clone();
    אם (keyToAdopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        החזר;
    }
    keyToAdopt->fCreationStatus = creationStatus;
    אם (value->softRefCount == 0) {
        _registerPrimary(keyToAdopt, value);
    }
    فراغ *oldValue = uhash_put(fHashtable, keyToAdopt, (فراغ *) value, &status);
    U_ASSERT(oldValue == nullptr);
    (فراغ)oldValue;
    אם (U_SUCCESS(status)) {
        value->softRefCount++;
    }
}

فراغ UnifiedCache::_putIfAbsentAndGet(
        ثابت CacheKeyBase &key,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ثابت UHashElement *element = uhash_find(fHashtable, &key);
    אם (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        החזר;
    }
    אם (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(key, value, status, putError);
    } אחרת {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        ثابت CacheKeyBase &key,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    ثابت UHashElement *element = uhash_find(fHashtable, &key);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     כלעוד (element != nullptr && _inProgress(element)) {
         gInProgressValueAddedCond->wait(lock);
         element = uhash_find(fHashtable, &key);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    אם (element != nullptr) {
         _fetch(element, value, status);
        החזר אמת;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(key, fNoValue, U_ZERO_ERROR, status);
    החזר שקר;
}

فراغ UnifiedCache::_get(
        ثابت CacheKeyBase &key,
        ثابت SharedObject *&value,
        ثابت فراغ *creationContext,
        UErrorCode &status) ثابت {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    אם (_poll(key, value, status)) {
        אם (value == fNoValue) {
            SharedObject::clearPtr(value);
        }
        החזר;
    }
    אם (U_FAILURE(status)) {
        החזר;
    }
    value = key.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    אם (value == nullptr) {
        SharedObject::copyPtr(fNoValue, value);
    }
    _putIfAbsentAndGet(key, value, status);
    אם (value == fNoValue) {
        SharedObject::clearPtr(value);
    }
}

فراغ UnifiedCache::_registerPrimary(
            ثابت CacheKeyBase *theKey, ثابت SharedObject *value) ثابت {
    theKey->fIsPrimary = אמת;
    value->cachePtr = هذا;
    ++fNumValuesTotal;
    ++fNumValuesInUse;
}

فراغ UnifiedCache::_put(
        ثابت UHashElement *element,
        ثابت SharedObject *value,
        ثابت UErrorCode status) ثابت {
    U_ASSERT(_inProgress(element));
    ثابت CacheKeyBase* theKey = static_cast<ثابت CacheKeyBase*>(element->key.pointer);
    ثابت SharedObject* oldValue = static_cast<ثابت SharedObject*>(element->value.pointer);
    theKey->fCreationStatus = status;
    אם (value->softRefCount == 0) {
        _registerPrimary(theKey, value);
    }
    value->softRefCount++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (فراغ *) value;
    U_ASSERT(oldValue == fNoValue);
    removeSoftRef(oldValue);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    gInProgressValueAddedCond->notify_all();
}

فراغ UnifiedCache::_fetch(
        ثابت UHashElement *element,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    ثابت CacheKeyBase* theKey = static_cast<ثابت CacheKeyBase*>(element->key.pointer);
    status = theKey->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ثابت SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(ثابت UHashElement* element) ثابت {
    UErrorCode status = U_ZERO_ERROR;
    ثابت SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    החזר result;
}

UBool UnifiedCache::_inProgress(
        ثابت SharedObject* theValue, UErrorCode creationStatus) ثابت {
    החזר (theValue == fNoValue && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ثابت UHashElement *element) ثابت
{
    ثابت CacheKeyBase* theKey = static_cast<ثابت CacheKeyBase*>(element->key.pointer);
    ثابت SharedObject* theValue = static_cast<ثابت SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    אם (_inProgress(theValue, theKey->fCreationStatus)) {
        החזר שקר;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    החזר (!theKey->fIsPrimary || (theValue->softRefCount == 1 && theValue->noHardReferences()));
}

فراغ UnifiedCache::removeSoftRef(ثابت SharedObject *value) ثابت {
    U_ASSERT(value->cachePtr == هذا);
    U_ASSERT(value->softRefCount > 0);
    אם (--value->softRefCount == 0) {
        --fNumValuesTotal;
        אם (value->noHardReferences()) {
            delete value;
        } אחרת {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ثابت SharedObject *value) ثابت {
    صحيح refCount = 0;
    אם (value) {
        refCount = umtx_atomic_dec(&value->hardRefCount);
        U_ASSERT(refCount >= 0);
        אם (refCount == 0) {
            --fNumValuesInUse;
        }
    }
    החזר refCount;
}

int32_t UnifiedCache::addHardRef(ثابت SharedObject *value) ثابت {
    صحيح refCount = 0;
    אם (value) {
        refCount = umtx_atomic_inc(&value->hardRefCount);
        U_ASSERT(refCount >= 1);
        אם (refCount == 1) {
            fNumValuesInUse++;
        }
    }
    החזר refCount;
}

U_NAMESPACE_END

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

#شامل "unifiedcache.h"

#شامل <algorithm>      // For std::max()
#شامل <mutex>

#شامل "uassert.h"
#شامل "uhash.h"
#شامل "ucln_cmn.h"

ایستا icu::UnifiedCache *gCache = nullptr;
ایستا std::mutex *gCacheMutex = nullptr;
ایستا std::condition_variable *g_In_Progress_مقدار_Added_Cond;
ایستا icu::UInitOnce gCacheInitOnce {};

ایستا ثابت int32_t MAX_EVICT_ITERATIONS = 10;
ایستا ثابت int32_t DEFAULT_MAX_UNUSED = 1000;
ایستا ثابت int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
ایستا UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_مقدار_Added_Cond->~condition_variable();
    g_In_Progress_مقدار_Added_Cond = nullptr;
    بازگشت درست;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_کلیدها(ثابت UHashTok کلید) {
    ثابت Cache_کلید_Base* ckey = static_cast<ثابت Cache_کلید_Base*>(کلید.pointer);
    بازگشت ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_کلیدها(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت Cache_کلید_Base* p1 = static_cast<ثابت Cache_کلید_Base*>(key1.pointer);
    ثابت Cache_کلید_Base* p2 = static_cast<ثابت Cache_کلید_Base*>(key2.pointer);
    بازگشت *p1 == *p2;
}

تهی U_EXPORT2
ucache_delete_کلید(تهی *obj) {
    Cache_کلید_Base* p = static_cast<Cache_کلید_Base*>(obj);
    delete p;
}

Cache_کلید_Base::~Cache_کلید_Base() {
}

ایستا تهی U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_مقدار_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = جدید UnifiedCache(status);
    اگر (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    اگر (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        بازگشت;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    اگر (U_FAILURE(status)) {
        بازگشت nullptr;
    }
    U_ASSERT(gCache != nullptr);
    بازگشت gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_رقم_مقادیر_Total(0),
        f_رقم_مقادیر_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_شمار(0),
        f_No_مقدار(nullptr) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    f_No_مقدار = جدید SharedObject();
    اگر (f_No_مقدار == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    f_No_مقدار->soft_Ref_شمار = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_مقدار->hard_Ref_شمار = 1;  // when other references to it are removed.
    f_No_مقدار->cachePtr = این;

    fHashtable = uhash_open(
            &ucache_hash_کلیدها,
            &ucache_compare_کلیدها,
            nullptr,
            &status);
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    uhash_set_کلید_Deleter(fHashtable, &ucache_delete_کلید);
}

تهی UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_اقلام, UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    اگر (count < 0 || percentage_Of_In_Use_اقلام < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_اقلام;
}

int32_t UnifiedCache::unused_شمار() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    بازگشت uhash_شمار(fHashtable) - f_رقم_مقادیر_In_Use;
}

int64_t UnifiedCache::auto_Evicted_شمار() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    بازگشت f_Auto_Evicted_شمار;
}

int32_t UnifiedCache::کلید_شمار() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    بازگشت uhash_شمار(fHashtable);
}

تهی UnifiedCache::flush() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    وقتی (_flush(نادرست));
}

تهی UnifiedCache::handleUnreferencedObject() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_رقم_مقادیر_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#شامل <stdio.h>

تهی UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    ثابت UnifiedCache *cache = getInstance(status);
    اگر (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        بازگشت;
    }
    cache->dumpContents();
}

تهی UnifiedCache::dumpContents() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
تهی UnifiedCache::_dumpContents() ثابت {
    int32_t pos = UHASH_FIRST;
    ثابت UHashElement *element = uhash_nextElement(fHashtable, &pos);
    نویسه buffer[256];
    int32_t cnt = 0;
    برای (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        ثابت SharedObject *sharedObject =
                (ثابت SharedObject *) element->value.pointer;
        ثابت Cache_کلید_Base *کلید =
                (ثابت Cache_کلید_Base *) element->کلید.pointer;
        اگر (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    کلید->writeDescription(buffer, 256),
                    کلید->creationStatus,
                    sharedObject == f_No_مقدار ? nullptr :sharedObject,
                    sharedObject->get_Ref_شمار(),
                    sharedObject->get_Soft_Ref_شمار());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_شمار(fHashtable));
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
        _flush(درست);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete f_No_مقدار;
    f_No_مقدار = nullptr;
}

ثابت UHashElement *
UnifiedCache::_nextElement() ثابت {
    ثابت UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    اگر (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        بازگشت uhash_nextElement(fHashtable, &fEvictPos);
    }
    بازگشت element;
}

UBool UnifiedCache::_flush(UBool all) ثابت {
    UBool result = نادرست;
    int32_t origSize = uhash_شمار(fHashtable);
    برای (int32_t i = 0; i < origSize; ++i) {
        ثابت UHashElement *element = _nextElement();
        اگر (element == nullptr) {
            بشکن;
        }
        اگر (all || _isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == این);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = درست;
        }
    }
    بازگشت result;
}

int32_t UnifiedCache::compute_شمار_Of_اقلام_To_Evict() ثابت {
    int32_t total_اقلام = uhash_شمار(fHashtable);
    int32_t evictable_اقلام = total_اقلام - f_رقم_مقادیر_In_Use;

    int32_t unusedLimitByPercentage = f_رقم_مقادیر_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t شمار_Of_اقلام_To_Evict = std::max<int32_t>(0, evictable_اقلام - unusedLimit);
    بازگشت شمار_Of_اقلام_To_Evict;
}

تهی UnifiedCache::_runEvictionSlice() ثابت {
    int32_t max_اقلام_To_Evict = compute_شمار_Of_اقلام_To_Evict();
    اگر (max_اقلام_To_Evict <= 0) {
        بازگشت;
    }
    برای (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ثابت UHashElement *element = _nextElement();
        اگر (element == nullptr) {
            بشکن;
        }
        اگر (_isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_شمار;
            اگر (--max_اقلام_To_Evict == 0) {
                بشکن;
            }
        }
    }
}

تهی UnifiedCache::_putNew(
        ثابت Cache_کلید_Base &کلید,
        ثابت SharedObject *value,
        ثابت UErrorCode creationStatus,
        UErrorCode &status) ثابت {
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    Cache_کلید_Base *کلید_To_Adopt = کلید.clone();
    اگر (کلید_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    کلید_To_Adopt->fCreationStatus = creationStatus;
    اگر (value->soft_Ref_شمار == 0) {
        _registerPrimary(کلید_To_Adopt, value);
    }
    تهی *old_مقدار = uhash_put(fHashtable, کلید_To_Adopt, (تهی *) value, &status);
    U_ASSERT(old_مقدار == nullptr);
    (تهی)old_مقدار;
    اگر (U_SUCCESS(status)) {
        value->soft_Ref_شمار++;
    }
}

تهی UnifiedCache::_putIfAbsentAndGet(
        ثابت Cache_کلید_Base &کلید,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ثابت UHashElement *element = uhash_find(fHashtable, &کلید);
    اگر (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        بازگشت;
    }
    اگر (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(کلید, value, status, putError);
    } وگرنه {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        ثابت Cache_کلید_Base &کلید,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    ثابت UHashElement *element = uhash_find(fHashtable, &کلید);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     وقتی (element != nullptr && _inProgress(element)) {
         g_In_Progress_مقدار_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &کلید);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    اگر (element != nullptr) {
         _fetch(element, value, status);
        بازگشت درست;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(کلید, f_No_مقدار, U_ZERO_ERROR, status);
    بازگشت نادرست;
}

تهی UnifiedCache::_get(
        ثابت Cache_کلید_Base &کلید,
        ثابت SharedObject *&value,
        ثابت تهی *creationContext,
        UErrorCode &status) ثابت {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    اگر (_poll(کلید, value, status)) {
        اگر (value == f_No_مقدار) {
            SharedObject::clearPtr(value);
        }
        بازگشت;
    }
    اگر (U_FAILURE(status)) {
        بازگشت;
    }
    value = کلید.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    اگر (value == nullptr) {
        SharedObject::copyPtr(f_No_مقدار, value);
    }
    _putIfAbsentAndGet(کلید, value, status);
    اگر (value == f_No_مقدار) {
        SharedObject::clearPtr(value);
    }
}

تهی UnifiedCache::_registerPrimary(
            ثابت Cache_کلید_Base *the_کلید, ثابت SharedObject *value) ثابت {
    the_کلید->fIsPrimary = درست;
    value->cachePtr = این;
    ++f_رقم_مقادیر_Total;
    ++f_رقم_مقادیر_In_Use;
}

تهی UnifiedCache::_put(
        ثابت UHashElement *element,
        ثابت SharedObject *value,
        ثابت UErrorCode status) ثابت {
    U_ASSERT(_inProgress(element));
    ثابت Cache_کلید_Base* the_کلید = static_cast<ثابت Cache_کلید_Base*>(element->کلید.pointer);
    ثابت SharedObject* old_مقدار = static_cast<ثابت SharedObject*>(element->value.pointer);
    the_کلید->fCreationStatus = status;
    اگر (value->soft_Ref_شمار == 0) {
        _registerPrimary(the_کلید, value);
    }
    value->soft_Ref_شمار++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (تهی *) value;
    U_ASSERT(old_مقدار == f_No_مقدار);
    removeSoftRef(old_مقدار);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_مقدار_Added_Cond->notify_all();
}

تهی UnifiedCache::_fetch(
        ثابت UHashElement *element,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    ثابت Cache_کلید_Base* the_کلید = static_cast<ثابت Cache_کلید_Base*>(element->کلید.pointer);
    status = the_کلید->fCreationStatus;

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
    بازگشت result;
}

UBool UnifiedCache::_inProgress(
        ثابت SharedObject* the_مقدار, UErrorCode creationStatus) ثابت {
    بازگشت (the_مقدار == f_No_مقدار && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ثابت UHashElement *element) ثابت
{
    ثابت Cache_کلید_Base* the_کلید = static_cast<ثابت Cache_کلید_Base*>(element->کلید.pointer);
    ثابت SharedObject* the_مقدار = static_cast<ثابت SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    اگر (_inProgress(the_مقدار, the_کلید->fCreationStatus)) {
        بازگشت نادرست;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    بازگشت (!the_کلید->fIsPrimary || (the_مقدار->soft_Ref_شمار == 1 && the_مقدار->noHardReferences()));
}

تهی UnifiedCache::removeSoftRef(ثابت SharedObject *value) ثابت {
    U_ASSERT(value->cachePtr == این);
    U_ASSERT(value->soft_Ref_شمار > 0);
    اگر (--value->soft_Ref_شمار == 0) {
        --f_رقم_مقادیر_Total;
        اگر (value->noHardReferences()) {
            delete value;
        } وگرنه {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ثابت SharedObject *value) ثابت {
    صحیح ref_شمار = 0;
    اگر (value) {
        ref_شمار = umtx_atomic_dec(&value->hard_Ref_شمار);
        U_ASSERT(ref_شمار >= 0);
        اگر (ref_شمار == 0) {
            --f_رقم_مقادیر_In_Use;
        }
    }
    بازگشت ref_شمار;
}

int32_t UnifiedCache::addHardRef(ثابت SharedObject *value) ثابت {
    صحیح ref_شمار = 0;
    اگر (value) {
        ref_شمار = umtx_atomic_inc(&value->hard_Ref_شمار);
        U_ASSERT(ref_شمار >= 1);
        اگر (ref_شمار == 1) {
            f_رقم_مقادیر_In_Use++;
        }
    }
    بازگشت ref_شمار;
}

U_NAMESPACE_END

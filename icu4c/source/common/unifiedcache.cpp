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
ساكن std::condition_variable *g_In_Progress_درج_Added_Cond;
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
    g_In_Progress_درج_Added_Cond->~condition_variable();
    g_In_Progress_درج_Added_Cond = nullptr;
    أرجع true;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_مبوداي(ثابت UHashTok مبودي) {
    ثابت Cache_مبودي_Base* ckey = static_cast<ثابت Cache_مبودي_Base*>(مبودي.pointer);
    أرجع ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_مبوداي(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت Cache_مبودي_Base* p1 = static_cast<ثابت Cache_مبودي_Base*>(key1.pointer);
    ثابت Cache_مبودي_Base* p2 = static_cast<ثابت Cache_مبودي_Base*>(key2.pointer);
    أرجع *p1 == *p2;
}

فراغ U_EXPORT2
ucache_delete_مبودي(فراغ *obj) {
    Cache_مبودي_Base* p = static_cast<Cache_مبودي_Base*>(obj);
    delete p;
}

Cache_مبودي_Base::~Cache_مبودي_Base() {
}

ساكن فراغ U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_درج_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = جديد UnifiedCache(status);
    إذا (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    إذا (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        أرجع;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    إذا (U_FAILURE(status)) {
        أرجع nullptr;
    }
    U_ASSERT(gCache != nullptr);
    أرجع gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_لمب_درجوجي_Total(0),
        f_لمب_درجوجي_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_كدي(0),
        f_No_درج(nullptr) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    f_No_درج = جديد SharedObject();
    إذا (f_No_درج == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    f_No_درج->soft_Ref_كدي = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_درج->hard_Ref_كدي = 1;  // when other references to it are removed.
    f_No_درج->cachePtr = هذا;

    fHashtable = uhash_open(
            &ucache_hash_مبوداي,
            &ucache_compare_مبوداي,
            nullptr,
            &status);
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    uhash_set_مبودي_Deleter(fHashtable, &ucache_delete_مبودي);
}

فراغ UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_ابوبو, UErrorCode &status) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    إذا (count < 0 || percentage_Of_In_Use_ابوبو < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_ابوبو;
}

int32_t UnifiedCache::unused_كدي() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    أرجع uhash_كدي(fHashtable) - f_لمب_درجوجي_In_Use;
}

int64_t UnifiedCache::auto_Evicted_كدي() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    أرجع f_Auto_Evicted_كدي;
}

int32_t UnifiedCache::مبودي_كدي() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    أرجع uhash_كدي(fHashtable);
}

فراغ UnifiedCache::flush() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    طالما (_flush(خطأ));
}

فراغ UnifiedCache::handleUnreferencedObject() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_لمب_درجوجي_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#ضمن <stdio.h>

فراغ UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    ثابت UnifiedCache *cache = getInstance(status);
    إذا (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        أرجع;
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
    لكل (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        ثابت SharedObject *sharedObject =
                (ثابت SharedObject *) element->value.pointer;
        ثابت Cache_مبودي_Base *مبودي =
                (ثابت Cache_مبودي_Base *) element->مبودي.pointer;
        إذا (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    مبودي->writeDescription(buffer, 256),
                    مبودي->creationStatus,
                    sharedObject == f_No_درج ? nullptr :sharedObject,
                    sharedObject->get_Ref_كدي(),
                    sharedObject->get_Soft_Ref_كدي());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_كدي(fHashtable));
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
    delete f_No_درج;
    f_No_درج = nullptr;
}

ثابت UHashElement *
UnifiedCache::_nextElement() ثابت {
    ثابت UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    إذا (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        أرجع uhash_nextElement(fHashtable, &fEvictPos);
    }
    أرجع element;
}

UBool UnifiedCache::_flush(UBool all) ثابت {
    UBool result = خطأ;
    int32_t origSize = uhash_كدي(fHashtable);
    لكل (int32_t i = 0; i < origSize; ++i) {
        ثابت UHashElement *element = _nextElement();
        إذا (element == nullptr) {
            اكسر;
        }
        إذا (all || _isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == هذا);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = true;
        }
    }
    أرجع result;
}

int32_t UnifiedCache::compute_كدي_Of_ابوبو_To_Evict() ثابت {
    int32_t total_ابوبو = uhash_كدي(fHashtable);
    int32_t evictable_ابوبو = total_ابوبو - f_لمب_درجوجي_In_Use;

    int32_t unusedLimitByPercentage = f_لمب_درجوجي_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t كدي_Of_ابوبو_To_Evict = std::max<int32_t>(0, evictable_ابوبو - unusedLimit);
    أرجع كدي_Of_ابوبو_To_Evict;
}

فراغ UnifiedCache::_runEvictionSlice() ثابت {
    int32_t max_ابوبو_To_Evict = compute_كدي_Of_ابوبو_To_Evict();
    إذا (max_ابوبو_To_Evict <= 0) {
        أرجع;
    }
    لكل (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ثابت UHashElement *element = _nextElement();
        إذا (element == nullptr) {
            اكسر;
        }
        إذا (_isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_كدي;
            إذا (--max_ابوبو_To_Evict == 0) {
                اكسر;
            }
        }
    }
}

فراغ UnifiedCache::_putNew(
        ثابت Cache_مبودي_Base &مبودي,
        ثابت SharedObject *value,
        ثابت UErrorCode creationStatus,
        UErrorCode &status) ثابت {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    Cache_مبودي_Base *مبودي_To_Adopt = مبودي.clone();
    إذا (مبودي_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    مبودي_To_Adopt->fCreationStatus = creationStatus;
    إذا (value->soft_Ref_كدي == 0) {
        _registerPrimary(مبودي_To_Adopt, value);
    }
    فراغ *old_درج = uhash_put(fHashtable, مبودي_To_Adopt, (فراغ *) value, &status);
    U_ASSERT(old_درج == nullptr);
    (فراغ)old_درج;
    إذا (U_SUCCESS(status)) {
        value->soft_Ref_كدي++;
    }
}

فراغ UnifiedCache::_putIfAbsentAndGet(
        ثابت Cache_مبودي_Base &مبودي,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ثابت UHashElement *element = uhash_find(fHashtable, &مبودي);
    إذا (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        أرجع;
    }
    إذا (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(مبودي, value, status, putError);
    } وإلا {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        ثابت Cache_مبودي_Base &مبودي,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    ثابت UHashElement *element = uhash_find(fHashtable, &مبودي);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     طالما (element != nullptr && _inProgress(element)) {
         g_In_Progress_درج_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &مبودي);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    إذا (element != nullptr) {
         _fetch(element, value, status);
        أرجع true;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(مبودي, f_No_درج, U_ZERO_ERROR, status);
    أرجع خطأ;
}

فراغ UnifiedCache::_get(
        ثابت Cache_مبودي_Base &مبودي,
        ثابت SharedObject *&value,
        ثابت فراغ *creationContext,
        UErrorCode &status) ثابت {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    إذا (_poll(مبودي, value, status)) {
        إذا (value == f_No_درج) {
            SharedObject::clearPtr(value);
        }
        أرجع;
    }
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    value = مبودي.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    إذا (value == nullptr) {
        SharedObject::copyPtr(f_No_درج, value);
    }
    _putIfAbsentAndGet(مبودي, value, status);
    إذا (value == f_No_درج) {
        SharedObject::clearPtr(value);
    }
}

فراغ UnifiedCache::_registerPrimary(
            ثابت Cache_مبودي_Base *the_مبودي, ثابت SharedObject *value) ثابت {
    the_مبودي->fIsPrimary = true;
    value->cachePtr = هذا;
    ++f_لمب_درجوجي_Total;
    ++f_لمب_درجوجي_In_Use;
}

فراغ UnifiedCache::_put(
        ثابت UHashElement *element,
        ثابت SharedObject *value,
        ثابت UErrorCode status) ثابت {
    U_ASSERT(_inProgress(element));
    ثابت Cache_مبودي_Base* the_مبودي = static_cast<ثابت Cache_مبودي_Base*>(element->مبودي.pointer);
    ثابت SharedObject* old_درج = static_cast<ثابت SharedObject*>(element->value.pointer);
    the_مبودي->fCreationStatus = status;
    إذا (value->soft_Ref_كدي == 0) {
        _registerPrimary(the_مبودي, value);
    }
    value->soft_Ref_كدي++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (فراغ *) value;
    U_ASSERT(old_درج == f_No_درج);
    removeSoftRef(old_درج);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_درج_Added_Cond->notify_all();
}

فراغ UnifiedCache::_fetch(
        ثابت UHashElement *element,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    ثابت Cache_مبودي_Base* the_مبودي = static_cast<ثابت Cache_مبودي_Base*>(element->مبودي.pointer);
    status = the_مبودي->fCreationStatus;

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
    أرجع result;
}

UBool UnifiedCache::_inProgress(
        ثابت SharedObject* the_درج, UErrorCode creationStatus) ثابت {
    أرجع (the_درج == f_No_درج && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ثابت UHashElement *element) ثابت
{
    ثابت Cache_مبودي_Base* the_مبودي = static_cast<ثابت Cache_مبودي_Base*>(element->مبودي.pointer);
    ثابت SharedObject* the_درج = static_cast<ثابت SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    إذا (_inProgress(the_درج, the_مبودي->fCreationStatus)) {
        أرجع خطأ;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    أرجع (!the_مبودي->fIsPrimary || (the_درج->soft_Ref_كدي == 1 && the_درج->noHardReferences()));
}

فراغ UnifiedCache::removeSoftRef(ثابت SharedObject *value) ثابت {
    U_ASSERT(value->cachePtr == هذا);
    U_ASSERT(value->soft_Ref_كدي > 0);
    إذا (--value->soft_Ref_كدي == 0) {
        --f_لمب_درجوجي_Total;
        إذا (value->noHardReferences()) {
            delete value;
        } وإلا {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ثابت SharedObject *value) ثابت {
    صحيح ref_كدي = 0;
    إذا (value) {
        ref_كدي = umtx_atomic_dec(&value->hard_Ref_كدي);
        U_ASSERT(ref_كدي >= 0);
        إذا (ref_كدي == 0) {
            --f_لمب_درجوجي_In_Use;
        }
    }
    أرجع ref_كدي;
}

int32_t UnifiedCache::addHardRef(ثابت SharedObject *value) ثابت {
    صحيح ref_كدي = 0;
    إذا (value) {
        ref_كدي = umtx_atomic_inc(&value->hard_Ref_كدي);
        U_ASSERT(ref_كدي >= 1);
        إذا (ref_كدي == 1) {
            f_لمب_درجوجي_In_Use++;
        }
    }
    أرجع ref_كدي;
}

U_NAMESPACE_END

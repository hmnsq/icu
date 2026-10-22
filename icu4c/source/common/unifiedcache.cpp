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
ساكن std::condition_variable *g_In_Progress_قيمة_Added_Cond;
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
    g_In_Progress_قيمة_Added_Cond->~condition_variable();
    g_In_Progress_قيمة_Added_Cond = nullptr;
    أرجع true;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_مفاتيح(ثابت UHashTok مفتاح) {
    ثابت Cache_مفتاح_Base* ckey = static_cast<ثابت Cache_مفتاح_Base*>(مفتاح.pointer);
    أرجع ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_مفاتيح(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت Cache_مفتاح_Base* p1 = static_cast<ثابت Cache_مفتاح_Base*>(key1.pointer);
    ثابت Cache_مفتاح_Base* p2 = static_cast<ثابت Cache_مفتاح_Base*>(key2.pointer);
    أرجع *p1 == *p2;
}

فراغ U_EXPORT2
ucache_delete_مفتاح(فراغ *obj) {
    Cache_مفتاح_Base* p = static_cast<Cache_مفتاح_Base*>(obj);
    delete p;
}

Cache_مفتاح_Base::~Cache_مفتاح_Base() {
}

ساكن فراغ U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_قيمة_Added_Cond = STATIC_NEW(std::condition_variable);
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
        f_رقم_قيم_Total(0),
        f_رقم_قيم_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_عدد(0),
        f_No_قيمة(nullptr) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    f_No_قيمة = جديد SharedObject();
    إذا (f_No_قيمة == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    f_No_قيمة->soft_Ref_عدد = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_قيمة->hard_Ref_عدد = 1;  // when other references to it are removed.
    f_No_قيمة->cachePtr = هذا;

    fHashtable = uhash_open(
            &ucache_hash_مفاتيح,
            &ucache_compare_مفاتيح,
            nullptr,
            &status);
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    uhash_set_مفتاح_Deleter(fHashtable, &ucache_delete_مفتاح);
}

فراغ UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_عناصر, UErrorCode &status) {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    إذا (count < 0 || percentage_Of_In_Use_عناصر < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_عناصر;
}

int32_t UnifiedCache::unused_عدد() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    أرجع uhash_عدد(fHashtable) - f_رقم_قيم_In_Use;
}

int64_t UnifiedCache::auto_Evicted_عدد() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    أرجع f_Auto_Evicted_عدد;
}

int32_t UnifiedCache::مفتاح_عدد() ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    أرجع uhash_عدد(fHashtable);
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
    --f_رقم_قيم_In_Use;
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
        ثابت Cache_مفتاح_Base *مفتاح =
                (ثابت Cache_مفتاح_Base *) element->مفتاح.pointer;
        إذا (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    مفتاح->writeDescription(buffer, 256),
                    مفتاح->creationStatus,
                    sharedObject == f_No_قيمة ? nullptr :sharedObject,
                    sharedObject->get_Ref_عدد(),
                    sharedObject->get_Soft_Ref_عدد());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_عدد(fHashtable));
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
    delete f_No_قيمة;
    f_No_قيمة = nullptr;
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
    int32_t origSize = uhash_عدد(fHashtable);
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

int32_t UnifiedCache::compute_عدد_Of_عناصر_To_Evict() ثابت {
    int32_t total_عناصر = uhash_عدد(fHashtable);
    int32_t evictable_عناصر = total_عناصر - f_رقم_قيم_In_Use;

    int32_t unusedLimitByPercentage = f_رقم_قيم_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t عدد_Of_عناصر_To_Evict = std::max<int32_t>(0, evictable_عناصر - unusedLimit);
    أرجع عدد_Of_عناصر_To_Evict;
}

فراغ UnifiedCache::_runEvictionSlice() ثابت {
    int32_t max_عناصر_To_Evict = compute_عدد_Of_عناصر_To_Evict();
    إذا (max_عناصر_To_Evict <= 0) {
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
            ++f_Auto_Evicted_عدد;
            إذا (--max_عناصر_To_Evict == 0) {
                اكسر;
            }
        }
    }
}

فراغ UnifiedCache::_putNew(
        ثابت Cache_مفتاح_Base &مفتاح,
        ثابت SharedObject *value,
        ثابت UErrorCode creationStatus,
        UErrorCode &status) ثابت {
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    Cache_مفتاح_Base *مفتاح_To_Adopt = مفتاح.clone();
    إذا (مفتاح_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    مفتاح_To_Adopt->fCreationStatus = creationStatus;
    إذا (value->soft_Ref_عدد == 0) {
        _registerPrimary(مفتاح_To_Adopt, value);
    }
    فراغ *old_قيمة = uhash_put(fHashtable, مفتاح_To_Adopt, (فراغ *) value, &status);
    U_ASSERT(old_قيمة == nullptr);
    (فراغ)old_قيمة;
    إذا (U_SUCCESS(status)) {
        value->soft_Ref_عدد++;
    }
}

فراغ UnifiedCache::_putIfAbsentAndGet(
        ثابت Cache_مفتاح_Base &مفتاح,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ثابت UHashElement *element = uhash_find(fHashtable, &مفتاح);
    إذا (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        أرجع;
    }
    إذا (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(مفتاح, value, status, putError);
    } وإلا {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        ثابت Cache_مفتاح_Base &مفتاح,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    ثابت UHashElement *element = uhash_find(fHashtable, &مفتاح);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     طالما (element != nullptr && _inProgress(element)) {
         g_In_Progress_قيمة_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &مفتاح);
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
    _putNew(مفتاح, f_No_قيمة, U_ZERO_ERROR, status);
    أرجع خطأ;
}

فراغ UnifiedCache::_get(
        ثابت Cache_مفتاح_Base &مفتاح,
        ثابت SharedObject *&value,
        ثابت فراغ *creationContext,
        UErrorCode &status) ثابت {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    إذا (_poll(مفتاح, value, status)) {
        إذا (value == f_No_قيمة) {
            SharedObject::clearPtr(value);
        }
        أرجع;
    }
    إذا (U_FAILURE(status)) {
        أرجع;
    }
    value = مفتاح.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    إذا (value == nullptr) {
        SharedObject::copyPtr(f_No_قيمة, value);
    }
    _putIfAbsentAndGet(مفتاح, value, status);
    إذا (value == f_No_قيمة) {
        SharedObject::clearPtr(value);
    }
}

فراغ UnifiedCache::_registerPrimary(
            ثابت Cache_مفتاح_Base *the_مفتاح, ثابت SharedObject *value) ثابت {
    the_مفتاح->fIsPrimary = true;
    value->cachePtr = هذا;
    ++f_رقم_قيم_Total;
    ++f_رقم_قيم_In_Use;
}

فراغ UnifiedCache::_put(
        ثابت UHashElement *element,
        ثابت SharedObject *value,
        ثابت UErrorCode status) ثابت {
    U_ASSERT(_inProgress(element));
    ثابت Cache_مفتاح_Base* the_مفتاح = static_cast<ثابت Cache_مفتاح_Base*>(element->مفتاح.pointer);
    ثابت SharedObject* old_قيمة = static_cast<ثابت SharedObject*>(element->value.pointer);
    the_مفتاح->fCreationStatus = status;
    إذا (value->soft_Ref_عدد == 0) {
        _registerPrimary(the_مفتاح, value);
    }
    value->soft_Ref_عدد++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (فراغ *) value;
    U_ASSERT(old_قيمة == f_No_قيمة);
    removeSoftRef(old_قيمة);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_قيمة_Added_Cond->notify_all();
}

فراغ UnifiedCache::_fetch(
        ثابت UHashElement *element,
        ثابت SharedObject *&value,
        UErrorCode &status) ثابت {
    ثابت Cache_مفتاح_Base* the_مفتاح = static_cast<ثابت Cache_مفتاح_Base*>(element->مفتاح.pointer);
    status = the_مفتاح->fCreationStatus;

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
        ثابت SharedObject* the_قيمة, UErrorCode creationStatus) ثابت {
    أرجع (the_قيمة == f_No_قيمة && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ثابت UHashElement *element) ثابت
{
    ثابت Cache_مفتاح_Base* the_مفتاح = static_cast<ثابت Cache_مفتاح_Base*>(element->مفتاح.pointer);
    ثابت SharedObject* the_قيمة = static_cast<ثابت SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    إذا (_inProgress(the_قيمة, the_مفتاح->fCreationStatus)) {
        أرجع خطأ;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    أرجع (!the_مفتاح->fIsPrimary || (the_قيمة->soft_Ref_عدد == 1 && the_قيمة->noHardReferences()));
}

فراغ UnifiedCache::removeSoftRef(ثابت SharedObject *value) ثابت {
    U_ASSERT(value->cachePtr == هذا);
    U_ASSERT(value->soft_Ref_عدد > 0);
    إذا (--value->soft_Ref_عدد == 0) {
        --f_رقم_قيم_Total;
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
    صحيح ref_عدد = 0;
    إذا (value) {
        ref_عدد = umtx_atomic_dec(&value->hard_Ref_عدد);
        U_ASSERT(ref_عدد >= 0);
        إذا (ref_عدد == 0) {
            --f_رقم_قيم_In_Use;
        }
    }
    أرجع ref_عدد;
}

int32_t UnifiedCache::addHardRef(ثابت SharedObject *value) ثابت {
    صحيح ref_عدد = 0;
    إذا (value) {
        ref_عدد = umtx_atomic_inc(&value->hard_Ref_عدد);
        U_ASSERT(ref_عدد >= 1);
        إذا (ref_عدد == 1) {
            f_رقم_قيم_In_Use++;
        }
    }
    أرجع ref_عدد;
}

U_NAMESPACE_END

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

#অন্তর্ভাব "unifiedcache.h"

#অন্তর্ভাব <algorithm>      // For std::max()
#অন্তর্ভাব <mutex>

#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "ucln_cmn.h"

স্থির icu::UnifiedCache *gCache = nullptr;
স্থির std::mutex *gCacheMutex = nullptr;
স্থির std::condition_variable *g_In_Progress_মান_Added_Cond;
স্থির icu::UInitOnce gCacheInitOnce {};

স্থির ধ্রুবক int32_t MAX_EVICT_ITERATIONS = 10;
স্থির ধ্রুবক int32_t DEFAULT_MAX_UNUSED = 1000;
স্থির ধ্রুবক int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
স্থির UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_মান_Added_Cond->~condition_variable();
    g_In_Progress_মান_Added_Cond = nullptr;
    ফেরত সত্য;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_চাবিগুলি(ধ্রুবক UHashTok চাবি) {
    ধ্রুবক Cache_চাবি_Base* ckey = static_cast<ধ্রুবক Cache_চাবি_Base*>(চাবি.pointer);
    ফেরত ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_চাবিগুলি(ধ্রুবক UHashTok key1, ধ্রুবক UHashTok key2) {
    ধ্রুবক Cache_চাবি_Base* p1 = static_cast<ধ্রুবক Cache_চাবি_Base*>(key1.pointer);
    ধ্রুবক Cache_চাবি_Base* p2 = static_cast<ধ্রুবক Cache_চাবি_Base*>(key2.pointer);
    ফেরত *p1 == *p2;
}

শূন্য U_EXPORT2
ucache_delete_চাবি(শূন্য *obj) {
    Cache_চাবি_Base* p = static_cast<Cache_চাবি_Base*>(obj);
    delete p;
}

Cache_চাবি_Base::~Cache_চাবি_Base() {
}

স্থির শূন্য U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_মান_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = নতুন UnifiedCache(status);
    যদি (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    যদি (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        ফেরত;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }
    U_ASSERT(gCache != nullptr);
    ফেরত gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_অঙ্ক_মানসমূহ_যোগ(0),
        f_অঙ্ক_মানসমূহ_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_গুনতি(0),
        f_No_মান(nullptr) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    f_No_মান = নতুন SharedObject();
    যদি (f_No_মান == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    f_No_মান->soft_Ref_গুনতি = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_মান->hard_Ref_গুনতি = 1;  // when other references to it are removed.
    f_No_মান->cachePtr = এই;

    fHashtable = uhash_open(
            &ucache_hash_চাবিগুলি,
            &ucache_compare_চাবিগুলি,
            nullptr,
            &status);
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    uhash_set_চাবি_Deleter(fHashtable, &ucache_delete_চাবি);
}

শূন্য UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_বস্তুসমূহ, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    যদি (count < 0 || percentage_Of_In_Use_বস্তুসমূহ < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_বস্তুসমূহ;
}

int32_t UnifiedCache::unused_গুনতি() ধ্রুবক {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ফেরত uhash_গুনতি(fHashtable) - f_অঙ্ক_মানসমূহ_In_Use;
}

int64_t UnifiedCache::auto_Evicted_গুনতি() ধ্রুবক {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ফেরত f_Auto_Evicted_গুনতি;
}

int32_t UnifiedCache::চাবি_গুনতি() ধ্রুবক {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ফেরত uhash_গুনতি(fHashtable);
}

শূন্য UnifiedCache::flush() ধ্রুবক {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    যখন (_flush(মিথ্যা));
}

শূন্য UnifiedCache::handleUnreferencedObject() ধ্রুবক {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_অঙ্ক_মানসমূহ_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#অন্তর্ভাব <stdio.h>

শূন্য UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    ধ্রুবক UnifiedCache *cache = getInstance(status);
    যদি (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        ফেরত;
    }
    cache->dumpContents();
}

শূন্য UnifiedCache::dumpContents() ধ্রুবক {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
শূন্য UnifiedCache::_dumpContents() ধ্রুবক {
    int32_t pos = UHASH_FIRST;
    ধ্রুবক UHashElement *element = uhash_পরবর্তী_Element(fHashtable, &pos);
    অক্ষর বাফার[256];
    int32_t cnt = 0;
    জন্য (; element != nullptr; element = uhash_পরবর্তী_Element(fHashtable, &pos)) {
        ধ্রুবক SharedObject *sharedObject =
                (ধ্রুবক SharedObject *) element->value.pointer;
        ধ্রুবক Cache_চাবি_Base *চাবি =
                (ধ্রুবক Cache_চাবি_Base *) element->চাবি.pointer;
        যদি (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    চাবি->writeDescription(বাফার, 256),
                    চাবি->creationStatus,
                    sharedObject == f_No_মান ? nullptr :sharedObject,
                    sharedObject->get_Ref_গুনতি(),
                    sharedObject->get_Soft_Ref_গুনতি());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_গুনতি(fHashtable));
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
        _flush(সত্য);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete f_No_মান;
    f_No_মান = nullptr;
}

ধ্রুবক UHashElement *
UnifiedCache::পরবর্তী_Element_2() ধ্রুবক {
    ধ্রুবক UHashElement *element = uhash_পরবর্তী_Element(fHashtable, &fEvictPos);
    যদি (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        ফেরত uhash_পরবর্তী_Element(fHashtable, &fEvictPos);
    }
    ফেরত element;
}

UBool UnifiedCache::_flush(UBool all) ধ্রুবক {
    UBool ফলাফল = মিথ্যা;
    int32_t orig_আকার = uhash_গুনতি(fHashtable);
    জন্য (int32_t i = 0; i < orig_আকার; ++i) {
        ধ্রুবক UHashElement *element = পরবর্তী_Element_2();
        যদি (element == nullptr) {
            বিরতি;
        }
        যদি (all || _isEvictable(element)) {
            ধ্রুবক SharedObject *sharedObject =
                    static_cast<ধ্রুবক SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == এই);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            ফলাফল = সত্য;
        }
    }
    ফেরত ফলাফল;
}

int32_t UnifiedCache::compute_গুনতি_Of_বস্তুসমূহ_To_Evict() ধ্রুবক {
    int32_t যোগ_বস্তুসমূহ = uhash_গুনতি(fHashtable);
    int32_t evictable_বস্তুসমূহ = যোগ_বস্তুসমূহ - f_অঙ্ক_মানসমূহ_In_Use;

    int32_t unusedLimitByPercentage = f_অঙ্ক_মানসমূহ_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t গুনতি_Of_বস্তুসমূহ_To_Evict = std::max<int32_t>(0, evictable_বস্তুসমূহ - unusedLimit);
    ফেরত গুনতি_Of_বস্তুসমূহ_To_Evict;
}

শূন্য UnifiedCache::_runEvictionSlice() ধ্রুবক {
    int32_t max_বস্তুসমূহ_To_Evict = compute_গুনতি_Of_বস্তুসমূহ_To_Evict();
    যদি (max_বস্তুসমূহ_To_Evict <= 0) {
        ফেরত;
    }
    জন্য (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ধ্রুবক UHashElement *element = পরবর্তী_Element_2();
        যদি (element == nullptr) {
            বিরতি;
        }
        যদি (_isEvictable(element)) {
            ধ্রুবক SharedObject *sharedObject =
                    static_cast<ধ্রুবক SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_গুনতি;
            যদি (--max_বস্তুসমূহ_To_Evict == 0) {
                বিরতি;
            }
        }
    }
}

শূন্য UnifiedCache::_putNew(
        ধ্রুবক Cache_চাবি_Base &চাবি,
        ধ্রুবক SharedObject *value,
        ধ্রুবক UErrorCode creationStatus,
        UErrorCode &status) ধ্রুবক {
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    Cache_চাবি_Base *চাবি_To_Adopt = চাবি.clone();
    যদি (চাবি_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    চাবি_To_Adopt->fCreationStatus = creationStatus;
    যদি (value->soft_Ref_গুনতি == 0) {
        _registerPrimary(চাবি_To_Adopt, value);
    }
    শূন্য *old_মান = uhash_put(fHashtable, চাবি_To_Adopt, (শূন্য *) value, &status);
    U_ASSERT(old_মান == nullptr);
    (শূন্য)old_মান;
    যদি (U_SUCCESS(status)) {
        value->soft_Ref_গুনতি++;
    }
}

শূন্য UnifiedCache::_putIfAbsentAndGet(
        ধ্রুবক Cache_চাবি_Base &চাবি,
        ধ্রুবক SharedObject *&value,
        UErrorCode &status) ধ্রুবক {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ধ্রুবক UHashElement *element = uhash_find(fHashtable, &চাবি);
    যদি (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        ফেরত;
    }
    যদি (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(চাবি, value, status, putError);
    } অন্যথা {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        ধ্রুবক Cache_চাবি_Base &চাবি,
        ধ্রুবক SharedObject *&value,
        UErrorCode &status) ধ্রুবক {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    ধ্রুবক UHashElement *element = uhash_find(fHashtable, &চাবি);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     যখন (element != nullptr && _inProgress(element)) {
         g_In_Progress_মান_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &চাবি);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    যদি (element != nullptr) {
         _fetch(element, value, status);
        ফেরত সত্য;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(চাবি, f_No_মান, U_ZERO_ERROR, status);
    ফেরত মিথ্যা;
}

শূন্য UnifiedCache::_get(
        ধ্রুবক Cache_চাবি_Base &চাবি,
        ধ্রুবক SharedObject *&value,
        ধ্রুবক শূন্য *creationContext,
        UErrorCode &status) ধ্রুবক {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    যদি (_poll(চাবি, value, status)) {
        যদি (value == f_No_মান) {
            SharedObject::clearPtr(value);
        }
        ফেরত;
    }
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    value = চাবি.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    যদি (value == nullptr) {
        SharedObject::copyPtr(f_No_মান, value);
    }
    _putIfAbsentAndGet(চাবি, value, status);
    যদি (value == f_No_মান) {
        SharedObject::clearPtr(value);
    }
}

শূন্য UnifiedCache::_registerPrimary(
            ধ্রুবক Cache_চাবি_Base *the_চাবি, ধ্রুবক SharedObject *value) ধ্রুবক {
    the_চাবি->fIsPrimary = সত্য;
    value->cachePtr = এই;
    ++f_অঙ্ক_মানসমূহ_যোগ;
    ++f_অঙ্ক_মানসমূহ_In_Use;
}

শূন্য UnifiedCache::_put(
        ধ্রুবক UHashElement *element,
        ধ্রুবক SharedObject *value,
        ধ্রুবক UErrorCode status) ধ্রুবক {
    U_ASSERT(_inProgress(element));
    ধ্রুবক Cache_চাবি_Base* the_চাবি = static_cast<ধ্রুবক Cache_চাবি_Base*>(element->চাবি.pointer);
    ধ্রুবক SharedObject* old_মান = static_cast<ধ্রুবক SharedObject*>(element->value.pointer);
    the_চাবি->fCreationStatus = status;
    যদি (value->soft_Ref_গুনতি == 0) {
        _registerPrimary(the_চাবি, value);
    }
    value->soft_Ref_গুনতি++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (শূন্য *) value;
    U_ASSERT(old_মান == f_No_মান);
    removeSoftRef(old_মান);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_মান_Added_Cond->notify_all();
}

শূন্য UnifiedCache::_fetch(
        ধ্রুবক UHashElement *element,
        ধ্রুবক SharedObject *&value,
        UErrorCode &status) ধ্রুবক {
    ধ্রুবক Cache_চাবি_Base* the_চাবি = static_cast<ধ্রুবক Cache_চাবি_Base*>(element->চাবি.pointer);
    status = the_চাবি->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ধ্রুবক SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(ধ্রুবক UHashElement* element) ধ্রুবক {
    UErrorCode status = U_ZERO_ERROR;
    ধ্রুবক SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool ফলাফল = _inProgress(value, status);
    removeHardRef(value);
    ফেরত ফলাফল;
}

UBool UnifiedCache::_inProgress(
        ধ্রুবক SharedObject* the_মান, UErrorCode creationStatus) ধ্রুবক {
    ফেরত (the_মান == f_No_মান && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ধ্রুবক UHashElement *element) ধ্রুবক
{
    ধ্রুবক Cache_চাবি_Base* the_চাবি = static_cast<ধ্রুবক Cache_চাবি_Base*>(element->চাবি.pointer);
    ধ্রুবক SharedObject* the_মান = static_cast<ধ্রুবক SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    যদি (_inProgress(the_মান, the_চাবি->fCreationStatus)) {
        ফেরত মিথ্যা;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    ফেরত (!the_চাবি->fIsPrimary || (the_মান->soft_Ref_গুনতি == 1 && the_মান->noHardReferences()));
}

শূন্য UnifiedCache::removeSoftRef(ধ্রুবক SharedObject *value) ধ্রুবক {
    U_ASSERT(value->cachePtr == এই);
    U_ASSERT(value->soft_Ref_গুনতি > 0);
    যদি (--value->soft_Ref_গুনতি == 0) {
        --f_অঙ্ক_মানসমূহ_যোগ;
        যদি (value->noHardReferences()) {
            delete value;
        } অন্যথা {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ধ্রুবক SharedObject *value) ধ্রুবক {
    পূর্ণসংখ্যা ref_গুনতি = 0;
    যদি (value) {
        ref_গুনতি = umtx_atomic_dec(&value->hard_Ref_গুনতি);
        U_ASSERT(ref_গুনতি >= 0);
        যদি (ref_গুনতি == 0) {
            --f_অঙ্ক_মানসমূহ_In_Use;
        }
    }
    ফেরত ref_গুনতি;
}

int32_t UnifiedCache::addHardRef(ধ্রুবক SharedObject *value) ধ্রুবক {
    পূর্ণসংখ্যা ref_গুনতি = 0;
    যদি (value) {
        ref_গুনতি = umtx_atomic_inc(&value->hard_Ref_গুনতি);
        U_ASSERT(ref_গুনতি >= 1);
        যদি (ref_গুনতি == 1) {
            f_অঙ্ক_মানসমূহ_In_Use++;
        }
    }
    ফেরত ref_গুনতি;
}

U_NAMESPACE_END

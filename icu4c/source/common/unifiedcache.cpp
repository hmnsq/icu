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
static std::condition_variable *g_In_Progress_हालागा_Added_Cond;
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
    g_In_Progress_हालागा_Added_Cond->~condition_variable();
    g_In_Progress_हालागा_Added_Cond = nullptr;
    return true;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_मगा_सुसि(const UHashTok सुसि) {
    const Cache_सुसि_Base* ckey = static_cast<const Cache_सुसि_Base*>(सुसि.pointer);
    return ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_मगा_सुसि(const UHashTok key1, const UHashTok key2) {
    const Cache_सुसि_Base* p1 = static_cast<const Cache_सुसि_Base*>(key1.pointer);
    const Cache_सुसि_Base* p2 = static_cast<const Cache_सुसि_Base*>(key2.pointer);
    return *p1 == *p2;
}

void U_EXPORT2
ucache_delete_सुसि(void *obj) {
    Cache_सुसि_Base* p = static_cast<Cache_सुसि_Base*>(obj);
    delete p;
}

Cache_सुसि_Base::~Cache_सुसि_Base() {
}

static void U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_हालागा_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = new UnifiedCache(status);
    if (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    if (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        return;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    if (U_FAILURE(status)) {
        return nullptr;
    }
    U_ASSERT(gCache != nullptr);
    return gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_नुमेरो_मगा_हालागा_Total(0),
        f_नुमेरो_मगा_हालागा_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_बिलांग(0),
        f_No_हालागा(nullptr) {
    if (U_FAILURE(status)) {
        return;
    }
    f_No_हालागा = new SharedObject();
    if (f_No_हालागा == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    f_No_हालागा->soft_Ref_बिलांग = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_हालागा->hard_Ref_बिलांग = 1;  // when other references to it are removed.
    f_No_हालागा->cachePtr = this;

    fHashtable = uhash_open(
            &ucache_hash_मगा_सुसि,
            &ucache_compare_मगा_सुसि,
            nullptr,
            &status);
    if (U_FAILURE(status)) {
        return;
    }
    uhash_set_सुसि_Deleter(fHashtable, &ucache_delete_सुसि);
}

void UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_मगा_बागाय, UErrorCode &status) {
    if (U_FAILURE(status)) {
        return;
    }
    if (count < 0 || percentage_Of_In_Use_मगा_बागाय < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_मगा_बागाय;
}

int32_t UnifiedCache::unused_बिलांग() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    return uhash_बिलांग(fHashtable) - f_नुमेरो_मगा_हालागा_In_Use;
}

int64_t UnifiedCache::auto_Evicted_बिलांग() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    return f_Auto_Evicted_बिलांग;
}

int32_t UnifiedCache::सुसि_बिलांग() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    return uhash_बिलांग(fHashtable);
}

void UnifiedCache::flush() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    while (_flush(false));
}

void UnifiedCache::handleUnreferencedObject() const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_नुमेरो_मगा_हालागा_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#include <stdio.h>

void UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    const UnifiedCache *cache = getInstance(status);
    if (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        return;
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
    char buffer[256];
    int32_t cnt = 0;
    for (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        const SharedObject *sharedObject =
                (const SharedObject *) element->value.pointer;
        const Cache_सुसि_Base *सुसि =
                (const Cache_सुसि_Base *) element->सुसि.pointer;
        if (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    सुसि->writeDescription(buffer, 256),
                    सुसि->creationStatus,
                    sharedObject == f_No_हालागा ? nullptr :sharedObject,
                    sharedObject->get_Ref_बिलांग(),
                    sharedObject->get_Soft_Ref_बिलांग());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_बिलांग(fHashtable));
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
    delete f_No_हालागा;
    f_No_हालागा = nullptr;
}

const UHashElement *
UnifiedCache::_nextElement() const {
    const UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    if (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        return uhash_nextElement(fHashtable, &fEvictPos);
    }
    return element;
}

UBool UnifiedCache::_flush(UBool all) const {
    UBool result = false;
    int32_t origSize = uhash_बिलांग(fHashtable);
    for (int32_t i = 0; i < origSize; ++i) {
        const UHashElement *element = _nextElement();
        if (element == nullptr) {
            break;
        }
        if (all || _isEvictable(element)) {
            const SharedObject *sharedObject =
                    static_cast<const SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == this);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = true;
        }
    }
    return result;
}

int32_t UnifiedCache::compute_बिलांग_Of_मगा_बागाय_To_Evict() const {
    int32_t total_मगा_बागाय = uhash_बिलांग(fHashtable);
    int32_t evictable_मगा_बागाय = total_मगा_बागाय - f_नुमेरो_मगा_हालागा_In_Use;

    int32_t unusedLimitByPercentage = f_नुमेरो_मगा_हालागा_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t बिलांग_Of_मगा_बागाय_To_Evict = std::max<int32_t>(0, evictable_मगा_बागाय - unusedLimit);
    return बिलांग_Of_मगा_बागाय_To_Evict;
}

void UnifiedCache::_runEvictionSlice() const {
    int32_t max_मगा_बागाय_To_Evict = compute_बिलांग_Of_मगा_बागाय_To_Evict();
    if (max_मगा_बागाय_To_Evict <= 0) {
        return;
    }
    for (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        const UHashElement *element = _nextElement();
        if (element == nullptr) {
            break;
        }
        if (_isEvictable(element)) {
            const SharedObject *sharedObject =
                    static_cast<const SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_बिलांग;
            if (--max_मगा_बागाय_To_Evict == 0) {
                break;
            }
        }
    }
}

void UnifiedCache::_putNew(
        const Cache_सुसि_Base &सुसि,
        const SharedObject *value,
        const UErrorCode creationStatus,
        UErrorCode &status) const {
    if (U_FAILURE(status)) {
        return;
    }
    Cache_सुसि_Base *सुसि_To_Adopt = सुसि.clone();
    if (सुसि_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    सुसि_To_Adopt->fCreationStatus = creationStatus;
    if (value->soft_Ref_बिलांग == 0) {
        _registerPrimary(सुसि_To_Adopt, value);
    }
    void *old_हालागा = uhash_put(fHashtable, सुसि_To_Adopt, (void *) value, &status);
    U_ASSERT(old_हालागा == nullptr);
    (void)old_हालागा;
    if (U_SUCCESS(status)) {
        value->soft_Ref_बिलांग++;
    }
}

void UnifiedCache::_putIfAbsentAndGet(
        const Cache_सुसि_Base &सुसि,
        const SharedObject *&value,
        UErrorCode &status) const {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    const UHashElement *element = uhash_find(fHashtable, &सुसि);
    if (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        return;
    }
    if (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(सुसि, value, status, putError);
    } else {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        const Cache_सुसि_Base &सुसि,
        const SharedObject *&value,
        UErrorCode &status) const {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    const UHashElement *element = uhash_find(fHashtable, &सुसि);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     while (element != nullptr && _inProgress(element)) {
         g_In_Progress_हालागा_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &सुसि);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    if (element != nullptr) {
         _fetch(element, value, status);
        return true;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(सुसि, f_No_हालागा, U_ZERO_ERROR, status);
    return false;
}

void UnifiedCache::_get(
        const Cache_सुसि_Base &सुसि,
        const SharedObject *&value,
        const void *creationContext,
        UErrorCode &status) const {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    if (_poll(सुसि, value, status)) {
        if (value == f_No_हालागा) {
            SharedObject::clearPtr(value);
        }
        return;
    }
    if (U_FAILURE(status)) {
        return;
    }
    value = सुसि.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    if (value == nullptr) {
        SharedObject::copyPtr(f_No_हालागा, value);
    }
    _putIfAbsentAndGet(सुसि, value, status);
    if (value == f_No_हालागा) {
        SharedObject::clearPtr(value);
    }
}

void UnifiedCache::_registerPrimary(
            const Cache_सुसि_Base *the_सुसि, const SharedObject *value) const {
    the_सुसि->fIsPrimary = true;
    value->cachePtr = this;
    ++f_नुमेरो_मगा_हालागा_Total;
    ++f_नुमेरो_मगा_हालागा_In_Use;
}

void UnifiedCache::_put(
        const UHashElement *element,
        const SharedObject *value,
        const UErrorCode status) const {
    U_ASSERT(_inProgress(element));
    const Cache_सुसि_Base* the_सुसि = static_cast<const Cache_सुसि_Base*>(element->सुसि.pointer);
    const SharedObject* old_हालागा = static_cast<const SharedObject*>(element->value.pointer);
    the_सुसि->fCreationStatus = status;
    if (value->soft_Ref_बिलांग == 0) {
        _registerPrimary(the_सुसि, value);
    }
    value->soft_Ref_बिलांग++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (void *) value;
    U_ASSERT(old_हालागा == f_No_हालागा);
    removeSoftRef(old_हालागा);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_हालागा_Added_Cond->notify_all();
}

void UnifiedCache::_fetch(
        const UHashElement *element,
        const SharedObject *&value,
        UErrorCode &status) const {
    const Cache_सुसि_Base* the_सुसि = static_cast<const Cache_सुसि_Base*>(element->सुसि.pointer);
    status = the_सुसि->fCreationStatus;

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
    return result;
}

UBool UnifiedCache::_inProgress(
        const SharedObject* the_हालागा, UErrorCode creationStatus) const {
    return (the_हालागा == f_No_हालागा && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(const UHashElement *element) const
{
    const Cache_सुसि_Base* the_सुसि = static_cast<const Cache_सुसि_Base*>(element->सुसि.pointer);
    const SharedObject* the_हालागा = static_cast<const SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    if (_inProgress(the_हालागा, the_सुसि->fCreationStatus)) {
        return false;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    return (!the_सुसि->fIsPrimary || (the_हालागा->soft_Ref_बिलांग == 1 && the_हालागा->noHardReferences()));
}

void UnifiedCache::removeSoftRef(const SharedObject *value) const {
    U_ASSERT(value->cachePtr == this);
    U_ASSERT(value->soft_Ref_बिलांग > 0);
    if (--value->soft_Ref_बिलांग == 0) {
        --f_नुमेरो_मगा_हालागा_Total;
        if (value->noHardReferences()) {
            delete value;
        } else {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(const SharedObject *value) const {
    int ref_बिलांग = 0;
    if (value) {
        ref_बिलांग = umtx_atomic_dec(&value->hard_Ref_बिलांग);
        U_ASSERT(ref_बिलांग >= 0);
        if (ref_बिलांग == 0) {
            --f_नुमेरो_मगा_हालागा_In_Use;
        }
    }
    return ref_बिलांग;
}

int32_t UnifiedCache::addHardRef(const SharedObject *value) const {
    int ref_बिलांग = 0;
    if (value) {
        ref_बिलांग = umtx_atomic_inc(&value->hard_Ref_बिलांग);
        U_ASSERT(ref_बिलांग >= 1);
        if (ref_बिलांग == 1) {
            f_नुमेरो_मगा_हालागा_In_Use++;
        }
    }
    return ref_बिलांग;
}

U_NAMESPACE_END

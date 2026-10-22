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

#அந்தர்பாவ "unifiedcache.h"

#அந்தர்பாவ <algorithm>      // For std::max()
#அந்தர்பாவ <mutex>

#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "ucln_cmn.h"

நிலையான icu::UnifiedCache *gCache = nullptr;
நிலையான std::mutex *gCacheMutex = nullptr;
நிலையான std::condition_variable *g_In_Progress_மதிப்பு_Added_Cond;
நிலையான icu::UInitOnce gCacheInitOnce {};

நிலையான மாறிலி int32_t MAX_EVICT_ITERATIONS = 10;
நிலையான மாறிலி int32_t DEFAULT_MAX_UNUSED = 1000;
நிலையான மாறிலி int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
நிலையான UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_மதிப்பு_Added_Cond->~condition_variable();
    g_In_Progress_மதிப்பு_Added_Cond = nullptr;
    திரும்பு உண்மை;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_சாவிகள்(மாறிலி UHashTok சாவி) {
    மாறிலி Cache_சாவி_Base* ckey = static_cast<மாறிலி Cache_சாவி_Base*>(சாவி.pointer);
    திரும்பு ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_சாவிகள்(மாறிலி UHashTok key1, மாறிலி UHashTok key2) {
    மாறிலி Cache_சாவி_Base* p1 = static_cast<மாறிலி Cache_சாவி_Base*>(key1.pointer);
    மாறிலி Cache_சாவி_Base* p2 = static_cast<மாறிலி Cache_சாவி_Base*>(key2.pointer);
    திரும்பு *p1 == *p2;
}

வெறுமை U_EXPORT2
ucache_delete_சாவி(வெறுமை *obj) {
    Cache_சாவி_Base* p = static_cast<Cache_சாவி_Base*>(obj);
    delete p;
}

Cache_சாவி_Base::~Cache_சாவி_Base() {
}

நிலையான வெறுமை U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_மதிப்பு_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = புதிய UnifiedCache(status);
    எனில் (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    எனில் (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        திரும்பு;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    எனில் (U_FAILURE(status)) {
        திரும்பு nullptr;
    }
    U_ASSERT(gCache != nullptr);
    திரும்பு gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_இலக்கம்_மதிப்புகள்_Total(0),
        f_இலக்கம்_மதிப்புகள்_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_மொத்தம்(0),
        f_No_மதிப்பு(nullptr) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    f_No_மதிப்பு = புதிய SharedObject();
    எனில் (f_No_மதிப்பு == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    f_No_மதிப்பு->soft_Ref_மொத்தம் = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_மதிப்பு->hard_Ref_மொத்தம் = 1;  // when other references to it are removed.
    f_No_மதிப்பு->cachePtr = இது;

    fHashtable = uhash_open(
            &ucache_hash_சாவிகள்,
            &ucache_compare_சாவிகள்,
            nullptr,
            &status);
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    uhash_set_சாவி_Deleter(fHashtable, &ucache_delete_சாவி);
}

வெறுமை UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_பொருட்கள், UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    எனில் (count < 0 || percentage_Of_In_Use_பொருட்கள் < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_பொருட்கள்;
}

int32_t UnifiedCache::unused_மொத்தம்() மாறிலி {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    திரும்பு uhash_மொத்தம்(fHashtable) - f_இலக்கம்_மதிப்புகள்_In_Use;
}

int64_t UnifiedCache::auto_Evicted_மொத்தம்() மாறிலி {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    திரும்பு f_Auto_Evicted_மொத்தம்;
}

int32_t UnifiedCache::சாவி_மொத்தம்() மாறிலி {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    திரும்பு uhash_மொத்தம்(fHashtable);
}

வெறுமை UnifiedCache::flush() மாறிலி {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    போது (_flush(பொய்));
}

வெறுமை UnifiedCache::handleUnreferencedObject() மாறிலி {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_இலக்கம்_மதிப்புகள்_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#அந்தர்பாவ <stdio.h>

வெறுமை UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    மாறிலி UnifiedCache *cache = getInstance(status);
    எனில் (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        திரும்பு;
    }
    cache->dumpContents();
}

வெறுமை UnifiedCache::dumpContents() மாறிலி {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
வெறுமை UnifiedCache::_dumpContents() மாறிலி {
    int32_t pos = UHASH_FIRST;
    மாறிலி UHashElement *element = uhash_nextElement(fHashtable, &pos);
    எழுத்து buffer[256];
    int32_t cnt = 0;
    க்கு (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        மாறிலி SharedObject *sharedObject =
                (மாறிலி SharedObject *) element->value.pointer;
        மாறிலி Cache_சாவி_Base *சாவி =
                (மாறிலி Cache_சாவி_Base *) element->சாவி.pointer;
        எனில் (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    சாவி->writeDescription(buffer, 256),
                    சாவி->creationStatus,
                    sharedObject == f_No_மதிப்பு ? nullptr :sharedObject,
                    sharedObject->get_Ref_மொத்தம்(),
                    sharedObject->get_Soft_Ref_மொத்தம்());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_மொத்தம்(fHashtable));
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
        _flush(உண்மை);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete f_No_மதிப்பு;
    f_No_மதிப்பு = nullptr;
}

மாறிலி UHashElement *
UnifiedCache::_nextElement() மாறிலி {
    மாறிலி UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    எனில் (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        திரும்பு uhash_nextElement(fHashtable, &fEvictPos);
    }
    திரும்பு element;
}

UBool UnifiedCache::_flush(UBool all) மாறிலி {
    UBool result = பொய்;
    int32_t origSize = uhash_மொத்தம்(fHashtable);
    க்கு (int32_t i = 0; i < origSize; ++i) {
        மாறிலி UHashElement *element = _nextElement();
        எனில் (element == nullptr) {
            நிறுத்து;
        }
        எனில் (all || _isEvictable(element)) {
            மாறிலி SharedObject *sharedObject =
                    static_cast<மாறிலி SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == இது);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = உண்மை;
        }
    }
    திரும்பு result;
}

int32_t UnifiedCache::compute_மொத்தம்_Of_பொருட்கள்_To_Evict() மாறிலி {
    int32_t total_பொருட்கள் = uhash_மொத்தம்(fHashtable);
    int32_t evictable_பொருட்கள் = total_பொருட்கள் - f_இலக்கம்_மதிப்புகள்_In_Use;

    int32_t unusedLimitByPercentage = f_இலக்கம்_மதிப்புகள்_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t மொத்தம்_Of_பொருட்கள்_To_Evict = std::max<int32_t>(0, evictable_பொருட்கள் - unusedLimit);
    திரும்பு மொத்தம்_Of_பொருட்கள்_To_Evict;
}

வெறுமை UnifiedCache::_runEvictionSlice() மாறிலி {
    int32_t max_பொருட்கள்_To_Evict = compute_மொத்தம்_Of_பொருட்கள்_To_Evict();
    எனில் (max_பொருட்கள்_To_Evict <= 0) {
        திரும்பு;
    }
    க்கு (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        மாறிலி UHashElement *element = _nextElement();
        எனில் (element == nullptr) {
            நிறுத்து;
        }
        எனில் (_isEvictable(element)) {
            மாறிலி SharedObject *sharedObject =
                    static_cast<மாறிலி SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_மொத்தம்;
            எனில் (--max_பொருட்கள்_To_Evict == 0) {
                நிறுத்து;
            }
        }
    }
}

வெறுமை UnifiedCache::_putNew(
        மாறிலி Cache_சாவி_Base &சாவி,
        மாறிலி SharedObject *value,
        மாறிலி UErrorCode creationStatus,
        UErrorCode &status) மாறிலி {
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    Cache_சாவி_Base *சாவி_To_Adopt = சாவி.clone();
    எனில் (சாவி_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    சாவி_To_Adopt->fCreationStatus = creationStatus;
    எனில் (value->soft_Ref_மொத்தம் == 0) {
        _registerPrimary(சாவி_To_Adopt, value);
    }
    வெறுமை *old_மதிப்பு = uhash_put(fHashtable, சாவி_To_Adopt, (வெறுமை *) value, &status);
    U_ASSERT(old_மதிப்பு == nullptr);
    (வெறுமை)old_மதிப்பு;
    எனில் (U_SUCCESS(status)) {
        value->soft_Ref_மொத்தம்++;
    }
}

வெறுமை UnifiedCache::_putIfAbsentAndGet(
        மாறிலி Cache_சாவி_Base &சாவி,
        மாறிலி SharedObject *&value,
        UErrorCode &status) மாறிலி {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    மாறிலி UHashElement *element = uhash_find(fHashtable, &சாவி);
    எனில் (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        திரும்பு;
    }
    எனில் (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(சாவி, value, status, putError);
    } இல்லையெனில் {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        மாறிலி Cache_சாவி_Base &சாவி,
        மாறிலி SharedObject *&value,
        UErrorCode &status) மாறிலி {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    மாறிலி UHashElement *element = uhash_find(fHashtable, &சாவி);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     போது (element != nullptr && _inProgress(element)) {
         g_In_Progress_மதிப்பு_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &சாவி);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    எனில் (element != nullptr) {
         _fetch(element, value, status);
        திரும்பு உண்மை;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(சாவி, f_No_மதிப்பு, U_ZERO_ERROR, status);
    திரும்பு பொய்;
}

வெறுமை UnifiedCache::_get(
        மாறிலி Cache_சாவி_Base &சாவி,
        மாறிலி SharedObject *&value,
        மாறிலி வெறுமை *creationContext,
        UErrorCode &status) மாறிலி {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    எனில் (_poll(சாவி, value, status)) {
        எனில் (value == f_No_மதிப்பு) {
            SharedObject::clearPtr(value);
        }
        திரும்பு;
    }
    எனில் (U_FAILURE(status)) {
        திரும்பு;
    }
    value = சாவி.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    எனில் (value == nullptr) {
        SharedObject::copyPtr(f_No_மதிப்பு, value);
    }
    _putIfAbsentAndGet(சாவி, value, status);
    எனில் (value == f_No_மதிப்பு) {
        SharedObject::clearPtr(value);
    }
}

வெறுமை UnifiedCache::_registerPrimary(
            மாறிலி Cache_சாவி_Base *the_சாவி, மாறிலி SharedObject *value) மாறிலி {
    the_சாவி->fIsPrimary = உண்மை;
    value->cachePtr = இது;
    ++f_இலக்கம்_மதிப்புகள்_Total;
    ++f_இலக்கம்_மதிப்புகள்_In_Use;
}

வெறுமை UnifiedCache::_put(
        மாறிலி UHashElement *element,
        மாறிலி SharedObject *value,
        மாறிலி UErrorCode status) மாறிலி {
    U_ASSERT(_inProgress(element));
    மாறிலி Cache_சாவி_Base* the_சாவி = static_cast<மாறிலி Cache_சாவி_Base*>(element->சாவி.pointer);
    மாறிலி SharedObject* old_மதிப்பு = static_cast<மாறிலி SharedObject*>(element->value.pointer);
    the_சாவி->fCreationStatus = status;
    எனில் (value->soft_Ref_மொத்தம் == 0) {
        _registerPrimary(the_சாவி, value);
    }
    value->soft_Ref_மொத்தம்++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (வெறுமை *) value;
    U_ASSERT(old_மதிப்பு == f_No_மதிப்பு);
    removeSoftRef(old_மதிப்பு);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_மதிப்பு_Added_Cond->notify_all();
}

வெறுமை UnifiedCache::_fetch(
        மாறிலி UHashElement *element,
        மாறிலி SharedObject *&value,
        UErrorCode &status) மாறிலி {
    மாறிலி Cache_சாவி_Base* the_சாவி = static_cast<மாறிலி Cache_சாவி_Base*>(element->சாவி.pointer);
    status = the_சாவி->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<மாறிலி SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(மாறிலி UHashElement* element) மாறிலி {
    UErrorCode status = U_ZERO_ERROR;
    மாறிலி SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    திரும்பு result;
}

UBool UnifiedCache::_inProgress(
        மாறிலி SharedObject* the_மதிப்பு, UErrorCode creationStatus) மாறிலி {
    திரும்பு (the_மதிப்பு == f_No_மதிப்பு && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(மாறிலி UHashElement *element) மாறிலி
{
    மாறிலி Cache_சாவி_Base* the_சாவி = static_cast<மாறிலி Cache_சாவி_Base*>(element->சாவி.pointer);
    மாறிலி SharedObject* the_மதிப்பு = static_cast<மாறிலி SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    எனில் (_inProgress(the_மதிப்பு, the_சாவி->fCreationStatus)) {
        திரும்பு பொய்;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    திரும்பு (!the_சாவி->fIsPrimary || (the_மதிப்பு->soft_Ref_மொத்தம் == 1 && the_மதிப்பு->noHardReferences()));
}

வெறுமை UnifiedCache::removeSoftRef(மாறிலி SharedObject *value) மாறிலி {
    U_ASSERT(value->cachePtr == இது);
    U_ASSERT(value->soft_Ref_மொத்தம் > 0);
    எனில் (--value->soft_Ref_மொத்தம் == 0) {
        --f_இலக்கம்_மதிப்புகள்_Total;
        எனில் (value->noHardReferences()) {
            delete value;
        } இல்லையெனில் {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(மாறிலி SharedObject *value) மாறிலி {
    முழு ref_மொத்தம் = 0;
    எனில் (value) {
        ref_மொத்தம் = umtx_atomic_dec(&value->hard_Ref_மொத்தம்);
        U_ASSERT(ref_மொத்தம் >= 0);
        எனில் (ref_மொத்தம் == 0) {
            --f_இலக்கம்_மதிப்புகள்_In_Use;
        }
    }
    திரும்பு ref_மொத்தம்;
}

int32_t UnifiedCache::addHardRef(மாறிலி SharedObject *value) மாறிலி {
    முழு ref_மொத்தம் = 0;
    எனில் (value) {
        ref_மொத்தம் = umtx_atomic_inc(&value->hard_Ref_மொத்தம்);
        U_ASSERT(ref_மொத்தம் >= 1);
        எனில் (ref_மொத்தம் == 1) {
            f_இலக்கம்_மதிப்புகள்_In_Use++;
        }
    }
    திரும்பு ref_மொத்தம்;
}

U_NAMESPACE_END

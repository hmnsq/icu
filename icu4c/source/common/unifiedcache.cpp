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

#അന്തര്ഭാവ "unifiedcache.h"

#അന്തര്ഭാവ <algorithm>      // For std::max()
#അന്തര്ഭാവ <mutex>

#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "ucln_cmn.h"

സ്ഥിതം icu::UnifiedCache *gCache = nullptr;
സ്ഥിതം std::mutex *gCacheMutex = nullptr;
സ്ഥിതം std::condition_variable *g_In_Progress_മൂല്യം_Added_Cond;
സ്ഥിതം icu::UInitOnce gCacheInitOnce {};

സ്ഥിതം സ്ഥിരം int32_t MAX_EVICT_ITERATIONS = 10;
സ്ഥിതം സ്ഥിരം int32_t DEFAULT_MAX_UNUSED = 1000;
സ്ഥിതം സ്ഥിരം int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
സ്ഥിതം UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_മൂല്യം_Added_Cond->~condition_variable();
    g_In_Progress_മൂല്യം_Added_Cond = nullptr;
    തിരികെ സത്യം;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_താക്കോലുകൾ(സ്ഥിരം UHashTok താക്കോൽ) {
    സ്ഥിരം Cache_താക്കോൽ_Base* ckey = static_cast<സ്ഥിരം Cache_താക്കോൽ_Base*>(താക്കോൽ.pointer);
    തിരികെ ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_താക്കോലുകൾ(സ്ഥിരം UHashTok key1, സ്ഥിരം UHashTok key2) {
    സ്ഥിരം Cache_താക്കോൽ_Base* p1 = static_cast<സ്ഥിരം Cache_താക്കോൽ_Base*>(key1.pointer);
    സ്ഥിരം Cache_താക്കോൽ_Base* p2 = static_cast<സ്ഥിരം Cache_താക്കോൽ_Base*>(key2.pointer);
    തിരികെ *p1 == *p2;
}

ശൂന്യം U_EXPORT2
ucache_delete_താക്കോൽ(ശൂന്യം *obj) {
    Cache_താക്കോൽ_Base* p = static_cast<Cache_താക്കോൽ_Base*>(obj);
    delete p;
}

Cache_താക്കോൽ_Base::~Cache_താക്കോൽ_Base() {
}

സ്ഥിതം ശൂന്യം U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_മൂല്യം_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = പുതിയ UnifiedCache(status);
    എങ്കിൽ (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    എങ്കിൽ (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        തിരികെ;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    U_ASSERT(gCache != nullptr);
    തിരികെ gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_അക്കം_മൂല്യങ്ങൾ_Total(0),
        f_അക്കം_മൂല്യങ്ങൾ_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_എണ്ണം(0),
        f_No_മൂല്യം(nullptr) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    f_No_മൂല്യം = പുതിയ SharedObject();
    എങ്കിൽ (f_No_മൂല്യം == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    f_No_മൂല്യം->soft_Ref_എണ്ണം = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_മൂല്യം->hard_Ref_എണ്ണം = 1;  // when other references to it are removed.
    f_No_മൂല്യം->cachePtr = ഇത്;

    fHashtable = uhash_open(
            &ucache_hash_താക്കോലുകൾ,
            &ucache_compare_താക്കോലുകൾ,
            nullptr,
            &status);
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    uhash_set_താക്കോൽ_Deleter(fHashtable, &ucache_delete_താക്കോൽ);
}

ശൂന്യം UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_ഇനങ്ങൾ, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    എങ്കിൽ (count < 0 || percentage_Of_In_Use_ഇനങ്ങൾ < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_ഇനങ്ങൾ;
}

int32_t UnifiedCache::unused_എണ്ണം() സ്ഥിരം {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    തിരികെ uhash_എണ്ണം(fHashtable) - f_അക്കം_മൂല്യങ്ങൾ_In_Use;
}

int64_t UnifiedCache::auto_Evicted_എണ്ണം() സ്ഥിരം {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    തിരികെ f_Auto_Evicted_എണ്ണം;
}

int32_t UnifiedCache::താക്കോൽ_എണ്ണം() സ്ഥിരം {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    തിരികെ uhash_എണ്ണം(fHashtable);
}

ശൂന്യം UnifiedCache::flush() സ്ഥിരം {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    ആകുമ്പോൾ (_flush(അസത്യം));
}

ശൂന്യം UnifiedCache::handleUnreferencedObject() സ്ഥിരം {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_അക്കം_മൂല്യങ്ങൾ_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#അന്തര്ഭാവ <stdio.h>

ശൂന്യം UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    സ്ഥിരം UnifiedCache *cache = getInstance(status);
    എങ്കിൽ (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        തിരികെ;
    }
    cache->dumpContents();
}

ശൂന്യം UnifiedCache::dumpContents() സ്ഥിരം {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
ശൂന്യം UnifiedCache::_dumpContents() സ്ഥിരം {
    int32_t pos = UHASH_FIRST;
    സ്ഥിരം UHashElement *element = uhash_nextElement(fHashtable, &pos);
    അക്ഷരം buffer[256];
    int32_t cnt = 0;
    വേണ്ടി (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        സ്ഥിരം SharedObject *sharedObject =
                (സ്ഥിരം SharedObject *) element->value.pointer;
        സ്ഥിരം Cache_താക്കോൽ_Base *താക്കോൽ =
                (സ്ഥിരം Cache_താക്കോൽ_Base *) element->താക്കോൽ.pointer;
        എങ്കിൽ (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    താക്കോൽ->writeDescription(buffer, 256),
                    താക്കോൽ->creationStatus,
                    sharedObject == f_No_മൂല്യം ? nullptr :sharedObject,
                    sharedObject->get_Ref_എണ്ണം(),
                    sharedObject->get_Soft_Ref_എണ്ണം());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_എണ്ണം(fHashtable));
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
        _flush(സത്യം);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete f_No_മൂല്യം;
    f_No_മൂല്യം = nullptr;
}

സ്ഥിരം UHashElement *
UnifiedCache::_nextElement() സ്ഥിരം {
    സ്ഥിരം UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    എങ്കിൽ (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        തിരികെ uhash_nextElement(fHashtable, &fEvictPos);
    }
    തിരികെ element;
}

UBool UnifiedCache::_flush(UBool all) സ്ഥിരം {
    UBool result = അസത്യം;
    int32_t origSize = uhash_എണ്ണം(fHashtable);
    വേണ്ടി (int32_t i = 0; i < origSize; ++i) {
        സ്ഥിരം UHashElement *element = _nextElement();
        എങ്കിൽ (element == nullptr) {
            നിർത്തുക;
        }
        എങ്കിൽ (all || _isEvictable(element)) {
            സ്ഥിരം SharedObject *sharedObject =
                    static_cast<സ്ഥിരം SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == ഇത്);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = സത്യം;
        }
    }
    തിരികെ result;
}

int32_t UnifiedCache::compute_എണ്ണം_Of_ഇനങ്ങൾ_To_Evict() സ്ഥിരം {
    int32_t total_ഇനങ്ങൾ = uhash_എണ്ണം(fHashtable);
    int32_t evictable_ഇനങ്ങൾ = total_ഇനങ്ങൾ - f_അക്കം_മൂല്യങ്ങൾ_In_Use;

    int32_t unusedLimitByPercentage = f_അക്കം_മൂല്യങ്ങൾ_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t എണ്ണം_Of_ഇനങ്ങൾ_To_Evict = std::max<int32_t>(0, evictable_ഇനങ്ങൾ - unusedLimit);
    തിരികെ എണ്ണം_Of_ഇനങ്ങൾ_To_Evict;
}

ശൂന്യം UnifiedCache::_runEvictionSlice() സ്ഥിരം {
    int32_t max_ഇനങ്ങൾ_To_Evict = compute_എണ്ണം_Of_ഇനങ്ങൾ_To_Evict();
    എങ്കിൽ (max_ഇനങ്ങൾ_To_Evict <= 0) {
        തിരികെ;
    }
    വേണ്ടി (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        സ്ഥിരം UHashElement *element = _nextElement();
        എങ്കിൽ (element == nullptr) {
            നിർത്തുക;
        }
        എങ്കിൽ (_isEvictable(element)) {
            സ്ഥിരം SharedObject *sharedObject =
                    static_cast<സ്ഥിരം SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_എണ്ണം;
            എങ്കിൽ (--max_ഇനങ്ങൾ_To_Evict == 0) {
                നിർത്തുക;
            }
        }
    }
}

ശൂന്യം UnifiedCache::_putNew(
        സ്ഥിരം Cache_താക്കോൽ_Base &താക്കോൽ,
        സ്ഥിരം SharedObject *value,
        സ്ഥിരം UErrorCode creationStatus,
        UErrorCode &status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    Cache_താക്കോൽ_Base *താക്കോൽ_To_Adopt = താക്കോൽ.clone();
    എങ്കിൽ (താക്കോൽ_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    താക്കോൽ_To_Adopt->fCreationStatus = creationStatus;
    എങ്കിൽ (value->soft_Ref_എണ്ണം == 0) {
        _registerPrimary(താക്കോൽ_To_Adopt, value);
    }
    ശൂന്യം *old_മൂല്യം = uhash_put(fHashtable, താക്കോൽ_To_Adopt, (ശൂന്യം *) value, &status);
    U_ASSERT(old_മൂല്യം == nullptr);
    (ശൂന്യം)old_മൂല്യം;
    എങ്കിൽ (U_SUCCESS(status)) {
        value->soft_Ref_എണ്ണം++;
    }
}

ശൂന്യം UnifiedCache::_putIfAbsentAndGet(
        സ്ഥിരം Cache_താക്കോൽ_Base &താക്കോൽ,
        സ്ഥിരം SharedObject *&value,
        UErrorCode &status) സ്ഥിരം {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    സ്ഥിരം UHashElement *element = uhash_find(fHashtable, &താക്കോൽ);
    എങ്കിൽ (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        തിരികെ;
    }
    എങ്കിൽ (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(താക്കോൽ, value, status, putError);
    } അല്ലെങ്കിൽ {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        സ്ഥിരം Cache_താക്കോൽ_Base &താക്കോൽ,
        സ്ഥിരം SharedObject *&value,
        UErrorCode &status) സ്ഥിരം {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    സ്ഥിരം UHashElement *element = uhash_find(fHashtable, &താക്കോൽ);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     ആകുമ്പോൾ (element != nullptr && _inProgress(element)) {
         g_In_Progress_മൂല്യം_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &താക്കോൽ);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    എങ്കിൽ (element != nullptr) {
         _fetch(element, value, status);
        തിരികെ സത്യം;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(താക്കോൽ, f_No_മൂല്യം, U_ZERO_ERROR, status);
    തിരികെ അസത്യം;
}

ശൂന്യം UnifiedCache::_get(
        സ്ഥിരം Cache_താക്കോൽ_Base &താക്കോൽ,
        സ്ഥിരം SharedObject *&value,
        സ്ഥിരം ശൂന്യം *creationContext,
        UErrorCode &status) സ്ഥിരം {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    എങ്കിൽ (_poll(താക്കോൽ, value, status)) {
        എങ്കിൽ (value == f_No_മൂല്യം) {
            SharedObject::clearPtr(value);
        }
        തിരികെ;
    }
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    value = താക്കോൽ.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    എങ്കിൽ (value == nullptr) {
        SharedObject::copyPtr(f_No_മൂല്യം, value);
    }
    _putIfAbsentAndGet(താക്കോൽ, value, status);
    എങ്കിൽ (value == f_No_മൂല്യം) {
        SharedObject::clearPtr(value);
    }
}

ശൂന്യം UnifiedCache::_registerPrimary(
            സ്ഥിരം Cache_താക്കോൽ_Base *the_താക്കോൽ, സ്ഥിരം SharedObject *value) സ്ഥിരം {
    the_താക്കോൽ->fIsPrimary = സത്യം;
    value->cachePtr = ഇത്;
    ++f_അക്കം_മൂല്യങ്ങൾ_Total;
    ++f_അക്കം_മൂല്യങ്ങൾ_In_Use;
}

ശൂന്യം UnifiedCache::_put(
        സ്ഥിരം UHashElement *element,
        സ്ഥിരം SharedObject *value,
        സ്ഥിരം UErrorCode status) സ്ഥിരം {
    U_ASSERT(_inProgress(element));
    സ്ഥിരം Cache_താക്കോൽ_Base* the_താക്കോൽ = static_cast<സ്ഥിരം Cache_താക്കോൽ_Base*>(element->താക്കോൽ.pointer);
    സ്ഥിരം SharedObject* old_മൂല്യം = static_cast<സ്ഥിരം SharedObject*>(element->value.pointer);
    the_താക്കോൽ->fCreationStatus = status;
    എങ്കിൽ (value->soft_Ref_എണ്ണം == 0) {
        _registerPrimary(the_താക്കോൽ, value);
    }
    value->soft_Ref_എണ്ണം++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (ശൂന്യം *) value;
    U_ASSERT(old_മൂല്യം == f_No_മൂല്യം);
    removeSoftRef(old_മൂല്യം);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_മൂല്യം_Added_Cond->notify_all();
}

ശൂന്യം UnifiedCache::_fetch(
        സ്ഥിരം UHashElement *element,
        സ്ഥിരം SharedObject *&value,
        UErrorCode &status) സ്ഥിരം {
    സ്ഥിരം Cache_താക്കോൽ_Base* the_താക്കോൽ = static_cast<സ്ഥിരം Cache_താക്കോൽ_Base*>(element->താക്കോൽ.pointer);
    status = the_താക്കോൽ->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<സ്ഥിരം SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(സ്ഥിരം UHashElement* element) സ്ഥിരം {
    UErrorCode status = U_ZERO_ERROR;
    സ്ഥിരം SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    തിരികെ result;
}

UBool UnifiedCache::_inProgress(
        സ്ഥിരം SharedObject* the_മൂല്യം, UErrorCode creationStatus) സ്ഥിരം {
    തിരികെ (the_മൂല്യം == f_No_മൂല്യം && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(സ്ഥിരം UHashElement *element) സ്ഥിരം
{
    സ്ഥിരം Cache_താക്കോൽ_Base* the_താക്കോൽ = static_cast<സ്ഥിരം Cache_താക്കോൽ_Base*>(element->താക്കോൽ.pointer);
    സ്ഥിരം SharedObject* the_മൂല്യം = static_cast<സ്ഥിരം SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    എങ്കിൽ (_inProgress(the_മൂല്യം, the_താക്കോൽ->fCreationStatus)) {
        തിരികെ അസത്യം;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    തിരികെ (!the_താക്കോൽ->fIsPrimary || (the_മൂല്യം->soft_Ref_എണ്ണം == 1 && the_മൂല്യം->noHardReferences()));
}

ശൂന്യം UnifiedCache::removeSoftRef(സ്ഥിരം SharedObject *value) സ്ഥിരം {
    U_ASSERT(value->cachePtr == ഇത്);
    U_ASSERT(value->soft_Ref_എണ്ണം > 0);
    എങ്കിൽ (--value->soft_Ref_എണ്ണം == 0) {
        --f_അക്കം_മൂല്യങ്ങൾ_Total;
        എങ്കിൽ (value->noHardReferences()) {
            delete value;
        } അല്ലെങ്കിൽ {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(സ്ഥിരം SharedObject *value) സ്ഥിരം {
    പൂർണ്ണസംഖ്യ ref_എണ്ണം = 0;
    എങ്കിൽ (value) {
        ref_എണ്ണം = umtx_atomic_dec(&value->hard_Ref_എണ്ണം);
        U_ASSERT(ref_എണ്ണം >= 0);
        എങ്കിൽ (ref_എണ്ണം == 0) {
            --f_അക്കം_മൂല്യങ്ങൾ_In_Use;
        }
    }
    തിരികെ ref_എണ്ണം;
}

int32_t UnifiedCache::addHardRef(സ്ഥിരം SharedObject *value) സ്ഥിരം {
    പൂർണ്ണസംഖ്യ ref_എണ്ണം = 0;
    എങ്കിൽ (value) {
        ref_എണ്ണം = umtx_atomic_inc(&value->hard_Ref_എണ്ണം);
        U_ASSERT(ref_എണ്ണം >= 1);
        എങ്കിൽ (ref_എണ്ണം == 1) {
            f_അക്കം_മൂല്യങ്ങൾ_In_Use++;
        }
    }
    തിരികെ ref_എണ്ണം;
}

U_NAMESPACE_END

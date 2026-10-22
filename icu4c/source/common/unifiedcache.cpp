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

#અન્તર્ભાવ "unifiedcache.h"

#અન્તર્ભાવ <algorithm>      // For std::max()
#અન્તર્ભાવ <mutex>

#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "ucln_cmn.h"

સ્થિર icu::UnifiedCache *gCache = nullptr;
સ્થિર std::mutex *gCacheMutex = nullptr;
સ્થિર std::condition_variable *g_In_Progress_કિંમત_Added_Cond;
સ્થિર icu::UInitOnce gCacheInitOnce {};

સ્થિર અચલ int32_t MAX_EVICT_ITERATIONS = 10;
સ્થિર અચલ int32_t DEFAULT_MAX_UNUSED = 1000;
સ્થિર અચલ int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
સ્થિર UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_કિંમત_Added_Cond->~condition_variable();
    g_In_Progress_કિંમત_Added_Cond = nullptr;
    પાછા સાચું;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_ચાવીઓ(અચલ UHashTok ચાવી) {
    અચલ Cache_ચાવી_Base* ckey = static_cast<અચલ Cache_ચાવી_Base*>(ચાવી.pointer);
    પાછા ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_ચાવીઓ(અચલ UHashTok key1, અચલ UHashTok key2) {
    અચલ Cache_ચાવી_Base* p1 = static_cast<અચલ Cache_ચાવી_Base*>(key1.pointer);
    અચલ Cache_ચાવી_Base* p2 = static_cast<અચલ Cache_ચાવી_Base*>(key2.pointer);
    પાછા *p1 == *p2;
}

શૂન્ય U_EXPORT2
ucache_delete_ચાવી(શૂન્ય *obj) {
    Cache_ચાવી_Base* p = static_cast<Cache_ચાવી_Base*>(obj);
    delete p;
}

Cache_ચાવી_Base::~Cache_ચાવી_Base() {
}

સ્થિર શૂન્ય U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_કિંમત_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = નવું UnifiedCache(status);
    જો (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    જો (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        પાછા;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    જો (U_FAILURE(status)) {
        પાછા nullptr;
    }
    U_ASSERT(gCache != nullptr);
    પાછા gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_અંક_કિંમતો_Total(0),
        f_અંક_કિંમતો_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_ગણતરી(0),
        f_No_કિંમત(nullptr) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    f_No_કિંમત = નવું SharedObject();
    જો (f_No_કિંમત == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    f_No_કિંમત->soft_Ref_ગણતરી = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_કિંમત->hard_Ref_ગણતરી = 1;  // when other references to it are removed.
    f_No_કિંમત->cachePtr = આ;

    fHashtable = uhash_open(
            &ucache_hash_ચાવીઓ,
            &ucache_compare_ચાવીઓ,
            nullptr,
            &status);
    જો (U_FAILURE(status)) {
        પાછા;
    }
    uhash_set_ચાવી_Deleter(fHashtable, &ucache_delete_ચાવી);
}

શૂન્ય UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_વસ્તુઓ, UErrorCode &status) {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    જો (count < 0 || percentage_Of_In_Use_વસ્તુઓ < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_વસ્તુઓ;
}

int32_t UnifiedCache::unused_ગણતરી() અચલ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    પાછા uhash_ગણતરી(fHashtable) - f_અંક_કિંમતો_In_Use;
}

int64_t UnifiedCache::auto_Evicted_ગણતરી() અચલ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    પાછા f_Auto_Evicted_ગણતરી;
}

int32_t UnifiedCache::ચાવી_ગણતરી() અચલ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    પાછા uhash_ગણતરી(fHashtable);
}

શૂન્ય UnifiedCache::flush() અચલ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    જ્યારે (_flush(ખોટું));
}

શૂન્ય UnifiedCache::handleUnreferencedObject() અચલ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_અંક_કિંમતો_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#અન્તર્ભાવ <stdio.h>

શૂન્ય UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    અચલ UnifiedCache *cache = getInstance(status);
    જો (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        પાછા;
    }
    cache->dumpContents();
}

શૂન્ય UnifiedCache::dumpContents() અચલ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
શૂન્ય UnifiedCache::_dumpContents() અચલ {
    int32_t pos = UHASH_FIRST;
    અચલ UHashElement *element = uhash_nextElement(fHashtable, &pos);
    અક્ષર buffer[256];
    int32_t cnt = 0;
    માટે (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        અચલ SharedObject *sharedObject =
                (અચલ SharedObject *) element->value.pointer;
        અચલ Cache_ચાવી_Base *ચાવી =
                (અચલ Cache_ચાવી_Base *) element->ચાવી.pointer;
        જો (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    ચાવી->writeDescription(buffer, 256),
                    ચાવી->creationStatus,
                    sharedObject == f_No_કિંમત ? nullptr :sharedObject,
                    sharedObject->get_Ref_ગણતરી(),
                    sharedObject->get_Soft_Ref_ગણતરી());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_ગણતરી(fHashtable));
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
        _flush(સાચું);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete f_No_કિંમત;
    f_No_કિંમત = nullptr;
}

અચલ UHashElement *
UnifiedCache::_nextElement() અચલ {
    અચલ UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    જો (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        પાછા uhash_nextElement(fHashtable, &fEvictPos);
    }
    પાછા element;
}

UBool UnifiedCache::_flush(UBool all) અચલ {
    UBool result = ખોટું;
    int32_t origSize = uhash_ગણતરી(fHashtable);
    માટે (int32_t i = 0; i < origSize; ++i) {
        અચલ UHashElement *element = _nextElement();
        જો (element == nullptr) {
            તોડો;
        }
        જો (all || _isEvictable(element)) {
            અચલ SharedObject *sharedObject =
                    static_cast<અચલ SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == આ);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = સાચું;
        }
    }
    પાછા result;
}

int32_t UnifiedCache::compute_ગણતરી_Of_વસ્તુઓ_To_Evict() અચલ {
    int32_t total_વસ્તુઓ = uhash_ગણતરી(fHashtable);
    int32_t evictable_વસ્તુઓ = total_વસ્તુઓ - f_અંક_કિંમતો_In_Use;

    int32_t unusedLimitByPercentage = f_અંક_કિંમતો_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t ગણતરી_Of_વસ્તુઓ_To_Evict = std::max<int32_t>(0, evictable_વસ્તુઓ - unusedLimit);
    પાછા ગણતરી_Of_વસ્તુઓ_To_Evict;
}

શૂન્ય UnifiedCache::_runEvictionSlice() અચલ {
    int32_t max_વસ્તુઓ_To_Evict = compute_ગણતરી_Of_વસ્તુઓ_To_Evict();
    જો (max_વસ્તુઓ_To_Evict <= 0) {
        પાછા;
    }
    માટે (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        અચલ UHashElement *element = _nextElement();
        જો (element == nullptr) {
            તોડો;
        }
        જો (_isEvictable(element)) {
            અચલ SharedObject *sharedObject =
                    static_cast<અચલ SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_ગણતરી;
            જો (--max_વસ્તુઓ_To_Evict == 0) {
                તોડો;
            }
        }
    }
}

શૂન્ય UnifiedCache::_putNew(
        અચલ Cache_ચાવી_Base &ચાવી,
        અચલ SharedObject *value,
        અચલ UErrorCode creationStatus,
        UErrorCode &status) અચલ {
    જો (U_FAILURE(status)) {
        પાછા;
    }
    Cache_ચાવી_Base *ચાવી_To_Adopt = ચાવી.clone();
    જો (ચાવી_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    ચાવી_To_Adopt->fCreationStatus = creationStatus;
    જો (value->soft_Ref_ગણતરી == 0) {
        _registerPrimary(ચાવી_To_Adopt, value);
    }
    શૂન્ય *old_કિંમત = uhash_put(fHashtable, ચાવી_To_Adopt, (શૂન્ય *) value, &status);
    U_ASSERT(old_કિંમત == nullptr);
    (શૂન્ય)old_કિંમત;
    જો (U_SUCCESS(status)) {
        value->soft_Ref_ગણતરી++;
    }
}

શૂન્ય UnifiedCache::_putIfAbsentAndGet(
        અચલ Cache_ચાવી_Base &ચાવી,
        અચલ SharedObject *&value,
        UErrorCode &status) અચલ {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    અચલ UHashElement *element = uhash_find(fHashtable, &ચાવી);
    જો (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        પાછા;
    }
    જો (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(ચાવી, value, status, putError);
    } નહીંતર {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        અચલ Cache_ચાવી_Base &ચાવી,
        અચલ SharedObject *&value,
        UErrorCode &status) અચલ {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    અચલ UHashElement *element = uhash_find(fHashtable, &ચાવી);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     જ્યારે (element != nullptr && _inProgress(element)) {
         g_In_Progress_કિંમત_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &ચાવી);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    જો (element != nullptr) {
         _fetch(element, value, status);
        પાછા સાચું;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(ચાવી, f_No_કિંમત, U_ZERO_ERROR, status);
    પાછા ખોટું;
}

શૂન્ય UnifiedCache::_get(
        અચલ Cache_ચાવી_Base &ચાવી,
        અચલ SharedObject *&value,
        અચલ શૂન્ય *creationContext,
        UErrorCode &status) અચલ {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    જો (_poll(ચાવી, value, status)) {
        જો (value == f_No_કિંમત) {
            SharedObject::clearPtr(value);
        }
        પાછા;
    }
    જો (U_FAILURE(status)) {
        પાછા;
    }
    value = ચાવી.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    જો (value == nullptr) {
        SharedObject::copyPtr(f_No_કિંમત, value);
    }
    _putIfAbsentAndGet(ચાવી, value, status);
    જો (value == f_No_કિંમત) {
        SharedObject::clearPtr(value);
    }
}

શૂન્ય UnifiedCache::_registerPrimary(
            અચલ Cache_ચાવી_Base *the_ચાવી, અચલ SharedObject *value) અચલ {
    the_ચાવી->fIsPrimary = સાચું;
    value->cachePtr = આ;
    ++f_અંક_કિંમતો_Total;
    ++f_અંક_કિંમતો_In_Use;
}

શૂન્ય UnifiedCache::_put(
        અચલ UHashElement *element,
        અચલ SharedObject *value,
        અચલ UErrorCode status) અચલ {
    U_ASSERT(_inProgress(element));
    અચલ Cache_ચાવી_Base* the_ચાવી = static_cast<અચલ Cache_ચાવી_Base*>(element->ચાવી.pointer);
    અચલ SharedObject* old_કિંમત = static_cast<અચલ SharedObject*>(element->value.pointer);
    the_ચાવી->fCreationStatus = status;
    જો (value->soft_Ref_ગણતરી == 0) {
        _registerPrimary(the_ચાવી, value);
    }
    value->soft_Ref_ગણતરી++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (શૂન્ય *) value;
    U_ASSERT(old_કિંમત == f_No_કિંમત);
    removeSoftRef(old_કિંમત);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_કિંમત_Added_Cond->notify_all();
}

શૂન્ય UnifiedCache::_fetch(
        અચલ UHashElement *element,
        અચલ SharedObject *&value,
        UErrorCode &status) અચલ {
    અચલ Cache_ચાવી_Base* the_ચાવી = static_cast<અચલ Cache_ચાવી_Base*>(element->ચાવી.pointer);
    status = the_ચાવી->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<અચલ SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(અચલ UHashElement* element) અચલ {
    UErrorCode status = U_ZERO_ERROR;
    અચલ SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    પાછા result;
}

UBool UnifiedCache::_inProgress(
        અચલ SharedObject* the_કિંમત, UErrorCode creationStatus) અચલ {
    પાછા (the_કિંમત == f_No_કિંમત && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(અચલ UHashElement *element) અચલ
{
    અચલ Cache_ચાવી_Base* the_ચાવી = static_cast<અચલ Cache_ચાવી_Base*>(element->ચાવી.pointer);
    અચલ SharedObject* the_કિંમત = static_cast<અચલ SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    જો (_inProgress(the_કિંમત, the_ચાવી->fCreationStatus)) {
        પાછા ખોટું;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    પાછા (!the_ચાવી->fIsPrimary || (the_કિંમત->soft_Ref_ગણતરી == 1 && the_કિંમત->noHardReferences()));
}

શૂન્ય UnifiedCache::removeSoftRef(અચલ SharedObject *value) અચલ {
    U_ASSERT(value->cachePtr == આ);
    U_ASSERT(value->soft_Ref_ગણતરી > 0);
    જો (--value->soft_Ref_ગણતરી == 0) {
        --f_અંક_કિંમતો_Total;
        જો (value->noHardReferences()) {
            delete value;
        } નહીંતર {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(અચલ SharedObject *value) અચલ {
    પૂર્ણાંક ref_ગણતરી = 0;
    જો (value) {
        ref_ગણતરી = umtx_atomic_dec(&value->hard_Ref_ગણતરી);
        U_ASSERT(ref_ગણતરી >= 0);
        જો (ref_ગણતરી == 0) {
            --f_અંક_કિંમતો_In_Use;
        }
    }
    પાછા ref_ગણતરી;
}

int32_t UnifiedCache::addHardRef(અચલ SharedObject *value) અચલ {
    પૂર્ણાંક ref_ગણતરી = 0;
    જો (value) {
        ref_ગણતરી = umtx_atomic_inc(&value->hard_Ref_ગણતરી);
        U_ASSERT(ref_ગણતરી >= 1);
        જો (ref_ગણતરી == 1) {
            f_અંક_કિંમતો_In_Use++;
        }
    }
    પાછા ref_ગણતરી;
}

U_NAMESPACE_END

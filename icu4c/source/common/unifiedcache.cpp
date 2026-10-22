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

#अन्तर्भाव "unifiedcache.h"

#अन्तर्भाव <algorithm>      // For std::max()
#अन्तर्भाव <mutex>

#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "ucln_cmn.h"

स्थिर icu::UnifiedCache *gCache = nullptr;
स्थिर std::mutex *gCacheMutex = nullptr;
स्थिर std::condition_variable *g_In_Progress_मूल्य_Added_Cond;
स्थिर icu::UInitOnce gCacheInitOnce {};

स्थिर ध्रुव int32_t MAX_EVICT_ITERATIONS = 10;
स्थिर ध्रुव int32_t DEFAULT_MAX_UNUSED = 1000;
स्थिर ध्रुव int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
स्थिर UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_मूल्य_Added_Cond->~condition_variable();
    g_In_Progress_मूल्य_Added_Cond = nullptr;
    प्रत्यागम सत्य;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_कुंजियाँ(ध्रुव UHashTok कुंजी) {
    ध्रुव Cache_कुंजी_Base* ckey = static_cast<ध्रुव Cache_कुंजी_Base*>(कुंजी.pointer);
    प्रत्यागम ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_कुंजियाँ(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    ध्रुव Cache_कुंजी_Base* p1 = static_cast<ध्रुव Cache_कुंजी_Base*>(key1.pointer);
    ध्रुव Cache_कुंजी_Base* p2 = static_cast<ध्रुव Cache_कुंजी_Base*>(key2.pointer);
    प्रत्यागम *p1 == *p2;
}

शून्य U_EXPORT2
ucache_delete_कुंजी(शून्य *obj) {
    Cache_कुंजी_Base* p = static_cast<Cache_कुंजी_Base*>(obj);
    delete p;
}

Cache_कुंजी_Base::~Cache_कुंजी_Base() {
}

स्थिर शून्य U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_मूल्य_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = नूतन UnifiedCache(status);
    यदि (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        प्रत्यागम;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    यदि (U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }
    U_ASSERT(gCache != nullptr);
    प्रत्यागम gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_अंक_मूल्यावली_Total(0),
        f_अंक_मूल्यावली_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_गिनती(0),
        f_No_मूल्य(nullptr) {
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    f_No_मूल्य = नूतन SharedObject();
    यदि (f_No_मूल्य == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    f_No_मूल्य->soft_Ref_गिनती = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_मूल्य->hard_Ref_गिनती = 1;  // when other references to it are removed.
    f_No_मूल्य->cachePtr = एतत्;

    fHashtable = uhash_open(
            &ucache_hash_कुंजियाँ,
            &ucache_compare_कुंजियाँ,
            nullptr,
            &status);
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    uhash_set_कुंजी_Deleter(fHashtable, &ucache_delete_कुंजी);
}

शून्य UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_वस्तुएँ, UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    यदि (count < 0 || percentage_Of_In_Use_वस्तुएँ < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_वस्तुएँ;
}

int32_t UnifiedCache::unused_गिनती() ध्रुव {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    प्रत्यागम uhash_गिनती(fHashtable) - f_अंक_मूल्यावली_In_Use;
}

int64_t UnifiedCache::auto_Evicted_गिनती() ध्रुव {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    प्रत्यागम f_Auto_Evicted_गिनती;
}

int32_t UnifiedCache::कुंजी_गिनती() ध्रुव {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    प्रत्यागम uhash_गिनती(fHashtable);
}

शून्य UnifiedCache::flush() ध्रुव {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    यावत् (_flush(असत्य));
}

शून्य UnifiedCache::handleUnreferencedObject() ध्रुव {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_अंक_मूल्यावली_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#अन्तर्भाव <stdio.h>

शून्य UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    ध्रुव UnifiedCache *cache = getInstance(status);
    यदि (U_FAILURE(status)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        प्रत्यागम;
    }
    cache->dumpContents();
}

शून्य UnifiedCache::dumpContents() ध्रुव {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
शून्य UnifiedCache::_dumpContents() ध्रुव {
    int32_t pos = UHASH_FIRST;
    ध्रुव UHashElement *element = uhash_nextElement(fHashtable, &pos);
    वर्ण buffer[256];
    int32_t cnt = 0;
    प्रति (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        ध्रुव SharedObject *sharedObject =
                (ध्रुव SharedObject *) element->value.pointer;
        ध्रुव Cache_कुंजी_Base *कुंजी =
                (ध्रुव Cache_कुंजी_Base *) element->कुंजी.pointer;
        यदि (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    कुंजी->writeDescription(buffer, 256),
                    कुंजी->creationStatus,
                    sharedObject == f_No_मूल्य ? nullptr :sharedObject,
                    sharedObject->get_Ref_गिनती(),
                    sharedObject->get_Soft_Ref_गिनती());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_गिनती(fHashtable));
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
        _flush(सत्य);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    delete f_No_मूल्य;
    f_No_मूल्य = nullptr;
}

ध्रुव UHashElement *
UnifiedCache::_nextElement() ध्रुव {
    ध्रुव UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    यदि (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        प्रत्यागम uhash_nextElement(fHashtable, &fEvictPos);
    }
    प्रत्यागम element;
}

UBool UnifiedCache::_flush(UBool all) ध्रुव {
    UBool result = असत्य;
    int32_t origSize = uhash_गिनती(fHashtable);
    प्रति (int32_t i = 0; i < origSize; ++i) {
        ध्रुव UHashElement *element = _nextElement();
        यदि (element == nullptr) {
            भङ्ग;
        }
        यदि (all || _isEvictable(element)) {
            ध्रुव SharedObject *sharedObject =
                    static_cast<ध्रुव SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == एतत्);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = सत्य;
        }
    }
    प्रत्यागम result;
}

int32_t UnifiedCache::compute_गिनती_Of_वस्तुएँ_To_Evict() ध्रुव {
    int32_t total_वस्तुएँ = uhash_गिनती(fHashtable);
    int32_t evictable_वस्तुएँ = total_वस्तुएँ - f_अंक_मूल्यावली_In_Use;

    int32_t unusedLimitByPercentage = f_अंक_मूल्यावली_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t गिनती_Of_वस्तुएँ_To_Evict = std::max<int32_t>(0, evictable_वस्तुएँ - unusedLimit);
    प्रत्यागम गिनती_Of_वस्तुएँ_To_Evict;
}

शून्य UnifiedCache::_runEvictionSlice() ध्रुव {
    int32_t max_वस्तुएँ_To_Evict = compute_गिनती_Of_वस्तुएँ_To_Evict();
    यदि (max_वस्तुएँ_To_Evict <= 0) {
        प्रत्यागम;
    }
    प्रति (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ध्रुव UHashElement *element = _nextElement();
        यदि (element == nullptr) {
            भङ्ग;
        }
        यदि (_isEvictable(element)) {
            ध्रुव SharedObject *sharedObject =
                    static_cast<ध्रुव SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_गिनती;
            यदि (--max_वस्तुएँ_To_Evict == 0) {
                भङ्ग;
            }
        }
    }
}

शून्य UnifiedCache::_putNew(
        ध्रुव Cache_कुंजी_Base &कुंजी,
        ध्रुव SharedObject *value,
        ध्रुव UErrorCode creationStatus,
        UErrorCode &status) ध्रुव {
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    Cache_कुंजी_Base *कुंजी_To_Adopt = कुंजी.clone();
    यदि (कुंजी_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    कुंजी_To_Adopt->fCreationStatus = creationStatus;
    यदि (value->soft_Ref_गिनती == 0) {
        _registerPrimary(कुंजी_To_Adopt, value);
    }
    शून्य *old_मूल्य = uhash_put(fHashtable, कुंजी_To_Adopt, (शून्य *) value, &status);
    U_ASSERT(old_मूल्य == nullptr);
    (शून्य)old_मूल्य;
    यदि (U_SUCCESS(status)) {
        value->soft_Ref_गिनती++;
    }
}

शून्य UnifiedCache::_putIfAbsentAndGet(
        ध्रुव Cache_कुंजी_Base &कुंजी,
        ध्रुव SharedObject *&value,
        UErrorCode &status) ध्रुव {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    ध्रुव UHashElement *element = uhash_find(fHashtable, &कुंजी);
    यदि (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        प्रत्यागम;
    }
    यदि (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(कुंजी, value, status, putError);
    } अन्यथा {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        ध्रुव Cache_कुंजी_Base &कुंजी,
        ध्रुव SharedObject *&value,
        UErrorCode &status) ध्रुव {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    ध्रुव UHashElement *element = uhash_find(fHashtable, &कुंजी);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     यावत् (element != nullptr && _inProgress(element)) {
         g_In_Progress_मूल्य_Added_Cond->wait(lock);
         element = uhash_find(fHashtable, &कुंजी);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    यदि (element != nullptr) {
         _fetch(element, value, status);
        प्रत्यागम सत्य;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(कुंजी, f_No_मूल्य, U_ZERO_ERROR, status);
    प्रत्यागम असत्य;
}

शून्य UnifiedCache::_get(
        ध्रुव Cache_कुंजी_Base &कुंजी,
        ध्रुव SharedObject *&value,
        ध्रुव शून्य *creationContext,
        UErrorCode &status) ध्रुव {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    यदि (_poll(कुंजी, value, status)) {
        यदि (value == f_No_मूल्य) {
            SharedObject::clearPtr(value);
        }
        प्रत्यागम;
    }
    यदि (U_FAILURE(status)) {
        प्रत्यागम;
    }
    value = कुंजी.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    यदि (value == nullptr) {
        SharedObject::copyPtr(f_No_मूल्य, value);
    }
    _putIfAbsentAndGet(कुंजी, value, status);
    यदि (value == f_No_मूल्य) {
        SharedObject::clearPtr(value);
    }
}

शून्य UnifiedCache::_registerPrimary(
            ध्रुव Cache_कुंजी_Base *the_कुंजी, ध्रुव SharedObject *value) ध्रुव {
    the_कुंजी->fIsPrimary = सत्य;
    value->cachePtr = एतत्;
    ++f_अंक_मूल्यावली_Total;
    ++f_अंक_मूल्यावली_In_Use;
}

शून्य UnifiedCache::_put(
        ध्रुव UHashElement *element,
        ध्रुव SharedObject *value,
        ध्रुव UErrorCode status) ध्रुव {
    U_ASSERT(_inProgress(element));
    ध्रुव Cache_कुंजी_Base* the_कुंजी = static_cast<ध्रुव Cache_कुंजी_Base*>(element->कुंजी.pointer);
    ध्रुव SharedObject* old_मूल्य = static_cast<ध्रुव SharedObject*>(element->value.pointer);
    the_कुंजी->fCreationStatus = status;
    यदि (value->soft_Ref_गिनती == 0) {
        _registerPrimary(the_कुंजी, value);
    }
    value->soft_Ref_गिनती++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (शून्य *) value;
    U_ASSERT(old_मूल्य == f_No_मूल्य);
    removeSoftRef(old_मूल्य);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_मूल्य_Added_Cond->notify_all();
}

शून्य UnifiedCache::_fetch(
        ध्रुव UHashElement *element,
        ध्रुव SharedObject *&value,
        UErrorCode &status) ध्रुव {
    ध्रुव Cache_कुंजी_Base* the_कुंजी = static_cast<ध्रुव Cache_कुंजी_Base*>(element->कुंजी.pointer);
    status = the_कुंजी->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ध्रुव SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(ध्रुव UHashElement* element) ध्रुव {
    UErrorCode status = U_ZERO_ERROR;
    ध्रुव SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    प्रत्यागम result;
}

UBool UnifiedCache::_inProgress(
        ध्रुव SharedObject* the_मूल्य, UErrorCode creationStatus) ध्रुव {
    प्रत्यागम (the_मूल्य == f_No_मूल्य && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ध्रुव UHashElement *element) ध्रुव
{
    ध्रुव Cache_कुंजी_Base* the_कुंजी = static_cast<ध्रुव Cache_कुंजी_Base*>(element->कुंजी.pointer);
    ध्रुव SharedObject* the_मूल्य = static_cast<ध्रुव SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    यदि (_inProgress(the_मूल्य, the_कुंजी->fCreationStatus)) {
        प्रत्यागम असत्य;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    प्रत्यागम (!the_कुंजी->fIsPrimary || (the_मूल्य->soft_Ref_गिनती == 1 && the_मूल्य->noHardReferences()));
}

शून्य UnifiedCache::removeSoftRef(ध्रुव SharedObject *value) ध्रुव {
    U_ASSERT(value->cachePtr == एतत्);
    U_ASSERT(value->soft_Ref_गिनती > 0);
    यदि (--value->soft_Ref_गिनती == 0) {
        --f_अंक_मूल्यावली_Total;
        यदि (value->noHardReferences()) {
            delete value;
        } अन्यथा {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ध्रुव SharedObject *value) ध्रुव {
    पूर्णाङ्क ref_गिनती = 0;
    यदि (value) {
        ref_गिनती = umtx_atomic_dec(&value->hard_Ref_गिनती);
        U_ASSERT(ref_गिनती >= 0);
        यदि (ref_गिनती == 0) {
            --f_अंक_मूल्यावली_In_Use;
        }
    }
    प्रत्यागम ref_गिनती;
}

int32_t UnifiedCache::addHardRef(ध्रुव SharedObject *value) ध्रुव {
    पूर्णाङ्क ref_गिनती = 0;
    यदि (value) {
        ref_गिनती = umtx_atomic_inc(&value->hard_Ref_गिनती);
        U_ASSERT(ref_गिनती >= 1);
        यदि (ref_गिनती == 1) {
            f_अंक_मूल्यावली_In_Use++;
        }
    }
    प्रत्यागम ref_गिनती;
}

U_NAMESPACE_END

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

#ਅਨ੍ਤਰ੍ਭਾਵ "unifiedcache.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>      // For std::max()
#ਅਨ੍ਤਰ੍ਭਾਵ <mutex>

#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"

ਸਥਿਰ icu::UnifiedCache *gCache = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ std::mutex *gCacheMutex = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ std::condition_variable *g_ਵਿੱਚ_ਤਰੱਕੀ_ਮੁੱਲ_Added_Cond;
ਸਥਿਰ icu::UInitOnce gCacheInitOnce {};

ਸਥਿਰ ਅਚਲ int32_t MAX_EVICT_ITERATIONS = 10;
ਸਥਿਰ ਅਚਲ int32_t DEFAULT_MAX_ਅਣਵਰਤਿਆ = 1000;
ਸਥਿਰ ਅਚਲ int32_t DEFAULT_PERCENTAGE_ਦਾ_ਵਿੱਚ_USE = 100;


U_CDECL_BEGIN
ਸਥਿਰ UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    ਮਿਟਾਓ gCache;
    gCache = ਖਾਲੀ_ਸੂਚਕ;
    gCacheMutex->~mutex();
    gCacheMutex = ਖਾਲੀ_ਸੂਚਕ;
    g_ਵਿੱਚ_ਤਰੱਕੀ_ਮੁੱਲ_Added_Cond->~condition_variable();
    g_ਵਿੱਚ_ਤਰੱਕੀ_ਮੁੱਲ_Added_Cond = ਖਾਲੀ_ਸੂਚਕ;
    ਵਾਪਸ ਸੱਚ;
}
U_CDECL_ਸਿਰਾ


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_ਕੁੰਜੀਆਂ(ਅਚਲ UHashTok ਕੁੰਜੀ) {
    ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ* ckey = static_cast<ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ*>(ਕੁੰਜੀ.ਸੂਚਕ);
    ਵਾਪਸ ckey->hash_ਕੋਡ();
}

UBool U_EXPORT2
ucache_compare_ਕੁੰਜੀਆਂ(ਅਚਲ UHashTok key1, ਅਚਲ UHashTok key2) {
    ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ* p1 = static_cast<ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ*>(key1.ਸੂਚਕ);
    ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ* p2 = static_cast<ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ*>(key2.ਸੂਚਕ);
    ਵਾਪਸ *p1 == *p2;
}

ਖਾਲੀ U_EXPORT2
ucache_delete_ਕੁੰਜੀ(ਖਾਲੀ *obj) {
    Cache_ਕੁੰਜੀ_ਅਧਾਰ* p = static_cast<Cache_ਕੁੰਜੀ_ਅਧਾਰ*>(obj);
    ਮਿਟਾਓ p;
}

Cache_ਕੁੰਜੀ_ਅਧਾਰ::~Cache_ਕੁੰਜੀ_ਅਧਾਰ() {
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV cacheInit(UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(gCache == ਖਾਲੀ_ਸੂਚਕ);
    ucln_common_ਰਜਿਸਟਰ_Cleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_ਵਿੱਚ_ਤਰੱਕੀ_ਮੁੱਲ_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = ਨਵਾਂ UnifiedCache(ਸਥਿਤੀ);
    ਜੇ (gCache == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਮਿਟਾਓ gCache;
        gCache = ਖਾਲੀ_ਸੂਚਕ;
        ਵਾਪਸ;
    }
}

UnifiedCache *UnifiedCache::getInstance(UError_ਕੋਡ &ਸਥਿਤੀ) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    U_ASSERT(gCache != ਖਾਲੀ_ਸੂਚਕ);
    ਵਾਪਸ gCache;
}

UnifiedCache::UnifiedCache(UError_ਕੋਡ &ਸਥਿਤੀ) :
        fHashtable(ਖਾਲੀ_ਸੂਚਕ),
        fEvictPos(UHASH_ਪਹਿਲਾ),
        f_ਅੰਕ_ਮੁੱਲਾਂ_ਕੁੱਲ(0),
        f_ਅੰਕ_ਮੁੱਲਾਂ_ਵਿੱਚ_Use(0),
        f_Max_ਅਣਵਰਤਿਆ(DEFAULT_MAX_ਅਣਵਰਤਿਆ),
        f_Max_Percentage_ਦਾ_ਵਿੱਚ_Use(DEFAULT_PERCENTAGE_ਦਾ_ਵਿੱਚ_USE),
        f_Auto_Evicted_ਗਿਣਤੀ(0),
        f_No_ਮੁੱਲ(ਖਾਲੀ_ਸੂਚਕ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    f_No_ਮੁੱਲ = ਨਵਾਂ SharedObject();
    ਜੇ (f_No_ਮੁੱਲ == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    f_No_ਮੁੱਲ->soft_Ref_ਗਿਣਤੀ = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_ਮੁੱਲ->hard_Ref_ਗਿਣਤੀ = 1;  // when other references to it are removed.
    f_No_ਮੁੱਲ->cachePtr = ਇਹ;

    fHashtable = uhash_open(
            &ucache_hash_ਕੁੰਜੀਆਂ,
            &ucache_compare_ਕੁੰਜੀਆਂ,
            ਖਾਲੀ_ਸੂਚਕ,
            &ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    uhash_ਸੈੱਟ_ਕੁੰਜੀ_Deleter(fHashtable, &ucache_delete_ਕੁੰਜੀ);
}

ਖਾਲੀ UnifiedCache::ਸੈੱਟ_Eviction_Policy(
        int32_t count, int32_t percentage_ਦਾ_ਵਿੱਚ_Use_ਚੀਜ਼ਾਂ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    ਜੇ (count < 0 || percentage_ਦਾ_ਵਿੱਚ_Use_ਚੀਜ਼ਾਂ < 0) {
        ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }
    std::lock_guard<std::mutex> ਤਾਲਾ(*gCacheMutex);
    f_Max_ਅਣਵਰਤਿਆ = count;
    f_Max_Percentage_ਦਾ_ਵਿੱਚ_Use = percentage_ਦਾ_ਵਿੱਚ_Use_ਚੀਜ਼ਾਂ;
}

int32_t UnifiedCache::ਅਣਵਰਤਿਆ_ਗਿਣਤੀ() ਅਚਲ {
    std::lock_guard<std::mutex> ਤਾਲਾ(*gCacheMutex);
    ਵਾਪਸ uhash_ਗਿਣਤੀ(fHashtable) - f_ਅੰਕ_ਮੁੱਲਾਂ_ਵਿੱਚ_Use;
}

int64_t UnifiedCache::auto_Evicted_ਗਿਣਤੀ() ਅਚਲ {
    std::lock_guard<std::mutex> ਤਾਲਾ(*gCacheMutex);
    ਵਾਪਸ f_Auto_Evicted_ਗਿਣਤੀ;
}

int32_t UnifiedCache::ਕੁੰਜੀ_ਗਿਣਤੀ() ਅਚਲ {
    std::lock_guard<std::mutex> ਤਾਲਾ(*gCacheMutex);
    ਵਾਪਸ uhash_ਗਿਣਤੀ(fHashtable);
}

ਖਾਲੀ UnifiedCache::flush() ਅਚਲ {
    std::lock_guard<std::mutex> ਤਾਲਾ(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    ਜਦੋਂ (_flush(ਝੂਠ));
}

ਖਾਲੀ UnifiedCache::handleUnreferencedObject() ਅਚਲ {
    std::lock_guard<std::mutex> ਤਾਲਾ(*gCacheMutex);
    --f_ਅੰਕ_ਮੁੱਲਾਂ_ਵਿੱਚ_Use;
    ਲੜੀਵਾਰ_Eviction_ਟੁਕੜਾ();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIFIED_CACHE_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

ਖਾਲੀ UnifiedCache::dump() {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ਅਚਲ UnifiedCache *cache = getInstance(ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        ਵਾਪਸ;
    }
    cache->dumpContents();
}

ਖਾਲੀ UnifiedCache::dumpContents() ਅਚਲ {
    std::lock_guard<std::mutex> ਤਾਲਾ(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
ਖਾਲੀ UnifiedCache::_dumpContents() ਅਚਲ {
    int32_t pos = UHASH_ਪਹਿਲਾ;
    ਅਚਲ UHashElement *element = uhash_ਅਗਲਾ_Element(fHashtable, &pos);
    ਅੱਖਰ ਬਫਰ[256];
    int32_t cnt = 0;
    ਲਈ (; element != ਖਾਲੀ_ਸੂਚਕ; element = uhash_ਅਗਲਾ_Element(fHashtable, &pos)) {
        ਅਚਲ SharedObject *sharedObject =
                (ਅਚਲ SharedObject *) element->value.ਸੂਚਕ;
        ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ *ਕੁੰਜੀ =
                (ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ *) element->ਕੁੰਜੀ.ਸੂਚਕ;
        ਜੇ (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    ਕੁੰਜੀ->writeDescription(ਬਫਰ, 256),
                    ਕੁੰਜੀ->creation_ਸਥਿਤੀ,
                    sharedObject == f_No_ਮੁੱਲ ? ਖਾਲੀ_ਸੂਚਕ :sharedObject,
                    sharedObject->get_Ref_ਗਿਣਤੀ(),
                    sharedObject->get_Soft_Ref_ਗਿਣਤੀ());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_ਗਿਣਤੀ(fHashtable));
}
#ਜੇ_ਅੰਤ

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> ਤਾਲਾ(*gCacheMutex);
        _flush(ਸੱਚ);
    }
    uhash_close(fHashtable);
    fHashtable = ਖਾਲੀ_ਸੂਚਕ;
    ਮਿਟਾਓ f_No_ਮੁੱਲ;
    f_No_ਮੁੱਲ = ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ UHashElement *
UnifiedCache::ਅਗਲਾ_Element_2() ਅਚਲ {
    ਅਚਲ UHashElement *element = uhash_ਅਗਲਾ_Element(fHashtable, &fEvictPos);
    ਜੇ (element == ਖਾਲੀ_ਸੂਚਕ) {
        fEvictPos = UHASH_ਪਹਿਲਾ;
        ਵਾਪਸ uhash_ਅਗਲਾ_Element(fHashtable, &fEvictPos);
    }
    ਵਾਪਸ element;
}

UBool UnifiedCache::_flush(UBool all) ਅਚਲ {
    UBool ਨਤੀਜਾ = ਝੂਠ;
    int32_t orig_ਮਾਪ = uhash_ਗਿਣਤੀ(fHashtable);
    ਲਈ (int32_t i = 0; i < orig_ਮਾਪ; ++i) {
        ਅਚਲ UHashElement *element = ਅਗਲਾ_Element_2();
        ਜੇ (element == ਖਾਲੀ_ਸੂਚਕ) {
            ਰੋਕੋ;
        }
        ਜੇ (all || _isEvictable(element)) {
            ਅਚਲ SharedObject *sharedObject =
                    static_cast<ਅਚਲ SharedObject*>(element->value.ਸੂਚਕ);
            U_ASSERT(sharedObject->cachePtr == ਇਹ);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            ਨਤੀਜਾ = ਸੱਚ;
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

int32_t UnifiedCache::compute_ਗਿਣਤੀ_ਦਾ_ਚੀਜ਼ਾਂ_ਮੰਜ਼ਿਲ_Evict() ਅਚਲ {
    int32_t ਕੁੱਲ_ਚੀਜ਼ਾਂ = uhash_ਗਿਣਤੀ(fHashtable);
    int32_t evictable_ਚੀਜ਼ਾਂ = ਕੁੱਲ_ਚੀਜ਼ਾਂ - f_ਅੰਕ_ਮੁੱਲਾਂ_ਵਿੱਚ_Use;

    int32_t ਅਣਵਰਤਿਆ_ਹੱਦਬੰਦੀ_By_Percentage = f_ਅੰਕ_ਮੁੱਲਾਂ_ਵਿੱਚ_Use * f_Max_Percentage_ਦਾ_ਵਿੱਚ_Use / 100;
    int32_t ਅਣਵਰਤਿਆ_ਹੱਦਬੰਦੀ = std::max(ਅਣਵਰਤਿਆ_ਹੱਦਬੰਦੀ_By_Percentage, f_Max_ਅਣਵਰਤਿਆ);
    int32_t ਗਿਣਤੀ_ਦਾ_ਚੀਜ਼ਾਂ_ਮੰਜ਼ਿਲ_Evict = std::max<int32_t>(0, evictable_ਚੀਜ਼ਾਂ - ਅਣਵਰਤਿਆ_ਹੱਦਬੰਦੀ);
    ਵਾਪਸ ਗਿਣਤੀ_ਦਾ_ਚੀਜ਼ਾਂ_ਮੰਜ਼ਿਲ_Evict;
}

ਖਾਲੀ UnifiedCache::ਲੜੀਵਾਰ_Eviction_ਟੁਕੜਾ() ਅਚਲ {
    int32_t max_ਚੀਜ਼ਾਂ_ਮੰਜ਼ਿਲ_Evict = compute_ਗਿਣਤੀ_ਦਾ_ਚੀਜ਼ਾਂ_ਮੰਜ਼ਿਲ_Evict();
    ਜੇ (max_ਚੀਜ਼ਾਂ_ਮੰਜ਼ਿਲ_Evict <= 0) {
        ਵਾਪਸ;
    }
    ਲਈ (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ਅਚਲ UHashElement *element = ਅਗਲਾ_Element_2();
        ਜੇ (element == ਖਾਲੀ_ਸੂਚਕ) {
            ਰੋਕੋ;
        }
        ਜੇ (_isEvictable(element)) {
            ਅਚਲ SharedObject *sharedObject =
                    static_cast<ਅਚਲ SharedObject*>(element->value.ਸੂਚਕ);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_ਗਿਣਤੀ;
            ਜੇ (--max_ਚੀਜ਼ਾਂ_ਮੰਜ਼ਿਲ_Evict == 0) {
                ਰੋਕੋ;
            }
        }
    }
}

ਖਾਲੀ UnifiedCache::_putNew(
        ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &ਕੁੰਜੀ,
        ਅਚਲ SharedObject *value,
        ਅਚਲ UError_ਕੋਡ creation_ਸਥਿਤੀ,
        UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    Cache_ਕੁੰਜੀ_ਅਧਾਰ *ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_Adopt = ਕੁੰਜੀ.clone();
    ਜੇ (ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_Adopt == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_Adopt->f_Creation_ਸਥਿਤੀ = creation_ਸਥਿਤੀ;
    ਜੇ (value->soft_Ref_ਗਿਣਤੀ == 0) {
        ਰਜਿਸਟਰ_Primary(ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_Adopt, value);
    }
    ਖਾਲੀ *old_ਮੁੱਲ = uhash_put(fHashtable, ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_Adopt, (ਖਾਲੀ *) value, &ਸਥਿਤੀ);
    U_ASSERT(old_ਮੁੱਲ == ਖਾਲੀ_ਸੂਚਕ);
    (ਖਾਲੀ)old_ਮੁੱਲ;
    ਜੇ (U_SUCCESS(ਸਥਿਤੀ)) {
        value->soft_Ref_ਗਿਣਤੀ++;
    }
}

ਖਾਲੀ UnifiedCache::_putIfAbsentAndGet(
        ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &ਕੁੰਜੀ,
        ਅਚਲ SharedObject *&value,
        UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    std::lock_guard<std::mutex> ਤਾਲਾ(*gCacheMutex);
    ਅਚਲ UHashElement *element = uhash_ਲੱਭੋ(fHashtable, &ਕੁੰਜੀ);
    ਜੇ (element != ਖਾਲੀ_ਸੂਚਕ && !ਵਿੱਚ_ਤਰੱਕੀ(element)) {
        _fetch(element, value, ਸਥਿਤੀ);
        ਵਾਪਸ;
    }
    ਜੇ (element == ਖਾਲੀ_ਸੂਚਕ) {
        UError_ਕੋਡ putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(ਕੁੰਜੀ, value, ਸਥਿਤੀ, putError);
    } ਨਹੀਂਤਾਂ {
        _put(element, value, ਸਥਿਤੀ);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    ਲੜੀਵਾਰ_Eviction_ਟੁਕੜਾ();
}


UBool UnifiedCache::_poll(
        ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &ਕੁੰਜੀ,
        ਅਚਲ SharedObject *&value,
        UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    U_ASSERT(value == ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(ਸਥਿਤੀ == U_ZERO_ERROR);
    std::unique_lock<std::mutex> ਤਾਲਾ(*gCacheMutex);
    ਅਚਲ UHashElement *element = uhash_ਲੱਭੋ(fHashtable, &ਕੁੰਜੀ);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     ਜਦੋਂ (element != ਖਾਲੀ_ਸੂਚਕ && ਵਿੱਚ_ਤਰੱਕੀ(element)) {
         g_ਵਿੱਚ_ਤਰੱਕੀ_ਮੁੱਲ_Added_Cond->wait(ਤਾਲਾ);
         element = uhash_ਲੱਭੋ(fHashtable, &ਕੁੰਜੀ);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    ਜੇ (element != ਖਾਲੀ_ਸੂਚਕ) {
         _fetch(element, value, ਸਥਿਤੀ);
        ਵਾਪਸ ਸੱਚ;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(ਕੁੰਜੀ, f_No_ਮੁੱਲ, U_ZERO_ERROR, ਸਥਿਤੀ);
    ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ UnifiedCache::_get(
        ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &ਕੁੰਜੀ,
        ਅਚਲ SharedObject *&value,
        ਅਚਲ ਖਾਲੀ *creationContext,
        UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    U_ASSERT(value == ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(ਸਥਿਤੀ == U_ZERO_ERROR);
    ਜੇ (_poll(ਕੁੰਜੀ, value, ਸਥਿਤੀ)) {
        ਜੇ (value == f_No_ਮੁੱਲ) {
            SharedObject::clearPtr(value);
        }
        ਵਾਪਸ;
    }
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    value = ਕੁੰਜੀ.createObject(creationContext, ਸਥਿਤੀ);
    U_ASSERT(value == ਖਾਲੀ_ਸੂਚਕ || value->hasHardReferences());
    U_ASSERT(value != ਖਾਲੀ_ਸੂਚਕ || ਸਥਿਤੀ != U_ZERO_ERROR);
    ਜੇ (value == ਖਾਲੀ_ਸੂਚਕ) {
        SharedObject::copyPtr(f_No_ਮੁੱਲ, value);
    }
    _putIfAbsentAndGet(ਕੁੰਜੀ, value, ਸਥਿਤੀ);
    ਜੇ (value == f_No_ਮੁੱਲ) {
        SharedObject::clearPtr(value);
    }
}

ਖਾਲੀ UnifiedCache::ਰਜਿਸਟਰ_Primary(
            ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ *the_ਕੁੰਜੀ, ਅਚਲ SharedObject *value) ਅਚਲ {
    the_ਕੁੰਜੀ->fIsPrimary = ਸੱਚ;
    value->cachePtr = ਇਹ;
    ++f_ਅੰਕ_ਮੁੱਲਾਂ_ਕੁੱਲ;
    ++f_ਅੰਕ_ਮੁੱਲਾਂ_ਵਿੱਚ_Use;
}

ਖਾਲੀ UnifiedCache::_put(
        ਅਚਲ UHashElement *element,
        ਅਚਲ SharedObject *value,
        ਅਚਲ UError_ਕੋਡ ਸਥਿਤੀ) ਅਚਲ {
    U_ASSERT(ਵਿੱਚ_ਤਰੱਕੀ(element));
    ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ* the_ਕੁੰਜੀ = static_cast<ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ*>(element->ਕੁੰਜੀ.ਸੂਚਕ);
    ਅਚਲ SharedObject* old_ਮੁੱਲ = static_cast<ਅਚਲ SharedObject*>(element->value.ਸੂਚਕ);
    the_ਕੁੰਜੀ->f_Creation_ਸਥਿਤੀ = ਸਥਿਤੀ;
    ਜੇ (value->soft_Ref_ਗਿਣਤੀ == 0) {
        ਰਜਿਸਟਰ_Primary(the_ਕੁੰਜੀ, value);
    }
    value->soft_Ref_ਗਿਣਤੀ++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.ਸੂਚਕ = (ਖਾਲੀ *) value;
    U_ASSERT(old_ਮੁੱਲ == f_No_ਮੁੱਲ);
    removeSoftRef(old_ਮੁੱਲ);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_ਵਿੱਚ_ਤਰੱਕੀ_ਮੁੱਲ_Added_Cond->notify_all();
}

ਖਾਲੀ UnifiedCache::_fetch(
        ਅਚਲ UHashElement *element,
        ਅਚਲ SharedObject *&value,
        UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
    ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ* the_ਕੁੰਜੀ = static_cast<ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ*>(element->ਕੁੰਜੀ.ਸੂਚਕ);
    ਸਥਿਤੀ = the_ਕੁੰਜੀ->f_Creation_ਸਥਿਤੀ;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ਅਚਲ SharedObject *>(element->value.ਸੂਚਕ);
    ਜੋੜੋ_Hard_Ref(value);
}


UBool UnifiedCache::ਵਿੱਚ_ਤਰੱਕੀ(ਅਚਲ UHashElement* element) ਅਚਲ {
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ਅਚਲ SharedObject * value = ਖਾਲੀ_ਸੂਚਕ;
    _fetch(element, value, ਸਥਿਤੀ);
    UBool ਨਤੀਜਾ = ਵਿੱਚ_ਤਰੱਕੀ(value, ਸਥਿਤੀ);
    removeHardRef(value);
    ਵਾਪਸ ਨਤੀਜਾ;
}

UBool UnifiedCache::ਵਿੱਚ_ਤਰੱਕੀ(
        ਅਚਲ SharedObject* the_ਮੁੱਲ, UError_ਕੋਡ creation_ਸਥਿਤੀ) ਅਚਲ {
    ਵਾਪਸ (the_ਮੁੱਲ == f_No_ਮੁੱਲ && creation_ਸਥਿਤੀ == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ਅਚਲ UHashElement *element) ਅਚਲ
{
    ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ* the_ਕੁੰਜੀ = static_cast<ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ*>(element->ਕੁੰਜੀ.ਸੂਚਕ);
    ਅਚਲ SharedObject* the_ਮੁੱਲ = static_cast<ਅਚਲ SharedObject*>(element->value.ਸੂਚਕ);

    // Entries that are under construction are never evictable
    ਜੇ (ਵਿੱਚ_ਤਰੱਕੀ(the_ਮੁੱਲ, the_ਕੁੰਜੀ->f_Creation_ਸਥਿਤੀ)) {
        ਵਾਪਸ ਝੂਠ;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    ਵਾਪਸ (!the_ਕੁੰਜੀ->fIsPrimary || (the_ਮੁੱਲ->soft_Ref_ਗਿਣਤੀ == 1 && the_ਮੁੱਲ->noHardReferences()));
}

ਖਾਲੀ UnifiedCache::removeSoftRef(ਅਚਲ SharedObject *value) ਅਚਲ {
    U_ASSERT(value->cachePtr == ਇਹ);
    U_ASSERT(value->soft_Ref_ਗਿਣਤੀ > 0);
    ਜੇ (--value->soft_Ref_ਗਿਣਤੀ == 0) {
        --f_ਅੰਕ_ਮੁੱਲਾਂ_ਕੁੱਲ;
        ਜੇ (value->noHardReferences()) {
            ਮਿਟਾਓ value;
        } ਨਹੀਂਤਾਂ {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = ਖਾਲੀ_ਸੂਚਕ;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ਅਚਲ SharedObject *value) ਅਚਲ {
    ਪੂਰਨ ref_ਗਿਣਤੀ = 0;
    ਜੇ (value) {
        ref_ਗਿਣਤੀ = umtx_atomic_dec(&value->hard_Ref_ਗਿਣਤੀ);
        U_ASSERT(ref_ਗਿਣਤੀ >= 0);
        ਜੇ (ref_ਗਿਣਤੀ == 0) {
            --f_ਅੰਕ_ਮੁੱਲਾਂ_ਵਿੱਚ_Use;
        }
    }
    ਵਾਪਸ ref_ਗਿਣਤੀ;
}

int32_t UnifiedCache::ਜੋੜੋ_Hard_Ref(ਅਚਲ SharedObject *value) ਅਚਲ {
    ਪੂਰਨ ref_ਗਿਣਤੀ = 0;
    ਜੇ (value) {
        ref_ਗਿਣਤੀ = umtx_atomic_inc(&value->hard_Ref_ਗਿਣਤੀ);
        U_ASSERT(ref_ਗਿਣਤੀ >= 1);
        ਜੇ (ref_ਗਿਣਤੀ == 1) {
            f_ਅੰਕ_ਮੁੱਲਾਂ_ਵਿੱਚ_Use++;
        }
    }
    ਵਾਪਸ ref_ਗਿਣਤੀ;
}

U_NAMESPACE_ਸਿਰਾ

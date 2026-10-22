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

#ಅನ್ತರ್ಭಾವ "unifiedcache.h"

#ಅನ್ತರ್ಭಾವ <algorithm>      // For std::max()
#ಅನ್ತರ್ಭಾವ <mutex>

#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"

ಸ್ಥಿರ icu::UnifiedCache *gCache = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ std::mutex *gCacheMutex = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ std::condition_variable *g_ಒಳ_ಪ್ರಗತಿ_ಮೌಲ್ಯ_Added_Cond;
ಸ್ಥಿರ icu::UInitOnce gCacheInitOnce {};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t MAX_EVICT_ITERATIONS = 10;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t DEFAULT_MAX_ಬಳಸದ = 1000;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t DEFAULT_PERCENTAGE_ದ_ಒಳ_USE = 100;


U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    ಅಳಿಸು gCache;
    gCache = ಶೂನ್ಯಸೂಚಕ;
    gCacheMutex->~mutex();
    gCacheMutex = ಶೂನ್ಯಸೂಚಕ;
    g_ಒಳ_ಪ್ರಗತಿ_ಮೌಲ್ಯ_Added_Cond->~condition_variable();
    g_ಒಳ_ಪ್ರಗತಿ_ಮೌಲ್ಯ_Added_Cond = ಶೂನ್ಯಸೂಚಕ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}
U_CDECL_ಕೊನೆ


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_ಕೀಲಿಗಳು(ಸ್ಥಿರಾಂಕ UHashTok ಕೀಲಿ) {
    ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ* ckey = static_cast<ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ*>(ಕೀಲಿ.ಸೂಚಕ);
    ಹಿಂತಿರುಗಿ ckey->hash_ಸಂಕೇತ();
}

UBool U_EXPORT2
ucache_compare_ಕೀಲಿಗಳು(ಸ್ಥಿರಾಂಕ UHashTok key1, ಸ್ಥಿರಾಂಕ UHashTok key2) {
    ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ* p1 = static_cast<ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ*>(key1.ಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ* p2 = static_cast<ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ*>(key2.ಸೂಚಕ);
    ಹಿಂತಿರುಗಿ *p1 == *p2;
}

ಶೂನ್ಯ U_EXPORT2
ucache_delete_ಕೀಲಿ(ಶೂನ್ಯ *obj) {
    Cache_ಕೀಲಿ_ತಳ* p = static_cast<Cache_ಕೀಲಿ_ತಳ*>(obj);
    ಅಳಿಸು p;
}

Cache_ಕೀಲಿ_ತಳ::~Cache_ಕೀಲಿ_ತಳ() {
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV cacheInit(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(gCache == ಶೂನ್ಯಸೂಚಕ);
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_ಒಳ_ಪ್ರಗತಿ_ಮೌಲ್ಯ_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = ಹೊಸ UnifiedCache(ಸ್ಥಿತಿ_2);
    ಆದರೆ (gCache == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಅಳಿಸು gCache;
        gCache = ಶೂನ್ಯಸೂಚಕ;
        ಹಿಂತಿರುಗಿ;
    }
}

UnifiedCache *UnifiedCache::getInstance(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    U_ASSERT(gCache != ಶೂನ್ಯಸೂಚಕ);
    ಹಿಂತಿರುಗಿ gCache;
}

UnifiedCache::UnifiedCache(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) :
        fHashtable(ಶೂನ್ಯಸೂಚಕ),
        fEvictPos(UHASH_ಮೊದಲ),
        f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಟ್ಟು(0),
        f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಳ_Use(0),
        f_Max_ಬಳಸದ(DEFAULT_MAX_ಬಳಸದ),
        f_Max_Percentage_ದ_ಒಳ_Use(DEFAULT_PERCENTAGE_ದ_ಒಳ_USE),
        f_Auto_Evicted_ಎಣಿಕೆ(0),
        f_No_ಮೌಲ್ಯ(ಶೂನ್ಯಸೂಚಕ) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    f_No_ಮೌಲ್ಯ = ಹೊಸ SharedObject();
    ಆದರೆ (f_No_ಮೌಲ್ಯ == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    f_No_ಮೌಲ್ಯ->soft_Ref_ಎಣಿಕೆ = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_ಮೌಲ್ಯ->hard_Ref_ಎಣಿಕೆ = 1;  // when other references to it are removed.
    f_No_ಮೌಲ್ಯ->cachePtr = ಇದು;

    fHashtable = uhash_open(
            &ucache_hash_ಕೀಲಿಗಳು,
            &ucache_compare_ಕೀಲಿಗಳು,
            ಶೂನ್ಯಸೂಚಕ,
            &ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    uhash_ಗಣ_ಕೀಲಿ_Deleter(fHashtable, &ucache_delete_ಕೀಲಿ);
}

ಶೂನ್ಯ UnifiedCache::ಗಣ_Eviction_Policy(
        int32_t count, int32_t percentage_ದ_ಒಳ_Use_ವಸ್ತುಗಳು, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (count < 0 || percentage_ದ_ಒಳ_Use_ವಸ್ತುಗಳು < 0) {
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    std::lock_guard<std::mutex> ಬೀಗ(*gCacheMutex);
    f_Max_ಬಳಸದ = count;
    f_Max_Percentage_ದ_ಒಳ_Use = percentage_ದ_ಒಳ_Use_ವಸ್ತುಗಳು;
}

int32_t UnifiedCache::ಬಳಸದ_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ {
    std::lock_guard<std::mutex> ಬೀಗ(*gCacheMutex);
    ಹಿಂತಿರುಗಿ uhash_ಎಣಿಕೆ(fHashtable) - f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಳ_Use;
}

int64_t UnifiedCache::auto_Evicted_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ {
    std::lock_guard<std::mutex> ಬೀಗ(*gCacheMutex);
    ಹಿಂತಿರುಗಿ f_Auto_Evicted_ಎಣಿಕೆ;
}

int32_t UnifiedCache::ಕೀಲಿ_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ {
    std::lock_guard<std::mutex> ಬೀಗ(*gCacheMutex);
    ಹಿಂತಿರುಗಿ uhash_ಎಣಿಕೆ(fHashtable);
}

ಶೂನ್ಯ UnifiedCache::flush() ಸ್ಥಿರಾಂಕ {
    std::lock_guard<std::mutex> ಬೀಗ(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    ಆಗ (_flush(ಸುಳ್ಳು));
}

ಶೂನ್ಯ UnifiedCache::handleUnreferencedObject() ಸ್ಥಿರಾಂಕ {
    std::lock_guard<std::mutex> ಬೀಗ(*gCacheMutex);
    --f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಳ_Use;
    ಸತತ_Eviction_ತುಂಡು();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIFIED_CACHE_DEBUG
#ಅನ್ತರ್ಭಾವ <stdio.h>

ಶೂನ್ಯ UnifiedCache::dump() {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ UnifiedCache *cache = getInstance(ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        ಹಿಂತಿರುಗಿ;
    }
    cache->dumpContents();
}

ಶೂನ್ಯ UnifiedCache::dumpContents() ಸ್ಥಿರಾಂಕ {
    std::lock_guard<std::mutex> ಬೀಗ(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
ಶೂನ್ಯ UnifiedCache::_dumpContents() ಸ್ಥಿರಾಂಕ {
    int32_t pos = UHASH_ಮೊದಲ;
    ಸ್ಥಿರಾಂಕ UHashElement *element = uhash_ಮುಂದಿನ_Element(fHashtable, &pos);
    ಅಕ್ಷರ ಬಫರ್[256];
    int32_t cnt = 0;
    ಗಾಗಿ (; element != ಶೂನ್ಯಸೂಚಕ; element = uhash_ಮುಂದಿನ_Element(fHashtable, &pos)) {
        ಸ್ಥಿರಾಂಕ SharedObject *sharedObject =
                (ಸ್ಥಿರಾಂಕ SharedObject *) element->value.ಸೂಚಕ;
        ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ *ಕೀಲಿ =
                (ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ *) element->ಕೀಲಿ.ಸೂಚಕ;
        ಆದರೆ (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    ಕೀಲಿ->writeDescription(ಬಫರ್, 256),
                    ಕೀಲಿ->creation_ಸ್ಥಿತಿ,
                    sharedObject == f_No_ಮೌಲ್ಯ ? ಶೂನ್ಯಸೂಚಕ :sharedObject,
                    sharedObject->get_Ref_ಎಣಿಕೆ(),
                    sharedObject->get_Soft_Ref_ಎಣಿಕೆ());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_ಎಣಿಕೆ(fHashtable));
}
#ಆದರೆ_ಅಂತ್ಯ

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> ಬೀಗ(*gCacheMutex);
        _flush(ನಿಜ);
    }
    uhash_close(fHashtable);
    fHashtable = ಶೂನ್ಯಸೂಚಕ;
    ಅಳಿಸು f_No_ಮೌಲ್ಯ;
    f_No_ಮೌಲ್ಯ = ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ UHashElement *
UnifiedCache::ಮುಂದಿನ_Element_2() ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ UHashElement *element = uhash_ಮುಂದಿನ_Element(fHashtable, &fEvictPos);
    ಆದರೆ (element == ಶೂನ್ಯಸೂಚಕ) {
        fEvictPos = UHASH_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ uhash_ಮುಂದಿನ_Element(fHashtable, &fEvictPos);
    }
    ಹಿಂತಿರುಗಿ element;
}

UBool UnifiedCache::_flush(UBool all) ಸ್ಥಿರಾಂಕ {
    UBool ಫಲಿತಾಂಶ = ಸುಳ್ಳು;
    int32_t orig_ಗಾತ್ರ = uhash_ಎಣಿಕೆ(fHashtable);
    ಗಾಗಿ (int32_t i = 0; i < orig_ಗಾತ್ರ; ++i) {
        ಸ್ಥಿರಾಂಕ UHashElement *element = ಮುಂದಿನ_Element_2();
        ಆದರೆ (element == ಶೂನ್ಯಸೂಚಕ) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (all || _isEvictable(element)) {
            ಸ್ಥಿರಾಂಕ SharedObject *sharedObject =
                    static_cast<ಸ್ಥಿರಾಂಕ SharedObject*>(element->value.ಸೂಚಕ);
            U_ASSERT(sharedObject->cachePtr == ಇದು);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            ಫಲಿತಾಂಶ = ನಿಜ;
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

int32_t UnifiedCache::compute_ಎಣಿಕೆ_ದ_ವಸ್ತುಗಳು_ಗಮ್ಯ_Evict() ಸ್ಥಿರಾಂಕ {
    int32_t ಒಟ್ಟು_ವಸ್ತುಗಳು = uhash_ಎಣಿಕೆ(fHashtable);
    int32_t evictable_ವಸ್ತುಗಳು = ಒಟ್ಟು_ವಸ್ತುಗಳು - f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಳ_Use;

    int32_t ಬಳಸದ_ಎಲ್ಲೆ_By_Percentage = f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಳ_Use * f_Max_Percentage_ದ_ಒಳ_Use / 100;
    int32_t ಬಳಸದ_ಎಲ್ಲೆ = std::max(ಬಳಸದ_ಎಲ್ಲೆ_By_Percentage, f_Max_ಬಳಸದ);
    int32_t ಎಣಿಕೆ_ದ_ವಸ್ತುಗಳು_ಗಮ್ಯ_Evict = std::max<int32_t>(0, evictable_ವಸ್ತುಗಳು - ಬಳಸದ_ಎಲ್ಲೆ);
    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_ದ_ವಸ್ತುಗಳು_ಗಮ್ಯ_Evict;
}

ಶೂನ್ಯ UnifiedCache::ಸತತ_Eviction_ತುಂಡು() ಸ್ಥಿರಾಂಕ {
    int32_t max_ವಸ್ತುಗಳು_ಗಮ್ಯ_Evict = compute_ಎಣಿಕೆ_ದ_ವಸ್ತುಗಳು_ಗಮ್ಯ_Evict();
    ಆದರೆ (max_ವಸ್ತುಗಳು_ಗಮ್ಯ_Evict <= 0) {
        ಹಿಂತಿರುಗಿ;
    }
    ಗಾಗಿ (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ಸ್ಥಿರಾಂಕ UHashElement *element = ಮುಂದಿನ_Element_2();
        ಆದರೆ (element == ಶೂನ್ಯಸೂಚಕ) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (_isEvictable(element)) {
            ಸ್ಥಿರಾಂಕ SharedObject *sharedObject =
                    static_cast<ಸ್ಥಿರಾಂಕ SharedObject*>(element->value.ಸೂಚಕ);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_ಎಣಿಕೆ;
            ಆದರೆ (--max_ವಸ್ತುಗಳು_ಗಮ್ಯ_Evict == 0) {
                ನಿಲ್ಲಿಸು;
            }
        }
    }
}

ಶೂನ್ಯ UnifiedCache::_putNew(
        ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &ಕೀಲಿ,
        ಸ್ಥಿರಾಂಕ SharedObject *value,
        ಸ್ಥಿರಾಂಕ UError_ಸಂಕೇತ creation_ಸ್ಥಿತಿ,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    Cache_ಕೀಲಿ_ತಳ *ಕೀಲಿ_ಗಮ್ಯ_Adopt = ಕೀಲಿ.clone();
    ಆದರೆ (ಕೀಲಿ_ಗಮ್ಯ_Adopt == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    ಕೀಲಿ_ಗಮ್ಯ_Adopt->f_Creation_ಸ್ಥಿತಿ = creation_ಸ್ಥಿತಿ;
    ಆದರೆ (value->soft_Ref_ಎಣಿಕೆ == 0) {
        ರಿಜಿಸ್ಟರ್_Primary(ಕೀಲಿ_ಗಮ್ಯ_Adopt, value);
    }
    ಶೂನ್ಯ *old_ಮೌಲ್ಯ = uhash_put(fHashtable, ಕೀಲಿ_ಗಮ್ಯ_Adopt, (ಶೂನ್ಯ *) value, &ಸ್ಥಿತಿ_2);
    U_ASSERT(old_ಮೌಲ್ಯ == ಶೂನ್ಯಸೂಚಕ);
    (ಶೂನ್ಯ)old_ಮೌಲ್ಯ;
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        value->soft_Ref_ಎಣಿಕೆ++;
    }
}

ಶೂನ್ಯ UnifiedCache::_putIfAbsentAndGet(
        ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &ಕೀಲಿ,
        ಸ್ಥಿರಾಂಕ SharedObject *&value,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    std::lock_guard<std::mutex> ಬೀಗ(*gCacheMutex);
    ಸ್ಥಿರಾಂಕ UHashElement *element = uhash_ಕಂಡುಹಿಡಿ(fHashtable, &ಕೀಲಿ);
    ಆದರೆ (element != ಶೂನ್ಯಸೂಚಕ && !ಒಳ_ಪ್ರಗತಿ(element)) {
        _fetch(element, value, ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (element == ಶೂನ್ಯಸೂಚಕ) {
        UError_ಸಂಕೇತ putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(ಕೀಲಿ, value, ಸ್ಥಿತಿ_2, putError);
    } ಇಲ್ಲದಿದ್ದರೆ {
        _put(element, value, ಸ್ಥಿತಿ_2);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    ಸತತ_Eviction_ತುಂಡು();
}


UBool UnifiedCache::_poll(
        ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &ಕೀಲಿ,
        ಸ್ಥಿರಾಂಕ SharedObject *&value,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    U_ASSERT(value == ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(ಸ್ಥಿತಿ_2 == U_ZERO_ERROR);
    std::unique_lock<std::mutex> ಬೀಗ(*gCacheMutex);
    ಸ್ಥಿರಾಂಕ UHashElement *element = uhash_ಕಂಡುಹಿಡಿ(fHashtable, &ಕೀಲಿ);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     ಆಗ (element != ಶೂನ್ಯಸೂಚಕ && ಒಳ_ಪ್ರಗತಿ(element)) {
         g_ಒಳ_ಪ್ರಗತಿ_ಮೌಲ್ಯ_Added_Cond->wait(ಬೀಗ);
         element = uhash_ಕಂಡುಹಿಡಿ(fHashtable, &ಕೀಲಿ);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    ಆದರೆ (element != ಶೂನ್ಯಸೂಚಕ) {
         _fetch(element, value, ಸ್ಥಿತಿ_2);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(ಕೀಲಿ, f_No_ಮೌಲ್ಯ, U_ZERO_ERROR, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ UnifiedCache::_get(
        ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &ಕೀಲಿ,
        ಸ್ಥಿರಾಂಕ SharedObject *&value,
        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *creationContext,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    U_ASSERT(value == ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(ಸ್ಥಿತಿ_2 == U_ZERO_ERROR);
    ಆದರೆ (_poll(ಕೀಲಿ, value, ಸ್ಥಿತಿ_2)) {
        ಆದರೆ (value == f_No_ಮೌಲ್ಯ) {
            SharedObject::clearPtr(value);
        }
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    value = ಕೀಲಿ.createObject(creationContext, ಸ್ಥಿತಿ_2);
    U_ASSERT(value == ಶೂನ್ಯಸೂಚಕ || value->hasHardReferences());
    U_ASSERT(value != ಶೂನ್ಯಸೂಚಕ || ಸ್ಥಿತಿ_2 != U_ZERO_ERROR);
    ಆದರೆ (value == ಶೂನ್ಯಸೂಚಕ) {
        SharedObject::copyPtr(f_No_ಮೌಲ್ಯ, value);
    }
    _putIfAbsentAndGet(ಕೀಲಿ, value, ಸ್ಥಿತಿ_2);
    ಆದರೆ (value == f_No_ಮೌಲ್ಯ) {
        SharedObject::clearPtr(value);
    }
}

ಶೂನ್ಯ UnifiedCache::ರಿಜಿಸ್ಟರ್_Primary(
            ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ *the_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ SharedObject *value) ಸ್ಥಿರಾಂಕ {
    the_ಕೀಲಿ->fIsPrimary = ನಿಜ;
    value->cachePtr = ಇದು;
    ++f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಟ್ಟು;
    ++f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಳ_Use;
}

ಶೂನ್ಯ UnifiedCache::_put(
        ಸ್ಥಿರಾಂಕ UHashElement *element,
        ಸ್ಥಿರಾಂಕ SharedObject *value,
        ಸ್ಥಿರಾಂಕ UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    U_ASSERT(ಒಳ_ಪ್ರಗತಿ(element));
    ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ* the_ಕೀಲಿ = static_cast<ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ*>(element->ಕೀಲಿ.ಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ SharedObject* old_ಮೌಲ್ಯ = static_cast<ಸ್ಥಿರಾಂಕ SharedObject*>(element->value.ಸೂಚಕ);
    the_ಕೀಲಿ->f_Creation_ಸ್ಥಿತಿ = ಸ್ಥಿತಿ_2;
    ಆದರೆ (value->soft_Ref_ಎಣಿಕೆ == 0) {
        ರಿಜಿಸ್ಟರ್_Primary(the_ಕೀಲಿ, value);
    }
    value->soft_Ref_ಎಣಿಕೆ++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.ಸೂಚಕ = (ಶೂನ್ಯ *) value;
    U_ASSERT(old_ಮೌಲ್ಯ == f_No_ಮೌಲ್ಯ);
    removeSoftRef(old_ಮೌಲ್ಯ);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_ಒಳ_ಪ್ರಗತಿ_ಮೌಲ್ಯ_Added_Cond->notify_all();
}

ಶೂನ್ಯ UnifiedCache::_fetch(
        ಸ್ಥಿರಾಂಕ UHashElement *element,
        ಸ್ಥಿರಾಂಕ SharedObject *&value,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ* the_ಕೀಲಿ = static_cast<ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ*>(element->ಕೀಲಿ.ಸೂಚಕ);
    ಸ್ಥಿತಿ_2 = the_ಕೀಲಿ->f_Creation_ಸ್ಥಿತಿ;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ಸ್ಥಿರಾಂಕ SharedObject *>(element->value.ಸೂಚಕ);
    ಕೂಡಿಸು_Hard_Ref(value);
}


UBool UnifiedCache::ಒಳ_ಪ್ರಗತಿ(ಸ್ಥಿರಾಂಕ UHashElement* element) ಸ್ಥಿರಾಂಕ {
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ SharedObject * value = ಶೂನ್ಯಸೂಚಕ;
    _fetch(element, value, ಸ್ಥಿತಿ_2);
    UBool ಫಲಿತಾಂಶ = ಒಳ_ಪ್ರಗತಿ(value, ಸ್ಥಿತಿ_2);
    removeHardRef(value);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UBool UnifiedCache::ಒಳ_ಪ್ರಗತಿ(
        ಸ್ಥಿರಾಂಕ SharedObject* the_ಮೌಲ್ಯ, UError_ಸಂಕೇತ creation_ಸ್ಥಿತಿ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ (the_ಮೌಲ್ಯ == f_No_ಮೌಲ್ಯ && creation_ಸ್ಥಿತಿ == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ಸ್ಥಿರಾಂಕ UHashElement *element) ಸ್ಥಿರಾಂಕ
{
    ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ* the_ಕೀಲಿ = static_cast<ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ*>(element->ಕೀಲಿ.ಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ SharedObject* the_ಮೌಲ್ಯ = static_cast<ಸ್ಥಿರಾಂಕ SharedObject*>(element->value.ಸೂಚಕ);

    // Entries that are under construction are never evictable
    ಆದರೆ (ಒಳ_ಪ್ರಗತಿ(the_ಮೌಲ್ಯ, the_ಕೀಲಿ->f_Creation_ಸ್ಥಿತಿ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    ಹಿಂತಿರುಗಿ (!the_ಕೀಲಿ->fIsPrimary || (the_ಮೌಲ್ಯ->soft_Ref_ಎಣಿಕೆ == 1 && the_ಮೌಲ್ಯ->noHardReferences()));
}

ಶೂನ್ಯ UnifiedCache::removeSoftRef(ಸ್ಥಿರಾಂಕ SharedObject *value) ಸ್ಥಿರಾಂಕ {
    U_ASSERT(value->cachePtr == ಇದು);
    U_ASSERT(value->soft_Ref_ಎಣಿಕೆ > 0);
    ಆದರೆ (--value->soft_Ref_ಎಣಿಕೆ == 0) {
        --f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಟ್ಟು;
        ಆದರೆ (value->noHardReferences()) {
            ಅಳಿಸು value;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = ಶೂನ್ಯಸೂಚಕ;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ಸ್ಥಿರಾಂಕ SharedObject *value) ಸ್ಥಿರಾಂಕ {
    ಪೂರ್ಣಾಂಕ ref_ಎಣಿಕೆ = 0;
    ಆದರೆ (value) {
        ref_ಎಣಿಕೆ = umtx_atomic_dec(&value->hard_Ref_ಎಣಿಕೆ);
        U_ASSERT(ref_ಎಣಿಕೆ >= 0);
        ಆದರೆ (ref_ಎಣಿಕೆ == 0) {
            --f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಳ_Use;
        }
    }
    ಹಿಂತಿರುಗಿ ref_ಎಣಿಕೆ;
}

int32_t UnifiedCache::ಕೂಡಿಸು_Hard_Ref(ಸ್ಥಿರಾಂಕ SharedObject *value) ಸ್ಥಿರಾಂಕ {
    ಪೂರ್ಣಾಂಕ ref_ಎಣಿಕೆ = 0;
    ಆದರೆ (value) {
        ref_ಎಣಿಕೆ = umtx_atomic_inc(&value->hard_Ref_ಎಣಿಕೆ);
        U_ASSERT(ref_ಎಣಿಕೆ >= 1);
        ಆದರೆ (ref_ಎಣಿಕೆ == 1) {
            f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಳ_Use++;
        }
    }
    ಹಿಂತಿರುಗಿ ref_ಎಣಿಕೆ;
}

U_NAMESPACE_ಕೊನೆ

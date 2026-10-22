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

#అన్తర్భావ "unifiedcache.h"

#అన్తర్భావ <algorithm>      // For std::max()
#అన్తర్భావ <mutex>

#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "ucln_cmn.h"

స్థిర icu::UnifiedCache *gCache = శూన్యసూచిక;
స్థిర std::mutex *gCacheMutex = శూన్యసూచిక;
స్థిర std::condition_variable *g_లోపల_పురోగతి_విలువ_Added_Cond;
స్థిర icu::UInitOnce gCacheInitOnce {};

స్థిర స్థిరరాశి int32_t MAX_EVICT_ITERATIONS = 10;
స్థిర స్థిరరాశి int32_t DEFAULT_MAX_వాడని = 1000;
స్థిర స్థిరరాశి int32_t DEFAULT_PERCENTAGE_యొక్క_లోపల_USE = 100;


U_CDECL_BEGIN
స్థిర UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    తొలగించు gCache;
    gCache = శూన్యసూచిక;
    gCacheMutex->~mutex();
    gCacheMutex = శూన్యసూచిక;
    g_లోపల_పురోగతి_విలువ_Added_Cond->~condition_variable();
    g_లోపల_పురోగతి_విలువ_Added_Cond = శూన్యసూచిక;
    తిరిగి నిజం;
}
U_CDECL_చివర


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_కీలకాలు(స్థిరరాశి UHashTok కీలకం) {
    స్థిరరాశి Cache_కీలకం_పునాది* ckey = static_cast<స్థిరరాశి Cache_కీలకం_పునాది*>(కీలకం.సూచిక);
    తిరిగి ckey->hash_సంకేతం();
}

UBool U_EXPORT2
ucache_compare_కీలకాలు(స్థిరరాశి UHashTok key1, స్థిరరాశి UHashTok key2) {
    స్థిరరాశి Cache_కీలకం_పునాది* p1 = static_cast<స్థిరరాశి Cache_కీలకం_పునాది*>(key1.సూచిక);
    స్థిరరాశి Cache_కీలకం_పునాది* p2 = static_cast<స్థిరరాశి Cache_కీలకం_పునాది*>(key2.సూచిక);
    తిరిగి *p1 == *p2;
}

శూన్య U_EXPORT2
ucache_delete_కీలకం(శూన్య *obj) {
    Cache_కీలకం_పునాది* p = static_cast<Cache_కీలకం_పునాది*>(obj);
    తొలగించు p;
}

Cache_కీలకం_పునాది::~Cache_కీలకం_పునాది() {
}

స్థిర శూన్య U_CALLCONV cacheInit(UError_సంకేతం &స్థితి_2) {
    U_ASSERT(gCache == శూన్యసూచిక);
    ucln_common_రిజిస్టరు_Cleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_లోపల_పురోగతి_విలువ_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = కొత్త UnifiedCache(స్థితి_2);
    ఒకవేళ (gCache == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తొలగించు gCache;
        gCache = శూన్యసూచిక;
        తిరిగి;
    }
}

UnifiedCache *UnifiedCache::getInstance(UError_సంకేతం &స్థితి_2) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    U_ASSERT(gCache != శూన్యసూచిక);
    తిరిగి gCache;
}

UnifiedCache::UnifiedCache(UError_సంకేతం &స్థితి_2) :
        fHashtable(శూన్యసూచిక),
        fEvictPos(UHASH_మొదటి),
        f_అంకె_విలువలు_మొత్తం(0),
        f_అంకె_విలువలు_లోపల_Use(0),
        f_Max_వాడని(DEFAULT_MAX_వాడని),
        f_Max_Percentage_యొక్క_లోపల_Use(DEFAULT_PERCENTAGE_యొక్క_లోపల_USE),
        f_Auto_Evicted_లెక్క(0),
        f_No_విలువ(శూన్యసూచిక) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    f_No_విలువ = కొత్త SharedObject();
    ఒకవేళ (f_No_విలువ == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    f_No_విలువ->soft_Ref_లెక్క = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_విలువ->hard_Ref_లెక్క = 1;  // when other references to it are removed.
    f_No_విలువ->cachePtr = ఇది;

    fHashtable = uhash_open(
            &ucache_hash_కీలకాలు,
            &ucache_compare_కీలకాలు,
            శూన్యసూచిక,
            &స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    uhash_సమితి_కీలకం_Deleter(fHashtable, &ucache_delete_కీలకం);
}

శూన్య UnifiedCache::సమితి_Eviction_Policy(
        int32_t count, int32_t percentage_యొక్క_లోపల_Use_వస్తువులు, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (count < 0 || percentage_యొక్క_లోపల_Use_వస్తువులు < 0) {
        స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    std::lock_guard<std::mutex> తాళం(*gCacheMutex);
    f_Max_వాడని = count;
    f_Max_Percentage_యొక్క_లోపల_Use = percentage_యొక్క_లోపల_Use_వస్తువులు;
}

int32_t UnifiedCache::వాడని_లెక్క() స్థిరరాశి {
    std::lock_guard<std::mutex> తాళం(*gCacheMutex);
    తిరిగి uhash_లెక్క(fHashtable) - f_అంకె_విలువలు_లోపల_Use;
}

int64_t UnifiedCache::auto_Evicted_లెక్క() స్థిరరాశి {
    std::lock_guard<std::mutex> తాళం(*gCacheMutex);
    తిరిగి f_Auto_Evicted_లెక్క;
}

int32_t UnifiedCache::కీలకం_లెక్క() స్థిరరాశి {
    std::lock_guard<std::mutex> తాళం(*gCacheMutex);
    తిరిగి uhash_లెక్క(fHashtable);
}

శూన్య UnifiedCache::flush() స్థిరరాశి {
    std::lock_guard<std::mutex> తాళం(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    అయితే (_flush(అబద్ధం));
}

శూన్య UnifiedCache::handleUnreferencedObject() స్థిరరాశి {
    std::lock_guard<std::mutex> తాళం(*gCacheMutex);
    --f_అంకె_విలువలు_లోపల_Use;
    నిరంతరం_Eviction_ముక్క();
}

#నిర్వచితమైతే UNIFIED_CACHE_DEBUG
#అన్తర్భావ <stdio.h>

శూన్య UnifiedCache::dump() {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    స్థిరరాశి UnifiedCache *cache = getInstance(స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        తిరిగి;
    }
    cache->dumpContents();
}

శూన్య UnifiedCache::dumpContents() స్థిరరాశి {
    std::lock_guard<std::mutex> తాళం(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
శూన్య UnifiedCache::_dumpContents() స్థిరరాశి {
    int32_t pos = UHASH_మొదటి;
    స్థిరరాశి UHashElement *element = uhash_తదుపరి_Element(fHashtable, &pos);
    అక్షరం బఫరు[256];
    int32_t cnt = 0;
    కొరకు (; element != శూన్యసూచిక; element = uhash_తదుపరి_Element(fHashtable, &pos)) {
        స్థిరరాశి SharedObject *sharedObject =
                (స్థిరరాశి SharedObject *) element->value.సూచిక;
        స్థిరరాశి Cache_కీలకం_పునాది *కీలకం =
                (స్థిరరాశి Cache_కీలకం_పునాది *) element->కీలకం.సూచిక;
        ఒకవేళ (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    కీలకం->writeDescription(బఫరు, 256),
                    కీలకం->creation_స్థితి,
                    sharedObject == f_No_విలువ ? శూన్యసూచిక :sharedObject,
                    sharedObject->get_Ref_లెక్క(),
                    sharedObject->get_Soft_Ref_లెక్క());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_లెక్క(fHashtable));
}
#అయితే_ముగింపు

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> తాళం(*gCacheMutex);
        _flush(నిజం);
    }
    uhash_close(fHashtable);
    fHashtable = శూన్యసూచిక;
    తొలగించు f_No_విలువ;
    f_No_విలువ = శూన్యసూచిక;
}

స్థిరరాశి UHashElement *
UnifiedCache::తదుపరి_Element_2() స్థిరరాశి {
    స్థిరరాశి UHashElement *element = uhash_తదుపరి_Element(fHashtable, &fEvictPos);
    ఒకవేళ (element == శూన్యసూచిక) {
        fEvictPos = UHASH_మొదటి;
        తిరిగి uhash_తదుపరి_Element(fHashtable, &fEvictPos);
    }
    తిరిగి element;
}

UBool UnifiedCache::_flush(UBool all) స్థిరరాశి {
    UBool ఫలితం = అబద్ధం;
    int32_t orig_కొలత = uhash_లెక్క(fHashtable);
    కొరకు (int32_t i = 0; i < orig_కొలత; ++i) {
        స్థిరరాశి UHashElement *element = తదుపరి_Element_2();
        ఒకవేళ (element == శూన్యసూచిక) {
            ఆపు;
        }
        ఒకవేళ (all || _isEvictable(element)) {
            స్థిరరాశి SharedObject *sharedObject =
                    static_cast<స్థిరరాశి SharedObject*>(element->value.సూచిక);
            U_ASSERT(sharedObject->cachePtr == ఇది);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            ఫలితం = నిజం;
        }
    }
    తిరిగి ఫలితం;
}

int32_t UnifiedCache::compute_లెక్క_యొక్క_వస్తువులు_గమ్యం_Evict() స్థిరరాశి {
    int32_t మొత్తం_వస్తువులు = uhash_లెక్క(fHashtable);
    int32_t evictable_వస్తువులు = మొత్తం_వస్తువులు - f_అంకె_విలువలు_లోపల_Use;

    int32_t వాడని_హద్దు_By_Percentage = f_అంకె_విలువలు_లోపల_Use * f_Max_Percentage_యొక్క_లోపల_Use / 100;
    int32_t వాడని_హద్దు = std::max(వాడని_హద్దు_By_Percentage, f_Max_వాడని);
    int32_t లెక్క_యొక్క_వస్తువులు_గమ్యం_Evict = std::max<int32_t>(0, evictable_వస్తువులు - వాడని_హద్దు);
    తిరిగి లెక్క_యొక్క_వస్తువులు_గమ్యం_Evict;
}

శూన్య UnifiedCache::నిరంతరం_Eviction_ముక్క() స్థిరరాశి {
    int32_t max_వస్తువులు_గమ్యం_Evict = compute_లెక్క_యొక్క_వస్తువులు_గమ్యం_Evict();
    ఒకవేళ (max_వస్తువులు_గమ్యం_Evict <= 0) {
        తిరిగి;
    }
    కొరకు (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        స్థిరరాశి UHashElement *element = తదుపరి_Element_2();
        ఒకవేళ (element == శూన్యసూచిక) {
            ఆపు;
        }
        ఒకవేళ (_isEvictable(element)) {
            స్థిరరాశి SharedObject *sharedObject =
                    static_cast<స్థిరరాశి SharedObject*>(element->value.సూచిక);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_లెక్క;
            ఒకవేళ (--max_వస్తువులు_గమ్యం_Evict == 0) {
                ఆపు;
            }
        }
    }
}

శూన్య UnifiedCache::_putNew(
        స్థిరరాశి Cache_కీలకం_పునాది &కీలకం,
        స్థిరరాశి SharedObject *value,
        స్థిరరాశి UError_సంకేతం creation_స్థితి,
        UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    Cache_కీలకం_పునాది *కీలకం_గమ్యం_Adopt = కీలకం.clone();
    ఒకవేళ (కీలకం_గమ్యం_Adopt == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    కీలకం_గమ్యం_Adopt->f_Creation_స్థితి = creation_స్థితి;
    ఒకవేళ (value->soft_Ref_లెక్క == 0) {
        రిజిస్టరు_Primary(కీలకం_గమ్యం_Adopt, value);
    }
    శూన్య *old_విలువ = uhash_put(fHashtable, కీలకం_గమ్యం_Adopt, (శూన్య *) value, &స్థితి_2);
    U_ASSERT(old_విలువ == శూన్యసూచిక);
    (శూన్య)old_విలువ;
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        value->soft_Ref_లెక్క++;
    }
}

శూన్య UnifiedCache::_putIfAbsentAndGet(
        స్థిరరాశి Cache_కీలకం_పునాది &కీలకం,
        స్థిరరాశి SharedObject *&value,
        UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    std::lock_guard<std::mutex> తాళం(*gCacheMutex);
    స్థిరరాశి UHashElement *element = uhash_కనుగొను(fHashtable, &కీలకం);
    ఒకవేళ (element != శూన్యసూచిక && !లోపల_పురోగతి(element)) {
        _fetch(element, value, స్థితి_2);
        తిరిగి;
    }
    ఒకవేళ (element == శూన్యసూచిక) {
        UError_సంకేతం putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(కీలకం, value, స్థితి_2, putError);
    } లేకపోతే {
        _put(element, value, స్థితి_2);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    నిరంతరం_Eviction_ముక్క();
}


UBool UnifiedCache::_poll(
        స్థిరరాశి Cache_కీలకం_పునాది &కీలకం,
        స్థిరరాశి SharedObject *&value,
        UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    U_ASSERT(value == శూన్యసూచిక);
    U_ASSERT(స్థితి_2 == U_ZERO_ERROR);
    std::unique_lock<std::mutex> తాళం(*gCacheMutex);
    స్థిరరాశి UHashElement *element = uhash_కనుగొను(fHashtable, &కీలకం);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     అయితే (element != శూన్యసూచిక && లోపల_పురోగతి(element)) {
         g_లోపల_పురోగతి_విలువ_Added_Cond->wait(తాళం);
         element = uhash_కనుగొను(fHashtable, &కీలకం);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    ఒకవేళ (element != శూన్యసూచిక) {
         _fetch(element, value, స్థితి_2);
        తిరిగి నిజం;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(కీలకం, f_No_విలువ, U_ZERO_ERROR, స్థితి_2);
    తిరిగి అబద్ధం;
}

శూన్య UnifiedCache::_get(
        స్థిరరాశి Cache_కీలకం_పునాది &కీలకం,
        స్థిరరాశి SharedObject *&value,
        స్థిరరాశి శూన్య *creationContext,
        UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    U_ASSERT(value == శూన్యసూచిక);
    U_ASSERT(స్థితి_2 == U_ZERO_ERROR);
    ఒకవేళ (_poll(కీలకం, value, స్థితి_2)) {
        ఒకవేళ (value == f_No_విలువ) {
            SharedObject::clearPtr(value);
        }
        తిరిగి;
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    value = కీలకం.createObject(creationContext, స్థితి_2);
    U_ASSERT(value == శూన్యసూచిక || value->hasHardReferences());
    U_ASSERT(value != శూన్యసూచిక || స్థితి_2 != U_ZERO_ERROR);
    ఒకవేళ (value == శూన్యసూచిక) {
        SharedObject::copyPtr(f_No_విలువ, value);
    }
    _putIfAbsentAndGet(కీలకం, value, స్థితి_2);
    ఒకవేళ (value == f_No_విలువ) {
        SharedObject::clearPtr(value);
    }
}

శూన్య UnifiedCache::రిజిస్టరు_Primary(
            స్థిరరాశి Cache_కీలకం_పునాది *the_కీలకం, స్థిరరాశి SharedObject *value) స్థిరరాశి {
    the_కీలకం->fIsPrimary = నిజం;
    value->cachePtr = ఇది;
    ++f_అంకె_విలువలు_మొత్తం;
    ++f_అంకె_విలువలు_లోపల_Use;
}

శూన్య UnifiedCache::_put(
        స్థిరరాశి UHashElement *element,
        స్థిరరాశి SharedObject *value,
        స్థిరరాశి UError_సంకేతం స్థితి_2) స్థిరరాశి {
    U_ASSERT(లోపల_పురోగతి(element));
    స్థిరరాశి Cache_కీలకం_పునాది* the_కీలకం = static_cast<స్థిరరాశి Cache_కీలకం_పునాది*>(element->కీలకం.సూచిక);
    స్థిరరాశి SharedObject* old_విలువ = static_cast<స్థిరరాశి SharedObject*>(element->value.సూచిక);
    the_కీలకం->f_Creation_స్థితి = స్థితి_2;
    ఒకవేళ (value->soft_Ref_లెక్క == 0) {
        రిజిస్టరు_Primary(the_కీలకం, value);
    }
    value->soft_Ref_లెక్క++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.సూచిక = (శూన్య *) value;
    U_ASSERT(old_విలువ == f_No_విలువ);
    removeSoftRef(old_విలువ);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_లోపల_పురోగతి_విలువ_Added_Cond->notify_all();
}

శూన్య UnifiedCache::_fetch(
        స్థిరరాశి UHashElement *element,
        స్థిరరాశి SharedObject *&value,
        UError_సంకేతం &స్థితి_2) స్థిరరాశి {
    స్థిరరాశి Cache_కీలకం_పునాది* the_కీలకం = static_cast<స్థిరరాశి Cache_కీలకం_పునాది*>(element->కీలకం.సూచిక);
    స్థితి_2 = the_కీలకం->f_Creation_స్థితి;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<స్థిరరాశి SharedObject *>(element->value.సూచిక);
    కూడు_Hard_Ref(value);
}


UBool UnifiedCache::లోపల_పురోగతి(స్థిరరాశి UHashElement* element) స్థిరరాశి {
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    స్థిరరాశి SharedObject * value = శూన్యసూచిక;
    _fetch(element, value, స్థితి_2);
    UBool ఫలితం = లోపల_పురోగతి(value, స్థితి_2);
    removeHardRef(value);
    తిరిగి ఫలితం;
}

UBool UnifiedCache::లోపల_పురోగతి(
        స్థిరరాశి SharedObject* the_విలువ, UError_సంకేతం creation_స్థితి) స్థిరరాశి {
    తిరిగి (the_విలువ == f_No_విలువ && creation_స్థితి == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(స్థిరరాశి UHashElement *element) స్థిరరాశి
{
    స్థిరరాశి Cache_కీలకం_పునాది* the_కీలకం = static_cast<స్థిరరాశి Cache_కీలకం_పునాది*>(element->కీలకం.సూచిక);
    స్థిరరాశి SharedObject* the_విలువ = static_cast<స్థిరరాశి SharedObject*>(element->value.సూచిక);

    // Entries that are under construction are never evictable
    ఒకవేళ (లోపల_పురోగతి(the_విలువ, the_కీలకం->f_Creation_స్థితి)) {
        తిరిగి అబద్ధం;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    తిరిగి (!the_కీలకం->fIsPrimary || (the_విలువ->soft_Ref_లెక్క == 1 && the_విలువ->noHardReferences()));
}

శూన్య UnifiedCache::removeSoftRef(స్థిరరాశి SharedObject *value) స్థిరరాశి {
    U_ASSERT(value->cachePtr == ఇది);
    U_ASSERT(value->soft_Ref_లెక్క > 0);
    ఒకవేళ (--value->soft_Ref_లెక్క == 0) {
        --f_అంకె_విలువలు_మొత్తం;
        ఒకవేళ (value->noHardReferences()) {
            తొలగించు value;
        } లేకపోతే {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = శూన్యసూచిక;
        }
    }
}

int32_t UnifiedCache::removeHardRef(స్థిరరాశి SharedObject *value) స్థిరరాశి {
    పూర్ణాంకం ref_లెక్క = 0;
    ఒకవేళ (value) {
        ref_లెక్క = umtx_atomic_dec(&value->hard_Ref_లెక్క);
        U_ASSERT(ref_లెక్క >= 0);
        ఒకవేళ (ref_లెక్క == 0) {
            --f_అంకె_విలువలు_లోపల_Use;
        }
    }
    తిరిగి ref_లెక్క;
}

int32_t UnifiedCache::కూడు_Hard_Ref(స్థిరరాశి SharedObject *value) స్థిరరాశి {
    పూర్ణాంకం ref_లెక్క = 0;
    ఒకవేళ (value) {
        ref_లెక్క = umtx_atomic_inc(&value->hard_Ref_లెక్క);
        U_ASSERT(ref_లెక్క >= 1);
        ఒకవేళ (ref_లెక్క == 1) {
            f_అంకె_విలువలు_లోపల_Use++;
        }
    }
    తిరిగి ref_లెక్క;
}

U_NAMESPACE_చివర

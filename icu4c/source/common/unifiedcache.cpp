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

ساكن icu::UnifiedCache *gCache = خالي_اشارو;
ساكن std::mutex *gCacheMutex = خالي_اشارو;
ساكن std::condition_variable *g_اندريون_ترقي_قدر_Added_Cond;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن ثابت int32_t MAX_EVICT_ITERATIONS = 10;
ساكن ثابت int32_t DEFAULT_MAX_اڻ_استعمال_ٿيل = 1000;
ساكن ثابت int32_t DEFAULT_PERCENTAGE_جو_اندريون_USE = 100;


U_CDECL_BEGIN
ساكن UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    ميٽيو gCache;
    gCache = خالي_اشارو;
    gCacheMutex->~mutex();
    gCacheMutex = خالي_اشارو;
    g_اندريون_ترقي_قدر_Added_Cond->~condition_variable();
    g_اندريون_ترقي_قدر_Added_Cond = خالي_اشارو;
    أرجع سچ;
}
U_CDECL_پڇاڙي


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_ڪنجيون(ثابت UHashTok ڪنجي) {
    ثابت Cache_ڪنجي_بنياد* ckey = static_cast<ثابت Cache_ڪنجي_بنياد*>(ڪنجي.اشارو);
    أرجع ckey->hash_ڪوڊ();
}

UBool U_EXPORT2
ucache_compare_ڪنجيون(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت Cache_ڪنجي_بنياد* p1 = static_cast<ثابت Cache_ڪنجي_بنياد*>(key1.اشارو);
    ثابت Cache_ڪنجي_بنياد* p2 = static_cast<ثابت Cache_ڪنجي_بنياد*>(key2.اشارو);
    أرجع *p1 == *p2;
}

فراغ U_EXPORT2
ucache_delete_ڪنجي(فراغ *obj) {
    Cache_ڪنجي_بنياد* p = static_cast<Cache_ڪنجي_بنياد*>(obj);
    ميٽيو p;
}

Cache_ڪنجي_بنياد::~Cache_ڪنجي_بنياد() {
}

ساكن فراغ U_CALLCONV cacheInit(UError_ڪوڊ &حالت) {
    U_ASSERT(gCache == خالي_اشارو);
    ucln_common_رجسٽر_Cleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_اندريون_ترقي_قدر_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = جديد UnifiedCache(حالت);
    إذا (gCache == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
    }
    إذا (U_FAILURE(حالت)) {
        ميٽيو gCache;
        gCache = خالي_اشارو;
        أرجع;
    }
}

UnifiedCache *UnifiedCache::getInstance(UError_ڪوڊ &حالت) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }
    U_ASSERT(gCache != خالي_اشارو);
    أرجع gCache;
}

UnifiedCache::UnifiedCache(UError_ڪوڊ &حالت) :
        fHashtable(خالي_اشارو),
        fEvictPos(UHASH_پهريون),
        f_انگ_قدرون_ڪل(0),
        f_انگ_قدرون_اندريون_Use(0),
        f_Max_اڻ_استعمال_ٿيل(DEFAULT_MAX_اڻ_استعمال_ٿيل),
        f_Max_Percentage_جو_اندريون_Use(DEFAULT_PERCENTAGE_جو_اندريون_USE),
        f_Auto_Evicted_ڳڻپ(0),
        f_No_قدر(خالي_اشارو) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    f_No_قدر = جديد SharedObject();
    إذا (f_No_قدر == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    f_No_قدر->soft_Ref_ڳڻپ = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_قدر->hard_Ref_ڳڻپ = 1;  // when other references to it are removed.
    f_No_قدر->cachePtr = هذا;

    fHashtable = uhash_open(
            &ucache_hash_ڪنجيون,
            &ucache_compare_ڪنجيون,
            خالي_اشارو,
            &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_سيٽ_ڪنجي_Deleter(fHashtable, &ucache_delete_ڪنجي);
}

فراغ UnifiedCache::سيٽ_Eviction_Policy(
        int32_t count, int32_t percentage_جو_اندريون_Use_شيون, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (count < 0 || percentage_جو_اندريون_Use_شيون < 0) {
        حالت = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }
    std::lock_guard<std::mutex> تالو(*gCacheMutex);
    f_Max_اڻ_استعمال_ٿيل = count;
    f_Max_Percentage_جو_اندريون_Use = percentage_جو_اندريون_Use_شيون;
}

int32_t UnifiedCache::اڻ_استعمال_ٿيل_ڳڻپ() ثابت {
    std::lock_guard<std::mutex> تالو(*gCacheMutex);
    أرجع uhash_ڳڻپ(fHashtable) - f_انگ_قدرون_اندريون_Use;
}

int64_t UnifiedCache::auto_Evicted_ڳڻپ() ثابت {
    std::lock_guard<std::mutex> تالو(*gCacheMutex);
    أرجع f_Auto_Evicted_ڳڻپ;
}

int32_t UnifiedCache::ڪنجي_ڳڻپ() ثابت {
    std::lock_guard<std::mutex> تالو(*gCacheMutex);
    أرجع uhash_ڳڻپ(fHashtable);
}

فراغ UnifiedCache::flush() ثابت {
    std::lock_guard<std::mutex> تالو(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    طالما (_flush(خطأ));
}

فراغ UnifiedCache::handleUnreferencedObject() ثابت {
    std::lock_guard<std::mutex> تالو(*gCacheMutex);
    --f_انگ_قدرون_اندريون_Use;
    لاڳيتو_Eviction_ٽڪرو();
}

#جيڪڏهن_وصف_ٿيل UNIFIED_CACHE_DEBUG
#ضمن <stdio.h>

فراغ UnifiedCache::dump() {
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    ثابت UnifiedCache *cache = getInstance(حالت);
    إذا (U_FAILURE(حالت)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        أرجع;
    }
    cache->dumpContents();
}

فراغ UnifiedCache::dumpContents() ثابت {
    std::lock_guard<std::mutex> تالو(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
فراغ UnifiedCache::_dumpContents() ثابت {
    int32_t pos = UHASH_پهريون;
    ثابت UHashElement *element = uhash_ايندڙ_Element(fHashtable, &pos);
    حرف بفر[256];
    int32_t cnt = 0;
    لكل (; element != خالي_اشارو; element = uhash_ايندڙ_Element(fHashtable, &pos)) {
        ثابت SharedObject *sharedObject =
                (ثابت SharedObject *) element->value.اشارو;
        ثابت Cache_ڪنجي_بنياد *ڪنجي =
                (ثابت Cache_ڪنجي_بنياد *) element->ڪنجي.اشارو;
        إذا (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    ڪنجي->writeDescription(بفر, 256),
                    ڪنجي->creation_حالت,
                    sharedObject == f_No_قدر ? خالي_اشارو :sharedObject,
                    sharedObject->get_Ref_ڳڻپ(),
                    sharedObject->get_Soft_Ref_ڳڻپ());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_ڳڻپ(fHashtable));
}
#جيڪڏهن_پورو

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> تالو(*gCacheMutex);
        _flush(سچ);
    }
    uhash_close(fHashtable);
    fHashtable = خالي_اشارو;
    ميٽيو f_No_قدر;
    f_No_قدر = خالي_اشارو;
}

ثابت UHashElement *
UnifiedCache::ايندڙ_Element_2() ثابت {
    ثابت UHashElement *element = uhash_ايندڙ_Element(fHashtable, &fEvictPos);
    إذا (element == خالي_اشارو) {
        fEvictPos = UHASH_پهريون;
        أرجع uhash_ايندڙ_Element(fHashtable, &fEvictPos);
    }
    أرجع element;
}

UBool UnifiedCache::_flush(UBool all) ثابت {
    UBool نتيجو = خطأ;
    int32_t orig_سائيز = uhash_ڳڻپ(fHashtable);
    لكل (int32_t i = 0; i < orig_سائيز; ++i) {
        ثابت UHashElement *element = ايندڙ_Element_2();
        إذا (element == خالي_اشارو) {
            اكسر;
        }
        إذا (all || _isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشارو);
            U_ASSERT(sharedObject->cachePtr == هذا);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            نتيجو = سچ;
        }
    }
    أرجع نتيجو;
}

int32_t UnifiedCache::compute_ڳڻپ_جو_شيون_منزل_Evict() ثابت {
    int32_t ڪل_شيون = uhash_ڳڻپ(fHashtable);
    int32_t evictable_شيون = ڪل_شيون - f_انگ_قدرون_اندريون_Use;

    int32_t اڻ_استعمال_ٿيل_حد_بندي_By_Percentage = f_انگ_قدرون_اندريون_Use * f_Max_Percentage_جو_اندريون_Use / 100;
    int32_t اڻ_استعمال_ٿيل_حد_بندي = std::max(اڻ_استعمال_ٿيل_حد_بندي_By_Percentage, f_Max_اڻ_استعمال_ٿيل);
    int32_t ڳڻپ_جو_شيون_منزل_Evict = std::max<int32_t>(0, evictable_شيون - اڻ_استعمال_ٿيل_حد_بندي);
    أرجع ڳڻپ_جو_شيون_منزل_Evict;
}

فراغ UnifiedCache::لاڳيتو_Eviction_ٽڪرو() ثابت {
    int32_t max_شيون_منزل_Evict = compute_ڳڻپ_جو_شيون_منزل_Evict();
    إذا (max_شيون_منزل_Evict <= 0) {
        أرجع;
    }
    لكل (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ثابت UHashElement *element = ايندڙ_Element_2();
        إذا (element == خالي_اشارو) {
            اكسر;
        }
        إذا (_isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشارو);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_ڳڻپ;
            إذا (--max_شيون_منزل_Evict == 0) {
                اكسر;
            }
        }
    }
}

فراغ UnifiedCache::_putNew(
        ثابت Cache_ڪنجي_بنياد &ڪنجي,
        ثابت SharedObject *value,
        ثابت UError_ڪوڊ creation_حالت,
        UError_ڪوڊ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    Cache_ڪنجي_بنياد *ڪنجي_منزل_Adopt = ڪنجي.clone();
    إذا (ڪنجي_منزل_Adopt == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    ڪنجي_منزل_Adopt->f_Creation_حالت = creation_حالت;
    إذا (value->soft_Ref_ڳڻپ == 0) {
        رجسٽر_Primary(ڪنجي_منزل_Adopt, value);
    }
    فراغ *old_قدر = uhash_put(fHashtable, ڪنجي_منزل_Adopt, (فراغ *) value, &حالت);
    U_ASSERT(old_قدر == خالي_اشارو);
    (فراغ)old_قدر;
    إذا (U_SUCCESS(حالت)) {
        value->soft_Ref_ڳڻپ++;
    }
}

فراغ UnifiedCache::_putIfAbsentAndGet(
        ثابت Cache_ڪنجي_بنياد &ڪنجي,
        ثابت SharedObject *&value,
        UError_ڪوڊ &حالت) ثابت {
    std::lock_guard<std::mutex> تالو(*gCacheMutex);
    ثابت UHashElement *element = uhash_ڳوليو(fHashtable, &ڪنجي);
    إذا (element != خالي_اشارو && !اندريون_ترقي(element)) {
        _fetch(element, value, حالت);
        أرجع;
    }
    إذا (element == خالي_اشارو) {
        UError_ڪوڊ putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(ڪنجي, value, حالت, putError);
    } وإلا {
        _put(element, value, حالت);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    لاڳيتو_Eviction_ٽڪرو();
}


UBool UnifiedCache::_poll(
        ثابت Cache_ڪنجي_بنياد &ڪنجي,
        ثابت SharedObject *&value,
        UError_ڪوڊ &حالت) ثابت {
    U_ASSERT(value == خالي_اشارو);
    U_ASSERT(حالت == U_ZERO_ERROR);
    std::unique_lock<std::mutex> تالو(*gCacheMutex);
    ثابت UHashElement *element = uhash_ڳوليو(fHashtable, &ڪنجي);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     طالما (element != خالي_اشارو && اندريون_ترقي(element)) {
         g_اندريون_ترقي_قدر_Added_Cond->wait(تالو);
         element = uhash_ڳوليو(fHashtable, &ڪنجي);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    إذا (element != خالي_اشارو) {
         _fetch(element, value, حالت);
        أرجع سچ;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(ڪنجي, f_No_قدر, U_ZERO_ERROR, حالت);
    أرجع خطأ;
}

فراغ UnifiedCache::_get(
        ثابت Cache_ڪنجي_بنياد &ڪنجي,
        ثابت SharedObject *&value,
        ثابت فراغ *creationContext,
        UError_ڪوڊ &حالت) ثابت {
    U_ASSERT(value == خالي_اشارو);
    U_ASSERT(حالت == U_ZERO_ERROR);
    إذا (_poll(ڪنجي, value, حالت)) {
        إذا (value == f_No_قدر) {
            SharedObject::clearPtr(value);
        }
        أرجع;
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    value = ڪنجي.createObject(creationContext, حالت);
    U_ASSERT(value == خالي_اشارو || value->hasHardReferences());
    U_ASSERT(value != خالي_اشارو || حالت != U_ZERO_ERROR);
    إذا (value == خالي_اشارو) {
        SharedObject::copyPtr(f_No_قدر, value);
    }
    _putIfAbsentAndGet(ڪنجي, value, حالت);
    إذا (value == f_No_قدر) {
        SharedObject::clearPtr(value);
    }
}

فراغ UnifiedCache::رجسٽر_Primary(
            ثابت Cache_ڪنجي_بنياد *the_ڪنجي, ثابت SharedObject *value) ثابت {
    the_ڪنجي->fIsPrimary = سچ;
    value->cachePtr = هذا;
    ++f_انگ_قدرون_ڪل;
    ++f_انگ_قدرون_اندريون_Use;
}

فراغ UnifiedCache::_put(
        ثابت UHashElement *element,
        ثابت SharedObject *value,
        ثابت UError_ڪوڊ حالت) ثابت {
    U_ASSERT(اندريون_ترقي(element));
    ثابت Cache_ڪنجي_بنياد* the_ڪنجي = static_cast<ثابت Cache_ڪنجي_بنياد*>(element->ڪنجي.اشارو);
    ثابت SharedObject* old_قدر = static_cast<ثابت SharedObject*>(element->value.اشارو);
    the_ڪنجي->f_Creation_حالت = حالت;
    إذا (value->soft_Ref_ڳڻپ == 0) {
        رجسٽر_Primary(the_ڪنجي, value);
    }
    value->soft_Ref_ڳڻپ++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.اشارو = (فراغ *) value;
    U_ASSERT(old_قدر == f_No_قدر);
    removeSoftRef(old_قدر);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_اندريون_ترقي_قدر_Added_Cond->notify_all();
}

فراغ UnifiedCache::_fetch(
        ثابت UHashElement *element,
        ثابت SharedObject *&value,
        UError_ڪوڊ &حالت) ثابت {
    ثابت Cache_ڪنجي_بنياد* the_ڪنجي = static_cast<ثابت Cache_ڪنجي_بنياد*>(element->ڪنجي.اشارو);
    حالت = the_ڪنجي->f_Creation_حالت;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ثابت SharedObject *>(element->value.اشارو);
    جوڙيو_Hard_Ref(value);
}


UBool UnifiedCache::اندريون_ترقي(ثابت UHashElement* element) ثابت {
    UError_ڪوڊ حالت = U_ZERO_ERROR;
    ثابت SharedObject * value = خالي_اشارو;
    _fetch(element, value, حالت);
    UBool نتيجو = اندريون_ترقي(value, حالت);
    removeHardRef(value);
    أرجع نتيجو;
}

UBool UnifiedCache::اندريون_ترقي(
        ثابت SharedObject* the_قدر, UError_ڪوڊ creation_حالت) ثابت {
    أرجع (the_قدر == f_No_قدر && creation_حالت == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ثابت UHashElement *element) ثابت
{
    ثابت Cache_ڪنجي_بنياد* the_ڪنجي = static_cast<ثابت Cache_ڪنجي_بنياد*>(element->ڪنجي.اشارو);
    ثابت SharedObject* the_قدر = static_cast<ثابت SharedObject*>(element->value.اشارو);

    // Entries that are under construction are never evictable
    إذا (اندريون_ترقي(the_قدر, the_ڪنجي->f_Creation_حالت)) {
        أرجع خطأ;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    أرجع (!the_ڪنجي->fIsPrimary || (the_قدر->soft_Ref_ڳڻپ == 1 && the_قدر->noHardReferences()));
}

فراغ UnifiedCache::removeSoftRef(ثابت SharedObject *value) ثابت {
    U_ASSERT(value->cachePtr == هذا);
    U_ASSERT(value->soft_Ref_ڳڻپ > 0);
    إذا (--value->soft_Ref_ڳڻپ == 0) {
        --f_انگ_قدرون_ڪل;
        إذا (value->noHardReferences()) {
            ميٽيو value;
        } وإلا {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = خالي_اشارو;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ثابت SharedObject *value) ثابت {
    صحيح ref_ڳڻپ = 0;
    إذا (value) {
        ref_ڳڻپ = umtx_atomic_dec(&value->hard_Ref_ڳڻپ);
        U_ASSERT(ref_ڳڻپ >= 0);
        إذا (ref_ڳڻپ == 0) {
            --f_انگ_قدرون_اندريون_Use;
        }
    }
    أرجع ref_ڳڻپ;
}

int32_t UnifiedCache::جوڙيو_Hard_Ref(ثابت SharedObject *value) ثابت {
    صحيح ref_ڳڻپ = 0;
    إذا (value) {
        ref_ڳڻپ = umtx_atomic_inc(&value->hard_Ref_ڳڻپ);
        U_ASSERT(ref_ڳڻپ >= 1);
        إذا (ref_ڳڻپ == 1) {
            f_انگ_قدرون_اندريون_Use++;
        }
    }
    أرجع ref_ڳڻپ;
}

U_NAMESPACE_پڇاڙي

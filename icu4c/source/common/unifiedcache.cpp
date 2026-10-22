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

ساكن icu::UnifiedCache *gCache = خالی_سوچک;
ساكن std::mutex *gCacheMutex = خالی_سوچک;
ساكن std::condition_variable *g_وچ_ترقی_قدر_Added_Cond;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن ثابت int32_t MAX_EVICT_ITERATIONS = 10;
ساكن ثابت int32_t DEFAULT_MAX_بن_ورتیا = 1000;
ساكن ثابت int32_t DEFAULT_PERCENTAGE_دا_وچ_USE = 100;


U_CDECL_BEGIN
ساكن UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    مٹاؤ gCache;
    gCache = خالی_سوچک;
    gCacheMutex->~mutex();
    gCacheMutex = خالی_سوچک;
    g_وچ_ترقی_قدر_Added_Cond->~condition_variable();
    g_وچ_ترقی_قدر_Added_Cond = خالی_سوچک;
    أرجع سچ;
}
U_CDECL_سرا


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_کلیدیں(ثابت UHashTok کلید) {
    ثابت Cache_کلید_ادھار* ckey = static_cast<ثابت Cache_کلید_ادھار*>(کلید.اشارہ_گر);
    أرجع ckey->hash_کوڈ();
}

UBool U_EXPORT2
ucache_compare_کلیدیں(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت Cache_کلید_ادھار* p1 = static_cast<ثابت Cache_کلید_ادھار*>(key1.اشارہ_گر);
    ثابت Cache_کلید_ادھار* p2 = static_cast<ثابت Cache_کلید_ادھار*>(key2.اشارہ_گر);
    أرجع *p1 == *p2;
}

فراغ U_EXPORT2
ucache_delete_کلید(فراغ *obj) {
    Cache_کلید_ادھار* p = static_cast<Cache_کلید_ادھار*>(obj);
    مٹاؤ p;
}

Cache_کلید_ادھار::~Cache_کلید_ادھار() {
}

ساكن فراغ U_CALLCONV cacheInit(UError_کوڈ &حالت) {
    U_ASSERT(gCache == خالی_سوچک);
    ucln_common_رجسٹر_Cleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_وچ_ترقی_قدر_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = جديد UnifiedCache(حالت);
    إذا (gCache == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
    }
    إذا (U_FAILURE(حالت)) {
        مٹاؤ gCache;
        gCache = خالی_سوچک;
        أرجع;
    }
}

UnifiedCache *UnifiedCache::getInstance(UError_کوڈ &حالت) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    U_ASSERT(gCache != خالی_سوچک);
    أرجع gCache;
}

UnifiedCache::UnifiedCache(UError_کوڈ &حالت) :
        fHashtable(خالی_سوچک),
        fEvictPos(UHASH_پہلا),
        f_عدد_اقدار_کل(0),
        f_عدد_اقدار_وچ_Use(0),
        f_Max_بن_ورتیا(DEFAULT_MAX_بن_ورتیا),
        f_Max_Percentage_دا_وچ_Use(DEFAULT_PERCENTAGE_دا_وچ_USE),
        f_Auto_Evicted_گنتی(0),
        f_No_قدر(خالی_سوچک) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    f_No_قدر = جديد SharedObject();
    إذا (f_No_قدر == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    f_No_قدر->soft_Ref_گنتی = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_قدر->hard_Ref_گنتی = 1;  // when other references to it are removed.
    f_No_قدر->cachePtr = هذا;

    fHashtable = uhash_open(
            &ucache_hash_کلیدیں,
            &ucache_compare_کلیدیں,
            خالی_سوچک,
            &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_سیٹ_کلید_Deleter(fHashtable, &ucache_delete_کلید);
}

فراغ UnifiedCache::سیٹ_Eviction_Policy(
        int32_t count, int32_t percentage_دا_وچ_Use_اشیاء, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (count < 0 || percentage_دا_وچ_Use_اشیاء < 0) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    std::lock_guard<std::mutex> جندرا(*gCacheMutex);
    f_Max_بن_ورتیا = count;
    f_Max_Percentage_دا_وچ_Use = percentage_دا_وچ_Use_اشیاء;
}

int32_t UnifiedCache::بن_ورتیا_گنتی() ثابت {
    std::lock_guard<std::mutex> جندرا(*gCacheMutex);
    أرجع uhash_گنتی(fHashtable) - f_عدد_اقدار_وچ_Use;
}

int64_t UnifiedCache::auto_Evicted_گنتی() ثابت {
    std::lock_guard<std::mutex> جندرا(*gCacheMutex);
    أرجع f_Auto_Evicted_گنتی;
}

int32_t UnifiedCache::کلید_گنتی() ثابت {
    std::lock_guard<std::mutex> جندرا(*gCacheMutex);
    أرجع uhash_گنتی(fHashtable);
}

فراغ UnifiedCache::flush() ثابت {
    std::lock_guard<std::mutex> جندرا(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    طالما (_flush(خطأ));
}

فراغ UnifiedCache::handleUnreferencedObject() ثابت {
    std::lock_guard<std::mutex> جندرا(*gCacheMutex);
    --f_عدد_اقدار_وچ_Use;
    لگاتار_Eviction_ٹکڑا();
}

#جے_تعریف_شدہ UNIFIED_CACHE_DEBUG
#ضمن <stdio.h>

فراغ UnifiedCache::dump() {
    UError_کوڈ حالت = U_ZERO_ERROR;
    ثابت UnifiedCache *cache = getInstance(حالت);
    إذا (U_FAILURE(حالت)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        أرجع;
    }
    cache->dumpContents();
}

فراغ UnifiedCache::dumpContents() ثابت {
    std::lock_guard<std::mutex> جندرا(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
فراغ UnifiedCache::_dumpContents() ثابت {
    int32_t pos = UHASH_پہلا;
    ثابت UHashElement *element = uhash_اگلا_Element(fHashtable, &pos);
    حرف بفر[256];
    int32_t cnt = 0;
    لكل (; element != خالی_سوچک; element = uhash_اگلا_Element(fHashtable, &pos)) {
        ثابت SharedObject *sharedObject =
                (ثابت SharedObject *) element->value.اشارہ_گر;
        ثابت Cache_کلید_ادھار *کلید =
                (ثابت Cache_کلید_ادھار *) element->کلید.اشارہ_گر;
        إذا (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    کلید->writeDescription(بفر, 256),
                    کلید->creation_حالت,
                    sharedObject == f_No_قدر ? خالی_سوچک :sharedObject,
                    sharedObject->get_Ref_گنتی(),
                    sharedObject->get_Soft_Ref_گنتی());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_گنتی(fHashtable));
}
#جے_انت

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> جندرا(*gCacheMutex);
        _flush(سچ);
    }
    uhash_close(fHashtable);
    fHashtable = خالی_سوچک;
    مٹاؤ f_No_قدر;
    f_No_قدر = خالی_سوچک;
}

ثابت UHashElement *
UnifiedCache::اگلا_Element_2() ثابت {
    ثابت UHashElement *element = uhash_اگلا_Element(fHashtable, &fEvictPos);
    إذا (element == خالی_سوچک) {
        fEvictPos = UHASH_پہلا;
        أرجع uhash_اگلا_Element(fHashtable, &fEvictPos);
    }
    أرجع element;
}

UBool UnifiedCache::_flush(UBool all) ثابت {
    UBool نتیجہ = خطأ;
    int32_t orig_ناپ = uhash_گنتی(fHashtable);
    لكل (int32_t i = 0; i < orig_ناپ; ++i) {
        ثابت UHashElement *element = اگلا_Element_2();
        إذا (element == خالی_سوچک) {
            اكسر;
        }
        إذا (all || _isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشارہ_گر);
            U_ASSERT(sharedObject->cachePtr == هذا);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            نتیجہ = سچ;
        }
    }
    أرجع نتیجہ;
}

int32_t UnifiedCache::compute_گنتی_دا_اشیاء_منزل_Evict() ثابت {
    int32_t کل_اشیاء = uhash_گنتی(fHashtable);
    int32_t evictable_اشیاء = کل_اشیاء - f_عدد_اقدار_وچ_Use;

    int32_t بن_ورتیا_حد_بندی_By_Percentage = f_عدد_اقدار_وچ_Use * f_Max_Percentage_دا_وچ_Use / 100;
    int32_t بن_ورتیا_حد_بندی = std::max(بن_ورتیا_حد_بندی_By_Percentage, f_Max_بن_ورتیا);
    int32_t گنتی_دا_اشیاء_منزل_Evict = std::max<int32_t>(0, evictable_اشیاء - بن_ورتیا_حد_بندی);
    أرجع گنتی_دا_اشیاء_منزل_Evict;
}

فراغ UnifiedCache::لگاتار_Eviction_ٹکڑا() ثابت {
    int32_t max_اشیاء_منزل_Evict = compute_گنتی_دا_اشیاء_منزل_Evict();
    إذا (max_اشیاء_منزل_Evict <= 0) {
        أرجع;
    }
    لكل (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ثابت UHashElement *element = اگلا_Element_2();
        إذا (element == خالی_سوچک) {
            اكسر;
        }
        إذا (_isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشارہ_گر);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_گنتی;
            إذا (--max_اشیاء_منزل_Evict == 0) {
                اكسر;
            }
        }
    }
}

فراغ UnifiedCache::_putNew(
        ثابت Cache_کلید_ادھار &کلید,
        ثابت SharedObject *value,
        ثابت UError_کوڈ creation_حالت,
        UError_کوڈ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    Cache_کلید_ادھار *کلید_منزل_Adopt = کلید.clone();
    إذا (کلید_منزل_Adopt == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    کلید_منزل_Adopt->f_Creation_حالت = creation_حالت;
    إذا (value->soft_Ref_گنتی == 0) {
        رجسٹر_Primary(کلید_منزل_Adopt, value);
    }
    فراغ *old_قدر = uhash_put(fHashtable, کلید_منزل_Adopt, (فراغ *) value, &حالت);
    U_ASSERT(old_قدر == خالی_سوچک);
    (فراغ)old_قدر;
    إذا (U_SUCCESS(حالت)) {
        value->soft_Ref_گنتی++;
    }
}

فراغ UnifiedCache::_putIfAbsentAndGet(
        ثابت Cache_کلید_ادھار &کلید,
        ثابت SharedObject *&value,
        UError_کوڈ &حالت) ثابت {
    std::lock_guard<std::mutex> جندرا(*gCacheMutex);
    ثابت UHashElement *element = uhash_لبھو(fHashtable, &کلید);
    إذا (element != خالی_سوچک && !وچ_ترقی(element)) {
        _fetch(element, value, حالت);
        أرجع;
    }
    إذا (element == خالی_سوچک) {
        UError_کوڈ putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(کلید, value, حالت, putError);
    } وإلا {
        _put(element, value, حالت);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    لگاتار_Eviction_ٹکڑا();
}


UBool UnifiedCache::_poll(
        ثابت Cache_کلید_ادھار &کلید,
        ثابت SharedObject *&value,
        UError_کوڈ &حالت) ثابت {
    U_ASSERT(value == خالی_سوچک);
    U_ASSERT(حالت == U_ZERO_ERROR);
    std::unique_lock<std::mutex> جندرا(*gCacheMutex);
    ثابت UHashElement *element = uhash_لبھو(fHashtable, &کلید);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     طالما (element != خالی_سوچک && وچ_ترقی(element)) {
         g_وچ_ترقی_قدر_Added_Cond->wait(جندرا);
         element = uhash_لبھو(fHashtable, &کلید);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    إذا (element != خالی_سوچک) {
         _fetch(element, value, حالت);
        أرجع سچ;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(کلید, f_No_قدر, U_ZERO_ERROR, حالت);
    أرجع خطأ;
}

فراغ UnifiedCache::_get(
        ثابت Cache_کلید_ادھار &کلید,
        ثابت SharedObject *&value,
        ثابت فراغ *creationContext,
        UError_کوڈ &حالت) ثابت {
    U_ASSERT(value == خالی_سوچک);
    U_ASSERT(حالت == U_ZERO_ERROR);
    إذا (_poll(کلید, value, حالت)) {
        إذا (value == f_No_قدر) {
            SharedObject::clearPtr(value);
        }
        أرجع;
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    value = کلید.createObject(creationContext, حالت);
    U_ASSERT(value == خالی_سوچک || value->hasHardReferences());
    U_ASSERT(value != خالی_سوچک || حالت != U_ZERO_ERROR);
    إذا (value == خالی_سوچک) {
        SharedObject::copyPtr(f_No_قدر, value);
    }
    _putIfAbsentAndGet(کلید, value, حالت);
    إذا (value == f_No_قدر) {
        SharedObject::clearPtr(value);
    }
}

فراغ UnifiedCache::رجسٹر_Primary(
            ثابت Cache_کلید_ادھار *the_کلید, ثابت SharedObject *value) ثابت {
    the_کلید->fIsPrimary = سچ;
    value->cachePtr = هذا;
    ++f_عدد_اقدار_کل;
    ++f_عدد_اقدار_وچ_Use;
}

فراغ UnifiedCache::_put(
        ثابت UHashElement *element,
        ثابت SharedObject *value,
        ثابت UError_کوڈ حالت) ثابت {
    U_ASSERT(وچ_ترقی(element));
    ثابت Cache_کلید_ادھار* the_کلید = static_cast<ثابت Cache_کلید_ادھار*>(element->کلید.اشارہ_گر);
    ثابت SharedObject* old_قدر = static_cast<ثابت SharedObject*>(element->value.اشارہ_گر);
    the_کلید->f_Creation_حالت = حالت;
    إذا (value->soft_Ref_گنتی == 0) {
        رجسٹر_Primary(the_کلید, value);
    }
    value->soft_Ref_گنتی++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.اشارہ_گر = (فراغ *) value;
    U_ASSERT(old_قدر == f_No_قدر);
    removeSoftRef(old_قدر);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_وچ_ترقی_قدر_Added_Cond->notify_all();
}

فراغ UnifiedCache::_fetch(
        ثابت UHashElement *element,
        ثابت SharedObject *&value,
        UError_کوڈ &حالت) ثابت {
    ثابت Cache_کلید_ادھار* the_کلید = static_cast<ثابت Cache_کلید_ادھار*>(element->کلید.اشارہ_گر);
    حالت = the_کلید->f_Creation_حالت;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ثابت SharedObject *>(element->value.اشارہ_گر);
    جوڑو_Hard_Ref(value);
}


UBool UnifiedCache::وچ_ترقی(ثابت UHashElement* element) ثابت {
    UError_کوڈ حالت = U_ZERO_ERROR;
    ثابت SharedObject * value = خالی_سوچک;
    _fetch(element, value, حالت);
    UBool نتیجہ = وچ_ترقی(value, حالت);
    removeHardRef(value);
    أرجع نتیجہ;
}

UBool UnifiedCache::وچ_ترقی(
        ثابت SharedObject* the_قدر, UError_کوڈ creation_حالت) ثابت {
    أرجع (the_قدر == f_No_قدر && creation_حالت == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ثابت UHashElement *element) ثابت
{
    ثابت Cache_کلید_ادھار* the_کلید = static_cast<ثابت Cache_کلید_ادھار*>(element->کلید.اشارہ_گر);
    ثابت SharedObject* the_قدر = static_cast<ثابت SharedObject*>(element->value.اشارہ_گر);

    // Entries that are under construction are never evictable
    إذا (وچ_ترقی(the_قدر, the_کلید->f_Creation_حالت)) {
        أرجع خطأ;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    أرجع (!the_کلید->fIsPrimary || (the_قدر->soft_Ref_گنتی == 1 && the_قدر->noHardReferences()));
}

فراغ UnifiedCache::removeSoftRef(ثابت SharedObject *value) ثابت {
    U_ASSERT(value->cachePtr == هذا);
    U_ASSERT(value->soft_Ref_گنتی > 0);
    إذا (--value->soft_Ref_گنتی == 0) {
        --f_عدد_اقدار_کل;
        إذا (value->noHardReferences()) {
            مٹاؤ value;
        } وإلا {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = خالی_سوچک;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ثابت SharedObject *value) ثابت {
    صحيح ref_گنتی = 0;
    إذا (value) {
        ref_گنتی = umtx_atomic_dec(&value->hard_Ref_گنتی);
        U_ASSERT(ref_گنتی >= 0);
        إذا (ref_گنتی == 0) {
            --f_عدد_اقدار_وچ_Use;
        }
    }
    أرجع ref_گنتی;
}

int32_t UnifiedCache::جوڑو_Hard_Ref(ثابت SharedObject *value) ثابت {
    صحيح ref_گنتی = 0;
    إذا (value) {
        ref_گنتی = umtx_atomic_inc(&value->hard_Ref_گنتی);
        U_ASSERT(ref_گنتی >= 1);
        إذا (ref_گنتی == 1) {
            f_عدد_اقدار_وچ_Use++;
        }
    }
    أرجع ref_گنتی;
}

U_NAMESPACE_سرا

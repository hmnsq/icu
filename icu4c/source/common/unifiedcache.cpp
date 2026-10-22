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

#شامل "unifiedcache.h"

#شامل <algorithm>      // For std::max()
#شامل <mutex>

#شامل "uassert.h"
#شامل "uhash.h"
#شامل "ucln_cmn.h"

ساکن icu::UnifiedCache *gCache = خالی_اشاریہ;
ساکن std::mutex *gCacheMutex = خالی_اشاریہ;
ساکن std::condition_variable *g_اندر_پیش_رفت_قدر_Added_Cond;
ساکن icu::UInitOnce gCacheInitOnce {};

ساکن ثابت int32_t MAX_EVICT_ITERATIONS = 10;
ساکن ثابت int32_t DEFAULT_MAX_غیر_مستعمل = 1000;
ساکن ثابت int32_t DEFAULT_PERCENTAGE_کا_اندر_USE = 100;


U_CDECL_BEGIN
ساکن UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    حذف gCache;
    gCache = خالی_اشاریہ;
    gCacheMutex->~mutex();
    gCacheMutex = خالی_اشاریہ;
    g_اندر_پیش_رفت_قدر_Added_Cond->~condition_variable();
    g_اندر_پیش_رفت_قدر_Added_Cond = خالی_اشاریہ;
    واپس سچ;
}
U_CDECL_انتہا


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_کلیدیں(ثابت UHashTok کلید) {
    ثابت Cache_کلید_بنیاد* ckey = static_cast<ثابت Cache_کلید_بنیاد*>(کلید.اشارہ_گر);
    واپس ckey->hash_رمز();
}

UBool U_EXPORT2
ucache_compare_کلیدیں(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت Cache_کلید_بنیاد* p1 = static_cast<ثابت Cache_کلید_بنیاد*>(key1.اشارہ_گر);
    ثابت Cache_کلید_بنیاد* p2 = static_cast<ثابت Cache_کلید_بنیاد*>(key2.اشارہ_گر);
    واپس *p1 == *p2;
}

خالی U_EXPORT2
ucache_delete_کلید(خالی *obj) {
    Cache_کلید_بنیاد* p = static_cast<Cache_کلید_بنیاد*>(obj);
    حذف p;
}

Cache_کلید_بنیاد::~Cache_کلید_بنیاد() {
}

ساکن خالی U_CALLCONV cacheInit(UError_رمز &حالت_2) {
    U_ASSERT(gCache == خالی_اشاریہ);
    ucln_common_رجسٹر_Cleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_اندر_پیش_رفت_قدر_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = نیا UnifiedCache(حالت_2);
    اگر (gCache == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
    }
    اگر (U_FAILURE(حالت_2)) {
        حذف gCache;
        gCache = خالی_اشاریہ;
        واپس;
    }
}

UnifiedCache *UnifiedCache::getInstance(UError_رمز &حالت_2) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    U_ASSERT(gCache != خالی_اشاریہ);
    واپس gCache;
}

UnifiedCache::UnifiedCache(UError_رمز &حالت_2) :
        fHashtable(خالی_اشاریہ),
        fEvictPos(UHASH_پہلا),
        f_عدد_اقدار_کل(0),
        f_عدد_اقدار_اندر_Use(0),
        f_Max_غیر_مستعمل(DEFAULT_MAX_غیر_مستعمل),
        f_Max_Percentage_کا_اندر_Use(DEFAULT_PERCENTAGE_کا_اندر_USE),
        f_Auto_Evicted_گنتی(0),
        f_No_قدر(خالی_اشاریہ) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    f_No_قدر = نیا SharedObject();
    اگر (f_No_قدر == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    f_No_قدر->soft_Ref_گنتی = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_قدر->hard_Ref_گنتی = 1;  // when other references to it are removed.
    f_No_قدر->cachePtr = یہ;

    fHashtable = uhash_open(
            &ucache_hash_کلیدیں,
            &ucache_compare_کلیدیں,
            خالی_اشاریہ,
            &حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    uhash_سیٹ_کلید_Deleter(fHashtable, &ucache_delete_کلید);
}

خالی UnifiedCache::سیٹ_Eviction_Policy(
        int32_t count, int32_t percentage_کا_اندر_Use_اشیاء, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    اگر (count < 0 || percentage_کا_اندر_Use_اشیاء < 0) {
        حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    std::lock_guard<std::mutex> تالا(*gCacheMutex);
    f_Max_غیر_مستعمل = count;
    f_Max_Percentage_کا_اندر_Use = percentage_کا_اندر_Use_اشیاء;
}

int32_t UnifiedCache::غیر_مستعمل_گنتی() ثابت {
    std::lock_guard<std::mutex> تالا(*gCacheMutex);
    واپس uhash_گنتی(fHashtable) - f_عدد_اقدار_اندر_Use;
}

int64_t UnifiedCache::auto_Evicted_گنتی() ثابت {
    std::lock_guard<std::mutex> تالا(*gCacheMutex);
    واپس f_Auto_Evicted_گنتی;
}

int32_t UnifiedCache::کلید_گنتی() ثابت {
    std::lock_guard<std::mutex> تالا(*gCacheMutex);
    واپس uhash_گنتی(fHashtable);
}

خالی UnifiedCache::flush() ثابت {
    std::lock_guard<std::mutex> تالا(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    جب (_flush(جھوٹ));
}

خالی UnifiedCache::handleUnreferencedObject() ثابت {
    std::lock_guard<std::mutex> تالا(*gCacheMutex);
    --f_عدد_اقدار_اندر_Use;
    تسلسل_Eviction_قاش();
}

#اگر_تعریف_شدہ UNIFIED_CACHE_DEBUG
#شامل <stdio.h>

خالی UnifiedCache::dump() {
    UError_رمز حالت_2 = U_ZERO_ERROR;
    ثابت UnifiedCache *cache = getInstance(حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        واپس;
    }
    cache->dumpContents();
}

خالی UnifiedCache::dumpContents() ثابت {
    std::lock_guard<std::mutex> تالا(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
خالی UnifiedCache::_dumpContents() ثابت {
    int32_t pos = UHASH_پہلا;
    ثابت UHashElement *element = uhash_اگلا_Element(fHashtable, &pos);
    حرف بفر[256];
    int32_t cnt = 0;
    برائے (; element != خالی_اشاریہ; element = uhash_اگلا_Element(fHashtable, &pos)) {
        ثابت SharedObject *sharedObject =
                (ثابت SharedObject *) element->value.اشارہ_گر;
        ثابت Cache_کلید_بنیاد *کلید =
                (ثابت Cache_کلید_بنیاد *) element->کلید.اشارہ_گر;
        اگر (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    کلید->writeDescription(بفر, 256),
                    کلید->creation_حالت,
                    sharedObject == f_No_قدر ? خالی_اشاریہ :sharedObject,
                    sharedObject->get_Ref_گنتی(),
                    sharedObject->get_Soft_Ref_گنتی());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_گنتی(fHashtable));
}
#اگر_ختم

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> تالا(*gCacheMutex);
        _flush(سچ);
    }
    uhash_close(fHashtable);
    fHashtable = خالی_اشاریہ;
    حذف f_No_قدر;
    f_No_قدر = خالی_اشاریہ;
}

ثابت UHashElement *
UnifiedCache::اگلا_Element_2() ثابت {
    ثابت UHashElement *element = uhash_اگلا_Element(fHashtable, &fEvictPos);
    اگر (element == خالی_اشاریہ) {
        fEvictPos = UHASH_پہلا;
        واپس uhash_اگلا_Element(fHashtable, &fEvictPos);
    }
    واپس element;
}

UBool UnifiedCache::_flush(UBool all) ثابت {
    UBool نتیجہ = جھوٹ;
    int32_t orig_سائز = uhash_گنتی(fHashtable);
    برائے (int32_t i = 0; i < orig_سائز; ++i) {
        ثابت UHashElement *element = اگلا_Element_2();
        اگر (element == خالی_اشاریہ) {
            توڑو;
        }
        اگر (all || _isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشارہ_گر);
            U_ASSERT(sharedObject->cachePtr == یہ);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            نتیجہ = سچ;
        }
    }
    واپس نتیجہ;
}

int32_t UnifiedCache::compute_گنتی_کا_اشیاء_مقصد_Evict() ثابت {
    int32_t کل_اشیاء = uhash_گنتی(fHashtable);
    int32_t evictable_اشیاء = کل_اشیاء - f_عدد_اقدار_اندر_Use;

    int32_t غیر_مستعمل_حد_بندی_By_Percentage = f_عدد_اقدار_اندر_Use * f_Max_Percentage_کا_اندر_Use / 100;
    int32_t غیر_مستعمل_حد_بندی = std::max(غیر_مستعمل_حد_بندی_By_Percentage, f_Max_غیر_مستعمل);
    int32_t گنتی_کا_اشیاء_مقصد_Evict = std::max<int32_t>(0, evictable_اشیاء - غیر_مستعمل_حد_بندی);
    واپس گنتی_کا_اشیاء_مقصد_Evict;
}

خالی UnifiedCache::تسلسل_Eviction_قاش() ثابت {
    int32_t max_اشیاء_مقصد_Evict = compute_گنتی_کا_اشیاء_مقصد_Evict();
    اگر (max_اشیاء_مقصد_Evict <= 0) {
        واپس;
    }
    برائے (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ثابت UHashElement *element = اگلا_Element_2();
        اگر (element == خالی_اشاریہ) {
            توڑو;
        }
        اگر (_isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشارہ_گر);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_گنتی;
            اگر (--max_اشیاء_مقصد_Evict == 0) {
                توڑو;
            }
        }
    }
}

خالی UnifiedCache::_putNew(
        ثابت Cache_کلید_بنیاد &کلید,
        ثابت SharedObject *value,
        ثابت UError_رمز creation_حالت,
        UError_رمز &حالت_2) ثابت {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    Cache_کلید_بنیاد *کلید_مقصد_Adopt = کلید.clone();
    اگر (کلید_مقصد_Adopt == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    کلید_مقصد_Adopt->f_Creation_حالت = creation_حالت;
    اگر (value->soft_Ref_گنتی == 0) {
        رجسٹر_Primary(کلید_مقصد_Adopt, value);
    }
    خالی *old_قدر = uhash_put(fHashtable, کلید_مقصد_Adopt, (خالی *) value, &حالت_2);
    U_ASSERT(old_قدر == خالی_اشاریہ);
    (خالی)old_قدر;
    اگر (U_SUCCESS(حالت_2)) {
        value->soft_Ref_گنتی++;
    }
}

خالی UnifiedCache::_putIfAbsentAndGet(
        ثابت Cache_کلید_بنیاد &کلید,
        ثابت SharedObject *&value,
        UError_رمز &حالت_2) ثابت {
    std::lock_guard<std::mutex> تالا(*gCacheMutex);
    ثابت UHashElement *element = uhash_ڈھونڈو(fHashtable, &کلید);
    اگر (element != خالی_اشاریہ && !اندر_پیش_رفت(element)) {
        _fetch(element, value, حالت_2);
        واپس;
    }
    اگر (element == خالی_اشاریہ) {
        UError_رمز putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(کلید, value, حالت_2, putError);
    } ورنہ {
        _put(element, value, حالت_2);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    تسلسل_Eviction_قاش();
}


UBool UnifiedCache::_poll(
        ثابت Cache_کلید_بنیاد &کلید,
        ثابت SharedObject *&value,
        UError_رمز &حالت_2) ثابت {
    U_ASSERT(value == خالی_اشاریہ);
    U_ASSERT(حالت_2 == U_ZERO_ERROR);
    std::unique_lock<std::mutex> تالا(*gCacheMutex);
    ثابت UHashElement *element = uhash_ڈھونڈو(fHashtable, &کلید);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     جب (element != خالی_اشاریہ && اندر_پیش_رفت(element)) {
         g_اندر_پیش_رفت_قدر_Added_Cond->wait(تالا);
         element = uhash_ڈھونڈو(fHashtable, &کلید);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    اگر (element != خالی_اشاریہ) {
         _fetch(element, value, حالت_2);
        واپس سچ;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(کلید, f_No_قدر, U_ZERO_ERROR, حالت_2);
    واپس جھوٹ;
}

خالی UnifiedCache::_get(
        ثابت Cache_کلید_بنیاد &کلید,
        ثابت SharedObject *&value,
        ثابت خالی *creationContext,
        UError_رمز &حالت_2) ثابت {
    U_ASSERT(value == خالی_اشاریہ);
    U_ASSERT(حالت_2 == U_ZERO_ERROR);
    اگر (_poll(کلید, value, حالت_2)) {
        اگر (value == f_No_قدر) {
            SharedObject::clearPtr(value);
        }
        واپس;
    }
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    value = کلید.createObject(creationContext, حالت_2);
    U_ASSERT(value == خالی_اشاریہ || value->hasHardReferences());
    U_ASSERT(value != خالی_اشاریہ || حالت_2 != U_ZERO_ERROR);
    اگر (value == خالی_اشاریہ) {
        SharedObject::copyPtr(f_No_قدر, value);
    }
    _putIfAbsentAndGet(کلید, value, حالت_2);
    اگر (value == f_No_قدر) {
        SharedObject::clearPtr(value);
    }
}

خالی UnifiedCache::رجسٹر_Primary(
            ثابت Cache_کلید_بنیاد *the_کلید, ثابت SharedObject *value) ثابت {
    the_کلید->fIsPrimary = سچ;
    value->cachePtr = یہ;
    ++f_عدد_اقدار_کل;
    ++f_عدد_اقدار_اندر_Use;
}

خالی UnifiedCache::_put(
        ثابت UHashElement *element,
        ثابت SharedObject *value,
        ثابت UError_رمز حالت_2) ثابت {
    U_ASSERT(اندر_پیش_رفت(element));
    ثابت Cache_کلید_بنیاد* the_کلید = static_cast<ثابت Cache_کلید_بنیاد*>(element->کلید.اشارہ_گر);
    ثابت SharedObject* old_قدر = static_cast<ثابت SharedObject*>(element->value.اشارہ_گر);
    the_کلید->f_Creation_حالت = حالت_2;
    اگر (value->soft_Ref_گنتی == 0) {
        رجسٹر_Primary(the_کلید, value);
    }
    value->soft_Ref_گنتی++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.اشارہ_گر = (خالی *) value;
    U_ASSERT(old_قدر == f_No_قدر);
    removeSoftRef(old_قدر);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_اندر_پیش_رفت_قدر_Added_Cond->notify_all();
}

خالی UnifiedCache::_fetch(
        ثابت UHashElement *element,
        ثابت SharedObject *&value,
        UError_رمز &حالت_2) ثابت {
    ثابت Cache_کلید_بنیاد* the_کلید = static_cast<ثابت Cache_کلید_بنیاد*>(element->کلید.اشارہ_گر);
    حالت_2 = the_کلید->f_Creation_حالت;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ثابت SharedObject *>(element->value.اشارہ_گر);
    جوڑو_Hard_Ref(value);
}


UBool UnifiedCache::اندر_پیش_رفت(ثابت UHashElement* element) ثابت {
    UError_رمز حالت_2 = U_ZERO_ERROR;
    ثابت SharedObject * value = خالی_اشاریہ;
    _fetch(element, value, حالت_2);
    UBool نتیجہ = اندر_پیش_رفت(value, حالت_2);
    removeHardRef(value);
    واپس نتیجہ;
}

UBool UnifiedCache::اندر_پیش_رفت(
        ثابت SharedObject* the_قدر, UError_رمز creation_حالت) ثابت {
    واپس (the_قدر == f_No_قدر && creation_حالت == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ثابت UHashElement *element) ثابت
{
    ثابت Cache_کلید_بنیاد* the_کلید = static_cast<ثابت Cache_کلید_بنیاد*>(element->کلید.اشارہ_گر);
    ثابت SharedObject* the_قدر = static_cast<ثابت SharedObject*>(element->value.اشارہ_گر);

    // Entries that are under construction are never evictable
    اگر (اندر_پیش_رفت(the_قدر, the_کلید->f_Creation_حالت)) {
        واپس جھوٹ;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    واپس (!the_کلید->fIsPrimary || (the_قدر->soft_Ref_گنتی == 1 && the_قدر->noHardReferences()));
}

خالی UnifiedCache::removeSoftRef(ثابت SharedObject *value) ثابت {
    U_ASSERT(value->cachePtr == یہ);
    U_ASSERT(value->soft_Ref_گنتی > 0);
    اگر (--value->soft_Ref_گنتی == 0) {
        --f_عدد_اقدار_کل;
        اگر (value->noHardReferences()) {
            حذف value;
        } ورنہ {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = خالی_اشاریہ;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ثابت SharedObject *value) ثابت {
    صحیح ref_گنتی = 0;
    اگر (value) {
        ref_گنتی = umtx_atomic_dec(&value->hard_Ref_گنتی);
        U_ASSERT(ref_گنتی >= 0);
        اگر (ref_گنتی == 0) {
            --f_عدد_اقدار_اندر_Use;
        }
    }
    واپس ref_گنتی;
}

int32_t UnifiedCache::جوڑو_Hard_Ref(ثابت SharedObject *value) ثابت {
    صحیح ref_گنتی = 0;
    اگر (value) {
        ref_گنتی = umtx_atomic_inc(&value->hard_Ref_گنتی);
        U_ASSERT(ref_گنتی >= 1);
        اگر (ref_گنتی == 1) {
            f_عدد_اقدار_اندر_Use++;
        }
    }
    واپس ref_گنتی;
}

U_NAMESPACE_انتہا

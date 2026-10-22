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

ساكن icu::UnifiedCache *gCache = تش_اشاره;
ساكن std::mutex *gCacheMutex = تش_اشاره;
ساكن std::condition_variable *g_دننه_پرمختګ_ارزښت_Added_Cond;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن ثابت int32_t MAX_EVICT_ITERATIONS = 10;
ساكن ثابت int32_t DEFAULT_MAX_ناکارول_شوی = 1000;
ساكن ثابت int32_t DEFAULT_PERCENTAGE_د_دننه_USE = 100;


U_CDECL_BEGIN
ساكن UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    ړنګول gCache;
    gCache = تش_اشاره;
    gCacheMutex->~mutex();
    gCacheMutex = تش_اشاره;
    g_دننه_پرمختګ_ارزښت_Added_Cond->~condition_variable();
    g_دننه_پرمختګ_ارزښت_Added_Cond = تش_اشاره;
    أرجع ریښتیا;
}
U_CDECL_پای_ټکی


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_کلیګانې(ثابت UHashTok کلی) {
    ثابت Cache_کلی_بنسټ* ckey = static_cast<ثابت Cache_کلی_بنسټ*>(کلی.اشاره_ګر);
    أرجع ckey->hash_کوډ();
}

UBool U_EXPORT2
ucache_compare_کلیګانې(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت Cache_کلی_بنسټ* p1 = static_cast<ثابت Cache_کلی_بنسټ*>(key1.اشاره_ګر);
    ثابت Cache_کلی_بنسټ* p2 = static_cast<ثابت Cache_کلی_بنسټ*>(key2.اشاره_ګر);
    أرجع *p1 == *p2;
}

فراغ U_EXPORT2
ucache_delete_کلی(فراغ *obj) {
    Cache_کلی_بنسټ* p = static_cast<Cache_کلی_بنسټ*>(obj);
    ړنګول p;
}

Cache_کلی_بنسټ::~Cache_کلی_بنسټ() {
}

ساكن فراغ U_CALLCONV cacheInit(UError_کوډ &حالت) {
    U_ASSERT(gCache == تش_اشاره);
    ucln_common_ثبتوونکی_Cleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_دننه_پرمختګ_ارزښت_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = جديد UnifiedCache(حالت);
    إذا (gCache == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
    }
    إذا (U_FAILURE(حالت)) {
        ړنګول gCache;
        gCache = تش_اشاره;
        أرجع;
    }
}

UnifiedCache *UnifiedCache::getInstance(UError_کوډ &حالت) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    U_ASSERT(gCache != تش_اشاره);
    أرجع gCache;
}

UnifiedCache::UnifiedCache(UError_کوډ &حالت) :
        fHashtable(تش_اشاره),
        fEvictPos(UHASH_لومړی),
        f_عدد_ارزښتونه_ټول(0),
        f_عدد_ارزښتونه_دننه_Use(0),
        f_Max_ناکارول_شوی(DEFAULT_MAX_ناکارول_شوی),
        f_Max_Percentage_د_دننه_Use(DEFAULT_PERCENTAGE_د_دننه_USE),
        f_Auto_Evicted_شمېر(0),
        f_No_ارزښت(تش_اشاره) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    f_No_ارزښت = جديد SharedObject();
    إذا (f_No_ارزښت == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    f_No_ارزښت->soft_Ref_شمېر = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_ارزښت->hard_Ref_شمېر = 1;  // when other references to it are removed.
    f_No_ارزښت->cachePtr = هذا;

    fHashtable = uhash_open(
            &ucache_hash_کلیګانې,
            &ucache_compare_کلیګانې,
            تش_اشاره,
            &حالت);
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    uhash_سېټ_کلی_Deleter(fHashtable, &ucache_delete_کلی);
}

فراغ UnifiedCache::سېټ_Eviction_Policy(
        int32_t count, int32_t percentage_د_دننه_Use_توکي, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    إذا (count < 0 || percentage_د_دننه_Use_توکي < 0) {
        حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    std::lock_guard<std::mutex> قلف(*gCacheMutex);
    f_Max_ناکارول_شوی = count;
    f_Max_Percentage_د_دننه_Use = percentage_د_دننه_Use_توکي;
}

int32_t UnifiedCache::ناکارول_شوی_شمېر() ثابت {
    std::lock_guard<std::mutex> قلف(*gCacheMutex);
    أرجع uhash_شمېر(fHashtable) - f_عدد_ارزښتونه_دننه_Use;
}

int64_t UnifiedCache::auto_Evicted_شمېر() ثابت {
    std::lock_guard<std::mutex> قلف(*gCacheMutex);
    أرجع f_Auto_Evicted_شمېر;
}

int32_t UnifiedCache::کلی_شمېر() ثابت {
    std::lock_guard<std::mutex> قلف(*gCacheMutex);
    أرجع uhash_شمېر(fHashtable);
}

فراغ UnifiedCache::flush() ثابت {
    std::lock_guard<std::mutex> قلف(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    طالما (_flush(خطأ));
}

فراغ UnifiedCache::handleUnreferencedObject() ثابت {
    std::lock_guard<std::mutex> قلف(*gCacheMutex);
    --f_عدد_ارزښتونه_دننه_Use;
    پرلپسې_Eviction_ټوټه();
}

#که_تعریف_شوی UNIFIED_CACHE_DEBUG
#ضمن <stdio.h>

فراغ UnifiedCache::dump() {
    UError_کوډ حالت = U_ZERO_ERROR;
    ثابت UnifiedCache *cache = getInstance(حالت);
    إذا (U_FAILURE(حالت)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        أرجع;
    }
    cache->dumpContents();
}

فراغ UnifiedCache::dumpContents() ثابت {
    std::lock_guard<std::mutex> قلف(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
فراغ UnifiedCache::_dumpContents() ثابت {
    int32_t pos = UHASH_لومړی;
    ثابت UHashElement *element = uhash_بل_Element(fHashtable, &pos);
    حرف بفر[256];
    int32_t cnt = 0;
    لكل (; element != تش_اشاره; element = uhash_بل_Element(fHashtable, &pos)) {
        ثابت SharedObject *sharedObject =
                (ثابت SharedObject *) element->value.اشاره_ګر;
        ثابت Cache_کلی_بنسټ *کلی =
                (ثابت Cache_کلی_بنسټ *) element->کلی.اشاره_ګر;
        إذا (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    کلی->writeDescription(بفر, 256),
                    کلی->creation_حالت,
                    sharedObject == f_No_ارزښت ? تش_اشاره :sharedObject,
                    sharedObject->get_Ref_شمېر(),
                    sharedObject->get_Soft_Ref_شمېر());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_شمېر(fHashtable));
}
#د_که_پای

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> قلف(*gCacheMutex);
        _flush(ریښتیا);
    }
    uhash_close(fHashtable);
    fHashtable = تش_اشاره;
    ړنګول f_No_ارزښت;
    f_No_ارزښت = تش_اشاره;
}

ثابت UHashElement *
UnifiedCache::بل_Element_2() ثابت {
    ثابت UHashElement *element = uhash_بل_Element(fHashtable, &fEvictPos);
    إذا (element == تش_اشاره) {
        fEvictPos = UHASH_لومړی;
        أرجع uhash_بل_Element(fHashtable, &fEvictPos);
    }
    أرجع element;
}

UBool UnifiedCache::_flush(UBool all) ثابت {
    UBool پایله = خطأ;
    int32_t orig_اندازه = uhash_شمېر(fHashtable);
    لكل (int32_t i = 0; i < orig_اندازه; ++i) {
        ثابت UHashElement *element = بل_Element_2();
        إذا (element == تش_اشاره) {
            اكسر;
        }
        إذا (all || _isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشاره_ګر);
            U_ASSERT(sharedObject->cachePtr == هذا);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            پایله = ریښتیا;
        }
    }
    أرجع پایله;
}

int32_t UnifiedCache::compute_شمېر_د_توکي_منزل_Evict() ثابت {
    int32_t ټول_توکي = uhash_شمېر(fHashtable);
    int32_t evictable_توکي = ټول_توکي - f_عدد_ارزښتونه_دننه_Use;

    int32_t ناکارول_شوی_حد_By_Percentage = f_عدد_ارزښتونه_دننه_Use * f_Max_Percentage_د_دننه_Use / 100;
    int32_t ناکارول_شوی_حد = std::max(ناکارول_شوی_حد_By_Percentage, f_Max_ناکارول_شوی);
    int32_t شمېر_د_توکي_منزل_Evict = std::max<int32_t>(0, evictable_توکي - ناکارول_شوی_حد);
    أرجع شمېر_د_توکي_منزل_Evict;
}

فراغ UnifiedCache::پرلپسې_Eviction_ټوټه() ثابت {
    int32_t max_توکي_منزل_Evict = compute_شمېر_د_توکي_منزل_Evict();
    إذا (max_توکي_منزل_Evict <= 0) {
        أرجع;
    }
    لكل (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ثابت UHashElement *element = بل_Element_2();
        إذا (element == تش_اشاره) {
            اكسر;
        }
        إذا (_isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشاره_ګر);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_شمېر;
            إذا (--max_توکي_منزل_Evict == 0) {
                اكسر;
            }
        }
    }
}

فراغ UnifiedCache::_putNew(
        ثابت Cache_کلی_بنسټ &کلی,
        ثابت SharedObject *value,
        ثابت UError_کوډ creation_حالت,
        UError_کوډ &حالت) ثابت {
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    Cache_کلی_بنسټ *کلی_منزل_Adopt = کلی.clone();
    إذا (کلی_منزل_Adopt == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    کلی_منزل_Adopt->f_Creation_حالت = creation_حالت;
    إذا (value->soft_Ref_شمېر == 0) {
        ثبتوونکی_Primary(کلی_منزل_Adopt, value);
    }
    فراغ *old_ارزښت = uhash_put(fHashtable, کلی_منزل_Adopt, (فراغ *) value, &حالت);
    U_ASSERT(old_ارزښت == تش_اشاره);
    (فراغ)old_ارزښت;
    إذا (U_SUCCESS(حالت)) {
        value->soft_Ref_شمېر++;
    }
}

فراغ UnifiedCache::_putIfAbsentAndGet(
        ثابت Cache_کلی_بنسټ &کلی,
        ثابت SharedObject *&value,
        UError_کوډ &حالت) ثابت {
    std::lock_guard<std::mutex> قلف(*gCacheMutex);
    ثابت UHashElement *element = uhash_پیدا_کړه(fHashtable, &کلی);
    إذا (element != تش_اشاره && !دننه_پرمختګ(element)) {
        _fetch(element, value, حالت);
        أرجع;
    }
    إذا (element == تش_اشاره) {
        UError_کوډ putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(کلی, value, حالت, putError);
    } وإلا {
        _put(element, value, حالت);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    پرلپسې_Eviction_ټوټه();
}


UBool UnifiedCache::_poll(
        ثابت Cache_کلی_بنسټ &کلی,
        ثابت SharedObject *&value,
        UError_کوډ &حالت) ثابت {
    U_ASSERT(value == تش_اشاره);
    U_ASSERT(حالت == U_ZERO_ERROR);
    std::unique_lock<std::mutex> قلف(*gCacheMutex);
    ثابت UHashElement *element = uhash_پیدا_کړه(fHashtable, &کلی);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     طالما (element != تش_اشاره && دننه_پرمختګ(element)) {
         g_دننه_پرمختګ_ارزښت_Added_Cond->wait(قلف);
         element = uhash_پیدا_کړه(fHashtable, &کلی);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    إذا (element != تش_اشاره) {
         _fetch(element, value, حالت);
        أرجع ریښتیا;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(کلی, f_No_ارزښت, U_ZERO_ERROR, حالت);
    أرجع خطأ;
}

فراغ UnifiedCache::_get(
        ثابت Cache_کلی_بنسټ &کلی,
        ثابت SharedObject *&value,
        ثابت فراغ *creationContext,
        UError_کوډ &حالت) ثابت {
    U_ASSERT(value == تش_اشاره);
    U_ASSERT(حالت == U_ZERO_ERROR);
    إذا (_poll(کلی, value, حالت)) {
        إذا (value == f_No_ارزښت) {
            SharedObject::clearPtr(value);
        }
        أرجع;
    }
    إذا (U_FAILURE(حالت)) {
        أرجع;
    }
    value = کلی.createObject(creationContext, حالت);
    U_ASSERT(value == تش_اشاره || value->hasHardReferences());
    U_ASSERT(value != تش_اشاره || حالت != U_ZERO_ERROR);
    إذا (value == تش_اشاره) {
        SharedObject::copyPtr(f_No_ارزښت, value);
    }
    _putIfAbsentAndGet(کلی, value, حالت);
    إذا (value == f_No_ارزښت) {
        SharedObject::clearPtr(value);
    }
}

فراغ UnifiedCache::ثبتوونکی_Primary(
            ثابت Cache_کلی_بنسټ *the_کلی, ثابت SharedObject *value) ثابت {
    the_کلی->fIsPrimary = ریښتیا;
    value->cachePtr = هذا;
    ++f_عدد_ارزښتونه_ټول;
    ++f_عدد_ارزښتونه_دننه_Use;
}

فراغ UnifiedCache::_put(
        ثابت UHashElement *element,
        ثابت SharedObject *value,
        ثابت UError_کوډ حالت) ثابت {
    U_ASSERT(دننه_پرمختګ(element));
    ثابت Cache_کلی_بنسټ* the_کلی = static_cast<ثابت Cache_کلی_بنسټ*>(element->کلی.اشاره_ګر);
    ثابت SharedObject* old_ارزښت = static_cast<ثابت SharedObject*>(element->value.اشاره_ګر);
    the_کلی->f_Creation_حالت = حالت;
    إذا (value->soft_Ref_شمېر == 0) {
        ثبتوونکی_Primary(the_کلی, value);
    }
    value->soft_Ref_شمېر++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.اشاره_ګر = (فراغ *) value;
    U_ASSERT(old_ارزښت == f_No_ارزښت);
    removeSoftRef(old_ارزښت);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_دننه_پرمختګ_ارزښت_Added_Cond->notify_all();
}

فراغ UnifiedCache::_fetch(
        ثابت UHashElement *element,
        ثابت SharedObject *&value,
        UError_کوډ &حالت) ثابت {
    ثابت Cache_کلی_بنسټ* the_کلی = static_cast<ثابت Cache_کلی_بنسټ*>(element->کلی.اشاره_ګر);
    حالت = the_کلی->f_Creation_حالت;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ثابت SharedObject *>(element->value.اشاره_ګر);
    جمع_کړه_Hard_Ref(value);
}


UBool UnifiedCache::دننه_پرمختګ(ثابت UHashElement* element) ثابت {
    UError_کوډ حالت = U_ZERO_ERROR;
    ثابت SharedObject * value = تش_اشاره;
    _fetch(element, value, حالت);
    UBool پایله = دننه_پرمختګ(value, حالت);
    removeHardRef(value);
    أرجع پایله;
}

UBool UnifiedCache::دننه_پرمختګ(
        ثابت SharedObject* the_ارزښت, UError_کوډ creation_حالت) ثابت {
    أرجع (the_ارزښت == f_No_ارزښت && creation_حالت == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ثابت UHashElement *element) ثابت
{
    ثابت Cache_کلی_بنسټ* the_کلی = static_cast<ثابت Cache_کلی_بنسټ*>(element->کلی.اشاره_ګر);
    ثابت SharedObject* the_ارزښت = static_cast<ثابت SharedObject*>(element->value.اشاره_ګر);

    // Entries that are under construction are never evictable
    إذا (دننه_پرمختګ(the_ارزښت, the_کلی->f_Creation_حالت)) {
        أرجع خطأ;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    أرجع (!the_کلی->fIsPrimary || (the_ارزښت->soft_Ref_شمېر == 1 && the_ارزښت->noHardReferences()));
}

فراغ UnifiedCache::removeSoftRef(ثابت SharedObject *value) ثابت {
    U_ASSERT(value->cachePtr == هذا);
    U_ASSERT(value->soft_Ref_شمېر > 0);
    إذا (--value->soft_Ref_شمېر == 0) {
        --f_عدد_ارزښتونه_ټول;
        إذا (value->noHardReferences()) {
            ړنګول value;
        } وإلا {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = تش_اشاره;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ثابت SharedObject *value) ثابت {
    صحيح ref_شمېر = 0;
    إذا (value) {
        ref_شمېر = umtx_atomic_dec(&value->hard_Ref_شمېر);
        U_ASSERT(ref_شمېر >= 0);
        إذا (ref_شمېر == 0) {
            --f_عدد_ارزښتونه_دننه_Use;
        }
    }
    أرجع ref_شمېر;
}

int32_t UnifiedCache::جمع_کړه_Hard_Ref(ثابت SharedObject *value) ثابت {
    صحيح ref_شمېر = 0;
    إذا (value) {
        ref_شمېر = umtx_atomic_inc(&value->hard_Ref_شمېر);
        U_ASSERT(ref_شمېر >= 1);
        إذا (ref_شمېر == 1) {
            f_عدد_ارزښتونه_دننه_Use++;
        }
    }
    أرجع ref_شمېر;
}

U_NAMESPACE_پای_ټکی

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

ساكن icu::UnifiedCache *gCache = اشاره_تهی;
ساكن std::mutex *gCacheMutex = اشاره_تهی;
ساكن std::condition_variable *g_درون_پیشرفت_مقدار_Added_Cond;
ساكن icu::UInitOnce gCacheInitOnce {};

ساكن ثابت int32_t MAX_EVICT_ITERATIONS = 10;
ساكن ثابت int32_t DEFAULT_MAX_استفادهنشده = 1000;
ساكن ثابت int32_t DEFAULT_PERCENTAGE_از_درون_USE = 100;


U_CDECL_BEGIN
ساكن UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    حذف gCache;
    gCache = اشاره_تهی;
    gCacheMutex->~mutex();
    gCacheMutex = اشاره_تهی;
    g_درون_پیشرفت_مقدار_Added_Cond->~condition_variable();
    g_درون_پیشرفت_مقدار_Added_Cond = اشاره_تهی;
    أرجع درست;
}
U_CDECL_انتها


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_کلیدها(ثابت UHashTok کلید) {
    ثابت Cache_کلید_پایه* ckey = static_cast<ثابت Cache_کلید_پایه*>(کلید.اشاره_گر);
    أرجع ckey->hash_کد();
}

UBool U_EXPORT2
ucache_compare_کلیدها(ثابت UHashTok key1, ثابت UHashTok key2) {
    ثابت Cache_کلید_پایه* p1 = static_cast<ثابت Cache_کلید_پایه*>(key1.اشاره_گر);
    ثابت Cache_کلید_پایه* p2 = static_cast<ثابت Cache_کلید_پایه*>(key2.اشاره_گر);
    أرجع *p1 == *p2;
}

فراغ U_EXPORT2
ucache_delete_کلید(فراغ *obj) {
    Cache_کلید_پایه* p = static_cast<Cache_کلید_پایه*>(obj);
    حذف p;
}

Cache_کلید_پایه::~Cache_کلید_پایه() {
}

ساكن فراغ U_CALLCONV cacheInit(UError_کد &وضعیت) {
    U_ASSERT(gCache == اشاره_تهی);
    ucln_common_ثبات_Cleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_درون_پیشرفت_مقدار_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = جديد UnifiedCache(وضعیت);
    إذا (gCache == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
    }
    إذا (U_FAILURE(وضعیت)) {
        حذف gCache;
        gCache = اشاره_تهی;
        أرجع;
    }
}

UnifiedCache *UnifiedCache::getInstance(UError_کد &وضعیت) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    U_ASSERT(gCache != اشاره_تهی);
    أرجع gCache;
}

UnifiedCache::UnifiedCache(UError_کد &وضعیت) :
        fHashtable(اشاره_تهی),
        fEvictPos(UHASH_اولی),
        f_رقم_مقادیر_مجموع(0),
        f_رقم_مقادیر_درون_Use(0),
        f_Max_استفادهنشده(DEFAULT_MAX_استفادهنشده),
        f_Max_Percentage_از_درون_Use(DEFAULT_PERCENTAGE_از_درون_USE),
        f_Auto_Evicted_شمار(0),
        f_No_مقدار(اشاره_تهی) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    f_No_مقدار = جديد SharedObject();
    إذا (f_No_مقدار == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    f_No_مقدار->soft_Ref_شمار = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_مقدار->hard_Ref_شمار = 1;  // when other references to it are removed.
    f_No_مقدار->cachePtr = هذا;

    fHashtable = uhash_open(
            &ucache_hash_کلیدها,
            &ucache_compare_کلیدها,
            اشاره_تهی,
            &وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    uhash_مجموعه_کلید_Deleter(fHashtable, &ucache_delete_کلید);
}

فراغ UnifiedCache::مجموعه_Eviction_Policy(
        int32_t count, int32_t percentage_از_درون_Use_اقلام, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    إذا (count < 0 || percentage_از_درون_Use_اقلام < 0) {
        وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }
    std::lock_guard<std::mutex> قفل(*gCacheMutex);
    f_Max_استفادهنشده = count;
    f_Max_Percentage_از_درون_Use = percentage_از_درون_Use_اقلام;
}

int32_t UnifiedCache::استفادهنشده_شمار() ثابت {
    std::lock_guard<std::mutex> قفل(*gCacheMutex);
    أرجع uhash_شمار(fHashtable) - f_رقم_مقادیر_درون_Use;
}

int64_t UnifiedCache::auto_Evicted_شمار() ثابت {
    std::lock_guard<std::mutex> قفل(*gCacheMutex);
    أرجع f_Auto_Evicted_شمار;
}

int32_t UnifiedCache::کلید_شمار() ثابت {
    std::lock_guard<std::mutex> قفل(*gCacheMutex);
    أرجع uhash_شمار(fHashtable);
}

فراغ UnifiedCache::flush() ثابت {
    std::lock_guard<std::mutex> قفل(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    طالما (_flush(خطأ));
}

فراغ UnifiedCache::handleUnreferencedObject() ثابت {
    std::lock_guard<std::mutex> قفل(*gCacheMutex);
    --f_رقم_مقادیر_درون_Use;
    پیاپی_Eviction_برش();
}

#اگر_تعریف_شده UNIFIED_CACHE_DEBUG
#ضمن <stdio.h>

فراغ UnifiedCache::dump() {
    UError_کد وضعیت = U_ZERO_ERROR;
    ثابت UnifiedCache *cache = getInstance(وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        fprintf(stderr, "Unified Cache: Error fetching cache.\n");
        أرجع;
    }
    cache->dumpContents();
}

فراغ UnifiedCache::dumpContents() ثابت {
    std::lock_guard<std::mutex> قفل(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
فراغ UnifiedCache::_dumpContents() ثابت {
    int32_t pos = UHASH_اولی;
    ثابت UHashElement *element = uhash_بعدی_Element(fHashtable, &pos);
    حرف میانگیر[256];
    int32_t cnt = 0;
    لكل (; element != اشاره_تهی; element = uhash_بعدی_Element(fHashtable, &pos)) {
        ثابت SharedObject *sharedObject =
                (ثابت SharedObject *) element->value.اشاره_گر;
        ثابت Cache_کلید_پایه *کلید =
                (ثابت Cache_کلید_پایه *) element->کلید.اشاره_گر;
        إذا (sharedObject->hasHardReferences()) {
            ++cnt;
            fprintf(
                    stderr,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    کلید->writeDescription(میانگیر, 256),
                    کلید->creation_وضعیت,
                    sharedObject == f_No_مقدار ? اشاره_تهی :sharedObject,
                    sharedObject->get_Ref_شمار(),
                    sharedObject->get_Soft_Ref_شمار());
        }
    }
    fprintf(stderr, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_شمار(fHashtable));
}
#پایان_اگر

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    flush();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> قفل(*gCacheMutex);
        _flush(درست);
    }
    uhash_close(fHashtable);
    fHashtable = اشاره_تهی;
    حذف f_No_مقدار;
    f_No_مقدار = اشاره_تهی;
}

ثابت UHashElement *
UnifiedCache::بعدی_Element_2() ثابت {
    ثابت UHashElement *element = uhash_بعدی_Element(fHashtable, &fEvictPos);
    إذا (element == اشاره_تهی) {
        fEvictPos = UHASH_اولی;
        أرجع uhash_بعدی_Element(fHashtable, &fEvictPos);
    }
    أرجع element;
}

UBool UnifiedCache::_flush(UBool all) ثابت {
    UBool نتیجه = خطأ;
    int32_t orig_اندازه = uhash_شمار(fHashtable);
    لكل (int32_t i = 0; i < orig_اندازه; ++i) {
        ثابت UHashElement *element = بعدی_Element_2();
        إذا (element == اشاره_تهی) {
            اكسر;
        }
        إذا (all || _isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشاره_گر);
            U_ASSERT(sharedObject->cachePtr == هذا);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            نتیجه = درست;
        }
    }
    أرجع نتیجه;
}

int32_t UnifiedCache::compute_شمار_از_اقلام_مقصد_Evict() ثابت {
    int32_t مجموع_اقلام = uhash_شمار(fHashtable);
    int32_t evictable_اقلام = مجموع_اقلام - f_رقم_مقادیر_درون_Use;

    int32_t استفادهنشده_حد_By_Percentage = f_رقم_مقادیر_درون_Use * f_Max_Percentage_از_درون_Use / 100;
    int32_t استفادهنشده_حد = std::max(استفادهنشده_حد_By_Percentage, f_Max_استفادهنشده);
    int32_t شمار_از_اقلام_مقصد_Evict = std::max<int32_t>(0, evictable_اقلام - استفادهنشده_حد);
    أرجع شمار_از_اقلام_مقصد_Evict;
}

فراغ UnifiedCache::پیاپی_Eviction_برش() ثابت {
    int32_t max_اقلام_مقصد_Evict = compute_شمار_از_اقلام_مقصد_Evict();
    إذا (max_اقلام_مقصد_Evict <= 0) {
        أرجع;
    }
    لكل (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        ثابت UHashElement *element = بعدی_Element_2();
        إذا (element == اشاره_تهی) {
            اكسر;
        }
        إذا (_isEvictable(element)) {
            ثابت SharedObject *sharedObject =
                    static_cast<ثابت SharedObject*>(element->value.اشاره_گر);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_شمار;
            إذا (--max_اقلام_مقصد_Evict == 0) {
                اكسر;
            }
        }
    }
}

فراغ UnifiedCache::_putNew(
        ثابت Cache_کلید_پایه &کلید,
        ثابت SharedObject *value,
        ثابت UError_کد creation_وضعیت,
        UError_کد &وضعیت) ثابت {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    Cache_کلید_پایه *کلید_مقصد_Adopt = کلید.clone();
    إذا (کلید_مقصد_Adopt == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    کلید_مقصد_Adopt->f_Creation_وضعیت = creation_وضعیت;
    إذا (value->soft_Ref_شمار == 0) {
        ثبات_Primary(کلید_مقصد_Adopt, value);
    }
    فراغ *old_مقدار = uhash_put(fHashtable, کلید_مقصد_Adopt, (فراغ *) value, &وضعیت);
    U_ASSERT(old_مقدار == اشاره_تهی);
    (فراغ)old_مقدار;
    إذا (U_SUCCESS(وضعیت)) {
        value->soft_Ref_شمار++;
    }
}

فراغ UnifiedCache::_putIfAbsentAndGet(
        ثابت Cache_کلید_پایه &کلید,
        ثابت SharedObject *&value,
        UError_کد &وضعیت) ثابت {
    std::lock_guard<std::mutex> قفل(*gCacheMutex);
    ثابت UHashElement *element = uhash_پیدا_کن(fHashtable, &کلید);
    إذا (element != اشاره_تهی && !درون_پیشرفت(element)) {
        _fetch(element, value, وضعیت);
        أرجع;
    }
    إذا (element == اشاره_تهی) {
        UError_کد putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(کلید, value, وضعیت, putError);
    } وإلا {
        _put(element, value, وضعیت);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    پیاپی_Eviction_برش();
}


UBool UnifiedCache::_poll(
        ثابت Cache_کلید_پایه &کلید,
        ثابت SharedObject *&value,
        UError_کد &وضعیت) ثابت {
    U_ASSERT(value == اشاره_تهی);
    U_ASSERT(وضعیت == U_ZERO_ERROR);
    std::unique_lock<std::mutex> قفل(*gCacheMutex);
    ثابت UHashElement *element = uhash_پیدا_کن(fHashtable, &کلید);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     طالما (element != اشاره_تهی && درون_پیشرفت(element)) {
         g_درون_پیشرفت_مقدار_Added_Cond->wait(قفل);
         element = uhash_پیدا_کن(fHashtable, &کلید);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    إذا (element != اشاره_تهی) {
         _fetch(element, value, وضعیت);
        أرجع درست;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(کلید, f_No_مقدار, U_ZERO_ERROR, وضعیت);
    أرجع خطأ;
}

فراغ UnifiedCache::_get(
        ثابت Cache_کلید_پایه &کلید,
        ثابت SharedObject *&value,
        ثابت فراغ *creationContext,
        UError_کد &وضعیت) ثابت {
    U_ASSERT(value == اشاره_تهی);
    U_ASSERT(وضعیت == U_ZERO_ERROR);
    إذا (_poll(کلید, value, وضعیت)) {
        إذا (value == f_No_مقدار) {
            SharedObject::clearPtr(value);
        }
        أرجع;
    }
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    value = کلید.createObject(creationContext, وضعیت);
    U_ASSERT(value == اشاره_تهی || value->hasHardReferences());
    U_ASSERT(value != اشاره_تهی || وضعیت != U_ZERO_ERROR);
    إذا (value == اشاره_تهی) {
        SharedObject::copyPtr(f_No_مقدار, value);
    }
    _putIfAbsentAndGet(کلید, value, وضعیت);
    إذا (value == f_No_مقدار) {
        SharedObject::clearPtr(value);
    }
}

فراغ UnifiedCache::ثبات_Primary(
            ثابت Cache_کلید_پایه *the_کلید, ثابت SharedObject *value) ثابت {
    the_کلید->fIsPrimary = درست;
    value->cachePtr = هذا;
    ++f_رقم_مقادیر_مجموع;
    ++f_رقم_مقادیر_درون_Use;
}

فراغ UnifiedCache::_put(
        ثابت UHashElement *element,
        ثابت SharedObject *value,
        ثابت UError_کد وضعیت) ثابت {
    U_ASSERT(درون_پیشرفت(element));
    ثابت Cache_کلید_پایه* the_کلید = static_cast<ثابت Cache_کلید_پایه*>(element->کلید.اشاره_گر);
    ثابت SharedObject* old_مقدار = static_cast<ثابت SharedObject*>(element->value.اشاره_گر);
    the_کلید->f_Creation_وضعیت = وضعیت;
    إذا (value->soft_Ref_شمار == 0) {
        ثبات_Primary(the_کلید, value);
    }
    value->soft_Ref_شمار++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.اشاره_گر = (فراغ *) value;
    U_ASSERT(old_مقدار == f_No_مقدار);
    removeSoftRef(old_مقدار);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_درون_پیشرفت_مقدار_Added_Cond->notify_all();
}

فراغ UnifiedCache::_fetch(
        ثابت UHashElement *element,
        ثابت SharedObject *&value,
        UError_کد &وضعیت) ثابت {
    ثابت Cache_کلید_پایه* the_کلید = static_cast<ثابت Cache_کلید_پایه*>(element->کلید.اشاره_گر);
    وضعیت = the_کلید->f_Creation_وضعیت;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<ثابت SharedObject *>(element->value.اشاره_گر);
    جمع_کن_Hard_Ref(value);
}


UBool UnifiedCache::درون_پیشرفت(ثابت UHashElement* element) ثابت {
    UError_کد وضعیت = U_ZERO_ERROR;
    ثابت SharedObject * value = اشاره_تهی;
    _fetch(element, value, وضعیت);
    UBool نتیجه = درون_پیشرفت(value, وضعیت);
    removeHardRef(value);
    أرجع نتیجه;
}

UBool UnifiedCache::درون_پیشرفت(
        ثابت SharedObject* the_مقدار, UError_کد creation_وضعیت) ثابت {
    أرجع (the_مقدار == f_No_مقدار && creation_وضعیت == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(ثابت UHashElement *element) ثابت
{
    ثابت Cache_کلید_پایه* the_کلید = static_cast<ثابت Cache_کلید_پایه*>(element->کلید.اشاره_گر);
    ثابت SharedObject* the_مقدار = static_cast<ثابت SharedObject*>(element->value.اشاره_گر);

    // Entries that are under construction are never evictable
    إذا (درون_پیشرفت(the_مقدار, the_کلید->f_Creation_وضعیت)) {
        أرجع خطأ;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    أرجع (!the_کلید->fIsPrimary || (the_مقدار->soft_Ref_شمار == 1 && the_مقدار->noHardReferences()));
}

فراغ UnifiedCache::removeSoftRef(ثابت SharedObject *value) ثابت {
    U_ASSERT(value->cachePtr == هذا);
    U_ASSERT(value->soft_Ref_شمار > 0);
    إذا (--value->soft_Ref_شمار == 0) {
        --f_رقم_مقادیر_مجموع;
        إذا (value->noHardReferences()) {
            حذف value;
        } وإلا {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = اشاره_تهی;
        }
    }
}

int32_t UnifiedCache::removeHardRef(ثابت SharedObject *value) ثابت {
    صحيح ref_شمار = 0;
    إذا (value) {
        ref_شمار = umtx_atomic_dec(&value->hard_Ref_شمار);
        U_ASSERT(ref_شمار >= 0);
        إذا (ref_شمار == 0) {
            --f_رقم_مقادیر_درون_Use;
        }
    }
    أرجع ref_شمار;
}

int32_t UnifiedCache::جمع_کن_Hard_Ref(ثابت SharedObject *value) ثابت {
    صحيح ref_شمار = 0;
    إذا (value) {
        ref_شمار = umtx_atomic_inc(&value->hard_Ref_شمار);
        U_ASSERT(ref_شمار >= 1);
        إذا (ref_شمار == 1) {
            f_رقم_مقادیر_درون_Use++;
        }
    }
    أرجع ref_شمار;
}

U_NAMESPACE_انتها

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
स्थिर std::condition_variable *g_In_Progress_मान_Added_Cond;
स्थिर icu::UInitOnce gCacheInitOnce {};

स्थिर अचर int32_t MAX_EVICT_ITERATIONS = 10;
स्थिर अचर int32_t DEFAULT_MAX_UNUSED = 1000;
स्थिर अचर int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
स्थिर UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_मान_Added_Cond->~condition_variable();
    g_In_Progress_मान_Added_Cond = nullptr;
    फर्काऊ सत्य;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_साँचोहरू(अचर UHashTok साँचो) {
    अचर Cache_साँचो_Base* ckey = static_cast<अचर Cache_साँचो_Base*>(साँचो.pointer);
    फर्काऊ ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_साँचोहरू(अचर UHashTok key1, अचर UHashTok key2) {
    अचर Cache_साँचो_Base* p1 = static_cast<अचर Cache_साँचो_Base*>(key1.pointer);
    अचर Cache_साँचो_Base* p2 = static_cast<अचर Cache_साँचो_Base*>(key2.pointer);
    फर्काऊ *p1 == *p2;
}

शून्य U_EXPORT2
ucache_delete_साँचो(शून्य *obj) {
    Cache_साँचो_Base* p = static_cast<Cache_साँचो_Base*>(obj);
    delete p;
}

Cache_साँचो_Base::~Cache_साँचो_Base() {
}

स्थिर शून्य U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_मान_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = नूतन UnifiedCache(status);
    यदि (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    यदि (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        फर्काऊ;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }
    U_ASSERT(gCache != nullptr);
    फर्काऊ gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_अङ्क_मानहरू_Total(0),
        f_अङ्क_मानहरू_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_गन्ती(0),
        f_No_मान(nullptr) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    f_No_मान = नूतन SharedObject();
    यदि (f_No_मान == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    f_No_मान->soft_Ref_गन्ती = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_मान->hard_Ref_गन्ती = 1;  // when other references to it are removed.
    f_No_मान->cachePtr = एतत्;

    fHashtable = uhash_open(
            &ucache_hash_साँचोहरू,
            &ucache_compare_साँचोहरू,
            nullptr,
            &status);
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    uhash_set_साँचो_Deleter(fHashtable, &ucache_delete_साँचो);
}

शून्य UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_वस्तुहरू, UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    यदि (count < 0 || percentage_Of_In_Use_वस्तुहरू < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_वस्तुहरू;
}

int32_t UnifiedCache::unused_गन्ती() अचर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    फर्काऊ uhash_गन्ती(fHashtable) - f_अङ्क_मानहरू_In_Use;
}

int64_t UnifiedCache::auto_Evicted_गन्ती() अचर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    फर्काऊ f_Auto_Evicted_गन्ती;
}

int32_t UnifiedCache::साँचो_गन्ती() अचर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    फर्काऊ uhash_गन्ती(fHashtable);
}

शून्य UnifiedCache::flush() अचर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    जबसम्म (_flush(असत्य));
}

शून्य UnifiedCache::handleUnreferencedObject() अचर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_अङ्क_मानहरू_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#अन्तर्भाव <stdio.h>

शून्य UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    अचर UnifiedCache *cache = getInstance(status);
    यदि (U_FAILURE(status)) {
        ख_लिखो(मानक_त्रुटि, "Unified Cache: Error fetching cache.\n");
        फर्काऊ;
    }
    cache->dumpContents();
}

शून्य UnifiedCache::dumpContents() अचर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
शून्य UnifiedCache::_dumpContents() अचर {
    int32_t pos = UHASH_FIRST;
    अचर UHashElement *element = uhash_nextElement(fHashtable, &pos);
    वर्ण buffer[256];
    int32_t cnt = 0;
    लागि (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        अचर SharedObject *sharedObject =
                (अचर SharedObject *) element->value.pointer;
        अचर Cache_साँचो_Base *साँचो =
                (अचर Cache_साँचो_Base *) element->साँचो.pointer;
        यदि (sharedObject->hasHardReferences()) {
            ++cnt;
            ख_लिखो(
                    मानक_त्रुटि,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    साँचो->writeDescription(buffer, 256),
                    साँचो->creationStatus,
                    sharedObject == f_No_मान ? nullptr :sharedObject,
                    sharedObject->get_Ref_गन्ती(),
                    sharedObject->get_Soft_Ref_गन्ती());
        }
    }
    ख_लिखो(मानक_त्रुटि, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_गन्ती(fHashtable));
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
    delete f_No_मान;
    f_No_मान = nullptr;
}

अचर UHashElement *
UnifiedCache::_nextElement() अचर {
    अचर UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    यदि (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        फर्काऊ uhash_nextElement(fHashtable, &fEvictPos);
    }
    फर्काऊ element;
}

UBool UnifiedCache::_flush(UBool all) अचर {
    UBool result = असत्य;
    int32_t origSize = uhash_गन्ती(fHashtable);
    लागि (int32_t i = 0; i < origSize; ++i) {
        अचर UHashElement *element = _nextElement();
        यदि (element == nullptr) {
            रोक;
        }
        यदि (all || _isEvictable(element)) {
            अचर SharedObject *sharedObject =
                    static_cast<अचर SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == एतत्);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = सत्य;
        }
    }
    फर्काऊ result;
}

int32_t UnifiedCache::compute_गन्ती_Of_वस्तुहरू_To_Evict() अचर {
    int32_t total_वस्तुहरू = uhash_गन्ती(fHashtable);
    int32_t evictable_वस्तुहरू = total_वस्तुहरू - f_अङ्क_मानहरू_In_Use;

    int32_t unusedLimitByPercentage = f_अङ्क_मानहरू_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t गन्ती_Of_वस्तुहरू_To_Evict = std::max<int32_t>(0, evictable_वस्तुहरू - unusedLimit);
    फर्काऊ गन्ती_Of_वस्तुहरू_To_Evict;
}

शून्य UnifiedCache::_runEvictionSlice() अचर {
    int32_t max_वस्तुहरू_To_Evict = compute_गन्ती_Of_वस्तुहरू_To_Evict();
    यदि (max_वस्तुहरू_To_Evict <= 0) {
        फर्काऊ;
    }
    लागि (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        अचर UHashElement *element = _nextElement();
        यदि (element == nullptr) {
            रोक;
        }
        यदि (_isEvictable(element)) {
            अचर SharedObject *sharedObject =
                    static_cast<अचर SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_गन्ती;
            यदि (--max_वस्तुहरू_To_Evict == 0) {
                रोक;
            }
        }
    }
}

शून्य UnifiedCache::_putNew(
        अचर Cache_साँचो_Base &साँचो,
        अचर SharedObject *value,
        अचर UErrorCode creationStatus,
        UErrorCode &status) अचर {
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    Cache_साँचो_Base *साँचो_To_Adopt = साँचो.clone();
    यदि (साँचो_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
    }
    साँचो_To_Adopt->fCreationStatus = creationStatus;
    यदि (value->soft_Ref_गन्ती == 0) {
        _registerPrimary(साँचो_To_Adopt, value);
    }
    शून्य *old_मान = uhash_put(fHashtable, साँचो_To_Adopt, (शून्य *) value, &status);
    U_ASSERT(old_मान == nullptr);
    (शून्य)old_मान;
    यदि (U_SUCCESS(status)) {
        value->soft_Ref_गन्ती++;
    }
}

शून्य UnifiedCache::_putIfAbsentAndGet(
        अचर Cache_साँचो_Base &साँचो,
        अचर SharedObject *&value,
        UErrorCode &status) अचर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    अचर UHashElement *element = uhash_find(fHashtable, &साँचो);
    यदि (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        फर्काऊ;
    }
    यदि (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(साँचो, value, status, putError);
    } अन्यथा {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        अचर Cache_साँचो_Base &साँचो,
        अचर SharedObject *&value,
        UErrorCode &status) अचर {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    अचर UHashElement *element = uhash_find(fHashtable, &साँचो);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     जबसम्म (element != nullptr && _inProgress(element)) {
         g_In_Progress_मान_Added_Cond->रुको(lock);
         element = uhash_find(fHashtable, &साँचो);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    यदि (element != nullptr) {
         _fetch(element, value, status);
        फर्काऊ सत्य;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(साँचो, f_No_मान, U_ZERO_ERROR, status);
    फर्काऊ असत्य;
}

शून्य UnifiedCache::_get(
        अचर Cache_साँचो_Base &साँचो,
        अचर SharedObject *&value,
        अचर शून्य *creationContext,
        UErrorCode &status) अचर {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    यदि (_poll(साँचो, value, status)) {
        यदि (value == f_No_मान) {
            SharedObject::clearPtr(value);
        }
        फर्काऊ;
    }
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    value = साँचो.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    यदि (value == nullptr) {
        SharedObject::copyPtr(f_No_मान, value);
    }
    _putIfAbsentAndGet(साँचो, value, status);
    यदि (value == f_No_मान) {
        SharedObject::clearPtr(value);
    }
}

शून्य UnifiedCache::_registerPrimary(
            अचर Cache_साँचो_Base *the_साँचो, अचर SharedObject *value) अचर {
    the_साँचो->fIsPrimary = सत्य;
    value->cachePtr = एतत्;
    ++f_अङ्क_मानहरू_Total;
    ++f_अङ्क_मानहरू_In_Use;
}

शून्य UnifiedCache::_put(
        अचर UHashElement *element,
        अचर SharedObject *value,
        अचर UErrorCode status) अचर {
    U_ASSERT(_inProgress(element));
    अचर Cache_साँचो_Base* the_साँचो = static_cast<अचर Cache_साँचो_Base*>(element->साँचो.pointer);
    अचर SharedObject* old_मान = static_cast<अचर SharedObject*>(element->value.pointer);
    the_साँचो->fCreationStatus = status;
    यदि (value->soft_Ref_गन्ती == 0) {
        _registerPrimary(the_साँचो, value);
    }
    value->soft_Ref_गन्ती++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (शून्य *) value;
    U_ASSERT(old_मान == f_No_मान);
    removeSoftRef(old_मान);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_मान_Added_Cond->notify_all();
}

शून्य UnifiedCache::_fetch(
        अचर UHashElement *element,
        अचर SharedObject *&value,
        UErrorCode &status) अचर {
    अचर Cache_साँचो_Base* the_साँचो = static_cast<अचर Cache_साँचो_Base*>(element->साँचो.pointer);
    status = the_साँचो->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<अचर SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(अचर UHashElement* element) अचर {
    UErrorCode status = U_ZERO_ERROR;
    अचर SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    फर्काऊ result;
}

UBool UnifiedCache::_inProgress(
        अचर SharedObject* the_मान, UErrorCode creationStatus) अचर {
    फर्काऊ (the_मान == f_No_मान && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(अचर UHashElement *element) अचर
{
    अचर Cache_साँचो_Base* the_साँचो = static_cast<अचर Cache_साँचो_Base*>(element->साँचो.pointer);
    अचर SharedObject* the_मान = static_cast<अचर SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    यदि (_inProgress(the_मान, the_साँचो->fCreationStatus)) {
        फर्काऊ असत्य;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    फर्काऊ (!the_साँचो->fIsPrimary || (the_मान->soft_Ref_गन्ती == 1 && the_मान->noHardReferences()));
}

शून्य UnifiedCache::removeSoftRef(अचर SharedObject *value) अचर {
    U_ASSERT(value->cachePtr == एतत्);
    U_ASSERT(value->soft_Ref_गन्ती > 0);
    यदि (--value->soft_Ref_गन्ती == 0) {
        --f_अङ्क_मानहरू_Total;
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

int32_t UnifiedCache::removeHardRef(अचर SharedObject *value) अचर {
    पूर्णाङ्क ref_गन्ती = 0;
    यदि (value) {
        ref_गन्ती = umtx_atomic_dec(&value->hard_Ref_गन्ती);
        U_ASSERT(ref_गन्ती >= 0);
        यदि (ref_गन्ती == 0) {
            --f_अङ्क_मानहरू_In_Use;
        }
    }
    फर्काऊ ref_गन्ती;
}

int32_t UnifiedCache::addHardRef(अचर SharedObject *value) अचर {
    पूर्णाङ्क ref_गन्ती = 0;
    यदि (value) {
        ref_गन्ती = umtx_atomic_inc(&value->hard_Ref_गन्ती);
        U_ASSERT(ref_गन्ती >= 1);
        यदि (ref_गन्ती == 1) {
            f_अङ्क_मानहरू_In_Use++;
        }
    }
    फर्काऊ ref_गन्ती;
}

U_NAMESPACE_END

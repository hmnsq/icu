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

स्थिर स्थिरांक int32_t MAX_EVICT_ITERATIONS = 10;
स्थिर स्थिरांक int32_t DEFAULT_MAX_UNUSED = 1000;
स्थिर स्थिरांक int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
स्थिर UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    delete gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_मूल्य_Added_Cond->~condition_variable();
    g_In_Progress_मूल्य_Added_Cond = nullptr;
    परत सत्य;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_किल्ल्या(स्थिरांक UHashTok किल्ली) {
    स्थिरांक Cache_किल्ली_Base* ckey = static_cast<स्थिरांक Cache_किल्ली_Base*>(किल्ली.pointer);
    परत ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_किल्ल्या(स्थिरांक UHashTok key1, स्थिरांक UHashTok key2) {
    स्थिरांक Cache_किल्ली_Base* p1 = static_cast<स्थिरांक Cache_किल्ली_Base*>(key1.pointer);
    स्थिरांक Cache_किल्ली_Base* p2 = static_cast<स्थिरांक Cache_किल्ली_Base*>(key2.pointer);
    परत *p1 == *p2;
}

शून्य U_EXPORT2
ucache_delete_किल्ली(शून्य *obj) {
    Cache_किल्ली_Base* p = static_cast<Cache_किल्ली_Base*>(obj);
    delete p;
}

Cache_किल्ली_Base::~Cache_किल्ली_Base() {
}

स्थिर शून्य U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_मूल्य_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = नवीन UnifiedCache(status);
    जर (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    जर (U_FAILURE(status)) {
        delete gCache;
        gCache = nullptr;
        परत;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    जर (U_FAILURE(status)) {
        परत nullptr;
    }
    U_ASSERT(gCache != nullptr);
    परत gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_अंक_मूल्ये_Total(0),
        f_अंक_मूल्ये_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_मोजणी(0),
        f_No_मूल्य(nullptr) {
    जर (U_FAILURE(status)) {
        परत;
    }
    f_No_मूल्य = नवीन SharedObject();
    जर (f_No_मूल्य == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    f_No_मूल्य->soft_Ref_मोजणी = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_मूल्य->hard_Ref_मोजणी = 1;  // when other references to it are removed.
    f_No_मूल्य->cachePtr = हे;

    fHashtable = uhash_open(
            &ucache_hash_किल्ल्या,
            &ucache_compare_किल्ल्या,
            nullptr,
            &status);
    जर (U_FAILURE(status)) {
        परत;
    }
    uhash_set_किल्ली_Deleter(fHashtable, &ucache_delete_किल्ली);
}

शून्य UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_वस्तुसंच, UErrorCode &status) {
    जर (U_FAILURE(status)) {
        परत;
    }
    जर (count < 0 || percentage_Of_In_Use_वस्तुसंच < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_वस्तुसंच;
}

int32_t UnifiedCache::unused_मोजणी() स्थिरांक {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    परत uhash_मोजणी(fHashtable) - f_अंक_मूल्ये_In_Use;
}

int64_t UnifiedCache::auto_Evicted_मोजणी() स्थिरांक {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    परत f_Auto_Evicted_मोजणी;
}

int32_t UnifiedCache::किल्ली_मोजणी() स्थिरांक {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    परत uhash_मोजणी(fHashtable);
}

शून्य UnifiedCache::flush() स्थिरांक {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    पर्यंत (_flush(असत्य));
}

शून्य UnifiedCache::handleUnreferencedObject() स्थिरांक {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_अंक_मूल्ये_In_Use;
    _runEvictionSlice();
}

#ifdef UNIFIED_CACHE_DEBUG
#अन्तर्भाव <stdio.h>

शून्य UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    स्थिरांक UnifiedCache *cache = getInstance(status);
    जर (U_FAILURE(status)) {
        ख_लिखो(मानक_त्रुटि, "Unified Cache: Error fetching cache.\n");
        परत;
    }
    cache->dumpContents();
}

शून्य UnifiedCache::dumpContents() स्थिरांक {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
शून्य UnifiedCache::_dumpContents() स्थिरांक {
    int32_t pos = UHASH_FIRST;
    स्थिरांक UHashElement *element = uhash_nextElement(fHashtable, &pos);
    अक्षर buffer[256];
    int32_t cnt = 0;
    साठी (; element != nullptr; element = uhash_nextElement(fHashtable, &pos)) {
        स्थिरांक SharedObject *sharedObject =
                (स्थिरांक SharedObject *) element->value.pointer;
        स्थिरांक Cache_किल्ली_Base *किल्ली =
                (स्थिरांक Cache_किल्ली_Base *) element->किल्ली.pointer;
        जर (sharedObject->hasHardReferences()) {
            ++cnt;
            ख_लिखो(
                    मानक_त्रुटि,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    किल्ली->writeDescription(buffer, 256),
                    किल्ली->creationStatus,
                    sharedObject == f_No_मूल्य ? nullptr :sharedObject,
                    sharedObject->get_Ref_मोजणी(),
                    sharedObject->get_Soft_Ref_मोजणी());
        }
    }
    ख_लिखो(मानक_त्रुटि, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_मोजणी(fHashtable));
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

स्थिरांक UHashElement *
UnifiedCache::_nextElement() स्थिरांक {
    स्थिरांक UHashElement *element = uhash_nextElement(fHashtable, &fEvictPos);
    जर (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        परत uhash_nextElement(fHashtable, &fEvictPos);
    }
    परत element;
}

UBool UnifiedCache::_flush(UBool all) स्थिरांक {
    UBool result = असत्य;
    int32_t origSize = uhash_मोजणी(fHashtable);
    साठी (int32_t i = 0; i < origSize; ++i) {
        स्थिरांक UHashElement *element = _nextElement();
        जर (element == nullptr) {
            थांबा;
        }
        जर (all || _isEvictable(element)) {
            स्थिरांक SharedObject *sharedObject =
                    static_cast<स्थिरांक SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == हे);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            result = सत्य;
        }
    }
    परत result;
}

int32_t UnifiedCache::compute_मोजणी_Of_वस्तुसंच_To_Evict() स्थिरांक {
    int32_t total_वस्तुसंच = uhash_मोजणी(fHashtable);
    int32_t evictable_वस्तुसंच = total_वस्तुसंच - f_अंक_मूल्ये_In_Use;

    int32_t unusedLimitByPercentage = f_अंक_मूल्ये_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t मोजणी_Of_वस्तुसंच_To_Evict = std::max<int32_t>(0, evictable_वस्तुसंच - unusedLimit);
    परत मोजणी_Of_वस्तुसंच_To_Evict;
}

शून्य UnifiedCache::_runEvictionSlice() स्थिरांक {
    int32_t max_वस्तुसंच_To_Evict = compute_मोजणी_Of_वस्तुसंच_To_Evict();
    जर (max_वस्तुसंच_To_Evict <= 0) {
        परत;
    }
    साठी (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        स्थिरांक UHashElement *element = _nextElement();
        जर (element == nullptr) {
            थांबा;
        }
        जर (_isEvictable(element)) {
            स्थिरांक SharedObject *sharedObject =
                    static_cast<स्थिरांक SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_मोजणी;
            जर (--max_वस्तुसंच_To_Evict == 0) {
                थांबा;
            }
        }
    }
}

शून्य UnifiedCache::_putNew(
        स्थिरांक Cache_किल्ली_Base &किल्ली,
        स्थिरांक SharedObject *value,
        स्थिरांक UErrorCode creationStatus,
        UErrorCode &status) स्थिरांक {
    जर (U_FAILURE(status)) {
        परत;
    }
    Cache_किल्ली_Base *किल्ली_To_Adopt = किल्ली.clone();
    जर (किल्ली_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    किल्ली_To_Adopt->fCreationStatus = creationStatus;
    जर (value->soft_Ref_मोजणी == 0) {
        _registerPrimary(किल्ली_To_Adopt, value);
    }
    शून्य *old_मूल्य = uhash_put(fHashtable, किल्ली_To_Adopt, (शून्य *) value, &status);
    U_ASSERT(old_मूल्य == nullptr);
    (शून्य)old_मूल्य;
    जर (U_SUCCESS(status)) {
        value->soft_Ref_मोजणी++;
    }
}

शून्य UnifiedCache::_putIfAbsentAndGet(
        स्थिरांक Cache_किल्ली_Base &किल्ली,
        स्थिरांक SharedObject *&value,
        UErrorCode &status) स्थिरांक {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    स्थिरांक UHashElement *element = uhash_find(fHashtable, &किल्ली);
    जर (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        परत;
    }
    जर (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(किल्ली, value, status, putError);
    } अन्यथा {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        स्थिरांक Cache_किल्ली_Base &किल्ली,
        स्थिरांक SharedObject *&value,
        UErrorCode &status) स्थिरांक {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    स्थिरांक UHashElement *element = uhash_find(fHashtable, &किल्ली);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     पर्यंत (element != nullptr && _inProgress(element)) {
         g_In_Progress_मूल्य_Added_Cond->रुको(lock);
         element = uhash_find(fHashtable, &किल्ली);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    जर (element != nullptr) {
         _fetch(element, value, status);
        परत सत्य;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(किल्ली, f_No_मूल्य, U_ZERO_ERROR, status);
    परत असत्य;
}

शून्य UnifiedCache::_get(
        स्थिरांक Cache_किल्ली_Base &किल्ली,
        स्थिरांक SharedObject *&value,
        स्थिरांक शून्य *creationContext,
        UErrorCode &status) स्थिरांक {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    जर (_poll(किल्ली, value, status)) {
        जर (value == f_No_मूल्य) {
            SharedObject::clearPtr(value);
        }
        परत;
    }
    जर (U_FAILURE(status)) {
        परत;
    }
    value = किल्ली.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    जर (value == nullptr) {
        SharedObject::copyPtr(f_No_मूल्य, value);
    }
    _putIfAbsentAndGet(किल्ली, value, status);
    जर (value == f_No_मूल्य) {
        SharedObject::clearPtr(value);
    }
}

शून्य UnifiedCache::_registerPrimary(
            स्थिरांक Cache_किल्ली_Base *the_किल्ली, स्थिरांक SharedObject *value) स्थिरांक {
    the_किल्ली->fIsPrimary = सत्य;
    value->cachePtr = हे;
    ++f_अंक_मूल्ये_Total;
    ++f_अंक_मूल्ये_In_Use;
}

शून्य UnifiedCache::_put(
        स्थिरांक UHashElement *element,
        स्थिरांक SharedObject *value,
        स्थिरांक UErrorCode status) स्थिरांक {
    U_ASSERT(_inProgress(element));
    स्थिरांक Cache_किल्ली_Base* the_किल्ली = static_cast<स्थिरांक Cache_किल्ली_Base*>(element->किल्ली.pointer);
    स्थिरांक SharedObject* old_मूल्य = static_cast<स्थिरांक SharedObject*>(element->value.pointer);
    the_किल्ली->fCreationStatus = status;
    जर (value->soft_Ref_मोजणी == 0) {
        _registerPrimary(the_किल्ली, value);
    }
    value->soft_Ref_मोजणी++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (शून्य *) value;
    U_ASSERT(old_मूल्य == f_No_मूल्य);
    removeSoftRef(old_मूल्य);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_मूल्य_Added_Cond->notify_all();
}

शून्य UnifiedCache::_fetch(
        स्थिरांक UHashElement *element,
        स्थिरांक SharedObject *&value,
        UErrorCode &status) स्थिरांक {
    स्थिरांक Cache_किल्ली_Base* the_किल्ली = static_cast<स्थिरांक Cache_किल्ली_Base*>(element->किल्ली.pointer);
    status = the_किल्ली->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<स्थिरांक SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(स्थिरांक UHashElement* element) स्थिरांक {
    UErrorCode status = U_ZERO_ERROR;
    स्थिरांक SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool result = _inProgress(value, status);
    removeHardRef(value);
    परत result;
}

UBool UnifiedCache::_inProgress(
        स्थिरांक SharedObject* the_मूल्य, UErrorCode creationStatus) स्थिरांक {
    परत (the_मूल्य == f_No_मूल्य && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(स्थिरांक UHashElement *element) स्थिरांक
{
    स्थिरांक Cache_किल्ली_Base* the_किल्ली = static_cast<स्थिरांक Cache_किल्ली_Base*>(element->किल्ली.pointer);
    स्थिरांक SharedObject* the_मूल्य = static_cast<स्थिरांक SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    जर (_inProgress(the_मूल्य, the_किल्ली->fCreationStatus)) {
        परत असत्य;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    परत (!the_किल्ली->fIsPrimary || (the_मूल्य->soft_Ref_मोजणी == 1 && the_मूल्य->noHardReferences()));
}

शून्य UnifiedCache::removeSoftRef(स्थिरांक SharedObject *value) स्थिरांक {
    U_ASSERT(value->cachePtr == हे);
    U_ASSERT(value->soft_Ref_मोजणी > 0);
    जर (--value->soft_Ref_मोजणी == 0) {
        --f_अंक_मूल्ये_Total;
        जर (value->noHardReferences()) {
            delete value;
        } अन्यथा {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(स्थिरांक SharedObject *value) स्थिरांक {
    पूर्णांक ref_मोजणी = 0;
    जर (value) {
        ref_मोजणी = umtx_atomic_dec(&value->hard_Ref_मोजणी);
        U_ASSERT(ref_मोजणी >= 0);
        जर (ref_मोजणी == 0) {
            --f_अंक_मूल्ये_In_Use;
        }
    }
    परत ref_मोजणी;
}

int32_t UnifiedCache::addHardRef(स्थिरांक SharedObject *value) स्थिरांक {
    पूर्णांक ref_मोजणी = 0;
    जर (value) {
        ref_मोजणी = umtx_atomic_inc(&value->hard_Ref_मोजणी);
        U_ASSERT(ref_मोजणी >= 1);
        जर (ref_मोजणी == 1) {
            f_अंक_मूल्ये_In_Use++;
        }
    }
    परत ref_मोजणी;
}

U_NAMESPACE_END

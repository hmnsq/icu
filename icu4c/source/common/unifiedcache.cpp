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

#समावेश "unifiedcache.h"

#समावेश <algorithm>      // For std::max()
#समावेश <mutex>

#समावेश "uassert.h"
#समावेश "uhash.h"
#समावेश "ucln_cmn.h"

अटल icu::UnifiedCache *gCache = nullptr;
अटल std::mutex *gCacheMutex = nullptr;
अटल std::condition_variable *g_In_Progress_मूल्य_Added_Cond;
अटल icu::UInitOnce gCacheInitOnce {};

अटल स्थिर int32_t MAX_EVICT_ITERATIONS = 10;
अटल स्थिर int32_t DEFAULT_MAX_UNUSED = 1000;
अटल स्थिर int32_t DEFAULT_PERCENTAGE_OF_IN_USE = 100;


U_CDECL_BEGIN
अटल UBool U_CALLCONV unifiedcache_cleanup() {
    gCacheInitOnce.reset();
    मिटाओ gCache;
    gCache = nullptr;
    gCacheMutex->~mutex();
    gCacheMutex = nullptr;
    g_In_Progress_मूल्य_Added_Cond->~condition_variable();
    g_In_Progress_मूल्य_Added_Cond = nullptr;
    वापस true;
}
U_CDECL_END


U_NAMESPACE_BEGIN

int32_t U_EXPORT2
ucache_hash_कुंजियाँ(स्थिर UHashTok कुंजी) {
    स्थिर Cache_कुंजी_Base* ckey = static_cast<स्थिर Cache_कुंजी_Base*>(कुंजी.pointer);
    वापस ckey->hashCode();
}

UBool U_EXPORT2
ucache_compare_कुंजियाँ(स्थिर UHashTok key1, स्थिर UHashTok key2) {
    स्थिर Cache_कुंजी_Base* p1 = static_cast<स्थिर Cache_कुंजी_Base*>(key1.pointer);
    स्थिर Cache_कुंजी_Base* p2 = static_cast<स्थिर Cache_कुंजी_Base*>(key2.pointer);
    वापस *p1 == *p2;
}

व्योम U_EXPORT2
ucache_delete_कुंजी(व्योम *obj) {
    Cache_कुंजी_Base* p = static_cast<Cache_कुंजी_Base*>(obj);
    मिटाओ p;
}

Cache_कुंजी_Base::~Cache_कुंजी_Base() {
}

अटल व्योम U_CALLCONV cacheInit(UErrorCode &status) {
    U_ASSERT(gCache == nullptr);
    ucln_common_registerCleanup(
            UCLN_COMMON_UNIFIED_CACHE, unifiedcache_cleanup);

    gCacheMutex = STATIC_NEW(std::mutex);
    g_In_Progress_मूल्य_Added_Cond = STATIC_NEW(std::condition_variable);
    gCache = नया UnifiedCache(status);
    अगर (gCache == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
    }
    अगर (U_FAILURE(status)) {
        मिटाओ gCache;
        gCache = nullptr;
        वापस;
    }
}

UnifiedCache *UnifiedCache::getInstance(UErrorCode &status) {
    umtx_initOnce(gCacheInitOnce, &cacheInit, status);
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    U_ASSERT(gCache != nullptr);
    वापस gCache;
}

UnifiedCache::UnifiedCache(UErrorCode &status) :
        fHashtable(nullptr),
        fEvictPos(UHASH_FIRST),
        f_अंक_मूल्यावली_योग(0),
        f_अंक_मूल्यावली_In_Use(0),
        fMaxUnused(DEFAULT_MAX_UNUSED),
        fMaxPercentageOfInUse(DEFAULT_PERCENTAGE_OF_IN_USE),
        f_Auto_Evicted_गिनती(0),
        f_No_मूल्य(nullptr) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    f_No_मूल्य = नया SharedObject();
    अगर (f_No_मूल्य == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    f_No_मूल्य->soft_Ref_गिनती = 1;  // Add fake references to prevent fNoValue from being deleted
    f_No_मूल्य->hard_Ref_गिनती = 1;  // when other references to it are removed.
    f_No_मूल्य->cachePtr = यह;

    fHashtable = uhash_open(
            &ucache_hash_कुंजियाँ,
            &ucache_compare_कुंजियाँ,
            nullptr,
            &status);
    अगर (U_FAILURE(status)) {
        वापस;
    }
    uhash_set_कुंजी_Deleter(fHashtable, &ucache_delete_कुंजी);
}

व्योम UnifiedCache::setEvictionPolicy(
        int32_t count, int32_t percentage_Of_In_Use_वस्तुएँ, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    अगर (count < 0 || percentage_Of_In_Use_वस्तुएँ < 0) {
        status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    fMaxUnused = count;
    fMaxPercentageOfInUse = percentage_Of_In_Use_वस्तुएँ;
}

int32_t UnifiedCache::unused_गिनती() स्थिर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    वापस uhash_गिनती(fHashtable) - f_अंक_मूल्यावली_In_Use;
}

int64_t UnifiedCache::auto_Evicted_गिनती() स्थिर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    वापस f_Auto_Evicted_गिनती;
}

int32_t UnifiedCache::कुंजी_गिनती() स्थिर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    वापस uhash_गिनती(fHashtable);
}

व्योम UnifiedCache::सफाई() स्थिर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);

    // Use a loop in case cache items that are flushed held hard references to
    // other cache items making those additional cache items eligible for
    // flushing.
    जबतक (_flush(false));
}

व्योम UnifiedCache::handleUnreferencedObject() स्थिर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    --f_अंक_मूल्यावली_In_Use;
    _runEvictionSlice();
}

#अगर_घोषित UNIFIED_CACHE_DEBUG
#समावेश <stdio.h>

व्योम UnifiedCache::dump() {
    UErrorCode status = U_ZERO_ERROR;
    स्थिर UnifiedCache *cache = getInstance(status);
    अगर (U_FAILURE(status)) {
        ख_लिखो(मानक_त्रुटि, "Unified Cache: Error fetching cache.\n");
        वापस;
    }
    cache->dumpContents();
}

व्योम UnifiedCache::dumpContents() स्थिर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    _dumpContents();
}

// Dumps content of cache.
// On entry, gCacheMutex must be held.
// On exit, cache contents dumped to stderr.
व्योम UnifiedCache::_dumpContents() स्थिर {
    int32_t pos = UHASH_FIRST;
    स्थिर UHashElement *element = uhash_अग्र_Element(fHashtable, &pos);
    अक्षर बफर[256];
    int32_t cnt = 0;
    क्रम (; element != nullptr; element = uhash_अग्र_Element(fHashtable, &pos)) {
        स्थिर SharedObject *sharedObject =
                (स्थिर SharedObject *) element->value.pointer;
        स्थिर Cache_कुंजी_Base *कुंजी =
                (स्थिर Cache_कुंजी_Base *) element->कुंजी.pointer;
        अगर (sharedObject->hasHardReferences()) {
            ++cnt;
            ख_लिखो(
                    मानक_त्रुटि,
                    "Unified Cache: Key '%s', error %d, value %p, total refcount %d, soft refcount %d\n",
                    कुंजी->writeDescription(बफर, 256),
                    कुंजी->creationStatus,
                    sharedObject == f_No_मूल्य ? nullptr :sharedObject,
                    sharedObject->get_Ref_गिनती(),
                    sharedObject->get_Soft_Ref_गिनती());
        }
    }
    ख_लिखो(मानक_त्रुटि, "Unified Cache: %d out of a total of %d still have hard references\n", cnt, uhash_गिनती(fHashtable));
}
#पूर्ण_अगर

UnifiedCache::~UnifiedCache() {
    // Try our best to clean up first.
    सफाई();
    {
        // Now all that should be left in the cache are entries that refer to
        // each other and entries with hard references from outside the cache.
        // Nothing we can do about these so proceed to wipe out the cache.
        std::lock_guard<std::mutex> lock(*gCacheMutex);
        _flush(true);
    }
    uhash_close(fHashtable);
    fHashtable = nullptr;
    मिटाओ f_No_मूल्य;
    f_No_मूल्य = nullptr;
}

स्थिर UHashElement *
UnifiedCache::अग्र_Element_2() स्थिर {
    स्थिर UHashElement *element = uhash_अग्र_Element(fHashtable, &fEvictPos);
    अगर (element == nullptr) {
        fEvictPos = UHASH_FIRST;
        वापस uhash_अग्र_Element(fHashtable, &fEvictPos);
    }
    वापस element;
}

UBool UnifiedCache::_flush(UBool all) स्थिर {
    UBool परिणाम = false;
    int32_t orig_आकार = uhash_गिनती(fHashtable);
    क्रम (int32_t i = 0; i < orig_आकार; ++i) {
        स्थिर UHashElement *element = अग्र_Element_2();
        अगर (element == nullptr) {
            अवरोध;
        }
        अगर (all || _isEvictable(element)) {
            स्थिर SharedObject *sharedObject =
                    static_cast<स्थिर SharedObject*>(element->value.pointer);
            U_ASSERT(sharedObject->cachePtr == यह);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);    // Deletes the sharedObject when softRefCount goes to zero.
            परिणाम = true;
        }
    }
    वापस परिणाम;
}

int32_t UnifiedCache::compute_गिनती_Of_वस्तुएँ_To_Evict() स्थिर {
    int32_t योग_वस्तुएँ = uhash_गिनती(fHashtable);
    int32_t evictable_वस्तुएँ = योग_वस्तुएँ - f_अंक_मूल्यावली_In_Use;

    int32_t unusedLimitByPercentage = f_अंक_मूल्यावली_In_Use * fMaxPercentageOfInUse / 100;
    int32_t unusedLimit = std::max(unusedLimitByPercentage, fMaxUnused);
    int32_t गिनती_Of_वस्तुएँ_To_Evict = std::max<int32_t>(0, evictable_वस्तुएँ - unusedLimit);
    वापस गिनती_Of_वस्तुएँ_To_Evict;
}

व्योम UnifiedCache::_runEvictionSlice() स्थिर {
    int32_t max_वस्तुएँ_To_Evict = compute_गिनती_Of_वस्तुएँ_To_Evict();
    अगर (max_वस्तुएँ_To_Evict <= 0) {
        वापस;
    }
    क्रम (int32_t i = 0; i < MAX_EVICT_ITERATIONS; ++i) {
        स्थिर UHashElement *element = अग्र_Element_2();
        अगर (element == nullptr) {
            अवरोध;
        }
        अगर (_isEvictable(element)) {
            स्थिर SharedObject *sharedObject =
                    static_cast<स्थिर SharedObject*>(element->value.pointer);
            uhash_removeElement(fHashtable, element);
            removeSoftRef(sharedObject);   // Deletes sharedObject when SoftRefCount goes to zero.
            ++f_Auto_Evicted_गिनती;
            अगर (--max_वस्तुएँ_To_Evict == 0) {
                अवरोध;
            }
        }
    }
}

व्योम UnifiedCache::_putNew(
        स्थिर Cache_कुंजी_Base &कुंजी,
        स्थिर SharedObject *value,
        स्थिर UErrorCode creationStatus,
        UErrorCode &status) स्थिर {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    Cache_कुंजी_Base *कुंजी_To_Adopt = कुंजी.clone();
    अगर (कुंजी_To_Adopt == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    कुंजी_To_Adopt->fCreationStatus = creationStatus;
    अगर (value->soft_Ref_गिनती == 0) {
        _registerPrimary(कुंजी_To_Adopt, value);
    }
    व्योम *old_मूल्य = uhash_put(fHashtable, कुंजी_To_Adopt, (व्योम *) value, &status);
    U_ASSERT(old_मूल्य == nullptr);
    (व्योम)old_मूल्य;
    अगर (U_SUCCESS(status)) {
        value->soft_Ref_गिनती++;
    }
}

व्योम UnifiedCache::_putIfAbsentAndGet(
        स्थिर Cache_कुंजी_Base &कुंजी,
        स्थिर SharedObject *&value,
        UErrorCode &status) स्थिर {
    std::lock_guard<std::mutex> lock(*gCacheMutex);
    स्थिर UHashElement *element = uhash_find(fHashtable, &कुंजी);
    अगर (element != nullptr && !_inProgress(element)) {
        _fetch(element, value, status);
        वापस;
    }
    अगर (element == nullptr) {
        UErrorCode putError = U_ZERO_ERROR;
        // best-effort basis only.
        _putNew(कुंजी, value, status, putError);
    } अन्यथा {
        _put(element, value, status);
    }
    // Run an eviction slice. This will run even if we added a primary entry
    // which doesn't increase the unused count, but that is still o.k
    _runEvictionSlice();
}


UBool UnifiedCache::_poll(
        स्थिर Cache_कुंजी_Base &कुंजी,
        स्थिर SharedObject *&value,
        UErrorCode &status) स्थिर {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    std::unique_lock<std::mutex> lock(*gCacheMutex);
    स्थिर UHashElement *element = uhash_find(fHashtable, &कुंजी);

    // If the hash table contains an inProgress placeholder entry for this key,
    // this means that another thread is currently constructing the value object.
    // Loop, waiting for that construction to complete.
     जबतक (element != nullptr && _inProgress(element)) {
         g_In_Progress_मूल्य_Added_Cond->रुको(lock);
         element = uhash_find(fHashtable, &कुंजी);
    }

    // If the hash table contains an entry for the key,
    // fetch out the contents and return them.
    अगर (element != nullptr) {
         _fetch(element, value, status);
        वापस true;
    }

    // The hash table contained nothing for this key.
    // Insert an inProgress place holder value.
    // Our caller will create the final value and update the hash table.
    _putNew(कुंजी, f_No_मूल्य, U_ZERO_ERROR, status);
    वापस false;
}

व्योम UnifiedCache::_get(
        स्थिर Cache_कुंजी_Base &कुंजी,
        स्थिर SharedObject *&value,
        स्थिर व्योम *creationContext,
        UErrorCode &status) स्थिर {
    U_ASSERT(value == nullptr);
    U_ASSERT(status == U_ZERO_ERROR);
    अगर (_poll(कुंजी, value, status)) {
        अगर (value == f_No_मूल्य) {
            SharedObject::clearPtr(value);
        }
        वापस;
    }
    अगर (U_FAILURE(status)) {
        वापस;
    }
    value = कुंजी.createObject(creationContext, status);
    U_ASSERT(value == nullptr || value->hasHardReferences());
    U_ASSERT(value != nullptr || status != U_ZERO_ERROR);
    अगर (value == nullptr) {
        SharedObject::copyPtr(f_No_मूल्य, value);
    }
    _putIfAbsentAndGet(कुंजी, value, status);
    अगर (value == f_No_मूल्य) {
        SharedObject::clearPtr(value);
    }
}

व्योम UnifiedCache::_registerPrimary(
            स्थिर Cache_कुंजी_Base *the_कुंजी, स्थिर SharedObject *value) स्थिर {
    the_कुंजी->fIsPrimary = true;
    value->cachePtr = यह;
    ++f_अंक_मूल्यावली_योग;
    ++f_अंक_मूल्यावली_In_Use;
}

व्योम UnifiedCache::_put(
        स्थिर UHashElement *element,
        स्थिर SharedObject *value,
        स्थिर UErrorCode status) स्थिर {
    U_ASSERT(_inProgress(element));
    स्थिर Cache_कुंजी_Base* the_कुंजी = static_cast<स्थिर Cache_कुंजी_Base*>(element->कुंजी.pointer);
    स्थिर SharedObject* old_मूल्य = static_cast<स्थिर SharedObject*>(element->value.pointer);
    the_कुंजी->fCreationStatus = status;
    अगर (value->soft_Ref_गिनती == 0) {
        _registerPrimary(the_कुंजी, value);
    }
    value->soft_Ref_गिनती++;
    UHashElement *ptr = const_cast<UHashElement *>(element);
    ptr->value.pointer = (व्योम *) value;
    U_ASSERT(old_मूल्य == f_No_मूल्य);
    removeSoftRef(old_मूल्य);

    // Tell waiting threads that we replace in-progress status with
    // an error.
    g_In_Progress_मूल्य_Added_Cond->notify_all();
}

व्योम UnifiedCache::_fetch(
        स्थिर UHashElement *element,
        स्थिर SharedObject *&value,
        UErrorCode &status) स्थिर {
    स्थिर Cache_कुंजी_Base* the_कुंजी = static_cast<स्थिर Cache_कुंजी_Base*>(element->कुंजी.pointer);
    status = the_कुंजी->fCreationStatus;

    // Since we have the cache lock, calling regular SharedObject add/removeRef
    // could cause us to deadlock on ourselves since they may need to lock
    // the cache mutex.
    removeHardRef(value);
    value = static_cast<स्थिर SharedObject *>(element->value.pointer);
    addHardRef(value);
}


UBool UnifiedCache::_inProgress(स्थिर UHashElement* element) स्थिर {
    UErrorCode status = U_ZERO_ERROR;
    स्थिर SharedObject * value = nullptr;
    _fetch(element, value, status);
    UBool परिणाम = _inProgress(value, status);
    removeHardRef(value);
    वापस परिणाम;
}

UBool UnifiedCache::_inProgress(
        स्थिर SharedObject* the_मूल्य, UErrorCode creationStatus) स्थिर {
    वापस (the_मूल्य == f_No_मूल्य && creationStatus == U_ZERO_ERROR);
}

UBool UnifiedCache::_isEvictable(स्थिर UHashElement *element) स्थिर
{
    स्थिर Cache_कुंजी_Base* the_कुंजी = static_cast<स्थिर Cache_कुंजी_Base*>(element->कुंजी.pointer);
    स्थिर SharedObject* the_मूल्य = static_cast<स्थिर SharedObject*>(element->value.pointer);

    // Entries that are under construction are never evictable
    अगर (_inProgress(the_मूल्य, the_कुंजी->fCreationStatus)) {
        वापस false;
    }

    // We can evict entries that are either not a primary or have just
    // one reference (The one reference being from the cache itself).
    वापस (!the_कुंजी->fIsPrimary || (the_मूल्य->soft_Ref_गिनती == 1 && the_मूल्य->noHardReferences()));
}

व्योम UnifiedCache::removeSoftRef(स्थिर SharedObject *value) स्थिर {
    U_ASSERT(value->cachePtr == यह);
    U_ASSERT(value->soft_Ref_गिनती > 0);
    अगर (--value->soft_Ref_गिनती == 0) {
        --f_अंक_मूल्यावली_योग;
        अगर (value->noHardReferences()) {
            मिटाओ value;
        } अन्यथा {
            // This path only happens from flush(all). Which only happens from the
            // UnifiedCache destructor.  Nulling out value.cacheptr changes the behavior
            // of value.removeRef(), causing the deletion to be done there.
            value->cachePtr = nullptr;
        }
    }
}

int32_t UnifiedCache::removeHardRef(स्थिर SharedObject *value) स्थिर {
    पूर्णांक ref_गिनती = 0;
    अगर (value) {
        ref_गिनती = umtx_atomic_dec(&value->hard_Ref_गिनती);
        U_ASSERT(ref_गिनती >= 0);
        अगर (ref_गिनती == 0) {
            --f_अंक_मूल्यावली_In_Use;
        }
    }
    वापस ref_गिनती;
}

int32_t UnifiedCache::addHardRef(स्थिर SharedObject *value) स्थिर {
    पूर्णांक ref_गिनती = 0;
    अगर (value) {
        ref_गिनती = umtx_atomic_inc(&value->hard_Ref_गिनती);
        U_ASSERT(ref_गिनती >= 1);
        अगर (ref_गिनती == 1) {
            f_अंक_मूल्यावली_In_Use++;
        }
    }
    वापस ref_गिनती;
}

U_NAMESPACE_END

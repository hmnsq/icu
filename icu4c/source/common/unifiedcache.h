// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*
* File UNIFIEDCACHE.H - The ICU Unified cache.
******************************************************************************
*/

#ifndef __UNIFIED_CACHE_H__
#define __UNIFIED_CACHE_H__

#ଅନ୍ତର୍ଭାଵ "utypeinfo.h"  // for 'typeid' to work

#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "sharedobject.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

ସଂରଚନା UHashtable;
ସଂରଚନା UHashElement;

U_NAMESPACE_BEGIN

class UnifiedCache;

/**
 * A base class for all cache keys.
 */
class U_COMMON_API Cache_ଚାବି_Base : ସର୍ବସାଧାରଣ UObject {
 ସର୍ବସାଧାରଣ:
   Cache_ଚାବି_Base() : fCreationStatus(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_ଚାବି_Base(ଅଚଳ Cache_ଚାବି_Base &other) 
           : UObject(other), fCreationStatus(other.fCreationStatus), fIsPrimary(false) { }
   ଆଭାସୀ ~Cache_ଚାବି_Base();

   /**
    * Returns the hash code for this object.
    */
   ଆଭାସୀ int32_t hashCode() ଅଚଳ = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   ଆଭାସୀ Cache_ଚାବି_Base *clone() ଅଚଳ = 0;

   /**
    * Create a new object for this key. Called by cache on cache miss.
    * createObject must add a reference to the object it returns. Note
    * that getting an object from the cache and returning it without calling
    * removeRef on it satisfies this requirement. It can also return nullptr
    * and set status to an error.
    *
    * @param creationContext the context in which the object is being
    *                        created. May be nullptr.
    * @param status          Implementations can return a failure here.
    *                        In addition, implementations may return a
    *                        non nullptr object and set a warning status.
    */
   ଆଭାସୀ ଅଚଳ SharedObject *createObject(
           ଅଚଳ ଶୂନ୍ୟ *creationContext, UErrorCode &status) ଅଚଳ = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   ଆଭାସୀ ଅକ୍ଷର *writeDescription(ଅକ୍ଷର *buffer, int32_t bufSize) ଅଚଳ = 0;

   friend ଧାଡ଼ିଭିତରେ bool operator==(ଅଚଳ Cache_ଚାବି_Base& lhs,
                                 ଅଚଳ Cache_ଚାବି_Base& rhs) {
       ଫେରନ୍ତୁ lhs.equals(rhs);
   }

   friend ଧାଡ଼ିଭିତରେ bool operator!=(ଅଚଳ Cache_ଚାବି_Base& lhs,
                                 ଅଚଳ Cache_ଚାବି_Base& rhs) {
       ଫେରନ୍ତୁ !lhs.equals(rhs);
   }

 ସୁରକ୍ଷିତ:
   ଆଭାସୀ bool equals(ଅଚଳ Cache_ଚାବି_Base& other) ଅଚଳ = 0;

 ବ୍ୟକ୍ତିଗତ:
   mutable UErrorCode fCreationStatus;
   mutable UBool fIsPrimary;
   friend class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
template<typename T>
class Cache_ଚାବି : ସର୍ବସାଧାରଣ Cache_ଚାବି_Base {
 ସର୍ବସାଧାରଣ:
   ଆଭାସୀ ~Cache_ଚାବି() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   ଆଭାସୀ int32_t hashCode() ଅଚଳ override {
       ଅଚଳ ଅକ୍ଷର *s = typeid(T).name();
       ଫେରନ୍ତୁ ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   ଆଭାସୀ ଅକ୍ଷର *writeDescription(ଅକ୍ଷର *buffer, int32_t bufLen) ଅଚଳ override {
       ଅଚଳ ଅକ୍ଷର *s = typeid(T).name();
       uprv_strncpy(buffer, s, bufLen);
       buffer[bufLen - 1] = 0;
       ଫେରନ୍ତୁ buffer;
   }

 ସୁରକ୍ଷିତ:
   /**
    * Two objects are equal if they are of the same type.
    */
   ଆଭାସୀ bool equals(ଅଚଳ Cache_ଚାବି_Base &other) ଅଚଳ override {
       ଫେରନ୍ତୁ this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
template<typename T>
class Locale_Cache_ଚାବି : ସର୍ବସାଧାରଣ Cache_ଚାବି<T> {
 ସୁରକ୍ଷିତ:
   Locale   fLoc;
   ଆଭାସୀ bool equals(ଅଚଳ Cache_ଚାବି_Base &other) ଅଚଳ override {
       ଯଦି (!Cache_ଚାବି<T>::equals(other)) {
           ଫେରନ୍ତୁ false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       ଫେରନ୍ତୁ operator==(static_cast<ଅଚଳ Locale_Cache_ଚାବି<T> &>(other));
   }
 ସର୍ବସାଧାରଣ:
   Locale_Cache_ଚାବି(ଅଚଳ Locale &loc) : fLoc(loc) {}
   Locale_Cache_ଚାବି(ଅଚଳ Locale_Cache_ଚାବି<T> &other)
           : Cache_ଚାବି<T>(other), fLoc(other.fLoc) { }
   ଆଭାସୀ ~Locale_Cache_ଚାବି() { }
   ଆଭାସୀ int32_t hashCode() ଅଚଳ override {
       ଫେରନ୍ତୁ (int32_t)(37u * (uint32_t)Cache_ଚାବି<T>::hashCode() + (uint32_t)fLoc.hashCode());
   }
   ଧାଡ଼ିଭିତରେ bool operator == (ଅଚଳ Locale_Cache_ଚାବି<T> &other) ଅଚଳ {
       ଫେରନ୍ତୁ fLoc == other.fLoc;
   }
   ଆଭାସୀ Cache_ଚାବି_Base *clone() ଅଚଳ override {
       ଫେରନ୍ତୁ new Locale_Cache_ଚାବି<T>(*this);
   }
   ଆଭାସୀ ଅଚଳ T *createObject(
           ଅଚଳ ଶୂନ୍ୟ *creationContext, UErrorCode &status) ଅଚଳ override;
   /**
    * Use the locale id as the description.
    */
   ଆଭାସୀ ଅକ୍ଷର *writeDescription(ଅକ୍ଷର *buffer, int32_t bufLen) ଅଚଳ override {
       ଅଚଳ ଅକ୍ଷର *s = fLoc.getName();
       uprv_strncpy(buffer, s, bufLen);
       buffer[bufLen - 1] = 0;
       ଫେରନ୍ତୁ buffer;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : ସର୍ବସାଧାରଣ UnifiedCacheBase {
 ସର୍ବସାଧାରଣ:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UErrorCode &status);

   /**
    * Return a pointer to the global cache instance.
    */
   ସ୍ଥିର UnifiedCache *getInstance(UErrorCode &status);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   template<typename T>
   ଶୂନ୍ୟ get(
           ଅଚଳ Cache_ଚାବି<T>& ଚାବି,
           ଅଚଳ T *&ptr,
           UErrorCode &status) ଅଚଳ {
       get(ଚାବି, nullptr, ptr, status);
   }

   /**
    * Fetches value from the cache by key.
    *
    * @param key             the cache key.
    * @param creationContext passed verbatim to createObject method of key
    * @param ptr             On entry, ptr must be nullptr or be included if
    *                        the reference count of the object it points
    *                        to. On exit, ptr points to the fetched object
    *                        from the cache or is left unchanged on
    *                        failure. Caller must call removeRef on ptr
    *                        if set to a non nullptr value.
    * @param status          Any error returned here. May be set to a
    *                        warning value even if ptr is set.
    */
   template<typename T>
   ଶୂନ୍ୟ get(
           ଅଚଳ Cache_ଚାବି<T>& ଚାବି,
           ଅଚଳ ଶୂନ୍ୟ *creationContext,
           ଅଚଳ T *&ptr,
           UErrorCode &status) ଅଚଳ {
       ଯଦି (U_FAILURE(status)) {
           ଫେରନ୍ତୁ;
       }
       UErrorCode creationStatus = U_ZERO_ERROR;
       ଅଚଳ SharedObject *ମୂଲ୍ୟ_2 = nullptr;
       _get(ଚାବି, ମୂଲ୍ୟ_2, creationContext, creationStatus);
       ଅଚଳ T *tvalue = (ଅଚଳ T *) ମୂଲ୍ୟ_2;
       ଯଦି (U_SUCCESS(creationStatus)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       ଯଦି (status == U_ZERO_ERROR || U_FAILURE(creationStatus)) {
           status = creationStatus;
       }
   }

#ifdef UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   ଶୂନ୍ୟ dumpContents() ଅଚଳ;
#endif

   /**
    * Convenience method to get a value of type T from cache for a
    * particular locale with creationContext == nullptr.
    * @param loc    the locale
    * @param ptr    On entry, must be nullptr or included in the ref count
    *               of the object to which it points.
    *               On exit, fetched value stored here or is left
    *               unchanged on failure. Caller must call removeRef on
    *               ptr if set to a non nullptr value.
    * @param status Any error returned here. May be set to a
    *               warning value even if ptr is set.
    */
   template<typename T>
   ସ୍ଥିର ଶୂନ୍ୟ getByLocale(
           ଅଚଳ Locale &loc, ଅଚଳ T *&ptr, UErrorCode &status) {
       ଅଚଳ UnifiedCache *cache = getInstance(status);
       ଯଦି (U_FAILURE(status)) {
           ଫେରନ୍ତୁ;
       }
       cache->get(Locale_Cache_ଚାବି<T>(loc), ptr, status);
   }

#ifdef UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   ସ୍ଥିର ଶୂନ୍ୟ dump();
#endif

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t ଚାବି_ଗଣନା() ଅଚଳ;

   /**
    * Removes any values from cache that are not referenced outside
    * the cache.
    */
   ଶୂନ୍ୟ flush() ଅଚଳ;

   /**
    * Configures at what point eviction of unused entries will begin.
    * Eviction is triggered whenever the number of evictable keys exceeds
    * BOTH count AND (number of in-use items) * (percentageOfInUseItems / 100).
    * Once the number of unused entries drops below one of these,
    * eviction ceases. Because eviction happens incrementally,
    * the actual unused entry count may exceed both these numbers
    * from time to time.
    *
    * A cache entry is defined as unused if it is not essential to guarantee
    * that for a given key X, the cache returns the same reference to the
    * same value as long as the client already holds a reference to that
    * value.
    *
    * If this method is never called, the default settings are 1000 and 100%.
    *
    * Although this method is thread-safe, it is designed to be called at
    * application startup. If it is called in the middle of execution, it
    * will have no immediate effect on the cache. However over time, the
    * cache will perform eviction slices in an attempt to honor the new
    * settings.
    *
    * If a client already holds references to many different unique values
    * in the cache such that the number of those unique values far exceeds
    * "count" then the cache may not be able to maintain this maximum.
    * However, if this happens, the cache still guarantees that the number of
    * unused entries will remain only a small percentage of the total cache
    * size.
    *
    * If the parameters passed are negative, setEvctionPolicy sets status to
    * U_ILLEGAL_ARGUMENT_ERROR.
    */
   ଶୂନ୍ୟ setEvictionPolicy(
           int32_t ଗଣନା_2, int32_t percentage_Of_In_Use_ବସ୍ତୁଗୁଡ଼ିକ, UErrorCode &status);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_ଗଣନା() ଅଚଳ;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t unused_ଗଣନା() ଅଚଳ;

   ଆଭାସୀ ଶୂନ୍ୟ handleUnreferencedObject() ଅଚଳ override;
   ଆଭାସୀ ~UnifiedCache();
   
 ବ୍ୟକ୍ତିଗତ:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_Total;
   mutable int32_t f_ଅଙ୍କ_ମୂଲ୍ୟଗୁଡ଼ିକ_In_Use;
   int32_t fMaxUnused;
   int32_t fMaxPercentageOfInUse;
   mutable int64_t f_Auto_Evicted_ଗଣନା;
   SharedObject *f_No_ମୂଲ୍ୟ;
   
   UnifiedCache(ଅଚଳ UnifiedCache &other) = delete;
   UnifiedCache &operator=(ଅଚଳ UnifiedCache &other) = delete;
   
   /**
    * Flushes the contents of the cache. If cache values hold references to other
    * cache values then _flush should be called in a loop until it returns false.
    * 
    * On entry, gCacheMutex must be held.
    * On exit, those values with are evictable are flushed.
    * 
    *  @param all if false flush evictable items only, which are those with no external
    *                    references, plus those that can be safely recreated.<br>
    *            if true, flush all elements. Any values (sharedObjects) with remaining
    *                     hard (external) references are not deleted, but are detached from
    *                     the cache, so that a subsequent removeRefs can delete them.
    *                     _flush is not thread safe when all is true.
    *   @return true if any value in cache was flushed or false otherwise.
    */
   UBool _flush(UBool all) ଅଚଳ;
   
   /**
    * Gets value out of cache.
    * On entry. gCacheMutex must not be held. value must be nullptr. status
    * must be U_ZERO_ERROR.
    * On exit. value and status set to what is in cache at key or on cache
    * miss the key's createObject() is called and value and status are set to
    * the result of that. In this latter case, best effort is made to add the
    * value and status to the cache. If createObject() fails to create a value,
    * fNoValue is stored in cache, and value is set to nullptr. Caller must call
    * removeRef on value if non nullptr.
    */
   ଶୂନ୍ୟ _get(
           ଅଚଳ Cache_ଚାବି_Base &ଚାବି,
           ଅଚଳ SharedObject *&ମୂଲ୍ୟ_2,
           ଅଚଳ ଶୂନ୍ୟ *creationContext,
           UErrorCode &status) ଅଚଳ;

    /**
     * Attempts to fetch value and status for key from cache.
     * On entry, gCacheMutex must not be held value must be nullptr and status must
     * be U_ZERO_ERROR.
     * On exit, either returns false (In this
     * case caller should try to create the object) or returns true with value
     * pointing to the fetched value and status set to fetched status. When
     * false is returned status may be set to failure if an in progress hash
     * entry could not be made but value will remain unchanged. When true is
     * returned, caller must call removeRef() on value.
     */
    UBool _poll(
            ଅଚଳ Cache_ଚାବି_Base &ଚାବି,
            ଅଚଳ SharedObject *&ମୂଲ୍ୟ_2,
            UErrorCode &status) ଅଚଳ;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    ଶୂନ୍ୟ _putNew(
        ଅଚଳ Cache_ଚାବି_Base &ଚାବି,
        ଅଚଳ SharedObject *ମୂଲ୍ୟ_2,
        ଅଚଳ UErrorCode creationStatus,
        UErrorCode &status) ଅଚଳ;
           
    /**
     * Places value and status at key if there is no value at key or if cache
     * entry for key is in progress. Otherwise, it leaves the current value and
     * status there.
     * 
     * On entry. gCacheMutex must not be held. Value must be
     * included in the reference count of the object to which it points.
     * 
     * On exit, value and status are changed to what was already in the cache if
     * something was there and not in progress. Otherwise, value and status are left
     * unchanged in which case they are placed in the cache on a best-effort basis.
     * Caller must call removeRef() on value.
     */
   ଶୂନ୍ୟ _putIfAbsentAndGet(
           ଅଚଳ Cache_ଚାବି_Base &ଚାବି,
           ଅଚଳ SharedObject *&ମୂଲ୍ୟ_2,
           UErrorCode &status) ଅଚଳ;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    ଅଚଳ UHashElement *_nextElement() ଅଚଳ;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_ଗଣନା_Of_ବସ୍ତୁଗୁଡ଼ିକ_To_Evict() ଅଚଳ;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   ଶୂନ୍ୟ _runEvictionSlice() ଅଚଳ;
 
   /**
    * Register a primary cache entry. A primary key is the first key to create
    * a given  SharedObject value. Subsequent keys whose create function
    * produce references to an already existing SharedObject are not primary -
    * they can be evicted and subsequently recreated.
    * 
    * On entry, gCacheMutex must be held.
    * On exit, items in use count incremented, entry is marked as a primary
    * entry, and value registered with cache so that subsequent calls to
    * addRef() and removeRef() on it correctly interact with the cache.
    */
   ଶୂନ୍ୟ _registerPrimary(ଅଚଳ Cache_ଚାବି_Base *the_ଚାବି, ଅଚଳ SharedObject *ମୂଲ୍ୟ_2) ଅଚଳ;
        
   /**
    * Store a value and creation error status in given hash entry.
    * On entry, gCacheMutex must be held. Hash entry element must be in progress.
    * value must be non nullptr.
    * On Exit, soft reference added to value. value and status stored in hash
    * entry. Soft reference removed from previous stored value. Waiting
    * threads notified.
    */
   ଶୂନ୍ୟ _put(
           ଅଚଳ UHashElement *element,
           ଅଚଳ SharedObject *ମୂଲ୍ୟ_2,
           ଅଚଳ UErrorCode status) ଅଚଳ;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   ଶୂନ୍ୟ removeSoftRef(ଅଚଳ SharedObject *ମୂଲ୍ୟ_2) ଅଚଳ;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t addHardRef(ଅଚଳ SharedObject *ମୂଲ୍ୟ_2) ଅଚଳ;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(ଅଚଳ SharedObject *ମୂଲ୍ୟ_2) ଅଚଳ;

   
#ifdef UNIFIED_CACHE_DEBUG
   ଶୂନ୍ୟ _dumpContents() ଅଚଳ;
#endif
   
   /**
    *  Fetch value and error code from a particular hash entry.
    *  On entry, gCacheMutex must be held. value must be either nullptr or must be
    *  included in the ref count of the object to which it points.
    *  On exit, value and status set to what is in the hash entry. Caller must
    *  eventually call removeRef on value.
    *  If hash entry is in progress, value will be set to gNoValue and status will
    *  be set to U_ZERO_ERROR.
    */
   ଶୂନ୍ୟ _fetch(ଅଚଳ UHashElement *element, ଅଚଳ SharedObject *&ମୂଲ୍ୟ_2,
                       UErrorCode &status) ଅଚଳ;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool _inProgress(ଅଚଳ UHashElement *element) ଅଚଳ;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool _inProgress(ଅଚଳ SharedObject *the_ମୂଲ୍ୟ, UErrorCode creationStatus) ଅଚଳ;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(ଅଚଳ UHashElement *element) ଅଚଳ;
};

U_NAMESPACE_END

#endif

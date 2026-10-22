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

#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "sharedobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

ಸಂರಚನಾ UHashtable;
ಸಂರಚನಾ UHashElement;

U_NAMESPACE_BEGIN

class UnifiedCache;

/**
 * A base class for all cache keys.
 */
class U_COMMON_API Cache_ಕೀಲಿ_Base : ಸಾರ್ವಜನಿಕ UObject {
 ಸಾರ್ವಜನಿಕ:
   Cache_ಕೀಲಿ_Base() : fCreationStatus(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_ಕೀಲಿ_Base(ಧ್ರುವ Cache_ಕೀಲಿ_Base &other) 
           : UObject(other), fCreationStatus(other.fCreationStatus), fIsPrimary(false) { }
   ಆಭಾಸ ~Cache_ಕೀಲಿ_Base();

   /**
    * Returns the hash code for this object.
    */
   ಆಭಾಸ int32_t hashCode() ಧ್ರುವ = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   ಆಭಾಸ Cache_ಕೀಲಿ_Base *clone() ಧ್ರುವ = 0;

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
   ಆಭಾಸ ಧ್ರುವ SharedObject *createObject(
           ಧ್ರುವ ಶೂನ್ಯ *creationContext, UErrorCode &status) ಧ್ರುವ = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   ಆಭಾಸ ವರ್ಣ *writeDescription(ವರ್ಣ *buffer, int32_t bufSize) ಧ್ರುವ = 0;

   friend ಅನ್ತಃಸ್ಥ bool operator==(ಧ್ರುವ Cache_ಕೀಲಿ_Base& lhs,
                                 ಧ್ರುವ Cache_ಕೀಲಿ_Base& rhs) {
       ಪ್ರತ್ಯಾಗಮ lhs.equals(rhs);
   }

   friend ಅನ್ತಃಸ್ಥ bool operator!=(ಧ್ರುವ Cache_ಕೀಲಿ_Base& lhs,
                                 ಧ್ರುವ Cache_ಕೀಲಿ_Base& rhs) {
       ಪ್ರತ್ಯಾಗಮ !lhs.equals(rhs);
   }

 ರಕ್ಷಿತ:
   ಆಭಾಸ bool equals(ಧ್ರುವ Cache_ಕೀಲಿ_Base& other) ಧ್ರುವ = 0;

 ಗುಪ್ತ:
   mutable UErrorCode fCreationStatus;
   mutable UBool fIsPrimary;
   friend class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
template<typename T>
class Cache_ಕೀಲಿ : ಸಾರ್ವಜನಿಕ Cache_ಕೀಲಿ_Base {
 ಸಾರ್ವಜನಿಕ:
   ಆಭಾಸ ~Cache_ಕೀಲಿ() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   ಆಭಾಸ int32_t hashCode() ಧ್ರುವ override {
       ಧ್ರುವ ವರ್ಣ *s = typeid(T).name();
       ಪ್ರತ್ಯಾಗಮ ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   ಆಭಾಸ ವರ್ಣ *writeDescription(ವರ್ಣ *buffer, int32_t bufLen) ಧ್ರುವ override {
       ಧ್ರುವ ವರ್ಣ *s = typeid(T).name();
       uprv_strncpy(buffer, s, bufLen);
       buffer[bufLen - 1] = 0;
       ಪ್ರತ್ಯಾಗಮ buffer;
   }

 ರಕ್ಷಿತ:
   /**
    * Two objects are equal if they are of the same type.
    */
   ಆಭಾಸ bool equals(ಧ್ರುವ Cache_ಕೀಲಿ_Base &other) ಧ್ರುವ override {
       ಪ್ರತ್ಯಾಗಮ this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
template<typename T>
class Locale_Cache_ಕೀಲಿ : ಸಾರ್ವಜನಿಕ Cache_ಕೀಲಿ<T> {
 ರಕ್ಷಿತ:
   Locale   fLoc;
   ಆಭಾಸ bool equals(ಧ್ರುವ Cache_ಕೀಲಿ_Base &other) ಧ್ರುವ override {
       ಯದಿ (!Cache_ಕೀಲಿ<T>::equals(other)) {
           ಪ್ರತ್ಯಾಗಮ false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       ಪ್ರತ್ಯಾಗಮ operator==(static_cast<ಧ್ರುವ Locale_Cache_ಕೀಲಿ<T> &>(other));
   }
 ಸಾರ್ವಜನಿಕ:
   Locale_Cache_ಕೀಲಿ(ಧ್ರುವ Locale &loc) : fLoc(loc) {}
   Locale_Cache_ಕೀಲಿ(ಧ್ರುವ Locale_Cache_ಕೀಲಿ<T> &other)
           : Cache_ಕೀಲಿ<T>(other), fLoc(other.fLoc) { }
   ಆಭಾಸ ~Locale_Cache_ಕೀಲಿ() { }
   ಆಭಾಸ int32_t hashCode() ಧ್ರುವ override {
       ಪ್ರತ್ಯಾಗಮ (int32_t)(37u * (uint32_t)Cache_ಕೀಲಿ<T>::hashCode() + (uint32_t)fLoc.hashCode());
   }
   ಅನ್ತಃಸ್ಥ bool operator == (ಧ್ರುವ Locale_Cache_ಕೀಲಿ<T> &other) ಧ್ರುವ {
       ಪ್ರತ್ಯಾಗಮ fLoc == other.fLoc;
   }
   ಆಭಾಸ Cache_ಕೀಲಿ_Base *clone() ಧ್ರುವ override {
       ಪ್ರತ್ಯಾಗಮ new Locale_Cache_ಕೀಲಿ<T>(*this);
   }
   ಆಭಾಸ ಧ್ರುವ T *createObject(
           ಧ್ರುವ ಶೂನ್ಯ *creationContext, UErrorCode &status) ಧ್ರುವ override;
   /**
    * Use the locale id as the description.
    */
   ಆಭಾಸ ವರ್ಣ *writeDescription(ವರ್ಣ *buffer, int32_t bufLen) ಧ್ರುವ override {
       ಧ್ರುವ ವರ್ಣ *s = fLoc.getName();
       uprv_strncpy(buffer, s, bufLen);
       buffer[bufLen - 1] = 0;
       ಪ್ರತ್ಯಾಗಮ buffer;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : ಸಾರ್ವಜನಿಕ UnifiedCacheBase {
 ಸಾರ್ವಜನಿಕ:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UErrorCode &status);

   /**
    * Return a pointer to the global cache instance.
    */
   ಸ್ಥಿರ UnifiedCache *getInstance(UErrorCode &status);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   template<typename T>
   ಶೂನ್ಯ get(
           ಧ್ರುವ Cache_ಕೀಲಿ<T>& ಕೀಲಿ,
           ಧ್ರುವ T *&ptr,
           UErrorCode &status) ಧ್ರುವ {
       get(ಕೀಲಿ, nullptr, ptr, status);
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
   ಶೂನ್ಯ get(
           ಧ್ರುವ Cache_ಕೀಲಿ<T>& ಕೀಲಿ,
           ಧ್ರುವ ಶೂನ್ಯ *creationContext,
           ಧ್ರುವ T *&ptr,
           UErrorCode &status) ಧ್ರುವ {
       ಯದಿ (U_FAILURE(status)) {
           ಪ್ರತ್ಯಾಗಮ;
       }
       UErrorCode creationStatus = U_ZERO_ERROR;
       ಧ್ರುವ SharedObject *ಮೌಲ್ಯ_2 = nullptr;
       _get(ಕೀಲಿ, ಮೌಲ್ಯ_2, creationContext, creationStatus);
       ಧ್ರುವ T *tvalue = (ಧ್ರುವ T *) ಮೌಲ್ಯ_2;
       ಯದಿ (U_SUCCESS(creationStatus)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       ಯದಿ (status == U_ZERO_ERROR || U_FAILURE(creationStatus)) {
           status = creationStatus;
       }
   }

#ifdef UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   ಶೂನ್ಯ dumpContents() ಧ್ರುವ;
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
   ಸ್ಥಿರ ಶೂನ್ಯ getByLocale(
           ಧ್ರುವ Locale &loc, ಧ್ರುವ T *&ptr, UErrorCode &status) {
       ಧ್ರುವ UnifiedCache *cache = getInstance(status);
       ಯದಿ (U_FAILURE(status)) {
           ಪ್ರತ್ಯಾಗಮ;
       }
       cache->get(Locale_Cache_ಕೀಲಿ<T>(loc), ptr, status);
   }

#ifdef UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   ಸ್ಥಿರ ಶೂನ್ಯ dump();
#endif

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t ಕೀಲಿ_ಎಣಿಕೆ() ಧ್ರುವ;

   /**
    * Removes any values from cache that are not referenced outside
    * the cache.
    */
   ಶೂನ್ಯ flush() ಧ್ರುವ;

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
   ಶೂನ್ಯ setEvictionPolicy(
           int32_t ಎಣಿಕೆ, int32_t percentage_Of_In_Use_ವಸ್ತುಗಳು, UErrorCode &status);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_ಎಣಿಕೆ() ಧ್ರುವ;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t unused_ಎಣಿಕೆ() ಧ್ರುವ;

   ಆಭಾಸ ಶೂನ್ಯ handleUnreferencedObject() ಧ್ರುವ override;
   ಆಭಾಸ ~UnifiedCache();
   
 ಗುಪ್ತ:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_Total;
   mutable int32_t f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_In_Use;
   int32_t fMaxUnused;
   int32_t fMaxPercentageOfInUse;
   mutable int64_t f_Auto_Evicted_ಎಣಿಕೆ;
   SharedObject *f_No_ಮೌಲ್ಯ;
   
   UnifiedCache(ಧ್ರುವ UnifiedCache &other) = delete;
   UnifiedCache &operator=(ಧ್ರುವ UnifiedCache &other) = delete;
   
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
   UBool _flush(UBool all) ಧ್ರುವ;
   
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
   ಶೂನ್ಯ _get(
           ಧ್ರುವ Cache_ಕೀಲಿ_Base &ಕೀಲಿ,
           ಧ್ರುವ SharedObject *&ಮೌಲ್ಯ_2,
           ಧ್ರುವ ಶೂನ್ಯ *creationContext,
           UErrorCode &status) ಧ್ರುವ;

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
            ಧ್ರುವ Cache_ಕೀಲಿ_Base &ಕೀಲಿ,
            ಧ್ರುವ SharedObject *&ಮೌಲ್ಯ_2,
            UErrorCode &status) ಧ್ರುವ;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    ಶೂನ್ಯ _putNew(
        ಧ್ರುವ Cache_ಕೀಲಿ_Base &ಕೀಲಿ,
        ಧ್ರುವ SharedObject *ಮೌಲ್ಯ_2,
        ಧ್ರುವ UErrorCode creationStatus,
        UErrorCode &status) ಧ್ರುವ;
           
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
   ಶೂನ್ಯ _putIfAbsentAndGet(
           ಧ್ರುವ Cache_ಕೀಲಿ_Base &ಕೀಲಿ,
           ಧ್ರುವ SharedObject *&ಮೌಲ್ಯ_2,
           UErrorCode &status) ಧ್ರುವ;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    ಧ್ರುವ UHashElement *_nextElement() ಧ್ರುವ;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_ಎಣಿಕೆ_Of_ವಸ್ತುಗಳು_To_Evict() ಧ್ರುವ;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   ಶೂನ್ಯ _runEvictionSlice() ಧ್ರುವ;
 
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
   ಶೂನ್ಯ _registerPrimary(ಧ್ರುವ Cache_ಕೀಲಿ_Base *the_ಕೀಲಿ, ಧ್ರುವ SharedObject *ಮೌಲ್ಯ_2) ಧ್ರುವ;
        
   /**
    * Store a value and creation error status in given hash entry.
    * On entry, gCacheMutex must be held. Hash entry element must be in progress.
    * value must be non nullptr.
    * On Exit, soft reference added to value. value and status stored in hash
    * entry. Soft reference removed from previous stored value. Waiting
    * threads notified.
    */
   ಶೂನ್ಯ _put(
           ಧ್ರುವ UHashElement *element,
           ಧ್ರುವ SharedObject *ಮೌಲ್ಯ_2,
           ಧ್ರುವ UErrorCode status) ಧ್ರುವ;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   ಶೂನ್ಯ removeSoftRef(ಧ್ರುವ SharedObject *ಮೌಲ್ಯ_2) ಧ್ರುವ;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t addHardRef(ಧ್ರುವ SharedObject *ಮೌಲ್ಯ_2) ಧ್ರುವ;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(ಧ್ರುವ SharedObject *ಮೌಲ್ಯ_2) ಧ್ರುವ;

   
#ifdef UNIFIED_CACHE_DEBUG
   ಶೂನ್ಯ _dumpContents() ಧ್ರುವ;
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
   ಶೂನ್ಯ _fetch(ಧ್ರುವ UHashElement *element, ಧ್ರುವ SharedObject *&ಮೌಲ್ಯ_2,
                       UErrorCode &status) ಧ್ರುವ;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool _inProgress(ಧ್ರುವ UHashElement *element) ಧ್ರುವ;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool _inProgress(ಧ್ರುವ SharedObject *the_ಮೌಲ್ಯ, UErrorCode creationStatus) ಧ್ರುವ;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(ಧ್ರುವ UHashElement *element) ಧ್ರುವ;
};

U_NAMESPACE_END

#endif

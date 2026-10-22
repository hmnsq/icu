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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UNIFIED_CACHE_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UNIFIED_CACHE_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "utypeinfo.h"  // for 'typeid' to work

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sharedobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

ਬਣਤਰ UHashtable;
ਬਣਤਰ UHashElement;

U_NAMESPACE_BEGIN

class UnifiedCache;

/**
 * A base class for all cache keys.
 */
class U_COMMON_API Cache_ਕੁੰਜੀ_ਅਧਾਰ : ਜਨਤਕ UObject {
 ਜਨਤਕ:
   Cache_ਕੁੰਜੀ_ਅਧਾਰ() : f_Creation_ਸਥਿਤੀ(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_ਕੁੰਜੀ_ਅਧਾਰ(ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &other) 
           : UObject(other), f_Creation_ਸਥਿਤੀ(other.f_Creation_ਸਥਿਤੀ), fIsPrimary(false) { }
   ਆਭਾਸੀ ~Cache_ਕੁੰਜੀ_ਅਧਾਰ();

   /**
    * Returns the hash code for this object.
    */
   ਆਭਾਸੀ int32_t hash_ਕੋਡ() ਅਚਲ = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   ਆਭਾਸੀ Cache_ਕੁੰਜੀ_ਅਧਾਰ *clone() ਅਚਲ = 0;

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
   ਆਭਾਸੀ ਅਚਲ SharedObject *createObject(
           ਅਚਲ ਖਾਲੀ *creationContext, UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   ਆਭਾਸੀ ਅੱਖਰ *writeDescription(ਅੱਖਰ *ਬਫਰ, int32_t buf_ਮਾਪ) ਅਚਲ = 0;

   ਮਿੱਤਰ ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ==(ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ& lhs,
                                 ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ& rhs) {
       ਵਾਪਸ lhs.equals(rhs);
   }

   ਮਿੱਤਰ ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ& lhs,
                                 ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ& rhs) {
       ਵਾਪਸ !lhs.equals(rhs);
   }

 ਸੁਰੱਖਿਅਤ:
   ਆਭਾਸੀ bool equals(ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ& other) ਅਚਲ = 0;

 ਨਿਜੀ:
   mutable UError_ਕੋਡ f_Creation_ਸਥਿਤੀ;
   mutable UBool fIsPrimary;
   ਮਿੱਤਰ class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
ਸਾਂਚਾ<typename T>
class Cache_ਕੁੰਜੀ : ਜਨਤਕ Cache_ਕੁੰਜੀ_ਅਧਾਰ {
 ਜਨਤਕ:
   ਆਭਾਸੀ ~Cache_ਕੁੰਜੀ() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   ਆਭਾਸੀ int32_t hash_ਕੋਡ() ਅਚਲ override {
       ਅਚਲ ਅੱਖਰ *s = typeid(T).name();
       ਵਾਪਸ ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   ਆਭਾਸੀ ਅੱਖਰ *writeDescription(ਅੱਖਰ *ਬਫਰ, int32_t bufLen) ਅਚਲ override {
       ਅਚਲ ਅੱਖਰ *s = typeid(T).name();
       uprv_strncpy(ਬਫਰ, s, bufLen);
       ਬਫਰ[bufLen - 1] = 0;
       ਵਾਪਸ ਬਫਰ;
   }

 ਸੁਰੱਖਿਅਤ:
   /**
    * Two objects are equal if they are of the same type.
    */
   ਆਭਾਸੀ bool equals(ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &other) ਅਚਲ override {
       ਵਾਪਸ this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
ਸਾਂਚਾ<typename T>
class Locale_Cache_ਕੁੰਜੀ : ਜਨਤਕ Cache_ਕੁੰਜੀ<T> {
 ਸੁਰੱਖਿਅਤ:
   Locale   fLoc;
   ਆਭਾਸੀ bool equals(ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &other) ਅਚਲ override {
       ਜੇ (!Cache_ਕੁੰਜੀ<T>::equals(other)) {
           ਵਾਪਸ false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       ਵਾਪਸ ਸੰਚਾਲਕ==(static_cast<ਅਚਲ Locale_Cache_ਕੁੰਜੀ<T> &>(other));
   }
 ਜਨਤਕ:
   Locale_Cache_ਕੁੰਜੀ(ਅਚਲ Locale &loc) : fLoc(loc) {}
   Locale_Cache_ਕੁੰਜੀ(ਅਚਲ Locale_Cache_ਕੁੰਜੀ<T> &other)
           : Cache_ਕੁੰਜੀ<T>(other), fLoc(other.fLoc) { }
   ਆਭਾਸੀ ~Locale_Cache_ਕੁੰਜੀ() { }
   ਆਭਾਸੀ int32_t hash_ਕੋਡ() ਅਚਲ override {
       ਵਾਪਸ (int32_t)(37u * (uint32_t)Cache_ਕੁੰਜੀ<T>::hash_ਕੋਡ() + (uint32_t)fLoc.hash_ਕੋਡ());
   }
   ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ == (ਅਚਲ Locale_Cache_ਕੁੰਜੀ<T> &other) ਅਚਲ {
       ਵਾਪਸ fLoc == other.fLoc;
   }
   ਆਭਾਸੀ Cache_ਕੁੰਜੀ_ਅਧਾਰ *clone() ਅਚਲ override {
       ਵਾਪਸ new Locale_Cache_ਕੁੰਜੀ<T>(*this);
   }
   ਆਭਾਸੀ ਅਚਲ T *createObject(
           ਅਚਲ ਖਾਲੀ *creationContext, UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ override;
   /**
    * Use the locale id as the description.
    */
   ਆਭਾਸੀ ਅੱਖਰ *writeDescription(ਅੱਖਰ *ਬਫਰ, int32_t bufLen) ਅਚਲ override {
       ਅਚਲ ਅੱਖਰ *s = fLoc.getName();
       uprv_strncpy(ਬਫਰ, s, bufLen);
       ਬਫਰ[bufLen - 1] = 0;
       ਵਾਪਸ ਬਫਰ;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : ਜਨਤਕ Unified_Cache_ਅਧਾਰ {
 ਜਨਤਕ:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UError_ਕੋਡ &ਸਥਿਤੀ);

   /**
    * Return a pointer to the global cache instance.
    */
   ਸਥਿਰ UnifiedCache *getInstance(UError_ਕੋਡ &ਸਥਿਤੀ);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   ਸਾਂਚਾ<typename T>
   ਖਾਲੀ get(
           ਅਚਲ Cache_ਕੁੰਜੀ<T>& ਕੁੰਜੀ,
           ਅਚਲ T *&ptr,
           UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
       get(ਕੁੰਜੀ, nullptr, ptr, ਸਥਿਤੀ);
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
   ਸਾਂਚਾ<typename T>
   ਖਾਲੀ get(
           ਅਚਲ Cache_ਕੁੰਜੀ<T>& ਕੁੰਜੀ,
           ਅਚਲ ਖਾਲੀ *creationContext,
           ਅਚਲ T *&ptr,
           UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ {
       ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
           ਵਾਪਸ;
       }
       UError_ਕੋਡ creation_ਸਥਿਤੀ = U_ZERO_ERROR;
       ਅਚਲ SharedObject *ਮੁੱਲ_2 = nullptr;
       _get(ਕੁੰਜੀ, ਮੁੱਲ_2, creationContext, creation_ਸਥਿਤੀ);
       ਅਚਲ T *tvalue = (ਅਚਲ T *) ਮੁੱਲ_2;
       ਜੇ (U_SUCCESS(creation_ਸਥਿਤੀ)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       ਜੇ (ਸਥਿਤੀ == U_ZERO_ERROR || U_FAILURE(creation_ਸਥਿਤੀ)) {
           ਸਥਿਤੀ = creation_ਸਥਿਤੀ;
       }
   }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   ਖਾਲੀ dumpContents() ਅਚਲ;
#ਜੇ_ਅੰਤ

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
   ਸਾਂਚਾ<typename T>
   ਸਥਿਰ ਖਾਲੀ getByLocale(
           ਅਚਲ Locale &loc, ਅਚਲ T *&ptr, UError_ਕੋਡ &ਸਥਿਤੀ) {
       ਅਚਲ UnifiedCache *cache = getInstance(ਸਥਿਤੀ);
       ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
           ਵਾਪਸ;
       }
       cache->get(Locale_Cache_ਕੁੰਜੀ<T>(loc), ptr, ਸਥਿਤੀ);
   }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   ਸਥਿਰ ਖਾਲੀ dump();
#ਜੇ_ਅੰਤ

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t ਕੁੰਜੀ_ਗਿਣਤੀ() ਅਚਲ;

   /**
    * Removes any values from cache that are not referenced outside
    * the cache.
    */
   ਖਾਲੀ flush() ਅਚਲ;

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
   ਖਾਲੀ ਸੈੱਟ_Eviction_Policy(
           int32_t ਗਿਣਤੀ_2, int32_t percentage_ਦਾ_ਵਿੱਚ_Use_ਚੀਜ਼ਾਂ, UError_ਕੋਡ &ਸਥਿਤੀ);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_ਗਿਣਤੀ() ਅਚਲ;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t ਅਣਵਰਤਿਆ_ਗਿਣਤੀ() ਅਚਲ;

   ਆਭਾਸੀ ਖਾਲੀ handleUnreferencedObject() ਅਚਲ override;
   ਆਭਾਸੀ ~UnifiedCache();
   
 ਨਿਜੀ:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_ਅੰਕ_ਮੁੱਲਾਂ_ਕੁੱਲ;
   mutable int32_t f_ਅੰਕ_ਮੁੱਲਾਂ_ਵਿੱਚ_Use;
   int32_t f_Max_ਅਣਵਰਤਿਆ;
   int32_t f_Max_Percentage_ਦਾ_ਵਿੱਚ_Use;
   mutable int64_t f_Auto_Evicted_ਗਿਣਤੀ;
   SharedObject *f_No_ਮੁੱਲ;
   
   UnifiedCache(ਅਚਲ UnifiedCache &other) = delete;
   UnifiedCache &ਸੰਚਾਲਕ=(ਅਚਲ UnifiedCache &other) = delete;
   
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
   UBool _flush(UBool all) ਅਚਲ;
   
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
   ਖਾਲੀ _get(
           ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &ਕੁੰਜੀ,
           ਅਚਲ SharedObject *&ਮੁੱਲ_2,
           ਅਚਲ ਖਾਲੀ *creationContext,
           UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ;

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
            ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &ਕੁੰਜੀ,
            ਅਚਲ SharedObject *&ਮੁੱਲ_2,
            UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    ਖਾਲੀ _putNew(
        ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &ਕੁੰਜੀ,
        ਅਚਲ SharedObject *ਮੁੱਲ_2,
        ਅਚਲ UError_ਕੋਡ creation_ਸਥਿਤੀ,
        UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ;
           
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
   ਖਾਲੀ _putIfAbsentAndGet(
           ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ &ਕੁੰਜੀ,
           ਅਚਲ SharedObject *&ਮੁੱਲ_2,
           UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    ਅਚਲ UHashElement *ਅਗਲਾ_Element_2() ਅਚਲ;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_ਗਿਣਤੀ_ਦਾ_ਚੀਜ਼ਾਂ_ਮੰਜ਼ਿਲ_Evict() ਅਚਲ;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   ਖਾਲੀ ਲੜੀਵਾਰ_Eviction_ਟੁਕੜਾ() ਅਚਲ;
 
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
   ਖਾਲੀ ਰਜਿਸਟਰ_Primary(ਅਚਲ Cache_ਕੁੰਜੀ_ਅਧਾਰ *the_ਕੁੰਜੀ, ਅਚਲ SharedObject *ਮੁੱਲ_2) ਅਚਲ;
        
   /**
    * Store a value and creation error status in given hash entry.
    * On entry, gCacheMutex must be held. Hash entry element must be in progress.
    * value must be non nullptr.
    * On Exit, soft reference added to value. value and status stored in hash
    * entry. Soft reference removed from previous stored value. Waiting
    * threads notified.
    */
   ਖਾਲੀ _put(
           ਅਚਲ UHashElement *element,
           ਅਚਲ SharedObject *ਮੁੱਲ_2,
           ਅਚਲ UError_ਕੋਡ ਸਥਿਤੀ) ਅਚਲ;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   ਖਾਲੀ removeSoftRef(ਅਚਲ SharedObject *ਮੁੱਲ_2) ਅਚਲ;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t ਜੋੜੋ_Hard_Ref(ਅਚਲ SharedObject *ਮੁੱਲ_2) ਅਚਲ;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(ਅਚਲ SharedObject *ਮੁੱਲ_2) ਅਚਲ;

   
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNIFIED_CACHE_DEBUG
   ਖਾਲੀ _dumpContents() ਅਚਲ;
#ਜੇ_ਅੰਤ
   
   /**
    *  Fetch value and error code from a particular hash entry.
    *  On entry, gCacheMutex must be held. value must be either nullptr or must be
    *  included in the ref count of the object to which it points.
    *  On exit, value and status set to what is in the hash entry. Caller must
    *  eventually call removeRef on value.
    *  If hash entry is in progress, value will be set to gNoValue and status will
    *  be set to U_ZERO_ERROR.
    */
   ਖਾਲੀ _fetch(ਅਚਲ UHashElement *element, ਅਚਲ SharedObject *&ਮੁੱਲ_2,
                       UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool ਵਿੱਚ_ਤਰੱਕੀ(ਅਚਲ UHashElement *element) ਅਚਲ;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool ਵਿੱਚ_ਤਰੱਕੀ(ਅਚਲ SharedObject *the_ਮੁੱਲ, UError_ਕੋਡ creation_ਸਥਿਤੀ) ਅਚਲ;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(ਅਚਲ UHashElement *element) ਅਚਲ;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ

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

#నిర్వచితం_కాకపోతే __UNIFIED_CACHE_H__
#నిర్వచించు __UNIFIED_CACHE_H__

#అన్తర్భావ "utypeinfo.h"  // for 'typeid' to work

#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "sharedobject.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "ustr_imp.h"

నిర్మాణం UHashtable;
నిర్మాణం UHashElement;

U_NAMESPACE_BEGIN

class UnifiedCache;

/**
 * A base class for all cache keys.
 */
class U_COMMON_API Cache_కీలకం_పునాది : బహిరంగ UObject {
 బహిరంగ:
   Cache_కీలకం_పునాది() : f_Creation_స్థితి(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_కీలకం_పునాది(స్థిరరాశి Cache_కీలకం_పునాది &other) 
           : UObject(other), f_Creation_స్థితి(other.f_Creation_స్థితి), fIsPrimary(false) { }
   మాయా ~Cache_కీలకం_పునాది();

   /**
    * Returns the hash code for this object.
    */
   మాయా int32_t hash_సంకేతం() స్థిరరాశి = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   మాయా Cache_కీలకం_పునాది *clone() స్థిరరాశి = 0;

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
   మాయా స్థిరరాశి SharedObject *createObject(
           స్థిరరాశి శూన్య *creationContext, UError_సంకేతం &స్థితి_2) స్థిరరాశి = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   మాయా అక్షరం *writeDescription(అక్షరం *బఫరు, int32_t buf_కొలత) స్థిరరాశి = 0;

   మిత్ర అంతఃవరుస bool క్రియాకారకం==(స్థిరరాశి Cache_కీలకం_పునాది& lhs,
                                 స్థిరరాశి Cache_కీలకం_పునాది& rhs) {
       తిరిగి lhs.equals(rhs);
   }

   మిత్ర అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి Cache_కీలకం_పునాది& lhs,
                                 స్థిరరాశి Cache_కీలకం_పునాది& rhs) {
       తిరిగి !lhs.equals(rhs);
   }

 రక్షిత:
   మాయా bool equals(స్థిరరాశి Cache_కీలకం_పునాది& other) స్థిరరాశి = 0;

 అంతరంగ:
   mutable UError_సంకేతం f_Creation_స్థితి;
   mutable UBool fIsPrimary;
   మిత్ర class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
మూస<typename T>
class Cache_కీలకం : బహిరంగ Cache_కీలకం_పునాది {
 బహిరంగ:
   మాయా ~Cache_కీలకం() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   మాయా int32_t hash_సంకేతం() స్థిరరాశి override {
       స్థిరరాశి అక్షరం *s = typeid(T).name();
       తిరిగి ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   మాయా అక్షరం *writeDescription(అక్షరం *బఫరు, int32_t bufLen) స్థిరరాశి override {
       స్థిరరాశి అక్షరం *s = typeid(T).name();
       uprv_strncpy(బఫరు, s, bufLen);
       బఫరు[bufLen - 1] = 0;
       తిరిగి బఫరు;
   }

 రక్షిత:
   /**
    * Two objects are equal if they are of the same type.
    */
   మాయా bool equals(స్థిరరాశి Cache_కీలకం_పునాది &other) స్థిరరాశి override {
       తిరిగి this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
మూస<typename T>
class Locale_Cache_కీలకం : బహిరంగ Cache_కీలకం<T> {
 రక్షిత:
   Locale   fLoc;
   మాయా bool equals(స్థిరరాశి Cache_కీలకం_పునాది &other) స్థిరరాశి override {
       ఒకవేళ (!Cache_కీలకం<T>::equals(other)) {
           తిరిగి false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       తిరిగి క్రియాకారకం==(static_cast<స్థిరరాశి Locale_Cache_కీలకం<T> &>(other));
   }
 బహిరంగ:
   Locale_Cache_కీలకం(స్థిరరాశి Locale &loc) : fLoc(loc) {}
   Locale_Cache_కీలకం(స్థిరరాశి Locale_Cache_కీలకం<T> &other)
           : Cache_కీలకం<T>(other), fLoc(other.fLoc) { }
   మాయా ~Locale_Cache_కీలకం() { }
   మాయా int32_t hash_సంకేతం() స్థిరరాశి override {
       తిరిగి (int32_t)(37u * (uint32_t)Cache_కీలకం<T>::hash_సంకేతం() + (uint32_t)fLoc.hash_సంకేతం());
   }
   అంతఃవరుస bool క్రియాకారకం == (స్థిరరాశి Locale_Cache_కీలకం<T> &other) స్థిరరాశి {
       తిరిగి fLoc == other.fLoc;
   }
   మాయా Cache_కీలకం_పునాది *clone() స్థిరరాశి override {
       తిరిగి new Locale_Cache_కీలకం<T>(*this);
   }
   మాయా స్థిరరాశి T *createObject(
           స్థిరరాశి శూన్య *creationContext, UError_సంకేతం &స్థితి_2) స్థిరరాశి override;
   /**
    * Use the locale id as the description.
    */
   మాయా అక్షరం *writeDescription(అక్షరం *బఫరు, int32_t bufLen) స్థిరరాశి override {
       స్థిరరాశి అక్షరం *s = fLoc.getName();
       uprv_strncpy(బఫరు, s, bufLen);
       బఫరు[bufLen - 1] = 0;
       తిరిగి బఫరు;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : బహిరంగ Unified_Cache_పునాది {
 బహిరంగ:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UError_సంకేతం &స్థితి_2);

   /**
    * Return a pointer to the global cache instance.
    */
   స్థిర UnifiedCache *getInstance(UError_సంకేతం &స్థితి_2);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   మూస<typename T>
   శూన్య get(
           స్థిరరాశి Cache_కీలకం<T>& కీలకం,
           స్థిరరాశి T *&ptr,
           UError_సంకేతం &స్థితి_2) స్థిరరాశి {
       get(కీలకం, nullptr, ptr, స్థితి_2);
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
   మూస<typename T>
   శూన్య get(
           స్థిరరాశి Cache_కీలకం<T>& కీలకం,
           స్థిరరాశి శూన్య *creationContext,
           స్థిరరాశి T *&ptr,
           UError_సంకేతం &స్థితి_2) స్థిరరాశి {
       ఒకవేళ (U_FAILURE(స్థితి_2)) {
           తిరిగి;
       }
       UError_సంకేతం creation_స్థితి = U_ZERO_ERROR;
       స్థిరరాశి SharedObject *విలువ_2 = nullptr;
       _get(కీలకం, విలువ_2, creationContext, creation_స్థితి);
       స్థిరరాశి T *tvalue = (స్థిరరాశి T *) విలువ_2;
       ఒకవేళ (U_SUCCESS(creation_స్థితి)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       ఒకవేళ (స్థితి_2 == U_ZERO_ERROR || U_FAILURE(creation_స్థితి)) {
           స్థితి_2 = creation_స్థితి;
       }
   }

#నిర్వచితమైతే UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   శూన్య dumpContents() స్థిరరాశి;
#అయితే_ముగింపు

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
   మూస<typename T>
   స్థిర శూన్య getByLocale(
           స్థిరరాశి Locale &loc, స్థిరరాశి T *&ptr, UError_సంకేతం &స్థితి_2) {
       స్థిరరాశి UnifiedCache *cache = getInstance(స్థితి_2);
       ఒకవేళ (U_FAILURE(స్థితి_2)) {
           తిరిగి;
       }
       cache->get(Locale_Cache_కీలకం<T>(loc), ptr, స్థితి_2);
   }

#నిర్వచితమైతే UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   స్థిర శూన్య dump();
#అయితే_ముగింపు

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t కీలకం_లెక్క() స్థిరరాశి;

   /**
    * Removes any values from cache that are not referenced outside
    * the cache.
    */
   శూన్య flush() స్థిరరాశి;

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
   శూన్య సమితి_Eviction_Policy(
           int32_t లెక్క, int32_t percentage_యొక్క_లోపల_Use_వస్తువులు, UError_సంకేతం &స్థితి_2);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_లెక్క() స్థిరరాశి;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t వాడని_లెక్క() స్థిరరాశి;

   మాయా శూన్య handleUnreferencedObject() స్థిరరాశి override;
   మాయా ~UnifiedCache();
   
 అంతరంగ:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_అంకె_విలువలు_మొత్తం;
   mutable int32_t f_అంకె_విలువలు_లోపల_Use;
   int32_t f_Max_వాడని;
   int32_t f_Max_Percentage_యొక్క_లోపల_Use;
   mutable int64_t f_Auto_Evicted_లెక్క;
   SharedObject *f_No_విలువ;
   
   UnifiedCache(స్థిరరాశి UnifiedCache &other) = delete;
   UnifiedCache &క్రియాకారకం=(స్థిరరాశి UnifiedCache &other) = delete;
   
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
   UBool _flush(UBool all) స్థిరరాశి;
   
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
   శూన్య _get(
           స్థిరరాశి Cache_కీలకం_పునాది &కీలకం,
           స్థిరరాశి SharedObject *&విలువ_2,
           స్థిరరాశి శూన్య *creationContext,
           UError_సంకేతం &స్థితి_2) స్థిరరాశి;

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
            స్థిరరాశి Cache_కీలకం_పునాది &కీలకం,
            స్థిరరాశి SharedObject *&విలువ_2,
            UError_సంకేతం &స్థితి_2) స్థిరరాశి;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    శూన్య _putNew(
        స్థిరరాశి Cache_కీలకం_పునాది &కీలకం,
        స్థిరరాశి SharedObject *విలువ_2,
        స్థిరరాశి UError_సంకేతం creation_స్థితి,
        UError_సంకేతం &స్థితి_2) స్థిరరాశి;
           
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
   శూన్య _putIfAbsentAndGet(
           స్థిరరాశి Cache_కీలకం_పునాది &కీలకం,
           స్థిరరాశి SharedObject *&విలువ_2,
           UError_సంకేతం &స్థితి_2) స్థిరరాశి;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    స్థిరరాశి UHashElement *తదుపరి_Element_2() స్థిరరాశి;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_లెక్క_యొక్క_వస్తువులు_గమ్యం_Evict() స్థిరరాశి;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   శూన్య నిరంతరం_Eviction_ముక్క() స్థిరరాశి;
 
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
   శూన్య రిజిస్టరు_Primary(స్థిరరాశి Cache_కీలకం_పునాది *the_కీలకం, స్థిరరాశి SharedObject *విలువ_2) స్థిరరాశి;
        
   /**
    * Store a value and creation error status in given hash entry.
    * On entry, gCacheMutex must be held. Hash entry element must be in progress.
    * value must be non nullptr.
    * On Exit, soft reference added to value. value and status stored in hash
    * entry. Soft reference removed from previous stored value. Waiting
    * threads notified.
    */
   శూన్య _put(
           స్థిరరాశి UHashElement *element,
           స్థిరరాశి SharedObject *విలువ_2,
           స్థిరరాశి UError_సంకేతం స్థితి_2) స్థిరరాశి;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   శూన్య removeSoftRef(స్థిరరాశి SharedObject *విలువ_2) స్థిరరాశి;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t కూడు_Hard_Ref(స్థిరరాశి SharedObject *విలువ_2) స్థిరరాశి;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(స్థిరరాశి SharedObject *విలువ_2) స్థిరరాశి;

   
#నిర్వచితమైతే UNIFIED_CACHE_DEBUG
   శూన్య _dumpContents() స్థిరరాశి;
#అయితే_ముగింపు
   
   /**
    *  Fetch value and error code from a particular hash entry.
    *  On entry, gCacheMutex must be held. value must be either nullptr or must be
    *  included in the ref count of the object to which it points.
    *  On exit, value and status set to what is in the hash entry. Caller must
    *  eventually call removeRef on value.
    *  If hash entry is in progress, value will be set to gNoValue and status will
    *  be set to U_ZERO_ERROR.
    */
   శూన్య _fetch(స్థిరరాశి UHashElement *element, స్థిరరాశి SharedObject *&విలువ_2,
                       UError_సంకేతం &స్థితి_2) స్థిరరాశి;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool లోపల_పురోగతి(స్థిరరాశి UHashElement *element) స్థిరరాశి;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool లోపల_పురోగతి(స్థిరరాశి SharedObject *the_విలువ, UError_సంకేతం creation_స్థితి) స్థిరరాశి;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(స్థిరరాశి UHashElement *element) స్థిరరాశి;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు

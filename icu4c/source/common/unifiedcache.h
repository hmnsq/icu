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

#ضمن "utypeinfo.h"  // for 'typeid' to work

#ضمن "unicode/uobject.h"
#ضمن "unicode/locid.h"
#ضمن "sharedobject.h"
#ضمن "unicode/unistr.h"
#ضمن "cstring.h"
#ضمن "ustr_imp.h"

بنية UHashtable;
بنية UHashElement;

U_NAMESPACE_BEGIN

class UnifiedCache;

/**
 * A base class for all cache keys.
 */
class U_COMMON_API Cache_کلیل_Base : عام UObject {
 عام:
   Cache_کلیل_Base() : fCreationStatus(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_کلیل_Base(ثابت Cache_کلیل_Base &other) 
           : UObject(other), fCreationStatus(other.fCreationStatus), fIsPrimary(false) { }
   virtual ~Cache_کلیل_Base();

   /**
    * Returns the hash code for this object.
    */
   virtual int32_t hashCode() ثابت = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   virtual Cache_کلیل_Base *clone() ثابت = 0;

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
   virtual ثابت SharedObject *createObject(
           ثابت فراغ *creationContext, UErrorCode &status) ثابت = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   virtual حرف *writeDescription(حرف *buffer, int32_t bufSize) ثابت = 0;

   friend مضمن bool operator==(ثابت Cache_کلیل_Base& lhs,
                                 ثابت Cache_کلیل_Base& rhs) {
       أرجع lhs.equals(rhs);
   }

   friend مضمن bool operator!=(ثابت Cache_کلیل_Base& lhs,
                                 ثابت Cache_کلیل_Base& rhs) {
       أرجع !lhs.equals(rhs);
   }

 محمي:
   virtual bool equals(ثابت Cache_کلیل_Base& other) ثابت = 0;

 خاص:
   mutable UErrorCode fCreationStatus;
   mutable UBool fIsPrimary;
   friend class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
template<typename T>
class Cache_کلیل : عام Cache_کلیل_Base {
 عام:
   virtual ~Cache_کلیل() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   virtual int32_t hashCode() ثابت override {
       ثابت حرف *s = typeid(T).name();
       أرجع ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   virtual حرف *writeDescription(حرف *buffer, int32_t bufLen) ثابت override {
       ثابت حرف *s = typeid(T).name();
       uprv_strncpy(buffer, s, bufLen);
       buffer[bufLen - 1] = 0;
       أرجع buffer;
   }

 محمي:
   /**
    * Two objects are equal if they are of the same type.
    */
   virtual bool equals(ثابت Cache_کلیل_Base &other) ثابت override {
       أرجع this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
template<typename T>
class Locale_Cache_کلیل : عام Cache_کلیل<T> {
 محمي:
   Locale   fLoc;
   virtual bool equals(ثابت Cache_کلیل_Base &other) ثابت override {
       إذا (!Cache_کلیل<T>::equals(other)) {
           أرجع false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       أرجع operator==(static_cast<ثابت Locale_Cache_کلیل<T> &>(other));
   }
 عام:
   Locale_Cache_کلیل(ثابت Locale &loc) : fLoc(loc) {}
   Locale_Cache_کلیل(ثابت Locale_Cache_کلیل<T> &other)
           : Cache_کلیل<T>(other), fLoc(other.fLoc) { }
   virtual ~Locale_Cache_کلیل() { }
   virtual int32_t hashCode() ثابت override {
       أرجع (int32_t)(37u * (uint32_t)Cache_کلیل<T>::hashCode() + (uint32_t)fLoc.hashCode());
   }
   مضمن bool operator == (ثابت Locale_Cache_کلیل<T> &other) ثابت {
       أرجع fLoc == other.fLoc;
   }
   virtual Cache_کلیل_Base *clone() ثابت override {
       أرجع new Locale_Cache_کلیل<T>(*this);
   }
   virtual ثابت T *createObject(
           ثابت فراغ *creationContext, UErrorCode &status) ثابت override;
   /**
    * Use the locale id as the description.
    */
   virtual حرف *writeDescription(حرف *buffer, int32_t bufLen) ثابت override {
       ثابت حرف *s = fLoc.getName();
       uprv_strncpy(buffer, s, bufLen);
       buffer[bufLen - 1] = 0;
       أرجع buffer;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : عام UnifiedCacheBase {
 عام:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UErrorCode &status);

   /**
    * Return a pointer to the global cache instance.
    */
   ساكن UnifiedCache *getInstance(UErrorCode &status);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   template<typename T>
   فراغ get(
           ثابت Cache_کلیل<T>& کلیل,
           ثابت T *&ptr,
           UErrorCode &status) ثابت {
       get(کلیل, nullptr, ptr, status);
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
   فراغ get(
           ثابت Cache_کلیل<T>& کلیل,
           ثابت فراغ *creationContext,
           ثابت T *&ptr,
           UErrorCode &status) ثابت {
       إذا (U_FAILURE(status)) {
           أرجع;
       }
       UErrorCode creationStatus = U_ZERO_ERROR;
       ثابت SharedObject *بەها_2 = nullptr;
       _get(کلیل, بەها_2, creationContext, creationStatus);
       ثابت T *tvalue = (ثابت T *) بەها_2;
       إذا (U_SUCCESS(creationStatus)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       إذا (status == U_ZERO_ERROR || U_FAILURE(creationStatus)) {
           status = creationStatus;
       }
   }

#ifdef UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   فراغ dumpContents() ثابت;
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
   ساكن فراغ getByLocale(
           ثابت Locale &loc, ثابت T *&ptr, UErrorCode &status) {
       ثابت UnifiedCache *cache = getInstance(status);
       إذا (U_FAILURE(status)) {
           أرجع;
       }
       cache->get(Locale_Cache_کلیل<T>(loc), ptr, status);
   }

#ifdef UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   ساكن فراغ dump();
#endif

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t کلیل_ژماردن() ثابت;

   /**
    * Removes any values from cache that are not referenced outside
    * the cache.
    */
   فراغ flush() ثابت;

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
   فراغ setEvictionPolicy(
           int32_t ژماردن, int32_t percentage_Of_In_Use_کاڵاکان, UErrorCode &status);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_ژماردن() ثابت;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t unused_ژماردن() ثابت;

   virtual فراغ handleUnreferencedObject() ثابت override;
   virtual ~UnifiedCache();
   
 خاص:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_ژمارە_بەهاکان_Total;
   mutable int32_t f_ژمارە_بەهاکان_In_Use;
   int32_t fMaxUnused;
   int32_t fMaxPercentageOfInUse;
   mutable int64_t f_Auto_Evicted_ژماردن;
   SharedObject *f_No_بەها;
   
   UnifiedCache(ثابت UnifiedCache &other) = delete;
   UnifiedCache &operator=(ثابت UnifiedCache &other) = delete;
   
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
   UBool _flush(UBool all) ثابت;
   
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
   فراغ _get(
           ثابت Cache_کلیل_Base &کلیل,
           ثابت SharedObject *&بەها_2,
           ثابت فراغ *creationContext,
           UErrorCode &status) ثابت;

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
            ثابت Cache_کلیل_Base &کلیل,
            ثابت SharedObject *&بەها_2,
            UErrorCode &status) ثابت;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    فراغ _putNew(
        ثابت Cache_کلیل_Base &کلیل,
        ثابت SharedObject *بەها_2,
        ثابت UErrorCode creationStatus,
        UErrorCode &status) ثابت;
           
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
   فراغ _putIfAbsentAndGet(
           ثابت Cache_کلیل_Base &کلیل,
           ثابت SharedObject *&بەها_2,
           UErrorCode &status) ثابت;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    ثابت UHashElement *_nextElement() ثابت;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_ژماردن_Of_کاڵاکان_To_Evict() ثابت;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   فراغ _runEvictionSlice() ثابت;
 
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
   فراغ _registerPrimary(ثابت Cache_کلیل_Base *the_کلیل, ثابت SharedObject *بەها_2) ثابت;
        
   /**
    * Store a value and creation error status in given hash entry.
    * On entry, gCacheMutex must be held. Hash entry element must be in progress.
    * value must be non nullptr.
    * On Exit, soft reference added to value. value and status stored in hash
    * entry. Soft reference removed from previous stored value. Waiting
    * threads notified.
    */
   فراغ _put(
           ثابت UHashElement *element,
           ثابت SharedObject *بەها_2,
           ثابت UErrorCode status) ثابت;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   فراغ removeSoftRef(ثابت SharedObject *بەها_2) ثابت;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t addHardRef(ثابت SharedObject *بەها_2) ثابت;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(ثابت SharedObject *بەها_2) ثابت;

   
#ifdef UNIFIED_CACHE_DEBUG
   فراغ _dumpContents() ثابت;
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
   فراغ _fetch(ثابت UHashElement *element, ثابت SharedObject *&بەها_2,
                       UErrorCode &status) ثابت;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool _inProgress(ثابت UHashElement *element) ثابت;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool _inProgress(ثابت SharedObject *the_بەها, UErrorCode creationStatus) ثابت;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(ثابت UHashElement *element) ثابت;
};

U_NAMESPACE_END

#endif

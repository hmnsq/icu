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

#അന്തര്ഭാവ "utypeinfo.h"  // for 'typeid' to work

#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "sharedobject.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "ustr_imp.h"

ഘടന UHashtable;
ഘടന UHashElement;

U_NAMESPACE_BEGIN

class UnifiedCache;

/**
 * A base class for all cache keys.
 */
class U_COMMON_API Cache_താക്കോൽ_Base : പൊതു UObject {
 പൊതു:
   Cache_താക്കോൽ_Base() : fCreationStatus(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_താക്കോൽ_Base(സ്ഥിരം Cache_താക്കോൽ_Base &other) 
           : UObject(other), fCreationStatus(other.fCreationStatus), fIsPrimary(false) { }
   മായാ ~Cache_താക്കോൽ_Base();

   /**
    * Returns the hash code for this object.
    */
   മായാ int32_t hashCode() സ്ഥിരം = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   മായാ Cache_താക്കോൽ_Base *clone() സ്ഥിരം = 0;

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
   മായാ സ്ഥിരം SharedObject *createObject(
           സ്ഥിരം ശൂന്യം *creationContext, UErrorCode &status) സ്ഥിരം = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   മായാ അക്ഷരം *writeDescription(അക്ഷരം *buffer, int32_t bufSize) സ്ഥിരം = 0;

   friend വരിയിൽ bool operator==(സ്ഥിരം Cache_താക്കോൽ_Base& lhs,
                                 സ്ഥിരം Cache_താക്കോൽ_Base& rhs) {
       തിരികെ lhs.equals(rhs);
   }

   friend വരിയിൽ bool operator!=(സ്ഥിരം Cache_താക്കോൽ_Base& lhs,
                                 സ്ഥിരം Cache_താക്കോൽ_Base& rhs) {
       തിരികെ !lhs.equals(rhs);
   }

 സംരക്ഷിത:
   മായാ bool equals(സ്ഥിരം Cache_താക്കോൽ_Base& other) സ്ഥിരം = 0;

 സ്വകാര്യ:
   mutable UErrorCode fCreationStatus;
   mutable UBool fIsPrimary;
   friend class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
template<typename T>
class Cache_താക്കോൽ : പൊതു Cache_താക്കോൽ_Base {
 പൊതു:
   മായാ ~Cache_താക്കോൽ() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   മായാ int32_t hashCode() സ്ഥിരം override {
       സ്ഥിരം അക്ഷരം *s = typeid(T).name();
       തിരികെ ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   മായാ അക്ഷരം *writeDescription(അക്ഷരം *buffer, int32_t bufLen) സ്ഥിരം override {
       സ്ഥിരം അക്ഷരം *s = typeid(T).name();
       uprv_strncpy(buffer, s, bufLen);
       buffer[bufLen - 1] = 0;
       തിരികെ buffer;
   }

 സംരക്ഷിത:
   /**
    * Two objects are equal if they are of the same type.
    */
   മായാ bool equals(സ്ഥിരം Cache_താക്കോൽ_Base &other) സ്ഥിരം override {
       തിരികെ this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
template<typename T>
class Locale_Cache_താക്കോൽ : പൊതു Cache_താക്കോൽ<T> {
 സംരക്ഷിത:
   Locale   fLoc;
   മായാ bool equals(സ്ഥിരം Cache_താക്കോൽ_Base &other) സ്ഥിരം override {
       എങ്കിൽ (!Cache_താക്കോൽ<T>::equals(other)) {
           തിരികെ false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       തിരികെ operator==(static_cast<സ്ഥിരം Locale_Cache_താക്കോൽ<T> &>(other));
   }
 പൊതു:
   Locale_Cache_താക്കോൽ(സ്ഥിരം Locale &loc) : fLoc(loc) {}
   Locale_Cache_താക്കോൽ(സ്ഥിരം Locale_Cache_താക്കോൽ<T> &other)
           : Cache_താക്കോൽ<T>(other), fLoc(other.fLoc) { }
   മായാ ~Locale_Cache_താക്കോൽ() { }
   മായാ int32_t hashCode() സ്ഥിരം override {
       തിരികെ (int32_t)(37u * (uint32_t)Cache_താക്കോൽ<T>::hashCode() + (uint32_t)fLoc.hashCode());
   }
   വരിയിൽ bool operator == (സ്ഥിരം Locale_Cache_താക്കോൽ<T> &other) സ്ഥിരം {
       തിരികെ fLoc == other.fLoc;
   }
   മായാ Cache_താക്കോൽ_Base *clone() സ്ഥിരം override {
       തിരികെ new Locale_Cache_താക്കോൽ<T>(*this);
   }
   മായാ സ്ഥിരം T *createObject(
           സ്ഥിരം ശൂന്യം *creationContext, UErrorCode &status) സ്ഥിരം override;
   /**
    * Use the locale id as the description.
    */
   മായാ അക്ഷരം *writeDescription(അക്ഷരം *buffer, int32_t bufLen) സ്ഥിരം override {
       സ്ഥിരം അക്ഷരം *s = fLoc.getName();
       uprv_strncpy(buffer, s, bufLen);
       buffer[bufLen - 1] = 0;
       തിരികെ buffer;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : പൊതു UnifiedCacheBase {
 പൊതു:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UErrorCode &status);

   /**
    * Return a pointer to the global cache instance.
    */
   സ്ഥിതം UnifiedCache *getInstance(UErrorCode &status);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   template<typename T>
   ശൂന്യം get(
           സ്ഥിരം Cache_താക്കോൽ<T>& താക്കോൽ,
           സ്ഥിരം T *&ptr,
           UErrorCode &status) സ്ഥിരം {
       get(താക്കോൽ, nullptr, ptr, status);
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
   ശൂന്യം get(
           സ്ഥിരം Cache_താക്കോൽ<T>& താക്കോൽ,
           സ്ഥിരം ശൂന്യം *creationContext,
           സ്ഥിരം T *&ptr,
           UErrorCode &status) സ്ഥിരം {
       എങ്കിൽ (U_FAILURE(status)) {
           തിരികെ;
       }
       UErrorCode creationStatus = U_ZERO_ERROR;
       സ്ഥിരം SharedObject *മൂല്യം_2 = nullptr;
       _get(താക്കോൽ, മൂല്യം_2, creationContext, creationStatus);
       സ്ഥിരം T *tvalue = (സ്ഥിരം T *) മൂല്യം_2;
       എങ്കിൽ (U_SUCCESS(creationStatus)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       എങ്കിൽ (status == U_ZERO_ERROR || U_FAILURE(creationStatus)) {
           status = creationStatus;
       }
   }

#ifdef UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   ശൂന്യം dumpContents() സ്ഥിരം;
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
   സ്ഥിതം ശൂന്യം getByLocale(
           സ്ഥിരം Locale &loc, സ്ഥിരം T *&ptr, UErrorCode &status) {
       സ്ഥിരം UnifiedCache *cache = getInstance(status);
       എങ്കിൽ (U_FAILURE(status)) {
           തിരികെ;
       }
       cache->get(Locale_Cache_താക്കോൽ<T>(loc), ptr, status);
   }

#ifdef UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   സ്ഥിതം ശൂന്യം dump();
#endif

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t താക്കോൽ_എണ്ണം() സ്ഥിരം;

   /**
    * Removes any values from cache that are not referenced outside
    * the cache.
    */
   ശൂന്യം flush() സ്ഥിരം;

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
   ശൂന്യം setEvictionPolicy(
           int32_t എണ്ണം, int32_t percentage_Of_In_Use_ഇനങ്ങൾ, UErrorCode &status);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_എണ്ണം() സ്ഥിരം;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t unused_എണ്ണം() സ്ഥിരം;

   മായാ ശൂന്യം handleUnreferencedObject() സ്ഥിരം override;
   മായാ ~UnifiedCache();
   
 സ്വകാര്യ:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_അക്കം_മൂല്യങ്ങൾ_Total;
   mutable int32_t f_അക്കം_മൂല്യങ്ങൾ_In_Use;
   int32_t fMaxUnused;
   int32_t fMaxPercentageOfInUse;
   mutable int64_t f_Auto_Evicted_എണ്ണം;
   SharedObject *f_No_മൂല്യം;
   
   UnifiedCache(സ്ഥിരം UnifiedCache &other) = delete;
   UnifiedCache &operator=(സ്ഥിരം UnifiedCache &other) = delete;
   
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
   UBool _flush(UBool all) സ്ഥിരം;
   
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
   ശൂന്യം _get(
           സ്ഥിരം Cache_താക്കോൽ_Base &താക്കോൽ,
           സ്ഥിരം SharedObject *&മൂല്യം_2,
           സ്ഥിരം ശൂന്യം *creationContext,
           UErrorCode &status) സ്ഥിരം;

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
            സ്ഥിരം Cache_താക്കോൽ_Base &താക്കോൽ,
            സ്ഥിരം SharedObject *&മൂല്യം_2,
            UErrorCode &status) സ്ഥിരം;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    ശൂന്യം _putNew(
        സ്ഥിരം Cache_താക്കോൽ_Base &താക്കോൽ,
        സ്ഥിരം SharedObject *മൂല്യം_2,
        സ്ഥിരം UErrorCode creationStatus,
        UErrorCode &status) സ്ഥിരം;
           
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
   ശൂന്യം _putIfAbsentAndGet(
           സ്ഥിരം Cache_താക്കോൽ_Base &താക്കോൽ,
           സ്ഥിരം SharedObject *&മൂല്യം_2,
           UErrorCode &status) സ്ഥിരം;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    സ്ഥിരം UHashElement *_nextElement() സ്ഥിരം;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_എണ്ണം_Of_ഇനങ്ങൾ_To_Evict() സ്ഥിരം;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   ശൂന്യം _runEvictionSlice() സ്ഥിരം;
 
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
   ശൂന്യം _registerPrimary(സ്ഥിരം Cache_താക്കോൽ_Base *the_താക്കോൽ, സ്ഥിരം SharedObject *മൂല്യം_2) സ്ഥിരം;
        
   /**
    * Store a value and creation error status in given hash entry.
    * On entry, gCacheMutex must be held. Hash entry element must be in progress.
    * value must be non nullptr.
    * On Exit, soft reference added to value. value and status stored in hash
    * entry. Soft reference removed from previous stored value. Waiting
    * threads notified.
    */
   ശൂന്യം _put(
           സ്ഥിരം UHashElement *element,
           സ്ഥിരം SharedObject *മൂല്യം_2,
           സ്ഥിരം UErrorCode status) സ്ഥിരം;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   ശൂന്യം removeSoftRef(സ്ഥിരം SharedObject *മൂല്യം_2) സ്ഥിരം;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t addHardRef(സ്ഥിരം SharedObject *മൂല്യം_2) സ്ഥിരം;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(സ്ഥിരം SharedObject *മൂല്യം_2) സ്ഥിരം;

   
#ifdef UNIFIED_CACHE_DEBUG
   ശൂന്യം _dumpContents() സ്ഥിരം;
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
   ശൂന്യം _fetch(സ്ഥിരം UHashElement *element, സ്ഥിരം SharedObject *&മൂല്യം_2,
                       UErrorCode &status) സ്ഥിരം;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool _inProgress(സ്ഥിരം UHashElement *element) സ്ഥിരം;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool _inProgress(സ്ഥിരം SharedObject *the_മൂല്യം, UErrorCode creationStatus) സ്ഥിരം;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(സ്ഥിരം UHashElement *element) സ്ഥിരം;
};

U_NAMESPACE_END

#endif

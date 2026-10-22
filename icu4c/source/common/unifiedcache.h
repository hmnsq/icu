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

#अगर_अघोषित __UNIFIED_CACHE_H__
#घोषणा __UNIFIED_CACHE_H__

#समावेश "utypeinfo.h"  // for 'typeid' to work

#समावेश "unicode/uobject.h"
#समावेश "unicode/locid.h"
#समावेश "sharedobject.h"
#समावेश "unicode/unistr.h"
#समावेश "cstring.h"
#समावेश "ustr_imp.h"

काष्ठा UHashtable;
काष्ठा UHashElement;

U_NAMESPACE_BEGIN

class UnifiedCache;

/**
 * A base class for all cache keys.
 */
class U_COMMON_API Cache_कुंजी_Base : खुला UObject {
 खुला:
   Cache_कुंजी_Base() : fCreationStatus(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_कुंजी_Base(स्थिर Cache_कुंजी_Base &other) 
           : UObject(other), fCreationStatus(other.fCreationStatus), fIsPrimary(false) { }
   भव ~Cache_कुंजी_Base();

   /**
    * Returns the hash code for this object.
    */
   भव int32_t hashCode() स्थिर = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   भव Cache_कुंजी_Base *clone() स्थिर = 0;

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
   भव स्थिर SharedObject *createObject(
           स्थिर व्योम *creationContext, UErrorCode &status) स्थिर = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   भव अक्षर *writeDescription(अक्षर *बफर, int32_t buf_आकार) स्थिर = 0;

   मित्र अंतरभूत bool चालक==(स्थिर Cache_कुंजी_Base& lhs,
                                 स्थिर Cache_कुंजी_Base& rhs) {
       वापस lhs.equals(rhs);
   }

   मित्र अंतरभूत bool चालक!=(स्थिर Cache_कुंजी_Base& lhs,
                                 स्थिर Cache_कुंजी_Base& rhs) {
       वापस !lhs.equals(rhs);
   }

 रक्षित:
   भव bool equals(स्थिर Cache_कुंजी_Base& other) स्थिर = 0;

 निजी:
   mutable UErrorCode fCreationStatus;
   mutable UBool fIsPrimary;
   मित्र class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
ढाँचा<typename T>
class Cache_कुंजी : खुला Cache_कुंजी_Base {
 खुला:
   भव ~Cache_कुंजी() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   भव int32_t hashCode() स्थिर override {
       स्थिर अक्षर *s = typeid(T).name();
       वापस ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   भव अक्षर *writeDescription(अक्षर *बफर, int32_t bufLen) स्थिर override {
       स्थिर अक्षर *s = typeid(T).name();
       uprv_strncpy(बफर, s, bufLen);
       बफर[bufLen - 1] = 0;
       वापस बफर;
   }

 रक्षित:
   /**
    * Two objects are equal if they are of the same type.
    */
   भव bool equals(स्थिर Cache_कुंजी_Base &other) स्थिर override {
       वापस this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
ढाँचा<typename T>
class Locale_Cache_कुंजी : खुला Cache_कुंजी<T> {
 रक्षित:
   Locale   fLoc;
   भव bool equals(स्थिर Cache_कुंजी_Base &other) स्थिर override {
       अगर (!Cache_कुंजी<T>::equals(other)) {
           वापस false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       वापस चालक==(static_cast<स्थिर Locale_Cache_कुंजी<T> &>(other));
   }
 खुला:
   Locale_Cache_कुंजी(स्थिर Locale &loc) : fLoc(loc) {}
   Locale_Cache_कुंजी(स्थिर Locale_Cache_कुंजी<T> &other)
           : Cache_कुंजी<T>(other), fLoc(other.fLoc) { }
   भव ~Locale_Cache_कुंजी() { }
   भव int32_t hashCode() स्थिर override {
       वापस (int32_t)(37u * (uint32_t)Cache_कुंजी<T>::hashCode() + (uint32_t)fLoc.hashCode());
   }
   अंतरभूत bool चालक == (स्थिर Locale_Cache_कुंजी<T> &other) स्थिर {
       वापस fLoc == other.fLoc;
   }
   भव Cache_कुंजी_Base *clone() स्थिर override {
       वापस new Locale_Cache_कुंजी<T>(*this);
   }
   भव स्थिर T *createObject(
           स्थिर व्योम *creationContext, UErrorCode &status) स्थिर override;
   /**
    * Use the locale id as the description.
    */
   भव अक्षर *writeDescription(अक्षर *बफर, int32_t bufLen) स्थिर override {
       स्थिर अक्षर *s = fLoc.getName();
       uprv_strncpy(बफर, s, bufLen);
       बफर[bufLen - 1] = 0;
       वापस बफर;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : खुला UnifiedCacheBase {
 खुला:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UErrorCode &status);

   /**
    * Return a pointer to the global cache instance.
    */
   अटल UnifiedCache *getInstance(UErrorCode &status);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   ढाँचा<typename T>
   व्योम get(
           स्थिर Cache_कुंजी<T>& कुंजी,
           स्थिर T *&ptr,
           UErrorCode &status) स्थिर {
       get(कुंजी, nullptr, ptr, status);
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
   ढाँचा<typename T>
   व्योम get(
           स्थिर Cache_कुंजी<T>& कुंजी,
           स्थिर व्योम *creationContext,
           स्थिर T *&ptr,
           UErrorCode &status) स्थिर {
       अगर (U_FAILURE(status)) {
           वापस;
       }
       UErrorCode creationStatus = U_ZERO_ERROR;
       स्थिर SharedObject *मूल्य_2 = nullptr;
       _get(कुंजी, मूल्य_2, creationContext, creationStatus);
       स्थिर T *tvalue = (स्थिर T *) मूल्य_2;
       अगर (U_SUCCESS(creationStatus)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       अगर (status == U_ZERO_ERROR || U_FAILURE(creationStatus)) {
           status = creationStatus;
       }
   }

#अगर_घोषित UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   व्योम dumpContents() स्थिर;
#पूर्ण_अगर

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
   ढाँचा<typename T>
   अटल व्योम getByLocale(
           स्थिर Locale &loc, स्थिर T *&ptr, UErrorCode &status) {
       स्थिर UnifiedCache *cache = getInstance(status);
       अगर (U_FAILURE(status)) {
           वापस;
       }
       cache->get(Locale_Cache_कुंजी<T>(loc), ptr, status);
   }

#अगर_घोषित UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   अटल व्योम dump();
#पूर्ण_अगर

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t कुंजी_गिनती() स्थिर;

   /**
    * Removes any values from cache that are not referenced outside
    * the cache.
    */
   व्योम flush() स्थिर;

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
   व्योम setEvictionPolicy(
           int32_t गिनती, int32_t percentage_Of_In_Use_वस्तुएँ, UErrorCode &status);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_गिनती() स्थिर;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t unused_गिनती() स्थिर;

   भव व्योम handleUnreferencedObject() स्थिर override;
   भव ~UnifiedCache();
   
 निजी:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_अंक_मूल्यावली_योग;
   mutable int32_t f_अंक_मूल्यावली_In_Use;
   int32_t fMaxUnused;
   int32_t fMaxPercentageOfInUse;
   mutable int64_t f_Auto_Evicted_गिनती;
   SharedObject *f_No_मूल्य;
   
   UnifiedCache(स्थिर UnifiedCache &other) = delete;
   UnifiedCache &चालक=(स्थिर UnifiedCache &other) = delete;
   
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
   UBool _flush(UBool all) स्थिर;
   
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
   व्योम _get(
           स्थिर Cache_कुंजी_Base &कुंजी,
           स्थिर SharedObject *&मूल्य_2,
           स्थिर व्योम *creationContext,
           UErrorCode &status) स्थिर;

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
            स्थिर Cache_कुंजी_Base &कुंजी,
            स्थिर SharedObject *&मूल्य_2,
            UErrorCode &status) स्थिर;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    व्योम _putNew(
        स्थिर Cache_कुंजी_Base &कुंजी,
        स्थिर SharedObject *मूल्य_2,
        स्थिर UErrorCode creationStatus,
        UErrorCode &status) स्थिर;
           
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
   व्योम _putIfAbsentAndGet(
           स्थिर Cache_कुंजी_Base &कुंजी,
           स्थिर SharedObject *&मूल्य_2,
           UErrorCode &status) स्थिर;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    स्थिर UHashElement *अग्र_Element_2() स्थिर;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_गिनती_Of_वस्तुएँ_To_Evict() स्थिर;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   व्योम _runEvictionSlice() स्थिर;
 
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
   व्योम _registerPrimary(स्थिर Cache_कुंजी_Base *the_कुंजी, स्थिर SharedObject *मूल्य_2) स्थिर;
        
   /**
    * Store a value and creation error status in given hash entry.
    * On entry, gCacheMutex must be held. Hash entry element must be in progress.
    * value must be non nullptr.
    * On Exit, soft reference added to value. value and status stored in hash
    * entry. Soft reference removed from previous stored value. Waiting
    * threads notified.
    */
   व्योम _put(
           स्थिर UHashElement *element,
           स्थिर SharedObject *मूल्य_2,
           स्थिर UErrorCode status) स्थिर;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   व्योम removeSoftRef(स्थिर SharedObject *मूल्य_2) स्थिर;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t addHardRef(स्थिर SharedObject *मूल्य_2) स्थिर;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(स्थिर SharedObject *मूल्य_2) स्थिर;

   
#अगर_घोषित UNIFIED_CACHE_DEBUG
   व्योम _dumpContents() स्थिर;
#पूर्ण_अगर
   
   /**
    *  Fetch value and error code from a particular hash entry.
    *  On entry, gCacheMutex must be held. value must be either nullptr or must be
    *  included in the ref count of the object to which it points.
    *  On exit, value and status set to what is in the hash entry. Caller must
    *  eventually call removeRef on value.
    *  If hash entry is in progress, value will be set to gNoValue and status will
    *  be set to U_ZERO_ERROR.
    */
   व्योम _fetch(स्थिर UHashElement *element, स्थिर SharedObject *&मूल्य_2,
                       UErrorCode &status) स्थिर;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool _inProgress(स्थिर UHashElement *element) स्थिर;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool _inProgress(स्थिर SharedObject *the_मूल्य, UErrorCode creationStatus) स्थिर;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(स्थिर UHashElement *element) स्थिर;
};

U_NAMESPACE_END

#पूर्ण_अगर

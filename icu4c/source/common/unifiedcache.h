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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __UNIFIED_CACHE_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __UNIFIED_CACHE_H__

#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work

#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "sharedobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

ರಚನೆ UHashtable;
ರಚನೆ UHashElement;

U_NAMESPACE_BEGIN

class UnifiedCache;

/**
 * A base class for all cache keys.
 */
class U_COMMON_API Cache_ಕೀಲಿ_ತಳ : ಸಾರ್ವಜನಿಕ UObject {
 ಸಾರ್ವಜನಿಕ:
   Cache_ಕೀಲಿ_ತಳ() : f_Creation_ಸ್ಥಿತಿ(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_ಕೀಲಿ_ತಳ(ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &other) 
           : UObject(other), f_Creation_ಸ್ಥಿತಿ(other.f_Creation_ಸ್ಥಿತಿ), fIsPrimary(false) { }
   ವಾಸ್ತವಿಕ ~Cache_ಕೀಲಿ_ತಳ();

   /**
    * Returns the hash code for this object.
    */
   ವಾಸ್ತವಿಕ int32_t hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   ವಾಸ್ತವಿಕ Cache_ಕೀಲಿ_ತಳ *clone() ಸ್ಥಿರಾಂಕ = 0;

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
   ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ SharedObject *createObject(
           ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *creationContext, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   ವಾಸ್ತವಿಕ ಅಕ್ಷರ *writeDescription(ಅಕ್ಷರ *ಬಫರ್, int32_t buf_ಗಾತ್ರ) ಸ್ಥಿರಾಂಕ = 0;

   ಮಿತ್ರ ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ& lhs,
                                 ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ& rhs) {
       ಹಿಂತಿರುಗಿ lhs.equals(rhs);
   }

   ಮಿತ್ರ ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ& lhs,
                                 ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ& rhs) {
       ಹಿಂತಿರುಗಿ !lhs.equals(rhs);
   }

 ಸಂರಕ್ಷಿತ:
   ವಾಸ್ತವಿಕ bool equals(ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ& other) ಸ್ಥಿರಾಂಕ = 0;

 ಖಾಸಗಿ:
   mutable UError_ಸಂಕೇತ f_Creation_ಸ್ಥಿತಿ;
   mutable UBool fIsPrimary;
   ಮಿತ್ರ class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
ಮಾದರಿ<typename T>
class Cache_ಕೀಲಿ : ಸಾರ್ವಜನಿಕ Cache_ಕೀಲಿ_ತಳ {
 ಸಾರ್ವಜನಿಕ:
   ವಾಸ್ತವಿಕ ~Cache_ಕೀಲಿ() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   ವಾಸ್ತವಿಕ int32_t hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ override {
       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = typeid(T).name();
       ಹಿಂತಿರುಗಿ ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   ವಾಸ್ತವಿಕ ಅಕ್ಷರ *writeDescription(ಅಕ್ಷರ *ಬಫರ್, int32_t bufLen) ಸ್ಥಿರಾಂಕ override {
       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = typeid(T).name();
       uprv_strncpy(ಬಫರ್, s, bufLen);
       ಬಫರ್[bufLen - 1] = 0;
       ಹಿಂತಿರುಗಿ ಬಫರ್;
   }

 ಸಂರಕ್ಷಿತ:
   /**
    * Two objects are equal if they are of the same type.
    */
   ವಾಸ್ತವಿಕ bool equals(ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &other) ಸ್ಥಿರಾಂಕ override {
       ಹಿಂತಿರುಗಿ this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
ಮಾದರಿ<typename T>
class Locale_Cache_ಕೀಲಿ : ಸಾರ್ವಜನಿಕ Cache_ಕೀಲಿ<T> {
 ಸಂರಕ್ಷಿತ:
   Locale   fLoc;
   ವಾಸ್ತವಿಕ bool equals(ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &other) ಸ್ಥಿರಾಂಕ override {
       ಆದರೆ (!Cache_ಕೀಲಿ<T>::equals(other)) {
           ಹಿಂತಿರುಗಿ false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       ಹಿಂತಿರುಗಿ ಕ್ರಿಯಾಕಾರಕ==(static_cast<ಸ್ಥಿರಾಂಕ Locale_Cache_ಕೀಲಿ<T> &>(other));
   }
 ಸಾರ್ವಜನಿಕ:
   Locale_Cache_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ Locale &loc) : fLoc(loc) {}
   Locale_Cache_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ Locale_Cache_ಕೀಲಿ<T> &other)
           : Cache_ಕೀಲಿ<T>(other), fLoc(other.fLoc) { }
   ವಾಸ್ತವಿಕ ~Locale_Cache_ಕೀಲಿ() { }
   ವಾಸ್ತವಿಕ int32_t hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ override {
       ಹಿಂತಿರುಗಿ (int32_t)(37u * (uint32_t)Cache_ಕೀಲಿ<T>::hash_ಸಂಕೇತ() + (uint32_t)fLoc.hash_ಸಂಕೇತ());
   }
   ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ == (ಸ್ಥಿರಾಂಕ Locale_Cache_ಕೀಲಿ<T> &other) ಸ್ಥಿರಾಂಕ {
       ಹಿಂತಿರುಗಿ fLoc == other.fLoc;
   }
   ವಾಸ್ತವಿಕ Cache_ಕೀಲಿ_ತಳ *clone() ಸ್ಥಿರಾಂಕ override {
       ಹಿಂತಿರುಗಿ new Locale_Cache_ಕೀಲಿ<T>(*this);
   }
   ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ T *createObject(
           ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *creationContext, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;
   /**
    * Use the locale id as the description.
    */
   ವಾಸ್ತವಿಕ ಅಕ್ಷರ *writeDescription(ಅಕ್ಷರ *ಬಫರ್, int32_t bufLen) ಸ್ಥಿರಾಂಕ override {
       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = fLoc.getName();
       uprv_strncpy(ಬಫರ್, s, bufLen);
       ಬಫರ್[bufLen - 1] = 0;
       ಹಿಂತಿರುಗಿ ಬಫರ್;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : ಸಾರ್ವಜನಿಕ Unified_Cache_ತಳ {
 ಸಾರ್ವಜನಿಕ:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

   /**
    * Return a pointer to the global cache instance.
    */
   ಸ್ಥಿರ UnifiedCache *getInstance(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   ಮಾದರಿ<typename T>
   ಶೂನ್ಯ get(
           ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ<T>& ಕೀಲಿ,
           ಸ್ಥಿರಾಂಕ T *&ptr,
           UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
       get(ಕೀಲಿ, nullptr, ptr, ಸ್ಥಿತಿ_2);
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
   ಮಾದರಿ<typename T>
   ಶೂನ್ಯ get(
           ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ<T>& ಕೀಲಿ,
           ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *creationContext,
           ಸ್ಥಿರಾಂಕ T *&ptr,
           UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
       ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
           ಹಿಂತಿರುಗಿ;
       }
       UError_ಸಂಕೇತ creation_ಸ್ಥಿತಿ = U_ZERO_ERROR;
       ಸ್ಥಿರಾಂಕ SharedObject *ಮೌಲ್ಯ_2 = nullptr;
       _get(ಕೀಲಿ, ಮೌಲ್ಯ_2, creationContext, creation_ಸ್ಥಿತಿ);
       ಸ್ಥಿರಾಂಕ T *tvalue = (ಸ್ಥಿರಾಂಕ T *) ಮೌಲ್ಯ_2;
       ಆದರೆ (U_SUCCESS(creation_ಸ್ಥಿತಿ)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       ಆದರೆ (ಸ್ಥಿತಿ_2 == U_ZERO_ERROR || U_FAILURE(creation_ಸ್ಥಿತಿ)) {
           ಸ್ಥಿತಿ_2 = creation_ಸ್ಥಿತಿ;
       }
   }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   ಶೂನ್ಯ dumpContents() ಸ್ಥಿರಾಂಕ;
#ಆದರೆ_ಅಂತ್ಯ

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
   ಮಾದರಿ<typename T>
   ಸ್ಥಿರ ಶೂನ್ಯ getByLocale(
           ಸ್ಥಿರಾಂಕ Locale &loc, ಸ್ಥಿರಾಂಕ T *&ptr, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
       ಸ್ಥಿರಾಂಕ UnifiedCache *cache = getInstance(ಸ್ಥಿತಿ_2);
       ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
           ಹಿಂತಿರುಗಿ;
       }
       cache->get(Locale_Cache_ಕೀಲಿ<T>(loc), ptr, ಸ್ಥಿತಿ_2);
   }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   ಸ್ಥಿರ ಶೂನ್ಯ dump();
#ಆದರೆ_ಅಂತ್ಯ

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t ಕೀಲಿ_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ;

   /**
    * Removes any values from cache that are not referenced outside
    * the cache.
    */
   ಶೂನ್ಯ flush() ಸ್ಥಿರಾಂಕ;

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
   ಶೂನ್ಯ ಗಣ_Eviction_Policy(
           int32_t ಎಣಿಕೆ_2, int32_t percentage_ದ_ಒಳ_Use_ವಸ್ತುಗಳು, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t ಬಳಸದ_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ;

   ವಾಸ್ತವಿಕ ಶೂನ್ಯ handleUnreferencedObject() ಸ್ಥಿರಾಂಕ override;
   ವಾಸ್ತವಿಕ ~UnifiedCache();
   
 ಖಾಸಗಿ:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಟ್ಟು;
   mutable int32_t f_ಅಂಕೆ_ಮೌಲ್ಯಗಳು_ಒಳ_Use;
   int32_t f_Max_ಬಳಸದ;
   int32_t f_Max_Percentage_ದ_ಒಳ_Use;
   mutable int64_t f_Auto_Evicted_ಎಣಿಕೆ;
   SharedObject *f_No_ಮೌಲ್ಯ;
   
   UnifiedCache(ಸ್ಥಿರಾಂಕ UnifiedCache &other) = delete;
   UnifiedCache &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UnifiedCache &other) = delete;
   
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
   UBool _flush(UBool all) ಸ್ಥಿರಾಂಕ;
   
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
           ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &ಕೀಲಿ,
           ಸ್ಥಿರಾಂಕ SharedObject *&ಮೌಲ್ಯ_2,
           ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *creationContext,
           UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

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
            ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &ಕೀಲಿ,
            ಸ್ಥಿರಾಂಕ SharedObject *&ಮೌಲ್ಯ_2,
            UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    ಶೂನ್ಯ _putNew(
        ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &ಕೀಲಿ,
        ಸ್ಥಿರಾಂಕ SharedObject *ಮೌಲ್ಯ_2,
        ಸ್ಥಿರಾಂಕ UError_ಸಂಕೇತ creation_ಸ್ಥಿತಿ,
        UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;
           
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
           ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ &ಕೀಲಿ,
           ಸ್ಥಿರಾಂಕ SharedObject *&ಮೌಲ್ಯ_2,
           UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    ಸ್ಥಿರಾಂಕ UHashElement *ಮುಂದಿನ_Element_2() ಸ್ಥಿರಾಂಕ;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_ಎಣಿಕೆ_ದ_ವಸ್ತುಗಳು_ಗಮ್ಯ_Evict() ಸ್ಥಿರಾಂಕ;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   ಶೂನ್ಯ ಸತತ_Eviction_ತುಂಡು() ಸ್ಥಿರಾಂಕ;
 
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
   ಶೂನ್ಯ ರಿಜಿಸ್ಟರ್_Primary(ಸ್ಥಿರಾಂಕ Cache_ಕೀಲಿ_ತಳ *the_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ SharedObject *ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ;
        
   /**
    * Store a value and creation error status in given hash entry.
    * On entry, gCacheMutex must be held. Hash entry element must be in progress.
    * value must be non nullptr.
    * On Exit, soft reference added to value. value and status stored in hash
    * entry. Soft reference removed from previous stored value. Waiting
    * threads notified.
    */
   ಶೂನ್ಯ _put(
           ಸ್ಥಿರಾಂಕ UHashElement *element,
           ಸ್ಥಿರಾಂಕ SharedObject *ಮೌಲ್ಯ_2,
           ಸ್ಥಿರಾಂಕ UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   ಶೂನ್ಯ removeSoftRef(ಸ್ಥಿರಾಂಕ SharedObject *ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t ಕೂಡಿಸು_Hard_Ref(ಸ್ಥಿರಾಂಕ SharedObject *ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(ಸ್ಥಿರಾಂಕ SharedObject *ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ;

   
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNIFIED_CACHE_DEBUG
   ಶೂನ್ಯ _dumpContents() ಸ್ಥಿರಾಂಕ;
#ಆದರೆ_ಅಂತ್ಯ
   
   /**
    *  Fetch value and error code from a particular hash entry.
    *  On entry, gCacheMutex must be held. value must be either nullptr or must be
    *  included in the ref count of the object to which it points.
    *  On exit, value and status set to what is in the hash entry. Caller must
    *  eventually call removeRef on value.
    *  If hash entry is in progress, value will be set to gNoValue and status will
    *  be set to U_ZERO_ERROR.
    */
   ಶೂನ್ಯ _fetch(ಸ್ಥಿರಾಂಕ UHashElement *element, ಸ್ಥಿರಾಂಕ SharedObject *&ಮೌಲ್ಯ_2,
                       UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool ಒಳ_ಪ್ರಗತಿ(ಸ್ಥಿರಾಂಕ UHashElement *element) ಸ್ಥಿರಾಂಕ;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool ಒಳ_ಪ್ರಗತಿ(ಸ್ಥಿರಾಂಕ SharedObject *the_ಮೌಲ್ಯ, UError_ಸಂಕೇತ creation_ಸ್ಥಿತಿ) ಸ್ಥಿರಾಂಕ;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(ಸ್ಥಿರಾಂಕ UHashElement *element) ಸ್ಥಿರಾಂಕ;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

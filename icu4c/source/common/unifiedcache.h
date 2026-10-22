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

#यदि_अपरिभाषितम् __UNIFIED_CACHE_H__
#परिभाषय __UNIFIED_CACHE_H__

#अन्तर्भाव "utypeinfo.h"  // for 'typeid' to work

#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "sharedobject.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ustr_imp.h"

संरचना UHashtable;
संरचना UHashElement;

U_NAMESPACE_BEGIN

class UnifiedCache;

/**
 * A base class for all cache keys.
 */
class U_COMMON_API Cache_कुञ्चिका_आधारः : सार्वजनिक UObject {
 सार्वजनिक:
   Cache_कुञ्चिका_आधारः() : f_Creation_स्थितिः(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_कुञ्चिका_आधारः(ध्रुव Cache_कुञ्चिका_आधारः &other) 
           : UObject(other), f_Creation_स्थितिः(other.f_Creation_स्थितिः), fIsPrimary(false) { }
   आभास ~Cache_कुञ्चिका_आधारः();

   /**
    * Returns the hash code for this object.
    */
   आभास int32_t hash_संहिता() ध्रुव = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   आभास Cache_कुञ्चिका_आधारः *clone() ध्रुव = 0;

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
   आभास ध्रुव SharedObject *createObject(
           ध्रुव शून्य *creationContext, UError_संहिता &स्थितिः) ध्रुव = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   आभास वर्ण *writeDescription(वर्ण *बफर, int32_t buf_आकार) ध्रुव = 0;

   मित्रम् अन्तःस्थ bool संक्रियकः==(ध्रुव Cache_कुञ्चिका_आधारः& lhs,
                                 ध्रुव Cache_कुञ्चिका_आधारः& rhs) {
       प्रत्यागम lhs.equals(rhs);
   }

   मित्रम् अन्तःस्थ bool संक्रियकः!=(ध्रुव Cache_कुञ्चिका_आधारः& lhs,
                                 ध्रुव Cache_कुञ्चिका_आधारः& rhs) {
       प्रत्यागम !lhs.equals(rhs);
   }

 रक्षित:
   आभास bool equals(ध्रुव Cache_कुञ्चिका_आधारः& other) ध्रुव = 0;

 गुप्त:
   mutable UError_संहिता f_Creation_स्थितिः;
   mutable UBool fIsPrimary;
   मित्रम् class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
प्रतिरूपम्<typename T>
class Cache_कुञ्चिका : सार्वजनिक Cache_कुञ्चिका_आधारः {
 सार्वजनिक:
   आभास ~Cache_कुञ्चिका() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   आभास int32_t hash_संहिता() ध्रुव override {
       ध्रुव वर्ण *s = typeid(T).name();
       प्रत्यागम ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   आभास वर्ण *writeDescription(वर्ण *बफर, int32_t bufLen) ध्रुव override {
       ध्रुव वर्ण *s = typeid(T).name();
       uprv_strncpy(बफर, s, bufLen);
       बफर[bufLen - 1] = 0;
       प्रत्यागम बफर;
   }

 रक्षित:
   /**
    * Two objects are equal if they are of the same type.
    */
   आभास bool equals(ध्रुव Cache_कुञ्चिका_आधारः &other) ध्रुव override {
       प्रत्यागम this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
प्रतिरूपम्<typename T>
class Locale_Cache_कुञ्चिका : सार्वजनिक Cache_कुञ्चिका<T> {
 रक्षित:
   Locale   fLoc;
   आभास bool equals(ध्रुव Cache_कुञ्चिका_आधारः &other) ध्रुव override {
       यदि (!Cache_कुञ्चिका<T>::equals(other)) {
           प्रत्यागम false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       प्रत्यागम संक्रियकः==(static_cast<ध्रुव Locale_Cache_कुञ्चिका<T> &>(other));
   }
 सार्वजनिक:
   Locale_Cache_कुञ्चिका(ध्रुव Locale &loc) : fLoc(loc) {}
   Locale_Cache_कुञ्चिका(ध्रुव Locale_Cache_कुञ्चिका<T> &other)
           : Cache_कुञ्चिका<T>(other), fLoc(other.fLoc) { }
   आभास ~Locale_Cache_कुञ्चिका() { }
   आभास int32_t hash_संहिता() ध्रुव override {
       प्रत्यागम (int32_t)(37u * (uint32_t)Cache_कुञ्चिका<T>::hash_संहिता() + (uint32_t)fLoc.hash_संहिता());
   }
   अन्तःस्थ bool संक्रियकः == (ध्रुव Locale_Cache_कुञ्चिका<T> &other) ध्रुव {
       प्रत्यागम fLoc == other.fLoc;
   }
   आभास Cache_कुञ्चिका_आधारः *clone() ध्रुव override {
       प्रत्यागम new Locale_Cache_कुञ्चिका<T>(*this);
   }
   आभास ध्रुव T *createObject(
           ध्रुव शून्य *creationContext, UError_संहिता &स्थितिः) ध्रुव override;
   /**
    * Use the locale id as the description.
    */
   आभास वर्ण *writeDescription(वर्ण *बफर, int32_t bufLen) ध्रुव override {
       ध्रुव वर्ण *s = fLoc.getName();
       uprv_strncpy(बफर, s, bufLen);
       बफर[bufLen - 1] = 0;
       प्रत्यागम बफर;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : सार्वजनिक Unified_Cache_आधारः {
 सार्वजनिक:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UError_संहिता &स्थितिः);

   /**
    * Return a pointer to the global cache instance.
    */
   स्थिर UnifiedCache *getInstance(UError_संहिता &स्थितिः);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   प्रतिरूपम्<typename T>
   शून्य get(
           ध्रुव Cache_कुञ्चिका<T>& कुञ्चिका,
           ध्रुव T *&ptr,
           UError_संहिता &स्थितिः) ध्रुव {
       get(कुञ्चिका, nullptr, ptr, स्थितिः);
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
   प्रतिरूपम्<typename T>
   शून्य get(
           ध्रुव Cache_कुञ्चिका<T>& कुञ्चिका,
           ध्रुव शून्य *creationContext,
           ध्रुव T *&ptr,
           UError_संहिता &स्थितिः) ध्रुव {
       यदि (U_FAILURE(स्थितिः)) {
           प्रत्यागम;
       }
       UError_संहिता creation_स्थितिः = U_ZERO_ERROR;
       ध्रुव SharedObject *मानम्_2 = nullptr;
       _get(कुञ्चिका, मानम्_2, creationContext, creation_स्थितिः);
       ध्रुव T *tvalue = (ध्रुव T *) मानम्_2;
       यदि (U_SUCCESS(creation_स्थितिः)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       यदि (स्थितिः == U_ZERO_ERROR || U_FAILURE(creation_स्थितिः)) {
           स्थितिः = creation_स्थितिः;
       }
   }

#यदि_परिभाषितम् UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   शून्य dumpContents() ध्रुव;
#यदि_अन्तः

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
   प्रतिरूपम्<typename T>
   स्थिर शून्य getByLocale(
           ध्रुव Locale &loc, ध्रुव T *&ptr, UError_संहिता &स्थितिः) {
       ध्रुव UnifiedCache *cache = getInstance(स्थितिः);
       यदि (U_FAILURE(स्थितिः)) {
           प्रत्यागम;
       }
       cache->get(Locale_Cache_कुञ्चिका<T>(loc), ptr, स्थितिः);
   }

#यदि_परिभाषितम् UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   स्थिर शून्य dump();
#यदि_अन्तः

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t कुञ्चिका_गणनम्() ध्रुव;

   /**
    * Removes any values from cache that are not referenced outside
    * the cache.
    */
   शून्य flush() ध्रुव;

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
   शून्य समुच्चयः_Eviction_Policy(
           int32_t गणनम्, int32_t percentage_सम्बन्धि_अन्तर्_Use_वस्तूनि, UError_संहिता &स्थितिः);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_गणनम्() ध्रुव;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t अप्रयुक्तम्_गणनम्() ध्रुव;

   आभास शून्य handleUnreferencedObject() ध्रुव override;
   आभास ~UnifiedCache();
   
 गुप्त:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_अङ्कः_मानानि_योग;
   mutable int32_t f_अङ्कः_मानानि_अन्तर्_Use;
   int32_t f_Max_अप्रयुक्तम्;
   int32_t f_Max_Percentage_सम्बन्धि_अन्तर्_Use;
   mutable int64_t f_Auto_Evicted_गणनम्;
   SharedObject *f_No_मानम्;
   
   UnifiedCache(ध्रुव UnifiedCache &other) = delete;
   UnifiedCache &संक्रियकः=(ध्रुव UnifiedCache &other) = delete;
   
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
   UBool _flush(UBool all) ध्रुव;
   
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
   शून्य _get(
           ध्रुव Cache_कुञ्चिका_आधारः &कुञ्चिका,
           ध्रुव SharedObject *&मानम्_2,
           ध्रुव शून्य *creationContext,
           UError_संहिता &स्थितिः) ध्रुव;

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
            ध्रुव Cache_कुञ्चिका_आधारः &कुञ्चिका,
            ध्रुव SharedObject *&मानम्_2,
            UError_संहिता &स्थितिः) ध्रुव;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    शून्य _putNew(
        ध्रुव Cache_कुञ्चिका_आधारः &कुञ्चिका,
        ध्रुव SharedObject *मानम्_2,
        ध्रुव UError_संहिता creation_स्थितिः,
        UError_संहिता &स्थितिः) ध्रुव;
           
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
   शून्य _putIfAbsentAndGet(
           ध्रुव Cache_कुञ्चिका_आधारः &कुञ्चिका,
           ध्रुव SharedObject *&मानम्_2,
           UError_संहिता &स्थितिः) ध्रुव;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    ध्रुव UHashElement *अग्र_Element_2() ध्रुव;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_गणनम्_सम्बन्धि_वस्तूनि_गन्तव्यम्_Evict() ध्रुव;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   शून्य सततम्_Eviction_खण्डिका() ध्रुव;
 
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
   शून्य पञ्जिकाकोशः_Primary(ध्रुव Cache_कुञ्चिका_आधारः *the_कुञ्चिका, ध्रुव SharedObject *मानम्_2) ध्रुव;
        
   /**
    * Store a value and creation error status in given hash entry.
    * On entry, gCacheMutex must be held. Hash entry element must be in progress.
    * value must be non nullptr.
    * On Exit, soft reference added to value. value and status stored in hash
    * entry. Soft reference removed from previous stored value. Waiting
    * threads notified.
    */
   शून्य _put(
           ध्रुव UHashElement *element,
           ध्रुव SharedObject *मानम्_2,
           ध्रुव UError_संहिता स्थितिः) ध्रुव;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   शून्य removeSoftRef(ध्रुव SharedObject *मानम्_2) ध्रुव;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t सङ्कलय_Hard_Ref(ध्रुव SharedObject *मानम्_2) ध्रुव;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(ध्रुव SharedObject *मानम्_2) ध्रुव;

   
#यदि_परिभाषितम् UNIFIED_CACHE_DEBUG
   शून्य _dumpContents() ध्रुव;
#यदि_अन्तः
   
   /**
    *  Fetch value and error code from a particular hash entry.
    *  On entry, gCacheMutex must be held. value must be either nullptr or must be
    *  included in the ref count of the object to which it points.
    *  On exit, value and status set to what is in the hash entry. Caller must
    *  eventually call removeRef on value.
    *  If hash entry is in progress, value will be set to gNoValue and status will
    *  be set to U_ZERO_ERROR.
    */
   शून्य _fetch(ध्रुव UHashElement *element, ध्रुव SharedObject *&मानम्_2,
                       UError_संहिता &स्थितिः) ध्रुव;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool अन्तर्_प्रगतिः(ध्रुव UHashElement *element) ध्रुव;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool अन्तर्_प्रगतिः(ध्रुव SharedObject *the_मानम्, UError_संहिता creation_स्थितिः) ध्रुव;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(ध्रुव UHashElement *element) ध्रुव;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः

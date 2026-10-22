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

#اگر_تعریف_نشده __UNIFIED_CACHE_H__
#تعریف_کن __UNIFIED_CACHE_H__

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
class U_COMMON_API Cache_کلید_پایه : عام UObject {
 عام:
   Cache_کلید_پایه() : f_Creation_وضعیت(U_ZERO_ERROR), fIsPrimary(false) {}

   /**
    * Copy constructor. Needed to support cloning.
    */
   Cache_کلید_پایه(ثابت Cache_کلید_پایه &other) 
           : UObject(other), f_Creation_وضعیت(other.f_Creation_وضعیت), fIsPrimary(false) { }
   مجازی ~Cache_کلید_پایه();

   /**
    * Returns the hash code for this object.
    */
   مجازی int32_t hash_کد() ثابت = 0;

   /**
    * Clones this object polymorphically. Caller owns returned value.
    */
   مجازی Cache_کلید_پایه *clone() ثابت = 0;

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
   مجازی ثابت SharedObject *createObject(
           ثابت فراغ *creationContext, UError_کد &وضعیت) ثابت = 0;

   /**
    * Writes a description of this key to buffer and returns buffer. Written
    * description is nullptr terminated.
    */
   مجازی حرف *writeDescription(حرف *میانگیر, int32_t buf_اندازه) ثابت = 0;

   دوست مضمن bool عملگر==(ثابت Cache_کلید_پایه& lhs,
                                 ثابت Cache_کلید_پایه& rhs) {
       أرجع lhs.equals(rhs);
   }

   دوست مضمن bool عملگر!=(ثابت Cache_کلید_پایه& lhs,
                                 ثابت Cache_کلید_پایه& rhs) {
       أرجع !lhs.equals(rhs);
   }

 محمي:
   مجازی bool equals(ثابت Cache_کلید_پایه& other) ثابت = 0;

 خاص:
   mutable UError_کد f_Creation_وضعیت;
   mutable UBool fIsPrimary;
   دوست class UnifiedCache;
};



/**
 * Templated version of CacheKeyBase. 
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
قالب<typename T>
class Cache_کلید : عام Cache_کلید_پایه {
 عام:
   مجازی ~Cache_کلید() { }
   /**
    * The template parameter, T, determines the hash code returned.
    */
   مجازی int32_t hash_کد() ثابت override {
       ثابت حرف *s = typeid(T).name();
       أرجع ustr_hashCharsN(s, static_cast<int32_t>(uprv_strlen(s)));
   }

   /**
    * Use the value type, T,  as the description.
    */
   مجازی حرف *writeDescription(حرف *میانگیر, int32_t bufLen) ثابت override {
       ثابت حرف *s = typeid(T).name();
       uprv_strncpy(میانگیر, s, bufLen);
       میانگیر[bufLen - 1] = 0;
       أرجع میانگیر;
   }

 محمي:
   /**
    * Two objects are equal if they are of the same type.
    */
   مجازی bool equals(ثابت Cache_کلید_پایه &other) ثابت override {
       أرجع this == &other || typeid(*this) == typeid(other);
   }
};

/**
 * Cache key based on locale.
 * A key of type LocaleCacheKey<T> maps to a value of type T.
 */
قالب<typename T>
class Locale_Cache_کلید : عام Cache_کلید<T> {
 محمي:
   Locale   fLoc;
   مجازی bool equals(ثابت Cache_کلید_پایه &other) ثابت override {
       إذا (!Cache_کلید<T>::equals(other)) {
           أرجع false;
       }
       // We know this and other are of same class because equals() on
       // CacheKey returned true.
       أرجع عملگر==(static_cast<ثابت Locale_Cache_کلید<T> &>(other));
   }
 عام:
   Locale_Cache_کلید(ثابت Locale &loc) : fLoc(loc) {}
   Locale_Cache_کلید(ثابت Locale_Cache_کلید<T> &other)
           : Cache_کلید<T>(other), fLoc(other.fLoc) { }
   مجازی ~Locale_Cache_کلید() { }
   مجازی int32_t hash_کد() ثابت override {
       أرجع (int32_t)(37u * (uint32_t)Cache_کلید<T>::hash_کد() + (uint32_t)fLoc.hash_کد());
   }
   مضمن bool عملگر == (ثابت Locale_Cache_کلید<T> &other) ثابت {
       أرجع fLoc == other.fLoc;
   }
   مجازی Cache_کلید_پایه *clone() ثابت override {
       أرجع new Locale_Cache_کلید<T>(*this);
   }
   مجازی ثابت T *createObject(
           ثابت فراغ *creationContext, UError_کد &وضعیت) ثابت override;
   /**
    * Use the locale id as the description.
    */
   مجازی حرف *writeDescription(حرف *میانگیر, int32_t bufLen) ثابت override {
       ثابت حرف *s = fLoc.getName();
       uprv_strncpy(میانگیر, s, bufLen);
       میانگیر[bufLen - 1] = 0;
       أرجع میانگیر;
   }

};

/**
 * The unified cache. A singleton type.
 * Design doc here:
 * https://docs.google.com/document/d/1RwGQJs4N4tawNbf809iYDRCvXoMKqDJihxzYt1ysmd8/edit?usp=sharing
 */
class U_COMMON_API UnifiedCache : عام Unified_Cache_پایه {
 عام:
   /**
    * @internal
    * Do not call directly. Instead use UnifiedCache::getInstance() as
    * there should be only one UnifiedCache in an application.
    */
   UnifiedCache(UError_کد &وضعیت);

   /**
    * Return a pointer to the global cache instance.
    */
   ساكن UnifiedCache *getInstance(UError_کد &وضعیت);

   /**
    * Fetches a value from the cache by key. Equivalent to
    * get(key, nullptr, ptr, status);
    */
   قالب<typename T>
   فراغ get(
           ثابت Cache_کلید<T>& کلید,
           ثابت T *&ptr,
           UError_کد &وضعیت) ثابت {
       get(کلید, nullptr, ptr, وضعیت);
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
   قالب<typename T>
   فراغ get(
           ثابت Cache_کلید<T>& کلید,
           ثابت فراغ *creationContext,
           ثابت T *&ptr,
           UError_کد &وضعیت) ثابت {
       إذا (U_FAILURE(وضعیت)) {
           أرجع;
       }
       UError_کد creation_وضعیت = U_ZERO_ERROR;
       ثابت SharedObject *مقدار_2 = nullptr;
       _get(کلید, مقدار_2, creationContext, creation_وضعیت);
       ثابت T *tvalue = (ثابت T *) مقدار_2;
       إذا (U_SUCCESS(creation_وضعیت)) {
           SharedObject::copyPtr(tvalue, ptr);
       }
       SharedObject::clearPtr(tvalue);
       // Take care not to overwrite a warning status passed in with
       // another warning or U_ZERO_ERROR.
       إذا (وضعیت == U_ZERO_ERROR || U_FAILURE(creation_وضعیت)) {
           وضعیت = creation_وضعیت;
       }
   }

#اگر_تعریف_شده UNIFIED_CACHE_DEBUG
   /**
    * Dumps the contents of this cache to standard error. Used for testing of
    * cache only.
    */
   فراغ dumpContents() ثابت;
#پایان_اگر

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
   قالب<typename T>
   ساكن فراغ getByLocale(
           ثابت Locale &loc, ثابت T *&ptr, UError_کد &وضعیت) {
       ثابت UnifiedCache *cache = getInstance(وضعیت);
       إذا (U_FAILURE(وضعیت)) {
           أرجع;
       }
       cache->get(Locale_Cache_کلید<T>(loc), ptr, وضعیت);
   }

#اگر_تعریف_شده UNIFIED_CACHE_DEBUG
   /**
    * Dumps the cache contents to stderr. For testing only.
    */
   ساكن فراغ dump();
#پایان_اگر

   /**
    * Returns the number of keys in this cache. For testing only.
    */
   int32_t کلید_شمار() ثابت;

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
   فراغ مجموعه_Eviction_Policy(
           int32_t شمار, int32_t percentage_از_درون_Use_اقلام, UError_کد &وضعیت);


   /**
    * Returns how many entries have been auto evicted during the lifetime
    * of this cache. This only includes auto evicted entries, not
    * entries evicted because of a call to flush().
    */
   int64_t auto_Evicted_شمار() ثابت;

   /**
    * Returns the unused entry count in this cache. For testing only,
    * Regular clients will not need this.
    */
   int32_t استفادهنشده_شمار() ثابت;

   مجازی فراغ handleUnreferencedObject() ثابت override;
   مجازی ~UnifiedCache();
   
 خاص:
   UHashtable *fHashtable;
   mutable int32_t fEvictPos;
   mutable int32_t f_رقم_مقادیر_مجموع;
   mutable int32_t f_رقم_مقادیر_درون_Use;
   int32_t f_Max_استفادهنشده;
   int32_t f_Max_Percentage_از_درون_Use;
   mutable int64_t f_Auto_Evicted_شمار;
   SharedObject *f_No_مقدار;
   
   UnifiedCache(ثابت UnifiedCache &other) = delete;
   UnifiedCache &عملگر=(ثابت UnifiedCache &other) = delete;
   
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
           ثابت Cache_کلید_پایه &کلید,
           ثابت SharedObject *&مقدار_2,
           ثابت فراغ *creationContext,
           UError_کد &وضعیت) ثابت;

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
            ثابت Cache_کلید_پایه &کلید,
            ثابت SharedObject *&مقدار_2,
            UError_کد &وضعیت) ثابت;
    
    /**
     * Places a new value and creationStatus in the cache for the given key.
     * On entry, gCacheMutex must be held. key must not exist in the cache. 
     * On exit, value and creation status placed under key. Soft reference added
     * to value on successful add. On error sets status.
     */
    فراغ _putNew(
        ثابت Cache_کلید_پایه &کلید,
        ثابت SharedObject *مقدار_2,
        ثابت UError_کد creation_وضعیت,
        UError_کد &وضعیت) ثابت;
           
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
           ثابت Cache_کلید_پایه &کلید,
           ثابت SharedObject *&مقدار_2,
           UError_کد &وضعیت) ثابت;

    /**
     * Returns the next element in the cache round robin style.
     * Returns nullptr if the cache is empty.
     * On entry, gCacheMutex must be held.
     */
    ثابت UHashElement *بعدی_Element_2() ثابت;
   
   /**
    * Return the number of cache items that would need to be evicted
    * to bring usage into conformance with eviction policy.
    * 
    * An item corresponds to an entry in the hash table, a hash table element.
    * 
    * On entry, gCacheMutex must be held.
    */
   int32_t compute_شمار_از_اقلام_مقصد_Evict() ثابت;
   
   /**
    * Run an eviction slice.
    * On entry, gCacheMutex must be held.
    * _runEvictionSlice runs a slice of the evict pipeline by examining the next
    * 10 entries in the cache round robin style evicting them if they are eligible.
    */
   فراغ پیاپی_Eviction_برش() ثابت;
 
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
   فراغ ثبات_Primary(ثابت Cache_کلید_پایه *the_کلید, ثابت SharedObject *مقدار_2) ثابت;
        
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
           ثابت SharedObject *مقدار_2,
           ثابت UError_کد وضعیت) ثابت;
    /**
     * Remove a soft reference, and delete the SharedObject if no references remain.
     * To be used from within the UnifiedCache implementation only.
     * gCacheMutex must be held by caller.
     * @param value the SharedObject to be acted on.
     */
   فراغ removeSoftRef(ثابت SharedObject *مقدار_2) ثابت;
   
   /**
    * Increment the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between zero and one reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the addition.
    */
   int32_t جمع_کن_Hard_Ref(ثابت SharedObject *مقدار_2) ثابت;
   
  /**
    * Decrement the hard reference count of the given SharedObject.
    * gCacheMutex must be held by the caller.
    * Update numValuesEvictable on transitions between one and zero reference.
    * 
    * @param value The SharedObject to be referenced.
    * @return the hard reference count after the removal.
    */
   int32_t removeHardRef(ثابت SharedObject *مقدار_2) ثابت;

   
#اگر_تعریف_شده UNIFIED_CACHE_DEBUG
   فراغ _dumpContents() ثابت;
#پایان_اگر
   
   /**
    *  Fetch value and error code from a particular hash entry.
    *  On entry, gCacheMutex must be held. value must be either nullptr or must be
    *  included in the ref count of the object to which it points.
    *  On exit, value and status set to what is in the hash entry. Caller must
    *  eventually call removeRef on value.
    *  If hash entry is in progress, value will be set to gNoValue and status will
    *  be set to U_ZERO_ERROR.
    */
   فراغ _fetch(ثابت UHashElement *element, ثابت SharedObject *&مقدار_2,
                       UError_کد &وضعیت) ثابت;
                       
    /**
     * Determine if given hash entry is in progress.
     * On entry, gCacheMutex must be held.
     */
   UBool درون_پیشرفت(ثابت UHashElement *element) ثابت;
   
   /**
    * Determine if given hash entry is in progress.
    * On entry, gCacheMutex must be held.
    */
   UBool درون_پیشرفت(ثابت SharedObject *the_مقدار, UError_کد creation_وضعیت) ثابت;
   
   /**
    * Determine if given hash entry is eligible for eviction.
    * On entry, gCacheMutex must be held.
    */
   UBool _isEvictable(ثابت UHashElement *element) ثابت;
};

U_NAMESPACE_انتها

#پایان_اگر

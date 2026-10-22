// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.h
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __SHAREDOBJECT_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __SHAREDOBJECT_H__


#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"

U_NAMESPACE_BEGIN

class SharedObject;

/**
 * Base class for unified cache exposing enough methods to SharedObject
 * instances to allow their addRef() and removeRef() methods to
 * update cache metrics. No other part of ICU, except for SharedObject,
 * should directly call the methods of this base class.
 */
class U_COMMON_API Unified_Cache_ਅਧਾਰ : ਜਨਤਕ UObject {
ਜਨਤਕ:
    Unified_Cache_ਅਧਾਰ() { }

    /**
     * Notify the cache implementation that an object was seen transitioning to
     * zero hard references. The cache may use this to keep track the number of
     * unreferenced SharedObjects, and to trigger evictions.
     */
    ਆਭਾਸੀ ਖਾਲੀ handleUnreferencedObject() ਅਚਲ = 0;

    ਆਭਾਸੀ ~Unified_Cache_ਅਧਾਰ();
ਨਿਜੀ:
    Unified_Cache_ਅਧਾਰ(ਅਚਲ Unified_Cache_ਅਧਾਰ &) = delete;
    Unified_Cache_ਅਧਾਰ &ਸੰਚਾਲਕ=(ਅਚਲ Unified_Cache_ਅਧਾਰ &) = delete;
};

/**
 * Base class for shared, reference-counted, auto-deleted objects.
 * Subclasses can be immutable.
 * If they are mutable, then they must implement their copy constructor
 * so that copyOnWrite() works.
 *
 * Either stack-allocate, use LocalPointer, or use addRef()/removeRef().
 * Sharing requires reference-counting.
 */
class U_COMMON_API_CLASS SharedObject : ਜਨਤਕ UObject {
ਜਨਤਕ:
    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject() :
            soft_Ref_ਗਿਣਤੀ(0),
            hard_Ref_ਗਿਣਤੀ(0),
            cachePtr(nullptr) {}

    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject(ਅਚਲ SharedObject &other) :
            UObject(other),
            soft_Ref_ਗਿਣਤੀ(0),
            hard_Ref_ਗਿਣਤੀ(0),
            cachePtr(nullptr) {}

    U_COMMON_API ਆਭਾਸੀ ~SharedObject();

    /**
     * Increments the number of hard references to this object. Thread-safe.
     * Not for use from within the Unified Cache implementation.
     */
    U_COMMON_API ਖਾਲੀ ਜੋੜੋ_Ref() ਅਚਲ;

    /**
     * Decrements the number of hard references to this object, and
     * arrange for possible cache-eviction and/or deletion if ref
     * count goes to zero. Thread-safe.
     * 
     * Not for use from within the UnifiedCache implementation.
     */
    U_COMMON_API ਖਾਲੀ removeRef() ਅਚਲ;

    /**
     * Returns the number of hard references for this object.
     * Uses a memory barrier.
     */
    U_COMMON_API int32_t get_Ref_ਗਿਣਤੀ() ਅਚਲ;

    /**
     * If noHardReferences() == true then this object has no hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API ਸਤਰਅੰਦਰ UBool noHardReferences() ਅਚਲ { ਵਾਪਸ get_Ref_ਗਿਣਤੀ() == 0; }

    /**
     * If hasHardReferences() == true then this object has hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API ਸਤਰਅੰਦਰ UBool hasHardReferences() ਅਚਲ { ਵਾਪਸ get_Ref_ਗਿਣਤੀ() != 0; }

    /**
     * Deletes this object if it has no references.
     * Available for non-cached SharedObjects only. Ownership of cached objects
     * is with the UnifiedCache, which is solely responsible for eviction and deletion.
     */
    U_COMMON_API ਖਾਲੀ delete_If_Zero_Ref_ਗਿਣਤੀ() ਅਚਲ;

        
    /**
     * Returns a writable version of ptr.
     * If there is exactly one owner, then ptr itself is returned as a
     *  non-const pointer.
     * If there are multiple owners, then ptr is replaced with a 
     * copy-constructed clone,
     * and that is returned.
     * Returns nullptr if cloning failed.
     *
     * T must be a subclass of SharedObject.
     */
    ਸਾਂਚਾ<typename T>
    ਸਥਿਰ T *copyOnWrite(ਅਚਲ T *&ptr) {
        ਅਚਲ T *p = ptr;
        ਜੇ(p->get_Ref_ਗਿਣਤੀ() <= 1) { ਵਾਪਸ const_cast<T *>(p); }
        T *p2 = new T(*p);
        ਜੇ(p2 == nullptr) { ਵਾਪਸ nullptr; }
        p->removeRef();
        ptr = p2;
        p2->ਜੋੜੋ_Ref();
        ਵਾਪਸ p2;
    }

    /**
     * Makes dest an owner of the object pointed to by src while adjusting
     * reference counts and deleting the previous object dest pointed to
     * if necessary. Before this call is made, dest must either be nullptr or
     * be included in the reference count of the object it points to. 
     *
     * T must be a subclass of SharedObject.
     */
    ਸਾਂਚਾ<typename T>
    ਸਥਿਰ ਖਾਲੀ copyPtr(ਅਚਲ T *src, ਅਚਲ T *&dest) {
        ਜੇ(src != dest) {
            ਜੇ(dest != nullptr) { dest->removeRef(); }
            dest = src;
            ਜੇ(src != nullptr) { src->ਜੋੜੋ_Ref(); }
        }
    }

    /**
     * Equivalent to copyPtr(nullptr, dest).
     */
    ਸਾਂਚਾ<typename T>
    ਸਥਿਰ ਖਾਲੀ clearPtr(ਅਚਲ T *&ptr) {
        ਜੇ (ptr != nullptr) {
            ptr->removeRef();
            ptr = nullptr;
        }
    }

ਨਿਜੀ:
    /**
     * The number of references from the UnifiedCache, which is
     * the number of times that the sharedObject is stored as a hash table value.
     * For use by UnifiedCache implementation code only.
     * All access is synchronized by UnifiedCache's gCacheMutex
     */
    mutable int32_t soft_Ref_ਗਿਣਤੀ;
    ਮਿੱਤਰ class UnifiedCache;

    /**
     * Reference count, excluding references from within the UnifiedCache implementation.
     */
    mutable u_atomic_int32_t hard_Ref_ਗਿਣਤੀ;
    
    mutable ਅਚਲ Unified_Cache_ਅਧਾਰ *cachePtr;

};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ

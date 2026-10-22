// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.h
*/

#ifndef __SHAREDOBJECT_H__
#define __SHAREDOBJECT_H__


#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "umutex.h"

U_NAMESPACE_BEGIN

class SharedObject;

/**
 * Base class for unified cache exposing enough methods to SharedObject
 * instances to allow their addRef() and removeRef() methods to
 * update cache metrics. No other part of ICU, except for SharedObject,
 * should directly call the methods of this base class.
 */
class U_COMMON_API UnifiedCacheBase : જાહેર UObject {
જાહેર:
    UnifiedCacheBase() { }

    /**
     * Notify the cache implementation that an object was seen transitioning to
     * zero hard references. The cache may use this to keep track the number of
     * unreferenced SharedObjects, and to trigger evictions.
     */
    આભાસી શૂન્ય handleUnreferencedObject() અચલ = 0;

    આભાસી ~UnifiedCacheBase();
ખાનગી:
    UnifiedCacheBase(અચલ UnifiedCacheBase &) = delete;
    UnifiedCacheBase &operator=(અચલ UnifiedCacheBase &) = delete;
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
class U_COMMON_API_CLASS SharedObject : જાહેર UObject {
જાહેર:
    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject() :
            soft_Ref_ગણતરી(0),
            hard_Ref_ગણતરી(0),
            cachePtr(nullptr) {}

    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject(અચલ SharedObject &other) :
            UObject(other),
            soft_Ref_ગણતરી(0),
            hard_Ref_ગણતરી(0),
            cachePtr(nullptr) {}

    U_COMMON_API આભાસી ~SharedObject();

    /**
     * Increments the number of hard references to this object. Thread-safe.
     * Not for use from within the Unified Cache implementation.
     */
    U_COMMON_API શૂન્ય addRef() અચલ;

    /**
     * Decrements the number of hard references to this object, and
     * arrange for possible cache-eviction and/or deletion if ref
     * count goes to zero. Thread-safe.
     * 
     * Not for use from within the UnifiedCache implementation.
     */
    U_COMMON_API શૂન્ય removeRef() અચલ;

    /**
     * Returns the number of hard references for this object.
     * Uses a memory barrier.
     */
    U_COMMON_API int32_t get_Ref_ગણતરી() અચલ;

    /**
     * If noHardReferences() == true then this object has no hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API અંતઃરેખ UBool noHardReferences() અચલ { પાછા get_Ref_ગણતરી() == 0; }

    /**
     * If hasHardReferences() == true then this object has hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API અંતઃરેખ UBool hasHardReferences() અચલ { પાછા get_Ref_ગણતરી() != 0; }

    /**
     * Deletes this object if it has no references.
     * Available for non-cached SharedObjects only. Ownership of cached objects
     * is with the UnifiedCache, which is solely responsible for eviction and deletion.
     */
    U_COMMON_API શૂન્ય delete_If_Zero_Ref_ગણતરી() અચલ;

        
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
    template<typename T>
    સ્થિર T *copyOnWrite(અચલ T *&ptr) {
        અચલ T *p = ptr;
        જો(p->get_Ref_ગણતરી() <= 1) { પાછા const_cast<T *>(p); }
        T *p2 = new T(*p);
        જો(p2 == nullptr) { પાછા nullptr; }
        p->removeRef();
        ptr = p2;
        p2->addRef();
        પાછા p2;
    }

    /**
     * Makes dest an owner of the object pointed to by src while adjusting
     * reference counts and deleting the previous object dest pointed to
     * if necessary. Before this call is made, dest must either be nullptr or
     * be included in the reference count of the object it points to. 
     *
     * T must be a subclass of SharedObject.
     */
    template<typename T>
    સ્થિર શૂન્ય copyPtr(અચલ T *src, અચલ T *&dest) {
        જો(src != dest) {
            જો(dest != nullptr) { dest->removeRef(); }
            dest = src;
            જો(src != nullptr) { src->addRef(); }
        }
    }

    /**
     * Equivalent to copyPtr(nullptr, dest).
     */
    template<typename T>
    સ્થિર શૂન્ય clearPtr(અચલ T *&ptr) {
        જો (ptr != nullptr) {
            ptr->removeRef();
            ptr = nullptr;
        }
    }

ખાનગી:
    /**
     * The number of references from the UnifiedCache, which is
     * the number of times that the sharedObject is stored as a hash table value.
     * For use by UnifiedCache implementation code only.
     * All access is synchronized by UnifiedCache's gCacheMutex
     */
    mutable int32_t soft_Ref_ગણતરી;
    friend class UnifiedCache;

    /**
     * Reference count, excluding references from within the UnifiedCache implementation.
     */
    mutable u_atomic_int32_t hard_Ref_ગણતરી;
    
    mutable અચલ UnifiedCacheBase *cachePtr;

};

U_NAMESPACE_END

#endif

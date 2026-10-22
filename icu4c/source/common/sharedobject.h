// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.h
*/

#నిర్వచితం_కాకపోతే __SHAREDOBJECT_H__
#నిర్వచించు __SHAREDOBJECT_H__


#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "umutex.h"

U_NAMESPACE_BEGIN

class SharedObject;

/**
 * Base class for unified cache exposing enough methods to SharedObject
 * instances to allow their addRef() and removeRef() methods to
 * update cache metrics. No other part of ICU, except for SharedObject,
 * should directly call the methods of this base class.
 */
class U_COMMON_API Unified_Cache_పునాది : బహిరంగ UObject {
బహిరంగ:
    Unified_Cache_పునాది() { }

    /**
     * Notify the cache implementation that an object was seen transitioning to
     * zero hard references. The cache may use this to keep track the number of
     * unreferenced SharedObjects, and to trigger evictions.
     */
    మాయా శూన్య handleUnreferencedObject() స్థిరరాశి = 0;

    మాయా ~Unified_Cache_పునాది();
అంతరంగ:
    Unified_Cache_పునాది(స్థిరరాశి Unified_Cache_పునాది &) = delete;
    Unified_Cache_పునాది &క్రియాకారకం=(స్థిరరాశి Unified_Cache_పునాది &) = delete;
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
class U_COMMON_API_CLASS SharedObject : బహిరంగ UObject {
బహిరంగ:
    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject() :
            soft_Ref_లెక్క(0),
            hard_Ref_లెక్క(0),
            cachePtr(nullptr) {}

    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject(స్థిరరాశి SharedObject &other) :
            UObject(other),
            soft_Ref_లెక్క(0),
            hard_Ref_లెక్క(0),
            cachePtr(nullptr) {}

    U_COMMON_API మాయా ~SharedObject();

    /**
     * Increments the number of hard references to this object. Thread-safe.
     * Not for use from within the Unified Cache implementation.
     */
    U_COMMON_API శూన్య కూడు_Ref() స్థిరరాశి;

    /**
     * Decrements the number of hard references to this object, and
     * arrange for possible cache-eviction and/or deletion if ref
     * count goes to zero. Thread-safe.
     * 
     * Not for use from within the UnifiedCache implementation.
     */
    U_COMMON_API శూన్య removeRef() స్థిరరాశి;

    /**
     * Returns the number of hard references for this object.
     * Uses a memory barrier.
     */
    U_COMMON_API int32_t get_Ref_లెక్క() స్థిరరాశి;

    /**
     * If noHardReferences() == true then this object has no hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API అంతఃవరుస UBool noHardReferences() స్థిరరాశి { తిరిగి get_Ref_లెక్క() == 0; }

    /**
     * If hasHardReferences() == true then this object has hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API అంతఃవరుస UBool hasHardReferences() స్థిరరాశి { తిరిగి get_Ref_లెక్క() != 0; }

    /**
     * Deletes this object if it has no references.
     * Available for non-cached SharedObjects only. Ownership of cached objects
     * is with the UnifiedCache, which is solely responsible for eviction and deletion.
     */
    U_COMMON_API శూన్య delete_If_Zero_Ref_లెక్క() స్థిరరాశి;

        
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
    మూస<typename T>
    స్థిర T *copyOnWrite(స్థిరరాశి T *&ptr) {
        స్థిరరాశి T *p = ptr;
        ఒకవేళ(p->get_Ref_లెక్క() <= 1) { తిరిగి const_cast<T *>(p); }
        T *p2 = new T(*p);
        ఒకవేళ(p2 == nullptr) { తిరిగి nullptr; }
        p->removeRef();
        ptr = p2;
        p2->కూడు_Ref();
        తిరిగి p2;
    }

    /**
     * Makes dest an owner of the object pointed to by src while adjusting
     * reference counts and deleting the previous object dest pointed to
     * if necessary. Before this call is made, dest must either be nullptr or
     * be included in the reference count of the object it points to. 
     *
     * T must be a subclass of SharedObject.
     */
    మూస<typename T>
    స్థిర శూన్య copyPtr(స్థిరరాశి T *src, స్థిరరాశి T *&dest) {
        ఒకవేళ(src != dest) {
            ఒకవేళ(dest != nullptr) { dest->removeRef(); }
            dest = src;
            ఒకవేళ(src != nullptr) { src->కూడు_Ref(); }
        }
    }

    /**
     * Equivalent to copyPtr(nullptr, dest).
     */
    మూస<typename T>
    స్థిర శూన్య clearPtr(స్థిరరాశి T *&ptr) {
        ఒకవేళ (ptr != nullptr) {
            ptr->removeRef();
            ptr = nullptr;
        }
    }

అంతరంగ:
    /**
     * The number of references from the UnifiedCache, which is
     * the number of times that the sharedObject is stored as a hash table value.
     * For use by UnifiedCache implementation code only.
     * All access is synchronized by UnifiedCache's gCacheMutex
     */
    mutable int32_t soft_Ref_లెక్క;
    మిత్ర class UnifiedCache;

    /**
     * Reference count, excluding references from within the UnifiedCache implementation.
     */
    mutable u_atomic_int32_t hard_Ref_లెక్క;
    
    mutable స్థిరరాశి Unified_Cache_పునాది *cachePtr;

};

U_NAMESPACE_చివర

#అయితే_ముగింపు

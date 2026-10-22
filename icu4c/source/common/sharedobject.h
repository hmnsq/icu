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


#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "umutex.h"

U_NAMESPACE_BEGIN

class SharedObject;

/**
 * Base class for unified cache exposing enough methods to SharedObject
 * instances to allow their addRef() and removeRef() methods to
 * update cache metrics. No other part of ICU, except for SharedObject,
 * should directly call the methods of this base class.
 */
class U_COMMON_API UnifiedCacheBase : सार्वजनिक UObject {
सार्वजनिक:
    UnifiedCacheBase() { }

    /**
     * Notify the cache implementation that an object was seen transitioning to
     * zero hard references. The cache may use this to keep track the number of
     * unreferenced SharedObjects, and to trigger evictions.
     */
    आभास शून्य handleUnreferencedObject() ध्रुव = 0;

    आभास ~UnifiedCacheBase();
गुप्त:
    UnifiedCacheBase(ध्रुव UnifiedCacheBase &) = delete;
    UnifiedCacheBase &operator=(ध्रुव UnifiedCacheBase &) = delete;
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
class U_COMMON_API_CLASS SharedObject : सार्वजनिक UObject {
सार्वजनिक:
    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject() :
            soft_Ref_गणनम्(0),
            hard_Ref_गणनम्(0),
            cachePtr(nullptr) {}

    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject(ध्रुव SharedObject &other) :
            UObject(other),
            soft_Ref_गणनम्(0),
            hard_Ref_गणनम्(0),
            cachePtr(nullptr) {}

    U_COMMON_API आभास ~SharedObject();

    /**
     * Increments the number of hard references to this object. Thread-safe.
     * Not for use from within the Unified Cache implementation.
     */
    U_COMMON_API शून्य addRef() ध्रुव;

    /**
     * Decrements the number of hard references to this object, and
     * arrange for possible cache-eviction and/or deletion if ref
     * count goes to zero. Thread-safe.
     * 
     * Not for use from within the UnifiedCache implementation.
     */
    U_COMMON_API शून्य removeRef() ध्रुव;

    /**
     * Returns the number of hard references for this object.
     * Uses a memory barrier.
     */
    U_COMMON_API int32_t get_Ref_गणनम्() ध्रुव;

    /**
     * If noHardReferences() == true then this object has no hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API अन्तःस्थ UBool noHardReferences() ध्रुव { प्रत्यागम get_Ref_गणनम्() == 0; }

    /**
     * If hasHardReferences() == true then this object has hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API अन्तःस्थ UBool hasHardReferences() ध्रुव { प्रत्यागम get_Ref_गणनम्() != 0; }

    /**
     * Deletes this object if it has no references.
     * Available for non-cached SharedObjects only. Ownership of cached objects
     * is with the UnifiedCache, which is solely responsible for eviction and deletion.
     */
    U_COMMON_API शून्य delete_If_Zero_Ref_गणनम्() ध्रुव;

        
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
    स्थिर T *copyOnWrite(ध्रुव T *&ptr) {
        ध्रुव T *p = ptr;
        जइ(p->get_Ref_गणनम्() <= 1) { प्रत्यागम const_cast<T *>(p); }
        T *p2 = new T(*p);
        जइ(p2 == nullptr) { प्रत्यागम nullptr; }
        p->removeRef();
        ptr = p2;
        p2->addRef();
        प्रत्यागम p2;
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
    स्थिर शून्य copyPtr(ध्रुव T *src, ध्रुव T *&dest) {
        जइ(src != dest) {
            जइ(dest != nullptr) { dest->removeRef(); }
            dest = src;
            जइ(src != nullptr) { src->addRef(); }
        }
    }

    /**
     * Equivalent to copyPtr(nullptr, dest).
     */
    template<typename T>
    स्थिर शून्य clearPtr(ध्रुव T *&ptr) {
        जइ (ptr != nullptr) {
            ptr->removeRef();
            ptr = nullptr;
        }
    }

गुप्त:
    /**
     * The number of references from the UnifiedCache, which is
     * the number of times that the sharedObject is stored as a hash table value.
     * For use by UnifiedCache implementation code only.
     * All access is synchronized by UnifiedCache's gCacheMutex
     */
    mutable int32_t soft_Ref_गणनम्;
    friend class UnifiedCache;

    /**
     * Reference count, excluding references from within the UnifiedCache implementation.
     */
    mutable u_atomic_int32_t hard_Ref_गणनम्;
    
    mutable ध्रुव UnifiedCacheBase *cachePtr;

};

U_NAMESPACE_END

#endif

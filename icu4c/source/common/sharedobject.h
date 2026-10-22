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


#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "umutex.h"

U_NAMESPACE_BEGIN

class SharedObject;

/**
 * Base class for unified cache exposing enough methods to SharedObject
 * instances to allow their addRef() and removeRef() methods to
 * update cache metrics. No other part of ICU, except for SharedObject,
 * should directly call the methods of this base class.
 */
class U_COMMON_API UnifiedCacheBase : പൊതു UObject {
പൊതു:
    UnifiedCacheBase() { }

    /**
     * Notify the cache implementation that an object was seen transitioning to
     * zero hard references. The cache may use this to keep track the number of
     * unreferenced SharedObjects, and to trigger evictions.
     */
    മായാ ശൂന്യം handleUnreferencedObject() സ്ഥിരം = 0;

    മായാ ~UnifiedCacheBase();
സ്വകാര്യ:
    UnifiedCacheBase(സ്ഥിരം UnifiedCacheBase &) = delete;
    UnifiedCacheBase &operator=(സ്ഥിരം UnifiedCacheBase &) = delete;
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
class U_COMMON_API_CLASS SharedObject : പൊതു UObject {
പൊതു:
    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject() :
            soft_Ref_എണ്ണം(0),
            hard_Ref_എണ്ണം(0),
            cachePtr(nullptr) {}

    /** Initializes totalRefCount, softRefCount to 0. */
    U_COMMON_API SharedObject(സ്ഥിരം SharedObject &other) :
            UObject(other),
            soft_Ref_എണ്ണം(0),
            hard_Ref_എണ്ണം(0),
            cachePtr(nullptr) {}

    U_COMMON_API മായാ ~SharedObject();

    /**
     * Increments the number of hard references to this object. Thread-safe.
     * Not for use from within the Unified Cache implementation.
     */
    U_COMMON_API ശൂന്യം addRef() സ്ഥിരം;

    /**
     * Decrements the number of hard references to this object, and
     * arrange for possible cache-eviction and/or deletion if ref
     * count goes to zero. Thread-safe.
     * 
     * Not for use from within the UnifiedCache implementation.
     */
    U_COMMON_API ശൂന്യം removeRef() സ്ഥിരം;

    /**
     * Returns the number of hard references for this object.
     * Uses a memory barrier.
     */
    U_COMMON_API int32_t get_Ref_എണ്ണം() സ്ഥിരം;

    /**
     * If noHardReferences() == true then this object has no hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API വരിയിൽ UBool noHardReferences() സ്ഥിരം { തിരികെ get_Ref_എണ്ണം() == 0; }

    /**
     * If hasHardReferences() == true then this object has hard references.
     * Must be called only from within the internals of UnifiedCache.
     */
    U_COMMON_API വരിയിൽ UBool hasHardReferences() സ്ഥിരം { തിരികെ get_Ref_എണ്ണം() != 0; }

    /**
     * Deletes this object if it has no references.
     * Available for non-cached SharedObjects only. Ownership of cached objects
     * is with the UnifiedCache, which is solely responsible for eviction and deletion.
     */
    U_COMMON_API ശൂന്യം delete_If_Zero_Ref_എണ്ണം() സ്ഥിരം;

        
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
    സ്ഥിതം T *copyOnWrite(സ്ഥിരം T *&ptr) {
        സ്ഥിരം T *p = ptr;
        എങ്കിൽ(p->get_Ref_എണ്ണം() <= 1) { തിരികെ const_cast<T *>(p); }
        T *p2 = new T(*p);
        എങ്കിൽ(p2 == nullptr) { തിരികെ nullptr; }
        p->removeRef();
        ptr = p2;
        p2->addRef();
        തിരികെ p2;
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
    സ്ഥിതം ശൂന്യം copyPtr(സ്ഥിരം T *src, സ്ഥിരം T *&dest) {
        എങ്കിൽ(src != dest) {
            എങ്കിൽ(dest != nullptr) { dest->removeRef(); }
            dest = src;
            എങ്കിൽ(src != nullptr) { src->addRef(); }
        }
    }

    /**
     * Equivalent to copyPtr(nullptr, dest).
     */
    template<typename T>
    സ്ഥിതം ശൂന്യം clearPtr(സ്ഥിരം T *&ptr) {
        എങ്കിൽ (ptr != nullptr) {
            ptr->removeRef();
            ptr = nullptr;
        }
    }

സ്വകാര്യ:
    /**
     * The number of references from the UnifiedCache, which is
     * the number of times that the sharedObject is stored as a hash table value.
     * For use by UnifiedCache implementation code only.
     * All access is synchronized by UnifiedCache's gCacheMutex
     */
    mutable int32_t soft_Ref_എണ്ണം;
    friend class UnifiedCache;

    /**
     * Reference count, excluding references from within the UnifiedCache implementation.
     */
    mutable u_atomic_int32_t hard_Ref_എണ്ണം;
    
    mutable സ്ഥിരം UnifiedCacheBase *cachePtr;

};

U_NAMESPACE_END

#endif

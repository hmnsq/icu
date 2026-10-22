// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#include "sharedobject.h"
#include "mutex.h"
#include "uassert.h"
#include "umutex.h"
#include "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

void
SharedObject::addRef() const {
    umtx_atomic_inc(&hard_Ref_बिलांग);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
void
SharedObject::removeRef() const {
    const UnifiedCacheBase *cache = this->cachePtr;
    int32_t updated_Ref_बिलांग = umtx_atomic_dec(&hard_Ref_बिलांग);
    U_ASSERT(updated_Ref_बिलांग >= 0);
    if (updated_Ref_बिलांग == 0) {
        if (cache) {
            cache->handleUnreferencedObject();
        } else {
            delete this;
        }
    }
}


int32_t
SharedObject::get_Ref_बिलांग() const {
    return umtx_loadAcquire(hard_Ref_बिलांग);
}

void
SharedObject::delete_If_Zero_Ref_बिलांग() const {
    if (this->cachePtr == nullptr && get_Ref_बिलांग() == 0) {
        delete this;
    }
}

U_NAMESPACE_END

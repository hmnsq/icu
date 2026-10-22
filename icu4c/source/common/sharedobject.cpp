// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#অন্তর্ভাব "sharedobject.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

শূন্য
SharedObject::addRef() ধ্রুব {
    umtx_atomic_inc(&hard_Ref_গুনতি);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
শূন্য
SharedObject::removeRef() ধ্রুব {
    ধ্রুব UnifiedCacheBase *cache = এতত্->cachePtr;
    int32_t updated_Ref_গুনতি = umtx_atomic_dec(&hard_Ref_গুনতি);
    U_ASSERT(updated_Ref_গুনতি >= 0);
    যদি (updated_Ref_গুনতি == 0) {
        যদি (cache) {
            cache->handleUnreferencedObject();
        } অন্যথা {
            delete এতত্;
        }
    }
}


int32_t
SharedObject::get_Ref_গুনতি() ধ্রুব {
    প্রত্যাগম umtx_loadAcquire(hard_Ref_গুনতি);
}

শূন্য
SharedObject::delete_If_Zero_Ref_গুনতি() ধ্রুব {
    যদি (এতত্->cachePtr == nullptr && get_Ref_গুনতি() == 0) {
        delete এতত্;
    }
}

U_NAMESPACE_END

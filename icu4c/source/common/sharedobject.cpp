// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#شامل "sharedobject.h"
#شامل "mutex.h"
#شامل "uassert.h"
#شامل "umutex.h"
#شامل "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

تهی
SharedObject::addRef() ثابت {
    umtx_atomic_inc(&hard_Ref_شمار);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
تهی
SharedObject::removeRef() ثابت {
    ثابت UnifiedCacheBase *cache = این->cachePtr;
    int32_t updated_Ref_شمار = umtx_atomic_dec(&hard_Ref_شمار);
    U_ASSERT(updated_Ref_شمار >= 0);
    اگر (updated_Ref_شمار == 0) {
        اگر (cache) {
            cache->handleUnreferencedObject();
        } وگرنه {
            delete این;
        }
    }
}


int32_t
SharedObject::get_Ref_شمار() ثابت {
    بازگشت umtx_loadAcquire(hard_Ref_شمار);
}

تهی
SharedObject::delete_If_Zero_Ref_شمار() ثابت {
    اگر (این->cachePtr == nullptr && get_Ref_شمار() == 0) {
        delete این;
    }
}

U_NAMESPACE_END

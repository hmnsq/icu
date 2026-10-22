// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#ضمن "sharedobject.h"
#ضمن "mutex.h"
#ضمن "uassert.h"
#ضمن "umutex.h"
#ضمن "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

فراغ
SharedObject::addRef() ثابت {
    umtx_atomic_inc(&hard_Ref_ژماردن);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
فراغ
SharedObject::removeRef() ثابت {
    ثابت UnifiedCacheBase *cache = هذا->cachePtr;
    int32_t updated_Ref_ژماردن = umtx_atomic_dec(&hard_Ref_ژماردن);
    U_ASSERT(updated_Ref_ژماردن >= 0);
    إذا (updated_Ref_ژماردن == 0) {
        إذا (cache) {
            cache->handleUnreferencedObject();
        } وإلا {
            delete هذا;
        }
    }
}


int32_t
SharedObject::get_Ref_ژماردن() ثابت {
    أرجع umtx_loadAcquire(hard_Ref_ژماردن);
}

فراغ
SharedObject::delete_If_Zero_Ref_ژماردن() ثابت {
    إذا (هذا->cachePtr == nullptr && get_Ref_ژماردن() == 0) {
        delete هذا;
    }
}

U_NAMESPACE_END

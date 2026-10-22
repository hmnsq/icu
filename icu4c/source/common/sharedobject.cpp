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

Unified_Cache_پایه::~Unified_Cache_پایه() {}

فراغ
SharedObject::جمع_کن_Ref() ثابت {
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
فراغ
SharedObject::removeRef() ثابت {
    ثابت Unified_Cache_پایه *cache = هذا->cachePtr;
    int32_t updated_Ref_شمار = umtx_atomic_dec(&hard_Ref_شمار);
    U_ASSERT(updated_Ref_شمار >= 0);
    إذا (updated_Ref_شمار == 0) {
        إذا (cache) {
            cache->handleUnreferencedObject();
        } وإلا {
            حذف هذا;
        }
    }
}


int32_t
SharedObject::get_Ref_شمار() ثابت {
    أرجع umtx_بارگیری_Acquire(hard_Ref_شمار);
}

فراغ
SharedObject::delete_If_Zero_Ref_شمار() ثابت {
    إذا (هذا->cachePtr == اشاره_تهی && get_Ref_شمار() == 0) {
        حذف هذا;
    }
}

U_NAMESPACE_انتها

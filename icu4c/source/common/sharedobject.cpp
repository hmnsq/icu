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

Unified_Cache_بنیاد::~Unified_Cache_بنیاد() {}

خالی
SharedObject::جوڑو_Ref() ثابت {
    umtx_atomic_inc(&hard_Ref_گنتی);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
خالی
SharedObject::removeRef() ثابت {
    ثابت Unified_Cache_بنیاد *cache = یہ->cachePtr;
    int32_t updated_Ref_گنتی = umtx_atomic_dec(&hard_Ref_گنتی);
    U_ASSERT(updated_Ref_گنتی >= 0);
    اگر (updated_Ref_گنتی == 0) {
        اگر (cache) {
            cache->handleUnreferencedObject();
        } ورنہ {
            حذف یہ;
        }
    }
}


int32_t
SharedObject::get_Ref_گنتی() ثابت {
    واپس umtx_بھرو_Acquire(hard_Ref_گنتی);
}

خالی
SharedObject::delete_If_Zero_Ref_گنتی() ثابت {
    اگر (یہ->cachePtr == خالی_اشاریہ && get_Ref_گنتی() == 0) {
        حذف یہ;
    }
}

U_NAMESPACE_انتہا

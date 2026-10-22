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

Unified_Cache_بنسټ::~Unified_Cache_بنسټ() {}

فراغ
SharedObject::جمع_کړه_Ref() ثابت {
    umtx_atomic_inc(&hard_Ref_شمېر);
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
    ثابت Unified_Cache_بنسټ *cache = هذا->cachePtr;
    int32_t updated_Ref_شمېر = umtx_atomic_dec(&hard_Ref_شمېر);
    U_ASSERT(updated_Ref_شمېر >= 0);
    إذا (updated_Ref_شمېر == 0) {
        إذا (cache) {
            cache->handleUnreferencedObject();
        } وإلا {
            ړنګول هذا;
        }
    }
}


int32_t
SharedObject::get_Ref_شمېر() ثابت {
    أرجع umtx_بارول_Acquire(hard_Ref_شمېر);
}

فراغ
SharedObject::delete_If_Zero_Ref_شمېر() ثابت {
    إذا (هذا->cachePtr == تش_اشاره && get_Ref_شمېر() == 0) {
        ړنګول هذا;
    }
}

U_NAMESPACE_پای_ټکی

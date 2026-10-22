// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#ਅਨ੍ਤਰ੍ਭਾਵ "sharedobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

Unified_Cache_ਅਧਾਰ::~Unified_Cache_ਅਧਾਰ() {}

ਖਾਲੀ
SharedObject::ਜੋੜੋ_Ref() ਅਚਲ {
    umtx_atomic_inc(&hard_Ref_ਗਿਣਤੀ);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
ਖਾਲੀ
SharedObject::removeRef() ਅਚਲ {
    ਅਚਲ Unified_Cache_ਅਧਾਰ *cache = ਇਹ->cachePtr;
    int32_t updated_Ref_ਗਿਣਤੀ = umtx_atomic_dec(&hard_Ref_ਗਿਣਤੀ);
    U_ASSERT(updated_Ref_ਗਿਣਤੀ >= 0);
    ਜੇ (updated_Ref_ਗਿਣਤੀ == 0) {
        ਜੇ (cache) {
            cache->handleUnreferencedObject();
        } ਨਹੀਂਤਾਂ {
            ਮਿਟਾਓ ਇਹ;
        }
    }
}


int32_t
SharedObject::get_Ref_ਗਿਣਤੀ() ਅਚਲ {
    ਵਾਪਸ umtx_ਲੱਦੋ_Acquire(hard_Ref_ਗਿਣਤੀ);
}

ਖਾਲੀ
SharedObject::delete_If_Zero_Ref_ਗਿਣਤੀ() ਅਚਲ {
    ਜੇ (ਇਹ->cachePtr == ਖਾਲੀ_ਸੂਚਕ && get_Ref_ਗਿਣਤੀ() == 0) {
        ਮਿਟਾਓ ਇਹ;
    }
}

U_NAMESPACE_ਸਿਰਾ

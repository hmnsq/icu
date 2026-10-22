// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#અન્તર્ભાવ "sharedobject.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

શૂન્ય
SharedObject::addRef() અચલ {
    umtx_atomic_inc(&hard_Ref_ગણતરી);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
શૂન્ય
SharedObject::removeRef() અચલ {
    અચલ UnifiedCacheBase *cache = આ->cachePtr;
    int32_t updated_Ref_ગણતરી = umtx_atomic_dec(&hard_Ref_ગણતરી);
    U_ASSERT(updated_Ref_ગણતરી >= 0);
    જો (updated_Ref_ગણતરી == 0) {
        જો (cache) {
            cache->handleUnreferencedObject();
        } નહીંતર {
            delete આ;
        }
    }
}


int32_t
SharedObject::get_Ref_ગણતરી() અચલ {
    પાછા umtx_loadAcquire(hard_Ref_ગણતરી);
}

શૂન્ય
SharedObject::delete_If_Zero_Ref_ગણતરી() અચલ {
    જો (આ->cachePtr == nullptr && get_Ref_ગણતરી() == 0) {
        delete આ;
    }
}

U_NAMESPACE_END

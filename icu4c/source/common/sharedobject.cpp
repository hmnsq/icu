// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#అన్తర్భావ "sharedobject.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

Unified_Cache_పునాది::~Unified_Cache_పునాది() {}

శూన్య
SharedObject::కూడు_Ref() స్థిరరాశి {
    umtx_atomic_inc(&hard_Ref_లెక్క);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
శూన్య
SharedObject::removeRef() స్థిరరాశి {
    స్థిరరాశి Unified_Cache_పునాది *cache = ఇది->cachePtr;
    int32_t updated_Ref_లెక్క = umtx_atomic_dec(&hard_Ref_లెక్క);
    U_ASSERT(updated_Ref_లెక్క >= 0);
    ఒకవేళ (updated_Ref_లెక్క == 0) {
        ఒకవేళ (cache) {
            cache->handleUnreferencedObject();
        } లేకపోతే {
            తొలగించు ఇది;
        }
    }
}


int32_t
SharedObject::get_Ref_లెక్క() స్థిరరాశి {
    తిరిగి umtx_ఎక్కించు_Acquire(hard_Ref_లెక్క);
}

శూన్య
SharedObject::delete_If_Zero_Ref_లెక్క() స్థిరరాశి {
    ఒకవేళ (ఇది->cachePtr == శూన్యసూచిక && get_Ref_లెక్క() == 0) {
        తొలగించు ఇది;
    }
}

U_NAMESPACE_చివర

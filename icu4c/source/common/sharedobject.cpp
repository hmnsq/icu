// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#அந்தர்பாவ "sharedobject.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

வெறுமை
SharedObject::addRef() மாறிலி {
    umtx_atomic_inc(&hard_Ref_மொத்தம்);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
வெறுமை
SharedObject::removeRef() மாறிலி {
    மாறிலி UnifiedCacheBase *cache = இது->cachePtr;
    int32_t updated_Ref_மொத்தம் = umtx_atomic_dec(&hard_Ref_மொத்தம்);
    U_ASSERT(updated_Ref_மொத்தம் >= 0);
    எனில் (updated_Ref_மொத்தம் == 0) {
        எனில் (cache) {
            cache->handleUnreferencedObject();
        } இல்லையெனில் {
            delete இது;
        }
    }
}


int32_t
SharedObject::get_Ref_மொத்தம்() மாறிலி {
    திரும்பு umtx_loadAcquire(hard_Ref_மொத்தம்);
}

வெறுமை
SharedObject::delete_If_Zero_Ref_மொத்தம்() மாறிலி {
    எனில் (இது->cachePtr == nullptr && get_Ref_மொத்தம்() == 0) {
        delete இது;
    }
}

U_NAMESPACE_END

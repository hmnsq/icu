// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#ଅନ୍ତର୍ଭାଵ "sharedobject.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

ଶୂନ୍ୟ
SharedObject::addRef() ଅଚଳ {
    umtx_atomic_inc(&hard_Ref_ଗଣନା);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
ଶୂନ୍ୟ
SharedObject::removeRef() ଅଚଳ {
    ଅଚଳ UnifiedCacheBase *cache = ଏହି->cachePtr;
    int32_t updated_Ref_ଗଣନା = umtx_atomic_dec(&hard_Ref_ଗଣନା);
    U_ASSERT(updated_Ref_ଗଣନା >= 0);
    ଯଦି (updated_Ref_ଗଣନା == 0) {
        ଯଦି (cache) {
            cache->handleUnreferencedObject();
        } ଅନ୍ୟଥା {
            delete ଏହି;
        }
    }
}


int32_t
SharedObject::get_Ref_ଗଣନା() ଅଚଳ {
    ଫେରନ୍ତୁ umtx_loadAcquire(hard_Ref_ଗଣନା);
}

ଶୂନ୍ୟ
SharedObject::delete_If_Zero_Ref_ଗଣନା() ଅଚଳ {
    ଯଦି (ଏହି->cachePtr == nullptr && get_Ref_ଗଣନା() == 0) {
        delete ଏହି;
    }
}

U_NAMESPACE_END

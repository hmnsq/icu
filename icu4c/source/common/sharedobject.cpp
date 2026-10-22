// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#अन्तर्भाव "sharedobject.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

शून्य
SharedObject::addRef() अचर {
    umtx_atomic_inc(&hard_Ref_गन्ती);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
शून्य
SharedObject::removeRef() अचर {
    अचर UnifiedCacheBase *cache = एतत्->cachePtr;
    int32_t updated_Ref_गन्ती = umtx_atomic_dec(&hard_Ref_गन्ती);
    U_ASSERT(updated_Ref_गन्ती >= 0);
    यदि (updated_Ref_गन्ती == 0) {
        यदि (cache) {
            cache->handleUnreferencedObject();
        } अन्यथा {
            delete एतत्;
        }
    }
}


int32_t
SharedObject::get_Ref_गन्ती() अचर {
    फर्काऊ umtx_loadAcquire(hard_Ref_गन्ती);
}

शून्य
SharedObject::delete_If_Zero_Ref_गन्ती() अचर {
    यदि (एतत्->cachePtr == nullptr && get_Ref_गन्ती() == 0) {
        delete एतत्;
    }
}

U_NAMESPACE_END

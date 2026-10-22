// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#समावेश "sharedobject.h"
#समावेश "mutex.h"
#समावेश "uassert.h"
#समावेश "umutex.h"
#समावेश "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

व्योम
SharedObject::addRef() स्थिर {
    umtx_atomic_inc(&hard_Ref_गिनती);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
व्योम
SharedObject::removeRef() स्थिर {
    स्थिर UnifiedCacheBase *cache = यह->cachePtr;
    int32_t updated_Ref_गिनती = umtx_atomic_dec(&hard_Ref_गिनती);
    U_ASSERT(updated_Ref_गिनती >= 0);
    अगर (updated_Ref_गिनती == 0) {
        अगर (cache) {
            cache->handleUnreferencedObject();
        } अन्यथा {
            मिटाओ यह;
        }
    }
}


int32_t
SharedObject::get_Ref_गिनती() स्थिर {
    वापस umtx_loadAcquire(hard_Ref_गिनती);
}

व्योम
SharedObject::delete_If_Zero_Ref_गिनती() स्थिर {
    अगर (यह->cachePtr == nullptr && get_Ref_गिनती() == 0) {
        मिटाओ यह;
    }
}

U_NAMESPACE_END

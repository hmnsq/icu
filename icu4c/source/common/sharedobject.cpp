// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#അന്തര്ഭാവ "sharedobject.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

UnifiedCacheBase::~UnifiedCacheBase() {}

ശൂന്യം
SharedObject::addRef() സ്ഥിരം {
    umtx_atomic_inc(&hard_Ref_എണ്ണം);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
ശൂന്യം
SharedObject::removeRef() സ്ഥിരം {
    സ്ഥിരം UnifiedCacheBase *cache = ഇത്->cachePtr;
    int32_t updated_Ref_എണ്ണം = umtx_atomic_dec(&hard_Ref_എണ്ണം);
    U_ASSERT(updated_Ref_എണ്ണം >= 0);
    എങ്കിൽ (updated_Ref_എണ്ണം == 0) {
        എങ്കിൽ (cache) {
            cache->handleUnreferencedObject();
        } അല്ലെങ്കിൽ {
            delete ഇത്;
        }
    }
}


int32_t
SharedObject::get_Ref_എണ്ണം() സ്ഥിരം {
    തിരികെ umtx_loadAcquire(hard_Ref_എണ്ണം);
}

ശൂന്യം
SharedObject::delete_If_Zero_Ref_എണ്ണം() സ്ഥിരം {
    എങ്കിൽ (ഇത്->cachePtr == nullptr && get_Ref_എണ്ണം() == 0) {
        delete ഇത്;
    }
}

U_NAMESPACE_END

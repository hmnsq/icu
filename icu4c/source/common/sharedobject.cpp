// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2015, International Business Machines
* Corporation and others.  All Rights Reserved.
******************************************************************************
* sharedobject.cpp
*/
#ಅನ್ತರ್ಭಾವ "sharedobject.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "unifiedcache.h"

U_NAMESPACE_BEGIN

SharedObject::~SharedObject() {}

Unified_Cache_ತಳ::~Unified_Cache_ತಳ() {}

ಶೂನ್ಯ
SharedObject::ಕೂಡಿಸು_Ref() ಸ್ಥಿರಾಂಕ {
    umtx_atomic_inc(&hard_Ref_ಎಣಿಕೆ);
}

// removeRef Decrement the reference count and delete if it is zero.
//           Note that SharedObjects with a non-null cachePtr are owned by the
//           unified cache, and the cache will be responsible for the actual deletion.
//           The deletion could be as soon as immediately following the
//           update to the reference count, if another thread is running
//           a cache eviction cycle concurrently.
//           NO ACCESS TO *this PERMITTED AFTER REFERENCE COUNT == 0 for cached objects.
//           THE OBJECT MAY ALREADY BE GONE.
ಶೂನ್ಯ
SharedObject::removeRef() ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ Unified_Cache_ತಳ *cache = ಇದು->cachePtr;
    int32_t updated_Ref_ಎಣಿಕೆ = umtx_atomic_dec(&hard_Ref_ಎಣಿಕೆ);
    U_ASSERT(updated_Ref_ಎಣಿಕೆ >= 0);
    ಆದರೆ (updated_Ref_ಎಣಿಕೆ == 0) {
        ಆದರೆ (cache) {
            cache->handleUnreferencedObject();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಅಳಿಸು ಇದು;
        }
    }
}


int32_t
SharedObject::get_Ref_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ umtx_ತುಂಬು_Acquire(hard_Ref_ಎಣಿಕೆ);
}

ಶೂನ್ಯ
SharedObject::delete_If_Zero_Ref_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಇದು->cachePtr == ಶೂನ್ಯಸೂಚಕ && get_Ref_ಎಣಿಕೆ() == 0) {
        ಅಳಿಸು ಇದು;
    }
}

U_NAMESPACE_ಕೊನೆ

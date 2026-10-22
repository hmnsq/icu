// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1997-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File umutex.cpp
*
* Modification History:
*
*   Date        Name        Description
*   04/02/97    aliu        Creation.
*   04/07/99    srl         updated
*   05/13/99    stephen     Changed to umutex (from cmutex).
*   11/22/99    aliu        Make non-global mutex autoinitialize [j151]
******************************************************************************
*/

#अन्तर्भाव "umutex.h"

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "cmemory.h"

U_NAMESPACE_BEGIN


#यदि defined(U_USER_MUTEX_CPP)
// Support for including an alternate implementation of mutexes has been withdrawn.
// See issue ICU-20185.
#दोषः U_USER_MUTEX_CPP not supported
#यदि_अन्तः


/*************************************************************************************************
 *
 *  ICU Mutex wrappers.
 *
 *************************************************************************************************/

नामाकाश {
std::mutex *initMutex;
std::condition_variable *initCondition;

// The ICU global mutex.
// Used when ICU implementation code passes nullptr for the mutex pointer.
UMutex globalMutex;

std::once_flag initFlag;
std::once_flag *pInitFlag = &initFlag;

}  // Anonymous namespace

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV umtx_cleanup() {
    initMutex->~mutex();
    initCondition->~condition_variable();
    UMutex::cleanup();

    // Reset the once_flag, by destructing it and creating a fresh one in its place.
    // Do not use this trick anywhere else in ICU; use umtx_initOnce, not std::call_once().
    pInitFlag->~once_flag();
    pInitFlag = नूतन(&initFlag) std::once_flag();
    प्रत्यागम सत्य;
}

स्थिर शून्य U_CALLCONV umtx_init() {
    initMutex = STATIC_NEW(std::mutex);
    initCondition = STATIC_NEW(std::condition_variable);
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_MUTEX, umtx_cleanup);
}
U_CDECL_अन्तः


std::mutex *UMutex::getMutex() {
    std::mutex *retPtr = fMutex.load(std::memory_order_acquire);
    यदि (retPtr == शून्यसूचकः) {
        std::call_once(*pInitFlag, umtx_init);
        std::lock_guard<std::mutex> guard(*initMutex);
        retPtr = fMutex.load(std::memory_order_acquire);
        यदि (retPtr == शून्यसूचकः) {
            fMutex = नूतन(fStorage) std::mutex();
            retPtr = fMutex;
            fListLink = g_List_शीर्ष;
            g_List_शीर्ष = एतत्;
        }
    }
    U_ASSERT(retPtr != शून्यसूचकः);
    प्रत्यागम retPtr;
}

UMutex *UMutex::g_List_शीर्ष = शून्यसूचकः;

शून्य UMutex::cleanup() {
    UMutex *next = शून्यसूचकः;
    प्रति (UMutex *m = g_List_शीर्ष; m != शून्यसूचकः; m = next) {
        (*m->fMutex).~mutex();
        m->fMutex = शून्यसूचकः;
        next = m->fListLink;
        m->fListLink = शून्यसूचकः;
    }
    g_List_शीर्ष = शून्यसूचकः;
}


U_CAPI शून्य  U_EXPORT2
umtx_तालकम्(UMutex *mutex) {
    यदि (mutex == शून्यसूचकः) {
        mutex = &globalMutex;
    }
    mutex->तालकम्();
}


U_CAPI शून्य  U_EXPORT2
umtx_unlock(UMutex* mutex)
{
    यदि (mutex == शून्यसूचकः) {
        mutex = &globalMutex;
    }
    mutex->unlock();
}


/*************************************************************************************************
 *
 *  UInitOnce Implementation
 *
 *************************************************************************************************/

// This function is called when a test of a UInitOnce::fState reveals that
//   initialization has not completed, that we either need to call the init
//   function on this thread, or wait for some other thread to complete.
//
// The actual call to the init function is made inline by template code
//   that knows the C++ types involved. This function returns true if
//   the caller needs to call the Init function.
//
U_COMMON_API UBool U_EXPORT2
umtx_initImplPreInit(UInitOnce &uio) {
    std::call_once(*pInitFlag, umtx_init);
    std::unique_lock<std::mutex> तालकम्(*initMutex);
    यदि (umtx_आरोपय_Acquire(uio.fState) == 0) {
        umtx_storeRelease(uio.fState, 1);
        प्रत्यागम सत्य;      // Caller will next call the init function.
    } अन्यथा {
        यावत् (umtx_आरोपय_Acquire(uio.fState) == 1) {
            // Another thread is currently running the initialization.
            // Wait until it completes.
            initCondition->प्रतीक्षस्व(तालकम्);
        }
        U_ASSERT(uio.fState == 2);
        प्रत्यागम असत्य;
    }
}


// This function is called by the thread that ran an initialization function,
// just after completing the function.
//   Some threads may be waiting on the condition, requiring the broadcast wakeup.
//   Some threads may be racing to test the fState variable outside of the mutex,
//   requiring the use of store/release when changing its value.

U_COMMON_API शून्य U_EXPORT2
umtx_initImplPostInit(UInitOnce &uio) {
    {
        std::unique_lock<std::mutex> तालकम्(*initMutex);
        umtx_storeRelease(uio.fState, 2);
    }
    initCondition->notify_all();
}

U_NAMESPACE_अन्तः

/*************************************************************************************************
 *
 *  Deprecated functions for setting user mutexes.
 *
 *************************************************************************************************/

U_DEPRECATED शून्य U_EXPORT2
u_समुच्चयः_Mutex_Functions(ध्रुव शून्य * /*context */, UMtxInitFn *, UMtxFn *,
                    UMtxFn *,  UMtxFn *, UError_संहिता *स्थितिः) {
    यदि (U_SUCCESS(*स्थितिः)) {
        *स्थितिः = U_UNSUPPORTED_ERROR;
    }
}



U_DEPRECATED शून्य U_EXPORT2
u_समुच्चयः_Atomic_Inc_Dec_Functions(ध्रुव शून्य * /*context */, UMtxAtomicFn *, UMtxAtomicFn *,
                           UError_संहिता *स्थितिः) {
    यदि (U_SUCCESS(*स्थितिः)) {
        *स्थितिः = U_UNSUPPORTED_ERROR;
    }
}

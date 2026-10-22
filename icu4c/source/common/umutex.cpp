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

#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

U_NAMESPACE_BEGIN


#ਜੇ defined(U_USER_MUTEX_CPP)
// Support for including an alternate implementation of mutexes has been withdrawn.
// See issue ICU-20185.
#ਗਲਤੀ U_USER_MUTEX_CPP not supported
#ਜੇ_ਅੰਤ


/*************************************************************************************************
 *
 *  ICU Mutex wrappers.
 *
 *************************************************************************************************/

ਨਾਮਾਕਾਸ਼ {
std::mutex *initMutex;
std::condition_variable *initCondition;

// The ICU global mutex.
// Used when ICU implementation code passes nullptr for the mutex pointer.
UMutex globalMutex;

std::once_flag initFlag;
std::once_flag *pInitFlag = &initFlag;

}  // Anonymous namespace

U_CDECL_BEGIN
ਸਥਿਰ UBool U_CALLCONV umtx_cleanup() {
    initMutex->~mutex();
    initCondition->~condition_variable();
    UMutex::cleanup();

    // Reset the once_flag, by destructing it and creating a fresh one in its place.
    // Do not use this trick anywhere else in ICU; use umtx_initOnce, not std::call_once().
    pInitFlag->~once_flag();
    pInitFlag = ਨਵਾਂ(&initFlag) std::once_flag();
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV umtx_init() {
    initMutex = STATIC_NEW(std::mutex);
    initCondition = STATIC_NEW(std::condition_variable);
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_MUTEX, umtx_cleanup);
}
U_CDECL_ਸਿਰਾ


std::mutex *UMutex::getMutex() {
    std::mutex *retPtr = fMutex.load(std::memory_order_acquire);
    ਜੇ (retPtr == ਖਾਲੀ_ਸੂਚਕ) {
        std::call_once(*pInitFlag, umtx_init);
        std::lock_guard<std::mutex> guard(*initMutex);
        retPtr = fMutex.load(std::memory_order_acquire);
        ਜੇ (retPtr == ਖਾਲੀ_ਸੂਚਕ) {
            fMutex = ਨਵਾਂ(fStorage) std::mutex();
            retPtr = fMutex;
            fListLink = g_List_ਸਿਰ;
            g_List_ਸਿਰ = ਇਹ;
        }
    }
    U_ASSERT(retPtr != ਖਾਲੀ_ਸੂਚਕ);
    ਵਾਪਸ retPtr;
}

UMutex *UMutex::g_List_ਸਿਰ = ਖਾਲੀ_ਸੂਚਕ;

ਖਾਲੀ UMutex::cleanup() {
    UMutex *next = ਖਾਲੀ_ਸੂਚਕ;
    ਲਈ (UMutex *m = g_List_ਸਿਰ; m != ਖਾਲੀ_ਸੂਚਕ; m = next) {
        (*m->fMutex).~mutex();
        m->fMutex = ਖਾਲੀ_ਸੂਚਕ;
        next = m->fListLink;
        m->fListLink = ਖਾਲੀ_ਸੂਚਕ;
    }
    g_List_ਸਿਰ = ਖਾਲੀ_ਸੂਚਕ;
}


U_CAPI ਖਾਲੀ  U_EXPORT2
umtx_ਤਾਲਾ(UMutex *mutex) {
    ਜੇ (mutex == ਖਾਲੀ_ਸੂਚਕ) {
        mutex = &globalMutex;
    }
    mutex->ਤਾਲਾ();
}


U_CAPI ਖਾਲੀ  U_EXPORT2
umtx_unlock(UMutex* mutex)
{
    ਜੇ (mutex == ਖਾਲੀ_ਸੂਚਕ) {
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
    std::unique_lock<std::mutex> ਤਾਲਾ(*initMutex);
    ਜੇ (umtx_ਲੱਦੋ_Acquire(uio.fState) == 0) {
        umtx_storeRelease(uio.fState, 1);
        ਵਾਪਸ ਸੱਚ;      // Caller will next call the init function.
    } ਨਹੀਂਤਾਂ {
        ਜਦੋਂ (umtx_ਲੱਦੋ_Acquire(uio.fState) == 1) {
            // Another thread is currently running the initialization.
            // Wait until it completes.
            initCondition->wait(ਤਾਲਾ);
        }
        U_ASSERT(uio.fState == 2);
        ਵਾਪਸ ਝੂਠ;
    }
}


// This function is called by the thread that ran an initialization function,
// just after completing the function.
//   Some threads may be waiting on the condition, requiring the broadcast wakeup.
//   Some threads may be racing to test the fState variable outside of the mutex,
//   requiring the use of store/release when changing its value.

U_COMMON_API ਖਾਲੀ U_EXPORT2
umtx_initImplPostInit(UInitOnce &uio) {
    {
        std::unique_lock<std::mutex> ਤਾਲਾ(*initMutex);
        umtx_storeRelease(uio.fState, 2);
    }
    initCondition->notify_all();
}

U_NAMESPACE_ਸਿਰਾ

/*************************************************************************************************
 *
 *  Deprecated functions for setting user mutexes.
 *
 *************************************************************************************************/

U_DEPRECATED ਖਾਲੀ U_EXPORT2
u_ਸੈੱਟ_Mutex_Functions(ਅਚਲ ਖਾਲੀ * /*context */, UMtxInitFn *, UMtxFn *,
                    UMtxFn *,  UMtxFn *, UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
    }
}



U_DEPRECATED ਖਾਲੀ U_EXPORT2
u_ਸੈੱਟ_Atomic_Inc_Dec_Functions(ਅਚਲ ਖਾਲੀ * /*context */, UMtxAtomicFn *, UMtxAtomicFn *,
                           UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਜੇ (U_SUCCESS(*ਸਥਿਤੀ)) {
        *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
    }
}

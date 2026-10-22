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

#ಅನ್ತರ್ಭಾವ "umutex.h"

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

U_NAMESPACE_BEGIN


#ಆದರೆ defined(U_USER_MUTEX_CPP)
// Support for including an alternate implementation of mutexes has been withdrawn.
// See issue ICU-20185.
#ದೋಷ U_USER_MUTEX_CPP not supported
#ಆದರೆ_ಅಂತ್ಯ


/*************************************************************************************************
 *
 *  ICU Mutex wrappers.
 *
 *************************************************************************************************/

ನಾಮಾಕಾಶ {
std::mutex *initMutex;
std::condition_variable *initCondition;

// The ICU global mutex.
// Used when ICU implementation code passes nullptr for the mutex pointer.
UMutex globalMutex;

std::once_flag initFlag;
std::once_flag *pInitFlag = &initFlag;

}  // Anonymous namespace

U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV umtx_cleanup() {
    initMutex->~mutex();
    initCondition->~condition_variable();
    UMutex::cleanup();

    // Reset the once_flag, by destructing it and creating a fresh one in its place.
    // Do not use this trick anywhere else in ICU; use umtx_initOnce, not std::call_once().
    pInitFlag->~once_flag();
    pInitFlag = ಹೊಸ(&initFlag) std::once_flag();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV umtx_init() {
    initMutex = STATIC_NEW(std::mutex);
    initCondition = STATIC_NEW(std::condition_variable);
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_MUTEX, umtx_cleanup);
}
U_CDECL_ಕೊನೆ


std::mutex *UMutex::getMutex() {
    std::mutex *retPtr = fMutex.load(std::memory_order_acquire);
    ಆದರೆ (retPtr == ಶೂನ್ಯಸೂಚಕ) {
        std::call_once(*pInitFlag, umtx_init);
        std::lock_guard<std::mutex> guard(*initMutex);
        retPtr = fMutex.load(std::memory_order_acquire);
        ಆದರೆ (retPtr == ಶೂನ್ಯಸೂಚಕ) {
            fMutex = ಹೊಸ(fStorage) std::mutex();
            retPtr = fMutex;
            fListLink = g_List_ತಲೆ;
            g_List_ತಲೆ = ಇದು;
        }
    }
    U_ASSERT(retPtr != ಶೂನ್ಯಸೂಚಕ);
    ಹಿಂತಿರುಗಿ retPtr;
}

UMutex *UMutex::g_List_ತಲೆ = ಶೂನ್ಯಸೂಚಕ;

ಶೂನ್ಯ UMutex::cleanup() {
    UMutex *next = ಶೂನ್ಯಸೂಚಕ;
    ಗಾಗಿ (UMutex *m = g_List_ತಲೆ; m != ಶೂನ್ಯಸೂಚಕ; m = next) {
        (*m->fMutex).~mutex();
        m->fMutex = ಶೂನ್ಯಸೂಚಕ;
        next = m->fListLink;
        m->fListLink = ಶೂನ್ಯಸೂಚಕ;
    }
    g_List_ತಲೆ = ಶೂನ್ಯಸೂಚಕ;
}


U_CAPI ಶೂನ್ಯ  U_EXPORT2
umtx_ಬೀಗ(UMutex *mutex) {
    ಆದರೆ (mutex == ಶೂನ್ಯಸೂಚಕ) {
        mutex = &globalMutex;
    }
    mutex->ಬೀಗ();
}


U_CAPI ಶೂನ್ಯ  U_EXPORT2
umtx_unlock(UMutex* mutex)
{
    ಆದರೆ (mutex == ಶೂನ್ಯಸೂಚಕ) {
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
    std::unique_lock<std::mutex> ಬೀಗ(*initMutex);
    ಆದರೆ (umtx_ತುಂಬು_Acquire(uio.fState) == 0) {
        umtx_storeRelease(uio.fState, 1);
        ಹಿಂತಿರುಗಿ ನಿಜ;      // Caller will next call the init function.
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಗ (umtx_ತುಂಬು_Acquire(uio.fState) == 1) {
            // Another thread is currently running the initialization.
            // Wait until it completes.
            initCondition->wait(ಬೀಗ);
        }
        U_ASSERT(uio.fState == 2);
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}


// This function is called by the thread that ran an initialization function,
// just after completing the function.
//   Some threads may be waiting on the condition, requiring the broadcast wakeup.
//   Some threads may be racing to test the fState variable outside of the mutex,
//   requiring the use of store/release when changing its value.

U_COMMON_API ಶೂನ್ಯ U_EXPORT2
umtx_initImplPostInit(UInitOnce &uio) {
    {
        std::unique_lock<std::mutex> ಬೀಗ(*initMutex);
        umtx_storeRelease(uio.fState, 2);
    }
    initCondition->notify_all();
}

U_NAMESPACE_ಕೊನೆ

/*************************************************************************************************
 *
 *  Deprecated functions for setting user mutexes.
 *
 *************************************************************************************************/

U_DEPRECATED ಶೂನ್ಯ U_EXPORT2
u_ಗಣ_Mutex_Functions(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * /*context */, UMtxInitFn *, UMtxFn *,
                    UMtxFn *,  UMtxFn *, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
    }
}



U_DEPRECATED ಶೂನ್ಯ U_EXPORT2
u_ಗಣ_Atomic_Inc_Dec_Functions(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ * /*context */, UMtxAtomicFn *, UMtxAtomicFn *,
                           UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
        *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
    }
}

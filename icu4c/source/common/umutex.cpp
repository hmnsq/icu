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

#ضمن "umutex.h"

#ضمن "unicode/utypes.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "cmemory.h"

U_NAMESPACE_BEGIN


#إذا defined(U_USER_MUTEX_CPP)
// Support for including an alternate implementation of mutexes has been withdrawn.
// See issue ICU-20185.
#خطا U_USER_MUTEX_CPP not supported
#پایان_اگر


/*************************************************************************************************
 *
 *  ICU Mutex wrappers.
 *
 *************************************************************************************************/

نطاق {
std::mutex *initMutex;
std::condition_variable *initCondition;

// The ICU global mutex.
// Used when ICU implementation code passes nullptr for the mutex pointer.
UMutex globalMutex;

std::once_flag initFlag;
std::once_flag *pInitFlag = &initFlag;

}  // Anonymous namespace

U_CDECL_BEGIN
ساكن UBool U_CALLCONV umtx_cleanup() {
    initMutex->~mutex();
    initCondition->~condition_variable();
    UMutex::cleanup();

    // Reset the once_flag, by destructing it and creating a fresh one in its place.
    // Do not use this trick anywhere else in ICU; use umtx_initOnce, not std::call_once().
    pInitFlag->~once_flag();
    pInitFlag = جديد(&initFlag) std::once_flag();
    أرجع درست;
}

ساكن فراغ U_CALLCONV umtx_init() {
    initMutex = STATIC_NEW(std::mutex);
    initCondition = STATIC_NEW(std::condition_variable);
    ucln_common_ثبات_Cleanup(UCLN_COMMON_MUTEX, umtx_cleanup);
}
U_CDECL_انتها


std::mutex *UMutex::getMutex() {
    std::mutex *retPtr = fMutex.load(std::memory_order_acquire);
    إذا (retPtr == اشاره_تهی) {
        std::call_once(*pInitFlag, umtx_init);
        std::lock_guard<std::mutex> guard(*initMutex);
        retPtr = fMutex.load(std::memory_order_acquire);
        إذا (retPtr == اشاره_تهی) {
            fMutex = جديد(fStorage) std::mutex();
            retPtr = fMutex;
            fListLink = g_List_سر;
            g_List_سر = هذا;
        }
    }
    U_ASSERT(retPtr != اشاره_تهی);
    أرجع retPtr;
}

UMutex *UMutex::g_List_سر = اشاره_تهی;

فراغ UMutex::cleanup() {
    UMutex *next = اشاره_تهی;
    لكل (UMutex *m = g_List_سر; m != اشاره_تهی; m = next) {
        (*m->fMutex).~mutex();
        m->fMutex = اشاره_تهی;
        next = m->fListLink;
        m->fListLink = اشاره_تهی;
    }
    g_List_سر = اشاره_تهی;
}


U_CAPI فراغ  U_EXPORT2
umtx_قفل(UMutex *mutex) {
    إذا (mutex == اشاره_تهی) {
        mutex = &globalMutex;
    }
    mutex->قفل();
}


U_CAPI فراغ  U_EXPORT2
umtx_unlock(UMutex* mutex)
{
    إذا (mutex == اشاره_تهی) {
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
    std::unique_lock<std::mutex> قفل(*initMutex);
    إذا (umtx_بارگیری_Acquire(uio.fState) == 0) {
        umtx_storeRelease(uio.fState, 1);
        أرجع درست;      // Caller will next call the init function.
    } وإلا {
        طالما (umtx_بارگیری_Acquire(uio.fState) == 1) {
            // Another thread is currently running the initialization.
            // Wait until it completes.
            initCondition->wait(قفل);
        }
        U_ASSERT(uio.fState == 2);
        أرجع خطأ;
    }
}


// This function is called by the thread that ran an initialization function,
// just after completing the function.
//   Some threads may be waiting on the condition, requiring the broadcast wakeup.
//   Some threads may be racing to test the fState variable outside of the mutex,
//   requiring the use of store/release when changing its value.

U_COMMON_API فراغ U_EXPORT2
umtx_initImplPostInit(UInitOnce &uio) {
    {
        std::unique_lock<std::mutex> قفل(*initMutex);
        umtx_storeRelease(uio.fState, 2);
    }
    initCondition->notify_all();
}

U_NAMESPACE_انتها

/*************************************************************************************************
 *
 *  Deprecated functions for setting user mutexes.
 *
 *************************************************************************************************/

U_DEPRECATED فراغ U_EXPORT2
u_مجموعه_Mutex_Functions(ثابت فراغ * /*context */, UMtxInitFn *, UMtxFn *,
                    UMtxFn *,  UMtxFn *, UError_کد *وضعیت) {
    إذا (U_SUCCESS(*وضعیت)) {
        *وضعیت = U_UNSUPPORTED_ERROR;
    }
}



U_DEPRECATED فراغ U_EXPORT2
u_مجموعه_Atomic_Inc_Dec_Functions(ثابت فراغ * /*context */, UMtxAtomicFn *, UMtxAtomicFn *,
                           UError_کد *وضعیت) {
    إذا (U_SUCCESS(*وضعیت)) {
        *وضعیت = U_UNSUPPORTED_ERROR;
    }
}

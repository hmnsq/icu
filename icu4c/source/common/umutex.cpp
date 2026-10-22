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

#شامل "umutex.h"

#شامل "unicode/utypes.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "cmemory.h"

U_NAMESPACE_BEGIN


#اگر defined(U_USER_MUTEX_CPP)
// Support for including an alternate implementation of mutexes has been withdrawn.
// See issue ICU-20185.
#غلطی U_USER_MUTEX_CPP not supported
#اگر_ختم


/*************************************************************************************************
 *
 *  ICU Mutex wrappers.
 *
 *************************************************************************************************/

نامخلا {
std::mutex *initMutex;
std::condition_variable *initCondition;

// The ICU global mutex.
// Used when ICU implementation code passes nullptr for the mutex pointer.
UMutex globalMutex;

std::once_flag initFlag;
std::once_flag *pInitFlag = &initFlag;

}  // Anonymous namespace

U_CDECL_BEGIN
ساکن UBool U_CALLCONV umtx_cleanup() {
    initMutex->~mutex();
    initCondition->~condition_variable();
    UMutex::cleanup();

    // Reset the once_flag, by destructing it and creating a fresh one in its place.
    // Do not use this trick anywhere else in ICU; use umtx_initOnce, not std::call_once().
    pInitFlag->~once_flag();
    pInitFlag = نیا(&initFlag) std::once_flag();
    واپس سچ;
}

ساکن خالی U_CALLCONV umtx_init() {
    initMutex = STATIC_NEW(std::mutex);
    initCondition = STATIC_NEW(std::condition_variable);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_MUTEX, umtx_cleanup);
}
U_CDECL_انتہا


std::mutex *UMutex::getMutex() {
    std::mutex *retPtr = fMutex.load(std::memory_order_acquire);
    اگر (retPtr == خالی_اشاریہ) {
        std::call_once(*pInitFlag, umtx_init);
        std::lock_guard<std::mutex> guard(*initMutex);
        retPtr = fMutex.load(std::memory_order_acquire);
        اگر (retPtr == خالی_اشاریہ) {
            fMutex = نیا(fStorage) std::mutex();
            retPtr = fMutex;
            fListLink = g_List_سر;
            g_List_سر = یہ;
        }
    }
    U_ASSERT(retPtr != خالی_اشاریہ);
    واپس retPtr;
}

UMutex *UMutex::g_List_سر = خالی_اشاریہ;

خالی UMutex::cleanup() {
    UMutex *next = خالی_اشاریہ;
    برائے (UMutex *m = g_List_سر; m != خالی_اشاریہ; m = next) {
        (*m->fMutex).~mutex();
        m->fMutex = خالی_اشاریہ;
        next = m->fListLink;
        m->fListLink = خالی_اشاریہ;
    }
    g_List_سر = خالی_اشاریہ;
}


U_CAPI خالی  U_EXPORT2
umtx_تالا(UMutex *mutex) {
    اگر (mutex == خالی_اشاریہ) {
        mutex = &globalMutex;
    }
    mutex->تالا();
}


U_CAPI خالی  U_EXPORT2
umtx_unlock(UMutex* mutex)
{
    اگر (mutex == خالی_اشاریہ) {
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
    std::unique_lock<std::mutex> تالا(*initMutex);
    اگر (umtx_بھرو_Acquire(uio.fState) == 0) {
        umtx_storeRelease(uio.fState, 1);
        واپس سچ;      // Caller will next call the init function.
    } ورنہ {
        جب (umtx_بھرو_Acquire(uio.fState) == 1) {
            // Another thread is currently running the initialization.
            // Wait until it completes.
            initCondition->wait(تالا);
        }
        U_ASSERT(uio.fState == 2);
        واپس جھوٹ;
    }
}


// This function is called by the thread that ran an initialization function,
// just after completing the function.
//   Some threads may be waiting on the condition, requiring the broadcast wakeup.
//   Some threads may be racing to test the fState variable outside of the mutex,
//   requiring the use of store/release when changing its value.

U_COMMON_API خالی U_EXPORT2
umtx_initImplPostInit(UInitOnce &uio) {
    {
        std::unique_lock<std::mutex> تالا(*initMutex);
        umtx_storeRelease(uio.fState, 2);
    }
    initCondition->notify_all();
}

U_NAMESPACE_انتہا

/*************************************************************************************************
 *
 *  Deprecated functions for setting user mutexes.
 *
 *************************************************************************************************/

U_DEPRECATED خالی U_EXPORT2
u_سیٹ_Mutex_Functions(ثابت خالی * /*context */, UMtxInitFn *, UMtxFn *,
                    UMtxFn *,  UMtxFn *, UError_رمز *حالت_2) {
    اگر (U_SUCCESS(*حالت_2)) {
        *حالت_2 = U_UNSUPPORTED_ERROR;
    }
}



U_DEPRECATED خالی U_EXPORT2
u_سیٹ_Atomic_Inc_Dec_Functions(ثابت خالی * /*context */, UMtxAtomicFn *, UMtxAtomicFn *,
                           UError_رمز *حالت_2) {
    اگر (U_SUCCESS(*حالت_2)) {
        *حالت_2 = U_UNSUPPORTED_ERROR;
    }
}

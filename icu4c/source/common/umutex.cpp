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

#అన్తర్భావ "umutex.h"

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "cmemory.h"

U_NAMESPACE_BEGIN


#ఒకవేళ defined(U_USER_MUTEX_CPP)
// Support for including an alternate implementation of mutexes has been withdrawn.
// See issue ICU-20185.
#దోషం U_USER_MUTEX_CPP not supported
#అయితే_ముగింపు


/*************************************************************************************************
 *
 *  ICU Mutex wrappers.
 *
 *************************************************************************************************/

నామాకాశ {
std::mutex *initMutex;
std::condition_variable *initCondition;

// The ICU global mutex.
// Used when ICU implementation code passes nullptr for the mutex pointer.
UMutex globalMutex;

std::once_flag initFlag;
std::once_flag *pInitFlag = &initFlag;

}  // Anonymous namespace

U_CDECL_BEGIN
స్థిర UBool U_CALLCONV umtx_cleanup() {
    initMutex->~mutex();
    initCondition->~condition_variable();
    UMutex::cleanup();

    // Reset the once_flag, by destructing it and creating a fresh one in its place.
    // Do not use this trick anywhere else in ICU; use umtx_initOnce, not std::call_once().
    pInitFlag->~once_flag();
    pInitFlag = కొత్త(&initFlag) std::once_flag();
    తిరిగి నిజం;
}

స్థిర శూన్య U_CALLCONV umtx_init() {
    initMutex = STATIC_NEW(std::mutex);
    initCondition = STATIC_NEW(std::condition_variable);
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_MUTEX, umtx_cleanup);
}
U_CDECL_చివర


std::mutex *UMutex::getMutex() {
    std::mutex *retPtr = fMutex.load(std::memory_order_acquire);
    ఒకవేళ (retPtr == శూన్యసూచిక) {
        std::call_once(*pInitFlag, umtx_init);
        std::lock_guard<std::mutex> guard(*initMutex);
        retPtr = fMutex.load(std::memory_order_acquire);
        ఒకవేళ (retPtr == శూన్యసూచిక) {
            fMutex = కొత్త(fStorage) std::mutex();
            retPtr = fMutex;
            fListLink = g_List_తల;
            g_List_తల = ఇది;
        }
    }
    U_ASSERT(retPtr != శూన్యసూచిక);
    తిరిగి retPtr;
}

UMutex *UMutex::g_List_తల = శూన్యసూచిక;

శూన్య UMutex::cleanup() {
    UMutex *next = శూన్యసూచిక;
    కొరకు (UMutex *m = g_List_తల; m != శూన్యసూచిక; m = next) {
        (*m->fMutex).~mutex();
        m->fMutex = శూన్యసూచిక;
        next = m->fListLink;
        m->fListLink = శూన్యసూచిక;
    }
    g_List_తల = శూన్యసూచిక;
}


U_CAPI శూన్య  U_EXPORT2
umtx_తాళం(UMutex *mutex) {
    ఒకవేళ (mutex == శూన్యసూచిక) {
        mutex = &globalMutex;
    }
    mutex->తాళం();
}


U_CAPI శూన్య  U_EXPORT2
umtx_unlock(UMutex* mutex)
{
    ఒకవేళ (mutex == శూన్యసూచిక) {
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
    std::unique_lock<std::mutex> తాళం(*initMutex);
    ఒకవేళ (umtx_ఎక్కించు_Acquire(uio.fState) == 0) {
        umtx_storeRelease(uio.fState, 1);
        తిరిగి నిజం;      // Caller will next call the init function.
    } లేకపోతే {
        అయితే (umtx_ఎక్కించు_Acquire(uio.fState) == 1) {
            // Another thread is currently running the initialization.
            // Wait until it completes.
            initCondition->wait(తాళం);
        }
        U_ASSERT(uio.fState == 2);
        తిరిగి అబద్ధం;
    }
}


// This function is called by the thread that ran an initialization function,
// just after completing the function.
//   Some threads may be waiting on the condition, requiring the broadcast wakeup.
//   Some threads may be racing to test the fState variable outside of the mutex,
//   requiring the use of store/release when changing its value.

U_COMMON_API శూన్య U_EXPORT2
umtx_initImplPostInit(UInitOnce &uio) {
    {
        std::unique_lock<std::mutex> తాళం(*initMutex);
        umtx_storeRelease(uio.fState, 2);
    }
    initCondition->notify_all();
}

U_NAMESPACE_చివర

/*************************************************************************************************
 *
 *  Deprecated functions for setting user mutexes.
 *
 *************************************************************************************************/

U_DEPRECATED శూన్య U_EXPORT2
u_సమితి_Mutex_Functions(స్థిరరాశి శూన్య * /*context */, UMtxInitFn *, UMtxFn *,
                    UMtxFn *,  UMtxFn *, UError_సంకేతం *స్థితి_2) {
    ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
    }
}



U_DEPRECATED శూన్య U_EXPORT2
u_సమితి_Atomic_Inc_Dec_Functions(స్థిరరాశి శూన్య * /*context */, UMtxAtomicFn *, UMtxAtomicFn *,
                           UError_సంకేతం *స్థితి_2) {
    ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
    }
}

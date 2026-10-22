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
#error U_USER_MUTEX_CPP not supported
#endif


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
    ucln_common_registerCleanup(UCLN_COMMON_MUTEX, umtx_cleanup);
}
U_CDECL_END


std::mutex *UMutex::getMutex() {
    std::mutex *retPtr = fMutex.load(std::memory_order_acquire);
    यदि (retPtr == nullptr) {
        std::call_once(*pInitFlag, umtx_init);
        std::lock_guard<std::mutex> guard(*initMutex);
        retPtr = fMutex.load(std::memory_order_acquire);
        यदि (retPtr == nullptr) {
            fMutex = नूतन(fStorage) std::mutex();
            retPtr = fMutex;
            fListLink = gListHead;
            gListHead = एतत्;
        }
    }
    U_ASSERT(retPtr != nullptr);
    प्रत्यागम retPtr;
}

UMutex *UMutex::gListHead = nullptr;

शून्य UMutex::cleanup() {
    UMutex *next = nullptr;
    प्रति (UMutex *m = gListHead; m != nullptr; m = next) {
        (*m->fMutex).~mutex();
        m->fMutex = nullptr;
        next = m->fListLink;
        m->fListLink = nullptr;
    }
    gListHead = nullptr;
}


U_CAPI शून्य  U_EXPORT2
umtx_lock(UMutex *mutex) {
    यदि (mutex == nullptr) {
        mutex = &globalMutex;
    }
    mutex->lock();
}


U_CAPI शून्य  U_EXPORT2
umtx_unlock(UMutex* mutex)
{
    यदि (mutex == nullptr) {
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
    std::unique_lock<std::mutex> lock(*initMutex);
    यदि (umtx_loadAcquire(uio.fState) == 0) {
        umtx_storeRelease(uio.fState, 1);
        प्रत्यागम सत्य;      // Caller will next call the init function.
    } अन्यथा {
        यावत् (umtx_loadAcquire(uio.fState) == 1) {
            // Another thread is currently running the initialization.
            // Wait until it completes.
            initCondition->रुको(lock);
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
        std::unique_lock<std::mutex> lock(*initMutex);
        umtx_storeRelease(uio.fState, 2);
    }
    initCondition->notify_all();
}

U_NAMESPACE_END

/*************************************************************************************************
 *
 *  Deprecated functions for setting user mutexes.
 *
 *************************************************************************************************/

U_DEPRECATED शून्य U_EXPORT2
u_setMutexFunctions(ध्रुव शून्य * /*context */, UMtxInitFn *, UMtxFn *,
                    UMtxFn *,  UMtxFn *, UErrorCode *status) {
    यदि (U_SUCCESS(*status)) {
        *status = U_UNSUPPORTED_ERROR;
    }
}



U_DEPRECATED शून्य U_EXPORT2
u_setAtomicIncDecFunctions(ध्रुव शून्य * /*context */, UMtxAtomicFn *, UMtxAtomicFn *,
                           UErrorCode *status) {
    यदि (U_SUCCESS(*status)) {
        *status = U_UNSUPPORTED_ERROR;
    }
}

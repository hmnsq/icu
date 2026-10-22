// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1997-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
* File UMUTEX.H
*
* Modification History:
*
*   Date        Name        Description
*   04/02/97  aliu        Creation.
*   04/07/99  srl         rewrite - C interface, multiple mutices
*   05/13/99  stephen     Changed to umutex (from cmutex)
******************************************************************************
*/

#ifndef UMUTEX_H
#define UMUTEX_H

#ಅನ್ತರ್ಭಾವ <atomic>
#ಅನ್ತರ್ಭಾವ <condition_variable>
#ಅನ್ತರ್ಭಾವ <mutex>
#ಅನ್ತರ್ಭಾವ <type_traits>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uclean.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

#ಅನ್ತರ್ಭಾವ "putilimp.h"

#ಯದಿ defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
// Support for including an alternate implementation of atomic & mutex operations has been withdrawn.
// See issue ICU-20185.
#error U_USER_ATOMICS and U_USER_MUTEX_H are not supported
#endif

U_NAMESPACE_BEGIN

/****************************************************************************
 *
 *   Low Level Atomic Operations, ICU wrappers for.
 *
 ****************************************************************************/

ಪ್ರಕಾರನಿರ್ದೇಶ std::atomic<int32_t> u_atomic_int32_t;

ಅನ್ತಃಸ್ಥ int32_t umtx_loadAcquire(u_atomic_int32_t &var) {
    ಪ್ರತ್ಯಾಗಮ var.load(std::memory_order_acquire);
}

ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::memory_order_release);
}

ಅನ್ತಃಸ್ಥ int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    ಪ್ರತ್ಯಾಗಮ var->fetch_add(1) + 1;
}

ಅನ್ತಃಸ್ಥ int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    ಪ್ರತ್ಯಾಗಮ var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

ಸಂರಚನಾ U_COMMON_API_CLASS UInitOnce {
ಗುಪ್ತ:
    friend U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    friend U_COMMON_API ಶೂನ್ಯ U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    template <typename T> friend ಶೂನ್ಯ umtx_initOnce(UInitOnce&, T*, ಶೂನ್ಯ (T::*)());
    friend ಶೂನ್ಯ umtx_initOnce(UInitOnce&, ಶೂನ್ಯ (*)());
    friend ಶೂನ್ಯ umtx_initOnce(UInitOnce&, ಶೂನ್ಯ (*)(UErrorCode&), UErrorCode&);
    template <typename T> friend ಶೂನ್ಯ umtx_initOnce(UInitOnce&, ಶೂನ್ಯ (*)(T), T);
    template <typename T> friend ಶೂನ್ಯ umtx_initOnce(UInitOnce&, ಶೂನ್ಯ (*)(T, UErrorCode&), T, UErrorCode&);

    u_atomic_int32_t fState{0};
    UErrorCode fErrCode{U_ZERO_ERROR};

ಸಾರ್ವಜನಿಕ:
    U_COMMON_API ಶೂನ್ಯ reset() { fState = 0; }
    U_COMMON_API UBool isReset() { ಪ್ರತ್ಯಾಗಮ umtx_loadAcquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API ಶೂನ್ಯ  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

template<class T> ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, T *obj, ಶೂನ್ಯ (U_CALLCONV T::*fp)()) {
    ಯದಿ (umtx_loadAcquire(uio.fState) == 2) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, ಶೂನ್ಯ (U_CALLCONV *fp)()) {
    ಯದಿ (umtx_loadAcquire(uio.fState) == 2) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, ಶೂನ್ಯ (U_CALLCONV *fp)(UErrorCode &), UErrorCode &errCode) {
    ಯದಿ (U_FAILURE(errCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } ಅನ್ಯಥಾ {
        // Someone else already ran the initialization.
        ಯದಿ (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
template<class T> ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, ಶೂನ್ಯ (U_CALLCONV *fp)(T), T context) {
    ಯದಿ (umtx_loadAcquire(uio.fState) == 2) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
template<class T> ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, ಶೂನ್ಯ (U_CALLCONV *fp)(T, UErrorCode &), T context, UErrorCode &errCode) {
    ಯದಿ (U_FAILURE(errCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } ಅನ್ಯಥಾ {
        // Someone else already ran the initialization.
        ಯದಿ (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#ಯದಿ (defined(_CPPLIB_VER) && !defined(_MSVC_STL_VERSION)) || \
    (defined(_MSVC_STL_VERSION) && _MSVC_STL_VERSION < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#ಅನ್ಯಥಾ
#   define UMUTEX_CONSTEXPR constexpr
#endif

/**
 * UMutex - ICU Mutex class.
 *
 * This is the preferred Mutex class for use within ICU implementation code.
 * It is a thin wrapper over C++ std::mutex, with these additions:
 *    - Static instances are safe, not triggering static construction or destruction,
 *      and the associated order of construction or destruction issues.
 *    - Plumbed into u_cleanup() for destructing the underlying std::mutex,
 *      which frees any OS level resources they may be holding.
 *
 * Limitations:
 *    - Static or global instances only. Cannot be heap allocated. Cannot appear as a
 *      member of another class.
 *    - No condition variables or other advanced features. If needed, you will need to use
 *      std::mutex and std::condition_variable directly. For an example, see unifiedcache.cpp
 *
 * Typical Usage:
 *    static UMutex myMutex;
 *
 *    {
 *       Mutex lock(myMutex);
 *       ...    // Do stuff that is protected by myMutex;
 *    }         // myMutex is released when lock goes out of scope.
 */

class U_COMMON_API_CLASS UMutex {
ಸಾರ್ವಜನಿಕ:
    U_COMMON_API UMUTEX_CONSTEXPR UMutex() {}
    U_COMMON_API ~UMutex() = ಸ್ವಭಾವ;

    UMutex(ಧ್ರುವ UMutex& other) = delete;
    UMutex& operator=(ಧ್ರುವ UMutex& other) = delete;
    ಶೂನ್ಯ* operator new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API ಶೂನ್ಯ lock() {
        std::mutex *m = fMutex.load(std::memory_order_acquire);
        ಯದಿ (m == nullptr) { m = getMutex(); }
        m->lock();
    }
    U_COMMON_API ಶೂನ್ಯ unlock() { fMutex.load(std::memory_order_relaxed)->unlock(); }

    U_COMMON_API ಸ್ಥಿರ ಶೂನ್ಯ cleanup();

ಗುಪ್ತ:
    alignas(std::mutex) ವರ್ಣ fStorage[ಪರಿಮಾಣ(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    ಸ್ಥಿರ UMutex *gListHead;

    /** Out-of-line function to lazily initialize a UMutex on first use.
     * Initial fast check is inline, in lock().  The returned value may never
     * be nullptr.
     */
    std::mutex *getMutex();
};


/* Lock a mutex.
 * @param mutex The given mutex to be locked.  Pass NULL to specify
 *              the global ICU mutex.  Recursive locks are an error
 *              and may cause a deadlock on some platforms.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2 umtx_lock(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_END

#endif /* UMUTEX_H */
/*eof*/

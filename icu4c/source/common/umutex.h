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

#অন্তর্ভাৱ <atomic>
#অন্তর্ভাৱ <condition_variable>
#অন্তর্ভাৱ <mutex>
#অন্তর্ভাৱ <type_traits>

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/uclean.h"
#অন্তর্ভাৱ "unicode/uobject.h"

#অন্তর্ভাৱ "putilimp.h"

#যদি defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
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

প্রকারনির্দেশ std::atomic<int32_t> u_atomic_int32_t;

অন্তঃস্থ int32_t umtx_loadAcquire(u_atomic_int32_t &var) {
    ঘূৰাই var.load(std::memory_order_acquire);
}

অন্তঃস্থ শূন্য umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::memory_order_release);
}

অন্তঃস্থ int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    ঘূৰাই var->fetch_add(1) + 1;
}

অন্তঃস্থ int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    ঘূৰাই var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

সংরচনা U_COMMON_API_CLASS UInitOnce {
গুপ্ত:
    friend U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    friend U_COMMON_API শূন্য U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    template <typename T> friend শূন্য umtx_initOnce(UInitOnce&, T*, শূন্য (T::*)());
    friend শূন্য umtx_initOnce(UInitOnce&, শূন্য (*)());
    friend শূন্য umtx_initOnce(UInitOnce&, শূন্য (*)(UErrorCode&), UErrorCode&);
    template <typename T> friend শূন্য umtx_initOnce(UInitOnce&, শূন্য (*)(T), T);
    template <typename T> friend শূন্য umtx_initOnce(UInitOnce&, শূন্য (*)(T, UErrorCode&), T, UErrorCode&);

    u_atomic_int32_t fState{0};
    UErrorCode fErrCode{U_ZERO_ERROR};

সার্ৱজনিক:
    U_COMMON_API শূন্য reset() { fState = 0; }
    U_COMMON_API UBool isReset() { ঘূৰাই umtx_loadAcquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API শূন্য  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

template<class T> শূন্য umtx_initOnce(UInitOnce &uio, T *obj, শূন্য (U_CALLCONV T::*fp)()) {
    যদি (umtx_loadAcquire(uio.fState) == 2) {
        ঘূৰাই;
    }
    যদি (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
অন্তঃস্থ শূন্য umtx_initOnce(UInitOnce &uio, শূন্য (U_CALLCONV *fp)()) {
    যদি (umtx_loadAcquire(uio.fState) == 2) {
        ঘূৰাই;
    }
    যদি (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
অন্তঃস্থ শূন্য umtx_initOnce(UInitOnce &uio, শূন্য (U_CALLCONV *fp)(UErrorCode &), UErrorCode &errCode) {
    যদি (U_FAILURE(errCode)) {
        ঘূৰাই;
    }
    যদি (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } অন্যথা {
        // Someone else already ran the initialization.
        যদি (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
template<class T> শূন্য umtx_initOnce(UInitOnce &uio, শূন্য (U_CALLCONV *fp)(T), T context) {
    যদি (umtx_loadAcquire(uio.fState) == 2) {
        ঘূৰাই;
    }
    যদি (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
template<class T> শূন্য umtx_initOnce(UInitOnce &uio, শূন্য (U_CALLCONV *fp)(T, UErrorCode &), T context, UErrorCode &errCode) {
    যদি (U_FAILURE(errCode)) {
        ঘূৰাই;
    }
    যদি (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } অন্যথা {
        // Someone else already ran the initialization.
        যদি (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#যদি (defined(_CPPLIB_VER) && !defined(_MSVC_STL_VERSION)) || \
    (defined(_MSVC_STL_VERSION) && _MSVC_STL_VERSION < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#অন্যথা
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
সার্ৱজনিক:
    U_COMMON_API UMUTEX_CONSTEXPR UMutex() {}
    U_COMMON_API ~UMutex() = স্ৱভাৱ;

    UMutex(ধ্রুৱ UMutex& other) = delete;
    UMutex& operator=(ধ্রুৱ UMutex& other) = delete;
    শূন্য* operator new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API শূন্য lock() {
        std::mutex *m = fMutex.load(std::memory_order_acquire);
        যদি (m == nullptr) { m = getMutex(); }
        m->lock();
    }
    U_COMMON_API শূন্য unlock() { fMutex.load(std::memory_order_relaxed)->unlock(); }

    U_COMMON_API স্থির শূন্য cleanup();

গুপ্ত:
    alignas(std::mutex) ৱর্ণ fStorage[পরিমাণ(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    স্থির UMutex *gListHead;

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
U_CAPI শূন্য U_EXPORT2 umtx_lock(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI শূন্য U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_END

#endif /* UMUTEX_H */
/*eof*/

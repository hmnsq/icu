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

#अन्तर्भाव <atomic>
#अन्तर्भाव <condition_variable>
#अन्तर्भाव <mutex>
#अन्तर्भाव <type_traits>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uclean.h"
#अन्तर्भाव "unicode/uobject.h"

#अन्तर्भाव "putilimp.h"

#सचे defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
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

प्रकारनिर्देश std::atomic<int32_t> u_atomic_int32_t;

अन्तःस्थ int32_t umtx_loadAcquire(u_atomic_int32_t &var) {
    पटिनिवत्त var.load(std::memory_order_acquire);
}

अन्तःस्थ शून्य umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::memory_order_release);
}

अन्तःस्थ int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    पटिनिवत्त var->fetch_add(1) + 1;
}

अन्तःस्थ int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    पटिनिवत्त var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

संरचना U_COMMON_API_CLASS UInitOnce {
गुप्त:
    friend U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    friend U_COMMON_API शून्य U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    template <typename T> friend शून्य umtx_initOnce(UInitOnce&, T*, शून्य (T::*)());
    friend शून्य umtx_initOnce(UInitOnce&, शून्य (*)());
    friend शून्य umtx_initOnce(UInitOnce&, शून्य (*)(UErrorCode&), UErrorCode&);
    template <typename T> friend शून्य umtx_initOnce(UInitOnce&, शून्य (*)(T), T);
    template <typename T> friend शून्य umtx_initOnce(UInitOnce&, शून्य (*)(T, UErrorCode&), T, UErrorCode&);

    u_atomic_int32_t fState{0};
    UErrorCode fErrCode{U_ZERO_ERROR};

सार्वजनिक:
    U_COMMON_API शून्य reset() { fState = 0; }
    U_COMMON_API UBool isReset() { पटिनिवत्त umtx_loadAcquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API शून्य  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

template<class T> शून्य umtx_initOnce(UInitOnce &uio, T *obj, शून्य (U_CALLCONV T::*fp)()) {
    सचे (umtx_loadAcquire(uio.fState) == 2) {
        पटिनिवत्त;
    }
    सचे (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
अन्तःस्थ शून्य umtx_initOnce(UInitOnce &uio, शून्य (U_CALLCONV *fp)()) {
    सचे (umtx_loadAcquire(uio.fState) == 2) {
        पटिनिवत्त;
    }
    सचे (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
अन्तःस्थ शून्य umtx_initOnce(UInitOnce &uio, शून्य (U_CALLCONV *fp)(UErrorCode &), UErrorCode &errCode) {
    सचे (U_FAILURE(errCode)) {
        पटिनिवत्त;
    }
    सचे (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } अञ्ञथा {
        // Someone else already ran the initialization.
        सचे (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
template<class T> शून्य umtx_initOnce(UInitOnce &uio, शून्य (U_CALLCONV *fp)(T), T context) {
    सचे (umtx_loadAcquire(uio.fState) == 2) {
        पटिनिवत्त;
    }
    सचे (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
template<class T> शून्य umtx_initOnce(UInitOnce &uio, शून्य (U_CALLCONV *fp)(T, UErrorCode &), T context, UErrorCode &errCode) {
    सचे (U_FAILURE(errCode)) {
        पटिनिवत्त;
    }
    सचे (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } अञ्ञथा {
        // Someone else already ran the initialization.
        सचे (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#सचे (defined(_CPPLIB_VER) && !defined(_MSVC_STL_VERSION)) || \
    (defined(_MSVC_STL_VERSION) && _MSVC_STL_VERSION < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#अञ्ञथा
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
सार्वजनिक:
    U_COMMON_API UMUTEX_CONSTEXPR UMutex() {}
    U_COMMON_API ~UMutex() = स्वभाव;

    UMutex(ध्रुव UMutex& other) = delete;
    UMutex& operator=(ध्रुव UMutex& other) = delete;
    शून्य* operator new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API शून्य lock() {
        std::mutex *m = fMutex.load(std::memory_order_acquire);
        सचे (m == nullptr) { m = getMutex(); }
        m->lock();
    }
    U_COMMON_API शून्य unlock() { fMutex.load(std::memory_order_relaxed)->unlock(); }

    U_COMMON_API स्थिर शून्य cleanup();

गुप्त:
    alignas(std::mutex) वर्ण fStorage[परिमाण(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    स्थिर UMutex *gListHead;

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
U_CAPI शून्य U_EXPORT2 umtx_lock(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI शून्य U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_END

#endif /* UMUTEX_H */
/*eof*/

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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UMUTEX_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UMUTEX_H

#ਅਨ੍ਤਰ੍ਭਾਵ <atomic>
#ਅਨ੍ਤਰ੍ਭਾਵ <condition_variable>
#ਅਨ੍ਤਰ੍ਭਾਵ <mutex>
#ਅਨ੍ਤਰ੍ਭਾਵ <type_traits>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uclean.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"

#ਜੇ defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
// Support for including an alternate implementation of atomic & mutex operations has been withdrawn.
// See issue ICU-20185.
#ਗਲਤੀ U_USER_ATOMICS and U_USER_MUTEX_H are not supported
#ਜੇ_ਅੰਤ

U_NAMESPACE_BEGIN

/****************************************************************************
 *
 *   Low Level Atomic Operations, ICU wrappers for.
 *
 ****************************************************************************/

ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::atomic<int32_t> u_atomic_int32_t;

ਸਤਰਅੰਦਰ int32_t umtx_ਲੱਦੋ_Acquire(u_atomic_int32_t &var) {
    ਵਾਪਸ var.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_acquire);
}

ਸਤਰਅੰਦਰ ਖਾਲੀ umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_release);
}

ਸਤਰਅੰਦਰ int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    ਵਾਪਸ var->fetch_ਜੋੜੋ(1) + 1;
}

ਸਤਰਅੰਦਰ int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    ਵਾਪਸ var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

ਬਣਤਰ U_COMMON_API_CLASS UInitOnce {
ਨਿਜੀ:
    ਮਿੱਤਰ U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    ਮਿੱਤਰ U_COMMON_API ਖਾਲੀ U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    ਸਾਂਚਾ <typename T> ਮਿੱਤਰ ਖਾਲੀ umtx_initOnce(UInitOnce&, T*, ਖਾਲੀ (T::*)());
    ਮਿੱਤਰ ਖਾਲੀ umtx_initOnce(UInitOnce&, ਖਾਲੀ (*)());
    ਮਿੱਤਰ ਖਾਲੀ umtx_initOnce(UInitOnce&, ਖਾਲੀ (*)(UError_ਕੋਡ&), UError_ਕੋਡ&);
    ਸਾਂਚਾ <typename T> ਮਿੱਤਰ ਖਾਲੀ umtx_initOnce(UInitOnce&, ਖਾਲੀ (*)(T), T);
    ਸਾਂਚਾ <typename T> ਮਿੱਤਰ ਖਾਲੀ umtx_initOnce(UInitOnce&, ਖਾਲੀ (*)(T, UError_ਕੋਡ&), T, UError_ਕੋਡ&);

    u_atomic_int32_t fState{0};
    UError_ਕੋਡ f_Err_ਕੋਡ{U_ZERO_ERROR};

ਜਨਤਕ:
    U_COMMON_API ਖਾਲੀ reset() { fState = 0; }
    U_COMMON_API UBool isReset() { ਵਾਪਸ umtx_ਲੱਦੋ_Acquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API ਖਾਲੀ  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

ਸਾਂਚਾ<class T> ਖਾਲੀ umtx_initOnce(UInitOnce &uio, T *obj, ਖਾਲੀ (U_CALLCONV T::*fp)()) {
    ਜੇ (umtx_ਲੱਦੋ_Acquire(uio.fState) == 2) {
        ਵਾਪਸ;
    }
    ਜੇ (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
ਸਤਰਅੰਦਰ ਖਾਲੀ umtx_initOnce(UInitOnce &uio, ਖਾਲੀ (U_CALLCONV *fp)()) {
    ਜੇ (umtx_ਲੱਦੋ_Acquire(uio.fState) == 2) {
        ਵਾਪਸ;
    }
    ਜੇ (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
ਸਤਰਅੰਦਰ ਖਾਲੀ umtx_initOnce(UInitOnce &uio, ਖਾਲੀ (U_CALLCONV *fp)(UError_ਕੋਡ &), UError_ਕੋਡ &err_ਕੋਡ) {
    ਜੇ (U_FAILURE(err_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ (umtx_ਲੱਦੋ_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(err_ਕੋਡ);
        uio.f_Err_ਕੋਡ = err_ਕੋਡ;
        umtx_initImplPostInit(uio);
    } ਨਹੀਂਤਾਂ {
        // Someone else already ran the initialization.
        ਜੇ (U_FAILURE(uio.f_Err_ਕੋਡ)) {
            err_ਕੋਡ = uio.f_Err_ਕੋਡ;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
ਸਾਂਚਾ<class T> ਖਾਲੀ umtx_initOnce(UInitOnce &uio, ਖਾਲੀ (U_CALLCONV *fp)(T), T context) {
    ਜੇ (umtx_ਲੱਦੋ_Acquire(uio.fState) == 2) {
        ਵਾਪਸ;
    }
    ਜੇ (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
ਸਾਂਚਾ<class T> ਖਾਲੀ umtx_initOnce(UInitOnce &uio, ਖਾਲੀ (U_CALLCONV *fp)(T, UError_ਕੋਡ &), T context, UError_ਕੋਡ &err_ਕੋਡ) {
    ਜੇ (U_FAILURE(err_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਜੇ (umtx_ਲੱਦੋ_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, err_ਕੋਡ);
        uio.f_Err_ਕੋਡ = err_ਕੋਡ;
        umtx_initImplPostInit(uio);
    } ਨਹੀਂਤਾਂ {
        // Someone else already ran the initialization.
        ਜੇ (U_FAILURE(uio.f_Err_ਕੋਡ)) {
            err_ਕੋਡ = uio.f_Err_ਕੋਡ;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#ਜੇ (defined(_CPPLIB_VER) && !defined(MSVC_STL_ਸੰਸਕਰਣ)) || \
    (defined(MSVC_STL_ਸੰਸਕਰਣ) && MSVC_STL_ਸੰਸਕਰਣ < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#ਨਹੀਂਤਾਂ
#   define UMUTEX_CONSTEXPR constexpr
#ਜੇ_ਅੰਤ

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
ਜਨਤਕ:
    U_COMMON_API UMUTEX_CONSTEXPR UMutex() {}
    U_COMMON_API ~UMutex() = ਮੂਲ;

    UMutex(ਅਚਲ UMutex& other) = delete;
    UMutex& ਸੰਚਾਲਕ=(ਅਚਲ UMutex& other) = delete;
    ਖਾਲੀ* ਸੰਚਾਲਕ new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API ਖਾਲੀ ਤਾਲਾ() {
        std::mutex *m = fMutex.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_acquire);
        ਜੇ (m == nullptr) { m = getMutex(); }
        m->ਤਾਲਾ();
    }
    U_COMMON_API ਖਾਲੀ unlock() { fMutex.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed)->unlock(); }

    U_COMMON_API ਸਥਿਰ ਖਾਲੀ cleanup();

ਨਿਜੀ:
    alignas(std::mutex) ਅੱਖਰ fStorage[ਆਕਾਰ(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    ਸਥਿਰ UMutex *g_List_ਸਿਰ;

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
U_CAPI ਖਾਲੀ U_EXPORT2 umtx_ਤਾਲਾ(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI ਖਾਲੀ U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* UMUTEX_H */
/*eof*/

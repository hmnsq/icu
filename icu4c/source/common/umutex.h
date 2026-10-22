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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UMUTEX_H
#ವ್ಯಾಖ್ಯಾನಿಸು UMUTEX_H

#ಅನ್ತರ್ಭಾವ <atomic>
#ಅನ್ತರ್ಭಾವ <condition_variable>
#ಅನ್ತರ್ಭಾವ <mutex>
#ಅನ್ತರ್ಭಾವ <type_traits>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uclean.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

#ಅನ್ತರ್ಭಾವ "putilimp.h"

#ಆದರೆ defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
// Support for including an alternate implementation of atomic & mutex operations has been withdrawn.
// See issue ICU-20185.
#ದೋಷ U_USER_ATOMICS and U_USER_MUTEX_H are not supported
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_BEGIN

/****************************************************************************
 *
 *   Low Level Atomic Operations, ICU wrappers for.
 *
 ****************************************************************************/

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::atomic<int32_t> u_atomic_int32_t;

ಸಾಲೊಳಗೆ int32_t umtx_ತುಂಬು_Acquire(u_atomic_int32_t &var) {
    ಹಿಂತಿರುಗಿ var.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_acquire);
}

ಸಾಲೊಳಗೆ ಶೂನ್ಯ umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::ಸ್ಮರಣೆ_ಕ್ರಮ_release);
}

ಸಾಲೊಳಗೆ int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    ಹಿಂತಿರುಗಿ var->fetch_ಕೂಡಿಸು(1) + 1;
}

ಸಾಲೊಳಗೆ int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    ಹಿಂತಿರುಗಿ var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

ರಚನೆ U_COMMON_API_CLASS UInitOnce {
ಖಾಸಗಿ:
    ಮಿತ್ರ U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    ಮಿತ್ರ U_COMMON_API ಶೂನ್ಯ U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    ಮಾದರಿ <typename T> ಮಿತ್ರ ಶೂನ್ಯ umtx_initOnce(UInitOnce&, T*, ಶೂನ್ಯ (T::*)());
    ಮಿತ್ರ ಶೂನ್ಯ umtx_initOnce(UInitOnce&, ಶೂನ್ಯ (*)());
    ಮಿತ್ರ ಶೂನ್ಯ umtx_initOnce(UInitOnce&, ಶೂನ್ಯ (*)(UError_ಸಂಕೇತ&), UError_ಸಂಕೇತ&);
    ಮಾದರಿ <typename T> ಮಿತ್ರ ಶೂನ್ಯ umtx_initOnce(UInitOnce&, ಶೂನ್ಯ (*)(T), T);
    ಮಾದರಿ <typename T> ಮಿತ್ರ ಶೂನ್ಯ umtx_initOnce(UInitOnce&, ಶೂನ್ಯ (*)(T, UError_ಸಂಕೇತ&), T, UError_ಸಂಕೇತ&);

    u_atomic_int32_t fState{0};
    UError_ಸಂಕೇತ f_Err_ಸಂಕೇತ{U_ZERO_ERROR};

ಸಾರ್ವಜನಿಕ:
    U_COMMON_API ಶೂನ್ಯ reset() { fState = 0; }
    U_COMMON_API UBool isReset() { ಹಿಂತಿರುಗಿ umtx_ತುಂಬು_Acquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API ಶೂನ್ಯ  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

ಮಾದರಿ<class T> ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, T *obj, ಶೂನ್ಯ (U_CALLCONV T::*fp)()) {
    ಆದರೆ (umtx_ತುಂಬು_Acquire(uio.fState) == 2) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
ಸಾಲೊಳಗೆ ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, ಶೂನ್ಯ (U_CALLCONV *fp)()) {
    ಆದರೆ (umtx_ತುಂಬು_Acquire(uio.fState) == 2) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
ಸಾಲೊಳಗೆ ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, ಶೂನ್ಯ (U_CALLCONV *fp)(UError_ಸಂಕೇತ &), UError_ಸಂಕೇತ &err_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(err_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (umtx_ತುಂಬು_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(err_ಸಂಕೇತ);
        uio.f_Err_ಸಂಕೇತ = err_ಸಂಕೇತ;
        umtx_initImplPostInit(uio);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Someone else already ran the initialization.
        ಆದರೆ (U_FAILURE(uio.f_Err_ಸಂಕೇತ)) {
            err_ಸಂಕೇತ = uio.f_Err_ಸಂಕೇತ;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
ಮಾದರಿ<class T> ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, ಶೂನ್ಯ (U_CALLCONV *fp)(T), T context) {
    ಆದರೆ (umtx_ತುಂಬು_Acquire(uio.fState) == 2) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
ಮಾದರಿ<class T> ಶೂನ್ಯ umtx_initOnce(UInitOnce &uio, ಶೂನ್ಯ (U_CALLCONV *fp)(T, UError_ಸಂಕೇತ &), T context, UError_ಸಂಕೇತ &err_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(err_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (umtx_ತುಂಬು_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, err_ಸಂಕೇತ);
        uio.f_Err_ಸಂಕೇತ = err_ಸಂಕೇತ;
        umtx_initImplPostInit(uio);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // Someone else already ran the initialization.
        ಆದರೆ (U_FAILURE(uio.f_Err_ಸಂಕೇತ)) {
            err_ಸಂಕೇತ = uio.f_Err_ಸಂಕೇತ;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#ಆದರೆ (defined(_CPPLIB_VER) && !defined(MSVC_STL_ಆವೃತ್ತಿ)) || \
    (defined(MSVC_STL_ಆವೃತ್ತಿ) && MSVC_STL_ಆವೃತ್ತಿ < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#ಇಲ್ಲದಿದ್ದರೆ
#   define UMUTEX_CONSTEXPR constexpr
#ಆದರೆ_ಅಂತ್ಯ

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
    U_COMMON_API ~UMutex() = ಪೂರ್ವನಿಯೋಜಿತ;

    UMutex(ಸ್ಥಿರಾಂಕ UMutex& other) = delete;
    UMutex& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UMutex& other) = delete;
    ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API ಶೂನ್ಯ ಬೀಗ() {
        std::mutex *m = fMutex.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_acquire);
        ಆದರೆ (m == nullptr) { m = getMutex(); }
        m->ಬೀಗ();
    }
    U_COMMON_API ಶೂನ್ಯ unlock() { fMutex.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed)->unlock(); }

    U_COMMON_API ಸ್ಥಿರ ಶೂನ್ಯ cleanup();

ಖಾಸಗಿ:
    alignas(std::mutex) ಅಕ್ಷರ fStorage[ಗಾತ್ರ(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    ಸ್ಥಿರ UMutex *g_List_ತಲೆ;

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
U_CAPI ಶೂನ್ಯ U_EXPORT2 umtx_ಬೀಗ(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* UMUTEX_H */
/*eof*/

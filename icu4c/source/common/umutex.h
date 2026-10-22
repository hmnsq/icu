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

#جے_بنا_تعریف UMUTEX_H
#تعریف_کرو UMUTEX_H

#ضمن <atomic>
#ضمن <condition_variable>
#ضمن <mutex>
#ضمن <type_traits>

#ضمن "unicode/utypes.h"
#ضمن "unicode/uclean.h"
#ضمن "unicode/uobject.h"

#ضمن "putilimp.h"

#إذا defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
// Support for including an alternate implementation of atomic & mutex operations has been withdrawn.
// See issue ICU-20185.
#غلطی U_USER_ATOMICS and U_USER_MUTEX_H are not supported
#جے_انت

U_NAMESPACE_BEGIN

/****************************************************************************
 *
 *   Low Level Atomic Operations, ICU wrappers for.
 *
 ****************************************************************************/

تعريفنوع std::atomic<int32_t> u_atomic_int32_t;

مضمن int32_t umtx_لدو_Acquire(u_atomic_int32_t &var) {
    أرجع var.لدو(std::حافظہ_ترتیب_acquire);
}

مضمن فراغ umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::حافظہ_ترتیب_release);
}

مضمن int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    أرجع var->fetch_جوڑو(1) + 1;
}

مضمن int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    أرجع var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

بنية U_COMMON_API_CLASS UInitOnce {
خاص:
    یار U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    یار U_COMMON_API فراغ U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    سانچا <typename T> یار فراغ umtx_initOnce(UInitOnce&, T*, فراغ (T::*)());
    یار فراغ umtx_initOnce(UInitOnce&, فراغ (*)());
    یار فراغ umtx_initOnce(UInitOnce&, فراغ (*)(UError_کوڈ&), UError_کوڈ&);
    سانچا <typename T> یار فراغ umtx_initOnce(UInitOnce&, فراغ (*)(T), T);
    سانچا <typename T> یار فراغ umtx_initOnce(UInitOnce&, فراغ (*)(T, UError_کوڈ&), T, UError_کوڈ&);

    u_atomic_int32_t fState{0};
    UError_کوڈ f_Err_کوڈ{U_ZERO_ERROR};

عام:
    U_COMMON_API فراغ reset() { fState = 0; }
    U_COMMON_API UBool isReset() { أرجع umtx_لدو_Acquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API فراغ  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

سانچا<class T> فراغ umtx_initOnce(UInitOnce &uio, T *obj, فراغ (U_CALLCONV T::*fp)()) {
    إذا (umtx_لدو_Acquire(uio.fState) == 2) {
        أرجع;
    }
    إذا (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
مضمن فراغ umtx_initOnce(UInitOnce &uio, فراغ (U_CALLCONV *fp)()) {
    إذا (umtx_لدو_Acquire(uio.fState) == 2) {
        أرجع;
    }
    إذا (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
مضمن فراغ umtx_initOnce(UInitOnce &uio, فراغ (U_CALLCONV *fp)(UError_کوڈ &), UError_کوڈ &err_کوڈ) {
    إذا (U_FAILURE(err_کوڈ)) {
        أرجع;
    }
    إذا (umtx_لدو_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(err_کوڈ);
        uio.f_Err_کوڈ = err_کوڈ;
        umtx_initImplPostInit(uio);
    } وإلا {
        // Someone else already ran the initialization.
        إذا (U_FAILURE(uio.f_Err_کوڈ)) {
            err_کوڈ = uio.f_Err_کوڈ;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
سانچا<class T> فراغ umtx_initOnce(UInitOnce &uio, فراغ (U_CALLCONV *fp)(T), T context) {
    إذا (umtx_لدو_Acquire(uio.fState) == 2) {
        أرجع;
    }
    إذا (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
سانچا<class T> فراغ umtx_initOnce(UInitOnce &uio, فراغ (U_CALLCONV *fp)(T, UError_کوڈ &), T context, UError_کوڈ &err_کوڈ) {
    إذا (U_FAILURE(err_کوڈ)) {
        أرجع;
    }
    إذا (umtx_لدو_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, err_کوڈ);
        uio.f_Err_کوڈ = err_کوڈ;
        umtx_initImplPostInit(uio);
    } وإلا {
        // Someone else already ran the initialization.
        إذا (U_FAILURE(uio.f_Err_کوڈ)) {
            err_کوڈ = uio.f_Err_کوڈ;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#إذا (defined(_CPPLIB_VER) && !defined(MSVC_STL_نسخہ)) || \
    (defined(MSVC_STL_نسخہ) && MSVC_STL_نسخہ < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#وإلا
#   define UMUTEX_CONSTEXPR constexpr
#جے_انت

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
عام:
    U_COMMON_API UMUTEX_CONSTEXPR UMutex() {}
    U_COMMON_API ~UMutex() = افتراضي;

    UMutex(ثابت UMutex& other) = delete;
    UMutex& عامل=(ثابت UMutex& other) = delete;
    فراغ* عامل new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API فراغ جندرا() {
        std::mutex *m = fMutex.لدو(std::حافظہ_ترتیب_acquire);
        إذا (m == nullptr) { m = getMutex(); }
        m->جندرا();
    }
    U_COMMON_API فراغ unlock() { fMutex.لدو(std::حافظہ_ترتیب_relaxed)->unlock(); }

    U_COMMON_API ساكن فراغ cleanup();

خاص:
    alignas(std::mutex) حرف fStorage[حجم(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    ساكن UMutex *g_List_سر;

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
U_CAPI فراغ U_EXPORT2 umtx_جندرا(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI فراغ U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_سرا

#جے_انت /* UMUTEX_H */
/*eof*/

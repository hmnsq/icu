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

#اگر_غیر_تعریف_شدہ UMUTEX_H
#تعریف_کرو UMUTEX_H

#شامل <atomic>
#شامل <condition_variable>
#شامل <mutex>
#شامل <type_traits>

#شامل "unicode/utypes.h"
#شامل "unicode/uclean.h"
#شامل "unicode/uobject.h"

#شامل "putilimp.h"

#اگر defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
// Support for including an alternate implementation of atomic & mutex operations has been withdrawn.
// See issue ICU-20185.
#غلطی U_USER_ATOMICS and U_USER_MUTEX_H are not supported
#اگر_ختم

U_NAMESPACE_BEGIN

/****************************************************************************
 *
 *   Low Level Atomic Operations, ICU wrappers for.
 *
 ****************************************************************************/

قسمتعریف std::atomic<int32_t> u_atomic_int32_t;

اندرونی int32_t umtx_بھرو_Acquire(u_atomic_int32_t &var) {
    واپس var.بھرو(std::حافظہ_ترتیب_acquire);
}

اندرونی خالی umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::حافظہ_ترتیب_release);
}

اندرونی int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    واپس var->fetch_جوڑو(1) + 1;
}

اندرونی int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    واپس var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

ساخت U_COMMON_API_CLASS UInitOnce {
نجی:
    دوست U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    دوست U_COMMON_API خالی U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    سانچہ <typename T> دوست خالی umtx_initOnce(UInitOnce&, T*, خالی (T::*)());
    دوست خالی umtx_initOnce(UInitOnce&, خالی (*)());
    دوست خالی umtx_initOnce(UInitOnce&, خالی (*)(UError_رمز&), UError_رمز&);
    سانچہ <typename T> دوست خالی umtx_initOnce(UInitOnce&, خالی (*)(T), T);
    سانچہ <typename T> دوست خالی umtx_initOnce(UInitOnce&, خالی (*)(T, UError_رمز&), T, UError_رمز&);

    u_atomic_int32_t fState{0};
    UError_رمز f_Err_رمز{U_ZERO_ERROR};

عوامی:
    U_COMMON_API خالی reset() { fState = 0; }
    U_COMMON_API UBool isReset() { واپس umtx_بھرو_Acquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API خالی  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

سانچہ<class T> خالی umtx_initOnce(UInitOnce &uio, T *obj, خالی (U_CALLCONV T::*fp)()) {
    اگر (umtx_بھرو_Acquire(uio.fState) == 2) {
        واپس;
    }
    اگر (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
اندرونی خالی umtx_initOnce(UInitOnce &uio, خالی (U_CALLCONV *fp)()) {
    اگر (umtx_بھرو_Acquire(uio.fState) == 2) {
        واپس;
    }
    اگر (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
اندرونی خالی umtx_initOnce(UInitOnce &uio, خالی (U_CALLCONV *fp)(UError_رمز &), UError_رمز &err_رمز) {
    اگر (U_FAILURE(err_رمز)) {
        واپس;
    }
    اگر (umtx_بھرو_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(err_رمز);
        uio.f_Err_رمز = err_رمز;
        umtx_initImplPostInit(uio);
    } ورنہ {
        // Someone else already ran the initialization.
        اگر (U_FAILURE(uio.f_Err_رمز)) {
            err_رمز = uio.f_Err_رمز;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
سانچہ<class T> خالی umtx_initOnce(UInitOnce &uio, خالی (U_CALLCONV *fp)(T), T context) {
    اگر (umtx_بھرو_Acquire(uio.fState) == 2) {
        واپس;
    }
    اگر (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
سانچہ<class T> خالی umtx_initOnce(UInitOnce &uio, خالی (U_CALLCONV *fp)(T, UError_رمز &), T context, UError_رمز &err_رمز) {
    اگر (U_FAILURE(err_رمز)) {
        واپس;
    }
    اگر (umtx_بھرو_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, err_رمز);
        uio.f_Err_رمز = err_رمز;
        umtx_initImplPostInit(uio);
    } ورنہ {
        // Someone else already ran the initialization.
        اگر (U_FAILURE(uio.f_Err_رمز)) {
            err_رمز = uio.f_Err_رمز;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#اگر (defined(_CPPLIB_VER) && !defined(MSVC_STL_نسخہ)) || \
    (defined(MSVC_STL_نسخہ) && MSVC_STL_نسخہ < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#ورنہ
#   define UMUTEX_CONSTEXPR constexpr
#اگر_ختم

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
عوامی:
    U_COMMON_API UMUTEX_CONSTEXPR UMutex() {}
    U_COMMON_API ~UMutex() = طےشدہ;

    UMutex(ثابت UMutex& other) = delete;
    UMutex& عامل=(ثابت UMutex& other) = delete;
    خالی* عامل new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API خالی تالا() {
        std::mutex *m = fMutex.بھرو(std::حافظہ_ترتیب_acquire);
        اگر (m == nullptr) { m = getMutex(); }
        m->تالا();
    }
    U_COMMON_API خالی unlock() { fMutex.بھرو(std::حافظہ_ترتیب_relaxed)->unlock(); }

    U_COMMON_API ساکن خالی cleanup();

نجی:
    alignas(std::mutex) حرف fStorage[حجم(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    ساکن UMutex *g_List_سر;

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
U_CAPI خالی U_EXPORT2 umtx_تالا(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI خالی U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_انتہا

#اگر_ختم /* UMUTEX_H */
/*eof*/

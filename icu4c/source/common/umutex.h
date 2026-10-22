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

#अगर_अघोषित UMUTEX_H
#घोषणा UMUTEX_H

#समावेश <atomic>
#समावेश <condition_variable>
#समावेश <mutex>
#समावेश <type_traits>

#समावेश "unicode/utypes.h"
#समावेश "unicode/uclean.h"
#समावेश "unicode/uobject.h"

#समावेश "putilimp.h"

#अगर defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
// Support for including an alternate implementation of atomic & mutex operations has been withdrawn.
// See issue ICU-20185.
#त्रुटि U_USER_ATOMICS and U_USER_MUTEX_H are not supported
#पूर्ण_अगर

U_NAMESPACE_BEGIN

/****************************************************************************
 *
 *   Low Level Atomic Operations, ICU wrappers for.
 *
 ****************************************************************************/

प्रकार std::atomic<int32_t> u_atomic_int32_t;

अंतरभूत int32_t umtx_loadAcquire(u_atomic_int32_t &var) {
    वापस var.load(std::memory_order_acquire);
}

अंतरभूत व्योम umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::memory_order_release);
}

अंतरभूत int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    वापस var->fetch_add(1) + 1;
}

अंतरभूत int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    वापस var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

काष्ठा U_COMMON_API_CLASS UInitOnce {
निजी:
    मित्र U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    मित्र U_COMMON_API व्योम U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    ढाँचा <typename T> मित्र व्योम umtx_initOnce(UInitOnce&, T*, व्योम (T::*)());
    मित्र व्योम umtx_initOnce(UInitOnce&, व्योम (*)());
    मित्र व्योम umtx_initOnce(UInitOnce&, व्योम (*)(UErrorCode&), UErrorCode&);
    ढाँचा <typename T> मित्र व्योम umtx_initOnce(UInitOnce&, व्योम (*)(T), T);
    ढाँचा <typename T> मित्र व्योम umtx_initOnce(UInitOnce&, व्योम (*)(T, UErrorCode&), T, UErrorCode&);

    u_atomic_int32_t fState{0};
    UErrorCode fErrCode{U_ZERO_ERROR};

खुला:
    U_COMMON_API व्योम reset() { fState = 0; }
    U_COMMON_API UBool isReset() { वापस umtx_loadAcquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API व्योम  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

ढाँचा<class T> व्योम umtx_initOnce(UInitOnce &uio, T *obj, व्योम (U_CALLCONV T::*fp)()) {
    अगर (umtx_loadAcquire(uio.fState) == 2) {
        वापस;
    }
    अगर (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
अंतरभूत व्योम umtx_initOnce(UInitOnce &uio, व्योम (U_CALLCONV *fp)()) {
    अगर (umtx_loadAcquire(uio.fState) == 2) {
        वापस;
    }
    अगर (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
अंतरभूत व्योम umtx_initOnce(UInitOnce &uio, व्योम (U_CALLCONV *fp)(UErrorCode &), UErrorCode &errCode) {
    अगर (U_FAILURE(errCode)) {
        वापस;
    }
    अगर (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } अन्यथा {
        // Someone else already ran the initialization.
        अगर (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
ढाँचा<class T> व्योम umtx_initOnce(UInitOnce &uio, व्योम (U_CALLCONV *fp)(T), T context) {
    अगर (umtx_loadAcquire(uio.fState) == 2) {
        वापस;
    }
    अगर (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
ढाँचा<class T> व्योम umtx_initOnce(UInitOnce &uio, व्योम (U_CALLCONV *fp)(T, UErrorCode &), T context, UErrorCode &errCode) {
    अगर (U_FAILURE(errCode)) {
        वापस;
    }
    अगर (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } अन्यथा {
        // Someone else already ran the initialization.
        अगर (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#अगर (defined(_CPPLIB_VER) && !defined(_MSVC_STL_VERSION)) || \
    (defined(_MSVC_STL_VERSION) && _MSVC_STL_VERSION < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#अन्यथा
#   define UMUTEX_CONSTEXPR constexpr
#पूर्ण_अगर

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
खुला:
    U_COMMON_API UMUTEX_CONSTEXPR UMutex() {}
    U_COMMON_API ~UMutex() = शेष;

    UMutex(स्थिर UMutex& other) = delete;
    UMutex& चालक=(स्थिर UMutex& other) = delete;
    व्योम* चालक new(माप_प्रकार) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API व्योम lock() {
        std::mutex *m = fMutex.load(std::memory_order_acquire);
        अगर (m == nullptr) { m = getMutex(); }
        m->lock();
    }
    U_COMMON_API व्योम unlock() { fMutex.load(std::memory_order_relaxed)->unlock(); }

    U_COMMON_API अटल व्योम cleanup();

निजी:
    alignas(std::mutex) अक्षर fStorage[माप(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    अटल UMutex *g_List_शीर्ष;

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
U_CAPI व्योम U_EXPORT2 umtx_lock(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI व्योम U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_END

#पूर्ण_अगर /* UMUTEX_H */
/*eof*/

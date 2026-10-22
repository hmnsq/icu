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

#ଅନ୍ତର୍ଭାଵ <atomic>
#ଅନ୍ତର୍ଭାଵ <condition_variable>
#ଅନ୍ତର୍ଭାଵ <mutex>
#ଅନ୍ତର୍ଭାଵ <type_traits>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uclean.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"

#ଅନ୍ତର୍ଭାଵ "putilimp.h"

#ଯଦି defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା std::atomic<int32_t> u_atomic_int32_t;

ଧାଡ଼ିଭିତରେ int32_t umtx_loadAcquire(u_atomic_int32_t &var) {
    ଫେରନ୍ତୁ var.load(std::memory_order_acquire);
}

ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::memory_order_release);
}

ଧାଡ଼ିଭିତରେ int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    ଫେରନ୍ତୁ var->fetch_add(1) + 1;
}

ଧାଡ଼ିଭିତରେ int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    ଫେରନ୍ତୁ var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

ସଂରଚନା U_COMMON_API_CLASS UInitOnce {
ବ୍ୟକ୍ତିଗତ:
    friend U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    friend U_COMMON_API ଶୂନ୍ୟ U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    template <typename T> friend ଶୂନ୍ୟ umtx_initOnce(UInitOnce&, T*, ଶୂନ୍ୟ (T::*)());
    friend ଶୂନ୍ୟ umtx_initOnce(UInitOnce&, ଶୂନ୍ୟ (*)());
    friend ଶୂନ୍ୟ umtx_initOnce(UInitOnce&, ଶୂନ୍ୟ (*)(UErrorCode&), UErrorCode&);
    template <typename T> friend ଶୂନ୍ୟ umtx_initOnce(UInitOnce&, ଶୂନ୍ୟ (*)(T), T);
    template <typename T> friend ଶୂନ୍ୟ umtx_initOnce(UInitOnce&, ଶୂନ୍ୟ (*)(T, UErrorCode&), T, UErrorCode&);

    u_atomic_int32_t fState{0};
    UErrorCode fErrCode{U_ZERO_ERROR};

ସର୍ବସାଧାରଣ:
    U_COMMON_API ଶୂନ୍ୟ reset() { fState = 0; }
    U_COMMON_API UBool isReset() { ଫେରନ୍ତୁ umtx_loadAcquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API ଶୂନ୍ୟ  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

template<class T> ଶୂନ୍ୟ umtx_initOnce(UInitOnce &uio, T *obj, ଶୂନ୍ୟ (U_CALLCONV T::*fp)()) {
    ଯଦି (umtx_loadAcquire(uio.fState) == 2) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ umtx_initOnce(UInitOnce &uio, ଶୂନ୍ୟ (U_CALLCONV *fp)()) {
    ଯଦି (umtx_loadAcquire(uio.fState) == 2) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ umtx_initOnce(UInitOnce &uio, ଶୂନ୍ୟ (U_CALLCONV *fp)(UErrorCode &), UErrorCode &errCode) {
    ଯଦି (U_FAILURE(errCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } ଅନ୍ୟଥା {
        // Someone else already ran the initialization.
        ଯଦି (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
template<class T> ଶୂନ୍ୟ umtx_initOnce(UInitOnce &uio, ଶୂନ୍ୟ (U_CALLCONV *fp)(T), T context) {
    ଯଦି (umtx_loadAcquire(uio.fState) == 2) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
template<class T> ଶୂନ୍ୟ umtx_initOnce(UInitOnce &uio, ଶୂନ୍ୟ (U_CALLCONV *fp)(T, UErrorCode &), T context, UErrorCode &errCode) {
    ଯଦି (U_FAILURE(errCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି (umtx_loadAcquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, errCode);
        uio.fErrCode = errCode;
        umtx_initImplPostInit(uio);
    } ଅନ୍ୟଥା {
        // Someone else already ran the initialization.
        ଯଦି (U_FAILURE(uio.fErrCode)) {
            errCode = uio.fErrCode;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#ଯଦି (defined(_CPPLIB_VER) && !defined(_MSVC_STL_VERSION)) || \
    (defined(_MSVC_STL_VERSION) && _MSVC_STL_VERSION < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#ଅନ୍ୟଥା
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
ସର୍ବସାଧାରଣ:
    U_COMMON_API UMUTEX_CONSTEXPR UMutex() {}
    U_COMMON_API ~UMutex() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

    UMutex(ଅଚଳ UMutex& other) = delete;
    UMutex& operator=(ଅଚଳ UMutex& other) = delete;
    ଶୂନ୍ୟ* operator new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API ଶୂନ୍ୟ lock() {
        std::mutex *m = fMutex.load(std::memory_order_acquire);
        ଯଦି (m == nullptr) { m = getMutex(); }
        m->lock();
    }
    U_COMMON_API ଶୂନ୍ୟ unlock() { fMutex.load(std::memory_order_relaxed)->unlock(); }

    U_COMMON_API ସ୍ଥିର ଶୂନ୍ୟ cleanup();

ବ୍ୟକ୍ତିଗତ:
    alignas(std::mutex) ଅକ୍ଷର fStorage[ଆକାର(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    ସ୍ଥିର UMutex *gListHead;

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
U_CAPI ଶୂନ୍ୟ U_EXPORT2 umtx_lock(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI ଶୂନ୍ୟ U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_END

#endif /* UMUTEX_H */
/*eof*/

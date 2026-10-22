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

#यदि_अपरिभाषितम् UMUTEX_H
#परिभाषय UMUTEX_H

#अन्तर्भाव <atomic>
#अन्तर्भाव <condition_variable>
#अन्तर्भाव <mutex>
#अन्तर्भाव <type_traits>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uclean.h"
#अन्तर्भाव "unicode/uobject.h"

#अन्तर्भाव "putilimp.h"

#यदि defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
// Support for including an alternate implementation of atomic & mutex operations has been withdrawn.
// See issue ICU-20185.
#दोषः U_USER_ATOMICS and U_USER_MUTEX_H are not supported
#यदि_अन्तः

U_NAMESPACE_BEGIN

/****************************************************************************
 *
 *   Low Level Atomic Operations, ICU wrappers for.
 *
 ****************************************************************************/

प्रकारनिर्देश std::atomic<int32_t> u_atomic_int32_t;

अन्तःस्थ int32_t umtx_आरोपय_Acquire(u_atomic_int32_t &var) {
    प्रत्यागम var.आरोपय(std::स्मृतिः_क्रमः_acquire);
}

अन्तःस्थ शून्य umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::स्मृतिः_क्रमः_release);
}

अन्तःस्थ int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    प्रत्यागम var->fetch_सङ्कलय(1) + 1;
}

अन्तःस्थ int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    प्रत्यागम var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

संरचना U_COMMON_API_CLASS UInitOnce {
गुप्त:
    मित्रम् U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    मित्रम् U_COMMON_API शून्य U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    प्रतिरूपम् <typename T> मित्रम् शून्य umtx_initOnce(UInitOnce&, T*, शून्य (T::*)());
    मित्रम् शून्य umtx_initOnce(UInitOnce&, शून्य (*)());
    मित्रम् शून्य umtx_initOnce(UInitOnce&, शून्य (*)(UError_संहिता&), UError_संहिता&);
    प्रतिरूपम् <typename T> मित्रम् शून्य umtx_initOnce(UInitOnce&, शून्य (*)(T), T);
    प्रतिरूपम् <typename T> मित्रम् शून्य umtx_initOnce(UInitOnce&, शून्य (*)(T, UError_संहिता&), T, UError_संहिता&);

    u_atomic_int32_t fState{0};
    UError_संहिता f_Err_संहिता{U_ZERO_ERROR};

सार्वजनिक:
    U_COMMON_API शून्य reset() { fState = 0; }
    U_COMMON_API UBool isReset() { प्रत्यागम umtx_आरोपय_Acquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API शून्य  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

प्रतिरूपम्<class T> शून्य umtx_initOnce(UInitOnce &uio, T *obj, शून्य (U_CALLCONV T::*fp)()) {
    यदि (umtx_आरोपय_Acquire(uio.fState) == 2) {
        प्रत्यागम;
    }
    यदि (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
अन्तःस्थ शून्य umtx_initOnce(UInitOnce &uio, शून्य (U_CALLCONV *fp)()) {
    यदि (umtx_आरोपय_Acquire(uio.fState) == 2) {
        प्रत्यागम;
    }
    यदि (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
अन्तःस्थ शून्य umtx_initOnce(UInitOnce &uio, शून्य (U_CALLCONV *fp)(UError_संहिता &), UError_संहिता &err_संहिता) {
    यदि (U_FAILURE(err_संहिता)) {
        प्रत्यागम;
    }
    यदि (umtx_आरोपय_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(err_संहिता);
        uio.f_Err_संहिता = err_संहिता;
        umtx_initImplPostInit(uio);
    } अन्यथा {
        // Someone else already ran the initialization.
        यदि (U_FAILURE(uio.f_Err_संहिता)) {
            err_संहिता = uio.f_Err_संहिता;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
प्रतिरूपम्<class T> शून्य umtx_initOnce(UInitOnce &uio, शून्य (U_CALLCONV *fp)(T), T context) {
    यदि (umtx_आरोपय_Acquire(uio.fState) == 2) {
        प्रत्यागम;
    }
    यदि (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
प्रतिरूपम्<class T> शून्य umtx_initOnce(UInitOnce &uio, शून्य (U_CALLCONV *fp)(T, UError_संहिता &), T context, UError_संहिता &err_संहिता) {
    यदि (U_FAILURE(err_संहिता)) {
        प्रत्यागम;
    }
    यदि (umtx_आरोपय_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, err_संहिता);
        uio.f_Err_संहिता = err_संहिता;
        umtx_initImplPostInit(uio);
    } अन्यथा {
        // Someone else already ran the initialization.
        यदि (U_FAILURE(uio.f_Err_संहिता)) {
            err_संहिता = uio.f_Err_संहिता;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#यदि (defined(_CPPLIB_VER) && !defined(MSVC_STL_संस्करणम्)) || \
    (defined(MSVC_STL_संस्करणम्) && MSVC_STL_संस्करणम् < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#अन्यथा
#   define UMUTEX_CONSTEXPR constexpr
#यदि_अन्तः

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
    UMutex& संक्रियकः=(ध्रुव UMutex& other) = delete;
    शून्य* संक्रियकः new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API शून्य तालकम्() {
        std::mutex *m = fMutex.आरोपय(std::स्मृतिः_क्रमः_acquire);
        यदि (m == nullptr) { m = getMutex(); }
        m->तालकम्();
    }
    U_COMMON_API शून्य unlock() { fMutex.आरोपय(std::स्मृतिः_क्रमः_relaxed)->unlock(); }

    U_COMMON_API स्थिर शून्य cleanup();

गुप्त:
    alignas(std::mutex) वर्ण fStorage[परिमाण(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    स्थिर UMutex *g_List_शीर्ष;

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
U_CAPI शून्य U_EXPORT2 umtx_तालकम्(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI शून्य U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_अन्तः

#यदि_अन्तः /* UMUTEX_H */
/*eof*/

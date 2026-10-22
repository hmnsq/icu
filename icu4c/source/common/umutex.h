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

#నిర్వచితం_కాకపోతే UMUTEX_H
#నిర్వచించు UMUTEX_H

#అన్తర్భావ <atomic>
#అన్తర్భావ <condition_variable>
#అన్తర్భావ <mutex>
#అన్తర్భావ <type_traits>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uclean.h"
#అన్తర్భావ "unicode/uobject.h"

#అన్తర్భావ "putilimp.h"

#ఒకవేళ defined(U_USER_ATOMICS_H) || defined(U_USER_MUTEX_H)
// Support for including an alternate implementation of atomic & mutex operations has been withdrawn.
// See issue ICU-20185.
#దోషం U_USER_ATOMICS and U_USER_MUTEX_H are not supported
#అయితే_ముగింపు

U_NAMESPACE_BEGIN

/****************************************************************************
 *
 *   Low Level Atomic Operations, ICU wrappers for.
 *
 ****************************************************************************/

రకనిర్వచనం std::atomic<int32_t> u_atomic_int32_t;

అంతఃవరుస int32_t umtx_ఎక్కించు_Acquire(u_atomic_int32_t &var) {
    తిరిగి var.ఎక్కించు(std::జ్ఞాపకం_క్రమం_acquire);
}

అంతఃవరుస శూన్య umtx_storeRelease(u_atomic_int32_t &var, int32_t val) {
    var.store(val, std::జ్ఞాపకం_క్రమం_release);
}

అంతఃవరుస int32_t umtx_atomic_inc(u_atomic_int32_t *var) {
    తిరిగి var->fetch_కూడు(1) + 1;
}

అంతఃవరుస int32_t umtx_atomic_dec(u_atomic_int32_t *var) {
    తిరిగి var->fetch_sub(1) - 1;
}


/*************************************************************************************************
 *
 *  UInitOnce Definitions.
 *
 *************************************************************************************************/

నిర్మాణం U_COMMON_API_CLASS UInitOnce {
అంతరంగ:
    మిత్ర U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce&);
    మిత్ర U_COMMON_API శూన్య U_EXPORT2 umtx_initImplPostInit(UInitOnce&);
    మూస <typename T> మిత్ర శూన్య umtx_initOnce(UInitOnce&, T*, శూన్య (T::*)());
    మిత్ర శూన్య umtx_initOnce(UInitOnce&, శూన్య (*)());
    మిత్ర శూన్య umtx_initOnce(UInitOnce&, శూన్య (*)(UError_సంకేతం&), UError_సంకేతం&);
    మూస <typename T> మిత్ర శూన్య umtx_initOnce(UInitOnce&, శూన్య (*)(T), T);
    మూస <typename T> మిత్ర శూన్య umtx_initOnce(UInitOnce&, శూన్య (*)(T, UError_సంకేతం&), T, UError_సంకేతం&);

    u_atomic_int32_t fState{0};
    UError_సంకేతం f_Err_సంకేతం{U_ZERO_ERROR};

బహిరంగ:
    U_COMMON_API శూన్య reset() { fState = 0; }
    U_COMMON_API UBool isReset() { తిరిగి umtx_ఎక్కించు_Acquire(fState) == 0; }
// Note: isReset() is used by service registration code.
//                 Thread safety of this usage needs review.
};

U_COMMON_API UBool U_EXPORT2 umtx_initImplPreInit(UInitOnce &);
U_COMMON_API శూన్య  U_EXPORT2 umtx_initImplPostInit(UInitOnce &);

మూస<class T> శూన్య umtx_initOnce(UInitOnce &uio, T *obj, శూన్య (U_CALLCONV T::*fp)()) {
    ఒకవేళ (umtx_ఎక్కించు_Acquire(uio.fState) == 2) {
        తిరిగి;
    }
    ఒకవేళ (umtx_initImplPreInit(uio)) {
        (obj->*fp)();
        umtx_initImplPostInit(uio);
    }
}


// umtx_initOnce variant for plain functions, or static class functions.
//               No context parameter.
అంతఃవరుస శూన్య umtx_initOnce(UInitOnce &uio, శూన్య (U_CALLCONV *fp)()) {
    ఒకవేళ (umtx_ఎక్కించు_Acquire(uio.fState) == 2) {
        తిరిగి;
    }
    ఒకవేళ (umtx_initImplPreInit(uio)) {
        (*fp)();
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions.
//               With ErrorCode, No context parameter.
అంతఃవరుస శూన్య umtx_initOnce(UInitOnce &uio, శూన్య (U_CALLCONV *fp)(UError_సంకేతం &), UError_సంకేతం &err_సంకేతం) {
    ఒకవేళ (U_FAILURE(err_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ (umtx_ఎక్కించు_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(err_సంకేతం);
        uio.f_Err_సంకేతం = err_సంకేతం;
        umtx_initImplPostInit(uio);
    } లేకపోతే {
        // Someone else already ran the initialization.
        ఒకవేళ (U_FAILURE(uio.f_Err_సంకేతం)) {
            err_సంకేతం = uio.f_Err_సంకేతం;
        }
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter.
మూస<class T> శూన్య umtx_initOnce(UInitOnce &uio, శూన్య (U_CALLCONV *fp)(T), T context) {
    ఒకవేళ (umtx_ఎక్కించు_Acquire(uio.fState) == 2) {
        తిరిగి;
    }
    ఒకవేళ (umtx_initImplPreInit(uio)) {
        (*fp)(context);
        umtx_initImplPostInit(uio);
    }
}

// umtx_initOnce variant for plain functions, or static class functions,
//               with a context parameter and an error code.
మూస<class T> శూన్య umtx_initOnce(UInitOnce &uio, శూన్య (U_CALLCONV *fp)(T, UError_సంకేతం &), T context, UError_సంకేతం &err_సంకేతం) {
    ఒకవేళ (U_FAILURE(err_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ (umtx_ఎక్కించు_Acquire(uio.fState) != 2 && umtx_initImplPreInit(uio)) {
        // We run the initialization.
        (*fp)(context, err_సంకేతం);
        uio.f_Err_సంకేతం = err_సంకేతం;
        umtx_initImplPostInit(uio);
    } లేకపోతే {
        // Someone else already ran the initialization.
        ఒకవేళ (U_FAILURE(uio.f_Err_సంకేతం)) {
            err_సంకేతం = uio.f_Err_సంకేతం;
        }
    }
}

// UMutex should be constexpr-constructible, so that no initialization code
// is run during startup.
// This works on all C++ libraries except MS VS before VS2019.
#ఒకవేళ (defined(_CPPLIB_VER) && !defined(MSVC_STL_సంస్కరణ)) || \
    (defined(MSVC_STL_సంస్కరణ) && MSVC_STL_సంస్కరణ < 142)
    // (VS std lib older than VS2017) || (VS std lib version < VS2019)
#   define UMUTEX_CONSTEXPR
#లేకపోతే
#   define UMUTEX_CONSTEXPR constexpr
#అయితే_ముగింపు

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
బహిరంగ:
    U_COMMON_API UMUTEX_CONSTEXPR UMutex() {}
    U_COMMON_API ~UMutex() = అప్రమేయ;

    UMutex(స్థిరరాశి UMutex& other) = delete;
    UMutex& క్రియాకారకం=(స్థిరరాశి UMutex& other) = delete;
    శూన్య* క్రియాకారకం new(size_t) = delete;

    // requirements for C++ BasicLockable, allows UMutex to work with std::lock_guard
    U_COMMON_API శూన్య తాళం() {
        std::mutex *m = fMutex.ఎక్కించు(std::జ్ఞాపకం_క్రమం_acquire);
        ఒకవేళ (m == nullptr) { m = getMutex(); }
        m->తాళం();
    }
    U_COMMON_API శూన్య unlock() { fMutex.ఎక్కించు(std::జ్ఞాపకం_క్రమం_relaxed)->unlock(); }

    U_COMMON_API స్థిర శూన్య cleanup();

అంతరంగ:
    alignas(std::mutex) అక్షరం fStorage[పరిమాణం(std::mutex)] {};
    std::atomic<std::mutex *> fMutex { nullptr };

    /** All initialized UMutexes are kept in a linked list, so that they can be found,
     * and the underlying std::mutex destructed, by u_cleanup().
     */
    UMutex *fListLink { nullptr };
    స్థిర UMutex *g_List_తల;

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
U_CAPI శూన్య U_EXPORT2 umtx_తాళం(UMutex* mutex);

/* Unlock a mutex.
 * @param mutex The given mutex to be unlocked.  Pass NULL to specify
 *              the global ICU mutex.
 */
U_CAPI శూన్య U_EXPORT2 umtx_unlock (UMutex* mutex);


U_NAMESPACE_చివర

#అయితే_ముగింపు /* UMUTEX_H */
/*eof*/

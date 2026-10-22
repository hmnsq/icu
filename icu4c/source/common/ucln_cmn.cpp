// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 2001-2014, International Business Machines
*                Corporation and others. All Rights Reserved.
******************************************************************************
*   file name:  ucln_cmn.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001July05
*   created by: George Rhoten
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/uclean.h"
#ضمن "cmemory.h"
#ضمن "mutex.h"
#ضمن "uassert.h"
#ضمن "ucln.h"
#ضمن "ucln_cmn.h"
#ضمن "utracimp.h"
#ضمن "umutex.h"

/**  Auto-client for UCLN_COMMON **/
#تعریف_کن UCLN_TYPE_IS_COMMON
#ضمن "ucln_imp.h"

ساكن cleanupFunc *gCommonCleanupFunctions[UCLN_COMMON_شمار];
ساكن cleanupFunc *gLibCleanupFunctions[UCLN_COMMON];


/************************************************
 The cleanup order is important in this function.
 Please be sure that you have read ucln.h
 ************************************************/
U_CAPI فراغ U_EXPORT2
u_cleanup()
{
    UTRACE_مدخل_OC(UTRACE_U_CLEANUP);
    icu::umtx_قفل(اشاره_تهی);     /* Force a memory barrier, so that we are sure to see   */
    icu::umtx_unlock(اشاره_تهی);   /*   all state left around by any other threads.        */

    ucln_lib_cleanup();

    cmemory_cleanup();       /* undo any heap functions set by u_setMemoryFunctions(). */
    UTRACE_EXIT();           /* Must be before utrace_cleanup(), which turns off tracing. */
/*#if U_ENABLE_TRACING*/
    utrace_cleanup();
/*#endif*/
}

U_CAPI فراغ U_EXPORT2 ucln_cleanupOne(ECleanupLibraryType libType) 
{
    إذا (gLibCleanupFunctions[libType])
    {
        gLibCleanupFunctions[libType]();
        gLibCleanupFunctions[libType] = اشاره_تهی;
    }
}

U_CFUNC فراغ
ucln_common_ثبات_Cleanup(ECleanupCommonType type,
                            cleanupFunc *func)
{
    // Thread safety messiness: From ticket 10295, calls to registerCleanup() may occur
    // concurrently. Although such cases should be storing the same value, they raise errors
    // from the thread sanity checker. Doing the store within a mutex avoids those.
    // BUT that can trigger a recursive entry into std::call_once() in umutex.cpp when this code,
    // running from the call_once function, tries to grab the ICU global mutex, which
    // re-enters the mutex init path. So, work-around by special casing UCLN_COMMON_MUTEX, not
    // using the ICU global mutex for it.
    //
    // No other point in ICU uses std::call_once().

    U_ASSERT(UCLN_COMMON_شروع < type && type < UCLN_COMMON_شمار);
    إذا (type == UCLN_COMMON_MUTEX) {
        gCommonCleanupFunctions[type] = func;
    } وإلا إذا (UCLN_COMMON_شروع < type && type < UCLN_COMMON_شمار)  {
        icu::Mutex m;     // See ticket 10295 for discussion.
        gCommonCleanupFunctions[type] = func;
    }
#إذا !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_ثبات_Automatic_Cleanup();
#پایان_اگر
}

// Note: ucln_registerCleanup() is called with the ICU global mutex locked.
//       Be aware if adding anything to the function.
//       See ticket 10295 for discussion.

U_CAPI فراغ U_EXPORT2
ucln_ثبات_Cleanup(ECleanupLibraryType type,
                     cleanupFunc *func)
{
    U_ASSERT(UCLN_شروع < type && type < UCLN_COMMON);
    إذا (UCLN_شروع < type && type < UCLN_COMMON)
    {
        gLibCleanupFunctions[type] = func;
    }
}

U_CFUNC UBool ucln_lib_cleanup() {
    int32_t libType = UCLN_شروع;
    int32_t commonFunc = UCLN_COMMON_شروع;

    لكل (libType++; libType<UCLN_COMMON; libType++) {
        ucln_cleanupOne(static_cast<ECleanupLibraryType>(libType));
    }

    لكل (commonFunc++; commonFunc<UCLN_COMMON_شمار; commonFunc++) {
        إذا (gCommonCleanupFunctions[commonFunc])
        {
            gCommonCleanupFunctions[commonFunc]();
            gCommonCleanupFunctions[commonFunc] = اشاره_تهی;
        }
    }
#إذا !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_un_ثبات_Automatic_Cleanup();
#پایان_اگر
    أرجع درست;
}

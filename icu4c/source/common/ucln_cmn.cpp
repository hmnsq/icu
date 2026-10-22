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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uclean.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utracimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"

/**  Auto-client for UCLN_COMMON **/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCLN_TYPE_IS_COMMON
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_imp.h"

ਸਥਿਰ cleanupFunc *gCommonCleanupFunctions[UCLN_COMMON_ਗਿਣਤੀ];
ਸਥਿਰ cleanupFunc *gLibCleanupFunctions[UCLN_COMMON];


/************************************************
 The cleanup order is important in this function.
 Please be sure that you have read ucln.h
 ************************************************/
U_CAPI ਖਾਲੀ U_EXPORT2
u_cleanup()
{
    UTRACE_ਇੰਦਰਾਜ_OC(UTRACE_U_CLEANUP);
    icu::umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);     /* Force a memory barrier, so that we are sure to see   */
    icu::umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);   /*   all state left around by any other threads.        */

    ucln_lib_cleanup();

    cmemory_cleanup();       /* undo any heap functions set by u_setMemoryFunctions(). */
    UTRACE_EXIT();           /* Must be before utrace_cleanup(), which turns off tracing. */
/*#if U_ENABLE_TRACING*/
    utrace_cleanup();
/*#endif*/
}

U_CAPI ਖਾਲੀ U_EXPORT2 ucln_cleanupOne(ECleanupLibraryType libType) 
{
    ਜੇ (gLibCleanupFunctions[libType])
    {
        gLibCleanupFunctions[libType]();
        gLibCleanupFunctions[libType] = ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CFUNC ਖਾਲੀ
ucln_common_ਰਜਿਸਟਰ_Cleanup(ECleanupCommonType type,
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

    U_ASSERT(UCLN_COMMON_ਸ਼ੁਰੂ < type && type < UCLN_COMMON_ਗਿਣਤੀ);
    ਜੇ (type == UCLN_COMMON_MUTEX) {
        gCommonCleanupFunctions[type] = func;
    } ਨਹੀਂਤਾਂ ਜੇ (UCLN_COMMON_ਸ਼ੁਰੂ < type && type < UCLN_COMMON_ਗਿਣਤੀ)  {
        icu::Mutex m;     // See ticket 10295 for discussion.
        gCommonCleanupFunctions[type] = func;
    }
#ਜੇ !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_ਰਜਿਸਟਰ_Automatic_Cleanup();
#ਜੇ_ਅੰਤ
}

// Note: ucln_registerCleanup() is called with the ICU global mutex locked.
//       Be aware if adding anything to the function.
//       See ticket 10295 for discussion.

U_CAPI ਖਾਲੀ U_EXPORT2
ucln_ਰਜਿਸਟਰ_Cleanup(ECleanupLibraryType type,
                     cleanupFunc *func)
{
    U_ASSERT(UCLN_ਸ਼ੁਰੂ < type && type < UCLN_COMMON);
    ਜੇ (UCLN_ਸ਼ੁਰੂ < type && type < UCLN_COMMON)
    {
        gLibCleanupFunctions[type] = func;
    }
}

U_CFUNC UBool ucln_lib_cleanup() {
    int32_t libType = UCLN_ਸ਼ੁਰੂ;
    int32_t commonFunc = UCLN_COMMON_ਸ਼ੁਰੂ;

    ਲਈ (libType++; libType<UCLN_COMMON; libType++) {
        ucln_cleanupOne(static_cast<ECleanupLibraryType>(libType));
    }

    ਲਈ (commonFunc++; commonFunc<UCLN_COMMON_ਗਿਣਤੀ; commonFunc++) {
        ਜੇ (gCommonCleanupFunctions[commonFunc])
        {
            gCommonCleanupFunctions[commonFunc]();
            gCommonCleanupFunctions[commonFunc] = ਖਾਲੀ_ਸੂਚਕ;
        }
    }
#ਜੇ !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_un_ਰਜਿਸਟਰ_Automatic_Cleanup();
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਸੱਚ;
}

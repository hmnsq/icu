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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uclean.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"

/**  Auto-client for UCLN_COMMON **/
#ವ್ಯಾಖ್ಯಾನಿಸು UCLN_TYPE_IS_COMMON
#ಅನ್ತರ್ಭಾವ "ucln_imp.h"

ಸ್ಥಿರ cleanupFunc *gCommonCleanupFunctions[UCLN_COMMON_ಎಣಿಕೆ];
ಸ್ಥಿರ cleanupFunc *gLibCleanupFunctions[UCLN_COMMON];


/************************************************
 The cleanup order is important in this function.
 Please be sure that you have read ucln.h
 ************************************************/
U_CAPI ಶೂನ್ಯ U_EXPORT2
u_cleanup()
{
    UTRACE_ನಮೂದು_OC(UTRACE_U_CLEANUP);
    icu::umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);     /* Force a memory barrier, so that we are sure to see   */
    icu::umtx_unlock(ಶೂನ್ಯಸೂಚಕ);   /*   all state left around by any other threads.        */

    ucln_lib_cleanup();

    cmemory_cleanup();       /* undo any heap functions set by u_setMemoryFunctions(). */
    UTRACE_EXIT();           /* Must be before utrace_cleanup(), which turns off tracing. */
/*#if U_ENABLE_TRACING*/
    utrace_cleanup();
/*#endif*/
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ucln_cleanupOne(ECleanupLibraryType libType) 
{
    ಆದರೆ (gLibCleanupFunctions[libType])
    {
        gLibCleanupFunctions[libType]();
        gLibCleanupFunctions[libType] = ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CFUNC ಶೂನ್ಯ
ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(ECleanupCommonType type,
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

    U_ASSERT(UCLN_COMMON_ಆರಂಭ < type && type < UCLN_COMMON_ಎಣಿಕೆ);
    ಆದರೆ (type == UCLN_COMMON_MUTEX) {
        gCommonCleanupFunctions[type] = func;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (UCLN_COMMON_ಆರಂಭ < type && type < UCLN_COMMON_ಎಣಿಕೆ)  {
        icu::Mutex m;     // See ticket 10295 for discussion.
        gCommonCleanupFunctions[type] = func;
    }
#ಆದರೆ !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_ರಿಜಿಸ್ಟರ್_Automatic_Cleanup();
#ಆದರೆ_ಅಂತ್ಯ
}

// Note: ucln_registerCleanup() is called with the ICU global mutex locked.
//       Be aware if adding anything to the function.
//       See ticket 10295 for discussion.

U_CAPI ಶೂನ್ಯ U_EXPORT2
ucln_ರಿಜಿಸ್ಟರ್_Cleanup(ECleanupLibraryType type,
                     cleanupFunc *func)
{
    U_ASSERT(UCLN_ಆರಂಭ < type && type < UCLN_COMMON);
    ಆದರೆ (UCLN_ಆರಂಭ < type && type < UCLN_COMMON)
    {
        gLibCleanupFunctions[type] = func;
    }
}

U_CFUNC UBool ucln_lib_cleanup() {
    int32_t libType = UCLN_ಆರಂಭ;
    int32_t commonFunc = UCLN_COMMON_ಆರಂಭ;

    ಗಾಗಿ (libType++; libType<UCLN_COMMON; libType++) {
        ucln_cleanupOne(static_cast<ECleanupLibraryType>(libType));
    }

    ಗಾಗಿ (commonFunc++; commonFunc<UCLN_COMMON_ಎಣಿಕೆ; commonFunc++) {
        ಆದರೆ (gCommonCleanupFunctions[commonFunc])
        {
            gCommonCleanupFunctions[commonFunc]();
            gCommonCleanupFunctions[commonFunc] = ಶೂನ್ಯಸೂಚಕ;
        }
    }
#ಆದರೆ !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_un_ರಿಜಿಸ್ಟರ್_Automatic_Cleanup();
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

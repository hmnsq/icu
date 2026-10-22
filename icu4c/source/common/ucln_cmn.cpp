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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uclean.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucln.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "utracimp.h"
#അന്തര്ഭാവ "umutex.h"

/**  Auto-client for UCLN_COMMON **/
#define UCLN_TYPE_IS_COMMON
#അന്തര്ഭാവ "ucln_imp.h"

സ്ഥിതം cleanupFunc *gCommonCleanupFunctions[UCLN_COMMON_എണ്ണം];
സ്ഥിതം cleanupFunc *gLibCleanupFunctions[UCLN_COMMON];


/************************************************
 The cleanup order is important in this function.
 Please be sure that you have read ucln.h
 ************************************************/
U_CAPI ശൂന്യം U_EXPORT2
u_cleanup()
{
    UTRACE_ENTRY_OC(UTRACE_U_CLEANUP);
    icu::umtx_lock(nullptr);     /* Force a memory barrier, so that we are sure to see   */
    icu::umtx_unlock(nullptr);   /*   all state left around by any other threads.        */

    ucln_lib_cleanup();

    cmemory_cleanup();       /* undo any heap functions set by u_setMemoryFunctions(). */
    UTRACE_EXIT();           /* Must be before utrace_cleanup(), which turns off tracing. */
/*#if U_ENABLE_TRACING*/
    utrace_cleanup();
/*#endif*/
}

U_CAPI ശൂന്യം U_EXPORT2 ucln_cleanupOne(ECleanupLibraryType libType) 
{
    എങ്കിൽ (gLibCleanupFunctions[libType])
    {
        gLibCleanupFunctions[libType]();
        gLibCleanupFunctions[libType] = nullptr;
    }
}

U_CFUNC ശൂന്യം
ucln_common_registerCleanup(ECleanupCommonType type,
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

    U_ASSERT(UCLN_COMMON_START < type && type < UCLN_COMMON_എണ്ണം);
    എങ്കിൽ (type == UCLN_COMMON_MUTEX) {
        gCommonCleanupFunctions[type] = func;
    } അല്ലെങ്കിൽ എങ്കിൽ (UCLN_COMMON_START < type && type < UCLN_COMMON_എണ്ണം)  {
        icu::Mutex m;     // See ticket 10295 for discussion.
        gCommonCleanupFunctions[type] = func;
    }
#എങ്കിൽ !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_registerAutomaticCleanup();
#endif
}

// Note: ucln_registerCleanup() is called with the ICU global mutex locked.
//       Be aware if adding anything to the function.
//       See ticket 10295 for discussion.

U_CAPI ശൂന്യം U_EXPORT2
ucln_registerCleanup(ECleanupLibraryType type,
                     cleanupFunc *func)
{
    U_ASSERT(UCLN_START < type && type < UCLN_COMMON);
    എങ്കിൽ (UCLN_START < type && type < UCLN_COMMON)
    {
        gLibCleanupFunctions[type] = func;
    }
}

U_CFUNC UBool ucln_lib_cleanup() {
    int32_t libType = UCLN_START;
    int32_t commonFunc = UCLN_COMMON_START;

    വേണ്ടി (libType++; libType<UCLN_COMMON; libType++) {
        ucln_cleanupOne(static_cast<ECleanupLibraryType>(libType));
    }

    വേണ്ടി (commonFunc++; commonFunc<UCLN_COMMON_എണ്ണം; commonFunc++) {
        എങ്കിൽ (gCommonCleanupFunctions[commonFunc])
        {
            gCommonCleanupFunctions[commonFunc]();
            gCommonCleanupFunctions[commonFunc] = nullptr;
        }
    }
#എങ്കിൽ !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_unRegisterAutomaticCleanup();
#endif
    തിരികെ സത്യം;
}

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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uclean.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucln.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "utracimp.h"
#அந்தர்பாவ "umutex.h"

/**  Auto-client for UCLN_COMMON **/
#define UCLN_TYPE_IS_COMMON
#அந்தர்பாவ "ucln_imp.h"

நிலையான cleanupFunc *gCommonCleanupFunctions[UCLN_COMMON_மொத்தம்];
நிலையான cleanupFunc *gLibCleanupFunctions[UCLN_COMMON];


/************************************************
 The cleanup order is important in this function.
 Please be sure that you have read ucln.h
 ************************************************/
U_CAPI வெறுமை U_EXPORT2
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

U_CAPI வெறுமை U_EXPORT2 ucln_cleanupOne(ECleanupLibraryType libType) 
{
    எனில் (gLibCleanupFunctions[libType])
    {
        gLibCleanupFunctions[libType]();
        gLibCleanupFunctions[libType] = nullptr;
    }
}

U_CFUNC வெறுமை
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

    U_ASSERT(UCLN_COMMON_START < type && type < UCLN_COMMON_மொத்தம்);
    எனில் (type == UCLN_COMMON_MUTEX) {
        gCommonCleanupFunctions[type] = func;
    } இல்லையெனில் எனில் (UCLN_COMMON_START < type && type < UCLN_COMMON_மொத்தம்)  {
        icu::Mutex m;     // See ticket 10295 for discussion.
        gCommonCleanupFunctions[type] = func;
    }
#எனில் !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_registerAutomaticCleanup();
#endif
}

// Note: ucln_registerCleanup() is called with the ICU global mutex locked.
//       Be aware if adding anything to the function.
//       See ticket 10295 for discussion.

U_CAPI வெறுமை U_EXPORT2
ucln_registerCleanup(ECleanupLibraryType type,
                     cleanupFunc *func)
{
    U_ASSERT(UCLN_START < type && type < UCLN_COMMON);
    எனில் (UCLN_START < type && type < UCLN_COMMON)
    {
        gLibCleanupFunctions[type] = func;
    }
}

U_CFUNC UBool ucln_lib_cleanup() {
    int32_t libType = UCLN_START;
    int32_t commonFunc = UCLN_COMMON_START;

    க்கு (libType++; libType<UCLN_COMMON; libType++) {
        ucln_cleanupOne(static_cast<ECleanupLibraryType>(libType));
    }

    க்கு (commonFunc++; commonFunc<UCLN_COMMON_மொத்தம்; commonFunc++) {
        எனில் (gCommonCleanupFunctions[commonFunc])
        {
            gCommonCleanupFunctions[commonFunc]();
            gCommonCleanupFunctions[commonFunc] = nullptr;
        }
    }
#எனில் !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_unRegisterAutomaticCleanup();
#endif
    திரும்பு உண்மை;
}

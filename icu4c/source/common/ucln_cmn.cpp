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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uclean.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucln.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "utracimp.h"
#అన్తర్భావ "umutex.h"

/**  Auto-client for UCLN_COMMON **/
#నిర్వచించు UCLN_TYPE_IS_COMMON
#అన్తర్భావ "ucln_imp.h"

స్థిర cleanupFunc *gCommonCleanupFunctions[UCLN_COMMON_లెక్క];
స్థిర cleanupFunc *gLibCleanupFunctions[UCLN_COMMON];


/************************************************
 The cleanup order is important in this function.
 Please be sure that you have read ucln.h
 ************************************************/
U_CAPI శూన్య U_EXPORT2
u_cleanup()
{
    UTRACE_నమోదు_OC(UTRACE_U_CLEANUP);
    icu::umtx_తాళం(శూన్యసూచిక);     /* Force a memory barrier, so that we are sure to see   */
    icu::umtx_unlock(శూన్యసూచిక);   /*   all state left around by any other threads.        */

    ucln_lib_cleanup();

    cmemory_cleanup();       /* undo any heap functions set by u_setMemoryFunctions(). */
    UTRACE_EXIT();           /* Must be before utrace_cleanup(), which turns off tracing. */
/*#if U_ENABLE_TRACING*/
    utrace_cleanup();
/*#endif*/
}

U_CAPI శూన్య U_EXPORT2 ucln_cleanupOne(ECleanupLibraryType libType) 
{
    ఒకవేళ (gLibCleanupFunctions[libType])
    {
        gLibCleanupFunctions[libType]();
        gLibCleanupFunctions[libType] = శూన్యసూచిక;
    }
}

U_CFUNC శూన్య
ucln_common_రిజిస్టరు_Cleanup(ECleanupCommonType type,
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

    U_ASSERT(UCLN_COMMON_ప్రారంభం < type && type < UCLN_COMMON_లెక్క);
    ఒకవేళ (type == UCLN_COMMON_MUTEX) {
        gCommonCleanupFunctions[type] = func;
    } లేకపోతే ఒకవేళ (UCLN_COMMON_ప్రారంభం < type && type < UCLN_COMMON_లెక్క)  {
        icu::Mutex m;     // See ticket 10295 for discussion.
        gCommonCleanupFunctions[type] = func;
    }
#ఒకవేళ !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_రిజిస్టరు_Automatic_Cleanup();
#అయితే_ముగింపు
}

// Note: ucln_registerCleanup() is called with the ICU global mutex locked.
//       Be aware if adding anything to the function.
//       See ticket 10295 for discussion.

U_CAPI శూన్య U_EXPORT2
ucln_రిజిస్టరు_Cleanup(ECleanupLibraryType type,
                     cleanupFunc *func)
{
    U_ASSERT(UCLN_ప్రారంభం < type && type < UCLN_COMMON);
    ఒకవేళ (UCLN_ప్రారంభం < type && type < UCLN_COMMON)
    {
        gLibCleanupFunctions[type] = func;
    }
}

U_CFUNC UBool ucln_lib_cleanup() {
    int32_t libType = UCLN_ప్రారంభం;
    int32_t commonFunc = UCLN_COMMON_ప్రారంభం;

    కొరకు (libType++; libType<UCLN_COMMON; libType++) {
        ucln_cleanupOne(static_cast<ECleanupLibraryType>(libType));
    }

    కొరకు (commonFunc++; commonFunc<UCLN_COMMON_లెక్క; commonFunc++) {
        ఒకవేళ (gCommonCleanupFunctions[commonFunc])
        {
            gCommonCleanupFunctions[commonFunc]();
            gCommonCleanupFunctions[commonFunc] = శూన్యసూచిక;
        }
    }
#ఒకవేళ !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_un_రిజిస్టరు_Automatic_Cleanup();
#అయితే_ముగింపు
    తిరిగి నిజం;
}

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

#شامل "unicode/utypes.h"
#شامل "unicode/uclean.h"
#شامل "cmemory.h"
#شامل "mutex.h"
#شامل "uassert.h"
#شامل "ucln.h"
#شامل "ucln_cmn.h"
#شامل "utracimp.h"
#شامل "umutex.h"

/**  Auto-client for UCLN_COMMON **/
#تعریف_کرو UCLN_TYPE_IS_COMMON
#شامل "ucln_imp.h"

ساکن cleanupFunc *gCommonCleanupFunctions[UCLN_COMMON_گنتی];
ساکن cleanupFunc *gLibCleanupFunctions[UCLN_COMMON];


/************************************************
 The cleanup order is important in this function.
 Please be sure that you have read ucln.h
 ************************************************/
U_CAPI خالی U_EXPORT2
u_cleanup()
{
    UTRACE_اندراج_OC(UTRACE_U_CLEANUP);
    icu::umtx_تالا(خالی_اشاریہ);     /* Force a memory barrier, so that we are sure to see   */
    icu::umtx_unlock(خالی_اشاریہ);   /*   all state left around by any other threads.        */

    ucln_lib_cleanup();

    cmemory_cleanup();       /* undo any heap functions set by u_setMemoryFunctions(). */
    UTRACE_EXIT();           /* Must be before utrace_cleanup(), which turns off tracing. */
/*#if U_ENABLE_TRACING*/
    utrace_cleanup();
/*#endif*/
}

U_CAPI خالی U_EXPORT2 ucln_cleanupOne(ECleanupLibraryType libType) 
{
    اگر (gLibCleanupFunctions[libType])
    {
        gLibCleanupFunctions[libType]();
        gLibCleanupFunctions[libType] = خالی_اشاریہ;
    }
}

U_CFUNC خالی
ucln_common_رجسٹر_Cleanup(ECleanupCommonType type,
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

    U_ASSERT(UCLN_COMMON_آغاز < type && type < UCLN_COMMON_گنتی);
    اگر (type == UCLN_COMMON_MUTEX) {
        gCommonCleanupFunctions[type] = func;
    } ورنہ اگر (UCLN_COMMON_آغاز < type && type < UCLN_COMMON_گنتی)  {
        icu::Mutex m;     // See ticket 10295 for discussion.
        gCommonCleanupFunctions[type] = func;
    }
#اگر !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_رجسٹر_Automatic_Cleanup();
#اگر_ختم
}

// Note: ucln_registerCleanup() is called with the ICU global mutex locked.
//       Be aware if adding anything to the function.
//       See ticket 10295 for discussion.

U_CAPI خالی U_EXPORT2
ucln_رجسٹر_Cleanup(ECleanupLibraryType type,
                     cleanupFunc *func)
{
    U_ASSERT(UCLN_آغاز < type && type < UCLN_COMMON);
    اگر (UCLN_آغاز < type && type < UCLN_COMMON)
    {
        gLibCleanupFunctions[type] = func;
    }
}

U_CFUNC UBool ucln_lib_cleanup() {
    int32_t libType = UCLN_آغاز;
    int32_t commonFunc = UCLN_COMMON_آغاز;

    برائے (libType++; libType<UCLN_COMMON; libType++) {
        ucln_cleanupOne(static_cast<ECleanupLibraryType>(libType));
    }

    برائے (commonFunc++; commonFunc<UCLN_COMMON_گنتی; commonFunc++) {
        اگر (gCommonCleanupFunctions[commonFunc])
        {
            gCommonCleanupFunctions[commonFunc]();
            gCommonCleanupFunctions[commonFunc] = خالی_اشاریہ;
        }
    }
#اگر !UCLN_NO_AUTO_CLEANUP && (defined(UCLN_AUTO_ATEXIT) || defined(UCLN_AUTO_LOCAL))
    ucln_un_رجسٹر_Automatic_Cleanup();
#اگر_ختم
    واپس سچ;
}

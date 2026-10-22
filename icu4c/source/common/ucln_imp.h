// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
* Copyright (C) 2009-2011, International Business Machines
*                Corporation and others. All Rights Reserved.
*
******************************************************************************
*   file name:  ucln_imp.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   This file contains the platform specific implementation of per-library cleanup.
*
*/


#ifndef __UCLN_IMP_H__
#define __UCLN_IMP_H__

#ಅನ್ತರ್ಭಾವ "ucln.h"
#ಅನ್ತರ್ಭಾವ <stdlib.h>

/**
 * Auto cleanup of ICU libraries
 * There are several methods in per library cleanup of icu libraries:
 * 1) Compiler/Platform based cleanup:
 *   a) Windows MSVC uses DllMain()
 *   b) GCC uses destructor function attribute
 *   c) Sun Studio, AIX VA, and HP-UX aCC uses a linker option to set the exit function
 * 2) Using atexit()
 * 3) Implementing own automatic cleanup functions
 *
 * For option 1, ensure that UCLN_NO_AUTO_CLEANUP is set to 0 by using --enable-auto-cleanup
 * configure option or by otherwise setting UCLN_NO_AUTO_CLEANUP to 0
 * For option 2, follow option 1 and also define UCLN_AUTO_ATEXIT
 * For option 3, follow option 1 and also define UCLN_AUTO_LOCAL (see below for more information)
 */

#ಯದಿ !UCLN_NO_AUTO_CLEANUP

/*
 * The following declarations are for when UCLN_AUTO_LOCAL or UCLN_AUTO_ATEXIT
 * are defined. They are commented out because they are static and will be defined
 * later. The information is still here to provide some guidance for the developer
 * who chooses to use UCLN_AUTO_LOCAL.
 */
/**
 * Give the library an opportunity to register an automatic cleanup.
 * This may be called more than once.
 */
/*static void ucln_registerAutomaticCleanup();*/
/**
 * Unregister an automatic cleanup, if possible. Called from cleanup.
 */
/*static void ucln_unRegisterAutomaticCleanup();*/

#ifdef UCLN_TYPE_IS_COMMON
#   define UCLN_CLEAN_ME_UP u_cleanup()
#ಅನ್ಯಥಾ
#   define UCLN_CLEAN_ME_UP ucln_cleanupOne(UCLN_TYPE)
#endif

/* ------------ automatic cleanup: registration. Choose ONE ------- */
#ಯದಿ defined(UCLN_AUTO_LOCAL)
/* To use:
 *  1. define UCLN_AUTO_LOCAL,
 *  2. create ucln_local_hook.c containing implementations of
 *           static void ucln_registerAutomaticCleanup()
 *           static void ucln_unRegisterAutomaticCleanup()
 */
#ಅನ್ತರ್ಭಾವ "ucln_local_hook.c"

#ನೋಚೇದ್ಯದಿ defined(UCLN_AUTO_ATEXIT)
/*
 * Use the ANSI C 'atexit' function. Note that this mechanism does not
 * guarantee the order of cleanup relative to other users of ICU!
 */
ಸ್ಥಿರ UBool gAutoCleanRegistered = false;

ಸ್ಥಿರ ಶೂನ್ಯ ucln_atexit_handler()
{
    UCLN_CLEAN_ME_UP;
}

ಸ್ಥಿರ ಶೂನ್ಯ ucln_registerAutomaticCleanup()
{
    ಯದಿ(!gAutoCleanRegistered) {
        gAutoCleanRegistered = true;
        atexit(&ucln_atexit_handler);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ ucln_unRegisterAutomaticCleanup () {
}
/* ------------end of automatic cleanup: registration. ------- */

#ನೋಚೇದ್ಯದಿ defined (UCLN_FINI)
/**
 * If UCLN_FINI is defined, it is the (versioned, etc) name of a cleanup
 * entrypoint. Add a stub to call ucln_cleanupOne
 * Used on AIX, Solaris, and HP-UX
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2 UCLN_FINI (ಶೂನ್ಯ);

U_CAPI ಶೂನ್ಯ U_EXPORT2 UCLN_FINI ()
{
    /* This function must be defined, if UCLN_FINI is defined, else link error. */
     UCLN_CLEAN_ME_UP;
}

/* Windows: DllMain */
#ನೋಚೇದ್ಯದಿ U_PLATFORM_HAS_WIN32_API
/* 
 * ICU's own DllMain.
 */

/* these are from putil.c */
/* READ READ READ READ!    Are you getting compilation errors from windows.h?
          Any source file which includes this (ucln_imp.h) header MUST 
          be defined with language extensions ON. */
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>
/*
 * This is a stub DllMain function with icu specific process handling code.
 */
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    BOOL status = true;

    ವಿಕಲ್ಪ(fdwReason) {
        ಪಕ್ಷ DLL_PROCESS_ATTACH:
             /* ICU does not trap process attach, but must pass these through properly. */
            /* ICU specific process attach could go here */
            ಭಙ್ಗ;

        ಪಕ್ಷ DLL_PROCESS_DETACH:
            /* Here is the one we actually care about. */

            UCLN_CLEAN_ME_UP;

            ಭಙ್ಗ;

        ಪಕ್ಷ DLL_THREAD_ATTACH:
            /* ICU does not trap thread attach, but must pass these through properly. */
            /* ICU specific thread attach could go here */
            ಭಙ್ಗ;

        ಪಕ್ಷ DLL_THREAD_DETACH:
            /* ICU does not trap thread detach, but must pass these through properly. */
            /* ICU specific thread detach could go here */
            ಭಙ್ಗ;

    }
    ಪ್ರತ್ಯಾಗಮ status;
}

#ನೋಚೇದ್ಯದಿ defined(__GNUC__)
/* GCC - use __attribute((destructor)) */
ಸ್ಥಿರ ಶೂನ್ಯ ucln_destructor()   __attribute__((destructor)) ;

ಸ್ಥಿರ ಶೂನ್ಯ ucln_destructor() 
{
    UCLN_CLEAN_ME_UP;
}

#endif

#endif /* UCLN_NO_AUTO_CLEANUP */

#ಅನ್ಯಥಾ
#error This file can only be included once.
#endif

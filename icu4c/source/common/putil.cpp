// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1997-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*  FILE NAME : putil.c (previously putil.cpp and ptypes.cpp)
*
*   Date        Name        Description
*   04/14/97    aliu        Creation.
*   04/24/97    aliu        Added getDefaultDataDirectory() and
*                            getDefaultLocaleID().
*   04/28/97    aliu        Rewritten to assume Unix and apply general methods
*                            for assumed case.  Non-UNIX platforms must be
*                            special-cased.  Rewrote numeric methods dealing
*                            with NaN and Infinity to be platform independent
*                             over all IEEE 754 platforms.
*   05/13/97    aliu        Restored sign of timezone
*                            (semantics are hours West of GMT)
*   06/16/98    erm         Added IEEE_754 stuff, cleaned up isInfinite, isNan,
*                             nextDouble..
*   07/22/98    stephen     Added remainder, max, min, trunc
*   08/13/98    stephen     Added isNegativeInfinity, isPositiveInfinity
*   08/24/98    stephen     Added longBitsFromDouble
*   09/08/98    stephen     Minor changes for Mac Port
*   03/02/99    stephen     Removed openFile().  Added AS400 support.
*                            Fixed EBCDIC tables
*   04/15/99    stephen     Converted to C.
*   06/28/99    stephen     Removed mutex locking in u_isBigEndian().
*   08/04/99    jeffrey R.  Added OS/2 changes
*   11/15/99    helena      Integrated S/390 IEEE support.
*   04/26/01    Barry N.    OS/400 support for uprv_getDefaultLocaleID
*   08/15/01    Steven H.   OS/400 support for uprv_getDefaultCodepage
*   01/03/08    Steven L.   Fake Time Support
******************************************************************************
*/

// Defines _XOPEN_SOURCE for access to POSIX functions.
// Must be before any other #includes.
#ಅನ್ತರ್ಭಾವ "uposixdefs.h"

// First, the platform type. Need this for U_PLATFORM.
#ಅನ್ತರ್ಭಾವ "unicode/platform.h"

/*
 * Cygwin with GCC requires inclusion of time.h after the above disabling strict asci mode statement.
 */
#ಅನ್ತರ್ಭಾವ <time.h>

#ಆದರೆ !U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
#ಅನ್ತರ್ಭಾವ <sys/time.h>
#ಆದರೆ_ಅಂತ್ಯ

/* include the rest of the ICU headers */
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "locmap.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"

/* Include standard headers. */
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <locale.h>
#ಅನ್ತರ್ಭಾವ <float.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_COMMON_IMPLEMENTATION
#ದೋಷ U_COMMON_IMPLEMENTATION not ಗಣ - must be ಗಣ ಗಾಗಿ all ICU ಮೂಲ files in common/ - see https://unicode-org.github.io/icu/userguide/icu/howtouseicu.html
#ಆದರೆ_ಅಂತ್ಯ


/* include system headers */
#ಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
    /*
     * TODO: U_PLATFORM_USES_ONLY_WIN32_API includes MinGW.
     * Should Cygwin be included as well (U_PLATFORM_HAS_WIN32_API)
     * to use native APIs as much as possible?
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WIN32_LEAN_AND_ಸರಾಸರಿ
#   define WIN32_LEAN_AND_ಸರಾಸರಿ
#ಆದರೆ_ಅಂತ್ಯ
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>
#   include "unicode/uloc.h"
#   include "wintz.h"
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_OS400
#   include <float.h>
#   include <qusec.h>       /* error code structure */
#   include <qusrjobi.h>
#   include <qliept.h>      /* EPT_CALL macro  - this include must be after all other "QSYSINCs" */
#   include <mih/testptr.h> /* For uprv_maximumPtr */
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_OS390
#   include "unicode/ucnv.h"   /* Needed for UCNV_SWAP_LFNL_OPTION_STRING */
#ಅಥವಾಆದರೆ U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS
#   include <limits.h>
#   include <unistd.h>
#   ಆದರೆ U_PLATFORM == U_PF_SOLARIS
#       ifndef _XPG4_2
#           define _XPG4_2
#       endif
#   elif U_PLATFORM == U_PF_ANDROID
#       include <sys/system_properties.h>
#       include <dlfcn.h>
#   endif
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_QNX
#   include <sys/neutrino.h>
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Only include langinfo.h if we have a way to get the codeset. If we later
 * depend on more feature, we can test on U_HAVE_NL_LANGINFO.
 *
 */

#ಆದರೆ U_HAVE_NL_LANGINFO_CODESET
#ಅನ್ತರ್ಭಾವ <langinfo.h>
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Simple things (presence of functions, etc) should just go in configure.in and be added to
 * icucfg.h via autoheader.
 */
#ಆದರೆ U_PLATFORM_IMPLEMENTS_POSIX
#   ಆದರೆ U_PLATFORM == U_PF_OS400
#    define HAVE_DLFCN_H 0
#    define HAVE_DLOPEN 0
#   ಇಲ್ಲದಿದ್ದರೆ
#   ifndef HAVE_DLFCN_H
#    define HAVE_DLFCN_H 1
#   endif
#   ifndef HAVE_DLOPEN
#    define HAVE_DLOPEN 1
#   endif
#   endif
#   ifndef HAVE_GETTIMEOFDAY
#    define HAVE_GETTIMEOFDAY 1
#   endif
#ಇಲ್ಲದಿದ್ದರೆ
#   define HAVE_DLFCN_H 0
#   define HAVE_DLOPEN 0
#   define HAVE_GETTIMEOFDAY 0
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_USE

/* Define the extension for data files, again... */
#ವ್ಯಾಖ್ಯಾನಿಸು ದತ್ತಾಂಶ_TYPE "dat"

/* Leave this copyright notice here! */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ copyright[] = U_COPYRIGHT_STRING;

/* floating point implementations ------------------------------------------- */

/* We return QNAN rather than SNAN*/
#ವ್ಯಾಖ್ಯಾನಿಸು SIGN 0x80000000U

/* Make it easy to define certain types of constants */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ {
    int64_t i64; /* This must be defined first in order to allow the initialization to work. This is a C89 feature. */
    ದ್ವಿಗುಣ d64;
} ಬಿಟ್_ನಮೂನೆ_Conversion;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಬಿಟ್_ನಮೂನೆ_Conversion gNan = {static_cast<int64_t>(INT64_C(0x7FF8000000000000))};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಬಿಟ್_ನಮೂನೆ_Conversion gInf = {static_cast<int64_t>(INT64_C(0x7FF0000000000000))};

/*---------------------------------------------------------------------------
  Platform utilities
  Our general strategy is to assume we're on a POSIX platform.  Platforms which
  are non-POSIX must declare themselves so.  The default POSIX implementation
  will sometimes work for non-POSIX platforms as well (e.g., the NaN-related
  functions).
  ---------------------------------------------------------------------------*/

#ಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API || U_PLATFORM == U_PF_OS400
#   undef U_POSIX_LOCALE
#ಇಲ್ಲದಿದ್ದರೆ
#   define U_POSIX_LOCALE    1
#ಆದರೆ_ಅಂತ್ಯ

/*
    WARNING! u_topNBytesOfDouble and u_bottomNBytesOfDouble
    can't be properly optimized by the gcc compiler sometimes (i.e. gcc 3.2).
*/
#ಆದರೆ !IEEE_754
ಸ್ಥಿರ ಅಕ್ಷರ*
u_ತುದಿ_NBytes_ದ_Double(ದ್ವಿಗುಣ* d, ಪೂರ್ಣಾಂಕ n)
{
#ಆದರೆ U_IS_BIG_ENDIAN
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*)d;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*)(d + 1) - n;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಅಕ್ಷರ*
u_bottom_NBytes_ದ_Double(ದ್ವಿಗುಣ* d, ಪೂರ್ಣಾಂಕ n)
{
#ಆದರೆ U_IS_BIG_ENDIAN
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*)(d + 1) - n;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (ಅಕ್ಷರ*)d;
#ಆದರೆ_ಅಂತ್ಯ
}
#ಆದರೆ_ಅಂತ್ಯ   /* !IEEE_754 */

#ಆದರೆ IEEE_754
ಸ್ಥಿರ UBool
u_sign_ಬಿಟ್(ದ್ವಿಗುಣ d) {
    uint8_t hiByte;
#ಆದರೆ U_IS_BIG_ENDIAN
    hiByte = *(uint8_t *)&d;
#ಇಲ್ಲದಿದ್ದರೆ
    hiByte = *(reinterpret_cast<uint8_t*>(&d) + ಗಾತ್ರ(ದ್ವಿಗುಣ) - 1);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ (hiByte & 0x80) != 0;
}
#ಆದರೆ_ಅಂತ್ಯ



#ಆದರೆ defined (U_DEBUG_FAKETIME)
/* Override the clock to test things without having to move the system clock.
 * Assumes POSIX gettimeofday() will function
 */
UDate fakeClock_t0 = 0; /** Time to start the clock from **/
UDate fakeClock_dt = 0; /** Offset (fake time - real time) **/
UBool fake_Clock_ಗಣ = ಸುಳ್ಳು; /** True if fake clock has spun up **/

ಸ್ಥಿರ UDate getUTCtime_real() {
    ರಚನೆ timeval posix_ಸಮಯ;
    gettimeofday(&posix_ಸಮಯ, ಶೂನ್ಯಸೂಚಕ);
    ಹಿಂತಿರುಗಿ (UDate)(((int64_t)posix_ಸಮಯ.tv_sec * U_MILLIS_ಪ್ರತಿ_ಎರಡನೇ) + (posix_ಸಮಯ.tv_usec/1000));
}

ಸ್ಥಿರ UDate getUTCtime_fake() {
    ಸ್ಥಿರ UMutex fakeClockMutex;
    umtx_ಬೀಗ(&fakeClockMutex);
    ಆದರೆ(!fake_Clock_ಗಣ) {
        UDate real = getUTCtime_real();
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fake_ಆರಂಭ = getenv("U_FAKETIME_START");
        ಆದರೆ((fake_ಆರಂಭ!=ಶೂನ್ಯಸೂಚಕ) && (fake_ಆರಂಭ[0]!=0)) {
            ಅಕ್ಷರಸರಣಿ_ಓದು(fake_ಆರಂಭ,"%lf",&fakeClock_t0);
            fakeClock_dt = fakeClock_t0 - real;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, so the ICU clock will start at a preset value\n"
                    "env variable U_FAKETIME_START=%.0f (%s) for an offset of %.0f ms from the current time %.0f\n",
                    fakeClock_t0, fake_ಆರಂಭ, fakeClock_dt, real);
        } ಇಲ್ಲದಿದ್ದರೆ {
          fakeClock_dt = 0;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, but U_FAKETIME_START was not set.\n"
                    "Set U_FAKETIME_START to the number of milliseconds since 1/1/1970 to set the ICU clock.\n");
        }
        fake_Clock_ಗಣ = ನಿಜ;
    }
    umtx_unlock(&fakeClockMutex);

    ಹಿಂತಿರುಗಿ getUTCtime_real() + fakeClock_dt;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ {
    int64_t int64;
    FILETIME file_ಸಮಯ;
} File_ಸಮಯ_Conversion;   /* This is like a ULARGE_INTEGER */

/* Number of 100 nanoseconds from 1/1/1601 to 1/1/1970 */
#ವ್ಯಾಖ್ಯಾನಿಸು EPOCH_BIAS  INT64_C(116444736000000000)
#ವ್ಯಾಖ್ಯಾನಿಸು HECTONANOSECOND_ಪ್ರತಿ_MILLISECOND   10000

#ಆದರೆ_ಅಂತ್ಯ

/*---------------------------------------------------------------------------
  Universal Implementations
  These are designed to work on all platforms.  Try these, and if they
  don't work on your platform, then special case your platform with new
  implementations.
---------------------------------------------------------------------------*/

U_CAPI UDate U_EXPORT2
uprv_getUTCtime()
{
#ಆದರೆ defined(U_DEBUG_FAKETIME)
    ಹಿಂತಿರುಗಿ getUTCtime_fake(); /* Hook for overriding the clock */
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ uprv_getRawUTCtime();
#ಆದರೆ_ಅಂತ್ಯ
}

/* Return UTC (GMT) time measured in milliseconds since 0:00 on 1/1/70.*/
U_CAPI UDate U_EXPORT2
uprv_getRawUTCtime()
{
#ಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API

    File_ಸಮಯ_Conversion win_ಸಮಯ;
    Get_System_ಸಮಯ_As_File_ಸಮಯ(&win_ಸಮಯ.file_ಸಮಯ);
    ಹಿಂತಿರುಗಿ (UDate)((win_ಸಮಯ.int64 - EPOCH_BIAS) / HECTONANOSECOND_ಪ್ರತಿ_MILLISECOND);
#ಇಲ್ಲದಿದ್ದರೆ

#ಆದರೆ HAVE_GETTIMEOFDAY
    ರಚನೆ timeval posix_ಸಮಯ;
    gettimeofday(&posix_ಸಮಯ, ಶೂನ್ಯಸೂಚಕ);
    ಹಿಂತಿರುಗಿ (UDate)(((int64_t)posix_ಸಮಯ.tv_sec * U_MILLIS_ಪ್ರತಿ_ಎರಡನೇ) + (posix_ಸಮಯ.tv_usec/1000));
#ಇಲ್ಲದಿದ್ದರೆ
    time_t epochtime;
    time(&epochtime);
    ಹಿಂತಿರುಗಿ (UDate)epochtime * U_MILLIS_ಪ್ರತಿ_ಎರಡನೇ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
}

/*-----------------------------------------------------------------------------
  IEEE 754
  These methods detect and return NaN and infinity values for doubles
  conforming to IEEE 754.  Platforms which support this standard include X86,
  Mac 680x0, Mac PowerPC, AIX RS/6000, and most others.
  If this doesn't work on your platform, you have non-IEEE floating-point, and
  will need to code your own versions.  A naive implementation is to return 0.0
  for getNaN and getInfinity, and false for isNaN and isInfinite.
  ---------------------------------------------------------------------------*/

U_CAPI UBool U_EXPORT2
uprv_isNaN(ದ್ವಿಗುಣ number)
{
#ಆದರೆ IEEE_754
    ಬಿಟ್_ನಮೂನೆ_Conversion converted_ಸಂಖ್ಯೆ;
    converted_ಸಂಖ್ಯೆ.d64 = number;
    /* Infinity is 0x7FF0000000000000U. Anything greater than that is a NaN */
    ಹಿಂತಿರುಗಿ (converted_ಸಂಖ್ಯೆ.i64 & U_INT64_MAX) > gInf.i64;

#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_OS390
    uint32_t ಮೇಲಿನ_ಬಿಟ್ಟುಗಳು = *(uint32_t*)u_ತುದಿ_NBytes_ದ_Double(&number,
                        ಗಾತ್ರ(uint32_t));
    uint32_t ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು  = *(uint32_t*)u_bottom_NBytes_ದ_Double(&number,
                        ಗಾತ್ರ(uint32_t));

    ಹಿಂತಿರುಗಿ ((ಮೇಲಿನ_ಬಿಟ್ಟುಗಳು & 0x7F080000L) == 0x7F080000L) &&
      (ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು == 0x00000000L);

#ಇಲ್ಲದಿದ್ದರೆ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    ಹಿಂತಿರುಗಿ number != number;
#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI UBool U_EXPORT2
uprv_isInfinite(ದ್ವಿಗುಣ number)
{
#ಆದರೆ IEEE_754
    ಬಿಟ್_ನಮೂನೆ_Conversion converted_ಸಂಖ್ಯೆ;
    converted_ಸಂಖ್ಯೆ.d64 = number;
    /* Infinity is exactly 0x7FF0000000000000U. */
    ಹಿಂತಿರುಗಿ (converted_ಸಂಖ್ಯೆ.i64 & U_INT64_MAX) == gInf.i64;
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_OS390
    uint32_t ಮೇಲಿನ_ಬಿಟ್ಟುಗಳು = *(uint32_t*)u_ತುದಿ_NBytes_ದ_Double(&number,
                        ಗಾತ್ರ(uint32_t));
    uint32_t ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು  = *(uint32_t*)u_bottom_NBytes_ದ_Double(&number,
                        ಗಾತ್ರ(uint32_t));

    ಹಿಂತಿರುಗಿ ((ಮೇಲಿನ_ಬಿಟ್ಟುಗಳು  & ~SIGN) == 0x70FF0000L) && (ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು == 0x00000000L);

#ಇಲ್ಲದಿದ್ದರೆ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    ಹಿಂತಿರುಗಿ number == (2.0 * number);
#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI UBool U_EXPORT2
uprv_is_ಧನಾತ್ಮಕ_Infinity(ದ್ವಿಗುಣ number)
{
#ಆದರೆ IEEE_754 || U_PLATFORM == U_PF_OS390
    ಹಿಂತಿರುಗಿ number > 0 && uprv_isInfinite(number);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ uprv_isInfinite(number);
#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI UBool U_EXPORT2
uprv_isNegativeInfinity(ದ್ವಿಗುಣ number)
{
#ಆದರೆ IEEE_754 || U_PLATFORM == U_PF_OS390
    ಹಿಂತಿರುಗಿ number < 0 && uprv_isInfinite(number);

#ಇಲ್ಲದಿದ್ದರೆ
    uint32_t ಮೇಲಿನ_ಬಿಟ್ಟುಗಳು = *(uint32_t*)u_ತುದಿ_NBytes_ದ_Double(&number,
                        ಗಾತ್ರ(uint32_t));
    ಹಿಂತಿರುಗಿ((ಮೇಲಿನ_ಬಿಟ್ಟುಗಳು & SIGN) && uprv_isInfinite(number));

#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_getNaN()
{
#ಆದರೆ IEEE_754 || U_PLATFORM == U_PF_OS390
    ಹಿಂತಿರುಗಿ gNan.d64;
#ಇಲ್ಲದಿದ್ದರೆ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    ಹಿಂತಿರುಗಿ 0.0;
#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_getInfinity()
{
#ಆದರೆ IEEE_754 || U_PLATFORM == U_PF_OS390
    ಹಿಂತಿರುಗಿ gInf.d64;
#ಇಲ್ಲದಿದ್ದರೆ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    ಹಿಂತಿರುಗಿ 0.0;
#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_floor(ದ್ವಿಗುಣ x)
{
    ಹಿಂತಿರುಗಿ floor(x);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_ceil(ದ್ವಿಗುಣ x)
{
    ಹಿಂತಿರುಗಿ ceil(x);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_ಸುತ್ತು(ದ್ವಿಗುಣ x)
{
    ಹಿಂತಿರುಗಿ uprv_floor(x + 0.5);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_fabs(ದ್ವಿಗುಣ x)
{
    ಹಿಂತಿರುಗಿ fabs(x);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_modf(ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ* y)
{
    ಹಿಂತಿರುಗಿ modf(x, y);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_fmod(ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y)
{
    ಹಿಂತಿರುಗಿ fmod(x, y);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_pow(ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y)
{
    /* This is declared as "double pow(double x, double y)" */
    ಹಿಂತಿರುಗಿ ಘಾತ(x, y);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_pow10(int32_t x)
{
    ಹಿಂತಿರುಗಿ ಘಾತ(10.0, (ದ್ವಿಗುಣ)x);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_fmax(ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y)
{
#ಆದರೆ IEEE_754
    /* first handle NaN*/
    ಆದರೆ(uprv_isNaN(x) || uprv_isNaN(y))
        ಹಿಂತಿರುಗಿ uprv_getNaN();

    /* check for -0 and 0*/
    ಆದರೆ(x == 0.0 && y == 0.0 && u_sign_ಬಿಟ್(x))
        ಹಿಂತಿರುಗಿ y;

#ಆದರೆ_ಅಂತ್ಯ

    /* this should work for all flt point w/o NaN and Inf special cases */
    ಹಿಂತಿರುಗಿ (x > y ? x : y);
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_fmin(ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y)
{
#ಆದರೆ IEEE_754
    /* first handle NaN*/
    ಆದರೆ(uprv_isNaN(x) || uprv_isNaN(y))
        ಹಿಂತಿರುಗಿ uprv_getNaN();

    /* check for -0 and 0*/
    ಆದರೆ(x == 0.0 && y == 0.0 && u_sign_ಬಿಟ್(y))
        ಹಿಂತಿರುಗಿ y;

#ಆದರೆ_ಅಂತ್ಯ

    /* this should work for all flt point w/o NaN and Inf special cases */
    ಹಿಂತಿರುಗಿ (x > y ? y : x);
}

U_CAPI UBool U_EXPORT2
uprv_add32_ಅತಿಪ್ರವಾಹ(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_add_overflow.
    // This function could be optimized by calling one of those primitives.
    ಸ್ವಯಂಚಾಲಿತ a64 = static_cast<int64_t>(a);
    ಸ್ವಯಂಚಾಲಿತ b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 + b64;
    *res = static_cast<int32_t>(res64);
    ಹಿಂತಿರುಗಿ res64 != *res;
}

U_CAPI UBool U_EXPORT2
uprv_mul32_ಅತಿಪ್ರವಾಹ(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_mul_overflow.
    // This function could be optimized by calling one of those primitives.
    ಸ್ವಯಂಚಾಲಿತ a64 = static_cast<int64_t>(a);
    ಸ್ವಯಂಚಾಲಿತ b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 * b64;
    *res = static_cast<int32_t>(res64);
    ಹಿಂತಿರುಗಿ res64 != *res;
}

/**
 * Truncates the given double.
 * trunc(3.3) = 3.0, trunc (-3.3) = -3.0
 * This is different than calling floor() or ceil():
 * floor(3.3) = 3, floor(-3.3) = -4
 * ceil(3.3) = 4, ceil(-3.3) = -3
 */
U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_trunc(ದ್ವಿಗುಣ d)
{
#ಆದರೆ IEEE_754
    /* handle error cases*/
    ಆದರೆ(uprv_isNaN(d))
        ಹಿಂತಿರುಗಿ uprv_getNaN();
    ಆದರೆ(uprv_isInfinite(d))
        ಹಿಂತಿರುಗಿ uprv_getInfinity();

    ಆದರೆ(u_sign_ಬಿಟ್(d))    /* Signbit() picks up -0.0;  d<0 does not. */
        ಹಿಂತಿರುಗಿ ceil(d);
    ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ floor(d);

#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ d >= 0 ? floor(d) : ceil(d);

#ಆದರೆ_ಅಂತ್ಯ
}

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 */
U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_maxMantissa()
{
    ಹಿಂತಿರುಗಿ ಘಾತ(2.0, DBL_MANT_DIG + 1.0) - 1.0;
}

U_CAPI ದ್ವಿಗುಣ U_EXPORT2
uprv_log(ದ್ವಿಗುಣ d)
{
    ಹಿಂತಿರುಗಿ ಲಘುಗಣಕ(d);
}

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_ಗರಿಷ್ಠ_Ptr(ಶೂನ್ಯ * ತಳ)
{
#ಆದರೆ U_PLATFORM == U_PF_OS400
    /*
     * With the provided function we should never be out of range of a given segment
     * (a traditional/typical segment that is).  Our segments have 5 bytes for the
     * id and 3 bytes for the offset.  The key is that the casting takes care of
     * only retrieving the offset portion minus x1000.  Hence, the smallest offset
     * seen in a program is x001000 and when casted to an int would be 0.
     * That's why we can only add 0xffefff.  Otherwise, we would exceed the segment.
     *
     * Currently, 16MB is the current addressing limitation on i5/OS if the activation is
     * non-TERASPACE.  If it is TERASPACE it is 2GB - 4k(header information).
     * This function determines the activation based on the pointer that is passed in and
     * calculates the appropriate maximum available size for
     * each pointer type (TERASPACE and non-TERASPACE)
     *
     * Unlike other operating systems, the pointer model isn't determined at
     * compile time on i5/OS.
     */
    ಆದರೆ ((ತಳ != ಶೂನ್ಯಸೂಚಕ) && (_TESTPTR(ತಳ, _C_TERASPACE_CHECK))) {
        /* if it is a TERASPACE pointer the max is 2GB - 4k */
        ಹಿಂತಿರುಗಿ ((ಶೂನ್ಯ *)(((ಅಕ್ಷರ *)ತಳ)-((uint32_t)(ತಳ))+((uint32_t)0x7fffefff)));
    }
    /* otherwise 16MB since nullptr ptr is not checkable or the ptr is not TERASPACE */
    ಹಿಂತಿರುಗಿ ((ಶೂನ್ಯ *)(((ಅಕ್ಷರ *)ತಳ)-((uint32_t)(ತಳ))+((uint32_t)0xffefff)));

#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ U_MAX_PTR(ತಳ);
#ಆದರೆ_ಅಂತ್ಯ
}

/*---------------------------------------------------------------------------
  Platform-specific Implementations
  Try these, and if they don't work on your platform, then special case your
  platform with new implementations.
  ---------------------------------------------------------------------------*/

/* Generic time zone layer -------------------------------------------------- */

/* Time zone utilities */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_tzset()
{
#ಆದರೆ defined(U_TZSET)
    U_TZSET();
#ಇಲ್ಲದಿದ್ದರೆ
    /* no initialization*/
#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI int32_t U_EXPORT2
uprv_timezone()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_TIMEZONE
    ಹಿಂತಿರುಗಿ U_TIMEZONE;
#ಇಲ್ಲದಿದ್ದರೆ
    time_t t, t1, t2;
    ರಚನೆ tm tmrec;
    int32_t tdiff = 0;

    time(&t);
    uprv_memcpy( &tmrec, localtime(&t), ಗಾತ್ರ(tmrec) );
#ಆದರೆ U_PLATFORM != U_PF_IPHONE
    UBool dst_checked = (tmrec.tm_isdst != 0); /* daylight savings time is checked*/
#ಆದರೆ_ಅಂತ್ಯ
    t1 = mktime(&tmrec);                 /* local time in seconds*/
    uprv_memcpy( &tmrec, gmtime(&t), ಗಾತ್ರ(tmrec) );
    t2 = mktime(&tmrec);                 /* GMT (or UTC) in seconds*/
    tdiff = t2 - t1;

#ಆದರೆ U_PLATFORM != U_PF_IPHONE
    /* imitate NT behaviour, which returns same timezone offset to GMT for
       winter and summer.
       This does not work on all platforms. For instance, on glibc on Linux
       and on Mac OS 10.5, tdiff calculated above remains the same
       regardless of whether DST is in effect or not. iOS is another
       platform where this does not work. Linux + glibc and Mac OS 10.5
       have U_TIMEZONE defined so that this code is not reached.
    */
    ಆದರೆ (dst_checked)
        tdiff += 3600;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ tdiff;
#ಆದರೆ_ಅಂತ್ಯ
}

/* Note that U_TZNAME does *not* have to be tzname, but if it is,
   some platforms need to have it declared here. */

#ಆದರೆ defined(U_TZNAME) && (U_PLATFORM == U_PF_IRIX || U_PLATFORM_IS_DARWIN_BASED)
/* RS6000 and others reject char **tzname.  */
ಬಾಹ್ಯ U_IMPORT ಅಕ್ಷರ *U_TZNAME[];
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !UCONFIG_NO_FILE_IO && ((U_PLATFORM_IS_DARWIN_BASED && (U_PLATFORM != U_PF_IPHONE || defined(U_TIMEZONE))) || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS)
/* These platforms are likely to use Olson timezone IDs. */
/* common targets of the symbolic link at TZDEFAULT are:
 * "/usr/share/zoneinfo/<olsonID>" default, older Linux distros, macOS to 10.12
 * "../usr/share/zoneinfo/<olsonID>" newer Linux distros: Red Hat Enterprise Linux 7, Ubuntu 16, SuSe Linux 12
 * "/usr/share/lib/zoneinfo/<olsonID>" Solaris
 * "../usr/share/lib/zoneinfo/<olsonID>" Solaris
 * "/var/db/timezone/zoneinfo/<olsonID>" macOS 10.13
 * To avoid checking lots of paths, just check that the target path
 * before the <olsonID> ends with "/zoneinfo/", and the <olsonID> is valid.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು CHECK_LOCALTIME_LINK 1
#ಆದರೆ U_PLATFORM_IS_DARWIN_BASED
#ಅನ್ತರ್ಭಾವ <tzfile.h>
#ವ್ಯಾಖ್ಯಾನಿಸು TZZONEINFO      (TZDIR "/")
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_SOLARIS
#ವ್ಯಾಖ್ಯಾನಿಸು TZDEFAULT       "/etc/localtime"
#ವ್ಯಾಖ್ಯಾನಿಸು TZZONEINFO      "/usr/share/lib/zoneinfo/"
#ವ್ಯಾಖ್ಯಾನಿಸು TZ_ENV_CHECK    "localtime"
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು TZDEFAULT       "/etc/localtime"
#ವ್ಯಾಖ್ಯಾನಿಸು TZZONEINFO      "/usr/share/zoneinfo/"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು TZZONEINFOTAIL  "/zoneinfo/"
#ಆದರೆ U_HAVE_DIRENT_H
#ವ್ಯಾಖ್ಯಾನಿಸು TZFILE_SKIP     "posixrules" /* tz file to skip when searching. */
/* Some Linux distributions have 'localtime' in /usr/share/zoneinfo
   symlinked to /etc/localtime, which makes searchForTZFile return
   'localtime' when it's the first match. */
#ವ್ಯಾಖ್ಯಾನಿಸು TZFILE_SKIP2    "localtime"
#ವ್ಯಾಖ್ಯಾನಿಸು ಹುಡುಕು_TZFILE
#ಅನ್ತರ್ಭಾವ <dirent.h>  /* Needed to search through system timezone files */
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಅಕ್ಷರ g_ಸಮಯ_Zone_ಬಫರ್[ಮಾರ್ಗ_MAX];
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *g_ಸಮಯ_Zone_ಬಫರ್_Ptr = ಶೂನ್ಯಸೂಚಕ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
#ವ್ಯಾಖ್ಯಾನಿಸು isNonDigit(ch) (ch < '0' || '9' < ch)
#ವ್ಯಾಖ್ಯಾನಿಸು isDigit(ch) ('0' <= ch && ch <= '9')
ಸ್ಥಿರ UBool isValidOlsonID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id) {
    int32_t idx = 0;
    int32_t idxMax = 0;

    /* Determine if this is something like Iceland (Olson ID)
    or AST4ADT (non-Olson ID) */
    ಆಗ (id[idx] && isNonDigit(id[idx]) && id[idx] != ',') {
        idx++;
    }

    /* Allow at maximum 2 numbers at the end of the id to support zone id's
    like GMT+11. */
    idxMax = idx + 2;
    ಆಗ (id[idx] && isDigit(id[idx]) && idx < idxMax) {
        idx++;
    }

    /* If we went through the whole string, then it might be okay.
    The timezone is sometimes set to "CST-7CDT", "CST6CDT5,J129,J131/19:30",
    "GRNLNDST3GRNLNDDT" or similar, so we cannot use it.
    The rest of the time it could be an Olson ID. George */
    ಹಿಂತಿರುಗಿ id[idx] == 0
        || uprv_strcmp(id, "PST8PDT") == 0
        || uprv_strcmp(id, "MST7MDT") == 0
        || uprv_strcmp(id, "CST6CDT") == 0
        || uprv_strcmp(id, "EST5EDT") == 0;
}

/* On some Unix-like OS, 'posix' subdirectory in
   /usr/share/zoneinfo replicates the top-level contents. 'right'
   subdirectory has the same set of files, but individual files
   are different from those in the top-level directory or 'posix'
   because 'right' has files for TAI (Int'l Atomic Time) while 'posix'
   has files for UTC.
   When the first match for /etc/localtime is in either of them
   (usually in posix because 'right' has different file contents),
   or TZ environment variable points to one of them, createTimeZone
   fails because, say, 'posix/America/New_York' is not an Olson
   timezone id ('America/New_York' is). So, we have to skip
   'posix/' and 'right/' at the beginning. */
ಸ್ಥಿರ ಶೂನ್ಯ skipZoneIDPrefix(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ** id) {
    ಆದರೆ (uprv_strncmp(*id, "posix/", 6) == 0
        || uprv_strncmp(*id, "right/", 6) == 0)
    {
        *id += 6;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(U_TZNAME) && !U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API

#ವ್ಯಾಖ್ಯಾನಿಸು CONVERT_HOURS_ಗಮ್ಯ_SECONDS(ಸ್ಥಾನಾಂತರ) (int32_t)(ಸ್ಥಾನಾಂತರ*3600)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಸ್ಥಾನಾಂತರ_Zone_Mapping {
    int32_t ಸ್ಥಾನಾಂತರ_Seconds;
    int32_t daylightType; /* 0=U_DAYLIGHT_NONE, 1=daylight in June-U_DAYLIGHT_JUNE, 2=daylight in December=U_DAYLIGHT_DECEMBER*/
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *stdID;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dstID;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *olsonID;
} ಸ್ಥಾನಾಂತರ_Zone_Mapping;

ಎಣಿಕೆ { U_DAYLIGHT_NONE=0,U_DAYLIGHT_JUNE=1,U_DAYLIGHT_DECEMBER=2 };

/*
This list tries to disambiguate a set of abbreviated timezone IDs and offsets
and maps it to an Olson ID.
Before adding anything to this list, take a look at
icu/source/tools/tzcode/tz.alias
Sometimes no daylight savings (0) is important to define due to aliases.
This list can be tested with icu/source/test/compat/tzone.pl
More values could be added to daylightType to increase precision.
*/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ ಸ್ಥಾನಾಂತರ_Zone_Mapping ಸ್ಥಾನಾಂತರ_ZONE_MAPPINGS[] = {
    {-45900, 2, "CHAST", "CHADT", "Pacific/Chatham"},
    {-43200, 1, "PETT", "PETST", "Asia/Kamchatka"},
    {-43200, 2, "NZST", "NZDT", "Pacific/Auckland"},
    {-43200, 1, "ANAT", "ANAST", "Asia/Anadyr"},
    {-39600, 1, "MAGT", "MAGST", "Asia/Magadan"},
    {-37800, 2, "LHST", "LHST", "Australia/Lord_Howe"},
    {-36000, 2, "EST", "EST", "Australia/Sydney"},
    {-36000, 1, "SAKT", "SAKST", "Asia/Sakhalin"},
    {-36000, 1, "VLAT", "VLAST", "Asia/Vladivostok"},
    {-34200, 2, "CST", "CST", "Australia/South"},
    {-32400, 1, "YAKT", "YAKST", "Asia/Yakutsk"},
    {-32400, 1, "CHOT", "CHOST", "Asia/Choibalsan"},
    {-31500, 2, "CWST", "CWST", "Australia/Eucla"},
    {-28800, 1, "IRKT", "IRKST", "Asia/Irkutsk"},
    {-28800, 1, "ULAT", "ULAST", "Asia/Ulaanbaatar"},
    {-28800, 2, "WST", "WST", "Australia/West"},
    {-25200, 1, "HOVT", "HOVST", "Asia/Hovd"},
    {-25200, 1, "KRAT", "KRAST", "Asia/Krasnoyarsk"},
    {-21600, 1, "NOVT", "NOVST", "Asia/Novosibirsk"},
    {-21600, 1, "OMST", "OMSST", "Asia/Omsk"},
    {-18000, 1, "YEKT", "YEKST", "Asia/Yekaterinburg"},
    {-14400, 1, "SAMT", "SAMST", "Europe/Samara"},
    {-14400, 1, "AMT", "AMST", "Asia/Yerevan"},
    {-14400, 1, "AZT", "AZST", "Asia/Baku"},
    {-10800, 1, "AST", "ADT", "Asia/Baghdad"},
    {-10800, 1, "MSK", "MSD", "Europe/Moscow"},
    {-10800, 1, "VOLT", "VOLST", "Europe/Volgograd"},
    {-7200, 0, "EET", "CEST", "Africa/Tripoli"},
    {-7200, 1, "EET", "EEST", "Europe/Athens"}, /* Conflicts with Africa/Cairo */
    {-7200, 1, "IST", "IDT", "Asia/Jerusalem"},
    {-3600, 0, "CET", "WEST", "Africa/Algiers"},
    {-3600, 2, "WAT", "WAST", "Africa/Windhoek"},
    {0, 1, "GMT", "IST", "Europe/Dublin"},
    {0, 1, "GMT", "BST", "Europe/London"},
    {0, 0, "WET", "WEST", "Africa/Casablanca"},
    {0, 0, "WET", "WET", "Africa/El_Aaiun"},
    {3600, 1, "AZOT", "AZOST", "Atlantic/Azores"},
    {3600, 1, "EGT", "EGST", "America/Scoresbysund"},
    {10800, 1, "PMST", "PMDT", "America/Miquelon"},
    {10800, 2, "UYT", "UYST", "America/Montevideo"},
    {10800, 1, "WGT", "WGST", "America/Godthab"},
    {10800, 2, "BRT", "BRST", "Brazil/East"},
    {12600, 1, "NST", "NDT", "America/St_Johns"},
    {14400, 1, "AST", "ADT", "Canada/Atlantic"},
    {14400, 2, "AMT", "AMST", "America/Cuiaba"},
    {14400, 2, "CLT", "CLST", "Chile/Continental"},
    {14400, 2, "FKT", "FKST", "Atlantic/Stanley"},
    {14400, 2, "PYT", "PYST", "America/Asuncion"},
    {18000, 1, "CST", "CDT", "America/Havana"},
    {18000, 1, "EST", "EDT", "US/Eastern"}, /* Conflicts with America/Grand_Turk */
    {21600, 2, "EAST", "EASST", "Chile/EasterIsland"},
    {21600, 0, "CST", "MDT", "Canada/Saskatchewan"},
    {21600, 0, "CST", "CDT", "America/Guatemala"},
    {21600, 1, "CST", "CDT", "US/Central"}, /* Conflicts with Mexico/General */
    {25200, 1, "MST", "MDT", "US/Mountain"}, /* Conflicts with Mexico/BajaSur */
    {28800, 0, "PST", "PST", "Pacific/Pitcairn"},
    {28800, 1, "PST", "PDT", "US/Pacific"}, /* Conflicts with Mexico/BajaNorte */
    {32400, 1, "AKST", "AKDT", "US/Alaska"},
    {36000, 1, "HAST", "HADT", "US/Aleutian"}
};

/*#define DEBUG_TZNAME*/

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* remap_Short_ಸಮಯ_Zone(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *stdID, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dstID, int32_t daylightType, int32_t ಸ್ಥಾನಾಂತರ)
{
    int32_t idx;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_TZNAME
    fprintf(stderr, "TZ=%s std=%s dst=%s daylight=%d offset=%d\n", getenv("TZ"), stdID, dstID, daylightType, ಸ್ಥಾನಾಂತರ);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಾಗಿ (idx = 0; idx < UPRV_LENGTHOF(ಸ್ಥಾನಾಂತರ_ZONE_MAPPINGS); idx++)
    {
        ಆದರೆ (ಸ್ಥಾನಾಂತರ == ಸ್ಥಾನಾಂತರ_ZONE_MAPPINGS[idx].ಸ್ಥಾನಾಂತರ_Seconds
            && daylightType == ಸ್ಥಾನಾಂತರ_ZONE_MAPPINGS[idx].daylightType
            && strcmp(ಸ್ಥಾನಾಂತರ_ZONE_MAPPINGS[idx].stdID, stdID) == 0
            && strcmp(ಸ್ಥಾನಾಂತರ_ZONE_MAPPINGS[idx].dstID, dstID) == 0)
        {
            ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ_ZONE_MAPPINGS[idx].olsonID;
        }
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಹುಡುಕು_TZFILE
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_READ_ಗಾತ್ರ 512

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ DefaultTZInfo {
    ಅಕ್ಷರ* defaultTZBuffer;
    int64_t default_TZFile_ಗಾತ್ರ;
    FILE* defaultTZFilePtr;
    int32_t defaultTZPosition;
} DefaultTZInfo;

/*
 * This method compares the two files given to see if they are a match.
 * It is currently use to compare two TZ files.
 */
ಸ್ಥಿರ UBool compare_ದ್ವಿಮಾನ_Files(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* TZFileName, DefaultTZInfo* tzInfo) {
    FILE* file;
    int64_t ಗಾತ್ರ_File;
    int64_t ಗಾತ್ರ_File_ಎಡ;
    int32_t ಗಾತ್ರ_File_Read;
    int32_t ಗಾತ್ರ_File_ಗಮ್ಯ_Read;
    ಅಕ್ಷರ ಬಫರ್_File[MAX_READ_ಗಾತ್ರ];
    UBool ಫಲಿತಾಂಶ = ನಿಜ;

    U_ASSERT(tzInfo->defaultTZFilePtr != ಶೂನ್ಯಸೂಚಕ);
    file = fopen(TZFileName, "r");

    tzInfo->defaultTZPosition = 0; /* reset position to begin search */

    ಆದರೆ (file != ಶೂನ್ಯಸೂಚಕ) {
        /* First check that the file size are equal. */
        ಆದರೆ (tzInfo->default_TZFile_ಗಾತ್ರ == 0) {
            fseek(tzInfo->defaultTZFilePtr, 0, SEEK_END);
            tzInfo->default_TZFile_ಗಾತ್ರ = ftell(tzInfo->defaultTZFilePtr);
        }
        ಆದರೆ (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಗಾತ್ರ_File = ftell(file);
        ಆದರೆ (ಗಾತ್ರ_File == -1) {
            fclose(file);
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಗಾತ್ರ_File_ಎಡ = ಗಾತ್ರ_File;

        ಆದರೆ (ಗಾತ್ರ_File != tzInfo->default_TZFile_ಗಾತ್ರ) {
            ಫಲಿತಾಂಶ = ಸುಳ್ಳು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* Store the data from the files in separate buffers and
             * compare each byte to determine equality.
             */
            ಆದರೆ (tzInfo->defaultTZBuffer == ಶೂನ್ಯಸೂಚಕ) {
                ಆದರೆ (fseek(tzInfo->defaultTZFilePtr, 0, SEEK_SET) != 0) {
                    fclose(file);
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
                tzInfo->defaultTZBuffer = static_cast<ಅಕ್ಷರ*>(uprv_malloc(ಗಾತ್ರ(ಅಕ್ಷರ) * tzInfo->default_TZFile_ಗಾತ್ರ));
                ಗಾತ್ರ_File_Read = fread(tzInfo->defaultTZBuffer, 1, tzInfo->default_TZFile_ಗಾತ್ರ, tzInfo->defaultTZFilePtr);
            }
            ಆದರೆ (fseek(file, 0, SEEK_SET) != 0) {
                fclose(file);
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಆಗ(ಗಾತ್ರ_File_ಎಡ > 0) {
                uprv_memset(ಬಫರ್_File, 0, MAX_READ_ಗಾತ್ರ);
                ಗಾತ್ರ_File_ಗಮ್ಯ_Read = ಗಾತ್ರ_File_ಎಡ < MAX_READ_ಗಾತ್ರ ? ಗಾತ್ರ_File_ಎಡ : MAX_READ_ಗಾತ್ರ;

                ಗಾತ್ರ_File_Read = fread(ಬಫರ್_File, 1, ಗಾತ್ರ_File_ಗಮ್ಯ_Read, file);
                ಆದರೆ (ಗಾತ್ರ_File_Read == -1) {
                    fclose(file);
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
                ಆದರೆ (memcmp(tzInfo->defaultTZBuffer + tzInfo->defaultTZPosition, ಬಫರ್_File, ಗಾತ್ರ_File_Read) != 0) {
                    ಫಲಿತಾಂಶ = ಸುಳ್ಳು;
                    ನಿಲ್ಲಿಸು;
                }
                ಗಾತ್ರ_File_ಎಡ -= ಗಾತ್ರ_File_Read;
                tzInfo->defaultTZPosition += ಗಾತ್ರ_File_Read;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = ಸುಳ್ಳು;
    }

    ಆದರೆ (file != ಶೂನ್ಯಸೂಚಕ) {
        fclose(file);
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}


/* dirent also lists two entries: "." and ".." that we can safely ignore. */
#ವ್ಯಾಖ್ಯಾನಿಸು SKIP1 "."
#ವ್ಯಾಖ್ಯಾನಿಸು SKIP2 ".."
ಸ್ಥಿರ UBool U_CALLCONV putil_cleanup();
ಸ್ಥಿರ CharString *g_ಹುಡುಕು_TZFile_ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;

/*
 * This method recursively traverses the directory given for a matching TZ file and returns the first match.
 * This function is not thread safe - it uses a global, gSearchTZFileResult, to hold its results.
 */
ಸ್ಥಿರ ಅಕ್ಷರ* ಹುಡುಕು_For_TZFile(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮಾರ್ಗ, DefaultTZInfo* tzInfo) {
    DIR* dirp = ಶೂನ್ಯಸೂಚಕ;
    ರಚನೆ dirent* dir_ನಮೂದು = ಶೂನ್ಯಸೂಚಕ;
    ಅಕ್ಷರ* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;

    /* There is no reference file to compare against, so do not traverse the zoneinfo tree. */
    ಆದರೆ (tzInfo->defaultTZFilePtr == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    /* Save the current path */
    CharString curpath(ಮಾರ್ಗ, -1, ಸ್ಥಿತಿ_2);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹೋಗು cleanupAndReturn;
    }

    dirp = opendir(ಮಾರ್ಗ);
    ಆದರೆ (dirp == ಶೂನ್ಯಸೂಚಕ) {
        ಹೋಗು cleanupAndReturn;
    }

    ಆದರೆ (g_ಹುಡುಕು_TZFile_ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        g_ಹುಡುಕು_TZFile_ಫಲಿತಾಂಶ = ಹೊಸ CharString;
        ಆದರೆ (g_ಹುಡುಕು_TZFile_ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
            ಹೋಗು cleanupAndReturn;
        }
        ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    }

    /* Check each entry in the directory. */
    ಆಗ((dir_ನಮೂದು = readdir(dirp)) != ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* dirName = dir_ನಮೂದು->d_name;
        ಆದರೆ (uprv_strcmp(dirName, SKIP1) != 0 && uprv_strcmp(dirName, SKIP2) != 0
            && uprv_strcmp(TZFILE_SKIP, dirName) != 0 && uprv_strcmp(TZFILE_SKIP2, dirName) != 0) {
            /* Create a newpath with the new entry to test each entry in the directory. */
            CharString newpath(curpath, ಸ್ಥಿತಿ_2);
            newpath.append(dirName, -1, ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ನಿಲ್ಲಿಸು;
            }

            DIR* subDirp = ಶೂನ್ಯಸೂಚಕ;
            ಆದರೆ ((subDirp = opendir(newpath.ದತ್ತಾಂಶ())) != ಶೂನ್ಯಸೂಚಕ) {
                /* If this new path is a directory, make a recursive call with the newpath. */
                closedir(subDirp);
                newpath.append('/', ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                    ನಿಲ್ಲಿಸು;
                }
                ಫಲಿತಾಂಶ = ಹುಡುಕು_For_TZFile(newpath.ದತ್ತಾಂಶ(), tzInfo);
                /*
                 Have to get out here. Otherwise, we'd keep looking
                 and return the first match in the top-level directory
                 if there's a match in the top-level. If not, this function
                 would return nullptr and set gTimeZoneBufferPtr to nullptr in initDefault().
                 It worked without this in most cases because we have a fallback of calling
                 localtime_r to figure out the default timezone.
                */
                ಆದರೆ (ಫಲಿತಾಂಶ != ಶೂನ್ಯಸೂಚಕ)
                    ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ(compare_ದ್ವಿಮಾನ_Files(newpath.ದತ್ತಾಂಶ(), tzInfo)) {
                    int32_t amount_ಗಮ್ಯ_Skip = ಗಾತ್ರ(TZZONEINFO) - 1;
                    ಆದರೆ (amount_ಗಮ್ಯ_Skip > newpath.length()) {
                        amount_ಗಮ್ಯ_Skip = newpath.length();
                    }
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* zoneid = newpath.ದತ್ತಾಂಶ() + amount_ಗಮ್ಯ_Skip;
                    skipZoneIDPrefix(&zoneid);
                    g_ಹುಡುಕು_TZFile_ಫಲಿತಾಂಶ->clear();
                    g_ಹುಡುಕು_TZFile_ಫಲಿತಾಂಶ->append(zoneid, -1, ಸ್ಥಿತಿ_2);
                    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    ಫಲಿತಾಂಶ = g_ಹುಡುಕು_TZFile_ಫಲಿತಾಂಶ->ದತ್ತಾಂಶ();
                    /* Get out after the first one found. */
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
    }

  cleanupAndReturn:
    ಆದರೆ (dirp) {
        closedir(dirp);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ U_PLATFORM == U_PF_ANDROID
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ(system_property_read_callback)(ಸ್ಥಿರಾಂಕ prop_info* info,
                                           ಶೂನ್ಯ (*callback)(ಶೂನ್ಯ* cookie,
                                                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name,
                                                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value,
                                                            uint32_t serial),
                                           ಶೂನ್ಯ* cookie);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ(system_property_get)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*, ಅಕ್ಷರ*);

ಸ್ಥಿರ ಅಕ್ಷರ g_Android_ಸಮಯ_Zone[PROP_ಮೌಲ್ಯ_MAX] = { '\0' };

ಸ್ಥಿರ ಶೂನ್ಯ u_property_read(ಶೂನ್ಯ* cookie, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* value,
                            uint32_t serial) {
    uprv_strcpy((ಅಕ್ಷರ* )cookie, value);
}
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_tzname_clear_cache()
{
#ಆದರೆ U_PLATFORM == U_PF_ANDROID
    /* Android's timezone is stored in system property. */
    g_Android_ಸಮಯ_Zone[0] = '\0';
    ಶೂನ್ಯ* libc = dlopen("libc.so", RTLD_NOLOAD);
    ಆದರೆ (libc) {
        /* Android API 26+ has new API to get system property and old API
         * (__system_property_get) is deprecated */
        system_property_read_callback* property_read_callback =
            (system_property_read_callback*)dlsym(
                libc, "__system_property_read_callback");
        ಆದರೆ (property_read_callback) {
            ಸ್ಥಿರಾಂಕ prop_info* info =
                __system_property_find("persist.sys.timezone");
            ಆದರೆ (info) {
                property_read_callback(info, &u_property_read, g_Android_ಸಮಯ_Zone);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            system_property_get* property_get =
                (system_property_get*)dlsym(libc, "__system_property_get");
            ಆದರೆ (property_get) {
                property_get("persist.sys.timezone", g_Android_ಸಮಯ_Zone);
            }
        }
        dlclose(libc);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    g_ಸಮಯ_Zone_ಬಫರ್_Ptr = ಶೂನ್ಯಸೂಚಕ;
#ಆದರೆ_ಅಂತ್ಯ
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uprv_tzname(ಪೂರ್ಣಾಂಕ n)
{
    (ಶೂನ್ಯ)n; // Avoid unreferenced parameter warning.
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tzid = ಶೂನ್ಯಸೂಚಕ;
#ಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
    tzid = uprv_detect_Windows_ಸಮಯ_Zone();

    ಆದರೆ (tzid != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ tzid;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_TZNAME
    // The return value is free'd in timezone.cpp on Windows because
    // the other code path returns a pointer to a heap location.
    // If we don't have a name already, then tzname wouldn't be any
    // better, so just fall back.
    ಹಿಂತಿರುಗಿ uprv_strdup("");
#ಆದರೆ_ಅಂತ್ಯ // !U_TZNAME

#ಇಲ್ಲದಿದ್ದರೆ

/*#if U_PLATFORM_IS_DARWIN_BASED
    int ret;

    tzid = getenv("TZFILE");
    if (tzid != nullptr) {
        return tzid;
    }
#endif*/

/* This code can be temporarily disabled to test tzname resolution later on. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DEBUG_TZNAME
#ಆದರೆ U_PLATFORM == U_PF_ANDROID
    tzid = g_Android_ಸಮಯ_Zone;
#ಇಲ್ಲದಿದ್ದರೆ
    tzid = getenv("TZ");
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (tzid != ಶೂನ್ಯಸೂಚಕ && isValidOlsonID(tzid)
#ಆದರೆ U_PLATFORM == U_PF_SOLARIS
    /* Don't misinterpret TZ "localtime" on Solaris as a time zone name. */
        && uprv_strcmp(tzid, TZ_ENV_CHECK) != 0
#ಆದರೆ_ಅಂತ್ಯ
    ) {
        /* The colon forces tzset() to treat the remainder as zoneinfo path */
        ಆದರೆ (tzid[0] == ':') {
            tzid++;
        }
        /* This might be a good Olson ID. */
        skipZoneIDPrefix(&tzid);
        ಹಿಂತಿರುಗಿ tzid;
    }
    /* else U_TZNAME will give a better result. */
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    /* Caller must handle threading issues */
    ಆದರೆ (g_ಸಮಯ_Zone_ಬಫರ್_Ptr == ಶೂನ್ಯಸೂಚಕ) {
        /*
        This is a trick to look at the name of the link to get the Olson ID
        because the tzfile contents is underspecified.
        This isn't guaranteed to work because it may not be a symlink.
        */
        ಅಕ್ಷರ *ret = realpath(TZDEFAULT, g_ಸಮಯ_Zone_ಬಫರ್);
        ಆದರೆ (ret != ಶೂನ್ಯಸೂಚಕ && uprv_strcmp(TZDEFAULT, g_ಸಮಯ_Zone_ಬಫರ್) != 0) {
            int32_t tz_Zone_Info_ಬಾಲ_Len = uprv_strlen(TZZONEINFOTAIL);
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tz_Zone_Info_ಬಾಲ_Ptr = uprv_strstr(g_ಸಮಯ_Zone_ಬಫರ್, TZZONEINFOTAIL);
            // MacOS14 has the realpath as something like
            // /usr/share/zoneinfo.default/Australia/Melbourne
            // which will not have "/zoneinfo/" in the path.
            // Therefore if we fail, we fall back to read the link which is
            // /var/db/timezone/zoneinfo/Australia/Melbourne
            // We also fall back to reading the link if the realpath leads to something like
            // /usr/share/zoneinfo/posixrules
            ಆದರೆ (tz_Zone_Info_ಬಾಲ_Ptr == ಶೂನ್ಯಸೂಚಕ ||
                    uprv_strcmp(tz_Zone_Info_ಬಾಲ_Ptr + tz_Zone_Info_ಬಾಲ_Len, "posixrules") == 0) {
                ssize_t size = readlink(TZDEFAULT, g_ಸಮಯ_Zone_ಬಫರ್, ಗಾತ್ರ(g_ಸಮಯ_Zone_ಬಫರ್)-1);
                ಆದರೆ (size > 0) {
                    g_ಸಮಯ_Zone_ಬಫರ್[size] = 0;
                    tz_Zone_Info_ಬಾಲ_Ptr = uprv_strstr(g_ಸಮಯ_Zone_ಬಫರ್, TZZONEINFOTAIL);
                }
            }
            ಆದರೆ (tz_Zone_Info_ಬಾಲ_Ptr != ಶೂನ್ಯಸೂಚಕ) {
                tz_Zone_Info_ಬಾಲ_Ptr += tz_Zone_Info_ಬಾಲ_Len;
                skipZoneIDPrefix(&tz_Zone_Info_ಬಾಲ_Ptr);
                ಆದರೆ (isValidOlsonID(tz_Zone_Info_ಬಾಲ_Ptr)) {
                    ಹಿಂತಿರುಗಿ (g_ಸಮಯ_Zone_ಬಫರ್_Ptr = tz_Zone_Info_ಬಾಲ_Ptr);
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ defined(ಹುಡುಕು_TZFILE)
            DefaultTZInfo* tzInfo = (DefaultTZInfo*)uprv_malloc(ಗಾತ್ರ(DefaultTZInfo));
            ಆದರೆ (tzInfo != ಶೂನ್ಯಸೂಚಕ) {
                tzInfo->defaultTZBuffer = ಶೂನ್ಯಸೂಚಕ;
                tzInfo->default_TZFile_ಗಾತ್ರ = 0;
                tzInfo->defaultTZFilePtr = fopen(TZDEFAULT, "r");
                tzInfo->defaultTZPosition = 0;

                g_ಸಮಯ_Zone_ಬಫರ್_Ptr = ಹುಡುಕು_For_TZFile(TZZONEINFO, tzInfo);

                /* Free previously allocated memory */
                ಆದರೆ (tzInfo->defaultTZBuffer != ಶೂನ್ಯಸೂಚಕ) {
                    uprv_free(tzInfo->defaultTZBuffer);
                }
                ಆದರೆ (tzInfo->defaultTZFilePtr != ಶೂನ್ಯಸೂಚಕ) {
                    fclose(tzInfo->defaultTZFilePtr);
                }
                uprv_free(tzInfo);
            }

            ಆದರೆ (g_ಸಮಯ_Zone_ಬಫರ್_Ptr != ಶೂನ್ಯಸೂಚಕ && isValidOlsonID(g_ಸಮಯ_Zone_ಬಫರ್_Ptr)) {
                ಹಿಂತಿರುಗಿ g_ಸಮಯ_Zone_ಬಫರ್_Ptr;
            }
#ಆದರೆ_ಅಂತ್ಯ
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ g_ಸಮಯ_Zone_ಬಫರ್_Ptr;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_TZNAME
#ಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
    /* The return value is free'd in timezone.cpp on Windows because
     * the other code path returns a pointer to a heap location. */
    ಹಿಂತಿರುಗಿ uprv_strdup(U_TZNAME[n]);
#ಇಲ್ಲದಿದ್ದರೆ
    /*
    U_TZNAME is usually a non-unique abbreviation, which isn't normally usable.
    So we remap the abbreviation to an olson ID.

    Since Windows exposes a little more timezone information,
    we normally don't use this code on Windows because
    uprv_detectWindowsTimeZone should have already given the correct answer.
    */
    {
        ರಚನೆ tm juneSol, decemberSol;
        ಪೂರ್ಣಾಂಕ daylightType;
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ time_t juneSolstice=1182478260; /*2007-06-21 18:11 UT*/
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ time_t decemberSolstice=1198332540; /*2007-12-22 06:09 UT*/

        /* This probing will tell us when daylight savings occurs.  */
        localtime_r(&juneSolstice, &juneSol);
        localtime_r(&decemberSolstice, &decemberSol);
        ಆದರೆ(decemberSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_DECEMBER;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(juneSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_JUNE;
        } ಇಲ್ಲದಿದ್ದರೆ {
          daylightType = U_DAYLIGHT_NONE;
        }
        tzid = remap_Short_ಸಮಯ_Zone(U_TZNAME[0], U_TZNAME[1], daylightType, uprv_timezone());
        ಆದರೆ (tzid != ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ tzid;
        }
    }
    ಹಿಂತಿರುಗಿ U_TZNAME[n];
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ "";
#ಆದರೆ_ಅಂತ್ಯ
}

/* Get and set the ICU data directory --------------------------------------- */

ಸ್ಥಿರ icu::UInitOnce g_ದತ್ತಾಂಶ_Dir_Init_Once {};
ಸ್ಥಿರ ಅಕ್ಷರ *g_ದತ್ತಾಂಶ_Directory = ಶೂನ್ಯಸೂಚಕ;

UInitOnce g_ಸಮಯ_Zone_Files_Init_Once {};
ಸ್ಥಿರ CharString *g_ಸಮಯ_Zone_Files_Directory = ಶೂನ್ಯಸೂಚಕ;

#ಆದರೆ U_POSIX_LOCALE || U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
 ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *gCorrectedPOSIXLocale = ಶೂನ್ಯಸೂಚಕ; /* Sometimes heap allocated */
 ಸ್ಥಿರ ತಾರ್ಕಿಕ g_Corrected_POSIXLocale_ರಾಶಿ_Allocated = ಸುಳ್ಳು;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ UBool U_CALLCONV putil_cleanup()
{
    ಆದರೆ (g_ದತ್ತಾಂಶ_Directory && *g_ದತ್ತಾಂಶ_Directory) {
        uprv_free(g_ದತ್ತಾಂಶ_Directory);
    }
    g_ದತ್ತಾಂಶ_Directory = ಶೂನ್ಯಸೂಚಕ;
    g_ದತ್ತಾಂಶ_Dir_Init_Once.reset();

    ಅಳಿಸು g_ಸಮಯ_Zone_Files_Directory;
    g_ಸಮಯ_Zone_Files_Directory = ಶೂನ್ಯಸೂಚಕ;
    g_ಸಮಯ_Zone_Files_Init_Once.reset();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಹುಡುಕು_TZFILE
    ಅಳಿಸು g_ಹುಡುಕು_TZFile_ಫಲಿತಾಂಶ;
    g_ಹುಡುಕು_TZFile_ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ U_POSIX_LOCALE || U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
    ಆದರೆ (gCorrectedPOSIXLocale && g_Corrected_POSIXLocale_ರಾಶಿ_Allocated) {
        uprv_free(const_cast<ಅಕ್ಷರ *>(gCorrectedPOSIXLocale));
        gCorrectedPOSIXLocale = ಶೂನ್ಯಸೂಚಕ;
        g_Corrected_POSIXLocale_ರಾಶಿ_Allocated = ಸುಳ್ಳು;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * Set the data directory.
 *    Make a copy of the passed string, and set the global data dir to point to it.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
u_ಗಣ_ದತ್ತಾಂಶ_Directory(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *directory) {
    ಅಕ್ಷರ *new_ದತ್ತಾಂಶ_Dir;
    int32_t length;

    ಆದರೆ(directory==ಶೂನ್ಯಸೂಚಕ || *directory==0) {
        /* A small optimization to prevent the malloc and copy when the
        shared library is used, and this is a way to make sure that nullptr
        is never returned.
        */
        new_ದತ್ತಾಂಶ_Dir = (ಅಕ್ಷರ *)"";
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        length=(int32_t)uprv_strlen(directory);
        new_ದತ್ತಾಂಶ_Dir = (ಅಕ್ಷರ *)uprv_malloc(length + 2);
        /* Exit out if newDataDir could not be created. */
        ಆದರೆ (new_ದತ್ತಾಂಶ_Dir == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ;
        }
        uprv_strcpy(new_ದತ್ತಾಂಶ_Dir, directory);

#ಆದರೆ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
        {
            ಅಕ್ಷರ *p;
            ಆಗ((p = uprv_strchr(new_ದತ್ತಾಂಶ_Dir, U_FILE_ALT_SEP_CHAR)) != ಶೂನ್ಯಸೂಚಕ) {
                *p = U_FILE_SEP_CHAR;
            }
        }
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (g_ದತ್ತಾಂಶ_Directory && *g_ದತ್ತಾಂಶ_Directory) {
        uprv_free(g_ದತ್ತಾಂಶ_Directory);
    }
    g_ದತ್ತಾಂಶ_Directory = new_ದತ್ತಾಂಶ_Dir;
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
}

U_CAPI UBool U_EXPORT2
uprv_ಮಾರ್ಗ_Is_Absolute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
  ಆದರೆ(!ಮಾರ್ಗ || !*ಮಾರ್ಗ) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  }

  ಆದರೆ(*ಮಾರ್ಗ == U_FILE_SEP_CHAR) {
    ಹಿಂತಿರುಗಿ ನಿಜ;
  }

#ಆದರೆ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
  ಆದರೆ(*ಮಾರ್ಗ == U_FILE_ALT_SEP_CHAR) {
    ಹಿಂತಿರುಗಿ ನಿಜ;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
  ಆದರೆ( (((ಮಾರ್ಗ[0] >= 'A') && (ಮಾರ್ಗ[0] <= 'Z')) ||
       ((ಮಾರ್ಗ[0] >= 'a') && (ಮಾರ್ಗ[0] <= 'z'))) &&
      ಮಾರ್ಗ[1] == ':' ) {
    ಹಿಂತಿರುಗಿ ನಿಜ;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/* Backup setting of ICU_DATA_DIR_PREFIX_ENV_VAR
   (needed for some Darwin ICU build environments) */
#ಆದರೆ U_PLATFORM_IS_DARWIN_BASED && defined(ಗುರಿ_OS_SIMULATOR) && ಗುರಿ_OS_SIMULATOR
# ಆದರೆ !defined(ICU_ದತ್ತಾಂಶ_DIR_ಪೂರ್ವಪ್ರತ್ಯಯ_ENV_VAR)
#  define ICU_ದತ್ತಾಂಶ_DIR_ಪೂರ್ವಪ್ರತ್ಯಯ_ENV_VAR "IPHONE_SIMULATOR_ROOT"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)
// Helper function to get the ICU Data Directory under the Windows directory location.
ಸ್ಥಿರ BOOL U_CALLCONV get_Icu_ದತ್ತಾಂಶ_Directory_Under_Windows_Directory(ಅಕ್ಷರ* directory_ಬಫರ್, UINT ಬಫರ್_ಉದ್ದಳತೆ)
{
    wchar_t windows_ಮಾರ್ಗ[MAX_ಮಾರ್ಗ];
    ಅಕ್ಷರ windows_ಮಾರ್ಗ_Utf8[MAX_ಮಾರ್ಗ];

    UINT length = GetSystemWindowsDirectoryW(windows_ಮಾರ್ಗ, UPRV_LENGTHOF(windows_ಮಾರ್ಗ));
    ಆದರೆ ((length > 0) && (length < (UPRV_LENGTHOF(windows_ಮಾರ್ಗ) - 1))) {
        // Convert UTF-16 to a UTF-8 string.
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        int32_t windows_ಮಾರ್ಗ_Utf8_Len = 0;
        u_str_ಗಮ್ಯ_UTF8(windows_ಮಾರ್ಗ_Utf8, static_cast<int32_t>(UPRV_LENGTHOF(windows_ಮಾರ್ಗ_Utf8)),
            &windows_ಮಾರ್ಗ_Utf8_Len, reinterpret_cast<ಸ್ಥಿರಾಂಕ char16_t*>(windows_ಮಾರ್ಗ), -1, &ಸ್ಥಿತಿ_2);

        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2) && (ಸ್ಥಿತಿ_2 != U_STRING_NOT_TERMINATED_WARNING) &&
            (windows_ಮಾರ್ಗ_Utf8_Len < (UPRV_LENGTHOF(windows_ಮಾರ್ಗ_Utf8) - 1))) {
            // Ensure it always has a separator, so we can append the ICU data path.
            ಆದರೆ (windows_ಮಾರ್ಗ_Utf8[windows_ಮಾರ್ಗ_Utf8_Len - 1] != U_FILE_SEP_CHAR) {
                windows_ಮಾರ್ಗ_Utf8[windows_ಮಾರ್ಗ_Utf8_Len++] = U_FILE_SEP_CHAR;
                windows_ಮಾರ್ಗ_Utf8[windows_ಮಾರ್ಗ_Utf8_Len] = '\0';
            }
            // Check if the concatenated string will fit.
            ಆದರೆ ((windows_ಮಾರ್ಗ_Utf8_Len + UPRV_LENGTHOF(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)) < ಬಫರ್_ಉದ್ದಳತೆ) {
                uprv_strcpy(directory_ಬಫರ್, windows_ಮಾರ್ಗ_Utf8);
                uprv_strcat(directory_ಬಫರ್, ICU_ದತ್ತಾಂಶ_DIR_WINDOWS);
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }
    }

    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV ದತ್ತಾಂಶ_Directory_Init_Fn() {
    /* If we already have the directory, then return immediately. Will happen if user called
     * u_setDataDirectory().
     */
    ಆದರೆ (g_ದತ್ತಾಂಶ_Directory) {
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;

    /*
    When ICU_NO_USER_DATA_OVERRIDE is defined, users aren't allowed to
    override ICU's data with the ICU_DATA environment variable. This prevents
    problems where multiple custom copies of ICU's specific version of data
    are installed on a system. Either the application must define the data
    directory with u_setDataDirectory, define ICU_DATA_DIR when compiling
    ICU, set the data with udata_setCommonData or trust that all of the
    required data is contained in ICU's data library that contains
    the entry point defined by U_ICUDATA_ENTRY_POINT.

    There may also be some platforms where environment variables
    are not allowed.
    */
#   ಆದರೆ !defined(ICU_NO_USER_ದತ್ತಾಂಶ_OVERRIDE) && !UCONFIG_NO_FILE_IO
    /* First try to get the environment variable */
#     ಆದರೆ U_PLATFORM_HAS_WINUWP_API == 0  // Windows UWP does not support getenv
        ಮಾರ್ಗ=getenv("ICU_DATA");
#     endif
#   endif

    /* ICU_DATA_DIR may be set as a compile option.
     * U_ICU_DATA_DEFAULT_DIR is provided and is set by ICU at compile time
     * and is used only when data is built in archive mode eliminating the need
     * for ICU_DATA_DIR to be set. U_ICU_DATA_DEFAULT_DIR is set to the installation
     * directory of the data dat file. Users should use ICU_DATA_DIR if they want to
     * set their own path.
     */
#ಆದರೆ defined(ICU_ದತ್ತಾಂಶ_DIR) || defined(U_ICU_ದತ್ತಾಂಶ_DEFAULT_DIR)
# ಆದರೆ defined(ICU_ದತ್ತಾಂಶ_DIR_ಪೂರ್ವಪ್ರತ್ಯಯ_ENV_VAR)
    ಅಕ್ಷರ datadir_ಮಾರ್ಗ_ಬಫರ್[ಮಾರ್ಗ_MAX];
# endif
    ಆದರೆ(ಮಾರ್ಗ==ಶೂನ್ಯಸೂಚಕ || *ಮಾರ್ಗ==0) {
# ಆದರೆ defined(ICU_ದತ್ತಾಂಶ_DIR_ಪೂರ್ವಪ್ರತ್ಯಯ_ENV_VAR)
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ = getenv(ICU_ದತ್ತಾಂಶ_DIR_ಪೂರ್ವಪ್ರತ್ಯಯ_ENV_VAR);
# endif
# ifdef ICU_ದತ್ತಾಂಶ_DIR
        ಮಾರ್ಗ=ICU_ದತ್ತಾಂಶ_DIR;
# ಇಲ್ಲದಿದ್ದರೆ
        ಮಾರ್ಗ=U_ICU_ದತ್ತಾಂಶ_DEFAULT_DIR;
# endif
# ಆದರೆ defined(ICU_ದತ್ತಾಂಶ_DIR_ಪೂರ್ವಪ್ರತ್ಯಯ_ENV_VAR)
        ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ != ಶೂನ್ಯಸೂಚಕ) {
            snprintf(datadir_ಮಾರ್ಗ_ಬಫರ್, ಗಾತ್ರ(datadir_ಮಾರ್ಗ_ಬಫರ್), "%s%s", ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ);
            ಮಾರ್ಗ=datadir_ಮಾರ್ಗ_ಬಫರ್;
        }
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)
    ಅಕ್ಷರ datadir_ಮಾರ್ಗ_ಬಫರ್[MAX_ಮಾರ್ಗ];
    ಆದರೆ (get_Icu_ದತ್ತಾಂಶ_Directory_Under_Windows_Directory(datadir_ಮಾರ್ಗ_ಬಫರ್, UPRV_LENGTHOF(datadir_ಮಾರ್ಗ_ಬಫರ್))) {
        ಮಾರ್ಗ = datadir_ಮಾರ್ಗ_ಬಫರ್;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(ಮಾರ್ಗ==ಶೂನ್ಯಸೂಚಕ) {
        /* It looks really bad, set it to something. */
        ಮಾರ್ಗ = "";
    }

    u_ಗಣ_ದತ್ತಾಂಶ_Directory(ಮಾರ್ಗ);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_EXPORT2
u_get_ದತ್ತಾಂಶ_Directory() {
    umtx_initOnce(g_ದತ್ತಾಂಶ_Dir_Init_Once, &ದತ್ತಾಂಶ_Directory_Init_Fn);
    ಹಿಂತಿರುಗಿ g_ದತ್ತಾಂಶ_Directory;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_ಸಮಯ_Zone_Files_Dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ;
    }
    g_ಸಮಯ_Zone_Files_Directory->clear();
    g_ಸಮಯ_Zone_Files_Directory->append(ಮಾರ್ಗ, ಸ್ಥಿತಿ_2);
#ಆದರೆ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    ಅಕ್ಷರ *p = g_ಸಮಯ_Zone_Files_Directory->ದತ್ತಾಂಶ();
    ಆಗ ((p = uprv_strchr(p, U_FILE_ALT_SEP_CHAR)) != ಶೂನ್ಯಸೂಚಕ) {
        *p = U_FILE_SEP_CHAR;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_STRING(x) ಗಮ್ಯ_STRING_2(x)
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಮ್ಯ_STRING_2(x) #x

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV ಸಮಯ_Zone_ದತ್ತಾಂಶ_Dir_Init_Fn(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(g_ಸಮಯ_Zone_Files_Directory == ಶೂನ್ಯಸೂಚಕ);
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    g_ಸಮಯ_Zone_Files_Directory = ಹೊಸ CharString();
    ಆದರೆ (g_ಸಮಯ_Zone_Files_Directory == ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir = "";

#ಆದರೆ defined(ICU_TIMEZONE_FILES_DIR_ಪೂರ್ವಪ್ರತ್ಯಯ_ENV_VAR)
    ಅಕ್ಷರ timezonefilesdir_ಮಾರ್ಗ_ಬಫರ್[ಮಾರ್ಗ_MAX];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ = getenv(ICU_TIMEZONE_FILES_DIR_ಪೂರ್ವಪ್ರತ್ಯಯ_ENV_VAR);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ U_PLATFORM_HAS_WINUWP_API == 1
// The UWP version does not support the environment variable setting.

# ಆದರೆ defined(ICU_ದತ್ತಾಂಶ_DIR_WINDOWS)
    // When using the Windows system data, we can possibly pick up time zone data from the Windows directory.
    ಅಕ್ಷರ datadir_ಮಾರ್ಗ_ಬಫರ್[MAX_ಮಾರ್ಗ];
    ಆದರೆ (get_Icu_ದತ್ತಾಂಶ_Directory_Under_Windows_Directory(datadir_ಮಾರ್ಗ_ಬಫರ್, UPRV_LENGTHOF(datadir_ಮಾರ್ಗ_ಬಫರ್))) {
        dir = datadir_ಮಾರ್ಗ_ಬಫರ್;
    }
# endif

#ಇಲ್ಲದಿದ್ದರೆ
    dir = getenv("ICU_TIMEZONE_FILES_DIR");
#ಆದರೆ_ಅಂತ್ಯ // U_PLATFORM_HAS_WINUWP_API

#ಆದರೆ defined(U_TIMEZONE_FILES_DIR)
    ಆದರೆ (dir == ಶೂನ್ಯಸೂಚಕ) {
        // Build time configuration setting.
        dir = ಗಮ್ಯ_STRING(U_TIMEZONE_FILES_DIR);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (dir == ಶೂನ್ಯಸೂಚಕ) {
        dir = "";
    }

#ಆದರೆ defined(ICU_TIMEZONE_FILES_DIR_ಪೂರ್ವಪ್ರತ್ಯಯ_ENV_VAR)
    ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ != ಶೂನ್ಯಸೂಚಕ) {
        snprintf(timezonefilesdir_ಮಾರ್ಗ_ಬಫರ್, ಗಾತ್ರ(timezonefilesdir_ಮಾರ್ಗ_ಬಫರ್), "%s%s", ಪೂರ್ವಪ್ರತ್ಯಯ, dir);
        dir = timezonefilesdir_ಮಾರ್ಗ_ಬಫರ್;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಗಣ_ಸಮಯ_Zone_Files_Dir(dir, ಸ್ಥಿತಿ_2);
}


U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_EXPORT2
u_get_ಸಮಯ_Zone_Files_Directory(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    umtx_initOnce(g_ಸಮಯ_Zone_Files_Init_Once, &ಸಮಯ_Zone_ದತ್ತಾಂಶ_Dir_Init_Fn, *ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ U_SUCCESS(*ಸ್ಥಿತಿ_2) ? g_ಸಮಯ_Zone_Files_Directory->ದತ್ತಾಂಶ() : "";
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_ಗಣ_ಸಮಯ_Zone_Files_Directory(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    umtx_initOnce(g_ಸಮಯ_Zone_Files_Init_Once, &ಸಮಯ_Zone_ದತ್ತಾಂಶ_Dir_Init_Fn, *ಸ್ಥಿತಿ_2);
    ಗಣ_ಸಮಯ_Zone_Files_Dir(ಮಾರ್ಗ, *ಸ್ಥಿತಿ_2);

    // Note: this function does some extra churn, first setting based on the
    //       environment, then immediately replacing with the value passed in.
    //       The logic is simpler that way, and performance shouldn't be an issue.
}


#ಆದರೆ U_POSIX_LOCALE
/* A helper function used by uprv_getPOSIXIDForDefaultLocale and
 * uprv_getPOSIXIDForDefaultCodepage. Returns the posix locale id for
 * LC_CTYPE and LC_MESSAGES. It doesn't support other locale categories.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *uprv_getPOSIXIDForCategory(ಪೂರ್ಣಾಂಕ category)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* posixID = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (category == LC_MESSAGES || category == LC_CTYPE) {
        /*
        * On Solaris two different calls to setlocale can result in
        * different values. Only get this value once.
        *
        * We must check this first because an application can set this.
        *
        * LC_ALL can't be used because it's platform dependent. The LANG
        * environment variable seems to affect LC_CTYPE variable by default.
        * Here is what setlocale(LC_ALL, nullptr) can return.
        * HPUX can return 'C C C C C C C'
        * Solaris can return /en_US/C/C/C/C/C on the second try.
        * Linux can return LC_CTYPE=C;LC_NUMERIC=C;...
        *
        * The default codepage detection also needs to use LC_CTYPE.
        *
        * Do not call setlocale(LC_*, "")! Using an empty string instead
        * of nullptr, will modify the libc behavior.
        */
        posixID = setlocale(category, ಶೂನ್ಯಸೂಚಕ);
        ಆದರೆ ((posixID == ಶೂನ್ಯಸೂಚಕ)
            || (uprv_strcmp("C", posixID) == 0)
            || (uprv_strcmp("POSIX", posixID) == 0))
        {
            /* Maybe we got some garbage.  Try something more reasonable */
            posixID = getenv("LC_ALL");
            /* Solaris speaks POSIX -  See IEEE Std 1003.1-2008
             * This is needed to properly handle empty env. variables
             */
#ಆದರೆ U_PLATFORM == U_PF_SOLARIS
            ಆದರೆ ((posixID == 0) || (posixID[0] == '\0')) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                ಆದರೆ ((posixID == 0) || (posixID[0] == '\0')) {
#ಇಲ್ಲದಿದ್ದರೆ
            ಆದರೆ (posixID == ಶೂನ್ಯಸೂಚಕ) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                ಆದರೆ (posixID == ಶೂನ್ಯಸೂಚಕ) {
#ಆದರೆ_ಅಂತ್ಯ
                    posixID = getenv("LANG");
                }
            }
        }
    }
    ಆದರೆ ((posixID == ಶೂನ್ಯಸೂಚಕ)
        || (uprv_strcmp("C", posixID) == 0)
        || (uprv_strcmp("POSIX", posixID) == 0))
    {
        /* Nothing worked.  Give it a nice POSIX default value. */
        posixID = "en_US_POSIX";
        // Note: this test will not catch 'C.UTF-8',
        // that will be handled in uprv_getDefaultLocaleID().
        // Leave this mapping here for the uprv_getPOSIXIDForDefaultCodepage()
        // caller which expects to see "en_US_POSIX" in many branches.
    }
    ಹಿಂತಿರುಗಿ posixID;
}

/* Return just the POSIX id for the default locale, whatever happens to be in
 * it. It gets the value from LC_MESSAGES and indirectly from LC_ALL and LANG.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *uprv_getPOSIXIDForDefaultLocale()
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* posixID = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (posixID == ಶೂನ್ಯಸೂಚಕ) {
        posixID = uprv_getPOSIXIDForCategory(LC_MESSAGES);
    }
    ಹಿಂತಿರುಗಿ posixID;
}

#ಆದರೆ !U_CHARSET_IS_UTF8
/* Return just the POSIX id for the default codepage, whatever happens to be in
 * it. It gets the value from LC_CTYPE and indirectly from LC_ALL and LANG.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *uprv_getPOSIXIDForDefaultCodepage()
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* posixID = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (posixID == 0) {
        posixID = uprv_getPOSIXIDForCategory(LC_CTYPE);
    }
    ಹಿಂತಿರುಗಿ posixID;
}
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

/* NOTE: The caller should handle thread safety */
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uprv_getDefaultLocaleID()
{
#ಆದರೆ U_POSIX_LOCALE
/*
  Note that:  (a '!' means the ID is improper somehow)
     LC_ALL  ---->     default_loc          codepage
--------------------------------------------------------
     ab.CD             ab                   CD
     ab@CD             ab__CD               -
     ab@CD.EF          ab__CD               EF

     ab_CD.EF@GH       ab_CD_GH             EF

Some 'improper' ways to do the same as above:
  !  ab_CD@GH.EF       ab_CD_GH             EF
  !  ab_CD.EF@GH.IJ    ab_CD_GH             EF
  !  ab_CD@ZZ.EF@GH.IJ ab_CD_GH             EF

     _CD@GH            _CD_GH               -
     _CD.EF@GH         _CD_GH               EF

The variant cannot have dots in it.
The 'rightmost' variant (@xxx) wins.
The leftmost codepage (.xxx) wins.
*/
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* posixID = uprv_getPOSIXIDForDefaultLocale();

    /* Format: (no spaces)
    ll [ _CC ] [ . MM ] [ @ VV]

      l = lang, C = ctry, M = charmap, V = variant
    */

    ಆದರೆ (gCorrectedPOSIXLocale != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ gCorrectedPOSIXLocale;
    }

    // Copy the ID into owned memory.
    // Over-allocate in case we replace "C" with "en_US_POSIX" (+10), + null termination
    ಅಕ್ಷರ *correctedPOSIXLocale = static_cast<ಅಕ್ಷರ *>(uprv_malloc(uprv_strlen(posixID) + 10 + 1));
    ಆದರೆ (correctedPOSIXLocale == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    uprv_strcpy(correctedPOSIXLocale, posixID);

    ಅಕ್ಷರ *ಎಲ್ಲೆ;
    ಆದರೆ ((ಎಲ್ಲೆ = uprv_strchr(correctedPOSIXLocale, '.')) != ಶೂನ್ಯಸೂಚಕ) {
        *ಎಲ್ಲೆ = 0;
    }
    ಆದರೆ ((ಎಲ್ಲೆ = uprv_strchr(correctedPOSIXLocale, '@')) != ಶೂನ್ಯಸೂಚಕ) {
        *ಎಲ್ಲೆ = 0;
    }

    ಆದರೆ ((uprv_strcmp("C", correctedPOSIXLocale) == 0) // no @ variant
        || (uprv_strcmp("POSIX", correctedPOSIXLocale) == 0)) {
      // Raw input was C.* or POSIX.*, Give it a nice POSIX default value.
      // (The "C"/"POSIX" case is handled in uprv_getPOSIXIDForCategory())
      uprv_strcpy(correctedPOSIXLocale, "en_US_POSIX");
    }

    /* Note that we scan the *uncorrected* ID. */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
    ಆದರೆ ((p = uprv_strrchr(posixID, '@')) != ಶೂನ್ಯಸೂಚಕ) {
        p++;

        /* Take care of any special cases here.. */
        ಆದರೆ (!uprv_strcmp(p, "nynorsk")) {
            p = "NY";
            /* Don't worry about no__NY. In practice, it won't appear. */
        }

        ಆದರೆ (uprv_strchr(correctedPOSIXLocale,'_') == ಶೂನ್ಯಸೂಚಕ) {
            uprv_strcat(correctedPOSIXLocale, "__"); /* aa@b -> aa__b (note this can make the new locale 1 char longer) */
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            uprv_strcat(correctedPOSIXLocale, "_"); /* aa_CC@b -> aa_CC_b */
        }

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q;
        ಆದರೆ ((q = uprv_strchr(p, '.')) != ಶೂನ್ಯಸೂಚಕ) {
            /* How big will the resulting string be? */
            int32_t len = (int32_t)(uprv_strlen(correctedPOSIXLocale) + (q-p));
            uprv_strncat(correctedPOSIXLocale, p, q-p); // do not include charset
            correctedPOSIXLocale[len] = 0;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            /* Anything following the @ sign */
            uprv_strcat(correctedPOSIXLocale, p);
        }

        /* Should there be a map from 'no@nynorsk' -> no_NO_NY here?
         * How about 'russian' -> 'ru'?
         * Many of the other locales using ISO codes will be handled by the
         * canonicalization functions in uloc_getDefault.
         */
    }

    ಆದರೆ (gCorrectedPOSIXLocale == ಶೂನ್ಯಸೂಚಕ) {
        gCorrectedPOSIXLocale = correctedPOSIXLocale;
        g_Corrected_POSIXLocale_ರಾಶಿ_Allocated = ನಿಜ;
        ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
        correctedPOSIXLocale = ಶೂನ್ಯಸೂಚಕ;
    }
    posixID = gCorrectedPOSIXLocale;

    ಆದರೆ (correctedPOSIXLocale != ಶೂನ್ಯಸೂಚಕ) {  /* Was already set - clean up. */
        uprv_free(correctedPOSIXLocale);
    }

    ಹಿಂತಿರುಗಿ posixID;

#ಅಥವಾಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
#ವ್ಯಾಖ್ಯಾನಿಸು POSIX_LOCALE_ಸಾಮರ್ಥ್ಯ 64
    UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    ಅಕ್ಷರ *correctedPOSIXLocale = ಶೂನ್ಯಸೂಚಕ;

    // If we have already figured this out just use the cached value
    ಆದರೆ (gCorrectedPOSIXLocale != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ gCorrectedPOSIXLocale;
    }

    // No cached value, need to determine the current value
    ಸ್ಥಿರ WCHAR windowsLocale[LOCALE_NAME_MAX_ಉದ್ದಳತೆ] = {};
    ಪೂರ್ಣಾಂಕ length = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, windowsLocale, LOCALE_NAME_MAX_ಉದ್ದಳತೆ);

    // Now we should have a Windows locale name that needs converted to the POSIX style.
    ಆದರೆ (length > 0) // If length is 0, then the GetLocaleInfoEx failed.
    {
        // First we need to go from UTF-16 to char (and also convert from _ to - while we're at it.)
        ಅಕ್ಷರ modifiedWindowsLocale[LOCALE_NAME_MAX_ಉದ್ದಳತೆ] = {};

        int32_t i;
        ಗಾಗಿ (i = 0; i < UPRV_LENGTHOF(modifiedWindowsLocale); i++)
        {
            ಆದರೆ (windowsLocale[i] == '_')
            {
                modifiedWindowsLocale[i] = '-';
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                modifiedWindowsLocale[i] = static_cast<ಅಕ್ಷರ>(windowsLocale[i]);
            }

            ಆದರೆ (modifiedWindowsLocale[i] == '\0')
            {
                ನಿಲ್ಲಿಸು;
            }
        }

        ಆದರೆ (i >= UPRV_LENGTHOF(modifiedWindowsLocale))
        {
            // Ran out of room, can't really happen, maybe we'll be lucky about a matching
            // locale when tags are dropped
            modifiedWindowsLocale[UPRV_LENGTHOF(modifiedWindowsLocale) - 1] = '\0';
        }

        // Now normalize the resulting name
        correctedPOSIXLocale = static_cast<ಅಕ್ಷರ *>(uprv_malloc(POSIX_LOCALE_ಸಾಮರ್ಥ್ಯ + 1));
        /* TODO: Should we just exit on memory allocation failure? */
        ಆದರೆ (correctedPOSIXLocale)
        {
            int32_t posixLen = uloc_canonicalize(modifiedWindowsLocale, correctedPOSIXLocale, POSIX_LOCALE_ಸಾಮರ್ಥ್ಯ, &ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2))
            {
                *(correctedPOSIXLocale + posixLen) = 0;
                gCorrectedPOSIXLocale = correctedPOSIXLocale;
                g_Corrected_POSIXLocale_ರಾಶಿ_Allocated = ನಿಜ;
                ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
            }
            ಇಲ್ಲದಿದ್ದರೆ
            {
                uprv_free(correctedPOSIXLocale);
            }
        }
    }

    // If unable to find a locale we can agree upon, use en-US by default
    ಆದರೆ (gCorrectedPOSIXLocale == ಶೂನ್ಯಸೂಚಕ) {
        gCorrectedPOSIXLocale = "en_US";
    }
    ಹಿಂತಿರುಗಿ gCorrectedPOSIXLocale;

#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_OS400
    /* locales are process scoped and are by definition thread safe */
    ಸ್ಥಿರ ಅಕ್ಷರ correctedLocale[64];
    ಸ್ಥಿರಾಂಕ  ಅಕ್ಷರ *localeID = getenv("LC_ALL");
           ಅಕ್ಷರ *p;

    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ)
        localeID = getenv("LANG");
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ)
        localeID = setlocale(LC_ALL, ಶೂನ್ಯಸೂಚಕ);
    /* Make sure we have something... */
    ಆದರೆ (localeID == ಶೂನ್ಯಸೂಚಕ)
        ಹಿಂತಿರುಗಿ "en_US_POSIX";

    /* Extract the locale name from the path. */
    ಆದರೆ((p = uprv_strrchr(localeID, '/')) != ಶೂನ್ಯಸೂಚಕ)
    {
        /* Increment p to start of locale name. */
        p++;
        localeID = p;
    }

    /* Copy to work location. */
    uprv_strcpy(correctedLocale, localeID);

    /* Strip off the '.locale' extension. */
    ಆದರೆ((p = uprv_strchr(correctedLocale, '.')) != ಶೂನ್ಯಸೂಚಕ) {
        *p = 0;
    }

    /* Upper case the locale name. */
    T_CString_ಗಮ್ಯ_ಊರ್ಧ್ವ_Case(correctedLocale);

    /* See if we are using the POSIX locale.  Any of the
    * following are equivalent and use the same QLGPGCMA
    * (POSIX) locale.
    * QLGPGCMA2 means UCS2
    * QLGPGCMA_4 means UTF-32
    * QLGPGCMA_8 means UTF-8
    */
    ಆದರೆ ((uprv_strcmp("C", correctedLocale) == 0) ||
        (uprv_strcmp("POSIX", correctedLocale) == 0) ||
        (uprv_strncmp("QLGPGCMA", correctedLocale, 8) == 0))
    {
        uprv_strcpy(correctedLocale, "en_US_POSIX");
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
        int16_t LocaleLen;

        /* Lower case the lang portion. */
        ಗಾಗಿ(p = correctedLocale; *p != 0 && *p != '_'; p++)
        {
            *p = uprv_tolower(*p);
        }

        /* Adjust for Euro.  After '_E' add 'URO'. */
        LocaleLen = uprv_strlen(correctedLocale);
        ಆದರೆ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'E')
        {
            uprv_strcat(correctedLocale, "URO");
        }

        /* If using Lotus-based locale then convert to
         * equivalent non Lotus.
         */
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'L')
        {
            correctedLocale[LocaleLen - 2] = 0;
        }

        /* There are separate simplified and traditional
         * locales called zh_HK_S and zh_HK_T.
         */
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strncmp(correctedLocale, "zh_HK", 5) == 0)
        {
            uprv_strcpy(correctedLocale, "zh_HK");
        }

        /* A special zh_CN_GBK locale...
        */
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(correctedLocale, "zh_CN_GBK") == 0)
        {
            uprv_strcpy(correctedLocale, "zh_CN");
        }

    }

    ಹಿಂತಿರುಗಿ correctedLocale;
#ಆದರೆ_ಅಂತ್ಯ

}

#ಆದರೆ !U_CHARSET_IS_UTF8
#ಆದರೆ U_POSIX_LOCALE
/*
Due to various platform differences, one platform may specify a charset,
when they really mean a different charset. Remap the names so that they are
compatible with ICU. Only conflicting/ambiguous aliases should be resolved
here. Before adding anything to this function, please consider adding unique
names to the ICU alias table in the data directory.
*/
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
remapPlatformDependentCodepage(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
    ಆದರೆ (locale != ಶೂನ್ಯಸೂಚಕ && *locale == 0) {
        /* Make sure that an empty locale is handled the same way. */
        locale = ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ (name == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
#ಆದರೆ U_PLATFORM == U_PF_AIX
    ಆದರೆ (uprv_strcmp(name, "IBM-943") == 0) {
        /* Use the ASCII compatible ibm-943 */
        name = "Shift-JIS";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(name, "IBM-1252") == 0) {
        /* Use the windows-1252 that contains the Euro */
        name = "IBM-5348";
    }
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_SOLARIS
    ಆದರೆ (locale != ಶೂನ್ಯಸೂಚಕ && uprv_strcmp(name, "EUC") == 0) {
        /* Solaris underspecifies the "EUC" name. */
        ಆದರೆ (uprv_strcmp(locale, "zh_CN") == 0) {
            name = "EUC-CN";
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(locale, "zh_TW") == 0) {
            name = "EUC-TW";
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(locale, "ko_KR") == 0) {
            name = "EUC-KR";
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-954 is the best match.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(name, "646") == 0) {
        /*
         * The default codepage given by Solaris is 646 but the C library routines treat it as if it was
         * ISO-8859-1 instead of US-ASCII(646).
         */
        name = "ISO-8859-1";
    }
#ಅಥವಾಆದರೆ U_PLATFORM_IS_DARWIN_BASED
    ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ && *name == 0) {
        /*
        No locale was specified, and an empty name was passed in.
        This usually indicates that nl_langinfo didn't return valid information.
        Mac OS X uses UTF-8 by default (especially the locale data and console).
        */
        name = "UTF-8";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (locale != ಶೂನ್ಯಸೂಚಕ && uprv_strcmp(locale, "en_US_POSIX") != 0 && uprv_strcmp(name, "US-ASCII") == 0) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_BSD
    ಆದರೆ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_HPUX
    ಆದರೆ (locale != ಶೂನ್ಯಸೂಚಕ && uprv_strcmp(locale, "zh_HK") == 0 && uprv_strcmp(name, "big5") == 0) {
        /* HP decided to extend big5 as hkbig5 even though it's not compatible :-( */
        /* zh_TW.big5 is not the same charset as zh_HK.big5! */
        name = "hkbig5";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_LINUX
    ಆದರೆ (locale != ಶೂನ್ಯಸೂಚಕ && uprv_strcmp(name, "euc") == 0) {
        /* Linux underspecifies the "EUC" name. */
        ಆದರೆ (uprv_strcmp(locale, "korean") == 0) {
            name = "EUC-KR";
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(locale, "japanese") == 0) {
            /* See comment below about eucJP */
            name = "eucjis";
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(name, "eucjp") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (locale != ಶೂನ್ಯಸೂಚಕ && uprv_strcmp(locale, "en_US_POSIX") != 0 &&
            (uprv_strcmp(name, "ANSI_X3.4-1968") == 0 || uprv_strcmp(name, "US-ASCII") == 0)) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
    /*
     * Linux returns ANSI_X3.4-1968 for C/POSIX, but the call site takes care of
     * it by falling back to 'US-ASCII' when nullptr is returned from this
     * function. So, we don't have to worry about it here.
     */
#ಆದರೆ_ಅಂತ್ಯ
    /* return nullptr when "" is passed in */
    ಆದರೆ (*name == 0) {
        name = ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ name;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
getCodepageFromPOSIXID(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *localeName, ಅಕ್ಷರ * ಬಫರ್, int32_t buff_ಸಾಮರ್ಥ್ಯ)
{
    ಅಕ್ಷರ localeBuf[100];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = ಶೂನ್ಯಸೂಚಕ;
    ಅಕ್ಷರ *variant = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (localeName != ಶೂನ್ಯಸೂಚಕ && (name = (uprv_strchr(localeName, '.'))) != ಶೂನ್ಯಸೂಚಕ) {
        size_t locale_ಸಾಮರ್ಥ್ಯ = uprv_min(ಗಾತ್ರ(localeBuf), (name-localeName)+1);
        uprv_strncpy(localeBuf, localeName, locale_ಸಾಮರ್ಥ್ಯ);
        localeBuf[locale_ಸಾಮರ್ಥ್ಯ-1] = 0; /* ensure NUL termination */
        name = uprv_strncpy(ಬಫರ್, name+1, buff_ಸಾಮರ್ಥ್ಯ);
        ಬಫರ್[buff_ಸಾಮರ್ಥ್ಯ-1] = 0; /* ensure NUL termination */
        ಆದರೆ ((variant = const_cast<ಅಕ್ಷರ *>(uprv_strchr(name, '@'))) != ಶೂನ್ಯಸೂಚಕ) {
            *variant = 0;
        }
        name = remapPlatformDependentCodepage(localeBuf, name);
    }
    ಹಿಂತಿರುಗಿ name;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*
int_getDefaultCodepage()
{
#ಆದರೆ U_PLATFORM == U_PF_OS400
    uint32_t ccsid = 37; /* Default to ibm-37 */
    ಸ್ಥಿರ ಅಕ್ಷರ codepage[64];
    Qwc_JOBI0400_t jobinfo;
    Qus_EC_t error = { ಗಾತ್ರ(Qus_EC_t) }; /* SPI error code */

    EPT_CALL(QUSRJOBI)(&jobinfo, ಗಾತ್ರ(jobinfo), "JOBI0400",
        "*                         ", "                ", &error);

    ಆದರೆ (error.ಬೈಟುಗಳು_ಲಭ್ಯ == 0) {
        ಆದರೆ (jobinfo.Coded_Char_ಗಣ_ID != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Coded_Char_ಗಣ_ID;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (jobinfo.Default_Coded_Char_ಗಣ_Id != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Default_Coded_Char_ಗಣ_Id;
        }
        /* else use the default */
    }
    snprintf(codepage, ಗಾತ್ರ(codepage), "ibm-%d", ccsid);
    ಹಿಂತಿರುಗಿ codepage;

#ಅಥವಾಆದರೆ U_PLATFORM == U_PF_OS390
    ಸ್ಥಿರ ಅಕ್ಷರ codepage[64];

    strncpy(codepage, nl_langinfo(CODESET),63-ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(UCNV_ಅದಲುಬದಲು_LFNL_OPTION_STRING));
    strcat(codepage,UCNV_ಅದಲುಬದಲು_LFNL_OPTION_STRING);
    codepage[63] = 0; /* NUL terminate */

    ಹಿಂತಿರುಗಿ codepage;

#ಅಥವಾಆದರೆ U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API
    ಸ್ಥಿರ ಅಕ್ಷರ codepage[64];
    DWORD codepage_ಸಂಖ್ಯೆ = 0;

#ಆದರೆ U_PLATFORM_HAS_WINUWP_API == 1
    // UWP doesn't have a direct API to get the default ACP as Microsoft would rather
    // have folks use Unicode than a "system" code page, however this is the same
    // codepage as the system default locale codepage.  (FWIW, the system locale is
    // ONLY used for codepage, it should never be used for anything else)
    GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_ಸಂಖ್ಯೆ,
        (LPWSTR)&codepage_ಸಂಖ್ಯೆ, ಗಾತ್ರ(codepage_ಸಂಖ್ಯೆ) / ಗಾತ್ರ(WCHAR));
#ಇಲ್ಲದಿದ್ದರೆ
    // Win32 apps can call GetACP
    codepage_ಸಂಖ್ಯೆ = GetACP();
#ಆದರೆ_ಅಂತ್ಯ
    // Special case for UTF-8
    ಆದರೆ (codepage_ಸಂಖ್ಯೆ == 65001)
    {
        ಹಿಂತಿರುಗಿ "UTF-8";
    }
    // Windows codepages can look like windows-1252, so format the found number
    // the numbers are eclectic, however all valid system code pages, besides UTF-8
    // are between 3 and 19999
    ಆದರೆ (codepage_ಸಂಖ್ಯೆ > 0 && codepage_ಸಂಖ್ಯೆ < 20000)
    {
        snprintf(codepage, ಗಾತ್ರ(codepage), "windows-%ld", codepage_ಸಂಖ್ಯೆ);
        ಹಿಂತಿರುಗಿ codepage;
    }
    // If the codepage number call failed then return UTF-8
    ಹಿಂತಿರುಗಿ "UTF-8";

#ಅಥವಾಆದರೆ U_POSIX_LOCALE
    ಸ್ಥಿರ ಅಕ್ಷರ codesetName[100];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *localeName = ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = ಶೂನ್ಯಸೂಚಕ;

    localeName = uprv_getPOSIXIDForDefaultCodepage();
    uprv_memset(codesetName, 0, ಗಾತ್ರ(codesetName));
    /* On Solaris nl_langinfo returns C locale values unless setlocale
     * was called earlier.
     */
#ಆದರೆ (U_HAVE_NL_LANGINFO_CODESET && U_PLATFORM != U_PF_SOLARIS)
    /* When available, check nl_langinfo first because it usually gives more
       useful names. It depends on LC_CTYPE.
       nl_langinfo may use the same buffer as setlocale. */
    {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codeset = nl_langinfo(U_NL_LANGINFO_CODESET);
#ಆದರೆ U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED
        /*
         * On Linux and MacOSX, ensure that default codepage for non C/POSIX locale is UTF-8
         * instead of ASCII.
         */
        ಆದರೆ (uprv_strcmp(localeName, "en_US_POSIX") != 0) {
            codeset = remapPlatformDependentCodepage(localeName, codeset);
        } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
        {
            codeset = remapPlatformDependentCodepage(ಶೂನ್ಯಸೂಚಕ, codeset);
        }

        ಆದರೆ (codeset != ಶೂನ್ಯಸೂಚಕ) {
            uprv_strncpy(codesetName, codeset, ಗಾತ್ರ(codesetName));
            codesetName[ಗಾತ್ರ(codesetName)-1] = 0;
            ಹಿಂತಿರುಗಿ codesetName;
        }
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Use setlocale in a nice way, and then check some environment variables.
       Maybe the application used setlocale already.
    */
    uprv_memset(codesetName, 0, ಗಾತ್ರ(codesetName));
    name = getCodepageFromPOSIXID(localeName, codesetName, ಗಾತ್ರ(codesetName));
    ಆದರೆ (name) {
        /* if we can find the codeset name from setlocale, return that. */
        ಹಿಂತಿರುಗಿ name;
    }

    ಆದರೆ (*codesetName == 0)
    {
        /* Everything failed. Return US ASCII (ISO 646). */
        (ಶೂನ್ಯ)uprv_strcpy(codesetName, "US-ASCII");
    }
    ಹಿಂತಿರುಗಿ codesetName;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ "US-ASCII";
#ಆದರೆ_ಅಂತ್ಯ
}


U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*  U_EXPORT2
uprv_getDefaultCodepage()
{
    ಸ್ಥಿರ ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ  *name = ಶೂನ್ಯಸೂಚಕ;
    umtx_ಬೀಗ(ಶೂನ್ಯಸೂಚಕ);
    ಆದರೆ (name == ಶೂನ್ಯಸೂಚಕ) {
        name = int_getDefaultCodepage();
    }
    umtx_unlock(ಶೂನ್ಯಸೂಚಕ);
    ಹಿಂತಿರುಗಿ name;
}
#ಆದರೆ_ಅಂತ್ಯ  /* !U_CHARSET_IS_UTF8 */


/* end of platform-specific implementation -------------- */

/* version handling --------------------------------------------------------- */

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_ಆವೃತ್ತಿ_From_String(UVersionInfo ಆವೃತ್ತಿ_Array, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆವೃತ್ತಿ_String) {
    ಅಕ್ಷರ *ಕೊನೆ;
    uint16_t part=0;

    ಆದರೆ(ಆವೃತ್ತಿ_Array==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(ಆವೃತ್ತಿ_String!=ಶೂನ್ಯಸೂಚಕ) {
        ಗಾಗಿ(;;) {
            ಆವೃತ್ತಿ_Array[part]=(uint8_t)uprv_strtoul(ಆವೃತ್ತಿ_String, &ಕೊನೆ, 10);
            ಆದರೆ(ಕೊನೆ==ಆವೃತ್ತಿ_String || ++part==U_MAX_ಆವೃತ್ತಿ_ಉದ್ದಳತೆ || *ಕೊನೆ!=U_ಆವೃತ್ತಿ_DELIMITER) {
                ನಿಲ್ಲಿಸು;
            }
            ಆವೃತ್ತಿ_String=ಕೊನೆ+1;
        }
    }

    ಆಗ(part<U_MAX_ಆವೃತ್ತಿ_ಉದ್ದಳತೆ) {
        ಆವೃತ್ತಿ_Array[part++]=0;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_ಆವೃತ್ತಿ_From_UString(UVersionInfo ಆವೃತ್ತಿ_Array, ಸ್ಥಿರಾಂಕ char16_t *ಆವೃತ್ತಿ_String) {
    ಆದರೆ(ಆವೃತ್ತಿ_Array!=ಶೂನ್ಯಸೂಚಕ && ಆವೃತ್ತಿ_String!=ಶೂನ್ಯಸೂಚಕ) {
        ಅಕ್ಷರ ಆವೃತ್ತಿ_Chars[U_MAX_ಆವೃತ್ತಿ_STRING_ಉದ್ದಳತೆ+1];
        int32_t len = u_strlen(ಆವೃತ್ತಿ_String);
        ಆದರೆ(len>U_MAX_ಆವೃತ್ತಿ_STRING_ಉದ್ದಳತೆ) {
            len = U_MAX_ಆವೃತ್ತಿ_STRING_ಉದ್ದಳತೆ;
        }
        u_UChars_ಗಮ್ಯ_Chars(ಆವೃತ್ತಿ_String, ಆವೃತ್ತಿ_Chars, len);
        ಆವೃತ್ತಿ_Chars[len]=0;
        u_ಆವೃತ್ತಿ_From_String(ಆವೃತ್ತಿ_Array, ಆವೃತ್ತಿ_Chars);
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_ಆವೃತ್ತಿ_ಗಮ್ಯ_String(ಸ್ಥಿರಾಂಕ UVersionInfo ಆವೃತ್ತಿ_Array, ಅಕ್ಷರ *ಆವೃತ್ತಿ_String) {
    uint16_t count, part;
    uint8_t ಕ್ಷೇತ್ರ_2;

    ಆದರೆ(ಆವೃತ್ತಿ_String==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(ಆವೃತ್ತಿ_Array==ಶೂನ್ಯಸೂಚಕ) {
        ಆವೃತ್ತಿ_String[0]=0;
        ಹಿಂತಿರುಗಿ;
    }

    /* count how many fields need to be written */
    ಗಾಗಿ(count=4; count>0 && ಆವೃತ್ತಿ_Array[count-1]==0; --count) {
    }

    ಆದರೆ(count <= 1) {
        count = 2;
    }

    /* write the first part */
    /* write the decimal field value */
    ಕ್ಷೇತ್ರ_2=ಆವೃತ್ತಿ_Array[0];
    ಆದರೆ(ಕ್ಷೇತ್ರ_2>=100) {
        *ಆವೃತ್ತಿ_String++=(ಅಕ್ಷರ)('0'+ಕ್ಷೇತ್ರ_2/100);
        ಕ್ಷೇತ್ರ_2%=100;
    }
    ಆದರೆ(ಕ್ಷೇತ್ರ_2>=10) {
        *ಆವೃತ್ತಿ_String++=(ಅಕ್ಷರ)('0'+ಕ್ಷೇತ್ರ_2/10);
        ಕ್ಷೇತ್ರ_2%=10;
    }
    *ಆವೃತ್ತಿ_String++=(ಅಕ್ಷರ)('0'+ಕ್ಷೇತ್ರ_2);

    /* write the following parts */
    ಗಾಗಿ(part=1; part<count; ++part) {
        /* write a dot first */
        *ಆವೃತ್ತಿ_String++=U_ಆವೃತ್ತಿ_DELIMITER;

        /* write the decimal field value */
        ಕ್ಷೇತ್ರ_2=ಆವೃತ್ತಿ_Array[part];
        ಆದರೆ(ಕ್ಷೇತ್ರ_2>=100) {
            *ಆವೃತ್ತಿ_String++=(ಅಕ್ಷರ)('0'+ಕ್ಷೇತ್ರ_2/100);
            ಕ್ಷೇತ್ರ_2%=100;
        }
        ಆದರೆ(ಕ್ಷೇತ್ರ_2>=10) {
            *ಆವೃತ್ತಿ_String++=(ಅಕ್ಷರ)('0'+ಕ್ಷೇತ್ರ_2/10);
            ಕ್ಷೇತ್ರ_2%=10;
        }
        *ಆವೃತ್ತಿ_String++=(ಅಕ್ಷರ)('0'+ಕ್ಷೇತ್ರ_2);
    }

    /* NUL-terminate */
    *ಆವೃತ್ತಿ_String=0;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_get_ಆವೃತ್ತಿ(UVersionInfo ಆವೃತ್ತಿ_Array) {
    (ಶೂನ್ಯ)copyright;   // Suppress unused variable warning from clang.
    u_ಆವೃತ್ತಿ_From_String(ಆವೃತ್ತಿ_Array, U_ICU_ಆವೃತ್ತಿ);
}

/**
 * icucfg.h dependent code
 */

#ಆದರೆ U_ENABLE_DYLOAD && HAVE_DLOPEN && !U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API

#ಆದರೆ HAVE_DLFCN_H
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __MVS__
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __SUSV3
#ವ್ಯಾಖ್ಯಾನಿಸು __SUSV3 1
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <dlfcn.h>
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_DLFCN_H */

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_dl_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libName, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಶೂನ್ಯ *ret = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ret;
  ret =  dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
  ಆದರೆ(ret==ಶೂನ್ಯಸೂಚಕ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_TRACE_DYLOAD
    ಬರೆ("dlerror on dlopen(%s): %s\n", libName, dlerror());
#ಆದರೆ_ಅಂತ್ಯ
    *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
  }
  ಹಿಂತಿರುಗಿ ret;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_dl_close(ಶೂನ್ಯ *lib, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
  dlclose(lib);
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(ಶೂನ್ಯ *lib, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sym, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  ಒಕ್ಕೂಟ {
      UVoidFunction *fp;
      ಶೂನ್ಯ *vp;
  } uret;
  uret.fp = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ uret.fp;
  uret.vp = dlsym(lib, sym);
  ಆದರೆ(uret.vp == ಶೂನ್ಯಸೂಚಕ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_TRACE_DYLOAD
    ಬರೆ("dlerror on dlsym(%p,%s): %s\n", lib,sym, dlerror());
#ಆದರೆ_ಅಂತ್ಯ
    *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
  }
  ಹಿಂತಿರುಗಿ uret.fp;
}

#ಅಥವಾಆದರೆ U_ENABLE_DYLOAD && U_PLATFORM_ಬಳಸುತ್ತದೆ_ONLY_WIN32_API && !U_PLATFORM_HAS_WINUWP_API

/* Windows API implementation. */
// Note: UWP does not expose/allow these APIs, so the UWP version gets the null implementation. */

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_dl_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libName, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  HMODULE lib = ಶೂನ್ಯಸೂಚಕ;

  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

  lib = ತುಂಬು_Library_A(libName);

  ಆದರೆ(lib==ಶೂನ್ಯಸೂಚಕ) {
    *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
  }

  ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ*)lib;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_dl_close(ಶೂನ್ಯ *lib, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  HMODULE handle = (HMODULE)lib;
  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;

  FreeLibrary(handle);

  ಹಿಂತಿರುಗಿ;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(ಶೂನ್ಯ *lib, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sym, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  HMODULE handle = (HMODULE)lib;
  UVoidFunction* addr = ಶೂನ್ಯಸೂಚಕ;

  ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2) || lib==ಶೂನ್ಯಸೂಚಕ) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

  addr = (UVoidFunction*)Get_Proc_ವಿಳಾಸ(handle, sym);

  ಆದರೆ(addr==ಶೂನ್ಯಸೂಚಕ) {
    DWORD ಕೊನೆಯ_Error = Get_ಕೊನೆಯ_Error();
    ಆದರೆ(ಕೊನೆಯ_Error == ERROR_PROC_NOT_ಸಿಕ್ಕಿತು) {
      *ಸ್ಥಿತಿ_2 = U_MISSING_RESOURCE_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
      *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR; /* other unknown error. */
    }
  }

  ಹಿಂತಿರುಗಿ addr;
}

#ಇಲ್ಲದಿದ್ದರೆ

/* No dynamic loading, null (nonexistent) implementation. */

U_CAPI ಶೂನ್ಯ * U_EXPORT2
uprv_dl_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libName, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    (ಶೂನ್ಯ)libName;
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_dl_close(ಶೂನ್ಯ *lib, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    (ಶೂನ್ಯ)lib;
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ;
    *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
    ಹಿಂತಿರುಗಿ;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(ಶೂನ್ಯ *lib, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* sym, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
  (ಶೂನ್ಯ)lib;
  (ಶೂನ್ಯ)sym;
  ಆದರೆ(U_SUCCESS(*ಸ್ಥಿತಿ_2)) {
    *ಸ್ಥಿತಿ_2 = U_UNSUPPORTED_ERROR;
  }
  ಹಿಂತಿರುಗಿ (UVoidFunction*)ಶೂನ್ಯಸೂಚಕ;
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

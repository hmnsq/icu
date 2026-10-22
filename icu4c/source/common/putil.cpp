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
#ਅਨ੍ਤਰ੍ਭਾਵ "uposixdefs.h"

// First, the platform type. Need this for U_PLATFORM.
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/platform.h"

/*
 * Cygwin with GCC requires inclusion of time.h after the above disabling strict asci mode statement.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>

#ਜੇ !U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/time.h>
#ਜੇ_ਅੰਤ

/* include the rest of the ICU headers */
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "putilimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"

/* Include standard headers. */
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <locale.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_COMMON_IMPLEMENTATION
#ਗਲਤੀ U_COMMON_IMPLEMENTATION not ਸੈੱਟ - must be ਸੈੱਟ ਲਈ all ICU ਸਰੋਤ files in common/ - see https://unicode-org.github.io/icu/userguide/icu/howtouseicu.html
#ਜੇ_ਅੰਤ


/* include system headers */
#ਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
    /*
     * TODO: U_PLATFORM_USES_ONLY_WIN32_API includes MinGW.
     * Should Cygwin be included as well (U_PLATFORM_HAS_WIN32_API)
     * to use native APIs as much as possible?
     */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WIN32_LEAN_AND_ਔਸਤ
#   define WIN32_LEAN_AND_ਔਸਤ
#ਜੇ_ਅੰਤ
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>
#   include "unicode/uloc.h"
#   include "wintz.h"
#ਜਾਂਜੇ U_PLATFORM == U_PF_OS400
#   include <float.h>
#   include <qusec.h>       /* error code structure */
#   include <qusrjobi.h>
#   include <qliept.h>      /* EPT_CALL macro  - this include must be after all other "QSYSINCs" */
#   include <mih/testptr.h> /* For uprv_maximumPtr */
#ਜਾਂਜੇ U_PLATFORM == U_PF_OS390
#   include "unicode/ucnv.h"   /* Needed for UCNV_SWAP_LFNL_OPTION_STRING */
#ਜਾਂਜੇ U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS
#   include <limits.h>
#   include <unistd.h>
#   ਜੇ U_PLATFORM == U_PF_SOLARIS
#       ifndef _XPG4_2
#           define _XPG4_2
#       endif
#   elif U_PLATFORM == U_PF_ANDROID
#       include <sys/system_properties.h>
#       include <dlfcn.h>
#   endif
#ਜਾਂਜੇ U_PLATFORM == U_PF_QNX
#   include <sys/neutrino.h>
#ਜੇ_ਅੰਤ


/*
 * Only include langinfo.h if we have a way to get the codeset. If we later
 * depend on more feature, we can test on U_HAVE_NL_LANGINFO.
 *
 */

#ਜੇ U_HAVE_NL_LANGINFO_CODESET
#ਅਨ੍ਤਰ੍ਭਾਵ <langinfo.h>
#ਜੇ_ਅੰਤ

/**
 * Simple things (presence of functions, etc) should just go in configure.in and be added to
 * icucfg.h via autoheader.
 */
#ਜੇ U_PLATFORM_IMPLEMENTS_POSIX
#   ਜੇ U_PLATFORM == U_PF_OS400
#    define HAVE_DLFCN_H 0
#    define HAVE_DLOPEN 0
#   ਨਹੀਂਤਾਂ
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
#ਨਹੀਂਤਾਂ
#   define HAVE_DLFCN_H 0
#   define HAVE_DLOPEN 0
#   define HAVE_GETTIMEOFDAY 0
#ਜੇ_ਅੰਤ

U_NAMESPACE_USE

/* Define the extension for data files, again... */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅੰਕੜੇ_TYPE "dat"

/* Leave this copyright notice here! */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ copyright[] = U_COPYRIGHT_STRING;

/* floating point implementations ------------------------------------------- */

/* We return QNAN rather than SNAN*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIGN 0x80000000U

/* Make it easy to define certain types of constants */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ {
    int64_t i64; /* This must be defined first in order to allow the initialization to work. This is a C89 feature. */
    ਦੁੱਗਣਾ d64;
} ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਮੂਨਾ_Conversion;
ਸਥਿਰ ਅਚਲ ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਮੂਨਾ_Conversion gNan = {static_cast<int64_t>(INT64_C(0x7FF8000000000000))};
ਸਥਿਰ ਅਚਲ ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਮੂਨਾ_Conversion gInf = {static_cast<int64_t>(INT64_C(0x7FF0000000000000))};

/*---------------------------------------------------------------------------
  Platform utilities
  Our general strategy is to assume we're on a POSIX platform.  Platforms which
  are non-POSIX must declare themselves so.  The default POSIX implementation
  will sometimes work for non-POSIX platforms as well (e.g., the NaN-related
  functions).
  ---------------------------------------------------------------------------*/

#ਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API || U_PLATFORM == U_PF_OS400
#   undef U_POSIX_LOCALE
#ਨਹੀਂਤਾਂ
#   define U_POSIX_LOCALE    1
#ਜੇ_ਅੰਤ

/*
    WARNING! u_topNBytesOfDouble and u_bottomNBytesOfDouble
    can't be properly optimized by the gcc compiler sometimes (i.e. gcc 3.2).
*/
#ਜੇ !IEEE_754
ਸਥਿਰ ਅੱਖਰ*
u_ਸਿਖਰ_NBytes_ਦਾ_Double(ਦੁੱਗਣਾ* d, ਪੂਰਨ n)
{
#ਜੇ U_IS_BIG_ENDIAN
    ਵਾਪਸ (ਅੱਖਰ*)d;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ (ਅੱਖਰ*)(d + 1) - n;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਅੱਖਰ*
u_bottom_NBytes_ਦਾ_Double(ਦੁੱਗਣਾ* d, ਪੂਰਨ n)
{
#ਜੇ U_IS_BIG_ENDIAN
    ਵਾਪਸ (ਅੱਖਰ*)(d + 1) - n;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ (ਅੱਖਰ*)d;
#ਜੇ_ਅੰਤ
}
#ਜੇ_ਅੰਤ   /* !IEEE_754 */

#ਜੇ IEEE_754
ਸਥਿਰ UBool
u_sign_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੁੱਗਣਾ d) {
    uint8_t hiByte;
#ਜੇ U_IS_BIG_ENDIAN
    hiByte = *(uint8_t *)&d;
#ਨਹੀਂਤਾਂ
    hiByte = *(reinterpret_cast<uint8_t*>(&d) + ਆਕਾਰ(ਦੁੱਗਣਾ) - 1);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ (hiByte & 0x80) != 0;
}
#ਜੇ_ਅੰਤ



#ਜੇ defined (U_DEBUG_FAKETIME)
/* Override the clock to test things without having to move the system clock.
 * Assumes POSIX gettimeofday() will function
 */
UDate fakeClock_t0 = 0; /** Time to start the clock from **/
UDate fakeClock_dt = 0; /** Offset (fake time - real time) **/
UBool fake_Clock_ਸੈੱਟ = ਝੂਠ; /** True if fake clock has spun up **/

ਸਥਿਰ UDate getUTCtime_real() {
    ਬਣਤਰ timeval posix_ਸਮਾਂ;
    gettimeofday(&posix_ਸਮਾਂ, ਖਾਲੀ_ਸੂਚਕ);
    ਵਾਪਸ (UDate)(((int64_t)posix_ਸਮਾਂ.tv_sec * U_MILLIS_ਪ੍ਰਤੀ_ਦੂਜਾ) + (posix_ਸਮਾਂ.tv_usec/1000));
}

ਸਥਿਰ UDate getUTCtime_fake() {
    ਸਥਿਰ UMutex fakeClockMutex;
    umtx_ਤਾਲਾ(&fakeClockMutex);
    ਜੇ(!fake_Clock_ਸੈੱਟ) {
        UDate real = getUTCtime_real();
        ਅਚਲ ਅੱਖਰ *fake_ਸ਼ੁਰੂ = getenv("U_FAKETIME_START");
        ਜੇ((fake_ਸ਼ੁਰੂ!=ਖਾਲੀ_ਸੂਚਕ) && (fake_ਸ਼ੁਰੂ[0]!=0)) {
            ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(fake_ਸ਼ੁਰੂ,"%lf",&fakeClock_t0);
            fakeClock_dt = fakeClock_t0 - real;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, so the ICU clock will start at a preset value\n"
                    "env variable U_FAKETIME_START=%.0f (%s) for an offset of %.0f ms from the current time %.0f\n",
                    fakeClock_t0, fake_ਸ਼ੁਰੂ, fakeClock_dt, real);
        } ਨਹੀਂਤਾਂ {
          fakeClock_dt = 0;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, but U_FAKETIME_START was not set.\n"
                    "Set U_FAKETIME_START to the number of milliseconds since 1/1/1970 to set the ICU clock.\n");
        }
        fake_Clock_ਸੈੱਟ = ਸੱਚ;
    }
    umtx_unlock(&fakeClockMutex);

    ਵਾਪਸ getUTCtime_real() + fakeClock_dt;
}
#ਜੇ_ਅੰਤ

#ਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ {
    int64_t int64;
    FILETIME file_ਸਮਾਂ;
} File_ਸਮਾਂ_Conversion;   /* This is like a ULARGE_INTEGER */

/* Number of 100 nanoseconds from 1/1/1601 to 1/1/1970 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EPOCH_BIAS  INT64_C(116444736000000000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HECTONANOSECOND_ਪ੍ਰਤੀ_MILLISECOND   10000

#ਜੇ_ਅੰਤ

/*---------------------------------------------------------------------------
  Universal Implementations
  These are designed to work on all platforms.  Try these, and if they
  don't work on your platform, then special case your platform with new
  implementations.
---------------------------------------------------------------------------*/

U_CAPI UDate U_EXPORT2
uprv_getUTCtime()
{
#ਜੇ defined(U_DEBUG_FAKETIME)
    ਵਾਪਸ getUTCtime_fake(); /* Hook for overriding the clock */
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ uprv_getRawUTCtime();
#ਜੇ_ਅੰਤ
}

/* Return UTC (GMT) time measured in milliseconds since 0:00 on 1/1/70.*/
U_CAPI UDate U_EXPORT2
uprv_getRawUTCtime()
{
#ਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API

    File_ਸਮਾਂ_Conversion win_ਸਮਾਂ;
    Get_System_ਸਮਾਂ_As_File_ਸਮਾਂ(&win_ਸਮਾਂ.file_ਸਮਾਂ);
    ਵਾਪਸ (UDate)((win_ਸਮਾਂ.int64 - EPOCH_BIAS) / HECTONANOSECOND_ਪ੍ਰਤੀ_MILLISECOND);
#ਨਹੀਂਤਾਂ

#ਜੇ HAVE_GETTIMEOFDAY
    ਬਣਤਰ timeval posix_ਸਮਾਂ;
    gettimeofday(&posix_ਸਮਾਂ, ਖਾਲੀ_ਸੂਚਕ);
    ਵਾਪਸ (UDate)(((int64_t)posix_ਸਮਾਂ.tv_sec * U_MILLIS_ਪ੍ਰਤੀ_ਦੂਜਾ) + (posix_ਸਮਾਂ.tv_usec/1000));
#ਨਹੀਂਤਾਂ
    time_t epochtime;
    time(&epochtime);
    ਵਾਪਸ (UDate)epochtime * U_MILLIS_ਪ੍ਰਤੀ_ਦੂਜਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
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
uprv_isNaN(ਦੁੱਗਣਾ number)
{
#ਜੇ IEEE_754
    ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਮੂਨਾ_Conversion converted_ਸੰਖਿਆ;
    converted_ਸੰਖਿਆ.d64 = number;
    /* Infinity is 0x7FF0000000000000U. Anything greater than that is a NaN */
    ਵਾਪਸ (converted_ਸੰਖਿਆ.i64 & U_INT64_MAX) > gInf.i64;

#ਜਾਂਜੇ U_PLATFORM == U_PF_OS390
    uint32_t ਉੱਚਾ_ਬਿੱਟ = *(uint32_t*)u_ਸਿਖਰ_NBytes_ਦਾ_Double(&number,
                        ਆਕਾਰ(uint32_t));
    uint32_t ਨੀਵਾਂ_ਬਿੱਟ  = *(uint32_t*)u_bottom_NBytes_ਦਾ_Double(&number,
                        ਆਕਾਰ(uint32_t));

    ਵਾਪਸ ((ਉੱਚਾ_ਬਿੱਟ & 0x7F080000L) == 0x7F080000L) &&
      (ਨੀਵਾਂ_ਬਿੱਟ == 0x00000000L);

#ਨਹੀਂਤਾਂ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    ਵਾਪਸ number != number;
#ਜੇ_ਅੰਤ
}

U_CAPI UBool U_EXPORT2
uprv_isInfinite(ਦੁੱਗਣਾ number)
{
#ਜੇ IEEE_754
    ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਮੂਨਾ_Conversion converted_ਸੰਖਿਆ;
    converted_ਸੰਖਿਆ.d64 = number;
    /* Infinity is exactly 0x7FF0000000000000U. */
    ਵਾਪਸ (converted_ਸੰਖਿਆ.i64 & U_INT64_MAX) == gInf.i64;
#ਜਾਂਜੇ U_PLATFORM == U_PF_OS390
    uint32_t ਉੱਚਾ_ਬਿੱਟ = *(uint32_t*)u_ਸਿਖਰ_NBytes_ਦਾ_Double(&number,
                        ਆਕਾਰ(uint32_t));
    uint32_t ਨੀਵਾਂ_ਬਿੱਟ  = *(uint32_t*)u_bottom_NBytes_ਦਾ_Double(&number,
                        ਆਕਾਰ(uint32_t));

    ਵਾਪਸ ((ਉੱਚਾ_ਬਿੱਟ  & ~SIGN) == 0x70FF0000L) && (ਨੀਵਾਂ_ਬਿੱਟ == 0x00000000L);

#ਨਹੀਂਤਾਂ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    ਵਾਪਸ number == (2.0 * number);
#ਜੇ_ਅੰਤ
}

U_CAPI UBool U_EXPORT2
uprv_is_ਸਕਾਰਾਤਮਕ_Infinity(ਦੁੱਗਣਾ number)
{
#ਜੇ IEEE_754 || U_PLATFORM == U_PF_OS390
    ਵਾਪਸ number > 0 && uprv_isInfinite(number);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ uprv_isInfinite(number);
#ਜੇ_ਅੰਤ
}

U_CAPI UBool U_EXPORT2
uprv_isNegativeInfinity(ਦੁੱਗਣਾ number)
{
#ਜੇ IEEE_754 || U_PLATFORM == U_PF_OS390
    ਵਾਪਸ number < 0 && uprv_isInfinite(number);

#ਨਹੀਂਤਾਂ
    uint32_t ਉੱਚਾ_ਬਿੱਟ = *(uint32_t*)u_ਸਿਖਰ_NBytes_ਦਾ_Double(&number,
                        ਆਕਾਰ(uint32_t));
    ਵਾਪਸ((ਉੱਚਾ_ਬਿੱਟ & SIGN) && uprv_isInfinite(number));

#ਜੇ_ਅੰਤ
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_getNaN()
{
#ਜੇ IEEE_754 || U_PLATFORM == U_PF_OS390
    ਵਾਪਸ gNan.d64;
#ਨਹੀਂਤਾਂ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    ਵਾਪਸ 0.0;
#ਜੇ_ਅੰਤ
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_getInfinity()
{
#ਜੇ IEEE_754 || U_PLATFORM == U_PF_OS390
    ਵਾਪਸ gInf.d64;
#ਨਹੀਂਤਾਂ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    ਵਾਪਸ 0.0;
#ਜੇ_ਅੰਤ
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_floor(ਦੁੱਗਣਾ x)
{
    ਵਾਪਸ floor(x);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_ceil(ਦੁੱਗਣਾ x)
{
    ਵਾਪਸ ceil(x);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_ਗੇੜਾ(ਦੁੱਗਣਾ x)
{
    ਵਾਪਸ uprv_floor(x + 0.5);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_fabs(ਦੁੱਗਣਾ x)
{
    ਵਾਪਸ fabs(x);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_modf(ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ* y)
{
    ਵਾਪਸ modf(x, y);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_fmod(ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y)
{
    ਵਾਪਸ fmod(x, y);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_pow(ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y)
{
    /* This is declared as "double pow(double x, double y)" */
    ਵਾਪਸ ਘਾਤ(x, y);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_pow10(int32_t x)
{
    ਵਾਪਸ ਘਾਤ(10.0, (ਦੁੱਗਣਾ)x);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_fmax(ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y)
{
#ਜੇ IEEE_754
    /* first handle NaN*/
    ਜੇ(uprv_isNaN(x) || uprv_isNaN(y))
        ਵਾਪਸ uprv_getNaN();

    /* check for -0 and 0*/
    ਜੇ(x == 0.0 && y == 0.0 && u_sign_ਦੋਆਧਾਰੀ_ਅੰਕ(x))
        ਵਾਪਸ y;

#ਜੇ_ਅੰਤ

    /* this should work for all flt point w/o NaN and Inf special cases */
    ਵਾਪਸ (x > y ? x : y);
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_fmin(ਦੁੱਗਣਾ x, ਦੁੱਗਣਾ y)
{
#ਜੇ IEEE_754
    /* first handle NaN*/
    ਜੇ(uprv_isNaN(x) || uprv_isNaN(y))
        ਵਾਪਸ uprv_getNaN();

    /* check for -0 and 0*/
    ਜੇ(x == 0.0 && y == 0.0 && u_sign_ਦੋਆਧਾਰੀ_ਅੰਕ(y))
        ਵਾਪਸ y;

#ਜੇ_ਅੰਤ

    /* this should work for all flt point w/o NaN and Inf special cases */
    ਵਾਪਸ (x > y ? y : x);
}

U_CAPI UBool U_EXPORT2
uprv_add32_ਉਛਾਲ(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_add_overflow.
    // This function could be optimized by calling one of those primitives.
    ਸਵੈਚਾਲਿਤ a64 = static_cast<int64_t>(a);
    ਸਵੈਚਾਲਿਤ b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 + b64;
    *res = static_cast<int32_t>(res64);
    ਵਾਪਸ res64 != *res;
}

U_CAPI UBool U_EXPORT2
uprv_mul32_ਉਛਾਲ(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_mul_overflow.
    // This function could be optimized by calling one of those primitives.
    ਸਵੈਚਾਲਿਤ a64 = static_cast<int64_t>(a);
    ਸਵੈਚਾਲਿਤ b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 * b64;
    *res = static_cast<int32_t>(res64);
    ਵਾਪਸ res64 != *res;
}

/**
 * Truncates the given double.
 * trunc(3.3) = 3.0, trunc (-3.3) = -3.0
 * This is different than calling floor() or ceil():
 * floor(3.3) = 3, floor(-3.3) = -4
 * ceil(3.3) = 4, ceil(-3.3) = -3
 */
U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_trunc(ਦੁੱਗਣਾ d)
{
#ਜੇ IEEE_754
    /* handle error cases*/
    ਜੇ(uprv_isNaN(d))
        ਵਾਪਸ uprv_getNaN();
    ਜੇ(uprv_isInfinite(d))
        ਵਾਪਸ uprv_getInfinity();

    ਜੇ(u_sign_ਦੋਆਧਾਰੀ_ਅੰਕ(d))    /* Signbit() picks up -0.0;  d<0 does not. */
        ਵਾਪਸ ceil(d);
    ਨਹੀਂਤਾਂ
        ਵਾਪਸ floor(d);

#ਨਹੀਂਤਾਂ
    ਵਾਪਸ d >= 0 ? floor(d) : ceil(d);

#ਜੇ_ਅੰਤ
}

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 */
U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_maxMantissa()
{
    ਵਾਪਸ ਘਾਤ(2.0, DBL_MANT_DIG + 1.0) - 1.0;
}

U_CAPI ਦੁੱਗਣਾ U_EXPORT2
uprv_log(ਦੁੱਗਣਾ d)
{
    ਵਾਪਸ ਲਘੂਗਣਕ(d);
}

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_ਵੱਧ_ਤੋਂ_ਵੱਧ_Ptr(ਖਾਲੀ * ਅਧਾਰ)
{
#ਜੇ U_PLATFORM == U_PF_OS400
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
    ਜੇ ((ਅਧਾਰ != ਖਾਲੀ_ਸੂਚਕ) && (_TESTPTR(ਅਧਾਰ, _C_TERASPACE_CHECK))) {
        /* if it is a TERASPACE pointer the max is 2GB - 4k */
        ਵਾਪਸ ((ਖਾਲੀ *)(((ਅੱਖਰ *)ਅਧਾਰ)-((uint32_t)(ਅਧਾਰ))+((uint32_t)0x7fffefff)));
    }
    /* otherwise 16MB since nullptr ptr is not checkable or the ptr is not TERASPACE */
    ਵਾਪਸ ((ਖਾਲੀ *)(((ਅੱਖਰ *)ਅਧਾਰ)-((uint32_t)(ਅਧਾਰ))+((uint32_t)0xffefff)));

#ਨਹੀਂਤਾਂ
    ਵਾਪਸ U_MAX_PTR(ਅਧਾਰ);
#ਜੇ_ਅੰਤ
}

/*---------------------------------------------------------------------------
  Platform-specific Implementations
  Try these, and if they don't work on your platform, then special case your
  platform with new implementations.
  ---------------------------------------------------------------------------*/

/* Generic time zone layer -------------------------------------------------- */

/* Time zone utilities */
U_CAPI ਖਾਲੀ U_EXPORT2
uprv_tzset()
{
#ਜੇ defined(U_TZSET)
    U_TZSET();
#ਨਹੀਂਤਾਂ
    /* no initialization*/
#ਜੇ_ਅੰਤ
}

U_CAPI int32_t U_EXPORT2
uprv_timezone()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_TIMEZONE
    ਵਾਪਸ U_TIMEZONE;
#ਨਹੀਂਤਾਂ
    time_t t, t1, t2;
    ਬਣਤਰ tm tmrec;
    int32_t tdiff = 0;

    time(&t);
    uprv_memcpy( &tmrec, localtime(&t), ਆਕਾਰ(tmrec) );
#ਜੇ U_PLATFORM != U_PF_IPHONE
    UBool dst_checked = (tmrec.tm_isdst != 0); /* daylight savings time is checked*/
#ਜੇ_ਅੰਤ
    t1 = mktime(&tmrec);                 /* local time in seconds*/
    uprv_memcpy( &tmrec, gmtime(&t), ਆਕਾਰ(tmrec) );
    t2 = mktime(&tmrec);                 /* GMT (or UTC) in seconds*/
    tdiff = t2 - t1;

#ਜੇ U_PLATFORM != U_PF_IPHONE
    /* imitate NT behaviour, which returns same timezone offset to GMT for
       winter and summer.
       This does not work on all platforms. For instance, on glibc on Linux
       and on Mac OS 10.5, tdiff calculated above remains the same
       regardless of whether DST is in effect or not. iOS is another
       platform where this does not work. Linux + glibc and Mac OS 10.5
       have U_TIMEZONE defined so that this code is not reached.
    */
    ਜੇ (dst_checked)
        tdiff += 3600;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ tdiff;
#ਜੇ_ਅੰਤ
}

/* Note that U_TZNAME does *not* have to be tzname, but if it is,
   some platforms need to have it declared here. */

#ਜੇ defined(U_TZNAME) && (U_PLATFORM == U_PF_IRIX || U_PLATFORM_IS_DARWIN_BASED)
/* RS6000 and others reject char **tzname.  */
ਬਾਹਰੀ U_IMPORT ਅੱਖਰ *U_TZNAME[];
#ਜੇ_ਅੰਤ

#ਜੇ !UCONFIG_NO_FILE_IO && ((U_PLATFORM_IS_DARWIN_BASED && (U_PLATFORM != U_PF_IPHONE || defined(U_TIMEZONE))) || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS)
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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHECK_LOCALTIME_LINK 1
#ਜੇ U_PLATFORM_IS_DARWIN_BASED
#ਅਨ੍ਤਰ੍ਭਾਵ <tzfile.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TZZONEINFO      (TZDIR "/")
#ਜਾਂਜੇ U_PLATFORM == U_PF_SOLARIS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TZDEFAULT       "/etc/localtime"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TZZONEINFO      "/usr/share/lib/zoneinfo/"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TZ_ENV_CHECK    "localtime"
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TZDEFAULT       "/etc/localtime"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TZZONEINFO      "/usr/share/zoneinfo/"
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TZZONEINFOTAIL  "/zoneinfo/"
#ਜੇ U_HAVE_DIRENT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TZFILE_SKIP     "posixrules" /* tz file to skip when searching. */
/* Some Linux distributions have 'localtime' in /usr/share/zoneinfo
   symlinked to /etc/localtime, which makes searchForTZFile return
   'localtime' when it's the first match. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TZFILE_SKIP2    "localtime"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੋਜ_TZFILE
#ਅਨ੍ਤਰ੍ਭਾਵ <dirent.h>  /* Needed to search through system timezone files */
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਅੱਖਰ g_ਸਮਾਂ_Zone_ਬਫਰ[ਰਸਤਾ_MAX];
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *g_ਸਮਾਂ_Zone_ਬਫਰ_Ptr = ਖਾਲੀ_ਸੂਚਕ;
#ਜੇ_ਅੰਤ

#ਜੇ !U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isNonDigit(ch) (ch < '0' || '9' < ch)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isDigit(ch) ('0' <= ch && ch <= '9')
ਸਥਿਰ UBool isValidOlsonID(ਅਚਲ ਅੱਖਰ *id) {
    int32_t idx = 0;
    int32_t idxMax = 0;

    /* Determine if this is something like Iceland (Olson ID)
    or AST4ADT (non-Olson ID) */
    ਜਦੋਂ (id[idx] && isNonDigit(id[idx]) && id[idx] != ',') {
        idx++;
    }

    /* Allow at maximum 2 numbers at the end of the id to support zone id's
    like GMT+11. */
    idxMax = idx + 2;
    ਜਦੋਂ (id[idx] && isDigit(id[idx]) && idx < idxMax) {
        idx++;
    }

    /* If we went through the whole string, then it might be okay.
    The timezone is sometimes set to "CST-7CDT", "CST6CDT5,J129,J131/19:30",
    "GRNLNDST3GRNLNDDT" or similar, so we cannot use it.
    The rest of the time it could be an Olson ID. George */
    ਵਾਪਸ id[idx] == 0
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
ਸਥਿਰ ਖਾਲੀ skipZoneIDPrefix(ਅਚਲ ਅੱਖਰ** id) {
    ਜੇ (uprv_strncmp(*id, "posix/", 6) == 0
        || uprv_strncmp(*id, "right/", 6) == 0)
    {
        *id += 6;
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(U_TZNAME) && !U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONVERT_HOURS_ਮੰਜ਼ਿਲ_SECONDS(ਵਿੱਥ) (int32_t)(ਵਿੱਥ*3600)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਵਿੱਥ_Zone_Mapping {
    int32_t ਵਿੱਥ_Seconds;
    int32_t daylightType; /* 0=U_DAYLIGHT_NONE, 1=daylight in June-U_DAYLIGHT_JUNE, 2=daylight in December=U_DAYLIGHT_DECEMBER*/
    ਅਚਲ ਅੱਖਰ *stdID;
    ਅਚਲ ਅੱਖਰ *dstID;
    ਅਚਲ ਅੱਖਰ *olsonID;
} ਵਿੱਥ_Zone_Mapping;

ਗਿਣਤੀ { U_DAYLIGHT_NONE=0,U_DAYLIGHT_JUNE=1,U_DAYLIGHT_DECEMBER=2 };

/*
This list tries to disambiguate a set of abbreviated timezone IDs and offsets
and maps it to an Olson ID.
Before adding anything to this list, take a look at
icu/source/tools/tzcode/tz.alias
Sometimes no daylight savings (0) is important to define due to aliases.
This list can be tested with icu/source/test/compat/tzone.pl
More values could be added to daylightType to increase precision.
*/
ਸਥਿਰ ਅਚਲ ਬਣਤਰ ਵਿੱਥ_Zone_Mapping ਵਿੱਥ_ZONE_MAPPINGS[] = {
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

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* remap_Short_ਸਮਾਂ_Zone(ਅਚਲ ਅੱਖਰ *stdID, ਅਚਲ ਅੱਖਰ *dstID, int32_t daylightType, int32_t ਵਿੱਥ)
{
    int32_t idx;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_TZNAME
    fprintf(stderr, "TZ=%s std=%s dst=%s daylight=%d offset=%d\n", getenv("TZ"), stdID, dstID, daylightType, ਵਿੱਥ);
#ਜੇ_ਅੰਤ
    ਲਈ (idx = 0; idx < UPRV_LENGTHOF(ਵਿੱਥ_ZONE_MAPPINGS); idx++)
    {
        ਜੇ (ਵਿੱਥ == ਵਿੱਥ_ZONE_MAPPINGS[idx].ਵਿੱਥ_Seconds
            && daylightType == ਵਿੱਥ_ZONE_MAPPINGS[idx].daylightType
            && strcmp(ਵਿੱਥ_ZONE_MAPPINGS[idx].stdID, stdID) == 0
            && strcmp(ਵਿੱਥ_ZONE_MAPPINGS[idx].dstID, dstID) == 0)
        {
            ਵਾਪਸ ਵਿੱਥ_ZONE_MAPPINGS[idx].olsonID;
        }
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਖੋਜ_TZFILE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_READ_ਮਾਪ 512

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ DefaultTZInfo {
    ਅੱਖਰ* defaultTZBuffer;
    int64_t default_TZFile_ਮਾਪ;
    FILE* defaultTZFilePtr;
    int32_t defaultTZPosition;
} DefaultTZInfo;

/*
 * This method compares the two files given to see if they are a match.
 * It is currently use to compare two TZ files.
 */
ਸਥਿਰ UBool compare_ਦੁਆਧਾਰੀ_Files(ਅਚਲ ਅੱਖਰ* TZFileName, DefaultTZInfo* tzInfo) {
    FILE* file;
    int64_t ਮਾਪ_File;
    int64_t ਮਾਪ_File_ਖੱਬਾ;
    int32_t ਮਾਪ_File_Read;
    int32_t ਮਾਪ_File_ਮੰਜ਼ਿਲ_Read;
    ਅੱਖਰ ਬਫਰ_File[MAX_READ_ਮਾਪ];
    UBool ਨਤੀਜਾ = ਸੱਚ;

    U_ASSERT(tzInfo->defaultTZFilePtr != ਖਾਲੀ_ਸੂਚਕ);
    file = fopen(TZFileName, "r");

    tzInfo->defaultTZPosition = 0; /* reset position to begin search */

    ਜੇ (file != ਖਾਲੀ_ਸੂਚਕ) {
        /* First check that the file size are equal. */
        ਜੇ (tzInfo->default_TZFile_ਮਾਪ == 0) {
            fseek(tzInfo->defaultTZFilePtr, 0, SEEK_END);
            tzInfo->default_TZFile_ਮਾਪ = ftell(tzInfo->defaultTZFilePtr);
        }
        ਜੇ (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            ਵਾਪਸ ਝੂਠ;
        }
        ਮਾਪ_File = ftell(file);
        ਜੇ (ਮਾਪ_File == -1) {
            fclose(file);
            ਵਾਪਸ ਝੂਠ;
        }
        ਮਾਪ_File_ਖੱਬਾ = ਮਾਪ_File;

        ਜੇ (ਮਾਪ_File != tzInfo->default_TZFile_ਮਾਪ) {
            ਨਤੀਜਾ = ਝੂਠ;
        } ਨਹੀਂਤਾਂ {
            /* Store the data from the files in separate buffers and
             * compare each byte to determine equality.
             */
            ਜੇ (tzInfo->defaultTZBuffer == ਖਾਲੀ_ਸੂਚਕ) {
                ਜੇ (fseek(tzInfo->defaultTZFilePtr, 0, SEEK_SET) != 0) {
                    fclose(file);
                    ਵਾਪਸ ਝੂਠ;
                }
                tzInfo->defaultTZBuffer = static_cast<ਅੱਖਰ*>(uprv_malloc(ਆਕਾਰ(ਅੱਖਰ) * tzInfo->default_TZFile_ਮਾਪ));
                ਮਾਪ_File_Read = fread(tzInfo->defaultTZBuffer, 1, tzInfo->default_TZFile_ਮਾਪ, tzInfo->defaultTZFilePtr);
            }
            ਜੇ (fseek(file, 0, SEEK_SET) != 0) {
                fclose(file);
                ਵਾਪਸ ਝੂਠ;
            }
            ਜਦੋਂ(ਮਾਪ_File_ਖੱਬਾ > 0) {
                uprv_memset(ਬਫਰ_File, 0, MAX_READ_ਮਾਪ);
                ਮਾਪ_File_ਮੰਜ਼ਿਲ_Read = ਮਾਪ_File_ਖੱਬਾ < MAX_READ_ਮਾਪ ? ਮਾਪ_File_ਖੱਬਾ : MAX_READ_ਮਾਪ;

                ਮਾਪ_File_Read = fread(ਬਫਰ_File, 1, ਮਾਪ_File_ਮੰਜ਼ਿਲ_Read, file);
                ਜੇ (ਮਾਪ_File_Read == -1) {
                    fclose(file);
                    ਵਾਪਸ ਝੂਠ;
                }
                ਜੇ (memcmp(tzInfo->defaultTZBuffer + tzInfo->defaultTZPosition, ਬਫਰ_File, ਮਾਪ_File_Read) != 0) {
                    ਨਤੀਜਾ = ਝੂਠ;
                    ਰੋਕੋ;
                }
                ਮਾਪ_File_ਖੱਬਾ -= ਮਾਪ_File_Read;
                tzInfo->defaultTZPosition += ਮਾਪ_File_Read;
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = ਝੂਠ;
    }

    ਜੇ (file != ਖਾਲੀ_ਸੂਚਕ) {
        fclose(file);
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}


/* dirent also lists two entries: "." and ".." that we can safely ignore. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SKIP1 "."
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SKIP2 ".."
ਸਥਿਰ UBool U_CALLCONV putil_cleanup();
ਸਥਿਰ CharString *g_ਖੋਜ_TZFile_ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;

/*
 * This method recursively traverses the directory given for a matching TZ file and returns the first match.
 * This function is not thread safe - it uses a global, gSearchTZFileResult, to hold its results.
 */
ਸਥਿਰ ਅੱਖਰ* ਖੋਜ_For_TZFile(ਅਚਲ ਅੱਖਰ* ਰਸਤਾ, DefaultTZInfo* tzInfo) {
    DIR* dirp = ਖਾਲੀ_ਸੂਚਕ;
    ਬਣਤਰ dirent* dir_ਇੰਦਰਾਜ = ਖਾਲੀ_ਸੂਚਕ;
    ਅੱਖਰ* ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;

    /* There is no reference file to compare against, so do not traverse the zoneinfo tree. */
    ਜੇ (tzInfo->defaultTZFilePtr == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    /* Save the current path */
    CharString curpath(ਰਸਤਾ, -1, ਸਥਿਤੀ);
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਜਾਓ cleanupAndReturn;
    }

    dirp = opendir(ਰਸਤਾ);
    ਜੇ (dirp == ਖਾਲੀ_ਸੂਚਕ) {
        ਜਾਓ cleanupAndReturn;
    }

    ਜੇ (g_ਖੋਜ_TZFile_ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        g_ਖੋਜ_TZFile_ਨਤੀਜਾ = ਨਵਾਂ CharString;
        ਜੇ (g_ਖੋਜ_TZFile_ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
            ਜਾਓ cleanupAndReturn;
        }
        ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    }

    /* Check each entry in the directory. */
    ਜਦੋਂ((dir_ਇੰਦਰਾਜ = readdir(dirp)) != ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ ਅੱਖਰ* dirName = dir_ਇੰਦਰਾਜ->d_name;
        ਜੇ (uprv_strcmp(dirName, SKIP1) != 0 && uprv_strcmp(dirName, SKIP2) != 0
            && uprv_strcmp(TZFILE_SKIP, dirName) != 0 && uprv_strcmp(TZFILE_SKIP2, dirName) != 0) {
            /* Create a newpath with the new entry to test each entry in the directory. */
            CharString newpath(curpath, ਸਥਿਤੀ);
            newpath.append(dirName, -1, ਸਥਿਤੀ);
            ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                ਰੋਕੋ;
            }

            DIR* subDirp = ਖਾਲੀ_ਸੂਚਕ;
            ਜੇ ((subDirp = opendir(newpath.ਅੰਕੜੇ())) != ਖਾਲੀ_ਸੂਚਕ) {
                /* If this new path is a directory, make a recursive call with the newpath. */
                closedir(subDirp);
                newpath.append('/', ਸਥਿਤੀ);
                ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                    ਰੋਕੋ;
                }
                ਨਤੀਜਾ = ਖੋਜ_For_TZFile(newpath.ਅੰਕੜੇ(), tzInfo);
                /*
                 Have to get out here. Otherwise, we'd keep looking
                 and return the first match in the top-level directory
                 if there's a match in the top-level. If not, this function
                 would return nullptr and set gTimeZoneBufferPtr to nullptr in initDefault().
                 It worked without this in most cases because we have a fallback of calling
                 localtime_r to figure out the default timezone.
                */
                ਜੇ (ਨਤੀਜਾ != ਖਾਲੀ_ਸੂਚਕ)
                    ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ {
                ਜੇ(compare_ਦੁਆਧਾਰੀ_Files(newpath.ਅੰਕੜੇ(), tzInfo)) {
                    int32_t amount_ਮੰਜ਼ਿਲ_Skip = ਆਕਾਰ(TZZONEINFO) - 1;
                    ਜੇ (amount_ਮੰਜ਼ਿਲ_Skip > newpath.length()) {
                        amount_ਮੰਜ਼ਿਲ_Skip = newpath.length();
                    }
                    ਅਚਲ ਅੱਖਰ* zoneid = newpath.ਅੰਕੜੇ() + amount_ਮੰਜ਼ਿਲ_Skip;
                    skipZoneIDPrefix(&zoneid);
                    g_ਖੋਜ_TZFile_ਨਤੀਜਾ->clear();
                    g_ਖੋਜ_TZFile_ਨਤੀਜਾ->append(zoneid, -1, ਸਥਿਤੀ);
                    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
                        ਰੋਕੋ;
                    }
                    ਨਤੀਜਾ = g_ਖੋਜ_TZFile_ਨਤੀਜਾ->ਅੰਕੜੇ();
                    /* Get out after the first one found. */
                    ਰੋਕੋ;
                }
            }
        }
    }

  cleanupAndReturn:
    ਜੇ (dirp) {
        closedir(dirp);
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ U_PLATFORM == U_PF_ANDROID
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ(system_property_read_callback)(ਅਚਲ prop_info* info,
                                           ਖਾਲੀ (*callback)(ਖਾਲੀ* cookie,
                                                            ਅਚਲ ਅੱਖਰ* name,
                                                            ਅਚਲ ਅੱਖਰ* value,
                                                            uint32_t serial),
                                           ਖਾਲੀ* cookie);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ(system_property_get)(ਅਚਲ ਅੱਖਰ*, ਅੱਖਰ*);

ਸਥਿਰ ਅੱਖਰ g_Android_ਸਮਾਂ_Zone[PROP_ਮੁੱਲ_MAX] = { '\0' };

ਸਥਿਰ ਖਾਲੀ u_property_read(ਖਾਲੀ* cookie, ਅਚਲ ਅੱਖਰ* name, ਅਚਲ ਅੱਖਰ* value,
                            uint32_t serial) {
    uprv_strcpy((ਅੱਖਰ* )cookie, value);
}
#ਜੇ_ਅੰਤ

U_CAPI ਖਾਲੀ U_EXPORT2
uprv_tzname_clear_cache()
{
#ਜੇ U_PLATFORM == U_PF_ANDROID
    /* Android's timezone is stored in system property. */
    g_Android_ਸਮਾਂ_Zone[0] = '\0';
    ਖਾਲੀ* libc = dlopen("libc.so", RTLD_NOLOAD);
    ਜੇ (libc) {
        /* Android API 26+ has new API to get system property and old API
         * (__system_property_get) is deprecated */
        system_property_read_callback* property_read_callback =
            (system_property_read_callback*)dlsym(
                libc, "__system_property_read_callback");
        ਜੇ (property_read_callback) {
            ਅਚਲ prop_info* info =
                __system_property_find("persist.sys.timezone");
            ਜੇ (info) {
                property_read_callback(info, &u_property_read, g_Android_ਸਮਾਂ_Zone);
            }
        } ਨਹੀਂਤਾਂ {
            system_property_get* property_get =
                (system_property_get*)dlsym(libc, "__system_property_get");
            ਜੇ (property_get) {
                property_get("persist.sys.timezone", g_Android_ਸਮਾਂ_Zone);
            }
        }
        dlclose(libc);
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    g_ਸਮਾਂ_Zone_ਬਫਰ_Ptr = ਖਾਲੀ_ਸੂਚਕ;
#ਜੇ_ਅੰਤ
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uprv_tzname(ਪੂਰਨ n)
{
    (ਖਾਲੀ)n; // Avoid unreferenced parameter warning.
    ਅਚਲ ਅੱਖਰ *tzid = ਖਾਲੀ_ਸੂਚਕ;
#ਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
    tzid = uprv_detect_Windows_ਸਮਾਂ_Zone();

    ਜੇ (tzid != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ tzid;
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_TZNAME
    // The return value is free'd in timezone.cpp on Windows because
    // the other code path returns a pointer to a heap location.
    // If we don't have a name already, then tzname wouldn't be any
    // better, so just fall back.
    ਵਾਪਸ uprv_strdup("");
#ਜੇ_ਅੰਤ // !U_TZNAME

#ਨਹੀਂਤਾਂ

/*#if U_PLATFORM_IS_DARWIN_BASED
    int ret;

    tzid = getenv("TZFILE");
    if (tzid != nullptr) {
        return tzid;
    }
#endif*/

/* This code can be temporarily disabled to test tzname resolution later on. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DEBUG_TZNAME
#ਜੇ U_PLATFORM == U_PF_ANDROID
    tzid = g_Android_ਸਮਾਂ_Zone;
#ਨਹੀਂਤਾਂ
    tzid = getenv("TZ");
#ਜੇ_ਅੰਤ
    ਜੇ (tzid != ਖਾਲੀ_ਸੂਚਕ && isValidOlsonID(tzid)
#ਜੇ U_PLATFORM == U_PF_SOLARIS
    /* Don't misinterpret TZ "localtime" on Solaris as a time zone name. */
        && uprv_strcmp(tzid, TZ_ENV_CHECK) != 0
#ਜੇ_ਅੰਤ
    ) {
        /* The colon forces tzset() to treat the remainder as zoneinfo path */
        ਜੇ (tzid[0] == ':') {
            tzid++;
        }
        /* This might be a good Olson ID. */
        skipZoneIDPrefix(&tzid);
        ਵਾਪਸ tzid;
    }
    /* else U_TZNAME will give a better result. */
#ਜੇ_ਅੰਤ

#ਜੇ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    /* Caller must handle threading issues */
    ਜੇ (g_ਸਮਾਂ_Zone_ਬਫਰ_Ptr == ਖਾਲੀ_ਸੂਚਕ) {
        /*
        This is a trick to look at the name of the link to get the Olson ID
        because the tzfile contents is underspecified.
        This isn't guaranteed to work because it may not be a symlink.
        */
        ਅੱਖਰ *ret = realpath(TZDEFAULT, g_ਸਮਾਂ_Zone_ਬਫਰ);
        ਜੇ (ret != ਖਾਲੀ_ਸੂਚਕ && uprv_strcmp(TZDEFAULT, g_ਸਮਾਂ_Zone_ਬਫਰ) != 0) {
            int32_t tz_Zone_Info_ਪੂਛ_Len = uprv_strlen(TZZONEINFOTAIL);
            ਅਚਲ ਅੱਖਰ *tz_Zone_Info_ਪੂਛ_Ptr = uprv_strstr(g_ਸਮਾਂ_Zone_ਬਫਰ, TZZONEINFOTAIL);
            // MacOS14 has the realpath as something like
            // /usr/share/zoneinfo.default/Australia/Melbourne
            // which will not have "/zoneinfo/" in the path.
            // Therefore if we fail, we fall back to read the link which is
            // /var/db/timezone/zoneinfo/Australia/Melbourne
            // We also fall back to reading the link if the realpath leads to something like
            // /usr/share/zoneinfo/posixrules
            ਜੇ (tz_Zone_Info_ਪੂਛ_Ptr == ਖਾਲੀ_ਸੂਚਕ ||
                    uprv_strcmp(tz_Zone_Info_ਪੂਛ_Ptr + tz_Zone_Info_ਪੂਛ_Len, "posixrules") == 0) {
                ssize_t size = readlink(TZDEFAULT, g_ਸਮਾਂ_Zone_ਬਫਰ, ਆਕਾਰ(g_ਸਮਾਂ_Zone_ਬਫਰ)-1);
                ਜੇ (size > 0) {
                    g_ਸਮਾਂ_Zone_ਬਫਰ[size] = 0;
                    tz_Zone_Info_ਪੂਛ_Ptr = uprv_strstr(g_ਸਮਾਂ_Zone_ਬਫਰ, TZZONEINFOTAIL);
                }
            }
            ਜੇ (tz_Zone_Info_ਪੂਛ_Ptr != ਖਾਲੀ_ਸੂਚਕ) {
                tz_Zone_Info_ਪੂਛ_Ptr += tz_Zone_Info_ਪੂਛ_Len;
                skipZoneIDPrefix(&tz_Zone_Info_ਪੂਛ_Ptr);
                ਜੇ (isValidOlsonID(tz_Zone_Info_ਪੂਛ_Ptr)) {
                    ਵਾਪਸ (g_ਸਮਾਂ_Zone_ਬਫਰ_Ptr = tz_Zone_Info_ਪੂਛ_Ptr);
                }
            }
        } ਨਹੀਂਤਾਂ {
#ਜੇ defined(ਖੋਜ_TZFILE)
            DefaultTZInfo* tzInfo = (DefaultTZInfo*)uprv_malloc(ਆਕਾਰ(DefaultTZInfo));
            ਜੇ (tzInfo != ਖਾਲੀ_ਸੂਚਕ) {
                tzInfo->defaultTZBuffer = ਖਾਲੀ_ਸੂਚਕ;
                tzInfo->default_TZFile_ਮਾਪ = 0;
                tzInfo->defaultTZFilePtr = fopen(TZDEFAULT, "r");
                tzInfo->defaultTZPosition = 0;

                g_ਸਮਾਂ_Zone_ਬਫਰ_Ptr = ਖੋਜ_For_TZFile(TZZONEINFO, tzInfo);

                /* Free previously allocated memory */
                ਜੇ (tzInfo->defaultTZBuffer != ਖਾਲੀ_ਸੂਚਕ) {
                    uprv_free(tzInfo->defaultTZBuffer);
                }
                ਜੇ (tzInfo->defaultTZFilePtr != ਖਾਲੀ_ਸੂਚਕ) {
                    fclose(tzInfo->defaultTZFilePtr);
                }
                uprv_free(tzInfo);
            }

            ਜੇ (g_ਸਮਾਂ_Zone_ਬਫਰ_Ptr != ਖਾਲੀ_ਸੂਚਕ && isValidOlsonID(g_ਸਮਾਂ_Zone_ਬਫਰ_Ptr)) {
                ਵਾਪਸ g_ਸਮਾਂ_Zone_ਬਫਰ_Ptr;
            }
#ਜੇ_ਅੰਤ
        }
    }
    ਨਹੀਂਤਾਂ {
        ਵਾਪਸ g_ਸਮਾਂ_Zone_ਬਫਰ_Ptr;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_TZNAME
#ਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
    /* The return value is free'd in timezone.cpp on Windows because
     * the other code path returns a pointer to a heap location. */
    ਵਾਪਸ uprv_strdup(U_TZNAME[n]);
#ਨਹੀਂਤਾਂ
    /*
    U_TZNAME is usually a non-unique abbreviation, which isn't normally usable.
    So we remap the abbreviation to an olson ID.

    Since Windows exposes a little more timezone information,
    we normally don't use this code on Windows because
    uprv_detectWindowsTimeZone should have already given the correct answer.
    */
    {
        ਬਣਤਰ tm juneSol, decemberSol;
        ਪੂਰਨ daylightType;
        ਸਥਿਰ ਅਚਲ time_t juneSolstice=1182478260; /*2007-06-21 18:11 UT*/
        ਸਥਿਰ ਅਚਲ time_t decemberSolstice=1198332540; /*2007-12-22 06:09 UT*/

        /* This probing will tell us when daylight savings occurs.  */
        localtime_r(&juneSolstice, &juneSol);
        localtime_r(&decemberSolstice, &decemberSol);
        ਜੇ(decemberSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_DECEMBER;
        } ਨਹੀਂਤਾਂ ਜੇ(juneSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_JUNE;
        } ਨਹੀਂਤਾਂ {
          daylightType = U_DAYLIGHT_NONE;
        }
        tzid = remap_Short_ਸਮਾਂ_Zone(U_TZNAME[0], U_TZNAME[1], daylightType, uprv_timezone());
        ਜੇ (tzid != ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ tzid;
        }
    }
    ਵਾਪਸ U_TZNAME[n];
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ "";
#ਜੇ_ਅੰਤ
}

/* Get and set the ICU data directory --------------------------------------- */

ਸਥਿਰ icu::UInitOnce g_ਅੰਕੜੇ_Dir_Init_Once {};
ਸਥਿਰ ਅੱਖਰ *g_ਅੰਕੜੇ_Directory = ਖਾਲੀ_ਸੂਚਕ;

UInitOnce g_ਸਮਾਂ_Zone_Files_Init_Once {};
ਸਥਿਰ CharString *g_ਸਮਾਂ_Zone_Files_Directory = ਖਾਲੀ_ਸੂਚਕ;

#ਜੇ U_POSIX_LOCALE || U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
 ਸਥਿਰ ਅਚਲ ਅੱਖਰ *gCorrectedPOSIXLocale = ਖਾਲੀ_ਸੂਚਕ; /* Sometimes heap allocated */
 ਸਥਿਰ ਤਾਰਕਿਕ g_Corrected_POSIXLocale_ਢੇਰ_Allocated = ਝੂਠ;
#ਜੇ_ਅੰਤ

ਸਥਿਰ UBool U_CALLCONV putil_cleanup()
{
    ਜੇ (g_ਅੰਕੜੇ_Directory && *g_ਅੰਕੜੇ_Directory) {
        uprv_free(g_ਅੰਕੜੇ_Directory);
    }
    g_ਅੰਕੜੇ_Directory = ਖਾਲੀ_ਸੂਚਕ;
    g_ਅੰਕੜੇ_Dir_Init_Once.reset();

    ਮਿਟਾਓ g_ਸਮਾਂ_Zone_Files_Directory;
    g_ਸਮਾਂ_Zone_Files_Directory = ਖਾਲੀ_ਸੂਚਕ;
    g_ਸਮਾਂ_Zone_Files_Init_Once.reset();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਖੋਜ_TZFILE
    ਮਿਟਾਓ g_ਖੋਜ_TZFile_ਨਤੀਜਾ;
    g_ਖੋਜ_TZFile_ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
#ਜੇ_ਅੰਤ

#ਜੇ U_POSIX_LOCALE || U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
    ਜੇ (gCorrectedPOSIXLocale && g_Corrected_POSIXLocale_ਢੇਰ_Allocated) {
        uprv_free(const_cast<ਅੱਖਰ *>(gCorrectedPOSIXLocale));
        gCorrectedPOSIXLocale = ਖਾਲੀ_ਸੂਚਕ;
        g_Corrected_POSIXLocale_ਢੇਰ_Allocated = ਝੂਠ;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ ਸੱਚ;
}

/*
 * Set the data directory.
 *    Make a copy of the passed string, and set the global data dir to point to it.
 */
U_CAPI ਖਾਲੀ U_EXPORT2
u_ਸੈੱਟ_ਅੰਕੜੇ_Directory(ਅਚਲ ਅੱਖਰ *directory) {
    ਅੱਖਰ *new_ਅੰਕੜੇ_Dir;
    int32_t length;

    ਜੇ(directory==ਖਾਲੀ_ਸੂਚਕ || *directory==0) {
        /* A small optimization to prevent the malloc and copy when the
        shared library is used, and this is a way to make sure that nullptr
        is never returned.
        */
        new_ਅੰਕੜੇ_Dir = (ਅੱਖਰ *)"";
    }
    ਨਹੀਂਤਾਂ {
        length=(int32_t)uprv_strlen(directory);
        new_ਅੰਕੜੇ_Dir = (ਅੱਖਰ *)uprv_malloc(length + 2);
        /* Exit out if newDataDir could not be created. */
        ਜੇ (new_ਅੰਕੜੇ_Dir == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ;
        }
        uprv_strcpy(new_ਅੰਕੜੇ_Dir, directory);

#ਜੇ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
        {
            ਅੱਖਰ *p;
            ਜਦੋਂ((p = uprv_strchr(new_ਅੰਕੜੇ_Dir, U_FILE_ALT_SEP_CHAR)) != ਖਾਲੀ_ਸੂਚਕ) {
                *p = U_FILE_SEP_CHAR;
            }
        }
#ਜੇ_ਅੰਤ
    }

    ਜੇ (g_ਅੰਕੜੇ_Directory && *g_ਅੰਕੜੇ_Directory) {
        uprv_free(g_ਅੰਕੜੇ_Directory);
    }
    g_ਅੰਕੜੇ_Directory = new_ਅੰਕੜੇ_Dir;
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
}

U_CAPI UBool U_EXPORT2
uprv_ਰਸਤਾ_Is_Absolute(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
  ਜੇ(!ਰਸਤਾ || !*ਰਸਤਾ) {
    ਵਾਪਸ ਝੂਠ;
  }

  ਜੇ(*ਰਸਤਾ == U_FILE_SEP_CHAR) {
    ਵਾਪਸ ਸੱਚ;
  }

#ਜੇ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
  ਜੇ(*ਰਸਤਾ == U_FILE_ALT_SEP_CHAR) {
    ਵਾਪਸ ਸੱਚ;
  }
#ਜੇ_ਅੰਤ

#ਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
  ਜੇ( (((ਰਸਤਾ[0] >= 'A') && (ਰਸਤਾ[0] <= 'Z')) ||
       ((ਰਸਤਾ[0] >= 'a') && (ਰਸਤਾ[0] <= 'z'))) &&
      ਰਸਤਾ[1] == ':' ) {
    ਵਾਪਸ ਸੱਚ;
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ ਝੂਠ;
}

/* Backup setting of ICU_DATA_DIR_PREFIX_ENV_VAR
   (needed for some Darwin ICU build environments) */
#ਜੇ U_PLATFORM_IS_DARWIN_BASED && defined(ਨਿਸ਼ਾਨਾ_OS_SIMULATOR) && ਨਿਸ਼ਾਨਾ_OS_SIMULATOR
# ਜੇ !defined(ICU_ਅੰਕੜੇ_DIR_ਅਗੇਤਰ_ENV_VAR)
#  define ICU_ਅੰਕੜੇ_DIR_ਅਗੇਤਰ_ENV_VAR "IPHONE_SIMULATOR_ROOT"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(ICU_ਅੰਕੜੇ_DIR_WINDOWS)
// Helper function to get the ICU Data Directory under the Windows directory location.
ਸਥਿਰ BOOL U_CALLCONV get_Icu_ਅੰਕੜੇ_Directory_Under_Windows_Directory(ਅੱਖਰ* directory_ਬਫਰ, UINT ਬਫਰ_ਲੰਮਾਈ)
{
    wchar_t windows_ਰਸਤਾ[MAX_ਰਸਤਾ];
    ਅੱਖਰ windows_ਰਸਤਾ_Utf8[MAX_ਰਸਤਾ];

    UINT length = GetSystemWindowsDirectoryW(windows_ਰਸਤਾ, UPRV_LENGTHOF(windows_ਰਸਤਾ));
    ਜੇ ((length > 0) && (length < (UPRV_LENGTHOF(windows_ਰਸਤਾ) - 1))) {
        // Convert UTF-16 to a UTF-8 string.
        UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
        int32_t windows_ਰਸਤਾ_Utf8_Len = 0;
        u_str_ਮੰਜ਼ਿਲ_UTF8(windows_ਰਸਤਾ_Utf8, static_cast<int32_t>(UPRV_LENGTHOF(windows_ਰਸਤਾ_Utf8)),
            &windows_ਰਸਤਾ_Utf8_Len, reinterpret_cast<ਅਚਲ char16_t*>(windows_ਰਸਤਾ), -1, &ਸਥਿਤੀ);

        ਜੇ (U_SUCCESS(ਸਥਿਤੀ) && (ਸਥਿਤੀ != U_STRING_NOT_TERMINATED_WARNING) &&
            (windows_ਰਸਤਾ_Utf8_Len < (UPRV_LENGTHOF(windows_ਰਸਤਾ_Utf8) - 1))) {
            // Ensure it always has a separator, so we can append the ICU data path.
            ਜੇ (windows_ਰਸਤਾ_Utf8[windows_ਰਸਤਾ_Utf8_Len - 1] != U_FILE_SEP_CHAR) {
                windows_ਰਸਤਾ_Utf8[windows_ਰਸਤਾ_Utf8_Len++] = U_FILE_SEP_CHAR;
                windows_ਰਸਤਾ_Utf8[windows_ਰਸਤਾ_Utf8_Len] = '\0';
            }
            // Check if the concatenated string will fit.
            ਜੇ ((windows_ਰਸਤਾ_Utf8_Len + UPRV_LENGTHOF(ICU_ਅੰਕੜੇ_DIR_WINDOWS)) < ਬਫਰ_ਲੰਮਾਈ) {
                uprv_strcpy(directory_ਬਫਰ, windows_ਰਸਤਾ_Utf8);
                uprv_strcat(directory_ਬਫਰ, ICU_ਅੰਕੜੇ_DIR_WINDOWS);
                ਵਾਪਸ ਸੱਚ;
            }
        }
    }

    ਵਾਪਸ ਝੂਠ;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ U_CALLCONV ਅੰਕੜੇ_Directory_Init_Fn() {
    /* If we already have the directory, then return immediately. Will happen if user called
     * u_setDataDirectory().
     */
    ਜੇ (g_ਅੰਕੜੇ_Directory) {
        ਵਾਪਸ;
    }

    ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;

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
#   ਜੇ !defined(ICU_NO_USER_ਅੰਕੜੇ_OVERRIDE) && !UCONFIG_NO_FILE_IO
    /* First try to get the environment variable */
#     ਜੇ U_PLATFORM_HAS_WINUWP_API == 0  // Windows UWP does not support getenv
        ਰਸਤਾ=getenv("ICU_DATA");
#     endif
#   endif

    /* ICU_DATA_DIR may be set as a compile option.
     * U_ICU_DATA_DEFAULT_DIR is provided and is set by ICU at compile time
     * and is used only when data is built in archive mode eliminating the need
     * for ICU_DATA_DIR to be set. U_ICU_DATA_DEFAULT_DIR is set to the installation
     * directory of the data dat file. Users should use ICU_DATA_DIR if they want to
     * set their own path.
     */
#ਜੇ defined(ICU_ਅੰਕੜੇ_DIR) || defined(U_ICU_ਅੰਕੜੇ_DEFAULT_DIR)
# ਜੇ defined(ICU_ਅੰਕੜੇ_DIR_ਅਗੇਤਰ_ENV_VAR)
    ਅੱਖਰ datadir_ਰਸਤਾ_ਬਫਰ[ਰਸਤਾ_MAX];
# endif
    ਜੇ(ਰਸਤਾ==ਖਾਲੀ_ਸੂਚਕ || *ਰਸਤਾ==0) {
# ਜੇ defined(ICU_ਅੰਕੜੇ_DIR_ਅਗੇਤਰ_ENV_VAR)
        ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ = getenv(ICU_ਅੰਕੜੇ_DIR_ਅਗੇਤਰ_ENV_VAR);
# endif
# ifdef ICU_ਅੰਕੜੇ_DIR
        ਰਸਤਾ=ICU_ਅੰਕੜੇ_DIR;
# ਨਹੀਂਤਾਂ
        ਰਸਤਾ=U_ICU_ਅੰਕੜੇ_DEFAULT_DIR;
# endif
# ਜੇ defined(ICU_ਅੰਕੜੇ_DIR_ਅਗੇਤਰ_ENV_VAR)
        ਜੇ (ਅਗੇਤਰ != ਖਾਲੀ_ਸੂਚਕ) {
            snprintf(datadir_ਰਸਤਾ_ਬਫਰ, ਆਕਾਰ(datadir_ਰਸਤਾ_ਬਫਰ), "%s%s", ਅਗੇਤਰ, ਰਸਤਾ);
            ਰਸਤਾ=datadir_ਰਸਤਾ_ਬਫਰ;
        }
# endif
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(ICU_ਅੰਕੜੇ_DIR_WINDOWS)
    ਅੱਖਰ datadir_ਰਸਤਾ_ਬਫਰ[MAX_ਰਸਤਾ];
    ਜੇ (get_Icu_ਅੰਕੜੇ_Directory_Under_Windows_Directory(datadir_ਰਸਤਾ_ਬਫਰ, UPRV_LENGTHOF(datadir_ਰਸਤਾ_ਬਫਰ))) {
        ਰਸਤਾ = datadir_ਰਸਤਾ_ਬਫਰ;
    }
#ਜੇ_ਅੰਤ

    ਜੇ(ਰਸਤਾ==ਖਾਲੀ_ਸੂਚਕ) {
        /* It looks really bad, set it to something. */
        ਰਸਤਾ = "";
    }

    u_ਸੈੱਟ_ਅੰਕੜੇ_Directory(ਰਸਤਾ);
}

U_CAPI ਅਚਲ ਅੱਖਰ * U_EXPORT2
u_get_ਅੰਕੜੇ_Directory() {
    umtx_initOnce(g_ਅੰਕੜੇ_Dir_Init_Once, &ਅੰਕੜੇ_Directory_Init_Fn);
    ਵਾਪਸ g_ਅੰਕੜੇ_Directory;
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_ਸਮਾਂ_Zone_Files_Dir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ;
    }
    g_ਸਮਾਂ_Zone_Files_Directory->clear();
    g_ਸਮਾਂ_Zone_Files_Directory->append(ਰਸਤਾ, ਸਥਿਤੀ);
#ਜੇ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    ਅੱਖਰ *p = g_ਸਮਾਂ_Zone_Files_Directory->ਅੰਕੜੇ();
    ਜਦੋਂ ((p = uprv_strchr(p, U_FILE_ALT_SEP_CHAR)) != ਖਾਲੀ_ਸੂਚਕ) {
        *p = U_FILE_SEP_CHAR;
    }
#ਜੇ_ਅੰਤ
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_STRING(x) ਮੰਜ਼ਿਲ_STRING_2(x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਮੰਜ਼ਿਲ_STRING_2(x) #x

ਸਥਿਰ ਖਾਲੀ U_CALLCONV ਸਮਾਂ_Zone_ਅੰਕੜੇ_Dir_Init_Fn(UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(g_ਸਮਾਂ_Zone_Files_Directory == ਖਾਲੀ_ਸੂਚਕ);
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    g_ਸਮਾਂ_Zone_Files_Directory = ਨਵਾਂ CharString();
    ਜੇ (g_ਸਮਾਂ_Zone_Files_Directory == ਖਾਲੀ_ਸੂਚਕ) {
        ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    ਅਚਲ ਅੱਖਰ *dir = "";

#ਜੇ defined(ICU_TIMEZONE_FILES_DIR_ਅਗੇਤਰ_ENV_VAR)
    ਅੱਖਰ timezonefilesdir_ਰਸਤਾ_ਬਫਰ[ਰਸਤਾ_MAX];
    ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ = getenv(ICU_TIMEZONE_FILES_DIR_ਅਗੇਤਰ_ENV_VAR);
#ਜੇ_ਅੰਤ

#ਜੇ U_PLATFORM_HAS_WINUWP_API == 1
// The UWP version does not support the environment variable setting.

# ਜੇ defined(ICU_ਅੰਕੜੇ_DIR_WINDOWS)
    // When using the Windows system data, we can possibly pick up time zone data from the Windows directory.
    ਅੱਖਰ datadir_ਰਸਤਾ_ਬਫਰ[MAX_ਰਸਤਾ];
    ਜੇ (get_Icu_ਅੰਕੜੇ_Directory_Under_Windows_Directory(datadir_ਰਸਤਾ_ਬਫਰ, UPRV_LENGTHOF(datadir_ਰਸਤਾ_ਬਫਰ))) {
        dir = datadir_ਰਸਤਾ_ਬਫਰ;
    }
# endif

#ਨਹੀਂਤਾਂ
    dir = getenv("ICU_TIMEZONE_FILES_DIR");
#ਜੇ_ਅੰਤ // U_PLATFORM_HAS_WINUWP_API

#ਜੇ defined(U_TIMEZONE_FILES_DIR)
    ਜੇ (dir == ਖਾਲੀ_ਸੂਚਕ) {
        // Build time configuration setting.
        dir = ਮੰਜ਼ਿਲ_STRING(U_TIMEZONE_FILES_DIR);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (dir == ਖਾਲੀ_ਸੂਚਕ) {
        dir = "";
    }

#ਜੇ defined(ICU_TIMEZONE_FILES_DIR_ਅਗੇਤਰ_ENV_VAR)
    ਜੇ (ਅਗੇਤਰ != ਖਾਲੀ_ਸੂਚਕ) {
        snprintf(timezonefilesdir_ਰਸਤਾ_ਬਫਰ, ਆਕਾਰ(timezonefilesdir_ਰਸਤਾ_ਬਫਰ), "%s%s", ਅਗੇਤਰ, dir);
        dir = timezonefilesdir_ਰਸਤਾ_ਬਫਰ;
    }
#ਜੇ_ਅੰਤ

    ਸੈੱਟ_ਸਮਾਂ_Zone_Files_Dir(dir, ਸਥਿਤੀ);
}


U_CAPI ਅਚਲ ਅੱਖਰ * U_EXPORT2
u_get_ਸਮਾਂ_Zone_Files_Directory(UError_ਕੋਡ *ਸਥਿਤੀ) {
    umtx_initOnce(g_ਸਮਾਂ_Zone_Files_Init_Once, &ਸਮਾਂ_Zone_ਅੰਕੜੇ_Dir_Init_Fn, *ਸਥਿਤੀ);
    ਵਾਪਸ U_SUCCESS(*ਸਥਿਤੀ) ? g_ਸਮਾਂ_Zone_Files_Directory->ਅੰਕੜੇ() : "";
}

U_CAPI ਖਾਲੀ U_EXPORT2
u_ਸੈੱਟ_ਸਮਾਂ_Zone_Files_Directory(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UError_ਕੋਡ *ਸਥਿਤੀ) {
    umtx_initOnce(g_ਸਮਾਂ_Zone_Files_Init_Once, &ਸਮਾਂ_Zone_ਅੰਕੜੇ_Dir_Init_Fn, *ਸਥਿਤੀ);
    ਸੈੱਟ_ਸਮਾਂ_Zone_Files_Dir(ਰਸਤਾ, *ਸਥਿਤੀ);

    // Note: this function does some extra churn, first setting based on the
    //       environment, then immediately replacing with the value passed in.
    //       The logic is simpler that way, and performance shouldn't be an issue.
}


#ਜੇ U_POSIX_LOCALE
/* A helper function used by uprv_getPOSIXIDForDefaultLocale and
 * uprv_getPOSIXIDForDefaultCodepage. Returns the posix locale id for
 * LC_CTYPE and LC_MESSAGES. It doesn't support other locale categories.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *uprv_getPOSIXIDForCategory(ਪੂਰਨ category)
{
    ਅਚਲ ਅੱਖਰ* posixID = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (category == LC_MESSAGES || category == LC_CTYPE) {
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
        posixID = setlocale(category, ਖਾਲੀ_ਸੂਚਕ);
        ਜੇ ((posixID == ਖਾਲੀ_ਸੂਚਕ)
            || (uprv_strcmp("C", posixID) == 0)
            || (uprv_strcmp("POSIX", posixID) == 0))
        {
            /* Maybe we got some garbage.  Try something more reasonable */
            posixID = getenv("LC_ALL");
            /* Solaris speaks POSIX -  See IEEE Std 1003.1-2008
             * This is needed to properly handle empty env. variables
             */
#ਜੇ U_PLATFORM == U_PF_SOLARIS
            ਜੇ ((posixID == 0) || (posixID[0] == '\0')) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                ਜੇ ((posixID == 0) || (posixID[0] == '\0')) {
#ਨਹੀਂਤਾਂ
            ਜੇ (posixID == ਖਾਲੀ_ਸੂਚਕ) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                ਜੇ (posixID == ਖਾਲੀ_ਸੂਚਕ) {
#ਜੇ_ਅੰਤ
                    posixID = getenv("LANG");
                }
            }
        }
    }
    ਜੇ ((posixID == ਖਾਲੀ_ਸੂਚਕ)
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
    ਵਾਪਸ posixID;
}

/* Return just the POSIX id for the default locale, whatever happens to be in
 * it. It gets the value from LC_MESSAGES and indirectly from LC_ALL and LANG.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *uprv_getPOSIXIDForDefaultLocale()
{
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ* posixID = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (posixID == ਖਾਲੀ_ਸੂਚਕ) {
        posixID = uprv_getPOSIXIDForCategory(LC_MESSAGES);
    }
    ਵਾਪਸ posixID;
}

#ਜੇ !U_CHARSET_IS_UTF8
/* Return just the POSIX id for the default codepage, whatever happens to be in
 * it. It gets the value from LC_CTYPE and indirectly from LC_ALL and LANG.
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *uprv_getPOSIXIDForDefaultCodepage()
{
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ* posixID = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (posixID == 0) {
        posixID = uprv_getPOSIXIDForCategory(LC_CTYPE);
    }
    ਵਾਪਸ posixID;
}
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

/* NOTE: The caller should handle thread safety */
U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uprv_getDefaultLocaleID()
{
#ਜੇ U_POSIX_LOCALE
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
    ਅਚਲ ਅੱਖਰ* posixID = uprv_getPOSIXIDForDefaultLocale();

    /* Format: (no spaces)
    ll [ _CC ] [ . MM ] [ @ VV]

      l = lang, C = ctry, M = charmap, V = variant
    */

    ਜੇ (gCorrectedPOSIXLocale != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ gCorrectedPOSIXLocale;
    }

    // Copy the ID into owned memory.
    // Over-allocate in case we replace "C" with "en_US_POSIX" (+10), + null termination
    ਅੱਖਰ *correctedPOSIXLocale = static_cast<ਅੱਖਰ *>(uprv_malloc(uprv_strlen(posixID) + 10 + 1));
    ਜੇ (correctedPOSIXLocale == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    uprv_strcpy(correctedPOSIXLocale, posixID);

    ਅੱਖਰ *ਹੱਦਬੰਦੀ;
    ਜੇ ((ਹੱਦਬੰਦੀ = uprv_strchr(correctedPOSIXLocale, '.')) != ਖਾਲੀ_ਸੂਚਕ) {
        *ਹੱਦਬੰਦੀ = 0;
    }
    ਜੇ ((ਹੱਦਬੰਦੀ = uprv_strchr(correctedPOSIXLocale, '@')) != ਖਾਲੀ_ਸੂਚਕ) {
        *ਹੱਦਬੰਦੀ = 0;
    }

    ਜੇ ((uprv_strcmp("C", correctedPOSIXLocale) == 0) // no @ variant
        || (uprv_strcmp("POSIX", correctedPOSIXLocale) == 0)) {
      // Raw input was C.* or POSIX.*, Give it a nice POSIX default value.
      // (The "C"/"POSIX" case is handled in uprv_getPOSIXIDForCategory())
      uprv_strcpy(correctedPOSIXLocale, "en_US_POSIX");
    }

    /* Note that we scan the *uncorrected* ID. */
    ਅਚਲ ਅੱਖਰ *p;
    ਜੇ ((p = uprv_strrchr(posixID, '@')) != ਖਾਲੀ_ਸੂਚਕ) {
        p++;

        /* Take care of any special cases here.. */
        ਜੇ (!uprv_strcmp(p, "nynorsk")) {
            p = "NY";
            /* Don't worry about no__NY. In practice, it won't appear. */
        }

        ਜੇ (uprv_strchr(correctedPOSIXLocale,'_') == ਖਾਲੀ_ਸੂਚਕ) {
            uprv_strcat(correctedPOSIXLocale, "__"); /* aa@b -> aa__b (note this can make the new locale 1 char longer) */
        }
        ਨਹੀਂਤਾਂ {
            uprv_strcat(correctedPOSIXLocale, "_"); /* aa_CC@b -> aa_CC_b */
        }

        ਅਚਲ ਅੱਖਰ *q;
        ਜੇ ((q = uprv_strchr(p, '.')) != ਖਾਲੀ_ਸੂਚਕ) {
            /* How big will the resulting string be? */
            int32_t len = (int32_t)(uprv_strlen(correctedPOSIXLocale) + (q-p));
            uprv_strncat(correctedPOSIXLocale, p, q-p); // do not include charset
            correctedPOSIXLocale[len] = 0;
        }
        ਨਹੀਂਤਾਂ {
            /* Anything following the @ sign */
            uprv_strcat(correctedPOSIXLocale, p);
        }

        /* Should there be a map from 'no@nynorsk' -> no_NO_NY here?
         * How about 'russian' -> 'ru'?
         * Many of the other locales using ISO codes will be handled by the
         * canonicalization functions in uloc_getDefault.
         */
    }

    ਜੇ (gCorrectedPOSIXLocale == ਖਾਲੀ_ਸੂਚਕ) {
        gCorrectedPOSIXLocale = correctedPOSIXLocale;
        g_Corrected_POSIXLocale_ਢੇਰ_Allocated = ਸੱਚ;
        ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
        correctedPOSIXLocale = ਖਾਲੀ_ਸੂਚਕ;
    }
    posixID = gCorrectedPOSIXLocale;

    ਜੇ (correctedPOSIXLocale != ਖਾਲੀ_ਸੂਚਕ) {  /* Was already set - clean up. */
        uprv_free(correctedPOSIXLocale);
    }

    ਵਾਪਸ posixID;

#ਜਾਂਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POSIX_LOCALE_ਸਮਰੱਥਾ 64
    UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
    ਅੱਖਰ *correctedPOSIXLocale = ਖਾਲੀ_ਸੂਚਕ;

    // If we have already figured this out just use the cached value
    ਜੇ (gCorrectedPOSIXLocale != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ gCorrectedPOSIXLocale;
    }

    // No cached value, need to determine the current value
    ਸਥਿਰ WCHAR windowsLocale[LOCALE_NAME_MAX_ਲੰਮਾਈ] = {};
    ਪੂਰਨ length = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, windowsLocale, LOCALE_NAME_MAX_ਲੰਮਾਈ);

    // Now we should have a Windows locale name that needs converted to the POSIX style.
    ਜੇ (length > 0) // If length is 0, then the GetLocaleInfoEx failed.
    {
        // First we need to go from UTF-16 to char (and also convert from _ to - while we're at it.)
        ਅੱਖਰ modifiedWindowsLocale[LOCALE_NAME_MAX_ਲੰਮਾਈ] = {};

        int32_t i;
        ਲਈ (i = 0; i < UPRV_LENGTHOF(modifiedWindowsLocale); i++)
        {
            ਜੇ (windowsLocale[i] == '_')
            {
                modifiedWindowsLocale[i] = '-';
            }
            ਨਹੀਂਤਾਂ
            {
                modifiedWindowsLocale[i] = static_cast<ਅੱਖਰ>(windowsLocale[i]);
            }

            ਜੇ (modifiedWindowsLocale[i] == '\0')
            {
                ਰੋਕੋ;
            }
        }

        ਜੇ (i >= UPRV_LENGTHOF(modifiedWindowsLocale))
        {
            // Ran out of room, can't really happen, maybe we'll be lucky about a matching
            // locale when tags are dropped
            modifiedWindowsLocale[UPRV_LENGTHOF(modifiedWindowsLocale) - 1] = '\0';
        }

        // Now normalize the resulting name
        correctedPOSIXLocale = static_cast<ਅੱਖਰ *>(uprv_malloc(POSIX_LOCALE_ਸਮਰੱਥਾ + 1));
        /* TODO: Should we just exit on memory allocation failure? */
        ਜੇ (correctedPOSIXLocale)
        {
            int32_t posixLen = uloc_canonicalize(modifiedWindowsLocale, correctedPOSIXLocale, POSIX_LOCALE_ਸਮਰੱਥਾ, &ਸਥਿਤੀ);
            ਜੇ (U_SUCCESS(ਸਥਿਤੀ))
            {
                *(correctedPOSIXLocale + posixLen) = 0;
                gCorrectedPOSIXLocale = correctedPOSIXLocale;
                g_Corrected_POSIXLocale_ਢੇਰ_Allocated = ਸੱਚ;
                ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
            }
            ਨਹੀਂਤਾਂ
            {
                uprv_free(correctedPOSIXLocale);
            }
        }
    }

    // If unable to find a locale we can agree upon, use en-US by default
    ਜੇ (gCorrectedPOSIXLocale == ਖਾਲੀ_ਸੂਚਕ) {
        gCorrectedPOSIXLocale = "en_US";
    }
    ਵਾਪਸ gCorrectedPOSIXLocale;

#ਜਾਂਜੇ U_PLATFORM == U_PF_OS400
    /* locales are process scoped and are by definition thread safe */
    ਸਥਿਰ ਅੱਖਰ correctedLocale[64];
    ਅਚਲ  ਅੱਖਰ *localeID = getenv("LC_ALL");
           ਅੱਖਰ *p;

    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ)
        localeID = getenv("LANG");
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ)
        localeID = setlocale(LC_ALL, ਖਾਲੀ_ਸੂਚਕ);
    /* Make sure we have something... */
    ਜੇ (localeID == ਖਾਲੀ_ਸੂਚਕ)
        ਵਾਪਸ "en_US_POSIX";

    /* Extract the locale name from the path. */
    ਜੇ((p = uprv_strrchr(localeID, '/')) != ਖਾਲੀ_ਸੂਚਕ)
    {
        /* Increment p to start of locale name. */
        p++;
        localeID = p;
    }

    /* Copy to work location. */
    uprv_strcpy(correctedLocale, localeID);

    /* Strip off the '.locale' extension. */
    ਜੇ((p = uprv_strchr(correctedLocale, '.')) != ਖਾਲੀ_ਸੂਚਕ) {
        *p = 0;
    }

    /* Upper case the locale name. */
    T_CString_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Case(correctedLocale);

    /* See if we are using the POSIX locale.  Any of the
    * following are equivalent and use the same QLGPGCMA
    * (POSIX) locale.
    * QLGPGCMA2 means UCS2
    * QLGPGCMA_4 means UTF-32
    * QLGPGCMA_8 means UTF-8
    */
    ਜੇ ((uprv_strcmp("C", correctedLocale) == 0) ||
        (uprv_strcmp("POSIX", correctedLocale) == 0) ||
        (uprv_strncmp("QLGPGCMA", correctedLocale, 8) == 0))
    {
        uprv_strcpy(correctedLocale, "en_US_POSIX");
    }
    ਨਹੀਂਤਾਂ
    {
        int16_t LocaleLen;

        /* Lower case the lang portion. */
        ਲਈ(p = correctedLocale; *p != 0 && *p != '_'; p++)
        {
            *p = uprv_tolower(*p);
        }

        /* Adjust for Euro.  After '_E' add 'URO'. */
        LocaleLen = uprv_strlen(correctedLocale);
        ਜੇ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'E')
        {
            uprv_strcat(correctedLocale, "URO");
        }

        /* If using Lotus-based locale then convert to
         * equivalent non Lotus.
         */
        ਨਹੀਂਤਾਂ ਜੇ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'L')
        {
            correctedLocale[LocaleLen - 2] = 0;
        }

        /* There are separate simplified and traditional
         * locales called zh_HK_S and zh_HK_T.
         */
        ਨਹੀਂਤਾਂ ਜੇ (uprv_strncmp(correctedLocale, "zh_HK", 5) == 0)
        {
            uprv_strcpy(correctedLocale, "zh_HK");
        }

        /* A special zh_CN_GBK locale...
        */
        ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(correctedLocale, "zh_CN_GBK") == 0)
        {
            uprv_strcpy(correctedLocale, "zh_CN");
        }

    }

    ਵਾਪਸ correctedLocale;
#ਜੇ_ਅੰਤ

}

#ਜੇ !U_CHARSET_IS_UTF8
#ਜੇ U_POSIX_LOCALE
/*
Due to various platform differences, one platform may specify a charset,
when they really mean a different charset. Remap the names so that they are
compatible with ICU. Only conflicting/ambiguous aliases should be resolved
here. Before adding anything to this function, please consider adding unique
names to the ICU alias table in the data directory.
*/
ਸਥਿਰ ਅਚਲ ਅੱਖਰ*
remapPlatformDependentCodepage(ਅਚਲ ਅੱਖਰ *locale, ਅਚਲ ਅੱਖਰ *name) {
    ਜੇ (locale != ਖਾਲੀ_ਸੂਚਕ && *locale == 0) {
        /* Make sure that an empty locale is handled the same way. */
        locale = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ (name == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
#ਜੇ U_PLATFORM == U_PF_AIX
    ਜੇ (uprv_strcmp(name, "IBM-943") == 0) {
        /* Use the ASCII compatible ibm-943 */
        name = "Shift-JIS";
    }
    ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(name, "IBM-1252") == 0) {
        /* Use the windows-1252 that contains the Euro */
        name = "IBM-5348";
    }
#ਜਾਂਜੇ U_PLATFORM == U_PF_SOLARIS
    ਜੇ (locale != ਖਾਲੀ_ਸੂਚਕ && uprv_strcmp(name, "EUC") == 0) {
        /* Solaris underspecifies the "EUC" name. */
        ਜੇ (uprv_strcmp(locale, "zh_CN") == 0) {
            name = "EUC-CN";
        }
        ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(locale, "zh_TW") == 0) {
            name = "EUC-TW";
        }
        ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(locale, "ko_KR") == 0) {
            name = "EUC-KR";
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-954 is the best match.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(name, "646") == 0) {
        /*
         * The default codepage given by Solaris is 646 but the C library routines treat it as if it was
         * ISO-8859-1 instead of US-ASCII(646).
         */
        name = "ISO-8859-1";
    }
#ਜਾਂਜੇ U_PLATFORM_IS_DARWIN_BASED
    ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ && *name == 0) {
        /*
        No locale was specified, and an empty name was passed in.
        This usually indicates that nl_langinfo didn't return valid information.
        Mac OS X uses UTF-8 by default (especially the locale data and console).
        */
        name = "UTF-8";
    }
    ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
    ਨਹੀਂਤਾਂ ਜੇ (locale != ਖਾਲੀ_ਸੂਚਕ && uprv_strcmp(locale, "en_US_POSIX") != 0 && uprv_strcmp(name, "US-ASCII") == 0) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
#ਜਾਂਜੇ U_PLATFORM == U_PF_BSD
    ਜੇ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
#ਜਾਂਜੇ U_PLATFORM == U_PF_HPUX
    ਜੇ (locale != ਖਾਲੀ_ਸੂਚਕ && uprv_strcmp(locale, "zh_HK") == 0 && uprv_strcmp(name, "big5") == 0) {
        /* HP decided to extend big5 as hkbig5 even though it's not compatible :-( */
        /* zh_TW.big5 is not the same charset as zh_HK.big5! */
        name = "hkbig5";
    }
    ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
#ਜਾਂਜੇ U_PLATFORM == U_PF_LINUX
    ਜੇ (locale != ਖਾਲੀ_ਸੂਚਕ && uprv_strcmp(name, "euc") == 0) {
        /* Linux underspecifies the "EUC" name. */
        ਜੇ (uprv_strcmp(locale, "korean") == 0) {
            name = "EUC-KR";
        }
        ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(locale, "japanese") == 0) {
            /* See comment below about eucJP */
            name = "eucjis";
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(name, "eucjp") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    ਨਹੀਂਤਾਂ ਜੇ (locale != ਖਾਲੀ_ਸੂਚਕ && uprv_strcmp(locale, "en_US_POSIX") != 0 &&
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
#ਜੇ_ਅੰਤ
    /* return nullptr when "" is passed in */
    ਜੇ (*name == 0) {
        name = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ name;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ*
getCodepageFromPOSIXID(ਅਚਲ ਅੱਖਰ *localeName, ਅੱਖਰ * ਬਫਰ, int32_t buff_ਸਮਰੱਥਾ)
{
    ਅੱਖਰ localeBuf[100];
    ਅਚਲ ਅੱਖਰ *name = ਖਾਲੀ_ਸੂਚਕ;
    ਅੱਖਰ *variant = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (localeName != ਖਾਲੀ_ਸੂਚਕ && (name = (uprv_strchr(localeName, '.'))) != ਖਾਲੀ_ਸੂਚਕ) {
        size_t locale_ਸਮਰੱਥਾ = uprv_min(ਆਕਾਰ(localeBuf), (name-localeName)+1);
        uprv_strncpy(localeBuf, localeName, locale_ਸਮਰੱਥਾ);
        localeBuf[locale_ਸਮਰੱਥਾ-1] = 0; /* ensure NUL termination */
        name = uprv_strncpy(ਬਫਰ, name+1, buff_ਸਮਰੱਥਾ);
        ਬਫਰ[buff_ਸਮਰੱਥਾ-1] = 0; /* ensure NUL termination */
        ਜੇ ((variant = const_cast<ਅੱਖਰ *>(uprv_strchr(name, '@'))) != ਖਾਲੀ_ਸੂਚਕ) {
            *variant = 0;
        }
        name = remapPlatformDependentCodepage(localeBuf, name);
    }
    ਵਾਪਸ name;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ ਅੱਖਰ*
int_getDefaultCodepage()
{
#ਜੇ U_PLATFORM == U_PF_OS400
    uint32_t ccsid = 37; /* Default to ibm-37 */
    ਸਥਿਰ ਅੱਖਰ codepage[64];
    Qwc_JOBI0400_t jobinfo;
    Qus_EC_t error = { ਆਕਾਰ(Qus_EC_t) }; /* SPI error code */

    EPT_CALL(QUSRJOBI)(&jobinfo, ਆਕਾਰ(jobinfo), "JOBI0400",
        "*                         ", "                ", &error);

    ਜੇ (error.ਬਾਈਟਾਂ_ਉਪਲਬਧ == 0) {
        ਜੇ (jobinfo.Coded_Char_ਸੈੱਟ_ID != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Coded_Char_ਸੈੱਟ_ID;
        }
        ਨਹੀਂਤਾਂ ਜੇ (jobinfo.Default_Coded_Char_ਸੈੱਟ_Id != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Default_Coded_Char_ਸੈੱਟ_Id;
        }
        /* else use the default */
    }
    snprintf(codepage, ਆਕਾਰ(codepage), "ibm-%d", ccsid);
    ਵਾਪਸ codepage;

#ਜਾਂਜੇ U_PLATFORM == U_PF_OS390
    ਸਥਿਰ ਅੱਖਰ codepage[64];

    strncpy(codepage, nl_langinfo(CODESET),63-ਲੜੀ_ਲੰਬਾਈ(UCNV_ਅਦਲਾ_ਬਦਲੀ_LFNL_OPTION_STRING));
    strcat(codepage,UCNV_ਅਦਲਾ_ਬਦਲੀ_LFNL_OPTION_STRING);
    codepage[63] = 0; /* NUL terminate */

    ਵਾਪਸ codepage;

#ਜਾਂਜੇ U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API
    ਸਥਿਰ ਅੱਖਰ codepage[64];
    DWORD codepage_ਸੰਖਿਆ = 0;

#ਜੇ U_PLATFORM_HAS_WINUWP_API == 1
    // UWP doesn't have a direct API to get the default ACP as Microsoft would rather
    // have folks use Unicode than a "system" code page, however this is the same
    // codepage as the system default locale codepage.  (FWIW, the system locale is
    // ONLY used for codepage, it should never be used for anything else)
    GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_ਸੰਖਿਆ,
        (LPWSTR)&codepage_ਸੰਖਿਆ, ਆਕਾਰ(codepage_ਸੰਖਿਆ) / ਆਕਾਰ(WCHAR));
#ਨਹੀਂਤਾਂ
    // Win32 apps can call GetACP
    codepage_ਸੰਖਿਆ = GetACP();
#ਜੇ_ਅੰਤ
    // Special case for UTF-8
    ਜੇ (codepage_ਸੰਖਿਆ == 65001)
    {
        ਵਾਪਸ "UTF-8";
    }
    // Windows codepages can look like windows-1252, so format the found number
    // the numbers are eclectic, however all valid system code pages, besides UTF-8
    // are between 3 and 19999
    ਜੇ (codepage_ਸੰਖਿਆ > 0 && codepage_ਸੰਖਿਆ < 20000)
    {
        snprintf(codepage, ਆਕਾਰ(codepage), "windows-%ld", codepage_ਸੰਖਿਆ);
        ਵਾਪਸ codepage;
    }
    // If the codepage number call failed then return UTF-8
    ਵਾਪਸ "UTF-8";

#ਜਾਂਜੇ U_POSIX_LOCALE
    ਸਥਿਰ ਅੱਖਰ codesetName[100];
    ਅਚਲ ਅੱਖਰ *localeName = ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ ਅੱਖਰ *name = ਖਾਲੀ_ਸੂਚਕ;

    localeName = uprv_getPOSIXIDForDefaultCodepage();
    uprv_memset(codesetName, 0, ਆਕਾਰ(codesetName));
    /* On Solaris nl_langinfo returns C locale values unless setlocale
     * was called earlier.
     */
#ਜੇ (U_HAVE_NL_LANGINFO_CODESET && U_PLATFORM != U_PF_SOLARIS)
    /* When available, check nl_langinfo first because it usually gives more
       useful names. It depends on LC_CTYPE.
       nl_langinfo may use the same buffer as setlocale. */
    {
        ਅਚਲ ਅੱਖਰ *codeset = nl_langinfo(U_NL_LANGINFO_CODESET);
#ਜੇ U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED
        /*
         * On Linux and MacOSX, ensure that default codepage for non C/POSIX locale is UTF-8
         * instead of ASCII.
         */
        ਜੇ (uprv_strcmp(localeName, "en_US_POSIX") != 0) {
            codeset = remapPlatformDependentCodepage(localeName, codeset);
        } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
        {
            codeset = remapPlatformDependentCodepage(ਖਾਲੀ_ਸੂਚਕ, codeset);
        }

        ਜੇ (codeset != ਖਾਲੀ_ਸੂਚਕ) {
            uprv_strncpy(codesetName, codeset, ਆਕਾਰ(codesetName));
            codesetName[ਆਕਾਰ(codesetName)-1] = 0;
            ਵਾਪਸ codesetName;
        }
    }
#ਜੇ_ਅੰਤ

    /* Use setlocale in a nice way, and then check some environment variables.
       Maybe the application used setlocale already.
    */
    uprv_memset(codesetName, 0, ਆਕਾਰ(codesetName));
    name = getCodepageFromPOSIXID(localeName, codesetName, ਆਕਾਰ(codesetName));
    ਜੇ (name) {
        /* if we can find the codeset name from setlocale, return that. */
        ਵਾਪਸ name;
    }

    ਜੇ (*codesetName == 0)
    {
        /* Everything failed. Return US ASCII (ISO 646). */
        (ਖਾਲੀ)uprv_strcpy(codesetName, "US-ASCII");
    }
    ਵਾਪਸ codesetName;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ "US-ASCII";
#ਜੇ_ਅੰਤ
}


U_CAPI ਅਚਲ ਅੱਖਰ*  U_EXPORT2
uprv_getDefaultCodepage()
{
    ਸਥਿਰ ਅੱਖਰ ਅਚਲ  *name = ਖਾਲੀ_ਸੂਚਕ;
    umtx_ਤਾਲਾ(ਖਾਲੀ_ਸੂਚਕ);
    ਜੇ (name == ਖਾਲੀ_ਸੂਚਕ) {
        name = int_getDefaultCodepage();
    }
    umtx_unlock(ਖਾਲੀ_ਸੂਚਕ);
    ਵਾਪਸ name;
}
#ਜੇ_ਅੰਤ  /* !U_CHARSET_IS_UTF8 */


/* end of platform-specific implementation -------------- */

/* version handling --------------------------------------------------------- */

U_CAPI ਖਾਲੀ U_EXPORT2
u_ਸੰਸਕਰਣ_From_String(UVersionInfo ਸੰਸਕਰਣ_Array, ਅਚਲ ਅੱਖਰ *ਸੰਸਕਰਣ_String) {
    ਅੱਖਰ *ਸਿਰਾ;
    uint16_t part=0;

    ਜੇ(ਸੰਸਕਰਣ_Array==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    ਜੇ(ਸੰਸਕਰਣ_String!=ਖਾਲੀ_ਸੂਚਕ) {
        ਲਈ(;;) {
            ਸੰਸਕਰਣ_Array[part]=(uint8_t)uprv_strtoul(ਸੰਸਕਰਣ_String, &ਸਿਰਾ, 10);
            ਜੇ(ਸਿਰਾ==ਸੰਸਕਰਣ_String || ++part==U_MAX_ਸੰਸਕਰਣ_ਲੰਮਾਈ || *ਸਿਰਾ!=U_ਸੰਸਕਰਣ_DELIMITER) {
                ਰੋਕੋ;
            }
            ਸੰਸਕਰਣ_String=ਸਿਰਾ+1;
        }
    }

    ਜਦੋਂ(part<U_MAX_ਸੰਸਕਰਣ_ਲੰਮਾਈ) {
        ਸੰਸਕਰਣ_Array[part++]=0;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
u_ਸੰਸਕਰਣ_From_UString(UVersionInfo ਸੰਸਕਰਣ_Array, ਅਚਲ char16_t *ਸੰਸਕਰਣ_String) {
    ਜੇ(ਸੰਸਕਰਣ_Array!=ਖਾਲੀ_ਸੂਚਕ && ਸੰਸਕਰਣ_String!=ਖਾਲੀ_ਸੂਚਕ) {
        ਅੱਖਰ ਸੰਸਕਰਣ_Chars[U_MAX_ਸੰਸਕਰਣ_STRING_ਲੰਮਾਈ+1];
        int32_t len = u_strlen(ਸੰਸਕਰਣ_String);
        ਜੇ(len>U_MAX_ਸੰਸਕਰਣ_STRING_ਲੰਮਾਈ) {
            len = U_MAX_ਸੰਸਕਰਣ_STRING_ਲੰਮਾਈ;
        }
        u_UChars_ਮੰਜ਼ਿਲ_Chars(ਸੰਸਕਰਣ_String, ਸੰਸਕਰਣ_Chars, len);
        ਸੰਸਕਰਣ_Chars[len]=0;
        u_ਸੰਸਕਰਣ_From_String(ਸੰਸਕਰਣ_Array, ਸੰਸਕਰਣ_Chars);
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
u_ਸੰਸਕਰਣ_ਮੰਜ਼ਿਲ_String(ਅਚਲ UVersionInfo ਸੰਸਕਰਣ_Array, ਅੱਖਰ *ਸੰਸਕਰਣ_String) {
    uint16_t count, part;
    uint8_t ਖੇਤਰ_2;

    ਜੇ(ਸੰਸਕਰਣ_String==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ;
    }

    ਜੇ(ਸੰਸਕਰਣ_Array==ਖਾਲੀ_ਸੂਚਕ) {
        ਸੰਸਕਰਣ_String[0]=0;
        ਵਾਪਸ;
    }

    /* count how many fields need to be written */
    ਲਈ(count=4; count>0 && ਸੰਸਕਰਣ_Array[count-1]==0; --count) {
    }

    ਜੇ(count <= 1) {
        count = 2;
    }

    /* write the first part */
    /* write the decimal field value */
    ਖੇਤਰ_2=ਸੰਸਕਰਣ_Array[0];
    ਜੇ(ਖੇਤਰ_2>=100) {
        *ਸੰਸਕਰਣ_String++=(ਅੱਖਰ)('0'+ਖੇਤਰ_2/100);
        ਖੇਤਰ_2%=100;
    }
    ਜੇ(ਖੇਤਰ_2>=10) {
        *ਸੰਸਕਰਣ_String++=(ਅੱਖਰ)('0'+ਖੇਤਰ_2/10);
        ਖੇਤਰ_2%=10;
    }
    *ਸੰਸਕਰਣ_String++=(ਅੱਖਰ)('0'+ਖੇਤਰ_2);

    /* write the following parts */
    ਲਈ(part=1; part<count; ++part) {
        /* write a dot first */
        *ਸੰਸਕਰਣ_String++=U_ਸੰਸਕਰਣ_DELIMITER;

        /* write the decimal field value */
        ਖੇਤਰ_2=ਸੰਸਕਰਣ_Array[part];
        ਜੇ(ਖੇਤਰ_2>=100) {
            *ਸੰਸਕਰਣ_String++=(ਅੱਖਰ)('0'+ਖੇਤਰ_2/100);
            ਖੇਤਰ_2%=100;
        }
        ਜੇ(ਖੇਤਰ_2>=10) {
            *ਸੰਸਕਰਣ_String++=(ਅੱਖਰ)('0'+ਖੇਤਰ_2/10);
            ਖੇਤਰ_2%=10;
        }
        *ਸੰਸਕਰਣ_String++=(ਅੱਖਰ)('0'+ਖੇਤਰ_2);
    }

    /* NUL-terminate */
    *ਸੰਸਕਰਣ_String=0;
}

U_CAPI ਖਾਲੀ U_EXPORT2
u_get_ਸੰਸਕਰਣ(UVersionInfo ਸੰਸਕਰਣ_Array) {
    (ਖਾਲੀ)copyright;   // Suppress unused variable warning from clang.
    u_ਸੰਸਕਰਣ_From_String(ਸੰਸਕਰਣ_Array, U_ICU_ਸੰਸਕਰਣ);
}

/**
 * icucfg.h dependent code
 */

#ਜੇ U_ENABLE_DYLOAD && HAVE_DLOPEN && !U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API

#ਜੇ HAVE_DLFCN_H
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __MVS__
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __SUSV3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __SUSV3 1
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <dlfcn.h>
#ਜੇ_ਅੰਤ /* HAVE_DLFCN_H */

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_dl_open(ਅਚਲ ਅੱਖਰ *libName, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਖਾਲੀ *ret = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ ret;
  ret =  dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
  ਜੇ(ret==ਖਾਲੀ_ਸੂਚਕ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_TRACE_DYLOAD
    ਲਿਖੋ("dlerror on dlopen(%s): %s\n", libName, dlerror());
#ਜੇ_ਅੰਤ
    *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
  }
  ਵਾਪਸ ret;
}

U_CAPI ਖਾਲੀ U_EXPORT2
uprv_dl_close(ਖਾਲੀ *lib, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ;
  dlclose(lib);
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(ਖਾਲੀ *lib, ਅਚਲ ਅੱਖਰ* sym, UError_ਕੋਡ *ਸਥਿਤੀ) {
  ਸੰਘ {
      UVoidFunction *fp;
      ਖਾਲੀ *vp;
  } uret;
  uret.fp = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ uret.fp;
  uret.vp = dlsym(lib, sym);
  ਜੇ(uret.vp == ਖਾਲੀ_ਸੂਚਕ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_TRACE_DYLOAD
    ਲਿਖੋ("dlerror on dlsym(%p,%s): %s\n", lib,sym, dlerror());
#ਜੇ_ਅੰਤ
    *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
  }
  ਵਾਪਸ uret.fp;
}

#ਜਾਂਜੇ U_ENABLE_DYLOAD && U_PLATFORM_ਵਰਤਦਾ_ONLY_WIN32_API && !U_PLATFORM_HAS_WINUWP_API

/* Windows API implementation. */
// Note: UWP does not expose/allow these APIs, so the UWP version gets the null implementation. */

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_dl_open(ਅਚਲ ਅੱਖਰ *libName, UError_ਕੋਡ *ਸਥਿਤੀ) {
  HMODULE lib = ਖਾਲੀ_ਸੂਚਕ;

  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

  lib = ਲੱਦੋ_Library_A(libName);

  ਜੇ(lib==ਖਾਲੀ_ਸੂਚਕ) {
    *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
  }

  ਵਾਪਸ (ਖਾਲੀ*)lib;
}

U_CAPI ਖਾਲੀ U_EXPORT2
uprv_dl_close(ਖਾਲੀ *lib, UError_ਕੋਡ *ਸਥਿਤੀ) {
  HMODULE handle = (HMODULE)lib;
  ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ;

  FreeLibrary(handle);

  ਵਾਪਸ;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(ਖਾਲੀ *lib, ਅਚਲ ਅੱਖਰ* sym, UError_ਕੋਡ *ਸਥਿਤੀ) {
  HMODULE handle = (HMODULE)lib;
  UVoidFunction* addr = ਖਾਲੀ_ਸੂਚਕ;

  ਜੇ(U_FAILURE(*ਸਥਿਤੀ) || lib==ਖਾਲੀ_ਸੂਚਕ) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;

  addr = (UVoidFunction*)Get_Proc_ਪਤਾ(handle, sym);

  ਜੇ(addr==ਖਾਲੀ_ਸੂਚਕ) {
    DWORD ਆਖਰੀ_Error = Get_ਆਖਰੀ_Error();
    ਜੇ(ਆਖਰੀ_Error == ERROR_PROC_NOT_ਮਿਲਿਆ) {
      *ਸਥਿਤੀ = U_MISSING_RESOURCE_ERROR;
    } ਨਹੀਂਤਾਂ {
      *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR; /* other unknown error. */
    }
  }

  ਵਾਪਸ addr;
}

#ਨਹੀਂਤਾਂ

/* No dynamic loading, null (nonexistent) implementation. */

U_CAPI ਖਾਲੀ * U_EXPORT2
uprv_dl_open(ਅਚਲ ਅੱਖਰ *libName, UError_ਕੋਡ *ਸਥਿਤੀ) {
    (ਖਾਲੀ)libName;
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

U_CAPI ਖਾਲੀ U_EXPORT2
uprv_dl_close(ਖਾਲੀ *lib, UError_ਕੋਡ *ਸਥਿਤੀ) {
    (ਖਾਲੀ)lib;
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)) ਵਾਪਸ;
    *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
    ਵਾਪਸ;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(ਖਾਲੀ *lib, ਅਚਲ ਅੱਖਰ* sym, UError_ਕੋਡ *ਸਥਿਤੀ) {
  (ਖਾਲੀ)lib;
  (ਖਾਲੀ)sym;
  ਜੇ(U_SUCCESS(*ਸਥਿਤੀ)) {
    *ਸਥਿਤੀ = U_UNSUPPORTED_ERROR;
  }
  ਵਾਪਸ (UVoidFunction*)ਖਾਲੀ_ਸੂਚਕ;
}

#ਜੇ_ਅੰਤ

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

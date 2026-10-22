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
#അന്തര്ഭാവ "uposixdefs.h"

// First, the platform type. Need this for U_PLATFORM.
#അന്തര്ഭാവ "unicode/platform.h"

/*
 * Cygwin with GCC requires inclusion of time.h after the above disabling strict asci mode statement.
 */
#അന്തര്ഭാവ <time.h>

#എങ്കിൽ !U_PLATFORM_USES_ONLY_WIN32_API
#അന്തര്ഭാവ <sys/time.h>
#endif

/* include the rest of the ICU headers */
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "locmap.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "charstr.h"

/* Include standard headers. */
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <math.h>
#അന്തര്ഭാവ <locale.h>
#അന്തര്ഭാവ <float.h>

#ifndef U_COMMON_IMPLEMENTATION
#error U_COMMON_IMPLEMENTATION not set - must be set വേണ്ടി all ICU source files in common/ - see https://unicode-org.github.io/icu/userguide/icu/howtouseicu.html
#endif


/* include system headers */
#എങ്കിൽ U_PLATFORM_USES_ONLY_WIN32_API
    /*
     * TODO: U_PLATFORM_USES_ONLY_WIN32_API includes MinGW.
     * Should Cygwin be included as well (U_PLATFORM_HAS_WIN32_API)
     * to use native APIs as much as possible?
     */
#ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#endif
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>
#   include "unicode/uloc.h"
#   include "wintz.h"
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_OS400
#   include <float.h>
#   include <qusec.h>       /* error code structure */
#   include <qusrjobi.h>
#   include <qliept.h>      /* EPT_CALL macro  - this include must be after all other "QSYSINCs" */
#   include <mih/testptr.h> /* For uprv_maximumPtr */
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_OS390
#   include "unicode/ucnv.h"   /* Needed for UCNV_SWAP_LFNL_OPTION_STRING */
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS
#   include <limits.h>
#   include <unistd.h>
#   എങ്കിൽ U_PLATFORM == U_PF_SOLARIS
#       ifndef _XPG4_2
#           define _XPG4_2
#       endif
#   elif U_PLATFORM == U_PF_ANDROID
#       include <sys/system_properties.h>
#       include <dlfcn.h>
#   endif
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_QNX
#   include <sys/neutrino.h>
#endif


/*
 * Only include langinfo.h if we have a way to get the codeset. If we later
 * depend on more feature, we can test on U_HAVE_NL_LANGINFO.
 *
 */

#എങ്കിൽ U_HAVE_NL_LANGINFO_CODESET
#അന്തര്ഭാവ <langinfo.h>
#endif

/**
 * Simple things (presence of functions, etc) should just go in configure.in and be added to
 * icucfg.h via autoheader.
 */
#എങ്കിൽ U_PLATFORM_IMPLEMENTS_POSIX
#   എങ്കിൽ U_PLATFORM == U_PF_OS400
#    define HAVE_DLFCN_H 0
#    define HAVE_DLOPEN 0
#   അല്ലെങ്കിൽ
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
#അല്ലെങ്കിൽ
#   define HAVE_DLFCN_H 0
#   define HAVE_DLOPEN 0
#   define HAVE_GETTIMEOFDAY 0
#endif

U_NAMESPACE_USE

/* Define the extension for data files, again... */
#define DATA_TYPE "dat"

/* Leave this copyright notice here! */
സ്ഥിതം സ്ഥിരം അക്ഷരം copyright[] = U_COPYRIGHT_STRING;

/* floating point implementations ------------------------------------------- */

/* We return QNAN rather than SNAN*/
#define SIGN 0x80000000U

/* Make it easy to define certain types of constants */
തരനിർവചനം സംഘം {
    int64_t i64; /* This must be defined first in order to allow the initialization to work. This is a C89 feature. */
    ഇരട്ട d64;
} BitPatternConversion;
സ്ഥിതം സ്ഥിരം BitPatternConversion gNan = {static_cast<int64_t>(INT64_C(0x7FF8000000000000))};
സ്ഥിതം സ്ഥിരം BitPatternConversion gInf = {static_cast<int64_t>(INT64_C(0x7FF0000000000000))};

/*---------------------------------------------------------------------------
  Platform utilities
  Our general strategy is to assume we're on a POSIX platform.  Platforms which
  are non-POSIX must declare themselves so.  The default POSIX implementation
  will sometimes work for non-POSIX platforms as well (e.g., the NaN-related
  functions).
  ---------------------------------------------------------------------------*/

#എങ്കിൽ U_PLATFORM_USES_ONLY_WIN32_API || U_PLATFORM == U_PF_OS400
#   undef U_POSIX_LOCALE
#അല്ലെങ്കിൽ
#   define U_POSIX_LOCALE    1
#endif

/*
    WARNING! u_topNBytesOfDouble and u_bottomNBytesOfDouble
    can't be properly optimized by the gcc compiler sometimes (i.e. gcc 3.2).
*/
#എങ്കിൽ !IEEE_754
സ്ഥിതം അക്ഷരം*
u_മുകൾ_NBytes_Of_Double(ഇരട്ട* d, പൂർണ്ണസംഖ്യ n)
{
#എങ്കിൽ U_IS_BIG_ENDIAN
    തിരികെ (അക്ഷരം*)d;
#അല്ലെങ്കിൽ
    തിരികെ (അക്ഷരം*)(d + 1) - n;
#endif
}

സ്ഥിതം അക്ഷരം*
u_bottomNBytesOfDouble(ഇരട്ട* d, പൂർണ്ണസംഖ്യ n)
{
#എങ്കിൽ U_IS_BIG_ENDIAN
    തിരികെ (അക്ഷരം*)(d + 1) - n;
#അല്ലെങ്കിൽ
    തിരികെ (അക്ഷരം*)d;
#endif
}
#endif   /* !IEEE_754 */

#എങ്കിൽ IEEE_754
സ്ഥിതം UBool
u_signBit(ഇരട്ട d) {
    uint8_t hiByte;
#എങ്കിൽ U_IS_BIG_ENDIAN
    hiByte = *(uint8_t *)&d;
#അല്ലെങ്കിൽ
    hiByte = *(reinterpret_cast<uint8_t*>(&d) + വലുപ്പം(ഇരട്ട) - 1);
#endif
    തിരികെ (hiByte & 0x80) != 0;
}
#endif



#എങ്കിൽ defined (U_DEBUG_FAKETIME)
/* Override the clock to test things without having to move the system clock.
 * Assumes POSIX gettimeofday() will function
 */
UDate fakeClock_t0 = 0; /** Time to start the clock from **/
UDate fakeClock_dt = 0; /** Offset (fake time - real time) **/
UBool fakeClock_set = അസത്യം; /** True if fake clock has spun up **/

സ്ഥിതം UDate getUTCtime_real() {
    ഘടന timeval posixTime;
    gettimeofday(&posixTime, nullptr);
    തിരികെ (UDate)(((int64_t)posixTime.tv_sec * U_MILLIS_PER_SECOND) + (posixTime.tv_usec/1000));
}

സ്ഥിതം UDate getUTCtime_fake() {
    സ്ഥിതം UMutex fakeClockMutex;
    umtx_lock(&fakeClockMutex);
    എങ്കിൽ(!fakeClock_set) {
        UDate real = getUTCtime_real();
        സ്ഥിരം അക്ഷരം *fake_start = getenv("U_FAKETIME_START");
        എങ്കിൽ((fake_start!=nullptr) && (fake_start[0]!=0)) {
            sscanf(fake_start,"%lf",&fakeClock_t0);
            fakeClock_dt = fakeClock_t0 - real;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, so the ICU clock will start at a preset value\n"
                    "env variable U_FAKETIME_START=%.0f (%s) for an offset of %.0f ms from the current time %.0f\n",
                    fakeClock_t0, fake_start, fakeClock_dt, real);
        } അല്ലെങ്കിൽ {
          fakeClock_dt = 0;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, but U_FAKETIME_START was not set.\n"
                    "Set U_FAKETIME_START to the number of milliseconds since 1/1/1970 to set the ICU clock.\n");
        }
        fakeClock_set = സത്യം;
    }
    umtx_unlock(&fakeClockMutex);

    തിരികെ getUTCtime_real() + fakeClock_dt;
}
#endif

#എങ്കിൽ U_PLATFORM_USES_ONLY_WIN32_API
തരനിർവചനം സംഘം {
    int64_t int64;
    FILETIME fileTime;
} FileTimeConversion;   /* This is like a ULARGE_INTEGER */

/* Number of 100 nanoseconds from 1/1/1601 to 1/1/1970 */
#define EPOCH_BIAS  INT64_C(116444736000000000)
#define HECTONANOSECOND_PER_MILLISECOND   10000

#endif

/*---------------------------------------------------------------------------
  Universal Implementations
  These are designed to work on all platforms.  Try these, and if they
  don't work on your platform, then special case your platform with new
  implementations.
---------------------------------------------------------------------------*/

U_CAPI UDate U_EXPORT2
uprv_getUTCtime()
{
#എങ്കിൽ defined(U_DEBUG_FAKETIME)
    തിരികെ getUTCtime_fake(); /* Hook for overriding the clock */
#അല്ലെങ്കിൽ
    തിരികെ uprv_getRawUTCtime();
#endif
}

/* Return UTC (GMT) time measured in milliseconds since 0:00 on 1/1/70.*/
U_CAPI UDate U_EXPORT2
uprv_getRawUTCtime()
{
#എങ്കിൽ U_PLATFORM_USES_ONLY_WIN32_API

    FileTimeConversion winTime;
    GetSystemTimeAsFileTime(&winTime.fileTime);
    തിരികെ (UDate)((winTime.int64 - EPOCH_BIAS) / HECTONANOSECOND_PER_MILLISECOND);
#അല്ലെങ്കിൽ

#എങ്കിൽ HAVE_GETTIMEOFDAY
    ഘടന timeval posixTime;
    gettimeofday(&posixTime, nullptr);
    തിരികെ (UDate)(((int64_t)posixTime.tv_sec * U_MILLIS_PER_SECOND) + (posixTime.tv_usec/1000));
#അല്ലെങ്കിൽ
    time_t epochtime;
    time(&epochtime);
    തിരികെ (UDate)epochtime * U_MILLIS_PER_SECOND;
#endif

#endif
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
uprv_isNaN(ഇരട്ട number)
{
#എങ്കിൽ IEEE_754
    BitPatternConversion converted_സംഖ്യ;
    converted_സംഖ്യ.d64 = number;
    /* Infinity is 0x7FF0000000000000U. Anything greater than that is a NaN */
    തിരികെ (converted_സംഖ്യ.i64 & U_INT64_MAX) > gInf.i64;

#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_OS390
    uint32_t മുകളിലെ_Bits = *(uint32_t*)u_മുകൾ_NBytes_Of_Double(&number,
                        വലുപ്പം(uint32_t));
    uint32_t താഴെയുള്ള_Bits  = *(uint32_t*)u_bottomNBytesOfDouble(&number,
                        വലുപ്പം(uint32_t));

    തിരികെ ((മുകളിലെ_Bits & 0x7F080000L) == 0x7F080000L) &&
      (താഴെയുള്ള_Bits == 0x00000000L);

#അല്ലെങ്കിൽ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    തിരികെ number != number;
#endif
}

U_CAPI UBool U_EXPORT2
uprv_isInfinite(ഇരട്ട number)
{
#എങ്കിൽ IEEE_754
    BitPatternConversion converted_സംഖ്യ;
    converted_സംഖ്യ.d64 = number;
    /* Infinity is exactly 0x7FF0000000000000U. */
    തിരികെ (converted_സംഖ്യ.i64 & U_INT64_MAX) == gInf.i64;
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_OS390
    uint32_t മുകളിലെ_Bits = *(uint32_t*)u_മുകൾ_NBytes_Of_Double(&number,
                        വലുപ്പം(uint32_t));
    uint32_t താഴെയുള്ള_Bits  = *(uint32_t*)u_bottomNBytesOfDouble(&number,
                        വലുപ്പം(uint32_t));

    തിരികെ ((മുകളിലെ_Bits  & ~SIGN) == 0x70FF0000L) && (താഴെയുള്ള_Bits == 0x00000000L);

#അല്ലെങ്കിൽ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    തിരികെ number == (2.0 * number);
#endif
}

U_CAPI UBool U_EXPORT2
uprv_isPositiveInfinity(ഇരട്ട number)
{
#എങ്കിൽ IEEE_754 || U_PLATFORM == U_PF_OS390
    തിരികെ number > 0 && uprv_isInfinite(number);
#അല്ലെങ്കിൽ
    തിരികെ uprv_isInfinite(number);
#endif
}

U_CAPI UBool U_EXPORT2
uprv_isNegativeInfinity(ഇരട്ട number)
{
#എങ്കിൽ IEEE_754 || U_PLATFORM == U_PF_OS390
    തിരികെ number < 0 && uprv_isInfinite(number);

#അല്ലെങ്കിൽ
    uint32_t മുകളിലെ_Bits = *(uint32_t*)u_മുകൾ_NBytes_Of_Double(&number,
                        വലുപ്പം(uint32_t));
    തിരികെ((മുകളിലെ_Bits & SIGN) && uprv_isInfinite(number));

#endif
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_getNaN()
{
#എങ്കിൽ IEEE_754 || U_PLATFORM == U_PF_OS390
    തിരികെ gNan.d64;
#അല്ലെങ്കിൽ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    തിരികെ 0.0;
#endif
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_getInfinity()
{
#എങ്കിൽ IEEE_754 || U_PLATFORM == U_PF_OS390
    തിരികെ gInf.d64;
#അല്ലെങ്കിൽ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    തിരികെ 0.0;
#endif
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_floor(ഇരട്ട x)
{
    തിരികെ floor(x);
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_ceil(ഇരട്ട x)
{
    തിരികെ ceil(x);
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_round(ഇരട്ട x)
{
    തിരികെ uprv_floor(x + 0.5);
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_fabs(ഇരട്ട x)
{
    തിരികെ fabs(x);
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_modf(ഇരട്ട x, ഇരട്ട* y)
{
    തിരികെ modf(x, y);
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_fmod(ഇരട്ട x, ഇരട്ട y)
{
    തിരികെ fmod(x, y);
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_pow(ഇരട്ട x, ഇരട്ട y)
{
    /* This is declared as "double pow(double x, double y)" */
    തിരികെ pow(x, y);
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_pow10(int32_t x)
{
    തിരികെ pow(10.0, (ഇരട്ട)x);
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_fmax(ഇരട്ട x, ഇരട്ട y)
{
#എങ്കിൽ IEEE_754
    /* first handle NaN*/
    എങ്കിൽ(uprv_isNaN(x) || uprv_isNaN(y))
        തിരികെ uprv_getNaN();

    /* check for -0 and 0*/
    എങ്കിൽ(x == 0.0 && y == 0.0 && u_signBit(x))
        തിരികെ y;

#endif

    /* this should work for all flt point w/o NaN and Inf special cases */
    തിരികെ (x > y ? x : y);
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_fmin(ഇരട്ട x, ഇരട്ട y)
{
#എങ്കിൽ IEEE_754
    /* first handle NaN*/
    എങ്കിൽ(uprv_isNaN(x) || uprv_isNaN(y))
        തിരികെ uprv_getNaN();

    /* check for -0 and 0*/
    എങ്കിൽ(x == 0.0 && y == 0.0 && u_signBit(y))
        തിരികെ y;

#endif

    /* this should work for all flt point w/o NaN and Inf special cases */
    തിരികെ (x > y ? y : x);
}

U_CAPI UBool U_EXPORT2
uprv_add32_overflow(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_add_overflow.
    // This function could be optimized by calling one of those primitives.
    auto a64 = static_cast<int64_t>(a);
    auto b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 + b64;
    *res = static_cast<int32_t>(res64);
    തിരികെ res64 != *res;
}

U_CAPI UBool U_EXPORT2
uprv_mul32_overflow(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_mul_overflow.
    // This function could be optimized by calling one of those primitives.
    auto a64 = static_cast<int64_t>(a);
    auto b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 * b64;
    *res = static_cast<int32_t>(res64);
    തിരികെ res64 != *res;
}

/**
 * Truncates the given double.
 * trunc(3.3) = 3.0, trunc (-3.3) = -3.0
 * This is different than calling floor() or ceil():
 * floor(3.3) = 3, floor(-3.3) = -4
 * ceil(3.3) = 4, ceil(-3.3) = -3
 */
U_CAPI ഇരട്ട U_EXPORT2
uprv_trunc(ഇരട്ട d)
{
#എങ്കിൽ IEEE_754
    /* handle error cases*/
    എങ്കിൽ(uprv_isNaN(d))
        തിരികെ uprv_getNaN();
    എങ്കിൽ(uprv_isInfinite(d))
        തിരികെ uprv_getInfinity();

    എങ്കിൽ(u_signBit(d))    /* Signbit() picks up -0.0;  d<0 does not. */
        തിരികെ ceil(d);
    അല്ലെങ്കിൽ
        തിരികെ floor(d);

#അല്ലെങ്കിൽ
    തിരികെ d >= 0 ? floor(d) : ceil(d);

#endif
}

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 */
U_CAPI ഇരട്ട U_EXPORT2
uprv_maxMantissa()
{
    തിരികെ pow(2.0, DBL_MANT_DIG + 1.0) - 1.0;
}

U_CAPI ഇരട്ട U_EXPORT2
uprv_log(ഇരട്ട d)
{
    തിരികെ log(d);
}

U_CAPI ശൂന്യം * U_EXPORT2
uprv_maximumPtr(ശൂന്യം * base)
{
#എങ്കിൽ U_PLATFORM == U_PF_OS400
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
    എങ്കിൽ ((base != nullptr) && (_TESTPTR(base, _C_TERASPACE_CHECK))) {
        /* if it is a TERASPACE pointer the max is 2GB - 4k */
        തിരികെ ((ശൂന്യം *)(((അക്ഷരം *)base)-((uint32_t)(base))+((uint32_t)0x7fffefff)));
    }
    /* otherwise 16MB since nullptr ptr is not checkable or the ptr is not TERASPACE */
    തിരികെ ((ശൂന്യം *)(((അക്ഷരം *)base)-((uint32_t)(base))+((uint32_t)0xffefff)));

#അല്ലെങ്കിൽ
    തിരികെ U_MAX_PTR(base);
#endif
}

/*---------------------------------------------------------------------------
  Platform-specific Implementations
  Try these, and if they don't work on your platform, then special case your
  platform with new implementations.
  ---------------------------------------------------------------------------*/

/* Generic time zone layer -------------------------------------------------- */

/* Time zone utilities */
U_CAPI ശൂന്യം U_EXPORT2
uprv_tzset()
{
#എങ്കിൽ defined(U_TZSET)
    U_TZSET();
#അല്ലെങ്കിൽ
    /* no initialization*/
#endif
}

U_CAPI int32_t U_EXPORT2
uprv_timezone()
{
#ifdef U_TIMEZONE
    തിരികെ U_TIMEZONE;
#അല്ലെങ്കിൽ
    time_t t, t1, t2;
    ഘടന tm tmrec;
    int32_t tdiff = 0;

    time(&t);
    uprv_memcpy( &tmrec, localtime(&t), വലുപ്പം(tmrec) );
#എങ്കിൽ U_PLATFORM != U_PF_IPHONE
    UBool dst_checked = (tmrec.tm_isdst != 0); /* daylight savings time is checked*/
#endif
    t1 = mktime(&tmrec);                 /* local time in seconds*/
    uprv_memcpy( &tmrec, gmtime(&t), വലുപ്പം(tmrec) );
    t2 = mktime(&tmrec);                 /* GMT (or UTC) in seconds*/
    tdiff = t2 - t1;

#എങ്കിൽ U_PLATFORM != U_PF_IPHONE
    /* imitate NT behaviour, which returns same timezone offset to GMT for
       winter and summer.
       This does not work on all platforms. For instance, on glibc on Linux
       and on Mac OS 10.5, tdiff calculated above remains the same
       regardless of whether DST is in effect or not. iOS is another
       platform where this does not work. Linux + glibc and Mac OS 10.5
       have U_TIMEZONE defined so that this code is not reached.
    */
    എങ്കിൽ (dst_checked)
        tdiff += 3600;
#endif
    തിരികെ tdiff;
#endif
}

/* Note that U_TZNAME does *not* have to be tzname, but if it is,
   some platforms need to have it declared here. */

#എങ്കിൽ defined(U_TZNAME) && (U_PLATFORM == U_PF_IRIX || U_PLATFORM_IS_DARWIN_BASED)
/* RS6000 and others reject char **tzname.  */
ബാഹ്യ U_IMPORT അക്ഷരം *U_TZNAME[];
#endif

#എങ്കിൽ !UCONFIG_NO_FILE_IO && ((U_PLATFORM_IS_DARWIN_BASED && (U_PLATFORM != U_PF_IPHONE || defined(U_TIMEZONE))) || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS)
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

#define CHECK_LOCALTIME_LINK 1
#എങ്കിൽ U_PLATFORM_IS_DARWIN_BASED
#അന്തര്ഭാവ <tzfile.h>
#define TZZONEINFO      (TZDIR "/")
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_SOLARIS
#define TZDEFAULT       "/etc/localtime"
#define TZZONEINFO      "/usr/share/lib/zoneinfo/"
#define TZ_ENV_CHECK    "localtime"
#അല്ലെങ്കിൽ
#define TZDEFAULT       "/etc/localtime"
#define TZZONEINFO      "/usr/share/zoneinfo/"
#endif
#define TZZONEINFOTAIL  "/zoneinfo/"
#എങ്കിൽ U_HAVE_DIRENT_H
#define TZFILE_SKIP     "posixrules" /* tz file to skip when searching. */
/* Some Linux distributions have 'localtime' in /usr/share/zoneinfo
   symlinked to /etc/localtime, which makes searchForTZFile return
   'localtime' when it's the first match. */
#define TZFILE_SKIP2    "localtime"
#define തിരയൽ_TZFILE
#അന്തര്ഭാവ <dirent.h>  /* Needed to search through system timezone files */
#endif
സ്ഥിതം അക്ഷരം gTimeZoneBuffer[PATH_MAX];
സ്ഥിതം സ്ഥിരം അക്ഷരം *gTimeZoneBufferPtr = nullptr;
#endif

#എങ്കിൽ !U_PLATFORM_USES_ONLY_WIN32_API
#define isNonDigit(ch) (ch < '0' || '9' < ch)
#define isDigit(ch) ('0' <= ch && ch <= '9')
സ്ഥിതം UBool isValidOlsonID(സ്ഥിരം അക്ഷരം *id) {
    int32_t idx = 0;
    int32_t idxMax = 0;

    /* Determine if this is something like Iceland (Olson ID)
    or AST4ADT (non-Olson ID) */
    ആകുമ്പോൾ (id[idx] && isNonDigit(id[idx]) && id[idx] != ',') {
        idx++;
    }

    /* Allow at maximum 2 numbers at the end of the id to support zone id's
    like GMT+11. */
    idxMax = idx + 2;
    ആകുമ്പോൾ (id[idx] && isDigit(id[idx]) && idx < idxMax) {
        idx++;
    }

    /* If we went through the whole string, then it might be okay.
    The timezone is sometimes set to "CST-7CDT", "CST6CDT5,J129,J131/19:30",
    "GRNLNDST3GRNLNDDT" or similar, so we cannot use it.
    The rest of the time it could be an Olson ID. George */
    തിരികെ id[idx] == 0
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
സ്ഥിതം ശൂന്യം skipZoneIDPrefix(സ്ഥിരം അക്ഷരം** id) {
    എങ്കിൽ (uprv_strncmp(*id, "posix/", 6) == 0
        || uprv_strncmp(*id, "right/", 6) == 0)
    {
        *id += 6;
    }
}
#endif

#എങ്കിൽ defined(U_TZNAME) && !U_PLATFORM_USES_ONLY_WIN32_API

#define CONVERT_HOURS_TO_SECONDS(offset) (int32_t)(offset*3600)
തരനിർവചനം ഘടന OffsetZoneMapping {
    int32_t offsetSeconds;
    int32_t daylightType; /* 0=U_DAYLIGHT_NONE, 1=daylight in June-U_DAYLIGHT_JUNE, 2=daylight in December=U_DAYLIGHT_DECEMBER*/
    സ്ഥിരം അക്ഷരം *stdID;
    സ്ഥിരം അക്ഷരം *dstID;
    സ്ഥിരം അക്ഷരം *olsonID;
} OffsetZoneMapping;

എണ്ണൽ { U_DAYLIGHT_NONE=0,U_DAYLIGHT_JUNE=1,U_DAYLIGHT_DECEMBER=2 };

/*
This list tries to disambiguate a set of abbreviated timezone IDs and offsets
and maps it to an Olson ID.
Before adding anything to this list, take a look at
icu/source/tools/tzcode/tz.alias
Sometimes no daylight savings (0) is important to define due to aliases.
This list can be tested with icu/source/test/compat/tzone.pl
More values could be added to daylightType to increase precision.
*/
സ്ഥിതം സ്ഥിരം ഘടന OffsetZoneMapping OFFSET_ZONE_MAPPINGS[] = {
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

സ്ഥിതം സ്ഥിരം അക്ഷരം* remapShortTimeZone(സ്ഥിരം അക്ഷരം *stdID, സ്ഥിരം അക്ഷരം *dstID, int32_t daylightType, int32_t offset)
{
    int32_t idx;
#ifdef DEBUG_TZNAME
    fprintf(stderr, "TZ=%s std=%s dst=%s daylight=%d offset=%d\n", getenv("TZ"), stdID, dstID, daylightType, offset);
#endif
    വേണ്ടി (idx = 0; idx < UPRV_LENGTHOF(OFFSET_ZONE_MAPPINGS); idx++)
    {
        എങ്കിൽ (offset == OFFSET_ZONE_MAPPINGS[idx].offsetSeconds
            && daylightType == OFFSET_ZONE_MAPPINGS[idx].daylightType
            && strcmp(OFFSET_ZONE_MAPPINGS[idx].stdID, stdID) == 0
            && strcmp(OFFSET_ZONE_MAPPINGS[idx].dstID, dstID) == 0)
        {
            തിരികെ OFFSET_ZONE_MAPPINGS[idx].olsonID;
        }
    }
    തിരികെ nullptr;
}
#endif

#ifdef തിരയൽ_TZFILE
#define MAX_READ_SIZE 512

തരനിർവചനം ഘടന DefaultTZInfo {
    അക്ഷരം* defaultTZBuffer;
    int64_t defaultTZFileSize;
    FILE* defaultTZFilePtr;
    int32_t defaultTZPosition;
} DefaultTZInfo;

/*
 * This method compares the two files given to see if they are a match.
 * It is currently use to compare two TZ files.
 */
സ്ഥിതം UBool compare_ദ്വിമാന_Files(സ്ഥിരം അക്ഷരം* TZFileName, DefaultTZInfo* tzInfo) {
    FILE* file;
    int64_t sizeFile;
    int64_t size_File_ഇടത്;
    int32_t sizeFileRead;
    int32_t sizeFileToRead;
    അക്ഷരം bufferFile[MAX_READ_SIZE];
    UBool result = സത്യം;

    U_ASSERT(tzInfo->defaultTZFilePtr != nullptr);
    file = fopen(TZFileName, "r");

    tzInfo->defaultTZPosition = 0; /* reset position to begin search */

    എങ്കിൽ (file != nullptr) {
        /* First check that the file size are equal. */
        എങ്കിൽ (tzInfo->defaultTZFileSize == 0) {
            fseek(tzInfo->defaultTZFilePtr, 0, SEEK_END);
            tzInfo->defaultTZFileSize = ftell(tzInfo->defaultTZFilePtr);
        }
        എങ്കിൽ (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            തിരികെ അസത്യം;
        }
        sizeFile = ftell(file);
        എങ്കിൽ (sizeFile == -1) {
            fclose(file);
            തിരികെ അസത്യം;
        }
        size_File_ഇടത് = sizeFile;

        എങ്കിൽ (sizeFile != tzInfo->defaultTZFileSize) {
            result = അസത്യം;
        } അല്ലെങ്കിൽ {
            /* Store the data from the files in separate buffers and
             * compare each byte to determine equality.
             */
            എങ്കിൽ (tzInfo->defaultTZBuffer == nullptr) {
                എങ്കിൽ (fseek(tzInfo->defaultTZFilePtr, 0, SEEK_SET) != 0) {
                    fclose(file);
                    തിരികെ അസത്യം;
                }
                tzInfo->defaultTZBuffer = static_cast<അക്ഷരം*>(uprv_malloc(വലുപ്പം(അക്ഷരം) * tzInfo->defaultTZFileSize));
                sizeFileRead = fread(tzInfo->defaultTZBuffer, 1, tzInfo->defaultTZFileSize, tzInfo->defaultTZFilePtr);
            }
            എങ്കിൽ (fseek(file, 0, SEEK_SET) != 0) {
                fclose(file);
                തിരികെ അസത്യം;
            }
            ആകുമ്പോൾ(size_File_ഇടത് > 0) {
                uprv_memset(bufferFile, 0, MAX_READ_SIZE);
                sizeFileToRead = size_File_ഇടത് < MAX_READ_SIZE ? size_File_ഇടത് : MAX_READ_SIZE;

                sizeFileRead = fread(bufferFile, 1, sizeFileToRead, file);
                എങ്കിൽ (sizeFileRead == -1) {
                    fclose(file);
                    തിരികെ അസത്യം;
                }
                എങ്കിൽ (memcmp(tzInfo->defaultTZBuffer + tzInfo->defaultTZPosition, bufferFile, sizeFileRead) != 0) {
                    result = അസത്യം;
                    നിർത്തുക;
                }
                size_File_ഇടത് -= sizeFileRead;
                tzInfo->defaultTZPosition += sizeFileRead;
            }
        }
    } അല്ലെങ്കിൽ {
        result = അസത്യം;
    }

    എങ്കിൽ (file != nullptr) {
        fclose(file);
    }

    തിരികെ result;
}


/* dirent also lists two entries: "." and ".." that we can safely ignore. */
#define SKIP1 "."
#define SKIP2 ".."
സ്ഥിതം UBool U_CALLCONV putil_cleanup();
സ്ഥിതം CharString *g_തിരയൽ_TZFile_Result = nullptr;

/*
 * This method recursively traverses the directory given for a matching TZ file and returns the first match.
 * This function is not thread safe - it uses a global, gSearchTZFileResult, to hold its results.
 */
സ്ഥിതം അക്ഷരം* തിരയൽ_For_TZFile(സ്ഥിരം അക്ഷരം* path, DefaultTZInfo* tzInfo) {
    DIR* dirp = nullptr;
    ഘടന dirent* dirEntry = nullptr;
    അക്ഷരം* result = nullptr;
    UErrorCode status = U_ZERO_ERROR;

    /* There is no reference file to compare against, so do not traverse the zoneinfo tree. */
    എങ്കിൽ (tzInfo->defaultTZFilePtr == nullptr) {
        തിരികെ nullptr;
    }

    /* Save the current path */
    CharString curpath(path, -1, status);
    എങ്കിൽ (U_FAILURE(status)) {
        പോകുക cleanupAndReturn;
    }

    dirp = opendir(path);
    എങ്കിൽ (dirp == nullptr) {
        പോകുക cleanupAndReturn;
    }

    എങ്കിൽ (g_തിരയൽ_TZFile_Result == nullptr) {
        g_തിരയൽ_TZFile_Result = പുതിയ CharString;
        എങ്കിൽ (g_തിരയൽ_TZFile_Result == nullptr) {
            പോകുക cleanupAndReturn;
        }
        ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    }

    /* Check each entry in the directory. */
    ആകുമ്പോൾ((dirEntry = readdir(dirp)) != nullptr) {
        സ്ഥിരം അക്ഷരം* dirName = dirEntry->d_name;
        എങ്കിൽ (uprv_strcmp(dirName, SKIP1) != 0 && uprv_strcmp(dirName, SKIP2) != 0
            && uprv_strcmp(TZFILE_SKIP, dirName) != 0 && uprv_strcmp(TZFILE_SKIP2, dirName) != 0) {
            /* Create a newpath with the new entry to test each entry in the directory. */
            CharString newpath(curpath, status);
            newpath.append(dirName, -1, status);
            എങ്കിൽ (U_FAILURE(status)) {
                നിർത്തുക;
            }

            DIR* subDirp = nullptr;
            എങ്കിൽ ((subDirp = opendir(newpath.data())) != nullptr) {
                /* If this new path is a directory, make a recursive call with the newpath. */
                closedir(subDirp);
                newpath.append('/', status);
                എങ്കിൽ (U_FAILURE(status)) {
                    നിർത്തുക;
                }
                result = തിരയൽ_For_TZFile(newpath.data(), tzInfo);
                /*
                 Have to get out here. Otherwise, we'd keep looking
                 and return the first match in the top-level directory
                 if there's a match in the top-level. If not, this function
                 would return nullptr and set gTimeZoneBufferPtr to nullptr in initDefault().
                 It worked without this in most cases because we have a fallback of calling
                 localtime_r to figure out the default timezone.
                */
                എങ്കിൽ (result != nullptr)
                    നിർത്തുക;
            } അല്ലെങ്കിൽ {
                എങ്കിൽ(compare_ദ്വിമാന_Files(newpath.data(), tzInfo)) {
                    int32_t amountToSkip = വലുപ്പം(TZZONEINFO) - 1;
                    എങ്കിൽ (amountToSkip > newpath.length()) {
                        amountToSkip = newpath.length();
                    }
                    സ്ഥിരം അക്ഷരം* zoneid = newpath.data() + amountToSkip;
                    skipZoneIDPrefix(&zoneid);
                    g_തിരയൽ_TZFile_Result->clear();
                    g_തിരയൽ_TZFile_Result->append(zoneid, -1, status);
                    എങ്കിൽ (U_FAILURE(status)) {
                        നിർത്തുക;
                    }
                    result = g_തിരയൽ_TZFile_Result->data();
                    /* Get out after the first one found. */
                    നിർത്തുക;
                }
            }
        }
    }

  cleanupAndReturn:
    എങ്കിൽ (dirp) {
        closedir(dirp);
    }
    തിരികെ result;
}
#endif

#എങ്കിൽ U_PLATFORM == U_PF_ANDROID
തരനിർവചനം പൂർണ്ണസംഖ്യ(system_property_read_callback)(സ്ഥിരം prop_info* info,
                                           ശൂന്യം (*callback)(ശൂന്യം* cookie,
                                                            സ്ഥിരം അക്ഷരം* name,
                                                            സ്ഥിരം അക്ഷരം* value,
                                                            uint32_t serial),
                                           ശൂന്യം* cookie);
തരനിർവചനം പൂർണ്ണസംഖ്യ(system_property_get)(സ്ഥിരം അക്ഷരം*, അക്ഷരം*);

സ്ഥിതം അക്ഷരം gAndroidTimeZone[PROP_മൂല്യം_MAX] = { '\0' };

സ്ഥിതം ശൂന്യം u_property_read(ശൂന്യം* cookie, സ്ഥിരം അക്ഷരം* name, സ്ഥിരം അക്ഷരം* value,
                            uint32_t serial) {
    uprv_strcpy((അക്ഷരം* )cookie, value);
}
#endif

U_CAPI ശൂന്യം U_EXPORT2
uprv_tzname_clear_cache()
{
#എങ്കിൽ U_PLATFORM == U_PF_ANDROID
    /* Android's timezone is stored in system property. */
    gAndroidTimeZone[0] = '\0';
    ശൂന്യം* libc = dlopen("libc.so", RTLD_NOLOAD);
    എങ്കിൽ (libc) {
        /* Android API 26+ has new API to get system property and old API
         * (__system_property_get) is deprecated */
        system_property_read_callback* property_read_callback =
            (system_property_read_callback*)dlsym(
                libc, "__system_property_read_callback");
        എങ്കിൽ (property_read_callback) {
            സ്ഥിരം prop_info* info =
                __system_property_find("persist.sys.timezone");
            എങ്കിൽ (info) {
                property_read_callback(info, &u_property_read, gAndroidTimeZone);
            }
        } അല്ലെങ്കിൽ {
            system_property_get* property_get =
                (system_property_get*)dlsym(libc, "__system_property_get");
            എങ്കിൽ (property_get) {
                property_get("persist.sys.timezone", gAndroidTimeZone);
            }
        }
        dlclose(libc);
    }
#endif

#എങ്കിൽ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    gTimeZoneBufferPtr = nullptr;
#endif
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uprv_tzname(പൂർണ്ണസംഖ്യ n)
{
    (ശൂന്യം)n; // Avoid unreferenced parameter warning.
    സ്ഥിരം അക്ഷരം *tzid = nullptr;
#എങ്കിൽ U_PLATFORM_USES_ONLY_WIN32_API
    tzid = uprv_detectWindowsTimeZone();

    എങ്കിൽ (tzid != nullptr) {
        തിരികെ tzid;
    }

#ifndef U_TZNAME
    // The return value is free'd in timezone.cpp on Windows because
    // the other code path returns a pointer to a heap location.
    // If we don't have a name already, then tzname wouldn't be any
    // better, so just fall back.
    തിരികെ uprv_strdup("");
#endif // !U_TZNAME

#അല്ലെങ്കിൽ

/*#if U_PLATFORM_IS_DARWIN_BASED
    int ret;

    tzid = getenv("TZFILE");
    if (tzid != nullptr) {
        return tzid;
    }
#endif*/

/* This code can be temporarily disabled to test tzname resolution later on. */
#ifndef DEBUG_TZNAME
#എങ്കിൽ U_PLATFORM == U_PF_ANDROID
    tzid = gAndroidTimeZone;
#അല്ലെങ്കിൽ
    tzid = getenv("TZ");
#endif
    എങ്കിൽ (tzid != nullptr && isValidOlsonID(tzid)
#എങ്കിൽ U_PLATFORM == U_PF_SOLARIS
    /* Don't misinterpret TZ "localtime" on Solaris as a time zone name. */
        && uprv_strcmp(tzid, TZ_ENV_CHECK) != 0
#endif
    ) {
        /* The colon forces tzset() to treat the remainder as zoneinfo path */
        എങ്കിൽ (tzid[0] == ':') {
            tzid++;
        }
        /* This might be a good Olson ID. */
        skipZoneIDPrefix(&tzid);
        തിരികെ tzid;
    }
    /* else U_TZNAME will give a better result. */
#endif

#എങ്കിൽ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    /* Caller must handle threading issues */
    എങ്കിൽ (gTimeZoneBufferPtr == nullptr) {
        /*
        This is a trick to look at the name of the link to get the Olson ID
        because the tzfile contents is underspecified.
        This isn't guaranteed to work because it may not be a symlink.
        */
        അക്ഷരം *ret = realpath(TZDEFAULT, gTimeZoneBuffer);
        എങ്കിൽ (ret != nullptr && uprv_strcmp(TZDEFAULT, gTimeZoneBuffer) != 0) {
            int32_t tzZoneInfoTailLen = uprv_strlen(TZZONEINFOTAIL);
            സ്ഥിരം അക്ഷരം *tzZoneInfoTailPtr = uprv_strstr(gTimeZoneBuffer, TZZONEINFOTAIL);
            // MacOS14 has the realpath as something like
            // /usr/share/zoneinfo.default/Australia/Melbourne
            // which will not have "/zoneinfo/" in the path.
            // Therefore if we fail, we fall back to read the link which is
            // /var/db/timezone/zoneinfo/Australia/Melbourne
            // We also fall back to reading the link if the realpath leads to something like
            // /usr/share/zoneinfo/posixrules
            എങ്കിൽ (tzZoneInfoTailPtr == nullptr ||
                    uprv_strcmp(tzZoneInfoTailPtr + tzZoneInfoTailLen, "posixrules") == 0) {
                ssize_t size = readlink(TZDEFAULT, gTimeZoneBuffer, വലുപ്പം(gTimeZoneBuffer)-1);
                എങ്കിൽ (size > 0) {
                    gTimeZoneBuffer[size] = 0;
                    tzZoneInfoTailPtr = uprv_strstr(gTimeZoneBuffer, TZZONEINFOTAIL);
                }
            }
            എങ്കിൽ (tzZoneInfoTailPtr != nullptr) {
                tzZoneInfoTailPtr += tzZoneInfoTailLen;
                skipZoneIDPrefix(&tzZoneInfoTailPtr);
                എങ്കിൽ (isValidOlsonID(tzZoneInfoTailPtr)) {
                    തിരികെ (gTimeZoneBufferPtr = tzZoneInfoTailPtr);
                }
            }
        } അല്ലെങ്കിൽ {
#എങ്കിൽ defined(തിരയൽ_TZFILE)
            DefaultTZInfo* tzInfo = (DefaultTZInfo*)uprv_malloc(വലുപ്പം(DefaultTZInfo));
            എങ്കിൽ (tzInfo != nullptr) {
                tzInfo->defaultTZBuffer = nullptr;
                tzInfo->defaultTZFileSize = 0;
                tzInfo->defaultTZFilePtr = fopen(TZDEFAULT, "r");
                tzInfo->defaultTZPosition = 0;

                gTimeZoneBufferPtr = തിരയൽ_For_TZFile(TZZONEINFO, tzInfo);

                /* Free previously allocated memory */
                എങ്കിൽ (tzInfo->defaultTZBuffer != nullptr) {
                    uprv_free(tzInfo->defaultTZBuffer);
                }
                എങ്കിൽ (tzInfo->defaultTZFilePtr != nullptr) {
                    fclose(tzInfo->defaultTZFilePtr);
                }
                uprv_free(tzInfo);
            }

            എങ്കിൽ (gTimeZoneBufferPtr != nullptr && isValidOlsonID(gTimeZoneBufferPtr)) {
                തിരികെ gTimeZoneBufferPtr;
            }
#endif
        }
    }
    അല്ലെങ്കിൽ {
        തിരികെ gTimeZoneBufferPtr;
    }
#endif
#endif

#ifdef U_TZNAME
#എങ്കിൽ U_PLATFORM_USES_ONLY_WIN32_API
    /* The return value is free'd in timezone.cpp on Windows because
     * the other code path returns a pointer to a heap location. */
    തിരികെ uprv_strdup(U_TZNAME[n]);
#അല്ലെങ്കിൽ
    /*
    U_TZNAME is usually a non-unique abbreviation, which isn't normally usable.
    So we remap the abbreviation to an olson ID.

    Since Windows exposes a little more timezone information,
    we normally don't use this code on Windows because
    uprv_detectWindowsTimeZone should have already given the correct answer.
    */
    {
        ഘടന tm juneSol, decemberSol;
        പൂർണ്ണസംഖ്യ daylightType;
        സ്ഥിതം സ്ഥിരം time_t juneSolstice=1182478260; /*2007-06-21 18:11 UT*/
        സ്ഥിതം സ്ഥിരം time_t decemberSolstice=1198332540; /*2007-12-22 06:09 UT*/

        /* This probing will tell us when daylight savings occurs.  */
        localtime_r(&juneSolstice, &juneSol);
        localtime_r(&decemberSolstice, &decemberSol);
        എങ്കിൽ(decemberSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_DECEMBER;
        } അല്ലെങ്കിൽ എങ്കിൽ(juneSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_JUNE;
        } അല്ലെങ്കിൽ {
          daylightType = U_DAYLIGHT_NONE;
        }
        tzid = remapShortTimeZone(U_TZNAME[0], U_TZNAME[1], daylightType, uprv_timezone());
        എങ്കിൽ (tzid != nullptr) {
            തിരികെ tzid;
        }
    }
    തിരികെ U_TZNAME[n];
#endif
#അല്ലെങ്കിൽ
    തിരികെ "";
#endif
}

/* Get and set the ICU data directory --------------------------------------- */

സ്ഥിതം icu::UInitOnce gDataDirInitOnce {};
സ്ഥിതം അക്ഷരം *gDataDirectory = nullptr;

UInitOnce gTimeZoneFilesInitOnce {};
സ്ഥിതം CharString *gTimeZoneFilesDirectory = nullptr;

#എങ്കിൽ U_POSIX_LOCALE || U_PLATFORM_USES_ONLY_WIN32_API
 സ്ഥിതം സ്ഥിരം അക്ഷരം *gCorrectedPOSIXLocale = nullptr; /* Sometimes heap allocated */
 സ്ഥിതം തർക്കം gCorrectedPOSIXLocaleHeapAllocated = അസത്യം;
#endif

സ്ഥിതം UBool U_CALLCONV putil_cleanup()
{
    എങ്കിൽ (gDataDirectory && *gDataDirectory) {
        uprv_free(gDataDirectory);
    }
    gDataDirectory = nullptr;
    gDataDirInitOnce.reset();

    delete gTimeZoneFilesDirectory;
    gTimeZoneFilesDirectory = nullptr;
    gTimeZoneFilesInitOnce.reset();

#ifdef തിരയൽ_TZFILE
    delete g_തിരയൽ_TZFile_Result;
    g_തിരയൽ_TZFile_Result = nullptr;
#endif

#എങ്കിൽ U_POSIX_LOCALE || U_PLATFORM_USES_ONLY_WIN32_API
    എങ്കിൽ (gCorrectedPOSIXLocale && gCorrectedPOSIXLocaleHeapAllocated) {
        uprv_free(const_cast<അക്ഷരം *>(gCorrectedPOSIXLocale));
        gCorrectedPOSIXLocale = nullptr;
        gCorrectedPOSIXLocaleHeapAllocated = അസത്യം;
    }
#endif
    തിരികെ സത്യം;
}

/*
 * Set the data directory.
 *    Make a copy of the passed string, and set the global data dir to point to it.
 */
U_CAPI ശൂന്യം U_EXPORT2
u_setDataDirectory(സ്ഥിരം അക്ഷരം *directory) {
    അക്ഷരം *newDataDir;
    int32_t length;

    എങ്കിൽ(directory==nullptr || *directory==0) {
        /* A small optimization to prevent the malloc and copy when the
        shared library is used, and this is a way to make sure that nullptr
        is never returned.
        */
        newDataDir = (അക്ഷരം *)"";
    }
    അല്ലെങ്കിൽ {
        length=(int32_t)uprv_strlen(directory);
        newDataDir = (അക്ഷരം *)uprv_malloc(length + 2);
        /* Exit out if newDataDir could not be created. */
        എങ്കിൽ (newDataDir == nullptr) {
            തിരികെ;
        }
        uprv_strcpy(newDataDir, directory);

#എങ്കിൽ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
        {
            അക്ഷരം *p;
            ആകുമ്പോൾ((p = uprv_strchr(newDataDir, U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
        }
#endif
    }

    എങ്കിൽ (gDataDirectory && *gDataDirectory) {
        uprv_free(gDataDirectory);
    }
    gDataDirectory = newDataDir;
    ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
}

U_CAPI UBool U_EXPORT2
uprv_pathIsAbsolute(സ്ഥിരം അക്ഷരം *path)
{
  എങ്കിൽ(!path || !*path) {
    തിരികെ അസത്യം;
  }

  എങ്കിൽ(*path == U_FILE_SEP_CHAR) {
    തിരികെ സത്യം;
  }

#എങ്കിൽ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
  എങ്കിൽ(*path == U_FILE_ALT_SEP_CHAR) {
    തിരികെ സത്യം;
  }
#endif

#എങ്കിൽ U_PLATFORM_USES_ONLY_WIN32_API
  എങ്കിൽ( (((path[0] >= 'A') && (path[0] <= 'Z')) ||
       ((path[0] >= 'a') && (path[0] <= 'z'))) &&
      path[1] == ':' ) {
    തിരികെ സത്യം;
  }
#endif

  തിരികെ അസത്യം;
}

/* Backup setting of ICU_DATA_DIR_PREFIX_ENV_VAR
   (needed for some Darwin ICU build environments) */
#എങ്കിൽ U_PLATFORM_IS_DARWIN_BASED && defined(TARGET_OS_SIMULATOR) && TARGET_OS_SIMULATOR
# എങ്കിൽ !defined(ICU_DATA_DIR_PREFIX_ENV_VAR)
#  define ICU_DATA_DIR_PREFIX_ENV_VAR "IPHONE_SIMULATOR_ROOT"
# endif
#endif

#എങ്കിൽ defined(ICU_DATA_DIR_WINDOWS)
// Helper function to get the ICU Data Directory under the Windows directory location.
സ്ഥിതം BOOL U_CALLCONV getIcuDataDirectoryUnderWindowsDirectory(അക്ഷരം* directoryBuffer, UINT bufferLength)
{
    wchar_t windowsPath[MAX_PATH];
    അക്ഷരം windowsPathUtf8[MAX_PATH];

    UINT length = GetSystemWindowsDirectoryW(windowsPath, UPRV_LENGTHOF(windowsPath));
    എങ്കിൽ ((length > 0) && (length < (UPRV_LENGTHOF(windowsPath) - 1))) {
        // Convert UTF-16 to a UTF-8 string.
        UErrorCode status = U_ZERO_ERROR;
        int32_t windowsPathUtf8Len = 0;
        u_strToUTF8(windowsPathUtf8, static_cast<int32_t>(UPRV_LENGTHOF(windowsPathUtf8)),
            &windowsPathUtf8Len, reinterpret_cast<സ്ഥിരം char16_t*>(windowsPath), -1, &status);

        എങ്കിൽ (U_SUCCESS(status) && (status != U_STRING_NOT_TERMINATED_WARNING) &&
            (windowsPathUtf8Len < (UPRV_LENGTHOF(windowsPathUtf8) - 1))) {
            // Ensure it always has a separator, so we can append the ICU data path.
            എങ്കിൽ (windowsPathUtf8[windowsPathUtf8Len - 1] != U_FILE_SEP_CHAR) {
                windowsPathUtf8[windowsPathUtf8Len++] = U_FILE_SEP_CHAR;
                windowsPathUtf8[windowsPathUtf8Len] = '\0';
            }
            // Check if the concatenated string will fit.
            എങ്കിൽ ((windowsPathUtf8Len + UPRV_LENGTHOF(ICU_DATA_DIR_WINDOWS)) < bufferLength) {
                uprv_strcpy(directoryBuffer, windowsPathUtf8);
                uprv_strcat(directoryBuffer, ICU_DATA_DIR_WINDOWS);
                തിരികെ സത്യം;
            }
        }
    }

    തിരികെ അസത്യം;
}
#endif

സ്ഥിതം ശൂന്യം U_CALLCONV dataDirectoryInitFn() {
    /* If we already have the directory, then return immediately. Will happen if user called
     * u_setDataDirectory().
     */
    എങ്കിൽ (gDataDirectory) {
        തിരികെ;
    }

    സ്ഥിരം അക്ഷരം *path = nullptr;

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
#   എങ്കിൽ !defined(ICU_NO_USER_DATA_OVERRIDE) && !UCONFIG_NO_FILE_IO
    /* First try to get the environment variable */
#     എങ്കിൽ U_PLATFORM_HAS_WINUWP_API == 0  // Windows UWP does not support getenv
        path=getenv("ICU_DATA");
#     endif
#   endif

    /* ICU_DATA_DIR may be set as a compile option.
     * U_ICU_DATA_DEFAULT_DIR is provided and is set by ICU at compile time
     * and is used only when data is built in archive mode eliminating the need
     * for ICU_DATA_DIR to be set. U_ICU_DATA_DEFAULT_DIR is set to the installation
     * directory of the data dat file. Users should use ICU_DATA_DIR if they want to
     * set their own path.
     */
#എങ്കിൽ defined(ICU_DATA_DIR) || defined(U_ICU_DATA_DEFAULT_DIR)
# എങ്കിൽ defined(ICU_DATA_DIR_PREFIX_ENV_VAR)
    അക്ഷരം datadir_path_buffer[PATH_MAX];
# endif
    എങ്കിൽ(path==nullptr || *path==0) {
# എങ്കിൽ defined(ICU_DATA_DIR_PREFIX_ENV_VAR)
        സ്ഥിരം അക്ഷരം *prefix = getenv(ICU_DATA_DIR_PREFIX_ENV_VAR);
# endif
# ifdef ICU_DATA_DIR
        path=ICU_DATA_DIR;
# അല്ലെങ്കിൽ
        path=U_ICU_DATA_DEFAULT_DIR;
# endif
# എങ്കിൽ defined(ICU_DATA_DIR_PREFIX_ENV_VAR)
        എങ്കിൽ (prefix != nullptr) {
            snprintf(datadir_path_buffer, വലുപ്പം(datadir_path_buffer), "%s%s", prefix, path);
            path=datadir_path_buffer;
        }
# endif
    }
#endif

#എങ്കിൽ defined(ICU_DATA_DIR_WINDOWS)
    അക്ഷരം datadir_path_buffer[MAX_PATH];
    എങ്കിൽ (getIcuDataDirectoryUnderWindowsDirectory(datadir_path_buffer, UPRV_LENGTHOF(datadir_path_buffer))) {
        path = datadir_path_buffer;
    }
#endif

    എങ്കിൽ(path==nullptr) {
        /* It looks really bad, set it to something. */
        path = "";
    }

    u_setDataDirectory(path);
}

U_CAPI സ്ഥിരം അക്ഷരം * U_EXPORT2
u_getDataDirectory() {
    umtx_initOnce(gDataDirInitOnce, &dataDirectoryInitFn);
    തിരികെ gDataDirectory;
}

സ്ഥിതം ശൂന്യം setTimeZoneFilesDir(സ്ഥിരം അക്ഷരം *path, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    gTimeZoneFilesDirectory->clear();
    gTimeZoneFilesDirectory->append(path, status);
#എങ്കിൽ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    അക്ഷരം *p = gTimeZoneFilesDirectory->data();
    ആകുമ്പോൾ ((p = uprv_strchr(p, U_FILE_ALT_SEP_CHAR)) != nullptr) {
        *p = U_FILE_SEP_CHAR;
    }
#endif
}

#define TO_STRING(x) TO_STRING_2(x)
#define TO_STRING_2(x) #x

സ്ഥിതം ശൂന്യം U_CALLCONV TimeZoneDataDirInitFn(UErrorCode &status) {
    U_ASSERT(gTimeZoneFilesDirectory == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    gTimeZoneFilesDirectory = പുതിയ CharString();
    എങ്കിൽ (gTimeZoneFilesDirectory == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }

    സ്ഥിരം അക്ഷരം *dir = "";

#എങ്കിൽ defined(ICU_TIMEZONE_FILES_DIR_PREFIX_ENV_VAR)
    അക്ഷരം timezonefilesdir_path_buffer[PATH_MAX];
    സ്ഥിരം അക്ഷരം *prefix = getenv(ICU_TIMEZONE_FILES_DIR_PREFIX_ENV_VAR);
#endif

#എങ്കിൽ U_PLATFORM_HAS_WINUWP_API == 1
// The UWP version does not support the environment variable setting.

# എങ്കിൽ defined(ICU_DATA_DIR_WINDOWS)
    // When using the Windows system data, we can possibly pick up time zone data from the Windows directory.
    അക്ഷരം datadir_path_buffer[MAX_PATH];
    എങ്കിൽ (getIcuDataDirectoryUnderWindowsDirectory(datadir_path_buffer, UPRV_LENGTHOF(datadir_path_buffer))) {
        dir = datadir_path_buffer;
    }
# endif

#അല്ലെങ്കിൽ
    dir = getenv("ICU_TIMEZONE_FILES_DIR");
#endif // U_PLATFORM_HAS_WINUWP_API

#എങ്കിൽ defined(U_TIMEZONE_FILES_DIR)
    എങ്കിൽ (dir == nullptr) {
        // Build time configuration setting.
        dir = TO_STRING(U_TIMEZONE_FILES_DIR);
    }
#endif

    എങ്കിൽ (dir == nullptr) {
        dir = "";
    }

#എങ്കിൽ defined(ICU_TIMEZONE_FILES_DIR_PREFIX_ENV_VAR)
    എങ്കിൽ (prefix != nullptr) {
        snprintf(timezonefilesdir_path_buffer, വലുപ്പം(timezonefilesdir_path_buffer), "%s%s", prefix, dir);
        dir = timezonefilesdir_path_buffer;
    }
#endif

    setTimeZoneFilesDir(dir, status);
}


U_CAPI സ്ഥിരം അക്ഷരം * U_EXPORT2
u_getTimeZoneFilesDirectory(UErrorCode *status) {
    umtx_initOnce(gTimeZoneFilesInitOnce, &TimeZoneDataDirInitFn, *status);
    തിരികെ U_SUCCESS(*status) ? gTimeZoneFilesDirectory->data() : "";
}

U_CAPI ശൂന്യം U_EXPORT2
u_setTimeZoneFilesDirectory(സ്ഥിരം അക്ഷരം *path, UErrorCode *status) {
    umtx_initOnce(gTimeZoneFilesInitOnce, &TimeZoneDataDirInitFn, *status);
    setTimeZoneFilesDir(path, *status);

    // Note: this function does some extra churn, first setting based on the
    //       environment, then immediately replacing with the value passed in.
    //       The logic is simpler that way, and performance shouldn't be an issue.
}


#എങ്കിൽ U_POSIX_LOCALE
/* A helper function used by uprv_getPOSIXIDForDefaultLocale and
 * uprv_getPOSIXIDForDefaultCodepage. Returns the posix locale id for
 * LC_CTYPE and LC_MESSAGES. It doesn't support other locale categories.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം *uprv_getPOSIXIDForCategory(പൂർണ്ണസംഖ്യ category)
{
    സ്ഥിരം അക്ഷരം* posixID = nullptr;
    എങ്കിൽ (category == LC_MESSAGES || category == LC_CTYPE) {
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
        posixID = setlocale(category, nullptr);
        എങ്കിൽ ((posixID == nullptr)
            || (uprv_strcmp("C", posixID) == 0)
            || (uprv_strcmp("POSIX", posixID) == 0))
        {
            /* Maybe we got some garbage.  Try something more reasonable */
            posixID = getenv("LC_ALL");
            /* Solaris speaks POSIX -  See IEEE Std 1003.1-2008
             * This is needed to properly handle empty env. variables
             */
#എങ്കിൽ U_PLATFORM == U_PF_SOLARIS
            എങ്കിൽ ((posixID == 0) || (posixID[0] == '\0')) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                എങ്കിൽ ((posixID == 0) || (posixID[0] == '\0')) {
#അല്ലെങ്കിൽ
            എങ്കിൽ (posixID == nullptr) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                എങ്കിൽ (posixID == nullptr) {
#endif
                    posixID = getenv("LANG");
                }
            }
        }
    }
    എങ്കിൽ ((posixID == nullptr)
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
    തിരികെ posixID;
}

/* Return just the POSIX id for the default locale, whatever happens to be in
 * it. It gets the value from LC_MESSAGES and indirectly from LC_ALL and LANG.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം *uprv_getPOSIXIDForDefaultLocale()
{
    സ്ഥിതം സ്ഥിരം അക്ഷരം* posixID = nullptr;
    എങ്കിൽ (posixID == nullptr) {
        posixID = uprv_getPOSIXIDForCategory(LC_MESSAGES);
    }
    തിരികെ posixID;
}

#എങ്കിൽ !U_CHARSET_IS_UTF8
/* Return just the POSIX id for the default codepage, whatever happens to be in
 * it. It gets the value from LC_CTYPE and indirectly from LC_ALL and LANG.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം *uprv_getPOSIXIDForDefaultCodepage()
{
    സ്ഥിതം സ്ഥിരം അക്ഷരം* posixID = nullptr;
    എങ്കിൽ (posixID == 0) {
        posixID = uprv_getPOSIXIDForCategory(LC_CTYPE);
    }
    തിരികെ posixID;
}
#endif
#endif

/* NOTE: The caller should handle thread safety */
U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
uprv_getDefaultLocaleID()
{
#എങ്കിൽ U_POSIX_LOCALE
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
    സ്ഥിരം അക്ഷരം* posixID = uprv_getPOSIXIDForDefaultLocale();

    /* Format: (no spaces)
    ll [ _CC ] [ . MM ] [ @ VV]

      l = lang, C = ctry, M = charmap, V = variant
    */

    എങ്കിൽ (gCorrectedPOSIXLocale != nullptr) {
        തിരികെ gCorrectedPOSIXLocale;
    }

    // Copy the ID into owned memory.
    // Over-allocate in case we replace "C" with "en_US_POSIX" (+10), + null termination
    അക്ഷരം *correctedPOSIXLocale = static_cast<അക്ഷരം *>(uprv_malloc(uprv_strlen(posixID) + 10 + 1));
    എങ്കിൽ (correctedPOSIXLocale == nullptr) {
        തിരികെ nullptr;
    }
    uprv_strcpy(correctedPOSIXLocale, posixID);

    അക്ഷരം *limit;
    എങ്കിൽ ((limit = uprv_strchr(correctedPOSIXLocale, '.')) != nullptr) {
        *limit = 0;
    }
    എങ്കിൽ ((limit = uprv_strchr(correctedPOSIXLocale, '@')) != nullptr) {
        *limit = 0;
    }

    എങ്കിൽ ((uprv_strcmp("C", correctedPOSIXLocale) == 0) // no @ variant
        || (uprv_strcmp("POSIX", correctedPOSIXLocale) == 0)) {
      // Raw input was C.* or POSIX.*, Give it a nice POSIX default value.
      // (The "C"/"POSIX" case is handled in uprv_getPOSIXIDForCategory())
      uprv_strcpy(correctedPOSIXLocale, "en_US_POSIX");
    }

    /* Note that we scan the *uncorrected* ID. */
    സ്ഥിരം അക്ഷരം *p;
    എങ്കിൽ ((p = uprv_strrchr(posixID, '@')) != nullptr) {
        p++;

        /* Take care of any special cases here.. */
        എങ്കിൽ (!uprv_strcmp(p, "nynorsk")) {
            p = "NY";
            /* Don't worry about no__NY. In practice, it won't appear. */
        }

        എങ്കിൽ (uprv_strchr(correctedPOSIXLocale,'_') == nullptr) {
            uprv_strcat(correctedPOSIXLocale, "__"); /* aa@b -> aa__b (note this can make the new locale 1 char longer) */
        }
        അല്ലെങ്കിൽ {
            uprv_strcat(correctedPOSIXLocale, "_"); /* aa_CC@b -> aa_CC_b */
        }

        സ്ഥിരം അക്ഷരം *q;
        എങ്കിൽ ((q = uprv_strchr(p, '.')) != nullptr) {
            /* How big will the resulting string be? */
            int32_t len = (int32_t)(uprv_strlen(correctedPOSIXLocale) + (q-p));
            uprv_strncat(correctedPOSIXLocale, p, q-p); // do not include charset
            correctedPOSIXLocale[len] = 0;
        }
        അല്ലെങ്കിൽ {
            /* Anything following the @ sign */
            uprv_strcat(correctedPOSIXLocale, p);
        }

        /* Should there be a map from 'no@nynorsk' -> no_NO_NY here?
         * How about 'russian' -> 'ru'?
         * Many of the other locales using ISO codes will be handled by the
         * canonicalization functions in uloc_getDefault.
         */
    }

    എങ്കിൽ (gCorrectedPOSIXLocale == nullptr) {
        gCorrectedPOSIXLocale = correctedPOSIXLocale;
        gCorrectedPOSIXLocaleHeapAllocated = സത്യം;
        ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
        correctedPOSIXLocale = nullptr;
    }
    posixID = gCorrectedPOSIXLocale;

    എങ്കിൽ (correctedPOSIXLocale != nullptr) {  /* Was already set - clean up. */
        uprv_free(correctedPOSIXLocale);
    }

    തിരികെ posixID;

#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM_USES_ONLY_WIN32_API
#define POSIX_LOCALE_CAPACITY 64
    UErrorCode status = U_ZERO_ERROR;
    അക്ഷരം *correctedPOSIXLocale = nullptr;

    // If we have already figured this out just use the cached value
    എങ്കിൽ (gCorrectedPOSIXLocale != nullptr) {
        തിരികെ gCorrectedPOSIXLocale;
    }

    // No cached value, need to determine the current value
    സ്ഥിതം WCHAR windowsLocale[LOCALE_NAME_MAX_LENGTH] = {};
    പൂർണ്ണസംഖ്യ length = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, windowsLocale, LOCALE_NAME_MAX_LENGTH);

    // Now we should have a Windows locale name that needs converted to the POSIX style.
    എങ്കിൽ (length > 0) // If length is 0, then the GetLocaleInfoEx failed.
    {
        // First we need to go from UTF-16 to char (and also convert from _ to - while we're at it.)
        അക്ഷരം modifiedWindowsLocale[LOCALE_NAME_MAX_LENGTH] = {};

        int32_t i;
        വേണ്ടി (i = 0; i < UPRV_LENGTHOF(modifiedWindowsLocale); i++)
        {
            എങ്കിൽ (windowsLocale[i] == '_')
            {
                modifiedWindowsLocale[i] = '-';
            }
            അല്ലെങ്കിൽ
            {
                modifiedWindowsLocale[i] = static_cast<അക്ഷരം>(windowsLocale[i]);
            }

            എങ്കിൽ (modifiedWindowsLocale[i] == '\0')
            {
                നിർത്തുക;
            }
        }

        എങ്കിൽ (i >= UPRV_LENGTHOF(modifiedWindowsLocale))
        {
            // Ran out of room, can't really happen, maybe we'll be lucky about a matching
            // locale when tags are dropped
            modifiedWindowsLocale[UPRV_LENGTHOF(modifiedWindowsLocale) - 1] = '\0';
        }

        // Now normalize the resulting name
        correctedPOSIXLocale = static_cast<അക്ഷരം *>(uprv_malloc(POSIX_LOCALE_CAPACITY + 1));
        /* TODO: Should we just exit on memory allocation failure? */
        എങ്കിൽ (correctedPOSIXLocale)
        {
            int32_t posixLen = uloc_canonicalize(modifiedWindowsLocale, correctedPOSIXLocale, POSIX_LOCALE_CAPACITY, &status);
            എങ്കിൽ (U_SUCCESS(status))
            {
                *(correctedPOSIXLocale + posixLen) = 0;
                gCorrectedPOSIXLocale = correctedPOSIXLocale;
                gCorrectedPOSIXLocaleHeapAllocated = സത്യം;
                ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
            }
            അല്ലെങ്കിൽ
            {
                uprv_free(correctedPOSIXLocale);
            }
        }
    }

    // If unable to find a locale we can agree upon, use en-US by default
    എങ്കിൽ (gCorrectedPOSIXLocale == nullptr) {
        gCorrectedPOSIXLocale = "en_US";
    }
    തിരികെ gCorrectedPOSIXLocale;

#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_OS400
    /* locales are process scoped and are by definition thread safe */
    സ്ഥിതം അക്ഷരം correctedLocale[64];
    സ്ഥിരം  അക്ഷരം *localeID = getenv("LC_ALL");
           അക്ഷരം *p;

    എങ്കിൽ (localeID == nullptr)
        localeID = getenv("LANG");
    എങ്കിൽ (localeID == nullptr)
        localeID = setlocale(LC_ALL, nullptr);
    /* Make sure we have something... */
    എങ്കിൽ (localeID == nullptr)
        തിരികെ "en_US_POSIX";

    /* Extract the locale name from the path. */
    എങ്കിൽ((p = uprv_strrchr(localeID, '/')) != nullptr)
    {
        /* Increment p to start of locale name. */
        p++;
        localeID = p;
    }

    /* Copy to work location. */
    uprv_strcpy(correctedLocale, localeID);

    /* Strip off the '.locale' extension. */
    എങ്കിൽ((p = uprv_strchr(correctedLocale, '.')) != nullptr) {
        *p = 0;
    }

    /* Upper case the locale name. */
    T_CString_toUpperCase(correctedLocale);

    /* See if we are using the POSIX locale.  Any of the
    * following are equivalent and use the same QLGPGCMA
    * (POSIX) locale.
    * QLGPGCMA2 means UCS2
    * QLGPGCMA_4 means UTF-32
    * QLGPGCMA_8 means UTF-8
    */
    എങ്കിൽ ((uprv_strcmp("C", correctedLocale) == 0) ||
        (uprv_strcmp("POSIX", correctedLocale) == 0) ||
        (uprv_strncmp("QLGPGCMA", correctedLocale, 8) == 0))
    {
        uprv_strcpy(correctedLocale, "en_US_POSIX");
    }
    അല്ലെങ്കിൽ
    {
        int16_t LocaleLen;

        /* Lower case the lang portion. */
        വേണ്ടി(p = correctedLocale; *p != 0 && *p != '_'; p++)
        {
            *p = uprv_tolower(*p);
        }

        /* Adjust for Euro.  After '_E' add 'URO'. */
        LocaleLen = uprv_strlen(correctedLocale);
        എങ്കിൽ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'E')
        {
            uprv_strcat(correctedLocale, "URO");
        }

        /* If using Lotus-based locale then convert to
         * equivalent non Lotus.
         */
        അല്ലെങ്കിൽ എങ്കിൽ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'L')
        {
            correctedLocale[LocaleLen - 2] = 0;
        }

        /* There are separate simplified and traditional
         * locales called zh_HK_S and zh_HK_T.
         */
        അല്ലെങ്കിൽ എങ്കിൽ (uprv_strncmp(correctedLocale, "zh_HK", 5) == 0)
        {
            uprv_strcpy(correctedLocale, "zh_HK");
        }

        /* A special zh_CN_GBK locale...
        */
        അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(correctedLocale, "zh_CN_GBK") == 0)
        {
            uprv_strcpy(correctedLocale, "zh_CN");
        }

    }

    തിരികെ correctedLocale;
#endif

}

#എങ്കിൽ !U_CHARSET_IS_UTF8
#എങ്കിൽ U_POSIX_LOCALE
/*
Due to various platform differences, one platform may specify a charset,
when they really mean a different charset. Remap the names so that they are
compatible with ICU. Only conflicting/ambiguous aliases should be resolved
here. Before adding anything to this function, please consider adding unique
names to the ICU alias table in the data directory.
*/
സ്ഥിതം സ്ഥിരം അക്ഷരം*
remapPlatformDependentCodepage(സ്ഥിരം അക്ഷരം *locale, സ്ഥിരം അക്ഷരം *name) {
    എങ്കിൽ (locale != nullptr && *locale == 0) {
        /* Make sure that an empty locale is handled the same way. */
        locale = nullptr;
    }
    എങ്കിൽ (name == nullptr) {
        തിരികെ nullptr;
    }
#എങ്കിൽ U_PLATFORM == U_PF_AIX
    എങ്കിൽ (uprv_strcmp(name, "IBM-943") == 0) {
        /* Use the ASCII compatible ibm-943 */
        name = "Shift-JIS";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(name, "IBM-1252") == 0) {
        /* Use the windows-1252 that contains the Euro */
        name = "IBM-5348";
    }
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_SOLARIS
    എങ്കിൽ (locale != nullptr && uprv_strcmp(name, "EUC") == 0) {
        /* Solaris underspecifies the "EUC" name. */
        എങ്കിൽ (uprv_strcmp(locale, "zh_CN") == 0) {
            name = "EUC-CN";
        }
        അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(locale, "zh_TW") == 0) {
            name = "EUC-TW";
        }
        അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(locale, "ko_KR") == 0) {
            name = "EUC-KR";
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-954 is the best match.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(name, "646") == 0) {
        /*
         * The default codepage given by Solaris is 646 but the C library routines treat it as if it was
         * ISO-8859-1 instead of US-ASCII(646).
         */
        name = "ISO-8859-1";
    }
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM_IS_DARWIN_BASED
    എങ്കിൽ (locale == nullptr && *name == 0) {
        /*
        No locale was specified, and an empty name was passed in.
        This usually indicates that nl_langinfo didn't return valid information.
        Mac OS X uses UTF-8 by default (especially the locale data and console).
        */
        name = "UTF-8";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (locale != nullptr && uprv_strcmp(locale, "en_US_POSIX") != 0 && uprv_strcmp(name, "US-ASCII") == 0) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_BSD
    എങ്കിൽ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_HPUX
    എങ്കിൽ (locale != nullptr && uprv_strcmp(locale, "zh_HK") == 0 && uprv_strcmp(name, "big5") == 0) {
        /* HP decided to extend big5 as hkbig5 even though it's not compatible :-( */
        /* zh_TW.big5 is not the same charset as zh_HK.big5! */
        name = "hkbig5";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_LINUX
    എങ്കിൽ (locale != nullptr && uprv_strcmp(name, "euc") == 0) {
        /* Linux underspecifies the "EUC" name. */
        എങ്കിൽ (uprv_strcmp(locale, "korean") == 0) {
            name = "EUC-KR";
        }
        അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(locale, "japanese") == 0) {
            /* See comment below about eucJP */
            name = "eucjis";
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(name, "eucjp") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    അല്ലെങ്കിൽ എങ്കിൽ (locale != nullptr && uprv_strcmp(locale, "en_US_POSIX") != 0 &&
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
#endif
    /* return nullptr when "" is passed in */
    എങ്കിൽ (*name == 0) {
        name = nullptr;
    }
    തിരികെ name;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം*
getCodepageFromPOSIXID(സ്ഥിരം അക്ഷരം *localeName, അക്ഷരം * buffer, int32_t buffCapacity)
{
    അക്ഷരം localeBuf[100];
    സ്ഥിരം അക്ഷരം *name = nullptr;
    അക്ഷരം *variant = nullptr;

    എങ്കിൽ (localeName != nullptr && (name = (uprv_strchr(localeName, '.'))) != nullptr) {
        size_t localeCapacity = uprv_min(വലുപ്പം(localeBuf), (name-localeName)+1);
        uprv_strncpy(localeBuf, localeName, localeCapacity);
        localeBuf[localeCapacity-1] = 0; /* ensure NUL termination */
        name = uprv_strncpy(buffer, name+1, buffCapacity);
        buffer[buffCapacity-1] = 0; /* ensure NUL termination */
        എങ്കിൽ ((variant = const_cast<അക്ഷരം *>(uprv_strchr(name, '@'))) != nullptr) {
            *variant = 0;
        }
        name = remapPlatformDependentCodepage(localeBuf, name);
    }
    തിരികെ name;
}
#endif

സ്ഥിതം സ്ഥിരം അക്ഷരം*
int_getDefaultCodepage()
{
#എങ്കിൽ U_PLATFORM == U_PF_OS400
    uint32_t ccsid = 37; /* Default to ibm-37 */
    സ്ഥിതം അക്ഷരം codepage[64];
    Qwc_JOBI0400_t jobinfo;
    Qus_EC_t error = { വലുപ്പം(Qus_EC_t) }; /* SPI error code */

    EPT_CALL(QUSRJOBI)(&jobinfo, വലുപ്പം(jobinfo), "JOBI0400",
        "*                         ", "                ", &error);

    എങ്കിൽ (error.Bytes_Available == 0) {
        എങ്കിൽ (jobinfo.Coded_Char_Set_ID != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Coded_Char_Set_ID;
        }
        അല്ലെങ്കിൽ എങ്കിൽ (jobinfo.Default_Coded_Char_Set_Id != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Default_Coded_Char_Set_Id;
        }
        /* else use the default */
    }
    snprintf(codepage, വലുപ്പം(codepage), "ibm-%d", ccsid);
    തിരികെ codepage;

#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM == U_PF_OS390
    സ്ഥിതം അക്ഷരം codepage[64];

    strncpy(codepage, nl_langinfo(CODESET),63-strlen(UCNV_SWAP_LFNL_OPTION_STRING));
    strcat(codepage,UCNV_SWAP_LFNL_OPTION_STRING);
    codepage[63] = 0; /* NUL terminate */

    തിരികെ codepage;

#അല്ലെങ്കിലെങ്കിൽ U_PLATFORM_USES_ONLY_WIN32_API
    സ്ഥിതം അക്ഷരം codepage[64];
    DWORD codepage_സംഖ്യ = 0;

#എങ്കിൽ U_PLATFORM_HAS_WINUWP_API == 1
    // UWP doesn't have a direct API to get the default ACP as Microsoft would rather
    // have folks use Unicode than a "system" code page, however this is the same
    // codepage as the system default locale codepage.  (FWIW, the system locale is
    // ONLY used for codepage, it should never be used for anything else)
    GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_സംഖ്യ,
        (LPWSTR)&codepage_സംഖ്യ, വലുപ്പം(codepage_സംഖ്യ) / വലുപ്പം(WCHAR));
#അല്ലെങ്കിൽ
    // Win32 apps can call GetACP
    codepage_സംഖ്യ = GetACP();
#endif
    // Special case for UTF-8
    എങ്കിൽ (codepage_സംഖ്യ == 65001)
    {
        തിരികെ "UTF-8";
    }
    // Windows codepages can look like windows-1252, so format the found number
    // the numbers are eclectic, however all valid system code pages, besides UTF-8
    // are between 3 and 19999
    എങ്കിൽ (codepage_സംഖ്യ > 0 && codepage_സംഖ്യ < 20000)
    {
        snprintf(codepage, വലുപ്പം(codepage), "windows-%ld", codepage_സംഖ്യ);
        തിരികെ codepage;
    }
    // If the codepage number call failed then return UTF-8
    തിരികെ "UTF-8";

#അല്ലെങ്കിലെങ്കിൽ U_POSIX_LOCALE
    സ്ഥിതം അക്ഷരം codesetName[100];
    സ്ഥിരം അക്ഷരം *localeName = nullptr;
    സ്ഥിരം അക്ഷരം *name = nullptr;

    localeName = uprv_getPOSIXIDForDefaultCodepage();
    uprv_memset(codesetName, 0, വലുപ്പം(codesetName));
    /* On Solaris nl_langinfo returns C locale values unless setlocale
     * was called earlier.
     */
#എങ്കിൽ (U_HAVE_NL_LANGINFO_CODESET && U_PLATFORM != U_PF_SOLARIS)
    /* When available, check nl_langinfo first because it usually gives more
       useful names. It depends on LC_CTYPE.
       nl_langinfo may use the same buffer as setlocale. */
    {
        സ്ഥിരം അക്ഷരം *codeset = nl_langinfo(U_NL_LANGINFO_CODESET);
#എങ്കിൽ U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED
        /*
         * On Linux and MacOSX, ensure that default codepage for non C/POSIX locale is UTF-8
         * instead of ASCII.
         */
        എങ്കിൽ (uprv_strcmp(localeName, "en_US_POSIX") != 0) {
            codeset = remapPlatformDependentCodepage(localeName, codeset);
        } അല്ലെങ്കിൽ
#endif
        {
            codeset = remapPlatformDependentCodepage(nullptr, codeset);
        }

        എങ്കിൽ (codeset != nullptr) {
            uprv_strncpy(codesetName, codeset, വലുപ്പം(codesetName));
            codesetName[വലുപ്പം(codesetName)-1] = 0;
            തിരികെ codesetName;
        }
    }
#endif

    /* Use setlocale in a nice way, and then check some environment variables.
       Maybe the application used setlocale already.
    */
    uprv_memset(codesetName, 0, വലുപ്പം(codesetName));
    name = getCodepageFromPOSIXID(localeName, codesetName, വലുപ്പം(codesetName));
    എങ്കിൽ (name) {
        /* if we can find the codeset name from setlocale, return that. */
        തിരികെ name;
    }

    എങ്കിൽ (*codesetName == 0)
    {
        /* Everything failed. Return US ASCII (ISO 646). */
        (ശൂന്യം)uprv_strcpy(codesetName, "US-ASCII");
    }
    തിരികെ codesetName;
#അല്ലെങ്കിൽ
    തിരികെ "US-ASCII";
#endif
}


U_CAPI സ്ഥിരം അക്ഷരം*  U_EXPORT2
uprv_getDefaultCodepage()
{
    സ്ഥിതം അക്ഷരം സ്ഥിരം  *name = nullptr;
    umtx_lock(nullptr);
    എങ്കിൽ (name == nullptr) {
        name = int_getDefaultCodepage();
    }
    umtx_unlock(nullptr);
    തിരികെ name;
}
#endif  /* !U_CHARSET_IS_UTF8 */


/* end of platform-specific implementation -------------- */

/* version handling --------------------------------------------------------- */

U_CAPI ശൂന്യം U_EXPORT2
u_versionFromString(UVersionInfo versionArray, സ്ഥിരം അക്ഷരം *versionString) {
    അക്ഷരം *end;
    uint16_t part=0;

    എങ്കിൽ(versionArray==nullptr) {
        തിരികെ;
    }

    എങ്കിൽ(versionString!=nullptr) {
        വേണ്ടി(;;) {
            versionArray[part]=(uint8_t)uprv_strtoul(versionString, &end, 10);
            എങ്കിൽ(end==versionString || ++part==U_MAX_VERSION_LENGTH || *end!=U_VERSION_DELIMITER) {
                നിർത്തുക;
            }
            versionString=end+1;
        }
    }

    ആകുമ്പോൾ(part<U_MAX_VERSION_LENGTH) {
        versionArray[part++]=0;
    }
}

U_CAPI ശൂന്യം U_EXPORT2
u_versionFromUString(UVersionInfo versionArray, സ്ഥിരം char16_t *versionString) {
    എങ്കിൽ(versionArray!=nullptr && versionString!=nullptr) {
        അക്ഷരം versionChars[U_MAX_VERSION_STRING_LENGTH+1];
        int32_t len = u_strlen(versionString);
        എങ്കിൽ(len>U_MAX_VERSION_STRING_LENGTH) {
            len = U_MAX_VERSION_STRING_LENGTH;
        }
        u_UCharsToChars(versionString, versionChars, len);
        versionChars[len]=0;
        u_versionFromString(versionArray, versionChars);
    }
}

U_CAPI ശൂന്യം U_EXPORT2
u_versionToString(സ്ഥിരം UVersionInfo versionArray, അക്ഷരം *versionString) {
    uint16_t count, part;
    uint8_t field;

    എങ്കിൽ(versionString==nullptr) {
        തിരികെ;
    }

    എങ്കിൽ(versionArray==nullptr) {
        versionString[0]=0;
        തിരികെ;
    }

    /* count how many fields need to be written */
    വേണ്ടി(count=4; count>0 && versionArray[count-1]==0; --count) {
    }

    എങ്കിൽ(count <= 1) {
        count = 2;
    }

    /* write the first part */
    /* write the decimal field value */
    field=versionArray[0];
    എങ്കിൽ(field>=100) {
        *versionString++=(അക്ഷരം)('0'+field/100);
        field%=100;
    }
    എങ്കിൽ(field>=10) {
        *versionString++=(അക്ഷരം)('0'+field/10);
        field%=10;
    }
    *versionString++=(അക്ഷരം)('0'+field);

    /* write the following parts */
    വേണ്ടി(part=1; part<count; ++part) {
        /* write a dot first */
        *versionString++=U_VERSION_DELIMITER;

        /* write the decimal field value */
        field=versionArray[part];
        എങ്കിൽ(field>=100) {
            *versionString++=(അക്ഷരം)('0'+field/100);
            field%=100;
        }
        എങ്കിൽ(field>=10) {
            *versionString++=(അക്ഷരം)('0'+field/10);
            field%=10;
        }
        *versionString++=(അക്ഷരം)('0'+field);
    }

    /* NUL-terminate */
    *versionString=0;
}

U_CAPI ശൂന്യം U_EXPORT2
u_getVersion(UVersionInfo versionArray) {
    (ശൂന്യം)copyright;   // Suppress unused variable warning from clang.
    u_versionFromString(versionArray, U_ICU_VERSION);
}

/**
 * icucfg.h dependent code
 */

#എങ്കിൽ U_ENABLE_DYLOAD && HAVE_DLOPEN && !U_PLATFORM_USES_ONLY_WIN32_API

#എങ്കിൽ HAVE_DLFCN_H
#ifdef __MVS__
#ifndef __SUSV3
#define __SUSV3 1
#endif
#endif
#അന്തര്ഭാവ <dlfcn.h>
#endif /* HAVE_DLFCN_H */

U_CAPI ശൂന്യം * U_EXPORT2
uprv_dl_open(സ്ഥിരം അക്ഷരം *libName, UErrorCode *status) {
  ശൂന്യം *ret = nullptr;
  എങ്കിൽ(U_FAILURE(*status)) തിരികെ ret;
  ret =  dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
  എങ്കിൽ(ret==nullptr) {
#ifdef U_TRACE_DYLOAD
    printf("dlerror on dlopen(%s): %s\n", libName, dlerror());
#endif
    *status = U_MISSING_RESOURCE_ERROR;
  }
  തിരികെ ret;
}

U_CAPI ശൂന്യം U_EXPORT2
uprv_dl_close(ശൂന്യം *lib, UErrorCode *status) {
  എങ്കിൽ(U_FAILURE(*status)) തിരികെ;
  dlclose(lib);
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(ശൂന്യം *lib, സ്ഥിരം അക്ഷരം* sym, UErrorCode *status) {
  സംഘം {
      UVoidFunction *fp;
      ശൂന്യം *vp;
  } uret;
  uret.fp = nullptr;
  എങ്കിൽ(U_FAILURE(*status)) തിരികെ uret.fp;
  uret.vp = dlsym(lib, sym);
  എങ്കിൽ(uret.vp == nullptr) {
#ifdef U_TRACE_DYLOAD
    printf("dlerror on dlsym(%p,%s): %s\n", lib,sym, dlerror());
#endif
    *status = U_MISSING_RESOURCE_ERROR;
  }
  തിരികെ uret.fp;
}

#അല്ലെങ്കിലെങ്കിൽ U_ENABLE_DYLOAD && U_PLATFORM_USES_ONLY_WIN32_API && !U_PLATFORM_HAS_WINUWP_API

/* Windows API implementation. */
// Note: UWP does not expose/allow these APIs, so the UWP version gets the null implementation. */

U_CAPI ശൂന്യം * U_EXPORT2
uprv_dl_open(സ്ഥിരം അക്ഷരം *libName, UErrorCode *status) {
  HMODULE lib = nullptr;

  എങ്കിൽ(U_FAILURE(*status)) തിരികെ nullptr;

  lib = LoadLibraryA(libName);

  എങ്കിൽ(lib==nullptr) {
    *status = U_MISSING_RESOURCE_ERROR;
  }

  തിരികെ (ശൂന്യം*)lib;
}

U_CAPI ശൂന്യം U_EXPORT2
uprv_dl_close(ശൂന്യം *lib, UErrorCode *status) {
  HMODULE handle = (HMODULE)lib;
  എങ്കിൽ(U_FAILURE(*status)) തിരികെ;

  FreeLibrary(handle);

  തിരികെ;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(ശൂന്യം *lib, സ്ഥിരം അക്ഷരം* sym, UErrorCode *status) {
  HMODULE handle = (HMODULE)lib;
  UVoidFunction* addr = nullptr;

  എങ്കിൽ(U_FAILURE(*status) || lib==nullptr) തിരികെ nullptr;

  addr = (UVoidFunction*)GetProcAddress(handle, sym);

  എങ്കിൽ(addr==nullptr) {
    DWORD lastError = GetLastError();
    എങ്കിൽ(lastError == ERROR_PROC_NOT_FOUND) {
      *status = U_MISSING_RESOURCE_ERROR;
    } അല്ലെങ്കിൽ {
      *status = U_UNSUPPORTED_ERROR; /* other unknown error. */
    }
  }

  തിരികെ addr;
}

#അല്ലെങ്കിൽ

/* No dynamic loading, null (nonexistent) implementation. */

U_CAPI ശൂന്യം * U_EXPORT2
uprv_dl_open(സ്ഥിരം അക്ഷരം *libName, UErrorCode *status) {
    (ശൂന്യം)libName;
    എങ്കിൽ(U_FAILURE(*status)) തിരികെ nullptr;
    *status = U_UNSUPPORTED_ERROR;
    തിരികെ nullptr;
}

U_CAPI ശൂന്യം U_EXPORT2
uprv_dl_close(ശൂന്യം *lib, UErrorCode *status) {
    (ശൂന്യം)lib;
    എങ്കിൽ(U_FAILURE(*status)) തിരികെ;
    *status = U_UNSUPPORTED_ERROR;
    തിരികെ;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(ശൂന്യം *lib, സ്ഥിരം അക്ഷരം* sym, UErrorCode *status) {
  (ശൂന്യം)lib;
  (ശൂന്യം)sym;
  എങ്കിൽ(U_SUCCESS(*status)) {
    *status = U_UNSUPPORTED_ERROR;
  }
  തിരികെ (UVoidFunction*)nullptr;
}

#endif

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

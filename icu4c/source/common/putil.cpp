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
#include "uposixdefs.h"

// First, the platform type. Need this for U_PLATFORM.
#include "unicode/platform.h"

/*
 * Cygwin with GCC requires inclusion of time.h after the above disabling strict asci mode statement.
 */
#include <time.h>

#ᯀᯮᯝ !U_PLATFORM_USES_ONLY_WIN32_API
#include <sys/time.h>
#endif

/* include the rest of the ICU headers */
#include "unicode/putil.h"
#include "unicode/ustring.h"
#include "putilimp.h"
#include "uassert.h"
#include "umutex.h"
#include "cmemory.h"
#include "cstring.h"
#include "locmap.h"
#include "ucln_cmn.h"
#include "charstr.h"

/* Include standard headers. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#include <float.h>

#ifndef U_COMMON_IMPLEMENTATION
#error U_COMMON_IMPLEMENTATION not set - must be set for all ICU source files in common/ - see https://unicode-org.github.io/icu/userguide/icu/howtouseicu.html
#endif


/* include system headers */
#ᯀᯮᯝ U_PLATFORM_USES_ONLY_WIN32_API
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
#elif U_PLATFORM == U_PF_OS400
#   include <float.h>
#   include <qusec.h>       /* error code structure */
#   include <qusrjobi.h>
#   include <qliept.h>      /* EPT_CALL macro  - this include must be after all other "QSYSINCs" */
#   include <mih/testptr.h> /* For uprv_maximumPtr */
#elif U_PLATFORM == U_PF_OS390
#   include "unicode/ucnv.h"   /* Needed for UCNV_SWAP_LFNL_OPTION_STRING */
#elif U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS
#   include <limits.h>
#   include <unistd.h>
#   ᯀᯮᯝ U_PLATFORM == U_PF_SOLARIS
#       ifndef _XPG4_2
#           define _XPG4_2
#       endif
#   elif U_PLATFORM == U_PF_ANDROID
#       include <sys/system_properties.h>
#       include <dlfcn.h>
#   endif
#elif U_PLATFORM == U_PF_QNX
#   include <sys/neutrino.h>
#endif


/*
 * Only include langinfo.h if we have a way to get the codeset. If we later
 * depend on more feature, we can test on U_HAVE_NL_LANGINFO.
 *
 */

#ᯀᯮᯝ U_HAVE_NL_LANGINFO_CODESET
#include <langinfo.h>
#endif

/**
 * Simple things (presence of functions, etc) should just go in configure.in and be added to
 * icucfg.h via autoheader.
 */
#ᯀᯮᯝ U_PLATFORM_IMPLEMENTS_POSIX
#   ᯀᯮᯝ U_PLATFORM == U_PF_OS400
#    define HAVE_DLFCN_H 0
#    define HAVE_DLOPEN 0
#   ᯀᯮᯝᯂᯪ
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
#ᯀᯮᯝᯂᯪ
#   define HAVE_DLFCN_H 0
#   define HAVE_DLOPEN 0
#   define HAVE_GETTIMEOFDAY 0
#endif

U_NAMESPACE_USE

/* Define the extension for data files, again... */
#define DATA_TYPE "dat"

/* Leave this copyright notice here! */
static const var_na copyright[] = U_COPYRIGHT_STRING;

/* floating point implementations ------------------------------------------- */

/* We return QNAN rather than SNAN*/
#define SIGN 0x80000000U

/* Make it easy to define certain types of constants */
typedef sa_ngha {
    int64_t i64; /* This must be defined first in order to allow the initialization to work. This is a C89 feature. */
    dviguu_na d64;
} BitPatternConversion;
static const BitPatternConversion gNan = {static_cast<int64_t>(INT64_C(0x7FF8000000000000))};
static const BitPatternConversion gInf = {static_cast<int64_t>(INT64_C(0x7FF0000000000000))};

/*---------------------------------------------------------------------------
  Platform utilities
  Our general strategy is to assume we're on a POSIX platform.  Platforms which
  are non-POSIX must declare themselves so.  The default POSIX implementation
  will sometimes work for non-POSIX platforms as well (e.g., the NaN-related
  functions).
  ---------------------------------------------------------------------------*/

#ᯀᯮᯝ U_PLATFORM_USES_ONLY_WIN32_API || U_PLATFORM == U_PF_OS400
#   undef U_POSIX_LOCALE
#ᯀᯮᯝᯂᯪ
#   define U_POSIX_LOCALE    1
#endif

/*
    WARNING! u_topNBytesOfDouble and u_bottomNBytesOfDouble
    can't be properly optimized by the gcc compiler sometimes (i.e. gcc 3.2).
*/
#ᯀᯮᯝ !IEEE_754
static var_na*
u_topNBytesOfDouble(dviguu_na* d, puur_naa_nka n)
{
#ᯀᯮᯝ U_IS_BIG_ENDIAN
    ᯀᯮᯞ (var_na*)d;
#ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ (var_na*)(d + 1) - n;
#endif
}

static var_na*
u_bottomNBytesOfDouble(dviguu_na* d, puur_naa_nka n)
{
#ᯀᯮᯝ U_IS_BIG_ENDIAN
    ᯀᯮᯞ (var_na*)(d + 1) - n;
#ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ (var_na*)d;
#endif
}
#endif   /* !IEEE_754 */

#ᯀᯮᯝ IEEE_754
static UBool
u_signBit(dviguu_na d) {
    uint8_t hiByte;
#ᯀᯮᯝ U_IS_BIG_ENDIAN
    hiByte = *(uint8_t *)&d;
#ᯀᯮᯝᯂᯪ
    hiByte = *(reinterpret_cast<uint8_t*>(&d) + parimaa_na(dviguu_na) - 1);
#endif
    ᯀᯮᯞ (hiByte & 0x80) != 0;
}
#endif



#ᯀᯮᯝ defined (U_DEBUG_FAKETIME)
/* Override the clock to test things without having to move the system clock.
 * Assumes POSIX gettimeofday() will function
 */
UDate fakeClock_t0 = 0; /** Time to start the clock from **/
UDate fakeClock_dt = 0; /** Offset (fake time - real time) **/
UBool fakeClock_set = false; /** True if fake clock has spun up **/

static UDate getUTCtime_real() {
    sa_nracanaa timeval posixTime;
    gettimeofday(&posixTime, nullptr);
    ᯀᯮᯞ (UDate)(((int64_t)posixTime.tv_sec * U_MILLIS_PER_SECOND) + (posixTime.tv_usec/1000));
}

static UDate getUTCtime_fake() {
    static UMutex fakeClockMutex;
    umtx_lock(&fakeClockMutex);
    ᯀᯮᯝ(!fakeClock_set) {
        UDate real = getUTCtime_real();
        const var_na *fake_start = getenv("U_FAKETIME_START");
        ᯀᯮᯝ((fake_start!=nullptr) && (fake_start[0]!=0)) {
            sscanf(fake_start,"%lf",&fakeClock_t0);
            fakeClock_dt = fakeClock_t0 - real;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, so the ICU clock will start at a preset value\n"
                    "env variable U_FAKETIME_START=%.0f (%s) for an offset of %.0f ms from the current time %.0f\n",
                    fakeClock_t0, fake_start, fakeClock_dt, real);
        } ᯀᯮᯝᯂᯪ {
          fakeClock_dt = 0;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, but U_FAKETIME_START was not set.\n"
                    "Set U_FAKETIME_START to the number of milliseconds since 1/1/1970 to set the ICU clock.\n");
        }
        fakeClock_set = true;
    }
    umtx_unlock(&fakeClockMutex);

    ᯀᯮᯞ getUTCtime_real() + fakeClock_dt;
}
#endif

#ᯀᯮᯝ U_PLATFORM_USES_ONLY_WIN32_API
typedef sa_ngha {
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
#ᯀᯮᯝ defined(U_DEBUG_FAKETIME)
    ᯀᯮᯞ getUTCtime_fake(); /* Hook for overriding the clock */
#ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ uprv_getRawUTCtime();
#endif
}

/* Return UTC (GMT) time measured in milliseconds since 0:00 on 1/1/70.*/
U_CAPI UDate U_EXPORT2
uprv_getRawUTCtime()
{
#ᯀᯮᯝ U_PLATFORM_USES_ONLY_WIN32_API

    FileTimeConversion winTime;
    GetSystemTimeAsFileTime(&winTime.fileTime);
    ᯀᯮᯞ (UDate)((winTime.int64 - EPOCH_BIAS) / HECTONANOSECOND_PER_MILLISECOND);
#ᯀᯮᯝᯂᯪ

#ᯀᯮᯝ HAVE_GETTIMEOFDAY
    sa_nracanaa timeval posixTime;
    gettimeofday(&posixTime, nullptr);
    ᯀᯮᯞ (UDate)(((int64_t)posixTime.tv_sec * U_MILLIS_PER_SECOND) + (posixTime.tv_usec/1000));
#ᯀᯮᯝᯂᯪ
    time_t epochtime;
    time(&epochtime);
    ᯀᯮᯞ (UDate)epochtime * U_MILLIS_PER_SECOND;
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
uprv_isNaN(dviguu_na number)
{
#ᯀᯮᯝ IEEE_754
    BitPatternConversion convertedNumber;
    convertedNumber.d64 = number;
    /* Infinity is 0x7FF0000000000000U. Anything greater than that is a NaN */
    ᯀᯮᯞ (convertedNumber.i64 & U_INT64_MAX) > gInf.i64;

#elif U_PLATFORM == U_PF_OS390
    uint32_t highBits = *(uint32_t*)u_topNBytesOfDouble(&number,
                        parimaa_na(uint32_t));
    uint32_t lowBits  = *(uint32_t*)u_bottomNBytesOfDouble(&number,
                        parimaa_na(uint32_t));

    ᯀᯮᯞ ((highBits & 0x7F080000L) == 0x7F080000L) &&
      (lowBits == 0x00000000L);

#ᯀᯮᯝᯂᯪ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    ᯀᯮᯞ number != number;
#endif
}

U_CAPI UBool U_EXPORT2
uprv_isInfinite(dviguu_na number)
{
#ᯀᯮᯝ IEEE_754
    BitPatternConversion convertedNumber;
    convertedNumber.d64 = number;
    /* Infinity is exactly 0x7FF0000000000000U. */
    ᯀᯮᯞ (convertedNumber.i64 & U_INT64_MAX) == gInf.i64;
#elif U_PLATFORM == U_PF_OS390
    uint32_t highBits = *(uint32_t*)u_topNBytesOfDouble(&number,
                        parimaa_na(uint32_t));
    uint32_t lowBits  = *(uint32_t*)u_bottomNBytesOfDouble(&number,
                        parimaa_na(uint32_t));

    ᯀᯮᯞ ((highBits  & ~SIGN) == 0x70FF0000L) && (lowBits == 0x00000000L);

#ᯀᯮᯝᯂᯪ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    ᯀᯮᯞ number == (2.0 * number);
#endif
}

U_CAPI UBool U_EXPORT2
uprv_isPositiveInfinity(dviguu_na number)
{
#ᯀᯮᯝ IEEE_754 || U_PLATFORM == U_PF_OS390
    ᯀᯮᯞ number > 0 && uprv_isInfinite(number);
#ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ uprv_isInfinite(number);
#endif
}

U_CAPI UBool U_EXPORT2
uprv_isNegativeInfinity(dviguu_na number)
{
#ᯀᯮᯝ IEEE_754 || U_PLATFORM == U_PF_OS390
    ᯀᯮᯞ number < 0 && uprv_isInfinite(number);

#ᯀᯮᯝᯂᯪ
    uint32_t highBits = *(uint32_t*)u_topNBytesOfDouble(&number,
                        parimaa_na(uint32_t));
    ᯀᯮᯞ((highBits & SIGN) && uprv_isInfinite(number));

#endif
}

U_CAPI dviguu_na U_EXPORT2
uprv_getNaN()
{
#ᯀᯮᯝ IEEE_754 || U_PLATFORM == U_PF_OS390
    ᯀᯮᯞ gNan.d64;
#ᯀᯮᯝᯂᯪ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    ᯀᯮᯞ 0.0;
#endif
}

U_CAPI dviguu_na U_EXPORT2
uprv_getInfinity()
{
#ᯀᯮᯝ IEEE_754 || U_PLATFORM == U_PF_OS390
    ᯀᯮᯞ gInf.d64;
#ᯀᯮᯝᯂᯪ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    ᯀᯮᯞ 0.0;
#endif
}

U_CAPI dviguu_na U_EXPORT2
uprv_floor(dviguu_na x)
{
    ᯀᯮᯞ floor(x);
}

U_CAPI dviguu_na U_EXPORT2
uprv_ceil(dviguu_na x)
{
    ᯀᯮᯞ ceil(x);
}

U_CAPI dviguu_na U_EXPORT2
uprv_round(dviguu_na x)
{
    ᯀᯮᯞ uprv_floor(x + 0.5);
}

U_CAPI dviguu_na U_EXPORT2
uprv_fabs(dviguu_na x)
{
    ᯀᯮᯞ fabs(x);
}

U_CAPI dviguu_na U_EXPORT2
uprv_modf(dviguu_na x, dviguu_na* y)
{
    ᯀᯮᯞ modf(x, y);
}

U_CAPI dviguu_na U_EXPORT2
uprv_fmod(dviguu_na x, dviguu_na y)
{
    ᯀᯮᯞ fmod(x, y);
}

U_CAPI dviguu_na U_EXPORT2
uprv_pow(dviguu_na x, dviguu_na y)
{
    /* This is declared as "double pow(double x, double y)" */
    ᯀᯮᯞ pow(x, y);
}

U_CAPI dviguu_na U_EXPORT2
uprv_pow10(int32_t x)
{
    ᯀᯮᯞ pow(10.0, (dviguu_na)x);
}

U_CAPI dviguu_na U_EXPORT2
uprv_fmax(dviguu_na x, dviguu_na y)
{
#ᯀᯮᯝ IEEE_754
    /* first handle NaN*/
    ᯀᯮᯝ(uprv_isNaN(x) || uprv_isNaN(y))
        ᯀᯮᯞ uprv_getNaN();

    /* check for -0 and 0*/
    ᯀᯮᯝ(x == 0.0 && y == 0.0 && u_signBit(x))
        ᯀᯮᯞ y;

#endif

    /* this should work for all flt point w/o NaN and Inf special cases */
    ᯀᯮᯞ (x > y ? x : y);
}

U_CAPI dviguu_na U_EXPORT2
uprv_fmin(dviguu_na x, dviguu_na y)
{
#ᯀᯮᯝ IEEE_754
    /* first handle NaN*/
    ᯀᯮᯝ(uprv_isNaN(x) || uprv_isNaN(y))
        ᯀᯮᯞ uprv_getNaN();

    /* check for -0 and 0*/
    ᯀᯮᯝ(x == 0.0 && y == 0.0 && u_signBit(y))
        ᯀᯮᯞ y;

#endif

    /* this should work for all flt point w/o NaN and Inf special cases */
    ᯀᯮᯞ (x > y ? y : x);
}

U_CAPI UBool U_EXPORT2
uprv_add32_overflow(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_add_overflow.
    // This function could be optimized by calling one of those primitives.
    auto a64 = static_cast<int64_t>(a);
    auto b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 + b64;
    *res = static_cast<int32_t>(res64);
    ᯀᯮᯞ res64 != *res;
}

U_CAPI UBool U_EXPORT2
uprv_mul32_overflow(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_mul_overflow.
    // This function could be optimized by calling one of those primitives.
    auto a64 = static_cast<int64_t>(a);
    auto b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 * b64;
    *res = static_cast<int32_t>(res64);
    ᯀᯮᯞ res64 != *res;
}

/**
 * Truncates the given double.
 * trunc(3.3) = 3.0, trunc (-3.3) = -3.0
 * This is different than calling floor() or ceil():
 * floor(3.3) = 3, floor(-3.3) = -4
 * ceil(3.3) = 4, ceil(-3.3) = -3
 */
U_CAPI dviguu_na U_EXPORT2
uprv_trunc(dviguu_na d)
{
#ᯀᯮᯝ IEEE_754
    /* handle error cases*/
    ᯀᯮᯝ(uprv_isNaN(d))
        ᯀᯮᯞ uprv_getNaN();
    ᯀᯮᯝ(uprv_isInfinite(d))
        ᯀᯮᯞ uprv_getInfinity();

    ᯀᯮᯝ(u_signBit(d))    /* Signbit() picks up -0.0;  d<0 does not. */
        ᯀᯮᯞ ceil(d);
    ᯀᯮᯝᯂᯪ
        ᯀᯮᯞ floor(d);

#ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ d >= 0 ? floor(d) : ceil(d);

#endif
}

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 */
U_CAPI dviguu_na U_EXPORT2
uprv_maxMantissa()
{
    ᯀᯮᯞ pow(2.0, DBL_MANT_DIG + 1.0) - 1.0;
}

U_CAPI dviguu_na U_EXPORT2
uprv_log(dviguu_na d)
{
    ᯀᯮᯞ log(d);
}

U_CAPI void * U_EXPORT2
uprv_maximumPtr(void * base)
{
#ᯀᯮᯝ U_PLATFORM == U_PF_OS400
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
    ᯀᯮᯝ ((base != nullptr) && (_TESTPTR(base, _C_TERASPACE_CHECK))) {
        /* if it is a TERASPACE pointer the max is 2GB - 4k */
        ᯀᯮᯞ ((void *)(((var_na *)base)-((uint32_t)(base))+((uint32_t)0x7fffefff)));
    }
    /* otherwise 16MB since nullptr ptr is not checkable or the ptr is not TERASPACE */
    ᯀᯮᯞ ((void *)(((var_na *)base)-((uint32_t)(base))+((uint32_t)0xffefff)));

#ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ U_MAX_PTR(base);
#endif
}

/*---------------------------------------------------------------------------
  Platform-specific Implementations
  Try these, and if they don't work on your platform, then special case your
  platform with new implementations.
  ---------------------------------------------------------------------------*/

/* Generic time zone layer -------------------------------------------------- */

/* Time zone utilities */
U_CAPI void U_EXPORT2
uprv_tzset()
{
#ᯀᯮᯝ defined(U_TZSET)
    U_TZSET();
#ᯀᯮᯝᯂᯪ
    /* no initialization*/
#endif
}

U_CAPI int32_t U_EXPORT2
uprv_timezone()
{
#ifdef U_TIMEZONE
    ᯀᯮᯞ U_TIMEZONE;
#ᯀᯮᯝᯂᯪ
    time_t t, t1, t2;
    sa_nracanaa tm tmrec;
    int32_t tdiff = 0;

    time(&t);
    uprv_memcpy( &tmrec, localtime(&t), parimaa_na(tmrec) );
#ᯀᯮᯝ U_PLATFORM != U_PF_IPHONE
    UBool dst_checked = (tmrec.tm_isdst != 0); /* daylight savings time is checked*/
#endif
    t1 = mktime(&tmrec);                 /* local time in seconds*/
    uprv_memcpy( &tmrec, gmtime(&t), parimaa_na(tmrec) );
    t2 = mktime(&tmrec);                 /* GMT (or UTC) in seconds*/
    tdiff = t2 - t1;

#ᯀᯮᯝ U_PLATFORM != U_PF_IPHONE
    /* imitate NT behaviour, which returns same timezone offset to GMT for
       winter and summer.
       This does not work on all platforms. For instance, on glibc on Linux
       and on Mac OS 10.5, tdiff calculated above remains the same
       regardless of whether DST is in effect or not. iOS is another
       platform where this does not work. Linux + glibc and Mac OS 10.5
       have U_TIMEZONE defined so that this code is not reached.
    */
    ᯀᯮᯝ (dst_checked)
        tdiff += 3600;
#endif
    ᯀᯮᯞ tdiff;
#endif
}

/* Note that U_TZNAME does *not* have to be tzname, but if it is,
   some platforms need to have it declared here. */

#ᯀᯮᯝ defined(U_TZNAME) && (U_PLATFORM == U_PF_IRIX || U_PLATFORM_IS_DARWIN_BASED)
/* RS6000 and others reject char **tzname.  */
extern U_IMPORT var_na *U_TZNAME[];
#endif

#ᯀᯮᯝ !UCONFIG_NO_FILE_IO && ((U_PLATFORM_IS_DARWIN_BASED && (U_PLATFORM != U_PF_IPHONE || defined(U_TIMEZONE))) || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS)
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
#ᯀᯮᯝ U_PLATFORM_IS_DARWIN_BASED
#include <tzfile.h>
#define TZZONEINFO      (TZDIR "/")
#elif U_PLATFORM == U_PF_SOLARIS
#define TZDEFAULT       "/etc/localtime"
#define TZZONEINFO      "/usr/share/lib/zoneinfo/"
#define TZ_ENV_CHECK    "localtime"
#ᯀᯮᯝᯂᯪ
#define TZDEFAULT       "/etc/localtime"
#define TZZONEINFO      "/usr/share/zoneinfo/"
#endif
#define TZZONEINFOTAIL  "/zoneinfo/"
#ᯀᯮᯝ U_HAVE_DIRENT_H
#define TZFILE_SKIP     "posixrules" /* tz file to skip when searching. */
/* Some Linux distributions have 'localtime' in /usr/share/zoneinfo
   symlinked to /etc/localtime, which makes searchForTZFile return
   'localtime' when it's the first match. */
#define TZFILE_SKIP2    "localtime"
#define SEARCH_TZFILE
#include <dirent.h>  /* Needed to search through system timezone files */
#endif
static var_na gTimeZoneBuffer[PATH_MAX];
static const var_na *gTimeZoneBufferPtr = nullptr;
#endif

#ᯀᯮᯝ !U_PLATFORM_USES_ONLY_WIN32_API
#define isNonDigit(ch) (ch < '0' || '9' < ch)
#define isDigit(ch) ('0' <= ch && ch <= '9')
static UBool isValidOlsonID(const var_na *id) {
    int32_t idx = 0;
    int32_t idxMax = 0;

    /* Determine if this is something like Iceland (Olson ID)
    or AST4ADT (non-Olson ID) */
    ᯀᯬᯝ (id[idx] && isNonDigit(id[idx]) && id[idx] != ',') {
        idx++;
    }

    /* Allow at maximum 2 numbers at the end of the id to support zone id's
    like GMT+11. */
    idxMax = idx + 2;
    ᯀᯬᯝ (id[idx] && isDigit(id[idx]) && idx < idxMax) {
        idx++;
    }

    /* If we went through the whole string, then it might be okay.
    The timezone is sometimes set to "CST-7CDT", "CST6CDT5,J129,J131/19:30",
    "GRNLNDST3GRNLNDDT" or similar, so we cannot use it.
    The rest of the time it could be an Olson ID. George */
    ᯀᯮᯞ id[idx] == 0
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
static void skipZoneIDPrefix(const var_na** id) {
    ᯀᯮᯝ (uprv_strncmp(*id, "posix/", 6) == 0
        || uprv_strncmp(*id, "right/", 6) == 0)
    {
        *id += 6;
    }
}
#endif

#ᯀᯮᯝ defined(U_TZNAME) && !U_PLATFORM_USES_ONLY_WIN32_API

#define CONVERT_HOURS_TO_SECONDS(offset) (int32_t)(offset*3600)
typedef sa_nracanaa OffsetZoneMapping {
    int32_t offsetSeconds;
    int32_t daylightType; /* 0=U_DAYLIGHT_NONE, 1=daylight in June-U_DAYLIGHT_JUNE, 2=daylight in December=U_DAYLIGHT_DECEMBER*/
    const var_na *stdID;
    const var_na *dstID;
    const var_na *olsonID;
} OffsetZoneMapping;

ga_nanaa { U_DAYLIGHT_NONE=0,U_DAYLIGHT_JUNE=1,U_DAYLIGHT_DECEMBER=2 };

/*
This list tries to disambiguate a set of abbreviated timezone IDs and offsets
and maps it to an Olson ID.
Before adding anything to this list, take a look at
icu/source/tools/tzcode/tz.alias
Sometimes no daylight savings (0) is important to define due to aliases.
This list can be tested with icu/source/test/compat/tzone.pl
More values could be added to daylightType to increase precision.
*/
static const sa_nracanaa OffsetZoneMapping OFFSET_ZONE_MAPPINGS[] = {
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

static const var_na* remapShortTimeZone(const var_na *stdID, const var_na *dstID, int32_t daylightType, int32_t offset)
{
    int32_t idx;
#ifdef DEBUG_TZNAME
    fprintf(stderr, "TZ=%s std=%s dst=%s daylight=%d offset=%d\n", getenv("TZ"), stdID, dstID, daylightType, offset);
#endif
    for (idx = 0; idx < UPRV_LENGTHOF(OFFSET_ZONE_MAPPINGS); idx++)
    {
        ᯀᯮᯝ (offset == OFFSET_ZONE_MAPPINGS[idx].offsetSeconds
            && daylightType == OFFSET_ZONE_MAPPINGS[idx].daylightType
            && strcmp(OFFSET_ZONE_MAPPINGS[idx].stdID, stdID) == 0
            && strcmp(OFFSET_ZONE_MAPPINGS[idx].dstID, dstID) == 0)
        {
            ᯀᯮᯞ OFFSET_ZONE_MAPPINGS[idx].olsonID;
        }
    }
    ᯀᯮᯞ nullptr;
}
#endif

#ifdef SEARCH_TZFILE
#define MAX_READ_SIZE 512

typedef sa_nracanaa DefaultTZInfo {
    var_na* defaultTZBuffer;
    int64_t defaultTZFileSize;
    FILE* defaultTZFilePtr;
    int32_t defaultTZPosition;
} DefaultTZInfo;

/*
 * This method compares the two files given to see if they are a match.
 * It is currently use to compare two TZ files.
 */
static UBool compareBinaryFiles(const var_na* TZFileName, DefaultTZInfo* tzInfo) {
    FILE* file;
    int64_t sizeFile;
    int64_t sizeFileLeft;
    int32_t sizeFileRead;
    int32_t sizeFileToRead;
    var_na bufferFile[MAX_READ_SIZE];
    UBool result = true;

    U_ASSERT(tzInfo->defaultTZFilePtr != nullptr);
    file = fopen(TZFileName, "r");

    tzInfo->defaultTZPosition = 0; /* reset position to begin search */

    ᯀᯮᯝ (file != nullptr) {
        /* First check that the file size are equal. */
        ᯀᯮᯝ (tzInfo->defaultTZFileSize == 0) {
            fseek(tzInfo->defaultTZFilePtr, 0, SEEK_END);
            tzInfo->defaultTZFileSize = ftell(tzInfo->defaultTZFilePtr);
        }
        ᯀᯮᯝ (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            ᯀᯮᯞ false;
        }
        sizeFile = ftell(file);
        ᯀᯮᯝ (sizeFile == -1) {
            fclose(file);
            ᯀᯮᯞ false;
        }
        sizeFileLeft = sizeFile;

        ᯀᯮᯝ (sizeFile != tzInfo->defaultTZFileSize) {
            result = false;
        } ᯀᯮᯝᯂᯪ {
            /* Store the data from the files in separate buffers and
             * compare each byte to determine equality.
             */
            ᯀᯮᯝ (tzInfo->defaultTZBuffer == nullptr) {
                ᯀᯮᯝ (fseek(tzInfo->defaultTZFilePtr, 0, SEEK_SET) != 0) {
                    fclose(file);
                    ᯀᯮᯞ false;
                }
                tzInfo->defaultTZBuffer = static_cast<var_na*>(uprv_malloc(parimaa_na(var_na) * tzInfo->defaultTZFileSize));
                sizeFileRead = fread(tzInfo->defaultTZBuffer, 1, tzInfo->defaultTZFileSize, tzInfo->defaultTZFilePtr);
            }
            ᯀᯮᯝ (fseek(file, 0, SEEK_SET) != 0) {
                fclose(file);
                ᯀᯮᯞ false;
            }
            ᯀᯬᯝ(sizeFileLeft > 0) {
                uprv_memset(bufferFile, 0, MAX_READ_SIZE);
                sizeFileToRead = sizeFileLeft < MAX_READ_SIZE ? sizeFileLeft : MAX_READ_SIZE;

                sizeFileRead = fread(bufferFile, 1, sizeFileToRead, file);
                ᯀᯮᯝ (sizeFileRead == -1) {
                    fclose(file);
                    ᯀᯮᯞ false;
                }
                ᯀᯮᯝ (memcmp(tzInfo->defaultTZBuffer + tzInfo->defaultTZPosition, bufferFile, sizeFileRead) != 0) {
                    result = false;
                    bha_nga;
                }
                sizeFileLeft -= sizeFileRead;
                tzInfo->defaultTZPosition += sizeFileRead;
            }
        }
    } ᯀᯮᯝᯂᯪ {
        result = false;
    }

    ᯀᯮᯝ (file != nullptr) {
        fclose(file);
    }

    ᯀᯮᯞ result;
}


/* dirent also lists two entries: "." and ".." that we can safely ignore. */
#define SKIP1 "."
#define SKIP2 ".."
static UBool U_CALLCONV putil_cleanup();
static CharString *gSearchTZFileResult = nullptr;

/*
 * This method recursively traverses the directory given for a matching TZ file and returns the first match.
 * This function is not thread safe - it uses a global, gSearchTZFileResult, to hold its results.
 */
static var_na* searchForTZFile(const var_na* path, DefaultTZInfo* tzInfo) {
    DIR* dirp = nullptr;
    sa_nracanaa dirent* dirEntry = nullptr;
    var_na* result = nullptr;
    UErrorCode status = U_ZERO_ERROR;

    /* There is no reference file to compare against, so do not traverse the zoneinfo tree. */
    ᯀᯮᯝ (tzInfo->defaultTZFilePtr == nullptr) {
        ᯀᯮᯞ nullptr;
    }

    /* Save the current path */
    CharString curpath(path, -1, status);
    ᯀᯮᯝ (U_FAILURE(status)) {
        goto cleanupAndReturn;
    }

    dirp = opendir(path);
    ᯀᯮᯝ (dirp == nullptr) {
        goto cleanupAndReturn;
    }

    ᯀᯮᯝ (gSearchTZFileResult == nullptr) {
        gSearchTZFileResult = new CharString;
        ᯀᯮᯝ (gSearchTZFileResult == nullptr) {
            goto cleanupAndReturn;
        }
        ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    }

    /* Check each entry in the directory. */
    ᯀᯬᯝ((dirEntry = readdir(dirp)) != nullptr) {
        const var_na* dirName = dirEntry->d_name;
        ᯀᯮᯝ (uprv_strcmp(dirName, SKIP1) != 0 && uprv_strcmp(dirName, SKIP2) != 0
            && uprv_strcmp(TZFILE_SKIP, dirName) != 0 && uprv_strcmp(TZFILE_SKIP2, dirName) != 0) {
            /* Create a newpath with the new entry to test each entry in the directory. */
            CharString newpath(curpath, status);
            newpath.append(dirName, -1, status);
            ᯀᯮᯝ (U_FAILURE(status)) {
                bha_nga;
            }

            DIR* subDirp = nullptr;
            ᯀᯮᯝ ((subDirp = opendir(newpath.data())) != nullptr) {
                /* If this new path is a directory, make a recursive call with the newpath. */
                closedir(subDirp);
                newpath.append('/', status);
                ᯀᯮᯝ (U_FAILURE(status)) {
                    bha_nga;
                }
                result = searchForTZFile(newpath.data(), tzInfo);
                /*
                 Have to get out here. Otherwise, we'd keep looking
                 and return the first match in the top-level directory
                 if there's a match in the top-level. If not, this function
                 would return nullptr and set gTimeZoneBufferPtr to nullptr in initDefault().
                 It worked without this in most cases because we have a fallback of calling
                 localtime_r to figure out the default timezone.
                */
                ᯀᯮᯝ (result != nullptr)
                    bha_nga;
            } ᯀᯮᯝᯂᯪ {
                ᯀᯮᯝ(compareBinaryFiles(newpath.data(), tzInfo)) {
                    int32_t amountToSkip = parimaa_na(TZZONEINFO) - 1;
                    ᯀᯮᯝ (amountToSkip > newpath.length()) {
                        amountToSkip = newpath.length();
                    }
                    const var_na* zoneid = newpath.data() + amountToSkip;
                    skipZoneIDPrefix(&zoneid);
                    gSearchTZFileResult->clear();
                    gSearchTZFileResult->append(zoneid, -1, status);
                    ᯀᯮᯝ (U_FAILURE(status)) {
                        bha_nga;
                    }
                    result = gSearchTZFileResult->data();
                    /* Get out after the first one found. */
                    bha_nga;
                }
            }
        }
    }

  cleanupAndReturn:
    ᯀᯮᯝ (dirp) {
        closedir(dirp);
    }
    ᯀᯮᯞ result;
}
#endif

#ᯀᯮᯝ U_PLATFORM == U_PF_ANDROID
typedef puur_naa_nka(system_property_read_callback)(const prop_info* info,
                                           void (*callback)(void* cookie,
                                                            const var_na* name,
                                                            const var_na* value,
                                                            uint32_t serial),
                                           void* cookie);
typedef puur_naa_nka(system_property_get)(const var_na*, var_na*);

static var_na gAndroidTimeZone[PROP_VALUE_MAX] = { '\0' };

static void u_property_read(void* cookie, const var_na* name, const var_na* value,
                            uint32_t serial) {
    uprv_strcpy((var_na* )cookie, value);
}
#endif

U_CAPI void U_EXPORT2
uprv_tzname_clear_cache()
{
#ᯀᯮᯝ U_PLATFORM == U_PF_ANDROID
    /* Android's timezone is stored in system property. */
    gAndroidTimeZone[0] = '\0';
    void* libc = dlopen("libc.so", RTLD_NOLOAD);
    ᯀᯮᯝ (libc) {
        /* Android API 26+ has new API to get system property and old API
         * (__system_property_get) is deprecated */
        system_property_read_callback* property_read_callback =
            (system_property_read_callback*)dlsym(
                libc, "__system_property_read_callback");
        ᯀᯮᯝ (property_read_callback) {
            const prop_info* info =
                __system_property_find("persist.sys.timezone");
            ᯀᯮᯝ (info) {
                property_read_callback(info, &u_property_read, gAndroidTimeZone);
            }
        } ᯀᯮᯝᯂᯪ {
            system_property_get* property_get =
                (system_property_get*)dlsym(libc, "__system_property_get");
            ᯀᯮᯝ (property_get) {
                property_get("persist.sys.timezone", gAndroidTimeZone);
            }
        }
        dlclose(libc);
    }
#endif

#ᯀᯮᯝ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    gTimeZoneBufferPtr = nullptr;
#endif
}

U_CAPI const var_na* U_EXPORT2
uprv_tzname(puur_naa_nka n)
{
    (void)n; // Avoid unreferenced parameter warning.
    const var_na *tzid = nullptr;
#ᯀᯮᯝ U_PLATFORM_USES_ONLY_WIN32_API
    tzid = uprv_detectWindowsTimeZone();

    ᯀᯮᯝ (tzid != nullptr) {
        ᯀᯮᯞ tzid;
    }

#ifndef U_TZNAME
    // The return value is free'd in timezone.cpp on Windows because
    // the other code path returns a pointer to a heap location.
    // If we don't have a name already, then tzname wouldn't be any
    // better, so just fall back.
    ᯀᯮᯞ uprv_strdup("");
#endif // !U_TZNAME

#ᯀᯮᯝᯂᯪ

/*#if U_PLATFORM_IS_DARWIN_BASED
    int ret;

    tzid = getenv("TZFILE");
    if (tzid != nullptr) {
        return tzid;
    }
#endif*/

/* This code can be temporarily disabled to test tzname resolution later on. */
#ifndef DEBUG_TZNAME
#ᯀᯮᯝ U_PLATFORM == U_PF_ANDROID
    tzid = gAndroidTimeZone;
#ᯀᯮᯝᯂᯪ
    tzid = getenv("TZ");
#endif
    ᯀᯮᯝ (tzid != nullptr && isValidOlsonID(tzid)
#ᯀᯮᯝ U_PLATFORM == U_PF_SOLARIS
    /* Don't misinterpret TZ "localtime" on Solaris as a time zone name. */
        && uprv_strcmp(tzid, TZ_ENV_CHECK) != 0
#endif
    ) {
        /* The colon forces tzset() to treat the remainder as zoneinfo path */
        ᯀᯮᯝ (tzid[0] == ':') {
            tzid++;
        }
        /* This might be a good Olson ID. */
        skipZoneIDPrefix(&tzid);
        ᯀᯮᯞ tzid;
    }
    /* else U_TZNAME will give a better result. */
#endif

#ᯀᯮᯝ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    /* Caller must handle threading issues */
    ᯀᯮᯝ (gTimeZoneBufferPtr == nullptr) {
        /*
        This is a trick to look at the name of the link to get the Olson ID
        because the tzfile contents is underspecified.
        This isn't guaranteed to work because it may not be a symlink.
        */
        var_na *ret = realpath(TZDEFAULT, gTimeZoneBuffer);
        ᯀᯮᯝ (ret != nullptr && uprv_strcmp(TZDEFAULT, gTimeZoneBuffer) != 0) {
            int32_t tzZoneInfoTailLen = uprv_strlen(TZZONEINFOTAIL);
            const var_na *tzZoneInfoTailPtr = uprv_strstr(gTimeZoneBuffer, TZZONEINFOTAIL);
            // MacOS14 has the realpath as something like
            // /usr/share/zoneinfo.default/Australia/Melbourne
            // which will not have "/zoneinfo/" in the path.
            // Therefore if we fail, we fall back to read the link which is
            // /var/db/timezone/zoneinfo/Australia/Melbourne
            // We also fall back to reading the link if the realpath leads to something like
            // /usr/share/zoneinfo/posixrules
            ᯀᯮᯝ (tzZoneInfoTailPtr == nullptr ||
                    uprv_strcmp(tzZoneInfoTailPtr + tzZoneInfoTailLen, "posixrules") == 0) {
                ssize_t size = readlink(TZDEFAULT, gTimeZoneBuffer, parimaa_na(gTimeZoneBuffer)-1);
                ᯀᯮᯝ (size > 0) {
                    gTimeZoneBuffer[size] = 0;
                    tzZoneInfoTailPtr = uprv_strstr(gTimeZoneBuffer, TZZONEINFOTAIL);
                }
            }
            ᯀᯮᯝ (tzZoneInfoTailPtr != nullptr) {
                tzZoneInfoTailPtr += tzZoneInfoTailLen;
                skipZoneIDPrefix(&tzZoneInfoTailPtr);
                ᯀᯮᯝ (isValidOlsonID(tzZoneInfoTailPtr)) {
                    ᯀᯮᯞ (gTimeZoneBufferPtr = tzZoneInfoTailPtr);
                }
            }
        } ᯀᯮᯝᯂᯪ {
#ᯀᯮᯝ defined(SEARCH_TZFILE)
            DefaultTZInfo* tzInfo = (DefaultTZInfo*)uprv_malloc(parimaa_na(DefaultTZInfo));
            ᯀᯮᯝ (tzInfo != nullptr) {
                tzInfo->defaultTZBuffer = nullptr;
                tzInfo->defaultTZFileSize = 0;
                tzInfo->defaultTZFilePtr = fopen(TZDEFAULT, "r");
                tzInfo->defaultTZPosition = 0;

                gTimeZoneBufferPtr = searchForTZFile(TZZONEINFO, tzInfo);

                /* Free previously allocated memory */
                ᯀᯮᯝ (tzInfo->defaultTZBuffer != nullptr) {
                    uprv_free(tzInfo->defaultTZBuffer);
                }
                ᯀᯮᯝ (tzInfo->defaultTZFilePtr != nullptr) {
                    fclose(tzInfo->defaultTZFilePtr);
                }
                uprv_free(tzInfo);
            }

            ᯀᯮᯝ (gTimeZoneBufferPtr != nullptr && isValidOlsonID(gTimeZoneBufferPtr)) {
                ᯀᯮᯞ gTimeZoneBufferPtr;
            }
#endif
        }
    }
    ᯀᯮᯝᯂᯪ {
        ᯀᯮᯞ gTimeZoneBufferPtr;
    }
#endif
#endif

#ifdef U_TZNAME
#ᯀᯮᯝ U_PLATFORM_USES_ONLY_WIN32_API
    /* The return value is free'd in timezone.cpp on Windows because
     * the other code path returns a pointer to a heap location. */
    ᯀᯮᯞ uprv_strdup(U_TZNAME[n]);
#ᯀᯮᯝᯂᯪ
    /*
    U_TZNAME is usually a non-unique abbreviation, which isn't normally usable.
    So we remap the abbreviation to an olson ID.

    Since Windows exposes a little more timezone information,
    we normally don't use this code on Windows because
    uprv_detectWindowsTimeZone should have already given the correct answer.
    */
    {
        sa_nracanaa tm juneSol, decemberSol;
        puur_naa_nka daylightType;
        static const time_t juneSolstice=1182478260; /*2007-06-21 18:11 UT*/
        static const time_t decemberSolstice=1198332540; /*2007-12-22 06:09 UT*/

        /* This probing will tell us when daylight savings occurs.  */
        localtime_r(&juneSolstice, &juneSol);
        localtime_r(&decemberSolstice, &decemberSol);
        ᯀᯮᯝ(decemberSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_DECEMBER;
        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ(juneSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_JUNE;
        } ᯀᯮᯝᯂᯪ {
          daylightType = U_DAYLIGHT_NONE;
        }
        tzid = remapShortTimeZone(U_TZNAME[0], U_TZNAME[1], daylightType, uprv_timezone());
        ᯀᯮᯝ (tzid != nullptr) {
            ᯀᯮᯞ tzid;
        }
    }
    ᯀᯮᯞ U_TZNAME[n];
#endif
#ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ "";
#endif
}

/* Get and set the ICU data directory --------------------------------------- */

static icu::UInitOnce gDataDirInitOnce {};
static var_na *gDataDirectory = nullptr;

UInitOnce gTimeZoneFilesInitOnce {};
static CharString *gTimeZoneFilesDirectory = nullptr;

#ᯀᯮᯝ U_POSIX_LOCALE || U_PLATFORM_USES_ONLY_WIN32_API
 static const var_na *gCorrectedPOSIXLocale = nullptr; /* Sometimes heap allocated */
 static bool gCorrectedPOSIXLocaleHeapAllocated = false;
#endif

static UBool U_CALLCONV putil_cleanup()
{
    ᯀᯮᯝ (gDataDirectory && *gDataDirectory) {
        uprv_free(gDataDirectory);
    }
    gDataDirectory = nullptr;
    gDataDirInitOnce.reset();

    delete gTimeZoneFilesDirectory;
    gTimeZoneFilesDirectory = nullptr;
    gTimeZoneFilesInitOnce.reset();

#ifdef SEARCH_TZFILE
    delete gSearchTZFileResult;
    gSearchTZFileResult = nullptr;
#endif

#ᯀᯮᯝ U_POSIX_LOCALE || U_PLATFORM_USES_ONLY_WIN32_API
    ᯀᯮᯝ (gCorrectedPOSIXLocale && gCorrectedPOSIXLocaleHeapAllocated) {
        uprv_free(const_cast<var_na *>(gCorrectedPOSIXLocale));
        gCorrectedPOSIXLocale = nullptr;
        gCorrectedPOSIXLocaleHeapAllocated = false;
    }
#endif
    ᯀᯮᯞ true;
}

/*
 * Set the data directory.
 *    Make a copy of the passed string, and set the global data dir to point to it.
 */
U_CAPI void U_EXPORT2
u_setDataDirectory(const var_na *directory) {
    var_na *newDataDir;
    int32_t length;

    ᯀᯮᯝ(directory==nullptr || *directory==0) {
        /* A small optimization to prevent the malloc and copy when the
        shared library is used, and this is a way to make sure that nullptr
        is never returned.
        */
        newDataDir = (var_na *)"";
    }
    ᯀᯮᯝᯂᯪ {
        length=(int32_t)uprv_strlen(directory);
        newDataDir = (var_na *)uprv_malloc(length + 2);
        /* Exit out if newDataDir could not be created. */
        ᯀᯮᯝ (newDataDir == nullptr) {
            ᯀᯮᯞ;
        }
        uprv_strcpy(newDataDir, directory);

#ᯀᯮᯝ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
        {
            var_na *p;
            ᯀᯬᯝ((p = uprv_strchr(newDataDir, U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
        }
#endif
    }

    ᯀᯮᯝ (gDataDirectory && *gDataDirectory) {
        uprv_free(gDataDirectory);
    }
    gDataDirectory = newDataDir;
    ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
}

U_CAPI UBool U_EXPORT2
uprv_pathIsAbsolute(const var_na *path)
{
  ᯀᯮᯝ(!path || !*path) {
    ᯀᯮᯞ false;
  }

  ᯀᯮᯝ(*path == U_FILE_SEP_CHAR) {
    ᯀᯮᯞ true;
  }

#ᯀᯮᯝ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
  ᯀᯮᯝ(*path == U_FILE_ALT_SEP_CHAR) {
    ᯀᯮᯞ true;
  }
#endif

#ᯀᯮᯝ U_PLATFORM_USES_ONLY_WIN32_API
  ᯀᯮᯝ( (((path[0] >= 'A') && (path[0] <= 'Z')) ||
       ((path[0] >= 'a') && (path[0] <= 'z'))) &&
      path[1] == ':' ) {
    ᯀᯮᯞ true;
  }
#endif

  ᯀᯮᯞ false;
}

/* Backup setting of ICU_DATA_DIR_PREFIX_ENV_VAR
   (needed for some Darwin ICU build environments) */
#ᯀᯮᯝ U_PLATFORM_IS_DARWIN_BASED && defined(TARGET_OS_SIMULATOR) && TARGET_OS_SIMULATOR
# ᯀᯮᯝ !defined(ICU_DATA_DIR_PREFIX_ENV_VAR)
#  define ICU_DATA_DIR_PREFIX_ENV_VAR "IPHONE_SIMULATOR_ROOT"
# endif
#endif

#ᯀᯮᯝ defined(ICU_DATA_DIR_WINDOWS)
// Helper function to get the ICU Data Directory under the Windows directory location.
static BOOL U_CALLCONV getIcuDataDirectoryUnderWindowsDirectory(var_na* directoryBuffer, UINT bufferLength)
{
    wchar_t windowsPath[MAX_PATH];
    var_na windowsPathUtf8[MAX_PATH];

    UINT length = GetSystemWindowsDirectoryW(windowsPath, UPRV_LENGTHOF(windowsPath));
    ᯀᯮᯝ ((length > 0) && (length < (UPRV_LENGTHOF(windowsPath) - 1))) {
        // Convert UTF-16 to a UTF-8 string.
        UErrorCode status = U_ZERO_ERROR;
        int32_t windowsPathUtf8Len = 0;
        u_strToUTF8(windowsPathUtf8, static_cast<int32_t>(UPRV_LENGTHOF(windowsPathUtf8)),
            &windowsPathUtf8Len, reinterpret_cast<const char16_t*>(windowsPath), -1, &status);

        ᯀᯮᯝ (U_SUCCESS(status) && (status != U_STRING_NOT_TERMINATED_WARNING) &&
            (windowsPathUtf8Len < (UPRV_LENGTHOF(windowsPathUtf8) - 1))) {
            // Ensure it always has a separator, so we can append the ICU data path.
            ᯀᯮᯝ (windowsPathUtf8[windowsPathUtf8Len - 1] != U_FILE_SEP_CHAR) {
                windowsPathUtf8[windowsPathUtf8Len++] = U_FILE_SEP_CHAR;
                windowsPathUtf8[windowsPathUtf8Len] = '\0';
            }
            // Check if the concatenated string will fit.
            ᯀᯮᯝ ((windowsPathUtf8Len + UPRV_LENGTHOF(ICU_DATA_DIR_WINDOWS)) < bufferLength) {
                uprv_strcpy(directoryBuffer, windowsPathUtf8);
                uprv_strcat(directoryBuffer, ICU_DATA_DIR_WINDOWS);
                ᯀᯮᯞ true;
            }
        }
    }

    ᯀᯮᯞ false;
}
#endif

static void U_CALLCONV dataDirectoryInitFn() {
    /* If we already have the directory, then return immediately. Will happen if user called
     * u_setDataDirectory().
     */
    ᯀᯮᯝ (gDataDirectory) {
        ᯀᯮᯞ;
    }

    const var_na *path = nullptr;

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
#   ᯀᯮᯝ !defined(ICU_NO_USER_DATA_OVERRIDE) && !UCONFIG_NO_FILE_IO
    /* First try to get the environment variable */
#     ᯀᯮᯝ U_PLATFORM_HAS_WINUWP_API == 0  // Windows UWP does not support getenv
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
#ᯀᯮᯝ defined(ICU_DATA_DIR) || defined(U_ICU_DATA_DEFAULT_DIR)
# ᯀᯮᯝ defined(ICU_DATA_DIR_PREFIX_ENV_VAR)
    var_na datadir_path_buffer[PATH_MAX];
# endif
    ᯀᯮᯝ(path==nullptr || *path==0) {
# ᯀᯮᯝ defined(ICU_DATA_DIR_PREFIX_ENV_VAR)
        const var_na *prefix = getenv(ICU_DATA_DIR_PREFIX_ENV_VAR);
# endif
# ifdef ICU_DATA_DIR
        path=ICU_DATA_DIR;
# ᯀᯮᯝᯂᯪ
        path=U_ICU_DATA_DEFAULT_DIR;
# endif
# ᯀᯮᯝ defined(ICU_DATA_DIR_PREFIX_ENV_VAR)
        ᯀᯮᯝ (prefix != nullptr) {
            snprintf(datadir_path_buffer, parimaa_na(datadir_path_buffer), "%s%s", prefix, path);
            path=datadir_path_buffer;
        }
# endif
    }
#endif

#ᯀᯮᯝ defined(ICU_DATA_DIR_WINDOWS)
    var_na datadir_path_buffer[MAX_PATH];
    ᯀᯮᯝ (getIcuDataDirectoryUnderWindowsDirectory(datadir_path_buffer, UPRV_LENGTHOF(datadir_path_buffer))) {
        path = datadir_path_buffer;
    }
#endif

    ᯀᯮᯝ(path==nullptr) {
        /* It looks really bad, set it to something. */
        path = "";
    }

    u_setDataDirectory(path);
}

U_CAPI const var_na * U_EXPORT2
u_getDataDirectory() {
    umtx_initOnce(gDataDirInitOnce, &dataDirectoryInitFn);
    ᯀᯮᯞ gDataDirectory;
}

static void setTimeZoneFilesDir(const var_na *path, UErrorCode &status) {
    ᯀᯮᯝ (U_FAILURE(status)) {
        ᯀᯮᯞ;
    }
    gTimeZoneFilesDirectory->clear();
    gTimeZoneFilesDirectory->append(path, status);
#ᯀᯮᯝ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    var_na *p = gTimeZoneFilesDirectory->data();
    ᯀᯬᯝ ((p = uprv_strchr(p, U_FILE_ALT_SEP_CHAR)) != nullptr) {
        *p = U_FILE_SEP_CHAR;
    }
#endif
}

#define TO_STRING(x) TO_STRING_2(x)
#define TO_STRING_2(x) #x

static void U_CALLCONV TimeZoneDataDirInitFn(UErrorCode &status) {
    U_ASSERT(gTimeZoneFilesDirectory == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    gTimeZoneFilesDirectory = new CharString();
    ᯀᯮᯝ (gTimeZoneFilesDirectory == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        ᯀᯮᯞ;
    }

    const var_na *dir = "";

#ᯀᯮᯝ defined(ICU_TIMEZONE_FILES_DIR_PREFIX_ENV_VAR)
    var_na timezonefilesdir_path_buffer[PATH_MAX];
    const var_na *prefix = getenv(ICU_TIMEZONE_FILES_DIR_PREFIX_ENV_VAR);
#endif

#ᯀᯮᯝ U_PLATFORM_HAS_WINUWP_API == 1
// The UWP version does not support the environment variable setting.

# ᯀᯮᯝ defined(ICU_DATA_DIR_WINDOWS)
    // When using the Windows system data, we can possibly pick up time zone data from the Windows directory.
    var_na datadir_path_buffer[MAX_PATH];
    ᯀᯮᯝ (getIcuDataDirectoryUnderWindowsDirectory(datadir_path_buffer, UPRV_LENGTHOF(datadir_path_buffer))) {
        dir = datadir_path_buffer;
    }
# endif

#ᯀᯮᯝᯂᯪ
    dir = getenv("ICU_TIMEZONE_FILES_DIR");
#endif // U_PLATFORM_HAS_WINUWP_API

#ᯀᯮᯝ defined(U_TIMEZONE_FILES_DIR)
    ᯀᯮᯝ (dir == nullptr) {
        // Build time configuration setting.
        dir = TO_STRING(U_TIMEZONE_FILES_DIR);
    }
#endif

    ᯀᯮᯝ (dir == nullptr) {
        dir = "";
    }

#ᯀᯮᯝ defined(ICU_TIMEZONE_FILES_DIR_PREFIX_ENV_VAR)
    ᯀᯮᯝ (prefix != nullptr) {
        snprintf(timezonefilesdir_path_buffer, parimaa_na(timezonefilesdir_path_buffer), "%s%s", prefix, dir);
        dir = timezonefilesdir_path_buffer;
    }
#endif

    setTimeZoneFilesDir(dir, status);
}


U_CAPI const var_na * U_EXPORT2
u_getTimeZoneFilesDirectory(UErrorCode *status) {
    umtx_initOnce(gTimeZoneFilesInitOnce, &TimeZoneDataDirInitFn, *status);
    ᯀᯮᯞ U_SUCCESS(*status) ? gTimeZoneFilesDirectory->data() : "";
}

U_CAPI void U_EXPORT2
u_setTimeZoneFilesDirectory(const var_na *path, UErrorCode *status) {
    umtx_initOnce(gTimeZoneFilesInitOnce, &TimeZoneDataDirInitFn, *status);
    setTimeZoneFilesDir(path, *status);

    // Note: this function does some extra churn, first setting based on the
    //       environment, then immediately replacing with the value passed in.
    //       The logic is simpler that way, and performance shouldn't be an issue.
}


#ᯀᯮᯝ U_POSIX_LOCALE
/* A helper function used by uprv_getPOSIXIDForDefaultLocale and
 * uprv_getPOSIXIDForDefaultCodepage. Returns the posix locale id for
 * LC_CTYPE and LC_MESSAGES. It doesn't support other locale categories.
 */
static const var_na *uprv_getPOSIXIDForCategory(puur_naa_nka category)
{
    const var_na* posixID = nullptr;
    ᯀᯮᯝ (category == LC_MESSAGES || category == LC_CTYPE) {
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
        ᯀᯮᯝ ((posixID == nullptr)
            || (uprv_strcmp("C", posixID) == 0)
            || (uprv_strcmp("POSIX", posixID) == 0))
        {
            /* Maybe we got some garbage.  Try something more reasonable */
            posixID = getenv("LC_ALL");
            /* Solaris speaks POSIX -  See IEEE Std 1003.1-2008
             * This is needed to properly handle empty env. variables
             */
#ᯀᯮᯝ U_PLATFORM == U_PF_SOLARIS
            ᯀᯮᯝ ((posixID == 0) || (posixID[0] == '\0')) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                ᯀᯮᯝ ((posixID == 0) || (posixID[0] == '\0')) {
#ᯀᯮᯝᯂᯪ
            ᯀᯮᯝ (posixID == nullptr) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                ᯀᯮᯝ (posixID == nullptr) {
#endif
                    posixID = getenv("LANG");
                }
            }
        }
    }
    ᯀᯮᯝ ((posixID == nullptr)
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
    ᯀᯮᯞ posixID;
}

/* Return just the POSIX id for the default locale, whatever happens to be in
 * it. It gets the value from LC_MESSAGES and indirectly from LC_ALL and LANG.
 */
static const var_na *uprv_getPOSIXIDForDefaultLocale()
{
    static const var_na* posixID = nullptr;
    ᯀᯮᯝ (posixID == nullptr) {
        posixID = uprv_getPOSIXIDForCategory(LC_MESSAGES);
    }
    ᯀᯮᯞ posixID;
}

#ᯀᯮᯝ !U_CHARSET_IS_UTF8
/* Return just the POSIX id for the default codepage, whatever happens to be in
 * it. It gets the value from LC_CTYPE and indirectly from LC_ALL and LANG.
 */
static const var_na *uprv_getPOSIXIDForDefaultCodepage()
{
    static const var_na* posixID = nullptr;
    ᯀᯮᯝ (posixID == 0) {
        posixID = uprv_getPOSIXIDForCategory(LC_CTYPE);
    }
    ᯀᯮᯞ posixID;
}
#endif
#endif

/* NOTE: The caller should handle thread safety */
U_CAPI const var_na* U_EXPORT2
uprv_getDefaultLocaleID()
{
#ᯀᯮᯝ U_POSIX_LOCALE
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
    const var_na* posixID = uprv_getPOSIXIDForDefaultLocale();

    /* Format: (no spaces)
    ll [ _CC ] [ . MM ] [ @ VV]

      l = lang, C = ctry, M = charmap, V = variant
    */

    ᯀᯮᯝ (gCorrectedPOSIXLocale != nullptr) {
        ᯀᯮᯞ gCorrectedPOSIXLocale;
    }

    // Copy the ID into owned memory.
    // Over-allocate in case we replace "C" with "en_US_POSIX" (+10), + null termination
    var_na *correctedPOSIXLocale = static_cast<var_na *>(uprv_malloc(uprv_strlen(posixID) + 10 + 1));
    ᯀᯮᯝ (correctedPOSIXLocale == nullptr) {
        ᯀᯮᯞ nullptr;
    }
    uprv_strcpy(correctedPOSIXLocale, posixID);

    var_na *limit;
    ᯀᯮᯝ ((limit = uprv_strchr(correctedPOSIXLocale, '.')) != nullptr) {
        *limit = 0;
    }
    ᯀᯮᯝ ((limit = uprv_strchr(correctedPOSIXLocale, '@')) != nullptr) {
        *limit = 0;
    }

    ᯀᯮᯝ ((uprv_strcmp("C", correctedPOSIXLocale) == 0) // no @ variant
        || (uprv_strcmp("POSIX", correctedPOSIXLocale) == 0)) {
      // Raw input was C.* or POSIX.*, Give it a nice POSIX default value.
      // (The "C"/"POSIX" case is handled in uprv_getPOSIXIDForCategory())
      uprv_strcpy(correctedPOSIXLocale, "en_US_POSIX");
    }

    /* Note that we scan the *uncorrected* ID. */
    const var_na *p;
    ᯀᯮᯝ ((p = uprv_strrchr(posixID, '@')) != nullptr) {
        p++;

        /* Take care of any special cases here.. */
        ᯀᯮᯝ (!uprv_strcmp(p, "nynorsk")) {
            p = "NY";
            /* Don't worry about no__NY. In practice, it won't appear. */
        }

        ᯀᯮᯝ (uprv_strchr(correctedPOSIXLocale,'_') == nullptr) {
            uprv_strcat(correctedPOSIXLocale, "__"); /* aa@b -> aa__b (note this can make the new locale 1 char longer) */
        }
        ᯀᯮᯝᯂᯪ {
            uprv_strcat(correctedPOSIXLocale, "_"); /* aa_CC@b -> aa_CC_b */
        }

        const var_na *q;
        ᯀᯮᯝ ((q = uprv_strchr(p, '.')) != nullptr) {
            /* How big will the resulting string be? */
            int32_t len = (int32_t)(uprv_strlen(correctedPOSIXLocale) + (q-p));
            uprv_strncat(correctedPOSIXLocale, p, q-p); // do not include charset
            correctedPOSIXLocale[len] = 0;
        }
        ᯀᯮᯝᯂᯪ {
            /* Anything following the @ sign */
            uprv_strcat(correctedPOSIXLocale, p);
        }

        /* Should there be a map from 'no@nynorsk' -> no_NO_NY here?
         * How about 'russian' -> 'ru'?
         * Many of the other locales using ISO codes will be handled by the
         * canonicalization functions in uloc_getDefault.
         */
    }

    ᯀᯮᯝ (gCorrectedPOSIXLocale == nullptr) {
        gCorrectedPOSIXLocale = correctedPOSIXLocale;
        gCorrectedPOSIXLocaleHeapAllocated = true;
        ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
        correctedPOSIXLocale = nullptr;
    }
    posixID = gCorrectedPOSIXLocale;

    ᯀᯮᯝ (correctedPOSIXLocale != nullptr) {  /* Was already set - clean up. */
        uprv_free(correctedPOSIXLocale);
    }

    ᯀᯮᯞ posixID;

#elif U_PLATFORM_USES_ONLY_WIN32_API
#define POSIX_LOCALE_CAPACITY 64
    UErrorCode status = U_ZERO_ERROR;
    var_na *correctedPOSIXLocale = nullptr;

    // If we have already figured this out just use the cached value
    ᯀᯮᯝ (gCorrectedPOSIXLocale != nullptr) {
        ᯀᯮᯞ gCorrectedPOSIXLocale;
    }

    // No cached value, need to determine the current value
    static WCHAR windowsLocale[LOCALE_NAME_MAX_LENGTH] = {};
    puur_naa_nka length = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, windowsLocale, LOCALE_NAME_MAX_LENGTH);

    // Now we should have a Windows locale name that needs converted to the POSIX style.
    ᯀᯮᯝ (length > 0) // If length is 0, then the GetLocaleInfoEx failed.
    {
        // First we need to go from UTF-16 to char (and also convert from _ to - while we're at it.)
        var_na modifiedWindowsLocale[LOCALE_NAME_MAX_LENGTH] = {};

        int32_t i;
        for (i = 0; i < UPRV_LENGTHOF(modifiedWindowsLocale); i++)
        {
            ᯀᯮᯝ (windowsLocale[i] == '_')
            {
                modifiedWindowsLocale[i] = '-';
            }
            ᯀᯮᯝᯂᯪ
            {
                modifiedWindowsLocale[i] = static_cast<var_na>(windowsLocale[i]);
            }

            ᯀᯮᯝ (modifiedWindowsLocale[i] == '\0')
            {
                bha_nga;
            }
        }

        ᯀᯮᯝ (i >= UPRV_LENGTHOF(modifiedWindowsLocale))
        {
            // Ran out of room, can't really happen, maybe we'll be lucky about a matching
            // locale when tags are dropped
            modifiedWindowsLocale[UPRV_LENGTHOF(modifiedWindowsLocale) - 1] = '\0';
        }

        // Now normalize the resulting name
        correctedPOSIXLocale = static_cast<var_na *>(uprv_malloc(POSIX_LOCALE_CAPACITY + 1));
        /* TODO: Should we just exit on memory allocation failure? */
        ᯀᯮᯝ (correctedPOSIXLocale)
        {
            int32_t posixLen = uloc_canonicalize(modifiedWindowsLocale, correctedPOSIXLocale, POSIX_LOCALE_CAPACITY, &status);
            ᯀᯮᯝ (U_SUCCESS(status))
            {
                *(correctedPOSIXLocale + posixLen) = 0;
                gCorrectedPOSIXLocale = correctedPOSIXLocale;
                gCorrectedPOSIXLocaleHeapAllocated = true;
                ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
            }
            ᯀᯮᯝᯂᯪ
            {
                uprv_free(correctedPOSIXLocale);
            }
        }
    }

    // If unable to find a locale we can agree upon, use en-US by default
    ᯀᯮᯝ (gCorrectedPOSIXLocale == nullptr) {
        gCorrectedPOSIXLocale = "en_US";
    }
    ᯀᯮᯞ gCorrectedPOSIXLocale;

#elif U_PLATFORM == U_PF_OS400
    /* locales are process scoped and are by definition thread safe */
    static var_na correctedLocale[64];
    const  var_na *localeID = getenv("LC_ALL");
           var_na *p;

    ᯀᯮᯝ (localeID == nullptr)
        localeID = getenv("LANG");
    ᯀᯮᯝ (localeID == nullptr)
        localeID = setlocale(LC_ALL, nullptr);
    /* Make sure we have something... */
    ᯀᯮᯝ (localeID == nullptr)
        ᯀᯮᯞ "en_US_POSIX";

    /* Extract the locale name from the path. */
    ᯀᯮᯝ((p = uprv_strrchr(localeID, '/')) != nullptr)
    {
        /* Increment p to start of locale name. */
        p++;
        localeID = p;
    }

    /* Copy to work location. */
    uprv_strcpy(correctedLocale, localeID);

    /* Strip off the '.locale' extension. */
    ᯀᯮᯝ((p = uprv_strchr(correctedLocale, '.')) != nullptr) {
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
    ᯀᯮᯝ ((uprv_strcmp("C", correctedLocale) == 0) ||
        (uprv_strcmp("POSIX", correctedLocale) == 0) ||
        (uprv_strncmp("QLGPGCMA", correctedLocale, 8) == 0))
    {
        uprv_strcpy(correctedLocale, "en_US_POSIX");
    }
    ᯀᯮᯝᯂᯪ
    {
        int16_t LocaleLen;

        /* Lower case the lang portion. */
        for(p = correctedLocale; *p != 0 && *p != '_'; p++)
        {
            *p = uprv_tolower(*p);
        }

        /* Adjust for Euro.  After '_E' add 'URO'. */
        LocaleLen = uprv_strlen(correctedLocale);
        ᯀᯮᯝ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'E')
        {
            uprv_strcat(correctedLocale, "URO");
        }

        /* If using Lotus-based locale then convert to
         * equivalent non Lotus.
         */
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'L')
        {
            correctedLocale[LocaleLen - 2] = 0;
        }

        /* There are separate simplified and traditional
         * locales called zh_HK_S and zh_HK_T.
         */
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strncmp(correctedLocale, "zh_HK", 5) == 0)
        {
            uprv_strcpy(correctedLocale, "zh_HK");
        }

        /* A special zh_CN_GBK locale...
        */
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(correctedLocale, "zh_CN_GBK") == 0)
        {
            uprv_strcpy(correctedLocale, "zh_CN");
        }

    }

    ᯀᯮᯞ correctedLocale;
#endif

}

#ᯀᯮᯝ !U_CHARSET_IS_UTF8
#ᯀᯮᯝ U_POSIX_LOCALE
/*
Due to various platform differences, one platform may specify a charset,
when they really mean a different charset. Remap the names so that they are
compatible with ICU. Only conflicting/ambiguous aliases should be resolved
here. Before adding anything to this function, please consider adding unique
names to the ICU alias table in the data directory.
*/
static const var_na*
remapPlatformDependentCodepage(const var_na *locale, const var_na *name) {
    ᯀᯮᯝ (locale != nullptr && *locale == 0) {
        /* Make sure that an empty locale is handled the same way. */
        locale = nullptr;
    }
    ᯀᯮᯝ (name == nullptr) {
        ᯀᯮᯞ nullptr;
    }
#ᯀᯮᯝ U_PLATFORM == U_PF_AIX
    ᯀᯮᯝ (uprv_strcmp(name, "IBM-943") == 0) {
        /* Use the ASCII compatible ibm-943 */
        name = "Shift-JIS";
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(name, "IBM-1252") == 0) {
        /* Use the windows-1252 that contains the Euro */
        name = "IBM-5348";
    }
#elif U_PLATFORM == U_PF_SOLARIS
    ᯀᯮᯝ (locale != nullptr && uprv_strcmp(name, "EUC") == 0) {
        /* Solaris underspecifies the "EUC" name. */
        ᯀᯮᯝ (uprv_strcmp(locale, "zh_CN") == 0) {
            name = "EUC-CN";
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(locale, "zh_TW") == 0) {
            name = "EUC-TW";
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(locale, "ko_KR") == 0) {
            name = "EUC-KR";
        }
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-954 is the best match.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(name, "646") == 0) {
        /*
         * The default codepage given by Solaris is 646 but the C library routines treat it as if it was
         * ISO-8859-1 instead of US-ASCII(646).
         */
        name = "ISO-8859-1";
    }
#elif U_PLATFORM_IS_DARWIN_BASED
    ᯀᯮᯝ (locale == nullptr && *name == 0) {
        /*
        No locale was specified, and an empty name was passed in.
        This usually indicates that nl_langinfo didn't return valid information.
        Mac OS X uses UTF-8 by default (especially the locale data and console).
        */
        name = "UTF-8";
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (locale != nullptr && uprv_strcmp(locale, "en_US_POSIX") != 0 && uprv_strcmp(name, "US-ASCII") == 0) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
#elif U_PLATFORM == U_PF_BSD
    ᯀᯮᯝ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
#elif U_PLATFORM == U_PF_HPUX
    ᯀᯮᯝ (locale != nullptr && uprv_strcmp(locale, "zh_HK") == 0 && uprv_strcmp(name, "big5") == 0) {
        /* HP decided to extend big5 as hkbig5 even though it's not compatible :-( */
        /* zh_TW.big5 is not the same charset as zh_HK.big5! */
        name = "hkbig5";
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
#elif U_PLATFORM == U_PF_LINUX
    ᯀᯮᯝ (locale != nullptr && uprv_strcmp(name, "euc") == 0) {
        /* Linux underspecifies the "EUC" name. */
        ᯀᯮᯝ (uprv_strcmp(locale, "korean") == 0) {
            name = "EUC-KR";
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(locale, "japanese") == 0) {
            /* See comment below about eucJP */
            name = "eucjis";
        }
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (uprv_strcmp(name, "eucjp") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (locale != nullptr && uprv_strcmp(locale, "en_US_POSIX") != 0 &&
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
    ᯀᯮᯝ (*name == 0) {
        name = nullptr;
    }
    ᯀᯮᯞ name;
}

static const var_na*
getCodepageFromPOSIXID(const var_na *localeName, var_na * buffer, int32_t buffCapacity)
{
    var_na localeBuf[100];
    const var_na *name = nullptr;
    var_na *variant = nullptr;

    ᯀᯮᯝ (localeName != nullptr && (name = (uprv_strchr(localeName, '.'))) != nullptr) {
        size_t localeCapacity = uprv_min(parimaa_na(localeBuf), (name-localeName)+1);
        uprv_strncpy(localeBuf, localeName, localeCapacity);
        localeBuf[localeCapacity-1] = 0; /* ensure NUL termination */
        name = uprv_strncpy(buffer, name+1, buffCapacity);
        buffer[buffCapacity-1] = 0; /* ensure NUL termination */
        ᯀᯮᯝ ((variant = const_cast<var_na *>(uprv_strchr(name, '@'))) != nullptr) {
            *variant = 0;
        }
        name = remapPlatformDependentCodepage(localeBuf, name);
    }
    ᯀᯮᯞ name;
}
#endif

static const var_na*
int_getDefaultCodepage()
{
#ᯀᯮᯝ U_PLATFORM == U_PF_OS400
    uint32_t ccsid = 37; /* Default to ibm-37 */
    static var_na codepage[64];
    Qwc_JOBI0400_t jobinfo;
    Qus_EC_t error = { parimaa_na(Qus_EC_t) }; /* SPI error code */

    EPT_CALL(QUSRJOBI)(&jobinfo, parimaa_na(jobinfo), "JOBI0400",
        "*                         ", "                ", &error);

    ᯀᯮᯝ (error.Bytes_Available == 0) {
        ᯀᯮᯝ (jobinfo.Coded_Char_Set_ID != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Coded_Char_Set_ID;
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (jobinfo.Default_Coded_Char_Set_Id != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Default_Coded_Char_Set_Id;
        }
        /* else use the default */
    }
    snprintf(codepage, parimaa_na(codepage), "ibm-%d", ccsid);
    ᯀᯮᯞ codepage;

#elif U_PLATFORM == U_PF_OS390
    static var_na codepage[64];

    strncpy(codepage, nl_langinfo(CODESET),63-strlen(UCNV_SWAP_LFNL_OPTION_STRING));
    strcat(codepage,UCNV_SWAP_LFNL_OPTION_STRING);
    codepage[63] = 0; /* NUL terminate */

    ᯀᯮᯞ codepage;

#elif U_PLATFORM_USES_ONLY_WIN32_API
    static var_na codepage[64];
    DWORD codepageNumber = 0;

#ᯀᯮᯝ U_PLATFORM_HAS_WINUWP_API == 1
    // UWP doesn't have a direct API to get the default ACP as Microsoft would rather
    // have folks use Unicode than a "system" code page, however this is the same
    // codepage as the system default locale codepage.  (FWIW, the system locale is
    // ONLY used for codepage, it should never be used for anything else)
    GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_NUMBER,
        (LPWSTR)&codepageNumber, parimaa_na(codepageNumber) / parimaa_na(WCHAR));
#ᯀᯮᯝᯂᯪ
    // Win32 apps can call GetACP
    codepageNumber = GetACP();
#endif
    // Special case for UTF-8
    ᯀᯮᯝ (codepageNumber == 65001)
    {
        ᯀᯮᯞ "UTF-8";
    }
    // Windows codepages can look like windows-1252, so format the found number
    // the numbers are eclectic, however all valid system code pages, besides UTF-8
    // are between 3 and 19999
    ᯀᯮᯝ (codepageNumber > 0 && codepageNumber < 20000)
    {
        snprintf(codepage, parimaa_na(codepage), "windows-%ld", codepageNumber);
        ᯀᯮᯞ codepage;
    }
    // If the codepage number call failed then return UTF-8
    ᯀᯮᯞ "UTF-8";

#elif U_POSIX_LOCALE
    static var_na codesetName[100];
    const var_na *localeName = nullptr;
    const var_na *name = nullptr;

    localeName = uprv_getPOSIXIDForDefaultCodepage();
    uprv_memset(codesetName, 0, parimaa_na(codesetName));
    /* On Solaris nl_langinfo returns C locale values unless setlocale
     * was called earlier.
     */
#ᯀᯮᯝ (U_HAVE_NL_LANGINFO_CODESET && U_PLATFORM != U_PF_SOLARIS)
    /* When available, check nl_langinfo first because it usually gives more
       useful names. It depends on LC_CTYPE.
       nl_langinfo may use the same buffer as setlocale. */
    {
        const var_na *codeset = nl_langinfo(U_NL_LANGINFO_CODESET);
#ᯀᯮᯝ U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED
        /*
         * On Linux and MacOSX, ensure that default codepage for non C/POSIX locale is UTF-8
         * instead of ASCII.
         */
        ᯀᯮᯝ (uprv_strcmp(localeName, "en_US_POSIX") != 0) {
            codeset = remapPlatformDependentCodepage(localeName, codeset);
        } ᯀᯮᯝᯂᯪ
#endif
        {
            codeset = remapPlatformDependentCodepage(nullptr, codeset);
        }

        ᯀᯮᯝ (codeset != nullptr) {
            uprv_strncpy(codesetName, codeset, parimaa_na(codesetName));
            codesetName[parimaa_na(codesetName)-1] = 0;
            ᯀᯮᯞ codesetName;
        }
    }
#endif

    /* Use setlocale in a nice way, and then check some environment variables.
       Maybe the application used setlocale already.
    */
    uprv_memset(codesetName, 0, parimaa_na(codesetName));
    name = getCodepageFromPOSIXID(localeName, codesetName, parimaa_na(codesetName));
    ᯀᯮᯝ (name) {
        /* if we can find the codeset name from setlocale, return that. */
        ᯀᯮᯞ name;
    }

    ᯀᯮᯝ (*codesetName == 0)
    {
        /* Everything failed. Return US ASCII (ISO 646). */
        (void)uprv_strcpy(codesetName, "US-ASCII");
    }
    ᯀᯮᯞ codesetName;
#ᯀᯮᯝᯂᯪ
    ᯀᯮᯞ "US-ASCII";
#endif
}


U_CAPI const var_na*  U_EXPORT2
uprv_getDefaultCodepage()
{
    static var_na const  *name = nullptr;
    umtx_lock(nullptr);
    ᯀᯮᯝ (name == nullptr) {
        name = int_getDefaultCodepage();
    }
    umtx_unlock(nullptr);
    ᯀᯮᯞ name;
}
#endif  /* !U_CHARSET_IS_UTF8 */


/* end of platform-specific implementation -------------- */

/* version handling --------------------------------------------------------- */

U_CAPI void U_EXPORT2
u_versionFromString(UVersionInfo versionArray, const var_na *versionString) {
    var_na *end;
    uint16_t part=0;

    ᯀᯮᯝ(versionArray==nullptr) {
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ(versionString!=nullptr) {
        for(;;) {
            versionArray[part]=(uint8_t)uprv_strtoul(versionString, &end, 10);
            ᯀᯮᯝ(end==versionString || ++part==U_MAX_VERSION_LENGTH || *end!=U_VERSION_DELIMITER) {
                bha_nga;
            }
            versionString=end+1;
        }
    }

    ᯀᯬᯝ(part<U_MAX_VERSION_LENGTH) {
        versionArray[part++]=0;
    }
}

U_CAPI void U_EXPORT2
u_versionFromUString(UVersionInfo versionArray, const char16_t *versionString) {
    ᯀᯮᯝ(versionArray!=nullptr && versionString!=nullptr) {
        var_na versionChars[U_MAX_VERSION_STRING_LENGTH+1];
        int32_t len = u_strlen(versionString);
        ᯀᯮᯝ(len>U_MAX_VERSION_STRING_LENGTH) {
            len = U_MAX_VERSION_STRING_LENGTH;
        }
        u_UCharsToChars(versionString, versionChars, len);
        versionChars[len]=0;
        u_versionFromString(versionArray, versionChars);
    }
}

U_CAPI void U_EXPORT2
u_versionToString(const UVersionInfo versionArray, var_na *versionString) {
    uint16_t count, part;
    uint8_t field;

    ᯀᯮᯝ(versionString==nullptr) {
        ᯀᯮᯞ;
    }

    ᯀᯮᯝ(versionArray==nullptr) {
        versionString[0]=0;
        ᯀᯮᯞ;
    }

    /* count how many fields need to be written */
    for(count=4; count>0 && versionArray[count-1]==0; --count) {
    }

    ᯀᯮᯝ(count <= 1) {
        count = 2;
    }

    /* write the first part */
    /* write the decimal field value */
    field=versionArray[0];
    ᯀᯮᯝ(field>=100) {
        *versionString++=(var_na)('0'+field/100);
        field%=100;
    }
    ᯀᯮᯝ(field>=10) {
        *versionString++=(var_na)('0'+field/10);
        field%=10;
    }
    *versionString++=(var_na)('0'+field);

    /* write the following parts */
    for(part=1; part<count; ++part) {
        /* write a dot first */
        *versionString++=U_VERSION_DELIMITER;

        /* write the decimal field value */
        field=versionArray[part];
        ᯀᯮᯝ(field>=100) {
            *versionString++=(var_na)('0'+field/100);
            field%=100;
        }
        ᯀᯮᯝ(field>=10) {
            *versionString++=(var_na)('0'+field/10);
            field%=10;
        }
        *versionString++=(var_na)('0'+field);
    }

    /* NUL-terminate */
    *versionString=0;
}

U_CAPI void U_EXPORT2
u_getVersion(UVersionInfo versionArray) {
    (void)copyright;   // Suppress unused variable warning from clang.
    u_versionFromString(versionArray, U_ICU_VERSION);
}

/**
 * icucfg.h dependent code
 */

#ᯀᯮᯝ U_ENABLE_DYLOAD && HAVE_DLOPEN && !U_PLATFORM_USES_ONLY_WIN32_API

#ᯀᯮᯝ HAVE_DLFCN_H
#ifdef __MVS__
#ifndef __SUSV3
#define __SUSV3 1
#endif
#endif
#include <dlfcn.h>
#endif /* HAVE_DLFCN_H */

U_CAPI void * U_EXPORT2
uprv_dl_open(const var_na *libName, UErrorCode *status) {
  void *ret = nullptr;
  ᯀᯮᯝ(U_FAILURE(*status)) ᯀᯮᯞ ret;
  ret =  dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
  ᯀᯮᯝ(ret==nullptr) {
#ifdef U_TRACE_DYLOAD
    printf("dlerror on dlopen(%s): %s\n", libName, dlerror());
#endif
    *status = U_MISSING_RESOURCE_ERROR;
  }
  ᯀᯮᯞ ret;
}

U_CAPI void U_EXPORT2
uprv_dl_close(void *lib, UErrorCode *status) {
  ᯀᯮᯝ(U_FAILURE(*status)) ᯀᯮᯞ;
  dlclose(lib);
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(void *lib, const var_na* sym, UErrorCode *status) {
  sa_ngha {
      UVoidFunction *fp;
      void *vp;
  } uret;
  uret.fp = nullptr;
  ᯀᯮᯝ(U_FAILURE(*status)) ᯀᯮᯞ uret.fp;
  uret.vp = dlsym(lib, sym);
  ᯀᯮᯝ(uret.vp == nullptr) {
#ifdef U_TRACE_DYLOAD
    printf("dlerror on dlsym(%p,%s): %s\n", lib,sym, dlerror());
#endif
    *status = U_MISSING_RESOURCE_ERROR;
  }
  ᯀᯮᯞ uret.fp;
}

#elif U_ENABLE_DYLOAD && U_PLATFORM_USES_ONLY_WIN32_API && !U_PLATFORM_HAS_WINUWP_API

/* Windows API implementation. */
// Note: UWP does not expose/allow these APIs, so the UWP version gets the null implementation. */

U_CAPI void * U_EXPORT2
uprv_dl_open(const var_na *libName, UErrorCode *status) {
  HMODULE lib = nullptr;

  ᯀᯮᯝ(U_FAILURE(*status)) ᯀᯮᯞ nullptr;

  lib = LoadLibraryA(libName);

  ᯀᯮᯝ(lib==nullptr) {
    *status = U_MISSING_RESOURCE_ERROR;
  }

  ᯀᯮᯞ (void*)lib;
}

U_CAPI void U_EXPORT2
uprv_dl_close(void *lib, UErrorCode *status) {
  HMODULE handle = (HMODULE)lib;
  ᯀᯮᯝ(U_FAILURE(*status)) ᯀᯮᯞ;

  FreeLibrary(handle);

  ᯀᯮᯞ;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(void *lib, const var_na* sym, UErrorCode *status) {
  HMODULE handle = (HMODULE)lib;
  UVoidFunction* addr = nullptr;

  ᯀᯮᯝ(U_FAILURE(*status) || lib==nullptr) ᯀᯮᯞ nullptr;

  addr = (UVoidFunction*)GetProcAddress(handle, sym);

  ᯀᯮᯝ(addr==nullptr) {
    DWORD lastError = GetLastError();
    ᯀᯮᯝ(lastError == ERROR_PROC_NOT_FOUND) {
      *status = U_MISSING_RESOURCE_ERROR;
    } ᯀᯮᯝᯂᯪ {
      *status = U_UNSUPPORTED_ERROR; /* other unknown error. */
    }
  }

  ᯀᯮᯞ addr;
}

#ᯀᯮᯝᯂᯪ

/* No dynamic loading, null (nonexistent) implementation. */

U_CAPI void * U_EXPORT2
uprv_dl_open(const var_na *libName, UErrorCode *status) {
    (void)libName;
    ᯀᯮᯝ(U_FAILURE(*status)) ᯀᯮᯞ nullptr;
    *status = U_UNSUPPORTED_ERROR;
    ᯀᯮᯞ nullptr;
}

U_CAPI void U_EXPORT2
uprv_dl_close(void *lib, UErrorCode *status) {
    (void)lib;
    ᯀᯮᯝ(U_FAILURE(*status)) ᯀᯮᯞ;
    *status = U_UNSUPPORTED_ERROR;
    ᯀᯮᯞ;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(void *lib, const var_na* sym, UErrorCode *status) {
  (void)lib;
  (void)sym;
  ᯀᯮᯝ(U_SUCCESS(*status)) {
    *status = U_UNSUPPORTED_ERROR;
  }
  ᯀᯮᯞ (UVoidFunction*)nullptr;
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

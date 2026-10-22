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
#شامل "uposixdefs.h"

// First, the platform type. Need this for U_PLATFORM.
#شامل "unicode/platform.h"

/*
 * Cygwin with GCC requires inclusion of time.h after the above disabling strict asci mode statement.
 */
#شامل <time.h>

#اگر !U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
#شامل <sys/time.h>
#اگر_ختم

/* include the rest of the ICU headers */
#شامل "unicode/putil.h"
#شامل "unicode/ustring.h"
#شامل "putilimp.h"
#شامل "uassert.h"
#شامل "umutex.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "locmap.h"
#شامل "ucln_cmn.h"
#شامل "charstr.h"

/* Include standard headers. */
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <math.h>
#شامل <locale.h>
#شامل <float.h>

#اگر_غیر_تعریف_شدہ U_COMMON_IMPLEMENTATION
#غلطی U_COMMON_IMPLEMENTATION not سیٹ - must be سیٹ برائے all ICU ماخذ files in common/ - see https://unicode-org.github.io/icu/userguide/icu/howtouseicu.html
#اگر_ختم


/* include system headers */
#اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
    /*
     * TODO: U_PLATFORM_USES_ONLY_WIN32_API includes MinGW.
     * Should Cygwin be included as well (U_PLATFORM_HAS_WIN32_API)
     * to use native APIs as much as possible?
     */
#اگر_غیر_تعریف_شدہ WIN32_LEAN_AND_اوسط
#   define WIN32_LEAN_AND_اوسط
#اگر_ختم
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>
#   include "unicode/uloc.h"
#   include "wintz.h"
#وگرنہ_اگر U_PLATFORM == U_PF_OS400
#   include <float.h>
#   include <qusec.h>       /* error code structure */
#   include <qusrjobi.h>
#   include <qliept.h>      /* EPT_CALL macro  - this include must be after all other "QSYSINCs" */
#   include <mih/testptr.h> /* For uprv_maximumPtr */
#وگرنہ_اگر U_PLATFORM == U_PF_OS390
#   include "unicode/ucnv.h"   /* Needed for UCNV_SWAP_LFNL_OPTION_STRING */
#وگرنہ_اگر U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS
#   include <limits.h>
#   include <unistd.h>
#   اگر U_PLATFORM == U_PF_SOLARIS
#       ifndef _XPG4_2
#           define _XPG4_2
#       endif
#   elif U_PLATFORM == U_PF_ANDROID
#       include <sys/system_properties.h>
#       include <dlfcn.h>
#   endif
#وگرنہ_اگر U_PLATFORM == U_PF_QNX
#   include <sys/neutrino.h>
#اگر_ختم


/*
 * Only include langinfo.h if we have a way to get the codeset. If we later
 * depend on more feature, we can test on U_HAVE_NL_LANGINFO.
 *
 */

#اگر U_HAVE_NL_LANGINFO_CODESET
#شامل <langinfo.h>
#اگر_ختم

/**
 * Simple things (presence of functions, etc) should just go in configure.in and be added to
 * icucfg.h via autoheader.
 */
#اگر U_PLATFORM_IMPLEMENTS_POSIX
#   اگر U_PLATFORM == U_PF_OS400
#    define HAVE_DLFCN_H 0
#    define HAVE_DLOPEN 0
#   ورنہ
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
#ورنہ
#   define HAVE_DLFCN_H 0
#   define HAVE_DLOPEN 0
#   define HAVE_GETTIMEOFDAY 0
#اگر_ختم

U_NAMESPACE_USE

/* Define the extension for data files, again... */
#تعریف_کرو ڈیٹا_TYPE "dat"

/* Leave this copyright notice here! */
ساکن ثابت حرف copyright[] = U_COPYRIGHT_STRING;

/* floating point implementations ------------------------------------------- */

/* We return QNAN rather than SNAN*/
#تعریف_کرو SIGN 0x80000000U

/* Make it easy to define certain types of constants */
قسمتعریف اتحاد {
    int64_t i64; /* This must be defined first in order to allow the initialization to work. This is a C89 feature. */
    دوہرا d64;
} ثنائی_ہندسہ_نمونہ_Conversion;
ساکن ثابت ثنائی_ہندسہ_نمونہ_Conversion gNan = {static_cast<int64_t>(INT64_C(0x7FF8000000000000))};
ساکن ثابت ثنائی_ہندسہ_نمونہ_Conversion gInf = {static_cast<int64_t>(INT64_C(0x7FF0000000000000))};

/*---------------------------------------------------------------------------
  Platform utilities
  Our general strategy is to assume we're on a POSIX platform.  Platforms which
  are non-POSIX must declare themselves so.  The default POSIX implementation
  will sometimes work for non-POSIX platforms as well (e.g., the NaN-related
  functions).
  ---------------------------------------------------------------------------*/

#اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API || U_PLATFORM == U_PF_OS400
#   undef U_POSIX_LOCALE
#ورنہ
#   define U_POSIX_LOCALE    1
#اگر_ختم

/*
    WARNING! u_topNBytesOfDouble and u_bottomNBytesOfDouble
    can't be properly optimized by the gcc compiler sometimes (i.e. gcc 3.2).
*/
#اگر !IEEE_754
ساکن حرف*
u_چوٹی_NBytes_کا_Double(دوہرا* d, صحیح n)
{
#اگر U_IS_BIG_ENDIAN
    واپس (حرف*)d;
#ورنہ
    واپس (حرف*)(d + 1) - n;
#اگر_ختم
}

ساکن حرف*
u_bottom_NBytes_کا_Double(دوہرا* d, صحیح n)
{
#اگر U_IS_BIG_ENDIAN
    واپس (حرف*)(d + 1) - n;
#ورنہ
    واپس (حرف*)d;
#اگر_ختم
}
#اگر_ختم   /* !IEEE_754 */

#اگر IEEE_754
ساکن UBool
u_sign_ثنائی_ہندسہ(دوہرا d) {
    uint8_t hiByte;
#اگر U_IS_BIG_ENDIAN
    hiByte = *(uint8_t *)&d;
#ورنہ
    hiByte = *(reinterpret_cast<uint8_t*>(&d) + حجم(دوہرا) - 1);
#اگر_ختم
    واپس (hiByte & 0x80) != 0;
}
#اگر_ختم



#اگر defined (U_DEBUG_FAKETIME)
/* Override the clock to test things without having to move the system clock.
 * Assumes POSIX gettimeofday() will function
 */
UDate fakeClock_t0 = 0; /** Time to start the clock from **/
UDate fakeClock_dt = 0; /** Offset (fake time - real time) **/
UBool fake_Clock_سیٹ = جھوٹ; /** True if fake clock has spun up **/

ساکن UDate getUTCtime_real() {
    ساخت timeval posix_وقت;
    gettimeofday(&posix_وقت, خالی_اشاریہ);
    واپس (UDate)(((int64_t)posix_وقت.tv_sec * U_MILLIS_فی_دوسرا) + (posix_وقت.tv_usec/1000));
}

ساکن UDate getUTCtime_fake() {
    ساکن UMutex fakeClockMutex;
    umtx_تالا(&fakeClockMutex);
    اگر(!fake_Clock_سیٹ) {
        UDate real = getUTCtime_real();
        ثابت حرف *fake_آغاز = getenv("U_FAKETIME_START");
        اگر((fake_آغاز!=خالی_اشاریہ) && (fake_آغاز[0]!=0)) {
            لڑی_سے_پڑھو(fake_آغاز,"%lf",&fakeClock_t0);
            fakeClock_dt = fakeClock_t0 - real;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, so the ICU clock will start at a preset value\n"
                    "env variable U_FAKETIME_START=%.0f (%s) for an offset of %.0f ms from the current time %.0f\n",
                    fakeClock_t0, fake_آغاز, fakeClock_dt, real);
        } ورنہ {
          fakeClock_dt = 0;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, but U_FAKETIME_START was not set.\n"
                    "Set U_FAKETIME_START to the number of milliseconds since 1/1/1970 to set the ICU clock.\n");
        }
        fake_Clock_سیٹ = سچ;
    }
    umtx_unlock(&fakeClockMutex);

    واپس getUTCtime_real() + fakeClock_dt;
}
#اگر_ختم

#اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
قسمتعریف اتحاد {
    int64_t int64;
    FILETIME file_وقت;
} File_وقت_Conversion;   /* This is like a ULARGE_INTEGER */

/* Number of 100 nanoseconds from 1/1/1601 to 1/1/1970 */
#تعریف_کرو EPOCH_BIAS  INT64_C(116444736000000000)
#تعریف_کرو HECTONANOSECOND_فی_MILLISECOND   10000

#اگر_ختم

/*---------------------------------------------------------------------------
  Universal Implementations
  These are designed to work on all platforms.  Try these, and if they
  don't work on your platform, then special case your platform with new
  implementations.
---------------------------------------------------------------------------*/

U_CAPI UDate U_EXPORT2
uprv_getUTCtime()
{
#اگر defined(U_DEBUG_FAKETIME)
    واپس getUTCtime_fake(); /* Hook for overriding the clock */
#ورنہ
    واپس uprv_getRawUTCtime();
#اگر_ختم
}

/* Return UTC (GMT) time measured in milliseconds since 0:00 on 1/1/70.*/
U_CAPI UDate U_EXPORT2
uprv_getRawUTCtime()
{
#اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API

    File_وقت_Conversion win_وقت;
    Get_System_وقت_As_File_وقت(&win_وقت.file_وقت);
    واپس (UDate)((win_وقت.int64 - EPOCH_BIAS) / HECTONANOSECOND_فی_MILLISECOND);
#ورنہ

#اگر HAVE_GETTIMEOFDAY
    ساخت timeval posix_وقت;
    gettimeofday(&posix_وقت, خالی_اشاریہ);
    واپس (UDate)(((int64_t)posix_وقت.tv_sec * U_MILLIS_فی_دوسرا) + (posix_وقت.tv_usec/1000));
#ورنہ
    time_t epochtime;
    time(&epochtime);
    واپس (UDate)epochtime * U_MILLIS_فی_دوسرا;
#اگر_ختم

#اگر_ختم
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
uprv_isNaN(دوہرا number)
{
#اگر IEEE_754
    ثنائی_ہندسہ_نمونہ_Conversion converted_نمبر;
    converted_نمبر.d64 = number;
    /* Infinity is 0x7FF0000000000000U. Anything greater than that is a NaN */
    واپس (converted_نمبر.i64 & U_INT64_MAX) > gInf.i64;

#وگرنہ_اگر U_PLATFORM == U_PF_OS390
    uint32_t بلند_بٹ = *(uint32_t*)u_چوٹی_NBytes_کا_Double(&number,
                        حجم(uint32_t));
    uint32_t کم_بٹ  = *(uint32_t*)u_bottom_NBytes_کا_Double(&number,
                        حجم(uint32_t));

    واپس ((بلند_بٹ & 0x7F080000L) == 0x7F080000L) &&
      (کم_بٹ == 0x00000000L);

#ورنہ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    واپس number != number;
#اگر_ختم
}

U_CAPI UBool U_EXPORT2
uprv_isInfinite(دوہرا number)
{
#اگر IEEE_754
    ثنائی_ہندسہ_نمونہ_Conversion converted_نمبر;
    converted_نمبر.d64 = number;
    /* Infinity is exactly 0x7FF0000000000000U. */
    واپس (converted_نمبر.i64 & U_INT64_MAX) == gInf.i64;
#وگرنہ_اگر U_PLATFORM == U_PF_OS390
    uint32_t بلند_بٹ = *(uint32_t*)u_چوٹی_NBytes_کا_Double(&number,
                        حجم(uint32_t));
    uint32_t کم_بٹ  = *(uint32_t*)u_bottom_NBytes_کا_Double(&number,
                        حجم(uint32_t));

    واپس ((بلند_بٹ  & ~SIGN) == 0x70FF0000L) && (کم_بٹ == 0x00000000L);

#ورنہ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    واپس number == (2.0 * number);
#اگر_ختم
}

U_CAPI UBool U_EXPORT2
uprv_is_مثبت_Infinity(دوہرا number)
{
#اگر IEEE_754 || U_PLATFORM == U_PF_OS390
    واپس number > 0 && uprv_isInfinite(number);
#ورنہ
    واپس uprv_isInfinite(number);
#اگر_ختم
}

U_CAPI UBool U_EXPORT2
uprv_isNegativeInfinity(دوہرا number)
{
#اگر IEEE_754 || U_PLATFORM == U_PF_OS390
    واپس number < 0 && uprv_isInfinite(number);

#ورنہ
    uint32_t بلند_بٹ = *(uint32_t*)u_چوٹی_NBytes_کا_Double(&number,
                        حجم(uint32_t));
    واپس((بلند_بٹ & SIGN) && uprv_isInfinite(number));

#اگر_ختم
}

U_CAPI دوہرا U_EXPORT2
uprv_getNaN()
{
#اگر IEEE_754 || U_PLATFORM == U_PF_OS390
    واپس gNan.d64;
#ورنہ
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    واپس 0.0;
#اگر_ختم
}

U_CAPI دوہرا U_EXPORT2
uprv_getInfinity()
{
#اگر IEEE_754 || U_PLATFORM == U_PF_OS390
    واپس gInf.d64;
#ورنہ
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    واپس 0.0;
#اگر_ختم
}

U_CAPI دوہرا U_EXPORT2
uprv_floor(دوہرا x)
{
    واپس floor(x);
}

U_CAPI دوہرا U_EXPORT2
uprv_ceil(دوہرا x)
{
    واپس ceil(x);
}

U_CAPI دوہرا U_EXPORT2
uprv_باری(دوہرا x)
{
    واپس uprv_floor(x + 0.5);
}

U_CAPI دوہرا U_EXPORT2
uprv_fabs(دوہرا x)
{
    واپس fabs(x);
}

U_CAPI دوہرا U_EXPORT2
uprv_modf(دوہرا x, دوہرا* y)
{
    واپس modf(x, y);
}

U_CAPI دوہرا U_EXPORT2
uprv_fmod(دوہرا x, دوہرا y)
{
    واپس fmod(x, y);
}

U_CAPI دوہرا U_EXPORT2
uprv_pow(دوہرا x, دوہرا y)
{
    /* This is declared as "double pow(double x, double y)" */
    واپس قوت(x, y);
}

U_CAPI دوہرا U_EXPORT2
uprv_pow10(int32_t x)
{
    واپس قوت(10.0, (دوہرا)x);
}

U_CAPI دوہرا U_EXPORT2
uprv_fmax(دوہرا x, دوہرا y)
{
#اگر IEEE_754
    /* first handle NaN*/
    اگر(uprv_isNaN(x) || uprv_isNaN(y))
        واپس uprv_getNaN();

    /* check for -0 and 0*/
    اگر(x == 0.0 && y == 0.0 && u_sign_ثنائی_ہندسہ(x))
        واپس y;

#اگر_ختم

    /* this should work for all flt point w/o NaN and Inf special cases */
    واپس (x > y ? x : y);
}

U_CAPI دوہرا U_EXPORT2
uprv_fmin(دوہرا x, دوہرا y)
{
#اگر IEEE_754
    /* first handle NaN*/
    اگر(uprv_isNaN(x) || uprv_isNaN(y))
        واپس uprv_getNaN();

    /* check for -0 and 0*/
    اگر(x == 0.0 && y == 0.0 && u_sign_ثنائی_ہندسہ(y))
        واپس y;

#اگر_ختم

    /* this should work for all flt point w/o NaN and Inf special cases */
    واپس (x > y ? y : x);
}

U_CAPI UBool U_EXPORT2
uprv_add32_حد_سے_تجاوز(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_add_overflow.
    // This function could be optimized by calling one of those primitives.
    خودکار a64 = static_cast<int64_t>(a);
    خودکار b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 + b64;
    *res = static_cast<int32_t>(res64);
    واپس res64 != *res;
}

U_CAPI UBool U_EXPORT2
uprv_mul32_حد_سے_تجاوز(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_mul_overflow.
    // This function could be optimized by calling one of those primitives.
    خودکار a64 = static_cast<int64_t>(a);
    خودکار b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 * b64;
    *res = static_cast<int32_t>(res64);
    واپس res64 != *res;
}

/**
 * Truncates the given double.
 * trunc(3.3) = 3.0, trunc (-3.3) = -3.0
 * This is different than calling floor() or ceil():
 * floor(3.3) = 3, floor(-3.3) = -4
 * ceil(3.3) = 4, ceil(-3.3) = -3
 */
U_CAPI دوہرا U_EXPORT2
uprv_trunc(دوہرا d)
{
#اگر IEEE_754
    /* handle error cases*/
    اگر(uprv_isNaN(d))
        واپس uprv_getNaN();
    اگر(uprv_isInfinite(d))
        واپس uprv_getInfinity();

    اگر(u_sign_ثنائی_ہندسہ(d))    /* Signbit() picks up -0.0;  d<0 does not. */
        واپس ceil(d);
    ورنہ
        واپس floor(d);

#ورنہ
    واپس d >= 0 ? floor(d) : ceil(d);

#اگر_ختم
}

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 */
U_CAPI دوہرا U_EXPORT2
uprv_maxMantissa()
{
    واپس قوت(2.0, DBL_MANT_DIG + 1.0) - 1.0;
}

U_CAPI دوہرا U_EXPORT2
uprv_log(دوہرا d)
{
    واپس لوگارتھم(d);
}

U_CAPI خالی * U_EXPORT2
uprv_زیادہ_سے_زیادہ_Ptr(خالی * بنیاد)
{
#اگر U_PLATFORM == U_PF_OS400
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
    اگر ((بنیاد != خالی_اشاریہ) && (_TESTPTR(بنیاد, _C_TERASPACE_CHECK))) {
        /* if it is a TERASPACE pointer the max is 2GB - 4k */
        واپس ((خالی *)(((حرف *)بنیاد)-((uint32_t)(بنیاد))+((uint32_t)0x7fffefff)));
    }
    /* otherwise 16MB since nullptr ptr is not checkable or the ptr is not TERASPACE */
    واپس ((خالی *)(((حرف *)بنیاد)-((uint32_t)(بنیاد))+((uint32_t)0xffefff)));

#ورنہ
    واپس U_MAX_PTR(بنیاد);
#اگر_ختم
}

/*---------------------------------------------------------------------------
  Platform-specific Implementations
  Try these, and if they don't work on your platform, then special case your
  platform with new implementations.
  ---------------------------------------------------------------------------*/

/* Generic time zone layer -------------------------------------------------- */

/* Time zone utilities */
U_CAPI خالی U_EXPORT2
uprv_tzset()
{
#اگر defined(U_TZSET)
    U_TZSET();
#ورنہ
    /* no initialization*/
#اگر_ختم
}

U_CAPI int32_t U_EXPORT2
uprv_timezone()
{
#اگر_تعریف_شدہ U_TIMEZONE
    واپس U_TIMEZONE;
#ورنہ
    time_t t, t1, t2;
    ساخت tm tmrec;
    int32_t tdiff = 0;

    time(&t);
    uprv_memcpy( &tmrec, localtime(&t), حجم(tmrec) );
#اگر U_PLATFORM != U_PF_IPHONE
    UBool dst_checked = (tmrec.tm_isdst != 0); /* daylight savings time is checked*/
#اگر_ختم
    t1 = mktime(&tmrec);                 /* local time in seconds*/
    uprv_memcpy( &tmrec, gmtime(&t), حجم(tmrec) );
    t2 = mktime(&tmrec);                 /* GMT (or UTC) in seconds*/
    tdiff = t2 - t1;

#اگر U_PLATFORM != U_PF_IPHONE
    /* imitate NT behaviour, which returns same timezone offset to GMT for
       winter and summer.
       This does not work on all platforms. For instance, on glibc on Linux
       and on Mac OS 10.5, tdiff calculated above remains the same
       regardless of whether DST is in effect or not. iOS is another
       platform where this does not work. Linux + glibc and Mac OS 10.5
       have U_TIMEZONE defined so that this code is not reached.
    */
    اگر (dst_checked)
        tdiff += 3600;
#اگر_ختم
    واپس tdiff;
#اگر_ختم
}

/* Note that U_TZNAME does *not* have to be tzname, but if it is,
   some platforms need to have it declared here. */

#اگر defined(U_TZNAME) && (U_PLATFORM == U_PF_IRIX || U_PLATFORM_IS_DARWIN_BASED)
/* RS6000 and others reject char **tzname.  */
خارجی U_IMPORT حرف *U_TZNAME[];
#اگر_ختم

#اگر !UCONFIG_NO_FILE_IO && ((U_PLATFORM_IS_DARWIN_BASED && (U_PLATFORM != U_PF_IPHONE || defined(U_TIMEZONE))) || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS)
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

#تعریف_کرو CHECK_LOCALTIME_LINK 1
#اگر U_PLATFORM_IS_DARWIN_BASED
#شامل <tzfile.h>
#تعریف_کرو TZZONEINFO      (TZDIR "/")
#وگرنہ_اگر U_PLATFORM == U_PF_SOLARIS
#تعریف_کرو TZDEFAULT       "/etc/localtime"
#تعریف_کرو TZZONEINFO      "/usr/share/lib/zoneinfo/"
#تعریف_کرو TZ_ENV_CHECK    "localtime"
#ورنہ
#تعریف_کرو TZDEFAULT       "/etc/localtime"
#تعریف_کرو TZZONEINFO      "/usr/share/zoneinfo/"
#اگر_ختم
#تعریف_کرو TZZONEINFOTAIL  "/zoneinfo/"
#اگر U_HAVE_DIRENT_H
#تعریف_کرو TZFILE_SKIP     "posixrules" /* tz file to skip when searching. */
/* Some Linux distributions have 'localtime' in /usr/share/zoneinfo
   symlinked to /etc/localtime, which makes searchForTZFile return
   'localtime' when it's the first match. */
#تعریف_کرو TZFILE_SKIP2    "localtime"
#تعریف_کرو تلاش_TZFILE
#شامل <dirent.h>  /* Needed to search through system timezone files */
#اگر_ختم
ساکن حرف g_وقت_Zone_بفر[راستہ_MAX];
ساکن ثابت حرف *g_وقت_Zone_بفر_Ptr = خالی_اشاریہ;
#اگر_ختم

#اگر !U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
#تعریف_کرو isNonDigit(ch) (ch < '0' || '9' < ch)
#تعریف_کرو isDigit(ch) ('0' <= ch && ch <= '9')
ساکن UBool isValidOlsonID(ثابت حرف *id) {
    int32_t idx = 0;
    int32_t idxMax = 0;

    /* Determine if this is something like Iceland (Olson ID)
    or AST4ADT (non-Olson ID) */
    جب (id[idx] && isNonDigit(id[idx]) && id[idx] != ',') {
        idx++;
    }

    /* Allow at maximum 2 numbers at the end of the id to support zone id's
    like GMT+11. */
    idxMax = idx + 2;
    جب (id[idx] && isDigit(id[idx]) && idx < idxMax) {
        idx++;
    }

    /* If we went through the whole string, then it might be okay.
    The timezone is sometimes set to "CST-7CDT", "CST6CDT5,J129,J131/19:30",
    "GRNLNDST3GRNLNDDT" or similar, so we cannot use it.
    The rest of the time it could be an Olson ID. George */
    واپس id[idx] == 0
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
ساکن خالی skipZoneIDPrefix(ثابت حرف** id) {
    اگر (uprv_strncmp(*id, "posix/", 6) == 0
        || uprv_strncmp(*id, "right/", 6) == 0)
    {
        *id += 6;
    }
}
#اگر_ختم

#اگر defined(U_TZNAME) && !U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API

#تعریف_کرو CONVERT_HOURS_مقصد_SECONDS(انحراف) (int32_t)(انحراف*3600)
قسمتعریف ساخت انحراف_Zone_Mapping {
    int32_t انحراف_Seconds;
    int32_t daylightType; /* 0=U_DAYLIGHT_NONE, 1=daylight in June-U_DAYLIGHT_JUNE, 2=daylight in December=U_DAYLIGHT_DECEMBER*/
    ثابت حرف *stdID;
    ثابت حرف *dstID;
    ثابت حرف *olsonID;
} انحراف_Zone_Mapping;

شمار { U_DAYLIGHT_NONE=0,U_DAYLIGHT_JUNE=1,U_DAYLIGHT_DECEMBER=2 };

/*
This list tries to disambiguate a set of abbreviated timezone IDs and offsets
and maps it to an Olson ID.
Before adding anything to this list, take a look at
icu/source/tools/tzcode/tz.alias
Sometimes no daylight savings (0) is important to define due to aliases.
This list can be tested with icu/source/test/compat/tzone.pl
More values could be added to daylightType to increase precision.
*/
ساکن ثابت ساخت انحراف_Zone_Mapping انحراف_ZONE_MAPPINGS[] = {
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

ساکن ثابت حرف* remap_Short_وقت_Zone(ثابت حرف *stdID, ثابت حرف *dstID, int32_t daylightType, int32_t انحراف)
{
    int32_t idx;
#اگر_تعریف_شدہ DEBUG_TZNAME
    fprintf(stderr, "TZ=%s std=%s dst=%s daylight=%d offset=%d\n", getenv("TZ"), stdID, dstID, daylightType, انحراف);
#اگر_ختم
    برائے (idx = 0; idx < UPRV_LENGTHOF(انحراف_ZONE_MAPPINGS); idx++)
    {
        اگر (انحراف == انحراف_ZONE_MAPPINGS[idx].انحراف_Seconds
            && daylightType == انحراف_ZONE_MAPPINGS[idx].daylightType
            && strcmp(انحراف_ZONE_MAPPINGS[idx].stdID, stdID) == 0
            && strcmp(انحراف_ZONE_MAPPINGS[idx].dstID, dstID) == 0)
        {
            واپس انحراف_ZONE_MAPPINGS[idx].olsonID;
        }
    }
    واپس خالی_اشاریہ;
}
#اگر_ختم

#اگر_تعریف_شدہ تلاش_TZFILE
#تعریف_کرو MAX_READ_سائز 512

قسمتعریف ساخت DefaultTZInfo {
    حرف* defaultTZBuffer;
    int64_t default_TZFile_سائز;
    FILE* defaultTZFilePtr;
    int32_t defaultTZPosition;
} DefaultTZInfo;

/*
 * This method compares the two files given to see if they are a match.
 * It is currently use to compare two TZ files.
 */
ساکن UBool compare_ثنائی_Files(ثابت حرف* TZFileName, DefaultTZInfo* tzInfo) {
    FILE* file;
    int64_t سائز_File;
    int64_t سائز_File_بایاں;
    int32_t سائز_File_Read;
    int32_t سائز_File_مقصد_Read;
    حرف بفر_File[MAX_READ_سائز];
    UBool نتیجہ = سچ;

    U_ASSERT(tzInfo->defaultTZFilePtr != خالی_اشاریہ);
    file = fopen(TZFileName, "r");

    tzInfo->defaultTZPosition = 0; /* reset position to begin search */

    اگر (file != خالی_اشاریہ) {
        /* First check that the file size are equal. */
        اگر (tzInfo->default_TZFile_سائز == 0) {
            fseek(tzInfo->defaultTZFilePtr, 0, SEEK_END);
            tzInfo->default_TZFile_سائز = ftell(tzInfo->defaultTZFilePtr);
        }
        اگر (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            واپس جھوٹ;
        }
        سائز_File = ftell(file);
        اگر (سائز_File == -1) {
            fclose(file);
            واپس جھوٹ;
        }
        سائز_File_بایاں = سائز_File;

        اگر (سائز_File != tzInfo->default_TZFile_سائز) {
            نتیجہ = جھوٹ;
        } ورنہ {
            /* Store the data from the files in separate buffers and
             * compare each byte to determine equality.
             */
            اگر (tzInfo->defaultTZBuffer == خالی_اشاریہ) {
                اگر (fseek(tzInfo->defaultTZFilePtr, 0, SEEK_SET) != 0) {
                    fclose(file);
                    واپس جھوٹ;
                }
                tzInfo->defaultTZBuffer = static_cast<حرف*>(uprv_malloc(حجم(حرف) * tzInfo->default_TZFile_سائز));
                سائز_File_Read = fread(tzInfo->defaultTZBuffer, 1, tzInfo->default_TZFile_سائز, tzInfo->defaultTZFilePtr);
            }
            اگر (fseek(file, 0, SEEK_SET) != 0) {
                fclose(file);
                واپس جھوٹ;
            }
            جب(سائز_File_بایاں > 0) {
                uprv_memset(بفر_File, 0, MAX_READ_سائز);
                سائز_File_مقصد_Read = سائز_File_بایاں < MAX_READ_سائز ? سائز_File_بایاں : MAX_READ_سائز;

                سائز_File_Read = fread(بفر_File, 1, سائز_File_مقصد_Read, file);
                اگر (سائز_File_Read == -1) {
                    fclose(file);
                    واپس جھوٹ;
                }
                اگر (memcmp(tzInfo->defaultTZBuffer + tzInfo->defaultTZPosition, بفر_File, سائز_File_Read) != 0) {
                    نتیجہ = جھوٹ;
                    توڑو;
                }
                سائز_File_بایاں -= سائز_File_Read;
                tzInfo->defaultTZPosition += سائز_File_Read;
            }
        }
    } ورنہ {
        نتیجہ = جھوٹ;
    }

    اگر (file != خالی_اشاریہ) {
        fclose(file);
    }

    واپس نتیجہ;
}


/* dirent also lists two entries: "." and ".." that we can safely ignore. */
#تعریف_کرو SKIP1 "."
#تعریف_کرو SKIP2 ".."
ساکن UBool U_CALLCONV putil_cleanup();
ساکن CharString *g_تلاش_TZFile_نتیجہ = خالی_اشاریہ;

/*
 * This method recursively traverses the directory given for a matching TZ file and returns the first match.
 * This function is not thread safe - it uses a global, gSearchTZFileResult, to hold its results.
 */
ساکن حرف* تلاش_For_TZFile(ثابت حرف* راستہ, DefaultTZInfo* tzInfo) {
    DIR* dirp = خالی_اشاریہ;
    ساخت dirent* dir_اندراج = خالی_اشاریہ;
    حرف* نتیجہ = خالی_اشاریہ;
    UError_رمز حالت_2 = U_ZERO_ERROR;

    /* There is no reference file to compare against, so do not traverse the zoneinfo tree. */
    اگر (tzInfo->defaultTZFilePtr == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }

    /* Save the current path */
    CharString curpath(راستہ, -1, حالت_2);
    اگر (U_FAILURE(حالت_2)) {
        جاؤ cleanupAndReturn;
    }

    dirp = opendir(راستہ);
    اگر (dirp == خالی_اشاریہ) {
        جاؤ cleanupAndReturn;
    }

    اگر (g_تلاش_TZFile_نتیجہ == خالی_اشاریہ) {
        g_تلاش_TZFile_نتیجہ = نیا CharString;
        اگر (g_تلاش_TZFile_نتیجہ == خالی_اشاریہ) {
            جاؤ cleanupAndReturn;
        }
        ucln_common_رجسٹر_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    }

    /* Check each entry in the directory. */
    جب((dir_اندراج = readdir(dirp)) != خالی_اشاریہ) {
        ثابت حرف* dirName = dir_اندراج->d_name;
        اگر (uprv_strcmp(dirName, SKIP1) != 0 && uprv_strcmp(dirName, SKIP2) != 0
            && uprv_strcmp(TZFILE_SKIP, dirName) != 0 && uprv_strcmp(TZFILE_SKIP2, dirName) != 0) {
            /* Create a newpath with the new entry to test each entry in the directory. */
            CharString newpath(curpath, حالت_2);
            newpath.append(dirName, -1, حالت_2);
            اگر (U_FAILURE(حالت_2)) {
                توڑو;
            }

            DIR* subDirp = خالی_اشاریہ;
            اگر ((subDirp = opendir(newpath.ڈیٹا())) != خالی_اشاریہ) {
                /* If this new path is a directory, make a recursive call with the newpath. */
                closedir(subDirp);
                newpath.append('/', حالت_2);
                اگر (U_FAILURE(حالت_2)) {
                    توڑو;
                }
                نتیجہ = تلاش_For_TZFile(newpath.ڈیٹا(), tzInfo);
                /*
                 Have to get out here. Otherwise, we'd keep looking
                 and return the first match in the top-level directory
                 if there's a match in the top-level. If not, this function
                 would return nullptr and set gTimeZoneBufferPtr to nullptr in initDefault().
                 It worked without this in most cases because we have a fallback of calling
                 localtime_r to figure out the default timezone.
                */
                اگر (نتیجہ != خالی_اشاریہ)
                    توڑو;
            } ورنہ {
                اگر(compare_ثنائی_Files(newpath.ڈیٹا(), tzInfo)) {
                    int32_t amount_مقصد_Skip = حجم(TZZONEINFO) - 1;
                    اگر (amount_مقصد_Skip > newpath.length()) {
                        amount_مقصد_Skip = newpath.length();
                    }
                    ثابت حرف* zoneid = newpath.ڈیٹا() + amount_مقصد_Skip;
                    skipZoneIDPrefix(&zoneid);
                    g_تلاش_TZFile_نتیجہ->clear();
                    g_تلاش_TZFile_نتیجہ->append(zoneid, -1, حالت_2);
                    اگر (U_FAILURE(حالت_2)) {
                        توڑو;
                    }
                    نتیجہ = g_تلاش_TZFile_نتیجہ->ڈیٹا();
                    /* Get out after the first one found. */
                    توڑو;
                }
            }
        }
    }

  cleanupAndReturn:
    اگر (dirp) {
        closedir(dirp);
    }
    واپس نتیجہ;
}
#اگر_ختم

#اگر U_PLATFORM == U_PF_ANDROID
قسمتعریف صحیح(system_property_read_callback)(ثابت prop_info* info,
                                           خالی (*callback)(خالی* cookie,
                                                            ثابت حرف* name,
                                                            ثابت حرف* value,
                                                            uint32_t serial),
                                           خالی* cookie);
قسمتعریف صحیح(system_property_get)(ثابت حرف*, حرف*);

ساکن حرف g_Android_وقت_Zone[PROP_قدر_MAX] = { '\0' };

ساکن خالی u_property_read(خالی* cookie, ثابت حرف* name, ثابت حرف* value,
                            uint32_t serial) {
    uprv_strcpy((حرف* )cookie, value);
}
#اگر_ختم

U_CAPI خالی U_EXPORT2
uprv_tzname_clear_cache()
{
#اگر U_PLATFORM == U_PF_ANDROID
    /* Android's timezone is stored in system property. */
    g_Android_وقت_Zone[0] = '\0';
    خالی* libc = dlopen("libc.so", RTLD_NOLOAD);
    اگر (libc) {
        /* Android API 26+ has new API to get system property and old API
         * (__system_property_get) is deprecated */
        system_property_read_callback* property_read_callback =
            (system_property_read_callback*)dlsym(
                libc, "__system_property_read_callback");
        اگر (property_read_callback) {
            ثابت prop_info* info =
                __system_property_find("persist.sys.timezone");
            اگر (info) {
                property_read_callback(info, &u_property_read, g_Android_وقت_Zone);
            }
        } ورنہ {
            system_property_get* property_get =
                (system_property_get*)dlsym(libc, "__system_property_get");
            اگر (property_get) {
                property_get("persist.sys.timezone", g_Android_وقت_Zone);
            }
        }
        dlclose(libc);
    }
#اگر_ختم

#اگر defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    g_وقت_Zone_بفر_Ptr = خالی_اشاریہ;
#اگر_ختم
}

U_CAPI ثابت حرف* U_EXPORT2
uprv_tzname(صحیح n)
{
    (خالی)n; // Avoid unreferenced parameter warning.
    ثابت حرف *tzid = خالی_اشاریہ;
#اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
    tzid = uprv_detect_Windows_وقت_Zone();

    اگر (tzid != خالی_اشاریہ) {
        واپس tzid;
    }

#اگر_غیر_تعریف_شدہ U_TZNAME
    // The return value is free'd in timezone.cpp on Windows because
    // the other code path returns a pointer to a heap location.
    // If we don't have a name already, then tzname wouldn't be any
    // better, so just fall back.
    واپس uprv_strdup("");
#اگر_ختم // !U_TZNAME

#ورنہ

/*#if U_PLATFORM_IS_DARWIN_BASED
    int ret;

    tzid = getenv("TZFILE");
    if (tzid != nullptr) {
        return tzid;
    }
#endif*/

/* This code can be temporarily disabled to test tzname resolution later on. */
#اگر_غیر_تعریف_شدہ DEBUG_TZNAME
#اگر U_PLATFORM == U_PF_ANDROID
    tzid = g_Android_وقت_Zone;
#ورنہ
    tzid = getenv("TZ");
#اگر_ختم
    اگر (tzid != خالی_اشاریہ && isValidOlsonID(tzid)
#اگر U_PLATFORM == U_PF_SOLARIS
    /* Don't misinterpret TZ "localtime" on Solaris as a time zone name. */
        && uprv_strcmp(tzid, TZ_ENV_CHECK) != 0
#اگر_ختم
    ) {
        /* The colon forces tzset() to treat the remainder as zoneinfo path */
        اگر (tzid[0] == ':') {
            tzid++;
        }
        /* This might be a good Olson ID. */
        skipZoneIDPrefix(&tzid);
        واپس tzid;
    }
    /* else U_TZNAME will give a better result. */
#اگر_ختم

#اگر defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    /* Caller must handle threading issues */
    اگر (g_وقت_Zone_بفر_Ptr == خالی_اشاریہ) {
        /*
        This is a trick to look at the name of the link to get the Olson ID
        because the tzfile contents is underspecified.
        This isn't guaranteed to work because it may not be a symlink.
        */
        حرف *ret = realpath(TZDEFAULT, g_وقت_Zone_بفر);
        اگر (ret != خالی_اشاریہ && uprv_strcmp(TZDEFAULT, g_وقت_Zone_بفر) != 0) {
            int32_t tz_Zone_Info_دم_Len = uprv_strlen(TZZONEINFOTAIL);
            ثابت حرف *tz_Zone_Info_دم_Ptr = uprv_strstr(g_وقت_Zone_بفر, TZZONEINFOTAIL);
            // MacOS14 has the realpath as something like
            // /usr/share/zoneinfo.default/Australia/Melbourne
            // which will not have "/zoneinfo/" in the path.
            // Therefore if we fail, we fall back to read the link which is
            // /var/db/timezone/zoneinfo/Australia/Melbourne
            // We also fall back to reading the link if the realpath leads to something like
            // /usr/share/zoneinfo/posixrules
            اگر (tz_Zone_Info_دم_Ptr == خالی_اشاریہ ||
                    uprv_strcmp(tz_Zone_Info_دم_Ptr + tz_Zone_Info_دم_Len, "posixrules") == 0) {
                ssize_t size = readlink(TZDEFAULT, g_وقت_Zone_بفر, حجم(g_وقت_Zone_بفر)-1);
                اگر (size > 0) {
                    g_وقت_Zone_بفر[size] = 0;
                    tz_Zone_Info_دم_Ptr = uprv_strstr(g_وقت_Zone_بفر, TZZONEINFOTAIL);
                }
            }
            اگر (tz_Zone_Info_دم_Ptr != خالی_اشاریہ) {
                tz_Zone_Info_دم_Ptr += tz_Zone_Info_دم_Len;
                skipZoneIDPrefix(&tz_Zone_Info_دم_Ptr);
                اگر (isValidOlsonID(tz_Zone_Info_دم_Ptr)) {
                    واپس (g_وقت_Zone_بفر_Ptr = tz_Zone_Info_دم_Ptr);
                }
            }
        } ورنہ {
#اگر defined(تلاش_TZFILE)
            DefaultTZInfo* tzInfo = (DefaultTZInfo*)uprv_malloc(حجم(DefaultTZInfo));
            اگر (tzInfo != خالی_اشاریہ) {
                tzInfo->defaultTZBuffer = خالی_اشاریہ;
                tzInfo->default_TZFile_سائز = 0;
                tzInfo->defaultTZFilePtr = fopen(TZDEFAULT, "r");
                tzInfo->defaultTZPosition = 0;

                g_وقت_Zone_بفر_Ptr = تلاش_For_TZFile(TZZONEINFO, tzInfo);

                /* Free previously allocated memory */
                اگر (tzInfo->defaultTZBuffer != خالی_اشاریہ) {
                    uprv_free(tzInfo->defaultTZBuffer);
                }
                اگر (tzInfo->defaultTZFilePtr != خالی_اشاریہ) {
                    fclose(tzInfo->defaultTZFilePtr);
                }
                uprv_free(tzInfo);
            }

            اگر (g_وقت_Zone_بفر_Ptr != خالی_اشاریہ && isValidOlsonID(g_وقت_Zone_بفر_Ptr)) {
                واپس g_وقت_Zone_بفر_Ptr;
            }
#اگر_ختم
        }
    }
    ورنہ {
        واپس g_وقت_Zone_بفر_Ptr;
    }
#اگر_ختم
#اگر_ختم

#اگر_تعریف_شدہ U_TZNAME
#اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
    /* The return value is free'd in timezone.cpp on Windows because
     * the other code path returns a pointer to a heap location. */
    واپس uprv_strdup(U_TZNAME[n]);
#ورنہ
    /*
    U_TZNAME is usually a non-unique abbreviation, which isn't normally usable.
    So we remap the abbreviation to an olson ID.

    Since Windows exposes a little more timezone information,
    we normally don't use this code on Windows because
    uprv_detectWindowsTimeZone should have already given the correct answer.
    */
    {
        ساخت tm juneSol, decemberSol;
        صحیح daylightType;
        ساکن ثابت time_t juneSolstice=1182478260; /*2007-06-21 18:11 UT*/
        ساکن ثابت time_t decemberSolstice=1198332540; /*2007-12-22 06:09 UT*/

        /* This probing will tell us when daylight savings occurs.  */
        localtime_r(&juneSolstice, &juneSol);
        localtime_r(&decemberSolstice, &decemberSol);
        اگر(decemberSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_DECEMBER;
        } ورنہ اگر(juneSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_JUNE;
        } ورنہ {
          daylightType = U_DAYLIGHT_NONE;
        }
        tzid = remap_Short_وقت_Zone(U_TZNAME[0], U_TZNAME[1], daylightType, uprv_timezone());
        اگر (tzid != خالی_اشاریہ) {
            واپس tzid;
        }
    }
    واپس U_TZNAME[n];
#اگر_ختم
#ورنہ
    واپس "";
#اگر_ختم
}

/* Get and set the ICU data directory --------------------------------------- */

ساکن icu::UInitOnce g_ڈیٹا_Dir_Init_Once {};
ساکن حرف *g_ڈیٹا_Directory = خالی_اشاریہ;

UInitOnce g_وقت_Zone_Files_Init_Once {};
ساکن CharString *g_وقت_Zone_Files_Directory = خالی_اشاریہ;

#اگر U_POSIX_LOCALE || U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
 ساکن ثابت حرف *gCorrectedPOSIXLocale = خالی_اشاریہ; /* Sometimes heap allocated */
 ساکن منطقی g_Corrected_POSIXLocale_ڈھیر_Allocated = جھوٹ;
#اگر_ختم

ساکن UBool U_CALLCONV putil_cleanup()
{
    اگر (g_ڈیٹا_Directory && *g_ڈیٹا_Directory) {
        uprv_free(g_ڈیٹا_Directory);
    }
    g_ڈیٹا_Directory = خالی_اشاریہ;
    g_ڈیٹا_Dir_Init_Once.reset();

    حذف g_وقت_Zone_Files_Directory;
    g_وقت_Zone_Files_Directory = خالی_اشاریہ;
    g_وقت_Zone_Files_Init_Once.reset();

#اگر_تعریف_شدہ تلاش_TZFILE
    حذف g_تلاش_TZFile_نتیجہ;
    g_تلاش_TZFile_نتیجہ = خالی_اشاریہ;
#اگر_ختم

#اگر U_POSIX_LOCALE || U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
    اگر (gCorrectedPOSIXLocale && g_Corrected_POSIXLocale_ڈھیر_Allocated) {
        uprv_free(const_cast<حرف *>(gCorrectedPOSIXLocale));
        gCorrectedPOSIXLocale = خالی_اشاریہ;
        g_Corrected_POSIXLocale_ڈھیر_Allocated = جھوٹ;
    }
#اگر_ختم
    واپس سچ;
}

/*
 * Set the data directory.
 *    Make a copy of the passed string, and set the global data dir to point to it.
 */
U_CAPI خالی U_EXPORT2
u_سیٹ_ڈیٹا_Directory(ثابت حرف *directory) {
    حرف *new_ڈیٹا_Dir;
    int32_t length;

    اگر(directory==خالی_اشاریہ || *directory==0) {
        /* A small optimization to prevent the malloc and copy when the
        shared library is used, and this is a way to make sure that nullptr
        is never returned.
        */
        new_ڈیٹا_Dir = (حرف *)"";
    }
    ورنہ {
        length=(int32_t)uprv_strlen(directory);
        new_ڈیٹا_Dir = (حرف *)uprv_malloc(length + 2);
        /* Exit out if newDataDir could not be created. */
        اگر (new_ڈیٹا_Dir == خالی_اشاریہ) {
            واپس;
        }
        uprv_strcpy(new_ڈیٹا_Dir, directory);

#اگر (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
        {
            حرف *p;
            جب((p = uprv_strchr(new_ڈیٹا_Dir, U_FILE_ALT_SEP_CHAR)) != خالی_اشاریہ) {
                *p = U_FILE_SEP_CHAR;
            }
        }
#اگر_ختم
    }

    اگر (g_ڈیٹا_Directory && *g_ڈیٹا_Directory) {
        uprv_free(g_ڈیٹا_Directory);
    }
    g_ڈیٹا_Directory = new_ڈیٹا_Dir;
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
}

U_CAPI UBool U_EXPORT2
uprv_راستہ_Is_Absolute(ثابت حرف *راستہ)
{
  اگر(!راستہ || !*راستہ) {
    واپس جھوٹ;
  }

  اگر(*راستہ == U_FILE_SEP_CHAR) {
    واپس سچ;
  }

#اگر (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
  اگر(*راستہ == U_FILE_ALT_SEP_CHAR) {
    واپس سچ;
  }
#اگر_ختم

#اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
  اگر( (((راستہ[0] >= 'A') && (راستہ[0] <= 'Z')) ||
       ((راستہ[0] >= 'a') && (راستہ[0] <= 'z'))) &&
      راستہ[1] == ':' ) {
    واپس سچ;
  }
#اگر_ختم

  واپس جھوٹ;
}

/* Backup setting of ICU_DATA_DIR_PREFIX_ENV_VAR
   (needed for some Darwin ICU build environments) */
#اگر U_PLATFORM_IS_DARWIN_BASED && defined(ہدف_OS_SIMULATOR) && ہدف_OS_SIMULATOR
# اگر !defined(ICU_ڈیٹا_DIR_سابقہ_ENV_VAR)
#  define ICU_ڈیٹا_DIR_سابقہ_ENV_VAR "IPHONE_SIMULATOR_ROOT"
# endif
#اگر_ختم

#اگر defined(ICU_ڈیٹا_DIR_WINDOWS)
// Helper function to get the ICU Data Directory under the Windows directory location.
ساکن BOOL U_CALLCONV get_Icu_ڈیٹا_Directory_Under_Windows_Directory(حرف* directory_بفر, UINT بفر_لمبائی)
{
    wchar_t windows_راستہ[MAX_راستہ];
    حرف windows_راستہ_Utf8[MAX_راستہ];

    UINT length = GetSystemWindowsDirectoryW(windows_راستہ, UPRV_LENGTHOF(windows_راستہ));
    اگر ((length > 0) && (length < (UPRV_LENGTHOF(windows_راستہ) - 1))) {
        // Convert UTF-16 to a UTF-8 string.
        UError_رمز حالت_2 = U_ZERO_ERROR;
        int32_t windows_راستہ_Utf8_Len = 0;
        u_str_مقصد_UTF8(windows_راستہ_Utf8, static_cast<int32_t>(UPRV_LENGTHOF(windows_راستہ_Utf8)),
            &windows_راستہ_Utf8_Len, reinterpret_cast<ثابت char16_t*>(windows_راستہ), -1, &حالت_2);

        اگر (U_SUCCESS(حالت_2) && (حالت_2 != U_STRING_NOT_TERMINATED_WARNING) &&
            (windows_راستہ_Utf8_Len < (UPRV_LENGTHOF(windows_راستہ_Utf8) - 1))) {
            // Ensure it always has a separator, so we can append the ICU data path.
            اگر (windows_راستہ_Utf8[windows_راستہ_Utf8_Len - 1] != U_FILE_SEP_CHAR) {
                windows_راستہ_Utf8[windows_راستہ_Utf8_Len++] = U_FILE_SEP_CHAR;
                windows_راستہ_Utf8[windows_راستہ_Utf8_Len] = '\0';
            }
            // Check if the concatenated string will fit.
            اگر ((windows_راستہ_Utf8_Len + UPRV_LENGTHOF(ICU_ڈیٹا_DIR_WINDOWS)) < بفر_لمبائی) {
                uprv_strcpy(directory_بفر, windows_راستہ_Utf8);
                uprv_strcat(directory_بفر, ICU_ڈیٹا_DIR_WINDOWS);
                واپس سچ;
            }
        }
    }

    واپس جھوٹ;
}
#اگر_ختم

ساکن خالی U_CALLCONV ڈیٹا_Directory_Init_Fn() {
    /* If we already have the directory, then return immediately. Will happen if user called
     * u_setDataDirectory().
     */
    اگر (g_ڈیٹا_Directory) {
        واپس;
    }

    ثابت حرف *راستہ = خالی_اشاریہ;

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
#   اگر !defined(ICU_NO_USER_ڈیٹا_OVERRIDE) && !UCONFIG_NO_FILE_IO
    /* First try to get the environment variable */
#     اگر U_PLATFORM_HAS_WINUWP_API == 0  // Windows UWP does not support getenv
        راستہ=getenv("ICU_DATA");
#     endif
#   endif

    /* ICU_DATA_DIR may be set as a compile option.
     * U_ICU_DATA_DEFAULT_DIR is provided and is set by ICU at compile time
     * and is used only when data is built in archive mode eliminating the need
     * for ICU_DATA_DIR to be set. U_ICU_DATA_DEFAULT_DIR is set to the installation
     * directory of the data dat file. Users should use ICU_DATA_DIR if they want to
     * set their own path.
     */
#اگر defined(ICU_ڈیٹا_DIR) || defined(U_ICU_ڈیٹا_DEFAULT_DIR)
# اگر defined(ICU_ڈیٹا_DIR_سابقہ_ENV_VAR)
    حرف datadir_راستہ_بفر[راستہ_MAX];
# endif
    اگر(راستہ==خالی_اشاریہ || *راستہ==0) {
# اگر defined(ICU_ڈیٹا_DIR_سابقہ_ENV_VAR)
        ثابت حرف *سابقہ = getenv(ICU_ڈیٹا_DIR_سابقہ_ENV_VAR);
# endif
# ifdef ICU_ڈیٹا_DIR
        راستہ=ICU_ڈیٹا_DIR;
# ورنہ
        راستہ=U_ICU_ڈیٹا_DEFAULT_DIR;
# endif
# اگر defined(ICU_ڈیٹا_DIR_سابقہ_ENV_VAR)
        اگر (سابقہ != خالی_اشاریہ) {
            snprintf(datadir_راستہ_بفر, حجم(datadir_راستہ_بفر), "%s%s", سابقہ, راستہ);
            راستہ=datadir_راستہ_بفر;
        }
# endif
    }
#اگر_ختم

#اگر defined(ICU_ڈیٹا_DIR_WINDOWS)
    حرف datadir_راستہ_بفر[MAX_راستہ];
    اگر (get_Icu_ڈیٹا_Directory_Under_Windows_Directory(datadir_راستہ_بفر, UPRV_LENGTHOF(datadir_راستہ_بفر))) {
        راستہ = datadir_راستہ_بفر;
    }
#اگر_ختم

    اگر(راستہ==خالی_اشاریہ) {
        /* It looks really bad, set it to something. */
        راستہ = "";
    }

    u_سیٹ_ڈیٹا_Directory(راستہ);
}

U_CAPI ثابت حرف * U_EXPORT2
u_get_ڈیٹا_Directory() {
    umtx_initOnce(g_ڈیٹا_Dir_Init_Once, &ڈیٹا_Directory_Init_Fn);
    واپس g_ڈیٹا_Directory;
}

ساکن خالی سیٹ_وقت_Zone_Files_Dir(ثابت حرف *راستہ, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        واپس;
    }
    g_وقت_Zone_Files_Directory->clear();
    g_وقت_Zone_Files_Directory->append(راستہ, حالت_2);
#اگر (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    حرف *p = g_وقت_Zone_Files_Directory->ڈیٹا();
    جب ((p = uprv_strchr(p, U_FILE_ALT_SEP_CHAR)) != خالی_اشاریہ) {
        *p = U_FILE_SEP_CHAR;
    }
#اگر_ختم
}

#تعریف_کرو مقصد_STRING(x) مقصد_STRING_2(x)
#تعریف_کرو مقصد_STRING_2(x) #x

ساکن خالی U_CALLCONV وقت_Zone_ڈیٹا_Dir_Init_Fn(UError_رمز &حالت_2) {
    U_ASSERT(g_وقت_Zone_Files_Directory == خالی_اشاریہ);
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    g_وقت_Zone_Files_Directory = نیا CharString();
    اگر (g_وقت_Zone_Files_Directory == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    ثابت حرف *dir = "";

#اگر defined(ICU_TIMEZONE_FILES_DIR_سابقہ_ENV_VAR)
    حرف timezonefilesdir_راستہ_بفر[راستہ_MAX];
    ثابت حرف *سابقہ = getenv(ICU_TIMEZONE_FILES_DIR_سابقہ_ENV_VAR);
#اگر_ختم

#اگر U_PLATFORM_HAS_WINUWP_API == 1
// The UWP version does not support the environment variable setting.

# اگر defined(ICU_ڈیٹا_DIR_WINDOWS)
    // When using the Windows system data, we can possibly pick up time zone data from the Windows directory.
    حرف datadir_راستہ_بفر[MAX_راستہ];
    اگر (get_Icu_ڈیٹا_Directory_Under_Windows_Directory(datadir_راستہ_بفر, UPRV_LENGTHOF(datadir_راستہ_بفر))) {
        dir = datadir_راستہ_بفر;
    }
# endif

#ورنہ
    dir = getenv("ICU_TIMEZONE_FILES_DIR");
#اگر_ختم // U_PLATFORM_HAS_WINUWP_API

#اگر defined(U_TIMEZONE_FILES_DIR)
    اگر (dir == خالی_اشاریہ) {
        // Build time configuration setting.
        dir = مقصد_STRING(U_TIMEZONE_FILES_DIR);
    }
#اگر_ختم

    اگر (dir == خالی_اشاریہ) {
        dir = "";
    }

#اگر defined(ICU_TIMEZONE_FILES_DIR_سابقہ_ENV_VAR)
    اگر (سابقہ != خالی_اشاریہ) {
        snprintf(timezonefilesdir_راستہ_بفر, حجم(timezonefilesdir_راستہ_بفر), "%s%s", سابقہ, dir);
        dir = timezonefilesdir_راستہ_بفر;
    }
#اگر_ختم

    سیٹ_وقت_Zone_Files_Dir(dir, حالت_2);
}


U_CAPI ثابت حرف * U_EXPORT2
u_get_وقت_Zone_Files_Directory(UError_رمز *حالت_2) {
    umtx_initOnce(g_وقت_Zone_Files_Init_Once, &وقت_Zone_ڈیٹا_Dir_Init_Fn, *حالت_2);
    واپس U_SUCCESS(*حالت_2) ? g_وقت_Zone_Files_Directory->ڈیٹا() : "";
}

U_CAPI خالی U_EXPORT2
u_سیٹ_وقت_Zone_Files_Directory(ثابت حرف *راستہ, UError_رمز *حالت_2) {
    umtx_initOnce(g_وقت_Zone_Files_Init_Once, &وقت_Zone_ڈیٹا_Dir_Init_Fn, *حالت_2);
    سیٹ_وقت_Zone_Files_Dir(راستہ, *حالت_2);

    // Note: this function does some extra churn, first setting based on the
    //       environment, then immediately replacing with the value passed in.
    //       The logic is simpler that way, and performance shouldn't be an issue.
}


#اگر U_POSIX_LOCALE
/* A helper function used by uprv_getPOSIXIDForDefaultLocale and
 * uprv_getPOSIXIDForDefaultCodepage. Returns the posix locale id for
 * LC_CTYPE and LC_MESSAGES. It doesn't support other locale categories.
 */
ساکن ثابت حرف *uprv_getPOSIXIDForCategory(صحیح category)
{
    ثابت حرف* posixID = خالی_اشاریہ;
    اگر (category == LC_MESSAGES || category == LC_CTYPE) {
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
        posixID = setlocale(category, خالی_اشاریہ);
        اگر ((posixID == خالی_اشاریہ)
            || (uprv_strcmp("C", posixID) == 0)
            || (uprv_strcmp("POSIX", posixID) == 0))
        {
            /* Maybe we got some garbage.  Try something more reasonable */
            posixID = getenv("LC_ALL");
            /* Solaris speaks POSIX -  See IEEE Std 1003.1-2008
             * This is needed to properly handle empty env. variables
             */
#اگر U_PLATFORM == U_PF_SOLARIS
            اگر ((posixID == 0) || (posixID[0] == '\0')) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                اگر ((posixID == 0) || (posixID[0] == '\0')) {
#ورنہ
            اگر (posixID == خالی_اشاریہ) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                اگر (posixID == خالی_اشاریہ) {
#اگر_ختم
                    posixID = getenv("LANG");
                }
            }
        }
    }
    اگر ((posixID == خالی_اشاریہ)
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
    واپس posixID;
}

/* Return just the POSIX id for the default locale, whatever happens to be in
 * it. It gets the value from LC_MESSAGES and indirectly from LC_ALL and LANG.
 */
ساکن ثابت حرف *uprv_getPOSIXIDForDefaultLocale()
{
    ساکن ثابت حرف* posixID = خالی_اشاریہ;
    اگر (posixID == خالی_اشاریہ) {
        posixID = uprv_getPOSIXIDForCategory(LC_MESSAGES);
    }
    واپس posixID;
}

#اگر !U_CHARSET_IS_UTF8
/* Return just the POSIX id for the default codepage, whatever happens to be in
 * it. It gets the value from LC_CTYPE and indirectly from LC_ALL and LANG.
 */
ساکن ثابت حرف *uprv_getPOSIXIDForDefaultCodepage()
{
    ساکن ثابت حرف* posixID = خالی_اشاریہ;
    اگر (posixID == 0) {
        posixID = uprv_getPOSIXIDForCategory(LC_CTYPE);
    }
    واپس posixID;
}
#اگر_ختم
#اگر_ختم

/* NOTE: The caller should handle thread safety */
U_CAPI ثابت حرف* U_EXPORT2
uprv_getDefaultLocaleID()
{
#اگر U_POSIX_LOCALE
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
    ثابت حرف* posixID = uprv_getPOSIXIDForDefaultLocale();

    /* Format: (no spaces)
    ll [ _CC ] [ . MM ] [ @ VV]

      l = lang, C = ctry, M = charmap, V = variant
    */

    اگر (gCorrectedPOSIXLocale != خالی_اشاریہ) {
        واپس gCorrectedPOSIXLocale;
    }

    // Copy the ID into owned memory.
    // Over-allocate in case we replace "C" with "en_US_POSIX" (+10), + null termination
    حرف *correctedPOSIXLocale = static_cast<حرف *>(uprv_malloc(uprv_strlen(posixID) + 10 + 1));
    اگر (correctedPOSIXLocale == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    uprv_strcpy(correctedPOSIXLocale, posixID);

    حرف *حد_بندی;
    اگر ((حد_بندی = uprv_strchr(correctedPOSIXLocale, '.')) != خالی_اشاریہ) {
        *حد_بندی = 0;
    }
    اگر ((حد_بندی = uprv_strchr(correctedPOSIXLocale, '@')) != خالی_اشاریہ) {
        *حد_بندی = 0;
    }

    اگر ((uprv_strcmp("C", correctedPOSIXLocale) == 0) // no @ variant
        || (uprv_strcmp("POSIX", correctedPOSIXLocale) == 0)) {
      // Raw input was C.* or POSIX.*, Give it a nice POSIX default value.
      // (The "C"/"POSIX" case is handled in uprv_getPOSIXIDForCategory())
      uprv_strcpy(correctedPOSIXLocale, "en_US_POSIX");
    }

    /* Note that we scan the *uncorrected* ID. */
    ثابت حرف *p;
    اگر ((p = uprv_strrchr(posixID, '@')) != خالی_اشاریہ) {
        p++;

        /* Take care of any special cases here.. */
        اگر (!uprv_strcmp(p, "nynorsk")) {
            p = "NY";
            /* Don't worry about no__NY. In practice, it won't appear. */
        }

        اگر (uprv_strchr(correctedPOSIXLocale,'_') == خالی_اشاریہ) {
            uprv_strcat(correctedPOSIXLocale, "__"); /* aa@b -> aa__b (note this can make the new locale 1 char longer) */
        }
        ورنہ {
            uprv_strcat(correctedPOSIXLocale, "_"); /* aa_CC@b -> aa_CC_b */
        }

        ثابت حرف *q;
        اگر ((q = uprv_strchr(p, '.')) != خالی_اشاریہ) {
            /* How big will the resulting string be? */
            int32_t len = (int32_t)(uprv_strlen(correctedPOSIXLocale) + (q-p));
            uprv_strncat(correctedPOSIXLocale, p, q-p); // do not include charset
            correctedPOSIXLocale[len] = 0;
        }
        ورنہ {
            /* Anything following the @ sign */
            uprv_strcat(correctedPOSIXLocale, p);
        }

        /* Should there be a map from 'no@nynorsk' -> no_NO_NY here?
         * How about 'russian' -> 'ru'?
         * Many of the other locales using ISO codes will be handled by the
         * canonicalization functions in uloc_getDefault.
         */
    }

    اگر (gCorrectedPOSIXLocale == خالی_اشاریہ) {
        gCorrectedPOSIXLocale = correctedPOSIXLocale;
        g_Corrected_POSIXLocale_ڈھیر_Allocated = سچ;
        ucln_common_رجسٹر_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
        correctedPOSIXLocale = خالی_اشاریہ;
    }
    posixID = gCorrectedPOSIXLocale;

    اگر (correctedPOSIXLocale != خالی_اشاریہ) {  /* Was already set - clean up. */
        uprv_free(correctedPOSIXLocale);
    }

    واپس posixID;

#وگرنہ_اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
#تعریف_کرو POSIX_LOCALE_استعداد 64
    UError_رمز حالت_2 = U_ZERO_ERROR;
    حرف *correctedPOSIXLocale = خالی_اشاریہ;

    // If we have already figured this out just use the cached value
    اگر (gCorrectedPOSIXLocale != خالی_اشاریہ) {
        واپس gCorrectedPOSIXLocale;
    }

    // No cached value, need to determine the current value
    ساکن WCHAR windowsLocale[LOCALE_NAME_MAX_لمبائی] = {};
    صحیح length = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, windowsLocale, LOCALE_NAME_MAX_لمبائی);

    // Now we should have a Windows locale name that needs converted to the POSIX style.
    اگر (length > 0) // If length is 0, then the GetLocaleInfoEx failed.
    {
        // First we need to go from UTF-16 to char (and also convert from _ to - while we're at it.)
        حرف modifiedWindowsLocale[LOCALE_NAME_MAX_لمبائی] = {};

        int32_t i;
        برائے (i = 0; i < UPRV_LENGTHOF(modifiedWindowsLocale); i++)
        {
            اگر (windowsLocale[i] == '_')
            {
                modifiedWindowsLocale[i] = '-';
            }
            ورنہ
            {
                modifiedWindowsLocale[i] = static_cast<حرف>(windowsLocale[i]);
            }

            اگر (modifiedWindowsLocale[i] == '\0')
            {
                توڑو;
            }
        }

        اگر (i >= UPRV_LENGTHOF(modifiedWindowsLocale))
        {
            // Ran out of room, can't really happen, maybe we'll be lucky about a matching
            // locale when tags are dropped
            modifiedWindowsLocale[UPRV_LENGTHOF(modifiedWindowsLocale) - 1] = '\0';
        }

        // Now normalize the resulting name
        correctedPOSIXLocale = static_cast<حرف *>(uprv_malloc(POSIX_LOCALE_استعداد + 1));
        /* TODO: Should we just exit on memory allocation failure? */
        اگر (correctedPOSIXLocale)
        {
            int32_t posixLen = uloc_canonicalize(modifiedWindowsLocale, correctedPOSIXLocale, POSIX_LOCALE_استعداد, &حالت_2);
            اگر (U_SUCCESS(حالت_2))
            {
                *(correctedPOSIXLocale + posixLen) = 0;
                gCorrectedPOSIXLocale = correctedPOSIXLocale;
                g_Corrected_POSIXLocale_ڈھیر_Allocated = سچ;
                ucln_common_رجسٹر_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
            }
            ورنہ
            {
                uprv_free(correctedPOSIXLocale);
            }
        }
    }

    // If unable to find a locale we can agree upon, use en-US by default
    اگر (gCorrectedPOSIXLocale == خالی_اشاریہ) {
        gCorrectedPOSIXLocale = "en_US";
    }
    واپس gCorrectedPOSIXLocale;

#وگرنہ_اگر U_PLATFORM == U_PF_OS400
    /* locales are process scoped and are by definition thread safe */
    ساکن حرف correctedLocale[64];
    ثابت  حرف *localeID = getenv("LC_ALL");
           حرف *p;

    اگر (localeID == خالی_اشاریہ)
        localeID = getenv("LANG");
    اگر (localeID == خالی_اشاریہ)
        localeID = setlocale(LC_ALL, خالی_اشاریہ);
    /* Make sure we have something... */
    اگر (localeID == خالی_اشاریہ)
        واپس "en_US_POSIX";

    /* Extract the locale name from the path. */
    اگر((p = uprv_strrchr(localeID, '/')) != خالی_اشاریہ)
    {
        /* Increment p to start of locale name. */
        p++;
        localeID = p;
    }

    /* Copy to work location. */
    uprv_strcpy(correctedLocale, localeID);

    /* Strip off the '.locale' extension. */
    اگر((p = uprv_strchr(correctedLocale, '.')) != خالی_اشاریہ) {
        *p = 0;
    }

    /* Upper case the locale name. */
    T_CString_مقصد_بالائی_Case(correctedLocale);

    /* See if we are using the POSIX locale.  Any of the
    * following are equivalent and use the same QLGPGCMA
    * (POSIX) locale.
    * QLGPGCMA2 means UCS2
    * QLGPGCMA_4 means UTF-32
    * QLGPGCMA_8 means UTF-8
    */
    اگر ((uprv_strcmp("C", correctedLocale) == 0) ||
        (uprv_strcmp("POSIX", correctedLocale) == 0) ||
        (uprv_strncmp("QLGPGCMA", correctedLocale, 8) == 0))
    {
        uprv_strcpy(correctedLocale, "en_US_POSIX");
    }
    ورنہ
    {
        int16_t LocaleLen;

        /* Lower case the lang portion. */
        برائے(p = correctedLocale; *p != 0 && *p != '_'; p++)
        {
            *p = uprv_tolower(*p);
        }

        /* Adjust for Euro.  After '_E' add 'URO'. */
        LocaleLen = uprv_strlen(correctedLocale);
        اگر (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'E')
        {
            uprv_strcat(correctedLocale, "URO");
        }

        /* If using Lotus-based locale then convert to
         * equivalent non Lotus.
         */
        ورنہ اگر (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'L')
        {
            correctedLocale[LocaleLen - 2] = 0;
        }

        /* There are separate simplified and traditional
         * locales called zh_HK_S and zh_HK_T.
         */
        ورنہ اگر (uprv_strncmp(correctedLocale, "zh_HK", 5) == 0)
        {
            uprv_strcpy(correctedLocale, "zh_HK");
        }

        /* A special zh_CN_GBK locale...
        */
        ورنہ اگر (uprv_strcmp(correctedLocale, "zh_CN_GBK") == 0)
        {
            uprv_strcpy(correctedLocale, "zh_CN");
        }

    }

    واپس correctedLocale;
#اگر_ختم

}

#اگر !U_CHARSET_IS_UTF8
#اگر U_POSIX_LOCALE
/*
Due to various platform differences, one platform may specify a charset,
when they really mean a different charset. Remap the names so that they are
compatible with ICU. Only conflicting/ambiguous aliases should be resolved
here. Before adding anything to this function, please consider adding unique
names to the ICU alias table in the data directory.
*/
ساکن ثابت حرف*
remapPlatformDependentCodepage(ثابت حرف *locale, ثابت حرف *name) {
    اگر (locale != خالی_اشاریہ && *locale == 0) {
        /* Make sure that an empty locale is handled the same way. */
        locale = خالی_اشاریہ;
    }
    اگر (name == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
#اگر U_PLATFORM == U_PF_AIX
    اگر (uprv_strcmp(name, "IBM-943") == 0) {
        /* Use the ASCII compatible ibm-943 */
        name = "Shift-JIS";
    }
    ورنہ اگر (uprv_strcmp(name, "IBM-1252") == 0) {
        /* Use the windows-1252 that contains the Euro */
        name = "IBM-5348";
    }
#وگرنہ_اگر U_PLATFORM == U_PF_SOLARIS
    اگر (locale != خالی_اشاریہ && uprv_strcmp(name, "EUC") == 0) {
        /* Solaris underspecifies the "EUC" name. */
        اگر (uprv_strcmp(locale, "zh_CN") == 0) {
            name = "EUC-CN";
        }
        ورنہ اگر (uprv_strcmp(locale, "zh_TW") == 0) {
            name = "EUC-TW";
        }
        ورنہ اگر (uprv_strcmp(locale, "ko_KR") == 0) {
            name = "EUC-KR";
        }
    }
    ورنہ اگر (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-954 is the best match.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    ورنہ اگر (uprv_strcmp(name, "646") == 0) {
        /*
         * The default codepage given by Solaris is 646 but the C library routines treat it as if it was
         * ISO-8859-1 instead of US-ASCII(646).
         */
        name = "ISO-8859-1";
    }
#وگرنہ_اگر U_PLATFORM_IS_DARWIN_BASED
    اگر (locale == خالی_اشاریہ && *name == 0) {
        /*
        No locale was specified, and an empty name was passed in.
        This usually indicates that nl_langinfo didn't return valid information.
        Mac OS X uses UTF-8 by default (especially the locale data and console).
        */
        name = "UTF-8";
    }
    ورنہ اگر (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
    ورنہ اگر (locale != خالی_اشاریہ && uprv_strcmp(locale, "en_US_POSIX") != 0 && uprv_strcmp(name, "US-ASCII") == 0) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
#وگرنہ_اگر U_PLATFORM == U_PF_BSD
    اگر (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
#وگرنہ_اگر U_PLATFORM == U_PF_HPUX
    اگر (locale != خالی_اشاریہ && uprv_strcmp(locale, "zh_HK") == 0 && uprv_strcmp(name, "big5") == 0) {
        /* HP decided to extend big5 as hkbig5 even though it's not compatible :-( */
        /* zh_TW.big5 is not the same charset as zh_HK.big5! */
        name = "hkbig5";
    }
    ورنہ اگر (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
#وگرنہ_اگر U_PLATFORM == U_PF_LINUX
    اگر (locale != خالی_اشاریہ && uprv_strcmp(name, "euc") == 0) {
        /* Linux underspecifies the "EUC" name. */
        اگر (uprv_strcmp(locale, "korean") == 0) {
            name = "EUC-KR";
        }
        ورنہ اگر (uprv_strcmp(locale, "japanese") == 0) {
            /* See comment below about eucJP */
            name = "eucjis";
        }
    }
    ورنہ اگر (uprv_strcmp(name, "eucjp") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    ورنہ اگر (locale != خالی_اشاریہ && uprv_strcmp(locale, "en_US_POSIX") != 0 &&
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
#اگر_ختم
    /* return nullptr when "" is passed in */
    اگر (*name == 0) {
        name = خالی_اشاریہ;
    }
    واپس name;
}

ساکن ثابت حرف*
getCodepageFromPOSIXID(ثابت حرف *localeName, حرف * بفر, int32_t buff_استعداد)
{
    حرف localeBuf[100];
    ثابت حرف *name = خالی_اشاریہ;
    حرف *variant = خالی_اشاریہ;

    اگر (localeName != خالی_اشاریہ && (name = (uprv_strchr(localeName, '.'))) != خالی_اشاریہ) {
        size_t locale_استعداد = uprv_min(حجم(localeBuf), (name-localeName)+1);
        uprv_strncpy(localeBuf, localeName, locale_استعداد);
        localeBuf[locale_استعداد-1] = 0; /* ensure NUL termination */
        name = uprv_strncpy(بفر, name+1, buff_استعداد);
        بفر[buff_استعداد-1] = 0; /* ensure NUL termination */
        اگر ((variant = const_cast<حرف *>(uprv_strchr(name, '@'))) != خالی_اشاریہ) {
            *variant = 0;
        }
        name = remapPlatformDependentCodepage(localeBuf, name);
    }
    واپس name;
}
#اگر_ختم

ساکن ثابت حرف*
int_getDefaultCodepage()
{
#اگر U_PLATFORM == U_PF_OS400
    uint32_t ccsid = 37; /* Default to ibm-37 */
    ساکن حرف codepage[64];
    Qwc_JOBI0400_t jobinfo;
    Qus_EC_t error = { حجم(Qus_EC_t) }; /* SPI error code */

    EPT_CALL(QUSRJOBI)(&jobinfo, حجم(jobinfo), "JOBI0400",
        "*                         ", "                ", &error);

    اگر (error.بائٹیں_دستیاب == 0) {
        اگر (jobinfo.Coded_Char_سیٹ_ID != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Coded_Char_سیٹ_ID;
        }
        ورنہ اگر (jobinfo.Default_Coded_Char_سیٹ_Id != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Default_Coded_Char_سیٹ_Id;
        }
        /* else use the default */
    }
    snprintf(codepage, حجم(codepage), "ibm-%d", ccsid);
    واپس codepage;

#وگرنہ_اگر U_PLATFORM == U_PF_OS390
    ساکن حرف codepage[64];

    strncpy(codepage, nl_langinfo(CODESET),63-لڑی_لمبائی(UCNV_تبادلہ_LFNL_OPTION_STRING));
    strcat(codepage,UCNV_تبادلہ_LFNL_OPTION_STRING);
    codepage[63] = 0; /* NUL terminate */

    واپس codepage;

#وگرنہ_اگر U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API
    ساکن حرف codepage[64];
    DWORD codepage_نمبر = 0;

#اگر U_PLATFORM_HAS_WINUWP_API == 1
    // UWP doesn't have a direct API to get the default ACP as Microsoft would rather
    // have folks use Unicode than a "system" code page, however this is the same
    // codepage as the system default locale codepage.  (FWIW, the system locale is
    // ONLY used for codepage, it should never be used for anything else)
    GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_نمبر,
        (LPWSTR)&codepage_نمبر, حجم(codepage_نمبر) / حجم(WCHAR));
#ورنہ
    // Win32 apps can call GetACP
    codepage_نمبر = GetACP();
#اگر_ختم
    // Special case for UTF-8
    اگر (codepage_نمبر == 65001)
    {
        واپس "UTF-8";
    }
    // Windows codepages can look like windows-1252, so format the found number
    // the numbers are eclectic, however all valid system code pages, besides UTF-8
    // are between 3 and 19999
    اگر (codepage_نمبر > 0 && codepage_نمبر < 20000)
    {
        snprintf(codepage, حجم(codepage), "windows-%ld", codepage_نمبر);
        واپس codepage;
    }
    // If the codepage number call failed then return UTF-8
    واپس "UTF-8";

#وگرنہ_اگر U_POSIX_LOCALE
    ساکن حرف codesetName[100];
    ثابت حرف *localeName = خالی_اشاریہ;
    ثابت حرف *name = خالی_اشاریہ;

    localeName = uprv_getPOSIXIDForDefaultCodepage();
    uprv_memset(codesetName, 0, حجم(codesetName));
    /* On Solaris nl_langinfo returns C locale values unless setlocale
     * was called earlier.
     */
#اگر (U_HAVE_NL_LANGINFO_CODESET && U_PLATFORM != U_PF_SOLARIS)
    /* When available, check nl_langinfo first because it usually gives more
       useful names. It depends on LC_CTYPE.
       nl_langinfo may use the same buffer as setlocale. */
    {
        ثابت حرف *codeset = nl_langinfo(U_NL_LANGINFO_CODESET);
#اگر U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED
        /*
         * On Linux and MacOSX, ensure that default codepage for non C/POSIX locale is UTF-8
         * instead of ASCII.
         */
        اگر (uprv_strcmp(localeName, "en_US_POSIX") != 0) {
            codeset = remapPlatformDependentCodepage(localeName, codeset);
        } ورنہ
#اگر_ختم
        {
            codeset = remapPlatformDependentCodepage(خالی_اشاریہ, codeset);
        }

        اگر (codeset != خالی_اشاریہ) {
            uprv_strncpy(codesetName, codeset, حجم(codesetName));
            codesetName[حجم(codesetName)-1] = 0;
            واپس codesetName;
        }
    }
#اگر_ختم

    /* Use setlocale in a nice way, and then check some environment variables.
       Maybe the application used setlocale already.
    */
    uprv_memset(codesetName, 0, حجم(codesetName));
    name = getCodepageFromPOSIXID(localeName, codesetName, حجم(codesetName));
    اگر (name) {
        /* if we can find the codeset name from setlocale, return that. */
        واپس name;
    }

    اگر (*codesetName == 0)
    {
        /* Everything failed. Return US ASCII (ISO 646). */
        (خالی)uprv_strcpy(codesetName, "US-ASCII");
    }
    واپس codesetName;
#ورنہ
    واپس "US-ASCII";
#اگر_ختم
}


U_CAPI ثابت حرف*  U_EXPORT2
uprv_getDefaultCodepage()
{
    ساکن حرف ثابت  *name = خالی_اشاریہ;
    umtx_تالا(خالی_اشاریہ);
    اگر (name == خالی_اشاریہ) {
        name = int_getDefaultCodepage();
    }
    umtx_unlock(خالی_اشاریہ);
    واپس name;
}
#اگر_ختم  /* !U_CHARSET_IS_UTF8 */


/* end of platform-specific implementation -------------- */

/* version handling --------------------------------------------------------- */

U_CAPI خالی U_EXPORT2
u_نسخہ_From_String(UVersionInfo نسخہ_Array, ثابت حرف *نسخہ_String) {
    حرف *انتہا;
    uint16_t part=0;

    اگر(نسخہ_Array==خالی_اشاریہ) {
        واپس;
    }

    اگر(نسخہ_String!=خالی_اشاریہ) {
        برائے(;;) {
            نسخہ_Array[part]=(uint8_t)uprv_strtoul(نسخہ_String, &انتہا, 10);
            اگر(انتہا==نسخہ_String || ++part==U_MAX_نسخہ_لمبائی || *انتہا!=U_نسخہ_DELIMITER) {
                توڑو;
            }
            نسخہ_String=انتہا+1;
        }
    }

    جب(part<U_MAX_نسخہ_لمبائی) {
        نسخہ_Array[part++]=0;
    }
}

U_CAPI خالی U_EXPORT2
u_نسخہ_From_UString(UVersionInfo نسخہ_Array, ثابت char16_t *نسخہ_String) {
    اگر(نسخہ_Array!=خالی_اشاریہ && نسخہ_String!=خالی_اشاریہ) {
        حرف نسخہ_Chars[U_MAX_نسخہ_STRING_لمبائی+1];
        int32_t len = u_strlen(نسخہ_String);
        اگر(len>U_MAX_نسخہ_STRING_لمبائی) {
            len = U_MAX_نسخہ_STRING_لمبائی;
        }
        u_UChars_مقصد_Chars(نسخہ_String, نسخہ_Chars, len);
        نسخہ_Chars[len]=0;
        u_نسخہ_From_String(نسخہ_Array, نسخہ_Chars);
    }
}

U_CAPI خالی U_EXPORT2
u_نسخہ_مقصد_String(ثابت UVersionInfo نسخہ_Array, حرف *نسخہ_String) {
    uint16_t count, part;
    uint8_t میدان_2;

    اگر(نسخہ_String==خالی_اشاریہ) {
        واپس;
    }

    اگر(نسخہ_Array==خالی_اشاریہ) {
        نسخہ_String[0]=0;
        واپس;
    }

    /* count how many fields need to be written */
    برائے(count=4; count>0 && نسخہ_Array[count-1]==0; --count) {
    }

    اگر(count <= 1) {
        count = 2;
    }

    /* write the first part */
    /* write the decimal field value */
    میدان_2=نسخہ_Array[0];
    اگر(میدان_2>=100) {
        *نسخہ_String++=(حرف)('0'+میدان_2/100);
        میدان_2%=100;
    }
    اگر(میدان_2>=10) {
        *نسخہ_String++=(حرف)('0'+میدان_2/10);
        میدان_2%=10;
    }
    *نسخہ_String++=(حرف)('0'+میدان_2);

    /* write the following parts */
    برائے(part=1; part<count; ++part) {
        /* write a dot first */
        *نسخہ_String++=U_نسخہ_DELIMITER;

        /* write the decimal field value */
        میدان_2=نسخہ_Array[part];
        اگر(میدان_2>=100) {
            *نسخہ_String++=(حرف)('0'+میدان_2/100);
            میدان_2%=100;
        }
        اگر(میدان_2>=10) {
            *نسخہ_String++=(حرف)('0'+میدان_2/10);
            میدان_2%=10;
        }
        *نسخہ_String++=(حرف)('0'+میدان_2);
    }

    /* NUL-terminate */
    *نسخہ_String=0;
}

U_CAPI خالی U_EXPORT2
u_get_نسخہ(UVersionInfo نسخہ_Array) {
    (خالی)copyright;   // Suppress unused variable warning from clang.
    u_نسخہ_From_String(نسخہ_Array, U_ICU_نسخہ);
}

/**
 * icucfg.h dependent code
 */

#اگر U_ENABLE_DYLOAD && HAVE_DLOPEN && !U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API

#اگر HAVE_DLFCN_H
#اگر_تعریف_شدہ __MVS__
#اگر_غیر_تعریف_شدہ __SUSV3
#تعریف_کرو __SUSV3 1
#اگر_ختم
#اگر_ختم
#شامل <dlfcn.h>
#اگر_ختم /* HAVE_DLFCN_H */

U_CAPI خالی * U_EXPORT2
uprv_dl_open(ثابت حرف *libName, UError_رمز *حالت_2) {
  خالی *ret = خالی_اشاریہ;
  اگر(U_FAILURE(*حالت_2)) واپس ret;
  ret =  dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
  اگر(ret==خالی_اشاریہ) {
#اگر_تعریف_شدہ U_TRACE_DYLOAD
    لکھو("dlerror on dlopen(%s): %s\n", libName, dlerror());
#اگر_ختم
    *حالت_2 = U_MISSING_RESOURCE_ERROR;
  }
  واپس ret;
}

U_CAPI خالی U_EXPORT2
uprv_dl_close(خالی *lib, UError_رمز *حالت_2) {
  اگر(U_FAILURE(*حالت_2)) واپس;
  dlclose(lib);
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(خالی *lib, ثابت حرف* sym, UError_رمز *حالت_2) {
  اتحاد {
      UVoidFunction *fp;
      خالی *vp;
  } uret;
  uret.fp = خالی_اشاریہ;
  اگر(U_FAILURE(*حالت_2)) واپس uret.fp;
  uret.vp = dlsym(lib, sym);
  اگر(uret.vp == خالی_اشاریہ) {
#اگر_تعریف_شدہ U_TRACE_DYLOAD
    لکھو("dlerror on dlsym(%p,%s): %s\n", lib,sym, dlerror());
#اگر_ختم
    *حالت_2 = U_MISSING_RESOURCE_ERROR;
  }
  واپس uret.fp;
}

#وگرنہ_اگر U_ENABLE_DYLOAD && U_PLATFORM_استعمال_کرتا_ONLY_WIN32_API && !U_PLATFORM_HAS_WINUWP_API

/* Windows API implementation. */
// Note: UWP does not expose/allow these APIs, so the UWP version gets the null implementation. */

U_CAPI خالی * U_EXPORT2
uprv_dl_open(ثابت حرف *libName, UError_رمز *حالت_2) {
  HMODULE lib = خالی_اشاریہ;

  اگر(U_FAILURE(*حالت_2)) واپس خالی_اشاریہ;

  lib = بھرو_Library_A(libName);

  اگر(lib==خالی_اشاریہ) {
    *حالت_2 = U_MISSING_RESOURCE_ERROR;
  }

  واپس (خالی*)lib;
}

U_CAPI خالی U_EXPORT2
uprv_dl_close(خالی *lib, UError_رمز *حالت_2) {
  HMODULE handle = (HMODULE)lib;
  اگر(U_FAILURE(*حالت_2)) واپس;

  FreeLibrary(handle);

  واپس;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(خالی *lib, ثابت حرف* sym, UError_رمز *حالت_2) {
  HMODULE handle = (HMODULE)lib;
  UVoidFunction* addr = خالی_اشاریہ;

  اگر(U_FAILURE(*حالت_2) || lib==خالی_اشاریہ) واپس خالی_اشاریہ;

  addr = (UVoidFunction*)Get_Proc_پتہ(handle, sym);

  اگر(addr==خالی_اشاریہ) {
    DWORD آخری_Error = Get_آخری_Error();
    اگر(آخری_Error == ERROR_PROC_NOT_ملا) {
      *حالت_2 = U_MISSING_RESOURCE_ERROR;
    } ورنہ {
      *حالت_2 = U_UNSUPPORTED_ERROR; /* other unknown error. */
    }
  }

  واپس addr;
}

#ورنہ

/* No dynamic loading, null (nonexistent) implementation. */

U_CAPI خالی * U_EXPORT2
uprv_dl_open(ثابت حرف *libName, UError_رمز *حالت_2) {
    (خالی)libName;
    اگر(U_FAILURE(*حالت_2)) واپس خالی_اشاریہ;
    *حالت_2 = U_UNSUPPORTED_ERROR;
    واپس خالی_اشاریہ;
}

U_CAPI خالی U_EXPORT2
uprv_dl_close(خالی *lib, UError_رمز *حالت_2) {
    (خالی)lib;
    اگر(U_FAILURE(*حالت_2)) واپس;
    *حالت_2 = U_UNSUPPORTED_ERROR;
    واپس;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(خالی *lib, ثابت حرف* sym, UError_رمز *حالت_2) {
  (خالی)lib;
  (خالی)sym;
  اگر(U_SUCCESS(*حالت_2)) {
    *حالت_2 = U_UNSUPPORTED_ERROR;
  }
  واپس (UVoidFunction*)خالی_اشاریہ;
}

#اگر_ختم

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

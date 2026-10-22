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
#ضمن "uposixdefs.h"

// First, the platform type. Need this for U_PLATFORM.
#ضمن "unicode/platform.h"

/*
 * Cygwin with GCC requires inclusion of time.h after the above disabling strict asci mode statement.
 */
#ضمن <time.h>

#إذا !U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
#ضمن <sys/time.h>
#پایان_اگر

/* include the rest of the ICU headers */
#ضمن "unicode/putil.h"
#ضمن "unicode/ustring.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"
#ضمن "umutex.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "locmap.h"
#ضمن "ucln_cmn.h"
#ضمن "charstr.h"

/* Include standard headers. */
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <math.h>
#ضمن <locale.h>
#ضمن <float.h>

#اگر_تعریف_نشده U_COMMON_IMPLEMENTATION
#خطا U_COMMON_IMPLEMENTATION not مجموعه - must be مجموعه لكل all ICU منبع files in common/ - see https://unicode-org.github.io/icu/userguide/icu/howtouseicu.html
#پایان_اگر


/* include system headers */
#إذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
    /*
     * TODO: U_PLATFORM_USES_ONLY_WIN32_API includes MinGW.
     * Should Cygwin be included as well (U_PLATFORM_HAS_WIN32_API)
     * to use native APIs as much as possible?
     */
#اگر_تعریف_نشده WIN32_LEAN_AND_میانگین
#   define WIN32_LEAN_AND_میانگین
#پایان_اگر
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>
#   include "unicode/uloc.h"
#   include "wintz.h"
#وإلاإذا U_PLATFORM == U_PF_OS400
#   include <float.h>
#   include <qusec.h>       /* error code structure */
#   include <qusrjobi.h>
#   include <qliept.h>      /* EPT_CALL macro  - this include must be after all other "QSYSINCs" */
#   include <mih/testptr.h> /* For uprv_maximumPtr */
#وإلاإذا U_PLATFORM == U_PF_OS390
#   include "unicode/ucnv.h"   /* Needed for UCNV_SWAP_LFNL_OPTION_STRING */
#وإلاإذا U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS
#   include <limits.h>
#   include <unistd.h>
#   إذا U_PLATFORM == U_PF_SOLARIS
#       ifndef _XPG4_2
#           define _XPG4_2
#       endif
#   elif U_PLATFORM == U_PF_ANDROID
#       include <sys/system_properties.h>
#       include <dlfcn.h>
#   endif
#وإلاإذا U_PLATFORM == U_PF_QNX
#   include <sys/neutrino.h>
#پایان_اگر


/*
 * Only include langinfo.h if we have a way to get the codeset. If we later
 * depend on more feature, we can test on U_HAVE_NL_LANGINFO.
 *
 */

#إذا U_HAVE_NL_LANGINFO_CODESET
#ضمن <langinfo.h>
#پایان_اگر

/**
 * Simple things (presence of functions, etc) should just go in configure.in and be added to
 * icucfg.h via autoheader.
 */
#إذا U_PLATFORM_IMPLEMENTS_POSIX
#   إذا U_PLATFORM == U_PF_OS400
#    define HAVE_DLFCN_H 0
#    define HAVE_DLOPEN 0
#   وإلا
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
#وإلا
#   define HAVE_DLFCN_H 0
#   define HAVE_DLOPEN 0
#   define HAVE_GETTIMEOFDAY 0
#پایان_اگر

U_NAMESPACE_USE

/* Define the extension for data files, again... */
#تعریف_کن داده_TYPE "dat"

/* Leave this copyright notice here! */
ساكن ثابت حرف copyright[] = U_COPYRIGHT_STRING;

/* floating point implementations ------------------------------------------- */

/* We return QNAN rather than SNAN*/
#تعریف_کن SIGN 0x80000000U

/* Make it easy to define certain types of constants */
تعريفنوع اتحاد {
    int64_t i64; /* This must be defined first in order to allow the initialization to work. This is a C89 feature. */
    مزدوج d64;
} بیت_الگو_Conversion;
ساكن ثابت بیت_الگو_Conversion gNan = {static_cast<int64_t>(INT64_C(0x7FF8000000000000))};
ساكن ثابت بیت_الگو_Conversion gInf = {static_cast<int64_t>(INT64_C(0x7FF0000000000000))};

/*---------------------------------------------------------------------------
  Platform utilities
  Our general strategy is to assume we're on a POSIX platform.  Platforms which
  are non-POSIX must declare themselves so.  The default POSIX implementation
  will sometimes work for non-POSIX platforms as well (e.g., the NaN-related
  functions).
  ---------------------------------------------------------------------------*/

#إذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API || U_PLATFORM == U_PF_OS400
#   undef U_POSIX_LOCALE
#وإلا
#   define U_POSIX_LOCALE    1
#پایان_اگر

/*
    WARNING! u_topNBytesOfDouble and u_bottomNBytesOfDouble
    can't be properly optimized by the gcc compiler sometimes (i.e. gcc 3.2).
*/
#إذا !IEEE_754
ساكن حرف*
u_بالاترین_NBytes_از_Double(مزدوج* d, صحيح n)
{
#إذا U_IS_BIG_ENDIAN
    أرجع (حرف*)d;
#وإلا
    أرجع (حرف*)(d + 1) - n;
#پایان_اگر
}

ساكن حرف*
u_bottom_NBytes_از_Double(مزدوج* d, صحيح n)
{
#إذا U_IS_BIG_ENDIAN
    أرجع (حرف*)(d + 1) - n;
#وإلا
    أرجع (حرف*)d;
#پایان_اگر
}
#پایان_اگر   /* !IEEE_754 */

#إذا IEEE_754
ساكن UBool
u_sign_بیت(مزدوج d) {
    uint8_t hiByte;
#إذا U_IS_BIG_ENDIAN
    hiByte = *(uint8_t *)&d;
#وإلا
    hiByte = *(reinterpret_cast<uint8_t*>(&d) + حجم(مزدوج) - 1);
#پایان_اگر
    أرجع (hiByte & 0x80) != 0;
}
#پایان_اگر



#إذا defined (U_DEBUG_FAKETIME)
/* Override the clock to test things without having to move the system clock.
 * Assumes POSIX gettimeofday() will function
 */
UDate fakeClock_t0 = 0; /** Time to start the clock from **/
UDate fakeClock_dt = 0; /** Offset (fake time - real time) **/
UBool fake_Clock_مجموعه = خطأ; /** True if fake clock has spun up **/

ساكن UDate getUTCtime_real() {
    بنية timeval posix_زمان;
    gettimeofday(&posix_زمان, اشاره_تهی);
    أرجع (UDate)(((int64_t)posix_زمان.tv_sec * U_MILLIS_در_هر_دومی) + (posix_زمان.tv_usec/1000));
}

ساكن UDate getUTCtime_fake() {
    ساكن UMutex fakeClockMutex;
    umtx_قفل(&fakeClockMutex);
    إذا(!fake_Clock_مجموعه) {
        UDate real = getUTCtime_real();
        ثابت حرف *fake_شروع = getenv("U_FAKETIME_START");
        إذا((fake_شروع!=اشاره_تهی) && (fake_شروع[0]!=0)) {
            از_رشته_بخوان(fake_شروع,"%lf",&fakeClock_t0);
            fakeClock_dt = fakeClock_t0 - real;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, so the ICU clock will start at a preset value\n"
                    "env variable U_FAKETIME_START=%.0f (%s) for an offset of %.0f ms from the current time %.0f\n",
                    fakeClock_t0, fake_شروع, fakeClock_dt, real);
        } وإلا {
          fakeClock_dt = 0;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, but U_FAKETIME_START was not set.\n"
                    "Set U_FAKETIME_START to the number of milliseconds since 1/1/1970 to set the ICU clock.\n");
        }
        fake_Clock_مجموعه = درست;
    }
    umtx_unlock(&fakeClockMutex);

    أرجع getUTCtime_real() + fakeClock_dt;
}
#پایان_اگر

#إذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
تعريفنوع اتحاد {
    int64_t int64;
    FILETIME file_زمان;
} File_زمان_Conversion;   /* This is like a ULARGE_INTEGER */

/* Number of 100 nanoseconds from 1/1/1601 to 1/1/1970 */
#تعریف_کن EPOCH_BIAS  INT64_C(116444736000000000)
#تعریف_کن HECTONANOSECOND_در_هر_MILLISECOND   10000

#پایان_اگر

/*---------------------------------------------------------------------------
  Universal Implementations
  These are designed to work on all platforms.  Try these, and if they
  don't work on your platform, then special case your platform with new
  implementations.
---------------------------------------------------------------------------*/

U_CAPI UDate U_EXPORT2
uprv_getUTCtime()
{
#إذا defined(U_DEBUG_FAKETIME)
    أرجع getUTCtime_fake(); /* Hook for overriding the clock */
#وإلا
    أرجع uprv_getRawUTCtime();
#پایان_اگر
}

/* Return UTC (GMT) time measured in milliseconds since 0:00 on 1/1/70.*/
U_CAPI UDate U_EXPORT2
uprv_getRawUTCtime()
{
#إذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API

    File_زمان_Conversion win_زمان;
    Get_System_زمان_As_File_زمان(&win_زمان.file_زمان);
    أرجع (UDate)((win_زمان.int64 - EPOCH_BIAS) / HECTONANOSECOND_در_هر_MILLISECOND);
#وإلا

#إذا HAVE_GETTIMEOFDAY
    بنية timeval posix_زمان;
    gettimeofday(&posix_زمان, اشاره_تهی);
    أرجع (UDate)(((int64_t)posix_زمان.tv_sec * U_MILLIS_در_هر_دومی) + (posix_زمان.tv_usec/1000));
#وإلا
    time_t epochtime;
    time(&epochtime);
    أرجع (UDate)epochtime * U_MILLIS_در_هر_دومی;
#پایان_اگر

#پایان_اگر
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
uprv_isNaN(مزدوج number)
{
#إذا IEEE_754
    بیت_الگو_Conversion converted_عدد;
    converted_عدد.d64 = number;
    /* Infinity is 0x7FF0000000000000U. Anything greater than that is a NaN */
    أرجع (converted_عدد.i64 & U_INT64_MAX) > gInf.i64;

#وإلاإذا U_PLATFORM == U_PF_OS390
    uint32_t بالا_بیتها = *(uint32_t*)u_بالاترین_NBytes_از_Double(&number,
                        حجم(uint32_t));
    uint32_t کم_بیتها  = *(uint32_t*)u_bottom_NBytes_از_Double(&number,
                        حجم(uint32_t));

    أرجع ((بالا_بیتها & 0x7F080000L) == 0x7F080000L) &&
      (کم_بیتها == 0x00000000L);

#وإلا
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    أرجع number != number;
#پایان_اگر
}

U_CAPI UBool U_EXPORT2
uprv_isInfinite(مزدوج number)
{
#إذا IEEE_754
    بیت_الگو_Conversion converted_عدد;
    converted_عدد.d64 = number;
    /* Infinity is exactly 0x7FF0000000000000U. */
    أرجع (converted_عدد.i64 & U_INT64_MAX) == gInf.i64;
#وإلاإذا U_PLATFORM == U_PF_OS390
    uint32_t بالا_بیتها = *(uint32_t*)u_بالاترین_NBytes_از_Double(&number,
                        حجم(uint32_t));
    uint32_t کم_بیتها  = *(uint32_t*)u_bottom_NBytes_از_Double(&number,
                        حجم(uint32_t));

    أرجع ((بالا_بیتها  & ~SIGN) == 0x70FF0000L) && (کم_بیتها == 0x00000000L);

#وإلا
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    أرجع number == (2.0 * number);
#پایان_اگر
}

U_CAPI UBool U_EXPORT2
uprv_is_مثبت_Infinity(مزدوج number)
{
#إذا IEEE_754 || U_PLATFORM == U_PF_OS390
    أرجع number > 0 && uprv_isInfinite(number);
#وإلا
    أرجع uprv_isInfinite(number);
#پایان_اگر
}

U_CAPI UBool U_EXPORT2
uprv_isNegativeInfinity(مزدوج number)
{
#إذا IEEE_754 || U_PLATFORM == U_PF_OS390
    أرجع number < 0 && uprv_isInfinite(number);

#وإلا
    uint32_t بالا_بیتها = *(uint32_t*)u_بالاترین_NBytes_از_Double(&number,
                        حجم(uint32_t));
    أرجع((بالا_بیتها & SIGN) && uprv_isInfinite(number));

#پایان_اگر
}

U_CAPI مزدوج U_EXPORT2
uprv_getNaN()
{
#إذا IEEE_754 || U_PLATFORM == U_PF_OS390
    أرجع gNan.d64;
#وإلا
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    أرجع 0.0;
#پایان_اگر
}

U_CAPI مزدوج U_EXPORT2
uprv_getInfinity()
{
#إذا IEEE_754 || U_PLATFORM == U_PF_OS390
    أرجع gInf.d64;
#وإلا
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    أرجع 0.0;
#پایان_اگر
}

U_CAPI مزدوج U_EXPORT2
uprv_floor(مزدوج x)
{
    أرجع floor(x);
}

U_CAPI مزدوج U_EXPORT2
uprv_ceil(مزدوج x)
{
    أرجع ceil(x);
}

U_CAPI مزدوج U_EXPORT2
uprv_دور(مزدوج x)
{
    أرجع uprv_floor(x + 0.5);
}

U_CAPI مزدوج U_EXPORT2
uprv_fabs(مزدوج x)
{
    أرجع fabs(x);
}

U_CAPI مزدوج U_EXPORT2
uprv_modf(مزدوج x, مزدوج* y)
{
    أرجع modf(x, y);
}

U_CAPI مزدوج U_EXPORT2
uprv_fmod(مزدوج x, مزدوج y)
{
    أرجع fmod(x, y);
}

U_CAPI مزدوج U_EXPORT2
uprv_pow(مزدوج x, مزدوج y)
{
    /* This is declared as "double pow(double x, double y)" */
    أرجع توان(x, y);
}

U_CAPI مزدوج U_EXPORT2
uprv_pow10(int32_t x)
{
    أرجع توان(10.0, (مزدوج)x);
}

U_CAPI مزدوج U_EXPORT2
uprv_fmax(مزدوج x, مزدوج y)
{
#إذا IEEE_754
    /* first handle NaN*/
    إذا(uprv_isNaN(x) || uprv_isNaN(y))
        أرجع uprv_getNaN();

    /* check for -0 and 0*/
    إذا(x == 0.0 && y == 0.0 && u_sign_بیت(x))
        أرجع y;

#پایان_اگر

    /* this should work for all flt point w/o NaN and Inf special cases */
    أرجع (x > y ? x : y);
}

U_CAPI مزدوج U_EXPORT2
uprv_fmin(مزدوج x, مزدوج y)
{
#إذا IEEE_754
    /* first handle NaN*/
    إذا(uprv_isNaN(x) || uprv_isNaN(y))
        أرجع uprv_getNaN();

    /* check for -0 and 0*/
    إذا(x == 0.0 && y == 0.0 && u_sign_بیت(y))
        أرجع y;

#پایان_اگر

    /* this should work for all flt point w/o NaN and Inf special cases */
    أرجع (x > y ? y : x);
}

U_CAPI UBool U_EXPORT2
uprv_add32_سرریز(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_add_overflow.
    // This function could be optimized by calling one of those primitives.
    خودکار a64 = static_cast<int64_t>(a);
    خودکار b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 + b64;
    *res = static_cast<int32_t>(res64);
    أرجع res64 != *res;
}

U_CAPI UBool U_EXPORT2
uprv_mul32_سرریز(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_mul_overflow.
    // This function could be optimized by calling one of those primitives.
    خودکار a64 = static_cast<int64_t>(a);
    خودکار b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 * b64;
    *res = static_cast<int32_t>(res64);
    أرجع res64 != *res;
}

/**
 * Truncates the given double.
 * trunc(3.3) = 3.0, trunc (-3.3) = -3.0
 * This is different than calling floor() or ceil():
 * floor(3.3) = 3, floor(-3.3) = -4
 * ceil(3.3) = 4, ceil(-3.3) = -3
 */
U_CAPI مزدوج U_EXPORT2
uprv_trunc(مزدوج d)
{
#إذا IEEE_754
    /* handle error cases*/
    إذا(uprv_isNaN(d))
        أرجع uprv_getNaN();
    إذا(uprv_isInfinite(d))
        أرجع uprv_getInfinity();

    إذا(u_sign_بیت(d))    /* Signbit() picks up -0.0;  d<0 does not. */
        أرجع ceil(d);
    وإلا
        أرجع floor(d);

#وإلا
    أرجع d >= 0 ? floor(d) : ceil(d);

#پایان_اگر
}

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 */
U_CAPI مزدوج U_EXPORT2
uprv_maxMantissa()
{
    أرجع توان(2.0, DBL_MANT_DIG + 1.0) - 1.0;
}

U_CAPI مزدوج U_EXPORT2
uprv_log(مزدوج d)
{
    أرجع لگاریتم(d);
}

U_CAPI فراغ * U_EXPORT2
uprv_بیشینه_Ptr(فراغ * پایه)
{
#إذا U_PLATFORM == U_PF_OS400
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
    إذا ((پایه != اشاره_تهی) && (_TESTPTR(پایه, _C_TERASPACE_CHECK))) {
        /* if it is a TERASPACE pointer the max is 2GB - 4k */
        أرجع ((فراغ *)(((حرف *)پایه)-((uint32_t)(پایه))+((uint32_t)0x7fffefff)));
    }
    /* otherwise 16MB since nullptr ptr is not checkable or the ptr is not TERASPACE */
    أرجع ((فراغ *)(((حرف *)پایه)-((uint32_t)(پایه))+((uint32_t)0xffefff)));

#وإلا
    أرجع U_MAX_PTR(پایه);
#پایان_اگر
}

/*---------------------------------------------------------------------------
  Platform-specific Implementations
  Try these, and if they don't work on your platform, then special case your
  platform with new implementations.
  ---------------------------------------------------------------------------*/

/* Generic time zone layer -------------------------------------------------- */

/* Time zone utilities */
U_CAPI فراغ U_EXPORT2
uprv_tzset()
{
#إذا defined(U_TZSET)
    U_TZSET();
#وإلا
    /* no initialization*/
#پایان_اگر
}

U_CAPI int32_t U_EXPORT2
uprv_timezone()
{
#اگر_تعریف_شده U_TIMEZONE
    أرجع U_TIMEZONE;
#وإلا
    time_t t, t1, t2;
    بنية tm tmrec;
    int32_t tdiff = 0;

    time(&t);
    uprv_memcpy( &tmrec, localtime(&t), حجم(tmrec) );
#إذا U_PLATFORM != U_PF_IPHONE
    UBool dst_checked = (tmrec.tm_isdst != 0); /* daylight savings time is checked*/
#پایان_اگر
    t1 = mktime(&tmrec);                 /* local time in seconds*/
    uprv_memcpy( &tmrec, gmtime(&t), حجم(tmrec) );
    t2 = mktime(&tmrec);                 /* GMT (or UTC) in seconds*/
    tdiff = t2 - t1;

#إذا U_PLATFORM != U_PF_IPHONE
    /* imitate NT behaviour, which returns same timezone offset to GMT for
       winter and summer.
       This does not work on all platforms. For instance, on glibc on Linux
       and on Mac OS 10.5, tdiff calculated above remains the same
       regardless of whether DST is in effect or not. iOS is another
       platform where this does not work. Linux + glibc and Mac OS 10.5
       have U_TIMEZONE defined so that this code is not reached.
    */
    إذا (dst_checked)
        tdiff += 3600;
#پایان_اگر
    أرجع tdiff;
#پایان_اگر
}

/* Note that U_TZNAME does *not* have to be tzname, but if it is,
   some platforms need to have it declared here. */

#إذا defined(U_TZNAME) && (U_PLATFORM == U_PF_IRIX || U_PLATFORM_IS_DARWIN_BASED)
/* RS6000 and others reject char **tzname.  */
خارجي U_IMPORT حرف *U_TZNAME[];
#پایان_اگر

#إذا !UCONFIG_NO_FILE_IO && ((U_PLATFORM_IS_DARWIN_BASED && (U_PLATFORM != U_PF_IPHONE || defined(U_TIMEZONE))) || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS)
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

#تعریف_کن CHECK_LOCALTIME_LINK 1
#إذا U_PLATFORM_IS_DARWIN_BASED
#ضمن <tzfile.h>
#تعریف_کن TZZONEINFO      (TZDIR "/")
#وإلاإذا U_PLATFORM == U_PF_SOLARIS
#تعریف_کن TZDEFAULT       "/etc/localtime"
#تعریف_کن TZZONEINFO      "/usr/share/lib/zoneinfo/"
#تعریف_کن TZ_ENV_CHECK    "localtime"
#وإلا
#تعریف_کن TZDEFAULT       "/etc/localtime"
#تعریف_کن TZZONEINFO      "/usr/share/zoneinfo/"
#پایان_اگر
#تعریف_کن TZZONEINFOTAIL  "/zoneinfo/"
#إذا U_HAVE_DIRENT_H
#تعریف_کن TZFILE_SKIP     "posixrules" /* tz file to skip when searching. */
/* Some Linux distributions have 'localtime' in /usr/share/zoneinfo
   symlinked to /etc/localtime, which makes searchForTZFile return
   'localtime' when it's the first match. */
#تعریف_کن TZFILE_SKIP2    "localtime"
#تعریف_کن جستجو_TZFILE
#ضمن <dirent.h>  /* Needed to search through system timezone files */
#پایان_اگر
ساكن حرف g_زمان_Zone_میانگیر[مسیر_MAX];
ساكن ثابت حرف *g_زمان_Zone_میانگیر_Ptr = اشاره_تهی;
#پایان_اگر

#إذا !U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
#تعریف_کن isNonDigit(ch) (ch < '0' || '9' < ch)
#تعریف_کن isDigit(ch) ('0' <= ch && ch <= '9')
ساكن UBool isValidOlsonID(ثابت حرف *id) {
    int32_t idx = 0;
    int32_t idxMax = 0;

    /* Determine if this is something like Iceland (Olson ID)
    or AST4ADT (non-Olson ID) */
    طالما (id[idx] && isNonDigit(id[idx]) && id[idx] != ',') {
        idx++;
    }

    /* Allow at maximum 2 numbers at the end of the id to support zone id's
    like GMT+11. */
    idxMax = idx + 2;
    طالما (id[idx] && isDigit(id[idx]) && idx < idxMax) {
        idx++;
    }

    /* If we went through the whole string, then it might be okay.
    The timezone is sometimes set to "CST-7CDT", "CST6CDT5,J129,J131/19:30",
    "GRNLNDST3GRNLNDDT" or similar, so we cannot use it.
    The rest of the time it could be an Olson ID. George */
    أرجع id[idx] == 0
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
ساكن فراغ skipZoneIDPrefix(ثابت حرف** id) {
    إذا (uprv_strncmp(*id, "posix/", 6) == 0
        || uprv_strncmp(*id, "right/", 6) == 0)
    {
        *id += 6;
    }
}
#پایان_اگر

#إذا defined(U_TZNAME) && !U_PLATFORM_استفاده_میکند_ONLY_WIN32_API

#تعریف_کن CONVERT_HOURS_مقصد_SECONDS(جابه_جایی) (int32_t)(جابه_جایی*3600)
تعريفنوع بنية جابه_جایی_Zone_Mapping {
    int32_t جابه_جایی_Seconds;
    int32_t daylightType; /* 0=U_DAYLIGHT_NONE, 1=daylight in June-U_DAYLIGHT_JUNE, 2=daylight in December=U_DAYLIGHT_DECEMBER*/
    ثابت حرف *stdID;
    ثابت حرف *dstID;
    ثابت حرف *olsonID;
} جابه_جایی_Zone_Mapping;

تعداد { U_DAYLIGHT_NONE=0,U_DAYLIGHT_JUNE=1,U_DAYLIGHT_DECEMBER=2 };

/*
This list tries to disambiguate a set of abbreviated timezone IDs and offsets
and maps it to an Olson ID.
Before adding anything to this list, take a look at
icu/source/tools/tzcode/tz.alias
Sometimes no daylight savings (0) is important to define due to aliases.
This list can be tested with icu/source/test/compat/tzone.pl
More values could be added to daylightType to increase precision.
*/
ساكن ثابت بنية جابه_جایی_Zone_Mapping جابه_جایی_ZONE_MAPPINGS[] = {
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

ساكن ثابت حرف* remap_Short_زمان_Zone(ثابت حرف *stdID, ثابت حرف *dstID, int32_t daylightType, int32_t جابه_جایی)
{
    int32_t idx;
#اگر_تعریف_شده DEBUG_TZNAME
    fprintf(stderr, "TZ=%s std=%s dst=%s daylight=%d offset=%d\n", getenv("TZ"), stdID, dstID, daylightType, جابه_جایی);
#پایان_اگر
    لكل (idx = 0; idx < UPRV_LENGTHOF(جابه_جایی_ZONE_MAPPINGS); idx++)
    {
        إذا (جابه_جایی == جابه_جایی_ZONE_MAPPINGS[idx].جابه_جایی_Seconds
            && daylightType == جابه_جایی_ZONE_MAPPINGS[idx].daylightType
            && strcmp(جابه_جایی_ZONE_MAPPINGS[idx].stdID, stdID) == 0
            && strcmp(جابه_جایی_ZONE_MAPPINGS[idx].dstID, dstID) == 0)
        {
            أرجع جابه_جایی_ZONE_MAPPINGS[idx].olsonID;
        }
    }
    أرجع اشاره_تهی;
}
#پایان_اگر

#اگر_تعریف_شده جستجو_TZFILE
#تعریف_کن MAX_READ_اندازه 512

تعريفنوع بنية DefaultTZInfo {
    حرف* defaultTZBuffer;
    int64_t default_TZFile_اندازه;
    FILE* defaultTZFilePtr;
    int32_t defaultTZPosition;
} DefaultTZInfo;

/*
 * This method compares the two files given to see if they are a match.
 * It is currently use to compare two TZ files.
 */
ساكن UBool compare_دودویی_Files(ثابت حرف* TZFileName, DefaultTZInfo* tzInfo) {
    FILE* file;
    int64_t اندازه_File;
    int64_t اندازه_File_چپ;
    int32_t اندازه_File_Read;
    int32_t اندازه_File_مقصد_Read;
    حرف میانگیر_File[MAX_READ_اندازه];
    UBool نتیجه = درست;

    U_ASSERT(tzInfo->defaultTZFilePtr != اشاره_تهی);
    file = fopen(TZFileName, "r");

    tzInfo->defaultTZPosition = 0; /* reset position to begin search */

    إذا (file != اشاره_تهی) {
        /* First check that the file size are equal. */
        إذا (tzInfo->default_TZFile_اندازه == 0) {
            fseek(tzInfo->defaultTZFilePtr, 0, SEEK_END);
            tzInfo->default_TZFile_اندازه = ftell(tzInfo->defaultTZFilePtr);
        }
        إذا (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            أرجع خطأ;
        }
        اندازه_File = ftell(file);
        إذا (اندازه_File == -1) {
            fclose(file);
            أرجع خطأ;
        }
        اندازه_File_چپ = اندازه_File;

        إذا (اندازه_File != tzInfo->default_TZFile_اندازه) {
            نتیجه = خطأ;
        } وإلا {
            /* Store the data from the files in separate buffers and
             * compare each byte to determine equality.
             */
            إذا (tzInfo->defaultTZBuffer == اشاره_تهی) {
                إذا (fseek(tzInfo->defaultTZFilePtr, 0, SEEK_SET) != 0) {
                    fclose(file);
                    أرجع خطأ;
                }
                tzInfo->defaultTZBuffer = static_cast<حرف*>(uprv_malloc(حجم(حرف) * tzInfo->default_TZFile_اندازه));
                اندازه_File_Read = fread(tzInfo->defaultTZBuffer, 1, tzInfo->default_TZFile_اندازه, tzInfo->defaultTZFilePtr);
            }
            إذا (fseek(file, 0, SEEK_SET) != 0) {
                fclose(file);
                أرجع خطأ;
            }
            طالما(اندازه_File_چپ > 0) {
                uprv_memset(میانگیر_File, 0, MAX_READ_اندازه);
                اندازه_File_مقصد_Read = اندازه_File_چپ < MAX_READ_اندازه ? اندازه_File_چپ : MAX_READ_اندازه;

                اندازه_File_Read = fread(میانگیر_File, 1, اندازه_File_مقصد_Read, file);
                إذا (اندازه_File_Read == -1) {
                    fclose(file);
                    أرجع خطأ;
                }
                إذا (memcmp(tzInfo->defaultTZBuffer + tzInfo->defaultTZPosition, میانگیر_File, اندازه_File_Read) != 0) {
                    نتیجه = خطأ;
                    اكسر;
                }
                اندازه_File_چپ -= اندازه_File_Read;
                tzInfo->defaultTZPosition += اندازه_File_Read;
            }
        }
    } وإلا {
        نتیجه = خطأ;
    }

    إذا (file != اشاره_تهی) {
        fclose(file);
    }

    أرجع نتیجه;
}


/* dirent also lists two entries: "." and ".." that we can safely ignore. */
#تعریف_کن SKIP1 "."
#تعریف_کن SKIP2 ".."
ساكن UBool U_CALLCONV putil_cleanup();
ساكن CharString *g_جستجو_TZFile_نتیجه = اشاره_تهی;

/*
 * This method recursively traverses the directory given for a matching TZ file and returns the first match.
 * This function is not thread safe - it uses a global, gSearchTZFileResult, to hold its results.
 */
ساكن حرف* جستجو_For_TZFile(ثابت حرف* مسیر, DefaultTZInfo* tzInfo) {
    DIR* dirp = اشاره_تهی;
    بنية dirent* dir_مدخل = اشاره_تهی;
    حرف* نتیجه = اشاره_تهی;
    UError_کد وضعیت = U_ZERO_ERROR;

    /* There is no reference file to compare against, so do not traverse the zoneinfo tree. */
    إذا (tzInfo->defaultTZFilePtr == اشاره_تهی) {
        أرجع اشاره_تهی;
    }

    /* Save the current path */
    CharString curpath(مسیر, -1, وضعیت);
    إذا (U_FAILURE(وضعیت)) {
        اذهب cleanupAndReturn;
    }

    dirp = opendir(مسیر);
    إذا (dirp == اشاره_تهی) {
        اذهب cleanupAndReturn;
    }

    إذا (g_جستجو_TZFile_نتیجه == اشاره_تهی) {
        g_جستجو_TZFile_نتیجه = جديد CharString;
        إذا (g_جستجو_TZFile_نتیجه == اشاره_تهی) {
            اذهب cleanupAndReturn;
        }
        ucln_common_ثبات_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    }

    /* Check each entry in the directory. */
    طالما((dir_مدخل = readdir(dirp)) != اشاره_تهی) {
        ثابت حرف* dirName = dir_مدخل->d_name;
        إذا (uprv_strcmp(dirName, SKIP1) != 0 && uprv_strcmp(dirName, SKIP2) != 0
            && uprv_strcmp(TZFILE_SKIP, dirName) != 0 && uprv_strcmp(TZFILE_SKIP2, dirName) != 0) {
            /* Create a newpath with the new entry to test each entry in the directory. */
            CharString newpath(curpath, وضعیت);
            newpath.append(dirName, -1, وضعیت);
            إذا (U_FAILURE(وضعیت)) {
                اكسر;
            }

            DIR* subDirp = اشاره_تهی;
            إذا ((subDirp = opendir(newpath.داده())) != اشاره_تهی) {
                /* If this new path is a directory, make a recursive call with the newpath. */
                closedir(subDirp);
                newpath.append('/', وضعیت);
                إذا (U_FAILURE(وضعیت)) {
                    اكسر;
                }
                نتیجه = جستجو_For_TZFile(newpath.داده(), tzInfo);
                /*
                 Have to get out here. Otherwise, we'd keep looking
                 and return the first match in the top-level directory
                 if there's a match in the top-level. If not, this function
                 would return nullptr and set gTimeZoneBufferPtr to nullptr in initDefault().
                 It worked without this in most cases because we have a fallback of calling
                 localtime_r to figure out the default timezone.
                */
                إذا (نتیجه != اشاره_تهی)
                    اكسر;
            } وإلا {
                إذا(compare_دودویی_Files(newpath.داده(), tzInfo)) {
                    int32_t amount_مقصد_Skip = حجم(TZZONEINFO) - 1;
                    إذا (amount_مقصد_Skip > newpath.length()) {
                        amount_مقصد_Skip = newpath.length();
                    }
                    ثابت حرف* zoneid = newpath.داده() + amount_مقصد_Skip;
                    skipZoneIDPrefix(&zoneid);
                    g_جستجو_TZFile_نتیجه->clear();
                    g_جستجو_TZFile_نتیجه->append(zoneid, -1, وضعیت);
                    إذا (U_FAILURE(وضعیت)) {
                        اكسر;
                    }
                    نتیجه = g_جستجو_TZFile_نتیجه->داده();
                    /* Get out after the first one found. */
                    اكسر;
                }
            }
        }
    }

  cleanupAndReturn:
    إذا (dirp) {
        closedir(dirp);
    }
    أرجع نتیجه;
}
#پایان_اگر

#إذا U_PLATFORM == U_PF_ANDROID
تعريفنوع صحيح(system_property_read_callback)(ثابت prop_info* info,
                                           فراغ (*callback)(فراغ* cookie,
                                                            ثابت حرف* name,
                                                            ثابت حرف* value,
                                                            uint32_t serial),
                                           فراغ* cookie);
تعريفنوع صحيح(system_property_get)(ثابت حرف*, حرف*);

ساكن حرف g_Android_زمان_Zone[PROP_مقدار_MAX] = { '\0' };

ساكن فراغ u_property_read(فراغ* cookie, ثابت حرف* name, ثابت حرف* value,
                            uint32_t serial) {
    uprv_strcpy((حرف* )cookie, value);
}
#پایان_اگر

U_CAPI فراغ U_EXPORT2
uprv_tzname_clear_cache()
{
#إذا U_PLATFORM == U_PF_ANDROID
    /* Android's timezone is stored in system property. */
    g_Android_زمان_Zone[0] = '\0';
    فراغ* libc = dlopen("libc.so", RTLD_NOLOAD);
    إذا (libc) {
        /* Android API 26+ has new API to get system property and old API
         * (__system_property_get) is deprecated */
        system_property_read_callback* property_read_callback =
            (system_property_read_callback*)dlsym(
                libc, "__system_property_read_callback");
        إذا (property_read_callback) {
            ثابت prop_info* info =
                __system_property_find("persist.sys.timezone");
            إذا (info) {
                property_read_callback(info, &u_property_read, g_Android_زمان_Zone);
            }
        } وإلا {
            system_property_get* property_get =
                (system_property_get*)dlsym(libc, "__system_property_get");
            إذا (property_get) {
                property_get("persist.sys.timezone", g_Android_زمان_Zone);
            }
        }
        dlclose(libc);
    }
#پایان_اگر

#إذا defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    g_زمان_Zone_میانگیر_Ptr = اشاره_تهی;
#پایان_اگر
}

U_CAPI ثابت حرف* U_EXPORT2
uprv_tzname(صحيح n)
{
    (فراغ)n; // Avoid unreferenced parameter warning.
    ثابت حرف *tzid = اشاره_تهی;
#إذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
    tzid = uprv_detect_Windows_زمان_Zone();

    إذا (tzid != اشاره_تهی) {
        أرجع tzid;
    }

#اگر_تعریف_نشده U_TZNAME
    // The return value is free'd in timezone.cpp on Windows because
    // the other code path returns a pointer to a heap location.
    // If we don't have a name already, then tzname wouldn't be any
    // better, so just fall back.
    أرجع uprv_strdup("");
#پایان_اگر // !U_TZNAME

#وإلا

/*#if U_PLATFORM_IS_DARWIN_BASED
    int ret;

    tzid = getenv("TZFILE");
    if (tzid != nullptr) {
        return tzid;
    }
#endif*/

/* This code can be temporarily disabled to test tzname resolution later on. */
#اگر_تعریف_نشده DEBUG_TZNAME
#إذا U_PLATFORM == U_PF_ANDROID
    tzid = g_Android_زمان_Zone;
#وإلا
    tzid = getenv("TZ");
#پایان_اگر
    إذا (tzid != اشاره_تهی && isValidOlsonID(tzid)
#إذا U_PLATFORM == U_PF_SOLARIS
    /* Don't misinterpret TZ "localtime" on Solaris as a time zone name. */
        && uprv_strcmp(tzid, TZ_ENV_CHECK) != 0
#پایان_اگر
    ) {
        /* The colon forces tzset() to treat the remainder as zoneinfo path */
        إذا (tzid[0] == ':') {
            tzid++;
        }
        /* This might be a good Olson ID. */
        skipZoneIDPrefix(&tzid);
        أرجع tzid;
    }
    /* else U_TZNAME will give a better result. */
#پایان_اگر

#إذا defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    /* Caller must handle threading issues */
    إذا (g_زمان_Zone_میانگیر_Ptr == اشاره_تهی) {
        /*
        This is a trick to look at the name of the link to get the Olson ID
        because the tzfile contents is underspecified.
        This isn't guaranteed to work because it may not be a symlink.
        */
        حرف *ret = realpath(TZDEFAULT, g_زمان_Zone_میانگیر);
        إذا (ret != اشاره_تهی && uprv_strcmp(TZDEFAULT, g_زمان_Zone_میانگیر) != 0) {
            int32_t tz_Zone_Info_دم_Len = uprv_strlen(TZZONEINFOTAIL);
            ثابت حرف *tz_Zone_Info_دم_Ptr = uprv_strstr(g_زمان_Zone_میانگیر, TZZONEINFOTAIL);
            // MacOS14 has the realpath as something like
            // /usr/share/zoneinfo.default/Australia/Melbourne
            // which will not have "/zoneinfo/" in the path.
            // Therefore if we fail, we fall back to read the link which is
            // /var/db/timezone/zoneinfo/Australia/Melbourne
            // We also fall back to reading the link if the realpath leads to something like
            // /usr/share/zoneinfo/posixrules
            إذا (tz_Zone_Info_دم_Ptr == اشاره_تهی ||
                    uprv_strcmp(tz_Zone_Info_دم_Ptr + tz_Zone_Info_دم_Len, "posixrules") == 0) {
                ssize_t size = readlink(TZDEFAULT, g_زمان_Zone_میانگیر, حجم(g_زمان_Zone_میانگیر)-1);
                إذا (size > 0) {
                    g_زمان_Zone_میانگیر[size] = 0;
                    tz_Zone_Info_دم_Ptr = uprv_strstr(g_زمان_Zone_میانگیر, TZZONEINFOTAIL);
                }
            }
            إذا (tz_Zone_Info_دم_Ptr != اشاره_تهی) {
                tz_Zone_Info_دم_Ptr += tz_Zone_Info_دم_Len;
                skipZoneIDPrefix(&tz_Zone_Info_دم_Ptr);
                إذا (isValidOlsonID(tz_Zone_Info_دم_Ptr)) {
                    أرجع (g_زمان_Zone_میانگیر_Ptr = tz_Zone_Info_دم_Ptr);
                }
            }
        } وإلا {
#إذا defined(جستجو_TZFILE)
            DefaultTZInfo* tzInfo = (DefaultTZInfo*)uprv_malloc(حجم(DefaultTZInfo));
            إذا (tzInfo != اشاره_تهی) {
                tzInfo->defaultTZBuffer = اشاره_تهی;
                tzInfo->default_TZFile_اندازه = 0;
                tzInfo->defaultTZFilePtr = fopen(TZDEFAULT, "r");
                tzInfo->defaultTZPosition = 0;

                g_زمان_Zone_میانگیر_Ptr = جستجو_For_TZFile(TZZONEINFO, tzInfo);

                /* Free previously allocated memory */
                إذا (tzInfo->defaultTZBuffer != اشاره_تهی) {
                    uprv_free(tzInfo->defaultTZBuffer);
                }
                إذا (tzInfo->defaultTZFilePtr != اشاره_تهی) {
                    fclose(tzInfo->defaultTZFilePtr);
                }
                uprv_free(tzInfo);
            }

            إذا (g_زمان_Zone_میانگیر_Ptr != اشاره_تهی && isValidOlsonID(g_زمان_Zone_میانگیر_Ptr)) {
                أرجع g_زمان_Zone_میانگیر_Ptr;
            }
#پایان_اگر
        }
    }
    وإلا {
        أرجع g_زمان_Zone_میانگیر_Ptr;
    }
#پایان_اگر
#پایان_اگر

#اگر_تعریف_شده U_TZNAME
#إذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
    /* The return value is free'd in timezone.cpp on Windows because
     * the other code path returns a pointer to a heap location. */
    أرجع uprv_strdup(U_TZNAME[n]);
#وإلا
    /*
    U_TZNAME is usually a non-unique abbreviation, which isn't normally usable.
    So we remap the abbreviation to an olson ID.

    Since Windows exposes a little more timezone information,
    we normally don't use this code on Windows because
    uprv_detectWindowsTimeZone should have already given the correct answer.
    */
    {
        بنية tm juneSol, decemberSol;
        صحيح daylightType;
        ساكن ثابت time_t juneSolstice=1182478260; /*2007-06-21 18:11 UT*/
        ساكن ثابت time_t decemberSolstice=1198332540; /*2007-12-22 06:09 UT*/

        /* This probing will tell us when daylight savings occurs.  */
        localtime_r(&juneSolstice, &juneSol);
        localtime_r(&decemberSolstice, &decemberSol);
        إذا(decemberSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_DECEMBER;
        } وإلا إذا(juneSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_JUNE;
        } وإلا {
          daylightType = U_DAYLIGHT_NONE;
        }
        tzid = remap_Short_زمان_Zone(U_TZNAME[0], U_TZNAME[1], daylightType, uprv_timezone());
        إذا (tzid != اشاره_تهی) {
            أرجع tzid;
        }
    }
    أرجع U_TZNAME[n];
#پایان_اگر
#وإلا
    أرجع "";
#پایان_اگر
}

/* Get and set the ICU data directory --------------------------------------- */

ساكن icu::UInitOnce g_داده_Dir_Init_Once {};
ساكن حرف *g_داده_Directory = اشاره_تهی;

UInitOnce g_زمان_Zone_Files_Init_Once {};
ساكن CharString *g_زمان_Zone_Files_Directory = اشاره_تهی;

#إذا U_POSIX_LOCALE || U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
 ساكن ثابت حرف *gCorrectedPOSIXLocale = اشاره_تهی; /* Sometimes heap allocated */
 ساكن منطقي g_Corrected_POSIXLocale_انباشته_Allocated = خطأ;
#پایان_اگر

ساكن UBool U_CALLCONV putil_cleanup()
{
    إذا (g_داده_Directory && *g_داده_Directory) {
        uprv_free(g_داده_Directory);
    }
    g_داده_Directory = اشاره_تهی;
    g_داده_Dir_Init_Once.reset();

    حذف g_زمان_Zone_Files_Directory;
    g_زمان_Zone_Files_Directory = اشاره_تهی;
    g_زمان_Zone_Files_Init_Once.reset();

#اگر_تعریف_شده جستجو_TZFILE
    حذف g_جستجو_TZFile_نتیجه;
    g_جستجو_TZFile_نتیجه = اشاره_تهی;
#پایان_اگر

#إذا U_POSIX_LOCALE || U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
    إذا (gCorrectedPOSIXLocale && g_Corrected_POSIXLocale_انباشته_Allocated) {
        uprv_free(const_cast<حرف *>(gCorrectedPOSIXLocale));
        gCorrectedPOSIXLocale = اشاره_تهی;
        g_Corrected_POSIXLocale_انباشته_Allocated = خطأ;
    }
#پایان_اگر
    أرجع درست;
}

/*
 * Set the data directory.
 *    Make a copy of the passed string, and set the global data dir to point to it.
 */
U_CAPI فراغ U_EXPORT2
u_مجموعه_داده_Directory(ثابت حرف *directory) {
    حرف *new_داده_Dir;
    int32_t length;

    إذا(directory==اشاره_تهی || *directory==0) {
        /* A small optimization to prevent the malloc and copy when the
        shared library is used, and this is a way to make sure that nullptr
        is never returned.
        */
        new_داده_Dir = (حرف *)"";
    }
    وإلا {
        length=(int32_t)uprv_strlen(directory);
        new_داده_Dir = (حرف *)uprv_malloc(length + 2);
        /* Exit out if newDataDir could not be created. */
        إذا (new_داده_Dir == اشاره_تهی) {
            أرجع;
        }
        uprv_strcpy(new_داده_Dir, directory);

#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
        {
            حرف *p;
            طالما((p = uprv_strchr(new_داده_Dir, U_FILE_ALT_SEP_CHAR)) != اشاره_تهی) {
                *p = U_FILE_SEP_CHAR;
            }
        }
#پایان_اگر
    }

    إذا (g_داده_Directory && *g_داده_Directory) {
        uprv_free(g_داده_Directory);
    }
    g_داده_Directory = new_داده_Dir;
    ucln_common_ثبات_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
}

U_CAPI UBool U_EXPORT2
uprv_مسیر_Is_Absolute(ثابت حرف *مسیر)
{
  إذا(!مسیر || !*مسیر) {
    أرجع خطأ;
  }

  إذا(*مسیر == U_FILE_SEP_CHAR) {
    أرجع درست;
  }

#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
  إذا(*مسیر == U_FILE_ALT_SEP_CHAR) {
    أرجع درست;
  }
#پایان_اگر

#إذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
  إذا( (((مسیر[0] >= 'A') && (مسیر[0] <= 'Z')) ||
       ((مسیر[0] >= 'a') && (مسیر[0] <= 'z'))) &&
      مسیر[1] == ':' ) {
    أرجع درست;
  }
#پایان_اگر

  أرجع خطأ;
}

/* Backup setting of ICU_DATA_DIR_PREFIX_ENV_VAR
   (needed for some Darwin ICU build environments) */
#إذا U_PLATFORM_IS_DARWIN_BASED && defined(هدف_OS_SIMULATOR) && هدف_OS_SIMULATOR
# إذا !defined(ICU_داده_DIR_پیشوند_ENV_VAR)
#  define ICU_داده_DIR_پیشوند_ENV_VAR "IPHONE_SIMULATOR_ROOT"
# endif
#پایان_اگر

#إذا defined(ICU_داده_DIR_WINDOWS)
// Helper function to get the ICU Data Directory under the Windows directory location.
ساكن BOOL U_CALLCONV get_Icu_داده_Directory_Under_Windows_Directory(حرف* directory_میانگیر, UINT میانگیر_درازی)
{
    wchar_t windows_مسیر[MAX_مسیر];
    حرف windows_مسیر_Utf8[MAX_مسیر];

    UINT length = GetSystemWindowsDirectoryW(windows_مسیر, UPRV_LENGTHOF(windows_مسیر));
    إذا ((length > 0) && (length < (UPRV_LENGTHOF(windows_مسیر) - 1))) {
        // Convert UTF-16 to a UTF-8 string.
        UError_کد وضعیت = U_ZERO_ERROR;
        int32_t windows_مسیر_Utf8_Len = 0;
        u_str_مقصد_UTF8(windows_مسیر_Utf8, static_cast<int32_t>(UPRV_LENGTHOF(windows_مسیر_Utf8)),
            &windows_مسیر_Utf8_Len, reinterpret_cast<ثابت char16_t*>(windows_مسیر), -1, &وضعیت);

        إذا (U_SUCCESS(وضعیت) && (وضعیت != U_STRING_NOT_TERMINATED_WARNING) &&
            (windows_مسیر_Utf8_Len < (UPRV_LENGTHOF(windows_مسیر_Utf8) - 1))) {
            // Ensure it always has a separator, so we can append the ICU data path.
            إذا (windows_مسیر_Utf8[windows_مسیر_Utf8_Len - 1] != U_FILE_SEP_CHAR) {
                windows_مسیر_Utf8[windows_مسیر_Utf8_Len++] = U_FILE_SEP_CHAR;
                windows_مسیر_Utf8[windows_مسیر_Utf8_Len] = '\0';
            }
            // Check if the concatenated string will fit.
            إذا ((windows_مسیر_Utf8_Len + UPRV_LENGTHOF(ICU_داده_DIR_WINDOWS)) < میانگیر_درازی) {
                uprv_strcpy(directory_میانگیر, windows_مسیر_Utf8);
                uprv_strcat(directory_میانگیر, ICU_داده_DIR_WINDOWS);
                أرجع درست;
            }
        }
    }

    أرجع خطأ;
}
#پایان_اگر

ساكن فراغ U_CALLCONV داده_Directory_Init_Fn() {
    /* If we already have the directory, then return immediately. Will happen if user called
     * u_setDataDirectory().
     */
    إذا (g_داده_Directory) {
        أرجع;
    }

    ثابت حرف *مسیر = اشاره_تهی;

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
#   إذا !defined(ICU_NO_USER_داده_OVERRIDE) && !UCONFIG_NO_FILE_IO
    /* First try to get the environment variable */
#     إذا U_PLATFORM_HAS_WINUWP_API == 0  // Windows UWP does not support getenv
        مسیر=getenv("ICU_DATA");
#     endif
#   endif

    /* ICU_DATA_DIR may be set as a compile option.
     * U_ICU_DATA_DEFAULT_DIR is provided and is set by ICU at compile time
     * and is used only when data is built in archive mode eliminating the need
     * for ICU_DATA_DIR to be set. U_ICU_DATA_DEFAULT_DIR is set to the installation
     * directory of the data dat file. Users should use ICU_DATA_DIR if they want to
     * set their own path.
     */
#إذا defined(ICU_داده_DIR) || defined(U_ICU_داده_DEFAULT_DIR)
# إذا defined(ICU_داده_DIR_پیشوند_ENV_VAR)
    حرف datadir_مسیر_میانگیر[مسیر_MAX];
# endif
    إذا(مسیر==اشاره_تهی || *مسیر==0) {
# إذا defined(ICU_داده_DIR_پیشوند_ENV_VAR)
        ثابت حرف *پیشوند = getenv(ICU_داده_DIR_پیشوند_ENV_VAR);
# endif
# ifdef ICU_داده_DIR
        مسیر=ICU_داده_DIR;
# وإلا
        مسیر=U_ICU_داده_DEFAULT_DIR;
# endif
# إذا defined(ICU_داده_DIR_پیشوند_ENV_VAR)
        إذا (پیشوند != اشاره_تهی) {
            snprintf(datadir_مسیر_میانگیر, حجم(datadir_مسیر_میانگیر), "%s%s", پیشوند, مسیر);
            مسیر=datadir_مسیر_میانگیر;
        }
# endif
    }
#پایان_اگر

#إذا defined(ICU_داده_DIR_WINDOWS)
    حرف datadir_مسیر_میانگیر[MAX_مسیر];
    إذا (get_Icu_داده_Directory_Under_Windows_Directory(datadir_مسیر_میانگیر, UPRV_LENGTHOF(datadir_مسیر_میانگیر))) {
        مسیر = datadir_مسیر_میانگیر;
    }
#پایان_اگر

    إذا(مسیر==اشاره_تهی) {
        /* It looks really bad, set it to something. */
        مسیر = "";
    }

    u_مجموعه_داده_Directory(مسیر);
}

U_CAPI ثابت حرف * U_EXPORT2
u_get_داده_Directory() {
    umtx_initOnce(g_داده_Dir_Init_Once, &داده_Directory_Init_Fn);
    أرجع g_داده_Directory;
}

ساكن فراغ مجموعه_زمان_Zone_Files_Dir(ثابت حرف *مسیر, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        أرجع;
    }
    g_زمان_Zone_Files_Directory->clear();
    g_زمان_Zone_Files_Directory->append(مسیر, وضعیت);
#إذا (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    حرف *p = g_زمان_Zone_Files_Directory->داده();
    طالما ((p = uprv_strchr(p, U_FILE_ALT_SEP_CHAR)) != اشاره_تهی) {
        *p = U_FILE_SEP_CHAR;
    }
#پایان_اگر
}

#تعریف_کن مقصد_STRING(x) مقصد_STRING_2(x)
#تعریف_کن مقصد_STRING_2(x) #x

ساكن فراغ U_CALLCONV زمان_Zone_داده_Dir_Init_Fn(UError_کد &وضعیت) {
    U_ASSERT(g_زمان_Zone_Files_Directory == اشاره_تهی);
    ucln_common_ثبات_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    g_زمان_Zone_Files_Directory = جديد CharString();
    إذا (g_زمان_Zone_Files_Directory == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    ثابت حرف *dir = "";

#إذا defined(ICU_TIMEZONE_FILES_DIR_پیشوند_ENV_VAR)
    حرف timezonefilesdir_مسیر_میانگیر[مسیر_MAX];
    ثابت حرف *پیشوند = getenv(ICU_TIMEZONE_FILES_DIR_پیشوند_ENV_VAR);
#پایان_اگر

#إذا U_PLATFORM_HAS_WINUWP_API == 1
// The UWP version does not support the environment variable setting.

# إذا defined(ICU_داده_DIR_WINDOWS)
    // When using the Windows system data, we can possibly pick up time zone data from the Windows directory.
    حرف datadir_مسیر_میانگیر[MAX_مسیر];
    إذا (get_Icu_داده_Directory_Under_Windows_Directory(datadir_مسیر_میانگیر, UPRV_LENGTHOF(datadir_مسیر_میانگیر))) {
        dir = datadir_مسیر_میانگیر;
    }
# endif

#وإلا
    dir = getenv("ICU_TIMEZONE_FILES_DIR");
#پایان_اگر // U_PLATFORM_HAS_WINUWP_API

#إذا defined(U_TIMEZONE_FILES_DIR)
    إذا (dir == اشاره_تهی) {
        // Build time configuration setting.
        dir = مقصد_STRING(U_TIMEZONE_FILES_DIR);
    }
#پایان_اگر

    إذا (dir == اشاره_تهی) {
        dir = "";
    }

#إذا defined(ICU_TIMEZONE_FILES_DIR_پیشوند_ENV_VAR)
    إذا (پیشوند != اشاره_تهی) {
        snprintf(timezonefilesdir_مسیر_میانگیر, حجم(timezonefilesdir_مسیر_میانگیر), "%s%s", پیشوند, dir);
        dir = timezonefilesdir_مسیر_میانگیر;
    }
#پایان_اگر

    مجموعه_زمان_Zone_Files_Dir(dir, وضعیت);
}


U_CAPI ثابت حرف * U_EXPORT2
u_get_زمان_Zone_Files_Directory(UError_کد *وضعیت) {
    umtx_initOnce(g_زمان_Zone_Files_Init_Once, &زمان_Zone_داده_Dir_Init_Fn, *وضعیت);
    أرجع U_SUCCESS(*وضعیت) ? g_زمان_Zone_Files_Directory->داده() : "";
}

U_CAPI فراغ U_EXPORT2
u_مجموعه_زمان_Zone_Files_Directory(ثابت حرف *مسیر, UError_کد *وضعیت) {
    umtx_initOnce(g_زمان_Zone_Files_Init_Once, &زمان_Zone_داده_Dir_Init_Fn, *وضعیت);
    مجموعه_زمان_Zone_Files_Dir(مسیر, *وضعیت);

    // Note: this function does some extra churn, first setting based on the
    //       environment, then immediately replacing with the value passed in.
    //       The logic is simpler that way, and performance shouldn't be an issue.
}


#إذا U_POSIX_LOCALE
/* A helper function used by uprv_getPOSIXIDForDefaultLocale and
 * uprv_getPOSIXIDForDefaultCodepage. Returns the posix locale id for
 * LC_CTYPE and LC_MESSAGES. It doesn't support other locale categories.
 */
ساكن ثابت حرف *uprv_getPOSIXIDForCategory(صحيح category)
{
    ثابت حرف* posixID = اشاره_تهی;
    إذا (category == LC_MESSAGES || category == LC_CTYPE) {
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
        posixID = setlocale(category, اشاره_تهی);
        إذا ((posixID == اشاره_تهی)
            || (uprv_strcmp("C", posixID) == 0)
            || (uprv_strcmp("POSIX", posixID) == 0))
        {
            /* Maybe we got some garbage.  Try something more reasonable */
            posixID = getenv("LC_ALL");
            /* Solaris speaks POSIX -  See IEEE Std 1003.1-2008
             * This is needed to properly handle empty env. variables
             */
#إذا U_PLATFORM == U_PF_SOLARIS
            إذا ((posixID == 0) || (posixID[0] == '\0')) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                إذا ((posixID == 0) || (posixID[0] == '\0')) {
#وإلا
            إذا (posixID == اشاره_تهی) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                إذا (posixID == اشاره_تهی) {
#پایان_اگر
                    posixID = getenv("LANG");
                }
            }
        }
    }
    إذا ((posixID == اشاره_تهی)
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
    أرجع posixID;
}

/* Return just the POSIX id for the default locale, whatever happens to be in
 * it. It gets the value from LC_MESSAGES and indirectly from LC_ALL and LANG.
 */
ساكن ثابت حرف *uprv_getPOSIXIDForDefaultLocale()
{
    ساكن ثابت حرف* posixID = اشاره_تهی;
    إذا (posixID == اشاره_تهی) {
        posixID = uprv_getPOSIXIDForCategory(LC_MESSAGES);
    }
    أرجع posixID;
}

#إذا !U_CHARSET_IS_UTF8
/* Return just the POSIX id for the default codepage, whatever happens to be in
 * it. It gets the value from LC_CTYPE and indirectly from LC_ALL and LANG.
 */
ساكن ثابت حرف *uprv_getPOSIXIDForDefaultCodepage()
{
    ساكن ثابت حرف* posixID = اشاره_تهی;
    إذا (posixID == 0) {
        posixID = uprv_getPOSIXIDForCategory(LC_CTYPE);
    }
    أرجع posixID;
}
#پایان_اگر
#پایان_اگر

/* NOTE: The caller should handle thread safety */
U_CAPI ثابت حرف* U_EXPORT2
uprv_getDefaultLocaleID()
{
#إذا U_POSIX_LOCALE
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

    إذا (gCorrectedPOSIXLocale != اشاره_تهی) {
        أرجع gCorrectedPOSIXLocale;
    }

    // Copy the ID into owned memory.
    // Over-allocate in case we replace "C" with "en_US_POSIX" (+10), + null termination
    حرف *correctedPOSIXLocale = static_cast<حرف *>(uprv_malloc(uprv_strlen(posixID) + 10 + 1));
    إذا (correctedPOSIXLocale == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    uprv_strcpy(correctedPOSIXLocale, posixID);

    حرف *حد;
    إذا ((حد = uprv_strchr(correctedPOSIXLocale, '.')) != اشاره_تهی) {
        *حد = 0;
    }
    إذا ((حد = uprv_strchr(correctedPOSIXLocale, '@')) != اشاره_تهی) {
        *حد = 0;
    }

    إذا ((uprv_strcmp("C", correctedPOSIXLocale) == 0) // no @ variant
        || (uprv_strcmp("POSIX", correctedPOSIXLocale) == 0)) {
      // Raw input was C.* or POSIX.*, Give it a nice POSIX default value.
      // (The "C"/"POSIX" case is handled in uprv_getPOSIXIDForCategory())
      uprv_strcpy(correctedPOSIXLocale, "en_US_POSIX");
    }

    /* Note that we scan the *uncorrected* ID. */
    ثابت حرف *p;
    إذا ((p = uprv_strrchr(posixID, '@')) != اشاره_تهی) {
        p++;

        /* Take care of any special cases here.. */
        إذا (!uprv_strcmp(p, "nynorsk")) {
            p = "NY";
            /* Don't worry about no__NY. In practice, it won't appear. */
        }

        إذا (uprv_strchr(correctedPOSIXLocale,'_') == اشاره_تهی) {
            uprv_strcat(correctedPOSIXLocale, "__"); /* aa@b -> aa__b (note this can make the new locale 1 char longer) */
        }
        وإلا {
            uprv_strcat(correctedPOSIXLocale, "_"); /* aa_CC@b -> aa_CC_b */
        }

        ثابت حرف *q;
        إذا ((q = uprv_strchr(p, '.')) != اشاره_تهی) {
            /* How big will the resulting string be? */
            int32_t len = (int32_t)(uprv_strlen(correctedPOSIXLocale) + (q-p));
            uprv_strncat(correctedPOSIXLocale, p, q-p); // do not include charset
            correctedPOSIXLocale[len] = 0;
        }
        وإلا {
            /* Anything following the @ sign */
            uprv_strcat(correctedPOSIXLocale, p);
        }

        /* Should there be a map from 'no@nynorsk' -> no_NO_NY here?
         * How about 'russian' -> 'ru'?
         * Many of the other locales using ISO codes will be handled by the
         * canonicalization functions in uloc_getDefault.
         */
    }

    إذا (gCorrectedPOSIXLocale == اشاره_تهی) {
        gCorrectedPOSIXLocale = correctedPOSIXLocale;
        g_Corrected_POSIXLocale_انباشته_Allocated = درست;
        ucln_common_ثبات_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
        correctedPOSIXLocale = اشاره_تهی;
    }
    posixID = gCorrectedPOSIXLocale;

    إذا (correctedPOSIXLocale != اشاره_تهی) {  /* Was already set - clean up. */
        uprv_free(correctedPOSIXLocale);
    }

    أرجع posixID;

#وإلاإذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
#تعریف_کن POSIX_LOCALE_ظرفیت 64
    UError_کد وضعیت = U_ZERO_ERROR;
    حرف *correctedPOSIXLocale = اشاره_تهی;

    // If we have already figured this out just use the cached value
    إذا (gCorrectedPOSIXLocale != اشاره_تهی) {
        أرجع gCorrectedPOSIXLocale;
    }

    // No cached value, need to determine the current value
    ساكن WCHAR windowsLocale[LOCALE_NAME_MAX_درازی] = {};
    صحيح length = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, windowsLocale, LOCALE_NAME_MAX_درازی);

    // Now we should have a Windows locale name that needs converted to the POSIX style.
    إذا (length > 0) // If length is 0, then the GetLocaleInfoEx failed.
    {
        // First we need to go from UTF-16 to char (and also convert from _ to - while we're at it.)
        حرف modifiedWindowsLocale[LOCALE_NAME_MAX_درازی] = {};

        int32_t i;
        لكل (i = 0; i < UPRV_LENGTHOF(modifiedWindowsLocale); i++)
        {
            إذا (windowsLocale[i] == '_')
            {
                modifiedWindowsLocale[i] = '-';
            }
            وإلا
            {
                modifiedWindowsLocale[i] = static_cast<حرف>(windowsLocale[i]);
            }

            إذا (modifiedWindowsLocale[i] == '\0')
            {
                اكسر;
            }
        }

        إذا (i >= UPRV_LENGTHOF(modifiedWindowsLocale))
        {
            // Ran out of room, can't really happen, maybe we'll be lucky about a matching
            // locale when tags are dropped
            modifiedWindowsLocale[UPRV_LENGTHOF(modifiedWindowsLocale) - 1] = '\0';
        }

        // Now normalize the resulting name
        correctedPOSIXLocale = static_cast<حرف *>(uprv_malloc(POSIX_LOCALE_ظرفیت + 1));
        /* TODO: Should we just exit on memory allocation failure? */
        إذا (correctedPOSIXLocale)
        {
            int32_t posixLen = uloc_canonicalize(modifiedWindowsLocale, correctedPOSIXLocale, POSIX_LOCALE_ظرفیت, &وضعیت);
            إذا (U_SUCCESS(وضعیت))
            {
                *(correctedPOSIXLocale + posixLen) = 0;
                gCorrectedPOSIXLocale = correctedPOSIXLocale;
                g_Corrected_POSIXLocale_انباشته_Allocated = درست;
                ucln_common_ثبات_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
            }
            وإلا
            {
                uprv_free(correctedPOSIXLocale);
            }
        }
    }

    // If unable to find a locale we can agree upon, use en-US by default
    إذا (gCorrectedPOSIXLocale == اشاره_تهی) {
        gCorrectedPOSIXLocale = "en_US";
    }
    أرجع gCorrectedPOSIXLocale;

#وإلاإذا U_PLATFORM == U_PF_OS400
    /* locales are process scoped and are by definition thread safe */
    ساكن حرف correctedLocale[64];
    ثابت  حرف *localeID = getenv("LC_ALL");
           حرف *p;

    إذا (localeID == اشاره_تهی)
        localeID = getenv("LANG");
    إذا (localeID == اشاره_تهی)
        localeID = setlocale(LC_ALL, اشاره_تهی);
    /* Make sure we have something... */
    إذا (localeID == اشاره_تهی)
        أرجع "en_US_POSIX";

    /* Extract the locale name from the path. */
    إذا((p = uprv_strrchr(localeID, '/')) != اشاره_تهی)
    {
        /* Increment p to start of locale name. */
        p++;
        localeID = p;
    }

    /* Copy to work location. */
    uprv_strcpy(correctedLocale, localeID);

    /* Strip off the '.locale' extension. */
    إذا((p = uprv_strchr(correctedLocale, '.')) != اشاره_تهی) {
        *p = 0;
    }

    /* Upper case the locale name. */
    T_CString_مقصد_فوقانی_Case(correctedLocale);

    /* See if we are using the POSIX locale.  Any of the
    * following are equivalent and use the same QLGPGCMA
    * (POSIX) locale.
    * QLGPGCMA2 means UCS2
    * QLGPGCMA_4 means UTF-32
    * QLGPGCMA_8 means UTF-8
    */
    إذا ((uprv_strcmp("C", correctedLocale) == 0) ||
        (uprv_strcmp("POSIX", correctedLocale) == 0) ||
        (uprv_strncmp("QLGPGCMA", correctedLocale, 8) == 0))
    {
        uprv_strcpy(correctedLocale, "en_US_POSIX");
    }
    وإلا
    {
        int16_t LocaleLen;

        /* Lower case the lang portion. */
        لكل(p = correctedLocale; *p != 0 && *p != '_'; p++)
        {
            *p = uprv_tolower(*p);
        }

        /* Adjust for Euro.  After '_E' add 'URO'. */
        LocaleLen = uprv_strlen(correctedLocale);
        إذا (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'E')
        {
            uprv_strcat(correctedLocale, "URO");
        }

        /* If using Lotus-based locale then convert to
         * equivalent non Lotus.
         */
        وإلا إذا (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'L')
        {
            correctedLocale[LocaleLen - 2] = 0;
        }

        /* There are separate simplified and traditional
         * locales called zh_HK_S and zh_HK_T.
         */
        وإلا إذا (uprv_strncmp(correctedLocale, "zh_HK", 5) == 0)
        {
            uprv_strcpy(correctedLocale, "zh_HK");
        }

        /* A special zh_CN_GBK locale...
        */
        وإلا إذا (uprv_strcmp(correctedLocale, "zh_CN_GBK") == 0)
        {
            uprv_strcpy(correctedLocale, "zh_CN");
        }

    }

    أرجع correctedLocale;
#پایان_اگر

}

#إذا !U_CHARSET_IS_UTF8
#إذا U_POSIX_LOCALE
/*
Due to various platform differences, one platform may specify a charset,
when they really mean a different charset. Remap the names so that they are
compatible with ICU. Only conflicting/ambiguous aliases should be resolved
here. Before adding anything to this function, please consider adding unique
names to the ICU alias table in the data directory.
*/
ساكن ثابت حرف*
remapPlatformDependentCodepage(ثابت حرف *locale, ثابت حرف *name) {
    إذا (locale != اشاره_تهی && *locale == 0) {
        /* Make sure that an empty locale is handled the same way. */
        locale = اشاره_تهی;
    }
    إذا (name == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
#إذا U_PLATFORM == U_PF_AIX
    إذا (uprv_strcmp(name, "IBM-943") == 0) {
        /* Use the ASCII compatible ibm-943 */
        name = "Shift-JIS";
    }
    وإلا إذا (uprv_strcmp(name, "IBM-1252") == 0) {
        /* Use the windows-1252 that contains the Euro */
        name = "IBM-5348";
    }
#وإلاإذا U_PLATFORM == U_PF_SOLARIS
    إذا (locale != اشاره_تهی && uprv_strcmp(name, "EUC") == 0) {
        /* Solaris underspecifies the "EUC" name. */
        إذا (uprv_strcmp(locale, "zh_CN") == 0) {
            name = "EUC-CN";
        }
        وإلا إذا (uprv_strcmp(locale, "zh_TW") == 0) {
            name = "EUC-TW";
        }
        وإلا إذا (uprv_strcmp(locale, "ko_KR") == 0) {
            name = "EUC-KR";
        }
    }
    وإلا إذا (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-954 is the best match.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    وإلا إذا (uprv_strcmp(name, "646") == 0) {
        /*
         * The default codepage given by Solaris is 646 but the C library routines treat it as if it was
         * ISO-8859-1 instead of US-ASCII(646).
         */
        name = "ISO-8859-1";
    }
#وإلاإذا U_PLATFORM_IS_DARWIN_BASED
    إذا (locale == اشاره_تهی && *name == 0) {
        /*
        No locale was specified, and an empty name was passed in.
        This usually indicates that nl_langinfo didn't return valid information.
        Mac OS X uses UTF-8 by default (especially the locale data and console).
        */
        name = "UTF-8";
    }
    وإلا إذا (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
    وإلا إذا (locale != اشاره_تهی && uprv_strcmp(locale, "en_US_POSIX") != 0 && uprv_strcmp(name, "US-ASCII") == 0) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
#وإلاإذا U_PLATFORM == U_PF_BSD
    إذا (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
#وإلاإذا U_PLATFORM == U_PF_HPUX
    إذا (locale != اشاره_تهی && uprv_strcmp(locale, "zh_HK") == 0 && uprv_strcmp(name, "big5") == 0) {
        /* HP decided to extend big5 as hkbig5 even though it's not compatible :-( */
        /* zh_TW.big5 is not the same charset as zh_HK.big5! */
        name = "hkbig5";
    }
    وإلا إذا (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
#وإلاإذا U_PLATFORM == U_PF_LINUX
    إذا (locale != اشاره_تهی && uprv_strcmp(name, "euc") == 0) {
        /* Linux underspecifies the "EUC" name. */
        إذا (uprv_strcmp(locale, "korean") == 0) {
            name = "EUC-KR";
        }
        وإلا إذا (uprv_strcmp(locale, "japanese") == 0) {
            /* See comment below about eucJP */
            name = "eucjis";
        }
    }
    وإلا إذا (uprv_strcmp(name, "eucjp") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    وإلا إذا (locale != اشاره_تهی && uprv_strcmp(locale, "en_US_POSIX") != 0 &&
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
#پایان_اگر
    /* return nullptr when "" is passed in */
    إذا (*name == 0) {
        name = اشاره_تهی;
    }
    أرجع name;
}

ساكن ثابت حرف*
getCodepageFromPOSIXID(ثابت حرف *localeName, حرف * میانگیر, int32_t buff_ظرفیت)
{
    حرف localeBuf[100];
    ثابت حرف *name = اشاره_تهی;
    حرف *variant = اشاره_تهی;

    إذا (localeName != اشاره_تهی && (name = (uprv_strchr(localeName, '.'))) != اشاره_تهی) {
        size_t locale_ظرفیت = uprv_min(حجم(localeBuf), (name-localeName)+1);
        uprv_strncpy(localeBuf, localeName, locale_ظرفیت);
        localeBuf[locale_ظرفیت-1] = 0; /* ensure NUL termination */
        name = uprv_strncpy(میانگیر, name+1, buff_ظرفیت);
        میانگیر[buff_ظرفیت-1] = 0; /* ensure NUL termination */
        إذا ((variant = const_cast<حرف *>(uprv_strchr(name, '@'))) != اشاره_تهی) {
            *variant = 0;
        }
        name = remapPlatformDependentCodepage(localeBuf, name);
    }
    أرجع name;
}
#پایان_اگر

ساكن ثابت حرف*
int_getDefaultCodepage()
{
#إذا U_PLATFORM == U_PF_OS400
    uint32_t ccsid = 37; /* Default to ibm-37 */
    ساكن حرف codepage[64];
    Qwc_JOBI0400_t jobinfo;
    Qus_EC_t error = { حجم(Qus_EC_t) }; /* SPI error code */

    EPT_CALL(QUSRJOBI)(&jobinfo, حجم(jobinfo), "JOBI0400",
        "*                         ", "                ", &error);

    إذا (error.بایتها_موجود == 0) {
        إذا (jobinfo.Coded_Char_مجموعه_ID != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Coded_Char_مجموعه_ID;
        }
        وإلا إذا (jobinfo.Default_Coded_Char_مجموعه_Id != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Default_Coded_Char_مجموعه_Id;
        }
        /* else use the default */
    }
    snprintf(codepage, حجم(codepage), "ibm-%d", ccsid);
    أرجع codepage;

#وإلاإذا U_PLATFORM == U_PF_OS390
    ساكن حرف codepage[64];

    strncpy(codepage, nl_langinfo(CODESET),63-طول_رشته(UCNV_جابجا_LFNL_OPTION_STRING));
    strcat(codepage,UCNV_جابجا_LFNL_OPTION_STRING);
    codepage[63] = 0; /* NUL terminate */

    أرجع codepage;

#وإلاإذا U_PLATFORM_استفاده_میکند_ONLY_WIN32_API
    ساكن حرف codepage[64];
    DWORD codepage_عدد = 0;

#إذا U_PLATFORM_HAS_WINUWP_API == 1
    // UWP doesn't have a direct API to get the default ACP as Microsoft would rather
    // have folks use Unicode than a "system" code page, however this is the same
    // codepage as the system default locale codepage.  (FWIW, the system locale is
    // ONLY used for codepage, it should never be used for anything else)
    GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_عدد,
        (LPWSTR)&codepage_عدد, حجم(codepage_عدد) / حجم(WCHAR));
#وإلا
    // Win32 apps can call GetACP
    codepage_عدد = GetACP();
#پایان_اگر
    // Special case for UTF-8
    إذا (codepage_عدد == 65001)
    {
        أرجع "UTF-8";
    }
    // Windows codepages can look like windows-1252, so format the found number
    // the numbers are eclectic, however all valid system code pages, besides UTF-8
    // are between 3 and 19999
    إذا (codepage_عدد > 0 && codepage_عدد < 20000)
    {
        snprintf(codepage, حجم(codepage), "windows-%ld", codepage_عدد);
        أرجع codepage;
    }
    // If the codepage number call failed then return UTF-8
    أرجع "UTF-8";

#وإلاإذا U_POSIX_LOCALE
    ساكن حرف codesetName[100];
    ثابت حرف *localeName = اشاره_تهی;
    ثابت حرف *name = اشاره_تهی;

    localeName = uprv_getPOSIXIDForDefaultCodepage();
    uprv_memset(codesetName, 0, حجم(codesetName));
    /* On Solaris nl_langinfo returns C locale values unless setlocale
     * was called earlier.
     */
#إذا (U_HAVE_NL_LANGINFO_CODESET && U_PLATFORM != U_PF_SOLARIS)
    /* When available, check nl_langinfo first because it usually gives more
       useful names. It depends on LC_CTYPE.
       nl_langinfo may use the same buffer as setlocale. */
    {
        ثابت حرف *codeset = nl_langinfo(U_NL_LANGINFO_CODESET);
#إذا U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED
        /*
         * On Linux and MacOSX, ensure that default codepage for non C/POSIX locale is UTF-8
         * instead of ASCII.
         */
        إذا (uprv_strcmp(localeName, "en_US_POSIX") != 0) {
            codeset = remapPlatformDependentCodepage(localeName, codeset);
        } وإلا
#پایان_اگر
        {
            codeset = remapPlatformDependentCodepage(اشاره_تهی, codeset);
        }

        إذا (codeset != اشاره_تهی) {
            uprv_strncpy(codesetName, codeset, حجم(codesetName));
            codesetName[حجم(codesetName)-1] = 0;
            أرجع codesetName;
        }
    }
#پایان_اگر

    /* Use setlocale in a nice way, and then check some environment variables.
       Maybe the application used setlocale already.
    */
    uprv_memset(codesetName, 0, حجم(codesetName));
    name = getCodepageFromPOSIXID(localeName, codesetName, حجم(codesetName));
    إذا (name) {
        /* if we can find the codeset name from setlocale, return that. */
        أرجع name;
    }

    إذا (*codesetName == 0)
    {
        /* Everything failed. Return US ASCII (ISO 646). */
        (فراغ)uprv_strcpy(codesetName, "US-ASCII");
    }
    أرجع codesetName;
#وإلا
    أرجع "US-ASCII";
#پایان_اگر
}


U_CAPI ثابت حرف*  U_EXPORT2
uprv_getDefaultCodepage()
{
    ساكن حرف ثابت  *name = اشاره_تهی;
    umtx_قفل(اشاره_تهی);
    إذا (name == اشاره_تهی) {
        name = int_getDefaultCodepage();
    }
    umtx_unlock(اشاره_تهی);
    أرجع name;
}
#پایان_اگر  /* !U_CHARSET_IS_UTF8 */


/* end of platform-specific implementation -------------- */

/* version handling --------------------------------------------------------- */

U_CAPI فراغ U_EXPORT2
u_نسخه_From_String(UVersionInfo نسخه_Array, ثابت حرف *نسخه_String) {
    حرف *انتها;
    uint16_t part=0;

    إذا(نسخه_Array==اشاره_تهی) {
        أرجع;
    }

    إذا(نسخه_String!=اشاره_تهی) {
        لكل(;;) {
            نسخه_Array[part]=(uint8_t)uprv_strtoul(نسخه_String, &انتها, 10);
            إذا(انتها==نسخه_String || ++part==U_MAX_نسخه_درازی || *انتها!=U_نسخه_DELIMITER) {
                اكسر;
            }
            نسخه_String=انتها+1;
        }
    }

    طالما(part<U_MAX_نسخه_درازی) {
        نسخه_Array[part++]=0;
    }
}

U_CAPI فراغ U_EXPORT2
u_نسخه_From_UString(UVersionInfo نسخه_Array, ثابت char16_t *نسخه_String) {
    إذا(نسخه_Array!=اشاره_تهی && نسخه_String!=اشاره_تهی) {
        حرف نسخه_Chars[U_MAX_نسخه_STRING_درازی+1];
        int32_t len = u_strlen(نسخه_String);
        إذا(len>U_MAX_نسخه_STRING_درازی) {
            len = U_MAX_نسخه_STRING_درازی;
        }
        u_UChars_مقصد_Chars(نسخه_String, نسخه_Chars, len);
        نسخه_Chars[len]=0;
        u_نسخه_From_String(نسخه_Array, نسخه_Chars);
    }
}

U_CAPI فراغ U_EXPORT2
u_نسخه_مقصد_String(ثابت UVersionInfo نسخه_Array, حرف *نسخه_String) {
    uint16_t count, part;
    uint8_t فیلد_2;

    إذا(نسخه_String==اشاره_تهی) {
        أرجع;
    }

    إذا(نسخه_Array==اشاره_تهی) {
        نسخه_String[0]=0;
        أرجع;
    }

    /* count how many fields need to be written */
    لكل(count=4; count>0 && نسخه_Array[count-1]==0; --count) {
    }

    إذا(count <= 1) {
        count = 2;
    }

    /* write the first part */
    /* write the decimal field value */
    فیلد_2=نسخه_Array[0];
    إذا(فیلد_2>=100) {
        *نسخه_String++=(حرف)('0'+فیلد_2/100);
        فیلد_2%=100;
    }
    إذا(فیلد_2>=10) {
        *نسخه_String++=(حرف)('0'+فیلد_2/10);
        فیلد_2%=10;
    }
    *نسخه_String++=(حرف)('0'+فیلد_2);

    /* write the following parts */
    لكل(part=1; part<count; ++part) {
        /* write a dot first */
        *نسخه_String++=U_نسخه_DELIMITER;

        /* write the decimal field value */
        فیلد_2=نسخه_Array[part];
        إذا(فیلد_2>=100) {
            *نسخه_String++=(حرف)('0'+فیلد_2/100);
            فیلد_2%=100;
        }
        إذا(فیلد_2>=10) {
            *نسخه_String++=(حرف)('0'+فیلد_2/10);
            فیلد_2%=10;
        }
        *نسخه_String++=(حرف)('0'+فیلد_2);
    }

    /* NUL-terminate */
    *نسخه_String=0;
}

U_CAPI فراغ U_EXPORT2
u_get_نسخه(UVersionInfo نسخه_Array) {
    (فراغ)copyright;   // Suppress unused variable warning from clang.
    u_نسخه_From_String(نسخه_Array, U_ICU_نسخه);
}

/**
 * icucfg.h dependent code
 */

#إذا U_ENABLE_DYLOAD && HAVE_DLOPEN && !U_PLATFORM_استفاده_میکند_ONLY_WIN32_API

#إذا HAVE_DLFCN_H
#اگر_تعریف_شده __MVS__
#اگر_تعریف_نشده __SUSV3
#تعریف_کن __SUSV3 1
#پایان_اگر
#پایان_اگر
#ضمن <dlfcn.h>
#پایان_اگر /* HAVE_DLFCN_H */

U_CAPI فراغ * U_EXPORT2
uprv_dl_open(ثابت حرف *libName, UError_کد *وضعیت) {
  فراغ *ret = اشاره_تهی;
  إذا(U_FAILURE(*وضعیت)) أرجع ret;
  ret =  dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
  إذا(ret==اشاره_تهی) {
#اگر_تعریف_شده U_TRACE_DYLOAD
    چاپ_کن("dlerror on dlopen(%s): %s\n", libName, dlerror());
#پایان_اگر
    *وضعیت = U_MISSING_RESOURCE_ERROR;
  }
  أرجع ret;
}

U_CAPI فراغ U_EXPORT2
uprv_dl_close(فراغ *lib, UError_کد *وضعیت) {
  إذا(U_FAILURE(*وضعیت)) أرجع;
  dlclose(lib);
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(فراغ *lib, ثابت حرف* sym, UError_کد *وضعیت) {
  اتحاد {
      UVoidFunction *fp;
      فراغ *vp;
  } uret;
  uret.fp = اشاره_تهی;
  إذا(U_FAILURE(*وضعیت)) أرجع uret.fp;
  uret.vp = dlsym(lib, sym);
  إذا(uret.vp == اشاره_تهی) {
#اگر_تعریف_شده U_TRACE_DYLOAD
    چاپ_کن("dlerror on dlsym(%p,%s): %s\n", lib,sym, dlerror());
#پایان_اگر
    *وضعیت = U_MISSING_RESOURCE_ERROR;
  }
  أرجع uret.fp;
}

#وإلاإذا U_ENABLE_DYLOAD && U_PLATFORM_استفاده_میکند_ONLY_WIN32_API && !U_PLATFORM_HAS_WINUWP_API

/* Windows API implementation. */
// Note: UWP does not expose/allow these APIs, so the UWP version gets the null implementation. */

U_CAPI فراغ * U_EXPORT2
uprv_dl_open(ثابت حرف *libName, UError_کد *وضعیت) {
  HMODULE lib = اشاره_تهی;

  إذا(U_FAILURE(*وضعیت)) أرجع اشاره_تهی;

  lib = بارگیری_Library_A(libName);

  إذا(lib==اشاره_تهی) {
    *وضعیت = U_MISSING_RESOURCE_ERROR;
  }

  أرجع (فراغ*)lib;
}

U_CAPI فراغ U_EXPORT2
uprv_dl_close(فراغ *lib, UError_کد *وضعیت) {
  HMODULE handle = (HMODULE)lib;
  إذا(U_FAILURE(*وضعیت)) أرجع;

  FreeLibrary(handle);

  أرجع;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(فراغ *lib, ثابت حرف* sym, UError_کد *وضعیت) {
  HMODULE handle = (HMODULE)lib;
  UVoidFunction* addr = اشاره_تهی;

  إذا(U_FAILURE(*وضعیت) || lib==اشاره_تهی) أرجع اشاره_تهی;

  addr = (UVoidFunction*)Get_Proc_آدرس(handle, sym);

  إذا(addr==اشاره_تهی) {
    DWORD آخرین_Error = Get_آخرین_Error();
    إذا(آخرین_Error == ERROR_PROC_NOT_یافت) {
      *وضعیت = U_MISSING_RESOURCE_ERROR;
    } وإلا {
      *وضعیت = U_UNSUPPORTED_ERROR; /* other unknown error. */
    }
  }

  أرجع addr;
}

#وإلا

/* No dynamic loading, null (nonexistent) implementation. */

U_CAPI فراغ * U_EXPORT2
uprv_dl_open(ثابت حرف *libName, UError_کد *وضعیت) {
    (فراغ)libName;
    إذا(U_FAILURE(*وضعیت)) أرجع اشاره_تهی;
    *وضعیت = U_UNSUPPORTED_ERROR;
    أرجع اشاره_تهی;
}

U_CAPI فراغ U_EXPORT2
uprv_dl_close(فراغ *lib, UError_کد *وضعیت) {
    (فراغ)lib;
    إذا(U_FAILURE(*وضعیت)) أرجع;
    *وضعیت = U_UNSUPPORTED_ERROR;
    أرجع;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(فراغ *lib, ثابت حرف* sym, UError_کد *وضعیت) {
  (فراغ)lib;
  (فراغ)sym;
  إذا(U_SUCCESS(*وضعیت)) {
    *وضعیت = U_UNSUPPORTED_ERROR;
  }
  أرجع (UVoidFunction*)اشاره_تهی;
}

#پایان_اگر

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

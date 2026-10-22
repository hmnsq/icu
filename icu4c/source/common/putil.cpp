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
#అన్తర్భావ "uposixdefs.h"

// First, the platform type. Need this for U_PLATFORM.
#అన్తర్భావ "unicode/platform.h"

/*
 * Cygwin with GCC requires inclusion of time.h after the above disabling strict asci mode statement.
 */
#అన్తర్భావ <time.h>

#ఒకవేళ !U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
#అన్తర్భావ <sys/time.h>
#అయితే_ముగింపు

/* include the rest of the ICU headers */
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "locmap.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "charstr.h"

/* Include standard headers. */
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <math.h>
#అన్తర్భావ <locale.h>
#అన్తర్భావ <float.h>

#నిర్వచితం_కాకపోతే U_COMMON_IMPLEMENTATION
#దోషం U_COMMON_IMPLEMENTATION not సమితి - must be సమితి కొరకు all ICU ఆధారం files in common/ - see https://unicode-org.github.io/icu/userguide/icu/howtouseicu.html
#అయితే_ముగింపు


/* include system headers */
#ఒకవేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
    /*
     * TODO: U_PLATFORM_USES_ONLY_WIN32_API includes MinGW.
     * Should Cygwin be included as well (U_PLATFORM_HAS_WIN32_API)
     * to use native APIs as much as possible?
     */
#నిర్వచితం_కాకపోతే WIN32_LEAN_AND_సగటు
#   define WIN32_LEAN_AND_సగటు
#అయితే_ముగింపు
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>
#   include "unicode/uloc.h"
#   include "wintz.h"
#లేదావేళ U_PLATFORM == U_PF_OS400
#   include <float.h>
#   include <qusec.h>       /* error code structure */
#   include <qusrjobi.h>
#   include <qliept.h>      /* EPT_CALL macro  - this include must be after all other "QSYSINCs" */
#   include <mih/testptr.h> /* For uprv_maximumPtr */
#లేదావేళ U_PLATFORM == U_PF_OS390
#   include "unicode/ucnv.h"   /* Needed for UCNV_SWAP_LFNL_OPTION_STRING */
#లేదావేళ U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS
#   include <limits.h>
#   include <unistd.h>
#   ఒకవేళ U_PLATFORM == U_PF_SOLARIS
#       ifndef _XPG4_2
#           define _XPG4_2
#       endif
#   elif U_PLATFORM == U_PF_ANDROID
#       include <sys/system_properties.h>
#       include <dlfcn.h>
#   endif
#లేదావేళ U_PLATFORM == U_PF_QNX
#   include <sys/neutrino.h>
#అయితే_ముగింపు


/*
 * Only include langinfo.h if we have a way to get the codeset. If we later
 * depend on more feature, we can test on U_HAVE_NL_LANGINFO.
 *
 */

#ఒకవేళ U_HAVE_NL_LANGINFO_CODESET
#అన్తర్భావ <langinfo.h>
#అయితే_ముగింపు

/**
 * Simple things (presence of functions, etc) should just go in configure.in and be added to
 * icucfg.h via autoheader.
 */
#ఒకవేళ U_PLATFORM_IMPLEMENTS_POSIX
#   ఒకవేళ U_PLATFORM == U_PF_OS400
#    define HAVE_DLFCN_H 0
#    define HAVE_DLOPEN 0
#   లేకపోతే
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
#లేకపోతే
#   define HAVE_DLFCN_H 0
#   define HAVE_DLOPEN 0
#   define HAVE_GETTIMEOFDAY 0
#అయితే_ముగింపు

U_NAMESPACE_USE

/* Define the extension for data files, again... */
#నిర్వచించు దత్తాంశం_TYPE "dat"

/* Leave this copyright notice here! */
స్థిర స్థిరరాశి అక్షరం copyright[] = U_COPYRIGHT_STRING;

/* floating point implementations ------------------------------------------- */

/* We return QNAN rather than SNAN*/
#నిర్వచించు SIGN 0x80000000U

/* Make it easy to define certain types of constants */
రకనిర్వచనం సంఘం {
    int64_t i64; /* This must be defined first in order to allow the initialization to work. This is a C89 feature. */
    ద్విగుణ d64;
} బిట్టు_నమూనా_Conversion;
స్థిర స్థిరరాశి బిట్టు_నమూనా_Conversion gNan = {static_cast<int64_t>(INT64_C(0x7FF8000000000000))};
స్థిర స్థిరరాశి బిట్టు_నమూనా_Conversion gInf = {static_cast<int64_t>(INT64_C(0x7FF0000000000000))};

/*---------------------------------------------------------------------------
  Platform utilities
  Our general strategy is to assume we're on a POSIX platform.  Platforms which
  are non-POSIX must declare themselves so.  The default POSIX implementation
  will sometimes work for non-POSIX platforms as well (e.g., the NaN-related
  functions).
  ---------------------------------------------------------------------------*/

#ఒకవేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API || U_PLATFORM == U_PF_OS400
#   undef U_POSIX_LOCALE
#లేకపోతే
#   define U_POSIX_LOCALE    1
#అయితే_ముగింపు

/*
    WARNING! u_topNBytesOfDouble and u_bottomNBytesOfDouble
    can't be properly optimized by the gcc compiler sometimes (i.e. gcc 3.2).
*/
#ఒకవేళ !IEEE_754
స్థిర అక్షరం*
u_శిఖరం_NBytes_యొక్క_Double(ద్విగుణ* d, పూర్ణాంకం n)
{
#ఒకవేళ U_IS_BIG_ENDIAN
    తిరిగి (అక్షరం*)d;
#లేకపోతే
    తిరిగి (అక్షరం*)(d + 1) - n;
#అయితే_ముగింపు
}

స్థిర అక్షరం*
u_bottom_NBytes_యొక్క_Double(ద్విగుణ* d, పూర్ణాంకం n)
{
#ఒకవేళ U_IS_BIG_ENDIAN
    తిరిగి (అక్షరం*)(d + 1) - n;
#లేకపోతే
    తిరిగి (అక్షరం*)d;
#అయితే_ముగింపు
}
#అయితే_ముగింపు   /* !IEEE_754 */

#ఒకవేళ IEEE_754
స్థిర UBool
u_sign_బిట్టు(ద్విగుణ d) {
    uint8_t hiByte;
#ఒకవేళ U_IS_BIG_ENDIAN
    hiByte = *(uint8_t *)&d;
#లేకపోతే
    hiByte = *(reinterpret_cast<uint8_t*>(&d) + పరిమాణం(ద్విగుణ) - 1);
#అయితే_ముగింపు
    తిరిగి (hiByte & 0x80) != 0;
}
#అయితే_ముగింపు



#ఒకవేళ defined (U_DEBUG_FAKETIME)
/* Override the clock to test things without having to move the system clock.
 * Assumes POSIX gettimeofday() will function
 */
UDate fakeClock_t0 = 0; /** Time to start the clock from **/
UDate fakeClock_dt = 0; /** Offset (fake time - real time) **/
UBool fake_Clock_సమితి = అబద్ధం; /** True if fake clock has spun up **/

స్థిర UDate getUTCtime_real() {
    నిర్మాణం timeval posix_సమయం;
    gettimeofday(&posix_సమయం, శూన్యసూచిక);
    తిరిగి (UDate)(((int64_t)posix_సమయం.tv_sec * U_MILLIS_చొప్పున_రెండవ) + (posix_సమయం.tv_usec/1000));
}

స్థిర UDate getUTCtime_fake() {
    స్థిర UMutex fakeClockMutex;
    umtx_తాళం(&fakeClockMutex);
    ఒకవేళ(!fake_Clock_సమితి) {
        UDate real = getUTCtime_real();
        స్థిరరాశి అక్షరం *fake_ప్రారంభం = getenv("U_FAKETIME_START");
        ఒకవేళ((fake_ప్రారంభం!=శూన్యసూచిక) && (fake_ప్రారంభం[0]!=0)) {
            అక్షరశ్రేణి_చదువు(fake_ప్రారంభం,"%lf",&fakeClock_t0);
            fakeClock_dt = fakeClock_t0 - real;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, so the ICU clock will start at a preset value\n"
                    "env variable U_FAKETIME_START=%.0f (%s) for an offset of %.0f ms from the current time %.0f\n",
                    fakeClock_t0, fake_ప్రారంభం, fakeClock_dt, real);
        } లేకపోతే {
          fakeClock_dt = 0;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, but U_FAKETIME_START was not set.\n"
                    "Set U_FAKETIME_START to the number of milliseconds since 1/1/1970 to set the ICU clock.\n");
        }
        fake_Clock_సమితి = నిజం;
    }
    umtx_unlock(&fakeClockMutex);

    తిరిగి getUTCtime_real() + fakeClock_dt;
}
#అయితే_ముగింపు

#ఒకవేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
రకనిర్వచనం సంఘం {
    int64_t int64;
    FILETIME file_సమయం;
} File_సమయం_Conversion;   /* This is like a ULARGE_INTEGER */

/* Number of 100 nanoseconds from 1/1/1601 to 1/1/1970 */
#నిర్వచించు EPOCH_BIAS  INT64_C(116444736000000000)
#నిర్వచించు HECTONANOSECOND_చొప్పున_MILLISECOND   10000

#అయితే_ముగింపు

/*---------------------------------------------------------------------------
  Universal Implementations
  These are designed to work on all platforms.  Try these, and if they
  don't work on your platform, then special case your platform with new
  implementations.
---------------------------------------------------------------------------*/

U_CAPI UDate U_EXPORT2
uprv_getUTCtime()
{
#ఒకవేళ defined(U_DEBUG_FAKETIME)
    తిరిగి getUTCtime_fake(); /* Hook for overriding the clock */
#లేకపోతే
    తిరిగి uprv_getRawUTCtime();
#అయితే_ముగింపు
}

/* Return UTC (GMT) time measured in milliseconds since 0:00 on 1/1/70.*/
U_CAPI UDate U_EXPORT2
uprv_getRawUTCtime()
{
#ఒకవేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API

    File_సమయం_Conversion win_సమయం;
    Get_System_సమయం_As_File_సమయం(&win_సమయం.file_సమయం);
    తిరిగి (UDate)((win_సమయం.int64 - EPOCH_BIAS) / HECTONANOSECOND_చొప్పున_MILLISECOND);
#లేకపోతే

#ఒకవేళ HAVE_GETTIMEOFDAY
    నిర్మాణం timeval posix_సమయం;
    gettimeofday(&posix_సమయం, శూన్యసూచిక);
    తిరిగి (UDate)(((int64_t)posix_సమయం.tv_sec * U_MILLIS_చొప్పున_రెండవ) + (posix_సమయం.tv_usec/1000));
#లేకపోతే
    time_t epochtime;
    time(&epochtime);
    తిరిగి (UDate)epochtime * U_MILLIS_చొప్పున_రెండవ;
#అయితే_ముగింపు

#అయితే_ముగింపు
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
uprv_isNaN(ద్విగుణ number)
{
#ఒకవేళ IEEE_754
    బిట్టు_నమూనా_Conversion converted_సంఖ్య;
    converted_సంఖ్య.d64 = number;
    /* Infinity is 0x7FF0000000000000U. Anything greater than that is a NaN */
    తిరిగి (converted_సంఖ్య.i64 & U_INT64_MAX) > gInf.i64;

#లేదావేళ U_PLATFORM == U_PF_OS390
    uint32_t ఎగువ_బిట్లు = *(uint32_t*)u_శిఖరం_NBytes_యొక్క_Double(&number,
                        పరిమాణం(uint32_t));
    uint32_t దిగువ_బిట్లు  = *(uint32_t*)u_bottom_NBytes_యొక్క_Double(&number,
                        పరిమాణం(uint32_t));

    తిరిగి ((ఎగువ_బిట్లు & 0x7F080000L) == 0x7F080000L) &&
      (దిగువ_బిట్లు == 0x00000000L);

#లేకపోతే
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    తిరిగి number != number;
#అయితే_ముగింపు
}

U_CAPI UBool U_EXPORT2
uprv_isInfinite(ద్విగుణ number)
{
#ఒకవేళ IEEE_754
    బిట్టు_నమూనా_Conversion converted_సంఖ్య;
    converted_సంఖ్య.d64 = number;
    /* Infinity is exactly 0x7FF0000000000000U. */
    తిరిగి (converted_సంఖ్య.i64 & U_INT64_MAX) == gInf.i64;
#లేదావేళ U_PLATFORM == U_PF_OS390
    uint32_t ఎగువ_బిట్లు = *(uint32_t*)u_శిఖరం_NBytes_యొక్క_Double(&number,
                        పరిమాణం(uint32_t));
    uint32_t దిగువ_బిట్లు  = *(uint32_t*)u_bottom_NBytes_యొక్క_Double(&number,
                        పరిమాణం(uint32_t));

    తిరిగి ((ఎగువ_బిట్లు  & ~SIGN) == 0x70FF0000L) && (దిగువ_బిట్లు == 0x00000000L);

#లేకపోతే
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    తిరిగి number == (2.0 * number);
#అయితే_ముగింపు
}

U_CAPI UBool U_EXPORT2
uprv_is_ధనాత్మక_Infinity(ద్విగుణ number)
{
#ఒకవేళ IEEE_754 || U_PLATFORM == U_PF_OS390
    తిరిగి number > 0 && uprv_isInfinite(number);
#లేకపోతే
    తిరిగి uprv_isInfinite(number);
#అయితే_ముగింపు
}

U_CAPI UBool U_EXPORT2
uprv_isNegativeInfinity(ద్విగుణ number)
{
#ఒకవేళ IEEE_754 || U_PLATFORM == U_PF_OS390
    తిరిగి number < 0 && uprv_isInfinite(number);

#లేకపోతే
    uint32_t ఎగువ_బిట్లు = *(uint32_t*)u_శిఖరం_NBytes_యొక్క_Double(&number,
                        పరిమాణం(uint32_t));
    తిరిగి((ఎగువ_బిట్లు & SIGN) && uprv_isInfinite(number));

#అయితే_ముగింపు
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_getNaN()
{
#ఒకవేళ IEEE_754 || U_PLATFORM == U_PF_OS390
    తిరిగి gNan.d64;
#లేకపోతే
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    తిరిగి 0.0;
#అయితే_ముగింపు
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_getInfinity()
{
#ఒకవేళ IEEE_754 || U_PLATFORM == U_PF_OS390
    తిరిగి gInf.d64;
#లేకపోతే
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    తిరిగి 0.0;
#అయితే_ముగింపు
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_floor(ద్విగుణ x)
{
    తిరిగి floor(x);
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_ceil(ద్విగుణ x)
{
    తిరిగి ceil(x);
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_విడత(ద్విగుణ x)
{
    తిరిగి uprv_floor(x + 0.5);
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_fabs(ద్విగుణ x)
{
    తిరిగి fabs(x);
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_modf(ద్విగుణ x, ద్విగుణ* y)
{
    తిరిగి modf(x, y);
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_fmod(ద్విగుణ x, ద్విగుణ y)
{
    తిరిగి fmod(x, y);
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_pow(ద్విగుణ x, ద్విగుణ y)
{
    /* This is declared as "double pow(double x, double y)" */
    తిరిగి ఘాతం(x, y);
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_pow10(int32_t x)
{
    తిరిగి ఘాతం(10.0, (ద్విగుణ)x);
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_fmax(ద్విగుణ x, ద్విగుణ y)
{
#ఒకవేళ IEEE_754
    /* first handle NaN*/
    ఒకవేళ(uprv_isNaN(x) || uprv_isNaN(y))
        తిరిగి uprv_getNaN();

    /* check for -0 and 0*/
    ఒకవేళ(x == 0.0 && y == 0.0 && u_sign_బిట్టు(x))
        తిరిగి y;

#అయితే_ముగింపు

    /* this should work for all flt point w/o NaN and Inf special cases */
    తిరిగి (x > y ? x : y);
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_fmin(ద్విగుణ x, ద్విగుణ y)
{
#ఒకవేళ IEEE_754
    /* first handle NaN*/
    ఒకవేళ(uprv_isNaN(x) || uprv_isNaN(y))
        తిరిగి uprv_getNaN();

    /* check for -0 and 0*/
    ఒకవేళ(x == 0.0 && y == 0.0 && u_sign_బిట్టు(y))
        తిరిగి y;

#అయితే_ముగింపు

    /* this should work for all flt point w/o NaN and Inf special cases */
    తిరిగి (x > y ? y : x);
}

U_CAPI UBool U_EXPORT2
uprv_add32_అధికప్రవాహం(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_add_overflow.
    // This function could be optimized by calling one of those primitives.
    స్వయంచాలక a64 = static_cast<int64_t>(a);
    స్వయంచాలక b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 + b64;
    *res = static_cast<int32_t>(res64);
    తిరిగి res64 != *res;
}

U_CAPI UBool U_EXPORT2
uprv_mul32_అధికప్రవాహం(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_mul_overflow.
    // This function could be optimized by calling one of those primitives.
    స్వయంచాలక a64 = static_cast<int64_t>(a);
    స్వయంచాలక b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 * b64;
    *res = static_cast<int32_t>(res64);
    తిరిగి res64 != *res;
}

/**
 * Truncates the given double.
 * trunc(3.3) = 3.0, trunc (-3.3) = -3.0
 * This is different than calling floor() or ceil():
 * floor(3.3) = 3, floor(-3.3) = -4
 * ceil(3.3) = 4, ceil(-3.3) = -3
 */
U_CAPI ద్విగుణ U_EXPORT2
uprv_trunc(ద్విగుణ d)
{
#ఒకవేళ IEEE_754
    /* handle error cases*/
    ఒకవేళ(uprv_isNaN(d))
        తిరిగి uprv_getNaN();
    ఒకవేళ(uprv_isInfinite(d))
        తిరిగి uprv_getInfinity();

    ఒకవేళ(u_sign_బిట్టు(d))    /* Signbit() picks up -0.0;  d<0 does not. */
        తిరిగి ceil(d);
    లేకపోతే
        తిరిగి floor(d);

#లేకపోతే
    తిరిగి d >= 0 ? floor(d) : ceil(d);

#అయితే_ముగింపు
}

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 */
U_CAPI ద్విగుణ U_EXPORT2
uprv_maxMantissa()
{
    తిరిగి ఘాతం(2.0, DBL_MANT_DIG + 1.0) - 1.0;
}

U_CAPI ద్విగుణ U_EXPORT2
uprv_log(ద్విగుణ d)
{
    తిరిగి సంవర్గమానం(d);
}

U_CAPI శూన్య * U_EXPORT2
uprv_గరిష్ఠ_Ptr(శూన్య * పునాది)
{
#ఒకవేళ U_PLATFORM == U_PF_OS400
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
    ఒకవేళ ((పునాది != శూన్యసూచిక) && (_TESTPTR(పునాది, _C_TERASPACE_CHECK))) {
        /* if it is a TERASPACE pointer the max is 2GB - 4k */
        తిరిగి ((శూన్య *)(((అక్షరం *)పునాది)-((uint32_t)(పునాది))+((uint32_t)0x7fffefff)));
    }
    /* otherwise 16MB since nullptr ptr is not checkable or the ptr is not TERASPACE */
    తిరిగి ((శూన్య *)(((అక్షరం *)పునాది)-((uint32_t)(పునాది))+((uint32_t)0xffefff)));

#లేకపోతే
    తిరిగి U_MAX_PTR(పునాది);
#అయితే_ముగింపు
}

/*---------------------------------------------------------------------------
  Platform-specific Implementations
  Try these, and if they don't work on your platform, then special case your
  platform with new implementations.
  ---------------------------------------------------------------------------*/

/* Generic time zone layer -------------------------------------------------- */

/* Time zone utilities */
U_CAPI శూన్య U_EXPORT2
uprv_tzset()
{
#ఒకవేళ defined(U_TZSET)
    U_TZSET();
#లేకపోతే
    /* no initialization*/
#అయితే_ముగింపు
}

U_CAPI int32_t U_EXPORT2
uprv_timezone()
{
#నిర్వచితమైతే U_TIMEZONE
    తిరిగి U_TIMEZONE;
#లేకపోతే
    time_t t, t1, t2;
    నిర్మాణం tm tmrec;
    int32_t tdiff = 0;

    time(&t);
    uprv_memcpy( &tmrec, localtime(&t), పరిమాణం(tmrec) );
#ఒకవేళ U_PLATFORM != U_PF_IPHONE
    UBool dst_checked = (tmrec.tm_isdst != 0); /* daylight savings time is checked*/
#అయితే_ముగింపు
    t1 = mktime(&tmrec);                 /* local time in seconds*/
    uprv_memcpy( &tmrec, gmtime(&t), పరిమాణం(tmrec) );
    t2 = mktime(&tmrec);                 /* GMT (or UTC) in seconds*/
    tdiff = t2 - t1;

#ఒకవేళ U_PLATFORM != U_PF_IPHONE
    /* imitate NT behaviour, which returns same timezone offset to GMT for
       winter and summer.
       This does not work on all platforms. For instance, on glibc on Linux
       and on Mac OS 10.5, tdiff calculated above remains the same
       regardless of whether DST is in effect or not. iOS is another
       platform where this does not work. Linux + glibc and Mac OS 10.5
       have U_TIMEZONE defined so that this code is not reached.
    */
    ఒకవేళ (dst_checked)
        tdiff += 3600;
#అయితే_ముగింపు
    తిరిగి tdiff;
#అయితే_ముగింపు
}

/* Note that U_TZNAME does *not* have to be tzname, but if it is,
   some platforms need to have it declared here. */

#ఒకవేళ defined(U_TZNAME) && (U_PLATFORM == U_PF_IRIX || U_PLATFORM_IS_DARWIN_BASED)
/* RS6000 and others reject char **tzname.  */
బాహ్య U_IMPORT అక్షరం *U_TZNAME[];
#అయితే_ముగింపు

#ఒకవేళ !UCONFIG_NO_FILE_IO && ((U_PLATFORM_IS_DARWIN_BASED && (U_PLATFORM != U_PF_IPHONE || defined(U_TIMEZONE))) || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS)
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

#నిర్వచించు CHECK_LOCALTIME_LINK 1
#ఒకవేళ U_PLATFORM_IS_DARWIN_BASED
#అన్తర్భావ <tzfile.h>
#నిర్వచించు TZZONEINFO      (TZDIR "/")
#లేదావేళ U_PLATFORM == U_PF_SOLARIS
#నిర్వచించు TZDEFAULT       "/etc/localtime"
#నిర్వచించు TZZONEINFO      "/usr/share/lib/zoneinfo/"
#నిర్వచించు TZ_ENV_CHECK    "localtime"
#లేకపోతే
#నిర్వచించు TZDEFAULT       "/etc/localtime"
#నిర్వచించు TZZONEINFO      "/usr/share/zoneinfo/"
#అయితే_ముగింపు
#నిర్వచించు TZZONEINFOTAIL  "/zoneinfo/"
#ఒకవేళ U_HAVE_DIRENT_H
#నిర్వచించు TZFILE_SKIP     "posixrules" /* tz file to skip when searching. */
/* Some Linux distributions have 'localtime' in /usr/share/zoneinfo
   symlinked to /etc/localtime, which makes searchForTZFile return
   'localtime' when it's the first match. */
#నిర్వచించు TZFILE_SKIP2    "localtime"
#నిర్వచించు శోధన_TZFILE
#అన్తర్భావ <dirent.h>  /* Needed to search through system timezone files */
#అయితే_ముగింపు
స్థిర అక్షరం g_సమయం_Zone_బఫరు[మార్గం_MAX];
స్థిర స్థిరరాశి అక్షరం *g_సమయం_Zone_బఫరు_Ptr = శూన్యసూచిక;
#అయితే_ముగింపు

#ఒకవేళ !U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
#నిర్వచించు isNonDigit(ch) (ch < '0' || '9' < ch)
#నిర్వచించు isDigit(ch) ('0' <= ch && ch <= '9')
స్థిర UBool isValidOlsonID(స్థిరరాశి అక్షరం *id) {
    int32_t idx = 0;
    int32_t idxMax = 0;

    /* Determine if this is something like Iceland (Olson ID)
    or AST4ADT (non-Olson ID) */
    అయితే (id[idx] && isNonDigit(id[idx]) && id[idx] != ',') {
        idx++;
    }

    /* Allow at maximum 2 numbers at the end of the id to support zone id's
    like GMT+11. */
    idxMax = idx + 2;
    అయితే (id[idx] && isDigit(id[idx]) && idx < idxMax) {
        idx++;
    }

    /* If we went through the whole string, then it might be okay.
    The timezone is sometimes set to "CST-7CDT", "CST6CDT5,J129,J131/19:30",
    "GRNLNDST3GRNLNDDT" or similar, so we cannot use it.
    The rest of the time it could be an Olson ID. George */
    తిరిగి id[idx] == 0
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
స్థిర శూన్య skipZoneIDPrefix(స్థిరరాశి అక్షరం** id) {
    ఒకవేళ (uprv_strncmp(*id, "posix/", 6) == 0
        || uprv_strncmp(*id, "right/", 6) == 0)
    {
        *id += 6;
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(U_TZNAME) && !U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API

#నిర్వచించు CONVERT_HOURS_గమ్యం_SECONDS(స్థానభ్రంశం) (int32_t)(స్థానభ్రంశం*3600)
రకనిర్వచనం నిర్మాణం స్థానభ్రంశం_Zone_Mapping {
    int32_t స్థానభ్రంశం_Seconds;
    int32_t daylightType; /* 0=U_DAYLIGHT_NONE, 1=daylight in June-U_DAYLIGHT_JUNE, 2=daylight in December=U_DAYLIGHT_DECEMBER*/
    స్థిరరాశి అక్షరం *stdID;
    స్థిరరాశి అక్షరం *dstID;
    స్థిరరాశి అక్షరం *olsonID;
} స్థానభ్రంశం_Zone_Mapping;

గణన { U_DAYLIGHT_NONE=0,U_DAYLIGHT_JUNE=1,U_DAYLIGHT_DECEMBER=2 };

/*
This list tries to disambiguate a set of abbreviated timezone IDs and offsets
and maps it to an Olson ID.
Before adding anything to this list, take a look at
icu/source/tools/tzcode/tz.alias
Sometimes no daylight savings (0) is important to define due to aliases.
This list can be tested with icu/source/test/compat/tzone.pl
More values could be added to daylightType to increase precision.
*/
స్థిర స్థిరరాశి నిర్మాణం స్థానభ్రంశం_Zone_Mapping స్థానభ్రంశం_ZONE_MAPPINGS[] = {
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

స్థిర స్థిరరాశి అక్షరం* remap_Short_సమయం_Zone(స్థిరరాశి అక్షరం *stdID, స్థిరరాశి అక్షరం *dstID, int32_t daylightType, int32_t స్థానభ్రంశం)
{
    int32_t idx;
#నిర్వచితమైతే DEBUG_TZNAME
    fprintf(stderr, "TZ=%s std=%s dst=%s daylight=%d offset=%d\n", getenv("TZ"), stdID, dstID, daylightType, స్థానభ్రంశం);
#అయితే_ముగింపు
    కొరకు (idx = 0; idx < UPRV_LENGTHOF(స్థానభ్రంశం_ZONE_MAPPINGS); idx++)
    {
        ఒకవేళ (స్థానభ్రంశం == స్థానభ్రంశం_ZONE_MAPPINGS[idx].స్థానభ్రంశం_Seconds
            && daylightType == స్థానభ్రంశం_ZONE_MAPPINGS[idx].daylightType
            && strcmp(స్థానభ్రంశం_ZONE_MAPPINGS[idx].stdID, stdID) == 0
            && strcmp(స్థానభ్రంశం_ZONE_MAPPINGS[idx].dstID, dstID) == 0)
        {
            తిరిగి స్థానభ్రంశం_ZONE_MAPPINGS[idx].olsonID;
        }
    }
    తిరిగి శూన్యసూచిక;
}
#అయితే_ముగింపు

#నిర్వచితమైతే శోధన_TZFILE
#నిర్వచించు MAX_READ_కొలత 512

రకనిర్వచనం నిర్మాణం DefaultTZInfo {
    అక్షరం* defaultTZBuffer;
    int64_t default_TZFile_కొలత;
    FILE* defaultTZFilePtr;
    int32_t defaultTZPosition;
} DefaultTZInfo;

/*
 * This method compares the two files given to see if they are a match.
 * It is currently use to compare two TZ files.
 */
స్థిర UBool compare_ద్విమాన_Files(స్థిరరాశి అక్షరం* TZFileName, DefaultTZInfo* tzInfo) {
    FILE* file;
    int64_t కొలత_File;
    int64_t కొలత_File_ఎడమ;
    int32_t కొలత_File_Read;
    int32_t కొలత_File_గమ్యం_Read;
    అక్షరం బఫరు_File[MAX_READ_కొలత];
    UBool ఫలితం = నిజం;

    U_ASSERT(tzInfo->defaultTZFilePtr != శూన్యసూచిక);
    file = fopen(TZFileName, "r");

    tzInfo->defaultTZPosition = 0; /* reset position to begin search */

    ఒకవేళ (file != శూన్యసూచిక) {
        /* First check that the file size are equal. */
        ఒకవేళ (tzInfo->default_TZFile_కొలత == 0) {
            fseek(tzInfo->defaultTZFilePtr, 0, SEEK_END);
            tzInfo->default_TZFile_కొలత = ftell(tzInfo->defaultTZFilePtr);
        }
        ఒకవేళ (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            తిరిగి అబద్ధం;
        }
        కొలత_File = ftell(file);
        ఒకవేళ (కొలత_File == -1) {
            fclose(file);
            తిరిగి అబద్ధం;
        }
        కొలత_File_ఎడమ = కొలత_File;

        ఒకవేళ (కొలత_File != tzInfo->default_TZFile_కొలత) {
            ఫలితం = అబద్ధం;
        } లేకపోతే {
            /* Store the data from the files in separate buffers and
             * compare each byte to determine equality.
             */
            ఒకవేళ (tzInfo->defaultTZBuffer == శూన్యసూచిక) {
                ఒకవేళ (fseek(tzInfo->defaultTZFilePtr, 0, SEEK_SET) != 0) {
                    fclose(file);
                    తిరిగి అబద్ధం;
                }
                tzInfo->defaultTZBuffer = static_cast<అక్షరం*>(uprv_malloc(పరిమాణం(అక్షరం) * tzInfo->default_TZFile_కొలత));
                కొలత_File_Read = fread(tzInfo->defaultTZBuffer, 1, tzInfo->default_TZFile_కొలత, tzInfo->defaultTZFilePtr);
            }
            ఒకవేళ (fseek(file, 0, SEEK_SET) != 0) {
                fclose(file);
                తిరిగి అబద్ధం;
            }
            అయితే(కొలత_File_ఎడమ > 0) {
                uprv_memset(బఫరు_File, 0, MAX_READ_కొలత);
                కొలత_File_గమ్యం_Read = కొలత_File_ఎడమ < MAX_READ_కొలత ? కొలత_File_ఎడమ : MAX_READ_కొలత;

                కొలత_File_Read = fread(బఫరు_File, 1, కొలత_File_గమ్యం_Read, file);
                ఒకవేళ (కొలత_File_Read == -1) {
                    fclose(file);
                    తిరిగి అబద్ధం;
                }
                ఒకవేళ (memcmp(tzInfo->defaultTZBuffer + tzInfo->defaultTZPosition, బఫరు_File, కొలత_File_Read) != 0) {
                    ఫలితం = అబద్ధం;
                    ఆపు;
                }
                కొలత_File_ఎడమ -= కొలత_File_Read;
                tzInfo->defaultTZPosition += కొలత_File_Read;
            }
        }
    } లేకపోతే {
        ఫలితం = అబద్ధం;
    }

    ఒకవేళ (file != శూన్యసూచిక) {
        fclose(file);
    }

    తిరిగి ఫలితం;
}


/* dirent also lists two entries: "." and ".." that we can safely ignore. */
#నిర్వచించు SKIP1 "."
#నిర్వచించు SKIP2 ".."
స్థిర UBool U_CALLCONV putil_cleanup();
స్థిర CharString *g_శోధన_TZFile_ఫలితం = శూన్యసూచిక;

/*
 * This method recursively traverses the directory given for a matching TZ file and returns the first match.
 * This function is not thread safe - it uses a global, gSearchTZFileResult, to hold its results.
 */
స్థిర అక్షరం* శోధన_For_TZFile(స్థిరరాశి అక్షరం* మార్గం, DefaultTZInfo* tzInfo) {
    DIR* dirp = శూన్యసూచిక;
    నిర్మాణం dirent* dir_నమోదు = శూన్యసూచిక;
    అక్షరం* ఫలితం = శూన్యసూచిక;
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;

    /* There is no reference file to compare against, so do not traverse the zoneinfo tree. */
    ఒకవేళ (tzInfo->defaultTZFilePtr == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }

    /* Save the current path */
    CharString curpath(మార్గం, -1, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        వెళ్ళు cleanupAndReturn;
    }

    dirp = opendir(మార్గం);
    ఒకవేళ (dirp == శూన్యసూచిక) {
        వెళ్ళు cleanupAndReturn;
    }

    ఒకవేళ (g_శోధన_TZFile_ఫలితం == శూన్యసూచిక) {
        g_శోధన_TZFile_ఫలితం = కొత్త CharString;
        ఒకవేళ (g_శోధన_TZFile_ఫలితం == శూన్యసూచిక) {
            వెళ్ళు cleanupAndReturn;
        }
        ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    }

    /* Check each entry in the directory. */
    అయితే((dir_నమోదు = readdir(dirp)) != శూన్యసూచిక) {
        స్థిరరాశి అక్షరం* dirName = dir_నమోదు->d_name;
        ఒకవేళ (uprv_strcmp(dirName, SKIP1) != 0 && uprv_strcmp(dirName, SKIP2) != 0
            && uprv_strcmp(TZFILE_SKIP, dirName) != 0 && uprv_strcmp(TZFILE_SKIP2, dirName) != 0) {
            /* Create a newpath with the new entry to test each entry in the directory. */
            CharString newpath(curpath, స్థితి_2);
            newpath.append(dirName, -1, స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) {
                ఆపు;
            }

            DIR* subDirp = శూన్యసూచిక;
            ఒకవేళ ((subDirp = opendir(newpath.దత్తాంశం())) != శూన్యసూచిక) {
                /* If this new path is a directory, make a recursive call with the newpath. */
                closedir(subDirp);
                newpath.append('/', స్థితి_2);
                ఒకవేళ (U_FAILURE(స్థితి_2)) {
                    ఆపు;
                }
                ఫలితం = శోధన_For_TZFile(newpath.దత్తాంశం(), tzInfo);
                /*
                 Have to get out here. Otherwise, we'd keep looking
                 and return the first match in the top-level directory
                 if there's a match in the top-level. If not, this function
                 would return nullptr and set gTimeZoneBufferPtr to nullptr in initDefault().
                 It worked without this in most cases because we have a fallback of calling
                 localtime_r to figure out the default timezone.
                */
                ఒకవేళ (ఫలితం != శూన్యసూచిక)
                    ఆపు;
            } లేకపోతే {
                ఒకవేళ(compare_ద్విమాన_Files(newpath.దత్తాంశం(), tzInfo)) {
                    int32_t amount_గమ్యం_Skip = పరిమాణం(TZZONEINFO) - 1;
                    ఒకవేళ (amount_గమ్యం_Skip > newpath.length()) {
                        amount_గమ్యం_Skip = newpath.length();
                    }
                    స్థిరరాశి అక్షరం* zoneid = newpath.దత్తాంశం() + amount_గమ్యం_Skip;
                    skipZoneIDPrefix(&zoneid);
                    g_శోధన_TZFile_ఫలితం->clear();
                    g_శోధన_TZFile_ఫలితం->append(zoneid, -1, స్థితి_2);
                    ఒకవేళ (U_FAILURE(స్థితి_2)) {
                        ఆపు;
                    }
                    ఫలితం = g_శోధన_TZFile_ఫలితం->దత్తాంశం();
                    /* Get out after the first one found. */
                    ఆపు;
                }
            }
        }
    }

  cleanupAndReturn:
    ఒకవేళ (dirp) {
        closedir(dirp);
    }
    తిరిగి ఫలితం;
}
#అయితే_ముగింపు

#ఒకవేళ U_PLATFORM == U_PF_ANDROID
రకనిర్వచనం పూర్ణాంకం(system_property_read_callback)(స్థిరరాశి prop_info* info,
                                           శూన్య (*callback)(శూన్య* cookie,
                                                            స్థిరరాశి అక్షరం* name,
                                                            స్థిరరాశి అక్షరం* value,
                                                            uint32_t serial),
                                           శూన్య* cookie);
రకనిర్వచనం పూర్ణాంకం(system_property_get)(స్థిరరాశి అక్షరం*, అక్షరం*);

స్థిర అక్షరం g_Android_సమయం_Zone[PROP_విలువ_MAX] = { '\0' };

స్థిర శూన్య u_property_read(శూన్య* cookie, స్థిరరాశి అక్షరం* name, స్థిరరాశి అక్షరం* value,
                            uint32_t serial) {
    uprv_strcpy((అక్షరం* )cookie, value);
}
#అయితే_ముగింపు

U_CAPI శూన్య U_EXPORT2
uprv_tzname_clear_cache()
{
#ఒకవేళ U_PLATFORM == U_PF_ANDROID
    /* Android's timezone is stored in system property. */
    g_Android_సమయం_Zone[0] = '\0';
    శూన్య* libc = dlopen("libc.so", RTLD_NOLOAD);
    ఒకవేళ (libc) {
        /* Android API 26+ has new API to get system property and old API
         * (__system_property_get) is deprecated */
        system_property_read_callback* property_read_callback =
            (system_property_read_callback*)dlsym(
                libc, "__system_property_read_callback");
        ఒకవేళ (property_read_callback) {
            స్థిరరాశి prop_info* info =
                __system_property_find("persist.sys.timezone");
            ఒకవేళ (info) {
                property_read_callback(info, &u_property_read, g_Android_సమయం_Zone);
            }
        } లేకపోతే {
            system_property_get* property_get =
                (system_property_get*)dlsym(libc, "__system_property_get");
            ఒకవేళ (property_get) {
                property_get("persist.sys.timezone", g_Android_సమయం_Zone);
            }
        }
        dlclose(libc);
    }
#అయితే_ముగింపు

#ఒకవేళ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    g_సమయం_Zone_బఫరు_Ptr = శూన్యసూచిక;
#అయితే_ముగింపు
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uprv_tzname(పూర్ణాంకం n)
{
    (శూన్య)n; // Avoid unreferenced parameter warning.
    స్థిరరాశి అక్షరం *tzid = శూన్యసూచిక;
#ఒకవేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
    tzid = uprv_detect_Windows_సమయం_Zone();

    ఒకవేళ (tzid != శూన్యసూచిక) {
        తిరిగి tzid;
    }

#నిర్వచితం_కాకపోతే U_TZNAME
    // The return value is free'd in timezone.cpp on Windows because
    // the other code path returns a pointer to a heap location.
    // If we don't have a name already, then tzname wouldn't be any
    // better, so just fall back.
    తిరిగి uprv_strdup("");
#అయితే_ముగింపు // !U_TZNAME

#లేకపోతే

/*#if U_PLATFORM_IS_DARWIN_BASED
    int ret;

    tzid = getenv("TZFILE");
    if (tzid != nullptr) {
        return tzid;
    }
#endif*/

/* This code can be temporarily disabled to test tzname resolution later on. */
#నిర్వచితం_కాకపోతే DEBUG_TZNAME
#ఒకవేళ U_PLATFORM == U_PF_ANDROID
    tzid = g_Android_సమయం_Zone;
#లేకపోతే
    tzid = getenv("TZ");
#అయితే_ముగింపు
    ఒకవేళ (tzid != శూన్యసూచిక && isValidOlsonID(tzid)
#ఒకవేళ U_PLATFORM == U_PF_SOLARIS
    /* Don't misinterpret TZ "localtime" on Solaris as a time zone name. */
        && uprv_strcmp(tzid, TZ_ENV_CHECK) != 0
#అయితే_ముగింపు
    ) {
        /* The colon forces tzset() to treat the remainder as zoneinfo path */
        ఒకవేళ (tzid[0] == ':') {
            tzid++;
        }
        /* This might be a good Olson ID. */
        skipZoneIDPrefix(&tzid);
        తిరిగి tzid;
    }
    /* else U_TZNAME will give a better result. */
#అయితే_ముగింపు

#ఒకవేళ defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    /* Caller must handle threading issues */
    ఒకవేళ (g_సమయం_Zone_బఫరు_Ptr == శూన్యసూచిక) {
        /*
        This is a trick to look at the name of the link to get the Olson ID
        because the tzfile contents is underspecified.
        This isn't guaranteed to work because it may not be a symlink.
        */
        అక్షరం *ret = realpath(TZDEFAULT, g_సమయం_Zone_బఫరు);
        ఒకవేళ (ret != శూన్యసూచిక && uprv_strcmp(TZDEFAULT, g_సమయం_Zone_బఫరు) != 0) {
            int32_t tz_Zone_Info_తోక_Len = uprv_strlen(TZZONEINFOTAIL);
            స్థిరరాశి అక్షరం *tz_Zone_Info_తోక_Ptr = uprv_strstr(g_సమయం_Zone_బఫరు, TZZONEINFOTAIL);
            // MacOS14 has the realpath as something like
            // /usr/share/zoneinfo.default/Australia/Melbourne
            // which will not have "/zoneinfo/" in the path.
            // Therefore if we fail, we fall back to read the link which is
            // /var/db/timezone/zoneinfo/Australia/Melbourne
            // We also fall back to reading the link if the realpath leads to something like
            // /usr/share/zoneinfo/posixrules
            ఒకవేళ (tz_Zone_Info_తోక_Ptr == శూన్యసూచిక ||
                    uprv_strcmp(tz_Zone_Info_తోక_Ptr + tz_Zone_Info_తోక_Len, "posixrules") == 0) {
                ssize_t size = readlink(TZDEFAULT, g_సమయం_Zone_బఫరు, పరిమాణం(g_సమయం_Zone_బఫరు)-1);
                ఒకవేళ (size > 0) {
                    g_సమయం_Zone_బఫరు[size] = 0;
                    tz_Zone_Info_తోక_Ptr = uprv_strstr(g_సమయం_Zone_బఫరు, TZZONEINFOTAIL);
                }
            }
            ఒకవేళ (tz_Zone_Info_తోక_Ptr != శూన్యసూచిక) {
                tz_Zone_Info_తోక_Ptr += tz_Zone_Info_తోక_Len;
                skipZoneIDPrefix(&tz_Zone_Info_తోక_Ptr);
                ఒకవేళ (isValidOlsonID(tz_Zone_Info_తోక_Ptr)) {
                    తిరిగి (g_సమయం_Zone_బఫరు_Ptr = tz_Zone_Info_తోక_Ptr);
                }
            }
        } లేకపోతే {
#ఒకవేళ defined(శోధన_TZFILE)
            DefaultTZInfo* tzInfo = (DefaultTZInfo*)uprv_malloc(పరిమాణం(DefaultTZInfo));
            ఒకవేళ (tzInfo != శూన్యసూచిక) {
                tzInfo->defaultTZBuffer = శూన్యసూచిక;
                tzInfo->default_TZFile_కొలత = 0;
                tzInfo->defaultTZFilePtr = fopen(TZDEFAULT, "r");
                tzInfo->defaultTZPosition = 0;

                g_సమయం_Zone_బఫరు_Ptr = శోధన_For_TZFile(TZZONEINFO, tzInfo);

                /* Free previously allocated memory */
                ఒకవేళ (tzInfo->defaultTZBuffer != శూన్యసూచిక) {
                    uprv_free(tzInfo->defaultTZBuffer);
                }
                ఒకవేళ (tzInfo->defaultTZFilePtr != శూన్యసూచిక) {
                    fclose(tzInfo->defaultTZFilePtr);
                }
                uprv_free(tzInfo);
            }

            ఒకవేళ (g_సమయం_Zone_బఫరు_Ptr != శూన్యసూచిక && isValidOlsonID(g_సమయం_Zone_బఫరు_Ptr)) {
                తిరిగి g_సమయం_Zone_బఫరు_Ptr;
            }
#అయితే_ముగింపు
        }
    }
    లేకపోతే {
        తిరిగి g_సమయం_Zone_బఫరు_Ptr;
    }
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితమైతే U_TZNAME
#ఒకవేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
    /* The return value is free'd in timezone.cpp on Windows because
     * the other code path returns a pointer to a heap location. */
    తిరిగి uprv_strdup(U_TZNAME[n]);
#లేకపోతే
    /*
    U_TZNAME is usually a non-unique abbreviation, which isn't normally usable.
    So we remap the abbreviation to an olson ID.

    Since Windows exposes a little more timezone information,
    we normally don't use this code on Windows because
    uprv_detectWindowsTimeZone should have already given the correct answer.
    */
    {
        నిర్మాణం tm juneSol, decemberSol;
        పూర్ణాంకం daylightType;
        స్థిర స్థిరరాశి time_t juneSolstice=1182478260; /*2007-06-21 18:11 UT*/
        స్థిర స్థిరరాశి time_t decemberSolstice=1198332540; /*2007-12-22 06:09 UT*/

        /* This probing will tell us when daylight savings occurs.  */
        localtime_r(&juneSolstice, &juneSol);
        localtime_r(&decemberSolstice, &decemberSol);
        ఒకవేళ(decemberSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_DECEMBER;
        } లేకపోతే ఒకవేళ(juneSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_JUNE;
        } లేకపోతే {
          daylightType = U_DAYLIGHT_NONE;
        }
        tzid = remap_Short_సమయం_Zone(U_TZNAME[0], U_TZNAME[1], daylightType, uprv_timezone());
        ఒకవేళ (tzid != శూన్యసూచిక) {
            తిరిగి tzid;
        }
    }
    తిరిగి U_TZNAME[n];
#అయితే_ముగింపు
#లేకపోతే
    తిరిగి "";
#అయితే_ముగింపు
}

/* Get and set the ICU data directory --------------------------------------- */

స్థిర icu::UInitOnce g_దత్తాంశం_Dir_Init_Once {};
స్థిర అక్షరం *g_దత్తాంశం_Directory = శూన్యసూచిక;

UInitOnce g_సమయం_Zone_Files_Init_Once {};
స్థిర CharString *g_సమయం_Zone_Files_Directory = శూన్యసూచిక;

#ఒకవేళ U_POSIX_LOCALE || U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
 స్థిర స్థిరరాశి అక్షరం *gCorrectedPOSIXLocale = శూన్యసూచిక; /* Sometimes heap allocated */
 స్థిర తార్కిక g_Corrected_POSIXLocale_కుప్ప_Allocated = అబద్ధం;
#అయితే_ముగింపు

స్థిర UBool U_CALLCONV putil_cleanup()
{
    ఒకవేళ (g_దత్తాంశం_Directory && *g_దత్తాంశం_Directory) {
        uprv_free(g_దత్తాంశం_Directory);
    }
    g_దత్తాంశం_Directory = శూన్యసూచిక;
    g_దత్తాంశం_Dir_Init_Once.reset();

    తొలగించు g_సమయం_Zone_Files_Directory;
    g_సమయం_Zone_Files_Directory = శూన్యసూచిక;
    g_సమయం_Zone_Files_Init_Once.reset();

#నిర్వచితమైతే శోధన_TZFILE
    తొలగించు g_శోధన_TZFile_ఫలితం;
    g_శోధన_TZFile_ఫలితం = శూన్యసూచిక;
#అయితే_ముగింపు

#ఒకవేళ U_POSIX_LOCALE || U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
    ఒకవేళ (gCorrectedPOSIXLocale && g_Corrected_POSIXLocale_కుప్ప_Allocated) {
        uprv_free(const_cast<అక్షరం *>(gCorrectedPOSIXLocale));
        gCorrectedPOSIXLocale = శూన్యసూచిక;
        g_Corrected_POSIXLocale_కుప్ప_Allocated = అబద్ధం;
    }
#అయితే_ముగింపు
    తిరిగి నిజం;
}

/*
 * Set the data directory.
 *    Make a copy of the passed string, and set the global data dir to point to it.
 */
U_CAPI శూన్య U_EXPORT2
u_సమితి_దత్తాంశం_Directory(స్థిరరాశి అక్షరం *directory) {
    అక్షరం *new_దత్తాంశం_Dir;
    int32_t length;

    ఒకవేళ(directory==శూన్యసూచిక || *directory==0) {
        /* A small optimization to prevent the malloc and copy when the
        shared library is used, and this is a way to make sure that nullptr
        is never returned.
        */
        new_దత్తాంశం_Dir = (అక్షరం *)"";
    }
    లేకపోతే {
        length=(int32_t)uprv_strlen(directory);
        new_దత్తాంశం_Dir = (అక్షరం *)uprv_malloc(length + 2);
        /* Exit out if newDataDir could not be created. */
        ఒకవేళ (new_దత్తాంశం_Dir == శూన్యసూచిక) {
            తిరిగి;
        }
        uprv_strcpy(new_దత్తాంశం_Dir, directory);

#ఒకవేళ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
        {
            అక్షరం *p;
            అయితే((p = uprv_strchr(new_దత్తాంశం_Dir, U_FILE_ALT_SEP_CHAR)) != శూన్యసూచిక) {
                *p = U_FILE_SEP_CHAR;
            }
        }
#అయితే_ముగింపు
    }

    ఒకవేళ (g_దత్తాంశం_Directory && *g_దత్తాంశం_Directory) {
        uprv_free(g_దత్తాంశం_Directory);
    }
    g_దత్తాంశం_Directory = new_దత్తాంశం_Dir;
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
}

U_CAPI UBool U_EXPORT2
uprv_మార్గం_Is_Absolute(స్థిరరాశి అక్షరం *మార్గం)
{
  ఒకవేళ(!మార్గం || !*మార్గం) {
    తిరిగి అబద్ధం;
  }

  ఒకవేళ(*మార్గం == U_FILE_SEP_CHAR) {
    తిరిగి నిజం;
  }

#ఒకవేళ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
  ఒకవేళ(*మార్గం == U_FILE_ALT_SEP_CHAR) {
    తిరిగి నిజం;
  }
#అయితే_ముగింపు

#ఒకవేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
  ఒకవేళ( (((మార్గం[0] >= 'A') && (మార్గం[0] <= 'Z')) ||
       ((మార్గం[0] >= 'a') && (మార్గం[0] <= 'z'))) &&
      మార్గం[1] == ':' ) {
    తిరిగి నిజం;
  }
#అయితే_ముగింపు

  తిరిగి అబద్ధం;
}

/* Backup setting of ICU_DATA_DIR_PREFIX_ENV_VAR
   (needed for some Darwin ICU build environments) */
#ఒకవేళ U_PLATFORM_IS_DARWIN_BASED && defined(లక్ష్యం_OS_SIMULATOR) && లక్ష్యం_OS_SIMULATOR
# ఒకవేళ !defined(ICU_దత్తాంశం_DIR_ఉపసర్గ_ENV_VAR)
#  define ICU_దత్తాంశం_DIR_ఉపసర్గ_ENV_VAR "IPHONE_SIMULATOR_ROOT"
# endif
#అయితే_ముగింపు

#ఒకవేళ defined(ICU_దత్తాంశం_DIR_WINDOWS)
// Helper function to get the ICU Data Directory under the Windows directory location.
స్థిర BOOL U_CALLCONV get_Icu_దత్తాంశం_Directory_Under_Windows_Directory(అక్షరం* directory_బఫరు, UINT బఫరు_నిడివి)
{
    wchar_t windows_మార్గం[MAX_మార్గం];
    అక్షరం windows_మార్గం_Utf8[MAX_మార్గం];

    UINT length = GetSystemWindowsDirectoryW(windows_మార్గం, UPRV_LENGTHOF(windows_మార్గం));
    ఒకవేళ ((length > 0) && (length < (UPRV_LENGTHOF(windows_మార్గం) - 1))) {
        // Convert UTF-16 to a UTF-8 string.
        UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
        int32_t windows_మార్గం_Utf8_Len = 0;
        u_str_గమ్యం_UTF8(windows_మార్గం_Utf8, static_cast<int32_t>(UPRV_LENGTHOF(windows_మార్గం_Utf8)),
            &windows_మార్గం_Utf8_Len, reinterpret_cast<స్థిరరాశి char16_t*>(windows_మార్గం), -1, &స్థితి_2);

        ఒకవేళ (U_SUCCESS(స్థితి_2) && (స్థితి_2 != U_STRING_NOT_TERMINATED_WARNING) &&
            (windows_మార్గం_Utf8_Len < (UPRV_LENGTHOF(windows_మార్గం_Utf8) - 1))) {
            // Ensure it always has a separator, so we can append the ICU data path.
            ఒకవేళ (windows_మార్గం_Utf8[windows_మార్గం_Utf8_Len - 1] != U_FILE_SEP_CHAR) {
                windows_మార్గం_Utf8[windows_మార్గం_Utf8_Len++] = U_FILE_SEP_CHAR;
                windows_మార్గం_Utf8[windows_మార్గం_Utf8_Len] = '\0';
            }
            // Check if the concatenated string will fit.
            ఒకవేళ ((windows_మార్గం_Utf8_Len + UPRV_LENGTHOF(ICU_దత్తాంశం_DIR_WINDOWS)) < బఫరు_నిడివి) {
                uprv_strcpy(directory_బఫరు, windows_మార్గం_Utf8);
                uprv_strcat(directory_బఫరు, ICU_దత్తాంశం_DIR_WINDOWS);
                తిరిగి నిజం;
            }
        }
    }

    తిరిగి అబద్ధం;
}
#అయితే_ముగింపు

స్థిర శూన్య U_CALLCONV దత్తాంశం_Directory_Init_Fn() {
    /* If we already have the directory, then return immediately. Will happen if user called
     * u_setDataDirectory().
     */
    ఒకవేళ (g_దత్తాంశం_Directory) {
        తిరిగి;
    }

    స్థిరరాశి అక్షరం *మార్గం = శూన్యసూచిక;

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
#   ఒకవేళ !defined(ICU_NO_USER_దత్తాంశం_OVERRIDE) && !UCONFIG_NO_FILE_IO
    /* First try to get the environment variable */
#     ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 0  // Windows UWP does not support getenv
        మార్గం=getenv("ICU_DATA");
#     endif
#   endif

    /* ICU_DATA_DIR may be set as a compile option.
     * U_ICU_DATA_DEFAULT_DIR is provided and is set by ICU at compile time
     * and is used only when data is built in archive mode eliminating the need
     * for ICU_DATA_DIR to be set. U_ICU_DATA_DEFAULT_DIR is set to the installation
     * directory of the data dat file. Users should use ICU_DATA_DIR if they want to
     * set their own path.
     */
#ఒకవేళ defined(ICU_దత్తాంశం_DIR) || defined(U_ICU_దత్తాంశం_DEFAULT_DIR)
# ఒకవేళ defined(ICU_దత్తాంశం_DIR_ఉపసర్గ_ENV_VAR)
    అక్షరం datadir_మార్గం_బఫరు[మార్గం_MAX];
# endif
    ఒకవేళ(మార్గం==శూన్యసూచిక || *మార్గం==0) {
# ఒకవేళ defined(ICU_దత్తాంశం_DIR_ఉపసర్గ_ENV_VAR)
        స్థిరరాశి అక్షరం *ఉపసర్గ = getenv(ICU_దత్తాంశం_DIR_ఉపసర్గ_ENV_VAR);
# endif
# ifdef ICU_దత్తాంశం_DIR
        మార్గం=ICU_దత్తాంశం_DIR;
# లేకపోతే
        మార్గం=U_ICU_దత్తాంశం_DEFAULT_DIR;
# endif
# ఒకవేళ defined(ICU_దత్తాంశం_DIR_ఉపసర్గ_ENV_VAR)
        ఒకవేళ (ఉపసర్గ != శూన్యసూచిక) {
            snprintf(datadir_మార్గం_బఫరు, పరిమాణం(datadir_మార్గం_బఫరు), "%s%s", ఉపసర్గ, మార్గం);
            మార్గం=datadir_మార్గం_బఫరు;
        }
# endif
    }
#అయితే_ముగింపు

#ఒకవేళ defined(ICU_దత్తాంశం_DIR_WINDOWS)
    అక్షరం datadir_మార్గం_బఫరు[MAX_మార్గం];
    ఒకవేళ (get_Icu_దత్తాంశం_Directory_Under_Windows_Directory(datadir_మార్గం_బఫరు, UPRV_LENGTHOF(datadir_మార్గం_బఫరు))) {
        మార్గం = datadir_మార్గం_బఫరు;
    }
#అయితే_ముగింపు

    ఒకవేళ(మార్గం==శూన్యసూచిక) {
        /* It looks really bad, set it to something. */
        మార్గం = "";
    }

    u_సమితి_దత్తాంశం_Directory(మార్గం);
}

U_CAPI స్థిరరాశి అక్షరం * U_EXPORT2
u_get_దత్తాంశం_Directory() {
    umtx_initOnce(g_దత్తాంశం_Dir_Init_Once, &దత్తాంశం_Directory_Init_Fn);
    తిరిగి g_దత్తాంశం_Directory;
}

స్థిర శూన్య సమితి_సమయం_Zone_Files_Dir(స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    g_సమయం_Zone_Files_Directory->clear();
    g_సమయం_Zone_Files_Directory->append(మార్గం, స్థితి_2);
#ఒకవేళ (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    అక్షరం *p = g_సమయం_Zone_Files_Directory->దత్తాంశం();
    అయితే ((p = uprv_strchr(p, U_FILE_ALT_SEP_CHAR)) != శూన్యసూచిక) {
        *p = U_FILE_SEP_CHAR;
    }
#అయితే_ముగింపు
}

#నిర్వచించు గమ్యం_STRING(x) గమ్యం_STRING_2(x)
#నిర్వచించు గమ్యం_STRING_2(x) #x

స్థిర శూన్య U_CALLCONV సమయం_Zone_దత్తాంశం_Dir_Init_Fn(UError_సంకేతం &స్థితి_2) {
    U_ASSERT(g_సమయం_Zone_Files_Directory == శూన్యసూచిక);
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    g_సమయం_Zone_Files_Directory = కొత్త CharString();
    ఒకవేళ (g_సమయం_Zone_Files_Directory == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    స్థిరరాశి అక్షరం *dir = "";

#ఒకవేళ defined(ICU_TIMEZONE_FILES_DIR_ఉపసర్గ_ENV_VAR)
    అక్షరం timezonefilesdir_మార్గం_బఫరు[మార్గం_MAX];
    స్థిరరాశి అక్షరం *ఉపసర్గ = getenv(ICU_TIMEZONE_FILES_DIR_ఉపసర్గ_ENV_VAR);
#అయితే_ముగింపు

#ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 1
// The UWP version does not support the environment variable setting.

# ఒకవేళ defined(ICU_దత్తాంశం_DIR_WINDOWS)
    // When using the Windows system data, we can possibly pick up time zone data from the Windows directory.
    అక్షరం datadir_మార్గం_బఫరు[MAX_మార్గం];
    ఒకవేళ (get_Icu_దత్తాంశం_Directory_Under_Windows_Directory(datadir_మార్గం_బఫరు, UPRV_LENGTHOF(datadir_మార్గం_బఫరు))) {
        dir = datadir_మార్గం_బఫరు;
    }
# endif

#లేకపోతే
    dir = getenv("ICU_TIMEZONE_FILES_DIR");
#అయితే_ముగింపు // U_PLATFORM_HAS_WINUWP_API

#ఒకవేళ defined(U_TIMEZONE_FILES_DIR)
    ఒకవేళ (dir == శూన్యసూచిక) {
        // Build time configuration setting.
        dir = గమ్యం_STRING(U_TIMEZONE_FILES_DIR);
    }
#అయితే_ముగింపు

    ఒకవేళ (dir == శూన్యసూచిక) {
        dir = "";
    }

#ఒకవేళ defined(ICU_TIMEZONE_FILES_DIR_ఉపసర్గ_ENV_VAR)
    ఒకవేళ (ఉపసర్గ != శూన్యసూచిక) {
        snprintf(timezonefilesdir_మార్గం_బఫరు, పరిమాణం(timezonefilesdir_మార్గం_బఫరు), "%s%s", ఉపసర్గ, dir);
        dir = timezonefilesdir_మార్గం_బఫరు;
    }
#అయితే_ముగింపు

    సమితి_సమయం_Zone_Files_Dir(dir, స్థితి_2);
}


U_CAPI స్థిరరాశి అక్షరం * U_EXPORT2
u_get_సమయం_Zone_Files_Directory(UError_సంకేతం *స్థితి_2) {
    umtx_initOnce(g_సమయం_Zone_Files_Init_Once, &సమయం_Zone_దత్తాంశం_Dir_Init_Fn, *స్థితి_2);
    తిరిగి U_SUCCESS(*స్థితి_2) ? g_సమయం_Zone_Files_Directory->దత్తాంశం() : "";
}

U_CAPI శూన్య U_EXPORT2
u_సమితి_సమయం_Zone_Files_Directory(స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం *స్థితి_2) {
    umtx_initOnce(g_సమయం_Zone_Files_Init_Once, &సమయం_Zone_దత్తాంశం_Dir_Init_Fn, *స్థితి_2);
    సమితి_సమయం_Zone_Files_Dir(మార్గం, *స్థితి_2);

    // Note: this function does some extra churn, first setting based on the
    //       environment, then immediately replacing with the value passed in.
    //       The logic is simpler that way, and performance shouldn't be an issue.
}


#ఒకవేళ U_POSIX_LOCALE
/* A helper function used by uprv_getPOSIXIDForDefaultLocale and
 * uprv_getPOSIXIDForDefaultCodepage. Returns the posix locale id for
 * LC_CTYPE and LC_MESSAGES. It doesn't support other locale categories.
 */
స్థిర స్థిరరాశి అక్షరం *uprv_getPOSIXIDForCategory(పూర్ణాంకం category)
{
    స్థిరరాశి అక్షరం* posixID = శూన్యసూచిక;
    ఒకవేళ (category == LC_MESSAGES || category == LC_CTYPE) {
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
        posixID = setlocale(category, శూన్యసూచిక);
        ఒకవేళ ((posixID == శూన్యసూచిక)
            || (uprv_strcmp("C", posixID) == 0)
            || (uprv_strcmp("POSIX", posixID) == 0))
        {
            /* Maybe we got some garbage.  Try something more reasonable */
            posixID = getenv("LC_ALL");
            /* Solaris speaks POSIX -  See IEEE Std 1003.1-2008
             * This is needed to properly handle empty env. variables
             */
#ఒకవేళ U_PLATFORM == U_PF_SOLARIS
            ఒకవేళ ((posixID == 0) || (posixID[0] == '\0')) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                ఒకవేళ ((posixID == 0) || (posixID[0] == '\0')) {
#లేకపోతే
            ఒకవేళ (posixID == శూన్యసూచిక) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                ఒకవేళ (posixID == శూన్యసూచిక) {
#అయితే_ముగింపు
                    posixID = getenv("LANG");
                }
            }
        }
    }
    ఒకవేళ ((posixID == శూన్యసూచిక)
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
    తిరిగి posixID;
}

/* Return just the POSIX id for the default locale, whatever happens to be in
 * it. It gets the value from LC_MESSAGES and indirectly from LC_ALL and LANG.
 */
స్థిర స్థిరరాశి అక్షరం *uprv_getPOSIXIDForDefaultLocale()
{
    స్థిర స్థిరరాశి అక్షరం* posixID = శూన్యసూచిక;
    ఒకవేళ (posixID == శూన్యసూచిక) {
        posixID = uprv_getPOSIXIDForCategory(LC_MESSAGES);
    }
    తిరిగి posixID;
}

#ఒకవేళ !U_CHARSET_IS_UTF8
/* Return just the POSIX id for the default codepage, whatever happens to be in
 * it. It gets the value from LC_CTYPE and indirectly from LC_ALL and LANG.
 */
స్థిర స్థిరరాశి అక్షరం *uprv_getPOSIXIDForDefaultCodepage()
{
    స్థిర స్థిరరాశి అక్షరం* posixID = శూన్యసూచిక;
    ఒకవేళ (posixID == 0) {
        posixID = uprv_getPOSIXIDForCategory(LC_CTYPE);
    }
    తిరిగి posixID;
}
#అయితే_ముగింపు
#అయితే_ముగింపు

/* NOTE: The caller should handle thread safety */
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uprv_getDefaultLocaleID()
{
#ఒకవేళ U_POSIX_LOCALE
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
    స్థిరరాశి అక్షరం* posixID = uprv_getPOSIXIDForDefaultLocale();

    /* Format: (no spaces)
    ll [ _CC ] [ . MM ] [ @ VV]

      l = lang, C = ctry, M = charmap, V = variant
    */

    ఒకవేళ (gCorrectedPOSIXLocale != శూన్యసూచిక) {
        తిరిగి gCorrectedPOSIXLocale;
    }

    // Copy the ID into owned memory.
    // Over-allocate in case we replace "C" with "en_US_POSIX" (+10), + null termination
    అక్షరం *correctedPOSIXLocale = static_cast<అక్షరం *>(uprv_malloc(uprv_strlen(posixID) + 10 + 1));
    ఒకవేళ (correctedPOSIXLocale == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    uprv_strcpy(correctedPOSIXLocale, posixID);

    అక్షరం *హద్దు;
    ఒకవేళ ((హద్దు = uprv_strchr(correctedPOSIXLocale, '.')) != శూన్యసూచిక) {
        *హద్దు = 0;
    }
    ఒకవేళ ((హద్దు = uprv_strchr(correctedPOSIXLocale, '@')) != శూన్యసూచిక) {
        *హద్దు = 0;
    }

    ఒకవేళ ((uprv_strcmp("C", correctedPOSIXLocale) == 0) // no @ variant
        || (uprv_strcmp("POSIX", correctedPOSIXLocale) == 0)) {
      // Raw input was C.* or POSIX.*, Give it a nice POSIX default value.
      // (The "C"/"POSIX" case is handled in uprv_getPOSIXIDForCategory())
      uprv_strcpy(correctedPOSIXLocale, "en_US_POSIX");
    }

    /* Note that we scan the *uncorrected* ID. */
    స్థిరరాశి అక్షరం *p;
    ఒకవేళ ((p = uprv_strrchr(posixID, '@')) != శూన్యసూచిక) {
        p++;

        /* Take care of any special cases here.. */
        ఒకవేళ (!uprv_strcmp(p, "nynorsk")) {
            p = "NY";
            /* Don't worry about no__NY. In practice, it won't appear. */
        }

        ఒకవేళ (uprv_strchr(correctedPOSIXLocale,'_') == శూన్యసూచిక) {
            uprv_strcat(correctedPOSIXLocale, "__"); /* aa@b -> aa__b (note this can make the new locale 1 char longer) */
        }
        లేకపోతే {
            uprv_strcat(correctedPOSIXLocale, "_"); /* aa_CC@b -> aa_CC_b */
        }

        స్థిరరాశి అక్షరం *q;
        ఒకవేళ ((q = uprv_strchr(p, '.')) != శూన్యసూచిక) {
            /* How big will the resulting string be? */
            int32_t len = (int32_t)(uprv_strlen(correctedPOSIXLocale) + (q-p));
            uprv_strncat(correctedPOSIXLocale, p, q-p); // do not include charset
            correctedPOSIXLocale[len] = 0;
        }
        లేకపోతే {
            /* Anything following the @ sign */
            uprv_strcat(correctedPOSIXLocale, p);
        }

        /* Should there be a map from 'no@nynorsk' -> no_NO_NY here?
         * How about 'russian' -> 'ru'?
         * Many of the other locales using ISO codes will be handled by the
         * canonicalization functions in uloc_getDefault.
         */
    }

    ఒకవేళ (gCorrectedPOSIXLocale == శూన్యసూచిక) {
        gCorrectedPOSIXLocale = correctedPOSIXLocale;
        g_Corrected_POSIXLocale_కుప్ప_Allocated = నిజం;
        ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
        correctedPOSIXLocale = శూన్యసూచిక;
    }
    posixID = gCorrectedPOSIXLocale;

    ఒకవేళ (correctedPOSIXLocale != శూన్యసూచిక) {  /* Was already set - clean up. */
        uprv_free(correctedPOSIXLocale);
    }

    తిరిగి posixID;

#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
#నిర్వచించు POSIX_LOCALE_సామర్థ్యం 64
    UError_సంకేతం స్థితి_2 = U_ZERO_ERROR;
    అక్షరం *correctedPOSIXLocale = శూన్యసూచిక;

    // If we have already figured this out just use the cached value
    ఒకవేళ (gCorrectedPOSIXLocale != శూన్యసూచిక) {
        తిరిగి gCorrectedPOSIXLocale;
    }

    // No cached value, need to determine the current value
    స్థిర WCHAR windowsLocale[LOCALE_NAME_MAX_నిడివి] = {};
    పూర్ణాంకం length = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, windowsLocale, LOCALE_NAME_MAX_నిడివి);

    // Now we should have a Windows locale name that needs converted to the POSIX style.
    ఒకవేళ (length > 0) // If length is 0, then the GetLocaleInfoEx failed.
    {
        // First we need to go from UTF-16 to char (and also convert from _ to - while we're at it.)
        అక్షరం modifiedWindowsLocale[LOCALE_NAME_MAX_నిడివి] = {};

        int32_t i;
        కొరకు (i = 0; i < UPRV_LENGTHOF(modifiedWindowsLocale); i++)
        {
            ఒకవేళ (windowsLocale[i] == '_')
            {
                modifiedWindowsLocale[i] = '-';
            }
            లేకపోతే
            {
                modifiedWindowsLocale[i] = static_cast<అక్షరం>(windowsLocale[i]);
            }

            ఒకవేళ (modifiedWindowsLocale[i] == '\0')
            {
                ఆపు;
            }
        }

        ఒకవేళ (i >= UPRV_LENGTHOF(modifiedWindowsLocale))
        {
            // Ran out of room, can't really happen, maybe we'll be lucky about a matching
            // locale when tags are dropped
            modifiedWindowsLocale[UPRV_LENGTHOF(modifiedWindowsLocale) - 1] = '\0';
        }

        // Now normalize the resulting name
        correctedPOSIXLocale = static_cast<అక్షరం *>(uprv_malloc(POSIX_LOCALE_సామర్థ్యం + 1));
        /* TODO: Should we just exit on memory allocation failure? */
        ఒకవేళ (correctedPOSIXLocale)
        {
            int32_t posixLen = uloc_canonicalize(modifiedWindowsLocale, correctedPOSIXLocale, POSIX_LOCALE_సామర్థ్యం, &స్థితి_2);
            ఒకవేళ (U_SUCCESS(స్థితి_2))
            {
                *(correctedPOSIXLocale + posixLen) = 0;
                gCorrectedPOSIXLocale = correctedPOSIXLocale;
                g_Corrected_POSIXLocale_కుప్ప_Allocated = నిజం;
                ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
            }
            లేకపోతే
            {
                uprv_free(correctedPOSIXLocale);
            }
        }
    }

    // If unable to find a locale we can agree upon, use en-US by default
    ఒకవేళ (gCorrectedPOSIXLocale == శూన్యసూచిక) {
        gCorrectedPOSIXLocale = "en_US";
    }
    తిరిగి gCorrectedPOSIXLocale;

#లేదావేళ U_PLATFORM == U_PF_OS400
    /* locales are process scoped and are by definition thread safe */
    స్థిర అక్షరం correctedLocale[64];
    స్థిరరాశి  అక్షరం *localeID = getenv("LC_ALL");
           అక్షరం *p;

    ఒకవేళ (localeID == శూన్యసూచిక)
        localeID = getenv("LANG");
    ఒకవేళ (localeID == శూన్యసూచిక)
        localeID = setlocale(LC_ALL, శూన్యసూచిక);
    /* Make sure we have something... */
    ఒకవేళ (localeID == శూన్యసూచిక)
        తిరిగి "en_US_POSIX";

    /* Extract the locale name from the path. */
    ఒకవేళ((p = uprv_strrchr(localeID, '/')) != శూన్యసూచిక)
    {
        /* Increment p to start of locale name. */
        p++;
        localeID = p;
    }

    /* Copy to work location. */
    uprv_strcpy(correctedLocale, localeID);

    /* Strip off the '.locale' extension. */
    ఒకవేళ((p = uprv_strchr(correctedLocale, '.')) != శూన్యసూచిక) {
        *p = 0;
    }

    /* Upper case the locale name. */
    T_CString_గమ్యం_పై_Case(correctedLocale);

    /* See if we are using the POSIX locale.  Any of the
    * following are equivalent and use the same QLGPGCMA
    * (POSIX) locale.
    * QLGPGCMA2 means UCS2
    * QLGPGCMA_4 means UTF-32
    * QLGPGCMA_8 means UTF-8
    */
    ఒకవేళ ((uprv_strcmp("C", correctedLocale) == 0) ||
        (uprv_strcmp("POSIX", correctedLocale) == 0) ||
        (uprv_strncmp("QLGPGCMA", correctedLocale, 8) == 0))
    {
        uprv_strcpy(correctedLocale, "en_US_POSIX");
    }
    లేకపోతే
    {
        int16_t LocaleLen;

        /* Lower case the lang portion. */
        కొరకు(p = correctedLocale; *p != 0 && *p != '_'; p++)
        {
            *p = uprv_tolower(*p);
        }

        /* Adjust for Euro.  After '_E' add 'URO'. */
        LocaleLen = uprv_strlen(correctedLocale);
        ఒకవేళ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'E')
        {
            uprv_strcat(correctedLocale, "URO");
        }

        /* If using Lotus-based locale then convert to
         * equivalent non Lotus.
         */
        లేకపోతే ఒకవేళ (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'L')
        {
            correctedLocale[LocaleLen - 2] = 0;
        }

        /* There are separate simplified and traditional
         * locales called zh_HK_S and zh_HK_T.
         */
        లేకపోతే ఒకవేళ (uprv_strncmp(correctedLocale, "zh_HK", 5) == 0)
        {
            uprv_strcpy(correctedLocale, "zh_HK");
        }

        /* A special zh_CN_GBK locale...
        */
        లేకపోతే ఒకవేళ (uprv_strcmp(correctedLocale, "zh_CN_GBK") == 0)
        {
            uprv_strcpy(correctedLocale, "zh_CN");
        }

    }

    తిరిగి correctedLocale;
#అయితే_ముగింపు

}

#ఒకవేళ !U_CHARSET_IS_UTF8
#ఒకవేళ U_POSIX_LOCALE
/*
Due to various platform differences, one platform may specify a charset,
when they really mean a different charset. Remap the names so that they are
compatible with ICU. Only conflicting/ambiguous aliases should be resolved
here. Before adding anything to this function, please consider adding unique
names to the ICU alias table in the data directory.
*/
స్థిర స్థిరరాశి అక్షరం*
remapPlatformDependentCodepage(స్థిరరాశి అక్షరం *locale, స్థిరరాశి అక్షరం *name) {
    ఒకవేళ (locale != శూన్యసూచిక && *locale == 0) {
        /* Make sure that an empty locale is handled the same way. */
        locale = శూన్యసూచిక;
    }
    ఒకవేళ (name == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
#ఒకవేళ U_PLATFORM == U_PF_AIX
    ఒకవేళ (uprv_strcmp(name, "IBM-943") == 0) {
        /* Use the ASCII compatible ibm-943 */
        name = "Shift-JIS";
    }
    లేకపోతే ఒకవేళ (uprv_strcmp(name, "IBM-1252") == 0) {
        /* Use the windows-1252 that contains the Euro */
        name = "IBM-5348";
    }
#లేదావేళ U_PLATFORM == U_PF_SOLARIS
    ఒకవేళ (locale != శూన్యసూచిక && uprv_strcmp(name, "EUC") == 0) {
        /* Solaris underspecifies the "EUC" name. */
        ఒకవేళ (uprv_strcmp(locale, "zh_CN") == 0) {
            name = "EUC-CN";
        }
        లేకపోతే ఒకవేళ (uprv_strcmp(locale, "zh_TW") == 0) {
            name = "EUC-TW";
        }
        లేకపోతే ఒకవేళ (uprv_strcmp(locale, "ko_KR") == 0) {
            name = "EUC-KR";
        }
    }
    లేకపోతే ఒకవేళ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-954 is the best match.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    లేకపోతే ఒకవేళ (uprv_strcmp(name, "646") == 0) {
        /*
         * The default codepage given by Solaris is 646 but the C library routines treat it as if it was
         * ISO-8859-1 instead of US-ASCII(646).
         */
        name = "ISO-8859-1";
    }
#లేదావేళ U_PLATFORM_IS_DARWIN_BASED
    ఒకవేళ (locale == శూన్యసూచిక && *name == 0) {
        /*
        No locale was specified, and an empty name was passed in.
        This usually indicates that nl_langinfo didn't return valid information.
        Mac OS X uses UTF-8 by default (especially the locale data and console).
        */
        name = "UTF-8";
    }
    లేకపోతే ఒకవేళ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
    లేకపోతే ఒకవేళ (locale != శూన్యసూచిక && uprv_strcmp(locale, "en_US_POSIX") != 0 && uprv_strcmp(name, "US-ASCII") == 0) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
#లేదావేళ U_PLATFORM == U_PF_BSD
    ఒకవేళ (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
#లేదావేళ U_PLATFORM == U_PF_HPUX
    ఒకవేళ (locale != శూన్యసూచిక && uprv_strcmp(locale, "zh_HK") == 0 && uprv_strcmp(name, "big5") == 0) {
        /* HP decided to extend big5 as hkbig5 even though it's not compatible :-( */
        /* zh_TW.big5 is not the same charset as zh_HK.big5! */
        name = "hkbig5";
    }
    లేకపోతే ఒకవేళ (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
#లేదావేళ U_PLATFORM == U_PF_LINUX
    ఒకవేళ (locale != శూన్యసూచిక && uprv_strcmp(name, "euc") == 0) {
        /* Linux underspecifies the "EUC" name. */
        ఒకవేళ (uprv_strcmp(locale, "korean") == 0) {
            name = "EUC-KR";
        }
        లేకపోతే ఒకవేళ (uprv_strcmp(locale, "japanese") == 0) {
            /* See comment below about eucJP */
            name = "eucjis";
        }
    }
    లేకపోతే ఒకవేళ (uprv_strcmp(name, "eucjp") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    లేకపోతే ఒకవేళ (locale != శూన్యసూచిక && uprv_strcmp(locale, "en_US_POSIX") != 0 &&
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
#అయితే_ముగింపు
    /* return nullptr when "" is passed in */
    ఒకవేళ (*name == 0) {
        name = శూన్యసూచిక;
    }
    తిరిగి name;
}

స్థిర స్థిరరాశి అక్షరం*
getCodepageFromPOSIXID(స్థిరరాశి అక్షరం *localeName, అక్షరం * బఫరు, int32_t buff_సామర్థ్యం)
{
    అక్షరం localeBuf[100];
    స్థిరరాశి అక్షరం *name = శూన్యసూచిక;
    అక్షరం *variant = శూన్యసూచిక;

    ఒకవేళ (localeName != శూన్యసూచిక && (name = (uprv_strchr(localeName, '.'))) != శూన్యసూచిక) {
        size_t locale_సామర్థ్యం = uprv_min(పరిమాణం(localeBuf), (name-localeName)+1);
        uprv_strncpy(localeBuf, localeName, locale_సామర్థ్యం);
        localeBuf[locale_సామర్థ్యం-1] = 0; /* ensure NUL termination */
        name = uprv_strncpy(బఫరు, name+1, buff_సామర్థ్యం);
        బఫరు[buff_సామర్థ్యం-1] = 0; /* ensure NUL termination */
        ఒకవేళ ((variant = const_cast<అక్షరం *>(uprv_strchr(name, '@'))) != శూన్యసూచిక) {
            *variant = 0;
        }
        name = remapPlatformDependentCodepage(localeBuf, name);
    }
    తిరిగి name;
}
#అయితే_ముగింపు

స్థిర స్థిరరాశి అక్షరం*
int_getDefaultCodepage()
{
#ఒకవేళ U_PLATFORM == U_PF_OS400
    uint32_t ccsid = 37; /* Default to ibm-37 */
    స్థిర అక్షరం codepage[64];
    Qwc_JOBI0400_t jobinfo;
    Qus_EC_t error = { పరిమాణం(Qus_EC_t) }; /* SPI error code */

    EPT_CALL(QUSRJOBI)(&jobinfo, పరిమాణం(jobinfo), "JOBI0400",
        "*                         ", "                ", &error);

    ఒకవేళ (error.బైట్లు_అందుబాటు == 0) {
        ఒకవేళ (jobinfo.Coded_Char_సమితి_ID != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Coded_Char_సమితి_ID;
        }
        లేకపోతే ఒకవేళ (jobinfo.Default_Coded_Char_సమితి_Id != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Default_Coded_Char_సమితి_Id;
        }
        /* else use the default */
    }
    snprintf(codepage, పరిమాణం(codepage), "ibm-%d", ccsid);
    తిరిగి codepage;

#లేదావేళ U_PLATFORM == U_PF_OS390
    స్థిర అక్షరం codepage[64];

    strncpy(codepage, nl_langinfo(CODESET),63-అక్షరశ్రేణి_పొడవు(UCNV_మార్పిడి_LFNL_OPTION_STRING));
    strcat(codepage,UCNV_మార్పిడి_LFNL_OPTION_STRING);
    codepage[63] = 0; /* NUL terminate */

    తిరిగి codepage;

#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
    స్థిర అక్షరం codepage[64];
    DWORD codepage_సంఖ్య = 0;

#ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 1
    // UWP doesn't have a direct API to get the default ACP as Microsoft would rather
    // have folks use Unicode than a "system" code page, however this is the same
    // codepage as the system default locale codepage.  (FWIW, the system locale is
    // ONLY used for codepage, it should never be used for anything else)
    GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_సంఖ్య,
        (LPWSTR)&codepage_సంఖ్య, పరిమాణం(codepage_సంఖ్య) / పరిమాణం(WCHAR));
#లేకపోతే
    // Win32 apps can call GetACP
    codepage_సంఖ్య = GetACP();
#అయితే_ముగింపు
    // Special case for UTF-8
    ఒకవేళ (codepage_సంఖ్య == 65001)
    {
        తిరిగి "UTF-8";
    }
    // Windows codepages can look like windows-1252, so format the found number
    // the numbers are eclectic, however all valid system code pages, besides UTF-8
    // are between 3 and 19999
    ఒకవేళ (codepage_సంఖ్య > 0 && codepage_సంఖ్య < 20000)
    {
        snprintf(codepage, పరిమాణం(codepage), "windows-%ld", codepage_సంఖ్య);
        తిరిగి codepage;
    }
    // If the codepage number call failed then return UTF-8
    తిరిగి "UTF-8";

#లేదావేళ U_POSIX_LOCALE
    స్థిర అక్షరం codesetName[100];
    స్థిరరాశి అక్షరం *localeName = శూన్యసూచిక;
    స్థిరరాశి అక్షరం *name = శూన్యసూచిక;

    localeName = uprv_getPOSIXIDForDefaultCodepage();
    uprv_memset(codesetName, 0, పరిమాణం(codesetName));
    /* On Solaris nl_langinfo returns C locale values unless setlocale
     * was called earlier.
     */
#ఒకవేళ (U_HAVE_NL_LANGINFO_CODESET && U_PLATFORM != U_PF_SOLARIS)
    /* When available, check nl_langinfo first because it usually gives more
       useful names. It depends on LC_CTYPE.
       nl_langinfo may use the same buffer as setlocale. */
    {
        స్థిరరాశి అక్షరం *codeset = nl_langinfo(U_NL_LANGINFO_CODESET);
#ఒకవేళ U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED
        /*
         * On Linux and MacOSX, ensure that default codepage for non C/POSIX locale is UTF-8
         * instead of ASCII.
         */
        ఒకవేళ (uprv_strcmp(localeName, "en_US_POSIX") != 0) {
            codeset = remapPlatformDependentCodepage(localeName, codeset);
        } లేకపోతే
#అయితే_ముగింపు
        {
            codeset = remapPlatformDependentCodepage(శూన్యసూచిక, codeset);
        }

        ఒకవేళ (codeset != శూన్యసూచిక) {
            uprv_strncpy(codesetName, codeset, పరిమాణం(codesetName));
            codesetName[పరిమాణం(codesetName)-1] = 0;
            తిరిగి codesetName;
        }
    }
#అయితే_ముగింపు

    /* Use setlocale in a nice way, and then check some environment variables.
       Maybe the application used setlocale already.
    */
    uprv_memset(codesetName, 0, పరిమాణం(codesetName));
    name = getCodepageFromPOSIXID(localeName, codesetName, పరిమాణం(codesetName));
    ఒకవేళ (name) {
        /* if we can find the codeset name from setlocale, return that. */
        తిరిగి name;
    }

    ఒకవేళ (*codesetName == 0)
    {
        /* Everything failed. Return US ASCII (ISO 646). */
        (శూన్య)uprv_strcpy(codesetName, "US-ASCII");
    }
    తిరిగి codesetName;
#లేకపోతే
    తిరిగి "US-ASCII";
#అయితే_ముగింపు
}


U_CAPI స్థిరరాశి అక్షరం*  U_EXPORT2
uprv_getDefaultCodepage()
{
    స్థిర అక్షరం స్థిరరాశి  *name = శూన్యసూచిక;
    umtx_తాళం(శూన్యసూచిక);
    ఒకవేళ (name == శూన్యసూచిక) {
        name = int_getDefaultCodepage();
    }
    umtx_unlock(శూన్యసూచిక);
    తిరిగి name;
}
#అయితే_ముగింపు  /* !U_CHARSET_IS_UTF8 */


/* end of platform-specific implementation -------------- */

/* version handling --------------------------------------------------------- */

U_CAPI శూన్య U_EXPORT2
u_సంస్కరణ_From_String(UVersionInfo సంస్కరణ_Array, స్థిరరాశి అక్షరం *సంస్కరణ_String) {
    అక్షరం *చివర;
    uint16_t part=0;

    ఒకవేళ(సంస్కరణ_Array==శూన్యసూచిక) {
        తిరిగి;
    }

    ఒకవేళ(సంస్కరణ_String!=శూన్యసూచిక) {
        కొరకు(;;) {
            సంస్కరణ_Array[part]=(uint8_t)uprv_strtoul(సంస్కరణ_String, &చివర, 10);
            ఒకవేళ(చివర==సంస్కరణ_String || ++part==U_MAX_సంస్కరణ_నిడివి || *చివర!=U_సంస్కరణ_DELIMITER) {
                ఆపు;
            }
            సంస్కరణ_String=చివర+1;
        }
    }

    అయితే(part<U_MAX_సంస్కరణ_నిడివి) {
        సంస్కరణ_Array[part++]=0;
    }
}

U_CAPI శూన్య U_EXPORT2
u_సంస్కరణ_From_UString(UVersionInfo సంస్కరణ_Array, స్థిరరాశి char16_t *సంస్కరణ_String) {
    ఒకవేళ(సంస్కరణ_Array!=శూన్యసూచిక && సంస్కరణ_String!=శూన్యసూచిక) {
        అక్షరం సంస్కరణ_Chars[U_MAX_సంస్కరణ_STRING_నిడివి+1];
        int32_t len = u_strlen(సంస్కరణ_String);
        ఒకవేళ(len>U_MAX_సంస్కరణ_STRING_నిడివి) {
            len = U_MAX_సంస్కరణ_STRING_నిడివి;
        }
        u_UChars_గమ్యం_Chars(సంస్కరణ_String, సంస్కరణ_Chars, len);
        సంస్కరణ_Chars[len]=0;
        u_సంస్కరణ_From_String(సంస్కరణ_Array, సంస్కరణ_Chars);
    }
}

U_CAPI శూన్య U_EXPORT2
u_సంస్కరణ_గమ్యం_String(స్థిరరాశి UVersionInfo సంస్కరణ_Array, అక్షరం *సంస్కరణ_String) {
    uint16_t count, part;
    uint8_t క్షేత్రం_2;

    ఒకవేళ(సంస్కరణ_String==శూన్యసూచిక) {
        తిరిగి;
    }

    ఒకవేళ(సంస్కరణ_Array==శూన్యసూచిక) {
        సంస్కరణ_String[0]=0;
        తిరిగి;
    }

    /* count how many fields need to be written */
    కొరకు(count=4; count>0 && సంస్కరణ_Array[count-1]==0; --count) {
    }

    ఒకవేళ(count <= 1) {
        count = 2;
    }

    /* write the first part */
    /* write the decimal field value */
    క్షేత్రం_2=సంస్కరణ_Array[0];
    ఒకవేళ(క్షేత్రం_2>=100) {
        *సంస్కరణ_String++=(అక్షరం)('0'+క్షేత్రం_2/100);
        క్షేత్రం_2%=100;
    }
    ఒకవేళ(క్షేత్రం_2>=10) {
        *సంస్కరణ_String++=(అక్షరం)('0'+క్షేత్రం_2/10);
        క్షేత్రం_2%=10;
    }
    *సంస్కరణ_String++=(అక్షరం)('0'+క్షేత్రం_2);

    /* write the following parts */
    కొరకు(part=1; part<count; ++part) {
        /* write a dot first */
        *సంస్కరణ_String++=U_సంస్కరణ_DELIMITER;

        /* write the decimal field value */
        క్షేత్రం_2=సంస్కరణ_Array[part];
        ఒకవేళ(క్షేత్రం_2>=100) {
            *సంస్కరణ_String++=(అక్షరం)('0'+క్షేత్రం_2/100);
            క్షేత్రం_2%=100;
        }
        ఒకవేళ(క్షేత్రం_2>=10) {
            *సంస్కరణ_String++=(అక్షరం)('0'+క్షేత్రం_2/10);
            క్షేత్రం_2%=10;
        }
        *సంస్కరణ_String++=(అక్షరం)('0'+క్షేత్రం_2);
    }

    /* NUL-terminate */
    *సంస్కరణ_String=0;
}

U_CAPI శూన్య U_EXPORT2
u_get_సంస్కరణ(UVersionInfo సంస్కరణ_Array) {
    (శూన్య)copyright;   // Suppress unused variable warning from clang.
    u_సంస్కరణ_From_String(సంస్కరణ_Array, U_ICU_సంస్కరణ);
}

/**
 * icucfg.h dependent code
 */

#ఒకవేళ U_ENABLE_DYLOAD && HAVE_DLOPEN && !U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API

#ఒకవేళ HAVE_DLFCN_H
#నిర్వచితమైతే __MVS__
#నిర్వచితం_కాకపోతే __SUSV3
#నిర్వచించు __SUSV3 1
#అయితే_ముగింపు
#అయితే_ముగింపు
#అన్తర్భావ <dlfcn.h>
#అయితే_ముగింపు /* HAVE_DLFCN_H */

U_CAPI శూన్య * U_EXPORT2
uprv_dl_open(స్థిరరాశి అక్షరం *libName, UError_సంకేతం *స్థితి_2) {
  శూన్య *ret = శూన్యసూచిక;
  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి ret;
  ret =  dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
  ఒకవేళ(ret==శూన్యసూచిక) {
#నిర్వచితమైతే U_TRACE_DYLOAD
    రాయి("dlerror on dlopen(%s): %s\n", libName, dlerror());
#అయితే_ముగింపు
    *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
  }
  తిరిగి ret;
}

U_CAPI శూన్య U_EXPORT2
uprv_dl_close(శూన్య *lib, UError_సంకేతం *స్థితి_2) {
  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి;
  dlclose(lib);
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(శూన్య *lib, స్థిరరాశి అక్షరం* sym, UError_సంకేతం *స్థితి_2) {
  సంఘం {
      UVoidFunction *fp;
      శూన్య *vp;
  } uret;
  uret.fp = శూన్యసూచిక;
  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి uret.fp;
  uret.vp = dlsym(lib, sym);
  ఒకవేళ(uret.vp == శూన్యసూచిక) {
#నిర్వచితమైతే U_TRACE_DYLOAD
    రాయి("dlerror on dlsym(%p,%s): %s\n", lib,sym, dlerror());
#అయితే_ముగింపు
    *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
  }
  తిరిగి uret.fp;
}

#లేదావేళ U_ENABLE_DYLOAD && U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API && !U_PLATFORM_HAS_WINUWP_API

/* Windows API implementation. */
// Note: UWP does not expose/allow these APIs, so the UWP version gets the null implementation. */

U_CAPI శూన్య * U_EXPORT2
uprv_dl_open(స్థిరరాశి అక్షరం *libName, UError_సంకేతం *స్థితి_2) {
  HMODULE lib = శూన్యసూచిక;

  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి శూన్యసూచిక;

  lib = ఎక్కించు_Library_A(libName);

  ఒకవేళ(lib==శూన్యసూచిక) {
    *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
  }

  తిరిగి (శూన్య*)lib;
}

U_CAPI శూన్య U_EXPORT2
uprv_dl_close(శూన్య *lib, UError_సంకేతం *స్థితి_2) {
  HMODULE handle = (HMODULE)lib;
  ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి;

  FreeLibrary(handle);

  తిరిగి;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(శూన్య *lib, స్థిరరాశి అక్షరం* sym, UError_సంకేతం *స్థితి_2) {
  HMODULE handle = (HMODULE)lib;
  UVoidFunction* addr = శూన్యసూచిక;

  ఒకవేళ(U_FAILURE(*స్థితి_2) || lib==శూన్యసూచిక) తిరిగి శూన్యసూచిక;

  addr = (UVoidFunction*)Get_Proc_చిరునామా(handle, sym);

  ఒకవేళ(addr==శూన్యసూచిక) {
    DWORD చివరి_Error = Get_చివరి_Error();
    ఒకవేళ(చివరి_Error == ERROR_PROC_NOT_దొరికింది) {
      *స్థితి_2 = U_MISSING_RESOURCE_ERROR;
    } లేకపోతే {
      *స్థితి_2 = U_UNSUPPORTED_ERROR; /* other unknown error. */
    }
  }

  తిరిగి addr;
}

#లేకపోతే

/* No dynamic loading, null (nonexistent) implementation. */

U_CAPI శూన్య * U_EXPORT2
uprv_dl_open(స్థిరరాశి అక్షరం *libName, UError_సంకేతం *స్థితి_2) {
    (శూన్య)libName;
    ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి శూన్యసూచిక;
    *స్థితి_2 = U_UNSUPPORTED_ERROR;
    తిరిగి శూన్యసూచిక;
}

U_CAPI శూన్య U_EXPORT2
uprv_dl_close(శూన్య *lib, UError_సంకేతం *స్థితి_2) {
    (శూన్య)lib;
    ఒకవేళ(U_FAILURE(*స్థితి_2)) తిరిగి;
    *స్థితి_2 = U_UNSUPPORTED_ERROR;
    తిరిగి;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(శూన్య *lib, స్థిరరాశి అక్షరం* sym, UError_సంకేతం *స్థితి_2) {
  (శూన్య)lib;
  (శూన్య)sym;
  ఒకవేళ(U_SUCCESS(*స్థితి_2)) {
    *స్థితి_2 = U_UNSUPPORTED_ERROR;
  }
  తిరిగి (UVoidFunction*)శూన్యసూచిక;
}

#అయితే_ముగింపు

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

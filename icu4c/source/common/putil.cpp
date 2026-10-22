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
#समावेश "uposixdefs.h"

// First, the platform type. Need this for U_PLATFORM.
#समावेश "unicode/platform.h"

/*
 * Cygwin with GCC requires inclusion of time.h after the above disabling strict asci mode statement.
 */
#समावेश <time.h>

#अगर !U_PLATFORM_USES_ONLY_WIN32_API
#समावेश <sys/time.h>
#पूर्ण_अगर

/* include the rest of the ICU headers */
#समावेश "unicode/putil.h"
#समावेश "unicode/ustring.h"
#समावेश "putilimp.h"
#समावेश "uassert.h"
#समावेश "umutex.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "locmap.h"
#समावेश "ucln_cmn.h"
#समावेश "charstr.h"

/* Include standard headers. */
#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <math.h>
#समावेश <locale.h>
#समावेश <float.h>

#अगर_अघोषित U_COMMON_IMPLEMENTATION
#त्रुटि U_COMMON_IMPLEMENTATION not set - must be set क्रम all ICU source files in common/ - see https://unicode-org.github.io/icu/userguide/icu/howtouseicu.html
#पूर्ण_अगर


/* include system headers */
#अगर U_PLATFORM_USES_ONLY_WIN32_API
    /*
     * TODO: U_PLATFORM_USES_ONLY_WIN32_API includes MinGW.
     * Should Cygwin be included as well (U_PLATFORM_HAS_WIN32_API)
     * to use native APIs as much as possible?
     */
#अगर_अघोषित WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#पूर्ण_अगर
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>
#   include "unicode/uloc.h"
#   include "wintz.h"
#या_अगर U_PLATFORM == U_PF_OS400
#   include <float.h>
#   include <qusec.h>       /* error code structure */
#   include <qusrjobi.h>
#   include <qliept.h>      /* EPT_CALL macro  - this include must be after all other "QSYSINCs" */
#   include <mih/testptr.h> /* For uprv_maximumPtr */
#या_अगर U_PLATFORM == U_PF_OS390
#   include "unicode/ucnv.h"   /* Needed for UCNV_SWAP_LFNL_OPTION_STRING */
#या_अगर U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS
#   include <limits.h>
#   include <unistd.h>
#   अगर U_PLATFORM == U_PF_SOLARIS
#       ifndef _XPG4_2
#           define _XPG4_2
#       endif
#   elif U_PLATFORM == U_PF_ANDROID
#       include <sys/system_properties.h>
#       include <dlfcn.h>
#   endif
#या_अगर U_PLATFORM == U_PF_QNX
#   include <sys/neutrino.h>
#पूर्ण_अगर


/*
 * Only include langinfo.h if we have a way to get the codeset. If we later
 * depend on more feature, we can test on U_HAVE_NL_LANGINFO.
 *
 */

#अगर U_HAVE_NL_LANGINFO_CODESET
#समावेश <langinfo.h>
#पूर्ण_अगर

/**
 * Simple things (presence of functions, etc) should just go in configure.in and be added to
 * icucfg.h via autoheader.
 */
#अगर U_PLATFORM_IMPLEMENTS_POSIX
#   अगर U_PLATFORM == U_PF_OS400
#    define HAVE_DLFCN_H 0
#    define HAVE_DLOPEN 0
#   अन्यथा
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
#अन्यथा
#   define HAVE_DLFCN_H 0
#   define HAVE_DLOPEN 0
#   define HAVE_GETTIMEOFDAY 0
#पूर्ण_अगर

U_NAMESPACE_USE

/* Define the extension for data files, again... */
#घोषणा तथ्य_TYPE "dat"

/* Leave this copyright notice here! */
अटल स्थिर अक्षर copyright[] = U_COPYRIGHT_STRING;

/* floating point implementations ------------------------------------------- */

/* We return QNAN rather than SNAN*/
#घोषणा SIGN 0x80000000U

/* Make it easy to define certain types of constants */
प्रकार जोड़ {
    int64_t i64; /* This must be defined first in order to allow the initialization to work. This is a C89 feature. */
    द्विगुन d64;
} BitPatternConversion;
अटल स्थिर BitPatternConversion gNan = {static_cast<int64_t>(INT64_C(0x7FF8000000000000))};
अटल स्थिर BitPatternConversion gInf = {static_cast<int64_t>(INT64_C(0x7FF0000000000000))};

/*---------------------------------------------------------------------------
  Platform utilities
  Our general strategy is to assume we're on a POSIX platform.  Platforms which
  are non-POSIX must declare themselves so.  The default POSIX implementation
  will sometimes work for non-POSIX platforms as well (e.g., the NaN-related
  functions).
  ---------------------------------------------------------------------------*/

#अगर U_PLATFORM_USES_ONLY_WIN32_API || U_PLATFORM == U_PF_OS400
#   undef U_POSIX_LOCALE
#अन्यथा
#   define U_POSIX_LOCALE    1
#पूर्ण_अगर

/*
    WARNING! u_topNBytesOfDouble and u_bottomNBytesOfDouble
    can't be properly optimized by the gcc compiler sometimes (i.e. gcc 3.2).
*/
#अगर !IEEE_754
अटल अक्षर*
u_शीर्ष_NBytes_Of_Double(द्विगुन* d, पूर्णांक n)
{
#अगर U_IS_BIG_ENDIAN
    वापस (अक्षर*)d;
#अन्यथा
    वापस (अक्षर*)(d + 1) - n;
#पूर्ण_अगर
}

अटल अक्षर*
u_bottomNBytesOfDouble(द्विगुन* d, पूर्णांक n)
{
#अगर U_IS_BIG_ENDIAN
    वापस (अक्षर*)(d + 1) - n;
#अन्यथा
    वापस (अक्षर*)d;
#पूर्ण_अगर
}
#पूर्ण_अगर   /* !IEEE_754 */

#अगर IEEE_754
अटल UBool
u_signBit(द्विगुन d) {
    uint8_t hiByte;
#अगर U_IS_BIG_ENDIAN
    hiByte = *(uint8_t *)&d;
#अन्यथा
    hiByte = *(reinterpret_cast<uint8_t*>(&d) + माप(द्विगुन) - 1);
#पूर्ण_अगर
    वापस (hiByte & 0x80) != 0;
}
#पूर्ण_अगर



#अगर defined (U_DEBUG_FAKETIME)
/* Override the clock to test things without having to move the system clock.
 * Assumes POSIX gettimeofday() will function
 */
UDate fakeClock_t0 = 0; /** Time to start the clock from **/
UDate fakeClock_dt = 0; /** Offset (fake time - real time) **/
UBool fakeClock_set = false; /** True if fake clock has spun up **/

अटल UDate getUTCtime_real() {
    काष्ठा timeval posixTime;
    gettimeofday(&posixTime, nullptr);
    वापस (UDate)(((int64_t)posixTime.tv_sec * U_MILLIS_PER_SECOND) + (posixTime.tv_usec/1000));
}

अटल UDate getUTCtime_fake() {
    अटल UMutex fakeClockMutex;
    umtx_lock(&fakeClockMutex);
    अगर(!fakeClock_set) {
        UDate real = getUTCtime_real();
        स्थिर अक्षर *fake_start = दो_पर्या("U_FAKETIME_START");
        अगर((fake_start!=nullptr) && (fake_start[0]!=0)) {
            माला_पूछो(fake_start,"%lf",&fakeClock_t0);
            fakeClock_dt = fakeClock_t0 - real;
            ख_लिखो(मानक_त्रुटि,"U_DEBUG_FAKETIME was set at compile time, so the ICU clock will start at a preset value\n"
                    "env variable U_FAKETIME_START=%.0f (%s) for an offset of %.0f ms from the current time %.0f\n",
                    fakeClock_t0, fake_start, fakeClock_dt, real);
        } अन्यथा {
          fakeClock_dt = 0;
            ख_लिखो(मानक_त्रुटि,"U_DEBUG_FAKETIME was set at compile time, but U_FAKETIME_START was not set.\n"
                    "Set U_FAKETIME_START to the number of milliseconds since 1/1/1970 to set the ICU clock.\n");
        }
        fakeClock_set = true;
    }
    umtx_unlock(&fakeClockMutex);

    वापस getUTCtime_real() + fakeClock_dt;
}
#पूर्ण_अगर

#अगर U_PLATFORM_USES_ONLY_WIN32_API
प्रकार जोड़ {
    int64_t int64;
    FILETIME fileTime;
} FileTimeConversion;   /* This is like a ULARGE_INTEGER */

/* Number of 100 nanoseconds from 1/1/1601 to 1/1/1970 */
#घोषणा EPOCH_BIAS  INT64_C(116444736000000000)
#घोषणा HECTONANOSECOND_PER_MILLISECOND   10000

#पूर्ण_अगर

/*---------------------------------------------------------------------------
  Universal Implementations
  These are designed to work on all platforms.  Try these, and if they
  don't work on your platform, then special case your platform with new
  implementations.
---------------------------------------------------------------------------*/

U_CAPI UDate U_EXPORT2
uprv_getUTCtime()
{
#अगर defined(U_DEBUG_FAKETIME)
    वापस getUTCtime_fake(); /* Hook for overriding the clock */
#अन्यथा
    वापस uprv_getRawUTCtime();
#पूर्ण_अगर
}

/* Return UTC (GMT) time measured in milliseconds since 0:00 on 1/1/70.*/
U_CAPI UDate U_EXPORT2
uprv_getRawUTCtime()
{
#अगर U_PLATFORM_USES_ONLY_WIN32_API

    FileTimeConversion winTime;
    GetSystemTimeAsFileTime(&winTime.fileTime);
    वापस (UDate)((winTime.int64 - EPOCH_BIAS) / HECTONANOSECOND_PER_MILLISECOND);
#अन्यथा

#अगर HAVE_GETTIMEOFDAY
    काष्ठा timeval posixTime;
    gettimeofday(&posixTime, nullptr);
    वापस (UDate)(((int64_t)posixTime.tv_sec * U_MILLIS_PER_SECOND) + (posixTime.tv_usec/1000));
#अन्यथा
    समय_प्रकार epochtime;
    समय(&epochtime);
    वापस (UDate)epochtime * U_MILLIS_PER_SECOND;
#पूर्ण_अगर

#पूर्ण_अगर
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
uprv_isNaN(द्विगुन number)
{
#अगर IEEE_754
    BitPatternConversion converted_संख्या;
    converted_संख्या.d64 = number;
    /* Infinity is 0x7FF0000000000000U. Anything greater than that is a NaN */
    वापस (converted_संख्या.i64 & U_INT64_MAX) > gInf.i64;

#या_अगर U_PLATFORM == U_PF_OS390
    uint32_t ऊपरी_Bits = *(uint32_t*)u_शीर्ष_NBytes_Of_Double(&number,
                        माप(uint32_t));
    uint32_t निचला_Bits  = *(uint32_t*)u_bottomNBytesOfDouble(&number,
                        माप(uint32_t));

    वापस ((ऊपरी_Bits & 0x7F080000L) == 0x7F080000L) &&
      (निचला_Bits == 0x00000000L);

#अन्यथा
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    वापस number != number;
#पूर्ण_अगर
}

U_CAPI UBool U_EXPORT2
uprv_isInfinite(द्विगुन number)
{
#अगर IEEE_754
    BitPatternConversion converted_संख्या;
    converted_संख्या.d64 = number;
    /* Infinity is exactly 0x7FF0000000000000U. */
    वापस (converted_संख्या.i64 & U_INT64_MAX) == gInf.i64;
#या_अगर U_PLATFORM == U_PF_OS390
    uint32_t ऊपरी_Bits = *(uint32_t*)u_शीर्ष_NBytes_Of_Double(&number,
                        माप(uint32_t));
    uint32_t निचला_Bits  = *(uint32_t*)u_bottomNBytesOfDouble(&number,
                        माप(uint32_t));

    वापस ((ऊपरी_Bits  & ~SIGN) == 0x70FF0000L) && (निचला_Bits == 0x00000000L);

#अन्यथा
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    वापस number == (2.0 * number);
#पूर्ण_अगर
}

U_CAPI UBool U_EXPORT2
uprv_isPositiveInfinity(द्विगुन number)
{
#अगर IEEE_754 || U_PLATFORM == U_PF_OS390
    वापस number > 0 && uprv_isInfinite(number);
#अन्यथा
    वापस uprv_isInfinite(number);
#पूर्ण_अगर
}

U_CAPI UBool U_EXPORT2
uprv_isNegativeInfinity(द्विगुन number)
{
#अगर IEEE_754 || U_PLATFORM == U_PF_OS390
    वापस number < 0 && uprv_isInfinite(number);

#अन्यथा
    uint32_t ऊपरी_Bits = *(uint32_t*)u_शीर्ष_NBytes_Of_Double(&number,
                        माप(uint32_t));
    वापस((ऊपरी_Bits & SIGN) && uprv_isInfinite(number));

#पूर्ण_अगर
}

U_CAPI द्विगुन U_EXPORT2
uprv_getNaN()
{
#अगर IEEE_754 || U_PLATFORM == U_PF_OS390
    वापस gNan.d64;
#अन्यथा
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    वापस 0.0;
#पूर्ण_अगर
}

U_CAPI द्विगुन U_EXPORT2
uprv_getInfinity()
{
#अगर IEEE_754 || U_PLATFORM == U_PF_OS390
    वापस gInf.d64;
#अन्यथा
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    वापस 0.0;
#पूर्ण_अगर
}

U_CAPI द्विगुन U_EXPORT2
uprv_floor(द्विगुन x)
{
    वापस न्यूनमान(x);
}

U_CAPI द्विगुन U_EXPORT2
uprv_ceil(द्विगुन x)
{
    वापस उच्चमान(x);
}

U_CAPI द्विगुन U_EXPORT2
uprv_round(द्विगुन x)
{
    वापस uprv_floor(x + 0.5);
}

U_CAPI द्विगुन U_EXPORT2
uprv_fabs(द्विगुन x)
{
    वापस भ_असल(x);
}

U_CAPI द्विगुन U_EXPORT2
uprv_modf(द्विगुन x, द्विगुन* y)
{
    वापस modf(x, y);
}

U_CAPI द्विगुन U_EXPORT2
uprv_fmod(द्विगुन x, द्विगुन y)
{
    वापस भ_शेष(x, y);
}

U_CAPI द्विगुन U_EXPORT2
uprv_pow(द्विगुन x, द्विगुन y)
{
    /* This is declared as "double pow(double x, double y)" */
    वापस घात(x, y);
}

U_CAPI द्विगुन U_EXPORT2
uprv_pow10(int32_t x)
{
    वापस घात(10.0, (द्विगुन)x);
}

U_CAPI द्विगुन U_EXPORT2
uprv_fmax(द्विगुन x, द्विगुन y)
{
#अगर IEEE_754
    /* first handle NaN*/
    अगर(uprv_isNaN(x) || uprv_isNaN(y))
        वापस uprv_getNaN();

    /* check for -0 and 0*/
    अगर(x == 0.0 && y == 0.0 && u_signBit(x))
        वापस y;

#पूर्ण_अगर

    /* this should work for all flt point w/o NaN and Inf special cases */
    वापस (x > y ? x : y);
}

U_CAPI द्विगुन U_EXPORT2
uprv_fmin(द्विगुन x, द्विगुन y)
{
#अगर IEEE_754
    /* first handle NaN*/
    अगर(uprv_isNaN(x) || uprv_isNaN(y))
        वापस uprv_getNaN();

    /* check for -0 and 0*/
    अगर(x == 0.0 && y == 0.0 && u_signBit(y))
        वापस y;

#पूर्ण_अगर

    /* this should work for all flt point w/o NaN and Inf special cases */
    वापस (x > y ? y : x);
}

U_CAPI UBool U_EXPORT2
uprv_add32_overflow(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_add_overflow.
    // This function could be optimized by calling one of those primitives.
    स्वतः a64 = static_cast<int64_t>(a);
    स्वतः b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 + b64;
    *res = static_cast<int32_t>(res64);
    वापस res64 != *res;
}

U_CAPI UBool U_EXPORT2
uprv_mul32_overflow(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_mul_overflow.
    // This function could be optimized by calling one of those primitives.
    स्वतः a64 = static_cast<int64_t>(a);
    स्वतः b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 * b64;
    *res = static_cast<int32_t>(res64);
    वापस res64 != *res;
}

/**
 * Truncates the given double.
 * trunc(3.3) = 3.0, trunc (-3.3) = -3.0
 * This is different than calling floor() or ceil():
 * floor(3.3) = 3, floor(-3.3) = -4
 * ceil(3.3) = 4, ceil(-3.3) = -3
 */
U_CAPI द्विगुन U_EXPORT2
uprv_trunc(द्विगुन d)
{
#अगर IEEE_754
    /* handle error cases*/
    अगर(uprv_isNaN(d))
        वापस uprv_getNaN();
    अगर(uprv_isInfinite(d))
        वापस uprv_getInfinity();

    अगर(u_signBit(d))    /* Signbit() picks up -0.0;  d<0 does not. */
        वापस उच्चमान(d);
    अन्यथा
        वापस न्यूनमान(d);

#अन्यथा
    वापस d >= 0 ? न्यूनमान(d) : उच्चमान(d);

#पूर्ण_अगर
}

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 */
U_CAPI द्विगुन U_EXPORT2
uprv_maxMantissa()
{
    वापस घात(2.0, द्विग_पूर्ण + 1.0) - 1.0;
}

U_CAPI द्विगुन U_EXPORT2
uprv_log(द्विगुन d)
{
    वापस log(d);
}

U_CAPI व्योम * U_EXPORT2
uprv_maximumPtr(व्योम * base)
{
#अगर U_PLATFORM == U_PF_OS400
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
    अगर ((base != nullptr) && (_TESTPTR(base, _C_TERASPACE_CHECK))) {
        /* if it is a TERASPACE pointer the max is 2GB - 4k */
        वापस ((व्योम *)(((अक्षर *)base)-((uint32_t)(base))+((uint32_t)0x7fffefff)));
    }
    /* otherwise 16MB since nullptr ptr is not checkable or the ptr is not TERASPACE */
    वापस ((व्योम *)(((अक्षर *)base)-((uint32_t)(base))+((uint32_t)0xffefff)));

#अन्यथा
    वापस U_MAX_PTR(base);
#पूर्ण_अगर
}

/*---------------------------------------------------------------------------
  Platform-specific Implementations
  Try these, and if they don't work on your platform, then special case your
  platform with new implementations.
  ---------------------------------------------------------------------------*/

/* Generic time zone layer -------------------------------------------------- */

/* Time zone utilities */
U_CAPI व्योम U_EXPORT2
uprv_tzset()
{
#अगर defined(U_TZSET)
    U_TZSET();
#अन्यथा
    /* no initialization*/
#पूर्ण_अगर
}

U_CAPI int32_t U_EXPORT2
uprv_timezone()
{
#अगर_घोषित U_TIMEZONE
    वापस U_TIMEZONE;
#अन्यथा
    समय_प्रकार t, t1, t2;
    काष्ठा पंचांग tmrec;
    int32_t tdiff = 0;

    समय(&t);
    uprv_memcpy( &tmrec, स_स्थानीय(&t), माप(tmrec) );
#अगर U_PLATFORM != U_PF_IPHONE
    UBool dst_checked = (tmrec.tm_isdst != 0); /* daylight savings time is checked*/
#पूर्ण_अगर
    t1 = स_गढ़ो(&tmrec);                 /* local time in seconds*/
    uprv_memcpy( &tmrec, स_जमट(&t), माप(tmrec) );
    t2 = स_गढ़ो(&tmrec);                 /* GMT (or UTC) in seconds*/
    tdiff = t2 - t1;

#अगर U_PLATFORM != U_PF_IPHONE
    /* imitate NT behaviour, which returns same timezone offset to GMT for
       winter and summer.
       This does not work on all platforms. For instance, on glibc on Linux
       and on Mac OS 10.5, tdiff calculated above remains the same
       regardless of whether DST is in effect or not. iOS is another
       platform where this does not work. Linux + glibc and Mac OS 10.5
       have U_TIMEZONE defined so that this code is not reached.
    */
    अगर (dst_checked)
        tdiff += 3600;
#पूर्ण_अगर
    वापस tdiff;
#पूर्ण_अगर
}

/* Note that U_TZNAME does *not* have to be tzname, but if it is,
   some platforms need to have it declared here. */

#अगर defined(U_TZNAME) && (U_PLATFORM == U_PF_IRIX || U_PLATFORM_IS_DARWIN_BASED)
/* RS6000 and others reject char **tzname.  */
बाह्य U_IMPORT अक्षर *U_TZNAME[];
#पूर्ण_अगर

#अगर !UCONFIG_NO_FILE_IO && ((U_PLATFORM_IS_DARWIN_BASED && (U_PLATFORM != U_PF_IPHONE || defined(U_TIMEZONE))) || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS)
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

#घोषणा CHECK_LOCALTIME_LINK 1
#अगर U_PLATFORM_IS_DARWIN_BASED
#समावेश <tzfile.h>
#घोषणा TZZONEINFO      (TZDIR "/")
#या_अगर U_PLATFORM == U_PF_SOLARIS
#घोषणा TZDEFAULT       "/etc/localtime"
#घोषणा TZZONEINFO      "/usr/share/lib/zoneinfo/"
#घोषणा TZ_ENV_CHECK    "localtime"
#अन्यथा
#घोषणा TZDEFAULT       "/etc/localtime"
#घोषणा TZZONEINFO      "/usr/share/zoneinfo/"
#पूर्ण_अगर
#घोषणा TZZONEINFOTAIL  "/zoneinfo/"
#अगर U_HAVE_DIRENT_H
#घोषणा TZFILE_SKIP     "posixrules" /* tz file to skip when searching. */
/* Some Linux distributions have 'localtime' in /usr/share/zoneinfo
   symlinked to /etc/localtime, which makes searchForTZFile return
   'localtime' when it's the first match. */
#घोषणा TZFILE_SKIP2    "localtime"
#घोषणा खोज_TZFILE
#समावेश <dirent.h>  /* Needed to search through system timezone files */
#पूर्ण_अगर
अटल अक्षर g_Time_Zone_बफर[PATH_MAX];
अटल स्थिर अक्षर *g_Time_Zone_बफर_Ptr = nullptr;
#पूर्ण_अगर

#अगर !U_PLATFORM_USES_ONLY_WIN32_API
#घोषणा isNonDigit(ch) (ch < '0' || '9' < ch)
#घोषणा isDigit(ch) ('0' <= ch && ch <= '9')
अटल UBool isValidOlsonID(स्थिर अक्षर *id) {
    int32_t idx = 0;
    int32_t idxMax = 0;

    /* Determine if this is something like Iceland (Olson ID)
    or AST4ADT (non-Olson ID) */
    जबतक (id[idx] && isNonDigit(id[idx]) && id[idx] != ',') {
        idx++;
    }

    /* Allow at maximum 2 numbers at the end of the id to support zone id's
    like GMT+11. */
    idxMax = idx + 2;
    जबतक (id[idx] && isDigit(id[idx]) && idx < idxMax) {
        idx++;
    }

    /* If we went through the whole string, then it might be okay.
    The timezone is sometimes set to "CST-7CDT", "CST6CDT5,J129,J131/19:30",
    "GRNLNDST3GRNLNDDT" or similar, so we cannot use it.
    The rest of the time it could be an Olson ID. George */
    वापस id[idx] == 0
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
अटल व्योम skipZoneIDPrefix(स्थिर अक्षर** id) {
    अगर (uprv_strncmp(*id, "posix/", 6) == 0
        || uprv_strncmp(*id, "right/", 6) == 0)
    {
        *id += 6;
    }
}
#पूर्ण_अगर

#अगर defined(U_TZNAME) && !U_PLATFORM_USES_ONLY_WIN32_API

#घोषणा CONVERT_HOURS_TO_SECONDS(offset) (int32_t)(offset*3600)
प्रकार काष्ठा OffsetZoneMapping {
    int32_t offsetSeconds;
    int32_t daylightType; /* 0=U_DAYLIGHT_NONE, 1=daylight in June-U_DAYLIGHT_JUNE, 2=daylight in December=U_DAYLIGHT_DECEMBER*/
    स्थिर अक्षर *stdID;
    स्थिर अक्षर *dstID;
    स्थिर अक्षर *olsonID;
} OffsetZoneMapping;

क्रमागत { U_DAYLIGHT_NONE=0,U_DAYLIGHT_JUNE=1,U_DAYLIGHT_DECEMBER=2 };

/*
This list tries to disambiguate a set of abbreviated timezone IDs and offsets
and maps it to an Olson ID.
Before adding anything to this list, take a look at
icu/source/tools/tzcode/tz.alias
Sometimes no daylight savings (0) is important to define due to aliases.
This list can be tested with icu/source/test/compat/tzone.pl
More values could be added to daylightType to increase precision.
*/
अटल स्थिर काष्ठा OffsetZoneMapping OFFSET_ZONE_MAPPINGS[] = {
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

अटल स्थिर अक्षर* remapShortTimeZone(स्थिर अक्षर *stdID, स्थिर अक्षर *dstID, int32_t daylightType, int32_t offset)
{
    int32_t idx;
#अगर_घोषित DEBUG_TZNAME
    ख_लिखो(मानक_त्रुटि, "TZ=%s std=%s dst=%s daylight=%d offset=%d\n", दो_पर्या("TZ"), stdID, dstID, daylightType, offset);
#पूर्ण_अगर
    क्रम (idx = 0; idx < UPRV_LENGTHOF(OFFSET_ZONE_MAPPINGS); idx++)
    {
        अगर (offset == OFFSET_ZONE_MAPPINGS[idx].offsetSeconds
            && daylightType == OFFSET_ZONE_MAPPINGS[idx].daylightType
            && म_भेद(OFFSET_ZONE_MAPPINGS[idx].stdID, stdID) == 0
            && म_भेद(OFFSET_ZONE_MAPPINGS[idx].dstID, dstID) == 0)
        {
            वापस OFFSET_ZONE_MAPPINGS[idx].olsonID;
        }
    }
    वापस nullptr;
}
#पूर्ण_अगर

#अगर_घोषित खोज_TZFILE
#घोषणा MAX_READ_आकार 512

प्रकार काष्ठा DefaultTZInfo {
    अक्षर* defaultTZBuffer;
    int64_t default_TZFile_आकार;
    खाता* defaultTZFilePtr;
    int32_t defaultTZPosition;
} DefaultTZInfo;

/*
 * This method compares the two files given to see if they are a match.
 * It is currently use to compare two TZ files.
 */
अटल UBool compare_द्विआधारी_Files(स्थिर अक्षर* TZFileName, DefaultTZInfo* tzInfo) {
    खाता* file;
    int64_t आकार_File;
    int64_t आकार_File_बायाँ;
    int32_t आकार_File_Read;
    int32_t आकार_File_To_Read;
    अक्षर बफर_File[MAX_READ_आकार];
    UBool परिणाम = true;

    U_ASSERT(tzInfo->defaultTZFilePtr != nullptr);
    file = ख_खोलो(TZFileName, "r");

    tzInfo->defaultTZPosition = 0; /* reset position to begin search */

    अगर (file != nullptr) {
        /* First check that the file size are equal. */
        अगर (tzInfo->default_TZFile_आकार == 0) {
            ख_जाओ(tzInfo->defaultTZFilePtr, 0, अंत_से);
            tzInfo->default_TZFile_आकार = ख_बताओ(tzInfo->defaultTZFilePtr);
        }
        अगर (ख_जाओ(file, 0, अंत_से) != 0) {
            ख_बंद(file);
            वापस false;
        }
        आकार_File = ख_बताओ(file);
        अगर (आकार_File == -1) {
            ख_बंद(file);
            वापस false;
        }
        आकार_File_बायाँ = आकार_File;

        अगर (आकार_File != tzInfo->default_TZFile_आकार) {
            परिणाम = false;
        } अन्यथा {
            /* Store the data from the files in separate buffers and
             * compare each byte to determine equality.
             */
            अगर (tzInfo->defaultTZBuffer == nullptr) {
                अगर (ख_जाओ(tzInfo->defaultTZFilePtr, 0, शुरू_से) != 0) {
                    ख_बंद(file);
                    वापस false;
                }
                tzInfo->defaultTZBuffer = static_cast<अक्षर*>(uprv_malloc(माप(अक्षर) * tzInfo->default_TZFile_आकार));
                आकार_File_Read = ख_पढ़ो(tzInfo->defaultTZBuffer, 1, tzInfo->default_TZFile_आकार, tzInfo->defaultTZFilePtr);
            }
            अगर (ख_जाओ(file, 0, शुरू_से) != 0) {
                ख_बंद(file);
                वापस false;
            }
            जबतक(आकार_File_बायाँ > 0) {
                uprv_memset(बफर_File, 0, MAX_READ_आकार);
                आकार_File_To_Read = आकार_File_बायाँ < MAX_READ_आकार ? आकार_File_बायाँ : MAX_READ_आकार;

                आकार_File_Read = ख_पढ़ो(बफर_File, 1, आकार_File_To_Read, file);
                अगर (आकार_File_Read == -1) {
                    ख_बंद(file);
                    वापस false;
                }
                अगर (स_भेद(tzInfo->defaultTZBuffer + tzInfo->defaultTZPosition, बफर_File, आकार_File_Read) != 0) {
                    परिणाम = false;
                    अवरोध;
                }
                आकार_File_बायाँ -= आकार_File_Read;
                tzInfo->defaultTZPosition += आकार_File_Read;
            }
        }
    } अन्यथा {
        परिणाम = false;
    }

    अगर (file != nullptr) {
        ख_बंद(file);
    }

    वापस परिणाम;
}


/* dirent also lists two entries: "." and ".." that we can safely ignore. */
#घोषणा SKIP1 "."
#घोषणा SKIP2 ".."
अटल UBool U_CALLCONV putil_cleanup();
अटल CharString *g_खोज_TZFile_परिणाम = nullptr;

/*
 * This method recursively traverses the directory given for a matching TZ file and returns the first match.
 * This function is not thread safe - it uses a global, gSearchTZFileResult, to hold its results.
 */
अटल अक्षर* खोज_For_TZFile(स्थिर अक्षर* path, DefaultTZInfo* tzInfo) {
    सूची* dirp = nullptr;
    काष्ठा dirent* dirEntry = nullptr;
    अक्षर* परिणाम = nullptr;
    UErrorCode status = U_ZERO_ERROR;

    /* There is no reference file to compare against, so do not traverse the zoneinfo tree. */
    अगर (tzInfo->defaultTZFilePtr == nullptr) {
        वापस nullptr;
    }

    /* Save the current path */
    CharString curpath(path, -1, status);
    अगर (U_FAILURE(status)) {
        जाओ cleanupAndReturn;
    }

    dirp = सूची_खोलो(path);
    अगर (dirp == nullptr) {
        जाओ cleanupAndReturn;
    }

    अगर (g_खोज_TZFile_परिणाम == nullptr) {
        g_खोज_TZFile_परिणाम = नया CharString;
        अगर (g_खोज_TZFile_परिणाम == nullptr) {
            जाओ cleanupAndReturn;
        }
        ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    }

    /* Check each entry in the directory. */
    जबतक((dirEntry = सूची_पढ़ो(dirp)) != nullptr) {
        स्थिर अक्षर* dirName = dirEntry->d_name;
        अगर (uprv_strcmp(dirName, SKIP1) != 0 && uprv_strcmp(dirName, SKIP2) != 0
            && uprv_strcmp(TZFILE_SKIP, dirName) != 0 && uprv_strcmp(TZFILE_SKIP2, dirName) != 0) {
            /* Create a newpath with the new entry to test each entry in the directory. */
            CharString newpath(curpath, status);
            newpath.append(dirName, -1, status);
            अगर (U_FAILURE(status)) {
                अवरोध;
            }

            सूची* subDirp = nullptr;
            अगर ((subDirp = सूची_खोलो(newpath.तथ्य())) != nullptr) {
                /* If this new path is a directory, make a recursive call with the newpath. */
                बंद_सूची(subDirp);
                newpath.append('/', status);
                अगर (U_FAILURE(status)) {
                    अवरोध;
                }
                परिणाम = खोज_For_TZFile(newpath.तथ्य(), tzInfo);
                /*
                 Have to get out here. Otherwise, we'd keep looking
                 and return the first match in the top-level directory
                 if there's a match in the top-level. If not, this function
                 would return nullptr and set gTimeZoneBufferPtr to nullptr in initDefault().
                 It worked without this in most cases because we have a fallback of calling
                 localtime_r to figure out the default timezone.
                */
                अगर (परिणाम != nullptr)
                    अवरोध;
            } अन्यथा {
                अगर(compare_द्विआधारी_Files(newpath.तथ्य(), tzInfo)) {
                    int32_t amountToSkip = माप(TZZONEINFO) - 1;
                    अगर (amountToSkip > newpath.length()) {
                        amountToSkip = newpath.length();
                    }
                    स्थिर अक्षर* zoneid = newpath.तथ्य() + amountToSkip;
                    skipZoneIDPrefix(&zoneid);
                    g_खोज_TZFile_परिणाम->सफा();
                    g_खोज_TZFile_परिणाम->append(zoneid, -1, status);
                    अगर (U_FAILURE(status)) {
                        अवरोध;
                    }
                    परिणाम = g_खोज_TZFile_परिणाम->तथ्य();
                    /* Get out after the first one found. */
                    अवरोध;
                }
            }
        }
    }

  cleanupAndReturn:
    अगर (dirp) {
        बंद_सूची(dirp);
    }
    वापस परिणाम;
}
#पूर्ण_अगर

#अगर U_PLATFORM == U_PF_ANDROID
प्रकार पूर्णांक(system_property_read_callback)(स्थिर prop_info* info,
                                           व्योम (*callback)(व्योम* cookie,
                                                            स्थिर अक्षर* name,
                                                            स्थिर अक्षर* value,
                                                            uint32_t serial),
                                           व्योम* cookie);
प्रकार पूर्णांक(system_property_get)(स्थिर अक्षर*, अक्षर*);

अटल अक्षर gAndroidTimeZone[PROP_मूल्य_MAX] = { '\0' };

अटल व्योम u_property_read(व्योम* cookie, स्थिर अक्षर* name, स्थिर अक्षर* value,
                            uint32_t serial) {
    uprv_strcpy((अक्षर* )cookie, value);
}
#पूर्ण_अगर

U_CAPI व्योम U_EXPORT2
uprv_tzname_clear_cache()
{
#अगर U_PLATFORM == U_PF_ANDROID
    /* Android's timezone is stored in system property. */
    gAndroidTimeZone[0] = '\0';
    व्योम* libc = dlopen("libc.so", RTLD_NOLOAD);
    अगर (libc) {
        /* Android API 26+ has new API to get system property and old API
         * (__system_property_get) is deprecated */
        system_property_read_callback* property_read_callback =
            (system_property_read_callback*)dlsym(
                libc, "__system_property_read_callback");
        अगर (property_read_callback) {
            स्थिर prop_info* info =
                __system_property_find("persist.sys.timezone");
            अगर (info) {
                property_read_callback(info, &u_property_read, gAndroidTimeZone);
            }
        } अन्यथा {
            system_property_get* property_get =
                (system_property_get*)dlsym(libc, "__system_property_get");
            अगर (property_get) {
                property_get("persist.sys.timezone", gAndroidTimeZone);
            }
        }
        dlclose(libc);
    }
#पूर्ण_अगर

#अगर defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    g_Time_Zone_बफर_Ptr = nullptr;
#पूर्ण_अगर
}

U_CAPI स्थिर अक्षर* U_EXPORT2
uprv_tzname(पूर्णांक n)
{
    (व्योम)n; // Avoid unreferenced parameter warning.
    स्थिर अक्षर *tzid = nullptr;
#अगर U_PLATFORM_USES_ONLY_WIN32_API
    tzid = uprv_detectWindowsTimeZone();

    अगर (tzid != nullptr) {
        वापस tzid;
    }

#अगर_अघोषित U_TZNAME
    // The return value is free'd in timezone.cpp on Windows because
    // the other code path returns a pointer to a heap location.
    // If we don't have a name already, then tzname wouldn't be any
    // better, so just fall back.
    वापस uprv_strdup("");
#पूर्ण_अगर // !U_TZNAME

#अन्यथा

/*#if U_PLATFORM_IS_DARWIN_BASED
    int ret;

    tzid = getenv("TZFILE");
    if (tzid != nullptr) {
        return tzid;
    }
#endif*/

/* This code can be temporarily disabled to test tzname resolution later on. */
#अगर_अघोषित DEBUG_TZNAME
#अगर U_PLATFORM == U_PF_ANDROID
    tzid = gAndroidTimeZone;
#अन्यथा
    tzid = दो_पर्या("TZ");
#पूर्ण_अगर
    अगर (tzid != nullptr && isValidOlsonID(tzid)
#अगर U_PLATFORM == U_PF_SOLARIS
    /* Don't misinterpret TZ "localtime" on Solaris as a time zone name. */
        && uprv_strcmp(tzid, TZ_ENV_CHECK) != 0
#पूर्ण_अगर
    ) {
        /* The colon forces tzset() to treat the remainder as zoneinfo path */
        अगर (tzid[0] == ':') {
            tzid++;
        }
        /* This might be a good Olson ID. */
        skipZoneIDPrefix(&tzid);
        वापस tzid;
    }
    /* else U_TZNAME will give a better result. */
#पूर्ण_अगर

#अगर defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    /* Caller must handle threading issues */
    अगर (g_Time_Zone_बफर_Ptr == nullptr) {
        /*
        This is a trick to look at the name of the link to get the Olson ID
        because the tzfile contents is underspecified.
        This isn't guaranteed to work because it may not be a symlink.
        */
        अक्षर *ret = realpath(TZDEFAULT, g_Time_Zone_बफर);
        अगर (ret != nullptr && uprv_strcmp(TZDEFAULT, g_Time_Zone_बफर) != 0) {
            int32_t tz_Zone_Info_पुच्छ_Len = uprv_strlen(TZZONEINFOTAIL);
            स्थिर अक्षर *tz_Zone_Info_पुच्छ_Ptr = uprv_strstr(g_Time_Zone_बफर, TZZONEINFOTAIL);
            // MacOS14 has the realpath as something like
            // /usr/share/zoneinfo.default/Australia/Melbourne
            // which will not have "/zoneinfo/" in the path.
            // Therefore if we fail, we fall back to read the link which is
            // /var/db/timezone/zoneinfo/Australia/Melbourne
            // We also fall back to reading the link if the realpath leads to something like
            // /usr/share/zoneinfo/posixrules
            अगर (tz_Zone_Info_पुच्छ_Ptr == nullptr ||
                    uprv_strcmp(tz_Zone_Info_पुच्छ_Ptr + tz_Zone_Info_पुच्छ_Len, "posixrules") == 0) {
                ssize_t size = readlink(TZDEFAULT, g_Time_Zone_बफर, माप(g_Time_Zone_बफर)-1);
                अगर (size > 0) {
                    g_Time_Zone_बफर[size] = 0;
                    tz_Zone_Info_पुच्छ_Ptr = uprv_strstr(g_Time_Zone_बफर, TZZONEINFOTAIL);
                }
            }
            अगर (tz_Zone_Info_पुच्छ_Ptr != nullptr) {
                tz_Zone_Info_पुच्छ_Ptr += tz_Zone_Info_पुच्छ_Len;
                skipZoneIDPrefix(&tz_Zone_Info_पुच्छ_Ptr);
                अगर (isValidOlsonID(tz_Zone_Info_पुच्छ_Ptr)) {
                    वापस (g_Time_Zone_बफर_Ptr = tz_Zone_Info_पुच्छ_Ptr);
                }
            }
        } अन्यथा {
#अगर defined(खोज_TZFILE)
            DefaultTZInfo* tzInfo = (DefaultTZInfo*)uprv_malloc(माप(DefaultTZInfo));
            अगर (tzInfo != nullptr) {
                tzInfo->defaultTZBuffer = nullptr;
                tzInfo->default_TZFile_आकार = 0;
                tzInfo->defaultTZFilePtr = ख_खोलो(TZDEFAULT, "r");
                tzInfo->defaultTZPosition = 0;

                g_Time_Zone_बफर_Ptr = खोज_For_TZFile(TZZONEINFO, tzInfo);

                /* Free previously allocated memory */
                अगर (tzInfo->defaultTZBuffer != nullptr) {
                    uprv_free(tzInfo->defaultTZBuffer);
                }
                अगर (tzInfo->defaultTZFilePtr != nullptr) {
                    ख_बंद(tzInfo->defaultTZFilePtr);
                }
                uprv_free(tzInfo);
            }

            अगर (g_Time_Zone_बफर_Ptr != nullptr && isValidOlsonID(g_Time_Zone_बफर_Ptr)) {
                वापस g_Time_Zone_बफर_Ptr;
            }
#पूर्ण_अगर
        }
    }
    अन्यथा {
        वापस g_Time_Zone_बफर_Ptr;
    }
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_घोषित U_TZNAME
#अगर U_PLATFORM_USES_ONLY_WIN32_API
    /* The return value is free'd in timezone.cpp on Windows because
     * the other code path returns a pointer to a heap location. */
    वापस uprv_strdup(U_TZNAME[n]);
#अन्यथा
    /*
    U_TZNAME is usually a non-unique abbreviation, which isn't normally usable.
    So we remap the abbreviation to an olson ID.

    Since Windows exposes a little more timezone information,
    we normally don't use this code on Windows because
    uprv_detectWindowsTimeZone should have already given the correct answer.
    */
    {
        काष्ठा पंचांग juneSol, decemberSol;
        पूर्णांक daylightType;
        अटल स्थिर समय_प्रकार juneSolstice=1182478260; /*2007-06-21 18:11 UT*/
        अटल स्थिर समय_प्रकार decemberSolstice=1198332540; /*2007-12-22 06:09 UT*/

        /* This probing will tell us when daylight savings occurs.  */
        localtime_r(&juneSolstice, &juneSol);
        localtime_r(&decemberSolstice, &decemberSol);
        अगर(decemberSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_DECEMBER;
        } अन्यथा अगर(juneSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_JUNE;
        } अन्यथा {
          daylightType = U_DAYLIGHT_NONE;
        }
        tzid = remapShortTimeZone(U_TZNAME[0], U_TZNAME[1], daylightType, uprv_timezone());
        अगर (tzid != nullptr) {
            वापस tzid;
        }
    }
    वापस U_TZNAME[n];
#पूर्ण_अगर
#अन्यथा
    वापस "";
#पूर्ण_अगर
}

/* Get and set the ICU data directory --------------------------------------- */

अटल icu::UInitOnce g_तथ्य_Dir_Init_Once {};
अटल अक्षर *g_तथ्य_Directory = nullptr;

UInitOnce gTimeZoneFilesInitOnce {};
अटल CharString *gTimeZoneFilesDirectory = nullptr;

#अगर U_POSIX_LOCALE || U_PLATFORM_USES_ONLY_WIN32_API
 अटल स्थिर अक्षर *gCorrectedPOSIXLocale = nullptr; /* Sometimes heap allocated */
 अटल bool gCorrectedPOSIXLocaleHeapAllocated = false;
#पूर्ण_अगर

अटल UBool U_CALLCONV putil_cleanup()
{
    अगर (g_तथ्य_Directory && *g_तथ्य_Directory) {
        uprv_free(g_तथ्य_Directory);
    }
    g_तथ्य_Directory = nullptr;
    g_तथ्य_Dir_Init_Once.reset();

    मिटाओ gTimeZoneFilesDirectory;
    gTimeZoneFilesDirectory = nullptr;
    gTimeZoneFilesInitOnce.reset();

#अगर_घोषित खोज_TZFILE
    मिटाओ g_खोज_TZFile_परिणाम;
    g_खोज_TZFile_परिणाम = nullptr;
#पूर्ण_अगर

#अगर U_POSIX_LOCALE || U_PLATFORM_USES_ONLY_WIN32_API
    अगर (gCorrectedPOSIXLocale && gCorrectedPOSIXLocaleHeapAllocated) {
        uprv_free(const_cast<अक्षर *>(gCorrectedPOSIXLocale));
        gCorrectedPOSIXLocale = nullptr;
        gCorrectedPOSIXLocaleHeapAllocated = false;
    }
#पूर्ण_अगर
    वापस true;
}

/*
 * Set the data directory.
 *    Make a copy of the passed string, and set the global data dir to point to it.
 */
U_CAPI व्योम U_EXPORT2
u_set_तथ्य_Directory(स्थिर अक्षर *directory) {
    अक्षर *new_तथ्य_Dir;
    int32_t length;

    अगर(directory==nullptr || *directory==0) {
        /* A small optimization to prevent the malloc and copy when the
        shared library is used, and this is a way to make sure that nullptr
        is never returned.
        */
        new_तथ्य_Dir = (अक्षर *)"";
    }
    अन्यथा {
        length=(int32_t)uprv_strlen(directory);
        new_तथ्य_Dir = (अक्षर *)uprv_malloc(length + 2);
        /* Exit out if newDataDir could not be created. */
        अगर (new_तथ्य_Dir == nullptr) {
            वापस;
        }
        uprv_strcpy(new_तथ्य_Dir, directory);

#अगर (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
        {
            अक्षर *p;
            जबतक((p = uprv_strchr(new_तथ्य_Dir, U_FILE_ALT_SEP_CHAR)) != nullptr) {
                *p = U_FILE_SEP_CHAR;
            }
        }
#पूर्ण_अगर
    }

    अगर (g_तथ्य_Directory && *g_तथ्य_Directory) {
        uprv_free(g_तथ्य_Directory);
    }
    g_तथ्य_Directory = new_तथ्य_Dir;
    ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
}

U_CAPI UBool U_EXPORT2
uprv_pathIsAbsolute(स्थिर अक्षर *path)
{
  अगर(!path || !*path) {
    वापस false;
  }

  अगर(*path == U_FILE_SEP_CHAR) {
    वापस true;
  }

#अगर (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
  अगर(*path == U_FILE_ALT_SEP_CHAR) {
    वापस true;
  }
#पूर्ण_अगर

#अगर U_PLATFORM_USES_ONLY_WIN32_API
  अगर( (((path[0] >= 'A') && (path[0] <= 'Z')) ||
       ((path[0] >= 'a') && (path[0] <= 'z'))) &&
      path[1] == ':' ) {
    वापस true;
  }
#पूर्ण_अगर

  वापस false;
}

/* Backup setting of ICU_DATA_DIR_PREFIX_ENV_VAR
   (needed for some Darwin ICU build environments) */
#अगर U_PLATFORM_IS_DARWIN_BASED && defined(TARGET_OS_SIMULATOR) && TARGET_OS_SIMULATOR
# अगर !defined(ICU_तथ्य_DIR_PREFIX_ENV_VAR)
#  define ICU_तथ्य_DIR_PREFIX_ENV_VAR "IPHONE_SIMULATOR_ROOT"
# endif
#पूर्ण_अगर

#अगर defined(ICU_तथ्य_DIR_WINDOWS)
// Helper function to get the ICU Data Directory under the Windows directory location.
अटल BOOL U_CALLCONV get_Icu_तथ्य_Directory_Under_Windows_Directory(अक्षर* directory_बफर, UINT बफर_लंबाई)
{
    ब_अक्षर_प्रकार windowsPath[MAX_PATH];
    अक्षर windowsPathUtf8[MAX_PATH];

    UINT length = GetSystemWindowsDirectoryW(windowsPath, UPRV_LENGTHOF(windowsPath));
    अगर ((length > 0) && (length < (UPRV_LENGTHOF(windowsPath) - 1))) {
        // Convert UTF-16 to a UTF-8 string.
        UErrorCode status = U_ZERO_ERROR;
        int32_t windowsPathUtf8Len = 0;
        u_strToUTF8(windowsPathUtf8, static_cast<int32_t>(UPRV_LENGTHOF(windowsPathUtf8)),
            &windowsPathUtf8Len, reinterpret_cast<स्थिर char16_t*>(windowsPath), -1, &status);

        अगर (U_SUCCESS(status) && (status != U_STRING_NOT_TERMINATED_WARNING) &&
            (windowsPathUtf8Len < (UPRV_LENGTHOF(windowsPathUtf8) - 1))) {
            // Ensure it always has a separator, so we can append the ICU data path.
            अगर (windowsPathUtf8[windowsPathUtf8Len - 1] != U_FILE_SEP_CHAR) {
                windowsPathUtf8[windowsPathUtf8Len++] = U_FILE_SEP_CHAR;
                windowsPathUtf8[windowsPathUtf8Len] = '\0';
            }
            // Check if the concatenated string will fit.
            अगर ((windowsPathUtf8Len + UPRV_LENGTHOF(ICU_तथ्य_DIR_WINDOWS)) < बफर_लंबाई) {
                uprv_strcpy(directory_बफर, windowsPathUtf8);
                uprv_strcat(directory_बफर, ICU_तथ्य_DIR_WINDOWS);
                वापस true;
            }
        }
    }

    वापस false;
}
#पूर्ण_अगर

अटल व्योम U_CALLCONV तथ्य_Directory_Init_Fn() {
    /* If we already have the directory, then return immediately. Will happen if user called
     * u_setDataDirectory().
     */
    अगर (g_तथ्य_Directory) {
        वापस;
    }

    स्थिर अक्षर *path = nullptr;

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
#   अगर !defined(ICU_NO_USER_तथ्य_OVERRIDE) && !UCONFIG_NO_FILE_IO
    /* First try to get the environment variable */
#     अगर U_PLATFORM_HAS_WINUWP_API == 0  // Windows UWP does not support getenv
        path=दो_पर्या("ICU_DATA");
#     endif
#   endif

    /* ICU_DATA_DIR may be set as a compile option.
     * U_ICU_DATA_DEFAULT_DIR is provided and is set by ICU at compile time
     * and is used only when data is built in archive mode eliminating the need
     * for ICU_DATA_DIR to be set. U_ICU_DATA_DEFAULT_DIR is set to the installation
     * directory of the data dat file. Users should use ICU_DATA_DIR if they want to
     * set their own path.
     */
#अगर defined(ICU_तथ्य_DIR) || defined(U_ICU_तथ्य_DEFAULT_DIR)
# अगर defined(ICU_तथ्य_DIR_PREFIX_ENV_VAR)
    अक्षर datadir_path_बफर[PATH_MAX];
# endif
    अगर(path==nullptr || *path==0) {
# अगर defined(ICU_तथ्य_DIR_PREFIX_ENV_VAR)
        स्थिर अक्षर *prefix = दो_पर्या(ICU_तथ्य_DIR_PREFIX_ENV_VAR);
# endif
# ifdef ICU_तथ्य_DIR
        path=ICU_तथ्य_DIR;
# अन्यथा
        path=U_ICU_तथ्य_DEFAULT_DIR;
# endif
# अगर defined(ICU_तथ्य_DIR_PREFIX_ENV_VAR)
        अगर (prefix != nullptr) {
            snprintf(datadir_path_बफर, माप(datadir_path_बफर), "%s%s", prefix, path);
            path=datadir_path_बफर;
        }
# endif
    }
#पूर्ण_अगर

#अगर defined(ICU_तथ्य_DIR_WINDOWS)
    अक्षर datadir_path_बफर[MAX_PATH];
    अगर (get_Icu_तथ्य_Directory_Under_Windows_Directory(datadir_path_बफर, UPRV_LENGTHOF(datadir_path_बफर))) {
        path = datadir_path_बफर;
    }
#पूर्ण_अगर

    अगर(path==nullptr) {
        /* It looks really bad, set it to something. */
        path = "";
    }

    u_set_तथ्य_Directory(path);
}

U_CAPI स्थिर अक्षर * U_EXPORT2
u_get_तथ्य_Directory() {
    umtx_initOnce(g_तथ्य_Dir_Init_Once, &तथ्य_Directory_Init_Fn);
    वापस g_तथ्य_Directory;
}

अटल व्योम setTimeZoneFilesDir(स्थिर अक्षर *path, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस;
    }
    gTimeZoneFilesDirectory->सफा();
    gTimeZoneFilesDirectory->append(path, status);
#अगर (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    अक्षर *p = gTimeZoneFilesDirectory->तथ्य();
    जबतक ((p = uprv_strchr(p, U_FILE_ALT_SEP_CHAR)) != nullptr) {
        *p = U_FILE_SEP_CHAR;
    }
#पूर्ण_अगर
}

#घोषणा TO_STRING(x) TO_STRING_2(x)
#घोषणा TO_STRING_2(x) #x

अटल व्योम U_CALLCONV Time_Zone_तथ्य_Dir_Init_Fn(UErrorCode &status) {
    U_ASSERT(gTimeZoneFilesDirectory == nullptr);
    ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    gTimeZoneFilesDirectory = नया CharString();
    अगर (gTimeZoneFilesDirectory == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    स्थिर अक्षर *dir = "";

#अगर defined(ICU_TIMEZONE_FILES_DIR_PREFIX_ENV_VAR)
    अक्षर timezonefilesdir_path_बफर[PATH_MAX];
    स्थिर अक्षर *prefix = दो_पर्या(ICU_TIMEZONE_FILES_DIR_PREFIX_ENV_VAR);
#पूर्ण_अगर

#अगर U_PLATFORM_HAS_WINUWP_API == 1
// The UWP version does not support the environment variable setting.

# अगर defined(ICU_तथ्य_DIR_WINDOWS)
    // When using the Windows system data, we can possibly pick up time zone data from the Windows directory.
    अक्षर datadir_path_बफर[MAX_PATH];
    अगर (get_Icu_तथ्य_Directory_Under_Windows_Directory(datadir_path_बफर, UPRV_LENGTHOF(datadir_path_बफर))) {
        dir = datadir_path_बफर;
    }
# endif

#अन्यथा
    dir = दो_पर्या("ICU_TIMEZONE_FILES_DIR");
#पूर्ण_अगर // U_PLATFORM_HAS_WINUWP_API

#अगर defined(U_TIMEZONE_FILES_DIR)
    अगर (dir == nullptr) {
        // Build time configuration setting.
        dir = TO_STRING(U_TIMEZONE_FILES_DIR);
    }
#पूर्ण_अगर

    अगर (dir == nullptr) {
        dir = "";
    }

#अगर defined(ICU_TIMEZONE_FILES_DIR_PREFIX_ENV_VAR)
    अगर (prefix != nullptr) {
        snprintf(timezonefilesdir_path_बफर, माप(timezonefilesdir_path_बफर), "%s%s", prefix, dir);
        dir = timezonefilesdir_path_बफर;
    }
#पूर्ण_अगर

    setTimeZoneFilesDir(dir, status);
}


U_CAPI स्थिर अक्षर * U_EXPORT2
u_getTimeZoneFilesDirectory(UErrorCode *status) {
    umtx_initOnce(gTimeZoneFilesInitOnce, &Time_Zone_तथ्य_Dir_Init_Fn, *status);
    वापस U_SUCCESS(*status) ? gTimeZoneFilesDirectory->तथ्य() : "";
}

U_CAPI व्योम U_EXPORT2
u_setTimeZoneFilesDirectory(स्थिर अक्षर *path, UErrorCode *status) {
    umtx_initOnce(gTimeZoneFilesInitOnce, &Time_Zone_तथ्य_Dir_Init_Fn, *status);
    setTimeZoneFilesDir(path, *status);

    // Note: this function does some extra churn, first setting based on the
    //       environment, then immediately replacing with the value passed in.
    //       The logic is simpler that way, and performance shouldn't be an issue.
}


#अगर U_POSIX_LOCALE
/* A helper function used by uprv_getPOSIXIDForDefaultLocale and
 * uprv_getPOSIXIDForDefaultCodepage. Returns the posix locale id for
 * LC_CTYPE and LC_MESSAGES. It doesn't support other locale categories.
 */
अटल स्थिर अक्षर *uprv_getPOSIXIDForCategory(पूर्णांक category)
{
    स्थिर अक्षर* posixID = nullptr;
    अगर (category == LC_MESSAGES || category == LC_CTYPE) {
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
        posixID = रखो_क्षेत्र(category, nullptr);
        अगर ((posixID == nullptr)
            || (uprv_strcmp("C", posixID) == 0)
            || (uprv_strcmp("POSIX", posixID) == 0))
        {
            /* Maybe we got some garbage.  Try something more reasonable */
            posixID = दो_पर्या("LC_ALL");
            /* Solaris speaks POSIX -  See IEEE Std 1003.1-2008
             * This is needed to properly handle empty env. variables
             */
#अगर U_PLATFORM == U_PF_SOLARIS
            अगर ((posixID == 0) || (posixID[0] == '\0')) {
                posixID = दो_पर्या(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                अगर ((posixID == 0) || (posixID[0] == '\0')) {
#अन्यथा
            अगर (posixID == nullptr) {
                posixID = दो_पर्या(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                अगर (posixID == nullptr) {
#पूर्ण_अगर
                    posixID = दो_पर्या("LANG");
                }
            }
        }
    }
    अगर ((posixID == nullptr)
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
    वापस posixID;
}

/* Return just the POSIX id for the default locale, whatever happens to be in
 * it. It gets the value from LC_MESSAGES and indirectly from LC_ALL and LANG.
 */
अटल स्थिर अक्षर *uprv_getPOSIXIDForDefaultLocale()
{
    अटल स्थिर अक्षर* posixID = nullptr;
    अगर (posixID == nullptr) {
        posixID = uprv_getPOSIXIDForCategory(LC_MESSAGES);
    }
    वापस posixID;
}

#अगर !U_CHARSET_IS_UTF8
/* Return just the POSIX id for the default codepage, whatever happens to be in
 * it. It gets the value from LC_CTYPE and indirectly from LC_ALL and LANG.
 */
अटल स्थिर अक्षर *uprv_getPOSIXIDForDefaultCodepage()
{
    अटल स्थिर अक्षर* posixID = nullptr;
    अगर (posixID == 0) {
        posixID = uprv_getPOSIXIDForCategory(LC_CTYPE);
    }
    वापस posixID;
}
#पूर्ण_अगर
#पूर्ण_अगर

/* NOTE: The caller should handle thread safety */
U_CAPI स्थिर अक्षर* U_EXPORT2
uprv_getDefaultLocaleID()
{
#अगर U_POSIX_LOCALE
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
    स्थिर अक्षर* posixID = uprv_getPOSIXIDForDefaultLocale();

    /* Format: (no spaces)
    ll [ _CC ] [ . MM ] [ @ VV]

      l = lang, C = ctry, M = charmap, V = variant
    */

    अगर (gCorrectedPOSIXLocale != nullptr) {
        वापस gCorrectedPOSIXLocale;
    }

    // Copy the ID into owned memory.
    // Over-allocate in case we replace "C" with "en_US_POSIX" (+10), + null termination
    अक्षर *correctedPOSIXLocale = static_cast<अक्षर *>(uprv_malloc(uprv_strlen(posixID) + 10 + 1));
    अगर (correctedPOSIXLocale == nullptr) {
        वापस nullptr;
    }
    uprv_strcpy(correctedPOSIXLocale, posixID);

    अक्षर *limit;
    अगर ((limit = uprv_strchr(correctedPOSIXLocale, '.')) != nullptr) {
        *limit = 0;
    }
    अगर ((limit = uprv_strchr(correctedPOSIXLocale, '@')) != nullptr) {
        *limit = 0;
    }

    अगर ((uprv_strcmp("C", correctedPOSIXLocale) == 0) // no @ variant
        || (uprv_strcmp("POSIX", correctedPOSIXLocale) == 0)) {
      // Raw input was C.* or POSIX.*, Give it a nice POSIX default value.
      // (The "C"/"POSIX" case is handled in uprv_getPOSIXIDForCategory())
      uprv_strcpy(correctedPOSIXLocale, "en_US_POSIX");
    }

    /* Note that we scan the *uncorrected* ID. */
    स्थिर अक्षर *p;
    अगर ((p = uprv_strrchr(posixID, '@')) != nullptr) {
        p++;

        /* Take care of any special cases here.. */
        अगर (!uprv_strcmp(p, "nynorsk")) {
            p = "NY";
            /* Don't worry about no__NY. In practice, it won't appear. */
        }

        अगर (uprv_strchr(correctedPOSIXLocale,'_') == nullptr) {
            uprv_strcat(correctedPOSIXLocale, "__"); /* aa@b -> aa__b (note this can make the new locale 1 char longer) */
        }
        अन्यथा {
            uprv_strcat(correctedPOSIXLocale, "_"); /* aa_CC@b -> aa_CC_b */
        }

        स्थिर अक्षर *q;
        अगर ((q = uprv_strchr(p, '.')) != nullptr) {
            /* How big will the resulting string be? */
            int32_t len = (int32_t)(uprv_strlen(correctedPOSIXLocale) + (q-p));
            uprv_strncat(correctedPOSIXLocale, p, q-p); // do not include charset
            correctedPOSIXLocale[len] = 0;
        }
        अन्यथा {
            /* Anything following the @ sign */
            uprv_strcat(correctedPOSIXLocale, p);
        }

        /* Should there be a map from 'no@nynorsk' -> no_NO_NY here?
         * How about 'russian' -> 'ru'?
         * Many of the other locales using ISO codes will be handled by the
         * canonicalization functions in uloc_getDefault.
         */
    }

    अगर (gCorrectedPOSIXLocale == nullptr) {
        gCorrectedPOSIXLocale = correctedPOSIXLocale;
        gCorrectedPOSIXLocaleHeapAllocated = true;
        ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
        correctedPOSIXLocale = nullptr;
    }
    posixID = gCorrectedPOSIXLocale;

    अगर (correctedPOSIXLocale != nullptr) {  /* Was already set - clean up. */
        uprv_free(correctedPOSIXLocale);
    }

    वापस posixID;

#या_अगर U_PLATFORM_USES_ONLY_WIN32_API
#घोषणा POSIX_LOCALE_CAPACITY 64
    UErrorCode status = U_ZERO_ERROR;
    अक्षर *correctedPOSIXLocale = nullptr;

    // If we have already figured this out just use the cached value
    अगर (gCorrectedPOSIXLocale != nullptr) {
        वापस gCorrectedPOSIXLocale;
    }

    // No cached value, need to determine the current value
    अटल WCHAR windowsLocale[LOCALE_NAME_MAX_लंबाई] = {};
    पूर्णांक length = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, windowsLocale, LOCALE_NAME_MAX_लंबाई);

    // Now we should have a Windows locale name that needs converted to the POSIX style.
    अगर (length > 0) // If length is 0, then the GetLocaleInfoEx failed.
    {
        // First we need to go from UTF-16 to char (and also convert from _ to - while we're at it.)
        अक्षर modifiedWindowsLocale[LOCALE_NAME_MAX_लंबाई] = {};

        int32_t i;
        क्रम (i = 0; i < UPRV_LENGTHOF(modifiedWindowsLocale); i++)
        {
            अगर (windowsLocale[i] == '_')
            {
                modifiedWindowsLocale[i] = '-';
            }
            अन्यथा
            {
                modifiedWindowsLocale[i] = static_cast<अक्षर>(windowsLocale[i]);
            }

            अगर (modifiedWindowsLocale[i] == '\0')
            {
                अवरोध;
            }
        }

        अगर (i >= UPRV_LENGTHOF(modifiedWindowsLocale))
        {
            // Ran out of room, can't really happen, maybe we'll be lucky about a matching
            // locale when tags are dropped
            modifiedWindowsLocale[UPRV_LENGTHOF(modifiedWindowsLocale) - 1] = '\0';
        }

        // Now normalize the resulting name
        correctedPOSIXLocale = static_cast<अक्षर *>(uprv_malloc(POSIX_LOCALE_CAPACITY + 1));
        /* TODO: Should we just exit on memory allocation failure? */
        अगर (correctedPOSIXLocale)
        {
            int32_t posixLen = uloc_canonicalize(modifiedWindowsLocale, correctedPOSIXLocale, POSIX_LOCALE_CAPACITY, &status);
            अगर (U_SUCCESS(status))
            {
                *(correctedPOSIXLocale + posixLen) = 0;
                gCorrectedPOSIXLocale = correctedPOSIXLocale;
                gCorrectedPOSIXLocaleHeapAllocated = true;
                ucln_common_registerCleanup(UCLN_COMMON_PUTIL, putil_cleanup);
            }
            अन्यथा
            {
                uprv_free(correctedPOSIXLocale);
            }
        }
    }

    // If unable to find a locale we can agree upon, use en-US by default
    अगर (gCorrectedPOSIXLocale == nullptr) {
        gCorrectedPOSIXLocale = "en_US";
    }
    वापस gCorrectedPOSIXLocale;

#या_अगर U_PLATFORM == U_PF_OS400
    /* locales are process scoped and are by definition thread safe */
    अटल अक्षर correctedLocale[64];
    स्थिर  अक्षर *localeID = दो_पर्या("LC_ALL");
           अक्षर *p;

    अगर (localeID == nullptr)
        localeID = दो_पर्या("LANG");
    अगर (localeID == nullptr)
        localeID = रखो_क्षेत्र(LC_ALL, nullptr);
    /* Make sure we have something... */
    अगर (localeID == nullptr)
        वापस "en_US_POSIX";

    /* Extract the locale name from the path. */
    अगर((p = uprv_strrchr(localeID, '/')) != nullptr)
    {
        /* Increment p to start of locale name. */
        p++;
        localeID = p;
    }

    /* Copy to work location. */
    uprv_strcpy(correctedLocale, localeID);

    /* Strip off the '.locale' extension. */
    अगर((p = uprv_strchr(correctedLocale, '.')) != nullptr) {
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
    अगर ((uprv_strcmp("C", correctedLocale) == 0) ||
        (uprv_strcmp("POSIX", correctedLocale) == 0) ||
        (uprv_strncmp("QLGPGCMA", correctedLocale, 8) == 0))
    {
        uprv_strcpy(correctedLocale, "en_US_POSIX");
    }
    अन्यथा
    {
        int16_t LocaleLen;

        /* Lower case the lang portion. */
        क्रम(p = correctedLocale; *p != 0 && *p != '_'; p++)
        {
            *p = uprv_tolower(*p);
        }

        /* Adjust for Euro.  After '_E' add 'URO'. */
        LocaleLen = uprv_strlen(correctedLocale);
        अगर (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'E')
        {
            uprv_strcat(correctedLocale, "URO");
        }

        /* If using Lotus-based locale then convert to
         * equivalent non Lotus.
         */
        अन्यथा अगर (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'L')
        {
            correctedLocale[LocaleLen - 2] = 0;
        }

        /* There are separate simplified and traditional
         * locales called zh_HK_S and zh_HK_T.
         */
        अन्यथा अगर (uprv_strncmp(correctedLocale, "zh_HK", 5) == 0)
        {
            uprv_strcpy(correctedLocale, "zh_HK");
        }

        /* A special zh_CN_GBK locale...
        */
        अन्यथा अगर (uprv_strcmp(correctedLocale, "zh_CN_GBK") == 0)
        {
            uprv_strcpy(correctedLocale, "zh_CN");
        }

    }

    वापस correctedLocale;
#पूर्ण_अगर

}

#अगर !U_CHARSET_IS_UTF8
#अगर U_POSIX_LOCALE
/*
Due to various platform differences, one platform may specify a charset,
when they really mean a different charset. Remap the names so that they are
compatible with ICU. Only conflicting/ambiguous aliases should be resolved
here. Before adding anything to this function, please consider adding unique
names to the ICU alias table in the data directory.
*/
अटल स्थिर अक्षर*
remapPlatformDependentCodepage(स्थिर अक्षर *locale, स्थिर अक्षर *name) {
    अगर (locale != nullptr && *locale == 0) {
        /* Make sure that an empty locale is handled the same way. */
        locale = nullptr;
    }
    अगर (name == nullptr) {
        वापस nullptr;
    }
#अगर U_PLATFORM == U_PF_AIX
    अगर (uprv_strcmp(name, "IBM-943") == 0) {
        /* Use the ASCII compatible ibm-943 */
        name = "Shift-JIS";
    }
    अन्यथा अगर (uprv_strcmp(name, "IBM-1252") == 0) {
        /* Use the windows-1252 that contains the Euro */
        name = "IBM-5348";
    }
#या_अगर U_PLATFORM == U_PF_SOLARIS
    अगर (locale != nullptr && uprv_strcmp(name, "EUC") == 0) {
        /* Solaris underspecifies the "EUC" name. */
        अगर (uprv_strcmp(locale, "zh_CN") == 0) {
            name = "EUC-CN";
        }
        अन्यथा अगर (uprv_strcmp(locale, "zh_TW") == 0) {
            name = "EUC-TW";
        }
        अन्यथा अगर (uprv_strcmp(locale, "ko_KR") == 0) {
            name = "EUC-KR";
        }
    }
    अन्यथा अगर (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-954 is the best match.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    अन्यथा अगर (uprv_strcmp(name, "646") == 0) {
        /*
         * The default codepage given by Solaris is 646 but the C library routines treat it as if it was
         * ISO-8859-1 instead of US-ASCII(646).
         */
        name = "ISO-8859-1";
    }
#या_अगर U_PLATFORM_IS_DARWIN_BASED
    अगर (locale == nullptr && *name == 0) {
        /*
        No locale was specified, and an empty name was passed in.
        This usually indicates that nl_langinfo didn't return valid information.
        Mac OS X uses UTF-8 by default (especially the locale data and console).
        */
        name = "UTF-8";
    }
    अन्यथा अगर (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
    अन्यथा अगर (locale != nullptr && uprv_strcmp(locale, "en_US_POSIX") != 0 && uprv_strcmp(name, "US-ASCII") == 0) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
#या_अगर U_PLATFORM == U_PF_BSD
    अगर (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
#या_अगर U_PLATFORM == U_PF_HPUX
    अगर (locale != nullptr && uprv_strcmp(locale, "zh_HK") == 0 && uprv_strcmp(name, "big5") == 0) {
        /* HP decided to extend big5 as hkbig5 even though it's not compatible :-( */
        /* zh_TW.big5 is not the same charset as zh_HK.big5! */
        name = "hkbig5";
    }
    अन्यथा अगर (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
#या_अगर U_PLATFORM == U_PF_LINUX
    अगर (locale != nullptr && uprv_strcmp(name, "euc") == 0) {
        /* Linux underspecifies the "EUC" name. */
        अगर (uprv_strcmp(locale, "korean") == 0) {
            name = "EUC-KR";
        }
        अन्यथा अगर (uprv_strcmp(locale, "japanese") == 0) {
            /* See comment below about eucJP */
            name = "eucjis";
        }
    }
    अन्यथा अगर (uprv_strcmp(name, "eucjp") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    अन्यथा अगर (locale != nullptr && uprv_strcmp(locale, "en_US_POSIX") != 0 &&
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
#पूर्ण_अगर
    /* return nullptr when "" is passed in */
    अगर (*name == 0) {
        name = nullptr;
    }
    वापस name;
}

अटल स्थिर अक्षर*
getCodepageFromPOSIXID(स्थिर अक्षर *localeName, अक्षर * बफर, int32_t buffCapacity)
{
    अक्षर localeBuf[100];
    स्थिर अक्षर *name = nullptr;
    अक्षर *variant = nullptr;

    अगर (localeName != nullptr && (name = (uprv_strchr(localeName, '.'))) != nullptr) {
        माप_प्रकार localeCapacity = uprv_min(माप(localeBuf), (name-localeName)+1);
        uprv_strncpy(localeBuf, localeName, localeCapacity);
        localeBuf[localeCapacity-1] = 0; /* ensure NUL termination */
        name = uprv_strncpy(बफर, name+1, buffCapacity);
        बफर[buffCapacity-1] = 0; /* ensure NUL termination */
        अगर ((variant = const_cast<अक्षर *>(uprv_strchr(name, '@'))) != nullptr) {
            *variant = 0;
        }
        name = remapPlatformDependentCodepage(localeBuf, name);
    }
    वापस name;
}
#पूर्ण_अगर

अटल स्थिर अक्षर*
int_getDefaultCodepage()
{
#अगर U_PLATFORM == U_PF_OS400
    uint32_t ccsid = 37; /* Default to ibm-37 */
    अटल अक्षर codepage[64];
    Qwc_JOBI0400_t jobinfo;
    Qus_EC_t error = { माप(Qus_EC_t) }; /* SPI error code */

    EPT_CALL(QUSRJOBI)(&jobinfo, माप(jobinfo), "JOBI0400",
        "*                         ", "                ", &error);

    अगर (error.Bytes_Available == 0) {
        अगर (jobinfo.Coded_Char_Set_ID != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Coded_Char_Set_ID;
        }
        अन्यथा अगर (jobinfo.Default_Coded_Char_Set_Id != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Default_Coded_Char_Set_Id;
        }
        /* else use the default */
    }
    snprintf(codepage, माप(codepage), "ibm-%d", ccsid);
    वापस codepage;

#या_अगर U_PLATFORM == U_PF_OS390
    अटल अक्षर codepage[64];

    म_नकलन(codepage, nl_langinfo(CODESET),63-म_माप(UCNV_SWAP_LFNL_OPTION_STRING));
    म_जोड़ो(codepage,UCNV_SWAP_LFNL_OPTION_STRING);
    codepage[63] = 0; /* NUL terminate */

    वापस codepage;

#या_अगर U_PLATFORM_USES_ONLY_WIN32_API
    अटल अक्षर codepage[64];
    DWORD codepage_संख्या = 0;

#अगर U_PLATFORM_HAS_WINUWP_API == 1
    // UWP doesn't have a direct API to get the default ACP as Microsoft would rather
    // have folks use Unicode than a "system" code page, however this is the same
    // codepage as the system default locale codepage.  (FWIW, the system locale is
    // ONLY used for codepage, it should never be used for anything else)
    GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_संख्या,
        (LPWSTR)&codepage_संख्या, माप(codepage_संख्या) / माप(WCHAR));
#अन्यथा
    // Win32 apps can call GetACP
    codepage_संख्या = GetACP();
#पूर्ण_अगर
    // Special case for UTF-8
    अगर (codepage_संख्या == 65001)
    {
        वापस "UTF-8";
    }
    // Windows codepages can look like windows-1252, so format the found number
    // the numbers are eclectic, however all valid system code pages, besides UTF-8
    // are between 3 and 19999
    अगर (codepage_संख्या > 0 && codepage_संख्या < 20000)
    {
        snprintf(codepage, माप(codepage), "windows-%ld", codepage_संख्या);
        वापस codepage;
    }
    // If the codepage number call failed then return UTF-8
    वापस "UTF-8";

#या_अगर U_POSIX_LOCALE
    अटल अक्षर codesetName[100];
    स्थिर अक्षर *localeName = nullptr;
    स्थिर अक्षर *name = nullptr;

    localeName = uprv_getPOSIXIDForDefaultCodepage();
    uprv_memset(codesetName, 0, माप(codesetName));
    /* On Solaris nl_langinfo returns C locale values unless setlocale
     * was called earlier.
     */
#अगर (U_HAVE_NL_LANGINFO_CODESET && U_PLATFORM != U_PF_SOLARIS)
    /* When available, check nl_langinfo first because it usually gives more
       useful names. It depends on LC_CTYPE.
       nl_langinfo may use the same buffer as setlocale. */
    {
        स्थिर अक्षर *codeset = nl_langinfo(U_NL_LANGINFO_CODESET);
#अगर U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED
        /*
         * On Linux and MacOSX, ensure that default codepage for non C/POSIX locale is UTF-8
         * instead of ASCII.
         */
        अगर (uprv_strcmp(localeName, "en_US_POSIX") != 0) {
            codeset = remapPlatformDependentCodepage(localeName, codeset);
        } अन्यथा
#पूर्ण_अगर
        {
            codeset = remapPlatformDependentCodepage(nullptr, codeset);
        }

        अगर (codeset != nullptr) {
            uprv_strncpy(codesetName, codeset, माप(codesetName));
            codesetName[माप(codesetName)-1] = 0;
            वापस codesetName;
        }
    }
#पूर्ण_अगर

    /* Use setlocale in a nice way, and then check some environment variables.
       Maybe the application used setlocale already.
    */
    uprv_memset(codesetName, 0, माप(codesetName));
    name = getCodepageFromPOSIXID(localeName, codesetName, माप(codesetName));
    अगर (name) {
        /* if we can find the codeset name from setlocale, return that. */
        वापस name;
    }

    अगर (*codesetName == 0)
    {
        /* Everything failed. Return US ASCII (ISO 646). */
        (व्योम)uprv_strcpy(codesetName, "US-ASCII");
    }
    वापस codesetName;
#अन्यथा
    वापस "US-ASCII";
#पूर्ण_अगर
}


U_CAPI स्थिर अक्षर*  U_EXPORT2
uprv_getDefaultCodepage()
{
    अटल अक्षर स्थिर  *name = nullptr;
    umtx_lock(nullptr);
    अगर (name == nullptr) {
        name = int_getDefaultCodepage();
    }
    umtx_unlock(nullptr);
    वापस name;
}
#पूर्ण_अगर  /* !U_CHARSET_IS_UTF8 */


/* end of platform-specific implementation -------------- */

/* version handling --------------------------------------------------------- */

U_CAPI व्योम U_EXPORT2
u_versionFromString(UVersionInfo versionArray, स्थिर अक्षर *versionString) {
    अक्षर *end;
    uint16_t part=0;

    अगर(versionArray==nullptr) {
        वापस;
    }

    अगर(versionString!=nullptr) {
        क्रम(;;) {
            versionArray[part]=(uint8_t)uprv_strtoul(versionString, &end, 10);
            अगर(end==versionString || ++part==U_MAX_VERSION_लंबाई || *end!=U_VERSION_DELIMITER) {
                अवरोध;
            }
            versionString=end+1;
        }
    }

    जबतक(part<U_MAX_VERSION_लंबाई) {
        versionArray[part++]=0;
    }
}

U_CAPI व्योम U_EXPORT2
u_versionFromUString(UVersionInfo versionArray, स्थिर char16_t *versionString) {
    अगर(versionArray!=nullptr && versionString!=nullptr) {
        अक्षर versionChars[U_MAX_VERSION_STRING_लंबाई+1];
        int32_t len = u_strlen(versionString);
        अगर(len>U_MAX_VERSION_STRING_लंबाई) {
            len = U_MAX_VERSION_STRING_लंबाई;
        }
        u_UCharsToChars(versionString, versionChars, len);
        versionChars[len]=0;
        u_versionFromString(versionArray, versionChars);
    }
}

U_CAPI व्योम U_EXPORT2
u_versionToString(स्थिर UVersionInfo versionArray, अक्षर *versionString) {
    uint16_t count, part;
    uint8_t field;

    अगर(versionString==nullptr) {
        वापस;
    }

    अगर(versionArray==nullptr) {
        versionString[0]=0;
        वापस;
    }

    /* count how many fields need to be written */
    क्रम(count=4; count>0 && versionArray[count-1]==0; --count) {
    }

    अगर(count <= 1) {
        count = 2;
    }

    /* write the first part */
    /* write the decimal field value */
    field=versionArray[0];
    अगर(field>=100) {
        *versionString++=(अक्षर)('0'+field/100);
        field%=100;
    }
    अगर(field>=10) {
        *versionString++=(अक्षर)('0'+field/10);
        field%=10;
    }
    *versionString++=(अक्षर)('0'+field);

    /* write the following parts */
    क्रम(part=1; part<count; ++part) {
        /* write a dot first */
        *versionString++=U_VERSION_DELIMITER;

        /* write the decimal field value */
        field=versionArray[part];
        अगर(field>=100) {
            *versionString++=(अक्षर)('0'+field/100);
            field%=100;
        }
        अगर(field>=10) {
            *versionString++=(अक्षर)('0'+field/10);
            field%=10;
        }
        *versionString++=(अक्षर)('0'+field);
    }

    /* NUL-terminate */
    *versionString=0;
}

U_CAPI व्योम U_EXPORT2
u_getVersion(UVersionInfo versionArray) {
    (व्योम)copyright;   // Suppress unused variable warning from clang.
    u_versionFromString(versionArray, U_ICU_VERSION);
}

/**
 * icucfg.h dependent code
 */

#अगर U_ENABLE_DYLOAD && HAVE_DLOPEN && !U_PLATFORM_USES_ONLY_WIN32_API

#अगर HAVE_DLFCN_H
#अगर_घोषित __MVS__
#अगर_अघोषित __SUSV3
#घोषणा __SUSV3 1
#पूर्ण_अगर
#पूर्ण_अगर
#समावेश <dlfcn.h>
#पूर्ण_अगर /* HAVE_DLFCN_H */

U_CAPI व्योम * U_EXPORT2
uprv_dl_open(स्थिर अक्षर *libName, UErrorCode *status) {
  व्योम *ret = nullptr;
  अगर(U_FAILURE(*status)) वापस ret;
  ret =  dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
  अगर(ret==nullptr) {
#अगर_घोषित U_TRACE_DYLOAD
    म_लिखो("dlerror on dlopen(%s): %s\n", libName, dlerror());
#पूर्ण_अगर
    *status = U_MISSING_RESOURCE_ERROR;
  }
  वापस ret;
}

U_CAPI व्योम U_EXPORT2
uprv_dl_close(व्योम *lib, UErrorCode *status) {
  अगर(U_FAILURE(*status)) वापस;
  dlclose(lib);
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(व्योम *lib, स्थिर अक्षर* sym, UErrorCode *status) {
  जोड़ {
      UVoidFunction *fp;
      व्योम *vp;
  } uret;
  uret.fp = nullptr;
  अगर(U_FAILURE(*status)) वापस uret.fp;
  uret.vp = dlsym(lib, sym);
  अगर(uret.vp == nullptr) {
#अगर_घोषित U_TRACE_DYLOAD
    म_लिखो("dlerror on dlsym(%p,%s): %s\n", lib,sym, dlerror());
#पूर्ण_अगर
    *status = U_MISSING_RESOURCE_ERROR;
  }
  वापस uret.fp;
}

#या_अगर U_ENABLE_DYLOAD && U_PLATFORM_USES_ONLY_WIN32_API && !U_PLATFORM_HAS_WINUWP_API

/* Windows API implementation. */
// Note: UWP does not expose/allow these APIs, so the UWP version gets the null implementation. */

U_CAPI व्योम * U_EXPORT2
uprv_dl_open(स्थिर अक्षर *libName, UErrorCode *status) {
  HMODULE lib = nullptr;

  अगर(U_FAILURE(*status)) वापस nullptr;

  lib = LoadLibraryA(libName);

  अगर(lib==nullptr) {
    *status = U_MISSING_RESOURCE_ERROR;
  }

  वापस (व्योम*)lib;
}

U_CAPI व्योम U_EXPORT2
uprv_dl_close(व्योम *lib, UErrorCode *status) {
  HMODULE handle = (HMODULE)lib;
  अगर(U_FAILURE(*status)) वापस;

  FreeLibrary(handle);

  वापस;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(व्योम *lib, स्थिर अक्षर* sym, UErrorCode *status) {
  HMODULE handle = (HMODULE)lib;
  UVoidFunction* addr = nullptr;

  अगर(U_FAILURE(*status) || lib==nullptr) वापस nullptr;

  addr = (UVoidFunction*)GetProcAddress(handle, sym);

  अगर(addr==nullptr) {
    DWORD lastError = GetLastError();
    अगर(lastError == ERROR_PROC_NOT_FOUND) {
      *status = U_MISSING_RESOURCE_ERROR;
    } अन्यथा {
      *status = U_UNSUPPORTED_ERROR; /* other unknown error. */
    }
  }

  वापस addr;
}

#अन्यथा

/* No dynamic loading, null (nonexistent) implementation. */

U_CAPI व्योम * U_EXPORT2
uprv_dl_open(स्थिर अक्षर *libName, UErrorCode *status) {
    (व्योम)libName;
    अगर(U_FAILURE(*status)) वापस nullptr;
    *status = U_UNSUPPORTED_ERROR;
    वापस nullptr;
}

U_CAPI व्योम U_EXPORT2
uprv_dl_close(व्योम *lib, UErrorCode *status) {
    (व्योम)lib;
    अगर(U_FAILURE(*status)) वापस;
    *status = U_UNSUPPORTED_ERROR;
    वापस;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(व्योम *lib, स्थिर अक्षर* sym, UErrorCode *status) {
  (व्योम)lib;
  (व्योम)sym;
  अगर(U_SUCCESS(*status)) {
    *status = U_UNSUPPORTED_ERROR;
  }
  वापस (UVoidFunction*)nullptr;
}

#पूर्ण_अगर

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

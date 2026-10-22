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
#अन्तर्भाव "uposixdefs.h"

// First, the platform type. Need this for U_PLATFORM.
#अन्तर्भाव "unicode/platform.h"

/*
 * Cygwin with GCC requires inclusion of time.h after the above disabling strict asci mode statement.
 */
#अन्तर्भाव <time.h>

#यदि !U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
#अन्तर्भाव <sys/time.h>
#यदि_अन्तः

/* include the rest of the ICU headers */
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "locmap.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "charstr.h"

/* Include standard headers. */
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <locale.h>
#अन्तर्भाव <float.h>

#यदि_अपरिभाषितम् U_COMMON_IMPLEMENTATION
#दोषः U_COMMON_IMPLEMENTATION not समुच्चयः - must be समुच्चयः प्रति all ICU source files in common/ - see https://unicode-org.github.io/icu/userguide/icu/howtouseicu.html
#यदि_अन्तः


/* include system headers */
#यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
    /*
     * TODO: U_PLATFORM_USES_ONLY_WIN32_API includes MinGW.
     * Should Cygwin be included as well (U_PLATFORM_HAS_WIN32_API)
     * to use native APIs as much as possible?
     */
#यदि_अपरिभाषितम् WIN32_LEAN_AND_माध्यम्
#   define WIN32_LEAN_AND_माध्यम्
#यदि_अन्तः
#   define VC_EXTRALEAN
#   define NOUSER
#   define NOSERVICE
#   define NOIME
#   define NOMCX
#   include <windows.h>
#   include "unicode/uloc.h"
#   include "wintz.h"
#नोचेद्यदि U_PLATFORM == U_PF_OS400
#   include <float.h>
#   include <qusec.h>       /* error code structure */
#   include <qusrjobi.h>
#   include <qliept.h>      /* EPT_CALL macro  - this include must be after all other "QSYSINCs" */
#   include <mih/testptr.h> /* For uprv_maximumPtr */
#नोचेद्यदि U_PLATFORM == U_PF_OS390
#   include "unicode/ucnv.h"   /* Needed for UCNV_SWAP_LFNL_OPTION_STRING */
#नोचेद्यदि U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS
#   include <limits.h>
#   include <unistd.h>
#   यदि U_PLATFORM == U_PF_SOLARIS
#       ifndef _XPG4_2
#           define _XPG4_2
#       endif
#   elif U_PLATFORM == U_PF_ANDROID
#       include <sys/system_properties.h>
#       include <dlfcn.h>
#   endif
#नोचेद्यदि U_PLATFORM == U_PF_QNX
#   include <sys/neutrino.h>
#यदि_अन्तः


/*
 * Only include langinfo.h if we have a way to get the codeset. If we later
 * depend on more feature, we can test on U_HAVE_NL_LANGINFO.
 *
 */

#यदि U_HAVE_NL_LANGINFO_CODESET
#अन्तर्भाव <langinfo.h>
#यदि_अन्तः

/**
 * Simple things (presence of functions, etc) should just go in configure.in and be added to
 * icucfg.h via autoheader.
 */
#यदि U_PLATFORM_IMPLEMENTS_POSIX
#   यदि U_PLATFORM == U_PF_OS400
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
#यदि_अन्तः

U_NAMESPACE_USE

/* Define the extension for data files, again... */
#परिभाषय दत्त_TYPE "dat"

/* Leave this copyright notice here! */
स्थिर ध्रुव वर्ण copyright[] = U_COPYRIGHT_STRING;

/* floating point implementations ------------------------------------------- */

/* We return QNAN rather than SNAN*/
#परिभाषय SIGN 0x80000000U

/* Make it easy to define certain types of constants */
प्रकारनिर्देश सङ्घ {
    int64_t i64; /* This must be defined first in order to allow the initialization to work. This is a C89 feature. */
    द्विगुण d64;
} Bit_आदर्शः_Conversion;
स्थिर ध्रुव Bit_आदर्शः_Conversion gNan = {static_cast<int64_t>(INT64_C(0x7FF8000000000000))};
स्थिर ध्रुव Bit_आदर्शः_Conversion gInf = {static_cast<int64_t>(INT64_C(0x7FF0000000000000))};

/*---------------------------------------------------------------------------
  Platform utilities
  Our general strategy is to assume we're on a POSIX platform.  Platforms which
  are non-POSIX must declare themselves so.  The default POSIX implementation
  will sometimes work for non-POSIX platforms as well (e.g., the NaN-related
  functions).
  ---------------------------------------------------------------------------*/

#यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API || U_PLATFORM == U_PF_OS400
#   undef U_POSIX_LOCALE
#अन्यथा
#   define U_POSIX_LOCALE    1
#यदि_अन्तः

/*
    WARNING! u_topNBytesOfDouble and u_bottomNBytesOfDouble
    can't be properly optimized by the gcc compiler sometimes (i.e. gcc 3.2).
*/
#यदि !IEEE_754
स्थिर वर्ण*
u_शिखरम्_NBytes_सम्बन्धि_Double(द्विगुण* d, पूर्णाङ्क n)
{
#यदि U_IS_BIG_ENDIAN
    प्रत्यागम (वर्ण*)d;
#अन्यथा
    प्रत्यागम (वर्ण*)(d + 1) - n;
#यदि_अन्तः
}

स्थिर वर्ण*
u_bottom_NBytes_सम्बन्धि_Double(द्विगुण* d, पूर्णाङ्क n)
{
#यदि U_IS_BIG_ENDIAN
    प्रत्यागम (वर्ण*)(d + 1) - n;
#अन्यथा
    प्रत्यागम (वर्ण*)d;
#यदि_अन्तः
}
#यदि_अन्तः   /* !IEEE_754 */

#यदि IEEE_754
स्थिर UBool
u_signBit(द्विगुण d) {
    uint8_t hiByte;
#यदि U_IS_BIG_ENDIAN
    hiByte = *(uint8_t *)&d;
#अन्यथा
    hiByte = *(reinterpret_cast<uint8_t*>(&d) + परिमाण(द्विगुण) - 1);
#यदि_अन्तः
    प्रत्यागम (hiByte & 0x80) != 0;
}
#यदि_अन्तः



#यदि defined (U_DEBUG_FAKETIME)
/* Override the clock to test things without having to move the system clock.
 * Assumes POSIX gettimeofday() will function
 */
UDate fakeClock_t0 = 0; /** Time to start the clock from **/
UDate fakeClock_dt = 0; /** Offset (fake time - real time) **/
UBool fake_Clock_समुच्चयः = असत्य; /** True if fake clock has spun up **/

स्थिर UDate getUTCtime_real() {
    संरचना timeval posixTime;
    gettimeofday(&posixTime, शून्यसूचकः);
    प्रत्यागम (UDate)(((int64_t)posixTime.tv_sec * U_MILLIS_प्रत्येकम्_SECOND) + (posixTime.tv_usec/1000));
}

स्थिर UDate getUTCtime_fake() {
    स्थिर UMutex fakeClockMutex;
    umtx_तालकम्(&fakeClockMutex);
    यदि(!fake_Clock_समुच्चयः) {
        UDate real = getUTCtime_real();
        ध्रुव वर्ण *fake_start = getenv("U_FAKETIME_START");
        यदि((fake_start!=शून्यसूचकः) && (fake_start[0]!=0)) {
            वाक्यात्_पठ(fake_start,"%lf",&fakeClock_t0);
            fakeClock_dt = fakeClock_t0 - real;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, so the ICU clock will start at a preset value\n"
                    "env variable U_FAKETIME_START=%.0f (%s) for an offset of %.0f ms from the current time %.0f\n",
                    fakeClock_t0, fake_start, fakeClock_dt, real);
        } अन्यथा {
          fakeClock_dt = 0;
            fprintf(stderr,"U_DEBUG_FAKETIME was set at compile time, but U_FAKETIME_START was not set.\n"
                    "Set U_FAKETIME_START to the number of milliseconds since 1/1/1970 to set the ICU clock.\n");
        }
        fake_Clock_समुच्चयः = सत्य;
    }
    umtx_unlock(&fakeClockMutex);

    प्रत्यागम getUTCtime_real() + fakeClock_dt;
}
#यदि_अन्तः

#यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
प्रकारनिर्देश सङ्घ {
    int64_t int64;
    FILETIME fileTime;
} FileTimeConversion;   /* This is like a ULARGE_INTEGER */

/* Number of 100 nanoseconds from 1/1/1601 to 1/1/1970 */
#परिभाषय EPOCH_BIAS  INT64_C(116444736000000000)
#परिभाषय HECTONANOSECOND_प्रत्येकम्_MILLISECOND   10000

#यदि_अन्तः

/*---------------------------------------------------------------------------
  Universal Implementations
  These are designed to work on all platforms.  Try these, and if they
  don't work on your platform, then special case your platform with new
  implementations.
---------------------------------------------------------------------------*/

U_CAPI UDate U_EXPORT2
uprv_getUTCtime()
{
#यदि defined(U_DEBUG_FAKETIME)
    प्रत्यागम getUTCtime_fake(); /* Hook for overriding the clock */
#अन्यथा
    प्रत्यागम uprv_getRawUTCtime();
#यदि_अन्तः
}

/* Return UTC (GMT) time measured in milliseconds since 0:00 on 1/1/70.*/
U_CAPI UDate U_EXPORT2
uprv_getRawUTCtime()
{
#यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API

    FileTimeConversion winTime;
    GetSystemTimeAsFileTime(&winTime.fileTime);
    प्रत्यागम (UDate)((winTime.int64 - EPOCH_BIAS) / HECTONANOSECOND_प्रत्येकम्_MILLISECOND);
#अन्यथा

#यदि HAVE_GETTIMEOFDAY
    संरचना timeval posixTime;
    gettimeofday(&posixTime, शून्यसूचकः);
    प्रत्यागम (UDate)(((int64_t)posixTime.tv_sec * U_MILLIS_प्रत्येकम्_SECOND) + (posixTime.tv_usec/1000));
#अन्यथा
    time_t epochtime;
    time(&epochtime);
    प्रत्यागम (UDate)epochtime * U_MILLIS_प्रत्येकम्_SECOND;
#यदि_अन्तः

#यदि_अन्तः
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
uprv_isNaN(द्विगुण number)
{
#यदि IEEE_754
    Bit_आदर्शः_Conversion converted_सङ्ख्या;
    converted_सङ्ख्या.d64 = number;
    /* Infinity is 0x7FF0000000000000U. Anything greater than that is a NaN */
    प्रत्यागम (converted_सङ्ख्या.i64 & U_INT64_MAX) > gInf.i64;

#नोचेद्यदि U_PLATFORM == U_PF_OS390
    uint32_t उच्चः_द्वयङ्काः = *(uint32_t*)u_शिखरम्_NBytes_सम्बन्धि_Double(&number,
                        परिमाण(uint32_t));
    uint32_t नीचः_द्वयङ्काः  = *(uint32_t*)u_bottom_NBytes_सम्बन्धि_Double(&number,
                        परिमाण(uint32_t));

    प्रत्यागम ((उच्चः_द्वयङ्काः & 0x7F080000L) == 0x7F080000L) &&
      (नीचः_द्वयङ्काः == 0x00000000L);

#अन्यथा
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    प्रत्यागम number != number;
#यदि_अन्तः
}

U_CAPI UBool U_EXPORT2
uprv_isInfinite(द्विगुण number)
{
#यदि IEEE_754
    Bit_आदर्शः_Conversion converted_सङ्ख्या;
    converted_सङ्ख्या.d64 = number;
    /* Infinity is exactly 0x7FF0000000000000U. */
    प्रत्यागम (converted_सङ्ख्या.i64 & U_INT64_MAX) == gInf.i64;
#नोचेद्यदि U_PLATFORM == U_PF_OS390
    uint32_t उच्चः_द्वयङ्काः = *(uint32_t*)u_शिखरम्_NBytes_सम्बन्धि_Double(&number,
                        परिमाण(uint32_t));
    uint32_t नीचः_द्वयङ्काः  = *(uint32_t*)u_bottom_NBytes_सम्बन्धि_Double(&number,
                        परिमाण(uint32_t));

    प्रत्यागम ((उच्चः_द्वयङ्काः  & ~SIGN) == 0x70FF0000L) && (नीचः_द्वयङ्काः == 0x00000000L);

#अन्यथा
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    प्रत्यागम number == (2.0 * number);
#यदि_अन्तः
}

U_CAPI UBool U_EXPORT2
uprv_is_धनात्मकम्_Infinity(द्विगुण number)
{
#यदि IEEE_754 || U_PLATFORM == U_PF_OS390
    प्रत्यागम number > 0 && uprv_isInfinite(number);
#अन्यथा
    प्रत्यागम uprv_isInfinite(number);
#यदि_अन्तः
}

U_CAPI UBool U_EXPORT2
uprv_isNegativeInfinity(द्विगुण number)
{
#यदि IEEE_754 || U_PLATFORM == U_PF_OS390
    प्रत्यागम number < 0 && uprv_isInfinite(number);

#अन्यथा
    uint32_t उच्चः_द्वयङ्काः = *(uint32_t*)u_शिखरम्_NBytes_सम्बन्धि_Double(&number,
                        परिमाण(uint32_t));
    प्रत्यागम((उच्चः_द्वयङ्काः & SIGN) && uprv_isInfinite(number));

#यदि_अन्तः
}

U_CAPI द्विगुण U_EXPORT2
uprv_getNaN()
{
#यदि IEEE_754 || U_PLATFORM == U_PF_OS390
    प्रत्यागम gNan.d64;
#अन्यथा
    /* If your platform doesn't support IEEE 754 but *does* have an NaN value,*/
    /* you'll need to replace this default implementation with what's correct*/
    /* for your platform.*/
    प्रत्यागम 0.0;
#यदि_अन्तः
}

U_CAPI द्विगुण U_EXPORT2
uprv_getInfinity()
{
#यदि IEEE_754 || U_PLATFORM == U_PF_OS390
    प्रत्यागम gInf.d64;
#अन्यथा
    /* If your platform doesn't support IEEE 754 but *does* have an infinity*/
    /* value, you'll need to replace this default implementation with what's*/
    /* correct for your platform.*/
    प्रत्यागम 0.0;
#यदि_अन्तः
}

U_CAPI द्विगुण U_EXPORT2
uprv_floor(द्विगुण x)
{
    प्रत्यागम floor(x);
}

U_CAPI द्विगुण U_EXPORT2
uprv_ceil(द्विगुण x)
{
    प्रत्यागम ceil(x);
}

U_CAPI द्विगुण U_EXPORT2
uprv_आवर्तनम्(द्विगुण x)
{
    प्रत्यागम uprv_floor(x + 0.5);
}

U_CAPI द्विगुण U_EXPORT2
uprv_fabs(द्विगुण x)
{
    प्रत्यागम fabs(x);
}

U_CAPI द्विगुण U_EXPORT2
uprv_modf(द्विगुण x, द्विगुण* y)
{
    प्रत्यागम modf(x, y);
}

U_CAPI द्विगुण U_EXPORT2
uprv_fmod(द्विगुण x, द्विगुण y)
{
    प्रत्यागम fmod(x, y);
}

U_CAPI द्विगुण U_EXPORT2
uprv_pow(द्विगुण x, द्विगुण y)
{
    /* This is declared as "double pow(double x, double y)" */
    प्रत्यागम घातः(x, y);
}

U_CAPI द्विगुण U_EXPORT2
uprv_pow10(int32_t x)
{
    प्रत्यागम घातः(10.0, (द्विगुण)x);
}

U_CAPI द्विगुण U_EXPORT2
uprv_fmax(द्विगुण x, द्विगुण y)
{
#यदि IEEE_754
    /* first handle NaN*/
    यदि(uprv_isNaN(x) || uprv_isNaN(y))
        प्रत्यागम uprv_getNaN();

    /* check for -0 and 0*/
    यदि(x == 0.0 && y == 0.0 && u_signBit(x))
        प्रत्यागम y;

#यदि_अन्तः

    /* this should work for all flt point w/o NaN and Inf special cases */
    प्रत्यागम (x > y ? x : y);
}

U_CAPI द्विगुण U_EXPORT2
uprv_fmin(द्विगुण x, द्विगुण y)
{
#यदि IEEE_754
    /* first handle NaN*/
    यदि(uprv_isNaN(x) || uprv_isNaN(y))
        प्रत्यागम uprv_getNaN();

    /* check for -0 and 0*/
    यदि(x == 0.0 && y == 0.0 && u_signBit(y))
        प्रत्यागम y;

#यदि_अन्तः

    /* this should work for all flt point w/o NaN and Inf special cases */
    प्रत्यागम (x > y ? y : x);
}

U_CAPI UBool U_EXPORT2
uprv_add32_अतिप्रवाहः(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_add_overflow.
    // This function could be optimized by calling one of those primitives.
    स्वचालित a64 = static_cast<int64_t>(a);
    स्वचालित b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 + b64;
    *res = static_cast<int32_t>(res64);
    प्रत्यागम res64 != *res;
}

U_CAPI UBool U_EXPORT2
uprv_mul32_अतिप्रवाहः(int32_t a, int32_t b, int32_t* res) {
    // NOTE: Some compilers (GCC, Clang) have primitives available, like __builtin_mul_overflow.
    // This function could be optimized by calling one of those primitives.
    स्वचालित a64 = static_cast<int64_t>(a);
    स्वचालित b64 = static_cast<int64_t>(b);
    int64_t res64 = a64 * b64;
    *res = static_cast<int32_t>(res64);
    प्रत्यागम res64 != *res;
}

/**
 * Truncates the given double.
 * trunc(3.3) = 3.0, trunc (-3.3) = -3.0
 * This is different than calling floor() or ceil():
 * floor(3.3) = 3, floor(-3.3) = -4
 * ceil(3.3) = 4, ceil(-3.3) = -3
 */
U_CAPI द्विगुण U_EXPORT2
uprv_trunc(द्विगुण d)
{
#यदि IEEE_754
    /* handle error cases*/
    यदि(uprv_isNaN(d))
        प्रत्यागम uprv_getNaN();
    यदि(uprv_isInfinite(d))
        प्रत्यागम uprv_getInfinity();

    यदि(u_signBit(d))    /* Signbit() picks up -0.0;  d<0 does not. */
        प्रत्यागम ceil(d);
    अन्यथा
        प्रत्यागम floor(d);

#अन्यथा
    प्रत्यागम d >= 0 ? floor(d) : ceil(d);

#यदि_अन्तः
}

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 */
U_CAPI द्विगुण U_EXPORT2
uprv_maxMantissa()
{
    प्रत्यागम घातः(2.0, DBL_MANT_DIG + 1.0) - 1.0;
}

U_CAPI द्विगुण U_EXPORT2
uprv_log(द्विगुण d)
{
    प्रत्यागम लघुगणकः(d);
}

U_CAPI शून्य * U_EXPORT2
uprv_अधिकतमम्_Ptr(शून्य * आधारः)
{
#यदि U_PLATFORM == U_PF_OS400
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
    यदि ((आधारः != शून्यसूचकः) && (_TESTPTR(आधारः, _C_TERASPACE_CHECK))) {
        /* if it is a TERASPACE pointer the max is 2GB - 4k */
        प्रत्यागम ((शून्य *)(((वर्ण *)आधारः)-((uint32_t)(आधारः))+((uint32_t)0x7fffefff)));
    }
    /* otherwise 16MB since nullptr ptr is not checkable or the ptr is not TERASPACE */
    प्रत्यागम ((शून्य *)(((वर्ण *)आधारः)-((uint32_t)(आधारः))+((uint32_t)0xffefff)));

#अन्यथा
    प्रत्यागम U_MAX_PTR(आधारः);
#यदि_अन्तः
}

/*---------------------------------------------------------------------------
  Platform-specific Implementations
  Try these, and if they don't work on your platform, then special case your
  platform with new implementations.
  ---------------------------------------------------------------------------*/

/* Generic time zone layer -------------------------------------------------- */

/* Time zone utilities */
U_CAPI शून्य U_EXPORT2
uprv_tzset()
{
#यदि defined(U_TZSET)
    U_TZSET();
#अन्यथा
    /* no initialization*/
#यदि_अन्तः
}

U_CAPI int32_t U_EXPORT2
uprv_timezone()
{
#यदि_परिभाषितम् U_TIMEZONE
    प्रत्यागम U_TIMEZONE;
#अन्यथा
    time_t t, t1, t2;
    संरचना tm tmrec;
    int32_t tdiff = 0;

    time(&t);
    uprv_memcpy( &tmrec, localtime(&t), परिमाण(tmrec) );
#यदि U_PLATFORM != U_PF_IPHONE
    UBool dst_checked = (tmrec.tm_isdst != 0); /* daylight savings time is checked*/
#यदि_अन्तः
    t1 = mktime(&tmrec);                 /* local time in seconds*/
    uprv_memcpy( &tmrec, gmtime(&t), परिमाण(tmrec) );
    t2 = mktime(&tmrec);                 /* GMT (or UTC) in seconds*/
    tdiff = t2 - t1;

#यदि U_PLATFORM != U_PF_IPHONE
    /* imitate NT behaviour, which returns same timezone offset to GMT for
       winter and summer.
       This does not work on all platforms. For instance, on glibc on Linux
       and on Mac OS 10.5, tdiff calculated above remains the same
       regardless of whether DST is in effect or not. iOS is another
       platform where this does not work. Linux + glibc and Mac OS 10.5
       have U_TIMEZONE defined so that this code is not reached.
    */
    यदि (dst_checked)
        tdiff += 3600;
#यदि_अन्तः
    प्रत्यागम tdiff;
#यदि_अन्तः
}

/* Note that U_TZNAME does *not* have to be tzname, but if it is,
   some platforms need to have it declared here. */

#यदि defined(U_TZNAME) && (U_PLATFORM == U_PF_IRIX || U_PLATFORM_IS_DARWIN_BASED)
/* RS6000 and others reject char **tzname.  */
बाह्य U_IMPORT वर्ण *U_TZNAME[];
#यदि_अन्तः

#यदि !UCONFIG_NO_FILE_IO && ((U_PLATFORM_IS_DARWIN_BASED && (U_PLATFORM != U_PF_IPHONE || defined(U_TIMEZONE))) || U_PLATFORM_IS_LINUX_BASED || U_PLATFORM == U_PF_BSD || U_PLATFORM == U_PF_SOLARIS)
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

#परिभाषय CHECK_LOCALTIME_LINK 1
#यदि U_PLATFORM_IS_DARWIN_BASED
#अन्तर्भाव <tzfile.h>
#परिभाषय TZZONEINFO      (TZDIR "/")
#नोचेद्यदि U_PLATFORM == U_PF_SOLARIS
#परिभाषय TZDEFAULT       "/etc/localtime"
#परिभाषय TZZONEINFO      "/usr/share/lib/zoneinfo/"
#परिभाषय TZ_ENV_CHECK    "localtime"
#अन्यथा
#परिभाषय TZDEFAULT       "/etc/localtime"
#परिभाषय TZZONEINFO      "/usr/share/zoneinfo/"
#यदि_अन्तः
#परिभाषय TZZONEINFOTAIL  "/zoneinfo/"
#यदि U_HAVE_DIRENT_H
#परिभाषय TZFILE_SKIP     "posixrules" /* tz file to skip when searching. */
/* Some Linux distributions have 'localtime' in /usr/share/zoneinfo
   symlinked to /etc/localtime, which makes searchForTZFile return
   'localtime' when it's the first match. */
#परिभाषय TZFILE_SKIP2    "localtime"
#परिभाषय अन्वेषणम्_TZFILE
#अन्तर्भाव <dirent.h>  /* Needed to search through system timezone files */
#यदि_अन्तः
स्थिर वर्ण g_Time_Zone_बफर[मार्गः_MAX];
स्थिर ध्रुव वर्ण *g_Time_Zone_बफर_Ptr = शून्यसूचकः;
#यदि_अन्तः

#यदि !U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
#परिभाषय isNonDigit(ch) (ch < '0' || '9' < ch)
#परिभाषय isDigit(ch) ('0' <= ch && ch <= '9')
स्थिर UBool isValidOlsonID(ध्रुव वर्ण *id) {
    int32_t idx = 0;
    int32_t idxMax = 0;

    /* Determine if this is something like Iceland (Olson ID)
    or AST4ADT (non-Olson ID) */
    यावत् (id[idx] && isNonDigit(id[idx]) && id[idx] != ',') {
        idx++;
    }

    /* Allow at maximum 2 numbers at the end of the id to support zone id's
    like GMT+11. */
    idxMax = idx + 2;
    यावत् (id[idx] && isDigit(id[idx]) && idx < idxMax) {
        idx++;
    }

    /* If we went through the whole string, then it might be okay.
    The timezone is sometimes set to "CST-7CDT", "CST6CDT5,J129,J131/19:30",
    "GRNLNDST3GRNLNDDT" or similar, so we cannot use it.
    The rest of the time it could be an Olson ID. George */
    प्रत्यागम id[idx] == 0
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
स्थिर शून्य skipZoneIDPrefix(ध्रुव वर्ण** id) {
    यदि (uprv_strncmp(*id, "posix/", 6) == 0
        || uprv_strncmp(*id, "right/", 6) == 0)
    {
        *id += 6;
    }
}
#यदि_अन्तः

#यदि defined(U_TZNAME) && !U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API

#परिभाषय CONVERT_HOURS_गन्तव्यम्_SECONDS(विस्थापनम्) (int32_t)(विस्थापनम्*3600)
प्रकारनिर्देश संरचना विस्थापनम्_Zone_Mapping {
    int32_t विस्थापनम्_Seconds;
    int32_t daylightType; /* 0=U_DAYLIGHT_NONE, 1=daylight in June-U_DAYLIGHT_JUNE, 2=daylight in December=U_DAYLIGHT_DECEMBER*/
    ध्रुव वर्ण *stdID;
    ध्रुव वर्ण *dstID;
    ध्रुव वर्ण *olsonID;
} विस्थापनम्_Zone_Mapping;

गणना { U_DAYLIGHT_NONE=0,U_DAYLIGHT_JUNE=1,U_DAYLIGHT_DECEMBER=2 };

/*
This list tries to disambiguate a set of abbreviated timezone IDs and offsets
and maps it to an Olson ID.
Before adding anything to this list, take a look at
icu/source/tools/tzcode/tz.alias
Sometimes no daylight savings (0) is important to define due to aliases.
This list can be tested with icu/source/test/compat/tzone.pl
More values could be added to daylightType to increase precision.
*/
स्थिर ध्रुव संरचना विस्थापनम्_Zone_Mapping विस्थापनम्_ZONE_MAPPINGS[] = {
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

स्थिर ध्रुव वर्ण* remapShortTimeZone(ध्रुव वर्ण *stdID, ध्रुव वर्ण *dstID, int32_t daylightType, int32_t विस्थापनम्)
{
    int32_t idx;
#यदि_परिभाषितम् DEBUG_TZNAME
    fprintf(stderr, "TZ=%s std=%s dst=%s daylight=%d offset=%d\n", getenv("TZ"), stdID, dstID, daylightType, विस्थापनम्);
#यदि_अन्तः
    प्रति (idx = 0; idx < UPRV_LENGTHOF(विस्थापनम्_ZONE_MAPPINGS); idx++)
    {
        यदि (विस्थापनम् == विस्थापनम्_ZONE_MAPPINGS[idx].विस्थापनम्_Seconds
            && daylightType == विस्थापनम्_ZONE_MAPPINGS[idx].daylightType
            && वाक्यं_तुलय(विस्थापनम्_ZONE_MAPPINGS[idx].stdID, stdID) == 0
            && वाक्यं_तुलय(विस्थापनम्_ZONE_MAPPINGS[idx].dstID, dstID) == 0)
        {
            प्रत्यागम विस्थापनम्_ZONE_MAPPINGS[idx].olsonID;
        }
    }
    प्रत्यागम शून्यसूचकः;
}
#यदि_अन्तः

#यदि_परिभाषितम् अन्वेषणम्_TZFILE
#परिभाषय MAX_READ_आकार 512

प्रकारनिर्देश संरचना DefaultTZInfo {
    वर्ण* defaultTZBuffer;
    int64_t default_TZFile_आकार;
    FILE* defaultTZFilePtr;
    int32_t defaultTZPosition;
} DefaultTZInfo;

/*
 * This method compares the two files given to see if they are a match.
 * It is currently use to compare two TZ files.
 */
स्थिर UBool compare_द्विमान_Files(ध्रुव वर्ण* TZFileName, DefaultTZInfo* tzInfo) {
    FILE* file;
    int64_t आकार_File;
    int64_t आकार_File_वामः;
    int32_t आकार_File_Read;
    int32_t आकार_File_गन्तव्यम्_Read;
    वर्ण बफर_File[MAX_READ_आकार];
    UBool परिणाम = सत्य;

    U_ASSERT(tzInfo->defaultTZFilePtr != शून्यसूचकः);
    file = fopen(TZFileName, "r");

    tzInfo->defaultTZPosition = 0; /* reset position to begin search */

    यदि (file != शून्यसूचकः) {
        /* First check that the file size are equal. */
        यदि (tzInfo->default_TZFile_आकार == 0) {
            fseek(tzInfo->defaultTZFilePtr, 0, SEEK_END);
            tzInfo->default_TZFile_आकार = ftell(tzInfo->defaultTZFilePtr);
        }
        यदि (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            प्रत्यागम असत्य;
        }
        आकार_File = ftell(file);
        यदि (आकार_File == -1) {
            fclose(file);
            प्रत्यागम असत्य;
        }
        आकार_File_वामः = आकार_File;

        यदि (आकार_File != tzInfo->default_TZFile_आकार) {
            परिणाम = असत्य;
        } अन्यथा {
            /* Store the data from the files in separate buffers and
             * compare each byte to determine equality.
             */
            यदि (tzInfo->defaultTZBuffer == शून्यसूचकः) {
                यदि (fseek(tzInfo->defaultTZFilePtr, 0, SEEK_SET) != 0) {
                    fclose(file);
                    प्रत्यागम असत्य;
                }
                tzInfo->defaultTZBuffer = static_cast<वर्ण*>(uprv_malloc(परिमाण(वर्ण) * tzInfo->default_TZFile_आकार));
                आकार_File_Read = fread(tzInfo->defaultTZBuffer, 1, tzInfo->default_TZFile_आकार, tzInfo->defaultTZFilePtr);
            }
            यदि (fseek(file, 0, SEEK_SET) != 0) {
                fclose(file);
                प्रत्यागम असत्य;
            }
            यावत्(आकार_File_वामः > 0) {
                uprv_memset(बफर_File, 0, MAX_READ_आकार);
                आकार_File_गन्तव्यम्_Read = आकार_File_वामः < MAX_READ_आकार ? आकार_File_वामः : MAX_READ_आकार;

                आकार_File_Read = fread(बफर_File, 1, आकार_File_गन्तव्यम्_Read, file);
                यदि (आकार_File_Read == -1) {
                    fclose(file);
                    प्रत्यागम असत्य;
                }
                यदि (memcmp(tzInfo->defaultTZBuffer + tzInfo->defaultTZPosition, बफर_File, आकार_File_Read) != 0) {
                    परिणाम = असत्य;
                    भङ्ग;
                }
                आकार_File_वामः -= आकार_File_Read;
                tzInfo->defaultTZPosition += आकार_File_Read;
            }
        }
    } अन्यथा {
        परिणाम = असत्य;
    }

    यदि (file != शून्यसूचकः) {
        fclose(file);
    }

    प्रत्यागम परिणाम;
}


/* dirent also lists two entries: "." and ".." that we can safely ignore. */
#परिभाषय SKIP1 "."
#परिभाषय SKIP2 ".."
स्थिर UBool U_CALLCONV putil_cleanup();
स्थिर CharString *g_अन्वेषणम्_TZFile_परिणाम = शून्यसूचकः;

/*
 * This method recursively traverses the directory given for a matching TZ file and returns the first match.
 * This function is not thread safe - it uses a global, gSearchTZFileResult, to hold its results.
 */
स्थिर वर्ण* अन्वेषणम्_For_TZFile(ध्रुव वर्ण* मार्गः, DefaultTZInfo* tzInfo) {
    DIR* dirp = शून्यसूचकः;
    संरचना dirent* dir_प्रविष्टिः = शून्यसूचकः;
    वर्ण* परिणाम = शून्यसूचकः;
    UError_संहिता स्थितिः = U_ZERO_ERROR;

    /* There is no reference file to compare against, so do not traverse the zoneinfo tree. */
    यदि (tzInfo->defaultTZFilePtr == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }

    /* Save the current path */
    CharString curpath(मार्गः, -1, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        गच्छ cleanupAndReturn;
    }

    dirp = opendir(मार्गः);
    यदि (dirp == शून्यसूचकः) {
        गच्छ cleanupAndReturn;
    }

    यदि (g_अन्वेषणम्_TZFile_परिणाम == शून्यसूचकः) {
        g_अन्वेषणम्_TZFile_परिणाम = नूतन CharString;
        यदि (g_अन्वेषणम्_TZFile_परिणाम == शून्यसूचकः) {
            गच्छ cleanupAndReturn;
        }
        ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    }

    /* Check each entry in the directory. */
    यावत्((dir_प्रविष्टिः = readdir(dirp)) != शून्यसूचकः) {
        ध्रुव वर्ण* dirName = dir_प्रविष्टिः->d_name;
        यदि (uprv_strcmp(dirName, SKIP1) != 0 && uprv_strcmp(dirName, SKIP2) != 0
            && uprv_strcmp(TZFILE_SKIP, dirName) != 0 && uprv_strcmp(TZFILE_SKIP2, dirName) != 0) {
            /* Create a newpath with the new entry to test each entry in the directory. */
            CharString newpath(curpath, स्थितिः);
            newpath.append(dirName, -1, स्थितिः);
            यदि (U_FAILURE(स्थितिः)) {
                भङ्ग;
            }

            DIR* subDirp = शून्यसूचकः;
            यदि ((subDirp = opendir(newpath.दत्त())) != शून्यसूचकः) {
                /* If this new path is a directory, make a recursive call with the newpath. */
                closedir(subDirp);
                newpath.append('/', स्थितिः);
                यदि (U_FAILURE(स्थितिः)) {
                    भङ्ग;
                }
                परिणाम = अन्वेषणम्_For_TZFile(newpath.दत्त(), tzInfo);
                /*
                 Have to get out here. Otherwise, we'd keep looking
                 and return the first match in the top-level directory
                 if there's a match in the top-level. If not, this function
                 would return nullptr and set gTimeZoneBufferPtr to nullptr in initDefault().
                 It worked without this in most cases because we have a fallback of calling
                 localtime_r to figure out the default timezone.
                */
                यदि (परिणाम != शून्यसूचकः)
                    भङ्ग;
            } अन्यथा {
                यदि(compare_द्विमान_Files(newpath.दत्त(), tzInfo)) {
                    int32_t amount_गन्तव्यम्_Skip = परिमाण(TZZONEINFO) - 1;
                    यदि (amount_गन्तव्यम्_Skip > newpath.length()) {
                        amount_गन्तव्यम्_Skip = newpath.length();
                    }
                    ध्रुव वर्ण* zoneid = newpath.दत्त() + amount_गन्तव्यम्_Skip;
                    skipZoneIDPrefix(&zoneid);
                    g_अन्वेषणम्_TZFile_परिणाम->clear();
                    g_अन्वेषणम्_TZFile_परिणाम->append(zoneid, -1, स्थितिः);
                    यदि (U_FAILURE(स्थितिः)) {
                        भङ्ग;
                    }
                    परिणाम = g_अन्वेषणम्_TZFile_परिणाम->दत्त();
                    /* Get out after the first one found. */
                    भङ्ग;
                }
            }
        }
    }

  cleanupAndReturn:
    यदि (dirp) {
        closedir(dirp);
    }
    प्रत्यागम परिणाम;
}
#यदि_अन्तः

#यदि U_PLATFORM == U_PF_ANDROID
प्रकारनिर्देश पूर्णाङ्क(system_property_read_callback)(ध्रुव prop_info* info,
                                           शून्य (*callback)(शून्य* cookie,
                                                            ध्रुव वर्ण* name,
                                                            ध्रुव वर्ण* value,
                                                            uint32_t serial),
                                           शून्य* cookie);
प्रकारनिर्देश पूर्णाङ्क(system_property_get)(ध्रुव वर्ण*, वर्ण*);

स्थिर वर्ण gAndroidTimeZone[PROP_मानम्_MAX] = { '\0' };

स्थिर शून्य u_property_read(शून्य* cookie, ध्रुव वर्ण* name, ध्रुव वर्ण* value,
                            uint32_t serial) {
    uprv_strcpy((वर्ण* )cookie, value);
}
#यदि_अन्तः

U_CAPI शून्य U_EXPORT2
uprv_tzname_clear_cache()
{
#यदि U_PLATFORM == U_PF_ANDROID
    /* Android's timezone is stored in system property. */
    gAndroidTimeZone[0] = '\0';
    शून्य* libc = dlopen("libc.so", RTLD_NOLOAD);
    यदि (libc) {
        /* Android API 26+ has new API to get system property and old API
         * (__system_property_get) is deprecated */
        system_property_read_callback* property_read_callback =
            (system_property_read_callback*)dlsym(
                libc, "__system_property_read_callback");
        यदि (property_read_callback) {
            ध्रुव prop_info* info =
                __system_property_find("persist.sys.timezone");
            यदि (info) {
                property_read_callback(info, &u_property_read, gAndroidTimeZone);
            }
        } अन्यथा {
            system_property_get* property_get =
                (system_property_get*)dlsym(libc, "__system_property_get");
            यदि (property_get) {
                property_get("persist.sys.timezone", gAndroidTimeZone);
            }
        }
        dlclose(libc);
    }
#यदि_अन्तः

#यदि defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    g_Time_Zone_बफर_Ptr = शून्यसूचकः;
#यदि_अन्तः
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uprv_tzname(पूर्णाङ्क n)
{
    (शून्य)n; // Avoid unreferenced parameter warning.
    ध्रुव वर्ण *tzid = शून्यसूचकः;
#यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
    tzid = uprv_detectWindowsTimeZone();

    यदि (tzid != शून्यसूचकः) {
        प्रत्यागम tzid;
    }

#यदि_अपरिभाषितम् U_TZNAME
    // The return value is free'd in timezone.cpp on Windows because
    // the other code path returns a pointer to a heap location.
    // If we don't have a name already, then tzname wouldn't be any
    // better, so just fall back.
    प्रत्यागम uprv_strdup("");
#यदि_अन्तः // !U_TZNAME

#अन्यथा

/*#if U_PLATFORM_IS_DARWIN_BASED
    int ret;

    tzid = getenv("TZFILE");
    if (tzid != nullptr) {
        return tzid;
    }
#endif*/

/* This code can be temporarily disabled to test tzname resolution later on. */
#यदि_अपरिभाषितम् DEBUG_TZNAME
#यदि U_PLATFORM == U_PF_ANDROID
    tzid = gAndroidTimeZone;
#अन्यथा
    tzid = getenv("TZ");
#यदि_अन्तः
    यदि (tzid != शून्यसूचकः && isValidOlsonID(tzid)
#यदि U_PLATFORM == U_PF_SOLARIS
    /* Don't misinterpret TZ "localtime" on Solaris as a time zone name. */
        && uprv_strcmp(tzid, TZ_ENV_CHECK) != 0
#यदि_अन्तः
    ) {
        /* The colon forces tzset() to treat the remainder as zoneinfo path */
        यदि (tzid[0] == ':') {
            tzid++;
        }
        /* This might be a good Olson ID. */
        skipZoneIDPrefix(&tzid);
        प्रत्यागम tzid;
    }
    /* else U_TZNAME will give a better result. */
#यदि_अन्तः

#यदि defined(CHECK_LOCALTIME_LINK) && !defined(DEBUG_SKIP_LOCALTIME_LINK)
    /* Caller must handle threading issues */
    यदि (g_Time_Zone_बफर_Ptr == शून्यसूचकः) {
        /*
        This is a trick to look at the name of the link to get the Olson ID
        because the tzfile contents is underspecified.
        This isn't guaranteed to work because it may not be a symlink.
        */
        वर्ण *ret = realpath(TZDEFAULT, g_Time_Zone_बफर);
        यदि (ret != शून्यसूचकः && uprv_strcmp(TZDEFAULT, g_Time_Zone_बफर) != 0) {
            int32_t tz_Zone_Info_पुच्छ_Len = uprv_strlen(TZZONEINFOTAIL);
            ध्रुव वर्ण *tz_Zone_Info_पुच्छ_Ptr = uprv_strstr(g_Time_Zone_बफर, TZZONEINFOTAIL);
            // MacOS14 has the realpath as something like
            // /usr/share/zoneinfo.default/Australia/Melbourne
            // which will not have "/zoneinfo/" in the path.
            // Therefore if we fail, we fall back to read the link which is
            // /var/db/timezone/zoneinfo/Australia/Melbourne
            // We also fall back to reading the link if the realpath leads to something like
            // /usr/share/zoneinfo/posixrules
            यदि (tz_Zone_Info_पुच्छ_Ptr == शून्यसूचकः ||
                    uprv_strcmp(tz_Zone_Info_पुच्छ_Ptr + tz_Zone_Info_पुच्छ_Len, "posixrules") == 0) {
                ssize_t size = readlink(TZDEFAULT, g_Time_Zone_बफर, परिमाण(g_Time_Zone_बफर)-1);
                यदि (size > 0) {
                    g_Time_Zone_बफर[size] = 0;
                    tz_Zone_Info_पुच्छ_Ptr = uprv_strstr(g_Time_Zone_बफर, TZZONEINFOTAIL);
                }
            }
            यदि (tz_Zone_Info_पुच्छ_Ptr != शून्यसूचकः) {
                tz_Zone_Info_पुच्छ_Ptr += tz_Zone_Info_पुच्छ_Len;
                skipZoneIDPrefix(&tz_Zone_Info_पुच्छ_Ptr);
                यदि (isValidOlsonID(tz_Zone_Info_पुच्छ_Ptr)) {
                    प्रत्यागम (g_Time_Zone_बफर_Ptr = tz_Zone_Info_पुच्छ_Ptr);
                }
            }
        } अन्यथा {
#यदि defined(अन्वेषणम्_TZFILE)
            DefaultTZInfo* tzInfo = (DefaultTZInfo*)uprv_malloc(परिमाण(DefaultTZInfo));
            यदि (tzInfo != शून्यसूचकः) {
                tzInfo->defaultTZBuffer = शून्यसूचकः;
                tzInfo->default_TZFile_आकार = 0;
                tzInfo->defaultTZFilePtr = fopen(TZDEFAULT, "r");
                tzInfo->defaultTZPosition = 0;

                g_Time_Zone_बफर_Ptr = अन्वेषणम्_For_TZFile(TZZONEINFO, tzInfo);

                /* Free previously allocated memory */
                यदि (tzInfo->defaultTZBuffer != शून्यसूचकः) {
                    uprv_free(tzInfo->defaultTZBuffer);
                }
                यदि (tzInfo->defaultTZFilePtr != शून्यसूचकः) {
                    fclose(tzInfo->defaultTZFilePtr);
                }
                uprv_free(tzInfo);
            }

            यदि (g_Time_Zone_बफर_Ptr != शून्यसूचकः && isValidOlsonID(g_Time_Zone_बफर_Ptr)) {
                प्रत्यागम g_Time_Zone_बफर_Ptr;
            }
#यदि_अन्तः
        }
    }
    अन्यथा {
        प्रत्यागम g_Time_Zone_बफर_Ptr;
    }
#यदि_अन्तः
#यदि_अन्तः

#यदि_परिभाषितम् U_TZNAME
#यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
    /* The return value is free'd in timezone.cpp on Windows because
     * the other code path returns a pointer to a heap location. */
    प्रत्यागम uprv_strdup(U_TZNAME[n]);
#अन्यथा
    /*
    U_TZNAME is usually a non-unique abbreviation, which isn't normally usable.
    So we remap the abbreviation to an olson ID.

    Since Windows exposes a little more timezone information,
    we normally don't use this code on Windows because
    uprv_detectWindowsTimeZone should have already given the correct answer.
    */
    {
        संरचना tm juneSol, decemberSol;
        पूर्णाङ्क daylightType;
        स्थिर ध्रुव time_t juneSolstice=1182478260; /*2007-06-21 18:11 UT*/
        स्थिर ध्रुव time_t decemberSolstice=1198332540; /*2007-12-22 06:09 UT*/

        /* This probing will tell us when daylight savings occurs.  */
        localtime_r(&juneSolstice, &juneSol);
        localtime_r(&decemberSolstice, &decemberSol);
        यदि(decemberSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_DECEMBER;
        } अन्यथा यदि(juneSol.tm_isdst > 0) {
          daylightType = U_DAYLIGHT_JUNE;
        } अन्यथा {
          daylightType = U_DAYLIGHT_NONE;
        }
        tzid = remapShortTimeZone(U_TZNAME[0], U_TZNAME[1], daylightType, uprv_timezone());
        यदि (tzid != शून्यसूचकः) {
            प्रत्यागम tzid;
        }
    }
    प्रत्यागम U_TZNAME[n];
#यदि_अन्तः
#अन्यथा
    प्रत्यागम "";
#यदि_अन्तः
}

/* Get and set the ICU data directory --------------------------------------- */

स्थिर icu::UInitOnce g_दत्त_Dir_Init_Once {};
स्थिर वर्ण *g_दत्त_Directory = शून्यसूचकः;

UInitOnce gTimeZoneFilesInitOnce {};
स्थिर CharString *gTimeZoneFilesDirectory = शून्यसूचकः;

#यदि U_POSIX_LOCALE || U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
 स्थिर ध्रुव वर्ण *gCorrectedPOSIXLocale = शून्यसूचकः; /* Sometimes heap allocated */
 स्थिर तार्किक gCorrectedPOSIXLocaleHeapAllocated = असत्य;
#यदि_अन्तः

स्थिर UBool U_CALLCONV putil_cleanup()
{
    यदि (g_दत्त_Directory && *g_दत्त_Directory) {
        uprv_free(g_दत्त_Directory);
    }
    g_दत्त_Directory = शून्यसूचकः;
    g_दत्त_Dir_Init_Once.reset();

    विलोपय gTimeZoneFilesDirectory;
    gTimeZoneFilesDirectory = शून्यसूचकः;
    gTimeZoneFilesInitOnce.reset();

#यदि_परिभाषितम् अन्वेषणम्_TZFILE
    विलोपय g_अन्वेषणम्_TZFile_परिणाम;
    g_अन्वेषणम्_TZFile_परिणाम = शून्यसूचकः;
#यदि_अन्तः

#यदि U_POSIX_LOCALE || U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
    यदि (gCorrectedPOSIXLocale && gCorrectedPOSIXLocaleHeapAllocated) {
        uprv_free(const_cast<वर्ण *>(gCorrectedPOSIXLocale));
        gCorrectedPOSIXLocale = शून्यसूचकः;
        gCorrectedPOSIXLocaleHeapAllocated = असत्य;
    }
#यदि_अन्तः
    प्रत्यागम सत्य;
}

/*
 * Set the data directory.
 *    Make a copy of the passed string, and set the global data dir to point to it.
 */
U_CAPI शून्य U_EXPORT2
u_समुच्चयः_दत्त_Directory(ध्रुव वर्ण *directory) {
    वर्ण *new_दत्त_Dir;
    int32_t length;

    यदि(directory==शून्यसूचकः || *directory==0) {
        /* A small optimization to prevent the malloc and copy when the
        shared library is used, and this is a way to make sure that nullptr
        is never returned.
        */
        new_दत्त_Dir = (वर्ण *)"";
    }
    अन्यथा {
        length=(int32_t)uprv_strlen(directory);
        new_दत्त_Dir = (वर्ण *)uprv_malloc(length + 2);
        /* Exit out if newDataDir could not be created. */
        यदि (new_दत्त_Dir == शून्यसूचकः) {
            प्रत्यागम;
        }
        uprv_strcpy(new_दत्त_Dir, directory);

#यदि (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
        {
            वर्ण *p;
            यावत्((p = uprv_strchr(new_दत्त_Dir, U_FILE_ALT_SEP_CHAR)) != शून्यसूचकः) {
                *p = U_FILE_SEP_CHAR;
            }
        }
#यदि_अन्तः
    }

    यदि (g_दत्त_Directory && *g_दत्त_Directory) {
        uprv_free(g_दत्त_Directory);
    }
    g_दत्त_Directory = new_दत्त_Dir;
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
}

U_CAPI UBool U_EXPORT2
uprv_मार्गः_Is_Absolute(ध्रुव वर्ण *मार्गः)
{
  यदि(!मार्गः || !*मार्गः) {
    प्रत्यागम असत्य;
  }

  यदि(*मार्गः == U_FILE_SEP_CHAR) {
    प्रत्यागम सत्य;
  }

#यदि (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
  यदि(*मार्गः == U_FILE_ALT_SEP_CHAR) {
    प्रत्यागम सत्य;
  }
#यदि_अन्तः

#यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
  यदि( (((मार्गः[0] >= 'A') && (मार्गः[0] <= 'Z')) ||
       ((मार्गः[0] >= 'a') && (मार्गः[0] <= 'z'))) &&
      मार्गः[1] == ':' ) {
    प्रत्यागम सत्य;
  }
#यदि_अन्तः

  प्रत्यागम असत्य;
}

/* Backup setting of ICU_DATA_DIR_PREFIX_ENV_VAR
   (needed for some Darwin ICU build environments) */
#यदि U_PLATFORM_IS_DARWIN_BASED && defined(TARGET_OS_SIMULATOR) && TARGET_OS_SIMULATOR
# यदि !defined(ICU_दत्त_DIR_उपसर्गः_ENV_VAR)
#  define ICU_दत्त_DIR_उपसर्गः_ENV_VAR "IPHONE_SIMULATOR_ROOT"
# endif
#यदि_अन्तः

#यदि defined(ICU_दत्त_DIR_WINDOWS)
// Helper function to get the ICU Data Directory under the Windows directory location.
स्थिर BOOL U_CALLCONV get_Icu_दत्त_Directory_Under_Windows_Directory(वर्ण* directory_बफर, UINT बफर_दीर्घता)
{
    wchar_t windows_मार्गः[MAX_मार्गः];
    वर्ण windows_मार्गः_Utf8[MAX_मार्गः];

    UINT length = GetSystemWindowsDirectoryW(windows_मार्गः, UPRV_LENGTHOF(windows_मार्गः));
    यदि ((length > 0) && (length < (UPRV_LENGTHOF(windows_मार्गः) - 1))) {
        // Convert UTF-16 to a UTF-8 string.
        UError_संहिता स्थितिः = U_ZERO_ERROR;
        int32_t windows_मार्गः_Utf8_Len = 0;
        u_str_गन्तव्यम्_UTF8(windows_मार्गः_Utf8, static_cast<int32_t>(UPRV_LENGTHOF(windows_मार्गः_Utf8)),
            &windows_मार्गः_Utf8_Len, reinterpret_cast<ध्रुव char16_t*>(windows_मार्गः), -1, &स्थितिः);

        यदि (U_SUCCESS(स्थितिः) && (स्थितिः != U_STRING_NOT_TERMINATED_WARNING) &&
            (windows_मार्गः_Utf8_Len < (UPRV_LENGTHOF(windows_मार्गः_Utf8) - 1))) {
            // Ensure it always has a separator, so we can append the ICU data path.
            यदि (windows_मार्गः_Utf8[windows_मार्गः_Utf8_Len - 1] != U_FILE_SEP_CHAR) {
                windows_मार्गः_Utf8[windows_मार्गः_Utf8_Len++] = U_FILE_SEP_CHAR;
                windows_मार्गः_Utf8[windows_मार्गः_Utf8_Len] = '\0';
            }
            // Check if the concatenated string will fit.
            यदि ((windows_मार्गः_Utf8_Len + UPRV_LENGTHOF(ICU_दत्त_DIR_WINDOWS)) < बफर_दीर्घता) {
                uprv_strcpy(directory_बफर, windows_मार्गः_Utf8);
                uprv_strcat(directory_बफर, ICU_दत्त_DIR_WINDOWS);
                प्रत्यागम सत्य;
            }
        }
    }

    प्रत्यागम असत्य;
}
#यदि_अन्तः

स्थिर शून्य U_CALLCONV दत्त_Directory_Init_Fn() {
    /* If we already have the directory, then return immediately. Will happen if user called
     * u_setDataDirectory().
     */
    यदि (g_दत्त_Directory) {
        प्रत्यागम;
    }

    ध्रुव वर्ण *मार्गः = शून्यसूचकः;

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
#   यदि !defined(ICU_NO_USER_दत्त_OVERRIDE) && !UCONFIG_NO_FILE_IO
    /* First try to get the environment variable */
#     यदि U_PLATFORM_HAS_WINUWP_API == 0  // Windows UWP does not support getenv
        मार्गः=getenv("ICU_DATA");
#     endif
#   endif

    /* ICU_DATA_DIR may be set as a compile option.
     * U_ICU_DATA_DEFAULT_DIR is provided and is set by ICU at compile time
     * and is used only when data is built in archive mode eliminating the need
     * for ICU_DATA_DIR to be set. U_ICU_DATA_DEFAULT_DIR is set to the installation
     * directory of the data dat file. Users should use ICU_DATA_DIR if they want to
     * set their own path.
     */
#यदि defined(ICU_दत्त_DIR) || defined(U_ICU_दत्त_DEFAULT_DIR)
# यदि defined(ICU_दत्त_DIR_उपसर्गः_ENV_VAR)
    वर्ण datadir_मार्गः_बफर[मार्गः_MAX];
# endif
    यदि(मार्गः==शून्यसूचकः || *मार्गः==0) {
# यदि defined(ICU_दत्त_DIR_उपसर्गः_ENV_VAR)
        ध्रुव वर्ण *उपसर्गः = getenv(ICU_दत्त_DIR_उपसर्गः_ENV_VAR);
# endif
# ifdef ICU_दत्त_DIR
        मार्गः=ICU_दत्त_DIR;
# अन्यथा
        मार्गः=U_ICU_दत्त_DEFAULT_DIR;
# endif
# यदि defined(ICU_दत्त_DIR_उपसर्गः_ENV_VAR)
        यदि (उपसर्गः != शून्यसूचकः) {
            snprintf(datadir_मार्गः_बफर, परिमाण(datadir_मार्गः_बफर), "%s%s", उपसर्गः, मार्गः);
            मार्गः=datadir_मार्गः_बफर;
        }
# endif
    }
#यदि_अन्तः

#यदि defined(ICU_दत्त_DIR_WINDOWS)
    वर्ण datadir_मार्गः_बफर[MAX_मार्गः];
    यदि (get_Icu_दत्त_Directory_Under_Windows_Directory(datadir_मार्गः_बफर, UPRV_LENGTHOF(datadir_मार्गः_बफर))) {
        मार्गः = datadir_मार्गः_बफर;
    }
#यदि_अन्तः

    यदि(मार्गः==शून्यसूचकः) {
        /* It looks really bad, set it to something. */
        मार्गः = "";
    }

    u_समुच्चयः_दत्त_Directory(मार्गः);
}

U_CAPI ध्रुव वर्ण * U_EXPORT2
u_get_दत्त_Directory() {
    umtx_initOnce(g_दत्त_Dir_Init_Once, &दत्त_Directory_Init_Fn);
    प्रत्यागम g_दत्त_Directory;
}

स्थिर शून्य समुच्चयः_Time_Zone_Files_Dir(ध्रुव वर्ण *मार्गः, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    gTimeZoneFilesDirectory->clear();
    gTimeZoneFilesDirectory->append(मार्गः, स्थितिः);
#यदि (U_FILE_SEP_CHAR != U_FILE_ALT_SEP_CHAR)
    वर्ण *p = gTimeZoneFilesDirectory->दत्त();
    यावत् ((p = uprv_strchr(p, U_FILE_ALT_SEP_CHAR)) != शून्यसूचकः) {
        *p = U_FILE_SEP_CHAR;
    }
#यदि_अन्तः
}

#परिभाषय गन्तव्यम्_STRING(x) गन्तव्यम्_STRING_2(x)
#परिभाषय गन्तव्यम्_STRING_2(x) #x

स्थिर शून्य U_CALLCONV Time_Zone_दत्त_Dir_Init_Fn(UError_संहिता &स्थितिः) {
    U_ASSERT(gTimeZoneFilesDirectory == शून्यसूचकः);
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
    gTimeZoneFilesDirectory = नूतन CharString();
    यदि (gTimeZoneFilesDirectory == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    ध्रुव वर्ण *dir = "";

#यदि defined(ICU_TIMEZONE_FILES_DIR_उपसर्गः_ENV_VAR)
    वर्ण timezonefilesdir_मार्गः_बफर[मार्गः_MAX];
    ध्रुव वर्ण *उपसर्गः = getenv(ICU_TIMEZONE_FILES_DIR_उपसर्गः_ENV_VAR);
#यदि_अन्तः

#यदि U_PLATFORM_HAS_WINUWP_API == 1
// The UWP version does not support the environment variable setting.

# यदि defined(ICU_दत्त_DIR_WINDOWS)
    // When using the Windows system data, we can possibly pick up time zone data from the Windows directory.
    वर्ण datadir_मार्गः_बफर[MAX_मार्गः];
    यदि (get_Icu_दत्त_Directory_Under_Windows_Directory(datadir_मार्गः_बफर, UPRV_LENGTHOF(datadir_मार्गः_बफर))) {
        dir = datadir_मार्गः_बफर;
    }
# endif

#अन्यथा
    dir = getenv("ICU_TIMEZONE_FILES_DIR");
#यदि_अन्तः // U_PLATFORM_HAS_WINUWP_API

#यदि defined(U_TIMEZONE_FILES_DIR)
    यदि (dir == शून्यसूचकः) {
        // Build time configuration setting.
        dir = गन्तव्यम्_STRING(U_TIMEZONE_FILES_DIR);
    }
#यदि_अन्तः

    यदि (dir == शून्यसूचकः) {
        dir = "";
    }

#यदि defined(ICU_TIMEZONE_FILES_DIR_उपसर्गः_ENV_VAR)
    यदि (उपसर्गः != शून्यसूचकः) {
        snprintf(timezonefilesdir_मार्गः_बफर, परिमाण(timezonefilesdir_मार्गः_बफर), "%s%s", उपसर्गः, dir);
        dir = timezonefilesdir_मार्गः_बफर;
    }
#यदि_अन्तः

    समुच्चयः_Time_Zone_Files_Dir(dir, स्थितिः);
}


U_CAPI ध्रुव वर्ण * U_EXPORT2
u_getTimeZoneFilesDirectory(UError_संहिता *स्थितिः) {
    umtx_initOnce(gTimeZoneFilesInitOnce, &Time_Zone_दत्त_Dir_Init_Fn, *स्थितिः);
    प्रत्यागम U_SUCCESS(*स्थितिः) ? gTimeZoneFilesDirectory->दत्त() : "";
}

U_CAPI शून्य U_EXPORT2
u_समुच्चयः_Time_Zone_Files_Directory(ध्रुव वर्ण *मार्गः, UError_संहिता *स्थितिः) {
    umtx_initOnce(gTimeZoneFilesInitOnce, &Time_Zone_दत्त_Dir_Init_Fn, *स्थितिः);
    समुच्चयः_Time_Zone_Files_Dir(मार्गः, *स्थितिः);

    // Note: this function does some extra churn, first setting based on the
    //       environment, then immediately replacing with the value passed in.
    //       The logic is simpler that way, and performance shouldn't be an issue.
}


#यदि U_POSIX_LOCALE
/* A helper function used by uprv_getPOSIXIDForDefaultLocale and
 * uprv_getPOSIXIDForDefaultCodepage. Returns the posix locale id for
 * LC_CTYPE and LC_MESSAGES. It doesn't support other locale categories.
 */
स्थिर ध्रुव वर्ण *uprv_getPOSIXIDForCategory(पूर्णाङ्क category)
{
    ध्रुव वर्ण* posixID = शून्यसूचकः;
    यदि (category == LC_MESSAGES || category == LC_CTYPE) {
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
        posixID = setlocale(category, शून्यसूचकः);
        यदि ((posixID == शून्यसूचकः)
            || (uprv_strcmp("C", posixID) == 0)
            || (uprv_strcmp("POSIX", posixID) == 0))
        {
            /* Maybe we got some garbage.  Try something more reasonable */
            posixID = getenv("LC_ALL");
            /* Solaris speaks POSIX -  See IEEE Std 1003.1-2008
             * This is needed to properly handle empty env. variables
             */
#यदि U_PLATFORM == U_PF_SOLARIS
            यदि ((posixID == 0) || (posixID[0] == '\0')) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                यदि ((posixID == 0) || (posixID[0] == '\0')) {
#अन्यथा
            यदि (posixID == शून्यसूचकः) {
                posixID = getenv(category == LC_MESSAGES ? "LC_MESSAGES" : "LC_CTYPE");
                यदि (posixID == शून्यसूचकः) {
#यदि_अन्तः
                    posixID = getenv("LANG");
                }
            }
        }
    }
    यदि ((posixID == शून्यसूचकः)
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
    प्रत्यागम posixID;
}

/* Return just the POSIX id for the default locale, whatever happens to be in
 * it. It gets the value from LC_MESSAGES and indirectly from LC_ALL and LANG.
 */
स्थिर ध्रुव वर्ण *uprv_getPOSIXIDForDefaultLocale()
{
    स्थिर ध्रुव वर्ण* posixID = शून्यसूचकः;
    यदि (posixID == शून्यसूचकः) {
        posixID = uprv_getPOSIXIDForCategory(LC_MESSAGES);
    }
    प्रत्यागम posixID;
}

#यदि !U_CHARSET_IS_UTF8
/* Return just the POSIX id for the default codepage, whatever happens to be in
 * it. It gets the value from LC_CTYPE and indirectly from LC_ALL and LANG.
 */
स्थिर ध्रुव वर्ण *uprv_getPOSIXIDForDefaultCodepage()
{
    स्थिर ध्रुव वर्ण* posixID = शून्यसूचकः;
    यदि (posixID == 0) {
        posixID = uprv_getPOSIXIDForCategory(LC_CTYPE);
    }
    प्रत्यागम posixID;
}
#यदि_अन्तः
#यदि_अन्तः

/* NOTE: The caller should handle thread safety */
U_CAPI ध्रुव वर्ण* U_EXPORT2
uprv_getDefaultLocaleID()
{
#यदि U_POSIX_LOCALE
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
    ध्रुव वर्ण* posixID = uprv_getPOSIXIDForDefaultLocale();

    /* Format: (no spaces)
    ll [ _CC ] [ . MM ] [ @ VV]

      l = lang, C = ctry, M = charmap, V = variant
    */

    यदि (gCorrectedPOSIXLocale != शून्यसूचकः) {
        प्रत्यागम gCorrectedPOSIXLocale;
    }

    // Copy the ID into owned memory.
    // Over-allocate in case we replace "C" with "en_US_POSIX" (+10), + null termination
    वर्ण *correctedPOSIXLocale = static_cast<वर्ण *>(uprv_malloc(uprv_strlen(posixID) + 10 + 1));
    यदि (correctedPOSIXLocale == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    uprv_strcpy(correctedPOSIXLocale, posixID);

    वर्ण *सीमा;
    यदि ((सीमा = uprv_strchr(correctedPOSIXLocale, '.')) != शून्यसूचकः) {
        *सीमा = 0;
    }
    यदि ((सीमा = uprv_strchr(correctedPOSIXLocale, '@')) != शून्यसूचकः) {
        *सीमा = 0;
    }

    यदि ((uprv_strcmp("C", correctedPOSIXLocale) == 0) // no @ variant
        || (uprv_strcmp("POSIX", correctedPOSIXLocale) == 0)) {
      // Raw input was C.* or POSIX.*, Give it a nice POSIX default value.
      // (The "C"/"POSIX" case is handled in uprv_getPOSIXIDForCategory())
      uprv_strcpy(correctedPOSIXLocale, "en_US_POSIX");
    }

    /* Note that we scan the *uncorrected* ID. */
    ध्रुव वर्ण *p;
    यदि ((p = uprv_strrchr(posixID, '@')) != शून्यसूचकः) {
        p++;

        /* Take care of any special cases here.. */
        यदि (!uprv_strcmp(p, "nynorsk")) {
            p = "NY";
            /* Don't worry about no__NY. In practice, it won't appear. */
        }

        यदि (uprv_strchr(correctedPOSIXLocale,'_') == शून्यसूचकः) {
            uprv_strcat(correctedPOSIXLocale, "__"); /* aa@b -> aa__b (note this can make the new locale 1 char longer) */
        }
        अन्यथा {
            uprv_strcat(correctedPOSIXLocale, "_"); /* aa_CC@b -> aa_CC_b */
        }

        ध्रुव वर्ण *q;
        यदि ((q = uprv_strchr(p, '.')) != शून्यसूचकः) {
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

    यदि (gCorrectedPOSIXLocale == शून्यसूचकः) {
        gCorrectedPOSIXLocale = correctedPOSIXLocale;
        gCorrectedPOSIXLocaleHeapAllocated = सत्य;
        ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
        correctedPOSIXLocale = शून्यसूचकः;
    }
    posixID = gCorrectedPOSIXLocale;

    यदि (correctedPOSIXLocale != शून्यसूचकः) {  /* Was already set - clean up. */
        uprv_free(correctedPOSIXLocale);
    }

    प्रत्यागम posixID;

#नोचेद्यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
#परिभाषय POSIX_LOCALE_क्षमता 64
    UError_संहिता स्थितिः = U_ZERO_ERROR;
    वर्ण *correctedPOSIXLocale = शून्यसूचकः;

    // If we have already figured this out just use the cached value
    यदि (gCorrectedPOSIXLocale != शून्यसूचकः) {
        प्रत्यागम gCorrectedPOSIXLocale;
    }

    // No cached value, need to determine the current value
    स्थिर WCHAR windowsLocale[LOCALE_NAME_MAX_दीर्घता] = {};
    पूर्णाङ्क length = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, windowsLocale, LOCALE_NAME_MAX_दीर्घता);

    // Now we should have a Windows locale name that needs converted to the POSIX style.
    यदि (length > 0) // If length is 0, then the GetLocaleInfoEx failed.
    {
        // First we need to go from UTF-16 to char (and also convert from _ to - while we're at it.)
        वर्ण modifiedWindowsLocale[LOCALE_NAME_MAX_दीर्घता] = {};

        int32_t i;
        प्रति (i = 0; i < UPRV_LENGTHOF(modifiedWindowsLocale); i++)
        {
            यदि (windowsLocale[i] == '_')
            {
                modifiedWindowsLocale[i] = '-';
            }
            अन्यथा
            {
                modifiedWindowsLocale[i] = static_cast<वर्ण>(windowsLocale[i]);
            }

            यदि (modifiedWindowsLocale[i] == '\0')
            {
                भङ्ग;
            }
        }

        यदि (i >= UPRV_LENGTHOF(modifiedWindowsLocale))
        {
            // Ran out of room, can't really happen, maybe we'll be lucky about a matching
            // locale when tags are dropped
            modifiedWindowsLocale[UPRV_LENGTHOF(modifiedWindowsLocale) - 1] = '\0';
        }

        // Now normalize the resulting name
        correctedPOSIXLocale = static_cast<वर्ण *>(uprv_malloc(POSIX_LOCALE_क्षमता + 1));
        /* TODO: Should we just exit on memory allocation failure? */
        यदि (correctedPOSIXLocale)
        {
            int32_t posixLen = uloc_canonicalize(modifiedWindowsLocale, correctedPOSIXLocale, POSIX_LOCALE_क्षमता, &स्थितिः);
            यदि (U_SUCCESS(स्थितिः))
            {
                *(correctedPOSIXLocale + posixLen) = 0;
                gCorrectedPOSIXLocale = correctedPOSIXLocale;
                gCorrectedPOSIXLocaleHeapAllocated = सत्य;
                ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_PUTIL, putil_cleanup);
            }
            अन्यथा
            {
                uprv_free(correctedPOSIXLocale);
            }
        }
    }

    // If unable to find a locale we can agree upon, use en-US by default
    यदि (gCorrectedPOSIXLocale == शून्यसूचकः) {
        gCorrectedPOSIXLocale = "en_US";
    }
    प्रत्यागम gCorrectedPOSIXLocale;

#नोचेद्यदि U_PLATFORM == U_PF_OS400
    /* locales are process scoped and are by definition thread safe */
    स्थिर वर्ण correctedLocale[64];
    ध्रुव  वर्ण *localeID = getenv("LC_ALL");
           वर्ण *p;

    यदि (localeID == शून्यसूचकः)
        localeID = getenv("LANG");
    यदि (localeID == शून्यसूचकः)
        localeID = setlocale(LC_ALL, शून्यसूचकः);
    /* Make sure we have something... */
    यदि (localeID == शून्यसूचकः)
        प्रत्यागम "en_US_POSIX";

    /* Extract the locale name from the path. */
    यदि((p = uprv_strrchr(localeID, '/')) != शून्यसूचकः)
    {
        /* Increment p to start of locale name. */
        p++;
        localeID = p;
    }

    /* Copy to work location. */
    uprv_strcpy(correctedLocale, localeID);

    /* Strip off the '.locale' extension. */
    यदि((p = uprv_strchr(correctedLocale, '.')) != शून्यसूचकः) {
        *p = 0;
    }

    /* Upper case the locale name. */
    T_CString_गन्तव्यम्_ऊर्ध्वस्थम्_Case(correctedLocale);

    /* See if we are using the POSIX locale.  Any of the
    * following are equivalent and use the same QLGPGCMA
    * (POSIX) locale.
    * QLGPGCMA2 means UCS2
    * QLGPGCMA_4 means UTF-32
    * QLGPGCMA_8 means UTF-8
    */
    यदि ((uprv_strcmp("C", correctedLocale) == 0) ||
        (uprv_strcmp("POSIX", correctedLocale) == 0) ||
        (uprv_strncmp("QLGPGCMA", correctedLocale, 8) == 0))
    {
        uprv_strcpy(correctedLocale, "en_US_POSIX");
    }
    अन्यथा
    {
        int16_t LocaleLen;

        /* Lower case the lang portion. */
        प्रति(p = correctedLocale; *p != 0 && *p != '_'; p++)
        {
            *p = uprv_tolower(*p);
        }

        /* Adjust for Euro.  After '_E' add 'URO'. */
        LocaleLen = uprv_strlen(correctedLocale);
        यदि (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'E')
        {
            uprv_strcat(correctedLocale, "URO");
        }

        /* If using Lotus-based locale then convert to
         * equivalent non Lotus.
         */
        अन्यथा यदि (correctedLocale[LocaleLen - 2] == '_' &&
            correctedLocale[LocaleLen - 1] == 'L')
        {
            correctedLocale[LocaleLen - 2] = 0;
        }

        /* There are separate simplified and traditional
         * locales called zh_HK_S and zh_HK_T.
         */
        अन्यथा यदि (uprv_strncmp(correctedLocale, "zh_HK", 5) == 0)
        {
            uprv_strcpy(correctedLocale, "zh_HK");
        }

        /* A special zh_CN_GBK locale...
        */
        अन्यथा यदि (uprv_strcmp(correctedLocale, "zh_CN_GBK") == 0)
        {
            uprv_strcpy(correctedLocale, "zh_CN");
        }

    }

    प्रत्यागम correctedLocale;
#यदि_अन्तः

}

#यदि !U_CHARSET_IS_UTF8
#यदि U_POSIX_LOCALE
/*
Due to various platform differences, one platform may specify a charset,
when they really mean a different charset. Remap the names so that they are
compatible with ICU. Only conflicting/ambiguous aliases should be resolved
here. Before adding anything to this function, please consider adding unique
names to the ICU alias table in the data directory.
*/
स्थिर ध्रुव वर्ण*
remapPlatformDependentCodepage(ध्रुव वर्ण *locale, ध्रुव वर्ण *name) {
    यदि (locale != शून्यसूचकः && *locale == 0) {
        /* Make sure that an empty locale is handled the same way. */
        locale = शून्यसूचकः;
    }
    यदि (name == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
#यदि U_PLATFORM == U_PF_AIX
    यदि (uprv_strcmp(name, "IBM-943") == 0) {
        /* Use the ASCII compatible ibm-943 */
        name = "Shift-JIS";
    }
    अन्यथा यदि (uprv_strcmp(name, "IBM-1252") == 0) {
        /* Use the windows-1252 that contains the Euro */
        name = "IBM-5348";
    }
#नोचेद्यदि U_PLATFORM == U_PF_SOLARIS
    यदि (locale != शून्यसूचकः && uprv_strcmp(name, "EUC") == 0) {
        /* Solaris underspecifies the "EUC" name. */
        यदि (uprv_strcmp(locale, "zh_CN") == 0) {
            name = "EUC-CN";
        }
        अन्यथा यदि (uprv_strcmp(locale, "zh_TW") == 0) {
            name = "EUC-TW";
        }
        अन्यथा यदि (uprv_strcmp(locale, "ko_KR") == 0) {
            name = "EUC-KR";
        }
    }
    अन्यथा यदि (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-954 is the best match.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    अन्यथा यदि (uprv_strcmp(name, "646") == 0) {
        /*
         * The default codepage given by Solaris is 646 but the C library routines treat it as if it was
         * ISO-8859-1 instead of US-ASCII(646).
         */
        name = "ISO-8859-1";
    }
#नोचेद्यदि U_PLATFORM_IS_DARWIN_BASED
    यदि (locale == शून्यसूचकः && *name == 0) {
        /*
        No locale was specified, and an empty name was passed in.
        This usually indicates that nl_langinfo didn't return valid information.
        Mac OS X uses UTF-8 by default (especially the locale data and console).
        */
        name = "UTF-8";
    }
    अन्यथा यदि (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
    अन्यथा यदि (locale != शून्यसूचकः && uprv_strcmp(locale, "en_US_POSIX") != 0 && uprv_strcmp(name, "US-ASCII") == 0) {
        /*
         * For non C/POSIX locale, default the code page to UTF-8 instead of US-ASCII.
         */
        name = "UTF-8";
    }
#नोचेद्यदि U_PLATFORM == U_PF_BSD
    यदि (uprv_strcmp(name, "CP949") == 0) {
        /* Remap CP949 to a similar codepage to avoid issues with backslash and won symbol. */
        name = "EUC-KR";
    }
#नोचेद्यदि U_PLATFORM == U_PF_HPUX
    यदि (locale != शून्यसूचकः && uprv_strcmp(locale, "zh_HK") == 0 && uprv_strcmp(name, "big5") == 0) {
        /* HP decided to extend big5 as hkbig5 even though it's not compatible :-( */
        /* zh_TW.big5 is not the same charset as zh_HK.big5! */
        name = "hkbig5";
    }
    अन्यथा यदि (uprv_strcmp(name, "eucJP") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
#नोचेद्यदि U_PLATFORM == U_PF_LINUX
    यदि (locale != शून्यसूचकः && uprv_strcmp(name, "euc") == 0) {
        /* Linux underspecifies the "EUC" name. */
        यदि (uprv_strcmp(locale, "korean") == 0) {
            name = "EUC-KR";
        }
        अन्यथा यदि (uprv_strcmp(locale, "japanese") == 0) {
            /* See comment below about eucJP */
            name = "eucjis";
        }
    }
    अन्यथा यदि (uprv_strcmp(name, "eucjp") == 0) {
        /*
        ibm-1350 is the best match, but unavailable.
        ibm-954 is mostly a superset of ibm-1350.
        ibm-33722 is the default for eucJP (similar to Windows).
        */
        name = "eucjis";
    }
    अन्यथा यदि (locale != शून्यसूचकः && uprv_strcmp(locale, "en_US_POSIX") != 0 &&
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
#यदि_अन्तः
    /* return nullptr when "" is passed in */
    यदि (*name == 0) {
        name = शून्यसूचकः;
    }
    प्रत्यागम name;
}

स्थिर ध्रुव वर्ण*
getCodepageFromPOSIXID(ध्रुव वर्ण *localeName, वर्ण * बफर, int32_t buff_क्षमता)
{
    वर्ण localeBuf[100];
    ध्रुव वर्ण *name = शून्यसूचकः;
    वर्ण *variant = शून्यसूचकः;

    यदि (localeName != शून्यसूचकः && (name = (uprv_strchr(localeName, '.'))) != शून्यसूचकः) {
        size_t locale_क्षमता = uprv_min(परिमाण(localeBuf), (name-localeName)+1);
        uprv_strncpy(localeBuf, localeName, locale_क्षमता);
        localeBuf[locale_क्षमता-1] = 0; /* ensure NUL termination */
        name = uprv_strncpy(बफर, name+1, buff_क्षमता);
        बफर[buff_क्षमता-1] = 0; /* ensure NUL termination */
        यदि ((variant = const_cast<वर्ण *>(uprv_strchr(name, '@'))) != शून्यसूचकः) {
            *variant = 0;
        }
        name = remapPlatformDependentCodepage(localeBuf, name);
    }
    प्रत्यागम name;
}
#यदि_अन्तः

स्थिर ध्रुव वर्ण*
int_getDefaultCodepage()
{
#यदि U_PLATFORM == U_PF_OS400
    uint32_t ccsid = 37; /* Default to ibm-37 */
    स्थिर वर्ण codepage[64];
    Qwc_JOBI0400_t jobinfo;
    Qus_EC_t error = { परिमाण(Qus_EC_t) }; /* SPI error code */

    EPT_CALL(QUSRJOBI)(&jobinfo, परिमाण(jobinfo), "JOBI0400",
        "*                         ", "                ", &error);

    यदि (error.अष्टकानि_उपलब्धम् == 0) {
        यदि (jobinfo.Coded_Char_समुच्चयः_ID != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Coded_Char_समुच्चयः_ID;
        }
        अन्यथा यदि (jobinfo.Default_Coded_Char_समुच्चयः_Id != 0xFFFF) {
            ccsid = (uint32_t)jobinfo.Default_Coded_Char_समुच्चयः_Id;
        }
        /* else use the default */
    }
    snprintf(codepage, परिमाण(codepage), "ibm-%d", ccsid);
    प्रत्यागम codepage;

#नोचेद्यदि U_PLATFORM == U_PF_OS390
    स्थिर वर्ण codepage[64];

    strncpy(codepage, nl_langinfo(CODESET),63-वाक्यदैर्घ्यम्(UCNV_SWAP_LFNL_OPTION_STRING));
    वाक्यं_योजय(codepage,UCNV_SWAP_LFNL_OPTION_STRING);
    codepage[63] = 0; /* NUL terminate */

    प्रत्यागम codepage;

#नोचेद्यदि U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API
    स्थिर वर्ण codepage[64];
    DWORD codepage_सङ्ख्या = 0;

#यदि U_PLATFORM_HAS_WINUWP_API == 1
    // UWP doesn't have a direct API to get the default ACP as Microsoft would rather
    // have folks use Unicode than a "system" code page, however this is the same
    // codepage as the system default locale codepage.  (FWIW, the system locale is
    // ONLY used for codepage, it should never be used for anything else)
    GetLocaleInfoEx(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE | LOCALE_RETURN_सङ्ख्या,
        (LPWSTR)&codepage_सङ्ख्या, परिमाण(codepage_सङ्ख्या) / परिमाण(WCHAR));
#अन्यथा
    // Win32 apps can call GetACP
    codepage_सङ्ख्या = GetACP();
#यदि_अन्तः
    // Special case for UTF-8
    यदि (codepage_सङ्ख्या == 65001)
    {
        प्रत्यागम "UTF-8";
    }
    // Windows codepages can look like windows-1252, so format the found number
    // the numbers are eclectic, however all valid system code pages, besides UTF-8
    // are between 3 and 19999
    यदि (codepage_सङ्ख्या > 0 && codepage_सङ्ख्या < 20000)
    {
        snprintf(codepage, परिमाण(codepage), "windows-%ld", codepage_सङ्ख्या);
        प्रत्यागम codepage;
    }
    // If the codepage number call failed then return UTF-8
    प्रत्यागम "UTF-8";

#नोचेद्यदि U_POSIX_LOCALE
    स्थिर वर्ण codesetName[100];
    ध्रुव वर्ण *localeName = शून्यसूचकः;
    ध्रुव वर्ण *name = शून्यसूचकः;

    localeName = uprv_getPOSIXIDForDefaultCodepage();
    uprv_memset(codesetName, 0, परिमाण(codesetName));
    /* On Solaris nl_langinfo returns C locale values unless setlocale
     * was called earlier.
     */
#यदि (U_HAVE_NL_LANGINFO_CODESET && U_PLATFORM != U_PF_SOLARIS)
    /* When available, check nl_langinfo first because it usually gives more
       useful names. It depends on LC_CTYPE.
       nl_langinfo may use the same buffer as setlocale. */
    {
        ध्रुव वर्ण *codeset = nl_langinfo(U_NL_LANGINFO_CODESET);
#यदि U_PLATFORM_IS_DARWIN_BASED || U_PLATFORM_IS_LINUX_BASED
        /*
         * On Linux and MacOSX, ensure that default codepage for non C/POSIX locale is UTF-8
         * instead of ASCII.
         */
        यदि (uprv_strcmp(localeName, "en_US_POSIX") != 0) {
            codeset = remapPlatformDependentCodepage(localeName, codeset);
        } अन्यथा
#यदि_अन्तः
        {
            codeset = remapPlatformDependentCodepage(शून्यसूचकः, codeset);
        }

        यदि (codeset != शून्यसूचकः) {
            uprv_strncpy(codesetName, codeset, परिमाण(codesetName));
            codesetName[परिमाण(codesetName)-1] = 0;
            प्रत्यागम codesetName;
        }
    }
#यदि_अन्तः

    /* Use setlocale in a nice way, and then check some environment variables.
       Maybe the application used setlocale already.
    */
    uprv_memset(codesetName, 0, परिमाण(codesetName));
    name = getCodepageFromPOSIXID(localeName, codesetName, परिमाण(codesetName));
    यदि (name) {
        /* if we can find the codeset name from setlocale, return that. */
        प्रत्यागम name;
    }

    यदि (*codesetName == 0)
    {
        /* Everything failed. Return US ASCII (ISO 646). */
        (शून्य)uprv_strcpy(codesetName, "US-ASCII");
    }
    प्रत्यागम codesetName;
#अन्यथा
    प्रत्यागम "US-ASCII";
#यदि_अन्तः
}


U_CAPI ध्रुव वर्ण*  U_EXPORT2
uprv_getDefaultCodepage()
{
    स्थिर वर्ण ध्रुव  *name = शून्यसूचकः;
    umtx_तालकम्(शून्यसूचकः);
    यदि (name == शून्यसूचकः) {
        name = int_getDefaultCodepage();
    }
    umtx_unlock(शून्यसूचकः);
    प्रत्यागम name;
}
#यदि_अन्तः  /* !U_CHARSET_IS_UTF8 */


/* end of platform-specific implementation -------------- */

/* version handling --------------------------------------------------------- */

U_CAPI शून्य U_EXPORT2
u_संस्करणम्_From_String(UVersionInfo संस्करणम्_Array, ध्रुव वर्ण *संस्करणम्_String) {
    वर्ण *अन्तः;
    uint16_t part=0;

    यदि(संस्करणम्_Array==शून्यसूचकः) {
        प्रत्यागम;
    }

    यदि(संस्करणम्_String!=शून्यसूचकः) {
        प्रति(;;) {
            संस्करणम्_Array[part]=(uint8_t)uprv_strtoul(संस्करणम्_String, &अन्तः, 10);
            यदि(अन्तः==संस्करणम्_String || ++part==U_MAX_संस्करणम्_दीर्घता || *अन्तः!=U_संस्करणम्_DELIMITER) {
                भङ्ग;
            }
            संस्करणम्_String=अन्तः+1;
        }
    }

    यावत्(part<U_MAX_संस्करणम्_दीर्घता) {
        संस्करणम्_Array[part++]=0;
    }
}

U_CAPI शून्य U_EXPORT2
u_संस्करणम्_From_UString(UVersionInfo संस्करणम्_Array, ध्रुव char16_t *संस्करणम्_String) {
    यदि(संस्करणम्_Array!=शून्यसूचकः && संस्करणम्_String!=शून्यसूचकः) {
        वर्ण संस्करणम्_Chars[U_MAX_संस्करणम्_STRING_दीर्घता+1];
        int32_t len = u_strlen(संस्करणम्_String);
        यदि(len>U_MAX_संस्करणम्_STRING_दीर्घता) {
            len = U_MAX_संस्करणम्_STRING_दीर्घता;
        }
        u_UChars_गन्तव्यम्_Chars(संस्करणम्_String, संस्करणम्_Chars, len);
        संस्करणम्_Chars[len]=0;
        u_संस्करणम्_From_String(संस्करणम्_Array, संस्करणम्_Chars);
    }
}

U_CAPI शून्य U_EXPORT2
u_संस्करणम्_गन्तव्यम्_String(ध्रुव UVersionInfo संस्करणम्_Array, वर्ण *संस्करणम्_String) {
    uint16_t count, part;
    uint8_t क्षेत्रम्_2;

    यदि(संस्करणम्_String==शून्यसूचकः) {
        प्रत्यागम;
    }

    यदि(संस्करणम्_Array==शून्यसूचकः) {
        संस्करणम्_String[0]=0;
        प्रत्यागम;
    }

    /* count how many fields need to be written */
    प्रति(count=4; count>0 && संस्करणम्_Array[count-1]==0; --count) {
    }

    यदि(count <= 1) {
        count = 2;
    }

    /* write the first part */
    /* write the decimal field value */
    क्षेत्रम्_2=संस्करणम्_Array[0];
    यदि(क्षेत्रम्_2>=100) {
        *संस्करणम्_String++=(वर्ण)('0'+क्षेत्रम्_2/100);
        क्षेत्रम्_2%=100;
    }
    यदि(क्षेत्रम्_2>=10) {
        *संस्करणम्_String++=(वर्ण)('0'+क्षेत्रम्_2/10);
        क्षेत्रम्_2%=10;
    }
    *संस्करणम्_String++=(वर्ण)('0'+क्षेत्रम्_2);

    /* write the following parts */
    प्रति(part=1; part<count; ++part) {
        /* write a dot first */
        *संस्करणम्_String++=U_संस्करणम्_DELIMITER;

        /* write the decimal field value */
        क्षेत्रम्_2=संस्करणम्_Array[part];
        यदि(क्षेत्रम्_2>=100) {
            *संस्करणम्_String++=(वर्ण)('0'+क्षेत्रम्_2/100);
            क्षेत्रम्_2%=100;
        }
        यदि(क्षेत्रम्_2>=10) {
            *संस्करणम्_String++=(वर्ण)('0'+क्षेत्रम्_2/10);
            क्षेत्रम्_2%=10;
        }
        *संस्करणम्_String++=(वर्ण)('0'+क्षेत्रम्_2);
    }

    /* NUL-terminate */
    *संस्करणम्_String=0;
}

U_CAPI शून्य U_EXPORT2
u_get_संस्करणम्(UVersionInfo संस्करणम्_Array) {
    (शून्य)copyright;   // Suppress unused variable warning from clang.
    u_संस्करणम्_From_String(संस्करणम्_Array, U_ICU_संस्करणम्);
}

/**
 * icucfg.h dependent code
 */

#यदि U_ENABLE_DYLOAD && HAVE_DLOPEN && !U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API

#यदि HAVE_DLFCN_H
#यदि_परिभाषितम् __MVS__
#यदि_अपरिभाषितम् __SUSV3
#परिभाषय __SUSV3 1
#यदि_अन्तः
#यदि_अन्तः
#अन्तर्भाव <dlfcn.h>
#यदि_अन्तः /* HAVE_DLFCN_H */

U_CAPI शून्य * U_EXPORT2
uprv_dl_open(ध्रुव वर्ण *libName, UError_संहिता *स्थितिः) {
  शून्य *ret = शून्यसूचकः;
  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम ret;
  ret =  dlopen(libName, RTLD_NOW|RTLD_GLOBAL);
  यदि(ret==शून्यसूचकः) {
#यदि_परिभाषितम् U_TRACE_DYLOAD
    प्रारूप_लिख("dlerror on dlopen(%s): %s\n", libName, dlerror());
#यदि_अन्तः
    *स्थितिः = U_MISSING_RESOURCE_ERROR;
  }
  प्रत्यागम ret;
}

U_CAPI शून्य U_EXPORT2
uprv_dl_close(शून्य *lib, UError_संहिता *स्थितिः) {
  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम;
  dlclose(lib);
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(शून्य *lib, ध्रुव वर्ण* sym, UError_संहिता *स्थितिः) {
  सङ्घ {
      UVoidFunction *fp;
      शून्य *vp;
  } uret;
  uret.fp = शून्यसूचकः;
  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम uret.fp;
  uret.vp = dlsym(lib, sym);
  यदि(uret.vp == शून्यसूचकः) {
#यदि_परिभाषितम् U_TRACE_DYLOAD
    प्रारूप_लिख("dlerror on dlsym(%p,%s): %s\n", lib,sym, dlerror());
#यदि_अन्तः
    *स्थितिः = U_MISSING_RESOURCE_ERROR;
  }
  प्रत्यागम uret.fp;
}

#नोचेद्यदि U_ENABLE_DYLOAD && U_PLATFORM_उपयुङ्क्ते_ONLY_WIN32_API && !U_PLATFORM_HAS_WINUWP_API

/* Windows API implementation. */
// Note: UWP does not expose/allow these APIs, so the UWP version gets the null implementation. */

U_CAPI शून्य * U_EXPORT2
uprv_dl_open(ध्रुव वर्ण *libName, UError_संहिता *स्थितिः) {
  HMODULE lib = शून्यसूचकः;

  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम शून्यसूचकः;

  lib = आरोपय_Library_A(libName);

  यदि(lib==शून्यसूचकः) {
    *स्थितिः = U_MISSING_RESOURCE_ERROR;
  }

  प्रत्यागम (शून्य*)lib;
}

U_CAPI शून्य U_EXPORT2
uprv_dl_close(शून्य *lib, UError_संहिता *स्थितिः) {
  HMODULE handle = (HMODULE)lib;
  यदि(U_FAILURE(*स्थितिः)) प्रत्यागम;

  FreeLibrary(handle);

  प्रत्यागम;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(शून्य *lib, ध्रुव वर्ण* sym, UError_संहिता *स्थितिः) {
  HMODULE handle = (HMODULE)lib;
  UVoidFunction* addr = शून्यसूचकः;

  यदि(U_FAILURE(*स्थितिः) || lib==शून्यसूचकः) प्रत्यागम शून्यसूचकः;

  addr = (UVoidFunction*)Get_Proc_स्थानाङ्कः(handle, sym);

  यदि(addr==शून्यसूचकः) {
    DWORD चरमः_Error = Get_चरमः_Error();
    यदि(चरमः_Error == ERROR_PROC_NOT_प्राप्तम्) {
      *स्थितिः = U_MISSING_RESOURCE_ERROR;
    } अन्यथा {
      *स्थितिः = U_UNSUPPORTED_ERROR; /* other unknown error. */
    }
  }

  प्रत्यागम addr;
}

#अन्यथा

/* No dynamic loading, null (nonexistent) implementation. */

U_CAPI शून्य * U_EXPORT2
uprv_dl_open(ध्रुव वर्ण *libName, UError_संहिता *स्थितिः) {
    (शून्य)libName;
    यदि(U_FAILURE(*स्थितिः)) प्रत्यागम शून्यसूचकः;
    *स्थितिः = U_UNSUPPORTED_ERROR;
    प्रत्यागम शून्यसूचकः;
}

U_CAPI शून्य U_EXPORT2
uprv_dl_close(शून्य *lib, UError_संहिता *स्थितिः) {
    (शून्य)lib;
    यदि(U_FAILURE(*स्थितिः)) प्रत्यागम;
    *स्थितिः = U_UNSUPPORTED_ERROR;
    प्रत्यागम;
}

U_CAPI UVoidFunction* U_EXPORT2
uprv_dlsym_func(शून्य *lib, ध्रुव वर्ण* sym, UError_संहिता *स्थितिः) {
  (शून्य)lib;
  (शून्य)sym;
  यदि(U_SUCCESS(*स्थितिः)) {
    *स्थितिः = U_UNSUPPORTED_ERROR;
  }
  प्रत्यागम (UVoidFunction*)शून्यसूचकः;
}

#यदि_अन्तः

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */

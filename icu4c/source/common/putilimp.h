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
*  FILE NAME : putilimp.h
*
*   Date        Name        Description
*   10/17/04    grhoten     Move internal functions from putil.h to this file.
******************************************************************************
*/

#నిర్వచితం_కాకపోతే PUTILIMP_H
#నిర్వచించు PUTILIMP_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/putil.h"

/**
 * \def U_SIGNED_RIGHT_SHIFT_IS_ARITHMETIC
 * Nearly all CPUs and compilers implement a right-shift of a signed integer
 * as an Arithmetic Shift Right which copies the sign bit (the Most Significant Bit (MSB))
 * into the vacated bits (sign extension).
 * For example, (int32_t)0xfff5fff3>>4 becomes 0xffff5fff and -1>>1=-1.
 *
 * This can be useful for storing a signed value in the upper bits
 * and another bit field in the lower bits.
 * The signed value can be retrieved by simple right-shifting.
 *
 * This is consistent with the Java language.
 *
 * However, the C standard allows compilers to implement a right-shift of a signed integer
 * as a Logical Shift Right which copies a 0 into the vacated bits.
 * For example, (int32_t)0xfff5fff3>>4 becomes 0x0fff5fff and -1>>1=0x7fffffff.
 *
 * Code that depends on the natural behavior should be guarded with this macro,
 * with an alternate path for unusual platforms.
 * @internal
 */
#నిర్వచితమైతే U_SIGNED_కుడి_జరుపు_IS_ARITHMETIC
    /* Use the predefined value. */
#లేకపోతే
    /*
     * Nearly all CPUs & compilers implement a right-shift of a signed integer
     * as an Arithmetic Shift Right (with sign extension).
     */
#   define U_SIGNED_కుడి_జరుపు_IS_ARITHMETIC 1
#అయితే_ముగింపు

/** Define this to 1 if your platform supports IEEE 754 floating point,
   to 0 if it does not. */
#నిర్వచితం_కాకపోతే IEEE_754
#   define IEEE_754 1
#అయితే_ముగింపు

/**
 * uintptr_t is an optional part of the standard definitions in stdint.h.
 * The opengroup.org documentation for stdint.h says
 * "On XSI-conformant systems, the intptr_t and uintptr_t types are required;
 * otherwise, they are optional."
 * We assume that when uintptr_t is defined, UINTPTR_MAX is defined as well.
 *
 * Do not use ptrdiff_t since it is signed. size_t is unsigned.
 */
/* TODO: This check fails on some z environments. Filed a ticket #9357 for this. */
#ఒకవేళ !defined(__intptr_t_defined) && !defined(UINTPTR_MAX) && (U_PLATFORM != U_PF_OS390)
రకనిర్వచనం size_t uintptr_t;
#అయితే_ముగింపు

/*===========================================================================*/
/** @{ Information about POSIX support                                       */
/*===========================================================================*/

#నిర్వచితమైతే U_HAVE_NL_LANGINFO_CODESET
    /* Use the predefined value. */
#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API || U_PLATFORM == U_PF_ANDROID || U_PLATFORM == U_PF_QNX
#   define U_HAVE_NL_LANGINFO_CODESET 0
#లేకపోతే
#   define U_HAVE_NL_LANGINFO_CODESET 1
#అయితే_ముగింపు

#నిర్వచితమైతే U_NL_LANGINFO_CODESET
    /* Use the predefined value. */
#లేదావేళ !U_HAVE_NL_LANGINFO_CODESET
#   define U_NL_LANGINFO_CODESET -1
#లేదావేళ U_PLATFORM == U_PF_OS400
   /* not defined */
#లేదావేళ U_PLATFORM == U_PF_HAIKU
   /* not defined */
#లేకపోతే
#   define U_NL_LANGINFO_CODESET CODESET
#అయితే_ముగింపు

#ఒకవేళ defined(U_TZSET) || defined(U_HAVE_TZSET)
    /* Use the predefined value. */
#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
    // UWP doesn't support tzset or environment variables for tz
#ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 0
#   define U_TZSET _tzset
#అయితే_ముగింపు
#లేదావేళ U_PLATFORM == U_PF_OS400
   /* not defined */
#లేదావేళ U_PLATFORM == U_PF_HAIKU
   /* not defined */
#లేదావేళ defined(__wasi__)
   /* not defined */
#లేకపోతే
#   define U_TZSET tzset
#అయితే_ముగింపు

#ఒకవేళ defined(U_TIMEZONE) || defined(U_HAVE_TIMEZONE)
    /* Use the predefined value. */
#లేదావేళ U_PLATFORM == U_PF_ANDROID
#   define U_TIMEZONE timezone
#లేదావేళ defined(__UCLIBC__)
    // uClibc does not have __timezone or _timezone.
#లేదావేళ defined(NEWLIB_సంస్కరణ)
#   define U_TIMEZONE _timezone
#లేదావేళ defined(__GLIBC__)
    // glibc
#   define U_TIMEZONE __timezone
#లేదావేళ U_PLATFORM_IS_LINUX_BASED
    // not defined
#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
#   define U_TIMEZONE _timezone
#లేదావేళ U_PLATFORM == U_PF_BSD && !defined(__NetBSD__)
   /* not defined */
#లేదావేళ U_PLATFORM == U_PF_OS400
   /* not defined */
#లేదావేళ U_PLATFORM == U_PF_IPHONE
   /* not defined */
#లేదావేళ defined(__wasi__)
   /* not defined */
#లేకపోతే
#   define U_TIMEZONE timezone
#అయితే_ముగింపు

#ఒకవేళ defined(U_TZNAME) || defined(U_HAVE_TZNAME)
    /* Use the predefined value. */
#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
    /* not usable on all windows platforms */
#ఒకవేళ U_PLATFORM_HAS_WINUWP_API == 0
#   define U_TZNAME _tzname
#అయితే_ముగింపు
#లేదావేళ U_PLATFORM == U_PF_OS400
   /* not defined */
#లేదావేళ U_PLATFORM == U_PF_HAIKU
    /* not defined, (well it is but a loop back to icu) */
#లేదావేళ defined(__wasi__)
   /* not defined */
#లేకపోతే
#   define U_TZNAME tzname
#అయితే_ముగింపు

#నిర్వచితమైతే U_HAVE_MMAP
    /* Use the predefined value. */
#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
#   define U_HAVE_MMAP 0
#లేకపోతే
#   define U_HAVE_MMAP 1
#అయితే_ముగింపు

#నిర్వచితమైతే U_HAVE_POPEN
    /* Use the predefined value. */
#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
#   define U_HAVE_POPEN 0
#లేదావేళ U_PLATFORM == U_PF_OS400
#   define U_HAVE_POPEN 0
#లేకపోతే
#   define U_HAVE_POPEN 1
#అయితే_ముగింపు

/**
 * \def U_HAVE_DIRENT_H
 * Defines whether dirent.h is available.
 * @internal
 */
#నిర్వచితమైతే U_HAVE_DIRENT_H
    /* Use the predefined value. */
#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
#   define U_HAVE_DIRENT_H 0
#లేకపోతే
#   define U_HAVE_DIRENT_H 1
#అయితే_ముగింపు

/** @} */

/*===========================================================================*/
/** @{ Programs used by ICU code                                             */
/*===========================================================================*/

/**
 * \def U_MAKE_IS_NMAKE
 * Defines whether the "make" program is Windows nmake.
 */
#నిర్వచితమైతే U_MAKE_IS_NMAKE
    /* Use the predefined value. */
#లేదావేళ U_PLATFORM == U_PF_WINDOWS
#   define U_MAKE_IS_NMAKE 1
#లేకపోతే
#   define U_MAKE_IS_NMAKE 0
#అయితే_ముగింపు

/** @} */

/*==========================================================================*/
/* Platform utilities                                                       */
/*==========================================================================*/

/**
 * Platform utilities isolates the platform dependencies of the
 * library.  For each platform which this code is ported to, these
 * functions may have to be re-implemented.
 */

/**
 * Floating point utility to determine if a double is Not a Number (NaN).
 * @internal
 */
U_CAPI UBool   U_EXPORT2 uprv_isNaN(ద్విగుణ d);
/**
 * Floating point utility to determine if a double has an infinite value.
 * @internal
 */
U_CAPI UBool   U_EXPORT2 uprv_isInfinite(ద్విగుణ d);
/**
 * Floating point utility to determine if a double has a positive infinite value.
 * @internal
 */
U_CAPI UBool   U_EXPORT2 uprv_is_ధనాత్మక_Infinity(ద్విగుణ d);
/**
 * Floating point utility to determine if a double has a negative infinite value.
 * @internal
 */
U_CAPI UBool   U_EXPORT2 uprv_isNegativeInfinity(ద్విగుణ d);
/**
 * Floating point utility that returns a Not a Number (NaN) value.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_getNaN(శూన్య);
/**
 * Floating point utility that returns an infinite value.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_getInfinity(శూన్య);

/**
 * Floating point utility to truncate a double.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_trunc(ద్విగుణ d);
/**
 * Floating point utility to calculate the floor of a double.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_floor(ద్విగుణ d);
/**
 * Floating point utility to calculate the ceiling of a double.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_ceil(ద్విగుణ d);
/**
 * Floating point utility to calculate the absolute value of a double.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_fabs(ద్విగుణ d);
/**
 * Floating point utility to calculate the fractional and integer parts of a double.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_modf(ద్విగుణ d, ద్విగుణ* pinteger);
/**
 * Floating point utility to calculate the remainder of a double divided by another double.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_fmod(ద్విగుణ d, ద్విగుణ y);
/**
 * Floating point utility to calculate d to the power of exponent (d^exponent).
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_pow(ద్విగుణ d, ద్విగుణ exponent);
/**
 * Floating point utility to calculate 10 to the power of exponent (10^exponent).
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_pow10(int32_t exponent);
/**
 * Floating point utility to calculate the maximum value of two doubles.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_fmax(ద్విగుణ d, ద్విగుణ y);
/**
 * Floating point utility to calculate the minimum value of two doubles.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_fmin(ద్విగుణ d, ద్విగుణ y);
/**
 * Private utility to calculate the maximum value of two integers.
 * @internal
 */
U_CAPI int32_t U_EXPORT2 uprv_max(int32_t d, int32_t y);
/**
 * Private utility to calculate the minimum value of two integers.
 * @internal
 */
U_CAPI int32_t U_EXPORT2 uprv_min(int32_t d, int32_t y);

#ఒకవేళ U_IS_BIG_ENDIAN
#   define uprv_isNegative(సంఖ్య) (*((గుర్తుగల అక్షరం *)&(సంఖ్య))<0)
#లేకపోతే
#   define uprv_isNegative(సంఖ్య) (*((గుర్తుగల అక్షరం *)&(సంఖ్య)+పరిమాణం(సంఖ్య)-1)<0)
#అయితే_ముగింపు

/**
 * Return the largest positive number that can be represented by an integer
 * type of arbitrary bit length.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_maxMantissa(శూన్య);

/**
 * Floating point utility to calculate the logarithm of a double.
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_log(ద్విగుణ d);

/**
 * Does common notion of rounding e.g. uprv_floor(x + 0.5);
 * @param x the double number
 * @return the rounded double
 * @internal
 */
U_CAPI ద్విగుణ  U_EXPORT2 uprv_విడత(ద్విగుణ x);

/**
 * Adds the signed integers a and b, storing the result in res.
 * Checks for signed integer overflow.
 * Similar to the GCC/Clang extension __builtin_add_overflow
 *
 * @param a The first operand.
 * @param b The second operand.
 * @param res a + b
 * @return true if overflow occurred; false if no overflow occurred.
 * @internal
 */
U_CAPI UBool U_EXPORT2 uprv_add32_అధికప్రవాహం(int32_t a, int32_t b, int32_t* res);

/**
 * Multiplies the signed integers a and b, storing the result in res.
 * Checks for signed integer overflow.
 * Similar to the GCC/Clang extension __builtin_mul_overflow
 *
 * @param a The first multiplicand.
 * @param b The second multiplicand.
 * @param res a * b
 * @return true if overflow occurred; false if no overflow occurred.
 * @internal
 */
U_CAPI UBool U_EXPORT2 uprv_mul32_అధికప్రవాహం(int32_t a, int32_t b, int32_t* res);

#ఒకవేళ 0
/**
 * Returns the number of digits after the decimal point in a double number x.
 *
 * @param x the double number
 * @return the number of digits after the decimal point in a double number x.
 * @internal
 */
/*U_CAPI int32_t  U_EXPORT2 uprv_digitsAfterDecimal(double x);*/
#అయితే_ముగింపు

#ఒకవేళ !U_CHARSET_IS_UTF8
/**
 * Please use ucnv_getDefaultName() instead.
 * Return the default codepage for this platform and locale.
 * This function can call setlocale() on Unix platforms. Please read the
 * platform documentation on setlocale() before calling this function.
 * @return the default codepage for this platform
 * @internal
 */
U_CAPI స్థిరరాశి అక్షరం*  U_EXPORT2 uprv_getDefaultCodepage(శూన్య);
#అయితే_ముగింపు

/**
 * Please use uloc_getDefault() instead.
 * Return the default locale ID string by querying the system, or
 *     zero if one cannot be found.
 * This function can call setlocale() on Unix platforms. Please read the
 * platform documentation on setlocale() before calling this function.
 * @return the default locale ID string
 * @internal
 */
U_CAPI స్థిరరాశి అక్షరం*  U_EXPORT2 uprv_getDefaultLocaleID(శూన్య);

/**
 * Time zone utilities
 *
 * Wrappers for C runtime library functions relating to timezones.
 * The t_tzset() function (similar to tzset) uses the current setting
 * of the environment variable TZ to assign values to three global
 * variables: daylight, timezone, and tzname. These variables have the
 * following meanings, and are declared in &lt;time.h&gt;.
 *
 *   daylight   Nonzero if daylight-saving-time zone (DST) is specified
 *              in TZ; otherwise, 0. Default value is 1.
 *   timezone   Difference in seconds between coordinated universal
 *              time and local time. E.g., -28,800 for PST (GMT-8hrs)
 *   tzname(0)  Three-letter time-zone name derived from TZ environment
 *              variable. E.g., "PST".
 *   tzname(1)  Three-letter DST zone name derived from TZ environment
 *              variable.  E.g., "PDT". If DST zone is omitted from TZ,
 *              tzname(1) is an empty string.
 *
 * Notes: For example, to set the TZ environment variable to correspond
 * to the current time zone in Germany, you can use one of the
 * following statements:
 *
 *   set TZ=GST1GDT
 *   set TZ=GST+1GDT
 *
 * If the TZ value is not set, t_tzset() attempts to use the time zone
 * information specified by the operating system. Under Windows NT
 * and Windows 95, this information is specified in the Control Panel's
 * Date/Time application.
 * @internal
 */
U_CAPI శూన్య     U_EXPORT2 uprv_tzset(శూన్య);

/**
 * Difference in seconds between coordinated universal
 * time and local time. E.g., -28,800 for PST (GMT-8hrs)
 * @return the difference in seconds between coordinated universal time and local time.
 * @internal
 */
U_CAPI int32_t  U_EXPORT2 uprv_timezone(శూన్య);

/**
 *   tzname(0)  Three-letter time-zone name derived from TZ environment
 *              variable. E.g., "PST".
 *   tzname(1)  Three-letter DST zone name derived from TZ environment
 *              variable.  E.g., "PDT". If DST zone is omitted from TZ,
 *              tzname(1) is an empty string.
 * @internal
 */
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2 uprv_tzname(పూర్ణాంకం n);

/**
 * Reset the global tzname cache.
 * @internal
 */
U_CAPI శూన్య uprv_tzname_clear_cache(శూన్య);

/**
 * Get UTC (GMT) time measured in milliseconds since 0:00 on 1/1/1970.
 * This function is affected by 'faketime' and should be the bottleneck for all user-visible ICU time functions.
 * @return the UTC time measured in milliseconds
 * @internal
 */
U_CAPI UDate U_EXPORT2 uprv_getUTCtime(శూన్య);

/**
 * Get UTC (GMT) time measured in milliseconds since 0:00 on 1/1/1970.
 * This function is not affected by 'faketime', so it should only be used by low level test functions- not by anything that
 * exposes time to the end user.
 * @return the UTC time measured in milliseconds
 * @internal
 */
U_CAPI UDate U_EXPORT2 uprv_getRawUTCtime(శూన్య);

/**
 * Determine whether a pathname is absolute or not, as defined by the platform.
 * @param path Pathname to test
 * @return true if the path is absolute
 * @internal (ICU 3.0)
 */
U_CAPI UBool U_EXPORT2 uprv_మార్గం_Is_Absolute(స్థిరరాశి అక్షరం *మార్గం);

/**
 * Use U_MAX_PTR instead of this function.
 * @param void pointer to test
 * @return the largest possible pointer greater than the base
 * @internal (ICU 3.8)
 */
U_CAPI శూన్య * U_EXPORT2 uprv_గరిష్ఠ_Ptr(శూన్య *పునాది);

/**
 * Maximum value of a (void*) - use to indicate the limit of an 'infinite' buffer.
 * In fact, buffer sizes must not exceed 2GB so that the difference between
 * the buffer limit and the buffer start can be expressed in an int32_t.
 *
 * The definition of U_MAX_PTR must fulfill the following conditions:
 * - return the largest possible pointer greater than base
 * - return a valid pointer according to the machine architecture (AS/400, 64-bit, etc.)
 * - avoid wrapping around at high addresses
 * - make sure that the returned pointer is not farther from base than 0x7fffffff bytes
 *
 * @param base The beginning of a buffer to find the maximum offset from
 * @internal
 */
#నిర్వచితం_కాకపోతే U_MAX_PTR
#  ఒకవేళ U_PLATFORM == U_PF_OS390 && !defined(_LP64)
    /* We have 31-bit pointers. */
#    define U_MAX_PTR(పునాది) ((శూన్య *)0x7fffffff)
#  elif U_PLATFORM == U_PF_OS400
#    define U_MAX_PTR(పునాది) uprv_గరిష్ఠ_Ptr((శూన్య *)పునాది)
#  elif 0
    /*
     * For platforms where pointers are scalar values (which is normal, but unlike i5/OS)
     * but that do not define uintptr_t.
     *
     * However, this does not work on modern compilers:
     * The C++ standard does not define pointer overflow, and allows compilers to
     * assume that p+u>p for any pointer p and any integer u>0.
     * Thus, modern compilers optimize away the ">" comparison.
     * (See ICU tickets #7187 and #8096.)
     */
#    define U_MAX_PTR(పునాది) \
    ((శూన్య *)(((అక్షరం *)(పునాది)+0x7fffffffu) > (అక్షరం *)(పునాది) \
        ? ((అక్షరం *)(పునాది)+0x7fffffffu) \
        : (అక్షరం *)-1))
#  లేకపోతే
    /* Default version. C++ standard compliant for scalar pointers. */
#    define U_MAX_PTR(పునాది) \
    ((శూన్య *)(((uintptr_t)(పునాది)+0x7fffffffu) > (uintptr_t)(పునాది) \
        ? ((uintptr_t)(పునాది)+0x7fffffffu) \
        : (uintptr_t)-1))
#  endif
#అయితే_ముగింపు


#నిర్వచితమైతే __cplusplus
/**
 * Pin a buffer capacity such that doing pointer arithmetic
 * on the destination pointer and capacity cannot overflow.
 *
 * The pinned capacity must fulfill the following conditions (for positive capacities):
 *   - dest + capacity is a valid pointer according to the machine architecture (AS/400, 64-bit, etc.)
 *   - (dest + capacity) >= dest
 *   - The size (in bytes) of T[capacity] does not exceed 0x7fffffff
 *
 * @param dest the destination buffer pointer.
 * @param capacity the requested buffer capacity, in units of type T.
 * @return the pinned capacity.
 * @internal
 */
మూస <typename T>
అంతఃవరుస int32_t pin_సామర్థ్యం(T *dest, int32_t సామర్థ్యం) {
    ఒకవేళ (సామర్థ్యం <= 0) { తిరిగి సామర్థ్యం; }

    uintptr_t destInt = (uintptr_t)dest;
    uintptr_t maxInt;

#  ఒకవేళ U_PLATFORM == U_PF_OS390 && !defined(_LP64)
    // We have 31-bit pointers.
    maxInt = 0x7fffffff;
#  elif U_PLATFORM == U_PF_OS400
    maxInt = (uintptr_t)uprv_గరిష్ఠ_Ptr((శూన్య *)dest);
#  లేకపోతే
    maxInt = destInt + 0x7fffffffu;
    ఒకవేళ (maxInt < destInt) {
        // Less than 2GB to the end of the address space.
        // Pin to that to prevent address overflow.
        maxInt = static_cast<uintptr_t>(-1);
    }
#  endif

    uintptr_t max_బైట్లు = maxInt - destInt;  // max. 2GB
    int32_t max_సామర్థ్యం = (int32_t)(max_బైట్లు / పరిమాణం(T));
    తిరిగి సామర్థ్యం <= max_సామర్థ్యం ? సామర్థ్యం : max_సామర్థ్యం;
}
#అయితే_ముగింపు   // __cplusplus

/*  Dynamic Library Functions */

రకనిర్వచనం శూన్య (UVoidFunction)(శూన్య);

#ఒకవేళ U_ENABLE_DYLOAD
/**
 * Load a library
 * @internal (ICU 4.4)
 */
U_CAPI శూన్య * U_EXPORT2 uprv_dl_open(స్థిరరాశి అక్షరం *libName, UError_సంకేతం *స్థితి_2);

/**
 * Close a library
 * @internal (ICU 4.4)
 */
U_CAPI శూన్య U_EXPORT2 uprv_dl_close( శూన్య *lib, UError_సంకేతం *స్థితి_2);

/**
 * Extract a symbol from a library (function)
 * @internal (ICU 4.8)
 */
U_CAPI UVoidFunction* U_EXPORT2 uprv_dlsym_func( శూన్య *lib, స్థిరరాశి అక్షరం *చిహ్నం_Name, UError_సంకేతం *స్థితి_2);

/**
 * Extract a symbol from a library (function)
 * Not implemented, no clients.
 * @internal
 */
/* U_CAPI void * U_EXPORT2 uprv_dlsym_data( void *lib, const char *symbolName, UErrorCode *status); */

#అయితే_ముగింపు

/**
 * Define malloc and related functions
 * @internal
 */
#ఒకవేళ U_PLATFORM == U_PF_OS400
# define uprv_default_malloc(x) _C_TS_malloc(x)
# define uprv_default_realloc(x,y) _C_TS_realloc(x,y)
# define uprv_default_free(x) _C_TS_free(x)
/* also _C_TS_calloc(x) */
#లేకపోతే
/* C defaults */
# define uprv_default_malloc(x) జ్ఞాపకం_ఇవ్వు(x)
# define uprv_default_realloc(x,y) realloc(x,y)
# define uprv_default_free(x) విడుదల(x)
#అయితే_ముగింపు


#అయితే_ముగింపు

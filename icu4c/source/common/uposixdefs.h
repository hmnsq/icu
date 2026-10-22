// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  uposixdefs.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011jul25
*   created by: Markus W. Scherer
*
*   Common definitions for implementation files working with POSIX functions.
*   *Important*: #include this file before any other header files!
*/

#که_نه_تعریف_شوی __UPOSIXDEFS_H__
#تعریف_کړه __UPOSIXDEFS_H__

/*
 * Define _XOPEN_SOURCE for access to POSIX functions.
 *
 * We cannot use U_PLATFORM from platform.h/utypes.h because
 * "The Open Group Base Specifications"
 * chapter "2.2 The Compilation Environment" says:
 * "In the compilation of an application that #defines a feature test macro
 * specified by IEEE Std 1003.1-2001,
 * no header defined by IEEE Std 1003.1-2001 shall be included prior to
 * the definition of the feature test macro."
 */
#که_تعریف_شوی XOPEN_سرچینه
    /* Use the predefined value. */
#وإلا
    /*
     * Version 6.0:
     * The Open Group Base Specifications Issue 6 (IEEE Std 1003.1, 2004 Edition)
     * also known as
     * SUSv3 = Open Group Single UNIX Specification, Version 3 (UNIX03)
     *
     * Note: This definition used to be in C source code (e.g., putil.c)
     * and define _XOPEN_SOURCE to different values depending on __STDC_VERSION__.
     * In C++ source code (e.g., putil.cpp), __STDC_VERSION__ is not defined at all.
     */
#   define XOPEN_سرچینه 600
#د_که_پای

/*
 * Make sure things like realpath and such functions work.
 * Poorly upgraded Solaris machines can't have this defined.
 * Cleanly installed Solaris can use this #define.
 *
 * z/OS needs this definition for timeval and to get usleep.
 */
#إذا !defined(XOPEN_سرچینه_EXTENDED) && defined(__TOS_MVS__)
#   define XOPEN_سرچینه_EXTENDED 1
#د_که_پای

/**
 * Solaris says:
 *   "...it is invalid to compile an XPG6 or a POSIX.1-2001 application with anything other
 *   than a c99 or later compiler."
 * Apparently C++11 is not "or later". Work around this.
 */
#إذا defined(__cplusplus) && (defined(sun) || defined(__sun)) && !defined (_STDC_C99)
#   define _STDC_C99
#د_که_پای

#إذا !defined _POSIX_C_SOURCE && \
    defined(__APPLE__) && defined(__MACH__) && !defined(__clang__)
// Needed to prevent EOWNERDEAD issues with GCC on Mac
#تعریف_کړه _POSIX_C_SOURCE 200809L
#د_که_پای

/* Prevent _XOPEN_SOURCE from breaking build on macOS when aligned_alloc exists. */
#إذا defined(__APPLE__) && !defined(DARWIN_C_سرچینه)
#   define DARWIN_C_سرچینه
#د_که_پای

#د_که_پای  /* __UPOSIXDEFS_H__ */

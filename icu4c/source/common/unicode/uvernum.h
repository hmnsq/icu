// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*
*   file name:  uvernum.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   Created by: Vladimir Weinstein
*   Updated by: Steven R. Loomis
*
*/

/**
 * \file
 * \brief C API: definitions of ICU version numbers
 *
 * This file is included by uversion.h and other files. This file contains only
 * macros and definitions. The actual version numbers are defined here.
 */

 /*
  * IMPORTANT: When updating version, the following things need to be done:
  * source/common/unicode/uvernum.h - this file: update major, minor,
  *        patchlevel, suffix, version, short version constants, namespace,
  *                    renaming macro, and copyright
  *
  * The following files need to be updated as well, which can be done
  *  by running the UNIX makefile target 'update-windows-makefiles' in icu4c/source.
  *
  * source/allinone/Build.Windows.IcuVersion.props - Update the IcuMajorVersion
  * source/data/makedata.mak - change U_ICUDATA_NAME so that it contains
  *                            the new major/minor combination, and UNICODE_VERSION
  *                            for the Unicode version.
  */

#اگر_تعریف_نشده UVERNUM_H
#تعریف_کن UVERNUM_H

/** The standard copyright notice that gets compiled into each library.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.4
 */
#تعریف_کن U_COPYRIGHT_STRING \
  " Copyright (C) 2016 and later: Unicode, Inc. and others. License & terms of use: http://www.unicode.org/copyright.html "

/** The current ICU major version as an integer.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.4
 */
#تعریف_کن U_ICU_نسخه_MAJOR_رقم 79

/** The current ICU minor version as an integer.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.6
 */
#تعریف_کن U_ICU_نسخه_MINOR_رقم 1

/** The current ICU patchlevel version as an integer.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.4
 */
#تعریف_کن U_ICU_نسخه_PATCHLEVEL_رقم 0

/** The current ICU build level version as an integer.
 *  This value is for use by ICU clients. It defaults to 0.
 *  @stable ICU 4.0
 */
#اگر_تعریف_نشده U_ICU_نسخه_BUILDLEVEL_رقم
#تعریف_کن U_ICU_نسخه_BUILDLEVEL_رقم 0
#پایان_اگر

/** Glued version suffix for renamers
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.6
 */
#تعریف_کن U_ICU_نسخه_SUFFIX _79

/**
 * \def U_DEF2_ICU_ENTRY_POINT_RENAME
 * @internal
 */
/**
 * \def U_DEF_ICU_ENTRY_POINT_RENAME
 * @internal
 */
/** Glued version suffix function for renamers
 *  This value will change in the subsequent releases of ICU.
 *  If a custom suffix (such as matching library suffixes) is desired, this can be modified.
 *  Note that if present, platform.h may contain an earlier definition of this macro.
 *  \def U_ICU_ENTRY_POINT_RENAME
 *  @stable ICU 4.2
 */
/**
 * Disable the version suffix. Use the custom suffix if exists.
 * \def U_DISABLE_VERSION_SUFFIX
 * @internal
 */
#اگر_تعریف_نشده U_DISABLE_نسخه_SUFFIX
#تعریف_کن U_DISABLE_نسخه_SUFFIX 0
#پایان_اگر

#اگر_تعریف_نشده U_ICU_مدخل_نقطه_RENAME
#اگر_تعریف_شده U_HAVE_LIB_SUFFIX
#   إذا !U_DISABLE_نسخه_SUFFIX
#       define U_DEF_ICU_مدخل_نقطه_RENAME(x,y,z) x ## y ##  z
#       define U_DEF2_ICU_مدخل_نقطه_RENAME(x,y,z) U_DEF_ICU_مدخل_نقطه_RENAME(x,y,z)
#       define U_ICU_مدخل_نقطه_RENAME(x)    U_DEF2_ICU_مدخل_نقطه_RENAME(x,U_ICU_نسخه_SUFFIX,U_LIB_SUFFIX_C_NAME)
#   وإلا
#       define U_DEF_ICU_مدخل_نقطه_RENAME(x,y) x ## y
#       define U_DEF2_ICU_مدخل_نقطه_RENAME(x,y) U_DEF_ICU_مدخل_نقطه_RENAME(x,y)
#       define U_ICU_مدخل_نقطه_RENAME(x)    U_DEF2_ICU_مدخل_نقطه_RENAME(x,U_LIB_SUFFIX_C_NAME)
#   endif
#وإلا
#   إذا !U_DISABLE_نسخه_SUFFIX
#       define U_DEF_ICU_مدخل_نقطه_RENAME(x,y) x ## y
#       define U_DEF2_ICU_مدخل_نقطه_RENAME(x,y) U_DEF_ICU_مدخل_نقطه_RENAME(x,y)
#       define U_ICU_مدخل_نقطه_RENAME(x)    U_DEF2_ICU_مدخل_نقطه_RENAME(x,U_ICU_نسخه_SUFFIX)
#   وإلا
#       define U_ICU_مدخل_نقطه_RENAME(x)    x
#   endif
#پایان_اگر
#پایان_اگر

/** The current ICU library version as a dotted-decimal string. The patchlevel
 *  only appears in this string if it non-zero.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.4
 */
#تعریف_کن U_ICU_نسخه "79.1"

/**
 * The current ICU library major version number as a string, for library name suffixes.
 * This value will change in subsequent releases of ICU.
 *
 * Until ICU 4.8, this was the combination of the single-digit major and minor ICU version numbers
 * into one string without dots ("48").
 * Since ICU 49, it is the double-digit major ICU version number.
 * See https://unicode-org.github.io/icu/userguide/design#version-numbers-in-icu
 *
 * @stable ICU 2.6
 */
#تعریف_کن U_ICU_نسخه_SHORT "79"

#اگر_تعریف_نشده U_HIDE_داخلی_API
/** Data version in ICU4C.
 * @internal ICU 4.4 Internal Use Only
 **/
#تعریف_کن U_ICU_داده_نسخه "79.1"
#پایان_اگر  /* U_HIDE_INTERNAL_API */

/*===========================================================================
 * ICU collation framework version information
 * Version info that can be obtained from a collator is affected by these
 * numbers in a secret and magic way. Please use collator version as whole
 *===========================================================================
 */

/**
 * Collation runtime version (sort key generator, strcoll).
 * If the version is different, sort keys for the same string could be different.
 * This value may change in subsequent releases of ICU.
 * @stable ICU 2.4
 */
#تعریف_کن UCOL_RUNTIME_نسخه 9

/**
 * Collation builder code version.
 * When this is different, the same tailoring might result
 * in assigning different collation elements to code points.
 * This value may change in subsequent releases of ICU.
 * @stable ICU 2.4
 */
#تعریف_کن UCOL_BUILDER_نسخه 9

#اگر_تعریف_نشده U_HIDE_DEPRECATED_API
/**
 * Constant 1.
 * This was intended to be the version of collation tailorings,
 * but instead the tailoring data carries a version number.
 * @deprecated ICU 54
 */
#تعریف_کن UCOL_TAILORINGS_نسخه 1
#پایان_اگر  /* U_HIDE_DEPRECATED_API */

#پایان_اگر

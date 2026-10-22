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

#اگر_غیر_تعریف_شدہ UVERNUM_H
#تعریف_کرو UVERNUM_H

/** The standard copyright notice that gets compiled into each library.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.4
 */
#تعریف_کرو U_COPYRIGHT_STRING \
  " Copyright (C) 2016 and later: Unicode, Inc. and others. License & terms of use: http://www.unicode.org/copyright.html "

/** The current ICU major version as an integer.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.4
 */
#تعریف_کرو U_ICU_نسخہ_MAJOR_عدد 79

/** The current ICU minor version as an integer.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.6
 */
#تعریف_کرو U_ICU_نسخہ_MINOR_عدد 1

/** The current ICU patchlevel version as an integer.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.4
 */
#تعریف_کرو U_ICU_نسخہ_PATCHLEVEL_عدد 0

/** The current ICU build level version as an integer.
 *  This value is for use by ICU clients. It defaults to 0.
 *  @stable ICU 4.0
 */
#اگر_غیر_تعریف_شدہ U_ICU_نسخہ_BUILDLEVEL_عدد
#تعریف_کرو U_ICU_نسخہ_BUILDLEVEL_عدد 0
#اگر_ختم

/** Glued version suffix for renamers
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.6
 */
#تعریف_کرو U_ICU_نسخہ_SUFFIX _79

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
#اگر_غیر_تعریف_شدہ U_DISABLE_نسخہ_SUFFIX
#تعریف_کرو U_DISABLE_نسخہ_SUFFIX 0
#اگر_ختم

#اگر_غیر_تعریف_شدہ U_ICU_اندراج_نقطہ_RENAME
#اگر_تعریف_شدہ U_HAVE_LIB_SUFFIX
#   اگر !U_DISABLE_نسخہ_SUFFIX
#       define U_DEF_ICU_اندراج_نقطہ_RENAME(x,y,z) x ## y ##  z
#       define U_DEF2_ICU_اندراج_نقطہ_RENAME(x,y,z) U_DEF_ICU_اندراج_نقطہ_RENAME(x,y,z)
#       define U_ICU_اندراج_نقطہ_RENAME(x)    U_DEF2_ICU_اندراج_نقطہ_RENAME(x,U_ICU_نسخہ_SUFFIX,U_LIB_SUFFIX_C_NAME)
#   ورنہ
#       define U_DEF_ICU_اندراج_نقطہ_RENAME(x,y) x ## y
#       define U_DEF2_ICU_اندراج_نقطہ_RENAME(x,y) U_DEF_ICU_اندراج_نقطہ_RENAME(x,y)
#       define U_ICU_اندراج_نقطہ_RENAME(x)    U_DEF2_ICU_اندراج_نقطہ_RENAME(x,U_LIB_SUFFIX_C_NAME)
#   endif
#ورنہ
#   اگر !U_DISABLE_نسخہ_SUFFIX
#       define U_DEF_ICU_اندراج_نقطہ_RENAME(x,y) x ## y
#       define U_DEF2_ICU_اندراج_نقطہ_RENAME(x,y) U_DEF_ICU_اندراج_نقطہ_RENAME(x,y)
#       define U_ICU_اندراج_نقطہ_RENAME(x)    U_DEF2_ICU_اندراج_نقطہ_RENAME(x,U_ICU_نسخہ_SUFFIX)
#   ورنہ
#       define U_ICU_اندراج_نقطہ_RENAME(x)    x
#   endif
#اگر_ختم
#اگر_ختم

/** The current ICU library version as a dotted-decimal string. The patchlevel
 *  only appears in this string if it non-zero.
 *  This value will change in the subsequent releases of ICU
 *  @stable ICU 2.4
 */
#تعریف_کرو U_ICU_نسخہ "79.1"

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
#تعریف_کرو U_ICU_نسخہ_SHORT "79"

#اگر_غیر_تعریف_شدہ U_HIDE_اندرونی_API
/** Data version in ICU4C.
 * @internal ICU 4.4 Internal Use Only
 **/
#تعریف_کرو U_ICU_ڈیٹا_نسخہ "79.1"
#اگر_ختم  /* U_HIDE_INTERNAL_API */

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
#تعریف_کرو UCOL_RUNTIME_نسخہ 9

/**
 * Collation builder code version.
 * When this is different, the same tailoring might result
 * in assigning different collation elements to code points.
 * This value may change in subsequent releases of ICU.
 * @stable ICU 2.4
 */
#تعریف_کرو UCOL_BUILDER_نسخہ 9

#اگر_غیر_تعریف_شدہ U_HIDE_DEPRECATED_API
/**
 * Constant 1.
 * This was intended to be the version of collation tailorings,
 * but instead the tailoring data carries a version number.
 * @deprecated ICU 54
 */
#تعریف_کرو UCOL_TAILORINGS_نسخہ 1
#اگر_ختم  /* U_HIDE_DEPRECATED_API */

#اگر_ختم

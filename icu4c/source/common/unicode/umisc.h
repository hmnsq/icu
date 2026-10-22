// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  umisc.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999oct15
*   created by: Markus W. Scherer
*/

#که_نه_تعریف_شوی UMISC_H
#تعریف_کړه UMISC_H

#ضمن "unicode/utypes.h"

/**
 * \file
 * \brief C API: Miscellaneous definitions
 *
 *  This file contains miscellaneous definitions for the C APIs.
 */

U_CDECL_BEGIN

/** A struct representing a range of text containing a specific field 
 *  @stable ICU 2.0
 */
تعريفنوع بنية UFieldPosition {
  /**
   * The field 
   * @stable ICU 2.0
   */
  int32_t ډګر_2;
  /**
   * The start of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t begin_شاخص;
  /** 
   * The limit of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t پای_ټکی_شاخص;
} UFieldPosition;

#إذا !UCONFIG_NO_SERVICE
/**
 * Opaque type returned by registerInstance, registerFactory and unregister for service registration.
 * @stable ICU 2.6
 */
تعريفنوع ثابت فراغ* URegistry_کلی;
#د_که_پای

U_CDECL_پای_ټکی

#د_که_پای

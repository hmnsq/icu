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

#اگر_غیر_تعریف_شدہ UMISC_H
#تعریف_کرو UMISC_H

#شامل "unicode/utypes.h"

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
قسمتعریف ساخت UFieldPosition {
  /**
   * The field 
   * @stable ICU 2.0
   */
  int32_t میدان_2;
  /**
   * The start of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t begin_اشاریہ;
  /** 
   * The limit of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t انتہا_اشاریہ;
} UFieldPosition;

#اگر !UCONFIG_NO_SERVICE
/**
 * Opaque type returned by registerInstance, registerFactory and unregister for service registration.
 * @stable ICU 2.6
 */
قسمتعریف ثابت خالی* URegistry_کلید;
#اگر_ختم

U_CDECL_انتہا

#اگر_ختم

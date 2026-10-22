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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UMISC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UMISC_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UFieldPosition {
  /**
   * The field 
   * @stable ICU 2.0
   */
  int32_t ਖੇਤਰ_2;
  /**
   * The start of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t begin_ਸੂਚਕਾਂਕ;
  /** 
   * The limit of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t ਸਿਰਾ_ਸੂਚਕਾਂਕ;
} UFieldPosition;

#ਜੇ !UCONFIG_NO_SERVICE
/**
 * Opaque type returned by registerInstance, registerFactory and unregister for service registration.
 * @stable ICU 2.6
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅਚਲ ਖਾਲੀ* URegistry_ਕੁੰਜੀ;
#ਜੇ_ਅੰਤ

U_CDECL_ਸਿਰਾ

#ਜੇ_ਅੰਤ

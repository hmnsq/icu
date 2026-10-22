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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UMISC_H
#ವ್ಯಾಖ್ಯಾನಿಸು UMISC_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UFieldPosition {
  /**
   * The field 
   * @stable ICU 2.0
   */
  int32_t ಕ್ಷೇತ್ರ_2;
  /**
   * The start of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t begin_ಸೂಚ್ಯಂಕ;
  /** 
   * The limit of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t ಕೊನೆ_ಸೂಚ್ಯಂಕ;
} UFieldPosition;

#ಆದರೆ !UCONFIG_NO_SERVICE
/**
 * Opaque type returned by registerInstance, registerFactory and unregister for service registration.
 * @stable ICU 2.6
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* URegistry_ಕೀಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

U_CDECL_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

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

#अगर_अघोषित UMISC_H
#घोषणा UMISC_H

#समावेश "unicode/utypes.h"

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
प्रकार काष्ठा UFieldPosition {
  /**
   * The field 
   * @stable ICU 2.0
   */
  int32_t field;
  /**
   * The start of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t begin_सूचकांक;
  /** 
   * The limit of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t end_सूचकांक;
} UFieldPosition;

#अगर !UCONFIG_NO_SERVICE
/**
 * Opaque type returned by registerInstance, registerFactory and unregister for service registration.
 * @stable ICU 2.6
 */
प्रकार स्थिर व्योम* URegistry_कुंजी;
#पूर्ण_अगर

U_CDECL_END

#पूर्ण_अगर

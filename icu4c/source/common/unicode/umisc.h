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

#నిర్వచితం_కాకపోతే UMISC_H
#నిర్వచించు UMISC_H

#అన్తర్భావ "unicode/utypes.h"

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
రకనిర్వచనం నిర్మాణం UFieldPosition {
  /**
   * The field 
   * @stable ICU 2.0
   */
  int32_t క్షేత్రం_2;
  /**
   * The start of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t begin_సూచీ;
  /** 
   * The limit of the text range containing field 
   * @stable ICU 2.0
   */
  int32_t చివర_సూచీ;
} UFieldPosition;

#ఒకవేళ !UCONFIG_NO_SERVICE
/**
 * Opaque type returned by registerInstance, registerFactory and unregister for service registration.
 * @stable ICU 2.6
 */
రకనిర్వచనం స్థిరరాశి శూన్య* URegistry_కీలకం;
#అయితే_ముగింపు

U_CDECL_చివర

#అయితే_ముగింపు

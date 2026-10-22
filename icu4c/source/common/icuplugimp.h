// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2009-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
*  FILE NAME : icuplugimp.h
* 
*  Internal functions for the ICU plugin system
*
*   Date         Name        Description
*   10/29/2009   sl          New.
******************************************************************************
*/


#ifndef ICUPLUGIMP_H
#define ICUPLUGIMP_H

#অন্তর্ভাব "unicode/icuplug.h"

#যদি UCONFIG_ENABLE_PLUGINS

/*========================*/
/** @{ Library Manipulation  
 */

/**
 * Open a library, adding a reference count if needed.
 * @param libName library name to load
 * @param status error code
 * @return the library pointer, or NULL
 * @internal internal use only
 */
U_CAPI শূন্য * U_EXPORT2
uplug_openLibrary(ধ্রুব বর্ণ *libName, UErrorCode *status);

/**
 * Close a library, if its reference count is 0
 * @param lib the library to close
 * @param status error code
 * @internal internal use only
 */
U_CAPI শূন্য U_EXPORT2
uplug_closeLibrary(শূন্য *lib, UErrorCode *status);

/**
 * Get a library's name, or NULL if not found.
 * @param lib the library's name
 * @param status error code
 * @return the library name, or NULL if not found.
 * @internal internal use only
 */
U_CAPI  বর্ণ * U_EXPORT2
uplug_findLibrary(শূন্য *lib, UErrorCode *status);

/** @} */

/*========================*/
/** {@ ICU Plugin internal interfaces
 */

/**
 * Initialize the plugins 
 * @param status error result
 * @internal - Internal use only.
 */
U_CAPI শূন্য U_EXPORT2
uplug_init(UErrorCode *status);

/**
 * Get raw plug N
 * @internal - Internal use only
 */ 
U_CAPI UPlugData* U_EXPORT2
uplug_getPlugInternal(int32_t n);

/**
 * Get the name of the plugin file. 
 * @internal - Internal use only.
 */
U_CAPI ধ্রুব বর্ণ* U_EXPORT2
uplug_getPluginFile(শূন্য);

/** @} */

#endif

#endif

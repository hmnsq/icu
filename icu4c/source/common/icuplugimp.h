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


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ICUPLUGIMP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICUPLUGIMP_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/icuplug.h"

#ਜੇ UCONFIG_ENABLE_PLUGINS

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
U_CAPI ਖਾਲੀ * U_EXPORT2
uplug_openLibrary(ਅਚਲ ਅੱਖਰ *libName, UError_ਕੋਡ *ਸਥਿਤੀ);

/**
 * Close a library, if its reference count is 0
 * @param lib the library to close
 * @param status error code
 * @internal internal use only
 */
U_CAPI ਖਾਲੀ U_EXPORT2
uplug_closeLibrary(ਖਾਲੀ *lib, UError_ਕੋਡ *ਸਥਿਤੀ);

/**
 * Get a library's name, or NULL if not found.
 * @param lib the library's name
 * @param status error code
 * @return the library name, or NULL if not found.
 * @internal internal use only
 */
U_CAPI  ਅੱਖਰ * U_EXPORT2
uplug_ਲੱਭੋ_Library(ਖਾਲੀ *lib, UError_ਕੋਡ *ਸਥਿਤੀ);

/** @} */

/*========================*/
/** {@ ICU Plugin internal interfaces
 */

/**
 * Initialize the plugins 
 * @param status error result
 * @internal - Internal use only.
 */
U_CAPI ਖਾਲੀ U_EXPORT2
uplug_init(UError_ਕੋਡ *ਸਥਿਤੀ);

/**
 * Get raw plug N
 * @internal - Internal use only
 */ 
U_CAPI UPlug_ਅੰਕੜੇ* U_EXPORT2
uplug_get_Plug_ਅੰਦਰੂਨੀ(int32_t n);

/**
 * Get the name of the plugin file. 
 * @internal - Internal use only.
 */
U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
uplug_getPluginFile(ਖਾਲੀ);

/** @} */

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ

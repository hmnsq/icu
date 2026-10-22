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


#اگر_تعریف_نشده ICUPLUGIMP_H
#تعریف_کن ICUPLUGIMP_H

#ضمن "unicode/icuplug.h"

#إذا UCONFIG_ENABLE_PLUGINS

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
U_CAPI فراغ * U_EXPORT2
uplug_openLibrary(ثابت حرف *libName, UError_کد *وضعیت);

/**
 * Close a library, if its reference count is 0
 * @param lib the library to close
 * @param status error code
 * @internal internal use only
 */
U_CAPI فراغ U_EXPORT2
uplug_closeLibrary(فراغ *lib, UError_کد *وضعیت);

/**
 * Get a library's name, or NULL if not found.
 * @param lib the library's name
 * @param status error code
 * @return the library name, or NULL if not found.
 * @internal internal use only
 */
U_CAPI  حرف * U_EXPORT2
uplug_پیدا_کن_Library(فراغ *lib, UError_کد *وضعیت);

/** @} */

/*========================*/
/** {@ ICU Plugin internal interfaces
 */

/**
 * Initialize the plugins 
 * @param status error result
 * @internal - Internal use only.
 */
U_CAPI فراغ U_EXPORT2
uplug_init(UError_کد *وضعیت);

/**
 * Get raw plug N
 * @internal - Internal use only
 */ 
U_CAPI UPlug_داده* U_EXPORT2
uplug_get_Plug_داخلی(int32_t n);

/**
 * Get the name of the plugin file. 
 * @internal - Internal use only.
 */
U_CAPI ثابت حرف* U_EXPORT2
uplug_getPluginFile(فراغ);

/** @} */

#پایان_اگر

#پایان_اگر

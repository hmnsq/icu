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


#यदि_अपरिभाषितम् ICUPLUGIMP_H
#परिभाषय ICUPLUGIMP_H

#अन्तर्भाव "unicode/icuplug.h"

#यदि UCONFIG_ENABLE_PLUGINS

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
U_CAPI शून्य * U_EXPORT2
uplug_openLibrary(ध्रुव वर्ण *libName, UError_संहिता *स्थितिः);

/**
 * Close a library, if its reference count is 0
 * @param lib the library to close
 * @param status error code
 * @internal internal use only
 */
U_CAPI शून्य U_EXPORT2
uplug_closeLibrary(शून्य *lib, UError_संहिता *स्थितिः);

/**
 * Get a library's name, or NULL if not found.
 * @param lib the library's name
 * @param status error code
 * @return the library name, or NULL if not found.
 * @internal internal use only
 */
U_CAPI  वर्ण * U_EXPORT2
uplug_findLibrary(शून्य *lib, UError_संहिता *स्थितिः);

/** @} */

/*========================*/
/** {@ ICU Plugin internal interfaces
 */

/**
 * Initialize the plugins 
 * @param status error result
 * @internal - Internal use only.
 */
U_CAPI शून्य U_EXPORT2
uplug_init(UError_संहिता *स्थितिः);

/**
 * Get raw plug N
 * @internal - Internal use only
 */ 
U_CAPI UPlug_दत्त* U_EXPORT2
uplug_get_Plug_आन्तरिकम्(int32_t n);

/**
 * Get the name of the plugin file. 
 * @internal - Internal use only.
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
uplug_getPluginFile(शून्य);

/** @} */

#यदि_अन्तः

#यदि_अन्तः

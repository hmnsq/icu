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


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ICUPLUGIMP_H
#ವ್ಯಾಖ್ಯಾನಿಸು ICUPLUGIMP_H

#ಅನ್ತರ್ಭಾವ "unicode/icuplug.h"

#ಆದರೆ UCONFIG_ENABLE_PLUGINS

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
U_CAPI ಶೂನ್ಯ * U_EXPORT2
uplug_openLibrary(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libName, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

/**
 * Close a library, if its reference count is 0
 * @param lib the library to close
 * @param status error code
 * @internal internal use only
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_closeLibrary(ಶೂನ್ಯ *lib, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

/**
 * Get a library's name, or NULL if not found.
 * @param lib the library's name
 * @param status error code
 * @return the library name, or NULL if not found.
 * @internal internal use only
 */
U_CAPI  ಅಕ್ಷರ * U_EXPORT2
uplug_ಕಂಡುಹಿಡಿ_Library(ಶೂನ್ಯ *lib, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

/** @} */

/*========================*/
/** {@ ICU Plugin internal interfaces
 */

/**
 * Initialize the plugins 
 * @param status error result
 * @internal - Internal use only.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uplug_init(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

/**
 * Get raw plug N
 * @internal - Internal use only
 */ 
U_CAPI UPlug_ದತ್ತಾಂಶ* U_EXPORT2
uplug_get_Plug_ಆಂತರಿಕ(int32_t n);

/**
 * Get the name of the plugin file. 
 * @internal - Internal use only.
 */
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
uplug_getPluginFile(ಶೂನ್ಯ);

/** @} */

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ

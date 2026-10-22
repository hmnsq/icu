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


#నిర్వచితం_కాకపోతే ICUPLUGIMP_H
#నిర్వచించు ICUPLUGIMP_H

#అన్తర్భావ "unicode/icuplug.h"

#ఒకవేళ UCONFIG_ENABLE_PLUGINS

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
U_CAPI శూన్య * U_EXPORT2
uplug_openLibrary(స్థిరరాశి అక్షరం *libName, UError_సంకేతం *స్థితి_2);

/**
 * Close a library, if its reference count is 0
 * @param lib the library to close
 * @param status error code
 * @internal internal use only
 */
U_CAPI శూన్య U_EXPORT2
uplug_closeLibrary(శూన్య *lib, UError_సంకేతం *స్థితి_2);

/**
 * Get a library's name, or NULL if not found.
 * @param lib the library's name
 * @param status error code
 * @return the library name, or NULL if not found.
 * @internal internal use only
 */
U_CAPI  అక్షరం * U_EXPORT2
uplug_కనుగొను_Library(శూన్య *lib, UError_సంకేతం *స్థితి_2);

/** @} */

/*========================*/
/** {@ ICU Plugin internal interfaces
 */

/**
 * Initialize the plugins 
 * @param status error result
 * @internal - Internal use only.
 */
U_CAPI శూన్య U_EXPORT2
uplug_init(UError_సంకేతం *స్థితి_2);

/**
 * Get raw plug N
 * @internal - Internal use only
 */ 
U_CAPI UPlug_దత్తాంశం* U_EXPORT2
uplug_get_Plug_అంతర్గత(int32_t n);

/**
 * Get the name of the plugin file. 
 * @internal - Internal use only.
 */
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uplug_getPluginFile(శూన్య);

/** @} */

#అయితే_ముగింపు

#అయితే_ముగింపు

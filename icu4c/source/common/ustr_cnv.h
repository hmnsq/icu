// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 1999-2010, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ustr_cnv.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004Aug27
*   created by: George Rhoten
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USTR_CNV_IMP_H
#ವ್ಯಾಖ್ಯಾನಿಸು USTR_CNV_IMP_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

/**
 * Get the default converter. This is a commonly used converter
 * that is used for the ustring and UnicodeString API.
 * Remember to use the u_releaseDefaultConverter when you are done.
 * @internal
 */
U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);


/**
 * Release the default converter to the converter cache.
 * @internal
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
u_releaseDefaultConverter(UConverter *converter);

/**
 * Flush the default converter, if cached. 
 * @internal
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
u_flushDefaultConverter(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ

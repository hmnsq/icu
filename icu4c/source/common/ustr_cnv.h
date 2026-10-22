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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USTR_CNV_IMP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USTR_CNV_IMP_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"

#ਜੇ !UCONFIG_NO_CONVERSION

/**
 * Get the default converter. This is a commonly used converter
 * that is used for the ustring and UnicodeString API.
 * Remember to use the u_releaseDefaultConverter when you are done.
 * @internal
 */
U_CAPI UConverter* U_EXPORT2
u_getDefaultConverter(UError_ਕੋਡ *ਸਥਿਤੀ);


/**
 * Release the default converter to the converter cache.
 * @internal
 */
U_CAPI ਖਾਲੀ U_EXPORT2
u_releaseDefaultConverter(UConverter *converter);

/**
 * Flush the default converter, if cached. 
 * @internal
 */
U_CAPI ਖਾਲੀ U_EXPORT2
u_flushDefaultConverter(ਖਾਲੀ);

#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ

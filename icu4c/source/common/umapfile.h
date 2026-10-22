// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************/

/*----------------------------------------------------------------------------------
 *
 *       Memory mapped file wrappers for use by the ICU Data Implementation
 *
 *           Porting note:  The implementation of these functions is very platform specific.
 *             Not all platforms can do real memory mapping.  Those that can't
 *             still must implement these functions, getting the data into memory using
 *             whatever means are available.
 *
 *            These functions are part of the ICU internal implementation, and
 *            are not intended to be used directly by applications.
 *
 *----------------------------------------------------------------------------------*/

#నిర్వచితం_కాకపోతే __UMAPFILE_H__
#నిర్వచించు __UMAPFILE_H__

#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "putilimp.h"

U_CAPI  UBool U_EXPORT2 uprv_mapFile(UData_జ్ఞాపకం *pdm, స్థిరరాశి అక్షరం *మార్గం, UError_సంకేతం *స్థితి_2);
U_CFUNC శూన్య  uprv_unmapFile(UData_జ్ఞాపకం *p_దత్తాంశం);

/* MAP_NONE: no memory mapping, no file access at all */
#నిర్వచించు MAP_NONE        0
#నిర్వచించు MAP_WIN32       1
#నిర్వచించు MAP_POSIX       2
#నిర్వచించు MAP_STDIO       3

#ఒకవేళ UCONFIG_NO_FILE_IO
#   define MAP_IMPLEMENTATION MAP_NONE
#లేదావేళ U_PLATFORM_ఉపయోగిస్తుంది_ONLY_WIN32_API
#   define MAP_IMPLEMENTATION MAP_WIN32
#లేదావేళ U_HAVE_MMAP || U_PLATFORM == U_PF_OS390
#   define MAP_IMPLEMENTATION MAP_POSIX
#లేకపోతే /* unknown platform, no memory map implementation: use stdio.h and uprv_malloc() instead */
#   define MAP_IMPLEMENTATION MAP_STDIO
#అయితే_ముగింపు

#అయితే_ముగింపు

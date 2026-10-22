// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2010, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************/


/*----------------------------------------------------------------------------------
 *
 *  UDataMemory     A class-like struct that serves as a handle to a piece of memory
 *                  that contains some ICU data (resource, converters, whatever.)
 *
 *                  When an application opens ICU data (with udata_open, for example,
 *                  a UDataMemory * is returned.
 *
 *----------------------------------------------------------------------------------*/
#ifndef __UDATAMEM_H__
#define __UDATAMEM_H__

#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "ucmndata.h"

ಸಂರಚನಾ UDataMemory {
    ಧ್ರುವ commonDataFuncs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ಧ್ರುವ DataHeader *pHeader;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ಧ್ರುವ ಶೂನ್ಯ       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             heapAllocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    ಶೂನ್ಯ             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    ಶೂನ್ಯ             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           length;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UDataMemory* U_EXPORT2 UDataMemory_createNewInstance(UErrorCode *pErr);
U_CFUNC ಶೂನ್ಯ         UDatamemory_assign  (UDataMemory *dest, UDataMemory *source);
U_CFUNC ಶೂನ್ಯ         UDataMemory_init    (UDataMemory *This);
U_CFUNC UBool        UDataMemory_isLoaded(ಧ್ರುವ UDataMemory *This);
U_CFUNC ಶೂನ್ಯ         UDataMemory_setData (UDataMemory *This, ಧ್ರುವ ಶೂನ್ಯ *dataAddr);

U_CFUNC ಧ್ರುವ DataHeader* UDataMemory_normalizeDataPointer(ಧ್ರುವ ಶೂನ್ಯ* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_getLength(ಧ್ರುವ UDataMemory *pData);

U_CAPI ಧ್ರುವ ಶೂನ್ಯ* U_EXPORT2
udata_getRawMemory(ಧ್ರುವ UDataMemory* pData U_LIFETIME_BOUND);

#endif

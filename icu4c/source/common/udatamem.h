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

#ضمن "unicode/udata.h"
#ضمن "ucmndata.h"

بنية UDataMemory {
    ثابت commonDataFuncs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ثابت DataHeader *pHeader;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ثابت فراغ       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             heapAllocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    فراغ             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    فراغ             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           length;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UDataMemory* U_EXPORT2 UDataMemory_createNewInstance(UErrorCode *pErr);
U_CFUNC فراغ         UDatamemory_assign  (UDataMemory *dest, UDataMemory *source);
U_CFUNC فراغ         UDataMemory_init    (UDataMemory *This);
U_CFUNC UBool        UDataMemory_isLoaded(ثابت UDataMemory *This);
U_CFUNC فراغ         UDataMemory_setData (UDataMemory *This, ثابت فراغ *dataAddr);

U_CFUNC ثابت DataHeader* UDataMemory_normalizeDataPointer(ثابت فراغ* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_getLength(ثابت UDataMemory *pData);

U_CAPI ثابت فراغ* U_EXPORT2
udata_getRawMemory(ثابت UDataMemory* pData U_LIFETIME_BOUND);

#endif

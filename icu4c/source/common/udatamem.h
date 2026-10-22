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

#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "ucmndata.h"

கட்டமைப்பு UDataMemory {
    மாறிலி commonDataFuncs  *vFuncs;      /* Function Pointers for accessing TOC             */

    மாறிலி DataHeader *pHeader;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    மாறிலி வெறுமை       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             heapAllocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    வெறுமை             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    வெறுமை             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           length;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UDataMemory* U_EXPORT2 UDataMemory_createNewInstance(UErrorCode *pErr);
U_CFUNC வெறுமை         UDatamemory_assign  (UDataMemory *dest, UDataMemory *source);
U_CFUNC வெறுமை         UDataMemory_init    (UDataMemory *This);
U_CFUNC UBool        UDataMemory_isLoaded(மாறிலி UDataMemory *This);
U_CFUNC வெறுமை         UDataMemory_setData (UDataMemory *This, மாறிலி வெறுமை *dataAddr);

U_CFUNC மாறிலி DataHeader* UDataMemory_normalizeDataPointer(மாறிலி வெறுமை* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_getLength(மாறிலி UDataMemory *pData);

U_CAPI மாறிலி வெறுமை* U_EXPORT2
udata_getRawMemory(மாறிலி UDataMemory* pData U_LIFETIME_BOUND);

#endif

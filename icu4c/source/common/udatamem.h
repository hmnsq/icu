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

#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "ucmndata.h"

ഘടന UDataMemory {
    സ്ഥിരം commonDataFuncs  *vFuncs;      /* Function Pointers for accessing TOC             */

    സ്ഥിരം DataHeader *pHeader;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    സ്ഥിരം ശൂന്യം       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             heapAllocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    ശൂന്യം             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    ശൂന്യം             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           length;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UDataMemory* U_EXPORT2 UDataMemory_createNewInstance(UErrorCode *pErr);
U_CFUNC ശൂന്യം         UDatamemory_assign  (UDataMemory *dest, UDataMemory *source);
U_CFUNC ശൂന്യം         UDataMemory_init    (UDataMemory *This);
U_CFUNC UBool        UDataMemory_isLoaded(സ്ഥിരം UDataMemory *This);
U_CFUNC ശൂന്യം         UDataMemory_setData (UDataMemory *This, സ്ഥിരം ശൂന്യം *dataAddr);

U_CFUNC സ്ഥിരം DataHeader* UDataMemory_normalizeDataPointer(സ്ഥിരം ശൂന്യം* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_getLength(സ്ഥിരം UDataMemory *pData);

U_CAPI സ്ഥിരം ശൂന്യം* U_EXPORT2
udata_getRawMemory(സ്ഥിരം UDataMemory* pData U_LIFETIME_BOUND);

#endif

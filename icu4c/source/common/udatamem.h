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

#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "ucmndata.h"

સંરચના UDataMemory {
    અચલ commonDataFuncs  *vFuncs;      /* Function Pointers for accessing TOC             */

    અચલ DataHeader *pHeader;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    અચલ શૂન્ય       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             heapAllocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    શૂન્ય             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    શૂન્ય             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           length;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UDataMemory* U_EXPORT2 UDataMemory_createNewInstance(UErrorCode *pErr);
U_CFUNC શૂન્ય         UDatamemory_assign  (UDataMemory *dest, UDataMemory *source);
U_CFUNC શૂન્ય         UDataMemory_init    (UDataMemory *This);
U_CFUNC UBool        UDataMemory_isLoaded(અચલ UDataMemory *This);
U_CFUNC શૂન્ય         UDataMemory_setData (UDataMemory *This, અચલ શૂન્ય *dataAddr);

U_CFUNC અચલ DataHeader* UDataMemory_normalizeDataPointer(અચલ શૂન્ય* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_getLength(અચલ UDataMemory *pData);

U_CAPI અચલ શૂન્ય* U_EXPORT2
udata_getRawMemory(અચલ UDataMemory* pData U_LIFETIME_BOUND);

#endif

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

#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "ucmndata.h"

ସଂରଚନା UDataMemory {
    ଅଚଳ commonDataFuncs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ଅଚଳ DataHeader *pHeader;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ଅଚଳ ଶୂନ୍ୟ       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             heapAllocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    ଶୂନ୍ୟ             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    ଶୂନ୍ୟ             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           length;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UDataMemory* U_EXPORT2 UDataMemory_createNewInstance(UErrorCode *pErr);
U_CFUNC ଶୂନ୍ୟ         UDatamemory_assign  (UDataMemory *dest, UDataMemory *source);
U_CFUNC ଶୂନ୍ୟ         UDataMemory_init    (UDataMemory *This);
U_CFUNC UBool        UDataMemory_isLoaded(ଅଚଳ UDataMemory *This);
U_CFUNC ଶୂନ୍ୟ         UDataMemory_setData (UDataMemory *This, ଅଚଳ ଶୂନ୍ୟ *dataAddr);

U_CFUNC ଅଚଳ DataHeader* UDataMemory_normalizeDataPointer(ଅଚଳ ଶୂନ୍ୟ* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_getLength(ଅଚଳ UDataMemory *pData);

U_CAPI ଅଚଳ ଶୂନ୍ୟ* U_EXPORT2
udata_getRawMemory(ଅଚଳ UDataMemory* pData U_LIFETIME_BOUND);

#endif

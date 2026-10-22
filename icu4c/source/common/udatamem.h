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

#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "ucmndata.h"

কাঠামো UDataMemory {
    ধ্রুবক common_তথ্য_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ধ্রুবক তথ্য_Header *pHeader;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ধ্রুবক শূন্য       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             heapAllocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    শূন্য             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    শূন্য             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           দৈর্ঘ্য;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UDataMemory* U_EXPORT2 UDataMemory_createNewInstance(UErrorCode *pErr);
U_CFUNC শূন্য         UDatamemory_assign  (UDataMemory *dest, UDataMemory *source);
U_CFUNC শূন্য         UDataMemory_init    (UDataMemory *This);
U_CFUNC UBool        UDataMemory_isLoaded(ধ্রুবক UDataMemory *This);
U_CFUNC শূন্য         UData_Memory_set_তথ্য (UDataMemory *This, ধ্রুবক শূন্য *তথ্য_Addr);

U_CFUNC ধ্রুবক তথ্য_Header* UData_Memory_normalize_তথ্য_Pointer(ধ্রুবক শূন্য* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_দৈর্ঘ্য(ধ্রুবক UDataMemory *p_তথ্য);

U_CAPI ধ্রুবক শূন্য* U_EXPORT2
udata_getRawMemory(ধ্রুবক UDataMemory* p_তথ্য U_LIFETIME_BOUND);

#endif

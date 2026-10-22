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
#अगर_अघोषित __UDATAMEM_H__
#घोषणा __UDATAMEM_H__

#समावेश "unicode/udata.h"
#समावेश "ucmndata.h"

काष्ठा UDataMemory {
    स्थिर common_तथ्य_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    स्थिर तथ्य_Header *pHeader;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    स्थिर व्योम       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             heapAllocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    व्योम             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    व्योम             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           लंबाई;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UDataMemory* U_EXPORT2 UDataMemory_createNewInstance(UErrorCode *pErr);
U_CFUNC व्योम         UDatamemory_assign  (UDataMemory *dest, UDataMemory *source);
U_CFUNC व्योम         UDataMemory_init    (UDataMemory *This);
U_CFUNC UBool        UDataMemory_isLoaded(स्थिर UDataMemory *This);
U_CFUNC व्योम         UData_Memory_set_तथ्य (UDataMemory *This, स्थिर व्योम *तथ्य_Addr);

U_CFUNC स्थिर तथ्य_Header* UData_Memory_normalize_तथ्य_Pointer(स्थिर व्योम* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_लंबाई(स्थिर UDataMemory *p_तथ्य);

U_CAPI स्थिर व्योम* U_EXPORT2
udata_getRawMemory(स्थिर UDataMemory* p_तथ्य U_LIFETIME_BOUND);

#पूर्ण_अगर

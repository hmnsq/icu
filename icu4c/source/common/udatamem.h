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
#यदि_अपरिभाषितम् __UDATAMEM_H__
#परिभाषय __UDATAMEM_H__

#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "ucmndata.h"

संरचना UData_स्मृतिः {
    ध्रुव common_दत्त_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ध्रुव दत्त_शीर्षकम् *p_शीर्षकम्;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ध्रुव शून्य       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             heapAllocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    शून्य             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    शून्य             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           दीर्घता;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UData_स्मृतिः* U_EXPORT2 UData_स्मृतिः_create_New_Instance(UError_संहिता *pErr);
U_CFUNC शून्य         UDatamemory_नियोजय  (UData_स्मृतिः *dest, UData_स्मृतिः *source);
U_CFUNC शून्य         UData_स्मृतिः_init    (UData_स्मृतिः *This);
U_CFUNC UBool        UData_स्मृतिः_is_Loaded(ध्रुव UData_स्मृतिः *This);
U_CFUNC शून्य         UData_स्मृतिः_समुच्चयः_दत्त (UData_स्मृतिः *This, ध्रुव शून्य *दत्त_Addr);

U_CFUNC ध्रुव दत्त_शीर्षकम्* UData_स्मृतिः_normalize_दत्त_Pointer(ध्रुव शून्य* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_दीर्घता(ध्रुव UData_स्मृतिः *p_दत्त);

U_CAPI ध्रुव शून्य* U_EXPORT2
udata_get_Raw_स्मृतिः(ध्रुव UData_स्मृतिः* p_दत्त U_LIFETIME_BOUND);

#यदि_अन्तः

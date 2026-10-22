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
#اگر_غیر_تعریف_شدہ __UDATAMEM_H__
#تعریف_کرو __UDATAMEM_H__

#شامل "unicode/udata.h"
#شامل "ucmndata.h"

ساخت UData_حافظہ {
    ثابت common_ڈیٹا_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ثابت ڈیٹا_سرخی *p_سرخی;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ثابت خالی       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             ڈھیر_Allocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    خالی             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    خالی             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           لمبائی;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UData_حافظہ* U_EXPORT2 UData_حافظہ_create_New_Instance(UError_رمز *pErr);
U_CFUNC خالی         UDatamemory_تفویض  (UData_حافظہ *dest, UData_حافظہ *ماخذ);
U_CFUNC خالی         UData_حافظہ_init    (UData_حافظہ *This);
U_CFUNC UBool        UData_حافظہ_is_Loaded(ثابت UData_حافظہ *This);
U_CFUNC خالی         UData_حافظہ_سیٹ_ڈیٹا (UData_حافظہ *This, ثابت خالی *ڈیٹا_Addr);

U_CFUNC ثابت ڈیٹا_سرخی* UData_حافظہ_normalize_ڈیٹا_اشارہ_گر(ثابت خالی* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_لمبائی(ثابت UData_حافظہ *p_ڈیٹا);

U_CAPI ثابت خالی* U_EXPORT2
udata_get_Raw_حافظہ(ثابت UData_حافظہ* p_ڈیٹا U_LIFETIME_BOUND);

#اگر_ختم

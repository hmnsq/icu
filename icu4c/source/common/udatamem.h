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
#اگر_تعریف_نشده __UDATAMEM_H__
#تعریف_کن __UDATAMEM_H__

#ضمن "unicode/udata.h"
#ضمن "ucmndata.h"

بنية UData_حافظه {
    ثابت common_داده_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ثابت داده_سرآیند *p_سرآیند;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ثابت فراغ       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             انباشته_Allocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    فراغ             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    فراغ             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           درازی;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UData_حافظه* U_EXPORT2 UData_حافظه_create_New_Instance(UError_کد *pErr);
U_CFUNC فراغ         UDatamemory_تخصیص_بده  (UData_حافظه *dest, UData_حافظه *منبع);
U_CFUNC فراغ         UData_حافظه_init    (UData_حافظه *This);
U_CFUNC UBool        UData_حافظه_is_Loaded(ثابت UData_حافظه *This);
U_CFUNC فراغ         UData_حافظه_مجموعه_داده (UData_حافظه *This, ثابت فراغ *داده_Addr);

U_CFUNC ثابت داده_سرآیند* UData_حافظه_normalize_داده_اشاره_گر(ثابت فراغ* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_درازی(ثابت UData_حافظه *p_داده);

U_CAPI ثابت فراغ* U_EXPORT2
udata_get_Raw_حافظه(ثابت UData_حافظه* p_داده U_LIFETIME_BOUND);

#پایان_اگر

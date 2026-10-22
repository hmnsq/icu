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
#جے_بنا_تعریف __UDATAMEM_H__
#تعریف_کرو __UDATAMEM_H__

#ضمن "unicode/udata.h"
#ضمن "ucmndata.h"

بنية UData_حافظہ {
    ثابت common_انکڑے_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ثابت انکڑے_سرخی *p_سرخی;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ثابت فراغ       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             ڈھیر_Allocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    فراغ             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    فراغ             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           لمائی;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UData_حافظہ* U_EXPORT2 UData_حافظہ_create_New_Instance(UError_کوڈ *pErr);
U_CFUNC فراغ         UDatamemory_سونپو  (UData_حافظہ *dest, UData_حافظہ *سروت);
U_CFUNC فراغ         UData_حافظہ_init    (UData_حافظہ *This);
U_CFUNC UBool        UData_حافظہ_is_Loaded(ثابت UData_حافظہ *This);
U_CFUNC فراغ         UData_حافظہ_سیٹ_انکڑے (UData_حافظہ *This, ثابت فراغ *انکڑے_Addr);

U_CFUNC ثابت انکڑے_سرخی* UData_حافظہ_normalize_انکڑے_اشارہ_گر(ثابت فراغ* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_لمائی(ثابت UData_حافظہ *p_انکڑے);

U_CAPI ثابت فراغ* U_EXPORT2
udata_get_Raw_حافظہ(ثابت UData_حافظہ* p_انکڑے U_LIFETIME_BOUND);

#جے_انت

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
#جيڪڏهن_وصف_نه_ٿيل __UDATAMEM_H__
#وصف_ڪريو __UDATAMEM_H__

#ضمن "unicode/udata.h"
#ضمن "ucmndata.h"

بنية UData_يادگيري {
    ثابت common_معلومات_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ثابت معلومات_سرو *p_سرو;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ثابت فراغ       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             ڍير_Allocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    فراغ             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    فراغ             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           ڊيگهائي;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UData_يادگيري* U_EXPORT2 UData_يادگيري_create_New_Instance(UError_ڪوڊ *pErr);
U_CFUNC فراغ         UDatamemory_سونپيو  (UData_يادگيري *dest, UData_يادگيري *ذريعو);
U_CFUNC فراغ         UData_يادگيري_init    (UData_يادگيري *This);
U_CFUNC UBool        UData_يادگيري_is_Loaded(ثابت UData_يادگيري *This);
U_CFUNC فراغ         UData_يادگيري_سيٽ_معلومات (UData_يادگيري *This, ثابت فراغ *معلومات_Addr);

U_CFUNC ثابت معلومات_سرو* UData_يادگيري_normalize_معلومات_اشارو(ثابت فراغ* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_ڊيگهائي(ثابت UData_يادگيري *p_معلومات);

U_CAPI ثابت فراغ* U_EXPORT2
udata_get_Raw_يادگيري(ثابت UData_يادگيري* p_معلومات U_LIFETIME_BOUND);

#جيڪڏهن_پورو

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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UDATAMEM_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UDATAMEM_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucmndata.h"

ਬਣਤਰ UData_ਸਮ੍ਰਿਤੀ {
    ਅਚਲ common_ਅੰਕੜੇ_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *p_ਸਿਰਲੇਖ;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    ਅਚਲ ਖਾਲੀ       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             ਢੇਰ_Allocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    ਖਾਲੀ             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    ਖਾਲੀ             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           ਲੰਮਾਈ;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UData_ਸਮ੍ਰਿਤੀ* U_EXPORT2 UData_ਸਮ੍ਰਿਤੀ_create_New_Instance(UError_ਕੋਡ *pErr);
U_CFUNC ਖਾਲੀ         UDatamemory_ਸੌਂਪੋ  (UData_ਸਮ੍ਰਿਤੀ *dest, UData_ਸਮ੍ਰਿਤੀ *ਸਰੋਤ);
U_CFUNC ਖਾਲੀ         UData_ਸਮ੍ਰਿਤੀ_init    (UData_ਸਮ੍ਰਿਤੀ *This);
U_CFUNC UBool        UData_ਸਮ੍ਰਿਤੀ_is_Loaded(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *This);
U_CFUNC ਖਾਲੀ         UData_ਸਮ੍ਰਿਤੀ_ਸੈੱਟ_ਅੰਕੜੇ (UData_ਸਮ੍ਰਿਤੀ *This, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ_Addr);

U_CFUNC ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ* UData_ਸਮ੍ਰਿਤੀ_normalize_ਅੰਕੜੇ_ਸੂਚਕ(ਅਚਲ ਖਾਲੀ* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_ਲੰਮਾਈ(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ);

U_CAPI ਅਚਲ ਖਾਲੀ* U_EXPORT2
udata_get_Raw_ਸਮ੍ਰਿਤੀ(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ* p_ਅੰਕੜੇ U_LIFETIME_BOUND);

#ਜੇ_ਅੰਤ

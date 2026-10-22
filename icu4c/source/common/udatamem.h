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
#నిర్వచితం_కాకపోతే __UDATAMEM_H__
#నిర్వచించు __UDATAMEM_H__

#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "ucmndata.h"

నిర్మాణం UData_జ్ఞాపకం {
    స్థిరరాశి common_దత్తాంశం_Funcs  *vFuncs;      /* Function Pointers for accessing TOC             */

    స్థిరరాశి దత్తాంశం_శీర్షిక *p_శీర్షిక;     /* Header of the memory being described by this    */
                                   /*   UDataMemory object.                           */
    స్థిరరాశి శూన్య       *toc;         /* For common memory, table of contents for        */
                                   /*   the pieces within.                            */
    UBool             కుప్ప_Allocated;  /* True if this UDataMemory Object is on the    */
                                   /*  heap and thus needs to be deleted when closed. */

    శూన్య             *mapAddr;     /* For mapped or allocated memory, the start addr. */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data.                           */
    శూన్య             *map;         /* Handle, or other data, OS dependent.            */
                                   /* Only non-null if a close operation should unmap */
                                   /*  the associated data, and additional info       */
                                   /*   beyond the mapAddr is needed to do that.      */
    int32_t           నిడివి;      /* Length of the data in bytes; -1 if unknown.     */
};

U_CAPI  UData_జ్ఞాపకం* U_EXPORT2 UData_జ్ఞాపకం_create_New_Instance(UError_సంకేతం *pErr);
U_CFUNC శూన్య         UDatamemory_కేటాయించు  (UData_జ్ఞాపకం *dest, UData_జ్ఞాపకం *ఆధారం);
U_CFUNC శూన్య         UData_జ్ఞాపకం_init    (UData_జ్ఞాపకం *This);
U_CFUNC UBool        UData_జ్ఞాపకం_is_Loaded(స్థిరరాశి UData_జ్ఞాపకం *This);
U_CFUNC శూన్య         UData_జ్ఞాపకం_సమితి_దత్తాంశం (UData_జ్ఞాపకం *This, స్థిరరాశి శూన్య *దత్తాంశం_Addr);

U_CFUNC స్థిరరాశి దత్తాంశం_శీర్షిక* UData_జ్ఞాపకం_normalize_దత్తాంశం_సూచిక(స్థిరరాశి శూన్య* p U_LIFETIME_BOUND);

U_CAPI int32_t U_EXPORT2
udata_get_నిడివి(స్థిరరాశి UData_జ్ఞాపకం *p_దత్తాంశం);

U_CAPI స్థిరరాశి శూన్య* U_EXPORT2
udata_get_Raw_జ్ఞాపకం(స్థిరరాశి UData_జ్ఞాపకం* p_దత్తాంశం U_LIFETIME_BOUND);

#అయితే_ముగింపు

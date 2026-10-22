// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2011, International Business Machines
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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/udata.h"

#అన్తర్భావ "udatamem.h"

U_CFUNC శూన్య UData_జ్ఞాపకం_init(UData_జ్ఞాపకం *This) {
    uprv_memset(This, 0, పరిమాణం(UData_జ్ఞాపకం));
    This->length=-1;
}


U_CFUNC శూన్య UDatamemory_కేటాయించు(UData_జ్ఞాపకం *dest, UData_జ్ఞాపకం *ఆధారం) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->కుప్ప_Allocated;
    uprv_memcpy(dest, ఆధారం, పరిమాణం(UData_జ్ఞాపకం));
    dest->కుప్ప_Allocated = mallocedFlag;
}

U_CFUNC UData_జ్ఞాపకం *UData_జ్ఞాపకం_create_New_Instance(UError_సంకేతం *pErr) {
    UData_జ్ఞాపకం *This;

    ఒకవేళ (U_FAILURE(*pErr)) {
        తిరిగి శూన్యసూచిక;
    }
    This = (UData_జ్ఞాపకం *)uprv_malloc(పరిమాణం(UData_జ్ఞాపకం));
    ఒకవేళ (This == శూన్యసూచిక) {
        *pErr = U_జ్ఞాపకం_కేటాయింపు_ERROR; }
    లేకపోతే {
        UData_జ్ఞాపకం_init(This);
        This->కుప్ప_Allocated = నిజం;
    }
    తిరిగి This;
}

U_CFUNC స్థిరరాశి దత్తాంశం_శీర్షిక*
UData_జ్ఞాపకం_normalize_దత్తాంశం_సూచిక(స్థిరరాశి శూన్య* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    స్థిరరాశి దత్తాంశం_శీర్షిక *pdh = (స్థిరరాశి దత్తాంశం_శీర్షిక *)p;
    ఒకవేళ(pdh==శూన్యసూచిక || (pdh->దత్తాంశం_శీర్షిక_2.magic1==0xda && pdh->దత్తాంశం_శీర్షిక_2.magic2==0x27)) {
        తిరిగి pdh;
    } లేకపోతే {
#ఒకవేళ U_PLATFORM == U_PF_OS400
        /*
        TODO: Fix this once the compiler implements this feature. Keep in sync with genccode.c

        This is here because this platform can't currently put
        const data into the read-only pages of an object or
        shared library (service program). Only strings are allowed in read-only
        pages, so we use char * strings to store the data.

        In order to prevent the beginning of the data from ever matching the
        magic numbers we must skip the initial double.
        [grhoten 4/24/2003]
        */
        తిరిగి (స్థిరరాశి దత్తాంశం_శీర్షిక *)*((స్థిరరాశి శూన్య **)p+1);
#లేకపోతే
        తిరిగి (స్థిరరాశి దత్తాంశం_శీర్షిక *)((స్థిరరాశి ద్విగుణ *)p+1);
#అయితే_ముగింపు
    }
}


U_CFUNC శూన్య UData_జ్ఞాపకం_సమితి_దత్తాంశం (UData_జ్ఞాపకం *This, స్థిరరాశి శూన్య *దత్తాంశం_Addr) {
    This->p_శీర్షిక = UData_జ్ఞాపకం_normalize_దత్తాంశం_సూచిక(దత్తాంశం_Addr);
}


U_CAPI శూన్య U_EXPORT2
udata_close(UData_జ్ఞాపకం *p_దత్తాంశం) {
    ఒకవేళ(p_దత్తాంశం!=శూన్యసూచిక) {
        uprv_unmapFile(p_దత్తాంశం);
        ఒకవేళ(p_దత్తాంశం->కుప్ప_Allocated ) {
            uprv_free(p_దత్తాంశం);
        } లేకపోతే {
            UData_జ్ఞాపకం_init(p_దత్తాంశం);
        }
    }
}

U_CAPI స్థిరరాశి శూన్య* U_EXPORT2
udata_get_జ్ఞాపకం(UData_జ్ఞాపకం* p_దత్తాంశం U_LIFETIME_BOUND) {
    ఒకవేళ(p_దత్తాంశం!=శూన్యసూచిక && p_దత్తాంశం->p_శీర్షిక!=శూన్యసూచిక) {
        తిరిగి (అక్షరం *)(p_దత్తాంశం->p_శీర్షిక)+udata_get_శీర్షిక_కొలత(p_దత్తాంశం->p_శీర్షిక);
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}

/**
 * Get the length of the data item if possible.
 * The length may be up to 15 bytes larger than the actual data.
 *
 * TODO Consider making this function public.
 * It would have to return the actual length in more cases.
 * For example, the length of the last item in a .dat package could be
 * computed from the size of the whole .dat package minus the offset of the
 * last item.
 * The size of a file that was directly memory-mapped could be determined
 * using some system API.
 *
 * In order to get perfect values for all data items, we may have to add a
 * length field to UDataInfo, but that complicates data generation
 * and may be overkill.
 *
 * @param pData The data item.
 * @return the length of the data item, or -1 if not known
 * @internal Currently used only in cintltst/udatatst.c
 */
U_CAPI int32_t U_EXPORT2
udata_get_నిడివి(స్థిరరాశి UData_జ్ఞాపకం *p_దత్తాంశం) {
    ఒకవేళ(p_దత్తాంశం!=శూన్యసూచిక && p_దత్తాంశం->p_శీర్షిక!=శూన్యసూచిక && p_దత్తాంశం->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        తిరిగి p_దత్తాంశం->length-udata_get_శీర్షిక_కొలత(p_దత్తాంశం->p_శీర్షిక);
    } లేకపోతే {
        తిరిగి -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI స్థిరరాశి శూన్య* U_EXPORT2
udata_get_Raw_జ్ఞాపకం(స్థిరరాశి UData_జ్ఞాపకం* p_దత్తాంశం U_LIFETIME_BOUND) {
    ఒకవేళ(p_దత్తాంశం!=శూన్యసూచిక && p_దత్తాంశం->p_శీర్షిక!=శూన్యసూచిక) {
        తిరిగి p_దత్తాంశం->p_శీర్షిక;
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}

U_CFUNC UBool UData_జ్ఞాపకం_is_Loaded(స్థిరరాశి UData_జ్ఞాపకం *This) {
    తిరిగి This->p_శీర్షిక != శూన్యసూచిక;
}

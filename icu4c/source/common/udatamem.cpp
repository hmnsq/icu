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

#ضمن "unicode/utypes.h"
#ضمن "cmemory.h"
#ضمن "unicode/udata.h"

#ضمن "udatamem.h"

U_CFUNC فراغ UData_يادگيري_init(UData_يادگيري *This) {
    uprv_memset(This, 0, حجم(UData_يادگيري));
    This->length=-1;
}


U_CFUNC فراغ UDatamemory_سونپيو(UData_يادگيري *dest, UData_يادگيري *ذريعو) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->ڍير_Allocated;
    uprv_memcpy(dest, ذريعو, حجم(UData_يادگيري));
    dest->ڍير_Allocated = mallocedFlag;
}

U_CFUNC UData_يادگيري *UData_يادگيري_create_New_Instance(UError_ڪوڊ *pErr) {
    UData_يادگيري *This;

    إذا (U_FAILURE(*pErr)) {
        أرجع خالي_اشارو;
    }
    This = (UData_يادگيري *)uprv_malloc(حجم(UData_يادگيري));
    إذا (This == خالي_اشارو) {
        *pErr = U_يادگيري_ورهاست_ERROR; }
    وإلا {
        UData_يادگيري_init(This);
        This->ڍير_Allocated = سچ;
    }
    أرجع This;
}

U_CFUNC ثابت معلومات_سرو*
UData_يادگيري_normalize_معلومات_اشارو(ثابت فراغ* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ثابت معلومات_سرو *pdh = (ثابت معلومات_سرو *)p;
    إذا(pdh==خالي_اشارو || (pdh->معلومات_سرو_2.magic1==0xda && pdh->معلومات_سرو_2.magic2==0x27)) {
        أرجع pdh;
    } وإلا {
#إذا U_PLATFORM == U_PF_OS400
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
        أرجع (ثابت معلومات_سرو *)*((ثابت فراغ **)p+1);
#وإلا
        أرجع (ثابت معلومات_سرو *)((ثابت مزدوج *)p+1);
#جيڪڏهن_پورو
    }
}


U_CFUNC فراغ UData_يادگيري_سيٽ_معلومات (UData_يادگيري *This, ثابت فراغ *معلومات_Addr) {
    This->p_سرو = UData_يادگيري_normalize_معلومات_اشارو(معلومات_Addr);
}


U_CAPI فراغ U_EXPORT2
udata_close(UData_يادگيري *p_معلومات) {
    إذا(p_معلومات!=خالي_اشارو) {
        uprv_unmapFile(p_معلومات);
        إذا(p_معلومات->ڍير_Allocated ) {
            uprv_free(p_معلومات);
        } وإلا {
            UData_يادگيري_init(p_معلومات);
        }
    }
}

U_CAPI ثابت فراغ* U_EXPORT2
udata_get_يادگيري(UData_يادگيري* p_معلومات U_LIFETIME_BOUND) {
    إذا(p_معلومات!=خالي_اشارو && p_معلومات->p_سرو!=خالي_اشارو) {
        أرجع (حرف *)(p_معلومات->p_سرو)+udata_get_سرو_سائيز(p_معلومات->p_سرو);
    } وإلا {
        أرجع خالي_اشارو;
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
udata_get_ڊيگهائي(ثابت UData_يادگيري *p_معلومات) {
    إذا(p_معلومات!=خالي_اشارو && p_معلومات->p_سرو!=خالي_اشارو && p_معلومات->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        أرجع p_معلومات->length-udata_get_سرو_سائيز(p_معلومات->p_سرو);
    } وإلا {
        أرجع -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI ثابت فراغ* U_EXPORT2
udata_get_Raw_يادگيري(ثابت UData_يادگيري* p_معلومات U_LIFETIME_BOUND) {
    إذا(p_معلومات!=خالي_اشارو && p_معلومات->p_سرو!=خالي_اشارو) {
        أرجع p_معلومات->p_سرو;
    } وإلا {
        أرجع خالي_اشارو;
    }
}

U_CFUNC UBool UData_يادگيري_is_Loaded(ثابت UData_يادگيري *This) {
    أرجع This->p_سرو != خالي_اشارو;
}

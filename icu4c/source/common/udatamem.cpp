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

U_CFUNC فراغ UData_حافظه_init(UData_حافظه *This) {
    uprv_memset(This, 0, حجم(UData_حافظه));
    This->length=-1;
}


U_CFUNC فراغ UDatamemory_تخصیص_بده(UData_حافظه *dest, UData_حافظه *منبع) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->انباشته_Allocated;
    uprv_memcpy(dest, منبع, حجم(UData_حافظه));
    dest->انباشته_Allocated = mallocedFlag;
}

U_CFUNC UData_حافظه *UData_حافظه_create_New_Instance(UError_کد *pErr) {
    UData_حافظه *This;

    إذا (U_FAILURE(*pErr)) {
        أرجع اشاره_تهی;
    }
    This = (UData_حافظه *)uprv_malloc(حجم(UData_حافظه));
    إذا (This == اشاره_تهی) {
        *pErr = U_حافظه_تخصیص_ERROR; }
    وإلا {
        UData_حافظه_init(This);
        This->انباشته_Allocated = درست;
    }
    أرجع This;
}

U_CFUNC ثابت داده_سرآیند*
UData_حافظه_normalize_داده_اشاره_گر(ثابت فراغ* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ثابت داده_سرآیند *pdh = (ثابت داده_سرآیند *)p;
    إذا(pdh==اشاره_تهی || (pdh->داده_سرآیند_2.magic1==0xda && pdh->داده_سرآیند_2.magic2==0x27)) {
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
        أرجع (ثابت داده_سرآیند *)*((ثابت فراغ **)p+1);
#وإلا
        أرجع (ثابت داده_سرآیند *)((ثابت مزدوج *)p+1);
#پایان_اگر
    }
}


U_CFUNC فراغ UData_حافظه_مجموعه_داده (UData_حافظه *This, ثابت فراغ *داده_Addr) {
    This->p_سرآیند = UData_حافظه_normalize_داده_اشاره_گر(داده_Addr);
}


U_CAPI فراغ U_EXPORT2
udata_close(UData_حافظه *p_داده) {
    إذا(p_داده!=اشاره_تهی) {
        uprv_unmapFile(p_داده);
        إذا(p_داده->انباشته_Allocated ) {
            uprv_free(p_داده);
        } وإلا {
            UData_حافظه_init(p_داده);
        }
    }
}

U_CAPI ثابت فراغ* U_EXPORT2
udata_get_حافظه(UData_حافظه* p_داده U_LIFETIME_BOUND) {
    إذا(p_داده!=اشاره_تهی && p_داده->p_سرآیند!=اشاره_تهی) {
        أرجع (حرف *)(p_داده->p_سرآیند)+udata_get_سرآیند_اندازه(p_داده->p_سرآیند);
    } وإلا {
        أرجع اشاره_تهی;
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
udata_get_درازی(ثابت UData_حافظه *p_داده) {
    إذا(p_داده!=اشاره_تهی && p_داده->p_سرآیند!=اشاره_تهی && p_داده->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        أرجع p_داده->length-udata_get_سرآیند_اندازه(p_داده->p_سرآیند);
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
udata_get_Raw_حافظه(ثابت UData_حافظه* p_داده U_LIFETIME_BOUND) {
    إذا(p_داده!=اشاره_تهی && p_داده->p_سرآیند!=اشاره_تهی) {
        أرجع p_داده->p_سرآیند;
    } وإلا {
        أرجع اشاره_تهی;
    }
}

U_CFUNC UBool UData_حافظه_is_Loaded(ثابت UData_حافظه *This) {
    أرجع This->p_سرآیند != اشاره_تهی;
}

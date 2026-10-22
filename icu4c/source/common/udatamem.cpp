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


U_CFUNC فراغ UDatamemory_ټاکل(UData_حافظه *dest, UData_حافظه *سرچینه) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->ډېرۍ_Allocated;
    uprv_memcpy(dest, سرچینه, حجم(UData_حافظه));
    dest->ډېرۍ_Allocated = mallocedFlag;
}

U_CFUNC UData_حافظه *UData_حافظه_create_New_Instance(UError_کوډ *pErr) {
    UData_حافظه *This;

    إذا (U_FAILURE(*pErr)) {
        أرجع تش_اشاره;
    }
    This = (UData_حافظه *)uprv_malloc(حجم(UData_حافظه));
    إذا (This == تش_اشاره) {
        *pErr = U_حافظه_ویش_ERROR; }
    وإلا {
        UData_حافظه_init(This);
        This->ډېرۍ_Allocated = ریښتیا;
    }
    أرجع This;
}

U_CFUNC ثابت معلومات_سرلیک*
UData_حافظه_normalize_معلومات_اشاره_ګر(ثابت فراغ* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ثابت معلومات_سرلیک *pdh = (ثابت معلومات_سرلیک *)p;
    إذا(pdh==تش_اشاره || (pdh->معلومات_سرلیک_2.magic1==0xda && pdh->معلومات_سرلیک_2.magic2==0x27)) {
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
        أرجع (ثابت معلومات_سرلیک *)*((ثابت فراغ **)p+1);
#وإلا
        أرجع (ثابت معلومات_سرلیک *)((ثابت مزدوج *)p+1);
#د_که_پای
    }
}


U_CFUNC فراغ UData_حافظه_سېټ_معلومات (UData_حافظه *This, ثابت فراغ *معلومات_Addr) {
    This->p_سرلیک = UData_حافظه_normalize_معلومات_اشاره_ګر(معلومات_Addr);
}


U_CAPI فراغ U_EXPORT2
udata_close(UData_حافظه *p_معلومات) {
    إذا(p_معلومات!=تش_اشاره) {
        uprv_unmapFile(p_معلومات);
        إذا(p_معلومات->ډېرۍ_Allocated ) {
            uprv_free(p_معلومات);
        } وإلا {
            UData_حافظه_init(p_معلومات);
        }
    }
}

U_CAPI ثابت فراغ* U_EXPORT2
udata_get_حافظه(UData_حافظه* p_معلومات U_LIFETIME_BOUND) {
    إذا(p_معلومات!=تش_اشاره && p_معلومات->p_سرلیک!=تش_اشاره) {
        أرجع (حرف *)(p_معلومات->p_سرلیک)+udata_get_سرلیک_اندازه(p_معلومات->p_سرلیک);
    } وإلا {
        أرجع تش_اشاره;
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
udata_get_اوږدتیا(ثابت UData_حافظه *p_معلومات) {
    إذا(p_معلومات!=تش_اشاره && p_معلومات->p_سرلیک!=تش_اشاره && p_معلومات->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        أرجع p_معلومات->length-udata_get_سرلیک_اندازه(p_معلومات->p_سرلیک);
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
udata_get_Raw_حافظه(ثابت UData_حافظه* p_معلومات U_LIFETIME_BOUND) {
    إذا(p_معلومات!=تش_اشاره && p_معلومات->p_سرلیک!=تش_اشاره) {
        أرجع p_معلومات->p_سرلیک;
    } وإلا {
        أرجع تش_اشاره;
    }
}

U_CFUNC UBool UData_حافظه_is_Loaded(ثابت UData_حافظه *This) {
    أرجع This->p_سرلیک != تش_اشاره;
}

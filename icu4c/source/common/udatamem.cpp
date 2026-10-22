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

U_CFUNC فراغ UData_حافظہ_init(UData_حافظہ *This) {
    uprv_memset(This, 0, حجم(UData_حافظہ));
    This->length=-1;
}


U_CFUNC فراغ UDatamemory_سونپو(UData_حافظہ *dest, UData_حافظہ *سروت) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->ڈھیر_Allocated;
    uprv_memcpy(dest, سروت, حجم(UData_حافظہ));
    dest->ڈھیر_Allocated = mallocedFlag;
}

U_CFUNC UData_حافظہ *UData_حافظہ_create_New_Instance(UError_کوڈ *pErr) {
    UData_حافظہ *This;

    إذا (U_FAILURE(*pErr)) {
        أرجع خالی_سوچک;
    }
    This = (UData_حافظہ *)uprv_malloc(حجم(UData_حافظہ));
    إذا (This == خالی_سوچک) {
        *pErr = U_حافظہ_ونڈ_ERROR; }
    وإلا {
        UData_حافظہ_init(This);
        This->ڈھیر_Allocated = سچ;
    }
    أرجع This;
}

U_CFUNC ثابت انکڑے_سرخی*
UData_حافظہ_normalize_انکڑے_اشارہ_گر(ثابت فراغ* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ثابت انکڑے_سرخی *pdh = (ثابت انکڑے_سرخی *)p;
    إذا(pdh==خالی_سوچک || (pdh->انکڑے_سرخی_2.magic1==0xda && pdh->انکڑے_سرخی_2.magic2==0x27)) {
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
        أرجع (ثابت انکڑے_سرخی *)*((ثابت فراغ **)p+1);
#وإلا
        أرجع (ثابت انکڑے_سرخی *)((ثابت مزدوج *)p+1);
#جے_انت
    }
}


U_CFUNC فراغ UData_حافظہ_سیٹ_انکڑے (UData_حافظہ *This, ثابت فراغ *انکڑے_Addr) {
    This->p_سرخی = UData_حافظہ_normalize_انکڑے_اشارہ_گر(انکڑے_Addr);
}


U_CAPI فراغ U_EXPORT2
udata_close(UData_حافظہ *p_انکڑے) {
    إذا(p_انکڑے!=خالی_سوچک) {
        uprv_unmapFile(p_انکڑے);
        إذا(p_انکڑے->ڈھیر_Allocated ) {
            uprv_free(p_انکڑے);
        } وإلا {
            UData_حافظہ_init(p_انکڑے);
        }
    }
}

U_CAPI ثابت فراغ* U_EXPORT2
udata_get_حافظہ(UData_حافظہ* p_انکڑے U_LIFETIME_BOUND) {
    إذا(p_انکڑے!=خالی_سوچک && p_انکڑے->p_سرخی!=خالی_سوچک) {
        أرجع (حرف *)(p_انکڑے->p_سرخی)+udata_get_سرخی_ناپ(p_انکڑے->p_سرخی);
    } وإلا {
        أرجع خالی_سوچک;
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
udata_get_لمائی(ثابت UData_حافظہ *p_انکڑے) {
    إذا(p_انکڑے!=خالی_سوچک && p_انکڑے->p_سرخی!=خالی_سوچک && p_انکڑے->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        أرجع p_انکڑے->length-udata_get_سرخی_ناپ(p_انکڑے->p_سرخی);
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
udata_get_Raw_حافظہ(ثابت UData_حافظہ* p_انکڑے U_LIFETIME_BOUND) {
    إذا(p_انکڑے!=خالی_سوچک && p_انکڑے->p_سرخی!=خالی_سوچک) {
        أرجع p_انکڑے->p_سرخی;
    } وإلا {
        أرجع خالی_سوچک;
    }
}

U_CFUNC UBool UData_حافظہ_is_Loaded(ثابت UData_حافظہ *This) {
    أرجع This->p_سرخی != خالی_سوچک;
}

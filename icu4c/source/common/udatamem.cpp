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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "udatamem.h"

U_CFUNC ਖਾਲੀ UData_ਸਮ੍ਰਿਤੀ_init(UData_ਸਮ੍ਰਿਤੀ *This) {
    uprv_memset(This, 0, ਆਕਾਰ(UData_ਸਮ੍ਰਿਤੀ));
    This->length=-1;
}


U_CFUNC ਖਾਲੀ UDatamemory_ਸੌਂਪੋ(UData_ਸਮ੍ਰਿਤੀ *dest, UData_ਸਮ੍ਰਿਤੀ *ਸਰੋਤ) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->ਢੇਰ_Allocated;
    uprv_memcpy(dest, ਸਰੋਤ, ਆਕਾਰ(UData_ਸਮ੍ਰਿਤੀ));
    dest->ਢੇਰ_Allocated = mallocedFlag;
}

U_CFUNC UData_ਸਮ੍ਰਿਤੀ *UData_ਸਮ੍ਰਿਤੀ_create_New_Instance(UError_ਕੋਡ *pErr) {
    UData_ਸਮ੍ਰਿਤੀ *This;

    ਜੇ (U_FAILURE(*pErr)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    This = (UData_ਸਮ੍ਰਿਤੀ *)uprv_malloc(ਆਕਾਰ(UData_ਸਮ੍ਰਿਤੀ));
    ਜੇ (This == ਖਾਲੀ_ਸੂਚਕ) {
        *pErr = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR; }
    ਨਹੀਂਤਾਂ {
        UData_ਸਮ੍ਰਿਤੀ_init(This);
        This->ਢੇਰ_Allocated = ਸੱਚ;
    }
    ਵਾਪਸ This;
}

U_CFUNC ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ*
UData_ਸਮ੍ਰਿਤੀ_normalize_ਅੰਕੜੇ_ਸੂਚਕ(ਅਚਲ ਖਾਲੀ* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *pdh = (ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)p;
    ਜੇ(pdh==ਖਾਲੀ_ਸੂਚਕ || (pdh->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic1==0xda && pdh->ਅੰਕੜੇ_ਸਿਰਲੇਖ_2.magic2==0x27)) {
        ਵਾਪਸ pdh;
    } ਨਹੀਂਤਾਂ {
#ਜੇ U_PLATFORM == U_PF_OS400
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
        ਵਾਪਸ (ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)*((ਅਚਲ ਖਾਲੀ **)p+1);
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ (ਅਚਲ ਅੰਕੜੇ_ਸਿਰਲੇਖ *)((ਅਚਲ ਦੁੱਗਣਾ *)p+1);
#ਜੇ_ਅੰਤ
    }
}


U_CFUNC ਖਾਲੀ UData_ਸਮ੍ਰਿਤੀ_ਸੈੱਟ_ਅੰਕੜੇ (UData_ਸਮ੍ਰਿਤੀ *This, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ_Addr) {
    This->p_ਸਿਰਲੇਖ = UData_ਸਮ੍ਰਿਤੀ_normalize_ਅੰਕੜੇ_ਸੂਚਕ(ਅੰਕੜੇ_Addr);
}


U_CAPI ਖਾਲੀ U_EXPORT2
udata_close(UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ) {
    ਜੇ(p_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ) {
        uprv_unmapFile(p_ਅੰਕੜੇ);
        ਜੇ(p_ਅੰਕੜੇ->ਢੇਰ_Allocated ) {
            uprv_free(p_ਅੰਕੜੇ);
        } ਨਹੀਂਤਾਂ {
            UData_ਸਮ੍ਰਿਤੀ_init(p_ਅੰਕੜੇ);
        }
    }
}

U_CAPI ਅਚਲ ਖਾਲੀ* U_EXPORT2
udata_get_ਸਮ੍ਰਿਤੀ(UData_ਸਮ੍ਰਿਤੀ* p_ਅੰਕੜੇ U_LIFETIME_BOUND) {
    ਜੇ(p_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ && p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ (ਅੱਖਰ *)(p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ)+udata_get_ਸਿਰਲੇਖ_ਮਾਪ(p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
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
udata_get_ਲੰਮਾਈ(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *p_ਅੰਕੜੇ) {
    ਜੇ(p_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ && p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ!=ਖਾਲੀ_ਸੂਚਕ && p_ਅੰਕੜੇ->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        ਵਾਪਸ p_ਅੰਕੜੇ->length-udata_get_ਸਿਰਲੇਖ_ਮਾਪ(p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI ਅਚਲ ਖਾਲੀ* U_EXPORT2
udata_get_Raw_ਸਮ੍ਰਿਤੀ(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ* p_ਅੰਕੜੇ U_LIFETIME_BOUND) {
    ਜੇ(p_ਅੰਕੜੇ!=ਖਾਲੀ_ਸੂਚਕ && p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ p_ਅੰਕੜੇ->p_ਸਿਰਲੇਖ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CFUNC UBool UData_ਸਮ੍ਰਿਤੀ_is_Loaded(ਅਚਲ UData_ਸਮ੍ਰਿਤੀ *This) {
    ਵਾਪਸ This->p_ਸਿਰਲੇਖ != ਖਾਲੀ_ਸੂਚਕ;
}

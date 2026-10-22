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

#شامل "unicode/utypes.h"
#شامل "cmemory.h"
#شامل "unicode/udata.h"

#شامل "udatamem.h"

U_CFUNC خالی UData_حافظہ_init(UData_حافظہ *This) {
    uprv_memset(This, 0, حجم(UData_حافظہ));
    This->length=-1;
}


U_CFUNC خالی UDatamemory_تفویض(UData_حافظہ *dest, UData_حافظہ *ماخذ) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->ڈھیر_Allocated;
    uprv_memcpy(dest, ماخذ, حجم(UData_حافظہ));
    dest->ڈھیر_Allocated = mallocedFlag;
}

U_CFUNC UData_حافظہ *UData_حافظہ_create_New_Instance(UError_رمز *pErr) {
    UData_حافظہ *This;

    اگر (U_FAILURE(*pErr)) {
        واپس خالی_اشاریہ;
    }
    This = (UData_حافظہ *)uprv_malloc(حجم(UData_حافظہ));
    اگر (This == خالی_اشاریہ) {
        *pErr = U_حافظہ_تخصیص_ERROR; }
    ورنہ {
        UData_حافظہ_init(This);
        This->ڈھیر_Allocated = سچ;
    }
    واپس This;
}

U_CFUNC ثابت ڈیٹا_سرخی*
UData_حافظہ_normalize_ڈیٹا_اشارہ_گر(ثابت خالی* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ثابت ڈیٹا_سرخی *pdh = (ثابت ڈیٹا_سرخی *)p;
    اگر(pdh==خالی_اشاریہ || (pdh->ڈیٹا_سرخی_2.magic1==0xda && pdh->ڈیٹا_سرخی_2.magic2==0x27)) {
        واپس pdh;
    } ورنہ {
#اگر U_PLATFORM == U_PF_OS400
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
        واپس (ثابت ڈیٹا_سرخی *)*((ثابت خالی **)p+1);
#ورنہ
        واپس (ثابت ڈیٹا_سرخی *)((ثابت دوہرا *)p+1);
#اگر_ختم
    }
}


U_CFUNC خالی UData_حافظہ_سیٹ_ڈیٹا (UData_حافظہ *This, ثابت خالی *ڈیٹا_Addr) {
    This->p_سرخی = UData_حافظہ_normalize_ڈیٹا_اشارہ_گر(ڈیٹا_Addr);
}


U_CAPI خالی U_EXPORT2
udata_close(UData_حافظہ *p_ڈیٹا) {
    اگر(p_ڈیٹا!=خالی_اشاریہ) {
        uprv_unmapFile(p_ڈیٹا);
        اگر(p_ڈیٹا->ڈھیر_Allocated ) {
            uprv_free(p_ڈیٹا);
        } ورنہ {
            UData_حافظہ_init(p_ڈیٹا);
        }
    }
}

U_CAPI ثابت خالی* U_EXPORT2
udata_get_حافظہ(UData_حافظہ* p_ڈیٹا U_LIFETIME_BOUND) {
    اگر(p_ڈیٹا!=خالی_اشاریہ && p_ڈیٹا->p_سرخی!=خالی_اشاریہ) {
        واپس (حرف *)(p_ڈیٹا->p_سرخی)+udata_get_سرخی_سائز(p_ڈیٹا->p_سرخی);
    } ورنہ {
        واپس خالی_اشاریہ;
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
udata_get_لمبائی(ثابت UData_حافظہ *p_ڈیٹا) {
    اگر(p_ڈیٹا!=خالی_اشاریہ && p_ڈیٹا->p_سرخی!=خالی_اشاریہ && p_ڈیٹا->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        واپس p_ڈیٹا->length-udata_get_سرخی_سائز(p_ڈیٹا->p_سرخی);
    } ورنہ {
        واپس -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI ثابت خالی* U_EXPORT2
udata_get_Raw_حافظہ(ثابت UData_حافظہ* p_ڈیٹا U_LIFETIME_BOUND) {
    اگر(p_ڈیٹا!=خالی_اشاریہ && p_ڈیٹا->p_سرخی!=خالی_اشاریہ) {
        واپس p_ڈیٹا->p_سرخی;
    } ورنہ {
        واپس خالی_اشاریہ;
    }
}

U_CFUNC UBool UData_حافظہ_is_Loaded(ثابت UData_حافظہ *This) {
    واپس This->p_سرخی != خالی_اشاریہ;
}

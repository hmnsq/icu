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

U_CFUNC فراغ UDataMemory_init(UDataMemory *This) {
    uprv_memset(This, 0, حجم(UDataMemory));
    This->length=-1;
}


U_CFUNC فراغ UDatamemory_assign(UDataMemory *dest, UDataMemory *source) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->heapAllocated;
    uprv_memcpy(dest, source, حجم(UDataMemory));
    dest->heapAllocated = mallocedFlag;
}

U_CFUNC UDataMemory *UDataMemory_createNewInstance(UErrorCode *pErr) {
    UDataMemory *This;

    إذا (U_FAILURE(*pErr)) {
        أرجع nullptr;
    }
    This = (UDataMemory *)uprv_malloc(حجم(UDataMemory));
    إذا (This == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR; }
    وإلا {
        UDataMemory_init(This);
        This->heapAllocated = true;
    }
    أرجع This;
}

U_CFUNC ثابت DataHeader*
UDataMemory_normalizeDataPointer(ثابت فراغ* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ثابت DataHeader *pdh = (ثابت DataHeader *)p;
    إذا(pdh==nullptr || (pdh->dataHeader.magic1==0xda && pdh->dataHeader.magic2==0x27)) {
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
        أرجع (ثابت DataHeader *)*((ثابت فراغ **)p+1);
#وإلا
        أرجع (ثابت DataHeader *)((ثابت مزدوج *)p+1);
#endif
    }
}


U_CFUNC فراغ UDataMemory_setData (UDataMemory *This, ثابت فراغ *dataAddr) {
    This->pHeader = UDataMemory_normalizeDataPointer(dataAddr);
}


U_CAPI فراغ U_EXPORT2
udata_close(UDataMemory *pData) {
    إذا(pData!=nullptr) {
        uprv_unmapFile(pData);
        إذا(pData->heapAllocated ) {
            uprv_free(pData);
        } وإلا {
            UDataMemory_init(pData);
        }
    }
}

U_CAPI ثابت فراغ* U_EXPORT2
udata_getMemory(UDataMemory* pData U_LIFETIME_BOUND) {
    إذا(pData!=nullptr && pData->pHeader!=nullptr) {
        أرجع (حرف *)(pData->pHeader)+udata_getHeaderSize(pData->pHeader);
    } وإلا {
        أرجع nullptr;
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
udata_getLength(ثابت UDataMemory *pData) {
    إذا(pData!=nullptr && pData->pHeader!=nullptr && pData->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        أرجع pData->length-udata_getHeaderSize(pData->pHeader);
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
udata_getRawMemory(ثابت UDataMemory* pData U_LIFETIME_BOUND) {
    إذا(pData!=nullptr && pData->pHeader!=nullptr) {
        أرجع pData->pHeader;
    } وإلا {
        أرجع nullptr;
    }
}

U_CFUNC UBool UDataMemory_isLoaded(ثابت UDataMemory *This) {
    أرجع This->pHeader != nullptr;
}

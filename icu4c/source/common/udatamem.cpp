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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"

#ଅନ୍ତର୍ଭାଵ "udatamem.h"

U_CFUNC ଶୂନ୍ୟ UDataMemory_init(UDataMemory *This) {
    uprv_memset(This, 0, ଆକାର(UDataMemory));
    This->length=-1;
}


U_CFUNC ଶୂନ୍ୟ UDatamemory_assign(UDataMemory *dest, UDataMemory *source) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->heapAllocated;
    uprv_memcpy(dest, source, ଆକାର(UDataMemory));
    dest->heapAllocated = mallocedFlag;
}

U_CFUNC UDataMemory *UDataMemory_createNewInstance(UErrorCode *pErr) {
    UDataMemory *This;

    ଯଦି (U_FAILURE(*pErr)) {
        ଫେରନ୍ତୁ nullptr;
    }
    This = (UDataMemory *)uprv_malloc(ଆକାର(UDataMemory));
    ଯଦି (This == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR; }
    ଅନ୍ୟଥା {
        UDataMemory_init(This);
        This->heapAllocated = ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ This;
}

U_CFUNC ଅଚଳ DataHeader*
UDataMemory_normalizeDataPointer(ଅଚଳ ଶୂନ୍ୟ* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ଅଚଳ DataHeader *pdh = (ଅଚଳ DataHeader *)p;
    ଯଦି(pdh==nullptr || (pdh->dataHeader.magic1==0xda && pdh->dataHeader.magic2==0x27)) {
        ଫେରନ୍ତୁ pdh;
    } ଅନ୍ୟଥା {
#ଯଦି U_PLATFORM == U_PF_OS400
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
        ଫେରନ୍ତୁ (ଅଚଳ DataHeader *)*((ଅଚଳ ଶୂନ୍ୟ **)p+1);
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ (ଅଚଳ DataHeader *)((ଅଚଳ ଦ୍ୱିଗୁଣ *)p+1);
#endif
    }
}


U_CFUNC ଶୂନ୍ୟ UDataMemory_setData (UDataMemory *This, ଅଚଳ ଶୂନ୍ୟ *dataAddr) {
    This->pHeader = UDataMemory_normalizeDataPointer(dataAddr);
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2
udata_close(UDataMemory *pData) {
    ଯଦି(pData!=nullptr) {
        uprv_unmapFile(pData);
        ଯଦି(pData->heapAllocated ) {
            uprv_free(pData);
        } ଅନ୍ୟଥା {
            UDataMemory_init(pData);
        }
    }
}

U_CAPI ଅଚଳ ଶୂନ୍ୟ* U_EXPORT2
udata_getMemory(UDataMemory* pData U_LIFETIME_BOUND) {
    ଯଦି(pData!=nullptr && pData->pHeader!=nullptr) {
        ଫେରନ୍ତୁ (ଅକ୍ଷର *)(pData->pHeader)+udata_getHeaderSize(pData->pHeader);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nullptr;
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
udata_getLength(ଅଚଳ UDataMemory *pData) {
    ଯଦି(pData!=nullptr && pData->pHeader!=nullptr && pData->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        ଫେରନ୍ତୁ pData->length-udata_getHeaderSize(pData->pHeader);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI ଅଚଳ ଶୂନ୍ୟ* U_EXPORT2
udata_getRawMemory(ଅଚଳ UDataMemory* pData U_LIFETIME_BOUND) {
    ଯଦି(pData!=nullptr && pData->pHeader!=nullptr) {
        ଫେରନ୍ତୁ pData->pHeader;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nullptr;
    }
}

U_CFUNC UBool UDataMemory_isLoaded(ଅଚଳ UDataMemory *This) {
    ଫେରନ୍ତୁ This->pHeader != nullptr;
}

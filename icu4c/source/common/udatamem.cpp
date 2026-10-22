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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"

#ಅನ್ತರ್ಭಾವ "udatamem.h"

U_CFUNC ಶೂನ್ಯ UDataMemory_init(UDataMemory *This) {
    uprv_memset(This, 0, ಪರಿಮಾಣ(UDataMemory));
    This->length=-1;
}


U_CFUNC ಶೂನ್ಯ UDatamemory_assign(UDataMemory *dest, UDataMemory *source) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->heapAllocated;
    uprv_memcpy(dest, source, ಪರಿಮಾಣ(UDataMemory));
    dest->heapAllocated = mallocedFlag;
}

U_CFUNC UDataMemory *UDataMemory_createNewInstance(UErrorCode *pErr) {
    UDataMemory *This;

    ಯದಿ (U_FAILURE(*pErr)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    This = (UDataMemory *)uprv_malloc(ಪರಿಮಾಣ(UDataMemory));
    ಯದಿ (This == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR; }
    ಅನ್ಯಥಾ {
        UDataMemory_init(This);
        This->heapAllocated = ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ This;
}

U_CFUNC ಧ್ರುವ DataHeader*
UDataMemory_normalizeDataPointer(ಧ್ರುವ ಶೂನ್ಯ* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ಧ್ರುವ DataHeader *pdh = (ಧ್ರುವ DataHeader *)p;
    ಯದಿ(pdh==nullptr || (pdh->dataHeader.magic1==0xda && pdh->dataHeader.magic2==0x27)) {
        ಪ್ರತ್ಯಾಗಮ pdh;
    } ಅನ್ಯಥಾ {
#ಯದಿ U_PLATFORM == U_PF_OS400
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
        ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ DataHeader *)*((ಧ್ರುವ ಶೂನ್ಯ **)p+1);
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ DataHeader *)((ಧ್ರುವ ದ್ವಿಗುಣ *)p+1);
#endif
    }
}


U_CFUNC ಶೂನ್ಯ UDataMemory_setData (UDataMemory *This, ಧ್ರುವ ಶೂನ್ಯ *dataAddr) {
    This->pHeader = UDataMemory_normalizeDataPointer(dataAddr);
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
udata_close(UDataMemory *pData) {
    ಯದಿ(pData!=nullptr) {
        uprv_unmapFile(pData);
        ಯದಿ(pData->heapAllocated ) {
            uprv_free(pData);
        } ಅನ್ಯಥಾ {
            UDataMemory_init(pData);
        }
    }
}

U_CAPI ಧ್ರುವ ಶೂನ್ಯ* U_EXPORT2
udata_getMemory(UDataMemory* pData U_LIFETIME_BOUND) {
    ಯದಿ(pData!=nullptr && pData->pHeader!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)(pData->pHeader)+udata_getHeaderSize(pData->pHeader);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ nullptr;
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
udata_getLength(ಧ್ರುವ UDataMemory *pData) {
    ಯದಿ(pData!=nullptr && pData->pHeader!=nullptr && pData->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        ಪ್ರತ್ಯಾಗಮ pData->length-udata_getHeaderSize(pData->pHeader);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI ಧ್ರುವ ಶೂನ್ಯ* U_EXPORT2
udata_getRawMemory(ಧ್ರುವ UDataMemory* pData U_LIFETIME_BOUND) {
    ಯದಿ(pData!=nullptr && pData->pHeader!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ pData->pHeader;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
}

U_CFUNC UBool UDataMemory_isLoaded(ಧ್ರುವ UDataMemory *This) {
    ಪ್ರತ್ಯಾಗಮ This->pHeader != nullptr;
}

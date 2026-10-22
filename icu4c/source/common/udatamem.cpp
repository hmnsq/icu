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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/udata.h"

#अन्तर्भाव "udatamem.h"

U_CFUNC शून्य UDataMemory_init(UDataMemory *This) {
    uprv_memset(This, 0, परिमाण(UDataMemory));
    This->length=-1;
}


U_CFUNC शून्य UDatamemory_assign(UDataMemory *dest, UDataMemory *source) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->heapAllocated;
    uprv_memcpy(dest, source, परिमाण(UDataMemory));
    dest->heapAllocated = mallocedFlag;
}

U_CFUNC UDataMemory *UDataMemory_createNewInstance(UErrorCode *pErr) {
    UDataMemory *This;

    यदि (U_FAILURE(*pErr)) {
        प्रत्यागम nullptr;
    }
    This = (UDataMemory *)uprv_malloc(परिमाण(UDataMemory));
    यदि (This == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR; }
    अन्यथा {
        UDataMemory_init(This);
        This->heapAllocated = सत्य;
    }
    प्रत्यागम This;
}

U_CFUNC ध्रुव DataHeader*
UDataMemory_normalizeDataPointer(ध्रुव शून्य* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ध्रुव DataHeader *pdh = (ध्रुव DataHeader *)p;
    यदि(pdh==nullptr || (pdh->dataHeader.magic1==0xda && pdh->dataHeader.magic2==0x27)) {
        प्रत्यागम pdh;
    } अन्यथा {
#यदि U_PLATFORM == U_PF_OS400
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
        प्रत्यागम (ध्रुव DataHeader *)*((ध्रुव शून्य **)p+1);
#अन्यथा
        प्रत्यागम (ध्रुव DataHeader *)((ध्रुव द्विगुण *)p+1);
#endif
    }
}


U_CFUNC शून्य UDataMemory_setData (UDataMemory *This, ध्रुव शून्य *dataAddr) {
    This->pHeader = UDataMemory_normalizeDataPointer(dataAddr);
}


U_CAPI शून्य U_EXPORT2
udata_close(UDataMemory *pData) {
    यदि(pData!=nullptr) {
        uprv_unmapFile(pData);
        यदि(pData->heapAllocated ) {
            uprv_free(pData);
        } अन्यथा {
            UDataMemory_init(pData);
        }
    }
}

U_CAPI ध्रुव शून्य* U_EXPORT2
udata_getMemory(UDataMemory* pData U_LIFETIME_BOUND) {
    यदि(pData!=nullptr && pData->pHeader!=nullptr) {
        प्रत्यागम (वर्ण *)(pData->pHeader)+udata_getHeaderSize(pData->pHeader);
    } अन्यथा {
        प्रत्यागम nullptr;
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
udata_getLength(ध्रुव UDataMemory *pData) {
    यदि(pData!=nullptr && pData->pHeader!=nullptr && pData->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        प्रत्यागम pData->length-udata_getHeaderSize(pData->pHeader);
    } अन्यथा {
        प्रत्यागम -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI ध्रुव शून्य* U_EXPORT2
udata_getRawMemory(ध्रुव UDataMemory* pData U_LIFETIME_BOUND) {
    यदि(pData!=nullptr && pData->pHeader!=nullptr) {
        प्रत्यागम pData->pHeader;
    } अन्यथा {
        प्रत्यागम nullptr;
    }
}

U_CFUNC UBool UDataMemory_isLoaded(ध्रुव UDataMemory *This) {
    प्रत्यागम This->pHeader != nullptr;
}

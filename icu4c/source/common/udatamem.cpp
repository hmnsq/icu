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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "unicode/udata.h"

#অন্তর্ভাব "udatamem.h"

U_CFUNC শূন্য UDataMemory_init(UDataMemory *This) {
    uprv_memset(This, 0, পরিমাণ(UDataMemory));
    This->length=-1;
}


U_CFUNC শূন্য UDatamemory_assign(UDataMemory *dest, UDataMemory *source) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->heapAllocated;
    uprv_memcpy(dest, source, পরিমাণ(UDataMemory));
    dest->heapAllocated = mallocedFlag;
}

U_CFUNC UDataMemory *UDataMemory_createNewInstance(UErrorCode *pErr) {
    UDataMemory *This;

    যদি (U_FAILURE(*pErr)) {
        প্রত্যাগম nullptr;
    }
    This = (UDataMemory *)uprv_malloc(পরিমাণ(UDataMemory));
    যদি (This == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR; }
    অন্যথা {
        UDataMemory_init(This);
        This->heapAllocated = সত্য;
    }
    প্রত্যাগম This;
}

U_CFUNC ধ্রুব DataHeader*
UDataMemory_normalizeDataPointer(ধ্রুব শূন্য* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ধ্রুব DataHeader *pdh = (ধ্রুব DataHeader *)p;
    যদি(pdh==nullptr || (pdh->dataHeader.magic1==0xda && pdh->dataHeader.magic2==0x27)) {
        প্রত্যাগম pdh;
    } অন্যথা {
#যদি U_PLATFORM == U_PF_OS400
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
        প্রত্যাগম (ধ্রুব DataHeader *)*((ধ্রুব শূন্য **)p+1);
#অন্যথা
        প্রত্যাগম (ধ্রুব DataHeader *)((ধ্রুব দ্বিগুণ *)p+1);
#endif
    }
}


U_CFUNC শূন্য UDataMemory_setData (UDataMemory *This, ধ্রুব শূন্য *dataAddr) {
    This->pHeader = UDataMemory_normalizeDataPointer(dataAddr);
}


U_CAPI শূন্য U_EXPORT2
udata_close(UDataMemory *pData) {
    যদি(pData!=nullptr) {
        uprv_unmapFile(pData);
        যদি(pData->heapAllocated ) {
            uprv_free(pData);
        } অন্যথা {
            UDataMemory_init(pData);
        }
    }
}

U_CAPI ধ্রুব শূন্য* U_EXPORT2
udata_getMemory(UDataMemory* pData U_LIFETIME_BOUND) {
    যদি(pData!=nullptr && pData->pHeader!=nullptr) {
        প্রত্যাগম (বর্ণ *)(pData->pHeader)+udata_getHeaderSize(pData->pHeader);
    } অন্যথা {
        প্রত্যাগম nullptr;
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
udata_getLength(ধ্রুব UDataMemory *pData) {
    যদি(pData!=nullptr && pData->pHeader!=nullptr && pData->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        প্রত্যাগম pData->length-udata_getHeaderSize(pData->pHeader);
    } অন্যথা {
        প্রত্যাগম -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI ধ্রুব শূন্য* U_EXPORT2
udata_getRawMemory(ধ্রুব UDataMemory* pData U_LIFETIME_BOUND) {
    যদি(pData!=nullptr && pData->pHeader!=nullptr) {
        প্রত্যাগম pData->pHeader;
    } অন্যথা {
        প্রত্যাগম nullptr;
    }
}

U_CFUNC UBool UDataMemory_isLoaded(ধ্রুব UDataMemory *This) {
    প্রত্যাগম This->pHeader != nullptr;
}

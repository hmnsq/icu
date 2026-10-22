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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "unicode/udata.h"

#അന്തര്ഭാവ "udatamem.h"

U_CFUNC ശൂന്യം UDataMemory_init(UDataMemory *This) {
    uprv_memset(This, 0, വലുപ്പം(UDataMemory));
    This->length=-1;
}


U_CFUNC ശൂന്യം UDatamemory_assign(UDataMemory *dest, UDataMemory *source) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->heapAllocated;
    uprv_memcpy(dest, source, വലുപ്പം(UDataMemory));
    dest->heapAllocated = mallocedFlag;
}

U_CFUNC UDataMemory *UDataMemory_createNewInstance(UErrorCode *pErr) {
    UDataMemory *This;

    എങ്കിൽ (U_FAILURE(*pErr)) {
        തിരികെ nullptr;
    }
    This = (UDataMemory *)uprv_malloc(വലുപ്പം(UDataMemory));
    എങ്കിൽ (This == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR; }
    അല്ലെങ്കിൽ {
        UDataMemory_init(This);
        This->heapAllocated = സത്യം;
    }
    തിരികെ This;
}

U_CFUNC സ്ഥിരം DataHeader*
UDataMemory_normalizeDataPointer(സ്ഥിരം ശൂന്യം* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    സ്ഥിരം DataHeader *pdh = (സ്ഥിരം DataHeader *)p;
    എങ്കിൽ(pdh==nullptr || (pdh->dataHeader.magic1==0xda && pdh->dataHeader.magic2==0x27)) {
        തിരികെ pdh;
    } അല്ലെങ്കിൽ {
#എങ്കിൽ U_PLATFORM == U_PF_OS400
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
        തിരികെ (സ്ഥിരം DataHeader *)*((സ്ഥിരം ശൂന്യം **)p+1);
#അല്ലെങ്കിൽ
        തിരികെ (സ്ഥിരം DataHeader *)((സ്ഥിരം ഇരട്ട *)p+1);
#endif
    }
}


U_CFUNC ശൂന്യം UDataMemory_setData (UDataMemory *This, സ്ഥിരം ശൂന്യം *dataAddr) {
    This->pHeader = UDataMemory_normalizeDataPointer(dataAddr);
}


U_CAPI ശൂന്യം U_EXPORT2
udata_close(UDataMemory *pData) {
    എങ്കിൽ(pData!=nullptr) {
        uprv_unmapFile(pData);
        എങ്കിൽ(pData->heapAllocated ) {
            uprv_free(pData);
        } അല്ലെങ്കിൽ {
            UDataMemory_init(pData);
        }
    }
}

U_CAPI സ്ഥിരം ശൂന്യം* U_EXPORT2
udata_getMemory(UDataMemory* pData U_LIFETIME_BOUND) {
    എങ്കിൽ(pData!=nullptr && pData->pHeader!=nullptr) {
        തിരികെ (അക്ഷരം *)(pData->pHeader)+udata_getHeaderSize(pData->pHeader);
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
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
udata_getLength(സ്ഥിരം UDataMemory *pData) {
    എങ്കിൽ(pData!=nullptr && pData->pHeader!=nullptr && pData->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        തിരികെ pData->length-udata_getHeaderSize(pData->pHeader);
    } അല്ലെങ്കിൽ {
        തിരികെ -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI സ്ഥിരം ശൂന്യം* U_EXPORT2
udata_getRawMemory(സ്ഥിരം UDataMemory* pData U_LIFETIME_BOUND) {
    എങ്കിൽ(pData!=nullptr && pData->pHeader!=nullptr) {
        തിരികെ pData->pHeader;
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
    }
}

U_CFUNC UBool UDataMemory_isLoaded(സ്ഥിരം UDataMemory *This) {
    തിരികെ This->pHeader != nullptr;
}

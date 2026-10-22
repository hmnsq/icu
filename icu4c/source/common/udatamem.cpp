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

#समावेश "unicode/utypes.h"
#समावेश "cmemory.h"
#समावेश "unicode/udata.h"

#समावेश "udatamem.h"

U_CFUNC व्योम UDataMemory_init(UDataMemory *This) {
    uprv_memset(This, 0, माप(UDataMemory));
    This->length=-1;
}


U_CFUNC व्योम UDatamemory_assign(UDataMemory *dest, UDataMemory *source) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->heapAllocated;
    uprv_memcpy(dest, source, माप(UDataMemory));
    dest->heapAllocated = mallocedFlag;
}

U_CFUNC UDataMemory *UDataMemory_createNewInstance(UErrorCode *pErr) {
    UDataMemory *This;

    अगर (U_FAILURE(*pErr)) {
        वापस nullptr;
    }
    This = (UDataMemory *)uprv_malloc(माप(UDataMemory));
    अगर (This == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR; }
    अन्यथा {
        UDataMemory_init(This);
        This->heapAllocated = true;
    }
    वापस This;
}

U_CFUNC स्थिर तथ्य_Header*
UData_Memory_normalize_तथ्य_Pointer(स्थिर व्योम* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    स्थिर तथ्य_Header *pdh = (स्थिर तथ्य_Header *)p;
    अगर(pdh==nullptr || (pdh->तथ्य_Header_2.magic1==0xda && pdh->तथ्य_Header_2.magic2==0x27)) {
        वापस pdh;
    } अन्यथा {
#अगर U_PLATFORM == U_PF_OS400
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
        वापस (स्थिर तथ्य_Header *)*((स्थिर व्योम **)p+1);
#अन्यथा
        वापस (स्थिर तथ्य_Header *)((स्थिर द्विगुन *)p+1);
#पूर्ण_अगर
    }
}


U_CFUNC व्योम UData_Memory_set_तथ्य (UDataMemory *This, स्थिर व्योम *तथ्य_Addr) {
    This->pHeader = UData_Memory_normalize_तथ्य_Pointer(तथ्य_Addr);
}


U_CAPI व्योम U_EXPORT2
udata_close(UDataMemory *p_तथ्य) {
    अगर(p_तथ्य!=nullptr) {
        uprv_unmapFile(p_तथ्य);
        अगर(p_तथ्य->heapAllocated ) {
            uprv_free(p_तथ्य);
        } अन्यथा {
            UDataMemory_init(p_तथ्य);
        }
    }
}

U_CAPI स्थिर व्योम* U_EXPORT2
udata_getMemory(UDataMemory* p_तथ्य U_LIFETIME_BOUND) {
    अगर(p_तथ्य!=nullptr && p_तथ्य->pHeader!=nullptr) {
        वापस (अक्षर *)(p_तथ्य->pHeader)+udata_get_Header_आकार(p_तथ्य->pHeader);
    } अन्यथा {
        वापस nullptr;
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
udata_get_लंबाई(स्थिर UDataMemory *p_तथ्य) {
    अगर(p_तथ्य!=nullptr && p_तथ्य->pHeader!=nullptr && p_तथ्य->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        वापस p_तथ्य->length-udata_get_Header_आकार(p_तथ्य->pHeader);
    } अन्यथा {
        वापस -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI स्थिर व्योम* U_EXPORT2
udata_getRawMemory(स्थिर UDataMemory* p_तथ्य U_LIFETIME_BOUND) {
    अगर(p_तथ्य!=nullptr && p_तथ्य->pHeader!=nullptr) {
        वापस p_तथ्य->pHeader;
    } अन्यथा {
        वापस nullptr;
    }
}

U_CFUNC UBool UDataMemory_isLoaded(स्थिर UDataMemory *This) {
    वापस This->pHeader != nullptr;
}

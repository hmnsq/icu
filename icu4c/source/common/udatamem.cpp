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
    uprv_memset(This, 0, আকার(UDataMemory));
    This->length=-1;
}


U_CFUNC শূন্য UDatamemory_assign(UDataMemory *dest, UDataMemory *source) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->heapAllocated;
    uprv_memcpy(dest, source, আকার(UDataMemory));
    dest->heapAllocated = mallocedFlag;
}

U_CFUNC UDataMemory *UDataMemory_createNewInstance(UErrorCode *pErr) {
    UDataMemory *This;

    যদি (U_FAILURE(*pErr)) {
        ফেরত nullptr;
    }
    This = (UDataMemory *)uprv_malloc(আকার(UDataMemory));
    যদি (This == nullptr) {
        *pErr = U_MEMORY_ALLOCATION_ERROR; }
    অন্যথা {
        UDataMemory_init(This);
        This->heapAllocated = সত্য;
    }
    ফেরত This;
}

U_CFUNC ধ্রুবক তথ্য_Header*
UData_Memory_normalize_তথ্য_Pointer(ধ্রুবক শূন্য* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ধ্রুবক তথ্য_Header *pdh = (ধ্রুবক তথ্য_Header *)p;
    যদি(pdh==nullptr || (pdh->তথ্য_Header_2.magic1==0xda && pdh->তথ্য_Header_2.magic2==0x27)) {
        ফেরত pdh;
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
        ফেরত (ধ্রুবক তথ্য_Header *)*((ধ্রুবক শূন্য **)p+1);
#অন্যথা
        ফেরত (ধ্রুবক তথ্য_Header *)((ধ্রুবক দ্বিগুণ *)p+1);
#endif
    }
}


U_CFUNC শূন্য UData_Memory_set_তথ্য (UDataMemory *This, ধ্রুবক শূন্য *তথ্য_Addr) {
    This->pHeader = UData_Memory_normalize_তথ্য_Pointer(তথ্য_Addr);
}


U_CAPI শূন্য U_EXPORT2
udata_close(UDataMemory *p_তথ্য) {
    যদি(p_তথ্য!=nullptr) {
        uprv_unmapFile(p_তথ্য);
        যদি(p_তথ্য->heapAllocated ) {
            uprv_free(p_তথ্য);
        } অন্যথা {
            UDataMemory_init(p_তথ্য);
        }
    }
}

U_CAPI ধ্রুবক শূন্য* U_EXPORT2
udata_getMemory(UDataMemory* p_তথ্য U_LIFETIME_BOUND) {
    যদি(p_তথ্য!=nullptr && p_তথ্য->pHeader!=nullptr) {
        ফেরত (অক্ষর *)(p_তথ্য->pHeader)+udata_get_Header_আকার(p_তথ্য->pHeader);
    } অন্যথা {
        ফেরত nullptr;
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
udata_get_দৈর্ঘ্য(ধ্রুবক UDataMemory *p_তথ্য) {
    যদি(p_তথ্য!=nullptr && p_তথ্য->pHeader!=nullptr && p_তথ্য->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        ফেরত p_তথ্য->length-udata_get_Header_আকার(p_তথ্য->pHeader);
    } অন্যথা {
        ফেরত -1;
    }
}

/**
 * Get the memory including the data header.
 * Used in cintltst/udatatst.c
 * @internal
 */
U_CAPI ধ্রুবক শূন্য* U_EXPORT2
udata_getRawMemory(ধ্রুবক UDataMemory* p_তথ্য U_LIFETIME_BOUND) {
    যদি(p_তথ্য!=nullptr && p_তথ্য->pHeader!=nullptr) {
        ফেরত p_তথ্য->pHeader;
    } অন্যথা {
        ফেরত nullptr;
    }
}

U_CFUNC UBool UDataMemory_isLoaded(ধ্রুবক UDataMemory *This) {
    ফেরত This->pHeader != nullptr;
}

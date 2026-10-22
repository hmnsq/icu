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

U_CFUNC शून्य UData_स्मृतिः_init(UData_स्मृतिः *This) {
    uprv_memset(This, 0, परिमाण(UData_स्मृतिः));
    This->length=-1;
}


U_CFUNC शून्य UDatamemory_नियोजय(UData_स्मृतिः *dest, UData_स्मृतिः *source) {
    /* UDataMemory Assignment.  Destination UDataMemory must be initialized first.  */
    UBool mallocedFlag = dest->heapAllocated;
    uprv_memcpy(dest, source, परिमाण(UData_स्मृतिः));
    dest->heapAllocated = mallocedFlag;
}

U_CFUNC UData_स्मृतिः *UData_स्मृतिः_create_New_Instance(UError_संहिता *pErr) {
    UData_स्मृतिः *This;

    यदि (U_FAILURE(*pErr)) {
        प्रत्यागम शून्यसूचकः;
    }
    This = (UData_स्मृतिः *)uprv_malloc(परिमाण(UData_स्मृतिः));
    यदि (This == शून्यसूचकः) {
        *pErr = U_स्मृतिः_आवण्टनम्_ERROR; }
    अन्यथा {
        UData_स्मृतिः_init(This);
        This->heapAllocated = सत्य;
    }
    प्रत्यागम This;
}

U_CFUNC ध्रुव दत्त_शीर्षकम्*
UData_स्मृतिः_normalize_दत्त_Pointer(ध्रुव शून्य* p U_LIFETIME_BOUND) {
    /* allow the data to be optionally prepended with an alignment-forcing double value */
    ध्रुव दत्त_शीर्षकम् *pdh = (ध्रुव दत्त_शीर्षकम् *)p;
    यदि(pdh==शून्यसूचकः || (pdh->दत्त_शीर्षकम्_2.magic1==0xda && pdh->दत्त_शीर्षकम्_2.magic2==0x27)) {
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
        प्रत्यागम (ध्रुव दत्त_शीर्षकम् *)*((ध्रुव शून्य **)p+1);
#अन्यथा
        प्रत्यागम (ध्रुव दत्त_शीर्षकम् *)((ध्रुव द्विगुण *)p+1);
#यदि_अन्तः
    }
}


U_CFUNC शून्य UData_स्मृतिः_समुच्चयः_दत्त (UData_स्मृतिः *This, ध्रुव शून्य *दत्त_Addr) {
    This->p_शीर्षकम् = UData_स्मृतिः_normalize_दत्त_Pointer(दत्त_Addr);
}


U_CAPI शून्य U_EXPORT2
udata_close(UData_स्मृतिः *p_दत्त) {
    यदि(p_दत्त!=शून्यसूचकः) {
        uprv_unmapFile(p_दत्त);
        यदि(p_दत्त->heapAllocated ) {
            uprv_free(p_दत्त);
        } अन्यथा {
            UData_स्मृतिः_init(p_दत्त);
        }
    }
}

U_CAPI ध्रुव शून्य* U_EXPORT2
udata_get_स्मृतिः(UData_स्मृतिः* p_दत्त U_LIFETIME_BOUND) {
    यदि(p_दत्त!=शून्यसूचकः && p_दत्त->p_शीर्षकम्!=शून्यसूचकः) {
        प्रत्यागम (वर्ण *)(p_दत्त->p_शीर्षकम्)+udata_get_शीर्षकम्_आकार(p_दत्त->p_शीर्षकम्);
    } अन्यथा {
        प्रत्यागम शून्यसूचकः;
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
udata_get_दीर्घता(ध्रुव UData_स्मृतिः *p_दत्त) {
    यदि(p_दत्त!=शून्यसूचकः && p_दत्त->p_शीर्षकम्!=शून्यसूचकः && p_दत्त->length>=0) {
        /*
         * subtract the header size,
         * return only the size of the actual data starting at udata_getMemory()
         */
        प्रत्यागम p_दत्त->length-udata_get_शीर्षकम्_आकार(p_दत्त->p_शीर्षकम्);
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
udata_get_Raw_स्मृतिः(ध्रुव UData_स्मृतिः* p_दत्त U_LIFETIME_BOUND) {
    यदि(p_दत्त!=शून्यसूचकः && p_दत्त->p_शीर्षकम्!=शून्यसूचकः) {
        प्रत्यागम p_दत्त->p_शीर्षकम्;
    } अन्यथा {
        प्रत्यागम शून्यसूचकः;
    }
}

U_CFUNC UBool UData_स्मृतिः_is_Loaded(ध्रुव UData_स्मृतिः *This) {
    प्रत्यागम This->p_शीर्षकम् != शून्यसूचकः;
}
